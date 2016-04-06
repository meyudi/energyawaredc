//
// Created by bhavesh on 6/4/16.
//

#include "randomnumbergenerator.h"

SimulationTime RandomNumberGenerator::GenerateNextNumber(int index, Rate parameter)
{
    gsl_rng_env_setup();

    T[0] = gsl_rng_default;
    r[0] = gsl_rng_alloc(T[0]);
    int i, n = 10;
    for (i = 0; i < n; i++) {
        double u = gsl_rng_uniform(r[0]);
        printf("%.5f\n", u);
    }

    gsl_rng_free(r[0]);
}