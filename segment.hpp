#ifndef SEGMENT_H
#define SEGMENT_H

#include <vector>


class PolygonSegment
{
private:

    std::vector<double>* xPoints;// = nullptr;
    std::vector<double>* yPoints;// = nullptr;

    double maxX;
    double minX;
    double maxY;
    double minY;

public:

    /**Constructor*/
    PolygonSegment(std::vector<double> *x, std::vector<double> *y);

    /**Destructor*/
    ~PolygonSegment();

};



#endif // SEGMENT_H
