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

#include "unicode/utypes.h"
#include "unicode/uniset.h"
#include "unicode/utf8.h"
#include "unicode/utf16.h"
#include "cmemory.h"
#include "bmpset.h"
#include "uassert.h"

U_NAMESPACE_BEGIN

BMPSet::BMPSet(const int32_t *parentList, int32_t parentListLength) :
        list(parentList), listLength(parentListLength) {
    uprv_memset(latin1Contains, 0, sizeof(latin1Contains));
    uprv_memset(table7FF, 0, sizeof(table7FF));
    uprv_memset(bmpBlockBits, 0, sizeof(bmpBlockBits));

    /*
     * Set the list indexes for binary searches for
     * U+0800, U+1000, U+2000, .., U+F000, U+10000.
     * U+0800 is the first 3-byte-UTF-8 code point. Lower code points are
     * looked up in the bit tables.
     * The last pair of indexes is for finding supplementary code points.
     */
    list4kStarts[0]=findCodePoint(0x800, 0, listLength-1);
    int32_t i;
    ために(i=1; i<=0x10; ++i) {
        list4kStarts[i]=findCodePoint(i<<12, list4kStarts[i-1], listLength-1);
    }
    list4kStarts[0x11]=listLength-1;
    containsFFFD=containsSlow(0xfffd, list4kStarts[0xf], list4kStarts[0x10]);

    initBits();
    overrideIllegal();
}

BMPSet::BMPSet(const BMPSet &otherBMPSet, const int32_t *newParentList, int32_t newParentListLength) :
        containsFFFD(otherBMPSet.containsFFFD),
        list(newParentList), listLength(newParentListLength) {
    uprv_memcpy(latin1Contains, otherBMPSet.latin1Contains, sizeof(latin1Contains));
    uprv_memcpy(table7FF, otherBMPSet.table7FF, sizeof(table7FF));
    uprv_memcpy(bmpBlockBits, otherBMPSet.bmpBlockBits, sizeof(bmpBlockBits));
    uprv_memcpy(list4kStarts, otherBMPSet.list4kStarts, sizeof(list4kStarts));
}

BMPSet::~BMPSet() {
}

/*
 * Set bits in a bit rectangle in "vertical" bit organization.
 * start<limit<=0x800
 */
static void set32x64Bits(uint32_t table[64], int32_t start, int32_t limit) {
    U_ASSERT(start<limit);
    U_ASSERT(limit<=0x800);

    int32_t lead=start>>6;  // Named for UTF-8 2-byte lead byte with upper 5 bits.
    int32_t trail=start&0x3f;  // Named for UTF-8 2-byte trail byte with lower 6 bits.

    // Set one bit indicating an all-one block.
    uint32_t bits = static_cast<uint32_t>(1) << lead;
    もし((start+1)==limit) {  // Single-character shortcut.
        table[trail]|=bits;
        戻る;
    }

    int32_t limitLead=limit>>6;
    int32_t limitTrail=limit&0x3f;

    もし(lead==limitLead) {
        // Partial vertical bit column.
        ながら(trail<limitTrail) {
            table[trail++]|=bits;
        }
    } 否则 {
        // Partial vertical bit column,
        // followed by a bit rectangle,
        // followed by another partial vertical bit column.
        もし(trail>0) {
            do {
                table[trail++]|=bits;
            } ながら(trail<64);
            ++lead;
        }
        もし(lead<limitLead) {
            bits = ~((static_cast<unsigned>(1) << lead) - 1);
            もし(limitLead<0x20) {
                bits &= (static_cast<unsigned>(1) << limitLead) - 1;
            }
            ために(trail=0; trail<64; ++trail) {
                table[trail]|=bits;
            }
        }
        // limit<=0x800. If limit==0x800 then limitLead=32 and limitTrail=0.
        // In that case, bits=1<<limitLead is undefined but the bits value
        // is not used because trail<limitTrail is already false.
        bits = static_cast<uint32_t>(1) << ((limitLead == 0x20) ? (limitLead - 1) : limitLead);
        ために(trail=0; trail<limitTrail; ++trail) {
            table[trail]|=bits;
        }
    }
}

void BMPSet::initBits() {
    UChar32 start, limit;
    int32_t listIndex=0;

    // Set latin1Contains[].
    do {
        start=list[listIndex++];
        もし(listIndex<listLength) {
            limit=list[listIndex++];
        } 否则 {
            limit=0x110000;
        }
        もし(start>=0x100) {
            break;
        }
        do {
            latin1Contains[start++]=1;
        } ながら(start<limit && start<0x100);
    } ながら(limit<=0x100);

    // Find the first range overlapping with (or after) 80..FF again,
    // to include them in table7FF as well.
    ために(listIndex=0;;) {
        start=list[listIndex++];
        もし(listIndex<listLength) {
            limit=list[listIndex++];
        } 否则 {
            limit=0x110000;
        }
        もし(limit>0x80) {
            もし(start<0x80) {
                start=0x80;
            }
            break;
        }
    }

    // Set table7FF[].
    ながら(start<0x800) {
        set32x64Bits(table7FF, start, limit<=0x800 ? limit : 0x800);
        もし(limit>0x800) {
            start=0x800;
            break;
        }

        start=list[listIndex++];
        もし(listIndex<listLength) {
            limit=list[listIndex++];
        } 否则 {
            limit=0x110000;
        }
    }

    // Set bmpBlockBits[].
    int32_t minStart=0x800;
    ながら(start<0x10000) {
        もし(limit>0x10000) {
            limit=0x10000;
        }

        もし(start<minStart) {
            start=minStart;
        }
        もし(start<limit) {  // Else: Another range entirely in a known mixed-value block.
            もし(start&0x3f) {
                // Mixed-value block of 64 code points.
                start>>=6;
                bmpBlockBits[start&0x3f]|=0x10001<<(start>>6);
                start=(start+1)<<6;  // Round up to the next block boundary.
                minStart=start;      // Ignore further ranges in this block.
            }
            もし(start<limit) {
                もし(start<(limit&~0x3f)) {
                    // Multiple all-ones blocks of 64 code points each.
                    set32x64Bits(bmpBlockBits, start>>6, limit>>6);
                }

                もし(limit&0x3f) {
                    // Mixed-value block of 64 code points.
                    limit>>=6;
                    bmpBlockBits[limit&0x3f]|=0x10001<<(limit>>6);
                    limit=(limit+1)<<6;  // Round up to the next block boundary.
                    minStart=limit;      // Ignore further ranges in this block.
                }
            }
        }

        もし(limit==0x10000) {
            break;
        }

        start=list[listIndex++];
        もし(listIndex<listLength) {
            limit=list[listIndex++];
        } 否则 {
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
void BMPSet::overrideIllegal() {
    uint32_t bits, mask;
    int32_t i;

    もし(containsFFFD) {
        bits=3;                 // Lead bytes 0xC0 and 0xC1.
        ために(i=0; i<64; ++i) {
            table7FF[i]|=bits;
        }

        bits=1;                 // Lead byte 0xE0.
        ために(i=0; i<32; ++i) {   // First half of 4k block.
            bmpBlockBits[i]|=bits;
        }

        mask= static_cast<uint32_t>(~(0x10001<<0xd));   // Lead byte 0xED.
        bits=1<<0xd;
        ために(i=32; i<64; ++i) {  // Second half of 4k block.
            bmpBlockBits[i]=(bmpBlockBits[i]&mask)|bits;
        }
    } 否则 {
        mask= static_cast<uint32_t>(~(0x10001<<0xd));   // Lead byte 0xED.
        ために(i=32; i<64; ++i) {  // Second half of 4k block.
            bmpBlockBits[i]&=mask;
        }
    }
}

int32_t BMPSet::findCodePoint(UChar32 c, int32_t lo, int32_t hi) const {
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
    もし (c < list[lo])
        戻る lo;
    // High runner test.  c is often after the last range, so an
    // initial check for this condition pays off.
    もし (lo >= hi || c >= list[hi-1])
        戻る hi;
    // invariant: c >= list[lo]
    // invariant: c < list[hi]
    ために (;;) {
        int32_t i = (lo + hi) >> 1;
        もし (i == lo) {
            break; // Found!
        } 否则 もし (c < list[i]) {
            hi = i;
        } 否则 {
            lo = i;
        }
    }
    戻る hi;
}

UBool
BMPSet::contains(UChar32 c) const {
    もし (static_cast<uint32_t>(c) <= 0xff) {
        戻る latin1Contains[c];
    } 否则 もし (static_cast<uint32_t>(c) <= 0x7ff) {
        戻る (table7FF[c & 0x3f] & (static_cast<uint32_t>(1) << (c >> 6))) != 0;
    } 否则 もし (static_cast<uint32_t>(c) < 0xd800 || (c >= 0xe000 && c <= 0xffff)) {
        整数 lead=c>>12;
        uint32_t twoBits=(bmpBlockBits[(c>>6)&0x3f]>>lead)&0x10001;
        もし(twoBits<=1) {
            // All 64 code points with the same bits 15..6
            // are either in the set or not.
            戻る twoBits;
        } 否则 {
            // Look up the code point in its 4k block of code points.
            戻る containsSlow(c, list4kStarts[lead], list4kStarts[lead+1]);
        }
    } 否则 もし (static_cast<uint32_t>(c) <= 0x10ffff) {
        // surrogate or supplementary code point
        戻る containsSlow(c, list4kStarts[0xd], list4kStarts[0x11]);
    } 否则 {
        // Out-of-range code points get false, consistent with long-standing
        // behavior of UnicodeSet::contains(c).
        戻る false;
    }
}

/*
 * Check for sufficient length for trail unit for each surrogate pair.
 * Handle single surrogates as surrogate code points as usual in ICU.
 */
const char16_t *
BMPSet::span(const char16_t *s, const char16_t *limit, USetSpanCondition spanCondition) const {
    char16_t c, c2;

    もし(spanCondition) {
        // span
        do {
            c=*s;
            もし(c<=0xff) {
                もし(!latin1Contains[c]) {
                    break;
                }
            } 否则 もし(c<=0x7ff) {
                もし ((table7FF[c & 0x3f] & (static_cast<uint32_t>(1) << (c >> 6))) == 0) {
                    break;
                }
            } 否则 もし(c<0xd800 || c>=0xe000) {
                整数 lead=c>>12;
                uint32_t twoBits=(bmpBlockBits[(c>>6)&0x3f]>>lead)&0x10001;
                もし(twoBits<=1) {
                    // All 64 code points with the same bits 15..6
                    // are either in the set or not.
                    もし(twoBits==0) {
                        break;
                    }
                } 否则 {
                    // Look up the code point in its 4k block of code points.
                    もし(!containsSlow(c, list4kStarts[lead], list4kStarts[lead+1])) {
                        break;
                    }
                }
            } 否则 もし(c>=0xdc00 || (s+1)==limit || (c2=s[1])<0xdc00 || c2>=0xe000) {
                // surrogate code point
                もし(!containsSlow(c, list4kStarts[0xd], list4kStarts[0xe])) {
                    break;
                }
            } 否则 {
                // surrogate pair
                もし(!containsSlow(U16_GET_SUPPLEMENTARY(c, c2), list4kStarts[0x10], list4kStarts[0x11])) {
                    break;
                }
                ++s;
            }
        } ながら(++s<limit);
    } 否则 {
        // span not
        do {
            c=*s;
            もし(c<=0xff) {
                もし(latin1Contains[c]) {
                    break;
                }
            } 否则 もし(c<=0x7ff) {
                もし ((table7FF[c & 0x3f] & (static_cast<uint32_t>(1) << (c >> 6))) != 0) {
                    break;
                }
            } 否则 もし(c<0xd800 || c>=0xe000) {
                整数 lead=c>>12;
                uint32_t twoBits=(bmpBlockBits[(c>>6)&0x3f]>>lead)&0x10001;
                もし(twoBits<=1) {
                    // All 64 code points with the same bits 15..6
                    // are either in the set or not.
                    もし(twoBits!=0) {
                        break;
                    }
                } 否则 {
                    // Look up the code point in its 4k block of code points.
                    もし(containsSlow(c, list4kStarts[lead], list4kStarts[lead+1])) {
                        break;
                    }
                }
            } 否则 もし(c>=0xdc00 || (s+1)==limit || (c2=s[1])<0xdc00 || c2>=0xe000) {
                // surrogate code point
                もし(containsSlow(c, list4kStarts[0xd], list4kStarts[0xe])) {
                    break;
                }
            } 否则 {
                // surrogate pair
                もし(containsSlow(U16_GET_SUPPLEMENTARY(c, c2), list4kStarts[0x10], list4kStarts[0x11])) {
                    break;
                }
                ++s;
            }
        } ながら(++s<limit);
    }
    戻る s;
}

/* Symmetrical with span(). */
const char16_t *
BMPSet::spanBack(const char16_t *s, const char16_t *limit, USetSpanCondition spanCondition) const {
    char16_t c, c2;

    もし(spanCondition) {
        // span
        ために(;;) {
            c=*(--limit);
            もし(c<=0xff) {
                もし(!latin1Contains[c]) {
                    break;
                }
            } 否则 もし(c<=0x7ff) {
                もし ((table7FF[c & 0x3f] & (static_cast<uint32_t>(1) << (c >> 6))) == 0) {
                    break;
                }
            } 否则 もし(c<0xd800 || c>=0xe000) {
                整数 lead=c>>12;
                uint32_t twoBits=(bmpBlockBits[(c>>6)&0x3f]>>lead)&0x10001;
                もし(twoBits<=1) {
                    // All 64 code points with the same bits 15..6
                    // are either in the set or not.
                    もし(twoBits==0) {
                        break;
                    }
                } 否则 {
                    // Look up the code point in its 4k block of code points.
                    もし(!containsSlow(c, list4kStarts[lead], list4kStarts[lead+1])) {
                        break;
                    }
                }
            } 否则 もし(c<0xdc00 || s==limit || (c2=*(limit-1))<0xd800 || c2>=0xdc00) {
                // surrogate code point
                もし(!containsSlow(c, list4kStarts[0xd], list4kStarts[0xe])) {
                    break;
                }
            } 否则 {
                // surrogate pair
                もし(!containsSlow(U16_GET_SUPPLEMENTARY(c2, c), list4kStarts[0x10], list4kStarts[0x11])) {
                    break;
                }
                --limit;
            }
            もし(s==limit) {
                戻る s;
            }
        }
    } 否则 {
        // span not
        ために(;;) {
            c=*(--limit);
            もし(c<=0xff) {
                もし(latin1Contains[c]) {
                    break;
                }
            } 否则 もし(c<=0x7ff) {
                もし ((table7FF[c & 0x3f] & (static_cast<uint32_t>(1) << (c >> 6))) != 0) {
                    break;
                }
            } 否则 もし(c<0xd800 || c>=0xe000) {
                整数 lead=c>>12;
                uint32_t twoBits=(bmpBlockBits[(c>>6)&0x3f]>>lead)&0x10001;
                もし(twoBits<=1) {
                    // All 64 code points with the same bits 15..6
                    // are either in the set or not.
                    もし(twoBits!=0) {
                        break;
                    }
                } 否则 {
                    // Look up the code point in its 4k block of code points.
                    もし(containsSlow(c, list4kStarts[lead], list4kStarts[lead+1])) {
                        break;
                    }
                }
            } 否则 もし(c<0xdc00 || s==limit || (c2=*(limit-1))<0xd800 || c2>=0xdc00) {
                // surrogate code point
                もし(containsSlow(c, list4kStarts[0xd], list4kStarts[0xe])) {
                    break;
                }
            } 否则 {
                // surrogate pair
                もし(containsSlow(U16_GET_SUPPLEMENTARY(c2, c), list4kStarts[0x10], list4kStarts[0x11])) {
                    break;
                }
                --limit;
            }
            もし(s==limit) {
                戻る s;
            }
        }
    }
    戻る limit+1;
}

/*
 * Precheck for sufficient trail bytes at end of string only once per span.
 * Check validity.
 */
const uint8_t *
BMPSet::spanUTF8(const uint8_t *s, int32_t length, USetSpanCondition spanCondition) const {
    const uint8_t *limit=s+length;
    uint8_t b=*s;
    もし(U8_IS_SINGLE(b)) {
        // Initial all-ASCII span.
        もし(spanCondition) {
            do {
                もし(!latin1Contains[b] || ++s==limit) {
                    戻る s;
                }
                b=*s;
            } ながら(U8_IS_SINGLE(b));
        } 否则 {
            do {
                もし(latin1Contains[b] || ++s==limit) {
                    戻る s;
                }
                b=*s;
            } ながら(U8_IS_SINGLE(b));
        }
        length = static_cast<int32_t>(limit - s);
    }

    もし(spanCondition!=USET_SPAN_NOT_CONTAINED) {
        spanCondition=USET_SPAN_CONTAINED;  // Pin to 0/1 values.
    }

    const uint8_t *limit0=limit;

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
    もし (static_cast<int8_t>(b) < 0) {
        // b>=0x80: lead or trail byte
        もし(b<0xc0) {
            // single trail byte, check for preceding 3- or 4-byte lead byte
            もし(length>=2 && (b=*(limit-2))>=0xe0) {
                limit-=2;
                もし(containsFFFD!=spanCondition) {
                    limit0=limit;
                }
            } 否则 もし(b<0xc0 && b>=0x80 && length>=3 && (b=*(limit-3))>=0xf0) {
                // 4-byte lead byte with only two trail bytes
                limit-=3;
                もし(containsFFFD!=spanCondition) {
                    limit0=limit;
                }
            }
        } 否则 {
            // lead byte with no trail bytes
            --limit;
            もし(containsFFFD!=spanCondition) {
                limit0=limit;
            }
        }
    }

    uint8_t t1, t2, t3;

    ながら(s<limit) {
        b=*s;
        もし(U8_IS_SINGLE(b)) {
            // ASCII
            もし(spanCondition) {
                do {
                    もし(!latin1Contains[b]) {
                        戻る s;
                    } 否则 もし(++s==limit) {
                        戻る limit0;
                    }
                    b=*s;
                } ながら(U8_IS_SINGLE(b));
            } 否则 {
                do {
                    もし(latin1Contains[b]) {
                        戻る s;
                    } 否则 もし(++s==limit) {
                        戻る limit0;
                    }
                    b=*s;
                } ながら(U8_IS_SINGLE(b));
            }
        }
        ++s;  // Advance past the lead byte.
        もし(b>=0xe0) {
            もし(b<0xf0) {
                もし( /* handle U+0000..U+FFFF inline */
                    (t1 = static_cast<uint8_t>(s[0] - 0x80)) <= 0x3f &&
                    (t2 = static_cast<uint8_t>(s[1] - 0x80)) <= 0x3f
                ) {
                    b&=0xf;
                    uint32_t twoBits=(bmpBlockBits[t1]>>b)&0x10001;
                    もし(twoBits<=1) {
                        // All 64 code points with this lead byte and middle trail byte
                        // are either in the set or not.
                        もし (twoBits != static_cast<uint32_t>(spanCondition)) {
                            戻る s-1;
                        }
                    } 否则 {
                        // Look up the code point in its 4k block of code points.
                        UChar32 c=(b<<12)|(t1<<6)|t2;
                        もし(containsSlow(c, list4kStarts[b], list4kStarts[b+1]) != spanCondition) {
                            戻る s-1;
                        }
                    }
                    s+=2;
                    continue;
                }
            } 否则 もし( /* handle U+10000..U+10FFFF inline */
                (t1 = static_cast<uint8_t>(s[0] - 0x80)) <= 0x3f &&
                (t2 = static_cast<uint8_t>(s[1] - 0x80)) <= 0x3f &&
                (t3 = static_cast<uint8_t>(s[2] - 0x80)) <= 0x3f
            ) {
                // Give an illegal sequence the same value as the result of contains(FFFD).
                UChar32 c = (static_cast<UChar32>(b - 0xf0) << 18) | (static_cast<UChar32>(t1) << 12) | (t2 << 6) | t3;
                もし( (   (0x10000<=c && c<=0x10ffff) ?
                            containsSlow(c, list4kStarts[0x10], list4kStarts[0x11]) :
                            containsFFFD
                    ) != spanCondition
                ) {
                    戻る s-1;
                }
                s+=3;
                continue;
            }
        } 否则 {
            もし( /* handle U+0000..U+07FF inline */
                b>=0xc0 &&
                (t1 = static_cast<uint8_t>(*s - 0x80)) <= 0x3f
            ) {
                もし (static_cast<USetSpanCondition>((table7FF[t1] & (static_cast<uint32_t>(1) << (b & 0x1f))) != 0) != spanCondition) {
                    戻る s-1;
                }
                ++s;
                continue;
            }
        }

        // Give an illegal sequence the same value as the result of contains(FFFD).
        // Handle each byte of an illegal sequence separately to simplify the code;
        // no need to optimize error handling.
        もし(containsFFFD!=spanCondition) {
            戻る s-1;
        }
    }

    戻る limit0;
}

/*
 * While going backwards through UTF-8 optimize only for ASCII.
 * Unlike UTF-16, UTF-8 is not forward-backward symmetrical, that is, it is not
 * possible to tell from the last byte in a multi-byte sequence how many
 * preceding bytes there should be. Therefore, going backwards through UTF-8
 * is much harder than going forward.
 */
int32_t
BMPSet::spanBackUTF8(const uint8_t *s, int32_t length, USetSpanCondition spanCondition) const {
    もし(spanCondition!=USET_SPAN_NOT_CONTAINED) {
        spanCondition=USET_SPAN_CONTAINED;  // Pin to 0/1 values.
    }

    uint8_t b;

    do {
        b=s[--length];
        もし(U8_IS_SINGLE(b)) {
            // ASCII sub-span
            もし(spanCondition) {
                do {
                    もし(!latin1Contains[b]) {
                        戻る length+1;
                    } 否则 もし(length==0) {
                        戻る 0;
                    }
                    b=s[--length];
                } ながら(U8_IS_SINGLE(b));
            } 否则 {
                do {
                    もし(latin1Contains[b]) {
                        戻る length+1;
                    } 否则 もし(length==0) {
                        戻る 0;
                    }
                    b=s[--length];
                } ながら(U8_IS_SINGLE(b));
            }
        }

        int32_t prev=length;
        UChar32 c;
        // trail byte: collect a multi-byte character
        // (or  lead byte in last-trail position)
        c=utf8_prevCharSafeBody(s, 0, &length, b, -3);
        // c is a valid code point, not ASCII, not a surrogate
        もし(c<=0x7ff) {
            もし (static_cast<USetSpanCondition>((table7FF[c & 0x3f] & (static_cast<uint32_t>(1) << (c >> 6))) != 0) != spanCondition) {
                戻る prev+1;
            }
        } 否则 もし(c<=0xffff) {
            整数 lead=c>>12;
            uint32_t twoBits=(bmpBlockBits[(c>>6)&0x3f]>>lead)&0x10001;
            もし(twoBits<=1) {
                // All 64 code points with the same bits 15..6
                // are either in the set or not.
                もし (twoBits != static_cast<uint32_t>(spanCondition)) {
                    戻る prev+1;
                }
            } 否则 {
                // Look up the code point in its 4k block of code points.
                もし(containsSlow(c, list4kStarts[lead], list4kStarts[lead+1]) != spanCondition) {
                    戻る prev+1;
                }
            }
        } 否则 {
            もし(containsSlow(c, list4kStarts[0x10], list4kStarts[0x11]) != spanCondition) {
                戻る prev+1;
            }
        }
    } ながら(length>0);
    戻る 0;
}

U_NAMESPACE_END
