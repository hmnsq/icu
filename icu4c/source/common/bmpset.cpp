// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
******************************************************************************
*
*   Copyright (C) 2007-2012, International Business Machines
*   Corporation and others.  All Rights Reserved.
*
******************************************************************************
*   file name:  bmpset.cpp
*   encoding:   UTF-8
*   tab size:   8 (not used)
*   indentation:4
*
*   created on: 2007jan29
*   created by: Markus W. Scherer
*/

#ضمن "unicode/utypes.h"
#ضمن "unicode/uniset.h"
#ضمن "unicode/utf8.h"
#ضمن "unicode/utf16.h"
#ضمن "cmemory.h"
#ضمن "bmpset.h"
#ضمن "uassert.h"

U_NAMESPACE_BEGIN

BMPSet::BMPSet(ثابت int32_t *پلار_List, int32_t پلار_List_اوږدتیا) :
        list(پلار_List), list_اوږدتیا(پلار_List_اوږدتیا) {
    uprv_memset(latin1_لري, 0, حجم(latin1_لري));
    uprv_memset(table7FF, 0, حجم(table7FF));
    uprv_memset(bmp_بلاک_بټونه, 0, حجم(bmp_بلاک_بټونه));

    /*
     * Set the list indexes for binary searches for
     * U+0800, U+1000, U+2000, .., U+F000, U+10000.
     * U+0800 is the first 3-byte-UTF-8 code point. Lower code points are
     * looked up in the bit tables.
     * The last pair of indexes is for finding supplementary code points.
     */
    list4kStarts[0]=پیدا_کړه_کوډ_ټکی(0x800, 0, list_اوږدتیا-1);
    int32_t i;
    لكل(i=1; i<=0x10; ++i) {
        list4kStarts[i]=پیدا_کړه_کوډ_ټکی(i<<12, list4kStarts[i-1], list_اوږدتیا-1);
    }
    list4kStarts[0x11]=list_اوږدتیا-1;
    لري_FFFD=لري_Slow(0xfffd, list4kStarts[0xf], list4kStarts[0x10]);

    init_بټونه();
    overrideIllegal();
}

BMPSet::BMPSet(ثابت BMPSet &otherBMPSet, ثابت int32_t *new_پلار_List, int32_t new_پلار_List_اوږدتیا) :
        لري_FFFD(otherBMPSet.لري_FFFD),
        list(new_پلار_List), list_اوږدتیا(new_پلار_List_اوږدتیا) {
    uprv_memcpy(latin1_لري, otherBMPSet.latin1_لري, حجم(latin1_لري));
    uprv_memcpy(table7FF, otherBMPSet.table7FF, حجم(table7FF));
    uprv_memcpy(bmp_بلاک_بټونه, otherBMPSet.bmp_بلاک_بټونه, حجم(bmp_بلاک_بټونه));
    uprv_memcpy(list4kStarts, otherBMPSet.list4kStarts, حجم(list4kStarts));
}

BMPSet::~BMPSet() {
}

/*
 * Set bits in a bit rectangle in "vertical" bit organization.
 * start<limit<=0x800
 */
ساكن فراغ set32x64_بټونه(uint32_t جدول[64], int32_t پیل, int32_t حد) {
    U_ASSERT(پیل<حد);
    U_ASSERT(حد<=0x800);

    int32_t lead=پیل>>6;  // Named for UTF-8 2-byte lead byte with upper 5 bits.
    int32_t trail=پیل&0x3f;  // Named for UTF-8 2-byte trail byte with lower 6 bits.

    // Set one bit indicating an all-one block.
    uint32_t بټونه = static_cast<uint32_t>(1) << lead;
    إذا((پیل+1)==حد) {  // Single-character shortcut.
        جدول[trail]|=بټونه;
        أرجع;
    }

    int32_t حد_Lead=حد>>6;
    int32_t حد_Trail=حد&0x3f;

    إذا(lead==حد_Lead) {
        // Partial vertical bit column.
        طالما(trail<حد_Trail) {
            جدول[trail++]|=بټونه;
        }
    } وإلا {
        // Partial vertical bit column,
        // followed by a bit rectangle,
        // followed by another partial vertical bit column.
        إذا(trail>0) {
            افعل {
                جدول[trail++]|=بټونه;
            } طالما(trail<64);
            ++lead;
        }
        إذا(lead<حد_Lead) {
            بټونه = ~((static_cast<بینښه>(1) << lead) - 1);
            إذا(حد_Lead<0x20) {
                بټونه &= (static_cast<بینښه>(1) << حد_Lead) - 1;
            }
            لكل(trail=0; trail<64; ++trail) {
                جدول[trail]|=بټونه;
            }
        }
        // limit<=0x800. If limit==0x800 then limitLead=32 and limitTrail=0.
        // In that case, bits=1<<limitLead is undefined but the bits value
        // is not used because trail<limitTrail is already false.
        بټونه = static_cast<uint32_t>(1) << ((حد_Lead == 0x20) ? (حد_Lead - 1) : حد_Lead);
        لكل(trail=0; trail<حد_Trail; ++trail) {
            جدول[trail]|=بټونه;
        }
    }
}

فراغ BMPSet::init_بټونه() {
    UChar32 پیل, حد;
    int32_t list_شاخص=0;

    // Set latin1Contains[].
    افعل {
        پیل=list[list_شاخص++];
        إذا(list_شاخص<list_اوږدتیا) {
            حد=list[list_شاخص++];
        } وإلا {
            حد=0x110000;
        }
        إذا(پیل>=0x100) {
            اكسر;
        }
        افعل {
            latin1_لري[پیل++]=1;
        } طالما(پیل<حد && پیل<0x100);
    } طالما(حد<=0x100);

    // Find the first range overlapping with (or after) 80..FF again,
    // to include them in table7FF as well.
    لكل(list_شاخص=0;;) {
        پیل=list[list_شاخص++];
        إذا(list_شاخص<list_اوږدتیا) {
            حد=list[list_شاخص++];
        } وإلا {
            حد=0x110000;
        }
        إذا(حد>0x80) {
            إذا(پیل<0x80) {
                پیل=0x80;
            }
            اكسر;
        }
    }

    // Set table7FF[].
    طالما(پیل<0x800) {
        set32x64_بټونه(table7FF, پیل, حد<=0x800 ? حد : 0x800);
        إذا(حد>0x800) {
            پیل=0x800;
            اكسر;
        }

        پیل=list[list_شاخص++];
        إذا(list_شاخص<list_اوږدتیا) {
            حد=list[list_شاخص++];
        } وإلا {
            حد=0x110000;
        }
    }

    // Set bmpBlockBits[].
    int32_t min_پیل=0x800;
    طالما(پیل<0x10000) {
        إذا(حد>0x10000) {
            حد=0x10000;
        }

        إذا(پیل<min_پیل) {
            پیل=min_پیل;
        }
        إذا(پیل<حد) {  // Else: Another range entirely in a known mixed-value block.
            إذا(پیل&0x3f) {
                // Mixed-value block of 64 code points.
                پیل>>=6;
                bmp_بلاک_بټونه[پیل&0x3f]|=0x10001<<(پیل>>6);
                پیل=(پیل+1)<<6;  // Round up to the next block boundary.
                min_پیل=پیل;      // Ignore further ranges in this block.
            }
            إذا(پیل<حد) {
                إذا(پیل<(حد&~0x3f)) {
                    // Multiple all-ones blocks of 64 code points each.
                    set32x64_بټونه(bmp_بلاک_بټونه, پیل>>6, حد>>6);
                }

                إذا(حد&0x3f) {
                    // Mixed-value block of 64 code points.
                    حد>>=6;
                    bmp_بلاک_بټونه[حد&0x3f]|=0x10001<<(حد>>6);
                    حد=(حد+1)<<6;  // Round up to the next block boundary.
                    min_پیل=حد;      // Ignore further ranges in this block.
                }
            }
        }

        إذا(حد==0x10000) {
            اكسر;
        }

        پیل=list[list_شاخص++];
        إذا(list_شاخص<list_اوږدتیا) {
            حد=list[list_شاخص++];
        } وإلا {
            حد=0x110000;
        }
    }
}

/*
 * Override some bits and bytes to the result of contains(FFFD)
 * for faster validity checking at runtime.
 * No need to set 0 values where they were reset to 0 in the constructor
 * and not modified by initBits().
 * (table7FF[] 0..7F, bmpBlockBits[] 0..7FF)
 * Need to set 0 values for surrogates D800..DFFF.
 */
فراغ BMPSet::overrideIllegal() {
    uint32_t بټونه, mask;
    int32_t i;

    إذا(لري_FFFD) {
        بټونه=3;                 // Lead bytes 0xC0 and 0xC1.
        لكل(i=0; i<64; ++i) {
            table7FF[i]|=بټونه;
        }

        بټونه=1;                 // Lead byte 0xE0.
        لكل(i=0; i<32; ++i) {   // First half of 4k block.
            bmp_بلاک_بټونه[i]|=بټونه;
        }

        mask= static_cast<uint32_t>(~(0x10001<<0xd));   // Lead byte 0xED.
        بټونه=1<<0xd;
        لكل(i=32; i<64; ++i) {  // Second half of 4k block.
            bmp_بلاک_بټونه[i]=(bmp_بلاک_بټونه[i]&mask)|بټونه;
        }
    } وإلا {
        mask= static_cast<uint32_t>(~(0x10001<<0xd));   // Lead byte 0xED.
        لكل(i=32; i<64; ++i) {  // Second half of 4k block.
            bmp_بلاک_بټونه[i]&=mask;
        }
    }
}

int32_t BMPSet::پیدا_کړه_کوډ_ټکی(UChar32 c, int32_t lo, int32_t hi) ثابت {
    /* Examples:
                                       findCodePoint(c)
       set              list[]         c=0 1 3 4 7 8
       ===              ==============   ===========
       []               [110000]         0 0 0 0 0 0
       [\u0000-\u0003]  [0, 4, 110000]   1 1 1 2 2 2
       [\u0004-\u0007]  [4, 8, 110000]   0 0 0 1 1 2
       [:Any:]          [0, 110000]      1 1 1 1 1 1
     */

    // Return the smallest i such that c < list[i].  Assume
    // list[len - 1] == HIGH and that c is legal (0..HIGH-1).
    إذا (c < list[lo])
        أرجع lo;
    // High runner test.  c is often after the last range, so an
    // initial check for this condition pays off.
    إذا (lo >= hi || c >= list[hi-1])
        أرجع hi;
    // invariant: c >= list[lo]
    // invariant: c < list[hi]
    لكل (;;) {
        int32_t i = (lo + hi) >> 1;
        إذا (i == lo) {
            اكسر; // Found!
        } وإلا إذا (c < list[i]) {
            hi = i;
        } وإلا {
            lo = i;
        }
    }
    أرجع hi;
}

UBool
BMPSet::لري(UChar32 c) ثابت {
    إذا (static_cast<uint32_t>(c) <= 0xff) {
        أرجع latin1_لري[c];
    } وإلا إذا (static_cast<uint32_t>(c) <= 0x7ff) {
        أرجع (table7FF[c & 0x3f] & (static_cast<uint32_t>(1) << (c >> 6))) != 0;
    } وإلا إذا (static_cast<uint32_t>(c) < 0xd800 || (c >= 0xe000 && c <= 0xffff)) {
        صحيح lead=c>>12;
        uint32_t two_بټونه=(bmp_بلاک_بټونه[(c>>6)&0x3f]>>lead)&0x10001;
        إذا(two_بټونه<=1) {
            // All 64 code points with the same bits 15..6
            // are either in the set or not.
            أرجع two_بټونه;
        } وإلا {
            // Look up the code point in its 4k block of code points.
            أرجع لري_Slow(c, list4kStarts[lead], list4kStarts[lead+1]);
        }
    } وإلا إذا (static_cast<uint32_t>(c) <= 0x10ffff) {
        // surrogate or supplementary code point
        أرجع لري_Slow(c, list4kStarts[0xd], list4kStarts[0x11]);
    } وإلا {
        // Out-of-range code points get false, consistent with long-standing
        // behavior of UnicodeSet::contains(c).
        أرجع خطأ;
    }
}

/*
 * Check for sufficient length for trail unit for each surrogate pair.
 * Handle single surrogates as surrogate code points as usual in ICU.
 */
ثابت char16_t *
BMPSet::پراختیا(ثابت char16_t *s, ثابت char16_t *حد, USet_پراختیا_Condition پراختیا_Condition) ثابت {
    char16_t c, c2;

    إذا(پراختیا_Condition) {
        //پراختیا
        افعل {
            c=*s;
            إذا(c<=0xff) {
                إذا(!latin1_لري[c]) {
                    اكسر;
                }
            } وإلا إذا(c<=0x7ff) {
                إذا ((table7FF[c & 0x3f] & (static_cast<uint32_t>(1) << (c >> 6))) == 0) {
                    اكسر;
                }
            } وإلا إذا(c<0xd800 || c>=0xe000) {
                صحيح lead=c>>12;
                uint32_t two_بټونه=(bmp_بلاک_بټونه[(c>>6)&0x3f]>>lead)&0x10001;
                إذا(two_بټونه<=1) {
                    // All 64 code points with the same bits 15..6
                    // are either in the set or not.
                    إذا(two_بټونه==0) {
                        اكسر;
                    }
                } وإلا {
                    // Look up the code point in its 4k block of code points.
                    إذا(!لري_Slow(c, list4kStarts[lead], list4kStarts[lead+1])) {
                        اكسر;
                    }
                }
            } وإلا إذا(c>=0xdc00 || (s+1)==حد || (c2=s[1])<0xdc00 || c2>=0xe000) {
                // surrogate code point
                إذا(!لري_Slow(c, list4kStarts[0xd], list4kStarts[0xe])) {
                    اكسر;
                }
            } وإلا {
                // surrogate pair
                إذا(!لري_Slow(U16_GET_SUPPLEMENTARY(c, c2), list4kStarts[0x10], list4kStarts[0x11])) {
                    اكسر;
                }
                ++s;
            }
        } طالما(++s<حد);
    } وإلا {
        // span not
        افعل {
            c=*s;
            إذا(c<=0xff) {
                إذا(latin1_لري[c]) {
                    اكسر;
                }
            } وإلا إذا(c<=0x7ff) {
                إذا ((table7FF[c & 0x3f] & (static_cast<uint32_t>(1) << (c >> 6))) != 0) {
                    اكسر;
                }
            } وإلا إذا(c<0xd800 || c>=0xe000) {
                صحيح lead=c>>12;
                uint32_t two_بټونه=(bmp_بلاک_بټونه[(c>>6)&0x3f]>>lead)&0x10001;
                إذا(two_بټونه<=1) {
                    // All 64 code points with the same bits 15..6
                    // are either in the set or not.
                    إذا(two_بټونه!=0) {
                        اكسر;
                    }
                } وإلا {
                    // Look up the code point in its 4k block of code points.
                    إذا(لري_Slow(c, list4kStarts[lead], list4kStarts[lead+1])) {
                        اكسر;
                    }
                }
            } وإلا إذا(c>=0xdc00 || (s+1)==حد || (c2=s[1])<0xdc00 || c2>=0xe000) {
                // surrogate code point
                إذا(لري_Slow(c, list4kStarts[0xd], list4kStarts[0xe])) {
                    اكسر;
                }
            } وإلا {
                // surrogate pair
                إذا(لري_Slow(U16_GET_SUPPLEMENTARY(c, c2), list4kStarts[0x10], list4kStarts[0x11])) {
                    اكسر;
                }
                ++s;
            }
        } طالما(++s<حد);
    }
    أرجع s;
}

/* Symmetrical with span(). */
ثابت char16_t *
BMPSet::پراختیا_شا(ثابت char16_t *s, ثابت char16_t *حد, USet_پراختیا_Condition پراختیا_Condition) ثابت {
    char16_t c, c2;

    إذا(پراختیا_Condition) {
        //پراختیا
        لكل(;;) {
            c=*(--حد);
            إذا(c<=0xff) {
                إذا(!latin1_لري[c]) {
                    اكسر;
                }
            } وإلا إذا(c<=0x7ff) {
                إذا ((table7FF[c & 0x3f] & (static_cast<uint32_t>(1) << (c >> 6))) == 0) {
                    اكسر;
                }
            } وإلا إذا(c<0xd800 || c>=0xe000) {
                صحيح lead=c>>12;
                uint32_t two_بټونه=(bmp_بلاک_بټونه[(c>>6)&0x3f]>>lead)&0x10001;
                إذا(two_بټونه<=1) {
                    // All 64 code points with the same bits 15..6
                    // are either in the set or not.
                    إذا(two_بټونه==0) {
                        اكسر;
                    }
                } وإلا {
                    // Look up the code point in its 4k block of code points.
                    إذا(!لري_Slow(c, list4kStarts[lead], list4kStarts[lead+1])) {
                        اكسر;
                    }
                }
            } وإلا إذا(c<0xdc00 || s==حد || (c2=*(حد-1))<0xd800 || c2>=0xdc00) {
                // surrogate code point
                إذا(!لري_Slow(c, list4kStarts[0xd], list4kStarts[0xe])) {
                    اكسر;
                }
            } وإلا {
                // surrogate pair
                إذا(!لري_Slow(U16_GET_SUPPLEMENTARY(c2, c), list4kStarts[0x10], list4kStarts[0x11])) {
                    اكسر;
                }
                --حد;
            }
            إذا(s==حد) {
                أرجع s;
            }
        }
    } وإلا {
        // span not
        لكل(;;) {
            c=*(--حد);
            إذا(c<=0xff) {
                إذا(latin1_لري[c]) {
                    اكسر;
                }
            } وإلا إذا(c<=0x7ff) {
                إذا ((table7FF[c & 0x3f] & (static_cast<uint32_t>(1) << (c >> 6))) != 0) {
                    اكسر;
                }
            } وإلا إذا(c<0xd800 || c>=0xe000) {
                صحيح lead=c>>12;
                uint32_t two_بټونه=(bmp_بلاک_بټونه[(c>>6)&0x3f]>>lead)&0x10001;
                إذا(two_بټونه<=1) {
                    // All 64 code points with the same bits 15..6
                    // are either in the set or not.
                    إذا(two_بټونه!=0) {
                        اكسر;
                    }
                } وإلا {
                    // Look up the code point in its 4k block of code points.
                    إذا(لري_Slow(c, list4kStarts[lead], list4kStarts[lead+1])) {
                        اكسر;
                    }
                }
            } وإلا إذا(c<0xdc00 || s==حد || (c2=*(حد-1))<0xd800 || c2>=0xdc00) {
                // surrogate code point
                إذا(لري_Slow(c, list4kStarts[0xd], list4kStarts[0xe])) {
                    اكسر;
                }
            } وإلا {
                // surrogate pair
                إذا(لري_Slow(U16_GET_SUPPLEMENTARY(c2, c), list4kStarts[0x10], list4kStarts[0x11])) {
                    اكسر;
                }
                --حد;
            }
            إذا(s==حد) {
                أرجع s;
            }
        }
    }
    أرجع حد+1;
}

/*
 * Precheck for sufficient trail bytes at end of string only once per span.
 * Check validity.
 */
ثابت uint8_t *
BMPSet::پراختیا_UTF8(ثابت uint8_t *s, int32_t length, USet_پراختیا_Condition پراختیا_Condition) ثابت {
    ثابت uint8_t *حد=s+length;
    uint8_t b=*s;
    إذا(U8_IS_SINGLE(b)) {
        // Initial all-ASCII span.
        إذا(پراختیا_Condition) {
            افعل {
                إذا(!latin1_لري[b] || ++s==حد) {
                    أرجع s;
                }
                b=*s;
            } طالما(U8_IS_SINGLE(b));
        } وإلا {
            افعل {
                إذا(latin1_لري[b] || ++s==حد) {
                    أرجع s;
                }
                b=*s;
            } طالما(U8_IS_SINGLE(b));
        }
        length = static_cast<int32_t>(حد - s);
    }

    إذا(پراختیا_Condition!=USET_پراختیا_NOT_CONTAINED) {
        پراختیا_Condition=USET_پراختیا_CONTAINED;  // Pin to 0/1 values.
    }

    ثابت uint8_t *limit0=حد;

    /*
     * Make sure that the last 1/2/3/4-byte sequence before limit is complete
     * or runs into a lead byte.
     * In the span loop compare s with limit only once
     * per multi-byte character.
     *
     * Give a trailing illegal sequence the same value as the result of contains(FFFD),
     * including it if that is part of the span, otherwise set limit0 to before
     * the truncated sequence.
     */
    b=*(حد-1);
    إذا (static_cast<int8_t>(b) < 0) {
        // b>=0x80: lead or trail byte
        إذا(b<0xc0) {
            // single trail byte, check for preceding 3- or 4-byte lead byte
            إذا(length>=2 && (b=*(حد-2))>=0xe0) {
                حد-=2;
                إذا(لري_FFFD!=پراختیا_Condition) {
                    limit0=حد;
                }
            } وإلا إذا(b<0xc0 && b>=0x80 && length>=3 && (b=*(حد-3))>=0xf0) {
                // 4-byte lead byte with only two trail bytes
                حد-=3;
                إذا(لري_FFFD!=پراختیا_Condition) {
                    limit0=حد;
                }
            }
        } وإلا {
            // lead byte with no trail bytes
            --حد;
            إذا(لري_FFFD!=پراختیا_Condition) {
                limit0=حد;
            }
        }
    }

    uint8_t t1, t2, t3;

    طالما(s<حد) {
        b=*s;
        إذا(U8_IS_SINGLE(b)) {
            // ASCII
            إذا(پراختیا_Condition) {
                افعل {
                    إذا(!latin1_لري[b]) {
                        أرجع s;
                    } وإلا إذا(++s==حد) {
                        أرجع limit0;
                    }
                    b=*s;
                } طالما(U8_IS_SINGLE(b));
            } وإلا {
                افعل {
                    إذا(latin1_لري[b]) {
                        أرجع s;
                    } وإلا إذا(++s==حد) {
                        أرجع limit0;
                    }
                    b=*s;
                } طالما(U8_IS_SINGLE(b));
            }
        }
        ++s;  // Advance past the lead byte.
        إذا(b>=0xe0) {
            إذا(b<0xf0) {
                إذا( /* handle U+0000..U+FFFF inline */
                    (t1 = static_cast<uint8_t>(s[0] - 0x80)) <= 0x3f &&
                    (t2 = static_cast<uint8_t>(s[1] - 0x80)) <= 0x3f
                ) {
                    b&=0xf;
                    uint32_t two_بټونه=(bmp_بلاک_بټونه[t1]>>b)&0x10001;
                    إذا(two_بټونه<=1) {
                        // All 64 code points with this lead byte and middle trail byte
                        // are either in the set or not.
                        إذا (two_بټونه != static_cast<uint32_t>(پراختیا_Condition)) {
                            أرجع s-1;
                        }
                    } وإلا {
                        // Look up the code point in its 4k block of code points.
                        UChar32 c=(b<<12)|(t1<<6)|t2;
                        إذا(لري_Slow(c, list4kStarts[b], list4kStarts[b+1]) != پراختیا_Condition) {
                            أرجع s-1;
                        }
                    }
                    s+=2;
                    استمر;
                }
            } وإلا إذا( /* handle U+10000..U+10FFFF inline */
                (t1 = static_cast<uint8_t>(s[0] - 0x80)) <= 0x3f &&
                (t2 = static_cast<uint8_t>(s[1] - 0x80)) <= 0x3f &&
                (t3 = static_cast<uint8_t>(s[2] - 0x80)) <= 0x3f
            ) {
                // Give an illegal sequence the same value as the result of contains(FFFD).
                UChar32 c = (static_cast<UChar32>(b - 0xf0) << 18) | (static_cast<UChar32>(t1) << 12) | (t2 << 6) | t3;
                إذا( (   (0x10000<=c && c<=0x10ffff) ?
                            لري_Slow(c, list4kStarts[0x10], list4kStarts[0x11]) :
                            لري_FFFD
                    ) != پراختیا_Condition
                ) {
                    أرجع s-1;
                }
                s+=3;
                استمر;
            }
        } وإلا {
            إذا( /* handle U+0000..U+07FF inline */
                b>=0xc0 &&
                (t1 = static_cast<uint8_t>(*s - 0x80)) <= 0x3f
            ) {
                إذا (static_cast<USet_پراختیا_Condition>((table7FF[t1] & (static_cast<uint32_t>(1) << (b & 0x1f))) != 0) != پراختیا_Condition) {
                    أرجع s-1;
                }
                ++s;
                استمر;
            }
        }

        // Give an illegal sequence the same value as the result of contains(FFFD).
        // Handle each byte of an illegal sequence separately to simplify the code;
        // no need to optimize error handling.
        إذا(لري_FFFD!=پراختیا_Condition) {
            أرجع s-1;
        }
    }

    أرجع limit0;
}

/*
 * While going backwards through UTF-8 optimize only for ASCII.
 * Unlike UTF-16, UTF-8 is not forward-backward symmetrical, that is, it is not
 * possible to tell from the last byte in a multi-byte sequence how many
 * preceding bytes there should be. Therefore, going backwards through UTF-8
 * is much harder than going forward.
 */
int32_t
BMPSet::پراختیا_شا_UTF8(ثابت uint8_t *s, int32_t length, USet_پراختیا_Condition پراختیا_Condition) ثابت {
    إذا(پراختیا_Condition!=USET_پراختیا_NOT_CONTAINED) {
        پراختیا_Condition=USET_پراختیا_CONTAINED;  // Pin to 0/1 values.
    }

    uint8_t b;

    افعل {
        b=s[--length];
        إذا(U8_IS_SINGLE(b)) {
            // ASCII sub-span
            إذا(پراختیا_Condition) {
                افعل {
                    إذا(!latin1_لري[b]) {
                        أرجع length+1;
                    } وإلا إذا(length==0) {
                        أرجع 0;
                    }
                    b=s[--length];
                } طالما(U8_IS_SINGLE(b));
            } وإلا {
                افعل {
                    إذا(latin1_لري[b]) {
                        أرجع length+1;
                    } وإلا إذا(length==0) {
                        أرجع 0;
                    }
                    b=s[--length];
                } طالما(U8_IS_SINGLE(b));
            }
        }

        int32_t prev=length;
        UChar32 c;
        // trail byte: collect a multi-byte character
        // (or  lead byte in last-trail position)
        c=utf8_prev_Char_خوندي_Body(s, 0, &length, b, -3);
        // c is a valid code point, not ASCII, not a surrogate
        إذا(c<=0x7ff) {
            إذا (static_cast<USet_پراختیا_Condition>((table7FF[c & 0x3f] & (static_cast<uint32_t>(1) << (c >> 6))) != 0) != پراختیا_Condition) {
                أرجع prev+1;
            }
        } وإلا إذا(c<=0xffff) {
            صحيح lead=c>>12;
            uint32_t two_بټونه=(bmp_بلاک_بټونه[(c>>6)&0x3f]>>lead)&0x10001;
            إذا(two_بټونه<=1) {
                // All 64 code points with the same bits 15..6
                // are either in the set or not.
                إذا (two_بټونه != static_cast<uint32_t>(پراختیا_Condition)) {
                    أرجع prev+1;
                }
            } وإلا {
                // Look up the code point in its 4k block of code points.
                إذا(لري_Slow(c, list4kStarts[lead], list4kStarts[lead+1]) != پراختیا_Condition) {
                    أرجع prev+1;
                }
            }
        } وإلا {
            إذا(لري_Slow(c, list4kStarts[0x10], list4kStarts[0x11]) != پراختیا_Condition) {
                أرجع prev+1;
            }
        }
    } طالما(length>0);
    أرجع 0;
}

U_NAMESPACE_پای_ټکی
