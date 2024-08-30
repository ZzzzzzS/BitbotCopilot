#pragma once

#include "MetaRTDView.h"
#include "ui_RTDDeviceInfo.h"
#include <QString>
#include <QVector>
#include <QVariant>
#include <QWidgetItem>
#include "ElaTheme.h"
#include "RTDDeviceInfoTableModel.h"

QT_BEGIN_NAMESPACE
namespace Ui { class RTDDeviceInfoClass; };
QT_END_NAMESPACE

class RTDDeviceInfo : public MetaRTDView
{
	Q_OBJECT

public:
	RTDDeviceInfo(QWidget* parent = nullptr);
	~RTDDeviceInfo();

	bool setHeaders(const QString& DeviceType, const QVector<QString>& DeviceList, const QVector<QString>& DeviceSensors);
	bool setHeaders(const QVector<QString>& UserDataHeader);
	QString DeviceTypeName();

public: //slots
	void UpdateDeviceInfo(QVariantList data);
	size_t getHeaderSize();
	void ResetUI();

private:
	Ui::RTDDeviceInfoClass* ui;
	bool isDeviceInfo__ = false;
	bool isUserExtraData__ = false;
	QVector<QString> VerticalHeader;
	QVector<QString> HorizentalHeader;
	size_t RowLength = 0;
	size_t RowNumber = 0;
	QString DeviceTypeName__;
	RTDDeviceInfoRTableModel* Model__;
};
