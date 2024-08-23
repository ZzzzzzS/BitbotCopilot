#include "MainWindow.h"
#include "ElaWindow.h"
#include <QDebug>

MainWindow::MainWindow(QWidget* parent)
    : ElaWindow(parent)
{
    this->InitWindow();
    this->InitPage();
    this->InitFooter();

    this->InitSignalSlot();

    this->moveToCenter();
}

MainWindow::~MainWindow()
{
}

void MainWindow::InitWindow()
{
    this->resize(1366, 768);
    this->setMinimumWidth(700);

    setUserInfoCardPixmap(QPixmap(":/UI/Image/logo.png"));
    setUserInfoCardTitle("BITBOT Copilot");
    setUserInfoCardSubTitle("BIT Humanoid Group");
    //setWindowTitle("Bitbot Pilot");
}


void MainWindow::InitPage()
{
    this->HomePage__ = new HomePage(this);
    this->addPageNode(tr("Home"), this->HomePage__, ElaIconType::House);

    this->PilotPage__ = new PilotPage(this);
    this->addPageNode(tr("Nav Deck"), this->PilotPage__, ElaIconType::GamepadModern);

    this->ViewDataPage__ = new ViewDataPage(this);
    this->addPageNode(tr("Data Viewer"), this->ViewDataPage__, ElaIconType::Database);
}

void MainWindow::InitFooter()
{
    //init about me
    this->AboutPage__ = new AboutPage(this);
    this->addFooterNode(tr("About"), this->AboutPage__, QString("About"), 0, ElaIconType::CircleUser);

    //init settings
    this->SettingsPage__ = new SettingsPage(this);
    this->addFooterNode(tr("Settings"), this->SettingsPage__, QString("Settings"), 0, ElaIconType::Gear);
}

void MainWindow::InitSignalSlot()
{
    QObject::connect(this->HomePage__, &HomePage::AttachBitbotSignal, this, [this]() {
        this->navigation(this->PilotPage__->property("ElaPageKey").toString());
        });

    /*QObject::connect(this->HomePage__, &HomePage::LaunchBitbotSignal, this, [this]() {
        this->navigation(this->PilotPage__->property("ElaPageKey").toString());
    });*/

    QObject::connect(this->HomePage__, &HomePage::ViewDataSignal, this, [this]() {
        this->navigation(this->ViewDataPage__->property("ElaPageKey").toString());
        });
}

void MainWindow::resizeEvent(QResizeEvent* event)
{
    /*if (this->width() < 1360)
        this->setIsNavigationBarEnable(false);
    else
        this->setIsNavigationBarEnable(true);*/
    this->setMinimumWidth(1050);
    if (this->width() < 1050)
    {
        this->resize(1050, this->height());
        return;
    }
        

    ElaWindow::resizeEvent(event);
}
