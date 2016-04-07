#include <iostream>
#include "datacenter.h"
#include "simulator.h"
#include "utility.h"

using namespace std;

int main()
{
    Simulator sim;
    cout << "Hello, World!" << endl;

    Logger logger(LogLevel::INFO);
    logger.log(LogLevel::DEBUG)<<"bhavesh";
    logger.log(LogLevel::INFO)<<"singh";
    return 0;
}