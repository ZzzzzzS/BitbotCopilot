#include "UI/MainWindow.h"

#include <QApplication>
#include <QLocale>
#include <QTranslator>
#include "ElaApplication.h"
#include "ElaWidget.h"
#include "QSplashScreen"

int main(int argc, char* argv[])
{
#if (QT_VERSION < QT_VERSION_CHECK(6, 0, 0))
    QGuiApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QGuiApplication::setAttribute(Qt::AA_UseHighDpiPixmaps);
#if (QT_VERSION >= QT_VERSION_CHECK(5, 14, 0))
    QGuiApplication::setHighDpiScaleFactorRoundingPolicy(Qt::HighDpiScaleFactorRoundingPolicy::PassThrough);
#endif
#endif

    QApplication a(argc, argv);

    a.setWindowIcon(QIcon(":/logo/Image/ProgramIcon.ico"));
    QSplashScreen screen(QPixmap(":/logo/Image/splash_Screen.png"));
    //screen.show();
    a.processEvents();

    eApp->init();
    QTranslator translator;
    const QStringList uiLanguages = QLocale::system().uiLanguages();
    for (const QString& locale : uiLanguages) {
        const QString baseName = "BITBOT-Frontend-Z_" + QLocale(locale).name();
        if (translator.load(":/i18n/" + baseName)) {
            a.installTranslator(&translator);
            break;
        }
    }
    //ElaWidget bbb;
    //bbb.show();

    MainWindow w;
    w.show();
    //screen.finish(&w);

    return a.exec();
}
