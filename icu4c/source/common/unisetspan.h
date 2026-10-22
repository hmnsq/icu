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

#جيڪڏهن_وصف_نه_ٿيل __UNISETSPAN_H__
#وصف_ڪريو __UNISETSPAN_H__

#ضمن "unicode/utypes.h"
#ضمن "unicode/uniset.h"

U_NAMESPACE_BEGIN

/*
 * Implement span() etc. for a set with strings.
 * Avoid recursion because of its exponential complexity.
 * Instead, try multiple paths at once and track them with an IndexList.
 */
class Unicode_سيٽ_String_وسعت : عام UMemory {
عام:
    /*
     * Which span() variant will be used?
     * The object is either built for one variant and used once,
     * or built for all and may be used many times.
     */
    تعداد {
        FWD             = 0x20,
        پٺ_2            = 0x10,
        UTF16           = 8,
        UTF8            = 4,
        CONTAINED       = 2,
        NOT_CONTAINED   = 1,

        ALL             = 0x3f,

        FWD_UTF16_CONTAINED     = FWD  | UTF16 |     CONTAINED,
        FWD_UTF16_NOT_CONTAINED = FWD  | UTF16 | NOT_CONTAINED,
        FWD_UTF8_CONTAINED      = FWD  | UTF8  |     CONTAINED,
        FWD_UTF8_NOT_CONTAINED  = FWD  | UTF8  | NOT_CONTAINED,
        پٺ_UTF16_CONTAINED    = پٺ_2 | UTF16 |     CONTAINED,
        پٺ_UTF16_NOT_CONTAINED= پٺ_2 | UTF16 | NOT_CONTAINED,
        پٺ_UTF8_CONTAINED     = پٺ_2 | UTF8  |     CONTAINED,
        پٺ_UTF8_NOT_CONTAINED = پٺ_2 | UTF8  | NOT_CONTAINED
    };

    Unicode_سيٽ_String_وسعت(ثابت Unicode_سيٽ &سيٽ, ثابت UVector &سيٽ_لڙيون, uint32_t which);

    // Copy constructor. Assumes which==ALL for a frozen set.
    Unicode_سيٽ_String_وسعت(ثابت Unicode_سيٽ_String_وسعت &other_String_وسعت, ثابت UVector &new_والد_سيٽ_لڙيون);

    ~Unicode_سيٽ_String_وسعت();

    /*
     * Do the strings need to be checked in span() etc.?
     * @return true if strings need to be checked (call span() here),
     *         false if not (use a BMPSet for best performance).
     */
    مضمن UBool needs_String_وسعت_UTF16();
    مضمن UBool needs_String_وسعت_UTF8();

    // For fast UnicodeSet::contains(c).
    مضمن UBool شامل_آهي(UChar32 c) ثابت;

    int32_t وسعت(ثابت char16_t *s, int32_t ڊيگهائي, USet_وسعت_Condition وسعت_Condition) ثابت;

    int32_t وسعت_پٺ(ثابت char16_t *s, int32_t ڊيگهائي, USet_وسعت_Condition وسعت_Condition) ثابت;

    int32_t وسعت_UTF8(ثابت uint8_t *s, int32_t ڊيگهائي, USet_وسعت_Condition وسعت_Condition) ثابت;

    int32_t وسعت_پٺ_UTF8(ثابت uint8_t *s, int32_t ڊيگهائي, USet_وسعت_Condition وسعت_Condition) ثابت;

خاص:
    // Special spanLength byte values.
    تعداد {
        // The spanLength is >=0xfe.
        LONG_وسعت=0xfe,
        // All code points in the string are contained in the parent set.
        ALL_CP_CONTAINED=0xff
    };

    // Add a starting or ending string character to the spanNotSet
    // so that a character span ends before any string.
    فراغ جوڙيو_منزل_وسعت_Not_سيٽ(UChar32 c);

    int32_t وسعت_Not(ثابت char16_t *s, int32_t ڊيگهائي) ثابت;
    int32_t وسعت_Not_پٺ(ثابت char16_t *s, int32_t ڊيگهائي) ثابت;
    int32_t وسعت_Not_UTF8(ثابت uint8_t *s, int32_t ڊيگهائي) ثابت;
    int32_t وسعت_Not_پٺ_UTF8(ثابت uint8_t *s, int32_t ڊيگهائي) ثابت;

    // Set for span(). Same as parent but without strings.
    Unicode_سيٽ وسعت_سيٽ;

    // Set for span(not contained).
    // Same as spanSet, plus characters that start or end strings.
    Unicode_سيٽ *p_وسعت_Not_سيٽ;

    // The strings of the parent set.
    ثابت UVector &لڙيون;

    // Pointer to the UTF-8 string lengths.
    // Also pointer to further allocated storage for meta data and
    // UTF-8 string contents as necessary.
    int32_t *utf8Lengths;

    // Pointer to the part of the (utf8Lengths) memory block that stores
    // the lengths of span(), spanBack() etc. for each string.
    uint8_t *وسعت_Lengths;

    // Pointer to the part of the (utf8Lengths) memory block that stores
    // the UTF-8 versions of the parent set's strings.
    uint8_t *utf8;

    // Number of bytes for all UTF-8 versions of strings together.
    int32_t utf8_ڊيگهائي;

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

UBool Unicode_سيٽ_String_وسعت::needs_String_وسعت_UTF16() {
    أرجع maxLength16 != 0;
}

UBool Unicode_سيٽ_String_وسعت::needs_String_وسعت_UTF8() {
    أرجع maxLength8 != 0;
}

UBool Unicode_سيٽ_String_وسعت::شامل_آهي(UChar32 c) ثابت {
    أرجع وسعت_سيٽ.شامل_آهي(c);
}

U_NAMESPACE_پڇاڙي

#جيڪڏهن_پورو
