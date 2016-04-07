//
// Created by bhavesh on 6/4/16.
//

#include <iostream>
#include "randomnumbergenerator.h"

void RandomNumberGenerator::Initialize()
{
    gsl_rng *r1 = gsl_rng_alloc(this->T);
    gsl_rng_set(r1, this->seed);

    this->secondarySeeds = new unsigned long[this->totalSeeds];
    // Generate the new seeds for the experiment with uniform distribution
    for (int i = 0; i < this->totalSeeds; i++)
    {
        this->secondarySeeds[i] = (unsigned long) (gsl_rng_uniform(r1) * 1000000);
    }
    std::cout << std::endl;

    this->r = new gsl_rng *[this->totalSeeds];

    for (int i = 0; i < this->totalSeeds; i++)
    {
        this->r[i] = gsl_rng_alloc(this->T);
        gsl_rng_set(this->r[i], this->secondarySeeds[i]);
    }
}

SimulationTime RandomNumberGenerator::GenerateNextNumber(int index, Rate parameter)
{
    return gsl_ran_exponential(r[index], (1 / (double) parameter));
}

RandomNumberGenerator::~RandomNumberGenerator()
{
    for (int i = 0; i < totalSeeds; i++)
        gsl_rng_free(this->r[i]);
    delete this->secondarySeeds;
    delete this->r;
}
