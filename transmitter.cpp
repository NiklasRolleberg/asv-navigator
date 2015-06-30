#include "transmitter.hpp"
#include <iostream>
#include <unistd.h>
#include <string>
#include <sstream>
#include <thread>
#include <cstdlib>

Transmitter::Transmitter(int arg0)
{
    std::cout << "Transmitter constructor" << std::endl;
}

Transmitter::~Transmitter()
{
    listen = false;
    if(listenThread != nullptr)
        if(listenThread->joinable())
            listenThread->join();
    delete listenThread;

    //TODO close serial port

    std::cout << "Transmitter destructor" << std::endl;
}
void Transmitter::start()
{
    //TODO open serial port
    std::cout << "Transmitter start, start listening to serial port" << std::endl;
    listen = true;

    listenThread = new std::thread(&Transmitter::listenToSerialPort, this , 0);

}

void Transmitter::listenToSerialPort(int serialport)
{
    while(listen)
    {
        //Loopen funkar
        //std::cout << "Listening to serial port" << std::endl;
        usleep(50000);
    }
}

void Transmitter::abort()
{
    std::cout << "Transmitter abort" << std::endl;
    listen = false;
    if(listenThread != nullptr)
        if(listenThread->joinable())
            listenThread->join();


    //TODO close serial port

}

int Transmitter::getDepthData()
{
    //std::cout << "Transmitter getDepthData" << std::endl;
    return 0;
}

int Transmitter::getPositionData()
{
    std::cout << "Transmitter getPositionData" << std::endl;
    return 0;
}

void Transmitter::setWaypoint(double lat, double lon)
{
    //std::cout << "SetWaypoint" << std::endl;
    std::stringstream s;
    s << "$MSSCP," << lat << "," << lon << ",checksum";
    writeToSerial(s.str());
}

void Transmitter::setTargetSpeed(double speed)
{
    std::stringstream s;
    s << "$MSSTS," << speed << ",checksum";
    writeToSerial(s.str());
}


void Transmitter::writeToSerial(std::string message)
{
    srand((unsigned)time(0));
    while(true)
    {
        if(!lock)
        {
            lock = true;
            std::cout << "Transmitter: write to serial port: " << message << std::endl;
            //Write stuff to serial port
            lock = false;
            return;
        }
        usleep(3 + (rand()%15));
    }
}

