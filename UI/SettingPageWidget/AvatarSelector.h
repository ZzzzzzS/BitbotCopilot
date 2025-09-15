#pragma once
#include "QWidget"
#include "ElaListView.h"
#include "QStandardItemModel"
#include "QStandardItem"
#include "QPixmap"
#include "ElaFlowLayout.h"
#include "ElaWidget.h"

class AvatarItem : public QWidget
{
    Q_OBJECT
public:
    AvatarItem(QWidget* parent = nullptr, QString avatarPath = QString());
    ~AvatarItem();
    void SetSelected(bool selected) { this->isSelected__ = selected; update(); }
    bool isSelected() const { return this->isSelected__; }
    QString GetAvatarPath() const { return this->avatarPath__; }
signals:
    void clicked(AvatarItem* item);

protected:
    virtual void paintEvent(QPaintEvent* event) override;
    virtual void mousePressEvent(QMouseEvent* event) override;
    virtual void enterEvent(QEnterEvent* event) override;
    virtual void leaveEvent(QEvent* event) override;

private:
    QString avatarPath__;
    QPixmap avatarPixmap__;
    bool isSelected__ = false;
};

class AvatarSelector : public QWidget
{
    Q_OBJECT
public:
    explicit AvatarSelector(QWidget* parent = nullptr);
    ~AvatarSelector();

    bool SetCurrentAvatar(QString avatarPath);
    QString GetCurrentAvatar();

signals:
    void avatarChanged(QString avatarPath);

private:
    ElaFlowLayout* flowLayout__;
    QList<AvatarItem*> avatarItems__;
    AvatarItem* currentSelectedItem__ = nullptr;

    void LoadAvatars();
    void SwitchSelectedItem(AvatarItem* item);
};