//Header
#ifndef MISSION_H
#define MISSION_H

#include "task.hpp"
#include <queue>          // std::queue
#include <vector>         // std::vecor
#include <string>         // std::string


/**The mission class contains tasks for the navigation object
   tasks could be to travel to some coordinates or to scan
   a polygon */
class Mission
{
private:
    std::queue<Task*> taskQueue; // queue with tasks

public:
    /**Constructor 1*/
    Mission(int arg0);

    /**Constructor 2*/
    Mission(std::vector<std::string> plan);  

    /**Destructor*/
    ~Mission();

    /** Get the next task*/
    Task* getNextTask();
};
#endif // MISSION_H
