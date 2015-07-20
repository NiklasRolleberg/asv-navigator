#include "singlebeamscanner.hpp"
#include <iostream>
#include "polygon.hpp"
#include "data.hpp"
#include <unistd.h>
#include <limits>
#include <cmath>

SingleBeamScanner::SingleBeamScanner(Data* dataptr, Polygon* polygonptr, double delta)
{
    std::cout << "scanner:SingleBeamScanner constructor" << std::endl;
    data = dataptr;
    polygon = polygonptr;
}


SingleBeamScanner::~SingleBeamScanner()
{
    std::cout << "scanner:SingleBeamScanner destructor" << std::endl;
}


void SingleBeamScanner::startScan()
{
    std::cout << "scanner:SingleBeamScanner: starting scan" << std::endl;


    /* real coordinates
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

    std::cout << "scanner:minimum found: " << minimum << " at index " << index << std::endl;

    //
    double targetLat = lat->at(index);
    double targetLon = lon->at(index);
    double targetSpeed = 3;

    data->setBoatWaypoint_real(targetLat, targetLon);
    //data->setBoatSpeed(targetSpeed);

    //std::cout << targetLat << " " << targetLon << std::endl;

    int lap = 1; //0
    double threshold = 3;
    while(lap < 2)
    {
        usleep(2000000);
        d = data->calculateDistance(data->getLat(),data->getLon(),targetLat, targetLon);
        std::cout << "scanner:distance to target " << d << std::endl;
        if(d < threshold)
        {
            std::cout << "scanner:waypoint reached, picking the next one" << std::endl;
            index++;
            if(index >= lat->size())
            {
                index = 0;
                lap++;
            }

            targetLat = lat->at(index);
            targetLon = lon->at(index);
            data->setBoatWaypoint_real(targetLat, targetLon);
            //data->setBoatSpeed(targetSpeed);
        }
    }
    */

    /**Local */
    //first test: make the boat run on the edges of the polygon
    std::vector<double>* ypoints = polygon->getYBoundaries();
    std::vector<double>* xpoints = polygon->getXBoundaries();

    double boatX = data->getX();
    double boatY = data->getY();

    std::cout << "boat pos: << " << boatX << "," << boatY << std::endl;
    
    //1) find closest node.
    int index = -1;
    double minimum = std::numeric_limits<double>::max();
    //std::cout << minimum << std::endl;
    double d = 0;
    for(int i=0;i< xpoints->size();i++)
    {
        double dx = xpoints->at(i) - boatX;
        double dy = ypoints->at(i) - boatY;
        d = sqrt(dx*dx+dy*dy);
        std::cout << d << std::endl;
        if(d<minimum)
        {
            index = i;
            minimum = d;
        }
    }

    std::cout << "scanner:minimum found: " << minimum << " at index " << index << std::endl;

    double targetX = xpoints->at(index);
    double targetY = ypoints->at(index);
    double targetSpeed = 1;
    
    int lap = 0;
    while(lap < 3)
    {
        usleep(2000000);

        boatX = data->getX();
	boatY = data->getY();
	std::cout << "scanner: boat local coordinates: (" << boatX << "," << boatY << ")" << std::endl; 
        double dx = xpoints->at(index) - boatX;
        double dy = ypoints->at(index) - boatY;
        d = sqrt(dx*dx+dy*dy);
	std::cout << "scanner: Distance: " << d << std::endl;
	if(d<3)
	{
	  index ++;
	  index = index % xpoints->size();
	  targetX = xpoints->at(index);
	  targetY = ypoints->at(index);

	  if(index == 0)
	    lap++;
	}
    }
}

void SingleBeamScanner::abortScan()
{
    std::cout << "scanner:SingleBeamScanner: abort scan" << std::endl;
}
