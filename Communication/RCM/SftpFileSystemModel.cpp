#include "SftpFileSystemModel.h"
#include <QIcon>
#include <chrono>

SftpFileSystemModel::SftpFileSystemModel(const QString RootPath, QObject* parent)
	:QAbstractItemModel(parent),
	connected__(false),
	operationInProgress__(false),
	isForwarding__(false),
	isBacking__(false),
	isRefreshing__(false),
	RootPath__(RootPath.toStdString()),
	CurrentPath__(RootPath.toStdString()),
	SftpSession__(nullptr),
	DirIconCache__(QIcon(":/UI/Image/folder.png")),
	FileIconCache__(QIcon(":/UI/Image/file.png"))
{
	qRegisterMetaType<std::string>("std::string");
	QObject::connect(this, &SftpFileSystemModel::TriggerCd, this, &SftpFileSystemModel::doCd);
	QObject::connect(this, &SftpFileSystemModel::TriggerConnect, this, &SftpFileSystemModel::doConnect);
	QObject::connect(this, &SftpFileSystemModel::TriggerDisconnect, this, &SftpFileSystemModel::doDisconnect);
	QObject::connect(this, &SftpFileSystemModel::TriggerRefresh, this, &SftpFileSystemModel::doRefresh);
}

SftpFileSystemModel::~SftpFileSystemModel()
{
	doDisconnect();
}

bool SftpFileSystemModel::connect()
{
	if (this->isOperationInProgress())
		return false;
	emit this->TriggerConnect();
	return true;
}

void SftpFileSystemModel::disconnect()
{
	if (this->isConnected())
	{
		emit this->TriggerDisconnect();
	}
}

QString SftpFileSystemModel::path() const
{
	if (this->isConnected())
		return QString::fromStdString(this->CurrentPath__);
	else
		return QString();
}

bool SftpFileSystemModel::isConnected() const
{
	return this->connected__;
}

bool SftpFileSystemModel::isOperationInProgress() const
{
	return this->operationInProgress__;
}

bool SftpFileSystemModel::cd(const QString& path)
{
	return this->cd(path.toStdString());
}

bool SftpFileSystemModel::cd(const std::string& path)
{
	if (this->isOperationInProgress() || !this->isConnected())
		return false;

	emit this->TriggerCd(path);

	return true;
}

bool SftpFileSystemModel::cdup()
{
	if (!this->isConnected() || this->isOperationInProgress())
		return false;

	std::string path = this->CurrentPath__;
	if (path == this->RootPath__)
		return false;

	// remove the last '/'
	std::string newpath = path.substr(0, path.find_last_of('/'));
	if (newpath.empty())
		newpath = ".";
	emit this->TriggerCd(newpath);

	return true;
}

bool SftpFileSystemModel::Refresh()
{
	if (this->isOperationInProgress())
		return false;

	emit this->TriggerRefresh();

	return true;
}

bool SftpFileSystemModel::isCurrentPathRoot() const
{
	return (this->CurrentPath__ == this->RootPath__);
}

bool SftpFileSystemModel::CanForward() const
{
	return this->ForwardStack__.size() > 0;
}

bool SftpFileSystemModel::CanBackward() const
{
	return this->BackwardStack__.size() > 0;
}

bool SftpFileSystemModel::Forward()
{
	if (!this->isConnected() || this->isOperationInProgress())
		return false;
	if (this->ForwardStack__.empty())
		return false;

	std::string path = this->ForwardStack__.top();
	this->isForwarding__ = true;
	emit this->TriggerCd(path);
	return true;
}

bool SftpFileSystemModel::Backward()
{
	if (!this->isConnected() || this->isOperationInProgress())
		return false;
	if (this->BackwardStack__.empty())
		return false;

	std::string path = this->BackwardStack__.top();
	this->isBacking__ = true;
	//qDebug() << "Backward to " << QString::fromStdString(path);
	emit this->TriggerCd(path);
	return true;
}

int SftpFileSystemModel::rowCount(const QModelIndex& parent) const
{
	//qDebug() << "rowCount" << this->DirInfoCache__.size();
	return this->DirInfoCache__.size();
}

int SftpFileSystemModel::columnCount(const QModelIndex& parent) const
{
	if (this->DirInfoCache__.empty())
		return 0;
	return 3;
}

QModelIndex SftpFileSystemModel::index(int row, int column, const QModelIndex& parent) const
{
	if (row < 0 || row >= this->DirInfoCache__.size())
		return QModelIndex();
	if (column < 0 || column >= 3)
		return QModelIndex();
	return this->createIndex(row, column, &this->DirInfoCache__[row]);
}

QModelIndex SftpFileSystemModel::parent(const QModelIndex& child) const
{
	return QModelIndex();
}

QVariant SftpFileSystemModel::data(const QModelIndex& index, int role) const
{
	QMutexLocker locker(&(mutex__));

	if (this->DirInfoCache__.size() == 0)
		return QVariant();

	if (role == Qt::DisplayRole)
	{
		if (index.row() < 0 || index.row() >= this->DirInfoCache__.size())
			return QVariant();

		if (index.column() == 0) // name
		{
			return QString::fromLocal8Bit(this->DirInfoCache__[index.row()].name);
		}
		else if (index.column() == 1) //time
		{
			auto time = this->DirInfoCache__[index.row()].mtime;
			//to date and time
			std::time_t t = static_cast<std::time_t>(time);
			std::tm tm = *std::localtime(&t);
			char buffer[80];
			std::strftime(buffer, 80, "%Y-%m-%d %H:%M:%S", &tm);
			return QString::fromLocal8Bit(buffer);

		}
		else if (index.column() == 2) //size
		{
			uint64_t sz = this->DirInfoCache__[index.row()].size;
			if (sz < 1024)
				return QString::number(sz) + " B";
			else if (sz < 1024 * 1024)
				return QString::number(sz / 1024) + " KB";
			else if (sz < 1024 * 1024 * 1024)
				return QString::number(sz / 1024 / 1024) + " MB";
			else
				return QString::number(sz / 1024 / 1024 / 1024) + " GB";
		}
		else
			return QVariant();
	}
	else if (role == Qt::DecorationRole)
	{
		if (index.row() < 0 || index.row() >= this->DirInfoCache__.size())
			return QVariant();

		if (index.column() == 0)
		{
			if (this->DirInfoCache__[index.row()].type == SSH_FILEXFER_TYPE_DIRECTORY)
				return this->DirIconCache__;
			else
				return this->FileIconCache__;
		}
		else
			return QVariant();
	}
	else if (role == Qt::ToolTipRole)
	{
		if (index.row() < 0 || index.row() >= this->DirInfoCache__.size())
			return QVariant();

		return QString::fromStdString(this->DirInfoCache__[index.row()].longname);
	}
	else
		return QVariant();
}

QVariant SftpFileSystemModel::headerData(int section, Qt::Orientation orientation, int role) const
{
	if (orientation == Qt::Orientation::Vertical)
		return QVariant();
	if (role == Qt::DisplayRole)
	{
		if (section == 0)
			return QString(tr("Name"));
		else if (section == 1)
			return QString(tr("Date Modified"));
		else if (section == 2)
			return QString(tr("Size"));
		else
			return QVariant();
	}
	else
		return QVariant();
}

void SftpFileSystemModel::setOperationInProgress(bool inProgress)
{
	if (this->operationInProgress__ != inProgress)
	{
		this->operationInProgress__ = inProgress;
		emit this->operationInProgressChanged(inProgress);
	}
}

void SftpFileSystemModel::doConnect()
{
	qDebug() << "doConnect";
	this->setOperationInProgress(true);
	this->SftpSession__ = zzs::SessionManager::getInstance()->CreateSftpSession();
	if (this->SftpSession__ != nullptr)
	{
		this->connected__ = true;
	}
	else
	{
		this->connected__ = false;
		emit this->error(tr("Failed to connect to the server"));
	}
	emit this->connectStateChanged(this->connected__.load());
	this->setOperationInProgress(false);
}

void SftpFileSystemModel::doDisconnect()
{
	this->setOperationInProgress(true);
	if (this->SftpSession__ != nullptr)
		zzs::SessionManager::getInstance()->DistorySftpSession(this->SftpSession__);
	this->SftpSession__ = nullptr;
	this->connected__ = false;
	this->setOperationInProgress(false);
}

void SftpFileSystemModel::doCd(const std::string& path)
{
	if (this->SftpSession__ == nullptr)
		return;

	this->setOperationInProgress(true);

	sftp_dir dir = sftp_opendir(this->SftpSession__, path.c_str());
	if (dir == nullptr)
	{
		QString Error = "Failed to open the directory, code is ";
		Error += QString::number(sftp_get_error(this->SftpSession__));
		emit this->error(Error);
		this->setOperationInProgress(false);
		return;
	}
	//read the directory to cache
	std::vector<sftp_attributes_struct_ex> DirInfoCache;
	while (true)
	{
		sftp_attributes attr;
		attr = sftp_readdir(this->SftpSession__, dir);
		if (attr == nullptr)
		{
			if (sftp_dir_eof(dir))
				break;
			else
			{
				QString Error = tr("Failed to read the directory, code is ");
				Error += QString::number(sftp_get_error(this->SftpSession__));
				emit this->error(Error);
				break;
			}
		}


		//TODO: filter only .csv files
		if (attr->type == SSH_FILEXFER_TYPE_DIRECTORY || attr->type == SSH_FILEXFER_TYPE_REGULAR)
		{
			sftp_attributes_struct_ex attr_copy(attr);
			if (attr_copy.type == SSH_FILEXFER_TYPE_DIRECTORY && attr_copy.name == ".")
			{
				sftp_attributes_free(attr);
				continue;
			}
			if (attr_copy.type == SSH_FILEXFER_TYPE_DIRECTORY && attr_copy.name == "..")
			{
				sftp_attributes_free(attr);
				continue;
			}
			//check if it is a csv file
			if (attr_copy.type == SSH_FILEXFER_TYPE_REGULAR && attr_copy.name.find(".csv") == std::string::npos)
			{
				sftp_attributes_free(attr);
				continue;
			}

			DirInfoCache.push_back(attr_copy);
		}
		sftp_attributes_free(attr);
	}
	sftp_closedir(dir);
	std::sort(DirInfoCache.begin(), DirInfoCache.end(), [](const sftp_attributes_struct_ex& a, const sftp_attributes_struct_ex& b) {
		return a.mtime > b.mtime;
		});

	this->mutex__.lock();
	this->beginResetModel();
	this->DirInfoCache__ = DirInfoCache;
	std::string lastpath = this->CurrentPath__;
	this->CurrentPath__ = path;
	this->endResetModel();
	this->mutex__.unlock();


	if (!this->isRefreshing__.load())
	{
		if (!this->isForwarding__ && !this->isBacking__)
		{
			this->BackwardStack__.push(lastpath);
			while (this->ForwardStack__.size() > 0)
				this->ForwardStack__.pop();
		}
		else if (this->isForwarding__)
		{
			this->BackwardStack__.push(lastpath);
			this->ForwardStack__.pop();
		}
		else if (this->isBacking__)
		{
			this->ForwardStack__.push(lastpath);
			this->BackwardStack__.pop();
		}
	}

	this->isRefreshing__ = false;
	this->isForwarding__ = false;
	this->isBacking__ = false;
	emit this->pathChanged(QString::fromStdString(path));

	this->setOperationInProgress(false);
}

void SftpFileSystemModel::doRefresh()
{
	this->doDisconnect();
	this->doConnect();
	if (this->isConnected())
	{
		this->isRefreshing__.store(true);
		this->doCd(this->CurrentPath__);
		this->isRefreshing__.store(false);
	}

}
