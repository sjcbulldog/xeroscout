#pragma once

#include "Alliance.h"
#include <QColor>
#include <QRectF>

namespace xero
{
	namespace scouting
	{
		namespace views
		{
			class FieldHighlight
			{
			public:
				enum DrawType
				{
					Circle,
					Rect,
				};

			public:
				FieldHighlight(QColor color) {
					color_ = color;
					apply_alliance_ = false;
				}

				FieldHighlight(QColor color, xero::scouting::datamodel::Alliance a) {
					color_ = color;
					apply_alliance_ = true;
					alliance_ = a;
				}

				bool doesAllianceMatch(xero::scouting::datamodel::Alliance a) {
					return !apply_alliance_ || a == alliance_;
				}

				QColor color() const {
					return color_;
				}

				xero::scouting::datamodel::Alliance alliance() const {
					return alliance_;
				}

				virtual bool isWithin(const QPointF& pt) = 0;

				virtual DrawType drawType() = 0;
				virtual const QRectF& drawBounds() = 0;

			protected:
				static double distSquared(const QPointF& r1, const QPointF& r2)
				{
					double dx = r1.x() - r2.x();
					double dy = r1.y() - r2.y();

					return dx * dx + dy * dy;
				}

			private:
				QColor color_;
				bool apply_alliance_;
				xero::scouting::datamodel::Alliance alliance_;
			};
		}
	}
}
