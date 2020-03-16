//----------------------------------------------------------------------------
// ObjectWindows - (C) Copyright 1991, 1994 by Borland International
//   Tutorial application -- step09.cpp
//----------------------------------------------------------------------------
#include <owl/pch.h>
#include <owl/applicat.h>
#include <owl/framewin.h>
#include <owl/dc.h>
#include <owl/inputdia.h>
#include <owl/opensave.h>
#include <owl/gdiobjec.h>
#include <owl/chooseco.h>
#include <classlib/arrays.h>
#include <stdlib.h>
#include <string.h>
#include "step09.rc"

using namespace std;
using namespace owl;

typedef TArray<TPoint> TPoints;
typedef TArrayIterator<TPoint> TPointsIterator;

class TLine : public TPoints {
  public:
    // Constructor to allow construction from a color and a pen size.
    // Also serves as default constructor.
    TLine(const TColor& color = TColor(), int penSize = 1) :
      TPoints(10, 0, 10), PenSize(penSize), Color(color) {}

    // Functions to modify and query pen attributes.
    int QueryPenSize() const
    {
      return PenSize;
    }

    TColor& QueryColor()
    {
      return Color;
    }

    void SetPen(const TColor& newColor, int penSize = 0);
    void SetPen(int penSize);

    // TLine draws itself.  Returns true if everything went OK.
    virtual bool Draw(TDC&) const;

    // The == operator must be defined for the container class, even if unused
    bool operator ==(const TLine& other) const
    {
      return &other == this;
    }

    friend tostream& operator <<(tostream& os, const TLine& line);
    friend tistream& operator >>(tistream& is, TLine& line);

  protected:
    int PenSize;
    TColor Color;
};

void
TLine::SetPen(int penSize)
{
  if (penSize < 1)
    PenSize = 1;
  else
    PenSize = penSize;
}

void
TLine::SetPen(const TColor& newColor, int penSize)
{
  // If penSize isn't the default (0), set PenSize to the new size.
  if (penSize)
    PenSize = penSize;

  Color = newColor;
}

bool
TLine::Draw(TDC& dc) const
{
  // Set pen for the dc to the values for this line
  TPen pen(Color, PenSize);
  dc.SelectObject(pen);

  // Iterates through the points in the line i.
  TPointsIterator j(*this);
  bool first = true;

  while (j) {
    TPoint p = j++;

    if (!first)
      dc.LineTo(p);
    else {
      dc.MoveTo(p);
      first = false;
    }
  }
  dc.RestorePen();
  return true;
}

typedef TArray<TLine> TLines;
typedef TArrayIterator<TLine> TLinesIterator;

class TDrawWindow : public TWindow {
  public:
    TDrawWindow(TWindow* parent = 0);
   ~TDrawWindow()
    {
      delete DragDC;
      delete Line;
      delete Lines;
      delete FileData;
    }

  protected:
    TDC* DragDC;
    TPen* Pen;
    TLines* Lines;
    TLine* Line; // To hold a single line at a time that later gets
                 // stuck in Lines
    TOpenSaveDialog::TData* FileData;
    bool IsDirty, IsNewFile;

    void GetPenSize(); // GetPenSize always calls Line->SetPen().

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
    void CmPenSize();
    void CmPenColor();
    void CmAbout();
    void SaveFile();
    void OpenFile();

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
  EV_COMMAND(CM_PENSIZE, CmPenSize),
  EV_COMMAND(CM_PENCOLOR, CmPenColor),
  EV_COMMAND(CM_ABOUT, CmAbout),
END_RESPONSE_TABLE;

TDrawWindow::TDrawWindow(TWindow* parent)
{
  Init(parent, 0, 0);
  DragDC    = 0;
  Lines     = new TLines(5, 0, 5);
  Line      = new TLine(TColor::Black, 1);
  IsNewFile = true;
  IsDirty   = false;
  FileData  = new TOpenSaveDialog::TData(OFN_HIDEREADONLY|OFN_FILEMUSTEXIST,
                                        _T("Point Files (*.PTS)|*.pts|"), 0, _T(""),
                                        _T("PTS"));
}

bool
TDrawWindow::CanClose()
{
  if (IsDirty)
    switch(MessageBox(_T("Do you want to save?"), _T("Drawing has changed"),
                      MB_YESNOCANCEL | MB_ICONQUESTION)) {
      case IDCANCEL:
        // Choosing Cancel means to abort the close -- return false.
        return false;

      case IDYES:
        // Choosing Yes means to save the drawing.
        CmFileSave();
    }
  return true;
}

void
TDrawWindow::EvLButtonDown(uint, const TPoint& point)
{
  if (!DragDC) {
    SetCapture();
    DragDC = new TClientDC(*this);
    Pen = new TPen(Line->QueryColor(), Line->QueryPenSize());
    DragDC->SelectObject(*Pen);
    DragDC->MoveTo(point);
    Line->Add(point);
    IsDirty = true;
  }
}

void
TDrawWindow::EvRButtonDown(uint, const TPoint&)
{
  GetPenSize();
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
    Lines->Add(*Line);
    Line->Flush();
    delete DragDC;
    delete Pen;
    DragDC = 0;
  }
}

void
TDrawWindow::CmPenSize()
{
  GetPenSize();
}

void
TDrawWindow::CmPenColor()
{
  TChooseColorDialog::TData colors;
  static TColor custColors[16] =
  {
    COLORREF(0x010101L), COLORREF(0x101010L), COLORREF(0x202020L), COLORREF(0x303030L),
    COLORREF(0x404040L), COLORREF(0x505050L), COLORREF(0x606060L), COLORREF(0x707070L),
    COLORREF(0x808080L), COLORREF(0x909090L), COLORREF(0xA0A0A0L), COLORREF(0xB0B0B0L),
    COLORREF(0xC0C0C0L), COLORREF(0xD0D0D0L), COLORREF(0xE0E0E0L), COLORREF(0xF0F0F0L)
  };

  colors.Flags = CC_RGBINIT;
  colors.Color = TColor(Line->QueryColor());
  colors.CustColors = custColors;
  if (TChooseColorDialog(this, colors).Execute() == IDOK)
    Line->SetPen(colors.Color);
}

void
TDrawWindow::GetPenSize()
{
  _TCHAR inputText[6];
  int penSize = Line->QueryPenSize();

  wsprintf(inputText, _T("%d"), penSize);
  if ((TInputDialog(this, _T("Line Thickness"),
                        _T("Input a new thickness:"),
                        inputText,
                        sizeof(inputText) / sizeof(_TCHAR))).Execute() == IDOK) {
    penSize = _ttoi(inputText);

    if (penSize < 1)
      penSize = 1;
  }
  Line->SetPen(penSize);
}

void
TDrawWindow::Paint(TDC& dc, bool, TRect&)
{
  // Iterates through the array of line objects.
  TLinesIterator i(*Lines);

  while (i)
    i++.Draw(dc);
}

void
TDrawWindow::CmFileNew()
{
  if (CanClose()) {
    Line->Flush();
    Lines->Flush();
    Invalidate();
    IsDirty = false;
    IsNewFile = true;
  }
}

void
TDrawWindow::CmFileOpen()
{
  if (CanClose())
    if ((TFileOpenDialog(this, *FileData)).Execute() == IDOK)
      OpenFile();
}

void
TDrawWindow::CmFileSave()
{
  if (IsNewFile)
    CmFileSaveAs();
  else
    SaveFile();
}

void
TDrawWindow::CmFileSaveAs()
{
  if (IsNewFile)
    _tcscpy(FileData->FileName, _T(""));

  if ((TFileSaveDialog(this, *FileData)).Execute() == IDOK)
    SaveFile();
}

void
TDrawWindow::CmAbout()
{
  TDialog(this, IDD_ABOUT).Execute();
}

void
TDrawWindow::SaveFile()
{
  tofstream os(FileData->FileName);

  if (!os)
    MessageBox(_T("Unable to open file"), _T("File Error"), MB_OK | MB_ICONEXCLAMATION);
  else {
    // Write the number of lines in the figure
    os << Lines->GetItemsInContainer();

    // Append a description using a resource string
    os << _T(' ') << GetApplication()->LoadString(IDS_FILEINFO) << '\n';

    // Get an iterator for the array of lines
    TLinesIterator i(*Lines);

    // While the iterator is valid (i.e. we haven't run out of lines)
    while (i)
      // Copy the current line from the iterator and increment the array.
      os << i++;

    // Set new file and dirty display indicator to false.
    IsNewFile = IsDirty = false;
  }
}

void
TDrawWindow::OpenFile()
{
  tifstream is(FileData->FileName);

  if (!is)
    MessageBox(_T("Unable to open file"), _T("File Error"), MB_OK | MB_ICONEXCLAMATION);
  else {
    unsigned numLines;
    _TCHAR fileinfo[100];

    Lines->Flush();
    Line->Flush();

    is >> numLines;
    is.getline(fileinfo, sizeof(fileinfo)/sizeof(_TCHAR));
    Parent->SetCaption(fileinfo);

    for (int i = 0; i < numLines; i++) {
      TLine line;
      is >> line;
      Lines->Add(line);
    }
  }
  IsNewFile = IsDirty = false;
  Invalidate();
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

tostream&
operator <<(tostream& os, const TLine& line)
{
  // Write the number of points in the line
  os << line.GetItemsInContainer();

  // Get and write pen attributes.
  os << ' '<< (COLORREF)line.Color << ' ' << line.PenSize;

  // Get an iterator for the array of points
  TPointsIterator j(line);

  // While the iterator is valid (i.e. we haven't run out of points)
  while(j)
    // Write the point from the iterator and increment the array.
    os << j++;
  os << '\n';

  // return the stream object
  return os;
}

tistream&
operator >>(tistream& is, TLine& line)
{
  unsigned numPoints;
  is >> numPoints;

  COLORREF color;
  int penSize;
  is >> color >> penSize;
  line.SetPen(TColor(color), penSize);

  while (numPoints--) {
    TPoint point;
    is >> point;
    line.Add(point);
  }

  // return the stream object
  return is;
}
