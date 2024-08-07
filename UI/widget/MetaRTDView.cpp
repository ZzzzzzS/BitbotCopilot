#include "MetaRTDView.h"
#include <QDebug>
#include <QStyleOption>
#include <QPainter>
#include <ElaTheme.h>

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
        this->setFixedSize(2 * WidthBase, HeightBase);
        break;
    case MetaRTDView::RTDViewType::LARGE_WINDOW:
        this->setFixedSize(2 * WidthBase, 2 * HeightBase);
        break;
    case MetaRTDView::RTDViewType::EXTEND_WINDOW:
        break;
    default:
        this->setFixedSize(WidthBase, HeightBase);
        break;
    }
    
    this->ThemeChangedSlot(eTheme->getThemeMode());
    QObject::connect(eTheme, &ElaTheme::themeModeChanged, this, &MetaRTDView::ThemeChangedSlot);
    
}

MetaRTDView::~MetaRTDView()
{}

void MetaRTDView::paintEvent(QPaintEvent * event)
{
    QStyleOption opt;
    opt.init(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}

void MetaRTDView::ThemeChangedSlot(ElaThemeType::ThemeMode mode)
{
    QString styleSheet;
    if (mode == ElaThemeType::ThemeMode::Light)
    {
        styleSheet= QString::fromUtf8(R"(
       #MetaBackground {
            background-color: rgba(255, 255, 255, 70); 
            border-radius: 10px; 
             border: 2px solid rgb(230,230,230);
        }
        )");
    }
    else
    {
        styleSheet = QString::fromUtf8(R"(
       #MetaBackground {
            background-color: rgba(80, 80, 80, 70); 
            border-radius: 10px; 
             border: 2px solid rgb(23,23,23);
        }
        )");
    }

    this->setStyleSheet(styleSheet);
}

