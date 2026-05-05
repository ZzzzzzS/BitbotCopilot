#pragma once
#include "QWidget"
#include "ElaWidget.h"
#include "ElaLineEdit.h"
#include "ElaText.h"
#include "AvatarSelector.h"
#include "UI/widget/CustomImageLabel.hpp"

class AvatarNameEditor : public QWidget
{
    Q_OBJECT
public:
    AvatarNameEditor(QWidget* parent = nullptr);
    ~AvatarNameEditor();

    void setAvatarPath(QString path);
    QString getAvatarPath() const;

    void setUserName(QString name);
    QString getUserName() const;

    void setNameEditable(bool editable);

protected:
    virtual void paintEvent(QPaintEvent* event) override;

private: //slots
    void onAvatarChanged(QString path);
private:
    CustomImageLabel* LargeAvatarLabel__;
    ElaText* title__;
    ElaText* userNameInfo__;
    ElaLineEdit* userName__;
    AvatarSelector* avatarSelector__;

};