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

#అన్తర్భావ "unicode/utypes.h"
#అన్తర్భావ "unicode/uniset.h"
#అన్తర్భావ "unicode/utf8.h"
#అన్తర్భావ "unicode/utf16.h"
#అన్తర్భావ "cmemory.h"
#అన్తర్భావ "bmpset.h"
#అన్తర్భావ "uassert.h"

U_NAMESPACE_BEGIN

BMPSet::BMPSet(స్థిరరాశి int32_t *మాతృ_List, int32_t మాతృ_List_నిడివి) :
        list(మాతృ_List), list_నిడివి(మాతృ_List_నిడివి) {
    uprv_memset(latin1_కలిగియుంది, 0, పరిమాణం(latin1_కలిగియుంది));
    uprv_memset(table7FF, 0, పరిమాణం(table7FF));
    uprv_memset(bmp_ఖండం_బిట్లు, 0, పరిమాణం(bmp_ఖండం_బిట్లు));

    /*
     * Set the list indexes for binary searches for
     * U+0800, U+1000, U+2000, .., U+F000, U+10000.
     * U+0800 is the first 3-byte-UTF-8 code point. Lower code points are
     * looked up in the bit tables.
     * The last pair of indexes is for finding supplementary code points.
     */
    list4kStarts[0]=కనుగొను_సంకేతం_బిందువు(0x800, 0, list_నిడివి-1);
    int32_t i;
    కొరకు(i=1; i<=0x10; ++i) {
        list4kStarts[i]=కనుగొను_సంకేతం_బిందువు(i<<12, list4kStarts[i-1], list_నిడివి-1);
    }
    list4kStarts[0x11]=list_నిడివి-1;
    కలిగియుంది_FFFD=కలిగియుంది_Slow(0xfffd, list4kStarts[0xf], list4kStarts[0x10]);

    init_బిట్లు();
    overrideIllegal();
}

BMPSet::BMPSet(స్థిరరాశి BMPSet &otherBMPSet, స్థిరరాశి int32_t *new_మాతృ_List, int32_t new_మాతృ_List_నిడివి) :
        కలిగియుంది_FFFD(otherBMPSet.కలిగియుంది_FFFD),
        list(new_మాతృ_List), list_నిడివి(new_మాతృ_List_నిడివి) {
    uprv_memcpy(latin1_కలిగియుంది, otherBMPSet.latin1_కలిగియుంది, పరిమాణం(latin1_కలిగియుంది));
    uprv_memcpy(table7FF, otherBMPSet.table7FF, పరిమాణం(table7FF));
    uprv_memcpy(bmp_ఖండం_బిట్లు, otherBMPSet.bmp_ఖండం_బిట్లు, పరిమాణం(bmp_ఖండం_బిట్లు));
    uprv_memcpy(list4kStarts, otherBMPSet.list4kStarts, పరిమాణం(list4kStarts));
}

BMPSet::~BMPSet() {
}

/*
 * Set bits in a bit rectangle in "vertical" bit organization.
 * start<limit<=0x800
 */
స్థిర శూన్య set32x64_బిట్లు(uint32_t పట్టిక[64], int32_t ప్రారంభం, int32_t హద్దు) {
    U_ASSERT(ప్రారంభం<హద్దు);
    U_ASSERT(హద్దు<=0x800);

    int32_t lead=ప్రారంభం>>6;  // Named for UTF-8 2-byte lead byte with upper 5 bits.
    int32_t trail=ప్రారంభం&0x3f;  // Named for UTF-8 2-byte trail byte with lower 6 bits.

    // Set one bit indicating an all-one block.
    uint32_t బిట్లు = static_cast<uint32_t>(1) << lead;
    ఒకవేళ((ప్రారంభం+1)==హద్దు) {  // Single-character shortcut.
        పట్టిక[trail]|=బిట్లు;
        తిరిగి;
    }

    int32_t హద్దు_Lead=హద్దు>>6;
    int32_t హద్దు_Trail=హద్దు&0x3f;

    ఒకవేళ(lead==హద్దు_Lead) {
        // Partial vertical bit column.
        అయితే(trail<హద్దు_Trail) {
            పట్టిక[trail++]|=బిట్లు;
        }
    } లేకపోతే {
        // Partial vertical bit column,
        // followed by a bit rectangle,
        // followed by another partial vertical bit column.
        ఒకవేళ(trail>0) {
            చేయి {
                పట్టిక[trail++]|=బిట్లు;
            } అయితే(trail<64);
            ++lead;
        }
        ఒకవేళ(lead<హద్దు_Lead) {
            బిట్లు = ~((static_cast<గుర్తులేని>(1) << lead) - 1);
            ఒకవేళ(హద్దు_Lead<0x20) {
                బిట్లు &= (static_cast<గుర్తులేని>(1) << హద్దు_Lead) - 1;
            }
            కొరకు(trail=0; trail<64; ++trail) {
                పట్టిక[trail]|=బిట్లు;
            }
        }
        // limit<=0x800. If limit==0x800 then limitLead=32 and limitTrail=0.
        // In that case, bits=1<<limitLead is undefined but the bits value
        // is not used because trail<limitTrail is already false.
        బిట్లు = static_cast<uint32_t>(1) << ((హద్దు_Lead == 0x20) ? (హద్దు_Lead - 1) : హద్దు_Lead);
        కొరకు(trail=0; trail<హద్దు_Trail; ++trail) {
            పట్టిక[trail]|=బిట్లు;
        }
    }
}

శూన్య BMPSet::init_బిట్లు() {
    UChar32 ప్రారంభం, హద్దు;
    int32_t list_సూచీ=0;

    // Set latin1Contains[].
    చేయి {
        ప్రారంభం=list[list_సూచీ++];
        ఒకవేళ(list_సూచీ<list_నిడివి) {
            హద్దు=list[list_సూచీ++];
        } లేకపోతే {
            హద్దు=0x110000;
        }
        ఒకవేళ(ప్రారంభం>=0x100) {
            ఆపు;
        }
        చేయి {
            latin1_కలిగియుంది[ప్రారంభం++]=1;
        } అయితే(ప్రారంభం<హద్దు && ప్రారంభం<0x100);
    } అయితే(హద్దు<=0x100);

    // Find the first range overlapping with (or after) 80..FF again,
    // to include them in table7FF as well.
    కొరకు(list_సూచీ=0;;) {
        ప్రారంభం=list[list_సూచీ++];
        ఒకవేళ(list_సూచీ<list_నిడివి) {
            హద్దు=list[list_సూచీ++];
        } లేకపోతే {
            హద్దు=0x110000;
        }
        ఒకవేళ(హద్దు>0x80) {
            ఒకవేళ(ప్రారంభం<0x80) {
                ప్రారంభం=0x80;
            }
            ఆపు;
        }
    }

    // Set table7FF[].
    అయితే(ప్రారంభం<0x800) {
        set32x64_బిట్లు(table7FF, ప్రారంభం, హద్దు<=0x800 ? హద్దు : 0x800);
        ఒకవేళ(హద్దు>0x800) {
            ప్రారంభం=0x800;
            ఆపు;
        }

        ప్రారంభం=list[list_సూచీ++];
        ఒకవేళ(list_సూచీ<list_నిడివి) {
            హద్దు=list[list_సూచీ++];
        } లేకపోతే {
            హద్దు=0x110000;
        }
    }

    // Set bmpBlockBits[].
    int32_t min_ప్రారంభం=0x800;
    అయితే(ప్రారంభం<0x10000) {
        ఒకవేళ(హద్దు>0x10000) {
            హద్దు=0x10000;
        }

        ఒకవేళ(ప్రారంభం<min_ప్రారంభం) {
            ప్రారంభం=min_ప్రారంభం;
        }
        ఒకవేళ(ప్రారంభం<హద్దు) {  // Else: Another range entirely in a known mixed-value block.
            ఒకవేళ(ప్రారంభం&0x3f) {
                // Mixed-value block of 64 code points.
                ప్రారంభం>>=6;
                bmp_ఖండం_బిట్లు[ప్రారంభం&0x3f]|=0x10001<<(ప్రారంభం>>6);
                ప్రారంభం=(ప్రారంభం+1)<<6;  // Round up to the next block boundary.
                min_ప్రారంభం=ప్రారంభం;      // Ignore further ranges in this block.
            }
            ఒకవేళ(ప్రారంభం<హద్దు) {
                ఒకవేళ(ప్రారంభం<(హద్దు&~0x3f)) {
                    // Multiple all-ones blocks of 64 code points each.
                    set32x64_బిట్లు(bmp_ఖండం_బిట్లు, ప్రారంభం>>6, హద్దు>>6);
                }

                ఒకవేళ(హద్దు&0x3f) {
                    // Mixed-value block of 64 code points.
                    హద్దు>>=6;
                    bmp_ఖండం_బిట్లు[హద్దు&0x3f]|=0x10001<<(హద్దు>>6);
                    హద్దు=(హద్దు+1)<<6;  // Round up to the next block boundary.
                    min_ప్రారంభం=హద్దు;      // Ignore further ranges in this block.
                }
            }
        }

        ఒకవేళ(హద్దు==0x10000) {
            ఆపు;
        }

        ప్రారంభం=list[list_సూచీ++];
        ఒకవేళ(list_సూచీ<list_నిడివి) {
            హద్దు=list[list_సూచీ++];
        } లేకపోతే {
            హద్దు=0x110000;
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
శూన్య BMPSet::overrideIllegal() {
    uint32_t బిట్లు, mask;
    int32_t i;

    ఒకవేళ(కలిగియుంది_FFFD) {
        బిట్లు=3;                 // Lead bytes 0xC0 and 0xC1.
        కొరకు(i=0; i<64; ++i) {
            table7FF[i]|=బిట్లు;
        }

        బిట్లు=1;                 // Lead byte 0xE0.
        కొరకు(i=0; i<32; ++i) {   // First half of 4k block.
            bmp_ఖండం_బిట్లు[i]|=బిట్లు;
        }

        mask= static_cast<uint32_t>(~(0x10001<<0xd));   // Lead byte 0xED.
        బిట్లు=1<<0xd;
        కొరకు(i=32; i<64; ++i) {  // Second half of 4k block.
            bmp_ఖండం_బిట్లు[i]=(bmp_ఖండం_బిట్లు[i]&mask)|బిట్లు;
        }
    } లేకపోతే {
        mask= static_cast<uint32_t>(~(0x10001<<0xd));   // Lead byte 0xED.
        కొరకు(i=32; i<64; ++i) {  // Second half of 4k block.
            bmp_ఖండం_బిట్లు[i]&=mask;
        }
    }
}

int32_t BMPSet::కనుగొను_సంకేతం_బిందువు(UChar32 c, int32_t lo, int32_t hi) స్థిరరాశి {
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
    ఒకవేళ (c < list[lo])
        తిరిగి lo;
    // High runner test.  c is often after the last range, so an
    // initial check for this condition pays off.
    ఒకవేళ (lo >= hi || c >= list[hi-1])
        తిరిగి hi;
    // invariant: c >= list[lo]
    // invariant: c < list[hi]
    కొరకు (;;) {
        int32_t i = (lo + hi) >> 1;
        ఒకవేళ (i == lo) {
            ఆపు; // Found!
        } లేకపోతే ఒకవేళ (c < list[i]) {
            hi = i;
        } లేకపోతే {
            lo = i;
        }
    }
    తిరిగి hi;
}

UBool
BMPSet::కలిగియుంది(UChar32 c) స్థిరరాశి {
    ఒకవేళ (static_cast<uint32_t>(c) <= 0xff) {
        తిరిగి latin1_కలిగియుంది[c];
    } లేకపోతే ఒకవేళ (static_cast<uint32_t>(c) <= 0x7ff) {
        తిరిగి (table7FF[c & 0x3f] & (static_cast<uint32_t>(1) << (c >> 6))) != 0;
    } లేకపోతే ఒకవేళ (static_cast<uint32_t>(c) < 0xd800 || (c >= 0xe000 && c <= 0xffff)) {
        పూర్ణాంకం lead=c>>12;
        uint32_t two_బిట్లు=(bmp_ఖండం_బిట్లు[(c>>6)&0x3f]>>lead)&0x10001;
        ఒకవేళ(two_బిట్లు<=1) {
            // All 64 code points with the same bits 15..6
            // are either in the set or not.
            తిరిగి two_బిట్లు;
        } లేకపోతే {
            // Look up the code point in its 4k block of code points.
            తిరిగి కలిగియుంది_Slow(c, list4kStarts[lead], list4kStarts[lead+1]);
        }
    } లేకపోతే ఒకవేళ (static_cast<uint32_t>(c) <= 0x10ffff) {
        // surrogate or supplementary code point
        తిరిగి కలిగియుంది_Slow(c, list4kStarts[0xd], list4kStarts[0x11]);
    } లేకపోతే {
        // Out-of-range code points get false, consistent with long-standing
        // behavior of UnicodeSet::contains(c).
        తిరిగి అబద్ధం;
    }
}

/*
 * Check for sufficient length for trail unit for each surrogate pair.
 * Handle single surrogates as surrogate code points as usual in ICU.
 */
స్థిరరాశి char16_t *
BMPSet::పరిధి(స్థిరరాశి char16_t *s, స్థిరరాశి char16_t *హద్దు, USet_పరిధి_Condition పరిధి_Condition) స్థిరరాశి {
    char16_t c, c2;

    ఒకవేళ(పరిధి_Condition) {
        //పరిధి
        చేయి {
            c=*s;
            ఒకవేళ(c<=0xff) {
                ఒకవేళ(!latin1_కలిగియుంది[c]) {
                    ఆపు;
                }
            } లేకపోతే ఒకవేళ(c<=0x7ff) {
                ఒకవేళ ((table7FF[c & 0x3f] & (static_cast<uint32_t>(1) << (c >> 6))) == 0) {
                    ఆపు;
                }
            } లేకపోతే ఒకవేళ(c<0xd800 || c>=0xe000) {
                పూర్ణాంకం lead=c>>12;
                uint32_t two_బిట్లు=(bmp_ఖండం_బిట్లు[(c>>6)&0x3f]>>lead)&0x10001;
                ఒకవేళ(two_బిట్లు<=1) {
                    // All 64 code points with the same bits 15..6
                    // are either in the set or not.
                    ఒకవేళ(two_బిట్లు==0) {
                        ఆపు;
                    }
                } లేకపోతే {
                    // Look up the code point in its 4k block of code points.
                    ఒకవేళ(!కలిగియుంది_Slow(c, list4kStarts[lead], list4kStarts[lead+1])) {
                        ఆపు;
                    }
                }
            } లేకపోతే ఒకవేళ(c>=0xdc00 || (s+1)==హద్దు || (c2=s[1])<0xdc00 || c2>=0xe000) {
                // surrogate code point
                ఒకవేళ(!కలిగియుంది_Slow(c, list4kStarts[0xd], list4kStarts[0xe])) {
                    ఆపు;
                }
            } లేకపోతే {
                // surrogate pair
                ఒకవేళ(!కలిగియుంది_Slow(U16_GET_SUPPLEMENTARY(c, c2), list4kStarts[0x10], list4kStarts[0x11])) {
                    ఆపు;
                }
                ++s;
            }
        } అయితే(++s<హద్దు);
    } లేకపోతే {
        // span not
        చేయి {
            c=*s;
            ఒకవేళ(c<=0xff) {
                ఒకవేళ(latin1_కలిగియుంది[c]) {
                    ఆపు;
                }
            } లేకపోతే ఒకవేళ(c<=0x7ff) {
                ఒకవేళ ((table7FF[c & 0x3f] & (static_cast<uint32_t>(1) << (c >> 6))) != 0) {
                    ఆపు;
                }
            } లేకపోతే ఒకవేళ(c<0xd800 || c>=0xe000) {
                పూర్ణాంకం lead=c>>12;
                uint32_t two_బిట్లు=(bmp_ఖండం_బిట్లు[(c>>6)&0x3f]>>lead)&0x10001;
                ఒకవేళ(two_బిట్లు<=1) {
                    // All 64 code points with the same bits 15..6
                    // are either in the set or not.
                    ఒకవేళ(two_బిట్లు!=0) {
                        ఆపు;
                    }
                } లేకపోతే {
                    // Look up the code point in its 4k block of code points.
                    ఒకవేళ(కలిగియుంది_Slow(c, list4kStarts[lead], list4kStarts[lead+1])) {
                        ఆపు;
                    }
                }
            } లేకపోతే ఒకవేళ(c>=0xdc00 || (s+1)==హద్దు || (c2=s[1])<0xdc00 || c2>=0xe000) {
                // surrogate code point
                ఒకవేళ(కలిగియుంది_Slow(c, list4kStarts[0xd], list4kStarts[0xe])) {
                    ఆపు;
                }
            } లేకపోతే {
                // surrogate pair
                ఒకవేళ(కలిగియుంది_Slow(U16_GET_SUPPLEMENTARY(c, c2), list4kStarts[0x10], list4kStarts[0x11])) {
                    ఆపు;
                }
                ++s;
            }
        } అయితే(++s<హద్దు);
    }
    తిరిగి s;
}

/* Symmetrical with span(). */
స్థిరరాశి char16_t *
BMPSet::పరిధి_వెనుక(స్థిరరాశి char16_t *s, స్థిరరాశి char16_t *హద్దు, USet_పరిధి_Condition పరిధి_Condition) స్థిరరాశి {
    char16_t c, c2;

    ఒకవేళ(పరిధి_Condition) {
        //పరిధి
        కొరకు(;;) {
            c=*(--హద్దు);
            ఒకవేళ(c<=0xff) {
                ఒకవేళ(!latin1_కలిగియుంది[c]) {
                    ఆపు;
                }
            } లేకపోతే ఒకవేళ(c<=0x7ff) {
                ఒకవేళ ((table7FF[c & 0x3f] & (static_cast<uint32_t>(1) << (c >> 6))) == 0) {
                    ఆపు;
                }
            } లేకపోతే ఒకవేళ(c<0xd800 || c>=0xe000) {
                పూర్ణాంకం lead=c>>12;
                uint32_t two_బిట్లు=(bmp_ఖండం_బిట్లు[(c>>6)&0x3f]>>lead)&0x10001;
                ఒకవేళ(two_బిట్లు<=1) {
                    // All 64 code points with the same bits 15..6
                    // are either in the set or not.
                    ఒకవేళ(two_బిట్లు==0) {
                        ఆపు;
                    }
                } లేకపోతే {
                    // Look up the code point in its 4k block of code points.
                    ఒకవేళ(!కలిగియుంది_Slow(c, list4kStarts[lead], list4kStarts[lead+1])) {
                        ఆపు;
                    }
                }
            } లేకపోతే ఒకవేళ(c<0xdc00 || s==హద్దు || (c2=*(హద్దు-1))<0xd800 || c2>=0xdc00) {
                // surrogate code point
                ఒకవేళ(!కలిగియుంది_Slow(c, list4kStarts[0xd], list4kStarts[0xe])) {
                    ఆపు;
                }
            } లేకపోతే {
                // surrogate pair
                ఒకవేళ(!కలిగియుంది_Slow(U16_GET_SUPPLEMENTARY(c2, c), list4kStarts[0x10], list4kStarts[0x11])) {
                    ఆపు;
                }
                --హద్దు;
            }
            ఒకవేళ(s==హద్దు) {
                తిరిగి s;
            }
        }
    } లేకపోతే {
        // span not
        కొరకు(;;) {
            c=*(--హద్దు);
            ఒకవేళ(c<=0xff) {
                ఒకవేళ(latin1_కలిగియుంది[c]) {
                    ఆపు;
                }
            } లేకపోతే ఒకవేళ(c<=0x7ff) {
                ఒకవేళ ((table7FF[c & 0x3f] & (static_cast<uint32_t>(1) << (c >> 6))) != 0) {
                    ఆపు;
                }
            } లేకపోతే ఒకవేళ(c<0xd800 || c>=0xe000) {
                పూర్ణాంకం lead=c>>12;
                uint32_t two_బిట్లు=(bmp_ఖండం_బిట్లు[(c>>6)&0x3f]>>lead)&0x10001;
                ఒకవేళ(two_బిట్లు<=1) {
                    // All 64 code points with the same bits 15..6
                    // are either in the set or not.
                    ఒకవేళ(two_బిట్లు!=0) {
                        ఆపు;
                    }
                } లేకపోతే {
                    // Look up the code point in its 4k block of code points.
                    ఒకవేళ(కలిగియుంది_Slow(c, list4kStarts[lead], list4kStarts[lead+1])) {
                        ఆపు;
                    }
                }
            } లేకపోతే ఒకవేళ(c<0xdc00 || s==హద్దు || (c2=*(హద్దు-1))<0xd800 || c2>=0xdc00) {
                // surrogate code point
                ఒకవేళ(కలిగియుంది_Slow(c, list4kStarts[0xd], list4kStarts[0xe])) {
                    ఆపు;
                }
            } లేకపోతే {
                // surrogate pair
                ఒకవేళ(కలిగియుంది_Slow(U16_GET_SUPPLEMENTARY(c2, c), list4kStarts[0x10], list4kStarts[0x11])) {
                    ఆపు;
                }
                --హద్దు;
            }
            ఒకవేళ(s==హద్దు) {
                తిరిగి s;
            }
        }
    }
    తిరిగి హద్దు+1;
}

/*
 * Precheck for sufficient trail bytes at end of string only once per span.
 * Check validity.
 */
స్థిరరాశి uint8_t *
BMPSet::పరిధి_UTF8(స్థిరరాశి uint8_t *s, int32_t length, USet_పరిధి_Condition పరిధి_Condition) స్థిరరాశి {
    స్థిరరాశి uint8_t *హద్దు=s+length;
    uint8_t b=*s;
    ఒకవేళ(U8_IS_SINGLE(b)) {
        // Initial all-ASCII span.
        ఒకవేళ(పరిధి_Condition) {
            చేయి {
                ఒకవేళ(!latin1_కలిగియుంది[b] || ++s==హద్దు) {
                    తిరిగి s;
                }
                b=*s;
            } అయితే(U8_IS_SINGLE(b));
        } లేకపోతే {
            చేయి {
                ఒకవేళ(latin1_కలిగియుంది[b] || ++s==హద్దు) {
                    తిరిగి s;
                }
                b=*s;
            } అయితే(U8_IS_SINGLE(b));
        }
        length = static_cast<int32_t>(హద్దు - s);
    }

    ఒకవేళ(పరిధి_Condition!=USET_పరిధి_NOT_CONTAINED) {
        పరిధి_Condition=USET_పరిధి_CONTAINED;  // Pin to 0/1 values.
    }

    స్థిరరాశి uint8_t *limit0=హద్దు;

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
    b=*(హద్దు-1);
    ఒకవేళ (static_cast<int8_t>(b) < 0) {
        // b>=0x80: lead or trail byte
        ఒకవేళ(b<0xc0) {
            // single trail byte, check for preceding 3- or 4-byte lead byte
            ఒకవేళ(length>=2 && (b=*(హద్దు-2))>=0xe0) {
                హద్దు-=2;
                ఒకవేళ(కలిగియుంది_FFFD!=పరిధి_Condition) {
                    limit0=హద్దు;
                }
            } లేకపోతే ఒకవేళ(b<0xc0 && b>=0x80 && length>=3 && (b=*(హద్దు-3))>=0xf0) {
                // 4-byte lead byte with only two trail bytes
                హద్దు-=3;
                ఒకవేళ(కలిగియుంది_FFFD!=పరిధి_Condition) {
                    limit0=హద్దు;
                }
            }
        } లేకపోతే {
            // lead byte with no trail bytes
            --హద్దు;
            ఒకవేళ(కలిగియుంది_FFFD!=పరిధి_Condition) {
                limit0=హద్దు;
            }
        }
    }

    uint8_t t1, t2, t3;

    అయితే(s<హద్దు) {
        b=*s;
        ఒకవేళ(U8_IS_SINGLE(b)) {
            // ASCII
            ఒకవేళ(పరిధి_Condition) {
                చేయి {
                    ఒకవేళ(!latin1_కలిగియుంది[b]) {
                        తిరిగి s;
                    } లేకపోతే ఒకవేళ(++s==హద్దు) {
                        తిరిగి limit0;
                    }
                    b=*s;
                } అయితే(U8_IS_SINGLE(b));
            } లేకపోతే {
                చేయి {
                    ఒకవేళ(latin1_కలిగియుంది[b]) {
                        తిరిగి s;
                    } లేకపోతే ఒకవేళ(++s==హద్దు) {
                        తిరిగి limit0;
                    }
                    b=*s;
                } అయితే(U8_IS_SINGLE(b));
            }
        }
        ++s;  // Advance past the lead byte.
        ఒకవేళ(b>=0xe0) {
            ఒకవేళ(b<0xf0) {
                ఒకవేళ( /* handle U+0000..U+FFFF inline */
                    (t1 = static_cast<uint8_t>(s[0] - 0x80)) <= 0x3f &&
                    (t2 = static_cast<uint8_t>(s[1] - 0x80)) <= 0x3f
                ) {
                    b&=0xf;
                    uint32_t two_బిట్లు=(bmp_ఖండం_బిట్లు[t1]>>b)&0x10001;
                    ఒకవేళ(two_బిట్లు<=1) {
                        // All 64 code points with this lead byte and middle trail byte
                        // are either in the set or not.
                        ఒకవేళ (two_బిట్లు != static_cast<uint32_t>(పరిధి_Condition)) {
                            తిరిగి s-1;
                        }
                    } లేకపోతే {
                        // Look up the code point in its 4k block of code points.
                        UChar32 c=(b<<12)|(t1<<6)|t2;
                        ఒకవేళ(కలిగియుంది_Slow(c, list4kStarts[b], list4kStarts[b+1]) != పరిధి_Condition) {
                            తిరిగి s-1;
                        }
                    }
                    s+=2;
                    కొనసాగు;
                }
            } లేకపోతే ఒకవేళ( /* handle U+10000..U+10FFFF inline */
                (t1 = static_cast<uint8_t>(s[0] - 0x80)) <= 0x3f &&
                (t2 = static_cast<uint8_t>(s[1] - 0x80)) <= 0x3f &&
                (t3 = static_cast<uint8_t>(s[2] - 0x80)) <= 0x3f
            ) {
                // Give an illegal sequence the same value as the result of contains(FFFD).
                UChar32 c = (static_cast<UChar32>(b - 0xf0) << 18) | (static_cast<UChar32>(t1) << 12) | (t2 << 6) | t3;
                ఒకవేళ( (   (0x10000<=c && c<=0x10ffff) ?
                            కలిగియుంది_Slow(c, list4kStarts[0x10], list4kStarts[0x11]) :
                            కలిగియుంది_FFFD
                    ) != పరిధి_Condition
                ) {
                    తిరిగి s-1;
                }
                s+=3;
                కొనసాగు;
            }
        } లేకపోతే {
            ఒకవేళ( /* handle U+0000..U+07FF inline */
                b>=0xc0 &&
                (t1 = static_cast<uint8_t>(*s - 0x80)) <= 0x3f
            ) {
                ఒకవేళ (static_cast<USet_పరిధి_Condition>((table7FF[t1] & (static_cast<uint32_t>(1) << (b & 0x1f))) != 0) != పరిధి_Condition) {
                    తిరిగి s-1;
                }
                ++s;
                కొనసాగు;
            }
        }

        // Give an illegal sequence the same value as the result of contains(FFFD).
        // Handle each byte of an illegal sequence separately to simplify the code;
        // no need to optimize error handling.
        ఒకవేళ(కలిగియుంది_FFFD!=పరిధి_Condition) {
            తిరిగి s-1;
        }
    }

    తిరిగి limit0;
}

/*
 * While going backwards through UTF-8 optimize only for ASCII.
 * Unlike UTF-16, UTF-8 is not forward-backward symmetrical, that is, it is not
 * possible to tell from the last byte in a multi-byte sequence how many
 * preceding bytes there should be. Therefore, going backwards through UTF-8
 * is much harder than going forward.
 */
int32_t
BMPSet::పరిధి_వెనుక_UTF8(స్థిరరాశి uint8_t *s, int32_t length, USet_పరిధి_Condition పరిధి_Condition) స్థిరరాశి {
    ఒకవేళ(పరిధి_Condition!=USET_పరిధి_NOT_CONTAINED) {
        పరిధి_Condition=USET_పరిధి_CONTAINED;  // Pin to 0/1 values.
    }

    uint8_t b;

    చేయి {
        b=s[--length];
        ఒకవేళ(U8_IS_SINGLE(b)) {
            // ASCII sub-span
            ఒకవేళ(పరిధి_Condition) {
                చేయి {
                    ఒకవేళ(!latin1_కలిగియుంది[b]) {
                        తిరిగి length+1;
                    } లేకపోతే ఒకవేళ(length==0) {
                        తిరిగి 0;
                    }
                    b=s[--length];
                } అయితే(U8_IS_SINGLE(b));
            } లేకపోతే {
                చేయి {
                    ఒకవేళ(latin1_కలిగియుంది[b]) {
                        తిరిగి length+1;
                    } లేకపోతే ఒకవేళ(length==0) {
                        తిరిగి 0;
                    }
                    b=s[--length];
                } అయితే(U8_IS_SINGLE(b));
            }
        }

        int32_t prev=length;
        UChar32 c;
        // trail byte: collect a multi-byte character
        // (or  lead byte in last-trail position)
        c=utf8_prev_Char_సురక్షిత_Body(s, 0, &length, b, -3);
        // c is a valid code point, not ASCII, not a surrogate
        ఒకవేళ(c<=0x7ff) {
            ఒకవేళ (static_cast<USet_పరిధి_Condition>((table7FF[c & 0x3f] & (static_cast<uint32_t>(1) << (c >> 6))) != 0) != పరిధి_Condition) {
                తిరిగి prev+1;
            }
        } లేకపోతే ఒకవేళ(c<=0xffff) {
            పూర్ణాంకం lead=c>>12;
            uint32_t two_బిట్లు=(bmp_ఖండం_బిట్లు[(c>>6)&0x3f]>>lead)&0x10001;
            ఒకవేళ(two_బిట్లు<=1) {
                // All 64 code points with the same bits 15..6
                // are either in the set or not.
                ఒకవేళ (two_బిట్లు != static_cast<uint32_t>(పరిధి_Condition)) {
                    తిరిగి prev+1;
                }
            } లేకపోతే {
                // Look up the code point in its 4k block of code points.
                ఒకవేళ(కలిగియుంది_Slow(c, list4kStarts[lead], list4kStarts[lead+1]) != పరిధి_Condition) {
                    తిరిగి prev+1;
                }
            }
        } లేకపోతే {
            ఒకవేళ(కలిగియుంది_Slow(c, list4kStarts[0x10], list4kStarts[0x11]) != పరిధి_Condition) {
                తిరిగి prev+1;
            }
        }
    } అయితే(length>0);
    తిరిగి 0;
}

U_NAMESPACE_చివర
