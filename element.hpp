#ifndef ELEMENT_H
#define ELEMENT_H

#include <vector>

/** each element represents a small square, it containes data for how many times
 it has been visited, the depth and also the neighbouring elements, etc.


 Status of element:
 -1 = default value / error
  0 = unknown
  1 = scanned
  2 = land / not accessible
  3 = probably land
  4 = ?
  5 = outside search area

 */
class Element
{
private:

  std::vector<Element*> neighbours;
  int indexX;
  int indexY;
  double posX;
  double posY;
  int timesVisited;
  int timesTargeted; //used to calculate travelcost.
  int status;

public:
  double accumulatedDepth; // TODO fixa getter
  Element(){};
  Element(double px, double py, int ix, int iy);
  ~Element();

  int getStatus();
  void setStatus(int newStatus);
  void updateDepth(double newDepth);
  double getDepth();
  void targeted();
  int getTimesVisited();
  int getTimesTargeted();

  int getIndexX();
  int getIndexY();

  double getX();
  double getY();

  void addNeighBour(Element* n);
  std::vector<Element*>* getNeighbours();
};


#endif //ELEMENT_H
