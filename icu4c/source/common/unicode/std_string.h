// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
*******************************************************************************
*
*   Copyright (C) 2009-2014, International Business Machines
*   Corporation and others.  All Rights Reserved.
*
*******************************************************************************
*   file name:  std_string.h
*   encoding:   UTF-8
*   tab size:   8 (not used)
*   indentation:4
*
*   created on: 2009feb19
*   created by: Markus W. Scherer
*/

#که_نه_تعریف_شوی __STD_STRING_H__
#تعریف_کړه __STD_STRING_H__

/**
 * \file
 * \brief C++ API: Central ICU header for including the C++ standard &lt;string&gt;
 *                 header and for related definitions.
 */

#ضمن "unicode/utypes.h"

#إذا U_SHOW_CPLUSPLUS_API

// Workaround for a libstdc++ bug before libstdc++4.6 (2011).
// https://bugs.llvm.org/show_bug.cgi?id=13364
#إذا defined(__GLIBCXX__)
namespace std { class type_info; }
#د_که_پای
#ضمن <string>

#د_که_پای /* U_SHOW_CPLUSPLUS_API */

#د_که_پای  // __STD_STRING_H__
