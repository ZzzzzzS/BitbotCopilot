#pragma once
#include "string"
#include "libssh/libssh.h"
#include "libssh/sftp.h"
#include "libssh/callbacks.h"
#include "thread"
#include "set"
#include "atomic"
#include "future"

namespace zzs
{
#define RCM zzs::SessionManager::getInstance()

	class  SessionManager
	{
	public:
		static SessionManager* getInstance();
		static void destoryInstance();
	
	public:
		void SetServerInfo(const std::string& ip, const std::string& port, const std::string& user, const std::string& pwd);
		bool CheckConnection();
		bool CheckError();
		std::string GetErrorMsg();
		bool Connect();
		void DisConnect();

		sftp_session CreateSftpSession();
		void DistorySftpSession(sftp_session sftp);
		ssh_channel CreateChannel();
		void DistoryChannel(ssh_channel channel);

	private:
		SessionManager();
		~SessionManager();

		bool doConnect();

		std::string m_ip;
		std::string m_port;
		std::string m_user;
		std::string m_pwd;

		std::set<sftp_session> m_sftpSessions_set;
		std::set<ssh_channel> m_channels_set;
		
		std::atomic_bool m_isConnected;
		std::atomic_bool m_isConnecting;
		std::atomic_bool m_isError;
		std::string m_errorMsg;
		std::future<bool> m_connectFuture;

		ssh_session m_sshSession;
	};

	static SessionManager* m_instance = nullptr;
};