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
#include "widget/RTDConnection.h"
#include "widget/BackendManager.h"
#include "../Communication/BitbotTcpProtocalV1.h"
#include "../Communication/MetaCommunication.hpp"
#include "../Utils/GamepadDriver/ZQGamepad.h"
#include "../Utils/Settings/SettingsHandler.h"
#include <QThread>
#include <QVariant>
#include <QVariantList>
#include <QMap>
#include <QVBoxLayout>
#include <QGraphicsBlurEffect>
#include <QProgressDialog>


class PilotPage : public ElaScrollPage
{
	Q_OBJECT

public:
	PilotPage(QWidget* parent = nullptr);
	~PilotPage();

	bool RunNewBitbot(bool LaunchBackend, bool dryrun);
	bool AutoInitBitbot(bool dryrun);

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

	void removeAllwidget(QLayout* lay);

	void ConnectionButtonClickedSlot();

protected:
	void keyPressEvent(QKeyEvent* event);
	void keyReleaseEvent(QKeyEvent* event);
	void showEvent(QShowEvent* event);
	void focusInEvent(QFocusEvent* event);
	void focusOutEvent(QFocusEvent* event);

private:
	ZQGamepad* GamepadHandle__;
	zzs::BITBOT_TCP_PROTOCAL_V1* CommHandle__;
	QThread* CommThread__;
	bool connected__ = false;
private:
	bool SurpressConnectionError__ = false;

	QGraphicsBlurEffect* blureffect__ = nullptr;
	QVBoxLayout* CentralLayout__ = nullptr;
	QVBoxLayout* ConnectedComponentLayout__ = nullptr;
	QWidget* CentralWidget__ = nullptr;

	ElaScrollPageArea* ConnectionAreaUI__ = nullptr;
	ElaSpinBox* SpinBox_Port__ = nullptr;
	ElaLineEdit* LineEdit_IP__ = nullptr;
	ElaPushButton* PushButton_Connect__ = nullptr;
	QString IP = QString("127.0.0.1");
	uint16_t port = 12888;

	BackendManager* BackendManagerUI__ = nullptr;
	RTDConnection* BackendConnectionUI__ = nullptr;
	RTDKernelStates* KernelStatusUI__ = nullptr;
	RTDRobotStates* RobotStateUI__ = nullptr;
	RTDKeyboardStatus* KeyboardEventUI__ = nullptr;
	RTDGamepadStatus* GamepadStatusUI__ = nullptr;
	QSpacerItem* horizontalSpacer = nullptr;
	QVector<RTDDeviceInfo*> DeviceListsUI__;
	RTDDeviceInfo* UserInfoUI__ = nullptr;

	QVector<QString> AllHeaders;
	QVector<QString> KernelHeaders;
	QVector<zzs::BITBOT_TCP_PROTOCAL_V1::ABSTRACT_DEVICE_HEADER> DeviceHeaders;

	QVector<QString> ExtraHeader;

	QMap<int, QString> StateLists;
	QMap<QString, QString> KeyEventMap;

private: //const definitions
	const size_t KRNL_STATUS_LEN = 4;
	const size_t ROBOT_STATUS_LEN = 1;
	const size_t AUTORUN_REFRESH_INTERVEL = 100;
private:
	void InitAutoRun();
	void AutoRunRefreshSlot();
	void CancelAutoRunSlot();
	void AutoRunSimClickButton(QString key);
	QTimer* AutoRunRefreshTimer__;
	int AutoRunNextCmdCycleRemain__ = 0;
	int AutoRunCurrentCmdIdx = 0;
	AutoRunCmdList AutoRunCmdList__;
	QProgressDialog* AutoRunDiag__ = nullptr;
};