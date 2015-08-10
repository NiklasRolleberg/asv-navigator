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
    double lastTargetLat = 0;
    double lastTargetLon = 0;
    double targetSpeed = 30;

    data->setBoatWaypoint_real(lastTargetLat, lastTargetLon, targetLat, targetLon, targetSpeed);
    //data->setBoatSpeed(targetSpeed);

    //std::cout << targetLat << " " << targetLon << std::endl;

    int lap = 0; //0
    while(lap < 4)
    {
        usleep(750000);
        d = data->calculateDistance(data->getLat(),data->getLon(),targetLat, targetLon);
        std::cout << "scanner:distance to target " << d << std::endl;

        if(!data->hasCorrectPath(lastTargetLat, lastTargetLon, targetLat, targetLon, targetSpeed))
        {
          std::cout << "resending path" << std::endl;
          data->setBoatWaypoint_real(lastTargetLat, lastTargetLon, targetLat, targetLon, targetSpeed);
        }

        if(d < tol)
        {
            std::cout << "scanner:waypoint reached, picking the next one" << std::endl;
            index++;
            if(index >= lat->size())
            {
                index = 0;
                lap++;
            }

            lastTargetLat = targetLat;
            lastTargetLon = targetLon;

            targetLat = lat->at(index);
            targetLon = lon->at(index);
            data->setBoatWaypoint_real(lastTargetLat, lastTargetLon, targetLat, targetLon, targetSpeed);
            //data->setBoatSpeed(targetSpeed);
        }
    }

 //-------------------------------------------------------------------------------------
/*
  std::cout << "sweeping pattern, delta = " << delta << std::endl;

  PolygonSegment* region = polygon->polygonSegments.at(0);

  bool goToRight = false;//(Math.random() < 0.5); //traveling from left side to right
  bool goToNextLine = true;
  bool skipRest = false; //true -> the boat has to find a new waypoint
  double targetY = data->getY();
  double targetX = region->findX(targetY, !goToRight);
  double targetSpeed = 5;
  data->setBoatWaypoint_local(0,0,targetX,targetY,targetSpeed);

  //north or south
  int updown = 1;
  if(targetY > (region->yMax/2.0))
    updown = -1;

  double dx;
  double dy;
  double targetLine = targetY;

  bool stop = false;

  //start sweeping
  while(!stop)
  {
    usleep(delay);

    double x = data->getX();
    double y = data->getY();
    double depth = 10;


    if(!data->hasCorrectPath(0,0,data->yTOlat(targetY),data->xTOlon(targetX),2)){
      std::cout << "wrong path, sending path again" << std::endl;
      data->setBoatWaypoint_local(0,0,targetX,targetY,targetSpeed);
    }



    dx = targetX-x;
    dy = targetY-y;
    //--set speed--

    //update depth
    updateDepth(polygon,x,y,depth,false);


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
      data->setBoatWaypoint_local(lastTargetX,lastTargetY,targetX,targetY,targetSpeed);
      if(skipRest)
      {
        skipRest = false;
        usleep(delay);
      }
    }
    //close to land
    //TODO add land following

  }
  std::cout <<"SweepingPattern done" << std::endl;
  polygon->saveMatrix();
  */
}

void SingleBeamScanner::updateDepth(Polygon* polygon, double x, double y, double depth, bool followingLand)
{
  //find index
  int ix = (int) round((x - polygon->minX) / polygon->delta);
  int iy = (int) round((y - polygon->minY) / polygon->delta);

  if(ix < 0 || ix > polygon->nx || iy < 0 || iy > polygon->ny)
  {
    std::cout << "index Oob" << std::endl;
    return;
  }

  if(!followingLand)
  {
    /*
    std::cout << "Updating depth at (" << ix << "," << iy << ")" << std::endl;
    std::cout << "x:" << x << std::endl;
    std::cout << "y:" << y << std::endl;
    */
    if(polygon->matrix != NULL)
      polygon->matrix[ix][iy]->updateDepth(1);
  }
  else
  {
    std::cout << "Not implemented" << std::endl;
  }


}

void SingleBeamScanner::abortScan()
{
  std::cout << "scanner:SingleBeamScanner: abort scan" << std::endl;
}
