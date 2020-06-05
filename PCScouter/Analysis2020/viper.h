#ifndef VIPER_H
#define VIPER_H

#include "input_data.h"
#include "capabilities.h"

std::map<Team,Robot_capabilities> parse_viper(std::string const& path);

#endif
