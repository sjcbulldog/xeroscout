#pragma once

#include "FieldRegion.h"
#include <QRectF>

namespace xero
{
	namespace scouting
	{
		namespace datamodel
		{
			class RectFieldRegion : public FieldRegion
			{
			public:
				RectFieldRegion(const QString &name, QColor c, const QRectF& r, Alliance a) : FieldRegion(name, c, a)
				{
					rect_ = r;
				}

				bool isWithin(const QPointF& pt) const override
				{
					return rect_.contains(pt);
				}

				virtual QRectF drawBounds() const override {
					return rect_;
				}

			private:
				QRectF rect_;
			};
		}
	}
}

