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

#ಅನ್ತರ್ಭಾವ "unicode/utypes.h"
#ಅನ್ತರ್ಭಾವ "patternprops.h"

U_NAMESPACE_BEGIN

/*
 * One byte per Latin-1 character.
 * Bit 0 is set if either Pattern property is true,
 * bit 1 if Pattern_Syntax is true,
 * bit 2 if Pattern_White_Space is true.
 * That is, Pattern_Syntax is encoded as 3 and Pattern_White_Space as 5.
 */
ಸ್ಥಿರ ಧ್ರುವ uint8_t latin1[256]={
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
ಸ್ಥಿರ ಧ್ರುವ uint8_t index2000[130]={
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
ಸ್ಥಿರ ಧ್ರುವ uint32_t syntax2000[]={
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
ಸ್ಥಿರ ಧ್ರುವ uint32_t syntaxOrWhiteSpace2000[]={
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
PatternProps::isSyntax(UChar32 c) {
    ಯದಿ(c<0) {
        ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;
    } ಅನ್ಯಥಾ ಯದಿ(c<=0xff) {
        ಪ್ರತ್ಯಾಗಮ (latin1[c] >> 1) & 1;
    } ಅನ್ಯಥಾ ಯದಿ(c<0x2010) {
        ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;
    } ಅನ್ಯಥಾ ಯದಿ(c<=0x3030) {
        uint32_t bits=syntax2000[index2000[(c-0x2000)>>5]];
        ಪ್ರತ್ಯಾಗಮ (bits >> (c & 0x1f)) & 1;
    } ಅನ್ಯಥಾ ಯದಿ(0xfd3e<=c && c<=0xfe46) {
        ಪ್ರತ್ಯಾಗಮ c<=0xfd3f || 0xfe45<=c;
    } ಅನ್ಯಥಾ {
        ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;
    }
}

UBool
PatternProps::isSyntaxOrWhiteSpace(UChar32 c) {
    ಯದಿ(c<0) {
        ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;
    } ಅನ್ಯಥಾ ಯದಿ(c<=0xff) {
        ಪ್ರತ್ಯಾಗಮ latin1[c] & 1;
    } ಅನ್ಯಥಾ ಯದಿ(c<0x200e) {
        ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;
    } ಅನ್ಯಥಾ ಯದಿ(c<=0x3030) {
        uint32_t bits=syntaxOrWhiteSpace2000[index2000[(c-0x2000)>>5]];
        ಪ್ರತ್ಯಾಗಮ (bits >> (c & 0x1f)) & 1;
    } ಅನ್ಯಥಾ ಯದಿ(0xfd3e<=c && c<=0xfe46) {
        ಪ್ರತ್ಯಾಗಮ c<=0xfd3f || 0xfe45<=c;
    } ಅನ್ಯಥಾ {
        ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;
    }
}

UBool
PatternProps::isWhiteSpace(UChar32 c) {
    ಯದಿ(c<0) {
        ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;
    } ಅನ್ಯಥಾ ಯದಿ(c<=0xff) {
        ಪ್ರತ್ಯಾಗಮ (latin1[c] >> 2) & 1;
    } ಅನ್ಯಥಾ ಯದಿ(0x200e<=c && c<=0x2029) {
        ಪ್ರತ್ಯಾಗಮ c<=0x200f || 0x2028<=c;
    } ಅನ್ಯಥಾ {
        ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;
    }
}

ಧ್ರುವ char16_t *
PatternProps::skipWhiteSpace(ಧ್ರುವ char16_t *s, int32_t length) {
    ಯಾವತ್(length>0 && isWhiteSpace(*s)) {
        ++s;
        --length;
    }
    ಪ್ರತ್ಯಾಗಮ s;
}

int32_t
PatternProps::skipWhiteSpace(ಧ್ರುವ UnicodeString& s, int32_t start) {
    int32_t i = start;
    int32_t length = s.length();
    ಯಾವತ್(i<length && isWhiteSpace(s.charAt(i))) {
        ++i;
    }
    ಪ್ರತ್ಯಾಗಮ i;
}

ಧ್ರುವ char16_t *
PatternProps::trimWhiteSpace(ಧ್ರುವ char16_t *s, int32_t &length) {
    ಯದಿ(length<=0 || (!isWhiteSpace(s[0]) && !isWhiteSpace(s[length-1]))) {
        ಪ್ರತ್ಯಾಗಮ s;
    }
    int32_t start=0;
    int32_t limit=length;
    ಯಾವತ್(start<limit && isWhiteSpace(s[start])) {
        ++start;
    }
    ಯದಿ(start<limit) {
        // There is non-white space at start; we will not move limit below that,
        // so we need not test start<limit in the loop.
        ಯಾವತ್(isWhiteSpace(s[limit-1])) {
            --limit;
        }
    }
    length=limit-start;
    ಪ್ರತ್ಯಾಗಮ s+start;
}

UBool
PatternProps::isIdentifier(ಧ್ರುವ char16_t *s, int32_t length) {
    ಯದಿ(length<=0) {
        ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;
    }
    ಧ್ರುವ char16_t *limit=s+length;
    ಕುರು {
        ಯದಿ(isSyntaxOrWhiteSpace(*s++)) {
            ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;
        }
    } ಯಾವತ್(s<limit);
    ಪ್ರತ್ಯಾಗಮ ಸತ್ಯ;
}

ಧ್ರುವ char16_t *
PatternProps::skipIdentifier(ಧ್ರುವ char16_t *s, int32_t length) {
    ಯಾವತ್(length>0 && !isSyntaxOrWhiteSpace(*s)) {
        ++s;
        --length;
    }
    ಪ್ರತ್ಯಾಗಮ s;
}

U_NAMESPACE_END
