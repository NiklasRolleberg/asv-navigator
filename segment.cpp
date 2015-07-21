#include "segment.hpp"
#include <iostream>

/**Constructor*/
PolygonSegment::PolygonSegment(std::vector<double> *x, std::vector<double> *y)
{
  std::cout << "PolygonSegment Constructor" << std::endl;
}

/**Destructor*/
PolygonSegment::~PolygonSegment()
{
  delete yPoints;
  delete xPoints;
  std::cout << "PolygonSegment Destructor" << std::endl;
}
