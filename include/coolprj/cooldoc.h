//----------------------------------------------------------------------------
//  Project sdidv
//  OWL NExt
//  Copyright 1999. All Rights reserved.
//
//  SUBSYSTEM:    sdidv Application
//  FILE:         cooldocument.h
//  AUTHOR:       Yura Bidus
//
//  OVERVIEW
//  ~~~~~~~~
//  Class definition for TCoolDocument (TDocument).
//  (Generated by OWL 6.x Class Expert for MS VC++, Version 1.5)
//----------------------------------------------------------------------------
#if !defined(COOLDOC_H) // Sentry, use file only if it's not already included.
#define COOLDOC_H

#include <owl\docview.h>
#include <coolprj/cooledit.h>

//{{TDocument = TCoolDocument}}
class _COOLCLASS TCoolDocument: public owl::TDocument {
  typedef owl::TDocument Inherited;
  public:
    TCoolDocument(owl::TDocument* parent = 0);
    virtual ~TCoolDocument();
    TCoolFileBuffer& GetTextBuffer(){ return TextBuffer; }

  protected_data:
    TCoolFileBuffer TextBuffer;

//{{TCoolDocumentVIRTUAL_BEGIN}}
  public:
    virtual bool Open(int mode, LPCTSTR path=0);
    virtual bool Commit(bool force=false);
    virtual bool IsDirty();
    virtual bool Revert(bool clear=false);
    virtual bool InitDoc();
//{{TCoolDocumentVIRTUAL_END}}
}; //{{TCoolDocument}}

//
inline bool TCoolDocument::IsDirty(){
  return owl::TDocument::IsDirty() || GetTextBuffer().IsDirty();
}

#endif  // COOLDOC_H sentry.

