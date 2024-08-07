#pragma once

#include "MetaRTDView.h"
#include "ui_RTDKernelStates.h"

QT_BEGIN_NAMESPACE
namespace Ui { class RTDKernelStatesClass; };
QT_END_NAMESPACE

class RTDKernelStates : public MetaRTDView
{
	Q_OBJECT

public:
	RTDKernelStates(QWidget *parent = nullptr);
	~RTDKernelStates();

	void UpdateKernelStatus(const QString& Headers, const QVariantList& Info);

private:
	Ui::RTDKernelStatesClass *ui;
};
