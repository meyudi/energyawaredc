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
    InitializeEventQueue();
    Event nextEvent;
    while (dc.unitsConsumed != dc.totalUnits )
    {
        // get the next event
        nextEvent.time= eventQueue.top().time;
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
    //TODO: generate random number generator object to refer to the functions

    SimulationTime arrivalTime = 0.0 ;
    //goto each and every virtual machine and call GenerateNextNumber with rng index and inverse of arrival rate
    for (int i = 0; i < dc.totalVMs; ++i)
    {
        // Need to pick up appropriate lamda which is dependent on the virtual machine and the slot which is affected by
        // simulation clock
        arrivalTime = rng.GenerateNextNumber(2*i, dc.virtualmachines[i].lambda[0] );

        //-1 for newPmId

        Event e = Event(arrivalTime, EventType:: REQUEST_ARRIVAL, i, dc.virtualmachines[i].hostId, 0 );

        //push event to the priority queue
        eventQueue.push(e);
    }
};



void Simulator::HandleArrivalEvent(const Event & event)
{
    //TODO: how to decide upon the lmbda value; how to use the simulation clock to get the corresponding slot lamda

   /* get th vm object
    * total number of request > 0 : server busy else server idle
    * server busy: generatenextnumber gets time, add it to the event.time, create new event and push it to the heap
    * server idle: total request count +1; getnextnumber of lambda and mu; create two new events and push it to the heap
    * get pm with host id; increment its cpu utilization and memory consumption;
    * if beyond threshold schedule migration finish event
    *
    */
    SimulationTime nextTime;
    double

    if (dc.virtualmachines[event.vmId].totalRequestCount > 0)
    {
        //server busy
        nextTime = rng.GenerateNextNumber(2*event.vmId, dc.virtualmachines[event.vmId].lambda[0] );

        //generate a new arrival event
        Event newArrivalEvent = Event(event.time + nextTime,EventType:: REQUEST_ARRIVAL, event.vmId, event.pmId, event.newPmId);

        //push the event to the queue
        eventQueue.push(newArrivalEvent);

        // increment the total number of the requests by one
        dc.virtualmachines[event.vmId].totalRequestCount += 1 ;
    }
    else
    {
     //server idle
        // increment N by one, which is the current arrival event
        dc.virtualmachines[event.vmId].totalRequestCount += 1;

        // get next time for arrival event
        nextTime = rng.GenerateNextNumber(2*event.vmId, dc.virtualmachines[event.vmId].lambda[0] );
        Event newArrivalEvent = Event(event.time + nextTime,EventType:: REQUEST_ARRIVAL, event.vmId, event.pmId, event.newPmId);

        // get next time for departure event
        nextTime = rng.GenerateNextNumber(2*event.vmId+1, dc.virtualmachines[event.vmId].mu );
        Event newDepartureEvent = Event(event.time + nextTime,EventType:: REQUEST_DEPARTURE, event.vmId, event.pmId, event.newPmId);

        // push the events to the min event heap
        eventQueue.push(newArrivalEvent); eventQueue.push(newDepartureEvent);

    }

    // change in physical machine state

    // memory consumption of the virtual and physical machines increment by requestMemorySize; no change in cpu utilization
    dc.virtualmachines[event.vmId].memoryConsumed += dc.virtualmachines[event.vmId].requestMemorySize ;
    dc.physicalMachines[event.pmId].memoryConsumed += dc.virtualmachines[event.vmId].requestMemorySize ;


    dc.virtualmachines[event.vmId].utilization = dc.virtualmachines[event.vmId].lambda[0]/ dc.virtualmachines[event.vmId].mu;
    //dc.physicalMachines[event.vmId].utilization = dc.virtualmachines[event.vmId].utilization ;
    // threshold check to create a new migration finished event

    // to include threshold in the definition of physical machine
    // threshold check needs discussion

}

void Simulator::HandleDepartureEvent(const Event & event)
{ }

void Simulator::HandleMigrationCompletionEvent(const Event & event)
{ }