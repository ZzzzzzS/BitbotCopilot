#pragma once
#include "ElaScrollPageArea.h"
#include "QMouseEvent"
#include "QPaintEvent"

class ClickableElaScrollPageArea : public ElaScrollPageArea
{
    Q_OBJECT
public:
    ClickableElaScrollPageArea(QWidget* parent);
    ~ClickableElaScrollPageArea();

signals:
    void clicked();

private:
    bool _isPressed = false;

protected:
    virtual bool event(QEvent* event) override;
    virtual void paintEvent(QPaintEvent* event) override;
    virtual void mouseMoveEvent(QMouseEvent* event) override;
    virtual void mousePressEvent(QMouseEvent* event) override;
    virtual void mouseReleaseEvent(QMouseEvent* event) override;
};