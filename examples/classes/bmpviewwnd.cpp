#include "pch.h"
#include "bmpviewwnd.h"

#include "resource.h"
#include <all_idds.rh>

#include <owl/scroller.h>
#include <owl/inputdia.h>
#include <owl/opensave.h>

#include <stdio.h>
#include <tchar.h>

using namespace owl;

DEFINE_RESPONSE_TABLE2(TBmpViewWindow, TClipboardViewer, TWindow)
EV_COMMAND(CM_FILEOPEN, CmFileOpen),
EV_COMMAND(CM_READ1, CmRead1),
EV_COMMAND(CM_READ2, CmRead2),
EV_COMMAND(CM_READBAD, CmReadBad),
EV_COMMAND(CM_MEMHANDLE, CmMemHandle),
EV_COMMAND(CM_MEMHANDLEREFRESH, CmMemHandleRefresh),
EV_COMMAND(CM_EDITCOPY, CmCopy),
EV_COMMAND(CM_EDITPASTE, CmPaste),
EV_COMMAND(CM_FIT, CmFit),
EV_COMMAND(CM_AUTOCLIPVIEW, CmAutoClipView),
EV_COMMAND(CM_PAINTFROMDDB, CmPaintFromDDB),
EV_COMMAND(CM_PAINTICON, CmPaintIcon),
EV_COMMAND_ENABLE(CM_EDITCOPY, CeCopy),
EV_COMMAND_ENABLE(CM_EDITPASTE, CePaste),
EV_COMMAND_ENABLE(CM_FIT, CeFit),
EV_COMMAND_ENABLE(CM_AUTOCLIPVIEW, CeAutoClipView),
EV_COMMAND_ENABLE(CM_PAINTFROMDDB, CePaintFromDDB),
EV_COMMAND_ENABLE(CM_PAINTICON, CePaintIcon),
EV_WM_SIZE,
EV_WM_PALETTECHANGED,
EV_WM_QUERYNEWPALETTE,
EV_WM_DRAWCLIPBOARD,
EV_WM_DESTROY,
END_RESPONSE_TABLE;

//
// Constructor for a TBmpViewWindow, sets scroll styles and constructs
// the Scroller object.  Also sets the Rop based on whether the display
// is monochrome (two-color) or polychrome.
//
TBmpViewWindow::TBmpViewWindow()
  :
  TWindow(0, 0, 0),
  TClipboardViewer()
{

  Attr.Style |= WS_BORDER | WS_VSCROLL | WS_HSCROLL;
  Attr.ExStyle |= WS_EX_CLIENTEDGE;

  BkgndBrush = new TBrush(TColor::SysWindow);
  Scroller = new TScroller(this, 1, 1, 200, 200);

  Fit = false;
  AutoClipView = false;
  PaintFromDDB = true;
  PaintIcon = false;

  Dib = 0;
  Bitmap = 0;
  MemHandle = 0;
  Palette = 0;
  PixelWidth = 0;
  PixelHeight = 0;


  // If the screen is a palettized device, then we'll need a halftone palette
  // for dibs with > 8 bpp
  //
  TScreenDC screenDC;
  if (screenDC.GetDeviceCaps(RASTERCAPS) & RC_PALETTE) {
    ScreenPalettized = true;
    DefPalette = new THalftonePalette;
  }
  else {
    ScreenPalettized = false;
    DefPalette = 0;
  }

  // Raster op to use when blitting to the screen
  //
  Rop = SRCCOPY;

  SetCaption(0);
  SetBkgndColor(TColor::Transparent);  // Don't pre-erase image background
}

//
//
//
TBmpViewWindow::~TBmpViewWindow()
{
  delete Bitmap;
  if (Palette != DefPalette)
    delete Palette;
  delete DefPalette;
  delete Dib;
  delete BkgndBrush;
}

//
// Build up a caption based on a filename, and set it into the title.
//
void
TBmpViewWindow::SetCaption(LPCTSTR name)
{
  FileName = name ? name : _T("untitled");

  if (Parent) {
    tstring caption = FileName + _T(" - ") + GetApplication()->GetName();
    Parent->SetCaption(caption.c_str());
  }
}

//
// Make a metafile & put it on the clipboard.
// Make a copy of each of the objects & place the copies on the clipboard
//
void
TBmpViewWindow::CmCopy()
{
  TClipboard clipboard(GetHandle());

  if (clipboard.EmptyClipboard()) {
    TMetaFileDC mfDC;
    mfDC.SetWindowExt(Dib->Size());
    mfDC.SetWindowOrg(TPoint(0, 0));
    mfDC.SetDIBitsToDevice(TRect(0, 0, PixelWidth, PixelHeight), TPoint(0, 0), *Dib);
    TMetaFilePict mf(mfDC.Close(), AutoDelete);
    mf.ToClipboard(clipboard, MM_TEXT, TSize(PixelWidth, PixelHeight));

    if (Palette)
      TPalette(*Palette).ToClipboard(clipboard);
    TDib(*Dib).ToClipboard(clipboard);
    if (Bitmap)
      TBitmap(*Bitmap).ToClipboard(clipboard);
  }
}

void
TBmpViewWindow::CeCopy(TCommandEnabler& ce)
{
  ce.Enable(Dib != 0);
}

//
// When a user selects edit.paste, get the data from the clipboard. This
// routine prefers CF_META over CF_DIB over CF_BITMAP
//
void
TBmpViewWindow::CmPaste()
{
  TClipboard clipboard(GetHandle());
  if (!clipboard)
    return;

  TPalette* newPal = 0;
  TDib*     newDib = 0;
  TBitmap*  newBitmap;

  // If there is a palette on the clipboard, get it & realize it
  //
  if (clipboard.IsClipboardFormatAvailable(CF_PALETTE)) {
    newPal = new TPalette(TPalette(clipboard));  // make a copy
    UpdatePalette(false);
  }

  // try DIB format 1st
  //
  if (clipboard.IsClipboardFormatAvailable(CF_DIB)) {
    newDib = new TDib(TDib(clipboard));        // make a copy
    if (!newPal && !newDib->GetColors())
      newPal = new TPalette((HPALETTE)::GetStockObject(DEFAULT_PALETTE));
    newBitmap = new TBitmap(*newDib, newPal);  // newPal==0 is OK
  }
  // try metafile 2nd
  //
  else if (clipboard.IsClipboardFormatAvailable(CF_METAFILEPICT)) {
    if (!newPal)
      newPal = new TPalette((HPALETTE)GetStockObject(DEFAULT_PALETTE));
    newBitmap = new TBitmap(TMetaFilePict(clipboard), *newPal,
      GetClientRect().Size());
  }
  // try bitmap (DDB) format 3rd
  //
  else if (clipboard.IsClipboardFormatAvailable(CF_BITMAP)) {
    newBitmap = new TBitmap(TBitmap(clipboard));     // make a copy
  }
  else
    return;  // Should never happen--this command is enabled by above tests

             // Got a bitmap & maybe more. Now keep it & setup things.
             //
  delete Bitmap;
  Bitmap = newBitmap;

  // Make a DIB from the bitmap if we didn't get one yet.
  //
  if (!newDib)
    newDib = new TDib(*newBitmap, newPal);
  delete Dib;
  Dib = newDib;

  // Either use the palette obtained above, or construct one from the
  // color table in the DIB
  //
  try {
    if (!newPal)
      newPal = new TPalette(*newDib);
  }
  catch (...) {
    newPal = new TPalette(::CreateHalftonePalette(TScreenDC()), AutoDelete);
    //    newPal = new TPalette((HPALETTE)::GetStockObject(DEFAULT_PALETTE));
  }
  if (Palette != DefPalette)
    delete Palette;
  Palette = newPal;
  Palette->GetObject(Colors);

  PixelWidth = Dib->Width();
  PixelHeight = Dib->Height();
  AdjustScroller();
  SetCaption(_T("clipboard"));

  // We'll paint the icon from the bitmap, or use the icon in the resource
  //
  if (PaintIcon)
    GetApplication()->GetMainWindow()->SetIcon(0, 0);
  else
    GetApplication()->GetMainWindow()->SetIcon(GetApplication(), GetApplication()->GetName());
}

//
//
//
void
TBmpViewWindow::CePaste(TCommandEnabler& ce)
{
  TClipboard clipboard(GetHandle());
  ce.Enable(
    clipboard && (
      clipboard.IsClipboardFormatAvailable(CF_METAFILEPICT) ||
      clipboard.IsClipboardFormatAvailable(CF_DIB) ||
      clipboard.IsClipboardFormatAvailable(CF_BITMAP)
      )
  );
}

//
// If either of the "Read bitmap" menu items is selected, then we read
// the bitmap resource with the ID of the menu item...
//
void
TBmpViewWindow::CmRead1()
{
  if (LoadBitmapResource(CM_READ1))
    SetCaption(_T("(Bitmap resource 1)"));
}

//
//
//
void
TBmpViewWindow::CmRead2()
{
  if (LoadBitmapResource(CM_READ2))
    SetCaption(_T("(Bitmap resource 2)"));
}

void
TBmpViewWindow::CmReadBad()
{
  LoadBitmapFile(_T(""));   // force a failure for kicks
}

//
//
//
void
TBmpViewWindow::CmMemHandle()
{
  _TCHAR buf[32];
  wsprintf(buf, _T("%0X"), MemHandle);
  if (TInputDialog(this, _T("Memory Bitmap"), _T("Enter handle"), buf, sizeof(buf) / sizeof(_TCHAR)).Execute() == IDOK) {
	HBITMAP handle;
	_tscanf_s(buf, _T("%x"), &handle);
	LoadMemHandle(handle);
  }
}

//
// Reload the bitmaps from the user supplied bitmap handle
//
void
TBmpViewWindow::CmMemHandleRefresh()
{
  if (MemHandle)
    LoadMemHandle(MemHandle);
}

//
// Toggle Fit member variable & adjust scroller as needed
//
void
TBmpViewWindow::CmFit()
{
  Fit = !Fit;
  AdjustScroller();
}

//
// The fit menu item is checked if the Fit member is true
//
void
TBmpViewWindow::CeFit(TCommandEnabler& ce)
{
  ce.SetCheck(Fit ? TCommandEnabler::Checked : TCommandEnabler::Unchecked);
}

//
// Toggle AutoAutoClipView member variable
//
void
TBmpViewWindow::CmAutoClipView()
{
  AutoClipView = !AutoClipView;
}

//
// Check AutoClipView according to flag
//
void
TBmpViewWindow::CeAutoClipView(TCommandEnabler& ce)
{
  ce.SetCheck(AutoClipView ? TCommandEnabler::Checked : TCommandEnabler::Unchecked);
}

//
// Toggle PaintFromDDB member variable
//
void
TBmpViewWindow::CmPaintFromDDB()
{
  PaintFromDDB = !PaintFromDDB;
  Invalidate();
}

//
// Check AutoClipView according to flag
//
void
TBmpViewWindow::CePaintFromDDB(TCommandEnabler& ce)
{
  ce.SetCheck(PaintFromDDB ? TCommandEnabler::Checked : TCommandEnabler::Unchecked);
}

//
// Toggle PaintIcon member variable
//
void
TBmpViewWindow::CmPaintIcon()
{
  PaintIcon = !PaintIcon;
}

//
// Check AutoClipView according to flag
//
void
TBmpViewWindow::CePaintIcon(TCommandEnabler& ce)
{
  ce.SetCheck(PaintIcon ? TCommandEnabler::Checked : TCommandEnabler::Unchecked);
}

//
// If the the "Open..." menu item is selected, then we prompt the user
// for a new bitmap file.  If the user selects one and it is one that
// we can read, we display it in the window and change the window's
// caption to reflect the new bitmap file.
//
void
TBmpViewWindow::CmFileOpen()
{
  static TOpenSaveDialog::TData data(
    OFN_HIDEREADONLY | OFN_FILEMUSTEXIST | OFN_NOREADONLYRETURN,
    _T("Bitmap Files (*.BMP)|*.bmp|"),
    NULL,
    NULL,
    NULL
  );
  if (TFileOpenDialog(this, data).Execute() == IDOK)
  {
    _TCHAR fileTitle[MAX_PATH];
    TOpenSaveDialog::GetFileTitle(data.FileName, fileTitle, MAX_PATH);
	LoadBitmapFile(data.FileName);
#if defined(__BORLANDC__)
	_tcslwr(fileTitle);
#else
	_tcslwr_s(fileTitle);
#endif
    SetCaption(fileTitle);
  }
}

//
// Adjust the Scroller range so that the the origin is the
// upper-most scrollable point and the corner is the
// bottom-most.
//
void
TBmpViewWindow::AdjustScroller()
{
  static bool inAdjust = false;
  if (inAdjust)
    return;

  inAdjust = true;
  TRect clientRect;
  GetClientRect(clientRect);

  // Only show scrollbars when image is larger than
  // the client area and we are not stretching to fit.
  //
  if (Fit) {
    Scroller->SetRange(0, 0);
  }
  else {
    Scroller->SetUnits(1, 1);// unit = 1 pixel
    Scroller->SetRange(PixelWidth, PixelHeight);
  }
  //Scroller->ScrollTo(0, 0);
  if (!GetUpdateRect(clientRect, false))
    Invalidate(false);

  inAdjust = false;
}

//
// Reset scroller range.
//
void
TBmpViewWindow::EvSize(uint SizeType, const TSize& Size)
{
  TWindow::EvSize(SizeType, Size);
  if (SizeType != SIZEICONIC) {
    AdjustScroller();
    Invalidate(false);
  }
}

//
// Somebody changed the palette. If its not us, then we need to update.
//
void
TBmpViewWindow::EvPaletteChanged(HWND hWndPalChg)
{
  if (hWndPalChg != GetHandle())
    UpdatePalette(false);    // pass false to UpdateColors() instead of repaint
}

//
// We need to re-realize the logical palette each time we regain the input
// focus
//
bool
TBmpViewWindow::EvQueryNewPalette()
{
  return UpdatePalette(true);
}

//
//
//
void
TBmpViewWindow::EvDrawClipboard()
{
  if (TClipboardViewer::DoDrawClipboard() == esComplete)
    return;
  if (AutoClipView)
    CmPaste();
}

//
//
//
void
TBmpViewWindow::EvDestroy()
{
  if (TClipboardViewer::DoDestroy() == esComplete)
    return;
  TWindow::EvDestroy();
}

//
//
//
bool
TBmpViewWindow::UpdatePalette(bool alwaysRepaint)
{
  TRACE("Enter UpdatePalette()");
  if (ScreenPalettized && Palette) {
    TClientDC clientDC(GetHandle());
    TRACE("UpdatePalette::Palette: " << std::hex << (UINT_PTR)(HPALETTE)*Palette);
    clientDC.SelectObject(*Palette, true);
    bool needsUpdate = clientDC.RealizePalette() > 0;
    if (alwaysRepaint || needsUpdate)
      Invalidate(false);
    //    else if (needsUpdate)
    //      clientDC.UpdateColors(); // can use this, but our painting is faster
    TRACE("Leave UpdatePalette(true)");
    return true;
  }
  TRACE("Leave UpdatePalette(false)");
  return false;
}

//
// Responds to an incoming Paint message by redrawing the bitmap.
// The Scroller's BeginView method, which sets the viewport origin
// relative to the present scroll position, has been called by TWindow's
// EvPaint
// Note that we Invalidate() ourselves with false to avoid the background
// paint flicker. Thats why we use
//
// The code can use either the Bitmap member with Stretch- and Bit- Blts,
// or use the Dib member with Stretch- and Set- DIBits...
//
void
TBmpViewWindow::Paint(TDC& dc, bool, TRect&)
{
  TRACE("Enter Paint()");
  TRect clientRect = GetClientRect();

  TRACE("Paint::BkBrush: " << std::hex << (UINT_PTR)(HBRUSH)*BkgndBrush);
  dc.SelectObject(*BkgndBrush);

  TMemoryDC memoryDC(dc);  // Get ready to possibly use the DDB

  if (PaintFromDDB && Bitmap) {
    TRACE("Paint::Bitmap: " << std::hex << (UINT_PTR)(HBITMAP)*Bitmap);
    memoryDC.SelectObject(*Bitmap);
    //    dc.SetStretchBltMode(STRETCH_HALFTONE);
    dc.SetStretchBltMode(STRETCH_ORSCANS);
    if (Palette) {
      TRACE("Paint::Palette: " << std::hex << (UINT_PTR)(HPALETTE)*Palette);
      dc.SelectObject(*Palette, false);
      dc.RealizePalette();
      memoryDC.SelectObject(*Palette, false);
      memoryDC.RealizePalette();
    }
    if (Rop == SRCCOPY) {
      if (Colors == 2) {
        if (Palette) {
          PALETTEENTRY pe;
          Palette->GetPaletteEntry(0, pe);
          dc.SetTextColor(TColor(pe));
          Palette->GetPaletteEntry(1, pe);
          dc.SetBkColor(TColor(pe));
        }
        else {
          dc.SetBkColor(TColor(0, 0, 0));
          dc.SetTextColor(TColor(255, 255, 255));
        }
      }
    }
  }

  if ((PaintFromDDB && Bitmap) || (!PaintFromDDB && Dib)) {
    TRect imageRect(0, 0, PixelWidth, PixelHeight);
    if (Parent->IsIconic()) {
      if (PaintFromDDB)
        dc.StretchBlt(Parent->GetClientRect(), memoryDC, imageRect, Rop);
      else
        dc.StretchDIBits(Parent->GetClientRect(), imageRect, *Dib, Rop);
    }
    else {
      clientRect += TPoint((int)Scroller->XPos, (int)Scroller->YPos);
      if (Fit) {
        // Stretch to fit
        //
        if (PaintFromDDB)
          dc.StretchBlt(clientRect, memoryDC, imageRect, Rop);
        else
          dc.StretchDIBits(clientRect, imageRect, *Dib);
      }
      else {
        // Let it clip
        //
        if (PaintFromDDB)
          dc.BitBlt(imageRect, memoryDC, TPoint(0, 0), Rop);
        else
          dc.SetDIBitsToDevice(imageRect, TPoint(0, 0), *Dib);

        // Clear borders here for no flicker
        //
        dc.PatBlt(TRect(TPoint(PixelWidth, 0), clientRect.BottomRight()));
        dc.PatBlt(TRect(TPoint(0, PixelHeight), clientRect.BottomRight()));
      }
    }
  }
  else
    // Nothing to paint
    //
    dc.PatBlt(clientRect, PATCOPY);

  TRACE("Leave Paint()");
}

//
// Setup all of the bitmap related members given a new dib
//
void
TBmpViewWindow::SetupFromDib(TDib* dib)
{
  TRACE("Enter SetupFromDib()");
  TRACE("SetupFromDib::Dib: " << std::hex << (UINT_PTR)dib->GetHandle());

  delete Dib;
  Dib = dib;

  if (Palette != DefPalette)
    delete Palette;
  Palette = 0;
  try {
    Palette = new TPalette(*dib);
  }
  catch (...) {
    //    Palette = new TPalette(::CreateHalftonePalette(TScreenDC()), AutoDelete);
    Palette = DefPalette;
  }

  TRACE("SetupFromDib::Palette: " << std::hex << (UINT_PTR)(HPALETTE)*Palette);

  {
    TWaitCursor w;
    delete Bitmap;
    if (Palette)
      Bitmap = new TBitmap(*Dib, Palette);
    else {
      TWindowDC dc(GetHandle());
      Bitmap = new TBitmap(dc, *Dib);
    }
    TRACE("SetupFromDib::Bitmap: " << std::hex << (UINT_PTR)(HBITMAP)*Bitmap);
  }

  PixelWidth = Dib->Width();
  PixelHeight = Dib->Height();

  UpdatePalette(true);

  AdjustScroller();

  // We'll paint the icon from the bitmap, or use the icon in the resource
  //
  if (PaintIcon)
    GetApplication()->GetMainWindow()->SetIcon(0, 0);
  else
    GetApplication()->GetMainWindow()->SetIcon(GetApplication(), GetApplication()->GetName());

  TRACE("Leave SetupFromDib()");
}

//
// Test if the passed resource is a Windows 3.0 (or PM 1.x) DI bitmap
// and if so read it.
// Report errors if unable to do so. Adjust the Scroller to the new
// bitmap dimensions.
//
bool
TBmpViewWindow::LoadBitmapResource(uint16 resId)
{
  TDib* newDib;
  try {
    newDib = new TDib(*GetModule(), TResId(resId));
  }
  catch (TXGdi) {
    MessageBox(_T("Cannot access bitmap resource"), GetApplication()->GetName(),
      MB_OK);
    return false;
  }
  SetupFromDib(newDib);
  return true;
}

//
// Test if the passed file is a Windows 3.0 DI (or PM 1.x) bitmap
// and if so read it.
// Report errors if unable to do so. Adjust the Scroller to the new
// bitmap dimensions.
//
bool
TBmpViewWindow::LoadBitmapFile(LPCTSTR name)
{
  TDib* newDib;
  try {
    newDib = new TDib(name);
  }
  catch (TXGdi) {
    MessageBox(_T("Cannot open bitmap file"), GetApplication()->GetName(), MB_OK);
    return false;
  }
  SetupFromDib(newDib);
  return true;
}

//
//
//
bool
TBmpViewWindow::LoadMemHandle(HBITMAP handle)
{
#if defined(USE_DDB)
  // Reqest size of info. buffer to validate HBITMAP
  //
  if (::GetObject(handle, 0, 0)) {
    TDib* newDib;
    try {
      newDib = new TDib(TBitmap(handle));
      SetupFromDib(newDib);
      MemHandle = handle;

      _TCHAR buf[20];
      wsprintf(buf, _T("hbm:%0X"), MemHandle);
      SetCaption(buf);
      return true;
    }
    catch (TXGdi) {
      MessageBox(_T("Cannot construct Dib from HBITMAP"),
        GetApplication()->GetName(), MB_OK);
    }
  }
  else
    MessageBox(_T("Not a valid bitmap handle"),
      GetApplication()->GetName(), MB_OK);
#endif
  return false;
}
