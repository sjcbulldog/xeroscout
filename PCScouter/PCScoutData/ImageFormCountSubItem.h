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
