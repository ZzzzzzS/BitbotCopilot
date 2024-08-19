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



PilotPage::PilotPage(QWidget* parent)
{
    qRegisterMetaType<zzs::META_COMMUNICATION::CONNECTION_STATUS>("CONN_STATUS");
    this->InitCommHandle();
    this->InitUserInput();

    this->CentralWidget__ = new QWidget(this);
    this->CentralWidget__->setWindowTitle("Bitbot Nav Deck");
    this->addCentralWidget(this->CentralWidget__, true, true, 0);
    this->InitConnectionWidget();

    this->ProcessDisconnetced();
}

PilotPage::~PilotPage()
{
    this->CommThread__->quit();
    this->CommThread__->wait();
}


void PilotPage::InitConnectionWidget()
{

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
    this->PushButton_Connect__->setObjectName(QString::fromUtf8("pushButton_Connect"));
    this->PushButton_Connect__->setFont(font);
    this->PushButton_Connect__->setText(tr("connect"));

    horizontalLayout_2->addWidget(this->PushButton_Connect__);


    horizontalLayout_3->addLayout(horizontalLayout_2);

    auto horizontalSpacer_3 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

    horizontalLayout_3->addItem(horizontalSpacer_3);


    gridLayout->addLayout(horizontalLayout_3, 0, 0, 1, 1);

    QObject::connect(this->PushButton_Connect__, &ElaPushButton::clicked, this, [this]() {
        this->IP = this->LineEdit_IP__->text();
        this->port = this->SpinBox_Port__->value();
        
        if (this->PushButton_Connect__->text() == tr("connect"))
        {
            if (bool ok = this->CommHandle__->Connect(IP, port, 5000); ok == false)
            {
                QMessageBox::warning(this, tr("Connection Failed"), tr("Failed to connected to BitBot, please check you network connection!"), QMessageBox::Ok);
            }
            else
            {
                this->PushButton_Connect__->setText(tr("connecting"));
                this->PushButton_Connect__->setEnabled(false);
            }
        }
        else if (this->PushButton_Connect__->text() == tr("disconnect"))
        {
            if (!this->CommHandle__->Disconnect())
            {
                QMessageBox::warning(this, tr("Disconnect Failed"), tr("Failed to disconnect to Bitbot, try again later"), QMessageBox::Ok);
            }
        }
        else if (this->PushButton_Connect__->text() == tr("connecting"))
        {
            QMessageBox::warning(this, tr("System is Busy"), tr("System is busy, connect/disconnect request will be ignored"));
        }
        else
        {
            QMessageBox::critical(this, tr("Catastrophic Failure"), tr("Unknown Button Status"), QMessageBox::Ok);
        }
     });
}

void PilotPage::InitCommHandle()
{
    this->CommThread__ = new QThread(this);
    this->CommHandle__ = new zzs::BITBOT_TCP_PROTOCAL_V1();
    //this->CommHandle__->moveToThread(this->CommThread__);
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
        case zzs::META_COMMUNICATION::CONNECTION_STATUS::ERROR:
            this->ProcessConnectionError();
            break;
        default:
            break;
        }
        });

    QObject::connect(this->CommHandle__, &zzs::META_COMMUNICATION::ReceivedPDO, this, &PilotPage::ProcessPDO);

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
        }
        });

    QObject::connect(this->GamepadHandle__, &ZQGamepad::JoystickMoved, [this](int id, Q_XSX_JOYSTICK_ENUM axis, float value) {
        QVariantMap map;
        QString JoystickName;
        JoystickName = this->GamepadHandle__->name(axis);
        map.insert(JoystickName, QVariant(value));
        this->CommHandle__->SendUserCommand(map);
        if (this->GamepadStatusUI__ != nullptr)
        {
            this->GamepadStatusUI__->JoystickMoved(id, axis, value);
        }
        });


    QObject::connect(this->GamepadHandle__, &ZQGamepad::ButtonClicked, [this](int id, Q_XSX_JOYSTICK_ENUM button, bool ButtonState) {
        QString ButtonName;
        ButtonName = this->GamepadHandle__->name(button);
        QVariantMap map;
        map.insert(ButtonName, QVariant(ButtonState));
        this->CommHandle__->SendUserCommand(map); //TODO: change to boundle command send to save bandwidth and reduce workload.

        if (this->KeyboardEventUI__ != nullptr)
        {
            this->KeyboardEventUI__->ButtonClicked(ButtonName, ButtonState);
        }
        });
}
void PilotPage::DrawConnectedUI()
{
    if (this->CentralLayout__ != nullptr) delete this->CentralLayout__;
    this->CentralLayout__ = nullptr;

    this->CentralLayout__ = new QVBoxLayout(this->CentralWidget__);
    this->CentralLayout__->addWidget(this->ConnectionAreaUI__);
    this->PushButton_Connect__->setText(tr("disconnect"));
    this->PushButton_Connect__->setEnabled(true);

    QHBoxLayout* lay = new QHBoxLayout(this->CentralWidget__);

    this->KernelStatusUI__ = new RTDKernelStates(this->CentralWidget__);
    lay->addWidget(this->KernelStatusUI__);

    this->RobotStateUI__ = new RTDRobotStates(this->CentralWidget__);
    lay->addWidget(this->RobotStateUI__);
    this->RobotStateUI__->SetHeaderList(this->StateLists);

    this->KeyboardEventUI__ = new RTDKeyboardStatus(this->CentralWidget__);
    lay->addWidget(this->KeyboardEventUI__);

    size_t GamepadCnt=this->GamepadHandle__->CountConnectedGamepad();
    this->GamepadStatusUI__ = new RTDGamepadStatus(GamepadCnt, this->CentralWidget__);
    lay->addWidget(this->GamepadStatusUI__);

    QSpacerItem* horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);
    lay->addItem(horizontalSpacer);
    this->CentralLayout__->addLayout(lay);

    QVBoxLayout* DeviceLayout = new QVBoxLayout(this->CentralWidget__);
    for (size_t i = 0; i < this->DeviceTypes.size(); i++)
    {
        RTDDeviceInfo* ui = new RTDDeviceInfo(this->CentralWidget__);
        ui->setHeaders(this->DeviceTypes[i], this->DeviceHeaders[i], this->DeviceSensorNames[i]);
        DeviceLayout->addWidget(ui);
        this->DeviceListsUI__.insert(this->DeviceTypes[i], ui);
    }

    this->UserInfoUI__ = new RTDDeviceInfo(this->CentralWidget__);
    this->UserInfoUI__->setHeaders(this->ExtraHeader);
    DeviceLayout->addWidget(this->UserInfoUI__);

    this->CentralLayout__->addLayout(DeviceLayout);

}
void PilotPage::DrawDisconnectedUI()
{
    if(this->KernelStatusUI__!=nullptr)  this->KernelStatusUI__->deleteLater();
    this->KernelStatusUI__ = nullptr;
    if(this->RobotStateUI__!=nullptr)  this->RobotStateUI__->deleteLater();
    this->RobotStateUI__ = nullptr;
    if (this->KeyboardEventUI__ != nullptr) this->KeyboardEventUI__->deleteLater();
    this->KeyboardEventUI__ = nullptr;
    if(this->GamepadStatusUI__!=nullptr)  this->GamepadStatusUI__->deleteLater();
    this->GamepadStatusUI__ = nullptr;

    for (auto& ui : this->DeviceListsUI__)
    {
        if (ui != nullptr)
            ui->deleteLater();
    }
    this->DeviceListsUI__.clear();
    if(this->UserInfoUI__!=nullptr) this->UserInfoUI__->deleteLater();
    this->UserInfoUI__ = nullptr;


    if(this->CentralLayout__!=nullptr) this->CentralLayout__->deleteLater();
    this->CentralLayout__ = nullptr;
   
    //reset ui
    this->CentralLayout__ = new QVBoxLayout(this->CentralWidget__);
    this->CentralLayout__->addWidget(this->ConnectionAreaUI__);
    this->PushButton_Connect__->setText(tr("connect"));
    this->PushButton_Connect__->setEnabled(true);
}

void PilotPage::ProcessDisconnetced()
{
    this->connected__ = false;
    this->AllHeaders.clear();
    this->KernelHeaders.clear();
    this->DeviceHeaders.clear();
    this->DeviceSensorNames.clear();
    this->DeviceTypes.clear();
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

    this->DeviceHeaders.clear(); //vector of vector
    this->DeviceTypes.clear(); //vector
    this->DeviceSensorNames.clear(); //vector of vector
    QVector<zzs::BITBOT_TCP_PROTOCAL_V1::ABSTRACT_DEVICE_HEADER> DeviceHeaders;
    this->CommHandle__->getDeviceHeader(DeviceHeaders);

    QMap<QString, size_t> DeviceTypeSet;
    size_t devTypeCnt = 0;

    for (auto& header : DeviceHeaders)
    {
        if (!DeviceTypeSet.contains(header.DeviceTypeName))
        {
            this->DeviceHeaders.push_back(header.SensorsName);
            this->DeviceTypes.push_back(header.DeviceTypeName);
            DeviceTypeSet.insert(header.DeviceTypeName, devTypeCnt++);
        }
    }

    this->DeviceSensorNames.resize(devTypeCnt);
    for (auto& header : DeviceHeaders)
    {
        size_t TypeIdx = DeviceTypeSet[header.DeviceTypeName];
        this->DeviceSensorNames[TypeIdx].push_back(header.DeviceName);
    }

    this->AllHeaders.clear();
    this->CommHandle__->getDataHeaderAll(this->AllHeaders);

    this->DrawConnectedUI();
}
void PilotPage::ProcessConnectionError()
{
    this->connected__ = false;
    this->DrawDisconnectedUI();
    QMessageBox::warning(this, tr("Connection Failed"), tr("Failed to connected to BitBot, please check you network connection!"), QMessageBox::Ok);
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

   
    for (auto& ui : this->DeviceListsUI__)
    {
        size_t DataSize = ui->getHeaderSize();
        ui->UpdateDeviceInfo(QVariantList(PDOInfo.begin(), PDOInfo.begin() + DataSize));
        PDOInfo = QVariantList(PDOInfo.begin() + DataSize, PDOInfo.end());
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
        }
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

