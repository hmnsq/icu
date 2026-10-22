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

BMPSet::BMPSet(ಧ್ರುವ int32_t *parentList, int32_t parentListLength) :
        list(parentList), listLength(parentListLength) {
    uprv_memset(latin1Contains, 0, ಪರಿಮಾಣ(latin1Contains));
    uprv_memset(table7FF, 0, ಪರಿಮಾಣ(table7FF));
    uprv_memset(bmpBlockBits, 0, ಪರಿಮಾಣ(bmpBlockBits));

    /*
     * Set the list indexes for binary searches for
     * U+0800, U+1000, U+2000, .., U+F000, U+10000.
     * U+0800 is the first 3-byte-UTF-8 code point. Lower code points are
     * looked up in the bit tables.
     * The last pair of indexes is for finding supplementary code points.
     */
    list4kStarts[0]=findCodePoint(0x800, 0, listLength-1);
    int32_t i;
    ಪ್ರತಿ(i=1; i<=0x10; ++i) {
        list4kStarts[i]=findCodePoint(i<<12, list4kStarts[i-1], listLength-1);
    }
    list4kStarts[0x11]=listLength-1;
    containsFFFD=containsSlow(0xfffd, list4kStarts[0xf], list4kStarts[0x10]);

    initBits();
    overrideIllegal();
}

BMPSet::BMPSet(ಧ್ರುವ BMPSet &otherBMPSet, ಧ್ರುವ int32_t *newParentList, int32_t newParentListLength) :
        containsFFFD(otherBMPSet.containsFFFD),
        list(newParentList), listLength(newParentListLength) {
    uprv_memcpy(latin1Contains, otherBMPSet.latin1Contains, ಪರಿಮಾಣ(latin1Contains));
    uprv_memcpy(table7FF, otherBMPSet.table7FF, ಪರಿಮಾಣ(table7FF));
    uprv_memcpy(bmpBlockBits, otherBMPSet.bmpBlockBits, ಪರಿಮಾಣ(bmpBlockBits));
    uprv_memcpy(list4kStarts, otherBMPSet.list4kStarts, ಪರಿಮಾಣ(list4kStarts));
}

BMPSet::~BMPSet() {
}

/*
 * Set bits in a bit rectangle in "vertical" bit organization.
 * start<limit<=0x800
 */
ಸ್ಥಿರ ಶೂನ್ಯ set32x64Bits(uint32_t table[64], int32_t start, int32_t limit) {
    U_ASSERT(start<limit);
    U_ASSERT(limit<=0x800);

    int32_t lead=start>>6;  // Named for UTF-8 2-byte lead byte with upper 5 bits.
    int32_t trail=start&0x3f;  // Named for UTF-8 2-byte trail byte with lower 6 bits.

    // Set one bit indicating an all-one block.
    uint32_t bits = static_cast<uint32_t>(1) << lead;
    ಯದಿ((start+1)==limit) {  // Single-character shortcut.
        table[trail]|=bits;
        ಪ್ರತ್ಯಾಗಮ;
    }

    int32_t limitLead=limit>>6;
    int32_t limitTrail=limit&0x3f;

    ಯದಿ(lead==limitLead) {
        // Partial vertical bit column.
        ಯಾವತ್(trail<limitTrail) {
            table[trail++]|=bits;
        }
    } ಅನ್ಯಥಾ {
        // Partial vertical bit column,
        // followed by a bit rectangle,
        // followed by another partial vertical bit column.
        ಯದಿ(trail>0) {
            ಕುರು {
                table[trail++]|=bits;
            } ಯಾವತ್(trail<64);
            ++lead;
        }
        ಯದಿ(lead<limitLead) {
            bits = ~((static_cast<unsigned>(1) << lead) - 1);
            ಯದಿ(limitLead<0x20) {
                bits &= (static_cast<unsigned>(1) << limitLead) - 1;
            }
            ಪ್ರತಿ(trail=0; trail<64; ++trail) {
                table[trail]|=bits;
            }
        }
        // limit<=0x800. If limit==0x800 then limitLead=32 and limitTrail=0.
        // In that case, bits=1<<limitLead is undefined but the bits value
        // is not used because trail<limitTrail is already false.
        bits = static_cast<uint32_t>(1) << ((limitLead == 0x20) ? (limitLead - 1) : limitLead);
        ಪ್ರತಿ(trail=0; trail<limitTrail; ++trail) {
            table[trail]|=bits;
        }
    }
}

ಶೂನ್ಯ BMPSet::initBits() {
    UChar32 start, limit;
    int32_t listIndex=0;

    // Set latin1Contains[].
    ಕುರು {
        start=list[listIndex++];
        ಯದಿ(listIndex<listLength) {
            limit=list[listIndex++];
        } ಅನ್ಯಥಾ {
            limit=0x110000;
        }
        ಯದಿ(start>=0x100) {
            ಭಙ್ಗ;
        }
        ಕುರು {
            latin1Contains[start++]=1;
        } ಯಾವತ್(start<limit && start<0x100);
    } ಯಾವತ್(limit<=0x100);

    // Find the first range overlapping with (or after) 80..FF again,
    // to include them in table7FF as well.
    ಪ್ರತಿ(listIndex=0;;) {
        start=list[listIndex++];
        ಯದಿ(listIndex<listLength) {
            limit=list[listIndex++];
        } ಅನ್ಯಥಾ {
            limit=0x110000;
        }
        ಯದಿ(limit>0x80) {
            ಯದಿ(start<0x80) {
                start=0x80;
            }
            ಭಙ್ಗ;
        }
    }

    // Set table7FF[].
    ಯಾವತ್(start<0x800) {
        set32x64Bits(table7FF, start, limit<=0x800 ? limit : 0x800);
        ಯದಿ(limit>0x800) {
            start=0x800;
            ಭಙ್ಗ;
        }

        start=list[listIndex++];
        ಯದಿ(listIndex<listLength) {
            limit=list[listIndex++];
        } ಅನ್ಯಥಾ {
            limit=0x110000;
        }
    }

    // Set bmpBlockBits[].
    int32_t minStart=0x800;
    ಯಾವತ್(start<0x10000) {
        ಯದಿ(limit>0x10000) {
            limit=0x10000;
        }

        ಯದಿ(start<minStart) {
            start=minStart;
        }
        ಯದಿ(start<limit) {  // Else: Another range entirely in a known mixed-value block.
            ಯದಿ(start&0x3f) {
                // Mixed-value block of 64 code points.
                start>>=6;
                bmpBlockBits[start&0x3f]|=0x10001<<(start>>6);
                start=(start+1)<<6;  // Round up to the next block boundary.
                minStart=start;      // Ignore further ranges in this block.
            }
            ಯದಿ(start<limit) {
                ಯದಿ(start<(limit&~0x3f)) {
                    // Multiple all-ones blocks of 64 code points each.
                    set32x64Bits(bmpBlockBits, start>>6, limit>>6);
                }

                ಯದಿ(limit&0x3f) {
                    // Mixed-value block of 64 code points.
                    limit>>=6;
                    bmpBlockBits[limit&0x3f]|=0x10001<<(limit>>6);
                    limit=(limit+1)<<6;  // Round up to the next block boundary.
                    minStart=limit;      // Ignore further ranges in this block.
                }
            }
        }

        ಯದಿ(limit==0x10000) {
            ಭಙ್ಗ;
        }

        start=list[listIndex++];
        ಯದಿ(listIndex<listLength) {
            limit=list[listIndex++];
        } ಅನ್ಯಥಾ {
            limit=0x110000;
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
    uint32_t bits, mask;
    int32_t i;

    ಯದಿ(containsFFFD) {
        bits=3;                 // Lead bytes 0xC0 and 0xC1.
        ಪ್ರತಿ(i=0; i<64; ++i) {
            table7FF[i]|=bits;
        }

        bits=1;                 // Lead byte 0xE0.
        ಪ್ರತಿ(i=0; i<32; ++i) {   // First half of 4k block.
            bmpBlockBits[i]|=bits;
        }

        mask= static_cast<uint32_t>(~(0x10001<<0xd));   // Lead byte 0xED.
        bits=1<<0xd;
        ಪ್ರತಿ(i=32; i<64; ++i) {  // Second half of 4k block.
            bmpBlockBits[i]=(bmpBlockBits[i]&mask)|bits;
        }
    } ಅನ್ಯಥಾ {
        mask= static_cast<uint32_t>(~(0x10001<<0xd));   // Lead byte 0xED.
        ಪ್ರತಿ(i=32; i<64; ++i) {  // Second half of 4k block.
            bmpBlockBits[i]&=mask;
        }
    }
}

int32_t BMPSet::findCodePoint(UChar32 c, int32_t lo, int32_t hi) ಧ್ರುವ {
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
    ಯದಿ (c < list[lo])
        ಪ್ರತ್ಯಾಗಮ lo;
    // High runner test.  c is often after the last range, so an
    // initial check for this condition pays off.
    ಯದಿ (lo >= hi || c >= list[hi-1])
        ಪ್ರತ್ಯಾಗಮ hi;
    // invariant: c >= list[lo]
    // invariant: c < list[hi]
    ಪ್ರತಿ (;;) {
        int32_t i = (lo + hi) >> 1;
        ಯದಿ (i == lo) {
            ಭಙ್ಗ; // Found!
        } ಅನ್ಯಥಾ ಯದಿ (c < list[i]) {
            hi = i;
        } ಅನ್ಯಥಾ {
            lo = i;
        }
    }
    ಪ್ರತ್ಯಾಗಮ hi;
}

UBool
BMPSet::contains(UChar32 c) ಧ್ರುವ {
    ಯದಿ (static_cast<uint32_t>(c) <= 0xff) {
        ಪ್ರತ್ಯಾಗಮ latin1Contains[c];
    } ಅನ್ಯಥಾ ಯದಿ (static_cast<uint32_t>(c) <= 0x7ff) {
        ಪ್ರತ್ಯಾಗಮ (table7FF[c & 0x3f] & (static_cast<uint32_t>(1) << (c >> 6))) != 0;
    } ಅನ್ಯಥಾ ಯದಿ (static_cast<uint32_t>(c) < 0xd800 || (c >= 0xe000 && c <= 0xffff)) {
        ಪೂರ್ಣಾಙ್ಕ lead=c>>12;
        uint32_t twoBits=(bmpBlockBits[(c>>6)&0x3f]>>lead)&0x10001;
        ಯದಿ(twoBits<=1) {
            // All 64 code points with the same bits 15..6
            // are either in the set or not.
            ಪ್ರತ್ಯಾಗಮ twoBits;
        } ಅನ್ಯಥಾ {
            // Look up the code point in its 4k block of code points.
            ಪ್ರತ್ಯಾಗಮ containsSlow(c, list4kStarts[lead], list4kStarts[lead+1]);
        }
    } ಅನ್ಯಥಾ ಯದಿ (static_cast<uint32_t>(c) <= 0x10ffff) {
        // surrogate or supplementary code point
        ಪ್ರತ್ಯಾಗಮ containsSlow(c, list4kStarts[0xd], list4kStarts[0x11]);
    } ಅನ್ಯಥಾ {
        // Out-of-range code points get false, consistent with long-standing
        // behavior of UnicodeSet::contains(c).
        ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;
    }
}

/*
 * Check for sufficient length for trail unit for each surrogate pair.
 * Handle single surrogates as surrogate code points as usual in ICU.
 */
ಧ್ರುವ char16_t *
BMPSet::span(ಧ್ರುವ char16_t *s, ಧ್ರುವ char16_t *limit, USetSpanCondition spanCondition) ಧ್ರುವ {
    char16_t c, c2;

    ಯದಿ(spanCondition) {
        // span
        ಕುರು {
            c=*s;
            ಯದಿ(c<=0xff) {
                ಯದಿ(!latin1Contains[c]) {
                    ಭಙ್ಗ;
                }
            } ಅನ್ಯಥಾ ಯದಿ(c<=0x7ff) {
                ಯದಿ ((table7FF[c & 0x3f] & (static_cast<uint32_t>(1) << (c >> 6))) == 0) {
                    ಭಙ್ಗ;
                }
            } ಅನ್ಯಥಾ ಯದಿ(c<0xd800 || c>=0xe000) {
                ಪೂರ್ಣಾಙ್ಕ lead=c>>12;
                uint32_t twoBits=(bmpBlockBits[(c>>6)&0x3f]>>lead)&0x10001;
                ಯದಿ(twoBits<=1) {
                    // All 64 code points with the same bits 15..6
                    // are either in the set or not.
                    ಯದಿ(twoBits==0) {
                        ಭಙ್ಗ;
                    }
                } ಅನ್ಯಥಾ {
                    // Look up the code point in its 4k block of code points.
                    ಯದಿ(!containsSlow(c, list4kStarts[lead], list4kStarts[lead+1])) {
                        ಭಙ್ಗ;
                    }
                }
            } ಅನ್ಯಥಾ ಯದಿ(c>=0xdc00 || (s+1)==limit || (c2=s[1])<0xdc00 || c2>=0xe000) {
                // surrogate code point
                ಯದಿ(!containsSlow(c, list4kStarts[0xd], list4kStarts[0xe])) {
                    ಭಙ್ಗ;
                }
            } ಅನ್ಯಥಾ {
                // surrogate pair
                ಯದಿ(!containsSlow(U16_GET_SUPPLEMENTARY(c, c2), list4kStarts[0x10], list4kStarts[0x11])) {
                    ಭಙ್ಗ;
                }
                ++s;
            }
        } ಯಾವತ್(++s<limit);
    } ಅನ್ಯಥಾ {
        // span not
        ಕುರು {
            c=*s;
            ಯದಿ(c<=0xff) {
                ಯದಿ(latin1Contains[c]) {
                    ಭಙ್ಗ;
                }
            } ಅನ್ಯಥಾ ಯದಿ(c<=0x7ff) {
                ಯದಿ ((table7FF[c & 0x3f] & (static_cast<uint32_t>(1) << (c >> 6))) != 0) {
                    ಭಙ್ಗ;
                }
            } ಅನ್ಯಥಾ ಯದಿ(c<0xd800 || c>=0xe000) {
                ಪೂರ್ಣಾಙ್ಕ lead=c>>12;
                uint32_t twoBits=(bmpBlockBits[(c>>6)&0x3f]>>lead)&0x10001;
                ಯದಿ(twoBits<=1) {
                    // All 64 code points with the same bits 15..6
                    // are either in the set or not.
                    ಯದಿ(twoBits!=0) {
                        ಭಙ್ಗ;
                    }
                } ಅನ್ಯಥಾ {
                    // Look up the code point in its 4k block of code points.
                    ಯದಿ(containsSlow(c, list4kStarts[lead], list4kStarts[lead+1])) {
                        ಭಙ್ಗ;
                    }
                }
            } ಅನ್ಯಥಾ ಯದಿ(c>=0xdc00 || (s+1)==limit || (c2=s[1])<0xdc00 || c2>=0xe000) {
                // surrogate code point
                ಯದಿ(containsSlow(c, list4kStarts[0xd], list4kStarts[0xe])) {
                    ಭಙ್ಗ;
                }
            } ಅನ್ಯಥಾ {
                // surrogate pair
                ಯದಿ(containsSlow(U16_GET_SUPPLEMENTARY(c, c2), list4kStarts[0x10], list4kStarts[0x11])) {
                    ಭಙ್ಗ;
                }
                ++s;
            }
        } ಯಾವತ್(++s<limit);
    }
    ಪ್ರತ್ಯಾಗಮ s;
}

/* Symmetrical with span(). */
ಧ್ರುವ char16_t *
BMPSet::spanBack(ಧ್ರುವ char16_t *s, ಧ್ರುವ char16_t *limit, USetSpanCondition spanCondition) ಧ್ರುವ {
    char16_t c, c2;

    ಯದಿ(spanCondition) {
        // span
        ಪ್ರತಿ(;;) {
            c=*(--limit);
            ಯದಿ(c<=0xff) {
                ಯದಿ(!latin1Contains[c]) {
                    ಭಙ್ಗ;
                }
            } ಅನ್ಯಥಾ ಯದಿ(c<=0x7ff) {
                ಯದಿ ((table7FF[c & 0x3f] & (static_cast<uint32_t>(1) << (c >> 6))) == 0) {
                    ಭಙ್ಗ;
                }
            } ಅನ್ಯಥಾ ಯದಿ(c<0xd800 || c>=0xe000) {
                ಪೂರ್ಣಾಙ್ಕ lead=c>>12;
                uint32_t twoBits=(bmpBlockBits[(c>>6)&0x3f]>>lead)&0x10001;
                ಯದಿ(twoBits<=1) {
                    // All 64 code points with the same bits 15..6
                    // are either in the set or not.
                    ಯದಿ(twoBits==0) {
                        ಭಙ್ಗ;
                    }
                } ಅನ್ಯಥಾ {
                    // Look up the code point in its 4k block of code points.
                    ಯದಿ(!containsSlow(c, list4kStarts[lead], list4kStarts[lead+1])) {
                        ಭಙ್ಗ;
                    }
                }
            } ಅನ್ಯಥಾ ಯದಿ(c<0xdc00 || s==limit || (c2=*(limit-1))<0xd800 || c2>=0xdc00) {
                // surrogate code point
                ಯದಿ(!containsSlow(c, list4kStarts[0xd], list4kStarts[0xe])) {
                    ಭಙ್ಗ;
                }
            } ಅನ್ಯಥಾ {
                // surrogate pair
                ಯದಿ(!containsSlow(U16_GET_SUPPLEMENTARY(c2, c), list4kStarts[0x10], list4kStarts[0x11])) {
                    ಭಙ್ಗ;
                }
                --limit;
            }
            ಯದಿ(s==limit) {
                ಪ್ರತ್ಯಾಗಮ s;
            }
        }
    } ಅನ್ಯಥಾ {
        // span not
        ಪ್ರತಿ(;;) {
            c=*(--limit);
            ಯದಿ(c<=0xff) {
                ಯದಿ(latin1Contains[c]) {
                    ಭಙ್ಗ;
                }
            } ಅನ್ಯಥಾ ಯದಿ(c<=0x7ff) {
                ಯದಿ ((table7FF[c & 0x3f] & (static_cast<uint32_t>(1) << (c >> 6))) != 0) {
                    ಭಙ್ಗ;
                }
            } ಅನ್ಯಥಾ ಯದಿ(c<0xd800 || c>=0xe000) {
                ಪೂರ್ಣಾಙ್ಕ lead=c>>12;
                uint32_t twoBits=(bmpBlockBits[(c>>6)&0x3f]>>lead)&0x10001;
                ಯದಿ(twoBits<=1) {
                    // All 64 code points with the same bits 15..6
                    // are either in the set or not.
                    ಯದಿ(twoBits!=0) {
                        ಭಙ್ಗ;
                    }
                } ಅನ್ಯಥಾ {
                    // Look up the code point in its 4k block of code points.
                    ಯದಿ(containsSlow(c, list4kStarts[lead], list4kStarts[lead+1])) {
                        ಭಙ್ಗ;
                    }
                }
            } ಅನ್ಯಥಾ ಯದಿ(c<0xdc00 || s==limit || (c2=*(limit-1))<0xd800 || c2>=0xdc00) {
                // surrogate code point
                ಯದಿ(containsSlow(c, list4kStarts[0xd], list4kStarts[0xe])) {
                    ಭಙ್ಗ;
                }
            } ಅನ್ಯಥಾ {
                // surrogate pair
                ಯದಿ(containsSlow(U16_GET_SUPPLEMENTARY(c2, c), list4kStarts[0x10], list4kStarts[0x11])) {
                    ಭಙ್ಗ;
                }
                --limit;
            }
            ಯದಿ(s==limit) {
                ಪ್ರತ್ಯಾಗಮ s;
            }
        }
    }
    ಪ್ರತ್ಯಾಗಮ limit+1;
}

/*
 * Precheck for sufficient trail bytes at end of string only once per span.
 * Check validity.
 */
ಧ್ರುವ uint8_t *
BMPSet::spanUTF8(ಧ್ರುವ uint8_t *s, int32_t length, USetSpanCondition spanCondition) ಧ್ರುವ {
    ಧ್ರುವ uint8_t *limit=s+length;
    uint8_t b=*s;
    ಯದಿ(U8_IS_SINGLE(b)) {
        // Initial all-ASCII span.
        ಯದಿ(spanCondition) {
            ಕುರು {
                ಯದಿ(!latin1Contains[b] || ++s==limit) {
                    ಪ್ರತ್ಯಾಗಮ s;
                }
                b=*s;
            } ಯಾವತ್(U8_IS_SINGLE(b));
        } ಅನ್ಯಥಾ {
            ಕುರು {
                ಯದಿ(latin1Contains[b] || ++s==limit) {
                    ಪ್ರತ್ಯಾಗಮ s;
                }
                b=*s;
            } ಯಾವತ್(U8_IS_SINGLE(b));
        }
        length = static_cast<int32_t>(limit - s);
    }

    ಯದಿ(spanCondition!=USET_SPAN_NOT_CONTAINED) {
        spanCondition=USET_SPAN_CONTAINED;  // Pin to 0/1 values.
    }

    ಧ್ರುವ uint8_t *limit0=limit;

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
    b=*(limit-1);
    ಯದಿ (static_cast<int8_t>(b) < 0) {
        // b>=0x80: lead or trail byte
        ಯದಿ(b<0xc0) {
            // single trail byte, check for preceding 3- or 4-byte lead byte
            ಯದಿ(length>=2 && (b=*(limit-2))>=0xe0) {
                limit-=2;
                ಯದಿ(containsFFFD!=spanCondition) {
                    limit0=limit;
                }
            } ಅನ್ಯಥಾ ಯದಿ(b<0xc0 && b>=0x80 && length>=3 && (b=*(limit-3))>=0xf0) {
                // 4-byte lead byte with only two trail bytes
                limit-=3;
                ಯದಿ(containsFFFD!=spanCondition) {
                    limit0=limit;
                }
            }
        } ಅನ್ಯಥಾ {
            // lead byte with no trail bytes
            --limit;
            ಯದಿ(containsFFFD!=spanCondition) {
                limit0=limit;
            }
        }
    }

    uint8_t t1, t2, t3;

    ಯಾವತ್(s<limit) {
        b=*s;
        ಯದಿ(U8_IS_SINGLE(b)) {
            // ASCII
            ಯದಿ(spanCondition) {
                ಕುರು {
                    ಯದಿ(!latin1Contains[b]) {
                        ಪ್ರತ್ಯಾಗಮ s;
                    } ಅನ್ಯಥಾ ಯದಿ(++s==limit) {
                        ಪ್ರತ್ಯಾಗಮ limit0;
                    }
                    b=*s;
                } ಯಾವತ್(U8_IS_SINGLE(b));
            } ಅನ್ಯಥಾ {
                ಕುರು {
                    ಯದಿ(latin1Contains[b]) {
                        ಪ್ರತ್ಯಾಗಮ s;
                    } ಅನ್ಯಥಾ ಯದಿ(++s==limit) {
                        ಪ್ರತ್ಯಾಗಮ limit0;
                    }
                    b=*s;
                } ಯಾವತ್(U8_IS_SINGLE(b));
            }
        }
        ++s;  // Advance past the lead byte.
        ಯದಿ(b>=0xe0) {
            ಯದಿ(b<0xf0) {
                ಯದಿ( /* handle U+0000..U+FFFF inline */
                    (t1 = static_cast<uint8_t>(s[0] - 0x80)) <= 0x3f &&
                    (t2 = static_cast<uint8_t>(s[1] - 0x80)) <= 0x3f
                ) {
                    b&=0xf;
                    uint32_t twoBits=(bmpBlockBits[t1]>>b)&0x10001;
                    ಯದಿ(twoBits<=1) {
                        // All 64 code points with this lead byte and middle trail byte
                        // are either in the set or not.
                        ಯದಿ (twoBits != static_cast<uint32_t>(spanCondition)) {
                            ಪ್ರತ್ಯಾಗಮ s-1;
                        }
                    } ಅನ್ಯಥಾ {
                        // Look up the code point in its 4k block of code points.
                        UChar32 c=(b<<12)|(t1<<6)|t2;
                        ಯದಿ(containsSlow(c, list4kStarts[b], list4kStarts[b+1]) != spanCondition) {
                            ಪ್ರತ್ಯಾಗಮ s-1;
                        }
                    }
                    s+=2;
                    ಅನುವರ್ತ;
                }
            } ಅನ್ಯಥಾ ಯದಿ( /* handle U+10000..U+10FFFF inline */
                (t1 = static_cast<uint8_t>(s[0] - 0x80)) <= 0x3f &&
                (t2 = static_cast<uint8_t>(s[1] - 0x80)) <= 0x3f &&
                (t3 = static_cast<uint8_t>(s[2] - 0x80)) <= 0x3f
            ) {
                // Give an illegal sequence the same value as the result of contains(FFFD).
                UChar32 c = (static_cast<UChar32>(b - 0xf0) << 18) | (static_cast<UChar32>(t1) << 12) | (t2 << 6) | t3;
                ಯದಿ( (   (0x10000<=c && c<=0x10ffff) ?
                            containsSlow(c, list4kStarts[0x10], list4kStarts[0x11]) :
                            containsFFFD
                    ) != spanCondition
                ) {
                    ಪ್ರತ್ಯಾಗಮ s-1;
                }
                s+=3;
                ಅನುವರ್ತ;
            }
        } ಅನ್ಯಥಾ {
            ಯದಿ( /* handle U+0000..U+07FF inline */
                b>=0xc0 &&
                (t1 = static_cast<uint8_t>(*s - 0x80)) <= 0x3f
            ) {
                ಯದಿ (static_cast<USetSpanCondition>((table7FF[t1] & (static_cast<uint32_t>(1) << (b & 0x1f))) != 0) != spanCondition) {
                    ಪ್ರತ್ಯಾಗಮ s-1;
                }
                ++s;
                ಅನುವರ್ತ;
            }
        }

        // Give an illegal sequence the same value as the result of contains(FFFD).
        // Handle each byte of an illegal sequence separately to simplify the code;
        // no need to optimize error handling.
        ಯದಿ(containsFFFD!=spanCondition) {
            ಪ್ರತ್ಯಾಗಮ s-1;
        }
    }

    ಪ್ರತ್ಯಾಗಮ limit0;
}

/*
 * While going backwards through UTF-8 optimize only for ASCII.
 * Unlike UTF-16, UTF-8 is not forward-backward symmetrical, that is, it is not
 * possible to tell from the last byte in a multi-byte sequence how many
 * preceding bytes there should be. Therefore, going backwards through UTF-8
 * is much harder than going forward.
 */
int32_t
BMPSet::spanBackUTF8(ಧ್ರುವ uint8_t *s, int32_t length, USetSpanCondition spanCondition) ಧ್ರುವ {
    ಯದಿ(spanCondition!=USET_SPAN_NOT_CONTAINED) {
        spanCondition=USET_SPAN_CONTAINED;  // Pin to 0/1 values.
    }

    uint8_t b;

    ಕುರು {
        b=s[--length];
        ಯದಿ(U8_IS_SINGLE(b)) {
            // ASCII sub-span
            ಯದಿ(spanCondition) {
                ಕುರು {
                    ಯದಿ(!latin1Contains[b]) {
                        ಪ್ರತ್ಯಾಗಮ length+1;
                    } ಅನ್ಯಥಾ ಯದಿ(length==0) {
                        ಪ್ರತ್ಯಾಗಮ 0;
                    }
                    b=s[--length];
                } ಯಾವತ್(U8_IS_SINGLE(b));
            } ಅನ್ಯಥಾ {
                ಕುರು {
                    ಯದಿ(latin1Contains[b]) {
                        ಪ್ರತ್ಯಾಗಮ length+1;
                    } ಅನ್ಯಥಾ ಯದಿ(length==0) {
                        ಪ್ರತ್ಯಾಗಮ 0;
                    }
                    b=s[--length];
                } ಯಾವತ್(U8_IS_SINGLE(b));
            }
        }

        int32_t prev=length;
        UChar32 c;
        // trail byte: collect a multi-byte character
        // (or  lead byte in last-trail position)
        c=utf8_prevCharSafeBody(s, 0, &length, b, -3);
        // c is a valid code point, not ASCII, not a surrogate
        ಯದಿ(c<=0x7ff) {
            ಯದಿ (static_cast<USetSpanCondition>((table7FF[c & 0x3f] & (static_cast<uint32_t>(1) << (c >> 6))) != 0) != spanCondition) {
                ಪ್ರತ್ಯಾಗಮ prev+1;
            }
        } ಅನ್ಯಥಾ ಯದಿ(c<=0xffff) {
            ಪೂರ್ಣಾಙ್ಕ lead=c>>12;
            uint32_t twoBits=(bmpBlockBits[(c>>6)&0x3f]>>lead)&0x10001;
            ಯದಿ(twoBits<=1) {
                // All 64 code points with the same bits 15..6
                // are either in the set or not.
                ಯದಿ (twoBits != static_cast<uint32_t>(spanCondition)) {
                    ಪ್ರತ್ಯಾಗಮ prev+1;
                }
            } ಅನ್ಯಥಾ {
                // Look up the code point in its 4k block of code points.
                ಯದಿ(containsSlow(c, list4kStarts[lead], list4kStarts[lead+1]) != spanCondition) {
                    ಪ್ರತ್ಯಾಗಮ prev+1;
                }
            }
        } ಅನ್ಯಥಾ {
            ಯದಿ(containsSlow(c, list4kStarts[0x10], list4kStarts[0x11]) != spanCondition) {
                ಪ್ರತ್ಯಾಗಮ prev+1;
            }
        }
    } ಯಾವತ್(length>0);
    ಪ್ರತ್ಯಾಗಮ 0;
}

U_NAMESPACE_END
