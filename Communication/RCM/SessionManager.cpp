#include "SessionManager.h"
#include "iostream"
#include "thread"
#include "future"
#include "exception"

zzs::SessionManager* zzs::SessionManager::getInstance()
{
	if (m_instance == nullptr)
	{
		m_instance = new SessionManager();
	}
	return m_instance;
}

void zzs::SessionManager::destoryInstance()
{
	if (m_instance != nullptr)
	{
		delete m_instance;
		m_instance = nullptr;
	}
}

void zzs::SessionManager::SetServerInfo(const std::string& ip, const std::string& port, const std::string& user, const std::string& pwd)
{
	this->m_ip = ip;
	this->m_port = port;
	this->m_user = user;
	this->m_pwd = pwd;
	ssh_options_set(this->m_sshSession, SSH_OPTIONS_HOST, this->m_ip.c_str());
	ssh_options_set(this->m_sshSession, SSH_OPTIONS_PORT_STR, this->m_port.c_str());
	ssh_options_set(this->m_sshSession, SSH_OPTIONS_USER, this->m_user.c_str());
	int verbosity = SSH_LOG_PROTOCOL;
	ssh_options_set(this->m_sshSession, SSH_OPTIONS_LOG_VERBOSITY, &verbosity);

	this->m_errorMsg = std::string("");
	this->m_isError.store(false);
}

bool zzs::SessionManager::CheckConnection()
{
	if (this->m_isConnecting.load())
	{
		return false;
	}
	auto rtn = ssh_is_connected(this->m_sshSession);
	if (rtn != 0 && this->m_isConnected)
	{
		this->m_isConnected.store(true);
		return true;
	}
	else
	{
		this->m_isConnected.store(false);
		return false;
	}
}

bool zzs::SessionManager::CheckError()
{
	if (this->m_isConnecting.load())
	{
		return false;
	}
	return this->m_isError.load();
}

std::string zzs::SessionManager::GetErrorMsg()
{
	if (this->m_isConnecting.load())
		return std::string("");
	return this->m_errorMsg;
}

bool zzs::SessionManager::Connect()
{
	this->m_isError.store(false);
	this->m_errorMsg = std::string("");
	if (this->m_isConnecting.load())
	{
		this->m_isConnected.store(false);
		return false;
	}

	if (this->m_isConnected.load())
	{
		return true;
	}


	if (this->m_ip.empty() || this->m_port.empty() || this->m_user.empty() || this->m_pwd.empty())
	{
		this->m_isError.store(true);
		this->m_errorMsg = std::string("Server info is empty");
		this->m_isConnected.store(false);
		return false;
	}

	this->m_connectFuture = std::async(std::launch::async, &SessionManager::doConnect, this);
	return true;
}

void zzs::SessionManager::DisConnect()
{
	while (this->m_isConnecting.load())
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(100));
	}
	this->m_isError.store(false);
	this->m_errorMsg = std::string("");
	if (!this->m_isConnected.load())
	{
		return;
	}

	while (!this->m_sftpSessions_set.empty())
	{
		this->DistorySftpSession(*this->m_sftpSessions_set.begin());
	}

	while (!this->m_channels_set.empty())
	{
		this->DistoryChannel(*this->m_channels_set.begin());
	}

	ssh_disconnect(this->m_sshSession);
	this->m_isConnected.store(false);
}

sftp_session zzs::SessionManager::CreateSftpSession()
{
	if (!this->CheckConnection())
	{
		this->m_isError.store(true);
		this->m_errorMsg = std::string("Remote connection error");
		return nullptr;
	}

	sftp_session sftp = sftp_new(this->m_sshSession);
	if (sftp == nullptr)
	{
		this->m_isError.store(true);
		this->m_errorMsg = std::string(std::string("Error allocating SFTP session") + ssh_get_error(this->m_sshSession));
		std::cout << "Error allocating SFTP session: " << ssh_get_error(this->m_sshSession) << "\n";
		return nullptr;
	}

	if (sftp_init(sftp) != SSH_OK)
	{
		this->m_isError.store(true);
		this->m_errorMsg = std::string(std::string("Error initializing SFTP session") + ssh_get_error(this->m_sshSession));
		std::cout << "Error initializing SFTP session: " << ssh_get_error(this->m_sshSession) << "\n";
		sftp_free(sftp);
		return nullptr;
	}

	this->m_sftpSessions_set.insert(sftp);
	this->m_isError.store(false);
	this->m_errorMsg = std::string("");
	return sftp;
}

void zzs::SessionManager::DistorySftpSession(sftp_session sftp)
{
	if (sftp != nullptr && this->m_sftpSessions_set.count(sftp) > 0)
	{
		sftp_free(sftp);
		this->m_sftpSessions_set.erase(sftp);
	}
	else
	{
		std::cout << "Error distorying SFTP session: Object dose not exist\n";
	}
}

ssh_channel zzs::SessionManager::CreateChannel()
{
	if (!this->m_isConnected.load())
	{
		this->m_isError.store(true);
		this->m_errorMsg = std::string("Remote connection error");
		return nullptr;
	}

	ssh_channel chennel = ssh_channel_new(this->m_sshSession);
	if (chennel == nullptr)
	{
		this->m_isError.store(true);
		this->m_errorMsg = std::string(std::string("Error creating channel") + ssh_get_error(this->m_sshSession));
		std::cout << "Error creating channel: " << ssh_get_error(this->m_sshSession) << "\n";
		return nullptr;
	}

	this->m_channels_set.insert(chennel);
	this->m_isError.store(false);
	this->m_errorMsg = std::string("");
	return chennel;
}

void zzs::SessionManager::DistoryChannel(ssh_channel channel)
{
	if (channel != nullptr && this->m_channels_set.count(channel) > 0)
	{
		ssh_channel_free(channel);
		this->m_channels_set.erase(channel);
	}
	else
	{
		std::cout << "Error distorying channel: Object dose not exist\n";
	}
}

zzs::SessionManager::SessionManager()
	:m_isConnected(false), m_isConnecting(false),
	m_ip(""), m_port(""), m_user(""), m_pwd("")
{
	this->m_sshSession = ssh_new();
	if (this->m_sshSession == nullptr)
	{
		this->m_isError.store(true);
		this->m_errorMsg = std::string("Error creating ssh session");
		std::cout << "Error creating ssh session\n";
		throw std::runtime_error("Error creating ssh session");
	}
	this->m_errorMsg = std::string("");
	this->m_isError.store(false);
}

zzs::SessionManager::~SessionManager()
{
	while (this->m_isConnecting.load())
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(100));
	}

	if (this->m_isConnected.load())
	{
		this->DisConnect();
	}
	ssh_free(this->m_sshSession);
}

bool zzs::SessionManager::doConnect()
{
	this->m_isConnecting.store(true);
	this->m_errorMsg = std::string("");
	this->m_isError.store(false);
	int rc = ssh_connect(this->m_sshSession);
	if (rc != SSH_OK)
	{
		auto rtn = ssh_is_connected(this->m_sshSession);
		std::cout << "Error connecting: " << ssh_get_error(this->m_sshSession) << "\n";
		this->m_isConnected.store(false);
		this->m_isConnecting.store(false);
		this->m_isError.store(true);
		this->m_errorMsg = std::string(std::string("Connecting error, ") + ssh_get_error(this->m_sshSession));

		return false;
	}

	if (ssh_session_is_known_server(this->m_sshSession) != SSH_KNOWN_HOSTS_OK)
	{
		std::cout << "Server not known\n";
		this->m_isConnected.store(false);
		this->m_isConnecting.store(false);
		this->m_isError.store(true);
		this->m_errorMsg = std::string("Server not known");

		return false;
	}

	if (ssh_userauth_password(this->m_sshSession, this->m_user.c_str(), this->m_pwd.c_str()) != SSH_AUTH_SUCCESS)
	{
		std::cout << "Error authenticating with password: " << ssh_get_error(this->m_sshSession) << "\n";
		this->m_isConnected.store(false);
		this->m_isConnecting.store(false);
		this->m_isError.store(true);
		this->m_errorMsg = std::string(std::string("Error authenticating with password") + ssh_get_error(this->m_sshSession));

		return false;
	}
	std::cout << "Connected\n";
	this->m_isConnected.store(true);
	this->m_isConnecting.store(false);
	this->m_isError.store(false);
	this->m_errorMsg = std::string("");
	return true;
}
