#include "RTDKernelStates.h"

RTDKernelStates::RTDKernelStates(QWidget *parent)
	: MetaRTDView(MetaRTDView::RTDViewType::SMALL_WINDOW,parent)
	, ui(new Ui::RTDKernelStatesClass())
{
	ui->setupUi(this);
}

RTDKernelStates::~RTDKernelStates()
{
	delete ui;
}

void RTDKernelStates::UpdateKernelStatus(const QString& Headers, const QVariantList& Info)
{
	this->ui->Label_PeriodCount->setText(QString::number(Info[0].toInt()));
	this->ui->Label_PeriodTime->setText(QString::number(Info[1].toFloat()));
	this->ui->Label_ProcessTime->setText(QString::number(Info[2].toFloat()));
	this->ui->Label_KernelTime->setText(QString::number(Info[3].toFloat()));

	float percentage_f;
	if (Info[1].toFloat() != 0)
		percentage_f = Info[2].toFloat() / Info[1].toFloat() * 100;
	else
		percentage_f = 0;
	int percentage = static_cast<int>(std::round(percentage_f));

	QString p = QString::number(percentage) + "%";
	this->ui->Label_Percentage->setText(p);
}

void RTDKernelStates::ResetUI()
{
	QVariantList Info;
	Info.push_back(0);
	Info.push_back(0);
	Info.push_back(0);
	Info.push_back(0);
	this->UpdateKernelStatus("N/A", Info);
}
