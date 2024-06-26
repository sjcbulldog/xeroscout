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

#include "FormView.h"
#include "FlowLayout.h"
#include <QtWidgets/QBoxLayout>
#include <QtWidgets/QSpinBox>
#include <QtWidgets/QGridLayout>
#include <cassert>

using namespace xero::scouting::datamodel;

namespace xero
{
	namespace scouting
	{
		namespace views
		{
			FormView::FormView(ImageManager& images, QString name, QString title, QColor titlec, FormView::FormType formtype, Alliance a, QWidget* parent) : QWidget(parent), ViewBase("FormView"), images_(images)
			{
				name_ = name;
				title_txt_ = title;
				instance_ = nullptr;
				formtype_ = formtype;
				alliance_ = a;

				QVBoxLayout* lay = new QVBoxLayout();
				setLayout(lay);

				QWidget* titlestrip = new QWidget();
				QHBoxLayout* hlay = new QHBoxLayout();
				titlestrip->setLayout(hlay);

				titles_ = new QLabel(title, titlestrip);
				titles_->setAlignment(Qt::AlignCenter);
				QPalette pal = titles_->palette();
				pal.setColor(titles_->foregroundRole(), titlec);
				titles_->setPalette(pal);
				hlay->addWidget(titles_);
				QSizePolicy p(QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);
				titles_->setSizePolicy(p);

				lay->addWidget(titlestrip);

				QFont f = titles_->font();
				f.setUnderline(true);
				f.setBold(true);
				f.setPointSizeF(16.0);
				titles_->setFont(f);

				tabs_ = new QTabWidget(this);
				lay->addWidget(tabs_);

				f = tabs_->font();
				f.setPointSizeF(18.0);
				tabs_->setFont(f);
			}

			FormView::~FormView()
			{
			}

			void FormView::createFlowSection(std::shared_ptr<const FormSection> section)
			{
				QWidget* tab = new QWidget(this);
				FlowLayout* layout = new FlowLayout();
				tab->setLayout(layout);

				for (auto item : section->items()) {
					if (item->alliance() == Alliance::Both || item->alliance() == alliance_)
					{
						FormItemDisplay* w = item->createDisplay(images_, this);
						if (item->hasSize()) {
							w->setFixedSize(item->width(), item->height());
						}
						layout->addWidget(w);
						instance_->addDisplayItem(item->tag(), w);
					}
				}

				tabs_->addTab(tab, section->name());
			}

			void FormView::createGridSection(std::shared_ptr<const FormSection> section)
			{
				QWidget* tab = new QWidget(this);
				QGridLayout* layout = new QGridLayout();
				tab->setLayout(layout);

				for (auto item : section->items()) {
					if (item->alliance() == Alliance::Both || item->alliance() == alliance_)
					{
						FormItemDisplay* w = item->createDisplay(images_, this);
						if (item->hasSize()) {
							w->setFixedSize(item->width(), item->height());
						}

						if (item->hasPos()) {
							layout->addWidget(w, item->row() - 1, item->col() - 1);
						}

						instance_->addDisplayItem(item->tag(), w);
					}
				}

				tabs_->addTab(tab, section->name());
			}

			void FormView::setScoutingForm(std::shared_ptr<const ScoutingForm> form)
			{
				clearView();
				form_ = form;

				if (form_ != nullptr)
				{
					instance_ = std::make_shared<FormInstance>(form);
					const auto& sections = form_->sections();
					for (auto section : sections) {
						if (section->type() == "grid") {
							createGridSection(section);
						}
						else {
							createFlowSection(section);
						}
					}
				}
			}

			void FormView::refreshView()
			{
				if (form_ == nullptr)
				{
					std::shared_ptr<const ScoutingForm> form;

					if (dataModel() != nullptr)
					{
						if (formtype_ == FormType::Team)
						{
							form = dataModel()->teamScoutingForm();
						}
						else if (formtype_ == FormType::Match)
						{
							form = dataModel()->matchScoutingForm();
						}
						else
						{
							assert(false);
						}
					}

					setScoutingForm(form);
				}
			}

			void FormView::clearView()
			{
				if (instance_ != nullptr)
					instance_->clear();

				for (int i = 0; i < tabs_->count(); i++) {
					delete tabs_->widget(i);
				}
				tabs_->clear();
			}

			void FormView::extractData(ScoutingDataMapPtr ptr)
			{
				instance_->extract(ptr);
			}

			void FormView::assignData(ConstScoutingDataMapPtr ptr)
			{
				instance_->assign(ptr);
			}
		}
	}
}
