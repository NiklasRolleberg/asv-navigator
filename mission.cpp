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

    //sleep
    taskQueue.push(new Task(10000000,false));
    
    
    std::vector<double> *lat = new std::vector<double>();
    std::vector<double> *lon = new std::vector<double>();

    lat->push_back(59.346779);
    lon->push_back(18.079781);

    lat->push_back(59.346512);
    lon->push_back(18.080666);

    lat->push_back(59.346266);
    lon->push_back(18.080523);

    //lat->push_back(59.3464928);
    //lon->push_back(18.0798550);

    /*
    //59.3493423,18.720005
    lat->push_back(59.3493423);
    lon->push_back(18.720005);
    */
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
  std::string message = "";
  
  for(int i=0;i<plan.size();i++)
  {
    std::cout << "Looking at line: " << plan[i] << std::endl;
    std::string line = plan[i];
    if(line[0] == '#' || line.length() < 6)
    {
      std::cout << "....no task or data found" << std::endl;
      continue;
    }
    
    if(line.substr(0,4) == "TASK")
    {
      //new task encountered? save the old one
      std::cout << "..its a task!" << std::endl;
      if(type != -1)
      {
	std::cout << "Saving last task" << std::endl;
	if(type == 1 && delay != -1)
	  taskQueue.push(new Task(delay,false));
	else if(type == 2 && lat != nullptr && lon != nullptr)
	{
	  if(lat->size() > 0 && lon->size() > 0)
	  {
	    double dlat = (*lat)[0];
    	    double dlon = (*lon)[0];
	    taskQueue.push(new Task(dlat,dlon));
	    lat = nullptr;
	    lon = nullptr;
	  }
	}
	else if(type == 3 && lat != nullptr && lon != nullptr)
	{
	  if(lat->size() > 0 && lon->size() > 0 && lat->size() == lon->size())
	  {
	    taskQueue.push(new Task(new Polygon(0,lat,lon)));
	    lat = nullptr;
	    lon = nullptr;
	  }
	}
	else if(type == 4)
	{
	  taskQueue.push(new Task(message));
	}  

	delete lat;
	delete lon;
	lat = nullptr;
	lon = nullptr;
	type = -1;
	message = "";

	
      }
      //Find the type of the new task
      char t = line[5];
      type = t - '0';
      std::cout << "new Task type: " << type << std::endl;
    }

    else if(line.substr(0,4) == "DATA")
    {
      std::cout << "it's a data Line, type: " << line.substr(5,4)  << std::endl;
      
      if(line.substr(5,4) == "TIM:")
      {
	std::cout << "time: " << line.substr(9,line.length()) << " " << line.length() - 9 << std::endl;
	
      }

    }
  }

  
  /*save the last task
  if(type /= -1)
  {
    if(type == 1 && delay != -1)
      taskQueue.push(new Task(delay,false));
    //TODO the rest
  }
  */
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

