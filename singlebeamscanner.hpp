#ifndef SINGLEBEAMSCANNER_H
#define SINGLEBEAMSCANNER_H

#include "polygon.hpp"
#include "data.hpp"
#include "segment.hpp"
#include "element.hpp"

//FOR GUI
#include "view.hpp"

struct Target {
  Target(int ix, int iy, int s): x(ix), y(iy), status(s){}
  int x,y,status;
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
    bool stop;

    double speed_level1;
    double speed_level2;
    double speed_level3;

    double depthThreshold;

    /**sends the boat to a new position
     *  x,y index for element
     *  ignoredepth: false-> stop and return false if it is shallow
       *             true -> dont stop if it is shallow, but return false if stuck
     * return: true ->target reached,
     *         false-> failed to reach target
     */
    bool gotoElement(int x, int y, bool ingoreDepth);

    /** Uses A* to find the best route to the element
     * return: true : sucess, it arrived as it was supposed to
     *         false: failed
     */
    bool traveltoElement(int x, int y);

    /** Looks for accessible unscanned elements
     *  returns: index for best target based on some calculations
     */
    Target findFarAway(int currentX,int currentY);

    /** Calculates the values of surrounding elements and return the index for the best one
     *  returns: index for best target based on some calculations
     */
    Target findClose(int currentX,int currentY);

    double scanValue(int x,int y,int originX,int originY, int recursivedepth);
    double nearValue(int x,int y,int originX,int originY, int recursivedepth);
    double headingValue(int x,int y,int originX,int originY);

    public:
    /**Constructor*/
    SingleBeamScanner(Data* data, Polygon* polygon,int delay, double delta,double tol);

    /**Destructor*/
    ~SingleBeamScanner();

    /**Start scan*/
    void startScan();

    /**stop scan*/
    void abortScan();
};



#endif // SWEEPINGSSCANNER_H
