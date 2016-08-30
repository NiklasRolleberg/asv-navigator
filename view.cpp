#include <thread>
#include "wx/wx.h"
#include "wx/sizer.h"
#include <vector>
#include "view.hpp"
#include "polygon.hpp"
#include "element.hpp"
#include "segment.hpp"
#include <ctime>


PathView::PathView()
{
  std::cout << "PathView constructor" << std::endl;
  uiThread = NULL;
  lastPosx = 0;
  lastPosy = 0;

  matrix = NULL;
  regions = NULL;
}


PathView::PathView(int a1, char** a2)
{
  std::cout << "PathView constructor 2" << std::endl;
  uiThread = NULL;
  lastPosx = 0;
  lastPosy = 0;
  //argc = a1;
  //argv = a2;
  matrix = NULL;
  regions = NULL;


  //TODO save stuff

}

PathView::~PathView()
{
  std::cout << "PathView destructor" << std::endl;
  //GetTopWindow()->Destroy();//Close(false);
  if(pathFrame != NULL)
  {
    pathFrame->Destroy();
    pathFrame->Refresh();
  }

  if(matrixFrame != NULL)
  {
    matrixFrame->Destroy();
    matrixFrame->Refresh();
  }

  if(uiThread != NULL)
  {
    if(uiThread->joinable())
      uiThread->join();
  }
}

void PathView::start(int ws, int max, Element*** m, int x, int y, std::vector<PolygonSegment*>* polygonSegments)
{

  matrix = m;
  regions = polygonSegments;
  nx = x;
  ny = y;
  windowSize = ws;
  maxXY = max;
  lastUpdate = clock();
  if(uiThread == NULL)
    uiThread = new std::thread(&PathView::init,this);
  usleep(500000);
}

void PathView::init()
{
  //std::cout << "PathView init" << std::endl;
  argc = 0;
  argv = new char*[argc];
  wxEntryStart( argc, argv );
  wxTheApp->CallOnInit();
  wxTheApp->OnRun();
}

bool PathView::OnInit()
{
  //For path View
  //std::cout << "PathView OnInit" << std::endl;
  wxBoxSizer* sizer = new wxBoxSizer(wxHORIZONTAL);
  pathFrame = new wxFrame((wxFrame *)NULL, -1,  wxT("Path"), wxPoint(50,50), wxSize(windowSize,windowSize+25));
  pathDrawPane = new BasicDrawPane( (wxFrame*) pathFrame , windowSize);
  sizer->Add(pathDrawPane, 1, wxEXPAND);

  pathFrame->SetSizer(sizer);
  pathFrame->SetAutoLayout(true);

  //for matrix view
  //matrixFrame
  wxBoxSizer* sizer2 = new wxBoxSizer(wxHORIZONTAL);
  matrixFrame = new wxFrame((wxFrame *)NULL, -1,  wxT("matrix"), wxPoint(550,50), wxSize(windowSize,windowSize+25));
  //matrixFrame = new wxFrame((wxFrame *)pathFrame, -1,  wxT("matrix"), wxPoint(50,50), wxSize(windowSize,windowSize+25));
  matrixDrawPane = new BasicDrawPane( (wxFrame*) matrixFrame , windowSize);
  sizer2->Add(matrixDrawPane, 1, wxEXPAND);

  matrixFrame->SetSizer(sizer2);
  matrixFrame->SetAutoLayout(true);

  matrixDrawPane->setMatrix(matrix,nx,ny);
  matrixDrawPane->setRegionContainer(regions, maxXY);

  pathFrame->Show();
  matrixFrame->Show();

  return true;
}

void PathView::drawPolygon(std::vector<double>* x, std::vector<double>* y)
{
  int m = std::min(x->size(), y->size());
  for(int i=1;i<= m;i++)
  {
    drawLine(x->at(i-1), y->at(i-1), x->at(i%m), y->at(i%m),5, 0,255,0);
  }
}

void PathView::drawPath(double posx, double posy,double heading, int max)
{
  if(lastPosx == 0 && lastPosy == 0)
  {
    lastPosx = posx;
    lastPosy = posy;
    maxXY = max;
    return;
  }
  pathDrawPane->setBoatPos(posx,posy,heading,max);
  drawLine((int) lastPosx, (int) lastPosy, (int) posx,(int) posy, 3, 255,0,0);
  lastPosx = posx;
  lastPosy = posy;
}

void PathView::drawLine(int startx, int starty, int stopx,int stopy,int width, int r, int g , int b)
{
  double scale = (double)windowSize / (double)maxXY;
  //std::cout << "drawing line" << std::endl;
  double x1 = startx*scale;
  double y1 = windowSize - starty*scale;
  double x2 = stopx*scale;
  double y2 = windowSize - stopy*scale;

  pathDrawPane->drawLine((int)x1,(int)y1, (int)x2, (int)y2,width,r,b,g);
}

void PathView::update()
{

  //Do stuff

  clock_t time = clock() - lastUpdate;
  double ms =  (((double) time / CLOCKS_PER_SEC) * 1000); //cpu-time
  //std::cout << "ms:" << ms << std::endl;
  if(ms > 1)
  {
    lastUpdate = clock();
    if(pathFrame != NULL)
      pathFrame->Refresh();
    if(matrixFrame != NULL)
      matrixFrame->Refresh();
  }
}


BEGIN_EVENT_TABLE(BasicDrawPane, wxPanel)
// catch paint events
EVT_PAINT(BasicDrawPane::paintEvent)

END_EVENT_TABLE()


BasicDrawPane::BasicDrawPane(wxFrame* parent, int ws) : wxPanel(parent)
{
  std::cout << "DrawPane constructor" << std::endl;
  windowSize = ws;
  bmp = wxBitmap(windowSize, windowSize, -1);
  drawMatrix = false;
  matrix = NULL;
  regions = NULL;
}


BasicDrawPane::~BasicDrawPane()
{
  std::cout << "DrawPane destructor" << std::endl;

  if(!drawMatrix)
  {
    wxString test(_T("logs/bild.png"));
    bmp.SaveFile(test, wxBITMAP_TYPE_PNG);
  }
}

void BasicDrawPane::setBoatPos(double x,double y,double heading,int max)
{
  boatX = x;
  boatY = y;
  boatHeading = heading;
  maxXY = max;
}

void BasicDrawPane::drawLine(int startx, int starty, int stopx,int stopy,int width,int r, int g , int b)
{
  wxMemoryDC memDC;
  memDC.SelectObject(bmp);

  memDC.SetPen( wxPen( wxColor(r,b,g), width ) ); // black line, 3 pixels thick
  memDC.DrawLine( startx, starty, stopx, stopy ); // draw line across the rectangle
}

void BasicDrawPane::paintEvent(wxPaintEvent & evt)
{
    wxPaintDC dc(this);
    render(dc);
}

void BasicDrawPane::paintNow()
{
    wxClientDC dc(this);
    render(dc);
}

void BasicDrawPane::render(wxDC&  dc)
{
  if(!drawMatrix)
  {
    dc.DrawBitmap(bmp, 0, 0, false);

    double scale = (double)windowSize / (double)maxXY;
    int r = 6;
    int cx = scale*boatX-r/2;
    int cy = windowSize-(scale*boatY);

    // draw a circle
    dc.SetBrush(*wxYELLOW_BRUSH); // green filling
    dc.SetPen( wxPen( wxColor(255,255,0), 6 ) ); // 5-pixels-thick red outline
    dc.DrawCircle( wxPoint(cx,cy), 5 /* radius */ );

    dc.DrawLine(cx+(6*sin(boatHeading)),cy+(6*cos(boatHeading)),cx+(10*cos(boatHeading)),cy-(10*sin(boatHeading)));
    dc.DrawLine(cx-(6*sin(boatHeading)),cy-(6*cos(boatHeading)),cx+(10*cos(boatHeading)),cy-(10*sin(boatHeading)));
  }
  if(drawMatrix)
  {
    double dx = windowSize/nx;
    double dy = windowSize/ny;

    double maxDepth = 1;
    //find maximum depth
    for(int i=0;i<nx;i++)
      for(int j=0;j<ny;j++)
        maxDepth = std::max(maxDepth,matrix[i][j]->getDepth());

    for(int j=0;j<ny;j++) {
      for(int i=0;i<nx;i++) {
        // draw a rectangle
        if(matrix[i][j]->getStatus() == 0)
          dc.SetBrush(*wxWHITE_BRUSH); // not scanned

        if(matrix[i][j]->getStatus() == 1)
        {
            //dc.SetBrush(*wxGREEN_BRUSH); // scanned
            //set color for scanned area
            double d = std::min(maxDepth,matrix[i][j]->getDepth());
            int red   = 0;
            int green = ((maxDepth-d)/maxDepth)*128;
            int blue  = green*2;

            red = std::max(0,std::min(255,red));
            green = std::max(0,std::min(128,green));
            blue = std::max(0,std::min(255,blue));
            /*
            if(matrix[i][j]->getDepth() > 10)
            {
              red = 0;
              green = 20;
              blue = 20;
            }
            else if (matrix[i][j]->getDepth() > 6)
            {
              red = 0;
              green = 0;
              blue = 50;
            }
            else if (matrix[i][j]->getDepth() > 4)
            {
              red = 0;
              green = 0;
              blue = 120;
            }
            else if (matrix[i][j]->getDepth() > 2)
            {
              red = 0;
              green = 0;
              blue = 200;
            }
            */
            wxBrush brush(wxColor(red,green,blue),1);
            dc.SetBrush(brush);
        }
        if(matrix[i][j]->getStatus() == 2)
            dc.SetBrush(*wxRED_BRUSH); // land

        if(matrix[i][j]->getStatus() == 3)
            dc.SetBrush(*wxYELLOW_BRUSH); // probably land

        if(matrix[i][j]->getStatus() == 5)
          dc.SetBrush(*wxGREY_BRUSH); // outside

        double x1 = scale*matrix[i][j]->getX();
        double y1 = scale*matrix[i][j]->getY();
        double d = std::min(dx,dy);
        dc.SetPen( wxPen( wxColor(255,255,255), 1 ) );
        dc.DrawRectangle( x1-(d/2), windowSize-y1-(d/2), d, d );
      }
    }

    //draw regions
    if(regions == NULL)
      return;

    for(int i=0;i<regions->size();i++)
    {
      PolygonSegment* ps = regions->at(i);

      int s = ps->xPoints->size();
      for(int j=0;j<s;j++)
      {
        double x1 = scale*ps->xPoints->at(j);
        double y1 = scale*ps->yPoints->at(j);
        double x2 = scale*ps->xPoints->at((j+1) % s);
        double y2 = scale*ps->yPoints->at((j+1) % s);

        // draw a line
        dc.SetPen( wxPen( wxColor(0,0,0), 3 ) ); // black line, 3 pixels thick
        //dc.DrawLine( 300, 100, 700, 300 ); // draw line across the rectangle
        dc.DrawLine(x1,windowSize - y1, x2, windowSize-y2);
      }
    }
  }
}

void BasicDrawPane::setMatrix(Element*** m, int x, int y)
{
  matrix = m;
  nx = x;
  ny = y;
  drawMatrix = true;
}

void BasicDrawPane::setRegionContainer(std::vector<PolygonSegment*>* r, double max)
{
  regions = r;
  scale = (double)windowSize / max;
}
