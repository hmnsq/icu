// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
*******************************************************************************
*   Copyright (C) 2010-2016, International Business Machines
*   Corporation and others.  All Rights Reserved.
*******************************************************************************
*   file name:  ucharstriebuilder.h
*   encoding:   UTF-8
*   tab size:   8 (not used)
*   indentation:4
*
*   created on: 2010nov14
*   created by: Markus W. Scherer
*/

#నిర్వచితం_కాకపోతే __UCHARSTRIEBUILDER_H__
#నిర్వచించు __UCHARSTRIEBUILDER_H__

#అన్తర్భావ "unicode/utypes.h"

#ఒకవేళ U_SHOW_CPLUSPLUS_API

#అన్తర్భావ "unicode/stringtriebuilder.h"
#అన్తర్భావ "unicode/ucharstrie.h"
#అన్తర్భావ "unicode/unistr.h"

/**
 * \file
 * \brief C++ API: Builder for icu::UCharsTrie
 */

U_NAMESPACE_BEGIN

class UCharsTrieElement;

/**
 * Builder class for UCharsTrie.
 *
 * This class is not intended for public subclassing.
 * @stable ICU 4.8
 */
class U_COMMON_API UCharsTrieBuilder : బహిరంగ StringTrieBuilder {
బహిరంగ:
    /**
     * Constructs an empty builder.
     * @param errorCode Standard ICU error code.
     * @stable ICU 4.8
     */
    UCharsTrieBuilder(UError_సంకేతం &error_సంకేతం);

    /**
     * Destructor.
     * @stable ICU 4.8
     */
    మాయా ~UCharsTrieBuilder();

    /**
     * Adds a (string, value) pair.
     * The string must be unique.
     * The string contents will be copied; the builder does not keep
     * a reference to the input UnicodeString or its buffer.
     * @param s The input string.
     * @param value The value associated with this string.
     * @param errorCode Standard ICU error code. Its input value must
     *                  pass the U_SUCCESS() test, or else the function returns
     *                  immediately. Check for U_FAILURE() on output or use with
     *                  function chaining. (See User Guide for details.)
     * @return *this
     * @stable ICU 4.8
     */
    UCharsTrieBuilder& కూడు(స్థిరరాశి UnicodeString& s,
                           int32_t విలువ_2,
                           UError_సంకేతం& error_సంకేతం) U_LIFETIME_BOUND;

    /**
     * Builds a UCharsTrie for the add()ed data.
     * Once built, no further data can be add()ed until clear() is called.
     *
     * A UCharsTrie cannot be empty. At least one (string, value) pair
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
     * @return A new UCharsTrie for the add()ed data.
     * @stable ICU 4.8
     */
    UCharsTrie *build(UStringTrieBuildOption buildOption, UError_సంకేతం &error_సంకేతం);

    /**
     * Builds a UCharsTrie for the add()ed data and char16_t-serializes it.
     * Once built, no further data can be add()ed until clear() is called.
     *
     * A UCharsTrie cannot be empty. At least one (string, value) pair
     * must have been add()ed.
     *
     * Multiple calls to buildUnicodeString() set the UnicodeStrings to the
     * builder's same char16_t array, without rebuilding.
     * If buildUnicodeString() is called after build(), the trie will be
     * re-serialized into a new array (because build() passes on ownership).
     * If build() is called after buildUnicodeString(), the trie object returned
     * by build() will become the owner of the underlying data for the
     * previously returned UnicodeString.
     * After clear() has been called, a new array will be used as well.
     * @param buildOption Build option, see UStringTrieBuildOption.
     * @param result A UnicodeString which will be set to the char16_t-serialized
     *               UCharsTrie for the add()ed data.
     * @param errorCode Standard ICU error code. Its input value must
     *                  pass the U_SUCCESS() test, or else the function returns
     *                  immediately. Check for U_FAILURE() on output or use with
     *                  function chaining. (See User Guide for details.)
     * @return result
     * @stable ICU 4.8
     */
    UnicodeString& buildUnicodeString(UStringTrieBuildOption buildOption,
                                      UnicodeString& ఫలితం U_LIFETIME_BOUND,
                                      UError_సంకేతం& error_సంకేతం) U_LIFETIME_BOUND;

    /**
     * Removes all (string, value) pairs.
     * New data can then be add()ed and a new trie can be built.
     * @return *this
     * @stable ICU 4.8
     */
    UCharsTrieBuilder& clear() U_LIFETIME_BOUND {
        అక్షరశ్రేణులు.remove();
        elements_నిడివి=0;
        uchars_నిడివి=0;
        తిరిగి *this;
    }

అంతరంగ:
    UCharsTrieBuilder(స్థిరరాశి UCharsTrieBuilder &other) = delete;  // no copy constructor
    UCharsTrieBuilder &క్రియాకారకం=(స్థిరరాశి UCharsTrieBuilder &other) = delete;  // no assignment operator

    శూన్య buildUChars(UStringTrieBuildOption buildOption, UError_సంకేతం &error_సంకేతం);

    మాయా int32_t get_Element_String_నిడివి(int32_t i) స్థిరరాశి override;
    మాయా char16_t getElementUnit(int32_t i, int32_t unit_సూచీ) స్థిరరాశి override;
    మాయా int32_t get_Element_విలువ(int32_t i) స్థిరరాశి override;

    మాయా int32_t get_హద్దు_యొక్క_రేఖీయ_సరిపోలిక(int32_t మొదటి, int32_t చివరి, int32_t unit_సూచీ) స్థిరరాశి override;

    మాయా int32_t లెక్క_Element_Units(int32_t ప్రారంభం, int32_t హద్దు, int32_t unit_సూచీ) స్థిరరాశి override;
    మాయా int32_t skipElementsBySomeUnits(int32_t i, int32_t unit_సూచీ, int32_t లెక్క) స్థిరరాశి override;
    మాయా int32_t సూచీ_యొక్క_Element_With_తదుపరి_Unit(int32_t i, int32_t unit_సూచీ, char16_t unit) స్థిరరాశి override;

    మాయా UBool సరిపోలిక_Nodes_Can_Have_విలువలు() స్థిరరాశి override { తిరిగి true; }

    మాయా int32_t get_Max_Branch_రేఖీయ_Sub_కణుపు_నిడివి() స్థిరరాశి override { తిరిగి UCharsTrie::k_Max_Branch_రేఖీయ_Sub_కణుపు_నిడివి; }
    మాయా int32_t get_Min_రేఖీయ_సరిపోలిక() స్థిరరాశి override { తిరిగి UCharsTrie::k_Min_రేఖీయ_సరిపోలిక; }
    మాయా int32_t get_Max_రేఖీయ_సరిపోలిక_నిడివి() స్థిరరాశి override { తిరిగి UCharsTrie::k_Max_రేఖీయ_సరిపోలిక_నిడివి; }

    class UCTLinear_సరిపోలిక_కణుపు : బహిరంగ రేఖీయ_సరిపోలిక_కణుపు {
    బహిరంగ:
        UCTLinear_సరిపోలిక_కణుపు(స్థిరరాశి char16_t *units, int32_t len, కణుపు_2 *తదుపరి_కణుపు);
        మాయా bool క్రియాకారకం==(స్థిరరాశి కణుపు_2 &other) స్థిరరాశి override;
        మాయా శూన్య లిఖ(StringTrieBuilder &builder) override;
    అంతరంగ:
        స్థిరరాశి char16_t *s;
    };

    మాయా కణుపు_2 *create_రేఖీయ_సరిపోలిక_కణుపు(int32_t i, int32_t unit_సూచీ, int32_t నిడివి,
                                        కణుపు_2 *తదుపరి_కణుపు) స్థిరరాశి override;

    UBool ensure_సామర్థ్యం(int32_t నిడివి);
    మాయా int32_t లిఖ(int32_t unit) override;
    int32_t లిఖ(స్థిరరాశి char16_t *s, int32_t నిడివి);
    మాయా int32_t writeElementUnits(int32_t i, int32_t unit_సూచీ, int32_t నిడివి) override;
    మాయా int32_t write_విలువ_And_Final(int32_t i, UBool isFinal) override;
    మాయా int32_t write_విలువ_And_Type(UBool has_విలువ, int32_t విలువ_2, int32_t కణుపు) override;
    మాయా int32_t write_Delta_గమ్యం(int32_t jump_లక్ష్యం) override;

    UnicodeString అక్షరశ్రేణులు;
    UCharsTrieElement *elements;
    int32_t elements_సామర్థ్యం;
    int32_t elements_నిడివి;

    // char16_t serialization of the trie.
    // Grows from the back: ucharsLength measures from the end of the buffer!
    char16_t *uchars;
    int32_t uchars_సామర్థ్యం;
    int32_t uchars_నిడివి;
};

U_NAMESPACE_చివర

#అయితే_ముగింపు /* U_SHOW_CPLUSPLUS_API */

#అయితే_ముగింపు  // __UCHARSTRIEBUILDER_H__
