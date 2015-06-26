#include <iostream>
#include "mission.hpp"


Mission::Mission(int arg0)
{
    std::cout << "Mission: constructor" << std::endl;
    task = new Task(4000000, false);
}

Mission::~Mission()
{
    delete task;
    std::cout << "Mission: destructor" << std::endl;
}

Task* Mission::getNextTask()
{
    std::cout << "Mission: getNextTask" << std::endl;
    return task;
}

