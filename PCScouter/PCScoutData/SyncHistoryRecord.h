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

#pragma once

#include "TabletIdentity.h"
#include <QDateTime>
#include <QStringList>

namespace xero
{
	namespace scouting
	{
		namespace datamodel
		{

			class SyncHistoryRecord
			{
			public:
				SyncHistoryRecord(const TabletIdentity& id, const QDateTime& when, const QStringList& pits, const QStringList& matches)
				{
					id_ = id;
					when_ = when.toMSecsSinceEpoch();
					pits_ = pits;
					matches_ = matches;
				}

				const TabletIdentity& id() const {
					return id_;
				}

				QDateTime when() const {
					return QDateTime::fromMSecsSinceEpoch(when_);
				}

				const QStringList& pits() const {
					return pits_;
				}

				const QStringList& matches() const {
					return matches_;
				}

			private:
				TabletIdentity id_;
				qint64 when_;
				QStringList pits_;
				QStringList matches_;
			};

		}
	}
}
