#include <iostream>
#include "navigator.hpp"
#include <unistd.h>
#include "transmitter.hpp"
#include "mission.hpp"
#include <vector>
#include <string>

int main(int argc, char* argv[])
{

  bool createFromFile = false;
  std::string create_filename = "nothing";
  bool backup = false;
  std::string backup_filename = "no_backup";
  for(int i=0;i<argc;i++)
  {
    if(strcmp("-B",argv[i]) == 0 || strcmp("-b",argv[i]) == 0)
    {
      std::cout << "make continuous backups" << std::endl;
      if(i+1 < argc)
      {
        backup_filename = std::string(argv[i+1]);
        backup = true;
      }
    }

    if(strcmp("-F",argv[i]) == 0 || strcmp("-f",argv[i]) == 0)
    {
      std::cout << "Contruct from saved data" << std::endl;
      if(i+1 < argc)
      {
        create_filename = std::string(argv[i+1]);
        createFromFile = true;
      }
    }
  }

  std::cout << "------------------------------------"<< std::endl;
  std::cout << "---------------START----------------"<< std::endl;
  std::cout << "------------------------------------"<< std::endl;

  std::cout << "Main: program started" << std::endl;

  // (1) Create a  mission and a transmitter
  Mission* mission;

  if(!createFromFile)
  {
    std::vector<std::string> missionPlan;
    for(std::string line; std::getline(std::cin,line);)
    {
      //std::cout << "READING: " << line << std::endl;
      if(line[0] != '#')
        missionPlan.push_back(line);
    }
    //std::cout << missionPlan.size() << std::endl;
    mission = new Mission(missionPlan);
  }
  else{
    mission = new Mission(create_filename);
  }

  //return 0;


  //Mission* mission = new Mission(0);

  std::cout << "\n---------starting mission-----------\n"<< std::endl;

  Transmitter* transmitter = new Transmitter(2);
  transmitter->start();

  // (2) Create the navigator, and pass the transmitter to
  double delta = 5;
  double tol = 2;//delta / 3.0;

  int delay_data = 50000;
  int delay_scanner = 500000;

  Navigator* navigator = new Navigator(transmitter,delay_data,delay_scanner,delta,tol, backup, backup_filename);

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
