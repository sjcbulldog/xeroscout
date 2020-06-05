#include "PickListView.h"
#include <QBoxLayout>

namespace xero
{
	namespace scouting
	{
		namespace views
		{
			PickListView::PickListView(const QString& name, QWidget* parent) : QWidget(parent), ViewBase(name)
			{
				QVBoxLayout* lay = new QVBoxLayout();
				setLayout(lay);

				browser_ = new QTextBrowser(this);
				lay->addWidget(browser_);
			}

			PickListView::~PickListView()
			{
			}

			void PickListView::clearView()
			{
				browser_->setHtml("");
			}

			void PickListView::refreshView()
			{
				browser_->setHtml(html_);
			}
		}
	}
}
