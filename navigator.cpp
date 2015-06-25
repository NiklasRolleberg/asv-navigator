//source file
#include "navigator.hpp"
#include <iostream>
#include <unistd.h>
#include "transmitter.hpp"

Navigator::Navigator(Transmitter* transmitter, int arg2, int arg3)
{
    tr_ptr = transmitter;
    std::cout << "Navigator: constructor" << std::endl;
    data = new Data(transmitter,500000,0);
}

Navigator::~Navigator()
{
    std::cout << "Navigator: Destructor" << std::endl;
    delete data;
    delete tr_ptr;
}

void Navigator::start()
{
    data->start();
}

void Navigator::setPolygon(int arg1)
{
    std::cout << "Set polygon" << std::endl;
}

void Navigator::abort()
{
    std::cout << "Abort search" << std::endl;
    data->stop();
}
