//Header
#ifndef TRANSMITTER_H
#define TRANSMITTER_H

#include <vector>
#include <string>
#include <thread>
#include <termios.h>
//#include <boost/asio.hpp>BOOST
#include <iostream>


class Transmitter
{
private:

    /* BOOST
    boost::asio::io_service *io = nullptr;
    boost::asio::serial_port *serial = nullptr;
    */

    /*NO BOOST*/
    int serialPort;


  
    bool listen = false;
    std::thread* listenThread;
    void writeToSerial(std::string message);
    void listenToSerialPort();
    bool lock = false;


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
    void requestData();

    //Send a message on the serial port*/
    void sendMessage(std::string s);
       
    /** Get all new messages*/
    int getMessages();
  
};

#endif

/*
 std::stringstream s;
    s << "$MSSTS," << speed << ",checksum";
    writeToSerial(s.str());
*/
