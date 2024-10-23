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
// #include<windows.h>
#include "UI/DataViewer/DataViewerPage.h"

#include <chrono>
#include <thread>
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

    CustomSplashScreen* screen =new CustomSplashScreen(QPixmap(":/logo/Image/splash_screen.png"));
    screen->show();
    a.setWindowIcon(QIcon(":/logo/Image/ProgramIcon.ico"));
    a.processEvents();

    //dknt的提议
    if (ZSet->isVIP())
    {
        // Sleep(1000); //这个地方先这样，让用户能充分看到我们的启动界面，等以后加钱再改
        std::this_thread::sleep_for(std::chrono::seconds(1));
    } else {
      // Sleep(3000);
      std::this_thread::sleep_for(std::chrono::seconds(3));
    }

    eApp->init();

    QTranslator translator;
    //const QStringList uiLanguages = QLocale::system().uiLanguages();
    //for (const QString& locale : uiLanguages) {
    //    const QString baseName = "BitbotCopilot_" + QLocale(locale).name();
    //    if (translator.load(":/i18n/" + baseName)) {
    //        a.installTranslator(&translator);
    //        break;
    //    }
    //}
    if (translator.load(":/BitbotCopilot_zh_CN.qm")) {
        a.installTranslator(&translator);
    }

    MainWindow w;
    w.show();
    a.processEvents();
    screen->finish(&w);
    delete screen;

    return a.exec();
}
