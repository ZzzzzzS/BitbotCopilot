#include "UI/MainWindow.h"

#include <QApplication>
#include <QLocale>
#include <QTranslator>
#include "ElaApplication.h"
#include "ElaWidget.h"
#include "QSplashScreen"
#include "UI/widget/CustomSplashScreen.h"
#include "Utils/Settings/SettingsHandler.h"
#include "QSettings"
#include "UI/DataViewer/DataViewerPage.h"
#include "ElaContentDialog.h"
#include "UI/widget/FluentLoadingWidget.h"
#include "Utils/Settings/parser.hpp"
#include "QMessageBox"

#include <chrono>
#include <thread>

#ifdef Q_OS_WIN32
#include "WinBase.h"
#endif

int main(int argc, char* argv[])
{
    //    SetProcessDPIAware(); // call before the main event loop
#if (QT_VERSION < QT_VERSION_CHECK(6, 0, 0))
    QGuiApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QGuiApplication::setAttribute(Qt::AA_UseHighDpiPixmaps);
#if (QT_VERSION >= QT_VERSION_CHECK(5, 14, 0))
    QGuiApplication::setHighDpiScaleFactorRoundingPolicy(Qt::HighDpiScaleFactorRoundingPolicy::PassThrough);
#endif
#endif

    QApplication a(argc, argv);
    a.setApplicationName("Bitbot Copilot");
    a.setApplicationDisplayName("Bitbot Copilot");
    a.setApplicationVersion(QString(BUILD_VERSION_COMMIT_HASH));
    ZCP_INIT; //初始化parser
    if (ZCP.isUpgradeMode())
    {
        if (ZCP.ProcessUpgradeLogic())
            return 0;
        else
        {
            QMessageBox::critical(nullptr, QObject::tr("Upgrade Failed"), QObject::tr("Program upgrade failed, try again later or contact publisher for more information."), QMessageBox::Ok);
        }
    }

    eApp->init();
    QFontDatabase::addApplicationFont(":/include/Font/segoe_slboot.ttf");
    a.processEvents();

    bool no_show_screen = ZCP.isNoSplashScreen();
    CustomSplashScreen* screen;
    if (!no_show_screen)
    {
        screen = new CustomSplashScreen(3000);
        screen->show();
    }

    a.processEvents();

    a.setWindowIcon(QIcon(":/logo/Image/ProgramIcon.ico"));
    a.processEvents();

    QTranslator translator;
    qDebug() << QLocale::system().language();
    if (translator.load(":/i18n/Translation/BitbotCopilot_zh_CN.qm") && QLocale::system().language() == QLocale::Chinese) {
        a.installTranslator(&translator);
        qDebug() << "Load zh_CN.qm";
    }

    MainWindow w;
    if (!no_show_screen)
        screen->exec();
    w.show();
    //w.showMaximized();
    //screen->close();
    a.processEvents();

    if (!no_show_screen)
        delete screen;

#ifdef Q_OS_WIN32
    EXECUTION_STATE s = SetThreadExecutionState(ES_CONTINUOUS | ES_SYSTEM_REQUIRED | ES_DISPLAY_REQUIRED);
#endif // Q_OS_WIN32
    int rtn = a.exec();

#ifdef Q_OS_WIN32
    SetThreadExecutionState(ES_CONTINUOUS);
#endif // Q_OS_WIN32


    return rtn;
}
