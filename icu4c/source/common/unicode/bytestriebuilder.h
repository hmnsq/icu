// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
*******************************************************************************
*   Copyright (C) 2010-2016, International Business Machines
*   Corporation and others.  All Rights Reserved.
*******************************************************************************
*   file name:  bytestriebuilder.h
*   encoding:   UTF-8
*   tab size:   8 (not used)
*   indentation:4
*
*   created on: 2010sep25
*   created by: Markus W. Scherer
*/

/**
 * \file
 * \brief C++ API: Builder for icu::BytesTrie
 */

#अगर_अघोषित __BYTESTRIEBUILDER_H__
#घोषणा __BYTESTRIEBUILDER_H__

#समावेश "unicode/utypes.h"

#अगर U_SHOW_CPLUSPLUS_API

#समावेश "unicode/bytestrie.h"
#समावेश "unicode/stringpiece.h"
#समावेश "unicode/stringtriebuilder.h"

class BytesTrieTest;

U_NAMESPACE_BEGIN

class BytesTrieElement;
class CharString;
/**
 * Builder class for BytesTrie.
 *
 * This class is not intended for public subclassing.
 * @stable ICU 4.8
 */
class U_COMMON_API BytesTrieBuilder : खुला StringTrieBuilder {
खुला:
    /**
     * Constructs an empty builder.
     * @param errorCode Standard ICU error code.
     * @stable ICU 4.8
     */
    BytesTrieBuilder(UErrorCode &errorCode);

    /**
     * Destructor.
     * @stable ICU 4.8
     */
    भव ~BytesTrieBuilder();

    /**
     * Adds a (byte sequence, value) pair.
     * The byte sequence must be unique.
     * The bytes will be copied; the builder does not keep
     * a reference to the input StringPiece or its data().
     * @param s The input byte sequence.
     * @param value The value associated with this byte sequence.
     * @param errorCode Standard ICU error code. Its input value must
     *                  pass the U_SUCCESS() test, or else the function returns
     *                  immediately. Check for U_FAILURE() on output or use with
     *                  function chaining. (See User Guide for details.)
     * @return *this
     * @stable ICU 4.8
     */
    BytesTrieBuilder& add(StringPiece s, int32_t मूल्य_2, UErrorCode& errorCode) U_LIFETIME_BOUND;

    /**
     * Builds a BytesTrie for the add()ed data.
     * Once built, no further data can be add()ed until clear() is called.
     *
     * A BytesTrie cannot be empty. At least one (byte sequence, value) pair
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
     * @return A new BytesTrie for the add()ed data.
     * @stable ICU 4.8
     */
    BytesTrie *build(UStringTrieBuildOption buildOption, UErrorCode &errorCode);

    /**
     * Builds a BytesTrie for the add()ed data and byte-serializes it.
     * Once built, no further data can be add()ed until clear() is called.
     *
     * A BytesTrie cannot be empty. At least one (byte sequence, value) pair
     * must have been add()ed.
     *
     * Multiple calls to buildStringPiece() return StringPieces referring to the
     * builder's same byte array, without rebuilding.
     * If buildStringPiece() is called after build(), the trie will be
     * re-serialized into a new array (because build() passes on ownership).
     * If build() is called after buildStringPiece(), the trie object returned
     * by build() will become the owner of the underlying string for the
     * previously returned StringPiece.
     * After clear() has been called, a new array will be used as well.
     * @param buildOption Build option, see UStringTrieBuildOption.
     * @param errorCode Standard ICU error code. Its input value must
     *                  pass the U_SUCCESS() test, or else the function returns
     *                  immediately. Check for U_FAILURE() on output or use with
     *                  function chaining. (See User Guide for details.)
     * @return A StringPiece which refers to the byte-serialized BytesTrie for the add()ed data.
     * @stable ICU 4.8
     */
    StringPiece buildStringPiece(UStringTrieBuildOption buildOption,
                                 UErrorCode& errorCode) U_LIFETIME_BOUND;

    /**
     * Removes all (byte sequence, value) pairs.
     * New data can then be add()ed and a new trie can be built.
     * @return *this
     * @stable ICU 4.8
     */
    BytesTrieBuilder& clear() U_LIFETIME_BOUND;

निजी:
    मित्र class ::BytesTrieTest;

    BytesTrieBuilder(स्थिर BytesTrieBuilder &other) = delete;  // no copy constructor
    BytesTrieBuilder &चालक=(स्थिर BytesTrieBuilder &other) = delete;  // no assignment operator

    व्योम buildBytes(UStringTrieBuildOption buildOption, UErrorCode &errorCode);

    भव int32_t get_Element_String_लंबाई(int32_t i) स्थिर override;
    भव char16_t getElementUnit(int32_t i, int32_t byte_सूचकांक) स्थिर override;
    भव int32_t get_Element_मूल्य(int32_t i) स्थिर override;

    भव int32_t getLimitOfLinearMatch(int32_t first, int32_t last, int32_t byte_सूचकांक) स्थिर override;

    भव int32_t गिनती_Element_Units(int32_t start, int32_t limit, int32_t byte_सूचकांक) स्थिर override;
    भव int32_t skipElementsBySomeUnits(int32_t i, int32_t byte_सूचकांक, int32_t गिनती) स्थिर override;
    भव int32_t सूचकांक_Of_Element_With_अग्र_Unit(int32_t i, int32_t byte_सूचकांक, char16_t byte) स्थिर override;

    भव UBool match_Nodes_Can_Have_मूल्यावली() स्थिर override { वापस false; }

    भव int32_t get_Max_Branch_Linear_Sub_गाँठ_लंबाई() स्थिर override { वापस BytesTrie::k_Max_Branch_Linear_Sub_गाँठ_लंबाई; }
    भव int32_t getMinLinearMatch() स्थिर override { वापस BytesTrie::kMinLinearMatch; }
    भव int32_t get_Max_Linear_Match_लंबाई() स्थिर override { वापस BytesTrie::k_Max_Linear_Match_लंबाई; }

    class BTLinear_Match_गाँठ : खुला Linear_Match_गाँठ {
    खुला:
        BTLinear_Match_गाँठ(स्थिर अक्षर *units, int32_t len, गाँठ_2 *अग्र_गाँठ);
        भव bool चालक==(स्थिर गाँठ_2 &other) स्थिर override;
        भव व्योम ग_लिखो(StringTrieBuilder &builder) override;
    निजी:
        स्थिर अक्षर *s;
    };
    
    भव गाँठ_2 *create_Linear_Match_गाँठ(int32_t i, int32_t byte_सूचकांक, int32_t लंबाई,
                                        गाँठ_2 *अग्र_गाँठ) स्थिर override;

    UBool ensureCapacity(int32_t लंबाई);
    भव int32_t ग_लिखो(int32_t byte) override;
    int32_t ग_लिखो(स्थिर अक्षर *b, int32_t लंबाई);
    भव int32_t writeElementUnits(int32_t i, int32_t byte_सूचकांक, int32_t लंबाई) override;
    भव int32_t write_मूल्य_And_Final(int32_t i, UBool isFinal) override;
    भव int32_t write_मूल्य_And_Type(UBool has_मूल्य, int32_t मूल्य_2, int32_t गाँठ) override;
    भव int32_t writeDeltaTo(int32_t jumpTarget) override;
    अटल int32_t internalEncodeDelta(int32_t i, अक्षर intBytes[]);

    CharString *strings;  // Pointer not object so we need not #include internal charstr.h.
    BytesTrieElement *elements;
    int32_t elementsCapacity;
    int32_t elements_लंबाई;

    // Byte serialization of the trie.
    // Grows from the back: bytesLength measures from the end of the buffer!
    अक्षर *bytes;
    int32_t bytesCapacity;
    int32_t bytes_लंबाई;
};

U_NAMESPACE_END

#पूर्ण_अगर /* U_SHOW_CPLUSPLUS_API */

#पूर्ण_अगर  // __BYTESTRIEBUILDER_H__
