#pragma once

#include "BlueAlliance.h"
#include <QObject>
#include <memory>

class ApplicationController : public QObject
{
	Q_OBJECT

public:
	ApplicationController(std::shared_ptr<xero::ba::BlueAlliance> ba);
	virtual ~ApplicationController();

	bool isDisplayInitialized() { return display_initialized_; }
	void setDisplayInitialized() { display_initialized_ = true; }

	virtual bool isDone() = 0;
	virtual void run() = 0;

	virtual bool shouldDisableApp() { return true; }

	virtual bool providesProgress() { return false; }
	virtual int percentDone() { return 0; }

signals:
	void errorMessage(const QString& msg);
	void logMessage(const QString& msg);
	void complete(bool err);

protected:
	std::shared_ptr<xero::ba::BlueAlliance> blueAlliance() {
		return ba_;
	}

private:
	std::shared_ptr<xero::ba::BlueAlliance> ba_;
	bool display_initialized_;
};

