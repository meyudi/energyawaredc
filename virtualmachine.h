//
// Created by bhavesh on 1/4/16.
//

#ifndef ENERGYAWAREDC_VIRTUALMACHINE_H
#define ENERGYAWAREDC_VIRTUALMACHINE_H

typedef int ArrivalRate;
typedef int ServiceRate;
typedef unsigned long long Byte;

class VirtualMachine
{
private:
    const int id;
    ArrivalRate lambda[96]; // The workload characterized as a series of lambdas, each for 15 minutes.
    ServiceRate mu;
    float utilization;
    const Byte totalMemory;
    Byte memoryConsumed;
    const Byte requestMemorySize; // Indicates the amount of memory allocated for processing the request.
    const Byte queuedRequestSize;
    int queueLength;

public:
    VirtualMachine();
};
#endif //ENERGYAWAREDC_VIRTUALMACHINE_H
