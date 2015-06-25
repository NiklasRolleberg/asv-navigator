//source file
#include "navigator.hpp"
#include <iostream>

Navigator::Navigator(int arg1, int arg2, int arg3)
{
    std::cout << "constructor" << std::endl;
    data = Data(0,0,0);
}

Navigator::~Navigator()
{
    std::cout << "Navigator: Destructor" << std::endl;
}

void Navigator::setPolygon(int arg1)
{
    std::cout << "Set polygon" << std::endl;
}

void Navigator::abort()
{
    std::cout << "Abort search" << std::endl;
}
