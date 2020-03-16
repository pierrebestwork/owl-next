#ifndef EDITOR_H
#define EDITOR_H

#include <owl/editview.h>
#include <owl/richedv.h>

class TPlainEditor
  : public owl::TEditView
{
public:

  TPlainEditor(owl::TDocument& doc, owl::TWindow* parent = 0);

protected:

  virtual void Paint(owl::TDC& dc, bool erase, owl::TRect& rect); // override

  void EvGetMinMaxInfo(MINMAXINFO& minmaxinfo);

  DECLARE_RESPONSE_TABLE(TPlainEditor);
};


class TRichEditor
  : public owl::TRichEditView
{
public:

  TRichEditor(owl::TDocument& doc, owl::TWindow* parent = 0);

protected:

  void EvGetMinMaxInfo(MINMAXINFO& minmaxinfo);

  DECLARE_RESPONSE_TABLE(TRichEditor);
};

#endif
