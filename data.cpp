#include "data.hpp"
#include <iostream>
//#include <thread>   // std::thread
#include <unistd.h>
#include "transmitter.hpp"

Data::Data(Transmitter* transmitter,int delay, int arg3)
{
    std::cout << "Data constructor" << std::endl;
    data_transmitterptr = transmitter;
    data_delay = delay;
    //Get initial values for variables
    boat_xpos = 0;
    boat_ypos = 0;
    boat_heading = 0;
    boat_speed = 0;
    boat_depth = 0;
}

Data::~Data()
{
    std::cout << "Data destructor" << std::endl;
    data_stop = true;
    if(data_threadptr != nullptr)
        if(data_threadptr->joinable())
            data_threadptr->join();
}

void Data::start()
{
    std::cout << "Data start" << std::endl;
    if(data_threadptr != nullptr)
        std::cerr << "Memory leak incoming.." << std::endl;
        //data_threadptr->terminate();


    data_threadptr = new std::thread(&Data::threadLoop, this);
    //data_threadptr->join();
    //threadLoop();
}

void Data::stop()
{
    std::cout << "Data stop" << std::endl;
    data_stop = true;
    if(data_threadptr != nullptr)
        if(data_threadptr->joinable())
            data_threadptr->join();
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

double Data::getSpeed()
{
    std::cout << "Data getSpeed" << std::endl;
}

double Data::getDepth()
{
    std::cout << "Data getDepth" << std::endl;
}

void Data::threadLoop()
{
    std::cout << "Data loop started" << std::endl;
    int i=0;
    while(!data_stop)
    {
        /*
        std::cout << "Check latitude" << std::endl;
        std::cout << "Check longitude" << std::endl;
        std::cout << "Check depth" << std::endl;
        std::cout << "Check speed" << std::endl;
        */
        std::cout << "varv: " << i++ << std::endl;
        data_transmitterptr->getDepthData();
        usleep(data_delay);
    }
    std::cout << "Data loop done" << std::endl;

}
