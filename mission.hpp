//Header
#ifndef MISSION_H
#define MISSION_H

#include "task.hpp"
#include <queue>          // std::queue


/**The mission class contains tasks for the navigation object
   tasks could be to travel to some coordinates or to scan
   a polygon */
class Mission
{
private:
    std::queue<Task*> taskQueue; // queue with tasks

public:
    /**Constructor*/
    Mission(int arg0);

    /**Destructor*/
    ~Mission();

    /** Get the next task*/
    Task* getNextTask();
};
#endif // MISSION_H
