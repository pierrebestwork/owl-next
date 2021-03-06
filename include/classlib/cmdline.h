//----------------------------------------------------------------------------
// Borland BIDS Container Library Compatibility header
// Copyright (c) 1998, Yura Bidus, All Rights Reserved
//
//----------------------------------------------------------------------------

#include <owl/private/defs.h>
#if defined(BI_HAS_PRAGMA_ONCE)
# pragma once
#endif
#if defined(BI_COMP_WATCOM)
# pragma read_only_file
#endif

#if !defined(CLASSLIB_CMDLINE_H)
#define CLASSLIB_CMDLINE_H

#pragma message ("<classlib/cmdline.h> has been replaced by <owl/cmdline.h>")
#include <owl/cmdline.h>

#endif  // CLASSLIB_CMDLINE_H
