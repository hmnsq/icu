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

#அந்தர்பாவ "unicode/utypes.h"
#அந்தர்பாவ "patternprops.h"

U_NAMESPACE_BEGIN

/*
 * One byte per Latin-1 character.
 * Bit 0 is set if either Pattern property is true,
 * bit 1 if Pattern_Syntax is true,
 * bit 2 if Pattern_White_Space is true.
 * That is, Pattern_Syntax is encoded as 3 and Pattern_White_Space as 5.
 */
நிலையான மாறிலி uint8_t latin1[256]={
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
நிலையான மாறிலி uint8_t index2000[130]={
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
நிலையான மாறிலி uint32_t syntax2000[]={
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
நிலையான மாறிலி uint32_t syntaxOrWhiteSpace2000[]={
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
    எனில்(c<0) {
        திரும்பு பொய்;
    } இல்லையெனில் எனில்(c<=0xff) {
        திரும்பு (latin1[c] >> 1) & 1;
    } இல்லையெனில் எனில்(c<0x2010) {
        திரும்பு பொய்;
    } இல்லையெனில் எனில்(c<=0x3030) {
        uint32_t bits=syntax2000[index2000[(c-0x2000)>>5]];
        திரும்பு (bits >> (c & 0x1f)) & 1;
    } இல்லையெனில் எனில்(0xfd3e<=c && c<=0xfe46) {
        திரும்பு c<=0xfd3f || 0xfe45<=c;
    } இல்லையெனில் {
        திரும்பு பொய்;
    }
}

UBool
PatternProps::isSyntaxOrWhiteSpace(UChar32 c) {
    எனில்(c<0) {
        திரும்பு பொய்;
    } இல்லையெனில் எனில்(c<=0xff) {
        திரும்பு latin1[c] & 1;
    } இல்லையெனில் எனில்(c<0x200e) {
        திரும்பு பொய்;
    } இல்லையெனில் எனில்(c<=0x3030) {
        uint32_t bits=syntaxOrWhiteSpace2000[index2000[(c-0x2000)>>5]];
        திரும்பு (bits >> (c & 0x1f)) & 1;
    } இல்லையெனில் எனில்(0xfd3e<=c && c<=0xfe46) {
        திரும்பு c<=0xfd3f || 0xfe45<=c;
    } இல்லையெனில் {
        திரும்பு பொய்;
    }
}

UBool
PatternProps::isWhiteSpace(UChar32 c) {
    எனில்(c<0) {
        திரும்பு பொய்;
    } இல்லையெனில் எனில்(c<=0xff) {
        திரும்பு (latin1[c] >> 2) & 1;
    } இல்லையெனில் எனில்(0x200e<=c && c<=0x2029) {
        திரும்பு c<=0x200f || 0x2028<=c;
    } இல்லையெனில் {
        திரும்பு பொய்;
    }
}

மாறிலி char16_t *
PatternProps::skipWhiteSpace(மாறிலி char16_t *s, int32_t length) {
    போது(length>0 && isWhiteSpace(*s)) {
        ++s;
        --length;
    }
    திரும்பு s;
}

int32_t
PatternProps::skipWhiteSpace(மாறிலி UnicodeString& s, int32_t start) {
    int32_t i = start;
    int32_t length = s.length();
    போது(i<length && isWhiteSpace(s.charAt(i))) {
        ++i;
    }
    திரும்பு i;
}

மாறிலி char16_t *
PatternProps::trimWhiteSpace(மாறிலி char16_t *s, int32_t &length) {
    எனில்(length<=0 || (!isWhiteSpace(s[0]) && !isWhiteSpace(s[length-1]))) {
        திரும்பு s;
    }
    int32_t start=0;
    int32_t limit=length;
    போது(start<limit && isWhiteSpace(s[start])) {
        ++start;
    }
    எனில்(start<limit) {
        // There is non-white space at start; we will not move limit below that,
        // so we need not test start<limit in the loop.
        போது(isWhiteSpace(s[limit-1])) {
            --limit;
        }
    }
    length=limit-start;
    திரும்பு s+start;
}

UBool
PatternProps::isIdentifier(மாறிலி char16_t *s, int32_t length) {
    எனில்(length<=0) {
        திரும்பு பொய்;
    }
    மாறிலி char16_t *limit=s+length;
    செய் {
        எனில்(isSyntaxOrWhiteSpace(*s++)) {
            திரும்பு பொய்;
        }
    } போது(s<limit);
    திரும்பு உண்மை;
}

மாறிலி char16_t *
PatternProps::skipIdentifier(மாறிலி char16_t *s, int32_t length) {
    போது(length>0 && !isSyntaxOrWhiteSpace(*s)) {
        ++s;
        --length;
    }
    திரும்பு s;
}

U_NAMESPACE_END
