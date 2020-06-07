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

namespace xero
{
	namespace scouting
	{
		namespace datamodel
		{
			class ImageFormOnOffSubitem : public ImageFormSubItem
			{
			public:
				ImageFormOnOffSubitem(const QString& subname, const QRect& bounds) : ImageFormSubItem(subname, bounds) {
				}

				ImageFormOnOffSubitem(const QString& subname, const QString &value, const QRect& bounds) : ImageFormSubItem(subname, bounds) {
					value_ = value;
				}

				virtual ~ImageFormOnOffSubitem() {
				}

				bool isChoice() const {
					return value_.length() > 0;
				}

				const QString& value() const {
					return value_;
				}

			private:
				//
				// This is empty for an on/off, but contains a value if it is a choice.  If it contains
				// a value, then the tag identifies other subitems that are part of the same choice
				//
				QString value_;
			};
		}
	}
}
