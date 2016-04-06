//
// Created by bhavesh on 6/4/16.
//

#ifndef ENERGYAWAREDC_RANDOMNUMBERGENERATOR_H
#define ENERGYAWAREDC_RANDOMNUMBERGENERATOR_H

#include <gsl/gsl_rng.h>
#include <gsl/gsl_randist.h>
#include "event.h"

class RandomNumberGenerator
{
private:
    int seed;
    int totalSeeds;
    const gsl_rng_type **T;
    gsl_rng **r;

public:
    SimulationTime GenerateNextNumber(int index, Rate parameter);
};
#endif //ENERGYAWAREDC_RANDOMNUMBERGENERATOR_H
