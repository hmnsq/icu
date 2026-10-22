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

#اگر_غیر_تعریف_شدہ __UCHARSTRIEBUILDER_H__
#تعریف_کرو __UCHARSTRIEBUILDER_H__

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
class U_COMMON_API UCharsTrieBuilder : عوامی StringTrieBuilder {
عوامی:
    /**
     * Constructs an empty builder.
     * @param errorCode Standard ICU error code.
     * @stable ICU 4.8
     */
    UCharsTrieBuilder(UError_رمز &error_رمز);

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
                           UError_رمز& error_رمز) U_LIFETIME_BOUND;

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
    UCharsTrie *build(UStringTrieBuildOption buildOption, UError_رمز &error_رمز);

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
                                      UError_رمز& error_رمز) U_LIFETIME_BOUND;

    /**
     * Removes all (string, value) pairs.
     * New data can then be add()ed and a new trie can be built.
     * @return *this
     * @stable ICU 4.8
     */
    UCharsTrieBuilder& clear() U_LIFETIME_BOUND {
        لڑیاں.remove();
        elements_لمبائی=0;
        uchars_لمبائی=0;
        واپس *this;
    }

نجی:
    UCharsTrieBuilder(ثابت UCharsTrieBuilder &other) = delete;  // no copy constructor
    UCharsTrieBuilder &عامل=(ثابت UCharsTrieBuilder &other) = delete;  // no assignment operator

    خالی buildUChars(UStringTrieBuildOption buildOption, UError_رمز &error_رمز);

    مجازی int32_t get_Element_String_لمبائی(int32_t i) ثابت override;
    مجازی char16_t getElementUnit(int32_t i, int32_t unit_اشاریہ) ثابت override;
    مجازی int32_t get_Element_قدر(int32_t i) ثابت override;

    مجازی int32_t get_حد_بندی_کا_خطی_مطابقت(int32_t پہلا, int32_t آخری, int32_t unit_اشاریہ) ثابت override;

    مجازی int32_t گنتی_Element_Units(int32_t آغاز, int32_t حد_بندی, int32_t unit_اشاریہ) ثابت override;
    مجازی int32_t skipElementsBySomeUnits(int32_t i, int32_t unit_اشاریہ, int32_t گنتی) ثابت override;
    مجازی int32_t اشاریہ_کا_Element_With_اگلا_Unit(int32_t i, int32_t unit_اشاریہ, char16_t unit) ثابت override;

    مجازی UBool مطابقت_Nodes_Can_Have_اقدار() ثابت override { واپس true; }

    مجازی int32_t get_Max_Branch_خطی_Sub_گرہ_لمبائی() ثابت override { واپس UCharsTrie::k_Max_Branch_خطی_Sub_گرہ_لمبائی; }
    مجازی int32_t get_Min_خطی_مطابقت() ثابت override { واپس UCharsTrie::k_Min_خطی_مطابقت; }
    مجازی int32_t get_Max_خطی_مطابقت_لمبائی() ثابت override { واپس UCharsTrie::k_Max_خطی_مطابقت_لمبائی; }

    class UCTLinear_مطابقت_گرہ : عوامی خطی_مطابقت_گرہ {
    عوامی:
        UCTLinear_مطابقت_گرہ(ثابت char16_t *units, int32_t len, گرہ_2 *اگلا_گرہ);
        مجازی bool عامل==(ثابت گرہ_2 &other) ثابت override;
        مجازی خالی اكتب(StringTrieBuilder &builder) override;
    نجی:
        ثابت char16_t *s;
    };

    مجازی گرہ_2 *create_خطی_مطابقت_گرہ(int32_t i, int32_t unit_اشاریہ, int32_t لمبائی,
                                        گرہ_2 *اگلا_گرہ) ثابت override;

    UBool ensure_استعداد(int32_t لمبائی);
    مجازی int32_t اكتب(int32_t unit) override;
    int32_t اكتب(ثابت char16_t *s, int32_t لمبائی);
    مجازی int32_t writeElementUnits(int32_t i, int32_t unit_اشاریہ, int32_t لمبائی) override;
    مجازی int32_t write_قدر_And_Final(int32_t i, UBool isFinal) override;
    مجازی int32_t write_قدر_And_Type(UBool has_قدر, int32_t قدر_2, int32_t گرہ) override;
    مجازی int32_t write_Delta_مقصد(int32_t jump_ہدف) override;

    UnicodeString لڑیاں;
    UCharsTrieElement *elements;
    int32_t elements_استعداد;
    int32_t elements_لمبائی;

    // char16_t serialization of the trie.
    // Grows from the back: ucharsLength measures from the end of the buffer!
    char16_t *uchars;
    int32_t uchars_استعداد;
    int32_t uchars_لمبائی;
};

U_NAMESPACE_انتہا

#اگر_ختم /* U_SHOW_CPLUSPLUS_API */

#اگر_ختم  // __UCHARSTRIEBUILDER_H__
