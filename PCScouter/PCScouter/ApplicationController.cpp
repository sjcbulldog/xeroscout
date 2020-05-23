#include "ApplicationController.h"

ApplicationController::ApplicationController(std::shared_ptr<xero::ba::BlueAlliance> ba)
{
	ba_ = ba;
	display_initialized_ = false;
}

ApplicationController::~ApplicationController()
{
}