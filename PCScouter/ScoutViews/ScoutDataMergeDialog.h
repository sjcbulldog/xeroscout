#pragma once
#include "scoutviews_global.h"
#include "ScoutingDataMap.h"
#include "ScoutingForm.h"
#include <QDialog>
#include <QTableWidget>
#include <QDialogButtonBox>

namespace xero
{
	namespace scouting
	{
		namespace views
		{
			class SCOUTVIEWS_EXPORT ScoutDataMergeDialog : public QDialog
			{
			public:
				ScoutDataMergeDialog(std::shared_ptr<const xero::scouting::datamodel::ScoutingForm> form, const std::list<xero::scouting::datamodel::ConstScoutingDataMapPtr>& list);
				virtual ~ScoutDataMergeDialog();

				xero::scouting::datamodel::ScoutingDataMapPtr result() {
					return result_;
				}

				void accept() override;

			private:
				void populate();

			private:
				QTableWidget* table_;
				QDialogButtonBox* box_;
				std::list<xero::scouting::datamodel::ConstScoutingDataMapPtr> list_;
				xero::scouting::datamodel::ScoutingDataMapPtr result_;
				std::shared_ptr<const xero::scouting::datamodel::ScoutingForm> form_;
			};
		}
	}
}


