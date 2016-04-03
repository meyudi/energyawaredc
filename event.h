//
// Created by bhavesh on 3/4/16.
//

#ifndef ENERGYAWAREDC_EVENT_H
#define ENERGYAWAREDC_EVENT_H

#include "virtualmachine.h"

typedef unsigned long long SimulationTime;
enum class EventType: unsigned char {REQUEST_ARRIVAL, REQUEST_DEPARTURE, MIGRATION_FINISHED};

class Event
{
    const SimulationTime time;
    const EventType type;
    Id vmId, pmId, newPmId;
};
#endif //ENERGYAWAREDC_EVENT_H
