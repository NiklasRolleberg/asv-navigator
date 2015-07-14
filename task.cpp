#include "task.hpp"
#include <iostream>
#include <unistd.h>
#include "polygon.hpp"
//#include <string>

Task::Task()
{
    std::cout << "Task: Default constructor" << std::endl;
    type = 0;
    message = "";
}

Task::Task(int sleepTime, bool drift)
{
    std::cout << "Task: wait constructor" << std::endl;
    type = 1;
    delay = sleepTime;
}

Task::Task(double coordinate1, double coordinate2)
{
    std::cout << "Task: goToPoint constructor" << std::endl;
    type = 2;
}

Task::Task(Polygon* polygon)
{
    std::cout << "Task: scan polygon constructor" << std::endl;
    type = 3;
    polygon_ptr = polygon;
}

Task::Task(std::string incMessage)
{
    std::cout << "Task: send message contructor" << std::endl;
    type = 4;
    message = incMessage;
}


Task::~Task()
{
    if(polygon_ptr != nullptr)
        delete polygon_ptr;
    std::cout << "Task destructor" << std::endl;
}

int Task::getType()
{
    return type;
}


int Task::getSleepTime()
{
    return delay;
}

Polygon* Task::getPolygon()
{
    return polygon_ptr;
}

std::string Task::getMessage()
{
    return message;
}

