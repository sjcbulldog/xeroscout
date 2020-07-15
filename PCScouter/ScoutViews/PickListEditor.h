#pragma once

#include "PickListEditorWidget.h"
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

			private:
				QScrollArea* area_;
				PickListEditorWidget* editor_;
			};
		}
	}
}

