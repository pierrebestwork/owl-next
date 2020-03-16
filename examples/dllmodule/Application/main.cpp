//
// Module Sample; application
// This file demonstrates use of a sample DLL that relies on the OWLNext framework.
//
// Copyright (C) 2012 Vidar Hasfjord 
// Distributed under the OWLNext License (see http://owlnext.sourceforge.net).

#include "../Module/module.h"
#include <owl/applicat.h>
#include <owl/framewin.h>
#include <owl/dialog.h>
#include "main.rh"
#include "../Module/module.rh"

using namespace owl;

class TMainDialog
  : public TDialog
{
public:

  TMainDialog::TMainDialog()
    : TDialog(0, IDD_MAIN)
  {}

protected:

  virtual void SetupWindow()
  {
    // Load a string from the application module.
    //
    tstring a = GetModule()->LoadString(IDS_MAIN);
    SetDlgItemText(IDC_MAIN, a);

    // Load a string from the sample DLL module.
    //
    tstring s = TSampleModule::GetInstance().LoadString(IDS_MODULE);
    SetDlgItemText(IDC_MODULE, s);
  }

  //
  // Demonstrate various use cases for the sample module dialogs.
  //
  void BnButtonClicked()
  {
    static int c = 0;
    switch (c++)
    {
    case 0: // Use TSampleModule::TDialog1; simple creation and use.
      {
        try
        {
          TSampleModule::TDialog1(this).Execute();
        }
        catch (...)
        {
          MessageBox(_T("Module dialog execution failed."), _T("Error"), MB_ICONERROR | MB_OK);
        }
        break;
      }

    case 1: // Use TSampleModule::TDialog2; controlled construction and destruction.
      {
        // Note: With C++11 or Boost we could use unique_ptr with a custom deleter here.
        //
        TSampleModule::TDialog2* p = 0;
        try
        {
          p = TSampleModule::GetInstance().CreateDialog2(this);
          CHECK(p);
          p->Execute();
        }
        catch (...)
        {
          MessageBox(_T("Module dialog execution failed."), _T("Error"), MB_ICONERROR | MB_OK);
        }
        if (p) TSampleModule::GetInstance().DeleteDialog2(p);
        break;
      }

    case 2: // Use the sample module's C interface to TSampleModule::TDialog2.
      {
        SampleModule_TDialog2Handle h = SampleModule_CreateDialog2(GetHandle());
        if (!h.p)
        {
          MessageBox(_T("Module dialog creation failed."), _T("Error"), MB_ICONERROR | MB_OK);
          return;
        }
        int r = SampleModule_ExecuteDialog2(h);
        if (r == -1)
        {
          MessageBox(_T("Module dialog execution failed."), _T("Error"), MB_ICONERROR | MB_OK);
          SampleModule_DeleteDialog2(h);
          return;
        }
        SampleModule_DeleteDialog2(h);
        break;
      }

    default:
      {
        MessageBox(_T("All test cases have been run; starting over."));
        c = 0;
        break;
      }

    } // switch
  }

  DECLARE_RESPONSE_TABLE(TMainDialog);
};

DEFINE_RESPONSE_TABLE1(TMainDialog, TDialog)
  EV_BN_CLICKED(IDC_OPEN_MODULE_DIALOG, BnButtonClicked),
END_RESPONSE_TABLE;

class TMyDlgApp 
  : public TApplication
{
public:

  TMyDlgApp() 
    : TApplication(_T("DLL Module Sample")) 
  {}

  void InitMainWindow()
  {
    TWindow* client = new TMainDialog;
    MainWindow = new TFrameWindow(0, GetName(), client, true);
    MainWindow->SetIcon(this, IDI_MAIN);
    MainWindow->ModifyStyle(WS_MAXIMIZEBOX | WS_SIZEBOX, 0);
  }
};

int OwlMain(int, char* [])
{
  return TMyDlgApp().Run();
}
