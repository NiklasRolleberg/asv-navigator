#ifndef ELEMENT_H
#define ELEMENT_H

#include <vector>

/** each element represents a small square, it containes data for how many times
 it has been visited, the depth and also the neighbouring elements, etc.*/
class Element
{
private:

  std::vector<Element*> neighbours;
  double posx;
  double posy;
  double accumulatedDepth;
  int timesVisited;
  int status;

public:
  Element(){};
  Element(double px, double py, int ix, int iy);
  ~Element();

  //void getStatus();
  //void setStatus(int newStatus);
  //void updateDepth(double newDepth);
  //double getDepth();

};


#endif //ELEMENT_H
