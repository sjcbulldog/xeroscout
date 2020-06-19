#pragma once
#include "scoutviews_global.h"
#include "FieldRegion.h"
#include <QWidget>
#include <QImage>
#include <vector>
#include <memory>

namespace xero
{
	namespace scouting
	{
		namespace views
		{
			class SCOUTVIEWS_EXPORT PatternListWidget : public QWidget
			{
			public:
				PatternListWidget(std::shared_ptr<QImage> enter, std::shared_ptr<QImage> exit, std::shared_ptr<QImage> idle, QWidget *parent);
				virtual ~PatternListWidget();

				void setRegions(std::vector<std::shared_ptr<const xero::scouting::datamodel::FieldRegion>>& regions);

			protected:
				void paintEvent(QPaintEvent* ev) override;
				void mousePressEvent(QMouseEvent* ev) override;
				void mouseMoveEvent(QMouseEvent* ev) override;
				void mouseReleaseEvent(QMouseEvent* ev) override;

			private:
				QRect bounds(int item);
				int itemAt(const QPoint& pos);
				QString itemToText(int item);

				void startDrag(int item);
				void paintOne(QPainter &painter, const QPoint &loc, std::shared_ptr<QImage> left, const QString& text, std::shared_ptr<QImage> right, bool selected);

			private:
				int top_bottom_border_;
				int left_right_border_;
				int between_item_border_;
				int text_graphics_border_;
				int image_size_;

				std::vector<std::shared_ptr<const xero::scouting::datamodel::FieldRegion>> regions_;

				std::shared_ptr<QImage> idle_image_;
				std::shared_ptr<QImage> enter_image_;
				std::shared_ptr<QImage> exit_image_;

				QSize size_;
				int selected_;

				QStringList region_list_;

				bool mouse_down_;
				QPoint mouse_down_pos_;
			};
		}
	}
}

