#include "SelectOffseasonRosterPage.h"
#include "NewEventOffseasonWizard.h"
#include <QPushButton>
#include <QLabel>
#include <QBoxLayout>
#include <QFileDialog>
#include <QLineEdit>

SelectOffseasonRosterPage::SelectOffseasonRosterPage(NewEventOffseasonWizard::PropertyMap& map) : map_(map)
{
	QLabel* l;
	QWidget* row;
	QPushButton* button;
	QHBoxLayout* rowlayout;
	QLineEdit* titlewidget;

	setTitle("Select Offseason Roster");

	QVBoxLayout* layout = new QVBoxLayout();

	rowlayout = new QHBoxLayout();
	row = new QWidget(this);
	row->setLayout(rowlayout);
	layout->addWidget(row);
	l = new QLabel(row);
	l->setText("Event Key: ");
	rowlayout->addWidget(l);
	titlewidget = new QLineEdit(row);
	rowlayout->addWidget(titlewidget);
	(void)connect(titlewidget, &QLineEdit::textEdited, this, &SelectOffseasonRosterPage::keyEdited);

	rowlayout = new QHBoxLayout();
	row = new QWidget(this);
	row->setLayout(rowlayout);
	layout->addWidget(row);
	l = new QLabel(row);
	l->setText("Event Name: ");
	rowlayout->addWidget(l);
	titlewidget = new QLineEdit(row);
	rowlayout->addWidget(titlewidget);

	(void)connect(titlewidget, &QLineEdit::textEdited, this, &SelectOffseasonRosterPage::titleEdited);

	// Pit scouting row
	rowlayout = new QHBoxLayout();
	row = new QWidget(this);
	row->setLayout(rowlayout);
	layout->addWidget(row);

	l = new QLabel(row);
	l->setText("Team Roster List: ");
	rowlayout->addWidget(l);

	offseason_roster_form_ = new QLabel(row);
	offseason_roster_form_->setText("NONE");
	rowlayout->addWidget(offseason_roster_form_, 100);

	button = new QPushButton(row);
	button->setText("...");
	rowlayout->addWidget(button);
	(void)connect(button, &QPushButton::pressed, this, &SelectOffseasonRosterPage::selectOffseasonRosterForm);

	setLayout(layout);
}

SelectOffseasonRosterPage::~SelectOffseasonRosterPage()
{
}

void SelectOffseasonRosterPage::keyEdited(const QString& text)
{
	if (text.length() == 0)
	{
		if (map_.find(EventKeyName) != map_.end()) {
			map_.erase(map_.find(EventKeyName));
		}
	}
	else
	{
		map_[EventKeyName] = QVariant(text);
	}

	emit completeChanged();
}

void SelectOffseasonRosterPage::titleEdited(const QString& text)
{
	if (text.length() == 0)
	{
		if (map_.find(EventTitleName) != map_.end()) {
			map_.erase(map_.find(EventTitleName));
		}
	}
	else
	{
		map_[EventTitleName] = QVariant(text);
	}

	emit completeChanged();
}

void SelectOffseasonRosterPage::initializePage()
{
	QWizardPage::initializePage();

	NewEventOffseasonWizard* w = dynamic_cast<NewEventOffseasonWizard*>(wizard());
	w->setTitle();
}

void SelectOffseasonRosterPage::selectOffseasonRosterForm()
{
	QString dir;

	QString filename = QFileDialog::getOpenFileName(this, "Select Pit Scouting Form", dir, "CSV Files (*.csv);;All Files (*.*)");
	map_[RosterFileName] = QVariant(filename);

	roster_ = filename;
	setLabelText(offseason_roster_form_, roster_);

	emit completeChanged();
}

void SelectOffseasonRosterPage::setLabelText(QLabel* label, const QString& str)
{
	QFontMetrics fm(label->font());
	QString txt = fm.elidedText(str, Qt::TextElideMode::ElideLeft, offseason_roster_form_->width());
	label->setText(txt);
}

bool SelectOffseasonRosterPage::isComplete() const
{
	return map_.find(RosterFileName) != map_.end() &&
			map_.find(EventTitleName) != map_.end() &&
			map_.find(EventKeyName) != map_.end();
}
