#pragma once
#include "NewEventOffseasonWizard.h"
#include <QWizardPage>
#include <QLabel>

class SelectOffseasonRosterPage : public QWizardPage
{
public:
	SelectOffseasonRosterPage(NewEventOffseasonWizard::PropertyMap& map);
	virtual ~SelectOffseasonRosterPage();

	bool isComplete() const override;
	void initializePage();

public:
	static constexpr const char* EventKeyName = "key";
	static constexpr const char* EventTitleName = "name";
	static constexpr const char* RosterFileName = "roster";

private:
	void keyEdited(const QString& text);
	void titleEdited(const QString& text);
	void selectOffseasonRosterForm();
	void setLabelText(QLabel* label, const QString& str);
	bool isValidTeamList(const QString& list, QString& err);

private:
	QString key_;
	QString title_;
	NewEventOffseasonWizard::PropertyMap& map_;
	QLabel* offseason_roster_form_;
};

