#pragma once

#include "Alliance.h"
#include <QColor>
#include <QRectF>

namespace xero
{
	namespace scouting
	{
		namespace datamodel
		{
			class FieldHighlight
			{
			public:
				enum class DrawType
				{
					Circle,
					Rect,
				};

			public:
				FieldHighlight(const QString &name, QColor color, xero::scouting::datamodel::Alliance a) {
					name_ = name;
					color_ = color;
					alliance_ = a;
				}
			
				const QString& name() const {
					return name_;
				}

				bool doesAllianceMatch(xero::scouting::datamodel::Alliance a) const {
					return alliance_ == xero::scouting::datamodel::Alliance::Both || a == alliance_;
				}

				QColor color() const {
					return color_;
				}

				xero::scouting::datamodel::Alliance alliance() const {
					return alliance_;
				}

				virtual bool isWithin(const QPointF& pt) const = 0;

				virtual DrawType drawType() const = 0;
				virtual const QRectF& drawBounds() const = 0;

			protected:
				static double distSquared(const QPointF& r1, const QPointF& r2)
				{
					double dx = r1.x() - r2.x();
					double dy = r1.y() - r2.y();

					return dx * dx + dy * dy;
				}

			private:
				QString name_;
				QColor color_;
				xero::scouting::datamodel::Alliance alliance_;
			};
		}
	}
}
