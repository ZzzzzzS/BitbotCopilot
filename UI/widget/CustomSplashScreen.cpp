#include "CustomSplashScreen.h"
#include "QPainter"
#include "QApplication"
#include "QScreen"
#include "QStyleOption"
#include "QStyle"
#include "QSvgRenderer"
#include "QDebug"
#include "QPainterPath"
#include "QGraphicsDropShadowEffect"
#include "ElaIcon.h"
#include "QTimer"
#include "QObject"

CustomSplashScreen::CustomSplashScreen(size_t waitTime, QWidget* parent)
    :QDialog(parent)
{
    this->setFixedSize(this->FixedWindowWidth + 2 * this->shadowBorderWidth, this->FixedWindowHeight + 2 * this->shadowBorderWidth);
    this->setWindowFlags(Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);
    this->setAttribute(Qt::WA_TranslucentBackground);
    this->moveToCenter();
    this->logo = new QPixmap(":/logo/Image/ProgramIcon.png");
    this->logo->setDevicePixelRatio(this->devicePixelRatio());
    this->OrgLogo = new QPixmap(":/UI/Image/logo.png");
    this->OrgLogo->setDevicePixelRatio(this->devicePixelRatio());

    this->CloseTimer = new QTimer(this);

    this->RefreshTimer = new QTimer(this);
    this->RefreshTimer->setInterval(30);
    this->LoadingAnimationCounter__ = this->BEGIN_CHARACTER_INDEX__;
    QObject::connect(this->RefreshTimer, &QTimer::timeout, this, [this]() {
        this->loadingIdx = (this->LoadingAnimationCounter__ > this->END_CHARACTER_INDEX__) ? this->END_CHARACTER_INDEX__ : this->LoadingAnimationCounter__;
        this->LoadingAnimationCounter__++;
        if (this->LoadingAnimationCounter__ > this->END_CHARACTER_INDEX__ + this->END_SLEEP_OFFSET)
            this->LoadingAnimationCounter__ = this->BEGIN_CHARACTER_INDEX__;
        this->update();
        });
    this->RefreshTimer->start();
    this->CloseTimer->singleShot(waitTime, this, &CustomSplashScreen::close);
    this->CloseTimer->singleShot(2 * waitTime, this, &CustomSplashScreen::close);
    this->CloseTimer->singleShot(3 * waitTime, this, &CustomSplashScreen::close);
}

CustomSplashScreen::~CustomSplashScreen()
{
}

void CustomSplashScreen::paintEvent(QPaintEvent* event)
{
    QPainter painter(this);

    painter.setRenderHints(QPainter::SmoothPixmapTransform | QPainter::Antialiasing | QPainter::TextAntialiasing);  // 反锯齿;

    painter.save();
    painter.setBrush(QBrush(QColor(255, 255, 255, 255)));
    painter.setPen(QPen(QBrush(QColor(128, 128, 128, 128)), 1, Qt::SolidLine, Qt::FlatCap, Qt::MiterJoin));
    painter.drawRoundedRect(QRect(this->shadowBorderWidth, this->shadowBorderWidth, this->FixedWindowWidth, this->FixedWindowHeight), 5, 5);
    painter.restore();

    painter.save();
    painter.setRenderHints(QPainter::Antialiasing);
    QPainterPath path;
    path.setFillRule(Qt::WindingFill);
    QColor color = QColor(0, 0, 0, 70);
    for (int i = 0; i < shadowBorderWidth; i++)
    {
        path.addRoundedRect(shadowBorderWidth - i, shadowBorderWidth - i, width() - (shadowBorderWidth - i) * 2, height() - (shadowBorderWidth - i) * 2, 5 + i, 5 + i);
        int alpha = 1 * (shadowBorderWidth - i + 1);
        color.setAlpha(alpha > 255 ? 255 : alpha);
        painter.setPen(color);
        painter.drawPath(path);
    }
    painter.restore();

    painter.save();
    painter.setBrush(QBrush(QColor(208, 237, 219, 255)));
    painter.setPen(Qt::NoPen);
    painter.drawRoundedRect(9 + this->shadowBorderWidth, 9 + this->shadowBorderWidth, 90, 24, 3, 3);
    painter.restore();

    painter.save();
    painter.setFont(QFont("Segoe UI", 8, QFont::Bold));
    painter.setPen(QPen(QBrush(QColor(50, 107, 73, 255)), 1, Qt::SolidLine, Qt::FlatCap, Qt::MiterJoin));
    painter.drawText(9, 9, 90, 23, Qt::AlignCenter, "Bitbot Copilot");
    painter.restore();

    painter.save();
    painter.setFont(QFont("Segoe UI", 8, QFont::Weight::DemiBold));
    painter.setPen(QPen(QBrush(QColor(165, 165, 165, 255)), 1, Qt::SolidLine, Qt::FlatCap, Qt::MiterJoin));
    painter.drawText(10, 222, 129, 15, Qt::AlignCenter, tr("Starting Bitbot Copilot..."));
    painter.restore();

    painter.save();
    painter.setFont(QFont("Segoe Boot Semilight", 8));
    painter.setPen(QPen(QBrush(QColor(127, 127, 127, 255)), 1, Qt::SolidLine, Qt::RoundCap, Qt::MiterJoin));
    QString TextLoading = QChar(this->loadingIdx);
    painter.drawText(350, 205, 40, 40, Qt::AlignCenter, TextLoading);
    painter.restore();

    painter.save();
    painter.drawPixmap((this->FixedWindowWidth - 96) / 2, (this->FixedWindowHeight - 96) / 2 - 15, 96, 96, *this->logo);
    painter.restore();

    painter.save();
    painter.setFont(QFont("Segoe UI", 8, QFont::Weight::Bold));
    painter.setPen(QPen(QBrush(QColor(127, 127, 127, 255)), 1, Qt::SolidLine, Qt::FlatCap, Qt::MiterJoin));
    painter.drawPixmap((this->FixedWindowWidth - 100) / 2 + 4, (this->FixedWindowHeight - 15) / 2 + 50, 16, 16, *this->OrgLogo);
    painter.drawText((this->FixedWindowWidth - 100) / 2 + 8, (this->FixedWindowHeight - 15) / 2 + 50, 100, 15, Qt::AlignCenter, "BHR Group");
    painter.restore();

    painter.save();
    painter.setFont(QFont("ElaAwesome", 8, QFont::Weight::Normal));
    painter.setPen(QPen(QBrush(QColor(127, 127, 127, 255)), 1, Qt::SolidLine, Qt::RoundCap, Qt::MiterJoin));
    QString Text = QChar(ElaIconType::WindowMinimize);
    painter.drawText(326, 6, 20, 17, Qt::AlignCenter, Text);
    QString Text1 = QChar(ElaIconType::XmarkLarge);
    painter.drawText(358, 9, 20, 20, Qt::AlignCenter, Text1);
    painter.restore();

    QWidget::paintEvent(event);
}

void CustomSplashScreen::moveToCenter()
{
    if (isMaximized() || isFullScreen())
    {
        return;
    }
#if QT_VERSION >= QT_VERSION_CHECK(5, 14, 0)
    auto geometry = screen()->availableGeometry();
#else
    auto geometry = qApp->screenAt(this->geometry().center())->geometry();
#endif
    setGeometry((geometry.left() + geometry.right() - width()) / 2, (geometry.top() + geometry.bottom() - height()) / 2, width(), height());
}