// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
**********************************************************************
*   Copyright (C) 1999-2006,2013 IBM Corp. All rights reserved.
**********************************************************************
*   Date        Name        Description
*   12/1/99    rgillam     Complete port from Java.
*   01/13/2000 helena      Added UErrorCode to ctors.
**********************************************************************
*/

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ DBBI_H
#ವ್ಯಾಖ್ಯಾನಿಸು DBBI_H

#ಅನ್ತರ್ಭಾವ "unicode/utypes.h"

#ಆದರೆ U_SHOW_CPLUSPLUS_API

#ಅನ್ತರ್ಭಾವ "unicode/rbbi.h"

#ಆದರೆ !UCONFIG_NO_BREAK_ITERATION

/**
 * \file
 * \brief C++ API: Dictionary Based Break Iterator
 */
 
U_NAMESPACE_BEGIN

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ U_HIDE_DEPRECATED_API
/**
 * An obsolete subclass of RuleBasedBreakIterator. Handling of dictionary-
 * based break iteration has been folded into the base class. This class
 * is deprecated as of ICU 3.6.
 * @deprecated ICU 3.6
 */
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ನಿಯಮ_Based_Break_Iterator DictionaryBasedBreakIterator;

#ಆದರೆ_ಅಂತ್ಯ  /* U_HIDE_DEPRECATED_API */

U_NAMESPACE_ಕೊನೆ

#ಆದರೆ_ಅಂತ್ಯ /* #if !UCONFIG_NO_BREAK_ITERATION */

#ಆದರೆ_ಅಂತ್ಯ /* U_SHOW_CPLUSPLUS_API */

#ಆದರೆ_ಅಂತ್ಯ
