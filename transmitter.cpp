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
/*
#include <boost/asio.hpp>
#include <boost/lockfree/queue.hpp>
*/

Transmitter::Transmitter(int arg0)
{
    std::cout << "Transmitter constructor" << std::endl;
    std::cout << "Opening serial port..";

    // NO BOOST
    serialPort = open("/dev/ttyACM0", O_RDWR | O_NOCTTY | O_NDELAY);
    if(serialPort == -1)
      std::cout << "failed" << std::endl;
    else
      std::cout << "sucsess" << std::endl;

    fcntl(serialPort, F_SETFL ,0);

    struct termios options;
    tcgetattr(serialPort, &options);
    cfsetispeed(&options, B115200);
    cfsetospeed(&options, B115200);
    tcsetattr(serialPort, TCSANOW, &options);
    
    /* BOOST
    std::string port = "/dev/ttyACM0";//"/dev/ttyUSB0";
    
    unsigned int baud_rate = 115200;//9600;
    
    io = new boost::asio::io_service();
    serial = new boost::asio::serial_port((*io),port);

    if(!serial->is_open())
    {
      std::cout << "Failed" << std::endl;
    }
    std::cout << std::endl;
	
    
    
    // option settings...
    serial->set_option(boost::asio::serial_port_base::baud_rate(baud_rate));
    serial->set_option(boost::asio::serial_port_base::character_size(8));
    serial->set_option(boost::asio::serial_port_base::stop_bits(boost::asio::serial_port_base::stop_bits::one));
    serial->set_option(boost::asio::serial_port_base::parity(boost::asio::serial_port_base::parity::none));
    serial->set_option(boost::asio::serial_port_base::flow_control(boost::asio::serial_port_base::flow_control::none)); 
    */
}

Transmitter::~Transmitter()
{
    listen = false;
    //serial->cancel(); BOOST
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
    //delete serial; BOOST
    close(serialPort); //NO BOOST
    
    
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

    char c;
    std::string message;

    //NO BOOST
    char* d = new char;
    
    while(listen)
    {
      //BOOST
      //boost::asio::read((*serial),boost::asio::buffer(&c,1));

      // NO BOOST
      int rd = read(serialPort,d,1);
      if(rd == -1) {
	std::cout << "read failed" << std::endl;
	usleep(1000000);
      }
      c = (*d);


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
    std::cout << "Listen to serial port loop ended" << std::endl;
}

void Transmitter::abort()
{
    std::cout << "Transmitter abort" << std::endl;
    
    listen = false;
    //serial->cancel(); BOOST
    if(listenThread != nullptr)
        if(listenThread->joinable())
            listenThread->join();
}


void Transmitter::requestData()
{
  //std::cout << "Transmitter:: requestData" << std::endl;
  std::string s = "$MSGPO,*00";
  writeToSerial(s);
}

void Transmitter::sendMessage(std::string s)
{
  writeToSerial(s);
}
  

int Transmitter::getMessages()
{
  std::cout << "Transmitter getmessages" << std::endl;
  return 0;
}


void Transmitter::writeToSerial(std::string message)
{
  std::cout << "Write to serial port: " << message << std::endl;
  message+='\n';
  
  while(lock){
    std::cout << "locked" << std::endl;
    usleep(100);
  }
  
      
  lock = true;
  
  //NO BOOST
  int wr=write(serialPort,message.c_str(),message.size());
  tcdrain(serialPort);
  usleep(100);
  // BOOST
  //boost::asio::write((*serial),boost::asio::buffer(message.c_str(),message.size()));
  lock = false;
  //std::cout << "Write complete" << std::endl;
  //is the message coming back? turn off echo in stty..
}

