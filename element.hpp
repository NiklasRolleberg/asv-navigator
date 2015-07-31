#ifndef ELEMENT_H
#define ELEMENT_H

#include <vector>

/** each element represents a small square, it containes data for how many times
 it has been visited, the depth and also the neighbouring elements, etc.*/
class Element
{
private:

  std::vector<Element*> neighbours;
  int indexX;
  int indexY;
  double posX;
  double posY;
  double accumulatedDepth;
  int timesVisited;
  int status;

public:
  Element(){};
  Element(double px, double py, int ix, int iy);
  ~Element();

  int getStatus();
  void setStatus(int newStatus);
  void updateDepth(double newDepth);
  double getDepth();
  int getTimesVisited();

  //int[] getIndex();
  double getX();
  double getY();
  std::vector<Element*>* getNeighbours();
};


#endif //ELEMENT_H
