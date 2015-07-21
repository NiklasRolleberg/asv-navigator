#include "element.hpp"
#include <iostream>

Element::Element(double px, double py, int ix, int iy)
{
  std::cout << "Element constructor" << std::endl;
}
Element::~Element()
{
  std::cout << "Element Destructor" << std::endl;
}
