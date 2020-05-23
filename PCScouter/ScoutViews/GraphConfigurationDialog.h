#pragma once

#include "GraphDescriptor.h"
#include <QDialog>
#include <QListWidget>
#include <QComboBox>
#include <QDialogButtonBox>

namespace xero
{
	namespace scouting
	{
		namespace views
		{
			class GraphConfigurationDialog : public QDialog
			{
			public:
				GraphConfigurationDialog(const xero::scouting::datamodel::GraphDescriptor& desc);
				virtual ~GraphConfigurationDialog();

			private:
				QComboBox* which_;
				QWidget* holder_;
				QListWidget* src_;
				QListWidget* graph_;
				QDialogButtonBox* buttons_;

				xero::scouting::datamodel::GraphDescriptor desc_;
			};
		}
	}
}
