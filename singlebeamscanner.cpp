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


SingleBeamScanner::SingleBeamScanner(Data* dataptr, Polygon* polygonptr,int inputdelay, double d, double t)
{
  std::cout << "scanner:SingleBeamScanner constructor" << std::endl;
  data = dataptr;
  polygon = polygonptr;
  delay = inputdelay; //1000000 1s
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

  /*
  //DEBUG
  //polygon->updateView(1,1);
  //sleep(1);
  for (int i=0;i<polygon->nx;i++)
  {
    for (int j=0;j<polygon->ny;j++)
    {
      if(i==j || abs(i-j) == 1)
        polygon->matrix[i][j]->setStatus(2);
    }
  }
  polygon->matrix[5][12]->setStatus(1);
  //polygon->updateView(0,0);
  //sleep(1);
  polygon->idland();
  //polygon->updateView(0,0);
  //sleep(1);
  polygon->removeAllRegions();
  polygon->generateRegions();
  */

  std::cout << "scanner:SingleBeamScanner: starting scan" << std::endl;
  std::cout << "sweeping pattern, delta = " << delta << "\n" << std::endl;

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
    std::cout << "massa data" << std::endl;
    std::cout << "polygonSegments: " << polygon->polygonSegments.size() << std::endl;
    std::cout << "boat pos: " << data->getX() << ", " << data->getY() << std::endl;
    std::cout << "X-limits [" << polygon->minX << "," << polygon->maxX << "]" << std::endl;
    std::cout << "Y-limits [" << polygon->minY << "," << polygon->maxY << "]" << std::endl;
    usleep(3000000);
    return;
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
      polygon->removeAllRegions();
      polygon->generateRegions();

      //check if the boat is inside a region allready
      index = -1;
      for(int i=0;i<polygon->polygonSegments.size();i++)
      {
        if(polygon->polygonSegments.at(i)->contains(data->getX(),data->getY()))
          index = i;
      }

      if(index =! -1)
      {
        std::cout << "boat is allready inside a region" << std::endl;
        //scan the region
        scanRegion(polygon->polygonSegments.at(index));
        polygon->removeRegion(polygon->polygonSegments.at(index));
      }
      else
      {
        //find closest region
        c = findClosest(data->getX(),data->getY());
        if(c.x == -1 || c.y==-1 || c.region == nullptr)
        {
          std::cout << "No accessible regions left to scan\n" << std::endl;
          stop = true;
          break; //no path found -> nothing left to scan
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

        //identify land
        polygon->idland();
      }
    }
  }


  std::cout <<"SweepingPattern done" << std::endl;
  polygon->saveMatrix();
  usleep(3000000);
}

//TODO kanske fixa så att den börjar scanna ävan om den inte
bool SingleBeamScanner::scanRegion(PolygonSegment* region)
{
  std::cout << "scanRegion" << std::endl;
  bool goToRight = (region->findX(data->getY(),false) - data->getX())
                    > (0.5)*(region->findX(data->getY(),false) - region->findX(data->getY(),true)); //traveling from left side to right
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

  double original_targetSpeed = 1.6;
  double targetSpeed = 1.6;
  double lastTargetX = data->getX();
  double lastTargetY = data->getY();


  data->setBoatWaypoint_local(lastTargetX,lastTargetY,targetX,targetY,targetSpeed,false);

  //north or south
  double updown = 0.8;
  //if((targetY-region->yMin) > ((region->yMax-region->yMin)/2.0))
 //   updown = -0.8;


  //adapt updown if the region is located at the top or bottom of the polygon
  if(targetY+delta*updown > region->yMax || targetY+delta*updown < region->yMin)
  {
    //std::cout << "INTE BRA!" << std::endl;
    updown*=0.6;
    std::cout << "UPDOWN adapted: " << updown << std::endl;
  }


  double dx;
  double dy;
  double targetLine = targetY;
  double lastDepth = data->getDepth();

  //start sweeping
  while(!stop)
  {
    usleep(delay);

    double x = data->getX();
    double y = data->getY();
    double depth = data->getDepth();
    double depthChange = depth -lastDepth;

    double depth_right = data->getDepth_Right();
    double depth_left = data->getDepth_Left();


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
      }

      std::cout << "TARGET: " << targetX << " " << targetY <<std::endl;
      if(skipRest)
      {
        std::cout << "skiprest was true" << std::endl;
        data->setBoatWaypoint_local(lastTargetX,lastTargetY,targetX,targetY,1.6,true);
        skipRest = false;
        usleep(4000000);
        depth = data->getDepth();
        depth_right = data->getDepth_Right();
        depth_left = data->getDepth_Left();
      }
      else
      {
        data->setBoatWaypoint_local(lastTargetX,lastTargetY,targetX,targetY,1.6,false);
      }
      /*
      //DEBUG
      //update depth of elements
      if(true)
      {
        double DX = (lastTargetX-targetX);
        double DY = (lastTargetY-targetY);
        double D = sqrt(DX*DX+DY*DY);
        DX = DX/D;
        DY = DY/D;
        for(int i=0;i<D;i++)
        {
            updateDepth(targetX + DX*i,targetY + DY*i, 2, false);
        }
      }
      */

    }

    //display depth data for debugging
    std::cout << "Depth      : " << depth << std::endl;
    std::cout << "Depth_right: " << depth_right << std::endl;
    std::cout << "Depth_left : " << depth_left << std::endl;

    //TODO adapt speed proportional to depth and depth change
    if(depth < 6 || depth_right < 2 || depth_left < 2 )
    {
      targetSpeed = 1;
      data->setBoatSpeed(targetSpeed);
    }
    else if(depthChange < -2 && depth < 4)
    {
      targetSpeed = targetSpeed - depthChange * targetSpeed;
      targetSpeed = std::max(1.0,targetSpeed);
      data->setBoatSpeed(targetSpeed);
    }
    else if(targetSpeed != original_targetSpeed)
    {
      targetSpeed=original_targetSpeed;
      data->setBoatSpeed(targetSpeed);
    }

    //close to land
    double t = 5;
    //if(depth < t && lastDepth < t || depth_right < t || depth_left < t)
    if((0.5*(depth_right + depth_left)) < t)
    {
      std::cout << "Starting land following" << std::endl;

      std::cout << "R: " << data->getDepth_Right() << std::endl;
      std::cout << "L: " << data->getDepth_Left() << std::endl;
      std::cout << "U: " << data->getDepth() << std::endl;
      std::cout << "dasdasd: " << 0.5*(depth_right + depth_left) << std::endl;


      data->setBoatSpeed(0);
      data->setBoatWaypoint_local(0,0,data->getX(),targetLine+delta*updown,0,true);
      usleep(2500000);
      followLand(targetLine,targetLine+delta*updown,region);
      //skiprest = true;
      //followLand(targetLine,targetLine+delta*updown,region);
      if(followLand(targetLine,targetLine+delta*updown,region))
      {
        targetLine = targetLine+delta*updown;
        std::cout << "lower line reached" << std::endl;
        skipRest = true;
        goToNextLine = false;
        targetY = targetLine;
      }
      else
      {
        targetLine = targetLine;
        std::cout << "upper line reached" << std::endl;
        targetY = targetLine;
        data->setBoatWaypoint_local(lastTargetX,lastTargetY,targetX,targetY,targetSpeed,false);
      }
    }

    lastDepth = depth;
  }

  std::cout << std::endl;
  return true;
}


bool SingleBeamScanner::followLand(double line1, double line2, PolygonSegment* region)
{
  std::cout << "Follow land" << std::endl;

  double targetDepth = 5; // m
  double targetSpeed = 0.6; // m/s
  double KP = 0.5; //Proportional gain

  double maxAngle = 3.1415 / 8; //22.5
  double turnAngle = 0;

  usleep(delay);
  double mean = (line1 + line2) / 2;

  while(abs(mean - data->getY()) < abs(delta*0.55) && !stop)
  {

    //V1 controller style
    //stop the boat from going outside the polygon
    if(!region->contains(data->getX(), data->getY()))
    {
      if(data->getX() < ((region->maxX()-region->minX())/2))
        data->setBoatWaypoint_local(0,0,region->findX(data->getY(),false),data->getY(),targetSpeed,true);
      else
        data->setBoatWaypoint_local(0,0,region->findX(data->getY(),true),data->getY(),targetSpeed,true);
      std::cout << "follow land: out of bounds" << std::endl;
      return true;
    }

    double error = 0.5*(data->getDepth_Right() + data->getDepth_Left()) - targetDepth;
    std::cout << "landfollowing error:" << error << std::endl;
    turnAngle = KP * error;
    if(data->getDepth_Right() < data->getDepth_Left())
      turnAngle *= -1;

    /*
    // V2 Mathematical style (turnAngle = the angle to the desired depth)
    //point = [x,y,depth]
    double d = 1.0/sqrt(2);
    double PR[] = {data->getX() + d*data->getDepth_Right()*cos(data->getHeading()+(3.1415/4))
                  ,data->getY() + d*data->getDepth_Right()*sin(data->getHeading()+(3.1415/4))
                  ,d*data->getDepth_Right()};
    double PL[] = {data->getX() + d*data->getDepth_Left()*cos(data->getHeading()-(3.1415/4))
                  ,data->getY() + d*data->getDepth_Left()*sin(data->getHeading()-(3.1415/4))
                  ,d*data->getDepth_Left()};

    // L = PL + t(PR-PL)

    if(PL[2] == PR[2])
      turnAngle = 0;
    else
    {
      // get t from calculating the point on the line where depth = targetDepth
      double t = (targetDepth - PL[2]) / (PR[2] - PL[2]);

      //point on the line
      //std::cout << "P: " << PL[0] + t*(PR[0]-PL[0]) << ", "
      //          << PL[1] + t*(PR[1]-PL[1]) << ", "
      //          << PL[2] + t*(PR[2]-PL[2]) << std::endl;

      double px = PL[0] + t*(PR[0]-PL[0]) - data->getX();
      double py = PL[1] + t*(PR[1]-PL[1]) - data->getY();
      double l = sqrt(px*px + py*py);
      px = px/l;
      py = py/l;
      double a  = asin(py);
      if(px<0)
        a = 3.1415 - a;
      turnAngle = data->getHeading() - a;
    }
    */

    std::cout << "R: " << data->getDepth_Right() << std::endl;
    std::cout << "L: " << data->getDepth_Left() << std::endl;
    std::cout << "U: " << data->getDepth() << std::endl;


    //Limit turnAngle
    if(turnAngle > 0)
      turnAngle = std::min(maxAngle,turnAngle);
    else
      turnAngle = std::max(-maxAngle,turnAngle);

    std::cout << "turnAngle:" << (turnAngle * 180.0 / 3.1415) << std::endl;
    double target_x = data->getX() + cos(data->getHeading()+turnAngle) * 50;
    double target_y = data->getY() + sin(data->getHeading()+turnAngle) * 50;
    data->setBoatWaypoint_local(0,0,target_x,target_y,targetSpeed,true);

    //update depth
    if (!updateDepth(data->getX(),data->getY(),data->getDepth(),true))
    {
      //this area has been scanened several times before -> abort scan
      std::cout << "Boat stuck aborting scan" << std::endl;
      std::cout << std::endl;
      return false;
    }

    usleep(delay);
    polygon->updateView(data->getX(),data->getY());
  }

  //close to upper line
  if(abs(data->getY()-line1) < abs(delta/2))
    return false;
  //close to line below
  return true;
}


//TODO fixa så att båten kommer fram till målet inte bara nästan
bool SingleBeamScanner::gotoRegion(Closest target)
{
  int x = (int) round((target.x - polygon->minX) / polygon->delta);
  int y = (int) round((target.y - polygon->minY) / polygon->delta);
  if(x<0)
    x = 0;
  if(x>=polygon->nx)
    x = polygon->nx-1;
  if(y<0)
    y = 0;
  if(y>=polygon->ny)
    y = polygon->ny-1;


  std::cout << "gotoRegion: " << x << "," << y << std::endl;
  //Create cost matrix
  double** cost = polygon->createCostMatrix(x, y);

  double targetX = target.x;
  double targetY = target.y;

  bool findNext = true;

  double Xpos,Ypos;

  while(true)
  {
    Xpos = data->getX();
    Ypos = data->getY();

    std::cout << "gotoRegion: i loopen" << std::endl;
    x = (int) round((Xpos - polygon->minX) / polygon->delta);
    y = (int) round((Ypos - polygon->minY) / polygon->delta);

    if(x<0)
      x = 0;
    if(x>=polygon->nx)
      x = polygon->nx-1;
    if(y<0)
      y = 0;
    if(y>=polygon->ny)
      y = polygon->ny-1;

    std::cout << "boat index: (" << x << "," << y << ")" << std::endl;

    double dx = targetX - Xpos;
    double dy = targetY - Ypos;

    //target reached, pick a new target
    if(sqrt(dx*dx + dy*dy) < tol || findNext)
    {
      findNext = false;
      //distance += sqrt(dx*dx + dy*dy);

      double tx = (Xpos - target.x);
      double ty = (Ypos - target.y);
      if(sqrt(tx*tx+ty*ty) < tol)
      {
        std::cout << "Reached (" << data->getX() << "," << data->getY() << ")" << std::endl;
        std::cout << "Target (" << target.x << "," << target.y << ")" << std::endl;
        std::cout << "distance to goal: " << sqrt(tx*tx+ty*ty) << std::endl;
        std::cout << "tx: " << tx << std::endl;
        std::cout << "ty: " << ty << std::endl;
        break;
      }

      //------------------------
      //find next waypoint
      int xIndex[] = {x+1, x-1, x+0, x+0, x+1, x-1, x+1, x-1};
      int yIndex[] = {y+0, y+0, y+1, y-1, y+1, y-1, y-1, y+1};
      double min = std::numeric_limits<double>::max();
      for(int i=0;i<8;i++)
      {
        if(xIndex[i] < 0 || xIndex[i] >= polygon->nx)
          continue;
        if(yIndex[i] < 0 || yIndex[i] >= polygon->ny)
          continue;
        if(cost[xIndex[i]][yIndex[i]] == -1)
          continue;

        if(cost[xIndex[i]][yIndex[i]] < min)
        {
          min = cost[xIndex[i]][yIndex[i]];
          targetX = polygon->minX + delta*xIndex[i];
          targetY = polygon->minY + delta*yIndex[i];
          std::cout << "min: " << min << std::endl;
        }
      }
      std::cout << "new target: (" << targetX << "," << targetY << ")" << std::endl;
      //------------------------
      data->setBoatWaypoint_local(0,0,targetX,targetY,4,true); //TODO speed ska nog vara 1.6
      polygon->updateView(data->getX(),data->getY());
    }

    if(rand()<0.3)
    {
      if(!data->hasCorrectPath(0,0,data->yTOlat(targetY),data->xTOlon(targetX),2)){
        std::cout << "wrong path, sending path again" << std::endl;
        data->setBoatWaypoint_local(0,0,targetX,targetY,1.6,true);
      }
    }

    usleep(delay);
  }

  //delete cost matrix
  std::cout << "deleting cost matrix" << std::endl;
  //delete the matrix
  for (int i = polygon->nx-1; i >= 0; --i)
  {
    delete[] cost[i];
  }
  delete[] cost;
  std::cout << "cost matrix deleted" << std::endl;


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
    //std::cout << "Not implemented" << std::endl;
    polygon->matrix[ix][iy]->updateDepth(depth);

    int indexX[] = {ix , ix+1, ix+1 ,ix+1 ,ix ,ix-1 ,ix-1 ,ix-1};
    int indexY[] = {iy-1 , iy-1, iy ,iy+1 ,iy+1 ,iy+1 ,iy ,iy-1};
    for(int k = 0; k < 8;k++)
    {
      int i = indexX[k];
      int j = indexY[k];
      if(i >= 0 && i < polygon->nx && j >= 0 && j < polygon->ny)
      {
        if(polygon->matrix[i][j]->getStatus() != 1)
        {
            polygon->matrix[i][j]->setStatus(2);
        }
      }
    }
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
      return Closest(startX,startY,polygon->polygonSegments.at(i));
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


  Element* target = NULL;
  PolygonSegment* targetRegion = NULL;
  double min = std::numeric_limits<double>::max();
  //std::cout << "findClosest: regions.size() " << polygon->polygonSegments.size() << std::endl;

  for(int i=0;i<polygon->polygonSegments.size();i++)
  {
    for(int j = 0;j<polygon->polygonSegments.at(i)->getBoundaryElements()->size();j++)
    {
      Element* e = polygon->polygonSegments.at(i)->getBoundaryElements()->at(j);
      //std::cout << "i,j" << i << "," << j << std::endl;
      if(e->getStatus() != 1)
      {
        //e->setStatus(2);
        //cost[e->getIndexX()][e->getIndexY()] = -1; //kanske onödig
        continue;
      }
      //e->setStatus(2);
      double c1 = cost[e->getIndexX()][e->getIndexY()];
      if(c1 == -1)
      {
        //std::cout << "not accessible" << std::endl;
        continue;
      }

      //double targeted = polygon->matrix[e->getIndexX()][e->getIndexY()]->getTimesTargeted();
      double targeted = e->getTimesTargeted();
      double c2 = (c1 +1) * (targeted+1);
      if(c2<min && targeted < 2 )
      {
        std::cout << "targeted: " << targeted << std::endl;
        //std::cout << "cost: " << c2 << " index:" << e->getIndexX() << "," << e->getIndexY() << std::endl;
        min = c2;
        target = e;
        targetRegion = polygon->polygonSegments.at(i);
      }
    }
  }

  if(target != NULL)
  {
    std::cout << "target:(" << target->getIndexX() << "," << target->getIndexY() << ")" << " cost: " << cost[target->getIndexX()][target->getIndexY()] << std::endl;
  } else
  {
    std::cout << "target = NULL" << std::endl;
  }

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

  std::cout << "deleting cost matrix" << std::endl;
  //delete the matrix
  for (int i = polygon->nx-1; i >= 0; --i)
  {
    delete[] cost[i];
  }
  delete[] cost;
  std::cout << "cost matrix deleted" << std::endl;


  //TODO return the one with the lowest value

  if(target== NULL)
    return Closest(-1,-1,nullptr);

  polygon->matrix[target->getIndexX()][target->getIndexY()]->targeted();
  return Closest(target->getX(),target->getY(),targetRegion);
}


void SingleBeamScanner::abortScan()
{
  std::cout << "scanner:SingleBeamScanner: abort scan" << std::endl;
}
