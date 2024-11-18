#pragma once
#include <QObject>
#include <QFile>
#include <QString>
#include <libssh/libssh.h>
#include <libssh/sftp.h>
#include <QThread>
#include <QMutex>
#include <atomic>

#include "SessionManager.h"

class SftpFileDownloader : public QObject
{
	Q_OBJECT
public:
	SftpFileDownloader(QObject* parent = nullptr);
	~SftpFileDownloader();

	bool DownloadFile(const QString& remotePath, const QString& localPath);
	void CancelDownload();
signals:
	void DownloadProgress(int progress);
	void DownloadFinished(bool success);
	void DownloadError(QString error);

private:
	std::atomic_bool m_cancel;
	QMutex m_mutex;
	sftp_session SftpSession__;
	void doDownload(const QString& remotePath, const QString& localPath);
};