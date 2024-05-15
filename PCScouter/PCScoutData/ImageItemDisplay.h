//
// Copyright(C) 2020 by Jack (Butch) Griffin
//
//	This program is free software : you can redistribute it and /or modify
//	it under the terms of the GNU General Public License as published by
//	the Free Software Foundation, either version 3 of the License, or
//	(at your option) any later version.
//
//	This program is distributed in the hope that it will be useful,
//	but WITHOUT ANY WARRANTY; without even the implied warranty of
//	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.See the
//	GNU General Public License for more details.
//
//	You should have received a copy of the GNU General Public License
//	along with this program.If not, see < https://www.gnu.org/licenses/>.
//
//
//
// This work we create by the named individual(s) above in support of the
// FRC robotics team Error Code Xero.
//

#pragma once
#include "FormItemDisplay.h"
#include "ImageFormCountSubItem.h"
#include "ImageFormOnOffSubitem.h"

namespace xero
{
	namespace scouting
	{
		namespace datamodel
		{
			class ImageItemDisplay :public FormItemDisplay
			{
			public:
				ImageItemDisplay(ImageSupplier& images, const FormItemDesc* desc, QWidget* parent);
				virtual ~ImageItemDisplay();

				virtual void setValues(const DataCollection& data);

				virtual DataCollection getValues();

			protected:
				void paintEvent(QPaintEvent* ev) override;
				void mousePressEvent(QMouseEvent* ev) override;
				void mouseDoubleClickEvent(QMouseEvent* ev) override;
				void resizeEvent(QResizeEvent* ev) override;
				void showEvent(QShowEvent* ev) override;

			private:
				void drawOnOff(QPainter& p, std::shared_ptr<ImageFormOnOffSubitem> item, const QString& longname);
				void drawChoice(QPainter& p, std::shared_ptr<ImageFormOnOffSubitem> item, const QString& longname);
				void drawCount(QPainter& p, std::shared_ptr<ImageFormCountSubItem> item, const QString& longname);

				void mousePressOnOff(std::shared_ptr<ImageFormOnOffSubitem> item, const QString &longname, QMouseEvent* ev);
				void mousePressChoice(std::shared_ptr<ImageFormOnOffSubitem> item, const QString &longname, QMouseEvent* ev);
				void mousePressCount(std::shared_ptr<ImageFormCountSubItem> item, const QString &longname, QMouseEvent* ev);
				
				QRect realBounds(std::shared_ptr<ImageFormSubItem> item);

				void drawBoundsCenterLine(QPainter& p, const QRect &r, const QString &txt);

				void computeMultiplier();

				std::shared_ptr<xero::scouting::datamodel::ImageFormSubItem> findByPoint(const QPoint& pt);

				static constexpr const int CenterLineWidth = 10;
				static constexpr const int CenterFontSize = 8;

			private:
				std::shared_ptr<QImage> image_;
				QSize size_;
				double multx_;
				double multy_;
				bool flipped_;

				std::map<QString, bool> on_off_state_;
				std::map<QString, int> count_state_;
				std::map<QString, QString> choice_state_;
			};
		}
	}
}


