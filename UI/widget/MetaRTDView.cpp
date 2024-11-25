#include "MetaRTDView.h"
#include <QDebug>
#include <QStyleOption>
#include <QPainter>
#include <ElaTheme.h>
#include <QGraphicsDropShadowEffect>
#include <QTimer>

MetaRTDView::MetaRTDView(RTDViewType type,QWidget *parent)
	: QWidget(parent),
    ViewType__(type)
{
    this->setObjectName(QString::fromUtf8("MetaBackground"));
    
    
    int WidthBase = 200;
    int HeightBase = 200;
    switch (this->ViewType__)
    {
    case MetaRTDView::RTDViewType::SMALL_WINDOW:
        this->setFixedSize(WidthBase, HeightBase);
        break;
    case MetaRTDView::RTDViewType::MIDDLE_WINDOW:
        this->setFixedSize(2 * WidthBase + 5, HeightBase);
        break;
    case MetaRTDView::RTDViewType::LARGE_WINDOW:
        this->setFixedSize(2 * WidthBase + 5, 2 * HeightBase + 5);
        break;
    case MetaRTDView::RTDViewType::EXTEND_WINDOW:
        //this->setFixedSize(5 * WidthBase + 20, 1 * HeightBase);
        break;
    default:
        this->setFixedSize(WidthBase, HeightBase);
        break;
    }
    
    this->ThemeChangedSlot(eTheme->getThemeMode());
    QObject::connect(eTheme, &ElaTheme::themeModeChanged, this, &MetaRTDView::ThemeChangedSlot);

    
    /*QGraphicsDropShadowEffect* effect = new QGraphicsDropShadowEffect(this);
    effect->setOffset(3);
    effect->setBlurRadius(20);
    effect->setColor(QColor("#444440"));*/
    //this->setGraphicsEffect(effect);
}

MetaRTDView::~MetaRTDView()
{}

void MetaRTDView::paintEvent(QPaintEvent * event)
{
    QStyleOption opt;
    opt.initFrom(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);

    
    p.setRenderHints(QPainter::SmoothPixmapTransform | QPainter::Antialiasing | QPainter::TextAntialiasing);
    // 高性能阴影
    eTheme->drawEffectShadow(&p, rect(), 3, 5);
    QWidget::paintEvent(event);
}

void MetaRTDView::ThemeChangedSlot(ElaThemeType::ThemeMode mode)
{
    QString styleSheet;
    if (mode == ElaThemeType::ThemeMode::Light)
    {
        styleSheet= QString::fromUtf8(R"(
       #MetaBackground {
            background-color: rgba(255, 255, 255, 100); 
            border-radius: 10px; 
        }
        )");
    }
    else
    {
        styleSheet = QString::fromUtf8(R"(
       #MetaBackground {
            background-color: rgba(80, 80, 80, 70); 
            border-radius: 10px; 
        }
        )");
    }
    this->setStyleSheet(styleSheet);
}

