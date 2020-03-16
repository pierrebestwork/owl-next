#pragma once
#include <owl/edit.h>

class TCpuInfoWindow : public owl::TEdit
{
public:
  TCpuInfoWindow();

protected:
  owl::TFont Font;
  enum { RefreshTimer = 1 };

  virtual void SetupWindow(); // override

  void EvTimer(owl::uint timer);

  DECLARE_RESPONSE_TABLE(TCpuInfoWindow);
};
