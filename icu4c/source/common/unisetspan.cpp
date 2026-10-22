// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
******************************************************************************
*
*   Copyright (C) 2007-2012, International Business Machines
*   Corporation and others.  All Rights Reserved.
*
******************************************************************************
*   file name:  unisetspan.cpp
*   encoding:   UTF-8
*   tab size:   8 (not used)
*   indentation:4
*
*   created on: 2007mar01
*   created by: Markus W. Scherer
*/

#ଅନ୍ତର୍ଭାଵ "unicode/utypes.h"
#ଅନ୍ତର୍ଭାଵ "unicode/uniset.h"
#ଅନ୍ତର୍ଭାଵ "unicode/ustring.h"
#ଅନ୍ତର୍ଭାଵ "unicode/utf8.h"
#ଅନ୍ତର୍ଭାଵ "unicode/utf16.h"
#ଅନ୍ତର୍ଭାଵ "cmemory.h"
#ଅନ୍ତର୍ଭାଵ "uvector.h"
#ଅନ୍ତର୍ଭାଵ "unisetspan.h"

U_NAMESPACE_BEGIN

/*
 * List of offsets from the current position from where to try matching
 * a code point or a string.
 * Store offsets rather than indexes to simplify the code and use the same list
 * for both increments (in span()) and decrements (in spanBack()).
 *
 * Assumption: The maximum offset is limited, and the offsets that are stored
 * at any one time are relatively dense, that is, there are normally no gaps of
 * hundreds or thousands of offset values.
 *
 * The implementation uses a circular buffer of byte flags,
 * each indicating whether the corresponding offset is in the list.
 * This avoids inserting into a sorted list of offsets (or absolute indexes) and
 * physically moving part of the list.
 *
 * Note: In principle, the caller should setMaxLength() to the maximum of the
 * max string length and U16_LENGTH/U8_LENGTH to account for
 * "long" single code points.
 * However, this implementation uses at least a staticList with more than
 * U8_LENGTH entries anyway.
 *
 * Note: If maxLength were guaranteed to be no more than 32 or 64,
 * the list could be stored as bit flags in a single integer.
 * Rather than handling a circular buffer with a start list index,
 * the integer would simply be shifted when lower offsets are removed.
 * UnicodeSet does not have a limit on the lengths of strings.
 */
ଶ୍ରେଣୀ OffsetList {  // Only ever stack-allocated, does not need to inherit UMemory.
ସର୍ବସାଧାରଣ:
    OffsetList() : list(staticList), capacity(0), length(0), start(0) {}

    ~OffsetList() {
        ଯଦି(list!=staticList) {
            uprv_free(list);
        }
    }

    // Call exactly once if the list is to be used.
    ଶୂନ୍ୟ setMaxLength(int32_t maxLength) {
        ଯଦି (maxLength <= static_cast<int32_t>(ଆକାର(staticList))) {
            capacity = static_cast<int32_t>(ଆକାର(staticList));
        } ଅନ୍ୟଥା {
            UBool* l = static_cast<UBool*>(uprv_malloc(maxLength));
            ଯଦି(l!=nullptr) {
                list=l;
                capacity=maxLength;
            }
        }
        uprv_memset(list, 0, capacity);
    }

    ଶୂନ୍ୟ clear() {
        uprv_memset(list, 0, capacity);
        start=length=0;
    }

    UBool isEmpty() ଅଚଳ {
        ଫେରନ୍ତୁ length == 0;
    }

    // Reduce all stored offsets by delta, used when the current position
    // moves by delta.
    // There must not be any offsets lower than delta.
    // If there is an offset equal to delta, it is removed.
    // delta=[1..maxLength]
    ଶୂନ୍ୟ shift(int32_t delta) {
        int32_t i=start+delta;
        ଯଦି(i>=capacity) {
            i-=capacity;
        }
        ଯଦି(list[i]) {
            list[i]=ମିଥ୍ୟା;
            --length;
        }
        start=i;
    }

    // Add an offset. The list must not contain it yet.
    // offset=[1..maxLength]
    ଶୂନ୍ୟ addOffset(int32_t offset) {
        int32_t i=start+offset;
        ଯଦି(i>=capacity) {
            i-=capacity;
        }
        list[i]=ସତ୍ୟ;
        ++length;
    }

    // offset=[1..maxLength]
    UBool containsOffset(int32_t offset) ଅଚଳ {
        int32_t i=start+offset;
        ଯଦି(i>=capacity) {
            i-=capacity;
        }
        ଫେରନ୍ତୁ list[i];
    }

    // Find the lowest stored offset from a non-empty list, remove it,
    // and reduce all other offsets by this minimum.
    // Returns [1..maxLength].
    int32_t କାଢ଼_Minimum() {
        // Look for the next offset in list[start+1..capacity-1].
        int32_t i=start, result;
        ଯେତେବେଳେ(++i<capacity) {
            ଯଦି(list[i]) {
                list[i]=ମିଥ୍ୟା;
                --length;
                result=i-start;
                start=i;
                ଫେରନ୍ତୁ result;
            }
        }
        // i==capacity

        // Wrap around and look for the next offset in list[0..start].
        // Since the list is not empty, there will be one.
        result=capacity-start;
        i=0;
        ଯେତେବେଳେ(!list[i]) {
            ++i;
        }
        list[i]=ମିଥ୍ୟା;
        --length;
        start=i;
        ଫେରନ୍ତୁ result+=i;
    }

ବ୍ୟକ୍ତିଗତ:
    UBool *list;
    int32_t capacity;
    int32_t length;
    int32_t start;

    UBool staticList[16];
};

// Get the number of UTF-8 bytes for a UTF-16 (sub)string.
ସ୍ଥିର int32_t
getUTF8Length(ଅଚଳ char16_t *s, int32_t length) {
    UErrorCode errorCode=U_ZERO_ERROR;
    int32_t length8=0;
    u_strToUTF8(nullptr, 0, &length8, s, length, &errorCode);
    ଯଦି(U_SUCCESS(errorCode) || errorCode==U_BUFFER_OVERFLOW_ERROR) {
        ଫେରନ୍ତୁ length8;
    } ଅନ୍ୟଥା {
        // The string contains an unpaired surrogate.
        // Ignore this string.
        ଫେରନ୍ତୁ 0;
    }
}

// Append the UTF-8 version of the string to t and return the appended UTF-8 length.
ସ୍ଥିର int32_t
appendUTF8(ଅଚଳ char16_t *s, int32_t length, uint8_t *t, int32_t capacity) {
    UErrorCode errorCode=U_ZERO_ERROR;
    int32_t length8=0;
    u_strToUTF8(reinterpret_cast<ଅକ୍ଷର*>(t), capacity, &length8, s, length, &errorCode);
    ଯଦି(U_SUCCESS(errorCode)) {
        ଫେରନ୍ତୁ length8;
    } ଅନ୍ୟଥା {
        // The string contains an unpaired surrogate.
        // Ignore this string.
        ଫେରନ୍ତୁ 0;
    }
}

ସ୍ଥିର ଧାଡ଼ିଭିତରେ uint8_t
makeSpanLengthByte(int32_t spanLength) {
    // 0xfe==UnicodeSetStringSpan::LONG_SPAN
    ଫେରନ୍ତୁ spanLength < 0xfe ? static_cast<uint8_t>(spanLength) : static_cast<uint8_t>(0xfe);
}

// Construct for all variants of span(), or only for any one variant.
// Initialize as little as possible, for single use.
UnicodeSetStringSpan::UnicodeSetStringSpan(ଅଚଳ UnicodeSet &set,
                                           ଅଚଳ UVector &setStrings,
                                           uint32_t which)
        : spanSet(0, 0x10ffff), pSpanNotSet(nullptr), strings(setStrings),
          utf8Lengths(nullptr), spanLengths(nullptr), utf8(nullptr),
          utf8Length(0),
          maxLength16(0), maxLength8(0),
          all(static_cast<UBool>(which == ALL)) {
    spanSet.retainAll(set);
    ଯଦି(which&NOT_CONTAINED) {
        // Default to the same sets.
        // addToSpanNotSet() will create a separate set if necessary.
        pSpanNotSet=&spanSet;
    }

    // Determine if the strings even need to be taken into account at all for span() etc.
    // If any string is relevant, then all strings need to be used for
    // span(longest match) but only the relevant ones for span(while contained).
    // TODO: Possible optimization: Distinguish CONTAINED vs. LONGEST_MATCH
    //   and do not store UTF-8 strings if !thisRelevant and CONTAINED.
    //   (Only store irrelevant UTF-8 strings for LONGEST_MATCH where they are relevant after all.)
    // Also count the lengths of the UTF-8 versions of the strings for memory allocation.
    int32_t stringsLength=strings.size();

    int32_t i, spanLength;
    UBool someRelevant=ମିଥ୍ୟା;
    ପାଇଁ(i=0; i<stringsLength; ++i) {
        ଅଚଳ UnicodeString& string = *static_cast<ଅଚଳ UnicodeString*>(strings.elementAt(i));
        ଅଚଳ char16_t *s16=string.getBuffer();
        int32_t length16=string.length();
        ଯଦି (length16==0) {
            ଜାରି;  // skip the empty string
        }
        UBool thisRelevant;
        spanLength=spanSet.span(s16, length16, USET_SPAN_CONTAINED);
        ଯଦି(spanLength<length16) {  // Relevant string.
            someRelevant=thisRelevant=ସତ୍ୟ;
        } ଅନ୍ୟଥା {
            thisRelevant=ମିଥ୍ୟା;
        }
        ଯଦି((which&UTF16) && length16>maxLength16) {
            maxLength16=length16;
        }
        ଯଦି((which&UTF8) && (thisRelevant || (which&CONTAINED))) {
            int32_t length8=getUTF8Length(s16, length16);
            utf8Length+=length8;
            ଯଦି(length8>maxLength8) {
                maxLength8=length8;
            }
        }
    }
    ଯଦି(!someRelevant) {
        maxLength16=maxLength8=0;
        ଫେରନ୍ତୁ;
    }

    // Freeze after checking for the need to use strings at all because freezing
    // a set takes some time and memory which are wasted if there are no relevant strings.
    ଯଦି(all) {
        spanSet.freeze();
    }

    uint8_t *spanBackLengths;
    uint8_t *spanUTF8Lengths;
    uint8_t *spanBackUTF8Lengths;

    // Allocate a block of meta data.
    int32_t allocSize;
    ଯଦି(all) {
        // UTF-8 lengths, 4 sets of span lengths, UTF-8 strings.
        allocSize=stringsLength*(4+1+1+1+1)+utf8Length;
    } ଅନ୍ୟଥା {
        allocSize=stringsLength;  // One set of span lengths.
        ଯଦି(which&UTF8) {
            // UTF-8 lengths and UTF-8 strings.
            allocSize+=stringsLength*4+utf8Length;
        }
    }
    ଯଦି (allocSize <= static_cast<int32_t>(ଆକାର(staticLengths))) {
        utf8Lengths=staticLengths;
    } ଅନ୍ୟଥା {
        utf8Lengths = static_cast<int32_t*>(uprv_malloc(allocSize));
        ଯଦି(utf8Lengths==nullptr) {
            maxLength16=maxLength8=0;  // Prevent usage by making needsStringSpanUTF16/8() return false.
            ଫେରନ୍ତୁ;  // Out of memory.
        }
    }

    ଯଦି(all) {
        // Store span lengths for all span() variants.
        spanLengths = reinterpret_cast<uint8_t*>(utf8Lengths + stringsLength);
        spanBackLengths=spanLengths+stringsLength;
        spanUTF8Lengths=spanBackLengths+stringsLength;
        spanBackUTF8Lengths=spanUTF8Lengths+stringsLength;
        utf8=spanBackUTF8Lengths+stringsLength;
    } ଅନ୍ୟଥା {
        // Store span lengths for only one span() variant.
        ଯଦି(which&UTF8) {
            spanLengths = reinterpret_cast<uint8_t*>(utf8Lengths + stringsLength);
            utf8=spanLengths+stringsLength;
        } ଅନ୍ୟଥା {
            spanLengths = reinterpret_cast<uint8_t*>(utf8Lengths);
        }
        spanBackLengths=spanUTF8Lengths=spanBackUTF8Lengths=spanLengths;
    }

    // Set the meta data and pSpanNotSet and write the UTF-8 strings.
    int32_t utf8_ଗଣନା=0;  // Count UTF-8 bytes written so far.

    ପାଇଁ(i=0; i<stringsLength; ++i) {
        ଅଚଳ UnicodeString& string = *static_cast<ଅଚଳ UnicodeString*>(strings.elementAt(i));
        ଅଚଳ char16_t *s16=string.getBuffer();
        int32_t length16=string.length();
        spanLength=spanSet.span(s16, length16, USET_SPAN_CONTAINED);
        ଯଦି(spanLength<length16 && length16>0) {  // Relevant string.
            ଯଦି(which&UTF16) {
                ଯଦି(which&CONTAINED) {
                    ଯଦି(which&FWD) {
                        spanLengths[i]=makeSpanLengthByte(spanLength);
                    }
                    ଯଦି(which&BACK) {
                        spanLength=length16-spanSet.spanBack(s16, length16, USET_SPAN_CONTAINED);
                        spanBackLengths[i]=makeSpanLengthByte(spanLength);
                    }
                } ଅନ୍ୟଥା /* not CONTAINED, not all, but NOT_CONTAINED */ {
                    spanLengths[i]=spanBackLengths[i]=0;  // Only store a relevant/irrelevant flag.
                }
            }
            ଯଦି(which&UTF8) {
                uint8_t *s8=utf8+utf8_ଗଣନା;
                int32_t length8=appendUTF8(s16, length16, s8, utf8Length-utf8_ଗଣନା);
                utf8_ଗଣନା+=utf8Lengths[i]=length8;
                ଯଦି(length8==0) {  // Irrelevant for UTF-8 because not representable in UTF-8.
                    spanUTF8Lengths[i] = spanBackUTF8Lengths[i] = static_cast<uint8_t>(ALL_CP_CONTAINED);
                } ଅନ୍ୟଥା {  // Relevant for UTF-8.
                    ଯଦି(which&CONTAINED) {
                        ଯଦି(which&FWD) {
                            spanLength = spanSet.spanUTF8(reinterpret_cast<ଅଚଳ ଅକ୍ଷର*>(s8), length8, USET_SPAN_CONTAINED);
                            spanUTF8Lengths[i]=makeSpanLengthByte(spanLength);
                        }
                        ଯଦି(which&BACK) {
                            spanLength = length8 - spanSet.spanBackUTF8(reinterpret_cast<ଅଚଳ ଅକ୍ଷର*>(s8), length8, USET_SPAN_CONTAINED);
                            spanBackUTF8Lengths[i]=makeSpanLengthByte(spanLength);
                        }
                    } ଅନ୍ୟଥା /* not CONTAINED, not all, but NOT_CONTAINED */ {
                        spanUTF8Lengths[i]=spanBackUTF8Lengths[i]=0;  // Only store a relevant/irrelevant flag.
                    }
                }
            }
            ଯଦି(which&NOT_CONTAINED) {
                // Add string start and end code points to the spanNotSet so that
                // a span(while not contained) stops before any string.
                UChar32 c;
                ଯଦି(which&FWD) {
                    int32_t len=0;
                    U16_NEXT(s16, len, length16, c);
                    addToSpanNotSet(c);
                }
                ଯଦି(which&BACK) {
                    int32_t len=length16;
                    U16_PREV(s16, 0, len, c);
                    addToSpanNotSet(c);
                }
            }
        } ଅନ୍ୟଥା {  // Irrelevant string. (Also the empty string.)
            ଯଦି(which&UTF8) {
                ଯଦି(which&CONTAINED) {  // Only necessary for LONGEST_MATCH.
                    uint8_t *s8=utf8+utf8_ଗଣନା;
                    int32_t length8=appendUTF8(s16, length16, s8, utf8Length-utf8_ଗଣନା);
                    utf8_ଗଣନା+=utf8Lengths[i]=length8;
                } ଅନ୍ୟଥା {
                    utf8Lengths[i]=0;
                }
            }
            ଯଦି(all) {
                spanLengths[i]=spanBackLengths[i]=
                    spanUTF8Lengths[i]=spanBackUTF8Lengths[i]=
                        static_cast<uint8_t>(ALL_CP_CONTAINED);
            } ଅନ୍ୟଥା {
                // All spanXYZLengths pointers contain the same address.
                spanLengths[i] = static_cast<uint8_t>(ALL_CP_CONTAINED);
            }
        }
    }

    // Finish.
    ଯଦି(all) {
        pSpanNotSet->freeze();
    }
}

// Copy constructor. Assumes which==ALL for a frozen set.
UnicodeSetStringSpan::UnicodeSetStringSpan(ଅଚଳ UnicodeSetStringSpan &otherStringSpan,
                                           ଅଚଳ UVector &newParentSetStrings)
        : spanSet(otherStringSpan.spanSet), pSpanNotSet(nullptr), strings(newParentSetStrings),
          utf8Lengths(nullptr), spanLengths(nullptr), utf8(nullptr),
          utf8Length(otherStringSpan.utf8Length),
          maxLength16(otherStringSpan.maxLength16), maxLength8(otherStringSpan.maxLength8),
          all(ସତ୍ୟ) {
    ଯଦି(otherStringSpan.pSpanNotSet==&otherStringSpan.spanSet) {
        pSpanNotSet=&spanSet;
    } ଅନ୍ୟଥା {
        pSpanNotSet=otherStringSpan.pSpanNotSet->clone();
    }

    // Allocate a block of meta data.
    // UTF-8 lengths, 4 sets of span lengths, UTF-8 strings.
    int32_t stringsLength=strings.size();
    int32_t allocSize=stringsLength*(4+1+1+1+1)+utf8Length;
    ଯଦି (allocSize <= static_cast<int32_t>(ଆକାର(staticLengths))) {
        utf8Lengths=staticLengths;
    } ଅନ୍ୟଥା {
        utf8Lengths = static_cast<int32_t*>(uprv_malloc(allocSize));
        ଯଦି(utf8Lengths==nullptr) {
            maxLength16=maxLength8=0;  // Prevent usage by making needsStringSpanUTF16/8() return false.
            ଫେରନ୍ତୁ;  // Out of memory.
        }
    }

    spanLengths = reinterpret_cast<uint8_t*>(utf8Lengths + stringsLength);
    utf8=spanLengths+stringsLength*4;
    uprv_memcpy(utf8Lengths, otherStringSpan.utf8Lengths, allocSize);
}

UnicodeSetStringSpan::~UnicodeSetStringSpan() {
    ଯଦି(pSpanNotSet!=nullptr && pSpanNotSet!=&spanSet) {
        delete pSpanNotSet;
    }
    ଯଦି(utf8Lengths!=nullptr && utf8Lengths!=staticLengths) {
        uprv_free(utf8Lengths);
    }
}

ଶୂନ୍ୟ UnicodeSetStringSpan::addToSpanNotSet(UChar32 c) {
    ଯଦି(pSpanNotSet==nullptr || pSpanNotSet==&spanSet) {
        ଯଦି(spanSet.contains(c)) {
            ଫେରନ୍ତୁ;  // Nothing to do.
        }
        UnicodeSet *newSet=spanSet.cloneAsThawed();
        ଯଦି(newSet==nullptr) {
            ଫେରନ୍ତୁ;  // Out of memory.
        } ଅନ୍ୟଥା {
            pSpanNotSet=newSet;
        }
    }
    pSpanNotSet->add(c);
}

// Compare strings without any argument checks. Requires length>0.
ସ୍ଥିର ଧାଡ଼ିଭିତରେ UBool
matches16(ଅଚଳ char16_t *s, ଅଚଳ char16_t *t, int32_t length) {
    କର {
        ଯଦି(*s++!=*t++) {
            ଫେରନ୍ତୁ ମିଥ୍ୟା;
        }
    } ଯେତେବେଳେ(--length>0);
    ଫେରନ୍ତୁ ସତ୍ୟ;
}

ସ୍ଥିର ଧାଡ଼ିଭିତରେ UBool
matches8(ଅଚଳ uint8_t *s, ଅଚଳ uint8_t *t, int32_t length) {
    କର {
        ଯଦି(*s++!=*t++) {
            ଫେରନ୍ତୁ ମିଥ୍ୟା;
        }
    } ଯେତେବେଳେ(--length>0);
    ଫେରନ୍ତୁ ସତ୍ୟ;
}

// Compare 16-bit Unicode strings (which may be malformed UTF-16)
// at code point boundaries.
// That is, each edge of a match must not be in the middle of a surrogate pair.
ସ୍ଥିର ଧାଡ଼ିଭିତରେ UBool
matches16CPB(ଅଚଳ char16_t *s, int32_t start, int32_t limit, ଅଚଳ char16_t *t, int32_t length) {
    s+=start;
    limit-=start;
    ଫେରନ୍ତୁ matches16(s, t, length) &&
           !(0<start && U16_IS_LEAD(s[-1]) && U16_IS_TRAIL(s[0])) &&
           !(length<limit && U16_IS_LEAD(s[length-1]) && U16_IS_TRAIL(s[length]));
}

// Does the set contain the next code point?
// If so, return its length; otherwise return its negative length.
ସ୍ଥିର ଧାଡ଼ିଭିତରେ int32_t
spanOne(ଅଚଳ UnicodeSet &set, ଅଚଳ char16_t *s, int32_t length) {
    char16_t c=*s, c2;
    ଯଦି(c>=0xd800 && c<=0xdbff && length>=2 && U16_IS_TRAIL(c2=s[1])) {
        ଫେରନ୍ତୁ set.contains(U16_GET_SUPPLEMENTARY(c, c2)) ? 2 : -2;
    }
    ଫେରନ୍ତୁ set.contains(c) ? 1 : -1;
}

ସ୍ଥିର ଧାଡ଼ିଭିତରେ int32_t
spanOneBack(ଅଚଳ UnicodeSet &set, ଅଚଳ char16_t *s, int32_t length) {
    char16_t c=s[length-1], c2;
    ଯଦି(c>=0xdc00 && c<=0xdfff && length>=2 && U16_IS_LEAD(c2=s[length-2])) {
        ଫେରନ୍ତୁ set.contains(U16_GET_SUPPLEMENTARY(c2, c)) ? 2 : -2;
    }
    ଫେରନ୍ତୁ set.contains(c) ? 1 : -1;
}

ସ୍ଥିର ଧାଡ଼ିଭିତରେ int32_t
spanOneUTF8(ଅଚଳ UnicodeSet &set, ଅଚଳ uint8_t *s, int32_t length) {
    UChar32 c=*s;
    ଯଦି(U8_IS_SINGLE(c)) {
        ଫେରନ୍ତୁ set.contains(c) ? 1 : -1;
    }
    // Take advantage of non-ASCII fastpaths in U8_NEXT_OR_FFFD().
    int32_t i=0;
    U8_NEXT_OR_FFFD(s, i, length, c);
    ଫେରନ୍ତୁ set.contains(c) ? i : -i;
}

ସ୍ଥିର ଧାଡ଼ିଭିତରେ int32_t
spanOneBackUTF8(ଅଚଳ UnicodeSet &set, ଅଚଳ uint8_t *s, int32_t length) {
    UChar32 c=s[length-1];
    ଯଦି(U8_IS_SINGLE(c)) {
        ଫେରନ୍ତୁ set.contains(c) ? 1 : -1;
    }
    int32_t i=length-1;
    c=utf8_prevCharSafeBody(s, 0, &i, c, -3);
    length-=i;
    ଫେରନ୍ତୁ set.contains(c) ? length : -length;
}

/*
 * Note: In span() when spanLength==0 (after a string match, or at the beginning
 * after an empty code point span) and in spanNot() and spanNotUTF8(),
 * string matching could use a binary search
 * because all string matches are done from the same start index.
 *
 * For UTF-8, this would require a comparison function that returns UTF-16 order.
 *
 * This optimization should not be necessary for normal UnicodeSets because
 * most sets have no strings, and most sets with strings have
 * very few very short strings.
 * For cases with many strings, it might be better to use a different API
 * and implementation with a DFA (state machine).
 */

/*
 * Algorithm for span(USET_SPAN_CONTAINED)
 *
 * Theoretical algorithm:
 * - Iterate through the string, and at each code point boundary:
 *   + If the code point there is in the set, then remember to continue after it.
 *   + If a set string matches at the current position, then remember to continue after it.
 *   + Either recursively span for each code point or string match,
 *     or recursively span for all but the shortest one and
 *     iteratively continue the span with the shortest local match.
 *   + Remember the longest recursive span (the farthest end point).
 *   + If there is no match at the current position, neither for the code point there
 *     nor for any set string, then stop and return the longest recursive span length.
 *
 * Optimized implementation:
 *
 * (We assume that most sets will have very few very short strings.
 * A span using a string-less set is extremely fast.)
 *
 * Create and cache a spanSet which contains all of the single code points
 * of the original set but none of its strings.
 *
 * - Start with spanLength=spanSet.span(USET_SPAN_CONTAINED).
 * - Loop:
 *   + Try to match each set string at the end of the spanLength.
 *     ~ Set strings that start with set-contained code points must be matched
 *       with a partial overlap because the recursive algorithm would have tried
 *       to match them at every position.
 *     ~ Set strings that entirely consist of set-contained code points
 *       are irrelevant for span(USET_SPAN_CONTAINED) because the
 *       recursive algorithm would continue after them anyway
 *       and find the longest recursive match from their end.
 *     ~ Rather than recursing, note each end point of a set string match.
 *   + If no set string matched after spanSet.span(), then return
 *     with where the spanSet.span() ended.
 *   + If at least one set string matched after spanSet.span(), then
 *     pop the shortest string match end point and continue
 *     the loop, trying to match all set strings from there.
 *   + If at least one more set string matched after a previous string match,
 *     then test if the code point after the previous string match is also
 *     contained in the set.
 *     Continue the loop with the shortest end point of either this code point
 *     or a matching set string.
 *   + If no more set string matched after a previous string match,
 *     then try another spanLength=spanSet.span(USET_SPAN_CONTAINED).
 *     Stop if spanLength==0, otherwise continue the loop.
 *
 * By noting each end point of a set string match,
 * the function visits each string position at most once and finishes
 * in linear time.
 *
 * The recursive algorithm may visit the same string position many times
 * if multiple paths lead to it and finishes in exponential time.
 */

/*
 * Algorithm for span(USET_SPAN_SIMPLE)
 *
 * Theoretical algorithm:
 * - Iterate through the string, and at each code point boundary:
 *   + If the code point there is in the set, then remember to continue after it.
 *   + If a set string matches at the current position, then remember to continue after it.
 *   + Continue from the farthest match position and ignore all others.
 *   + If there is no match at the current position,
 *     then stop and return the current position.
 *
 * Optimized implementation:
 *
 * (Same assumption and spanSet as above.)
 *
 * - Start with spanLength=spanSet.span(USET_SPAN_CONTAINED).
 * - Loop:
 *   + Try to match each set string at the end of the spanLength.
 *     ~ Set strings that start with set-contained code points must be matched
 *       with a partial overlap because the standard algorithm would have tried
 *       to match them earlier.
 *     ~ Set strings that entirely consist of set-contained code points
 *       must be matched with a full overlap because the longest-match algorithm
 *       would hide set string matches that end earlier.
 *       Such set strings need not be matched earlier inside the code point span
 *       because the standard algorithm would then have continued after
 *       the set string match anyway.
 *     ~ Remember the longest set string match (farthest end point) from the earliest
 *       starting point.
 *   + If no set string matched after spanSet.span(), then return
 *     with where the spanSet.span() ended.
 *   + If at least one set string matched, then continue the loop after the
 *     longest match from the earliest position.
 *   + If no more set string matched after a previous string match,
 *     then try another spanLength=spanSet.span(USET_SPAN_CONTAINED).
 *     Stop if spanLength==0, otherwise continue the loop.
 */

int32_t UnicodeSetStringSpan::span(ଅଚଳ char16_t *s, int32_t length, USetSpanCondition spanCondition) ଅଚଳ {
    ଯଦି(spanCondition==USET_SPAN_NOT_CONTAINED) {
        ଫେରନ୍ତୁ spanNot(s, length);
    }
    int32_t spanLength=spanSet.span(s, length, USET_SPAN_CONTAINED);
    ଯଦି(spanLength==length) {
        ଫେରନ୍ତୁ length;
    }

    // Consider strings; they may overlap with the span.
    OffsetList offsets;
    ଯଦି(spanCondition==USET_SPAN_CONTAINED) {
        // Use offset list to try all possibilities.
        offsets.setMaxLength(maxLength16);
    }
    int32_t pos=spanLength, rest=length-pos;
    int32_t i, stringsLength=strings.size();
    ପାଇଁ(;;) {
        ଯଦି(spanCondition==USET_SPAN_CONTAINED) {
            ପାଇଁ(i=0; i<stringsLength; ++i) {
                int32_t overlap=spanLengths[i];
                ଯଦି(overlap==ALL_CP_CONTAINED) {
                    ଜାରି;  // Irrelevant string. (Also the empty string.)
                }
                ଅଚଳ UnicodeString& string = *static_cast<ଅଚଳ UnicodeString*>(strings.elementAt(i));
                ଅଚଳ char16_t *s16=string.getBuffer();
                int32_t length16=string.length();
                U_ASSERT(length>0);

                // Try to match this string at pos-overlap..pos.
                ଯଦି(overlap>=LONG_SPAN) {
                    overlap=length16;
                    // While contained: No point matching fully inside the code point span.
                    U16_BACK_1(s16, 0, overlap);  // Length of the string minus the last code point.
                }
                ଯଦି(overlap>spanLength) {
                    overlap=spanLength;
                }
                int32_t inc=length16-overlap;  // Keep overlap+inc==length16.
                ପାଇଁ(;;) {
                    ଯଦି(inc>rest) {
                        ଭାଙ୍ଗ;
                    }
                    // Try to match if the increment is not listed already.
                    ଯଦି(!offsets.containsOffset(inc) && matches16CPB(s, pos-overlap, length, s16, length16)) {
                        ଯଦି(inc==rest) {
                            ଫେରନ୍ତୁ length;  // Reached the end of the string.
                        }
                        offsets.addOffset(inc);
                    }
                    ଯଦି(overlap==0) {
                        ଭାଙ୍ଗ;
                    }
                    --overlap;
                    ++inc;
                }
            }
        } ଅନ୍ୟଥା /* USET_SPAN_SIMPLE */ {
            int32_t maxInc=0, maxOverlap=0;
            ପାଇଁ(i=0; i<stringsLength; ++i) {
                int32_t overlap=spanLengths[i];
                // For longest match, we do need to try to match even an all-contained string
                // to find the match from the earliest start.

                ଅଚଳ UnicodeString& string = *static_cast<ଅଚଳ UnicodeString*>(strings.elementAt(i));
                ଅଚଳ char16_t *s16=string.getBuffer();
                int32_t length16=string.length();
                ଯଦି (length16==0) {
                    ଜାରି;  // skip the empty string
                }

                // Try to match this string at pos-overlap..pos.
                ଯଦି(overlap>=LONG_SPAN) {
                    overlap=length16;
                    // Longest match: Need to match fully inside the code point span
                    // to find the match from the earliest start.
                }
                ଯଦି(overlap>spanLength) {
                    overlap=spanLength;
                }
                int32_t inc=length16-overlap;  // Keep overlap+inc==length16.
                ପାଇଁ(;;) {
                    ଯଦି(inc>rest || overlap<maxOverlap) {
                        ଭାଙ୍ଗ;
                    }
                    // Try to match if the string is longer or starts earlier.
                    ଯଦି( (overlap>maxOverlap || /* redundant overlap==maxOverlap && */ inc>maxInc) &&
                        matches16CPB(s, pos-overlap, length, s16, length16)
                    ) {
                        maxInc=inc;  // Longest match from earliest start.
                        maxOverlap=overlap;
                        ଭାଙ୍ଗ;
                    }
                    --overlap;
                    ++inc;
                }
            }

            ଯଦି(maxInc!=0 || maxOverlap!=0) {
                // Longest-match algorithm, and there was a string match.
                // Simply continue after it.
                pos+=maxInc;
                rest-=maxInc;
                ଯଦି(rest==0) {
                    ଫେରନ୍ତୁ length;  // Reached the end of the string.
                }
                spanLength=0;  // Match strings from after a string match.
                ଜାରି;
            }
        }
        // Finished trying to match all strings at pos.

        ଯଦି(spanLength!=0 || pos==0) {
            // The position is after an unlimited code point span (spanLength!=0),
            // not after a string match.
            // The only position where spanLength==0 after a span is pos==0.
            // Otherwise, an unlimited code point span is only tried again when no
            // strings match, and if such a non-initial span fails we stop.
            ଯଦି(offsets.isEmpty()) {
                ଫେରନ୍ତୁ pos;  // No strings matched after a span.
            }
            // Match strings from after the next string match.
        } ଅନ୍ୟଥା {
            // The position is after a string match (or a single code point).
            ଯଦି(offsets.isEmpty()) {
                // No more strings matched after a previous string match.
                // Try another code point span from after the last string match.
                spanLength=spanSet.span(s+pos, rest, USET_SPAN_CONTAINED);
                ଯଦି( spanLength==rest || // Reached the end of the string, or
                    spanLength==0       // neither strings nor span progressed.
                ) {
                    ଫେରନ୍ତୁ pos+spanLength;
                }
                pos+=spanLength;
                rest-=spanLength;
                ଜାରି;  // spanLength>0: Match strings from after a span.
            } ଅନ୍ୟଥା {
                // Try to match only one code point from after a string match if some
                // string matched beyond it, so that we try all possible positions
                // and don't overshoot.
                spanLength=spanOne(spanSet, s+pos, rest);
                ଯଦି(spanLength>0) {
                    ଯଦି(spanLength==rest) {
                        ଫେରନ୍ତୁ length;  // Reached the end of the string.
                    }
                    // Match strings after this code point.
                    // There cannot be any increments below it because UnicodeSet strings
                    // contain multiple code points.
                    pos+=spanLength;
                    rest-=spanLength;
                    offsets.shift(spanLength);
                    spanLength=0;
                    ଜାରି;  // Match strings from after a single code point.
                }
                // Match strings from after the next string match.
            }
        }
        int32_t minOffset=offsets.କାଢ଼_Minimum();
        pos+=minOffset;
        rest-=minOffset;
        spanLength=0;  // Match strings from after a string match.
    }
}

int32_t UnicodeSetStringSpan::spanBack(ଅଚଳ char16_t *s, int32_t length, USetSpanCondition spanCondition) ଅଚଳ {
    ଯଦି(spanCondition==USET_SPAN_NOT_CONTAINED) {
        ଫେରନ୍ତୁ spanNotBack(s, length);
    }
    int32_t pos=spanSet.spanBack(s, length, USET_SPAN_CONTAINED);
    ଯଦି(pos==0) {
        ଫେରନ୍ତୁ 0;
    }
    int32_t spanLength=length-pos;

    // Consider strings; they may overlap with the span.
    OffsetList offsets;
    ଯଦି(spanCondition==USET_SPAN_CONTAINED) {
        // Use offset list to try all possibilities.
        offsets.setMaxLength(maxLength16);
    }
    int32_t i, stringsLength=strings.size();
    uint8_t *spanBackLengths=spanLengths;
    ଯଦି(all) {
        spanBackLengths+=stringsLength;
    }
    ପାଇଁ(;;) {
        ଯଦି(spanCondition==USET_SPAN_CONTAINED) {
            ପାଇଁ(i=0; i<stringsLength; ++i) {
                int32_t overlap=spanBackLengths[i];
                ଯଦି(overlap==ALL_CP_CONTAINED) {
                    ଜାରି;  // Irrelevant string. (Also the empty string.)
                }
                ଅଚଳ UnicodeString& string = *static_cast<ଅଚଳ UnicodeString*>(strings.elementAt(i));
                ଅଚଳ char16_t *s16=string.getBuffer();
                int32_t length16=string.length();
                U_ASSERT(length>0);

                // Try to match this string at pos-(length16-overlap)..pos-length16.
                ଯଦି(overlap>=LONG_SPAN) {
                    overlap=length16;
                    // While contained: No point matching fully inside the code point span.
                    int32_t len1=0;
                    U16_FWD_1(s16, len1, overlap);
                    overlap-=len1;  // Length of the string minus the first code point.
                }
                ଯଦି(overlap>spanLength) {
                    overlap=spanLength;
                }
                int32_t dec=length16-overlap;  // Keep dec+overlap==length16.
                ପାଇଁ(;;) {
                    ଯଦି(dec>pos) {
                        ଭାଙ୍ଗ;
                    }
                    // Try to match if the decrement is not listed already.
                    ଯଦି(!offsets.containsOffset(dec) && matches16CPB(s, pos-dec, length, s16, length16)) {
                        ଯଦି(dec==pos) {
                            ଫେରନ୍ତୁ 0;  // Reached the start of the string.
                        }
                        offsets.addOffset(dec);
                    }
                    ଯଦି(overlap==0) {
                        ଭାଙ୍ଗ;
                    }
                    --overlap;
                    ++dec;
                }
            }
        } ଅନ୍ୟଥା /* USET_SPAN_SIMPLE */ {
            int32_t maxDec=0, maxOverlap=0;
            ପାଇଁ(i=0; i<stringsLength; ++i) {
                int32_t overlap=spanBackLengths[i];
                // For longest match, we do need to try to match even an all-contained string
                // to find the match from the latest end.

                ଅଚଳ UnicodeString& string = *static_cast<ଅଚଳ UnicodeString*>(strings.elementAt(i));
                ଅଚଳ char16_t *s16=string.getBuffer();
                int32_t length16=string.length();
                ଯଦି (length16==0) {
                    ଜାରି;  // skip the empty string
                }

                // Try to match this string at pos-(length16-overlap)..pos-length16.
                ଯଦି(overlap>=LONG_SPAN) {
                    overlap=length16;
                    // Longest match: Need to match fully inside the code point span
                    // to find the match from the latest end.
                }
                ଯଦି(overlap>spanLength) {
                    overlap=spanLength;
                }
                int32_t dec=length16-overlap;  // Keep dec+overlap==length16.
                ପାଇଁ(;;) {
                    ଯଦି(dec>pos || overlap<maxOverlap) {
                        ଭାଙ୍ଗ;
                    }
                    // Try to match if the string is longer or ends later.
                    ଯଦି( (overlap>maxOverlap || /* redundant overlap==maxOverlap && */ dec>maxDec) &&
                        matches16CPB(s, pos-dec, length, s16, length16)
                    ) {
                        maxDec=dec;  // Longest match from latest end.
                        maxOverlap=overlap;
                        ଭାଙ୍ଗ;
                    }
                    --overlap;
                    ++dec;
                }
            }

            ଯଦି(maxDec!=0 || maxOverlap!=0) {
                // Longest-match algorithm, and there was a string match.
                // Simply continue before it.
                pos-=maxDec;
                ଯଦି(pos==0) {
                    ଫେରନ୍ତୁ 0;  // Reached the start of the string.
                }
                spanLength=0;  // Match strings from before a string match.
                ଜାରି;
            }
        }
        // Finished trying to match all strings at pos.

        ଯଦି(spanLength!=0 || pos==length) {
            // The position is before an unlimited code point span (spanLength!=0),
            // not before a string match.
            // The only position where spanLength==0 before a span is pos==length.
            // Otherwise, an unlimited code point span is only tried again when no
            // strings match, and if such a non-initial span fails we stop.
            ଯଦି(offsets.isEmpty()) {
                ଫେରନ୍ତୁ pos;  // No strings matched before a span.
            }
            // Match strings from before the next string match.
        } ଅନ୍ୟଥା {
            // The position is before a string match (or a single code point).
            ଯଦି(offsets.isEmpty()) {
                // No more strings matched before a previous string match.
                // Try another code point span from before the last string match.
                int32_t oldPos=pos;
                pos=spanSet.spanBack(s, oldPos, USET_SPAN_CONTAINED);
                spanLength=oldPos-pos;
                ଯଦି( pos==0 ||           // Reached the start of the string, or
                    spanLength==0       // neither strings nor span progressed.
                ) {
                    ଫେରନ୍ତୁ pos;
                }
                ଜାରି;  // spanLength>0: Match strings from before a span.
            } ଅନ୍ୟଥା {
                // Try to match only one code point from before a string match if some
                // string matched beyond it, so that we try all possible positions
                // and don't overshoot.
                spanLength=spanOneBack(spanSet, s, pos);
                ଯଦି(spanLength>0) {
                    ଯଦି(spanLength==pos) {
                        ଫେରନ୍ତୁ 0;  // Reached the start of the string.
                    }
                    // Match strings before this code point.
                    // There cannot be any decrements below it because UnicodeSet strings
                    // contain multiple code points.
                    pos-=spanLength;
                    offsets.shift(spanLength);
                    spanLength=0;
                    ଜାରି;  // Match strings from before a single code point.
                }
                // Match strings from before the next string match.
            }
        }
        pos-=offsets.କାଢ଼_Minimum();
        spanLength=0;  // Match strings from before a string match.
    }
}

int32_t UnicodeSetStringSpan::spanUTF8(ଅଚଳ uint8_t *s, int32_t length, USetSpanCondition spanCondition) ଅଚଳ {
    ଯଦି(spanCondition==USET_SPAN_NOT_CONTAINED) {
        ଫେରନ୍ତୁ spanNotUTF8(s, length);
    }
    int32_t spanLength = spanSet.spanUTF8(reinterpret_cast<ଅଚଳ ଅକ୍ଷର*>(s), length, USET_SPAN_CONTAINED);
    ଯଦି(spanLength==length) {
        ଫେରନ୍ତୁ length;
    }

    // Consider strings; they may overlap with the span.
    OffsetList offsets;
    ଯଦି(spanCondition==USET_SPAN_CONTAINED) {
        // Use offset list to try all possibilities.
        offsets.setMaxLength(maxLength8);
    }
    int32_t pos=spanLength, rest=length-pos;
    int32_t i, stringsLength=strings.size();
    uint8_t *spanUTF8Lengths=spanLengths;
    ଯଦି(all) {
        spanUTF8Lengths+=2*stringsLength;
    }
    ପାଇଁ(;;) {
        ଅଚଳ uint8_t *s8=utf8;
        int32_t length8;
        ଯଦି(spanCondition==USET_SPAN_CONTAINED) {
            ପାଇଁ(i=0; i<stringsLength; ++i) {
                length8=utf8Lengths[i];
                ଯଦି(length8==0) {
                    ଜାରି;  // String not representable in UTF-8.
                }
                int32_t overlap=spanUTF8Lengths[i];
                ଯଦି(overlap==ALL_CP_CONTAINED) {
                    s8+=length8;
                    ଜାରି;  // Irrelevant string.
                }

                // Try to match this string at pos-overlap..pos.
                ଯଦି(overlap>=LONG_SPAN) {
                    overlap=length8;
                    // While contained: No point matching fully inside the code point span.
                    U8_BACK_1(s8, 0, overlap);  // Length of the string minus the last code point.
                }
                ଯଦି(overlap>spanLength) {
                    overlap=spanLength;
                }
                int32_t inc=length8-overlap;  // Keep overlap+inc==length8.
                ପାଇଁ(;;) {
                    ଯଦି(inc>rest) {
                        ଭାଙ୍ଗ;
                    }
                    // Try to match if the increment is not listed already.
                    // Match at code point boundaries. (The UTF-8 strings were converted
                    // from UTF-16 and are guaranteed to be well-formed.)
                    ଯଦି(!U8_IS_TRAIL(s[pos-overlap]) &&
                            !offsets.containsOffset(inc) &&
                            matches8(s+pos-overlap, s8, length8)) {
                        ଯଦି(inc==rest) {
                            ଫେରନ୍ତୁ length;  // Reached the end of the string.
                        }
                        offsets.addOffset(inc);
                    }
                    ଯଦି(overlap==0) {
                        ଭାଙ୍ଗ;
                    }
                    --overlap;
                    ++inc;
                }
                s8+=length8;
            }
        } ଅନ୍ୟଥା /* USET_SPAN_SIMPLE */ {
            int32_t maxInc=0, maxOverlap=0;
            ପାଇଁ(i=0; i<stringsLength; ++i) {
                length8=utf8Lengths[i];
                ଯଦି(length8==0) {
                    ଜାରି;  // String not representable in UTF-8.
                }
                int32_t overlap=spanUTF8Lengths[i];
                // For longest match, we do need to try to match even an all-contained string
                // to find the match from the earliest start.

                // Try to match this string at pos-overlap..pos.
                ଯଦି(overlap>=LONG_SPAN) {
                    overlap=length8;
                    // Longest match: Need to match fully inside the code point span
                    // to find the match from the earliest start.
                }
                ଯଦି(overlap>spanLength) {
                    overlap=spanLength;
                }
                int32_t inc=length8-overlap;  // Keep overlap+inc==length8.
                ପାଇଁ(;;) {
                    ଯଦି(inc>rest || overlap<maxOverlap) {
                        ଭାଙ୍ଗ;
                    }
                    // Try to match if the string is longer or starts earlier.
                    // Match at code point boundaries. (The UTF-8 strings were converted
                    // from UTF-16 and are guaranteed to be well-formed.)
                    ଯଦି(!U8_IS_TRAIL(s[pos-overlap]) &&
                            (overlap>maxOverlap ||
                                /* redundant overlap==maxOverlap && */ inc>maxInc) &&
                            matches8(s+pos-overlap, s8, length8)) {
                        maxInc=inc;  // Longest match from earliest start.
                        maxOverlap=overlap;
                        ଭାଙ୍ଗ;
                    }
                    --overlap;
                    ++inc;
                }
                s8+=length8;
            }

            ଯଦି(maxInc!=0 || maxOverlap!=0) {
                // Longest-match algorithm, and there was a string match.
                // Simply continue after it.
                pos+=maxInc;
                rest-=maxInc;
                ଯଦି(rest==0) {
                    ଫେରନ୍ତୁ length;  // Reached the end of the string.
                }
                spanLength=0;  // Match strings from after a string match.
                ଜାରି;
            }
        }
        // Finished trying to match all strings at pos.

        ଯଦି(spanLength!=0 || pos==0) {
            // The position is after an unlimited code point span (spanLength!=0),
            // not after a string match.
            // The only position where spanLength==0 after a span is pos==0.
            // Otherwise, an unlimited code point span is only tried again when no
            // strings match, and if such a non-initial span fails we stop.
            ଯଦି(offsets.isEmpty()) {
                ଫେରନ୍ତୁ pos;  // No strings matched after a span.
            }
            // Match strings from after the next string match.
        } ଅନ୍ୟଥା {
            // The position is after a string match (or a single code point).
            ଯଦି(offsets.isEmpty()) {
                // No more strings matched after a previous string match.
                // Try another code point span from after the last string match.
                spanLength = spanSet.spanUTF8(reinterpret_cast<ଅଚଳ ଅକ୍ଷର*>(s) + pos, rest, USET_SPAN_CONTAINED);
                ଯଦି( spanLength==rest || // Reached the end of the string, or
                    spanLength==0       // neither strings nor span progressed.
                ) {
                    ଫେରନ୍ତୁ pos+spanLength;
                }
                pos+=spanLength;
                rest-=spanLength;
                ଜାରି;  // spanLength>0: Match strings from after a span.
            } ଅନ୍ୟଥା {
                // Try to match only one code point from after a string match if some
                // string matched beyond it, so that we try all possible positions
                // and don't overshoot.
                spanLength=spanOneUTF8(spanSet, s+pos, rest);
                ଯଦି(spanLength>0) {
                    ଯଦି(spanLength==rest) {
                        ଫେରନ୍ତୁ length;  // Reached the end of the string.
                    }
                    // Match strings after this code point.
                    // There cannot be any increments below it because UnicodeSet strings
                    // contain multiple code points.
                    pos+=spanLength;
                    rest-=spanLength;
                    offsets.shift(spanLength);
                    spanLength=0;
                    ଜାରି;  // Match strings from after a single code point.
                }
                // Match strings from after the next string match.
            }
        }
        int32_t minOffset=offsets.କାଢ଼_Minimum();
        pos+=minOffset;
        rest-=minOffset;
        spanLength=0;  // Match strings from after a string match.
    }
}

int32_t UnicodeSetStringSpan::spanBackUTF8(ଅଚଳ uint8_t *s, int32_t length, USetSpanCondition spanCondition) ଅଚଳ {
    ଯଦି(spanCondition==USET_SPAN_NOT_CONTAINED) {
        ଫେରନ୍ତୁ spanNotBackUTF8(s, length);
    }
    int32_t pos = spanSet.spanBackUTF8(reinterpret_cast<ଅଚଳ ଅକ୍ଷର*>(s), length, USET_SPAN_CONTAINED);
    ଯଦି(pos==0) {
        ଫେରନ୍ତୁ 0;
    }
    int32_t spanLength=length-pos;

    // Consider strings; they may overlap with the span.
    OffsetList offsets;
    ଯଦି(spanCondition==USET_SPAN_CONTAINED) {
        // Use offset list to try all possibilities.
        offsets.setMaxLength(maxLength8);
    }
    int32_t i, stringsLength=strings.size();
    uint8_t *spanBackUTF8Lengths=spanLengths;
    ଯଦି(all) {
        spanBackUTF8Lengths+=3*stringsLength;
    }
    ପାଇଁ(;;) {
        ଅଚଳ uint8_t *s8=utf8;
        int32_t length8;
        ଯଦି(spanCondition==USET_SPAN_CONTAINED) {
            ପାଇଁ(i=0; i<stringsLength; ++i) {
                length8=utf8Lengths[i];
                ଯଦି(length8==0) {
                    ଜାରି;  // String not representable in UTF-8.
                }
                int32_t overlap=spanBackUTF8Lengths[i];
                ଯଦି(overlap==ALL_CP_CONTAINED) {
                    s8+=length8;
                    ଜାରି;  // Irrelevant string.
                }

                // Try to match this string at pos-(length8-overlap)..pos-length8.
                ଯଦି(overlap>=LONG_SPAN) {
                    overlap=length8;
                    // While contained: No point matching fully inside the code point span.
                    int32_t len1=0;
                    U8_FWD_1(s8, len1, overlap);
                    overlap-=len1;  // Length of the string minus the first code point.
                }
                ଯଦି(overlap>spanLength) {
                    overlap=spanLength;
                }
                int32_t dec=length8-overlap;  // Keep dec+overlap==length8.
                ପାଇଁ(;;) {
                    ଯଦି(dec>pos) {
                        ଭାଙ୍ଗ;
                    }
                    // Try to match if the decrement is not listed already.
                    // Match at code point boundaries. (The UTF-8 strings were converted
                    // from UTF-16 and are guaranteed to be well-formed.)
                    ଯଦି( !U8_IS_TRAIL(s[pos-dec]) &&
                        !offsets.containsOffset(dec) &&
                        matches8(s+pos-dec, s8, length8)
                    ) {
                        ଯଦି(dec==pos) {
                            ଫେରନ୍ତୁ 0;  // Reached the start of the string.
                        }
                        offsets.addOffset(dec);
                    }
                    ଯଦି(overlap==0) {
                        ଭାଙ୍ଗ;
                    }
                    --overlap;
                    ++dec;
                }
                s8+=length8;
            }
        } ଅନ୍ୟଥା /* USET_SPAN_SIMPLE */ {
            int32_t maxDec=0, maxOverlap=0;
            ପାଇଁ(i=0; i<stringsLength; ++i) {
                length8=utf8Lengths[i];
                ଯଦି(length8==0) {
                    ଜାରି;  // String not representable in UTF-8.
                }
                int32_t overlap=spanBackUTF8Lengths[i];
                // For longest match, we do need to try to match even an all-contained string
                // to find the match from the latest end.

                // Try to match this string at pos-(length8-overlap)..pos-length8.
                ଯଦି(overlap>=LONG_SPAN) {
                    overlap=length8;
                    // Longest match: Need to match fully inside the code point span
                    // to find the match from the latest end.
                }
                ଯଦି(overlap>spanLength) {
                    overlap=spanLength;
                }
                int32_t dec=length8-overlap;  // Keep dec+overlap==length8.
                ପାଇଁ(;;) {
                    ଯଦି(dec>pos || overlap<maxOverlap) {
                        ଭାଙ୍ଗ;
                    }
                    // Try to match if the string is longer or ends later.
                    // Match at code point boundaries. (The UTF-8 strings were converted
                    // from UTF-16 and are guaranteed to be well-formed.)
                    ଯଦି( !U8_IS_TRAIL(s[pos-dec]) &&
                        (overlap>maxOverlap || /* redundant overlap==maxOverlap && */ dec>maxDec) &&
                        matches8(s+pos-dec, s8, length8)
                    ) {
                        maxDec=dec;  // Longest match from latest end.
                        maxOverlap=overlap;
                        ଭାଙ୍ଗ;
                    }
                    --overlap;
                    ++dec;
                }
                s8+=length8;
            }

            ଯଦି(maxDec!=0 || maxOverlap!=0) {
                // Longest-match algorithm, and there was a string match.
                // Simply continue before it.
                pos-=maxDec;
                ଯଦି(pos==0) {
                    ଫେରନ୍ତୁ 0;  // Reached the start of the string.
                }
                spanLength=0;  // Match strings from before a string match.
                ଜାରି;
            }
        }
        // Finished trying to match all strings at pos.

        ଯଦି(spanLength!=0 || pos==length) {
            // The position is before an unlimited code point span (spanLength!=0),
            // not before a string match.
            // The only position where spanLength==0 before a span is pos==length.
            // Otherwise, an unlimited code point span is only tried again when no
            // strings match, and if such a non-initial span fails we stop.
            ଯଦି(offsets.isEmpty()) {
                ଫେରନ୍ତୁ pos;  // No strings matched before a span.
            }
            // Match strings from before the next string match.
        } ଅନ୍ୟଥା {
            // The position is before a string match (or a single code point).
            ଯଦି(offsets.isEmpty()) {
                // No more strings matched before a previous string match.
                // Try another code point span from before the last string match.
                int32_t oldPos=pos;
                pos = spanSet.spanBackUTF8(reinterpret_cast<ଅଚଳ ଅକ୍ଷର*>(s), oldPos, USET_SPAN_CONTAINED);
                spanLength=oldPos-pos;
                ଯଦି( pos==0 ||           // Reached the start of the string, or
                    spanLength==0       // neither strings nor span progressed.
                ) {
                    ଫେରନ୍ତୁ pos;
                }
                ଜାରି;  // spanLength>0: Match strings from before a span.
            } ଅନ୍ୟଥା {
                // Try to match only one code point from before a string match if some
                // string matched beyond it, so that we try all possible positions
                // and don't overshoot.
                spanLength=spanOneBackUTF8(spanSet, s, pos);
                ଯଦି(spanLength>0) {
                    ଯଦି(spanLength==pos) {
                        ଫେରନ୍ତୁ 0;  // Reached the start of the string.
                    }
                    // Match strings before this code point.
                    // There cannot be any decrements below it because UnicodeSet strings
                    // contain multiple code points.
                    pos-=spanLength;
                    offsets.shift(spanLength);
                    spanLength=0;
                    ଜାରି;  // Match strings from before a single code point.
                }
                // Match strings from before the next string match.
            }
        }
        pos-=offsets.କାଢ଼_Minimum();
        spanLength=0;  // Match strings from before a string match.
    }
}

/*
 * Algorithm for spanNot()==span(USET_SPAN_NOT_CONTAINED)
 *
 * Theoretical algorithm:
 * - Iterate through the string, and at each code point boundary:
 *   + If the code point there is in the set, then return with the current position.
 *   + If a set string matches at the current position, then return with the current position.
 *
 * Optimized implementation:
 *
 * (Same assumption as for span() above.)
 *
 * Create and cache a spanNotSet which contains all of the single code points
 * of the original set but none of its strings.
 * For each set string add its initial code point to the spanNotSet.
 * (Also add its final code point for spanNotBack().)
 *
 * - Loop:
 *   + Do spanLength=spanNotSet.span(USET_SPAN_NOT_CONTAINED).
 *   + If the current code point is in the original set, then
 *     return the current position.
 *   + If any set string matches at the current position, then
 *     return the current position.
 *   + If there is no match at the current position, neither for the code point there
 *     nor for any set string, then skip this code point and continue the loop.
 *     This happens for set-string-initial code points that were added to spanNotSet
 *     when there is not actually a match for such a set string.
 */

int32_t UnicodeSetStringSpan::spanNot(ଅଚଳ char16_t *s, int32_t length) ଅଚଳ {
    int32_t pos=0, rest=length;
    int32_t i, stringsLength=strings.size();
    କର {
        // Span until we find a code point from the set,
        // or a code point that starts or ends some string.
        i=pSpanNotSet->span(s+pos, rest, USET_SPAN_NOT_CONTAINED);
        ଯଦି(i==rest) {
            ଫେରନ୍ତୁ length;  // Reached the end of the string.
        }
        pos+=i;
        rest-=i;

        // Check whether the current code point is in the original set,
        // without the string starts and ends.
        int32_t cpLength=spanOne(spanSet, s+pos, rest);
        ଯଦି(cpLength>0) {
            ଫେରନ୍ତୁ pos;  // There is a set element at pos.
        }

        // Try to match the strings at pos.
        ପାଇଁ(i=0; i<stringsLength; ++i) {
            ଯଦି(spanLengths[i]==ALL_CP_CONTAINED) {
                ଜାରି;  // Irrelevant string. (Also the empty string.)
            }
            ଅଚଳ UnicodeString& string = *static_cast<ଅଚଳ UnicodeString*>(strings.elementAt(i));
            ଅଚଳ char16_t *s16=string.getBuffer();
            int32_t length16=string.length();
            U_ASSERT(length>0);
            ଯଦି(length16<=rest && matches16CPB(s, pos, length, s16, length16)) {
                ଫେରନ୍ତୁ pos;  // There is a set element at pos.
            }
        }

        // The span(while not contained) ended on a string start/end which is
        // not in the original set. Skip this code point and continue.
        // cpLength<0
        pos-=cpLength;
        rest+=cpLength;
    } ଯେତେବେଳେ(rest!=0);
    ଫେରନ୍ତୁ length;  // Reached the end of the string.
}

int32_t UnicodeSetStringSpan::spanNotBack(ଅଚଳ char16_t *s, int32_t length) ଅଚଳ {
    int32_t pos=length;
    int32_t i, stringsLength=strings.size();
    କର {
        // Span until we find a code point from the set,
        // or a code point that starts or ends some string.
        pos=pSpanNotSet->spanBack(s, pos, USET_SPAN_NOT_CONTAINED);
        ଯଦି(pos==0) {
            ଫେରନ୍ତୁ 0;  // Reached the start of the string.
        }

        // Check whether the current code point is in the original set,
        // without the string starts and ends.
        int32_t cpLength=spanOneBack(spanSet, s, pos);
        ଯଦି(cpLength>0) {
            ଫେରନ୍ତୁ pos;  // There is a set element at pos.
        }

        // Try to match the strings at pos.
        ପାଇଁ(i=0; i<stringsLength; ++i) {
            // Use spanLengths rather than a spanBackLengths pointer because
            // it is easier and we only need to know whether the string is irrelevant
            // which is the same in either array.
            ଯଦି(spanLengths[i]==ALL_CP_CONTAINED) {
                ଜାରି;  // Irrelevant string. (Also the empty string.)
            }
            ଅଚଳ UnicodeString& string = *static_cast<ଅଚଳ UnicodeString*>(strings.elementAt(i));
            ଅଚଳ char16_t *s16=string.getBuffer();
            int32_t length16=string.length();
            U_ASSERT(length>0);
            ଯଦି(length16<=pos && matches16CPB(s, pos-length16, length, s16, length16)) {
                ଫେରନ୍ତୁ pos;  // There is a set element at pos.
            }
        }

        // The span(while not contained) ended on a string start/end which is
        // not in the original set. Skip this code point and continue.
        // cpLength<0
        pos+=cpLength;
    } ଯେତେବେଳେ(pos!=0);
    ଫେରନ୍ତୁ 0;  // Reached the start of the string.
}

int32_t UnicodeSetStringSpan::spanNotUTF8(ଅଚଳ uint8_t *s, int32_t length) ଅଚଳ {
    int32_t pos=0, rest=length;
    int32_t i, stringsLength=strings.size();
    uint8_t *spanUTF8Lengths=spanLengths;
    ଯଦି(all) {
        spanUTF8Lengths+=2*stringsLength;
    }
    କର {
        // Span until we find a code point from the set,
        // or a code point that starts or ends some string.
        i = pSpanNotSet->spanUTF8(reinterpret_cast<ଅଚଳ ଅକ୍ଷର*>(s) + pos, rest, USET_SPAN_NOT_CONTAINED);
        ଯଦି(i==rest) {
            ଫେରନ୍ତୁ length;  // Reached the end of the string.
        }
        pos+=i;
        rest-=i;

        // Check whether the current code point is in the original set,
        // without the string starts and ends.
        int32_t cpLength=spanOneUTF8(spanSet, s+pos, rest);
        ଯଦି(cpLength>0) {
            ଫେରନ୍ତୁ pos;  // There is a set element at pos.
        }

        // Try to match the strings at pos.
        ଅଚଳ uint8_t *s8=utf8;
        int32_t length8;
        ପାଇଁ(i=0; i<stringsLength; ++i) {
            length8=utf8Lengths[i];
            // ALL_CP_CONTAINED: Irrelevant string.
            ଯଦି(length8!=0 && spanUTF8Lengths[i]!=ALL_CP_CONTAINED && length8<=rest && matches8(s+pos, s8, length8)) {
                ଫେରନ୍ତୁ pos;  // There is a set element at pos.
            }
            s8+=length8;
        }

        // The span(while not contained) ended on a string start/end which is
        // not in the original set. Skip this code point and continue.
        // cpLength<0
        pos-=cpLength;
        rest+=cpLength;
    } ଯେତେବେଳେ(rest!=0);
    ଫେରନ୍ତୁ length;  // Reached the end of the string.
}

int32_t UnicodeSetStringSpan::spanNotBackUTF8(ଅଚଳ uint8_t *s, int32_t length) ଅଚଳ {
    int32_t pos=length;
    int32_t i, stringsLength=strings.size();
    uint8_t *spanBackUTF8Lengths=spanLengths;
    ଯଦି(all) {
        spanBackUTF8Lengths+=3*stringsLength;
    }
    କର {
        // Span until we find a code point from the set,
        // or a code point that starts or ends some string.
        pos = pSpanNotSet->spanBackUTF8(reinterpret_cast<ଅଚଳ ଅକ୍ଷର*>(s), pos, USET_SPAN_NOT_CONTAINED);
        ଯଦି(pos==0) {
            ଫେରନ୍ତୁ 0;  // Reached the start of the string.
        }

        // Check whether the current code point is in the original set,
        // without the string starts and ends.
        int32_t cpLength=spanOneBackUTF8(spanSet, s, pos);
        ଯଦି(cpLength>0) {
            ଫେରନ୍ତୁ pos;  // There is a set element at pos.
        }

        // Try to match the strings at pos.
        ଅଚଳ uint8_t *s8=utf8;
        int32_t length8;
        ପାଇଁ(i=0; i<stringsLength; ++i) {
            length8=utf8Lengths[i];
            // ALL_CP_CONTAINED: Irrelevant string.
            ଯଦି(length8!=0 && spanBackUTF8Lengths[i]!=ALL_CP_CONTAINED && length8<=pos && matches8(s+pos-length8, s8, length8)) {
                ଫେରନ୍ତୁ pos;  // There is a set element at pos.
            }
            s8+=length8;
        }

        // The span(while not contained) ended on a string start/end which is
        // not in the original set. Skip this code point and continue.
        // cpLength<0
        pos+=cpLength;
    } ଯେତେବେଳେ(pos!=0);
    ଫେରନ୍ତୁ 0;  // Reached the start of the string.
}

U_NAMESPACE_END
