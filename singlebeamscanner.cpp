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


SingleBeamScanner::SingleBeamScanner(Data* dataptr, Polygon* polygonptr, double d, double t)
{
  std::cout << "scanner:SingleBeamScanner constructor" << std::endl;
  data = dataptr;
  polygon = polygonptr;
  delay = 1000000; //1000000 1s
  delta = d;
  tol = t;

  stop = false;
}


SingleBeamScanner::~SingleBeamScanner()
{
  std::cout << "scanner:SingleBeamScanner destructor" << std::endl;
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

    data->setBoatWaypoint_real(lastTargetLat, lastTargetLon, targetLat, targetLon, targetSpeed,false);
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
          data->setBoatWaypoint_real(lastTargetLat, lastTargetLon, targetLat, targetLon, targetSpeed,false);
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
            data->setBoatWaypoint_real(lastTargetLat, lastTargetLon, targetLat, targetLon, targetSpeed,alse);
            //data->setBoatSpeed(targetSpeed);
        }
    }
*/
 //-------------------------------------------------------------------------------------

  std::cout << "sweeping pattern, delta = " << delta << "\n" << std::endl;

  /*
  //DEBUG
  for(int i=0;i<polygon->nx;i++)
  {
    for(int j=0;j<polygon->ny;j++)
    {
      if(i == polygon->nx/2)
        polygon->matrix[i][j]->setStatus(1);
      //else
      //polygon->matrix[i][j]->setStatus(0);
    }
  }
  polygon->removeRegion(polygon->polygonSegments.at(0));
  polygon->generateRegions();
  */

  //Start scanning the first region
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
    scanRegion(region);
    polygon->removeRegion(region);
    region = NULL;
  }

  while(!stop)
  {
    std::cout << "in the loop" << std::endl;
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
        stop = true;
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
  polygon->saveMatrix();

}

bool SingleBeamScanner::scanRegion(PolygonSegment* region)
{
  bool goToRight = false;//(Math.random() < 0.5); //traveling from left side to right
  bool goToNextLine = true;
  bool skipRest = false; //true -> the boat has to find a new waypoint
  double targetY = data->getY();


  //if the boat has drifted outside the region, change targetY to a point just inside the boundary;
  if(targetY>=region->yMax)
    targetY = region->yMax-0.5*delta;

  if(targetY<=region->yMin)
    targetY = region->yMin+0.5*delta;

  /*
  std::cout << "TargetY: " << targetY << std::endl;
  std::cout << "ymin:    " << region->yMin << std::endl;
  */

  double targetX = region->findX(targetY, !goToRight);

  double targetSpeed = 1.6;
  double lastTargetX = data->getX();
  double lastTargetY = data->getY();


  data->setBoatWaypoint_local(lastTargetX,lastTargetY,targetX,targetY,targetSpeed,false);

  //north or south
  double updown = 0.8;
  if((targetY-region->yMin) > ((region->yMax-region->yMin)/2.0))
    updown = -0.8;

  double dx;
  double dy;
  double targetLine = targetY;

  //start sweeping
  while(!stop)
  {
    usleep(delay);

    double x = data->getX();
    double y = data->getY();
    double depth = 1;


    if(!data->hasCorrectPath(data->yTOlat(lastTargetY),data->xTOlon(lastTargetX),data->yTOlat(targetY),data->xTOlon(targetX),2)){
      std::cout << "wrong path, sending path again" << std::endl;
      data->setBoatWaypoint_local(lastTargetX,lastTargetY,targetX,targetY,targetSpeed,false);
    }


    dx = targetX-x;
    dy = targetY-y;
    //--set speed--

    //update depth
    if (!updateDepth(x,y,depth,false))
    {
      //this area has been scanened several times before -> abort scan
      std::cout << "Boat stuck aborting scan" << std::endl;
      std::cout << std::endl;
      return false;
    }

    polygon->updateView(x,y);

    std::cout << "Distance to target: " << sqrt(dx*dx + dy*dy) << std::endl;
    //target reached -> choose new target
    if(sqrt(dx*dx + dy*dy) < tol || skipRest)
    {
      lastTargetX = targetX;
      lastTargetY = targetY;

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
      if(targetY > region->yMax || targetY < region->yMin || !region->contains(targetX,targetY))
      {
        std::cout << "Scanning completed, min/max y reached" << std::endl;
        /*
        if(targetY > region->yMax )
          std::cout << "MAX" << std::endl;
        if(targetY < region->yMin )
          std::cout << "MIN" << std::endl;
        if(!region->contains(targetX,targetY))
          std::cout << "OUTSIDE" << std::endl;
        std::cout << "Updown: " << updown << std::endl;
        std::cout << "TargetY: " << targetY << std::endl;
        std::cout << "ymax:    " << region->yMax << std::endl;
        std::cout << "ymin:    " << region->yMin << std::endl;
        */

        //stop = true;
        break;
        //kex.setSpeed(0);
      }

      std::cout << "TARGET: " << targetX << " " << targetY <<std::endl;
      data->setBoatWaypoint_local(lastTargetX,lastTargetY,targetX,targetY,targetSpeed,false);
      if(skipRest)
      {
        skipRest = false;
        usleep(delay);
      }

      /*
      //DEBUG
      //change status of elements
      double DX = (lastTargetX-targetX);
      double DY = (lastTargetY-targetY);
      double D = sqrt(DX*DX+DY*DY);
      DX = DX/D;
      DY = DY/D;
      for(int i=0;i<D;i++)
      {
          updateDepth(targetX + DX*i,targetY + DY*i, 2, false);
      }
      */

    }
    //close to land
    //TODO add land following

  }
  std::cout << std::endl;
  return true;
}

//TODO Gör om
bool SingleBeamScanner::gotoRegion(Closest target)
{
  data->setBoatWaypoint_local(0,0,target.x,target.y,1.6,true);
  double dx = data->getX() - target.x;
  double dy = data->getY() - target.y;
  double d = sqrt(dx*dx+dy*dy);
  while(d > tol)
  {
    usleep(1000000);
    dx = data->getX() - target.x;
    dy = data->getY() - target.y;
    d = sqrt(dx*dx+dy*dy);
    polygon->updateView(data->getX(),data->getY());
  }
  return true; //false = failed
}


bool SingleBeamScanner::updateDepth(double x, double y, double depth, bool followingLand)
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

  //check if the boat allready is inside a polygonsegment
  for(int i=0;i<<polygon->polygonSegments.size();i++)
  {
    if(polygon->polygonSegments.at(i)->contains(startX,startY))
    {
      std::cout << "boat is allready inside a polygonsegment" << std::endl;
      Closest(startX,startY,polygon->polygonSegments.at(i));
    }
  }


  //create a cost matrix
  int ix = (int) round((startX - polygon->minX) / polygon->delta);
  int iy = (int) round((startY - polygon->minY) / polygon->delta);

  if(ix<0)
    ix = 0;
  if(ix>=polygon->nx)
    ix = polygon->nx-1;
  if(iy<0)
    iy = 0;
  if(iy>=polygon->ny)
    iy = polygon->ny-1;

  double** cost = polygon->createCostMatrix(ix, iy);

  if(cost == NULL)
    return Closest(-1,-1,nullptr);

/*
  std::cout << "Cost matrix:" << std::endl;
  for(int j=0;j<polygon->ny;j++)
  {
    for(int i=0;i<polygon->nx-1;i++)
    {
      std::cout << cost[i][j] << ", ";
    }
    std::cout << cost[polygon->nx-1][j] << std::endl;
  }
*/

  Element* target = NULL;
  PolygonSegment* targetRegion = NULL;
  double min = std::numeric_limits<double>::max();
  //std::cout << "findClosest: regions.size() " << polygon->polygonSegments.size() << std::endl;

  for(int i=0;i<polygon->polygonSegments.size();i++)
  {
    for(int j = 0;j<polygon->polygonSegments.at(i)->getBoundaryElements()->size();j++)
    {
      Element* e = polygon->polygonSegments.at(i)->getBoundaryElements()->at(j);
      double c1 = cost[e->getIndexX()][e->getIndexY()];
      double targeted = polygon->matrix[e->getIndexX()][e->getIndexY()]->getTimesTargeted();
      double c2 = (c1 +1) * targeted;
      if(c2<min && c1 != -1 && targeted < 2 )
      {
        std::cout << "targeted: " << targeted << std::endl;
        //std::cout << "cost: " << c2 << " index:" << e->getIndexX() << "," << e->getIndexY() << std::endl;
        min = c2;
        target = e;
        targetRegion = polygon->polygonSegments.at(i);
      }
    }
  }

  //delete the matrix
  for (int i = polygon->nx-1; i >= 0; --i)
  {
    delete[] cost[i];
  }
  delete[] cost;

  //TODO return the one with the lowest value

  if(target== NULL)
    return Closest(-1,-1,nullptr);

  /*
  //DEBUG
  polygon->matrix[target->getIndexX()][target->getIndexY()]->setStatus(2);
  polygon->updateView(data->getX(),data->getY());
  usleep(1000000);
  */

  polygon->matrix[target->getIndexX()][target->getIndexY()]->targeted();
  return Closest(target->getX(),target->getY(),targetRegion);
}


void SingleBeamScanner::abortScan()
{
  std::cout << "scanner:SingleBeamScanner: abort scan" << std::endl;
}
