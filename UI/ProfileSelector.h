#pragma once
#include <QObject>
#include <ElaWidget.h>
#include "ElaInteractiveCard.h"
#include "QList"
#include "ElaPushButton.h"

class ProfileIDCard : public ElaInteractiveCard
{
    Q_OBJECT
public:
    ProfileIDCard(QWidget* parent, QString profileName, QString ipAddress, QString avatar);
    ~ProfileIDCard();

    void setSelected(bool select);
    bool isSelected();
    QString getProfileName();

protected:
    void paintEvent(QPaintEvent* event) override;
    bool m_selected;
    QString m_profileName;
};

class ProfileSelector : public ElaWidget
{
    Q_OBJECT

public:
    static QString Select();

public:
    ProfileSelector(QWidget* parent = nullptr);
    ~ProfileSelector();

signals:
    void selected(QString);

private:
    void InitSelectButton();
    void InitCurrentProfile(QWidget* parent);
    void InitAvailableProfiles(QWidget* parent);
    void SwitchSelectedCard(ProfileIDCard* card);

private:
    ProfileIDCard* CurrentProfileCard;
    ProfileIDCard* CurrentSelectedCard;

    QList<ProfileIDCard*> AvailableProfileCards;

    ElaPushButton* SelectedButton;
};