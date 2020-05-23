//
// Copyright 2020 by Jack W. (Butch) Griffin
// 
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// 
// http ://www.apache.org/licenses/LICENSE-2.0
// 
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissionsand
// limitations under the License.
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
