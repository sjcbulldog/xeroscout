#pragma once
#include <qwidget.h>

namespace xero
{
	namespace scouting
	{
		namespace datamodel
		{
			class StartStopWidget : public QWidget
			{
				Q_OBJECT

			public:
				StartStopWidget(QWidget* parent);
				virtual ~StartStopWidget();

				void setStopped();
				bool state() {
					return state_;
				}

			signals:
				void cycleStarted();
				void cycleCompleted();
				void cycleAbandoned();

			protected:
				void paintEvent(QPaintEvent* ev) override;
				void mousePressEvent(QMouseEvent* ev) override;

			private:
				void drawRectWithText(QPainter& p, const QRect& r, const QString& txt, QColor rcolor, QColor tcolor);

			private:
				bool state_;
			};
		}
	}
}

