#pragma once

#include "ElaWidget.h"
#include "ElaPushButton.h"

class RobotConfigForm;

class PopupSettingsPage : public ElaWidget
{
    Q_OBJECT
public:
    PopupSettingsPage(QWidget* parent = nullptr);
    ~PopupSettingsPage();

    QString exec();

signals:
    void NewProfileCreated(QString ProfileName);

private:
    void InitBottomButton();
    void paintEvent(QPaintEvent* event) override;

private:
    void onConfirmClicked();

private:
    RobotConfigForm* configForm__;

    ElaPushButton* confirmButton__;
    ElaPushButton* cancelButton__;
};
