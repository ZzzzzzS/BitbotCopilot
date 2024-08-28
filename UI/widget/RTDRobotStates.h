#pragma once

#include "MetaRTDView.h"
#include "ui_RTDRobotStates.h"
#include "QVariant"

QT_BEGIN_NAMESPACE
namespace Ui { class RTDRobotStatesClass; };
QT_END_NAMESPACE

class RTDRobotStates : public MetaRTDView
{
	Q_OBJECT

public:
	RTDRobotStates(QWidget *parent = nullptr);
	~RTDRobotStates();
	void SetHeaderList(const QMap<int, QString>& header);
	void UpdateStates(QVariant v);
	void ResetUI();
	QString CurrentState();

private:
	Ui::RTDRobotStatesClass *ui;
	QMap<int, QString> states;
	const size_t icon_height = 63;
};
