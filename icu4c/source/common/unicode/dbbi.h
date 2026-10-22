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

#यदि_अपरिभाषितम् DBBI_H
#परिभाषय DBBI_H

#अन्तर्भाव "unicode/utypes.h"

#यदि U_SHOW_CPLUSPLUS_API

#अन्तर्भाव "unicode/rbbi.h"

#यदि !UCONFIG_NO_BREAK_ITERATION

/**
 * \file
 * \brief C++ API: Dictionary Based Break Iterator
 */
 
U_NAMESPACE_BEGIN

#यदि_अपरिभाषितम् U_HIDE_DEPRECATED_API
/**
 * An obsolete subclass of RuleBasedBreakIterator. Handling of dictionary-
 * based break iteration has been folded into the base class. This class
 * is deprecated as of ICU 3.6.
 * @deprecated ICU 3.6
 */
प्रकारनिर्देश नियमः_Based_Break_Iterator DictionaryBasedBreakIterator;

#यदि_अन्तः  /* U_HIDE_DEPRECATED_API */

U_NAMESPACE_अन्तः

#यदि_अन्तः /* #if !UCONFIG_NO_BREAK_ITERATION */

#यदि_अन्तः /* U_SHOW_CPLUSPLUS_API */

#यदि_अन्तः
