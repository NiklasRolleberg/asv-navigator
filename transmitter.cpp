#include "transmitter.hpp"
#include <iostream>
#include <unistd.h>
//#include <string>
#include <sstream>
#include <thread>
#include <cstdlib>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <errno.h>
#include <termios.h>
#include <boost/asio.hpp>

Transmitter::Transmitter(int arg0)
{
    std::cout << "Transmitter constructor" << std::endl;
    std::cout << "Opening serial port..";

    std::string port = "/dev/ttyUSB0";
    unsigned int baud_rate = 9600;


    //funkar
    /*
    bost::asio::io_service io();
    boost::asio::serial_port serial(io,port);
    */
    
    io = new boost::asio::io_service();
    serial = new boost::asio::serial_port((*io),port);    
}

Transmitter::~Transmitter()
{
    listen = false;
    serial->cancel();
    if(listenThread != nullptr)
    {
        if(listenThread->joinable())
	{
      	    std::cout << "Waiting for serialport reader to time out"<<std::endl;
            listenThread->join();
	}
    }
    delete listenThread;

    
    //std::cout << "Closong serial port " << std::endl;
    // TODO close serial port
    
    std::cout << "Transmitter destructor" << std::endl;
}
void Transmitter::start()
{
    std::cout << "Transmitter start, start listening to serial port" << std::endl;
    listen = true; //true

    listenThread = new std::thread(&Transmitter::listenToSerialPort, this);
    usleep(100000); // let the thread start
}

void Transmitter::listenToSerialPort()
{
    std::cout << "Listen to serial port loop started" << std::endl;
    while(listen)
    {
      char c;
      std::string message;
      for(;;)
      {
	  boost::asio::read((*serial),boost::asio::buffer(&c,1));
          switch(c)
          {
              case '\r':
                  break;
              case '\n':
		  std::cout << "Message rescieved: " << message  << std::endl;
		  message = "";
		  break;
              default:
                  message+=c;
	  }
      }
    }
    std::cout << "Listen to serial port loop ended" << std::endl;
}

void Transmitter::abort()
{
    std::cout << "Transmitter abort" << std::endl;
    
    listen = false;
    if(listenThread != nullptr)
        if(listenThread->joinable())
            listenThread->join();
}

int Transmitter::getDepthData()
{
    //std::cout << "Transmitter getDepthData" << std::endl;
    writeToSerial("$I want data,checksum");
    return 0;
}

int Transmitter::getPositionData()
{
    //std::cout << "Transmitter getPositionData" << std::endl;
    //writeToSerial("$I want position data,checksum");
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
  std::cout << "Write to serial port: " << message << std::endl;
  message+='\n';
  //boost::asio::async_write((*serial),boost::asio::buffer(message.c_str(),message.size()));
  boost::asio::write((*serial),boost::asio::buffer(message.c_str(),message.size()));

}

