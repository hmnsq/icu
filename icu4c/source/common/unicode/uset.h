// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
*******************************************************************************
*
*   Copyright (C) 2002-2014, International Business Machines
*   Corporation and others.  All Rights Reserved.
*
*******************************************************************************
*   file name:  uset.h
*   encoding:   UTF-8
*   tab size:   8 (not used)
*   indentation:4
*
*   created on: 2002mar07
*   created by: Markus W. Scherer
*
*   C version of UnicodeSet.
*/


/**
 * \file
 * \brief C API: Unicode Set
 *
 * <p>This is a C wrapper around the C++ UnicodeSet class.</p>
 */

#నిర్వచితం_కాకపోతే __USET_H__
#నిర్వచించు __USET_H__

#అన్తర్భావ "unicode/utypes.h"
#అన్తర్భావ "unicode/uchar.h"

#ఒకవేళ U_SHOW_CPLUSPLUS_API || U_SHOW_CPLUSPLUS_శీర్షిక_API
#అన్తర్భావ <string>
#అన్తర్భావ <string_view>
#అన్తర్భావ "unicode/char16ptr.h"
#అన్తర్భావ "unicode/localpointer.h"
#అన్తర్భావ "unicode/utf16.h"
#అయితే_ముగింపు

#నిర్వచితం_కాకపోతే USET_DEFINED

#నిర్వచితం_కాకపోతే U_లోపల_DOXYGEN
#నిర్వచించు USET_DEFINED
#అయితే_ముగింపు
/**
 * USet is the C API type corresponding to C++ class UnicodeSet.
 * Use the uset_* API to manipulate.  Create with
 * uset_open*, and destroy with uset_close.
 * @stable ICU 2.4
 */
రకనిర్వచనం నిర్మాణం USet USet;
#అయితే_ముగింపు

/**
 * Bitmask values to be passed to uset_openPatternOptions() or
 * uset_applyPattern() taking an option parameter.
 *
 * Use at most one of USET_CASE_INSENSITIVE, USET_ADD_CASE_MAPPINGS, USET_SIMPLE_CASE_INSENSITIVE.
 * These case options are mutually exclusive.
 *
 * Undefined options bits are ignored, and reserved for future use.
 *
 * @stable ICU 2.4
 */
గణన {
    /**
     * Ignore white space within patterns unless quoted or escaped.
     * @stable ICU 2.4
     */
    USET_IGNORE_SPACE = 1,

    /**
     * Enable case insensitive matching.  E.g., "[ab]" with this flag
     * will match 'a', 'A', 'b', and 'B'.  "[^ab]" with this flag will
     * match all except 'a', 'A', 'b', and 'B'. This performs a full
     * closure over case mappings, e.g. 'ſ' (U+017F long s) for 's'.
     *
     * The resulting set is a superset of the input for the code points but
     * not for the strings.
     * It performs a case mapping closure of the code points and adds
     * full case folding strings for the code points, and reduces strings of
     * the original set to their full case folding equivalents.
     *
     * This is designed for case-insensitive matches, for example
     * in regular expressions. The full code point case closure allows checking of
     * an input character directly against the closure set.
     * Strings are matched by comparing the case-folded form from the closure
     * set with an incremental case folding of the string in question.
     *
     * The closure set will also contain single code points if the original
     * set contained case-equivalent strings (like U+00DF for "ss" or "Ss" etc.).
     * This is not necessary (that is, redundant) for the above matching method
     * but results in the same closure sets regardless of whether the original
     * set contained the code point or a string.
     *
     * @stable ICU 2.4
     */
    USET_CASE_INSENSITIVE = 2,

    /**
     * Adds all case mappings for each element in the set.
     * This adds the full lower-, title-, and uppercase mappings as well as the full case folding
     * of each existing element in the set.
     *
     * Unlike the “case insensitive” options, this does not perform a closure.
     * For example, it does not add 'ſ' (U+017F long s) for 's',
     * 'K' (U+212A Kelvin sign) for 'k', or replace set strings by their case-folded versions.
     *
     * @stable ICU 3.2
     */
    USET_కూడు_CASE_MAPPINGS = 4,

    /**
     * Enable case insensitive matching.
     * Same as USET_CASE_INSENSITIVE but using only Simple_Case_Folding (scf) mappings,
     * which map each code point to one code point,
     * not full Case_Folding (cf) mappings, which map some code points to multiple code points.
     *
     * This is designed for case-insensitive matches, for example in certain
     * regular expression implementations where only Simple_Case_Folding mappings are used,
     * such as in ECMAScript (JavaScript) regular expressions.
     *
     * @stable ICU 73
     */
    USET_SIMPLE_CASE_INSENSITIVE = 6
};

/**
 * Argument values for whether span() and similar functions continue while
 * the current character is contained vs. not contained in the set.
 *
 * The functionality is straightforward for sets with only single code points,
 * without strings (which is the common case):
 * - USET_SPAN_CONTAINED and USET_SPAN_SIMPLE work the same.
 * - USET_SPAN_CONTAINED and USET_SPAN_SIMPLE are inverses of USET_SPAN_NOT_CONTAINED.
 * - span() and spanBack() partition any string the same way when
 *   alternating between span(USET_SPAN_NOT_CONTAINED) and
 *   span(either "contained" condition).
 * - Using a complemented (inverted) set and the opposite span conditions
 *   yields the same results.
 *
 * When a set contains multi-code point strings, then these statements may not
 * be true, depending on the strings in the set (for example, whether they
 * overlap with each other) and the string that is processed.
 * For a set with strings:
 * - The complement of the set contains the opposite set of code points,
 *   but the same set of strings.
 *   Therefore, complementing both the set and the span conditions
 *   may yield different results.
 * - When starting spans at different positions in a string
 *   (span(s, ...) vs. span(s+1, ...)) the ends of the spans may be different
 *   because a set string may start before the later position.
 * - span(USET_SPAN_SIMPLE) may be shorter than
 *   span(USET_SPAN_CONTAINED) because it will not recursively try
 *   all possible paths.
 *   For example, with a set which contains the three strings "xy", "xya" and "ax",
 *   span("xyax", USET_SPAN_CONTAINED) will return 4 but
 *   span("xyax", USET_SPAN_SIMPLE) will return 3.
 *   span(USET_SPAN_SIMPLE) will never be longer than
 *   span(USET_SPAN_CONTAINED).
 * - With either "contained" condition, span() and spanBack() may partition
 *   a string in different ways.
 *   For example, with a set which contains the two strings "ab" and "ba",
 *   and when processing the string "aba",
 *   span() will yield contained/not-contained boundaries of { 0, 2, 3 }
 *   while spanBack() will yield boundaries of { 0, 1, 3 }.
 *
 * Note: If it is important to get the same boundaries whether iterating forward
 * or backward through a string, then either only span() should be used and
 * the boundaries cached for backward operation, or an ICU BreakIterator
 * could be used.
 *
 * Note: Unpaired surrogates are treated like surrogate code points.
 * Similarly, set strings match only on code point boundaries,
 * never in the middle of a surrogate pair.
 * Illegal UTF-8 sequences are treated like U+FFFD.
 * When processing UTF-8 strings, malformed set strings
 * (strings with unpaired surrogates which cannot be converted to UTF-8)
 * are ignored.
 *
 * @stable ICU 3.8
 */
రకనిర్వచనం గణన USet_పరిధి_Condition {
    /**
     * Continues a span() while there is no set element at the current position.
     * Increments by one code point at a time.
     * Stops before the first set element (character or string).
     * (For code points only, this is like while contains(current)==false).
     *
     * When span() returns, the substring between where it started and the position
     * it returned consists only of characters that are not in the set,
     * and none of its strings overlap with the span.
     *
     * @stable ICU 3.8
     */
    USET_పరిధి_NOT_CONTAINED = 0,
    /**
     * Spans the longest substring that is a concatenation of set elements (characters or strings).
     * (For characters only, this is like while contains(current)==true).
     *
     * When span() returns, the substring between where it started and the position
     * it returned consists only of set elements (characters or strings) that are in the set.
     *
     * If a set contains strings, then the span will be the longest substring for which there
     * exists at least one non-overlapping concatenation of set elements (characters or strings).
     * This is equivalent to a POSIX regular expression for <code>(OR of each set element)*</code>.
     * (Java/ICU/Perl regex stops at the first match of an OR.)
     *
     * @stable ICU 3.8
     */
    USET_పరిధి_CONTAINED = 1,
    /**
     * Continues a span() while there is a set element at the current position.
     * Increments by the longest matching element at each position.
     * (For characters only, this is like while contains(current)==true).
     *
     * When span() returns, the substring between where it started and the position
     * it returned consists only of set elements (characters or strings) that are in the set.
     *
     * If a set only contains single characters, then this is the same
     * as USET_SPAN_CONTAINED.
     *
     * If a set contains strings, then the span will be the longest substring
     * with a match at each position with the longest single set element (character or string).
     *
     * Use this span condition together with other longest-match algorithms,
     * such as ICU converters (ucnv_getUnicodeSet()).
     *
     * @stable ICU 3.8
     */
    USET_పరిధి_SIMPLE = 2,
#నిర్వచితం_కాకపోతే U_HIDE_DEPRECATED_API
    /**
     * One more than the last span condition.
     * @deprecated ICU 58 The numeric value may change over time, see ICU ticket #12420.
     */
    USET_పరిధి_CONDITION_లెక్క
#అయితే_ముగింపు  // U_HIDE_DEPRECATED_API
} USet_పరిధి_Condition;

గణన {
    /**
     * Capacity of USerializedSet::staticArray.
     * Enough for any single-code point set.
     * Also provides padding for nice sizeof(USerializedSet).
     * @stable ICU 2.4
     */
    USET_SERIALIZED_STATIC_ARRAY_సామర్థ్యం=8
};

/**
 * A serialized form of a Unicode set.  Limited manipulations are
 * possible directly on a serialized set.  See below.
 * @stable ICU 2.4
 */
రకనిర్వచనం నిర్మాణం USerialized_సమితి {
    /**
     * The serialized Unicode Set.
     * @stable ICU 2.4
     */
    స్థిరరాశి uint16_t *array;
    /**
     * The length of the array that contains BMP characters.
     * @stable ICU 2.4
     */
    int32_t bmp_నిడివి;
    /**
     * The total length of the array.
     * @stable ICU 2.4
     */
    int32_t నిడివి;
    /**
     * A small buffer for the array to reduce memory allocations.
     * @stable ICU 2.4
     */
    uint16_t staticArray[USET_SERIALIZED_STATIC_ARRAY_సామర్థ్యం];
} USerialized_సమితి;

/*********************************************************************
 * USet API
 *********************************************************************/

/**
 * Create an empty USet object.
 * Equivalent to uset_open(1, 0).
 * @return a newly created USet.  The caller must call uset_close() on
 * it when done.
 * @stable ICU 4.2
 */
U_CAPI USet* U_EXPORT2
uset_open_ఖాళీలు(శూన్య);

/**
 * Creates a USet object that contains the range of characters
 * start..end, inclusive.  If <code>start > end</code> 
 * then an empty set is created (same as using uset_openEmpty()).
 * @param start first character of the range, inclusive
 * @param end last character of the range, inclusive
 * @return a newly created USet.  The caller must call uset_close() on
 * it when done.
 * @stable ICU 2.4
 */
U_CAPI USet* U_EXPORT2
uset_open(UChar32 ప్రారంభం, UChar32 చివర);

/**
 * Creates a set from the given pattern.  See the UnicodeSet class
 * description for the syntax of the pattern language.
 * @param pattern a string specifying what characters are in the set
 * @param patternLength the length of the pattern, or -1 if null
 * terminated
 * @param ec the error code
 * @stable ICU 2.4
 */
U_CAPI USet* U_EXPORT2
uset_open_నమూనా(స్థిరరాశి UChar* నమూనా, int32_t నమూనా_నిడివి,
                 UError_సంకేతం* ec);

/**
 * Creates a set from the given pattern.  See the UnicodeSet class
 * description for the syntax of the pattern language.
 * @param pattern a string specifying what characters are in the set
 * @param patternLength the length of the pattern, or -1 if null
 * terminated
 * @param options bitmask for options to apply to the pattern.
 * Valid options are USET_IGNORE_SPACE and
 * at most one of USET_CASE_INSENSITIVE, USET_ADD_CASE_MAPPINGS, USET_SIMPLE_CASE_INSENSITIVE.
 * These case options are mutually exclusive.
 * @param ec the error code
 * @stable ICU 2.4
 */
U_CAPI USet* U_EXPORT2
uset_open_నమూనా_Options(స్థిరరాశి UChar* నమూనా, int32_t నమూనా_నిడివి,
                 uint32_t options,
                 UError_సంకేతం* ec);

/**
 * Disposes of the storage used by a USet object.  This function should
 * be called exactly once for objects returned by uset_open().
 * @param set the object to dispose of
 * @stable ICU 2.4
 */
U_CAPI శూన్య U_EXPORT2
uset_close(USet* సమితి);

#ఒకవేళ U_SHOW_CPLUSPLUS_API

U_NAMESPACE_BEGIN

/**
 * \class LocalUSetPointer
 * "Smart pointer" class, closes a USet via uset_close().
 * For most methods see the LocalPointerBase base class.
 *
 * @see LocalPointerBase
 * @see LocalPointer
 * @stable ICU 4.4
 */
U_DEFINE_LOCAL_OPEN_సూచిక(Local_USet_సూచిక, USet, uset_close);

U_NAMESPACE_చివర

#అయితే_ముగింపు

/**
 * Returns a copy of this object.
 * If this set is frozen, then the clone will be frozen as well.
 * Use uset_cloneAsThawed() for a mutable clone of a frozen set.
 * @param set the original set
 * @return the newly allocated copy of the set
 * @see uset_cloneAsThawed
 * @stable ICU 3.8
 */
U_CAPI USet * U_EXPORT2
uset_clone(స్థిరరాశి USet *సమితి);

/**
 * Determines whether the set has been frozen (made immutable) or not.
 * See the ICU4J Freezable interface for details.
 * @param set the set
 * @return true/false for whether the set has been frozen
 * @see uset_freeze
 * @see uset_cloneAsThawed
 * @stable ICU 3.8
 */
U_CAPI UBool U_EXPORT2
uset_isFrozen(స్థిరరాశి USet *సమితి);

/**
 * Freeze the set (make it immutable).
 * Once frozen, it cannot be unfrozen and is therefore thread-safe
 * until it is deleted.
 * See the ICU4J Freezable interface for details.
 * Freezing the set may also make some operations faster, for example
 * uset_contains() and uset_span().
 * A frozen set will not be modified. (It remains frozen.)
 * @param set the set
 * @see uset_isFrozen
 * @see uset_cloneAsThawed
 * @stable ICU 3.8
 */
U_CAPI శూన్య U_EXPORT2
uset_freeze(USet *సమితి);

/**
 * Clone the set and make the clone mutable.
 * See the ICU4J Freezable interface for details.
 * @param set the set
 * @return the mutable clone
 * @see uset_freeze
 * @see uset_isFrozen
 * @see uset_clone
 * @stable ICU 3.8
 */
U_CAPI USet * U_EXPORT2
uset_cloneAsThawed(స్థిరరాశి USet *సమితి);

/**
 * Causes the USet object to represent the range <code>start - end</code>.
 * If <code>start > end</code> then this USet is set to an empty range.
 * A frozen set will not be modified.
 * @param set the object to set to the given range
 * @param start first character in the set, inclusive
 * @param end last character in the set, inclusive
 * @stable ICU 3.2
 */
U_CAPI శూన్య U_EXPORT2
uset_సమితి(USet* సమితి,
         UChar32 ప్రారంభం, UChar32 చివర);

/**
 * Modifies the set to represent the set specified by the given
 * pattern. See the UnicodeSet class description for the syntax of 
 * the pattern language. See also the User Guide chapter about UnicodeSet.
 * <em>Empties the set passed before applying the pattern.</em>
 * A frozen set will not be modified.
 * @param set               The set to which the pattern is to be applied. 
 * @param pattern           A pointer to UChar string specifying what characters are in the set.
 *                          The character at pattern[0] must be a '['.
 * @param patternLength     The length of the UChar string. -1 if NUL terminated.
 * @param options           A bitmask for options to apply to the pattern.
 *                          Valid options are USET_IGNORE_SPACE and
 *                          at most one of USET_CASE_INSENSITIVE, USET_ADD_CASE_MAPPINGS,
 *                          USET_SIMPLE_CASE_INSENSITIVE.
 *                          These case options are mutually exclusive.
 * @param status            Returns an error if the pattern cannot be parsed.
 * @return                  Upon successful parse, the value is either
 *                          the index of the character after the closing ']' 
 *                          of the parsed pattern.
 *                          If the status code indicates failure, then the return value 
 *                          is the index of the error in the source.
 *
 * @stable ICU 2.8
 */
U_CAPI int32_t U_EXPORT2 
uset_apply_నమూనా(USet *సమితి,
                  స్థిరరాశి UChar *నమూనా, int32_t నమూనా_నిడివి,
                  uint32_t options,
                  UError_సంకేతం *స్థితి_2);

/**
 * Modifies the set to contain those code points which have the given value
 * for the given binary or enumerated property, as returned by
 * u_getIntPropertyValue.  Prior contents of this set are lost.
 * A frozen set will not be modified.
 *
 * @param set the object to contain the code points defined by the property
 *
 * @param prop a property in the range UCHAR_BIN_START..UCHAR_BIN_LIMIT-1
 * or UCHAR_INT_START..UCHAR_INT_LIMIT-1
 * or UCHAR_MASK_START..UCHAR_MASK_LIMIT-1.
 *
 * @param value a value in the range u_getIntPropertyMinValue(prop)..
 * u_getIntPropertyMaxValue(prop), with one exception.  If prop is
 * UCHAR_GENERAL_CATEGORY_MASK, then value should not be a UCharCategory, but
 * rather a mask value produced by U_GET_GC_MASK().  This allows grouped
 * categories such as [:L:] to be represented.
 *
 * @param ec error code input/output parameter
 *
 * @stable ICU 3.2
 */
U_CAPI శూన్య U_EXPORT2
uset_apply_Int_Property_విలువ(USet* సమితి,
                           UProperty prop, int32_t విలువ_2, UError_సంకేతం* ec);

/**
 * Modifies the set to contain those code points which have the
 * given value for the given property.  Prior contents of this
 * set are lost.
 * A frozen set will not be modified.
 *
 * @param set the object to contain the code points defined by the given
 * property and value alias
 *
 * @param prop a string specifying a property alias, either short or long.
 * The name is matched loosely.  See PropertyAliases.txt for names and a
 * description of loose matching.  If the value string is empty, then this
 * string is interpreted as either a General_Category value alias, a Script
 * value alias, a binary property alias, or a special ID.  Special IDs are
 * matched loosely and correspond to the following sets:
 *
 * "ANY" = [\\u0000-\\U0010FFFF],
 * "ASCII" = [\\u0000-\\u007F],
 * "Assigned" = [:^Cn:].
 *
 * @param propLength the length of the prop, or -1 if NULL
 *
 * @param value a string specifying a value alias, either short or long.
 * The name is matched loosely.  See PropertyValueAliases.txt for names
 * and a description of loose matching.  In addition to aliases listed,
 * numeric values and canonical combining classes may be expressed
 * numerically, e.g., ("nv", "0.5") or ("ccc", "220").  The value string
 * may also be empty.
 *
 * @param valueLength the length of the value, or -1 if NULL
 *
 * @param ec error code input/output parameter
 *
 * @stable ICU 3.2
 */
U_CAPI శూన్య U_EXPORT2
uset_applyPropertyAlias(USet* సమితి,
                        స్థిరరాశి UChar *prop, int32_t prop_నిడివి,
                        స్థిరరాశి UChar *విలువ_2, int32_t విలువ_నిడివి,
                        UError_సంకేతం* ec);

/**
 * Return true if the given position, in the given pattern, appears
 * to be the start of a UnicodeSet pattern.
 *
 * @param pattern a string specifying the pattern
 * @param patternLength the length of the pattern, or -1 if NULL
 * @param pos the given position
 * @stable ICU 3.2
 */
U_CAPI UBool U_EXPORT2
uset_resembles_నమూనా(స్థిరరాశి UChar *నమూనా, int32_t నమూనా_నిడివి,
                      int32_t pos);

/**
 * Returns a string representation of this set.  If the result of
 * calling this function is passed to a uset_openPattern(), it
 * will produce another set that is equal to this one.
 * @param set the set
 * @param result the string to receive the rules, may be NULL
 * @param resultCapacity the capacity of result, may be 0 if result is NULL
 * @param escapeUnprintable if true then convert unprintable
 * character to their hex escape representations, \\uxxxx or
 * \\Uxxxxxxxx.  Unprintable characters are those other than
 * U+000A, U+0020..U+007E.
 * @param ec error code.
 * @return length of string, possibly larger than resultCapacity
 * @stable ICU 2.4
 */
U_CAPI int32_t U_EXPORT2
uset_గమ్యం_నమూనా(స్థిరరాశి USet* సమితి,
               UChar* ఫలితం, int32_t ఫలితం_సామర్థ్యం,
               UBool escapeUnprintable,
               UError_సంకేతం* ec);

/**
 * Adds the given character to the given USet.  After this call,
 * uset_contains(set, c) will return true.
 * A frozen set will not be modified.
 * @param set the object to which to add the character
 * @param c the character to add
 * @stable ICU 2.4
 */
U_CAPI శూన్య U_EXPORT2
uset_కూడు(USet* సమితి, UChar32 c);

/**
 * Adds all of the elements in the specified set to this set if
 * they're not already present.  This operation effectively
 * modifies this set so that its value is the <i>union</i> of the two
 * sets.  The behavior of this operation is unspecified if the specified
 * collection is modified while the operation is in progress.
 * A frozen set will not be modified.
 *
 * @param set the object to which to add the set
 * @param additionalSet the source set whose elements are to be added to this set.
 * @stable ICU 2.6
 */
U_CAPI శూన్య U_EXPORT2
uset_కూడు_All(USet* సమితి, స్థిరరాశి USet *additional_సమితి);

/**
 * Adds the given range of characters to the given USet.  After this call,
 * uset_contains(set, start, end) will return true.
 * A frozen set will not be modified.
 * @param set the object to which to add the character
 * @param start the first character of the range to add, inclusive
 * @param end the last character of the range to add, inclusive
 * @stable ICU 2.2
 */
U_CAPI శూన్య U_EXPORT2
uset_కూడు_Range(USet* సమితి, UChar32 ప్రారంభం, UChar32 చివర);

/**
 * Adds the given string to the given USet.  After this call,
 * uset_containsString(set, str, strLen) will return true.
 * A frozen set will not be modified.
 * @param set the object to which to add the character
 * @param str the string to add
 * @param strLen the length of the string or -1 if null terminated.
 * @stable ICU 2.4
 */
U_CAPI శూన్య U_EXPORT2
uset_కూడు_String(USet* సమితి, స్థిరరాశి UChar* str, int32_t strLen);

/**
 * Adds each of the characters in this string to the set. Note: "ch" => {"c", "h"}
 * If this set already contains any particular character, it has no effect on that character.
 * A frozen set will not be modified.
 * @param set the object to which to add the character
 * @param str the source string
 * @param strLen the length of the string or -1 if null terminated.
 * @stable ICU 3.4
 */
U_CAPI శూన్య U_EXPORT2
uset_కూడు_All_సంకేతం_Points(USet* సమితి, స్థిరరాశి UChar *str, int32_t strLen);

/**
 * Removes the given character from the given USet.  After this call,
 * uset_contains(set, c) will return false.
 * A frozen set will not be modified.
 * @param set the object from which to remove the character
 * @param c the character to remove
 * @stable ICU 2.4
 */
U_CAPI శూన్య U_EXPORT2
uset_remove(USet* సమితి, UChar32 c);

/**
 * Removes the given range of characters from the given USet.  After this call,
 * uset_contains(set, start, end) will return false.
 * A frozen set will not be modified.
 * @param set the object to which to add the character
 * @param start the first character of the range to remove, inclusive
 * @param end the last character of the range to remove, inclusive
 * @stable ICU 2.2
 */
U_CAPI శూన్య U_EXPORT2
uset_removeRange(USet* సమితి, UChar32 ప్రారంభం, UChar32 చివర);

/**
 * Removes the given string to the given USet.  After this call,
 * uset_containsString(set, str, strLen) will return false.
 * A frozen set will not be modified.
 * @param set the object to which to add the character
 * @param str the string to remove
 * @param strLen the length of the string or -1 if null terminated.
 * @stable ICU 2.4
 */
U_CAPI శూన్య U_EXPORT2
uset_removeString(USet* సమితి, స్థిరరాశి UChar* str, int32_t strLen);

/**
 * Removes EACH of the characters in this string. Note: "ch" == {"c", "h"}
 * A frozen set will not be modified.
 *
 * @param set the object to be modified
 * @param str the string
 * @param length the length of the string, or -1 if NUL-terminated
 * @stable ICU 69
 */
U_CAPI శూన్య U_EXPORT2
uset_remove_All_సంకేతం_Points(USet *సమితి, స్థిరరాశి UChar *str, int32_t నిడివి);

/**
 * Removes from this set all of its elements that are contained in the
 * specified set.  This operation effectively modifies this
 * set so that its value is the <i>asymmetric set difference</i> of
 * the two sets.
 * A frozen set will not be modified.
 * @param set the object from which the elements are to be removed
 * @param removeSet the object that defines which elements will be
 * removed from this set
 * @stable ICU 3.2
 */
U_CAPI శూన్య U_EXPORT2
uset_removeAll(USet* సమితి, స్థిరరాశి USet* remove_సమితి);

/**
 * Retain only the elements in this set that are contained in the
 * specified range.  If <code>start > end</code> then an empty range is
 * retained, leaving the set empty.  This is equivalent to
 * a boolean logic AND, or a set INTERSECTION.
 * A frozen set will not be modified.
 *
 * @param set the object for which to retain only the specified range
 * @param start first character, inclusive, of range
 * @param end last character, inclusive, of range
 * @stable ICU 3.2
 */
U_CAPI శూన్య U_EXPORT2
uset_retain(USet* సమితి, UChar32 ప్రారంభం, UChar32 చివర);

/**
 * Retains only the specified string from this set if it is present.
 * Upon return this set will be empty if it did not contain s, or
 * will only contain s if it did contain s.
 * A frozen set will not be modified.
 *
 * @param set the object to be modified
 * @param str the string
 * @param length the length of the string, or -1 if NUL-terminated
 * @stable ICU 69
 */
U_CAPI శూన్య U_EXPORT2
uset_retainString(USet *సమితి, స్థిరరాశి UChar *str, int32_t నిడివి);

/**
 * Retains EACH of the characters in this string. Note: "ch" == {"c", "h"}
 * A frozen set will not be modified.
 *
 * @param set the object to be modified
 * @param str the string
 * @param length the length of the string, or -1 if NUL-terminated
 * @stable ICU 69
 */
U_CAPI శూన్య U_EXPORT2
uset_retain_All_సంకేతం_Points(USet *సమితి, స్థిరరాశి UChar *str, int32_t నిడివి);

/**
 * Retains only the elements in this set that are contained in the
 * specified set.  In other words, removes from this set all of
 * its elements that are not contained in the specified set.  This
 * operation effectively modifies this set so that its value is
 * the <i>intersection</i> of the two sets.
 * A frozen set will not be modified.
 *
 * @param set the object on which to perform the retain
 * @param retain set that defines which elements this set will retain
 * @stable ICU 3.2
 */
U_CAPI శూన్య U_EXPORT2
uset_retainAll(USet* సమితి, స్థిరరాశి USet* retain);

/**
 * Reallocate this objects internal structures to take up the least
 * possible space, without changing this object's value.
 * A frozen set will not be modified.
 *
 * @param set the object on which to perform the compact
 * @stable ICU 3.2
 */
U_CAPI శూన్య U_EXPORT2
uset_compact(USet* సమితి);

/**
 * This is equivalent to
 * <code>uset_complementRange(set, 0, 0x10FFFF)</code>.
 *
 * <strong>Note:</strong> This performs a symmetric difference with all code points
 * <em>and thus retains all multicharacter strings</em>.
 * In order to achieve a “code point complement” (all code points minus this set),
 * the easiest is to <code>uset_complement(set); uset_removeAllStrings(set);</code>.
 *
 * A frozen set will not be modified.
 * @param set the set
 * @stable ICU 2.4
 */
U_CAPI శూన్య U_EXPORT2
uset_complement(USet* సమితి);

/**
 * Complements the specified range in this set.  Any character in
 * the range will be removed if it is in this set, or will be
 * added if it is not in this set.  If <code>start > end</code>
 * then an empty range is complemented, leaving the set unchanged.
 * This is equivalent to a boolean logic XOR.
 * A frozen set will not be modified.
 *
 * @param set the object to be modified
 * @param start first character, inclusive, of range
 * @param end last character, inclusive, of range
 * @stable ICU 69
 */
U_CAPI శూన్య U_EXPORT2
uset_complementRange(USet *సమితి, UChar32 ప్రారంభం, UChar32 చివర);

/**
 * Complements the specified string in this set.
 * The string will be removed if it is in this set, or will be added if it is not in this set.
 * A frozen set will not be modified.
 *
 * @param set the object to be modified
 * @param str the string
 * @param length the length of the string, or -1 if NUL-terminated
 * @stable ICU 69
 */
U_CAPI శూన్య U_EXPORT2
uset_complementString(USet *సమితి, స్థిరరాశి UChar *str, int32_t నిడివి);

/**
 * Complements EACH of the characters in this string. Note: "ch" == {"c", "h"}
 * A frozen set will not be modified.
 *
 * @param set the object to be modified
 * @param str the string
 * @param length the length of the string, or -1 if NUL-terminated
 * @stable ICU 69
 */
U_CAPI శూన్య U_EXPORT2
uset_complement_All_సంకేతం_Points(USet *సమితి, స్థిరరాశి UChar *str, int32_t నిడివి);

/**
 * Complements in this set all elements contained in the specified
 * set.  Any character in the other set will be removed if it is
 * in this set, or will be added if it is not in this set.
 * A frozen set will not be modified.
 *
 * @param set the set with which to complement
 * @param complement set that defines which elements will be xor'ed
 * from this set.
 * @stable ICU 3.2
 */
U_CAPI శూన్య U_EXPORT2
uset_complementAll(USet* సమితి, స్థిరరాశి USet* complement);

/**
 * Removes all of the elements from this set.  This set will be
 * empty after this call returns.
 * A frozen set will not be modified.
 * @param set the set
 * @stable ICU 2.4
 */
U_CAPI శూన్య U_EXPORT2
uset_clear(USet* సమితి);

/**
 * Close this set over the given attribute.  For the attribute
 * USET_CASE_INSENSITIVE, the result is to modify this set so that:
 *
 * 1. For each character or string 'a' in this set, all strings or
 * characters 'b' such that foldCase(a) == foldCase(b) are added
 * to this set.
 *
 * 2. For each string 'e' in the resulting set, if e !=
 * foldCase(e), 'e' will be removed.
 *
 * Example: [aq\\u00DF{Bc}{bC}{Fi}] => [aAqQ\\u00DF\\uFB01{ss}{bc}{fi}]
 *
 * (Here foldCase(x) refers to the operation u_strFoldCase, and a
 * == b denotes that the contents are the same, not pointer
 * comparison.)
 *
 * A frozen set will not be modified.
 *
 * @param set the set
 *
 * @param attributes bitmask for attributes to close over.
 * Valid options:
 * At most one of USET_CASE_INSENSITIVE, USET_ADD_CASE_MAPPINGS, USET_SIMPLE_CASE_INSENSITIVE.
 * These case options are mutually exclusive.
 * Unrelated options bits are ignored.
 * @stable ICU 4.2
 */
U_CAPI శూన్య U_EXPORT2
uset_closeOver(USet* సమితి, int32_t లక్షణాలు);

/**
 * Remove all strings from this set.
 *
 * @param set the set
 * @stable ICU 4.2
 */
U_CAPI శూన్య U_EXPORT2
uset_remove_All_అక్షరశ్రేణులు(USet* సమితి);

/**
 * Returns true if the given USet contains no characters and no
 * strings.
 * @param set the set
 * @return true if set is empty
 * @stable ICU 2.4
 */
U_CAPI UBool U_EXPORT2
uset_is_ఖాళీలు(స్థిరరాశి USet* సమితి);

/**
 * @param set the set
 * @return true if this set contains multi-character strings or the empty string.
 * @stable ICU 70
 */
U_CAPI UBool U_EXPORT2
uset_has_అక్షరశ్రేణులు(స్థిరరాశి USet *సమితి);

/**
 * Returns true if the given USet contains the given character.
 * This function works faster with a frozen set.
 * @param set the set
 * @param c The codepoint to check for within the set
 * @return true if set contains c
 * @stable ICU 2.4
 */
U_CAPI UBool U_EXPORT2
uset_కలిగియుంది(స్థిరరాశి USet* సమితి, UChar32 c);

/**
 * Returns true if the given USet contains all characters c
 * where start <= c && c <= end.
 * @param set the set
 * @param start the first character of the range to test, inclusive
 * @param end the last character of the range to test, inclusive
 * @return true if set contains the range
 * @stable ICU 2.2
 */
U_CAPI UBool U_EXPORT2
uset_కలిగియుంది_Range(స్థిరరాశి USet* సమితి, UChar32 ప్రారంభం, UChar32 చివర);

/**
 * Returns true if the given USet contains the given string.
 * @param set the set
 * @param str the string
 * @param strLen the length of the string or -1 if null terminated.
 * @return true if set contains str
 * @stable ICU 2.4
 */
U_CAPI UBool U_EXPORT2
uset_కలిగియుంది_String(స్థిరరాశి USet* సమితి, స్థిరరాశి UChar* str, int32_t strLen);

/**
 * Returns the index of the given character within this set, where
 * the set is ordered by ascending code point.  If the character
 * is not in this set, return -1.  The inverse of this method is
 * <code>charAt()</code>.
 * @param set the set
 * @param c the character to obtain the index for
 * @return an index from 0..size()-1, or -1
 * @stable ICU 3.2
 */
U_CAPI int32_t U_EXPORT2
uset_సూచీ_యొక్క(స్థిరరాశి USet* సమితి, UChar32 c);

/**
 * Returns the character at the given index within this set, where
 * the set is ordered by ascending code point.  If the index is
 * out of range for characters, returns (UChar32)-1.
 * The inverse of this method is <code>indexOf()</code>.
 *
 * For iteration, this is slower than uset_getRangeCount()/uset_getItemCount()
 * with uset_getItem(), because for each call it skips linearly over <code>index</code>
 * characters in the ranges.
 *
 * @param set the set
 * @param charIndex an index from 0..size()-1 to obtain the char for
 * @return the character at the given index, or (UChar32)-1.
 * @stable ICU 3.2
 */
U_CAPI UChar32 U_EXPORT2
uset_charAt(స్థిరరాశి USet* సమితి, int32_t char_సూచీ);

/**
 * Returns the number of characters and strings contained in this set.
 * The last uset_getStringCount() == (uset_getItemCount() - uset_getRangeCount()) items are strings.
 *
 * This is slower than uset_getRangeCount() and uset_getItemCount() because
 * it counts the code points of all ranges.
 *
 * @param set the set
 * @return a non-negative integer counting the characters and strings
 * contained in set
 * @stable ICU 2.4
 * @see uset_getRangeCount
 * @see uset_getStringCount
 * @see uset_getItemCount
 */
U_CAPI int32_t U_EXPORT2
uset_కొలత(స్థిరరాశి USet* సమితి);

/**
 * @param set the set
 * @return the number of ranges in this set.
 * @stable ICU 70
 * @see uset_getItemCount
 * @see uset_getItem
 * @see uset_getStringCount
 * @see uset_size
 */
U_CAPI int32_t U_EXPORT2
uset_get_Range_లెక్క(స్థిరరాశి USet *సమితి);

/**
 * @param set the set
 * @return the number of strings in this set.
 * @stable ICU 76
 * @see uset_getRangeCount
 * @see uset_getItemCount
 * @see uset_size
 */
U_CAPI int32_t U_EXPORT2
uset_get_String_లెక్క(స్థిరరాశి USet *సమితి);

/**
 * Returns the index-th string (empty or multi-character) in the set.
 * The string may not be NUL-terminated.
 * The output length must be used, and the caller must not read more than that many UChars.
 *
 * @param set the set
 * @param index the string index, 0 .. uset_getStringCount() - 1
 * @param pLength the output string length; must not be NULL
 * @return the pointer to the string; NULL if the index is out of range or pLength is NULL
 * @stable ICU 76
 * @see uset_getStringCount
 */
U_CAPI స్థిరరాశి UChar* U_EXPORT2
uset_getString(స్థిరరాశి USet* సమితి U_LIFETIME_BOUND, int32_t సూచీ, int32_t* p_నిడివి);

/**
 * Returns the number of items in this set.  An item is either a range
 * of characters or a single multicharacter string.
 * @param set the set
 * @return a non-negative integer counting the character ranges
 * and/or strings contained in set
 * @stable ICU 2.4
 * @see uset_getRangeCount
 * @see uset_getStringCount
 */
U_CAPI int32_t U_EXPORT2
uset_get_అంశం_లెక్క(స్థిరరాశి USet* సమితి);

/**
 * Returns an item of this set.  An item is either a range of
 * characters or a single multicharacter string (which can be the empty string).
 *
 * If <code>itemIndex</code> is less than uset_getRangeCount(), then this function returns 0,
 * and the range is <code>*start</code>..<code>*end</code>.
 *
 * If <code>itemIndex</code> is at least uset_getRangeCount() and less than uset_getItemCount(), then
 * this function copies the string into <code>str[strCapacity]</code> and
 * returns the length of the string (0 for the empty string).
 * See uset_getString() for a function that does not copy the string contents.
 *
 * If <code>itemIndex</code> is out of range, then this function returns -1.
 *
 * Note that 0 is returned for each range as well as for the empty string.
 *
 * @param set the set
 * @param itemIndex a non-negative integer in the range 0..uset_getItemCount(set)-1
 * @param start pointer to variable to receive first character in range, inclusive;
 *              can be NULL for a string item
 * @param end pointer to variable to receive last character in range, inclusive;
 *            can be NULL for a string item
 * @param str buffer to receive the string, may be NULL
 * @param strCapacity capacity of str, or 0 if str is NULL
 * @param ec error code; U_INDEX_OUTOFBOUNDS_ERROR if the itemIndex is out of range
 * @return the length of the string (0 or >= 2), or 0 if the item is a range,
 *         or -1 if the itemIndex is out of range
 * @stable ICU 2.4
 * @see uset_getString
 */
U_CAPI int32_t U_EXPORT2
uset_get_అంశం(స్థిరరాశి USet* సమితి, int32_t అంశం_సూచీ,
             UChar32* ప్రారంభం, UChar32* చివర,
             UChar* str, int32_t str_సామర్థ్యం,
             UError_సంకేతం* ec);

/**
 * Returns true if set1 contains all the characters and strings
 * of set2. It answers the question, 'Is set1 a superset of set2?'
 * @param set1 set to be checked for containment
 * @param set2 set to be checked for containment
 * @return true if the test condition is met
 * @stable ICU 3.2
 */
U_CAPI UBool U_EXPORT2
uset_కలిగియుంది_All(స్థిరరాశి USet* set1, స్థిరరాశి USet* set2);

/**
 * Returns true if this set contains all the characters
 * of the given string. This is does not check containment of grapheme
 * clusters, like uset_containsString.
 * @param set set of characters to be checked for containment
 * @param str string containing codepoints to be checked for containment
 * @param strLen the length of the string or -1 if null terminated.
 * @return true if the test condition is met
 * @stable ICU 3.4
 */
U_CAPI UBool U_EXPORT2
uset_కలిగియుంది_All_సంకేతం_Points(స్థిరరాశి USet* సమితి, స్థిరరాశి UChar *str, int32_t strLen);

/**
 * Returns true if set1 contains none of the characters and strings
 * of set2. It answers the question, 'Is set1 a disjoint set of set2?'
 * @param set1 set to be checked for containment
 * @param set2 set to be checked for containment
 * @return true if the test condition is met
 * @stable ICU 3.2
 */
U_CAPI UBool U_EXPORT2
uset_కలిగియుంది_None(స్థిరరాశి USet* set1, స్థిరరాశి USet* set2);

/**
 * Returns true if set1 contains some of the characters and strings
 * of set2. It answers the question, 'Does set1 and set2 have an intersection?'
 * @param set1 set to be checked for containment
 * @param set2 set to be checked for containment
 * @return true if the test condition is met
 * @stable ICU 3.2
 */
U_CAPI UBool U_EXPORT2
uset_కలిగియుంది_Some(స్థిరరాశి USet* set1, స్థిరరాశి USet* set2);

/**
 * Returns the length of the initial substring of the input string which
 * consists only of characters and strings that are contained in this set
 * (USET_SPAN_CONTAINED, USET_SPAN_SIMPLE),
 * or only of characters and strings that are not contained
 * in this set (USET_SPAN_NOT_CONTAINED).
 * See USetSpanCondition for details.
 * Similar to the strspn() C library function.
 * Unpaired surrogates are treated according to contains() of their surrogate code points.
 * This function works faster with a frozen set and with a non-negative string length argument.
 * @param set the set
 * @param s start of the string
 * @param length of the string; can be -1 for NUL-terminated
 * @param spanCondition specifies the containment condition
 * @return the length of the initial substring according to the spanCondition;
 *         0 if the start of the string does not fit the spanCondition
 * @stable ICU 3.8
 * @see USetSpanCondition
 */
U_CAPI int32_t U_EXPORT2
uset_పరిధి(స్థిరరాశి USet *సమితి, స్థిరరాశి UChar *s, int32_t నిడివి, USet_పరిధి_Condition పరిధి_Condition);

/**
 * Returns the start of the trailing substring of the input string which
 * consists only of characters and strings that are contained in this set
 * (USET_SPAN_CONTAINED, USET_SPAN_SIMPLE),
 * or only of characters and strings that are not contained
 * in this set (USET_SPAN_NOT_CONTAINED).
 * See USetSpanCondition for details.
 * Unpaired surrogates are treated according to contains() of their surrogate code points.
 * This function works faster with a frozen set and with a non-negative string length argument.
 * @param set the set
 * @param s start of the string
 * @param length of the string; can be -1 for NUL-terminated
 * @param spanCondition specifies the containment condition
 * @return the start of the trailing substring according to the spanCondition;
 *         the string length if the end of the string does not fit the spanCondition
 * @stable ICU 3.8
 * @see USetSpanCondition
 */
U_CAPI int32_t U_EXPORT2
uset_పరిధి_వెనుక(స్థిరరాశి USet *సమితి, స్థిరరాశి UChar *s, int32_t నిడివి, USet_పరిధి_Condition పరిధి_Condition);

/**
 * Returns the length of the initial substring of the input string which
 * consists only of characters and strings that are contained in this set
 * (USET_SPAN_CONTAINED, USET_SPAN_SIMPLE),
 * or only of characters and strings that are not contained
 * in this set (USET_SPAN_NOT_CONTAINED).
 * See USetSpanCondition for details.
 * Similar to the strspn() C library function.
 * Malformed byte sequences are treated according to contains(0xfffd).
 * This function works faster with a frozen set and with a non-negative string length argument.
 * @param set the set
 * @param s start of the string (UTF-8)
 * @param length of the string; can be -1 for NUL-terminated
 * @param spanCondition specifies the containment condition
 * @return the length of the initial substring according to the spanCondition;
 *         0 if the start of the string does not fit the spanCondition
 * @stable ICU 3.8
 * @see USetSpanCondition
 */
U_CAPI int32_t U_EXPORT2
uset_పరిధి_UTF8(స్థిరరాశి USet *సమితి, స్థిరరాశి అక్షరం *s, int32_t నిడివి, USet_పరిధి_Condition పరిధి_Condition);

/**
 * Returns the start of the trailing substring of the input string which
 * consists only of characters and strings that are contained in this set
 * (USET_SPAN_CONTAINED, USET_SPAN_SIMPLE),
 * or only of characters and strings that are not contained
 * in this set (USET_SPAN_NOT_CONTAINED).
 * See USetSpanCondition for details.
 * Malformed byte sequences are treated according to contains(0xfffd).
 * This function works faster with a frozen set and with a non-negative string length argument.
 * @param set the set
 * @param s start of the string (UTF-8)
 * @param length of the string; can be -1 for NUL-terminated
 * @param spanCondition specifies the containment condition
 * @return the start of the trailing substring according to the spanCondition;
 *         the string length if the end of the string does not fit the spanCondition
 * @stable ICU 3.8
 * @see USetSpanCondition
 */
U_CAPI int32_t U_EXPORT2
uset_పరిధి_వెనుక_UTF8(స్థిరరాశి USet *సమితి, స్థిరరాశి అక్షరం *s, int32_t నిడివి, USet_పరిధి_Condition పరిధి_Condition);

/**
 * Returns true if set1 contains all of the characters and strings
 * of set2, and vis versa. It answers the question, 'Is set1 equal to set2?'
 * @param set1 set to be checked for containment
 * @param set2 set to be checked for containment
 * @return true if the test condition is met
 * @stable ICU 3.2
 */
U_CAPI UBool U_EXPORT2
uset_equals(స్థిరరాశి USet* set1, స్థిరరాశి USet* set2);

/*********************************************************************
 * Serialized set API
 *********************************************************************/

/**
 * Serializes this set into an array of 16-bit integers.  Serialization
 * (currently) only records the characters in the set; multicharacter
 * strings are ignored.
 *
 * The array
 * has following format (each line is one 16-bit integer):
 *
 *  length     = (n+2*m) | (m!=0?0x8000:0)
 *  bmpLength  = n; present if m!=0
 *  bmp[0]
 *  bmp[1]
 *  ...
 *  bmp[n-1]
 *  supp-high[0]
 *  supp-low[0]
 *  supp-high[1]
 *  supp-low[1]
 *  ...
 *  supp-high[m-1]
 *  supp-low[m-1]
 *
 * The array starts with a header.  After the header are n bmp
 * code points, then m supplementary code points.  Either n or m
 * or both may be zero.  n+2*m is always <= 0x7FFF.
 *
 * If there are no supplementary characters (if m==0) then the
 * header is one 16-bit integer, 'length', with value n.
 *
 * If there are supplementary characters (if m!=0) then the header
 * is two 16-bit integers.  The first, 'length', has value
 * (n+2*m)|0x8000.  The second, 'bmpLength', has value n.
 *
 * After the header the code points are stored in ascending order.
 * Supplementary code points are stored as most significant 16
 * bits followed by least significant 16 bits.
 *
 * @param set the set
 * @param dest pointer to buffer of destCapacity 16-bit integers.
 * May be NULL only if destCapacity is zero.
 * @param destCapacity size of dest, or zero.  Must not be negative.
 * @param pErrorCode pointer to the error code.  Will be set to
 * U_INDEX_OUTOFBOUNDS_ERROR if n+2*m > 0x7FFF.  Will be set to
 * U_BUFFER_OVERFLOW_ERROR if n+2*m+(m!=0?2:1) > destCapacity.
 * @return the total length of the serialized format, including
 * the header, that is, n+2*m+(m!=0?2:1), or 0 on error other
 * than U_BUFFER_OVERFLOW_ERROR.
 * @stable ICU 2.4
 */
U_CAPI int32_t U_EXPORT2
uset_serialize(స్థిరరాశి USet* సమితి, uint16_t* dest, int32_t dest_సామర్థ్యం, UError_సంకేతం* p_Error_సంకేతం);

/**
 * Given a serialized array, fill in the given serialized set object.
 * @param fillSet pointer to result
 * @param src pointer to start of array
 * @param srcLength length of array
 * @return true if the given array is valid, otherwise false
 * @stable ICU 2.4
 */
U_CAPI UBool U_EXPORT2
uset_get_Serialized_సమితి(USerialized_సమితి* fill_సమితి, స్థిరరాశి uint16_t* src, int32_t src_నిడివి);

/**
 * Set the USerializedSet to contain the given character (and nothing
 * else).
 * @param fillSet pointer to result
 * @param c The codepoint to set
 * @stable ICU 2.4
 */
U_CAPI శూన్య U_EXPORT2
uset_సమితి_Serialized_గమ్యం_One(USerialized_సమితి* fill_సమితి, UChar32 c);

/**
 * Returns true if the given USerializedSet contains the given
 * character.
 * @param set the serialized set
 * @param c The codepoint to check for within the set
 * @return true if set contains c
 * @stable ICU 2.4
 */
U_CAPI UBool U_EXPORT2
uset_serialized_కలిగియుంది(స్థిరరాశి USerialized_సమితి* సమితి, UChar32 c);

/**
 * Returns the number of disjoint ranges of characters contained in
 * the given serialized set.  Ignores any strings contained in the
 * set.
 * @param set the serialized set
 * @return a non-negative integer counting the character ranges
 * contained in set
 * @stable ICU 2.4
 */
U_CAPI int32_t U_EXPORT2
uset_get_Serialized_Range_లెక్క(స్థిరరాశి USerialized_సమితి* సమితి);

/**
 * Returns a range of characters contained in the given serialized
 * set.
 * @param set the serialized set
 * @param rangeIndex a non-negative integer in the range 0..
 * uset_getSerializedRangeCount(set)-1
 * @param pStart pointer to variable to receive first character
 * in range, inclusive
 * @param pEnd pointer to variable to receive last character in range,
 * inclusive
 * @return true if rangeIndex is valid, otherwise false
 * @stable ICU 2.4
 */
U_CAPI UBool U_EXPORT2
uset_getSerializedRange(స్థిరరాశి USerialized_సమితి* సమితి, int32_t range_సూచీ,
                        UChar32* p_ప్రారంభం, UChar32* p_చివర);

#ఒకవేళ U_SHOW_CPLUSPLUS_API || U_SHOW_CPLUSPLUS_శీర్షిక_API

namespace U_శీర్షిక_ONLY_NAMESPACE {

// Note: Not U_COMMON_API, and not a subclass of UMemory, because this is a header-only class,
// not intended to be used via export from the ICU DLL.

/**
 * Iterator returned by USetCodePoints.
 * @stable ICU 76
 */
class USet_సంకేతం_బిందువు_Iterator {
బహిరంగ:
    /** @stable ICU 76 */
    USet_సంకేతం_బిందువు_Iterator(స్థిరరాశి USet_సంకేతం_బిందువు_Iterator &other) = అప్రమేయ;

    /** @stable ICU 76 */
    bool క్రియాకారకం==(స్థిరరాశి USet_సంకేతం_బిందువు_Iterator &other) స్థిరరాశి {
        // No need to compare rangeCount & end given private constructor
        // and assuming we don't compare iterators across the set being modified.
        // And comparing rangeIndex is redundant with comparing c.
        // We might even skip comparing uset.
        // Unless we want operator==() to be "correct" for more than iteration.
        తిరిగి uset == other.uset && c == other.c;
    }

    /** @stable ICU 76 */
    bool క్రియాకారకం!=(స్థిరరాశి USet_సంకేతం_బిందువు_Iterator &other) స్థిరరాశి { తిరిగి !క్రియాకారకం==(other); }

    /** @stable ICU 76 */
    UChar32 క్రియాకారకం*() స్థిరరాశి { తిరిగి c; }

    /**
     * Pre-increment.
     * @stable ICU 76
     */
    USet_సంకేతం_బిందువు_Iterator& క్రియాకారకం++() U_LIFETIME_BOUND {
        ఒకవేళ (c < చివర) {
            ++c;
        } లేకపోతే ఒకవేళ (range_సూచీ < range_లెక్క) {
            UError_సంకేతం error_సంకేతం = U_ZERO_ERROR;
            int32_t ఫలితం = uset_get_అంశం(uset, range_సూచీ, &c, &చివర, nullptr, 0, &error_సంకేతం);
            ఒకవేళ (U_SUCCESS(error_సంకేతం) && ఫలితం == 0) {
                ++range_సూచీ;
            } లేకపోతే {
                c = చివర = U_SENTINEL;
            }
        } లేకపోతే {
            c = చివర = U_SENTINEL;
        }
        తిరిగి *this;
    }

    /**
     * Post-increment.
     * @stable ICU 76
     */
    USet_సంకేతం_బిందువు_Iterator క్రియాకారకం++(పూర్ణాంకం) {
        USet_సంకేతం_బిందువు_Iterator ఫలితం(*this);
        క్రియాకారకం++();
        తిరిగి ఫలితం;
    }

అంతరంగ:
    మిత్ర class USet_సంకేతం_Points;

    USet_సంకేతం_బిందువు_Iterator(స్థిరరాశి USet* pUset U_LIFETIME_BOUND,
                          int32_t n_Range_సూచీ,
                          int32_t n_Range_లెక్క)
            : uset(pUset), range_సూచీ(n_Range_సూచీ), range_లెక్క(n_Range_లెక్క),
                c(U_SENTINEL), చివర(U_SENTINEL) {
        // Fetch the first range.
        క్రియాకారకం++();
    }

    స్థిరరాశి USet *uset;
    int32_t range_సూచీ;
    int32_t range_లెక్క;
    UChar32 c, చివర;
};

/**
 * C++ "range" for iterating over the code points of a USet.
 *
 * \code
 * using U_HEADER_NESTED_NAMESPACE::USetCodePoints;
 * LocalUSetPointer uset(uset_openPattern(u"[abcçカ🚴]", -1, &errorCode));
 * for (UChar32 c : USetCodePoints(uset.getAlias())) {
 *     printf("uset.codePoint U+%04lx\n", (long)c);
 * }
 * \endcode
 *
 * C++ UnicodeSet has member functions for iteration, including codePoints().
 *
 * @stable ICU 76
 * @see USetRanges
 * @see USetStrings
 * @see USetElements
 */
class USet_సంకేతం_Points {
బహిరంగ:
    /**
     * Constructs a C++ "range" object over the code points of the USet.
     * @stable ICU 76
     */
    USet_సంకేతం_Points(స్థిరరాశి USet* pUset U_LIFETIME_BOUND)
            : uset(pUset), range_లెక్క(uset_get_Range_లెక్క(pUset)) {}

    /** @stable ICU 76 */
    USet_సంకేతం_Points(స్థిరరాశి USet_సంకేతం_Points &other) = అప్రమేయ;

    /** @stable ICU 76 */
    USet_సంకేతం_బిందువు_Iterator begin() స్థిరరాశి {
        తిరిగి USet_సంకేతం_బిందువు_Iterator(uset, 0, range_లెక్క);
    }

    /** @stable ICU 76 */
    USet_సంకేతం_బిందువు_Iterator చివర() స్థిరరాశి {
        తిరిగి USet_సంకేతం_బిందువు_Iterator(uset, range_లెక్క, range_లెక్క);
    }

అంతరంగ:
    స్థిరరాశి USet *uset;
    int32_t range_లెక్క;
};

/**
 * A contiguous range of code points in a USet/UnicodeSet.
 * Returned by USetRangeIterator which is returned by USetRanges.
 * Both the rangeStart and rangeEnd are in the range.
 * (end() returns an iterator corresponding to rangeEnd+1.)
 * @stable ICU 76
 */
నిర్మాణం సంకేతం_బిందువు_Range {
    /** @stable ICU 76 */
    నిర్మాణం iterator {
        /** @stable ICU 76 */
        iterator(UChar32 aC) : c(aC) {}

        /** @stable ICU 76 */
        bool క్రియాకారకం==(స్థిరరాశి iterator &other) స్థిరరాశి { తిరిగి c == other.c; }
        /** @stable ICU 76 */
        bool క్రియాకారకం!=(స్థిరరాశి iterator &other) స్థిరరాశి { తిరిగి !క్రియాకారకం==(other); }

        /** @stable ICU 76 */
        UChar32 క్రియాకారకం*() స్థిరరాశి { తిరిగి c; }

        /**
         * Pre-increment.
         * @stable ICU 76
         */
        iterator& క్రియాకారకం++() U_LIFETIME_BOUND {
            ++c;
            తిరిగి *this;
        }

        /**
         * Post-increment.
         * @stable ICU 76
         */
        iterator క్రియాకారకం++(పూర్ణాంకం) {
            తిరిగి c++;
        }

        /**
         * The current code point in the range.
         * @stable ICU 76
         */
        UChar32 c;
    };

    /** @stable ICU 76 */
    సంకేతం_బిందువు_Range(UChar32 ప్రారంభం, UChar32 చివర) : range_ప్రారంభం(ప్రారంభం), range_చివర(చివర) {}
    /** @stable ICU 76 */
    సంకేతం_బిందువు_Range(స్థిరరాశి సంకేతం_బిందువు_Range &other) = అప్రమేయ;
    /** @stable ICU 76 */
    size_t కొలత_2() స్థిరరాశి { తిరిగి (range_చివర + 1) - range_ప్రారంభం; }
    /** @stable ICU 76 */
    iterator begin() స్థిరరాశి { తిరిగి range_ప్రారంభం; }
    /** @stable ICU 76 */
    iterator చివర() స్థిరరాశి { తిరిగి range_చివర + 1; }

    /**
     * Start of a USet/UnicodeSet range of code points.
     * @stable ICU 76
     */
    UChar32 range_ప్రారంభం;
    /**
     * Inclusive end of a USet/UnicodeSet range of code points.
     * @stable ICU 76
     */
    UChar32 range_చివర;
};

/**
 * Iterator returned by USetRanges.
 * @stable ICU 76
 */
class USetRangeIterator {
బహిరంగ:
    /** @stable ICU 76 */
    USetRangeIterator(స్థిరరాశి USetRangeIterator &other) = అప్రమేయ;

    /** @stable ICU 76 */
    bool క్రియాకారకం==(స్థిరరాశి USetRangeIterator &other) స్థిరరాశి {
        // No need to compare rangeCount given private constructor
        // and assuming we don't compare iterators across the set being modified.
        // We might even skip comparing uset.
        // Unless we want operator==() to be "correct" for more than iteration.
        తిరిగి uset == other.uset && range_సూచీ == other.range_సూచీ;
    }

    /** @stable ICU 76 */
    bool క్రియాకారకం!=(స్థిరరాశి USetRangeIterator &other) స్థిరరాశి { తిరిగి !క్రియాకారకం==(other); }

    /** @stable ICU 76 */
    సంకేతం_బిందువు_Range క్రియాకారకం*() స్థిరరాశి {
        ఒకవేళ (range_సూచీ < range_లెక్క) {
            UChar32 ప్రారంభం, చివర;
            UError_సంకేతం error_సంకేతం = U_ZERO_ERROR;
            int32_t ఫలితం = uset_get_అంశం(uset, range_సూచీ, &ప్రారంభం, &చివర, nullptr, 0, &error_సంకేతం);
            ఒకవేళ (U_SUCCESS(error_సంకేతం) && ఫలితం == 0) {
                తిరిగి సంకేతం_బిందువు_Range(ప్రారంభం, చివర);
            }
        }
        తిరిగి సంకేతం_బిందువు_Range(U_SENTINEL, U_SENTINEL);
    }

    /**
     * Pre-increment.
     * @stable ICU 76
     */
    USetRangeIterator& క్రియాకారకం++() U_LIFETIME_BOUND {
        ++range_సూచీ;
        తిరిగి *this;
    }

    /**
     * Post-increment.
     * @stable ICU 76
     */
    USetRangeIterator క్రియాకారకం++(పూర్ణాంకం) {
        USetRangeIterator ఫలితం(*this);
        ++range_సూచీ;
        తిరిగి ఫలితం;
    }

అంతరంగ:
    మిత్ర class USetRanges;

    USetRangeIterator(స్థిరరాశి USet* pUset U_LIFETIME_BOUND, int32_t n_Range_సూచీ, int32_t n_Range_లెక్క)
            : uset(pUset), range_సూచీ(n_Range_సూచీ), range_లెక్క(n_Range_లెక్క) {}

    స్థిరరాశి USet *uset;
    int32_t range_సూచీ;
    int32_t range_లెక్క;
};

/**
 * C++ "range" for iterating over the code point ranges of a USet.
 *
 * \code
 * using U_HEADER_NESTED_NAMESPACE::USetRanges;
 * LocalUSetPointer uset(uset_openPattern(u"[abcçカ🚴]", -1, &errorCode));
 * for (auto [start, end] : USetRanges(uset.getAlias())) {
 *     printf("uset.range U+%04lx..U+%04lx\n", (long)start, (long)end);
 * }
 * for (auto range : USetRanges(uset.getAlias())) {
 *     for (UChar32 c : range) {
 *         printf("uset.range.c U+%04lx\n", (long)c);
 *     }
 * }
 * \endcode
 *
 * C++ UnicodeSet has member functions for iteration, including ranges().
 *
 * @stable ICU 76
 * @see USetCodePoints
 * @see USetStrings
 * @see USetElements
 */
class USetRanges {
బహిరంగ:
    /**
     * Constructs a C++ "range" object over the code point ranges of the USet.
     * @stable ICU 76
     */
    USetRanges(స్థిరరాశి USet* pUset U_LIFETIME_BOUND)
            : uset(pUset), range_లెక్క(uset_get_Range_లెక్క(pUset)) {}

    /** @stable ICU 76 */
    USetRanges(స్థిరరాశి USetRanges &other) = అప్రమేయ;

    /** @stable ICU 76 */
    USetRangeIterator begin() స్థిరరాశి {
        తిరిగి USetRangeIterator(uset, 0, range_లెక్క);
    }

    /** @stable ICU 76 */
    USetRangeIterator చివర() స్థిరరాశి {
        తిరిగి USetRangeIterator(uset, range_లెక్క, range_లెక్క);
    }

అంతరంగ:
    స్థిరరాశి USet *uset;
    int32_t range_లెక్క;
};

/**
 * Iterator returned by USetStrings.
 * @stable ICU 76
 */
class USetStringIterator {
బహిరంగ:
    /** @stable ICU 76 */
    USetStringIterator(స్థిరరాశి USetStringIterator &other) = అప్రమేయ;

    /** @stable ICU 76 */
    bool క్రియాకారకం==(స్థిరరాశి USetStringIterator &other) స్థిరరాశి {
        // No need to compare count given private constructor
        // and assuming we don't compare iterators across the set being modified.
        // We might even skip comparing uset.
        // Unless we want operator==() to be "correct" for more than iteration.
        తిరిగి uset == other.uset && సూచీ == other.సూచీ;
    }

    /** @stable ICU 76 */
    bool క్రియాకారకం!=(స్థిరరాశి USetStringIterator &other) స్థిరరాశి { తిరిగి !క్రియాకారకం==(other); }

    /** @stable ICU 76 */
    std::u16string_view క్రియాకారకం*() స్థిరరాశి {
        ఒకవేళ (సూచీ < లెక్క) {
            int32_t నిడివి;
            స్థిరరాశి UChar *uchars = uset_getString(uset, సూచీ, &నిడివి);
            // assert uchars != nullptr;
            తిరిగి {uprv_char16PtrFromUChar(uchars), static_cast<size_t>(నిడివి)};
        }
        తిరిగి {};
    }

    /**
     * Pre-increment.
     * @stable ICU 76
     */
    USetStringIterator& క్రియాకారకం++() U_LIFETIME_BOUND {
        ++సూచీ;
        తిరిగి *this;
    }

    /**
     * Post-increment.
     * @stable ICU 76
     */
    USetStringIterator క్రియాకారకం++(పూర్ణాంకం) {
        USetStringIterator ఫలితం(*this);
        ++సూచీ;
        తిరిగి ఫలితం;
    }

అంతరంగ:
    మిత్ర class USet_అక్షరశ్రేణులు;

    USetStringIterator(స్థిరరాశి USet* pUset U_LIFETIME_BOUND, int32_t n_సూచీ, int32_t n_లెక్క)
            : uset(pUset), సూచీ(n_సూచీ), లెక్క(n_లెక్క) {}

    స్థిరరాశి USet *uset;
    int32_t సూచీ;
    int32_t లెక్క;
};

/**
 * C++ "range" for iterating over the empty and multi-character strings of a USet.
 *
 * \code
 * using U_HEADER_NESTED_NAMESPACE::USetStrings;
 * LocalUSetPointer uset(uset_openPattern(u"[abcçカ🚴{}{abc}{de}]", -1, &errorCode));
 * for (auto s : USetStrings(uset.getAlias())) {
 *     int32_t len32 = s.length();
 *     char utf8[200];
 *     u_strToUTF8WithSub(utf8, int32_t{sizeof(utf8) - 1}, nullptr,
 *                        s.data(), len32, 0xFFFD, nullptr, errorCode);
 *     printf("uset.string length %ld \"%s\"\n", long{len32}, utf8);
 * }
 * \endcode
 *
 * C++ UnicodeSet has member functions for iteration, including strings().
 *
 * @stable ICU 76
 * @see USetCodePoints
 * @see USetRanges
 * @see USetElements
 */
class USet_అక్షరశ్రేణులు {
బహిరంగ:
    /**
     * Constructs a C++ "range" object over the strings of the USet.
     * @stable ICU 76
     */
    USet_అక్షరశ్రేణులు(స్థిరరాశి USet* pUset U_LIFETIME_BOUND)
            : uset(pUset), లెక్క(uset_get_String_లెక్క(pUset)) {}

    /** @stable ICU 76 */
    USet_అక్షరశ్రేణులు(స్థిరరాశి USet_అక్షరశ్రేణులు &other) = అప్రమేయ;

    /** @stable ICU 76 */
    USetStringIterator begin() స్థిరరాశి {
        తిరిగి USetStringIterator(uset, 0, లెక్క);
    }

    /** @stable ICU 76 */
    USetStringIterator చివర() స్థిరరాశి {
        తిరిగి USetStringIterator(uset, లెక్క, లెక్క);
    }

అంతరంగ:
    స్థిరరాశి USet *uset;
    int32_t లెక్క;
};

/**
 * Iterator returned by USetElements.
 * @stable ICU 77
 */
class USetElementIterator {
బహిరంగ:
    /** @stable ICU 77 */
    USetElementIterator(స్థిరరాశి USetElementIterator &other) = అప్రమేయ;

    /** @stable ICU 77 */
    bool క్రియాకారకం==(స్థిరరాశి USetElementIterator &other) స్థిరరాశి {
        // No need to compare rangeCount & end given private constructor
        // and assuming we don't compare iterators across the set being modified.
        // We might even skip comparing uset.
        // Unless we want operator==() to be "correct" for more than iteration.
        తిరిగి uset == other.uset && c == other.c && సూచీ == other.సూచీ;
    }

    /** @stable ICU 77 */
    bool క్రియాకారకం!=(స్థిరరాశి USetElementIterator &other) స్థిరరాశి { తిరిగి !క్రియాకారకం==(other); }

    /** @stable ICU 77 */
    std::u16string క్రియాకారకం*() స్థిరరాశి {
        ఒకవేళ (c >= 0) {
            తిరిగి c <= 0xffff ?
                std::u16string({static_cast<char16_t>(c)}) :
                std::u16string({U16_LEAD(c), U16_TRAIL(c)});
        } లేకపోతే ఒకవేళ (సూచీ < మొత్తం_లెక్క) {
            int32_t నిడివి;
            స్థిరరాశి UChar *uchars = uset_getString(uset, సూచీ - range_లెక్క, &నిడివి);
            // assert uchars != nullptr;
            తిరిగి {uprv_char16PtrFromUChar(uchars), static_cast<size_t>(నిడివి)};
        } లేకపోతే {
            తిరిగి {};
        }
    }

    /**
     * Pre-increment.
     * @stable ICU 77
     */
    USetElementIterator& క్రియాకారకం++() U_LIFETIME_BOUND {
        ఒకవేళ (c < చివర) {
            ++c;
        } లేకపోతే ఒకవేళ (సూచీ < range_లెక్క) {
            UError_సంకేతం error_సంకేతం = U_ZERO_ERROR;
            int32_t ఫలితం = uset_get_అంశం(uset, సూచీ, &c, &చివర, nullptr, 0, &error_సంకేతం);
            ఒకవేళ (U_SUCCESS(error_సంకేతం) && ఫలితం == 0) {
                ++సూచీ;
            } లేకపోతే {
                c = చివర = U_SENTINEL;
            }
        } లేకపోతే ఒకవేళ (c >= 0) {
            // assert index == rangeCount;
            // Switch from the last range to the first string.
            c = చివర = U_SENTINEL;
        } లేకపోతే {
            ++సూచీ;
        }
        తిరిగి *this;
    }

    /**
     * Post-increment.
     * @stable ICU 77
     */
    USetElementIterator క్రియాకారకం++(పూర్ణాంకం) {
        USetElementIterator ఫలితం(*this);
        క్రియాకారకం++();
        తిరిగి ఫలితం;
    }

అంతరంగ:
    మిత్ర class USetElements;

    USetElementIterator(స్థిరరాశి USet* pUset U_LIFETIME_BOUND,
                        int32_t n_సూచీ,
                        int32_t n_Range_లెక్క,
                        int32_t n_మొత్తం_లెక్క)
            : uset(pUset), సూచీ(n_సూచీ), range_లెక్క(n_Range_లెక్క), మొత్తం_లెక్క(n_మొత్తం_లెక్క),
                c(U_SENTINEL), చివర(U_SENTINEL) {
        ఒకవేళ (సూచీ < range_లెక్క) {
            // Fetch the first range.
            క్రియాకారకం++();
        }
        // Otherwise don't move beyond the (index - rangeCount)-th string.
    }

    స్థిరరాశి USet *uset;
    int32_t సూచీ;
    /** Number of UnicodeSet/USet code point ranges. */
    int32_t range_లెక్క;
    /**
     * Number of code point ranges plus number of strings.
     * index starts from 0, counts ranges while less than rangeCount,
     * then counts strings while at least rangeCount and less than totalCount.
     *
     * Note that totalCount is the same as uset_getItemCount(), but usually
     * smaller than the number of elements returned by this iterator
     * because we return each code point of each range.
     */
    int32_t మొత్తం_లెక్క;
    UChar32 c, చివర;
};

/**
 * A C++ "range" for iterating over all of the elements of a USet.
 * Convenient all-in one iteration, but creates a std::u16string for each
 * code point or string.
 *
 * Code points are returned first, then empty and multi-character strings.
 *
 * \code
 * using U_HEADER_NESTED_NAMESPACE::USetElements;
 * LocalUSetPointer uset(uset_openPattern(u"[abcçカ🚴{}{abc}{de}]", -1, &errorCode));
 * for (auto el : USetElements(uset.getAlias())) {
 *     int32_t len32 = el.length();
 *     char utf8[200];
 *     u_strToUTF8WithSub(utf8, int32_t{sizeof(utf8) - 1}, nullptr,
 *                        el.data(), len32, 0xFFFD, nullptr, errorCode);
 *     printf("uset.element length %ld \"%s\"\n", long{len32}, utf8);
 * }
 * \endcode
 *
 * C++ UnicodeSet has member functions for iteration, including begin() and end().
 *
 * @return an all-elements iterator.
 * @stable ICU 77
 * @see USetCodePoints
 * @see USetRanges
 * @see USetStrings
 */
class USetElements {
బహిరంగ:
    /**
     * Constructs a C++ "range" object over all of the elements of the USet.
     * @stable ICU 77
     */
    USetElements(స్థిరరాశి USet* pUset U_LIFETIME_BOUND)
        : uset(pUset), range_లెక్క(uset_get_Range_లెక్క(pUset)),
            string_లెక్క(uset_get_String_లెక్క(pUset)) {}

    /** @stable ICU 77 */
    USetElements(స్థిరరాశి USetElements &other) = అప్రమేయ;

    /** @stable ICU 77 */
    USetElementIterator begin() స్థిరరాశి {
        తిరిగి USetElementIterator(uset, 0, range_లెక్క, range_లెక్క + string_లెక్క);
    }

    /** @stable ICU 77 */
    USetElementIterator చివర() స్థిరరాశి {
        తిరిగి USetElementIterator(uset, range_లెక్క + string_లెక్క, range_లెక్క, range_లెక్క + string_లెక్క);
    }

అంతరంగ:
    స్థిరరాశి USet *uset;
    int32_t range_లెక్క, string_లెక్క;
};

}  // namespace U_HEADER_ONLY_NAMESPACE

#అయితే_ముగింపు  // U_SHOW_CPLUSPLUS_API || U_SHOW_CPLUSPLUS_HEADER_API

#అయితే_ముగింపు  // __USET_H__
