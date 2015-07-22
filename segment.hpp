#ifndef SEGMENT_H
#define SEGMENT_H

#include <vector>


class PolygonSegment
{
private:

    std::vector<double> xPoints;// = nullptr;
    std::vector<double> yPoints;// = nullptr;

public:

    double xMax;
    double xMin;
    double yMax;
    double yMin;

    /**Constructor*/
    PolygonSegment(std::vector<double> *x, std::vector<double> *y);

    /**Destructor*/
    ~PolygonSegment();


    bool contains(double x, double y);
    double findX(double y, bool right);
    double maxX();
    double minX();
    double maxY();
    double minY();



};



#endif // SEGMENT_H
