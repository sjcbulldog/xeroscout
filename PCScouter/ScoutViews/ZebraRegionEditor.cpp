#include "ZebraRegionEditor.h"
#include "PolygonFieldRegion.h"
#include "CircleFieldRegion.h"
#include "RectFieldRegion.h"
#include <QBoxLayout>
#include <QMenu>
#include <QInputDialog>
#include <QMessageBox>

using namespace xero::scouting::datamodel;

namespace xero
{
	namespace scouting
	{
		namespace views
		{
			ZebraRegionEditor::ZebraRegionEditor(GameFieldManager &mgr, const QString &year, QWidget* parent) : FieldBasedWidget(mgr, parent), ViewBase("RegionEditor")
			{
				QVBoxLayout* vlay = new QVBoxLayout();
				setLayout(vlay);

				auto f = new PathFieldView(this);
				setField(f, year);

				connect(field(), &PathFieldView::showContextMenu, this, &ZebraRegionEditor::fieldContextMenu);
				connect(field(), &PathFieldView::keyPressed, this, &ZebraRegionEditor::fieldKeyPressed);
				field()->setViewMode(PathFieldView::ViewMode::Track);
				vlay->addWidget(field());


				field()->setViewMode(PathFieldView::ViewMode::Editor);
			}

			ZebraRegionEditor::~ZebraRegionEditor()
			{
			}

			void ZebraRegionEditor::setYear(const QString& year)
			{
				setField(field(), year);
			}

			void ZebraRegionEditor::clearView()
			{
			}

			void ZebraRegionEditor::refreshView()
			{
			}

			void ZebraRegionEditor::madeActive()
			{
				field()->clearHighlights();
				for (auto h : dataModel()->fieldRegions())
					field()->addHighlight(h);
			}

			void ZebraRegionEditor::fieldContextMenu(QPoint pt)
			{
				QAction* act;

				menu_point_ = pt;

				QMenu* menu = new QMenu("Field");
				QMenu* all = new QMenu("Add Region");
				menu->addMenu(all);

				QMenu* both = new QMenu("Both Alliances");

				auto cb = std::bind(&ZebraRegionEditor::addHighlight, this, Alliance::Both, HighlightType::Circle);
				act = both->addAction("Add Circular Area");
				connect(act, &QAction::triggered, cb);

				cb = std::bind(&ZebraRegionEditor::addHighlight, this, Alliance::Both, HighlightType::Rectangle);
				act = both->addAction("Add Rectangular Area");
				connect(act, &QAction::triggered, cb);

				cb = std::bind(&ZebraRegionEditor::addHighlight, this, Alliance::Both, HighlightType::Polygon);
				act = both->addAction("Add Polygon Area");
				connect(act, &QAction::triggered, cb);

				all->addMenu(both);

				QMenu* red = new QMenu("Red Alliance");

				cb = std::bind(&ZebraRegionEditor::addHighlight, this, Alliance::Red, HighlightType::Circle);
				act = red->addAction("Add Circular Area");
				connect(act, &QAction::triggered, cb);

				cb = std::bind(&ZebraRegionEditor::addHighlight, this, Alliance::Red, HighlightType::Rectangle);
				act = red->addAction("Add Rectangular Area");
				connect(act, &QAction::triggered, cb);

				cb = std::bind(&ZebraRegionEditor::addHighlight, this, Alliance::Red, HighlightType::Polygon);
				act = red->addAction("Add Polygon Area");
				connect(act, &QAction::triggered, cb);

				all->addMenu(red);

				QMenu* blue = new QMenu("Blue Alliance");

				cb = std::bind(&ZebraRegionEditor::addHighlight, this, Alliance::Blue, HighlightType::Circle);
				act = blue->addAction("Add Circular Area");
				connect(act, &QAction::triggered, cb);

				cb = std::bind(&ZebraRegionEditor::addHighlight, this, Alliance::Blue, HighlightType::Rectangle);
				act = blue->addAction("Add Rectangular Area");
				connect(act, &QAction::triggered, cb);

				cb = std::bind(&ZebraRegionEditor::addHighlight, this, Alliance::Blue, HighlightType::Polygon);
				act = blue->addAction("Add Polygon Area");
				connect(act, &QAction::triggered, cb);

				all->addMenu(blue);

				if (field()->isOneSelected())
				{
					act = all->addAction("Rename Current Region");
					connect(act, &QAction::triggered, this, &ZebraRegionEditor::renameRegion);
				}

				int count = 0;
				QMenu* remove = new QMenu("Remove");
				QAction* first = nullptr;
				for (auto h : dataModel()->fieldRegions())
				{
					act = remove->addAction(h->name());
					if (first == nullptr)
						first = act;
					auto removedb = std::bind(&ZebraRegionEditor::removeHighlight, this, h->name());
					connect(act, &QAction::triggered, removedb);
					count++;
				}

				if (count > 1)
				{
					act = new QAction("*** ALL ***");
					remove->insertAction(first, act);
					remove->insertSeparator(first);
					connect(act, &QAction::triggered, this, &ZebraRegionEditor::removeAllRegions);
				}

				if (count)
					menu->addMenu(remove);

				menu->exec(pt);
			}

			void ZebraRegionEditor::fieldKeyPressed(Qt::Key key)
			{
				if (key == Qt::Key::Key_Delete)
				{
					auto list = field()->selectedRegions();
					for (auto r : list)
					{
						field()->removeHighlight(r);
						dataModel()->removeFieldRegion(r);
					}
				}
			}

			void ZebraRegionEditor::renameRegion()
			{
				bool ok;

				disconnect(field_connect_);

				QString name = QInputDialog::getText(this, "Name", "Name", QLineEdit::Normal, "", &ok);
				if (ok)
				{
					if (!validRegionName(name))
					{
						QMessageBox::critical(this, "Error", "Invalid region name, must be all letters");
						return;
					}
					auto region = field()->selectedRegions().front();
					dataModel()->renameFieldRegion(region, name);
					field()->update();
				}
			}

			void ZebraRegionEditor::polygonSelected(const std::vector<QPointF>& points, Alliance a)
			{
				bool ok;

				disconnect(field_connect_);

				QString label = "Name";
				if (a != Alliance::Both)
					label = "Name: " + toString(a) + "-";

				QString name = QInputDialog::getText(this, "Name", label, QLineEdit::Normal, "", &ok);
				if (ok)
				{
					if (!validRegionName(name))
					{
						QMessageBox::critical(this, "Error", "Invalid region name, must be all letters");
						return;
					}
					QColor c = QColor(242, 245, 66, 128);

					if (a == Alliance::Red)
						c = QColor(255, 0, 0, 128);
					else if (a == Alliance::Blue)
						c = QColor(0, 0, 255, 128);

					if (a != Alliance::Both)
						name = toString(a) + "-" + name;

					auto h = std::make_shared<PolygonFieldRegion>(name, c, points, a);
					if (!dataModel()->addFieldRegion(h))
					{
						QMessageBox::critical(this, "Error", "Cannot add region, a region with the name '" + name + "' already exists");
						return;
					}
					field()->addHighlight(h);
				}
			}

			bool ZebraRegionEditor::validRegionName(const QString& name)
			{
				for (auto& ch : name)
				{
					if (!ch.isLetter())
						return false;
				}

				return true;
			}

			void ZebraRegionEditor::areaSelected(const QRectF& area, Alliance a, HighlightType ht)
			{
				bool ok;

				disconnect(field_connect_);

				QString label = "Name";
				if (a != Alliance::Both)
					label = "Name: " + toString(a) + "-";

				QString name = QInputDialog::getText(this, "Name", label, QLineEdit::Normal, "", &ok);
				if (ok)
				{
					if (!validRegionName(name))
					{
						QMessageBox::critical(this, "Error", "Invalid region name, must be all letters");
						return;
					}

					QColor c = QColor(242, 245, 66, 128);

					if (a == Alliance::Red)
						c = QColor(255, 0, 0, 128);
					else if (a == Alliance::Blue)
						c = QColor(0, 0, 255, 128);

					if (a != Alliance::Both)
						name = toString(a) + "-" + name;

					if (ht == HighlightType::Circle)
					{
						qDebug() << "ZebraViewWidget::areaSelected " << area;
						auto h = std::make_shared<CircleFieldRegion>(name, c, area.center(), area.width() / 2, a);
						if (!dataModel()->addFieldRegion(h))
						{
							QMessageBox::critical(this, "Error", "Cannot add region, a region with the name '" + name + "' already exists");
							return;
						}
						field()->addHighlight(h);
					}
					else
					{
						qDebug() << "ZebraViewWidget::areaSelected " << area;
						auto h = std::make_shared<RectFieldRegion>(name, c, area, a);
						if (!dataModel()->addFieldRegion(h))
						{
							QMessageBox::critical(this, "Error", "Cannot add region, a region with the name '" + name + "' already exists");
							return;
						}
						field()->addHighlight(h);
					}
				}
			}

			void ZebraRegionEditor::removeAllRegions()
			{
				while (dataModel()->fieldRegions().size() > 0)
				{
					auto h = dataModel()->fieldRegions().front();
					dataModel()->removeFieldRegion(h);
					field()->removeHighlight(h);
				}
			}

			void ZebraRegionEditor::removeHighlight(const QString& name)
			{
				std::shared_ptr<const FieldRegion> region = nullptr;

				for (auto h : dataModel()->fieldRegions())
				{
					if (h->name() == name)
					{
						region = h;
						break;
					}
				}

				if (region != nullptr)
				{
					dataModel()->removeFieldRegion(region);
					field()->removeHighlight(region);
				}
			}

			void ZebraRegionEditor::addHighlight(Alliance a, HighlightType ht)
			{
				QPointF fpt = field()->globalToWorld(menu_point_);

				if (ht == HighlightType::Circle)
				{
					field()->selectCircularArea();
					auto cb = std::bind(&ZebraRegionEditor::areaSelected, this, std::placeholders::_1, a, ht);
					field_connect_ = connect(field(), &PathFieldView::areaSelected, cb);
				}
				else if (ht == HighlightType::Rectangle)
				{
					field()->selectRectArea();
					auto cb = std::bind(&ZebraRegionEditor::areaSelected, this, std::placeholders::_1, a, ht);
					field_connect_ = connect(field(), &PathFieldView::areaSelected, cb);
				}
				else if (ht == HighlightType::Polygon)
				{
					field()->selectPolygonArea();
					auto cb = std::bind(&ZebraRegionEditor::polygonSelected, this, std::placeholders::_1, a);
					field_connect_ = connect(field(), &PathFieldView::polySelected, cb);
				}
			}

		}
	}
}