//----------------------------------------------------------------------------
// ObjectWindows - (C) Copyright 1991, 1994 by Borland International
//   Tutorial application -- step06.cpp
//----------------------------------------------------------------------------
#include <owl/pch.h>
#include <owl/applicat.h>
#include <owl/framewin.h>
#include <owl/dc.h>
#include <owl/inputdia.h>
#include <classlib/arrays.h>
#include <stdlib.h>
#include "step06.rc"

using namespace owl;

typedef TArray<TPoint> TPoints;
typedef TArrayIterator<TPoint> TPointsIterator;

class TDrawWindow : public TWindow {
  public:
    TDrawWindow(TWindow* parent = 0);
   ~TDrawWindow()
    {
      delete DragDC;
      delete Pen;
      delete Line;
    }

    void SetPenSize(int newSize);

  protected:
    TDC* DragDC;
    int PenSize;
    TPen* Pen;
    TPoints* Line; // To store points in line.

    // Override member function of TWindow
    bool CanClose();

    // Message response functions
    void EvLButtonDown(uint, const TPoint&);
    void EvRButtonDown(uint, const TPoint&);
    void EvMouseMove(uint, const TPoint&);
    void EvLButtonUp(uint, const TPoint&);
    void Paint(TDC&, bool, TRect&);
    void CmFileNew();
    void CmFileOpen();
    void CmFileSave();
    void CmFileSaveAs();
    void CmAbout();

  DECLARE_RESPONSE_TABLE(TDrawWindow);
};

DEFINE_RESPONSE_TABLE1(TDrawWindow, TWindow)
  EV_WM_LBUTTONDOWN,
  EV_WM_RBUTTONDOWN,
  EV_WM_MOUSEMOVE,
  EV_WM_LBUTTONUP,
  EV_COMMAND(CM_FILENEW, CmFileNew),
  EV_COMMAND(CM_FILEOPEN, CmFileOpen),
  EV_COMMAND(CM_FILESAVE, CmFileSave),
  EV_COMMAND(CM_FILESAVEAS, CmFileSaveAs),
  EV_COMMAND(CM_ABOUT, CmAbout),
END_RESPONSE_TABLE;

TDrawWindow::TDrawWindow(TWindow* parent)
{
  Init(parent, 0, 0);
  DragDC  = 0;
  PenSize = 1;
  Pen     = new TPen(TColor::Black, PenSize);
  Line    = new TPoints(10, 0, 10);
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
  Line->Flush();
  Invalidate();

  if (!DragDC) {
    SetCapture();
    DragDC = new TClientDC(*this);
    DragDC->SelectObject(*Pen);
    DragDC->MoveTo(point);
    Line->Add(point);
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
  if (DragDC) {
    DragDC->LineTo(point);
    Line->Add(point);
  }
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

void
TDrawWindow::Paint(TDC& dc, bool, TRect&)
{
  bool first = true;
  TPointsIterator i(*Line);

  dc.SelectObject(*Pen);

  while (i) {
    TPoint p = i++;

    if (!first)
      dc.LineTo(p);
    else {
      dc.MoveTo(p);
      first = false;
    }
  }
}

void
TDrawWindow::CmFileNew()
{
  Line->Flush();
  Invalidate();
}

void
TDrawWindow::CmFileOpen()
{
  MessageBox(_T("Feature not implemented"), _T("File Open"), MB_OK);
}

void
TDrawWindow::CmFileSave()
{
  MessageBox(_T("Feature not implemented"), _T("File Save"), MB_OK);
}

void
TDrawWindow::CmFileSaveAs()
{
  MessageBox(_T("Feature not implemented"), _T("File Save As"), MB_OK);
}

void
TDrawWindow::CmAbout()
{
  MessageBox(_T("Feature not implemented"), _T("About Drawing Pad"), MB_OK);
}

class TDrawApp : public TApplication {
  public:
    TDrawApp() : TApplication() {}

    void InitMainWindow()
    {
      SetMainWindow(new TFrameWindow(0, _T("Drawing Pad"), new TDrawWindow));
      GetMainWindow()->AssignMenu(_T("COMMANDS"));
    }
};

int
OwlMain(int /*argc*/, _TCHAR* /*argv*/ [])
{
  return TDrawApp().Run();
}
