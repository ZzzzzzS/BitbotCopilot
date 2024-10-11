#include "BitbotTcpProtocalV1.h"
#include <QByteArray>
#include <QAbstractSocket>
#include <QMetaType>
#include <QString>
#include <iostream>
#include <QJsonDocument>
#include <QTimer>
#include <QApplication>

zzs::BITBOT_TCP_PROTOCAL_V1::BITBOT_TCP_PROTOCAL_V1(const QJsonObject& cfg, QObject* parent)
	:META_COMMUNICATION(parent)
{
	qRegisterMetaType<QAbstractSocket::SocketError>("SOCKET_ERROR");
	qRegisterMetaType<zzs::META_COMMUNICATION::CONNECTION_STATUS>("CONN_STATUS");
	auto WSSProxyManager = QNetworkProxy(QNetworkProxy::ProxyType::NoProxy);
	this->SDOManager__ = new QNetworkAccessManager(this);
	this->SDOManager__->setProxy(WSSProxyManager);
	this->PDOManager__ = new QWebSocket(QString(), QWebSocketProtocol::VersionLatest, this);
	this->PDOManager__->setProxy(WSSProxyManager);
	this->RefreshTimer__->setInterval(100);
	QObject::connect(this->RefreshTimer__, &QTimer::timeout, this, [this]() {
		this->RequestPDO();
	});
}

zzs::BITBOT_TCP_PROTOCAL_V1::~BITBOT_TCP_PROTOCAL_V1()
{
}


bool zzs::BITBOT_TCP_PROTOCAL_V1::Connect(QString Host, uint16_t port, uint timeout)
{
	if (this->PDODataConnection__ == CONNECTION_STATUS::CONNECTING || this->PDODataConnection__==CONNECTION_STATUS::CONNECTED)
	{
		qDebug() << this->PDOManager__->state();
		return false;
	}

	if (this->StateListConnection__ == CONNECTION_STATUS::CONNECTING ||
		this->ControlListConnection__ == CONNECTION_STATUS::CONNECTING ||
		this->PDOHeaderConnection__ == CONNECTION_STATUS::CONNECTING ||
		this->PDODataConnection__ == CONNECTION_STATUS::CONNECTING)
		return false;

	QTimer::singleShot(0, this, [this, Host, port, timeout]() {
		this->doConnect(Host, port, timeout);
		});
	qApp->processEvents();
	qApp->processEvents();

	return true;
}

bool zzs::BITBOT_TCP_PROTOCAL_V1::Disconnect()
{
	if (this->StateListConnection__ == CONNECTION_STATUS::CONNECTING ||
		this->ControlListConnection__ == CONNECTION_STATUS::CONNECTING ||
		this->PDOHeaderConnection__ == CONNECTION_STATUS::CONNECTING ||
		this->PDODataConnection__ == CONNECTION_STATUS::CONNECTING)
		return false;
	
	QTimer::singleShot(0, this, [this]() {
		this->PDOManager__->close();
		});
	qApp->processEvents();
	this->AlreadSentErrorMessage__ = false;
	this->StateListConnection__ = CONNECTION_STATUS::DISCONNECT;
	this->ControlListConnection__ = CONNECTION_STATUS::DISCONNECT;
	this->PDOHeaderConnection__ = CONNECTION_STATUS::DISCONNECT;
	this->CheckConnection();
	return true;
}

bool zzs::BITBOT_TCP_PROTOCAL_V1::SendUserCommand(const QVariantMap& CommandPairs)
{
	if(this->PDODataConnection__!=CONNECTION_STATUS::CONNECTED)
		return false;
	if (CommandPairs.isEmpty()) return false;
	
	QJsonObject json;
	json.insert("type", "events");
	QJsonArray KeyValueArray;
	for (auto& [key,value] : CommandPairs.toStdMap())
	{ 
		QString ButtonKey;
		if (this->ReverseAvailableKeyPairs__.contains(key))
		{
			ButtonKey = key;
		}
		else if (this->AvailableKeyPairs__.contains(key))
		{
			ButtonKey = this->AvailableKeyPairs__[key];
		}
		else
		{
			std::cerr << "[" << typeid(this).name() << "]: Button " << key.toStdString() << " is not in the allowed list" << std::endl;
			continue;
		}

		QJsonValue ButtonValue;
		QJsonObject SingleEvent;
		if (value.type()==QMetaType::Float||value.type()==QMetaType::Double)
		{
			double var = value.toDouble();
			qint64* intjar = reinterpret_cast<qint64*>(&var);
			ButtonValue = QJsonValue(*intjar); //some magical code to suit bitbot communication protocal.
		}
		else if (value.type()==QMetaType::Int || value.type()==QMetaType::UInt 
			|| value.type()==QMetaType::Short || value.type()==QMetaType::UShort)
		{
			ButtonValue = QJsonValue(value.toLongLong());
		}
		else if (value.type() == QMetaType::Bool)
		{
			ButtonValue = QJsonValue(value.toLongLong());
		}
		else
		{
			std::cerr << "[" << typeid(this).name() << "]: Button " << key.toStdString() << "'s value type is not in the allowed list" << std::endl;
			continue;
		}
		SingleEvent.insert("name", ButtonKey);
		SingleEvent.insert("value", ButtonValue);
		KeyValueArray.append(SingleEvent);
	}
	//json.insert("data", KeyValueArray);
	qDebug() << "user cmd:" << KeyValueArray;
	QJsonObject events;
	events.insert("events", KeyValueArray);
	QByteArray UserCmdParser = QJsonDocument(events).toJson(QJsonDocument::Compact);
	json.insert("data", QJsonValue(QString(UserCmdParser))); //magical again!

	this->UserCommandArray = QJsonDocument(json).toJson(QJsonDocument::Compact);

	QTimer::singleShot(0,this, [this]() {
		this->PDOManager__->sendBinaryMessage(this->UserCommandArray);
		});

		return true;
}

bool zzs::BITBOT_TCP_PROTOCAL_V1::getStateList(QMap<int, QString>& States)
{
	if (this->ConnectionState__ != CONNECTION_STATUS::CONNECTED)
		return false;
	States = this->StateList__;
	return true;
}

bool zzs::BITBOT_TCP_PROTOCAL_V1::getAvailableKeys(QMap<QString, QString>& KeyPairs,bool reverse)
{
	if (this->ConnectionState__ != CONNECTION_STATUS::CONNECTED)
		return false;

	KeyPairs = (reverse) ? this->ReverseAvailableKeyPairs__ : this->AvailableKeyPairs__;
	return true;
}

bool zzs::BITBOT_TCP_PROTOCAL_V1::getDataHeaderAll(QVector<QString>& Headers)
{
	if (this->ConnectionState__ != CONNECTION_STATUS::CONNECTED)
		return false;
	Headers = this->PDODataHeaders__;
	return true;
}

bool zzs::BITBOT_TCP_PROTOCAL_V1::getKernelHeader(QVector<QString>& Headers)
{
	if (this->ConnectionState__ != CONNECTION_STATUS::CONNECTED)
		return false;
	Headers = this->KernelDataHeaders__;
	return true;
}

bool zzs::BITBOT_TCP_PROTOCAL_V1::getExtraHeader(QVector<QString>& Headers)
{
	if (this->ConnectionState__ != CONNECTION_STATUS::CONNECTED)
		return false;
	Headers = this->ExtraDataHeaders__;
	return true;
}

bool zzs::BITBOT_TCP_PROTOCAL_V1::getDeviceHeader(QVector<ABSTRACT_DEVICE_HEADER>& Headers)
{
	if (this->ConnectionState__ != CONNECTION_STATUS::CONNECTED)
		return false;
	Headers = this->DeviceHeaders__;
	return true;
}

void zzs::BITBOT_TCP_PROTOCAL_V1::doConnect(QString Host, uint16_t port, uint timeout)
{
	if (timeout != 0)
	{
		this->SDOManager__->setTransferTimeout(timeout);
	}

	this->AlreadSentErrorMessage__ = false;
	this->StateListConnection__ = CONNECTION_STATUS::DISCONNECT;
	this->ControlListConnection__ = CONNECTION_STATUS::DISCONNECT;
	this->PDOHeaderConnection__ = CONNECTION_STATUS::DISCONNECT;
	this->PDODataConnection__ = CONNECTION_STATUS::DISCONNECT;

	QString PortString = QString::number(port);
	QString UrlPrefix = QString("http://") + Host + QString(":") + PortString;
	QString RequestStatesUrl = UrlPrefix + QString("/monitor/stateslist");
	QString RequestControlUrl = UrlPrefix + QString("/setting/control/get");
	QString RequestPDOHeaderUrl = UrlPrefix + QString("/monitor/headers");
	QString WsPDODataUrl = QString("ws://") + Host + QString(":") + PortString + QString("/console");

	//http request
	QNetworkRequest StateListRequester = QNetworkRequest(QUrl(RequestStatesUrl));
	StateListRequester.setRawHeader("User-Agent", "BITBOT COPILOT");
	this->StateListConnection__ = CONNECTION_STATUS::CONNECTING;
	this->StateListReply__ = this->SDOManager__->get(StateListRequester);
	QObject::connect(this->StateListReply__, &QNetworkReply::finished, this, [this]() {
		if (this->StateListReply__->error() == QNetworkReply::NoError)
		{
			QByteArray StateListArray = this->StateListReply__->readAll();
			qDebug() << "State List Received:" << StateListArray;
			if (this->ParseStateList(StateListArray))
				this->StateListConnection__ = CONNECTION_STATUS::CONNECTED;
			else
				this->StateListConnection__ = CONNECTION_STATUS::ERRORED;
		}
		else
		{
			this->StateListConnection__ = CONNECTION_STATUS::ERRORED;
		}
		this->CheckConnection();
		});
	QObject::connect(this->StateListReply__, &QNetworkReply::errorOccurred, this, [this](QNetworkReply::NetworkError Code) {
		qDebug() << "State List Error" << Code;
		this->StateListConnection__ = CONNECTION_STATUS::ERRORED;
		this->CheckConnection();
		});

	QNetworkRequest ControlListRequester = QNetworkRequest(QUrl(RequestControlUrl));
	ControlListRequester.setRawHeader("User-Agent", "BITBOT COPILOT");
	this->ControlListConnection__ = CONNECTION_STATUS::CONNECTING;
	this->ControlListReply__ = this->SDOManager__->get(ControlListRequester);
	QObject::connect(this->ControlListReply__, &QNetworkReply::finished, this, [this]() {
		if (this->ControlListReply__->error() == QNetworkReply::NoError)
		{
			QByteArray ControlListArray = this->ControlListReply__->readAll();
			qDebug() << "Control List Received:" << ControlListArray;
			if (this->ParseControlList(ControlListArray))
				this->ControlListConnection__ = CONNECTION_STATUS::CONNECTED;
			else
				this->ControlListConnection__ = CONNECTION_STATUS::ERRORED;
		}
		else
		{
			this->ControlListConnection__ = CONNECTION_STATUS::ERRORED;
		}
		this->CheckConnection();
		});
	QObject::connect(this->ControlListReply__, &QNetworkReply::errorOccurred, this, [this](QNetworkReply::NetworkError Code) {
		qDebug() << "Control List Error" << Code;
		this->ControlListConnection__ = CONNECTION_STATUS::ERRORED;
		this->CheckConnection();
		});

	QNetworkRequest PDOHeaderRequester = QNetworkRequest(QUrl(RequestPDOHeaderUrl));
	PDOHeaderRequester.setRawHeader("User-Agent", "BITBOT COPILOT");
	this->PDOHeaderConnection__ = CONNECTION_STATUS::CONNECTING;
	this->PDOHeaderReply__ = this->SDOManager__->get(PDOHeaderRequester);
	QObject::connect(this->PDOHeaderReply__, &QNetworkReply::finished, this, [this]() {
		if (this->PDOHeaderReply__->error() == QNetworkReply::NoError)
		{
			QByteArray PDOHeaderArray = this->PDOHeaderReply__->readAll();
			qDebug() << "PDOHeader Received:" << PDOHeaderArray;
			if (this->ParsePDOHeader(PDOHeaderArray))
				this->PDOHeaderConnection__ = CONNECTION_STATUS::CONNECTED;
			else
				this->PDOHeaderConnection__ = CONNECTION_STATUS::ERRORED;
		}
		else
		{
			this->PDOHeaderConnection__ = CONNECTION_STATUS::ERRORED;
		}
		this->CheckConnection();
		});
	QObject::connect(this->PDOHeaderReply__, &QNetworkReply::errorOccurred, this, [this](QNetworkReply::NetworkError Code) {
		qDebug() << "PDO Header Error" << Code;
		this->PDOHeaderConnection__ = CONNECTION_STATUS::ERRORED;
		this->CheckConnection();
		});

	//websocket request
	QObject::connect(this->PDOManager__, &QWebSocket::connected, this, [this]() {
		this->PDODataConnection__ = CONNECTION_STATUS::CONNECTED;
		this->CheckConnection();
		this->RefreshTimer__->start();
		});
	QObject::connect(this->PDOManager__, &QWebSocket::disconnected, this, [this]() {
		this->RefreshTimer__->stop();
		this->PDODataConnection__ = CONNECTION_STATUS::DISCONNECT;
		this->CheckConnection();
		});
	QObject::connect(this->PDOManager__, QOverload<QAbstractSocket::SocketError>::of(&QWebSocket::error), this, [this](QAbstractSocket::SocketError error) {
		qDebug() << "websocket error:" << error;
		this->RefreshTimer__->stop();
		this->PDODataConnection__ = CONNECTION_STATUS::ERRORED;
		//this->PDOManager__->abort();
		//this->PDOManager__->close();
		this->CheckConnection();
		});
	QObject::connect(this->PDOManager__, &QWebSocket::textMessageReceived, this, [this](QString Message) {

		this->ParsePDOData(Message.toLatin1());
		});

	this->PDOManager__->open(QUrl(WsPDODataUrl));
}

bool zzs::BITBOT_TCP_PROTOCAL_V1::RequestPDO()
{
	
	if(this->PDODataConnection__!=CONNECTION_STATUS::CONNECTED)
		return false;
	else
	{
		QString arr("{\"type\":\"request_data\",\"data\":\"\"}");
		this->PDOManager__->sendTextMessage(arr);
		return true;
	}
}

bool zzs::BITBOT_TCP_PROTOCAL_V1::CheckConnection()
{
	if (
		this->StateListConnection__ == CONNECTION_STATUS::CONNECTED &&
		this->ControlListConnection__ == CONNECTION_STATUS::CONNECTED &&
		this->PDOHeaderConnection__ == CONNECTION_STATUS::CONNECTED &&
		this->PDODataConnection__ == CONNECTION_STATUS::CONNECTED
		)
	{
		this->AlreadSentErrorMessage__ = false;
		emit this->ConnectionStateChanged(static_cast<int>(META_COMMUNICATION::CONNECTION_STATUS::CONNECTED));
		return true;
	}
	else if (
		this->StateListConnection__ == CONNECTION_STATUS::CONNECTING ||
		this->ControlListConnection__ == CONNECTION_STATUS::CONNECTING ||
		this->PDOHeaderConnection__ == CONNECTION_STATUS::CONNECTING ||
		this->PDODataConnection__ == CONNECTION_STATUS::CONNECTING
		)
	{
		return false;
	}
	else if (
		this->StateListConnection__ == CONNECTION_STATUS::ERRORED ||
		this->ControlListConnection__ == CONNECTION_STATUS::ERRORED ||
		this->PDOHeaderConnection__ == CONNECTION_STATUS::ERRORED ||
		this->PDODataConnection__ == CONNECTION_STATUS::ERRORED
		)
	{
		if (!this->AlreadSentErrorMessage__)
		{
			this->AlreadSentErrorMessage__ = true;
			this->PDOManager__->abort();
			emit this->ConnectionStateChanged(static_cast<int>(META_COMMUNICATION::CONNECTION_STATUS::ERRORED));
		}
		return false;
	}
	else if (
		this->StateListConnection__ == CONNECTION_STATUS::DISCONNECT &&
		this->ControlListConnection__ == CONNECTION_STATUS::DISCONNECT &&
		this->PDOHeaderConnection__ == CONNECTION_STATUS::DISCONNECT &&
		this->PDODataConnection__ == CONNECTION_STATUS::DISCONNECT
		)
	{
		this->AlreadSentErrorMessage__ = false;
		emit this->ConnectionStateChanged(static_cast<int>(META_COMMUNICATION::CONNECTION_STATUS::DISCONNECT));
		return false;
	}
	else
	{
		return false;
	}
}

bool zzs::BITBOT_TCP_PROTOCAL_V1::ParseStateList(const QByteArray& array)
{
	QJsonParseError err;
	QJsonDocument parser = QJsonDocument::fromJson(array, &err);
	if (err.error != QJsonParseError::NoError)
	{
		qDebug() << "failed to parse state list";
		return false;
	}
	try
	{
		QJsonArray StateArray = parser["states"].toArray();
		this->StateList__.clear();
		for (auto i : StateArray)
		{
			auto StateObject = i.toObject();
			int id = StateObject["id"].toInt();
			QString name = StateObject["name"].toString();
			this->StateList__.insert(id, name);
		}
	}
	catch (const std::exception& e)
	{
		std::cerr << "[" << typeid(this).name() << "] exception: " << e.what() << std::endl;
		return false;
	}
	return true;
}

bool zzs::BITBOT_TCP_PROTOCAL_V1::ParseControlList(const QByteArray& array)
{
	QJsonParseError err;
	QJsonDocument parser = QJsonDocument::fromJson(array, &err);
	if (err.error != QJsonParseError::NoError)
	{
		qDebug() << "failed to parse control list";
		return false;
	}
	try
	{
		QJsonArray ControlArray = parser.array();
		this->AvailableKeyPairs__.clear();
		this->ReverseAvailableKeyPairs__.clear();
		for (auto i : ControlArray)
		{
			auto ControlObject = i.toObject();
			QString event = ControlObject["event"].toString();
			QString kb_key = ControlObject["kb_key"].toString();
			this->ReverseAvailableKeyPairs__.insert(event, kb_key);
			this->AvailableKeyPairs__.insert(kb_key, event);
		}
	}
	catch (const std::exception& e)
	{
		std::cerr << "[" << typeid(this).name() << "] exception: " << e.what() << std::endl;
		return false;
	}
	return true;
}

bool zzs::BITBOT_TCP_PROTOCAL_V1::ParsePDOHeader(const QByteArray& array)
{
	QJsonParseError err;
	QJsonDocument parser = QJsonDocument::fromJson(array, &err);
	if (err.error != QJsonParseError::NoError)
	{
		qDebug() << "failed to parse PDO header";
		return false;
	}
	QJsonObject RootObj = parser.object();
	try
	{
		QJsonArray KernelHeader = RootObj["kernel"].toArray();
		QJsonObject BusHeader = RootObj["bus"].toObject();
		QJsonArray BusDeviceHeader = BusHeader["devices"].toArray();
		QJsonArray ExtraDataHeader = RootObj["extra"].toArray();
		this->PDODataHeaders__.clear();

		this->KernelDataHeaders__.clear();
		for (auto i : KernelHeader)
		{
			QString kernel_state = i.toString();
			this->KernelDataHeaders__.push_back(kernel_state);
			this->PDODataHeaders__.push_back(kernel_state);
		}

		this->DeviceHeaders__.clear();
		for (auto i : BusDeviceHeader)
		{
			QJsonObject DevObj = i.toObject();
			ABSTRACT_DEVICE_HEADER DevHeader;
			DevHeader.DeviceName = DevObj["name"].toString();
			DevHeader.DeviceTypeName = DevObj["type"].toString();
			
			QJsonArray DevSensors = DevObj["headers"].toArray();
			for (auto j : DevSensors)
			{
				QString name = j.toString();
				DevHeader.SensorsName.push_back(name);
				this->PDODataHeaders__.push_back(name);
			}
			this->DeviceHeaders__.push_back(DevHeader);
		}

		this->ExtraDataHeaders__.clear();
		for (auto i : ExtraDataHeader)
		{
			QString ExDat = i.toString();
			this->ExtraDataHeaders__.push_back(ExDat);
			this->PDODataHeaders__.push_back(ExDat);
		}
	}
	catch (const std::exception& e)
	{
		std::cerr << "[" << typeid(this).name() << "] exception: " << e.what() << std::endl;
		return false;
	}


	return true;
}

bool zzs::BITBOT_TCP_PROTOCAL_V1::ParsePDOData(const QByteArray& array)
{
	QJsonParseError err;
	QJsonDocument parser = QJsonDocument::fromJson(array, &err);
	if (err.error != QJsonParseError::NoError)
	{
		qDebug() << "failed to parse PDO data";
		return false;
	}
	
	QJsonObject root_obj = parser.object();

	try
	{
		if (!root_obj.contains("type") || !root_obj.contains("data") || root_obj["type"].toString() != QString("monitor_data"))
		{
			qDebug() << "PDO data format is unsupported";
			return false;
		}
		
		QString DataString = root_obj["data"].toString();
		//qDebug() << "received data string" << DataString;

		QJsonParseError err2;
		QJsonObject RealPayload = QJsonDocument::fromJson(DataString.toLatin1(), &err2).object();
		if (err2.error != QJsonParseError::NoError)
		{
			qDebug() << "failed to parse PDO payload";
			return false;
		}
		QJsonArray PayloadValueArray = RealPayload["data"].toArray();
		if (PayloadValueArray.size() != this->PDODataHeaders__.size())
		{
			qDebug() << "Receive PDO data miss aligned!";
			return false;
		}
		QVariantList PDOList = PayloadValueArray.toVariantList();
		emit this->ReceivedPDO(PDOList);
		return true;
	}
	catch (const std::exception& e)
	{
		std::cerr << "[" << typeid(this).name() << "] exception: " << e.what() << std::endl;
		return false;
	}
	
	return true;
}
