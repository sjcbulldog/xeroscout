#include "GraphConfigurationDialog.h"
#include <QBoxLayout>
#include <QPushButton>

using namespace xero::scouting::datamodel;

namespace xero
{
	namespace scouting
	{
		namespace views
		{
			GraphConfigurationDialog::GraphConfigurationDialog(const GraphDescriptor& desc) : desc_(desc)
			{
				QVBoxLayout* lay = new QVBoxLayout();
				setLayout(lay);

				which_ = new QComboBox(this);
				lay->addWidget(which_);

				holder_ = new QWidget(this);
				lay->addWidget(holder_);

				QSizePolicy p(QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Expanding);
				holder_->setSizePolicy(p);

				QHBoxLayout* hlay = new QHBoxLayout();
				holder_->setLayout(hlay);

				src_ = new QListWidget(holder_);
				hlay->addWidget(src_);

				graph_ = new QListWidget(holder_);
				hlay->addWidget(graph_);

				QDialogButtonBox::StandardButtons buttons = QDialogButtonBox::Ok | QDialogButtonBox::Cancel;
				buttons_ = new QDialogButtonBox(buttons, this);
				lay->addWidget(buttons_);

				QPushButton* b;
				b = buttons_->button(QDialogButtonBox::Ok);
				connect(b, &QPushButton::pressed, this, &QDialog::accept);

				b = buttons_->button(QDialogButtonBox::Cancel);
				connect(b, &QPushButton::pressed, this, &QDialog::reject);
			}

			GraphConfigurationDialog::~GraphConfigurationDialog()
			{
			}
		}
	}
}
