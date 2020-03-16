#include "pch.h"
#include "cpuinfownd.h"

#include <owl/system.h>
#include <owl/gdiobjec.h>

using namespace owl;
using namespace std;

DEFINE_RESPONSE_TABLE1(TCpuInfoWindow, TEdit)
EV_WM_TIMER,
END_RESPONSE_TABLE;


tstring GetCpuInfo()
{
  const TSystem::TProcessorInfo& i = TSystem::GetProcessorInfo();
  tstring a = TSystem::GetProcessorArchitectureName(TSystem::GetProcessorArchitecture());
  tstring v = i.GetVendorName(i.GetVendorId());
  tostringstream s;
  s << boolalpha
    << _T("Architecture: ") << (a.empty() ? _T("(unknown)") : a.c_str()) << _T("\r\n")
    << _T("Number of processors: ") << TSystem::GetNumberOfProcessors() << _T("\r\n")
    << _T("\r\n")
    << _T("Name: ") << i.GetName() << _T("\r\n")
    << _T("Vendor ID: ") << i.GetVendorId() << _T("\r\n")
    << _T("Vendor name: ") << (v.empty() ? _T("(unknown)") : v.c_str()) << _T("\r\n")
    << _T("\r\n")
    << _T("Model: ") << i.GetModel() << _T("\r\n")
    << _T("Ext Model: ") << i.GetExtModel() << _T("\r\n")
    << _T("Family: ") << i.GetFamily() << _T("\r\n")
    << _T("Ext Family: ") << i.GetExtFamily() << _T("\r\n")
    << _T("Type: ") << i.GetType() << _T("\r\n")
    << _T("Stepping: ") << i.GetStepping() << _T("\r\n")
    << _T("Nominal frequency: ") << i.GetNominalFrequency() << _T(" MHz \r\n")
    << _T("Current frequency: ") << i.GetCurrentFrequency() << _T(" MHz \r\n")
    << _T("\r\n")
    << _T("Has MMX: ") << i.HasMmx() << _T("\r\n")
    << _T("Has MMXExt: ") << i.HasMmxExt() << _T("\r\n")
    << _T("Has 3DNow: ") << i.Has3dNow() << _T("\r\n")
    << _T("Has 3DNowExt: ") << i.Has3dNowExt() << _T("\r\n")
    << _T("Has SSE: ") << i.HasSse() << _T("\r\n")
    << _T("Has SSE2: ") << i.HasSse2() << _T("\r\n")
    << _T("Has SSE3: ") << i.HasSse3() << _T("\r\n")
    << _T("Has HTT: ") << i.HasHtt() << _T("\r\n");
  return s.str();
}


TCpuInfoWindow::TCpuInfoWindow()
  : TEdit(0, 1, 0, 0, 0, 0, 0),
  Font(_T("Microsoft Sans Serif"), -12)
{
  ModifyStyle(0, ES_READONLY | ES_MULTILINE);
  SetWindowPos(NULL, 0, 0, 350, 400, SWP_NOMOVE);
}

void TCpuInfoWindow::SetupWindow() // override
{
  TEdit::SetupWindow();
  SetText(GetCpuInfo());
  SetWindowFont(Font, false);
  SetTimer(RefreshTimer, 5000);
}

void TCpuInfoWindow::EvTimer(uint timer)
{
  if (timer == RefreshTimer)
  {
    TRange r = GetSelection();
    SetText(GetCpuInfo());
    SetSelection(r);
  }
}
