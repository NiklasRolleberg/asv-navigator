#ifndef POLYGON_H
#define POLYGON_H

#ifndef SHOW_GUI
#define SHOW_GUI false
#endif

#include <vector>
#include <iostream>
#include <set>
#include "element.hpp"
#include "segment.hpp"

#include "view.hpp"

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
		std::vector<PolygonSegment*>* triangulateRegion(PolygonSegment* ps);
    PolygonSegment* createSegmentFromElements(std::set<Element*>);

		/**vecor cross product*/
		double cross(const Point &O, const Point &A, const Point &B);

		/** true if element is connected to a scanned element*/
		bool BFS(Element* e, std::set<Element*> &container);

		bool showGUI;
		PathView* GUI;

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

		/**Save all data so the scan can be restarted from this moment*/
		void saveAll(std::string filename);

    /**Save the element matrix for debuging*/
    void saveMatrix();

    /**remove a region from the list and delete it*/
    void removeRegion(PolygonSegment* region);

		/**remove all regions*/
		void removeAllRegions();

    /**Creates regions from the unscanned elements in the matrix*/
    void generateRegions();

		/**Marks elements contained by land as land*/
		void idland();

		/**Creates a nx x ny matrix with costs for all accessible elements with cost=0 at cx,cy*/
		double** createCostMatrix(int cx, int cy);

		/**update the window*/
		void updateView(double currentX, double currentY);

};

#endif // POLYGON_H
