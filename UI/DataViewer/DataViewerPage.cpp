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
#include "QProgressDialog"
#include "ElaSuggestBox.h"
#include "QTouchEvent"
#include "QCursor"

DataViewerPage::DataViewerPage(QWidget* parent)
    : QWidget(parent)
    , ui(new Ui::DataViewerPage)
    , AggregatedDataModel__(nullptr)
    , WindowTheme(Theme_e::Light)
{
    ui->setupUi(this);
    this->setWindowTitle(tr("New Data Viewer"));
    this->setAttribute(Qt::WA_TranslucentBackground, true);
    this->setAttribute(Qt::WA_AcceptTouchEvents);

    this->setAcceptDrops(true);

    QObject::connect(this->ui->DataListWidget, &QTreeView::clicked, this, &DataViewerPage::ModelItemClickedSlot);
    this->AggregatedDataModel__ = new QStandardItemModel(this);
    this->ui->DataListWidget->setModel(this->AggregatedDataModel__);
    this->AggregatedDataModel__->setHorizontalHeaderLabels(QStringList(QString("    " + tr("Experiment Data"))));

    this->PlotHandle = this->ui->DataPlotWidget;
    this->PlotHandle->installEventFilter(this);
    this->PlotHandle->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom);
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

    QObject::connect(this->ui->pushButton_clean, &ElaPushButton::clicked, this, &DataViewerPage::RemoveButtonClickedSlot);

    this->initReloadButton();
    this->initListWidgetRightClickedMenu();
    this->ui->SearchBoxWidget->setPlaceholderText(tr("Search"));
    QObject::connect(this->ui->SearchBoxWidget, &ElaSuggestBox::suggestionClicked, this, &DataViewerPage::SearchClickedSlot);

    this->InitSquareZoom();
    this->InitShowDataPoint();
    this->ui->pushButton_help->setText("?");

    this->ui->pushButton_savePDF->setText(QChar((unsigned short)ElaIconType::FloppyDisk));
    QObject::connect(this->ui->pushButton_savePDF, &ElaPushButton::clicked, this, &DataViewerPage::SavePlotSlot);

    this->InitFloatingAxis();

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
    this->PlotHandle->legend->setIconSize(40, 25);
    this->PlotHandle->legend->setBorderPen(Qt::NoPen);


    QColor ButtonLightDefaultColor(eTheme->getThemeColor(ElaThemeType::ThemeMode::Light, ElaThemeType::ThemeColor::PrimaryNormal));
    QColor ButtonLightHoverColor(eTheme->getThemeColor(ElaThemeType::ThemeMode::Light, ElaThemeType::ThemeColor::PrimaryHover));
    QColor ButtonLightPressColor(eTheme->getThemeColor(ElaThemeType::ThemeMode::Light, ElaThemeType::ThemeColor::PrimaryPress));
    QColor ButtonLightTextColor(255, 255, 255);

    QColor ButtonDarkDefaultColor(eTheme->getThemeColor(ElaThemeType::ThemeMode::Dark, ElaThemeType::ThemeColor::PrimaryNormal));
    QColor ButtonDarkHoverColor(eTheme->getThemeColor(ElaThemeType::ThemeMode::Dark, ElaThemeType::ThemeColor::PrimaryHover));
    QColor ButtonDarkPressColor(eTheme->getThemeColor(ElaThemeType::ThemeMode::Dark, ElaThemeType::ThemeColor::PrimaryPress));
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


    this->ui->pushButton_clean->setLightDefaultColor(ButtonLightDefaultColor);
    this->ui->pushButton_clean->setLightHoverColor(ButtonLightHoverColor);
    this->ui->pushButton_clean->setLightTextColor(ButtonLightTextColor);
    this->ui->pushButton_clean->setLightPressColor(ButtonLightPressColor);
    this->ui->pushButton_clean->setDarkDefaultColor(ButtonDarkDefaultColor);
    this->ui->pushButton_clean->setDarkHoverColor(ButtonDarkHoverColor);
    this->ui->pushButton_clean->setDarkTextColor(ButtonDarkTextColor);
    this->ui->pushButton_clean->setDarkPressColor(ButtonDarkPressColor);
    this->ui->pushButton_clean->setBorderRadius(this->ui->PushButton_load->getBorderRadius());

    //QColor Transparent = QColor(0, 0, 0, 0);
    //this->ui->pushButton_reset->setLightDefaultColor(Transparent);
    //this->ui->pushButton_reset->setDarkDefaultColor(Transparent);

    //this->ui->pushButton_help->setLightDefaultColor(Transparent);
    //this->ui->pushButton_help->setDarkDefaultColor(Transparent);



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

        QColor ColorRefLine(120, 120, 120);
        this->PlotRefLine__->setPen(QPen(ColorRefLine, 2, Qt::SolidLine));
        this->WindowTheme = Theme_e::Light;
    }
    else
    {
        QColor GridDarkColor(74, 74, 74);
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

        QColor ColorRefLine(150, 150, 150);
        this->PlotRefLine__->setPen(QPen(ColorRefLine, 2, Qt::SolidLine));
        this->WindowTheme = Theme_e::Dark;
    }
    this->ui->label_subtitle->setPalette(LabelPalette);

    QList<QColor> CurveColor;
    for (auto GroupItem = this->AggregatedDataGroup__.begin(); GroupItem != this->AggregatedDataGroup__.end(); GroupItem++)
    {
        for (auto CurveItem = GroupItem.value().VisiableCurve.begin(); CurveItem != GroupItem.value().VisiableCurve.end(); CurveItem++)
        {
            this->UpdateCurveTheme(GroupItem.key(), CurveItem.key(), theme);
            auto graph = CurveItem.value();
            CurveColor.push_back(graph->pen().color());
        }
    }
    this->PlotFlowIndcator__->UpdateColor(CurveColor);
    QTimer::singleShot(0, [this]() {
        this->PlotHandle->replot();
        });
}

void DataViewerPage::dragEnterEvent(QDragEnterEvent* event)
{
    qDebug() << "drag enter";
    if (event->mimeData()->hasUrls() && event->mimeData()->urls().size() == 1)
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
        bool cancel = false;
        QProgressDialog* diag = new QProgressDialog(tr("Loading data..."), tr("Cancel"), 0, 0, this);
        QObject::connect(diag, &QProgressDialog::canceled, this, [&cancel]() {
            cancel = true;
            });
        diag->setWindowFlag(Qt::WindowCloseButtonHint, false);
        diag->setWindowFlag(Qt::WindowContextHelpButtonHint, false);
        diag->setAttribute(Qt::WA_DeleteOnClose);
        diag->show();
        qApp->processEvents();

        zzs::CSVReader::Ptr r = zzs::CSVReader::Create(localpath.toStdString(), true);
        r->RegistReadStateCallback([&cancel, this](bool& cancel_) {
            qApp->processEvents();
            cancel_ = cancel;
            });

        auto groupkey = localpath.split("/").back();
        if (!r->open() || !this->AddCurveGroup(groupkey, r))
        {
            if (cancel == false)
                QMessageBox::warning(this, tr("Failed to Open File"), tr("Failed to open this file, try again later."), QMessageBox::Ok);
            diag->close();
            qApp->processEvents();
            return;
        }
        diag->close();
        qApp->processEvents();
        this->setWindowTitle(groupkey);
        this->ui->WelcomeWidget->hide();
        this->ui->DataWidget->show();
        this->ui->DataListWidget->expand(this->AggregatedDataGroup__[groupkey].ParentModel->index());
        emit this->FileLoaded(true);
    }
}

bool DataViewerPage::event(QEvent* e)
{
    auto type = e->type();
    //qDebug() << type;
    if (type == QEvent::Type::TouchBegin ||
        type == QEvent::Type::TouchCancel ||
        type == QEvent::Type::TouchEnd ||
        type == QEvent::Type::TouchUpdate
        )
    {
        QTouchEvent* TouchEvent = dynamic_cast<QTouchEvent*>(e);
        if (TouchEvent != nullptr)
        {
            TouchEvent->accept();
            qDebug() << "touch event!";
        }
        else
            qDebug() << "cast failed";
    }

    return QWidget::event(e);
}

bool DataViewerPage::eventFilter(QObject* watched, QEvent* event)
{
    if (watched == this->PlotHandle)
    {
        if (event->type() == QEvent::Enter)
        {
            qDebug() << "move inter";
            this->MouseMovedInPlot__ = true;
            this->PlotRefLine__->setVisible(true);

            //FIXME: solve tracer performance issue.
            auto groups = this->AggregatedDataGroup__.keys();
            QList<QString> CurveNames;
            QList<QColor> CurveColors;
            QList<double> CurveValue;
            QPoint GMouse = QCursor::pos();
            QPoint LMouse = this->PlotHandle->mapFromGlobal(GMouse);
            double CurX = this->PlotHandle->xAxis->pixelToCoord(LMouse.x());
            for (auto&& group : groups)
            {
                auto curves = this->AggregatedDataGroup__[group].Tracer.keys();
                for (auto&& curve : curves)
                {
                    auto tracer = this->AggregatedDataGroup__[group].Tracer[curve];
                    tracer->setVisible(true);
                    tracer->setGraphKey(CurX);
                    double y = tracer->positions()[0]->value();
                    CurveValue.append(y);
                    if (groups.size() == 1)
                    {
                        CurveNames.push_back(curve);
                    }
                    else
                    {
                        CurveNames.push_back(group + "/" + curve);
                    }
                    QColor c = tracer->pen().color();
                    CurveColors.push_back(c);
                }
            }

            QTimer::singleShot(0, [this]() {
                this->PlotHandle->replot();
                });

            this->PlotFlowIndcator__->SetLabelText(CurveNames, CurveColors);
            this->PlotFlowIndcator__->UpdateValue(CurveValue);
            if(!CurveNames.empty())
                this->PlotFlowIndcator__->show();
            qDebug() << "inter call";
            return true;
        }
        else if (event->type() == QEvent::Leave)
        {
            qDebug() << "move out";

            QPoint TopLeftCoord = this->PlotHandle->mapToGlobal(QPoint(0, 0));
            QPoint l = QPoint(this->PlotHandle->width(), this->PlotHandle->height());
            QPoint ButtonRightCoord = this->PlotHandle->mapToGlobal(l);
            QPoint MouseCoord = QCursor::pos();
            if (MouseCoord.x() > TopLeftCoord.x() &&
                MouseCoord.y() > TopLeftCoord.y() &&
                MouseCoord.x() < ButtonRightCoord.x() &&
                MouseCoord.y() < ButtonRightCoord.y())
            {
                return true;
            }

            this->MouseMovedInPlot__ = false;
            this->PlotRefLine__->setVisible(false);

            auto groups = this->AggregatedDataGroup__.keys();
            for (auto&& group : groups)
            {
                auto curves = this->AggregatedDataGroup__[group].Tracer.keys();
                for (auto&& curve : curves)
                {
                    auto tracer = this->AggregatedDataGroup__[group].Tracer[curve];
                    tracer->setVisible(false);
                }
            }

            QTimer::singleShot(0, [this]() {
                this->PlotHandle->replot();
                });
            this->PlotFlowIndcator__->hide();
            return true;
        }
    }
    return QWidget::eventFilter(watched, event);
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
        if (this->PlotHandle->graphCount() == 1)
        {
            QCPRange CurrentRangeX = this->PlotHandle->xAxis->range();
            QCPRange CurrentRangeY = this->PlotHandle->yAxis->range();
            auto graph = this->PlotHandle->graph();
            graph->rescaleAxes(false);
            QCPRange TargetRangeX = this->PlotHandle->xAxis->range();
            QCPRange TargetRangeY = this->PlotHandle->yAxis->range();
            this->PlotHandle->xAxis->setRange(CurrentRangeX);
            this->PlotHandle->yAxis->setRange(CurrentRangeY);
            this->PlotHandle->replot();
            qApp->processEvents();
            this->FakeZoomAnimation(TargetRangeX, TargetRangeY);
        }
    }
    else
    {
        if (!this->AggregatedDataGroup__[CurveGroup].VisiableCurve.contains(CurveName))
        {
            return;
        }
        this->RemoveCurve(CurveGroup, CurveName);
    }
    if (replot)
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

    if (this->AggregatedDataGroup__.size() == 1)
    {
        QStringList keys = this->AggregatedDataGroup__.first().Curves.keys();
        QString GroupName = this->AggregatedDataGroup__.firstKey();
        QString GroupKey = GroupName.split('.')[0];
        for (auto key : keys)
        {
            this->ui->SearchBoxWidget->removeSuggestion(key);
            this->ui->SearchBoxWidget->addSuggestion(GroupKey + "/" + key);
        }
    }

    this->AggregatedDataGroup__.insert(GroupName, ng);
    this->AggregatedDataModel__->appendRow(ParentModel);

    QStringList keys = this->AggregatedDataGroup__[GroupName].Curves.keys();
    QString GroupKey = GroupName.split('.')[0];
    for (auto key : keys)
    {
        if (this->AggregatedDataGroup__.size() == 1)
            this->ui->SearchBoxWidget->addSuggestion(key);
        else
            this->ui->SearchBoxWidget->addSuggestion(GroupKey + "/" + key);
    }

    return true;
}

bool DataViewerPage::RemoveCurveGroup(const QString& GroupName)
{
    if (!this->AggregatedDataGroup__.contains(GroupName)) return false;

    DataGroup_t rg = this->AggregatedDataGroup__[GroupName];

    QStringList keynames = rg.Curves.keys();
    QString GroupKey = GroupName.split('.')[0];
    for (auto key : keynames)
    {
        if (this->AggregatedDataGroup__.size() == 1)
        {
            this->ui->SearchBoxWidget->removeSuggestion(key);
        }
        else
        {
            QString keyword = GroupKey + "/" + key;
            this->ui->SearchBoxWidget->removeSuggestion(keyword);
        }
    }

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
        this->AvailableColorPair.insert(j.value(), p);
    }

    this->AggregatedDataGroup__.remove(GroupName);
    if (this->AggregatedDataGroup__.size() == 1)
    {
        QString GroupName_ = this->AggregatedDataGroup__.firstKey();
        QString GroupKey_ = GroupName_.split('.')[0];
        QStringList Keys_ = this->AggregatedDataGroup__.first().Curves.keys();
        for (auto key : Keys_)
        {
            this->ui->SearchBoxWidget->removeSuggestion(GroupKey_ + "/" + key);
            this->ui->SearchBoxWidget->addSuggestion(key);
        }
    }

    return true;
}

void DataViewerPage::InitColorList()
{
    this->AvailableColorPair.insert(0, std::make_pair(QColor(16, 96, 220), QColor(55, 122, 231)));
    this->AvailableColorPair.insert(1, std::make_pair(QColor(0, 181, 83), QColor(72, 222, 125)));
    this->AvailableColorPair.insert(2, std::make_pair(QColor(218, 105, 2), QColor(224, 135, 58)));
    this->AvailableColorPair.insert(3, std::make_pair(QColor(199, 25, 105), QColor(219, 56, 127)));
    this->AvailableColorPair.insert(4, std::make_pair(QColor(138, 193, 20), QColor(179, 225, 81)));
    this->AvailableColorPair.insert(5, std::make_pair(QColor(107, 105, 214), QColor(142, 140, 216)));

    this->UsedColorPair.clear();
}

void DataViewerPage::ModelItemClickedSlot(const QModelIndex& index)
{
    //qDebug() << "clicked!";
    //qDebug() << index.data(Qt::DisplayRole);
    //qDebug() << index.data(Qt::CheckStateRole);
    //qDebug() << index.parent().data(Qt::DisplayRole);

    if (index.parent().isValid())
    {
        QString CurveGroup = index.parent().data(Qt::DisplayRole).toString();
        QString CurveName = index.data(Qt::DisplayRole).toString();
        bool containCurve = this->AggregatedDataGroup__[CurveGroup].VisiableCurve.contains(CurveName);

        if (containCurve)
        {
            this->AggregatedDataGroup__[CurveGroup].LinkedModelItem[CurveName]->setCheckState(Qt::Unchecked);
            this->SetCurveVisiable(CurveGroup, CurveName, false, false);
        }
        else
        {
            this->AggregatedDataGroup__[CurveGroup].LinkedModelItem[CurveName]->setCheckState(Qt::Checked);
            this->SetCurveVisiable(CurveGroup, CurveName, true, false);
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
                this->SetCurveVisiable(CurveGroup, Key, false, false);
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
    for (auto iter = this->UsedColorPair.cbegin(); iter != this->UsedColorPair.cend();iter++)
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

    auto tracer = this->AggregatedDataGroup__[CurveGroup].Tracer[CurveName];
    QPen TracerPen = CurvePen;
    TracerPen.setWidthF(6);
    tracer->setPen(TracerPen);
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

    graph->setName(CurveName);
    if (this->isShowDataPoint__)
        graph->setScatterStyle(QCPScatterStyle::ssCircle);
    else
        graph->setScatterStyle(QCPScatterStyle::ssNone);

    //qDebug() << "add curve";
    //qDebug() << "Available color" << this->AvailableColorPair.size();
    //qDebug() << this->AvailableColorPair;
    //qDebug() << "used color" << this->UsedColorPair.size();
    //qDebug() << this->UsedColorPair;

    QCPItemTracer* tracer = new QCPItemTracer(this->PlotHandle);
    tracer->setStyle(QCPItemTracer::tsCircle);
    tracer->setBrush(Qt::SolidPattern);
    tracer->setSize(6);
    tracer->setGraph(graph);
    tracer->setVisible(true);
    tracer->position->setType(QCPItemPosition::ptPlotCoords);
    this->AggregatedDataGroup__[CurveGroup].Tracer[CurveName] = tracer;
}

void DataViewerPage::RemoveCurve(const QString& CurveGroup, const QString& CurveName)
{
    size_t ColorIdx = this->AggregatedDataGroup__[CurveGroup].ColorIndex[CurveName];
    this->AvailableColorPair.insert(ColorIdx, this->UsedColorPair[ColorIdx]);
    this->UsedColorPair.remove(ColorIdx);

    this->AggregatedDataGroup__[CurveGroup].ColorIndex.remove(CurveName);
    this->PlotHandle->removeGraph(this->AggregatedDataGroup__[CurveGroup].VisiableCurve[CurveName]);
    this->AggregatedDataGroup__[CurveGroup].VisiableCurve.remove(CurveName);

    //qDebug() << "remove curve";
    //qDebug() << "Available color" << this->AvailableColorPair.size();
    //qDebug() << this->AvailableColorPair;
    //qDebug() << "used color" << this->UsedColorPair.size();
    //qDebug() << this->UsedColorPair;

    auto tracer = this->AggregatedDataGroup__[CurveGroup].Tracer[CurveName];
    tracer->setGraph(nullptr);
    this->PlotHandle->removeItem(tracer);
    this->AggregatedDataGroup__[CurveGroup].Tracer.remove(CurveName);
}

void DataViewerPage::PlotHandleMouseWheelSlot(QWheelEvent* event)
{
    /*qDebug() << event->angleDelta();
    qDebug() << event->position();
    qDebug() << event->position().x() / this->PlotHandle->width() << "," << event->position().y() / this->PlotHandle->height();*/
    QCPRange RangeX = this->PlotHandle->xAxis->range();
    QCPRange RangeY = this->PlotHandle->yAxis->range();
    QPoint scale = event->angleDelta();
    constexpr double ScaleFactor = 1200.0;
    if (scale.x() == 0)
    {
        auto [xlow, xhigh] = this->ComputeDeltaDirection(RangeX.lower, RangeX.upper,
            event->position().x() / this->PlotHandle->width(),
            scale.y() / ScaleFactor, false);
        RangeX.lower = xlow;
        RangeX.upper = xhigh;
        RangeX.normalize();

        auto [ylow, yhigh] = this->ComputeDeltaDirection(RangeY.lower, RangeY.upper,
            event->position().y() / this->PlotHandle->height(),
            scale.y() / ScaleFactor, true);
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
            -scale.x() / ScaleFactor, false);
        RangeX.lower = xlow;
        RangeX.upper = xhigh;
        RangeX.normalize();
        this->PlotHandle->xAxis->setRange(RangeX);
    }
}

void DataViewerPage::RangeChangedSlot(const QCPRange& newRange)
{

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
    QStringList paths;
    QString file = QFileDialog::getOpenFileName(this, tr("Open File"), tr("~"), QString("*.csv"));
    if (file.isEmpty())
        return;

    paths.push_back(file);
    if (paths.isEmpty())
        return;

    bool hasOpenedFile = false;
    QString FirstFilename;
    int loadedFile = 0;
    for (auto localpath : paths)
    {
        if (localpath.isEmpty() || !localpath.endsWith(".csv", Qt::CaseInsensitive))
        {
            QMessageBox::warning(this, tr("Unsupported File Type"), localpath + tr(" is unsupported path or file type, only .csv file type is supported!"), QMessageBox::Ok);
            continue;
        }
        else
        {
            bool cancel = false;
            QProgressDialog* diag = new QProgressDialog(tr("Loading data..."), tr("Cancel"), 0, 0, this);
            QObject::connect(diag, &QProgressDialog::canceled, this, [&cancel]() {
                cancel = true;
                });
            diag->setWindowFlag(Qt::WindowCloseButtonHint, false);
            diag->setWindowFlag(Qt::WindowContextHelpButtonHint, false);
            diag->setAttribute(Qt::WA_DeleteOnClose);
            diag->show();
            qApp->processEvents();
            zzs::CSVReader::Ptr r = zzs::CSVReader::Create(localpath.toStdString(), true);
            r->RegistReadStateCallback([&cancel, this](bool& cancel_) {
                qApp->processEvents();
                cancel_ = cancel;
                });
            auto groupkey = localpath.split("/").back();
            if (!r->open() || !this->AddCurveGroup(groupkey, r))
            {
                if (!cancel)
                    QMessageBox::warning(this, tr("Failed to Open File"), tr("Failed to open ") + localpath + tr(", try again later."), QMessageBox::Ok);
                diag->close();
                qApp->processEvents();
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
                diag->close();
                qApp->processEvents();
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

void DataViewerPage::initReloadButton()
{
    this->ui->PushButton_load->setIsTransparent(false);
    this->ui->PushButton_load->setToolButtonStyle(Qt::ToolButtonTextOnly);

    QFont font = this->font();
    font.setPixelSize(15);
    setFont(font);
    this->ui->PushButton_load->setFont(font);

    ElaMenu* menu = new ElaMenu(this);

    QAction* LoadfromLocalAction = new QAction(tr("Load Local File"), this);
    LoadfromLocalAction->setFont(font);
    //LoadfromLocalAction->setProperty("ElaIconType", QChar((unsigned short)ElaIconType::ComputerClassic));
    menu->addAction(LoadfromLocalAction);
    QObject::connect(LoadfromLocalAction, &QAction::triggered, this, &DataViewerPage::LoadLocalFileSlot);

    QAction* LoadfromRobotAction = new QAction(tr("Load from Robot"), this);
    LoadfromRobotAction->setFont(font);
    //LoadfromRobotAction->setProperty("ElaIconType", QChar((unsigned short)ElaIconType::Command));
    menu->addAction(LoadfromRobotAction);
    QObject::connect(LoadfromRobotAction, &QAction::triggered, this, &DataViewerPage::LoadRobotFileSlot);

    this->ui->PushButton_load->setMenu(menu);
    menu->setMenuItemHeight(40);

}

void DataViewerPage::initListWidgetRightClickedMenu()
{
    this->ui->DataListWidget->setContextMenuPolicy(Qt::CustomContextMenu);
    QObject::connect(this->ui->DataListWidget, &ElaTreeView::customContextMenuRequested, this, &DataViewerPage::ListWidgetRightClickedSlot);
}

void DataViewerPage::InitSquareZoom()
{
    this->ui->SquareZoomButton->setText(QChar((unsigned short)ElaIconType::SquareDashedCirclePlus));
    this->ui->pushButton_reset->setText(QChar((unsigned short)ElaIconType::ArrowsMinimize));


    this->SquareZoomMode__ = false;
    QObject::connect(this->ui->SquareZoomButton, &ElaToggleButton::toggled, this, [this](bool toggle) {
        if (toggle)
        {
            this->SquareZoomMode__ = true;
            this->PlotHandle->setSelectionRectMode(QCP::SelectionRectMode::srmCustom);
        }
        else
        {
            this->SquareZoomMode__ = false;
            this->PlotHandle->setSelectionRectMode(QCP::SelectionRectMode::srmNone);
        }
        });
    QObject::connect(this->PlotHandle->selectionRect(), &QCPSelectionRect::SelectedRange, this, &DataViewerPage::SquareZoomRangeSlot);
    this->PlotHandle->selectionRect()->setPen(QPen(Qt::NoPen));
    this->PlotHandle->selectionRect()->setBrush(QBrush(QColor(128, 128, 128, 128)));


    QObject::connect(this->ui->pushButton_reset, &ElaPushButton::clicked, this, [this]() {
        qDebug() << "reset view";
        //this->PlotHandle->rescaleAxes(false);
        auto CurrentRangeX = this->PlotHandle->xAxis->range();
        auto CurrentRangeY = this->PlotHandle->yAxis->range();
        this->PlotHandle->rescaleAxes(false);
        qApp->processEvents();
        auto TargetRangeX = this->PlotHandle->xAxis->range();
        auto TargetRangeY = this->PlotHandle->yAxis->range();
        qApp->processEvents();
        this->PlotHandle->xAxis->setRange(CurrentRangeX);
        this->PlotHandle->yAxis->setRange(CurrentRangeY);
        qApp->processEvents();
        this->FakeZoomAnimation(TargetRangeX, TargetRangeY);
        //this->PlotHandle->replot();
        });
}


void DataViewerPage::SquareZoomRangeSlot(const QCPRange& rangeX, const QCPRange& rangeY)
{
    //this->PlotHandle->xAxis->setRange(rangeX);
    //this->PlotHandle->yAxis->setRange(rangeY);
    this->FakeZoomAnimation(rangeX, rangeY);
    this->PlotHandle->replot();
    this->ui->SquareZoomButton->setIsToggled(false);
}


void DataViewerPage::SavePlotSlot()
{
    QString filename = QFileDialog::getSaveFileName(this, tr("Save the Figure"), "./",
        "PDF File(*.pdf);; \
        JPEG File Interchange Format(*.jpg);; \
        Portable Network Graphics (*.png)");
    if (filename.isEmpty())
        return;

    QString format = filename.split('.').back();
    int width = this->PlotHandle->width();
    int height = this->PlotHandle->height();
    bool saved = false;

    /*
            this->PlotHandle->xAxis->setTickLabelColor(TickLabelColorLight);
        this->PlotHandle->yAxis->setTickLabelColor(TickLabelColorLight);
        this->PlotHandle->legend->setTextColor(TickLabelColorLight);
        this->PlotHandle->legend->setBrush(QColor(226, 226, 226, 128));*/
    QColor TickLabelColorX = this->PlotHandle->xAxis->tickLabelColor();
    QColor TickLabelColorY = this->PlotHandle->yAxis->tickLabelColor();
    QColor LegendTextColor = this->PlotHandle->legend->textColor();
    QBrush LegendBrush = this->PlotHandle->legend->brush();
    QPen xAxisPen = this->PlotHandle->xAxis->grid()->pen();
    QPen yAxisPen = this->PlotHandle->yAxis->grid()->pen();
    QPen modiX = xAxisPen;
    QPen modiY = yAxisPen;
    modiX.setColor(QColor(200, 200, 200));
    modiY.setColor(QColor(200, 200, 200));

    this->PlotHandle->xAxis->setTickLabelColor(QColor(0, 0, 0, 255));
    this->PlotHandle->yAxis->setTickLabelColor(QColor(0, 0, 0, 255));
    this->PlotHandle->legend->setTextColor(QColor(0, 0, 0, 255));
    this->PlotHandle->legend->setBrush(QColor(255, 255, 255));
    this->PlotHandle->legend->setBorderPen(Qt::SolidLine);
    this->PlotHandle->setBackground(QBrush(QColor(255, 255, 255, 255)));
    this->PlotHandle->xAxis->grid()->setPen(modiX);
    this->PlotHandle->yAxis->grid()->setPen(modiY);

    if (format == "pdf")
    {
        saved = this->PlotHandle->savePdf(filename, width, height);
    }
    else if (format == "jpg")
    {
        saved = this->PlotHandle->saveJpg(filename, width * 2, height * 2);
    }
    else if (format == "png")
    {
        saved = this->PlotHandle->savePng(filename, width * 2, height * 2);
    }
    else
    {
        QMessageBox::critical(this, tr("Error"), tr("Unknown File Format"), QMessageBox::Ok);
        saved = true; //prevent re-pop up window
    }

    this->PlotHandle->xAxis->setTickLabelColor(TickLabelColorX);
    this->PlotHandle->yAxis->setTickLabelColor(TickLabelColorY);
    this->PlotHandle->legend->setBrush(LegendBrush);
    this->PlotHandle->legend->setTextColor(LegendTextColor);
    this->PlotHandle->legend->setBorderPen(Qt::NoPen);
    this->PlotHandle->setBackground(QBrush(QColor(0, 0, 0, 0)));
    this->PlotHandle->xAxis->grid()->setPen(xAxisPen);
    this->PlotHandle->yAxis->grid()->setPen(yAxisPen);

    if (!saved)
        QMessageBox::critical(this, tr("Error"), tr("Failed to save file, try again later."), QMessageBox::Ok);
    else
        QMessageBox::information(this, tr("Saved"), tr("Save success!"), QMessageBox::Ok);
}

void DataViewerPage::InitShowDataPoint()
{
    this->isShowDataPoint__ = false;
    this->ui->pushButton_showPoint->setText(QChar((unsigned short)ElaIconType::GripDots));
    QObject::connect(this->ui->pushButton_showPoint, &ElaPushButton::clicked, this, [this]() {
        this->isShowDataPoint__ = !this->isShowDataPoint__;

        if (this->isShowDataPoint__)
            this->ui->pushButton_showPoint->setText(QChar((unsigned short)ElaIconType::WaveformLines));
        else
            this->ui->pushButton_showPoint->setText(QChar((unsigned short)ElaIconType::GripDots));

        auto Groups = this->AggregatedDataGroup__.keys();
        for (auto& Group : Groups)
        {
            if (this->AggregatedDataGroup__[Group].VisiableCurve.empty())
                continue;
            auto Curves = this->AggregatedDataGroup__[Group].VisiableCurve.keys();
            for (auto& Curve : Curves)
            {
                if (this->isShowDataPoint__)
                    this->AggregatedDataGroup__[Group].VisiableCurve[Curve]->setScatterStyle(QCPScatterStyle::ssCircle);
                else
                    this->AggregatedDataGroup__[Group].VisiableCurve[Curve]->setScatterStyle(QCPScatterStyle::ssNone);
            }
        }
        this->PlotHandle->replot();
        });

}

void DataViewerPage::InitFloatingAxis()
{
    this->PlotRefLine__ = new QCPItemStraightLine(this->PlotHandle);
    this->PlotRefLine__->setVisible(false);
    QObject::connect(this->PlotHandle, &QCustomPlot::mouseMove, this, &DataViewerPage::PlotMouseMoveHandle);

    this->PlotFlowIndcator__ = new DataViewerFlowIndicator();
}

void DataViewerPage::PlotMouseMoveHandle(QMouseEvent* event)
{
    if (event->buttons() != Qt::NoButton)
    {
        this->PlotFlowIndcator__->hide();
        return;
    }
    
    //qDebug() << "mouse moved" << event->pos() << event->buttons();
    QPointF MousePos = event->pos();
    QPoint MouseGlobalPos = event->globalPos();
    double CurX = this->PlotHandle->xAxis->pixelToCoord(MousePos.x());
    double TopY = this->PlotHandle->yAxis->pixelToCoord(0);
    double ButtomY = this->PlotHandle->yAxis->pixelToCoord(this->PlotHandle->height());
    this->PlotRefLine__->point1->setCoords(CurX, TopY);
    this->PlotRefLine__->point2->setCoords(CurX, ButtomY);

    QList<double> CurveValue;
    auto groups = this->AggregatedDataGroup__.keys();
    for (auto&& group : groups)
    {
        auto curves = this->AggregatedDataGroup__[group].Tracer.keys();
        for (auto&& curve : curves)
        {
            auto tracer = this->AggregatedDataGroup__[group].Tracer[curve];
            tracer->setVisible(true);
            tracer->setGraphKey(CurX);
            double y = tracer->positions()[0]->value();
            CurveValue.append(y);
        }
    }

    this->PlotHandle->replot();
    MouseGlobalPos.rx() += 15;
    MouseGlobalPos.ry() += 15;
    if (CurveValue.empty())
    {
        this->PlotFlowIndcator__->hide();
    }      
    else
    {
        this->PlotFlowIndcator__->UpdateValue(CurveValue);
        this->PlotFlowIndcator__->move(MouseGlobalPos);
        this->PlotFlowIndcator__->show();
    }
}

void DataViewerPage::SearchClickedSlot(QString suggestText, QVariantMap suggestData)
{

    QString GroupName, CurveName;
    if (this->AggregatedDataGroup__.size() == 1)
    {
        CurveName = suggestText;
        GroupName = this->AggregatedDataGroup__.firstKey();
    }
    else
    {
        QStringList Split = suggestText.split('/');
        GroupName = Split[0] + ".csv";
        CurveName = Split[1];
    }

    if (!this->AggregatedDataGroup__.contains(GroupName) || !this->AggregatedDataGroup__[GroupName].Curves.contains(CurveName))
        return;

    if (this->AggregatedDataGroup__[GroupName].VisiableCurve.contains(CurveName))
    {
        this->SetCurveVisiable(GroupName, CurveName, false, false);
        this->AggregatedDataGroup__[GroupName].LinkedModelItem[CurveName]->setCheckState(Qt::Unchecked);
    }

    else
    {
        this->SetCurveVisiable(GroupName, CurveName, true, false);
        this->AggregatedDataGroup__[GroupName].LinkedModelItem[CurveName]->setCheckState(Qt::Checked);
    }

    if (this->AggregatedDataGroup__[GroupName].VisiableCurve.empty())
    {
        this->AggregatedDataGroup__[GroupName].ParentModel->setCheckState(Qt::Unchecked);
    }
    else if (this->AggregatedDataGroup__[GroupName].VisiableCurve.size() == this->AggregatedDataGroup__[GroupName].Curves.size())
    {
        this->AggregatedDataGroup__[GroupName].ParentModel->setCheckState(Qt::Checked);
    }
    else
    {
        this->AggregatedDataGroup__[GroupName].ParentModel->setCheckState(Qt::PartiallyChecked);
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

void DataViewerPage::FakeZoomAnimation(const QCPRange& TargetRangeX, const QCPRange& TargetRangeY, size_t time, size_t MaxIter)
{
    qDebug() << "fake animation start";
    QCPRange CurrentRangeX = this->PlotHandle->xAxis->range();
    QCPRange CurrentRangeY = this->PlotHandle->yAxis->range();

    auto CalCulateIter = [MaxIter](double CurrentPos, double TargetPos) ->std::vector<double> {
        std::vector<double> PosList(MaxIter);
        double DeltaRange = TargetPos - CurrentPos;
        double A = -6.0 * DeltaRange / (double)(MaxIter * MaxIter * MaxIter);

        auto Range = [A, MaxIter](double x)->double {
            double MaxIterf = static_cast<double>(MaxIter);
            double rtn = A * std::pow(x, 3) / 3 - A * MaxIterf * std::pow(x, 2) / 2;
            return rtn;
            };

        for (size_t i = 0; i < MaxIter; i++)
        {

            PosList[i] = CurrentPos + Range(static_cast<double>(i));
            //PosList[i] = CurrentPos + DeltaRange * ((double)i) / MaxIter;
        }
        return PosList;
        };

    auto PosListXL = CalCulateIter(CurrentRangeX.lower, TargetRangeX.lower);
    auto PosListXH = CalCulateIter(CurrentRangeX.upper, TargetRangeX.upper);
    auto PosListYL = CalCulateIter(CurrentRangeY.lower, TargetRangeY.lower);
    auto PosListYH = CalCulateIter(CurrentRangeY.upper, TargetRangeY.upper);

    auto start = std::chrono::system_clock::now();
    while (true)
    {
        auto current = std::chrono::system_clock::now();

        std::chrono::duration<double> diff = current - start;
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(current - start);
        int time_count = duration.count();
        time_count = std::max(time_count, static_cast<int>(0));
        if (time_count > time)
            break;
        double idx_f = (double)time_count / (double)time * MaxIter;
        int idx = std::round(idx_f);
        idx = std::clamp(idx, 0, static_cast<int>(MaxIter - 1));

        CurrentRangeX.lower = PosListXL[idx];
        CurrentRangeX.upper = PosListXH[idx];
        CurrentRangeY.lower = PosListYL[idx];
        CurrentRangeY.upper = PosListYH[idx];
        this->PlotHandle->xAxis->setRange(CurrentRangeX);
        this->PlotHandle->yAxis->setRange(CurrentRangeY);
        this->PlotHandle->replot(QCustomPlot::rpImmediateRefresh);
        //qApp->processEvents(QEventLoop::ExcludeUserInputEvents | QEventLoop::ExcludeSocketNotifiers, 1);
        qApp->processEvents();
    }


    this->PlotHandle->xAxis->setRange(TargetRangeX);
    this->PlotHandle->yAxis->setRange(TargetRangeY);
    this->PlotHandle->replot(QCustomPlot::rpImmediateRefresh);
    //qApp->processEvents(QEventLoop::ExcludeUserInputEvents | QEventLoop::ExcludeSocketNotifiers, 1);
    qApp->processEvents();
    qDebug() << "fake animation done";
}


void DataViewerPage::ListWidgetRightClickedSlot(const QPoint& pos)
{
    QModelIndex index = this->ui->DataListWidget->indexAt(pos);
    if (index.parent().isValid())
        return;
    QString GroupKey = index.data(Qt::DisplayRole).toString();
    ElaMenu* menu = new ElaMenu(this);
    auto CleanAction = menu->addAction(QString("    ") + tr("Clean") + QString("    "), this, [this, GroupKey]() {
        this->RemoveCurveGroup(GroupKey);

        if (this->AggregatedDataGroup__.isEmpty())
        {
            this->ui->DataWidget->hide();
            this->ui->WelcomeWidget->show();
        }
        });

    bool isModelExpanded = this->ui->DataListWidget->isExpanded(index);
    QString ActionName = (isModelExpanded) ? (QString("    ") + tr("Collapse") + QString("    ")) : (QString("    ") + tr("Expand") + QString("    "));
    auto ExpandAction = menu->addAction(ActionName, this, [this, isModelExpanded, index]() {
        this->ui->DataListWidget->setExpanded(index, !isModelExpanded);
        });
    //action->setProperty("ElaIconType", QChar((unsigned short)icon));
    //ExpandAction->setProperty("ElaIconType", QChar((unsigned short)ElaIconType::SquareList));
    //CleanAction->setProperty("ElaIconType", QChar((unsigned short)ElaIconType::Broom));

    QFont font = this->font();
    font.setPixelSize(15);
    ExpandAction->setFont(font);
    CleanAction->setFont(font);

    menu->setAttribute(Qt::WA_DeleteOnClose);
    menu->setMenuItemHeight(40);

    menu->exec(QCursor::pos());
}