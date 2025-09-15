#include "SettingsLineInfoWidget.h"
#include "QHBoxLayout"
#include "QVBoxLayout"
#include "QFont"

SettingsLineInfoWidget::SettingsLineInfoWidget(QWidget* parent, const QString& title, const QString& subtitle, const QChar& Icon)
    :QWidget(parent)
{
    auto horizontalLayout_3 = new QHBoxLayout(this);
    //horizontalLayout_3->setSpacing(6);
    //horizontalLayout_3->setObjectName(QString::fromUtf8("horizontalLayout_3"));

    this->icon__ = new ElaText(this);
    QFont iconFont = this->icon__->font();
    iconFont.setFamily("ElaAwesome");
    iconFont.setPixelSize(28);
    this->icon__->setFixedSize(40, 40);
    this->icon__->setText(Icon);
    this->icon__->setFont(iconFont);
    this->icon__->setAlignment(Qt::AlignCenter);
    //icon->setPixmap(QPixmap(":/UI/Image/frontend_icon.png"));
    //icon->setScaledContents(true);
    this->title__ = new ElaText(this);
    this->title__->setText(title);
    QFont titlefont;
    titlefont.setPixelSize(16);
    titlefont.setWeight(QFont::Weight::Normal);
    this->title__->setFont(titlefont);
    if (subtitle.isEmpty())
        this->title__->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    else
        this->title__->setAlignment(Qt::AlignLeft | Qt::AlignBottom);

    this->subtitle__ = new ElaText(this);
    this->subtitle__->setAlignment(Qt::AlignLeft | Qt::AlignTop);
    this->subtitle__->setWordWrap(false);
    this->subtitle__->setSizePolicy(QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Preferred);
    this->subtitle__->setText(subtitle);
    QFont subtitlefont;
    subtitlefont.setPixelSize(10);
    this->subtitle__->setFont(subtitlefont);

    auto LamsetTheme = [this](ElaThemeType::ThemeMode Mode) {
        QPalette LabelPalette;
        if (Mode == ElaThemeType::ThemeMode::Dark)
        {
            QBrush brush_subLabel(QColor(206, 206, 206, 255));
            brush_subLabel.setStyle(Qt::SolidPattern);
            LabelPalette.setBrush(QPalette::Active, QPalette::WindowText, brush_subLabel);
            LabelPalette.setBrush(QPalette::Inactive, QPalette::WindowText, brush_subLabel);
        }
        else
        {
            QBrush brush_subLabel(QColor(55, 55, 55, 255));
            brush_subLabel.setStyle(Qt::SolidPattern);
            LabelPalette.setBrush(QPalette::Active, QPalette::WindowText, brush_subLabel);
            LabelPalette.setBrush(QPalette::Inactive, QPalette::WindowText, brush_subLabel);
        }
        this->subtitle__->setPalette(LabelPalette);
        };
    LamsetTheme(eTheme->getThemeMode());
    QObject::connect(eTheme, &ElaTheme::themeModeChanged, this, LamsetTheme);

    QVBoxLayout* TitleLayout = new QVBoxLayout();
    TitleLayout->setContentsMargins(1, 1, 1, 1);
    TitleLayout->setSpacing(1);
    TitleLayout->addWidget(this->title__);
    if (!subtitle.isEmpty())
        TitleLayout->addWidget(this->subtitle__);

    horizontalLayout_3->addSpacerItem(new QSpacerItem(10, 10, QSizePolicy::Fixed, QSizePolicy::Minimum));
    horizontalLayout_3->addWidget(this->icon__);
    horizontalLayout_3->addSpacerItem(new QSpacerItem(10, 10, QSizePolicy::Fixed, QSizePolicy::Minimum));
    horizontalLayout_3->addLayout(TitleLayout);
    horizontalLayout_3->addSpacerItem(new QSpacerItem(10, 10, QSizePolicy::Fixed, QSizePolicy::Minimum));
}

SettingsLineInfoWidget::~SettingsLineInfoWidget()
{

}

void SettingsLineInfoWidget::setTitleFontSize(int size)
{
    this->title__->setTextPixelSize(size);
}
