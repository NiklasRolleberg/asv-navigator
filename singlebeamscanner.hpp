#ifndef SINGLEBEAMSCANNER_H
#define SINGLEBEAMSCANNER_H

#include "polygon.hpp"
#include "data.hpp"
#include "segment.hpp"
#include "element.hpp"


/** Scans the area contained by the polygon */
class SingleBeamScanner
{
private:
    Data* data;
    Polygon* polygon;
    double delta;
    double tol;
    int delay;

    /**Update values in the element matrix*/
    void updateDepth(Polygon* p, double x, double y, double depth, bool followingLand);

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
