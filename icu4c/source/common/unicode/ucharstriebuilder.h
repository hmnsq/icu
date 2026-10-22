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

#অন্তর্ভাব "unicode/utypes.h"

#যদি U_SHOW_CPLUSPLUS_API

#অন্তর্ভাব "unicode/stringtriebuilder.h"
#অন্তর্ভাব "unicode/ucharstrie.h"
#অন্তর্ভাব "unicode/unistr.h"

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
class U_COMMON_API UCharsTrieBuilder : সর্বজনীন StringTrieBuilder {
সর্বজনীন:
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
    আভাসী ~UCharsTrieBuilder();

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
    UCharsTrieBuilder& add(ধ্রুবক UnicodeString& s,
                           int32_t মান_2,
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
                                      UnicodeString& ফলাফল U_LIFETIME_BOUND,
                                      UErrorCode& errorCode) U_LIFETIME_BOUND;

    /**
     * Removes all (string, value) pairs.
     * New data can then be add()ed and a new trie can be built.
     * @return *this
     * @stable ICU 4.8
     */
    UCharsTrieBuilder& clear() U_LIFETIME_BOUND {
        strings.remove();
        elements_দৈর্ঘ্য=0;
        uchars_দৈর্ঘ্য=0;
        ফেরত *this;
    }

ব্যক্তিগত:
    UCharsTrieBuilder(ধ্রুবক UCharsTrieBuilder &other) = delete;  // no copy constructor
    UCharsTrieBuilder &operator=(ধ্রুবক UCharsTrieBuilder &other) = delete;  // no assignment operator

    শূন্য buildUChars(UStringTrieBuildOption buildOption, UErrorCode &errorCode);

    আভাসী int32_t get_Element_String_দৈর্ঘ্য(int32_t i) ধ্রুবক override;
    আভাসী char16_t getElementUnit(int32_t i, int32_t unit_সূচক) ধ্রুবক override;
    আভাসী int32_t get_Element_মান(int32_t i) ধ্রুবক override;

    আভাসী int32_t getLimitOfLinearMatch(int32_t first, int32_t last, int32_t unit_সূচক) ধ্রুবক override;

    আভাসী int32_t গুনতি_Element_Units(int32_t start, int32_t limit, int32_t unit_সূচক) ধ্রুবক override;
    আভাসী int32_t skipElementsBySomeUnits(int32_t i, int32_t unit_সূচক, int32_t গুনতি) ধ্রুবক override;
    আভাসী int32_t সূচক_Of_Element_With_পরবর্তী_Unit(int32_t i, int32_t unit_সূচক, char16_t unit) ধ্রুবক override;

    আভাসী UBool match_Nodes_Can_Have_মানসমূহ() ধ্রুবক override { ফেরত true; }

    আভাসী int32_t get_Max_Branch_Linear_Sub_গ্রন্থি_দৈর্ঘ্য() ধ্রুবক override { ফেরত UCharsTrie::k_Max_Branch_Linear_Sub_গ্রন্থি_দৈর্ঘ্য; }
    আভাসী int32_t getMinLinearMatch() ধ্রুবক override { ফেরত UCharsTrie::kMinLinearMatch; }
    আভাসী int32_t get_Max_Linear_Match_দৈর্ঘ্য() ধ্রুবক override { ফেরত UCharsTrie::k_Max_Linear_Match_দৈর্ঘ্য; }

    class UCTLinear_Match_গ্রন্থি : সর্বজনীন Linear_Match_গ্রন্থি {
    সর্বজনীন:
        UCTLinear_Match_গ্রন্থি(ধ্রুবক char16_t *units, int32_t len, গ্রন্থি_2 *পরবর্তী_গ্রন্থি);
        আভাসী bool operator==(ধ্রুবক গ্রন্থি_2 &other) ধ্রুবক override;
        আভাসী শূন্য লিখ(StringTrieBuilder &builder) override;
    ব্যক্তিগত:
        ধ্রুবক char16_t *s;
    };

    আভাসী গ্রন্থি_2 *create_Linear_Match_গ্রন্থি(int32_t i, int32_t unit_সূচক, int32_t দৈর্ঘ্য,
                                        গ্রন্থি_2 *পরবর্তী_গ্রন্থি) ধ্রুবক override;

    UBool ensureCapacity(int32_t দৈর্ঘ্য);
    আভাসী int32_t লিখ(int32_t unit) override;
    int32_t লিখ(ধ্রুবক char16_t *s, int32_t দৈর্ঘ্য);
    আভাসী int32_t writeElementUnits(int32_t i, int32_t unit_সূচক, int32_t দৈর্ঘ্য) override;
    আভাসী int32_t write_মান_And_Final(int32_t i, UBool isFinal) override;
    আভাসী int32_t write_মান_And_Type(UBool has_মান, int32_t মান_2, int32_t গ্রন্থি) override;
    আভাসী int32_t writeDeltaTo(int32_t jumpTarget) override;

    UnicodeString strings;
    UCharsTrieElement *elements;
    int32_t elementsCapacity;
    int32_t elements_দৈর্ঘ্য;

    // char16_t serialization of the trie.
    // Grows from the back: ucharsLength measures from the end of the buffer!
    char16_t *uchars;
    int32_t ucharsCapacity;
    int32_t uchars_দৈর্ঘ্য;
};

U_NAMESPACE_END

#endif /* U_SHOW_CPLUSPLUS_API */

#endif  // __UCHARSTRIEBUILDER_H__
