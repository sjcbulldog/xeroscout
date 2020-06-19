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
			class IntroView : public QWidget, public ViewBase
			{
			public:
				IntroView(const QString& name, QWidget* widget);
				virtual ~IntroView();

				void clearView() override {

				}

				void refreshView() override {

				}

				void setFile(const QString& filename);
				void setHtml(const QString& html) {
					browser_->setHtml(html);
				}

			private:
				QTextBrowser* browser_;
			};
		}
	}
}

