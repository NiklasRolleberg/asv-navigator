#include "singlebeamscanner.hpp"
#include <iostream>
#include <sstream>
#include "polygon.hpp"
#include "data.hpp"
#include <unistd.h>
#include <limits>
#include <cmath>
#include "segment.hpp"
#include <iomanip>
//#include element

SingleBeamScanner::SingleBeamScanner(Data* dataptr, Polygon* polygonptr, double d, double t)
{
  std::cout << "scanner:SingleBeamScanner constructor" << std::endl;
  data = dataptr;
  polygon = polygonptr;
  delay = 1000000;
  delta = d;
  tol = t;
}


SingleBeamScanner::~SingleBeamScanner()
{
  std::cout << "scanner:SingleBeamScanner destructor" << std::endl;
  //TODO delete polygon?
}


void SingleBeamScanner::startScan()
{
  std::cout << "Writing polygon coordinates to log" << std::endl;
  std::stringstream s;
  s << "POLYGONCOORDINATES:" <<  std::setprecision(8);
  for(int i=0;i< polygon->getLatBoundaries()->size();i++)
  {
    s << "(" << polygon -> getLatBoundaries()->at(i) << ',' << polygon->getLonBoundaries()->at(i) << ")";
  }
  //s << std::endl;
  data->writeToLog(s.str());

  std::cout << "scanner:SingleBeamScanner: starting scan" << std::endl;

    //real coordinates
    /*
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
        //std::cout << d << std::endl;
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

    int lap = 0; //0
    double threshold = 5;
    while(lap < 4)
    {
        usleep(500000);
        d = data->calculateDistance(data->getLat(),data->getLon(),targetLat, targetLon);
        std::cout << "scanner:distance to target " << d << std::endl;
        //data->data_transmitterptr->sendMessage("$MSGCP,*00");
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

    /**Local
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

    data->setBoatWaypoint_local(targetX,targetY);

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

      if(d<10)
      {
        index ++;
        index = index % xpoints->size();
        targetX = xpoints->at(index);
        targetY = ypoints->at(index);
        std::cout << "target reached" << std::endl;
        data->setBoatWaypoint_local(targetX,targetY);

         if(index == 0)
            lap++;
       }
   }
   */

  std::cout << "sweeping pattern, delta = " << delta << std::endl;

  PolygonSegment* region = polygon->polygonSegments.at(0);

  bool goToRight = false;//(Math.random() < 0.5); //traveling from left side to right
  bool goToNextLine = true;
  bool skipRest = false; //true -> the boat has to find a new waypoint
  double targetY = data->getY();
  double targetX = region->findX(targetY, !goToRight);
  data->setBoatWaypoint_local(targetX,targetY);

  //north out south
  int updown = 1;
  if(targetY > (region->yMax/2.0))
    updown = -1;

  double dx;// = targetX-data->getX();
  double dy;// = targetY-data->getY();
  double targetLine = targetY;

  bool stop = false;
  //double tol = 5; // radius around target

  //start sweeping
  while(!stop)
  {
    dx = targetX-data->getX();//data[0];
    dy = targetY-data->getY();//data[1];
    //--set speed--

    std::cout << "Distance to target: " << sqrt(dx*dx + dy*dy) << std::endl;
    //target reached -> choose new target
    if(sqrt(dx*dx + dy*dy) < tol || skipRest)
    {
      double lastTargetX = targetX;
      double lastTargetY = targetY;

      //System.out.println("Waypoint reached");
      if(goToNextLine)
      {
        std::cout << "GO TO NEXT LINE" << std::endl;
        targetLine +=delta*updown;
        targetY = targetLine;
        targetX = region->findX(targetY, !goToRight);
        goToNextLine = false;
      }
      else if(goToRight && !goToNextLine)
      {
        std::cout << "GO TO RIGHT" << std::endl;
        targetY = targetLine;
        targetX = region->findX(targetY,true);
        goToRight = false;
        goToNextLine = true;
      }
      else if(!goToRight && !goToNextLine)
      {
        std::cout << "GO TO LEFT" << std::endl;
        targetY = targetLine;
        targetX = region->findX(targetY,false);
        goToRight = true;
        goToNextLine = true;
      }
      if(targetY > region->yMax || targetY < region->yMin)
      {
        std::cout << "Scanning completed, min/max y reached" << std::endl;
        stop = true;
        break;
        //kex.setSpeed(0);
      }

      std::cout << "TARGET: " << targetX << " " << targetY <<std::endl;
      //xte.setWaypoint(lastTargetX, lastTargetY, targetX, targetY);
      data->setBoatWaypoint_local(targetX,targetY);
      if(skipRest)
      {
        skipRest = false;
        usleep(delay);
      }
    }
    //close to land

    usleep(delay);
  }
  std::cout <<"SweepingPattern done" << std::endl;
}

void SingleBeamScanner::abortScan()
{
  std::cout << "scanner:SingleBeamScanner: abort scan" << std::endl;
}
