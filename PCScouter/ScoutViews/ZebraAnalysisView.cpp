#include "DataModelTeam.h"
#include "ZebraAnalysisView.h"
#include "ZebraSequence.h"
#include "DataModelBuilder.h"
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
				report_txt_->clear();
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
						auto track = DataModelBuilder::createTrack(dataModel(), m->key(), tkey);
						auto seq = std::make_shared<ZebraSequence>(m->key(), tkey, track, dataModel()->fieldRegions());
						sequences_.push_back(seq);
					}
				}

				analyzer_.setSequences(sequences_);
				analyzer_.analyze();
				QString html;

				printSequences(html);
				html += "<hr><br><br>";
				// printRawData(html);

				report_txt_->setHtml(html);
			}

			void ZebraAnalysisView::printSequences(QString &html)
			{
				QString last;
				bool first = true;

				html = "<center><table border=\"1\">";

				html += "<tr>";
				html += "<th>Match</th>";
				html += "<th>Pattern</th>";
				html += "<th>Start Event</th>";
				html += "<th>End Event</th>";
				html += "<th>Duration</th>";
				html += "</tr>";

				for (auto& match : analyzer_.matches())
				{
					html += "<tr>";
					double duration = match.endTime() - match.startTime();

					html += "<td>" + match.sequence()->matchKey() + "</td>";
					html += "<td>" + match.name() + "</td>";
					html += "<td>" + QString::number(match.start()) + "</td>";
					html += "<td>" + QString::number(match.end()) + "</td>";
					html += "<td>" + QString::number(duration) + "</td>";
					html += "</tr>";
				}

				html += "</table>";
			}

			void ZebraAnalysisView::printRawData(QString &html)
			{
				int evcols = 8;
				html = "<center><table border=\"1\">";

				html += "<tr>";
				html += "<th>Match</th>";
				for(int i = 0 ; i < evcols ; i++)
					html += "<th>Event</th>";
				html += "</tr>";

				for (auto seq : sequences_)
				{
					html += "<tr>";
					html += "<td>" + seq->matchKey() + "</tr>";

					int index = 0;
					int col = 0;
					for (auto it = seq->cbegin(); it != seq->cend(); it++)
					{
						if (col == evcols)
						{
							html += "</tr>";
							html += "<tr>";
							html += "<td></td>";
							col = 0;
						}

						html += "<td>" + QString::number(index++) + it->toString() + "</td>";
						col++;
					}

					html += "</tr>";
				}
				html += "</table>";
			}
		}
	}
}
