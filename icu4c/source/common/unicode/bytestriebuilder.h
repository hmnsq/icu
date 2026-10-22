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

#که_نه_تعریف_شوی __BYTESTRIEBUILDER_H__
#تعریف_کړه __BYTESTRIEBUILDER_H__

#ضمن "unicode/utypes.h"

#إذا U_SHOW_CPLUSPLUS_API

#ضمن "unicode/bytestrie.h"
#ضمن "unicode/stringpiece.h"
#ضمن "unicode/stringtriebuilder.h"

class بایټونه_Trie_Test;

U_NAMESPACE_BEGIN

class بایټونه_Trie_Element;
class CharString;
/**
 * Builder class for BytesTrie.
 *
 * This class is not intended for public subclassing.
 * @stable ICU 4.8
 */
class U_COMMON_API بایټونه_Trie_Builder : عام StringTrieBuilder {
عام:
    /**
     * Constructs an empty builder.
     * @param errorCode Standard ICU error code.
     * @stable ICU 4.8
     */
    بایټونه_Trie_Builder(UError_کوډ &error_کوډ);

    /**
     * Destructor.
     * @stable ICU 4.8
     */
    مجازي ~بایټونه_Trie_Builder();

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
    بایټونه_Trie_Builder& جمع_کړه(StringPiece s, int32_t ارزښت_2, UError_کوډ& error_کوډ) U_LIFETIME_BOUND;

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
    بایټونه_Trie *build(UStringTrieBuildOption buildOption, UError_کوډ &error_کوډ);

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
                                 UError_کوډ& error_کوډ) U_LIFETIME_BOUND;

    /**
     * Removes all (byte sequence, value) pairs.
     * New data can then be add()ed and a new trie can be built.
     * @return *this
     * @stable ICU 4.8
     */
    بایټونه_Trie_Builder& clear() U_LIFETIME_BOUND;

خاص:
    ملګری class ::بایټونه_Trie_Test;

    بایټونه_Trie_Builder(ثابت بایټونه_Trie_Builder &other) = delete;  // no copy constructor
    بایټونه_Trie_Builder &چلوونکی=(ثابت بایټونه_Trie_Builder &other) = delete;  // no assignment operator

    فراغ build_بایټونه(UStringTrieBuildOption buildOption, UError_کوډ &error_کوډ);

    مجازي int32_t get_Element_String_اوږدتیا(int32_t i) ثابت override;
    مجازي char16_t getElementUnit(int32_t i, int32_t byte_شاخص) ثابت override;
    مجازي int32_t get_Element_ارزښت(int32_t i) ثابت override;

    مجازي int32_t get_حد_د_خطي_سمون(int32_t لومړی, int32_t وروستی, int32_t byte_شاخص) ثابت override;

    مجازي int32_t شمېر_Element_Units(int32_t پیل, int32_t حد, int32_t byte_شاخص) ثابت override;
    مجازي int32_t skipElementsBySomeUnits(int32_t i, int32_t byte_شاخص, int32_t شمېر) ثابت override;
    مجازي int32_t شاخص_د_Element_With_بل_Unit(int32_t i, int32_t byte_شاخص, char16_t byte) ثابت override;

    مجازي UBool سمون_Nodes_Can_Have_ارزښتونه() ثابت override { أرجع false; }

    مجازي int32_t get_Max_Branch_خطي_Sub_غوټه_اوږدتیا() ثابت override { أرجع بایټونه_Trie::k_Max_Branch_خطي_Sub_غوټه_اوږدتیا; }
    مجازي int32_t get_Min_خطي_سمون() ثابت override { أرجع بایټونه_Trie::k_Min_خطي_سمون; }
    مجازي int32_t get_Max_خطي_سمون_اوږدتیا() ثابت override { أرجع بایټونه_Trie::k_Max_خطي_سمون_اوږدتیا; }

    class BTLinear_سمون_غوټه : عام خطي_سمون_غوټه {
    عام:
        BTLinear_سمون_غوټه(ثابت حرف *units, int32_t len, غوټه_2 *بل_غوټه);
        مجازي bool چلوونکی==(ثابت غوټه_2 &other) ثابت override;
        مجازي فراغ اكتب(StringTrieBuilder &builder) override;
    خاص:
        ثابت حرف *s;
    };
    
    مجازي غوټه_2 *create_خطي_سمون_غوټه(int32_t i, int32_t byte_شاخص, int32_t اوږدتیا,
                                        غوټه_2 *بل_غوټه) ثابت override;

    UBool ensure_ظرفیت(int32_t اوږدتیا);
    مجازي int32_t اكتب(int32_t byte) override;
    int32_t اكتب(ثابت حرف *b, int32_t اوږدتیا);
    مجازي int32_t writeElementUnits(int32_t i, int32_t byte_شاخص, int32_t اوږدتیا) override;
    مجازي int32_t write_ارزښت_And_Final(int32_t i, UBool isFinal) override;
    مجازي int32_t write_ارزښت_And_Type(UBool has_ارزښت, int32_t ارزښت_2, int32_t غوټه) override;
    مجازي int32_t write_Delta_منزل(int32_t jump_موخه) override;
    ساكن int32_t دننی_Encode_Delta(int32_t i, حرف int_بایټونه[]);

    CharString *مزي_ګانې;  // Pointer not object so we need not #include internal charstr.h.
    بایټونه_Trie_Element *elements;
    int32_t elements_ظرفیت;
    int32_t elements_اوږدتیا;

    // Byte serialization of the trie.
    // Grows from the back: bytesLength measures from the end of the buffer!
    حرف *بایټونه;
    int32_t بایټونه_ظرفیت;
    int32_t بایټونه_اوږدتیا;
};

U_NAMESPACE_پای_ټکی

#د_که_پای /* U_SHOW_CPLUSPLUS_API */

#د_که_پای  // __BYTESTRIEBUILDER_H__
