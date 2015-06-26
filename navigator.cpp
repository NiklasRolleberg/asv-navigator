//source file
#include "navigator.hpp"
#include <iostream>
#include <unistd.h>
#include "transmitter.hpp"
#include "mission.hpp"
#include "task.hpp"

Navigator::Navigator(Transmitter* transmitter, int arg2, int arg3)
{
    tr_ptr = transmitter;
    std::cout << "Navigator: constructor" << std::endl;
    data = new Data(transmitter,500000,0);
}

Navigator::~Navigator()
{
    delete data;
    std::cout << "Navigator: Destructor" << std::endl;
}

void Navigator::setMission(Mission* mission)
{
    std::cout << "Set mission" << std::endl;
    mission_ptr = mission;
}

void Navigator::start()
{
    std::cout << "Navigator: Mission started" << std::endl;
    //start data collection from the boat
    data->start();


    //execute tasks
    Task* t = mission_ptr->getNextTask();
    if(t->getType() == 1)
    {
        usleep(t->getSleepTime());
    }


    data->stop();
    std::cout << "Navigator: Mission completed" << std::endl;
}


void Navigator::abort()
{
    std::cout << "Abort search" << std::endl;
    data->stop();
}

void Navigator::executeTask()
{
    std::cout << "Navigator: execute task" << std::endl;
}

