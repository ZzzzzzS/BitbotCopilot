#include "RTDDeviceInfoTableModel.h"
#include <QDebug>
#include <algorithm>

RTDDeviceInfoRTableModel::RTDDeviceInfoRTableModel(QObject* parent)
	:QAbstractTableModel(parent)
{
	this->clear();
}

RTDDeviceInfoRTableModel::~RTDDeviceInfoRTableModel()
{
	this->clear();
}

bool RTDDeviceInfoRTableModel::setHeaders(const QVector<QString>& DeviceList, const QVector<QString>& DeviceSensors)
{
	if (DeviceList.empty() || DeviceSensors.empty())
	{
		this->clear();
		return false;
	}
	this->_DeviceName = DeviceList.toList();
	this->_header = DeviceSensors.toList();
	this->IncludeDeviceName = true;
	this->_dataList.resize(DeviceList.size());
	for (size_t i = 0; i < this->_dataList.size(); i++)
	{
		this->_dataList[i].resize(DeviceSensors.size());
	}

	emit this->headerDataChanged(Qt::Horizontal, 0, std::min(static_cast<long long>(this->_header.size() - 1), static_cast<long long>(0)));
	emit this->headerDataChanged(Qt::Vertical, 0, std::min(static_cast<long long>(this->_DeviceName.size() - 1), static_cast<long long>(0)));
	return true;
}

bool RTDDeviceInfoRTableModel::setHeaders(const QVector<QString>& UserDataHeader)
{
	if (UserDataHeader.empty())
	{
		this->clear();
		return false;
	}
	this->_DeviceName.clear();
	this->_header = UserDataHeader.toList();
	this->_dataList.resize(1);
	this->_dataList[0].resize(UserDataHeader.size());
	emit this->headerDataChanged(Qt::Horizontal, 0, std::min(static_cast<long long>(this->_header.size() - 1), static_cast<long long>(0)));
	emit this->headerDataChanged(Qt::Vertical, 0, 0);
	return true;
}

void RTDDeviceInfoRTableModel::UpdateDeviceInfo(QVariantList data)
{
	if (data.empty())	return;
	if (this->_header.empty() || this->_dataList.empty()) return;

	if (data.size() != this->_header.size() * (this->IncludeDeviceName?this->_DeviceName.size() : 1))
	{
		throw std::runtime_error("data and headers are not equal!");
	}

	size_t RowNumber = this->_dataList.size();
	size_t RowLength = this->_dataList[0].size();
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
				d.type() == QMetaType::UShort ||
				d.type() == QMetaType::LongLong ||
				d.type() == QMetaType::ULongLong ||
				d.type() == QMetaType::Long ||
				d.type() == QMetaType::ULong
				)
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
				item = QString("0");
			}
			this->_dataList[i][j] = item;
		}
	}
	emit this->dataChanged(this->index(0, 0), this->index(RowNumber-1, RowLength-1), { Qt::DisplayRole });
}

void RTDDeviceInfoRTableModel::clear()
{
	this->beginResetModel();
	this->IncludeDeviceName = false;
	this->_header.clear();
	this->_DeviceName.clear();
	this->_dataList.clear();
	this->endResetModel();
}  

int RTDDeviceInfoRTableModel::rowCount(const QModelIndex& parent) const
{
	//qDebug() << "row count call";
	if (this->IncludeDeviceName)
		return this->_DeviceName.size();
	else
		return 1;
}

int RTDDeviceInfoRTableModel::columnCount(const QModelIndex& parent) const
{
	//qDebug() << "col count call";
	return this->_header.size();
}

QVariant RTDDeviceInfoRTableModel::data(const QModelIndex& index, int role) const
{
	//qDebug() << "data call" << index.row() << "," << index.column() << "," << role;
	if (role == Qt::DisplayRole)
	{
		return this->_dataList[index.row()][index.column()];
	}
	return QVariant();
}

QVariant RTDDeviceInfoRTableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
	//qDebug() << "header call";
	if (orientation == Qt::Horizontal && role == Qt::DisplayRole)
	{
		return this->_header[section];
	}
	else if (orientation == Qt::Vertical && role == Qt::DisplayRole)
	{
		if (this->IncludeDeviceName)
			return this->_DeviceName[section];
		else
			return QVariant("User Info");
	}
	else
	{
		return QAbstractTableModel::headerData(section, orientation, role);
	}
}

