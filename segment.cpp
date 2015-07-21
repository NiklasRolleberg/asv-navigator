#include "segment.hpp"
#include <iostream>

/**Constructor*/
PolygonSegment::PolygonSegment(std::vector<double> *x, std::vector<double> *y)
{
  std::cout << "PolygonSegment Constructor" << std::endl;


  for(int i=0;i<x->size() && i<y->size();i++)
  {
    xPoints.push_back(x->at(i));
    yPoints.push_back(y->at(i));
  }
  //std::cout << "PolygonSegment size: " << xPoints.size() << std::endl;
  xMax = maxX();
  xMin = minX();
  yMax = maxY();
  yMin = minY();
}

/**Destructor*/
PolygonSegment::~PolygonSegment()
{
  std::cout << "PolygonSegment Destructor" << std::endl;
}


bool PolygonSegment::contains(double x, double y)
{
  if(y>yMax || y<yMin || x>xMax || x<xMin)
    return false;
  double left = findX(y,false);
  double right = findX(y,true);
  if(x<right && x>left)
    return true;
  return false;
}

double PolygonSegment::findX(double y, bool right)
{
  return 0;
}

double PolygonSegment::maxX()
{
  return 0;
}

double PolygonSegment::minX()
{
  return 0;
}

double PolygonSegment::maxY()
{
  return 0;
}

double PolygonSegment::minY()
{
  return 0;
}
