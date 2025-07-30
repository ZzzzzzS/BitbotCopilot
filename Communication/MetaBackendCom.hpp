#pragma once
#include "QObject"
#include "QString"
#include "QByteArray"
#include "QStringList"


namespace zzs
{
	class MetaBackendCommander : public QObject
	{
		Q_OBJECT
	public:
		enum class ProcessState
		{
			NotRunning,
			Starting,
			Running
		};
	public:
		MetaBackendCommander(const QString& cmd, const QStringList& args, QObject* parent)
			:Command__(cmd), Arguments__(args)
		{
			qRegisterMetaType<ProcessState>("BackendState");
		}

		virtual ~MetaBackendCommander() {}

		virtual bool Start() = 0;
		virtual bool Stop() = 0;
		virtual bool Kill() = 0;
		virtual bool isRunning() = 0;

	signals:
		void CommandOutput(const QByteArray& output);
		void CommandFinished();
		void CommandError(const QString& error);
		void StateChanged(ProcessState newState);

	protected:
		QString Command__;
		QStringList Arguments__;
	};
};