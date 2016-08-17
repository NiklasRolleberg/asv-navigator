#ifndef TASK_H
#define TASK_H

#include "polygon.hpp"
#include <string>

/** A task could be to go to some coordinates, scan a polygon, wait
    or something else*/
class Task
{
private:
    int type;
    int delay;// = 0;
    Polygon* polygon_ptr;// = nullptr;
    std::string message;// = "";
    double targetLat;
    double targetLon;


public:
    int old_nx;
    int old_ny;
    int* old_status;
    double* old_depth;
    int* old_visited;


    /**Default constructor (type 0)*/
    Task();

    /** Constructor for wait (type 1)*/
    Task(int wait, bool drift);

    /** Constructor for traveling to coordinates (type 2)*/
    Task(double coordinates1, double coordinates2);

    /** Constructor for scanning a polygon (type 3)*/
    Task(Polygon* polygon);

    /** Constructor for scanning a polygon and use old data (type 5)*/
    Task(Polygon* polygon, int rows, int cols, int* arr1, double* arr2, int* arr3);

    /** Constructor for sending a message  (type 4)*/
    Task(std::string message);

    /**Destructor*/
    ~Task();

    /**Get Task type*/
    int getType();

    int getSleepTime();

    double getTargetLat();
    double getTargetLon();

    Polygon* getPolygon();

    std::string getMessage();
};

#endif //TASK_H
