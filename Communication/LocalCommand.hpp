#pragma once
#include <QObject>
#include "./MetaBackendCom.hpp"
#include <QProcess>
#include <QMetaEnum>

namespace zzs
{
	class LocalCommand : public MetaBackendCommander
	{
		Q_OBJECT
	public:
		LocalCommand(const QString& cmd, const QStringList& args, const QString& WorkingDir = QString(), QObject* parent = nullptr)
			:MetaBackendCommander(cmd, args, parent),
			Process__(new QProcess(this))
		{
			if (!WorkingDir.isEmpty())
				this->Process__->setWorkingDirectory(WorkingDir);
			QObject::connect(this->Process__, &QProcess::readyReadStandardOutput, this, &LocalCommand::ProcessOutput);
			QObject::connect(this->Process__, &QProcess::readAllStandardError, this, &LocalCommand::ProcessOutput);
			QObject::connect(this->Process__, &QProcess::finished, this, &LocalCommand::ProcessFinished);
			QObject::connect(this->Process__, &QProcess::errorOccurred, this, [this](QProcess::ProcessError error) {
				emit CommandError(QMetaEnum::fromType<QProcess::ProcessError>().valueToKey(error));
				});
			QObject::connect(this->Process__, &QProcess::stateChanged, this, [this](QProcess::ProcessState newState) {
				switch (newState)
				{
				case QProcess::NotRunning:
					emit StateChanged(ProcessState::NotRunning);
					break;
				case QProcess::Starting:
					emit StateChanged(ProcessState::Starting);
					break;
				case QProcess::Running:
					emit StateChanged(ProcessState::Running);
					break;
				default:
					break;
				}
				});
		}

		virtual ~LocalCommand()
		{
			this->Process__->close();
			delete this->Process__;
		}
		virtual bool Start() override
		{
			this->Process__->start(this->Command__, this->Arguments__);
			return this->Process__->waitForStarted();
		}
		virtual bool Stop() override
		{
			this->Process__->terminate();
			return this->Process__->waitForFinished(3000);
		}
		virtual bool Kill() override
		{
			this->Process__->kill();
			return this->Process__->waitForFinished(3000);
		}

		virtual bool isRunning() override
		{
			return this->Process__->state() == QProcess::Running;
		}
	private slots:
		void ProcessOutput()
		{
			emit CommandOutput(this->Process__->readAllStandardOutput());
		}
		void ProcessFinished()
		{
			emit CommandFinished();
		}
	private:
		QProcess* Process__;

	};
}