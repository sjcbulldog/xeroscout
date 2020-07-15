#include "DataSetRulesEditor.h"
#include "DataSetFieldPopup.h"
#include "FieldItemDelegate.h"
#include "DataSetExprContext.h"
#include "Expr.h"
#include <QListWidget>
#include <QPushButton>
#include <QPixmap>
#include <QPainter>
#include <QColorDialog>
#include <QKeyEvent>
#include <set>

using namespace xero::scouting::datamodel;
using namespace xero::expr;

namespace xero
{
	namespace scouting
	{
		namespace views
		{
			DataSetRulesEditor::DataSetRulesEditor(const std::list<std::shared_ptr<const DataSetHighlightRules>>& rules, ScoutingDataSet &set, QWidget* parent) : QDialog(parent), data_(set)
			{
				ui.setupUi(this);

				// Make a copy of the rules to edit
				for (auto r : rules)
				{
					auto rule = std::make_shared<DataSetHighlightRules>(*r);
					rules_.push_back(rule);

					QListWidgetItem* item = new QListWidgetItem(rule->name());
					item->setFlags(Qt::ItemFlag::ItemIsEnabled | Qt::ItemFlag::ItemIsSelectable | Qt::ItemFlag::ItemIsEditable);
					ui.all_rules_->addItem(item);
				}

				connect(ui.new_, &QPushButton::pressed, this, &DataSetRulesEditor::newRule);
				connect(ui.delete_, &QPushButton::pressed, this, &DataSetRulesEditor::deleteRule);
				connect(ui.fore_, &QPushButton::pressed, this, &DataSetRulesEditor::foregroundColor);
				connect(ui.back_, &QPushButton::pressed, this, &DataSetRulesEditor::backgroundColor);
				connect(ui.desc_, &QLineEdit::textEdited, this, &DataSetRulesEditor::descTextChanged);
				connect(ui.equation_, &QLineEdit::textEdited, this, &DataSetRulesEditor::equTextChanged);
				connect(ui.fields_, &QListWidget::itemChanged, this, &DataSetRulesEditor::selectChanged);
				connect(ui.highlights_, &QListWidget::itemChanged, this, &DataSetRulesEditor::highlightChanged);
				connect(ui.all_rules_, &QListWidget::currentRowChanged, this, &DataSetRulesEditor::ruleRowChanged);
				connect(ui.fields_, &QListWidget::itemDoubleClicked, this, &DataSetRulesEditor::fieldDoubleClicked);
				connect(ui.highlights_, &QListWidget::itemDoubleClicked, this, &DataSetRulesEditor::highlightDoubleClicked);
				connect(ui.all_rules_, &QListWidget::itemChanged, this, &DataSetRulesEditor::ruleNameChanged);

				QPushButton* b;

				b = ui.buttons_->button(QDialogButtonBox::StandardButton::Ok);
				connect(b, &QPushButton::pressed, this, &QDialog::accept);

				b = ui.buttons_->button(QDialogButtonBox::StandardButton::Cancel);
				connect(b, &QPushButton::pressed, this, &QDialog::reject);

				ui.fields_->installEventFilter(this);
				ui.highlights_->installEventFilter(this);

				ui.fields_->setItemDelegate(new FieldItemDelegate(set));
				ui.highlights_->setItemDelegate(new FieldItemDelegate(set));

				QFont font = ui.matched_count_->font();
				font.setPointSizeF(5.0);
				ui.matched_count_->setFont(font);

				font = ui.group_size_->font();
				font.setPointSizeF(5.0);
				ui.group_size_->setFont(font);

				font = ui.equ_error_->font();
				font.setPointSizeF(5.0);
				ui.equ_error_->setFont(font);

				ui.matched_count_->setText("No Fields  ");
				ui.group_size_->setText("No Fields  ");

				ui.equation_->setWordList(set.fieldNames());

				ui.desc_->setEnabled(false);
				ui.equation_->setEnabled(false);
				ui.fields_->setEnabled(false);
				ui.highlights_->setEnabled(false);
				ui.fore_->setEnabled(false);
				ui.back_->setEnabled(false);

				if (ui.all_rules_->count() > 0)
					ui.all_rules_->setCurrentRow(0);
			}

			DataSetRulesEditor::~DataSetRulesEditor()
			{
			}

			void DataSetRulesEditor::keyPressEvent(QKeyEvent* evt)
			{
				if (evt->key() == Qt::Key_Enter || evt->key() == Qt::Key_Return)
					return;

				QDialog::keyPressEvent(evt);
			}

			void DataSetRulesEditor::addField(QListWidget* list)
			{
				//
				// Remove the item that says "add new item"
				//
				QListWidgetItem* add = list->takeItem(list->count() - 1);

				//
				// Create a new item at the end of the list
				//
				QListWidgetItem *item = new QListWidgetItem("");
				item->setFlags(Qt::ItemFlag::ItemIsEnabled | Qt::ItemFlag::ItemIsSelectable | Qt::ItemFlag::ItemIsEditable);
				list->addItem(item);

				list->addItem(add);
				list->editItem(item);
			}

			void DataSetRulesEditor::fieldDoubleClicked(QListWidgetItem* item)
			{
				if (item->text() == addNewFieldText)
					addField(ui.fields_);
			}

			void DataSetRulesEditor::highlightDoubleClicked(QListWidgetItem* item)
			{
				if (item->text() == addNewFieldText)
					addField(ui.highlights_);
			}

			void DataSetRulesEditor::ruleNameChanged(QListWidgetItem* item)
			{
				if (current_ >= 0 && current_ < rules_.size())
				{
					rules_[current_]->setName(item->text());
				}
			}

			void DataSetRulesEditor::ruleRowChanged(int row)
			{
				current_ = row;
				if (current_ >= rules_.size())
					current_ = static_cast<int>(rules_.size() - 1);
				initRule();
			}

			void DataSetRulesEditor::setCombinationsAndSizes()
			{
				std::vector<std::vector<QVariant>> combinations;
				std::map<int, std::vector<int>> rowgroups;

				data_.getCombinations(rules_[current_]->fields(), combinations, rowgroups);

				std::set<int> sizes;
				for (int i = 0; i < rowgroups.size(); i++) {
					sizes.insert(static_cast<int>(rowgroups[i].size()));
				}

				ui.matched_count_->setText("Groups: " + QString::number(combinations.size()) + "  ");
				QString txt = "Sizes:";
				for (auto n : sizes)
					txt += " " + QString::number(n);

				txt += "  ";
				ui.group_size_->setText(txt);
			}

			void DataSetRulesEditor::selectChanged(QListWidgetItem* item)
			{
				if (current_ >= 0 && current_ < rules_.size())
				{
					QStringList list;

					for (int i = 0; i < ui.fields_->count(); i++)
					{
						auto item = ui.fields_->item(i);
						if (item->text() != addNewFieldText)
							list.push_back(item->text());
					}

					rules_[current_]->setFields(list);

					setCombinationsAndSizes();
				}
			}

			void DataSetRulesEditor::highlightChanged(QListWidgetItem* item)
			{
				if (current_ >= 0 && current_ < rules_.size())
				{
					QStringList list;

					for (int i = 0; i < ui.highlights_->count(); i++)
					{
						auto item = ui.highlights_->item(i);
						if (item->text() != addNewFieldText)
							list.push_back(item->text());
					}

					rules_[current_]->setHighlights(list);
				}
			}

			void DataSetRulesEditor::descTextChanged(const QString &text)
			{
				if (current_ >= 0 && current_ < rules_.size())
					rules_[current_]->setDescriptor(text);
			}

			void DataSetRulesEditor::setEquationError()
			{
				std::vector<std::vector<QVariant>> combinations;
				std::map<int, std::vector<int>> rowgroups;
				data_.getCombinations(rules_[current_]->fields(), combinations, rowgroups);

				DataSetExprContext context(data_, rowgroups[0]);
				Expr expr;

				QString err;
				if (!expr.parse(context, rules_[current_]->equation(), err))
				{
					ui.equ_error_->setText(err);
				}
				else
				{
					ui.equ_error_->setText("");
				}
			}

			void DataSetRulesEditor::equTextChanged(const QString& text)
			{
				if (current_ >= 0 && current_ < rules_.size())
				{
					rules_[current_]->setEquation(text);

					setEquationError();
				}
			}

			bool DataSetRulesEditor::eventFilter(QObject* obj, QEvent* ev)
			{
				QListWidget* list = dynamic_cast<QListWidget*>(obj);
				if (list != nullptr)
				{
					if (ev->type() == QEvent::KeyPress)
					{
						QKeyEvent* keyev = dynamic_cast<QKeyEvent*>(ev);
						if (keyev != nullptr)
						{
							if (keyev->key() == Qt::Key::Key_Delete)
							{
								QModelIndex index = list->currentIndex();
								QListWidgetItem* item = list->takeItem(index.row());
								delete item;
								return true;
							}
						}
					}
				}
				return QObject::eventFilter(obj, ev);
			}

			void DataSetRulesEditor::deleteRule()
			{
				if (current_ >= 0 && current_ < rules_.size())
				{
					//
					// Remove rule from rules_ vector
					//
					rules_.erase(rules_.begin() + current_);

					//
					// Remove rule from rules window
					//
					auto item = ui.all_rules_->takeItem(current_);
					delete item;
				}
			}

			void DataSetRulesEditor::foregroundColor()
			{
				if (current_ >= 0 && current_ < rules_.size())
				{
					QColor c = QColorDialog::getColor(rules_[current_]->foreground(), this, "Foreground Color");
					rules_[current_]->setForeground(c);

					updateSample(rules_[current_]->foreground(), rules_[current_]->background());
				}
			}

			void DataSetRulesEditor::backgroundColor()
			{
				if (current_ >= 0 && current_ < rules_.size())
				{
					QColor c = QColorDialog::getColor(rules_[current_]->foreground(), this, "Background Color");
					rules_[current_]->setBackground(c);

					updateSample(rules_[current_]->foreground(), rules_[current_]->background());
				}
			}

			void DataSetRulesEditor::newRule()
			{
				QString name;
				int i = 1;
				while (true)
				{
					name = "NewRule_" + QString::number(i);
					if (ui.all_rules_->findItems(name, Qt::MatchFlag::MatchExactly).size() == 0)
						break;

					i++;
				}

				QStringList empty;
				auto rule = std::make_shared<DataSetHighlightRules>(name, empty, empty, "", QColor(255, 0, 0), QColor(255, 255, 255), "New Rule Description");
				rules_.push_back(rule);

				QListWidgetItem* item = new QListWidgetItem(rule->name());
				item->setFlags(Qt::ItemFlag::ItemIsEnabled | Qt::ItemFlag::ItemIsSelectable | Qt::ItemFlag::ItemIsEditable);
				ui.all_rules_->addItem(item);

				ui.all_rules_->setCurrentItem(item);

				current_ = static_cast<int>(rules_.size() - 1);
				initRule();
			}

			void DataSetRulesEditor::initRule()
			{
				if (current_ == -1)
				{
					ui.desc_->clear();
					ui.equation_->clear();
					ui.fields_->clear();
					ui.highlights_->clear();
					updateSample(QColor(0, 0, 0), QColor(255, 255, 255));

					ui.desc_->setEnabled(false);
					ui.equation_->setEnabled(false);
					ui.fields_->setEnabled(false);
					ui.highlights_->setEnabled(false);
					ui.fore_->setEnabled(false);
					ui.back_->setEnabled(false);
				}
				else
				{
					ui.desc_->setEnabled(true);
					ui.equation_->setEnabled(true);
					ui.fields_->setEnabled(true);
					ui.highlights_->setEnabled(true);
					ui.fore_->setEnabled(true);
					ui.back_->setEnabled(true);

					QListWidgetItem* item;
					auto rule = rules_[current_];
					ui.desc_->setText(rule->descriptor());
					ui.equation_->setText(rule->equation());

					ui.fields_->clear();
					for (auto s : rule->fields())
					{
						item = new QListWidgetItem(s);
						ui.fields_->addItem(item);
					}
					item = new QListWidgetItem(addNewFieldText);
					item->setFlags(Qt::ItemFlag::ItemIsEnabled);
					ui.fields_->addItem(item);

					ui.highlights_->clear();
					for (auto h : rule->highlights())
					{
						item = new QListWidgetItem(h);
						ui.highlights_->addItem(item);
					}
					item = new QListWidgetItem(addNewFieldText);
					item->setFlags(Qt::ItemFlag::ItemIsEnabled);
					ui.highlights_->addItem(item);

					updateSample(rule->foreground(), rule->background());
					setCombinationsAndSizes();
					setEquationError();
				}
			}

			void DataSetRulesEditor::updateSample(QColor fore, QColor back)
			{
				int width = ui.sample_->width();
				int height = ui.sample_->height();
				QPixmap p(width, height);
				QPainter paint(&p);
				QString txt = "Sample";

				ui.sample_->setText("");
				paint.setPen(Qt::PenStyle::NoPen);
				paint.setBrush(QBrush(back));
				paint.drawRect(0, 0, p.width(), p.height());

				paint.setFont(ui.sample_->font());
				QFontMetrics fm(paint.font());

				paint.setPen(QPen(fore));
				QPoint pt(p.width() / 2 - fm.horizontalAdvance(txt) / 2, p.height() / 2 + fm.height() / 2);
				paint.drawText(pt, txt);

				ui.sample_->setPixmap(p);

			}
		}
	}
}
