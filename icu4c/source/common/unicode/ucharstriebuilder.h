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

#جيڪڏهن_وصف_نه_ٿيل __UCHARSTRIEBUILDER_H__
#وصف_ڪريو __UCHARSTRIEBUILDER_H__

#ضمن "unicode/utypes.h"

#إذا U_SHOW_CPLUSPLUS_API

#ضمن "unicode/stringtriebuilder.h"
#ضمن "unicode/ucharstrie.h"
#ضمن "unicode/unistr.h"

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
class U_COMMON_API UCharsTrieBuilder : عام StringTrieBuilder {
عام:
    /**
     * Constructs an empty builder.
     * @param errorCode Standard ICU error code.
     * @stable ICU 4.8
     */
    UCharsTrieBuilder(UError_ڪوڊ &error_ڪوڊ);

    /**
     * Destructor.
     * @stable ICU 4.8
     */
    مجازي ~UCharsTrieBuilder();

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
    UCharsTrieBuilder& جوڙيو(ثابت UnicodeString& s,
                           int32_t قدر_2,
                           UError_ڪوڊ& error_ڪوڊ) U_LIFETIME_BOUND;

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
    UCharsTrie *build(UStringTrieBuildOption buildOption, UError_ڪوڊ &error_ڪوڊ);

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
                                      UnicodeString& نتيجو U_LIFETIME_BOUND,
                                      UError_ڪوڊ& error_ڪوڊ) U_LIFETIME_BOUND;

    /**
     * Removes all (string, value) pairs.
     * New data can then be add()ed and a new trie can be built.
     * @return *this
     * @stable ICU 4.8
     */
    UCharsTrieBuilder& clear() U_LIFETIME_BOUND {
        لڙيون.remove();
        elements_ڊيگهائي=0;
        uchars_ڊيگهائي=0;
        أرجع *this;
    }

خاص:
    UCharsTrieBuilder(ثابت UCharsTrieBuilder &other) = delete;  // no copy constructor
    UCharsTrieBuilder &عامل=(ثابت UCharsTrieBuilder &other) = delete;  // no assignment operator

    فراغ buildUChars(UStringTrieBuildOption buildOption, UError_ڪوڊ &error_ڪوڊ);

    مجازي int32_t get_Element_String_ڊيگهائي(int32_t i) ثابت override;
    مجازي char16_t getElementUnit(int32_t i, int32_t unit_اشاريو) ثابت override;
    مجازي int32_t get_Element_قدر(int32_t i) ثابت override;

    مجازي int32_t get_حد_بندي_جو_لڪيري_ميلاپ(int32_t پهريون, int32_t آخري, int32_t unit_اشاريو) ثابت override;

    مجازي int32_t ڳڻپ_Element_Units(int32_t شروعات, int32_t حد_بندي, int32_t unit_اشاريو) ثابت override;
    مجازي int32_t skipElementsBySomeUnits(int32_t i, int32_t unit_اشاريو, int32_t ڳڻپ) ثابت override;
    مجازي int32_t اشاريو_جو_Element_With_ايندڙ_Unit(int32_t i, int32_t unit_اشاريو, char16_t unit) ثابت override;

    مجازي UBool ميلاپ_Nodes_Can_Have_قدرون() ثابت override { أرجع true; }

    مجازي int32_t get_Max_Branch_لڪيري_Sub_ڳنڍ_ڊيگهائي() ثابت override { أرجع UCharsTrie::k_Max_Branch_لڪيري_Sub_ڳنڍ_ڊيگهائي; }
    مجازي int32_t get_Min_لڪيري_ميلاپ() ثابت override { أرجع UCharsTrie::k_Min_لڪيري_ميلاپ; }
    مجازي int32_t get_Max_لڪيري_ميلاپ_ڊيگهائي() ثابت override { أرجع UCharsTrie::k_Max_لڪيري_ميلاپ_ڊيگهائي; }

    class UCTLinear_ميلاپ_ڳنڍ : عام لڪيري_ميلاپ_ڳنڍ {
    عام:
        UCTLinear_ميلاپ_ڳنڍ(ثابت char16_t *units, int32_t len, ڳنڍ_2 *ايندڙ_ڳنڍ);
        مجازي bool عامل==(ثابت ڳنڍ_2 &other) ثابت override;
        مجازي فراغ اكتب(StringTrieBuilder &builder) override;
    خاص:
        ثابت char16_t *s;
    };

    مجازي ڳنڍ_2 *create_لڪيري_ميلاپ_ڳنڍ(int32_t i, int32_t unit_اشاريو, int32_t ڊيگهائي,
                                        ڳنڍ_2 *ايندڙ_ڳنڍ) ثابت override;

    UBool ensure_صلاحيت(int32_t ڊيگهائي);
    مجازي int32_t اكتب(int32_t unit) override;
    int32_t اكتب(ثابت char16_t *s, int32_t ڊيگهائي);
    مجازي int32_t writeElementUnits(int32_t i, int32_t unit_اشاريو, int32_t ڊيگهائي) override;
    مجازي int32_t write_قدر_And_Final(int32_t i, UBool isFinal) override;
    مجازي int32_t write_قدر_And_Type(UBool has_قدر, int32_t قدر_2, int32_t ڳنڍ) override;
    مجازي int32_t write_Delta_منزل(int32_t jump_هدف) override;

    UnicodeString لڙيون;
    UCharsTrieElement *elements;
    int32_t elements_صلاحيت;
    int32_t elements_ڊيگهائي;

    // char16_t serialization of the trie.
    // Grows from the back: ucharsLength measures from the end of the buffer!
    char16_t *uchars;
    int32_t uchars_صلاحيت;
    int32_t uchars_ڊيگهائي;
};

U_NAMESPACE_پڇاڙي

#جيڪڏهن_پورو /* U_SHOW_CPLUSPLUS_API */

#جيڪڏهن_پورو  // __UCHARSTRIEBUILDER_H__
