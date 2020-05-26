#pragma once

#include <QString>
#include <QRect>

namespace xero
{
	namespace scouting
	{
		namespace datamodel
		{
			class ImageFormSubItem
			{
			public:
				ImageFormSubItem(const QString& subname, const QRect &bounds) {
					subname_ = subname;
					bounds_ = bounds;
				}

				virtual ~ImageFormSubItem() {
				}

				const QRect& bounds() {
					return bounds_;
				}

				const QString& subname() const {
					return subname_;
				}

			private:
				QString subname_;
				QRect bounds_;
			};
		}
	}
}


