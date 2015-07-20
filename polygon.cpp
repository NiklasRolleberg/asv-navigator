#include "polygon.hpp"
#include <iostream>
#include <vector>

Polygon::Polygon(int points, std::vector<double> *lat, std::vector<double> *lon)
{
    std::cout << "Polygon constructor" << std::endl;
    latitude = lat;
    longitude = lon;

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
}

void Polygon::setGridResolution(double delta)
{
  std::cout << "polygon setGridRes" << std::endl;
}

void Polygon::initialize()
{
  
}

std::vector<double>* Polygon::getXBoundaries()
{
  return xPoints;
}
std::vector<double>* Polygon::getYBoundaries()
{
  return yPoints;
}
