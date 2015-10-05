#include "data.hpp"
#include <iostream>
//#include <thread>   // std::thread
#include <unistd.h>
#include "transmitter.hpp"
#include "polygon.hpp"
#include <cmath>
#include <sstream>
#include <string>
#include <queue>
#include <stdlib.h>
#include <vector>
#include <iomanip>

Data::Data(Transmitter* transmitter,int delay, int arg3)
{
    std::cout << "Data constructor" << std::endl;
    data_transmitterptr = transmitter;
    data_delay = delay;
    //Get initial values for variables

    //59.352884, 18.073585
    //boat_latitude = 59.352884;
    //boat_longitude = 18.073585;

    //59.300837,18.214686
    //59.296566, 18.226487
    boat_latitude = 59.296566;
    boat_longitude = 18.226487;

    boat_targetLat = 59.3534;
    boat_targetLon = 18.34343;

    boat_heading_real = 0;

    boat_xpos = 0;
    boat_ypos = 0;
    boat_heading_local = 0;
    boat_speed = 0;
    boat_depth = 0;
    localEnabled = false;
    data_threadptr = nullptr;
}

Data::~Data()
{
    std::cout << "Data destructor" << std::endl;
    data_stop = true;
    if(data_threadptr != nullptr)
        if(data_threadptr->joinable())
            data_threadptr->join();
    delete data_threadptr;
}

void Data::start()
{
    std::cout << "Data start" << std::endl;
    data_stop = false;
    if(data_threadptr != nullptr)
        std::cerr << "Memory leak incoming.." << std::endl;
        //data_threadptr->terminate();


    data_threadptr = new std::thread(&Data::threadLoop, this);
    //data_threadptr->join();
    //threadLoop();
}

void Data::stop()
{
    std::cout << "Data stop" << std::endl;
    data_stop = true;
    if(data_threadptr != nullptr)
        if(data_threadptr->joinable())
            data_threadptr->join();
}

double Data::getLat()
{
    //std::cout << "Data get latitude" << std::endl;
    return boat_latitude;
}

double Data::getLon()
{
    //std::cout << "Data get longitude" << std::endl;
    return boat_longitude;
}

double Data::getLatLongHeading()
{
    //std::cout << "Data getHeading (Lat,Long)" << std::endl;
    return boat_heading_real;
}


void Data::setLocalCoordinateSystem(Polygon* polygon, double delta)
{
    std::cout << "Polygon added, starting creation of local coordinate system, and initializes the polygon" << std::endl;

    //polygon.get..
    //calculate max min lat long
    minLat = std::numeric_limits<double>::max();
    maxLat = std::numeric_limits<double>::min();
    dLat = 1;

    minLon = std::numeric_limits<double>::max();
    maxLon = std::numeric_limits<double>::min();
    dLon = 1;


    std::vector<double> *latitude = polygon->getLatBoundaries();
    for(int i=0; i < latitude->size();i++)
    {
      double d = (*latitude)[i];
      if(d<minLat)
	       minLat = d;
      if(d>maxLat)
	       maxLat = d;
    }
    dLat = maxLat-minLat;

    std::vector<double> *longitude = polygon->getLonBoundaries();
    for(int i=0; i < longitude->size();i++)
    {
      double d = (*longitude)[i];
      if(d<minLon)
	       minLon = d;
      if(d>maxLon)
	       maxLon = d;
    }
    dLon = maxLon-minLon;

    dx = calculateDistance(minLat,minLon,minLat,maxLon);

    dy = calculateDistance(minLat,minLon,maxLat,minLon);

    std::cout << "minLat : " << minLat << std::endl;
    std::cout << "maxLat : " << maxLat << std::endl;
    std::cout << "dLat : " << dLat << std::endl;
    std::cout << "minLon : " << minLon << std::endl;
    std::cout << "maxLon : " << maxLon << std::endl;
    std::cout << "dLon : " << dLon << std::endl;
    std::cout << "dx: " << dx << std::endl;
    std::cout << "dy: " << dy << std::endl;


    localEnabled = true;

    boat_xpos = lonTOx(boat_longitude);
    boat_ypos = latTOy(boat_latitude);

    //calculate the polygon points in local coordinates
    std::vector<double>* localX = new std::vector<double>(); //TODO delete
    std::vector<double>* localY = new std::vector<double>();

    for(int i=0;i<latitude->size();i++)
    {
      localX->push_back(lonTOx((*longitude)[i]));
      localY->push_back(latTOy((*latitude)[i]));
    }


    std::cout << "Polygon in local coordinates" << std::endl;

    for(int i=0;i<localX->size();i++)
    {
      std::cout << "(" << (*localX)[i] << "," << (*localY)[i] << ")" << std::endl;
    }

    polygon->setLocalBoundaries(localX,localY);
    polygon->setGridSize(delta);
    polygon->initialize();


}

//TODO kanske lite inline

double Data::lonTOx(double lon)
{
  if(localEnabled)
    return ((lon-minLon)/dLon)*dx;
  return 0;
}

double Data::xTOlon(double x)
{
  if(localEnabled)
    return minLon + x*(dLon/dx);
  return 0;
}

double Data::latTOy(double lat)
{
  if(localEnabled)
    return ((lat-minLat)/dLat)*dy;
  return 0;
}

double Data::yTOlat(double y)
{
  if(localEnabled)
    return  minLat + y*(dLat/dy);
  return 0;
}

void Data::removeLocalCoordinateSystem()
{
    std::cout << "remove local coordinate system" << std::endl;
    localEnabled = false;
    minLat = 0;
    maxLat = 0;
    dLat = 0;
    minLon = 0;
    maxLon = 0;
    dLon = 0;
    dx = 0;
    dy = 0;
}

double Data::getX()
{
    //std::cout << "Data getX" << std::endl;
    return boat_xpos;
}

double Data::getY()
{
    //std::cout << "Data getY" << std::endl;
    return boat_ypos;
}

double Data::getHeading()
{
    //std::cout << "Data getHeading" << std::endl;
    return boat_heading_local;
}

double Data::getSpeed()
{
    //std::cout << "Data getSpeed" << std::endl;
    return boat_speed;
}

double Data::getDepth()
{
    //std::cout << "Data getDepth" << std::endl;
    return boat_depth;
}

bool Data::hasCorrectPath(double lat0, double lon0, double lat1, double lon1, double tol)
{
  //TODO check start pos
  if(calculateDistance(lat1,lon1,boat_targetLat,boat_targetLon) <  tol)
    return true;
  return false;
}

void Data::threadLoop()
{
  std::cout << "Data loop started" << std::endl;
  bool i = false;
  while(!data_stop)
  {
    data_transmitterptr->requestData();
    //if(i)
      //data_transmitterptr->sendMessage("$MSGCP,*00");
    i = (i==false);
    usleep(data_delay/2);
    std::queue<std::string>* messages = data_transmitterptr->getMessages();
    //std::cout << messages->size() << " messages rescieved" << std::endl;
    for(int i=0;i< messages->size();i++)
    {
      processMessage(messages->front());
      messages->pop();
    }
    usleep(data_delay/2);
  }
  std::cout << "Data loop done" << std::endl;
}


void Data::processMessage(std::string m)
{
  //std::cout << "message rescieved: " << m << std::endl;
  //std::cout << "processMessage" << std::endl;

  if(!isValid(m))
  {
    //std::cout << "message not valid:\t " << m << std::endl;
    return;
  }
  //std::cout << "message valid:\t " << m << std::endl;

  int startIndex = -1;
  for(int i=0;i<m.length();i++)
  {
    if(m[i] == '$')
    {
      startIndex = i;
      break;
    }
  }


  if(startIndex != -1 && (m.length() - startIndex) > 6)
  {
    if(m.substr(startIndex+1,5) == "MSGPS")
    {
      //find latitude
      //first ','
      int firstIndex = startIndex+6;
      int lastIndex;
      std::string latitude = "";
      for(int i=firstIndex+1; i<m.length() ;i++)
      {
        if(m[i] == ',')
        {
          lastIndex = i;
          break;
        }
        latitude += m[i];
      }

    	//find longitude
    	firstIndex = lastIndex;
    	std::string longitude = "";
    	for(int i=firstIndex+1; i<m.length() ;i++)
    	{
    	  if(m[i] == ',')
    	  {
    	    lastIndex = i;
    	    break;
    	  }
    	  longitude += m[i];
    	}

    	//std::cout << "LATITUDE: " << latitude  << "\n" << "LONGITUDE: " << longitude << std::endl;

    	boat_latitude = strtod(latitude.c_str(),NULL);
    	boat_longitude = strtod(longitude.c_str(),NULL);

    	if(localEnabled)
    	{
    	  boat_xpos = lonTOx(boat_longitude);
    	  boat_ypos = latTOy(boat_latitude);
    	  //std::cout << "Local coordinates: (" << boat_xpos << "," << boat_ypos << ")" <<std::endl;
    	}

    	//std::cout << "boat LATITUDE: " << boat_latitude  << "\n" << "boat LONGITUDE: " << boat_longitude << std::endl;
    }
    else if(m.substr(startIndex+1,5) == "MSCPA")
    {
      //$MSCPA,59.34836197,18.07197952,0.00,59.29697418,18.22008705,0.00,500.00,*50
      //TODO extract startlat, startlon,stoplat,stomlon

      int current = 0;;
      for(int i=0;i<5;i++)
      {
        while(current < m.length())
        {
          current++;
          if(m[current] == ',')
            break;
        }
        int next = current+1;
        while(next < m.length())
        {
          next++;
          if(m[next] == ',')
            break;
        }
        std::string nr = m.substr(current+1,next-current-1);
        current  = next-1;

        //std::cout << "i= " << i << " number= " << nr << std::endl;
        //i==0  startlat
        //i==1  startlon
        //i==2  startDepth
        //i==3  stoplat
        //i==4  stoplon

        if(i==3)
          boat_targetLat = strtod(nr.c_str(),NULL);
        if(i==4)
          boat_targetLon = strtod(nr.c_str(),NULL);
      }
      //std::cout << "targetLat, targetLon " << boat_targetLat << " " <<  boat_targetLon << std::endl;
    }
  }
  //std::cout << "Unknown message: " << m << std::endl;
}


void Data::setBoatWaypoint_real(double lat0, double lon0,double lat1, double lon1, double speed, bool noStartPos)
{

    //DEBUG
    boat_targetLat = lat1;
    boat_targetLon = lon1;
    boat_latitude = lat1;
    boat_longitude = lon1;
    boat_xpos = lonTOx(lon1);
    boat_ypos = latTOy(lat1);

    //std::cout << "Data: Set real waypoint, real coordinates: (" << lat0 << "," << lon0 << ") -> ("<< lat1 <<","<< lon1 << ")" << std::endl;
    std::stringstream s;

    /*
    //DEBUG
    s << "$MSSCP,0,0,0," << std::setprecision(10) << lat1 << "," << lon1 << ",0,"<< speed << ",*00";
    data_transmitterptr->sendMessage(s.str()); //send path
    */


    if(noStartPos)
      s << "MSSCP,,,," << std::setprecision(10) << lat1 << "," << lon1 << ",,"<< speed << ",";
    else
      s << "MSSCP," << std::setprecision(10) << lat0 << ","<< lon0 << ",0," << lat1 << "," << lon1 << ",0,"<< speed << ",";
    s << '*' << std::hex << calculateChecksum(s.str());
    std::string str = "$" + s.str();

    data_transmitterptr->sendMessage(str); //send path
    data_transmitterptr->sendMessage("$MSSTA,*74"); //start
    data_transmitterptr->sendMessage("$MSGCP,*66"); //ask for current path
}

void Data::setBoatWaypoint_local(double x0, double y0,double x1, double y1, double speed, bool noStartPos)
{
    std::cout << "Data: Set local waypoint, local coordinates:: (" << x0 << "," << y0 << ") -> (" << x1 << "," << y1 << ")" << std::endl;

    setBoatWaypoint_real(yTOlat(y0),xTOlon(x0),yTOlat(y1),xTOlon(x1),speed,noStartPos);

}

void Data::setBoatSpeed(double speed)
{
    std::cout << "Data: Set speed" << std::endl;
    /*
    std::stringstream s;
    s << "$MSSTS," << speed << ",checksum";
    data_transmitterptr->sendMessage(s.str());
    */
}

double Data::calculateDistance(double lat1,double lon1,double lat2,double lon2)
{
    double R = 6378127; // metres
    double phi1 = lat1 * M_PI / 180;// .toRadians();
    double phi2 = lat2 * M_PI / 180;// .toRadians();
    double dphi = (lat2-lat1) * M_PI / 180;// .toRadians();
    double dlambda = (lon2-lon1) * M_PI / 180;// .toRadians();

    double a = sin(dphi/2) * sin(dphi/2) +
            cos(phi1) * cos(phi2) *
            sin(dlambda/2) * sin(dlambda/2);
    double c = 2 * atan2(sqrt(a), sqrt(1-a));

    return R * c;
}

int Data::calculateChecksum(std::string s) {
  int c = 0;
    for(int i=0;i<s.length();i++)
      c ^= s[i];
    return c;
}

bool Data::isValid(std::string s)
{
  int i;
  for(i=0;i<s.length();i++)
    if(s[i]=='*')
      break;
  if(s.length()-i < 2)
    return false;

  //std::cout << "extracted checksum: " << s.substr(i+1,2) << std::endl;
  //std::cout << "message to calculate checksum on: " << s.substr(1,i-1) << std::endl;
  std::string cs = s.substr(i+1,2);
  if (cs=="00")
    return true;

  int cs1 = strtol( s.substr(i+1,2).c_str(), NULL, 16 );
  int cs2 = calculateChecksum(s.substr(1,i-1));

  //std::cout << "cs1: " << cs1 << " cs2: " << cs2 << std::endl;

  if(cs1 == cs2)
    return true;
  return false;
}


void Data::writeToLog(std::string s)
{
  data_transmitterptr->writeToLog(s);
}

void Data::sendMessage(std::string s)
{
    data_transmitterptr->sendMessage(s);
}
