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
    serialPort = open("/dev/ttyUSB0", O_RDWR | O_NOCTTY | O_NDELAY);
    if(serialPort == -1)
      std::cout << "failed" << std::endl;
    else
      std::cout << "sucsess" << std::endl;

    fcntl(serialPort, F_SETFL ,0);
    
    //struct termios options = {0};
    //tcgetattr(serialPort,&options);
    /*
      stty -F /dev/ttyUSB0 intr ^C 
      stty -F /dev/ttyUSB0 quit ^\ 
      stty -F /dev/ttyUSB0 erase ^? 
      stty -F /dev/ttyUSB0 kill ^H 
      stty -F /dev/ttyUSB0 eof ^D 
      stty -F /dev/ttyUSB0 start ^Q 
      stty -F /dev/ttyUSB0 stop ^S susp ^Z 
      stty -F /dev/ttyUSB0 rprnt ^R 
      stty -F /dev/ttyUSB0 werase ^W 
      stty -F /dev/ttyUSB0 lnext ^V 
      stty -F /dev/ttyUSB0 flush ^O
    */
    //parenb parodd cs8 hupcl cstopb cread clocal -crtscts -ignbrk -brkint ignpar -parmrk -inpck -istrip -inlcr -igncr -icrnl -ixon ixoff -iuclc -ixany -imaxbel -iutf8 -opost -olcuc -ocrnl -onlcr -onocr -onlret -ofill -ofdel nl0 cr0 tab0 bs0 vt0 ff0 isig icanon -iexten -echo -echoe -echok -echonl -noflsh -xcase -tostop -echoprt -echoctl -echoke

    //dessa verkar funka
    /*
    speed 9600 baud; rows 0; columns 0; line = 0;
intr = ^C; quit = ^\; erase = ^?; kill = ^U; eof = ^D; eol = <undef>;
eol2 = <undef>; swtch = <undef>; start = ^Q; stop = ^S; susp = ^Z; rprnt = ^R;
werase = ^W; lnext = ^V; flush = ^O; min = 0; time = 0;
-parenb -parodd cmspar cs8 hupcl -cstopb cread clocal -crtscts
-ignbrk -brkint -ignpar -parmrk -inpck -istrip -inlcr -igncr -icrnl -ixon -ixoff
-iuclc -ixany -imaxbel -iutf8
-opost -olcuc -ocrnl onlcr -onocr -onlret -ofill -ofdel nl0 cr0 tab0 bs0 vt0 ff0
-isig -icanon iexten -echo echoe echok -echonl -noflsh -xcase -tostop -echoprt
echoctl echoke
    */

    
    
    /* 
    CRTSCTS : output hardware flow control (if possible)
    CS8     : 8n1 (8bit,no parity,1 stopbit)
    CLOCAL  : local connection, no modem contol
    CREAD   : enable receiving characters
    */
    //options.c_cflag = ~CRTSCTS | CS8 | CLOCAL | CREAD | ~CMSPAR;
        
    /*
    IGNPAR  : ignore bytes with parity errors
    IXON    : enable flow control
    */
    //options.c_iflag = IGNPAR | IXOFF;
       
    /*
    Raw output.
    */
    //options.c_oflag = 0;
         
    /*
    ICANON  : enable canonical input
    */
    //options.c_lflag = ISIG | ICANON | FLUSHO;
         
    /* 
    control characters 
    */
    /*
    options.c_cc[VINTR]    =       3; //CTRL-C; 
    //options.c_cc[VQUIT]    =       CTRL-\;
    options.c_cc[VERASE]   =       127; // DEL;
    options.c_cc[VKILL]    =       64; //@; 
    options.c_cc[VEOF]     =       4; //Ctrl-d;
    options.c_cc[VSWTC]    =       '\0'; 
    options.c_cc[VSTART]   =       17; //Ctrl-q; 
    options.c_cc[VSTOP]    =       19; //Ctrl-s;
    options.c_cc[VSUSP]    =       26; //Ctrl-z;
    options.c_cc[VEOL]     =       '\0';
    options.c_cc[VREPRINT] =       18; //Ctrl-r;
    options.c_cc[VDISCARD] =       21; //Ctrl-u;
    options.c_cc[VWERASE]  =       23; //Ctrl-w;
    options.c_cc[VLNEXT]   =       22; //Ctrl-v;
    options.c_cc[VEOL2]    =       '\0';
   
    
    
    options.c_cc[VTIME] = 0.01;  //  1s=10   0.1s=1 *
    options.c_cc[VMIN] = 0;
    cfsetispeed(&options, B9600);
    cfsetospeed(&options, B9600);
    //cfmakeraw(&options);
    
    tcflush(serialPort, TCIFLUSH);
    tcsetattr(serialPort, TCSANOW, &options);
    */
    
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
    char* d = new char[128];

    
    //int i=0;
    while(listen)
    {
      usleep(1000);
      /*
      if(rand()%100 < 50)
      {
	std::stringstream s;
	s << "$MSSTS," << i << ",*00";
	i++;
	writeToSerial(s.str());
	continue;
      }
      */

      //if(lock)
      //continue;
      
      //BOOST
      //boost::asio::read((*serial),boost::asio::buffer(&c,1));

      // NO BOOST
      int rd = read(serialPort,d,128);
      if(rd == -1) {
	std::cout << "read failed" << std::endl;
	usleep(1000000);
      }

      if(rd == 0)
      {
	//Nothing to read
	continue;
      }
	  
      
      for(int i=0;i<rd;i++)
      {
	c = d[i];
	switch(c)
	{
	case '\r':
	    break;
	case '\n':
	    std::cout << "Message rescieved: " << message  << "  rd = " << rd << std::endl;
	    message = "";
	    break;
	default:
	    message+=c;
	}
      }
    }
    delete[] d;
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
  //printf("Write to serial port\n");
  message+='\n';
  
  while(lock){
    //std::cout << "-serial port locked-" << std::endl;;
    usleep(1000);
  }
  
      
  lock = true;
  
  //NO BOOST
  int wr=write(serialPort,message.c_str(),message.size());

  if(wr < 0)
  {
      std::cout << "Writing error" << std::endl;
  }
  
  std::cout << "tcDrain" << std::endl;
  //printf("tcDrain\n");
  int tc = tcdrain(serialPort);
  std::cout << "Write complete: " << std::endl;//tc <<  std::endl;
  //printf("Write complete\n");
  //usleep(100);
  // BOOST
  //boost::asio::write((*serial),boost::asio::buffer(message.c_str(),message.size()));
  lock = false;
  
  //is the message coming back? turn off echo in stty..
  // turn off conversion of characters with -icrnl
}

