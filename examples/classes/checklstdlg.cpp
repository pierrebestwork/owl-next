#include "pch.h"
#include "checklstdlg.h"
#include "resource.h"

using namespace owl;


const int CheckListId = 100;
const int NumItems = 15;

const int BufSize = 4096;

_TCHAR Buffer[BufSize];

//
// CheckListXWindow constructor
//
TCheckListXDialog::TCheckListXDialog(TWindow* parent)
  :
  TDialog(parent, TResId(IDD_CHECKLISTDIALOG))
{
  Items = new TCheckListItem[NumItems];

  for (int i = 0; i < NumItems; i++) {
    _TCHAR buffer[20];
    wsprintf(buffer, _T("Item %d"), i);
    Items[i].SetText(buffer);
  }
  Items[0].Toggle();
  Items[1].SetIndeterminate();
  Items[2].SetThreeStates(true);

  CheckList = new TCheckList(this, CheckListId, 10, 10, 400, 200, Items, NumItems);
}


//
// CheckListXWindow destructor
//
TCheckListXDialog::~TCheckListXDialog()
{
  delete[] Items;
}


//
// SetupWindow
//
void
TCheckListXDialog::SetupWindow()
{
  TDialog::SetupWindow();
  // put new stuff here
  //

}

//
//
//
bool
TCheckListXDialog::CanClose()
{
  ::Buffer[0] = 0;
  _tcscpy_s(::Buffer, BufSize, _T("You've selected\r\n"));
  for (int i = 0; i < NumItems; i++) {
    if (Items[i].IsChecked() || Items[i].IsIndeterminate()) {
      _tcscat_s(::Buffer, BufSize, Items[i].GetText().c_str());
      if (Items[i].IsIndeterminate())
        _tcscat_s(::Buffer, BufSize, _T(" (ind)"));
      _tcscat_s(::Buffer, BufSize, _T("\r\n"));
    }
  }

  return MessageBox(::Buffer, _T("Okay to close?"), MB_OKCANCEL) == IDOK;
}


//
// CleanupWindow
//
void
TCheckListXDialog::CleanupWindow()
{
  // put new stuff here
  //
  TDialog::CleanupWindow();
}


//
// Response table for CheckListXWindow
//
DEFINE_RESPONSE_TABLE1(TCheckListXDialog, TDialog)
END_RESPONSE_TABLE;
