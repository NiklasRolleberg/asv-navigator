//source file
#include "navigator.hpp"
#include <iostream>
#include <unistd.h>
#include "transmitter.hpp"
#include "mission.hpp"
#include "task.hpp"
#include "singlebeamscanner.hpp"
#include <string>

Navigator::Navigator(Transmitter* transmitter, int arg2, int arg3)
{
    tr_ptr = transmitter;
    std::cout << "Navigator: constructor" << std::endl;
    data = new Data(transmitter,100000,0); // 1000000 1s
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
	   case 4:
                std::cout << "New task type 4 (send a message)" << std::endl;
		sendMessage(t);
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
    data->setLocalCoordinateSystem(task->getPolygon());
    SingleBeamScanner scanner = SingleBeamScanner(data, task->getPolygon());
    std::cout << "start scanning" << std::endl;
    scanner.startScan();
    //usleep(2000000);
    std::cout << "scanning completed" << std::endl;
    data->removeLocalCoordinateSystem();
}

/**Execute task of type 4*/
void Navigator::sendMessage(Task* task)
{
    tr_ptr->sendMessage(task->getMessage());
}

