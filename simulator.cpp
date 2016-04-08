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

/*-----------------------------------------------------------------------------------------*/

Simulator::~Simulator()
{
    // Release the dynamic memory allocated in DC object.
    delete dc.physicalMachines;
    delete dc.virtualmachines;
}

/*-----------------------------------------------------------------------------------------*/

int Simulator::Start()
{
    // Initialize the eventQueue with all arrivals for the VMs.
    InitializeEventQueue();
    Event nextEvent;
    while (dc.unitsConsumed <= dc.totalUnits)
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



        //pop the processed event
        eventQueue.pop() ;

    }
    return 0;
}

/*-----------------------------------------------------------------------------------------*/

void Simulator::InitializeEventQueue()
{
    // Example of usage of logger function.
    // Initialize the logger object in the function.
    Logger logger(this->logLevel);
    logger.log(LogLevel::DEBUG) << "bhavesh Singh" << endl << "Uddhav Arote";

    SimulationTime arrivalTime = 0.0;
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
    float newUtilization ;


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


    newUtilization = dc.virtualmachines[event.vmId].lambda[(int)event.time/900] / dc.virtualmachines[event.vmId].mu;

    if (newUtilization != dc.virtualmachines[event.vmId].utilization)
    {
        dc.physicalMachines[event.pmId].utilization +=
            (newUtilization - dc.virtualmachines[event.pmId].utilization)/ dc.physicalMachines[event.pmId].vmList.size();

        dc.virtualmachines[event.vmId].utilization = newUtilization ;
    }


    // call to the threshold change function
    MigrateVM(event) ;

    // calculate the energyUnits consumed
    UpdateEnergyConsumption(event);


    //update Simulation Clock time
    this->simulationClockTime = event.time ;
}

/*-----------------------------------------------------------------------------------------*/

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
    float newUtilization  ;

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

    newUtilization = dc.virtualmachines[event.vmId].lambda[(int)event.time/900] / dc.virtualmachines[event.vmId].mu;
    if (newUtilization != dc.virtualmachines[event.vmId].utilization)
    {
        dc.physicalMachines[event.pmId].utilization +=
            (newUtilization - dc.virtualmachines[event.pmId].utilization)/ dc.physicalMachines[event.pmId].vmList.size();

        dc.virtualmachines[event.vmId].utilization = newUtilization ;
    }


    // call to the threshold check function
    MigrateVM(event) ;

    // calulate the energyUnits consumed
    UpdateEnergyConsumption(event);

    //update Simulation Clock time
    this->simulationClockTime = event.time ;


}


/*-----------------------------------------------------------------------------------------*/
void Simulator::HandleMigrationCompletionEvent(const Event &event)
{



    /*
     * remove this vm from old pm vmlist; add this vm to the new pm vmlist
     * subtract vm memory from the old pm; add vm memory to the new pm
     * subtract utilization from the old pm; add utilization to new pm ;
     * change the hostid of the vm to new pm
     * if old pm vmlist size is zero then put that machine into idle state;
     * change the power state of the new pm if was in idle state
     */


    // remove the vm from the current physical machine
    dc.physicalMachines[event.pmId].memoryConsumed -= dc.virtualmachines[event.vmId].memoryConsumed;
    dc.physicalMachines[event.pmId].utilization =
        (dc.physicalMachines[event.pmId].utilization * dc.physicalMachines[event.pmId].vmList.size() - dc.virtualmachines[event.vmId].utilization)/ (dc.physicalMachines[event.pmId].vmList.size() -1);
    dc.physicalMachines[event.pmId].vmList.erase(
        std::remove(dc.physicalMachines[event.pmId].vmList.begin(), dc.physicalMachines[event.pmId].vmList.end(), event.vmId)
        , dc.physicalMachines[event.pmId].vmList.end());


    // add the vm to the new physical machine
    dc.physicalMachines[event.newPmId].utilization =
        (dc.physicalMachines[event.newPmId].utilization * dc.physicalMachines[event.newPmId].vmList.size() + dc.virtualmachines[event.vmId].utilization)/ (dc.physicalMachines[event.newPmId].vmList.size() +1);
    dc.physicalMachines[event.newPmId].vmList.push_back(event.vmId);
    dc.physicalMachines[event.newPmId].memoryConsumed += dc.virtualmachines[event.vmId].memoryConsumed ;

    // change the host id of the vm
    dc.virtualmachines[event.vmId].hostId = event.newPmId ;


    // before the state change update the energy consumed;
    UpdateEnergyConsumption(event);


    //change the power state of the pms
    if(dc.physicalMachines[event.pmId].vmList.size() == 0)
    {
        dc.physicalMachines[event.pmId].state = PowerState::IDLE ;

        // remove from the ring id list
        dc.ring[dc.physicalMachines[event.pmId].ringId].erase(
            std::remove(dc.ring[dc.physicalMachines[event.pmId].ringId].begin(),dc.ring[dc.physicalMachines[event.pmId].ringId].end(),event.pmId ),
            dc.ring[dc.physicalMachines[event.pmId].ringId].end()) ;

        // add to idle ring id list
        dc.idleRing[dc.physicalMachines[event.pmId].ringId].push_back(event.pmId);
    }
    if (dc.physicalMachines[event.newPmId].state == PowerState::IDLE)
    {
        dc.physicalMachines[event.newPmId].state = static_cast<PowerState>( dc.physicalMachines[event.newPmId].ringId );

        // remove from the idle ring id list
        dc.idleRing[dc.physicalMachines[event.newPmId].ringId].erase(
            std::remove(dc.idleRing[dc.physicalMachines[event.newPmId].ringId].begin(),dc.idleRing[dc.physicalMachines[event.newPmId].ringId].end(),event.pmId ),
            dc.idleRing[dc.physicalMachines[event.newPmId].ringId].end()) ;

        // push to the ring id list
        dc.ring[dc.physicalMachines[event.pmId].ringId].push_back(event.pmId);
    }
    //update Simulation Clock time
    this->simulationClockTime = event.time ;

}

/*-----------------------------------------------------------------------------------------*/
void Simulator::MigrateVM(const Event &)
{

}

/*-----------------------------------------------------------------------------------------*/
void Simulator::UpdateEnergyConsumption(const Event & event )
{
    for (int i = 0; i < 3; ++i)
    {
        if (i == static_cast<int>(PowerState::HIGH_POWER) )
        {
            dc.unitsConsumed += dc.ring[i].size() * Configuration::HIGH_POWER_STATE * (event.time - simulationClockTime);
            dc.unitsConsumed += dc.idleRing[i].size() * Configuration::IDLE_STATE * (event.time - simulationClockTime);
        }


        else if(i == static_cast<int>(PowerState::MEDIUM_POWER))
        {
            dc.unitsConsumed += dc.ring[i].size() * Configuration::MEDIUM_POWER_STATE * (event.time - simulationClockTime);
            dc.unitsConsumed += dc.idleRing[i].size() * Configuration::IDLE_STATE * (event.time - simulationClockTime);
        }


        else if(i == static_cast<int>(PowerState::LOW_POWER))
        {
            dc.unitsConsumed += dc.ring[i].size() * Configuration::LOW_POWER_STATE * (event.time - simulationClockTime);
            dc.unitsConsumed += dc.idleRing[i].size() * Configuration::IDLE_STATE * (event.time - simulationClockTime);
        }

    }
}