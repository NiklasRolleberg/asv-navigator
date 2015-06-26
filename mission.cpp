#include <iostream>
#include "mission.hpp"
#include "polygon.hpp"

Mission::Mission(int arg0)
{
    std::cout << "Mission: constructor" << std::endl;
    //task = new Task(4000000, false);

    //Create tasks and add to queue
    taskQueue.push(new Task(4000000,false));
    taskQueue.push(new Task(13.23,23.23123));
    taskQueue.push(new Task(3000000,false));
    taskQueue.push(new Task(new Polygon(5)));
    taskQueue.push(new Task(2000000,false));
    taskQueue.push(new Task(1000000,false));
}

Mission::~Mission()
{
    while(!taskQueue.empty())
    {
        Task* task = taskQueue.front();
        taskQueue.pop();
        delete task;
    }

    std::cout << "Mission: destructor" << std::endl;
}

Task* Mission::getNextTask()
{
    std::cout << "Mission: getNextTask" << std::endl;
    if(!taskQueue.empty())
    {
        Task* task = taskQueue.front();
        taskQueue.pop();
        return task;
    }
    return nullptr;
}

