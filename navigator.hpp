//Header
#ifndef NAVIGATOR_H
#define NAVIGATOR_H

#include "data.hpp"
#include "transmitter.hpp"
#include "mission.hpp"
#include "task.hpp"
class Navigator
{
private:
    Data* data = nullptr;
    Transmitter* tr_ptr = nullptr;
    Mission* mission_ptr = nullptr;

public:
    Navigator(){};

    /**Constructor*/
    Navigator(Transmitter* transmitter, int arg2, int arg3);

    /**Destructor*/
    ~Navigator();

    /**Set a mission*/
    void setMission(Mission* mission);

    /**Start mission*/
    void start();

    /**cancel mission*/
    void abort();

    /**Carry out a task from the mission. e.g. transport from
     one place to another or scan a polygon*/
    void executeTask();
};

#endif
