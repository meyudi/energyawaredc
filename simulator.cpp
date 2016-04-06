//
// Created by bhavesh on 2/4/16.
//

#include <iostream>
#include "simulator.h"

Simulator::Simulator()
{
    // Init DC object.
    cin >> dc.totalPMs;
    // Initialize all other data members.
}

Simulator::~Simulator()
{
    // Release DC object.
}

int Simulator::Start()
{
    // Initialize the eventQueue with all arrivals for the VMs.

    while (true);
    //GetNextEvent();
    return 0;
}

void Simulator::InitializeEventQueue()
{
    //TODO: generate random number generator object to refer to the functions

    SimulationTime arrivalTime = 0.0 ;
    //goto each and every virtual machine and call GenerateNextNumber with rng index and inverse of arrival rate
    for (int i = 0; i < dc.totalVMs; ++i)
    {
        // this next statement is problematic .. need to resolve !
        //arrivalTime = RandomNumberGenerator::GenerateNextNumber(2*i, dc.virtualmachines[i].lambda );

        //-1 for newPmId

        Event e = Event(arrivalTime, EventType:: REQUEST_ARRIVAL, i, dc.virtualmachines[i].hostId, -1 );

        //push event to the priority queue
        eventQueue.push(e);
    }
};