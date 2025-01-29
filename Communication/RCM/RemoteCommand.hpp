#pragma once
#include <QObject>
#include "SessionManager.h"
#include "../MetaBackendCom.hpp"
#include "libssh/libssh.h"
#include "QTimer"
#include "QApplication"

namespace zzs
{
	class RemoteCommand : public MetaBackendCommander
	{
		Q_OBJECT
	public:
		RemoteCommand(const QString& cmd, const QStringList& args, QObject* parent = nullptr, size_t refreshInterval=20, size_t BufferSZ=4096)
			:MetaBackendCommander(cmd, args, parent),
			Channel__(nullptr), BufferSize__(BufferSZ), Buffer__(new char[BufferSZ])
		{
			this->RefreshTimer__ = new QTimer(this);
			this->RefreshTimer__->setInterval(refreshInterval);
			connect(this->RefreshTimer__, &QTimer::timeout, this, &RemoteCommand::RefreshChannel);
		}
		virtual ~RemoteCommand() override
		{
			if (this->Channel__ != nullptr)
				RCM->DistoryChannel(this->Channel__);
			delete[] this->Buffer__;
		}

		virtual bool isRunning() override
		{
			return this->Channel__ != nullptr;
		}

		virtual bool Start() override
		{
			emit this->StateChanged(ProcessState::Starting);
			this->Channel__ = RCM->CreateChannel();
			if (this->Channel__ == nullptr)
			{
				emit this->CommandError(tr("Failed to create the remote channel"));
				this->StateChanged(ProcessState::NotRunning);
				return false;
			}


			int rc= ssh_channel_open_session(this->Channel__);
			if (rc != SSH_OK)
			{
				RCM->DistoryChannel(this->Channel__);
				this->Channel__ = nullptr;
				emit this->CommandError(tr("Failed to open the remote channel"));
				this->StateChanged(ProcessState::NotRunning);
				return false;
			}

			QString FullCmd = this->Command__;
			for (auto arg : this->Arguments__)
			{
				FullCmd += " ";
				FullCmd += arg;
			}

			rc = ssh_channel_request_exec(this->Channel__, FullCmd.toStdString().c_str());
			if (rc != SSH_OK)
			{
				RCM->DistoryChannel(this->Channel__);
				this->Channel__ = nullptr;
				emit this->CommandError(tr("Failed to execute the command"));
				this->StateChanged(ProcessState::NotRunning);
				return false;
			}
			this->RefreshTimer__->start();
			emit this->StateChanged(ProcessState::Running);
			return true;
		}
		virtual bool Stop() override
		{
			return this->doKill("INT");
		}
		virtual bool Kill() override
		{
			return this->doKill("KILL");
		}
	private:
		bool doKill(const char* cmd)
		{
			if (this->Channel__ == nullptr)
			{
				emit this->CommandError(tr("The channel is not running"));
				emit this->StateChanged(ProcessState::NotRunning);
				return false;
			}

		
			int rc = ssh_channel_request_send_signal(this->Channel__, cmd);
			if (rc != SSH_OK)
			{
				emit this->CommandError(tr("Failed to send the stop signal"));
				emit this->StateChanged(ProcessState::Running);
				return false;
			}
			this->RefreshTimer__->stop();
			qApp->processEvents();

			ssh_channel_close(this->Channel__);
			RCM->DistoryChannel(this->Channel__);
			this->Channel__ = nullptr;

			emit this->StateChanged(ProcessState::NotRunning);
			return true;
		}

		void RefreshChannel()
		{
			if (this->Channel__ == nullptr)
				return;
			int nbytes = ssh_channel_read_nonblocking(this->Channel__, this->Buffer__, this->BufferSize__, 0);
			if (nbytes == SSH_ERROR)
			{
				ssh_channel_close(this->Channel__);
				RCM->DistoryChannel(this->Channel__);
				this->Channel__ = nullptr;
				emit this->CommandError(tr("Failed to read the output"));
				emit this->StateChanged(ProcessState::NotRunning);
				return;
			}
			else if (nbytes == SSH_EOF)
			{
				ssh_channel_close(this->Channel__);
				RCM->DistoryChannel(this->Channel__);
				this->Channel__ = nullptr;
				emit this->CommandFinished();
				emit this->StateChanged(ProcessState::NotRunning);
				return;
			}
			else if (nbytes > 0)
			{
				emit this->CommandOutput(QByteArray(this->Buffer__, nbytes));
			}
			else
			{
				//no data
			}
		}

		ssh_channel Channel__;
		size_t BufferSize__;
		char* Buffer__;

		QTimer* RefreshTimer__;
	};
};