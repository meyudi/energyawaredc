#include <iostream>
#include <iomanip>
#include "datacenter.h"
#include "simulator.h"
#include "utility.h"

using namespace std;

int main()
{
    Simulator sim;
    return sim.Start();
//    cout << "Hello, World!" << endl;
//    if (static_cast<PowerState>(0) == PowerState::HIGH_POWER)
//        cout << "HIGH_POWER" << endl;
//    RandomNumberGenerator randomNumberGenerator;
//    randomNumberGenerator.seed = 98;
//    randomNumberGenerator.totalSeeds = 20;
//    randomNumberGenerator.Initialize();
//    double sum = 0;
//    for (int i = 0; i < 20; i++)
//    {
//        for (int j = 0; j < 50; j++)
//        {
//            sum += randomNumberGenerator.GenerateNextNumber(i, j * 5 + 100);
//            cout << setprecision(5) << randomNumberGenerator.GenerateNextNumber(i, j * 5 + 100) << " ";
//        }
//        cout << sum / 50;
//        cout << endl;
//    }
//    Logger logger(LogLevel::INFO);
//    logger.log(LogLevel::DEBUG) << "bhavesh";
//    logger.log(LogLevel::INFO) << "singh";
//    return 0;
}