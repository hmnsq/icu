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

#جيڪڏهن_وصف_نه_ٿيل __BYTESTRIEBUILDER_H__
#وصف_ڪريو __BYTESTRIEBUILDER_H__

#ضمن "unicode/utypes.h"

#إذا U_SHOW_CPLUSPLUS_API

#ضمن "unicode/bytestrie.h"
#ضمن "unicode/stringpiece.h"
#ضمن "unicode/stringtriebuilder.h"

class بائيٽون_Trie_Test;

U_NAMESPACE_BEGIN

class بائيٽون_Trie_Element;
class CharString;
/**
 * Builder class for BytesTrie.
 *
 * This class is not intended for public subclassing.
 * @stable ICU 4.8
 */
class U_COMMON_API بائيٽون_Trie_Builder : عام StringTrieBuilder {
عام:
    /**
     * Constructs an empty builder.
     * @param errorCode Standard ICU error code.
     * @stable ICU 4.8
     */
    بائيٽون_Trie_Builder(UError_ڪوڊ &error_ڪوڊ);

    /**
     * Destructor.
     * @stable ICU 4.8
     */
    مجازي ~بائيٽون_Trie_Builder();

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
    بائيٽون_Trie_Builder& جوڙيو(StringPiece s, int32_t قدر_2, UError_ڪوڊ& error_ڪوڊ) U_LIFETIME_BOUND;

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
    بائيٽون_Trie *build(UStringTrieBuildOption buildOption, UError_ڪوڊ &error_ڪوڊ);

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
                                 UError_ڪوڊ& error_ڪوڊ) U_LIFETIME_BOUND;

    /**
     * Removes all (byte sequence, value) pairs.
     * New data can then be add()ed and a new trie can be built.
     * @return *this
     * @stable ICU 4.8
     */
    بائيٽون_Trie_Builder& clear() U_LIFETIME_BOUND;

خاص:
    دوست class ::بائيٽون_Trie_Test;

    بائيٽون_Trie_Builder(ثابت بائيٽون_Trie_Builder &other) = delete;  // no copy constructor
    بائيٽون_Trie_Builder &عامل=(ثابت بائيٽون_Trie_Builder &other) = delete;  // no assignment operator

    فراغ build_بائيٽون(UStringTrieBuildOption buildOption, UError_ڪوڊ &error_ڪوڊ);

    مجازي int32_t get_Element_String_ڊيگهائي(int32_t i) ثابت override;
    مجازي char16_t getElementUnit(int32_t i, int32_t byte_اشاريو) ثابت override;
    مجازي int32_t get_Element_قدر(int32_t i) ثابت override;

    مجازي int32_t get_حد_بندي_جو_لڪيري_ميلاپ(int32_t پهريون, int32_t آخري, int32_t byte_اشاريو) ثابت override;

    مجازي int32_t ڳڻپ_Element_Units(int32_t شروعات, int32_t حد_بندي, int32_t byte_اشاريو) ثابت override;
    مجازي int32_t skipElementsBySomeUnits(int32_t i, int32_t byte_اشاريو, int32_t ڳڻپ) ثابت override;
    مجازي int32_t اشاريو_جو_Element_With_ايندڙ_Unit(int32_t i, int32_t byte_اشاريو, char16_t byte) ثابت override;

    مجازي UBool ميلاپ_Nodes_Can_Have_قدرون() ثابت override { أرجع false; }

    مجازي int32_t get_Max_Branch_لڪيري_Sub_ڳنڍ_ڊيگهائي() ثابت override { أرجع بائيٽون_Trie::k_Max_Branch_لڪيري_Sub_ڳنڍ_ڊيگهائي; }
    مجازي int32_t get_Min_لڪيري_ميلاپ() ثابت override { أرجع بائيٽون_Trie::k_Min_لڪيري_ميلاپ; }
    مجازي int32_t get_Max_لڪيري_ميلاپ_ڊيگهائي() ثابت override { أرجع بائيٽون_Trie::k_Max_لڪيري_ميلاپ_ڊيگهائي; }

    class BTLinear_ميلاپ_ڳنڍ : عام لڪيري_ميلاپ_ڳنڍ {
    عام:
        BTLinear_ميلاپ_ڳنڍ(ثابت حرف *units, int32_t len, ڳنڍ_2 *ايندڙ_ڳنڍ);
        مجازي bool عامل==(ثابت ڳنڍ_2 &other) ثابت override;
        مجازي فراغ اكتب(StringTrieBuilder &builder) override;
    خاص:
        ثابت حرف *s;
    };
    
    مجازي ڳنڍ_2 *create_لڪيري_ميلاپ_ڳنڍ(int32_t i, int32_t byte_اشاريو, int32_t ڊيگهائي,
                                        ڳنڍ_2 *ايندڙ_ڳنڍ) ثابت override;

    UBool ensure_صلاحيت(int32_t ڊيگهائي);
    مجازي int32_t اكتب(int32_t byte) override;
    int32_t اكتب(ثابت حرف *b, int32_t ڊيگهائي);
    مجازي int32_t writeElementUnits(int32_t i, int32_t byte_اشاريو, int32_t ڊيگهائي) override;
    مجازي int32_t write_قدر_And_Final(int32_t i, UBool isFinal) override;
    مجازي int32_t write_قدر_And_Type(UBool has_قدر, int32_t قدر_2, int32_t ڳنڍ) override;
    مجازي int32_t write_Delta_منزل(int32_t jump_هدف) override;
    ساكن int32_t اندروني_Encode_Delta(int32_t i, حرف int_بائيٽون[]);

    CharString *لڙيون;  // Pointer not object so we need not #include internal charstr.h.
    بائيٽون_Trie_Element *elements;
    int32_t elements_صلاحيت;
    int32_t elements_ڊيگهائي;

    // Byte serialization of the trie.
    // Grows from the back: bytesLength measures from the end of the buffer!
    حرف *بائيٽون;
    int32_t بائيٽون_صلاحيت;
    int32_t بائيٽون_ڊيگهائي;
};

U_NAMESPACE_پڇاڙي

#جيڪڏهن_پورو /* U_SHOW_CPLUSPLUS_API */

#جيڪڏهن_پورو  // __BYTESTRIEBUILDER_H__
