#pragma once

#include <QObject>
#include <QDebug>
#include <QJsonObject>
#include <QJsonArray>
#include <QMap>
#include <QVector>
#include <QUrl>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QWebSocket>
#include <QByteArray>
#include <QNetworkProxy>

#include "MetaCommunication.hpp"


namespace zzs
{
	class BITBOT_TCP_PROTOCAL_V1 : public META_COMMUNICATION
	{
		Q_OBJECT
	public:
		struct ABSTRACT_DEVICE_HEADER
		{
			QString DeviceName;
			QString DeviceTypeName;
			QVector<QString> SensorsName;
		};
	public:
		BITBOT_TCP_PROTOCAL_V1(const QJsonObject& cfg = QJsonObject(), QObject* parent = nullptr);
		~BITBOT_TCP_PROTOCAL_V1();

		bool Connect(QString Host, uint16_t port, uint timeout = 0) final;
		bool Disconnect() final;
		bool SendUserCommand(const QVariantMap& CommandPair) final;


		bool getStateList(QMap<int, QString>& States);
		bool getAvailableKeys(QMap<QString, QString>& KeyPairs, bool reverse = true);
		bool getDataHeaderAll(QVector<QString>& Headers);
		bool getKernelHeader(QVector<QString>& Headers);
		bool getExtraHeader(QVector<QString>& Headers);
		bool getDeviceHeader(QVector<ABSTRACT_DEVICE_HEADER>& Headers);


	private:
		bool RequestPDO() final;
		bool CheckConnection();
		bool ParseStateList(const QByteArray& array);
		bool ParseControlList(const QByteArray& array);
		bool ParsePDOHeader(const QByteArray& array);
		bool ParsePDOData(const QByteArray& array);

	private:
		QVector<QString> PDODataHeaders__;
		QMap<int, QString> StateList__;
		QMap<QString, QString> AvailableKeyPairs__; //<key,name>
		QMap<QString, QString> ReverseAvailableKeyPairs__; //<name,key>
		QVector<QString> KernelDataHeaders__;
		QVector<QString> ExtraDataHeaders__;
		QVector<ABSTRACT_DEVICE_HEADER> DeviceHeaders__;

		QUrl RequestPDOUrl;
		QVariantList PDOData__;

		QNetworkAccessManager* SDOManager__ = nullptr;
		QWebSocket* PDOManager__ = nullptr;

		QNetworkReply* StateListReply__ = nullptr;
		QNetworkReply* ControlListReply__ = nullptr;
		QNetworkReply* PDOHeaderReply__ = nullptr;
		CONNECTION_STATUS StateListConnection__ = CONNECTION_STATUS::DISCONNECT;
		CONNECTION_STATUS ControlListConnection__ = CONNECTION_STATUS::DISCONNECT;
		CONNECTION_STATUS PDOHeaderConnection__ = CONNECTION_STATUS::DISCONNECT;
		CONNECTION_STATUS PDODataConnection__ = CONNECTION_STATUS::DISCONNECT;
		bool AlreadSentErrorMessage__ = false;

		QByteArray UserCommandArray;
	};
};