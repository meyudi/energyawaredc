//
// Created by bhavesh on 1/4/16.
//

#ifndef ENERGYAWAREDC_PHYSICALMACHINE_H
#define ENERGYAWAREDC_PHYSICALMACHINE_H

#include <vector>
#include "virtualmachine.h"

using namespace std;

enum class PowerState: unsigned char
{
    HIGH_POWER = 0, MEDIUM_POWER = 1, LOW_POWER = 2, IDLE = 3,
};

class PhysicalMachine
{
public:
    Id physicalMachineId;
    int ringId;
    PowerState state;
    float utilization;
    Byte totalMemory;
    Byte memoryConsumed;
//    int vmDensity; // Commented. Not directly usable in our context. We assume that we can only pack as many as allowed.
//    int numVMs; // Commented. As vmList.size() will give us the number of VMs on this PM.
    vector<Id> vmList;
};
#endif //ENERGYAWAREDC_PHYSICALMACHINE_H
