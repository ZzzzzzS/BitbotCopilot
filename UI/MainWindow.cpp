#include "MainWindow.h"
#include "ElaWindow.h"
#include <QDebug>
#include <QMessageBox>
#include "AboutPage.h"
#include "QPalette"
#include "ElaApplication.h"
#include "ElaDockWidget.h"
#include "UI/widget/FluentMessageBox.hpp"
#include "ElaStatusBar.h"
#include "QSystemTrayIcon"
#include "Utils/Settings/SettingsHandler.h"
#include "UI/ProfileSelector.h"
#include "UI/widget/FluentMessageBox.hpp"
#include "QProcess"

MainWindow::MainWindow(QWidget* parent)
    : ElaWindow(parent),
    AboutWindow__(nullptr)
{
    this->InitWindow();
    this->InitPage();
    this->InitDockVirtualTrackpad();
    this->InitFooter();
    this->InitSSHConnection();

    this->InitSignalSlot();

    this->moveToCenter();

#ifdef PRERELEASE_BUILD
    ElaStatusBar* statusBar = new ElaStatusBar(this);
    QString dateTime = __DATE__;
    dateTime.replace(" ", "");
    dateTime.replace(" ", "");
    dateTime.replace(" ", "");
    if (dateTime.length() == 8)
    {
        dateTime.insert(3, "0");
    }

    QDate BuildDate = QLocale(QLocale::English).toDate(dateTime, "MMMddyyyy");
    qDebug() << BuildDate;
    QString InfoText = BuildDate.toString(Qt::ISODate) + "-";
    InfoText += QString(BUILD_VERSION_COMMIT_HASH);

    ElaText* statusText = new ElaText(tr("BETA VERSION! FOR EVALUATION PURPOSE ONLY!   BUILD VERSION: ") + InfoText, this);
    statusText->setTextPixelSize(14);
    statusText->setMinimumWidth(670);
    statusBar->addWidget(statusText);
    this->setStatusBar(statusBar);
#endif // PRERELEASE_BUILD
}

MainWindow::~MainWindow()
{
    if (this->SessionManager__ != nullptr)
    {
        this->SessionManager__->destoryInstance();
    }
}


void MainWindow::InitWindow()
{
    this->resize(1366, 768);
    this->setMinimumWidth(700);

    auto [UserName, ip, Avatar] = ZSet->getUserProfileInfo();

    //setUserInfoCardPixmap(QPixmap(":/UI/Image/logo.png"));
    qDebug() << Avatar;
    setUserInfoCardPixmap(QPixmap(Avatar));
    setUserInfoCardTitle(UserName.split(".ini").first());
    setUserInfoCardSubTitle(QString("ip: ") + ip);
    //setWindowTitle("Bitbot Copilot");

    this->InitMica();
    if (this->isDarkMode())
        eTheme->setThemeMode(ElaThemeType::Dark);
    else
        eTheme->setThemeMode(ElaThemeType::Light);

    this->TrayIcon__ = new QSystemTrayIcon(QIcon(":/logo/Image/ProgramIcon.ico"), this);
    this->TrayIcon__->show();
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
    this->AboutKey__ = QString("About");
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
            FluentMessageBox::warningOk(this, tr("Failed to Attach Bitbot"), tr("Failed to attach Bitbot backend, nav deck is already running, please disconnect it first"));
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
            FluentMessageBox::warningOk(this, tr("Failed to Attach Bitbot"), tr("Failed to attach Bitbot backend, nav deck is already running, please disconnect it first"));
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
            FluentMessageBox::warningOk(this, tr("Failed to Auto Initialize Bitbot"), tr("Failed to auto run new bitbot task, nav deck is already running or launch list is not configured."));
        }
        });



    QObject::connect(this->HomePage__, &HomePage::ViewDataSignal, this, [this]() {
        this->navigation(this->ViewDataPage__->property("ElaPageKey").toString());
        });

    QObject::connect(this, &MainWindow::userInfoCardClicked, this, [this]() {
        QString profile = ProfileSelector::Select();
        if (profile.isEmpty())
            return;

        if (!ZSet->updateCurrentUserProfile(profile))
        {
            FluentMessageBox::warningOk(this, tr("Failed to switch robot profile."), tr("robot profile dose not exist or contain error(s)."));
        }
        else
        {
            if (QProcess::startDetached(QCoreApplication::applicationFilePath(), QStringList() << "--no_splash", QCoreApplication::applicationDirPath()))
                qApp->quit();
            else
                FluentMessageBox::warningOk(this, tr("Failed to switch robot profile."), tr("launching failed."));
        }
        });

}

void MainWindow::resizeEvent(QResizeEvent* event)
{
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
            if (eTheme->getThemeMode() != ElaThemeType::Light)
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

    QObject::connect(this->VirtualTrackpad__, &VirtualTrackpad::VirtualButtonValueSet, this, [this](QString Event, double KeyState) {
        QVariantMap map;
        map.insert(Event, QVariant(KeyState));
        this->CommHandle__->SendUserCommand(map);
        });

    QObject::connect(this->VirtualTrackpad__, &VirtualTrackpad::VirtualTrackpadMoved, this, [this](QString Axis1, double value1, QString Axis2, double value2) {
        QVariantMap map;
        map.insert(Axis1, QVariant(static_cast<int>(value1 * 32768.0)));
        map.insert(Axis2, QVariant(static_cast<int>(value2 * 32768.0)));
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

void MainWindow::InitSSHConnection()
{
    if (!ZSet->isBackendRemote())
        return;

    this->SessionManager__ = zzs::SessionManager::getInstance();
    auto serverinfo = ZSet->getBackendConfig_ex();
    this->SessionManager__->SetServerInfo(std::get<0>(serverinfo).toStdString(),
        std::get<1>(serverinfo).toStdString(),
        std::get<2>(serverinfo).toStdString(),
        std::get<3>(serverinfo).toStdString());
    //bool ok = this->SessionManager__->Connect();
    // if (!ok)
    //     return; // 等用的时候再连接，解决断网重连问题
    //this->TrayIcon__->showMessage("Bitbot Copilot", tr("Connecting to Robot"), QSystemTrayIcon::MessageIcon::Information, 2000);
    this->CheckSSHConnectionTimer__ = new QTimer(this);
    this->CheckSSHConnectionTimer__->setInterval(1000);
    this->Connected__ = false;
    QObject::connect(this->CheckSSHConnectionTimer__, &QTimer::timeout, this, [this]() {
        bool connected = this->SessionManager__->CheckConnection();
        if (connected && !this->Connected__)
        {
            this->TrayIcon__->showMessage("Bitbot Copilot", tr("Connection with Robot is Established"), QSystemTrayIcon::MessageIcon::Information, 2000);
        }
        else if (!connected && this->Connected__)
        {
            this->TrayIcon__->showMessage("Bitbot Copilot", tr("Connection with Robot is Lost or Closed"), QSystemTrayIcon::MessageIcon::Information, 2000);
        }
        this->Connected__ = connected;

        bool error = this->SessionManager__->CheckError();
        if (error && !this->Errored__)
        {
            this->TrayIcon__->showMessage("Bitbot Copilot", tr("Error Occured: ") + QString::fromStdString(this->SessionManager__->GetErrorMsg()), QSystemTrayIcon::MessageIcon::Warning, 5000);
        }
        this->Errored__ = error;
        });


    this->CheckSSHConnectionTimer__->start();
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

    const QPalette defaultPalette;
    return defaultPalette.color(QPalette::WindowText).lightness()
         > defaultPalette.color(QPalette::Window).lightness();
}

QString MainWindow::getMicaBackground()
{
#ifdef Q_OS_WIN
    QSettings wallpaper("HKEY_CURRENT_USER\\Control Panel\\Desktop", QSettings::NativeFormat);
    QString val = wallpaper.value("Wallpaper").toString();
    qDebug() << val;
    if (QImage(val).isNull())
        return QString();
    return val;
#endif


    return QString();
}
