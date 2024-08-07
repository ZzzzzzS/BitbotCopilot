#include "RTDDeviceInfo.h"

RTDDeviceInfo::RTDDeviceInfo(QWidget *parent)
	: MetaRTDView(MetaRTDView::RTDViewType::EXTEND_WINDOW,parent)
	, ui(new Ui::RTDDeviceInfoClass())
{
	ui->setupUi(this);
	this->ui->CentralWidget->setRowCount(1);
	this->ui->CentralWidget->setColumnCount(1);
	this->ThemeChanged(eTheme->getThemeMode());
	QObject::connect(eTheme, &ElaTheme::themeModeChanged, this, &RTDDeviceInfo::ThemeChanged);
}

RTDDeviceInfo::~RTDDeviceInfo()
{
	delete ui;
}

bool RTDDeviceInfo::setHeaders(const QString& DeviceType, const QVector<QString>& DeviceList, const QVector<QString>& DeviceSensors)
{
	if (this->isDeviceInfo__ == true || this->isUserExtraData__ == true)
		return false;
	
	this->isDeviceInfo__ = true;
	this->VerticalHeader = DeviceList;
	this->HorizentalHeader = DeviceSensors;
	this->RowLength = DeviceSensors.size();
	this->RowNumber = DeviceList.size();
	this->ui->CentralWidget->setRowCount(this->RowNumber);
	this->ui->CentralWidget->setColumnCount(this->RowLength);
	
	this->ui->CentralWidget->setVerticalHeaderLabels(this->VerticalHeader.toList());
	this->ui->CentralWidget->setHorizontalHeaderLabels(this->HorizentalHeader.toList());
	this->TableItem.clear();
	this->TableItem.resize(this->RowLength * this->RowNumber);
	for (size_t i = 0; i < RowNumber; i++)
	{
		for (size_t j = 0; j < RowLength; j++)
		{
			this->TableItem[i * RowLength + j] = new QTableWidgetItem("N/A");
			this->ui->CentralWidget->setItem(i, j, this->TableItem[i * RowLength + j]);
		}
	}

	return true;
}

bool RTDDeviceInfo::setHeaders(const QVector<QString>& UserDataHeader)
{
	if (this->isDeviceInfo__ == true || this->isUserExtraData__ == true)
		return false;

	this->HorizentalHeader = UserDataHeader;
	this->RowLength = UserDataHeader.size();
	this->RowNumber = 1;
	this->ui->CentralWidget->setRowCount(this->RowNumber);
	this->ui->CentralWidget->setColumnCount(this->RowLength);
	this->ui->CentralWidget->setHorizontalHeaderLabels(this->HorizentalHeader.toList());

	for (size_t i = 0; i < RowNumber; i++)
	{
		for (size_t j = 0; j < RowLength; j++)
		{
			this->TableItem[i * RowLength + j] = new QTableWidgetItem("N/A");
			this->ui->CentralWidget->setItem(i, j, this->TableItem[i * RowLength + j]);
		}
	}

	return true;
}

void RTDDeviceInfo::UpdateDeviceInfo(QVariantList data)
{
	if (data.size() != this->RowLength * this->RowNumber)
	{
		throw std::runtime_error("data and headers are not equal!");
	}

	if (data.size() < this->RowLength * this->RowNumber)
	{
		throw std::runtime_error("data and headers are not equal!");
	}

	for (size_t i = 0; i < RowNumber; i++)
	{
		for (size_t j = 0; j < RowLength; j++)
		{
			QVariant d = data[i * RowLength + j];
			QString item;
			if (d.type() == QMetaType::QString)
			{
				item = d.toString();
			}
			else if (d.type() == QMetaType::Int ||
				d.type() == QMetaType::UInt ||
				d.type() == QMetaType::Short ||
				d.type() == QMetaType::UShort)
			{
				item = QString::number(d.toInt());
			}
			else if (d.type() == QMetaType::Float ||
				d.type() == QMetaType::Double)
			{
				item = QString::number(d.toFloat());
			}
			else if (d.type() == QMetaType::Bool)
			{
				item = d.toBool() ? "True" : "False";
			}
			else
			{
				item = QString("N/A");
			}
			this->TableItem[i * RowLength + j]->setText(item);
		}
	}
}

void RTDDeviceInfo::ThemeChanged(ElaThemeType::ThemeMode mode)
{
	if (mode == ElaThemeType::Light)
	{

	}
	else
	{

	}
}
