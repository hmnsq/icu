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

#अन्तर्भाव "unicode/utypes.h"
#अन्तर्भाव "unicode/uniset.h"
#अन्तर्भाव "unicode/utf8.h"
#अन्तर्भाव "unicode/utf16.h"
#अन्तर्भाव "cmemory.h"
#अन्तर्भाव "bmpset.h"
#अन्तर्भाव "uassert.h"

U_NAMESPACE_BEGIN

BMPSet::BMPSet(ध्रुव int32_t *parentList, int32_t parent_List_दीर्घता) :
        list(parentList), list_दीर्घता(parent_List_दीर्घता) {
    uprv_memset(latin1_अन्तर्भवति, 0, परिमाण(latin1_अन्तर्भवति));
    uprv_memset(table7FF, 0, परिमाण(table7FF));
    uprv_memset(bmp_खण्डम्_द्वयङ्काः, 0, परिमाण(bmp_खण्डम्_द्वयङ्काः));

    /*
     * Set the list indexes for binary searches for
     * U+0800, U+1000, U+2000, .., U+F000, U+10000.
     * U+0800 is the first 3-byte-UTF-8 code point. Lower code points are
     * looked up in the bit tables.
     * The last pair of indexes is for finding supplementary code points.
     */
    list4kStarts[0]=find_संहिता_Point(0x800, 0, list_दीर्घता-1);
    int32_t i;
    प्रति(i=1; i<=0x10; ++i) {
        list4kStarts[i]=find_संहिता_Point(i<<12, list4kStarts[i-1], list_दीर्घता-1);
    }
    list4kStarts[0x11]=list_दीर्घता-1;
    अन्तर्भवति_FFFD=अन्तर्भवति_Slow(0xfffd, list4kStarts[0xf], list4kStarts[0x10]);

    init_द्वयङ्काः();
    overrideIllegal();
}

BMPSet::BMPSet(ध्रुव BMPSet &otherBMPSet, ध्रुव int32_t *newParentList, int32_t new_Parent_List_दीर्घता) :
        अन्तर्भवति_FFFD(otherBMPSet.अन्तर्भवति_FFFD),
        list(newParentList), list_दीर्घता(new_Parent_List_दीर्घता) {
    uprv_memcpy(latin1_अन्तर्भवति, otherBMPSet.latin1_अन्तर्भवति, परिमाण(latin1_अन्तर्भवति));
    uprv_memcpy(table7FF, otherBMPSet.table7FF, परिमाण(table7FF));
    uprv_memcpy(bmp_खण्डम्_द्वयङ्काः, otherBMPSet.bmp_खण्डम्_द्वयङ्काः, परिमाण(bmp_खण्डम्_द्वयङ्काः));
    uprv_memcpy(list4kStarts, otherBMPSet.list4kStarts, परिमाण(list4kStarts));
}

BMPSet::~BMPSet() {
}

/*
 * Set bits in a bit rectangle in "vertical" bit organization.
 * start<limit<=0x800
 */
स्थिर शून्य set32x64_द्वयङ्काः(uint32_t सारणी[64], int32_t start, int32_t सीमा) {
    U_ASSERT(start<सीमा);
    U_ASSERT(सीमा<=0x800);

    int32_t lead=start>>6;  // Named for UTF-8 2-byte lead byte with upper 5 bits.
    int32_t trail=start&0x3f;  // Named for UTF-8 2-byte trail byte with lower 6 bits.

    // Set one bit indicating an all-one block.
    uint32_t द्वयङ्काः = static_cast<uint32_t>(1) << lead;
    यदि((start+1)==सीमा) {  // Single-character shortcut.
        सारणी[trail]|=द्वयङ्काः;
        प्रत्यागम;
    }

    int32_t सीमा_Lead=सीमा>>6;
    int32_t सीमा_Trail=सीमा&0x3f;

    यदि(lead==सीमा_Lead) {
        // Partial vertical bit column.
        यावत्(trail<सीमा_Trail) {
            सारणी[trail++]|=द्वयङ्काः;
        }
    } अन्यथा {
        // Partial vertical bit column,
        // followed by a bit rectangle,
        // followed by another partial vertical bit column.
        यदि(trail>0) {
            कुरु {
                सारणी[trail++]|=द्वयङ्काः;
            } यावत्(trail<64);
            ++lead;
        }
        यदि(lead<सीमा_Lead) {
            द्वयङ्काः = ~((static_cast<अचिह्नित>(1) << lead) - 1);
            यदि(सीमा_Lead<0x20) {
                द्वयङ्काः &= (static_cast<अचिह्नित>(1) << सीमा_Lead) - 1;
            }
            प्रति(trail=0; trail<64; ++trail) {
                सारणी[trail]|=द्वयङ्काः;
            }
        }
        // limit<=0x800. If limit==0x800 then limitLead=32 and limitTrail=0.
        // In that case, bits=1<<limitLead is undefined but the bits value
        // is not used because trail<limitTrail is already false.
        द्वयङ्काः = static_cast<uint32_t>(1) << ((सीमा_Lead == 0x20) ? (सीमा_Lead - 1) : सीमा_Lead);
        प्रति(trail=0; trail<सीमा_Trail; ++trail) {
            सारणी[trail]|=द्वयङ्काः;
        }
    }
}

शून्य BMPSet::init_द्वयङ्काः() {
    UChar32 start, सीमा;
    int32_t list_सूचकांक=0;

    // Set latin1Contains[].
    कुरु {
        start=list[list_सूचकांक++];
        यदि(list_सूचकांक<list_दीर्घता) {
            सीमा=list[list_सूचकांक++];
        } अन्यथा {
            सीमा=0x110000;
        }
        यदि(start>=0x100) {
            भङ्ग;
        }
        कुरु {
            latin1_अन्तर्भवति[start++]=1;
        } यावत्(start<सीमा && start<0x100);
    } यावत्(सीमा<=0x100);

    // Find the first range overlapping with (or after) 80..FF again,
    // to include them in table7FF as well.
    प्रति(list_सूचकांक=0;;) {
        start=list[list_सूचकांक++];
        यदि(list_सूचकांक<list_दीर्घता) {
            सीमा=list[list_सूचकांक++];
        } अन्यथा {
            सीमा=0x110000;
        }
        यदि(सीमा>0x80) {
            यदि(start<0x80) {
                start=0x80;
            }
            भङ्ग;
        }
    }

    // Set table7FF[].
    यावत्(start<0x800) {
        set32x64_द्वयङ्काः(table7FF, start, सीमा<=0x800 ? सीमा : 0x800);
        यदि(सीमा>0x800) {
            start=0x800;
            भङ्ग;
        }

        start=list[list_सूचकांक++];
        यदि(list_सूचकांक<list_दीर्घता) {
            सीमा=list[list_सूचकांक++];
        } अन्यथा {
            सीमा=0x110000;
        }
    }

    // Set bmpBlockBits[].
    int32_t minStart=0x800;
    यावत्(start<0x10000) {
        यदि(सीमा>0x10000) {
            सीमा=0x10000;
        }

        यदि(start<minStart) {
            start=minStart;
        }
        यदि(start<सीमा) {  // Else: Another range entirely in a known mixed-value block.
            यदि(start&0x3f) {
                // Mixed-value block of 64 code points.
                start>>=6;
                bmp_खण्डम्_द्वयङ्काः[start&0x3f]|=0x10001<<(start>>6);
                start=(start+1)<<6;  // Round up to the next block boundary.
                minStart=start;      // Ignore further ranges in this block.
            }
            यदि(start<सीमा) {
                यदि(start<(सीमा&~0x3f)) {
                    // Multiple all-ones blocks of 64 code points each.
                    set32x64_द्वयङ्काः(bmp_खण्डम्_द्वयङ्काः, start>>6, सीमा>>6);
                }

                यदि(सीमा&0x3f) {
                    // Mixed-value block of 64 code points.
                    सीमा>>=6;
                    bmp_खण्डम्_द्वयङ्काः[सीमा&0x3f]|=0x10001<<(सीमा>>6);
                    सीमा=(सीमा+1)<<6;  // Round up to the next block boundary.
                    minStart=सीमा;      // Ignore further ranges in this block.
                }
            }
        }

        यदि(सीमा==0x10000) {
            भङ्ग;
        }

        start=list[list_सूचकांक++];
        यदि(list_सूचकांक<list_दीर्घता) {
            सीमा=list[list_सूचकांक++];
        } अन्यथा {
            सीमा=0x110000;
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
शून्य BMPSet::overrideIllegal() {
    uint32_t द्वयङ्काः, mask;
    int32_t i;

    यदि(अन्तर्भवति_FFFD) {
        द्वयङ्काः=3;                 // Lead bytes 0xC0 and 0xC1.
        प्रति(i=0; i<64; ++i) {
            table7FF[i]|=द्वयङ्काः;
        }

        द्वयङ्काः=1;                 // Lead byte 0xE0.
        प्रति(i=0; i<32; ++i) {   // First half of 4k block.
            bmp_खण्डम्_द्वयङ्काः[i]|=द्वयङ्काः;
        }

        mask= static_cast<uint32_t>(~(0x10001<<0xd));   // Lead byte 0xED.
        द्वयङ्काः=1<<0xd;
        प्रति(i=32; i<64; ++i) {  // Second half of 4k block.
            bmp_खण्डम्_द्वयङ्काः[i]=(bmp_खण्डम्_द्वयङ्काः[i]&mask)|द्वयङ्काः;
        }
    } अन्यथा {
        mask= static_cast<uint32_t>(~(0x10001<<0xd));   // Lead byte 0xED.
        प्रति(i=32; i<64; ++i) {  // Second half of 4k block.
            bmp_खण्डम्_द्वयङ्काः[i]&=mask;
        }
    }
}

int32_t BMPSet::find_संहिता_Point(UChar32 c, int32_t lo, int32_t hi) ध्रुव {
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
    यदि (c < list[lo])
        प्रत्यागम lo;
    // High runner test.  c is often after the last range, so an
    // initial check for this condition pays off.
    यदि (lo >= hi || c >= list[hi-1])
        प्रत्यागम hi;
    // invariant: c >= list[lo]
    // invariant: c < list[hi]
    प्रति (;;) {
        int32_t i = (lo + hi) >> 1;
        यदि (i == lo) {
            भङ्ग; // Found!
        } अन्यथा यदि (c < list[i]) {
            hi = i;
        } अन्यथा {
            lo = i;
        }
    }
    प्रत्यागम hi;
}

UBool
BMPSet::अन्तर्भवति(UChar32 c) ध्रुव {
    यदि (static_cast<uint32_t>(c) <= 0xff) {
        प्रत्यागम latin1_अन्तर्भवति[c];
    } अन्यथा यदि (static_cast<uint32_t>(c) <= 0x7ff) {
        प्रत्यागम (table7FF[c & 0x3f] & (static_cast<uint32_t>(1) << (c >> 6))) != 0;
    } अन्यथा यदि (static_cast<uint32_t>(c) < 0xd800 || (c >= 0xe000 && c <= 0xffff)) {
        पूर्णाङ्क lead=c>>12;
        uint32_t two_द्वयङ्काः=(bmp_खण्डम्_द्वयङ्काः[(c>>6)&0x3f]>>lead)&0x10001;
        यदि(two_द्वयङ्काः<=1) {
            // All 64 code points with the same bits 15..6
            // are either in the set or not.
            प्रत्यागम two_द्वयङ्काः;
        } अन्यथा {
            // Look up the code point in its 4k block of code points.
            प्रत्यागम अन्तर्भवति_Slow(c, list4kStarts[lead], list4kStarts[lead+1]);
        }
    } अन्यथा यदि (static_cast<uint32_t>(c) <= 0x10ffff) {
        // surrogate or supplementary code point
        प्रत्यागम अन्तर्भवति_Slow(c, list4kStarts[0xd], list4kStarts[0x11]);
    } अन्यथा {
        // Out-of-range code points get false, consistent with long-standing
        // behavior of UnicodeSet::contains(c).
        प्रत्यागम असत्य;
    }
}

/*
 * Check for sufficient length for trail unit for each surrogate pair.
 * Handle single surrogates as surrogate code points as usual in ICU.
 */
ध्रुव char16_t *
BMPSet::विस्तृतिः(ध्रुव char16_t *s, ध्रुव char16_t *सीमा, USet_विस्तृतिः_Condition विस्तृतिः_Condition) ध्रुव {
    char16_t c, c2;

    यदि(विस्तृतिः_Condition) {
        //विस्तृतिः
        कुरु {
            c=*s;
            यदि(c<=0xff) {
                यदि(!latin1_अन्तर्भवति[c]) {
                    भङ्ग;
                }
            } अन्यथा यदि(c<=0x7ff) {
                यदि ((table7FF[c & 0x3f] & (static_cast<uint32_t>(1) << (c >> 6))) == 0) {
                    भङ्ग;
                }
            } अन्यथा यदि(c<0xd800 || c>=0xe000) {
                पूर्णाङ्क lead=c>>12;
                uint32_t two_द्वयङ्काः=(bmp_खण्डम्_द्वयङ्काः[(c>>6)&0x3f]>>lead)&0x10001;
                यदि(two_द्वयङ्काः<=1) {
                    // All 64 code points with the same bits 15..6
                    // are either in the set or not.
                    यदि(two_द्वयङ्काः==0) {
                        भङ्ग;
                    }
                } अन्यथा {
                    // Look up the code point in its 4k block of code points.
                    यदि(!अन्तर्भवति_Slow(c, list4kStarts[lead], list4kStarts[lead+1])) {
                        भङ्ग;
                    }
                }
            } अन्यथा यदि(c>=0xdc00 || (s+1)==सीमा || (c2=s[1])<0xdc00 || c2>=0xe000) {
                // surrogate code point
                यदि(!अन्तर्भवति_Slow(c, list4kStarts[0xd], list4kStarts[0xe])) {
                    भङ्ग;
                }
            } अन्यथा {
                // surrogate pair
                यदि(!अन्तर्भवति_Slow(U16_GET_SUPPLEMENTARY(c, c2), list4kStarts[0x10], list4kStarts[0x11])) {
                    भङ्ग;
                }
                ++s;
            }
        } यावत्(++s<सीमा);
    } अन्यथा {
        // span not
        कुरु {
            c=*s;
            यदि(c<=0xff) {
                यदि(latin1_अन्तर्भवति[c]) {
                    भङ्ग;
                }
            } अन्यथा यदि(c<=0x7ff) {
                यदि ((table7FF[c & 0x3f] & (static_cast<uint32_t>(1) << (c >> 6))) != 0) {
                    भङ्ग;
                }
            } अन्यथा यदि(c<0xd800 || c>=0xe000) {
                पूर्णाङ्क lead=c>>12;
                uint32_t two_द्वयङ्काः=(bmp_खण्डम्_द्वयङ्काः[(c>>6)&0x3f]>>lead)&0x10001;
                यदि(two_द्वयङ्काः<=1) {
                    // All 64 code points with the same bits 15..6
                    // are either in the set or not.
                    यदि(two_द्वयङ्काः!=0) {
                        भङ्ग;
                    }
                } अन्यथा {
                    // Look up the code point in its 4k block of code points.
                    यदि(अन्तर्भवति_Slow(c, list4kStarts[lead], list4kStarts[lead+1])) {
                        भङ्ग;
                    }
                }
            } अन्यथा यदि(c>=0xdc00 || (s+1)==सीमा || (c2=s[1])<0xdc00 || c2>=0xe000) {
                // surrogate code point
                यदि(अन्तर्भवति_Slow(c, list4kStarts[0xd], list4kStarts[0xe])) {
                    भङ्ग;
                }
            } अन्यथा {
                // surrogate pair
                यदि(अन्तर्भवति_Slow(U16_GET_SUPPLEMENTARY(c, c2), list4kStarts[0x10], list4kStarts[0x11])) {
                    भङ्ग;
                }
                ++s;
            }
        } यावत्(++s<सीमा);
    }
    प्रत्यागम s;
}

/* Symmetrical with span(). */
ध्रुव char16_t *
BMPSet::विस्तृतिः_पृष्ठभागः(ध्रुव char16_t *s, ध्रुव char16_t *सीमा, USet_विस्तृतिः_Condition विस्तृतिः_Condition) ध्रुव {
    char16_t c, c2;

    यदि(विस्तृतिः_Condition) {
        //विस्तृतिः
        प्रति(;;) {
            c=*(--सीमा);
            यदि(c<=0xff) {
                यदि(!latin1_अन्तर्भवति[c]) {
                    भङ्ग;
                }
            } अन्यथा यदि(c<=0x7ff) {
                यदि ((table7FF[c & 0x3f] & (static_cast<uint32_t>(1) << (c >> 6))) == 0) {
                    भङ्ग;
                }
            } अन्यथा यदि(c<0xd800 || c>=0xe000) {
                पूर्णाङ्क lead=c>>12;
                uint32_t two_द्वयङ्काः=(bmp_खण्डम्_द्वयङ्काः[(c>>6)&0x3f]>>lead)&0x10001;
                यदि(two_द्वयङ्काः<=1) {
                    // All 64 code points with the same bits 15..6
                    // are either in the set or not.
                    यदि(two_द्वयङ्काः==0) {
                        भङ्ग;
                    }
                } अन्यथा {
                    // Look up the code point in its 4k block of code points.
                    यदि(!अन्तर्भवति_Slow(c, list4kStarts[lead], list4kStarts[lead+1])) {
                        भङ्ग;
                    }
                }
            } अन्यथा यदि(c<0xdc00 || s==सीमा || (c2=*(सीमा-1))<0xd800 || c2>=0xdc00) {
                // surrogate code point
                यदि(!अन्तर्भवति_Slow(c, list4kStarts[0xd], list4kStarts[0xe])) {
                    भङ्ग;
                }
            } अन्यथा {
                // surrogate pair
                यदि(!अन्तर्भवति_Slow(U16_GET_SUPPLEMENTARY(c2, c), list4kStarts[0x10], list4kStarts[0x11])) {
                    भङ्ग;
                }
                --सीमा;
            }
            यदि(s==सीमा) {
                प्रत्यागम s;
            }
        }
    } अन्यथा {
        // span not
        प्रति(;;) {
            c=*(--सीमा);
            यदि(c<=0xff) {
                यदि(latin1_अन्तर्भवति[c]) {
                    भङ्ग;
                }
            } अन्यथा यदि(c<=0x7ff) {
                यदि ((table7FF[c & 0x3f] & (static_cast<uint32_t>(1) << (c >> 6))) != 0) {
                    भङ्ग;
                }
            } अन्यथा यदि(c<0xd800 || c>=0xe000) {
                पूर्णाङ्क lead=c>>12;
                uint32_t two_द्वयङ्काः=(bmp_खण्डम्_द्वयङ्काः[(c>>6)&0x3f]>>lead)&0x10001;
                यदि(two_द्वयङ्काः<=1) {
                    // All 64 code points with the same bits 15..6
                    // are either in the set or not.
                    यदि(two_द्वयङ्काः!=0) {
                        भङ्ग;
                    }
                } अन्यथा {
                    // Look up the code point in its 4k block of code points.
                    यदि(अन्तर्भवति_Slow(c, list4kStarts[lead], list4kStarts[lead+1])) {
                        भङ्ग;
                    }
                }
            } अन्यथा यदि(c<0xdc00 || s==सीमा || (c2=*(सीमा-1))<0xd800 || c2>=0xdc00) {
                // surrogate code point
                यदि(अन्तर्भवति_Slow(c, list4kStarts[0xd], list4kStarts[0xe])) {
                    भङ्ग;
                }
            } अन्यथा {
                // surrogate pair
                यदि(अन्तर्भवति_Slow(U16_GET_SUPPLEMENTARY(c2, c), list4kStarts[0x10], list4kStarts[0x11])) {
                    भङ्ग;
                }
                --सीमा;
            }
            यदि(s==सीमा) {
                प्रत्यागम s;
            }
        }
    }
    प्रत्यागम सीमा+1;
}

/*
 * Precheck for sufficient trail bytes at end of string only once per span.
 * Check validity.
 */
ध्रुव uint8_t *
BMPSet::विस्तृतिः_UTF8(ध्रुव uint8_t *s, int32_t length, USet_विस्तृतिः_Condition विस्तृतिः_Condition) ध्रुव {
    ध्रुव uint8_t *सीमा=s+length;
    uint8_t b=*s;
    यदि(U8_IS_SINGLE(b)) {
        // Initial all-ASCII span.
        यदि(विस्तृतिः_Condition) {
            कुरु {
                यदि(!latin1_अन्तर्भवति[b] || ++s==सीमा) {
                    प्रत्यागम s;
                }
                b=*s;
            } यावत्(U8_IS_SINGLE(b));
        } अन्यथा {
            कुरु {
                यदि(latin1_अन्तर्भवति[b] || ++s==सीमा) {
                    प्रत्यागम s;
                }
                b=*s;
            } यावत्(U8_IS_SINGLE(b));
        }
        length = static_cast<int32_t>(सीमा - s);
    }

    यदि(विस्तृतिः_Condition!=USET_विस्तृतिः_NOT_CONTAINED) {
        विस्तृतिः_Condition=USET_विस्तृतिः_CONTAINED;  // Pin to 0/1 values.
    }

    ध्रुव uint8_t *limit0=सीमा;

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
    b=*(सीमा-1);
    यदि (static_cast<int8_t>(b) < 0) {
        // b>=0x80: lead or trail byte
        यदि(b<0xc0) {
            // single trail byte, check for preceding 3- or 4-byte lead byte
            यदि(length>=2 && (b=*(सीमा-2))>=0xe0) {
                सीमा-=2;
                यदि(अन्तर्भवति_FFFD!=विस्तृतिः_Condition) {
                    limit0=सीमा;
                }
            } अन्यथा यदि(b<0xc0 && b>=0x80 && length>=3 && (b=*(सीमा-3))>=0xf0) {
                // 4-byte lead byte with only two trail bytes
                सीमा-=3;
                यदि(अन्तर्भवति_FFFD!=विस्तृतिः_Condition) {
                    limit0=सीमा;
                }
            }
        } अन्यथा {
            // lead byte with no trail bytes
            --सीमा;
            यदि(अन्तर्भवति_FFFD!=विस्तृतिः_Condition) {
                limit0=सीमा;
            }
        }
    }

    uint8_t t1, t2, t3;

    यावत्(s<सीमा) {
        b=*s;
        यदि(U8_IS_SINGLE(b)) {
            // ASCII
            यदि(विस्तृतिः_Condition) {
                कुरु {
                    यदि(!latin1_अन्तर्भवति[b]) {
                        प्रत्यागम s;
                    } अन्यथा यदि(++s==सीमा) {
                        प्रत्यागम limit0;
                    }
                    b=*s;
                } यावत्(U8_IS_SINGLE(b));
            } अन्यथा {
                कुरु {
                    यदि(latin1_अन्तर्भवति[b]) {
                        प्रत्यागम s;
                    } अन्यथा यदि(++s==सीमा) {
                        प्रत्यागम limit0;
                    }
                    b=*s;
                } यावत्(U8_IS_SINGLE(b));
            }
        }
        ++s;  // Advance past the lead byte.
        यदि(b>=0xe0) {
            यदि(b<0xf0) {
                यदि( /* handle U+0000..U+FFFF inline */
                    (t1 = static_cast<uint8_t>(s[0] - 0x80)) <= 0x3f &&
                    (t2 = static_cast<uint8_t>(s[1] - 0x80)) <= 0x3f
                ) {
                    b&=0xf;
                    uint32_t two_द्वयङ्काः=(bmp_खण्डम्_द्वयङ्काः[t1]>>b)&0x10001;
                    यदि(two_द्वयङ्काः<=1) {
                        // All 64 code points with this lead byte and middle trail byte
                        // are either in the set or not.
                        यदि (two_द्वयङ्काः != static_cast<uint32_t>(विस्तृतिः_Condition)) {
                            प्रत्यागम s-1;
                        }
                    } अन्यथा {
                        // Look up the code point in its 4k block of code points.
                        UChar32 c=(b<<12)|(t1<<6)|t2;
                        यदि(अन्तर्भवति_Slow(c, list4kStarts[b], list4kStarts[b+1]) != विस्तृतिः_Condition) {
                            प्रत्यागम s-1;
                        }
                    }
                    s+=2;
                    अनुवर्त;
                }
            } अन्यथा यदि( /* handle U+10000..U+10FFFF inline */
                (t1 = static_cast<uint8_t>(s[0] - 0x80)) <= 0x3f &&
                (t2 = static_cast<uint8_t>(s[1] - 0x80)) <= 0x3f &&
                (t3 = static_cast<uint8_t>(s[2] - 0x80)) <= 0x3f
            ) {
                // Give an illegal sequence the same value as the result of contains(FFFD).
                UChar32 c = (static_cast<UChar32>(b - 0xf0) << 18) | (static_cast<UChar32>(t1) << 12) | (t2 << 6) | t3;
                यदि( (   (0x10000<=c && c<=0x10ffff) ?
                            अन्तर्भवति_Slow(c, list4kStarts[0x10], list4kStarts[0x11]) :
                            अन्तर्भवति_FFFD
                    ) != विस्तृतिः_Condition
                ) {
                    प्रत्यागम s-1;
                }
                s+=3;
                अनुवर्त;
            }
        } अन्यथा {
            यदि( /* handle U+0000..U+07FF inline */
                b>=0xc0 &&
                (t1 = static_cast<uint8_t>(*s - 0x80)) <= 0x3f
            ) {
                यदि (static_cast<USet_विस्तृतिः_Condition>((table7FF[t1] & (static_cast<uint32_t>(1) << (b & 0x1f))) != 0) != विस्तृतिः_Condition) {
                    प्रत्यागम s-1;
                }
                ++s;
                अनुवर्त;
            }
        }

        // Give an illegal sequence the same value as the result of contains(FFFD).
        // Handle each byte of an illegal sequence separately to simplify the code;
        // no need to optimize error handling.
        यदि(अन्तर्भवति_FFFD!=विस्तृतिः_Condition) {
            प्रत्यागम s-1;
        }
    }

    प्रत्यागम limit0;
}

/*
 * While going backwards through UTF-8 optimize only for ASCII.
 * Unlike UTF-16, UTF-8 is not forward-backward symmetrical, that is, it is not
 * possible to tell from the last byte in a multi-byte sequence how many
 * preceding bytes there should be. Therefore, going backwards through UTF-8
 * is much harder than going forward.
 */
int32_t
BMPSet::विस्तृतिः_पृष्ठभागः_UTF8(ध्रुव uint8_t *s, int32_t length, USet_विस्तृतिः_Condition विस्तृतिः_Condition) ध्रुव {
    यदि(विस्तृतिः_Condition!=USET_विस्तृतिः_NOT_CONTAINED) {
        विस्तृतिः_Condition=USET_विस्तृतिः_CONTAINED;  // Pin to 0/1 values.
    }

    uint8_t b;

    कुरु {
        b=s[--length];
        यदि(U8_IS_SINGLE(b)) {
            // ASCII sub-span
            यदि(विस्तृतिः_Condition) {
                कुरु {
                    यदि(!latin1_अन्तर्भवति[b]) {
                        प्रत्यागम length+1;
                    } अन्यथा यदि(length==0) {
                        प्रत्यागम 0;
                    }
                    b=s[--length];
                } यावत्(U8_IS_SINGLE(b));
            } अन्यथा {
                कुरु {
                    यदि(latin1_अन्तर्भवति[b]) {
                        प्रत्यागम length+1;
                    } अन्यथा यदि(length==0) {
                        प्रत्यागम 0;
                    }
                    b=s[--length];
                } यावत्(U8_IS_SINGLE(b));
            }
        }

        int32_t पूर्व=length;
        UChar32 c;
        // trail byte: collect a multi-byte character
        // (or  lead byte in last-trail position)
        c=utf8_पूर्व_Char_Safe_Body(s, 0, &length, b, -3);
        // c is a valid code point, not ASCII, not a surrogate
        यदि(c<=0x7ff) {
            यदि (static_cast<USet_विस्तृतिः_Condition>((table7FF[c & 0x3f] & (static_cast<uint32_t>(1) << (c >> 6))) != 0) != विस्तृतिः_Condition) {
                प्रत्यागम पूर्व+1;
            }
        } अन्यथा यदि(c<=0xffff) {
            पूर्णाङ्क lead=c>>12;
            uint32_t two_द्वयङ्काः=(bmp_खण्डम्_द्वयङ्काः[(c>>6)&0x3f]>>lead)&0x10001;
            यदि(two_द्वयङ्काः<=1) {
                // All 64 code points with the same bits 15..6
                // are either in the set or not.
                यदि (two_द्वयङ्काः != static_cast<uint32_t>(विस्तृतिः_Condition)) {
                    प्रत्यागम पूर्व+1;
                }
            } अन्यथा {
                // Look up the code point in its 4k block of code points.
                यदि(अन्तर्भवति_Slow(c, list4kStarts[lead], list4kStarts[lead+1]) != विस्तृतिः_Condition) {
                    प्रत्यागम पूर्व+1;
                }
            }
        } अन्यथा {
            यदि(अन्तर्भवति_Slow(c, list4kStarts[0x10], list4kStarts[0x11]) != विस्तृतिः_Condition) {
                प्रत्यागम पूर्व+1;
            }
        }
    } यावत्(length>0);
    प्रत्यागम 0;
}

U_NAMESPACE_अन्तः
