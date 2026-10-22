// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
*******************************************************************************
*   Copyright (C) 2010-2016, International Business Machines
*   Corporation and others.  All Rights Reserved.
*******************************************************************************
*   file name:  ucharstriebuilder.h
*   encoding:   UTF-8
*   tab size:   8 (not used)
*   indentation:4
*
*   created on: 2010nov14
*   created by: Markus W. Scherer
*/

#ifndef __UCHARSTRIEBUILDER_H__
#define __UCHARSTRIEBUILDER_H__

#شامل "unicode/utypes.h"

#اگر U_SHOW_CPLUSPLUS_API

#شامل "unicode/stringtriebuilder.h"
#شامل "unicode/ucharstrie.h"
#شامل "unicode/unistr.h"

/**
 * \file
 * \brief C++ API: Builder for icu::UCharsTrie
 */

U_NAMESPACE_BEGIN

class UCharsTrieElement;

/**
 * Builder class for UCharsTrie.
 *
 * This class is not intended for public subclassing.
 * @stable ICU 4.8
 */
class U_COMMON_API UCharsTrieBuilder : عمومی StringTrieBuilder {
عمومی:
    /**
     * Constructs an empty builder.
     * @param errorCode Standard ICU error code.
     * @stable ICU 4.8
     */
    UCharsTrieBuilder(UErrorCode &errorCode);

    /**
     * Destructor.
     * @stable ICU 4.8
     */
    مجازی ~UCharsTrieBuilder();

    /**
     * Adds a (string, value) pair.
     * The string must be unique.
     * The string contents will be copied; the builder does not keep
     * a reference to the input UnicodeString or its buffer.
     * @param s The input string.
     * @param value The value associated with this string.
     * @param errorCode Standard ICU error code. Its input value must
     *                  pass the U_SUCCESS() test, or else the function returns
     *                  immediately. Check for U_FAILURE() on output or use with
     *                  function chaining. (See User Guide for details.)
     * @return *this
     * @stable ICU 4.8
     */
    UCharsTrieBuilder& add(ثابت UnicodeString& s,
                           int32_t مقدار_2,
                           UErrorCode& errorCode) U_LIFETIME_BOUND;

    /**
     * Builds a UCharsTrie for the add()ed data.
     * Once built, no further data can be add()ed until clear() is called.
     *
     * A UCharsTrie cannot be empty. At least one (string, value) pair
     * must have been add()ed.
     *
     * This method passes ownership of the builder's internal result array to the new trie object.
     * Another call to any build() variant will re-serialize the trie.
     * After clear() has been called, a new array will be used as well.
     * @param buildOption Build option, see UStringTrieBuildOption.
     * @param errorCode Standard ICU error code. Its input value must
     *                  pass the U_SUCCESS() test, or else the function returns
     *                  immediately. Check for U_FAILURE() on output or use with
     *                  function chaining. (See User Guide for details.)
     * @return A new UCharsTrie for the add()ed data.
     * @stable ICU 4.8
     */
    UCharsTrie *build(UStringTrieBuildOption buildOption, UErrorCode &errorCode);

    /**
     * Builds a UCharsTrie for the add()ed data and char16_t-serializes it.
     * Once built, no further data can be add()ed until clear() is called.
     *
     * A UCharsTrie cannot be empty. At least one (string, value) pair
     * must have been add()ed.
     *
     * Multiple calls to buildUnicodeString() set the UnicodeStrings to the
     * builder's same char16_t array, without rebuilding.
     * If buildUnicodeString() is called after build(), the trie will be
     * re-serialized into a new array (because build() passes on ownership).
     * If build() is called after buildUnicodeString(), the trie object returned
     * by build() will become the owner of the underlying data for the
     * previously returned UnicodeString.
     * After clear() has been called, a new array will be used as well.
     * @param buildOption Build option, see UStringTrieBuildOption.
     * @param result A UnicodeString which will be set to the char16_t-serialized
     *               UCharsTrie for the add()ed data.
     * @param errorCode Standard ICU error code. Its input value must
     *                  pass the U_SUCCESS() test, or else the function returns
     *                  immediately. Check for U_FAILURE() on output or use with
     *                  function chaining. (See User Guide for details.)
     * @return result
     * @stable ICU 4.8
     */
    UnicodeString& buildUnicodeString(UStringTrieBuildOption buildOption,
                                      UnicodeString& result U_LIFETIME_BOUND,
                                      UErrorCode& errorCode) U_LIFETIME_BOUND;

    /**
     * Removes all (string, value) pairs.
     * New data can then be add()ed and a new trie can be built.
     * @return *this
     * @stable ICU 4.8
     */
    UCharsTrieBuilder& clear() U_LIFETIME_BOUND {
        strings.remove();
        elementsLength=0;
        ucharsLength=0;
        بازگشت *this;
    }

خصوصی:
    UCharsTrieBuilder(ثابت UCharsTrieBuilder &other) = delete;  // no copy constructor
    UCharsTrieBuilder &operator=(ثابت UCharsTrieBuilder &other) = delete;  // no assignment operator

    تهی buildUChars(UStringTrieBuildOption buildOption, UErrorCode &errorCode);

    مجازی int32_t getElementStringLength(int32_t i) ثابت override;
    مجازی char16_t getElementUnit(int32_t i, int32_t unitIndex) ثابت override;
    مجازی int32_t get_Element_مقدار(int32_t i) ثابت override;

    مجازی int32_t getLimitOfLinearMatch(int32_t first, int32_t last, int32_t unitIndex) ثابت override;

    مجازی int32_t شمار_Element_Units(int32_t start, int32_t limit, int32_t unitIndex) ثابت override;
    مجازی int32_t skipElementsBySomeUnits(int32_t i, int32_t unitIndex, int32_t شمار) ثابت override;
    مجازی int32_t indexOfElementWithNextUnit(int32_t i, int32_t unitIndex, char16_t unit) ثابت override;

    مجازی UBool match_Nodes_Can_Have_مقادیر() ثابت override { بازگشت true; }

    مجازی int32_t get_Max_Branch_Linear_Sub_گره_Length() ثابت override { بازگشت UCharsTrie::k_Max_Branch_Linear_Sub_گره_Length; }
    مجازی int32_t getMinLinearMatch() ثابت override { بازگشت UCharsTrie::kMinLinearMatch; }
    مجازی int32_t getMaxLinearMatchLength() ثابت override { بازگشت UCharsTrie::kMaxLinearMatchLength; }

    class UCTLinear_Match_گره : عمومی Linear_Match_گره {
    عمومی:
        UCTLinear_Match_گره(ثابت char16_t *units, int32_t len, گره_2 *next_گره);
        مجازی bool operator==(ثابت گره_2 &other) ثابت override;
        مجازی تهی اكتب(StringTrieBuilder &builder) override;
    خصوصی:
        ثابت char16_t *s;
    };

    مجازی گره_2 *create_Linear_Match_گره(int32_t i, int32_t unitIndex, int32_t length,
                                        گره_2 *next_گره) ثابت override;

    UBool ensureCapacity(int32_t length);
    مجازی int32_t اكتب(int32_t unit) override;
    int32_t اكتب(ثابت char16_t *s, int32_t length);
    مجازی int32_t writeElementUnits(int32_t i, int32_t unitIndex, int32_t length) override;
    مجازی int32_t write_مقدار_And_Final(int32_t i, UBool isFinal) override;
    مجازی int32_t write_مقدار_And_Type(UBool has_مقدار, int32_t مقدار_2, int32_t گره) override;
    مجازی int32_t writeDeltaTo(int32_t jumpTarget) override;

    UnicodeString strings;
    UCharsTrieElement *elements;
    int32_t elementsCapacity;
    int32_t elementsLength;

    // char16_t serialization of the trie.
    // Grows from the back: ucharsLength measures from the end of the buffer!
    char16_t *uchars;
    int32_t ucharsCapacity;
    int32_t ucharsLength;
};

U_NAMESPACE_END

#endif /* U_SHOW_CPLUSPLUS_API */

#endif  // __UCHARSTRIEBUILDER_H__
