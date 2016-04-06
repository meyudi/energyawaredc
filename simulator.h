//
// Created by bhavesh on 2/4/16.
//

#ifndef ENERGYAWAREDC_SIMULATOR_H
#define ENERGYAWAREDC_SIMULATOR_H

#include <queue>

#include "event.h"
#include "datacenter.h"

using namespace std;

struct comparator
{
    bool operator()(const Event &e1, const Event &e2)
    {
        return e1.time > e2.time;
    };
};

class Simulator
{
private:
    SimulationTime simulationClockTime;
    priority_queue<Event, vector<Event>, comparator> eventQueue;
    DataCenter dc;
public:
    Simulator();
    ~Simulator();
    int Start();
    void InitializeEventQueue();
    //SimulationTime
    void HandleArrivalEvent(const Event &);
    void HandleDepartureEvent(const Event &);
    void HandleMigrationCompletionEvent(const Event &);
};
#endif //ENERGYAWAREDC_SIMULATOR_H
