//
// Created by bhavesh on 6/4/16.
//

#ifndef ENERGYAWAREDC_CONFIGURATION_H
#define ENERGYAWAREDC_CONFIGURATION_H

#include "event.h"
// Use this class for all "configuration" constants that need to be used.
class Configuration
{
    static SimulationTime VM_MIGRATION_TIME = 5000; // All times in milliseconds

    // Power consumed by Physical Machines in each state
    // Units: Watts
    static unsigned long HIGH_POWER_STATE = 100;
    static unsigned long MEDIUM_POWER_STATE = 80;
    static unsigned long LOW_POWER_STATE=60;
    static unsigned long IDLE_STATE = 40;
};
#endif //ENERGYAWAREDC_CONFIGURATION_H
