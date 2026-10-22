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

#ಅನ್ತರ್ಭಾವ "unicode/utypes.h"
#ಅನ್ತರ್ಭಾವ "unicode/uniset.h"
#ಅನ್ತರ್ಭಾವ "unicode/utf8.h"
#ಅನ್ತರ್ಭಾವ "unicode/utf16.h"
#ಅನ್ತರ್ಭಾವ "cmemory.h"
#ಅನ್ತರ್ಭಾವ "bmpset.h"
#ಅನ್ತರ್ಭಾವ "uassert.h"

U_NAMESPACE_BEGIN

BMPSet::BMPSet(ಸ್ಥಿರಾಂಕ int32_t *ಪೋಷಕ_List, int32_t ಪೋಷಕ_List_ಉದ್ದಳತೆ) :
        list(ಪೋಷಕ_List), list_ಉದ್ದಳತೆ(ಪೋಷಕ_List_ಉದ್ದಳತೆ) {
    uprv_memset(latin1_ಒಳಗೊಂಡಿದೆ, 0, ಗಾತ್ರ(latin1_ಒಳಗೊಂಡಿದೆ));
    uprv_memset(table7FF, 0, ಗಾತ್ರ(table7FF));
    uprv_memset(bmp_ಖಂಡ_ಬಿಟ್ಟುಗಳು, 0, ಗಾತ್ರ(bmp_ಖಂಡ_ಬಿಟ್ಟುಗಳು));

    /*
     * Set the list indexes for binary searches for
     * U+0800, U+1000, U+2000, .., U+F000, U+10000.
     * U+0800 is the first 3-byte-UTF-8 code point. Lower code points are
     * looked up in the bit tables.
     * The last pair of indexes is for finding supplementary code points.
     */
    list4kStarts[0]=ಕಂಡುಹಿಡಿ_ಸಂಕೇತ_ಬಿಂದು(0x800, 0, list_ಉದ್ದಳತೆ-1);
    int32_t i;
    ಗಾಗಿ(i=1; i<=0x10; ++i) {
        list4kStarts[i]=ಕಂಡುಹಿಡಿ_ಸಂಕೇತ_ಬಿಂದು(i<<12, list4kStarts[i-1], list_ಉದ್ದಳತೆ-1);
    }
    list4kStarts[0x11]=list_ಉದ್ದಳತೆ-1;
    ಒಳಗೊಂಡಿದೆ_FFFD=ಒಳಗೊಂಡಿದೆ_Slow(0xfffd, list4kStarts[0xf], list4kStarts[0x10]);

    init_ಬಿಟ್ಟುಗಳು();
    overrideIllegal();
}

BMPSet::BMPSet(ಸ್ಥಿರಾಂಕ BMPSet &otherBMPSet, ಸ್ಥಿರಾಂಕ int32_t *new_ಪೋಷಕ_List, int32_t new_ಪೋಷಕ_List_ಉದ್ದಳತೆ) :
        ಒಳಗೊಂಡಿದೆ_FFFD(otherBMPSet.ಒಳಗೊಂಡಿದೆ_FFFD),
        list(new_ಪೋಷಕ_List), list_ಉದ್ದಳತೆ(new_ಪೋಷಕ_List_ಉದ್ದಳತೆ) {
    uprv_memcpy(latin1_ಒಳಗೊಂಡಿದೆ, otherBMPSet.latin1_ಒಳಗೊಂಡಿದೆ, ಗಾತ್ರ(latin1_ಒಳಗೊಂಡಿದೆ));
    uprv_memcpy(table7FF, otherBMPSet.table7FF, ಗಾತ್ರ(table7FF));
    uprv_memcpy(bmp_ಖಂಡ_ಬಿಟ್ಟುಗಳು, otherBMPSet.bmp_ಖಂಡ_ಬಿಟ್ಟುಗಳು, ಗಾತ್ರ(bmp_ಖಂಡ_ಬಿಟ್ಟುಗಳು));
    uprv_memcpy(list4kStarts, otherBMPSet.list4kStarts, ಗಾತ್ರ(list4kStarts));
}

BMPSet::~BMPSet() {
}

/*
 * Set bits in a bit rectangle in "vertical" bit organization.
 * start<limit<=0x800
 */
ಸ್ಥಿರ ಶೂನ್ಯ set32x64_ಬಿಟ್ಟುಗಳು(uint32_t ಕೋಷ್ಟಕ[64], int32_t ಆರಂಭ, int32_t ಎಲ್ಲೆ) {
    U_ASSERT(ಆರಂಭ<ಎಲ್ಲೆ);
    U_ASSERT(ಎಲ್ಲೆ<=0x800);

    int32_t lead=ಆರಂಭ>>6;  // Named for UTF-8 2-byte lead byte with upper 5 bits.
    int32_t trail=ಆರಂಭ&0x3f;  // Named for UTF-8 2-byte trail byte with lower 6 bits.

    // Set one bit indicating an all-one block.
    uint32_t ಬಿಟ್ಟುಗಳು = static_cast<uint32_t>(1) << lead;
    ಆದರೆ((ಆರಂಭ+1)==ಎಲ್ಲೆ) {  // Single-character shortcut.
        ಕೋಷ್ಟಕ[trail]|=ಬಿಟ್ಟುಗಳು;
        ಹಿಂತಿರುಗಿ;
    }

    int32_t ಎಲ್ಲೆ_Lead=ಎಲ್ಲೆ>>6;
    int32_t ಎಲ್ಲೆ_Trail=ಎಲ್ಲೆ&0x3f;

    ಆದರೆ(lead==ಎಲ್ಲೆ_Lead) {
        // Partial vertical bit column.
        ಆಗ(trail<ಎಲ್ಲೆ_Trail) {
            ಕೋಷ್ಟಕ[trail++]|=ಬಿಟ್ಟುಗಳು;
        }
    } ಇಲ್ಲದಿದ್ದರೆ {
        // Partial vertical bit column,
        // followed by a bit rectangle,
        // followed by another partial vertical bit column.
        ಆದರೆ(trail>0) {
            ಮಾಡು {
                ಕೋಷ್ಟಕ[trail++]|=ಬಿಟ್ಟುಗಳು;
            } ಆಗ(trail<64);
            ++lead;
        }
        ಆದರೆ(lead<ಎಲ್ಲೆ_Lead) {
            ಬಿಟ್ಟುಗಳು = ~((static_cast<ಚಿಹ್ನೆರಹಿತ>(1) << lead) - 1);
            ಆದರೆ(ಎಲ್ಲೆ_Lead<0x20) {
                ಬಿಟ್ಟುಗಳು &= (static_cast<ಚಿಹ್ನೆರಹಿತ>(1) << ಎಲ್ಲೆ_Lead) - 1;
            }
            ಗಾಗಿ(trail=0; trail<64; ++trail) {
                ಕೋಷ್ಟಕ[trail]|=ಬಿಟ್ಟುಗಳು;
            }
        }
        // limit<=0x800. If limit==0x800 then limitLead=32 and limitTrail=0.
        // In that case, bits=1<<limitLead is undefined but the bits value
        // is not used because trail<limitTrail is already false.
        ಬಿಟ್ಟುಗಳು = static_cast<uint32_t>(1) << ((ಎಲ್ಲೆ_Lead == 0x20) ? (ಎಲ್ಲೆ_Lead - 1) : ಎಲ್ಲೆ_Lead);
        ಗಾಗಿ(trail=0; trail<ಎಲ್ಲೆ_Trail; ++trail) {
            ಕೋಷ್ಟಕ[trail]|=ಬಿಟ್ಟುಗಳು;
        }
    }
}

ಶೂನ್ಯ BMPSet::init_ಬಿಟ್ಟುಗಳು() {
    UChar32 ಆರಂಭ, ಎಲ್ಲೆ;
    int32_t list_ಸೂಚ್ಯಂಕ=0;

    // Set latin1Contains[].
    ಮಾಡು {
        ಆರಂಭ=list[list_ಸೂಚ್ಯಂಕ++];
        ಆದರೆ(list_ಸೂಚ್ಯಂಕ<list_ಉದ್ದಳತೆ) {
            ಎಲ್ಲೆ=list[list_ಸೂಚ್ಯಂಕ++];
        } ಇಲ್ಲದಿದ್ದರೆ {
            ಎಲ್ಲೆ=0x110000;
        }
        ಆದರೆ(ಆರಂಭ>=0x100) {
            ನಿಲ್ಲಿಸು;
        }
        ಮಾಡು {
            latin1_ಒಳಗೊಂಡಿದೆ[ಆರಂಭ++]=1;
        } ಆಗ(ಆರಂಭ<ಎಲ್ಲೆ && ಆರಂಭ<0x100);
    } ಆಗ(ಎಲ್ಲೆ<=0x100);

    // Find the first range overlapping with (or after) 80..FF again,
    // to include them in table7FF as well.
    ಗಾಗಿ(list_ಸೂಚ್ಯಂಕ=0;;) {
        ಆರಂಭ=list[list_ಸೂಚ್ಯಂಕ++];
        ಆದರೆ(list_ಸೂಚ್ಯಂಕ<list_ಉದ್ದಳತೆ) {
            ಎಲ್ಲೆ=list[list_ಸೂಚ್ಯಂಕ++];
        } ಇಲ್ಲದಿದ್ದರೆ {
            ಎಲ್ಲೆ=0x110000;
        }
        ಆದರೆ(ಎಲ್ಲೆ>0x80) {
            ಆದರೆ(ಆರಂಭ<0x80) {
                ಆರಂಭ=0x80;
            }
            ನಿಲ್ಲಿಸು;
        }
    }

    // Set table7FF[].
    ಆಗ(ಆರಂಭ<0x800) {
        set32x64_ಬಿಟ್ಟುಗಳು(table7FF, ಆರಂಭ, ಎಲ್ಲೆ<=0x800 ? ಎಲ್ಲೆ : 0x800);
        ಆದರೆ(ಎಲ್ಲೆ>0x800) {
            ಆರಂಭ=0x800;
            ನಿಲ್ಲಿಸು;
        }

        ಆರಂಭ=list[list_ಸೂಚ್ಯಂಕ++];
        ಆದರೆ(list_ಸೂಚ್ಯಂಕ<list_ಉದ್ದಳತೆ) {
            ಎಲ್ಲೆ=list[list_ಸೂಚ್ಯಂಕ++];
        } ಇಲ್ಲದಿದ್ದರೆ {
            ಎಲ್ಲೆ=0x110000;
        }
    }

    // Set bmpBlockBits[].
    int32_t min_ಆರಂಭ=0x800;
    ಆಗ(ಆರಂಭ<0x10000) {
        ಆದರೆ(ಎಲ್ಲೆ>0x10000) {
            ಎಲ್ಲೆ=0x10000;
        }

        ಆದರೆ(ಆರಂಭ<min_ಆರಂಭ) {
            ಆರಂಭ=min_ಆರಂಭ;
        }
        ಆದರೆ(ಆರಂಭ<ಎಲ್ಲೆ) {  // Else: Another range entirely in a known mixed-value block.
            ಆದರೆ(ಆರಂಭ&0x3f) {
                // Mixed-value block of 64 code points.
                ಆರಂಭ>>=6;
                bmp_ಖಂಡ_ಬಿಟ್ಟುಗಳು[ಆರಂಭ&0x3f]|=0x10001<<(ಆರಂಭ>>6);
                ಆರಂಭ=(ಆರಂಭ+1)<<6;  // Round up to the next block boundary.
                min_ಆರಂಭ=ಆರಂಭ;      // Ignore further ranges in this block.
            }
            ಆದರೆ(ಆರಂಭ<ಎಲ್ಲೆ) {
                ಆದರೆ(ಆರಂಭ<(ಎಲ್ಲೆ&~0x3f)) {
                    // Multiple all-ones blocks of 64 code points each.
                    set32x64_ಬಿಟ್ಟುಗಳು(bmp_ಖಂಡ_ಬಿಟ್ಟುಗಳು, ಆರಂಭ>>6, ಎಲ್ಲೆ>>6);
                }

                ಆದರೆ(ಎಲ್ಲೆ&0x3f) {
                    // Mixed-value block of 64 code points.
                    ಎಲ್ಲೆ>>=6;
                    bmp_ಖಂಡ_ಬಿಟ್ಟುಗಳು[ಎಲ್ಲೆ&0x3f]|=0x10001<<(ಎಲ್ಲೆ>>6);
                    ಎಲ್ಲೆ=(ಎಲ್ಲೆ+1)<<6;  // Round up to the next block boundary.
                    min_ಆರಂಭ=ಎಲ್ಲೆ;      // Ignore further ranges in this block.
                }
            }
        }

        ಆದರೆ(ಎಲ್ಲೆ==0x10000) {
            ನಿಲ್ಲಿಸು;
        }

        ಆರಂಭ=list[list_ಸೂಚ್ಯಂಕ++];
        ಆದರೆ(list_ಸೂಚ್ಯಂಕ<list_ಉದ್ದಳತೆ) {
            ಎಲ್ಲೆ=list[list_ಸೂಚ್ಯಂಕ++];
        } ಇಲ್ಲದಿದ್ದರೆ {
            ಎಲ್ಲೆ=0x110000;
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
ಶೂನ್ಯ BMPSet::overrideIllegal() {
    uint32_t ಬಿಟ್ಟುಗಳು, mask;
    int32_t i;

    ಆದರೆ(ಒಳಗೊಂಡಿದೆ_FFFD) {
        ಬಿಟ್ಟುಗಳು=3;                 // Lead bytes 0xC0 and 0xC1.
        ಗಾಗಿ(i=0; i<64; ++i) {
            table7FF[i]|=ಬಿಟ್ಟುಗಳು;
        }

        ಬಿಟ್ಟುಗಳು=1;                 // Lead byte 0xE0.
        ಗಾಗಿ(i=0; i<32; ++i) {   // First half of 4k block.
            bmp_ಖಂಡ_ಬಿಟ್ಟುಗಳು[i]|=ಬಿಟ್ಟುಗಳು;
        }

        mask= static_cast<uint32_t>(~(0x10001<<0xd));   // Lead byte 0xED.
        ಬಿಟ್ಟುಗಳು=1<<0xd;
        ಗಾಗಿ(i=32; i<64; ++i) {  // Second half of 4k block.
            bmp_ಖಂಡ_ಬಿಟ್ಟುಗಳು[i]=(bmp_ಖಂಡ_ಬಿಟ್ಟುಗಳು[i]&mask)|ಬಿಟ್ಟುಗಳು;
        }
    } ಇಲ್ಲದಿದ್ದರೆ {
        mask= static_cast<uint32_t>(~(0x10001<<0xd));   // Lead byte 0xED.
        ಗಾಗಿ(i=32; i<64; ++i) {  // Second half of 4k block.
            bmp_ಖಂಡ_ಬಿಟ್ಟುಗಳು[i]&=mask;
        }
    }
}

int32_t BMPSet::ಕಂಡುಹಿಡಿ_ಸಂಕೇತ_ಬಿಂದು(UChar32 c, int32_t lo, int32_t hi) ಸ್ಥಿರಾಂಕ {
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
    ಆದರೆ (c < list[lo])
        ಹಿಂತಿರುಗಿ lo;
    // High runner test.  c is often after the last range, so an
    // initial check for this condition pays off.
    ಆದರೆ (lo >= hi || c >= list[hi-1])
        ಹಿಂತಿರುಗಿ hi;
    // invariant: c >= list[lo]
    // invariant: c < list[hi]
    ಗಾಗಿ (;;) {
        int32_t i = (lo + hi) >> 1;
        ಆದರೆ (i == lo) {
            ನಿಲ್ಲಿಸು; // Found!
        } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (c < list[i]) {
            hi = i;
        } ಇಲ್ಲದಿದ್ದರೆ {
            lo = i;
        }
    }
    ಹಿಂತಿರುಗಿ hi;
}

UBool
BMPSet::ಒಳಗೊಂಡಿದೆ(UChar32 c) ಸ್ಥಿರಾಂಕ {
    ಆದರೆ (static_cast<uint32_t>(c) <= 0xff) {
        ಹಿಂತಿರುಗಿ latin1_ಒಳಗೊಂಡಿದೆ[c];
    } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (static_cast<uint32_t>(c) <= 0x7ff) {
        ಹಿಂತಿರುಗಿ (table7FF[c & 0x3f] & (static_cast<uint32_t>(1) << (c >> 6))) != 0;
    } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (static_cast<uint32_t>(c) < 0xd800 || (c >= 0xe000 && c <= 0xffff)) {
        ಪೂರ್ಣಾಂಕ lead=c>>12;
        uint32_t two_ಬಿಟ್ಟುಗಳು=(bmp_ಖಂಡ_ಬಿಟ್ಟುಗಳು[(c>>6)&0x3f]>>lead)&0x10001;
        ಆದರೆ(two_ಬಿಟ್ಟುಗಳು<=1) {
            // All 64 code points with the same bits 15..6
            // are either in the set or not.
            ಹಿಂತಿರುಗಿ two_ಬಿಟ್ಟುಗಳು;
        } ಇಲ್ಲದಿದ್ದರೆ {
            // Look up the code point in its 4k block of code points.
            ಹಿಂತಿರುಗಿ ಒಳಗೊಂಡಿದೆ_Slow(c, list4kStarts[lead], list4kStarts[lead+1]);
        }
    } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (static_cast<uint32_t>(c) <= 0x10ffff) {
        // surrogate or supplementary code point
        ಹಿಂತಿರುಗಿ ಒಳಗೊಂಡಿದೆ_Slow(c, list4kStarts[0xd], list4kStarts[0x11]);
    } ಇಲ್ಲದಿದ್ದರೆ {
        // Out-of-range code points get false, consistent with long-standing
        // behavior of UnicodeSet::contains(c).
        ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
    }
}

/*
 * Check for sufficient length for trail unit for each surrogate pair.
 * Handle single surrogates as surrogate code points as usual in ICU.
 */
ಸ್ಥಿರಾಂಕ char16_t *
BMPSet::ಹರವು(ಸ್ಥಿರಾಂಕ char16_t *s, ಸ್ಥಿರಾಂಕ char16_t *ಎಲ್ಲೆ, USet_ಹರವು_Condition ಹರವು_Condition) ಸ್ಥಿರಾಂಕ {
    char16_t c, c2;

    ಆದರೆ(ಹರವು_Condition) {
        //ಹರವು
        ಮಾಡು {
            c=*s;
            ಆದರೆ(c<=0xff) {
                ಆದರೆ(!latin1_ಒಳಗೊಂಡಿದೆ[c]) {
                    ನಿಲ್ಲಿಸು;
                }
            } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(c<=0x7ff) {
                ಆದರೆ ((table7FF[c & 0x3f] & (static_cast<uint32_t>(1) << (c >> 6))) == 0) {
                    ನಿಲ್ಲಿಸು;
                }
            } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(c<0xd800 || c>=0xe000) {
                ಪೂರ್ಣಾಂಕ lead=c>>12;
                uint32_t two_ಬಿಟ್ಟುಗಳು=(bmp_ಖಂಡ_ಬಿಟ್ಟುಗಳು[(c>>6)&0x3f]>>lead)&0x10001;
                ಆದರೆ(two_ಬಿಟ್ಟುಗಳು<=1) {
                    // All 64 code points with the same bits 15..6
                    // are either in the set or not.
                    ಆದರೆ(two_ಬಿಟ್ಟುಗಳು==0) {
                        ನಿಲ್ಲಿಸು;
                    }
                } ಇಲ್ಲದಿದ್ದರೆ {
                    // Look up the code point in its 4k block of code points.
                    ಆದರೆ(!ಒಳಗೊಂಡಿದೆ_Slow(c, list4kStarts[lead], list4kStarts[lead+1])) {
                        ನಿಲ್ಲಿಸು;
                    }
                }
            } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(c>=0xdc00 || (s+1)==ಎಲ್ಲೆ || (c2=s[1])<0xdc00 || c2>=0xe000) {
                // surrogate code point
                ಆದರೆ(!ಒಳಗೊಂಡಿದೆ_Slow(c, list4kStarts[0xd], list4kStarts[0xe])) {
                    ನಿಲ್ಲಿಸು;
                }
            } ಇಲ್ಲದಿದ್ದರೆ {
                // surrogate pair
                ಆದರೆ(!ಒಳಗೊಂಡಿದೆ_Slow(U16_GET_SUPPLEMENTARY(c, c2), list4kStarts[0x10], list4kStarts[0x11])) {
                    ನಿಲ್ಲಿಸು;
                }
                ++s;
            }
        } ಆಗ(++s<ಎಲ್ಲೆ);
    } ಇಲ್ಲದಿದ್ದರೆ {
        // span not
        ಮಾಡು {
            c=*s;
            ಆದರೆ(c<=0xff) {
                ಆದರೆ(latin1_ಒಳಗೊಂಡಿದೆ[c]) {
                    ನಿಲ್ಲಿಸು;
                }
            } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(c<=0x7ff) {
                ಆದರೆ ((table7FF[c & 0x3f] & (static_cast<uint32_t>(1) << (c >> 6))) != 0) {
                    ನಿಲ್ಲಿಸು;
                }
            } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(c<0xd800 || c>=0xe000) {
                ಪೂರ್ಣಾಂಕ lead=c>>12;
                uint32_t two_ಬಿಟ್ಟುಗಳು=(bmp_ಖಂಡ_ಬಿಟ್ಟುಗಳು[(c>>6)&0x3f]>>lead)&0x10001;
                ಆದರೆ(two_ಬಿಟ್ಟುಗಳು<=1) {
                    // All 64 code points with the same bits 15..6
                    // are either in the set or not.
                    ಆದರೆ(two_ಬಿಟ್ಟುಗಳು!=0) {
                        ನಿಲ್ಲಿಸು;
                    }
                } ಇಲ್ಲದಿದ್ದರೆ {
                    // Look up the code point in its 4k block of code points.
                    ಆದರೆ(ಒಳಗೊಂಡಿದೆ_Slow(c, list4kStarts[lead], list4kStarts[lead+1])) {
                        ನಿಲ್ಲಿಸು;
                    }
                }
            } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(c>=0xdc00 || (s+1)==ಎಲ್ಲೆ || (c2=s[1])<0xdc00 || c2>=0xe000) {
                // surrogate code point
                ಆದರೆ(ಒಳಗೊಂಡಿದೆ_Slow(c, list4kStarts[0xd], list4kStarts[0xe])) {
                    ನಿಲ್ಲಿಸು;
                }
            } ಇಲ್ಲದಿದ್ದರೆ {
                // surrogate pair
                ಆದರೆ(ಒಳಗೊಂಡಿದೆ_Slow(U16_GET_SUPPLEMENTARY(c, c2), list4kStarts[0x10], list4kStarts[0x11])) {
                    ನಿಲ್ಲಿಸು;
                }
                ++s;
            }
        } ಆಗ(++s<ಎಲ್ಲೆ);
    }
    ಹಿಂತಿರುಗಿ s;
}

/* Symmetrical with span(). */
ಸ್ಥಿರಾಂಕ char16_t *
BMPSet::ಹರವು_ಹಿಂಭಾಗ(ಸ್ಥಿರಾಂಕ char16_t *s, ಸ್ಥಿರಾಂಕ char16_t *ಎಲ್ಲೆ, USet_ಹರವು_Condition ಹರವು_Condition) ಸ್ಥಿರಾಂಕ {
    char16_t c, c2;

    ಆದರೆ(ಹರವು_Condition) {
        //ಹರವು
        ಗಾಗಿ(;;) {
            c=*(--ಎಲ್ಲೆ);
            ಆದರೆ(c<=0xff) {
                ಆದರೆ(!latin1_ಒಳಗೊಂಡಿದೆ[c]) {
                    ನಿಲ್ಲಿಸು;
                }
            } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(c<=0x7ff) {
                ಆದರೆ ((table7FF[c & 0x3f] & (static_cast<uint32_t>(1) << (c >> 6))) == 0) {
                    ನಿಲ್ಲಿಸು;
                }
            } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(c<0xd800 || c>=0xe000) {
                ಪೂರ್ಣಾಂಕ lead=c>>12;
                uint32_t two_ಬಿಟ್ಟುಗಳು=(bmp_ಖಂಡ_ಬಿಟ್ಟುಗಳು[(c>>6)&0x3f]>>lead)&0x10001;
                ಆದರೆ(two_ಬಿಟ್ಟುಗಳು<=1) {
                    // All 64 code points with the same bits 15..6
                    // are either in the set or not.
                    ಆದರೆ(two_ಬಿಟ್ಟುಗಳು==0) {
                        ನಿಲ್ಲಿಸು;
                    }
                } ಇಲ್ಲದಿದ್ದರೆ {
                    // Look up the code point in its 4k block of code points.
                    ಆದರೆ(!ಒಳಗೊಂಡಿದೆ_Slow(c, list4kStarts[lead], list4kStarts[lead+1])) {
                        ನಿಲ್ಲಿಸು;
                    }
                }
            } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(c<0xdc00 || s==ಎಲ್ಲೆ || (c2=*(ಎಲ್ಲೆ-1))<0xd800 || c2>=0xdc00) {
                // surrogate code point
                ಆದರೆ(!ಒಳಗೊಂಡಿದೆ_Slow(c, list4kStarts[0xd], list4kStarts[0xe])) {
                    ನಿಲ್ಲಿಸು;
                }
            } ಇಲ್ಲದಿದ್ದರೆ {
                // surrogate pair
                ಆದರೆ(!ಒಳಗೊಂಡಿದೆ_Slow(U16_GET_SUPPLEMENTARY(c2, c), list4kStarts[0x10], list4kStarts[0x11])) {
                    ನಿಲ್ಲಿಸು;
                }
                --ಎಲ್ಲೆ;
            }
            ಆದರೆ(s==ಎಲ್ಲೆ) {
                ಹಿಂತಿರುಗಿ s;
            }
        }
    } ಇಲ್ಲದಿದ್ದರೆ {
        // span not
        ಗಾಗಿ(;;) {
            c=*(--ಎಲ್ಲೆ);
            ಆದರೆ(c<=0xff) {
                ಆದರೆ(latin1_ಒಳಗೊಂಡಿದೆ[c]) {
                    ನಿಲ್ಲಿಸು;
                }
            } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(c<=0x7ff) {
                ಆದರೆ ((table7FF[c & 0x3f] & (static_cast<uint32_t>(1) << (c >> 6))) != 0) {
                    ನಿಲ್ಲಿಸು;
                }
            } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(c<0xd800 || c>=0xe000) {
                ಪೂರ್ಣಾಂಕ lead=c>>12;
                uint32_t two_ಬಿಟ್ಟುಗಳು=(bmp_ಖಂಡ_ಬಿಟ್ಟುಗಳು[(c>>6)&0x3f]>>lead)&0x10001;
                ಆದರೆ(two_ಬಿಟ್ಟುಗಳು<=1) {
                    // All 64 code points with the same bits 15..6
                    // are either in the set or not.
                    ಆದರೆ(two_ಬಿಟ್ಟುಗಳು!=0) {
                        ನಿಲ್ಲಿಸು;
                    }
                } ಇಲ್ಲದಿದ್ದರೆ {
                    // Look up the code point in its 4k block of code points.
                    ಆದರೆ(ಒಳಗೊಂಡಿದೆ_Slow(c, list4kStarts[lead], list4kStarts[lead+1])) {
                        ನಿಲ್ಲಿಸು;
                    }
                }
            } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(c<0xdc00 || s==ಎಲ್ಲೆ || (c2=*(ಎಲ್ಲೆ-1))<0xd800 || c2>=0xdc00) {
                // surrogate code point
                ಆದರೆ(ಒಳಗೊಂಡಿದೆ_Slow(c, list4kStarts[0xd], list4kStarts[0xe])) {
                    ನಿಲ್ಲಿಸು;
                }
            } ಇಲ್ಲದಿದ್ದರೆ {
                // surrogate pair
                ಆದರೆ(ಒಳಗೊಂಡಿದೆ_Slow(U16_GET_SUPPLEMENTARY(c2, c), list4kStarts[0x10], list4kStarts[0x11])) {
                    ನಿಲ್ಲಿಸು;
                }
                --ಎಲ್ಲೆ;
            }
            ಆದರೆ(s==ಎಲ್ಲೆ) {
                ಹಿಂತಿರುಗಿ s;
            }
        }
    }
    ಹಿಂತಿರುಗಿ ಎಲ್ಲೆ+1;
}

/*
 * Precheck for sufficient trail bytes at end of string only once per span.
 * Check validity.
 */
ಸ್ಥಿರಾಂಕ uint8_t *
BMPSet::ಹರವು_UTF8(ಸ್ಥಿರಾಂಕ uint8_t *s, int32_t length, USet_ಹರವು_Condition ಹರವು_Condition) ಸ್ಥಿರಾಂಕ {
    ಸ್ಥಿರಾಂಕ uint8_t *ಎಲ್ಲೆ=s+length;
    uint8_t b=*s;
    ಆದರೆ(U8_IS_SINGLE(b)) {
        // Initial all-ASCII span.
        ಆದರೆ(ಹರವು_Condition) {
            ಮಾಡು {
                ಆದರೆ(!latin1_ಒಳಗೊಂಡಿದೆ[b] || ++s==ಎಲ್ಲೆ) {
                    ಹಿಂತಿರುಗಿ s;
                }
                b=*s;
            } ಆಗ(U8_IS_SINGLE(b));
        } ಇಲ್ಲದಿದ್ದರೆ {
            ಮಾಡು {
                ಆದರೆ(latin1_ಒಳಗೊಂಡಿದೆ[b] || ++s==ಎಲ್ಲೆ) {
                    ಹಿಂತಿರುಗಿ s;
                }
                b=*s;
            } ಆಗ(U8_IS_SINGLE(b));
        }
        length = static_cast<int32_t>(ಎಲ್ಲೆ - s);
    }

    ಆದರೆ(ಹರವು_Condition!=USET_ಹರವು_NOT_CONTAINED) {
        ಹರವು_Condition=USET_ಹರವು_CONTAINED;  // Pin to 0/1 values.
    }

    ಸ್ಥಿರಾಂಕ uint8_t *limit0=ಎಲ್ಲೆ;

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
    b=*(ಎಲ್ಲೆ-1);
    ಆದರೆ (static_cast<int8_t>(b) < 0) {
        // b>=0x80: lead or trail byte
        ಆದರೆ(b<0xc0) {
            // single trail byte, check for preceding 3- or 4-byte lead byte
            ಆದರೆ(length>=2 && (b=*(ಎಲ್ಲೆ-2))>=0xe0) {
                ಎಲ್ಲೆ-=2;
                ಆದರೆ(ಒಳಗೊಂಡಿದೆ_FFFD!=ಹರವು_Condition) {
                    limit0=ಎಲ್ಲೆ;
                }
            } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(b<0xc0 && b>=0x80 && length>=3 && (b=*(ಎಲ್ಲೆ-3))>=0xf0) {
                // 4-byte lead byte with only two trail bytes
                ಎಲ್ಲೆ-=3;
                ಆದರೆ(ಒಳಗೊಂಡಿದೆ_FFFD!=ಹರವು_Condition) {
                    limit0=ಎಲ್ಲೆ;
                }
            }
        } ಇಲ್ಲದಿದ್ದರೆ {
            // lead byte with no trail bytes
            --ಎಲ್ಲೆ;
            ಆದರೆ(ಒಳಗೊಂಡಿದೆ_FFFD!=ಹರವು_Condition) {
                limit0=ಎಲ್ಲೆ;
            }
        }
    }

    uint8_t t1, t2, t3;

    ಆಗ(s<ಎಲ್ಲೆ) {
        b=*s;
        ಆದರೆ(U8_IS_SINGLE(b)) {
            // ASCII
            ಆದರೆ(ಹರವು_Condition) {
                ಮಾಡು {
                    ಆದರೆ(!latin1_ಒಳಗೊಂಡಿದೆ[b]) {
                        ಹಿಂತಿರುಗಿ s;
                    } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(++s==ಎಲ್ಲೆ) {
                        ಹಿಂತಿರುಗಿ limit0;
                    }
                    b=*s;
                } ಆಗ(U8_IS_SINGLE(b));
            } ಇಲ್ಲದಿದ್ದರೆ {
                ಮಾಡು {
                    ಆದರೆ(latin1_ಒಳಗೊಂಡಿದೆ[b]) {
                        ಹಿಂತಿರುಗಿ s;
                    } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(++s==ಎಲ್ಲೆ) {
                        ಹಿಂತಿರುಗಿ limit0;
                    }
                    b=*s;
                } ಆಗ(U8_IS_SINGLE(b));
            }
        }
        ++s;  // Advance past the lead byte.
        ಆದರೆ(b>=0xe0) {
            ಆದರೆ(b<0xf0) {
                ಆದರೆ( /* handle U+0000..U+FFFF inline */
                    (t1 = static_cast<uint8_t>(s[0] - 0x80)) <= 0x3f &&
                    (t2 = static_cast<uint8_t>(s[1] - 0x80)) <= 0x3f
                ) {
                    b&=0xf;
                    uint32_t two_ಬಿಟ್ಟುಗಳು=(bmp_ಖಂಡ_ಬಿಟ್ಟುಗಳು[t1]>>b)&0x10001;
                    ಆದರೆ(two_ಬಿಟ್ಟುಗಳು<=1) {
                        // All 64 code points with this lead byte and middle trail byte
                        // are either in the set or not.
                        ಆದರೆ (two_ಬಿಟ್ಟುಗಳು != static_cast<uint32_t>(ಹರವು_Condition)) {
                            ಹಿಂತಿರುಗಿ s-1;
                        }
                    } ಇಲ್ಲದಿದ್ದರೆ {
                        // Look up the code point in its 4k block of code points.
                        UChar32 c=(b<<12)|(t1<<6)|t2;
                        ಆದರೆ(ಒಳಗೊಂಡಿದೆ_Slow(c, list4kStarts[b], list4kStarts[b+1]) != ಹರವು_Condition) {
                            ಹಿಂತಿರುಗಿ s-1;
                        }
                    }
                    s+=2;
                    ಮುಂದುವರಿ;
                }
            } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ( /* handle U+10000..U+10FFFF inline */
                (t1 = static_cast<uint8_t>(s[0] - 0x80)) <= 0x3f &&
                (t2 = static_cast<uint8_t>(s[1] - 0x80)) <= 0x3f &&
                (t3 = static_cast<uint8_t>(s[2] - 0x80)) <= 0x3f
            ) {
                // Give an illegal sequence the same value as the result of contains(FFFD).
                UChar32 c = (static_cast<UChar32>(b - 0xf0) << 18) | (static_cast<UChar32>(t1) << 12) | (t2 << 6) | t3;
                ಆದರೆ( (   (0x10000<=c && c<=0x10ffff) ?
                            ಒಳಗೊಂಡಿದೆ_Slow(c, list4kStarts[0x10], list4kStarts[0x11]) :
                            ಒಳಗೊಂಡಿದೆ_FFFD
                    ) != ಹರವು_Condition
                ) {
                    ಹಿಂತಿರುಗಿ s-1;
                }
                s+=3;
                ಮುಂದುವರಿ;
            }
        } ಇಲ್ಲದಿದ್ದರೆ {
            ಆದರೆ( /* handle U+0000..U+07FF inline */
                b>=0xc0 &&
                (t1 = static_cast<uint8_t>(*s - 0x80)) <= 0x3f
            ) {
                ಆದರೆ (static_cast<USet_ಹರವು_Condition>((table7FF[t1] & (static_cast<uint32_t>(1) << (b & 0x1f))) != 0) != ಹರವು_Condition) {
                    ಹಿಂತಿರುಗಿ s-1;
                }
                ++s;
                ಮುಂದುವರಿ;
            }
        }

        // Give an illegal sequence the same value as the result of contains(FFFD).
        // Handle each byte of an illegal sequence separately to simplify the code;
        // no need to optimize error handling.
        ಆದರೆ(ಒಳಗೊಂಡಿದೆ_FFFD!=ಹರವು_Condition) {
            ಹಿಂತಿರುಗಿ s-1;
        }
    }

    ಹಿಂತಿರುಗಿ limit0;
}

/*
 * While going backwards through UTF-8 optimize only for ASCII.
 * Unlike UTF-16, UTF-8 is not forward-backward symmetrical, that is, it is not
 * possible to tell from the last byte in a multi-byte sequence how many
 * preceding bytes there should be. Therefore, going backwards through UTF-8
 * is much harder than going forward.
 */
int32_t
BMPSet::ಹರವು_ಹಿಂಭಾಗ_UTF8(ಸ್ಥಿರಾಂಕ uint8_t *s, int32_t length, USet_ಹರವು_Condition ಹರವು_Condition) ಸ್ಥಿರಾಂಕ {
    ಆದರೆ(ಹರವು_Condition!=USET_ಹರವು_NOT_CONTAINED) {
        ಹರವು_Condition=USET_ಹರವು_CONTAINED;  // Pin to 0/1 values.
    }

    uint8_t b;

    ಮಾಡು {
        b=s[--length];
        ಆದರೆ(U8_IS_SINGLE(b)) {
            // ASCII sub-span
            ಆದರೆ(ಹರವು_Condition) {
                ಮಾಡು {
                    ಆದರೆ(!latin1_ಒಳಗೊಂಡಿದೆ[b]) {
                        ಹಿಂತಿರುಗಿ length+1;
                    } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(length==0) {
                        ಹಿಂತಿರುಗಿ 0;
                    }
                    b=s[--length];
                } ಆಗ(U8_IS_SINGLE(b));
            } ಇಲ್ಲದಿದ್ದರೆ {
                ಮಾಡು {
                    ಆದರೆ(latin1_ಒಳಗೊಂಡಿದೆ[b]) {
                        ಹಿಂತಿರುಗಿ length+1;
                    } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(length==0) {
                        ಹಿಂತಿರುಗಿ 0;
                    }
                    b=s[--length];
                } ಆಗ(U8_IS_SINGLE(b));
            }
        }

        int32_t prev=length;
        UChar32 c;
        // trail byte: collect a multi-byte character
        // (or  lead byte in last-trail position)
        c=utf8_prev_Char_ಸುರಕ್ಷಿತ_Body(s, 0, &length, b, -3);
        // c is a valid code point, not ASCII, not a surrogate
        ಆದರೆ(c<=0x7ff) {
            ಆದರೆ (static_cast<USet_ಹರವು_Condition>((table7FF[c & 0x3f] & (static_cast<uint32_t>(1) << (c >> 6))) != 0) != ಹರವು_Condition) {
                ಹಿಂತಿರುಗಿ prev+1;
            }
        } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(c<=0xffff) {
            ಪೂರ್ಣಾಂಕ lead=c>>12;
            uint32_t two_ಬಿಟ್ಟುಗಳು=(bmp_ಖಂಡ_ಬಿಟ್ಟುಗಳು[(c>>6)&0x3f]>>lead)&0x10001;
            ಆದರೆ(two_ಬಿಟ್ಟುಗಳು<=1) {
                // All 64 code points with the same bits 15..6
                // are either in the set or not.
                ಆದರೆ (two_ಬಿಟ್ಟುಗಳು != static_cast<uint32_t>(ಹರವು_Condition)) {
                    ಹಿಂತಿರುಗಿ prev+1;
                }
            } ಇಲ್ಲದಿದ್ದರೆ {
                // Look up the code point in its 4k block of code points.
                ಆದರೆ(ಒಳಗೊಂಡಿದೆ_Slow(c, list4kStarts[lead], list4kStarts[lead+1]) != ಹರವು_Condition) {
                    ಹಿಂತಿರುಗಿ prev+1;
                }
            }
        } ಇಲ್ಲದಿದ್ದರೆ {
            ಆದರೆ(ಒಳಗೊಂಡಿದೆ_Slow(c, list4kStarts[0x10], list4kStarts[0x11]) != ಹರವು_Condition) {
                ಹಿಂತಿರುಗಿ prev+1;
            }
        }
    } ಆಗ(length>0);
    ಹಿಂತಿರುಗಿ 0;
}

U_NAMESPACE_ಕೊನೆ
