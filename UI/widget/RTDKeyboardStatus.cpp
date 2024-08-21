#include "RTDKeyboardStatus.h"
#include "QHBoxLayout"
#include <QFont>
#include <QTimer>

RTDKeyboardStatus::RTDKeyboardStatus(QWidget *parent)
	: MetaRTDView(MetaRTDView::RTDViewType::SMALL_WINDOW,parent)
	, ui(new Ui::RTDKeyboardStatusClass())
{
	ui->setupUi(this);
    this->ScrollAreaThemeChangedSlot(eTheme->getThemeMode());
    QObject::connect(eTheme, &ElaTheme::themeModeChanged, this, &RTDKeyboardStatus::ScrollAreaThemeChangedSlot);

    this->CentralWidget = this->ui->scrollAreaWidgetContents;
    this->CentralLayout = new QVBoxLayout(this->CentralWidget);
    this->CentralLayout->setContentsMargins(0, 0, 1, 1);
    this->CentralLayout->setAlignment(Qt::AlignCenter);
}

RTDKeyboardStatus::~RTDKeyboardStatus()
{
	delete ui;
}

void RTDKeyboardStatus::ResetUI()
{
    for (auto i : this->ActionHistoryWidgets)
    {
        this->CentralLayout->removeWidget(i);
        delete i;
    }
    this->ActionHistoryWidgets.clear();
}

void RTDKeyboardStatus::ButtonClicked(QString Button, bool Press)
{
    ElaScrollPageArea* new_area = this->AppendAction(Button, Press);
    this->ActionHistoryWidgets.push_back(new_area);
    //this->CentralLayout->addWidget(new_area);
    this->CentralLayout->insertWidget(0, new_area);
    this->ui->scrollArea->ensureWidgetVisible(new_area);
}

void RTDKeyboardStatus::ScrollAreaThemeChangedSlot(ElaThemeType::ThemeMode mode)
{
    QString styleSheet;
    if (mode == ElaThemeType::ThemeMode::Light)
    {
       styleSheet = QString::fromUtf8(R"(
       #scrollAreaWidgetContents {
            background-color: rgba(255, 255, 255, 70); 
        },
        #scrollArea {
            background-color: rgba(255, 255, 255, 70); 
        }
        )");
    }
    else
    {
       styleSheet = QString::fromUtf8(R"(
       #scrollAreaWidgetContents {
            background-color: rgba(80, 80, 80, 70); 
        },
        #scrollArea {
            background-color: rgba(80, 80, 80, 70); 
        }
        )");
    } 

    this->ui->scrollAreaWidgetContents->setStyleSheet(styleSheet);
    this->ui->scrollArea->setStyleSheet(styleSheet);
}

ElaScrollPageArea* RTDKeyboardStatus::AppendAction(const QString& Button, bool Press)
{
    ElaScrollPageArea* NewActionWidget = new ElaScrollPageArea(this->CentralWidget);
    NewActionWidget->setFixedHeight(30);
    NewActionWidget->setFixedWidth(160);

    QFont font;
    font.setPointSize(7);

    ElaText* ButtonLabel = new ElaText(NewActionWidget);
    ButtonLabel->setAlignment(Qt::AlignVCenter | Qt::AlignLeft);
    ButtonLabel->setText(Button);
    ButtonLabel->setSizePolicy(QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Preferred);
    ButtonLabel->setFont(font);

    ElaText* ValueLabel = new ElaText(NewActionWidget);
    ValueLabel->setAlignment(Qt::AlignVCenter | Qt::AlignRight);
    ValueLabel->setText(Press ? QString(tr(" Pressed")) : QString(tr(" Released")));
    ValueLabel->setSizePolicy(QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Preferred);
    ValueLabel->setFont(font);
    
    QHBoxLayout* Layout = new QHBoxLayout(NewActionWidget);
    Layout->addWidget(ButtonLabel);
    Layout->addWidget(ValueLabel);

    return NewActionWidget;
}
