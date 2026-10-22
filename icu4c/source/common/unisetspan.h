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

#ifndef __UNISETSPAN_H__
#define __UNISETSPAN_H__

#अन्तर्भाव "unicode/utypes.h"
#अन्तर्भाव "unicode/uniset.h"

U_NAMESPACE_BEGIN

/*
 * Implement span() etc. for a set with strings.
 * Avoid recursion because of its exponential complexity.
 * Instead, try multiple paths at once and track them with an IndexList.
 */
class UnicodeSetStringSpan : सार्वजनिक UMemory {
सार्वजनिक:
    /*
     * Which span() variant will be used?
     * The object is either built for one variant and used once,
     * or built for all and may be used many times.
     */
    गणना {
        FWD             = 0x20,
        BACK            = 0x10,
        UTF16           = 8,
        UTF8            = 4,
        CONTAINED       = 2,
        NOT_CONTAINED   = 1,

        ALL             = 0x3f,

        FWD_UTF16_CONTAINED     = FWD  | UTF16 |     CONTAINED,
        FWD_UTF16_NOT_CONTAINED = FWD  | UTF16 | NOT_CONTAINED,
        FWD_UTF8_CONTAINED      = FWD  | UTF8  |     CONTAINED,
        FWD_UTF8_NOT_CONTAINED  = FWD  | UTF8  | NOT_CONTAINED,
        BACK_UTF16_CONTAINED    = BACK | UTF16 |     CONTAINED,
        BACK_UTF16_NOT_CONTAINED= BACK | UTF16 | NOT_CONTAINED,
        BACK_UTF8_CONTAINED     = BACK | UTF8  |     CONTAINED,
        BACK_UTF8_NOT_CONTAINED = BACK | UTF8  | NOT_CONTAINED
    };

    UnicodeSetStringSpan(ध्रुव UnicodeSet &set, ध्रुव UVector &setStrings, uint32_t which);

    // Copy constructor. Assumes which==ALL for a frozen set.
    UnicodeSetStringSpan(ध्रुव UnicodeSetStringSpan &otherStringSpan, ध्रुव UVector &newParentSetStrings);

    ~UnicodeSetStringSpan();

    /*
     * Do the strings need to be checked in span() etc.?
     * @return true if strings need to be checked (call span() here),
     *         false if not (use a BMPSet for best performance).
     */
    अन्तःस्थ UBool needsStringSpanUTF16();
    अन्तःस्थ UBool needsStringSpanUTF8();

    // For fast UnicodeSet::contains(c).
    अन्तःस्थ UBool contains(UChar32 c) ध्रुव;

    int32_t span(ध्रुव char16_t *s, int32_t length, USetSpanCondition spanCondition) ध्रुव;

    int32_t spanBack(ध्रुव char16_t *s, int32_t length, USetSpanCondition spanCondition) ध्रुव;

    int32_t spanUTF8(ध्रुव uint8_t *s, int32_t length, USetSpanCondition spanCondition) ध्रुव;

    int32_t spanBackUTF8(ध्रुव uint8_t *s, int32_t length, USetSpanCondition spanCondition) ध्रुव;

गुप्त:
    // Special spanLength byte values.
    गणना {
        // The spanLength is >=0xfe.
        LONG_SPAN=0xfe,
        // All code points in the string are contained in the parent set.
        ALL_CP_CONTAINED=0xff
    };

    // Add a starting or ending string character to the spanNotSet
    // so that a character span ends before any string.
    शून्य addToSpanNotSet(UChar32 c);

    int32_t spanNot(ध्रुव char16_t *s, int32_t length) ध्रुव;
    int32_t spanNotBack(ध्रुव char16_t *s, int32_t length) ध्रुव;
    int32_t spanNotUTF8(ध्रुव uint8_t *s, int32_t length) ध्रुव;
    int32_t spanNotBackUTF8(ध्रुव uint8_t *s, int32_t length) ध्रुव;

    // Set for span(). Same as parent but without strings.
    UnicodeSet spanSet;

    // Set for span(not contained).
    // Same as spanSet, plus characters that start or end strings.
    UnicodeSet *pSpanNotSet;

    // The strings of the parent set.
    ध्रुव UVector &strings;

    // Pointer to the UTF-8 string lengths.
    // Also pointer to further allocated storage for meta data and
    // UTF-8 string contents as necessary.
    int32_t *utf8Lengths;

    // Pointer to the part of the (utf8Lengths) memory block that stores
    // the lengths of span(), spanBack() etc. for each string.
    uint8_t *spanLengths;

    // Pointer to the part of the (utf8Lengths) memory block that stores
    // the UTF-8 versions of the parent set's strings.
    uint8_t *utf8;

    // Number of bytes for all UTF-8 versions of strings together.
    int32_t utf8Length;

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

UBool UnicodeSetStringSpan::needsStringSpanUTF16() {
    प्रत्यागम maxLength16 != 0;
}

UBool UnicodeSetStringSpan::needsStringSpanUTF8() {
    प्रत्यागम maxLength8 != 0;
}

UBool UnicodeSetStringSpan::contains(UChar32 c) ध्रुव {
    प्रत्यागम spanSet.contains(c);
}

U_NAMESPACE_END

#endif
