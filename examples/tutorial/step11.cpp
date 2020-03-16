//----------------------------------------------------------------------------
// ObjectWindows - (C) Copyright 1991, 1994 by Borland International
//   Tutorial application -- step11.cpp
//----------------------------------------------------------------------------
#include <owl/pch.h>
#include <owl/applicat.h>
#include <owl/decmdifr.h>
#include <owl/mdi.h>
#include <owl/mdichild.h>
#include <owl/dc.h>
#include <owl/inputdia.h>
#include <owl/opensave.h>
#include <owl/controlb.h>
#include <owl/buttonga.h>
#include <owl/statusba.h>
#include <owl/gdiobjec.h>
#include <owl/chooseco.h>
#include <classlib/arrays.h>
#include <stdlib.h>
#include <string.h>
#include "step11.rc"

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

class TDrawMDIClient : public TMDIClient {
  public:
    TDrawMDIClient() : TMDIClient()
    {
      NewChildNum = 0;
      FileData    = 0;
    }

    TMDIChild* InitChild();

    TOpenSaveDialog::TData* GetFileData()
    {
      return FileData ? new TOpenSaveDialog::TData(*FileData) : 0;
    }

  protected:
    void CmFileNew();
    void CmFileOpen();
    void CmAbout();

  private:
    uint16 NewChildNum;
    TOpenSaveDialog::TData *FileData;

  DECLARE_RESPONSE_TABLE(TDrawMDIClient);
};

DEFINE_RESPONSE_TABLE1(TDrawMDIClient, TMDIClient)
  EV_COMMAND(CM_FILENEW, CmFileNew),
  EV_COMMAND(CM_FILEOPEN, CmFileOpen),
  EV_COMMAND(CM_ABOUT, CmAbout),
END_RESPONSE_TABLE;

// TDrawMDIChild must be declared now, because TDrawMDIClient functions
// call TDrawMDIChild's constructor.

class TDrawMDIChild : public TMDIChild {
  public:
    TDrawMDIChild(TDrawMDIClient& parent, LPCTSTR title = 0);
   ~TDrawMDIChild()
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
    void CmFileSave();
    void CmFileSaveAs();
    void CmPenSize();
    void CmPenColor();
    void SaveFile();
    void OpenFile();

  DECLARE_RESPONSE_TABLE(TDrawMDIChild);
};

DEFINE_RESPONSE_TABLE1(TDrawMDIChild, TMDIChild)
  EV_WM_LBUTTONDOWN,
  EV_WM_RBUTTONDOWN,
  EV_WM_MOUSEMOVE,
  EV_WM_LBUTTONUP,
  EV_COMMAND(CM_FILESAVE, CmFileSave),
  EV_COMMAND(CM_FILESAVEAS, CmFileSaveAs),
  EV_COMMAND(CM_PENSIZE, CmPenSize),
  EV_COMMAND(CM_PENCOLOR, CmPenColor),
END_RESPONSE_TABLE;

// Define TDrawMDIClient functions.

TMDIChild*
TDrawMDIClient::InitChild()
{
  _TCHAR title[15];
  if(!FileData)
    wsprintf(title, _T("New drawing %d"), NewChildNum);

  TMDIChild* child = new TDrawMDIChild(*this, FileData ? FileData->FileName : title);
  child->SetIcon(GetApplication(), TResId(_T("IDI_TUTORIAL")));
  return child;
}

void
TDrawMDIClient::CmFileNew()
{
  FileData = 0;
  NewChildNum++;
  CreateChild();
}

void
TDrawMDIClient::CmFileOpen()
{
  // Create FileData.
  FileData = new TOpenSaveDialog::TData(OFN_HIDEREADONLY|OFN_FILEMUSTEXIST,
                                        _T("Point Files (*.PTS)|*.pts|"), 0, _T(""),
                                        _T("PTS"));
  // As long as the file open operation goes OK...
  if ((TFileOpenDialog(this, *FileData)).Execute() == IDOK)
    // Create the child window.
    CreateChild();

  // FileData is no longer needed.
  FileData = 0;
}

void
TDrawMDIClient::CmAbout()
{
  TDialog(this, IDD_ABOUT).Execute();
}

// Define TDrawMDIChild functions.

TDrawMDIChild::TDrawMDIChild(TDrawMDIClient& parent, LPCTSTR title) :
  TMDIChild(parent, title)
{
  DragDC  = 0;
  Lines   = new TLines(5, 0, 5);
  Line    = new TLine(TColor::Black, 1);
  IsDirty = false;

  // If the parent returns a valid FileData member, this is an open operation
  if(parent.GetFileData()) {
    // Not a new file
    IsNewFile = false;
    // Copy the parent's FileData member, since that'll go away
    FileData = parent.GetFileData();
    OpenFile();
  }
  // But if the parent returns 0, this is a new operation
  else {
    // This is a new file
    IsNewFile = true;
    // Create a new FileData member
    FileData = new TOpenSaveDialog::TData(OFN_HIDEREADONLY|OFN_FILEMUSTEXIST,
                                          _T("Point Files (*.PTS)|*.pts|"), 0, _T(""),
                                          _T("PTS"));
  }
}

bool
TDrawMDIChild::CanClose()
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
TDrawMDIChild::EvLButtonDown(uint, const TPoint& point)
{
  if (!DragDC) {
    SetCapture();
    DragDC = new TClientDC(*this);
    Pen    = new TPen(Line->QueryColor(), Line->QueryPenSize());
    DragDC->SelectObject(*Pen);
    DragDC->MoveTo(point);
    Line->Add(point);
    IsDirty = true;
  }
}

void
TDrawMDIChild::EvRButtonDown(uint, const TPoint&)
{
  GetPenSize();
}

void
TDrawMDIChild::EvMouseMove(uint, const TPoint& point)
{
  if (DragDC) {
    DragDC->LineTo(point);
    Line->Add(point);
  }
}

void
TDrawMDIChild::EvLButtonUp(uint, const TPoint&)
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
TDrawMDIChild::CmPenSize()
{
  GetPenSize();
}

void
TDrawMDIChild::CmPenColor()
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
TDrawMDIChild::GetPenSize()
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
TDrawMDIChild::Paint(TDC& dc, bool, TRect&)
{
  // Iterates through the array of line objects.
  TLinesIterator i(*Lines);

  while (i)
    i++.Draw(dc);
}

void
TDrawMDIChild::CmFileSave()
{
  if (IsNewFile)
    CmFileSaveAs();
  else
    SaveFile();
}

void
TDrawMDIChild::CmFileSaveAs()
{
  if (IsNewFile)
    _tcscpy(FileData->FileName, _T(""));

  if ((TFileSaveDialog(this, *FileData)).Execute() == IDOK)
    SaveFile();

  SetCaption(FileData->FileName);
}

void
TDrawMDIChild::SaveFile()
{
  tofstream os(FileData->FileName);

  if (!os)
    MessageBox(_T("Unable to open file"), _T("File Error"), MB_OK | MB_ICONEXCLAMATION);
  else {
    // Write the number of lines in the figure
    os << Lines->GetItemsInContainer();

    // Append a description using a resource string
    os << ' ' << GetApplication()->LoadString(IDS_FILEINFO) << '\n';

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
TDrawMDIChild::OpenFile()
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

    void InitMainWindow();
};

void
TDrawApp::InitMainWindow()
{
  // Create a decorated MDI frame
  TDecoratedMDIFrame* frame = new TDecoratedMDIFrame(_T("Drawing Pad"), TResId(_T("COMMANDS")), *new TDrawMDIClient, true);

  // Construct a status bar
  TStatusBar* sb = new TStatusBar(frame, TGadget::Recessed);

  // Construct a control bar
  TControlBar* cb = new TControlBar(frame);
#if OWLVersion >= 0x600
  cb->EnableFlatStyle();
#endif
  cb->Insert(*new TButtonGadget(CM_FILENEW, CM_FILENEW, TButtonGadget::Command));
  cb->Insert(*new TButtonGadget(CM_FILEOPEN, CM_FILEOPEN, TButtonGadget::Command));
  cb->Insert(*new TButtonGadget(CM_FILESAVE, CM_FILESAVE, TButtonGadget::Command));
  cb->Insert(*new TButtonGadget(CM_FILESAVEAS, CM_FILESAVEAS, TButtonGadget::Command));
  cb->Insert(*new TSeparatorGadget);
  cb->Insert(*new TButtonGadget(CM_PENSIZE, CM_PENSIZE, TButtonGadget::Command));
  cb->Insert(*new TButtonGadget(CM_PENCOLOR, CM_PENCOLOR, TButtonGadget::Command));
  cb->Insert(*new TSeparatorGadget);
  cb->Insert(*new TButtonGadget(CM_ABOUT, CM_ABOUT, TButtonGadget::Command));

  // Insert the status bar and control bar into the frame
  frame->Insert(*sb, TDecoratedFrame::Bottom);
  frame->Insert(*cb, TDecoratedFrame::Top);

  // Set the main window and its menu
  SetMainWindow(frame);
}

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
  os << ' ' << (COLORREF)line.Color << ' ' << line.PenSize;

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

