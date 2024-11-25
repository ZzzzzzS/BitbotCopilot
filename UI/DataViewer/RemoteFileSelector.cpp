#include "RemoteFileSelector.h"
#include "QFont"
#include "QHBoxLayout"
#include "QVBoxLayout"
#include "QFileSystemModel"
#include "QPainter"
#include "ElaTheme.h"
#include "QSpacerItem"
#include "ElaApplication.h"
#include "UI/widget/FluentMessageBox.hpp"
#include "QTimer"
#include "ElaScrollBar.h"
#include "QApplication"
#include "QScroller"
#include "QProgressDialog"
#include "QHeaderView"
#include "ElaTableView.h"
#include "QSortFilterProxyModel"

#include "Utils/Settings/SettingsHandler.h"

QString RemoteFileSelector::getOpenFileName(QWidget* parent, const QString& caption, const QString& dir)
{
	RemoteFileSelector* selector = new RemoteFileSelector(parent, caption, dir);
	//selector->setAttribute(Qt::WA_DeleteOnClose);
	QString file;
	QObject::connect(selector, &RemoteFileSelector::FileSelected, [&file,selector](const QString& fileSelected) {
		file = fileSelected;
		selector->close();
		qDebug() << "FileSelected";
		});

	QObject::connect(selector, &RemoteFileSelector::Cancelled, [&file, selector]() {
		file = QString();
		selector->close();
		});

	QObject::connect(selector, &RemoteFileSelector::Error, [&file, selector,parent](const QString& error) {
		file = QString();
		FluentMessageBox::warningOk(parent, "Error:", error);
		});
	selector->exec();

	auto closingwid = new QProgressDialog(tr("Closing remote file selector, please wait..."), "", 0, 0, parent);
	closingwid->show();
	for (size_t i = 0; i < 10; i++)
	{
		qApp->processEvents(); //have to call this to make the dialog show up
	}
	delete selector;
	closingwid->close();
	qApp->processEvents();
	delete closingwid;
	return file;
}

RemoteFileSelector::RemoteFileSelector(QWidget* parent, const QString& caption, const QString& dir)
	:QDialog(parent),
	ShowListMode__(false)
{
	//set window attributes
	this->setWindowModality(Qt::ApplicationModal);
	this->setWindowTitle(caption);
	this->setMinimumSize(600, 400);

	this->_themeMode = eTheme->getThemeMode();
	this->_isEnableMica = eApp->getIsEnableMica();

	//create widgets
	this->ForwardButton__ = this->CreateFixSizeButton(QChar(ElaIconType::ArrowRight));
	this->BackwardButton__ = this->CreateFixSizeButton(QChar(ElaIconType::ArrowLeft));
	this->UpButton__ = this->CreateFixSizeButton(QChar(ElaIconType::ArrowUp));
	this->RefreshButton__ = this->CreateFixSizeButton(QChar(ElaIconType::ArrowRotateRight));
	this->ShowList__ = this->CreateFixSizeButton(QChar(ElaIconType::Bars));
	this->ForwardButton__->setEnabled(false);
	this->BackwardButton__->setEnabled(false);
	this->UpButton__->setEnabled(false);
	this->ShowList__->setEnabled(false);
	this->RefreshButton__->setEnabled(true);

	this->PathBar__ = new ElaBreadcrumbBar(this);
	this->PathBar__->setFixedHeight(25);

	this->FileList__ = new QListView(this);
	//this->FileList__->setIsTransparent(false);
	//this->FileList__->setItemHeight(50);

	this->FileList__->setObjectName("FileList");
	this->FileList__->setFlow(QListView::LeftToRight);
	this->FileList__->setViewMode(QListView::IconMode);
	this->FileList__->setResizeMode(QListView::Adjust);
	this->FileList__->setGridSize(QSize(100, 100));
	this->FileList__->setWrapping(true);
	this->FileList__->setWordWrap(true);
	this->FileList__->setSelectionRectVisible(true);
	this->FileList__->setMouseTracking(true);
	this->FileList__->setVerticalScrollBar(new ElaScrollBar(this));
	this->FileList__->setHorizontalScrollBar(new ElaScrollBar(this));
	this->FileList__->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
	this->FileList__->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
	this->FileList__->setHorizontalScrollMode(QAbstractItemView::ScrollPerPixel);
	this->FileList__->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
	QScroller::grabGesture(this->FileList__, QScroller::LeftMouseButtonGesture);

	this->FileTable__ = new QTableView(this);
	this->FileTable__->setShowGrid(false);
	this->FileTable__->verticalHeader()->setVisible(false);
	this->FileTable__->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeMode::ResizeToContents);
	this->FileTable__->verticalHeader()->setDefaultSectionSize(40);
	this->FileTable__->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
	this->FileTable__->setHorizontalScrollMode(QAbstractItemView::ScrollPerPixel);
	this->FileTable__->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
	QScroller::grabGesture(this->FileTable__, QScroller::LeftMouseButtonGesture);

	this->LoadingWidget__ = new SimpleInfinateLoadingWidget(12, this);
	this->LoadingWidget__->setFixedSize(35, 35);
	this->LoadingWidget__->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

	this->DisconnectedWidget__ = new RemoteFileSelectorDisConnectedWidget(this);

	//create layout
	this->PathBar__->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
	QHBoxLayout* topLayout = new QHBoxLayout();
	topLayout->addWidget(this->BackwardButton__);
	topLayout->addWidget(this->ForwardButton__);
	topLayout->addWidget(this->UpButton__);
	topLayout->addWidget(this->RefreshButton__);
	topLayout->addItem(new QSpacerItem(15, 0, QSizePolicy::Fixed, QSizePolicy::Fixed));
	topLayout->addWidget(this->PathBar__);
	topLayout->addItem(new QSpacerItem(5, 35, QSizePolicy::Fixed, QSizePolicy::Fixed));
	topLayout->addWidget(this->LoadingWidget__);
	topLayout->addWidget(this->ShowList__);

	QVBoxLayout* fileVLay = new QVBoxLayout();
	fileVLay->addSpacerItem(new QSpacerItem(0, 25, QSizePolicy::Minimum, QSizePolicy::Fixed));
	fileVLay->addWidget(this->FileList__);
	fileVLay->addWidget(this->FileTable__);
	fileVLay->addWidget(this->DisconnectedWidget__);
	fileVLay->setContentsMargins(0, 0, 0, 0);
	//fileVLay->addSpacerItem(new QSpacerItem(0, 10, QSizePolicy::Minimum, QSizePolicy::Fixed));

	QHBoxLayout* fileHLay = new QHBoxLayout();
	fileHLay->addSpacerItem(new QSpacerItem(10, 0, QSizePolicy::Fixed, QSizePolicy::Minimum));
	fileHLay->addLayout(fileVLay);
	fileHLay->setContentsMargins(0, 0, 0, 0);
	//fileHLay->addSpacerItem(new QSpacerItem(10, 0, QSizePolicy::Fixed, QSizePolicy::Minimum));

	QVBoxLayout* mainLayout = new QVBoxLayout();
	mainLayout->addLayout(topLayout);
	mainLayout->addLayout(fileHLay);
	this->setLayout(mainLayout);

	//create datamodel
	QString rootPath = ZSet->getBackendDataRootPath();
	this->FileSystemModel__ = new SftpFileSystemModel(rootPath, nullptr);
	this->FileSystemThread__ = new QThread(this);
	this->FileSystemModel__->moveToThread(this->FileSystemThread__);
	this->FileSystemThread__->start();


	//model->setRootPath(dir);
	this->FileList__->setModel(this->FileSystemModel__);
	this->FileTable__->setModel(this->FileSystemModel__);

	//create signal slots and cfg
	this->ShowConnectedUI(false);
	
	QObject::connect(this->FileSystemModel__, &SftpFileSystemModel::error, this, &RemoteFileSelector::Error);
	QObject::connect(this->FileSystemModel__, &SftpFileSystemModel::connectStateChanged, this, [this](bool conn) {
		qDebug() << "connectStateChanged";
		this->ShowConnectedUI(conn);
		});
	QObject::connect(this->FileSystemModel__, &SftpFileSystemModel::operationInProgressChanged, this, [this](bool inProgress) {
		this->EnableButton(!inProgress);
		if (inProgress)
		{
			this->LoadingWidget__->start(true);
			this->update(0, 0, this->width(), this->height());
			qApp->processEvents();
		}
		else
		{
			QTimer::singleShot(1000, this, [this]() {
				this->LoadingWidget__->start(false);
				this->update(0, 0, this->width(), this->height());
				qApp->processEvents();
			});
		}
	});

	QObject::connect(this->FileSystemModel__, &SftpFileSystemModel::pathChanged, this, &RemoteFileSelector::PathChanged);
	QObject::connect(this->ForwardButton__, &ElaPushButton::clicked, this, &RemoteFileSelector::ForwardButtonClicked);
	QObject::connect(this->BackwardButton__, &ElaPushButton::clicked, this, &RemoteFileSelector::BackwardButtonClicked);
	QObject::connect(this->UpButton__, &ElaPushButton::clicked, this, &RemoteFileSelector::UpButtonClicked);
	QObject::connect(this->RefreshButton__, &ElaPushButton::clicked, this, &RemoteFileSelector::RefreshButtonClicked);
	QObject::connect(this->FileList__, &QListView::doubleClicked, this, &RemoteFileSelector::FileDoubleClicked);
	QObject::connect(this->FileTable__,&QTableView::doubleClicked, this, &RemoteFileSelector::FileDoubleClicked);

	QObject::connect(this->ShowList__, &ElaPushButton::clicked, this, [this]() {
		this->ShowListMode__ = !this->ShowListMode__;
		if (this->ShowListMode__)
		{
			qDebug() << "set to grid";
			this->FileList__->setVisible(false);
			this->FileTable__->setVisible(true);
			this->ShowList__->setText(QChar(ElaIconType::Grid2));
		}
		else
		{
			qDebug() << "set to list";
			this->FileList__->setVisible(true);
			this->FileTable__->setVisible(false);
			this->ShowList__->setText(QChar(ElaIconType::Bars));
		}
		
	});
	

	eApp->syncMica(this);
	this->ThemeChanged(this->_themeMode);
	QObject::connect(eTheme, &ElaTheme::themeModeChanged, this, &RemoteFileSelector::ThemeChanged);
	QObject::connect(eApp, &ElaApplication::pIsEnableMicaChanged, this, [=]() {
		this->_isEnableMica = eApp->getIsEnableMica();
		update();
		});
	this->FileSystemModel__->Refresh();
}

RemoteFileSelector::~RemoteFileSelector()
{
	this->FileSystemThread__->quit();
	this->FileSystemThread__->wait();
	this->FileSystemModel__->deleteLater();
	//delete this->FileSystemModel__;
}

void RemoteFileSelector::paintEvent(QPaintEvent* event)
{
	QPainter painter(this);
	QDialog::paintEvent(event);
	if (!this->_isEnableMica)
	{
		painter.save();
		painter.setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing);
		painter.setPen(Qt::NoPen);
		painter.setBrush(ElaThemeColor(this->_themeMode, WindowBase));
		painter.drawRect(rect());
		painter.restore();
	}

	painter.save();
	painter.setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing);
	painter.setPen(Qt::NoPen);
	QColor color = (eTheme->getThemeMode() == ElaThemeType::Light) ? QColor(255, 255, 255, 250) : QColor(0x4B, 0x4B, 0x4B, 128);
	painter.setBrush(color);
	QRect pathBarRect = this->PathBar__->rect();
	pathBarRect.setHeight(pathBarRect.height()+10);
	pathBarRect.setWidth(pathBarRect.width() + 10);
	pathBarRect.moveTo(this->PathBar__->x()-10,this->PathBar__->y()-5);
	painter.drawRoundedRect(pathBarRect, 5, 5);
	painter.restore();

	if (this->FileList__->isVisible() || this->FileTable__->isVisible())
	{
		painter.save();
		painter.setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing);
		QColor pencolor = (eTheme->getThemeMode() == ElaThemeType::Light) ? QColor(210, 210, 210, 128) : QColor(70, 70, 70, 128);
		painter.setPen(QPen(pencolor));
		QColor color2 = (eTheme->getThemeMode() == ElaThemeType::Light) ? QColor(255, 255, 255, 250) : QColor(25, 25, 25, 128);
		painter.setBrush(color2);
		QRect FileRect;
		if (this->ShowListMode__)
			FileRect = this->FileTable__->rect();
		else
			FileRect = this->FileList__->rect();
		//FileRect.setHeight(FileRect.height() + 10);
		FileRect.setHeight(this->height());
		FileRect.setWidth(this->width());
		if(this->ShowListMode__)
			FileRect.moveTo(0, this->FileTable__->y() - 10);
		else
			FileRect.moveTo(0, this->FileList__->y() - 10);
		painter.drawRect(FileRect);
		painter.restore();
	}

}

ElaPushButton* RemoteFileSelector::CreateFixSizeButton(const QString& text)
{
	ElaPushButton* button = new ElaPushButton(this);
	button->setStyleSheet("background-color: transparent;");
	button->setText(text);
	QFont font = button->font();
	font.setFamily("ElaAwesome");
	font.setPointSize(12);
	button->setFont(font);
	button->setFixedSize(35, 35);
	button->setTransparent(true);
	return button;
}

void RemoteFileSelector::ThemeChanged(ElaThemeType::ThemeMode mode)
{
	this->_themeMode = mode;
	QPalette DataListBackgroundPalette;
	if (mode == ElaThemeType::Light)
	{
		QBrush brush_DataListBackground(QColor(0, 0, 0, 0));
		brush_DataListBackground.setStyle(Qt::SolidPattern);
		DataListBackgroundPalette.setBrush(QPalette::Active, QPalette::Base, brush_DataListBackground);
		DataListBackgroundPalette.setBrush(QPalette::Inactive, QPalette::Base, brush_DataListBackground);

		this->FileList__->setStyleSheet(R"(
			QListView {
			    background-color: transparent;
				border: none;
			}
			
			QListView::item {
			    background-color: transparent;
			    color: #000000;
			    border: transparent;
			}
			
			QListView::item:hover {
			    background-color: #f5f5f5;
			}
			
			QListView::item:selected {
				background-color: #f5f5f5;
				border: transparent;
			}
        )");

		this->FileTable__->setStyleSheet(R"(
			QTableView {
			    background-color: transparent;
				border: none;
			}
			
			QTableView::item {
			    background-color: transparent;
			    color: #000000;
			    border: transparent;
			}
			
			QTableView::item:hover {
			    background-color: #f5f5f5;
			}
			
			QTableView::item:selected {
				background-color: #f5f5f5;
				border: transparent;
			}
        )");

		this->FileTable__->horizontalHeader()->setStyleSheet(R"(
			QHeaderView {
			    background-color: transparent;
				border: none;
			}
			
			QHeaderView::section {
			    background-color: transparent;
			    color: #000000;
			    border: transparent;
			}
		)");
	}
	else
	{
		QBrush brush_DataListBackground(QColor(0, 0, 0, 0));
		brush_DataListBackground.setStyle(Qt::SolidPattern);
		DataListBackgroundPalette.setBrush(QPalette::Active, QPalette::Base, brush_DataListBackground);
		DataListBackgroundPalette.setBrush(QPalette::Inactive, QPalette::Base, brush_DataListBackground);

		this->FileList__->setStyleSheet(R"(
			QListView {
			    background-color: transparent;
				border: none;
			}

			QListView::item {
			    background-color: transparent;
			    color: #ffffff;
			    border: transparent;
			}
			
			QListView::item:hover {
			    background-color: #4e4e4e;
			}
			
			QListView::item:selected {
			    background-color: #4e4e4e;
				border: transparent;
			}
        )");

		this->FileTable__->setStyleSheet(R"(
			QTableView {
			    background-color: transparent;
				border: none;
			}

			QTableView::item {
			    background-color: transparent;
			    color: #ffffff;
			    border: transparent;
			}
			
			QTableView::item:hover {
			    background-color: #4e4e4e;
			}
			
			QTableView::item:selected {
			    background-color: #4e4e4e;
				border: transparent;
			}
        )");

		this->FileTable__->horizontalHeader()->setStyleSheet(R"(
			QHeaderView {
			    background-color: transparent;
			}
			
			QHeaderView::section {
			    background-color: transparent;
			    color: #ffffff;
			}
		)");
	}
	this->FileList__->setPalette(DataListBackgroundPalette);
	qDebug() << "ThemeChanged";
}

void RemoteFileSelector::ShowConnectedUI(bool show)
{
	if (show)
	{
		this->DisconnectedWidget__->setVisible(false);
		this->PathBar__->setBreadcrumbList(QStringList());
		if (this->ShowListMode__)
		{
			this->FileList__->setVisible(false);
			this->FileTable__->setVisible(true);
			this->ShowList__->setText(QChar(ElaIconType::Grid2));
		}
		else
		{
			this->FileList__->setVisible(true);
			this->FileTable__->setVisible(false);
			this->ShowList__->setText(QChar(ElaIconType::Bars));
		}

	}
	else
	{
		this->DisconnectedWidget__->setVisible(true);
		this->PathBar__->setBreadcrumbList(QStringList({tr("Connection Lost")}));
		this->FileList__->setVisible(false);
		this->FileTable__->setVisible(false);
	}
}

void RemoteFileSelector::EnableButton(bool enable)
{
	if (!enable)
	{
		this->ForwardButton__->setEnabled(false);
		this->BackwardButton__->setEnabled(false);
		this->UpButton__->setEnabled(false);
		this->RefreshButton__->setEnabled(false);
		this->ShowList__->setEnabled(false);
	}
	else
	{
		this->RefreshButton__->setEnabled(true);
		this->ShowList__->setEnabled(this->FileSystemModel__->rowCount()!=0);
		this->UpButton__->setEnabled(!this->FileSystemModel__->isCurrentPathRoot());
		this->ForwardButton__->setEnabled(this->FileSystemModel__->CanForward());
		this->BackwardButton__->setEnabled(this->FileSystemModel__->CanBackward());
	}
	this->update(0, 0, this->width(), this->height());
}

void RemoteFileSelector::PathChanged(const QString& path)
{
	//split path
	QStringList pathList = path.split("/");
	//remove / at the end
	if (pathList.size() > 0 && pathList.last().isEmpty())
		pathList.removeLast();
	this->PathBar__->setBreadcrumbList(pathList);

	this->UpButton__->setEnabled(!this->FileSystemModel__->isCurrentPathRoot());
	this->ForwardButton__->setEnabled(this->FileSystemModel__->CanForward());
	this->BackwardButton__->setEnabled(this->FileSystemModel__->CanBackward());
}

void RemoteFileSelector::ForwardButtonClicked()
{
	this->FileSystemModel__->Forward();
}

void RemoteFileSelector::BackwardButtonClicked()
{
	this->FileSystemModel__->Backward();
}

void RemoteFileSelector::UpButtonClicked()
{
	this->FileSystemModel__->cdup();
}

void RemoteFileSelector::RefreshButtonClicked()
{
	this->FileSystemModel__->Refresh();
}

void RemoteFileSelector::FileDoubleClicked(const QModelIndex& index)
{
	qDebug() << "FileDoubleClicked";
	qDebug() << index.data().toString();
	sftp_attributes_struct_ex* x = (sftp_attributes_struct_ex*)index.internalPointer();
	if (x->type == SSH_FILEXFER_TYPE_DIRECTORY)	
	{
		std::string path = this->FileSystemModel__->path().toStdString() + "/" + x->name;
		this->FileSystemModel__->cd(path);
	}
	else
	{
		emit this->FileSelected(this->FileSystemModel__->path() + "/" + x->name.c_str());
		qApp->processEvents();
		this->done(QDialog::Accepted);
	}
}

RemoteFileSelectorDisConnectedWidget::RemoteFileSelectorDisConnectedWidget(QWidget* parent)
	:QWidget(parent)
{
	this->Notation__ = new ElaText(this);
	this->Title__ = new ElaText(this);
	this->Icon__ = new QLabel(this);

	this->Notation__->setText(tr("Robot is not connected, please check your connection or remote configuration."));
	this->Title__->setText(tr("Robot is not connected"));
	this->Notation__->setAlignment(Qt::AlignVCenter | Qt::AlignLeft);
	this->Title__->setAlignment(Qt::AlignCenter);
	this->Notation__->setWordWrap(true);
	this->Title__->setWordWrap(true);

	this->Notation__->setTextStyle(ElaTextType::Body);
	this->Title__->setTextStyle(ElaTextType::Title);
	this->Notation__->setMinimumWidth(350);
	this->Title__->setMinimumWidth(350);

	this->Icon__->setPixmap(QPixmap(":/UI/Image/error_state_icon.png"));
	this->Icon__->setScaledContents(true);
	this->Icon__->setFixedSize(96, 96);
	this->Icon__->setAlignment(Qt::AlignHCenter);
	
	QHBoxLayout* IconLay = new QHBoxLayout();
	IconLay->addSpacerItem(new QSpacerItem(20, 20, QSizePolicy::Expanding, QSizePolicy::Minimum));
	IconLay->addWidget(this->Icon__);
	IconLay->addSpacerItem(new QSpacerItem(20, 20, QSizePolicy::Expanding, QSizePolicy::Minimum));

	QVBoxLayout* vlay = new QVBoxLayout();
	vlay->addSpacerItem(new QSpacerItem(20, 20, QSizePolicy::Minimum, QSizePolicy::Expanding));
	vlay->addLayout(IconLay);
	vlay->addWidget(this->Title__);
	vlay->addWidget(this->Notation__);
	vlay->addSpacerItem(new QSpacerItem(20, 20, QSizePolicy::Minimum, QSizePolicy::Expanding));

	QHBoxLayout* hlay = new QHBoxLayout();
	hlay->addSpacerItem(new QSpacerItem(20, 20, QSizePolicy::Expanding, QSizePolicy::Minimum));
	hlay->addLayout(vlay);
	hlay->addSpacerItem(new QSpacerItem(20, 20, QSizePolicy::Expanding, QSizePolicy::Minimum));

	this->setLayout(hlay);

	this->ThemeChanged(eTheme->getThemeMode());
	QObject::connect(eTheme, &ElaTheme::themeModeChanged, this, &RemoteFileSelectorDisConnectedWidget::ThemeChanged);
}

RemoteFileSelectorDisConnectedWidget::~RemoteFileSelectorDisConnectedWidget()
{
}

void RemoteFileSelectorDisConnectedWidget::ThemeChanged(ElaThemeType::ThemeMode mode)
{
	QPalette LabelPalette;
	QPalette MainLabelPalette;
	if (mode == ElaThemeType::ThemeMode::Dark)
	{
		QBrush brush_subLabel(QColor(206, 206, 206, 255));
		brush_subLabel.setStyle(Qt::SolidPattern);
		LabelPalette.setBrush(QPalette::Active, QPalette::WindowText, brush_subLabel);
		LabelPalette.setBrush(QPalette::Inactive, QPalette::WindowText, brush_subLabel);

		QBrush brush_mainLabel(QColor(255, 255, 255, 255));
		brush_mainLabel.setStyle(Qt::SolidPattern);
		MainLabelPalette.setBrush(QPalette::Active, QPalette::WindowText, brush_mainLabel);
		MainLabelPalette.setBrush(QPalette::Inactive, QPalette::WindowText, brush_mainLabel);
	}
	else
	{
		QBrush brush_subLabel(QColor(55, 55, 55, 255));
		brush_subLabel.setStyle(Qt::SolidPattern);
		LabelPalette.setBrush(QPalette::Active, QPalette::WindowText, brush_subLabel);
		LabelPalette.setBrush(QPalette::Inactive, QPalette::WindowText, brush_subLabel);

		QBrush brush_mainLabel(QColor(0, 0, 0, 255));
		brush_mainLabel.setStyle(Qt::SolidPattern);
		MainLabelPalette.setBrush(QPalette::Active, QPalette::WindowText, brush_mainLabel);
		MainLabelPalette.setBrush(QPalette::Inactive, QPalette::WindowText, brush_mainLabel);
	}
	this->Title__->setPalette(MainLabelPalette);
	this->Notation__->setPalette(LabelPalette);
}
