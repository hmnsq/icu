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

#నిర్వచితం_కాకపోతే __BYTESTRIEBUILDER_H__
#నిర్వచించు __BYTESTRIEBUILDER_H__

#అన్తర్భావ "unicode/utypes.h"

#ఒకవేళ U_SHOW_CPLUSPLUS_API

#అన్తర్భావ "unicode/bytestrie.h"
#అన్తర్భావ "unicode/stringpiece.h"
#అన్తర్భావ "unicode/stringtriebuilder.h"

class బైట్లు_Trie_Test;

U_NAMESPACE_BEGIN

class బైట్లు_Trie_Element;
class CharString;
/**
 * Builder class for BytesTrie.
 *
 * This class is not intended for public subclassing.
 * @stable ICU 4.8
 */
class U_COMMON_API బైట్లు_Trie_Builder : బహిరంగ StringTrieBuilder {
బహిరంగ:
    /**
     * Constructs an empty builder.
     * @param errorCode Standard ICU error code.
     * @stable ICU 4.8
     */
    బైట్లు_Trie_Builder(UError_సంకేతం &error_సంకేతం);

    /**
     * Destructor.
     * @stable ICU 4.8
     */
    మాయా ~బైట్లు_Trie_Builder();

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
    బైట్లు_Trie_Builder& కూడు(StringPiece s, int32_t విలువ_2, UError_సంకేతం& error_సంకేతం) U_LIFETIME_BOUND;

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
    బైట్లు_Trie *build(UStringTrieBuildOption buildOption, UError_సంకేతం &error_సంకేతం);

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
                                 UError_సంకేతం& error_సంకేతం) U_LIFETIME_BOUND;

    /**
     * Removes all (byte sequence, value) pairs.
     * New data can then be add()ed and a new trie can be built.
     * @return *this
     * @stable ICU 4.8
     */
    బైట్లు_Trie_Builder& clear() U_LIFETIME_BOUND;

అంతరంగ:
    మిత్ర class ::బైట్లు_Trie_Test;

    బైట్లు_Trie_Builder(స్థిరరాశి బైట్లు_Trie_Builder &other) = delete;  // no copy constructor
    బైట్లు_Trie_Builder &క్రియాకారకం=(స్థిరరాశి బైట్లు_Trie_Builder &other) = delete;  // no assignment operator

    శూన్య build_బైట్లు(UStringTrieBuildOption buildOption, UError_సంకేతం &error_సంకేతం);

    మాయా int32_t get_Element_String_నిడివి(int32_t i) స్థిరరాశి override;
    మాయా char16_t getElementUnit(int32_t i, int32_t byte_సూచీ) స్థిరరాశి override;
    మాయా int32_t get_Element_విలువ(int32_t i) స్థిరరాశి override;

    మాయా int32_t get_హద్దు_యొక్క_రేఖీయ_సరిపోలిక(int32_t మొదటి, int32_t చివరి, int32_t byte_సూచీ) స్థిరరాశి override;

    మాయా int32_t లెక్క_Element_Units(int32_t ప్రారంభం, int32_t హద్దు, int32_t byte_సూచీ) స్థిరరాశి override;
    మాయా int32_t skipElementsBySomeUnits(int32_t i, int32_t byte_సూచీ, int32_t లెక్క) స్థిరరాశి override;
    మాయా int32_t సూచీ_యొక్క_Element_With_తదుపరి_Unit(int32_t i, int32_t byte_సూచీ, char16_t byte) స్థిరరాశి override;

    మాయా UBool సరిపోలిక_Nodes_Can_Have_విలువలు() స్థిరరాశి override { తిరిగి false; }

    మాయా int32_t get_Max_Branch_రేఖీయ_Sub_కణుపు_నిడివి() స్థిరరాశి override { తిరిగి బైట్లు_Trie::k_Max_Branch_రేఖీయ_Sub_కణుపు_నిడివి; }
    మాయా int32_t get_Min_రేఖీయ_సరిపోలిక() స్థిరరాశి override { తిరిగి బైట్లు_Trie::k_Min_రేఖీయ_సరిపోలిక; }
    మాయా int32_t get_Max_రేఖీయ_సరిపోలిక_నిడివి() స్థిరరాశి override { తిరిగి బైట్లు_Trie::k_Max_రేఖీయ_సరిపోలిక_నిడివి; }

    class BTLinear_సరిపోలిక_కణుపు : బహిరంగ రేఖీయ_సరిపోలిక_కణుపు {
    బహిరంగ:
        BTLinear_సరిపోలిక_కణుపు(స్థిరరాశి అక్షరం *units, int32_t len, కణుపు_2 *తదుపరి_కణుపు);
        మాయా bool క్రియాకారకం==(స్థిరరాశి కణుపు_2 &other) స్థిరరాశి override;
        మాయా శూన్య లిఖ(StringTrieBuilder &builder) override;
    అంతరంగ:
        స్థిరరాశి అక్షరం *s;
    };
    
    మాయా కణుపు_2 *create_రేఖీయ_సరిపోలిక_కణుపు(int32_t i, int32_t byte_సూచీ, int32_t నిడివి,
                                        కణుపు_2 *తదుపరి_కణుపు) స్థిరరాశి override;

    UBool ensure_సామర్థ్యం(int32_t నిడివి);
    మాయా int32_t లిఖ(int32_t byte) override;
    int32_t లిఖ(స్థిరరాశి అక్షరం *b, int32_t నిడివి);
    మాయా int32_t writeElementUnits(int32_t i, int32_t byte_సూచీ, int32_t నిడివి) override;
    మాయా int32_t write_విలువ_And_Final(int32_t i, UBool isFinal) override;
    మాయా int32_t write_విలువ_And_Type(UBool has_విలువ, int32_t విలువ_2, int32_t కణుపు) override;
    మాయా int32_t write_Delta_గమ్యం(int32_t jump_లక్ష్యం) override;
    స్థిర int32_t అంతర్గత_Encode_Delta(int32_t i, అక్షరం int_బైట్లు[]);

    CharString *అక్షరశ్రేణులు;  // Pointer not object so we need not #include internal charstr.h.
    బైట్లు_Trie_Element *elements;
    int32_t elements_సామర్థ్యం;
    int32_t elements_నిడివి;

    // Byte serialization of the trie.
    // Grows from the back: bytesLength measures from the end of the buffer!
    అక్షరం *బైట్లు;
    int32_t బైట్లు_సామర్థ్యం;
    int32_t బైట్లు_నిడివి;
};

U_NAMESPACE_చివర

#అయితే_ముగింపు /* U_SHOW_CPLUSPLUS_API */

#అయితే_ముగింపు  // __BYTESTRIEBUILDER_H__
