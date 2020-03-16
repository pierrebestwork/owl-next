#pragma once
#include <owl/dialog.h>
#include <owl/checklst.h>

class TCheckListXDialog : public owl::TDialog {
public:
  TCheckListXDialog(owl::TWindow* parent);
  ~TCheckListXDialog();

  void  SetupWindow();
  void  CleanupWindow();

  bool  CanClose();

private:
  owl::TCheckList* CheckList;
  owl::TCheckListItem* Items;

  DECLARE_RESPONSE_TABLE(TCheckListXDialog);
};