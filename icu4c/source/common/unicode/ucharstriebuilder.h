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

#यदि_अपरिभाषितम् __UCHARSTRIEBUILDER_H__
#परिभाषय __UCHARSTRIEBUILDER_H__

#अन्तर्भाव "unicode/utypes.h"

#यदि U_SHOW_CPLUSPLUS_API

#अन्तर्भाव "unicode/stringtriebuilder.h"
#अन्तर्भाव "unicode/ucharstrie.h"
#अन्तर्भाव "unicode/unistr.h"

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
class U_COMMON_API UCharsTrieBuilder : सार्वजनिक StringTrieBuilder {
सार्वजनिक:
    /**
     * Constructs an empty builder.
     * @param errorCode Standard ICU error code.
     * @stable ICU 4.8
     */
    UCharsTrieBuilder(UError_संहिता &error_संहिता);

    /**
     * Destructor.
     * @stable ICU 4.8
     */
    आभास ~UCharsTrieBuilder();

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
    UCharsTrieBuilder& सङ्कलय(ध्रुव UnicodeString& s,
                           int32_t मानम्_2,
                           UError_संहिता& error_संहिता) U_LIFETIME_BOUND;

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
    UCharsTrie *build(UStringTrieBuildOption buildOption, UError_संहिता &error_संहिता);

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
                                      UnicodeString& परिणाम U_LIFETIME_BOUND,
                                      UError_संहिता& error_संहिता) U_LIFETIME_BOUND;

    /**
     * Removes all (string, value) pairs.
     * New data can then be add()ed and a new trie can be built.
     * @return *this
     * @stable ICU 4.8
     */
    UCharsTrieBuilder& clear() U_LIFETIME_BOUND {
        वाक्यानि.remove();
        elements_दीर्घता=0;
        uchars_दीर्घता=0;
        प्रत्यागम *this;
    }

गुप्त:
    UCharsTrieBuilder(ध्रुव UCharsTrieBuilder &other) = delete;  // no copy constructor
    UCharsTrieBuilder &संक्रियकः=(ध्रुव UCharsTrieBuilder &other) = delete;  // no assignment operator

    शून्य buildUChars(UStringTrieBuildOption buildOption, UError_संहिता &error_संहिता);

    आभास int32_t get_Element_String_दीर्घता(int32_t i) ध्रुव override;
    आभास char16_t getElementUnit(int32_t i, int32_t unit_सूचकांक) ध्रुव override;
    आभास int32_t get_Element_मानम्(int32_t i) ध्रुव override;

    आभास int32_t get_सीमा_सम्बन्धि_रैखिकम्_मेलनम्(int32_t first, int32_t चरमः, int32_t unit_सूचकांक) ध्रुव override;

    आभास int32_t गणनम्_Element_Units(int32_t start, int32_t सीमा, int32_t unit_सूचकांक) ध्रुव override;
    आभास int32_t skipElementsBySomeUnits(int32_t i, int32_t unit_सूचकांक, int32_t गणनम्) ध्रुव override;
    आभास int32_t सूचकांक_सम्बन्धि_Element_With_अग्र_Unit(int32_t i, int32_t unit_सूचकांक, char16_t unit) ध्रुव override;

    आभास UBool मेलनम्_Nodes_Can_Have_मानानि() ध्रुव override { प्रत्यागम true; }

    आभास int32_t get_Max_Branch_रैखिकम्_Sub_ग्रन्थिः_दीर्घता() ध्रुव override { प्रत्यागम UCharsTrie::k_Max_Branch_रैखिकम्_Sub_ग्रन्थिः_दीर्घता; }
    आभास int32_t get_Min_रैखिकम्_मेलनम्() ध्रुव override { प्रत्यागम UCharsTrie::k_Min_रैखिकम्_मेलनम्; }
    आभास int32_t get_Max_रैखिकम्_मेलनम्_दीर्घता() ध्रुव override { प्रत्यागम UCharsTrie::k_Max_रैखिकम्_मेलनम्_दीर्घता; }

    class UCTLinear_मेलनम्_ग्रन्थिः : सार्वजनिक रैखिकम्_मेलनम्_ग्रन्थिः {
    सार्वजनिक:
        UCTLinear_मेलनम्_ग्रन्थिः(ध्रुव char16_t *units, int32_t len, ग्रन्थिः_2 *अग्र_ग्रन्थिः);
        आभास bool संक्रियकः==(ध्रुव ग्रन्थिः_2 &other) ध्रुव override;
        आभास शून्य लिख(StringTrieBuilder &builder) override;
    गुप्त:
        ध्रुव char16_t *s;
    };

    आभास ग्रन्थिः_2 *create_रैखिकम्_मेलनम्_ग्रन्थिः(int32_t i, int32_t unit_सूचकांक, int32_t दीर्घता,
                                        ग्रन्थिः_2 *अग्र_ग्रन्थिः) ध्रुव override;

    UBool ensure_क्षमता(int32_t दीर्घता);
    आभास int32_t लिख(int32_t unit) override;
    int32_t लिख(ध्रुव char16_t *s, int32_t दीर्घता);
    आभास int32_t writeElementUnits(int32_t i, int32_t unit_सूचकांक, int32_t दीर्घता) override;
    आभास int32_t write_मानम्_And_Final(int32_t i, UBool isFinal) override;
    आभास int32_t write_मानम्_And_Type(UBool has_मानम्, int32_t मानम्_2, int32_t ग्रन्थिः) override;
    आभास int32_t write_Delta_गन्तव्यम्(int32_t jumpTarget) override;

    UnicodeString वाक्यानि;
    UCharsTrieElement *elements;
    int32_t elements_क्षमता;
    int32_t elements_दीर्घता;

    // char16_t serialization of the trie.
    // Grows from the back: ucharsLength measures from the end of the buffer!
    char16_t *uchars;
    int32_t uchars_क्षमता;
    int32_t uchars_दीर्घता;
};

U_NAMESPACE_अन्तः

#यदि_अन्तः /* U_SHOW_CPLUSPLUS_API */

#यदि_अन्तः  // __UCHARSTRIEBUILDER_H__
