#pragma once
#include <QWidget>
#include <vector>
#include <map>

namespace xero
{
	namespace scouting
	{
		namespace views
		{
			class TeamSelectedWidget : public QWidget
			{
				Q_OBJECT

			public:
				TeamSelectedWidget(QWidget* parent);
				virtual ~TeamSelectedWidget();

				void setTeams(const std::vector<int>& teams) {
					teams_.clear();
					for (int team : teams)
					{
						teams_[team] = false;
					}

					updateSize();
				}

				std::list<int> selected() const {
					std::list<int> list;

					for (auto pair : teams_)
					{
						if (pair.second)
							list.push_back(pair.first);
					}

					return list;
				}

			signals:
				void teamStateChanged(int team, bool selected);

			protected:
				void paintEvent(QPaintEvent* ev) override;
				void mousePressEvent(QMouseEvent* ev) override;

			private:
				constexpr static const int TeamBoxHeight = 36;
				constexpr static const int TeamBoxSpacing = 12;
				constexpr static const int TeamBoxWidth = 50;
				constexpr static const int TopMargin = 8;
				constexpr static const int BottomMargin = 8;
				constexpr static const int LeftMargin = 8;
				constexpr static const int RightMargin = 8;

			private:
				void updateSize();

			private:
				std::map<int, bool> teams_;
				QColor back_color_selected_;
				QColor back_color_unselected_;
			};
		}
	}
}
