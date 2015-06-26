#ifndef TASK_H
#define TASK_H

/** A task could be to go to some coordinates, scan a polygon, wait
    or something else*/
class Task
{
private:
    int type;
    int delay = 0;

public:
    /**Default constructor (type 0)*/
    Task();

    /** Constructor for wait (type 1)*/
    Task(int wait, bool drift);

    /** Constructor for traveling to coordinates (type 2)*/
    Task(double coordinates1, double coordinates2);

    /** Constructor for scanning a polygon (type 3)*/
    Task(int polygon);

    /**Destructor*/
    ~Task();

    /**Get Task type*/
    int getType();

    int getSleepTime();
};

#endif //TASK_H
