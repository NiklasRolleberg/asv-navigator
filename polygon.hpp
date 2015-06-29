#ifndef POLYGON_H
#define POLYGON_H

#include <vector>

/**The polygon class contains coordinates for the nodes, both in local and
    lat/long coordinate systems and methods for calculating if a position
    is inside the polygon*/

class Polygon
{
private:
    int maxxyminxyskafinnasmed = 0;

    std::vector<double>* latitude = nullptr;
    std::vector<double>* longitude = nullptr;

public:
    /**Constructor*/
    Polygon(int points, std::vector<double> *lat, std::vector<double> *lon);

    /**Destructor*/
    ~Polygon();

    std::vector<double>* getLonBoundaries();
    std::vector<double>* getLatBoundaries();

};



#endif // POLYGON_H


