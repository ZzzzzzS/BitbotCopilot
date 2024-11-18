#include "SftpFileTransport.h"
#include <QMutexLocker>
#include <QTimer>
#include <fcntl.h>
#include <QApplication>
#include <QFileInfo>
#include <QDir>

#define MAX_XFER_BUF_SIZE 16384

SftpFileDownloader::SftpFileDownloader(QObject* parent)
	: QObject(parent),
	SftpSession__(nullptr)
{
	this->m_cancel = false;
}

SftpFileDownloader::~SftpFileDownloader()
{
}

bool SftpFileDownloader::DownloadFile(const QString& remotePath, const QString& localPath)
{
	if (!this->m_mutex.tryLock())
	{
		emit this->DownloadError("Another download is in progress");
		emit this->DownloadFinished(false);
		return false;
	}
	else
	{
		this->m_mutex.unlock();
	}
	QTimer::singleShot(0, this, [=]() { this->doDownload(remotePath, localPath); });
	qApp->processEvents();
	return true;
}

void SftpFileDownloader::CancelDownload()
{
	this->m_cancel = true;
}

void SftpFileDownloader::doDownload(const QString& remotePath, const QString& localPath)
{
	QMutexLocker locker(&m_mutex);
	this->SftpSession__ = zzs::SessionManager::getInstance()->CreateSftpSession();

	if (this->SftpSession__ == nullptr)
	{
		emit this->DownloadError("Failed to connect to the server");
		emit this->DownloadFinished(false);
		return;
	}
	std::string remotePathStr = remotePath.toStdString();
	sftp_file file = sftp_open(this->SftpSession__, remotePathStr.c_str(), O_RDONLY, 0);
	if (file == nullptr)
	{
		zzs::SessionManager::getInstance()->DistorySftpSession(this->SftpSession__);
		emit this->DownloadError("Failed to open the remote file");
		emit this->DownloadFinished(false);
		return;
	}

	// Get the file size
	uint64_t fileSize;
	sftp_attributes attrs = sftp_fstat(file);
	if (attrs == nullptr)
	{
		sftp_close(file);
		zzs::SessionManager::getInstance()->DistorySftpSession(this->SftpSession__);
		emit this->DownloadError("Failed to get the file size");
		emit this->DownloadFinished(false);
		return;
	}
	fileSize = attrs->size;
	sftp_attributes_free(attrs);

	//mkdir
	QFileInfo fileInfo(localPath);
	QDir dir = fileInfo.dir();
	if (!dir.exists())
	{
		dir.mkpath(".");
	}

	QFile localFile(localPath);
	if (!localFile.open(QIODevice::WriteOnly))
	{
		sftp_close(file);
		zzs::SessionManager::getInstance()->DistorySftpSession(this->SftpSession__);
		emit this->DownloadError("Failed to open the local file");
		emit this->DownloadFinished(false);
		return;
	}

	char* buffer=new char[MAX_XFER_BUF_SIZE];
	uint64_t totalBytesRead = 0;
	bool success = true;
	while (totalBytesRead < fileSize)
	{
		if (this->m_cancel)
		{
			emit this->DownloadError("Download canceled");
			emit this->DownloadFinished(false);
			success = false;
			break;
		}

		int bytesRead = sftp_read(file, buffer, MAX_XFER_BUF_SIZE);
		if (bytesRead < 0)
		{
			emit this->DownloadError("Failed to read the remote file");
			emit this->DownloadFinished(false);
			success = false;
			break;
		}
		localFile.write(buffer, bytesRead);
		totalBytesRead += bytesRead;
		emit this->DownloadProgress((int)(totalBytesRead * 100 / fileSize));
	}
	localFile.close();
	sftp_close(file);
	zzs::SessionManager::getInstance()->DistorySftpSession(this->SftpSession__);
	this->SftpSession__ = nullptr;
	delete[] buffer;
	if (!success)
		localFile.remove();
	else
		emit this->DownloadFinished(true);

}
