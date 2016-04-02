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
    vector<PhysicalMachine> ring[3];
    vector<VirtualMachine> virtualmachines;
public:
    DataCenter();
};
#endif //ENERGYAWAREDC_DATACENTER_H
