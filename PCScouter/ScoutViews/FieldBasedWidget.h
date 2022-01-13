#pragma once

#include "GameFieldManager.h"
#include "PathFieldView.h"
#include <QtWidgets/QWidget>
#include <QtWidgets/QMessageBox>

namespace xero
{
	namespace scouting
	{
		namespace views
		{
			class FieldBasedWidget : public QWidget
			{
			public:
				FieldBasedWidget(GameFieldManager &mgr, QWidget* parent) : QWidget(parent), field_mgr_(mgr)
				{

				}

				virtual ~FieldBasedWidget()
				{
				}

			protected:
				void setField(PathFieldView* field, const QString &year) {
					field_view_ = field;

					std::shared_ptr<GameField> fld = nullptr;

					for (auto f : field_mgr_.getFields()) {
						QString name = f->getName().c_str();
						if (name.contains(year))
						{
							fld = f;
							break;
						}
					}

					if (fld != nullptr)
						field->setField(fld);
					else
					{
						QMessageBox::critical(this, "No Field", "There is no field for this target year " + year);
					}
				}

				PathFieldView* field() {
					return field_view_;
				}

			private:
				PathFieldView* field_view_;
				GameFieldManager& field_mgr_;
			};
		}
	}
}

