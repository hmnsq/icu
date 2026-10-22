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

BMPSet::BMPSet(ثابت int32_t *والد_List, int32_t والد_List_ڊيگهائي) :
        list(والد_List), list_ڊيگهائي(والد_List_ڊيگهائي) {
    uprv_memset(latin1_شامل_آهي, 0, حجم(latin1_شامل_آهي));
    uprv_memset(table7FF, 0, حجم(table7FF));
    uprv_memset(bmp_بلاڪ_بٽ, 0, حجم(bmp_بلاڪ_بٽ));

    /*
     * Set the list indexes for binary searches for
     * U+0800, U+1000, U+2000, .., U+F000, U+10000.
     * U+0800 is the first 3-byte-UTF-8 code point. Lower code points are
     * looked up in the bit tables.
     * The last pair of indexes is for finding supplementary code points.
     */
    list4kStarts[0]=ڳوليو_ڪوڊ_نقطو(0x800, 0, list_ڊيگهائي-1);
    int32_t i;
    لكل(i=1; i<=0x10; ++i) {
        list4kStarts[i]=ڳوليو_ڪوڊ_نقطو(i<<12, list4kStarts[i-1], list_ڊيگهائي-1);
    }
    list4kStarts[0x11]=list_ڊيگهائي-1;
    شامل_آهي_FFFD=شامل_آهي_Slow(0xfffd, list4kStarts[0xf], list4kStarts[0x10]);

    init_بٽ();
    overrideIllegal();
}

BMPSet::BMPSet(ثابت BMPSet &otherBMPSet, ثابت int32_t *new_والد_List, int32_t new_والد_List_ڊيگهائي) :
        شامل_آهي_FFFD(otherBMPSet.شامل_آهي_FFFD),
        list(new_والد_List), list_ڊيگهائي(new_والد_List_ڊيگهائي) {
    uprv_memcpy(latin1_شامل_آهي, otherBMPSet.latin1_شامل_آهي, حجم(latin1_شامل_آهي));
    uprv_memcpy(table7FF, otherBMPSet.table7FF, حجم(table7FF));
    uprv_memcpy(bmp_بلاڪ_بٽ, otherBMPSet.bmp_بلاڪ_بٽ, حجم(bmp_بلاڪ_بٽ));
    uprv_memcpy(list4kStarts, otherBMPSet.list4kStarts, حجم(list4kStarts));
}

BMPSet::~BMPSet() {
}

/*
 * Set bits in a bit rectangle in "vertical" bit organization.
 * start<limit<=0x800
 */
ساكن فراغ set32x64_بٽ(uint32_t جدول[64], int32_t شروعات, int32_t حد_بندي) {
    U_ASSERT(شروعات<حد_بندي);
    U_ASSERT(حد_بندي<=0x800);

    int32_t lead=شروعات>>6;  // Named for UTF-8 2-byte lead byte with upper 5 bits.
    int32_t trail=شروعات&0x3f;  // Named for UTF-8 2-byte trail byte with lower 6 bits.

    // Set one bit indicating an all-one block.
    uint32_t بٽ = static_cast<uint32_t>(1) << lead;
    إذا((شروعات+1)==حد_بندي) {  // Single-character shortcut.
        جدول[trail]|=بٽ;
        أرجع;
    }

    int32_t حد_بندي_Lead=حد_بندي>>6;
    int32_t حد_بندي_Trail=حد_بندي&0x3f;

    إذا(lead==حد_بندي_Lead) {
        // Partial vertical bit column.
        طالما(trail<حد_بندي_Trail) {
            جدول[trail++]|=بٽ;
        }
    } وإلا {
        // Partial vertical bit column,
        // followed by a bit rectangle,
        // followed by another partial vertical bit column.
        إذا(trail>0) {
            افعل {
                جدول[trail++]|=بٽ;
            } طالما(trail<64);
            ++lead;
        }
        إذا(lead<حد_بندي_Lead) {
            بٽ = ~((static_cast<بنانشان>(1) << lead) - 1);
            إذا(حد_بندي_Lead<0x20) {
                بٽ &= (static_cast<بنانشان>(1) << حد_بندي_Lead) - 1;
            }
            لكل(trail=0; trail<64; ++trail) {
                جدول[trail]|=بٽ;
            }
        }
        // limit<=0x800. If limit==0x800 then limitLead=32 and limitTrail=0.
        // In that case, bits=1<<limitLead is undefined but the bits value
        // is not used because trail<limitTrail is already false.
        بٽ = static_cast<uint32_t>(1) << ((حد_بندي_Lead == 0x20) ? (حد_بندي_Lead - 1) : حد_بندي_Lead);
        لكل(trail=0; trail<حد_بندي_Trail; ++trail) {
            جدول[trail]|=بٽ;
        }
    }
}

فراغ BMPSet::init_بٽ() {
    UChar32 شروعات, حد_بندي;
    int32_t list_اشاريو=0;

    // Set latin1Contains[].
    افعل {
        شروعات=list[list_اشاريو++];
        إذا(list_اشاريو<list_ڊيگهائي) {
            حد_بندي=list[list_اشاريو++];
        } وإلا {
            حد_بندي=0x110000;
        }
        إذا(شروعات>=0x100) {
            اكسر;
        }
        افعل {
            latin1_شامل_آهي[شروعات++]=1;
        } طالما(شروعات<حد_بندي && شروعات<0x100);
    } طالما(حد_بندي<=0x100);

    // Find the first range overlapping with (or after) 80..FF again,
    // to include them in table7FF as well.
    لكل(list_اشاريو=0;;) {
        شروعات=list[list_اشاريو++];
        إذا(list_اشاريو<list_ڊيگهائي) {
            حد_بندي=list[list_اشاريو++];
        } وإلا {
            حد_بندي=0x110000;
        }
        إذا(حد_بندي>0x80) {
            إذا(شروعات<0x80) {
                شروعات=0x80;
            }
            اكسر;
        }
    }

    // Set table7FF[].
    طالما(شروعات<0x800) {
        set32x64_بٽ(table7FF, شروعات, حد_بندي<=0x800 ? حد_بندي : 0x800);
        إذا(حد_بندي>0x800) {
            شروعات=0x800;
            اكسر;
        }

        شروعات=list[list_اشاريو++];
        إذا(list_اشاريو<list_ڊيگهائي) {
            حد_بندي=list[list_اشاريو++];
        } وإلا {
            حد_بندي=0x110000;
        }
    }

    // Set bmpBlockBits[].
    int32_t min_شروعات=0x800;
    طالما(شروعات<0x10000) {
        إذا(حد_بندي>0x10000) {
            حد_بندي=0x10000;
        }

        إذا(شروعات<min_شروعات) {
            شروعات=min_شروعات;
        }
        إذا(شروعات<حد_بندي) {  // Else: Another range entirely in a known mixed-value block.
            إذا(شروعات&0x3f) {
                // Mixed-value block of 64 code points.
                شروعات>>=6;
                bmp_بلاڪ_بٽ[شروعات&0x3f]|=0x10001<<(شروعات>>6);
                شروعات=(شروعات+1)<<6;  // Round up to the next block boundary.
                min_شروعات=شروعات;      // Ignore further ranges in this block.
            }
            إذا(شروعات<حد_بندي) {
                إذا(شروعات<(حد_بندي&~0x3f)) {
                    // Multiple all-ones blocks of 64 code points each.
                    set32x64_بٽ(bmp_بلاڪ_بٽ, شروعات>>6, حد_بندي>>6);
                }

                إذا(حد_بندي&0x3f) {
                    // Mixed-value block of 64 code points.
                    حد_بندي>>=6;
                    bmp_بلاڪ_بٽ[حد_بندي&0x3f]|=0x10001<<(حد_بندي>>6);
                    حد_بندي=(حد_بندي+1)<<6;  // Round up to the next block boundary.
                    min_شروعات=حد_بندي;      // Ignore further ranges in this block.
                }
            }
        }

        إذا(حد_بندي==0x10000) {
            اكسر;
        }

        شروعات=list[list_اشاريو++];
        إذا(list_اشاريو<list_ڊيگهائي) {
            حد_بندي=list[list_اشاريو++];
        } وإلا {
            حد_بندي=0x110000;
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
    uint32_t بٽ, mask;
    int32_t i;

    إذا(شامل_آهي_FFFD) {
        بٽ=3;                 // Lead bytes 0xC0 and 0xC1.
        لكل(i=0; i<64; ++i) {
            table7FF[i]|=بٽ;
        }

        بٽ=1;                 // Lead byte 0xE0.
        لكل(i=0; i<32; ++i) {   // First half of 4k block.
            bmp_بلاڪ_بٽ[i]|=بٽ;
        }

        mask= static_cast<uint32_t>(~(0x10001<<0xd));   // Lead byte 0xED.
        بٽ=1<<0xd;
        لكل(i=32; i<64; ++i) {  // Second half of 4k block.
            bmp_بلاڪ_بٽ[i]=(bmp_بلاڪ_بٽ[i]&mask)|بٽ;
        }
    } وإلا {
        mask= static_cast<uint32_t>(~(0x10001<<0xd));   // Lead byte 0xED.
        لكل(i=32; i<64; ++i) {  // Second half of 4k block.
            bmp_بلاڪ_بٽ[i]&=mask;
        }
    }
}

int32_t BMPSet::ڳوليو_ڪوڊ_نقطو(UChar32 c, int32_t lo, int32_t hi) ثابت {
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
BMPSet::شامل_آهي(UChar32 c) ثابت {
    إذا (static_cast<uint32_t>(c) <= 0xff) {
        أرجع latin1_شامل_آهي[c];
    } وإلا إذا (static_cast<uint32_t>(c) <= 0x7ff) {
        أرجع (table7FF[c & 0x3f] & (static_cast<uint32_t>(1) << (c >> 6))) != 0;
    } وإلا إذا (static_cast<uint32_t>(c) < 0xd800 || (c >= 0xe000 && c <= 0xffff)) {
        صحيح lead=c>>12;
        uint32_t two_بٽ=(bmp_بلاڪ_بٽ[(c>>6)&0x3f]>>lead)&0x10001;
        إذا(two_بٽ<=1) {
            // All 64 code points with the same bits 15..6
            // are either in the set or not.
            أرجع two_بٽ;
        } وإلا {
            // Look up the code point in its 4k block of code points.
            أرجع شامل_آهي_Slow(c, list4kStarts[lead], list4kStarts[lead+1]);
        }
    } وإلا إذا (static_cast<uint32_t>(c) <= 0x10ffff) {
        // surrogate or supplementary code point
        أرجع شامل_آهي_Slow(c, list4kStarts[0xd], list4kStarts[0x11]);
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
BMPSet::وسعت(ثابت char16_t *s, ثابت char16_t *حد_بندي, USet_وسعت_Condition وسعت_Condition) ثابت {
    char16_t c, c2;

    إذا(وسعت_Condition) {
        //وسعت
        افعل {
            c=*s;
            إذا(c<=0xff) {
                إذا(!latin1_شامل_آهي[c]) {
                    اكسر;
                }
            } وإلا إذا(c<=0x7ff) {
                إذا ((table7FF[c & 0x3f] & (static_cast<uint32_t>(1) << (c >> 6))) == 0) {
                    اكسر;
                }
            } وإلا إذا(c<0xd800 || c>=0xe000) {
                صحيح lead=c>>12;
                uint32_t two_بٽ=(bmp_بلاڪ_بٽ[(c>>6)&0x3f]>>lead)&0x10001;
                إذا(two_بٽ<=1) {
                    // All 64 code points with the same bits 15..6
                    // are either in the set or not.
                    إذا(two_بٽ==0) {
                        اكسر;
                    }
                } وإلا {
                    // Look up the code point in its 4k block of code points.
                    إذا(!شامل_آهي_Slow(c, list4kStarts[lead], list4kStarts[lead+1])) {
                        اكسر;
                    }
                }
            } وإلا إذا(c>=0xdc00 || (s+1)==حد_بندي || (c2=s[1])<0xdc00 || c2>=0xe000) {
                // surrogate code point
                إذا(!شامل_آهي_Slow(c, list4kStarts[0xd], list4kStarts[0xe])) {
                    اكسر;
                }
            } وإلا {
                // surrogate pair
                إذا(!شامل_آهي_Slow(U16_GET_SUPPLEMENTARY(c, c2), list4kStarts[0x10], list4kStarts[0x11])) {
                    اكسر;
                }
                ++s;
            }
        } طالما(++s<حد_بندي);
    } وإلا {
        // span not
        افعل {
            c=*s;
            إذا(c<=0xff) {
                إذا(latin1_شامل_آهي[c]) {
                    اكسر;
                }
            } وإلا إذا(c<=0x7ff) {
                إذا ((table7FF[c & 0x3f] & (static_cast<uint32_t>(1) << (c >> 6))) != 0) {
                    اكسر;
                }
            } وإلا إذا(c<0xd800 || c>=0xe000) {
                صحيح lead=c>>12;
                uint32_t two_بٽ=(bmp_بلاڪ_بٽ[(c>>6)&0x3f]>>lead)&0x10001;
                إذا(two_بٽ<=1) {
                    // All 64 code points with the same bits 15..6
                    // are either in the set or not.
                    إذا(two_بٽ!=0) {
                        اكسر;
                    }
                } وإلا {
                    // Look up the code point in its 4k block of code points.
                    إذا(شامل_آهي_Slow(c, list4kStarts[lead], list4kStarts[lead+1])) {
                        اكسر;
                    }
                }
            } وإلا إذا(c>=0xdc00 || (s+1)==حد_بندي || (c2=s[1])<0xdc00 || c2>=0xe000) {
                // surrogate code point
                إذا(شامل_آهي_Slow(c, list4kStarts[0xd], list4kStarts[0xe])) {
                    اكسر;
                }
            } وإلا {
                // surrogate pair
                إذا(شامل_آهي_Slow(U16_GET_SUPPLEMENTARY(c, c2), list4kStarts[0x10], list4kStarts[0x11])) {
                    اكسر;
                }
                ++s;
            }
        } طالما(++s<حد_بندي);
    }
    أرجع s;
}

/* Symmetrical with span(). */
ثابت char16_t *
BMPSet::وسعت_پٺ(ثابت char16_t *s, ثابت char16_t *حد_بندي, USet_وسعت_Condition وسعت_Condition) ثابت {
    char16_t c, c2;

    إذا(وسعت_Condition) {
        //وسعت
        لكل(;;) {
            c=*(--حد_بندي);
            إذا(c<=0xff) {
                إذا(!latin1_شامل_آهي[c]) {
                    اكسر;
                }
            } وإلا إذا(c<=0x7ff) {
                إذا ((table7FF[c & 0x3f] & (static_cast<uint32_t>(1) << (c >> 6))) == 0) {
                    اكسر;
                }
            } وإلا إذا(c<0xd800 || c>=0xe000) {
                صحيح lead=c>>12;
                uint32_t two_بٽ=(bmp_بلاڪ_بٽ[(c>>6)&0x3f]>>lead)&0x10001;
                إذا(two_بٽ<=1) {
                    // All 64 code points with the same bits 15..6
                    // are either in the set or not.
                    إذا(two_بٽ==0) {
                        اكسر;
                    }
                } وإلا {
                    // Look up the code point in its 4k block of code points.
                    إذا(!شامل_آهي_Slow(c, list4kStarts[lead], list4kStarts[lead+1])) {
                        اكسر;
                    }
                }
            } وإلا إذا(c<0xdc00 || s==حد_بندي || (c2=*(حد_بندي-1))<0xd800 || c2>=0xdc00) {
                // surrogate code point
                إذا(!شامل_آهي_Slow(c, list4kStarts[0xd], list4kStarts[0xe])) {
                    اكسر;
                }
            } وإلا {
                // surrogate pair
                إذا(!شامل_آهي_Slow(U16_GET_SUPPLEMENTARY(c2, c), list4kStarts[0x10], list4kStarts[0x11])) {
                    اكسر;
                }
                --حد_بندي;
            }
            إذا(s==حد_بندي) {
                أرجع s;
            }
        }
    } وإلا {
        // span not
        لكل(;;) {
            c=*(--حد_بندي);
            إذا(c<=0xff) {
                إذا(latin1_شامل_آهي[c]) {
                    اكسر;
                }
            } وإلا إذا(c<=0x7ff) {
                إذا ((table7FF[c & 0x3f] & (static_cast<uint32_t>(1) << (c >> 6))) != 0) {
                    اكسر;
                }
            } وإلا إذا(c<0xd800 || c>=0xe000) {
                صحيح lead=c>>12;
                uint32_t two_بٽ=(bmp_بلاڪ_بٽ[(c>>6)&0x3f]>>lead)&0x10001;
                إذا(two_بٽ<=1) {
                    // All 64 code points with the same bits 15..6
                    // are either in the set or not.
                    إذا(two_بٽ!=0) {
                        اكسر;
                    }
                } وإلا {
                    // Look up the code point in its 4k block of code points.
                    إذا(شامل_آهي_Slow(c, list4kStarts[lead], list4kStarts[lead+1])) {
                        اكسر;
                    }
                }
            } وإلا إذا(c<0xdc00 || s==حد_بندي || (c2=*(حد_بندي-1))<0xd800 || c2>=0xdc00) {
                // surrogate code point
                إذا(شامل_آهي_Slow(c, list4kStarts[0xd], list4kStarts[0xe])) {
                    اكسر;
                }
            } وإلا {
                // surrogate pair
                إذا(شامل_آهي_Slow(U16_GET_SUPPLEMENTARY(c2, c), list4kStarts[0x10], list4kStarts[0x11])) {
                    اكسر;
                }
                --حد_بندي;
            }
            إذا(s==حد_بندي) {
                أرجع s;
            }
        }
    }
    أرجع حد_بندي+1;
}

/*
 * Precheck for sufficient trail bytes at end of string only once per span.
 * Check validity.
 */
ثابت uint8_t *
BMPSet::وسعت_UTF8(ثابت uint8_t *s, int32_t length, USet_وسعت_Condition وسعت_Condition) ثابت {
    ثابت uint8_t *حد_بندي=s+length;
    uint8_t b=*s;
    إذا(U8_IS_SINGLE(b)) {
        // Initial all-ASCII span.
        إذا(وسعت_Condition) {
            افعل {
                إذا(!latin1_شامل_آهي[b] || ++s==حد_بندي) {
                    أرجع s;
                }
                b=*s;
            } طالما(U8_IS_SINGLE(b));
        } وإلا {
            افعل {
                إذا(latin1_شامل_آهي[b] || ++s==حد_بندي) {
                    أرجع s;
                }
                b=*s;
            } طالما(U8_IS_SINGLE(b));
        }
        length = static_cast<int32_t>(حد_بندي - s);
    }

    إذا(وسعت_Condition!=USET_وسعت_NOT_CONTAINED) {
        وسعت_Condition=USET_وسعت_CONTAINED;  // Pin to 0/1 values.
    }

    ثابت uint8_t *limit0=حد_بندي;

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
    b=*(حد_بندي-1);
    إذا (static_cast<int8_t>(b) < 0) {
        // b>=0x80: lead or trail byte
        إذا(b<0xc0) {
            // single trail byte, check for preceding 3- or 4-byte lead byte
            إذا(length>=2 && (b=*(حد_بندي-2))>=0xe0) {
                حد_بندي-=2;
                إذا(شامل_آهي_FFFD!=وسعت_Condition) {
                    limit0=حد_بندي;
                }
            } وإلا إذا(b<0xc0 && b>=0x80 && length>=3 && (b=*(حد_بندي-3))>=0xf0) {
                // 4-byte lead byte with only two trail bytes
                حد_بندي-=3;
                إذا(شامل_آهي_FFFD!=وسعت_Condition) {
                    limit0=حد_بندي;
                }
            }
        } وإلا {
            // lead byte with no trail bytes
            --حد_بندي;
            إذا(شامل_آهي_FFFD!=وسعت_Condition) {
                limit0=حد_بندي;
            }
        }
    }

    uint8_t t1, t2, t3;

    طالما(s<حد_بندي) {
        b=*s;
        إذا(U8_IS_SINGLE(b)) {
            // ASCII
            إذا(وسعت_Condition) {
                افعل {
                    إذا(!latin1_شامل_آهي[b]) {
                        أرجع s;
                    } وإلا إذا(++s==حد_بندي) {
                        أرجع limit0;
                    }
                    b=*s;
                } طالما(U8_IS_SINGLE(b));
            } وإلا {
                افعل {
                    إذا(latin1_شامل_آهي[b]) {
                        أرجع s;
                    } وإلا إذا(++s==حد_بندي) {
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
                    uint32_t two_بٽ=(bmp_بلاڪ_بٽ[t1]>>b)&0x10001;
                    إذا(two_بٽ<=1) {
                        // All 64 code points with this lead byte and middle trail byte
                        // are either in the set or not.
                        إذا (two_بٽ != static_cast<uint32_t>(وسعت_Condition)) {
                            أرجع s-1;
                        }
                    } وإلا {
                        // Look up the code point in its 4k block of code points.
                        UChar32 c=(b<<12)|(t1<<6)|t2;
                        إذا(شامل_آهي_Slow(c, list4kStarts[b], list4kStarts[b+1]) != وسعت_Condition) {
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
                            شامل_آهي_Slow(c, list4kStarts[0x10], list4kStarts[0x11]) :
                            شامل_آهي_FFFD
                    ) != وسعت_Condition
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
                إذا (static_cast<USet_وسعت_Condition>((table7FF[t1] & (static_cast<uint32_t>(1) << (b & 0x1f))) != 0) != وسعت_Condition) {
                    أرجع s-1;
                }
                ++s;
                استمر;
            }
        }

        // Give an illegal sequence the same value as the result of contains(FFFD).
        // Handle each byte of an illegal sequence separately to simplify the code;
        // no need to optimize error handling.
        إذا(شامل_آهي_FFFD!=وسعت_Condition) {
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
BMPSet::وسعت_پٺ_UTF8(ثابت uint8_t *s, int32_t length, USet_وسعت_Condition وسعت_Condition) ثابت {
    إذا(وسعت_Condition!=USET_وسعت_NOT_CONTAINED) {
        وسعت_Condition=USET_وسعت_CONTAINED;  // Pin to 0/1 values.
    }

    uint8_t b;

    افعل {
        b=s[--length];
        إذا(U8_IS_SINGLE(b)) {
            // ASCII sub-span
            إذا(وسعت_Condition) {
                افعل {
                    إذا(!latin1_شامل_آهي[b]) {
                        أرجع length+1;
                    } وإلا إذا(length==0) {
                        أرجع 0;
                    }
                    b=s[--length];
                } طالما(U8_IS_SINGLE(b));
            } وإلا {
                افعل {
                    إذا(latin1_شامل_آهي[b]) {
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
        c=utf8_prev_Char_محفوظ_Body(s, 0, &length, b, -3);
        // c is a valid code point, not ASCII, not a surrogate
        إذا(c<=0x7ff) {
            إذا (static_cast<USet_وسعت_Condition>((table7FF[c & 0x3f] & (static_cast<uint32_t>(1) << (c >> 6))) != 0) != وسعت_Condition) {
                أرجع prev+1;
            }
        } وإلا إذا(c<=0xffff) {
            صحيح lead=c>>12;
            uint32_t two_بٽ=(bmp_بلاڪ_بٽ[(c>>6)&0x3f]>>lead)&0x10001;
            إذا(two_بٽ<=1) {
                // All 64 code points with the same bits 15..6
                // are either in the set or not.
                إذا (two_بٽ != static_cast<uint32_t>(وسعت_Condition)) {
                    أرجع prev+1;
                }
            } وإلا {
                // Look up the code point in its 4k block of code points.
                إذا(شامل_آهي_Slow(c, list4kStarts[lead], list4kStarts[lead+1]) != وسعت_Condition) {
                    أرجع prev+1;
                }
            }
        } وإلا {
            إذا(شامل_آهي_Slow(c, list4kStarts[0x10], list4kStarts[0x11]) != وسعت_Condition) {
                أرجع prev+1;
            }
        }
    } طالما(length>0);
    أرجع 0;
}

U_NAMESPACE_پڇاڙي
