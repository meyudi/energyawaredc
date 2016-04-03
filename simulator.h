//
// Created by bhavesh on 2/4/16.
//

#ifndef ENERGYAWAREDC_SIMULATOR_H
#define ENERGYAWAREDC_SIMULATOR_H

#include <vector>
#include "event.h"
#include "datacenter.h"

using namespace std;

class Simulator
{
private:
    SimulationTime simulationClockTime;
    vector<Event> eventQueue;
    DataCenter dc;
public:
    Simulator();
    int Start();
    //GetNextEvent();
};
#endif //ENERGYAWAREDC_SIMULATOR_H
