#include "data.hpp"
#include <iostream>
//#include <thread>   // std::thread
#include <unistd.h>
#include "transmitter.hpp"
#include "polygon.hpp"
#include <cmath>

Data::Data(Transmitter* transmitter,int delay, int arg3)
{
    std::cout << "Data constructor" << std::endl;
    data_transmitterptr = transmitter;
    data_delay = delay;
    //Get initial values for variables

    //59.352884, 18.073585
    boat_latitude = 59.352884;
    boat_longitude = 18.073585;
    boat_heading_real = 0;

    boat_xpos = 0;
    boat_ypos = 0;
    boat_heading_local = 0;
    boat_speed = 0;
    boat_depth = 0;
}

Data::~Data()
{
    std::cout << "Data destructor" << std::endl;
    data_stop = true;
    if(data_threadptr != nullptr)
        if(data_threadptr->joinable())
            data_threadptr->join();
    delete data_threadptr;
}

void Data::start()
{
    std::cout << "Data start" << std::endl;
    if(data_threadptr != nullptr)
        std::cerr << "Memory leak incoming.." << std::endl;
        //data_threadptr->terminate();


    data_threadptr = new std::thread(&Data::threadLoop, this);
    //data_threadptr->join();
    //threadLoop();
}

void Data::stop()
{
    std::cout << "Data stop" << std::endl;
    data_stop = true;
    if(data_threadptr != nullptr)
        if(data_threadptr->joinable())
            data_threadptr->join();
}

double Data::getLat()
{
    //std::cout << "Data get latitude" << std::endl;
    return boat_latitude;
}

double Data::getLon()
{
    //std::cout << "Data get longitude" << std::endl;
    return boat_longitude;
}

double Data::getLatLongHeading()
{
    //std::cout << "Data getHeading (Lat,Long)" << std::endl;
    return boat_heading_real;
}


void Data::setLocalCoordinateSystem(Polygon* polygon)
{
    std::cout << "Polygon added, starting creation of local coordinate system" << std::endl;
}

void Data::removeLocalCoordinateSystem()
{
    std::cout << "remove local coordinate system" << std::endl;
}

Polygon* Data::getLocalPolygon()
{
    std::cout << "get polygon in local coordinates" << std::endl;
    return localPolygon;
}


double Data::getX()
{
    std::cout << "Data getX" << std::endl;
}

double Data::getY()
{
    std::cout << "Data getY" << std::endl;
}

double Data::getHeading()
{
    std::cout << "Data getHeading" << std::endl;
}

double Data::getSpeed()
{
    std::cout << "Data getSpeed" << std::endl;
}

double Data::getDepth()
{
    std::cout << "Data getDepth" << std::endl;
}

void Data::threadLoop()
{
    std::cout << "Data loop started" << std::endl;
    int i=0;
    while(!data_stop)
    {
        //int a = data_transmitterptr->getMessages();
        //exempel: $MSGPS,1.3,LAT,0.3154,LON,0.900,SOG,0.0,COG,*4A
        //               latitude,longitude,   speed ,  course, checksum
	//TODO read the messages and to stuff..
	
        //data_transmitterptr->requestData();

        std::stringstream s;
        s << "$MSSTS," << i << ",*00";
        data_transmitterptr->sendMessage(s.str());
	i++;
	
        usleep(data_delay);
    }
    std::cout << "Data loop done" << std::endl;
}



void Data::setBoatWaypoint_real(double lat, double lon)
{
    std::cout << "Data: Set real waypoint" << std::endl;

    std::stringstream s;
    s << "$MSSTS," << lat << "," << lon << ",checksum";
    data_transmitterptr->sendMessage(s.str());
    
}

void Data::setBoatWaypoint_local(double x, double y)
{
    std::cout << "Data: Set local waypoint" << std::endl;
}

void Data::setBoatSpeed(double speed)
{
    std::cout << "Data: Set speed" << std::endl;

    std::stringstream s;
    s << "$MSSTS," << speed << ",checksum";
    data_transmitterptr->sendMessage(s.str());
}

double Data::calculateDistance(double lat1,double lon1,double lat2,double lon2)
{
    double R = 6371000; // metres
    double phi1 = lat1 * M_PI / 180;// .toRadians();
    double phi2 = lat2 * M_PI / 180;// .toRadians();
    double dphi = (lat2-lat1) * M_PI / 180;// .toRadians();
    double dlambda = (lon2-lon1) * M_PI / 180;// .toRadians();

    double a = sin(dphi/2) * sin(dphi/2) +
            cos(phi1) * cos(phi2) *
            sin(dlambda/2) * sin(dlambda/2);
    double c = 2 * atan2(sqrt(a), sqrt(1-a));

    return R * c;


}
