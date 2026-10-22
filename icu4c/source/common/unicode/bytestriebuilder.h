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

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ __BYTESTRIEBUILDER_H__
#ವ್ಯಾಖ್ಯಾನಿಸು __BYTESTRIEBUILDER_H__

#ಅನ್ತರ್ಭಾವ "unicode/utypes.h"

#ಆದರೆ U_SHOW_CPLUSPLUS_API

#ಅನ್ತರ್ಭಾವ "unicode/bytestrie.h"
#ಅನ್ತರ್ಭಾವ "unicode/stringpiece.h"
#ಅನ್ತರ್ಭಾವ "unicode/stringtriebuilder.h"

class ಬೈಟುಗಳು_Trie_Test;

U_NAMESPACE_BEGIN

class ಬೈಟುಗಳು_Trie_Element;
class CharString;
/**
 * Builder class for BytesTrie.
 *
 * This class is not intended for public subclassing.
 * @stable ICU 4.8
 */
class U_COMMON_API ಬೈಟುಗಳು_Trie_Builder : ಸಾರ್ವಜನಿಕ StringTrieBuilder {
ಸಾರ್ವಜನಿಕ:
    /**
     * Constructs an empty builder.
     * @param errorCode Standard ICU error code.
     * @stable ICU 4.8
     */
    ಬೈಟುಗಳು_Trie_Builder(UError_ಸಂಕೇತ &error_ಸಂಕೇತ);

    /**
     * Destructor.
     * @stable ICU 4.8
     */
    ವಾಸ್ತವಿಕ ~ಬೈಟುಗಳು_Trie_Builder();

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
    ಬೈಟುಗಳು_Trie_Builder& ಕೂಡಿಸು(StringPiece s, int32_t ಮೌಲ್ಯ_2, UError_ಸಂಕೇತ& error_ಸಂಕೇತ) U_LIFETIME_BOUND;

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
    ಬೈಟುಗಳು_Trie *build(UStringTrieBuildOption buildOption, UError_ಸಂಕೇತ &error_ಸಂಕೇತ);

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
                                 UError_ಸಂಕೇತ& error_ಸಂಕೇತ) U_LIFETIME_BOUND;

    /**
     * Removes all (byte sequence, value) pairs.
     * New data can then be add()ed and a new trie can be built.
     * @return *this
     * @stable ICU 4.8
     */
    ಬೈಟುಗಳು_Trie_Builder& clear() U_LIFETIME_BOUND;

ಖಾಸಗಿ:
    ಮಿತ್ರ class ::ಬೈಟುಗಳು_Trie_Test;

    ಬೈಟುಗಳು_Trie_Builder(ಸ್ಥಿರಾಂಕ ಬೈಟುಗಳು_Trie_Builder &other) = delete;  // no copy constructor
    ಬೈಟುಗಳು_Trie_Builder &ಕ್ರಿಯಾಕಾರಕ=(ಸ್ಥಿರಾಂಕ ಬೈಟುಗಳು_Trie_Builder &other) = delete;  // no assignment operator

    ಶೂನ್ಯ build_ಬೈಟುಗಳು(UStringTrieBuildOption buildOption, UError_ಸಂಕೇತ &error_ಸಂಕೇತ);

    ವಾಸ್ತವಿಕ int32_t get_Element_String_ಉದ್ದಳತೆ(int32_t i) ಸ್ಥಿರಾಂಕ override;
    ವಾಸ್ತವಿಕ char16_t getElementUnit(int32_t i, int32_t byte_ಸೂಚ್ಯಂಕ) ಸ್ಥಿರಾಂಕ override;
    ವಾಸ್ತವಿಕ int32_t get_Element_ಮೌಲ್ಯ(int32_t i) ಸ್ಥಿರಾಂಕ override;

    ವಾಸ್ತವಿಕ int32_t get_ಎಲ್ಲೆ_ದ_ರೇಖೀಯ_ಹೊಂದಿಕೆ(int32_t ಮೊದಲ, int32_t ಕೊನೆಯ, int32_t byte_ಸೂಚ್ಯಂಕ) ಸ್ಥಿರಾಂಕ override;

    ವಾಸ್ತವಿಕ int32_t ಎಣಿಕೆ_Element_Units(int32_t ಆರಂಭ, int32_t ಎಲ್ಲೆ, int32_t byte_ಸೂಚ್ಯಂಕ) ಸ್ಥಿರಾಂಕ override;
    ವಾಸ್ತವಿಕ int32_t skipElementsBySomeUnits(int32_t i, int32_t byte_ಸೂಚ್ಯಂಕ, int32_t ಎಣಿಕೆ_2) ಸ್ಥಿರಾಂಕ override;
    ವಾಸ್ತವಿಕ int32_t ಸೂಚ್ಯಂಕ_ದ_Element_With_ಮುಂದಿನ_Unit(int32_t i, int32_t byte_ಸೂಚ್ಯಂಕ, char16_t byte) ಸ್ಥಿರಾಂಕ override;

    ವಾಸ್ತವಿಕ UBool ಹೊಂದಿಕೆ_Nodes_Can_Have_ಮೌಲ್ಯಗಳು() ಸ್ಥಿರಾಂಕ override { ಹಿಂತಿರುಗಿ false; }

    ವಾಸ್ತವಿಕ int32_t get_Max_Branch_ರೇಖೀಯ_Sub_ಗಂಟು_ಉದ್ದಳತೆ() ಸ್ಥಿರಾಂಕ override { ಹಿಂತಿರುಗಿ ಬೈಟುಗಳು_Trie::k_Max_Branch_ರೇಖೀಯ_Sub_ಗಂಟು_ಉದ್ದಳತೆ; }
    ವಾಸ್ತವಿಕ int32_t get_Min_ರೇಖೀಯ_ಹೊಂದಿಕೆ() ಸ್ಥಿರಾಂಕ override { ಹಿಂತಿರುಗಿ ಬೈಟುಗಳು_Trie::k_Min_ರೇಖೀಯ_ಹೊಂದಿಕೆ; }
    ವಾಸ್ತವಿಕ int32_t get_Max_ರೇಖೀಯ_ಹೊಂದಿಕೆ_ಉದ್ದಳತೆ() ಸ್ಥಿರಾಂಕ override { ಹಿಂತಿರುಗಿ ಬೈಟುಗಳು_Trie::k_Max_ರೇಖೀಯ_ಹೊಂದಿಕೆ_ಉದ್ದಳತೆ; }

    class BTLinear_ಹೊಂದಿಕೆ_ಗಂಟು : ಸಾರ್ವಜನಿಕ ರೇಖೀಯ_ಹೊಂದಿಕೆ_ಗಂಟು {
    ಸಾರ್ವಜನಿಕ:
        BTLinear_ಹೊಂದಿಕೆ_ಗಂಟು(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *units, int32_t len, ಗಂಟು_2 *ಮುಂದಿನ_ಗಂಟು);
        ವಾಸ್ತವಿಕ bool ಕ್ರಿಯಾಕಾರಕ==(ಸ್ಥಿರಾಂಕ ಗಂಟು_2 &other) ಸ್ಥಿರಾಂಕ override;
        ವಾಸ್ತವಿಕ ಶೂನ್ಯ ಲಿಖ(StringTrieBuilder &builder) override;
    ಖಾಸಗಿ:
        ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *s;
    };
    
    ವಾಸ್ತವಿಕ ಗಂಟು_2 *create_ರೇಖೀಯ_ಹೊಂದಿಕೆ_ಗಂಟು(int32_t i, int32_t byte_ಸೂಚ್ಯಂಕ, int32_t ಉದ್ದಳತೆ,
                                        ಗಂಟು_2 *ಮುಂದಿನ_ಗಂಟು) ಸ್ಥಿರಾಂಕ override;

    UBool ensure_ಸಾಮರ್ಥ್ಯ(int32_t ಉದ್ದಳತೆ);
    ವಾಸ್ತವಿಕ int32_t ಲಿಖ(int32_t byte) override;
    int32_t ಲಿಖ(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *b, int32_t ಉದ್ದಳತೆ);
    ವಾಸ್ತವಿಕ int32_t writeElementUnits(int32_t i, int32_t byte_ಸೂಚ್ಯಂಕ, int32_t ಉದ್ದಳತೆ) override;
    ವಾಸ್ತವಿಕ int32_t write_ಮೌಲ್ಯ_And_Final(int32_t i, UBool isFinal) override;
    ವಾಸ್ತವಿಕ int32_t write_ಮೌಲ್ಯ_And_Type(UBool has_ಮೌಲ್ಯ, int32_t ಮೌಲ್ಯ_2, int32_t ಗಂಟು) override;
    ವಾಸ್ತವಿಕ int32_t write_Delta_ಗಮ್ಯ(int32_t jump_ಗುರಿ) override;
    ಸ್ಥಿರ int32_t ಆಂತರಿಕ_Encode_Delta(int32_t i, ಅಕ್ಷರ int_ಬೈಟುಗಳು[]);

    CharString *ಅಕ್ಷರಸರಣಿಗಳು;  // Pointer not object so we need not #include internal charstr.h.
    ಬೈಟುಗಳು_Trie_Element *elements;
    int32_t elements_ಸಾಮರ್ಥ್ಯ;
    int32_t elements_ಉದ್ದಳತೆ;

    // Byte serialization of the trie.
    // Grows from the back: bytesLength measures from the end of the buffer!
    ಅಕ್ಷರ *ಬೈಟುಗಳು;
    int32_t ಬೈಟುಗಳು_ಸಾಮರ್ಥ್ಯ;
    int32_t ಬೈಟುಗಳು_ಉದ್ದಳತೆ;
};

U_NAMESPACE_ಕೊನೆ

#ಆದರೆ_ಅಂತ್ಯ /* U_SHOW_CPLUSPLUS_API */

#ಆದರೆ_ಅಂತ್ಯ  // __BYTESTRIEBUILDER_H__
