#include "RTDDeviceInfo.h"
#include <QDebug>

RTDDeviceInfo::RTDDeviceInfo(QWidget* parent)
	: MetaRTDView(MetaRTDView::RTDViewType::EXTEND_WINDOW, parent)
	, ui(new Ui::RTDDeviceInfoClass())
{
	ui->setupUi(this);
	this->Model__ = new RTDDeviceInfoRTableModel(this);
	this->ui->tableView->setModel(this->Model__);
	this->ui->tableView->setAlternatingRowColors(true);
	this->ui->tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeMode::Stretch);
}

RTDDeviceInfo::~RTDDeviceInfo()
{
	delete ui;
}

bool RTDDeviceInfo::setHeaders(const QString& DeviceType, const QVector<QString>& DeviceList, const QVector<QString>& DeviceSensors)
{
	//if (this->isDeviceInfo__ == true || this->isUserExtraData__ == true)
	//	return false;


	this->ui->label_DeviceName->setText(DeviceType);
	this->DeviceTypeName__ = DeviceType;
	this->isDeviceInfo__ = true;
	this->VerticalHeader = DeviceSensors;
	this->HorizentalHeader = DeviceList;
	this->RowLength = DeviceList.size();
	this->RowNumber =  DeviceSensors.size();
	this->Model__->setHeaders(DeviceList, DeviceSensors);

	return true;
}

bool RTDDeviceInfo::setHeaders(const QVector<QString>& UserDataHeader)
{
	//if (this->isDeviceInfo__ == true || this->isUserExtraData__ == true)
	//	return false;

	this->ui->label_DeviceName->setText(tr("User Info"));
	this->HorizentalHeader = UserDataHeader;
	this->RowLength = UserDataHeader.size();
	this->RowNumber = 1;
	
	this->Model__->setHeaders(UserDataHeader);

	return true;
}

QString RTDDeviceInfo::DeviceTypeName()
{
	return this->DeviceTypeName__;
}

void RTDDeviceInfo::UpdateDeviceInfo(QVariantList data)
{
	this->Model__->UpdateDeviceInfo(data);
}

size_t RTDDeviceInfo::getHeaderSize()
{
	return this->RowLength * this->RowNumber;
}

void RTDDeviceInfo::ResetUI()
{
	this->Model__->clear();
}

