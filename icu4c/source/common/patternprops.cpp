// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
*******************************************************************************
*   Copyright (C) 2011, International Business Machines
*   Corporation and others.  All Rights Reserved.
*******************************************************************************
*   file name:  patternprops.cpp
*   encoding:   UTF-8
*   tab size:   8 (not used)
*   indentation:4
*
*   created on: 2011mar13
*   created by: Markus W. Scherer
*/

#अन्तर्भाव "unicode/utypes.h"
#अन्तर्भाव "patternprops.h"

U_NAMESPACE_BEGIN

/*
 * One byte per Latin-1 character.
 * Bit 0 is set if either Pattern property is true,
 * bit 1 if Pattern_Syntax is true,
 * bit 2 if Pattern_White_Space is true.
 * That is, Pattern_Syntax is encoded as 3 and Pattern_White_Space as 5.
 */
स्थिर ध्रुव uint8_t latin1[256]={
    // WS: 9..D
    0, 0, 0, 0, 0, 0, 0, 0, 0, 5, 5, 5, 5, 5, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    // WS: 20  Syntax: 21..2F
    5, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
    // Syntax: 3A..40
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 3, 3, 3, 3, 3,
    3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    // Syntax: 5B..5E
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 3, 3, 3, 0,
    // Syntax: 60
    3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    // Syntax: 7B..7E
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 3, 3, 3, 0,
    // WS: 85
    0, 0, 0, 0, 0, 5, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    // Syntax: A1..A7, A9, AB, AC, AE
    0, 3, 3, 3, 3, 3, 3, 3, 0, 3, 0, 3, 3, 0, 3, 0,
    // Syntax: B0, B1, B6, BB, BF
    3, 3, 0, 0, 0, 0, 3, 0, 0, 0, 0, 3, 0, 0, 0, 3,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    // Syntax: D7
    0, 0, 0, 0, 0, 0, 0, 3, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    // Syntax: F7
    0, 0, 0, 0, 0, 0, 0, 3, 0, 0, 0, 0, 0, 0, 0, 0
};

/*
 * One byte per 32 characters from U+2000..U+303F indexing into
 * a small table of 32-bit data words.
 * The first two data words are all-zeros and all-ones.
 */
स्थिर ध्रुव uint8_t index2000[130]={
    2, 3, 4, 0, 0, 0, 0, 0,  // 20xx
    0, 0, 0, 0, 5, 1, 1, 1,  // 21xx
    1, 1, 1, 1, 1, 1, 1, 1,  // 22xx
    1, 1, 1, 1, 1, 1, 1, 1,  // 23xx
    1, 1, 1, 0, 0, 0, 0, 0,  // 24xx
    1, 1, 1, 1, 1, 1, 1, 1,  // 25xx
    1, 1, 1, 1, 1, 1, 1, 1,  // 26xx
    1, 1, 1, 6, 7, 1, 1, 1,  // 27xx
    1, 1, 1, 1, 1, 1, 1, 1,  // 28xx
    1, 1, 1, 1, 1, 1, 1, 1,  // 29xx
    1, 1, 1, 1, 1, 1, 1, 1,  // 2Axx
    1, 1, 1, 1, 1, 1, 1, 1,  // 2Bxx
    0, 0, 0, 0, 0, 0, 0, 0,  // 2Cxx
    0, 0, 0, 0, 0, 0, 0, 0,  // 2Dxx
    1, 1, 1, 1, 0, 0, 0, 0,  // 2Exx
    0, 0, 0, 0, 0, 0, 0, 0,  // 2Fxx
    8, 9  // 3000..303F
};

/*
 * One 32-bit integer per 32 characters. Ranges of all-false and all-true
 * are mapped to the first two values, other ranges map to appropriate bit patterns.
 */
स्थिर ध्रुव uint32_t syntax2000[]={
    0,
    0xffffffff,
    0xffff0000,  // 2: 2010..201F
    0x7fff00ff,  // 3: 2020..2027, 2030..203E
    0x7feffffe,  // 4: 2041..2053, 2055..205E
    0xffff0000,  // 5: 2190..219F
    0x003fffff,  // 6: 2760..2775
    0xfff00000,  // 7: 2794..279F
    0xffffff0e,  // 8: 3001..3003, 3008..301F
    0x00010001   // 9: 3020, 3030
};

/*
 * Same as syntax2000, but with additional bits set for the
 * Pattern_White_Space characters 200E 200F 2028 2029.
 */
स्थिर ध्रुव uint32_t syntaxOrWhiteSpace2000[]={
    0,
    0xffffffff,
    0xffffc000,  // 2: 200E..201F
    0x7fff03ff,  // 3: 2020..2029, 2030..203E
    0x7feffffe,  // 4: 2041..2053, 2055..205E
    0xffff0000,  // 5: 2190..219F
    0x003fffff,  // 6: 2760..2775
    0xfff00000,  // 7: 2794..279F
    0xffffff0e,  // 8: 3001..3003, 3008..301F
    0x00010001   // 9: 3020, 3030
};

UBool
आदर्शः_Props::isSyntax(UChar32 c) {
    यदि(c<0) {
        प्रत्यागम असत्य;
    } अन्यथा यदि(c<=0xff) {
        प्रत्यागम (latin1[c] >> 1) & 1;
    } अन्यथा यदि(c<0x2010) {
        प्रत्यागम असत्य;
    } अन्यथा यदि(c<=0x3030) {
        uint32_t द्वयङ्काः=syntax2000[index2000[(c-0x2000)>>5]];
        प्रत्यागम (द्वयङ्काः >> (c & 0x1f)) & 1;
    } अन्यथा यदि(0xfd3e<=c && c<=0xfe46) {
        प्रत्यागम c<=0xfd3f || 0xfe45<=c;
    } अन्यथा {
        प्रत्यागम असत्य;
    }
}

UBool
आदर्शः_Props::isSyntaxOrWhiteSpace(UChar32 c) {
    यदि(c<0) {
        प्रत्यागम असत्य;
    } अन्यथा यदि(c<=0xff) {
        प्रत्यागम latin1[c] & 1;
    } अन्यथा यदि(c<0x200e) {
        प्रत्यागम असत्य;
    } अन्यथा यदि(c<=0x3030) {
        uint32_t द्वयङ्काः=syntaxOrWhiteSpace2000[index2000[(c-0x2000)>>5]];
        प्रत्यागम (द्वयङ्काः >> (c & 0x1f)) & 1;
    } अन्यथा यदि(0xfd3e<=c && c<=0xfe46) {
        प्रत्यागम c<=0xfd3f || 0xfe45<=c;
    } अन्यथा {
        प्रत्यागम असत्य;
    }
}

UBool
आदर्शः_Props::isWhiteSpace(UChar32 c) {
    यदि(c<0) {
        प्रत्यागम असत्य;
    } अन्यथा यदि(c<=0xff) {
        प्रत्यागम (latin1[c] >> 2) & 1;
    } अन्यथा यदि(0x200e<=c && c<=0x2029) {
        प्रत्यागम c<=0x200f || 0x2028<=c;
    } अन्यथा {
        प्रत्यागम असत्य;
    }
}

ध्रुव char16_t *
आदर्शः_Props::skipWhiteSpace(ध्रुव char16_t *s, int32_t length) {
    यावत्(length>0 && isWhiteSpace(*s)) {
        ++s;
        --length;
    }
    प्रत्यागम s;
}

int32_t
आदर्शः_Props::skipWhiteSpace(ध्रुव UnicodeString& s, int32_t start) {
    int32_t i = start;
    int32_t length = s.length();
    यावत्(i<length && isWhiteSpace(s.charAt(i))) {
        ++i;
    }
    प्रत्यागम i;
}

ध्रुव char16_t *
आदर्शः_Props::trimWhiteSpace(ध्रुव char16_t *s, int32_t &length) {
    यदि(length<=0 || (!isWhiteSpace(s[0]) && !isWhiteSpace(s[length-1]))) {
        प्रत्यागम s;
    }
    int32_t start=0;
    int32_t सीमा=length;
    यावत्(start<सीमा && isWhiteSpace(s[start])) {
        ++start;
    }
    यदि(start<सीमा) {
        // There is non-white space at start; we will not move limit below that,
        // so we need not test start<limit in the loop.
        यावत्(isWhiteSpace(s[सीमा-1])) {
            --सीमा;
        }
    }
    length=सीमा-start;
    प्रत्यागम s+start;
}

UBool
आदर्शः_Props::isIdentifier(ध्रुव char16_t *s, int32_t length) {
    यदि(length<=0) {
        प्रत्यागम असत्य;
    }
    ध्रुव char16_t *सीमा=s+length;
    कुरु {
        यदि(isSyntaxOrWhiteSpace(*s++)) {
            प्रत्यागम असत्य;
        }
    } यावत्(s<सीमा);
    प्रत्यागम सत्य;
}

ध्रुव char16_t *
आदर्शः_Props::skipIdentifier(ध्रुव char16_t *s, int32_t length) {
    यावत्(length>0 && !isSyntaxOrWhiteSpace(*s)) {
        ++s;
        --length;
    }
    प्रत्यागम s;
}

U_NAMESPACE_अन्तः
