//
/// \file
/// Compiler-specific defines for Borland C++ and C++Builder
//
// Part of OWLNext - the next generation Object Windows Library
// Copyright (c) 1995, 1996 by Borland International, All Rights Reserved
//
// For more information, including license details, see
// http://owlnext.sourceforge.net
//

#if !defined(OWL_PRIVATE_BORLANDC_H)
#define OWL_PRIVATE_BORLANDC_H

// 
// The following workaround for BCC32 is needed because this compiler cannot cope with the adaptor
// AnyDispatch, which is used to fit the new dispatch functions into the old response table format. 
// An error is generated at the instantiation of the AnyDispatch template. So, to circumvent this
// issue we enforce the new response table format for BCC32.
//
#if defined(OWL5_COMPAT) && defined(__BORLANDC__) && !defined(__clang__)
#define OWL_NEW_RESPONSE_TABLE 1
#pragma message("Warning OWLNext include/owl/private/borlandc.h 37: " \
  "BCC32 does not support the compatibility layer for old response tables. " \
  "As a workaround, the new response table format is used even in OWL5_COMPAT mode.")
#endif

//
// The following workaround for BCC32 is needed because BCC32 chokes on SignatureCheck in response
// table entries. However, explicit signature checking is not needed for BCC32 anyway, since its
// error message for a handler signature mismatch will include the correct signature already.
//
#if defined (__BORLANDC__) && !defined(__clang__)
#define OWL_EV_SIGNATURE_CHECK 0
#endif

#define BI_COMP_BORLANDC __BORLANDC__
#if defined(__clang__)
#define BI_COMP_CLANG
#endif

#define BI_HAS_PRAGMA_ONCE

#if defined(OWL5_COMPAT) && !defined(UNICODE)
#define OWL_SUPPORT_BWCC
#define OWL_SUPPORT_CTL3D
#endif

#define BI_DEFINE_MINMAX_TMPL
#define BI_MULTI_THREAD

#if defined(__MT__)
#define BI_MULTI_THREAD_RTL
#endif

#ifdef UNICODE
#define _USE_OLD_RW_STL
#endif

#if !defined(BI_OWL_PACKING)
#define BI_OWL_PACKING 8
#endif

#if !defined(OWL_BREAK)
#define OWL_BREAK {DebugBreak();}
#endif

#define OWL_OUTPUT_DEBUG_STRING(lpsz) ::OutputDebugString(lpsz)

#if defined(BI_COMP_CLANG)
#define OWL_HAS_STD_EXCEPTION_PTR
#endif

//
// Include link directives
//
#if !defined(_BUILDOWLDLL)
#include <owl/private/owllink.h>
#endif

#endif
