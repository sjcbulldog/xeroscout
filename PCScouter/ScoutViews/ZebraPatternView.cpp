#include "ZebraPatternView.h"
#include "DocumentView.h"
#include "SequenceEnterExitPattern.h"
#include "SequenceEnterIdlePattern.h"
#include <QBoxLayout>
#include <QHeaderView>
#include <QInputDialog>
#include <QMessageBox>
#include <QScrollBar>

using namespace xero::scouting::datamodel;

namespace xero
{
	namespace scouting
	{
		namespace views
		{
			ZebraPatternView::ZebraPatternView(xero::scouting::datamodel::ImageManager& images, QWidget* parent) : QWidget(parent), ViewBase("ZebraPatternEditor"), images_(images)
			{
				QBoxLayout* lay = new QHBoxLayout();
				setLayout(lay);

				auto idle = images_.get("idle.png");
				auto enter = images_.get("enter.png");
				auto exit = images_.get("exit.png");

				patterns_ = new PatternListWidget(enter, exit, idle, this);
				patterns_->setSizePolicy(QSizePolicy::Policy::Fixed, QSizePolicy::Policy::Fixed);

				patterns_scroll_ = new QScrollArea(this);
				lay->addWidget(patterns_scroll_);
				patterns_scroll_->setSizePolicy(QSizePolicy::Policy::MinimumExpanding, QSizePolicy::Policy::Expanding);
				patterns_scroll_->setWidget(patterns_);
				patterns_scroll_->setHorizontalScrollBarPolicy(Qt::ScrollBarPolicy::ScrollBarAlwaysOff);
				patterns_scroll_->setHorizontalScrollBarPolicy(Qt::ScrollBarPolicy::ScrollBarAlwaysOn);

				activities_ = new RobotActivityEditWidget(enter, exit, idle, this);
				activities_->setSizePolicy(QSizePolicy::Policy::Fixed, QSizePolicy::Policy::Fixed);

				activities_scroll_ = new QScrollArea(this);
				lay->addWidget(activities_scroll_);
				patterns_scroll_->setSizePolicy(QSizePolicy::Policy::Minimum, QSizePolicy::Policy::Expanding);
				activities_scroll_->setWidget(activities_);
				activities_scroll_->setHorizontalScrollBarPolicy(Qt::ScrollBarPolicy::ScrollBarAlwaysOn);
				activities_scroll_->setVerticalScrollBarPolicy(Qt::ScrollBarPolicy::ScrollBarAlwaysOn);

				lay->addWidget(activities_scroll_);
				
				connect(activities_, &RobotActivityEditWidget::addedActivity, this, &ZebraPatternView::addedActivity);
				connect(activities_, &RobotActivityEditWidget::deletedActivity, this, &ZebraPatternView::deletedActivity);
				connect(activities_, &RobotActivityEditWidget::changedActivity, this, &ZebraPatternView::changedActivity);
				connect(activities_, &RobotActivityEditWidget::renamedActivity, this, &ZebraPatternView::renamedActivity);
			}

			ZebraPatternView::~ZebraPatternView()
			{
			}

			void ZebraPatternView::renamedActivity(const QString& oldname, const QString& newname)
			{
				dataModel()->renameActivity(oldname, newname);
			}

			void ZebraPatternView::addedActivity(std::shared_ptr<const RobotActivity> act)
			{
				dataModel()->addActivity(act);
			}

			void ZebraPatternView::deletedActivity(std::shared_ptr<const RobotActivity> act)
			{
				dataModel()->removeActivty(act);
			}

			void ZebraPatternView::changedActivity(std::shared_ptr<const RobotActivity> act)
			{
				dataModel()->updatedActivity(act);
			}

			void ZebraPatternView::madeActive()
			{
				auto regions = dataModel()->fieldRegions();
				patterns_->setRegions(regions);
				int width = patterns_->minimumWidth() + 2 * patterns_scroll_->frameWidth() + patterns_scroll_->verticalScrollBar()->sizeHint().width();
				patterns_scroll_->setMinimumWidth(width);

				activities_->setRegions(dataModel()->fieldRegions());
				activities_->setActivities(dataModel()->activities());
			}
		}
	}
}
