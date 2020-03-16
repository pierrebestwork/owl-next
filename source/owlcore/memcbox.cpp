//----------------------------------------------------------------------------
//  ObjectWindows, OWL NExt
//  Copyright 1998-1999 by Yura Bidus. All Rights reserved.
//
/// \file
///  Source file for implementation of TMemComboBox (TComboBox).
///  (Generated by OWL 6.x Class Expert for MS VC++, Version 1.5)
//----------------------------------------------------------------------------
#include <owl/pch.h>

#include <owl/configfl.h>
#include <owl/memcbox.h>

#if defined(__BORLANDC__)
# pragma option -w-ccc // Disable "Condition is always true/false"
#endif

namespace owl {

OWL_DIAGINFO;
DIAG_DECLARE_GROUP(OwlControl);

static tchar countName[] = _T("Count");
static tchar valueName[] = _T("Value");

//
// Build a response table for all messages/commands handled
// by the application.
//
DEFINE_RESPONSE_TABLE1(TMemComboBox, TComboBox)
  EV_WM_KILLFOCUS,
END_RESPONSE_TABLE;


//--------------------------------------------------------
// TMemComboBox Constructor
//
TMemComboBox::TMemComboBox(TWindow* parent, int id, int x, int y, int w, int h,
                           const tstring& name, uint textLimit,
                           uint itemLimit, TModule* module)
:
  TComboBox(parent, id, x, y, w, h, CBS_DROPDOWN, textLimit, module),
  Name(name),
  ItemLimit(itemLimit),
  TransferBufferFieldType(tbftCharArray)
{
}
//
TMemComboBox::TMemComboBox(TWindow* parent, int resId, const tstring& name,
                           uint textLimit, uint itemLimit, TModule* module)
:
  TComboBox(parent, resId, textLimit, module),
  Name(name),
  ItemLimit(itemLimit),
  TransferBufferFieldType(tbftCharArray)
{
}
//
tstring TMemComboBox::GetSectionName()
{
  return _T("History Lists\\");
}
//
void TMemComboBox::CleanupWindow()
{
  SaveContents();

  // Call base class function.
  TComboBox::CleanupWindow();
}
//
void TMemComboBox::SetupWindow()
{
  // Call base class function.
  TComboBox::SetupWindow();

  LoadContents();
}
//
void
TMemComboBox::LoadContents()
{
  TPointer<TConfigFile> config(CreateConfigFile());

  tstring sec_name = GetSectionName() + Name;

  TConfigFileSection section(*(TConfigFile*)config, sec_name.c_str());

  int count = section.ReadInteger(countName);

  TTmpBuffer<tchar> nameBuf(MAX_PATH);
  TTmpBuffer<tchar> valBuf(MAX_PATH);
  for(int i = 0; i < count; i++)
  {
    wsprintf(nameBuf, _T("%s%d"), valueName, i);
    if (section.ReadString(nameBuf,valBuf,MAX_PATH,_T("")))
    {
      AddString(valBuf);
    }
  }

  wsprintf(nameBuf, _T("%s"), valueName);
  if (section.ReadString(nameBuf, valBuf, MAX_PATH, _T("")))
    SetWindowText(valBuf);
  else
    SetWindowText(_T(""));
}
//
void
TMemComboBox::SaveContents()
{
  TPointer<TConfigFile> config(CreateConfigFile());
  tstring sec_name = GetSectionName() + Name;
  TConfigFileSection section(*(TConfigFile*)config, sec_name.c_str());

  TTmpBuffer<tchar> nameBuf(MAX_PATH);
  GetText(nameBuf, MAX_PATH);
  if(::_tcslen(nameBuf))
     AddToList(nameBuf);

  uint count = GetCount();
  if(count > ItemLimit)
    count = ItemLimit;
  section.WriteInteger(countName, count);

  TTmpBuffer<tchar> valBuf(MAX_PATH);

  wsprintf(nameBuf, _T("%s"), valueName);
  if (GetWindowText(valBuf, MAX_PATH) > 0)
    section.WriteString(nameBuf, valBuf);
  else
    section.EraseEntry(nameBuf); // If the combo box text is empty, clear the old value


  for(int i = 0; i < (int)count; i++)
  {
    wsprintf(nameBuf, _T("%s%d"), valueName, i);
    if(GetString(valBuf, i) > 0)
      section.WriteString(nameBuf, valBuf);
  }
}
//
TConfigFile*
TMemComboBox::CreateConfigFile()
{
  return new TRegConfigFile(_T("OWL Next"));
}
//
void TMemComboBox::EvKillFocus(HWND hWndGetFocus)
{
  Inherited::EvKillFocus(hWndGetFocus);

  TTmpBuffer<tchar> nameBuf(MAX_PATH);
  GetText(nameBuf, MAX_PATH);
  if (::_tcslen(nameBuf))
  {
    AddToList(nameBuf);
    SetSelIndex(0);    // AddToList() will clear the text box, so the need to restore it
  }
}
//
void TMemComboBox::AddToList(LPTSTR text)
{
  int index = FindStringExact(text,-1);
  if (index != CB_ERR)
    DeleteString(index);
  InsertString(text, 0);
}

uint TMemComboBox::Transfer(void* buffer, TTransferDirection direction)
{
  uint n = 0;
  switch (TransferBufferFieldType)
  {
  case tbftCharArray:
    n = TransferCharArray(buffer, direction);
    break;

  case tbftString:
    n = TransferString(buffer, direction);
    break;

  case tbftComboBoxData:
    n = TComboBox::Transfer(buffer, direction);
    break;

  default:
    CHECKX(false, _T("TMemComboBox::Transfer: Unknown field type."));
  }
  return n;
}

TMemComboBox::TTransferBufferFieldType TMemComboBox::GetTransferBufferFieldType() const
{
  return TransferBufferFieldType;
}

void TMemComboBox::SetTransferBufferFieldType(TTransferBufferFieldType t)
{
  TransferBufferFieldType = t;
}

uint
TMemComboBox::TransferCharArray(void* buffer, TTransferDirection direction)
{
  if (!buffer) return 0;
  tchar* s = static_cast<tchar*>(buffer);
  if (direction == tdGetData)
    GetText(s, TextLimit);
  else if (direction == tdSetData)
    SetText(s);
  return TextLimit * sizeof(tchar);
}

uint
TMemComboBox::TransferString(void* buffer, TTransferDirection direction)
{
  if (!buffer) return 0;
  tstring& s = *static_cast<tstring*>(buffer);
  if (direction == tdGetData)
    s = GetText();
  else if (direction == tdSetData)
    SetText(s);
  return sizeof(tstring);
}

IMPLEMENT_STREAMABLE1(TMemComboBox, TComboBox);

#if !defined(BI_NO_OBJ_STREAMING)

//
// Reads an instance of TMemComboBox from the supplied ipstream
//
void*
TMemComboBox::Streamer::Read(ipstream& is, uint32 /*version*/) const
{
  ReadBaseObject((TComboBox*)GetObject(), is);
  is >> GetObject()->Name;
  is >> GetObject()->ItemLimit;
  return GetObject();
}

//
// Writes the TComboBox to the supplied opstream
//
void
TMemComboBox::Streamer::Write(opstream& os) const
{
  WriteBaseObject((TComboBox*)GetObject(), os);
  os << GetObject()->Name;
  os << GetObject()->ItemLimit;
}
#endif  // if !defined(BI_NO_OBJ_STREAMING)

} // OWL namespace
