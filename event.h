//
// Created by bhavesh on 3/4/16.
//

#ifndef ENERGYAWAREDC_EVENT_H
#define ENERGYAWAREDC_EVENT_H

#include "virtualmachine.h"

typedef double SimulationTime; // time unit is in seconds.

enum class EventType: unsigned char
{
    REQUEST_ARRIVAL, REQUEST_DEPARTURE, MIGRATION_FINISHED
};

class Event
{
public:
    SimulationTime time;
    EventType type;
    Id vmId, pmId, newPmId;


    Event();
    Event(SimulationTime time , EventType type, Id vmId, Id pmId, Id newPmId) ;
    ~Event() ;
};
#endif //ENERGYAWAREDC_EVENT_H
