//
// Created by bhavesh on 1/4/16.
//

#ifndef ENERGYAWAREDC_PHYSICALMACHINE_H
#define ENERGYAWAREDC_PHYSICALMACHINE_H
#include "virtualmachine.h"
#include <vector>

using namespace std;

enum class MachineState : unsigned char {POWER_OFF, LOW_POWER, HIGH_POWER};

class PhysicalMachine
{
private:
    const int id;
    MachineState state;
    float utilization;
    int numVMs;
    vector<VirtualMachine> vmList;
};
#endif //ENERGYAWAREDC_PHYSICALMACHINE_H
