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

#include "scoutviews_global.h"
#include <QLayout>
#include <qwidget.h>
#include <QList>
#include <QStyle>

namespace xero
{
    namespace scouting
    {
        namespace views
        {

            class SCOUTVIEWS_EXPORT FlowLayout : public QLayout
            {
            public:
                explicit FlowLayout(QWidget* parent, int margin = -1, int hSpacing = -1, int vSpacing = -1);
                explicit FlowLayout(int margin = -1, int hSpacing = -1, int vSpacing = -1);
                ~FlowLayout();

                void addItem(QLayoutItem* item) override;
                int horizontalSpacing() const;
                int verticalSpacing() const;
                Qt::Orientations expandingDirections() const override;
                bool hasHeightForWidth() const override;
                int heightForWidth(int) const override;
                int count() const override;
                QLayoutItem* itemAt(int index) const override;
                QSize minimumSize() const override;
                void setGeometry(const QRect& rect) override;
                QSize sizeHint() const override;
                QLayoutItem* takeAt(int index) override;

            private:
                int doLayout(const QRect& rect, bool testOnly) const;
                int smartSpacing(QStyle::PixelMetric pm) const;

                QList<QLayoutItem*> itemList;
                int m_hSpace;
                int m_vSpace;
            };
        }
    }
}
