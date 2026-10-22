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

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ __UNISETSPAN_H__
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ __UNISETSPAN_H__

#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/utypes.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/uniset.h"

U_NAMESPACE_BEGIN

/*
 * Implement span() etc. for a set with strings.
 * Avoid recursion because of its exponential complexity.
 * Instead, try multiple paths at once and track them with an IndexList.
 */
class Unicode_ਸੈੱਟ_String_ਵਿਸਤਾਰ : ਜਨਤਕ UMemory {
ਜਨਤਕ:
    /*
     * Which span() variant will be used?
     * The object is either built for one variant and used once,
     * or built for all and may be used many times.
     */
    ਗਿਣਤੀ {
        FWD             = 0x20,
        ਪਿੱਠ_2            = 0x10,
        UTF16           = 8,
        UTF8            = 4,
        CONTAINED       = 2,
        NOT_CONTAINED   = 1,

        ALL             = 0x3f,

        FWD_UTF16_CONTAINED     = FWD  | UTF16 |     CONTAINED,
        FWD_UTF16_NOT_CONTAINED = FWD  | UTF16 | NOT_CONTAINED,
        FWD_UTF8_CONTAINED      = FWD  | UTF8  |     CONTAINED,
        FWD_UTF8_NOT_CONTAINED  = FWD  | UTF8  | NOT_CONTAINED,
        ਪਿੱਠ_UTF16_CONTAINED    = ਪਿੱਠ_2 | UTF16 |     CONTAINED,
        ਪਿੱਠ_UTF16_NOT_CONTAINED= ਪਿੱਠ_2 | UTF16 | NOT_CONTAINED,
        ਪਿੱਠ_UTF8_CONTAINED     = ਪਿੱਠ_2 | UTF8  |     CONTAINED,
        ਪਿੱਠ_UTF8_NOT_CONTAINED = ਪਿੱਠ_2 | UTF8  | NOT_CONTAINED
    };

    Unicode_ਸੈੱਟ_String_ਵਿਸਤਾਰ(ਅਚਲ Unicode_ਸੈੱਟ &ਸੈੱਟ, ਅਚਲ UVector &ਸੈੱਟ_ਲੜੀਆਂ, uint32_t which);

    // Copy constructor. Assumes which==ALL for a frozen set.
    Unicode_ਸੈੱਟ_String_ਵਿਸਤਾਰ(ਅਚਲ Unicode_ਸੈੱਟ_String_ਵਿਸਤਾਰ &other_String_ਵਿਸਤਾਰ, ਅਚਲ UVector &new_ਮਾਪਾ_ਸੈੱਟ_ਲੜੀਆਂ);

    ~Unicode_ਸੈੱਟ_String_ਵਿਸਤਾਰ();

    /*
     * Do the strings need to be checked in span() etc.?
     * @return true if strings need to be checked (call span() here),
     *         false if not (use a BMPSet for best performance).
     */
    ਸਤਰਅੰਦਰ UBool needs_String_ਵਿਸਤਾਰ_UTF16();
    ਸਤਰਅੰਦਰ UBool needs_String_ਵਿਸਤਾਰ_UTF8();

    // For fast UnicodeSet::contains(c).
    ਸਤਰਅੰਦਰ UBool ਸ਼ਾਮਲ_ਹੈ(UChar32 c) ਅਚਲ;

    int32_t ਵਿਸਤਾਰ(ਅਚਲ char16_t *s, int32_t ਲੰਮਾਈ, USet_ਵਿਸਤਾਰ_Condition ਵਿਸਤਾਰ_Condition) ਅਚਲ;

    int32_t ਵਿਸਤਾਰ_ਪਿੱਠ(ਅਚਲ char16_t *s, int32_t ਲੰਮਾਈ, USet_ਵਿਸਤਾਰ_Condition ਵਿਸਤਾਰ_Condition) ਅਚਲ;

    int32_t ਵਿਸਤਾਰ_UTF8(ਅਚਲ uint8_t *s, int32_t ਲੰਮਾਈ, USet_ਵਿਸਤਾਰ_Condition ਵਿਸਤਾਰ_Condition) ਅਚਲ;

    int32_t ਵਿਸਤਾਰ_ਪਿੱਠ_UTF8(ਅਚਲ uint8_t *s, int32_t ਲੰਮਾਈ, USet_ਵਿਸਤਾਰ_Condition ਵਿਸਤਾਰ_Condition) ਅਚਲ;

ਨਿਜੀ:
    // Special spanLength byte values.
    ਗਿਣਤੀ {
        // The spanLength is >=0xfe.
        LONG_ਵਿਸਤਾਰ=0xfe,
        // All code points in the string are contained in the parent set.
        ALL_CP_CONTAINED=0xff
    };

    // Add a starting or ending string character to the spanNotSet
    // so that a character span ends before any string.
    ਖਾਲੀ ਜੋੜੋ_ਮੰਜ਼ਿਲ_ਵਿਸਤਾਰ_Not_ਸੈੱਟ(UChar32 c);

    int32_t ਵਿਸਤਾਰ_Not(ਅਚਲ char16_t *s, int32_t ਲੰਮਾਈ) ਅਚਲ;
    int32_t ਵਿਸਤਾਰ_Not_ਪਿੱਠ(ਅਚਲ char16_t *s, int32_t ਲੰਮਾਈ) ਅਚਲ;
    int32_t ਵਿਸਤਾਰ_Not_UTF8(ਅਚਲ uint8_t *s, int32_t ਲੰਮਾਈ) ਅਚਲ;
    int32_t ਵਿਸਤਾਰ_Not_ਪਿੱਠ_UTF8(ਅਚਲ uint8_t *s, int32_t ਲੰਮਾਈ) ਅਚਲ;

    // Set for span(). Same as parent but without strings.
    Unicode_ਸੈੱਟ ਵਿਸਤਾਰ_ਸੈੱਟ;

    // Set for span(not contained).
    // Same as spanSet, plus characters that start or end strings.
    Unicode_ਸੈੱਟ *p_ਵਿਸਤਾਰ_Not_ਸੈੱਟ;

    // The strings of the parent set.
    ਅਚਲ UVector &ਲੜੀਆਂ;

    // Pointer to the UTF-8 string lengths.
    // Also pointer to further allocated storage for meta data and
    // UTF-8 string contents as necessary.
    int32_t *utf8Lengths;

    // Pointer to the part of the (utf8Lengths) memory block that stores
    // the lengths of span(), spanBack() etc. for each string.
    uint8_t *ਵਿਸਤਾਰ_Lengths;

    // Pointer to the part of the (utf8Lengths) memory block that stores
    // the UTF-8 versions of the parent set's strings.
    uint8_t *utf8;

    // Number of bytes for all UTF-8 versions of strings together.
    int32_t utf8_ਲੰਮਾਈ;

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

UBool Unicode_ਸੈੱਟ_String_ਵਿਸਤਾਰ::needs_String_ਵਿਸਤਾਰ_UTF16() {
    ਵਾਪਸ maxLength16 != 0;
}

UBool Unicode_ਸੈੱਟ_String_ਵਿਸਤਾਰ::needs_String_ਵਿਸਤਾਰ_UTF8() {
    ਵਾਪਸ maxLength8 != 0;
}

UBool Unicode_ਸੈੱਟ_String_ਵਿਸਤਾਰ::ਸ਼ਾਮਲ_ਹੈ(UChar32 c) ਅਚਲ {
    ਵਾਪਸ ਵਿਸਤਾਰ_ਸੈੱਟ.ਸ਼ਾਮਲ_ਹੈ(c);
}

U_NAMESPACE_ਸਿਰਾ

#ਜੇ_ਅੰਤ
