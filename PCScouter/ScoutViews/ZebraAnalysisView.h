#pragma once

#include "scoutviews_global.h"
#include "ViewBase.h"
#include <QWidget>

namespace xero
{
	namespace scouting
	{
		namespace views
		{
			class SCOUTVIEWS_EXPORT ZebraAnalysisView : public QWidget, public ViewBase
			{
			public:
				ZebraAnalysisView(QWidget* parent);
				~ZebraAnalysisView();
			private:
			};
		}
	}
}

