#pragma once
#include "ImageManager.h"
#include <QWizard>

class TabletPoolWizardPage;

class NewEventOffseasonWizard : public QWizard
{
	Q_OBJECT;

public:
	typedef std::map<QString, QVariant> PropertyMap;

public:
	NewEventOffseasonWizard(xero::scouting::datamodel::ImageManager& mgr, const QStringList &tablets);
	virtual ~NewEventOffseasonWizard();

	void setTitle();

	QString getEventKey();
	QString getEventName();
	QString getPitScoutingForm();
	QString getMatchScoutingForm();
	QString getRosterForm();
	int getYear();

	QStringList getTabletList();
	QStringList getPitTabletList();
	QStringList getMatchTabletList();

private:
	QString key_;
	QString title_;
	PropertyMap props_;
	TabletPoolWizardPage* page_;
	QStringList tablets_;
};

