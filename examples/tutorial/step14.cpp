//----------------------------------------------------------------------------
// ObjectWindows - (C) Copyright 1994 by Borland International
//   Tutorial application -- step14.cpp
//   OLE 2 container example
//----------------------------------------------------------------------------
#include <owl/pch.h>
#include <owl/applicat.h>
#include <owl/appdict.h>
#include <owl/dialog.h>
#include <owl/controlb.h>
#include <owl/buttonga.h>
#include <owl/statusba.h>
#include <owl/docmanag.h>
#if OWLVersion >= 0x600
#include <ocf/olemdifr.h>
#include <ocf/oleview.h>
#else
#include <owl/olemdifr.h>
#include <owl/oleview.h>
#endif
#include <stdlib.h>
#include <string.h>
#include "step14.rc"

using namespace owl;
using namespace ocf;

DEFINE_APP_DICTIONARY(AppDictionary);
static TPointer<TRegistrar> Registrar;

// Registration
//
REGISTRATION_FORMAT_BUFFER(100)

BEGIN_REGISTRATION(AppReg)
  REGDATA(clsid,      _T("{5E4BD310-8ABC-101B-A23B-CE4E85D07ED2}"))
  REGDATA(appname,    _T("DrawPad Container"))
END_REGISTRATION

//
// TDrawApp
//
class _USERCLASS TDrawApp : public TApplication, public TOcModule {
  public:
    TDrawApp() : TApplication(::AppReg["appname"]) {}

  protected:
    TMDIClient*      Client;

    // Override methods of TApplication
    void InitInstance();
    void InitMainWindow();

    // Event handlers
    void EvNewView(TView& view);
    void EvCloseView(TView& view);
    void EvDropFiles(TDropInfo dropInfo);
    void CmAbout();

  DECLARE_RESPONSE_TABLE(TDrawApp);
};

DEFINE_RESPONSE_TABLE1(TDrawApp, TApplication)
  EV_OWLVIEW(dnCreate, EvNewView),
  EV_OWLVIEW(dnClose,  EvCloseView),
  EV_WM_DROPFILES,
  EV_COMMAND(CM_ABOUT, CmAbout),
END_RESPONSE_TABLE;

void
TDrawApp::InitMainWindow()
{
  TOleMDIFrame* frame;
  frame = new TOleMDIFrame(GetName(), 0, *(Client = new TMDIClient), true);

  // Construct a status bar
  TStatusBar* sb = new TStatusBar(frame, TGadget::Recessed);

  // Construct a control bar
  TControlBar* cb = new TControlBar(frame);
#if OWLVersion >= 0x600
  cb->EnableFlatStyle();
#endif
  cb->Insert(*new TButtonGadget(CM_FILENEW, CM_FILENEW, TButtonGadget::Command));
  cb->Insert(*new TButtonGadget(CM_FILEOPEN, CM_FILEOPEN, TButtonGadget::Command));
  cb->Insert(*new TButtonGadget(CM_FILESAVE, CM_FILESAVE, TButtonGadget::Command));
  cb->Insert(*new TButtonGadget(CM_FILESAVEAS, CM_FILESAVEAS, TButtonGadget::Command));
  cb->Insert(*new TSeparatorGadget);
  cb->Insert(*new TButtonGadget(CM_PENSIZE, CM_PENSIZE, TButtonGadget::Command));
  cb->Insert(*new TButtonGadget(CM_PENCOLOR, CM_PENCOLOR, TButtonGadget::Command));
  cb->Insert(*new TSeparatorGadget);
  cb->Insert(*new TButtonGadget(CM_ABOUT, CM_ABOUT, TButtonGadget::Command));
  cb->SetHintMode(TGadgetWindow::EnterHints);

  cb->GetWindowAttr().Id = IDW_TOOLBAR;

  // Insert the status bar and control bar into the frame
  frame->Insert(*sb, TDecoratedFrame::Bottom);
  frame->Insert(*cb, TDecoratedFrame::Top);

  // Set the main window and its menu
  SetMainWindow(frame);
  GetMainWindow()->SetMenuDescr(TMenuDescr(IDM_MDICMNDS));

  // Install the document manager
  SetDocManager(new TDocManager(dmMDI | dmMenu, this));
}

void
TDrawApp::InitInstance()
{
  TApplication::InitInstance();
  GetMainWindow()->DragAcceptFiles(true);
}

void
TDrawApp::EvDropFiles(TDropInfo dropInfo)
{
  int fileCount = dropInfo.DragQueryFileCount();
  for (int index = 0; index < fileCount; index++) {
    int fileLength = dropInfo.DragQueryFileNameLen(index)+1;
    _TCHAR* filePath = new _TCHAR[fileLength];
    dropInfo.DragQueryFile(index, filePath, fileLength);
    TDocTemplate* tpl = GetDocManager()->MatchTemplate(filePath);
    GetDocManager()->CreateDoc(tpl, filePath);
    delete filePath;
  }
  dropInfo.DragFinish();
}

void
TDrawApp::EvNewView(TView& view)
{
  TMDIChild* child = new TMDIChild(*Client, 0, view.GetWindow());
  if (view.GetViewMenu())
    child->SetMenuDescr(*view.GetViewMenu());
  child->Create();
}

void
TDrawApp::EvCloseView(TView& /*view*/)
{
  // nothing needs to be done here for MDI
}

void
TDrawApp::CmAbout()
{
  TDialog(GetMainWindow(), IDD_ABOUT).Execute();
}

int
OwlMain(int /*argc*/, _TCHAR* /*argv*/ [])
{
  try {
    Registrar = new TOcRegistrar(AppReg, TOleDocViewFactory<TDrawApp>(),
                                 TApplication::GetCmdLine(), ::DocTemplateStaticHead);
    if (Registrar->IsOptionSet(amAnyRegOption))
      return 0;

    return Registrar->Run();
  }
  catch (const std::exception& x) {
    ::MessageBoxA(0, x.what(), "Exception", MB_OK);
  }
  return -1;
}
