#include "DataModelTeam.h"
#include "ZebraAnalysisView.h"
#include "ZebraSequence.h"
#include <QBoxLayout>

using namespace xero::scouting::datamodel;

namespace xero
{
	namespace scouting
	{
		namespace views
		{
			ZebraAnalysisView::ZebraAnalysisView(QWidget *parent) : QWidget(parent), ViewBase("ZebraAnalysis")
			{
				QBoxLayout* lay = new QVBoxLayout();
				setLayout(lay);

				QWidget* top_form = new QWidget();
				lay->addWidget(top_form);

				QBoxLayout* formlay = new QHBoxLayout();
				top_form->setLayout(formlay);

				box_ = new QComboBox(top_form);
				formlay->addWidget(box_);
				(void)connect(box_, static_cast<void (QComboBox::*)(int index)>(&QComboBox::currentIndexChanged), this, &ZebraAnalysisView::teamChanged);

				report_ = new QGroupBox("Report", this);
				lay->addWidget(report_);

				lay = new QVBoxLayout();
				report_->setLayout(lay);
				report_txt_ = new QTextBrowser(report_);
				lay->addWidget(report_txt_);

				report_txt_->clear();
				report_txt_->append("No Data");

				report_txt_->setReadOnly(true);
				report_txt_->setOpenLinks(false);
				report_txt_->setOpenExternalLinks(false);
			}

			ZebraAnalysisView::~ZebraAnalysisView()
			{
			}

			void ZebraAnalysisView::showEvent(QShowEvent* ev)
			{
				if (box_->count() == 0)
				{
					auto dm = dataModel();
					if (dm != nullptr) {
						std::list<std::shared_ptr<const DataModelTeam>> teams = dm->teams();
						teams.sort([](std::shared_ptr<const DataModelTeam> a, std::shared_ptr<const DataModelTeam> b) -> bool
							{
								return a->number() < b->number();
							});

						for (auto t : teams)
							box_->addItem(QString::number(t->number()) + " - " + t->nick(), t->key());
					}
				}
			}

			void ZebraAnalysisView::teamChanged(int index)
			{
				regenerate();
			}

			void ZebraAnalysisView::refreshView()
			{
				regenerate();
			}

			void ZebraAnalysisView::clearView()
			{
				report_txt_->clear();
			}

			void ZebraAnalysisView::regenerate()
			{
				sequences_.clear();

				QString tkey = box_->currentData().toString();
				for (auto m : dataModel()->matches())
				{
					if (!m->hasZebra())
						continue;

					Alliance c;
					int slot;

					if (m->teamToAllianceSlot(tkey, c, slot))
					{
					}
				}
			}
		}
	}
}