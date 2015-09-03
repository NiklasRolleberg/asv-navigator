#include <thread>
#include "wx/wx.h"
#include "wx/sizer.h"
#include <vector>
#include "view.hpp"
#include "polygon.hpp"
#include "element.hpp"
#include "segment.hpp"


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

void PathView::drawPath(double posx, double posy)
{
  if(lastPosx == 0 && lastPosy == 0)
  {
    lastPosx = posx;
    lastPosy = posy;
    return;
  }

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
  if(pathFrame != NULL)
    pathFrame->Refresh();
  if(matrixFrame != NULL)
    matrixFrame->Refresh();
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
    dc.DrawBitmap(bmp, 0, 0, false);

  if(drawMatrix)
  {
    double dx = windowSize/nx;
    double dy = windowSize/ny;

    for(int j=0;j<ny;j++) {
      for(int i=0;i<nx;i++) {
        // draw a rectangle
        if(matrix[i][j]->getStatus() == 0)
          dc.SetBrush(*wxBLACK_BRUSH); // not scanned

        if(matrix[i][j]->getStatus() == 1)
            dc.SetBrush(*wxGREEN_BRUSH); // scanned

        if(matrix[i][j]->getStatus() == 2)
            dc.SetBrush(*wxRED_BRUSH); // land

        if(matrix[i][j]->getStatus() == 3)
            dc.SetBrush(*wxYELLOW_BRUSH); // probably land

        if(matrix[i][j]->getStatus() == 5)
          dc.SetBrush(*wxGREY_BRUSH); // outside

        dc.SetPen( wxPen( wxColor(255,255,255), 1 ) );
        dc.DrawRectangle( i*dx, windowSize-(j+1)*dy, dx, dy );
      }
    }

    //draw regions
    if(regions == NULL)
      return;

    for(int i=0;i<regions->size();i++)
    {
      PolygonSegment* ps = regions->at(i);

      int s = ps->xPoints.size();
      for(int j=0;j<s;j++)
      {
        double x1 = scale*ps->xPoints[j];
        double y1 = scale*ps->yPoints[j];
        double x2 = scale*ps->xPoints[(j+1) % s];
        double y2 = scale*ps->yPoints[(j+1) % s];

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
