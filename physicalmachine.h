//
// Created by bhavesh on 1/4/16.
//

#ifndef ENERGYAWAREDC_PHYSICALMACHINE_H
#define ENERGYAWAREDC_PHYSICALMACHINE_H
#include "virtualmachine.h"
#include <vector>

using namespace std;

enum class PowerState : unsigned char {IDLE, LOW_POWER, MEDIUM_POWER, HIGH_POWER};

class PhysicalMachine
{
private:
    const int id;
    const int ringId;
    const PowerState state;
    float utilization;
    const Byte totalMemory;
    Byte memoryConsumed;
    const int vmDensity;
    int numVMs;
    vector<VirtualMachine> vmList;
};
#endif //ENERGYAWAREDC_PHYSICALMACHINE_H
