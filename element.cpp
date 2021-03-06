#include "element.hpp"
#include <iostream>
#include <algorithm>

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

  timesTargeted = 0;
}
Element::~Element()
{
  //std::cout << "Element Destructor" << std::endl;
}

void Element::updateDepth(double newDepth,bool changeState)
{
  //std::cout << "Element updating depth" << std::endl;
  if(changeState)
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

void Element::setAccumulatedDepth(double depth)
{
  accumulatedDepth = depth;
}

void Element::setTimesVisited(int v)
{
  timesVisited = v;
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

double Element::getAccumulatedDepth()
{
  return accumulatedDepth;
}

int Element::getIndexX()
{
  return indexX;
}
int Element::getIndexY()
{
  return indexY;
}

double Element::getX()
{
  return posX;
}

double Element::getY()
{
  return posY;
}

void Element::targeted()
{
  timesTargeted++;
}

int Element::getTimesTargeted()
{
  return timesTargeted;
}

void Element::addNeighBour(Element* n)
{
  if(std::find(neighbours.begin(), neighbours.end(), n) == neighbours.end())
    neighbours.push_back(n);
}

std::vector<Element*>* Element::getNeighbours()
{
  return &neighbours;
}
