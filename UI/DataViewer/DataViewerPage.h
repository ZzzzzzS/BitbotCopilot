#ifndef DATAVIEWERPAGE_H
#define DATAVIEWERPAGE_H

#include <QWidget>
#include "qcustomplot.h"
#include "QMap"
#include "QString"
#include "QStandardItemModel"
#include "QStandardItem"
#include "Utils/DataReadWriter/csv_io.hpp"
#include "tuple"
#include "QAction"
#include "ElaMenu.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class DataViewerPage;
}
QT_END_NAMESPACE

enum Theme_e
{
    Light=0,
    Dark=1
};


struct DataGroup_t
{
    QVector<double> t;
    QStandardItem* ParentModel = nullptr;
    QMap<QString, QVector<double>> Curves;
    QMap<QString, QStandardItem*> LinkedModelItem;
    QMap<QString, size_t> ColorIndex;
    QMap<QString, QCPGraph*> VisiableCurve;
};

class DataViewerPage : public QWidget
{
    Q_OBJECT

public:
    DataViewerPage(QWidget *parent = nullptr);
    ~DataViewerPage();
    void SetTheme(Theme_e theme);

protected:
    void dragEnterEvent(QDragEnterEvent* event);
    void dropEvent(QDropEvent* event);
    bool event(QEvent* event);

private:
    void SetCurveVisiable(const QString& CurveGroup, const QString& CurveName, bool Visiable,bool replot=true);
    bool AddCurveGroup(const QString& GroupName, zzs::CSVReader::Ptr csv_handle);
    bool RemoveCurveGroup(const QString& GroupName);
    void InitColorList();
    void ModelItemClickedSlot(const QModelIndex& index);
    void RefillAvailableColor();

    void UpdateCurveTheme(const QString& CurveGroup, const QString& CurveName, Theme_e theme);
    void SetupCurve(const QString& CurveGroup, const QString& CurveName);
    void RemoveCurve(const QString& CurveGroup, const QString& CurveName);
    
    void PlotHandleMouseWheelSlot(QWheelEvent* event);
    void RangeChangedSlot(const QCPRange& newRange);
    std::tuple<double,double> ComputeDeltaDirection(double low, double high, double point, double vel,bool reverse);

    void LoadLocalFileSlot();
    void LoadRobotFileSlot();
    void RemoveButtonClickedSlot();
    void ListWidgetRightClickedSlot(const QPoint& pos);

    void initReloadButton();
    void initListWidgetRightClickedMenu();
    void InitSquareZoom();;
    void SavePlotSlot();
    void InitShowDataPoint();
    
    void SearchClickedSlot(QString suggestText, QVariantMap suggestData);
signals:
    void FileLoaded(bool);

private:
    Ui::DataViewerPage *ui;

    QMap<QString, DataGroup_t> AggregatedDataGroup__;
    QStandardItemModel* AggregatedDataModel__;
    QCustomPlot* PlotHandle;

    using ColorPair = std::pair<QColor, QColor>;
    QMap<size_t, ColorPair> AvailableColorPair;
    QMap<size_t, ColorPair> UsedColorPair;
    Theme_e WindowTheme;

    QMap<QString, QAction*> SearchActions;
    ElaMenu* SeachMenu;

    bool SquareZoomMode__=false;
    bool isShowDataPoint__;
};
#endif // DATAVIEWERPAGE_H
