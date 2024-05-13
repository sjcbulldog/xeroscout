#include "SelectOffseasonRosterPage.h"
#include "NewEventOffseasonWizard.h"
#include "CsvReader.h"
#include "PCScouter.h"
#include <QPushButton>
#include <QLabel>
#include <QBoxLayout>
#include <QtCore/QSettings>
#include <QtWidgets/QFileDialog>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QMessageBox>
#include <QtWidgets/QSpinBox>

SelectOffseasonRosterPage::SelectOffseasonRosterPage(NewEventOffseasonWizard::PropertyMap& map) : map_(map)
{
	QLabel* l;
	QWidget* row;
	QPushButton* button;
	QHBoxLayout* rowlayout;
	QLineEdit* titlewidget;
	QSpinBox* yearwidget;

	setTitle("Select Offseason Roster");

	QVBoxLayout* layout = new QVBoxLayout();

	rowlayout = new QHBoxLayout();
	row = new QWidget(this);
	row->setLayout(rowlayout);
	layout->addWidget(row);
	l = new QLabel(row);
	l->setText("Event Year: ");
	rowlayout->addWidget(l);
	yearwidget = new QSpinBox(row);
	yearwidget->setMinimum(2004);
	yearwidget->setMaximum(2200);
	int year = QDateTime::currentDateTime().date().year();
	yearwidget->setValue(year);
	rowlayout->addWidget(yearwidget);
	(void)connect(yearwidget, &QSpinBox::valueChanged, this, &SelectOffseasonRosterPage::yearChanged);

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
	l->setText("Teams List: ");
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

void SelectOffseasonRosterPage::yearChanged(int year)
{
	map_[YearName] = QVariant(year);
}

bool SelectOffseasonRosterPage::isValidTeamList(const QString& filename, QString& error)
{
	bool ret = true;

	QFileInfo info(filename);
	if (info.exists()) 
	{
		if (!info.isFile()) 
		{
			error = "the file '" + filename + "' exists, but is not a regular file";
			ret = false;
		}
		else {
			CsvReader reader(false);
			std::filesystem::path path(filename.toStdString());
			if (!reader.readFile(path, CsvReader::HeaderType::Headers))
			{
				error = "the file '" + filename + "' is not a valid CSV file";
				ret = false;
			}
			else 
			{
				for (int i = 0; i < reader.rowCount(); i++) {
					const DataRow& row = reader.getRow(i);
					if (row.size() < 2) {
						error += info.fileName() + ":" + QString::number(i + 1) + ": less than two entries in row - bad CSV file\n";
						ret = false;
					}
					else
					{
						if (!std::holds_alternative<double>(row[0]))
						{
							error += info.fileName() + ":" + QString::number(i + 1) + ": expected first column to be a number - bad CSV file\n";
							ret = false;
						}

						if (!std::holds_alternative<std::string>(row[1]))
						{
							error += info.fileName() + ":" + QString::number(i + 1) + ": expected second column to be a string - bad CSV file\n";
							ret = false;
						}
					}
				}
			}
		}
	}
	else 
	{
		error = "the file '" + filename + "' does not exist";
		ret = false;
	}

	return ret;
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

	QSettings settings;
	if (settings.contains(PCScouter::OffseasonDir))
	{
		dir = settings.value(PCScouter::OffseasonDir).toString();
	}

	QString filename = QFileDialog::getOpenFileName(this, "Select Teams List", dir, "CSV Files (*.csv);;All Files (*.*)");

	if (filename.length()) 
	{
		QFileInfo info(filename);
		if (!info.exists())
		{
			QMessageBox::critical(this, "Error", "File specified does not exist");
			map_[RosterFileName] = QVariant("");
			setLabelText(offseason_roster_form_, "");
		}
		else
		{
			settings.setValue(PCScouter::OffseasonDir, info.absolutePath());
			QString error;
			if (!isValidTeamList(filename, error)) {
				QMessageBox::critical(this, "Error", error);
				map_[RosterFileName] = QVariant("");
				setLabelText(offseason_roster_form_, "");
			}
			else {
				map_[RosterFileName] = QVariant(filename);
				setLabelText(offseason_roster_form_, filename);
			}
		}
	}

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
