//
// Created by bhavesh on 2/4/16.
//

#include <iostream>
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
    }

    for (unsigned int i = 0, ringId = 0; i < dc.totalPMs; i++, ringId = (++ringId) % 3)
    {
        dc.physicalMachines[i].ringId = ringId;
        dc.physicalMachines[i].state = static_cast<PowerState>(ringId);
    }

    for (unsigned int i = 0, hostId = 0; i < dc.totalVMs; i++, hostId = (++hostId) % dc.totalPMs)
    {
        dc.virtualmachines[i].hostId = hostId;
        dc.physicalMachines[hostId].vmList.push_back(i);
    }
}

Simulator::~Simulator()
{
    // Release the dynamic memory allocated in DC object.
    delete dc.physicalMachines;
    delete dc.virtualmachines;
}

int Simulator::Start()
{
    // Initialize the eventQueue with all arrivals for the VMs.
    InitializeEventQueue();
    Event nextEvent;
    while (dc.unitsConsumed != dc.totalUnits)
    {
        // get the next event
        nextEvent.time = eventQueue.top().time;
        nextEvent.type = eventQueue.top().type;
        nextEvent.vmId = eventQueue.top().vmId;
        nextEvent.pmId = eventQueue.top().pmId;
        nextEvent.newPmId = eventQueue.top().newPmId;

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



        // need to update the lamda_i's of all the vms after every event execution

    }
    return 0;
}

void Simulator::InitializeEventQueue()
{
    // Example of usage of logger function.
    // Initialize the logger object in the function.
    Logger logger(this->logLevel);
    logger.log(LogLevel::DEBUG) << "bhavesh Singh" << endl << "Uddhav Arote";

    SimulationTime arrivalTime = 0.0;
    //goto each and every virtual machine and call GenerateNextNumber with rng index and inverse of arrival rate
    for (int i = 0; i < dc.totalVMs; ++i)
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


void Simulator::HandleArrivalEvent(const Event &event)
{

    /* get th vm object
     * total number of request > 0 : server busy else server idle
     * server busy: generatenextnumber gets time, add it to the event.time, create new event and push it to the heap
     * server idle: total request count +1; getnextnumber of lambda and mu; create two new events and push it to the heap
     * get pm with host id; increment its cpu utilization and memory consumption;
     * if beyond threshold schedule migration finish event
     *
     */
    SimulationTime nextTime;


    if (dc.virtualmachines[event.vmId].totalRequestCount > 0)
    {
        //server busy
        nextTime = rng.GenerateNextNumber(2 * event.vmId, dc.virtualmachines[event.vmId].lambda[(int)event.time/900]);

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
        nextTime = rng.GenerateNextNumber(2 * event.vmId, dc.virtualmachines[event.vmId].lambda[(int)event.time/900]);
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

    // memory consumption of the virtual and physical machines increment by requestMemorySize; no change in cpu utilization
    dc.virtualmachines[event.vmId].memoryConsumed += dc.virtualmachines[event.vmId].requestMemorySize;
    dc.physicalMachines[event.pmId].memoryConsumed += dc.virtualmachines[event.vmId].requestMemorySize;


    dc.virtualmachines[event.vmId].utilization =
        dc.virtualmachines[event.vmId].lambda[(int)event.time/900] / dc.virtualmachines[event.vmId].mu;
    //dc.physicalMachines[event.vmId].utilization = dc.virtualmachines[event.vmId].utilization ;
    // threshold check to create a new migration finished event

    // to include threshold in the definition of physical machine
    // threshold check needs discussion

}

void Simulator::HandleDepartureEvent(const Event &event)
{
    /*
     * get the vm object
     * if sever is busy: update the total request count in the vm; if the count is non zero schedule a departure event
     * if sever is idle: do not do anything
     * get the pm with host id; decrement the memory consumption of the vm and the pm
     * if threshold cross schedule the migration event
     */
    SimulationTime  nextTime;

    if (dc.virtualmachines[event.vmId].totalRequestCount > 0)
    {
        dc.virtualmachines[event.vmId].totalRequestCount -= 1;

        if (dc.virtualmachines[event.vmId].totalRequestCount > 0)
        {
            nextTime = rng.GenerateNextNumber(2 * event.vmId +1, dc.virtualmachines[event.vmId].mu);

            Event nextDeparture =
                Event(event.time + nextTime, EventType::REQUEST_DEPARTURE, event.vmId, event.pmId, event.newPmId );

            eventQueue.push(nextDeparture);
        }

    }
    else
    {
        // do not do anything
    }

    // change in physical machine state

    // memory consumption of the virtual and physical machines increment by requestMemorySize; no change in cpu utilization
    dc.virtualmachines[event.vmId].memoryConsumed -= dc.virtualmachines[event.vmId].requestMemorySize;
    dc.physicalMachines[event.pmId].memoryConsumed -= dc.virtualmachines[event.vmId].requestMemorySize;


    dc.virtualmachines[event.vmId].utilization =
        dc.virtualmachines[event.vmId].lambda[(int)event.time/900] / dc.virtualmachines[event.vmId].mu;



}

void Simulator::HandleMigrationCompletionEvent(const Event &event)
{ }