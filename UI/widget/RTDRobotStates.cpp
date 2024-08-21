#include "RTDRobotStates.h"
#include <QPixmap>

RTDRobotStates::RTDRobotStates(QWidget *parent)
	: MetaRTDView(MetaRTDView::RTDViewType::SMALL_WINDOW, parent)
	, ui(new Ui::RTDRobotStatesClass())
{
	ui->setupUi(this);
	this->ResetUI();
}

RTDRobotStates::~RTDRobotStates()
{
	delete ui;
}

void RTDRobotStates::SetHeaderList(const QMap<int, QString>& header)
{
	this->states = header;
}

void RTDRobotStates::UpdateStates(QVariant v)
{
	int s = v.toInt();
	QString state_name;
	if (!this->states.contains(s))
	{
		state_name = QString(tr("Unknown State ")) + state_name;
		this->ui->label_icon->setPixmap(QPixmap(":/UI/Image/error_state_icon.png").scaledToHeight(this->icon_height));
		this->ui->Label_CurrentState->setText(state_name);
	}

	if (this->states[s] == "kernel_idle")
	{
		this->ui->label_icon->setPixmap(QPixmap(":/UI/Image/kernel_idle_icon.png").scaledToHeight(this->icon_height));
	}
	else if (this->states[s] == "power on")
	{
		this->ui->label_icon->setPixmap(QPixmap(":/UI/Image/kernel_power_on_icon.png").scaledToHeight(this->icon_height));
	}
	else if (this->states[s] == "power on finish")
	{
		this->ui->label_icon->setPixmap(QPixmap(":/UI/Image/kernel_poweron_finished_icon.png").scaledToHeight(this->icon_height));
	}
	else if (this->states[s] == "kernel_stopped")
	{
		this->ui->label_icon->setPixmap(QPixmap(":/UI/Image/kernel_stop_icon.png").scaledToHeight(this->icon_height));
	}
	else
	{
		this->ui->label_icon->setPixmap(QPixmap(":/UI/Image/robot_state.png").scaledToHeight(this->icon_height));
	}

	this->ui->Label_CurrentState->setText(this->states[s]);
}

void RTDRobotStates::ResetUI()
{
	this->ui->label_icon->setPixmap(QPixmap(":/UI/Image/disconnected_icon.png").scaledToHeight(this->icon_height));
	this->ui->Label_CurrentState->setText("Disconnected");
}
