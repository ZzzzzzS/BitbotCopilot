#pragma once
#include <QApplication>
#include "parser.h"
#include "QCommandLineParser"
#include "QCommandLineOption"
#include "QProcess"
#include "QDebug"
#include "QFile"
#include "QThread"
#include "QFileInfo"

#define ZCP CmdParser::getInstance()
#define ZCP_INIT CmdParser::getInstance()
class CmdParser
{
public:
    static CmdParser& getInstance()
    {
        static CmdParser instance;
        return instance;
    }
public:
    CmdParser()
        :m_UpdateMode("upgrade_mode", "for internal use ONLY, indication of software update"),
        m_NoSplash("no_splash", "Disable splash screen"),
        m_UpdatePath("upgrade_from", "for internal use ONLY, indication of software update"),
        m_UpdatePath2("upgrade_to", "for internal use ONLY, indication of software update")
    {
        m_UpdatePath.setValueName("path");
        m_UpdatePath2.setValueName("path");

        m_parser.addVersionOption();
        m_parser.addHelpOption();

        m_parser.addOption(m_UpdateMode);
        m_parser.addOption(m_NoSplash);
        m_parser.addOption(m_UpdatePath);
        m_parser.addOption(m_UpdatePath2);

        m_parser.process(qApp->arguments());
    }

    bool isNoSplashScreen() const
    {
        return m_parser.isSet("no_splash");
    }

    bool isUpgradeMode() const
    {
        return m_parser.isSet("upgrade_mode") && !m_parser.value(m_UpdatePath).isEmpty() && !m_parser.value(m_UpdatePath2).isEmpty();
    }

    bool ProcessUpgradeLogic()
    {
        //检查指定的待升级程序是否存在
        QString updateFrom = m_parser.value(m_UpdatePath);
        QString updateTo = m_parser.value(m_UpdatePath2);

        //检查待升级程序是否存在
        if (!QFile::exists(updateFrom))
        {
            qDebug() << "Update source file does not exist:" << updateFrom;
            return false;
        }

        //获取升级后历史程序的名称并将待升级程序拷贝入位置
        QFile::copy(updateFrom, updateTo);

        //删除源程序，并拷贝本程序到目标位置
        for (int try_ = 0;try_ < 10;try_++)
        {
            if (QFile::remove(updateFrom))
                break;
            QThread::sleep(3);

            if (try_ == 9)
            {
                qDebug() << "Failed to remove update source file:" << updateFrom;
                return false;
            }
        }

        QFile::copy(QCoreApplication::applicationFilePath(), updateFrom);

        //启动目标程序
        QString WorkingDir = QFileInfo(updateFrom).absolutePath();
        return QProcess::startDetached(updateFrom, {}, WorkingDir);
    }
private:
    QCommandLineParser m_parser;
    QCommandLineOption m_UpdateMode;
    QCommandLineOption m_NoSplash;
    QCommandLineOption m_UpdatePath;
    QCommandLineOption m_UpdatePath2;
};
