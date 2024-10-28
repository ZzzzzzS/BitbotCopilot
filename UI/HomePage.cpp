#include "HomePage.h"
#include <QVBoxLayout>
#include "ElaImageCard.h"
#include "ElaAcrylicUrlCard.h"
#include "ElaText.h"
#include "ElaScrollArea.h"
#include "ElaReminderCard.h"
#include "ElaFlowLayout.h"
#include "ElaContentDialog.h"
#include "QMessageBox"
#include "UI/widget/FluentMessageBox.hpp"

HomePage::HomePage(QWidget* parent)
    :ElaScrollPage(parent)
{
    this->setPageTitleSpacing(5);

    QWidget* centralWidget = new QWidget(this);
    centralWidget->setWindowTitle(tr("Home"));
    QVBoxLayout* centerVLayout = new QVBoxLayout(centralWidget);
    centerVLayout->setContentsMargins(0, 0, 0, 0);
    ElaImageCard* backgroundCard = new ElaImageCard(this);
    backgroundCard->setCardImage(QImage(":/UI/Image/Home_Background.png"));

    ElaText* flowLayoutText = new ElaText(tr("Quick Start"), this);

    //set flow
    // ElaFlowLayout
    this->AttachBitbotCard = new ElaReminderCard(this);
    QObject::connect(this->AttachBitbotCard, &ElaReminderCard::clicked, this, [=]() { emit this->AttachBitbotSignal(); });
    this->AttachBitbotCard->setTitle(tr("Attach to Bitbot Task"));
    this->AttachBitbotCard->setSubTitle(tr("Attach to a running Bitbot\nbackend"));
    this->AttachBitbotCard->setCardPixmap(QPixmap(":/UI/Image/attach_bitbot.png"));

    this->LaunchBitbotCard = new ElaReminderCard(this);
    QObject::connect(this->LaunchBitbotCard, &ElaReminderCard::clicked, this, [=]() { emit this->LaunchBitbotSignal(); });
    this->LaunchBitbotCard->setTitle(tr("Launch New Bitbot Task"));
    this->LaunchBitbotCard->setSubTitle(tr("Launch a new Bitbot backend"));
    this->LaunchBitbotCard->setCardPixmap(QPixmap(":/UI/Image/launch_bitbot.png"));

    this->AutoRunBitbotCard = new ElaReminderCard(this);
    QObject::connect(this->AutoRunBitbotCard, &ElaReminderCard::clicked, this, [=]() { emit this->AutoRunBitbotSignal(); });
    this->AutoRunBitbotCard->setTitle(tr("Auto Run Bitbot"));
    this->AutoRunBitbotCard->setSubTitle(tr("Launch and perform initialization with one click"));
    this->AutoRunBitbotCard->setCardPixmap(QPixmap(":/UI/Image/auto_launch.png"));
    /*QObject::connect(this->AutoRunBitbotCard, &ElaReminderCard::clicked, this, [=]() {
        FluentMessageBox::criticalOk(this, tr("unsupported function"), tr("this function is not supported yet!"), QMessageBox::Ok);
    });*/

    this->DataViewerCard = new ElaReminderCard(this);
    QObject::connect(this->DataViewerCard, &ElaReminderCard::clicked, this, [=]() { emit this->ViewDataSignal(); });
    this->DataViewerCard->setTitle(tr("Data Viewer"));
    this->DataViewerCard->setSubTitle(tr("View Bitbot experiment data"));
    this->DataViewerCard->setCardPixmap(QPixmap(":/UI/Image/view_data.png"));

    this->LaunchVSCCard = new ElaReminderCard(this);
    this->LaunchVSCCard->setTitle(tr("Launch Robot VSCode"));
    this->LaunchVSCCard->setSubTitle(tr("Launch VSCode on robot side"));
    this->LaunchVSCCard->setCardPixmap(QPixmap(":/UI/Image/vscode.png"));

    QObject::connect(this->LaunchVSCCard, &ElaReminderCard::clicked, this, [=]() {
        FluentMessageBox::criticalOk(this, tr("unsupported function"), tr("this function is not supported yet!"));
        });



    ElaFlowLayout* flowLayout = new ElaFlowLayout(0, 5, 5);
    flowLayout->setIsAnimation(true);
    flowLayout->addWidget(this->AttachBitbotCard);
    flowLayout->addWidget(this->LaunchBitbotCard);
    flowLayout->addWidget(this->LaunchVSCCard);
    flowLayout->addWidget(this->AutoRunBitbotCard);
    flowLayout->addWidget(this->DataViewerCard);

    //add layout
    centerVLayout->addWidget(backgroundCard);
    centerVLayout->addWidget(flowLayoutText);
    centerVLayout->addLayout(flowLayout);
    centerVLayout->setSpacing(20);
    centerVLayout->addStretch();

    this->addCentralWidget(centralWidget);

    //set url card
    this->UrlCard1 = new ElaAcrylicUrlCard(this);
    this->UrlCard1->setUrl("https://bitbot.lmy.name/");
    this->UrlCard1->setCardPixmap(QPixmap(":/UI/Image/lmy_bitbot_logo.png"));
    this->UrlCard1->setTitle("BITBOT");
    this->UrlCard1->setSubTitle(tr("a robot software framework"));

    this->UrlCard2 = new ElaAcrylicUrlCard(this);
    this->UrlCard2->setUrl("https://blog.zzshub.cn/");
    this->UrlCard2->setCardPixmap(QPixmap(":/UI/Image/zzslogo.jpg"));
    this->UrlCard2->setTitle("ZZSHUB");
    this->UrlCard2->setSubTitle("zhouzishun@mail.zzshub.cn");

    ElaScrollArea* cardScrollArea = new ElaScrollArea(backgroundCard);
    cardScrollArea->setWidgetResizable(true);
    cardScrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    cardScrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    cardScrollArea->setIsGrabGesture(Qt::Horizontal, true);
    QWidget* cardScrollAreaWidget = new QWidget(this);
    cardScrollAreaWidget->setStyleSheet("background-color:transparent;");
    cardScrollArea->setWidget(cardScrollAreaWidget);
    QHBoxLayout* cardScrollAreaWidgetLayout = new QHBoxLayout(cardScrollAreaWidget);
    cardScrollAreaWidgetLayout->setSpacing(15);
    cardScrollAreaWidgetLayout->setContentsMargins(20, 40, 0, 0);
    cardScrollAreaWidgetLayout->addWidget(this->UrlCard1);
    cardScrollAreaWidgetLayout->addWidget(this->UrlCard2);
    cardScrollAreaWidgetLayout->addStretch();

    QHBoxLayout* backgroundLayout = new QHBoxLayout(backgroundCard);
    backgroundLayout->addWidget(cardScrollArea);
    backgroundLayout->setContentsMargins(0, 0, 0, 0);

}

HomePage::~HomePage()
{
}

void HomePage::InitBackground()
{
}

void HomePage::InitQuickStart()
{
}
