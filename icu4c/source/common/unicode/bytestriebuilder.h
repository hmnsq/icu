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

#ଅନ୍ତର୍ଭାଵ "unicode/utypes.h"

#ଯଦି U_SHOW_CPLUSPLUS_API

#ଅନ୍ତର୍ଭାଵ "unicode/bytestrie.h"
#ଅନ୍ତର୍ଭାଵ "unicode/stringpiece.h"
#ଅନ୍ତର୍ଭାଵ "unicode/stringtriebuilder.h"

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
class U_COMMON_API BytesTrieBuilder : ସର୍ବସାଧାରଣ StringTrieBuilder {
ସର୍ବସାଧାରଣ:
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
    ଆଭାସୀ ~BytesTrieBuilder();

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
    BytesTrieBuilder& add(StringPiece s, int32_t ମୂଲ୍ୟ_2, UErrorCode& errorCode) U_LIFETIME_BOUND;

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

ବ୍ୟକ୍ତିଗତ:
    friend class ::BytesTrieTest;

    BytesTrieBuilder(ଅଚଳ BytesTrieBuilder &other) = delete;  // no copy constructor
    BytesTrieBuilder &operator=(ଅଚଳ BytesTrieBuilder &other) = delete;  // no assignment operator

    ଶୂନ୍ୟ buildBytes(UStringTrieBuildOption buildOption, UErrorCode &errorCode);

    ଆଭାସୀ int32_t getElementStringLength(int32_t i) ଅଚଳ override;
    ଆଭାସୀ char16_t getElementUnit(int32_t i, int32_t byteIndex) ଅଚଳ override;
    ଆଭାସୀ int32_t get_Element_ମୂଲ୍ୟ(int32_t i) ଅଚଳ override;

    ଆଭାସୀ int32_t getLimitOfLinearMatch(int32_t first, int32_t last, int32_t byteIndex) ଅଚଳ override;

    ଆଭାସୀ int32_t ଗଣନା_Element_Units(int32_t start, int32_t limit, int32_t byteIndex) ଅଚଳ override;
    ଆଭାସୀ int32_t skipElementsBySomeUnits(int32_t i, int32_t byteIndex, int32_t ଗଣନା_2) ଅଚଳ override;
    ଆଭାସୀ int32_t indexOfElementWithNextUnit(int32_t i, int32_t byteIndex, char16_t byte) ଅଚଳ override;

    ଆଭାସୀ UBool match_Nodes_Can_Have_ମୂଲ୍ୟଗୁଡ଼ିକ() ଅଚଳ override { ଫେରନ୍ତୁ false; }

    ଆଭାସୀ int32_t get_Max_Branch_Linear_Sub_ଗଣ୍ଠି_Length() ଅଚଳ override { ଫେରନ୍ତୁ BytesTrie::k_Max_Branch_Linear_Sub_ଗଣ୍ଠି_Length; }
    ଆଭାସୀ int32_t getMinLinearMatch() ଅଚଳ override { ଫେରନ୍ତୁ BytesTrie::kMinLinearMatch; }
    ଆଭାସୀ int32_t getMaxLinearMatchLength() ଅଚଳ override { ଫେରନ୍ତୁ BytesTrie::kMaxLinearMatchLength; }

    class BTLinear_Match_ଗଣ୍ଠି : ସର୍ବସାଧାରଣ Linear_Match_ଗଣ୍ଠି {
    ସର୍ବସାଧାରଣ:
        BTLinear_Match_ଗଣ୍ଠି(ଅଚଳ ଅକ୍ଷର *units, int32_t len, ଗଣ୍ଠି_2 *next_ଗଣ୍ଠି);
        ଆଭାସୀ bool operator==(ଅଚଳ ଗଣ୍ଠି_2 &other) ଅଚଳ override;
        ଆଭାସୀ ଶୂନ୍ୟ ଲିଖ(StringTrieBuilder &builder) override;
    ବ୍ୟକ୍ତିଗତ:
        ଅଚଳ ଅକ୍ଷର *s;
    };
    
    ଆଭାସୀ ଗଣ୍ଠି_2 *create_Linear_Match_ଗଣ୍ଠି(int32_t i, int32_t byteIndex, int32_t length,
                                        ଗଣ୍ଠି_2 *next_ଗଣ୍ଠି) ଅଚଳ override;

    UBool ensureCapacity(int32_t length);
    ଆଭାସୀ int32_t ଲିଖ(int32_t byte) override;
    int32_t ଲିଖ(ଅଚଳ ଅକ୍ଷର *b, int32_t length);
    ଆଭାସୀ int32_t writeElementUnits(int32_t i, int32_t byteIndex, int32_t length) override;
    ଆଭାସୀ int32_t write_ମୂଲ୍ୟ_And_Final(int32_t i, UBool isFinal) override;
    ଆଭାସୀ int32_t write_ମୂଲ୍ୟ_And_Type(UBool has_ମୂଲ୍ୟ, int32_t ମୂଲ୍ୟ_2, int32_t ଗଣ୍ଠି) override;
    ଆଭାସୀ int32_t writeDeltaTo(int32_t jumpTarget) override;
    ସ୍ଥିର int32_t internalEncodeDelta(int32_t i, ଅକ୍ଷର intBytes[]);

    CharString *strings;  // Pointer not object so we need not #include internal charstr.h.
    BytesTrieElement *elements;
    int32_t elementsCapacity;
    int32_t elementsLength;

    // Byte serialization of the trie.
    // Grows from the back: bytesLength measures from the end of the buffer!
    ଅକ୍ଷର *bytes;
    int32_t bytesCapacity;
    int32_t bytesLength;
};

U_NAMESPACE_END

#endif /* U_SHOW_CPLUSPLUS_API */

#endif  // __BYTESTRIEBUILDER_H__
