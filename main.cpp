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

    CustomSplashScreen* screen = new CustomSplashScreen(QPixmap(":/logo/Image/splash_screen.png"));
    screen->show();
    a.setWindowIcon(QIcon(":/logo/Image/ProgramIcon.ico"));
    a.processEvents();

    eApp->init();
    QFontDatabase::addApplicationFont(":/include/Font/segoe_slboot.ttf");
    

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
    std::this_thread::sleep_for(std::chrono::seconds(2));//wait for mica initialization
    w.show();
    a.processEvents();
    screen->finish(&w);
    delete screen;

    ElaContentDialog aaa(&w);
    aaa.show();
    auto bbb = new FluentLoadingWidget("test1", 0, 10);
    aaa.setCentralWidget(bbb);
    bbb->UpdatePercentage(5);
    aaa.setButtonNumber(2);
    aaa.setRightButtonText("ok", false);

    return a.exec();
}
