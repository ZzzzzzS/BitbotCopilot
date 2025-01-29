#include "PilotPage.h"
#include "QVBoxLayout"
#include <QGridLayout>
#include "widget/MetaRTDView.h"
#include "widget/RTDKernelStates.h"
#include "widget/RTDDeviceInfo.h"
#include "ElaScrollPageArea.h"
#include "ElaToggleButton.h"
#include "ElaLineEdit.h"
#include "ElaPushButton.h"
#include "ElaProgressBar.h"
#include "ElaSpinBox.h"
#include <QtWidgets/QSpacerItem>
#include "../Communication/BitbotTcpProtocalV1.h"
#include <QMap>
#include <QMessageBox>
#include <iostream>
#include <QMetaType>
#include "../Utils/Settings/SettingsHandler.h"
#include <QTimer>
#include "widget/CustomImageLabel.hpp"
#include "QGraphicsBlurEffect"
#include "UI/widget/FluentMessageBox.hpp"
#include "ElaToggleSwitch.h"
#include "ElaText.h"


PilotPage::PilotPage(QWidget* parent)
{
    qRegisterMetaType<zzs::META_COMMUNICATION::CONNECTION_STATUS>("CONN_STATUS");
    this->InitCommHandle();
    this->InitUserInput();
    this->InitAutoRun();

    this->CentralWidget__ = new QWidget(this);
    this->CentralWidget__->setWindowTitle(tr(" Bitbot Nav Deck"));
    this->setPageTitleSpacing(10);
    this->addCentralWidget(this->CentralWidget__, true, false, 0);
    this->InitConnectionWidget();

    this->ProcessDisconnetced();

    this->blureffect__ = new QGraphicsBlurEffect(this);
    blureffect__->setBlurRadius(5);	//数值越大，越模糊
}

PilotPage::~PilotPage()
{
    this->CommThread__->quit();
    this->CommThread__->wait();
}

bool PilotPage::RunNewBitbot(bool LaunchBackend, bool dryrun)
{
    bool check_result = false;
    check_result = !this->connected__ && this->PushButton_Connect__->isEnabled();
    if (LaunchBackend)
        check_result = check_result && (!this->BackendManagerUI__->isRunning());


    if (!dryrun)
    {
        if (!check_result)
            return false;

        if (LaunchBackend)
        {
            this->BackendManagerUI__->StartBackend();
            this->PushButton_Connect__->setEnabled(false);
            QTimer::singleShot(2000, this, [this]() {
                std::tie(this->IP, this->port) = ZSet->getIPAndPort();
                this->LineEdit_IP__->setText(this->IP);
                this->SpinBox_Port__->setValue(this->port);
                this->ConnectionButtonClickedSlot();
                });
        }
        else
        {
            this->ConnectionButtonClickedSlot();
        }

    }
    return check_result;
}

bool PilotPage::AutoInitBitbot(bool dryrun)
{
    bool AllowRun = false;
    AllowRun = this->RunNewBitbot(true, true) && this->AutoRunCmdList__.size() != 0;
    if (!AllowRun) return false;
    if (dryrun) return AllowRun;

    this->RunNewBitbot(true, false);
    this->SurpressConnectionError__ = true;
    this->AutoRunCurrentCmdIdx = -1;
    this->AutoRunNextCmdCycleRemain__ = 4000 / this->AUTORUN_REFRESH_INTERVEL; //首个指令延时4秒再执行
    this->AutoRunDiag__ = new QProgressDialog(tr("Launching new Bitbot backend..."), tr("Cancel"), 0, 0, this);
    this->AutoRunDiag__->setAutoClose(false);
    //this->AutoRunDiag__->setBar(new ElaProgressBar(this->AutoRunDiag__));
    this->AutoRunDiag__->setAttribute(Qt::WA_DeleteOnClose);
    this->AutoRunDiag__->setAttribute(Qt::WA_ShowModal);
    this->AutoRunDiag__->setWindowFlags(this->AutoRunDiag__->windowFlags() & ~(
        Qt::WindowCloseButtonHint
        | Qt::WindowMinMaxButtonsHint
        | Qt::WindowContextHelpButtonHint));
    QPushButton* cancel_button = new QPushButton(this->AutoRunDiag__);
    cancel_button->setText(tr("Cancel"));
    //cancel_button->setEnabled(false); //取消的逻辑太复杂了，暂时先不让取消了
    this->AutoRunDiag__->setCancelButton(cancel_button);
    this->AutoRunDiag__->show();

    QObject::connect(cancel_button, &QPushButton::clicked, this, [this]() {
        qDebug() << "auto run cancel button clicked";
        this->CancelAutoRunSlot();
        });



    this->AutoRunRefreshTimer__->start();
    return true;
}

zzs::BITBOT_TCP_PROTOCAL_V1* PilotPage::getCommHandle()
{
    return this->CommHandle__;
}


void PilotPage::InitConnectionWidget()
{
    std::tie(this->IP, this->port) = ZSet->getIPAndPort();

    this->ConnectionAreaUI__ = new ElaScrollPageArea(this->CentralWidget__);
    this->ConnectionAreaUI__->setSizePolicy(QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Fixed);

    QFont font;
    font.setPointSize(10);

    auto gridLayout = new QGridLayout(this->ConnectionAreaUI__);
    gridLayout->setSpacing(6);
    gridLayout->setContentsMargins(11, 11, 11, 11);
    gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
    auto horizontalLayout_3 = new QHBoxLayout();
    horizontalLayout_3->setSpacing(6);
    horizontalLayout_3->setObjectName(QString::fromUtf8("horizontalLayout_3"));
    auto horizontalSpacer_2 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

    ElaText* icon = new ElaText(this->ConnectionAreaUI__);
    QFont iconFont = icon->font();
    iconFont.setFamily("ElaAwesome");
    iconFont.setPixelSize(28);
    icon->setFixedSize(40, 40);
    icon->setText(QChar(ElaIconType::Computer));
    icon->setFont(iconFont);
    icon->setAlignment(Qt::AlignCenter);
    //icon->setPixmap(QPixmap(":/UI/Image/frontend_icon.png"));
    //icon->setScaledContents(true);
    ElaText* name = new ElaText(this->ConnectionAreaUI__);
    name->setText(tr("Frontend Manager"));
    QFont namefont;
    namefont.setPixelSize(16);
    name->setFont(namefont);
    name->setAlignment(Qt::AlignLeft | Qt::AlignBottom);

    ElaText* subname = new ElaText(this->ConnectionAreaUI__);
    subname->setAlignment(Qt::AlignLeft | Qt::AlignTop);
    subname->setWordWrap(false);
    subname->setSizePolicy(QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Preferred);
    subname->setText(tr("Instantly access your robot states from remote"));
    QFont subnamefont;
    subnamefont.setPixelSize(10);
    subname->setFont(subnamefont);

    auto LamsetTheme = [subname](ElaThemeType::ThemeMode Mode) {
        QPalette LabelPalette;
        if (Mode == ElaThemeType::ThemeMode::Dark)
        {
            QBrush brush_subLabel(QColor(206, 206, 206, 255));
            brush_subLabel.setStyle(Qt::SolidPattern);
            LabelPalette.setBrush(QPalette::Active, QPalette::WindowText, brush_subLabel);
            LabelPalette.setBrush(QPalette::Inactive, QPalette::WindowText, brush_subLabel);
        }
        else
        {
            QBrush brush_subLabel(QColor(55, 55, 55, 255));
            brush_subLabel.setStyle(Qt::SolidPattern);
            LabelPalette.setBrush(QPalette::Active, QPalette::WindowText, brush_subLabel);
            LabelPalette.setBrush(QPalette::Inactive, QPalette::WindowText, brush_subLabel);
        }
        subname->setPalette(LabelPalette);
        };
    LamsetTheme(eTheme->getThemeMode());
    QObject::connect(eTheme, &ElaTheme::themeModeChanged, this, LamsetTheme);

    QVBoxLayout* iconLayout = new QVBoxLayout();
    iconLayout->addWidget(name);
    iconLayout->addWidget(subname);

    horizontalLayout_3->addSpacerItem(new QSpacerItem(10, 10, QSizePolicy::Fixed, QSizePolicy::Minimum));
    horizontalLayout_3->addWidget(icon);
    horizontalLayout_3->addSpacerItem(new QSpacerItem(10, 10, QSizePolicy::Fixed, QSizePolicy::Minimum));
    horizontalLayout_3->addLayout(iconLayout);

    horizontalLayout_3->addItem(horizontalSpacer_2);

    auto horizontalLayout_2 = new QHBoxLayout();
    horizontalLayout_2->setSpacing(6);
    horizontalLayout_2->setObjectName(QString::fromUtf8("horizontalLayout_2"));
    auto horizontalLayout = new QHBoxLayout();
    horizontalLayout->setSpacing(6);
    horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
    auto label = new ElaText(this->ConnectionAreaUI__);
    label->setObjectName(QString::fromUtf8("label"));
    label->setText(tr("IP:"));
    label->setFont(font);

    horizontalLayout->addWidget(label);

    this->LineEdit_IP__ = new ElaLineEdit(this->ConnectionAreaUI__);
    this->LineEdit_IP__->setFixedHeight(30);
    this->LineEdit_IP__->setFixedWidth(200);
    this->LineEdit_IP__->setFont(font);
    this->LineEdit_IP__->setAlignment(Qt::AlignCenter);
    this->LineEdit_IP__->setText(this->IP);

    this->LineEdit_IP__->setObjectName(QString::fromUtf8("lineEdit_IP"));
    this->LineEdit_IP__->setEnabled(true);
    QSizePolicy sizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
    sizePolicy.setHorizontalStretch(0);
    sizePolicy.setVerticalStretch(0);
    sizePolicy.setHeightForWidth(this->LineEdit_IP__->sizePolicy().hasHeightForWidth());
    this->LineEdit_IP__->setSizePolicy(sizePolicy);

    horizontalLayout->addWidget(this->LineEdit_IP__);

    auto label_2 = new ElaText(this->ConnectionAreaUI__);
    label_2->setObjectName(QString::fromUtf8("label_2"));
    label_2->setText("          " + tr("Port:"));
    label_2->setFont(font);
    label_2->setWordWrap(false);

    horizontalLayout->addWidget(label_2);

    this->SpinBox_Port__ = new ElaSpinBox(this->ConnectionAreaUI__);
    this->SpinBox_Port__->setMaximum(65535);
    this->SpinBox_Port__->setMinimum(1);
    this->SpinBox_Port__->setObjectName(QString::fromUtf8("spinBox_Port"));
    this->SpinBox_Port__->setFont(font);
    this->SpinBox_Port__->setValue(this->port);

    horizontalLayout->addWidget(this->SpinBox_Port__);


    horizontalLayout_2->addLayout(horizontalLayout);

    auto horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Preferred, QSizePolicy::Minimum);

    horizontalLayout_2->addItem(horizontalSpacer);

    this->PushButton_Connect__ = new ElaPushButton(this->ConnectionAreaUI__);
    this->PushButton_Connect__->setBorderRadius(5);
    this->PushButton_Connect__->setObjectName(QString::fromUtf8("pushButton_Connect"));
    this->PushButton_Connect__->setFont(font);
    this->PushButton_Connect__->setText(tr("connect"));

    QColor ButtonLightDefaultColor(eTheme->getThemeColor(ElaThemeType::ThemeMode::Light, ElaThemeType::ThemeColor::PrimaryNormal));
    QColor ButtonLightHoverColor(eTheme->getThemeColor(ElaThemeType::ThemeMode::Light, ElaThemeType::ThemeColor::PrimaryHover));
    QColor ButtonLightPressColor(eTheme->getThemeColor(ElaThemeType::ThemeMode::Light, ElaThemeType::ThemeColor::PrimaryPress));
    QColor ButtonLightTextColor(255, 255, 255);

    QColor ButtonDarkDefaultColor(eTheme->getThemeColor(ElaThemeType::ThemeMode::Dark, ElaThemeType::ThemeColor::PrimaryNormal));
    QColor ButtonDarkHoverColor(eTheme->getThemeColor(ElaThemeType::ThemeMode::Dark, ElaThemeType::ThemeColor::PrimaryHover));
    QColor ButtonDarkPressColor(eTheme->getThemeColor(ElaThemeType::ThemeMode::Dark, ElaThemeType::ThemeColor::PrimaryPress));
    QColor ButtonDarkTextColor(0, 0, 0);

    this->PushButton_Connect__->setDarkDefaultColor(ButtonDarkDefaultColor);
    this->PushButton_Connect__->setDarkHoverColor(ButtonDarkHoverColor);
    this->PushButton_Connect__->setDarkPressColor(ButtonDarkPressColor);
    this->PushButton_Connect__->setDarkTextColor(ButtonDarkTextColor);
    this->PushButton_Connect__->setLightDefaultColor(ButtonLightDefaultColor);
    this->PushButton_Connect__->setLightHoverColor(ButtonLightHoverColor);
    this->PushButton_Connect__->setLightPressColor(ButtonLightPressColor);
    this->PushButton_Connect__->setLightTextColor(ButtonLightTextColor);

    this->PushButton_Connect__->setFixedWidth(100);

    horizontalLayout_2->addWidget(this->PushButton_Connect__);


    horizontalLayout_3->addLayout(horizontalLayout_2);

    auto horizontalSpacer_3 = new QSpacerItem(40, 20, QSizePolicy::Preferred, QSizePolicy::Minimum);

    horizontalLayout_3->addItem(horizontalSpacer_3);


    gridLayout->addLayout(horizontalLayout_3, 0, 0, 1, 1);

    QObject::connect(this->PushButton_Connect__, &ElaPushButton::clicked, this, &PilotPage::ConnectionButtonClickedSlot, Qt::QueuedConnection);

    /////////////////backend///////////////////////////////////
    this->BackendManagerUI__ = new BackendManager(this->CentralWidget__);
    QHBoxLayout* hor_box = new QHBoxLayout();
    hor_box->addWidget(this->BackendManagerUI__);

    this->CentralLayout__ = new QVBoxLayout(this->CentralWidget__);
    this->CentralLayout__->addWidget(this->ConnectionAreaUI__);
    this->CentralLayout__->addLayout(hor_box);
    QSpacerItem* B_space = new QSpacerItem(20, 20, QSizePolicy::Minimum, QSizePolicy::Expanding);
    this->CentralLayout__->addSpacerItem(B_space);
}

void PilotPage::InitCommHandle()
{
    this->CommThread__ = new QThread(this);
    this->CommHandle__ = new zzs::BITBOT_TCP_PROTOCAL_V1(QJsonObject(), nullptr);
    this->CommHandle__->moveToThread(this->CommThread__);
    QObject::connect(this->CommHandle__, &zzs::BITBOT_TCP_PROTOCAL_V1::ConnectionStateChanged, this, [this](int status__) {
        zzs::META_COMMUNICATION::CONNECTION_STATUS status = static_cast<zzs::META_COMMUNICATION::CONNECTION_STATUS>(status__);
        switch (status)
        {
        case zzs::META_COMMUNICATION::CONNECTION_STATUS::CONNECTED:
            this->ProcessConnected();
            break;
        case zzs::META_COMMUNICATION::CONNECTION_STATUS::DISCONNECT:
            this->ProcessDisconnetced();
            break;
        case zzs::META_COMMUNICATION::CONNECTION_STATUS::ERRORED:
            this->ProcessConnectionError();
            break;
        default:
            break;
        }
        }, Qt::QueuedConnection);

    QObject::connect(this->CommHandle__, &zzs::META_COMMUNICATION::ReceivedPDO, this, &PilotPage::ProcessPDO, Qt::QueuedConnection);

    this->CommThread__->start();
}
void PilotPage::InitUserInput()
{
    this->GamepadHandle__ = new ZQGamepad(125, this);
    this->GamepadHandle__->start();

    QObject::connect(this->GamepadHandle__, &ZQGamepad::ConnectionChanged, this, [this](int id, bool Connected) {
        if (this->GamepadStatusUI__ != nullptr)
        {
            this->GamepadStatusUI__->DeviceConnectionChanged(id, Connected);
            if (Connected)
                this->GamepadHandle__->SetRumble(id, 65535, 65535, 500);
        }
        }, Qt::QueuedConnection);


    QObject::connect(this->GamepadHandle__, &ZQGamepad::JoystickMoved, this, [this](int id, Q_XSX_JOYSTICK_ENUM axis, float value) {
        QString JoystickName;
        JoystickName = this->GamepadHandle__->name(axis);

        if (this->GamepadStatusUI__ != nullptr)
        {
            this->GamepadStatusUI__->JoystickMoved(id, axis, value);
        }

        if (!this->KeyEventMap.contains(JoystickName))
            return;

        QVariantMap map;
        map.insert(this->KeyEventMap[JoystickName], QVariant(value));
        this->CommHandle__->SendUserCommand(map);
        }, Qt::QueuedConnection);


    QObject::connect(this->GamepadHandle__, &ZQGamepad::ButtonClicked, this, [this](int id, Q_XSX_JOYSTICK_ENUM button, bool ButtonState) {
        QString ButtonName;
        ButtonName = this->GamepadHandle__->name(button);
        if (!this->KeyEventMap.contains(ButtonName))
            return;

        QVariantMap map;
        map.insert(this->KeyEventMap[ButtonName], QVariant(ButtonState ? 1 : 2));
        this->CommHandle__->SendUserCommand(map); //TODO: change to boundle command send to save bandwidth and reduce workload.


        if (this->KeyboardEventUI__ != nullptr)
        {
            this->KeyboardEventUI__->ButtonClicked(ButtonName, ButtonState);
        }
        }, Qt::QueuedConnection);
}
void PilotPage::DrawConnectedUI()
{
    this->PushButton_Connect__->setText(tr("Disconnect"));
    this->PushButton_Connect__->setEnabled(true);

    if (this->ConnectedComponentLayout__ == nullptr)
    {
        this->ConnectedComponentLayout__ = new QVBoxLayout();
        QHBoxLayout* hor_box = new QHBoxLayout();
        hor_box->addLayout(this->ConnectedComponentLayout__);

        //this->CentralLayout__->addLayout(hor_box);
        this->CentralLayout__->insertLayout(1, hor_box);
    }


    QHBoxLayout* lay = new QHBoxLayout();

    //if (this->BackendConnectionUI__ == nullptr)
    //{
    //    this->BackendConnectionUI__=new 
    //}

    if (this->horizontalSpacerleft == nullptr)
    {
        this->horizontalSpacerleft = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);
        lay->addItem(this->horizontalSpacerleft);
    }

    if (this->KernelStatusUI__ == nullptr)
    {
        this->KernelStatusUI__ = new RTDKernelStates(this->CentralWidget__);
        this->KernelStatusUI__->setAttribute(Qt::WA_DeleteOnClose);
        lay->addWidget(this->KernelStatusUI__);
    }
    else
    {
        this->KernelStatusUI__->show();
    }

    if (this->RobotStateUI__ == nullptr)
    {
        this->RobotStateUI__ = new RTDRobotStates(this->CentralWidget__);
        this->RobotStateUI__->setAttribute(Qt::WA_DeleteOnClose);
        lay->addWidget(this->RobotStateUI__);
    }
    else
    {
        this->RobotStateUI__->show();
    }
    this->RobotStateUI__->SetHeaderList(this->StateLists);

    if (this->KeyboardEventUI__ == nullptr)
    {
        this->KeyboardEventUI__ = new RTDKeyboardStatus(this->CentralWidget__);
        this->KeyboardEventUI__->setAttribute(Qt::WA_DeleteOnClose);
        lay->addWidget(this->KeyboardEventUI__);
    }
    else
    {
        this->KeyboardEventUI__->show();
    }

    if (this->GamepadStatusUI__ == nullptr)
    {
        size_t GamepadCnt = this->GamepadHandle__->CountConnectedGamepad();
        this->GamepadStatusUI__ = new RTDGamepadStatus(GamepadCnt, this->CentralWidget__);
        this->GamepadStatusUI__->setAttribute(Qt::WA_DeleteOnClose);
        lay->addWidget(this->GamepadStatusUI__);
    }
    else
    {
        this->GamepadStatusUI__->show();
    }

    if (this->horizontalSpacerright == nullptr)
    {
        this->horizontalSpacerright = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);
        lay->addItem(this->horizontalSpacerright);
    }



    QVBoxLayout* DeviceLayout = new QVBoxLayout();

    if (this->RTC_RTDSelectorUI__ == nullptr)
    {
        this->RTC_RTDSelectorUI__ = new QWidget(this->CentralWidget__);
        auto RTC_RTDText = new ElaText(this->RTC_RTDSelectorUI__);
        RTC_RTDText->setText(tr("Graph Mode "));
        RTC_RTDText->setAlignment(Qt::AlignRight | Qt::AlignTop);
        RTC_RTDText->setTextStyle(ElaTextType::Body);
        RTC_RTDSelectorSwitch__ = new ElaToggleSwitch(this->RTC_RTDSelectorUI__);
        QHBoxLayout* RTC_RTDLayout = new QHBoxLayout();
        RTC_RTDLayout->addSpacerItem(new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum));
        RTC_RTDLayout->addWidget(RTC_RTDText);
        RTC_RTDLayout->addWidget(RTC_RTDSelectorSwitch__);
        RTC_RTDLayout->addSpacerItem(new QSpacerItem(20, 20, QSizePolicy::Fixed, QSizePolicy::Minimum));
        this->RTC_RTDSelectorUI__->setLayout(RTC_RTDLayout);
        QObject::connect(this->RTC_RTDSelectorSwitch__, &ElaToggleSwitch::toggled, this, &PilotPage::ShowGraphButtonToggled);
        DeviceLayout->addWidget(this->RTC_RTDSelectorUI__);

        this->WarperRealTimeDataViewer__ = new MetaRTDView(MetaRTDView::RTDViewType::EXTEND_WINDOW, this->CentralWidget__);
        this->RealTimeDataViewer__ = new DataViewerPage(this->WarperRealTimeDataViewer__, true);
        QGridLayout* WarpergridLayout = new QGridLayout(this->WarperRealTimeDataViewer__);
        WarpergridLayout->addWidget(this->RealTimeDataViewer__);

        DeviceLayout->addWidget(this->WarperRealTimeDataViewer__);
        this->WarperRealTimeDataViewer__->hide();
    }
    else
    {
        this->WarperRealTimeDataViewer__->hide();
        this->RTC_RTDSelectorUI__->show();
        this->RTC_RTDSelectorSwitch__->setIsToggled(false);
    }

    {
        QStringList DeviceGroup;
        QVector<QStringList> DeviceSensors;
        for (auto& i : this->DeviceHeaders)
        {
            DeviceGroup.push_back(i.DeviceName);
            QStringList sensors;
            for (auto& j : i.SensorsName)
            {
                sensors.push_back(j);
            }
            DeviceSensors.push_back(sensors);
        }
        DeviceGroup.push_back("User Info");
        DeviceSensors.push_back(this->ExtraHeader);

        this->RealTimeDataViewer__->SetUpRealTimeHeader(DeviceGroup, DeviceSensors);
    }


    QMap<QString, QVector<QString>> DeviceSensors;
    QMap<QString, QVector<QString>> DeviceTypeList;

    for (auto& i : this->DeviceHeaders)
    {
        if (!DeviceSensors.contains(i.DeviceTypeName))
        {
            DeviceSensors.insert(i.DeviceTypeName, i.SensorsName);
        }

        if (!DeviceTypeList.contains(i.DeviceTypeName))
        {
            DeviceTypeList.insert(i.DeviceTypeName, QVector<QString>());
        }
        DeviceTypeList[i.DeviceTypeName].push_back(i.DeviceName);
    }

    QMap<QString, QVector<QString>>::const_iterator i = DeviceSensors.constBegin();
    size_t cnt = 0;
    size_t ori_dev_size = this->DeviceListsUI__.size();
    while (i != DeviceSensors.constEnd()) {
        RTDDeviceInfo* wid;

        if (cnt < ori_dev_size)
        {
            wid = this->DeviceListsUI__[cnt];
            wid->show();
        }
        else
        {
            wid = new RTDDeviceInfo(this->CentralWidget__);
            wid->setAttribute(Qt::WA_DeleteOnClose);
            DeviceLayout->addWidget(wid);
            this->DeviceListsUI__.push_back(wid);
        }

        QString DeviceTypeName = i.key();
        QVector<QString> DeviceList = DeviceTypeList[i.key()];
        QVector<QString> DeviceSensors = i.value();
        wid->setHeaders(DeviceTypeName, DeviceList, DeviceSensors);

        ++i;
        cnt++;
    }

    if (this->UserInfoUI__ == nullptr)
    {
        this->UserInfoUI__ = new RTDDeviceInfo(this->CentralWidget__);
        this->UserInfoUI__->setAttribute(Qt::WA_DeleteOnClose);
        this->UserInfoUI__->setHeaders(this->ExtraHeader);
        DeviceLayout->addWidget(this->UserInfoUI__);
    }
    else
    {
        this->UserInfoUI__->setHeaders(this->ExtraHeader);
        this->UserInfoUI__->show();
    }

    this->ConnectedComponentLayout__->addLayout(lay);
    this->ConnectedComponentLayout__->addLayout(DeviceLayout);

}

void PilotPage::ShowGraphButtonToggled(bool checked)
{
    if (checked)
    {
        this->WarperRealTimeDataViewer__->show();
        this->UserInfoUI__->hide();
        this->WarperRealTimeDataViewer__->setMinimumHeight(630);
        for (auto i : this->DeviceListsUI__)
        {
            i->hide();
        }
    }
    else
    {
        this->WarperRealTimeDataViewer__->hide();
        this->UserInfoUI__->show();
        for (auto i : this->DeviceListsUI__)
        {
            i->show();
        }
    }
}

void PilotPage::DrawDisconnectedUI()
{
    if (this->KernelStatusUI__ != nullptr)
    {
        this->KernelStatusUI__->hide();
        this->KernelStatusUI__->ResetUI();
    }

    if (this->RobotStateUI__ != nullptr)
    {
        this->RobotStateUI__->hide();
        this->RobotStateUI__->ResetUI();
    }

    if (this->KeyboardEventUI__ != nullptr)
    {
        this->KeyboardEventUI__->hide();
        this->KeyboardEventUI__->ResetUI();
    }

    if (this->GamepadStatusUI__ != nullptr)
    {
        this->GamepadStatusUI__->hide();
        this->GamepadStatusUI__->ResetUI();
    }

    if (this->RTC_RTDSelectorUI__ != nullptr)
    {
        this->RTC_RTDSelectorUI__->hide();
    }

    if (this->WarperRealTimeDataViewer__ != nullptr)
    {
        this->WarperRealTimeDataViewer__->hide();
        this->RealTimeDataViewer__->ResetRealTimeUI();
        this->RTC_RTDSelectorSwitch__->setIsToggled(false);
    }
    qApp->processEvents();

    for (auto& i : this->DeviceListsUI__)
    {
        if (i != nullptr)
        {
            i->hide();
            i->ResetUI();
        }
    }
    qApp->processEvents();

    if (this->UserInfoUI__ != nullptr)
    {
        this->UserInfoUI__->hide();
        this->UserInfoUI__->ResetUI();
    }
    qApp->processEvents();

    qApp->processEvents();

    //reset ui
    this->PushButton_Connect__->setText(tr("Connect"));
    this->PushButton_Connect__->setEnabled(true);
}

void PilotPage::ProcessDisconnetced()
{
    this->connected__ = false;
    this->AllHeaders.clear();
    this->KernelHeaders.clear();
    this->DeviceHeaders.clear();
    this->ExtraHeader.clear();
    this->StateLists.clear();
    this->KeyEventMap.clear();

    this->DrawDisconnectedUI();
}
void PilotPage::ProcessConnected()
{
    this->connected__ = true;

    this->KeyEventMap.clear();
    this->CommHandle__->getAvailableKeys(this->KeyEventMap, false);

    this->StateLists.clear();
    this->CommHandle__->getStateList(this->StateLists);

    this->KernelHeaders.clear();
    this->CommHandle__->getKernelHeader(this->KernelHeaders);

    this->ExtraHeader.clear();
    this->CommHandle__->getExtraHeader(this->ExtraHeader);

    this->DeviceHeaders.clear();
    this->CommHandle__->getDeviceHeader(this->DeviceHeaders);


    this->AllHeaders.clear();
    this->CommHandle__->getDataHeaderAll(this->AllHeaders);

    auto pads = this->GamepadHandle__->getAvailableGamepadID();
    for (auto& pad : pads)
    {
        bool ok = this->GamepadHandle__->SetRumble(pad, 65535, 65535, 500);
        qDebug() << ok;
    }

    this->DrawConnectedUI();
    this->setFocus();
}
void PilotPage::ProcessConnectionError()
{
    this->connected__ = false;
    if (this->RobotStateUI__ != nullptr && this->RobotStateUI__->CurrentState() == QString("kernel_stopped"))
        this->SurpressConnectionError__ = true;
    this->DrawDisconnectedUI();
    if (this->SurpressConnectionError__)
        this->SurpressConnectionError__ = false;
    else
        FluentMessageBox::warningOk(this, tr("Connection Failed"), tr("Failed to connected to BitBot, please check you network connection!"), QMessageBox::Ok);
}

void PilotPage::ProcessPDO(QVariantList PDOInfo)
{
    size_t HeaderLength = this->AllHeaders.size();

    if (this->RobotStateUI__ != nullptr)
    {
        this->RobotStateUI__->UpdateStates(PDOInfo[0]);
    }
    PDOInfo = QVariantList(PDOInfo.begin() + 1, PDOInfo.end());

    if (this->KernelStatusUI__ != nullptr)
    {
        this->KernelStatusUI__->UpdateKernelStatus(QString("kernel"), QVariantList(PDOInfo.begin(), PDOInfo.begin() + 4));
    }
    PDOInfo = QVariantList(PDOInfo.begin() + 4, PDOInfo.end());

    QMap<QString, QVariantList> RemapPDOInfo;


    for (size_t i = 0; i < this->DeviceHeaders.size(); i++)
    {
        //for table mode
        QString CurrentTypeName = this->DeviceHeaders[i].DeviceTypeName;
        if (!RemapPDOInfo.contains(CurrentTypeName))
        {
            RemapPDOInfo.insert(CurrentTypeName, QVariantList());
        }
        size_t len = this->DeviceHeaders[i].SensorsName.size();
        for (size_t j = 0; j < len; j++)
        {
            RemapPDOInfo[CurrentTypeName].push_back(PDOInfo[j]);
        }

        QString DeviceGroupName;
        QStringList DeviceSensorsName;
        QVector<double> DeviceSensorsValue;
        //for graph mode
        DeviceGroupName = this->DeviceHeaders[i].DeviceName;
        for (size_t j = 0;j < this->DeviceHeaders[i].SensorsName.size();j++)
        {
            DeviceSensorsName.push_back(this->DeviceHeaders[i].SensorsName[j]);
            DeviceSensorsValue.push_back(PDOInfo[j].toDouble());
        }
        this->RealTimeDataViewer__->UpdateRealTimeData(DeviceGroupName, DeviceSensorsName, DeviceSensorsValue);

        PDOInfo = QVariantList(PDOInfo.begin() + len, PDOInfo.end());
    }

    for (auto& ui : this->DeviceListsUI__)
    {
        size_t DataSize = ui->getHeaderSize();
        QString name = ui->DeviceTypeName();
        QVariantList val = RemapPDOInfo[name];
        ui->UpdateDeviceInfo(val);
    }

    if (this->UserInfoUI__ != nullptr)
    {
        if (size_t UserDataSize = this->UserInfoUI__->getHeaderSize();UserDataSize != PDOInfo.size())
        {
            std::cout << "PDO data size mismatch with header, receive failed!\n";
        }
        else
        {
            this->UserInfoUI__->UpdateDeviceInfo(PDOInfo);

            //for graph mode
            QVector<double> UserSensorsValue;
            for (size_t i = 0; i < UserDataSize; i++)
            {
                UserSensorsValue.push_back(PDOInfo[i].toDouble());
            }
            this->RealTimeDataViewer__->UpdateRealTimeData("User Info", this->ExtraHeader, UserSensorsValue);

        }
    }
    this->RealTimeDataViewer__->RefreshRealTimeData(); //refresh graph mode data
}

void PilotPage::removeAllwidget(QLayout* lay)
{
    qDebug() << "remove!";
    if (lay) {
        // auto tmp = lay->children();

        while (lay->count() > 0) {
            qDebug() << lay->count();

            QLayout* subLay = lay->itemAt(0)->layout();
            if (subLay) {
                this->removeAllwidget(subLay);    // 子布局递归调用
            }

            QWidget* wgt = lay->itemAt(0)->widget();
            if (wgt) {
                lay->removeWidget(wgt);
                wgt->setParent(nullptr);
                wgt->deleteLater();
                wgt = nullptr;
            }
        }

        lay->deleteLater();
        lay = nullptr;
    }
}

void PilotPage::ConnectionButtonClickedSlot()
{
    this->IP = this->LineEdit_IP__->text();
    this->port = this->SpinBox_Port__->value();

    if (this->PushButton_Connect__->text() == tr("Connect"))
    {
        if (bool ok = this->CommHandle__->Connect(IP, port, 3000); ok == false)
        {
            FluentMessageBox::warningOk(this, tr("Connection Failed"), tr("Failed to connected to BitBot, please check you network connection!"), QMessageBox::Ok);
        }
        else
        {
            this->PushButton_Connect__->setText(tr("connecting"));
            this->PushButton_Connect__->setEnabled(false);
        }
    }
    else if (this->PushButton_Connect__->text() == tr("Disconnect"))
    {
        if (!this->CommHandle__->Disconnect())
        {
            FluentMessageBox::warningOk(this, tr("Disconnect Failed"), tr("Failed to disconnect to Bitbot, try again later"), QMessageBox::Ok);
        }
    }
    else if (this->PushButton_Connect__->text() == tr("connecting"))
    {
        FluentMessageBox::warningOk(this, tr("System is Busy"), tr("System is busy, connect/disconnect request will be ignored"));
    }
    else
    {
        FluentMessageBox::criticalOk(this, tr("Catastrophic Failure"), tr("Unknown Button Status"), QMessageBox::Ok);
    }
}

void PilotPage::keyPressEvent(QKeyEvent* event)
{
    if (!this->connected__)
        return;
    //TODO: add key allow check
    QString key = event->text();
    qDebug() << key << " is pressed";


    if (!this->KeyEventMap.contains(key))
        return;

    QVariantMap map;
    map.insert(this->KeyEventMap[key], QVariant(1));
    this->CommHandle__->SendUserCommand(map);
    if (this->KeyboardEventUI__ != nullptr)
    {
        this->KeyboardEventUI__->ButtonClicked(key, true);
    }
}

void PilotPage::keyReleaseEvent(QKeyEvent* event)
{
    if (!this->connected__)
        return;
    //TODO: add key allow check
    QString key = event->text();
    qDebug() << key << " is released";

    if (!this->KeyEventMap.contains(key))
        return;

    QVariantMap map;
    map.insert(this->KeyEventMap[key], QVariant(2));
    this->CommHandle__->SendUserCommand(map);
    if (this->KeyboardEventUI__ != nullptr)
    {
        this->KeyboardEventUI__->ButtonClicked(key, false);
    }
}

void PilotPage::showEvent(QShowEvent* event)
{
    ElaScrollPage::showEvent(event);
    this->setFocus();
    this->BackendManagerUI__->ResetUI();
}

void PilotPage::focusInEvent(QFocusEvent* event)
{
    /*this->setGraphicsEffect(nullptr);
    qDebug() << "forces in";*/
}

void PilotPage::focusOutEvent(QFocusEvent* event)
{
    //this->blureffect__ = new QGraphicsBlurEffect(this);
    //blureffect__->setBlurRadius(5);	//数值越大，越模糊
    //this->setGraphicsEffect(this->blureffect__);
    //qDebug() << "focus out";
}

void PilotPage::InitAutoRun()
{
    this->AutoRunCmdList__ = ZSet->getAutoRunCommandList();
    this->AutoRunRefreshTimer__ = new QTimer(this);
    this->AutoRunRefreshTimer__->setInterval(this->AUTORUN_REFRESH_INTERVEL);
    QObject::connect(this->AutoRunRefreshTimer__, &QTimer::timeout, this, &PilotPage::AutoRunRefreshSlot);
}

void PilotPage::AutoRunRefreshSlot()
{

    if (!this->connected__)
    {
        if (this->AutoRunNextCmdCycleRemain__ == 0 && this->AutoRunCurrentCmdIdx == -1)
        {
            this->CancelAutoRunSlot();
            qApp->processEvents();
            FluentMessageBox::warningOk(this, tr("Failed to Auto Initialize Bitbot"), tr("Failed to auto initialize Bitbot, check your connection and try again later."), QMessageBox::Ok);
            return;
        }
        else
        {
            this->AutoRunNextCmdCycleRemain__--;
            return;
        }
    }

    QString state = this->RobotStateUI__->CurrentState();
    if (state == QString("Disconnected"))
        return;

    this->AutoRunNextCmdCycleRemain__--;
    if (this->AutoRunCurrentCmdIdx == -1)
    {
        if (this->AutoRunNextCmdCycleRemain__ == 0)
        {
            this->AutoRunCurrentCmdIdx = 0;
            this->AutoRunDiag__->setMinimum(-1);
            this->AutoRunDiag__->setMaximum(this->AutoRunCmdList__.size());
            this->AutoRunDiag__->setValue(this->AutoRunCurrentCmdIdx);
            this->AutoRunDiag__->setLabelText(QString(tr("Processing Command: ") + this->AutoRunCmdList__[this->AutoRunCurrentCmdIdx].KeyName + tr("...")));
            qApp->processEvents();
            this->AutoRunSimClickButton(this->AutoRunCmdList__[this->AutoRunCurrentCmdIdx].KeyName);
            qApp->processEvents();
            this->AutoRunNextCmdCycleRemain__ = this->AutoRunCmdList__[this->AutoRunCurrentCmdIdx].WaitTime / this->AUTORUN_REFRESH_INTERVEL;
        }
    }
    else
    {
        if (this->AutoRunNextCmdCycleRemain__ == 0 || this->AutoRunCmdList__[this->AutoRunCurrentCmdIdx].WaitUntil == state)
        {
            this->AutoRunCurrentCmdIdx++;
            if (this->AutoRunCurrentCmdIdx >= this->AutoRunCmdList__.size())
            {
                this->SurpressConnectionError__ = false;
                if (this->AutoRunDiag__ != nullptr)
                {
                    this->AutoRunDiag__->close();
                    this->AutoRunDiag__->deleteLater();
                    this->AutoRunDiag__ = nullptr;
                }
                this->AutoRunRefreshTimer__->stop();
                qApp->processEvents();
                this->setFocus();
            }
            else
            {
                this->AutoRunDiag__->setValue(this->AutoRunCurrentCmdIdx);
                this->AutoRunDiag__->setLabelText(QString(tr("Processing Command: ") + this->AutoRunCmdList__[this->AutoRunCurrentCmdIdx].KeyName + tr("...")));
                qApp->processEvents();
                this->AutoRunSimClickButton(this->AutoRunCmdList__[this->AutoRunCurrentCmdIdx].KeyName);
                qApp->processEvents();
                this->AutoRunNextCmdCycleRemain__ = this->AutoRunCmdList__[this->AutoRunCurrentCmdIdx].WaitTime / this->AUTORUN_REFRESH_INTERVEL;
            }
        }
    }
}

void PilotPage::CancelAutoRunSlot()
{
    this->AutoRunRefreshTimer__->stop();
    if (this->AutoRunDiag__ != nullptr)
    {
        this->AutoRunDiag__->close();
        this->AutoRunDiag__->deleteLater();
        qApp->processEvents();
        this->AutoRunDiag__ = nullptr;
    }
    if (this->connected__ &&
        this->RobotStateUI__ != nullptr &&
        this->RobotStateUI__->CurrentState() != QString("Disconnected"))
    {
        this->SurpressConnectionError__ = true;
        this->AutoRunSimClickButton(" ");
    }
    else
    {
        this->BackendManagerUI__->TerminateBackend();
    }


}

void PilotPage::AutoRunSimClickButton(QString key)
{
    if (!this->connected__)
        return;
    if (!this->KeyEventMap.contains(key))
        return;


    QTimer::singleShot(0, [this, key]() {
        qDebug() << key << " is pressed";
        QVariantMap map;
        map.insert(this->KeyEventMap[key], QVariant(1));
        this->CommHandle__->SendUserCommand(map);
        if (this->KeyboardEventUI__ != nullptr)
        {
            this->KeyboardEventUI__->ButtonClicked(key, true);
        }
        });
    qApp->processEvents();


    QTimer::singleShot(110, [this, key]() {
        qDebug() << key << " is released";

        if (!this->KeyEventMap.contains(key))
            return;

        QVariantMap map;
        map.insert(this->KeyEventMap[key], QVariant(2));
        this->CommHandle__->SendUserCommand(map);
        if (this->KeyboardEventUI__ != nullptr)
        {
            this->KeyboardEventUI__->ButtonClicked(key, false);
        }
        });

}

