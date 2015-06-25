//Header
#ifndef NAVIGATOR_H
#define NAVIGATOR_H

#include "data.hpp"

class Navigator
{
private:
    Data data;

public:
    Navigator(){};
    /**Constructor*/
    Navigator(int arg1, int arg2, int arg3);

    /**Destructor*/
    ~Navigator();

    /**Set polygon*/
    void setPolygon(int arg1);

    /**Stop scanning*/
    void abort();

};
#endif
