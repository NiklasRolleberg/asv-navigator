//Header
#ifndef MISSION_H
#define MISSION_H

#include "task.hpp"


/**The mission class contains tasks for the navigation object
   tasks could be to travel to some coordinates or to scan
   a polygon */
class Mission
{
private:
    int QUEUE = 0; // queue with tasks
    Task* task = nullptr;

public:
    /**Constructor*/
    Mission(int arg0);

    /**Destructor*/
    ~Mission();

    /** Get the next task*/
    Task* getNextTask();
};
#endif // MISSION_H
