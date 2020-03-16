//---------------------------------------------------------------------------
//  Project VCL2OWL
//
//  (C) 2007 OWLNext
//
//  SUBSYSTEM:  VLC Application with OWL Dialog example
//  FILE:       OWLUnit.h
//  AUTHOR:     Sebastian Ledesma based on Kent Reisdorph info 
//
//  OVERVIEW
//  ~~~~~~~~
//  Interface for OWL dialog calling.
//
//----------------------------------------------------------------------------

#ifndef OWLUnitH
#define OWLUnitH

#include <tchar.h>

// Remember to keep transfer buffers byte packed!

#include <pshpack1.h>
struct TOWLDlgXfer{
  _TCHAR name[32];
  _TCHAR lastName[32];
};
#include <poppack.h>

extern "C" {

bool CallOWLDialog(HINSTANCE hInstance, HWND hWndParent, TOWLDlgXfer *xferBuffer);

}

#endif
//---------------------------------------------------------------------------
