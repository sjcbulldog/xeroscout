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

#include "ImageItemDisplay.h"
#include "ImageFormItem.h"
#include "ImageFormCountSubItem.h"
#include "ImageFormOnOffSubitem.h"
#include "DataCollection.h"
#include <QPainter>
#include <QSize>
#include <QFontMetrics>
#include <QMouseEvent>
#include <QKeyEvent>
#include <QDebug>
#include <QtWidgets/QLayout>

namespace xero
{
	namespace scouting
	{
		namespace datamodel
		{
			ImageItemDisplay::ImageItemDisplay(ImageSupplier& images, const FormItemDesc* desc, QWidget* parent) : FormItemDisplay(images, desc, parent)
			{
				const ImageFormItem* fdesc = dynamic_cast<const ImageFormItem*>(desc);
				assert(fdesc != nullptr);

				image_ = images.get(fdesc->imageTag());
				if (image_ != nullptr)
				{
					DataCollection d;
					setValues(d);
				}

				flipped_ = false;
				setFocusPolicy(Qt::StrongFocus);
			}

			ImageItemDisplay::~ImageItemDisplay()
			{
			}


			QRect ImageItemDisplay::realBounds(std::shared_ptr<ImageFormSubItem> item)
			{
				const ImageFormItem* fdesc = dynamic_cast<const ImageFormItem*>(desc());
				assert(fdesc != nullptr);

				QRect r = item->bounds();
				if (flipped_) {
					r = QRect(image_->width() - r.x(), r.y(), r.width(), r.height());
				}

				r = QRect(QPoint(r.x() * multx_, r.y() * multy_), QSize(r.width() * multx_, r.height() * multy_));
				return r;
			}

			std::shared_ptr<ImageFormSubItem> ImageItemDisplay::findByPoint(const QPoint& pt)
			{
				const ImageFormItem* fdesc = dynamic_cast<const ImageFormItem*>(desc());
				assert(fdesc != nullptr);

				for (auto item : fdesc->items())
				{


					QRect r = realBounds(item);
					if (r.contains(pt))
					{
						return item;
					}
				}

				return NULL;
			}

			void ImageItemDisplay::mouseDoubleClickEvent(QMouseEvent* ev)
			{
				const ImageFormItem* fdesc = dynamic_cast<const ImageFormItem*>(desc());
				assert(fdesc != nullptr);

				std::shared_ptr<ImageFormSubItem> item = findByPoint(ev->pos());
				if (item == nullptr)
				{
					flipped_ = !flipped_;
					update();
				}
			}

			void ImageItemDisplay::mousePressEvent(QMouseEvent* ev)
			{
				const ImageFormItem* fdesc = dynamic_cast<const ImageFormItem*>(desc());
				assert(fdesc != nullptr);

				std::shared_ptr<ImageFormSubItem> item = findByPoint(ev->pos());
				if (item != nullptr)
				{
					QString longname = FormItemDesc::genComplexName(fdesc->tag(), item->subname());

					auto on_off = std::dynamic_pointer_cast<ImageFormOnOffSubitem>(item);
					if (on_off != nullptr)
					{
						if (on_off->isChoice())
						{
							mousePressChoice(on_off, longname, ev);
						}
						else
						{
							mousePressOnOff(on_off, longname, ev);
						}
					}

					auto count = std::dynamic_pointer_cast<ImageFormCountSubItem>(item);
					if (count != nullptr)
					{
						mousePressCount(count, longname, ev);
					}
				}

				update();
			}

			void ImageItemDisplay::mousePressOnOff(std::shared_ptr<ImageFormOnOffSubitem> item, const QString& longname, QMouseEvent* ev)
			{
				bool v = false;
				auto it = on_off_state_.find(longname);
				if (it != on_off_state_.end() && it->second)
					v = true;

				on_off_state_.insert_or_assign(longname, !v);
			}

			void ImageItemDisplay::mousePressChoice(std::shared_ptr<ImageFormOnOffSubitem> item, const QString& longname, QMouseEvent* ev)
			{
				choice_state_.insert_or_assign(longname, item->value());
			}

			void ImageItemDisplay::mousePressCount(std::shared_ptr<ImageFormCountSubItem> item, const QString& longname, QMouseEvent* ev)
			{
				QRect r = realBounds(item);
				int sign = 0;

				QRect upper = QRect(r.topLeft(), QSize(r.width(), r.height()));
				if (upper.contains(ev->pos()))
					sign = 1;

				QRect lower = QRect(QPoint(r.left(), r.top() + r.height() / 2), QSize(r.width(), r.height() / 2));
				if (lower.contains(ev->pos()))
					sign = -1;

				int v = 0;
				auto it = count_state_.find(longname);
				if (it != count_state_.end())
					v = it->second;

				v += sign;
				if (v < item->minimum())
					v = item->minimum();

				if (v > item->maximum())
					v = item->maximum();

				count_state_.insert_or_assign(longname, v);
			}

			void ImageItemDisplay::computeMultiplier()
			{
				if (image_ == nullptr)
				{
					multx_ = 1.0;
					multy_ = 1.0;
				}
				else
				{
					QSize s = image_->size();
					multx_ = (double)width() / (double)s.width();
					multy_ = (double)height() / (double)s.height();
				}
			}

			void ImageItemDisplay::showEvent(QShowEvent* ev)
			{
				if (image_ != nullptr) {
					QSize ps = parentWidget()->size();
					setGeometry(0, 0, ps.width(), ps.height());
				}
			}

			void ImageItemDisplay::resizeEvent(QResizeEvent* ev)
			{
				if (image_ != nullptr) {
					QSize ps = parentWidget()->size();
					setGeometry(0, 0, ps.width(), ps.height());
					computeMultiplier();
					update();
				}
			}

			void ImageItemDisplay::paintEvent(QPaintEvent* ev)
			{
				const ImageFormItem* fdesc = dynamic_cast<const ImageFormItem*>(desc());
				assert(fdesc != nullptr);

				QPainter p(this);

				QFont f = p.font();
				f.setPointSizeF(6.0);
				p.setFont(f);

				QRect r(QPoint(0, 0), size());
				if (flipped_)
					p.drawImage(r, image_->mirrored(true,false));
				else {
					p.drawImage(r, *image_);
				}

				QPen pen(QColor(0, 0, 0));
				pen.setWidth(4);

				QFontMetrics fm(p.font());

				for (auto item : fdesc->items())
				{
					QString longname = FormItemDesc::genComplexName(fdesc->tag(), item->subname());

					QRect r = realBounds(item);
					p.drawRect(r);

					p.save();
					auto on_off = std::dynamic_pointer_cast<ImageFormOnOffSubitem>(item);
					if (on_off != nullptr)
					{
						if (on_off->isChoice())
						{
							drawChoice(p, on_off, longname);
						}
						else
						{
							drawOnOff(p, on_off, longname);
						}
					}

					auto count = std::dynamic_pointer_cast<ImageFormCountSubItem>(item);
					if (count != nullptr)
					{
						drawCount(p, count, longname);
					}

					p.restore();
				}
			}

			void ImageItemDisplay::drawBoundsCenterLine(QPainter& p, const QRect &r, const QString &txt)
			{
				p.save();

				QPen pen(QColor(0, 0, 0));
				pen.setWidth(CenterLineWidth);
				p.setPen(pen);
				p.drawLine(r.left() + CenterLineWidth / 2, r.center().y(), r.right() - CenterLineWidth / 2 + 1, r.center().y());

				QFont f = p.font();
				f.setPointSizeF(CenterFontSize);
				p.setFont(f);
				QFontMetrics fm(f);

				pen = QPen(QColor(255, 255, 255));
				p.setPen(pen);
				QPoint pt = QPoint(r.center().x() - fm.horizontalAdvance(txt) / 2, r.center().y() + CenterLineWidth / 2 - fm.descent());
				p.drawText(pt, txt);

				p.restore();
			}

			void ImageItemDisplay::drawOnOff(QPainter& p, std::shared_ptr<ImageFormOnOffSubitem> item, const QString& longname)
			{
				QRect r = realBounds(item);
				bool v = false;
				auto it = on_off_state_.find(longname);
				if (it != on_off_state_.end() && it->second)
					v = true;

				drawBoundsCenterLine(p, r, item->subname());

				if (v)
				{

					QPen pen(QColor(0, 255, 0));
					pen.setWidth(4);
					p.setPen(pen);

					p.drawLine(r.x(), r.y(), r.x() + r.width(), r.y() + r.height());
					p.drawLine(r.x() + r.width(), r.y(), r.x(), r.y() + r.height());
				}
			}

			void ImageItemDisplay::drawChoice(QPainter& p, std::shared_ptr<ImageFormOnOffSubitem> item, const QString& longname)
			{
				QRect r = realBounds(item);
				bool v = false;
				auto it = choice_state_.find(longname);
				if (it != choice_state_.end() && it->second == item->value())
					v = true;

				drawBoundsCenterLine(p, r, item->value());

				if (v)
				{
					QRect r = realBounds(item);
					QPen pen(QColor(0, 255, 0));

					pen.setWidth(4);
					p.setPen(pen);
					p.drawLine(r.x(), r.y(), r.x() + r.width(), r.y() + r.height());
					p.drawLine(r.x() + r.width(), r.y(), r.x(), r.y() + r.height());
				}
			}

			void ImageItemDisplay::drawCount(QPainter& p, std::shared_ptr<ImageFormCountSubItem> item, const QString& longname)
			{
				QPoint pt;
				int linewidth = 10;
				QRect r = realBounds(item);

				int v = 0;
				auto it = count_state_.find(longname);
				if (it != count_state_.end())
					v = it->second;

				QString num = QString::number(v);

				drawBoundsCenterLine(p, r, item->subname());

				p.save();

				QFont f(p.font());
				f.setPointSizeF(16);
				p.setFont(f);
				QFontMetrics fm(f);

				QColor color(255, 255, 0);
				if (item->hasColor())
				{
					const QString& cstr = item->color();
					if (QColor::isValidColor(cstr))
					{
						color = QColor(item->color());
					}
				}
				QPen pentxt(color);

				p.setPen(pentxt);
				pt = QPoint(r.x() - fm.horizontalAdvance(num + "AA") / 2, r.center().y() + fm.height() / 2);
				if (pt.x() < 0)
					pt = QPoint(r.right(), r.center().y() + fm.height() / 2);
				p.drawText(pt, num);

				int size = 10;
				QRect upper = QRect(r.topLeft(), QSize(r.width(), r.height() / 2));
				pt = upper.center();
				QPen pen;
				pen.setWidth(4);
				p.setPen(pen);
				p.drawLine(pt.x() - size, pt.y(), pt.x() + size, pt.y());
				p.drawLine(pt.x(), pt.y() - size, pt.x(), pt.y() + size);

				QRect lower = QRect(QPoint(r.left(), r.top() + r.height() / 2), QSize(r.width(), r.height() / 2));
				pt = lower.center();
				p.drawLine(pt.x() - size, pt.y(), pt.x() + size, pt.y());

				p.restore();
			}

			void ImageItemDisplay::setValues(const DataCollection& data)
			{
				const ImageFormItem* fdesc = dynamic_cast<const ImageFormItem*>(desc());
				assert(fdesc != nullptr);

				for (auto item : fdesc->items())
				{
					QString longname = FormItemDesc::genComplexName(fdesc->tag(), item->subname());

					auto on_off = std::dynamic_pointer_cast<ImageFormOnOffSubitem>(item);
					if (on_off != nullptr)
					{
						//
						// This could still be an on/off or a choice item
						//
						if (on_off->isChoice())
						{
							QString val = on_off->value();

							if (data.has(longname))
							{
								QVariant v = data.dataByName(longname);
								if (v.type() == QVariant::Type::String)
									val = v.toString();
							}
							choice_state_.insert_or_assign(longname, val);
						}
						else
						{
							bool val = false;

							if (data.has(longname))
							{
								QVariant v = data.dataByName(longname);
								if (v.type() == QVariant::Type::Bool)
									val = v.toBool();
							}
							on_off_state_.insert_or_assign(longname, val);
						}
					}

					auto count = std::dynamic_pointer_cast<ImageFormCountSubItem>(item);
					if (count != nullptr)
					{
						int val = count->minimum();

						if (data.has(longname))
						{
							QVariant v = data.dataByName(longname);
							if (v.type() == QVariant::Type::Int)
								val = v.toInt();

							if (val < count->minimum())
								val = count->minimum();

							if (val > count->maximum())
								val = count->maximum();
						}
						count_state_.insert_or_assign(longname, val);
					}
				}
			}

			DataCollection ImageItemDisplay::getValues()
			{
				const ImageFormItem* fdesc = dynamic_cast<const ImageFormItem*>(desc());
				assert(fdesc != nullptr);

				DataCollection d;

				for (auto pair : on_off_state_)
					d.add(pair.first, QVariant(pair.second));

				for (auto pair : count_state_)
					d.add(pair.first, QVariant(pair.second));

				for (auto pair : choice_state_)
					d.add(pair.first, QVariant(pair.second));

				fdesc->addPublishedItems(d);

				return d;
			}
		}
	}
}
