#include "element.hpp"
#include <iostream>

Element::Element(double px, double py, int ix, int iy)
{
  //std::cout << "Element constructor (" << px << "," << py << ")" << std::endl;
  posX = px;
  posY = py;

  indexX = ix;
  indexY = iy;

  status = -1;

  accumulatedDepth = 0;
  timesVisited = 0;

}
Element::~Element()
{
  //std::cout << "Element Destructor" << std::endl;
}

void Element::updateDepth(double newDepth)
{
  //std::cout << "Element updating depth" << std::endl;
  status = 1;
  accumulatedDepth+=newDepth;
  timesVisited++;
}

int Element::getStatus()
{
  return status;
}

void Element::setStatus(int newStatus)
{
  status = newStatus;
}

int Element::getTimesVisited()
{
  return timesVisited;
}

double Element::getDepth()
{
  if(timesVisited != 0)
    return accumulatedDepth / timesVisited;
  return 0;
}

double Element::getX()
{
  return posX;
}

double Element::getY()
{
  return posY;
}

std::vector<Element*>* Element::getNeighbours()
{
  return &neighbours;
}
