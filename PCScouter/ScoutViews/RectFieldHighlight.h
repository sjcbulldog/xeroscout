#pragma once

#include "FieldHighlight.h"
#include <QRectF>

namespace xero
{
	namespace scouting
	{
		namespace views
		{
			class RectFieldHighlight : public FieldHighlight
			{
			public:
				RectFieldHighlight(QColor c, const QRectF& r) : FieldHighlight(c)
				{
					rect_ = r;
				}

				RectFieldHighlight(QColor c, const QRectF& r, xero::scouting::datamodel::Alliance a) : FieldHighlight(c, a)
				{
					rect_ = r;
				}

				bool isWithin(const QPointF& pt) override
				{
					return rect_.contains(pt);
				}

				virtual DrawType drawType() {
					return DrawType::Rect;
				}

				virtual const QRectF& drawBounds() {
					return rect_;
				}

			private:
				QRectF rect_;
			};
		}
	}
}

