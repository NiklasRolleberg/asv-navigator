
#include <iostream>
#include "mission.hpp"
#include "polygon.hpp"
#include <vector>
#include <fstream>
#include <cstring>

Mission::Mission(int arg0)
{
  std::cout << "Mission: constructor" << std::endl;
  //sleep
  taskQueue.push(new Task(1000000,false));
}

Mission::Mission(std::string filename)
{
  std::cout << "Mission: constructor creating from: " << filename << std::endl;
  //sleep
  taskQueue.push(new Task(1000000,false));

  std::vector<std::string> boatpos;
  std::vector<std::string> settings;
  std::vector<std::string> polygon;
  std::vector<std::string> matrix;


  int c = -1;

  std::string line;
  std::ifstream infile;
  infile.open (filename);
  while(!infile.eof())
  {
    getline(infile,line); // Saves the line in STRING.
    std::cout << line << std::endl; // Prints our STRING.

    if(line == "<boat>")
      c = 0;
    if(line.substr(0,8) == "<polygon")
      c = 1 ;
    if(line.substr(0,9) == "<settings")
      c = 2;
    if(line.substr(0,7) == "<matrix")
      c = 3;

    if(line == "</boat>")
      c = -1;
    if(line == "</polygon>")
      c = -1;
    if(line == "</settings>")
      c = -1;
    if(line == "</matrix>")
      c = -1;

    if(c == 0)
      boatpos.push_back(line);
    if(c == 1)
      polygon.push_back(line);
    if(c == 2)
      settings.push_back(line);
    if(c == 3)
      matrix.push_back(line);
    std::cout << "c: " << c << std::endl;
  }
	infile.close();

  //look for boat tag and make a goto-task to that position
  std::cout << "boatpos : " << boatpos.size() << std::endl;
  std::cout << "polygon : " << polygon.size() << std::endl;
  std::cout << "settings: " << settings.size() << std::endl;
  std::cout << "matrix  : " << matrix.size() << std::endl;

  if(boatpos.size()==3)
  {
    double lat = std::stod(boatpos.at(1));
    double lon = std::stod(boatpos.at(2));
    taskQueue.push(new Task(lat,lon));
  }

  //find settings tag and save delta
  double delta = -1;
  std::vector<double>* polygon_lat = new std::vector<double>();
  std::vector<double>* polygon_lon = new std::vector<double>();

  if(settings.size()==2)
    delta = std::stod(settings.at(1).substr(6));

  if(polygon.size()==3)
  {
    std::string s = "";
    line = polygon.at(1);
    for(int i=0; i<line.length();i++)
    {
      char c = line[i];
      if(i == line.length()-1 || c == ',')
      {
        if(c != ',')
          s += c;
        if(s.length() == 0)
          continue;
        double latitude = std::stod(s);
        polygon_lat->push_back(latitude);
        s = "";
        std::cout << "latitude saved: " << latitude << std::endl;
      }
      else
        s +=c;
    }

    s = "";
    line = polygon.at(2);
    for(int i=0; i<line.length();i++)
    {
      char c = line[i];
      if(i == line.length()-1 || c == ',')
      {
        if(c != ',')
          s += c;
        if(s.length() == 0)
          continue;
        double longitude = std::stod(s);
        polygon_lon->push_back(longitude);
        s = "";
        std::cout << "longitude saved: " << longitude << std::endl;
      }
      else
        s +=c;
    }
  }

  //matrix
  std::string status = "";
  std::string depth = "";
  std::string visited = "";
  int rows = -1;
  int cols = -1;
  if(matrix.size() == 10)
  {
    int r_index = matrix.at(0).find("rows=") + 5;
    int c_index = matrix.at(0).find("columns=") + 8;
    int end_index = matrix.at(0).find(">");

    std::cout << matrix.at(0) << std::endl;
    std::cout << "r_index: " << r_index << std::endl;
    std::cout << "c_index: " << c_index << std::endl;
    std::cout << "end_index: " << end_index << std::endl;

    rows = atoi(matrix.at(0).substr(r_index,c_index-r_index-8).c_str());
    cols = atoi(matrix.at(0).substr(c_index,end_index-c_index).c_str());
  }
  std::cout << "Rows: " << rows << std::endl;
  std::cout << "Cols: " << cols << std::endl;

  if(rows != -1 && cols != -1)
  {
    status = matrix.at(2);
    depth = matrix.at(5);
    visited = matrix.at(8);
  }
  else
  {
    std::cout << "Error! nx or ny =-1. somthing is wrong " << std::endl;
    exit(-1);
  }

  std::cout << "Status:\n" << status << std::endl;
  std::cout << "depth:\n" << depth << std::endl;
  std::cout << "Visited:\n" << visited << std::endl;
  std::cout << "lat: " << polygon_lat->size() << std::endl;
  std::cout << "lon: " << polygon_lon->size() << std::endl;

  Polygon* poly = new Polygon(polygon_lat,polygon_lon);

  int* arr_status = new int[rows*cols];
  double* arr_depth = new double[rows*cols];
  int* arr_visited = new int[rows*cols];

  //find the status values in the status string
  int str_index = 0;
  int arr_index = 0;
  std::string nr = "";
  while(str_index < status.size() && arr_index < rows*cols)
  {
    char c = status[str_index];
    if(c == ',')
    {
      if(nr != "")
      {
        std::cout << "status!: " << strtod(nr.c_str(),NULL) << std::endl;
        arr_status[arr_index] = strtod(nr.c_str(),NULL);
        arr_index++;
        nr = "";
      }
    }
    else
      nr += c;
    str_index++;
  }

  //find the depth values in the depth string
  str_index = 0;
  arr_index = 0;
  nr = "";
  while(str_index < depth.size() && arr_index < rows*cols)
  {
    char c = depth[str_index];
    if(c == ',')
    {
      if(nr != "")
      {
        std::cout << "Depth!: " << strtod(nr.c_str(),NULL) << std::endl;
        arr_depth[arr_index] = strtod(nr.c_str(),NULL);
        arr_index++;
        nr = "";
      }
    }
    else
      nr += c;
    str_index++;
  }

  //find the visited values in the visited string
  str_index = 0;
  arr_index = 0;
  nr = "";
  while(str_index < visited.size() && arr_index < rows*cols)
  {
    char c = visited[str_index];
    if(c == ',')
    {
      if(nr != "")
      {
        std::cout << "Visited!: " << strtod(nr.c_str(),NULL) << std::endl;
        arr_visited[arr_index] = strtod(nr.c_str(),NULL);
        arr_index++;
        nr = "";
      }
    }
    else
      nr += c;
    str_index++;
  }

  taskQueue.push(new Task(poly,rows,cols,arr_status, arr_depth, arr_visited));


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
		//std::cout << "Looking at line: " << plan[i] << std::endl;
		std::string line = plan[i];
		if(line[0] == '#' || line.length() < 6)
		{
			//std::cout << "....no task or data found" << std::endl;
			continue;
		}

		if(line.substr(0,4) == "TASK")
		{
			 //new task encountered? save the old one
			 //std::cout << "..its a task!" << std::endl;
			if(type != -1)
			{
				std::cout << "Saving previous task" << std::endl;
				if(type == 1 && delay != -1)
					taskQueue.push(new Task(delay,false));
				else if(type == 2 && lat != nullptr && lon != nullptr)
				{
					if(lat->size() > 0 && lon->size() > 0)
					{
						double dlat = (*lat)[0];
						double dlon = (*lon)[0];
						taskQueue.push(new Task(dlat,dlon));
						delete lat;
						delete lon;
						lat = nullptr;
						lon = nullptr;
					}
				}
				else if(type == 3 && lat != nullptr && lon != nullptr)
				{
					if(lat->size() > 0 && lon->size() > 0 && lat->size() == lon->size())
					{
						taskQueue.push(new Task(new Polygon(lat,lon)));
						lat = nullptr;
						lon = nullptr;
					}
				}
				else if(type == 4)
				{
					taskQueue.push(new Task(message));
				}

				type = -1;
				message = "";
			}

			//Find the type of the new task
			char t = line[5];
			type = t - '0';
			std::cout << "new Task type: " << type << std::endl;
		}

			//Line contains data, save the data
			else if(line.substr(0,4) == "DATA")
			{
				std::cout << "it's a data Line, type: " << line.substr(5,4)  << std::endl;
				//TIME
				if(line.substr(5,4) == "TIM:")
				{
					//std::cout << "time: " << line.substr(9,line.length()) << " " << line.length() - 9 << std::endl;
					delay = atoi(line.substr(9,line.length()).c_str()); //value = 45
					std::cout << "delay saved: " << delay << std::endl;
				}

			//LATITUDE
			else if(line.substr(5,4) == "LAT:")
			{
				std::string s = "";
				for(int i=9; i<line.length();i++)
				{
					char c = line[i];
					//std::cout << c << std::endl;
					if(i == line.length()-1 || c == ',')
					{
						if(c != ',')
							s += c;
						if(s.length() == 0)
							continue;
						double latitude = std::stod(s);
						if(lat == nullptr)
							lat = new std::vector<double>();
						lat->push_back(latitude);
						s = "";
						std::cout << "latitude saved: " << latitude << std::endl;
					}
					else
						s +=c;
				}
			}

			//LONGITUDE
			else if(line.substr(5,4) == "LON:")
			{
				std::string s = "";
				for(int i=9; i<line.length();i++)
				{
					char c = line[i];
					//std::cout << c << std::endl;
					if(i == line.length()-1 || c == ',')
					{
						if(c != ',')
							s += c;
						if(s.length() == 0)
							continue;
						double longitude = std::stod(s);
						if(lon == nullptr)
							lon = new std::vector<double>();
						lon->push_back(longitude);
						s = "";
						std::cout << "longitude saved: " << longitude << std::endl;
					}
					else
						s +=c;
				}
			}

			//MESSAGE
			else if(line.substr(5,4) == "MES:")
			{
				message = line.substr(9,line.length()-1);
				std::cout << "Message saved: " << message << std::endl;
			}
		}
	}


	//save the last task
	if(type != -1)
	{
		std::cout << "Saving final task, type: " << type << std::endl;
		if(type == 1 && delay != -1)
			taskQueue.push(new Task(delay,false));
		else if(type == 2 && lat != nullptr && lon != nullptr)
		{
			if(lat->size() > 0 && lon->size() > 0)
			{
				double dlat = (*lat)[0];
				double dlon = (*lon)[0];
				taskQueue.push(new Task(dlat,dlon));
			}
		}
		else if(type == 3 && lat != nullptr && lon != nullptr)
		{
			if(lat->size() > 0 && lon->size() > 0 && lat->size() == lon->size())
			{
				taskQueue.push(new Task(new Polygon(lat,lon)));
			}
		}
		else if(type == 4)
		{
			std::cout << "message task: message= " << message << std::endl;
			if(message != "")
				taskQueue.push(new Task(message));
		}
	}

	if(lat!= nullptr)
		delete lat;
	if(lon!=nullptr)
		delete lon;
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
    //std::cout << "Mission: getNextTask" << std::endl;
    if(!taskQueue.empty())
    {
        Task* task = taskQueue.front();
        taskQueue.pop();
	      return task;
    }
    //std::cout << "Mission: task is a nullptr" << std::endl;
    return nullptr;
}
