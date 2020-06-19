#pragma once

#include "scoutviews_global.h"
#include "PatternListWidget.h"
#include "RobotActivityEditWidget.h"
#include "ViewBase.h"
#include "RobotActivity.h"
#include "ImageManager.h"
#include <QWidget>
#include <QSplitter>
#include <QListWidget>
#include <QScrollArea>

namespace xero
{
	namespace scouting
	{
		namespace views
		{
			class SCOUTVIEWS_EXPORT ZebraPatternView : public QWidget, public ViewBase
			{
			public:
				ZebraPatternView(xero::scouting::datamodel::ImageManager& images, QWidget* parent);
				virtual ~ZebraPatternView();

				virtual void clearView() {
				}

				virtual void refreshView() {
				}

				void madeActive() override; 

			private:
				void addedActivity(std::shared_ptr<const xero::scouting::datamodel::RobotActivity> act);
				void changedActivity(std::shared_ptr<const xero::scouting::datamodel::RobotActivity> act);
				void deletedActivity(std::shared_ptr<const xero::scouting::datamodel::RobotActivity> act);
				void renamedActivity(const QString& oldname, const QString& namename);

			private:
				QScrollArea* patterns_scroll_;
				PatternListWidget* patterns_;

				QScrollArea* activities_scroll_;
				RobotActivityEditWidget* activities_;
				xero::scouting::datamodel::ImageManager& images_;
			};
		}
	}
}
