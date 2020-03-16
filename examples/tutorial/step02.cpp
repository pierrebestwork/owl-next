//----------------------------------------------------------------------------
// ObjectWindows - (C) Copyright 1991, 1994 by Borland International
//   Tutorial application -- step02.cpp
//----------------------------------------------------------------------------
#include <owl/pch.h>
#include <owl/applicat.h>
#include <owl/framewin.h>

using namespace owl;

class TDrawWindow : public TWindow {
  public:
    TDrawWindow(TWindow* parent = 0);

  protected:
    // Override member function of TWindow
    bool CanClose();

    // Message response functions
    void EvLButtonDown(uint, const TPoint&);
    void EvRButtonDown(uint, const TPoint&);

  DECLARE_RESPONSE_TABLE(TDrawWindow);
};

DEFINE_RESPONSE_TABLE1(TDrawWindow, TWindow)
  EV_WM_LBUTTONDOWN,
  EV_WM_RBUTTONDOWN,
END_RESPONSE_TABLE;

TDrawWindow::TDrawWindow(TWindow* parent)
{
  Init(parent, 0, 0);
}

bool
TDrawWindow::CanClose()
{
  return MessageBox(_T("Do you want to save?"), _T("Drawing has changed"),
                    MB_YESNO | MB_ICONQUESTION) == IDNO;
}

void
TDrawWindow::EvLButtonDown(uint, const TPoint&)
{
  MessageBox(_T("You have pressed the left mouse button"),
    _T("Message Dispatched"), MB_OK);
}

void
TDrawWindow::EvRButtonDown(uint, const TPoint&)
{
  MessageBox(_T("You have pressed the right mouse button"),
    _T("Message Dispatched"), MB_OK);
}

class TDrawApp : public TApplication {
  public:
    TDrawApp() : TApplication() {}

    void InitMainWindow()
    {
      SetMainWindow(new TFrameWindow(0, _T("Sample ObjectWindows Program"),
        new TDrawWindow));
    }
};

int
OwlMain(int /*argc*/, _TCHAR* /*argv*/ [])
{
  return TDrawApp().Run();
}
