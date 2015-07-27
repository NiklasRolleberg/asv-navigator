#ifndef SINGLEBEAMSCANNER_H
#define SINGLEBEAMSCANNER_H

#include "polygon.hpp"
#include "data.hpp"


/** Scans the area contained by the polygon */
class SingleBeamScanner
{
private:
    Data* data;
    Polygon* polygon;
    double delta;
    double tol;
    int delay;
public:
    /**Constructor*/
  SingleBeamScanner(Data* data, Polygon* polygon,double delta,double tol);

    /**Destructor*/
    ~SingleBeamScanner();

    /**Start scan*/
    void startScan();

    /**stop scan*/
    void abortScan();
};



#endif // SWEEPINGSSCANNER_H
