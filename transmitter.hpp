//Header
#ifndef TRANSMITTER_H
#define TRANSMITTER_H

#include <vector>
#include <string>
#include <thread>

class Transmitter
{
private:

    int serialPort;
    //char* buff, *buffer, *buffptr;
  
    bool lock = false;
    bool listen = false;
    std::thread* listenThread;
    void writeToSerial(std::string message);
    void listenToSerialPort();


public:
    Transmitter(){};

    /**Constructor*/
    Transmitter(int arg0);

    /**Destructor*/
    ~Transmitter();

    /**Start listening to serial port*/
    void start();

    /**Stop listening to serial port*/
    void abort();

    /**Get depth data since last time*/
    //std::vector<double> getDepthData();
    int getDepthData();

    /**Get position data since last time*/
    //std::vector<double[2]> getPositionData();
    int getPositionData();

    /**Set waypoint for the boat*/
    void setWaypoint(double lat, double lon);

    void setTargetSpeed(double speed);
};

#endif

