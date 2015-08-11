#include "polygon.hpp"
#include <iostream>
#include <vector>
#include <limits>
#include <fstream>
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

  //add a few meters to avoid segmentation fault
  //minX-=10;
  //minY-=10;
  //maxX+=10;
  //maxY+=10;

  std::cout << "maxX: " << maxX << ", minX: " << minX << "\nmaxY: " << maxY << ", minY: " << minY << std::endl;


  //calculate grid size
  nx = (maxX-minX)/delta;
  ny = (maxY-minY)/delta;
  nx+=1; //compensate for truncation error
  ny+=1;

  std::cout << "delta=" << delta << "-> grid size is: " << nx << "x" << ny << std::endl;

  //TODO (1) create a polygon-segment object for the entire polygon (searchCell in kexet)

  // add PolygonSegment
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

      //TODO set the status of the element depending on if it is outside or inside the polygon
      //TODO check this by using methods in the polygon-segment class
    }
  }
  std::cout << nx*ny << " elements created" << std::endl;


  //TODO add neighbours to elements that are inside the polygon

  //TODO (add elements-pointers to the polygon segment objects)
}

void Polygon::removeRegion(PolygonSegment* region)
{
  std::cout << "deleting region" << std::endl;
}


void Polygon::generateRegions()
{
  std::cout << "generateRegions()" << std::endl;
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
