#ifndef VIEW_H
#define VIEW_H

#include <thread>
#include "wx/wx.h"
#include "wx/sizer.h"
#include <vector>
//#include "polygon.hpp"
#include "element.hpp"
#include "segment.hpp"


class BasicDrawPane : public wxPanel
{

   wxBitmap bmp;
   int windowSize;
   Element*** matrix;
   int nx,ny;
   bool drawMatrix;

public:
    BasicDrawPane(wxFrame* parent, int windowSize);

    ~BasicDrawPane();

    void paintEvent(wxPaintEvent & evt);
    void paintNow();

    void render(wxDC& dc);

    void drawLine(int startx, int starty, int stopx,int stopy,int width,int r, int g , int b);

    void setMatrix(Element***,int nx,int ny);

    DECLARE_EVENT_TABLE()
};


class PathView: public wxApp
{
    bool OnInit();
    void init();

    //1 = boat path
    //2 = matrix

    Element*** matrix;
    //std::vector<PolygonSegment*>*regions;
    int nx,ny;

    std::thread* uiThread;

    wxFrame *pathFrame;
    wxFrame *matrixFrame;
    BasicDrawPane * pathDrawPane;
    BasicDrawPane * matrixDrawPane;

    int argc;
    char **argv;

    int windowSize;
    int maxXY;

    double lastPosx;
    double lastPosy;

public:
  PathView();
  PathView(int argc, char *argv[]);
  ~PathView();

  /**Start view for showing matrix*/
  void start(int windowSize, int maxXY, Element*** m, int nx, int ny);//, std::vector<PolygonSegment*>* polygonSegments);

  void update();

  void drawPolygon(std::vector<double>* x, std::vector<double>* y);

  void drawPath(double posx, double posy);

  void drawLine(int startx, int starty, int stopx,int stopy,int width, int r, int g , int b);
};

#endif //VIEW_H
