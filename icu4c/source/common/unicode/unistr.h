// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
**********************************************************************
*   Copyright (C) 1998-2016, International Business Machines
*   Corporation and others.  All Rights Reserved.
**********************************************************************
*
* File unistr.h
*
* Modification History:
*
*   Date        Name        Description
*   09/25/98    stephen     Creation.
*   11/11/98    stephen     Changed per 11/9 code review.
*   04/20/99    stephen     Overhauled per 4/16 code review.
*   11/18/99    aliu        Made to inherit from Replaceable.  Added method
*                           handleReplaceBetween(); other methods unchanged.
*   06/25/01    grhoten     Remove dependency on iostream.
******************************************************************************
*/

#جے_بنا_تعریف UNISTR_H
#تعریف_کرو UNISTR_H

/**
 * \file
 * \brief C++ API: Unicode String
 */

#ضمن "unicode/utypes.h"

#إذا U_SHOW_CPLUSPLUS_API

#ضمن <cstddef>
#ضمن <string_view>
#ضمن "unicode/char16ptr.h"
#ضمن "unicode/rep.h"
#ضمن "unicode/std_string.h"
#ضمن "unicode/stringpiece.h"
#ضمن "unicode/bytestream.h"

بنية UConverter;          // unicode/ucnv.h

#جے_بنا_تعریف USTRING_H
/**
 * \ingroup ustring_ustrlen
 * @param s Pointer to sequence of UChars.
 * @return Length of sequence.
 */
U_CAPI int32_t U_EXPORT2 u_strlen(ثابت UChar *s);
#جے_انت

U_NAMESPACE_BEGIN

#إذا !UCONFIG_NO_BREAK_ITERATION
class BreakIterator;        // unicode/brkiter.h
#جے_انت
class Edits;

U_NAMESPACE_سرا

// Not #ifndef U_HIDE_INTERNAL_API because UnicodeString needs the UStringCaseMapper.
/**
 * Internal string case mapping function type.
 * All error checking must be done.
 * src and dest must not overlap.
 * @internal
 */
تعريفنوع int32_t U_CALLCONV
UStringCaseMapper(int32_t caseLocale, uint32_t options,
#إذا !UCONFIG_NO_BREAK_ITERATION
                  icu::BreakIterator *iter,
#جے_انت
                  char16_t *dest, int32_t dest_سمرتھا,
                  ثابت char16_t *src, int32_t src_لمائی,
                  icu::Edits *edits,
                  UError_کوڈ &error_کوڈ);

U_NAMESPACE_BEGIN

class Locale;               // unicode/locid.h
class StringCharacterIterator;
class UnicodeStringAppendable;  // unicode/appendable.h

/* The <iostream> include has been moved to unicode/ustream.h */

/**
 * Constant to be used in the UnicodeString(char *, int32_t, EInvariant) constructor
 * which constructs a Unicode string from an invariant-character char * string.
 * About invariant characters see utypes.h.
 * This constructor has no runtime dependency on conversion code and is
 * therefore recommended over ones taking a charset name string
 * (where the empty string "" indicates invariant-character conversion).
 *
 * @stable ICU 3.2
 */
#تعریف_کرو US_INV icu::UnicodeString::kInvariant

/**
 * \def UNICODE_STRING
 * Obsolete macro approximating UnicodeString literals.
 *
 * Prior to the availability of C++11 and u"UTF-16 string literals",
 * this macro was provided for portability and efficiency when
 * initializing UnicodeStrings from literals.
 *
 * Since C++17 and ICU 76, you can use UTF-16 string literals with compile-time
 * length determination:
 * \code
 * UnicodeString str(u"literal");
 * if (str == u"other literal") { ... }
 * \endcode
 *
 * The string parameter must be a C string literal.
 * The length of the string, not including the terminating
 * `NUL`, must be specified as a constant.
 * @stable ICU 2.0
 */
#إذا !U_CHAR16_IS_TYPEDEF
# define UNICODE_STRING(cs, لمائی_4) icu::UnicodeString(true, u ## cs, لمائی_4)
#وإلا
# define UNICODE_STRING(cs, لمائی_4) icu::UnicodeString(true, (ثابت char16_t*)u ## cs, لمائی_4)
#جے_انت

/**
 * Unicode String literals in C++.
 * Obsolete macro approximating UnicodeString literals.
 * See UNICODE_STRING.
 *
 * The string parameter must be a C string literal.
 * @stable ICU 2.0
 * @see UNICODE_STRING
 */
#تعریف_کرو UNICODE_STRING_SIMPLE(cs) UNICODE_STRING(cs, -1)

/**
 * \def UNISTR_FROM_CHAR_EXPLICIT
 * This can be defined to be empty or "explicit".
 * If explicit, then the UnicodeString(char16_t) and UnicodeString(UChar32)
 * constructors are marked as explicit, preventing their inadvertent use.
 * @stable ICU 49
 */
#جے_بنا_تعریف UNISTR_FROM_CHAR_EXPLICIT
# إذا defined(U_COMBINED_IMPLEMENTATION) || defined(U_COMMON_IMPLEMENTATION) || defined(U_I18N_IMPLEMENTATION) || defined(U_IO_IMPLEMENTATION)
    // Auto-"explicit" in ICU library code.
#   define UNISTR_FROM_CHAR_EXPLICIT explicit
# وإلا
    // Empty by default for source code compatibility.
#   define UNISTR_FROM_CHAR_EXPLICIT
# endif
#جے_انت

/**
 * \def UNISTR_FROM_STRING_EXPLICIT
 * This can be defined to be empty or "explicit".
 * If explicit, then the UnicodeString(const char *) and UnicodeString(const char16_t *)
 * constructors are marked as explicit, preventing their inadvertent use.
 *
 * In particular, this helps prevent accidentally depending on ICU conversion code
 * by passing a string literal into an API with a const UnicodeString & parameter.
 * @stable ICU 49
 */
#جے_بنا_تعریف UNISTR_FROM_STRING_EXPLICIT
# إذا defined(U_COMBINED_IMPLEMENTATION) || defined(U_COMMON_IMPLEMENTATION) || defined(U_I18N_IMPLEMENTATION) || defined(U_IO_IMPLEMENTATION)
    // Auto-"explicit" in ICU library code.
#   define UNISTR_FROM_STRING_EXPLICIT explicit
# وإلا
    // Empty by default for source code compatibility.
#   define UNISTR_FROM_STRING_EXPLICIT
# endif
#جے_انت

/**
 * \def UNISTR_OBJECT_SIZE
 * Desired sizeof(UnicodeString) in bytes.
 * It should be a multiple of sizeof(pointer) to avoid unusable space for padding.
 * The object size may want to be a multiple of 16 bytes,
 * which is a common granularity for heap allocation.
 *
 * Any space inside the object beyond sizeof(vtable pointer) + 2
 * is available for storing short strings inside the object.
 * The bigger the object, the longer a string that can be stored inside the object,
 * without additional heap allocation.
 *
 * Depending on a platform's pointer size, pointer alignment requirements,
 * and struct padding, the compiler will usually round up sizeof(UnicodeString)
 * to 4 * sizeof(pointer) (or 3 * sizeof(pointer) for P128 data models),
 * to hold the fields for heap-allocated strings.
 * Such a minimum size also ensures that the object is easily large enough
 * to hold at least 2 char16_ts, for one supplementary code point (U16_MAX_LENGTH).
 *
 * sizeof(UnicodeString) >= 48 should work for all known platforms.
 *
 * For example, on a 64-bit machine where sizeof(vtable pointer) is 8,
 * sizeof(UnicodeString) = 64 would leave space for
 * (64 - sizeof(vtable pointer) - 2) / U_SIZEOF_UCHAR = (64 - 8 - 2) / 2 = 27
 * char16_ts stored inside the object.
 *
 * The minimum object size on a 64-bit machine would be
 * 4 * sizeof(pointer) = 4 * 8 = 32 bytes,
 * and the internal buffer would hold up to 11 char16_ts in that case.
 *
 * @see U16_MAX_LENGTH
 * @stable ICU 56
 */
#جے_بنا_تعریف UNISTR_OBJECT_ناپ
# define UNISTR_OBJECT_ناپ 64
#جے_انت

/**
 * UnicodeString is a string class that stores Unicode characters directly and provides
 * similar functionality as the Java String and StringBuffer/StringBuilder classes.
 * It is a concrete implementation of the abstract class Replaceable (for transliteration).
 *
 * The UnicodeString equivalent of std::string’s clear() is remove().
 *
 * Starting with ICU 78, a UnicodeString is a C++ "range" of char16_t code units.
 * utfStringCodePoints() and unsafeUTFStringCodePoints() can be used to iterate over
 * the code points.
 *
 * A UnicodeString may "alias" an external array of characters
 * (that is, point to it, rather than own the array)
 * whose lifetime must then at least match the lifetime of the aliasing object.
 * This aliasing may be preserved when returning a UnicodeString by value,
 * depending on the compiler and the function implementation,
 * via Return Value Optimization (RVO) or the move assignment operator.
 * (However, the copy assignment operator does not preserve aliasing.)
 * For details see the description of storage models at the end of the class API docs
 * and in the User Guide chapter linked from there.
 *
 * The UnicodeString class is not suitable for subclassing.
 *
 * For an overview of Unicode strings in C and C++ see the
 * [User Guide Strings chapter](https://unicode-org.github.io/icu/userguide/strings#strings-in-cc).
 *
 * In ICU, a Unicode string consists of 16-bit Unicode *code units*.
 * A Unicode character may be stored with either one code unit
 * (the most common case) or with a matched pair of special code units
 * ("surrogates"). The data type for code units is char16_t.
 * For single-character handling, a Unicode character code *point* is a value
 * in the range 0..0x10ffff. ICU uses the UChar32 type for code points.
 *
 * Indexes and offsets into and lengths of strings always count code units, not code points.
 * This is the same as with multi-byte char* strings in traditional string handling.
 * Operations on partial strings typically do not test for code point boundaries.
 * If necessary, the user needs to take care of such boundaries by testing for the code unit
 * values or by using functions like
 * UnicodeString::getChar32Start() and UnicodeString::getChar32Limit()
 * (or, in C, the equivalent macros U16_SET_CP_START() and U16_SET_CP_LIMIT(), see utf.h).
 *
 * UnicodeString methods are more lenient with regard to input parameter values
 * than other ICU APIs. In particular:
 * - If indexes are out of bounds for a UnicodeString object
 *   (< 0 or > length()) then they are "pinned" to the nearest boundary.
 * - If the buffer passed to an insert/append/replace operation is owned by the
 *   target object, e.g., calling str.append(str), an extra copy may take place
 *   to ensure safety.
 * - If primitive string pointer values (e.g., const char16_t * or char *)
 *   for input strings are nullptr, then those input string parameters are treated
 *   as if they pointed to an empty string.
 *   However, this is *not* the case for char * parameters for charset names
 *   or other IDs.
 * - Most UnicodeString methods do not take a UErrorCode parameter because
 *   there are usually very few opportunities for failure other than a shortage
 *   of memory, error codes in low-level C++ string methods would be inconvenient,
 *   and the error code as the last parameter (ICU convention) would prevent
 *   the use of default parameter values.
 *   Instead, such methods set the UnicodeString into a "bogus" state
 *   (see isBogus()) if an error occurs.
 *
 * In string comparisons, two UnicodeString objects that are both "bogus"
 * compare equal (to be transitive and prevent endless loops in sorting),
 * and a "bogus" string compares less than any non-"bogus" one.
 *
 * Const UnicodeString methods are thread-safe. Multiple threads can use
 * const methods on the same UnicodeString object simultaneously,
 * but non-const methods must not be called concurrently (in multiple threads)
 * with any other (const or non-const) methods.
 *
 * Similarly, const UnicodeString & parameters are thread-safe.
 * One object may be passed in as such a parameter concurrently in multiple threads.
 * This includes the const UnicodeString & parameters for
 * copy construction, assignment, and cloning.
 *
 * UnicodeString uses several storage methods.
 * String contents can be stored inside the UnicodeString object itself,
 * in an allocated and shared buffer, or in an outside buffer that is "aliased".
 * Most of this is done transparently, but careful aliasing in particular provides
 * significant performance improvements.
 * Also, the internal buffer is accessible via special functions.
 * For details see the
 * [User Guide Strings chapter](https://unicode-org.github.io/icu/userguide/strings#maximizing-performance-with-the-unicodestring-storage-model).
 *
 * @see utf.h
 * @see utfiterator.h
 * @see utfStringCodePoints
 * @see unsafeUTFStringCodePoints
 * @see CharacterIterator
 * @stable ICU 2.0
 */
class U_COMMON_API UnicodeString : عام Replaceable
{
عام:
  /** C++ boilerplate @internal */
  using قدر_type = char16_t;

  /**
   * Constant to be used in the UnicodeString(char *, int32_t, EInvariant) constructor
   * which constructs a Unicode string from an invariant-character char * string.
   * Use the macro US_INV instead of the full qualification for this value.
   *
   * @see US_INV
   * @stable ICU 3.2
   */
  تعداد EInvariant {
    /**
     * @see EInvariant
     * @stable ICU 3.2
     */
    kInvariant
  };

  //========================================
  // Read-only operations
  //========================================

  /* Comparison - bitwise only - for international comparison use collation */

  /**
   * Equality operator. Performs only bitwise comparison.
   * @param text The UnicodeString to compare to this one.
   * @return true if `text` contains the same characters as this one,
   * false otherwise.
   * @stable ICU 2.0
   */
  مضمن bool عامل== (ثابت UnicodeString& متن) ثابت;

  /**
   * Equality operator. Performs only bitwise comparison with `text`
   * which is, or which is implicitly convertible to,
   * a std::u16string_view or (if U_SIZEOF_WCHAR_T==2) std::wstring_view.
   *
   * For performance, you can use UTF-16 string literals with compile-time
   * length determination:
   * \code
   * UnicodeString str = ...;
   * if (str == u"literal") { ... }
   * \endcode
   * @param text The string view to compare to this string.
   * @return true if `text` contains the same characters as this one, false otherwise.
   * @stable ICU 76
   */
  سانچا<typename S, typename = std::enable_if_t<Convertible_منزل_U16_String_View<S>>>
  مضمن bool عامل==(ثابت S &متن) ثابت {
    std::u16string_view sv(اندرونی_2::منزل_U16_String_View(متن));
    uint32_t len;  // unsigned to avoid a compiler warning
    أرجع !isBogus() && (len = لمائی()) == sv.لمائی() && doEquals(sv.انکڑے(), len);
  }

  /**
   * Inequality operator. Performs only bitwise comparison.
   * @param text The UnicodeString to compare to this one.
   * @return false if `text` contains the same characters as this one,
   * true otherwise.
   * @stable ICU 2.0
   */
  مضمن bool عامل!= (ثابت UnicodeString& متن) ثابت;

  /**
   * Inequality operator. Performs only bitwise comparison with `text`
   * which is, or which is implicitly convertible to,
   * a std::u16string_view or (if U_SIZEOF_WCHAR_T==2) std::wstring_view.
   *
   * For performance, you can use std::u16string_view literals with compile-time
   * length determination:
   * \code
   * #include &lt;string_view&gt;
   * using namespace std::string_view_literals;
   * UnicodeString str = ...;
   * if (str != u"literal"sv) { ... }
   * \endcode
   * @param text The string view to compare to this string.
   * @return false if `text` contains the same characters as this one, true otherwise.
   * @stable ICU 76
   */
  سانچا<typename S, typename = std::enable_if_t<Convertible_منزل_U16_String_View<S>>>
  مضمن bool عامل!=(ثابت S &متن) ثابت {
    أرجع !عامل==(متن);
  }

  /**
   * Greater than operator. Performs only bitwise comparison.
   * @param text The UnicodeString to compare to this one.
   * @return true if the characters in this are bitwise
   * greater than the characters in `text`, false otherwise
   * @stable ICU 2.0
   */
  مضمن UBool عامل> (ثابت UnicodeString& متن) ثابت;

  /**
   * Less than operator. Performs only bitwise comparison.
   * @param text The UnicodeString to compare to this one.
   * @return true if the characters in this are bitwise
   * less than the characters in `text`, false otherwise
   * @stable ICU 2.0
   */
  مضمن UBool عامل< (ثابت UnicodeString& متن) ثابت;

  /**
   * Greater than or equal operator. Performs only bitwise comparison.
   * @param text The UnicodeString to compare to this one.
   * @return true if the characters in this are bitwise
   * greater than or equal to the characters in `text`, false otherwise
   * @stable ICU 2.0
   */
  مضمن UBool عامل>= (ثابت UnicodeString& متن) ثابت;

  /**
   * Less than or equal operator. Performs only bitwise comparison.
   * @param text The UnicodeString to compare to this one.
   * @return true if the characters in this are bitwise
   * less than or equal to the characters in `text`, false otherwise
   * @stable ICU 2.0
   */
  مضمن UBool عامل<= (ثابت UnicodeString& متن) ثابت;

  /**
   * Compare the characters bitwise in this UnicodeString to
   * the characters in `text`.
   * @param text The UnicodeString to compare to this one.
   * @return The result of bitwise character comparison: 0 if this
   * contains the same characters as `text`, -1 if the characters in
   * this are bitwise less than the characters in `text`, +1 if the
   * characters in this are bitwise greater than the characters
   * in `text`.
   * @stable ICU 2.0
   */
  مضمن int8_t compare(ثابت UnicodeString& متن) ثابت;

  /**
   * Compare the characters bitwise in the range
   * [`start`, `start + length`) with the characters
   * in the **entire string** `text`.
   * (The parameters "start" and "length" are not applied to the other text "text".)
   * @param start the offset at which the compare operation begins
   * @param length the number of characters of text to compare.
   * @param text the other text to be compared against this string.
   * @return The result of bitwise character comparison: 0 if this
   * contains the same characters as `text`, -1 if the characters in
   * this are bitwise less than the characters in `text`, +1 if the
   * characters in this are bitwise greater than the characters
   * in `text`.
   * @stable ICU 2.0
   */
  مضمن int8_t compare(int32_t شروع,
         int32_t لمائی,
         ثابت UnicodeString& متن) ثابت;

  /**
   * Compare the characters bitwise in the range
   * [`start`, `start + length`) with the characters
   * in `srcText` in the range
   * [`srcStart`, `srcStart + srcLength`).
   * @param start the offset at which the compare operation begins
   * @param length the number of characters in this to compare.
   * @param srcText the text to be compared
   * @param srcStart the offset into `srcText` to start comparison
   * @param srcLength the number of characters in `src` to compare
   * @return The result of bitwise character comparison: 0 if this
   * contains the same characters as `srcText`, -1 if the characters in
   * this are bitwise less than the characters in `srcText`, +1 if the
   * characters in this are bitwise greater than the characters
   * in `srcText`.
   * @stable ICU 2.0
   */
   مضمن int8_t compare(int32_t شروع,
         int32_t لمائی,
         ثابت UnicodeString& src_متن,
         int32_t src_شروع,
         int32_t src_لمائی) ثابت;

  /**
   * Compare the characters bitwise in this UnicodeString with the first
   * `srcLength` characters in `srcChars`.
   * @param srcChars The characters to compare to this UnicodeString.
   * @param srcLength the number of characters in `srcChars` to compare
   * @return The result of bitwise character comparison: 0 if this
   * contains the same characters as `srcChars`, -1 if the characters in
   * this are bitwise less than the characters in `srcChars`, +1 if the
   * characters in this are bitwise greater than the characters
   * in `srcChars`.
   * @stable ICU 2.0
   */
  مضمن int8_t compare(ConstChar16Ptr srcChars,
         int32_t src_لمائی) ثابت;

  /**
   * Compare the characters bitwise in the range
   * [`start`, `start + length`) with the first
   * `length` characters in `srcChars`
   * @param start the offset at which the compare operation begins
   * @param length the number of characters to compare.
   * @param srcChars the characters to be compared
   * @return The result of bitwise character comparison: 0 if this
   * contains the same characters as `srcChars`, -1 if the characters in
   * this are bitwise less than the characters in `srcChars`, +1 if the
   * characters in this are bitwise greater than the characters
   * in `srcChars`.
   * @stable ICU 2.0
   */
  مضمن int8_t compare(int32_t شروع,
         int32_t لمائی,
         ثابت char16_t *srcChars) ثابت;

  /**
   * Compare the characters bitwise in the range
   * [`start`, `start + length`) with the characters
   * in `srcChars` in the range
   * [`srcStart`, `srcStart + srcLength`).
   * @param start the offset at which the compare operation begins
   * @param length the number of characters in this to compare
   * @param srcChars the characters to be compared
   * @param srcStart the offset into `srcChars` to start comparison
   * @param srcLength the number of characters in `srcChars` to compare
   * @return The result of bitwise character comparison: 0 if this
   * contains the same characters as `srcChars`, -1 if the characters in
   * this are bitwise less than the characters in `srcChars`, +1 if the
   * characters in this are bitwise greater than the characters
   * in `srcChars`.
   * @stable ICU 2.0
   */
  مضمن int8_t compare(int32_t شروع,
         int32_t لمائی,
         ثابت char16_t *srcChars,
         int32_t src_شروع,
         int32_t src_لمائی) ثابت;

  /**
   * Compare the characters bitwise in the range
   * [`start`, `limit`) with the characters
   * in `srcText` in the range
   * [`srcStart`, `srcLimit`).
   * @param start the offset at which the compare operation begins
   * @param limit the offset immediately following the compare operation
   * @param srcText the text to be compared
   * @param srcStart the offset into `srcText` to start comparison
   * @param srcLimit the offset into `srcText` to limit comparison
   * @return The result of bitwise character comparison: 0 if this
   * contains the same characters as `srcText`, -1 if the characters in
   * this are bitwise less than the characters in `srcText`, +1 if the
   * characters in this are bitwise greater than the characters
   * in `srcText`.
   * @stable ICU 2.0
   */
  مضمن int8_t compareBetween(int32_t شروع,
            int32_t حد_بندی,
            ثابت UnicodeString& src_متن,
            int32_t src_شروع,
            int32_t src_حد_بندی) ثابت;

  /**
   * Compare two Unicode strings in code point order.
   * The result may be different from the results of compare(), operator<, etc.
   * if supplementary characters are present:
   *
   * In UTF-16, supplementary characters (with code points U+10000 and above) are
   * stored with pairs of surrogate code units. These have values from 0xd800 to 0xdfff,
   * which means that they compare as less than some other BMP characters like U+feff.
   * This function compares Unicode strings in code point order.
   * If either of the UTF-16 strings is malformed (i.e., it contains unpaired surrogates), then the result is not defined.
   *
   * @param text Another string to compare this one to.
   * @return a negative/zero/positive integer corresponding to whether
   * this string is less than/equal to/greater than the second one
   * in code point order
   * @stable ICU 2.0
   */
  مضمن int8_t compare_کوڈ_نقطہ_ترتیب(ثابت UnicodeString& متن) ثابت;

  /**
   * Compare two Unicode strings in code point order.
   * The result may be different from the results of compare(), operator<, etc.
   * if supplementary characters are present:
   *
   * In UTF-16, supplementary characters (with code points U+10000 and above) are
   * stored with pairs of surrogate code units. These have values from 0xd800 to 0xdfff,
   * which means that they compare as less than some other BMP characters like U+feff.
   * This function compares Unicode strings in code point order.
   * If either of the UTF-16 strings is malformed (i.e., it contains unpaired surrogates), then the result is not defined.
   *
   * @param start The start offset in this string at which the compare operation begins.
   * @param length The number of code units from this string to compare.
   * @param srcText Another string to compare this one to.
   * @return a negative/zero/positive integer corresponding to whether
   * this string is less than/equal to/greater than the second one
   * in code point order
   * @stable ICU 2.0
   */
  مضمن int8_t compare_کوڈ_نقطہ_ترتیب(int32_t شروع,
                                      int32_t لمائی,
                                      ثابت UnicodeString& src_متن) ثابت;

  /**
   * Compare two Unicode strings in code point order.
   * The result may be different from the results of compare(), operator<, etc.
   * if supplementary characters are present:
   *
   * In UTF-16, supplementary characters (with code points U+10000 and above) are
   * stored with pairs of surrogate code units. These have values from 0xd800 to 0xdfff,
   * which means that they compare as less than some other BMP characters like U+feff.
   * This function compares Unicode strings in code point order.
   * If either of the UTF-16 strings is malformed (i.e., it contains unpaired surrogates), then the result is not defined.
   *
   * @param start The start offset in this string at which the compare operation begins.
   * @param length The number of code units from this string to compare.
   * @param srcText Another string to compare this one to.
   * @param srcStart The start offset in that string at which the compare operation begins.
   * @param srcLength The number of code units from that string to compare.
   * @return a negative/zero/positive integer corresponding to whether
   * this string is less than/equal to/greater than the second one
   * in code point order
   * @stable ICU 2.0
   */
   مضمن int8_t compare_کوڈ_نقطہ_ترتیب(int32_t شروع,
                                       int32_t لمائی,
                                       ثابت UnicodeString& src_متن,
                                       int32_t src_شروع,
                                       int32_t src_لمائی) ثابت;

  /**
   * Compare two Unicode strings in code point order.
   * The result may be different from the results of compare(), operator<, etc.
   * if supplementary characters are present:
   *
   * In UTF-16, supplementary characters (with code points U+10000 and above) are
   * stored with pairs of surrogate code units. These have values from 0xd800 to 0xdfff,
   * which means that they compare as less than some other BMP characters like U+feff.
   * This function compares Unicode strings in code point order.
   * If either of the UTF-16 strings is malformed (i.e., it contains unpaired surrogates), then the result is not defined.
   *
   * @param srcChars A pointer to another string to compare this one to.
   * @param srcLength The number of code units from that string to compare.
   * @return a negative/zero/positive integer corresponding to whether
   * this string is less than/equal to/greater than the second one
   * in code point order
   * @stable ICU 2.0
   */
  مضمن int8_t compare_کوڈ_نقطہ_ترتیب(ConstChar16Ptr srcChars,
                                      int32_t src_لمائی) ثابت;

  /**
   * Compare two Unicode strings in code point order.
   * The result may be different from the results of compare(), operator<, etc.
   * if supplementary characters are present:
   *
   * In UTF-16, supplementary characters (with code points U+10000 and above) are
   * stored with pairs of surrogate code units. These have values from 0xd800 to 0xdfff,
   * which means that they compare as less than some other BMP characters like U+feff.
   * This function compares Unicode strings in code point order.
   * If either of the UTF-16 strings is malformed (i.e., it contains unpaired surrogates), then the result is not defined.
   *
   * @param start The start offset in this string at which the compare operation begins.
   * @param length The number of code units from this string to compare.
   * @param srcChars A pointer to another string to compare this one to.
   * @return a negative/zero/positive integer corresponding to whether
   * this string is less than/equal to/greater than the second one
   * in code point order
   * @stable ICU 2.0
   */
  مضمن int8_t compare_کوڈ_نقطہ_ترتیب(int32_t شروع,
                                      int32_t لمائی,
                                      ثابت char16_t *srcChars) ثابت;

  /**
   * Compare two Unicode strings in code point order.
   * The result may be different from the results of compare(), operator<, etc.
   * if supplementary characters are present:
   *
   * In UTF-16, supplementary characters (with code points U+10000 and above) are
   * stored with pairs of surrogate code units. These have values from 0xd800 to 0xdfff,
   * which means that they compare as less than some other BMP characters like U+feff.
   * This function compares Unicode strings in code point order.
   * If either of the UTF-16 strings is malformed (i.e., it contains unpaired surrogates), then the result is not defined.
   *
   * @param start The start offset in this string at which the compare operation begins.
   * @param length The number of code units from this string to compare.
   * @param srcChars A pointer to another string to compare this one to.
   * @param srcStart The start offset in that string at which the compare operation begins.
   * @param srcLength The number of code units from that string to compare.
   * @return a negative/zero/positive integer corresponding to whether
   * this string is less than/equal to/greater than the second one
   * in code point order
   * @stable ICU 2.0
   */
  مضمن int8_t compare_کوڈ_نقطہ_ترتیب(int32_t شروع,
                                      int32_t لمائی,
                                      ثابت char16_t *srcChars,
                                      int32_t src_شروع,
                                      int32_t src_لمائی) ثابت;

  /**
   * Compare two Unicode strings in code point order.
   * The result may be different from the results of compare(), operator<, etc.
   * if supplementary characters are present:
   *
   * In UTF-16, supplementary characters (with code points U+10000 and above) are
   * stored with pairs of surrogate code units. These have values from 0xd800 to 0xdfff,
   * which means that they compare as less than some other BMP characters like U+feff.
   * This function compares Unicode strings in code point order.
   * If either of the UTF-16 strings is malformed (i.e., it contains unpaired surrogates), then the result is not defined.
   *
   * @param start The start offset in this string at which the compare operation begins.
   * @param limit The offset after the last code unit from this string to compare.
   * @param srcText Another string to compare this one to.
   * @param srcStart The start offset in that string at which the compare operation begins.
   * @param srcLimit The offset after the last code unit from that string to compare.
   * @return a negative/zero/positive integer corresponding to whether
   * this string is less than/equal to/greater than the second one
   * in code point order
   * @stable ICU 2.0
   */
  مضمن int8_t compare_کوڈ_نقطہ_ترتیب_Between(int32_t شروع,
                                             int32_t حد_بندی,
                                             ثابت UnicodeString& src_متن,
                                             int32_t src_شروع,
                                             int32_t src_حد_بندی) ثابت;

  /**
   * Compare two strings case-insensitively using full case folding.
   * This is equivalent to this->foldCase(options).compare(text.foldCase(options)).
   *
   * @param text Another string to compare this one to.
   * @param options A bit set of options:
   *   - U_FOLD_CASE_DEFAULT or 0 is used for default options:
   *     Comparison in code unit order with default case folding.
   *
   *   - U_COMPARE_CODE_POINT_ORDER
   *     Set to choose code point order instead of code unit order
   *     (see u_strCompare for details).
   *
   *   - U_FOLD_CASE_EXCLUDE_SPECIAL_I
   *
   * @return A negative, zero, or positive integer indicating the comparison result.
   * @stable ICU 2.0
   */
  مضمن int8_t caseCompare(ثابت UnicodeString& متن, uint32_t options) ثابت;

  /**
   * Compare two strings case-insensitively using full case folding.
   * This is equivalent to this->foldCase(options).compare(srcText.foldCase(options)).
   *
   * @param start The start offset in this string at which the compare operation begins.
   * @param length The number of code units from this string to compare.
   * @param srcText Another string to compare this one to.
   * @param options A bit set of options:
   *   - U_FOLD_CASE_DEFAULT or 0 is used for default options:
   *     Comparison in code unit order with default case folding.
   *
   *   - U_COMPARE_CODE_POINT_ORDER
   *     Set to choose code point order instead of code unit order
   *     (see u_strCompare for details).
   *
   *   - U_FOLD_CASE_EXCLUDE_SPECIAL_I
   *
   * @return A negative, zero, or positive integer indicating the comparison result.
   * @stable ICU 2.0
   */
  مضمن int8_t caseCompare(int32_t شروع,
         int32_t لمائی,
         ثابت UnicodeString& src_متن,
         uint32_t options) ثابت;

  /**
   * Compare two strings case-insensitively using full case folding.
   * This is equivalent to this->foldCase(options).compare(srcText.foldCase(options)).
   *
   * @param start The start offset in this string at which the compare operation begins.
   * @param length The number of code units from this string to compare.
   * @param srcText Another string to compare this one to.
   * @param srcStart The start offset in that string at which the compare operation begins.
   * @param srcLength The number of code units from that string to compare.
   * @param options A bit set of options:
   *   - U_FOLD_CASE_DEFAULT or 0 is used for default options:
   *     Comparison in code unit order with default case folding.
   *
   *   - U_COMPARE_CODE_POINT_ORDER
   *     Set to choose code point order instead of code unit order
   *     (see u_strCompare for details).
   *
   *   - U_FOLD_CASE_EXCLUDE_SPECIAL_I
   *
   * @return A negative, zero, or positive integer indicating the comparison result.
   * @stable ICU 2.0
   */
  مضمن int8_t caseCompare(int32_t شروع,
         int32_t لمائی,
         ثابت UnicodeString& src_متن,
         int32_t src_شروع,
         int32_t src_لمائی,
         uint32_t options) ثابت;

  /**
   * Compare two strings case-insensitively using full case folding.
   * This is equivalent to this->foldCase(options).compare(srcChars.foldCase(options)).
   *
   * @param srcChars A pointer to another string to compare this one to.
   * @param srcLength The number of code units from that string to compare.
   * @param options A bit set of options:
   *   - U_FOLD_CASE_DEFAULT or 0 is used for default options:
   *     Comparison in code unit order with default case folding.
   *
   *   - U_COMPARE_CODE_POINT_ORDER
   *     Set to choose code point order instead of code unit order
   *     (see u_strCompare for details).
   *
   *   - U_FOLD_CASE_EXCLUDE_SPECIAL_I
   *
   * @return A negative, zero, or positive integer indicating the comparison result.
   * @stable ICU 2.0
   */
  مضمن int8_t caseCompare(ConstChar16Ptr srcChars,
         int32_t src_لمائی,
         uint32_t options) ثابت;

  /**
   * Compare two strings case-insensitively using full case folding.
   * This is equivalent to this->foldCase(options).compare(srcChars.foldCase(options)).
   *
   * @param start The start offset in this string at which the compare operation begins.
   * @param length The number of code units from this string to compare.
   * @param srcChars A pointer to another string to compare this one to.
   * @param options A bit set of options:
   *   - U_FOLD_CASE_DEFAULT or 0 is used for default options:
   *     Comparison in code unit order with default case folding.
   *
   *   - U_COMPARE_CODE_POINT_ORDER
   *     Set to choose code point order instead of code unit order
   *     (see u_strCompare for details).
   *
   *   - U_FOLD_CASE_EXCLUDE_SPECIAL_I
   *
   * @return A negative, zero, or positive integer indicating the comparison result.
   * @stable ICU 2.0
   */
  مضمن int8_t caseCompare(int32_t شروع,
         int32_t لمائی,
         ثابت char16_t *srcChars,
         uint32_t options) ثابت;

  /**
   * Compare two strings case-insensitively using full case folding.
   * This is equivalent to this->foldCase(options).compare(srcChars.foldCase(options)).
   *
   * @param start The start offset in this string at which the compare operation begins.
   * @param length The number of code units from this string to compare.
   * @param srcChars A pointer to another string to compare this one to.
   * @param srcStart The start offset in that string at which the compare operation begins.
   * @param srcLength The number of code units from that string to compare.
   * @param options A bit set of options:
   *   - U_FOLD_CASE_DEFAULT or 0 is used for default options:
   *     Comparison in code unit order with default case folding.
   *
   *   - U_COMPARE_CODE_POINT_ORDER
   *     Set to choose code point order instead of code unit order
   *     (see u_strCompare for details).
   *
   *   - U_FOLD_CASE_EXCLUDE_SPECIAL_I
   *
   * @return A negative, zero, or positive integer indicating the comparison result.
   * @stable ICU 2.0
   */
  مضمن int8_t caseCompare(int32_t شروع,
         int32_t لمائی,
         ثابت char16_t *srcChars,
         int32_t src_شروع,
         int32_t src_لمائی,
         uint32_t options) ثابت;

  /**
   * Compare two strings case-insensitively using full case folding.
   * This is equivalent to this->foldCase(options).compareBetween(text.foldCase(options)).
   *
   * @param start The start offset in this string at which the compare operation begins.
   * @param limit The offset after the last code unit from this string to compare.
   * @param srcText Another string to compare this one to.
   * @param srcStart The start offset in that string at which the compare operation begins.
   * @param srcLimit The offset after the last code unit from that string to compare.
   * @param options A bit set of options:
   *   - U_FOLD_CASE_DEFAULT or 0 is used for default options:
   *     Comparison in code unit order with default case folding.
   *
   *   - U_COMPARE_CODE_POINT_ORDER
   *     Set to choose code point order instead of code unit order
   *     (see u_strCompare for details).
   *
   *   - U_FOLD_CASE_EXCLUDE_SPECIAL_I
   *
   * @return A negative, zero, or positive integer indicating the comparison result.
   * @stable ICU 2.0
   */
  مضمن int8_t caseCompareBetween(int32_t شروع,
            int32_t حد_بندی,
            ثابت UnicodeString& src_متن,
            int32_t src_شروع,
            int32_t src_حد_بندی,
            uint32_t options) ثابت;

  /**
   * Determine if this starts with the characters in `text`
   * @param text The text to match.
   * @return true if this starts with the characters in `text`,
   * false otherwise
   * @stable ICU 2.0
   */
  مضمن UBool startsWith(ثابت UnicodeString& متن) ثابت;

  /**
   * Determine if this starts with the characters in `srcText`
   * in the range [`srcStart`, `srcStart + srcLength`).
   * @param srcText The text to match.
   * @param srcStart the offset into `srcText` to start matching
   * @param srcLength the number of characters in `srcText` to match
   * @return true if this starts with the characters in `text`,
   * false otherwise
   * @stable ICU 2.0
   */
  مضمن UBool startsWith(ثابت UnicodeString& src_متن,
            int32_t src_شروع,
            int32_t src_لمائی) ثابت;

  /**
   * Determine if this starts with the characters in `srcChars`
   * @param srcChars The characters to match.
   * @param srcLength the number of characters in `srcChars`
   * @return true if this starts with the characters in `srcChars`,
   * false otherwise
   * @stable ICU 2.0
   */
  مضمن UBool startsWith(ConstChar16Ptr srcChars,
            int32_t src_لمائی) ثابت;

  /**
   * Determine if this ends with the characters in `srcChars`
   * in the range  [`srcStart`, `srcStart + srcLength`).
   * @param srcChars The characters to match.
   * @param srcStart the offset into `srcText` to start matching
   * @param srcLength the number of characters in `srcChars` to match
   * @return true if this ends with the characters in `srcChars`, false otherwise
   * @stable ICU 2.0
   */
  مضمن UBool startsWith(ثابت char16_t *srcChars,
            int32_t src_شروع,
            int32_t src_لمائی) ثابت;

  /**
   * Determine if this ends with the characters in `text`
   * @param text The text to match.
   * @return true if this ends with the characters in `text`,
   * false otherwise
   * @stable ICU 2.0
   */
  مضمن UBool سرے_With(ثابت UnicodeString& متن) ثابت;

  /**
   * Determine if this ends with the characters in `srcText`
   * in the range [`srcStart`, `srcStart + srcLength`).
   * @param srcText The text to match.
   * @param srcStart the offset into `srcText` to start matching
   * @param srcLength the number of characters in `srcText` to match
   * @return true if this ends with the characters in `text`,
   * false otherwise
   * @stable ICU 2.0
   */
  مضمن UBool سرے_With(ثابت UnicodeString& src_متن,
          int32_t src_شروع,
          int32_t src_لمائی) ثابت;

  /**
   * Determine if this ends with the characters in `srcChars`
   * @param srcChars The characters to match.
   * @param srcLength the number of characters in `srcChars`
   * @return true if this ends with the characters in `srcChars`,
   * false otherwise
   * @stable ICU 2.0
   */
  مضمن UBool سرے_With(ConstChar16Ptr srcChars,
          int32_t src_لمائی) ثابت;

  /**
   * Determine if this ends with the characters in `srcChars`
   * in the range  [`srcStart`, `srcStart + srcLength`).
   * @param srcChars The characters to match.
   * @param srcStart the offset into `srcText` to start matching
   * @param srcLength the number of characters in `srcChars` to match
   * @return true if this ends with the characters in `srcChars`,
   * false otherwise
   * @stable ICU 2.0
   */
  مضمن UBool سرے_With(ثابت char16_t *srcChars,
          int32_t src_شروع,
          int32_t src_لمائی) ثابت;


  /* Searching - bitwise only */

  /**
   * Locate in this the first occurrence of the characters in `text`,
   * using bitwise comparison.
   * @param text The text to search for.
   * @return The offset into this of the start of `text`,
   * or -1 if not found.
   * @stable ICU 2.0
   */
  مضمن int32_t اشاریہ_دا(ثابت UnicodeString& متن) ثابت;

  /**
   * Locate in this the first occurrence of the characters in `text`
   * starting at offset `start`, using bitwise comparison.
   * @param text The text to search for.
   * @param start The offset at which searching will start.
   * @return The offset into this of the start of `text`,
   * or -1 if not found.
   * @stable ICU 2.0
   */
  مضمن int32_t اشاریہ_دا(ثابت UnicodeString& متن,
              int32_t شروع) ثابت;

  /**
   * Locate in this the first occurrence in the range
   * [`start`, `start + length`) of the characters
   * in `text`, using bitwise comparison.
   * @param text The text to search for.
   * @param start The offset at which searching will start.
   * @param length The number of characters to search
   * @return The offset into this of the start of `text`,
   * or -1 if not found.
   * @stable ICU 2.0
   */
  مضمن int32_t اشاریہ_دا(ثابت UnicodeString& متن,
              int32_t شروع,
              int32_t لمائی) ثابت;

  /**
   * Locate in this the first occurrence in the range
   * [`start`, `start + length`) of the characters
   *  in `srcText` in the range
   * [`srcStart`, `srcStart + srcLength`),
   * using bitwise comparison.
   * @param srcText The text to search for.
   * @param srcStart the offset into `srcText` at which
   * to start matching
   * @param srcLength the number of characters in `srcText` to match
   * @param start the offset into this at which to start matching
   * @param length the number of characters in this to search
   * @return The offset into this of the start of `text`,
   * or -1 if not found.
   * @stable ICU 2.0
   */
  مضمن int32_t اشاریہ_دا(ثابت UnicodeString& src_متن,
              int32_t src_شروع,
              int32_t src_لمائی,
              int32_t شروع,
              int32_t لمائی) ثابت;

  /**
   * Locate in this the first occurrence of the characters in
   * `srcChars`
   * starting at offset `start`, using bitwise comparison.
   * @param srcChars The text to search for.
   * @param srcLength the number of characters in `srcChars` to match
   * @param start the offset into this at which to start matching
   * @return The offset into this of the start of `text`,
   * or -1 if not found.
   * @stable ICU 2.0
   */
  مضمن int32_t اشاریہ_دا(ثابت char16_t *srcChars,
              int32_t src_لمائی,
              int32_t شروع) ثابت;

  /**
   * Locate in this the first occurrence in the range
   * [`start`, `start + length`) of the characters
   * in `srcChars`, using bitwise comparison.
   * @param srcChars The text to search for.
   * @param srcLength the number of characters in `srcChars`
   * @param start The offset at which searching will start.
   * @param length The number of characters to search
   * @return The offset into this of the start of `srcChars`,
   * or -1 if not found.
   * @stable ICU 2.0
   */
  مضمن int32_t اشاریہ_دا(ConstChar16Ptr srcChars,
              int32_t src_لمائی,
              int32_t شروع,
              int32_t لمائی) ثابت;

  /**
   * Locate in this the first occurrence in the range
   * [`start`, `start + length`) of the characters
   * in `srcChars` in the range
   * [`srcStart`, `srcStart + srcLength`),
   * using bitwise comparison.
   * @param srcChars The text to search for.
   * @param srcStart the offset into `srcChars` at which
   * to start matching
   * @param srcLength the number of characters in `srcChars` to match
   * @param start the offset into this at which to start matching
   * @param length the number of characters in this to search
   * @return The offset into this of the start of `text`,
   * or -1 if not found.
   * @stable ICU 2.0
   */
  int32_t اشاریہ_دا(ثابت char16_t *srcChars,
              int32_t src_شروع,
              int32_t src_لمائی,
              int32_t شروع,
              int32_t لمائی) ثابت;

  /**
   * Locate in this the first occurrence of the BMP code point `c`,
   * using bitwise comparison.
   * @param c The code unit to search for.
   * @return The offset into this of `c`, or -1 if not found.
   * @stable ICU 2.0
   */
  مضمن int32_t اشاریہ_دا(char16_t c) ثابت;

  /**
   * Locate in this the first occurrence of the code point `c`,
   * using bitwise comparison.
   *
   * @param c The code point to search for.
   * @return The offset into this of `c`, or -1 if not found.
   * @stable ICU 2.0
   */
  مضمن int32_t اشاریہ_دا(UChar32 c) ثابت;

  /**
   * Locate in this the first occurrence of the BMP code point `c`,
   * starting at offset `start`, using bitwise comparison.
   * @param c The code unit to search for.
   * @param start The offset at which searching will start.
   * @return The offset into this of `c`, or -1 if not found.
   * @stable ICU 2.0
   */
  مضمن int32_t اشاریہ_دا(char16_t c,
              int32_t شروع) ثابت;

  /**
   * Locate in this the first occurrence of the code point `c`
   * starting at offset `start`, using bitwise comparison.
   *
   * @param c The code point to search for.
   * @param start The offset at which searching will start.
   * @return The offset into this of `c`, or -1 if not found.
   * @stable ICU 2.0
   */
  مضمن int32_t اشاریہ_دا(UChar32 c,
              int32_t شروع) ثابت;

  /**
   * Locate in this the first occurrence of the BMP code point `c`
   * in the range [`start`, `start + length`),
   * using bitwise comparison.
   * @param c The code unit to search for.
   * @param start the offset into this at which to start matching
   * @param length the number of characters in this to search
   * @return The offset into this of `c`, or -1 if not found.
   * @stable ICU 2.0
   */
  مضمن int32_t اشاریہ_دا(char16_t c,
              int32_t شروع,
              int32_t لمائی) ثابت;

  /**
   * Locate in this the first occurrence of the code point `c`
   * in the range [`start`, `start + length`),
   * using bitwise comparison.
   *
   * @param c The code point to search for.
   * @param start the offset into this at which to start matching
   * @param length the number of characters in this to search
   * @return The offset into this of `c`, or -1 if not found.
   * @stable ICU 2.0
   */
  مضمن int32_t اشاریہ_دا(UChar32 c,
              int32_t شروع,
              int32_t لمائی) ثابت;

  /**
   * Locate in this the last occurrence of the characters in `text`,
   * using bitwise comparison.
   * @param text The text to search for.
   * @return The offset into this of the start of `text`,
   * or -1 if not found.
   * @stable ICU 2.0
   */
  مضمن int32_t آخری_اشاریہ_دا(ثابت UnicodeString& متن) ثابت;

  /**
   * Locate in this the last occurrence of the characters in `text`
   * starting at offset `start`, using bitwise comparison.
   * @param text The text to search for.
   * @param start The offset at which searching will start.
   * @return The offset into this of the start of `text`,
   * or -1 if not found.
   * @stable ICU 2.0
   */
  مضمن int32_t آخری_اشاریہ_دا(ثابت UnicodeString& متن,
              int32_t شروع) ثابت;

  /**
   * Locate in this the last occurrence in the range
   * [`start`, `start + length`) of the characters
   * in `text`, using bitwise comparison.
   * @param text The text to search for.
   * @param start The offset at which searching will start.
   * @param length The number of characters to search
   * @return The offset into this of the start of `text`,
   * or -1 if not found.
   * @stable ICU 2.0
   */
  مضمن int32_t آخری_اشاریہ_دا(ثابت UnicodeString& متن,
              int32_t شروع,
              int32_t لمائی) ثابت;

  /**
   * Locate in this the last occurrence in the range
   * [`start`, `start + length`) of the characters
   * in `srcText` in the range
   * [`srcStart`, `srcStart + srcLength`),
   * using bitwise comparison.
   * @param srcText The text to search for.
   * @param srcStart the offset into `srcText` at which
   * to start matching
   * @param srcLength the number of characters in `srcText` to match
   * @param start the offset into this at which to start matching
   * @param length the number of characters in this to search
   * @return The offset into this of the start of `text`,
   * or -1 if not found.
   * @stable ICU 2.0
   */
  مضمن int32_t آخری_اشاریہ_دا(ثابت UnicodeString& src_متن,
              int32_t src_شروع,
              int32_t src_لمائی,
              int32_t شروع,
              int32_t لمائی) ثابت;

  /**
   * Locate in this the last occurrence of the characters in `srcChars`
   * starting at offset `start`, using bitwise comparison.
   * @param srcChars The text to search for.
   * @param srcLength the number of characters in `srcChars` to match
   * @param start the offset into this at which to start matching
   * @return The offset into this of the start of `text`,
   * or -1 if not found.
   * @stable ICU 2.0
   */
  مضمن int32_t آخری_اشاریہ_دا(ثابت char16_t *srcChars,
              int32_t src_لمائی,
              int32_t شروع) ثابت;

  /**
   * Locate in this the last occurrence in the range
   * [`start`, `start + length`) of the characters
   * in `srcChars`, using bitwise comparison.
   * @param srcChars The text to search for.
   * @param srcLength the number of characters in `srcChars`
   * @param start The offset at which searching will start.
   * @param length The number of characters to search
   * @return The offset into this of the start of `srcChars`,
   * or -1 if not found.
   * @stable ICU 2.0
   */
  مضمن int32_t آخری_اشاریہ_دا(ConstChar16Ptr srcChars,
              int32_t src_لمائی,
              int32_t شروع,
              int32_t لمائی) ثابت;

  /**
   * Locate in this the last occurrence in the range
   * [`start`, `start + length`) of the characters
   * in `srcChars` in the range
   * [`srcStart`, `srcStart + srcLength`),
   * using bitwise comparison.
   * @param srcChars The text to search for.
   * @param srcStart the offset into `srcChars` at which
   * to start matching
   * @param srcLength the number of characters in `srcChars` to match
   * @param start the offset into this at which to start matching
   * @param length the number of characters in this to search
   * @return The offset into this of the start of `text`,
   * or -1 if not found.
   * @stable ICU 2.0
   */
  int32_t آخری_اشاریہ_دا(ثابت char16_t *srcChars,
              int32_t src_شروع,
              int32_t src_لمائی,
              int32_t شروع,
              int32_t لمائی) ثابت;

  /**
   * Locate in this the last occurrence of the BMP code point `c`,
   * using bitwise comparison.
   * @param c The code unit to search for.
   * @return The offset into this of `c`, or -1 if not found.
   * @stable ICU 2.0
   */
  مضمن int32_t آخری_اشاریہ_دا(char16_t c) ثابت;

  /**
   * Locate in this the last occurrence of the code point `c`,
   * using bitwise comparison.
   *
   * @param c The code point to search for.
   * @return The offset into this of `c`, or -1 if not found.
   * @stable ICU 2.0
   */
  مضمن int32_t آخری_اشاریہ_دا(UChar32 c) ثابت;

  /**
   * Locate in this the last occurrence of the BMP code point `c`
   * starting at offset `start`, using bitwise comparison.
   * @param c The code unit to search for.
   * @param start The offset at which searching will start.
   * @return The offset into this of `c`, or -1 if not found.
   * @stable ICU 2.0
   */
  مضمن int32_t آخری_اشاریہ_دا(char16_t c,
              int32_t شروع) ثابت;

  /**
   * Locate in this the last occurrence of the code point `c`
   * starting at offset `start`, using bitwise comparison.
   *
   * @param c The code point to search for.
   * @param start The offset at which searching will start.
   * @return The offset into this of `c`, or -1 if not found.
   * @stable ICU 2.0
   */
  مضمن int32_t آخری_اشاریہ_دا(UChar32 c,
              int32_t شروع) ثابت;

  /**
   * Locate in this the last occurrence of the BMP code point `c`
   * in the range [`start`, `start + length`),
   * using bitwise comparison.
   * @param c The code unit to search for.
   * @param start the offset into this at which to start matching
   * @param length the number of characters in this to search
   * @return The offset into this of `c`, or -1 if not found.
   * @stable ICU 2.0
   */
  مضمن int32_t آخری_اشاریہ_دا(char16_t c,
              int32_t شروع,
              int32_t لمائی) ثابت;

  /**
   * Locate in this the last occurrence of the code point `c`
   * in the range [`start`, `start + length`),
   * using bitwise comparison.
   *
   * @param c The code point to search for.
   * @param start the offset into this at which to start matching
   * @param length the number of characters in this to search
   * @return The offset into this of `c`, or -1 if not found.
   * @stable ICU 2.0
   */
  مضمن int32_t آخری_اشاریہ_دا(UChar32 c,
              int32_t شروع,
              int32_t لمائی) ثابت;


  /* Character access */

  /**
   * Return the code unit at offset `offset`.
   * If the offset is not valid (0..length()-1) then U+ffff is returned.
   * @param offset a valid offset into the text
   * @return the code unit at offset `offset`
   *         or 0xffff if the offset is not valid for this string
   * @stable ICU 2.0
   */
  مضمن char16_t charAt(int32_t وتھ) ثابت;

  /**
   * Return the code unit at offset `offset`.
   * If the offset is not valid (0..length()-1) then U+ffff is returned.
   * @param offset a valid offset into the text
   * @return the code unit at offset `offset`
   * @stable ICU 2.0
   */
  مضمن char16_t عامل[] (int32_t وتھ) ثابت;

  /**
   * Return the code point that contains the code unit
   * at offset `offset`.
   * If the offset is not valid (0..length()-1) then U+ffff is returned.
   * @param offset a valid offset into the text
   * that indicates the text offset of any of the code units
   * that will be assembled into a code point (21-bit value) and returned
   * @return the code point of text at `offset`
   *         or 0xffff if the offset is not valid for this string
   * @stable ICU 2.0
   */
  UChar32 char32At(int32_t وتھ) ثابت;

  /**
   * Adjust a random-access offset so that
   * it points to the beginning of a Unicode character.
   * The offset that is passed in points to
   * any code unit of a code point,
   * while the returned offset will point to the first code unit
   * of the same code point.
   * In UTF-16, if the input offset points to a second surrogate
   * of a surrogate pair, then the returned offset will point
   * to the first surrogate.
   * @param offset a valid offset into one code point of the text
   * @return offset of the first code unit of the same code point
   * @see U16_SET_CP_START
   * @stable ICU 2.0
   */
  int32_t get_Char32_شروع(int32_t وتھ) ثابت;

  /**
   * Adjust a random-access offset so that
   * it points behind a Unicode character.
   * The offset that is passed in points behind
   * any code unit of a code point,
   * while the returned offset will point behind the last code unit
   * of the same code point.
   * In UTF-16, if the input offset points behind the first surrogate
   * (i.e., to the second surrogate)
   * of a surrogate pair, then the returned offset will point
   * behind the second surrogate (i.e., to the first surrogate).
   * @param offset a valid offset after any code unit of a code point of the text
   * @return offset of the first code unit after the same code point
   * @see U16_SET_CP_LIMIT
   * @stable ICU 2.0
   */
  int32_t get_Char32_حد_بندی(int32_t وتھ) ثابت;

  /**
   * Move the code unit index along the string by delta code points.
   * Interpret the input index as a code unit-based offset into the string,
   * move the index forward or backward by delta code points, and
   * return the resulting index.
   * The input index should point to the first code unit of a code point,
   * if there is more than one.
   *
   * Both input and output indexes are code unit-based as for all
   * string indexes/offsets in ICU (and other libraries, like MBCS char*).
   * If delta<0 then the index is moved backward (toward the start of the string).
   * If delta>0 then the index is moved forward (toward the end of the string).
   *
   * This behaves like CharacterIterator::move32(delta, kCurrent).
   *
   * Behavior for out-of-bounds indexes:
   * `moveIndex32` pins the input index to 0..length(), i.e.,
   * if the input index<0 then it is pinned to 0;
   * if it is index>length() then it is pinned to length().
   * Afterwards, the index is moved by `delta` code points
   * forward or backward,
   * but no further backward than to 0 and no further forward than to length().
   * The resulting index return value will be in between 0 and length(), inclusively.
   *
   * Examples:
   * \code
   *     // s has code points 'a' U+10000 'b' U+10ffff U+2029
   *     UnicodeString s(u"a\U00010000b\U0010ffff\u2029");
   *
   *     // initial index: position of U+10000
   *     int32_t index=1;
   *
   *     // the following examples will all result in index==4, position of U+10ffff
   *
   *     // skip 2 code points from some position in the string
   *     index=s.moveIndex32(index, 2); // skips U+10000 and 'b'
   *
   *     // go to the 3rd code point from the start of s (0-based)
   *     index=s.moveIndex32(0, 3); // skips 'a', U+10000, and 'b'
   *
   *     // go to the next-to-last code point of s
   *     index=s.moveIndex32(s.length(), -2); // backward-skips U+2029 and U+10ffff
   * \endcode
   *
   * @param index input code unit index
   * @param delta (signed) code point count to move the index forward or backward
   *        in the string
   * @return the resulting code unit index
   * @stable ICU 2.0
   */
  int32_t moveIndex32(int32_t اشاریہ, int32_t delta) ثابت;

  /* Substring extraction */

  /**
   * Copy the characters in the range
   * [`start`, `start + length`) into the array `dst`,
   * beginning at `dstStart`.
   * If the string aliases to `dst` itself as an external buffer,
   * then extract() will not copy the contents.
   *
   * @param start offset of first character which will be copied into the array
   * @param length the number of characters to extract
   * @param dst array in which to copy characters.  The length of `dst`
   * must be at least (`dstStart + length`).
   * @param dstStart the offset in `dst` where the first character
   * will be extracted
   * @stable ICU 2.0
   */
  مضمن فراغ کڈھو(int32_t شروع,
           int32_t لمائی,
           Char16Ptr dst,
           int32_t dst_شروع = 0) ثابت;

  /**
   * Copy the contents of the string into dest.
   * This is a convenience function that
   * checks if there is enough space in dest,
   * extracts the entire string if possible,
   * and NUL-terminates dest if possible.
   *
   * If the string fits into dest but cannot be NUL-terminated
   * (length()==destCapacity) then the error code is set to U_STRING_NOT_TERMINATED_WARNING.
   * If the string itself does not fit into dest
   * (length()>destCapacity) then the error code is set to U_BUFFER_OVERFLOW_ERROR.
   *
   * If the string aliases to `dest` itself as an external buffer,
   * then extract() will not copy the contents.
   *
   * @param dest Destination string buffer.
   * @param destCapacity Number of char16_ts available at dest.
   * @param errorCode ICU error code.
   * @return length()
   * @stable ICU 2.0
   */
  int32_t
  کڈھو(Char16Ptr dest, int32_t dest_سمرتھا,
          UError_کوڈ &error_کوڈ) ثابت;

  /**
   * Copy the characters in the range
   * [`start`, `start + length`) into the  UnicodeString
   * `target`.
   * @param start offset of first character which will be copied
   * @param length the number of characters to extract
   * @param target UnicodeString into which to copy characters.
   * @stable ICU 2.0
   */
  مضمن فراغ کڈھو(int32_t شروع,
           int32_t لمائی,
           UnicodeString& نشانہ) ثابت;

  /**
   * Copy the characters in the range [`start`, `limit`)
   * into the array `dst`, beginning at `dstStart`.
   * @param start offset of first character which will be copied into the array
   * @param limit offset immediately following the last character to be copied
   * @param dst array in which to copy characters.  The length of `dst`
   * must be at least (`dstStart + (limit - start)`).
   * @param dstStart the offset in `dst` where the first character
   * will be extracted
   * @stable ICU 2.0
   */
  مضمن فراغ کڈھو_Between(int32_t شروع,
              int32_t حد_بندی,
              char16_t *dst,
              int32_t dst_شروع = 0) ثابت;

  /**
   * Copy the characters in the range [`start`, `limit`)
   * into the UnicodeString `target`.  Replaceable API.
   * @param start offset of first character which will be copied
   * @param limit offset immediately following the last character to be copied
   * @param target UnicodeString into which to copy characters.
   * @stable ICU 2.0
   */
  مجازی فراغ کڈھو_Between(int32_t شروع,
              int32_t حد_بندی,
              UnicodeString& نشانہ) ثابت override;

  /**
   * Copy the characters in the range
   * [`start`, `start + startLength`) into an array of characters.
   * All characters must be invariant (see utypes.h).
   * Use US_INV as the last, signature-distinguishing parameter.
   *
   * This function does not write any more than `targetCapacity`
   * characters but returns the length of the entire output string
   * so that one can allocate a larger buffer and call the function again
   * if necessary.
   * The output string is NUL-terminated if possible.
   *
   * @param start offset of first character which will be copied
   * @param startLength the number of characters to extract
   * @param target the target buffer for extraction, can be nullptr
   *               if targetLength is 0
   * @param targetCapacity the length of the target buffer
   * @param inv Signature-distinguishing parameter, use US_INV.
   * @return the output string length, not including the terminating NUL
   * @stable ICU 3.2
   */
  int32_t کڈھو(int32_t شروع,
           int32_t شروع_لمائی,
           حرف *نشانہ,
           int32_t نشانہ_سمرتھا,
           تعداد EInvariant inv) ثابت;

#إذا U_CHARSET_IS_UTF8 || !UCONFIG_NO_CONVERSION

  /**
   * Copy the characters in the range
   * [`start`, `start + length`) into an array of characters
   * in the platform's default codepage.
   * This function does not write any more than `targetLength`
   * characters but returns the length of the entire output string
   * so that one can allocate a larger buffer and call the function again
   * if necessary.
   * The output string is NUL-terminated if possible.
   *
   * @param start offset of first character which will be copied
   * @param startLength the number of characters to extract
   * @param target the target buffer for extraction
   * @param targetLength the length of the target buffer
   * If `target` is nullptr, then the number of bytes required for
   * `target` is returned.
   * @return the output string length, not including the terminating NUL
   * @stable ICU 2.0
   */
  int32_t کڈھو(int32_t شروع,
           int32_t شروع_لمائی,
           حرف *نشانہ,
           uint32_t نشانہ_لمائی) ثابت;

#جے_انت

#إذا !UCONFIG_NO_CONVERSION

  /**
   * Copy the characters in the range
   * [`start`, `start + length`) into an array of characters
   * in a specified codepage.
   * The output string is NUL-terminated.
   *
   * Recommendation: For invariant-character strings use
   * extract(int32_t start, int32_t length, char *target, int32_t targetCapacity, enum EInvariant inv) const
   * because it avoids object code dependencies of UnicodeString on
   * the conversion code.
   *
   * @param start offset of first character which will be copied
   * @param startLength the number of characters to extract
   * @param target the target buffer for extraction
   * @param codepage the desired codepage for the characters.  0 has
   * the special meaning of the default codepage
   * If `codepage` is an empty string (`""`),
   * then a simple conversion is performed on the codepage-invariant
   * subset ("invariant characters") of the platform encoding. See utypes.h.
   * If `target` is nullptr, then the number of bytes required for
   * `target` is returned. It is assumed that the target is big enough
   * to fit all of the characters.
   * @return the output string length, not including the terminating NUL
   * @stable ICU 2.0
   */
  مضمن int32_t کڈھو(int32_t شروع,
                         int32_t شروع_لمائی,
                         حرف* نشانہ,
                         ثابت حرف* codepage = nullptr) ثابت;

  /**
   * Copy the characters in the range
   * [`start`, `start + length`) into an array of characters
   * in a specified codepage.
   * This function does not write any more than `targetLength`
   * characters but returns the length of the entire output string
   * so that one can allocate a larger buffer and call the function again
   * if necessary.
   * The output string is NUL-terminated if possible.
   *
   * Recommendation: For invariant-character strings use
   * extract(int32_t start, int32_t length, char *target, int32_t targetCapacity, enum EInvariant inv) const
   * because it avoids object code dependencies of UnicodeString on
   * the conversion code.
   *
   * @param start offset of first character which will be copied
   * @param startLength the number of characters to extract
   * @param target the target buffer for extraction
   * @param targetLength the length of the target buffer
   * @param codepage the desired codepage for the characters.  0 has
   * the special meaning of the default codepage
   * If `codepage` is an empty string (`""`),
   * then a simple conversion is performed on the codepage-invariant
   * subset ("invariant characters") of the platform encoding. See utypes.h.
   * If `target` is nullptr, then the number of bytes required for
   * `target` is returned.
   * @return the output string length, not including the terminating NUL
   * @stable ICU 2.0
   */
  int32_t کڈھو(int32_t شروع,
           int32_t شروع_لمائی,
           حرف *نشانہ,
           uint32_t نشانہ_لمائی,
           ثابت حرف *codepage) ثابت;

  /**
   * Convert the UnicodeString into a codepage string using an existing UConverter.
   * The output string is NUL-terminated if possible.
   *
   * This function avoids the overhead of opening and closing a converter if
   * multiple strings are extracted.
   *
   * @param dest destination string buffer, can be nullptr if destCapacity==0
   * @param destCapacity the number of chars available at dest
   * @param cnv the converter object to be used (ucnv_resetFromUnicode() will be called),
   *        or nullptr for the default converter
   * @param errorCode normal ICU error code
   * @return the length of the output string, not counting the terminating NUL;
   *         if the length is greater than destCapacity, then the string will not fit
   *         and a buffer of the indicated length would need to be passed in
   * @stable ICU 2.0
   */
  int32_t کڈھو(حرف *dest, int32_t dest_سمرتھا,
                  UConverter *cnv,
                  UError_کوڈ &error_کوڈ) ثابت;

#جے_انت

  /**
   * Create a temporary substring for the specified range.
   * Unlike the substring constructor and setTo() functions,
   * the object returned here will be a read-only alias (using getBuffer())
   * rather than copying the text.
   * As a result, this substring operation is much faster but requires
   * that the original string not be modified or deleted during the lifetime
   * of the returned substring object.
   * @param start offset of the first character visible in the substring
   * @param length length of the substring
   * @return a read-only alias UnicodeString object for the substring
   * @stable ICU 4.4
   */
  UnicodeString عارضی_Sub_String(int32_t شروع=0, int32_t لمائی=INT32_MAX) ثابت U_LIFETIME_BOUND;

  /**
   * Create a temporary substring for the specified range.
   * Same as tempSubString(start, length) except that the substring range
   * is specified as a (start, limit) pair (with an exclusive limit index)
   * rather than a (start, length) pair.
   * @param start offset of the first character visible in the substring
   * @param limit offset immediately following the last character visible in the substring
   * @return a read-only alias UnicodeString object for the substring
   * @stable ICU 4.4
   */
  مضمن UnicodeString عارضی_Sub_String_Between(int32_t شروع,
                                            int32_t حد_بندی = INT32_MAX) ثابت U_LIFETIME_BOUND;

  /**
   * Convert the UnicodeString to UTF-8 and write the result
   * to a ByteSink. This is called by toUTF8String().
   * Unpaired surrogates are replaced with U+FFFD.
   * Calls u_strToUTF8WithSub().
   *
   * @param sink A ByteSink to which the UTF-8 version of the string is written.
   *             sink.Flush() is called at the end.
   * @stable ICU 4.2
   * @see toUTF8String
   */
  فراغ منزل_UTF8(ByteSink &sink) ثابت;

  /**
   * Convert the UnicodeString to UTF-8 and append the result
   * to a standard string.
   * Unpaired surrogates are replaced with U+FFFD.
   * Calls toUTF8().
   *
   * @tparam StringClass A std::string or a std::u8string (or a compatible type)
   * @param result A std::string or a std::u8string (or a compatible object)
   *        to which the UTF-8 version of the string is appended.
   * @return The string object.
   * @stable ICU 4.2
   * @see toUTF8
   */
  سانچا <typename StringClass>
  StringClass& منزل_UTF8_String(StringClass& نتیجہ U_LIFETIME_BOUND) ثابت {
    StringByteSink<StringClass> sbs(&نتیجہ, لمائی());
    منزل_UTF8(sbs);
    أرجع نتیجہ;
  }

#جے_بنا_تعریف U_HIDE_DRAFT_API
  /**
   * Convert the UnicodeString to a UTF-8 string.
   * Unpaired surrogates are replaced with U+FFFD.
   * Calls toUTF8().
   *
   * @tparam StringClass A std::string or a std::u8string (or a compatible type)
   * @return A std::string or a std::u8string (or a compatible object)
   *        with the UTF-8 version of the string.
   * @draft ICU 78
   * @see toUTF8
   */
  سانچا<typename StringClass>
  StringClass منزل_UTF8_String() ثابت {
    StringClass نتیجہ;
    StringByteSink<StringClass> sbs(&نتیجہ, لمائی());
    منزل_UTF8(sbs);
    أرجع نتیجہ;
  }
#جے_انت  // U_HIDE_DRAFT_API

  /**
   * Convert the UnicodeString to UTF-32.
   * Unpaired surrogates are replaced with U+FFFD.
   * Calls u_strToUTF32WithSub().
   *
   * @param utf32 destination string buffer, can be nullptr if capacity==0
   * @param capacity the number of UChar32s available at utf32
   * @param errorCode Standard ICU error code. Its input value must
   *                  pass the U_SUCCESS() test, or else the function returns
   *                  immediately. Check for U_FAILURE() on output or use with
   *                  function chaining. (See User Guide for details.)
   * @return The length of the UTF-32 string.
   * @see fromUTF32
   * @stable ICU 4.2
   */
  int32_t منزل_UTF32(UChar32 *utf32, int32_t سمرتھا, UError_کوڈ &error_کوڈ) ثابت;

  /* Length operations */

  /**
   * Return the length of the UnicodeString object.
   * The length is the number of char16_t code units are in the UnicodeString.
   * If you want the number of code points, please use countChar32().
   * @return the length of the UnicodeString object
   * @see countChar32
   * @stable ICU 2.0
   */
  مضمن int32_t لمائی() ثابت;

  /**
   * Count Unicode code points in the length char16_t code units of the string.
   * A code point may occupy either one or two char16_t code units.
   * Counting code points involves reading all code units.
   *
   * This functions is basically the inverse of moveIndex32().
   *
   * @param start the index of the first code unit to check
   * @param length the number of char16_t code units to check
   * @return the number of code points in the specified code units
   * @see length
   * @stable ICU 2.0
   */
  int32_t
  گنتی_Char32(int32_t شروع=0, int32_t لمائی=INT32_MAX) ثابت;

  /**
   * Check if the length char16_t code units of the string
   * contain more Unicode code points than a certain number.
   * This is more efficient than counting all code points in this part of the string
   * and comparing that number with a threshold.
   * This function may not need to scan the string at all if the length
   * falls within a certain range, and
   * never needs to count more than 'number+1' code points.
   * Logically equivalent to (countChar32(start, length)>number).
   * A Unicode code point may occupy either one or two char16_t code units.
   *
   * @param start the index of the first code unit to check (0 for the entire string)
   * @param length the number of char16_t code units to check
   *               (use INT32_MAX for the entire string; remember that start/length
   *                values are pinned)
   * @param number The number of code points in the (sub)string is compared against
   *               the 'number' parameter.
   * @return Boolean value for whether the string contains more Unicode code points
   *         than 'number'. Same as (u_countChar32(s, length)>number).
   * @see countChar32
   * @see u_strHasMoreChar32Than
   * @stable ICU 2.4
   */
  UBool
  has_ہور_Char32_Than(int32_t شروع, int32_t لمائی, int32_t نمبر) ثابت;

  /**
   * Determine if this string is empty.
   * @return true if this string contains 0 characters, false otherwise.
   * @stable ICU 2.0
   */
  مضمن UBool is_خالی_تھاواں() ثابت;

  /**
   * Return the capacity of the internal buffer of the UnicodeString object.
   * This is useful together with the getBuffer functions.
   * See there for details.
   *
   * @return the number of char16_ts available in the internal buffer
   * @see getBuffer
   * @stable ICU 2.0
   */
  مضمن int32_t get_سمرتھا() ثابت;

  /* Other operations */

  /**
   * Generate a hash code for this object.
   * @return The hash code of this UnicodeString.
   * @stable ICU 2.0
   */
  مضمن int32_t hash_کوڈ() ثابت;

  /**
   * Determine if this object contains a valid string.
   * A bogus string has no value. It is different from an empty string,
   * although in both cases isEmpty() returns true and length() returns 0.
   * setToBogus() and isBogus() can be used to indicate that no string value is available.
   * For a bogus string, getBuffer() and getTerminatedBuffer() return nullptr, and
   * length() returns 0.
   *
   * @return true if the string is bogus/invalid, false otherwise
   * @see setToBogus()
   * @stable ICU 2.0
   */
  مضمن UBool isBogus() ثابت;

#جے_بنا_تعریف U_HIDE_DRAFT_API
خاص:
  // These type aliases are private; there is no guarantee that they will remain
  // aliases to the same types in subsequent versions of ICU.
  // Note that whether `std::u16string_view::const_iterator` is a pointer or a
  // class that models contiguous_iterator is platform-dependent.
  using unspecified_iterator = std::u16string_view::const_iterator;
  using unspecified_الٹ_iterator = std::u16string_view::const_الٹ_iterator;

عام:
  /**
   * @return an iterator to the first code unit in this string.
   *     The iterator may be a pointer or a contiguous-iterator object.
   * @draft ICU 78
   */
  unspecified_iterator begin() ثابت { أرجع std::u16string_view(*this).begin(); }
  /**
   * @return an iterator to just past the last code unit in this string.
   *     The iterator may be a pointer or a contiguous-iterator object.
   * @draft ICU 78
   */
  unspecified_iterator سرا() ثابت { أرجع std::u16string_view(*this).سرا(); }
  /**
   * @return a reverse iterator to the last code unit in this string.
   *     The iterator may be a pointer or a contiguous-iterator object.
   * @draft ICU 78
   */
  unspecified_الٹ_iterator rbegin() ثابت { أرجع std::u16string_view(*this).rbegin(); }
  /**
   * @return a reverse iterator to just before the first code unit in this string.
   *     The iterator may be a pointer or a contiguous-iterator object.
   * @draft ICU 78
   */
  unspecified_الٹ_iterator rend() ثابت { أرجع std::u16string_view(*this).rend(); }
#جے_انت  // U_HIDE_DRAFT_API

  //========================================
  // Write operations
  //========================================

  /* Assignment operations */

  /**
   * Assignment operator.  Replace the characters in this UnicodeString
   * with the characters from `srcText`.
   *
   * Starting with ICU 2.4, the assignment operator and the copy constructor
   * allocate a new buffer and copy the buffer contents even for readonly aliases.
   * By contrast, the fastCopyFrom() function implements the old,
   * more efficient but less safe behavior
   * of making this string also a readonly alias to the same buffer.
   *
   * If the source object has an "open" buffer from getBuffer(minCapacity),
   * then the copy is an empty string.
   *
   * @param srcText The text containing the characters to replace
   * @return a reference to this
   * @stable ICU 2.0
   * @see fastCopyFrom
   */
  UnicodeString& عامل=(ثابت UnicodeString& src_متن) U_LIFETIME_BOUND;

  /**
   * Almost the same as the assignment operator.
   * Replace the characters in this UnicodeString
   * with the characters from `srcText`.
   *
   * This function works the same as the assignment operator
   * for all strings except for ones that are readonly aliases.
   *
   * Starting with ICU 2.4, the assignment operator and the copy constructor
   * allocate a new buffer and copy the buffer contents even for readonly aliases.
   * This function implements the old, more efficient but less safe behavior
   * of making this string also a readonly alias to the same buffer.
   *
   * The fastCopyFrom function must be used only if it is known that the lifetime of
   * this UnicodeString does not exceed the lifetime of the aliased buffer
   * including its contents, for example for strings from resource bundles
   * or aliases to string constants.
   *
   * If the source object has an "open" buffer from getBuffer(minCapacity),
   * then the copy is an empty string.
   *
   * @param src The text containing the characters to replace.
   * @return a reference to this
   * @stable ICU 2.4
   */
  UnicodeString& fastCopyFrom(ثابت UnicodeString& src) U_LIFETIME_BOUND;

  /**
   * Assignment operator. Replaces the characters in this UnicodeString
   * with a copy of the characters from the `src`
   * which is, or which is implicitly convertible to,
   * a std::u16string_view or (if U_SIZEOF_WCHAR_T==2) std::wstring_view.
   *
   * @param src The string view containing the characters to copy.
   * @return a reference to this
   * @stable ICU 76
   */
  سانچا <typename S, typename = std::enable_if_t<Convertible_منزل_U16_String_View<S>>>
  مضمن UnicodeString& عامل=(ثابت S& src) U_LIFETIME_BOUND {
    unBogus();
    أرجع doReplace(0, لمائی(), اندرونی_2::منزل_U16_String_View(src));
  }

  /**
   * Move assignment operator; might leave src in bogus state.
   * This string will have the same contents and state that the source string had.
   * The behavior is undefined if *this and src are the same object.
   * @param src source string
   * @return *this
   * @stable ICU 56
   */
  UnicodeString& عامل=(UnicodeString&& src) noexcept U_LIFETIME_BOUND;

  /**
   * Swap strings.
   * @param other other string
   * @stable ICU 56
   */
  فراغ وٹاندرا(UnicodeString &other) noexcept;

  /**
   * Non-member UnicodeString swap function.
   * @param s1 will get s2's contents and state
   * @param s2 will get s1's contents and state
   * @stable ICU 56
   */
  یار مضمن فراغ U_EXPORT2
  وٹاندرا(UnicodeString &s1, UnicodeString &s2) noexcept {
    s1.وٹاندرا(s2);
  }

  /**
   * Assignment operator.  Replace the characters in this UnicodeString
   * with the code unit `ch`.
   * @param ch the code unit to replace
   * @return a reference to this
   * @stable ICU 2.0
   */
  مضمن UnicodeString& عامل=(char16_t ch) U_LIFETIME_BOUND;

  /**
   * Assignment operator.  Replace the characters in this UnicodeString
   * with the code point `ch`.
   * @param ch the code point to replace
   * @return a reference to this
   * @stable ICU 2.0
   */
  مضمن UnicodeString& عامل=(UChar32 ch) U_LIFETIME_BOUND;

  /**
   * Set the text in the UnicodeString object to the characters
   * in `srcText` in the range
   * [`srcStart`, `srcText.length()`).
   * `srcText` is not modified.
   * @param srcText the source for the new characters
   * @param srcStart the offset into `srcText` where new characters
   * will be obtained
   * @return a reference to this
   * @stable ICU 2.2
   */
  مضمن UnicodeString& سیٹ_منزل(ثابت UnicodeString& src_متن, int32_t src_شروع) U_LIFETIME_BOUND;

  /**
   * Set the text in the UnicodeString object to the characters
   * in `srcText` in the range
   * [`srcStart`, `srcStart + srcLength`).
   * `srcText` is not modified.
   * @param srcText the source for the new characters
   * @param srcStart the offset into `srcText` where new characters
   * will be obtained
   * @param srcLength the number of characters in `srcText` in the
   * replace string.
   * @return a reference to this
   * @stable ICU 2.0
   */
  مضمن UnicodeString& سیٹ_منزل(ثابت UnicodeString& src_متن,
                              int32_t src_شروع,
                              int32_t src_لمائی) U_LIFETIME_BOUND;

  /**
   * Set the text in the UnicodeString object to the characters in
   * `srcText`.
   * `srcText` is not modified.
   * @param srcText the source for the new characters
   * @return a reference to this
   * @stable ICU 2.0
   */
  مضمن UnicodeString& سیٹ_منزل(ثابت UnicodeString& src_متن) U_LIFETIME_BOUND;

  /**
   * Set the characters in the UnicodeString object to the characters
   * in `srcChars`. `srcChars` is not modified.
   * @param srcChars the source for the new characters
   * @param srcLength the number of Unicode characters in srcChars.
   * @return a reference to this
   * @stable ICU 2.0
   */
  مضمن UnicodeString& سیٹ_منزل(ثابت char16_t* srcChars, int32_t src_لمائی) U_LIFETIME_BOUND;

  /**
   * Set the characters in the UnicodeString object to the code unit
   * `srcChar`.
   * @param srcChar the code unit which becomes the UnicodeString's character
   * content
   * @return a reference to this
   * @stable ICU 2.0
   */
  مضمن UnicodeString& سیٹ_منزل(char16_t srcChar) U_LIFETIME_BOUND;

  /**
   * Set the characters in the UnicodeString object to the code point
   * `srcChar`.
   * @param srcChar the code point which becomes the UnicodeString's character
   * content
   * @return a reference to this
   * @stable ICU 2.0
   */
  مضمن UnicodeString& سیٹ_منزل(UChar32 srcChar) U_LIFETIME_BOUND;

  /**
   * Aliasing setTo() function, analogous to the readonly-aliasing char16_t* constructor.
   * The text will be used for the UnicodeString object, but
   * it will not be released when the UnicodeString is destroyed.
   * This has copy-on-write semantics:
   * When the string is modified, then the buffer is first copied into
   * newly allocated memory.
   * The aliased buffer is never modified.
   *
   * In an assignment to another UnicodeString, when using the copy constructor
   * or the assignment operator, the text will be copied.
   * When using fastCopyFrom(), the text will be aliased again,
   * so that both strings then alias the same readonly-text.
   *
   * @param isTerminated specifies if `text` is `NUL`-terminated.
   *                     This must be true if `textLength==-1`.
   * @param text The characters to alias for the UnicodeString.
   * @param textLength The number of Unicode characters in `text` to alias.
   *                   If -1, then this constructor will determine the length
   *                   by calling `u_strlen()`.
   * @return a reference to this
   * @stable ICU 2.0
   */
  UnicodeString& سیٹ_منزل(UBool isTerminated,
                       ConstChar16Ptr متن U_LIFETIME_BOUND,
                       int32_t متن_لمائی) U_LIFETIME_BOUND;

  /**
   * Aliasing setTo() function, analogous to the writable-aliasing char16_t* constructor.
   * The text will be used for the UnicodeString object, but
   * it will not be released when the UnicodeString is destroyed.
   * This has write-through semantics:
   * For as long as the capacity of the buffer is sufficient, write operations
   * will directly affect the buffer. When more capacity is necessary, then
   * a new buffer will be allocated and the contents copied as with regularly
   * constructed strings.
   * In an assignment to another UnicodeString, the buffer will be copied.
   * The extract(Char16Ptr dst) function detects whether the dst pointer is the same
   * as the string buffer itself and will in this case not copy the contents.
   *
   * @param buffer The characters to alias for the UnicodeString.
   * @param buffLength The number of Unicode characters in `buffer` to alias.
   * @param buffCapacity The size of `buffer` in char16_ts.
   * @return a reference to this
   * @stable ICU 2.0
   */
  UnicodeString& سیٹ_منزل(char16_t* بفر U_LIFETIME_BOUND,
                       int32_t buff_لمائی,
                       int32_t buff_سمرتھا) U_LIFETIME_BOUND;

  /**
   * Make this UnicodeString object invalid.
   * The string will test true with isBogus().
   *
   * A bogus string has no value. It is different from an empty string.
   * It can be used to indicate that no string value is available.
   * getBuffer() and getTerminatedBuffer() return nullptr, and
   * length() returns 0.
   *
   * This utility function is used throughout the UnicodeString
   * implementation to indicate that a UnicodeString operation failed,
   * and may be used in other functions,
   * especially but not exclusively when such functions do not
   * take a UErrorCode for simplicity.
   *
   * The following methods, and no others, will clear a string object's bogus flag:
   * - remove()
   * - remove(0, INT32_MAX)
   * - truncate(0)
   * - operator=() (assignment operator)
   * - setTo(...)
   *
   * The simplest ways to turn a bogus string into an empty one
   * is to use the remove() function.
   * Examples for other functions that are equivalent to "set to empty string":
   * \code
   * if(s.isBogus()) {
   *   s.remove();           // set to an empty string (remove all), or
   *   s.remove(0, INT32_MAX); // set to an empty string (remove all), or
   *   s.truncate(0);        // set to an empty string (complete truncation), or
   *   s=UnicodeString();    // assign an empty string, or
   *   s.setTo((UChar32)-1); // set to a pseudo code point that is out of range, or
   *   s.setTo(u"", 0);      // set to an empty C Unicode string
   * }
   * \endcode
   *
   * @see isBogus()
   * @stable ICU 2.0
   */
  فراغ سیٹ_منزل_Bogus();

  /**
   * Set the character at the specified offset to the specified character.
   * @param offset A valid offset into the text of the character to set
   * @param ch The new character
   * @return A reference to this
   * @stable ICU 2.0
   */
  UnicodeString& سیٹ_Char_At(int32_t وتھ, char16_t ch) U_LIFETIME_BOUND;

  /* Append operations */

  /**
   * Append operator. Append the code unit `ch` to the UnicodeString
   * object.
   * @param ch the code unit to be appended
   * @return a reference to this
   * @stable ICU 2.0
   */
  مضمن UnicodeString& عامل+=(char16_t ch) U_LIFETIME_BOUND;

  /**
   * Append operator. Append the code point `ch` to the UnicodeString
   * object.
   * @param ch the code point to be appended
   * @return a reference to this
   * @stable ICU 2.0
   */
  مضمن UnicodeString& عامل+=(UChar32 ch) U_LIFETIME_BOUND;

  /**
   * Append operator. Append the characters in `srcText` to the
   * UnicodeString object. `srcText` is not modified.
   * @param srcText the source for the new characters
   * @return a reference to this
   * @stable ICU 2.0
   */
  مضمن UnicodeString& عامل+=(ثابت UnicodeString& src_متن) U_LIFETIME_BOUND;

  /**
   * Append operator. Appends the characters in `src`
   * which is, or which is implicitly convertible to,
   * a std::u16string_view or (if U_SIZEOF_WCHAR_T==2) std::wstring_view,
   * to the UnicodeString object.
   *
   * @param src the source for the new characters
   * @return a reference to this
   * @stable ICU 76
   */
  سانچا <typename S, typename = std::enable_if_t<Convertible_منزل_U16_String_View<S>>>
  مضمن UnicodeString& عامل+=(ثابت S& src) U_LIFETIME_BOUND {
    أرجع doAppend(اندرونی_2::منزل_U16_String_View(src));
  }

  /**
   * Append the characters
   * in `srcText` in the range
   * [`srcStart`, `srcStart + srcLength`) to the
   * UnicodeString object at offset `start`. `srcText`
   * is not modified.
   * @param srcText the source for the new characters
   * @param srcStart the offset into `srcText` where new characters
   * will be obtained
   * @param srcLength the number of characters in `srcText` in
   * the append string
   * @return a reference to this
   * @stable ICU 2.0
   */
  مضمن UnicodeString& append(ثابت UnicodeString& src_متن,
                               int32_t src_شروع,
                               int32_t src_لمائی) U_LIFETIME_BOUND;

  /**
   * Append the characters in `srcText` to the UnicodeString object.
   * `srcText` is not modified.
   * @param srcText the source for the new characters
   * @return a reference to this
   * @stable ICU 2.0
   */
  مضمن UnicodeString& append(ثابت UnicodeString& src_متن) U_LIFETIME_BOUND;

  /**
   * Append the characters in `srcChars` in the range
   * [`srcStart`, `srcStart + srcLength`) to the UnicodeString
   * object at offset
   * `start`. `srcChars` is not modified.
   * @param srcChars the source for the new characters
   * @param srcStart the offset into `srcChars` where new characters
   * will be obtained
   * @param srcLength the number of characters in `srcChars` in
   *                  the append string; can be -1 if `srcChars` is NUL-terminated
   * @return a reference to this
   * @stable ICU 2.0
   */
  مضمن UnicodeString& append(ثابت char16_t* srcChars,
                               int32_t src_شروع,
                               int32_t src_لمائی) U_LIFETIME_BOUND;

  /**
   * Append the characters in `srcChars` to the UnicodeString object.
   * `srcChars` is not modified.
   * @param srcChars the source for the new characters
   * @param srcLength the number of Unicode characters in `srcChars`;
   *                  can be -1 if `srcChars` is NUL-terminated
   * @return a reference to this
   * @stable ICU 2.0
   */
  مضمن UnicodeString& append(ConstChar16Ptr srcChars, int32_t src_لمائی) U_LIFETIME_BOUND;

  /**
   * Appends the characters in `src`
   * which is, or which is implicitly convertible to,
   * a std::u16string_view or (if U_SIZEOF_WCHAR_T==2) std::wstring_view,
   * to the UnicodeString object.
   *
   * @param src the source for the new characters
   * @return a reference to this
   * @stable ICU 76
   */
  سانچا <typename S, typename = std::enable_if_t<Convertible_منزل_U16_String_View<S>>>
  مضمن UnicodeString& append(ثابت S& src) U_LIFETIME_BOUND {
    أرجع doAppend(اندرونی_2::منزل_U16_String_View(src));
  }

  /**
   * Append the code unit `srcChar` to the UnicodeString object.
   * @param srcChar the code unit to append
   * @return a reference to this
   * @stable ICU 2.0
   */
  مضمن UnicodeString& append(char16_t srcChar) U_LIFETIME_BOUND;

  /**
   * Append the code point `srcChar` to the UnicodeString object.
   * @param srcChar the code point to append
   * @return a reference to this
   * @stable ICU 2.0
   */
  UnicodeString& append(UChar32 srcChar) U_LIFETIME_BOUND;

#جے_بنا_تعریف U_HIDE_DRAFT_API
  /**
   * Appends the code unit `c` to the UnicodeString object.
   * Same as append(c) except does not return *this.
   *
   * @param c the code unit to append
   * @draft ICU 78
   */
  مضمن فراغ ڈالو_پٹھ(char16_t c) { append(c); }
#جے_انت  // U_HIDE_DRAFT_API

  /* Insert operations */

  /**
   * Insert the characters in `srcText` in the range
   * [`srcStart`, `srcStart + srcLength`) into the UnicodeString
   * object at offset `start`. `srcText` is not modified.
   * @param start the offset where the insertion begins
   * @param srcText the source for the new characters
   * @param srcStart the offset into `srcText` where new characters
   * will be obtained
   * @param srcLength the number of characters in `srcText` in
   * the insert string
   * @return a reference to this
   * @stable ICU 2.0
   */
  مضمن UnicodeString& داخل_کرو(int32_t شروع,
                               ثابت UnicodeString& src_متن,
                               int32_t src_شروع,
                               int32_t src_لمائی) U_LIFETIME_BOUND;

  /**
   * Insert the characters in `srcText` into the UnicodeString object
   * at offset `start`. `srcText` is not modified.
   * @param start the offset where the insertion begins
   * @param srcText the source for the new characters
   * @return a reference to this
   * @stable ICU 2.0
   */
  مضمن UnicodeString& داخل_کرو(int32_t شروع, ثابت UnicodeString& src_متن) U_LIFETIME_BOUND;

  /**
   * Insert the characters in `srcChars` in the range
   * [`srcStart`, `srcStart + srcLength`) into the UnicodeString
   *  object at offset `start`. `srcChars` is not modified.
   * @param start the offset at which the insertion begins
   * @param srcChars the source for the new characters
   * @param srcStart the offset into `srcChars` where new characters
   * will be obtained
   * @param srcLength the number of characters in `srcChars`
   * in the insert string
   * @return a reference to this
   * @stable ICU 2.0
   */
  مضمن UnicodeString& داخل_کرو(int32_t شروع,
                               ثابت char16_t* srcChars,
                               int32_t src_شروع,
                               int32_t src_لمائی) U_LIFETIME_BOUND;

  /**
   * Insert the characters in `srcChars` into the UnicodeString object
   * at offset `start`. `srcChars` is not modified.
   * @param start the offset where the insertion begins
   * @param srcChars the source for the new characters
   * @param srcLength the number of Unicode characters in srcChars.
   * @return a reference to this
   * @stable ICU 2.0
   */
  مضمن UnicodeString& داخل_کرو(int32_t شروع,
                               ConstChar16Ptr srcChars,
                               int32_t src_لمائی) U_LIFETIME_BOUND;

  /**
   * Insert the code unit `srcChar` into the UnicodeString object at
   * offset `start`.
   * @param start the offset at which the insertion occurs
   * @param srcChar the code unit to insert
   * @return a reference to this
   * @stable ICU 2.0
   */
  مضمن UnicodeString& داخل_کرو(int32_t شروع, char16_t srcChar) U_LIFETIME_BOUND;

  /**
   * Insert the code point `srcChar` into the UnicodeString object at
   * offset `start`.
   * @param start the offset at which the insertion occurs
   * @param srcChar the code point to insert
   * @return a reference to this
   * @stable ICU 2.0
   */
  مضمن UnicodeString& داخل_کرو(int32_t شروع, UChar32 srcChar) U_LIFETIME_BOUND;

  /* Replace operations */

  /**
   * Replace the characters in the range
   * [`start`, `start + length`) with the characters in
   * `srcText` in the range
   * [`srcStart`, `srcStart + srcLength`).
   * `srcText` is not modified.
   * @param start the offset at which the replace operation begins
   * @param length the number of characters to replace. The character at
   * `start + length` is not modified.
   * @param srcText the source for the new characters
   * @param srcStart the offset into `srcText` where new characters
   * will be obtained
   * @param srcLength the number of characters in `srcText` in
   * the replace string
   * @return a reference to this
   * @stable ICU 2.0
   */
  مضمن UnicodeString& replace(int32_t شروع,
                                int32_t لمائی,
                                ثابت UnicodeString& src_متن,
                                int32_t src_شروع,
                                int32_t src_لمائی) U_LIFETIME_BOUND;

  /**
   * Replace the characters in the range
   * [`start`, `start + length`)
   * with the characters in `srcText`.  `srcText` is
   *  not modified.
   * @param start the offset at which the replace operation begins
   * @param length the number of characters to replace. The character at
   * `start + length` is not modified.
   * @param srcText the source for the new characters
   * @return a reference to this
   * @stable ICU 2.0
   */
  مضمن UnicodeString& replace(int32_t شروع,
                                int32_t لمائی,
                                ثابت UnicodeString& src_متن) U_LIFETIME_BOUND;

  /**
   * Replace the characters in the range
   * [`start`, `start + length`) with the characters in
   * `srcChars` in the range
   * [`srcStart`, `srcStart + srcLength`). `srcChars`
   * is not modified.
   * @param start the offset at which the replace operation begins
   * @param length the number of characters to replace.  The character at
   * `start + length` is not modified.
   * @param srcChars the source for the new characters
   * @param srcStart the offset into `srcChars` where new characters
   * will be obtained
   * @param srcLength the number of characters in `srcChars`
   * in the replace string
   * @return a reference to this
   * @stable ICU 2.0
   */
  مضمن UnicodeString& replace(int32_t شروع,
                                int32_t لمائی,
                                ثابت char16_t* srcChars,
                                int32_t src_شروع,
                                int32_t src_لمائی) U_LIFETIME_BOUND;

  /**
   * Replace the characters in the range
   * [`start`, `start + length`) with the characters in
   * `srcChars`.  `srcChars` is not modified.
   * @param start the offset at which the replace operation begins
   * @param length number of characters to replace.  The character at
   * `start + length` is not modified.
   * @param srcChars the source for the new characters
   * @param srcLength the number of Unicode characters in srcChars
   * @return a reference to this
   * @stable ICU 2.0
   */
  مضمن UnicodeString& replace(int32_t شروع,
                                int32_t لمائی,
                                ConstChar16Ptr srcChars,
                                int32_t src_لمائی) U_LIFETIME_BOUND;

  /**
   * Replace the characters in the range
   * [`start`, `start + length`) with the code unit
   * `srcChar`.
   * @param start the offset at which the replace operation begins
   * @param length the number of characters to replace.  The character at
   * `start + length` is not modified.
   * @param srcChar the new code unit
   * @return a reference to this
   * @stable ICU 2.0
   */
  مضمن UnicodeString& replace(int32_t شروع, int32_t لمائی, char16_t srcChar) U_LIFETIME_BOUND;

  /**
   * Replace the characters in the range
   * [`start`, `start + length`) with the code point
   * `srcChar`.
   * @param start the offset at which the replace operation begins
   * @param length the number of characters to replace.  The character at
   * `start + length` is not modified.
   * @param srcChar the new code point
   * @return a reference to this
   * @stable ICU 2.0
   */
  UnicodeString& replace(int32_t شروع, int32_t لمائی, UChar32 srcChar) U_LIFETIME_BOUND;

  /**
   * Replace the characters in the range [`start`, `limit`)
   * with the characters in `srcText`. `srcText` is not modified.
   * @param start the offset at which the replace operation begins
   * @param limit the offset immediately following the replace range
   * @param srcText the source for the new characters
   * @return a reference to this
   * @stable ICU 2.0
   */
  مضمن UnicodeString& replaceBetween(int32_t شروع,
                                       int32_t حد_بندی,
                                       ثابت UnicodeString& src_متن) U_LIFETIME_BOUND;

  /**
   * Replace the characters in the range [`start`, `limit`)
   * with the characters in `srcText` in the range
   * [`srcStart`, `srcLimit`). `srcText` is not modified.
   * @param start the offset at which the replace operation begins
   * @param limit the offset immediately following the replace range
   * @param srcText the source for the new characters
   * @param srcStart the offset into `srcChars` where new characters
   * will be obtained
   * @param srcLimit the offset immediately following the range to copy
   * in `srcText`
   * @return a reference to this
   * @stable ICU 2.0
   */
  مضمن UnicodeString& replaceBetween(int32_t شروع,
                                       int32_t حد_بندی,
                                       ثابت UnicodeString& src_متن,
                                       int32_t src_شروع,
                                       int32_t src_حد_بندی) U_LIFETIME_BOUND;

  /**
   * Replace a substring of this object with the given text.
   * @param start the beginning index, inclusive; `0 <= start <= limit`.
   * @param limit the ending index, exclusive; `start <= limit <= length()`.
   * @param text the text to replace characters `start` to `limit - 1`
   * @stable ICU 2.0
   */
  مجازی فراغ handleReplaceBetween(int32_t شروع,
                                    int32_t حد_بندی,
                                    ثابت UnicodeString& متن) override;

  /**
   * Replaceable API
   * @return true if it has MetaData
   * @stable ICU 2.4
   */
  مجازی UBool has_Meta_انکڑے() ثابت override;

  /**
   * Copy a substring of this object, retaining attribute (out-of-band)
   * information.  This method is used to duplicate or reorder substrings.
   * The destination index must not overlap the source range.
   *
   * @param start the beginning index, inclusive; `0 <= start <= limit`.
   * @param limit the ending index, exclusive; `start <= limit <= length()`.
   * @param dest the destination index.  The characters from
   *             `start..limit-1` will be copied to `dest`.
   * Implementations of this method may assume that `dest <= start ||
   * dest >= limit`.
   * @stable ICU 2.0
   */
  مجازی فراغ copy(int32_t شروع, int32_t حد_بندی, int32_t dest) override;

  /* Search and replace operations */

  /**
   * Replace all occurrences of characters in oldText with the characters
   * in newText
   * @param oldText the text containing the search text
   * @param newText the text containing the replacement text
   * @return a reference to this
   * @stable ICU 2.0
   */
  مضمن UnicodeString& لبھو_And_Replace(ثابت UnicodeString& old_متن,
                                       ثابت UnicodeString& new_متن) U_LIFETIME_BOUND;

  /**
   * Replace all occurrences of characters in oldText with characters
   * in newText
   * in the range [`start`, `start + length`).
   * @param start the start of the range in which replace will performed
   * @param length the length of the range in which replace will be performed
   * @param oldText the text containing the search text
   * @param newText the text containing the replacement text
   * @return a reference to this
   * @stable ICU 2.0
   */
  مضمن UnicodeString& لبھو_And_Replace(int32_t شروع,
                                       int32_t لمائی,
                                       ثابت UnicodeString& old_متن,
                                       ثابت UnicodeString& new_متن) U_LIFETIME_BOUND;

  /**
   * Replace all occurrences of characters in oldText in the range
   * [`oldStart`, `oldStart + oldLength`) with the characters
   * in newText in the range
   * [`newStart`, `newStart + newLength`)
   * in the range [`start`, `start + length`).
   * @param start the start of the range in which replace will performed
   * @param length the length of the range in which replace will be performed
   * @param oldText the text containing the search text
   * @param oldStart the start of the search range in `oldText`
   * @param oldLength the length of the search range in `oldText`
   * @param newText the text containing the replacement text
   * @param newStart the start of the replacement range in `newText`
   * @param newLength the length of the replacement range in `newText`
   * @return a reference to this
   * @stable ICU 2.0
   */
  UnicodeString& لبھو_And_Replace(int32_t شروع,
                                int32_t لمائی,
                                ثابت UnicodeString& old_متن,
                                int32_t old_شروع,
                                int32_t old_لمائی,
                                ثابت UnicodeString& new_متن,
                                int32_t new_شروع,
                                int32_t new_لمائی) U_LIFETIME_BOUND;


  /* Remove operations */

  /**
   * Removes all characters from the UnicodeString object and clears the bogus flag.
   * This is the UnicodeString equivalent of std::string’s clear().
   *
   * @return a reference to this
   * @see setToBogus
   * @stable ICU 2.0
   */
  مضمن UnicodeString& remove() U_LIFETIME_BOUND;

  /**
   * Remove the characters in the range
   * [`start`, `start + length`) from the UnicodeString object.
   * @param start the offset of the first character to remove
   * @param length the number of characters to remove
   * @return a reference to this
   * @stable ICU 2.0
   */
  مضمن UnicodeString& remove(int32_t شروع,
                               int32_t لمائی = static_cast<int32_t>(INT32_MAX)) U_LIFETIME_BOUND;

  /**
   * Remove the characters in the range
   * [`start`, `limit`) from the UnicodeString object.
   * @param start the offset of the first character to remove
   * @param limit the offset immediately following the range to remove
   * @return a reference to this
   * @stable ICU 2.0
   */
  مضمن UnicodeString&
  removeBetween(int32_t شروع, int32_t حد_بندی = static_cast<int32_t>(INT32_MAX)) U_LIFETIME_BOUND;

  /**
   * Retain only the characters in the range
   * [`start`, `limit`) from the UnicodeString object.
   * Removes characters before `start` and at and after `limit`.
   * @param start the offset of the first character to retain
   * @param limit the offset immediately following the range to retain
   * @return a reference to this
   * @stable ICU 4.4
   */
  مضمن UnicodeString& retainBetween(int32_t شروع, int32_t حد_بندی = INT32_MAX) U_LIFETIME_BOUND;

  /* Length operations */

  /**
   * Pad the start of this UnicodeString with the character `padChar`.
   * If the length of this UnicodeString is less than targetLength,
   * length() - targetLength copies of padChar will be added to the
   * beginning of this UnicodeString.
   * @param targetLength the desired length of the string
   * @param padChar the character to use for padding. Defaults to
   * space (U+0020)
   * @return true if the text was padded, false otherwise.
   * @stable ICU 2.0
   */
  UBool padLeading(int32_t نشانہ_لمائی,
                    char16_t padChar = 0x0020);

  /**
   * Pad the end of this UnicodeString with the character `padChar`.
   * If the length of this UnicodeString is less than targetLength,
   * length() - targetLength copies of padChar will be added to the
   * end of this UnicodeString.
   * @param targetLength the desired length of the string
   * @param padChar the character to use for padding. Defaults to
   * space (U+0020)
   * @return true if the text was padded, false otherwise.
   * @stable ICU 2.0
   */
  UBool padTrailing(int32_t نشانہ_لمائی,
                     char16_t padChar = 0x0020);

  /**
   * Truncate this UnicodeString to the `targetLength`.
   * @param targetLength the desired length of this UnicodeString.
   * @return true if the text was truncated, false otherwise
   * @stable ICU 2.0
   */
  مضمن UBool truncate(int32_t نشانہ_لمائی);

  /**
   * Trims leading and trailing whitespace from this UnicodeString.
   * @return a reference to this
   * @stable ICU 2.0
   */
  UnicodeString& trim() U_LIFETIME_BOUND;

  /* Miscellaneous operations */

  /**
   * Reverse this UnicodeString in place.
   * @return a reference to this
   * @stable ICU 2.0
   */
  مضمن UnicodeString& الٹ() U_LIFETIME_BOUND;

  /**
   * Reverse the range [`start`, `start + length`) in
   * this UnicodeString.
   * @param start the start of the range to reverse
   * @param length the number of characters to to reverse
   * @return a reference to this
   * @stable ICU 2.0
   */
  مضمن UnicodeString& الٹ(int32_t شروع, int32_t لمائی) U_LIFETIME_BOUND;

  /**
   * Convert the characters in this to UPPER CASE following the conventions of
   * the default locale.
   * @return A reference to this.
   * @stable ICU 2.0
   */
  UnicodeString& منزل_اتلا() U_LIFETIME_BOUND;

  /**
   * Convert the characters in this to UPPER CASE following the conventions of
   * a specific locale.
   * @param locale The locale containing the conventions to use.
   * @return A reference to this.
   * @stable ICU 2.0
   */
  UnicodeString& منزل_اتلا(ثابت Locale& locale) U_LIFETIME_BOUND;

  /**
   * Convert the characters in this to lower case following the conventions of
   * the default locale.
   * @return A reference to this.
   * @stable ICU 2.0
   */
  UnicodeString& منزل_ہیٹھلا() U_LIFETIME_BOUND;

  /**
   * Convert the characters in this to lower case following the conventions of
   * a specific locale.
   * @param locale The locale containing the conventions to use.
   * @return A reference to this.
   * @stable ICU 2.0
   */
  UnicodeString& منزل_ہیٹھلا(ثابت Locale& locale) U_LIFETIME_BOUND;

#إذا !UCONFIG_NO_BREAK_ITERATION

  /**
   * Titlecase this string, convenience function using the default locale.
   *
   * Casing is locale-dependent and context-sensitive.
   * Titlecasing uses a break iterator to find the first characters of words
   * that are to be titlecased. It titlecases those characters and lowercases
   * all others.
   *
   * The titlecase break iterator can be provided to customize for arbitrary
   * styles, using rules and dictionaries beyond the standard iterators.
   * It may be more efficient to always provide an iterator to avoid
   * opening and closing one for each string.
   * If the break iterator passed in is null, the default Unicode algorithm
   * will be used to determine the titlecase positions.
   *
   * This function uses only the setText(), first() and next() methods of the
   * provided break iterator.
   *
   * @param titleIter A break iterator to find the first characters of words
   *                  that are to be titlecased.
   *                  If none is provided (0), then a standard titlecase
   *                  break iterator is opened.
   *                  Otherwise the provided iterator is set to the string's text.
   * @return A reference to this.
   * @stable ICU 2.1
   */
  UnicodeString& منزل_Title(BreakIterator* titleIter) U_LIFETIME_BOUND;

  /**
   * Titlecase this string.
   *
   * Casing is locale-dependent and context-sensitive.
   * Titlecasing uses a break iterator to find the first characters of words
   * that are to be titlecased. It titlecases those characters and lowercases
   * all others.
   *
   * The titlecase break iterator can be provided to customize for arbitrary
   * styles, using rules and dictionaries beyond the standard iterators.
   * It may be more efficient to always provide an iterator to avoid
   * opening and closing one for each string.
   * If the break iterator passed in is null, the default Unicode algorithm
   * will be used to determine the titlecase positions.
   *
   * This function uses only the setText(), first() and next() methods of the
   * provided break iterator.
   *
   * @param titleIter A break iterator to find the first characters of words
   *                  that are to be titlecased.
   *                  If none is provided (0), then a standard titlecase
   *                  break iterator is opened.
   *                  Otherwise the provided iterator is set to the string's text.
   * @param locale    The locale to consider.
   * @return A reference to this.
   * @stable ICU 2.1
   */
  UnicodeString& منزل_Title(BreakIterator* titleIter, ثابت Locale& locale) U_LIFETIME_BOUND;

  /**
   * Titlecase this string, with options.
   *
   * Casing is locale-dependent and context-sensitive.
   * Titlecasing uses a break iterator to find the first characters of words
   * that are to be titlecased. It titlecases those characters and lowercases
   * all others. (This can be modified with options.)
   *
   * The titlecase break iterator can be provided to customize for arbitrary
   * styles, using rules and dictionaries beyond the standard iterators.
   * It may be more efficient to always provide an iterator to avoid
   * opening and closing one for each string.
   * If the break iterator passed in is null, the default Unicode algorithm
   * will be used to determine the titlecase positions.
   *
   * This function uses only the setText(), first() and next() methods of the
   * provided break iterator.
   *
   * @param titleIter A break iterator to find the first characters of words
   *                  that are to be titlecased.
   *                  If none is provided (0), then a standard titlecase
   *                  break iterator is opened.
   *                  Otherwise the provided iterator is set to the string's text.
   * @param locale    The locale to consider.
   * @param options   Options bit set, usually 0. See U_TITLECASE_NO_LOWERCASE,
   *                  U_TITLECASE_NO_BREAK_ADJUSTMENT, U_TITLECASE_ADJUST_TO_CASED,
   *                  U_TITLECASE_WHOLE_STRING, U_TITLECASE_SENTENCES.
   * @return A reference to this.
   * @stable ICU 3.8
   */
  UnicodeString& منزل_Title(BreakIterator* titleIter,
                         ثابت Locale& locale,
                         uint32_t options) U_LIFETIME_BOUND;

#جے_انت

  /**
   * Case-folds the characters in this string.
   *
   * Case-folding is locale-independent and not context-sensitive,
   * but there is an option for whether to include or exclude mappings for dotted I
   * and dotless i that are marked with 'T' in CaseFolding.txt.
   *
   * The result may be longer or shorter than the original.
   *
   * @param options Either U_FOLD_CASE_DEFAULT or U_FOLD_CASE_EXCLUDE_SPECIAL_I
   * @return A reference to this.
   * @stable ICU 2.0
   */
  UnicodeString& foldCase(uint32_t options = 0 /*U_FOLD_CASE_DEFAULT*/) U_LIFETIME_BOUND;

  //========================================
  // Access to the internal buffer
  //========================================

  /**
   * Get a read/write pointer to the internal buffer.
   * The buffer is guaranteed to be large enough for at least minCapacity char16_ts,
   * writable, and is still owned by the UnicodeString object.
   * Calls to getBuffer(minCapacity) must not be nested, and
   * must be matched with calls to releaseBuffer(newLength).
   * If the string buffer was read-only or shared,
   * then it will be reallocated and copied.
   *
   * An attempted nested call will return 0, and will not further modify the
   * state of the UnicodeString object.
   * It also returns 0 if the string is bogus.
   *
   * The actual capacity of the string buffer may be larger than minCapacity.
   * getCapacity() returns the actual capacity.
   * For many operations, the full capacity should be used to avoid reallocations.
   *
   * While the buffer is "open" between getBuffer(minCapacity)
   * and releaseBuffer(newLength), the following applies:
   * - The string length is set to 0.
   * - Any read API call on the UnicodeString object will behave like on a 0-length string.
   * - Any write API call on the UnicodeString object is disallowed and will have no effect.
   * - You can read from and write to the returned buffer.
   * - The previous string contents will still be in the buffer;
   *   if you want to use it, then you need to call length() before getBuffer(minCapacity).
   *   If the length() was greater than minCapacity, then any contents after minCapacity
   *   may be lost.
   *   The buffer contents is not NUL-terminated by getBuffer().
   *   If length() < getCapacity() then you can terminate it by writing a NUL
   *   at index length().
   * - You must call releaseBuffer(newLength) before and in order to
   *   return to normal UnicodeString operation.
   *
   * @param minCapacity the minimum number of char16_ts that are to be available
   *        in the buffer, starting at the returned pointer;
   *        default to the current string capacity if minCapacity==-1
   * @return a writable pointer to the internal string buffer,
   *         or nullptr if an error occurs (nested calls, out of memory)
   *
   * @see releaseBuffer
   * @see getTerminatedBuffer()
   * @stable ICU 2.0
   */
  char16_t* get_بفر(int32_t min_سمرتھا) U_LIFETIME_BOUND;

  /**
   * Release a read/write buffer on a UnicodeString object with an
   * "open" getBuffer(minCapacity).
   * This function must be called in a matched pair with getBuffer(minCapacity).
   * releaseBuffer(newLength) must be called if and only if a getBuffer(minCapacity) is "open".
   *
   * It will set the string length to newLength, at most to the current capacity.
   * If newLength==-1 then it will set the length according to the
   * first NUL in the buffer, or to the capacity if there is no NUL.
   *
   * After calling releaseBuffer(newLength) the UnicodeString is back to normal operation.
   *
   * @param newLength the new length of the UnicodeString object;
   *        defaults to the current capacity if newLength is greater than that;
   *        if newLength==-1, it defaults to u_strlen(buffer) but not more than
   *        the current capacity of the string
   *
   * @see getBuffer(int32_t minCapacity)
   * @stable ICU 2.0
   */
  فراغ release_بفر(int32_t new_لمائی=-1);

  /**
   * Get a read-only pointer to the internal buffer.
   * This can be called at any time on a valid UnicodeString.
   *
   * It returns 0 if the string is bogus, or
   * during an "open" getBuffer(minCapacity).
   *
   * It can be called as many times as desired.
   * The pointer that it returns will remain valid until the UnicodeString object is modified,
   * at which time the pointer is semantically invalidated and must not be used any more.
   *
   * The capacity of the buffer can be determined with getCapacity().
   * The part after length() may or may not be initialized and valid,
   * depending on the history of the UnicodeString object.
   *
   * The buffer contents is (probably) not NUL-terminated.
   * You can check if it is with
   * `(s.length() < s.getCapacity() && buffer[s.length()]==0)`.
   * (See getTerminatedBuffer().)
   *
   * The buffer may reside in read-only memory. Its contents must not
   * be modified.
   *
   * @return a read-only pointer to the internal string buffer,
   *         or nullptr if the string is empty or bogus
   *
   * @see getBuffer(int32_t minCapacity)
   * @see getTerminatedBuffer()
   * @stable ICU 2.0
   */
  مضمن ثابت char16_t* get_بفر() ثابت U_LIFETIME_BOUND;

  /**
   * Get a read-only pointer to the internal buffer,
   * making sure that it is NUL-terminated.
   * This can be called at any time on a valid UnicodeString.
   *
   * It returns 0 if the string is bogus, or
   * during an "open" getBuffer(minCapacity), or if the buffer cannot
   * be NUL-terminated (because memory allocation failed).
   *
   * It can be called as many times as desired.
   * The pointer that it returns will remain valid until the UnicodeString object is modified,
   * at which time the pointer is semantically invalidated and must not be used any more.
   *
   * The capacity of the buffer can be determined with getCapacity().
   * The part after length()+1 may or may not be initialized and valid,
   * depending on the history of the UnicodeString object.
   *
   * The buffer contents is guaranteed to be NUL-terminated.
   * getTerminatedBuffer() may reallocate the buffer if a terminating NUL
   * is written.
   * For this reason, this function is not const, unlike getBuffer().
   * Note that a UnicodeString may also contain NUL characters as part of its contents.
   *
   * The buffer may reside in read-only memory. Its contents must not
   * be modified.
   *
   * @return a read-only pointer to the internal string buffer,
   *         or 0 if the string is empty or bogus
   *
   * @see getBuffer(int32_t minCapacity)
   * @see getBuffer()
   * @stable ICU 2.2
   */
  ثابت char16_t* get_Terminated_بفر() U_LIFETIME_BOUND;

  /**
   * Converts to a std::u16string_view.
   *
   * @return a string view of the contents of this string
   * @stable ICU 76
   */
  مضمن عامل std::u16string_view() ثابت {
    أرجع {get_بفر(), static_cast<std::u16string_view::ناپ_type>(لمائی())};
  }

#إذا U_SIZEOF_WCHAR_T==2 || defined(U_وچ_DOXYGEN)
  /**
   * Converts to a std::wstring_view.
   *
   * Note: This should remain draft until C++ standard plans
   * about char16_t vs. wchar_t become clearer.
   *
   * @return a string view of the contents of this string
   * @stable ICU 76
   */
  مضمن عامل std::wstring_view() ثابت {
    ثابت char16_t *p = get_بفر();
#جے_تعریف_شدہ U_ALIASING_BARRIER
    U_ALIASING_BARRIER(p);
#جے_انت
    أرجع { reinterpret_cast<ثابت wchar_t *>(p), (std::wstring_view::ناپ_type)لمائی() };
  }
#جے_انت  // U_SIZEOF_WCHAR_T

  //========================================
  // Constructors
  //========================================

  /** Construct an empty UnicodeString.
   * @stable ICU 2.0
   */
  مضمن UnicodeString();

  /**
   * Construct a UnicodeString with capacity to hold `capacity` char16_ts
   * @param capacity the number of char16_ts this UnicodeString should hold
   * before a resize is necessary; if count is greater than 0 and count
   * code points c take up more space than capacity, then capacity is adjusted
   * accordingly.
   * @param c is used to initially fill the string
   * @param count specifies how many code points c are to be written in the
   *              string
   * @stable ICU 2.0
   */
  UnicodeString(int32_t سمرتھا, UChar32 c, int32_t گنتی);

  /**
   * Single char16_t (code unit) constructor.
   *
   * It is recommended to mark this constructor "explicit" by
   * `-DUNISTR_FROM_CHAR_EXPLICIT=explicit`
   * on the compiler command line or similar.
   * @param ch the character to place in the UnicodeString
   * @stable ICU 2.0
   */
  UNISTR_FROM_CHAR_EXPLICIT UnicodeString(char16_t ch);

  /**
   * Single UChar32 (code point) constructor.
   *
   * It is recommended to mark this constructor "explicit" by
   * `-DUNISTR_FROM_CHAR_EXPLICIT=explicit`
   * on the compiler command line or similar.
   * @param ch the character to place in the UnicodeString
   * @stable ICU 2.0
   */
  UNISTR_FROM_CHAR_EXPLICIT UnicodeString(UChar32 ch);

#جے_تعریف_شدہ U_HIDE_DRAFT_API
  /**
   * char16_t* constructor.
   *
   * It is recommended to mark this constructor "explicit" by
   * `-DUNISTR_FROM_STRING_EXPLICIT=explicit`
   * on the compiler command line or similar.
   *
   * Note, for string literals:
   * Since C++17 and ICU 76, you can use UTF-16 string literals with compile-time
   * length determination:
   * \code
   * UnicodeString str(u"literal");
   * if (str == u"other literal") { ... }
   * \endcode
   *
   * @param text The characters to place in the UnicodeString.  `text`
   * must be NUL (U+0000) terminated.
   * @stable ICU 2.0
   */
  UNISTR_FROM_STRING_EXPLICIT UnicodeString(ثابت char16_t *متن) :
      UnicodeString(متن, -1) {}
#جے_انت  // U_HIDE_DRAFT_API

#إذا !U_CHAR16_IS_TYPEDEF && \
    (defined(U_HIDE_DRAFT_API) || (defined(LIBCPP_نسخہ) && LIBCPP_نسخہ >= 180000))
  /**
   * uint16_t * constructor.
   * Delegates to UnicodeString(const char16_t *).
   *
   * It is recommended to mark this constructor "explicit" by
   * `-DUNISTR_FROM_STRING_EXPLICIT=explicit`
   * on the compiler command line or similar.
   *
   * Note, for string literals:
   * Since C++17 and ICU 76, you can use UTF-16 string literals with compile-time
   * length determination:
   * \code
   * UnicodeString str(u"literal");
   * if (str == u"other literal") { ... }
   * \endcode
   *
   * @param text NUL-terminated UTF-16 string
   * @stable ICU 59
   */
  UNISTR_FROM_STRING_EXPLICIT UnicodeString(ثابت uint16_t *متن) :
      UnicodeString(ConstChar16Ptr(متن), -1) {}
#جے_انت

#إذا defined(U_HIDE_DRAFT_API) && (U_SIZEOF_WCHAR_T==2 || defined(U_وچ_DOXYGEN))
  /**
   * wchar_t * constructor.
   * (Only defined if U_SIZEOF_WCHAR_T==2.)
   * Delegates to UnicodeString(const char16_t *).
   *
   * It is recommended to mark this constructor "explicit" by
   * `-DUNISTR_FROM_STRING_EXPLICIT=explicit`
   * on the compiler command line or similar.
   *
   * Note, for string literals:
   * Since C++17 and ICU 76, you can use UTF-16 string literals with compile-time
   * length determination:
   * \code
   * UnicodeString str(u"literal");
   * if (str == u"other literal") { ... }
   * \endcode
   *
   * @param text NUL-terminated UTF-16 string
   * @stable ICU 59
   */
  UNISTR_FROM_STRING_EXPLICIT UnicodeString(ثابت wchar_t *متن) :
      UnicodeString(ConstChar16Ptr(متن), -1) {}
#جے_انت

  /**
   * nullptr_t constructor.
   * Effectively the same as the default constructor, makes an empty string object.
   *
   * It is recommended to mark this constructor "explicit" by
   * `-DUNISTR_FROM_STRING_EXPLICIT=explicit`
   * on the compiler command line or similar.
   * @param text nullptr
   * @stable ICU 59
   */
  UNISTR_FROM_STRING_EXPLICIT مضمن UnicodeString(ثابت std::nullptr_t متن);

  /**
   * char16_t* constructor.
   *
   * Note, for string literals:
   * Since C++17 and ICU 76, you can use UTF-16 string literals with compile-time
   * length determination:
   * \code
   * UnicodeString str(u"literal");
   * if (str == u"other literal") { ... }
   * \endcode
   *
   * @param text The characters to place in the UnicodeString.
   * @param textLength The number of Unicode characters in `text`
   * to copy.
   * @stable ICU 2.0
   */
  UnicodeString(ثابت char16_t *متن,
        int32_t متن_لمائی);

#إذا !U_CHAR16_IS_TYPEDEF
  /**
   * uint16_t * constructor.
   * Delegates to UnicodeString(const char16_t *, int32_t).
   *
   * Note, for string literals:
   * Since C++17 and ICU 76, you can use UTF-16 string literals with compile-time
   * length determination:
   * \code
   * UnicodeString str(u"literal");
   * if (str == u"other literal") { ... }
   * \endcode
   *
   * @param text UTF-16 string
   * @param textLength string length
   * @stable ICU 59
   */
  UnicodeString(ثابت uint16_t *متن, int32_t متن_لمائی) :
      UnicodeString(ConstChar16Ptr(متن), متن_لمائی) {}
#جے_انت

#إذا U_SIZEOF_WCHAR_T==2 || defined(U_وچ_DOXYGEN)
  /**
   * wchar_t * constructor.
   * (Only defined if U_SIZEOF_WCHAR_T==2.)
   * Delegates to UnicodeString(const char16_t *, int32_t).
   *
   * Note, for string literals:
   * Since C++17 and ICU 76, you can use UTF-16 string literals with compile-time
   * length determination:
   * \code
   * UnicodeString str(u"literal");
   * if (str == u"other literal") { ... }
   * \endcode
   *
   * @param text UTF-16 string
   * @param textLength string length
   * @stable ICU 59
   */
  UnicodeString(ثابت wchar_t *متن, int32_t متن_لمائی) :
      UnicodeString(ConstChar16Ptr(متن), متن_لمائی) {}
#جے_انت

  /**
   * nullptr_t constructor.
   * Effectively the same as the default constructor, makes an empty string object.
   * @param text nullptr
   * @param textLength ignored
   * @stable ICU 59
   */
  مضمن UnicodeString(ثابت std::nullptr_t متن, int32_t متن_لمائی);

  /**
   * Constructor from `text`
   * which is, or which is implicitly convertible to,
   * a std::u16string_view or (if U_SIZEOF_WCHAR_T==2) std::wstring_view.
   * The string is bogus if the string view is too long.
   *
   * If you need a UnicodeString but need not copy the string view contents,
   * then you can call the UnicodeString::readOnlyAlias() function instead of this constructor.
   *
   * @param text UTF-16 string
   * @stable ICU 76
   */
  سانچا<typename S, typename = std::enable_if_t<Convertible_منزل_U16_String_View<S>>>
  UNISTR_FROM_STRING_EXPLICIT UnicodeString(ثابت S &متن) {
    fUnion.fFields.f_لمائی_And_Flags = kShortString;
    doAppend(اندرونی_2::منزل_U16_String_View_Nullable(متن));
  }

  /**
   * Readonly-aliasing char16_t* constructor.
   * The text will be used for the UnicodeString object, but
   * it will not be released when the UnicodeString is destroyed.
   * This has copy-on-write semantics:
   * When the string is modified, then the buffer is first copied into
   * newly allocated memory.
   * The aliased buffer is never modified.
   *
   * In an assignment to another UnicodeString, when using the copy constructor
   * or the assignment operator, the text will be copied.
   * When using fastCopyFrom(), the text will be aliased again,
   * so that both strings then alias the same readonly-text.
   *
   * Note, for string literals:
   * Since C++17 and ICU 76, you can use UTF-16 string literals with compile-time
   * length determination:
   * \code
   * UnicodeString alias = UnicodeString::readOnlyAlias(u"literal");
   * if (str == u"other literal") { ... }
   * \endcode
   *
   * @param isTerminated specifies if `text` is `NUL`-terminated.
   *                     This must be true if `textLength==-1`.
   * @param text The characters to alias for the UnicodeString.
   * @param textLength The number of Unicode characters in `text` to alias.
   *                   If -1, then this constructor will determine the length
   *                   by calling `u_strlen()`.
   * @stable ICU 2.0
   */
  UnicodeString(UBool isTerminated,
                ConstChar16Ptr متن U_LIFETIME_BOUND,
                int32_t متن_لمائی);

  /**
   * Writable-aliasing char16_t* constructor.
   * The text will be used for the UnicodeString object, but
   * it will not be released when the UnicodeString is destroyed.
   * This has write-through semantics:
   * For as long as the capacity of the buffer is sufficient, write operations
   * will directly affect the buffer. When more capacity is necessary, then
   * a new buffer will be allocated and the contents copied as with regularly
   * constructed strings.
   * In an assignment to another UnicodeString, the buffer will be copied.
   * The extract(Char16Ptr dst) function detects whether the dst pointer is the same
   * as the string buffer itself and will in this case not copy the contents.
   *
   * @param buffer The characters to alias for the UnicodeString.
   * @param buffLength The number of Unicode characters in `buffer` to alias.
   * @param buffCapacity The size of `buffer` in char16_ts.
   * @stable ICU 2.0
   */
  UnicodeString(char16_t* بفر U_LIFETIME_BOUND, int32_t buff_لمائی, int32_t buff_سمرتھا);

#إذا !U_CHAR16_IS_TYPEDEF
  /**
   * Writable-aliasing uint16_t * constructor.
   * Delegates to UnicodeString(const char16_t *, int32_t, int32_t).
   * @param buffer writable buffer of/for UTF-16 text
   * @param buffLength length of the current buffer contents
   * @param buffCapacity buffer capacity
   * @stable ICU 59
   */
  UnicodeString(uint16_t* بفر U_LIFETIME_BOUND, int32_t buff_لمائی, int32_t buff_سمرتھا) :
      UnicodeString(Char16Ptr(بفر), buff_لمائی, buff_سمرتھا) {}
#جے_انت

#إذا U_SIZEOF_WCHAR_T==2 || defined(U_وچ_DOXYGEN)
  /**
   * Writable-aliasing wchar_t * constructor.
   * (Only defined if U_SIZEOF_WCHAR_T==2.)
   * Delegates to UnicodeString(const char16_t *, int32_t, int32_t).
   * @param buffer writable buffer of/for UTF-16 text
   * @param buffLength length of the current buffer contents
   * @param buffCapacity buffer capacity
   * @stable ICU 59
   */
  UnicodeString(wchar_t* بفر U_LIFETIME_BOUND, int32_t buff_لمائی, int32_t buff_سمرتھا) :
      UnicodeString(Char16Ptr(بفر), buff_لمائی, buff_سمرتھا) {}
#جے_انت

  /**
   * Writable-aliasing nullptr_t constructor.
   * Effectively the same as the default constructor, makes an empty string object.
   * @param buffer nullptr
   * @param buffLength ignored
   * @param buffCapacity ignored
   * @stable ICU 59
   */
  مضمن UnicodeString(std::nullptr_t بفر, int32_t buff_لمائی, int32_t buff_سمرتھا);

#إذا U_CHARSET_IS_UTF8 || !UCONFIG_NO_CONVERSION

  /**
   * char* constructor.
   * Uses the default converter (and thus depends on the ICU conversion code)
   * unless U_CHARSET_IS_UTF8 is set to 1.
   *
   * For ASCII (really "invariant character") strings it is more efficient to use
   * the constructor that takes a US_INV (for its enum EInvariant).
   *
   * Note, for string literals:
   * Since C++17 and ICU 76, you can use UTF-16 string literals with compile-time
   * length determination:
   * \code
   * UnicodeString str(u"literal");
   * if (str == u"other literal") { ... }
   * \endcode
   *
   * It is recommended to mark this constructor "explicit" by
   * `-DUNISTR_FROM_STRING_EXPLICIT=explicit`
   * on the compiler command line or similar.
   * @param codepageData an array of bytes, null-terminated,
   *                     in the platform's default codepage.
   * @stable ICU 2.0
   */
  UNISTR_FROM_STRING_EXPLICIT UnicodeString(ثابت حرف *codepage_انکڑے);

  /**
   * char* constructor.
   * Uses the default converter (and thus depends on the ICU conversion code)
   * unless U_CHARSET_IS_UTF8 is set to 1.
   * @param codepageData an array of bytes in the platform's default codepage.
   * @param dataLength The number of bytes in `codepageData`.
   * @stable ICU 2.0
   */
  UnicodeString(ثابت حرف *codepage_انکڑے, int32_t انکڑے_لمائی);

#جے_انت

#إذا !UCONFIG_NO_CONVERSION

  /**
   * char* constructor.
   * @param codepageData an array of bytes, null-terminated
   * @param codepage the encoding of `codepageData`.  The special
   * value 0 for `codepage` indicates that the text is in the
   * platform's default codepage.
   *
   * If `codepage` is an empty string (`""`),
   * then a simple conversion is performed on the codepage-invariant
   * subset ("invariant characters") of the platform encoding. See utypes.h.
   * Recommendation: For invariant-character strings use the constructor
   * UnicodeString(const char *src, int32_t length, enum EInvariant inv)
   * because it avoids object code dependencies of UnicodeString on
   * the conversion code.
   *
   * @stable ICU 2.0
   */
  UnicodeString(ثابت حرف *codepage_انکڑے, ثابت حرف *codepage);

  /**
   * char* constructor.
   * @param codepageData an array of bytes.
   * @param dataLength The number of bytes in `codepageData`.
   * @param codepage the encoding of `codepageData`.  The special
   * value 0 for `codepage` indicates that the text is in the
   * platform's default codepage.
   * If `codepage` is an empty string (`""`),
   * then a simple conversion is performed on the codepage-invariant
   * subset ("invariant characters") of the platform encoding. See utypes.h.
   * Recommendation: For invariant-character strings use the constructor
   * UnicodeString(const char *src, int32_t length, enum EInvariant inv)
   * because it avoids object code dependencies of UnicodeString on
   * the conversion code.
   *
   * @stable ICU 2.0
   */
  UnicodeString(ثابت حرف *codepage_انکڑے, int32_t انکڑے_لمائی, ثابت حرف *codepage);

  /**
   * char * / UConverter constructor.
   * This constructor uses an existing UConverter object to
   * convert the codepage string to Unicode and construct a UnicodeString
   * from that.
   *
   * The converter is reset at first.
   * If the error code indicates a failure before this constructor is called,
   * or if an error occurs during conversion or construction,
   * then the string will be bogus.
   *
   * This function avoids the overhead of opening and closing a converter if
   * multiple strings are constructed.
   *
   * @param src input codepage string
   * @param srcLength length of the input string, can be -1 for NUL-terminated strings
   * @param cnv converter object (ucnv_resetToUnicode() will be called),
   *        can be nullptr for the default converter
   * @param errorCode normal ICU error code
   * @stable ICU 2.0
   */
  UnicodeString(
        ثابت حرف *src, int32_t src_لمائی,
        UConverter *cnv,
        UError_کوڈ &error_کوڈ);

#جے_انت

  /**
   * Constructs a Unicode string from an invariant-character char * string.
   * About invariant characters see utypes.h.
   * This constructor has no runtime dependency on conversion code and is
   * therefore recommended over ones taking a charset name string
   * (where the empty string "" indicates invariant-character conversion).
   *
   * Use the macro US_INV as the third, signature-distinguishing parameter.
   *
   * For example:
   * \code
   *     void fn(const char *s) {
   *       UnicodeString ustr(s, -1, US_INV);
   *       // use ustr ...
   *     }
   * \endcode
   *
   * Note, for string literals:
   * Since C++17 and ICU 76, you can use UTF-16 string literals with compile-time
   * length determination:
   * \code
   * UnicodeString str(u"literal");
   * if (str == u"other literal") { ... }
   * \endcode
   *
   * @param src String using only invariant characters.
   * @param textLength Length of src, or -1 if NUL-terminated.
   * @param inv Signature-distinguishing parameter, use US_INV.
   *
   * @see US_INV
   * @stable ICU 3.2
   */
  UnicodeString(ثابت حرف *src, int32_t متن_لمائی, تعداد EInvariant inv);


  /**
   * Copy constructor.
   *
   * Starting with ICU 2.4, the assignment operator and the copy constructor
   * allocate a new buffer and copy the buffer contents even for readonly aliases.
   * By contrast, the fastCopyFrom() function implements the old,
   * more efficient but less safe behavior
   * of making this string also a readonly alias to the same buffer.
   *
   * If the source object has an "open" buffer from getBuffer(minCapacity),
   * then the copy is an empty string.
   *
   * @param that The UnicodeString object to copy.
   * @stable ICU 2.0
   * @see fastCopyFrom
   */
  UnicodeString(ثابت UnicodeString& that);

  /**
   * Move constructor; might leave src in bogus state.
   * This string will have the same contents and state that the source string had.
   * @param src source string
   * @stable ICU 56
   */
  UnicodeString(UnicodeString &&src) noexcept;

  /**
   * 'Substring' constructor from tail of source string.
   * @param src The UnicodeString object to copy.
   * @param srcStart The offset into `src` at which to start copying.
   * @stable ICU 2.2
   */
  UnicodeString(ثابت UnicodeString& src, int32_t src_شروع);

  /**
   * 'Substring' constructor from subrange of source string.
   * @param src The UnicodeString object to copy.
   * @param srcStart The offset into `src` at which to start copying.
   * @param srcLength The number of characters from `src` to copy.
   * @stable ICU 2.2
   */
  UnicodeString(ثابت UnicodeString& src, int32_t src_شروع, int32_t src_لمائی);

  /**
   * Clone this object, an instance of a subclass of Replaceable.
   * Clones can be used concurrently in multiple threads.
   * If a subclass does not implement clone(), or if an error occurs,
   * then nullptr is returned.
   * The caller must delete the clone.
   *
   * @return a clone of this object
   *
   * @see Replaceable::clone
   * @see getDynamicClassID
   * @stable ICU 2.6
   */
  مجازی UnicodeString *clone() ثابت override;

  /** Destructor.
   * @stable ICU 2.0
   */
  مجازی ~UnicodeString();

  /**
   * Readonly-aliasing factory method.
   * Aliases the same buffer as the input `text`
   * which is, or which is implicitly convertible to,
   * a std::u16string_view or (if U_SIZEOF_WCHAR_T==2) std::wstring_view.
   * The string is bogus if the string view is too long.
   *
   * The text will be used for the UnicodeString object, but
   * it will not be released when the UnicodeString is destroyed.
   * This has copy-on-write semantics:
   * When the string is modified, then the buffer is first copied into
   * newly allocated memory.
   * The aliased buffer is never modified.
   *
   * In an assignment to another UnicodeString, when using the copy constructor
   * or the assignment operator, the text will be copied.
   * When using fastCopyFrom(), the text will be aliased again,
   * so that both strings then alias the same readonly-text.
   *
   * @param text The string view to alias for the UnicodeString.
   * @stable ICU 76
   */
  سانچا <typename S, typename = std::enable_if_t<Convertible_منزل_U16_String_View<S>>>
  ساكن مضمن UnicodeString readOnlyAlias(ثابت S& متن U_LIFETIME_BOUND) {
    أرجع readOnlyAliasFromU16StringView(اندرونی_2::منزل_U16_String_View(متن));
  }

  /**
   * Readonly-aliasing factory method.
   * Aliases the same buffer as the input `text`.
   *
   * The text will be used for the UnicodeString object, but
   * it will not be released when the UnicodeString is destroyed.
   * This has copy-on-write semantics:
   * When the string is modified, then the buffer is first copied into
   * newly allocated memory.
   * The aliased buffer is never modified.
   *
   * In an assignment to another UnicodeString, when using the copy constructor
   * or the assignment operator, the text will be copied.
   * When using fastCopyFrom(), the text will be aliased again,
   * so that both strings then alias the same readonly-text.
   *
   * @param text The UnicodeString to alias.
   * @stable ICU 76
   */
  ساكن مضمن UnicodeString readOnlyAlias(ثابت UnicodeString& متن U_LIFETIME_BOUND) {
    أرجع readOnlyAliasFromUnicodeString(متن);
  }

  /**
   * Create a UnicodeString from a UTF-8 string.
   * Illegal input is replaced with U+FFFD. Otherwise, errors result in a bogus string.
   * Calls u_strFromUTF8WithSub().
   *
   * @param utf8 UTF-8 input string.
   *             Note that a StringPiece can be implicitly constructed
   *             from a std::string or a NUL-terminated const char * string.
   * @return A UnicodeString with equivalent UTF-16 contents.
   * @see toUTF8
   * @see toUTF8String
   * @stable ICU 4.2
   */
  ساكن UnicodeString fromUTF8(StringPiece utf8);

  /**
   * Create a UnicodeString from a UTF-32 string.
   * Illegal input is replaced with U+FFFD. Otherwise, errors result in a bogus string.
   * Calls u_strFromUTF32WithSub().
   *
   * @param utf32 UTF-32 input string. Must not be nullptr.
   * @param length Length of the input string, or -1 if NUL-terminated.
   * @return A UnicodeString with equivalent UTF-16 contents.
   * @see toUTF32
   * @stable ICU 4.2
   */
  ساكن UnicodeString fromUTF32(ثابت UChar32 *utf32, int32_t لمائی);

  /* Miscellaneous operations */

  /**
   * Unescape a string of characters and return a string containing
   * the result.  The following escape sequences are recognized:
   *
   * \\uhhhh       4 hex digits; h in [0-9A-Fa-f]
   * \\Uhhhhhhhh   8 hex digits
   * \\xhh         1-2 hex digits
   * \\ooo         1-3 octal digits; o in [0-7]
   * \\cX          control-X; X is masked with 0x1F
   *
   * as well as the standard ANSI C escapes:
   *
   * \\a => U+0007, \\b => U+0008, \\t => U+0009, \\n => U+000A,
   * \\v => U+000B, \\f => U+000C, \\r => U+000D, \\e => U+001B,
   * \\" => U+0022, \\' => U+0027, \\? => U+003F, \\\\ => U+005C
   *
   * Anything else following a backslash is generically escaped.  For
   * example, "[a\\-z]" returns "[a-z]".
   *
   * If an escape sequence is ill-formed, this method returns an empty
   * string.  An example of an ill-formed sequence is "\\u" followed by
   * fewer than 4 hex digits.
   *
   * This function is similar to u_unescape() but not identical to it.
   * The latter takes a source char*, so it does escape recognition
   * and also invariant conversion.
   *
   * @return a string with backslash escapes interpreted, or an
   * empty string on error.
   * @see UnicodeString#unescapeAt()
   * @see u_unescape()
   * @see u_unescapeAt()
   * @stable ICU 2.0
   */
  UnicodeString unescape() ثابت;

  /**
   * Unescape a single escape sequence and return the represented
   * character.  See unescape() for a listing of the recognized escape
   * sequences.  The character at offset-1 is assumed (without
   * checking) to be a backslash.  If the escape sequence is
   * ill-formed, or the offset is out of range, U_SENTINEL=-1 is
   * returned.
   *
   * @param offset an input output parameter.  On input, it is the
   * offset into this string where the escape sequence is located,
   * after the initial backslash.  On output, it is advanced after the
   * last character parsed.  On error, it is not advanced at all.
   * @return the character represented by the escape sequence at
   * offset, or U_SENTINEL=-1 on error.
   * @see UnicodeString#unescape()
   * @see u_unescape()
   * @see u_unescapeAt()
   * @stable ICU 2.0
   */
  UChar32 unescapeAt(int32_t &وتھ) ثابت;

  /**
   * ICU "poor man's RTTI", returns a UClassID for this class.
   *
   * @stable ICU 2.2
   */
  ساكن UClassID U_EXPORT2 getStaticClassID();

  /**
   * ICU "poor man's RTTI", returns a UClassID for the actual class.
   *
   * @stable ICU 2.2
   */
  مجازی UClassID getDynamicClassID() ثابت override;

  //========================================
  // Implementation methods
  //========================================

محمي:
  /**
   * Implement Replaceable::getLength() (see jitterbug 1027).
   * @stable ICU 2.4
   */
  مجازی int32_t get_لمائی() ثابت override;

  /**
   * The change in Replaceable to use virtual getCharAt() allows
   * UnicodeString::charAt() to be inline again (see jitterbug 709).
   * @stable ICU 2.4
   */
  مجازی char16_t getCharAt(int32_t وتھ) ثابت override;

  /**
   * The change in Replaceable to use virtual getChar32At() allows
   * UnicodeString::char32At() to be inline again (see jitterbug 709).
   * @stable ICU 2.4
   */
  مجازی UChar32 getChar32At(int32_t وتھ) ثابت override;

خاص:
  ساكن UnicodeString readOnlyAliasFromU16StringView(std::u16string_view متن U_LIFETIME_BOUND);
  ساكن UnicodeString readOnlyAliasFromUnicodeString(ثابت UnicodeString& متن U_LIFETIME_BOUND);

  // For char* constructors. Could be made public.
  UnicodeString& سیٹ_منزل_UTF8(StringPiece utf8) U_LIFETIME_BOUND;
  // For extract(char*).
  // We could make a toUTF8(target, capacity, errorCode) public but not
  // this version: New API will be cleaner if we make callers create substrings
  // rather than having start+length on every method,
  // and it should take a UErrorCode&.
  int32_t
  منزل_UTF8(int32_t شروع, int32_t len,
         حرف *نشانہ, int32_t سمرتھا) ثابت;

  /**
   * Internal string contents comparison, called by operator==.
   * Requires: this & text not bogus and have same lengths.
   */
  مضمن UBool doEquals(ثابت UnicodeString &متن, int32_t len) ثابت {
    أرجع doEquals(متن.get_Array_شروع(), len);
  }
  UBool doEquals(ثابت char16_t *متن, int32_t len) ثابت;

  مضمن UBool
  doEqualsSubstring(int32_t شروع,
           int32_t لمائی,
           ثابت UnicodeString& src_متن,
           int32_t src_شروع,
           int32_t src_لمائی) ثابت;

  UBool doEqualsSubstring(int32_t شروع,
           int32_t لمائی,
           ثابت char16_t *srcChars,
           int32_t src_شروع,
           int32_t src_لمائی) ثابت;

  مضمن int8_t
  doCompare(int32_t شروع,
           int32_t لمائی,
           ثابت UnicodeString& src_متن,
           int32_t src_شروع,
           int32_t src_لمائی) ثابت;

  int8_t doCompare(int32_t شروع,
           int32_t لمائی,
           ثابت char16_t *srcChars,
           int32_t src_شروع,
           int32_t src_لمائی) ثابت;

  مضمن int8_t
  do_Compare_کوڈ_نقطہ_ترتیب(int32_t شروع,
                          int32_t لمائی,
                          ثابت UnicodeString& src_متن,
                          int32_t src_شروع,
                          int32_t src_لمائی) ثابت;

  int8_t do_Compare_کوڈ_نقطہ_ترتیب(int32_t شروع,
                                 int32_t لمائی,
                                 ثابت char16_t *srcChars,
                                 int32_t src_شروع,
                                 int32_t src_لمائی) ثابت;

  مضمن int8_t
  doCaseCompare(int32_t شروع,
                int32_t لمائی,
                ثابت UnicodeString &src_متن,
                int32_t src_شروع,
                int32_t src_لمائی,
                uint32_t options) ثابت;

  int8_t
  doCaseCompare(int32_t شروع,
                int32_t لمائی,
                ثابت char16_t *srcChars,
                int32_t src_شروع,
                int32_t src_لمائی,
                uint32_t options) ثابت;

  int32_t do_اشاریہ_دا(char16_t c,
            int32_t شروع,
            int32_t لمائی) ثابت;

  int32_t do_اشاریہ_دا(UChar32 c,
                        int32_t شروع,
                        int32_t لمائی) ثابت;

  int32_t do_آخری_اشاریہ_دا(char16_t c,
                int32_t شروع,
                int32_t لمائی) ثابت;

  int32_t do_آخری_اشاریہ_دا(UChar32 c,
                            int32_t شروع,
                            int32_t لمائی) ثابت;

  فراغ do_کڈھو(int32_t شروع,
         int32_t لمائی,
         char16_t *dst,
         int32_t dst_شروع) ثابت;

  مضمن فراغ do_کڈھو(int32_t شروع,
         int32_t لمائی,
         UnicodeString& نشانہ) ثابت;

  مضمن char16_t doCharAt(int32_t وتھ)  ثابت;

  UnicodeString& doReplace(int32_t شروع,
                           int32_t لمائی,
                           ثابت UnicodeString& src_متن,
                           int32_t src_شروع,
                           int32_t src_لمائی) U_LIFETIME_BOUND;

  UnicodeString& doReplace(int32_t شروع,
                           int32_t لمائی,
                           ثابت char16_t* srcChars,
                           int32_t src_شروع,
                           int32_t src_لمائی) U_LIFETIME_BOUND;
  UnicodeString& doReplace(int32_t شروع, int32_t لمائی, std::u16string_view src) U_LIFETIME_BOUND;

  UnicodeString& doAppend(ثابت UnicodeString& src,
                          int32_t src_شروع,
                          int32_t src_لمائی) U_LIFETIME_BOUND;
  UnicodeString& doAppend(ثابت char16_t* srcChars,
                          int32_t src_شروع,
                          int32_t src_لمائی) U_LIFETIME_BOUND;
  UnicodeString& doAppend(std::u16string_view src) U_LIFETIME_BOUND;

  UnicodeString& do_الٹ(int32_t شروع, int32_t لمائی) U_LIFETIME_BOUND;

  // calculate hash code
  int32_t do_Hash_کوڈ() ثابت;

  // get pointer to start of array
  // these do not check for kOpenGetBuffer, unlike the public getBuffer() function
  مضمن char16_t* get_Array_شروع() U_LIFETIME_BOUND;
  مضمن ثابت char16_t* get_Array_شروع() ثابت U_LIFETIME_BOUND;

  مضمن UBool has_Short_لمائی() ثابت;
  مضمن int32_t get_Short_لمائی() ثابت;

  // A UnicodeString object (not necessarily its current buffer)
  // is writable unless it isBogus() or it has an "open" getBuffer(minCapacity).
  مضمن UBool isWritable() ثابت;

  // Is the current buffer writable?
  مضمن UBool is_بفر_Writable() ثابت;

  // None of the following does releaseArray().
  مضمن فراغ سیٹ_Zero_لمائی();
  مضمن فراغ سیٹ_Short_لمائی(int32_t len);
  مضمن فراغ سیٹ_لمائی(int32_t len);
  مضمن فراغ سیٹ_منزل_خالی_تھاواں();
  مضمن فراغ سیٹ_Array(char16_t *array, int32_t len, int32_t سمرتھا); // sets length but not flags

  // allocate the array; result may be the stack buffer
  // sets refCount to 1 if appropriate
  // sets fArray, fCapacity, and flags
  // sets length to 0
  // returns boolean for success or failure
  UBool allocate(int32_t سمرتھا);

  // release the array if owned
  فراغ releaseArray();

  // turn a bogus string into an empty one
  فراغ unBogus();

  // implements assignment operator, copy constructor, and fastCopyFrom()
  UnicodeString& copyFrom(ثابت UnicodeString& src, UBool fastCopy = false) U_LIFETIME_BOUND;

  // Copies just the fields without memory management.
  فراغ copyFieldsFrom(UnicodeString &src, UBool سیٹ_Src_منزل_Bogus) noexcept;

  // Pin start and limit to acceptable values.
  مضمن فراغ pin_اشاریہ(int32_t& شروع) ثابت;
  مضمن فراغ pinIndices(int32_t& شروع,
                         int32_t& لمائی) ثابت;

#إذا !UCONFIG_NO_CONVERSION

  /* Internal extract() using UConverter. */
  int32_t do_کڈھو(int32_t شروع, int32_t لمائی,
                    حرف *dest, int32_t dest_سمرتھا,
                    UConverter *cnv,
                    UError_کوڈ &error_کوڈ) ثابت;

  /*
   * Real constructor for converting from codepage data.
   * It assumes that it is called with !fRefCounted.
   *
   * If `codepage==0`, then the default converter
   * is used for the platform encoding.
   * If `codepage` is an empty string (`""`),
   * then a simple conversion is performed on the codepage-invariant
   * subset ("invariant characters") of the platform encoding. See utypes.h.
   */
  فراغ doCodepageCreate(ثابت حرف *codepage_انکڑے,
                        int32_t انکڑے_لمائی,
                        ثابت حرف *codepage);

  /*
   * Worker function for creating a UnicodeString from
   * a codepage string using a UConverter.
   */
  فراغ
  doCodepageCreate(ثابت حرف *codepage_انکڑے,
                   int32_t انکڑے_لمائی,
                   UConverter *converter,
                   UError_کوڈ &حالت);

#جے_انت

  /*
   * This function is called when write access to the array
   * is necessary.
   *
   * We need to make a copy of the array if
   * the buffer is read-only, or
   * the buffer is refCounted (shared), and refCount>1, or
   * the buffer is too small.
   *
   * Return false if memory could not be allocated.
   */
  UBool cloneArrayIfNeeded(int32_t new_سمرتھا = -1,
                           int32_t grow_سمرتھا = -1,
                           UBool doCopyArray = true,
                           int32_t** p_بفر_منزل_Delete = nullptr,
                           UBool forceClone = false);

  /**
   * Common function for UnicodeString case mappings.
   * The stringCaseMapper has the same type UStringCaseMapper
   * as in ustr_imp.h for ustrcase_map().
   */
  UnicodeString& caseMap(int32_t caseLocale,
                         uint32_t options,
#إذا !UCONFIG_NO_BREAK_ITERATION
                         BreakIterator* iter,
#جے_انت
                         UStringCaseMapper* stringCaseMapper) U_LIFETIME_BOUND;

  // ref counting
  فراغ جوڑو_Ref();
  int32_t removeRef();
  int32_t ref_گنتی() ثابت;

  // constants
  تعداد {
    /**
     * Size of stack buffer for short strings.
     * Must be at least U16_MAX_LENGTH for the single-code point constructor to work.
     * @see UNISTR_OBJECT_SIZE
     */
    US_STACKBUF_ناپ = static_cast<int32_t>(UNISTR_OBJECT_ناپ - حجم(فراغ*) - 2) / U_SIZEOF_UCHAR,
    kInvalidUChar=0xffff, // U+FFFF returned by charAt(invalid index)
    k_Invalid_Hash_کوڈ=0, // invalid hash code
    k_خالی_تھاواں_Hash_کوڈ=1, // hash code for empty string

    // bit flag values for fLengthAndFlags
    kIsBogus=1,         // this string is bogus, i.e., not valid or nullptr
    k_Using_ڈھیر_بفر=2,// using fUnion.fStackFields instead of fUnion.fFields
    kRefCounted=4,      // there is a refCount field before the characters in fArray
    k_بفر_Is_Readonly=8,// do not write to this buffer
    k_Open_Get_بفر=16,  // getBuffer(minCapacity) was called (is "open"),
                        // and releaseBuffer(newLength) must be called
    kAllStorageFlags=0x1f,

    k_لمائی_کھسکاؤ=5,     // remaining 11 bits for non-negative short length, or negative if long
    kLength1=1<<k_لمائی_کھسکاؤ,
    k_Max_Short_لمائی=0x3ff,  // max non-negative short length (leaves top bit 0)
    k_لمائی_Is_Large=0xffe0,  // short length < 0, real length is in fUnion.fFields.fLength

    // combined values for convenience
    kShortString=k_Using_ڈھیر_بفر,
    kLongString=kRefCounted,
    kReadonlyAlias=k_بفر_Is_Readonly,
    kWritableAlias=0
  };

  یار class UnicodeStringAppendable;

  اتحاد ڈھیر_بفر_Or_Fields;        // forward declaration necessary before friend declaration
  یار اتحاد ڈھیر_بفر_Or_Fields; // make US_STACKBUF_SIZE visible inside fUnion

  /*
   * The following are all the class fields that are stored
   * in each UnicodeString object.
   * Note that UnicodeString has virtual functions,
   * therefore there is an implicit vtable pointer
   * as the first real field.
   * The fields should be aligned such that no padding is necessary.
   * On 32-bit machines, the size should be 32 bytes,
   * on 64-bit machines (8-byte pointers), it should be 40 bytes.
   *
   * We use a hack to achieve this.
   *
   * With at least some compilers, each of the following is forced to
   * a multiple of sizeof(pointer) [the largest field base unit here is a data pointer],
   * rounded up with additional padding if the fields do not already fit that requirement:
   * - sizeof(class UnicodeString)
   * - offsetof(UnicodeString, fUnion)
   * - sizeof(fUnion)
   * - sizeof(fStackFields)
   *
   * We optimize for the longest possible internal buffer for short strings.
   * fUnion.fStackFields begins with 2 bytes for storage flags
   * and the length of relatively short strings,
   * followed by the buffer for short string contents.
   * There is no padding inside fStackFields.
   *
   * Heap-allocated and aliased strings use fUnion.fFields.
   * Both fStackFields and fFields must begin with the same fields for flags and short length,
   * that is, those must have the same memory offsets inside the object,
   * because the flags must be inspected in order to decide which half of fUnion is being used.
   * We assume that the compiler does not reorder the fields.
   *
   * (Padding at the end of fFields is ok:
   * As long as it is no larger than fStackFields, it is not wasted space.)
   *
   * For some of the history of the UnicodeString class fields layout, see
   * - ICU ticket #11551 "longer UnicodeString contents in stack buffer"
   * - ICU ticket #11336 "UnicodeString: recombine stack buffer arrays"
   * - ICU ticket #8322 "why is sizeof(UnicodeString)==48?"
   */
  // (implicit) *vtable;
  اتحاد ڈھیر_بفر_Or_Fields {
    // fStackFields is used iff (fLengthAndFlags&kUsingStackBuffer) else fFields is used.
    // Each struct of the union must begin with fLengthAndFlags.
    بنية {
      int16_t f_لمائی_And_Flags;          // bit fields: see constants above
      char16_t f_بفر[US_STACKBUF_ناپ];  // buffer for short strings
    } f_ڈھیر_Fields;
    بنية {
      int16_t f_لمائی_And_Flags;          // bit fields: see constants above
      int32_t f_لمائی;    // number of characters in fArray if >127; else undefined
      int32_t f_سمرتھا;  // capacity of fArray (in char16_ts)
      // array pointer last to minimize padding for machines with P128 data model
      // or pointer sizes that are not a power of 2
      char16_t   *fArray;    // the Unicode data
    } fFields;
  } fUnion;
};

/**
 * Creates a new UnicodeString from the concatenation of two others.
 *
 * @param s1 The first string to be copied to the new one.
 * @param s2 The second string to be copied to the new one, after s1.
 * @return UnicodeString(s1).append(s2)
 * @stable ICU 2.8
 */
U_COMMON_API UnicodeString U_EXPORT2
عامل+ (ثابت UnicodeString &s1, ثابت UnicodeString &s2);

/**
 * Creates a new UnicodeString from the concatenation of a UnicodeString and `s2`
 * which is, or which is implicitly convertible to,
 * a std::u16string_view or (if U_SIZEOF_WCHAR_T==2) std::wstring_view.
 *
 * @param s1 The string to be copied to the new one.
 * @param s2 The string view to be copied to the new string, after s1.
 * @return UnicodeString(s1).append(s2)
 * @stable ICU 76
 */
سانچا<
    typename US, typename S,
    typename = std::enable_if_t<Convertible_منزل_U16_String_View<S> && std::is_same_v<US, UnicodeString>>>
مضمن UnicodeString عامل+(ثابت US &s1, ثابت S &s2) {
  أرجع unistr_اندرونی_Concat(s1, اندرونی_2::منزل_U16_String_View(s2));
}

#جے_بنا_تعریف U_FORCE_HIDE_اندرونی_API
/** @internal */
U_COMMON_API UnicodeString U_EXPORT2
unistr_اندرونی_Concat(ثابت UnicodeString &s1, std::u16string_view s2);
#جے_انت

//========================================
// Inline members
//========================================

//========================================
// Privates
//========================================

مضمن فراغ
UnicodeString::pin_اشاریہ(int32_t& شروع) ثابت
{
  // pin index
  إذا(شروع < 0) {
    شروع = 0;
  } وإلا إذا(شروع > لمائی()) {
    شروع = لمائی();
  }
}

مضمن فراغ
UnicodeString::pinIndices(int32_t& شروع,
                          int32_t& لمائی_4) ثابت
{
  // pin indices
  int32_t len = لمائی();
  إذا(شروع < 0) {
    شروع = 0;
  } وإلا إذا(شروع > len) {
    شروع = len;
  }
  إذا(لمائی_4 < 0) {
    لمائی_4 = 0;
  } وإلا إذا(لمائی_4 > (len - شروع)) {
    لمائی_4 = (len - شروع);
  }
}

مضمن char16_t*
UnicodeString::get_Array_شروع() U_LIFETIME_BOUND {
  أرجع (fUnion.fFields.f_لمائی_And_Flags&k_Using_ڈھیر_بفر) ?
    fUnion.f_ڈھیر_Fields.f_بفر : fUnion.fFields.fArray;
}

مضمن ثابت char16_t*
UnicodeString::get_Array_شروع() ثابت U_LIFETIME_BOUND {
  أرجع (fUnion.fFields.f_لمائی_And_Flags&k_Using_ڈھیر_بفر) ?
    fUnion.f_ڈھیر_Fields.f_بفر : fUnion.fFields.fArray;
}

//========================================
// Default constructor
//========================================

مضمن
UnicodeString::UnicodeString() {
  fUnion.f_ڈھیر_Fields.f_لمائی_And_Flags=kShortString;
}

مضمن UnicodeString::UnicodeString(ثابت std::nullptr_t /*متن*/) {
  fUnion.f_ڈھیر_Fields.f_لمائی_And_Flags=kShortString;
}

مضمن UnicodeString::UnicodeString(ثابت std::nullptr_t /*متن*/, int32_t /*لمائی*/) {
  fUnion.f_ڈھیر_Fields.f_لمائی_And_Flags=kShortString;
}

مضمن UnicodeString::UnicodeString(std::nullptr_t /*بفر*/, int32_t /*buffLength*/, int32_t /*buffCapacity*/) {
  fUnion.f_ڈھیر_Fields.f_لمائی_And_Flags=kShortString;
}

//========================================
// Read-only implementation methods
//========================================
مضمن UBool
UnicodeString::has_Short_لمائی() ثابت {
  أرجع fUnion.fFields.f_لمائی_And_Flags>=0;
}

مضمن int32_t
UnicodeString::get_Short_لمائی() ثابت {
  // fLengthAndFlags must be non-negative -> short length >= 0
  // and arithmetic or logical shift does not matter.
  أرجع fUnion.fFields.f_لمائی_And_Flags>>k_لمائی_کھسکاؤ;
}

مضمن int32_t
UnicodeString::لمائی() ثابت {
  أرجع has_Short_لمائی() ? get_Short_لمائی() : fUnion.fFields.f_لمائی;
}

مضمن int32_t
UnicodeString::get_سمرتھا() ثابت {
  أرجع (fUnion.fFields.f_لمائی_And_Flags&k_Using_ڈھیر_بفر) ?
    US_STACKBUF_ناپ : fUnion.fFields.f_سمرتھا;
}

مضمن int32_t
UnicodeString::hash_کوڈ() ثابت
{ أرجع do_Hash_کوڈ(); }

مضمن UBool
UnicodeString::isBogus() ثابت
{ أرجع fUnion.fFields.f_لمائی_And_Flags & kIsBogus; }

مضمن UBool
UnicodeString::isWritable() ثابت
{ أرجع !(fUnion.fFields.f_لمائی_And_Flags & (k_Open_Get_بفر | kIsBogus)); }

مضمن UBool
UnicodeString::is_بفر_Writable() ثابت
{
  أرجع
      !(fUnion.fFields.f_لمائی_And_Flags&(k_Open_Get_بفر|kIsBogus|k_بفر_Is_Readonly)) &&
      (!(fUnion.fFields.f_لمائی_And_Flags&kRefCounted) || ref_گنتی()==1);
}

مضمن ثابت char16_t *
UnicodeString::get_بفر() ثابت U_LIFETIME_BOUND {
  إذا(fUnion.fFields.f_لمائی_And_Flags&(kIsBogus|k_Open_Get_بفر)) {
    أرجع nullptr;
  } وإلا إذا(fUnion.fFields.f_لمائی_And_Flags&k_Using_ڈھیر_بفر) {
    أرجع fUnion.f_ڈھیر_Fields.f_بفر;
  } وإلا {
    أرجع fUnion.fFields.fArray;
  }
}

//========================================
// Read-only alias methods
//========================================
مضمن int8_t
UnicodeString::doCompare(int32_t شروع,
              int32_t this_لمائی,
              ثابت UnicodeString& src_متن,
              int32_t src_شروع,
              int32_t src_لمائی) ثابت
{
  إذا(src_متن.isBogus()) {
    أرجع static_cast<int8_t>(!isBogus()); // 0 if both are bogus, 1 otherwise
  } وإلا {
    src_متن.pinIndices(src_شروع, src_لمائی);
    أرجع doCompare(شروع, this_لمائی, src_متن.get_Array_شروع(), src_شروع, src_لمائی);
  }
}

مضمن UBool
UnicodeString::doEqualsSubstring(int32_t شروع,
              int32_t this_لمائی,
              ثابت UnicodeString& src_متن,
              int32_t src_شروع,
              int32_t src_لمائی) ثابت
{
  إذا(src_متن.isBogus()) {
    أرجع isBogus();
  } وإلا {
    src_متن.pinIndices(src_شروع, src_لمائی);
    أرجع !isBogus() && doEqualsSubstring(شروع, this_لمائی, src_متن.get_Array_شروع(), src_شروع, src_لمائی);
  }
}

مضمن bool
UnicodeString::عامل== (ثابت UnicodeString& متن) ثابت
{
  إذا(isBogus()) {
    أرجع متن.isBogus();
  } وإلا {
    int32_t len = لمائی(), متن_لمائی = متن.لمائی();
    أرجع !متن.isBogus() && len == متن_لمائی && doEquals(متن, len);
  }
}

مضمن bool
UnicodeString::عامل!= (ثابت UnicodeString& متن) ثابت
{ أرجع (! عامل==(متن)); }

مضمن UBool
UnicodeString::عامل> (ثابت UnicodeString& متن) ثابت
{ أرجع doCompare(0, لمائی(), متن, 0, متن.لمائی()) == 1; }

مضمن UBool
UnicodeString::عامل< (ثابت UnicodeString& متن) ثابت
{ أرجع doCompare(0, لمائی(), متن, 0, متن.لمائی()) == -1; }

مضمن UBool
UnicodeString::عامل>= (ثابت UnicodeString& متن) ثابت
{ أرجع doCompare(0, لمائی(), متن, 0, متن.لمائی()) != -1; }

مضمن UBool
UnicodeString::عامل<= (ثابت UnicodeString& متن) ثابت
{ أرجع doCompare(0, لمائی(), متن, 0, متن.لمائی()) != 1; }

مضمن int8_t
UnicodeString::compare(ثابت UnicodeString& متن) ثابت
{ أرجع doCompare(0, لمائی(), متن, 0, متن.لمائی()); }

مضمن int8_t
UnicodeString::compare(int32_t شروع,
               int32_t لمائی_4,
               ثابت UnicodeString& src_متن) ثابت
{ أرجع doCompare(شروع, لمائی_4, src_متن, 0, src_متن.لمائی()); }

مضمن int8_t
UnicodeString::compare(ConstChar16Ptr srcChars,
               int32_t src_لمائی) ثابت
{ أرجع doCompare(0, لمائی(), srcChars, 0, src_لمائی); }

مضمن int8_t
UnicodeString::compare(int32_t شروع,
               int32_t لمائی_4,
               ثابت UnicodeString& src_متن,
               int32_t src_شروع,
               int32_t src_لمائی) ثابت
{ أرجع doCompare(شروع, لمائی_4, src_متن, src_شروع, src_لمائی); }

مضمن int8_t
UnicodeString::compare(int32_t شروع,
               int32_t لمائی_4,
               ثابت char16_t *srcChars) ثابت
{ أرجع doCompare(شروع, لمائی_4, srcChars, 0, لمائی_4); }

مضمن int8_t
UnicodeString::compare(int32_t شروع,
               int32_t لمائی_4,
               ثابت char16_t *srcChars,
               int32_t src_شروع,
               int32_t src_لمائی) ثابت
{ أرجع doCompare(شروع, لمائی_4, srcChars, src_شروع, src_لمائی); }

مضمن int8_t
UnicodeString::compareBetween(int32_t شروع,
                  int32_t حد_بندی,
                  ثابت UnicodeString& src_متن,
                  int32_t src_شروع,
                  int32_t src_حد_بندی) ثابت
{ أرجع doCompare(شروع, حد_بندی - شروع,
           src_متن, src_شروع, src_حد_بندی - src_شروع); }

مضمن int8_t
UnicodeString::do_Compare_کوڈ_نقطہ_ترتیب(int32_t شروع,
                                       int32_t this_لمائی,
                                       ثابت UnicodeString& src_متن,
                                       int32_t src_شروع,
                                       int32_t src_لمائی) ثابت
{
  إذا(src_متن.isBogus()) {
    أرجع static_cast<int8_t>(!isBogus()); // 0 if both are bogus, 1 otherwise
  } وإلا {
    src_متن.pinIndices(src_شروع, src_لمائی);
    أرجع do_Compare_کوڈ_نقطہ_ترتیب(شروع, this_لمائی, src_متن.get_Array_شروع(), src_شروع, src_لمائی);
  }
}

مضمن int8_t
UnicodeString::compare_کوڈ_نقطہ_ترتیب(ثابت UnicodeString& متن) ثابت
{ أرجع do_Compare_کوڈ_نقطہ_ترتیب(0, لمائی(), متن, 0, متن.لمائی()); }

مضمن int8_t
UnicodeString::compare_کوڈ_نقطہ_ترتیب(int32_t شروع,
                                     int32_t لمائی_4,
                                     ثابت UnicodeString& src_متن) ثابت
{ أرجع do_Compare_کوڈ_نقطہ_ترتیب(شروع, لمائی_4, src_متن, 0, src_متن.لمائی()); }

مضمن int8_t
UnicodeString::compare_کوڈ_نقطہ_ترتیب(ConstChar16Ptr srcChars,
                                     int32_t src_لمائی) ثابت
{ أرجع do_Compare_کوڈ_نقطہ_ترتیب(0, لمائی(), srcChars, 0, src_لمائی); }

مضمن int8_t
UnicodeString::compare_کوڈ_نقطہ_ترتیب(int32_t شروع,
                                     int32_t لمائی_4,
                                     ثابت UnicodeString& src_متن,
                                     int32_t src_شروع,
                                     int32_t src_لمائی) ثابت
{ أرجع do_Compare_کوڈ_نقطہ_ترتیب(شروع, لمائی_4, src_متن, src_شروع, src_لمائی); }

مضمن int8_t
UnicodeString::compare_کوڈ_نقطہ_ترتیب(int32_t شروع,
                                     int32_t لمائی_4,
                                     ثابت char16_t *srcChars) ثابت
{ أرجع do_Compare_کوڈ_نقطہ_ترتیب(شروع, لمائی_4, srcChars, 0, لمائی_4); }

مضمن int8_t
UnicodeString::compare_کوڈ_نقطہ_ترتیب(int32_t شروع,
                                     int32_t لمائی_4,
                                     ثابت char16_t *srcChars,
                                     int32_t src_شروع,
                                     int32_t src_لمائی) ثابت
{ أرجع do_Compare_کوڈ_نقطہ_ترتیب(شروع, لمائی_4, srcChars, src_شروع, src_لمائی); }

مضمن int8_t
UnicodeString::compare_کوڈ_نقطہ_ترتیب_Between(int32_t شروع,
                                            int32_t حد_بندی,
                                            ثابت UnicodeString& src_متن,
                                            int32_t src_شروع,
                                            int32_t src_حد_بندی) ثابت
{ أرجع do_Compare_کوڈ_نقطہ_ترتیب(شروع, حد_بندی - شروع,
           src_متن, src_شروع, src_حد_بندی - src_شروع); }

مضمن int8_t
UnicodeString::doCaseCompare(int32_t شروع,
                             int32_t this_لمائی,
                             ثابت UnicodeString &src_متن,
                             int32_t src_شروع,
                             int32_t src_لمائی,
                             uint32_t options) ثابت
{
  إذا(src_متن.isBogus()) {
    أرجع static_cast<int8_t>(!isBogus()); // 0 if both are bogus, 1 otherwise
  } وإلا {
    src_متن.pinIndices(src_شروع, src_لمائی);
    أرجع doCaseCompare(شروع, this_لمائی, src_متن.get_Array_شروع(), src_شروع, src_لمائی, options);
  }
}

مضمن int8_t
UnicodeString::caseCompare(ثابت UnicodeString &متن, uint32_t options) ثابت {
  أرجع doCaseCompare(0, لمائی(), متن, 0, متن.لمائی(), options);
}

مضمن int8_t
UnicodeString::caseCompare(int32_t شروع,
                           int32_t لمائی_4,
                           ثابت UnicodeString &src_متن,
                           uint32_t options) ثابت {
  أرجع doCaseCompare(شروع, لمائی_4, src_متن, 0, src_متن.لمائی(), options);
}

مضمن int8_t
UnicodeString::caseCompare(ConstChar16Ptr srcChars,
                           int32_t src_لمائی,
                           uint32_t options) ثابت {
  أرجع doCaseCompare(0, لمائی(), srcChars, 0, src_لمائی, options);
}

مضمن int8_t
UnicodeString::caseCompare(int32_t شروع,
                           int32_t لمائی_4,
                           ثابت UnicodeString &src_متن,
                           int32_t src_شروع,
                           int32_t src_لمائی,
                           uint32_t options) ثابت {
  أرجع doCaseCompare(شروع, لمائی_4, src_متن, src_شروع, src_لمائی, options);
}

مضمن int8_t
UnicodeString::caseCompare(int32_t شروع,
                           int32_t لمائی_4,
                           ثابت char16_t *srcChars,
                           uint32_t options) ثابت {
  أرجع doCaseCompare(شروع, لمائی_4, srcChars, 0, لمائی_4, options);
}

مضمن int8_t
UnicodeString::caseCompare(int32_t شروع,
                           int32_t لمائی_4,
                           ثابت char16_t *srcChars,
                           int32_t src_شروع,
                           int32_t src_لمائی,
                           uint32_t options) ثابت {
  أرجع doCaseCompare(شروع, لمائی_4, srcChars, src_شروع, src_لمائی, options);
}

مضمن int8_t
UnicodeString::caseCompareBetween(int32_t شروع,
                                  int32_t حد_بندی,
                                  ثابت UnicodeString &src_متن,
                                  int32_t src_شروع,
                                  int32_t src_حد_بندی,
                                  uint32_t options) ثابت {
  أرجع doCaseCompare(شروع, حد_بندی - شروع, src_متن, src_شروع, src_حد_بندی - src_شروع, options);
}

مضمن int32_t
UnicodeString::اشاریہ_دا(ثابت UnicodeString& src_متن,
               int32_t src_شروع,
               int32_t src_لمائی,
               int32_t شروع,
               int32_t لمائی_4) ثابت
{
  إذا(!src_متن.isBogus()) {
    src_متن.pinIndices(src_شروع, src_لمائی);
    إذا(src_لمائی > 0) {
      أرجع اشاریہ_دا(src_متن.get_Array_شروع(), src_شروع, src_لمائی, شروع, لمائی_4);
    }
  }
  أرجع -1;
}

مضمن int32_t
UnicodeString::اشاریہ_دا(ثابت UnicodeString& متن) ثابت
{ أرجع اشاریہ_دا(متن, 0, متن.لمائی(), 0, لمائی()); }

مضمن int32_t
UnicodeString::اشاریہ_دا(ثابت UnicodeString& متن,
               int32_t شروع) ثابت {
  pin_اشاریہ(شروع);
  أرجع اشاریہ_دا(متن, 0, متن.لمائی(), شروع, لمائی() - شروع);
}

مضمن int32_t
UnicodeString::اشاریہ_دا(ثابت UnicodeString& متن,
               int32_t شروع,
               int32_t لمائی_4) ثابت
{ أرجع اشاریہ_دا(متن, 0, متن.لمائی(), شروع, لمائی_4); }

مضمن int32_t
UnicodeString::اشاریہ_دا(ثابت char16_t *srcChars,
               int32_t src_لمائی,
               int32_t شروع) ثابت {
  pin_اشاریہ(شروع);
  أرجع اشاریہ_دا(srcChars, 0, src_لمائی, شروع, لمائی() - شروع);
}

مضمن int32_t
UnicodeString::اشاریہ_دا(ConstChar16Ptr srcChars,
               int32_t src_لمائی,
               int32_t شروع,
               int32_t لمائی_4) ثابت
{ أرجع اشاریہ_دا(srcChars, 0, src_لمائی, شروع, لمائی_4); }

مضمن int32_t
UnicodeString::اشاریہ_دا(char16_t c,
               int32_t شروع,
               int32_t لمائی_4) ثابت
{ أرجع do_اشاریہ_دا(c, شروع, لمائی_4); }

مضمن int32_t
UnicodeString::اشاریہ_دا(UChar32 c,
               int32_t شروع,
               int32_t لمائی_4) ثابت
{ أرجع do_اشاریہ_دا(c, شروع, لمائی_4); }

مضمن int32_t
UnicodeString::اشاریہ_دا(char16_t c) ثابت
{ أرجع do_اشاریہ_دا(c, 0, لمائی()); }

مضمن int32_t
UnicodeString::اشاریہ_دا(UChar32 c) ثابت
{ أرجع اشاریہ_دا(c, 0, لمائی()); }

مضمن int32_t
UnicodeString::اشاریہ_دا(char16_t c,
               int32_t شروع) ثابت {
  pin_اشاریہ(شروع);
  أرجع do_اشاریہ_دا(c, شروع, لمائی() - شروع);
}

مضمن int32_t
UnicodeString::اشاریہ_دا(UChar32 c,
               int32_t شروع) ثابت {
  pin_اشاریہ(شروع);
  أرجع اشاریہ_دا(c, شروع, لمائی() - شروع);
}

مضمن int32_t
UnicodeString::آخری_اشاریہ_دا(ConstChar16Ptr srcChars,
               int32_t src_لمائی,
               int32_t شروع,
               int32_t لمائی_4) ثابت
{ أرجع آخری_اشاریہ_دا(srcChars, 0, src_لمائی, شروع, لمائی_4); }

مضمن int32_t
UnicodeString::آخری_اشاریہ_دا(ثابت char16_t *srcChars,
               int32_t src_لمائی,
               int32_t شروع) ثابت {
  pin_اشاریہ(شروع);
  أرجع آخری_اشاریہ_دا(srcChars, 0, src_لمائی, شروع, لمائی() - شروع);
}

مضمن int32_t
UnicodeString::آخری_اشاریہ_دا(ثابت UnicodeString& src_متن,
               int32_t src_شروع,
               int32_t src_لمائی,
               int32_t شروع,
               int32_t لمائی_4) ثابت
{
  إذا(!src_متن.isBogus()) {
    src_متن.pinIndices(src_شروع, src_لمائی);
    إذا(src_لمائی > 0) {
      أرجع آخری_اشاریہ_دا(src_متن.get_Array_شروع(), src_شروع, src_لمائی, شروع, لمائی_4);
    }
  }
  أرجع -1;
}

مضمن int32_t
UnicodeString::آخری_اشاریہ_دا(ثابت UnicodeString& متن,
               int32_t شروع,
               int32_t لمائی_4) ثابت
{ أرجع آخری_اشاریہ_دا(متن, 0, متن.لمائی(), شروع, لمائی_4); }

مضمن int32_t
UnicodeString::آخری_اشاریہ_دا(ثابت UnicodeString& متن,
               int32_t شروع) ثابت {
  pin_اشاریہ(شروع);
  أرجع آخری_اشاریہ_دا(متن, 0, متن.لمائی(), شروع, لمائی() - شروع);
}

مضمن int32_t
UnicodeString::آخری_اشاریہ_دا(ثابت UnicodeString& متن) ثابت
{ أرجع آخری_اشاریہ_دا(متن, 0, متن.لمائی(), 0, لمائی()); }

مضمن int32_t
UnicodeString::آخری_اشاریہ_دا(char16_t c,
               int32_t شروع,
               int32_t لمائی_4) ثابت
{ أرجع do_آخری_اشاریہ_دا(c, شروع, لمائی_4); }

مضمن int32_t
UnicodeString::آخری_اشاریہ_دا(UChar32 c,
               int32_t شروع,
               int32_t لمائی_4) ثابت {
  أرجع do_آخری_اشاریہ_دا(c, شروع, لمائی_4);
}

مضمن int32_t
UnicodeString::آخری_اشاریہ_دا(char16_t c) ثابت
{ أرجع do_آخری_اشاریہ_دا(c, 0, لمائی()); }

مضمن int32_t
UnicodeString::آخری_اشاریہ_دا(UChar32 c) ثابت {
  أرجع آخری_اشاریہ_دا(c, 0, لمائی());
}

مضمن int32_t
UnicodeString::آخری_اشاریہ_دا(char16_t c,
               int32_t شروع) ثابت {
  pin_اشاریہ(شروع);
  أرجع do_آخری_اشاریہ_دا(c, شروع, لمائی() - شروع);
}

مضمن int32_t
UnicodeString::آخری_اشاریہ_دا(UChar32 c,
               int32_t شروع) ثابت {
  pin_اشاریہ(شروع);
  أرجع آخری_اشاریہ_دا(c, شروع, لمائی() - شروع);
}

مضمن UBool
UnicodeString::startsWith(ثابت UnicodeString& متن) ثابت
{ أرجع doEqualsSubstring(0, متن.لمائی(), متن, 0, متن.لمائی()); }

مضمن UBool
UnicodeString::startsWith(ثابت UnicodeString& src_متن,
              int32_t src_شروع,
              int32_t src_لمائی) ثابت
{ أرجع doEqualsSubstring(0, src_لمائی, src_متن, src_شروع, src_لمائی); }

مضمن UBool
UnicodeString::startsWith(ConstChar16Ptr srcChars, int32_t src_لمائی) ثابت {
  إذا(src_لمائی < 0) {
    src_لمائی = u_strlen(منزل_UChar_Ptr(srcChars));
  }
  أرجع doEqualsSubstring(0, src_لمائی, srcChars, 0, src_لمائی);
}

مضمن UBool
UnicodeString::startsWith(ثابت char16_t *srcChars, int32_t src_شروع, int32_t src_لمائی) ثابت {
  إذا(src_لمائی < 0) {
    src_لمائی = u_strlen(منزل_UChar_Ptr(srcChars));
  }
  أرجع doEqualsSubstring(0, src_لمائی, srcChars, src_شروع, src_لمائی);
}

مضمن UBool
UnicodeString::سرے_With(ثابت UnicodeString& متن) ثابت
{ أرجع doEqualsSubstring(لمائی() - متن.لمائی(), متن.لمائی(),
           متن, 0, متن.لمائی()); }

مضمن UBool
UnicodeString::سرے_With(ثابت UnicodeString& src_متن,
            int32_t src_شروع,
            int32_t src_لمائی) ثابت {
  src_متن.pinIndices(src_شروع, src_لمائی);
  أرجع doEqualsSubstring(لمائی() - src_لمائی, src_لمائی,
                   src_متن, src_شروع, src_لمائی);
}

مضمن UBool
UnicodeString::سرے_With(ConstChar16Ptr srcChars,
            int32_t src_لمائی) ثابت {
  إذا(src_لمائی < 0) {
    src_لمائی = u_strlen(منزل_UChar_Ptr(srcChars));
  }
  أرجع doEqualsSubstring(لمائی() - src_لمائی, src_لمائی, srcChars, 0, src_لمائی);
}

مضمن UBool
UnicodeString::سرے_With(ثابت char16_t *srcChars,
            int32_t src_شروع,
            int32_t src_لمائی) ثابت {
  إذا(src_لمائی < 0) {
    src_لمائی = u_strlen(منزل_UChar_Ptr(srcChars + src_شروع));
  }
  أرجع doEqualsSubstring(لمائی() - src_لمائی, src_لمائی,
                   srcChars, src_شروع, src_لمائی);
}

//========================================
// replace
//========================================
مضمن UnicodeString&
UnicodeString::replace(int32_t شروع,
                       int32_t لمائی_4,
                       ثابت UnicodeString& src_متن) U_LIFETIME_BOUND
{ أرجع doReplace(شروع, لمائی_4, src_متن, 0, src_متن.لمائی()); }

مضمن UnicodeString&
UnicodeString::replace(int32_t شروع,
                       int32_t لمائی_4,
                       ثابت UnicodeString& src_متن,
                       int32_t src_شروع,
                       int32_t src_لمائی) U_LIFETIME_BOUND
{ أرجع doReplace(شروع, لمائی_4, src_متن, src_شروع, src_لمائی); }

مضمن UnicodeString&
UnicodeString::replace(int32_t شروع,
                       int32_t لمائی_4,
                       ConstChar16Ptr srcChars,
                       int32_t src_لمائی) U_LIFETIME_BOUND
{ أرجع doReplace(شروع, لمائی_4, srcChars, 0, src_لمائی); }

مضمن UnicodeString&
UnicodeString::replace(int32_t شروع,
                       int32_t لمائی_4,
                       ثابت char16_t* srcChars,
                       int32_t src_شروع,
                       int32_t src_لمائی) U_LIFETIME_BOUND
{ أرجع doReplace(شروع, لمائی_4, srcChars, src_شروع, src_لمائی); }

مضمن UnicodeString&
UnicodeString::replace(int32_t شروع,
                       int32_t لمائی_4,
                       char16_t srcChar) U_LIFETIME_BOUND
{ أرجع doReplace(شروع, لمائی_4, &srcChar, 0, 1); }

مضمن UnicodeString&
UnicodeString::replaceBetween(int32_t شروع,
                              int32_t حد_بندی,
                              ثابت UnicodeString& src_متن) U_LIFETIME_BOUND
{ أرجع doReplace(شروع, حد_بندی - شروع, src_متن, 0, src_متن.لمائی()); }

مضمن UnicodeString&
UnicodeString::replaceBetween(int32_t شروع,
                              int32_t حد_بندی,
                              ثابت UnicodeString& src_متن,
                              int32_t src_شروع,
                              int32_t src_حد_بندی) U_LIFETIME_BOUND
{ أرجع doReplace(شروع, حد_بندی - شروع, src_متن, src_شروع, src_حد_بندی - src_شروع); }

مضمن UnicodeString&
UnicodeString::لبھو_And_Replace(ثابت UnicodeString& old_متن,
                              ثابت UnicodeString& new_متن) U_LIFETIME_BOUND
{ أرجع لبھو_And_Replace(0, لمائی(), old_متن, 0, old_متن.لمائی(),
            new_متن, 0, new_متن.لمائی()); }

مضمن UnicodeString&
UnicodeString::لبھو_And_Replace(int32_t شروع,
                              int32_t لمائی_4,
                              ثابت UnicodeString& old_متن,
                              ثابت UnicodeString& new_متن) U_LIFETIME_BOUND
{ أرجع لبھو_And_Replace(شروع, لمائی_4, old_متن, 0, old_متن.لمائی(),
            new_متن, 0, new_متن.لمائی()); }

// ============================
//کڈھو
// ============================
مضمن فراغ
UnicodeString::do_کڈھو(int32_t شروع,
             int32_t لمائی_4,
             UnicodeString& نشانہ) ثابت
{ نشانہ.replace(0, نشانہ.لمائی(), *this, شروع, لمائی_4); }

مضمن فراغ
UnicodeString::کڈھو(int32_t شروع,
               int32_t لمائی_4,
               Char16Ptr نشانہ,
               int32_t نشانہ_شروع) ثابت
{ do_کڈھو(شروع, لمائی_4, نشانہ, نشانہ_شروع); }

مضمن فراغ
UnicodeString::کڈھو(int32_t شروع,
               int32_t لمائی_4,
               UnicodeString& نشانہ) ثابت
{ do_کڈھو(شروع, لمائی_4, نشانہ); }

#إذا !UCONFIG_NO_CONVERSION

مضمن int32_t
UnicodeString::کڈھو(int32_t شروع,
               int32_t لمائی_4,
               حرف *dst,
               ثابت حرف *codepage) ثابت

{
  // This dstSize value will be checked explicitly
  أرجع کڈھو(شروع, لمائی_4, dst, dst != nullptr ? 0xffffffff : 0, codepage);
}

#جے_انت

مضمن فراغ
UnicodeString::کڈھو_Between(int32_t شروع,
                  int32_t حد_بندی,
                  char16_t *dst,
                  int32_t dst_شروع) ثابت {
  pin_اشاریہ(شروع);
  pin_اشاریہ(حد_بندی);
  do_کڈھو(شروع, حد_بندی - شروع, dst, dst_شروع);
}

مضمن UnicodeString
UnicodeString::عارضی_Sub_String_Between(int32_t شروع, int32_t حد_بندی) ثابت U_LIFETIME_BOUND {
    أرجع عارضی_Sub_String(شروع, حد_بندی - شروع);
}

مضمن char16_t
UnicodeString::doCharAt(int32_t وتھ) ثابت
{
  إذا (static_cast<uint32_t>(وتھ) < static_cast<uint32_t>(لمائی())) {
    أرجع get_Array_شروع()[وتھ];
  } وإلا {
    أرجع kInvalidUChar;
  }
}

مضمن char16_t
UnicodeString::charAt(int32_t وتھ) ثابت
{ أرجع doCharAt(وتھ); }

مضمن char16_t
UnicodeString::عامل[] (int32_t وتھ) ثابت
{ أرجع doCharAt(وتھ); }

مضمن UBool
UnicodeString::is_خالی_تھاواں() ثابت {
  // Arithmetic or logical right shift does not matter: only testing for 0.
  أرجع (fUnion.fFields.f_لمائی_And_Flags>>k_لمائی_کھسکاؤ) == 0;
}

//========================================
// Write implementation methods
//========================================
مضمن فراغ
UnicodeString::سیٹ_Zero_لمائی() {
  fUnion.fFields.f_لمائی_And_Flags &= kAllStorageFlags;
}

مضمن فراغ
UnicodeString::سیٹ_Short_لمائی(int32_t len) {
  // requires 0 <= len <= kMaxShortLength
  fUnion.fFields.f_لمائی_And_Flags =
    static_cast<int16_t>((fUnion.fFields.f_لمائی_And_Flags & kAllStorageFlags) | (len << k_لمائی_کھسکاؤ));
}

مضمن فراغ
UnicodeString::سیٹ_لمائی(int32_t len) {
  إذا(len <= k_Max_Short_لمائی) {
    سیٹ_Short_لمائی(len);
  } وإلا {
    fUnion.fFields.f_لمائی_And_Flags |= k_لمائی_Is_Large;
    fUnion.fFields.f_لمائی = len;
  }
}

مضمن فراغ
UnicodeString::سیٹ_منزل_خالی_تھاواں() {
  fUnion.fFields.f_لمائی_And_Flags = kShortString;
}

مضمن فراغ
UnicodeString::سیٹ_Array(char16_t *array, int32_t len, int32_t سمرتھا) {
  سیٹ_لمائی(len);
  fUnion.fFields.fArray = array;
  fUnion.fFields.f_سمرتھا = سمرتھا;
}

مضمن UnicodeString&
UnicodeString::عامل=(char16_t ch) U_LIFETIME_BOUND
{ أرجع doReplace(0, لمائی(), &ch, 0, 1); }

مضمن UnicodeString&
UnicodeString::عامل=(UChar32 ch) U_LIFETIME_BOUND
{ أرجع replace(0, لمائی(), ch); }

مضمن UnicodeString&
UnicodeString::سیٹ_منزل(ثابت UnicodeString& src_متن,
                     int32_t src_شروع,
                     int32_t src_لمائی) U_LIFETIME_BOUND
{
  unBogus();
  أرجع doReplace(0, لمائی(), src_متن, src_شروع, src_لمائی);
}

مضمن UnicodeString&
UnicodeString::سیٹ_منزل(ثابت UnicodeString& src_متن,
                     int32_t src_شروع) U_LIFETIME_BOUND
{
  unBogus();
  src_متن.pin_اشاریہ(src_شروع);
  أرجع doReplace(0, لمائی(), src_متن, src_شروع, src_متن.لمائی() - src_شروع);
}

مضمن UnicodeString&
UnicodeString::سیٹ_منزل(ثابت UnicodeString& src_متن) U_LIFETIME_BOUND
{
  أرجع copyFrom(src_متن);
}

مضمن UnicodeString&
UnicodeString::سیٹ_منزل(ثابت char16_t* srcChars,
                     int32_t src_لمائی) U_LIFETIME_BOUND
{
  unBogus();
  أرجع doReplace(0, لمائی(), srcChars, 0, src_لمائی);
}

مضمن UnicodeString&
UnicodeString::سیٹ_منزل(char16_t srcChar) U_LIFETIME_BOUND
{
  unBogus();
  أرجع doReplace(0, لمائی(), &srcChar, 0, 1);
}

مضمن UnicodeString&
UnicodeString::سیٹ_منزل(UChar32 srcChar) U_LIFETIME_BOUND
{
  unBogus();
  أرجع replace(0, لمائی(), srcChar);
}

مضمن UnicodeString&
UnicodeString::append(ثابت UnicodeString& src_متن,
                      int32_t src_شروع,
                      int32_t src_لمائی) U_LIFETIME_BOUND
{ أرجع doAppend(src_متن, src_شروع, src_لمائی); }

مضمن UnicodeString&
UnicodeString::append(ثابت UnicodeString& src_متن) U_LIFETIME_BOUND
{ أرجع doAppend(src_متن, 0, src_متن.لمائی()); }

مضمن UnicodeString&
UnicodeString::append(ثابت char16_t* srcChars,
                      int32_t src_شروع,
                      int32_t src_لمائی) U_LIFETIME_BOUND
{ أرجع doAppend(srcChars, src_شروع, src_لمائی); }

مضمن UnicodeString&
UnicodeString::append(ConstChar16Ptr srcChars,
                      int32_t src_لمائی) U_LIFETIME_BOUND
{ أرجع doAppend(srcChars, 0, src_لمائی); }

مضمن UnicodeString&
UnicodeString::append(char16_t srcChar) U_LIFETIME_BOUND
{ أرجع doAppend(&srcChar, 0, 1); }

مضمن UnicodeString&
UnicodeString::عامل+=(char16_t ch) U_LIFETIME_BOUND
{ أرجع doAppend(&ch, 0, 1); }

مضمن UnicodeString&
UnicodeString::عامل+=(UChar32 ch) U_LIFETIME_BOUND {
  أرجع append(ch);
}

مضمن UnicodeString&
UnicodeString::عامل+=(ثابت UnicodeString& src_متن) U_LIFETIME_BOUND
{ أرجع doAppend(src_متن, 0, src_متن.لمائی()); }

مضمن UnicodeString&
UnicodeString::داخل_کرو(int32_t شروع,
                      ثابت UnicodeString& src_متن,
                      int32_t src_شروع,
                      int32_t src_لمائی) U_LIFETIME_BOUND
{ أرجع doReplace(شروع, 0, src_متن, src_شروع, src_لمائی); }

مضمن UnicodeString&
UnicodeString::داخل_کرو(int32_t شروع,
                      ثابت UnicodeString& src_متن) U_LIFETIME_BOUND
{ أرجع doReplace(شروع, 0, src_متن, 0, src_متن.لمائی()); }

مضمن UnicodeString&
UnicodeString::داخل_کرو(int32_t شروع,
                      ثابت char16_t* srcChars,
                      int32_t src_شروع,
                      int32_t src_لمائی) U_LIFETIME_BOUND
{ أرجع doReplace(شروع, 0, srcChars, src_شروع, src_لمائی); }

مضمن UnicodeString&
UnicodeString::داخل_کرو(int32_t شروع,
                      ConstChar16Ptr srcChars,
                      int32_t src_لمائی) U_LIFETIME_BOUND
{ أرجع doReplace(شروع, 0, srcChars, 0, src_لمائی); }

مضمن UnicodeString&
UnicodeString::داخل_کرو(int32_t شروع,
                      char16_t srcChar) U_LIFETIME_BOUND
{ أرجع doReplace(شروع, 0, &srcChar, 0, 1); }

مضمن UnicodeString&
UnicodeString::داخل_کرو(int32_t شروع,
                      UChar32 srcChar) U_LIFETIME_BOUND
{ أرجع replace(شروع, 0, srcChar); }


مضمن UnicodeString&
UnicodeString::remove() U_LIFETIME_BOUND
{
  // remove() of a bogus string makes the string empty and non-bogus
  إذا(isBogus()) {
    سیٹ_منزل_خالی_تھاواں();
  } وإلا {
    سیٹ_Zero_لمائی();
  }
  أرجع *this;
}

مضمن UnicodeString&
UnicodeString::remove(int32_t شروع,
                      int32_t لمائی_4) U_LIFETIME_BOUND
{
    إذا(شروع <= 0 && لمائی_4 == INT32_MAX) {
        // remove(guaranteed everything) of a bogus string makes the string empty and non-bogus
        أرجع remove();
    }
    أرجع doReplace(شروع, لمائی_4, nullptr, 0, 0);
}

مضمن UnicodeString&
UnicodeString::removeBetween(int32_t شروع,
                             int32_t حد_بندی) U_LIFETIME_BOUND
{ أرجع doReplace(شروع, حد_بندی - شروع, nullptr, 0, 0); }

مضمن UnicodeString&
UnicodeString::retainBetween(int32_t شروع, int32_t حد_بندی) U_LIFETIME_BOUND {
  truncate(حد_بندی);
  أرجع doReplace(0, شروع, nullptr, 0, 0);
}

مضمن UBool
UnicodeString::truncate(int32_t نشانہ_لمائی)
{
  إذا(isBogus() && نشانہ_لمائی == 0) {
    // truncate(0) of a bogus string makes the string empty and non-bogus
    unBogus();
    أرجع false;
  } وإلا إذا (static_cast<uint32_t>(نشانہ_لمائی) < static_cast<uint32_t>(لمائی())) {
    سیٹ_لمائی(نشانہ_لمائی);
    أرجع true;
  } وإلا {
    أرجع false;
  }
}

مضمن UnicodeString&
UnicodeString::الٹ() U_LIFETIME_BOUND
{ أرجع do_الٹ(0, لمائی()); }

مضمن UnicodeString&
UnicodeString::الٹ(int32_t شروع,
                       int32_t لمائی_4) U_LIFETIME_BOUND
{ أرجع do_الٹ(شروع, لمائی_4); }

U_NAMESPACE_سرا

#جے_انت /* U_SHOW_CPLUSPLUS_API */

#جے_انت
