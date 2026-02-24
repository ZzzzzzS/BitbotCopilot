#include "ClickableElaScrollPageArea.h"
#include "ElaTheme.h"
#include "QPainter"

ClickableElaScrollPageArea::ClickableElaScrollPageArea(QWidget* parent, int height)
    :ElaScrollPageArea(parent, height)
{
    setMouseTracking(true);
}

ClickableElaScrollPageArea::~ClickableElaScrollPageArea()
{

}

bool ClickableElaScrollPageArea::event(QEvent* event)
{
    switch (event->type())
    {
    case QEvent::Enter:
    case QEvent::Leave:
    {
        update();
        break;
    }
    default:
    {
        break;
    }
    }
    return QWidget::event(event);
}

void ClickableElaScrollPageArea::paintEvent(QPaintEvent* event)
{
    bool isUnderMouse = underMouse();
    auto _themeMode = eTheme->getThemeMode();
    QPainter painter(this);
    painter.save();
    painter.setPen(ElaThemeColor(_themeMode, BasicBorder));
    painter.setBrush(_isPressed ? ElaThemeColor(_themeMode, BasicSelectedAlpha) : isUnderMouse ? ElaThemeColor(_themeMode, BasicPressAlpha)
        : ElaThemeColor(_themeMode, BasicBaseAlpha));
    QRect foregroundRect(1, 1, width() - 2, height() - 2);
    painter.drawRoundedRect(foregroundRect, 6, 6);
    painter.restore();
}

void ClickableElaScrollPageArea::mouseMoveEvent(QMouseEvent* event)
{
    update();
    QWidget::mouseMoveEvent(event);
}

void ClickableElaScrollPageArea::mousePressEvent(QMouseEvent* event)
{
    QWidget* posWidget = childAt(event->pos());
    _isPressed = true;
    update();
    QWidget::mousePressEvent(event);
}

void ClickableElaScrollPageArea::mouseReleaseEvent(QMouseEvent* event)
{
    QWidget* posWidget = childAt(event->pos());
    _isPressed = false;
    update();
    emit this->clicked();
    QWidget::mouseReleaseEvent(event);
}
