#pragma once
#include <QWidget>
#include "ElaScrollPage.h"
#include "ElaScrollPageArea.h"
#include "ElaSpinBox.h"
#include "ElaLineEdit.h"
#include "ElaPushButton.h"
#include "widget/RTDKernelStates.h"
#include "widget/RTDRobotStates.h"
#include "widget/RTDGamepadStatus.h"
#include "widget/RTDKeyboardStatus.h"
#include "widget/RTDDeviceInfo.h"
#include "../Communication/BitbotTcpProtocalV1.h"
#include "../Communication/MetaCommunication.hpp"
#include "../Utils/GamepadDriver/ZQGamepad.h"
#include <QThread>
#include <QVariant>
#include <QVariantList>
#include <QMap>
#include <QVBoxLayout>


class PilotPage : public ElaScrollPage
{
	Q_OBJECT

public:
	PilotPage(QWidget* parent = nullptr);
	~PilotPage();

private:
	void InitConnectionWidget();
	void InitCommHandle();
	void InitUserInput();
	void DrawConnectedUI();
	void DrawDisconnectedUI();

	void ProcessDisconnetced();
	void ProcessConnected();
	void ProcessConnectionError();
	void ProcessPDO(QVariantList PDOInfo);

protected:
	void keyPressEvent(QKeyEvent* event);
	void keyReleaseEvent(QKeyEvent* event);


private:
	ZQGamepad* GamepadHandle__;
	zzs::BITBOT_TCP_PROTOCAL_V1* CommHandle__;
	QThread* CommThread__;
	bool connected__ = false;
private:
	QVBoxLayout* CentralLayout__ = nullptr;
	QWidget* CentralWidget__ = nullptr;

	ElaScrollPageArea* ConnectionAreaUI__ = nullptr;
	ElaSpinBox* SpinBox_Port__ = nullptr;
	ElaLineEdit* LineEdit_IP__ = nullptr;
	ElaPushButton* PushButton_Connect__ = nullptr;
	QString IP = QString("172.17.50.219");
	uint16_t port = 12888;

	RTDKernelStates* KernelStatusUI__ = nullptr;
	RTDRobotStates* RobotStateUI__ = nullptr;
	RTDKeyboardStatus* KeyboardEventUI__ = nullptr;
	RTDGamepadStatus* GamepadStatusUI__ = nullptr;
	QMap<QString, RTDDeviceInfo*> DeviceListsUI__;
	RTDDeviceInfo* UserInfoUI__ = nullptr;

	QVector<QString> AllHeaders;
	QVector<QString> KernelHeaders;
	QVector<QVector<QString>> DeviceHeaders;
	QVector<QVector<QString>> DeviceSensorNames;
	QVector<QString> DeviceTypes;
	QVector<QString> ExtraHeader;

	QMap<int, QString> StateLists;
	QMap<QString, QString> KeyEventMap;

private: //const definitions
	const size_t KRNL_STATUS_LEN = 4;
	const size_t ROBOT_STATUS_LEN = 1;


};