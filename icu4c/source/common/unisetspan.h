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

#నిర్వచితం_కాకపోతే __UNISETSPAN_H__
#నిర్వచించు __UNISETSPAN_H__

#అన్తర్భావ "unicode/utypes.h"
#అన్తర్భావ "unicode/uniset.h"

U_NAMESPACE_BEGIN

/*
 * Implement span() etc. for a set with strings.
 * Avoid recursion because of its exponential complexity.
 * Instead, try multiple paths at once and track them with an IndexList.
 */
class Unicode_సమితి_String_పరిధి : బహిరంగ UMemory {
బహిరంగ:
    /*
     * Which span() variant will be used?
     * The object is either built for one variant and used once,
     * or built for all and may be used many times.
     */
    గణన {
        FWD             = 0x20,
        వెనుక_2            = 0x10,
        UTF16           = 8,
        UTF8            = 4,
        CONTAINED       = 2,
        NOT_CONTAINED   = 1,

        ALL             = 0x3f,

        FWD_UTF16_CONTAINED     = FWD  | UTF16 |     CONTAINED,
        FWD_UTF16_NOT_CONTAINED = FWD  | UTF16 | NOT_CONTAINED,
        FWD_UTF8_CONTAINED      = FWD  | UTF8  |     CONTAINED,
        FWD_UTF8_NOT_CONTAINED  = FWD  | UTF8  | NOT_CONTAINED,
        వెనుక_UTF16_CONTAINED    = వెనుక_2 | UTF16 |     CONTAINED,
        వెనుక_UTF16_NOT_CONTAINED= వెనుక_2 | UTF16 | NOT_CONTAINED,
        వెనుక_UTF8_CONTAINED     = వెనుక_2 | UTF8  |     CONTAINED,
        వెనుక_UTF8_NOT_CONTAINED = వెనుక_2 | UTF8  | NOT_CONTAINED
    };

    Unicode_సమితి_String_పరిధి(స్థిరరాశి Unicode_సమితి &సమితి, స్థిరరాశి UVector &సమితి_అక్షరశ్రేణులు, uint32_t which);

    // Copy constructor. Assumes which==ALL for a frozen set.
    Unicode_సమితి_String_పరిధి(స్థిరరాశి Unicode_సమితి_String_పరిధి &other_String_పరిధి, స్థిరరాశి UVector &new_మాతృ_సమితి_అక్షరశ్రేణులు);

    ~Unicode_సమితి_String_పరిధి();

    /*
     * Do the strings need to be checked in span() etc.?
     * @return true if strings need to be checked (call span() here),
     *         false if not (use a BMPSet for best performance).
     */
    అంతఃవరుస UBool needs_String_పరిధి_UTF16();
    అంతఃవరుస UBool needs_String_పరిధి_UTF8();

    // For fast UnicodeSet::contains(c).
    అంతఃవరుస UBool కలిగియుంది(UChar32 c) స్థిరరాశి;

    int32_t పరిధి(స్థిరరాశి char16_t *s, int32_t నిడివి, USet_పరిధి_Condition పరిధి_Condition) స్థిరరాశి;

    int32_t పరిధి_వెనుక(స్థిరరాశి char16_t *s, int32_t నిడివి, USet_పరిధి_Condition పరిధి_Condition) స్థిరరాశి;

    int32_t పరిధి_UTF8(స్థిరరాశి uint8_t *s, int32_t నిడివి, USet_పరిధి_Condition పరిధి_Condition) స్థిరరాశి;

    int32_t పరిధి_వెనుక_UTF8(స్థిరరాశి uint8_t *s, int32_t నిడివి, USet_పరిధి_Condition పరిధి_Condition) స్థిరరాశి;

అంతరంగ:
    // Special spanLength byte values.
    గణన {
        // The spanLength is >=0xfe.
        LONG_పరిధి=0xfe,
        // All code points in the string are contained in the parent set.
        ALL_CP_CONTAINED=0xff
    };

    // Add a starting or ending string character to the spanNotSet
    // so that a character span ends before any string.
    శూన్య కూడు_గమ్యం_పరిధి_Not_సమితి(UChar32 c);

    int32_t పరిధి_Not(స్థిరరాశి char16_t *s, int32_t నిడివి) స్థిరరాశి;
    int32_t పరిధి_Not_వెనుక(స్థిరరాశి char16_t *s, int32_t నిడివి) స్థిరరాశి;
    int32_t పరిధి_Not_UTF8(స్థిరరాశి uint8_t *s, int32_t నిడివి) స్థిరరాశి;
    int32_t పరిధి_Not_వెనుక_UTF8(స్థిరరాశి uint8_t *s, int32_t నిడివి) స్థిరరాశి;

    // Set for span(). Same as parent but without strings.
    Unicode_సమితి పరిధి_సమితి;

    // Set for span(not contained).
    // Same as spanSet, plus characters that start or end strings.
    Unicode_సమితి *p_పరిధి_Not_సమితి;

    // The strings of the parent set.
    స్థిరరాశి UVector &అక్షరశ్రేణులు;

    // Pointer to the UTF-8 string lengths.
    // Also pointer to further allocated storage for meta data and
    // UTF-8 string contents as necessary.
    int32_t *utf8Lengths;

    // Pointer to the part of the (utf8Lengths) memory block that stores
    // the lengths of span(), spanBack() etc. for each string.
    uint8_t *పరిధి_Lengths;

    // Pointer to the part of the (utf8Lengths) memory block that stores
    // the UTF-8 versions of the parent set's strings.
    uint8_t *utf8;

    // Number of bytes for all UTF-8 versions of strings together.
    int32_t utf8_నిడివి;

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

UBool Unicode_సమితి_String_పరిధి::needs_String_పరిధి_UTF16() {
    తిరిగి maxLength16 != 0;
}

UBool Unicode_సమితి_String_పరిధి::needs_String_పరిధి_UTF8() {
    తిరిగి maxLength8 != 0;
}

UBool Unicode_సమితి_String_పరిధి::కలిగియుంది(UChar32 c) స్థిరరాశి {
    తిరిగి పరిధి_సమితి.కలిగియుంది(c);
}

U_NAMESPACE_చివర

#అయితే_ముగింపు
