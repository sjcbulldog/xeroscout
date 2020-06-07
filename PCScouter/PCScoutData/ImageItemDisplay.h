//
// Copyright 2020 by Jack W. (Butch) Griffin
//
// Courtesy of Error Code Xero
// 
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// 
// http ://www.apache.org/licenses/LICENSE-2.0
// 
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissionsand
// limitations under the License.
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
				void paintEvent(QPaintEvent* ev);
				void mousePressEvent(QMouseEvent* ev);

			private:
				void drawOnOff(QPainter& p, std::shared_ptr<ImageFormOnOffSubitem> item, const QString& longname);
				void drawChoice(QPainter& p, std::shared_ptr<ImageFormOnOffSubitem> item, const QString& longname);
				void drawCount(QPainter& p, std::shared_ptr<ImageFormCountSubItem> item, const QString& longname);

				void mousePressOnOff(std::shared_ptr<ImageFormOnOffSubitem> item, const QString &longname, QMouseEvent* ev);
				void mousePressChoice(std::shared_ptr<ImageFormOnOffSubitem> item, const QString &longname, QMouseEvent* ev);
				void mousePressCount(std::shared_ptr<ImageFormCountSubItem> item, const QString &longname, QMouseEvent* ev);
				
				QRect realBounds(std::shared_ptr<ImageFormSubItem> item);

				void drawBoundsCenterLine(QPainter& p, const QRect &r, const QString &txt);

				static constexpr const int CenterLineWidth = 10;
				static constexpr const int CenterFontSize = 8;

			private:
				double scale_;
				std::shared_ptr<QImage> image_;
				QSize size_;

				std::map<QString, bool> on_off_state_;
				std::map<QString, int> count_state_;
				std::map<QString, QString> choice_state_;
			};
		}
	}
}


