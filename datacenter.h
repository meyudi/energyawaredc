//
// Created by bhavesh on 31/3/16.
//

#ifndef ENERGYAWAREDC_DATACENTER_H
#define ENERGYAWAREDC_DATACENTER_H
#include<vector>
#include "virtualmachine.h"

using namespace std;

typedef unsigned int EnergyUnit;

class DataCenter
{
private:
    int totalPMs, totalVMs;
    EnergyUnit totalUnits;
    EnergyUnit unitsConsumed;
public:
    DataCenter();
};
#endif //ENERGYAWAREDC_DATACENTER_H
