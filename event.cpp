//
// Created by bhavesh on 3/4/16.
//

#include "event.h"

Event::Event()
{ }

Event::Event(SimulationTime time, EventType type, Id vmId, Id pmId, Id newPmId)
{
    this->time = time ;
    this->type = type;
    this->vmId = vmId;
    this->pmId = pmId;
    this->newPmId = newPmId;
}

Event::~Event()
{ }