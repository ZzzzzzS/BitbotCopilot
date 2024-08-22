#include "BackendManager.h"
#include "BackendManager.h"
#include "BackendManager.h"
#include "./ui_BackendManager.h"
#include <QObject>
#include <QAction>
#include <QMetaEnum>
#include <QMessageBox>
#include <QDebug>
#include "ElaTheme.h"
#include <QPixmap>

QString Cmd = "ssh";

BackendManager::BackendManager(QWidget* parent)
    : MetaRTDView(MetaRTDView::RTDViewType::EXTEND_WINDOW,parent)
    , ui(new Ui::BackendManager)
{
    ui->setupUi(this);
    this->BackendProcess__ = new QProcess(this);
    this->ui->label_icon->setPixmap(QPixmap(":/UI/Image/backend_icon.png").scaledToWidth(40, Qt::SmoothTransformation));
    this->ui->textEdit_BackendInfo->setReadOnly(true);
    QObject::connect(this->BackendProcess__, &QProcess::stateChanged, this, [this](QProcess::ProcessState newState) {
        switch (newState)
        {
        case QProcess::NotRunning:
            this->ui->pushButton_connect->setEnabled(true);
            this->ui->pushButton_connect->setText(tr("Connect"));
            break;
        case QProcess::Starting:
            break;
        case QProcess::Running:
        {
            this->ui->pushButton_connect->setEnabled(true);
            this->ui->pushButton_connect->setText(tr("Stop"));
            emit this->ProcessStarted();
            break;
        }

        default:
            break;
        }
        });

    QObject::connect(this->BackendProcess__, &QProcess::errorOccurred, this, [this](QProcess::ProcessError error) {
        emit this->ProcessErrored();
        QString ErrMsg = QMetaEnum::fromType<QProcess::ProcessError>().valueToKey(error);
        QMessageBox::warning(this, tr("Bitbot Backend Error"), tr("Bitbot backend error, backend ") + ErrMsg, QMessageBox::Ok);
        });

    QObject::connect(this->BackendProcess__, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished), this, [this](int exitCode, QProcess::ExitStatus exitStatus) {
        switch (exitStatus)
        {
        case QProcess::NormalExit:
            emit this->ProcessFinished();
            break;
        case QProcess::CrashExit:
            emit this->ProcessErrored();
            break;
        default:
            break;
        }
        this->ui->pushButton_connect->setEnabled(true);
        this->ui->pushButton_connect->setText(tr("Connect"));
        });


    QObject::connect(this->BackendProcess__, &QProcess::readyReadStandardOutput, this, [this]() {
        QByteArray msg = this->BackendProcess__->readAllStandardOutput();
        this->ui->textEdit_BackendInfo->append(QString(msg));
        });


    QObject::connect(this->BackendProcess__, &QProcess::readyReadStandardError, this, [this]() {
        QByteArray err_msg = this->BackendProcess__->readAllStandardError();
        this->ui->textEdit_BackendInfo->append(QString("[WARNING]") + QString(err_msg));
        });
    
    QObject::connect(this->ui->textEdit_BackendInfo, &QTextEdit::textChanged, this, [this]() {
        this->ui->textEdit_BackendInfo->moveCursor(QTextCursor::End);
        });
   

    QObject::connect(this->ui->pushButton_connect, &QPushButton::clicked, this, [this]() {
        if (this->ui->pushButton_connect->text() == tr("Connect"))
        {
            this->ui->textEdit_BackendInfo->clear();
            this->ui->pushButton_connect->setText(tr("Connecting"));
            this->ui->pushButton_connect->setEnabled(false);
            QStringList args;
            args.push_back("bitbot@192.168.8.112");
            args.push_back("/home/bitbot/cetc_robot_ganzhi_v2/build/bin/main_app");
            this->BackendProcess__->start(Cmd, args);
        }
        else if (this->ui->pushButton_connect->text() == tr("Stop"))
        {
            this->ui->pushButton_connect->setText(tr("Force Stop"));
            this->ui->pushButton_connect->setEnabled(true);
            //this->BackendProcess__->terminate();
            QStringList args;
            args.append("bitbot@192.168.8.112");
            args.append("pkill");
            args.append("main_app");
            QProcess::execute("ssh", args);
        }
        else if (this->ui->pushButton_connect->text() == tr("Connecting"))
        {

        }
        else if (this->ui->pushButton_connect->text() == tr("Force Stop"))
        {
            this->ui->pushButton_connect->setEnabled(true);
            this->BackendProcess__->kill();
        }
        else
        {
            qDebug() << "backend: unknown button status";
        }
        });

    QObject::connect(eTheme, &ElaTheme::themeModeChanged, this, &BackendManager::ThemeChanged);
    this->ThemeChanged(eTheme->getThemeMode());
    //this->repaint();
}

BackendManager::~BackendManager()
{
    this->BackendProcess__->disconnect();
    delete ui;
}

void BackendManager::ResetUI()
{

}

void BackendManager::ThemeChanged(ElaThemeType::ThemeMode themeMode)
{
    if (themeMode == ElaThemeType::Light)
    {
        QPalette palette;
        palette.setColor(QPalette::Text, Qt::black);
        palette.setColor(QPalette::Base, Qt::white);
        palette.setColor(QPalette::PlaceholderText, QColor(0x00, 0x00, 0x00, 128));
        this->ui->textEdit_BackendInfo->setPalette(palette);
        
        QPalette palette_label;
        palette_label.setColor(QPalette::WindowText, Qt::black);
        this->ui->label->setPalette(palette_label);
    }
    else
    {
        QPalette palette;
        palette.setColor(QPalette::Text, Qt::white);
        palette.setColor(QPalette::Base, Qt::black);
        palette.setColor(QPalette::PlaceholderText, QColor(0xBA, 0xBA, 0xBA));
        this->ui->textEdit_BackendInfo->setPalette(palette);

        QPalette palette_label;
        palette_label.setColor(QPalette::WindowText, Qt::white);
        this->ui->label->setPalette(palette_label);
    }

    QString styleSheet;
    if (themeMode == ElaThemeType::ThemeMode::Light)
    {
        styleSheet = QString::fromUtf8(R"(
       QTextEdit {
            background-color: rgba(255, 255, 255, 100); 
            border-radius: 10px; 
        }
        )");
    }
    else
    {
        styleSheet = QString::fromUtf8(R"(
       QTextEdit {
            background-color: rgba(80, 80, 80, 70); 
            border-radius: 10px; 
        }
        )");
    }

    //this->setStyleSheet(styleSheet);
}
