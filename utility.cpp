//
// Created by Uddhav Arote on 07/04/16.
//

#include "utility.h"

Logger::Logger(LogLevel logLevel)
{
    this->nullstream.setstate(ios_base::badbit);
    this->logLevel = logLevel;
}

ostringstream &Logger::log(LogLevel logLevel)
{
    if (this->logLevel == LogLevel::DEBUG)
        return ss;
    else if (logLevel == LogLevel::INFO)
        return ss;
    else
        return nullstream;
}

Logger::~Logger()
{
    cout << ss.str();
    ss.flush();
}