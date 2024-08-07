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



PilotPage::PilotPage(QWidget* parent)
{
	QWidget* centralWidget = new QWidget(this);
	centralWidget->setWindowTitle("Bitbot Nav Deck");
	QGridLayout* CentralLayout = new QGridLayout(centralWidget);
	
	this->InitConnectionWidget();
	CentralLayout->addWidget(this->ConnectionAreaUI, 0, 0);

    QHBoxLayout* lay = new QHBoxLayout(centralWidget);

	this->KernelStatusUI = new RTDKernelStates(centralWidget);
    lay->addWidget(this->KernelStatusUI);

	this->RobotStateUI = new RTDRobotStates(centralWidget);
    lay->addWidget(this->RobotStateUI);

	this->KeyboardEventUI = new RTDKeyboardStatus(centralWidget);
    lay->addWidget(this->KeyboardEventUI);

	this->GamepadStatusUI = new RTDGamepadStatus(2,centralWidget);
    lay->addWidget(this->GamepadStatusUI);

    QSpacerItem* horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);
    lay->addItem(horizontalSpacer);

    CentralLayout->addLayout(lay, 1, 0);

    RTDDeviceInfo* dev = new RTDDeviceInfo(centralWidget);
    CentralLayout->addWidget(dev, 2, 0);

    RTDDeviceInfo* dev2 = new RTDDeviceInfo(centralWidget);
    CentralLayout->addWidget(dev2, 3, 0);


	CentralLayout->setHorizontalSpacing(20);
	CentralLayout->setVerticalSpacing(20);

	this->addCentralWidget(centralWidget, true, true, 0);
}

PilotPage::~PilotPage()
{
}

void PilotPage::InitConnectionWidget()
{
	this->ConnectionAreaUI = new ElaScrollPageArea(this);
    this->ConnectionAreaUI->setSizePolicy(QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Fixed);
	
    QFont font;
    font.setPointSize(10);

    auto gridLayout = new QGridLayout(this->ConnectionAreaUI);
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
    auto label = new ElaText(this->ConnectionAreaUI);
    label->setObjectName(QString::fromUtf8("label"));
    label->setText(tr("IP:"));
    label->setFont(font);

    horizontalLayout->addWidget(label);

    auto lineEdit_IP = new ElaLineEdit(this->ConnectionAreaUI);
    lineEdit_IP->setFixedHeight(30);
    lineEdit_IP->setFixedWidth(200);
    lineEdit_IP->setFont(font);
    lineEdit_IP->setAlignment(Qt::AlignCenter);

    lineEdit_IP->setObjectName(QString::fromUtf8("lineEdit_IP"));
    lineEdit_IP->setEnabled(true);
    QSizePolicy sizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
    sizePolicy.setHorizontalStretch(0);
    sizePolicy.setVerticalStretch(0);
    sizePolicy.setHeightForWidth(lineEdit_IP->sizePolicy().hasHeightForWidth());
    lineEdit_IP->setSizePolicy(sizePolicy);

    horizontalLayout->addWidget(lineEdit_IP);

    auto label_2 = new ElaText(this->ConnectionAreaUI);
    label_2->setObjectName(QString::fromUtf8("label_2"));
    label_2->setText("          " + tr("Port:"));
    label_2->setFont(font);

    horizontalLayout->addWidget(label_2);

    auto spinBox_Port = new ElaSpinBox(this->ConnectionAreaUI);
    spinBox_Port->setObjectName(QString::fromUtf8("spinBox_Port"));
    spinBox_Port->setFont(font);

    horizontalLayout->addWidget(spinBox_Port);


    horizontalLayout_2->addLayout(horizontalLayout);

    auto horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Preferred, QSizePolicy::Minimum);

    horizontalLayout_2->addItem(horizontalSpacer);

    auto pushButton_Connect = new ElaPushButton(this->ConnectionAreaUI);
    pushButton_Connect->setObjectName(QString::fromUtf8("pushButton_Connect"));
    pushButton_Connect->setFont(font);

    horizontalLayout_2->addWidget(pushButton_Connect);


    horizontalLayout_3->addLayout(horizontalLayout_2);

    auto horizontalSpacer_3 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

    horizontalLayout_3->addItem(horizontalSpacer_3);


    gridLayout->addLayout(horizontalLayout_3, 0, 0, 1, 1);
}
