#pragma once
#include "QWidget"
#include "ElaText.h"
#include "ElaTheme.h"
#include "QString"


class SettingsLineInfoWidget : public QWidget
{
    Q_OBJECT

public:
    SettingsLineInfoWidget(QWidget* parent, const QString& title, const QString& subtitle = QString(), const QChar& icon = QChar());
    ~SettingsLineInfoWidget();

    void setTitleFontSize(int size);

private:
    ElaText* title__;
    ElaText* subtitle__;
    ElaText* icon__;

};