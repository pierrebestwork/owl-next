#include <owl/pch.h>

#include "About.h"
#include "App.rh"
#include <owl/static.h>
#include <owl/module.h>
#include <strsafe.h>

using namespace owl;

TAboutDlg::TAboutDlg(TWindow* parent, TModule* module)
  : TDialog(parent, IDD_ABOUT, module)
{}

void TAboutDlg::SetupWindow()
{
  // Get the static text for the value based on VERSIONINFO.
  //
  TStatic* versionCtrl = new TStatic(this, IDC_VERSION, 255);
  TStatic* copyrightCtrl = new TStatic(this, IDC_COPYRIGHT, 255);
  TStatic* debugCtrl = new TStatic(this, IDC_DEBUG, 255);

  TDialog::SetupWindow();

  // Get the product name and product version strings.
  //
  TModuleVersionInfo applVersion(GetModule()->GetHandle());
  tstring prodName = applVersion.GetProductName();
  tstring prodVersion = applVersion.GetProductVersion();

  // IDC_VERSION is the product name and version number, the initial value of IDC_VERSION is
  // the word Version(in whatever language) product name VERSION product version.
  //
  tstring versionLiteral = versionCtrl->GetText();
  tchar b[512];
  StringCbPrintf(b, sizeof(b), _T("%s %s %s"), prodName.c_str(), versionLiteral.c_str(), prodVersion.c_str());
  versionCtrl->SetText(b);

  // Get the legal copyright string.
  //
  tstring copyright = applVersion.GetLegalCopyright();
  copyrightCtrl->SetText(copyright);

  // Only get the SpecialBuild text if the VERSIONINFO resource is there.
  //
  if (applVersion.GetFileFlags() & VS_FF_SPECIALBUILD)
    debugCtrl->SetText(applVersion.GetSpecialBuild());
}
