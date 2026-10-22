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

#अगर_अघोषित __UCHARSTRIEBUILDER_H__
#घोषणा __UCHARSTRIEBUILDER_H__

#समावेश "unicode/utypes.h"

#अगर U_SHOW_CPLUSPLUS_API

#समावेश "unicode/stringtriebuilder.h"
#समावेश "unicode/ucharstrie.h"
#समावेश "unicode/unistr.h"

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
class U_COMMON_API UCharsTrieBuilder : खुला StringTrieBuilder {
खुला:
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
    भव ~UCharsTrieBuilder();

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
    UCharsTrieBuilder& add(स्थिर UnicodeString& s,
                           int32_t मूल्य_2,
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
                                      UnicodeString& परिणाम U_LIFETIME_BOUND,
                                      UErrorCode& errorCode) U_LIFETIME_BOUND;

    /**
     * Removes all (string, value) pairs.
     * New data can then be add()ed and a new trie can be built.
     * @return *this
     * @stable ICU 4.8
     */
    UCharsTrieBuilder& clear() U_LIFETIME_BOUND {
        strings.हटाओ();
        elements_लंबाई=0;
        uchars_लंबाई=0;
        वापस *this;
    }

निजी:
    UCharsTrieBuilder(स्थिर UCharsTrieBuilder &other) = delete;  // no copy constructor
    UCharsTrieBuilder &चालक=(स्थिर UCharsTrieBuilder &other) = delete;  // no assignment operator

    व्योम buildUChars(UStringTrieBuildOption buildOption, UErrorCode &errorCode);

    भव int32_t get_Element_String_लंबाई(int32_t i) स्थिर override;
    भव char16_t getElementUnit(int32_t i, int32_t unit_सूचकांक) स्थिर override;
    भव int32_t get_Element_मूल्य(int32_t i) स्थिर override;

    भव int32_t getLimitOfLinearMatch(int32_t first, int32_t last, int32_t unit_सूचकांक) स्थिर override;

    भव int32_t गिनती_Element_Units(int32_t start, int32_t limit, int32_t unit_सूचकांक) स्थिर override;
    भव int32_t skipElementsBySomeUnits(int32_t i, int32_t unit_सूचकांक, int32_t गिनती) स्थिर override;
    भव int32_t सूचकांक_Of_Element_With_अग्र_Unit(int32_t i, int32_t unit_सूचकांक, char16_t unit) स्थिर override;

    भव UBool match_Nodes_Can_Have_मूल्यावली() स्थिर override { वापस true; }

    भव int32_t get_Max_Branch_Linear_Sub_गाँठ_लंबाई() स्थिर override { वापस UCharsTrie::k_Max_Branch_Linear_Sub_गाँठ_लंबाई; }
    भव int32_t getMinLinearMatch() स्थिर override { वापस UCharsTrie::kMinLinearMatch; }
    भव int32_t get_Max_Linear_Match_लंबाई() स्थिर override { वापस UCharsTrie::k_Max_Linear_Match_लंबाई; }

    class UCTLinear_Match_गाँठ : खुला Linear_Match_गाँठ {
    खुला:
        UCTLinear_Match_गाँठ(स्थिर char16_t *units, int32_t len, गाँठ_2 *अग्र_गाँठ);
        भव bool चालक==(स्थिर गाँठ_2 &other) स्थिर override;
        भव व्योम ग_लिखो(StringTrieBuilder &builder) override;
    निजी:
        स्थिर char16_t *s;
    };

    भव गाँठ_2 *create_Linear_Match_गाँठ(int32_t i, int32_t unit_सूचकांक, int32_t लंबाई,
                                        गाँठ_2 *अग्र_गाँठ) स्थिर override;

    UBool ensureCapacity(int32_t लंबाई);
    भव int32_t ग_लिखो(int32_t unit) override;
    int32_t ग_लिखो(स्थिर char16_t *s, int32_t लंबाई);
    भव int32_t writeElementUnits(int32_t i, int32_t unit_सूचकांक, int32_t लंबाई) override;
    भव int32_t write_मूल्य_And_Final(int32_t i, UBool isFinal) override;
    भव int32_t write_मूल्य_And_Type(UBool has_मूल्य, int32_t मूल्य_2, int32_t गाँठ) override;
    भव int32_t writeDeltaTo(int32_t jumpTarget) override;

    UnicodeString strings;
    UCharsTrieElement *elements;
    int32_t elementsCapacity;
    int32_t elements_लंबाई;

    // char16_t serialization of the trie.
    // Grows from the back: ucharsLength measures from the end of the buffer!
    char16_t *uchars;
    int32_t ucharsCapacity;
    int32_t uchars_लंबाई;
};

U_NAMESPACE_END

#पूर्ण_अगर /* U_SHOW_CPLUSPLUS_API */

#पूर्ण_अगर  // __UCHARSTRIEBUILDER_H__
