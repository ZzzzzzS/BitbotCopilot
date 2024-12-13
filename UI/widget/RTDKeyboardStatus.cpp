#include "RTDKeyboardStatus.h"
#include "QHBoxLayout"
#include <QFont>
#include <QTimer>
#include "ElaApplication.h"

RTDKeyboardStatus::RTDKeyboardStatus(QWidget *parent)
	: MetaRTDView(MetaRTDView::RTDViewType::SMALL_WINDOW,parent)
	, ui(new Ui::RTDKeyboardStatusClass())
{
	ui->setupUi(this);
    //QObject::connect(eTheme, &ElaTheme::themeModeChanged, this, &RTDKeyboardStatus::ScrollAreaThemeChangedSlot);
    this->ui->label->setPixmap(QPixmap(":/UI/Image/keyboard_icon.png"), 70, 70);
	this->CentralWidget = new QWidget();
	this->CentralWidget->setStyleSheet("background-color: rgba(255, 255, 255, 0);");
    eApp->syncMica(this->CentralWidget);
	this->ui->scrollArea->setWidget(this->CentralWidget);
    this->CentralLayout = new QVBoxLayout(this->CentralWidget);
    this->CentralLayout->setContentsMargins(0, 0, 1, 1);
    this->CentralLayout->setAlignment(Qt::AlignCenter);
    //this->CentralWidget->setAttribute(Qt::WA_TranslucentBackground);
    //this->ScrollAreaThemeChangedSlot(eTheme->getThemeMode());
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
        {
            background-color: rgba(255, 255, 255, 0); 
            background: rgba(255, 255, 255, 0);
        }
        )");
    }
    else
    {
       styleSheet = QString::fromUtf8(R"(
       {
            background-color: rgba(80, 80, 80, 0); 
            background: rgba(80, 80, 80, 0);
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

    QString Button_;
    if (Button == QString(" "))
        Button_ = QString("Space");
    else
        Button_ = Button;
    ButtonLabel->setText(Button_);
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
