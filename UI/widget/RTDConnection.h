#pragma once

#include "MetaRTDView.h"
#include "ui_RTDConnection.h"

QT_BEGIN_NAMESPACE
namespace Ui { class RTDConnectionClass; };
QT_END_NAMESPACE

class RTDConnection : public MetaRTDView
{
	Q_OBJECT

public:
	RTDConnection(QWidget *parent = nullptr);
	~RTDConnection();
	void ResetUI();

signals:
	void ConnectionButtonClicked();


private:
	Ui::RTDConnectionClass *ui;
};
