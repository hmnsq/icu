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

#অন্তর্ভাব "unicode/utypes.h"

#যদি U_SHOW_CPLUSPLUS_API

#অন্তর্ভাব "unicode/bytestrie.h"
#অন্তর্ভাব "unicode/stringpiece.h"
#অন্তর্ভাব "unicode/stringtriebuilder.h"

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
class U_COMMON_API BytesTrieBuilder : সার্বজনিক StringTrieBuilder {
সার্বজনিক:
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
    আভাস ~BytesTrieBuilder();

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
    BytesTrieBuilder& add(StringPiece s, int32_t মান_2, UErrorCode& errorCode) U_LIFETIME_BOUND;

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

গুপ্ত:
    friend class ::BytesTrieTest;

    BytesTrieBuilder(ধ্রুব BytesTrieBuilder &other) = delete;  // no copy constructor
    BytesTrieBuilder &operator=(ধ্রুব BytesTrieBuilder &other) = delete;  // no assignment operator

    শূন্য buildBytes(UStringTrieBuildOption buildOption, UErrorCode &errorCode);

    আভাস int32_t getElementStringLength(int32_t i) ধ্রুব override;
    আভাস char16_t getElementUnit(int32_t i, int32_t byteIndex) ধ্রুব override;
    আভাস int32_t get_Element_মান(int32_t i) ধ্রুব override;

    আভাস int32_t getLimitOfLinearMatch(int32_t first, int32_t last, int32_t byteIndex) ধ্রুব override;

    আভাস int32_t গুনতি_Element_Units(int32_t start, int32_t limit, int32_t byteIndex) ধ্রুব override;
    আভাস int32_t skipElementsBySomeUnits(int32_t i, int32_t byteIndex, int32_t গুনতি) ধ্রুব override;
    আভাস int32_t indexOfElementWithNextUnit(int32_t i, int32_t byteIndex, char16_t byte) ধ্রুব override;

    আভাস UBool match_Nodes_Can_Have_মানসমূহ() ধ্রুব override { প্রত্যাগম false; }

    আভাস int32_t get_Max_Branch_Linear_Sub_গ্রন্থি_Length() ধ্রুব override { প্রত্যাগম BytesTrie::k_Max_Branch_Linear_Sub_গ্রন্থি_Length; }
    আভাস int32_t getMinLinearMatch() ধ্রুব override { প্রত্যাগম BytesTrie::kMinLinearMatch; }
    আভাস int32_t getMaxLinearMatchLength() ধ্রুব override { প্রত্যাগম BytesTrie::kMaxLinearMatchLength; }

    class BTLinear_Match_গ্রন্থি : সার্বজনিক Linear_Match_গ্রন্থি {
    সার্বজনিক:
        BTLinear_Match_গ্রন্থি(ধ্রুব বর্ণ *units, int32_t len, গ্রন্থি_2 *next_গ্রন্থি);
        আভাস bool operator==(ধ্রুব গ্রন্থি_2 &other) ধ্রুব override;
        আভাস শূন্য লিখ(StringTrieBuilder &builder) override;
    গুপ্ত:
        ধ্রুব বর্ণ *s;
    };
    
    আভাস গ্রন্থি_2 *create_Linear_Match_গ্রন্থি(int32_t i, int32_t byteIndex, int32_t length,
                                        গ্রন্থি_2 *next_গ্রন্থি) ধ্রুব override;

    UBool ensureCapacity(int32_t length);
    আভাস int32_t লিখ(int32_t byte) override;
    int32_t লিখ(ধ্রুব বর্ণ *b, int32_t length);
    আভাস int32_t writeElementUnits(int32_t i, int32_t byteIndex, int32_t length) override;
    আভাস int32_t write_মান_And_Final(int32_t i, UBool isFinal) override;
    আভাস int32_t write_মান_And_Type(UBool has_মান, int32_t মান_2, int32_t গ্রন্থি) override;
    আভাস int32_t writeDeltaTo(int32_t jumpTarget) override;
    স্থির int32_t internalEncodeDelta(int32_t i, বর্ণ intBytes[]);

    CharString *strings;  // Pointer not object so we need not #include internal charstr.h.
    BytesTrieElement *elements;
    int32_t elementsCapacity;
    int32_t elementsLength;

    // Byte serialization of the trie.
    // Grows from the back: bytesLength measures from the end of the buffer!
    বর্ণ *bytes;
    int32_t bytesCapacity;
    int32_t bytesLength;
};

U_NAMESPACE_END

#endif /* U_SHOW_CPLUSPLUS_API */

#endif  // __BYTESTRIEBUILDER_H__
