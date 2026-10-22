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

#جے_بنا_تعریف __UCHARSTRIEBUILDER_H__
#تعریف_کرو __UCHARSTRIEBUILDER_H__

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
    UCharsTrieBuilder(UError_کوڈ &error_کوڈ);

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
    UCharsTrieBuilder& جوڑو(ثابت UnicodeString& s,
                           int32_t قدر_2,
                           UError_کوڈ& error_کوڈ) U_LIFETIME_BOUND;

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
    UCharsTrie *build(UStringTrieBuildOption buildOption, UError_کوڈ &error_کوڈ);

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
                                      UnicodeString& نتیجہ U_LIFETIME_BOUND,
                                      UError_کوڈ& error_کوڈ) U_LIFETIME_BOUND;

    /**
     * Removes all (string, value) pairs.
     * New data can then be add()ed and a new trie can be built.
     * @return *this
     * @stable ICU 4.8
     */
    UCharsTrieBuilder& clear() U_LIFETIME_BOUND {
        لڑیاں.remove();
        elements_لمائی=0;
        uchars_لمائی=0;
        أرجع *this;
    }

خاص:
    UCharsTrieBuilder(ثابت UCharsTrieBuilder &other) = delete;  // no copy constructor
    UCharsTrieBuilder &عامل=(ثابت UCharsTrieBuilder &other) = delete;  // no assignment operator

    فراغ buildUChars(UStringTrieBuildOption buildOption, UError_کوڈ &error_کوڈ);

    مجازی int32_t get_Element_String_لمائی(int32_t i) ثابت override;
    مجازی char16_t getElementUnit(int32_t i, int32_t unit_اشاریہ) ثابت override;
    مجازی int32_t get_Element_قدر(int32_t i) ثابت override;

    مجازی int32_t get_حد_بندی_دا_خطی_میل(int32_t پہلا, int32_t آخری, int32_t unit_اشاریہ) ثابت override;

    مجازی int32_t گنتی_Element_Units(int32_t شروع, int32_t حد_بندی, int32_t unit_اشاریہ) ثابت override;
    مجازی int32_t skipElementsBySomeUnits(int32_t i, int32_t unit_اشاریہ, int32_t گنتی) ثابت override;
    مجازی int32_t اشاریہ_دا_Element_With_اگلا_Unit(int32_t i, int32_t unit_اشاریہ, char16_t unit) ثابت override;

    مجازی UBool میل_Nodes_Can_Have_اقدار() ثابت override { أرجع true; }

    مجازی int32_t get_Max_Branch_خطی_Sub_گرہ_لمائی() ثابت override { أرجع UCharsTrie::k_Max_Branch_خطی_Sub_گرہ_لمائی; }
    مجازی int32_t get_Min_خطی_میل() ثابت override { أرجع UCharsTrie::k_Min_خطی_میل; }
    مجازی int32_t get_Max_خطی_میل_لمائی() ثابت override { أرجع UCharsTrie::k_Max_خطی_میل_لمائی; }

    class UCTLinear_میل_گرہ : عام خطی_میل_گرہ {
    عام:
        UCTLinear_میل_گرہ(ثابت char16_t *units, int32_t len, گرہ_2 *اگلا_گرہ);
        مجازی bool عامل==(ثابت گرہ_2 &other) ثابت override;
        مجازی فراغ اكتب(StringTrieBuilder &builder) override;
    خاص:
        ثابت char16_t *s;
    };

    مجازی گرہ_2 *create_خطی_میل_گرہ(int32_t i, int32_t unit_اشاریہ, int32_t لمائی,
                                        گرہ_2 *اگلا_گرہ) ثابت override;

    UBool ensure_سمرتھا(int32_t لمائی);
    مجازی int32_t اكتب(int32_t unit) override;
    int32_t اكتب(ثابت char16_t *s, int32_t لمائی);
    مجازی int32_t writeElementUnits(int32_t i, int32_t unit_اشاریہ, int32_t لمائی) override;
    مجازی int32_t write_قدر_And_Final(int32_t i, UBool isFinal) override;
    مجازی int32_t write_قدر_And_Type(UBool has_قدر, int32_t قدر_2, int32_t گرہ) override;
    مجازی int32_t write_Delta_منزل(int32_t jump_نشانہ) override;

    UnicodeString لڑیاں;
    UCharsTrieElement *elements;
    int32_t elements_سمرتھا;
    int32_t elements_لمائی;

    // char16_t serialization of the trie.
    // Grows from the back: ucharsLength measures from the end of the buffer!
    char16_t *uchars;
    int32_t uchars_سمرتھا;
    int32_t uchars_لمائی;
};

U_NAMESPACE_سرا

#جے_انت /* U_SHOW_CPLUSPLUS_API */

#جے_انت  // __UCHARSTRIEBUILDER_H__
