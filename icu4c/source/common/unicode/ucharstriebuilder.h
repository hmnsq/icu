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

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ __UCHARSTRIEBUILDER_H__
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ __UCHARSTRIEBUILDER_H__

#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/utypes.h"

#ਜੇ U_SHOW_CPLUSPLUS_API

#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/stringtriebuilder.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/ucharstrie.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/unistr.h"

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
class U_COMMON_API UCharsTrieBuilder : ਜਨਤਕ StringTrieBuilder {
ਜਨਤਕ:
    /**
     * Constructs an empty builder.
     * @param errorCode Standard ICU error code.
     * @stable ICU 4.8
     */
    UCharsTrieBuilder(UError_ਕੋਡ &error_ਕੋਡ);

    /**
     * Destructor.
     * @stable ICU 4.8
     */
    ਆਭਾਸੀ ~UCharsTrieBuilder();

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
    UCharsTrieBuilder& ਜੋੜੋ(ਅਚਲ UnicodeString& s,
                           int32_t ਮੁੱਲ_2,
                           UError_ਕੋਡ& error_ਕੋਡ) U_LIFETIME_BOUND;

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
    UCharsTrie *build(UStringTrieBuildOption buildOption, UError_ਕੋਡ &error_ਕੋਡ);

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
                                      UnicodeString& ਨਤੀਜਾ U_LIFETIME_BOUND,
                                      UError_ਕੋਡ& error_ਕੋਡ) U_LIFETIME_BOUND;

    /**
     * Removes all (string, value) pairs.
     * New data can then be add()ed and a new trie can be built.
     * @return *this
     * @stable ICU 4.8
     */
    UCharsTrieBuilder& clear() U_LIFETIME_BOUND {
        ਲੜੀਆਂ.remove();
        elements_ਲੰਮਾਈ=0;
        uchars_ਲੰਮਾਈ=0;
        ਵਾਪਸ *this;
    }

ਨਿਜੀ:
    UCharsTrieBuilder(ਅਚਲ UCharsTrieBuilder &other) = delete;  // no copy constructor
    UCharsTrieBuilder &ਸੰਚਾਲਕ=(ਅਚਲ UCharsTrieBuilder &other) = delete;  // no assignment operator

    ਖਾਲੀ buildUChars(UStringTrieBuildOption buildOption, UError_ਕੋਡ &error_ਕੋਡ);

    ਆਭਾਸੀ int32_t get_Element_String_ਲੰਮਾਈ(int32_t i) ਅਚਲ override;
    ਆਭਾਸੀ char16_t getElementUnit(int32_t i, int32_t unit_ਸੂਚਕਾਂਕ) ਅਚਲ override;
    ਆਭਾਸੀ int32_t get_Element_ਮੁੱਲ(int32_t i) ਅਚਲ override;

    ਆਭਾਸੀ int32_t get_ਹੱਦਬੰਦੀ_ਦਾ_ਰੇਖਿਕ_ਮੇਲ(int32_t ਪਹਿਲਾ, int32_t ਆਖਰੀ, int32_t unit_ਸੂਚਕਾਂਕ) ਅਚਲ override;

    ਆਭਾਸੀ int32_t ਗਿਣਤੀ_Element_Units(int32_t ਸ਼ੁਰੂ, int32_t ਹੱਦਬੰਦੀ, int32_t unit_ਸੂਚਕਾਂਕ) ਅਚਲ override;
    ਆਭਾਸੀ int32_t skipElementsBySomeUnits(int32_t i, int32_t unit_ਸੂਚਕਾਂਕ, int32_t ਗਿਣਤੀ_2) ਅਚਲ override;
    ਆਭਾਸੀ int32_t ਸੂਚਕਾਂਕ_ਦਾ_Element_With_ਅਗਲਾ_Unit(int32_t i, int32_t unit_ਸੂਚਕਾਂਕ, char16_t unit) ਅਚਲ override;

    ਆਭਾਸੀ UBool ਮੇਲ_Nodes_Can_Have_ਮੁੱਲਾਂ() ਅਚਲ override { ਵਾਪਸ true; }

    ਆਭਾਸੀ int32_t get_Max_Branch_ਰੇਖਿਕ_Sub_ਗੰਢ_ਲੰਮਾਈ() ਅਚਲ override { ਵਾਪਸ UCharsTrie::k_Max_Branch_ਰੇਖਿਕ_Sub_ਗੰਢ_ਲੰਮਾਈ; }
    ਆਭਾਸੀ int32_t get_Min_ਰੇਖਿਕ_ਮੇਲ() ਅਚਲ override { ਵਾਪਸ UCharsTrie::k_Min_ਰੇਖਿਕ_ਮੇਲ; }
    ਆਭਾਸੀ int32_t get_Max_ਰੇਖਿਕ_ਮੇਲ_ਲੰਮਾਈ() ਅਚਲ override { ਵਾਪਸ UCharsTrie::k_Max_ਰੇਖਿਕ_ਮੇਲ_ਲੰਮਾਈ; }

    class UCTLinear_ਮੇਲ_ਗੰਢ : ਜਨਤਕ ਰੇਖਿਕ_ਮੇਲ_ਗੰਢ {
    ਜਨਤਕ:
        UCTLinear_ਮੇਲ_ਗੰਢ(ਅਚਲ char16_t *units, int32_t len, ਗੰਢ_2 *ਅਗਲਾ_ਗੰਢ);
        ਆਭਾਸੀ bool ਸੰਚਾਲਕ==(ਅਚਲ ਗੰਢ_2 &other) ਅਚਲ override;
        ਆਭਾਸੀ ਖਾਲੀ ਲਿਖ(StringTrieBuilder &builder) override;
    ਨਿਜੀ:
        ਅਚਲ char16_t *s;
    };

    ਆਭਾਸੀ ਗੰਢ_2 *create_ਰੇਖਿਕ_ਮੇਲ_ਗੰਢ(int32_t i, int32_t unit_ਸੂਚਕਾਂਕ, int32_t ਲੰਮਾਈ,
                                        ਗੰਢ_2 *ਅਗਲਾ_ਗੰਢ) ਅਚਲ override;

    UBool ensure_ਸਮਰੱਥਾ(int32_t ਲੰਮਾਈ);
    ਆਭਾਸੀ int32_t ਲਿਖ(int32_t unit) override;
    int32_t ਲਿਖ(ਅਚਲ char16_t *s, int32_t ਲੰਮਾਈ);
    ਆਭਾਸੀ int32_t writeElementUnits(int32_t i, int32_t unit_ਸੂਚਕਾਂਕ, int32_t ਲੰਮਾਈ) override;
    ਆਭਾਸੀ int32_t write_ਮੁੱਲ_And_Final(int32_t i, UBool isFinal) override;
    ਆਭਾਸੀ int32_t write_ਮੁੱਲ_And_Type(UBool has_ਮੁੱਲ, int32_t ਮੁੱਲ_2, int32_t ਗੰਢ) override;
    ਆਭਾਸੀ int32_t write_Delta_ਮੰਜ਼ਿਲ(int32_t jump_ਨਿਸ਼ਾਨਾ) override;

    UnicodeString ਲੜੀਆਂ;
    UCharsTrieElement *elements;
    int32_t elements_ਸਮਰੱਥਾ;
    int32_t elements_ਲੰਮਾਈ;

    // char16_t serialization of the trie.
    // Grows from the back: ucharsLength measures from the end of the buffer!
    char16_t *uchars;
    int32_t uchars_ਸਮਰੱਥਾ;
    int32_t uchars_ਲੰਮਾਈ;
};

U_NAMESPACE_ਸਿਰਾ

#ਜੇ_ਅੰਤ /* U_SHOW_CPLUSPLUS_API */

#ਜੇ_ਅੰਤ  // __UCHARSTRIEBUILDER_H__
