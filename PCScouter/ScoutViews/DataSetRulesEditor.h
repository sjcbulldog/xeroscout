#pragma once

#include "DataSetHighlightRules.h"
#include "ScoutingDataSet.h"
#include <QDialog>
#include "ui_DataSetRulesEditor.h"
#include <list>
#include <memory>

namespace xero
{
	namespace scouting
	{
		namespace views
		{
			class DataSetRulesEditor : public QDialog
			{
				Q_OBJECT

			public:
				DataSetRulesEditor(const std::list<std::shared_ptr<const xero::scouting::datamodel::DataSetHighlightRules>> &rules, 
					xero::scouting::datamodel::ScoutingDataSet &ds, QWidget* parent = Q_NULLPTR);
				~DataSetRulesEditor();

				std::list<std::shared_ptr<xero::scouting::datamodel::DataSetHighlightRules>> rules() {
					std::list<std::shared_ptr<xero::scouting::datamodel::DataSetHighlightRules>> list;

					for (auto r : rules_)
						list.push_back(r);

					return list;
				}

			protected:
				bool eventFilter(QObject* obj, QEvent* ev);
				void keyPressEvent(QKeyEvent* ev);

			private:
				void newRule();
				void deleteRule();

				void foregroundColor();
				void backgroundColor();

				void initRule();
				void updateSample(QColor fore, QColor back);

				void descTextChanged(const QString& text);
				void equTextChanged(const QString& text);
				void selectChanged(QListWidgetItem* item);
				void highlightChanged(QListWidgetItem* item);
				void fieldDoubleClicked(QListWidgetItem* item);
				void highlightDoubleClicked(QListWidgetItem* item);
				void ruleNameChanged(QListWidgetItem* item);
				void ruleRowChanged(int row);

				void addField(QListWidget* list);

				void setCombinationsAndSizes();
				void setEquationError();

			private:
				constexpr static const char* addNewFieldText = "--- Double click to add new field ---";

			private:
				int current_;
				Ui::DataSetRulesEditor ui;
				std::vector<std::shared_ptr<xero::scouting::datamodel::DataSetHighlightRules>> rules_;
				xero::scouting::datamodel::ScoutingDataSet& data_;
			};
		}
	}
}
