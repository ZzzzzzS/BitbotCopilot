#include "MainWindow.h"
#include "ElaWindow.h"
#include <QDebug>
#include <QMessageBox>
#include "AboutPage.h"
#include "QPalette"
#include "ElaApplication.h"
#include "ElaDockWidget.h"

MainWindow::MainWindow(QWidget* parent)
    : ElaWindow(parent),
    AboutWindow__(nullptr)
{
    this->InitWindow();
    this->InitPage();
    this->InitDockVirtualTrackpad();
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

    this->InitMica();
    if (this->isDarkMode())
        eTheme->setThemeMode(ElaThemeType::Dark);
    else
        eTheme->setThemeMode(ElaThemeType::Light);
}


void MainWindow::InitPage()
{
    this->HomePage__ = new HomePage(this);
    this->addPageNode(tr("Home"), this->HomePage__, ElaIconType::House);

    this->PilotPage__ = new PilotPage(this);
    this->CommHandle__ = this->PilotPage__->getCommHandle();
    this->addPageNode(tr("Nav Deck"), this->PilotPage__, ElaIconType::GamepadModern);

    this->ViewDataPage__ = new ViewDataPage(this);
    this->addPageNode(tr("Data Viewer"), this->ViewDataPage__, ElaIconType::Database);
}

void MainWindow::InitFooter()
{
    //init virtual trackpad
    QString TrackpadIdx = QString("Virtual Trackpad");
    this->addFooterNode(tr("Virtual Trackpad"), nullptr, TrackpadIdx, 0, ElaIconType::Keyboard);
    QObject::connect(this, &ElaWindow::navigationNodeClicked, this, [this, TrackpadIdx](ElaNavigationType::NavigationNodeType nodeType, QString nodeKey) {
        if (nodeKey == TrackpadIdx && nodeType == ElaNavigationType::NavigationNodeType::FooterNode)
        {
            if (this->WindowBottomDocker__->isVisible())
            {
                this->WindowBottomDocker__->close();
            }
            else
            {
                this->WindowBottomDocker__->setFloating(false);
                this->WindowBottomDocker__->show();
            }
        }
     });

    //init about me
    this->AboutKey__= QString("About");
    this->AboutWindow__ = new AboutPageCentralWidget(this);
    this->addFooterNode(tr("About"), this->AboutWindow__, AboutKey__, 0, ElaIconType::IconName::CircleUser);
    /*connect(this, &ElaWindow::navigationNodeClicked, this, [=](ElaNavigationType::NavigationNodeType nodeType, QString nodeKey) {
        if (this->AboutKey__ == nodeKey)
        {
            if(this->AboutWindow__==nullptr)
                this->AboutWindow__ = new AboutPageCentralWidget();
            this->AboutWindow__->setAttribute(Qt::WA_DeleteOnClose);
            this->AboutWindow__->show();
            QObject::connect(this->AboutWindow__, &QObject::destroyed, this, [this]() {
                this->AboutWindow__ = nullptr;
            });
        }
    });*/


    //init settings
    this->SettingsPage__ = new SettingsPage(this);
    QString Settings = QString("Settings");
    this->addFooterNode(tr("Settings"), this->SettingsPage__, Settings, 0, ElaIconType::IconName::Gear);
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
        if (!this->isDarkMode())
        {
            if(eTheme->getThemeMode()!=ElaThemeType::Light)
                this->TriggerThemeChangeAnimation();
        }
        else
        {
            if (eTheme->getThemeMode() != ElaThemeType::Dark)
                this->TriggerThemeChangeAnimation();
        }

        //some time there are some delay in changing wallpaper.

        QTimer::singleShot(1000, this, [this]() {
            this->InitMica();
            });

        QTimer::singleShot(5000, this, [this]() {
            this->InitMica();
            });
           
    }
}

void MainWindow::InitMica()
{
    static QString LastMicaBackground;
    QString MicaBackground = getMicaBackground();
    if (MicaBackground == LastMicaBackground)
        return;
    if (!MicaBackground.isEmpty())
    {
        eApp->setMicaImagePath(MicaBackground);
        eApp->setIsEnableMica(true);
    }
    else
    {
        eApp->setIsEnableMica(false);
    }
    LastMicaBackground = MicaBackground;
}

void MainWindow::InitDockVirtualTrackpad()
{
    this->WindowBottomDocker__ = new ElaDockWidget(tr("Virtual Trackpad"), this);
    this->VirtualTrackpad__ = new VirtualTrackpad(this);
    this->WindowBottomDocker__->setWidget(this->VirtualTrackpad__);
    this->WindowBottomDocker__->setAllowedAreas(Qt::DockWidgetArea::BottomDockWidgetArea);
    this->addDockWidget(Qt::BottomDockWidgetArea, this->WindowBottomDocker__);
    this->WindowBottomDocker__->close();

    QObject::connect(this->VirtualTrackpad__, &VirtualTrackpad::VirtualButtonPressed, this, [this](QString Event, int KeyState) {
        QVariantMap map;
        map.insert(Event, QVariant(KeyState));
        this->CommHandle__->SendUserCommand(map);
        });
    
    QObject::connect(this->VirtualTrackpad__, &VirtualTrackpad::VirtualTrackpadMoved, this, [this](QString Axis1, double value1, QString Axis2, double value2) {
        QVariantMap map;
        map.insert(Axis1, QVariant(value1));
        map.insert(Axis2, QVariant(value2));
        this->CommHandle__->SendUserCommand(map);
     });

    
    QObject::connect(this->CommHandle__, &zzs::BITBOT_TCP_PROTOCAL_V1::ConnectionStateChanged, this, [this](int status) {
        if (status == static_cast<int>(zzs::BITBOT_TCP_PROTOCAL_V1::CONNECTION_STATUS::CONNECTED))
        {
            QMap<QString, QString> availableKeys;
            this->CommHandle__->getAvailableKeys(availableKeys, false);
            this->VirtualTrackpad__->UpdateAvailableButton(availableKeys);
            this->VirtualTrackpad__->setConnected(true);
        }
        else
        {
            this->VirtualTrackpad__->setConnected(false);
        }
    });
}

bool MainWindow::isDarkMode()
{
#ifdef Q_OS_WIN
    QSettings wallpaper("HKEY_CURRENT_USER\\Software\\Microsoft\\Windows\\CurrentVersion\\Themes\\Personalize", QSettings::NativeFormat);
    bool ok;
    int val = wallpaper.value("AppsUseLightTheme").toInt(&ok);
    if (!ok) return false;
    return val == 0 ? true : false;
#endif // Q_OS_WIN

    return false;
}

QString MainWindow::getMicaBackground()
{
#ifdef Q_OS_WIN
    QSettings wallpaper("HKEY_CURRENT_USER\\Control Panel\\Desktop", QSettings::NativeFormat);
    QString val = wallpaper.value("Wallpaper").toString();
    qDebug() << val;
    return val;
#endif
    return QString();
}
