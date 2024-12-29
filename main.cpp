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
    eApp->init();
    QFontDatabase::addApplicationFont(":/include/Font/segoe_slboot.ttf");
    a.processEvents();
    CustomSplashScreen* screen = new CustomSplashScreen(4000);
    screen->show();
    a.processEvents();

    a.setWindowIcon(QIcon(":/logo/Image/ProgramIcon.ico"));
    a.processEvents();

    QTranslator translator;
    //const QStringList uiLanguages = QLocale::system().uiLanguages();
    //for (const QString& locale : uiLanguages) {
    //    const QString baseName = "BitbotCopilot_" + QLocale(locale).name();
    //    if (translator.load(":/i18n/" + baseName)) {
    //        a.installTranslator(&translator);
    //        break;
    //    }
    //}
    if (translator.load("./BitbotCopilot_zh_CN.qm")) {
        a.installTranslator(&translator);
        qDebug() << "Load zh_CN.qm";
    }

    MainWindow w;
    screen->exec();
    w.show();
    //screen->close();
    a.processEvents();
    delete screen;

    return a.exec();
}
