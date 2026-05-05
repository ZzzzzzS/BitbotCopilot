#pragma once
#include <QObject>
#include <ElaWidget.h>
#include "QList"
#include "ElaPushButton.h"
#include "UI/widget/ProfileIDCard.h"

class ProfileSelector : public ElaWidget
{
    Q_OBJECT

public:
    static QString Select();
    static int Select(QString& profile_name);

public:
    ProfileSelector(QWidget* parent = nullptr);
    ~ProfileSelector();

signals:
    void selected(QString);
    void StateCode(int); // 0: success, 1: cancel, 2: new profile

private:
    void InitSelectButton();
    void InitAddNewProfileButton(QWidget* parent);
    void InitCurrentProfile(QWidget* parent);
    void InitAvailableProfiles(QWidget* parent);
    void SwitchSelectedCard(ProfileIDCard* card);


private:
    ProfileIDCard* CurrentProfileCard;
    ProfileIDCard* CurrentSelectedCard;
    ProfileIDCard* AddNewProfileCard;

    QList<ProfileIDCard*> AvailableProfileCards;

    ElaPushButton* SelectedButton;
};
