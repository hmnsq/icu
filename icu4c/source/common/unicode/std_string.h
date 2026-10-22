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

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ __STD_STRING_H__
#ವ್ಯಾಖ್ಯಾನಿಸು __STD_STRING_H__

/**
 * \file
 * \brief C++ API: Central ICU header for including the C++ standard &lt;string&gt;
 *                 header and for related definitions.
 */

#ಅನ್ತರ್ಭಾವ "unicode/utypes.h"

#ಆದರೆ U_SHOW_CPLUSPLUS_API

// Workaround for a libstdc++ bug before libstdc++4.6 (2011).
// https://bugs.llvm.org/show_bug.cgi?id=13364
#ಆದರೆ defined(__GLIBCXX__)
namespace std { class type_info; }
#ಆದರೆ_ಅಂತ್ಯ
#ಅನ್ತರ್ಭಾವ <string>

#ಆದರೆ_ಅಂತ್ಯ /* U_SHOW_CPLUSPLUS_API */

#ಆದರೆ_ಅಂತ್ಯ  // __STD_STRING_H__
