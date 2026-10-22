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

#اگر_تعریف_نشده __UCHARSTRIEBUILDER_H__
#تعریف_کن __UCHARSTRIEBUILDER_H__

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
    UCharsTrieBuilder(UError_کد &error_کد);

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
    UCharsTrieBuilder& جمع_کن(ثابت UnicodeString& s,
                           int32_t مقدار_2,
                           UError_کد& error_کد) U_LIFETIME_BOUND;

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
    UCharsTrie *build(UStringTrieBuildOption buildOption, UError_کد &error_کد);

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
                                      UnicodeString& نتیجه U_LIFETIME_BOUND,
                                      UError_کد& error_کد) U_LIFETIME_BOUND;

    /**
     * Removes all (string, value) pairs.
     * New data can then be add()ed and a new trie can be built.
     * @return *this
     * @stable ICU 4.8
     */
    UCharsTrieBuilder& clear() U_LIFETIME_BOUND {
        رشتهها.remove();
        elements_درازی=0;
        uchars_درازی=0;
        أرجع *this;
    }

خاص:
    UCharsTrieBuilder(ثابت UCharsTrieBuilder &other) = delete;  // no copy constructor
    UCharsTrieBuilder &عملگر=(ثابت UCharsTrieBuilder &other) = delete;  // no assignment operator

    فراغ buildUChars(UStringTrieBuildOption buildOption, UError_کد &error_کد);

    مجازی int32_t get_Element_String_درازی(int32_t i) ثابت override;
    مجازی char16_t getElementUnit(int32_t i, int32_t unit_شاخص) ثابت override;
    مجازی int32_t get_Element_مقدار(int32_t i) ثابت override;

    مجازی int32_t get_حد_از_خطی_تطابق(int32_t اولی, int32_t آخرین, int32_t unit_شاخص) ثابت override;

    مجازی int32_t شمار_Element_Units(int32_t شروع, int32_t حد, int32_t unit_شاخص) ثابت override;
    مجازی int32_t skipElementsBySomeUnits(int32_t i, int32_t unit_شاخص, int32_t شمار) ثابت override;
    مجازی int32_t شاخص_از_Element_With_بعدی_Unit(int32_t i, int32_t unit_شاخص, char16_t unit) ثابت override;

    مجازی UBool تطابق_Nodes_Can_Have_مقادیر() ثابت override { أرجع true; }

    مجازی int32_t get_Max_Branch_خطی_Sub_گره_درازی() ثابت override { أرجع UCharsTrie::k_Max_Branch_خطی_Sub_گره_درازی; }
    مجازی int32_t get_Min_خطی_تطابق() ثابت override { أرجع UCharsTrie::k_Min_خطی_تطابق; }
    مجازی int32_t get_Max_خطی_تطابق_درازی() ثابت override { أرجع UCharsTrie::k_Max_خطی_تطابق_درازی; }

    class UCTLinear_تطابق_گره : عام خطی_تطابق_گره {
    عام:
        UCTLinear_تطابق_گره(ثابت char16_t *units, int32_t len, گره_2 *بعدی_گره);
        مجازی bool عملگر==(ثابت گره_2 &other) ثابت override;
        مجازی فراغ اكتب(StringTrieBuilder &builder) override;
    خاص:
        ثابت char16_t *s;
    };

    مجازی گره_2 *create_خطی_تطابق_گره(int32_t i, int32_t unit_شاخص, int32_t درازی,
                                        گره_2 *بعدی_گره) ثابت override;

    UBool ensure_ظرفیت(int32_t درازی);
    مجازی int32_t اكتب(int32_t unit) override;
    int32_t اكتب(ثابت char16_t *s, int32_t درازی);
    مجازی int32_t writeElementUnits(int32_t i, int32_t unit_شاخص, int32_t درازی) override;
    مجازی int32_t write_مقدار_And_Final(int32_t i, UBool isFinal) override;
    مجازی int32_t write_مقدار_And_Type(UBool has_مقدار, int32_t مقدار_2, int32_t گره) override;
    مجازی int32_t write_Delta_مقصد(int32_t jump_هدف) override;

    UnicodeString رشتهها;
    UCharsTrieElement *elements;
    int32_t elements_ظرفیت;
    int32_t elements_درازی;

    // char16_t serialization of the trie.
    // Grows from the back: ucharsLength measures from the end of the buffer!
    char16_t *uchars;
    int32_t uchars_ظرفیت;
    int32_t uchars_درازی;
};

U_NAMESPACE_انتها

#پایان_اگر /* U_SHOW_CPLUSPLUS_API */

#پایان_اگر  // __UCHARSTRIEBUILDER_H__
