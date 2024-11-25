#pragma once

#include <QLabel>
#include <QPixmap>
#include <QPainter>
#include <QPaintEvent>

class CustomImageLabel : public QLabel
{
	Q_OBJECT

public:
	CustomImageLabel(QWidget* parent = nullptr, Qt::WindowFlags f = Qt::WindowFlags())
		:QLabel(parent,f){}
	~CustomImageLabel(){}


	void setPixmap(const QPixmap& img = QPixmap(), size_t PixHeight = 0, size_t PixWidth = 0)
	{
		this->height__ = (PixHeight == 0) ? img.height() : PixHeight;
		this->width__ = (PixWidth == 0) ? img.width() : PixWidth;
		this->img__ = img;
		this->update();
	}


	QPixmap pixmap()
	{
		return this->img__;
	}


private:
	void paintEvent(QPaintEvent* e)
	{
		QLabel::paintEvent(e);
		QPainter painter(this);
		//painter.save();
		painter.setRenderHints(QPainter::SmoothPixmapTransform | QPainter::Antialiasing | QPainter::TextAntialiasing);
		
		QRect r = e->rect();
		
		QRect draw_rect;
		if (r.width() >= this->width__ && r.height() >= this->height__)
		{
			int start_x = r.x() + (r.width() - this->width__) / 2;
			int start_y = r.y() + (r.height() - this->height__) / 2;
			draw_rect.setX(start_x);
			draw_rect.setY(start_y);
			draw_rect.setWidth(this->width__);
			draw_rect.setHeight(this->height__);
		}
		else if (r.width() >= this->width__ && r.height() < this->height__)
		{
			double scale = static_cast<double>(r.height()) / static_cast<double>(this->height__);
			int new_width = this->width__ * scale;
			int new_height = r.height();
			int start_x = r.x() + (r.width() - new_width) / 2;
			int start_y = r.y();
			draw_rect.setX(start_x);
			draw_rect.setY(start_y);
			draw_rect.setWidth(new_width);
			draw_rect.setHeight(new_height);
		}
		else if (r.width() < this->width__ && r.height() >= this->height__)
		{
			double scale = static_cast<double>(r.width()) / static_cast<double>(this->width__);
			int new_height = this->height__ * scale;
			int new_width = r.width();
			int start_x = r.x();
			int start_y = r.y() + (r.height() - new_height) / 2;
			draw_rect.setX(start_x);
			draw_rect.setY(start_y);
			draw_rect.setWidth(new_width);
			draw_rect.setHeight(new_height);
		}
		else
		{
			double scale1 = static_cast<double>(r.height()) / static_cast<double>(this->height__);
			double scale2 = static_cast<double>(r.width()) / static_cast<double>(this->width__);
			double scale = (scale1 > scale2) ? scale2 : scale1;
			int new_height = this->height__ * scale;
			int new_width = this->width__ * scale;
			int start_x = r.x() + (r.width() - new_width) / 2;
			start_x = std::min(0, start_x);
			int start_y = r.y() + (r.height() - new_height) / 2;
			start_y = std::min(0, start_y);

			draw_rect.setX(start_x);
			draw_rect.setY(start_y);
			draw_rect.setWidth(new_width);
			draw_rect.setHeight(new_height);
		}


		painter.drawPixmap(draw_rect, this->img__);

		//painter.restore();
	}

private:
	QPixmap img__;
	size_t width__;
	size_t height__;
};
