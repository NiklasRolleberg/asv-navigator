
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

    //TODO add goto-point task to a point inside the polygon

    //sleep
    taskQueue.push(new Task(1000000,false));

    //go to a point inside the polygon
    //taskQueue.push(new Task(59.300837,18.214686));

    //sleep
    //taskQueue.push(new Task(1000000,false));

    std::vector<double> *lat = new std::vector<double>();
    std::vector<double> *lon = new std::vector<double>();

    /*
    //    59.326,18.071
    lat->push_back(59.326);
    lon->push_back(18.071);

    //    59.329,18.075
    lat->push_back(59.329);
    lon->push_back(18.075);

    //    59.325,18.082
    lat->push_back(59.325);
    lon->push_back(18.082);
    */
    /*
    //59.347717, 18.074659
    lat->push_back(59.347717);
    lon->push_back(18.074659);
    */

    /*
    lat->push_back(59.346779);
    lon->push_back(18.079781);

    lat->push_back(59.346512);
    lon->push_back(18.080666);

    lat->push_back(59.346266);
    lon->push_back(18.080523);
    */
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

    /*
    //test vid GIH
    //59.346605, 18.079773
    lat->push_back(59.346605);
    lon->push_back(18.079773);

    //59.346758, 18.079872
    lat->push_back(59.346758);
    lon->push_back(18.079872);

    //59.346524, 18.080650
    lat->push_back(59.346524);
    lon->push_back(18.080650);

    //59.346326, 18.080384
    lat->push_back(59.346323);
    lon->push_back(18.080384);
    */


    //test på sjön

    lat->push_back(59.301091);
    lon->push_back(18.213823);

    lat->push_back(59.301125);
    lon->push_back(18.215594);

    lat->push_back(59.300555);
    lon->push_back(18.215850);

    lat->push_back(59.300515);
    lon->push_back(18.213855);


    taskQueue.push(new Task(new Polygon(lat,lon)));
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
    std::cout << "Mission: getNextTask" << std::endl;
    if(!taskQueue.empty())
    {
        Task* task = taskQueue.front();
        taskQueue.pop();
		return task;
    }
    //std::cout << "Mission: task is a nullptr" << std::endl;
    return nullptr;
}
