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

#അന്തര്ഭാവ "unicode/utypes.h"
#അന്തര്ഭാവ "unicode/uniset.h"
#അന്തര്ഭാവ "unicode/ustring.h"
#അന്തര്ഭാവ "unicode/utf8.h"
#അന്തര്ഭാവ "unicode/utf16.h"
#അന്തര്ഭാവ "cmemory.h"
#അന്തര്ഭാവ "uvector.h"
#അന്തര്ഭാവ "unisetspan.h"

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
വർഗ്ഗം OffsetList {  // Only ever stack-allocated, does not need to inherit UMemory.
പൊതു:
    OffsetList() : list(staticList), capacity(0), length(0), start(0) {}

    ~OffsetList() {
        എങ്കിൽ(list!=staticList) {
            uprv_free(list);
        }
    }

    // Call exactly once if the list is to be used.
    ശൂന്യം setMaxLength(int32_t maxLength) {
        എങ്കിൽ (maxLength <= static_cast<int32_t>(വലുപ്പം(staticList))) {
            capacity = static_cast<int32_t>(വലുപ്പം(staticList));
        } അല്ലെങ്കിൽ {
            UBool* l = static_cast<UBool*>(uprv_malloc(maxLength));
            എങ്കിൽ(l!=nullptr) {
                list=l;
                capacity=maxLength;
            }
        }
        uprv_memset(list, 0, capacity);
    }

    ശൂന്യം clear() {
        uprv_memset(list, 0, capacity);
        start=length=0;
    }

    UBool isEmpty() സ്ഥിരം {
        തിരികെ length == 0;
    }

    // Reduce all stored offsets by delta, used when the current position
    // moves by delta.
    // There must not be any offsets lower than delta.
    // If there is an offset equal to delta, it is removed.
    // delta=[1..maxLength]
    ശൂന്യം shift(int32_t delta) {
        int32_t i=start+delta;
        എങ്കിൽ(i>=capacity) {
            i-=capacity;
        }
        എങ്കിൽ(list[i]) {
            list[i]=അസത്യം;
            --length;
        }
        start=i;
    }

    // Add an offset. The list must not contain it yet.
    // offset=[1..maxLength]
    ശൂന്യം addOffset(int32_t offset) {
        int32_t i=start+offset;
        എങ്കിൽ(i>=capacity) {
            i-=capacity;
        }
        list[i]=സത്യം;
        ++length;
    }

    // offset=[1..maxLength]
    UBool containsOffset(int32_t offset) സ്ഥിരം {
        int32_t i=start+offset;
        എങ്കിൽ(i>=capacity) {
            i-=capacity;
        }
        തിരികെ list[i];
    }

    // Find the lowest stored offset from a non-empty list, remove it,
    // and reduce all other offsets by this minimum.
    // Returns [1..maxLength].
    int32_t എടുക്കുക_Minimum() {
        // Look for the next offset in list[start+1..capacity-1].
        int32_t i=start, result;
        ആകുമ്പോൾ(++i<capacity) {
            എങ്കിൽ(list[i]) {
                list[i]=അസത്യം;
                --length;
                result=i-start;
                start=i;
                തിരികെ result;
            }
        }
        // i==capacity

        // Wrap around and look for the next offset in list[0..start].
        // Since the list is not empty, there will be one.
        result=capacity-start;
        i=0;
        ആകുമ്പോൾ(!list[i]) {
            ++i;
        }
        list[i]=അസത്യം;
        --length;
        start=i;
        തിരികെ result+=i;
    }

സ്വകാര്യ:
    UBool *list;
    int32_t capacity;
    int32_t length;
    int32_t start;

    UBool staticList[16];
};

// Get the number of UTF-8 bytes for a UTF-16 (sub)string.
സ്ഥിതം int32_t
getUTF8Length(സ്ഥിരം char16_t *s, int32_t length) {
    UErrorCode errorCode=U_ZERO_ERROR;
    int32_t length8=0;
    u_strToUTF8(nullptr, 0, &length8, s, length, &errorCode);
    എങ്കിൽ(U_SUCCESS(errorCode) || errorCode==U_BUFFER_OVERFLOW_ERROR) {
        തിരികെ length8;
    } അല്ലെങ്കിൽ {
        // The string contains an unpaired surrogate.
        // Ignore this string.
        തിരികെ 0;
    }
}

// Append the UTF-8 version of the string to t and return the appended UTF-8 length.
സ്ഥിതം int32_t
appendUTF8(സ്ഥിരം char16_t *s, int32_t length, uint8_t *t, int32_t capacity) {
    UErrorCode errorCode=U_ZERO_ERROR;
    int32_t length8=0;
    u_strToUTF8(reinterpret_cast<അക്ഷരം*>(t), capacity, &length8, s, length, &errorCode);
    എങ്കിൽ(U_SUCCESS(errorCode)) {
        തിരികെ length8;
    } അല്ലെങ്കിൽ {
        // The string contains an unpaired surrogate.
        // Ignore this string.
        തിരികെ 0;
    }
}

സ്ഥിതം വരിയിൽ uint8_t
makeSpanLengthByte(int32_t spanLength) {
    // 0xfe==UnicodeSetStringSpan::LONG_SPAN
    തിരികെ spanLength < 0xfe ? static_cast<uint8_t>(spanLength) : static_cast<uint8_t>(0xfe);
}

// Construct for all variants of span(), or only for any one variant.
// Initialize as little as possible, for single use.
UnicodeSetStringSpan::UnicodeSetStringSpan(സ്ഥിരം UnicodeSet &set,
                                           സ്ഥിരം UVector &setStrings,
                                           uint32_t which)
        : spanSet(0, 0x10ffff), pSpanNotSet(nullptr), strings(setStrings),
          utf8Lengths(nullptr), spanLengths(nullptr), utf8(nullptr),
          utf8Length(0),
          maxLength16(0), maxLength8(0),
          all(static_cast<UBool>(which == ALL)) {
    spanSet.retainAll(set);
    എങ്കിൽ(which&NOT_CONTAINED) {
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
    UBool someRelevant=അസത്യം;
    വേണ്ടി(i=0; i<stringsLength; ++i) {
        സ്ഥിരം UnicodeString& string = *static_cast<സ്ഥിരം UnicodeString*>(strings.elementAt(i));
        സ്ഥിരം char16_t *s16=string.getBuffer();
        int32_t length16=string.length();
        എങ്കിൽ (length16==0) {
            തുടരുക;  // skip the empty string
        }
        UBool thisRelevant;
        spanLength=spanSet.span(s16, length16, USET_SPAN_CONTAINED);
        എങ്കിൽ(spanLength<length16) {  // Relevant string.
            someRelevant=thisRelevant=സത്യം;
        } അല്ലെങ്കിൽ {
            thisRelevant=അസത്യം;
        }
        എങ്കിൽ((which&UTF16) && length16>maxLength16) {
            maxLength16=length16;
        }
        എങ്കിൽ((which&UTF8) && (thisRelevant || (which&CONTAINED))) {
            int32_t length8=getUTF8Length(s16, length16);
            utf8Length+=length8;
            എങ്കിൽ(length8>maxLength8) {
                maxLength8=length8;
            }
        }
    }
    എങ്കിൽ(!someRelevant) {
        maxLength16=maxLength8=0;
        തിരികെ;
    }

    // Freeze after checking for the need to use strings at all because freezing
    // a set takes some time and memory which are wasted if there are no relevant strings.
    എങ്കിൽ(all) {
        spanSet.freeze();
    }

    uint8_t *spanBackLengths;
    uint8_t *spanUTF8Lengths;
    uint8_t *spanBackUTF8Lengths;

    // Allocate a block of meta data.
    int32_t allocSize;
    എങ്കിൽ(all) {
        // UTF-8 lengths, 4 sets of span lengths, UTF-8 strings.
        allocSize=stringsLength*(4+1+1+1+1)+utf8Length;
    } അല്ലെങ്കിൽ {
        allocSize=stringsLength;  // One set of span lengths.
        എങ്കിൽ(which&UTF8) {
            // UTF-8 lengths and UTF-8 strings.
            allocSize+=stringsLength*4+utf8Length;
        }
    }
    എങ്കിൽ (allocSize <= static_cast<int32_t>(വലുപ്പം(staticLengths))) {
        utf8Lengths=staticLengths;
    } അല്ലെങ്കിൽ {
        utf8Lengths = static_cast<int32_t*>(uprv_malloc(allocSize));
        എങ്കിൽ(utf8Lengths==nullptr) {
            maxLength16=maxLength8=0;  // Prevent usage by making needsStringSpanUTF16/8() return false.
            തിരികെ;  // Out of memory.
        }
    }

    എങ്കിൽ(all) {
        // Store span lengths for all span() variants.
        spanLengths = reinterpret_cast<uint8_t*>(utf8Lengths + stringsLength);
        spanBackLengths=spanLengths+stringsLength;
        spanUTF8Lengths=spanBackLengths+stringsLength;
        spanBackUTF8Lengths=spanUTF8Lengths+stringsLength;
        utf8=spanBackUTF8Lengths+stringsLength;
    } അല്ലെങ്കിൽ {
        // Store span lengths for only one span() variant.
        എങ്കിൽ(which&UTF8) {
            spanLengths = reinterpret_cast<uint8_t*>(utf8Lengths + stringsLength);
            utf8=spanLengths+stringsLength;
        } അല്ലെങ്കിൽ {
            spanLengths = reinterpret_cast<uint8_t*>(utf8Lengths);
        }
        spanBackLengths=spanUTF8Lengths=spanBackUTF8Lengths=spanLengths;
    }

    // Set the meta data and pSpanNotSet and write the UTF-8 strings.
    int32_t utf8_എണ്ണം=0;  // Count UTF-8 bytes written so far.

    വേണ്ടി(i=0; i<stringsLength; ++i) {
        സ്ഥിരം UnicodeString& string = *static_cast<സ്ഥിരം UnicodeString*>(strings.elementAt(i));
        സ്ഥിരം char16_t *s16=string.getBuffer();
        int32_t length16=string.length();
        spanLength=spanSet.span(s16, length16, USET_SPAN_CONTAINED);
        എങ്കിൽ(spanLength<length16 && length16>0) {  // Relevant string.
            എങ്കിൽ(which&UTF16) {
                എങ്കിൽ(which&CONTAINED) {
                    എങ്കിൽ(which&FWD) {
                        spanLengths[i]=makeSpanLengthByte(spanLength);
                    }
                    എങ്കിൽ(which&BACK) {
                        spanLength=length16-spanSet.spanBack(s16, length16, USET_SPAN_CONTAINED);
                        spanBackLengths[i]=makeSpanLengthByte(spanLength);
                    }
                } അല്ലെങ്കിൽ /* not CONTAINED, not all, but NOT_CONTAINED */ {
                    spanLengths[i]=spanBackLengths[i]=0;  // Only store a relevant/irrelevant flag.
                }
            }
            എങ്കിൽ(which&UTF8) {
                uint8_t *s8=utf8+utf8_എണ്ണം;
                int32_t length8=appendUTF8(s16, length16, s8, utf8Length-utf8_എണ്ണം);
                utf8_എണ്ണം+=utf8Lengths[i]=length8;
                എങ്കിൽ(length8==0) {  // Irrelevant for UTF-8 because not representable in UTF-8.
                    spanUTF8Lengths[i] = spanBackUTF8Lengths[i] = static_cast<uint8_t>(ALL_CP_CONTAINED);
                } അല്ലെങ്കിൽ {  // Relevant for UTF-8.
                    എങ്കിൽ(which&CONTAINED) {
                        എങ്കിൽ(which&FWD) {
                            spanLength = spanSet.spanUTF8(reinterpret_cast<സ്ഥിരം അക്ഷരം*>(s8), length8, USET_SPAN_CONTAINED);
                            spanUTF8Lengths[i]=makeSpanLengthByte(spanLength);
                        }
                        എങ്കിൽ(which&BACK) {
                            spanLength = length8 - spanSet.spanBackUTF8(reinterpret_cast<സ്ഥിരം അക്ഷരം*>(s8), length8, USET_SPAN_CONTAINED);
                            spanBackUTF8Lengths[i]=makeSpanLengthByte(spanLength);
                        }
                    } അല്ലെങ്കിൽ /* not CONTAINED, not all, but NOT_CONTAINED */ {
                        spanUTF8Lengths[i]=spanBackUTF8Lengths[i]=0;  // Only store a relevant/irrelevant flag.
                    }
                }
            }
            എങ്കിൽ(which&NOT_CONTAINED) {
                // Add string start and end code points to the spanNotSet so that
                // a span(while not contained) stops before any string.
                UChar32 c;
                എങ്കിൽ(which&FWD) {
                    int32_t len=0;
                    U16_NEXT(s16, len, length16, c);
                    addToSpanNotSet(c);
                }
                എങ്കിൽ(which&BACK) {
                    int32_t len=length16;
                    U16_PREV(s16, 0, len, c);
                    addToSpanNotSet(c);
                }
            }
        } അല്ലെങ്കിൽ {  // Irrelevant string. (Also the empty string.)
            എങ്കിൽ(which&UTF8) {
                എങ്കിൽ(which&CONTAINED) {  // Only necessary for LONGEST_MATCH.
                    uint8_t *s8=utf8+utf8_എണ്ണം;
                    int32_t length8=appendUTF8(s16, length16, s8, utf8Length-utf8_എണ്ണം);
                    utf8_എണ്ണം+=utf8Lengths[i]=length8;
                } അല്ലെങ്കിൽ {
                    utf8Lengths[i]=0;
                }
            }
            എങ്കിൽ(all) {
                spanLengths[i]=spanBackLengths[i]=
                    spanUTF8Lengths[i]=spanBackUTF8Lengths[i]=
                        static_cast<uint8_t>(ALL_CP_CONTAINED);
            } അല്ലെങ്കിൽ {
                // All spanXYZLengths pointers contain the same address.
                spanLengths[i] = static_cast<uint8_t>(ALL_CP_CONTAINED);
            }
        }
    }

    // Finish.
    എങ്കിൽ(all) {
        pSpanNotSet->freeze();
    }
}

// Copy constructor. Assumes which==ALL for a frozen set.
UnicodeSetStringSpan::UnicodeSetStringSpan(സ്ഥിരം UnicodeSetStringSpan &otherStringSpan,
                                           സ്ഥിരം UVector &newParentSetStrings)
        : spanSet(otherStringSpan.spanSet), pSpanNotSet(nullptr), strings(newParentSetStrings),
          utf8Lengths(nullptr), spanLengths(nullptr), utf8(nullptr),
          utf8Length(otherStringSpan.utf8Length),
          maxLength16(otherStringSpan.maxLength16), maxLength8(otherStringSpan.maxLength8),
          all(സത്യം) {
    എങ്കിൽ(otherStringSpan.pSpanNotSet==&otherStringSpan.spanSet) {
        pSpanNotSet=&spanSet;
    } അല്ലെങ്കിൽ {
        pSpanNotSet=otherStringSpan.pSpanNotSet->clone();
    }

    // Allocate a block of meta data.
    // UTF-8 lengths, 4 sets of span lengths, UTF-8 strings.
    int32_t stringsLength=strings.size();
    int32_t allocSize=stringsLength*(4+1+1+1+1)+utf8Length;
    എങ്കിൽ (allocSize <= static_cast<int32_t>(വലുപ്പം(staticLengths))) {
        utf8Lengths=staticLengths;
    } അല്ലെങ്കിൽ {
        utf8Lengths = static_cast<int32_t*>(uprv_malloc(allocSize));
        എങ്കിൽ(utf8Lengths==nullptr) {
            maxLength16=maxLength8=0;  // Prevent usage by making needsStringSpanUTF16/8() return false.
            തിരികെ;  // Out of memory.
        }
    }

    spanLengths = reinterpret_cast<uint8_t*>(utf8Lengths + stringsLength);
    utf8=spanLengths+stringsLength*4;
    uprv_memcpy(utf8Lengths, otherStringSpan.utf8Lengths, allocSize);
}

UnicodeSetStringSpan::~UnicodeSetStringSpan() {
    എങ്കിൽ(pSpanNotSet!=nullptr && pSpanNotSet!=&spanSet) {
        delete pSpanNotSet;
    }
    എങ്കിൽ(utf8Lengths!=nullptr && utf8Lengths!=staticLengths) {
        uprv_free(utf8Lengths);
    }
}

ശൂന്യം UnicodeSetStringSpan::addToSpanNotSet(UChar32 c) {
    എങ്കിൽ(pSpanNotSet==nullptr || pSpanNotSet==&spanSet) {
        എങ്കിൽ(spanSet.contains(c)) {
            തിരികെ;  // Nothing to do.
        }
        UnicodeSet *newSet=spanSet.cloneAsThawed();
        എങ്കിൽ(newSet==nullptr) {
            തിരികെ;  // Out of memory.
        } അല്ലെങ്കിൽ {
            pSpanNotSet=newSet;
        }
    }
    pSpanNotSet->add(c);
}

// Compare strings without any argument checks. Requires length>0.
സ്ഥിതം വരിയിൽ UBool
matches16(സ്ഥിരം char16_t *s, സ്ഥിരം char16_t *t, int32_t length) {
    ചെയ്യുക {
        എങ്കിൽ(*s++!=*t++) {
            തിരികെ അസത്യം;
        }
    } ആകുമ്പോൾ(--length>0);
    തിരികെ സത്യം;
}

സ്ഥിതം വരിയിൽ UBool
matches8(സ്ഥിരം uint8_t *s, സ്ഥിരം uint8_t *t, int32_t length) {
    ചെയ്യുക {
        എങ്കിൽ(*s++!=*t++) {
            തിരികെ അസത്യം;
        }
    } ആകുമ്പോൾ(--length>0);
    തിരികെ സത്യം;
}

// Compare 16-bit Unicode strings (which may be malformed UTF-16)
// at code point boundaries.
// That is, each edge of a match must not be in the middle of a surrogate pair.
സ്ഥിതം വരിയിൽ UBool
matches16CPB(സ്ഥിരം char16_t *s, int32_t start, int32_t limit, സ്ഥിരം char16_t *t, int32_t length) {
    s+=start;
    limit-=start;
    തിരികെ matches16(s, t, length) &&
           !(0<start && U16_IS_LEAD(s[-1]) && U16_IS_TRAIL(s[0])) &&
           !(length<limit && U16_IS_LEAD(s[length-1]) && U16_IS_TRAIL(s[length]));
}

// Does the set contain the next code point?
// If so, return its length; otherwise return its negative length.
സ്ഥിതം വരിയിൽ int32_t
spanOne(സ്ഥിരം UnicodeSet &set, സ്ഥിരം char16_t *s, int32_t length) {
    char16_t c=*s, c2;
    എങ്കിൽ(c>=0xd800 && c<=0xdbff && length>=2 && U16_IS_TRAIL(c2=s[1])) {
        തിരികെ set.contains(U16_GET_SUPPLEMENTARY(c, c2)) ? 2 : -2;
    }
    തിരികെ set.contains(c) ? 1 : -1;
}

സ്ഥിതം വരിയിൽ int32_t
spanOneBack(സ്ഥിരം UnicodeSet &set, സ്ഥിരം char16_t *s, int32_t length) {
    char16_t c=s[length-1], c2;
    എങ്കിൽ(c>=0xdc00 && c<=0xdfff && length>=2 && U16_IS_LEAD(c2=s[length-2])) {
        തിരികെ set.contains(U16_GET_SUPPLEMENTARY(c2, c)) ? 2 : -2;
    }
    തിരികെ set.contains(c) ? 1 : -1;
}

സ്ഥിതം വരിയിൽ int32_t
spanOneUTF8(സ്ഥിരം UnicodeSet &set, സ്ഥിരം uint8_t *s, int32_t length) {
    UChar32 c=*s;
    എങ്കിൽ(U8_IS_SINGLE(c)) {
        തിരികെ set.contains(c) ? 1 : -1;
    }
    // Take advantage of non-ASCII fastpaths in U8_NEXT_OR_FFFD().
    int32_t i=0;
    U8_NEXT_OR_FFFD(s, i, length, c);
    തിരികെ set.contains(c) ? i : -i;
}

സ്ഥിതം വരിയിൽ int32_t
spanOneBackUTF8(സ്ഥിരം UnicodeSet &set, സ്ഥിരം uint8_t *s, int32_t length) {
    UChar32 c=s[length-1];
    എങ്കിൽ(U8_IS_SINGLE(c)) {
        തിരികെ set.contains(c) ? 1 : -1;
    }
    int32_t i=length-1;
    c=utf8_prevCharSafeBody(s, 0, &i, c, -3);
    length-=i;
    തിരികെ set.contains(c) ? length : -length;
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

int32_t UnicodeSetStringSpan::span(സ്ഥിരം char16_t *s, int32_t length, USetSpanCondition spanCondition) സ്ഥിരം {
    എങ്കിൽ(spanCondition==USET_SPAN_NOT_CONTAINED) {
        തിരികെ spanNot(s, length);
    }
    int32_t spanLength=spanSet.span(s, length, USET_SPAN_CONTAINED);
    എങ്കിൽ(spanLength==length) {
        തിരികെ length;
    }

    // Consider strings; they may overlap with the span.
    OffsetList offsets;
    എങ്കിൽ(spanCondition==USET_SPAN_CONTAINED) {
        // Use offset list to try all possibilities.
        offsets.setMaxLength(maxLength16);
    }
    int32_t pos=spanLength, rest=length-pos;
    int32_t i, stringsLength=strings.size();
    വേണ്ടി(;;) {
        എങ്കിൽ(spanCondition==USET_SPAN_CONTAINED) {
            വേണ്ടി(i=0; i<stringsLength; ++i) {
                int32_t overlap=spanLengths[i];
                എങ്കിൽ(overlap==ALL_CP_CONTAINED) {
                    തുടരുക;  // Irrelevant string. (Also the empty string.)
                }
                സ്ഥിരം UnicodeString& string = *static_cast<സ്ഥിരം UnicodeString*>(strings.elementAt(i));
                സ്ഥിരം char16_t *s16=string.getBuffer();
                int32_t length16=string.length();
                U_ASSERT(length>0);

                // Try to match this string at pos-overlap..pos.
                എങ്കിൽ(overlap>=LONG_SPAN) {
                    overlap=length16;
                    // While contained: No point matching fully inside the code point span.
                    U16_BACK_1(s16, 0, overlap);  // Length of the string minus the last code point.
                }
                എങ്കിൽ(overlap>spanLength) {
                    overlap=spanLength;
                }
                int32_t inc=length16-overlap;  // Keep overlap+inc==length16.
                വേണ്ടി(;;) {
                    എങ്കിൽ(inc>rest) {
                        നിർത്തുക;
                    }
                    // Try to match if the increment is not listed already.
                    എങ്കിൽ(!offsets.containsOffset(inc) && matches16CPB(s, pos-overlap, length, s16, length16)) {
                        എങ്കിൽ(inc==rest) {
                            തിരികെ length;  // Reached the end of the string.
                        }
                        offsets.addOffset(inc);
                    }
                    എങ്കിൽ(overlap==0) {
                        നിർത്തുക;
                    }
                    --overlap;
                    ++inc;
                }
            }
        } അല്ലെങ്കിൽ /* USET_SPAN_SIMPLE */ {
            int32_t maxInc=0, maxOverlap=0;
            വേണ്ടി(i=0; i<stringsLength; ++i) {
                int32_t overlap=spanLengths[i];
                // For longest match, we do need to try to match even an all-contained string
                // to find the match from the earliest start.

                സ്ഥിരം UnicodeString& string = *static_cast<സ്ഥിരം UnicodeString*>(strings.elementAt(i));
                സ്ഥിരം char16_t *s16=string.getBuffer();
                int32_t length16=string.length();
                എങ്കിൽ (length16==0) {
                    തുടരുക;  // skip the empty string
                }

                // Try to match this string at pos-overlap..pos.
                എങ്കിൽ(overlap>=LONG_SPAN) {
                    overlap=length16;
                    // Longest match: Need to match fully inside the code point span
                    // to find the match from the earliest start.
                }
                എങ്കിൽ(overlap>spanLength) {
                    overlap=spanLength;
                }
                int32_t inc=length16-overlap;  // Keep overlap+inc==length16.
                വേണ്ടി(;;) {
                    എങ്കിൽ(inc>rest || overlap<maxOverlap) {
                        നിർത്തുക;
                    }
                    // Try to match if the string is longer or starts earlier.
                    എങ്കിൽ( (overlap>maxOverlap || /* redundant overlap==maxOverlap && */ inc>maxInc) &&
                        matches16CPB(s, pos-overlap, length, s16, length16)
                    ) {
                        maxInc=inc;  // Longest match from earliest start.
                        maxOverlap=overlap;
                        നിർത്തുക;
                    }
                    --overlap;
                    ++inc;
                }
            }

            എങ്കിൽ(maxInc!=0 || maxOverlap!=0) {
                // Longest-match algorithm, and there was a string match.
                // Simply continue after it.
                pos+=maxInc;
                rest-=maxInc;
                എങ്കിൽ(rest==0) {
                    തിരികെ length;  // Reached the end of the string.
                }
                spanLength=0;  // Match strings from after a string match.
                തുടരുക;
            }
        }
        // Finished trying to match all strings at pos.

        എങ്കിൽ(spanLength!=0 || pos==0) {
            // The position is after an unlimited code point span (spanLength!=0),
            // not after a string match.
            // The only position where spanLength==0 after a span is pos==0.
            // Otherwise, an unlimited code point span is only tried again when no
            // strings match, and if such a non-initial span fails we stop.
            എങ്കിൽ(offsets.isEmpty()) {
                തിരികെ pos;  // No strings matched after a span.
            }
            // Match strings from after the next string match.
        } അല്ലെങ്കിൽ {
            // The position is after a string match (or a single code point).
            എങ്കിൽ(offsets.isEmpty()) {
                // No more strings matched after a previous string match.
                // Try another code point span from after the last string match.
                spanLength=spanSet.span(s+pos, rest, USET_SPAN_CONTAINED);
                എങ്കിൽ( spanLength==rest || // Reached the end of the string, or
                    spanLength==0       // neither strings nor span progressed.
                ) {
                    തിരികെ pos+spanLength;
                }
                pos+=spanLength;
                rest-=spanLength;
                തുടരുക;  // spanLength>0: Match strings from after a span.
            } അല്ലെങ്കിൽ {
                // Try to match only one code point from after a string match if some
                // string matched beyond it, so that we try all possible positions
                // and don't overshoot.
                spanLength=spanOne(spanSet, s+pos, rest);
                എങ്കിൽ(spanLength>0) {
                    എങ്കിൽ(spanLength==rest) {
                        തിരികെ length;  // Reached the end of the string.
                    }
                    // Match strings after this code point.
                    // There cannot be any increments below it because UnicodeSet strings
                    // contain multiple code points.
                    pos+=spanLength;
                    rest-=spanLength;
                    offsets.shift(spanLength);
                    spanLength=0;
                    തുടരുക;  // Match strings from after a single code point.
                }
                // Match strings from after the next string match.
            }
        }
        int32_t minOffset=offsets.എടുക്കുക_Minimum();
        pos+=minOffset;
        rest-=minOffset;
        spanLength=0;  // Match strings from after a string match.
    }
}

int32_t UnicodeSetStringSpan::spanBack(സ്ഥിരം char16_t *s, int32_t length, USetSpanCondition spanCondition) സ്ഥിരം {
    എങ്കിൽ(spanCondition==USET_SPAN_NOT_CONTAINED) {
        തിരികെ spanNotBack(s, length);
    }
    int32_t pos=spanSet.spanBack(s, length, USET_SPAN_CONTAINED);
    എങ്കിൽ(pos==0) {
        തിരികെ 0;
    }
    int32_t spanLength=length-pos;

    // Consider strings; they may overlap with the span.
    OffsetList offsets;
    എങ്കിൽ(spanCondition==USET_SPAN_CONTAINED) {
        // Use offset list to try all possibilities.
        offsets.setMaxLength(maxLength16);
    }
    int32_t i, stringsLength=strings.size();
    uint8_t *spanBackLengths=spanLengths;
    എങ്കിൽ(all) {
        spanBackLengths+=stringsLength;
    }
    വേണ്ടി(;;) {
        എങ്കിൽ(spanCondition==USET_SPAN_CONTAINED) {
            വേണ്ടി(i=0; i<stringsLength; ++i) {
                int32_t overlap=spanBackLengths[i];
                എങ്കിൽ(overlap==ALL_CP_CONTAINED) {
                    തുടരുക;  // Irrelevant string. (Also the empty string.)
                }
                സ്ഥിരം UnicodeString& string = *static_cast<സ്ഥിരം UnicodeString*>(strings.elementAt(i));
                സ്ഥിരം char16_t *s16=string.getBuffer();
                int32_t length16=string.length();
                U_ASSERT(length>0);

                // Try to match this string at pos-(length16-overlap)..pos-length16.
                എങ്കിൽ(overlap>=LONG_SPAN) {
                    overlap=length16;
                    // While contained: No point matching fully inside the code point span.
                    int32_t len1=0;
                    U16_FWD_1(s16, len1, overlap);
                    overlap-=len1;  // Length of the string minus the first code point.
                }
                എങ്കിൽ(overlap>spanLength) {
                    overlap=spanLength;
                }
                int32_t dec=length16-overlap;  // Keep dec+overlap==length16.
                വേണ്ടി(;;) {
                    എങ്കിൽ(dec>pos) {
                        നിർത്തുക;
                    }
                    // Try to match if the decrement is not listed already.
                    എങ്കിൽ(!offsets.containsOffset(dec) && matches16CPB(s, pos-dec, length, s16, length16)) {
                        എങ്കിൽ(dec==pos) {
                            തിരികെ 0;  // Reached the start of the string.
                        }
                        offsets.addOffset(dec);
                    }
                    എങ്കിൽ(overlap==0) {
                        നിർത്തുക;
                    }
                    --overlap;
                    ++dec;
                }
            }
        } അല്ലെങ്കിൽ /* USET_SPAN_SIMPLE */ {
            int32_t maxDec=0, maxOverlap=0;
            വേണ്ടി(i=0; i<stringsLength; ++i) {
                int32_t overlap=spanBackLengths[i];
                // For longest match, we do need to try to match even an all-contained string
                // to find the match from the latest end.

                സ്ഥിരം UnicodeString& string = *static_cast<സ്ഥിരം UnicodeString*>(strings.elementAt(i));
                സ്ഥിരം char16_t *s16=string.getBuffer();
                int32_t length16=string.length();
                എങ്കിൽ (length16==0) {
                    തുടരുക;  // skip the empty string
                }

                // Try to match this string at pos-(length16-overlap)..pos-length16.
                എങ്കിൽ(overlap>=LONG_SPAN) {
                    overlap=length16;
                    // Longest match: Need to match fully inside the code point span
                    // to find the match from the latest end.
                }
                എങ്കിൽ(overlap>spanLength) {
                    overlap=spanLength;
                }
                int32_t dec=length16-overlap;  // Keep dec+overlap==length16.
                വേണ്ടി(;;) {
                    എങ്കിൽ(dec>pos || overlap<maxOverlap) {
                        നിർത്തുക;
                    }
                    // Try to match if the string is longer or ends later.
                    എങ്കിൽ( (overlap>maxOverlap || /* redundant overlap==maxOverlap && */ dec>maxDec) &&
                        matches16CPB(s, pos-dec, length, s16, length16)
                    ) {
                        maxDec=dec;  // Longest match from latest end.
                        maxOverlap=overlap;
                        നിർത്തുക;
                    }
                    --overlap;
                    ++dec;
                }
            }

            എങ്കിൽ(maxDec!=0 || maxOverlap!=0) {
                // Longest-match algorithm, and there was a string match.
                // Simply continue before it.
                pos-=maxDec;
                എങ്കിൽ(pos==0) {
                    തിരികെ 0;  // Reached the start of the string.
                }
                spanLength=0;  // Match strings from before a string match.
                തുടരുക;
            }
        }
        // Finished trying to match all strings at pos.

        എങ്കിൽ(spanLength!=0 || pos==length) {
            // The position is before an unlimited code point span (spanLength!=0),
            // not before a string match.
            // The only position where spanLength==0 before a span is pos==length.
            // Otherwise, an unlimited code point span is only tried again when no
            // strings match, and if such a non-initial span fails we stop.
            എങ്കിൽ(offsets.isEmpty()) {
                തിരികെ pos;  // No strings matched before a span.
            }
            // Match strings from before the next string match.
        } അല്ലെങ്കിൽ {
            // The position is before a string match (or a single code point).
            എങ്കിൽ(offsets.isEmpty()) {
                // No more strings matched before a previous string match.
                // Try another code point span from before the last string match.
                int32_t oldPos=pos;
                pos=spanSet.spanBack(s, oldPos, USET_SPAN_CONTAINED);
                spanLength=oldPos-pos;
                എങ്കിൽ( pos==0 ||           // Reached the start of the string, or
                    spanLength==0       // neither strings nor span progressed.
                ) {
                    തിരികെ pos;
                }
                തുടരുക;  // spanLength>0: Match strings from before a span.
            } അല്ലെങ്കിൽ {
                // Try to match only one code point from before a string match if some
                // string matched beyond it, so that we try all possible positions
                // and don't overshoot.
                spanLength=spanOneBack(spanSet, s, pos);
                എങ്കിൽ(spanLength>0) {
                    എങ്കിൽ(spanLength==pos) {
                        തിരികെ 0;  // Reached the start of the string.
                    }
                    // Match strings before this code point.
                    // There cannot be any decrements below it because UnicodeSet strings
                    // contain multiple code points.
                    pos-=spanLength;
                    offsets.shift(spanLength);
                    spanLength=0;
                    തുടരുക;  // Match strings from before a single code point.
                }
                // Match strings from before the next string match.
            }
        }
        pos-=offsets.എടുക്കുക_Minimum();
        spanLength=0;  // Match strings from before a string match.
    }
}

int32_t UnicodeSetStringSpan::spanUTF8(സ്ഥിരം uint8_t *s, int32_t length, USetSpanCondition spanCondition) സ്ഥിരം {
    എങ്കിൽ(spanCondition==USET_SPAN_NOT_CONTAINED) {
        തിരികെ spanNotUTF8(s, length);
    }
    int32_t spanLength = spanSet.spanUTF8(reinterpret_cast<സ്ഥിരം അക്ഷരം*>(s), length, USET_SPAN_CONTAINED);
    എങ്കിൽ(spanLength==length) {
        തിരികെ length;
    }

    // Consider strings; they may overlap with the span.
    OffsetList offsets;
    എങ്കിൽ(spanCondition==USET_SPAN_CONTAINED) {
        // Use offset list to try all possibilities.
        offsets.setMaxLength(maxLength8);
    }
    int32_t pos=spanLength, rest=length-pos;
    int32_t i, stringsLength=strings.size();
    uint8_t *spanUTF8Lengths=spanLengths;
    എങ്കിൽ(all) {
        spanUTF8Lengths+=2*stringsLength;
    }
    വേണ്ടി(;;) {
        സ്ഥിരം uint8_t *s8=utf8;
        int32_t length8;
        എങ്കിൽ(spanCondition==USET_SPAN_CONTAINED) {
            വേണ്ടി(i=0; i<stringsLength; ++i) {
                length8=utf8Lengths[i];
                എങ്കിൽ(length8==0) {
                    തുടരുക;  // String not representable in UTF-8.
                }
                int32_t overlap=spanUTF8Lengths[i];
                എങ്കിൽ(overlap==ALL_CP_CONTAINED) {
                    s8+=length8;
                    തുടരുക;  // Irrelevant string.
                }

                // Try to match this string at pos-overlap..pos.
                എങ്കിൽ(overlap>=LONG_SPAN) {
                    overlap=length8;
                    // While contained: No point matching fully inside the code point span.
                    U8_BACK_1(s8, 0, overlap);  // Length of the string minus the last code point.
                }
                എങ്കിൽ(overlap>spanLength) {
                    overlap=spanLength;
                }
                int32_t inc=length8-overlap;  // Keep overlap+inc==length8.
                വേണ്ടി(;;) {
                    എങ്കിൽ(inc>rest) {
                        നിർത്തുക;
                    }
                    // Try to match if the increment is not listed already.
                    // Match at code point boundaries. (The UTF-8 strings were converted
                    // from UTF-16 and are guaranteed to be well-formed.)
                    എങ്കിൽ(!U8_IS_TRAIL(s[pos-overlap]) &&
                            !offsets.containsOffset(inc) &&
                            matches8(s+pos-overlap, s8, length8)) {
                        എങ്കിൽ(inc==rest) {
                            തിരികെ length;  // Reached the end of the string.
                        }
                        offsets.addOffset(inc);
                    }
                    എങ്കിൽ(overlap==0) {
                        നിർത്തുക;
                    }
                    --overlap;
                    ++inc;
                }
                s8+=length8;
            }
        } അല്ലെങ്കിൽ /* USET_SPAN_SIMPLE */ {
            int32_t maxInc=0, maxOverlap=0;
            വേണ്ടി(i=0; i<stringsLength; ++i) {
                length8=utf8Lengths[i];
                എങ്കിൽ(length8==0) {
                    തുടരുക;  // String not representable in UTF-8.
                }
                int32_t overlap=spanUTF8Lengths[i];
                // For longest match, we do need to try to match even an all-contained string
                // to find the match from the earliest start.

                // Try to match this string at pos-overlap..pos.
                എങ്കിൽ(overlap>=LONG_SPAN) {
                    overlap=length8;
                    // Longest match: Need to match fully inside the code point span
                    // to find the match from the earliest start.
                }
                എങ്കിൽ(overlap>spanLength) {
                    overlap=spanLength;
                }
                int32_t inc=length8-overlap;  // Keep overlap+inc==length8.
                വേണ്ടി(;;) {
                    എങ്കിൽ(inc>rest || overlap<maxOverlap) {
                        നിർത്തുക;
                    }
                    // Try to match if the string is longer or starts earlier.
                    // Match at code point boundaries. (The UTF-8 strings were converted
                    // from UTF-16 and are guaranteed to be well-formed.)
                    എങ്കിൽ(!U8_IS_TRAIL(s[pos-overlap]) &&
                            (overlap>maxOverlap ||
                                /* redundant overlap==maxOverlap && */ inc>maxInc) &&
                            matches8(s+pos-overlap, s8, length8)) {
                        maxInc=inc;  // Longest match from earliest start.
                        maxOverlap=overlap;
                        നിർത്തുക;
                    }
                    --overlap;
                    ++inc;
                }
                s8+=length8;
            }

            എങ്കിൽ(maxInc!=0 || maxOverlap!=0) {
                // Longest-match algorithm, and there was a string match.
                // Simply continue after it.
                pos+=maxInc;
                rest-=maxInc;
                എങ്കിൽ(rest==0) {
                    തിരികെ length;  // Reached the end of the string.
                }
                spanLength=0;  // Match strings from after a string match.
                തുടരുക;
            }
        }
        // Finished trying to match all strings at pos.

        എങ്കിൽ(spanLength!=0 || pos==0) {
            // The position is after an unlimited code point span (spanLength!=0),
            // not after a string match.
            // The only position where spanLength==0 after a span is pos==0.
            // Otherwise, an unlimited code point span is only tried again when no
            // strings match, and if such a non-initial span fails we stop.
            എങ്കിൽ(offsets.isEmpty()) {
                തിരികെ pos;  // No strings matched after a span.
            }
            // Match strings from after the next string match.
        } അല്ലെങ്കിൽ {
            // The position is after a string match (or a single code point).
            എങ്കിൽ(offsets.isEmpty()) {
                // No more strings matched after a previous string match.
                // Try another code point span from after the last string match.
                spanLength = spanSet.spanUTF8(reinterpret_cast<സ്ഥിരം അക്ഷരം*>(s) + pos, rest, USET_SPAN_CONTAINED);
                എങ്കിൽ( spanLength==rest || // Reached the end of the string, or
                    spanLength==0       // neither strings nor span progressed.
                ) {
                    തിരികെ pos+spanLength;
                }
                pos+=spanLength;
                rest-=spanLength;
                തുടരുക;  // spanLength>0: Match strings from after a span.
            } അല്ലെങ്കിൽ {
                // Try to match only one code point from after a string match if some
                // string matched beyond it, so that we try all possible positions
                // and don't overshoot.
                spanLength=spanOneUTF8(spanSet, s+pos, rest);
                എങ്കിൽ(spanLength>0) {
                    എങ്കിൽ(spanLength==rest) {
                        തിരികെ length;  // Reached the end of the string.
                    }
                    // Match strings after this code point.
                    // There cannot be any increments below it because UnicodeSet strings
                    // contain multiple code points.
                    pos+=spanLength;
                    rest-=spanLength;
                    offsets.shift(spanLength);
                    spanLength=0;
                    തുടരുക;  // Match strings from after a single code point.
                }
                // Match strings from after the next string match.
            }
        }
        int32_t minOffset=offsets.എടുക്കുക_Minimum();
        pos+=minOffset;
        rest-=minOffset;
        spanLength=0;  // Match strings from after a string match.
    }
}

int32_t UnicodeSetStringSpan::spanBackUTF8(സ്ഥിരം uint8_t *s, int32_t length, USetSpanCondition spanCondition) സ്ഥിരം {
    എങ്കിൽ(spanCondition==USET_SPAN_NOT_CONTAINED) {
        തിരികെ spanNotBackUTF8(s, length);
    }
    int32_t pos = spanSet.spanBackUTF8(reinterpret_cast<സ്ഥിരം അക്ഷരം*>(s), length, USET_SPAN_CONTAINED);
    എങ്കിൽ(pos==0) {
        തിരികെ 0;
    }
    int32_t spanLength=length-pos;

    // Consider strings; they may overlap with the span.
    OffsetList offsets;
    എങ്കിൽ(spanCondition==USET_SPAN_CONTAINED) {
        // Use offset list to try all possibilities.
        offsets.setMaxLength(maxLength8);
    }
    int32_t i, stringsLength=strings.size();
    uint8_t *spanBackUTF8Lengths=spanLengths;
    എങ്കിൽ(all) {
        spanBackUTF8Lengths+=3*stringsLength;
    }
    വേണ്ടി(;;) {
        സ്ഥിരം uint8_t *s8=utf8;
        int32_t length8;
        എങ്കിൽ(spanCondition==USET_SPAN_CONTAINED) {
            വേണ്ടി(i=0; i<stringsLength; ++i) {
                length8=utf8Lengths[i];
                എങ്കിൽ(length8==0) {
                    തുടരുക;  // String not representable in UTF-8.
                }
                int32_t overlap=spanBackUTF8Lengths[i];
                എങ്കിൽ(overlap==ALL_CP_CONTAINED) {
                    s8+=length8;
                    തുടരുക;  // Irrelevant string.
                }

                // Try to match this string at pos-(length8-overlap)..pos-length8.
                എങ്കിൽ(overlap>=LONG_SPAN) {
                    overlap=length8;
                    // While contained: No point matching fully inside the code point span.
                    int32_t len1=0;
                    U8_FWD_1(s8, len1, overlap);
                    overlap-=len1;  // Length of the string minus the first code point.
                }
                എങ്കിൽ(overlap>spanLength) {
                    overlap=spanLength;
                }
                int32_t dec=length8-overlap;  // Keep dec+overlap==length8.
                വേണ്ടി(;;) {
                    എങ്കിൽ(dec>pos) {
                        നിർത്തുക;
                    }
                    // Try to match if the decrement is not listed already.
                    // Match at code point boundaries. (The UTF-8 strings were converted
                    // from UTF-16 and are guaranteed to be well-formed.)
                    എങ്കിൽ( !U8_IS_TRAIL(s[pos-dec]) &&
                        !offsets.containsOffset(dec) &&
                        matches8(s+pos-dec, s8, length8)
                    ) {
                        എങ്കിൽ(dec==pos) {
                            തിരികെ 0;  // Reached the start of the string.
                        }
                        offsets.addOffset(dec);
                    }
                    എങ്കിൽ(overlap==0) {
                        നിർത്തുക;
                    }
                    --overlap;
                    ++dec;
                }
                s8+=length8;
            }
        } അല്ലെങ്കിൽ /* USET_SPAN_SIMPLE */ {
            int32_t maxDec=0, maxOverlap=0;
            വേണ്ടി(i=0; i<stringsLength; ++i) {
                length8=utf8Lengths[i];
                എങ്കിൽ(length8==0) {
                    തുടരുക;  // String not representable in UTF-8.
                }
                int32_t overlap=spanBackUTF8Lengths[i];
                // For longest match, we do need to try to match even an all-contained string
                // to find the match from the latest end.

                // Try to match this string at pos-(length8-overlap)..pos-length8.
                എങ്കിൽ(overlap>=LONG_SPAN) {
                    overlap=length8;
                    // Longest match: Need to match fully inside the code point span
                    // to find the match from the latest end.
                }
                എങ്കിൽ(overlap>spanLength) {
                    overlap=spanLength;
                }
                int32_t dec=length8-overlap;  // Keep dec+overlap==length8.
                വേണ്ടി(;;) {
                    എങ്കിൽ(dec>pos || overlap<maxOverlap) {
                        നിർത്തുക;
                    }
                    // Try to match if the string is longer or ends later.
                    // Match at code point boundaries. (The UTF-8 strings were converted
                    // from UTF-16 and are guaranteed to be well-formed.)
                    എങ്കിൽ( !U8_IS_TRAIL(s[pos-dec]) &&
                        (overlap>maxOverlap || /* redundant overlap==maxOverlap && */ dec>maxDec) &&
                        matches8(s+pos-dec, s8, length8)
                    ) {
                        maxDec=dec;  // Longest match from latest end.
                        maxOverlap=overlap;
                        നിർത്തുക;
                    }
                    --overlap;
                    ++dec;
                }
                s8+=length8;
            }

            എങ്കിൽ(maxDec!=0 || maxOverlap!=0) {
                // Longest-match algorithm, and there was a string match.
                // Simply continue before it.
                pos-=maxDec;
                എങ്കിൽ(pos==0) {
                    തിരികെ 0;  // Reached the start of the string.
                }
                spanLength=0;  // Match strings from before a string match.
                തുടരുക;
            }
        }
        // Finished trying to match all strings at pos.

        എങ്കിൽ(spanLength!=0 || pos==length) {
            // The position is before an unlimited code point span (spanLength!=0),
            // not before a string match.
            // The only position where spanLength==0 before a span is pos==length.
            // Otherwise, an unlimited code point span is only tried again when no
            // strings match, and if such a non-initial span fails we stop.
            എങ്കിൽ(offsets.isEmpty()) {
                തിരികെ pos;  // No strings matched before a span.
            }
            // Match strings from before the next string match.
        } അല്ലെങ്കിൽ {
            // The position is before a string match (or a single code point).
            എങ്കിൽ(offsets.isEmpty()) {
                // No more strings matched before a previous string match.
                // Try another code point span from before the last string match.
                int32_t oldPos=pos;
                pos = spanSet.spanBackUTF8(reinterpret_cast<സ്ഥിരം അക്ഷരം*>(s), oldPos, USET_SPAN_CONTAINED);
                spanLength=oldPos-pos;
                എങ്കിൽ( pos==0 ||           // Reached the start of the string, or
                    spanLength==0       // neither strings nor span progressed.
                ) {
                    തിരികെ pos;
                }
                തുടരുക;  // spanLength>0: Match strings from before a span.
            } അല്ലെങ്കിൽ {
                // Try to match only one code point from before a string match if some
                // string matched beyond it, so that we try all possible positions
                // and don't overshoot.
                spanLength=spanOneBackUTF8(spanSet, s, pos);
                എങ്കിൽ(spanLength>0) {
                    എങ്കിൽ(spanLength==pos) {
                        തിരികെ 0;  // Reached the start of the string.
                    }
                    // Match strings before this code point.
                    // There cannot be any decrements below it because UnicodeSet strings
                    // contain multiple code points.
                    pos-=spanLength;
                    offsets.shift(spanLength);
                    spanLength=0;
                    തുടരുക;  // Match strings from before a single code point.
                }
                // Match strings from before the next string match.
            }
        }
        pos-=offsets.എടുക്കുക_Minimum();
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

int32_t UnicodeSetStringSpan::spanNot(സ്ഥിരം char16_t *s, int32_t length) സ്ഥിരം {
    int32_t pos=0, rest=length;
    int32_t i, stringsLength=strings.size();
    ചെയ്യുക {
        // Span until we find a code point from the set,
        // or a code point that starts or ends some string.
        i=pSpanNotSet->span(s+pos, rest, USET_SPAN_NOT_CONTAINED);
        എങ്കിൽ(i==rest) {
            തിരികെ length;  // Reached the end of the string.
        }
        pos+=i;
        rest-=i;

        // Check whether the current code point is in the original set,
        // without the string starts and ends.
        int32_t cpLength=spanOne(spanSet, s+pos, rest);
        എങ്കിൽ(cpLength>0) {
            തിരികെ pos;  // There is a set element at pos.
        }

        // Try to match the strings at pos.
        വേണ്ടി(i=0; i<stringsLength; ++i) {
            എങ്കിൽ(spanLengths[i]==ALL_CP_CONTAINED) {
                തുടരുക;  // Irrelevant string. (Also the empty string.)
            }
            സ്ഥിരം UnicodeString& string = *static_cast<സ്ഥിരം UnicodeString*>(strings.elementAt(i));
            സ്ഥിരം char16_t *s16=string.getBuffer();
            int32_t length16=string.length();
            U_ASSERT(length>0);
            എങ്കിൽ(length16<=rest && matches16CPB(s, pos, length, s16, length16)) {
                തിരികെ pos;  // There is a set element at pos.
            }
        }

        // The span(while not contained) ended on a string start/end which is
        // not in the original set. Skip this code point and continue.
        // cpLength<0
        pos-=cpLength;
        rest+=cpLength;
    } ആകുമ്പോൾ(rest!=0);
    തിരികെ length;  // Reached the end of the string.
}

int32_t UnicodeSetStringSpan::spanNotBack(സ്ഥിരം char16_t *s, int32_t length) സ്ഥിരം {
    int32_t pos=length;
    int32_t i, stringsLength=strings.size();
    ചെയ്യുക {
        // Span until we find a code point from the set,
        // or a code point that starts or ends some string.
        pos=pSpanNotSet->spanBack(s, pos, USET_SPAN_NOT_CONTAINED);
        എങ്കിൽ(pos==0) {
            തിരികെ 0;  // Reached the start of the string.
        }

        // Check whether the current code point is in the original set,
        // without the string starts and ends.
        int32_t cpLength=spanOneBack(spanSet, s, pos);
        എങ്കിൽ(cpLength>0) {
            തിരികെ pos;  // There is a set element at pos.
        }

        // Try to match the strings at pos.
        വേണ്ടി(i=0; i<stringsLength; ++i) {
            // Use spanLengths rather than a spanBackLengths pointer because
            // it is easier and we only need to know whether the string is irrelevant
            // which is the same in either array.
            എങ്കിൽ(spanLengths[i]==ALL_CP_CONTAINED) {
                തുടരുക;  // Irrelevant string. (Also the empty string.)
            }
            സ്ഥിരം UnicodeString& string = *static_cast<സ്ഥിരം UnicodeString*>(strings.elementAt(i));
            സ്ഥിരം char16_t *s16=string.getBuffer();
            int32_t length16=string.length();
            U_ASSERT(length>0);
            എങ്കിൽ(length16<=pos && matches16CPB(s, pos-length16, length, s16, length16)) {
                തിരികെ pos;  // There is a set element at pos.
            }
        }

        // The span(while not contained) ended on a string start/end which is
        // not in the original set. Skip this code point and continue.
        // cpLength<0
        pos+=cpLength;
    } ആകുമ്പോൾ(pos!=0);
    തിരികെ 0;  // Reached the start of the string.
}

int32_t UnicodeSetStringSpan::spanNotUTF8(സ്ഥിരം uint8_t *s, int32_t length) സ്ഥിരം {
    int32_t pos=0, rest=length;
    int32_t i, stringsLength=strings.size();
    uint8_t *spanUTF8Lengths=spanLengths;
    എങ്കിൽ(all) {
        spanUTF8Lengths+=2*stringsLength;
    }
    ചെയ്യുക {
        // Span until we find a code point from the set,
        // or a code point that starts or ends some string.
        i = pSpanNotSet->spanUTF8(reinterpret_cast<സ്ഥിരം അക്ഷരം*>(s) + pos, rest, USET_SPAN_NOT_CONTAINED);
        എങ്കിൽ(i==rest) {
            തിരികെ length;  // Reached the end of the string.
        }
        pos+=i;
        rest-=i;

        // Check whether the current code point is in the original set,
        // without the string starts and ends.
        int32_t cpLength=spanOneUTF8(spanSet, s+pos, rest);
        എങ്കിൽ(cpLength>0) {
            തിരികെ pos;  // There is a set element at pos.
        }

        // Try to match the strings at pos.
        സ്ഥിരം uint8_t *s8=utf8;
        int32_t length8;
        വേണ്ടി(i=0; i<stringsLength; ++i) {
            length8=utf8Lengths[i];
            // ALL_CP_CONTAINED: Irrelevant string.
            എങ്കിൽ(length8!=0 && spanUTF8Lengths[i]!=ALL_CP_CONTAINED && length8<=rest && matches8(s+pos, s8, length8)) {
                തിരികെ pos;  // There is a set element at pos.
            }
            s8+=length8;
        }

        // The span(while not contained) ended on a string start/end which is
        // not in the original set. Skip this code point and continue.
        // cpLength<0
        pos-=cpLength;
        rest+=cpLength;
    } ആകുമ്പോൾ(rest!=0);
    തിരികെ length;  // Reached the end of the string.
}

int32_t UnicodeSetStringSpan::spanNotBackUTF8(സ്ഥിരം uint8_t *s, int32_t length) സ്ഥിരം {
    int32_t pos=length;
    int32_t i, stringsLength=strings.size();
    uint8_t *spanBackUTF8Lengths=spanLengths;
    എങ്കിൽ(all) {
        spanBackUTF8Lengths+=3*stringsLength;
    }
    ചെയ്യുക {
        // Span until we find a code point from the set,
        // or a code point that starts or ends some string.
        pos = pSpanNotSet->spanBackUTF8(reinterpret_cast<സ്ഥിരം അക്ഷരം*>(s), pos, USET_SPAN_NOT_CONTAINED);
        എങ്കിൽ(pos==0) {
            തിരികെ 0;  // Reached the start of the string.
        }

        // Check whether the current code point is in the original set,
        // without the string starts and ends.
        int32_t cpLength=spanOneBackUTF8(spanSet, s, pos);
        എങ്കിൽ(cpLength>0) {
            തിരികെ pos;  // There is a set element at pos.
        }

        // Try to match the strings at pos.
        സ്ഥിരം uint8_t *s8=utf8;
        int32_t length8;
        വേണ്ടി(i=0; i<stringsLength; ++i) {
            length8=utf8Lengths[i];
            // ALL_CP_CONTAINED: Irrelevant string.
            എങ്കിൽ(length8!=0 && spanBackUTF8Lengths[i]!=ALL_CP_CONTAINED && length8<=pos && matches8(s+pos-length8, s8, length8)) {
                തിരികെ pos;  // There is a set element at pos.
            }
            s8+=length8;
        }

        // The span(while not contained) ended on a string start/end which is
        // not in the original set. Skip this code point and continue.
        // cpLength<0
        pos+=cpLength;
    } ആകുമ്പോൾ(pos!=0);
    തിരികെ 0;  // Reached the start of the string.
}

U_NAMESPACE_END
