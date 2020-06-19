#pragma once
#include "scoutviews_global.h"
#include "RobotActivity.h"
#include "FieldRegion.h"
#include <QWidget>
#include <QImage>
#include <set>

namespace xero
{
	namespace scouting
	{
		namespace views
		{
			class SCOUTVIEWS_EXPORT RobotActivityEditWidget : public QWidget
			{
				Q_OBJECT

			public:
				RobotActivityEditWidget(std::shared_ptr<QImage> enter, std::shared_ptr<QImage> exit, std::shared_ptr<QImage> idle, QWidget* parent);
				virtual ~RobotActivityEditWidget();

				void setActivities(std::vector<std::shared_ptr<const xero::scouting::datamodel::RobotActivity>> activities);
				void setRegions(std::vector<std::shared_ptr<const xero::scouting::datamodel::FieldRegion>> regions);

			signals:
				void addedActivity(std::shared_ptr<const xero::scouting::datamodel::RobotActivity> act);
				void deletedActivity(std::shared_ptr<const xero::scouting::datamodel::RobotActivity> act);
				void changedActivity(std::shared_ptr<const xero::scouting::datamodel::RobotActivity> act);
				void renamedActivity(const QString& oldname, const QString& newname);

			protected:
				void paintEvent(QPaintEvent* ev) override;
				void dragEnterEvent(QDragEnterEvent* ev) override;
				void dragMoveEvent(QDragMoveEvent* ev) override;
				void dragLeaveEvent(QDragLeaveEvent* ev) override;
				void dropEvent(QDropEvent* ev) override;
				void mousePressEvent(QMouseEvent* ev) override;
				void mouseDoubleClickEvent(QMouseEvent* ev) override;
				void keyPressEvent(QKeyEvent* ev) override;

			private:
				enum class SelectType
				{
					Activity,
					Pattern,
				};

				struct SelectedItem
				{
					SelectType type_;
					int activity_;
					int pattern_;
				};

			private:
				void paintTab(QPainter &painter, const QString &text, int xpos, int nitems);
				void paintNewActivity(QPainter& painter, int xpos);
				void paintActivity(QPainter& painter, int xpos, std::shared_ptr<const xero::scouting::datamodel::RobotActivity> act);
				void paintSelectedActivity(QPainter& painter, int col);
				void paintSelectedPattern(QPainter& painter, int col, int pattern);

				QRect columnBounds(int col);
				int columnAt(const QPoint& pt);
				int calcColumnWidth();
				void setSize();

				QString newActivityName();
				std::shared_ptr<const xero::scouting::datamodel::RobotActivity> findActivityByName(const QString& name);
				std::shared_ptr<const xero::scouting::datamodel::FieldRegion> findRegionByName(const QString& name);

				void createNewActivity(const QString& text);
				void addNewPattern(int col, const QString& text);
				bool isRegionPerAlliance(const QString& name);

				void renameActivity(int col);
				void updateMinMaxCount(int col, int pattern);

			private:
				int top_bottom_border_;
				int left_right_border_;
				int between_item_border_;
				int text_graphics_border_;
				int image_size_;
				int between_column_spacing_;
				int column_left_right_border_;
				int title_area_height_;
				int tab_text_spacing;
				int handle_size_;

				int column_width_;
				int highlight_column_;
				std::vector<SelectedItem> selected_;

				std::shared_ptr<QImage> idle_image_;
				std::shared_ptr<QImage> enter_image_;
				std::shared_ptr<QImage> exit_image_;

				std::vector<std::shared_ptr<xero::scouting::datamodel::RobotActivity>> activities_;
				std::vector<std::shared_ptr<const xero::scouting::datamodel::FieldRegion>> regions_;
			};
		}
	}
}

