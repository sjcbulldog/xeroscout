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

#include "TabletAssigner.h"

namespace xero
{
	namespace scouting
	{
		namespace datamodel
		{

			TabletAssigner::TabletAssigner()
			{
			}

			TabletAssigner::TabletAssigner(const QStringList& list)
			{
				tablets_ = list;
			}

			TabletAssigner::~TabletAssigner()
			{
			}

			void TabletAssigner::assign(int matches, TabletAssigner::Strategy s)
			{
				assert(tablets_.size() >= 6);

				switch (s)
				{
				case Strategy::Simple:
					assignSimple(matches);
					break;

				case Strategy::Annealing:
					assignAnnealing(matches);
					break;
				}
			}

			void TabletAssigner::assignSimple(int matches)
			{
				int index = 0;

				for (int i = 0; i < matches; i++)
				{
					QStringList match;
					for (int j = 0; j < 6; j++) {

						match.push_back(tablets_[index]);
						index++;
						if (index == tablets_.size())
							index = 0;
					}
					matches_.push_back(match);
				}
			}

			int TabletAssigner::cost()
			{
				return 0;
			}

			void TabletAssigner::assignAnnealing(int matches)
			{
				assert(false);
			}
		}
	}
}
