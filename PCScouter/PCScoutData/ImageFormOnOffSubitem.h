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
