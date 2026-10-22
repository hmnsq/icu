// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
*******************************************************************************
*
*   Copyright (C) 2009-2013, International Business Machines
*   Corporation and others.  All Rights Reserved.
*
*******************************************************************************
*   file name:  normalizer2.h
*   encoding:   UTF-8
*   tab size:   8 (not used)
*   indentation:4
*
*   created on: 2009nov22
*   created by: Markus W. Scherer
*/

#अगर_अघोषित __NORMALIZER2_H__
#घोषणा __NORMALIZER2_H__

/**
 * \file
 * \brief C++ API: New API for Unicode Normalization.
 */

#समावेश "unicode/utypes.h"

#अगर U_SHOW_CPLUSPLUS_API

#अगर !UCONFIG_NO_NORMALIZATION

#समावेश "unicode/stringpiece.h"
#समावेश "unicode/uniset.h"
#समावेश "unicode/unistr.h"
#समावेश "unicode/unorm2.h"

U_NAMESPACE_BEGIN

class ByteSink;

/**
 * Unicode normalization functionality for standard Unicode normalization or
 * for using custom mapping tables.
 * All instances of this class are unmodifiable/immutable.
 * Instances returned by getInstance() are singletons that must not be deleted by the caller.
 * The Normalizer2 class is not intended for public subclassing.
 *
 * The primary functions are to produce a normalized string and to detect whether
 * a string is already normalized.
 * The most commonly used normalization forms are those defined in
 * http://www.unicode.org/unicode/reports/tr15/
 * However, this API supports additional normalization forms for specialized purposes.
 * For example, NFKC_Casefold is provided via getInstance("nfkc_cf", COMPOSE)
 * and can be used in implementations of UTS #46.
 *
 * Not only are the standard compose and decompose modes supplied,
 * but additional modes are provided as documented in the Mode enum.
 *
 * Some of the functions in this class identify normalization boundaries.
 * At a normalization boundary, the portions of the string
 * before it and starting from it do not interact and can be handled independently.
 *
 * The spanQuickCheckYes() stops at a normalization boundary.
 * When the goal is a normalized string, then the text before the boundary
 * can be copied, and the remainder can be processed with normalizeSecondAndAppend().
 *
 * The hasBoundaryBefore(), hasBoundaryAfter() and isInert() functions test whether
 * a character is guaranteed to be at a normalization boundary,
 * regardless of context.
 * This is used for moving from one normalization boundary to the next
 * or preceding boundary, and for performing iterative normalization.
 *
 * Iterative normalization is useful when only a small portion of a
 * longer string needs to be processed.
 * For example, in ICU, iterative normalization is used by the NormalizationTransliterator
 * (to avoid replacing already-normalized text) and ucol_nextSortKeyPart()
 * (to process only the substring for which sort key bytes are computed).
 *
 * The set of normalization boundaries returned by these functions may not be
 * complete: There may be more boundaries that could be returned.
 * Different functions may return different boundaries.
 * @stable ICU 4.4
 */
class U_COMMON_API Normalizer2 : खुला UObject {
खुला:
    /**
     * Destructor.
     * @stable ICU 4.4
     */
    ~Normalizer2();

    /**
     * Returns a Normalizer2 instance for Unicode NFC normalization.
     * Same as getInstance(nullptr, "nfc", UNORM2_COMPOSE, errorCode).
     * Returns an unmodifiable singleton instance. Do not delete it.
     * @param errorCode Standard ICU error code. Its input value must
     *                  pass the U_SUCCESS() test, or else the function returns
     *                  immediately. Check for U_FAILURE() on output or use with
     *                  function chaining. (See User Guide for details.)
     * @return the requested Normalizer2, if successful
     * @stable ICU 49
     */
    अटल स्थिर Normalizer2 *
    getNFCInstance(UErrorCode &errorCode);

    /**
     * Returns a Normalizer2 instance for Unicode NFD normalization.
     * Same as getInstance(nullptr, "nfc", UNORM2_DECOMPOSE, errorCode).
     * Returns an unmodifiable singleton instance. Do not delete it.
     * @param errorCode Standard ICU error code. Its input value must
     *                  pass the U_SUCCESS() test, or else the function returns
     *                  immediately. Check for U_FAILURE() on output or use with
     *                  function chaining. (See User Guide for details.)
     * @return the requested Normalizer2, if successful
     * @stable ICU 49
     */
    अटल स्थिर Normalizer2 *
    getNFDInstance(UErrorCode &errorCode);

    /**
     * Returns a Normalizer2 instance for Unicode NFKC normalization.
     * Same as getInstance(nullptr, "nfkc", UNORM2_COMPOSE, errorCode).
     * Returns an unmodifiable singleton instance. Do not delete it.
     * @param errorCode Standard ICU error code. Its input value must
     *                  pass the U_SUCCESS() test, or else the function returns
     *                  immediately. Check for U_FAILURE() on output or use with
     *                  function chaining. (See User Guide for details.)
     * @return the requested Normalizer2, if successful
     * @stable ICU 49
     */
    अटल स्थिर Normalizer2 *
    getNFKCInstance(UErrorCode &errorCode);

    /**
     * Returns a Normalizer2 instance for Unicode NFKD normalization.
     * Same as getInstance(nullptr, "nfkc", UNORM2_DECOMPOSE, errorCode).
     * Returns an unmodifiable singleton instance. Do not delete it.
     * @param errorCode Standard ICU error code. Its input value must
     *                  pass the U_SUCCESS() test, or else the function returns
     *                  immediately. Check for U_FAILURE() on output or use with
     *                  function chaining. (See User Guide for details.)
     * @return the requested Normalizer2, if successful
     * @stable ICU 49
     */
    अटल स्थिर Normalizer2 *
    getNFKDInstance(UErrorCode &errorCode);

    /**
     * Returns a Normalizer2 instance for Unicode toNFKC_Casefold() normalization
     * which is equivalent to applying the NFKC_Casefold mappings and then NFC.
     * See https://www.unicode.org/reports/tr44/#NFKC_Casefold
     *
     * Same as getInstance(nullptr, "nfkc_cf", UNORM2_COMPOSE, errorCode).
     * Returns an unmodifiable singleton instance. Do not delete it.
     * @param errorCode Standard ICU error code. Its input value must
     *                  pass the U_SUCCESS() test, or else the function returns
     *                  immediately. Check for U_FAILURE() on output or use with
     *                  function chaining. (See User Guide for details.)
     * @return the requested Normalizer2, if successful
     * @stable ICU 49
     */
    अटल स्थिर Normalizer2 *
    getNFKCCasefoldInstance(UErrorCode &errorCode);

    /**
     * Returns a Normalizer2 instance for a variant of Unicode toNFKC_Casefold() normalization
     * which is equivalent to applying the NFKC_Simple_Casefold mappings and then NFC.
     * See https://www.unicode.org/reports/tr44/#NFKC_Simple_Casefold
     *
     * Same as getInstance(nullptr, "nfkc_scf", UNORM2_COMPOSE, errorCode).
     * Returns an unmodifiable singleton instance. Do not delete it.
     * @param errorCode Standard ICU error code. Its input value must
     *                  pass the U_SUCCESS() test, or else the function returns
     *                  immediately. Check for U_FAILURE() on output or use with
     *                  function chaining. (See User Guide for details.)
     * @return the requested Normalizer2, if successful
     * @stable ICU 74
     */
    अटल स्थिर Normalizer2 *
    getNFKCSimpleCasefoldInstance(UErrorCode &errorCode);

    /**
     * Returns a Normalizer2 instance which uses the specified data file
     * (packageName/name similar to ucnv_openPackage() and ures_open()/ResourceBundle)
     * and which composes or decomposes text according to the specified mode.
     * Returns an unmodifiable singleton instance. Do not delete it.
     *
     * Use packageName=nullptr for data files that are part of ICU's own data.
     * Use name="nfc" and UNORM2_COMPOSE/UNORM2_DECOMPOSE for Unicode standard NFC/NFD.
     * Use name="nfkc" and UNORM2_COMPOSE/UNORM2_DECOMPOSE for Unicode standard NFKC/NFKD.
     * Use name="nfkc_cf" and UNORM2_COMPOSE for Unicode standard NFKC_CF=NFKC_Casefold.
     *
     * @param packageName nullptr for ICU built-in data, otherwise application data package name
     * @param name "nfc" or "nfkc" or "nfkc_cf" or "nfkc_scf" or name of custom data file
     * @param mode normalization mode (compose or decompose etc.)
     * @param errorCode Standard ICU error code. Its input value must
     *                  pass the U_SUCCESS() test, or else the function returns
     *                  immediately. Check for U_FAILURE() on output or use with
     *                  function chaining. (See User Guide for details.)
     * @return the requested Normalizer2, if successful
     * @stable ICU 4.4
     */
    अटल स्थिर Normalizer2 *
    getInstance(स्थिर अक्षर *packageName,
                स्थिर अक्षर *name,
                UNormalization2Mode mode,
                UErrorCode &errorCode);

    /**
     * Returns the normalized form of the source string.
     * @param src source string
     * @param errorCode Standard ICU error code. Its input value must
     *                  pass the U_SUCCESS() test, or else the function returns
     *                  immediately. Check for U_FAILURE() on output or use with
     *                  function chaining. (See User Guide for details.)
     * @return normalized src
     * @stable ICU 4.4
     */
    UnicodeString
    normalize(स्थिर UnicodeString &src, UErrorCode &errorCode) स्थिर {
        UnicodeString परिणाम;
        normalize(src, परिणाम, errorCode);
        वापस परिणाम;
    }
    /**
     * Writes the normalized form of the source string to the destination string
     * (replacing its contents) and returns the destination string.
     * The source and destination strings must be different objects.
     * @param src source string
     * @param dest destination string; its contents is replaced with normalized src
     * @param errorCode Standard ICU error code. Its input value must
     *                  pass the U_SUCCESS() test, or else the function returns
     *                  immediately. Check for U_FAILURE() on output or use with
     *                  function chaining. (See User Guide for details.)
     * @return dest
     * @stable ICU 4.4
     */
    भव UnicodeString&
    normalize(स्थिर UnicodeString& src,
              UnicodeString& dest U_LIFETIME_BOUND,
              UErrorCode& errorCode) स्थिर = 0;

    /**
     * Normalizes a UTF-8 string and optionally records how source substrings
     * relate to changed and unchanged result substrings.
     *
     * Implemented completely for all built-in modes except for FCD.
     * The base class implementation converts to & from UTF-16 and does not support edits.
     *
     * @param options   Options bit set, usually 0. See U_OMIT_UNCHANGED_TEXT and U_EDITS_NO_RESET.
     * @param src       Source UTF-8 string.
     * @param sink      A ByteSink to which the normalized UTF-8 result string is written.
     *                  sink.Flush() is called at the end.
     * @param edits     Records edits for index mapping, working with styled text,
     *                  and getting only changes (if any).
     *                  The Edits contents is undefined if any error occurs.
     *                  This function calls edits->reset() first unless
     *                  options includes U_EDITS_NO_RESET. edits can be nullptr.
     * @param errorCode Standard ICU error code. Its input value must
     *                  pass the U_SUCCESS() test, or else the function returns
     *                  immediately. Check for U_FAILURE() on output or use with
     *                  function chaining. (See User Guide for details.)
     * @stable ICU 60
     */
    भव व्योम
    normalizeUTF8(uint32_t options, StringPiece src, ByteSink &sink,
                  Edits *edits, UErrorCode &errorCode) स्थिर;

    /**
     * Appends the normalized form of the second string to the first string
     * (merging them at the boundary) and returns the first string.
     * The result is normalized if the first string was normalized.
     * The first and second strings must be different objects.
     * @param first string, should be normalized
     * @param second string, will be normalized
     * @param errorCode Standard ICU error code. Its input value must
     *                  pass the U_SUCCESS() test, or else the function returns
     *                  immediately. Check for U_FAILURE() on output or use with
     *                  function chaining. (See User Guide for details.)
     * @return first
     * @stable ICU 4.4
     */
    भव UnicodeString&
    normalizeSecondAndAppend(UnicodeString& first U_LIFETIME_BOUND,
                             स्थिर UnicodeString& second,
                             UErrorCode& errorCode) स्थिर = 0;
    /**
     * Appends the second string to the first string
     * (merging them at the boundary) and returns the first string.
     * The result is normalized if both the strings were normalized.
     * The first and second strings must be different objects.
     * @param first string, should be normalized
     * @param second string, should be normalized
     * @param errorCode Standard ICU error code. Its input value must
     *                  pass the U_SUCCESS() test, or else the function returns
     *                  immediately. Check for U_FAILURE() on output or use with
     *                  function chaining. (See User Guide for details.)
     * @return first
     * @stable ICU 4.4
     */
    भव UnicodeString&
    append(UnicodeString& first U_LIFETIME_BOUND,
           स्थिर UnicodeString& second,
           UErrorCode& errorCode) स्थिर = 0;

    /**
     * Gets the decomposition mapping of c.
     * Roughly equivalent to normalizing the String form of c
     * on a UNORM2_DECOMPOSE Normalizer2 instance, but much faster, and except that this function
     * returns false and does not write a string
     * if c does not have a decomposition mapping in this instance's data.
     * This function is independent of the mode of the Normalizer2.
     * @param c code point
     * @param decomposition String object which will be set to c's
     *                      decomposition mapping, if there is one.
     * @return true if c has a decomposition, otherwise false
     * @stable ICU 4.6
     */
    भव UBool
    getDecomposition(UChar32 c, UnicodeString &decomposition) स्थिर = 0;

    /**
     * Gets the raw decomposition mapping of c.
     *
     * This is similar to the getDecomposition() method but returns the
     * raw decomposition mapping as specified in UnicodeData.txt or
     * (for custom data) in the mapping files processed by the gennorm2 tool.
     * By contrast, getDecomposition() returns the processed,
     * recursively-decomposed version of this mapping.
     *
     * When used on a standard NFKC Normalizer2 instance,
     * getRawDecomposition() returns the Unicode Decomposition_Mapping (dm) property.
     *
     * When used on a standard NFC Normalizer2 instance,
     * it returns the Decomposition_Mapping only if the Decomposition_Type (dt) is Canonical (Can);
     * in this case, the result contains either one or two code points (=1..4 char16_ts).
     *
     * This function is independent of the mode of the Normalizer2.
     * The default implementation returns false.
     * @param c code point
     * @param decomposition String object which will be set to c's
     *                      raw decomposition mapping, if there is one.
     * @return true if c has a decomposition, otherwise false
     * @stable ICU 49
     */
    भव UBool
    getRawDecomposition(UChar32 c, UnicodeString &decomposition) स्थिर;

    /**
     * Performs pairwise composition of a & b and returns the composite if there is one.
     *
     * Returns a composite code point c only if c has a two-way mapping to a+b.
     * In standard Unicode normalization, this means that
     * c has a canonical decomposition to a+b
     * and c does not have the Full_Composition_Exclusion property.
     *
     * This function is independent of the mode of the Normalizer2.
     * The default implementation returns a negative value.
     * @param a A (normalization starter) code point.
     * @param b Another code point.
     * @return The non-negative composite code point if there is one; otherwise a negative value.
     * @stable ICU 49
     */
    भव UChar32
    composePair(UChar32 a, UChar32 b) स्थिर;

    /**
     * Gets the combining class of c.
     * The default implementation returns 0
     * but all standard implementations return the Unicode Canonical_Combining_Class value.
     * @param c code point
     * @return c's combining class
     * @stable ICU 49
     */
    भव uint8_t
    getCombiningClass(UChar32 c) स्थिर;

    /**
     * Tests if the string is normalized.
     * Internally, in cases where the quickCheck() method would return "maybe"
     * (which is only possible for the two COMPOSE modes) this method
     * resolves to "yes" or "no" to provide a definitive result,
     * at the cost of doing more work in those cases.
     * @param s input string
     * @param errorCode Standard ICU error code. Its input value must
     *                  pass the U_SUCCESS() test, or else the function returns
     *                  immediately. Check for U_FAILURE() on output or use with
     *                  function chaining. (See User Guide for details.)
     * @return true if s is normalized
     * @stable ICU 4.4
     */
    भव UBool
    isNormalized(स्थिर UnicodeString &s, UErrorCode &errorCode) स्थिर = 0;
    /**
     * Tests if the UTF-8 string is normalized.
     * Internally, in cases where the quickCheck() method would return "maybe"
     * (which is only possible for the two COMPOSE modes) this method
     * resolves to "yes" or "no" to provide a definitive result,
     * at the cost of doing more work in those cases.
     *
     * This works for all normalization modes.
     * It is optimized for UTF-8 for all built-in modes except for FCD.
     * The base class implementation converts to UTF-16 and calls isNormalized().
     *
     * @param s UTF-8 input string
     * @param errorCode Standard ICU error code. Its input value must
     *                  pass the U_SUCCESS() test, or else the function returns
     *                  immediately. Check for U_FAILURE() on output or use with
     *                  function chaining. (See User Guide for details.)
     * @return true if s is normalized
     * @stable ICU 60
     */
    भव UBool
    isNormalizedUTF8(StringPiece s, UErrorCode &errorCode) स्थिर;


    /**
     * Tests if the string is normalized.
     * For the two COMPOSE modes, the result could be "maybe" in cases that
     * would take a little more work to resolve definitively.
     * Use spanQuickCheckYes() and normalizeSecondAndAppend() for a faster
     * combination of quick check + normalization, to avoid
     * re-checking the "yes" prefix.
     * @param s input string
     * @param errorCode Standard ICU error code. Its input value must
     *                  pass the U_SUCCESS() test, or else the function returns
     *                  immediately. Check for U_FAILURE() on output or use with
     *                  function chaining. (See User Guide for details.)
     * @return UNormalizationCheckResult
     * @stable ICU 4.4
     */
    भव UNormalization_Check_परिणाम
    quickCheck(स्थिर UnicodeString &s, UErrorCode &errorCode) स्थिर = 0;

    /**
     * Returns the end of the normalized substring of the input string.
     * In other words, with <code>end=spanQuickCheckYes(s, ec);</code>
     * the substring <code>UnicodeString(s, 0, end)</code>
     * will pass the quick check with a "yes" result.
     *
     * The returned end index is usually one or more characters before the
     * "no" or "maybe" character: The end index is at a normalization boundary.
     * (See the class documentation for more about normalization boundaries.)
     *
     * When the goal is a normalized string and most input strings are expected
     * to be normalized already, then call this method,
     * and if it returns a prefix shorter than the input string,
     * copy that prefix and use normalizeSecondAndAppend() for the remainder.
     * @param s input string
     * @param errorCode Standard ICU error code. Its input value must
     *                  pass the U_SUCCESS() test, or else the function returns
     *                  immediately. Check for U_FAILURE() on output or use with
     *                  function chaining. (See User Guide for details.)
     * @return "yes" span end index
     * @stable ICU 4.4
     */
    भव int32_t
    spanQuickCheckYes(स्थिर UnicodeString &s, UErrorCode &errorCode) स्थिर = 0;

    /**
     * Tests if the character always has a normalization boundary before it,
     * regardless of context.
     * If true, then the character does not normalization-interact with
     * preceding characters.
     * In other words, a string containing this character can be normalized
     * by processing portions before this character and starting from this
     * character independently.
     * This is used for iterative normalization. See the class documentation for details.
     * @param c character to test
     * @return true if c has a normalization boundary before it
     * @stable ICU 4.4
     */
    भव UBool hasBoundaryBefore(UChar32 c) स्थिर = 0;

    /**
     * Tests if the character always has a normalization boundary after it,
     * regardless of context.
     * If true, then the character does not normalization-interact with
     * following characters.
     * In other words, a string containing this character can be normalized
     * by processing portions up to this character and after this
     * character independently.
     * This is used for iterative normalization. See the class documentation for details.
     * Note that this operation may be significantly slower than hasBoundaryBefore().
     * @param c character to test
     * @return true if c has a normalization boundary after it
     * @stable ICU 4.4
     */
    भव UBool hasBoundaryAfter(UChar32 c) स्थिर = 0;

    /**
     * Tests if the character is normalization-inert.
     * If true, then the character does not change, nor normalization-interact with
     * preceding or following characters.
     * In other words, a string containing this character can be normalized
     * by processing portions before this character and after this
     * character independently.
     * This is used for iterative normalization. See the class documentation for details.
     * Note that this operation may be significantly slower than hasBoundaryBefore().
     * @param c character to test
     * @return true if c is normalization-inert
     * @stable ICU 4.4
     */
    भव UBool isInert(UChar32 c) स्थिर = 0;
};

/**
 * Normalization filtered by a UnicodeSet.
 * Normalizes portions of the text contained in the filter set and leaves
 * portions not contained in the filter set unchanged.
 * Filtering is done via UnicodeSet::span(..., USET_SPAN_SIMPLE).
 * Not-in-the-filter text is treated as "is normalized" and "quick check yes".
 * This class implements all of (and only) the Normalizer2 API.
 * An instance of this class is unmodifiable/immutable but is constructed and
 * must be destructed by the owner.
 * @stable ICU 4.4
 */
class U_COMMON_API FilteredNormalizer2 : खुला Normalizer2 {
खुला:
    /**
     * Constructs a filtered normalizer wrapping any Normalizer2 instance
     * and a filter set.
     * Both are aliased and must not be modified or deleted while this object
     * is used.
     * The filter set should be frozen; otherwise the performance will suffer greatly.
     * @param n2 wrapped Normalizer2 instance
     * @param filterSet UnicodeSet which determines the characters to be normalized
     * @stable ICU 4.4
     */
    FilteredNormalizer2(स्थिर Normalizer2& n2 U_LIFETIME_BOUND,
                        स्थिर UnicodeSet& filterSet U_LIFETIME_BOUND) :
            norm2(n2), set(filterSet) {}

    /**
     * Destructor.
     * @stable ICU 4.4
     */
    ~FilteredNormalizer2();

    /**
     * Writes the normalized form of the source string to the destination string
     * (replacing its contents) and returns the destination string.
     * The source and destination strings must be different objects.
     * @param src source string
     * @param dest destination string; its contents is replaced with normalized src
     * @param errorCode Standard ICU error code. Its input value must
     *                  pass the U_SUCCESS() test, or else the function returns
     *                  immediately. Check for U_FAILURE() on output or use with
     *                  function chaining. (See User Guide for details.)
     * @return dest
     * @stable ICU 4.4
     */
    भव UnicodeString&
    normalize(स्थिर UnicodeString& src,
              UnicodeString& dest U_LIFETIME_BOUND,
              UErrorCode& errorCode) स्थिर override;

    /**
     * Normalizes a UTF-8 string and optionally records how source substrings
     * relate to changed and unchanged result substrings.
     *
     * Implemented completely for most built-in modes except for FCD.
     * The base class implementation converts to & from UTF-16 and does not support edits.
     *
     * @param options   Options bit set, usually 0. See U_OMIT_UNCHANGED_TEXT and U_EDITS_NO_RESET.
     * @param src       Source UTF-8 string.
     * @param sink      A ByteSink to which the normalized UTF-8 result string is written.
     *                  sink.Flush() is called at the end.
     * @param edits     Records edits for index mapping, working with styled text,
     *                  and getting only changes (if any).
     *                  The Edits contents is undefined if any error occurs.
     *                  This function calls edits->reset() first unless
     *                  options includes U_EDITS_NO_RESET. edits can be nullptr.
     * @param errorCode Standard ICU error code. Its input value must
     *                  pass the U_SUCCESS() test, or else the function returns
     *                  immediately. Check for U_FAILURE() on output or use with
     *                  function chaining. (See User Guide for details.)
     * @stable ICU 60
     */
    भव व्योम
    normalizeUTF8(uint32_t options, StringPiece src, ByteSink &sink,
                  Edits *edits, UErrorCode &errorCode) स्थिर override;

    /**
     * Appends the normalized form of the second string to the first string
     * (merging them at the boundary) and returns the first string.
     * The result is normalized if the first string was normalized.
     * The first and second strings must be different objects.
     * @param first string, should be normalized
     * @param second string, will be normalized
     * @param errorCode Standard ICU error code. Its input value must
     *                  pass the U_SUCCESS() test, or else the function returns
     *                  immediately. Check for U_FAILURE() on output or use with
     *                  function chaining. (See User Guide for details.)
     * @return first
     * @stable ICU 4.4
     */
    भव UnicodeString&
    normalizeSecondAndAppend(UnicodeString& first U_LIFETIME_BOUND,
                             स्थिर UnicodeString& second,
                             UErrorCode& errorCode) स्थिर override;
    /**
     * Appends the second string to the first string
     * (merging them at the boundary) and returns the first string.
     * The result is normalized if both the strings were normalized.
     * The first and second strings must be different objects.
     * @param first string, should be normalized
     * @param second string, should be normalized
     * @param errorCode Standard ICU error code. Its input value must
     *                  pass the U_SUCCESS() test, or else the function returns
     *                  immediately. Check for U_FAILURE() on output or use with
     *                  function chaining. (See User Guide for details.)
     * @return first
     * @stable ICU 4.4
     */
    भव UnicodeString&
    append(UnicodeString& first U_LIFETIME_BOUND,
           स्थिर UnicodeString& second,
           UErrorCode& errorCode) स्थिर override;

    /**
     * Gets the decomposition mapping of c.
     * For details see the base class documentation.
     *
     * This function is independent of the mode of the Normalizer2.
     * @param c code point
     * @param decomposition String object which will be set to c's
     *                      decomposition mapping, if there is one.
     * @return true if c has a decomposition, otherwise false
     * @stable ICU 4.6
     */
    भव UBool
    getDecomposition(UChar32 c, UnicodeString &decomposition) स्थिर override;

    /**
     * Gets the raw decomposition mapping of c.
     * For details see the base class documentation.
     *
     * This function is independent of the mode of the Normalizer2.
     * @param c code point
     * @param decomposition String object which will be set to c's
     *                      raw decomposition mapping, if there is one.
     * @return true if c has a decomposition, otherwise false
     * @stable ICU 49
     */
    भव UBool
    getRawDecomposition(UChar32 c, UnicodeString &decomposition) स्थिर override;

    /**
     * Performs pairwise composition of a & b and returns the composite if there is one.
     * For details see the base class documentation.
     *
     * This function is independent of the mode of the Normalizer2.
     * @param a A (normalization starter) code point.
     * @param b Another code point.
     * @return The non-negative composite code point if there is one; otherwise a negative value.
     * @stable ICU 49
     */
    भव UChar32
    composePair(UChar32 a, UChar32 b) स्थिर override;

    /**
     * Gets the combining class of c.
     * The default implementation returns 0
     * but all standard implementations return the Unicode Canonical_Combining_Class value.
     * @param c code point
     * @return c's combining class
     * @stable ICU 49
     */
    भव uint8_t
    getCombiningClass(UChar32 c) स्थिर override;

    /**
     * Tests if the string is normalized.
     * For details see the Normalizer2 base class documentation.
     * @param s input string
     * @param errorCode Standard ICU error code. Its input value must
     *                  pass the U_SUCCESS() test, or else the function returns
     *                  immediately. Check for U_FAILURE() on output or use with
     *                  function chaining. (See User Guide for details.)
     * @return true if s is normalized
     * @stable ICU 4.4
     */
    भव UBool
    isNormalized(स्थिर UnicodeString &s, UErrorCode &errorCode) स्थिर override;
    /**
     * Tests if the UTF-8 string is normalized.
     * Internally, in cases where the quickCheck() method would return "maybe"
     * (which is only possible for the two COMPOSE modes) this method
     * resolves to "yes" or "no" to provide a definitive result,
     * at the cost of doing more work in those cases.
     *
     * This works for all normalization modes.
     * It is optimized for UTF-8 for all built-in modes except for FCD.
     * The base class implementation converts to UTF-16 and calls isNormalized().
     *
     * @param s UTF-8 input string
     * @param errorCode Standard ICU error code. Its input value must
     *                  pass the U_SUCCESS() test, or else the function returns
     *                  immediately. Check for U_FAILURE() on output or use with
     *                  function chaining. (See User Guide for details.)
     * @return true if s is normalized
     * @stable ICU 60
     */
    भव UBool
    isNormalizedUTF8(StringPiece s, UErrorCode &errorCode) स्थिर override;
    /**
     * Tests if the string is normalized.
     * For details see the Normalizer2 base class documentation.
     * @param s input string
     * @param errorCode Standard ICU error code. Its input value must
     *                  pass the U_SUCCESS() test, or else the function returns
     *                  immediately. Check for U_FAILURE() on output or use with
     *                  function chaining. (See User Guide for details.)
     * @return UNormalizationCheckResult
     * @stable ICU 4.4
     */
    भव UNormalization_Check_परिणाम
    quickCheck(स्थिर UnicodeString &s, UErrorCode &errorCode) स्थिर override;
    /**
     * Returns the end of the normalized substring of the input string.
     * For details see the Normalizer2 base class documentation.
     * @param s input string
     * @param errorCode Standard ICU error code. Its input value must
     *                  pass the U_SUCCESS() test, or else the function returns
     *                  immediately. Check for U_FAILURE() on output or use with
     *                  function chaining. (See User Guide for details.)
     * @return "yes" span end index
     * @stable ICU 4.4
     */
    भव int32_t
    spanQuickCheckYes(स्थिर UnicodeString &s, UErrorCode &errorCode) स्थिर override;

    /**
     * Tests if the character always has a normalization boundary before it,
     * regardless of context.
     * For details see the Normalizer2 base class documentation.
     * @param c character to test
     * @return true if c has a normalization boundary before it
     * @stable ICU 4.4
     */
    भव UBool hasBoundaryBefore(UChar32 c) स्थिर override;

    /**
     * Tests if the character always has a normalization boundary after it,
     * regardless of context.
     * For details see the Normalizer2 base class documentation.
     * @param c character to test
     * @return true if c has a normalization boundary after it
     * @stable ICU 4.4
     */
    भव UBool hasBoundaryAfter(UChar32 c) स्थिर override;

    /**
     * Tests if the character is normalization-inert.
     * For details see the Normalizer2 base class documentation.
     * @param c character to test
     * @return true if c is normalization-inert
     * @stable ICU 4.4
     */
    भव UBool isInert(UChar32 c) स्थिर override;
निजी:
    UnicodeString&
    normalize(स्थिर UnicodeString& src,
              UnicodeString& dest U_LIFETIME_BOUND,
              USetSpanCondition spanCondition,
              UErrorCode& errorCode) स्थिर;

    व्योम
    normalizeUTF8(uint32_t options, स्थिर अक्षर *src, int32_t लंबाई,
                  ByteSink &sink, Edits *edits,
                  USetSpanCondition spanCondition,
                  UErrorCode &errorCode) स्थिर;

    UnicodeString&
    normalizeSecondAndAppend(UnicodeString& first U_LIFETIME_BOUND,
                             स्थिर UnicodeString& second,
                             UBool doNormalize,
                             UErrorCode& errorCode) स्थिर;

    स्थिर Normalizer2 &norm2;
    स्थिर UnicodeSet &set;
};

U_NAMESPACE_END

#पूर्ण_अगर  // !UCONFIG_NO_NORMALIZATION

#पूर्ण_अगर /* U_SHOW_CPLUSPLUS_API */

#पूर्ण_अगर  // __NORMALIZER2_H__
