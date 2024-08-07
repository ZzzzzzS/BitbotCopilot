#pragma once
#include <QWidget>
#include "ElaScrollPage.h"
#include "ElaScrollPageArea.h"
#include "widget/RTDKernelStates.h"
#include "widget/RTDRobotStates.h"
#include "widget/RTDGamepadStatus.h"
#include "widget/RTDKeyboardStatus.h"


class PilotPage : public ElaScrollPage
{
	Q_OBJECT

public:
	PilotPage(QWidget* parent = nullptr);
	~PilotPage();

private:
	void InitConnectionWidget();

private:
	RTDKernelStates* KernelStatusUI;
	RTDRobotStates* RobotStateUI;
	RTDKeyboardStatus* KeyboardEventUI;
	RTDGamepadStatus* GamepadStatusUI;
	ElaScrollPageArea* ConnectionAreaUI;
};