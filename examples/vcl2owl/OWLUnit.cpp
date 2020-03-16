//---------------------------------------------------------------------------
//  Project VCL2OWL
//
//  (C) 2007 OWLNext
//
//  SUBSYSTEM:  VLC Application with OWL Dialog example
//  FILE:       OWLUnit.cpp
//  AUTHOR:     Sebastian Ledesma based on Kent Reisdorph info 
//
//  OVERVIEW
//  ~~~~~~~~
//  Definition for OWL dialog calling.
//
//----------------------------------------------------------------------------

//#include <vcl.h> Don´t include VCL, this is a ´OWL´ Unit
//#pragma package(smart_init)

#pragma option -b     //Treat enum as ints (VCL units, dont use this option) 

#include <owl\dialog.h>
#include <owl\edit.h>
#pragma hdrstop

#include "OWLUnit.h"
#include "OWLDlg.rh"

using namespace owl;

//---------------------------------------------------------------------------

class TMyOWLDialog: public TDialog {
  public:
    TMyOWLDialog(TWindow *parent, TResId resId, TModule* module = 0);

  protected:
    TEdit *name;
    TEdit *lastName;

};

TMyOWLDialog::TMyOWLDialog(TWindow *parent, TResId resId, TModule* module)
  : TDialog(parent, resId, module) {
  name=new TEdit(this, IDC_FIRSTNAMEEDIT, 32);
  lastName=new TEdit(this, IDC_LASTNAMEEDIT, 32);
  SetBkgndColor(RGB(255,225,132));
}



//Interface for calling from non-OWL windows  (like a VCL form)
bool CallOWLDialog(HINSTANCE hInstance, HWND hWndParent, TOWLDlgXfer *pXferBuffer) {
  TModule module(_T("VCLModule"), hInstance);
  TWindow vclWindow(hWndParent, &module); //make an alias window
  TDialog* dlg = new TMyOWLDialog(&vclWindow, IDD_DIALOG1);

  if (pXferBuffer) {
  dlg->SetTransferBuffer(pXferBuffer);
  }
  bool result=dlg->Execute()==IDOK;

  delete dlg;
  return result;
}


