#pragma once

#include <owl/applicat.h>

class TClassesApp : public owl::TApplication
{
public:
  TClassesApp();
  ~TClassesApp();

protected:
  virtual void InitMainWindow(); // TApplication override

};

