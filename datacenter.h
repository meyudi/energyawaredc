//
// Created by bhavesh on 31/3/16.
//

#ifndef ENERGYAWAREDC_DATACENTER_H
#define ENERGYAWAREDC_DATACENTER_H

#include<vector>
#include "physicalmachine.h"
#include "virtualmachine.h"

using namespace std;

typedef unsigned int EnergyUnit;

class DataCenter
{
private:
    int totalPMs, totalVMs;
    EnergyUnit totalUnits;
    EnergyUnit unitsConsumed;
    PhysicalMachine* physicalMachines;
    vector<Id> ring[3];
    VirtualMachine* virtualmachines;
public:
    DataCenter();
};
#endif //ENERGYAWAREDC_DATACENTER_H
