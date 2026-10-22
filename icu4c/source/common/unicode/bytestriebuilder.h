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

#यदि_अपरिभाषितम् __BYTESTRIEBUILDER_H__
#परिभाषय __BYTESTRIEBUILDER_H__

#अन्तर्भाव "unicode/utypes.h"

#यदि U_SHOW_CPLUSPLUS_API

#अन्तर्भाव "unicode/bytestrie.h"
#अन्तर्भाव "unicode/stringpiece.h"
#अन्तर्भाव "unicode/stringtriebuilder.h"

class अष्टकानि_Trie_Test;

U_NAMESPACE_BEGIN

class अष्टकानि_Trie_Element;
class CharString;
/**
 * Builder class for BytesTrie.
 *
 * This class is not intended for public subclassing.
 * @stable ICU 4.8
 */
class U_COMMON_API अष्टकानि_Trie_Builder : सार्वजनिक StringTrieBuilder {
सार्वजनिक:
    /**
     * Constructs an empty builder.
     * @param errorCode Standard ICU error code.
     * @stable ICU 4.8
     */
    अष्टकानि_Trie_Builder(UError_संहिता &error_संहिता);

    /**
     * Destructor.
     * @stable ICU 4.8
     */
    आभास ~अष्टकानि_Trie_Builder();

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
    अष्टकानि_Trie_Builder& सङ्कलय(StringPiece s, int32_t मानम्_2, UError_संहिता& error_संहिता) U_LIFETIME_BOUND;

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
    अष्टकानि_Trie *build(UStringTrieBuildOption buildOption, UError_संहिता &error_संहिता);

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
                                 UError_संहिता& error_संहिता) U_LIFETIME_BOUND;

    /**
     * Removes all (byte sequence, value) pairs.
     * New data can then be add()ed and a new trie can be built.
     * @return *this
     * @stable ICU 4.8
     */
    अष्टकानि_Trie_Builder& clear() U_LIFETIME_BOUND;

गुप्त:
    मित्रम् class ::अष्टकानि_Trie_Test;

    अष्टकानि_Trie_Builder(ध्रुव अष्टकानि_Trie_Builder &other) = delete;  // no copy constructor
    अष्टकानि_Trie_Builder &संक्रियकः=(ध्रुव अष्टकानि_Trie_Builder &other) = delete;  // no assignment operator

    शून्य build_अष्टकानि(UStringTrieBuildOption buildOption, UError_संहिता &error_संहिता);

    आभास int32_t get_Element_String_दीर्घता(int32_t i) ध्रुव override;
    आभास char16_t getElementUnit(int32_t i, int32_t byte_सूचकांक) ध्रुव override;
    आभास int32_t get_Element_मानम्(int32_t i) ध्रुव override;

    आभास int32_t get_सीमा_सम्बन्धि_रैखिकम्_मेलनम्(int32_t first, int32_t चरमः, int32_t byte_सूचकांक) ध्रुव override;

    आभास int32_t गणनम्_Element_Units(int32_t start, int32_t सीमा, int32_t byte_सूचकांक) ध्रुव override;
    आभास int32_t skipElementsBySomeUnits(int32_t i, int32_t byte_सूचकांक, int32_t गणनम्) ध्रुव override;
    आभास int32_t सूचकांक_सम्बन्धि_Element_With_अग्र_Unit(int32_t i, int32_t byte_सूचकांक, char16_t byte) ध्रुव override;

    आभास UBool मेलनम्_Nodes_Can_Have_मानानि() ध्रुव override { प्रत्यागम false; }

    आभास int32_t get_Max_Branch_रैखिकम्_Sub_ग्रन्थिः_दीर्घता() ध्रुव override { प्रत्यागम अष्टकानि_Trie::k_Max_Branch_रैखिकम्_Sub_ग्रन्थिः_दीर्घता; }
    आभास int32_t get_Min_रैखिकम्_मेलनम्() ध्रुव override { प्रत्यागम अष्टकानि_Trie::k_Min_रैखिकम्_मेलनम्; }
    आभास int32_t get_Max_रैखिकम्_मेलनम्_दीर्घता() ध्रुव override { प्रत्यागम अष्टकानि_Trie::k_Max_रैखिकम्_मेलनम्_दीर्घता; }

    class BTLinear_मेलनम्_ग्रन्थिः : सार्वजनिक रैखिकम्_मेलनम्_ग्रन्थिः {
    सार्वजनिक:
        BTLinear_मेलनम्_ग्रन्थिः(ध्रुव वर्ण *units, int32_t len, ग्रन्थिः_2 *अग्र_ग्रन्थिः);
        आभास bool संक्रियकः==(ध्रुव ग्रन्थिः_2 &other) ध्रुव override;
        आभास शून्य लिख(StringTrieBuilder &builder) override;
    गुप्त:
        ध्रुव वर्ण *s;
    };
    
    आभास ग्रन्थिः_2 *create_रैखिकम्_मेलनम्_ग्रन्थिः(int32_t i, int32_t byte_सूचकांक, int32_t दीर्घता,
                                        ग्रन्थिः_2 *अग्र_ग्रन्थिः) ध्रुव override;

    UBool ensure_क्षमता(int32_t दीर्घता);
    आभास int32_t लिख(int32_t byte) override;
    int32_t लिख(ध्रुव वर्ण *b, int32_t दीर्घता);
    आभास int32_t writeElementUnits(int32_t i, int32_t byte_सूचकांक, int32_t दीर्घता) override;
    आभास int32_t write_मानम्_And_Final(int32_t i, UBool isFinal) override;
    आभास int32_t write_मानम्_And_Type(UBool has_मानम्, int32_t मानम्_2, int32_t ग्रन्थिः) override;
    आभास int32_t write_Delta_गन्तव्यम्(int32_t jumpTarget) override;
    स्थिर int32_t आन्तरिकम्_Encode_Delta(int32_t i, वर्ण int_अष्टकानि[]);

    CharString *वाक्यानि;  // Pointer not object so we need not #include internal charstr.h.
    अष्टकानि_Trie_Element *elements;
    int32_t elements_क्षमता;
    int32_t elements_दीर्घता;

    // Byte serialization of the trie.
    // Grows from the back: bytesLength measures from the end of the buffer!
    वर्ण *अष्टकानि;
    int32_t अष्टकानि_क्षमता;
    int32_t अष्टकानि_दीर्घता;
};

U_NAMESPACE_अन्तः

#यदि_अन्तः /* U_SHOW_CPLUSPLUS_API */

#यदि_अन्तः  // __BYTESTRIEBUILDER_H__
