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

#ifndef __BYTESTRIEBUILDER_H__
#define __BYTESTRIEBUILDER_H__

#ضمن "unicode/utypes.h"

#إذا U_SHOW_CPLUSPLUS_API

#ضمن "unicode/bytestrie.h"
#ضمن "unicode/stringpiece.h"
#ضمن "unicode/stringtriebuilder.h"

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
class U_COMMON_API BytesTrieBuilder : عام StringTrieBuilder {
عام:
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
    virtual ~BytesTrieBuilder();

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
    BytesTrieBuilder& add(StringPiece s, int32_t بەها_2, UErrorCode& errorCode) U_LIFETIME_BOUND;

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

خاص:
    friend class ::BytesTrieTest;

    BytesTrieBuilder(ثابت BytesTrieBuilder &other) = delete;  // no copy constructor
    BytesTrieBuilder &operator=(ثابت BytesTrieBuilder &other) = delete;  // no assignment operator

    فراغ buildBytes(UStringTrieBuildOption buildOption, UErrorCode &errorCode);

    virtual int32_t getElementStringLength(int32_t i) ثابت override;
    virtual char16_t getElementUnit(int32_t i, int32_t byteIndex) ثابت override;
    virtual int32_t get_Element_بەها(int32_t i) ثابت override;

    virtual int32_t getLimitOfLinearMatch(int32_t first, int32_t last, int32_t byteIndex) ثابت override;

    virtual int32_t ژماردن_Element_Units(int32_t start, int32_t limit, int32_t byteIndex) ثابت override;
    virtual int32_t skipElementsBySomeUnits(int32_t i, int32_t byteIndex, int32_t ژماردن) ثابت override;
    virtual int32_t indexOfElementWithNextUnit(int32_t i, int32_t byteIndex, char16_t byte) ثابت override;

    virtual UBool match_Nodes_Can_Have_بەهاکان() ثابت override { أرجع false; }

    virtual int32_t get_Max_Branch_Linear_Sub_گرێ_Length() ثابت override { أرجع BytesTrie::k_Max_Branch_Linear_Sub_گرێ_Length; }
    virtual int32_t getMinLinearMatch() ثابت override { أرجع BytesTrie::kMinLinearMatch; }
    virtual int32_t getMaxLinearMatchLength() ثابت override { أرجع BytesTrie::kMaxLinearMatchLength; }

    class BTLinear_Match_گرێ : عام Linear_Match_گرێ {
    عام:
        BTLinear_Match_گرێ(ثابت حرف *units, int32_t len, گرێ_2 *next_گرێ);
        virtual bool operator==(ثابت گرێ_2 &other) ثابت override;
        virtual فراغ اكتب(StringTrieBuilder &builder) override;
    خاص:
        ثابت حرف *s;
    };
    
    virtual گرێ_2 *create_Linear_Match_گرێ(int32_t i, int32_t byteIndex, int32_t length,
                                        گرێ_2 *next_گرێ) ثابت override;

    UBool ensureCapacity(int32_t length);
    virtual int32_t اكتب(int32_t byte) override;
    int32_t اكتب(ثابت حرف *b, int32_t length);
    virtual int32_t writeElementUnits(int32_t i, int32_t byteIndex, int32_t length) override;
    virtual int32_t write_بەها_And_Final(int32_t i, UBool isFinal) override;
    virtual int32_t write_بەها_And_Type(UBool has_بەها, int32_t بەها_2, int32_t گرێ) override;
    virtual int32_t writeDeltaTo(int32_t jumpTarget) override;
    ساكن int32_t internalEncodeDelta(int32_t i, حرف intBytes[]);

    CharString *strings;  // Pointer not object so we need not #include internal charstr.h.
    BytesTrieElement *elements;
    int32_t elementsCapacity;
    int32_t elementsLength;

    // Byte serialization of the trie.
    // Grows from the back: bytesLength measures from the end of the buffer!
    حرف *bytes;
    int32_t bytesCapacity;
    int32_t bytesLength;
};

U_NAMESPACE_END

#endif /* U_SHOW_CPLUSPLUS_API */

#endif  // __BYTESTRIEBUILDER_H__
