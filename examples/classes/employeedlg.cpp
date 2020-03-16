#include "pch.h"
#include "employeedlg.h"

#include "resource.h"

#include <owl/validate.h>

//
// Allow ourselves to be a little lax about namespace qualification.
// Never do this in a header though!
//
using namespace owl;

class TDecimalValidator : public TFilterValidator
{
public:
  TDecimalValidator(int digitsAfterDecimal) : _digitsAfterDecimal(digitsAfterDecimal), TFilterValidator(_T("0-9+-."))
  {
  }

  bool IsValid(LPCTSTR str)
  {
    if (TFilterValidator::IsValid(str)) {
      const _TCHAR* p = _tcschr(str, _T('.'));
      if (p == NULL)
        return true;

      if (static_cast<int>(_tcslen(p + 1)) > _digitsAfterDecimal)
        return false;

      if (_tcschr(p + 1, _T('.')) != NULL)
        return false;

      return true;
    }

    return false;
  }

  bool IsValidInput(LPTSTR str, bool suppressFill)
  {
    if (TFilterValidator::IsValidInput(str, suppressFill)) {
      const _TCHAR* p = _tcschr(str, _T('.'));
      if (p == NULL)
        return true;

      if (static_cast<int>(_tcslen(p + 1)) > _digitsAfterDecimal)
        return false;

      if (_tcschr(p + 1, _T('.')) != NULL)
        return false;

      return true;
    }

    return false;
  }

protected:
  int _digitsAfterDecimal;
};

//
// Event handler bindings
//
DEFINE_RESPONSE_TABLE1(TEmployeeDlg, TDialog)
EV_COMMAND(IDC_CUSTOM, CmSetCustom),
END_RESPONSE_TABLE;

//
// Constructor; sets up the validators we want to associate with the fields.
//
TEmployeeDlg::TEmployeeDlg(TWindow* parent, TEmployeeInformation& dataSource)
  : TEmployeeBaseDlg(parent, IDD_EMPLOYEEINFO, dataSource),
  CustomData(*new TEdit(this, IDC_EDIT2))
{
  // Use a filter validator for the name.
  // Here we limit allowed characters to letters, space and punctuation.
  // 
  (new TEdit(this, IDC_NAME))
    ->SetValidator(new TFilterValidator(_T("A-Za-z. ")));

  // Use a picture validator for the Social Security number.
  // Here we specify the valid form of the number, including conventional punctuation.
  //
  (new TEdit(this, IDC_SS))
    ->SetValidator(new TPXPictureValidator(_T("###-##-####")));

  // Use a range validator for the employee number.
  // Here we limit the number to the range [1, 999].
  //
  (new TEdit(this, IDC_EMPID))
    ->SetValidator(new TRangeValidator(1, 999));

  // Use a picture validator for the department field.
  // Here we specify a set of allowed values.
  //
  (new TEdit(this, IDC_DEPT))
    ->SetValidator(new TPXPictureValidator(_T("Sales,Dev,Mfg")));

  // Use a picture validator for the security clearance code.
  // Here we limit the field to a set of allowed values.
  //
  (new TEdit(this, IDC_SECURITY))
    ->SetValidator(new TPXPictureValidator(_T("11,12,13,14,15")));

  // Use a decimal validator for the coeffcient.
  // Here we limit the field to a floating point number with 4 numbers after the decimal point.
  //
  (new TEdit(this, IDC_COEF))
    ->SetValidator(new TDecimalValidator(4));

  // Use a picture validator for the custom input field.
  // This demonstrates dynamic application of custom pictures (input format templates).
  // See CmSetCustom.
  //
  TValidator* v = new TPXPictureValidator(_T("###"), true);
  v->UnsetOption(voOnAppend);
  CustomData.SetValidator(v);
}

//
// Dialog data transfer (using the new DDT framework introduced in OWLNext 6.32)
// For more information about the DDT framework, see the OWLNext wiki:
// http://sourceforge.net/apps/mediawiki/owlnext/index.php?title=Dialog_Data_Transfer
//
void TEmployeeDlg::DoTransferData(const owl::TTransferInfo& i, TEmployeeInformation& e)
{
  TransferEditData(i, IDC_NAME, e.Name);
  TransferEditData(i, IDC_SS, e.SocialSecurityNumber);
  TransferEditData(i, IDC_EMPID, e.EmployeeId);
  TransferEditData(i, IDC_DEPT, e.Department);
  TransferEditData(i, IDC_SECURITY, e.SecurityClearance);
  TransferEditData(i, IDC_COEF, e.Coefficient);
  TransferCheckBoxData(i, IDC_FTIME, e.FullTime);
  TransferCheckBoxData(i, IDC_PERMANENT, e.Permanent);
  TransferCheckBoxData(i, IDC_EXEMPT, e.Exempt);
  tstring picture = _T("###");
  TransferEditData(i, IDC_EDIT1, picture);
  TransferEditData(i, CustomData, e.CustomData);
}

//
// Update the picture (input format template) for the custom input field.
// Loads the picture from the picture field and applies it to the validator for the
// custom input field.
//
void TEmployeeDlg::CmSetCustom()
{
  try
  {
    // Get the user-defined picture (input format template).
    //
    tstring picture = GetDlgItemText(IDC_EDIT1);

    // Apply this user-defined picture to the custom input field.
    // Note that an edit field owns its validator, so the old validator is automatically 
    // deallocated when we apply the new validator via SetValidator.
    //
    TValidator* v = new TPXPictureValidator(picture, true);
    v->UnsetOption(voOnAppend);
    CustomData.SetValidator(v);

    // Move focus to the custom input field, ready for testing the new picture.
    //
    CustomData.SetFocus();
  }
  catch (const TXValidator& x)
  {
    // Catches any syntax errors in the specified picture.
    //
    MessageBox(x.why(), GetApplication()->GetName());
    return;
  }
}
