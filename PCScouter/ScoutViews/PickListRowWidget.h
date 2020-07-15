#pragma once
#include <qwidget.h>

#include "PickListEntry.h"

namespace xero
{
	namespace scouting
	{
		namespace views
		{
			class PickListRowWidget : public QWidget
			{
				Q_OBJECT

			public:
				PickListRowWidget(const xero::scouting::datamodel::PickListEntry& entry, int rank, QWidget *parent = nullptr);
				virtual ~PickListRowWidget();

				void setRank(int rank) {
					rank_ = rank;
				}

			signals:
				void rowChanged();

			protected:
				void paintEvent(QPaintEvent* ev) override;
				void enterEvent(QEvent* ev) override;
				void leaveEvent(QEvent* ev) override;
				void mouseMoveEvent(QMouseEvent* ev) override;
				void mousePressEvent(QMouseEvent* ev) override;
				void dragEnterEvent(QDragEnterEvent* ev) override;
				void dragLeaveEvent(QDragLeaveEvent* ev) override;
				void dragMoveEvent(QDragMoveEvent* ev) override;
				void dropEvent(QDropEvent* ev) override;

			private:

				constexpr static const int LeftMargin = 4;
				constexpr static const int RightMargin = 4;
				constexpr static const int TopMargin = 2;
				constexpr static const int BottomMargin = 6;
				constexpr static const int NumberThirds = 16;
				constexpr static const int TeamAreaWidth = 96;
				constexpr static const int NumberWidth = 22;
				constexpr static const int ThirdAreaWidth = 48;
				constexpr static const int MinimumHeight = 36;
				constexpr static const int MaximumHeight = MinimumHeight;
				constexpr static const int MinimumWidth = LeftMargin + TeamAreaWidth + NumberThirds * ThirdAreaWidth + RightMargin;
				constexpr static const int MaximumWidth = MinimumWidth;

			private:
				void paintBackground(QPainter& paint, bool forceoff = false);
				void paintContent(QPainter& paint);
				void paintThirdArea(QPainter& paint, int index, bool highlight);
				void paintDropPos(QPainter& paint);

				int xposToThird(int x);
				void putDrag(int index);

			private:
				int rank_;
				xero::scouting::datamodel::PickListEntry entry_;
				bool current_;
				int third_;
				int droppos_;
				int dragging_index_;
				int dragging_team_;
				double dragging_score_;

				QColor background_not_selected_;
				QColor background_selected_;
				QColor background_third_;
			};
		}
	}
}
