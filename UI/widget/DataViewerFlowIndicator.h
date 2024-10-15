#pragma once
#include "ElaWidget.h"
#include "ElaTheme.h"
#include "QList"
#include "ElaText.h"
#include "QLabel"
#include "QHBoxLayout"
#include "QVBoxLayout"

class DataViewerFlowIndicator : public QWidget
{
	Q_OBJECT
public:
	DataViewerFlowIndicator(QWidget* Parent = nullptr);
	~DataViewerFlowIndicator();

	void UpdateValue(const QList<double>& value);
	void SetLabelText(const QList<QString>& text, const QList<QColor>& Colors);
	void UpdateColor(const QList<QColor>& Colors);

protected:
	virtual void paintEvent(QPaintEvent* event) override;
	void enterEvent(QEvent* event);
	void mouseMoveEvent(QMouseEvent* event);
	void leaveEvent(QEvent* event);
private:
	void ThemeChanged(ElaThemeType::ThemeMode theme);

private:
	ElaThemeType::ThemeMode _themeMode;
	bool _isEnableMica;

	QList<QLabel*> CurveIconList__;
	QList<ElaText*> CurveNameList__;
	QList<ElaText*> CurveValueList__;
	QList<QHBoxLayout*> CurveLayout__;
	QList<QColor> ColorPairList__;
	QVBoxLayout* VLay__;
};
