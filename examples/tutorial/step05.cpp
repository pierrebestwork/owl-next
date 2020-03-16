//----------------------------------------------------------------------------
// ObjectWindows - (C) Copyright 1991, 1994 by Borland International
//   Tutorial application -- step05.cpp
//----------------------------------------------------------------------------
#include <owl/pch.h>
#include <owl/applicat.h>
#include <owl/framewin.h>
#include <owl/dc.h>
#include <owl/inputdia.h>
#include <stdlib.h>

using namespace owl;

class TDrawWindow : public TWindow {
  public:
    TDrawWindow(TWindow* parent = 0);
   ~TDrawWindow()
    {
      delete DragDC;
      delete Pen;
    }

    void SetPenSize(int newSize);

  protected:
    TDC* DragDC;
    int PenSize;
    TPen* Pen;

    // Override member function of TWindow
    bool CanClose();

    // Message response functions
    void EvLButtonDown(uint, const TPoint&);
    void EvRButtonDown(uint, const TPoint&);
    void EvMouseMove(uint, const TPoint&);
    void EvLButtonUp(uint, const TPoint&);

  DECLARE_RESPONSE_TABLE(TDrawWindow);
};

DEFINE_RESPONSE_TABLE1(TDrawWindow, TWindow)
  EV_WM_LBUTTONDOWN,
  EV_WM_RBUTTONDOWN,
  EV_WM_MOUSEMOVE,
  EV_WM_LBUTTONUP,
END_RESPONSE_TABLE;

TDrawWindow::TDrawWindow(TWindow* parent)
{
  Init(parent, 0, 0);
  DragDC  = 0;
  PenSize = 1;
  Pen     = new TPen(TColor::Black, PenSize);
}

bool
TDrawWindow::CanClose()
{
  return MessageBox(_T("Do you want to save?"), _T("Drawing has changed"),
                    MB_YESNO | MB_ICONQUESTION) == IDNO;
}

void
TDrawWindow::EvLButtonDown(uint, const TPoint& point)
{
  Invalidate();

  if (!DragDC) {
    SetCapture();
    DragDC = new TClientDC(*this);
    DragDC->SelectObject(*Pen);
    DragDC->MoveTo(point);
  }
}

void
TDrawWindow::EvRButtonDown(uint, const TPoint&)
{
  _TCHAR inputText[6];

  wsprintf(inputText, _T("%d"), PenSize);
  if ((TInputDialog(this, _T("Line Thickness"),
                        _T("Input a new thickness:"),
                        inputText,
                        sizeof(inputText) / sizeof(_TCHAR))).Execute() == IDOK) {
    int newPenSize = _ttoi(inputText);

    if (newPenSize < 0)
      newPenSize = 1;

    SetPenSize(newPenSize);
  }
}

void
TDrawWindow::EvMouseMove(uint, const TPoint& point)
{
  if (DragDC)
    DragDC->LineTo(point);
}

void
TDrawWindow::EvLButtonUp(uint, const TPoint&)
{
  if (DragDC) {
    ReleaseCapture();
    delete DragDC;
    DragDC = 0;
  }
}

void
TDrawWindow::SetPenSize(int newSize)
{
  delete Pen;
  PenSize = newSize;
  Pen     = new TPen(TColor::Black, PenSize);
}

class TDrawApp : public TApplication {
  public:
    TDrawApp() : TApplication() {}

    void InitMainWindow()
    {
      SetMainWindow(new TFrameWindow(0, _T("Drawing Pad"), new TDrawWindow));
    }
};

int
OwlMain(int /*argc*/, _TCHAR* /*argv*/ [])
{
  return TDrawApp().Run();
}
