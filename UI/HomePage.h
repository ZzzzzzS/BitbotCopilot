#pragma once
#include "ElaScrollPage.h"

class ElaReminderCard;
class ElaAcrylicUrlCard;

class HomePage : public ElaScrollPage
{
	Q_OBJECT
public:
	explicit HomePage(QWidget* parent = nullptr);
	~HomePage();

signals:
	void AttachBitbotSignal();
	void LaunchBitbotSignal();
	void AutoRunBitbotSignal();
	void ViewDataSignal();

private:
	void InitBackground();
	void InitQuickStart();

private:
	ElaReminderCard* AttachBitbotCard;
	ElaReminderCard* LaunchBitbotCard;
	ElaReminderCard* AutoRunBitbotCard;
	ElaReminderCard* DataViewerCard;
	//ElaReminderCard* LaunchVSCCard;

	ElaAcrylicUrlCard* UrlCard2;
	ElaAcrylicUrlCard* UrlCard1;

};