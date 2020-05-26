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
				ImageFormCountSubItem(const QString &subname, const QRect &bounds, int minv, int maxv) : ImageFormSubItem(subname, bounds) {
					minv_ = minv;
					maxv_ = maxv;
				}

				virtual ~ImageFormCountSubItem() {
				}

				int minimum() const {
					return minv_;
				}

				int maximum() const {
					return maxv_;
				}

			private:
				int minv_;
				int maxv_;
			};
		}
	}
}
