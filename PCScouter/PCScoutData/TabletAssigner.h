//
// Copyright(C) 2020 by Jack (Butch) Griffin
//
//	This program is free software : you can redistribute it and /or modify
//	it under the terms of the GNU General Public License as published by
//	the Free Software Foundation, either version 3 of the License, or
//	(at your option) any later version.
//
//	This program is distributed in the hope that it will be useful,
//	but WITHOUT ANY WARRANTY; without even the implied warranty of
//	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.See the
//	GNU General Public License for more details.
//
//	You should have received a copy of the GNU General Public License
//	along with this program.If not, see < https://www.gnu.org/licenses/>.
//
//
//
// This work we create by the named individual(s) above in support of the
// FRC robotics team Error Code Xero.
//

#pragma once

#include <QStringList>

namespace xero
{
	namespace scouting
	{
		namespace datamodel
		{

			class TabletAssigner
			{
			public:
				TabletAssigner();
				TabletAssigner(const QStringList& list);
				virtual ~TabletAssigner();

				enum class Strategy
				{
					Simple,
					Annealing
				};

				int count() {
					return static_cast<int>(matches_.size());
				}

				void addTablet(const QString& tablet) {
					tablets_.push_back(tablet);
				}

				void addTablets(const QStringList& list) {
					for (auto t : list)
						tablets_.push_back(t);
				}

				void assign(int matches, Strategy s);

				const QStringList& match(int which) {
					return matches_[which];
				}

			private:
				void assignSimple(int matches);
				void assignAnnealing(int matches);

				int cost();

			private:
				QStringList tablets_;
				std::vector<QStringList> matches_;
			};

		}
	}
}
