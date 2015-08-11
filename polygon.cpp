#include "polygon.hpp"
#include <iostream>
#include <vector>
#include <limits>
#include <fstream>
#include <cmath>
#include <set>
#include "element.hpp"
#include "segment.hpp"

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
}

Polygon::~Polygon()
{
    //std::cout << "polygon destructor called" << std::endl;
    //delete element matrix

    if(matrix != NULL)
    {
      for (int i = 0; i < nx; ++i)
      {
        for (int j = 0; j < ny; ++j)
        {
          delete matrix[i][j];
        }
      }
      for (int i = 0; i < nx; ++i)
      {
        delete matrix[i];
      }
      delete matrix;
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

       //void setLocalBoundaries(std::vector<double>*x,std::vector<double>*y)
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
  polygonSegments.push_back(new PolygonSegment(xPoints,yPoints));

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
}

void Polygon::addBoundaryElements(PolygonSegment* ps)
{
  std::cout << "add boundary elements to polygonsegment" << std::endl;
  //works for convex segments

  double y = ps->yMin;

  while(y < ps->yMax)
  {
    double xpos1 = ps->findX(y, true);
    double xpos2 = ps->findX(y, false);
    int xIndex1 = (int) round((xpos1 - ps->xMin) / delta);
    int xIndex2 = (int) round((xpos2 - ps->xMin) / delta);
    int yIndex = (int)  round((y - ps->yMin) / delta);

    ps->addBoundaryElement(matrix[xIndex1][yIndex]);
    ps->addBoundaryElement(matrix[xIndex2][yIndex]);
    y+=delta/2.0;
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
        not_scanned.insert(matrix[i][j]);
    }
  }
  std::cout << "\t" << not_scanned.size() << " unscanned elements found" << std::endl;

  std::vector<std::set<Element*>> clusters;

  while(true)
  {
    if(not_scanned.empty())
      break;

    std::set<Element*> cluster;
    cluster.insert(*not_scanned.begin());
    for(std::set<Element*>::iterator it=cluster.begin(); it!=cluster.end(); ++it)
    {
      for(int i=0;i< (*it)->getNeighbours()->size();i++)
      {
        if(not_scanned.find((*it)->getNeighbours()->at(i)) != not_scanned.end())
        {
          cluster.insert((*it)->getNeighbours()->at(i));
          //std::cout << "cluster size:" << cluster.size() << std::endl;
          not_scanned.erase(not_scanned.find((*it)->getNeighbours()->at(i))); //TODO kan göras bättre
        }
      }
    }
    clusters.push_back(cluster);
  }

  std::cout <<"\t"<< clusters.size() << " new regions will be created" << std::endl;

  //expand with known neighbours
  for(int i=0;i<clusters.size();i++)
  {
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

  //TODO find the convex hull
  for(int i=0;i<clusters.size();i++)
  {
    std::cout << "\tnumber of elements in new cell: " << clusters.at(i).size() << std::endl;

    //TODO do something

    std::cout << "\tnumber of points in convex hull: " << std::endl;
  }

  //TODO create a polygonSegment
  //TODO add boundaryElements


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

  for(int j=0;j<ny;j++) {
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
