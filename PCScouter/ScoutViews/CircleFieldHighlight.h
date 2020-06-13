#pragma once

#include "FieldHighlight.h"

namespace xero
{
	namespace scouting
	{
		namespace views
		{
			class CircleFieldHighlight : public FieldHighlight
			{
			public:
				CircleFieldHighlight(QColor c, const QPointF& loc, double radius) : FieldHighlight(c)
				{
					loc_ = loc;
					radius_ = radius;
					bounds_ = QRect(loc.x() - radius_, loc.y() - radius_, 2 * radius_, 2 * radius_);
				}

				CircleFieldHighlight(QColor c, const QPointF& loc, double radius, xero::scouting::datamodel::Alliance a) : FieldHighlight(c, a)
				{
					loc_ = loc;
					radius_ = radius;
				}

				bool isWithin(const QPointF& pt) override
				{
					return distSquared(pt, loc_) < radius_ * radius_;
				}

				virtual DrawType drawType() {
					return DrawType::Circle;
				}

				virtual const QRectF& drawBounds() {
					return bounds_;
				}

			private:
				QPointF loc_;
				double radius_;
				QRectF bounds_;
			};
		}
	}
}

