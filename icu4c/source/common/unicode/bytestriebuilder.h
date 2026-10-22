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
class U_COMMON_API BytesTrieBuilder : সর্বজনীন StringTrieBuilder {
সর্বজনীন:
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
    আভাসী ~BytesTrieBuilder();

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

ব্যক্তিগত:
    friend class ::BytesTrieTest;

    BytesTrieBuilder(ধ্রুবক BytesTrieBuilder &other) = delete;  // no copy constructor
    BytesTrieBuilder &operator=(ধ্রুবক BytesTrieBuilder &other) = delete;  // no assignment operator

    শূন্য buildBytes(UStringTrieBuildOption buildOption, UErrorCode &errorCode);

    আভাসী int32_t get_Element_String_দৈর্ঘ্য(int32_t i) ধ্রুবক override;
    আভাসী char16_t getElementUnit(int32_t i, int32_t byte_সূচক) ধ্রুবক override;
    আভাসী int32_t get_Element_মান(int32_t i) ধ্রুবক override;

    আভাসী int32_t getLimitOfLinearMatch(int32_t first, int32_t last, int32_t byte_সূচক) ধ্রুবক override;

    আভাসী int32_t গুনতি_Element_Units(int32_t start, int32_t limit, int32_t byte_সূচক) ধ্রুবক override;
    আভাসী int32_t skipElementsBySomeUnits(int32_t i, int32_t byte_সূচক, int32_t গুনতি) ধ্রুবক override;
    আভাসী int32_t সূচক_Of_Element_With_পরবর্তী_Unit(int32_t i, int32_t byte_সূচক, char16_t byte) ধ্রুবক override;

    আভাসী UBool match_Nodes_Can_Have_মানসমূহ() ধ্রুবক override { ফেরত false; }

    আভাসী int32_t get_Max_Branch_Linear_Sub_গ্রন্থি_দৈর্ঘ্য() ধ্রুবক override { ফেরত BytesTrie::k_Max_Branch_Linear_Sub_গ্রন্থি_দৈর্ঘ্য; }
    আভাসী int32_t getMinLinearMatch() ধ্রুবক override { ফেরত BytesTrie::kMinLinearMatch; }
    আভাসী int32_t get_Max_Linear_Match_দৈর্ঘ্য() ধ্রুবক override { ফেরত BytesTrie::k_Max_Linear_Match_দৈর্ঘ্য; }

    class BTLinear_Match_গ্রন্থি : সর্বজনীন Linear_Match_গ্রন্থি {
    সর্বজনীন:
        BTLinear_Match_গ্রন্থি(ধ্রুবক অক্ষর *units, int32_t len, গ্রন্থি_2 *পরবর্তী_গ্রন্থি);
        আভাসী bool operator==(ধ্রুবক গ্রন্থি_2 &other) ধ্রুবক override;
        আভাসী শূন্য লিখ(StringTrieBuilder &builder) override;
    ব্যক্তিগত:
        ধ্রুবক অক্ষর *s;
    };
    
    আভাসী গ্রন্থি_2 *create_Linear_Match_গ্রন্থি(int32_t i, int32_t byte_সূচক, int32_t দৈর্ঘ্য,
                                        গ্রন্থি_2 *পরবর্তী_গ্রন্থি) ধ্রুবক override;

    UBool ensureCapacity(int32_t দৈর্ঘ্য);
    আভাসী int32_t লিখ(int32_t byte) override;
    int32_t লিখ(ধ্রুবক অক্ষর *b, int32_t দৈর্ঘ্য);
    আভাসী int32_t writeElementUnits(int32_t i, int32_t byte_সূচক, int32_t দৈর্ঘ্য) override;
    আভাসী int32_t write_মান_And_Final(int32_t i, UBool isFinal) override;
    আভাসী int32_t write_মান_And_Type(UBool has_মান, int32_t মান_2, int32_t গ্রন্থি) override;
    আভাসী int32_t writeDeltaTo(int32_t jumpTarget) override;
    স্থির int32_t internalEncodeDelta(int32_t i, অক্ষর intBytes[]);

    CharString *strings;  // Pointer not object so we need not #include internal charstr.h.
    BytesTrieElement *elements;
    int32_t elementsCapacity;
    int32_t elements_দৈর্ঘ্য;

    // Byte serialization of the trie.
    // Grows from the back: bytesLength measures from the end of the buffer!
    অক্ষর *bytes;
    int32_t bytesCapacity;
    int32_t bytes_দৈর্ঘ্য;
};

U_NAMESPACE_END

#endif /* U_SHOW_CPLUSPLUS_API */

#endif  // __BYTESTRIEBUILDER_H__
