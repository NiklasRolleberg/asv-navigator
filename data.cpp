#include "data.hpp"
#include <iostream>

Data::Data(int arg1,int arg2, int arg3)
{
    std::cout << "Data constructor" << std::endl;
}

Data::~Data()
{
    std::cout << "Data destructor" << std::endl;
}


void Data::start()
{
    std::cout << "Data start" << std::endl;
}


void Data::stop()
{
    std::cout << "Data stop" << std::endl;
}

double Data::getX()
{
    std::cout << "Data getX" << std::endl;
}


double Data::getY()
{
    std::cout << "Data getY" << std::endl;
}


double Data::getHeading()
{
    std::cout << "Data getHeading" << std::endl;
}

double Data::getDepth()
{
    std::cout << "Data getDepth" << std::endl;
}
