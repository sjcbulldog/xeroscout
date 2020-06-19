#pragma once

#include "Alliance.h"
#include "PathFieldView.h"
#include "ViewBase.h"
#include <QWidget>

namespace xero
{
	namespace scouting
	{
		namespace views
		{
			class ZebraRegionEditor : public QWidget, public ViewBase
			{
			public:
				ZebraRegionEditor(QWidget* parent);
				virtual ~ZebraRegionEditor();

				virtual void clearView() override ;
				virtual void refreshView() override ;

				virtual void madeActive() override;

				PathFieldView* field() { return field_; }

			private:

				enum class HighlightType
				{
					Circle,
					Rectangle,
					Polygon
				};

			private:
				void fieldContextMenu(QPoint pt);
				void fieldKeyPressed(Qt::Key key);
				void renameRegion();
				void addHighlight(xero::scouting::datamodel::Alliance a, HighlightType ht);
				void removeHighlight(const QString& name);
				bool validRegionName(const QString& name);
				void polygonSelected(const std::vector<QPointF>& points, xero::scouting::datamodel::Alliance a);
				void areaSelected(const QRectF& area, xero::scouting::datamodel::Alliance a, HighlightType ht);
				void removeAllRegions();

			private:
				PathFieldView* field_;
				QMetaObject::Connection field_connect_;
				QPoint menu_point_;
			};
		}
	}
}


