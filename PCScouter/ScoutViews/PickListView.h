#pragma once

#include "ViewBase.h"
#include <QWidget>
#include <QTextBrowser>

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

				void updateHtml(const QString& html) {
					html_ = html;
					browser_->setHtml(html);
				}

				virtual void clearView() ;
				virtual void refreshView() ;

			private:
				QTextBrowser* browser_;
				QString html_;
			};
		}
	}
}
