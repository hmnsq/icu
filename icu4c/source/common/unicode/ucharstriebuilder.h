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

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ __UCHARSTRIEBUILDER_H__
#ವ್ಯಾಖ್ಯಾನಿಸು __UCHARSTRIEBUILDER_H__

#ಅನ್ತರ್ಭಾವ "unicode/utypes.h"

#ಆದರೆ U_SHOW_CPLUSPLUS_API

#ಅನ್ತರ್ಭಾವ "unicode/stringtriebuilder.h"
#ಅನ್ತರ್ಭಾವ "unicode/ucharstrie.h"
#ಅನ್ತರ್ಭಾವ "unicode/unistr.h"

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
class U_COMMON_API UCharsTrieBuilder : ಸಾರ್ವಜನಿಕ StringTrieBuilder {
ಸಾರ್ವಜನಿಕ:
    /**
     * Constructs an empty builder.
     * @param errorCode Standard ICU error code.
     * @stable ICU 4.8
     */
    UCharsTrieBuilder(UError_ಸಂಕೇತ &error_ಸಂಕೇತ);

    /**
     * Destructor.
     * @stable ICU 4.8
     */
    ವಾಸ್ತವಿಕ ~UCharsTrieBuilder();

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
    UCharsTrieBuilder& ಕೂಡಿಸು(ಸ್ಥಿರಾಂಕ UnicodeString& s,
                           int32_t ಮೌಲ್ಯ_2,
                           UError_ಸಂಕೇತ& error_ಸಂಕೇತ) U_LIFETIME_BOUND;

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
    UCharsTrie *build(UStringTrieBuildOption buildOption, UError_ಸಂಕೇತ &error_ಸಂಕೇತ);

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
                                      UnicodeString& ಫಲಿತಾಂಶ U_LIFETIME_BOUND,
                                      UError_ಸಂಕೇತ& error_ಸಂಕೇತ) U_LIFETIME_BOUND;

    /**
     * Removes all (string, value) pairs.
     * New data can then be add()ed and a new trie can be built.
     * @return *this
     * @stable ICU 4.8
     */
    UCharsTrieBuilder& clear() U_LIFETIME_BOUND {
        ಅಕ್ಷರಸರಣಿಗಳು.remove();
        elements_ಉದ್ದಳತೆ=0;
        uchars_ಉದ್ದಳತೆ=0;
        ಹಿಂತಿರುಗಿ *this;
    }

ಖಾಸಗಿ:
    UCharsTrieBuilder(ಸ್ಥಿರಾಂಕ UCharsTrieBuilder &other) = delete;  // no copy constructor
    UCharsTrieBuilder &ಕ್ರಿಯಾಕಾರಕ=(ಸ್ಥಿರಾಂಕ UCharsTrieBuilder &other) = delete;  // no assignment operator

    ಶೂನ್ಯ buildUChars(UStringTrieBuildOption buildOption, UError_ಸಂಕೇತ &error_ಸಂಕೇತ);

    ವಾಸ್ತವಿಕ int32_t get_Element_String_ಉದ್ದಳತೆ(int32_t i) ಸ್ಥಿರಾಂಕ override;
    ವಾಸ್ತವಿಕ char16_t getElementUnit(int32_t i, int32_t unit_ಸೂಚ್ಯಂಕ) ಸ್ಥಿರಾಂಕ override;
    ವಾಸ್ತವಿಕ int32_t get_Element_ಮೌಲ್ಯ(int32_t i) ಸ್ಥಿರಾಂಕ override;

    ವಾಸ್ತವಿಕ int32_t get_ಎಲ್ಲೆ_ದ_ರೇಖೀಯ_ಹೊಂದಿಕೆ(int32_t ಮೊದಲ, int32_t ಕೊನೆಯ, int32_t unit_ಸೂಚ್ಯಂಕ) ಸ್ಥಿರಾಂಕ override;

    ವಾಸ್ತವಿಕ int32_t ಎಣಿಕೆ_Element_Units(int32_t ಆರಂಭ, int32_t ಎಲ್ಲೆ, int32_t unit_ಸೂಚ್ಯಂಕ) ಸ್ಥಿರಾಂಕ override;
    ವಾಸ್ತವಿಕ int32_t skipElementsBySomeUnits(int32_t i, int32_t unit_ಸೂಚ್ಯಂಕ, int32_t ಎಣಿಕೆ_2) ಸ್ಥಿರಾಂಕ override;
    ವಾಸ್ತವಿಕ int32_t ಸೂಚ್ಯಂಕ_ದ_Element_With_ಮುಂದಿನ_Unit(int32_t i, int32_t unit_ಸೂಚ್ಯಂಕ, char16_t unit) ಸ್ಥಿರಾಂಕ override;

    ವಾಸ್ತವಿಕ UBool ಹೊಂದಿಕೆ_Nodes_Can_Have_ಮೌಲ್ಯಗಳು() ಸ್ಥಿರಾಂಕ override { ಹಿಂತಿರುಗಿ true; }

    ವಾಸ್ತವಿಕ int32_t get_Max_Branch_ರೇಖೀಯ_Sub_ಗಂಟು_ಉದ್ದಳತೆ() ಸ್ಥಿರಾಂಕ override { ಹಿಂತಿರುಗಿ UCharsTrie::k_Max_Branch_ರೇಖೀಯ_Sub_ಗಂಟು_ಉದ್ದಳತೆ; }
    ವಾಸ್ತವಿಕ int32_t get_Min_ರೇಖೀಯ_ಹೊಂದಿಕೆ() ಸ್ಥಿರಾಂಕ override { ಹಿಂತಿರುಗಿ UCharsTrie::k_Min_ರೇಖೀಯ_ಹೊಂದಿಕೆ; }
    ವಾಸ್ತವಿಕ int32_t get_Max_ರೇಖೀಯ_ಹೊಂದಿಕೆ_ಉದ್ದಳತೆ() ಸ್ಥಿರಾಂಕ override { ಹಿಂತಿರುಗಿ UCharsTrie::k_Max_ರೇಖೀಯ_ಹೊಂದಿಕೆ_ಉದ್ದಳತೆ; }

    class UCTLinear_ಹೊಂದಿಕೆ_ಗಂಟು : ಸಾರ್ವಜನಿಕ ರೇಖೀಯ_ಹೊಂದಿಕೆ_ಗಂಟು {
    ಸಾರ್ವಜನಿಕ:
        UCTLinear_ಹೊಂದಿಕೆ_ಗಂಟು(ಸ್ಥಿರಾಂಕ char16_t *units, int32_t len, ಗಂಟು_2 *ಮುಂದಿನ_ಗಂಟು);
        ವಾಸ್ತವಿಕ bool ಕ್ರಿಯಾಕಾರಕ==(ಸ್ಥಿರಾಂಕ ಗಂಟು_2 &other) ಸ್ಥಿರಾಂಕ override;
        ವಾಸ್ತವಿಕ ಶೂನ್ಯ ಲಿಖ(StringTrieBuilder &builder) override;
    ಖಾಸಗಿ:
        ಸ್ಥಿರಾಂಕ char16_t *s;
    };

    ವಾಸ್ತವಿಕ ಗಂಟು_2 *create_ರೇಖೀಯ_ಹೊಂದಿಕೆ_ಗಂಟು(int32_t i, int32_t unit_ಸೂಚ್ಯಂಕ, int32_t ಉದ್ದಳತೆ,
                                        ಗಂಟು_2 *ಮುಂದಿನ_ಗಂಟು) ಸ್ಥಿರಾಂಕ override;

    UBool ensure_ಸಾಮರ್ಥ್ಯ(int32_t ಉದ್ದಳತೆ);
    ವಾಸ್ತವಿಕ int32_t ಲಿಖ(int32_t unit) override;
    int32_t ಲಿಖ(ಸ್ಥಿರಾಂಕ char16_t *s, int32_t ಉದ್ದಳತೆ);
    ವಾಸ್ತವಿಕ int32_t writeElementUnits(int32_t i, int32_t unit_ಸೂಚ್ಯಂಕ, int32_t ಉದ್ದಳತೆ) override;
    ವಾಸ್ತವಿಕ int32_t write_ಮೌಲ್ಯ_And_Final(int32_t i, UBool isFinal) override;
    ವಾಸ್ತವಿಕ int32_t write_ಮೌಲ್ಯ_And_Type(UBool has_ಮೌಲ್ಯ, int32_t ಮೌಲ್ಯ_2, int32_t ಗಂಟು) override;
    ವಾಸ್ತವಿಕ int32_t write_Delta_ಗಮ್ಯ(int32_t jump_ಗುರಿ) override;

    UnicodeString ಅಕ್ಷರಸರಣಿಗಳು;
    UCharsTrieElement *elements;
    int32_t elements_ಸಾಮರ್ಥ್ಯ;
    int32_t elements_ಉದ್ದಳತೆ;

    // char16_t serialization of the trie.
    // Grows from the back: ucharsLength measures from the end of the buffer!
    char16_t *uchars;
    int32_t uchars_ಸಾಮರ್ಥ್ಯ;
    int32_t uchars_ಉದ್ದಳತೆ;
};

U_NAMESPACE_ಕೊನೆ

#ಆದರೆ_ಅಂತ್ಯ /* U_SHOW_CPLUSPLUS_API */

#ಆದರೆ_ಅಂತ್ಯ  // __UCHARSTRIEBUILDER_H__
