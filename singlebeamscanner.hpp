#ifndef SINGLEBEAMSCANNER_H
#define SINGLEBEAMSCANNER_H

#include "polygon.hpp"
#include "data.hpp"
#include "segment.hpp"
#include "element.hpp"

struct Closest {
  Closest(int ix, int iy,PolygonSegment* ptr): x(ix), y(iy),region(ptr){}
  int x,y;
  PolygonSegment* region;
};

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
    bool updateDepth(Polygon* p, double x, double y, double depth, bool followingLand);

    /**scan a polygonsegment with sweeping pattern*/
    bool scanRegion(PolygonSegment* region);

    /**find the closest element in the closest region*/
    Closest findClosest(int startX,int startY);

    /**Go to to a new region*/
    bool gotoRegion(Closest target);

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
