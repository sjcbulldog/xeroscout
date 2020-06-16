#pragma once

#include "scoutviews_global.h"
#include "ViewBase.h"
#include "ZebraSequence.h"
#include "SequenceAnalyzer.h"
#include <QWidget>
#include <QComboBox>
#include <QGroupBox>
#include <QTextBrowser>
#include <list>
#include <memory>

namespace xero
{
	namespace scouting
	{
		namespace views
		{
			class SCOUTVIEWS_EXPORT ZebraAnalysisView : public QWidget, public ViewBase
			{
			public:
				ZebraAnalysisView(QWidget* parent);
				~ZebraAnalysisView();

				void refreshView();
				void clearView();

			protected:
				void showEvent(QShowEvent* ev) override;

			private:
				void teamChanged(int index);
				void regenerate();

				void printRawData(QString& html);
				void printSequences(QString& html);

			private:
				QComboBox* box_;
				QGroupBox* report_;
				QTextBrowser* report_txt_;
				QString current_team_;
				std::list<std::shared_ptr<xero::scouting::datamodel::ZebraSequence>> sequences_;
				xero::scouting::datamodel::SequenceAnalyzer analyzer_;
			};
		}
	}
}

