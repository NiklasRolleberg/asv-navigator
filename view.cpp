#include <thread>
#include "wx/wx.h"
#include "wx/sizer.h"
#include <vector>
#include "view.hpp"


View::View()
{
  std::cout << "View constructor" << std::endl;
  uiThread = NULL;
  lastPosx = 0;
  lastPosy = 0;
}


View::View(int a1, char** a2)
{
  std::cout << "View constructor 2" << std::endl;
  uiThread = NULL;
  lastPosx = 0;
  lastPosy = 0;
  //argc = a1;
  //argv = a2;

  //TODO save stuff

}

View::~View()
{
  std::cout << "View destructor" << std::endl;
  GetTopWindow()->Destroy();//Close(false);
  frame->Refresh();
  if(uiThread != NULL)
    if(uiThread->joinable())
      uiThread->join();
}

void View::start(int ws, int max)
{
  std::cout << "View start" << std::endl;
  windowSize = ws;
  maxXY = max;
  if(uiThread == NULL)
    uiThread = new std::thread(&View::init,this);
  std::cout << "View: thread started" << std::endl;
  usleep(500000);
}

void View::init()
{
  std::cout << "View init" << std::endl;
  argc = 0;
  argv = new char*[argc];
  wxEntryStart( argc, argv );
  wxTheApp->CallOnInit();
  wxTheApp->OnRun();
}

bool View::OnInit()
{
  std::cout << "View OnInit" << std::endl;
  wxBoxSizer* sizer = new wxBoxSizer(wxHORIZONTAL);
  frame = new wxFrame((wxFrame *)NULL, -1,  wxT("wxframe"), wxPoint(50,50), wxSize(windowSize,windowSize+25));
  drawPane = new BasicDrawPane( (wxFrame*) frame , windowSize);
  sizer->Add(drawPane, 1, wxEXPAND);

  frame->SetSizer(sizer);
  frame->SetAutoLayout(true);

  frame->Show();
  return true;
}

void View::drawPolygon(std::vector<double>* x, std::vector<double>* y)
{
  int m = std::min(x->size(), y->size());
  for(int i=1;i<= m;i++)
  {
    drawLine(x->at(i-1), y->at(i-1), x->at(i%m), y->at(i%m),5, 0,255,0);
  }
}

void View::drawPath(double posx, double posy)
{
  if(lastPosx == 0 && lastPosy == 0)
  {
    lastPosx = posx;
    lastPosy = posy;
    return;
  }

  drawLine((int) lastPosx, (int) lastPosy, (int) posx,(int) posy, 1, 255,0,0);
  lastPosx = posx;
  lastPosy = posy;
}

void View::drawLine(int startx, int starty, int stopx,int stopy,int width, int r, int g , int b)
{
  double scale = (double)windowSize / (double)maxXY;
  //std::cout << "drawing line" << std::endl;
  double x1 = startx*scale;
  double y1 = windowSize - starty*scale;
  double x2 = stopx*scale;
  double y2 = windowSize - stopy*scale;

  drawPane->drawLine((int)x1,(int)y1, (int)x2, (int)y2,width,r,b,g);
}

void View::update()
{
  frame->Refresh();
}

BEGIN_EVENT_TABLE(BasicDrawPane, wxPanel)
// catch paint events
EVT_PAINT(BasicDrawPane::paintEvent)

END_EVENT_TABLE()

//----------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------


BasicDrawPane::BasicDrawPane(wxFrame* parent, int ws) : wxPanel(parent)
{
  windowSize = ws;
  bmp = wxBitmap(windowSize, windowSize, -1);
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
    dc.DrawBitmap(bmp, 0, 0, false);
}
