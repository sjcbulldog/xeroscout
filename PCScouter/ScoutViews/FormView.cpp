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
#include <QBoxLayout>
#include <QSpinBox>
#include <cassert>

using namespace xero::scouting::datamodel;

namespace xero
{
	namespace scouting
	{
		namespace views
		{
			FormView::FormView(ImageManager& images, QString name, QString title, QColor titlec, QWidget* parent) : QWidget(parent), ViewBase("FormView"), images_(images)
			{
				name_ = name;
				title_txt_ = title;
				instance_ = nullptr;

				QVBoxLayout* lay = new QVBoxLayout();
				setLayout(lay);

				QWidget* titlestrip = new QWidget();
				QHBoxLayout* hlay = new QHBoxLayout();
				titlestrip->setLayout(hlay);

				smaller_ = new QPushButton("-", titlestrip);
				QFont bf = smaller_->font();
				bf.setPointSizeF(22);
				bf.setBold(true);
				smaller_->setFont(bf);
				hlay->addWidget(smaller_);

				titles_ = new QLabel(title, titlestrip);
				titles_->setAlignment(Qt::AlignCenter);
				QPalette pal = titles_->palette();
				pal.setColor(titles_->foregroundRole(), titlec);
				titles_->setPalette(pal);
				hlay->addWidget(titles_);
				QSizePolicy p(QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);
				titles_->setSizePolicy(p);

				bigger_ = new QPushButton("+", titlestrip);
				bigger_->setFont(bf);
				hlay->addWidget(bigger_);

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

				connect(bigger_, &QPushButton::pressed, this, &FormView::plus);
				connect(smaller_, &QPushButton::pressed, this, &FormView::minus);
			}

			FormView::~FormView()
			{
			}

			void FormView::plus()
			{
				setScale(1.1);
			}

			void FormView::minus()
			{
				setScale(0.9);
			}

			void FormView::setScale(double s)
			{
				auto& sections = form_->sections();
				for (auto section : sections) {
					for (auto item : section->items()) {
						auto disp = instance_->displayItem(item->tag());
						if (disp != nullptr)
							disp->setScale(s);
					}
				}
			}

			void FormView::createSection(std::shared_ptr<const FormSection> section)
			{
				QWidget* tab = new QWidget(this);
				FlowLayout* layout = new FlowLayout();
				tab->setLayout(layout);

				for (auto item : section->items()) {
					if (alliance_.length() == 0 || item->alliance() == alliance_ || item->alliance().length() == 0)
					{
						FormItemDisplay* w = item->createDisplay(images_, this);
						layout->addWidget(w);
						instance_->addDisplayItem(item->tag(), w);
					}
				}

				tabs_->addTab(tab, section->name());
			}

			void FormView::setScoutingForm(std::shared_ptr<const ScoutingForm> form, const QString &alliance)
			{
				alliance_ = alliance;

				clearView();

				form_ = form;
				instance_ = std::make_shared<FormInstance>(form);
				auto& sections = form_->sections();
				for (auto section : sections) {
					createSection(section);
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
