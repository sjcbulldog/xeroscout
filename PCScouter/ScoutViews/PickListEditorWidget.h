#pragma once

#include "PickListRowWidget.h"
#include <QWidget>
#include <set>

namespace xero
{
	namespace scouting
	{
		namespace views
		{
			class PickListEditorWidget : public QWidget
			{
				Q_OBJECT

			public:
				PickListEditorWidget(QWidget* parent = Q_NULLPTR);
				~PickListEditorWidget();

				void setPickList(const std::vector<xero::scouting::datamodel::PickListEntry>& picklist) {
					picklist_ = picklist;
					recreateChildren();
				}

				void clearPickList() {
					picklist_.clear();
					recreateChildren();
				}

				const std::vector<xero::scouting::datamodel::PickListEntry>& picklist() const {
					return picklist_;
				}

				void teamSelected(int team, bool selected);

			signals:
				void picklistChanged();

			protected:
				void resizeEvent(QResizeEvent* ev) override ;
				void paintEvent(QPaintEvent* ev) override;
				void dragEnterEvent(QDragEnterEvent* ev) override;
				void dragLeaveEvent(QDragLeaveEvent* ev) override;
				void dragMoveEvent(QDragMoveEvent* ev) override;
				void dropEvent(QDropEvent* ev) override;

			private:
				void doLayout();
				void recreateChildren();

				void rowChanged(int row);
				void draggingRow(QPoint pos, int row);
				void putDrag(int row);

			private:
				std::vector<PickListRowWidget*> rows_;
				std::vector<xero::scouting::datamodel::PickListEntry> picklist_;
				int left_margin_;
				int right_margin_;
				int top_margin_;
				int bottom_margin_;
				int between_gap_;
				int droppos_;
				std::set<int> selected_;

				bool diddrop_;
				int dragging_row_;
				xero::scouting::datamodel::PickListEntry dragging_entry_;
			};
		}
	}
}
