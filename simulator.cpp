//
// Created by bhavesh on 2/4/16.
//

#include <iostream>
#include <algorithm>
#include <iomanip>
#include "simulator.h"

Simulator::Simulator()
{
    // Set the simulation clock to the beginning of time
    this->simulationClockTime = 0;

    // Initialize the LogLevel for this run of the simulator.
    bool debug;
    cin >> debug;
    if (debug)
        this->logLevel = LogLevel::DEBUG;
    else
        this->logLevel = LogLevel::INFO;

    // Initialize the random number generator object.
    cin >> this->rng.seed;
    cin >> this->rng.totalSeeds;
    this->rng.Initialize();

    // Initialize the DC object.
    // Initialize total energy units
    cin >> dc.totalUnits;

    // Total number of PMs
    cin >> dc.totalPMs;

    // Create and initialize the PM array
    dc.physicalMachines = new PhysicalMachine[dc.totalPMs]; // TODO: Remember to clean this in destructor
    for (unsigned int i = 0; i < dc.totalPMs; i++)
    {
        dc.physicalMachines[i].physicalMachineId = i;
        dc.physicalMachines[i].memoryConsumed = 0;
        dc.physicalMachines[i].utilization = 0;
        cin >> dc.physicalMachines[i].totalMemory;
    }

    cin >> dc.totalVMs;
    dc.virtualmachines = new VirtualMachine[dc.totalVMs]; // TODO: Remember to clean this in destructor
    for (unsigned int i = 0; i < dc.totalVMs; i++)
    {
        dc.virtualmachines[i].virtualMachineId = i;
        dc.virtualmachines[i].utilization = 0;
        dc.virtualmachines[i].memoryConsumed = 0;
        dc.virtualmachines[i].totalRequestCount = 0;
        for (unsigned int j = 0; j < 96; j++)
            cin >> dc.virtualmachines[i].lambda[j];
        cin >> dc.virtualmachines[i].mu;
        cin >> dc.virtualmachines[i].requestMemorySize;
        dc.virtualmachines[i].isMigrating = false;
    }

    // Assign the PMs a ring Id, using round robin, starting from inner ring.
    for (unsigned int i = 0, ringId = 0; i < dc.totalPMs; i++, ringId = (++ringId) % 3)
    {
        dc.physicalMachines[i].ringId = ringId;
        // Add PMs to idle rings for ring ids 0,1
        if (ringId != 2)
        {
            dc.idleRing[ringId].push_back(i);
            dc.physicalMachines[i].state = PowerState::IDLE;
        }
        else
        {
            dc.ring[ringId].push_back(i);
            dc.physicalMachines[i].state = static_cast<PowerState>(ringId);
        }
    }

    /*// Assign the VMs a PM, using round robin, starting from PM Id 0.
    for (unsigned int i = 0, hostId = 0; i < dc.totalVMs; i++, hostId = (++hostId) % dc.totalPMs)
    {
        dc.virtualmachines[i].hostId = hostId;
        dc.physicalMachines[hostId].vmList.push_back(i);
    }*/
    for (unsigned int i = 0, hostId = 0; i < dc.totalVMs; i++, hostId = (++hostId) % (unsigned) dc.ring[2].size())
    {
        dc.virtualmachines[i].hostId = dc.ring[2][hostId];
        dc.physicalMachines[dc.ring[2][hostId]].vmList.push_back(i);
    }
}

/*-----------------------------------------------------------------------------------------*/

Simulator::~Simulator()
{
    // Release the dynamic memory allocated in DC object.
//    delete dc.physicalMachines;
//    delete dc.virtualmachines;
}

/*-----------------------------------------------------------------------------------------*/

int Simulator::Start()
{
    // Initialize the eventQueue with all arrivals for the VMs.
    InitializeEventQueue();
    Event nextEvent;

    PrintStatus("Start");
    // TODO: Change this to run till the end of time.
//    while (dc.unitsConsumed <= dc.totalUnits)
    while (this->simulationClockTime < 96 * 15 * 60)
    {
        // get the next event
        nextEvent = eventQueue.top(); // Faster than what was done below.
//        nextEvent.time = eventQueue.top().time;
//        nextEvent.type = eventQueue.top().type;
//        nextEvent.vmId = eventQueue.top().vmId;
//        nextEvent.pmId = eventQueue.top().pmId;
//        nextEvent.newPmId = eventQueue.top().newPmId;

        switch (nextEvent.type)
        {
            case EventType::REQUEST_ARRIVAL:
                HandleArrivalEvent(nextEvent);
                break;

            case EventType::REQUEST_DEPARTURE:
                HandleDepartureEvent(nextEvent);
                break;

            case EventType::MIGRATION_FINISHED:
                HandleMigrationCompletionEvent(nextEvent);
                break;
        }



        //pop the processed event
        eventQueue.pop();

    }

    PrintStatus("End");
    return 0;
}

/*-----------------------------------------------------------------------------------------*/

void Simulator::InitializeEventQueue()
{
    // Initialize the logger object in the function.
//    Logger logger(this->logLevel);
//    logger.log(LogLevel::DEBUG) << "Initializing event queue..." << endl;

    SimulationTime arrivalTime;
    //goto each and every virtual machine and call GenerateNextNumber with rng index and inverse of arrival rate
    for (unsigned int i = 0; i < dc.totalVMs; ++i)
    {
        // Need to pick up appropriate lamda which is dependent on the virtual machine and the slot which is affected by
        // simulation clock
        arrivalTime = rng.GenerateNextNumber(2 * i, dc.virtualmachines[i].lambda[0]);

        //-1 for newPmId

        Event e = Event(arrivalTime, EventType::REQUEST_ARRIVAL, i, dc.virtualmachines[i].hostId, 0);

        //push event to the priority queue
        eventQueue.push(e);
    }
};

/*-----------------------------------------------------------------------------------------*/

void Simulator::HandleArrivalEvent(Event &event)
{
    // Initialize the logger object in the function.
//    Logger logger(this->logLevel);

    /* get the vm object
     * total number of request > 0 : server busy else server idle
     * server busy: generatenextnumber gets time, add it to the event.time, create new event and push it to the heap
     * server idle: total request count +1; getnextnumber of lambda and mu; create two new events and push it to the heap
     * get pm with host id; increment its cpu utilization and memory consumption;
     * if beyond threshold schedule migration finish event
     *
     */
    SimulationTime nextTime;
    float newUtilization;

    if (event.pmId != dc.virtualmachines[event.vmId].hostId)
        event.pmId = dc.virtualmachines[event.vmId].hostId;

    if (dc.virtualmachines[event.vmId].totalRequestCount > 0)
    {
        //server busy
        nextTime =
            rng.GenerateNextNumber(2 * event.vmId, dc.virtualmachines[event.vmId].lambda[(int) event.time / 900]);

        //generate a new arrival event
        Event newArrivalEvent =
            Event(event.time + nextTime, EventType::REQUEST_ARRIVAL, event.vmId, event.pmId, event.newPmId);

        //push the event to the queue
        eventQueue.push(newArrivalEvent);

        // increment the total number of the requests by one
        dc.virtualmachines[event.vmId].totalRequestCount += 1;
    }
    else
    {
        //server idle
        // increment N by one, which is the current arrival event
        dc.virtualmachines[event.vmId].totalRequestCount += 1;

        // get next time for arrival event
        nextTime =
            rng.GenerateNextNumber(2 * event.vmId, dc.virtualmachines[event.vmId].lambda[(int) event.time / 900]);
        Event newArrivalEvent =
            Event(event.time + nextTime, EventType::REQUEST_ARRIVAL, event.vmId, event.pmId, event.newPmId);

        // get next time for departure event
        nextTime = rng.GenerateNextNumber(2 * event.vmId + 1, dc.virtualmachines[event.vmId].mu);
        Event newDepartureEvent =
            Event(event.time + nextTime, EventType::REQUEST_DEPARTURE, event.vmId, event.pmId, event.newPmId);

        // push the events to the min event heap
        eventQueue.push(newArrivalEvent);
        eventQueue.push(newDepartureEvent);

    }

    // change in physical machine state

    // memory consumption of the virtual and physical machines increment by requestMemorySize
    dc.virtualmachines[event.vmId].memoryConsumed += dc.virtualmachines[event.vmId].requestMemorySize;
//    if (!dc.virtualmachines[event.vmId].isMigrating)
    dc.physicalMachines[event.pmId].memoryConsumed += dc.virtualmachines[event.vmId].requestMemorySize;
//    dc.physicalMachines[event.pmId].memoryConsumed =
//        dc.physicalMachines[event.pmId].memoryConsumed + dc.virtualmachines[event.vmId].requestMemorySize
//            > dc.physicalMachines[event.pmId].totalMemory ? dc.physicalMachines[event.pmId].totalMemory :
//        dc.physicalMachines[event.pmId].memoryConsumed + dc.virtualmachines[event.vmId].requestMemorySize;

    // Change CPU utilization if lambda has changed
    newUtilization =
        dc.virtualmachines[event.vmId].lambda[(int) event.time / 900] / dc.virtualmachines[event.vmId].mu > 1 ? 1 :
        dc.virtualmachines[event.vmId].lambda[(int) event.time / 900] / dc.virtualmachines[event.vmId].mu;

//    if (newUtilization != dc.virtualmachines[event.vmId].utilization)
    {
//        if (!dc.virtualmachines[event.vmId].isMigrating)
//        dc.physicalMachines[event.pmId].utilization = (dc.physicalMachines[event.pmId].utilization * dc.physicalMachines[event.pmId].vmList.size()
//            + newUtilization - dc.virtualmachines[event.vmId].utilization) / dc.physicalMachines[event.pmId].vmList.size();

        dc.virtualmachines[event.vmId].utilization = newUtilization;
    }

    float newPMUtilization = 0;
    for (unsigned int i = 0; i < dc.physicalMachines[event.pmId].vmList.size(); i++)
        newPMUtilization += dc.virtualmachines[dc.physicalMachines[event.pmId].vmList[i]].utilization;
    dc.physicalMachines[event.pmId].utilization = newPMUtilization / dc.physicalMachines[event.pmId].vmList.size();

//    logger.log(LogLevel::DEBUG) << event.time << ": Arrival: VM: (" << event.vmId << ","
//        << dc.virtualmachines[event.vmId].memoryConsumed << "," << dc.virtualmachines[event.vmId].utilization << ","
//        << dc.virtualmachines[event.vmId].totalRequestCount << "," << dc.virtualmachines[event.vmId].isMigrating
//        << "): PM: (" << event.pmId << "," << dc.physicalMachines[event.pmId].memoryConsumed << ","
//        << dc.physicalMachines[event.pmId].utilization << "," << dc.physicalMachines[event.pmId].ringId << ","
//        << dc.physicalMachines[event.pmId].vmList.size() << "): Energy: " << dc.unitsConsumed << endl;

    // call to the threshold change function
    if (!dc.virtualmachines[event.vmId].isMigrating)
        MigrateVM(event);

    // calculate the energyUnits consumed
    UpdateEnergyConsumption(event);


    //update Simulation Clock time
    this->simulationClockTime = event.time;
}

/*-----------------------------------------------------------------------------------------*/

void Simulator::HandleDepartureEvent(Event &event)
{
    // Initialize the logger object in the function.
//    Logger logger(this->logLevel);

    /*
     * get the vm object
     * if sever is busy: update the total request count in the vm; if the count is non zero schedule a departure event
     * if sever is idle: do not do anything
     * get the pm with host id; decrement the memory consumption of the vm and the pm
     * if threshold cross schedule the migration event
     */

    SimulationTime nextTime;
    float newUtilization;

    if (event.pmId != dc.virtualmachines[event.vmId].hostId)
        event.pmId = dc.virtualmachines[event.vmId].hostId;

//    if (dc.virtualmachines[event.vmId].totalRequestCount > 0)
//    {
    dc.virtualmachines[event.vmId].totalRequestCount -= 1;

    if (dc.virtualmachines[event.vmId].totalRequestCount > 0)
    {
        // There are more requests in line, so schedule a departure event.
        nextTime = rng.GenerateNextNumber(2 * event.vmId + 1, dc.virtualmachines[event.vmId].mu);

        Event nextDeparture =
            Event(event.time + nextTime, EventType::REQUEST_DEPARTURE, event.vmId, event.pmId, event.newPmId);

        eventQueue.push(nextDeparture);
    }

//    }
//    else
//    {
//        // do not do anything
//    }

    // change in physical machine state

    // memory consumption of the virtual and physical machines decrement by requestMemorySize
    dc.virtualmachines[event.vmId].memoryConsumed -= dc.virtualmachines[event.vmId].requestMemorySize;
//    if (!dc.virtualmachines[event.vmId].isMigrating)
    dc.physicalMachines[event.pmId].memoryConsumed -= dc.virtualmachines[event.vmId].requestMemorySize;
//        dc.physicalMachines[event.pmId].memoryConsumed =
//            dc.physicalMachines[event.pmId].memoryConsumed < dc.virtualmachines[event.vmId].requestMemorySize ? 0 :
//            dc.physicalMachines[event.pmId].memoryConsumed - dc.virtualmachines[event.vmId].requestMemorySize;

    // Change CPU utilization if lambda has changed
    newUtilization =
        dc.virtualmachines[event.vmId].lambda[(int) event.time / 900] / dc.virtualmachines[event.vmId].mu > 1 ? 1 :
        dc.virtualmachines[event.vmId].lambda[(int) event.time / 900] / dc.virtualmachines[event.vmId].mu;
//    if (newUtilization != dc.virtualmachines[event.vmId].utilization)
    {
//        if (!dc.virtualmachines[event.vmId].isMigrating)
//        dc.physicalMachines[event.pmId].utilization = (dc.physicalMachines[event.pmId].utilization * dc.physicalMachines[event.pmId].vmList.size()
//            + newUtilization - dc.virtualmachines[event.vmId].utilization) / dc.physicalMachines[event.pmId].vmList.size();

        dc.virtualmachines[event.vmId].utilization = newUtilization;
    }

    float newPMUtilization = 0;
    for (unsigned int i = 0; i < dc.physicalMachines[event.pmId].vmList.size(); i++)
        newPMUtilization += dc.virtualmachines[dc.physicalMachines[event.pmId].vmList[i]].utilization;
    dc.physicalMachines[event.pmId].utilization = newPMUtilization / dc.physicalMachines[event.pmId].vmList.size();

//    logger.log(LogLevel::DEBUG) << event.time << ": Departure: VM: (" << event.vmId << ","
//        << dc.virtualmachines[event.vmId].memoryConsumed << "," << dc.virtualmachines[event.vmId].utilization << ","
//        << dc.virtualmachines[event.vmId].totalRequestCount << "," << dc.virtualmachines[event.vmId].isMigrating
//        << "): PM: (" << event.pmId << "," << dc.physicalMachines[event.pmId].memoryConsumed << ","
//        << dc.physicalMachines[event.pmId].utilization << "," << dc.physicalMachines[event.pmId].ringId << ","
//        << dc.physicalMachines[event.pmId].vmList.size() << "): Energy: " << dc.unitsConsumed << endl;

    // call to the threshold check function
    if (!dc.virtualmachines[event.vmId].isMigrating)
        MigrateVM(event);

    // calulate the energyUnits consumed
    UpdateEnergyConsumption(event);

    //update Simulation Clock time
    this->simulationClockTime = event.time;
}

/*-----------------------------------------------------------------------------------------*/
void Simulator::HandleMigrationCompletionEvent(const Event &event)
{
    // Initialize the logger object in the function.
//    Logger logger(this->logLevel);
    //logger.log(LogLevel::INFO) << event.time << ": Migration: VM: " << event.vmId << " Old PM: " << event.pmId << " New PM: " << event.newPmId << endl;

    /*
     * remove this vm from old pm vmlist; add this vm to the new pm vmlist
     * subtract vm memory from the old pm; add vm memory to the new pm
     * subtract utilization from the old pm; add utilization to new pm ;
     * change the hostid of the vm to new pm
     * if old pm vmlist size is zero then put that machine into idle state;
     * change the power state of the new pm if was in idle state
     */

    // bring the new physical machine up
    if (dc.physicalMachines[event.newPmId].state == PowerState::IDLE)
    {
        dc.physicalMachines[event.newPmId].state = static_cast<PowerState>( dc.physicalMachines[event.newPmId].ringId );

        // remove from the idle ring id list
        dc.idleRing[dc.physicalMachines[event.newPmId].ringId].erase(
            std::remove(dc.idleRing[dc.physicalMachines[event.newPmId].ringId].begin(),
                        dc.idleRing[dc.physicalMachines[event.newPmId].ringId].end(),
                        event.newPmId),
            dc.idleRing[dc.physicalMachines[event.newPmId].ringId].end());

        // push to the ring id list
        dc.ring[dc.physicalMachines[event.newPmId].ringId].push_back(event.newPmId);
    }

    // remove the vm from the current physical machine; basically subtract the numbers.
    if (dc.physicalMachines[event.pmId].vmList.size() > 1)
    {
        dc.physicalMachines[event.pmId].memoryConsumed -= dc.virtualmachines[event.vmId].memoryConsumed;
        dc.physicalMachines[event.pmId].utilization =
            (dc.physicalMachines[event.pmId].utilization * dc.physicalMachines[event.pmId].vmList.size()
                - dc.virtualmachines[event.vmId].utilization) / (dc.physicalMachines[event.pmId].vmList.size() - 1);
    }
    else
    {
        dc.physicalMachines[event.pmId].memoryConsumed = 0;
        dc.physicalMachines[event.pmId].utilization = 0;
    }

    // remove from the old PM
    dc.physicalMachines[event.pmId].vmList.erase(std::remove(dc.physicalMachines[event.pmId].vmList.begin(),
                                                             dc.physicalMachines[event.pmId].vmList.end(),
                                                             event.vmId),
                                                 dc.physicalMachines[event.pmId].vmList.end());


    // add the vm to the new physical machine
    dc.physicalMachines[event.newPmId].vmList.push_back(event.vmId);

    dc.physicalMachines[event.newPmId].utilization = (dc.physicalMachines[event.newPmId].utilization * dc.physicalMachines[event.newPmId].vmList.size() + dc.virtualmachines[event.vmId].utilization) / (dc.physicalMachines[event.newPmId].vmList.size() + 1);

    //TODO: See if we need this any more. Else keep commented.
//    dc.physicalMachines[event.newPmId].utilization =
//        dc.physicalMachines[event.newPmId].utilization > 1 ? 1.0f : dc.physicalMachines[event.newPmId].utilization;

    dc.physicalMachines[event.newPmId].memoryConsumed += dc.virtualmachines[event.vmId].memoryConsumed;
//    dc.physicalMachines[event.newPmId].memoryConsumed =
//        dc.physicalMachines[event.newPmId].memoryConsumed + dc.virtualmachines[event.vmId].memoryConsumed
//            > dc.physicalMachines[event.newPmId].totalMemory ? dc.physicalMachines[event.newPmId].totalMemory :
//        dc.physicalMachines[event.newPmId].memoryConsumed + dc.virtualmachines[event.vmId].memoryConsumed;

    // change the host id of the vm
    dc.virtualmachines[event.vmId].hostId = event.newPmId;

    // set isMigrating to false
    dc.virtualmachines[event.vmId].isMigrating = false;

    // before the state change update the energy consumed;
    UpdateEnergyConsumption(event);


    // remove from the old PM
//    dc.physicalMachines[event.pmId].vmList.erase(std::remove(dc.physicalMachines[event.pmId].vmList.begin(),
//                                                             dc.physicalMachines[event.pmId].vmList.end(),
//                                                             event.vmId),
//                                                 dc.physicalMachines[event.pmId].vmList.end());

    //change the power state of the pms
    if (dc.physicalMachines[event.pmId].vmList.size() == 0)
    {
        dc.physicalMachines[event.pmId].state = PowerState::IDLE;

        // remove from the ring id list
        dc.ring[dc.physicalMachines[event.pmId].ringId].erase(
            std::remove(dc.ring[dc.physicalMachines[event.pmId].ringId].begin(),
                        dc.ring[dc.physicalMachines[event.pmId].ringId].end(),
                        event.pmId),
            dc.ring[dc.physicalMachines[event.pmId].ringId].end());

        // add to idle ring id list
        dc.idleRing[dc.physicalMachines[event.pmId].ringId].push_back(event.pmId);
    }


//    logger.log(LogLevel::DEBUG) << event.time << ": Migration Finish: VM: (" << event.vmId << ","
//        << dc.virtualmachines[event.vmId].memoryConsumed << "," << dc.virtualmachines[event.vmId].utilization << ","
//        << dc.virtualmachines[event.vmId].totalRequestCount << "," << dc.virtualmachines[event.vmId].isMigrating
//        << "): old PM: (" << event.pmId << "," << dc.physicalMachines[event.pmId].memoryConsumed << ","
//        << dc.physicalMachines[event.pmId].utilization << "," << dc.physicalMachines[event.pmId].ringId << ","
//        << dc.physicalMachines[event.pmId].vmList.size() << "): new PM: (" << event.newPmId << ","
//        << dc.physicalMachines[event.newPmId].memoryConsumed << ","
//        << dc.physicalMachines[event.newPmId].utilization << "," << dc.physicalMachines[event.newPmId].ringId << ","
//        << dc.physicalMachines[event.newPmId].vmList.size() << "): Ring: (";
//
//    for (unsigned int i = 0; i < 3; i++)
//        logger.log(LogLevel::DEBUG) << dc.ring[i].size() << ",";
//    logger.log(LogLevel::DEBUG) << "): idle Ring: (";
//    for (unsigned int i = 0; i < 3; i++)
//        logger.log(LogLevel::DEBUG) << dc.idleRing[i].size() << ",";
//    logger.log(LogLevel::DEBUG) << "): Energy: " << dc.unitsConsumed << endl;

    //update Simulation Clock time
    this->simulationClockTime = event.time;

    PrintStatus("Migration");

}

/*-----------------------------------------------------------------------------------------*/
void Simulator::MigrateVM(const Event &event)
{
    // If power state is low or medium, i.e. ring 2,1, then only check if upper threshold (.33,.66) is crossed.
    if (dc.physicalMachines[event.pmId].state == PowerState::LOW_POWER
        || dc.physicalMachines[event.pmId].state == PowerState::MEDIUM_POWER)
    {
        float threshold = (float) (3 - static_cast<int>(dc.physicalMachines[event.pmId].state)) / 3;
        // Check if memory crossed the upper threshold.
        if ((float) dc.physicalMachines[event.pmId].memoryConsumed / dc.physicalMachines[event.pmId].totalMemory
            > threshold)
        {
            // Cycle through all the PMs in the ring from current ring to
            // progressively higher power rings till ring 1 (ring 0 handled differently)
            // and place the current VM on the first PM to have enough free space
            bool candidatePMFound = false;
            for (int i = dc.physicalMachines[event.pmId].ringId; i > 0; i--)
            {
                threshold = (float) (3 - i) / 3;
                for (unsigned int j = 0; j < dc.ring[i].size(); j++)
                {
                    if (((float) (dc.physicalMachines[dc.ring[i][j]].memoryConsumed
                        + dc.virtualmachines[event.vmId].memoryConsumed)
                        / dc.physicalMachines[dc.ring[i][j]].totalMemory
                        <= threshold) || threshold == 1)
                    {
                        // We found a candidate PM
                        Event event1(event.time + Configuration::VM_MIGRATION_TIME,
                                     EventType::MIGRATION_FINISHED,
                                     event.vmId,
                                     event.pmId,
                                     dc.ring[i][j]);
                        dc.virtualmachines[event.vmId].isMigrating = true;
                        this->eventQueue.push(event1);
//                        HandleMigrationStartEvent(event1);
                        candidatePMFound = true;
                        break;
                    }
                }
                if (candidatePMFound)
                    break;
            }
            if (!candidatePMFound)
            {
                // We look for idle PMs in the ring from current ring to progressively higher power rings
                for (int i = dc.physicalMachines[event.pmId].ringId; i >= 0; i--)
                {
                    // TODO: The following code can be simplified. Just dump it on the first idle PM. No need for these checks.
                    threshold = (float) (3 - i) / 3;
                    for (unsigned int j = 0; j < dc.idleRing[i].size(); j++)
                    {
                        if ((float) (dc.physicalMachines[dc.idleRing[i][j]].memoryConsumed
                            + dc.virtualmachines[event.vmId].memoryConsumed)
                            / dc.physicalMachines[dc.idleRing[i][j]].totalMemory
                            <= threshold)
                        {
                            // We found a candidate PM
                            Event event1(event.time + Configuration::VM_MIGRATION_TIME,
                                         EventType::MIGRATION_FINISHED,
                                         event.vmId,
                                         event.pmId,
                                         dc.idleRing[i][j]);
                            dc.virtualmachines[event.vmId].isMigrating = true;
                            this->eventQueue.push(event1);
//                        HandleMigrationStartEvent(event1);
                            candidatePMFound = true;
                            break;
                        }
                    }
                    if (candidatePMFound)
                        break;
                }

                if (!candidatePMFound)
                {
                    // This is the case when no suitable machine was found on any lower rings.
                    // So we need to fit the VM on one of ring0 PMs.
                    // We find the one with the least memory and push it there.
                    Id minPM = dc.ring[0][0];
                    for (unsigned int i = 0; i < dc.ring[0].size(); i++)
                        if (dc.physicalMachines[dc.ring[0][i]].memoryConsumed < dc.physicalMachines[minPM].memoryConsumed)
                            minPM = dc.ring[0][i];

                    // We found a candidate PM
                    Event event1(event.time + Configuration::VM_MIGRATION_TIME,
                                 EventType::MIGRATION_FINISHED,
                                 event.vmId,
                                 event.pmId,
                                 minPM);
                    dc.virtualmachines[event.vmId].isMigrating = true;
                    this->eventQueue.push(event1);
                    candidatePMFound = true;
                }
            }
        }
        else if (dc.physicalMachines[event.pmId].utilization > threshold)
        {
            // Check if CPU crossed the upper threshold
            // Cycle through all the PMs in the ring from current ring to
            // progressively higher power rings till ring 1 (ring 0 handled differently)
            // and place the current VM on the first PM to have enough free CPU
            bool candidatePMFound = false;
            for (int i = dc.physicalMachines[event.pmId].ringId; i > 0; i--)
            {
                threshold = (float) (3 - i) / 3;
                for (unsigned int j = 0; j < dc.ring[i].size(); j++)
                {
                    if (((dc.physicalMachines[dc.ring[i][j]].utilization * dc.physicalMachines[dc.ring[i][j]].vmList.size() + dc.virtualmachines[event.vmId].utilization) / (dc.physicalMachines[dc.ring[i][j]].vmList.size() + 1) <= threshold) || threshold == 1)
                    {
                        // We found a candidate PM
                        Event event1(event.time + Configuration::VM_MIGRATION_TIME,
                                     EventType::MIGRATION_FINISHED,
                                     event.vmId,
                                     event.pmId,
                                     dc.ring[i][j]);
                        dc.virtualmachines[event.vmId].isMigrating = true;
                        this->eventQueue.push(event1);
//                        HandleMigrationStartEvent(event1);
                        candidatePMFound = true;
                        break;
                    }
                }
                if (candidatePMFound)
                    break;
            }
            if (!candidatePMFound)
            {
                // We look for idle PMs in the ring from current ring to progressively higher power rings
                for (int i = dc.physicalMachines[event.pmId].ringId; i >= 0; i--)
                {
                    // TODO: The following code can be simplified. Just dump it on the first idle PM. No need for these checks.
                    threshold = (float) (3 - i) / 3;
                    for (unsigned int j = 0; j < dc.idleRing[i].size(); j++)
                    {
                        if ((dc.physicalMachines[dc.idleRing[i][j]].utilization * dc.physicalMachines[dc.idleRing[i][j]].vmList.size() + dc.virtualmachines[event.vmId].utilization) / (dc.physicalMachines[dc.idleRing[i][j]].vmList.size() + 1) <= threshold)
                        {
                            // We found a candidate PM
                            Event event1(event.time + Configuration::VM_MIGRATION_TIME,
                                         EventType::MIGRATION_FINISHED,
                                         event.vmId,
                                         event.pmId,
                                         dc.idleRing[i][j]);
                            dc.virtualmachines[event.vmId].isMigrating = true;
                            this->eventQueue.push(event1);
//                        HandleMigrationStartEvent(event1);
                            candidatePMFound = true;
                            break;
                        }
                    }
                    if (candidatePMFound)
                        break;
                }

                if (!candidatePMFound)
                {
                    // This is the case when no suitable machine was found on any lower rings.
                    // So we need to fit the VM on one of ring0 PMs.
                    // We find the one with the least memory and push it there.
                    Id minPM = dc.ring[0][0];
                    for (unsigned int i = 0; i < dc.ring[0].size(); i++)
                        if (dc.physicalMachines[dc.ring[0][i]].memoryConsumed < dc.physicalMachines[minPM].memoryConsumed)
                            minPM = dc.ring[0][i];

                    // We found a candidate PM
                    Event event1(event.time + Configuration::VM_MIGRATION_TIME,
                                 EventType::MIGRATION_FINISHED,
                                 event.vmId,
                                 event.pmId,
                                 minPM);
                    dc.virtualmachines[event.vmId].isMigrating = true;
                    this->eventQueue.push(event1);
                    candidatePMFound = true;
                }
            }
        }
    }

    // If power state is medium or high, i.e. ring 1,0, then only check if lower threshold (.33,.66) is crossed.
    if (dc.physicalMachines[event.pmId].state == PowerState::MEDIUM_POWER
        || dc.physicalMachines[event.pmId].state == PowerState::HIGH_POWER)
    {
        float threshold = (float) (2 - static_cast<int>(dc.physicalMachines[event.pmId].state)) / 3;
        // Check if memory crossed the lower threshold.
        if ((float) dc.physicalMachines[event.pmId].memoryConsumed / dc.physicalMachines[event.pmId].totalMemory <= threshold
            && dc.physicalMachines[event.pmId].utilization <= threshold)
        {
            // Cycle through all the PMs in the ring from current ring to progressively lower power rings
            // and place the current VM on the first PM to have enough free space
            bool candidatePMFound = false;
            for (unsigned int i = dc.physicalMachines[event.pmId].ringId + 1; i <= 2; i++)
            {
                threshold = (float) (3 - i) / 3;
                for (unsigned int j = 0; j < dc.ring[i].size(); j++)
                {
                    if (((float) (dc.physicalMachines[dc.ring[i][j]].memoryConsumed + dc.virtualmachines[event.vmId].memoryConsumed) / dc.physicalMachines[dc.ring[i][j]].totalMemory <= threshold)
                        && ((dc.physicalMachines[dc.ring[i][j]].utilization * dc.physicalMachines[dc.ring[i][j]].vmList.size() + dc.virtualmachines[event.vmId].utilization) / (dc.physicalMachines[dc.ring[i][j]].vmList.size() + 1) <= threshold))
                    {
                        // We found a candidate PM
                        Event event1(event.time + Configuration::VM_MIGRATION_TIME,
                                     EventType::MIGRATION_FINISHED,
                                     event.vmId,
                                     event.pmId,
                                     dc.ring[i][j]);
                        dc.virtualmachines[event.vmId].isMigrating = true;
                        this->eventQueue.push(event1);
//                        HandleMigrationStartEvent(event1);
                        candidatePMFound = true;
                        break;
                    }
                }
                if (candidatePMFound)
                    break;
            }
//            if (!candidatePMFound)
//            {
//                // TODO: This is the corner case when all PMs in the outer rings are also loaded fully.
//                // We do not handle this currently.
//            }
        }

        // Check if CPU crossed lower the threshold
        /*else if ()
        {
            // Cycle through all the PMs in the ring from current ring to progressively lower rings
            // and place the current VM on the first PM to have enough free CPU
            bool candidatePMFound = false;
            for (unsigned int i = dc.physicalMachines[event.pmId].ringId + 1; i <= 2; i++)
            {
                threshold = (float) (3 - i) / 3;
                for (unsigned int j = 0; j < dc.ring[i].size(); j++)
                {
                    if ((dc.physicalMachines[dc.ring[i][j]].utilization
                        * dc.physicalMachines[dc.ring[i][j]].vmList.size()
                        + dc.virtualmachines[event.vmId].utilization)
                        / (dc.physicalMachines[dc.ring[i][j]].vmList.size() + 1)
                        <= threshold)
                    {
                        // We found a candidate PM
                        Event event1(event.time + Configuration::VM_MIGRATION_TIME,
                                     EventType::MIGRATION_FINISHED,
                                     event.vmId,
                                     event.pmId,
                                     dc.ring[i][j]);
                        dc.virtualmachines[event.vmId].isMigrating = true;
                        this->eventQueue.push(event1);
//                        HandleMigrationStartEvent(event1);
                        candidatePMFound = true;
                        break;
                    }
                }
                if (candidatePMFound)
                    break;
            }
            if (!candidatePMFound)
            {
                // TODO: This is the corner case when all PMs in the inner most ring are also loaded fully. We do not handle this currently.
            }
        }*/
    }
}

/*-----------------------------------------------------------------------------------------*/
void Simulator::UpdateEnergyConsumption(const Event &event)
{
    for (int i = 0; i < 3; ++i)
    {
        if (i == static_cast<int>(PowerState::HIGH_POWER))
        {
            dc.unitsConsumed +=
                dc.ring[i].size() * Configuration::HIGH_POWER_STATE * (event.time - simulationClockTime);
            dc.unitsConsumed += dc.idleRing[i].size() * Configuration::IDLE_STATE * (event.time - simulationClockTime);
        }


        else if (i == static_cast<int>(PowerState::MEDIUM_POWER))
        {
            dc.unitsConsumed +=
                dc.ring[i].size() * Configuration::MEDIUM_POWER_STATE * (event.time - simulationClockTime);
            dc.unitsConsumed += dc.idleRing[i].size() * Configuration::IDLE_STATE * (event.time - simulationClockTime);
        }


        else if (i == static_cast<int>(PowerState::LOW_POWER))
        {
            dc.unitsConsumed += dc.ring[i].size() * Configuration::LOW_POWER_STATE * (event.time - simulationClockTime);
            dc.unitsConsumed += dc.idleRing[i].size() * Configuration::IDLE_STATE * (event.time - simulationClockTime);
        }

    }
}

void Simulator::PrintStatus(string eventName)
{
    Logger logger(this->logLevel);
    //logger.log(LogLevel::INFO) << event.time << ": Migration: VM: " << event.vmId << " Old PM: " << event.pmId << " New PM: " << event.newPmId << endl;

    /*for (unsigned int i = 0; i < dc.totalVMs; i++)
    {
        logger.log(LogLevel::INFO)
            << setw(20) << left << this->simulationClockTime
            << setw(10) << left << i
            << setw(10) << left << dc.physicalMachines[dc.virtualmachines[0].hostId].physicalMachineId
            << setw(10) << left << dc.physicalMachines[dc.virtualmachines[0].hostId].ringId
            << setw(10) << left << static_cast<int>(dc.physicalMachines[dc.virtualmachines[0].hostId].state)
            << setw(10) << left << dc.physicalMachines[dc.virtualmachines[0].hostId].utilization
            << setw(20) << left << dc.physicalMachines[dc.virtualmachines[0].hostId].memoryConsumed
            << endl;
    }*/

    unsigned int ringPMs[3]{0, 0, 0}, idleRingPMs[3]{0, 0, 0}, ringVMs[3]{0, 0, 0};
    for (unsigned int i = 0; i < dc.totalPMs; i++)
    {
        if (dc.physicalMachines[i].state == PowerState::IDLE)
            idleRingPMs[dc.physicalMachines[i].ringId]++;
        else
            ringPMs[dc.physicalMachines[i].ringId]++;
        ringVMs[dc.physicalMachines[i].ringId] += dc.physicalMachines[i].vmList.size();
    }
    logger.log(LogLevel::INFO)
        << left << setprecision(3) << fixed
        << this->simulationClockTime << ": " << eventName
        << ": " << this->dc.unitsConsumed << endl;

    for (unsigned i = 0; i < 3; i++)
    {
        logger.log(LogLevel::INFO)
            << setw(5) << i
            << setw(5) << ringPMs[i]
            << setw(5) << idleRingPMs[i]
            << setw(5) << ringVMs[i];
        stringstream PMUtil, PMMem;
        PMUtil << "|";
        for (unsigned int j = 0; j < dc.ring[i].size(); j++)
            PMUtil << setprecision(1) << fixed << dc.physicalMachines[dc.ring[i][j]].utilization << "|";
        PMMem << "|";
        for (unsigned int j = 0; j < dc.ring[i].size(); j++)
            PMMem << dc.physicalMachines[dc.ring[i][j]].memoryConsumed << "|";
        logger.log(LogLevel::DEBUG) << setw(50) << PMUtil.str()
            << setw(20) << PMMem.str();
        logger.log(LogLevel::INFO) << endl;
    }
}