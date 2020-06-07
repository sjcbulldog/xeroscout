//
// Copyright 2020 by Jack W. (Butch) Griffin
//
// Courtesy of Error Code Xero
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

#include "pcscoutdata_global.h"
#include <QStringList>
#include <QJsonObject>
#include <QJsonArray>
#include <vector>
#include <memory>

namespace xero
{
	namespace scouting
	{
		namespace datamodel
		{
			class PCSCOUTDATA_EXPORT GraphDescriptor
			{
			public:
				class GraphPane
				{
				public:
					GraphPane(const QString& title) {
						title_ = title;
						minv_ = -1;
						maxv_ = -1;
					}

					virtual ~GraphPane() {
					}

					bool hasRange() const {
						//
						// This is to be sure our floating point gets us something close to zero
						// or positive
						//
						return minv_ > -0.00001 && maxv_ > -0.00001;
					}

					const QString& title() const {
						return title_;
					}

					void setTitle(const QString& txt) {
						title_ = txt;
					}

					void addX(const QString& var) {
						if (!x_.contains(var))
							x_.push_back(var);
					}

					void removeX(const QString& var) {
						x_.removeOne(var);
					}

					const QStringList& x() const {
						return x_;
					}

					void setRange(double minv, double maxv) {
						minv_ = minv;
						maxv_ = maxv;
					}

					double minv() const {
						return minv_;
					}

					double maxv() const {
						return maxv_;
					}

					QJsonObject generateJSON() const {
						QJsonObject obj;

						obj["title"] = title_;

						QJsonArray range;
						range.push_back(minv_);
						range.push_back(maxv_);
						obj["range"] = range;

						QJsonArray fields;
						for (const QString& field : x_)
							fields.push_back(field);

						obj["x"] = fields;
						return obj;
					}

				private:
					QString title_;
					QStringList x_;
					double minv_;
					double maxv_;
				};

			public:
				GraphDescriptor(const QString &name);
				GraphDescriptor();
				virtual ~GraphDescriptor();

				bool isValid() const {
					return panes_.size() != 0;
				}

				QJsonObject generateJSON() const;

				const QString& name() const {
					return name_;
				}

				std::shared_ptr<GraphPane> addPane(const QString& title);
				void deletePane(std::shared_ptr<GraphPane>);

				int count() const {
					return static_cast<int>(panes_.size());
				}

				std::shared_ptr<const GraphPane> pane(int index) const {
					return panes_[index];
				}

				std::shared_ptr<GraphPane> pane(int index) {
					return panes_[index];
				}

			private:
				QString name_;
				std::vector<std::shared_ptr<GraphPane>> panes_;
			};
		}
	}
}
