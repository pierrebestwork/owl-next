#include "pch.h"
#include "app.h"
#include "frame.h"

#include "resource.h"

using namespace owl;

TClassesApp::TClassesApp()
  :TApplication(_T("OWLNext examples"))
{
}


TClassesApp::~TClassesApp()
{
}

void TClassesApp::InitMainWindow()
{
  TClassesMDIFrame* frame = new TClassesMDIFrame(0, GetName()); // Use the application name as title.
  SetMainWindow(frame);
  GetMainWindow()->SetMenuDescr(TMenuDescr(IDM_MAINMENU));
}
