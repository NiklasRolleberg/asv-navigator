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

SingleBeamScanner::SingleBeamScanner(Data* dataptr, Polygon* polygonptr,int inputdelay,
                                     double d, double t, bool backup, std::string fname)
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

  //std::cout << fname <<  std::endl;

  makebackup = backup;
  backupFilename = fname;

  depthThreshold = 2;
  targetSpeed = 1;

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

  //polygon->saveAll("backup/polygondata.xml");
  //return;

  int save = 1;

  while(!stop)
  {
    save++;
    save=save%10; //save every 10:th loop
    if(save == 0 && makebackup)
    {
      //fist write a temp file in the backup folder backup
      //(this is to always have a beckup file if the program kraches during the writing)
      polygon->saveAll("backup/temp.xml", data->getLat(), data->getLon());
      //then write the real file
      polygon->saveAll(backupFilename, data->getLat(), data->getLon());
    }
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
      std::cout << "looking for elements further away" << std::endl;
      t = findFarAway(x,y);
      if(t.status!=0)
      {
        std::cout << "no accessible regions left to scan" << std::endl;
        polygon->saveMatrix();
        return; // scan complete
      }
      if(!traveltoElement(t.x,t.y))
      {
        std::cout << "traveltoElement failed" << std::endl;
        polygon->saveMatrix();
        usleep(10000000);
        return; //abort scan (the boat is probably stuck somwere)
      }
      continue; //goto (2)
    }
    bool sucsess = gotoElement(t.x,t.y,false);
    if(!sucsess)
    {
      //failed
      polygon->matrix[t.x][t.y] -> setStatus(2); //mark as land
  		polygon->idland(); //mark enclosed areas as land
      if(!gotoElement(x,y,true)) //go back to previous element
      {
        std::cout << "failed to go back to previous element" << std::endl;
        return;
      }
    }
    std::cout << "itteration complete" << std::endl;
    usleep(100000);
  }


  std::cout << "scan complete" << std::endl;
  polygon->saveMatrix();

  usleep(3000000);
}


bool SingleBeamScanner::gotoElement(int x, int y, bool ignoreDepth)
{
  std::cout << "gotoElement (" << x << "," << y << ")" << std::endl;
  Element* target = polygon->matrix[x][y];

  double targetX = target->getX();
  double targetY = target->getY();


  //DEBUG

  //if(rand() % 5 == 0 && !ignoreDepth && target->getStatus() != 1)
    //return false;

/*
  int cx = polygon->nx/3;
  int cy = polygon->ny/3;
  if(sqrt((x-cx)*(x-cx)+(y-cy)*(y-cy))<3 && !ignoreDepth)
    return false;

  cx = 2*polygon->nx/3;
  cy = 2*polygon->ny/3;
  if(sqrt((x-cx)*(x-cx)+(y-cy)*(y-cy))<3 && !ignoreDepth)
    return false;

  if(((polygon->nx-x)*(polygon->nx-x)+(polygon->ny-y)*(polygon->ny-y)) < 100 && !ignoreDepth)
    return false;
  if(((x)*(x)+(polygon->ny-y)*(polygon->ny-y)) < 256 && !ignoreDepth)
    return false;
  if( x == 20 && y<10 && !ignoreDepth)
    return false;
  if( x == 15 && y<10 && !ignoreDepth)
    return false;
  if( (polygon->nx-x) < 8 && y==13 && !ignoreDepth)
    return false;
  if( (polygon->nx-x) < 5 && y==10 && !ignoreDepth)
    return false;
  if( (polygon->nx -x) < 5 && y==7 && !ignoreDepth)
    return false;
*/

  data->setBoatWaypoint_local(0,0,targetX,targetY,1,true);

  double currentX;
  double currentY;

  while(!stop)
  {
    usleep(delay);

    currentX = data->getX();
    currentY = data->getY();

    //check if path is correct
  if(!data->hasCorrectPath(0,0,data->yTOlat(targetY),data->xTOlon(targetX),targetSpeed))
  {
    std::cout << "wrong path, sending path again" << std::endl;
    data->setBoatWaypoint_local(0,0,targetX,targetY,targetSpeed,true);
  }

    updateDepth(currentX,currentY,data->getDepth());

    double d = sqrt((targetX-currentX)*(targetX-currentX) + (targetY-currentY)*(targetY-currentY));
    std::cout << "Distance left to target:" << d << "m" << std::endl;
    std::cout << "Depth: " << data->getDepth();
    if(d<tol)
      return true;

    //if(0.5*(data->getDepth_Right()+data->getDepth_Left()) < depthThreshold)
    if(data->getDepth() < depthThreshold)
    {
      std::cout << "near land" << std::endl;
      std::cout << "Down    :" << data->getDepth() << std::endl;
      std::cout << "Right   :" << data->getDepth_Right() << std::endl;
      std::cout << "Left    :" << data->getDepth_Left() << std::endl;

      if(!ignoreDepth)
        return false;
    }

  }


  //target->updateDepth(5);
  polygon->updateView(data->getX(),data->getY(),data->getHeading());
  return true;
}

bool SingleBeamScanner::traveltoElement(int x, int y)
{
  double Xpos = data->getX();
  double Ypos = data->getY();


  int currentX = (int) round((Xpos - polygon->minX) / polygon->delta);
  int currentY = (int) round((Ypos - polygon->minY) / polygon->delta);
  std::cout << "boat index: (" << currentX << "," << currentY << ")" << std::endl;

  if(currentX < 0 ||
     currentX >= polygon->nx ||
     currentY < 0 ||
     currentY >= polygon->ny)
  {
    std::cout << "Boat is not inside the polygon" << std::endl;
    std::cout << "aborting traveltoElement!" << std::endl;
    return false;
  }

  //(1) create a costmatrix around the target
  //(2) start at the boats current index and trevel towards lower cost value untill the target is reached
  double** cost = polygon->createCostMatrix(x, y);

  int targetX = -1;
  int targetY = -1;

  while(!stop)
  {
    if(currentX == x && currentY == y)
    {
      std::cout << "Target reached!" << std::endl;
      //updateDepth(data->getX(),data->getY(),data->getDepth());
      return true;
    }

    //find next waypoint
    //int xIndex[] = {currentX+1, currentX-1, currentX+0, currentX+0, currentX+1, currentX-1, currentX+1, currentX-1};
    //int yIndex[] = {currentY+0, currentY+0, currentY+1, currentY-1, currentY+1, currentY-1, currentY-1, currentY+1}; //diagonalt

    int xIndex[] = {currentX+1, currentX-1, currentX+0, currentX+0};
    int yIndex[] = {currentY+0, currentY+0, currentY+1, currentY-1};

    double min = std::numeric_limits<double>::max();

    //std::cout << "Index:" << std::endl;
    //for(int i=0;i<8;i++)
    //  std::cout << "(" << xIndex[i] << "," << yIndex[i] << ")" << std::endl;

    for(int i=0;i<4;i++)
    {
      std::cout << "looking at(" << xIndex[i] << "," << yIndex[i] << ")" << std::endl;
      std::cout << "current: (" << currentX << "," << currentY << std::endl;

      if(xIndex[i] < 0 || xIndex[i] >= polygon->nx)
        continue;
      if(yIndex[i] < 0 || yIndex[i] >= polygon->ny)
        continue;
      std::cout << "cost: " << cost[xIndex[i]][yIndex[i]] << std::endl;
      if(cost[xIndex[i]][yIndex[i]] == -1)
        continue;

      std::cout << "looking at(" << xIndex[i] << "," << yIndex[i] << ")" << " cost: " << cost[xIndex[i]][yIndex[i]] << std::endl;
      if(cost[xIndex[i]][yIndex[i]] < min && cost[xIndex[i]][yIndex[i]] != -1)
      {
        min = cost[xIndex[i]][yIndex[i]];
        targetX = xIndex[i];
        targetY = yIndex[i];
        std::cout << "min: " << min << std::endl;
      }
    }
    std::cout << "new target: (" << targetX << "," << targetY << ")" << std::endl;
    //usleep(1000000);

    if(targetX == -1 && targetY == -1)
      return false; // failed

    if(gotoElement(targetX,targetY,true))
    {
      currentX = targetX;
      currentY = targetY;
    }
    else
    {
      std::cout << "Failed" << std::endl;
      return false;
    }
  }

  //delete the matrix
  for (int i = polygon->nx-1; i >= 0; --i)
    delete[] cost[i];
  delete[] cost;


  //data->setBoatWaypoint_local(0,0,target->getX(),target->getY(),1,true);
  //polygon->updateView(data->getX(),data->getY());
  return false;
}

Target SingleBeamScanner::findClose(int x,int y)
{
  std::cout << "findclose" << std::endl;
  std::vector<Element*> neighbours;
  //int x_arr[] = {x , x+1, x+1 ,x+1 ,x ,x-1 ,x-1 ,x-1};
  //int y_arr[] = {y-1 , y-1, y ,y+1 ,y+1 ,y+1 ,y ,y-1}; //Diagonalt

  int x_arr[] = {x  , x+1, x   ,x-1};
  int y_arr[] = {y-1, y  , y+1 ,y};


  for(int i=0;i<4;i++)
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

  //std::cout << "neighbours: " << neighbours.size() << std::endl;
  //for(int i=0;i<neighbours.size();i++)
  //{
    //std::cout << "(" << neighbours.at(i)->getIndexX() << "," << neighbours.at(i)->getIndexY() << ")" << std::endl;
  //}

  //calulate the values of the neighbours
  double highestValue = -std::numeric_limits<double>::max();
  bool unscanned = false;
  int index = -1;

  double scanweight = 1;
  double nearweight = -0.1;//.07;
  double headingweight = 0.7;

  for(int i=0;i<neighbours.size();i++)
  {
    Element* n = neighbours.at(i);

    double scanVal = scanweight * scanValue(n->getIndexX(),n->getIndexY(),x,y,0);
    double nearVal = nearweight * nearValue(n->getIndexX(),n->getIndexY(),x,y,0);
    double headingVal = headingweight * headingValue(n->getIndexX(),n->getIndexY(),x,y);

    std::cout << "\nElement      : (" << n->getIndexX() << "," << n->getIndexY() << ")" << std::endl;
    std::cout << "scanValue    : " << scanVal << std::endl;
    std::cout << "nearValue    : " << nearVal << std::endl;
    std::cout << "headingValue : " << headingVal << std::endl;
    //std::cout << "highestValue:  " << highestValue << std::endl;

    double value = scanVal + nearVal + headingVal;

    if(!unscanned)
    {
      if(n->getStatus() == 0)
      {
        highestValue = value;
        index = i;
        unscanned = true;
        std::cout << "unscanned element found" << std::endl;
      }
      else if(value > highestValue && scanVal !=0)
      {
        highestValue = value;
        index = i;
        std::cout << "scanned element is the best so far" << std::endl;
      }
    }
    else
    {
      if(unscanned && n->getStatus() == 0 && value > highestValue)
      {
        highestValue = value;
        index = i;
        std::cout << "unscanned element is the best so far" << std::endl;
      }
    }
  }

  if(index != -1 && highestValue != 0)
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
  std::cout << "Find far away" << std::endl;
  //(1) (create a costmatrix)
  //(2) (look for unscanned elements)
  //(3) check the cost for the neighbouring elements of the unscanned elements
  //(4) pick the neighbour with the lowest cost
  //(5) return the neighbour as target
  //(6) return failed if no elements/ neighbours were found

  //(1)
  double** cost = polygon->createCostMatrix(currentX, currentY);

  //look for unscanned elements
  std::vector<Element*> unscanned;
  std::vector<Element*> targets;

  for(int ix=0;ix<polygon->nx;ix++)
  {
    for(int iy=0;iy<polygon->ny;iy++)
    {
      if(polygon->matrix[ix][iy]->getStatus() == 0)
        unscanned.push_back(polygon->matrix[ix][iy]);
    }
  }
  std::cout << "Number of unscanned elements: " << unscanned.size() << std::endl;

  for(int i=0;i<unscanned.size();i++)
  {
    for(int j=0;j<unscanned.at(i)->getNeighbours()->size();j++)
    {
        if(unscanned.at(i)->getNeighbours()->at(j)->getStatus() == 1)
          targets.push_back(unscanned.at(i)->getNeighbours()->at(j));
    }
  }

  std::cout << "Number of possible targets: " << targets.size() << std::endl;

  double lowestCost = std::numeric_limits<double>::max();
  Element* best = NULL;

  for(int i=0;i<targets.size();i++)
  {
    int ix = targets.at(i)->getIndexX();
    int iy = targets.at(i)->getIndexY();
    if(cost[ix][iy] < lowestCost && !(ix==currentX && iy==currentY))
    {
      lowestCost = cost[ix][iy];
      best = targets.at(i);
    }
    std::cout << "Target:(" << ix << "," << iy << "):" << cost[ix][iy] << std::endl;
  }

  //delete the matrix
  for (int i = polygon->nx-1; i >= 0; --i)
    delete[] cost[i];
  delete[] cost;

  if(best == NULL)
    std::cout << "Target is NULL" << std::endl;

  if(best != NULL)
  {
    int status = 0; //sucsess
    return Target(best->getIndexX(),best->getIndexY(),status);
  }

  int status = 1; //failed
  return Target(currentX,currentY,status);
}


bool SingleBeamScanner::updateDepth(double x, double y, double depth)
{
  //find index
  int ix = (int) round((x - polygon->minX) / polygon->delta);
  int iy = (int) round((y - polygon->minY) / polygon->delta);

  if(ix < 0 || ix >= polygon->nx || iy < 0 || iy >= polygon->ny)
  {
    std::cout << "index Oob" << std::endl;
    return true; //kanske false ?;
  }

  std::cout << "Updating depth at (" << ix << "," << iy << ")" << std::endl;
  if(polygon->matrix != NULL)
  {
    if(polygon->matrix[ix][iy]->getStatus()!=2) //once a element has been marked as land it will always be land
      polygon->matrix[ix][iy]->updateDepth(depth);
    polygon->updateView(x,y,data->getHeading());
    if(polygon->matrix[ix][iy]->getTimesVisited() > delta*delta
      && delta*delta > 30 ) //hitta på en bra gräns
    {
      return false;
    }
  }
  return true;
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
      return 1.0; // ((x-originX)*(x-originX) + (y-originY)*(y-originY));
    if(status == 1)
      return 0;
    if(status == 5)
      return 0;
    return 0;
  }

  double a = 0;
  if(status == 0)
    a = 0.5;//(recursivedepth*recursivedepth+1.0) / ((x-originX)*(x-originX) + (y-originY)*(y-originY));

  if(status == 0 || status == 1)
    return (a +
      scanValue(x+1,y, originX,originY, recursivedepth-1) +
      scanValue(x-1,y, originX,originY, recursivedepth-1) +
      scanValue(x,y+1, originX,originY, recursivedepth-1) +
      scanValue(x,y-1, originX,originY, recursivedepth-1))/8;
      /*
      scanValue(x+1,y-1, originX,originY, recursivedepth-1) + //diagonalt
      scanValue(x-1,y+1, originX,originY, recursivedepth-1) +
      scanValue(x+1,y+1, originX,originY, recursivedepth-1) +
      scanValue(x-1,y-1, originX,originY, recursivedepth-1))*/

  return 0;
}

double SingleBeamScanner::nearValue(int x, int y, int originX, int originY, int recursivedepth)
{
  if(x<0||x>=polygon->nx ||
     y<0||y>=polygon->ny ||
     (x==originX && y == originY))
     return 0;

  int status = polygon->matrix[x][y]->getStatus();
  if(status != 0)
    return 0;

  //check number of scanned neighbours (2 is optimal)
  int xIndex[] = {x+1, x-1, x+0, x+0};
  int yIndex[] = {y+0, y+0, y+1, y-1};

  int scanned = 0;
  for(int i=0;i<4;i++)
  {
    if(xIndex[i] > 0 &&
       xIndex[i] < polygon->nx &&
       yIndex[i] > 0 &&
       yIndex[i] < polygon->ny)
    {
      if(polygon->matrix[xIndex[i]][yIndex[i]]->getStatus() == 1)
        scanned++;
    }
  }

  std::cout << "nearvalue::scanned: " << scanned << std::endl;

  switch (scanned)
  {
    case 0:
      return 0;
      break;
    case 1:
      return 0.3;
      break;
    case 2:
      return 0.6;
      break;
    case 3:
      return 1;
      break;
    default:
      return 0;
  }

}

double SingleBeamScanner::headingValue(int x,int y, int originX, int originY)
{
  double boatHeading = data->getHeading();

  double dx = ((double)x-(double)originX);
  double dy = ((double)y-(double)originY);
  double d = sqrt(dx*dx+dy*dy);

  if(d==0)
    d=0.001;

  double alpha;
  if(dx>0)
    if(dy>0)
      alpha = asin(dy/d);
    else
      alpha = 2*M_PI+asin(dy/d);
  else
    alpha = M_PI-asin(dy/d);

  double anticlockwise = std::max(boatHeading,alpha) - std::min(boatHeading,alpha);
  double clockwise = (2*M_PI) - anticlockwise;
  double diff = std::min(anticlockwise,clockwise);

  //std::cout << "\nanticlockwise: " << anticlockwise*180/M_PI << std::endl;
  //std::cout << "    clockwise: " << clockwise*180/M_PI << std::endl;
  //std::cout << "            BoatHeading: " << boatHeading*180/M_PI << std::endl;
  //std::cout << "Heading to this element: " << alpha*180/M_PI << std::endl;
  //std::cout << "                   diff: " << diff*180/M_PI << std::endl;

  return -diff/M_PI;

  //return 0;
}
