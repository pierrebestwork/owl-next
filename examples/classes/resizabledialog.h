#pragma once

#include <owl/dialog.h>
#include <owl/layoutwi.h>

class TResizableDialog : public owl::TDialog, virtual public owl::TLayoutWindow
{
public:
  enum AnchorTypeX { EdgeLeft, EdgeRight, PercentWidth };
  enum AnchorTypeY { EdgeTop, EdgeBottom, PercentHeight };

  TResizableDialog(owl::TWindow *parent, owl::TResId resId, owl::TModule *module = 0);
  virtual ~TResizableDialog();

  void SetAppName(const owl::tstring& name) { appName = name; }

protected:
  //void SetupWindow();

  virtual void SetupLayout() = 0;

  void Anchor(owl::TWindow& child, AnchorTypeX anchorX, AnchorTypeY anchorY, AnchorTypeX anchorW, AnchorTypeY anchorH);

  void AnchorRight(owl::TWindow& child);
  void AnchorRightBottom(owl::TWindow& child);
  void AnchorLeftRight(owl::TWindow& child);
  void AnchorTopBottom(owl::TWindow& child);
  void AnchorBottom(owl::TWindow& child);
  void AnchorAll(owl::TWindow& child);

  void Anchor(int resId, AnchorTypeX anchorX, AnchorTypeY anchorY, AnchorTypeX anchorW, AnchorTypeY anchorH);

  void AnchorRight(int resId);
  void AnchorRightBottom(int resId);
  void AnchorLeftRight(int resId);
  void AnchorTopBottom(int resId);
  void AnchorBottom(int resId);
  void AnchorAll(int resId);

  void EvSize(owl::uint sizeType, const owl::TSize& size);
  void EvGetMinMaxInfo(MINMAXINFO & minmaxinfo);
  void EvWindowPosChanged(const WINDOWPOS& windowPos);

  bool EvInitDialog(THandle /*hFocus*/);

private:
  void InitializeLayout();

  bool layoutInitialized;
  int minWidth, minHeight;

  owl::tstring appName;
  owl::tstring key;

  owl::TWindow * GetOrCreateControlWithId(int resId);

  DECLARE_RESPONSE_TABLE(TResizableDialog);

};

