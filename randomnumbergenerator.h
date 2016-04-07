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
    const gsl_rng_type *T = gsl_rng_ranlxs2;
    gsl_rng **r;
    unsigned long int *secondarySeeds;

public:
    unsigned long int seed;
    int totalSeeds;

    void Initialize();

    /*
     * This function returns the simulation time using the exponential function
     * index: index of the random number generator
     * parameter: exponential distribution parameter
     */
    SimulationTime GenerateNextNumber(int index, Rate parameter);

    ~RandomNumberGenerator();
};
#endif //ENERGYAWAREDC_RANDOMNUMBERGENERATOR_H
