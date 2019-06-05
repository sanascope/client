/*
  ==============================================================================

	Utils.cpp
	Created: 21 May 2019 11:17:28am
	Author:  alan

  ==============================================================================
*/

#include "Utils.h"

void Utils::log(String message) {
	Logger::getCurrentLogger()->writeToLog(message);
}

  String Utils::getTimeLabel() {
	  Time t = Time::getCurrentTime();
	  std::stringstream label;
	  label << std::setfill('0')
		  << std::setw(4) << (t.getYear())
		  << "-" << std::setw(2) << std::to_string(t.getMonth())
		  << "-" << std::setw(2) << std::to_string(t.getDayOfMonth())
		  << "_" << std::setw(2) << std::to_string(t.getHours())
		  << "-" << std::setw(2) << std::to_string(t.getMinutes())
		  << "-" << std::setw(2) << std::to_string(t.getSeconds());
	  return label.str();
  }