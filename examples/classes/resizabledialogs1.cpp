#include "pch.h"
#include "resizabledialogs1.h"

#include "resizabledialogs.rh"

using namespace owl;

TSampleResizableDialog1::TSampleResizableDialog1(TWindow *parent, TResId resId, TModule *module)
  : TResizableDialog(parent, resId == TResId(0) ? TResId(IDD_RESIZABLEDIALOG1) : resId, module), TLayoutWindow(parent)
{
  edit1 = new TEdit(this, IDC_EDIT1);
  combo1 = new TComboBox(this, IDC_COMBO1);

  listBox1 = new TListBox(this, IDC_LIST1);
  listView1 = new TListViewCtrl(this, IDC_LIST2);

  button1 = new TButton(this, IDC_BUTTON1);
  button2 = new TButton(this, IDC_BUTTON2);
  button3 = new TButton(this, IDC_BUTTON3);
  button4 = new TButton(this, IDC_BUTTON4);
  button5 = new TButton(this, IDC_BUTTON5);
  button6 = new TButton(this, IDC_BUTTON6);
  button7 = new TButton(this, IDC_BUTTON7);
}



TSampleResizableDialog1::~TSampleResizableDialog1()
{
}

void TSampleResizableDialog1::SetupWindow()
{
  TResizableDialog::SetupWindow();
}

void TSampleResizableDialog1::SetupLayout()
{
  AnchorLeftRight(IDC_GROUPBOX1);
  AnchorLeftRight(*edit1);
  AnchorLeftRight(*combo1);

  AnchorTopBottom(IDC_GROUPBOX2);
  AnchorTopBottom(*listBox1);

  AnchorAll(IDC_GROUPBOX3);
  AnchorAll(*listView1);

  AnchorRight(IDC_GROUPBOX4);
  AnchorRight(*button1);
  AnchorRight(*button2);
  AnchorRight(*button3);

  AnchorBottom(IDC_GROUPBOX5);
  AnchorBottom(*button4);
  AnchorBottom(*button5);

  AnchorRightBottom(IDC_GROUPBOX6);
  AnchorRightBottom(*button6);
  AnchorRightBottom(*button7);
}
