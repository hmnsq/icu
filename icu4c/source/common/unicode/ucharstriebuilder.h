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

#که_نه_تعریف_شوی __UCHARSTRIEBUILDER_H__
#تعریف_کړه __UCHARSTRIEBUILDER_H__

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
    UCharsTrieBuilder(UError_کوډ &error_کوډ);

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
    UCharsTrieBuilder& جمع_کړه(ثابت UnicodeString& s,
                           int32_t ارزښت_2,
                           UError_کوډ& error_کوډ) U_LIFETIME_BOUND;

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
    UCharsTrie *build(UStringTrieBuildOption buildOption, UError_کوډ &error_کوډ);

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
                                      UnicodeString& پایله U_LIFETIME_BOUND,
                                      UError_کوډ& error_کوډ) U_LIFETIME_BOUND;

    /**
     * Removes all (string, value) pairs.
     * New data can then be add()ed and a new trie can be built.
     * @return *this
     * @stable ICU 4.8
     */
    UCharsTrieBuilder& clear() U_LIFETIME_BOUND {
        مزي_ګانې.remove();
        elements_اوږدتیا=0;
        uchars_اوږدتیا=0;
        أرجع *this;
    }

خاص:
    UCharsTrieBuilder(ثابت UCharsTrieBuilder &other) = delete;  // no copy constructor
    UCharsTrieBuilder &چلوونکی=(ثابت UCharsTrieBuilder &other) = delete;  // no assignment operator

    فراغ buildUChars(UStringTrieBuildOption buildOption, UError_کوډ &error_کوډ);

    مجازي int32_t get_Element_String_اوږدتیا(int32_t i) ثابت override;
    مجازي char16_t getElementUnit(int32_t i, int32_t unit_شاخص) ثابت override;
    مجازي int32_t get_Element_ارزښت(int32_t i) ثابت override;

    مجازي int32_t get_حد_د_خطي_سمون(int32_t لومړی, int32_t وروستی, int32_t unit_شاخص) ثابت override;

    مجازي int32_t شمېر_Element_Units(int32_t پیل, int32_t حد, int32_t unit_شاخص) ثابت override;
    مجازي int32_t skipElementsBySomeUnits(int32_t i, int32_t unit_شاخص, int32_t شمېر) ثابت override;
    مجازي int32_t شاخص_د_Element_With_بل_Unit(int32_t i, int32_t unit_شاخص, char16_t unit) ثابت override;

    مجازي UBool سمون_Nodes_Can_Have_ارزښتونه() ثابت override { أرجع true; }

    مجازي int32_t get_Max_Branch_خطي_Sub_غوټه_اوږدتیا() ثابت override { أرجع UCharsTrie::k_Max_Branch_خطي_Sub_غوټه_اوږدتیا; }
    مجازي int32_t get_Min_خطي_سمون() ثابت override { أرجع UCharsTrie::k_Min_خطي_سمون; }
    مجازي int32_t get_Max_خطي_سمون_اوږدتیا() ثابت override { أرجع UCharsTrie::k_Max_خطي_سمون_اوږدتیا; }

    class UCTLinear_سمون_غوټه : عام خطي_سمون_غوټه {
    عام:
        UCTLinear_سمون_غوټه(ثابت char16_t *units, int32_t len, غوټه_2 *بل_غوټه);
        مجازي bool چلوونکی==(ثابت غوټه_2 &other) ثابت override;
        مجازي فراغ اكتب(StringTrieBuilder &builder) override;
    خاص:
        ثابت char16_t *s;
    };

    مجازي غوټه_2 *create_خطي_سمون_غوټه(int32_t i, int32_t unit_شاخص, int32_t اوږدتیا,
                                        غوټه_2 *بل_غوټه) ثابت override;

    UBool ensure_ظرفیت(int32_t اوږدتیا);
    مجازي int32_t اكتب(int32_t unit) override;
    int32_t اكتب(ثابت char16_t *s, int32_t اوږدتیا);
    مجازي int32_t writeElementUnits(int32_t i, int32_t unit_شاخص, int32_t اوږدتیا) override;
    مجازي int32_t write_ارزښت_And_Final(int32_t i, UBool isFinal) override;
    مجازي int32_t write_ارزښت_And_Type(UBool has_ارزښت, int32_t ارزښت_2, int32_t غوټه) override;
    مجازي int32_t write_Delta_منزل(int32_t jump_موخه) override;

    UnicodeString مزي_ګانې;
    UCharsTrieElement *elements;
    int32_t elements_ظرفیت;
    int32_t elements_اوږدتیا;

    // char16_t serialization of the trie.
    // Grows from the back: ucharsLength measures from the end of the buffer!
    char16_t *uchars;
    int32_t uchars_ظرفیت;
    int32_t uchars_اوږدتیا;
};

U_NAMESPACE_پای_ټکی

#د_که_پای /* U_SHOW_CPLUSPLUS_API */

#د_که_پای  // __UCHARSTRIEBUILDER_H__
