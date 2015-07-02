#include "singlebeamscanner.hpp"
#include <iostream>
#include "polygon.hpp"
#include "data.hpp"
#include <unistd.h>
#include <limits>

SingleBeamScanner::SingleBeamScanner(Data* dataptr, Polygon* polygonptr)
{
    std::cout << "SingleBeamScanner constructor" << std::endl;
    data = dataptr;
    polygon = polygonptr;
}


SingleBeamScanner::~SingleBeamScanner()
{
    std::cout << "SingleBeamScanner destructor" << std::endl;
}


void SingleBeamScanner::startScan()
{
    std::cout << "SingleBeamScanner: starting scan" << std::endl;

    //first test: make the boat run on the edges of the polygon
    std::vector<double>* lat = polygon->getLatBoundaries();
    std::vector<double>* lon = polygon->getLonBoundaries();

    double boatLat = data->getLat();
    double boatLon = data->getLon();

    //1) find closest node.
    int index = -1;
    double minimum = std::numeric_limits<double>::max();
    //std::cout << minimum << std::endl;
    double d = 0;
    for(int i=0;i< lat->size();i++)
    {
        d = data->calculateDistance(boatLat,boatLon,lat->at(i), lon->at(i));
        std::cout << d << std::endl;
        if(d<minimum)
        {
            index = i;
            minimum = d;
        }
    }

    std::cout << "minimum found: " << minimum << " at index " << index << std::endl;

    //
    double targetLat = lat->at(index);
    double targetLon = lon->at(index);
    double targetSpeed = 3;
    data->setBoatWaypoint_real(targetLat, targetLon);
    data->setBoatSpeed(targetSpeed);

    //std::cout << targetLat << " " << targetLon << std::endl;

    int lap = 1; //0
    double threshold = 30;
    while(lap < 2)
    {
        usleep(300000);
        d = data->calculateDistance(data->getLat(),data->getLon(),targetLat, targetLon);
        //std::cout << "distance to target " << d << std::endl;
        if(d < threshold)
        {
            std::cout << "waypoint reached, picking the next one" << std::endl;
            index++;
            if(index >= lat->size())
            {
                index = 0;
                lap++;
            }

            targetLat = lat->at(index);
            targetLon = lon->at(index);
            data->setBoatWaypoint_real(targetLat, targetLon);
            data->setBoatSpeed(targetSpeed);
        }
    }


}

void SingleBeamScanner::abortScan()
{
    std::cout << "SingleBeamScanner: abort scan" << std::endl;
}
