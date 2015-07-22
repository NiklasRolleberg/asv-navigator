#include "segment.hpp"
#include <iostream>
#include <limits>

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

  //Kanske funkar..
  //std::cout << "min/max calculated:\n" << "maxX: " << xMax << ", minX: " << xMin<< "\nmaxY: " << yMax << ", minY: " << yMin << std::endl;
  //std::cout << "testing findX (y=mean), left: " << findX((yMax-yMin)/2,false) << " right: " << findX((yMax-yMin)/2,true) << std::endl;

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

  int l1[] = {-1,-1};
  int l2[] = {-1,-1};

  int l11 = -1;
  int l12 = -1;

  int l21 = -1;
  int l22 = -1;

  int l = yPoints.size();
  for(int i=0; i < l; i++)
  {
    if(((yPoints.at((i+1)%l) >= y) && (yPoints.at(i) <= y))
    || ((yPoints.at((i+1)%l) <= y) && (yPoints.at(i) >= y)))
    {
      //interpolate
      if(l11 == -1)
      {
        l11 = (i+1)%l;
        l12 = i%l;
      }
      else
      {
        l21 = (i+1)%l;
        l22 = i%l;
      }
    }
  }

  /*error return some default value*/
  if(l11 == -1 || l12 == -1 || l21 == -1 || l22 == -1)
  {
    double meanX = 0;
    for(int i=0;i<xPoints.size();i++)
    {
      meanX += xPoints.at(i);
    }
    meanX /= xPoints.size();
    return meanX;
  }

  //interpolate x
  double x0 = xPoints.at(l11);
  double y0 = yPoints.at(l11);
  double x1 = xPoints.at(l12);
  double y1 = yPoints.at(l12);

  //how far is y on the line
  double p = (y-y0) / (y1-y0);
  double pX1 = (1-p)*x0 + p*x1;

  //System.out.println("p1=" + p);
  x0 = xPoints.at(l21);
  y0 = yPoints.at(l21);
  x1 = xPoints.at(l22);
  y1 = yPoints.at(l22);

  //how far is y on the line
  p = (y-y0) / (y1-y0);
  double pX2 = (1-p)*x0 + p*x1;
  //System.out.println("p2=" + p);

  double max;
  double min;

  if(pX1>pX2)
  {
    max = pX1;
    min = pX2;
  }
  else
  {
    max = pX2;
    min = pX1;
  }

  if(right)
  {
    return max;
  }
  return min;
}

double PolygonSegment::maxX()
{
  double temp = std::numeric_limits<double>::min();
  for (double value : xPoints)
  {
    if (value > temp)
      temp = value;
  }
  return temp;
}

double PolygonSegment::minX()
{
  double temp = std::numeric_limits<double>::max();
  for (double value : xPoints)
  {
    if (value < temp)
      temp = value;
  }
  return temp;
}

double PolygonSegment::maxY()
{
  double temp = std::numeric_limits<double>::min();
  for (double value : yPoints)
  {
    if (value > temp)
      temp = value;
  }
  return temp;
}

double PolygonSegment::minY()
{
  double temp = std::numeric_limits<double>::min();
  for (double value : yPoints)
  {
    if (value < temp)
      temp = value;
  }
  return temp;
}
