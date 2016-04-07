//
// Created by Uddhav Arote on 07/04/16.
//

#ifndef ENERGYAWAREDC_UTILITY_H
#define ENERGYAWAREDC_UTILITY_H

#include <iostream>
#include <sstream>

using namespace std;

// The code for Log class is taken from Dr. Dobb's Journal September 2007 issue
// with appropriate modifications to suit our needs. <http://www.drdobbs.com/cpp/logging-in-c/201804215>

enum class LogLevel {INFO, DEBUG};

class Logger
{
private:
    ostringstream ss,nullstream;
    LogLevel logLevel;

public:
    Logger(LogLevel);
    ~Logger();
    ostringstream& log(LogLevel);
};

#endif //ENERGYAWAREDC_UTILITY_H
