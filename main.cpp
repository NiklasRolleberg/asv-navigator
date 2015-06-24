#include <iostream>
#include "navigator.hpp"

int main()
{
  std::cout << "Main: program started" << std::endl;

  Navigator test = Navigator(0,1,2);
  
  std::cout << "Main: navigator created" << std::endl;

  test.test(2);
  
  std::cout << "Main: program done" << std::endl;
}
