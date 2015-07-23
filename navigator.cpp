//source file
#include "navigator.hpp"
#include <iostream>
#include <unistd.h>
#include "transmitter.hpp"
#include "mission.hpp"
#include "task.hpp"
#include "singlebeamscanner.hpp"
#include <string>

Navigator::Navigator(Transmitter* transmitter, double d)
{
    tr_ptr = transmitter;
    std::cout << "Navigator: constructor" << std::endl;
    data = new Data(transmitter,1000000,0); // 1000000 1s
    delta = d;
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
				//std::cout << "size: " << sizeof((*t)) << std::endl;
                usleep(t->getSleepTime());
                break;
            case 2:
                std::cout << "New task type = 2 (go to coordinates)" << std::endl;
                goToCoordinates(t);
                break;
            case 3:
                std::cout << "New task type = 3 (scan polygon)" << std::endl;
                scanPolygon(t);
                break;
			case 4:
                std::cout << "New task type 4 (send a message)" << std::endl;
				sendMessage(t);
				break;
            default:
				std::cout << "Unknown task type " << type << std::endl;
        }
		delete t ; //memory  leak... --------------------------------------------------------------------------------- //TODO fix this
		//std::cout << "delete t.." << std::endl;
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
    double lat = task->getTargetLat();
    double lon = task->getTargetLon();

    data->setBoatWaypoint_real(lat, lon);
    double d = data->calculateDistance(lat,lon, data->getLat(), data->getLon());
    while(d > 3)
    {
      usleep(1000000);
      d = data->calculateDistance(lat,lon, data->getLat(), data->getLon());
      std::cout << "GOTO-TASK:  distance to target: " << d << std::endl;
    }
    std::cout << "Target reached, task completed" << std::endl;
}

/**Execute task of type 3*/
void Navigator::scanPolygon(Task* task)
{
    std::cout << "Navigator: execute scan polygon task" << std::endl;
    data->setLocalCoordinateSystem(task->getPolygon(),delta);
    SingleBeamScanner scanner = SingleBeamScanner(data, task->getPolygon(),delta);
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
