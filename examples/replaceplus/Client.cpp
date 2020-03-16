#include "pch.h"
#pragma hdrstop

#include "Client.h"
#include "Help.h"
#include "Util.h"
#include "Replacer.h"
#include "App.rh"
#include <owl/memcbox.h>
#include <owl/glyphbtn.h>
#include <owl/tooltip.h>
#include <owl/configfl.h>

using namespace owl;

//-----------------------------------------------------------------------------

DEFINE_RESPONSE_TABLE1(TReplaceClient, TDialog)
  EV_BN_CLICKED(IDC_BROWSE, BnBrowseClicked),
  EV_BN_CLICKED(IDC_ACTIONFIND, BnActionClicked),
  EV_BN_CLICKED(IDC_ACTIONTOUCH, BnActionClicked), 
  EV_BN_CLICKED(IDOK, BnReplaceClicked),
  EV_BN_CLICKED(IDC_HLP, BnHelpClicked),
END_RESPONSE_TABLE;

//-----------------------------------------------------------------------------

TReplaceClient::TReplaceClient(TWindow* parent, TReplaceData& data, const tstring& regKey)
  : TTransferDialog<TReplaceData>(parent, IDD_CLIENT, data)
{
  new TGlyphButton(this, IDOK, TGlyphButton::btOk);
  new TGlyphButton(this, IDCANCEL, TGlyphButton::btCancel);
  new TGlyphButton(this, IDC_BROWSE, TGlyphButton::btBrowse);
  new TGlyphButton(this, IDC_HLP, TGlyphButton::btHelp);

  // Specialize TMemCombox to store settings in the Windows Registry.
  //
  struct TRegComboBox 
    : public TMemComboBox
  {
    tstring RegKey;

    TRegComboBox(TWindow* parent, int resId, const tstring& name, const tstring& regKey)
      : TMemComboBox(parent, resId, name), RegKey(regKey) {}

    virtual TConfigFile* CreateConfigFile() // override
    {return new TRegConfigFile(RegKey);}
  };
  new TRegComboBox(this, IDC_FOLDER, _T("Folder"), regKey);
  new TRegComboBox(this, IDC_FILTER, _T("Filter"), regKey);
  new TRegComboBox(this, IDC_FIND, _T("Find"), regKey);
  new TRegComboBox(this, IDC_REPLACE, _T("Replace"), regKey);
}

//-----------------------------------------------------------------------------

void TReplaceClient::SetupWindow()
{
  TDialog::SetupWindow();

#if OWLVersion < 0x0633

  // Add tooltips to various controls.
  //
  TTooltip& t = *new TTooltip(this);
  t.Create();

  TToolInfo tOk(GetHandle(), GetDlgItem(IDOK), _T("Perform the action"));
  tOk.EnableSubclassing();
  t.AddTool(tOk);

  TToolInfo tCancel(GetHandle(), GetDlgItem(IDCANCEL), _T("Close the program"));
  tCancel.EnableSubclassing();
  t.AddTool(tCancel);

  TToolInfo tBrowse(GetHandle(), GetDlgItem(IDC_BROWSE), _T("Browse for folder"));
  tBrowse.EnableSubclassing();
  t.AddTool(tBrowse);

  TToolInfo tRecurse(GetHandle(), GetDlgItem(IDC_SUBFOLDERS), _T("Recurse subfolders of chosen folder"));
  tRecurse.EnableSubclassing();
  t.AddTool(tRecurse);

  TToolInfo tActionFind(GetHandle(), GetDlgItem(IDC_ACTIONFIND), _T("Find and replace text in files"));
  tActionFind.EnableSubclassing();
  t.AddTool(tActionFind);

  TToolInfo tActionTouch(GetHandle(), GetDlgItem(IDC_ACTIONTOUCH), _T("Change file date/time"));
  tActionTouch.EnableSubclassing();
  t.AddTool(tActionTouch);

  TToolInfo tFolder(GetHandle(), GetDlgItem(IDC_FOLDER), _T("Enter start folder or select from recent list"));
  tFolder.EnableSubclassing();
  t.AddTool(tFolder);

  TToolInfo tFilter(GetHandle(), GetDlgItem(IDC_FILTER), _T("Enter filters like *.cpp;*.c;*.h or select from recent list"));
  tFilter.EnableSubclassing();
  t.AddTool(tFilter);

#else

  // Add tooltips to various controls (simpler method for 6.33 and later).
  //
  TTooltip& t = *TTooltip::Make(this);
  t.AddTool(IDOK, _T("Perform the action"));
  t.AddTool(IDCANCEL, _T("Close the program"));
  t.AddTool(IDC_BROWSE, _T("Browse for folder"));
  t.AddTool(IDC_SUBFOLDERS, _T("Recurse subfolders of chosen folder"));
  t.AddTool(IDC_ACTIONFIND, _T("Find and replace text in files"));
  t.AddTool(IDC_ACTIONTOUCH, _T("Change file date/time"));
  t.AddTool(IDC_FOLDER, _T("Enter start folder or select from recent list"));
  t.AddTool(IDC_FILTER, _T("Enter filters like *.cpp;*.c;*.h or select from recent list"));

#endif

}

//-----------------------------------------------------------------------------

void TReplaceClient::DoTransferData(const TTransferInfo& i, TReplaceData& d)
{
  TransferDlgItemText(i, IDC_FOLDER, d.Folder);
  TransferDlgItemText(i, IDC_FILTER, d.Filter);
  TransferDlgItemText(i, IDC_FIND, d.SearchTerm);
  TransferDlgItemText(i, IDC_REPLACE, d.Replacement);
  TransferCheckBoxData(i, IDC_SUBFOLDERS, d.RecurseFlag);
  TransferRadioButtonData(i, IDC_ACTIONFIND, d.Action);
  TransferDateTimePickerData(i, IDC_EDITTIME, d.Time);
  TransferDateTimePickerData(i, IDC_EDITDATE, d.Date);

  if (i.Operation == tdSetData)
  {
    // Update the control state.
    //
    BnActionClicked();
  }
}

//-----------------------------------------------------------------------------

void TReplaceClient::BnBrowseClicked()
{
  tstring folder = GetDlgItemText(IDC_FOLDER);
  folder = BrowseForFolder(GetHandle(), folder, _T("Select root folder"));
  if (!folder.empty())
    SetDlgItemText(IDC_FOLDER, folder);
}

//-----------------------------------------------------------------------------

void TReplaceClient::BnActionClicked()
{
  const int idFind[] = {IDC_ST_FIND, IDC_FIND, IDC_REPLACE, IDC_ST_REPLACE};
  const int idTouch[] = {IDC_ST_TIME, IDC_EDITTIME, IDC_EDITDATE};

  bool shouldFind = IsChecked(IDC_ACTIONFIND);
  EnableDlgItem(idFind, shouldFind);
  DisableDlgItem(idTouch, shouldFind);
  ShowDlgItem(idFind, shouldFind);
  HideDlgItem(idTouch, shouldFind);
}

//-----------------------------------------------------------------------------

void TReplaceClient::BnReplaceClicked()
{
  // Validate dialog contents.
  //
  try 
  {
    if (GetDlgItemText(IDC_FOLDER).empty()) throw tstring(_T("Enter folder."));
    if (GetDlgItemText(IDC_FILTER).empty()) throw tstring(_T("Enter filter."));
    if (IsChecked(IDC_ACTIONFIND)) 
    {
      if (GetDlgItemText(IDC_FIND).empty()) throw tstring(_T("Enter search string."));
    }
  }
  catch (const tstring& m)
  {
    MessageBox(m, _T("Validation Error"), MB_ICONINFORMATION);
    return;
  }

  // Call TransferData, which will call DoTransferData, to update the arguments.
  // Then send the command to the parent window; this will be handled by the application.
  // See TReplaceApp::CmReplace.
  //
  TransferData(tdGetData);
  GetParent()->PostMessage(WM_COMMAND, CM_REPLACE);
}

//-----------------------------------------------------------------------------

void TReplaceClient::BnHelpClicked()
{
  ExecuteHelpDialog(this);
}
