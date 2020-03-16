#include "pch.h"
#include "resizabledialog.h"


#include <owl/configfl.h>

using namespace owl;

DEFINE_RESPONSE_TABLE2(TResizableDialog, TDialog, TLayoutWindow)
  EV_WM_SIZE,
  EV_WM_GETMINMAXINFO,
  EV_WM_WINDOWPOSCHANGED,
END_RESPONSE_TABLE;

TResizableDialog::TResizableDialog(owl::TWindow *parent, owl::TResId resId, owl::TModule *module)
  : TDialog(parent, resId, module), TLayoutWindow(parent), layoutInitialized(false), minWidth(0), minHeight(0)
{
}


TResizableDialog::~TResizableDialog()
{
}


void TResizableDialog::InitializeLayout()
{
  TRect rect = GetWindowRect();
  if (rect.Width() > 0 && rect.Height() > 0)
  {
    if (!layoutInitialized)
	{
      minWidth = rect.Width();
      minHeight = rect.Height();
      SetupLayout();
      layoutInitialized = true;
      Layout();
      Invalidate(true);
    }
  }
}

bool TResizableDialog::EvInitDialog(THandle hFocus)
{
  bool res = TDialog::EvInitDialog(hFocus);

  if (appName.length() > 0)
  {
    TRegConfigFile config(appName);
    int w = config.ReadInteger(_T("UI"), key + _T("_W"), 0);
    int h = config.ReadInteger(_T("UI"), key + _T("_H"), 0);
    int x = config.ReadInteger(_T("UI"), key + _T("_X"), 0);
    int y = config.ReadInteger(_T("UI"), key + _T("_Y"), 0);

    if (w > 0 && h > 0)
    {
      InitializeLayout();

      MoveWindow(x, y, w, h, true);

      ClientSize = GetClientRect().Size();

      Layout();
    }
  }

  return res;
}

void TResizableDialog::EvWindowPosChanged(const WINDOWPOS& windowPos)
{
  TDialog::EvWindowPosChanged(windowPos);
  TLayoutWindow::EvWindowPosChanged(windowPos);

  if (appName.length() > 0)
  {
    if (layoutInitialized && windowPos.cx > 0 && windowPos.cy > 0)
    {
      TRegConfigFile config(appName);
      config.WriteInteger(_T("UI"), key + _T("_X"), windowPos.x);
      config.WriteInteger(_T("UI"), key + _T("_Y"), windowPos.y);
      config.WriteInteger(_T("UI"), key + _T("_W"), windowPos.cx);
      config.WriteInteger(_T("UI"), key + _T("_H"), windowPos.cy);
    }
  }
}

void TResizableDialog::EvSize(uint sizeType, const TSize& size)
{
  if (size.cx > 0 && size.cy > 0)
  {
    InitializeLayout();
  }

  TDialog::EvSize(sizeType, size);
  TLayoutWindow::EvSize(sizeType, size);
}

void TResizableDialog::EvGetMinMaxInfo(MINMAXINFO & minmaxinfo)
{
  TDialog::EvGetMinMaxInfo(minmaxinfo);

  if (layoutInitialized)
  {
    minmaxinfo.ptMinTrackSize.x = minWidth;
    minmaxinfo.ptMinTrackSize.y = minHeight;
  }
}

void TResizableDialog::Anchor(TWindow& child, AnchorTypeX anchorX, AnchorTypeY anchorY, AnchorTypeX anchorW, AnchorTypeY anchorH)
{
  TRect rect = GetClientRect();
  TRect rectChild = GetChildRect(child);

  TLayoutMetrics layoutMetrics;
  layoutMetrics.SetMeasurementUnits(TMeasurementUnits::lmPixels);

  switch (anchorX)
  {
  case PercentWidth: layoutMetrics.X.Set(lmLeft, lmPercentOf, lmParent, lmRight, rectChild.left * 100 / rect.right); break;
  case EdgeRight: layoutMetrics.X.Set(lmRight, lmLeftOf, lmParent, lmRight, rect.right - rectChild.right); break;
  }

  switch (anchorY)
  {
  case PercentHeight: layoutMetrics.Y.Set(lmTop, lmPercentOf, lmParent, lmBottom, rectChild.top * 100 / rect.bottom); break;
  case EdgeBottom: layoutMetrics.Y.Set(lmBottom, lmAbove, lmParent, lmBottom, rect.bottom - rectChild.bottom); break;
  }

  switch (anchorW)
  {
  case PercentWidth: layoutMetrics.Width.Set(lmRight, lmPercentOf, lmParent, lmRight, rectChild.right * 100 / rect.right); break;
  case EdgeRight: layoutMetrics.Width.Set(lmRight, lmLeftOf, lmParent, lmRight, rect.right - rectChild.right); break;
  }

  switch (anchorH)
  {
  case PercentHeight: layoutMetrics.Height.Set(lmBottom, lmPercentOf, lmParent, lmBottom, rectChild.bottom * 100 / rect.bottom); break;
  case EdgeBottom: layoutMetrics.Height.Set(lmBottom, lmAbove, lmParent, lmBottom, rect.bottom - rectChild.bottom); break;
  }

  SetChildLayoutMetrics(child, layoutMetrics);
}


void TResizableDialog::AnchorRight(TWindow& child)
{
  Anchor(child, EdgeRight, EdgeTop, EdgeLeft, EdgeTop);
}

void TResizableDialog::AnchorRightBottom(TWindow& child)
{
  Anchor(child, EdgeRight, EdgeBottom, EdgeLeft, EdgeTop);
}

void TResizableDialog::AnchorBottom(TWindow& child)
{
  Anchor(child, EdgeLeft, EdgeBottom, EdgeLeft, EdgeTop);
}

void TResizableDialog::AnchorAll(TWindow& child)
{
  Anchor(child, EdgeLeft, EdgeTop, EdgeRight, EdgeBottom);
}

void TResizableDialog::AnchorLeftRight(TWindow& child)
{
  Anchor(child, EdgeLeft, EdgeTop, EdgeRight, EdgeTop);
}

void TResizableDialog::AnchorTopBottom(TWindow& child)
{
  Anchor(child, EdgeLeft, EdgeTop, EdgeLeft, EdgeBottom);
}

void TResizableDialog::Anchor(int resId, AnchorTypeX anchorX, AnchorTypeY anchorY, AnchorTypeX anchorW, AnchorTypeY anchorH)
{
  Anchor(*GetOrCreateControlWithId(resId), anchorX, anchorY, anchorW, anchorH);
}


void TResizableDialog::AnchorRight(int resId)
{
  AnchorRight(*GetOrCreateControlWithId(resId));
}

void TResizableDialog::AnchorRightBottom(int resId)
{
  AnchorRightBottom(*GetOrCreateControlWithId(resId));
}

void TResizableDialog::AnchorBottom(int resId)
{
  AnchorBottom(*GetOrCreateControlWithId(resId));
}

void TResizableDialog::AnchorLeftRight(int resId)
{
  AnchorLeftRight(*GetOrCreateControlWithId(resId));
}

void TResizableDialog::AnchorAll(int resId)
{
  AnchorAll(*GetOrCreateControlWithId(resId));
}

void TResizableDialog::AnchorTopBottom(int resId)
{
  AnchorTopBottom(*GetOrCreateControlWithId(resId));
}

TWindow * TResizableDialog::GetOrCreateControlWithId(int resId)
{
  TWindow * wnd = ChildWithId(resId);
  if (wnd == NULL)
  {
    wnd = new TControl(this, resId);
    wnd->Create();
  }

  return wnd;
}


