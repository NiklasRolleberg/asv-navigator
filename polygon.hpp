#ifndef POLYGON_H
#define POLYGON_H

#include <vector>
#include <iostream>
#include <set>
#include "element.hpp"
#include "segment.hpp"


struct Point {
	double x, y;

  Point(int px = 99, int py = -99): x(px), y(py){}

	bool operator <(const Point &p) const
  {
		return x < p.x || (x == p.x && y < p.y);
	}
};

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

    void addBoundaryElements(PolygonSegment* ps);
    PolygonSegment* createSegmentFromElements(std::set<Element*>);
    double cross(const Point &O, const Point &A, const Point &B);

public:

    int nx;
    int ny;

    double maxX;
    double minX;
    double maxY;
    double minY;
    double delta;


    Element*** matrix;
    std::vector<PolygonSegment*>polygonSegments;

    /**Constructor*/
    Polygon(std::vector<double> *lat, std::vector<double> *lon);

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

    /**Save the element matrix for debuging*/
    void saveMatrix();

    /**remove a region from the list and delete it*/
    void removeRegion(PolygonSegment* region);

    /**Creates regions from the unscanned elements in the matrix*/
    void generateRegions();



};



#endif // POLYGON_H
