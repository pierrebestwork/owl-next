#include "pch.h"
#include "glyphbtndlg.h"

#include "resource.h"

#include <owl/glyphbtn.h>

using namespace owl;

#if defined(_OWLDLL)
#define USE_OWLEXTDLL
#else
#define USE_OWLEXTLIB
#endif
#include <owlext/bmpmenu.h>

class TMenuButton : public TButton
{
public:
  TMenuButton(TWindow* parent, int id, LPCTSTR text, int X, int Y, int W, int H, bool isDefault = false, TModule* module = 0)
    : TButton(parent, id, text, X, Y, W, H, isDefault, module) {}
  TMenuButton(TWindow* parent, int id, const tstring& text, int X, int Y, int W, int H, bool isDefault = false, TModule* module = 0)
    : TButton(parent, id, text, X, Y, W, H, isDefault, module) {}
  TMenuButton(TWindow* parent, int resourceId, TModule* module = 0) 
    : TButton(parent, resourceId, module) {}
  TMenuButton(THandle hWnd, TModule* module = 0)
    : TButton(hWnd, module) {}

protected:
  void SetupWindow()
  {
    TButton::SetupWindow();

    bmpmenu.SetMenuFont(new TFont(_T("MS Sans Serif"), 20));
  }

  void BNClicked()
  {
    TRect rect = GetWindowRect();

    ShowPopupMenu(rect.Left(), rect.Bottom());
  }

  void EvContextMenu(HWND, int x, int y)
  {
    ShowPopupMenu(x, y);
  }

  void ShowPopupMenu(int x, int y)
  {
    TPopupMenu popupmenu;
    popupmenu.AppendMenu(MF_STRING, 1, _T("Item 1"));
    popupmenu.AppendMenu(MF_STRING, 2, _T("Item 2"));
    popupmenu.AppendMenu(MF_STRING, 3, _T("Item 3"));

    bmpmenu.RemapMenu(popupmenu);

    int res = popupmenu.TrackPopupMenu(TPM_RETURNCMD, x, y, 0, GetHandle());
  }

  void EvDrawItem(owl::uint ctrlId, const DRAWITEMSTRUCT & drawInfo)
  {
    if (bmpmenu.DrawItem(const_cast<DRAWITEMSTRUCT &>(drawInfo)))
      return;
    TButton::EvDrawItem(ctrlId, drawInfo);
  }

  void EvMeasureItem(owl::uint ctrlId, MEASUREITEMSTRUCT & measureInfo)
  {
    if (bmpmenu.MeasureItem(measureInfo))
      return;
    TButton::EvMeasureItem(ctrlId, measureInfo);
  }

private:
  OwlExt::TBmpMenu bmpmenu;

  DECLARE_RESPONSE_TABLE(TMenuButton);
};

DEFINE_RESPONSE_TABLE1(TMenuButton, TButton)
  EV_NOTIFY_AT_CHILD(BN_CLICKED, BNClicked),
  EV_WM_CONTEXTMENU,
  EV_WM_DRAWITEM,
  EV_WM_MEASUREITEM,
END_RESPONSE_TABLE;

TGlyphButtonDialog::TGlyphButtonDialog(TWindow* parent)
  : TDialog(parent, TResId(IDD_GLYPHBUTTONDIALOG))
{
  new TGlyphButton(this, IDOK, TGlyphButton::btOk);
  new TGlyphButton(this, IDCANCEL, TGlyphButton::btCancel);
  new TGlyphButton(this, IDYES, TGlyphButton::btYes);
  new TGlyphButton(this, IDNO, TGlyphButton::btNo);
  new TGlyphButton(this, IDABORT, TGlyphButton::btAbort);
  new TGlyphButton(this, IDIGNORE, TGlyphButton::btIgnore);
  new TGlyphButton(this, IDCLOSE, TGlyphButton::btClose);
  new TGlyphButton(this, IDHELP, TGlyphButton::btHelp);
  new TGlyphButton(this, IDC_APPLY, TGlyphButton::btApply);
  new TGlyphButton(this, IDC_REVERT, TGlyphButton::btRevert);
  new TGlyphButton(this, IDC_ADD, TGlyphButton::btAdd);
  new TGlyphButton(this, IDC_DELETE, TGlyphButton::btDelete);
  new TGlyphButton(this, IDC_EDIT, TGlyphButton::btEdit);
  new TGlyphButton(this, IDC_SETUP, TGlyphButton::btSetup);
  new TGlyphButton(this, IDC_BROWSE, TGlyphButton::btBrowse);
  //new TGlyphButton(this, IDC_KEY, TGlyphButton::btKey);
  new TMenuButton(this, IDC_KEY);

  TGlyphButton* btnCustom = new TGlyphButton(this, IDC_CUSTOMGLYPH);
  btnCustom->SetGlyph(TResId(IDB_OWLGLYPH));

}


TGlyphButtonDialog::~TGlyphButtonDialog()
{
}
