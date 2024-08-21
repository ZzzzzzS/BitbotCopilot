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
    this->CentralWidget__->setWindowTitle(" Bitbot Nav Deck");
    this->setPageTitleSpacing(10);
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
            if (bool ok = this->CommHandle__->Connect(IP, port, 3000); ok == false)
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
     },Qt::QueuedConnection);

    this->CentralLayout__ = new QVBoxLayout(this->CentralWidget__);
    this->CentralLayout__->addWidget(this->ConnectionAreaUI__);
}

void PilotPage::InitCommHandle()
{
    this->CommThread__ = new QThread(this);
    this->CommHandle__ = new zzs::BITBOT_TCP_PROTOCAL_V1(QJsonObject(), this);
    //this->CommHandle__->moveToThread(this->CommThread__); //FIXME: solve multi thread wwebsocket message sending
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
            if(Connected)
                this->GamepadHandle__->SetRumble(id, 65535, 65535, 500);
        }
        },Qt::QueuedConnection);
    

    QObject::connect(this->GamepadHandle__, &ZQGamepad::JoystickMoved,this, [this](int id, Q_XSX_JOYSTICK_ENUM axis, float value) {
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
        },Qt::QueuedConnection);


    QObject::connect(this->GamepadHandle__, &ZQGamepad::ButtonClicked,this, [this](int id, Q_XSX_JOYSTICK_ENUM button, bool ButtonState) {
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
        },Qt::QueuedConnection);
}
void PilotPage::DrawConnectedUI()
{
    this->PushButton_Connect__->setText(tr("disconnect"));
    this->PushButton_Connect__->setEnabled(true);

    if (this->ConnectedComponentLayout__ == nullptr)
    {
        this->ConnectedComponentLayout__ = new QVBoxLayout();
        QSpacerItem* l_space= new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);
        QSpacerItem* r_space= new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);
        QHBoxLayout* hor_box = new QHBoxLayout();
        hor_box->addSpacerItem(l_space);
        hor_box->addLayout(this->ConnectedComponentLayout__);
        hor_box->addSpacerItem(r_space);

        this->CentralLayout__->addLayout(hor_box);
    }
    
   
    QHBoxLayout* lay = new QHBoxLayout();

    //if (this->BackendConnectionUI__ == nullptr)
    //{
    //    this->BackendConnectionUI__=new 
    //}

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

    if (this->horizontalSpacer == nullptr)
    {
        this->horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);
        lay->addItem(this->horizontalSpacer);
    }

    
    QVBoxLayout* DeviceLayout = new QVBoxLayout();
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
        wid->setHeaders(DeviceTypeName, DeviceSensors, DeviceList);
        
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
        this->UserInfoUI__->show();
    }
    
    this->ConnectedComponentLayout__->addLayout(lay);
    this->ConnectedComponentLayout__->addLayout(DeviceLayout);
    
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
    
    for (auto& i : this->DeviceListsUI__)
    {
        if (i != nullptr)
        {
            i->hide();
            i->ResetUI();
        }    
    }

    if (this->UserInfoUI__ != nullptr)
    {
        this->UserInfoUI__->hide();
        this->UserInfoUI__->ResetUI();
    }

    qApp->processEvents();
   
    //reset ui
    this->PushButton_Connect__->setText(tr("connect"));
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

    auto pads=this->GamepadHandle__->getAvailableGamepadID();
    for (auto& pad : pads)
    {
        bool ok= this->GamepadHandle__->SetRumble(pad, 65535, 65535, 500);
        qDebug() << ok;
    }

    this->DrawConnectedUI();
    this->setFocus();
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

    QMap<QString, QVariantList> RemapPDOInfo;
    for (size_t i = 0; i < this->DeviceHeaders.size(); i++)
    {
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
        PDOInfo= QVariantList(PDOInfo.begin() + len, PDOInfo.end());
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
        }
    }
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

