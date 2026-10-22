// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
**********************************************************************
* Copyright (C) 1999-2010, International Business Machines Corporation and others.
* All Rights Reserved.
**********************************************************************
*   Date        Name        Description
*   11/17/99    aliu        Creation.
**********************************************************************
*/
#अगर_अघोषित UNIFILT_H
#घोषणा UNIFILT_H

#समावेश "unicode/utypes.h"

#अगर U_SHOW_CPLUSPLUS_API

#समावेश "unicode/unifunct.h"
#समावेश "unicode/unimatch.h"

/**
 * \file 
 * \brief C++ API: Unicode Filter
 */

U_NAMESPACE_BEGIN

/**
 * U_ETHER is used to represent character values for positions outside
 * a range.  For example, transliterator uses this to represent
 * characters outside the range contextStart..contextLimit-1.  This
 * allows explicit matching by rules and UnicodeSets of text outside a
 * defined range.
 * @stable ICU 3.0
 */
#घोषणा U_ETHER ((char16_t)0xFFFF)

/**
 *
 * <code>UnicodeFilter</code> defines a protocol for selecting a
 * subset of the full range (U+0000 to U+10FFFF) of Unicode characters.
 * Currently, filters are used in conjunction with classes like
 * {@link Transliterator} to only process selected characters through a
 * transformation.
 *
 * <p>Note: UnicodeFilter currently stubs out two pure virtual methods
 * of its base class, UnicodeMatcher.  These methods are toPattern()
 * and matchesIndexValue().  This is done so that filter classes that
 * are not actually used as matchers -- specifically, those in the
 * UnicodeFilterLogic component, and those in tests -- can continue to
 * work without defining these methods.  As long as a filter is not
 * used in an RBT during real transliteration, these methods will not
 * be called.  However, this breaks the UnicodeMatcher base class
 * protocol, and it is not a correct solution.
 *
 * <p>In the future we may revisit the UnicodeMatcher / UnicodeFilter
 * hierarchy and either redesign it, or simply remove the stubs in
 * UnicodeFilter and force subclasses to implement the full
 * UnicodeMatcher protocol.
 *
 * @see UnicodeFilterLogic
 * @stable ICU 2.0
 */
class U_COMMON_API UnicodeFilter : खुला UnicodeFunctor, खुला UnicodeMatcher {

खुला:
    /**
     * Destructor
     * @stable ICU 2.0
     */
    भव ~UnicodeFilter();

    /**
     * Clones this object polymorphically.
     * The caller owns the result and should delete it when done.
     * @return clone, or nullptr if an error occurred
     * @stable ICU 2.4
     */
    भव UnicodeFilter* clone() स्थिर override = 0;

    /**
     * Returns <tt>true</tt> for characters that are in the selected
     * subset.  In other words, if a character is <b>to be
     * filtered</b>, then <tt>contains()</tt> returns
     * <b><tt>false</tt></b>.
     * @stable ICU 2.0
     */
    भव UBool contains(UChar32 c) स्थिर = 0;

    /**
     * UnicodeFunctor API.  Cast 'this' to a UnicodeMatcher* pointer
     * and return the pointer.
     * @stable ICU 2.4
     */
    भव UnicodeMatcher* toMatcher() स्थिर U_LIFETIME_BOUND override;

    /**
     * Implement UnicodeMatcher API.
     * @stable ICU 2.4
     */
    भव UMatchDegree matches(स्थिर Replaceable& text,
                                 int32_t& offset,
                                 int32_t limit,
                                 UBool incremental) override;

    /**
     * UnicodeFunctor API.  Nothing to do.
     * @stable ICU 2.4
     */
    भव व्योम set_तथ्य(स्थिर Transliteration_Rule_तथ्य*) override;

    /**
     * ICU "poor man's RTTI", returns a UClassID for this class.
     *
     * @stable ICU 2.2
     */
    अटल UClassID U_EXPORT2 getStaticClassID();

रक्षित:

    /*
     * Since this class has pure virtual functions,
     * a constructor can't be used.
     * @stable ICU 2.0
     */
/*    UnicodeFilter();*/
};

/*inline UnicodeFilter::UnicodeFilter() {}*/

U_NAMESPACE_END

#पूर्ण_अगर /* U_SHOW_CPLUSPLUS_API */

#पूर्ण_अगर
