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

#شامل "unicode/utypes.h"
#شامل "unicode/uniset.h"
#شامل "unicode/utf8.h"
#شامل "unicode/utf16.h"
#شامل "cmemory.h"
#شامل "bmpset.h"
#شامل "uassert.h"

U_NAMESPACE_BEGIN

BMPSet::BMPSet(ثابت int32_t *والد_List, int32_t والد_List_لمبائی) :
        list(والد_List), list_لمبائی(والد_List_لمبائی) {
    uprv_memset(latin1_شامل_ہے, 0, حجم(latin1_شامل_ہے));
    uprv_memset(table7FF, 0, حجم(table7FF));
    uprv_memset(bmp_بلاک_بٹ, 0, حجم(bmp_بلاک_بٹ));

    /*
     * Set the list indexes for binary searches for
     * U+0800, U+1000, U+2000, .., U+F000, U+10000.
     * U+0800 is the first 3-byte-UTF-8 code point. Lower code points are
     * looked up in the bit tables.
     * The last pair of indexes is for finding supplementary code points.
     */
    list4kStarts[0]=ڈھونڈو_رمز_نقطہ(0x800, 0, list_لمبائی-1);
    int32_t i;
    برائے(i=1; i<=0x10; ++i) {
        list4kStarts[i]=ڈھونڈو_رمز_نقطہ(i<<12, list4kStarts[i-1], list_لمبائی-1);
    }
    list4kStarts[0x11]=list_لمبائی-1;
    شامل_ہے_FFFD=شامل_ہے_Slow(0xfffd, list4kStarts[0xf], list4kStarts[0x10]);

    init_بٹ();
    overrideIllegal();
}

BMPSet::BMPSet(ثابت BMPSet &otherBMPSet, ثابت int32_t *new_والد_List, int32_t new_والد_List_لمبائی) :
        شامل_ہے_FFFD(otherBMPSet.شامل_ہے_FFFD),
        list(new_والد_List), list_لمبائی(new_والد_List_لمبائی) {
    uprv_memcpy(latin1_شامل_ہے, otherBMPSet.latin1_شامل_ہے, حجم(latin1_شامل_ہے));
    uprv_memcpy(table7FF, otherBMPSet.table7FF, حجم(table7FF));
    uprv_memcpy(bmp_بلاک_بٹ, otherBMPSet.bmp_بلاک_بٹ, حجم(bmp_بلاک_بٹ));
    uprv_memcpy(list4kStarts, otherBMPSet.list4kStarts, حجم(list4kStarts));
}

BMPSet::~BMPSet() {
}

/*
 * Set bits in a bit rectangle in "vertical" bit organization.
 * start<limit<=0x800
 */
ساکن خالی set32x64_بٹ(uint32_t جدول[64], int32_t آغاز, int32_t حد_بندی) {
    U_ASSERT(آغاز<حد_بندی);
    U_ASSERT(حد_بندی<=0x800);

    int32_t lead=آغاز>>6;  // Named for UTF-8 2-byte lead byte with upper 5 bits.
    int32_t trail=آغاز&0x3f;  // Named for UTF-8 2-byte trail byte with lower 6 bits.

    // Set one bit indicating an all-one block.
    uint32_t بٹ = static_cast<uint32_t>(1) << lead;
    اگر((آغاز+1)==حد_بندی) {  // Single-character shortcut.
        جدول[trail]|=بٹ;
        واپس;
    }

    int32_t حد_بندی_Lead=حد_بندی>>6;
    int32_t حد_بندی_Trail=حد_بندی&0x3f;

    اگر(lead==حد_بندی_Lead) {
        // Partial vertical bit column.
        جب(trail<حد_بندی_Trail) {
            جدول[trail++]|=بٹ;
        }
    } ورنہ {
        // Partial vertical bit column,
        // followed by a bit rectangle,
        // followed by another partial vertical bit column.
        اگر(trail>0) {
            کرو {
                جدول[trail++]|=بٹ;
            } جب(trail<64);
            ++lead;
        }
        اگر(lead<حد_بندی_Lead) {
            بٹ = ~((static_cast<غیرعلامتی>(1) << lead) - 1);
            اگر(حد_بندی_Lead<0x20) {
                بٹ &= (static_cast<غیرعلامتی>(1) << حد_بندی_Lead) - 1;
            }
            برائے(trail=0; trail<64; ++trail) {
                جدول[trail]|=بٹ;
            }
        }
        // limit<=0x800. If limit==0x800 then limitLead=32 and limitTrail=0.
        // In that case, bits=1<<limitLead is undefined but the bits value
        // is not used because trail<limitTrail is already false.
        بٹ = static_cast<uint32_t>(1) << ((حد_بندی_Lead == 0x20) ? (حد_بندی_Lead - 1) : حد_بندی_Lead);
        برائے(trail=0; trail<حد_بندی_Trail; ++trail) {
            جدول[trail]|=بٹ;
        }
    }
}

خالی BMPSet::init_بٹ() {
    UChar32 آغاز, حد_بندی;
    int32_t list_اشاریہ=0;

    // Set latin1Contains[].
    کرو {
        آغاز=list[list_اشاریہ++];
        اگر(list_اشاریہ<list_لمبائی) {
            حد_بندی=list[list_اشاریہ++];
        } ورنہ {
            حد_بندی=0x110000;
        }
        اگر(آغاز>=0x100) {
            توڑو;
        }
        کرو {
            latin1_شامل_ہے[آغاز++]=1;
        } جب(آغاز<حد_بندی && آغاز<0x100);
    } جب(حد_بندی<=0x100);

    // Find the first range overlapping with (or after) 80..FF again,
    // to include them in table7FF as well.
    برائے(list_اشاریہ=0;;) {
        آغاز=list[list_اشاریہ++];
        اگر(list_اشاریہ<list_لمبائی) {
            حد_بندی=list[list_اشاریہ++];
        } ورنہ {
            حد_بندی=0x110000;
        }
        اگر(حد_بندی>0x80) {
            اگر(آغاز<0x80) {
                آغاز=0x80;
            }
            توڑو;
        }
    }

    // Set table7FF[].
    جب(آغاز<0x800) {
        set32x64_بٹ(table7FF, آغاز, حد_بندی<=0x800 ? حد_بندی : 0x800);
        اگر(حد_بندی>0x800) {
            آغاز=0x800;
            توڑو;
        }

        آغاز=list[list_اشاریہ++];
        اگر(list_اشاریہ<list_لمبائی) {
            حد_بندی=list[list_اشاریہ++];
        } ورنہ {
            حد_بندی=0x110000;
        }
    }

    // Set bmpBlockBits[].
    int32_t min_آغاز=0x800;
    جب(آغاز<0x10000) {
        اگر(حد_بندی>0x10000) {
            حد_بندی=0x10000;
        }

        اگر(آغاز<min_آغاز) {
            آغاز=min_آغاز;
        }
        اگر(آغاز<حد_بندی) {  // Else: Another range entirely in a known mixed-value block.
            اگر(آغاز&0x3f) {
                // Mixed-value block of 64 code points.
                آغاز>>=6;
                bmp_بلاک_بٹ[آغاز&0x3f]|=0x10001<<(آغاز>>6);
                آغاز=(آغاز+1)<<6;  // Round up to the next block boundary.
                min_آغاز=آغاز;      // Ignore further ranges in this block.
            }
            اگر(آغاز<حد_بندی) {
                اگر(آغاز<(حد_بندی&~0x3f)) {
                    // Multiple all-ones blocks of 64 code points each.
                    set32x64_بٹ(bmp_بلاک_بٹ, آغاز>>6, حد_بندی>>6);
                }

                اگر(حد_بندی&0x3f) {
                    // Mixed-value block of 64 code points.
                    حد_بندی>>=6;
                    bmp_بلاک_بٹ[حد_بندی&0x3f]|=0x10001<<(حد_بندی>>6);
                    حد_بندی=(حد_بندی+1)<<6;  // Round up to the next block boundary.
                    min_آغاز=حد_بندی;      // Ignore further ranges in this block.
                }
            }
        }

        اگر(حد_بندی==0x10000) {
            توڑو;
        }

        آغاز=list[list_اشاریہ++];
        اگر(list_اشاریہ<list_لمبائی) {
            حد_بندی=list[list_اشاریہ++];
        } ورنہ {
            حد_بندی=0x110000;
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
خالی BMPSet::overrideIllegal() {
    uint32_t بٹ, mask;
    int32_t i;

    اگر(شامل_ہے_FFFD) {
        بٹ=3;                 // Lead bytes 0xC0 and 0xC1.
        برائے(i=0; i<64; ++i) {
            table7FF[i]|=بٹ;
        }

        بٹ=1;                 // Lead byte 0xE0.
        برائے(i=0; i<32; ++i) {   // First half of 4k block.
            bmp_بلاک_بٹ[i]|=بٹ;
        }

        mask= static_cast<uint32_t>(~(0x10001<<0xd));   // Lead byte 0xED.
        بٹ=1<<0xd;
        برائے(i=32; i<64; ++i) {  // Second half of 4k block.
            bmp_بلاک_بٹ[i]=(bmp_بلاک_بٹ[i]&mask)|بٹ;
        }
    } ورنہ {
        mask= static_cast<uint32_t>(~(0x10001<<0xd));   // Lead byte 0xED.
        برائے(i=32; i<64; ++i) {  // Second half of 4k block.
            bmp_بلاک_بٹ[i]&=mask;
        }
    }
}

int32_t BMPSet::ڈھونڈو_رمز_نقطہ(UChar32 c, int32_t lo, int32_t hi) ثابت {
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
    اگر (c < list[lo])
        واپس lo;
    // High runner test.  c is often after the last range, so an
    // initial check for this condition pays off.
    اگر (lo >= hi || c >= list[hi-1])
        واپس hi;
    // invariant: c >= list[lo]
    // invariant: c < list[hi]
    برائے (;;) {
        int32_t i = (lo + hi) >> 1;
        اگر (i == lo) {
            توڑو; // Found!
        } ورنہ اگر (c < list[i]) {
            hi = i;
        } ورنہ {
            lo = i;
        }
    }
    واپس hi;
}

UBool
BMPSet::شامل_ہے(UChar32 c) ثابت {
    اگر (static_cast<uint32_t>(c) <= 0xff) {
        واپس latin1_شامل_ہے[c];
    } ورنہ اگر (static_cast<uint32_t>(c) <= 0x7ff) {
        واپس (table7FF[c & 0x3f] & (static_cast<uint32_t>(1) << (c >> 6))) != 0;
    } ورنہ اگر (static_cast<uint32_t>(c) < 0xd800 || (c >= 0xe000 && c <= 0xffff)) {
        صحیح lead=c>>12;
        uint32_t two_بٹ=(bmp_بلاک_بٹ[(c>>6)&0x3f]>>lead)&0x10001;
        اگر(two_بٹ<=1) {
            // All 64 code points with the same bits 15..6
            // are either in the set or not.
            واپس two_بٹ;
        } ورنہ {
            // Look up the code point in its 4k block of code points.
            واپس شامل_ہے_Slow(c, list4kStarts[lead], list4kStarts[lead+1]);
        }
    } ورنہ اگر (static_cast<uint32_t>(c) <= 0x10ffff) {
        // surrogate or supplementary code point
        واپس شامل_ہے_Slow(c, list4kStarts[0xd], list4kStarts[0x11]);
    } ورنہ {
        // Out-of-range code points get false, consistent with long-standing
        // behavior of UnicodeSet::contains(c).
        واپس جھوٹ;
    }
}

/*
 * Check for sufficient length for trail unit for each surrogate pair.
 * Handle single surrogates as surrogate code points as usual in ICU.
 */
ثابت char16_t *
BMPSet::وسعت(ثابت char16_t *s, ثابت char16_t *حد_بندی, USet_وسعت_Condition وسعت_Condition) ثابت {
    char16_t c, c2;

    اگر(وسعت_Condition) {
        //وسعت
        کرو {
            c=*s;
            اگر(c<=0xff) {
                اگر(!latin1_شامل_ہے[c]) {
                    توڑو;
                }
            } ورنہ اگر(c<=0x7ff) {
                اگر ((table7FF[c & 0x3f] & (static_cast<uint32_t>(1) << (c >> 6))) == 0) {
                    توڑو;
                }
            } ورنہ اگر(c<0xd800 || c>=0xe000) {
                صحیح lead=c>>12;
                uint32_t two_بٹ=(bmp_بلاک_بٹ[(c>>6)&0x3f]>>lead)&0x10001;
                اگر(two_بٹ<=1) {
                    // All 64 code points with the same bits 15..6
                    // are either in the set or not.
                    اگر(two_بٹ==0) {
                        توڑو;
                    }
                } ورنہ {
                    // Look up the code point in its 4k block of code points.
                    اگر(!شامل_ہے_Slow(c, list4kStarts[lead], list4kStarts[lead+1])) {
                        توڑو;
                    }
                }
            } ورنہ اگر(c>=0xdc00 || (s+1)==حد_بندی || (c2=s[1])<0xdc00 || c2>=0xe000) {
                // surrogate code point
                اگر(!شامل_ہے_Slow(c, list4kStarts[0xd], list4kStarts[0xe])) {
                    توڑو;
                }
            } ورنہ {
                // surrogate pair
                اگر(!شامل_ہے_Slow(U16_GET_SUPPLEMENTARY(c, c2), list4kStarts[0x10], list4kStarts[0x11])) {
                    توڑو;
                }
                ++s;
            }
        } جب(++s<حد_بندی);
    } ورنہ {
        // span not
        کرو {
            c=*s;
            اگر(c<=0xff) {
                اگر(latin1_شامل_ہے[c]) {
                    توڑو;
                }
            } ورنہ اگر(c<=0x7ff) {
                اگر ((table7FF[c & 0x3f] & (static_cast<uint32_t>(1) << (c >> 6))) != 0) {
                    توڑو;
                }
            } ورنہ اگر(c<0xd800 || c>=0xe000) {
                صحیح lead=c>>12;
                uint32_t two_بٹ=(bmp_بلاک_بٹ[(c>>6)&0x3f]>>lead)&0x10001;
                اگر(two_بٹ<=1) {
                    // All 64 code points with the same bits 15..6
                    // are either in the set or not.
                    اگر(two_بٹ!=0) {
                        توڑو;
                    }
                } ورنہ {
                    // Look up the code point in its 4k block of code points.
                    اگر(شامل_ہے_Slow(c, list4kStarts[lead], list4kStarts[lead+1])) {
                        توڑو;
                    }
                }
            } ورنہ اگر(c>=0xdc00 || (s+1)==حد_بندی || (c2=s[1])<0xdc00 || c2>=0xe000) {
                // surrogate code point
                اگر(شامل_ہے_Slow(c, list4kStarts[0xd], list4kStarts[0xe])) {
                    توڑو;
                }
            } ورنہ {
                // surrogate pair
                اگر(شامل_ہے_Slow(U16_GET_SUPPLEMENTARY(c, c2), list4kStarts[0x10], list4kStarts[0x11])) {
                    توڑو;
                }
                ++s;
            }
        } جب(++s<حد_بندی);
    }
    واپس s;
}

/* Symmetrical with span(). */
ثابت char16_t *
BMPSet::وسعت_عقب(ثابت char16_t *s, ثابت char16_t *حد_بندی, USet_وسعت_Condition وسعت_Condition) ثابت {
    char16_t c, c2;

    اگر(وسعت_Condition) {
        //وسعت
        برائے(;;) {
            c=*(--حد_بندی);
            اگر(c<=0xff) {
                اگر(!latin1_شامل_ہے[c]) {
                    توڑو;
                }
            } ورنہ اگر(c<=0x7ff) {
                اگر ((table7FF[c & 0x3f] & (static_cast<uint32_t>(1) << (c >> 6))) == 0) {
                    توڑو;
                }
            } ورنہ اگر(c<0xd800 || c>=0xe000) {
                صحیح lead=c>>12;
                uint32_t two_بٹ=(bmp_بلاک_بٹ[(c>>6)&0x3f]>>lead)&0x10001;
                اگر(two_بٹ<=1) {
                    // All 64 code points with the same bits 15..6
                    // are either in the set or not.
                    اگر(two_بٹ==0) {
                        توڑو;
                    }
                } ورنہ {
                    // Look up the code point in its 4k block of code points.
                    اگر(!شامل_ہے_Slow(c, list4kStarts[lead], list4kStarts[lead+1])) {
                        توڑو;
                    }
                }
            } ورنہ اگر(c<0xdc00 || s==حد_بندی || (c2=*(حد_بندی-1))<0xd800 || c2>=0xdc00) {
                // surrogate code point
                اگر(!شامل_ہے_Slow(c, list4kStarts[0xd], list4kStarts[0xe])) {
                    توڑو;
                }
            } ورنہ {
                // surrogate pair
                اگر(!شامل_ہے_Slow(U16_GET_SUPPLEMENTARY(c2, c), list4kStarts[0x10], list4kStarts[0x11])) {
                    توڑو;
                }
                --حد_بندی;
            }
            اگر(s==حد_بندی) {
                واپس s;
            }
        }
    } ورنہ {
        // span not
        برائے(;;) {
            c=*(--حد_بندی);
            اگر(c<=0xff) {
                اگر(latin1_شامل_ہے[c]) {
                    توڑو;
                }
            } ورنہ اگر(c<=0x7ff) {
                اگر ((table7FF[c & 0x3f] & (static_cast<uint32_t>(1) << (c >> 6))) != 0) {
                    توڑو;
                }
            } ورنہ اگر(c<0xd800 || c>=0xe000) {
                صحیح lead=c>>12;
                uint32_t two_بٹ=(bmp_بلاک_بٹ[(c>>6)&0x3f]>>lead)&0x10001;
                اگر(two_بٹ<=1) {
                    // All 64 code points with the same bits 15..6
                    // are either in the set or not.
                    اگر(two_بٹ!=0) {
                        توڑو;
                    }
                } ورنہ {
                    // Look up the code point in its 4k block of code points.
                    اگر(شامل_ہے_Slow(c, list4kStarts[lead], list4kStarts[lead+1])) {
                        توڑو;
                    }
                }
            } ورنہ اگر(c<0xdc00 || s==حد_بندی || (c2=*(حد_بندی-1))<0xd800 || c2>=0xdc00) {
                // surrogate code point
                اگر(شامل_ہے_Slow(c, list4kStarts[0xd], list4kStarts[0xe])) {
                    توڑو;
                }
            } ورنہ {
                // surrogate pair
                اگر(شامل_ہے_Slow(U16_GET_SUPPLEMENTARY(c2, c), list4kStarts[0x10], list4kStarts[0x11])) {
                    توڑو;
                }
                --حد_بندی;
            }
            اگر(s==حد_بندی) {
                واپس s;
            }
        }
    }
    واپس حد_بندی+1;
}

/*
 * Precheck for sufficient trail bytes at end of string only once per span.
 * Check validity.
 */
ثابت uint8_t *
BMPSet::وسعت_UTF8(ثابت uint8_t *s, int32_t length, USet_وسعت_Condition وسعت_Condition) ثابت {
    ثابت uint8_t *حد_بندی=s+length;
    uint8_t b=*s;
    اگر(U8_IS_SINGLE(b)) {
        // Initial all-ASCII span.
        اگر(وسعت_Condition) {
            کرو {
                اگر(!latin1_شامل_ہے[b] || ++s==حد_بندی) {
                    واپس s;
                }
                b=*s;
            } جب(U8_IS_SINGLE(b));
        } ورنہ {
            کرو {
                اگر(latin1_شامل_ہے[b] || ++s==حد_بندی) {
                    واپس s;
                }
                b=*s;
            } جب(U8_IS_SINGLE(b));
        }
        length = static_cast<int32_t>(حد_بندی - s);
    }

    اگر(وسعت_Condition!=USET_وسعت_NOT_CONTAINED) {
        وسعت_Condition=USET_وسعت_CONTAINED;  // Pin to 0/1 values.
    }

    ثابت uint8_t *limit0=حد_بندی;

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
    b=*(حد_بندی-1);
    اگر (static_cast<int8_t>(b) < 0) {
        // b>=0x80: lead or trail byte
        اگر(b<0xc0) {
            // single trail byte, check for preceding 3- or 4-byte lead byte
            اگر(length>=2 && (b=*(حد_بندی-2))>=0xe0) {
                حد_بندی-=2;
                اگر(شامل_ہے_FFFD!=وسعت_Condition) {
                    limit0=حد_بندی;
                }
            } ورنہ اگر(b<0xc0 && b>=0x80 && length>=3 && (b=*(حد_بندی-3))>=0xf0) {
                // 4-byte lead byte with only two trail bytes
                حد_بندی-=3;
                اگر(شامل_ہے_FFFD!=وسعت_Condition) {
                    limit0=حد_بندی;
                }
            }
        } ورنہ {
            // lead byte with no trail bytes
            --حد_بندی;
            اگر(شامل_ہے_FFFD!=وسعت_Condition) {
                limit0=حد_بندی;
            }
        }
    }

    uint8_t t1, t2, t3;

    جب(s<حد_بندی) {
        b=*s;
        اگر(U8_IS_SINGLE(b)) {
            // ASCII
            اگر(وسعت_Condition) {
                کرو {
                    اگر(!latin1_شامل_ہے[b]) {
                        واپس s;
                    } ورنہ اگر(++s==حد_بندی) {
                        واپس limit0;
                    }
                    b=*s;
                } جب(U8_IS_SINGLE(b));
            } ورنہ {
                کرو {
                    اگر(latin1_شامل_ہے[b]) {
                        واپس s;
                    } ورنہ اگر(++s==حد_بندی) {
                        واپس limit0;
                    }
                    b=*s;
                } جب(U8_IS_SINGLE(b));
            }
        }
        ++s;  // Advance past the lead byte.
        اگر(b>=0xe0) {
            اگر(b<0xf0) {
                اگر( /* handle U+0000..U+FFFF inline */
                    (t1 = static_cast<uint8_t>(s[0] - 0x80)) <= 0x3f &&
                    (t2 = static_cast<uint8_t>(s[1] - 0x80)) <= 0x3f
                ) {
                    b&=0xf;
                    uint32_t two_بٹ=(bmp_بلاک_بٹ[t1]>>b)&0x10001;
                    اگر(two_بٹ<=1) {
                        // All 64 code points with this lead byte and middle trail byte
                        // are either in the set or not.
                        اگر (two_بٹ != static_cast<uint32_t>(وسعت_Condition)) {
                            واپس s-1;
                        }
                    } ورنہ {
                        // Look up the code point in its 4k block of code points.
                        UChar32 c=(b<<12)|(t1<<6)|t2;
                        اگر(شامل_ہے_Slow(c, list4kStarts[b], list4kStarts[b+1]) != وسعت_Condition) {
                            واپس s-1;
                        }
                    }
                    s+=2;
                    جاری;
                }
            } ورنہ اگر( /* handle U+10000..U+10FFFF inline */
                (t1 = static_cast<uint8_t>(s[0] - 0x80)) <= 0x3f &&
                (t2 = static_cast<uint8_t>(s[1] - 0x80)) <= 0x3f &&
                (t3 = static_cast<uint8_t>(s[2] - 0x80)) <= 0x3f
            ) {
                // Give an illegal sequence the same value as the result of contains(FFFD).
                UChar32 c = (static_cast<UChar32>(b - 0xf0) << 18) | (static_cast<UChar32>(t1) << 12) | (t2 << 6) | t3;
                اگر( (   (0x10000<=c && c<=0x10ffff) ?
                            شامل_ہے_Slow(c, list4kStarts[0x10], list4kStarts[0x11]) :
                            شامل_ہے_FFFD
                    ) != وسعت_Condition
                ) {
                    واپس s-1;
                }
                s+=3;
                جاری;
            }
        } ورنہ {
            اگر( /* handle U+0000..U+07FF inline */
                b>=0xc0 &&
                (t1 = static_cast<uint8_t>(*s - 0x80)) <= 0x3f
            ) {
                اگر (static_cast<USet_وسعت_Condition>((table7FF[t1] & (static_cast<uint32_t>(1) << (b & 0x1f))) != 0) != وسعت_Condition) {
                    واپس s-1;
                }
                ++s;
                جاری;
            }
        }

        // Give an illegal sequence the same value as the result of contains(FFFD).
        // Handle each byte of an illegal sequence separately to simplify the code;
        // no need to optimize error handling.
        اگر(شامل_ہے_FFFD!=وسعت_Condition) {
            واپس s-1;
        }
    }

    واپس limit0;
}

/*
 * While going backwards through UTF-8 optimize only for ASCII.
 * Unlike UTF-16, UTF-8 is not forward-backward symmetrical, that is, it is not
 * possible to tell from the last byte in a multi-byte sequence how many
 * preceding bytes there should be. Therefore, going backwards through UTF-8
 * is much harder than going forward.
 */
int32_t
BMPSet::وسعت_عقب_UTF8(ثابت uint8_t *s, int32_t length, USet_وسعت_Condition وسعت_Condition) ثابت {
    اگر(وسعت_Condition!=USET_وسعت_NOT_CONTAINED) {
        وسعت_Condition=USET_وسعت_CONTAINED;  // Pin to 0/1 values.
    }

    uint8_t b;

    کرو {
        b=s[--length];
        اگر(U8_IS_SINGLE(b)) {
            // ASCII sub-span
            اگر(وسعت_Condition) {
                کرو {
                    اگر(!latin1_شامل_ہے[b]) {
                        واپس length+1;
                    } ورنہ اگر(length==0) {
                        واپس 0;
                    }
                    b=s[--length];
                } جب(U8_IS_SINGLE(b));
            } ورنہ {
                کرو {
                    اگر(latin1_شامل_ہے[b]) {
                        واپس length+1;
                    } ورنہ اگر(length==0) {
                        واپس 0;
                    }
                    b=s[--length];
                } جب(U8_IS_SINGLE(b));
            }
        }

        int32_t پچھلا=length;
        UChar32 c;
        // trail byte: collect a multi-byte character
        // (or  lead byte in last-trail position)
        c=utf8_پچھلا_Char_محفوظ_Body(s, 0, &length, b, -3);
        // c is a valid code point, not ASCII, not a surrogate
        اگر(c<=0x7ff) {
            اگر (static_cast<USet_وسعت_Condition>((table7FF[c & 0x3f] & (static_cast<uint32_t>(1) << (c >> 6))) != 0) != وسعت_Condition) {
                واپس پچھلا+1;
            }
        } ورنہ اگر(c<=0xffff) {
            صحیح lead=c>>12;
            uint32_t two_بٹ=(bmp_بلاک_بٹ[(c>>6)&0x3f]>>lead)&0x10001;
            اگر(two_بٹ<=1) {
                // All 64 code points with the same bits 15..6
                // are either in the set or not.
                اگر (two_بٹ != static_cast<uint32_t>(وسعت_Condition)) {
                    واپس پچھلا+1;
                }
            } ورنہ {
                // Look up the code point in its 4k block of code points.
                اگر(شامل_ہے_Slow(c, list4kStarts[lead], list4kStarts[lead+1]) != وسعت_Condition) {
                    واپس پچھلا+1;
                }
            }
        } ورنہ {
            اگر(شامل_ہے_Slow(c, list4kStarts[0x10], list4kStarts[0x11]) != وسعت_Condition) {
                واپس پچھلا+1;
            }
        }
    } جب(length>0);
    واپس 0;
}

U_NAMESPACE_انتہا
