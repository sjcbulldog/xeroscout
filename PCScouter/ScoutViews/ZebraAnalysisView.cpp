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

				idle_ = 2.0;
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

				if (key.length() == 0)
					return;

				std::list<std::pair<QString, QString>> selected;

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
							selected.push_back(std::make_pair(key, tkey));
					}

					c = Alliance::Blue;
					for (int slot = 1; slot <= 3; slot++)
					{
						QString tkey = m->team(c, slot);
						auto track = DataModelBuilder::createTrack(dataModel(), key, tkey);
						if (track->hasData())
							selected.push_back(std::make_pair(key, tkey));
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
								selected.push_back(std::make_pair(m->key(), key));
						}
					}
				}

				analyzer_.setDataModel(dataModel());
				analyzer_.analyze(selected);
				QString html;

				printSequences(html);
				html += "<hr><br>";

				report_txt_->setHtml(html);
			}

			void ZebraAnalysisView::printSequences(QString &html)
			{
				QString last;
				bool first = true;

				html += "<style>";
				html += "table {";
				html += "  border-collapse: collapse;";
				html += "}";
				html += "td, th {";
				html += "  padding: 3px;";
				html += "}";
				html += "</style>";
				html += "<center>";
				html += "<table border=\"1\">";

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
		}
	}
}
