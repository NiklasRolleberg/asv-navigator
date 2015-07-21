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
    Data* data;// = nullptr;
    Transmitter* tr_ptr;// = nullptr;
    Mission* mission_ptr;// = nullptr;
    double delta;

public:
    Navigator(){};

    /**Constructor*/
    Navigator(Transmitter* transmitter, double d);

    /**Destructor*/
    ~Navigator();

    /**Set a mission*/
    void setMission(Mission* mission);

    /**Start mission*/
    void start();

    /**cancel mission*/
    void abort();

    /**Goes to the coordinates specified in the task*/
    void goToCoordinates(Task* task);

    /**scans the polygon specified in the task*/
    void scanPolygon(Task* task);

    /**Sends a message*/
    void sendMessage(Task* task);
};

#endif
