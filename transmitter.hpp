//Header
#ifndef TRANSMITTER_H
#define TRANSMITTER_H

#include <vector>
#include <string>
#include <thread>
#include <termios.h>
//#include <boost/asio.hpp>BOOST
#include <iostream>
#include <queue>

class Transmitter
{
private:

    std::queue<std::string>* messageQueue;

    /* BOOST
    boost::asio::io_service *io = nullptr;
    boost::asio::serial_port *serial = nullptr;
    */

    /*NO BOOST*/
    int serialPort;
  
    bool listen; // = false;
    bool lock;// = false;

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
    void requestData();

    //Send a message on the serial port*/
    void sendMessage(std::string s);
       
    /** Get all new messages*/
    std::queue<std::string>* getMessages();
  
};

#endif

/*
 std::stringstream s;
    s << "$MSSTS," << speed << ",checksum";
    writeToSerial(s.str());
*/
