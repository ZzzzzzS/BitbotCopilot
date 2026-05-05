#pragma once
#include "ElaScrollPage.h"

class QStackedWidget;
class QVBoxLayout;
class ElaPushButton;
class ElaText;
class ElaBreadcrumbBar;
class RobotConfigForm;
class ProfileIDCard;

class SettingsPage : public ElaScrollPage
{
    Q_OBJECT

public:
    SettingsPage(QWidget* parent = nullptr);
    ~SettingsPage();

private slots:
    void onCardClicked();
    void onAddNewClicked();
    void onEditorSaveClicked();
    void onEditorDeleteClicked();

private:
    void initSelectionPage();
    void initEditorPage();
    void reloadUserCards();
    void loadProfileToEditor(const QString& profileName);
    void setEditorDeleteVisible(bool visible);
    void backToSelectionPage();
    
    QWidget* selectionPage__;
    QVBoxLayout* cardsLayout__;
    
    ProfileIDCard* currentProfileCard__;
    ProfileIDCard* addNewProfileCard__;
    QList<ProfileIDCard*> otherProfileCards__;
    
    QWidget* editorPage__;
    RobotConfigForm* configForm__;
    ElaPushButton* editorSaveButton__;
    ElaPushButton* editorDeleteButton__;
    
    QString editingProfileName__;
    bool isNewProfileMode__;
    
    QStackedWidget* stackedWidget__;
    ElaBreadcrumbBar* breadcrumbBar__;
};
