#pragma once
#include <QObject>
#include <QString>
#include <QDialog>
#include <QThread>
#include <stack>
#include <QPropertyAnimation>

#include "ElaWidget.h"
#include "ElaListView.h"
#include "ElaPushButton.h"
#include "ElaMenu.h"
#include "ElaText.h"
#include "ElaLineEdit.h"
#include "ElaTheme.h"
#include "ElaTreeView.h"
#include "ElaBreadcrumbBar.h"
#include "ElaAppBar.h"

#include "UI/widget/FluentLoadingWidget.h"
#include "Communication/RCM/SftpFileSystemModel.h"
#include "Communication/RCM/SessionManager.h"

class RemoteFileSelectorDisConnectedWidget : public QWidget
{
public:
	RemoteFileSelectorDisConnectedWidget(QWidget* parent = nullptr);
	~RemoteFileSelectorDisConnectedWidget();

private:
	void ThemeChanged(ElaThemeType::ThemeMode mode);
	ElaText* Notation__;
	ElaText* Title__;
	QLabel* Icon__;
};

class RemoteFileSelector : public QDialog
{
	Q_OBJECT

public:
	static QString getOpenFileName(QWidget* parent = nullptr, const QString& caption = QString(), const QString& dir = QString());
public:
	RemoteFileSelector(QWidget* parent = nullptr, const QString& caption = QString(), const QString& dir = QString());
	~RemoteFileSelector();
signals:
	void FileSelected(const QString& file);
	void Cancelled();
	void Error(const QString& error);

protected:
	void paintEvent(QPaintEvent* event);

private:
	ElaPushButton* CreateFixSizeButton(const QString& text);
	void ThemeChanged(ElaThemeType::ThemeMode mode);
	void ShowConnectedUI(bool show);
	void EnableButton(bool enable);

	void PathChanged(const QString& path);
	void ForwardButtonClicked();
	void BackwardButtonClicked();
	void UpButtonClicked();
	void RefreshButtonClicked();
	void FileDoubleClicked(const QModelIndex& index);
private:
	ElaPushButton* ForwardButton__;
	ElaPushButton* BackwardButton__;
	ElaPushButton* UpButton__;
	ElaPushButton* RefreshButton__;
	ElaBreadcrumbBar* PathBar__;
	QListView* FileList__;
	ElaMenu* Menu__;
	ElaText* LoadingIcon__;
	SimpleInfinateLoadingWidget* LoadingWidget__;
	SftpFileSystemModel* FileSystemModel__;
	QThread* FileSystemThread__;
	//ElaAppBar* AppBar__;
	RemoteFileSelectorDisConnectedWidget* DisconnectedWidget__;

	ElaThemeType::ThemeMode _themeMode;
	bool _isEnableMica;

};