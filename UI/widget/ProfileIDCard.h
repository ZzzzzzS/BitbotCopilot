#pragma once
#include "ElaInteractiveCard.h"
#include "QList"

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
