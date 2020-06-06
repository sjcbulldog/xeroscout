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
				tabs_ = new QTabWidget();
				QVBoxLayout* lay = new QVBoxLayout();
				setLayout(lay);
				lay->addWidget(tabs_);

				picks_win_ = new QTextBrowser(tabs_);
				tabs_->addTab(picks_win_, "PickList");

				robot_win_ = new QTextBrowser(tabs_);
				tabs_->addTab(robot_win_, "Robot Capabilities");
			}

			PickListView::~PickListView()
			{
			}

			void PickListView::clearView()
			{
				picks_win_->setHtml("");
				robot_win_->setHtml("");
			}

			void PickListView::refreshView()
			{
				picks_win_->setHtml(picks_);
				robot_win_->setHtml(robots_);
			}
		}
	}
}
