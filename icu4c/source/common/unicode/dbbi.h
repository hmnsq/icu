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

#اگر_تعریف_نشده DBBI_H
#تعریف_کن DBBI_H

#ضمن "unicode/utypes.h"

#إذا U_SHOW_CPLUSPLUS_API

#ضمن "unicode/rbbi.h"

#إذا !UCONFIG_NO_BREAK_ITERATION

/**
 * \file
 * \brief C++ API: Dictionary Based Break Iterator
 */
 
U_NAMESPACE_BEGIN

#اگر_تعریف_نشده U_HIDE_DEPRECATED_API
/**
 * An obsolete subclass of RuleBasedBreakIterator. Handling of dictionary-
 * based break iteration has been folded into the base class. This class
 * is deprecated as of ICU 3.6.
 * @deprecated ICU 3.6
 */
تعريفنوع قاعده_Based_Break_Iterator DictionaryBasedBreakIterator;

#پایان_اگر  /* U_HIDE_DEPRECATED_API */

U_NAMESPACE_انتها

#پایان_اگر /* #if !UCONFIG_NO_BREAK_ITERATION */

#پایان_اگر /* U_SHOW_CPLUSPLUS_API */

#پایان_اگر
