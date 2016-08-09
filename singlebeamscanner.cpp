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
#include <vector>

SingleBeamScanner::SingleBeamScanner(Data* dataptr, Polygon* polygonptr,int inputdelay, double d, double t)
{
  std::cout << "scanner:SingleBeamScanner constructor" << std::endl;
  data = dataptr;
  polygon = polygonptr;
  delay = inputdelay; //1000000 1s
  delta = d;
  tol = t;

/*
  speed_level1 = 1;
  speed_level2 = 0.7;
  speed_level3 = 0.5;
*/

  depthThreshold = 5;

  stop = false;
}


SingleBeamScanner::~SingleBeamScanner()
{
  std::cout << "scanner:SingleBeamScanner destructor" << std::endl;
}

void SingleBeamScanner::abortScan()
{
  std::cout << "scanner:SingleBeamScanner: abort scan" << std::endl;
  stop = true;
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

  //(1) find the closest element to the boat
  //(2) calculate the values of all surrounding elements
  //(3) go to the element with the highest value
    //(3.1) sucsess! goto (2)
    //(3.2) failed due to shallow depth -> mark target as land
      //(3.2.1) go back to the previous element without checking depth
        //(3.2.1.1) sucsess! goto (2)
        //(3.2.1.2) failed. The boat is probably stuck on land. abort!
 //(4) All values are equal=0;
 //(5) Use A* to look for elements further away
 //(6) Pick the closest of these elements and go there
  //(6.1) sucsess goto (2)
  //(6.2) failed something is wrong. abort

  while(!stop)
  {
    //(1)
    double Xpos = data->getX();
    double Ypos = data->getY();

    int x = (int) round((Xpos - polygon->minX) / polygon->delta);
    int y = (int) round((Ypos - polygon->minY) / polygon->delta);
    std::cout << "boat index: (" << x << "," << y << ")" << std::endl;
    std::cout << "max Index : (" << polygon->nx << "," << polygon->ny << ")" << std::endl;

    if(x < 0 ||
       x >= polygon->nx ||
       y < 0 ||
       y >= polygon->ny)
    {
      std::cout << "Boat is not inside the polygon" << std::endl;
      std::cout << "aborting scan!" << std::endl;
      return;
    }

    //(2) calculate the values of all surrounding elements
    Target t = findClose(x,y);
    if(t.status!=0)
    {
      std::cout << "no good element found" << std::endl;

      //TODO findFarAway()
      return;
    }
    bool sucsess = gotoElement(t.x,t.y,true);
    if(!sucsess)
    {
      //failed
      polygon->matrix[t.x][t.y] -> setStatus(2); //mark as land
  		polygon->idland(); //mark enclosed areas as land
    }
    std::cout << "itteration complete" << std::endl;
    usleep(100000);
  }


  std::cout << "scan complete" << std::endl;
  polygon->saveMatrix();

  usleep(3000000);
}


bool SingleBeamScanner::gotoElement(int x, int y, bool ingoreDepth)
{
  std::cout << "gotoElement (" << x << "," << y << ")" << std::endl;
  Element* target = polygon->matrix[x][y];

  data->setBoatWaypoint_local(0,0,target->getX(),target->getY(),1,true);
  target->updateDepth(5);
  polygon->updateView(data->getX(),data->getY());
  return true;
}

bool SingleBeamScanner::traveltoElement(int x, int y)
{
  Element* target = polygon->matrix[x][y];
  int currentX,currentY;

  data->setBoatWaypoint_local(0,0,target->getX(),target->getY(),1,true);
  polygon->updateView(data->getX(),data->getY());
  return true;
}

Target SingleBeamScanner::findClose(int x,int y)
{
  std::cout << "findclose" << std::endl;
  std::vector<Element*> neighbours;
  int x_arr[] = {x , x+1, x+1 ,x+1 ,x ,x-1 ,x-1 ,x-1};
  int y_arr[] = {y-1 , y-1, y ,y+1 ,y+1 ,y+1 ,y ,y-1};

  for(int i=0;i<8;i++)
  {
    if(x_arr[i]<0 ||
       x_arr[i]>=polygon->nx ||
       y_arr[i]<0 ||
       y_arr[i]>=polygon->ny)
        continue;

    if(polygon->matrix[x_arr[i]][y_arr[i]]->getStatus() == 5)
      continue;

    if(polygon->matrix[x_arr[i]][y_arr[i]]->getStatus() == 0 ||
       polygon->matrix[x_arr[i]][y_arr[i]]->getStatus() == 1)
       neighbours.push_back(polygon->matrix[x_arr[i]][y_arr[i]]);
  }

  std::cout << "neighbours: " << neighbours.size() << std::endl;
  for(int i=0;i<neighbours.size();i++)
  {
    std::cout << "(" << neighbours.at(i)->getIndexX() << "," << neighbours.at(i)->getIndexY() << ")" << std::endl;
  }

  //calulate the values of the neighbours
  double highestValue = -std::numeric_limits<double>::max();
  bool unscanned = false;
  int index = -1;

  double scanweight = 1.0;
  double nearweight = 1.0;
  double headingweight = 1.0;

  for(int i=0;i<neighbours.size();i++)
  {
    Element* n = neighbours.at(i);

    double scanVal = scanweight * scanValue(n->getIndexX(),n->getIndexY(),x,y,5);
    double nearVal = 0;//nearweight * nearValue(n->getIndexX(),n->getIndexY(),x,y,1);
    double headingVal = headingweight * headingValue(n->getIndexX(),n->getIndexY(),x,y);

    std::cout << "\nscanValue    : " << scanVal << std::endl;
    //std::cout << "nearValue    : " << nearVal << std::endl;
    //std::cout << "headingValue : " << headingVal << std::endl;
    //std::cout << "highestValue:  " << highestValue << std::endl;

    double value = scanVal + nearVal + headingVal;

    if(!unscanned)
    {
      if(n->getStatus() == 0)
      {
        highestValue = value;
        index = i;
        unscanned = true;
      }
      else if(value > highestValue)
      {
        highestValue = value;
        index = i;
      }
    }
    else
    {
      if(unscanned && n->getStatus() == 0 && value > highestValue)
      {
        highestValue = value;
        index = i;
      }
    }
  }

  if(index != -1)
  {
    std::cout << "Best target: (" << neighbours.at(index)->getIndexX() << "," << neighbours.at(index)->getIndexY() << ")" << std::endl;
    int status = 0; //sucsess
    return Target(neighbours.at(index)->getIndexX(),neighbours.at(index)->getIndexY(),status);
  }

  int status = 1; //failed to find a good target
  return Target(x,y,status);

}

Target SingleBeamScanner::findFarAway(int currentX,int currentY)
{
  int status = 0; //sucsess
  return Target(currentX,currentY,status);
}


///////////////////// Value functions /////////////////////////

double SingleBeamScanner::scanValue(int x,int y,int originX,int originY, int recursivedepth)
{
  if(x<0||x>=polygon->nx ||
     y<0||y>=polygon->ny ||
     (x==originX && y == originY))
     return 0;

  int status = polygon->matrix[x][y]->getStatus();

  if(recursivedepth == 0)
  {
    if(status == 0)
      return 10.0 / ((x-originX)*(x-originX) + (y-originY)*(y-originY));
    if(status == 1)
      return 0;
    if(status == 5)
      return -1;
    return -10;
  }

  double a = 0;
  if(polygon->matrix[x][y]->getStatus() == 0)
    a = 10.0 / ((x-originX)*(x-originX) + (y-originY)*(y-originY));

  if(polygon->matrix[x][y]->getStatus() == 0 ||
     polygon->matrix[x][y]->getStatus() == 1)
    return a + (
      scanValue(x+1,y, originX,originY, recursivedepth-1) +
      scanValue(x-1,y, originX,originY, recursivedepth-1) +
      scanValue(x,y+1, originX,originY, recursivedepth-1) +
      scanValue(x,y-1, originX,originY, recursivedepth-1) +

      scanValue(x+1,y-1, originX,originY, recursivedepth-1) + //diagonalt
      scanValue(x-1,y+1, originX,originY, recursivedepth-1) +
      scanValue(x+1,y+1, originX,originY, recursivedepth-1) +
      scanValue(x-1,y-1, originX,originY, recursivedepth-1));

  return 0;
}

double SingleBeamScanner::nearValue(int x,int y,int originX,int originY, int recursivedepth)
{
  return 0;
}

double SingleBeamScanner::headingValue(int x,int y, int originX, int originY)
{
  double boatHeading = data->getHeading();
  std::cout << "BoatHeading: " << boatHeading << std::endl;

  double dx = (x-originX);
  double dy = (y-originY);
  double alpha = asin(dy/sqrt(dx*dx+dy*dy));
  if(dx<0)
    alpha = M_PI - alpha;

    std::cout << "calculated heading:" << alpha << std::endl;

  return -50*abs(boatHeading-alpha);

  return 0;
}
