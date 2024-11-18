#pragma once
#include <QObject>
#include <QAbstractItemModel>
#include <atomic>
#include <vector> 
#include <stack>
#include <libssh/sftp.h>
#include <QMutex>
#include <QMutexLocker>
#include <QIcon>

#include "SessionManager.h"

struct sftp_attributes_struct_ex {
	std::string name;
	std::string longname;/* ls -l output on openssh, not reliable else */
	uint32_t flags;
	uint8_t type;
	uint64_t size;
	uint32_t uid;
	uint32_t gid;
	std::string owner;/* set if openssh and version 4 */
	std::string group;/* set if openssh and version 4 */
	uint32_t permissions;
	uint64_t atime64;
	uint32_t atime;
	uint32_t atime_nseconds;
	uint64_t createtime;
	uint32_t createtime_nseconds;
	uint64_t mtime64;
	uint32_t mtime;
	uint32_t mtime_nseconds;
	uint32_t extended_count;

	sftp_attributes_struct_ex(sftp_attributes original)
	{
		name = std::string(original->name);
		longname = std::string(original->longname);
		flags = original->flags;
		type = original->type;
		size = original->size;
		uid = original->uid;
		gid = original->gid;
		owner = std::string(original->owner);
		group = std::string(original->group);
		permissions = original->permissions;
		atime64 = original->atime64;
		atime = original->atime;
		atime_nseconds = original->atime_nseconds;
		createtime = original->createtime;
		createtime_nseconds = original->createtime_nseconds;
		mtime64 = original->mtime64;
		mtime = original->mtime;
		mtime_nseconds = original->mtime_nseconds;
		extended_count = original->extended_count;
	}
};

class SftpFileSystemModel : public QAbstractItemModel
{
	Q_OBJECT
public:
	SftpFileSystemModel(const QString RootPath,QObject* parent = nullptr);
	~SftpFileSystemModel();
	
	bool connect();
	void disconnect();

	QString path() const;
	bool isConnected() const;
	bool isOperationInProgress() const;
	bool cd(const QString& path);
	bool cd(const std::string& path);
	bool cdup();
	bool Refresh();
	bool isCurrentPathRoot() const;
	bool CanForward() const;
	bool CanBackward() const;
	bool Forward();
	bool Backward();

	int rowCount(const QModelIndex& parent = QModelIndex()) const override;
	int columnCount(const QModelIndex& parent = QModelIndex()) const override;
	QModelIndex index(int row, int column, const QModelIndex& parent = QModelIndex()) const override;
	QModelIndex parent(const QModelIndex& child) const override;
	QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
	QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;


signals:
	void connectStateChanged(bool connected);
	void pathChanged(const QString& path);
	void error(const QString& error);
	void operationInProgressChanged(bool inProgress);

signals:
	void TriggerCd(const std::string& path);
	void TriggerConnect();
	void TriggerDisconnect();
	void TriggerRefresh();

private:
	std::atomic_bool connected__;
	std::atomic_bool operationInProgress__;
	std::atomic_bool isForwarding__;
	std::atomic_bool isBacking__;
	std::atomic_bool isRefreshing__;
	
	mutable QMutex mutex__;
	std::vector<sftp_attributes_struct_ex> DirInfoCache__;
	std::string CurrentPath__;
	std::string RootPath__;
	sftp_session SftpSession__;

	QIcon DirIconCache__;
	QIcon FileIconCache__;

	std::stack<std::string> BackwardStack__;
	std::stack<std::string> ForwardStack__;

private:
	void setOperationInProgress(bool inProgress);
	void doConnect();
	void doDisconnect();
	void doCd(const std::string& path);
	void doRefresh();

};