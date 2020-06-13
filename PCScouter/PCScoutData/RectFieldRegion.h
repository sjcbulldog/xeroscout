#pragma once

#include "FieldHighlight.h"
#include <QRectF>

namespace xero
{
	namespace scouting
	{
		namespace datamodel
		{
			class RectFieldHighlight : public FieldHighlight
			{
			public:
				RectFieldHighlight(const QString &name, QColor c, const QRectF& r, Alliance a) : FieldHighlight(name, c, a)
				{
					rect_ = r;
				}

				bool isWithin(const QPointF& pt) const override
				{
					return rect_.contains(pt);
				}

				virtual DrawType drawType() const override {
					return DrawType::Rect;
				}

				virtual const QRectF& drawBounds() const override {
					return rect_;
				}

			private:
				QRectF rect_;
			};
		}
	}
}

