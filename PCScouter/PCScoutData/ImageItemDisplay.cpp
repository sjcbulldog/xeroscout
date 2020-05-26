#include "ImageItemDisplay.h"
#include "ImageFormItem.h"
#include "ImageFormCountSubItem.h"
#include "ImageFormOnOffSubitem.h"
#include "DataCollection.h"
#include <QPainter>
#include <QSize>
#include <QFontMetrics>
#include <QMouseEvent>

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

				scale_ = fdesc->scale();
				image_ = images.get(fdesc->imageTag());

				QSize s = image_->size();
				size_ = QSize(s.width() * scale_, s.height() * scale_);
				setMinimumSize(size_);
				setMaximumSize(size_);

				DataCollection d;
				setValues(d);
			}

			ImageItemDisplay::~ImageItemDisplay()
			{
			}

			QRect ImageItemDisplay::realBounds(std::shared_ptr<ImageFormSubItem> item)
			{
				const ImageFormItem* fdesc = dynamic_cast<const ImageFormItem*>(desc());
				assert(fdesc != nullptr);

				QRect r = item->bounds();
				r = QRect(QPoint(r.x() * scale_, r.y() * scale_), QSize(r.width() * scale_, r.height() * scale_));

				return r;
			}

			void ImageItemDisplay::mousePressEvent(QMouseEvent* ev)
			{
				const ImageFormItem* fdesc = dynamic_cast<const ImageFormItem*>(desc());
				assert(fdesc != nullptr);

				for (auto item : fdesc->items())
				{
					QString longname = FormItemDesc::genComplexName(fdesc->tag(), item->subname());

					QRect r = realBounds(item);
					if (r.contains(ev->pos()))
					{
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
				int v = 0;
				auto it = count_state_.find(longname);
				if (it != count_state_.end())
					v = it->second;

				count_state_.insert_or_assign(longname, v + 1);
			}

			void ImageItemDisplay::paintEvent(QPaintEvent* ev)
			{
				const ImageFormItem* fdesc = dynamic_cast<const ImageFormItem*>(desc());
				assert(fdesc != nullptr);

				QPainter p(this);

				QFont f = p.font();
				f.setPointSizeF(6.0);
				p.setFont(f);

				QRect r(QPoint(0, 0), size_);
				p.drawImage(r, *image_);

				QPen pen(QColor(0, 0, 0));
				pen.setWidth(4);

				QFontMetrics fm(p.font());

				for (auto item : fdesc->items())
				{
					QString longname = FormItemDesc::genComplexName(fdesc->tag(), item->subname());

					QRect r = realBounds(item);
					p.drawRect(r);

					int x = r.x() - fm.horizontalAdvance(item->subname() + "A");
					int y = r.y() + r.height() / 2;
					QPoint pt(x, y);
					p.drawText(pt, item->subname());

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

			void ImageItemDisplay::drawOnOff(QPainter& p, std::shared_ptr<ImageFormOnOffSubitem> item, const QString& longname)
			{
				bool v = false;
				auto it = on_off_state_.find(longname);
				if (it != on_off_state_.end() && it->second)
					v = true;

				if (v)
				{
					QRect r = realBounds(item);
					QPen pen(QColor(0, 255, 0));

					p.setPen(pen);
					p.drawLine(r.x(), r.y(), r.x() + r.width(), r.y() + r.height());
					p.drawLine(r.x() + r.width(), r.y(), r.x(), r.y() + r.height());
				}
			}

			void ImageItemDisplay::drawChoice(QPainter& p, std::shared_ptr<ImageFormOnOffSubitem> item, const QString& longname)
			{
				bool v = false;
				auto it = choice_state_.find(longname);
				if (it != choice_state_.end() && it->second == item->value())
					v = true;

				if (v)
				{
					QRect r = realBounds(item);
					QPen pen(QColor(0, 255, 0));

					p.setPen(pen);
					p.drawLine(r.x(), r.y(), r.x() + r.width(), r.y() + r.height());
					p.drawLine(r.x() + r.width(), r.y(), r.x(), r.y() + r.height());
				}
			}

			void ImageItemDisplay::drawCount(QPainter& p, std::shared_ptr<ImageFormCountSubItem> item, const QString& longname)
			{
				int v = 0;
				auto it = count_state_.find(longname);
				if (it != count_state_.end())
					v = it->second;

				QString num = QString::number(v);
				QRect r = realBounds(item);

				p.save();

				QFont f = p.font();
				f.setPointSizeF(16);
				p.setFont(f);

				QFontMetrics fm(f);

				QPoint pt(r.center().x() - fm.horizontalAdvance(num) / 2, r.center().y());
				p.drawText(pt, num);

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
							if (choice_state_.find(longname) == choice_state_.end())
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
