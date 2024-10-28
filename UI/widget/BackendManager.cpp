#include "BackendManager.h"
#include "./ui_BackendManager.h"
#include <QObject>
#include <QAction>
#include <QMetaEnum>
#include <QMessageBox>
#include <QDebug>
#include "ElaTheme.h"
#include <QPixmap>
#include "../../Utils/Settings/SettingsHandler.h"
#include <tuple>
#include "ElaPlainTextEdit.h"
#include "UI/widget/FluentMessageBox.hpp"

BackendManager::BackendManager(QWidget* parent)
    : MetaRTDView(MetaRTDView::RTDViewType::EXTEND_WINDOW, parent)
    , ui(new Ui::BackendManager)
{
    ui->setupUi(this);
    this->ui->textEdit_BackendInfo->setReadOnly(true);

    std::tie(this->ExecPath, this->ExecName) = ZSet->getBackendPathAndName();
    this->isRemote = ZSet->isBackendRemote();
    if (this->isRemote)
    {
        std::tie(this->UserName, this->IP) = ZSet->getRemoteBackendUserNameAndIP();
    }

    this->BackendProcess__ = new QProcess(this);
    this->ui->label_icon->setPixmap(QPixmap(":/UI/Image/backend_icon.png"), 40, 40);
    //this->ui->textEdit_BackendInfo->setReadOnly(true);
    //this->ui->textEdit_BackendInfo->setBackgroundVisible(true);

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
        FluentMessageBox::warningOk(this, tr("Bitbot Backend Error"), tr("Bitbot backend error, backend ") + ErrMsg, QMessageBox::Ok);
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
        this->ui->textEdit_BackendInfo->appendPlainText(QString(msg));
        });

    QObject::connect(this->BackendProcess__, &QProcess::readyReadStandardError, this, [this]() {
        QByteArray err_msg = this->BackendProcess__->readAllStandardError();
        this->ui->textEdit_BackendInfo->appendPlainText(QString("[WARNING] ") + QString(err_msg));
        });

    QObject::connect(this->ui->textEdit_BackendInfo, &ElaPlainTextEdit::textChanged, this, [this]() {
        this->ui->textEdit_BackendInfo->moveCursor(QTextCursor::End);
        });



    QObject::connect(this->ui->pushButton_connect, &QPushButton::clicked, this, &BackendManager::ConnectionButtonClickedSlot);

    //QObject::connect(eTheme, &ElaTheme::themeModeChanged, this, &BackendManager::ThemeChanged);
    //this->ThemeChanged(eTheme->getThemeMode());
    //this->repaint();
}

BackendManager::~BackendManager()
{
    this->BackendProcess__->disconnect();
    if (this->BackendProcess__->state() != QProcess::ProcessState::NotRunning)
    {
        this->TerminateBackend();
    }
    delete ui;
}

void BackendManager::ResetUI()
{
    if (!this->isRunning())
        this->ui->textEdit_BackendInfo->clear();
}

bool BackendManager::isRunning()
{
    return (this->BackendProcess__->state() != QProcess::ProcessState::NotRunning);
}

bool BackendManager::StartBackend()
{
    if ((this->BackendProcess__->state() != QProcess::ProcessState::NotRunning))
        return false;

    this->ConnectionButtonClickedSlot();
    qApp->processEvents();
    return true;
}

void BackendManager::ThemeChanged(ElaThemeType::ThemeMode themeMode)
{
}

void BackendManager::closeEvent(QCloseEvent* event)
{
    //FIXME: fix close event not working issue
    // if (this->BackendProcess__->state() != QProcess::ProcessState::NotRunning)
    // {
    //     int button = QMessageBox::warning(this, tr("Backend is Still Running"), tr("Backend is still running, would you like to terminate it now?"), QMessageBox::Yes, QMessageBox::No);
    //     if (button == QMessageBox::Yes)
    //     {
    //         this->TerminateBackend();
    //     }
    //     else
    //     {
    //         int button = QMessageBox::warning(this, tr("Unmanaged Backend is Danger!"), tr("The backend is still running and can only be killed by task manager, do you really want to close the window with backend running still?"), QMessageBox::Yes, QMessageBox::No);
    //         if (button == QMessageBox::No)
    //         {
    //             event->ignore();
    //         }
    //     }
    // }
}

void BackendManager::TerminateBackend()
{
    if (this->isRemote)
    {
        QString Cmd = "ssh";
        QStringList args;
        args.append("-o ConnectTimeout=2");
        args.append(this->UserName + "@" + this->IP);
        args.append("pkill");
        args.append(this->ExecName);
        QProcess::execute(Cmd, args);
    }
    else
    {
        this->BackendProcess__->terminate();
    }
}

void BackendManager::ConnectionButtonClickedSlot()
{
    if (this->ui->pushButton_connect->text() == tr("Connect"))
    {
        this->ui->textEdit_BackendInfo->clear();
        this->ui->pushButton_connect->setText(tr("Connecting"));
        this->ui->pushButton_connect->setEnabled(false);
        //QStringList args;
        //args.push_back("bitbot@192.168.8.112");
        //args.push_back("/home/bitbot/cetc_robot_ganzhi_v2/build/bin/main_app");

        QString Cmd;
        QStringList args;
        if (!this->isRemote)
        {
            Cmd = this->ExecPath + "/" + this->ExecName;  //FIXME: fix local run issue!
        }
        else
        {
            //Cmd = "wsl";
            Cmd = "ssh";
            args.append("-o ConnectTimeout=2");
            args.append(this->UserName + "@" + this->IP);
            args.append("pkill");
            args.append(this->ExecName + ";");
            args.append("cd");
            args.append(this->ExecPath + ";");
            args.append("./" + this->ExecName);
        }

        this->BackendProcess__->start(Cmd, args);
    }
    else if (this->ui->pushButton_connect->text() == tr("Stop"))
    {
        this->ui->pushButton_connect->setText(tr("Force Stop"));
        this->ui->pushButton_connect->setEnabled(true);
        //this->BackendProcess__->terminate();
        /*QStringList args;
        args.append("bitbot@192.168.8.112");
        args.append("pkill");
        args.append("main_app");
        QProcess::execute("ssh", args);*/

        this->TerminateBackend();
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
}
