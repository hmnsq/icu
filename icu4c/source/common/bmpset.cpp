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

#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/utypes.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/uniset.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/utf8.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/utf16.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "cmemory.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "bmpset.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "uassert.h"

U_NAMESPACE_BEGIN

BMPSet::BMPSet(ਅਚਲ int32_t *ਮਾਪਾ_List, int32_t ਮਾਪਾ_List_ਲੰਮਾਈ) :
        list(ਮਾਪਾ_List), list_ਲੰਮਾਈ(ਮਾਪਾ_List_ਲੰਮਾਈ) {
    uprv_memset(latin1_ਸ਼ਾਮਲ_ਹੈ, 0, ਆਕਾਰ(latin1_ਸ਼ਾਮਲ_ਹੈ));
    uprv_memset(table7FF, 0, ਆਕਾਰ(table7FF));
    uprv_memset(bmp_ਖੰਡ_ਬਿੱਟ, 0, ਆਕਾਰ(bmp_ਖੰਡ_ਬਿੱਟ));

    /*
     * Set the list indexes for binary searches for
     * U+0800, U+1000, U+2000, .., U+F000, U+10000.
     * U+0800 is the first 3-byte-UTF-8 code point. Lower code points are
     * looked up in the bit tables.
     * The last pair of indexes is for finding supplementary code points.
     */
    list4kStarts[0]=ਲੱਭੋ_ਕੋਡ_ਬਿੰਦੂ(0x800, 0, list_ਲੰਮਾਈ-1);
    int32_t i;
    ਲਈ(i=1; i<=0x10; ++i) {
        list4kStarts[i]=ਲੱਭੋ_ਕੋਡ_ਬਿੰਦੂ(i<<12, list4kStarts[i-1], list_ਲੰਮਾਈ-1);
    }
    list4kStarts[0x11]=list_ਲੰਮਾਈ-1;
    ਸ਼ਾਮਲ_ਹੈ_FFFD=ਸ਼ਾਮਲ_ਹੈ_Slow(0xfffd, list4kStarts[0xf], list4kStarts[0x10]);

    init_ਬਿੱਟ();
    overrideIllegal();
}

BMPSet::BMPSet(ਅਚਲ BMPSet &otherBMPSet, ਅਚਲ int32_t *new_ਮਾਪਾ_List, int32_t new_ਮਾਪਾ_List_ਲੰਮਾਈ) :
        ਸ਼ਾਮਲ_ਹੈ_FFFD(otherBMPSet.ਸ਼ਾਮਲ_ਹੈ_FFFD),
        list(new_ਮਾਪਾ_List), list_ਲੰਮਾਈ(new_ਮਾਪਾ_List_ਲੰਮਾਈ) {
    uprv_memcpy(latin1_ਸ਼ਾਮਲ_ਹੈ, otherBMPSet.latin1_ਸ਼ਾਮਲ_ਹੈ, ਆਕਾਰ(latin1_ਸ਼ਾਮਲ_ਹੈ));
    uprv_memcpy(table7FF, otherBMPSet.table7FF, ਆਕਾਰ(table7FF));
    uprv_memcpy(bmp_ਖੰਡ_ਬਿੱਟ, otherBMPSet.bmp_ਖੰਡ_ਬਿੱਟ, ਆਕਾਰ(bmp_ਖੰਡ_ਬਿੱਟ));
    uprv_memcpy(list4kStarts, otherBMPSet.list4kStarts, ਆਕਾਰ(list4kStarts));
}

BMPSet::~BMPSet() {
}

/*
 * Set bits in a bit rectangle in "vertical" bit organization.
 * start<limit<=0x800
 */
ਸਥਿਰ ਖਾਲੀ set32x64_ਬਿੱਟ(uint32_t ਸਾਰਣੀ[64], int32_t ਸ਼ੁਰੂ, int32_t ਹੱਦਬੰਦੀ) {
    U_ASSERT(ਸ਼ੁਰੂ<ਹੱਦਬੰਦੀ);
    U_ASSERT(ਹੱਦਬੰਦੀ<=0x800);

    int32_t lead=ਸ਼ੁਰੂ>>6;  // Named for UTF-8 2-byte lead byte with upper 5 bits.
    int32_t trail=ਸ਼ੁਰੂ&0x3f;  // Named for UTF-8 2-byte trail byte with lower 6 bits.

    // Set one bit indicating an all-one block.
    uint32_t ਬਿੱਟ = static_cast<uint32_t>(1) << lead;
    ਜੇ((ਸ਼ੁਰੂ+1)==ਹੱਦਬੰਦੀ) {  // Single-character shortcut.
        ਸਾਰਣੀ[trail]|=ਬਿੱਟ;
        ਵਾਪਸ;
    }

    int32_t ਹੱਦਬੰਦੀ_Lead=ਹੱਦਬੰਦੀ>>6;
    int32_t ਹੱਦਬੰਦੀ_Trail=ਹੱਦਬੰਦੀ&0x3f;

    ਜੇ(lead==ਹੱਦਬੰਦੀ_Lead) {
        // Partial vertical bit column.
        ਜਦੋਂ(trail<ਹੱਦਬੰਦੀ_Trail) {
            ਸਾਰਣੀ[trail++]|=ਬਿੱਟ;
        }
    } ਨਹੀਂਤਾਂ {
        // Partial vertical bit column,
        // followed by a bit rectangle,
        // followed by another partial vertical bit column.
        ਜੇ(trail>0) {
            ਕਰੋ {
                ਸਾਰਣੀ[trail++]|=ਬਿੱਟ;
            } ਜਦੋਂ(trail<64);
            ++lead;
        }
        ਜੇ(lead<ਹੱਦਬੰਦੀ_Lead) {
            ਬਿੱਟ = ~((static_cast<ਬਿਨਾਨਿਸ਼ਾਨੀ>(1) << lead) - 1);
            ਜੇ(ਹੱਦਬੰਦੀ_Lead<0x20) {
                ਬਿੱਟ &= (static_cast<ਬਿਨਾਨਿਸ਼ਾਨੀ>(1) << ਹੱਦਬੰਦੀ_Lead) - 1;
            }
            ਲਈ(trail=0; trail<64; ++trail) {
                ਸਾਰਣੀ[trail]|=ਬਿੱਟ;
            }
        }
        // limit<=0x800. If limit==0x800 then limitLead=32 and limitTrail=0.
        // In that case, bits=1<<limitLead is undefined but the bits value
        // is not used because trail<limitTrail is already false.
        ਬਿੱਟ = static_cast<uint32_t>(1) << ((ਹੱਦਬੰਦੀ_Lead == 0x20) ? (ਹੱਦਬੰਦੀ_Lead - 1) : ਹੱਦਬੰਦੀ_Lead);
        ਲਈ(trail=0; trail<ਹੱਦਬੰਦੀ_Trail; ++trail) {
            ਸਾਰਣੀ[trail]|=ਬਿੱਟ;
        }
    }
}

ਖਾਲੀ BMPSet::init_ਬਿੱਟ() {
    UChar32 ਸ਼ੁਰੂ, ਹੱਦਬੰਦੀ;
    int32_t list_ਸੂਚਕਾਂਕ=0;

    // Set latin1Contains[].
    ਕਰੋ {
        ਸ਼ੁਰੂ=list[list_ਸੂਚਕਾਂਕ++];
        ਜੇ(list_ਸੂਚਕਾਂਕ<list_ਲੰਮਾਈ) {
            ਹੱਦਬੰਦੀ=list[list_ਸੂਚਕਾਂਕ++];
        } ਨਹੀਂਤਾਂ {
            ਹੱਦਬੰਦੀ=0x110000;
        }
        ਜੇ(ਸ਼ੁਰੂ>=0x100) {
            ਰੋਕੋ;
        }
        ਕਰੋ {
            latin1_ਸ਼ਾਮਲ_ਹੈ[ਸ਼ੁਰੂ++]=1;
        } ਜਦੋਂ(ਸ਼ੁਰੂ<ਹੱਦਬੰਦੀ && ਸ਼ੁਰੂ<0x100);
    } ਜਦੋਂ(ਹੱਦਬੰਦੀ<=0x100);

    // Find the first range overlapping with (or after) 80..FF again,
    // to include them in table7FF as well.
    ਲਈ(list_ਸੂਚਕਾਂਕ=0;;) {
        ਸ਼ੁਰੂ=list[list_ਸੂਚਕਾਂਕ++];
        ਜੇ(list_ਸੂਚਕਾਂਕ<list_ਲੰਮਾਈ) {
            ਹੱਦਬੰਦੀ=list[list_ਸੂਚਕਾਂਕ++];
        } ਨਹੀਂਤਾਂ {
            ਹੱਦਬੰਦੀ=0x110000;
        }
        ਜੇ(ਹੱਦਬੰਦੀ>0x80) {
            ਜੇ(ਸ਼ੁਰੂ<0x80) {
                ਸ਼ੁਰੂ=0x80;
            }
            ਰੋਕੋ;
        }
    }

    // Set table7FF[].
    ਜਦੋਂ(ਸ਼ੁਰੂ<0x800) {
        set32x64_ਬਿੱਟ(table7FF, ਸ਼ੁਰੂ, ਹੱਦਬੰਦੀ<=0x800 ? ਹੱਦਬੰਦੀ : 0x800);
        ਜੇ(ਹੱਦਬੰਦੀ>0x800) {
            ਸ਼ੁਰੂ=0x800;
            ਰੋਕੋ;
        }

        ਸ਼ੁਰੂ=list[list_ਸੂਚਕਾਂਕ++];
        ਜੇ(list_ਸੂਚਕਾਂਕ<list_ਲੰਮਾਈ) {
            ਹੱਦਬੰਦੀ=list[list_ਸੂਚਕਾਂਕ++];
        } ਨਹੀਂਤਾਂ {
            ਹੱਦਬੰਦੀ=0x110000;
        }
    }

    // Set bmpBlockBits[].
    int32_t min_ਸ਼ੁਰੂ=0x800;
    ਜਦੋਂ(ਸ਼ੁਰੂ<0x10000) {
        ਜੇ(ਹੱਦਬੰਦੀ>0x10000) {
            ਹੱਦਬੰਦੀ=0x10000;
        }

        ਜੇ(ਸ਼ੁਰੂ<min_ਸ਼ੁਰੂ) {
            ਸ਼ੁਰੂ=min_ਸ਼ੁਰੂ;
        }
        ਜੇ(ਸ਼ੁਰੂ<ਹੱਦਬੰਦੀ) {  // Else: Another range entirely in a known mixed-value block.
            ਜੇ(ਸ਼ੁਰੂ&0x3f) {
                // Mixed-value block of 64 code points.
                ਸ਼ੁਰੂ>>=6;
                bmp_ਖੰਡ_ਬਿੱਟ[ਸ਼ੁਰੂ&0x3f]|=0x10001<<(ਸ਼ੁਰੂ>>6);
                ਸ਼ੁਰੂ=(ਸ਼ੁਰੂ+1)<<6;  // Round up to the next block boundary.
                min_ਸ਼ੁਰੂ=ਸ਼ੁਰੂ;      // Ignore further ranges in this block.
            }
            ਜੇ(ਸ਼ੁਰੂ<ਹੱਦਬੰਦੀ) {
                ਜੇ(ਸ਼ੁਰੂ<(ਹੱਦਬੰਦੀ&~0x3f)) {
                    // Multiple all-ones blocks of 64 code points each.
                    set32x64_ਬਿੱਟ(bmp_ਖੰਡ_ਬਿੱਟ, ਸ਼ੁਰੂ>>6, ਹੱਦਬੰਦੀ>>6);
                }

                ਜੇ(ਹੱਦਬੰਦੀ&0x3f) {
                    // Mixed-value block of 64 code points.
                    ਹੱਦਬੰਦੀ>>=6;
                    bmp_ਖੰਡ_ਬਿੱਟ[ਹੱਦਬੰਦੀ&0x3f]|=0x10001<<(ਹੱਦਬੰਦੀ>>6);
                    ਹੱਦਬੰਦੀ=(ਹੱਦਬੰਦੀ+1)<<6;  // Round up to the next block boundary.
                    min_ਸ਼ੁਰੂ=ਹੱਦਬੰਦੀ;      // Ignore further ranges in this block.
                }
            }
        }

        ਜੇ(ਹੱਦਬੰਦੀ==0x10000) {
            ਰੋਕੋ;
        }

        ਸ਼ੁਰੂ=list[list_ਸੂਚਕਾਂਕ++];
        ਜੇ(list_ਸੂਚਕਾਂਕ<list_ਲੰਮਾਈ) {
            ਹੱਦਬੰਦੀ=list[list_ਸੂਚਕਾਂਕ++];
        } ਨਹੀਂਤਾਂ {
            ਹੱਦਬੰਦੀ=0x110000;
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
ਖਾਲੀ BMPSet::overrideIllegal() {
    uint32_t ਬਿੱਟ, mask;
    int32_t i;

    ਜੇ(ਸ਼ਾਮਲ_ਹੈ_FFFD) {
        ਬਿੱਟ=3;                 // Lead bytes 0xC0 and 0xC1.
        ਲਈ(i=0; i<64; ++i) {
            table7FF[i]|=ਬਿੱਟ;
        }

        ਬਿੱਟ=1;                 // Lead byte 0xE0.
        ਲਈ(i=0; i<32; ++i) {   // First half of 4k block.
            bmp_ਖੰਡ_ਬਿੱਟ[i]|=ਬਿੱਟ;
        }

        mask= static_cast<uint32_t>(~(0x10001<<0xd));   // Lead byte 0xED.
        ਬਿੱਟ=1<<0xd;
        ਲਈ(i=32; i<64; ++i) {  // Second half of 4k block.
            bmp_ਖੰਡ_ਬਿੱਟ[i]=(bmp_ਖੰਡ_ਬਿੱਟ[i]&mask)|ਬਿੱਟ;
        }
    } ਨਹੀਂਤਾਂ {
        mask= static_cast<uint32_t>(~(0x10001<<0xd));   // Lead byte 0xED.
        ਲਈ(i=32; i<64; ++i) {  // Second half of 4k block.
            bmp_ਖੰਡ_ਬਿੱਟ[i]&=mask;
        }
    }
}

int32_t BMPSet::ਲੱਭੋ_ਕੋਡ_ਬਿੰਦੂ(UChar32 c, int32_t lo, int32_t hi) ਅਚਲ {
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
    ਜੇ (c < list[lo])
        ਵਾਪਸ lo;
    // High runner test.  c is often after the last range, so an
    // initial check for this condition pays off.
    ਜੇ (lo >= hi || c >= list[hi-1])
        ਵਾਪਸ hi;
    // invariant: c >= list[lo]
    // invariant: c < list[hi]
    ਲਈ (;;) {
        int32_t i = (lo + hi) >> 1;
        ਜੇ (i == lo) {
            ਰੋਕੋ; // Found!
        } ਨਹੀਂਤਾਂ ਜੇ (c < list[i]) {
            hi = i;
        } ਨਹੀਂਤਾਂ {
            lo = i;
        }
    }
    ਵਾਪਸ hi;
}

UBool
BMPSet::ਸ਼ਾਮਲ_ਹੈ(UChar32 c) ਅਚਲ {
    ਜੇ (static_cast<uint32_t>(c) <= 0xff) {
        ਵਾਪਸ latin1_ਸ਼ਾਮਲ_ਹੈ[c];
    } ਨਹੀਂਤਾਂ ਜੇ (static_cast<uint32_t>(c) <= 0x7ff) {
        ਵਾਪਸ (table7FF[c & 0x3f] & (static_cast<uint32_t>(1) << (c >> 6))) != 0;
    } ਨਹੀਂਤਾਂ ਜੇ (static_cast<uint32_t>(c) < 0xd800 || (c >= 0xe000 && c <= 0xffff)) {
        ਪੂਰਨ lead=c>>12;
        uint32_t two_ਬਿੱਟ=(bmp_ਖੰਡ_ਬਿੱਟ[(c>>6)&0x3f]>>lead)&0x10001;
        ਜੇ(two_ਬਿੱਟ<=1) {
            // All 64 code points with the same bits 15..6
            // are either in the set or not.
            ਵਾਪਸ two_ਬਿੱਟ;
        } ਨਹੀਂਤਾਂ {
            // Look up the code point in its 4k block of code points.
            ਵਾਪਸ ਸ਼ਾਮਲ_ਹੈ_Slow(c, list4kStarts[lead], list4kStarts[lead+1]);
        }
    } ਨਹੀਂਤਾਂ ਜੇ (static_cast<uint32_t>(c) <= 0x10ffff) {
        // surrogate or supplementary code point
        ਵਾਪਸ ਸ਼ਾਮਲ_ਹੈ_Slow(c, list4kStarts[0xd], list4kStarts[0x11]);
    } ਨਹੀਂਤਾਂ {
        // Out-of-range code points get false, consistent with long-standing
        // behavior of UnicodeSet::contains(c).
        ਵਾਪਸ ਝੂਠ;
    }
}

/*
 * Check for sufficient length for trail unit for each surrogate pair.
 * Handle single surrogates as surrogate code points as usual in ICU.
 */
ਅਚਲ char16_t *
BMPSet::ਵਿਸਤਾਰ(ਅਚਲ char16_t *s, ਅਚਲ char16_t *ਹੱਦਬੰਦੀ, USet_ਵਿਸਤਾਰ_Condition ਵਿਸਤਾਰ_Condition) ਅਚਲ {
    char16_t c, c2;

    ਜੇ(ਵਿਸਤਾਰ_Condition) {
        //ਵਿਸਤਾਰ
        ਕਰੋ {
            c=*s;
            ਜੇ(c<=0xff) {
                ਜੇ(!latin1_ਸ਼ਾਮਲ_ਹੈ[c]) {
                    ਰੋਕੋ;
                }
            } ਨਹੀਂਤਾਂ ਜੇ(c<=0x7ff) {
                ਜੇ ((table7FF[c & 0x3f] & (static_cast<uint32_t>(1) << (c >> 6))) == 0) {
                    ਰੋਕੋ;
                }
            } ਨਹੀਂਤਾਂ ਜੇ(c<0xd800 || c>=0xe000) {
                ਪੂਰਨ lead=c>>12;
                uint32_t two_ਬਿੱਟ=(bmp_ਖੰਡ_ਬਿੱਟ[(c>>6)&0x3f]>>lead)&0x10001;
                ਜੇ(two_ਬਿੱਟ<=1) {
                    // All 64 code points with the same bits 15..6
                    // are either in the set or not.
                    ਜੇ(two_ਬਿੱਟ==0) {
                        ਰੋਕੋ;
                    }
                } ਨਹੀਂਤਾਂ {
                    // Look up the code point in its 4k block of code points.
                    ਜੇ(!ਸ਼ਾਮਲ_ਹੈ_Slow(c, list4kStarts[lead], list4kStarts[lead+1])) {
                        ਰੋਕੋ;
                    }
                }
            } ਨਹੀਂਤਾਂ ਜੇ(c>=0xdc00 || (s+1)==ਹੱਦਬੰਦੀ || (c2=s[1])<0xdc00 || c2>=0xe000) {
                // surrogate code point
                ਜੇ(!ਸ਼ਾਮਲ_ਹੈ_Slow(c, list4kStarts[0xd], list4kStarts[0xe])) {
                    ਰੋਕੋ;
                }
            } ਨਹੀਂਤਾਂ {
                // surrogate pair
                ਜੇ(!ਸ਼ਾਮਲ_ਹੈ_Slow(U16_GET_SUPPLEMENTARY(c, c2), list4kStarts[0x10], list4kStarts[0x11])) {
                    ਰੋਕੋ;
                }
                ++s;
            }
        } ਜਦੋਂ(++s<ਹੱਦਬੰਦੀ);
    } ਨਹੀਂਤਾਂ {
        // span not
        ਕਰੋ {
            c=*s;
            ਜੇ(c<=0xff) {
                ਜੇ(latin1_ਸ਼ਾਮਲ_ਹੈ[c]) {
                    ਰੋਕੋ;
                }
            } ਨਹੀਂਤਾਂ ਜੇ(c<=0x7ff) {
                ਜੇ ((table7FF[c & 0x3f] & (static_cast<uint32_t>(1) << (c >> 6))) != 0) {
                    ਰੋਕੋ;
                }
            } ਨਹੀਂਤਾਂ ਜੇ(c<0xd800 || c>=0xe000) {
                ਪੂਰਨ lead=c>>12;
                uint32_t two_ਬਿੱਟ=(bmp_ਖੰਡ_ਬਿੱਟ[(c>>6)&0x3f]>>lead)&0x10001;
                ਜੇ(two_ਬਿੱਟ<=1) {
                    // All 64 code points with the same bits 15..6
                    // are either in the set or not.
                    ਜੇ(two_ਬਿੱਟ!=0) {
                        ਰੋਕੋ;
                    }
                } ਨਹੀਂਤਾਂ {
                    // Look up the code point in its 4k block of code points.
                    ਜੇ(ਸ਼ਾਮਲ_ਹੈ_Slow(c, list4kStarts[lead], list4kStarts[lead+1])) {
                        ਰੋਕੋ;
                    }
                }
            } ਨਹੀਂਤਾਂ ਜੇ(c>=0xdc00 || (s+1)==ਹੱਦਬੰਦੀ || (c2=s[1])<0xdc00 || c2>=0xe000) {
                // surrogate code point
                ਜੇ(ਸ਼ਾਮਲ_ਹੈ_Slow(c, list4kStarts[0xd], list4kStarts[0xe])) {
                    ਰੋਕੋ;
                }
            } ਨਹੀਂਤਾਂ {
                // surrogate pair
                ਜੇ(ਸ਼ਾਮਲ_ਹੈ_Slow(U16_GET_SUPPLEMENTARY(c, c2), list4kStarts[0x10], list4kStarts[0x11])) {
                    ਰੋਕੋ;
                }
                ++s;
            }
        } ਜਦੋਂ(++s<ਹੱਦਬੰਦੀ);
    }
    ਵਾਪਸ s;
}

/* Symmetrical with span(). */
ਅਚਲ char16_t *
BMPSet::ਵਿਸਤਾਰ_ਪਿੱਠ(ਅਚਲ char16_t *s, ਅਚਲ char16_t *ਹੱਦਬੰਦੀ, USet_ਵਿਸਤਾਰ_Condition ਵਿਸਤਾਰ_Condition) ਅਚਲ {
    char16_t c, c2;

    ਜੇ(ਵਿਸਤਾਰ_Condition) {
        //ਵਿਸਤਾਰ
        ਲਈ(;;) {
            c=*(--ਹੱਦਬੰਦੀ);
            ਜੇ(c<=0xff) {
                ਜੇ(!latin1_ਸ਼ਾਮਲ_ਹੈ[c]) {
                    ਰੋਕੋ;
                }
            } ਨਹੀਂਤਾਂ ਜੇ(c<=0x7ff) {
                ਜੇ ((table7FF[c & 0x3f] & (static_cast<uint32_t>(1) << (c >> 6))) == 0) {
                    ਰੋਕੋ;
                }
            } ਨਹੀਂਤਾਂ ਜੇ(c<0xd800 || c>=0xe000) {
                ਪੂਰਨ lead=c>>12;
                uint32_t two_ਬਿੱਟ=(bmp_ਖੰਡ_ਬਿੱਟ[(c>>6)&0x3f]>>lead)&0x10001;
                ਜੇ(two_ਬਿੱਟ<=1) {
                    // All 64 code points with the same bits 15..6
                    // are either in the set or not.
                    ਜੇ(two_ਬਿੱਟ==0) {
                        ਰੋਕੋ;
                    }
                } ਨਹੀਂਤਾਂ {
                    // Look up the code point in its 4k block of code points.
                    ਜੇ(!ਸ਼ਾਮਲ_ਹੈ_Slow(c, list4kStarts[lead], list4kStarts[lead+1])) {
                        ਰੋਕੋ;
                    }
                }
            } ਨਹੀਂਤਾਂ ਜੇ(c<0xdc00 || s==ਹੱਦਬੰਦੀ || (c2=*(ਹੱਦਬੰਦੀ-1))<0xd800 || c2>=0xdc00) {
                // surrogate code point
                ਜੇ(!ਸ਼ਾਮਲ_ਹੈ_Slow(c, list4kStarts[0xd], list4kStarts[0xe])) {
                    ਰੋਕੋ;
                }
            } ਨਹੀਂਤਾਂ {
                // surrogate pair
                ਜੇ(!ਸ਼ਾਮਲ_ਹੈ_Slow(U16_GET_SUPPLEMENTARY(c2, c), list4kStarts[0x10], list4kStarts[0x11])) {
                    ਰੋਕੋ;
                }
                --ਹੱਦਬੰਦੀ;
            }
            ਜੇ(s==ਹੱਦਬੰਦੀ) {
                ਵਾਪਸ s;
            }
        }
    } ਨਹੀਂਤਾਂ {
        // span not
        ਲਈ(;;) {
            c=*(--ਹੱਦਬੰਦੀ);
            ਜੇ(c<=0xff) {
                ਜੇ(latin1_ਸ਼ਾਮਲ_ਹੈ[c]) {
                    ਰੋਕੋ;
                }
            } ਨਹੀਂਤਾਂ ਜੇ(c<=0x7ff) {
                ਜੇ ((table7FF[c & 0x3f] & (static_cast<uint32_t>(1) << (c >> 6))) != 0) {
                    ਰੋਕੋ;
                }
            } ਨਹੀਂਤਾਂ ਜੇ(c<0xd800 || c>=0xe000) {
                ਪੂਰਨ lead=c>>12;
                uint32_t two_ਬਿੱਟ=(bmp_ਖੰਡ_ਬਿੱਟ[(c>>6)&0x3f]>>lead)&0x10001;
                ਜੇ(two_ਬਿੱਟ<=1) {
                    // All 64 code points with the same bits 15..6
                    // are either in the set or not.
                    ਜੇ(two_ਬਿੱਟ!=0) {
                        ਰੋਕੋ;
                    }
                } ਨਹੀਂਤਾਂ {
                    // Look up the code point in its 4k block of code points.
                    ਜੇ(ਸ਼ਾਮਲ_ਹੈ_Slow(c, list4kStarts[lead], list4kStarts[lead+1])) {
                        ਰੋਕੋ;
                    }
                }
            } ਨਹੀਂਤਾਂ ਜੇ(c<0xdc00 || s==ਹੱਦਬੰਦੀ || (c2=*(ਹੱਦਬੰਦੀ-1))<0xd800 || c2>=0xdc00) {
                // surrogate code point
                ਜੇ(ਸ਼ਾਮਲ_ਹੈ_Slow(c, list4kStarts[0xd], list4kStarts[0xe])) {
                    ਰੋਕੋ;
                }
            } ਨਹੀਂਤਾਂ {
                // surrogate pair
                ਜੇ(ਸ਼ਾਮਲ_ਹੈ_Slow(U16_GET_SUPPLEMENTARY(c2, c), list4kStarts[0x10], list4kStarts[0x11])) {
                    ਰੋਕੋ;
                }
                --ਹੱਦਬੰਦੀ;
            }
            ਜੇ(s==ਹੱਦਬੰਦੀ) {
                ਵਾਪਸ s;
            }
        }
    }
    ਵਾਪਸ ਹੱਦਬੰਦੀ+1;
}

/*
 * Precheck for sufficient trail bytes at end of string only once per span.
 * Check validity.
 */
ਅਚਲ uint8_t *
BMPSet::ਵਿਸਤਾਰ_UTF8(ਅਚਲ uint8_t *s, int32_t length, USet_ਵਿਸਤਾਰ_Condition ਵਿਸਤਾਰ_Condition) ਅਚਲ {
    ਅਚਲ uint8_t *ਹੱਦਬੰਦੀ=s+length;
    uint8_t b=*s;
    ਜੇ(U8_IS_SINGLE(b)) {
        // Initial all-ASCII span.
        ਜੇ(ਵਿਸਤਾਰ_Condition) {
            ਕਰੋ {
                ਜੇ(!latin1_ਸ਼ਾਮਲ_ਹੈ[b] || ++s==ਹੱਦਬੰਦੀ) {
                    ਵਾਪਸ s;
                }
                b=*s;
            } ਜਦੋਂ(U8_IS_SINGLE(b));
        } ਨਹੀਂਤਾਂ {
            ਕਰੋ {
                ਜੇ(latin1_ਸ਼ਾਮਲ_ਹੈ[b] || ++s==ਹੱਦਬੰਦੀ) {
                    ਵਾਪਸ s;
                }
                b=*s;
            } ਜਦੋਂ(U8_IS_SINGLE(b));
        }
        length = static_cast<int32_t>(ਹੱਦਬੰਦੀ - s);
    }

    ਜੇ(ਵਿਸਤਾਰ_Condition!=USET_ਵਿਸਤਾਰ_NOT_CONTAINED) {
        ਵਿਸਤਾਰ_Condition=USET_ਵਿਸਤਾਰ_CONTAINED;  // Pin to 0/1 values.
    }

    ਅਚਲ uint8_t *limit0=ਹੱਦਬੰਦੀ;

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
    b=*(ਹੱਦਬੰਦੀ-1);
    ਜੇ (static_cast<int8_t>(b) < 0) {
        // b>=0x80: lead or trail byte
        ਜੇ(b<0xc0) {
            // single trail byte, check for preceding 3- or 4-byte lead byte
            ਜੇ(length>=2 && (b=*(ਹੱਦਬੰਦੀ-2))>=0xe0) {
                ਹੱਦਬੰਦੀ-=2;
                ਜੇ(ਸ਼ਾਮਲ_ਹੈ_FFFD!=ਵਿਸਤਾਰ_Condition) {
                    limit0=ਹੱਦਬੰਦੀ;
                }
            } ਨਹੀਂਤਾਂ ਜੇ(b<0xc0 && b>=0x80 && length>=3 && (b=*(ਹੱਦਬੰਦੀ-3))>=0xf0) {
                // 4-byte lead byte with only two trail bytes
                ਹੱਦਬੰਦੀ-=3;
                ਜੇ(ਸ਼ਾਮਲ_ਹੈ_FFFD!=ਵਿਸਤਾਰ_Condition) {
                    limit0=ਹੱਦਬੰਦੀ;
                }
            }
        } ਨਹੀਂਤਾਂ {
            // lead byte with no trail bytes
            --ਹੱਦਬੰਦੀ;
            ਜੇ(ਸ਼ਾਮਲ_ਹੈ_FFFD!=ਵਿਸਤਾਰ_Condition) {
                limit0=ਹੱਦਬੰਦੀ;
            }
        }
    }

    uint8_t t1, t2, t3;

    ਜਦੋਂ(s<ਹੱਦਬੰਦੀ) {
        b=*s;
        ਜੇ(U8_IS_SINGLE(b)) {
            // ASCII
            ਜੇ(ਵਿਸਤਾਰ_Condition) {
                ਕਰੋ {
                    ਜੇ(!latin1_ਸ਼ਾਮਲ_ਹੈ[b]) {
                        ਵਾਪਸ s;
                    } ਨਹੀਂਤਾਂ ਜੇ(++s==ਹੱਦਬੰਦੀ) {
                        ਵਾਪਸ limit0;
                    }
                    b=*s;
                } ਜਦੋਂ(U8_IS_SINGLE(b));
            } ਨਹੀਂਤਾਂ {
                ਕਰੋ {
                    ਜੇ(latin1_ਸ਼ਾਮਲ_ਹੈ[b]) {
                        ਵਾਪਸ s;
                    } ਨਹੀਂਤਾਂ ਜੇ(++s==ਹੱਦਬੰਦੀ) {
                        ਵਾਪਸ limit0;
                    }
                    b=*s;
                } ਜਦੋਂ(U8_IS_SINGLE(b));
            }
        }
        ++s;  // Advance past the lead byte.
        ਜੇ(b>=0xe0) {
            ਜੇ(b<0xf0) {
                ਜੇ( /* handle U+0000..U+FFFF inline */
                    (t1 = static_cast<uint8_t>(s[0] - 0x80)) <= 0x3f &&
                    (t2 = static_cast<uint8_t>(s[1] - 0x80)) <= 0x3f
                ) {
                    b&=0xf;
                    uint32_t two_ਬਿੱਟ=(bmp_ਖੰਡ_ਬਿੱਟ[t1]>>b)&0x10001;
                    ਜੇ(two_ਬਿੱਟ<=1) {
                        // All 64 code points with this lead byte and middle trail byte
                        // are either in the set or not.
                        ਜੇ (two_ਬਿੱਟ != static_cast<uint32_t>(ਵਿਸਤਾਰ_Condition)) {
                            ਵਾਪਸ s-1;
                        }
                    } ਨਹੀਂਤਾਂ {
                        // Look up the code point in its 4k block of code points.
                        UChar32 c=(b<<12)|(t1<<6)|t2;
                        ਜੇ(ਸ਼ਾਮਲ_ਹੈ_Slow(c, list4kStarts[b], list4kStarts[b+1]) != ਵਿਸਤਾਰ_Condition) {
                            ਵਾਪਸ s-1;
                        }
                    }
                    s+=2;
                    ਜਾਰੀ;
                }
            } ਨਹੀਂਤਾਂ ਜੇ( /* handle U+10000..U+10FFFF inline */
                (t1 = static_cast<uint8_t>(s[0] - 0x80)) <= 0x3f &&
                (t2 = static_cast<uint8_t>(s[1] - 0x80)) <= 0x3f &&
                (t3 = static_cast<uint8_t>(s[2] - 0x80)) <= 0x3f
            ) {
                // Give an illegal sequence the same value as the result of contains(FFFD).
                UChar32 c = (static_cast<UChar32>(b - 0xf0) << 18) | (static_cast<UChar32>(t1) << 12) | (t2 << 6) | t3;
                ਜੇ( (   (0x10000<=c && c<=0x10ffff) ?
                            ਸ਼ਾਮਲ_ਹੈ_Slow(c, list4kStarts[0x10], list4kStarts[0x11]) :
                            ਸ਼ਾਮਲ_ਹੈ_FFFD
                    ) != ਵਿਸਤਾਰ_Condition
                ) {
                    ਵਾਪਸ s-1;
                }
                s+=3;
                ਜਾਰੀ;
            }
        } ਨਹੀਂਤਾਂ {
            ਜੇ( /* handle U+0000..U+07FF inline */
                b>=0xc0 &&
                (t1 = static_cast<uint8_t>(*s - 0x80)) <= 0x3f
            ) {
                ਜੇ (static_cast<USet_ਵਿਸਤਾਰ_Condition>((table7FF[t1] & (static_cast<uint32_t>(1) << (b & 0x1f))) != 0) != ਵਿਸਤਾਰ_Condition) {
                    ਵਾਪਸ s-1;
                }
                ++s;
                ਜਾਰੀ;
            }
        }

        // Give an illegal sequence the same value as the result of contains(FFFD).
        // Handle each byte of an illegal sequence separately to simplify the code;
        // no need to optimize error handling.
        ਜੇ(ਸ਼ਾਮਲ_ਹੈ_FFFD!=ਵਿਸਤਾਰ_Condition) {
            ਵਾਪਸ s-1;
        }
    }

    ਵਾਪਸ limit0;
}

/*
 * While going backwards through UTF-8 optimize only for ASCII.
 * Unlike UTF-16, UTF-8 is not forward-backward symmetrical, that is, it is not
 * possible to tell from the last byte in a multi-byte sequence how many
 * preceding bytes there should be. Therefore, going backwards through UTF-8
 * is much harder than going forward.
 */
int32_t
BMPSet::ਵਿਸਤਾਰ_ਪਿੱਠ_UTF8(ਅਚਲ uint8_t *s, int32_t length, USet_ਵਿਸਤਾਰ_Condition ਵਿਸਤਾਰ_Condition) ਅਚਲ {
    ਜੇ(ਵਿਸਤਾਰ_Condition!=USET_ਵਿਸਤਾਰ_NOT_CONTAINED) {
        ਵਿਸਤਾਰ_Condition=USET_ਵਿਸਤਾਰ_CONTAINED;  // Pin to 0/1 values.
    }

    uint8_t b;

    ਕਰੋ {
        b=s[--length];
        ਜੇ(U8_IS_SINGLE(b)) {
            // ASCII sub-span
            ਜੇ(ਵਿਸਤਾਰ_Condition) {
                ਕਰੋ {
                    ਜੇ(!latin1_ਸ਼ਾਮਲ_ਹੈ[b]) {
                        ਵਾਪਸ length+1;
                    } ਨਹੀਂਤਾਂ ਜੇ(length==0) {
                        ਵਾਪਸ 0;
                    }
                    b=s[--length];
                } ਜਦੋਂ(U8_IS_SINGLE(b));
            } ਨਹੀਂਤਾਂ {
                ਕਰੋ {
                    ਜੇ(latin1_ਸ਼ਾਮਲ_ਹੈ[b]) {
                        ਵਾਪਸ length+1;
                    } ਨਹੀਂਤਾਂ ਜੇ(length==0) {
                        ਵਾਪਸ 0;
                    }
                    b=s[--length];
                } ਜਦੋਂ(U8_IS_SINGLE(b));
            }
        }

        int32_t prev=length;
        UChar32 c;
        // trail byte: collect a multi-byte character
        // (or  lead byte in last-trail position)
        c=utf8_prev_Char_ਸੁਰੱਖਿਅਤ_Body(s, 0, &length, b, -3);
        // c is a valid code point, not ASCII, not a surrogate
        ਜੇ(c<=0x7ff) {
            ਜੇ (static_cast<USet_ਵਿਸਤਾਰ_Condition>((table7FF[c & 0x3f] & (static_cast<uint32_t>(1) << (c >> 6))) != 0) != ਵਿਸਤਾਰ_Condition) {
                ਵਾਪਸ prev+1;
            }
        } ਨਹੀਂਤਾਂ ਜੇ(c<=0xffff) {
            ਪੂਰਨ lead=c>>12;
            uint32_t two_ਬਿੱਟ=(bmp_ਖੰਡ_ਬਿੱਟ[(c>>6)&0x3f]>>lead)&0x10001;
            ਜੇ(two_ਬਿੱਟ<=1) {
                // All 64 code points with the same bits 15..6
                // are either in the set or not.
                ਜੇ (two_ਬਿੱਟ != static_cast<uint32_t>(ਵਿਸਤਾਰ_Condition)) {
                    ਵਾਪਸ prev+1;
                }
            } ਨਹੀਂਤਾਂ {
                // Look up the code point in its 4k block of code points.
                ਜੇ(ਸ਼ਾਮਲ_ਹੈ_Slow(c, list4kStarts[lead], list4kStarts[lead+1]) != ਵਿਸਤਾਰ_Condition) {
                    ਵਾਪਸ prev+1;
                }
            }
        } ਨਹੀਂਤਾਂ {
            ਜੇ(ਸ਼ਾਮਲ_ਹੈ_Slow(c, list4kStarts[0x10], list4kStarts[0x11]) != ਵਿਸਤਾਰ_Condition) {
                ਵਾਪਸ prev+1;
            }
        }
    } ਜਦੋਂ(length>0);
    ਵਾਪਸ 0;
}

U_NAMESPACE_ਸਿਰਾ
