#pragma once

#include "FieldRegion.h"
#include <QPolygonF>

namespace xero
{
	namespace scouting
	{
		namespace datamodel
		{
			class PolygonFieldRegion : public FieldRegion
			{
			public:
				PolygonFieldRegion(const QString& name, QColor c, const std::vector<QPointF> &points, xero::scouting::datamodel::Alliance a) : FieldRegion(name, c, a)
				{
					points_ = points;
					QVector<QPointF> pts;
					for (auto onept : points)
						pts.push_back(onept);
					poly_ = QPolygonF(pts);
				}

				bool isWithin(const QPointF& pt) const override
				{
					return poly_.containsPoint(pt, Qt::FillRule::OddEvenFill);
				}

				const QPolygonF& polygon() const {
					return poly_;
				}

				virtual QRectF drawBounds() const {
					return poly_.boundingRect();
				}

				const std::vector<QPointF>& points() const {
					return points_;
				}

			private:
				std::vector<QPointF> points_;
				QPolygonF poly_;
			};
		}
	}
}

