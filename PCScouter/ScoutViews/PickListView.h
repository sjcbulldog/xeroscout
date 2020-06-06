#pragma once

#include "ViewBase.h"
#include <QWidget>
#include <QTextBrowser>
#include <QTabWidget>

namespace xero
{
	namespace scouting
	{
		namespace views
		{
			class PickListView : public QWidget, public ViewBase
			{
			public:
				PickListView(const QString& name, QWidget* parent);
				virtual ~PickListView();

				void setHTML(const QString& picks, const QString &robots) {
					picks_ = picks;
					picks_win_->setHtml(picks_);

					robots_ = robots;
					robot_win_->setHtml(robots_);
				}

				virtual void clearView() ;
				virtual void refreshView() ;

			private:
				QTabWidget* tabs_;
				QTextBrowser* picks_win_;
				QTextBrowser* robot_win_;
				QString picks_;
				QString robots_;
			};
		}
	}
}
