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

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ __BYTESTRIEBUILDER_H__
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ __BYTESTRIEBUILDER_H__

#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/utypes.h"

#ਜੇ U_SHOW_CPLUSPLUS_API

#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/bytestrie.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/stringpiece.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/stringtriebuilder.h"

class ਬਾਈਟਾਂ_Trie_Test;

U_NAMESPACE_BEGIN

class ਬਾਈਟਾਂ_Trie_Element;
class CharString;
/**
 * Builder class for BytesTrie.
 *
 * This class is not intended for public subclassing.
 * @stable ICU 4.8
 */
class U_COMMON_API ਬਾਈਟਾਂ_Trie_Builder : ਜਨਤਕ StringTrieBuilder {
ਜਨਤਕ:
    /**
     * Constructs an empty builder.
     * @param errorCode Standard ICU error code.
     * @stable ICU 4.8
     */
    ਬਾਈਟਾਂ_Trie_Builder(UError_ਕੋਡ &error_ਕੋਡ);

    /**
     * Destructor.
     * @stable ICU 4.8
     */
    ਆਭਾਸੀ ~ਬਾਈਟਾਂ_Trie_Builder();

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
    ਬਾਈਟਾਂ_Trie_Builder& ਜੋੜੋ(StringPiece s, int32_t ਮੁੱਲ_2, UError_ਕੋਡ& error_ਕੋਡ) U_LIFETIME_BOUND;

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
    ਬਾਈਟਾਂ_Trie *build(UStringTrieBuildOption buildOption, UError_ਕੋਡ &error_ਕੋਡ);

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
                                 UError_ਕੋਡ& error_ਕੋਡ) U_LIFETIME_BOUND;

    /**
     * Removes all (byte sequence, value) pairs.
     * New data can then be add()ed and a new trie can be built.
     * @return *this
     * @stable ICU 4.8
     */
    ਬਾਈਟਾਂ_Trie_Builder& clear() U_LIFETIME_BOUND;

ਨਿਜੀ:
    ਮਿੱਤਰ class ::ਬਾਈਟਾਂ_Trie_Test;

    ਬਾਈਟਾਂ_Trie_Builder(ਅਚਲ ਬਾਈਟਾਂ_Trie_Builder &other) = delete;  // no copy constructor
    ਬਾਈਟਾਂ_Trie_Builder &ਸੰਚਾਲਕ=(ਅਚਲ ਬਾਈਟਾਂ_Trie_Builder &other) = delete;  // no assignment operator

    ਖਾਲੀ build_ਬਾਈਟਾਂ(UStringTrieBuildOption buildOption, UError_ਕੋਡ &error_ਕੋਡ);

    ਆਭਾਸੀ int32_t get_Element_String_ਲੰਮਾਈ(int32_t i) ਅਚਲ override;
    ਆਭਾਸੀ char16_t getElementUnit(int32_t i, int32_t byte_ਸੂਚਕਾਂਕ) ਅਚਲ override;
    ਆਭਾਸੀ int32_t get_Element_ਮੁੱਲ(int32_t i) ਅਚਲ override;

    ਆਭਾਸੀ int32_t get_ਹੱਦਬੰਦੀ_ਦਾ_ਰੇਖਿਕ_ਮੇਲ(int32_t ਪਹਿਲਾ, int32_t ਆਖਰੀ, int32_t byte_ਸੂਚਕਾਂਕ) ਅਚਲ override;

    ਆਭਾਸੀ int32_t ਗਿਣਤੀ_Element_Units(int32_t ਸ਼ੁਰੂ, int32_t ਹੱਦਬੰਦੀ, int32_t byte_ਸੂਚਕਾਂਕ) ਅਚਲ override;
    ਆਭਾਸੀ int32_t skipElementsBySomeUnits(int32_t i, int32_t byte_ਸੂਚਕਾਂਕ, int32_t ਗਿਣਤੀ_2) ਅਚਲ override;
    ਆਭਾਸੀ int32_t ਸੂਚਕਾਂਕ_ਦਾ_Element_With_ਅਗਲਾ_Unit(int32_t i, int32_t byte_ਸੂਚਕਾਂਕ, char16_t byte) ਅਚਲ override;

    ਆਭਾਸੀ UBool ਮੇਲ_Nodes_Can_Have_ਮੁੱਲਾਂ() ਅਚਲ override { ਵਾਪਸ false; }

    ਆਭਾਸੀ int32_t get_Max_Branch_ਰੇਖਿਕ_Sub_ਗੰਢ_ਲੰਮਾਈ() ਅਚਲ override { ਵਾਪਸ ਬਾਈਟਾਂ_Trie::k_Max_Branch_ਰੇਖਿਕ_Sub_ਗੰਢ_ਲੰਮਾਈ; }
    ਆਭਾਸੀ int32_t get_Min_ਰੇਖਿਕ_ਮੇਲ() ਅਚਲ override { ਵਾਪਸ ਬਾਈਟਾਂ_Trie::k_Min_ਰੇਖਿਕ_ਮੇਲ; }
    ਆਭਾਸੀ int32_t get_Max_ਰੇਖਿਕ_ਮੇਲ_ਲੰਮਾਈ() ਅਚਲ override { ਵਾਪਸ ਬਾਈਟਾਂ_Trie::k_Max_ਰੇਖਿਕ_ਮੇਲ_ਲੰਮਾਈ; }

    class BTLinear_ਮੇਲ_ਗੰਢ : ਜਨਤਕ ਰੇਖਿਕ_ਮੇਲ_ਗੰਢ {
    ਜਨਤਕ:
        BTLinear_ਮੇਲ_ਗੰਢ(ਅਚਲ ਅੱਖਰ *units, int32_t len, ਗੰਢ_2 *ਅਗਲਾ_ਗੰਢ);
        ਆਭਾਸੀ bool ਸੰਚਾਲਕ==(ਅਚਲ ਗੰਢ_2 &other) ਅਚਲ override;
        ਆਭਾਸੀ ਖਾਲੀ ਲਿਖ(StringTrieBuilder &builder) override;
    ਨਿਜੀ:
        ਅਚਲ ਅੱਖਰ *s;
    };
    
    ਆਭਾਸੀ ਗੰਢ_2 *create_ਰੇਖਿਕ_ਮੇਲ_ਗੰਢ(int32_t i, int32_t byte_ਸੂਚਕਾਂਕ, int32_t ਲੰਮਾਈ,
                                        ਗੰਢ_2 *ਅਗਲਾ_ਗੰਢ) ਅਚਲ override;

    UBool ensure_ਸਮਰੱਥਾ(int32_t ਲੰਮਾਈ);
    ਆਭਾਸੀ int32_t ਲਿਖ(int32_t byte) override;
    int32_t ਲਿਖ(ਅਚਲ ਅੱਖਰ *b, int32_t ਲੰਮਾਈ);
    ਆਭਾਸੀ int32_t writeElementUnits(int32_t i, int32_t byte_ਸੂਚਕਾਂਕ, int32_t ਲੰਮਾਈ) override;
    ਆਭਾਸੀ int32_t write_ਮੁੱਲ_And_Final(int32_t i, UBool isFinal) override;
    ਆਭਾਸੀ int32_t write_ਮੁੱਲ_And_Type(UBool has_ਮੁੱਲ, int32_t ਮੁੱਲ_2, int32_t ਗੰਢ) override;
    ਆਭਾਸੀ int32_t write_Delta_ਮੰਜ਼ਿਲ(int32_t jump_ਨਿਸ਼ਾਨਾ) override;
    ਸਥਿਰ int32_t ਅੰਦਰੂਨੀ_Encode_Delta(int32_t i, ਅੱਖਰ int_ਬਾਈਟਾਂ[]);

    CharString *ਲੜੀਆਂ;  // Pointer not object so we need not #include internal charstr.h.
    ਬਾਈਟਾਂ_Trie_Element *elements;
    int32_t elements_ਸਮਰੱਥਾ;
    int32_t elements_ਲੰਮਾਈ;

    // Byte serialization of the trie.
    // Grows from the back: bytesLength measures from the end of the buffer!
    ਅੱਖਰ *ਬਾਈਟਾਂ;
    int32_t ਬਾਈਟਾਂ_ਸਮਰੱਥਾ;
    int32_t ਬਾਈਟਾਂ_ਲੰਮਾਈ;
};

U_NAMESPACE_ਸਿਰਾ

#ਜੇ_ਅੰਤ /* U_SHOW_CPLUSPLUS_API */

#ਜੇ_ਅੰਤ  // __BYTESTRIEBUILDER_H__
