#include "UI/MainWindow.h"

#include <QApplication>
#include <QLocale>
#include <QTranslator>
#include "ElaApplication.h"
#include "ElaWidget.h"
#include "QSplashScreen"
#include "UI/widget/CustomSplashScreen.h"
#include "Utils/Settings/SettingsHandler.h"

#include<windows.h>


int main(int argc, char* argv[])
{
    qputenv("QT_QPA_PLATFORM", "windows:darkmode=[1|2]");
#if (QT_VERSION < QT_VERSION_CHECK(6, 0, 0))
    QGuiApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QGuiApplication::setAttribute(Qt::AA_UseHighDpiPixmaps);
#if (QT_VERSION >= QT_VERSION_CHECK(5, 14, 0))
    QGuiApplication::setHighDpiScaleFactorRoundingPolicy(Qt::HighDpiScaleFactorRoundingPolicy::PassThrough);
#endif
#endif

    QApplication a(argc, argv);

    a.setWindowIcon(QIcon(":/logo/Image/ProgramIcon.ico"));

    CustomSplashScreen screen(QPixmap(":/logo/Image/Splash_Screen.png"));
    screen.show();

    a.processEvents();

    //dknt的提议
    if (ZSet->isVIP())
    {
        Sleep(1000); //这个地方先这样，让用户能充分看到我们的启动界面，等以后加钱再改
    }
    else
    {
        Sleep(3000);
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
    screen.finish(&w);
    

    return a.exec();
}
