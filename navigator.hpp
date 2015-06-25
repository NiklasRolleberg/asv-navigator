//Header
#ifndef NAVIGATOR_H
#define NAVIGATOR_H

#include "data.hpp"
#include "transmitter.hpp"

class Navigator
{
private:
    Data* data = nullptr;
    Transmitter* tr_ptr = nullptr;
public:
    Navigator(){};
    /**Constructor*/
    Navigator(Transmitter* transmitter, int arg2, int arg3);

    /**Destructor*/
    ~Navigator();

    /**Start scanning*/
    void start();

    /**Set polygon*/
    void setPolygon(int arg1);

    /**Stop scanning*/
    void abort();

};

#endif
