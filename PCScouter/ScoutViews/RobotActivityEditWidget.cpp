#include "RobotActivityEditWidget.h"
#include "SequenceEnterExitPattern.h"
#include "SequenceEnterIdlePattern.h"
#include <QPainter>
#include <QDragEnterEvent>
#include <QDragMoveEvent>
#include <QDragLeaveEvent>
#include <QDropEvent>
#include <QMimeData>
#include <QMouseEvent>
#include <QInputDialog>
#include <QDebug>

using namespace xero::scouting::datamodel;

namespace xero
{
	namespace scouting
	{
		namespace views
		{
			RobotActivityEditWidget::RobotActivityEditWidget(std::shared_ptr<QImage> enter, std::shared_ptr<QImage> exit, std::shared_ptr<QImage> idle, QWidget* w) : QWidget(w)
			{
				top_bottom_border_ = 8;
				left_right_border_ = 8;
				text_graphics_border_ = 8;
				title_area_height_ = 18;
				column_left_right_border_ = 8;
				between_column_spacing_ = 8;
				tab_text_spacing = 2;
				image_size_ = 48;
				highlight_column_ = -1;
				between_item_border_ = 4;
				handle_size_ = 8;

				enter_image_ = enter;
				exit_image_ = exit;
				idle_image_ = idle;

				setAcceptDrops(true);

				setFocusPolicy(Qt::FocusPolicy::StrongFocus);
			}

			RobotActivityEditWidget::~RobotActivityEditWidget()
			{
			}

			void RobotActivityEditWidget::keyPressEvent(QKeyEvent* ev)
			{
				if (ev->key() == Qt::Key::Key_Delete)
				{
					for (const SelectedItem& item : selected_)
					{
						auto act = activities_[item.activity_];

						if (item.type_ == SelectType::Activity)
						{
							activities_.erase(activities_.begin() + item.activity_);
							emit deletedActivity(act);
						}
						else if (item.type_ == SelectType::Pattern)
						{
							act->deletePattern(item.pattern_);
							emit changedActivity(act);
						}
					}

					int i = activities_.size() - 1;
					while (i >= 0)
					{
						if (activities_[i]->count() == 0)
						{
							auto act = activities_[i];
							activities_.erase(activities_.begin() + i);
							emit deletedActivity(act); 
						}

						i--;
					}
					
					selected_.clear();
					update();
				}
				else if (ev->key() == Qt::Key::Key_Up || ev->key() == Qt::Key::Key_Down)
				{
					if (selected_.size() != 1)
						return;

					if (selected_[0].type_ != SelectType::Pattern)
						return;

					SelectedItem item = selected_[0];
					auto act = activities_[item.activity_];
					bool updated = false;
					if (ev->key() == Qt::Key::Key_Up && item.pattern_ != 0)
					{
						act->movePatternUp(item.pattern_);
						item.pattern_--;
						selected_[0] = item;
						updated = true;
					}
					else if (ev->key() == Qt::Key::Key_Down && item.pattern_ != act->count() - 1)
					{
						act->movePatternDown(item.pattern_);
						item.pattern_++;
						selected_[0] = item;
						updated = true;
					}

					if (updated)
					{
						emit changedActivity(act);
						update();
					}
				}
			}

			void RobotActivityEditWidget::updateMinMaxCount(int col, int pattern)
			{
				auto act = activities_[col];
				auto p = act->patterns()[pattern];

				int mincnt = QInputDialog::getInt(this, "Mininum Match Count", "Minimum Match Count", p->minCount());
				int maxcnt = QInputDialog::getInt(this, "Maximum Match Count", "Maximum Match Count", p->maxCount());

				act->setMinMaxCount(pattern, mincnt, maxcnt);
				emit changedActivity(act);
			}

			void RobotActivityEditWidget::renameActivity(int col)
			{
				bool ok;
				QString name;

				auto act = activities_[col];
				name = QInputDialog::getText(this, "Name", "New Name", QLineEdit::Normal, act->name(), &ok);
				if (!ok)
					return;

				emit renamedActivity(act->name(), name);
				act->setName(name);

				update();
			}

			void RobotActivityEditWidget::mouseDoubleClickEvent(QMouseEvent* ev)
			{
				int col = columnAt(ev->pos());
				if (col == -1 || col >= activities_.size())
					return;

				QRect b = columnBounds(col);
				if (ev->y() < b.top() + title_area_height_)
				{
					renameActivity(col);
				}
				else
				{
					int pattern = (ev->y() - b.top() - title_area_height_) / (image_size_ + between_item_border_);
					updateMinMaxCount(col, pattern);
				}
			}

			void RobotActivityEditWidget::mousePressEvent(QMouseEvent* ev)
			{
				if ((ev->modifiers() & (Qt::KeyboardModifier::ControlModifier | Qt::KeyboardModifier::ShiftModifier)) == 0)
					selected_.clear();

				int col = columnAt(ev->pos());
				if (col >= 0 && col < activities_.size())
				{
					QRect bounds = columnBounds(col);
					SelectedItem item;

					if (ev->y() < bounds.top() + title_area_height_)
					{
						item.activity_ = col;
						item.pattern_ = -1;
						item.type_ = SelectType::Activity;

						int i = selected_.size() - 1;
						while (i >= 0)
						{
							if (selected_[i].type_ == SelectType::Pattern && selected_[i].activity_ == col)
								selected_.erase(selected_.begin() + i);

							i--;
						}
					}
					else
					{
						item.activity_ = col;
						item.pattern_ = (ev->y() - bounds.top() - title_area_height_) / (image_size_ + between_item_border_);
						item.type_ = SelectType::Pattern;
					}
					selected_.push_back(item);
				}

				update();
			}

			void RobotActivityEditWidget::setActivities(std::vector<std::shared_ptr<const RobotActivity>> activities)
			{
				activities_.clear();
				for (auto a : activities)
				{
					activities_.push_back(std::const_pointer_cast<RobotActivity>(a));
				}

				setSize();
			}

			void RobotActivityEditWidget::setRegions(std::vector<std::shared_ptr<const FieldRegion>> regions)
			{
				regions_ = regions;
				setSize();
			}

			int RobotActivityEditWidget::calcColumnWidth()
			{
				QFontMetricsF fm(font());

				int maxwidth = 0;
				for (auto r : regions_)
				{
					int width = fm.horizontalAdvance(r->name()); 
					width += image_size_ + image_size_ + text_graphics_border_ + text_graphics_border_ + left_right_border_ + left_right_border_;

					if (width > maxwidth)
						maxwidth = width;
				}

				return maxwidth + column_left_right_border_ * 2;
			}

			void RobotActivityEditWidget::setSize()
			{
				column_width_ = calcColumnWidth();
				int width = (activities_.size() + 1) * column_width_ + left_right_border_ * 2 + between_column_spacing_ * activities_.size();
				int height = title_area_height_ + image_size_;

				for (auto a : activities_)
				{
					int aheight = title_area_height_ + a->patterns().size() * (image_size_ + between_item_border_);
					if (aheight > height)
						height = aheight;
				}

				height += top_bottom_border_ * 2;
				setMinimumHeight(height);
				setMaximumHeight(height);
				setMinimumWidth(width);
				setMaximumWidth(width);
			}

			void RobotActivityEditWidget::paintEvent(QPaintEvent* ev)
			{
				QPainter painter(this);

				int pos = left_right_border_;
				for (auto a : activities_)
				{
					paintActivity(painter, pos, a);
					pos += column_width_ + between_column_spacing_;
				}

				paintNewActivity(painter, pos);

				if (highlight_column_ != -1)
				{
					QRect bounds = columnBounds(highlight_column_);
					QBrush brush(QColor(255, 255, 32, 128), Qt::BrushStyle::Dense6Pattern);
					painter.setBrush(brush);
					painter.drawRect(bounds);
				}

				for (const SelectedItem& item : selected_)
				{
					if (item.type_ == SelectType::Activity)
						paintSelectedActivity(painter, item.activity_);
					else
						paintSelectedPattern(painter, item.activity_, item.pattern_);
				}
			}

			void RobotActivityEditWidget::paintSelectedPattern(QPainter& painter, int col, int pattern)
			{
				QRect r;
				painter.save();

				QFontMetrics fm(painter.font());

				QVector<QPoint> points;
				QRect b = columnBounds(col);

				int ypos = b.top() + title_area_height_ + pattern * (image_size_ + between_item_border_);

				QBrush br(QColor(255, 255, 32));
				painter.setBrush(br);

				QPen pen(QColor(0, 0, 0));
				painter.setPen(pen);

				r = QRect(b.left() - handle_size_ / 2, ypos - handle_size_ / 2, handle_size_, handle_size_);
				painter.drawRect(r);

				r = QRect(b.left() + column_width_ - handle_size_ / 2, ypos - handle_size_ / 2, handle_size_, handle_size_);
				painter.drawRect(r);

				r = QRect(b.left() - handle_size_ / 2, ypos + image_size_ - handle_size_ / 2, handle_size_, handle_size_);
				painter.drawRect(r);

				r = QRect(b.left() + column_width_ - handle_size_ / 2, ypos + image_size_ - handle_size_ / 2, handle_size_, handle_size_);
				painter.drawRect(r);

				painter.restore();
			}

			void RobotActivityEditWidget::paintSelectedActivity(QPainter& painter, int col)
			{
				QRect r;
				painter.save();

				QFontMetrics fm(painter.font());

				QRect b = columnBounds(col);

				QBrush br(QColor(255, 255, 32));
				painter.setBrush(br);

				QPen pen(QColor(0, 0, 0));
				painter.setPen(pen);

				QPoint pt = b.topLeft();
				r = QRect(pt.x() - handle_size_ / 2, pt.y() - handle_size_ / 2, handle_size_, handle_size_);
				painter.drawRect(r);

				pt = b.bottomLeft();
				r = QRect(pt.x() - handle_size_ / 2, pt.y() - handle_size_ / 2, handle_size_, handle_size_);
				painter.drawRect(r);

				pt = b.topRight();
				r = QRect(pt.x() - handle_size_ / 2, pt.y() - handle_size_ / 2, handle_size_, handle_size_);
				painter.drawRect(r);

				pt = b.bottomRight();
				r = QRect(pt.x() - handle_size_ / 2, pt.y() - handle_size_ / 2, handle_size_, handle_size_);
				painter.drawRect(r);

				painter.restore();
			}

			void RobotActivityEditWidget::paintTab(QPainter& painter, const QString & text, int xpos, int items)
			{
				painter.save();
				QFontMetrics fm(painter.font());
				QVector<QPoint> points;

				QPen pen(QColor(0, 0, 0));
				painter.setPen(pen);

				QBrush br(QColor(220, 200, 200));
				painter.setBrush(br);

				int height = image_size_ * items;
				if (items > 1)
					height += (items - 1) * between_item_border_;

				int twidth = fm.horizontalAdvance(text);
				int top = top_bottom_border_;
				points.push_back(QPoint(xpos, top));
				points.push_back(QPoint(xpos + twidth + tab_text_spacing * 2, top));
				points.push_back(QPoint(xpos + twidth + tab_text_spacing * 2, top + title_area_height_));
				points.push_back(QPoint(xpos + column_width_, top + title_area_height_));
				points.push_back(QPoint(xpos + column_width_, top + title_area_height_ + height));
				points.push_back(QPoint(xpos, top + title_area_height_ + height));
				points.push_back(QPoint(xpos, top));
				painter.drawPolygon(&points[0], points.size(), Qt::FillRule::OddEvenFill);

				QPoint pt(xpos + tab_text_spacing, top + fm.height());
				painter.drawText(pt, text);

				painter.restore();
			}

			void RobotActivityEditWidget::paintNewActivity(QPainter& painter, int xpos)
			{
				QString newtitle("New Activity");
				QString dragmsg("Drag Pattern Here ...");
				QFontMetrics fm(painter.font());

				painter.save();

				paintTab(painter, newtitle, xpos, 1);

				QPoint pt = QPoint(xpos + column_width_ / 2 - fm.horizontalAdvance(dragmsg) / 2, top_bottom_border_ + title_area_height_ + image_size_ / 2 + fm.descent());
				painter.drawText(pt, dragmsg);

				painter.restore();
			}

			void RobotActivityEditWidget::paintActivity(QPainter& painter, int xpos, std::shared_ptr<const RobotActivity> act)
			{
				QFontMetrics fm(painter.font());
				paintTab(painter, act->name(), xpos, act->count());

				QRect r;
				int ypos = top_bottom_border_ + title_area_height_;
				for (auto p : act->patterns())
				{
					std::shared_ptr<QImage> left, right;
					QString text;

					auto ee = std::dynamic_pointer_cast<const SequenceEnterExitPattern>(p);
					if (ee != nullptr)
					{
						text = ee->name();
						left = enter_image_;
						right = exit_image_;
					}

					auto ei = std::dynamic_pointer_cast<const SequenceEnterIdlePattern>(p);
					if (ei != nullptr)
					{
						text = ei->name();
						left = enter_image_;
						right = idle_image_;
					}

					r = QRect(xpos, ypos, image_size_, image_size_);
					painter.drawImage(r, *left.get());

					int itemwidth = fm.horizontalAdvance(text);
					QPoint pt(xpos + column_width_ / 2 - itemwidth / 2, ypos + image_size_ / 2 + fm.descent());
					painter.drawText(pt, text);

					r = QRect(xpos + column_width_ - left_right_border_ - image_size_, ypos, image_size_, image_size_);
					painter.drawImage(r, *right.get());

					QFont save = painter.font();
					QFont small = save;
					small.setPointSizeF(6.0);
					painter.setFont(small);

					QFontMetrics smallfm(small);
					QString num = "min="+QString::number(p->minCount());
					pt = QPoint(xpos + image_size_ + text_graphics_border_, ypos + image_size_ - smallfm.height());
					painter.drawText(pt, num);

					num = "max="+QString::number(p->maxCount());
					pt = QPoint(xpos + column_width_ - image_size_ - text_graphics_border_ - smallfm.horizontalAdvance(num) , ypos + image_size_ - smallfm.height());
					painter.drawText(pt, num);

					painter.setFont(save);

					ypos += image_size_ + between_item_border_;
				}
			}

			QRect RobotActivityEditWidget::columnBounds(int col)
			{
				int width = column_width_;
				int y = top_bottom_border_;
				int x = left_right_border_ + column_width_ * col;
				if (col > 0)
					x += between_column_spacing_* col ;
				int height;

				if (col == activities_.size())
				{
					height = title_area_height_ + image_size_;
				}
				else
				{
					assert(col < activities_.size());
					auto act = activities_[col];
					height = title_area_height_ + act->patterns().size() * image_size_;
				}

				return QRect(x, y, width, height);
			}

			int RobotActivityEditWidget::columnAt(const QPoint& pos)
			{
				if (pos.x() < left_right_border_ || pos.y() < top_bottom_border_)
					return -1;

				for (int i = 0; i <= activities_.size(); i++)
				{
					QRect r = columnBounds(i);
					if (r.contains(pos))
						return i;
				}

				return -1;
			}

			void RobotActivityEditWidget::dragEnterEvent(QDragEnterEvent* ev)
			{
				if (ev->mimeData()->hasText())
				{
					ev->accept();
				}
			}

			void RobotActivityEditWidget::dragMoveEvent(QDragMoveEvent* ev)
			{
				int col = columnAt(ev->pos());
				if (col != -1)
				{
					ev->accept();
					ev->setDropAction(Qt::CopyAction);
					highlight_column_ = col;
				}
				else
				{
					ev->ignore();
					highlight_column_ = -1;
				}
				update();
			}

			void RobotActivityEditWidget::dragLeaveEvent(QDragLeaveEvent* ev)
			{
				highlight_column_ = -1;
			}

			std::shared_ptr<const RobotActivity> RobotActivityEditWidget::findActivityByName(const QString& name)
			{
				for (auto a : activities_)
				{
					if (a->name() == name)
						return a;
				}

				return nullptr;
			}

			std::shared_ptr<const FieldRegion> RobotActivityEditWidget::findRegionByName(const QString& name)
			{
				for (auto r : regions_)
				{
					if (r->name() == name)
						return r;
				}

				return nullptr;
			}


			QString RobotActivityEditWidget::newActivityName()
			{
				QString name;
				int i = 1;

				while (true)
				{
					name = "Activity-" + QString::number(i);
					if (findActivityByName(name) == nullptr)
						break;
					i++;
				}

				return name;
			}

			void RobotActivityEditWidget::createNewActivity(const QString& text)
			{
				QString name = newActivityName();
				std::shared_ptr<RobotActivity> act = std::make_shared<RobotActivity>(name);

				activities_.push_back(act);

				blockSignals(true);
				//
				// Do now send a changed signal when we add the pattern as we are adding a new
				// activity and want to send the addedActivity signal
				//
				addNewPattern(activities_.size() - 1, text);
				blockSignals(false);

				emit addedActivity(act);
			}

			bool RobotActivityEditWidget::isRegionPerAlliance(const QString& name)
			{
				QString red = toString(Alliance::Red) + "-" + name;
				QString blue = toString(Alliance::Blue) + "-" + name;

				return findRegionByName(red) != nullptr && findRegionByName(blue) != nullptr;
			}

			void RobotActivityEditWidget::addNewPattern(int col, const QString& text)
			{
				auto act = activities_[col];
				int index = text.indexOf(':');
				assert(index != -1);

				QString type = text.left(index);
				QString region = text.mid(index + 1);

				bool per = isRegionPerAlliance(region);
				std::shared_ptr<SequencePattern> p;

				if (type == "EnterExit")
				{
					p = std::make_shared<SequenceEnterExitPattern>(region, 1, 1, per);
				}
				else if (type == "EnterIdle")
				{
					double d = QInputDialog::getDouble(this, "Idle Period", "Idle Period", 1.0);
					p = std::make_shared<SequenceEnterIdlePattern>(region, d, 1, 1, per);
				}
				else
				{
					assert(false);
				}

				act->addPattern(p);
				emit changedActivity(act);
			}

			void RobotActivityEditWidget::dropEvent(QDropEvent* ev)
			{
				int col = columnAt(ev->pos());
				QString pattern = ev->mimeData()->text();

				if (col != -1)
				{
					if (col == activities_.size())
					{
						createNewActivity(pattern);
					}
					else
					{
						addNewPattern(col, pattern);
					}

					ev->setDropAction(Qt::CopyAction);
					ev->accept();

					setSize();
				}
				else
				{
					ev->ignore();
				}

				highlight_column_ = -1;
				update();
			}
		}
	}
}
