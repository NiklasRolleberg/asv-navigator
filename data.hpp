//Header
#ifndef DATA_H
#define DATA_H
#define M_PI 3.14159265358979323846

#include <thread>   // std::thread
#include "transmitter.hpp"
#include "polygon.hpp"
#include <limits>
#include <string>

class Sonar_gradient {
private:
    int n;
    double* values;
    bool first;
    double gradient;
  public:
    Sonar_gradient(int nr);
    ~Sonar_gradient();
    void addValue(double depth);
    double getGradient();
};

class Data
{
private:
    double boat_latitude;
    double boat_longitude;
    double boat_heading_real;

    double boat_targetLat;
    double boat_targetLon;

    double boat_xpos;
    double boat_ypos;
    double boat_heading_local;
    double boat_speed;
    double boat_sonar_depth;
    double boat_sonar_front_right;
    double boat_sonar_front_left;

    Sonar_gradient *gradient_depth;
    Sonar_gradient *gradient_front_right;
    Sonar_gradient *gradient_front_left;

    bool data_stop; //= false; //false
    int data_delay; //= 500000; //0.5s
    Transmitter* data_transmitterptr;
    std::thread *data_threadptr;// = nullptr;
    Polygon* localPolygon;// = nullptr;

    //for coordinate system
    bool localEnabled;
    double minLat;
    double maxLat;
    double dLat;
    double minLon;
    double maxLon;
    double dLon;

    double dx;
    double dy;


    /**collects data, calculates new coordinates*/
    void threadLoop();

    /**Determine the  type of message andstore data from message */
    void processMessage(std::string m);

    //calculate nmea checksum
    int calculateChecksum(std::string s);

    //check if a message has a valid checksum
    bool isValid(std::string s);

public:

    Data(){};

    /**Constructor*/
    Data(Transmitter* transmitter, int delay, int arg3);

    /**Destructor*/
    ~Data();

    /**Start collecting data*/
    void start();

    /**Stop collecting data*/
    void stop();


    //Conversions between local and global coordinates
    double lonTOx(double lon);
    double xTOlon(double x);
    double latTOy(double lat);
    double yTOlat(double y);


    /**Get boat x-position*/
    double getLat();

    /**Get boat y-position*/
    double getLon();

    /**Get boat heading*/
    double getLatLongHeading();


    /**create a local coordinateSystem*/
    void setLocalCoordinateSystem(Polygon* polygon, double delta);

    /**create a local coordinateSystem*/
    void removeLocalCoordinateSystem();

    /**Get boat x-position*/
    double getX();

    /**Get boat y-position*/
    double getY();

    /**Get boat heading*/
    double getHeading();

    /**Get boat speed*/
    double getSpeed();

    /**Get Depth*/
    double getDepth();

    /**change/time*/
    double getDepth_Slope();

    /**Get depth to the right*/
    double getDepth_Right();

    /**change/time*/
    double getDepth_Right_Slope();

    /**Get depth to the left*/
    double getDepth_Left();

    /**change/time*/
    double getDepth_Left_Slope();

    /**Ask for path*/
    void askForPath();

    /**Ask for operational mode*/
    void askForMode();

    /** true if the distance between the boats path and the coordinates given is less than tol */
    bool hasCorrectPath(double lat0, double lon0, double lat1, double lon1, double tol);

    /**Set waypoint for the boat*/
    void setBoatWaypoint_real(double lat0, double lon0,double lat1, double lon1, double speed, bool noStartPos);

    /**Set waypoint for the boat*/
    void setBoatWaypoint_local(double x0, double y0, double x1, double y1, double speed, bool noStartPos);

    /**Set waypoint for the boat*/
    void setBoatSpeed(double speed);

    /**Calculate distance between two coordinates*/
    double calculateDistance(double lat0,double lon0, double lat1, double lon1);

    /**Write a message to the transmitter log*/
    void writeToLog(std::string s);

    /**Send a custom message through the transmitter*/
    void sendMessage(std::string s);
};




#endif // DATA_H
