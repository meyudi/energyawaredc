//
// Created by Uddhav Arote on 07/04/16.
//

#ifndef ENERGYAWAREDC_UTILITY_H
#define ENERGYAWAREDC_UTILITY_H

#include <sstream>

using namespace std;

enum class LogLevel {DEBUG, NORMAL};

class Utility
{
public:
    static void log(stringstream , LogLevel ) ;



};

#endif //ENERGYAWAREDC_UTILITY_H
