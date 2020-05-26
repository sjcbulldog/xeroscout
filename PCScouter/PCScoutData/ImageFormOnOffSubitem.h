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
