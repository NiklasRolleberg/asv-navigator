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
*/
 //-------------------------------------------------------------------------------------

  std::cout << "sweeping pattern, delta = " << delta << "\n" << std::endl;


  /**Start scanning the first region */
  int index = -1;
  for(int i=0;i<polygon->polygonSegments.size();i++)
  {
    if(polygon->polygonSegments.at(i)->contains(data->getX(),data->getY()))
      index = i;
  }

  if(index == -1)
  {
    std::cout << "boat is not inside the polygon" << std::endl;
    //return;
  }

  PolygonSegment* region = NULL;

  if(index != -1)
  {
    region = polygon->polygonSegments.at(index);
    //scanRegion(region);
    polygon->removeRegion(region);
    region = NULL;
  }


  //DEBUG
  for(int i=0;i<polygon->nx;i++)
  {
    for(int j=0;j<polygon->ny;j++)
    {
      if(polygon->matrix[i][j]->getStatus() == 0 && i == polygon->nx/2)
        polygon->matrix[i][j]->setStatus(1);
    }
  }
  

  while(true)
  {
    //(rework regions) TODO kanske senare

    //if regions left in list pick the closest accessible one
    //else identify new regions and pick the closest accessible one
    polygon->saveMatrix();
    Closest c = findClosest(data->getX(),data->getY());
    if(c.x == -1 || c.y==-1 || c.region == nullptr)
    {
      //no path found try generating new regions
      //TODO remove all old regions
      polygon->generateRegions();

      c = findClosest(data->getX(),data->getY());
      if(c.x == -1 || c.y==-1 || c.region == nullptr)
      {
        std::cout << "No accessible regions left to scan\n" << std::endl;
        break; //no path found -> nothing left to scan
      }
    }

    std::cout << "new region found" << std::endl;

    //go to that region
    if(!gotoRegion(c)) {
      std::cout << "go to region failed" << std::endl;
      continue; //continue with something else
    }

    std::cout << "new region reached\n" << std::endl;

    //scan that region
    scanRegion(c.region);
    polygon->removeRegion(c.region);
  }


  std::cout <<"SweepingPattern done" << std::endl;
  //polygon->saveMatrix();

}

bool SingleBeamScanner::scanRegion(PolygonSegment* region)
{
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
    double depth = 1;

    /*
    if(!data->hasCorrectPath(0,0,data->yTOlat(targetY),data->xTOlon(targetX),2)){
      std::cout << "wrong path, sending path again" << std::endl;
      data->setBoatWaypoint_local(0,0,targetX,targetY,targetSpeed);
    }
    */

    dx = targetX-x;
    dy = targetY-y;
    //--set speed--

    //update depth
    if (!updateDepth(polygon,x,y,depth,false))
    {
      //this area has been scanened several times before -> abort scan
      std::cout << "Boat stuck aborting scan" << std::endl;
      std::cout << std::endl;
      return false;
    }

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
  std::cout << std::endl;
  return true;
}

bool SingleBeamScanner::gotoRegion(Closest target)
{
  return true; //false = failed
}


bool SingleBeamScanner::updateDepth(Polygon* polygon, double x, double y, double depth, bool followingLand)
{
  //find index
  int ix = (int) round((x - polygon->minX) / polygon->delta);
  int iy = (int) round((y - polygon->minY) / polygon->delta);

  if(ix < 0 || ix >= polygon->nx || iy < 0 || iy >= polygon->ny)
  {
    std::cout << "index Oob" << std::endl;
    return true; //TODO kanske false;
  }

  if(!followingLand)
  {

    //std::cout << "Updating depth at (" << ix << "," << iy << ")" << std::endl;
    /*
    std::cout << "x:" << x << std::endl;
    std::cout << "y:" << y << std::endl;
    */
    if(polygon->matrix != NULL)
    {
      polygon->matrix[ix][iy]->updateDepth(depth);
      if(polygon->matrix[ix][iy]->getTimesVisited() > 5*delta) //hitta på en bra gräns
      {
        return true; //ska vara false
      }
    }
  }
  else
  {
    std::cout << "Not implemented" << std::endl;
  }

  return true;
}


Closest SingleBeamScanner::findClosest(int startX,int startY)
{
  if(polygon->polygonSegments.empty())
    return Closest(-1,-1,nullptr);
  return Closest(startX,startY,polygon->polygonSegments.at(0));
}


void SingleBeamScanner::abortScan()
{
  std::cout << "scanner:SingleBeamScanner: abort scan" << std::endl;
}
