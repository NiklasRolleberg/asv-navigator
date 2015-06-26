//Header
#ifndef DATA_H
#define DATA_H

#include <thread>   // std::thread
#include "transmitter.hpp"

class Data
{
private:
    double boat_xpos;
    double boat_ypos;
    double boat_heading;
    double boat_speed;
    double boat_depth;

    bool data_stop = false;
    int data_delay = 500000; //0.5s
    Transmitter* data_transmitterptr;
    std::thread *data_threadptr = nullptr;

    /**collects data, calculates new coordinates*/
    void threadLoop();

public:

    Data(){};

    /**Constructor*/
    Data(Transmitter* transmitter, int delay, int arg3);

    /**Destructor*/
    ~Data();

    /**Start collecting data*/
    void start();

    /**Stop collecting data*/
    void stop();


    /**Get boat x-position*/
    double getLat();

    /**Get boat y-position*/
    double getLon();

    /**Get boat heading*/
    double getLatLongHeading();


    /**create a local coordinateSystem*/
    void setLocalCoordinateSystem(int polygon);

    /**create a local coordinateSystem*/
    void removeLocalCoordinateSystem();

    /**get the polygon in local coordinates*/
    int getLocalPolygon();

    /**Get boat x-position*/
    double getX();

    /**Get boat y-position*/
    double getY();

    /**Get boat heading*/
    double getHeading();


    /**Get boat speed*/
    double getSpeed();

    /**Get Depth*/
    double getDepth();
};




#endif // DATA_H
