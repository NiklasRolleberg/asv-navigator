//Header
#ifndef DATA_H
#define DATA_H

class Data
{
private:
    //void run();

public:

    Data(){};

    /**Constructor*/
    Data(int arg1, int arg2, int arg3);

    /**Destructor*/
    ~Data();

    /**Start collecting data*/
    void start();

    /**Stop collecting data*/
    void stop();

    /**Get boat x-position*/
    double getX();

    /**Get boat y-position*/
    double getY();

    /**Get boat heading*/
    double getHeading();

    /**Get Depth*/
    double getDepth();
};


#endif // DATA_H
