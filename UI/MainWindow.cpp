#include "MainWindow.h"
#include "ElaWindow.h"
#include <QDebug>
#include <QMessageBox>

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
        if (this->PilotPage__->RunNewBitbot(false, true))
        {
            this->navigation(this->PilotPage__->property("ElaPageKey").toString());
            qApp->processEvents();
            this->PilotPage__->RunNewBitbot(false, false);
        }
        else
        {
            QMessageBox::warning(this, tr("Failed to Attach Bitbot"), tr("Failed to attach Bitbot backend, nav deck is already running, please disconnect it first"));
        }
    });
    


    QObject::connect(this->HomePage__, &HomePage::LaunchBitbotSignal, this, [this]() {
        if (this->PilotPage__->RunNewBitbot(true, true))
        {
            this->navigation(this->PilotPage__->property("ElaPageKey").toString());
            qApp->processEvents();
            this->PilotPage__->RunNewBitbot(true, false);
        }
        else
        {
            QMessageBox::warning(this, tr("Failed to Attach Bitbot"), tr("Failed to attach Bitbot backend, nav deck is already running, please disconnect it first"));
        }
    });

    QObject::connect(this->HomePage__, &HomePage::AutoRunBitbotSignal, this, [this]() {
        if (this->PilotPage__->AutoInitBitbot(true))
        {
            this->navigation(this->PilotPage__->property("ElaPageKey").toString());
            qApp->processEvents();
            this->PilotPage__->AutoInitBitbot(false);
            qApp->processEvents();
        }
        else
        {
            QMessageBox::warning(this, tr("Failed to Auto Initialize Bitbot"), tr("Failed to auto run new bitbot task, nav deck is already running or launch list is not configured."));
        }
    });

    

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

void MainWindow::changeEvent(QEvent* event)
{
    ElaWindow::changeEvent(event);
    if (event->type() == QEvent::StyleChange)
    {
        qDebug() << "theme changed";
        QPalette defaultPalette;
        if (defaultPalette.color(QPalette::WindowText).lightness()
    > defaultPalette.color(QPalette::Window).lightness())
            qDebug() << "dark";
        else
            qDebug() << "light";
    }
}
