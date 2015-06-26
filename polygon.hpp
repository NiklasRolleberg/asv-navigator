#ifndef POLYGON_H
#define POLYGON_H

/**The polygon class contains coordinates for the nodes, both in local and
    lat/long coordinate systems and methods for calculating if a position
    is inside the polygon*/

class Polygon
{
private:
    int maxxyminxyskafinnasmed = 0;

public:
    /**Constructor*/
    Polygon(int points);

    /**Destructor*/
    ~Polygon();
};



#endif // POLYGON_H


