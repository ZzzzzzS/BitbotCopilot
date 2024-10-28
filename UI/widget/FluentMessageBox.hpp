#pragma once
#include <QObject>
#include <QMessageBox>
#include "ElaContentDialog.h"

class FluentMessageBox : public QObject
{
	Q_OBJECT
public:
	static void informationOk(QWidget* parent, const QString& title, const QString& text,QMessageBox::StandardButton buttons=QMessageBox::Ok)
	{
		QString title_;
		if (title.back() != '.' || title.back() != '!' || title.back() != '?')
		{
			title_ = title + ". ";
		}
		else if (title.back() == ' ')
		{
			title_ = title;
		}
		else
		{
			title_ = title + " ";
		}
		auto Wid = new ElaContentDialog(parent, QString(tr("Infomation: ")), title_ + text);
		//Wid->setTitleSubTitle(QString(tr("Infomation: ")), title_ + text);
		Wid->setButtonNumber(1);
		Wid->setRightButtonText(tr("Ok"), true);
		auto rtn = Wid->exec();
		delete Wid;
	}

	static void warningOk(QWidget* parent, const QString& title, const QString& text, QMessageBox::StandardButton buttons = QMessageBox::Ok)
	{
		QString title_;
		if (title.back() != '.' || title.back() != '!' || title.back() != '?')
		{
			title_ = title + ". ";
		}
		else if (title.back() == ' ')
		{
			title_ = title;
		}
		else
		{
			title_ = title + " ";
		}

		auto Wid = new ElaContentDialog(parent, QString(tr("Warning: ")), title_ + text);
		Wid->setTitleSubTitle(QString(tr("Warning: ")), title_ + text);
		Wid->setButtonNumber(1);
		Wid->setRightButtonText(tr("Ok"), true);
		auto rtn = Wid->exec();
		delete Wid;
	}

	static void criticalOk(QWidget* parent, const QString& title, const QString& text, QMessageBox::StandardButton buttons = QMessageBox::Ok)
	{
		QString title_;
		if (title.back() != '.' || title.back() != '!' || title.back() != '?')
		{
			title_ = title + ". ";
		}
		else if (title.back() == ' ')
		{
			title_ = title;
		}
		else
		{
			title_ = title + " ";
		}

		auto Wid = new ElaContentDialog(parent, QString(tr("Critical: ")), title_ + text);
		//Wid->setTitleSubTitle(QString(tr("Critical: ")), title_ + text);
		Wid->setButtonNumber(1);
		Wid->setRightButtonText(tr("Ok"), true);
		auto rtn = Wid->exec();
		delete Wid;
	}
};