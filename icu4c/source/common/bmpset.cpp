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

#ଅନ୍ତର୍ଭାଵ "unicode/utypes.h"
#ଅନ୍ତର୍ଭାଵ "unicode/uniset.h"
#ଅନ୍ତର୍ଭାଵ "unicode/utf8.h"
#ଅନ୍ତର୍ଭାଵ "unicode/utf16.h"
#ଅନ୍ତର୍ଭାଵ "cmemory.h"
#ଅନ୍ତର୍ଭାଵ "bmpset.h"
#ଅନ୍ତର୍ଭାଵ "uassert.h"

U_NAMESPACE_BEGIN

BMPSet::BMPSet(ଅଚଳ int32_t *parentList, int32_t parentListLength) :
        list(parentList), listLength(parentListLength) {
    uprv_memset(latin1Contains, 0, ଆକାର(latin1Contains));
    uprv_memset(table7FF, 0, ଆକାର(table7FF));
    uprv_memset(bmpBlockBits, 0, ଆକାର(bmpBlockBits));

    /*
     * Set the list indexes for binary searches for
     * U+0800, U+1000, U+2000, .., U+F000, U+10000.
     * U+0800 is the first 3-byte-UTF-8 code point. Lower code points are
     * looked up in the bit tables.
     * The last pair of indexes is for finding supplementary code points.
     */
    list4kStarts[0]=findCodePoint(0x800, 0, listLength-1);
    int32_t i;
    ପାଇଁ(i=1; i<=0x10; ++i) {
        list4kStarts[i]=findCodePoint(i<<12, list4kStarts[i-1], listLength-1);
    }
    list4kStarts[0x11]=listLength-1;
    containsFFFD=containsSlow(0xfffd, list4kStarts[0xf], list4kStarts[0x10]);

    initBits();
    overrideIllegal();
}

BMPSet::BMPSet(ଅଚଳ BMPSet &otherBMPSet, ଅଚଳ int32_t *newParentList, int32_t newParentListLength) :
        containsFFFD(otherBMPSet.containsFFFD),
        list(newParentList), listLength(newParentListLength) {
    uprv_memcpy(latin1Contains, otherBMPSet.latin1Contains, ଆକାର(latin1Contains));
    uprv_memcpy(table7FF, otherBMPSet.table7FF, ଆକାର(table7FF));
    uprv_memcpy(bmpBlockBits, otherBMPSet.bmpBlockBits, ଆକାର(bmpBlockBits));
    uprv_memcpy(list4kStarts, otherBMPSet.list4kStarts, ଆକାର(list4kStarts));
}

BMPSet::~BMPSet() {
}

/*
 * Set bits in a bit rectangle in "vertical" bit organization.
 * start<limit<=0x800
 */
ସ୍ଥିର ଶୂନ୍ୟ set32x64Bits(uint32_t table[64], int32_t start, int32_t limit) {
    U_ASSERT(start<limit);
    U_ASSERT(limit<=0x800);

    int32_t lead=start>>6;  // Named for UTF-8 2-byte lead byte with upper 5 bits.
    int32_t trail=start&0x3f;  // Named for UTF-8 2-byte trail byte with lower 6 bits.

    // Set one bit indicating an all-one block.
    uint32_t bits = static_cast<uint32_t>(1) << lead;
    ଯଦି((start+1)==limit) {  // Single-character shortcut.
        table[trail]|=bits;
        ଫେରନ୍ତୁ;
    }

    int32_t limitLead=limit>>6;
    int32_t limitTrail=limit&0x3f;

    ଯଦି(lead==limitLead) {
        // Partial vertical bit column.
        ଯେତେବେଳେ(trail<limitTrail) {
            table[trail++]|=bits;
        }
    } ଅନ୍ୟଥା {
        // Partial vertical bit column,
        // followed by a bit rectangle,
        // followed by another partial vertical bit column.
        ଯଦି(trail>0) {
            କର {
                table[trail++]|=bits;
            } ଯେତେବେଳେ(trail<64);
            ++lead;
        }
        ଯଦି(lead<limitLead) {
            bits = ~((static_cast<unsigned>(1) << lead) - 1);
            ଯଦି(limitLead<0x20) {
                bits &= (static_cast<unsigned>(1) << limitLead) - 1;
            }
            ପାଇଁ(trail=0; trail<64; ++trail) {
                table[trail]|=bits;
            }
        }
        // limit<=0x800. If limit==0x800 then limitLead=32 and limitTrail=0.
        // In that case, bits=1<<limitLead is undefined but the bits value
        // is not used because trail<limitTrail is already false.
        bits = static_cast<uint32_t>(1) << ((limitLead == 0x20) ? (limitLead - 1) : limitLead);
        ପାଇଁ(trail=0; trail<limitTrail; ++trail) {
            table[trail]|=bits;
        }
    }
}

ଶୂନ୍ୟ BMPSet::initBits() {
    UChar32 start, limit;
    int32_t listIndex=0;

    // Set latin1Contains[].
    କର {
        start=list[listIndex++];
        ଯଦି(listIndex<listLength) {
            limit=list[listIndex++];
        } ଅନ୍ୟଥା {
            limit=0x110000;
        }
        ଯଦି(start>=0x100) {
            ଭାଙ୍ଗ;
        }
        କର {
            latin1Contains[start++]=1;
        } ଯେତେବେଳେ(start<limit && start<0x100);
    } ଯେତେବେଳେ(limit<=0x100);

    // Find the first range overlapping with (or after) 80..FF again,
    // to include them in table7FF as well.
    ପାଇଁ(listIndex=0;;) {
        start=list[listIndex++];
        ଯଦି(listIndex<listLength) {
            limit=list[listIndex++];
        } ଅନ୍ୟଥା {
            limit=0x110000;
        }
        ଯଦି(limit>0x80) {
            ଯଦି(start<0x80) {
                start=0x80;
            }
            ଭାଙ୍ଗ;
        }
    }

    // Set table7FF[].
    ଯେତେବେଳେ(start<0x800) {
        set32x64Bits(table7FF, start, limit<=0x800 ? limit : 0x800);
        ଯଦି(limit>0x800) {
            start=0x800;
            ଭାଙ୍ଗ;
        }

        start=list[listIndex++];
        ଯଦି(listIndex<listLength) {
            limit=list[listIndex++];
        } ଅନ୍ୟଥା {
            limit=0x110000;
        }
    }

    // Set bmpBlockBits[].
    int32_t minStart=0x800;
    ଯେତେବେଳେ(start<0x10000) {
        ଯଦି(limit>0x10000) {
            limit=0x10000;
        }

        ଯଦି(start<minStart) {
            start=minStart;
        }
        ଯଦି(start<limit) {  // Else: Another range entirely in a known mixed-value block.
            ଯଦି(start&0x3f) {
                // Mixed-value block of 64 code points.
                start>>=6;
                bmpBlockBits[start&0x3f]|=0x10001<<(start>>6);
                start=(start+1)<<6;  // Round up to the next block boundary.
                minStart=start;      // Ignore further ranges in this block.
            }
            ଯଦି(start<limit) {
                ଯଦି(start<(limit&~0x3f)) {
                    // Multiple all-ones blocks of 64 code points each.
                    set32x64Bits(bmpBlockBits, start>>6, limit>>6);
                }

                ଯଦି(limit&0x3f) {
                    // Mixed-value block of 64 code points.
                    limit>>=6;
                    bmpBlockBits[limit&0x3f]|=0x10001<<(limit>>6);
                    limit=(limit+1)<<6;  // Round up to the next block boundary.
                    minStart=limit;      // Ignore further ranges in this block.
                }
            }
        }

        ଯଦି(limit==0x10000) {
            ଭାଙ୍ଗ;
        }

        start=list[listIndex++];
        ଯଦି(listIndex<listLength) {
            limit=list[listIndex++];
        } ଅନ୍ୟଥା {
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
ଶୂନ୍ୟ BMPSet::overrideIllegal() {
    uint32_t bits, mask;
    int32_t i;

    ଯଦି(containsFFFD) {
        bits=3;                 // Lead bytes 0xC0 and 0xC1.
        ପାଇଁ(i=0; i<64; ++i) {
            table7FF[i]|=bits;
        }

        bits=1;                 // Lead byte 0xE0.
        ପାଇଁ(i=0; i<32; ++i) {   // First half of 4k block.
            bmpBlockBits[i]|=bits;
        }

        mask= static_cast<uint32_t>(~(0x10001<<0xd));   // Lead byte 0xED.
        bits=1<<0xd;
        ପାଇଁ(i=32; i<64; ++i) {  // Second half of 4k block.
            bmpBlockBits[i]=(bmpBlockBits[i]&mask)|bits;
        }
    } ଅନ୍ୟଥା {
        mask= static_cast<uint32_t>(~(0x10001<<0xd));   // Lead byte 0xED.
        ପାଇଁ(i=32; i<64; ++i) {  // Second half of 4k block.
            bmpBlockBits[i]&=mask;
        }
    }
}

int32_t BMPSet::findCodePoint(UChar32 c, int32_t lo, int32_t hi) ଅଚଳ {
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
    ଯଦି (c < list[lo])
        ଫେରନ୍ତୁ lo;
    // High runner test.  c is often after the last range, so an
    // initial check for this condition pays off.
    ଯଦି (lo >= hi || c >= list[hi-1])
        ଫେରନ୍ତୁ hi;
    // invariant: c >= list[lo]
    // invariant: c < list[hi]
    ପାଇଁ (;;) {
        int32_t i = (lo + hi) >> 1;
        ଯଦି (i == lo) {
            ଭାଙ୍ଗ; // Found!
        } ଅନ୍ୟଥା ଯଦି (c < list[i]) {
            hi = i;
        } ଅନ୍ୟଥା {
            lo = i;
        }
    }
    ଫେରନ୍ତୁ hi;
}

UBool
BMPSet::contains(UChar32 c) ଅଚଳ {
    ଯଦି (static_cast<uint32_t>(c) <= 0xff) {
        ଫେରନ୍ତୁ latin1Contains[c];
    } ଅନ୍ୟଥା ଯଦି (static_cast<uint32_t>(c) <= 0x7ff) {
        ଫେରନ୍ତୁ (table7FF[c & 0x3f] & (static_cast<uint32_t>(1) << (c >> 6))) != 0;
    } ଅନ୍ୟଥା ଯଦି (static_cast<uint32_t>(c) < 0xd800 || (c >= 0xe000 && c <= 0xffff)) {
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା lead=c>>12;
        uint32_t twoBits=(bmpBlockBits[(c>>6)&0x3f]>>lead)&0x10001;
        ଯଦି(twoBits<=1) {
            // All 64 code points with the same bits 15..6
            // are either in the set or not.
            ଫେରନ୍ତୁ twoBits;
        } ଅନ୍ୟଥା {
            // Look up the code point in its 4k block of code points.
            ଫେରନ୍ତୁ containsSlow(c, list4kStarts[lead], list4kStarts[lead+1]);
        }
    } ଅନ୍ୟଥା ଯଦି (static_cast<uint32_t>(c) <= 0x10ffff) {
        // surrogate or supplementary code point
        ଫେରନ୍ତୁ containsSlow(c, list4kStarts[0xd], list4kStarts[0x11]);
    } ଅନ୍ୟଥା {
        // Out-of-range code points get false, consistent with long-standing
        // behavior of UnicodeSet::contains(c).
        ଫେରନ୍ତୁ ମିଥ୍ୟା;
    }
}

/*
 * Check for sufficient length for trail unit for each surrogate pair.
 * Handle single surrogates as surrogate code points as usual in ICU.
 */
ଅଚଳ char16_t *
BMPSet::span(ଅଚଳ char16_t *s, ଅଚଳ char16_t *limit, USetSpanCondition spanCondition) ଅଚଳ {
    char16_t c, c2;

    ଯଦି(spanCondition) {
        // span
        କର {
            c=*s;
            ଯଦି(c<=0xff) {
                ଯଦି(!latin1Contains[c]) {
                    ଭାଙ୍ଗ;
                }
            } ଅନ୍ୟଥା ଯଦି(c<=0x7ff) {
                ଯଦି ((table7FF[c & 0x3f] & (static_cast<uint32_t>(1) << (c >> 6))) == 0) {
                    ଭାଙ୍ଗ;
                }
            } ଅନ୍ୟଥା ଯଦି(c<0xd800 || c>=0xe000) {
                ପୂର୍ଣ୍ଣସଂଖ୍ୟା lead=c>>12;
                uint32_t twoBits=(bmpBlockBits[(c>>6)&0x3f]>>lead)&0x10001;
                ଯଦି(twoBits<=1) {
                    // All 64 code points with the same bits 15..6
                    // are either in the set or not.
                    ଯଦି(twoBits==0) {
                        ଭାଙ୍ଗ;
                    }
                } ଅନ୍ୟଥା {
                    // Look up the code point in its 4k block of code points.
                    ଯଦି(!containsSlow(c, list4kStarts[lead], list4kStarts[lead+1])) {
                        ଭାଙ୍ଗ;
                    }
                }
            } ଅନ୍ୟଥା ଯଦି(c>=0xdc00 || (s+1)==limit || (c2=s[1])<0xdc00 || c2>=0xe000) {
                // surrogate code point
                ଯଦି(!containsSlow(c, list4kStarts[0xd], list4kStarts[0xe])) {
                    ଭାଙ୍ଗ;
                }
            } ଅନ୍ୟଥା {
                // surrogate pair
                ଯଦି(!containsSlow(U16_GET_SUPPLEMENTARY(c, c2), list4kStarts[0x10], list4kStarts[0x11])) {
                    ଭାଙ୍ଗ;
                }
                ++s;
            }
        } ଯେତେବେଳେ(++s<limit);
    } ଅନ୍ୟଥା {
        // span not
        କର {
            c=*s;
            ଯଦି(c<=0xff) {
                ଯଦି(latin1Contains[c]) {
                    ଭାଙ୍ଗ;
                }
            } ଅନ୍ୟଥା ଯଦି(c<=0x7ff) {
                ଯଦି ((table7FF[c & 0x3f] & (static_cast<uint32_t>(1) << (c >> 6))) != 0) {
                    ଭାଙ୍ଗ;
                }
            } ଅନ୍ୟଥା ଯଦି(c<0xd800 || c>=0xe000) {
                ପୂର୍ଣ୍ଣସଂଖ୍ୟା lead=c>>12;
                uint32_t twoBits=(bmpBlockBits[(c>>6)&0x3f]>>lead)&0x10001;
                ଯଦି(twoBits<=1) {
                    // All 64 code points with the same bits 15..6
                    // are either in the set or not.
                    ଯଦି(twoBits!=0) {
                        ଭାଙ୍ଗ;
                    }
                } ଅନ୍ୟଥା {
                    // Look up the code point in its 4k block of code points.
                    ଯଦି(containsSlow(c, list4kStarts[lead], list4kStarts[lead+1])) {
                        ଭାଙ୍ଗ;
                    }
                }
            } ଅନ୍ୟଥା ଯଦି(c>=0xdc00 || (s+1)==limit || (c2=s[1])<0xdc00 || c2>=0xe000) {
                // surrogate code point
                ଯଦି(containsSlow(c, list4kStarts[0xd], list4kStarts[0xe])) {
                    ଭାଙ୍ଗ;
                }
            } ଅନ୍ୟଥା {
                // surrogate pair
                ଯଦି(containsSlow(U16_GET_SUPPLEMENTARY(c, c2), list4kStarts[0x10], list4kStarts[0x11])) {
                    ଭାଙ୍ଗ;
                }
                ++s;
            }
        } ଯେତେବେଳେ(++s<limit);
    }
    ଫେରନ୍ତୁ s;
}

/* Symmetrical with span(). */
ଅଚଳ char16_t *
BMPSet::spanBack(ଅଚଳ char16_t *s, ଅଚଳ char16_t *limit, USetSpanCondition spanCondition) ଅଚଳ {
    char16_t c, c2;

    ଯଦି(spanCondition) {
        // span
        ପାଇଁ(;;) {
            c=*(--limit);
            ଯଦି(c<=0xff) {
                ଯଦି(!latin1Contains[c]) {
                    ଭାଙ୍ଗ;
                }
            } ଅନ୍ୟଥା ଯଦି(c<=0x7ff) {
                ଯଦି ((table7FF[c & 0x3f] & (static_cast<uint32_t>(1) << (c >> 6))) == 0) {
                    ଭାଙ୍ଗ;
                }
            } ଅନ୍ୟଥା ଯଦି(c<0xd800 || c>=0xe000) {
                ପୂର୍ଣ୍ଣସଂଖ୍ୟା lead=c>>12;
                uint32_t twoBits=(bmpBlockBits[(c>>6)&0x3f]>>lead)&0x10001;
                ଯଦି(twoBits<=1) {
                    // All 64 code points with the same bits 15..6
                    // are either in the set or not.
                    ଯଦି(twoBits==0) {
                        ଭାଙ୍ଗ;
                    }
                } ଅନ୍ୟଥା {
                    // Look up the code point in its 4k block of code points.
                    ଯଦି(!containsSlow(c, list4kStarts[lead], list4kStarts[lead+1])) {
                        ଭାଙ୍ଗ;
                    }
                }
            } ଅନ୍ୟଥା ଯଦି(c<0xdc00 || s==limit || (c2=*(limit-1))<0xd800 || c2>=0xdc00) {
                // surrogate code point
                ଯଦି(!containsSlow(c, list4kStarts[0xd], list4kStarts[0xe])) {
                    ଭାଙ୍ଗ;
                }
            } ଅନ୍ୟଥା {
                // surrogate pair
                ଯଦି(!containsSlow(U16_GET_SUPPLEMENTARY(c2, c), list4kStarts[0x10], list4kStarts[0x11])) {
                    ଭାଙ୍ଗ;
                }
                --limit;
            }
            ଯଦି(s==limit) {
                ଫେରନ୍ତୁ s;
            }
        }
    } ଅନ୍ୟଥା {
        // span not
        ପାଇଁ(;;) {
            c=*(--limit);
            ଯଦି(c<=0xff) {
                ଯଦି(latin1Contains[c]) {
                    ଭାଙ୍ଗ;
                }
            } ଅନ୍ୟଥା ଯଦି(c<=0x7ff) {
                ଯଦି ((table7FF[c & 0x3f] & (static_cast<uint32_t>(1) << (c >> 6))) != 0) {
                    ଭାଙ୍ଗ;
                }
            } ଅନ୍ୟଥା ଯଦି(c<0xd800 || c>=0xe000) {
                ପୂର୍ଣ୍ଣସଂଖ୍ୟା lead=c>>12;
                uint32_t twoBits=(bmpBlockBits[(c>>6)&0x3f]>>lead)&0x10001;
                ଯଦି(twoBits<=1) {
                    // All 64 code points with the same bits 15..6
                    // are either in the set or not.
                    ଯଦି(twoBits!=0) {
                        ଭାଙ୍ଗ;
                    }
                } ଅନ୍ୟଥା {
                    // Look up the code point in its 4k block of code points.
                    ଯଦି(containsSlow(c, list4kStarts[lead], list4kStarts[lead+1])) {
                        ଭାଙ୍ଗ;
                    }
                }
            } ଅନ୍ୟଥା ଯଦି(c<0xdc00 || s==limit || (c2=*(limit-1))<0xd800 || c2>=0xdc00) {
                // surrogate code point
                ଯଦି(containsSlow(c, list4kStarts[0xd], list4kStarts[0xe])) {
                    ଭାଙ୍ଗ;
                }
            } ଅନ୍ୟଥା {
                // surrogate pair
                ଯଦି(containsSlow(U16_GET_SUPPLEMENTARY(c2, c), list4kStarts[0x10], list4kStarts[0x11])) {
                    ଭାଙ୍ଗ;
                }
                --limit;
            }
            ଯଦି(s==limit) {
                ଫେରନ୍ତୁ s;
            }
        }
    }
    ଫେରନ୍ତୁ limit+1;
}

/*
 * Precheck for sufficient trail bytes at end of string only once per span.
 * Check validity.
 */
ଅଚଳ uint8_t *
BMPSet::spanUTF8(ଅଚଳ uint8_t *s, int32_t length, USetSpanCondition spanCondition) ଅଚଳ {
    ଅଚଳ uint8_t *limit=s+length;
    uint8_t b=*s;
    ଯଦି(U8_IS_SINGLE(b)) {
        // Initial all-ASCII span.
        ଯଦି(spanCondition) {
            କର {
                ଯଦି(!latin1Contains[b] || ++s==limit) {
                    ଫେରନ୍ତୁ s;
                }
                b=*s;
            } ଯେତେବେଳେ(U8_IS_SINGLE(b));
        } ଅନ୍ୟଥା {
            କର {
                ଯଦି(latin1Contains[b] || ++s==limit) {
                    ଫେରନ୍ତୁ s;
                }
                b=*s;
            } ଯେତେବେଳେ(U8_IS_SINGLE(b));
        }
        length = static_cast<int32_t>(limit - s);
    }

    ଯଦି(spanCondition!=USET_SPAN_NOT_CONTAINED) {
        spanCondition=USET_SPAN_CONTAINED;  // Pin to 0/1 values.
    }

    ଅଚଳ uint8_t *limit0=limit;

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
    ଯଦି (static_cast<int8_t>(b) < 0) {
        // b>=0x80: lead or trail byte
        ଯଦି(b<0xc0) {
            // single trail byte, check for preceding 3- or 4-byte lead byte
            ଯଦି(length>=2 && (b=*(limit-2))>=0xe0) {
                limit-=2;
                ଯଦି(containsFFFD!=spanCondition) {
                    limit0=limit;
                }
            } ଅନ୍ୟଥା ଯଦି(b<0xc0 && b>=0x80 && length>=3 && (b=*(limit-3))>=0xf0) {
                // 4-byte lead byte with only two trail bytes
                limit-=3;
                ଯଦି(containsFFFD!=spanCondition) {
                    limit0=limit;
                }
            }
        } ଅନ୍ୟଥା {
            // lead byte with no trail bytes
            --limit;
            ଯଦି(containsFFFD!=spanCondition) {
                limit0=limit;
            }
        }
    }

    uint8_t t1, t2, t3;

    ଯେତେବେଳେ(s<limit) {
        b=*s;
        ଯଦି(U8_IS_SINGLE(b)) {
            // ASCII
            ଯଦି(spanCondition) {
                କର {
                    ଯଦି(!latin1Contains[b]) {
                        ଫେରନ୍ତୁ s;
                    } ଅନ୍ୟଥା ଯଦି(++s==limit) {
                        ଫେରନ୍ତୁ limit0;
                    }
                    b=*s;
                } ଯେତେବେଳେ(U8_IS_SINGLE(b));
            } ଅନ୍ୟଥା {
                କର {
                    ଯଦି(latin1Contains[b]) {
                        ଫେରନ୍ତୁ s;
                    } ଅନ୍ୟଥା ଯଦି(++s==limit) {
                        ଫେରନ୍ତୁ limit0;
                    }
                    b=*s;
                } ଯେତେବେଳେ(U8_IS_SINGLE(b));
            }
        }
        ++s;  // Advance past the lead byte.
        ଯଦି(b>=0xe0) {
            ଯଦି(b<0xf0) {
                ଯଦି( /* handle U+0000..U+FFFF inline */
                    (t1 = static_cast<uint8_t>(s[0] - 0x80)) <= 0x3f &&
                    (t2 = static_cast<uint8_t>(s[1] - 0x80)) <= 0x3f
                ) {
                    b&=0xf;
                    uint32_t twoBits=(bmpBlockBits[t1]>>b)&0x10001;
                    ଯଦି(twoBits<=1) {
                        // All 64 code points with this lead byte and middle trail byte
                        // are either in the set or not.
                        ଯଦି (twoBits != static_cast<uint32_t>(spanCondition)) {
                            ଫେରନ୍ତୁ s-1;
                        }
                    } ଅନ୍ୟଥା {
                        // Look up the code point in its 4k block of code points.
                        UChar32 c=(b<<12)|(t1<<6)|t2;
                        ଯଦି(containsSlow(c, list4kStarts[b], list4kStarts[b+1]) != spanCondition) {
                            ଫେରନ୍ତୁ s-1;
                        }
                    }
                    s+=2;
                    ଜାରି;
                }
            } ଅନ୍ୟଥା ଯଦି( /* handle U+10000..U+10FFFF inline */
                (t1 = static_cast<uint8_t>(s[0] - 0x80)) <= 0x3f &&
                (t2 = static_cast<uint8_t>(s[1] - 0x80)) <= 0x3f &&
                (t3 = static_cast<uint8_t>(s[2] - 0x80)) <= 0x3f
            ) {
                // Give an illegal sequence the same value as the result of contains(FFFD).
                UChar32 c = (static_cast<UChar32>(b - 0xf0) << 18) | (static_cast<UChar32>(t1) << 12) | (t2 << 6) | t3;
                ଯଦି( (   (0x10000<=c && c<=0x10ffff) ?
                            containsSlow(c, list4kStarts[0x10], list4kStarts[0x11]) :
                            containsFFFD
                    ) != spanCondition
                ) {
                    ଫେରନ୍ତୁ s-1;
                }
                s+=3;
                ଜାରି;
            }
        } ଅନ୍ୟଥା {
            ଯଦି( /* handle U+0000..U+07FF inline */
                b>=0xc0 &&
                (t1 = static_cast<uint8_t>(*s - 0x80)) <= 0x3f
            ) {
                ଯଦି (static_cast<USetSpanCondition>((table7FF[t1] & (static_cast<uint32_t>(1) << (b & 0x1f))) != 0) != spanCondition) {
                    ଫେରନ୍ତୁ s-1;
                }
                ++s;
                ଜାରି;
            }
        }

        // Give an illegal sequence the same value as the result of contains(FFFD).
        // Handle each byte of an illegal sequence separately to simplify the code;
        // no need to optimize error handling.
        ଯଦି(containsFFFD!=spanCondition) {
            ଫେରନ୍ତୁ s-1;
        }
    }

    ଫେରନ୍ତୁ limit0;
}

/*
 * While going backwards through UTF-8 optimize only for ASCII.
 * Unlike UTF-16, UTF-8 is not forward-backward symmetrical, that is, it is not
 * possible to tell from the last byte in a multi-byte sequence how many
 * preceding bytes there should be. Therefore, going backwards through UTF-8
 * is much harder than going forward.
 */
int32_t
BMPSet::spanBackUTF8(ଅଚଳ uint8_t *s, int32_t length, USetSpanCondition spanCondition) ଅଚଳ {
    ଯଦି(spanCondition!=USET_SPAN_NOT_CONTAINED) {
        spanCondition=USET_SPAN_CONTAINED;  // Pin to 0/1 values.
    }

    uint8_t b;

    କର {
        b=s[--length];
        ଯଦି(U8_IS_SINGLE(b)) {
            // ASCII sub-span
            ଯଦି(spanCondition) {
                କର {
                    ଯଦି(!latin1Contains[b]) {
                        ଫେରନ୍ତୁ length+1;
                    } ଅନ୍ୟଥା ଯଦି(length==0) {
                        ଫେରନ୍ତୁ 0;
                    }
                    b=s[--length];
                } ଯେତେବେଳେ(U8_IS_SINGLE(b));
            } ଅନ୍ୟଥା {
                କର {
                    ଯଦି(latin1Contains[b]) {
                        ଫେରନ୍ତୁ length+1;
                    } ଅନ୍ୟଥା ଯଦି(length==0) {
                        ଫେରନ୍ତୁ 0;
                    }
                    b=s[--length];
                } ଯେତେବେଳେ(U8_IS_SINGLE(b));
            }
        }

        int32_t prev=length;
        UChar32 c;
        // trail byte: collect a multi-byte character
        // (or  lead byte in last-trail position)
        c=utf8_prevCharSafeBody(s, 0, &length, b, -3);
        // c is a valid code point, not ASCII, not a surrogate
        ଯଦି(c<=0x7ff) {
            ଯଦି (static_cast<USetSpanCondition>((table7FF[c & 0x3f] & (static_cast<uint32_t>(1) << (c >> 6))) != 0) != spanCondition) {
                ଫେରନ୍ତୁ prev+1;
            }
        } ଅନ୍ୟଥା ଯଦି(c<=0xffff) {
            ପୂର୍ଣ୍ଣସଂଖ୍ୟା lead=c>>12;
            uint32_t twoBits=(bmpBlockBits[(c>>6)&0x3f]>>lead)&0x10001;
            ଯଦି(twoBits<=1) {
                // All 64 code points with the same bits 15..6
                // are either in the set or not.
                ଯଦି (twoBits != static_cast<uint32_t>(spanCondition)) {
                    ଫେରନ୍ତୁ prev+1;
                }
            } ଅନ୍ୟଥା {
                // Look up the code point in its 4k block of code points.
                ଯଦି(containsSlow(c, list4kStarts[lead], list4kStarts[lead+1]) != spanCondition) {
                    ଫେରନ୍ତୁ prev+1;
                }
            }
        } ଅନ୍ୟଥା {
            ଯଦି(containsSlow(c, list4kStarts[0x10], list4kStarts[0x11]) != spanCondition) {
                ଫେରନ୍ତୁ prev+1;
            }
        }
    } ଯେତେବେଳେ(length>0);
    ଫେରନ୍ତୁ 0;
}

U_NAMESPACE_END
