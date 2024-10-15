#include "DataViewerFlowIndicator.h"
#include "ElaTheme.h"
#include "ElaApplication.h"
#include "QPainter"
#include "QLabel"
#include "ElaText.h"
#include "QHBoxLayout"
#include "QVBoxLayout"
#include "QDebug"
#include "QGraphicsDropShadowEffect"
#include "QPainterPath"

DataViewerFlowIndicator::DataViewerFlowIndicator(QWidget* Parent)
	:QWidget(Parent)
{
	this->setWindowFlag(Qt::ToolTip);
    this->setWindowFlag(Qt::FramelessWindowHint);

    this->setAttribute(Qt::WA_TranslucentBackground);
    this->VLay__ = new QVBoxLayout(this);

    this->_themeMode = eTheme->getThemeMode();
    connect(eTheme, &ElaTheme::themeModeChanged, this, [=](ElaThemeType::ThemeMode themeMode) {
        this->_themeMode = themeMode;
        this->ThemeChanged(themeMode);
        update();
        });
    
    this->_isEnableMica = false;// = eApp->getIsEnableMica();
    connect(eApp, &ElaApplication::pIsEnableMicaChanged, this, [=]() {
        this->_isEnableMica = eApp->getIsEnableMica();
        this->_isEnableMica = false;
        update();
        });
    //eApp->syncMica(this, this->_isEnableMica);
    this->setBaseSize(0, 0);
    this->setMouseTracking(true);
}

DataViewerFlowIndicator::~DataViewerFlowIndicator()
{
}

void DataViewerFlowIndicator::UpdateValue(const QList<double>& value)
{
    if (value.size() != this->CurveValueList__.size())
    {
        qDebug() << "value and index mismatch, value will not update";
    }
    for (size_t i = 0; i < value.size(); i++)
    {
        QString val = QString::number(value[i], 'f', 2);
        this->CurveValueList__[i]->setText(val);
    }
}

void DataViewerFlowIndicator::SetLabelText(const QList<QString>& text, const QList<QColor>& Colors)
{
    if (text.size() != Colors.size())
    {
        qDebug() << "label size and color size mis-match, will not update label";
        return;
    }
    for (size_t i = 0; i < this->CurveIconList__.size(); i++)
    {
        delete this->CurveIconList__[i];
        delete this->CurveNameList__[i];
        delete this->CurveValueList__[i];
        delete this->CurveLayout__[i];
    }
    this->CurveIconList__.clear();
    this->CurveNameList__.clear();
    this->CurveValueList__.clear();
    this->CurveLayout__.clear();

    this->ColorPairList__ = Colors;
    for (size_t i = 0; i < text.size(); i++)
    {
        QLabel* Icon = new QLabel(this);
        Icon->setFixedSize(5, 20);
        QPalette p;
        QColor color = this->ColorPairList__[i];
        color.setAlpha(255);
        p.setColor(QPalette::Window, color);
        Icon->setPalette(p);
        Icon->setAutoFillBackground(true);

        ElaText* Label = new ElaText(this);
        QFont font = Label->font();
        font.setPointSize(10);
        Label->setFont(font);
        Label->setText(text[i]);
        
        ElaText* Value = new ElaText(this);
        Value->setFont(font);
        Value->setText("0.0");

        QHBoxLayout* hlay = new QHBoxLayout();
        hlay->addWidget(Icon);
        hlay->addWidget(Label);
        hlay->addWidget(Value);
        this->VLay__->addLayout(hlay);

        this->CurveIconList__.push_back(Icon);
        this->CurveNameList__.push_back(Label);
        this->CurveValueList__.push_back(Value);
        this->CurveLayout__.push_back(hlay);
    }
    this->ThemeChanged(eTheme->getThemeMode());

    if (text.empty())
    {
        this->setBaseSize(0, 0);
    }
}

void DataViewerFlowIndicator::UpdateColor(const QList<QColor>& Colors)
{
    if (Colors.size() != this->CurveValueList__.size())
    {
        qDebug() << "color list and curve list mismatched!";
        return;
    }
    for (size_t i = 0; i < Colors.size(); i++)
    {
        QPalette p = this->CurveIconList__[i]->palette();
        p.setColor(QPalette::Window, Colors[i]);
    }
}

void DataViewerFlowIndicator::paintEvent(QPaintEvent* event)
{
    QWidget::paintEvent(event);
    if (!this->_isEnableMica)
    {
        QPainter painter(this);
        painter.save();
        painter.setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing);
        QPen pen = painter.pen();
        pen.setWidth(3);
        pen.setStyle(Qt::PenStyle::SolidLine);
        pen.setCapStyle(Qt::RoundCap);
        pen.setJoinStyle(Qt::MiterJoin);
        if (this->_themeMode == ElaThemeType::Light)
        {
            pen.setColor(QColor(200, 200, 200, 128));
        }
        else
        {
            pen.setColor(QColor(80, 80, 80, 128));
        }
        painter.setPen(pen);
        QColor brushcolor = ElaThemeColor(this->_themeMode, WindowBase);
        brushcolor.setAlpha(200);
        painter.setBrush(brushcolor);
        //painter.drawRect(rect());
        painter.drawRoundedRect(rect(), 4, 4);
        painter.restore();
    }
}

void DataViewerFlowIndicator::ThemeChanged(ElaThemeType::ThemeMode theme)
{
}

void DataViewerFlowIndicator::enterEvent(QEvent* event)
{
    QPoint GMouse = QCursor::pos();
    this->move(GMouse);
}

void DataViewerFlowIndicator::mouseMoveEvent(QMouseEvent* event)
{
    qDebug() << "moc";
    QPoint GMouse = QCursor::pos();
    this->move(GMouse);
}

void DataViewerFlowIndicator::leaveEvent(QEvent* event)
{
    this->hide();
}
