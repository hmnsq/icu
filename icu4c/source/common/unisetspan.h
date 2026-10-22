// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
******************************************************************************
*
*   Copyright (C) 2007, International Business Machines
*   Corporation and others.  All Rights Reserved.
*
******************************************************************************
*   file name:  unisetspan.h
*   encoding:   UTF-8
*   tab size:   8 (not used)
*   indentation:4
*
*   created on: 2007mar01
*   created by: Markus W. Scherer
*/

#यदि_अपरिभाषितम् __UNISETSPAN_H__
#परिभाषय __UNISETSPAN_H__

#अन्तर्भाव "unicode/utypes.h"
#अन्तर्भाव "unicode/uniset.h"

U_NAMESPACE_BEGIN

/*
 * Implement span() etc. for a set with strings.
 * Avoid recursion because of its exponential complexity.
 * Instead, try multiple paths at once and track them with an IndexList.
 */
class Unicode_समुच्चयः_String_विस्तृतिः : सार्वजनिक UMemory {
सार्वजनिक:
    /*
     * Which span() variant will be used?
     * The object is either built for one variant and used once,
     * or built for all and may be used many times.
     */
    गणना {
        FWD             = 0x20,
        पृष्ठभागः_2            = 0x10,
        UTF16           = 8,
        UTF8            = 4,
        CONTAINED       = 2,
        NOT_CONTAINED   = 1,

        ALL             = 0x3f,

        FWD_UTF16_CONTAINED     = FWD  | UTF16 |     CONTAINED,
        FWD_UTF16_NOT_CONTAINED = FWD  | UTF16 | NOT_CONTAINED,
        FWD_UTF8_CONTAINED      = FWD  | UTF8  |     CONTAINED,
        FWD_UTF8_NOT_CONTAINED  = FWD  | UTF8  | NOT_CONTAINED,
        पृष्ठभागः_UTF16_CONTAINED    = पृष्ठभागः_2 | UTF16 |     CONTAINED,
        पृष्ठभागः_UTF16_NOT_CONTAINED= पृष्ठभागः_2 | UTF16 | NOT_CONTAINED,
        पृष्ठभागः_UTF8_CONTAINED     = पृष्ठभागः_2 | UTF8  |     CONTAINED,
        पृष्ठभागः_UTF8_NOT_CONTAINED = पृष्ठभागः_2 | UTF8  | NOT_CONTAINED
    };

    Unicode_समुच्चयः_String_विस्तृतिः(ध्रुव Unicode_समुच्चयः &समुच्चयः, ध्रुव UVector &समुच्चयः_वाक्यानि, uint32_t which);

    // Copy constructor. Assumes which==ALL for a frozen set.
    Unicode_समुच्चयः_String_विस्तृतिः(ध्रुव Unicode_समुच्चयः_String_विस्तृतिः &other_String_विस्तृतिः, ध्रुव UVector &new_Parent_समुच्चयः_वाक्यानि);

    ~Unicode_समुच्चयः_String_विस्तृतिः();

    /*
     * Do the strings need to be checked in span() etc.?
     * @return true if strings need to be checked (call span() here),
     *         false if not (use a BMPSet for best performance).
     */
    अन्तःस्थ UBool needs_String_विस्तृतिः_UTF16();
    अन्तःस्थ UBool needs_String_विस्तृतिः_UTF8();

    // For fast UnicodeSet::contains(c).
    अन्तःस्थ UBool अन्तर्भवति(UChar32 c) ध्रुव;

    int32_t विस्तृतिः(ध्रुव char16_t *s, int32_t दीर्घता, USet_विस्तृतिः_Condition विस्तृतिः_Condition) ध्रुव;

    int32_t विस्तृतिः_पृष्ठभागः(ध्रुव char16_t *s, int32_t दीर्घता, USet_विस्तृतिः_Condition विस्तृतिः_Condition) ध्रुव;

    int32_t विस्तृतिः_UTF8(ध्रुव uint8_t *s, int32_t दीर्घता, USet_विस्तृतिः_Condition विस्तृतिः_Condition) ध्रुव;

    int32_t विस्तृतिः_पृष्ठभागः_UTF8(ध्रुव uint8_t *s, int32_t दीर्घता, USet_विस्तृतिः_Condition विस्तृतिः_Condition) ध्रुव;

गुप्त:
    // Special spanLength byte values.
    गणना {
        // The spanLength is >=0xfe.
        LONG_विस्तृतिः=0xfe,
        // All code points in the string are contained in the parent set.
        ALL_CP_CONTAINED=0xff
    };

    // Add a starting or ending string character to the spanNotSet
    // so that a character span ends before any string.
    शून्य सङ्कलय_गन्तव्यम्_विस्तृतिः_Not_समुच्चयः(UChar32 c);

    int32_t विस्तृतिः_Not(ध्रुव char16_t *s, int32_t दीर्घता) ध्रुव;
    int32_t विस्तृतिः_Not_पृष्ठभागः(ध्रुव char16_t *s, int32_t दीर्घता) ध्रुव;
    int32_t विस्तृतिः_Not_UTF8(ध्रुव uint8_t *s, int32_t दीर्घता) ध्रुव;
    int32_t विस्तृतिः_Not_पृष्ठभागः_UTF8(ध्रुव uint8_t *s, int32_t दीर्घता) ध्रुव;

    // Set for span(). Same as parent but without strings.
    Unicode_समुच्चयः विस्तृतिः_समुच्चयः;

    // Set for span(not contained).
    // Same as spanSet, plus characters that start or end strings.
    Unicode_समुच्चयः *p_विस्तृतिः_Not_समुच्चयः;

    // The strings of the parent set.
    ध्रुव UVector &वाक्यानि;

    // Pointer to the UTF-8 string lengths.
    // Also pointer to further allocated storage for meta data and
    // UTF-8 string contents as necessary.
    int32_t *utf8Lengths;

    // Pointer to the part of the (utf8Lengths) memory block that stores
    // the lengths of span(), spanBack() etc. for each string.
    uint8_t *विस्तृतिः_Lengths;

    // Pointer to the part of the (utf8Lengths) memory block that stores
    // the UTF-8 versions of the parent set's strings.
    uint8_t *utf8;

    // Number of bytes for all UTF-8 versions of strings together.
    int32_t utf8_दीर्घता;

    // Maximum lengths of relevant strings.
    int32_t maxLength16;
    int32_t maxLength8;

    // Set up for all variants of span()?
    UBool all;

    // Memory for small numbers and lengths of strings.
    // For example, for 8 strings:
    // 8 UTF-8 lengths, 8*4 bytes span lengths, 8*2 3-byte UTF-8 characters
    // = 112 bytes = int32_t[28].
    int32_t staticLengths[32];
};

UBool Unicode_समुच्चयः_String_विस्तृतिः::needs_String_विस्तृतिः_UTF16() {
    प्रत्यागम maxLength16 != 0;
}

UBool Unicode_समुच्चयः_String_विस्तृतिः::needs_String_विस्तृतिः_UTF8() {
    प्रत्यागम maxLength8 != 0;
}

UBool Unicode_समुच्चयः_String_विस्तृतिः::अन्तर्भवति(UChar32 c) ध्रुव {
    प्रत्यागम विस्तृतिः_समुच्चयः.अन्तर्भवति(c);
}

U_NAMESPACE_अन्तः

#यदि_अन्तः
