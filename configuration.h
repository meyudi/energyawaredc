//
// Created by bhavesh on 6/4/16.
//

#ifndef ENERGYAWAREDC_CONFIGURATION_H
#define ENERGYAWAREDC_CONFIGURATION_H

#include "event.h"
// Use this class for all "configuration" constants that need to be used.
class Configuration
{
public:
    static constexpr SimulationTime VM_MIGRATION_TIME = 0; // All times in seconds

    // Power consumed by Physical Machines in each state
    // Units: Watts
    static constexpr unsigned long HIGH_POWER_STATE = 100;
    static constexpr unsigned long MEDIUM_POWER_STATE = 80;
    static constexpr unsigned long LOW_POWER_STATE=60;
    static constexpr unsigned long IDLE_STATE = 40;
};
#endif //ENERGYAWAREDC_CONFIGURATION_H
