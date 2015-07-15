#include <iostream>
#include "mission.hpp"
#include "polygon.hpp"
#include <vector>

Mission::Mission(int arg0)
{
    std::cout << "Mission: constructor" << std::endl;
    //task = new Task(4000000, false);

    //Create tasks and add to queue
    //taskQueue.push(new Task(4000000,false));
    //taskQueue.push(new Task(13.23,23.23123));
    //taskQueue.push(new Task(3000000,false));
    //taskQueue.push(new Task(new Polygon(5)));
    //taskQueue.push(new Task(2000000,false));
    //taskQueue.push(new Task(1000000,false));
    /*
    taskQueue.push(new Task(5000000,false));
    taskQueue.push(new Task("$MSGPO,*00"));
    taskQueue.push(new Task("TestMessage02"));
    taskQueue.push(new Task(5000000,false));
    */

    //taskQueue.push(new Task("$MSSDL,3,*00"));
    std::vector<double> *lat = new std::vector<double>();
    std::vector<double> *lon = new std::vector<double>();

    lat->push_back(59.349204);
    lon->push_back(18.070359);

    /*
//59.353086, 18.073589
    lat->push_back(59.353086);
    lon->push_back(18.073589);


//59.353077, 18.073817
    lat->push_back(59.353077);
    lon->push_back(18.073817);


//59.353061, 18.074006
    lat->push_back(59.353061);
    lon->push_back(18.074006);

//59.352913, 18.074006
    lat->push_back(59.352913);
    lon->push_back(18.074006);

//59.352708, 18.074095
    lat->push_back(59.352708);
    lon->push_back(18.074095);


//59.352668, 18.073518
    lat->push_back(59.352668);
    lon->push_back(18.073518);
    */
    /*
    lat->push_back(1.23);
    lat->push_back(2.31);
    lat->push_back(3.21);

    lon->push_back(4.56);
    lon->push_back(5.64);
    lon->push_back(6.54);
    */
    taskQueue.push(new Task(new Polygon(3,lat,lon)));
}


Mission::Mission(std::vector<std::string> plan)
{
  std::cout << "Mission: constructor" << std::endl;

  int type = -1;
  int delay = -1;
  std::vector<double> *lat = nullptr; //new std::vector<double>();
  std::vector<double> *lon = nullptr; //new std::vector<double>();
  std::string message = "no message";
  
  for(int i=0;i<plan.size();i++)
  {
    std::cout << "Looking at line: " << plan[i] <<" ";
    std::string line = plan[i];
    if(line[0] == '#' || line.length() < 6)
    {
      std::cout << "....no task or data found" << std::endl;
      continue;
    }
    
    if(line.substr(0,3) == "TASK") {
      //new task encountered? create the old one
      std::cout << "..its a task!" << std::endl;
      if(type != -1)
      {
	if(type == 1 && delay != -1)
	  taskQueue.push(new Task(delay,false));
	// TODO the rest
	//if(type == 2 && lat != nullptr && lon != nullptr)
	  
      }
      
    }
  }

  if(type /= -1)
  {
    if(type == 1 && delay != -1)
      taskQueue.push(new Task(delay,false));
    //TODO the rest
  }
  
}


Mission::~Mission()
{
    while(!taskQueue.empty())
    {
        Task* task = taskQueue.front();
        taskQueue.pop();
        delete task;
    }

    std::cout << "Mission: destructor" << std::endl;
}

Task* Mission::getNextTask()
{
    std::cout << "Mission: getNextTask" << std::endl;
    if(!taskQueue.empty())
    {
        Task* task = taskQueue.front();
        taskQueue.pop();
        return task;
    }
    return nullptr;
}

