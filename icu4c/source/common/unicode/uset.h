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

#که_نه_تعریف_شوی __USET_H__
#تعریف_کړه __USET_H__

#ضمن "unicode/utypes.h"
#ضمن "unicode/uchar.h"

#إذا U_SHOW_CPLUSPLUS_API || U_SHOW_CPLUSPLUS_سرلیک_API
#ضمن <string>
#ضمن <string_view>
#ضمن "unicode/char16ptr.h"
#ضمن "unicode/localpointer.h"
#ضمن "unicode/utf16.h"
#د_که_پای

#که_نه_تعریف_شوی USET_DEFINED

#که_نه_تعریف_شوی U_دننه_DOXYGEN
#تعریف_کړه USET_DEFINED
#د_که_پای
/**
 * USet is the C API type corresponding to C++ class UnicodeSet.
 * Use the uset_* API to manipulate.  Create with
 * uset_open*, and destroy with uset_close.
 * @stable ICU 2.4
 */
تعريفنوع بنية USet USet;
#د_که_پای

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
تعداد {
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
    USET_جمع_کړه_CASE_MAPPINGS = 4,

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
تعريفنوع تعداد USet_پراختیا_Condition {
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
    USET_پراختیا_NOT_CONTAINED = 0,
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
    USET_پراختیا_CONTAINED = 1,
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
    USET_پراختیا_SIMPLE = 2,
#که_نه_تعریف_شوی U_HIDE_DEPRECATED_API
    /**
     * One more than the last span condition.
     * @deprecated ICU 58 The numeric value may change over time, see ICU ticket #12420.
     */
    USET_پراختیا_CONDITION_شمېر
#د_که_پای  // U_HIDE_DEPRECATED_API
} USet_پراختیا_Condition;

تعداد {
    /**
     * Capacity of USerializedSet::staticArray.
     * Enough for any single-code point set.
     * Also provides padding for nice sizeof(USerializedSet).
     * @stable ICU 2.4
     */
    USET_SERIALIZED_STATIC_ARRAY_ظرفیت=8
};

/**
 * A serialized form of a Unicode set.  Limited manipulations are
 * possible directly on a serialized set.  See below.
 * @stable ICU 2.4
 */
تعريفنوع بنية USerialized_سېټ {
    /**
     * The serialized Unicode Set.
     * @stable ICU 2.4
     */
    ثابت uint16_t *array;
    /**
     * The length of the array that contains BMP characters.
     * @stable ICU 2.4
     */
    int32_t bmp_اوږدتیا;
    /**
     * The total length of the array.
     * @stable ICU 2.4
     */
    int32_t اوږدتیا;
    /**
     * A small buffer for the array to reduce memory allocations.
     * @stable ICU 2.4
     */
    uint16_t staticArray[USET_SERIALIZED_STATIC_ARRAY_ظرفیت];
} USerialized_سېټ;

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
uset_open_تشه(فراغ);

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
uset_open(UChar32 پیل, UChar32 پای_ټکی);

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
uset_open_نمونه(ثابت UChar* نمونه, int32_t نمونه_اوږدتیا,
                 UError_کوډ* ec);

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
uset_open_نمونه_Options(ثابت UChar* نمونه, int32_t نمونه_اوږدتیا,
                 uint32_t options,
                 UError_کوډ* ec);

/**
 * Disposes of the storage used by a USet object.  This function should
 * be called exactly once for objects returned by uset_open().
 * @param set the object to dispose of
 * @stable ICU 2.4
 */
U_CAPI فراغ U_EXPORT2
uset_close(USet* سېټ);

#إذا U_SHOW_CPLUSPLUS_API

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
U_DEFINE_LOCAL_OPEN_اشاره_ګر(Local_USet_اشاره_ګر, USet, uset_close);

U_NAMESPACE_پای_ټکی

#د_که_پای

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
uset_clone(ثابت USet *سېټ);

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
uset_isFrozen(ثابت USet *سېټ);

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
U_CAPI فراغ U_EXPORT2
uset_freeze(USet *سېټ);

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
uset_cloneAsThawed(ثابت USet *سېټ);

/**
 * Causes the USet object to represent the range <code>start - end</code>.
 * If <code>start > end</code> then this USet is set to an empty range.
 * A frozen set will not be modified.
 * @param set the object to set to the given range
 * @param start first character in the set, inclusive
 * @param end last character in the set, inclusive
 * @stable ICU 3.2
 */
U_CAPI فراغ U_EXPORT2
uset_سېټ(USet* سېټ,
         UChar32 پیل, UChar32 پای_ټکی);

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
uset_apply_نمونه(USet *سېټ,
                  ثابت UChar *نمونه, int32_t نمونه_اوږدتیا,
                  uint32_t options,
                  UError_کوډ *حالت);

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
U_CAPI فراغ U_EXPORT2
uset_apply_Int_Property_ارزښت(USet* سېټ,
                           UProperty prop, int32_t ارزښت_2, UError_کوډ* ec);

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
U_CAPI فراغ U_EXPORT2
uset_applyPropertyAlias(USet* سېټ,
                        ثابت UChar *prop, int32_t prop_اوږدتیا,
                        ثابت UChar *ارزښت_2, int32_t ارزښت_اوږدتیا,
                        UError_کوډ* ec);

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
uset_resembles_نمونه(ثابت UChar *نمونه, int32_t نمونه_اوږدتیا,
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
uset_منزل_نمونه(ثابت USet* سېټ,
               UChar* پایله, int32_t پایله_ظرفیت,
               UBool escapeUnprintable,
               UError_کوډ* ec);

/**
 * Adds the given character to the given USet.  After this call,
 * uset_contains(set, c) will return true.
 * A frozen set will not be modified.
 * @param set the object to which to add the character
 * @param c the character to add
 * @stable ICU 2.4
 */
U_CAPI فراغ U_EXPORT2
uset_جمع_کړه(USet* سېټ, UChar32 c);

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
U_CAPI فراغ U_EXPORT2
uset_جمع_کړه_All(USet* سېټ, ثابت USet *additional_سېټ);

/**
 * Adds the given range of characters to the given USet.  After this call,
 * uset_contains(set, start, end) will return true.
 * A frozen set will not be modified.
 * @param set the object to which to add the character
 * @param start the first character of the range to add, inclusive
 * @param end the last character of the range to add, inclusive
 * @stable ICU 2.2
 */
U_CAPI فراغ U_EXPORT2
uset_جمع_کړه_Range(USet* سېټ, UChar32 پیل, UChar32 پای_ټکی);

/**
 * Adds the given string to the given USet.  After this call,
 * uset_containsString(set, str, strLen) will return true.
 * A frozen set will not be modified.
 * @param set the object to which to add the character
 * @param str the string to add
 * @param strLen the length of the string or -1 if null terminated.
 * @stable ICU 2.4
 */
U_CAPI فراغ U_EXPORT2
uset_جمع_کړه_String(USet* سېټ, ثابت UChar* str, int32_t strLen);

/**
 * Adds each of the characters in this string to the set. Note: "ch" => {"c", "h"}
 * If this set already contains any particular character, it has no effect on that character.
 * A frozen set will not be modified.
 * @param set the object to which to add the character
 * @param str the source string
 * @param strLen the length of the string or -1 if null terminated.
 * @stable ICU 3.4
 */
U_CAPI فراغ U_EXPORT2
uset_جمع_کړه_All_کوډ_Points(USet* سېټ, ثابت UChar *str, int32_t strLen);

/**
 * Removes the given character from the given USet.  After this call,
 * uset_contains(set, c) will return false.
 * A frozen set will not be modified.
 * @param set the object from which to remove the character
 * @param c the character to remove
 * @stable ICU 2.4
 */
U_CAPI فراغ U_EXPORT2
uset_remove(USet* سېټ, UChar32 c);

/**
 * Removes the given range of characters from the given USet.  After this call,
 * uset_contains(set, start, end) will return false.
 * A frozen set will not be modified.
 * @param set the object to which to add the character
 * @param start the first character of the range to remove, inclusive
 * @param end the last character of the range to remove, inclusive
 * @stable ICU 2.2
 */
U_CAPI فراغ U_EXPORT2
uset_removeRange(USet* سېټ, UChar32 پیل, UChar32 پای_ټکی);

/**
 * Removes the given string to the given USet.  After this call,
 * uset_containsString(set, str, strLen) will return false.
 * A frozen set will not be modified.
 * @param set the object to which to add the character
 * @param str the string to remove
 * @param strLen the length of the string or -1 if null terminated.
 * @stable ICU 2.4
 */
U_CAPI فراغ U_EXPORT2
uset_removeString(USet* سېټ, ثابت UChar* str, int32_t strLen);

/**
 * Removes EACH of the characters in this string. Note: "ch" == {"c", "h"}
 * A frozen set will not be modified.
 *
 * @param set the object to be modified
 * @param str the string
 * @param length the length of the string, or -1 if NUL-terminated
 * @stable ICU 69
 */
U_CAPI فراغ U_EXPORT2
uset_remove_All_کوډ_Points(USet *سېټ, ثابت UChar *str, int32_t اوږدتیا);

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
U_CAPI فراغ U_EXPORT2
uset_removeAll(USet* سېټ, ثابت USet* remove_سېټ);

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
U_CAPI فراغ U_EXPORT2
uset_retain(USet* سېټ, UChar32 پیل, UChar32 پای_ټکی);

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
U_CAPI فراغ U_EXPORT2
uset_retainString(USet *سېټ, ثابت UChar *str, int32_t اوږدتیا);

/**
 * Retains EACH of the characters in this string. Note: "ch" == {"c", "h"}
 * A frozen set will not be modified.
 *
 * @param set the object to be modified
 * @param str the string
 * @param length the length of the string, or -1 if NUL-terminated
 * @stable ICU 69
 */
U_CAPI فراغ U_EXPORT2
uset_retain_All_کوډ_Points(USet *سېټ, ثابت UChar *str, int32_t اوږدتیا);

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
U_CAPI فراغ U_EXPORT2
uset_retainAll(USet* سېټ, ثابت USet* retain);

/**
 * Reallocate this objects internal structures to take up the least
 * possible space, without changing this object's value.
 * A frozen set will not be modified.
 *
 * @param set the object on which to perform the compact
 * @stable ICU 3.2
 */
U_CAPI فراغ U_EXPORT2
uset_compact(USet* سېټ);

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
U_CAPI فراغ U_EXPORT2
uset_complement(USet* سېټ);

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
U_CAPI فراغ U_EXPORT2
uset_complementRange(USet *سېټ, UChar32 پیل, UChar32 پای_ټکی);

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
U_CAPI فراغ U_EXPORT2
uset_complementString(USet *سېټ, ثابت UChar *str, int32_t اوږدتیا);

/**
 * Complements EACH of the characters in this string. Note: "ch" == {"c", "h"}
 * A frozen set will not be modified.
 *
 * @param set the object to be modified
 * @param str the string
 * @param length the length of the string, or -1 if NUL-terminated
 * @stable ICU 69
 */
U_CAPI فراغ U_EXPORT2
uset_complement_All_کوډ_Points(USet *سېټ, ثابت UChar *str, int32_t اوږدتیا);

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
U_CAPI فراغ U_EXPORT2
uset_complementAll(USet* سېټ, ثابت USet* complement);

/**
 * Removes all of the elements from this set.  This set will be
 * empty after this call returns.
 * A frozen set will not be modified.
 * @param set the set
 * @stable ICU 2.4
 */
U_CAPI فراغ U_EXPORT2
uset_clear(USet* سېټ);

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
U_CAPI فراغ U_EXPORT2
uset_closeOver(USet* سېټ, int32_t ځانګړتیاوې);

/**
 * Remove all strings from this set.
 *
 * @param set the set
 * @stable ICU 4.2
 */
U_CAPI فراغ U_EXPORT2
uset_remove_All_مزي_ګانې(USet* سېټ);

/**
 * Returns true if the given USet contains no characters and no
 * strings.
 * @param set the set
 * @return true if set is empty
 * @stable ICU 2.4
 */
U_CAPI UBool U_EXPORT2
uset_is_تشه(ثابت USet* سېټ);

/**
 * @param set the set
 * @return true if this set contains multi-character strings or the empty string.
 * @stable ICU 70
 */
U_CAPI UBool U_EXPORT2
uset_has_مزي_ګانې(ثابت USet *سېټ);

/**
 * Returns true if the given USet contains the given character.
 * This function works faster with a frozen set.
 * @param set the set
 * @param c The codepoint to check for within the set
 * @return true if set contains c
 * @stable ICU 2.4
 */
U_CAPI UBool U_EXPORT2
uset_لري(ثابت USet* سېټ, UChar32 c);

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
uset_لري_Range(ثابت USet* سېټ, UChar32 پیل, UChar32 پای_ټکی);

/**
 * Returns true if the given USet contains the given string.
 * @param set the set
 * @param str the string
 * @param strLen the length of the string or -1 if null terminated.
 * @return true if set contains str
 * @stable ICU 2.4
 */
U_CAPI UBool U_EXPORT2
uset_لري_String(ثابت USet* سېټ, ثابت UChar* str, int32_t strLen);

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
uset_شاخص_د(ثابت USet* سېټ, UChar32 c);

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
uset_charAt(ثابت USet* سېټ, int32_t char_شاخص);

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
uset_اندازه(ثابت USet* سېټ);

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
uset_get_Range_شمېر(ثابت USet *سېټ);

/**
 * @param set the set
 * @return the number of strings in this set.
 * @stable ICU 76
 * @see uset_getRangeCount
 * @see uset_getItemCount
 * @see uset_size
 */
U_CAPI int32_t U_EXPORT2
uset_get_String_شمېر(ثابت USet *سېټ);

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
U_CAPI ثابت UChar* U_EXPORT2
uset_getString(ثابت USet* سېټ U_LIFETIME_BOUND, int32_t شاخص, int32_t* p_اوږدتیا);

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
uset_get_توکی_شمېر(ثابت USet* سېټ);

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
uset_get_توکی(ثابت USet* سېټ, int32_t توکی_شاخص,
             UChar32* پیل, UChar32* پای_ټکی,
             UChar* str, int32_t str_ظرفیت,
             UError_کوډ* ec);

/**
 * Returns true if set1 contains all the characters and strings
 * of set2. It answers the question, 'Is set1 a superset of set2?'
 * @param set1 set to be checked for containment
 * @param set2 set to be checked for containment
 * @return true if the test condition is met
 * @stable ICU 3.2
 */
U_CAPI UBool U_EXPORT2
uset_لري_All(ثابت USet* set1, ثابت USet* set2);

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
uset_لري_All_کوډ_Points(ثابت USet* سېټ, ثابت UChar *str, int32_t strLen);

/**
 * Returns true if set1 contains none of the characters and strings
 * of set2. It answers the question, 'Is set1 a disjoint set of set2?'
 * @param set1 set to be checked for containment
 * @param set2 set to be checked for containment
 * @return true if the test condition is met
 * @stable ICU 3.2
 */
U_CAPI UBool U_EXPORT2
uset_لري_None(ثابت USet* set1, ثابت USet* set2);

/**
 * Returns true if set1 contains some of the characters and strings
 * of set2. It answers the question, 'Does set1 and set2 have an intersection?'
 * @param set1 set to be checked for containment
 * @param set2 set to be checked for containment
 * @return true if the test condition is met
 * @stable ICU 3.2
 */
U_CAPI UBool U_EXPORT2
uset_لري_Some(ثابت USet* set1, ثابت USet* set2);

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
uset_پراختیا(ثابت USet *سېټ, ثابت UChar *s, int32_t اوږدتیا, USet_پراختیا_Condition پراختیا_Condition);

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
uset_پراختیا_شا(ثابت USet *سېټ, ثابت UChar *s, int32_t اوږدتیا, USet_پراختیا_Condition پراختیا_Condition);

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
uset_پراختیا_UTF8(ثابت USet *سېټ, ثابت حرف *s, int32_t اوږدتیا, USet_پراختیا_Condition پراختیا_Condition);

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
uset_پراختیا_شا_UTF8(ثابت USet *سېټ, ثابت حرف *s, int32_t اوږدتیا, USet_پراختیا_Condition پراختیا_Condition);

/**
 * Returns true if set1 contains all of the characters and strings
 * of set2, and vis versa. It answers the question, 'Is set1 equal to set2?'
 * @param set1 set to be checked for containment
 * @param set2 set to be checked for containment
 * @return true if the test condition is met
 * @stable ICU 3.2
 */
U_CAPI UBool U_EXPORT2
uset_equals(ثابت USet* set1, ثابت USet* set2);

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
uset_serialize(ثابت USet* سېټ, uint16_t* dest, int32_t dest_ظرفیت, UError_کوډ* p_Error_کوډ);

/**
 * Given a serialized array, fill in the given serialized set object.
 * @param fillSet pointer to result
 * @param src pointer to start of array
 * @param srcLength length of array
 * @return true if the given array is valid, otherwise false
 * @stable ICU 2.4
 */
U_CAPI UBool U_EXPORT2
uset_get_Serialized_سېټ(USerialized_سېټ* fill_سېټ, ثابت uint16_t* src, int32_t src_اوږدتیا);

/**
 * Set the USerializedSet to contain the given character (and nothing
 * else).
 * @param fillSet pointer to result
 * @param c The codepoint to set
 * @stable ICU 2.4
 */
U_CAPI فراغ U_EXPORT2
uset_سېټ_Serialized_منزل_One(USerialized_سېټ* fill_سېټ, UChar32 c);

/**
 * Returns true if the given USerializedSet contains the given
 * character.
 * @param set the serialized set
 * @param c The codepoint to check for within the set
 * @return true if set contains c
 * @stable ICU 2.4
 */
U_CAPI UBool U_EXPORT2
uset_serialized_لري(ثابت USerialized_سېټ* سېټ, UChar32 c);

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
uset_get_Serialized_Range_شمېر(ثابت USerialized_سېټ* سېټ);

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
uset_getSerializedRange(ثابت USerialized_سېټ* سېټ, int32_t range_شاخص,
                        UChar32* p_پیل, UChar32* p_پای_ټکی);

#إذا U_SHOW_CPLUSPLUS_API || U_SHOW_CPLUSPLUS_سرلیک_API

namespace U_سرلیک_ONLY_NAMESPACE {

// Note: Not U_COMMON_API, and not a subclass of UMemory, because this is a header-only class,
// not intended to be used via export from the ICU DLL.

/**
 * Iterator returned by USetCodePoints.
 * @stable ICU 76
 */
class USet_کوډ_ټکی_Iterator {
عام:
    /** @stable ICU 76 */
    USet_کوډ_ټکی_Iterator(ثابت USet_کوډ_ټکی_Iterator &other) = افتراضي;

    /** @stable ICU 76 */
    bool چلوونکی==(ثابت USet_کوډ_ټکی_Iterator &other) ثابت {
        // No need to compare rangeCount & end given private constructor
        // and assuming we don't compare iterators across the set being modified.
        // And comparing rangeIndex is redundant with comparing c.
        // We might even skip comparing uset.
        // Unless we want operator==() to be "correct" for more than iteration.
        أرجع uset == other.uset && c == other.c;
    }

    /** @stable ICU 76 */
    bool چلوونکی!=(ثابت USet_کوډ_ټکی_Iterator &other) ثابت { أرجع !چلوونکی==(other); }

    /** @stable ICU 76 */
    UChar32 چلوونکی*() ثابت { أرجع c; }

    /**
     * Pre-increment.
     * @stable ICU 76
     */
    USet_کوډ_ټکی_Iterator& چلوونکی++() U_LIFETIME_BOUND {
        إذا (c < پای_ټکی) {
            ++c;
        } وإلا إذا (range_شاخص < range_شمېر) {
            UError_کوډ error_کوډ = U_ZERO_ERROR;
            int32_t پایله = uset_get_توکی(uset, range_شاخص, &c, &پای_ټکی, nullptr, 0, &error_کوډ);
            إذا (U_SUCCESS(error_کوډ) && پایله == 0) {
                ++range_شاخص;
            } وإلا {
                c = پای_ټکی = U_SENTINEL;
            }
        } وإلا {
            c = پای_ټکی = U_SENTINEL;
        }
        أرجع *this;
    }

    /**
     * Post-increment.
     * @stable ICU 76
     */
    USet_کوډ_ټکی_Iterator چلوونکی++(صحيح) {
        USet_کوډ_ټکی_Iterator پایله(*this);
        چلوونکی++();
        أرجع پایله;
    }

خاص:
    ملګری class USet_کوډ_Points;

    USet_کوډ_ټکی_Iterator(ثابت USet* pUset U_LIFETIME_BOUND,
                          int32_t n_Range_شاخص,
                          int32_t n_Range_شمېر)
            : uset(pUset), range_شاخص(n_Range_شاخص), range_شمېر(n_Range_شمېر),
                c(U_SENTINEL), پای_ټکی(U_SENTINEL) {
        // Fetch the first range.
        چلوونکی++();
    }

    ثابت USet *uset;
    int32_t range_شاخص;
    int32_t range_شمېر;
    UChar32 c, پای_ټکی;
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
class USet_کوډ_Points {
عام:
    /**
     * Constructs a C++ "range" object over the code points of the USet.
     * @stable ICU 76
     */
    USet_کوډ_Points(ثابت USet* pUset U_LIFETIME_BOUND)
            : uset(pUset), range_شمېر(uset_get_Range_شمېر(pUset)) {}

    /** @stable ICU 76 */
    USet_کوډ_Points(ثابت USet_کوډ_Points &other) = افتراضي;

    /** @stable ICU 76 */
    USet_کوډ_ټکی_Iterator begin() ثابت {
        أرجع USet_کوډ_ټکی_Iterator(uset, 0, range_شمېر);
    }

    /** @stable ICU 76 */
    USet_کوډ_ټکی_Iterator پای_ټکی() ثابت {
        أرجع USet_کوډ_ټکی_Iterator(uset, range_شمېر, range_شمېر);
    }

خاص:
    ثابت USet *uset;
    int32_t range_شمېر;
};

/**
 * A contiguous range of code points in a USet/UnicodeSet.
 * Returned by USetRangeIterator which is returned by USetRanges.
 * Both the rangeStart and rangeEnd are in the range.
 * (end() returns an iterator corresponding to rangeEnd+1.)
 * @stable ICU 76
 */
بنية کوډ_ټکی_Range {
    /** @stable ICU 76 */
    بنية iterator {
        /** @stable ICU 76 */
        iterator(UChar32 aC) : c(aC) {}

        /** @stable ICU 76 */
        bool چلوونکی==(ثابت iterator &other) ثابت { أرجع c == other.c; }
        /** @stable ICU 76 */
        bool چلوونکی!=(ثابت iterator &other) ثابت { أرجع !چلوونکی==(other); }

        /** @stable ICU 76 */
        UChar32 چلوونکی*() ثابت { أرجع c; }

        /**
         * Pre-increment.
         * @stable ICU 76
         */
        iterator& چلوونکی++() U_LIFETIME_BOUND {
            ++c;
            أرجع *this;
        }

        /**
         * Post-increment.
         * @stable ICU 76
         */
        iterator چلوونکی++(صحيح) {
            أرجع c++;
        }

        /**
         * The current code point in the range.
         * @stable ICU 76
         */
        UChar32 c;
    };

    /** @stable ICU 76 */
    کوډ_ټکی_Range(UChar32 پیل, UChar32 پای_ټکی) : range_پیل(پیل), range_پای_ټکی(پای_ټکی) {}
    /** @stable ICU 76 */
    کوډ_ټکی_Range(ثابت کوډ_ټکی_Range &other) = افتراضي;
    /** @stable ICU 76 */
    size_t اندازه_2() ثابت { أرجع (range_پای_ټکی + 1) - range_پیل; }
    /** @stable ICU 76 */
    iterator begin() ثابت { أرجع range_پیل; }
    /** @stable ICU 76 */
    iterator پای_ټکی() ثابت { أرجع range_پای_ټکی + 1; }

    /**
     * Start of a USet/UnicodeSet range of code points.
     * @stable ICU 76
     */
    UChar32 range_پیل;
    /**
     * Inclusive end of a USet/UnicodeSet range of code points.
     * @stable ICU 76
     */
    UChar32 range_پای_ټکی;
};

/**
 * Iterator returned by USetRanges.
 * @stable ICU 76
 */
class USetRangeIterator {
عام:
    /** @stable ICU 76 */
    USetRangeIterator(ثابت USetRangeIterator &other) = افتراضي;

    /** @stable ICU 76 */
    bool چلوونکی==(ثابت USetRangeIterator &other) ثابت {
        // No need to compare rangeCount given private constructor
        // and assuming we don't compare iterators across the set being modified.
        // We might even skip comparing uset.
        // Unless we want operator==() to be "correct" for more than iteration.
        أرجع uset == other.uset && range_شاخص == other.range_شاخص;
    }

    /** @stable ICU 76 */
    bool چلوونکی!=(ثابت USetRangeIterator &other) ثابت { أرجع !چلوونکی==(other); }

    /** @stable ICU 76 */
    کوډ_ټکی_Range چلوونکی*() ثابت {
        إذا (range_شاخص < range_شمېر) {
            UChar32 پیل, پای_ټکی;
            UError_کوډ error_کوډ = U_ZERO_ERROR;
            int32_t پایله = uset_get_توکی(uset, range_شاخص, &پیل, &پای_ټکی, nullptr, 0, &error_کوډ);
            إذا (U_SUCCESS(error_کوډ) && پایله == 0) {
                أرجع کوډ_ټکی_Range(پیل, پای_ټکی);
            }
        }
        أرجع کوډ_ټکی_Range(U_SENTINEL, U_SENTINEL);
    }

    /**
     * Pre-increment.
     * @stable ICU 76
     */
    USetRangeIterator& چلوونکی++() U_LIFETIME_BOUND {
        ++range_شاخص;
        أرجع *this;
    }

    /**
     * Post-increment.
     * @stable ICU 76
     */
    USetRangeIterator چلوونکی++(صحيح) {
        USetRangeIterator پایله(*this);
        ++range_شاخص;
        أرجع پایله;
    }

خاص:
    ملګری class USetRanges;

    USetRangeIterator(ثابت USet* pUset U_LIFETIME_BOUND, int32_t n_Range_شاخص, int32_t n_Range_شمېر)
            : uset(pUset), range_شاخص(n_Range_شاخص), range_شمېر(n_Range_شمېر) {}

    ثابت USet *uset;
    int32_t range_شاخص;
    int32_t range_شمېر;
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
عام:
    /**
     * Constructs a C++ "range" object over the code point ranges of the USet.
     * @stable ICU 76
     */
    USetRanges(ثابت USet* pUset U_LIFETIME_BOUND)
            : uset(pUset), range_شمېر(uset_get_Range_شمېر(pUset)) {}

    /** @stable ICU 76 */
    USetRanges(ثابت USetRanges &other) = افتراضي;

    /** @stable ICU 76 */
    USetRangeIterator begin() ثابت {
        أرجع USetRangeIterator(uset, 0, range_شمېر);
    }

    /** @stable ICU 76 */
    USetRangeIterator پای_ټکی() ثابت {
        أرجع USetRangeIterator(uset, range_شمېر, range_شمېر);
    }

خاص:
    ثابت USet *uset;
    int32_t range_شمېر;
};

/**
 * Iterator returned by USetStrings.
 * @stable ICU 76
 */
class USetStringIterator {
عام:
    /** @stable ICU 76 */
    USetStringIterator(ثابت USetStringIterator &other) = افتراضي;

    /** @stable ICU 76 */
    bool چلوونکی==(ثابت USetStringIterator &other) ثابت {
        // No need to compare count given private constructor
        // and assuming we don't compare iterators across the set being modified.
        // We might even skip comparing uset.
        // Unless we want operator==() to be "correct" for more than iteration.
        أرجع uset == other.uset && شاخص == other.شاخص;
    }

    /** @stable ICU 76 */
    bool چلوونکی!=(ثابت USetStringIterator &other) ثابت { أرجع !چلوونکی==(other); }

    /** @stable ICU 76 */
    std::u16string_view چلوونکی*() ثابت {
        إذا (شاخص < شمېر) {
            int32_t اوږدتیا;
            ثابت UChar *uchars = uset_getString(uset, شاخص, &اوږدتیا);
            // assert uchars != nullptr;
            أرجع {uprv_char16PtrFromUChar(uchars), static_cast<size_t>(اوږدتیا)};
        }
        أرجع {};
    }

    /**
     * Pre-increment.
     * @stable ICU 76
     */
    USetStringIterator& چلوونکی++() U_LIFETIME_BOUND {
        ++شاخص;
        أرجع *this;
    }

    /**
     * Post-increment.
     * @stable ICU 76
     */
    USetStringIterator چلوونکی++(صحيح) {
        USetStringIterator پایله(*this);
        ++شاخص;
        أرجع پایله;
    }

خاص:
    ملګری class USet_مزي_ګانې;

    USetStringIterator(ثابت USet* pUset U_LIFETIME_BOUND, int32_t n_شاخص, int32_t n_شمېر)
            : uset(pUset), شاخص(n_شاخص), شمېر(n_شمېر) {}

    ثابت USet *uset;
    int32_t شاخص;
    int32_t شمېر;
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
class USet_مزي_ګانې {
عام:
    /**
     * Constructs a C++ "range" object over the strings of the USet.
     * @stable ICU 76
     */
    USet_مزي_ګانې(ثابت USet* pUset U_LIFETIME_BOUND)
            : uset(pUset), شمېر(uset_get_String_شمېر(pUset)) {}

    /** @stable ICU 76 */
    USet_مزي_ګانې(ثابت USet_مزي_ګانې &other) = افتراضي;

    /** @stable ICU 76 */
    USetStringIterator begin() ثابت {
        أرجع USetStringIterator(uset, 0, شمېر);
    }

    /** @stable ICU 76 */
    USetStringIterator پای_ټکی() ثابت {
        أرجع USetStringIterator(uset, شمېر, شمېر);
    }

خاص:
    ثابت USet *uset;
    int32_t شمېر;
};

/**
 * Iterator returned by USetElements.
 * @stable ICU 77
 */
class USetElementIterator {
عام:
    /** @stable ICU 77 */
    USetElementIterator(ثابت USetElementIterator &other) = افتراضي;

    /** @stable ICU 77 */
    bool چلوونکی==(ثابت USetElementIterator &other) ثابت {
        // No need to compare rangeCount & end given private constructor
        // and assuming we don't compare iterators across the set being modified.
        // We might even skip comparing uset.
        // Unless we want operator==() to be "correct" for more than iteration.
        أرجع uset == other.uset && c == other.c && شاخص == other.شاخص;
    }

    /** @stable ICU 77 */
    bool چلوونکی!=(ثابت USetElementIterator &other) ثابت { أرجع !چلوونکی==(other); }

    /** @stable ICU 77 */
    std::u16string چلوونکی*() ثابت {
        إذا (c >= 0) {
            أرجع c <= 0xffff ?
                std::u16string({static_cast<char16_t>(c)}) :
                std::u16string({U16_LEAD(c), U16_TRAIL(c)});
        } وإلا إذا (شاخص < ټول_شمېر) {
            int32_t اوږدتیا;
            ثابت UChar *uchars = uset_getString(uset, شاخص - range_شمېر, &اوږدتیا);
            // assert uchars != nullptr;
            أرجع {uprv_char16PtrFromUChar(uchars), static_cast<size_t>(اوږدتیا)};
        } وإلا {
            أرجع {};
        }
    }

    /**
     * Pre-increment.
     * @stable ICU 77
     */
    USetElementIterator& چلوونکی++() U_LIFETIME_BOUND {
        إذا (c < پای_ټکی) {
            ++c;
        } وإلا إذا (شاخص < range_شمېر) {
            UError_کوډ error_کوډ = U_ZERO_ERROR;
            int32_t پایله = uset_get_توکی(uset, شاخص, &c, &پای_ټکی, nullptr, 0, &error_کوډ);
            إذا (U_SUCCESS(error_کوډ) && پایله == 0) {
                ++شاخص;
            } وإلا {
                c = پای_ټکی = U_SENTINEL;
            }
        } وإلا إذا (c >= 0) {
            // assert index == rangeCount;
            // Switch from the last range to the first string.
            c = پای_ټکی = U_SENTINEL;
        } وإلا {
            ++شاخص;
        }
        أرجع *this;
    }

    /**
     * Post-increment.
     * @stable ICU 77
     */
    USetElementIterator چلوونکی++(صحيح) {
        USetElementIterator پایله(*this);
        چلوونکی++();
        أرجع پایله;
    }

خاص:
    ملګری class USetElements;

    USetElementIterator(ثابت USet* pUset U_LIFETIME_BOUND,
                        int32_t n_شاخص,
                        int32_t n_Range_شمېر,
                        int32_t n_ټول_شمېر)
            : uset(pUset), شاخص(n_شاخص), range_شمېر(n_Range_شمېر), ټول_شمېر(n_ټول_شمېر),
                c(U_SENTINEL), پای_ټکی(U_SENTINEL) {
        إذا (شاخص < range_شمېر) {
            // Fetch the first range.
            چلوونکی++();
        }
        // Otherwise don't move beyond the (index - rangeCount)-th string.
    }

    ثابت USet *uset;
    int32_t شاخص;
    /** Number of UnicodeSet/USet code point ranges. */
    int32_t range_شمېر;
    /**
     * Number of code point ranges plus number of strings.
     * index starts from 0, counts ranges while less than rangeCount,
     * then counts strings while at least rangeCount and less than totalCount.
     *
     * Note that totalCount is the same as uset_getItemCount(), but usually
     * smaller than the number of elements returned by this iterator
     * because we return each code point of each range.
     */
    int32_t ټول_شمېر;
    UChar32 c, پای_ټکی;
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
عام:
    /**
     * Constructs a C++ "range" object over all of the elements of the USet.
     * @stable ICU 77
     */
    USetElements(ثابت USet* pUset U_LIFETIME_BOUND)
        : uset(pUset), range_شمېر(uset_get_Range_شمېر(pUset)),
            string_شمېر(uset_get_String_شمېر(pUset)) {}

    /** @stable ICU 77 */
    USetElements(ثابت USetElements &other) = افتراضي;

    /** @stable ICU 77 */
    USetElementIterator begin() ثابت {
        أرجع USetElementIterator(uset, 0, range_شمېر, range_شمېر + string_شمېر);
    }

    /** @stable ICU 77 */
    USetElementIterator پای_ټکی() ثابت {
        أرجع USetElementIterator(uset, range_شمېر + string_شمېر, range_شمېر, range_شمېر + string_شمېر);
    }

خاص:
    ثابت USet *uset;
    int32_t range_شمېر, string_شمېر;
};

}  // namespace U_HEADER_ONLY_NAMESPACE

#د_که_پای  // U_SHOW_CPLUSPLUS_API || U_SHOW_CPLUSPLUS_HEADER_API

#د_که_پای  // __USET_H__
