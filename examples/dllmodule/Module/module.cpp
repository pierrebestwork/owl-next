//
// Module Sample; module implementation
// This file implements a sample DLL that relies on the OWLNext framework.
//
// Copyright (C) 2012 Vidar Hasfjord 
// Distributed under the OWLNext License (see http://owlnext.sourceforge.net).

#include "module.h"
#include "module.rh"

using namespace owl;

namespace
{
  TSampleModule* ModuleInstance;
}

TSampleModule& TSampleModule::GetInstance()
{
  PRECONDITION(ModuleInstance);
  return *ModuleInstance;
}

TSampleModule::TDialog1::TDialog1(TWindow* parent)
  : TDialog(parent, IDD_SAMPLE, ModuleInstance)
{}

void TSampleModule::TDialog1::SetupWindow()
{
  TDialog::SetupWindow();

#if 1

  // Load a string from this module.
  //
  tstring s = GetModule()->LoadString(IDS_MODULE);
  SetDlgItemText(IDC_STATIC1, s);

#elif 0 // Quicker way to do the same thing:

  // Load a string from this module (using the TWindow::LoadString shorthand).
  //
  tstring s = LoadString(IDS_MODULE);
  SetDlgItemText(IDC_STATIC1, s);

#elif 0 // Even quicker:

  // Load a string from this module (using the SetDlgItemText overload).
  //
  SetDlgItemText(IDC_STATIC1, IDS_MODULE);

#endif

}

TSampleModule::TDialog2::TDialog2(TWindow* parent)
  : TDialog1(parent)
{}

TSampleModule::TDialog2::~TDialog2()
{}

TSampleModule::TDialog2* TSampleModule::CreateDialog2(TWindow* parent)
{return new TDialog2(parent);}

void TSampleModule::DeleteDialog2(TDialog2* p)
{delete p;}

//
// OWL maintains an internal list of modules used to search for resources. Whether the module is
// added to the list is controlled by the third parameter of the TModule constructor.
// Here we choose not to be part of the list, since we do not want client code to inadvertently
// look up a resource in our module. We will control resource loading from this module explicitly.
//
TSampleModule::TSampleModule(THandle handle)
  : TModule(_T("Sample"), handle, false) // addToList = false
{}

TSampleModule::~TSampleModule()
{}

//
// Implementation of the C interface to the module
// No exceptions can be allowed to escape these functions, so we need full exception handling.
//
extern "C" 
{

  SampleModule_TDialog2Handle SampleModule_CreateDialog2(HWND parent)
  {
    try
    {
      TWindow* w = GetWindowPtr(parent);
      SampleModule_TDialog2Handle h = {TSampleModule::GetInstance().CreateDialog2(w)};
      return h;
    }
    catch (...)
    {
      SampleModule_TDialog2Handle h = {0};
      return h;
    }
  }

  int SampleModule_ExecuteDialog2(SampleModule_TDialog2Handle h)
  {
    try
    {
      TSampleModule::TDialog2* p = static_cast<TSampleModule::TDialog2*>(h.p);
      return p->Execute();
    }
    catch (...)
    {
      return -1;
    }
  }

  void SampleModule_DeleteDialog2(SampleModule_TDialog2Handle h)
  {
    try
    {
      TSampleModule::TDialog2* p = static_cast<TSampleModule::TDialog2*>(h.p);
      TSampleModule::GetInstance().DeleteDialog2(p);
    }
    catch (...)
    {}
  }

}

//
// DLL entry point; manages the lifetime of the module singleton.
//
BOOL WINAPI DllMain(HINSTANCE handle, DWORD reason, LPVOID)
{
  try
  {
    switch (reason)
    {
    case DLL_PROCESS_ATTACH:
      ModuleInstance = new TSampleModule(handle);
      break;

    case DLL_PROCESS_DETACH:
      delete ModuleInstance;
      ModuleInstance = 0;
      break;
    }
    return TRUE;
  }
  catch (...)
  {
    return FALSE;
  }
}
