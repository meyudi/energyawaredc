//
// Created by bhavesh on 2/4/16.
//

#include <iostream>
#include "simulator.h"

Simulator::Simulator()
{
    // Initialize the DC object.
    // Total number of PMs
    cin >> dc.totalPMs;
    // Create and initialize the PM array
    dc.physicalMachines = new PhysicalMachine[dc.totalPMs];
    for (int i = 0; i < dc.totalPMs; i++)
    {
        dc.physicalMachines[i].physicalMachineId = i;
        dc.physicalMachines[i].state = PowerState::IDLE;
        dc.physicalMachines[i].utilization = 0;
        cin >> dc.physicalMachines[i].totalMemory;
    }

    cin >> dc.totalVMs;
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

