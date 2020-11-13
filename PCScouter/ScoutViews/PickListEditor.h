#pragma once

#include "PickListEditorWidget.h"
#include "TeamSelectedWidget.h"
#include "ViewBase.h"
#include <QWidget>
#include <QScrollArea>

namespace xero
{
	namespace scouting
	{
		namespace views
		{
			class PickListEditor : public QWidget, public ViewBase
			{
				Q_OBJECT 

			public:
				PickListEditor(QWidget* parent, const QString& name);
				virtual ~PickListEditor();

				virtual void clearView();
				virtual void refreshView();

			private:
				void picklistDataChanged();
				void teamChanged(int team, bool selected);

			private:
				QScrollArea* picklist_area_;
				QScrollArea* teamlist_area_;
				TeamSelectedWidget* teams_;
				PickListEditorWidget* editor_;
			};
		}
	}
}

