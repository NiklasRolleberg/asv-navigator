#ifndef POLYGON_H
#define POLYGON_H

#include <vector>
#include "element.hpp"

/**The polygon class contains coordinates for the nodes, both in local and
    lat/long coordinate systems and methods for calculating if a position
    is inside the polygon*/

class Polygon
{
private:

    std::vector<double>* latitude;// = nullptr;
    std::vector<double>* longitude;// = nullptr;

    bool localSet;
    std::vector<double>* xPoints;// = nullptr;
    std::vector<double>* yPoints;// = nullptr;

    double maxX;
    double minX;
    double maxY;
    double minY;

    double delta;
    int nx;
    int ny;

    Element*** matrix;


public:
    /**Constructor*/
    Polygon(int points, std::vector<double> *lat, std::vector<double> *lon);

    /**Destructor*/
    ~Polygon();

    //for global coordinates
    std::vector<double>* getLonBoundaries();
    std::vector<double>* getLatBoundaries();

    /** set boundaries for the local coorinate system */
    void setLocalBoundaries(std::vector<double>*x,std::vector<double>*y);

    /**set the grid resolution*/
    void setGridSize(double delta);

    /**Calculate min,max values for area and creates the grid matrix
       and creates the first polygonsegment (searchCell i kexet)*/
    void initialize();

    std::vector<double>* getXBoundaries();
    std::vector<double>* getYBoundaries();

  /* more functions to be implemented
     get segmentList <vector kanske>
     get matrix
     bool insidePolygon
     get max,min of x and y
     get delta?

  */



};



#endif // POLYGON_H
