#pragma once

#include "ImageManager.h"
#include "ViewBase.h"
#include <QWidget>
#include <QImage>

namespace xero
{
	namespace scouting
	{
		namespace views
		{
			class ImageView : public QWidget, public ViewBase
			{
			public:
				ImageView(const QString& name, std::shared_ptr<QImage> image, QWidget* parent);
				virtual ~ImageView();

				virtual void clearView();
				virtual void refreshView();

				int number() const {
					return number_;
				}

				void setNumber(int num) {
					number_ = num;
				}

			protected:
				void paintEvent(QPaintEvent* ev) override;
				void resizeEvent(QResizeEvent* ev) override;
				void mousePressEvent(QMouseEvent* ev) override;
				void mouseMoveEvent(QMouseEvent* ev) override;
				void mouseReleaseEvent(QMouseEvent* ev) override;
				void mouseDoubleClickEvent(QMouseEvent* ev) override;

			private:
				struct line
				{
					QColor color_;
					int draw_size_;
					std::vector<QPoint> points_;
				};

			private:
				void computeMultiplier();
				void setColorCursor();

			private:
				double multiplier_;
				int number_;
				std::shared_ptr<QImage> image_;
				int draw_size_;
				size_t current_color_;
				bool drawing_;
				line current_line_;
				std::list<line> lines_;

				static std::vector<QColor> colors_;
			};
		}
	}
}
