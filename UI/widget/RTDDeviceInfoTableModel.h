#pragma once
#include <QAbstractTableModel>

class RTDDeviceInfoRTableModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    explicit RTDDeviceInfoRTableModel(QObject* parent = nullptr);
    ~RTDDeviceInfoRTableModel();

    bool setHeaders(const QVector<QString>& DeviceList, const QVector<QString>& DeviceSensors);
    bool setHeaders(const QVector<QString>& UserDataHeader);
    void UpdateDeviceInfo(QVariantList data);
    void clear();

public:
    int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    int columnCount(const QModelIndex& parent = QModelIndex()) const override;

    QVariant data(const QModelIndex& index, int role) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

private:
    QStringList _header;
    QVector<QVector<QString>> _dataList;
    QStringList _DeviceName;
    bool IncludeDeviceName = false;
};