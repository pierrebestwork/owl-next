//----------------------------------------------------------------------------
// ObjectWindows - (C) Copyright 1994 by Borland International
//   Tutorial application -- step16.cpp
//   Embed/Linking/Automation Server example
//----------------------------------------------------------------------------
#include <owl/pch.h>
#include <owl/applicat.h>
#include <owl/appdict.h>
#include <owl/dialog.h>
#include <owl/controlb.h>
#include <owl/buttonga.h>
#include <owl/listbox.h>
#include <owl/statusba.h>
#include <owl/docmanag.h>
#if OWLVersion >= 0x600
#include <ocf/olemdifr.h>
#include <ocf/oledoc.h>
#include <ocf/oleview.h>
#else
#include <owl/olemdifr.h>
#include <owl/oledoc.h>
#include <owl/oleview.h>
#endif
#include <classlib/arrays.h>
#include <ocf/automacr.h>
#include <stdlib.h>
#include <string.h>

using namespace owl;
using namespace ocf;

#include "step16.rc"
#include "step16dv.h"
#include "step16.h"

DEFINE_APP_DICTIONARY(AppDictionary);
static TPointer<TRegistrar> Registrar;

// Registration
//
REGISTRATION_FORMAT_BUFFER(100)

BEGIN_REGISTRATION(AppReg)
  REGDATA(clsid,      "{5E4BD330-8ABC-101B-A23B-CE4E85D07ED2}")
  REGDATA(progid,     "DrawPad.Application.16")
  REGDATA(description,"DrawPad AutoServer Application")
  REGDATA(appname,    "DrawPad AutoServer")
//REGDATA(debugger,   "tdw")
  REGDATA(cmdline,    "/automation")
  REGDATA(usage,      ocrMultipleUse)
END_REGISTRATION

//
// TDrawApp
//
DEFINE_RESPONSE_TABLE1(TDrawApp, TApplication)
  EV_OWLVIEW(dnCreate, EvNewView),
  EV_OWLVIEW(dnClose,  EvCloseView),
  EV_WM_DROPFILES,
  EV_COMMAND(CM_ABOUT, CmAbout),
END_RESPONSE_TABLE;

DEFINE_AUTOAGGREGATE(TDrawApp, OcApp->Aggregate)
  EXPOSE_PROPRW(Visible, TAutoBool,    "Visible",     "Main window shown", 0)
  EXPOSE_METHOD(NewDoc,  TDrawDocument,"NewDocument", "Create new document", 0)
  EXPOSE_METHOD(OpenDoc, TDrawDocument,"OpenDocument","Open existing document", 0)
   REQUIRED_ARG(         TAutoString,  "Name")
  EXPOSE_PROPRO(AppName, TAutoString,  "Name",        "Application name", 0)
  EXPOSE_PROPRO(FullName,TAutoString,  "FullName",    "Complete path to application", 0)
  EXPOSE_APPLICATION(TDrawApp,         "Application", "Application object", 0)
  EXPOSE_QUIT(                         "Quit",        "Shutdown application", 0)
END_AUTOAGGREGATE(TDrawApp,tfAppObject|tfCanCreate,"TDrawApp","Application class", 0)


DEFINE_AUTOCLASS(TDrawDocument)
  EXPOSE_PROPRW(PenSize,    TAutoShort, "PenSize",    "Current pen size", 0)
  EXPOSE_PROPRW(PenColor,   TAutoLong,  "PenColor",   "Current pen color", 0)
  EXPOSE_METHOD(AddPoint,   TAutoVoid,  "AddPoint",   "Add a point to the current line", 0)
   REQUIRED_ARG(            TAutoShort, "X")
   REQUIRED_ARG(            TAutoShort, "Y")
  EXPOSE_METHOD(AddLine,    TAutoVoid,  "AddLine",    "Add current line into drawing", 0)
  EXPOSE_METHOD(ClearLine,  TAutoVoid,  "ClearLine",  "Erases current line", 0)
  EXPOSE_APPLICATION(       TDrawApp,   "Application","Application object", 0)
END_AUTOCLASS(TDrawDocument, tfNormal,  "TDrawDoc",   "Draw document class", 0)


const char far*
TDrawApp::GetPath()
{
  static char buf[_MAX_PATH];
  GetModuleFileName(buf, sizeof(buf)-1);
  return buf;
}

bool
TDrawApp::GetShow()
{
  TFrameWindow* frame = GetMainWindow();
  return (frame && frame->IsWindow() && frame->IsWindowVisible());
}

void
TDrawApp::SetShow(bool visible)
{
  TFrameWindow* frame = GetMainWindow();
  if (frame && frame->IsWindow()) {
    unsigned flags = visible ? SWP_NOACTIVATE|SWP_NOSIZE|SWP_NOMOVE|SWP_SHOWWINDOW
                : SWP_NOACTIVATE|SWP_NOSIZE|SWP_NOMOVE|SWP_NOZORDER|SWP_HIDEWINDOW;
    frame->SetWindowPos(HWND_TOP, 0,0,0,0, flags);
  }
}

BEGIN_REGISTRATION(DocReg)
  REGDATA(progid,     "DrawPad.Drawing.16")
  REGDATA(description,"DrawPad (Step16--AutoServer) Drawing")
  REGDATA(menuname,   "Drawing")
  REGDATA(extension,  "p16")
  REGDATA(docfilter,  "*.p16")
  REGDOCFLAGS(dtAutoOpen | dtAutoDelete | dtUpdateDir | dtCreatePrompt | dtRegisterExt)
//REGDATA(debugger,   "tdw")
  REGDATA(insertable, "")
  REGDATA(verb0,      "&Edit")
  REGDATA(verb1,      "&Open")
  REGFORMAT(0, ocrEmbedSource,  ocrContent,  ocrIStorage,  ocrSet)
  REGFORMAT(1, ocrMetafilePict, ocrContent,  ocrMfPict|ocrStaticMed, ocrGet)
END_REGISTRATION
BEGIN_REGISTRATION(ListReg)
  REGDATA(description,"Line List")
  REGDATA(extension,  "p16")
  REGDATA(docfilter,  "*.p16")
  REGDOCFLAGS(dtAutoDelete | dtHidden)
END_REGISTRATION

DEFINE_DOC_TEMPLATE_CLASS(TDrawDocument, TDrawView,       DrawTemplate);
DEFINE_DOC_TEMPLATE_CLASS(TDrawDocument, TDrawListView,   DrawListTemplate);
DrawTemplate drawTpl(DocReg);
DrawListTemplate drawListTpl(ListReg);


//extern TDocTemplate drawTpl;

TDrawDocument*
TDrawApp::OpenDoc(const char far* name)
{
  long flags = name ? 0 : dtNewDoc;
  TDocManager* docManager = GetDocManager();
  if (!docManager)
    return 0;
  HWND hWnd = ::GetFocus();
  TDocument* doc = GetDocManager()->CreateDoc(&drawTpl, name, 0, flags);
  ::SetFocus(hWnd);
  return dynamic_cast<TDrawDocument*>(doc);
}

/*
#define RETARG(t) void Return(TAutoVal& v){v=Val;} t Val;
#define AUTOFUNC0(name, func, ret, defs) \
  AUTOFUNC_ARG1(name, Val=This->func();, defs, RETARG(ret))
    AUTOFUNC0 (NewDoc,  OpenDoc, TAutoObject<TDrawDocument>, )
*/

TDrawApp::TDrawApp() :
  TApplication(::AppReg["appname"], owl::Module, &::AppDictionary)
{
  TAutoVal v;

  TAutoObject<TDrawDocument> val;

  v = (TObjectDescriptor)val;

}

void
TDrawApp::InitMainWindow()
{
  TOleMDIFrame* frame;
  frame = new TOleMDIFrame(GetName(), 0, *(Client = new TMDIClient(this)), true, this);

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
  SetDocManager(new TDocManager(dmMDI, this));
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
    char* filePath = new char [fileLength];
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
  TOleView* ov = TYPESAFE_DOWNCAST(&view, TOleView);
  if (view.GetDocument().IsEmbedded() && ov && !ov->IsOpenEditing()) {
    TWindow* vw = view.GetWindow();
    vw->SetParent(TYPESAFE_DOWNCAST(GetMainWindow(), TOleFrame)->GetRemViewBucket());
    vw->Create();
  }
  else {
    TMDIChild* child = new TMDIChild(*Client, 0);
    if (view.GetViewMenu())
      child->SetMenuDescr(*view.GetViewMenu());
    child->Create();
    child->SetClientWindow(view.GetWindow());
  }

  if (!ov || !ov->GetOcRemView())
    OcApp->SetOption(amEmbedding, false);
}

void
TDrawApp::EvCloseView(TView& /*view*/)
{
  // nothing needs to be done here for MDI
}

void
TDrawApp::CmAbout()
{
  TDialog(&TWindow(::GetFocus(), this), IDD_ABOUT).Execute();
}

int
OwlMain(int /*argc*/, char* /*argv*/ [])
{
  try {
    Registrar = new TOcRegistrar(AppReg, TOleDocViewAutoFactory<TDrawApp>(),
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
