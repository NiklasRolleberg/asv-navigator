#include <iostream>
#include "navigator.hpp"
#include <unistd.h>
#include "transmitter.hpp"
#include "mission.hpp"

int main()
{
  std::cout << "Main: program started" << std::endl;

  // (1) Create a transmitter and a mission
  Transmitter* transmitter = new Transmitter(2);
  transmitter->start();
  Mission* mission = new Mission(0);

  // (2) Create the navigator, and pass the transmitter to it
  Navigator* navigator = new Navigator(transmitter,1,2);

  // (3) give the navigator a mission
  navigator->setMission(mission);

  // (4) start navigator
  navigator->start();


  //navigator.abort();
  //transmitter->abort();

  delete navigator;
  delete mission;
  delete transmitter;
  std::cout << "Main: program done" << std::endl;
}
