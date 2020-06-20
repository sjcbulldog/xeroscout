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

				selector_ = new MatchTeamSelector(dataModel(), this);
				connect(selector_, &MatchTeamSelector::matchSelected, this, &ZebraAnalysisView::matchesRobotsSelected);
				connect(selector_, &MatchTeamSelector::robotSelected, this, &ZebraAnalysisView::matchesRobotsSelected);
				connect(selector_, &MatchTeamSelector::selectedItemChanged, this, &ZebraAnalysisView::comboxChanged);
				selector_->setSizePolicy(QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);
				lay->addWidget(selector_);

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

			void ZebraAnalysisView::matchesRobotsSelected()
			{
				regenerate();
			}

			void ZebraAnalysisView::comboxChanged(const QString& key)
			{
				regenerate();
			}

			void ZebraAnalysisView::madeActive()
			{
				analyzer_.setActivities(dataModel()->activities());
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
				QString key = selector_->currentKey();

				report_txt_->clear();
				sequences_.clear();

				if (key.length() == 0)
					return;

				if (selector_->isMatchesSelected())
				{
					auto m = dataModel()->findMatchByKey(key);
					if (!m->hasZebra())
						return;

					Alliance c = Alliance::Red;
					for (int slot = 1; slot <= 3; slot++)
					{
						QString tkey = m->team(c, slot);
						auto track = DataModelBuilder::createTrack(dataModel(), key, tkey);
						if (track->hasData())
						{
							auto seq = std::make_shared<ZebraSequence>(key, tkey, track, dataModel()->fieldRegions());
							sequences_.push_back(seq);
						}
					}

					c = Alliance::Blue;
					for (int slot = 1; slot <= 3; slot++)
					{
						QString tkey = m->team(c, slot);
						auto track = DataModelBuilder::createTrack(dataModel(), key, tkey);
						if (track->hasData())
						{
							auto seq = std::make_shared<ZebraSequence>(key, tkey, track, dataModel()->fieldRegions());
							sequences_.push_back(seq);
						}
					}
				}
				else
				{
					for (auto m : dataModel()->matches())
					{
						if (!m->hasZebra())
							continue;

						Alliance c;
						int slot;

						if (m->teamToAllianceSlot(key, c, slot))
						{						
							auto track = DataModelBuilder::createTrack(dataModel(), m->key(), key);
							if (track->hasData())
							{
								auto seq = std::make_shared<ZebraSequence>(m->key(), key, track, dataModel()->fieldRegions());
								sequences_.push_back(seq);
							}
						}
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
				html += "<th>Team</th>";
				html += "<th>Alliance</th>";
				html += "<th>Pattern</th>";
				html += "<th>Start</th>";
				html += "<th>Duration</th>";
				html += "</tr>";

				for (auto& match : analyzer_.matches())
				{
					html += "<tr>";
					double duration = match.endTime() - match.startTime();

					auto m = dataModel()->findMatchByKey(match.sequence()->matchKey());
					html += "<td>" + m->title() +"</td>";

					auto t = dataModel()->findTeamByKey(match.sequence()->teamKey());
					html += "<td>" + t->title() + "</td>";

					Alliance c;
					int slot;
					m->teamToAllianceSlot(t->key(), c, slot);
					html += "<td>" + toString(c) + "</td>";

					html += "<td>" + match.name() + "</td>";
					html += "<td>" + QString::number(match.startTime(), 'f', 2) + "</td>";
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
