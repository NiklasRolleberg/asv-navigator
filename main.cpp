#include <iostream>
#include "navigator.hpp"
#include <unistd.h>
#include "transmitter.hpp"

int main()
{
  std::cout << "Main: program started" << std::endl;

  Navigator n = Navigator(new Transmitter(2),1,2);
  std::cout << "Main: navigator created" << std::endl;

  n.start();
  usleep(1000000);
  n.abort();
  std::cout << "Main: program done" << std::endl;
}
