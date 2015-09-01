#include <iostream>
#include "navigator.hpp"
#include <unistd.h>
#include "transmitter.hpp"
#include "mission.hpp"
#include <vector>
#include <string>

int main()
{
  std::cout << "------------------------------------"<< std::endl;
  std::cout << "---------------START----------------"<< std::endl;
  std::cout << "------------------------------------"<< std::endl;

  std::cout << "Main: program started" << std::endl;

  // (1) Create a  mission and a transmitter

  std::vector<std::string> missionPlan;
  for(std::string line; std::getline(std::cin,line);)
  {
    //std::cout << "READING: " << line << std::endl;
    if(line[0] != '#')
      missionPlan.push_back(line);
  }
  //std::cout << missionPlan.size() << std::endl;

  Mission* mission = new Mission(missionPlan);

  //return 0;


  //Mission* mission = new Mission(0);

  std::cout << "\n---------starting mission-----------\n"<< std::endl;

  Transmitter* transmitter = new Transmitter(2);
  transmitter->start();

  // (2) Create the navigator, and pass the transmitter to
  double delta = 2;
  double tol = 4;//delta / 3.0;

  Navigator* navigator = new Navigator(transmitter,delta,tol);

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

  std::cout << "------------------------------------"<< std::endl;
  std::cout << "----------------DONE----------------"<< std::endl;
  std::cout << "------------------------------------"<< std::endl;
}
