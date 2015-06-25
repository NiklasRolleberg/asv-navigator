#include <iostream>
#include "navigator.hpp"

int main()
{
  std::cout << "Main: program started" << std::endl;

  Navigator n = Navigator(0,1,2);

  std::cout << "Main: navigator created" << std::endl;


  std::cout << "Main: program done" << std::endl;
}
