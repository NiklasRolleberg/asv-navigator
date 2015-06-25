#include "transmitter.hpp"
#include <iostream>
#include <unistd.h>

Transmitter::Transmitter(int arg0)
{
    std::cout << "Transmitter constructor" << std::endl;
}

Transmitter::~Transmitter()
{
    std::cout << "Transmitter destructor" << std::endl;
}
void Transmitter::start()
{
    std::cout << "Transmitter start" << std::endl;
}

void Transmitter::abort()
{
    std::cout << "Transmitter abort" << std::endl;
}

int Transmitter::getDepthData()
{
    std::cout << "Transmitter getDepthData" << std::endl;
    return 0;
}

int Transmitter::getPositionData()
{
    std::cout << "Transmitter getPositionData" << std::endl;
    return 0;
}
