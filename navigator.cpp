//source file
#include "navigator.hpp"
#include <iostream>
#include <unistd.h>
#include "transmitter.hpp"
#include "mission.hpp"
#include "task.hpp"
#include "singlebeamscanner.hpp"
#include <string>

Navigator::Navigator(Transmitter* transmitter,int data_delay, int scanner_delay, double d, double t,
                      bool backup_data, std::string filename)
{
    tr_ptr = transmitter;
    std::cout << "Navigator: constructor" << std::endl;
    data = new Data(transmitter,data_delay,0); // 1000000 1s
    delta = d;
    tol = t;
    update_delay = scanner_delay;

    backup = backup_data;
    backup_filename = filename;
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

    std::cout << std::endl;
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
                break;
            case 3:
                std::cout << "New task type = 3 (scan polygon)" << std::endl;
                scanPolygon(t);
                break;
            case 4:
                std::cout << "New task type 4 (send a message)" << std::endl;
		            sendMessage(t);
		            break;
            case 5:
                std::cout << "New task type = 5 ( continue to scan polygon)" << std::endl;
                scanPolygon(t);
                break;
            default:
				        std::cout << "Unknown task type " << type << std::endl;
        }
        delete t ;
	      std::cout << std::endl;
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

    data->setBoatWaypoint_real(0,0,lat, lon,1.6,true);
    double d = data->calculateDistance(lat,lon, data->getLat(), data->getLon());
    while(d > tol)
    {
      usleep(update_delay);
      d = data->calculateDistance(lat,lon, data->getLat(), data->getLon());
      std::cout << "GOTO-TASK:  distance to target: " << d << std::endl;

      if(!data->hasCorrectPath(0,0,lat,lon,2)) {
        std::cout << "incorrect path. resending path" << std::endl;
        data->setBoatWaypoint_real(0,0,lat, lon,1.6,true);
      }

    }
    std::cout << "Target reached, task completed" << std::endl;
}

/**Execute task of type 3 or 5*/
void Navigator::scanPolygon(Task* task)
{
    std::cout << "Navigator: execute scan polygon task" << std::endl;
    data->setLocalCoordinateSystem(task->getPolygon(),delta);
    if(task->getType() == 5)
    {
      //change values in polygon->matrix if the size is the same as the old data
      std::cout << "Polygon nx" << task->getPolygon()->nx << " old:" << task->old_nx << std::endl;
      std::cout << "Polygon ny" << task->getPolygon()->ny << " old:" << task->old_ny << std::endl;

      if(task->getPolygon()->nx == task->old_nx && task->getPolygon()->ny == task->old_ny)
      {
        std::cout << "                         Size is the same!" << std::endl;
        //change the values in polygon->matrix
        for(int i=0;i<task->getPolygon()->nx;i++)
        {
          for(int j=0;j<task->getPolygon()->ny;j++)
          {
            int index = i*task->getPolygon()->ny + j;
            task->getPolygon()->matrix[i][j]->setStatus(task->old_status[index]);
            task->getPolygon()->matrix[i][j]->setAccumulatedDepth(task->old_depth[index]);
            task->getPolygon()->matrix[i][j]->setTimesVisited(task->old_visited[index]);
          }
        }

        //delete[] task->old_status;
        //delete[] task->old_depth;
        //delete[] task->old_visited;
      }
      else
        std::cout << "                         Size is not the same!" << std::endl;
    }

    SingleBeamScanner scanner = SingleBeamScanner(data, task->getPolygon(),update_delay,delta, tol, backup, backup_filename);
    std::cout << "start scanning" << std::endl;
    scanner.startScan();
    usleep(2000000);
    std::cout << "scanning completed" << std::endl;
    data->removeLocalCoordinateSystem();
}

/**Execute task of type 4*/
void Navigator::sendMessage(Task* task)
{
    tr_ptr->sendMessage(task->getMessage());
}
