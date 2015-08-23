#ifndef VIEW_H
#define VIEW_H

#include <thread>
#include "wx/wx.h"
#include "wx/sizer.h"
#include <vector>

class BasicDrawPane : public wxPanel
{

   wxBitmap bmp;
   int windowSize;

public:
    BasicDrawPane(wxFrame* parent, int windowSize);

    ~BasicDrawPane();

    void paintEvent(wxPaintEvent & evt);
    void paintNow();

    void render(wxDC& dc);

    void drawLine(int startx, int starty, int stopx,int stopy,int width,int r, int g , int b);

    DECLARE_EVENT_TABLE()
};


class View: public wxApp
{
    bool OnInit();
    void init();

    std::thread* uiThread;

    wxFrame *frame;
    BasicDrawPane * drawPane;

    int argc;
    char **argv;

    int windowSize;
    int maxXY;

    double lastPosx;
    double lastPosy;

public:
  View();
  View(int argc, char *argv[]);
  ~View();
  void start(int windowSize, int maxXY);

  void update();

  void drawPolygon(std::vector<double>* x, std::vector<double>* y);

  void drawPath(double posx, double posy);

  void drawLine(int startx, int starty, int stopx,int stopy,int width, int r, int g , int b);
};

#endif //VIEW_H
