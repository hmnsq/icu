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

#अगर_अघोषित DBBI_H
#घोषणा DBBI_H

#समावेश "unicode/utypes.h"

#अगर U_SHOW_CPLUSPLUS_API

#समावेश "unicode/rbbi.h"

#अगर !UCONFIG_NO_BREAK_ITERATION

/**
 * \file
 * \brief C++ API: Dictionary Based Break Iterator
 */
 
U_NAMESPACE_BEGIN

#अगर_अघोषित U_HIDE_DEPRECATED_API
/**
 * An obsolete subclass of RuleBasedBreakIterator. Handling of dictionary-
 * based break iteration has been folded into the base class. This class
 * is deprecated as of ICU 3.6.
 * @deprecated ICU 3.6
 */
प्रकार RuleBasedBreakIterator DictionaryBasedBreakIterator;

#पूर्ण_अगर  /* U_HIDE_DEPRECATED_API */

U_NAMESPACE_END

#पूर्ण_अगर /* #if !UCONFIG_NO_BREAK_ITERATION */

#पूर्ण_अगर /* U_SHOW_CPLUSPLUS_API */

#पूर्ण_अगर
