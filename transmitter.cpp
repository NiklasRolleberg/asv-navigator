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

Transmitter::Transmitter(int arg0)
{
    std::cout << "Transmitter constructor" << std::endl;
    std::cout << "Opening serial port..";
    serialPort=open("/dev/ttyUSB0", O_RDWR | O_NOCTTY | O_NDELAY);
    if(serialPort == -1)
      std::cout << "failed" << std::endl;
    else
      std::cout << "sucsess" << std::endl;
    fcntl(serialPort, F_SETFL,0);
}

Transmitter::~Transmitter()
{
    listen = false;
    if(listenThread != nullptr)
    {
        if(listenThread->joinable())
	{
      	    std::cout << "Waiting for serialport reader to time out"<<std::endl;
            listenThread->join();
	}
    }
    delete listenThread;

    //delete[] buff;
    //delete[] buffer;
    //delete[] buffptr;
    


    std::cout << "Closong serial port " << std::endl;
    close(serialPort);
      
    std::cout << "Transmitter destructor" << std::endl;
}
void Transmitter::start()
{
    std::cout << "Transmitter start, start listening to serial port" << std::endl;
    listen = false; //true

    listenThread = new std::thread(&Transmitter::listenToSerialPort, this);
    usleep(100000); // let the thread start
}

void Transmitter::listenToSerialPort()
{
    std::cout << "Listen to serial port loop started" << std::endl;
    char* buff = new char[8];
    std::string message = "";
    while(listen)
    {
        usleep(1000);
	if(lock)
	  continue;
        //Loopen funkar
        //std::cout << "Listening to serial port" << std::endl;
        int rd = read(serialPort,buff,1);

	if(rd == -1)
	{
	  char*buffer;
	  char * errorMessage = strerror_r( errno, buffer, 256 ); // get string message
	  printf("ERROR: ");
	  printf(errorMessage);
	  printf("\n");
	  delete[] buffer;
	  continue; 
	}

        //one char in buffer
	char ch = buff[0];
	message += ch;
	if(ch == '\n')
        {
	  std::cout << "Message: " << message  << std::endl;
	  message = "";
	}
	
	/*
	//print message
	printf("incoming message:");
	for(int i=0;i<rd;i++)
	{
	  printf(&buff[i]);
	}
	printf("\n");
	*/

	
	
    }
    delete[] buff;
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
    //writeToSerial("$I want data,checksum\n");
    return 0;
}

int Transmitter::getPositionData()
{
    std::cout << "Transmitter getPositionData" << std::endl;
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
    srand((unsigned)time(0));
    while(true)
    {
        if(!lock)
        {
            lock = true;
            std::cout << "Transmitter: write to serial port: " << message << std::endl;
            //Write stuff to serial port
	    message += '\n';
	    std::cout << "Transmitter: bytes to send: " << message.size() << std::endl;
	    int wr=write(serialPort,message.c_str(),message.size());
	    //usleep(10000);
            lock = false;
            return;
        }
        usleep(3 + (rand()%15));
    }
}

