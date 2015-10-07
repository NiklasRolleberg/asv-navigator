#ifndef SEGMENT_H
#define SEGMENT_H

#include <vector>
#include "element.hpp"

class PolygonSegment
{
private:

    std::vector<Element*> boundaries;
    std::vector<Element*> contained; //kanske ej behövs

public:

    double xMax;
    double xMin;
    double yMax;
    double yMin;

    std::vector<double> *xPoints;// = nullptr;
    std::vector<double> *yPoints;// = nullptr;

    /**Constructor*/
    PolygonSegment(std::vector<double> *x, std::vector<double> *y);

    /**Destructor*/
    ~PolygonSegment();

    void addBoundaryElement(Element* e);
    std::vector<Element*>* getBoundaryElements();
    bool contains(double x, double y);
    double findX(double y, bool right);
    double findY(double x, bool top);
    double maxX();
    double minX();
    double maxY();
    double minY();



};



#endif // SEGMENT_H
