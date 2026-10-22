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

#اگر_غیر_تعریف_شدہ __BYTESTRIEBUILDER_H__
#تعریف_کرو __BYTESTRIEBUILDER_H__

#شامل "unicode/utypes.h"

#اگر U_SHOW_CPLUSPLUS_API

#شامل "unicode/bytestrie.h"
#شامل "unicode/stringpiece.h"
#شامل "unicode/stringtriebuilder.h"

class بائٹیں_Trie_Test;

U_NAMESPACE_BEGIN

class بائٹیں_Trie_Element;
class CharString;
/**
 * Builder class for BytesTrie.
 *
 * This class is not intended for public subclassing.
 * @stable ICU 4.8
 */
class U_COMMON_API بائٹیں_Trie_Builder : عوامی StringTrieBuilder {
عوامی:
    /**
     * Constructs an empty builder.
     * @param errorCode Standard ICU error code.
     * @stable ICU 4.8
     */
    بائٹیں_Trie_Builder(UError_رمز &error_رمز);

    /**
     * Destructor.
     * @stable ICU 4.8
     */
    مجازی ~بائٹیں_Trie_Builder();

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
    بائٹیں_Trie_Builder& جوڑو(StringPiece s, int32_t قدر_2, UError_رمز& error_رمز) U_LIFETIME_BOUND;

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
    بائٹیں_Trie *build(UStringTrieBuildOption buildOption, UError_رمز &error_رمز);

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
                                 UError_رمز& error_رمز) U_LIFETIME_BOUND;

    /**
     * Removes all (byte sequence, value) pairs.
     * New data can then be add()ed and a new trie can be built.
     * @return *this
     * @stable ICU 4.8
     */
    بائٹیں_Trie_Builder& clear() U_LIFETIME_BOUND;

نجی:
    دوست class ::بائٹیں_Trie_Test;

    بائٹیں_Trie_Builder(ثابت بائٹیں_Trie_Builder &other) = delete;  // no copy constructor
    بائٹیں_Trie_Builder &عامل=(ثابت بائٹیں_Trie_Builder &other) = delete;  // no assignment operator

    خالی build_بائٹیں(UStringTrieBuildOption buildOption, UError_رمز &error_رمز);

    مجازی int32_t get_Element_String_لمبائی(int32_t i) ثابت override;
    مجازی char16_t getElementUnit(int32_t i, int32_t byte_اشاریہ) ثابت override;
    مجازی int32_t get_Element_قدر(int32_t i) ثابت override;

    مجازی int32_t get_حد_بندی_کا_خطی_مطابقت(int32_t پہلا, int32_t آخری, int32_t byte_اشاریہ) ثابت override;

    مجازی int32_t گنتی_Element_Units(int32_t آغاز, int32_t حد_بندی, int32_t byte_اشاریہ) ثابت override;
    مجازی int32_t skipElementsBySomeUnits(int32_t i, int32_t byte_اشاریہ, int32_t گنتی) ثابت override;
    مجازی int32_t اشاریہ_کا_Element_With_اگلا_Unit(int32_t i, int32_t byte_اشاریہ, char16_t byte) ثابت override;

    مجازی UBool مطابقت_Nodes_Can_Have_اقدار() ثابت override { واپس false; }

    مجازی int32_t get_Max_Branch_خطی_Sub_گرہ_لمبائی() ثابت override { واپس بائٹیں_Trie::k_Max_Branch_خطی_Sub_گرہ_لمبائی; }
    مجازی int32_t get_Min_خطی_مطابقت() ثابت override { واپس بائٹیں_Trie::k_Min_خطی_مطابقت; }
    مجازی int32_t get_Max_خطی_مطابقت_لمبائی() ثابت override { واپس بائٹیں_Trie::k_Max_خطی_مطابقت_لمبائی; }

    class BTLinear_مطابقت_گرہ : عوامی خطی_مطابقت_گرہ {
    عوامی:
        BTLinear_مطابقت_گرہ(ثابت حرف *units, int32_t len, گرہ_2 *اگلا_گرہ);
        مجازی bool عامل==(ثابت گرہ_2 &other) ثابت override;
        مجازی خالی اكتب(StringTrieBuilder &builder) override;
    نجی:
        ثابت حرف *s;
    };
    
    مجازی گرہ_2 *create_خطی_مطابقت_گرہ(int32_t i, int32_t byte_اشاریہ, int32_t لمبائی,
                                        گرہ_2 *اگلا_گرہ) ثابت override;

    UBool ensure_استعداد(int32_t لمبائی);
    مجازی int32_t اكتب(int32_t byte) override;
    int32_t اكتب(ثابت حرف *b, int32_t لمبائی);
    مجازی int32_t writeElementUnits(int32_t i, int32_t byte_اشاریہ, int32_t لمبائی) override;
    مجازی int32_t write_قدر_And_Final(int32_t i, UBool isFinal) override;
    مجازی int32_t write_قدر_And_Type(UBool has_قدر, int32_t قدر_2, int32_t گرہ) override;
    مجازی int32_t write_Delta_مقصد(int32_t jump_ہدف) override;
    ساکن int32_t اندرونی_Encode_Delta(int32_t i, حرف int_بائٹیں[]);

    CharString *لڑیاں;  // Pointer not object so we need not #include internal charstr.h.
    بائٹیں_Trie_Element *elements;
    int32_t elements_استعداد;
    int32_t elements_لمبائی;

    // Byte serialization of the trie.
    // Grows from the back: bytesLength measures from the end of the buffer!
    حرف *بائٹیں;
    int32_t بائٹیں_استعداد;
    int32_t بائٹیں_لمبائی;
};

U_NAMESPACE_انتہا

#اگر_ختم /* U_SHOW_CPLUSPLUS_API */

#اگر_ختم  // __BYTESTRIEBUILDER_H__
