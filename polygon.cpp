#include "polygon.hpp"
#include <iostream>
#include <vector>
#include <limits>
#include <fstream>
#include <cmath>
#include <set>
#include <algorithm>
#include "element.hpp"
#include "segment.hpp"

//FOR GUI
#include "view.hpp"
IMPLEMENT_APP_NO_MAIN(PathView);
IMPLEMENT_WX_THEME_SUPPORT;

Polygon::Polygon(std::vector<double> *lat, std::vector<double> *lon)
{
  std::cout << "Polygon constructor" << std::endl;
  latitude = lat;
  longitude = lon;
  xPoints = NULL;
  yPoints = NULL;
  localSet = false;
  matrix = NULL;
  /*
  for(int i=0;i<latitude->size();i++) {
      std::cout << "Latitude: " << latitude->at(i) << std::endl;
  }
  */

  showGUI = true;
  GUI = NULL;
}

Polygon::~Polygon()
{
  //std::cout << "polygon destructor called" << std::endl;
  //delete element matrix
  showGUI = false;
  if (GUI != NULL)
    delete GUI;

  if(matrix != NULL)
  {
    for (int i = 0; i < nx; ++i)
    {
      for (int j = 0; j < ny; ++j)
      {
        delete matrix[i][j]; //OK
      }
    }
    for (int i = 0; i < nx; ++i)
    {
      delete[] matrix[i]; //OK
    }
    delete[] matrix; // OK
    std::cout << "Matrix deleted" << std::endl;
  }


  while(polygonSegments.size() > 0)
  {
    //remove polygonSegment
    delete polygonSegments.at(0);
    polygonSegments.erase (polygonSegments.begin()+0);
    //std::cout << "vecor size:" << polygonSegments.size() << std::endl;
  }

  //std::cout << "polygonSegments deleted" << std::endl;


  delete xPoints;
  delete yPoints;

  //std::cout << "x&y deleted" << std::endl;

  delete latitude;
  delete longitude;

  //std::cout << "latlong deleted" << std::endl;

  std::cout << "Polygon destructor" << std::endl;
}

std::vector<double>* Polygon::getLonBoundaries()
{
    return longitude;
}

std::vector<double>* Polygon::getLatBoundaries()
{
    return latitude;
}

void Polygon::setLocalBoundaries(std::vector<double>*x,std::vector<double>*y)
{
  std::cout << "Polygon set local boundaries" << std::endl;
  xPoints = x;
  yPoints = y;
  localSet = true;
}

void Polygon::setGridSize(double d)
{
  std::cout << "polygon setGridSize" << std::endl;
  delta = d;
}

void Polygon::initialize()
{
  std::cout << "Polygon initialize" << std::endl;

  if(!localSet)
    return;

  std::cout << "calculating min,max for x and y" << std::endl;

  maxX = std::numeric_limits<double>::min();
  minX = std::numeric_limits<double>::max();
  maxY = std::numeric_limits<double>::min();
  minY = std::numeric_limits<double>::max();

  for(int i=0;i<xPoints->size();i++)
  {
    double x = xPoints->at(i);
    double y = yPoints->at(i);

    if(x<minX)
      minX = x;
    if(x>maxX)
      maxX = x;
    if(y<minY)
      minY = y;
    if(y>maxY)
      maxY = y;
  }

  std::cout << "maxX: " << maxX << ", minX: " << minX << "\nmaxY: " << maxY << ", minY: " << minY << std::endl;


  //calculate grid size
  nx = (maxX-minX)/delta;
  ny = (maxY-minY)/delta;
  nx+=1; //compensate for truncation error
  ny+=1;

  std::cout << "delta=" << delta << "-> grid size is: " << nx << "x" << ny << std::endl;

  //(1) create a polygon-segment object for the entire polygon (searchCell in kexet)
  //create new vectors for the polygonsegment
  std::vector<double>* xPoints_copy = new std::vector<double>();
  std::vector<double>* yPoints_copy = new std::vector<double>();
  for(int i=0;i<xPoints->size();i++)
  {
    xPoints_copy->push_back(xPoints->at(i));
    yPoints_copy->push_back(yPoints->at(i));
  }
  polygonSegments.push_back(new PolygonSegment(xPoints_copy,yPoints_copy));

  //TODO (2) (check if the segment is convex, if not it should be triangulated) kanske senare iaf

  //nx = 3;
  //ny = 2;
  // Create the 2d array:
  matrix = new Element**[nx];
  for (int i = 0; i < nx; ++i)
  {
    matrix[i] = new Element*[ny];
  }
  // Create the Elements:
  for (int i = 0; i < nx; ++i)
  {
    for (int j = 0; j < ny; ++j)
    {
      matrix[i][j] = new Element(minX + delta*i, minY + delta*j,i,j);
      bool inside = false;
      for(int k=0; k<polygonSegments.size();k++)
      {
        if(polygonSegments.at(k)->contains(minX + delta*i, minY + delta*j))
        {
          inside = true;
          break;
        }
      }

      if(inside)
        matrix[i][j]->setStatus(0);
      else
        matrix[i][j]->setStatus(5);
    }
  }
  std::cout << nx*ny << " elements created" << std::endl;


  //add neighbours to elements that are inside the polygon
  for (int i = 0; i < nx; ++i)
  {
    for (int j = 0; j < ny; ++j)
    {
      int x[] = {i , i+1, i+1 ,i+1 ,i ,i-1 ,i-1 ,i-1};
      int y[] = {j-1 , j-1, j ,j+1 ,j+1 ,j+1 ,j ,j-1};

      for(int k=0;k<8;k++)
      {
        int ix = x[k];
        int iy = y[k];

        if(ix>=0 && ix<nx && iy>=0 && iy<ny)
        {
          matrix[i][j]->addNeighBour(matrix[ix][iy]);
        }
      }
    }
  }

  //TODO (add elements-pointers to the polygon segment objects)
  for(int i=0;i<polygonSegments.size();i++)
    addBoundaryElements(polygonSegments.at(i));

  /*
  std::vector<PolygonSegment*>* tri = triangulateRegion(polygonSegments.at(0));
  //removeRegion(polygonSegments.at(0));
  if(tri != NULL)
  {
    for(int i=0;i<tri->size();i++)
      polygonSegments.push_back(tri->at(i));
  }
*/
  if(showGUI)
  {
    GUI = new PathView();//a1,a2
    std::vector<PolygonSegment*>* pointer;
    pointer = &polygonSegments;
    GUI->start(500,std::max(maxX,maxY),matrix,nx,ny,pointer);

    GUI->drawPolygon(getXBoundaries(),getYBoundaries());
    GUI->update();
  }
}

void Polygon::addBoundaryElements(PolygonSegment* ps)
{
  std::cout << "add boundary elements to polygonsegment" << std::endl;
  //works for convex segments

  /*
  std::cout << "info about ps:" << std::endl;
  std::cout << "xMin: " << ps->xMin << std::endl;
  std::cout << "xMax: " << ps->xMax << std::endl;
  std::cout << "yMin: " << ps->yMin << std::endl;
  std::cout << "yMax: " << ps->yMax << std::endl;
  */
  double y = ps->yMin;

  while(y < ps->yMax)
  {
    double xpos1 = ps->findX(y, true);
    double xpos2 = ps->findX(y, false);
    int xIndex1 = (int) round((xpos1 - minX) / delta);
    int xIndex2 = (int) round((xpos2 - minX) / delta);
    int yIndex = (int)  round((y - minY) / delta);

    if(yIndex<ny && yIndex>=0)
    {
      if(xIndex1<nx && xIndex1>=0)
      {
        ps->addBoundaryElement(matrix[xIndex1][yIndex]);
        //matrix[xIndex1][yIndex]->setStatus(3); //DEBUG
      }
      if(xIndex2<nx && xIndex2>=0)
      {
        ps->addBoundaryElement(matrix[xIndex2][yIndex]);
        //matrix[xIndex2][yIndex]->setStatus(3); //DEBUG
      }
    }
    y+=delta/2.0;
  }

  double x = ps->xMin;
  while(x < ps->xMax)
  {
    double ypos1 = ps->findY(x, true);
    double ypos2 = ps->findY(x, false);
    int xIndex = (int)  round((x - minX) / delta);
    int yIndex1 =(int)  round((ypos1 - minY) / delta);
    int yIndex2 =(int)  round((ypos2 - minY) / delta);

    if(xIndex<nx && xIndex>=0)
    {
      if(yIndex1<ny && yIndex1>=0)
      {
        ps->addBoundaryElement(matrix[xIndex][yIndex1]);
        //matrix[xIndex][yIndex1]->setStatus(3); //DEBUG
      }
      if(yIndex2<ny && yIndex2>=0)
      {
        ps->addBoundaryElement(matrix[xIndex][yIndex2]);
        //matrix[xIndex][yIndex2]->setStatus(3); //DEBUG
      }
    }
    x+=delta/2.0;
  }
}

void Polygon::removeRegion(PolygonSegment* region)
{
  std::cout << "deleting region" << std::endl;
  //(1) find index
  int index = -1;
  for(int i=0;i<polygonSegments.size();i++)
  {
    if( region == polygonSegments.at(i))
    {
      index = i;
      break;
    }
  }

  if(index == -1)
    return;

  delete polygonSegments.at(index);
  polygonSegments.erase (polygonSegments.begin()+index);
}

void Polygon::removeAllRegions()
{
  std::cout << "deleting all regions" << std::endl;

  for(int i=0;i<polygonSegments.size();i++)
  {
    delete polygonSegments.at(i);
  }
  polygonSegments.clear();
}


void Polygon::generateRegions()
{
  std::cout << "generateRegions()" << std::endl;

  //(1) Find unknown elements
  //(2) expand with known neighbours
  //(3) find the convex hull
  //(4) create a polygonSegment
  //(5) add boundaryElements

  //Find unknown elements
  std::set<Element*> not_scanned;
  for(int i=0;i<nx;i++)
  {
    for(int j=0;j<ny;j++)
    {
      if(matrix[i][j]->getStatus() == 0)
      {
        not_scanned.insert(matrix[i][j]);
        //matrix[i][j]->setStatus(2);
      }
    }
  }
  std::cout << not_scanned.size() << " unscanned elements found" << std::endl;

  std::vector<std::set<Element*>> clusters;

  while(true)
  {
    if(not_scanned.empty())
      break;

    std::set<Element*> cluster;
    cluster.insert(*not_scanned.begin());
    not_scanned.erase(not_scanned.begin());
    //std::cout << "cluster size:" << cluster.size() << std::endl;
    for(std::set<Element*>::iterator it=cluster.begin(); it!=cluster.end(); ++it)
    {
      for(int i=0;i< (*it)->getNeighbours()->size();i++)
      {
        if(not_scanned.find((*it)->getNeighbours()->at(i)) != not_scanned.end())
        {
          cluster.insert((*it)->getNeighbours()->at(i));
          not_scanned.erase(not_scanned.find((*it)->getNeighbours()->at(i))); //TODO kan göras bättre
        }
      }
    }
    clusters.push_back(cluster);
  }

  std::cout << clusters.size() << " new regions will be created" << std::endl;

  //expand with known neighbours
  for(int i=0;i<clusters.size();i++)
  {
    if(clusters.at(i).size() < 2)
      continue;

    std::set<Element*> neighbours;
    for(std::set<Element*>::iterator it=clusters.at(i).begin(); it!=clusters.at(i).end(); ++it)
    {
      for(int j=0;j<(*it)->getNeighbours()->size();j++)
      {
        if(clusters.at(i).find((*it)->getNeighbours()->at(j)) == clusters.at(i).end()
            && (*it)->getNeighbours()->at(j)->getStatus() == 1)
        {
          neighbours.insert((*it)->getNeighbours()->at(j));
        }
      }
    }

    for(std::set<Element*>::iterator it=neighbours.begin(); it!=neighbours.end(); ++it)
    {
      clusters.at(i).insert(*it);
    }
  }

  for(int i=0;i<clusters.size();i++)
  {

    //TODO create a polygonSegment
    PolygonSegment* newRegion = createSegmentFromElements(clusters.at(i)); //skicka pekare/ref om det går för långsamt
    std::cout << "new region created" << std::endl;

    //TODO add boundaryElements
    if(newRegion != NULL)
    {
      addBoundaryElements(newRegion);
      polygonSegments.push_back(newRegion);
    }
  }

  //DEBUG
  //GUI->update();
  //usleep(10000000);

}

//from https://en.wikibooks.org/wiki/Algorithm_Implementation/Geometry/Convex_hull/Monotone_chain
PolygonSegment* Polygon::createSegmentFromElements(std::set<Element*> cluster)
{

  if(cluster.size()<3)
    return NULL;

  if(cluster.size()==3)
  {
    std::vector<double>* x = new std::vector<double>();
    std::vector<double>* y = new std::vector<double>();

    for(std::set<Element*>::iterator it=cluster.begin(); it!=cluster.end(); ++it)
    {
      x->push_back((*it)->getX());
      y->push_back((*it)->getY());
    }
    PolygonSegment* ps = new PolygonSegment(x,y);
    return ps;
  }


  //find the convex hull
  std::cout << "Convex hull. points before: " << cluster.size() << std::endl;
  std::vector<Point> P;

  std::set<Element*> neighbours;
  for(std::set<Element*>::iterator it=cluster.begin(); it!=cluster.end(); ++it)
  {
    P.push_back(Point((*it)->getX(),(*it)->getY()));
  }

  // Note: the last point in the returned list is the same as the first one.
  int n = P.size(), k = 0;
  std::vector<Point> H(2*n);
  std::sort(P.begin(), P.end());

  // Build lower hull
  for (int i = 0; i < n; ++i)
  {
    while (k >= 2 && cross(H[k-2], H[k-1], P[i]) <= 0) k--;
      H[k++] = P[i];
  }

  // Build upper hull
  for (int i = n-2, t = k+1; i >= 0; i--)
  {
    while (k >= t && cross(H[k-2], H[k-1], P[i]) <= 0) k--;
      H[k++] = P[i];
  }

	H.resize(k);

  std::cout << "Convex hull. points after: " << H.size()-1 << std::endl;

  /*
  for(int i=0;i<H.size();i++)
  {
    std::cout << "Point: (" << H.at(i).x/delta << "," << H.at(i).y/delta << ")" << std::endl;
  }*/

  std::vector<double>* x = new std::vector<double>();
  std::vector<double>* y = new std::vector<double>();

  for(int i=0;i<H.size();i++)
  {
    x->push_back(H.at(i).x);
    y->push_back(H.at(i).y);
  }

  //polygonSegments.push_back(new PolygonSegment(x,y));

  PolygonSegment* ps = new PolygonSegment(x,y);
  return ps;

  //error
  return NULL;
}

// 2D cross product of OA and OB vectors, i.e. z-component of their 3D cross product.
// Returns a positive value, if OAB makes a counter-clockwise turn,
// negative for clockwise turn, and zero if the points are collinear.
double Polygon::cross(const Point &O, const Point &A, const Point &B)
{
	return (long)(A.x - O.x) * (B.y - O.y) - (long)(A.y - O.y) * (B.x - O.x);
}


std::vector<PolygonSegment*>* Polygon::triangulateRegion(PolygonSegment* ps)
{
  std::cout << "EarClipping!" << std::endl;

  //(1) check the number of points
  //(2) check rotation
  //(3) use ear clipping to split polygon into traingles
  //(4) create new polygonsegments and add boundary elements

  //(1)
  if(ps->xPoints->size()<4)
    return NULL;

  //(2) try to use the "cross" function
  std::vector<double> xVertex;
  std::vector<double> yVertex;
  std::vector<PolygonSegment*>* list = new std::vector<PolygonSegment*>();

  //add coordinates
  for(int i=0;i<ps->xPoints->size();i++)
  {
    xVertex.push_back((*ps->xPoints)[i]);
    yVertex.push_back((*ps->yPoints)[i]);
  }

  //Ear clipping method
  int index = 0;
  int triangle[] = {index, index+1, index+2};
  while(true)
  {
    bool ear = true;
    //Check if triangle = ear
    double triangleX[3];
    double triangleY[3];


    for(int i=0;i<3;i++)
    {
      triangleX[i] = xVertex[triangle[i]];
      triangleY[i] = yVertex[triangle[i]];
    }
    for(int i=0; i<xVertex.size(); i++)
    {
      if(i == triangle[0] || i == triangle[1] || i == triangle[2])
        continue;

      //TODO FIXA DETTA
      if(false)//insideTriangle(triangleX, triangleY, xVertex.get(i), yVertex.get(i)))
      {
        //System.out.println("No ear");
        ear = false;
      }
    }

    Point A(triangleX[0],triangleY[0]);
    Point B(triangleX[1],triangleY[1]);
    Point C(triangleX[2],triangleY[2]);
    std::cout << "Earclipping cross: " << cross(A, B, C) << std::endl;
    if(cross(A, B, C) > 0) //TODO testa saker
    {
      //System.out.println("No ear");
      ear = false;
    }

    //if triangle is an ear, remove the ear and create a SearchCell
    if(ear)
    {
      //System.out.println("Ear found");
      std::vector<double>* X = new std::vector<double>();
      std::vector<double>* Y = new std::vector<double>();

      for(int j=0;j<3;j++)
      {
        X->push_back(xVertex.at(triangle[j]));
        Y->push_back(yVertex.at(triangle[j]));
      }

      list->push_back(new PolygonSegment(X,Y));
      //System.out.println("Removing vertexes");
      xVertex.erase(xVertex.begin()+triangle[1]);
      yVertex.erase(yVertex.begin()+triangle[1]);
      index = triangle[1];
      //System.out.println("Remaining vertexes: " + xVertex.size());
    }
    if(!ear)
    {
      index++;
    }

    if(xVertex.size() < 4)
    {
      //System.out.println("Earclipping done!");
      std::vector<double>* X = new std::vector<double>();
      std::vector<double>* Y = new std::vector<double>();

      for(int j=0;j<xVertex.size();j++)
      {
        X->push_back(xVertex.at(j));
        Y->push_back(yVertex.at(j));
      }
      list->push_back(new PolygonSegment(X,Y));
      break;
    }

    triangle[0] = index % xVertex.size();
    triangle[1] = (index+1) % xVertex.size();
    triangle[2] = (index+2) % xVertex.size();

    std::cout << "EarClipping: list size " << list->size() << std::endl;
  }

  return list;
}

/////////////////////////////////////////////////////////////////////////////////////////

bool Polygon::BFS(Element* e, std::set<Element*> &container)
{
  if(e->getStatus() == 1)
    return true;
  if(e->getStatus() != 0)
    return false;

  std::vector <Element*>* neighbours = e->getNeighbours();

  for(int i=0;i<neighbours->size();i++)
  {
    Element* n = neighbours->at(i);
    if(container.find(n) != container.end())
      continue;
    container.insert(n);
    if(BFS(n,container))
      return true;
  }
  return false;
}

void Polygon::idland()
{
  // Finds regions enclosed by land and changes status to 2
  std::cout << "idland" << std::endl;
  std::set<Element*> container;
  for(int i=0; i<nx; i++)
  {
    for(int j=0; j<ny; j++)
    {
      container.clear();
      if(matrix[i][j]->getStatus() == 0)
      {
        if(!BFS(matrix[i][j], container))
        {
          matrix[i][j]->setStatus(2);
          for(std::set<Element*>::iterator it=container.begin(); it!=container.end(); ++it)
          {
            (*it)->setStatus(2);
          }
        }
      }
    }
  }
}

/////////////////////////////////////////////////////////////////////////////////////////

double** Polygon::createCostMatrix(int cx, int cy)
{

  //std::cout << "createCostMatrix: (" << cx << "," << cy << ")" << std::endl;

  if(!localSet)
    return NULL;

  double** g_score = new double*[nx];
  //double g_score[nx][ny];
  double f_score[nx][ny];
  //std::cout << "(1)" << std::endl;
  for (int i = 0; i < nx; ++i)
  {
    g_score[i] = new double[ny];
  }
  for(int i=0;i<nx;i++)
    for(int j=0;j<ny;j++)
      g_score[i][j] = -1;

  //std::cout << "(2)" << std::endl;

  std::set<Element*> openSet;
  std::set<Element*> closedSet;

  //std::cout << "(3)" << std::endl;
  openSet.insert(matrix[cx][cy]);

  g_score[cx][cy] = 0;
  f_score[cx][cy] = 0;

  while(!openSet.empty())
  {
    //std::cout << "openSet: " << openSet.size() << std::endl;
    //std::cout << "closedSet: " << closedSet.size() << std::endl;

    //find node with lowest f_score value
    double min = std::numeric_limits<double>::max();
    Element* current = NULL;
    for(std::set<Element*>::iterator it=openSet.begin(); it!=openSet.end(); ++it)
    {
      //std::cout << "it:" << &(*it) << std::endl;
      int ix = (*it)->getIndexX();
      int iy = (*it)->getIndexY();
      if(f_score[ix][iy] <= min)
      {
        min = f_score[ix][iy];
        current = (*it);
      }
    }

    openSet.erase(current);
    closedSet.insert(current);

    if(current == NULL)
      std::cout << "current is NULL" << std::endl;

    /*
    std::cout << "current:" << std::endl;
    std::cout << "pos:" << current->getX() << "," << current->getY() << std::endl;
    std::cout << "index:" << current->getIndexX() << "," << current->getIndexY() << std::endl;
    std::cout << "nr of neighbours:" << current->getNeighbours()->size() << std::endl;
    */
    //itterate through neighbours
    for(int i=0;i<current->getNeighbours()->size();i++)
    {
      Element* n = current->getNeighbours()->at(i);
      if(closedSet.find(n) != closedSet.end() || n->getStatus() != 1)
        continue;

      double dx = n->getX()-current->getX();
      double dy = n->getY()-current->getY();
      //std::cout << "n: " << n << std::endl;
      //std::cout << "index: (" << n->getIndexX() << "," << n->getIndexY() << ")" << std::endl;
      //std::cout << "(dx,dy): (" << dx << "," << dy << ")" << std::endl;
      double tentative_g_score;

      if (g_score[current->getIndexX()][current->getIndexY()] != -1)
        tentative_g_score = g_score[current->getIndexX()][current->getIndexY()]
                            + sqrt(dx*dx + dy*dy
                            + 10*(1.0 / current->getDepth())); //higher cost for low depth
      else
        tentative_g_score = sqrt(dx*dx + dy*dy);

      //std::cout << "F:(1)" << std::endl;

      if (openSet.find(n) == openSet.end() || tentative_g_score < g_score[n->getIndexX()][n->getIndexY()])
      {
        //std::cout << "F:(2)" << std::endl;
        g_score[n->getIndexX()][n->getIndexY()] = tentative_g_score;
        f_score[n->getIndexX()][n->getIndexY()] = tentative_g_score;
        //std::cout << "F:(3)" << std::endl;
        if(openSet.find(n) == openSet.end())
        {
          //std::cout << "polygon.cpp:729 openset.insert()" << n << std::endl;
          openSet.insert(n); //funkar nog
          //std::cout << "polygon.cpp:731 insert done" << n << std::endl;
        }
      }
/*fel*/
      //std::cout << "(4)" << std::endl;
    }
    //std::cout << "(5)" << std::endl;
  }
  std::cout << "costmatrix created" << std::endl;
  return g_score;
}

std::vector<double>* Polygon::getXBoundaries()
{
  return xPoints;
}
std::vector<double>* Polygon::getYBoundaries()
{
  return yPoints;
}

void Polygon::saveMatrix()
{
  if(matrix == NULL)
    return;

  std::cout << "Saving matrix" << std::endl;
  std::string fileName1 = "logs/matrix_depth.csv";
  std::string fileName2 = "logs/matrix_visited.csv";
  std::string fileName3 = "logs/matrix_status.csv";

  std::ofstream *logfile1 = new std::ofstream(fileName1);
  std::ofstream *logfile2 = new std::ofstream(fileName2);
  std::ofstream *logfile3 = new std::ofstream(fileName3);

  for(int j=ny-1;j>0;j--) {
    for(int i=0;i<nx-1;i++) {
        (*logfile1) << matrix[i][j]->getDepth() << ", ";
        (*logfile2) << matrix[i][j]->getTimesVisited() << ", ";
        (*logfile3) << matrix[i][j]->getStatus() << ", ";
    }
    (*logfile1) << matrix[nx-1][j]->getDepth() << std::endl;
    (*logfile2) << matrix[nx-1][j]->getTimesVisited() << std::endl;
    (*logfile3) << matrix[nx-1][j]->getStatus() << std::endl;
  }

  std::cout << "matrix saved" << std::endl;

  logfile1->close();
  logfile2->close();
  logfile3->close();
  delete logfile1;
  delete logfile2;
  delete logfile3;
}


void Polygon::updateView(double currentX, double currentY)
{
  //std::cout << "polygon::update view" << std::endl;
  if(showGUI && GUI != NULL)
  {
    GUI->drawPath(currentX,currentY);
  	GUI->update();
  }
}
