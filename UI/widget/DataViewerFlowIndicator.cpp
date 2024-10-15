#include "DataViewerFlowIndicator.h"
#include "ElaTheme.h"
#include "ElaApplication.h"
#include "QPainter"
#include "QLabel"
#include "ElaText.h"
#include "QHBoxLayout"
#include "QVBoxLayout"
#include "QDebug"

DataViewerFlowIndicator::DataViewerFlowIndicator(QWidget* Parent)
	:QWidget(Parent)
{
	this->setWindowFlag(Qt::ToolTip);
    this->VLay__ = new QVBoxLayout(this);

    this->_themeMode = eTheme->getThemeMode();
    connect(eTheme, &ElaTheme::themeModeChanged, this, [=](ElaThemeType::ThemeMode themeMode) {
        this->_themeMode = themeMode;
        this->ThemeChanged(themeMode);
        update();
        });
    
    this->_isEnableMica = eApp->getIsEnableMica();
    connect(eApp, &ElaApplication::pIsEnableMicaChanged, this, [=]() {
        this->_isEnableMica = eApp->getIsEnableMica();
        update();
        });
    eApp->syncMica(this, this->_isEnableMica);
    this->setBaseSize(0, 0);
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
        QString val = QString::number(value[i], 'g', 2);
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
        Icon->setFixedSize(5, 5);

        ElaText* Label = new ElaText(this);
        QFont font = Label->font();
        font.setPointSize(10);
        Label->setFont(font);
        Label->setText(text[i]);
        
        ElaText* Value = new ElaText(this);
        Value->setFont(font);
        Label->setText("0.0");

        QHBoxLayout* hlay = new QHBoxLayout(this);
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

void DataViewerFlowIndicator::paintEvent(QPaintEvent* event)
{
    QWidget::paintEvent(event);
    if (!this->_isEnableMica)
    {
        QPainter painter(this);
        painter.save();
        painter.setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing);
        painter.setPen(Qt::NoPen);
        painter.setBrush(ElaThemeColor(this->_themeMode, WindowBase));
        painter.drawRect(rect());
        painter.drawRoundedRect(rect(), 15, 15);
        painter.restore();
    }
    else
    {
        QPainter painter(this);
        painter.save();
        painter.setRenderHint(QPainter::Antialiasing); 
        //painter.setBrush(QBrush(QColor(255, 255, 255))); 
        //painter.setPen(Qt::NoPen); 
        QPen pen = painter.pen();
        pen.setColor(ElaThemeColor(this->_themeMode, WindowBase));
        painter.setPen(pen);
        QRect rect = this->rect(); 
        painter.drawRoundedRect(rect, 10, 10); 
        painter.restore();
    }
    //QWidget::paintEvent(event);
}

void DataViewerFlowIndicator::ThemeChanged(ElaThemeType::ThemeMode theme)
{
    for (size_t i = 0; i < this->ColorPairList__.size(); i++)
    {
        QPalette p;
        QColor color = this->ColorPairList__[i];
        p.setColor(QPalette::Window, color);
        this->CurveIconList__[i]->setPalette(p);
    }
}
