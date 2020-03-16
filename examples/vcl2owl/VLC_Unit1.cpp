//---------------------------------------------------------------------------

#include <stdio.h>
#include <vcl.h>
#pragma hdrstop

#include "VLC_Unit1.h"
#include "OWLUnit.h"

//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"

TForm1 *Form1;
//---------------------------------------------------------------------------
__fastcall TForm1::TForm1(TComponent* Owner)
  : TForm(Owner)
{
}
//---------------------------------------------------------------------------
void __fastcall TForm1::Button1Click(TObject *Sender) {
  TOWLDlgXfer xferBuffer;
  memset(&xferBuffer, 0, sizeof(TOWLDlgXfer));
  _tcscpy(xferBuffer.name, _T("Who are"));
  _tcscpy(xferBuffer.lastName, _T("You?"));

  if (CallOWLDialog(MainInstance, Handle, &xferBuffer)) {
    _TCHAR szBuffer[256];
    wsprintf (szBuffer, _T("Hello %s %s!"),xferBuffer.name, xferBuffer.lastName);
    UnicodeString str(szBuffer);
    Label1->SetTextBuf(str.c_str());
  }
}
//---------------------------------------------------------------------------
void __fastcall TForm1::Button2Click(TObject *Sender) {
  Close();  
}
//---------------------------------------------------------------------------


