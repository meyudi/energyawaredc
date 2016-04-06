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
    IDLE, LOW_POWER, MEDIUM_POWER, HIGH_POWER
};

class PhysicalMachine
{
public:
    Id physicalMachineId;
//    const int ringId;
    PowerState state;
    float utilization;
    Byte totalMemory;
    Byte memoryConsumed;
    int vmDensity;
    int numVMs;
    vector<Id> vmList;
};
#endif //ENERGYAWAREDC_PHYSICALMACHINE_H
