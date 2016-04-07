//
// Created by bhavesh on 1/4/16.
//

#ifndef ENERGYAWAREDC_VIRTUALMACHINE_H
#define ENERGYAWAREDC_VIRTUALMACHINE_H

typedef unsigned int Id;

typedef int Rate; // Unit of rates is per second.
typedef unsigned long long Byte;

class VirtualMachine
{
public:
    Id virtualMachineId;
    Id hostId;
    Rate lambda[96]; // The workload characterized as a series of lambdas, each for 15 minutes
    Rate mu;
    float utilization;
//    Byte totalMemory; // Assuming VM has enough memory i.e. over-committed memory
    Byte memoryConsumed;
    Byte requestMemorySize; // Indicates the amount of memory allocated for processing the request
    Byte requestQueueSize; // The size of the request queue of this VM
    int totalRequestCount; // Represents the total number of requests in the system

    VirtualMachine(); // constructor
    ~VirtualMachine(); // destructor
};
#endif //ENERGYAWAREDC_VIRTUALMACHINE_H
