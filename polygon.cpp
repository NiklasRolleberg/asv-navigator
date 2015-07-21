#include "polygon.hpp"
#include <iostream>
#include <vector>
#include <limits>
#include "element.hpp"

Polygon::Polygon(int points, std::vector<double> *lat, std::vector<double> *lon)
{
    std::cout << "Polygon constructor" << std::endl;
    latitude = lat;
    longitude = lon;
    localSet = false;
    /*
    for(int i=0;i<latitude->size();i++) {
        std::cout << "Latitude: " << latitude->at(i) << std::endl;
    }
    */
}

Polygon::~Polygon()
{
    delete xPoints;
    delete yPoints;

    delete latitude;
    delete longitude;

    std::cout << "Polygon destructor" << std::endl;
}

std::vector<double>* Polygon::getLonBoundaries()
{
    return longitude;
}

std::vector<double>* Polygon::getLatBoundaries()
{
    return latitude;
}

       //void setLocalBoundaries(std::vector<double>*x,std::vector<double>*y)
void Polygon::setLocalBoundaries(std::vector<double>*x,std::vector<double>*y)
{
  std::cout << "Polygon set local boundaries" << std::endl;
  xPoints = x;
  yPoints = y;
  localSet = true;
}

void Polygon::setGridSize(double d)
{
  std::cout << "polygon setGridSize" << std::endl;
  delta = d;
}

void Polygon::initialize()
{
  std::cout << "Polygon initialize" << std::endl;

  if(!localSet)
    return;

  std::cout << "calculating min,max for x and y" << std::endl;

  maxX = std::numeric_limits<double>::min();
  minX = std::numeric_limits<double>::max();
  maxY = std::numeric_limits<double>::min();
  minY = std::numeric_limits<double>::max();

  for(int i=0;i<xPoints->size();i++)
  {
    double x = xPoints->at(i);
    double y = yPoints->at(i);

    if(x<minX)
      minX = x;
    if(x>maxX)
      maxX = x;
    if(y<minY)
      minY = y;
    if(y>maxY)
      maxY = y;
  }

  //add a few meters to avoid segmentation fault
  minX-=10;
  minY-=10;
  maxX+=10;
  maxY+=10;

  std::cout << "maxX: " << maxX << ", minX: " << minX << "\nmaxY: " << maxY << ", minY: " << minY << std::endl;


  //calculate grid size
  nx = (maxX-minX)/delta;
  ny = (maxY-minY)/delta;
  nx+=1; //compensate for truncation error
  ny+=1;

  std::cout << "delta=" << delta << "-> grid size is: " << nx << "x" << ny << std::endl;

}

std::vector<double>* Polygon::getXBoundaries()
{
  return xPoints;
}
std::vector<double>* Polygon::getYBoundaries()
{
  return yPoints;
}
