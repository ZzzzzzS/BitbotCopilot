#include "DataViewerPage.h"
#include "./ui_DataViewerPage.h"
#include "QVector"
#include "qcustomplot.h"
#include "QPen"
#include "QFont"
#include "QTimer"
#include "ElaMenu.h"
#include "QAction"
#include "ElaApplication.h"
#include "ElaTheme.h"

DataViewerPage::DataViewerPage(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::DataViewerPage)
    , AggregatedDataModel__(nullptr)
    , WindowTheme(Theme_e::Light)
{
    ui->setupUi(this);
    this->setWindowTitle(tr("New Data Viewer"));
    this->setAttribute(Qt::WA_TranslucentBackground, true);

    this->setAcceptDrops(true);

    QObject::connect(this->ui->DataListWidget, &QTreeView::clicked, this, &DataViewerPage::ModelItemClickedSlot);
    this->AggregatedDataModel__ = new QStandardItemModel(this);
    this->ui->DataListWidget->setModel(this->AggregatedDataModel__);
    this->AggregatedDataModel__->setHorizontalHeaderLabels(QStringList(QString("    "+tr("Experiment Data"))));

    this->PlotHandle = this->ui->DataPlotWidget;
    this->PlotHandle->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectPlottables);
    this->PlotHandle->setOpenGl(true);
    QObject::connect(this->PlotHandle, &QCustomPlot::mouseWheel, this, &DataViewerPage::PlotHandleMouseWheelSlot);

    this->InitColorList();
    
    this->ui->WelcomeWidget->setAttribute(Qt::WA_TranslucentBackground, true);
    //this->ui->DataPlotWidget->setBackground()
    this->ui->DataPlotWidget->setBackground(QBrush(QColor(0, 0, 0, 0)));
    //this->ui->DataListWidget->setAutoFillBackground(false);
    this->ui->DataWidget->hide();
    //this->ui->WelcomeWidget->hide();
    QObject::connect(this->ui->pushButton_loadlocal, &ElaPushButton::clicked, this, &DataViewerPage::LoadLocalFileSlot);
    QObject::connect(this->ui->pushButton_loadrobot, &ElaPushButton::clicked, this, &DataViewerPage::LoadRobotFileSlot);

    QObject::connect(this->ui->pushButton_help, &ElaPushButton::clicked, this, &DataViewerPage::HelpButtonClickedSlot);
    QObject::connect(this->ui->pushButton_clean, &ElaPushButton::clicked, this, &DataViewerPage::RemoveButtonClickedSlot);

    
    this->SetTheme((eTheme->getThemeMode() == ElaThemeType::ThemeMode::Light) ? Theme_e::Light : Theme_e::Dark);
    QObject::connect(eTheme, &ElaTheme::themeModeChanged, this, [this](ElaThemeType::ThemeMode mode) {
        if (mode == ElaThemeType::ThemeMode::Light)
        {
            this->SetTheme(Theme_e::Light);
        }
        else
        {
            this->SetTheme(Theme_e::Dark);
        }
    });

    this->initReloadButton();
}

DataViewerPage::~DataViewerPage()
{
    delete ui;
}

void DataViewerPage::SetTheme(Theme_e theme)
{
    QFont axis_font("Segoe UI", 10);
    this->PlotHandle->xAxis->setTickLabelFont(axis_font);
    this->PlotHandle->yAxis->setTickLabelFont(axis_font);

    this->PlotHandle->xAxis2->setTickLengthIn(0);
    this->PlotHandle->xAxis->setTickLengthIn(0);
    this->PlotHandle->yAxis2->setTickLengthIn(0);
    this->PlotHandle->yAxis->setTickLengthIn(0);

    this->PlotHandle->xAxis2->setTickLengthOut(0);
    this->PlotHandle->xAxis->setTickLengthOut(0);
    this->PlotHandle->yAxis2->setTickLengthOut(0);
    this->PlotHandle->yAxis->setTickLengthOut(0);


    this->PlotHandle->xAxis2->setVisible(true);
    this->PlotHandle->xAxis2->setTickLabels(false);
    this->PlotHandle->yAxis2->setVisible(true);
    this->PlotHandle->yAxis2->setTickLabels(false);
    this->PlotHandle->xAxis2->setSubTicks(false);
    this->PlotHandle->xAxis->setSubTicks(false);
    this->PlotHandle->yAxis2->setSubTicks(false);
    this->PlotHandle->yAxis->setSubTicks(false);

    this->PlotHandle->legend->setFont(axis_font);
    this->PlotHandle->legend->setIconSize(40, 30);
    this->PlotHandle->legend->setBorderPen(Qt::NoPen);

    QColor ButtonLightDefaultColor(0, 103, 192);
    QColor ButtonLightHoverColor(26, 118, 198);
    QColor ButtonLightTextColor(255, 255, 255);

    QColor ButtonDarkDefaultColor(76, 194, 255);
    QColor ButtonDarkHoverColor(73, 179, 234);
    QColor ButtonDarkTextColor(0, 0, 0);

    //this->ui->pushButton_loadlocal->setLightDefaultColor(ButtonLightDefaultColor);
    //this->ui->pushButton_loadlocal->setLightHoverColor(ButtonLightHoverColor);
    //this->ui->pushButton_loadlocal->setLightTextColor(ButtonLightTextColor);
    //this->ui->pushButton_loadlocal->setLightPressColor(ButtonLightHoverColor);
    //this->ui->pushButton_loadlocal->setDarkDefaultColor(ButtonDarkDefaultColor);
    //this->ui->pushButton_loadlocal->setDarkHoverColor(ButtonDarkHoverColor);
    //this->ui->pushButton_loadlocal->setDarkTextColor(ButtonDarkTextColor);
    //this->ui->pushButton_loadlocal->setDarkPressColor(ButtonDarkHoverColor);
    //
    //this->ui->pushButton_loadrobot->setLightDefaultColor(ButtonLightDefaultColor);
    //this->ui->pushButton_loadrobot->setLightHoverColor(ButtonLightHoverColor);
    //this->ui->pushButton_loadrobot->setLightTextColor(ButtonLightTextColor);
    //this->ui->pushButton_loadrobot->setLightPressColor(ButtonLightHoverColor);
    //this->ui->pushButton_loadrobot->setDarkDefaultColor(ButtonDarkDefaultColor);
    //this->ui->pushButton_loadrobot->setDarkHoverColor(ButtonDarkHoverColor);
    //this->ui->pushButton_loadrobot->setDarkTextColor(ButtonDarkTextColor);
    //this->ui->pushButton_loadrobot->setDarkPressColor(ButtonDarkHoverColor);

    //this->ui->PushButton_load->setLightDefaultColor(ButtonLightDefaultColor);
    //this->ui->PushButton_load->setLightHoverColor(ButtonLightHoverColor);
    //this->ui->PushButton_load->setLightTextColor(ButtonLightTextColor);
    //this->ui->PushButton_load->setDarkDefaultColor(ButtonDarkDefaultColor);
    //this->ui->PushButton_load->setDarkHoverColor(ButtonDarkHoverColor);
    //this->ui->PushButton_load->setDarkTextColor(ButtonDarkTextColor);


    QPalette LabelPalette;
    if (theme == Theme_e::Light)
    {
        QColor GridLightColor(226, 226, 226);
        QPen axis_pen_X = this->PlotHandle->xAxis->basePen();
        //axis_pen_X.setWidthF(1.5);
        axis_pen_X.setColor(GridLightColor);
        this->PlotHandle->xAxis->setBasePen(axis_pen_X);
        this->PlotHandle->xAxis2->setBasePen(axis_pen_X);

        QPen axis_pen_Y = this->PlotHandle->yAxis->basePen();
        //axis_pen_Y.setWidthF(1.5);
        axis_pen_Y.setColor(GridLightColor);
        this->PlotHandle->yAxis->setBasePen(axis_pen_Y);
        this->PlotHandle->yAxis2->setBasePen(axis_pen_Y);

        QPen gridx = this->PlotHandle->xAxis->grid()->pen();
        gridx.setStyle(Qt::SolidLine);
        gridx.setColor(GridLightColor);
        this->PlotHandle->xAxis->grid()->setPen(gridx);

        QPen gridy = this->PlotHandle->yAxis->grid()->pen();
        gridy.setStyle(Qt::SolidLine);
        gridy.setColor(GridLightColor);
        this->PlotHandle->yAxis->grid()->setPen(gridy);

        QColor TickLabelColorLight(0, 0, 0);
        this->PlotHandle->xAxis->setTickLabelColor(TickLabelColorLight);
        this->PlotHandle->yAxis->setTickLabelColor(TickLabelColorLight);
        this->PlotHandle->legend->setTextColor(TickLabelColorLight);
        this->PlotHandle->legend->setBrush(QColor(226, 226, 226, 128));

        QBrush brush_versionLabel(QColor(55, 55, 55, 255));
        brush_versionLabel.setStyle(Qt::SolidPattern);
        LabelPalette.setBrush(QPalette::Active, QPalette::WindowText, brush_versionLabel);
        LabelPalette.setBrush(QPalette::Inactive, QPalette::WindowText, brush_versionLabel);
    }
    else
    {
        QColor GridDarkColor(74,74,74);
        QPen axis_pen_X = this->PlotHandle->xAxis->basePen();
        //axis_pen_X.setWidthF(1.5);
        axis_pen_X.setColor(GridDarkColor);
        this->PlotHandle->xAxis->setBasePen(axis_pen_X);
        this->PlotHandle->xAxis2->setBasePen(axis_pen_X);

        QPen axis_pen_Y = this->PlotHandle->yAxis->basePen();
        //axis_pen_Y.setWidthF(1.5);
        axis_pen_Y.setColor(GridDarkColor);
        this->PlotHandle->yAxis->setBasePen(axis_pen_Y);
        this->PlotHandle->yAxis2->setBasePen(axis_pen_Y);

        QPen gridx = this->PlotHandle->xAxis->grid()->pen();
        gridx.setStyle(Qt::SolidLine);
        gridx.setColor(GridDarkColor);
        this->PlotHandle->xAxis->grid()->setPen(gridx);

        QPen gridy = this->PlotHandle->yAxis->grid()->pen();
        gridy.setStyle(Qt::SolidLine);
        gridy.setColor(GridDarkColor);
        this->PlotHandle->yAxis->grid()->setPen(gridy);

        QColor TickLabelColorDark(226, 226, 226);
        this->PlotHandle->xAxis->setTickLabelColor(TickLabelColorDark);
        this->PlotHandle->yAxis->setTickLabelColor(TickLabelColorDark);
        this->PlotHandle->legend->setTextColor(TickLabelColorDark);
        this->PlotHandle->legend->setBrush(QColor(74, 74, 74, 128));

        QBrush brush_versionLabel(QColor(206, 206, 206, 255));
        brush_versionLabel.setStyle(Qt::SolidPattern);
        LabelPalette.setBrush(QPalette::Active, QPalette::WindowText, brush_versionLabel);
        LabelPalette.setBrush(QPalette::Inactive, QPalette::WindowText, brush_versionLabel);
    }
    this->ui->label_subtitle->setPalette(LabelPalette);

    for (auto GroupItem = this->AggregatedDataGroup__.begin(); GroupItem != this->AggregatedDataGroup__.end(); GroupItem++)
    {
        for (auto CurveItem = GroupItem.value().VisiableCurve.begin(); CurveItem != GroupItem.value().VisiableCurve.end(); CurveItem++)
        {
            this->UpdateCurveTheme(GroupItem.key(), CurveItem.key(), theme);
        }
    }
    
    QTimer::singleShot(0, [this]() {
        this->PlotHandle->replot();
     });
}

void DataViewerPage::dragEnterEvent(QDragEnterEvent* event)
{
    qDebug() << "drag enter";
    if (event->mimeData()->hasUrls() && event->mimeData()->urls().size()==1)
    {
        qDebug() << event->mimeData()->urls();
        event->accept();
    }
}

void DataViewerPage::dropEvent(QDropEvent* event)
{
    qDebug() << "DropEvent";
    auto urls = event->mimeData()->urls();
    auto ImageUrl = urls[0];
    qDebug() << ImageUrl;
    QString localpath = ImageUrl.toLocalFile();
    if (localpath.isEmpty() || !localpath.endsWith(".csv", Qt::CaseInsensitive))
    {
        QMessageBox::warning(this, tr("Unsupported File Type"), tr("Unsupported path or file type, only .csv file type is supported!"), QMessageBox::Ok);
        return;
    }
    else
    {
        zzs::CSVReader::Ptr r = zzs::CSVReader::Create(localpath.toStdString(), true);
        auto groupkey = localpath.split("/").back();
        if (!r->open() || !this->AddCurveGroup(groupkey, r))
        {
            QMessageBox::warning(this, tr("Failed to Open File"), tr("Failed to open this file, try again later."), QMessageBox::Ok);
            return;
        }
        this->setWindowTitle(groupkey);
        this->ui->WelcomeWidget->hide();
        this->ui->DataWidget->show();
        this->ui->DataListWidget->expand(this->AggregatedDataGroup__[groupkey].ParentModel->index());
        emit this->FileLoaded(true);
    }
}

void DataViewerPage::SetCurveVisiable(const QString& CurveGroup, const QString& CurveName, bool Visiable, bool replot)
{
    if (!this->AggregatedDataGroup__.contains(CurveGroup) || !this->AggregatedDataGroup__[CurveGroup].Curves.contains(CurveName))
        return;

    if (Visiable)
    {
        if (this->AggregatedDataGroup__[CurveGroup].VisiableCurve.contains(CurveName))
        {
            return;
        }
        this->RefillAvailableColor();
        this->SetupCurve(CurveGroup, CurveName);
        this->UpdateCurveTheme(CurveGroup, CurveName, this->WindowTheme);
    }
    else
    {
        if (!this->AggregatedDataGroup__[CurveGroup].VisiableCurve.contains(CurveName))
        {
            return;
        }
        this->RemoveCurve(CurveGroup, CurveName);
    }
    if(replot)
        this->PlotHandle->replot();
}

bool DataViewerPage::AddCurveGroup(const QString& GroupName, zzs::CSVReader::Ptr csv_handle)
{
    if (GroupName.isEmpty()) return false;
    if (csv_handle->RowSize() == 0 || csv_handle->ColSize() == 0) return false;
    if (this->AggregatedDataGroup__.contains(GroupName)) return false;

    DataGroup_t ng;
    QStandardItem* ParentModel = new QStandardItem(GroupName);
    ParentModel->setCheckable(true);
    ParentModel->setCheckState(Qt::CheckState::Unchecked);
    ParentModel->setEditable(false);
    ng.ParentModel = ParentModel;

    auto headers = csv_handle->getHeader();
    for (auto& [key, value] : headers)
    {
        auto curve = csv_handle->getColumn(value);
        QString QKey = QString::fromStdString(key);
        QVector<double> QCurve;
        QCurve.resize(curve.size());
        for (size_t i = 0; i < curve.size(); i++)
        {
            QCurve[i] = curve[i];
        }
        ng.Curves.insert(QKey, QCurve);
        //linked model
        QStandardItem* ChildModel = new QStandardItem(QKey);
        ChildModel->setEditable(false);
        ChildModel->setCheckable(true);
        ChildModel->setCheckState(Qt::CheckState::Unchecked);
        ParentModel->appendRow(ChildModel);
        ng.LinkedModelItem.insert(QKey, ChildModel);
    }

    QVector<double> t = QVector<double>(csv_handle->RowSize());
    for (size_t i = 0; i < t.size(); i++)
    {
        t[i] = static_cast<double>(i);
    }
    ng.t = t;
    this->AggregatedDataGroup__.insert(GroupName, ng);
    this->AggregatedDataModel__->appendRow(ParentModel);
    return true;
}

bool DataViewerPage::RemoveCurveGroup(const QString& GroupName)
{
    if (!this->AggregatedDataGroup__.contains(GroupName)) return false;

    DataGroup_t rg = this->AggregatedDataGroup__[GroupName];
    QMapIterator<QString, QCPGraph*> i(rg.VisiableCurve);
    while (i.hasNext()) {
        i.next();
        this->PlotHandle->removeGraph(i.value());
    }
    this->PlotHandle->replot();
    int row = rg.ParentModel->row();
    this->AggregatedDataModel__->removeRow(row);
    
    QMapIterator<QString, size_t> j(rg.ColorIndex);
    while (j.hasNext())
    {
        j.next();
        ColorPair p = this->UsedColorPair[j.value()];
        this->UsedColorPair.remove(j.value());
        this->AvailableColorPair.insert(j.value(),p );
    }

    this->AggregatedDataGroup__.remove(GroupName);
    return true;
}

void DataViewerPage::InitColorList()
{
    this->AvailableColorPair.insert(0, std::make_pair(QColor(16, 96, 220), QColor(55, 122, 231)));
    this->AvailableColorPair.insert(1, std::make_pair(QColor(0, 181, 83), QColor(72, 222, 125)));
    this->AvailableColorPair.insert(2, std::make_pair(QColor(218, 105, 2), QColor(224, 135, 58)));
    this->AvailableColorPair.insert(3, std::make_pair(QColor(199, 25, 105), QColor(219, 56, 127)));
    this->AvailableColorPair.insert(4, std::make_pair(QColor(138, 193, 20), QColor(179, 225, 81)));
    this->UsedColorPair.clear();
}

void DataViewerPage::ModelItemClickedSlot(const QModelIndex& index)
{
    qDebug() << "clicked!";
    qDebug()<<index.data(Qt::DisplayRole);
    qDebug() << index.data(Qt::CheckStateRole);
    qDebug() << index.parent().data(Qt::DisplayRole);

    if (index.parent().isValid())
    {
        QString CurveGroup = index.parent().data(Qt::DisplayRole).toString();
        QString CurveName = index.data(Qt::DisplayRole).toString();
        bool containCurve = this->AggregatedDataGroup__[CurveGroup].VisiableCurve.contains(CurveName);

        if (containCurve)
        {
            this->AggregatedDataGroup__[CurveGroup].LinkedModelItem[CurveName]->setCheckState(Qt::Unchecked);
            this->SetCurveVisiable(CurveGroup, CurveName, false,false);
        }
        else
        {
            this->AggregatedDataGroup__[CurveGroup].LinkedModelItem[CurveName]->setCheckState(Qt::Checked);
            this->SetCurveVisiable(CurveGroup, CurveName, true,false);
        }

        if (!this->AggregatedDataGroup__[CurveGroup].Curves.empty() &&
            this->AggregatedDataGroup__[CurveGroup].VisiableCurve.size() ==
            this->AggregatedDataGroup__[CurveGroup].Curves.size())
        {
            this->AggregatedDataGroup__[CurveGroup].ParentModel->setCheckState(Qt::Checked);
        }
        else if (this->AggregatedDataGroup__[CurveGroup].VisiableCurve.size() != 0)
        {
            this->AggregatedDataGroup__[CurveGroup].ParentModel->setCheckState(Qt::PartiallyChecked);
        }
        else
        {
            this->AggregatedDataGroup__[CurveGroup].ParentModel->setCheckState(Qt::Unchecked);
        }
    }
    else
    {
        QString CurveGroup = index.data(Qt::DisplayRole).toString();

        bool hasCurveVisiable = !this->AggregatedDataGroup__[CurveGroup].VisiableCurve.isEmpty();
        if (hasCurveVisiable)
        {
            for (auto item = this->AggregatedDataGroup__[CurveGroup].LinkedModelItem.begin(); 
                item != this->AggregatedDataGroup__[CurveGroup].LinkedModelItem.end(); item++)
            {
                QString Key = item.key();
                item.value()->setCheckState(Qt::Unchecked);
                this->SetCurveVisiable(CurveGroup, Key, false,false);
            }
            this->AggregatedDataGroup__[CurveGroup].ParentModel->setCheckState(Qt::Unchecked);
        }
        else
        {
            for (auto item = this->AggregatedDataGroup__[CurveGroup].LinkedModelItem.begin();
                item != this->AggregatedDataGroup__[CurveGroup].LinkedModelItem.end(); item++)
            {
                QString Key = item.key();
                item.value()->setCheckState(Qt::Checked);
                this->SetCurveVisiable(CurveGroup, Key, true, false);
            }
            this->AggregatedDataGroup__[CurveGroup].ParentModel->setCheckState(Qt::Checked);
        }
        //this->PlotHandle->replot();
    }

    if (this->AggregatedDataGroup__.empty())
    {
        this->PlotHandle->legend->setVisible(false);
    }
    else
    {
        bool visiable = false;
        for (auto item = this->AggregatedDataGroup__.begin(); item != this->AggregatedDataGroup__.end(); item++)
        {
            visiable |= !item.value().VisiableCurve.isEmpty();
        }
        this->PlotHandle->legend->setVisible(visiable);
    }
    this->PlotHandle->replot();
}

void DataViewerPage::RefillAvailableColor()
{
    if (!this->AvailableColorPair.empty())
        return;
    size_t MaxCapacity = this->UsedColorPair.size();
    size_t MaxIdx = this->UsedColorPair.lastKey();

    size_t IdxOffset = std::max(MaxCapacity, MaxIdx);
    for (auto iter=this->UsedColorPair.cbegin(); iter!=this->UsedColorPair.cend() ;iter++)
    {
        this->AvailableColorPair.insert(iter.key() + IdxOffset, iter.value());
    }
}

void DataViewerPage::UpdateCurveTheme(const QString& CurveGroup, const QString& CurveName, Theme_e theme)
{
    QPen CurvePen;
    auto ColorIdx = this->AggregatedDataGroup__[CurveGroup].ColorIndex[CurveName];
    QColor CurveColor;
    if (theme == Theme_e::Light)
        CurveColor = this->UsedColorPair[ColorIdx].first;
    else
        CurveColor = this->UsedColorPair[ColorIdx].second;

    CurvePen.setColor(CurveColor);
    CurvePen.setBrush(QBrush(CurveColor, Qt::SolidPattern));
    CurvePen.setWidthF(2.0);
    CurvePen.setStyle(Qt::SolidLine);
    CurvePen.setCapStyle(Qt::RoundCap);
    CurvePen.setJoinStyle(Qt::RoundJoin);

    QCPGraph* graph = this->AggregatedDataGroup__[CurveGroup].VisiableCurve[CurveName];
    graph->setPen(CurvePen);
    //graph->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ScatterShape::ssDisc));
}

void DataViewerPage::SetupCurve(const QString& CurveGroup, const QString& CurveName)
{
    size_t ColorIdx = this->AvailableColorPair.firstKey();
    this->UsedColorPair.insert(ColorIdx, this->AvailableColorPair[ColorIdx]);
    this->AvailableColorPair.remove(ColorIdx);
    this->AggregatedDataGroup__[CurveGroup].ColorIndex[CurveName] = ColorIdx;
    
    auto graph = this->PlotHandle->addGraph();
    this->AggregatedDataGroup__[CurveGroup].VisiableCurve[CurveName] = graph;
    graph->setSmooth(false);
    graph->setData(this->AggregatedDataGroup__[CurveGroup].t, this->AggregatedDataGroup__[CurveGroup].Curves[CurveName]);
    if(this->PlotHandle->graphCount()==1)
        graph->rescaleAxes(false);
    graph->setName(CurveName);

    qDebug() << "add curve";
    qDebug() << "Available color" << this->AvailableColorPair.size();
    qDebug() << this->AvailableColorPair;
    qDebug() << "used color" << this->UsedColorPair.size();
    qDebug() << this->UsedColorPair;
}

void DataViewerPage::RemoveCurve(const QString& CurveGroup, const QString& CurveName)
{
    size_t ColorIdx = this->AggregatedDataGroup__[CurveGroup].ColorIndex[CurveName];
    this->AvailableColorPair.insert(ColorIdx, this->UsedColorPair[ColorIdx]);
    this->UsedColorPair.remove(ColorIdx);

    this->AggregatedDataGroup__[CurveGroup].ColorIndex.remove(CurveName);
    this->PlotHandle->removeGraph(this->AggregatedDataGroup__[CurveGroup].VisiableCurve[CurveName]);
    this->AggregatedDataGroup__[CurveGroup].VisiableCurve.remove(CurveName);

    qDebug() << "remove curve";
    qDebug() << "Available color" << this->AvailableColorPair.size();
    qDebug() << this->AvailableColorPair;
    qDebug() << "used color" << this->UsedColorPair.size();
    qDebug() << this->UsedColorPair;
}

void DataViewerPage::PlotHandleMouseWheelSlot(QWheelEvent* event)
{
    qDebug() << event->angleDelta();
    qDebug() << event->position();
    qDebug() << event->position().x() / this->PlotHandle->width()<<"," << event->position().y() / this->PlotHandle->height();
    QCPRange RangeX = this->PlotHandle->xAxis->range();
    QCPRange RangeY = this->PlotHandle->yAxis->range();
    QPoint scale = event->angleDelta();
    constexpr double ScaleFactor = 1200.0;
    if (scale.x() == 0)
    {
        auto [xlow, xhigh] = this->ComputeDeltaDirection(RangeX.lower, RangeX.upper, 
            event->position().x() / this->PlotHandle->width(),
            scale.y() / ScaleFactor,false);
        RangeX.lower = xlow;
        RangeX.upper = xhigh;
        RangeX.normalize();

        auto [ylow, yhigh] = this->ComputeDeltaDirection(RangeY.lower, RangeY.upper,
            event->position().y() / this->PlotHandle->height(),
            scale.y() / ScaleFactor,true);
        RangeY.lower = ylow;
        RangeY.upper = yhigh;
        RangeY.normalize();

        this->PlotHandle->xAxis->setRange(RangeX);
        this->PlotHandle->yAxis->setRange(RangeY);
    }
    else //when alt key is pressed
    {
        auto [xlow, xhigh] = this->ComputeDeltaDirection(RangeX.lower, RangeX.upper,
            event->position().x() / this->PlotHandle->width(),
            -scale.x() / ScaleFactor,false);
        RangeX.lower = xlow;
        RangeX.upper = xhigh;
        RangeX.normalize();
        this->PlotHandle->xAxis->setRange(RangeX);
    }
}

std::tuple<double, double> DataViewerPage::ComputeDeltaDirection(double low, double high, double point, double vel, bool reverse)
{
    point = std::clamp(point, 0.0, 1.0);
    double point_ = 1.0 - point;
    if (reverse)
        std::swap(point, point_);
    double length = high - low;
    length *= vel;

    double new_low = low - length * point;
    double new_high = high + length * point_;
    return { new_low,new_high };
}

void DataViewerPage::LoadLocalFileSlot()
{
    QStringList paths = QFileDialog::getOpenFileNames(this, tr("Open File"), tr("~"), QString("*.csv"));
    if (paths.isEmpty())
        return;

    bool hasOpenedFile=false;
    QString FirstFilename;
    int loadedFile = 0;
    for (auto localpath : paths)
    {
        if (localpath.isEmpty() || !localpath.endsWith(".csv", Qt::CaseInsensitive))
        {
            QMessageBox::warning(this, tr("Unsupported File Type"), localpath+tr(" is unsupported path or file type, only .csv file type is supported!"), QMessageBox::Ok);
            continue;
        }
        else
        {
            zzs::CSVReader::Ptr r = zzs::CSVReader::Create(localpath.toStdString(), true);
            auto groupkey = localpath.split("/").back();
            if (!r->open() || !this->AddCurveGroup(groupkey, r))
            {
                QMessageBox::warning(this, tr("Failed to Open File"), tr("Failed to open ") + localpath + tr(", try again later."), QMessageBox::Ok);
                continue;
            }
            else
            {
                this->ui->DataListWidget->expand(this->AggregatedDataGroup__[groupkey].ParentModel->index());
                if (!hasOpenedFile)
                {
                    FirstFilename = groupkey;
                }
                hasOpenedFile = true;
                loadedFile++;
            }
        }
    }
    if (hasOpenedFile)
    {
        this->ui->WelcomeWidget->hide();
        this->ui->DataWidget->show();
        if (loadedFile > 1)
        {
            FirstFilename += tr(" etc.");
        }
        this->setWindowTitle(FirstFilename);
        emit this->FileLoaded(true);
    }
    else
    {
        this->setWindowTitle(tr("New Data Viewer"));
        emit this->FileLoaded(false);
    }
}

void DataViewerPage::LoadRobotFileSlot()
{
    QMessageBox::critical(this, tr("Unsupported Function"), tr("This Feature is under development, try again in the future release."), QMessageBox::Ok);
}

void DataViewerPage::RemoveButtonClickedSlot()
{
    auto keys = this->AggregatedDataGroup__.keys();
    for (auto key : keys)
    {
        this->RemoveCurveGroup(key);
    }
    this->PlotHandle->legend->setVisible(false);
    this->ui->DataWidget->hide();
    this->ui->WelcomeWidget->show();
}

void DataViewerPage::HelpButtonClickedSlot()
{
    QMessageBox::critical(this, tr("Unsupported Function"), tr("This Feature is under development, try again in the future release."), QMessageBox::Ok);
}

void DataViewerPage::initReloadButton()
{
    this->ui->PushButton_load->setIsTransparent(false);
    this->ui->PushButton_load->setToolButtonStyle(Qt::ToolButtonTextOnly);
    
    QFont font= this->font();
    font.setPixelSize(15);
    setFont(font);
    this->ui->PushButton_load->setFont(font);

    ElaMenu* menu = new ElaMenu(this);
    
    /*    QAction* action = new QAction(text, this);
    action->setProperty("ElaIconType", QChar((unsigned short)icon));
    QMenu::addAction(action);*/
    QAction* LoadfromLocalAction = new QAction(QString("    ")+tr("Load Local File")+ QString("    "), this);
    LoadfromLocalAction->setFont(font);
    menu->addAction(LoadfromLocalAction);
    QObject::connect(LoadfromLocalAction, &QAction::triggered, this, &DataViewerPage::LoadLocalFileSlot);

    QAction* LoadfromRobotAction = new QAction(QString("    ") + tr("Load from Robot") + QString("    "), this);
    LoadfromRobotAction->setFont(font);
    menu->addAction(LoadfromRobotAction);
    QObject::connect(LoadfromRobotAction, &QAction::triggered, this, &DataViewerPage::LoadRobotFileSlot);

    this->ui->PushButton_load->setMenu(menu);
    menu->setMenuItemHeight(40);
    
}
