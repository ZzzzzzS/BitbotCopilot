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
#include "Communication/LocalCommand.hpp"
#include "Communication/RCM/RemoteCommand.hpp"

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
		QString CmdCombo = "pkill -9 " + this->ExecName + "; cd " + this->ExecPath + " && ./" + this->ExecName;
        this->CmdHandle = new zzs::RemoteCommand(CmdCombo, QStringList(), this);
    }
    else
    {
		QString CmdCombo = this->ExecPath + "/" + this->ExecName;
        this->CmdHandle = new zzs::LocalCommand(CmdCombo, QStringList(), this->ExecPath, this);
    }

    this->ui->label_icon->setText(QChar(ElaIconType::Server));
	QFont font = this->ui->label_icon->font();
	font.setPointSize(28);
    font.setFamily("ElaAwesome");
	this->ui->label_icon->setFont(font);
	this->ui->label_icon->setAlignment(Qt::AlignCenter);
    //this->ui->textEdit_BackendInfo->setReadOnly(true);
    //this->ui->textEdit_BackendInfo->setBackgroundVisible(true);

    QObject::connect(this->CmdHandle, &zzs::MetaBackendCommander::StateChanged, this, [this](zzs::MetaBackendCommander::ProcessState newState) {
        switch (newState)
        {
        case zzs::MetaBackendCommander::ProcessState::NotRunning:
            this->ui->pushButton_connect->setEnabled(true);
            this->ui->pushButton_connect->setText(tr("Connect"));
            break;
        case zzs::MetaBackendCommander::ProcessState::Starting:
            break;
        case zzs::MetaBackendCommander::ProcessState::Running:
        {
            this->ui->pushButton_connect->setEnabled(true);
            this->ui->pushButton_connect->setText(tr("Stop"));
            break;
        }
        default:
            break;
        }
        });

    QObject::connect(this->CmdHandle, &zzs::MetaBackendCommander::CommandError, this, [this](const QString& ErrMsg) {
        FluentMessageBox::warningOk(this, tr("Bitbot Backend Error"), tr("Bitbot backend error, backend ") + ErrMsg, QMessageBox::Ok);
        });

    QObject::connect(this->CmdHandle, &zzs::MetaBackendCommander::CommandFinished, this, [this]() {
        this->ui->pushButton_connect->setEnabled(true);
        this->ui->pushButton_connect->setText(tr("Connect"));
        });

    QObject::connect(this->CmdHandle, &zzs::MetaBackendCommander::CommandOutput, this, [this](QByteArray msg) {
		this->ui->textEdit_BackendInfo->appendPlainText(QString(msg));
	});

    QObject::connect(this->ui->textEdit_BackendInfo, &ElaPlainTextEdit::textChanged, this, [this]() {
        this->ui->textEdit_BackendInfo->moveCursor(QTextCursor::End);
        });



    QObject::connect(this->ui->pushButton_connect, &QPushButton::clicked, this, &BackendManager::ConnectionButtonClickedSlot);

    QObject::connect(eTheme, &ElaTheme::themeModeChanged, this, &BackendManager::ThemeChanged);
    this->ThemeChanged(eTheme->getThemeMode());
    //this->repaint();
}

BackendManager::~BackendManager()
{
	if (this->CmdHandle->isRunning())
		this->CmdHandle->Kill();
    delete ui;
}

void BackendManager::ResetUI()
{
    if (!this->isRunning())
        this->ui->textEdit_BackendInfo->clear();
}

bool BackendManager::isRunning()
{
    return this->CmdHandle->isRunning();
}

bool BackendManager::StartBackend()
{
    if(this->CmdHandle->isRunning())
		return false;

    this->ConnectionButtonClickedSlot();
    qApp->processEvents();
    return true;
}

void BackendManager::ThemeChanged(ElaThemeType::ThemeMode themeMode)
{
	switch (themeMode)
	{
	case ElaThemeType::ThemeMode::Light:
		this->ui->textEdit_BackendInfo->setStyleSheet("background-color: white; color: black;");
		break;
	case ElaThemeType::ThemeMode::Dark:
		this->ui->textEdit_BackendInfo->setStyleSheet("background-color: black; color: white;");
		break;
	default:
		break;
	}
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
    bool rc = this->CmdHandle->Stop();
}

void BackendManager::ConnectionButtonClickedSlot()
{
    if (this->ui->pushButton_connect->text() == tr("Connect"))
    {
        this->ui->textEdit_BackendInfo->clear();
        this->ui->pushButton_connect->setText(tr("Connecting"));
        this->ui->pushButton_connect->setEnabled(false);
        qApp->processEvents();
        this->CmdHandle->Start();
    }
    else if (this->ui->pushButton_connect->text() == tr("Stop"))
    {
        this->ui->pushButton_connect->setText(tr("Force Stop"));
        this->ui->pushButton_connect->setEnabled(true);
        this->TerminateBackend();
        qApp->processEvents();
    }
    else if (this->ui->pushButton_connect->text() == tr("Connecting"))
    {

    }
    else if (this->ui->pushButton_connect->text() == tr("Force Stop"))
    {
        this->ui->pushButton_connect->setEnabled(true);
        this->CmdHandle->Kill();
        qApp->processEvents();
    }
    else
    {
        qDebug() << "backend: unknown button status";
    }
}
