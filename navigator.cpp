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
    data = new Data(transmitter,1000000,0); // f�r l�ng delay
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


    Task* t = nullptr;
    while((t = mission_ptr->getNextTask()) != nullptr)
    {
        int type = t->getType();
        switch (type)
        {
            case 1:
                std::cout << "New task type = 1 (sleep)" << std::endl;
                usleep(t->getSleepTime());
                break;
            case 2:
                std::cout << "New task type = 2 (go to coordinates)" << std::endl;
                goToCoordinates(t);
                //TODO Do something
                break;
            case 3:
                std::cout << "New task type = 3 (scan polygon)" << std::endl;
                scanPolygon(t);
                //TODO Do something
                break;
            default:
                std::cout << "Unknown task type " << type << std::endl;
        }
        delete t;
    }


    data->stop();
    std::cout << "Navigator: Mission completed" << std::endl;
}


void Navigator::abort()
{
    std::cout << "Abort search" << std::endl;
    data->stop();
}

/**Execute task of type 2*/
void Navigator::goToCoordinates(Task* task)
{
    std::cout << "Navigator: execute go to coordinates task" << std::endl;
    usleep(2000000);
}

/**Execute task of type 3*/
void Navigator::scanPolygon(Task* task)
{
    std::cout << "Navigator: execute scan polygon task" << std::endl;
    data->setLocalCoordinateSystem(0);

    std::cout << "start scanning" << std::endl;
    usleep(2000000);
    std::cout << "scanning completed" << std::endl;
    data->removeLocalCoordinateSystem();
}

