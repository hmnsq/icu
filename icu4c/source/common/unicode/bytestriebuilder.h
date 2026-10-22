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

#اگر_تعریف_نشده __BYTESTRIEBUILDER_H__
#تعریف_کن __BYTESTRIEBUILDER_H__

#ضمن "unicode/utypes.h"

#إذا U_SHOW_CPLUSPLUS_API

#ضمن "unicode/bytestrie.h"
#ضمن "unicode/stringpiece.h"
#ضمن "unicode/stringtriebuilder.h"

class بایتها_Trie_Test;

U_NAMESPACE_BEGIN

class بایتها_Trie_Element;
class CharString;
/**
 * Builder class for BytesTrie.
 *
 * This class is not intended for public subclassing.
 * @stable ICU 4.8
 */
class U_COMMON_API بایتها_Trie_Builder : عام StringTrieBuilder {
عام:
    /**
     * Constructs an empty builder.
     * @param errorCode Standard ICU error code.
     * @stable ICU 4.8
     */
    بایتها_Trie_Builder(UError_کد &error_کد);

    /**
     * Destructor.
     * @stable ICU 4.8
     */
    مجازی ~بایتها_Trie_Builder();

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
    بایتها_Trie_Builder& جمع_کن(StringPiece s, int32_t مقدار_2, UError_کد& error_کد) U_LIFETIME_BOUND;

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
    بایتها_Trie *build(UStringTrieBuildOption buildOption, UError_کد &error_کد);

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
                                 UError_کد& error_کد) U_LIFETIME_BOUND;

    /**
     * Removes all (byte sequence, value) pairs.
     * New data can then be add()ed and a new trie can be built.
     * @return *this
     * @stable ICU 4.8
     */
    بایتها_Trie_Builder& clear() U_LIFETIME_BOUND;

خاص:
    دوست class ::بایتها_Trie_Test;

    بایتها_Trie_Builder(ثابت بایتها_Trie_Builder &other) = delete;  // no copy constructor
    بایتها_Trie_Builder &عملگر=(ثابت بایتها_Trie_Builder &other) = delete;  // no assignment operator

    فراغ build_بایتها(UStringTrieBuildOption buildOption, UError_کد &error_کد);

    مجازی int32_t get_Element_String_درازی(int32_t i) ثابت override;
    مجازی char16_t getElementUnit(int32_t i, int32_t byte_شاخص) ثابت override;
    مجازی int32_t get_Element_مقدار(int32_t i) ثابت override;

    مجازی int32_t get_حد_از_خطی_تطابق(int32_t اولی, int32_t آخرین, int32_t byte_شاخص) ثابت override;

    مجازی int32_t شمار_Element_Units(int32_t شروع, int32_t حد, int32_t byte_شاخص) ثابت override;
    مجازی int32_t skipElementsBySomeUnits(int32_t i, int32_t byte_شاخص, int32_t شمار) ثابت override;
    مجازی int32_t شاخص_از_Element_With_بعدی_Unit(int32_t i, int32_t byte_شاخص, char16_t byte) ثابت override;

    مجازی UBool تطابق_Nodes_Can_Have_مقادیر() ثابت override { أرجع false; }

    مجازی int32_t get_Max_Branch_خطی_Sub_گره_درازی() ثابت override { أرجع بایتها_Trie::k_Max_Branch_خطی_Sub_گره_درازی; }
    مجازی int32_t get_Min_خطی_تطابق() ثابت override { أرجع بایتها_Trie::k_Min_خطی_تطابق; }
    مجازی int32_t get_Max_خطی_تطابق_درازی() ثابت override { أرجع بایتها_Trie::k_Max_خطی_تطابق_درازی; }

    class BTLinear_تطابق_گره : عام خطی_تطابق_گره {
    عام:
        BTLinear_تطابق_گره(ثابت حرف *units, int32_t len, گره_2 *بعدی_گره);
        مجازی bool عملگر==(ثابت گره_2 &other) ثابت override;
        مجازی فراغ اكتب(StringTrieBuilder &builder) override;
    خاص:
        ثابت حرف *s;
    };
    
    مجازی گره_2 *create_خطی_تطابق_گره(int32_t i, int32_t byte_شاخص, int32_t درازی,
                                        گره_2 *بعدی_گره) ثابت override;

    UBool ensure_ظرفیت(int32_t درازی);
    مجازی int32_t اكتب(int32_t byte) override;
    int32_t اكتب(ثابت حرف *b, int32_t درازی);
    مجازی int32_t writeElementUnits(int32_t i, int32_t byte_شاخص, int32_t درازی) override;
    مجازی int32_t write_مقدار_And_Final(int32_t i, UBool isFinal) override;
    مجازی int32_t write_مقدار_And_Type(UBool has_مقدار, int32_t مقدار_2, int32_t گره) override;
    مجازی int32_t write_Delta_مقصد(int32_t jump_هدف) override;
    ساكن int32_t داخلی_Encode_Delta(int32_t i, حرف int_بایتها[]);

    CharString *رشتهها;  // Pointer not object so we need not #include internal charstr.h.
    بایتها_Trie_Element *elements;
    int32_t elements_ظرفیت;
    int32_t elements_درازی;

    // Byte serialization of the trie.
    // Grows from the back: bytesLength measures from the end of the buffer!
    حرف *بایتها;
    int32_t بایتها_ظرفیت;
    int32_t بایتها_درازی;
};

U_NAMESPACE_انتها

#پایان_اگر /* U_SHOW_CPLUSPLUS_API */

#پایان_اگر  // __BYTESTRIEBUILDER_H__
