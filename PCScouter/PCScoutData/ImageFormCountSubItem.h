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

#include "ImageFormSubItem.h"
#include <QString>

namespace xero
{
	namespace scouting
	{
		namespace datamodel
		{
			class ImageFormCountSubItem : public ImageFormSubItem
			{
			public:
				ImageFormCountSubItem(const QString &subname, QString color, const QRect &bounds, int minv, int maxv) : ImageFormSubItem(subname, bounds) {
					minv_ = minv;
					maxv_ = maxv;
					limits_ = false;
					color_ = color ;
				}

				ImageFormCountSubItem(const QString& subname, QString color, const QRect& bounds, int minv, int maxv, int minlimit, int maxlimit) : ImageFormSubItem(subname, bounds) {
					minv_ = minv;
					maxv_ = maxv;
					color_ = color;
					limits_ = true;
					minlimit_ = minlimit;
					maxlimit_ = maxlimit;
				}

				virtual ~ImageFormCountSubItem() {
				}

				bool hasLimits() const {
					return limits_;
				}

				double minLimit() const {
					return minlimit_;
				}

				double maxLimit() const {
					return maxlimit_;
				}

				int minimum() const {
					return minv_;
				}

				int maximum() const {
					return maxv_;
				}

				bool hasColor() const {
					return color_.length() > 0;
				}

				const QString& color() const {
					return color_;
				}

			private:
				int minv_;
				int maxv_;
				bool limits_;
				double minlimit_;
				double maxlimit_;
				QString color_;
			};
		}
	}
}
