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

#ضمن "unicode/utypes.h"
#ضمن "unicode/uniset.h"
#ضمن "unicode/ustring.h"
#ضمن "unicode/utf8.h"
#ضمن "unicode/utf16.h"
#ضمن "cmemory.h"
#ضمن "uvector.h"
#ضمن "unisetspan.h"

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
صنف OffsetList {  // Only ever stack-allocated, does not need to inherit UMemory.
عام:
    OffsetList() : list(staticList), capacity(0), length(0), start(0) {}

    ~OffsetList() {
        𐡄𐡍(list!=staticList) {
            uprv_free(list);
        }
    }

    // Call exactly once if the list is to be used.
    فراغ setMaxLength(int32_t maxLength) {
        𐡄𐡍 (maxLength <= static_cast<int32_t>(حجم(staticList))) {
            capacity = static_cast<int32_t>(حجم(staticList));
        } وإلا {
            UBool* l = static_cast<UBool*>(uprv_malloc(maxLength));
            𐡄𐡍(l!=nullptr) {
                list=l;
                capacity=maxLength;
            }
        }
        uprv_memset(list, 0, capacity);
    }

    فراغ clear() {
        uprv_memset(list, 0, capacity);
        start=length=0;
    }

    UBool isEmpty() ثابت {
        أرجع length == 0;
    }

    // Reduce all stored offsets by delta, used when the current position
    // moves by delta.
    // There must not be any offsets lower than delta.
    // If there is an offset equal to delta, it is removed.
    // delta=[1..maxLength]
    فراغ shift(int32_t delta) {
        int32_t i=start+delta;
        𐡄𐡍(i>=capacity) {
            i-=capacity;
        }
        𐡄𐡍(list[i]) {
            list[i]=خطأ;
            --length;
        }
        start=i;
    }

    // Add an offset. The list must not contain it yet.
    // offset=[1..maxLength]
    فراغ addOffset(int32_t offset) {
        int32_t i=start+offset;
        𐡄𐡍(i>=capacity) {
            i-=capacity;
        }
        list[i]=𐡒𐡔𐡈;
        ++length;
    }

    // offset=[1..maxLength]
    UBool containsOffset(int32_t offset) ثابت {
        int32_t i=start+offset;
        𐡄𐡍(i>=capacity) {
            i-=capacity;
        }
        أرجع list[i];
    }

    // Find the lowest stored offset from a non-empty list, remove it,
    // and reduce all other offsets by this minimum.
    // Returns [1..maxLength].
    int32_t popMinimum() {
        // Look for the next offset in list[start+1..capacity-1].
        int32_t i=start, result;
        طالما(++i<capacity) {
            𐡄𐡍(list[i]) {
                list[i]=خطأ;
                --length;
                result=i-start;
                start=i;
                أرجع result;
            }
        }
        // i==capacity

        // Wrap around and look for the next offset in list[0..start].
        // Since the list is not empty, there will be one.
        result=capacity-start;
        i=0;
        طالما(!list[i]) {
            ++i;
        }
        list[i]=خطأ;
        --length;
        start=i;
        أرجع result+=i;
    }

خاص:
    UBool *list;
    int32_t capacity;
    int32_t length;
    int32_t start;

    UBool staticList[16];
};

// Get the number of UTF-8 bytes for a UTF-16 (sub)string.
ساكن int32_t
getUTF8Length(ثابت char16_t *s, int32_t length) {
    UErrorCode errorCode=U_ZERO_ERROR;
    int32_t length8=0;
    u_strToUTF8(nullptr, 0, &length8, s, length, &errorCode);
    𐡄𐡍(U_SUCCESS(errorCode) || errorCode==U_BUFFER_OVERFLOW_ERROR) {
        أرجع length8;
    } وإلا {
        // The string contains an unpaired surrogate.
        // Ignore this string.
        أرجع 0;
    }
}

// Append the UTF-8 version of the string to t and return the appended UTF-8 length.
ساكن int32_t
appendUTF8(ثابت char16_t *s, int32_t length, uint8_t *t, int32_t capacity) {
    UErrorCode errorCode=U_ZERO_ERROR;
    int32_t length8=0;
    u_strToUTF8(reinterpret_cast<حرف*>(t), capacity, &length8, s, length, &errorCode);
    𐡄𐡍(U_SUCCESS(errorCode)) {
        أرجع length8;
    } وإلا {
        // The string contains an unpaired surrogate.
        // Ignore this string.
        أرجع 0;
    }
}

ساكن مضمن uint8_t
makeSpanLengthByte(int32_t spanLength) {
    // 0xfe==UnicodeSetStringSpan::LONG_SPAN
    أرجع spanLength < 0xfe ? static_cast<uint8_t>(spanLength) : static_cast<uint8_t>(0xfe);
}

// Construct for all variants of span(), or only for any one variant.
// Initialize as little as possible, for single use.
UnicodeSetStringSpan::UnicodeSetStringSpan(ثابت UnicodeSet &set,
                                           ثابت UVector &setStrings,
                                           uint32_t which)
        : spanSet(0, 0x10ffff), pSpanNotSet(nullptr), strings(setStrings),
          utf8Lengths(nullptr), spanLengths(nullptr), utf8(nullptr),
          utf8Length(0),
          maxLength16(0), maxLength8(0),
          all(static_cast<UBool>(which == ALL)) {
    spanSet.retainAll(set);
    𐡄𐡍(which&NOT_CONTAINED) {
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
    UBool someRelevant=خطأ;
    لكل(i=0; i<stringsLength; ++i) {
        ثابت UnicodeString& string = *static_cast<ثابت UnicodeString*>(strings.elementAt(i));
        ثابت char16_t *s16=string.getBuffer();
        int32_t length16=string.length();
        𐡄𐡍 (length16==0) {
            استمر;  // skip the empty string
        }
        UBool thisRelevant;
        spanLength=spanSet.span(s16, length16, USET_SPAN_CONTAINED);
        𐡄𐡍(spanLength<length16) {  // Relevant string.
            someRelevant=thisRelevant=𐡒𐡔𐡈;
        } وإلا {
            thisRelevant=خطأ;
        }
        𐡄𐡍((which&UTF16) && length16>maxLength16) {
            maxLength16=length16;
        }
        𐡄𐡍((which&UTF8) && (thisRelevant || (which&CONTAINED))) {
            int32_t length8=getUTF8Length(s16, length16);
            utf8Length+=length8;
            𐡄𐡍(length8>maxLength8) {
                maxLength8=length8;
            }
        }
    }
    𐡄𐡍(!someRelevant) {
        maxLength16=maxLength8=0;
        أرجع;
    }

    // Freeze after checking for the need to use strings at all because freezing
    // a set takes some time and memory which are wasted if there are no relevant strings.
    𐡄𐡍(all) {
        spanSet.freeze();
    }

    uint8_t *spanBackLengths;
    uint8_t *spanUTF8Lengths;
    uint8_t *spanBackUTF8Lengths;

    // Allocate a block of meta data.
    int32_t allocSize;
    𐡄𐡍(all) {
        // UTF-8 lengths, 4 sets of span lengths, UTF-8 strings.
        allocSize=stringsLength*(4+1+1+1+1)+utf8Length;
    } وإلا {
        allocSize=stringsLength;  // One set of span lengths.
        𐡄𐡍(which&UTF8) {
            // UTF-8 lengths and UTF-8 strings.
            allocSize+=stringsLength*4+utf8Length;
        }
    }
    𐡄𐡍 (allocSize <= static_cast<int32_t>(حجم(staticLengths))) {
        utf8Lengths=staticLengths;
    } وإلا {
        utf8Lengths = static_cast<int32_t*>(uprv_malloc(allocSize));
        𐡄𐡍(utf8Lengths==nullptr) {
            maxLength16=maxLength8=0;  // Prevent usage by making needsStringSpanUTF16/8() return false.
            أرجع;  // Out of memory.
        }
    }

    𐡄𐡍(all) {
        // Store span lengths for all span() variants.
        spanLengths = reinterpret_cast<uint8_t*>(utf8Lengths + stringsLength);
        spanBackLengths=spanLengths+stringsLength;
        spanUTF8Lengths=spanBackLengths+stringsLength;
        spanBackUTF8Lengths=spanUTF8Lengths+stringsLength;
        utf8=spanBackUTF8Lengths+stringsLength;
    } وإلا {
        // Store span lengths for only one span() variant.
        𐡄𐡍(which&UTF8) {
            spanLengths = reinterpret_cast<uint8_t*>(utf8Lengths + stringsLength);
            utf8=spanLengths+stringsLength;
        } وإلا {
            spanLengths = reinterpret_cast<uint8_t*>(utf8Lengths);
        }
        spanBackLengths=spanUTF8Lengths=spanBackUTF8Lengths=spanLengths;
    }

    // Set the meta data and pSpanNotSet and write the UTF-8 strings.
    int32_t utf8Count=0;  // Count UTF-8 bytes written so far.

    لكل(i=0; i<stringsLength; ++i) {
        ثابت UnicodeString& string = *static_cast<ثابت UnicodeString*>(strings.elementAt(i));
        ثابت char16_t *s16=string.getBuffer();
        int32_t length16=string.length();
        spanLength=spanSet.span(s16, length16, USET_SPAN_CONTAINED);
        𐡄𐡍(spanLength<length16 && length16>0) {  // Relevant string.
            𐡄𐡍(which&UTF16) {
                𐡄𐡍(which&CONTAINED) {
                    𐡄𐡍(which&FWD) {
                        spanLengths[i]=makeSpanLengthByte(spanLength);
                    }
                    𐡄𐡍(which&BACK) {
                        spanLength=length16-spanSet.spanBack(s16, length16, USET_SPAN_CONTAINED);
                        spanBackLengths[i]=makeSpanLengthByte(spanLength);
                    }
                } وإلا /* not CONTAINED, not all, but NOT_CONTAINED */ {
                    spanLengths[i]=spanBackLengths[i]=0;  // Only store a relevant/irrelevant flag.
                }
            }
            𐡄𐡍(which&UTF8) {
                uint8_t *s8=utf8+utf8Count;
                int32_t length8=appendUTF8(s16, length16, s8, utf8Length-utf8Count);
                utf8Count+=utf8Lengths[i]=length8;
                𐡄𐡍(length8==0) {  // Irrelevant for UTF-8 because not representable in UTF-8.
                    spanUTF8Lengths[i] = spanBackUTF8Lengths[i] = static_cast<uint8_t>(ALL_CP_CONTAINED);
                } وإلا {  // Relevant for UTF-8.
                    𐡄𐡍(which&CONTAINED) {
                        𐡄𐡍(which&FWD) {
                            spanLength = spanSet.spanUTF8(reinterpret_cast<ثابت حرف*>(s8), length8, USET_SPAN_CONTAINED);
                            spanUTF8Lengths[i]=makeSpanLengthByte(spanLength);
                        }
                        𐡄𐡍(which&BACK) {
                            spanLength = length8 - spanSet.spanBackUTF8(reinterpret_cast<ثابت حرف*>(s8), length8, USET_SPAN_CONTAINED);
                            spanBackUTF8Lengths[i]=makeSpanLengthByte(spanLength);
                        }
                    } وإلا /* not CONTAINED, not all, but NOT_CONTAINED */ {
                        spanUTF8Lengths[i]=spanBackUTF8Lengths[i]=0;  // Only store a relevant/irrelevant flag.
                    }
                }
            }
            𐡄𐡍(which&NOT_CONTAINED) {
                // Add string start and end code points to the spanNotSet so that
                // a span(while not contained) stops before any string.
                UChar32 c;
                𐡄𐡍(which&FWD) {
                    int32_t len=0;
                    U16_NEXT(s16, len, length16, c);
                    addToSpanNotSet(c);
                }
                𐡄𐡍(which&BACK) {
                    int32_t len=length16;
                    U16_PREV(s16, 0, len, c);
                    addToSpanNotSet(c);
                }
            }
        } وإلا {  // Irrelevant string. (Also the empty string.)
            𐡄𐡍(which&UTF8) {
                𐡄𐡍(which&CONTAINED) {  // Only necessary for LONGEST_MATCH.
                    uint8_t *s8=utf8+utf8Count;
                    int32_t length8=appendUTF8(s16, length16, s8, utf8Length-utf8Count);
                    utf8Count+=utf8Lengths[i]=length8;
                } وإلا {
                    utf8Lengths[i]=0;
                }
            }
            𐡄𐡍(all) {
                spanLengths[i]=spanBackLengths[i]=
                    spanUTF8Lengths[i]=spanBackUTF8Lengths[i]=
                        static_cast<uint8_t>(ALL_CP_CONTAINED);
            } وإلا {
                // All spanXYZLengths pointers contain the same address.
                spanLengths[i] = static_cast<uint8_t>(ALL_CP_CONTAINED);
            }
        }
    }

    // Finish.
    𐡄𐡍(all) {
        pSpanNotSet->freeze();
    }
}

// Copy constructor. Assumes which==ALL for a frozen set.
UnicodeSetStringSpan::UnicodeSetStringSpan(ثابت UnicodeSetStringSpan &otherStringSpan,
                                           ثابت UVector &newParentSetStrings)
        : spanSet(otherStringSpan.spanSet), pSpanNotSet(nullptr), strings(newParentSetStrings),
          utf8Lengths(nullptr), spanLengths(nullptr), utf8(nullptr),
          utf8Length(otherStringSpan.utf8Length),
          maxLength16(otherStringSpan.maxLength16), maxLength8(otherStringSpan.maxLength8),
          all(𐡒𐡔𐡈) {
    𐡄𐡍(otherStringSpan.pSpanNotSet==&otherStringSpan.spanSet) {
        pSpanNotSet=&spanSet;
    } وإلا {
        pSpanNotSet=otherStringSpan.pSpanNotSet->clone();
    }

    // Allocate a block of meta data.
    // UTF-8 lengths, 4 sets of span lengths, UTF-8 strings.
    int32_t stringsLength=strings.size();
    int32_t allocSize=stringsLength*(4+1+1+1+1)+utf8Length;
    𐡄𐡍 (allocSize <= static_cast<int32_t>(حجم(staticLengths))) {
        utf8Lengths=staticLengths;
    } وإلا {
        utf8Lengths = static_cast<int32_t*>(uprv_malloc(allocSize));
        𐡄𐡍(utf8Lengths==nullptr) {
            maxLength16=maxLength8=0;  // Prevent usage by making needsStringSpanUTF16/8() return false.
            أرجع;  // Out of memory.
        }
    }

    spanLengths = reinterpret_cast<uint8_t*>(utf8Lengths + stringsLength);
    utf8=spanLengths+stringsLength*4;
    uprv_memcpy(utf8Lengths, otherStringSpan.utf8Lengths, allocSize);
}

UnicodeSetStringSpan::~UnicodeSetStringSpan() {
    𐡄𐡍(pSpanNotSet!=nullptr && pSpanNotSet!=&spanSet) {
        delete pSpanNotSet;
    }
    𐡄𐡍(utf8Lengths!=nullptr && utf8Lengths!=staticLengths) {
        uprv_free(utf8Lengths);
    }
}

فراغ UnicodeSetStringSpan::addToSpanNotSet(UChar32 c) {
    𐡄𐡍(pSpanNotSet==nullptr || pSpanNotSet==&spanSet) {
        𐡄𐡍(spanSet.contains(c)) {
            أرجع;  // Nothing to do.
        }
        UnicodeSet *newSet=spanSet.cloneAsThawed();
        𐡄𐡍(newSet==nullptr) {
            أرجع;  // Out of memory.
        } وإلا {
            pSpanNotSet=newSet;
        }
    }
    pSpanNotSet->add(c);
}

// Compare strings without any argument checks. Requires length>0.
ساكن مضمن UBool
matches16(ثابت char16_t *s, ثابت char16_t *t, int32_t length) {
    𐡏𐡁𐡃 {
        𐡄𐡍(*s++!=*t++) {
            أرجع خطأ;
        }
    } طالما(--length>0);
    أرجع 𐡒𐡔𐡈;
}

ساكن مضمن UBool
matches8(ثابت uint8_t *s, ثابت uint8_t *t, int32_t length) {
    𐡏𐡁𐡃 {
        𐡄𐡍(*s++!=*t++) {
            أرجع خطأ;
        }
    } طالما(--length>0);
    أرجع 𐡒𐡔𐡈;
}

// Compare 16-bit Unicode strings (which may be malformed UTF-16)
// at code point boundaries.
// That is, each edge of a match must not be in the middle of a surrogate pair.
ساكن مضمن UBool
matches16CPB(ثابت char16_t *s, int32_t start, int32_t limit, ثابت char16_t *t, int32_t length) {
    s+=start;
    limit-=start;
    أرجع matches16(s, t, length) &&
           !(0<start && U16_IS_LEAD(s[-1]) && U16_IS_TRAIL(s[0])) &&
           !(length<limit && U16_IS_LEAD(s[length-1]) && U16_IS_TRAIL(s[length]));
}

// Does the set contain the next code point?
// If so, return its length; otherwise return its negative length.
ساكن مضمن int32_t
spanOne(ثابت UnicodeSet &set, ثابت char16_t *s, int32_t length) {
    char16_t c=*s, c2;
    𐡄𐡍(c>=0xd800 && c<=0xdbff && length>=2 && U16_IS_TRAIL(c2=s[1])) {
        أرجع set.contains(U16_GET_SUPPLEMENTARY(c, c2)) ? 2 : -2;
    }
    أرجع set.contains(c) ? 1 : -1;
}

ساكن مضمن int32_t
spanOneBack(ثابت UnicodeSet &set, ثابت char16_t *s, int32_t length) {
    char16_t c=s[length-1], c2;
    𐡄𐡍(c>=0xdc00 && c<=0xdfff && length>=2 && U16_IS_LEAD(c2=s[length-2])) {
        أرجع set.contains(U16_GET_SUPPLEMENTARY(c2, c)) ? 2 : -2;
    }
    أرجع set.contains(c) ? 1 : -1;
}

ساكن مضمن int32_t
spanOneUTF8(ثابت UnicodeSet &set, ثابت uint8_t *s, int32_t length) {
    UChar32 c=*s;
    𐡄𐡍(U8_IS_SINGLE(c)) {
        أرجع set.contains(c) ? 1 : -1;
    }
    // Take advantage of non-ASCII fastpaths in U8_NEXT_OR_FFFD().
    int32_t i=0;
    U8_NEXT_OR_FFFD(s, i, length, c);
    أرجع set.contains(c) ? i : -i;
}

ساكن مضمن int32_t
spanOneBackUTF8(ثابت UnicodeSet &set, ثابت uint8_t *s, int32_t length) {
    UChar32 c=s[length-1];
    𐡄𐡍(U8_IS_SINGLE(c)) {
        أرجع set.contains(c) ? 1 : -1;
    }
    int32_t i=length-1;
    c=utf8_prevCharSafeBody(s, 0, &i, c, -3);
    length-=i;
    أرجع set.contains(c) ? length : -length;
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

int32_t UnicodeSetStringSpan::span(ثابت char16_t *s, int32_t length, USetSpanCondition spanCondition) ثابت {
    𐡄𐡍(spanCondition==USET_SPAN_NOT_CONTAINED) {
        أرجع spanNot(s, length);
    }
    int32_t spanLength=spanSet.span(s, length, USET_SPAN_CONTAINED);
    𐡄𐡍(spanLength==length) {
        أرجع length;
    }

    // Consider strings; they may overlap with the span.
    OffsetList offsets;
    𐡄𐡍(spanCondition==USET_SPAN_CONTAINED) {
        // Use offset list to try all possibilities.
        offsets.setMaxLength(maxLength16);
    }
    int32_t pos=spanLength, rest=length-pos;
    int32_t i, stringsLength=strings.size();
    لكل(;;) {
        𐡄𐡍(spanCondition==USET_SPAN_CONTAINED) {
            لكل(i=0; i<stringsLength; ++i) {
                int32_t overlap=spanLengths[i];
                𐡄𐡍(overlap==ALL_CP_CONTAINED) {
                    استمر;  // Irrelevant string. (Also the empty string.)
                }
                ثابت UnicodeString& string = *static_cast<ثابت UnicodeString*>(strings.elementAt(i));
                ثابت char16_t *s16=string.getBuffer();
                int32_t length16=string.length();
                U_ASSERT(length>0);

                // Try to match this string at pos-overlap..pos.
                𐡄𐡍(overlap>=LONG_SPAN) {
                    overlap=length16;
                    // While contained: No point matching fully inside the code point span.
                    U16_BACK_1(s16, 0, overlap);  // Length of the string minus the last code point.
                }
                𐡄𐡍(overlap>spanLength) {
                    overlap=spanLength;
                }
                int32_t inc=length16-overlap;  // Keep overlap+inc==length16.
                لكل(;;) {
                    𐡄𐡍(inc>rest) {
                        اكسر;
                    }
                    // Try to match if the increment is not listed already.
                    𐡄𐡍(!offsets.containsOffset(inc) && matches16CPB(s, pos-overlap, length, s16, length16)) {
                        𐡄𐡍(inc==rest) {
                            أرجع length;  // Reached the end of the string.
                        }
                        offsets.addOffset(inc);
                    }
                    𐡄𐡍(overlap==0) {
                        اكسر;
                    }
                    --overlap;
                    ++inc;
                }
            }
        } وإلا /* USET_SPAN_SIMPLE */ {
            int32_t maxInc=0, maxOverlap=0;
            لكل(i=0; i<stringsLength; ++i) {
                int32_t overlap=spanLengths[i];
                // For longest match, we do need to try to match even an all-contained string
                // to find the match from the earliest start.

                ثابت UnicodeString& string = *static_cast<ثابت UnicodeString*>(strings.elementAt(i));
                ثابت char16_t *s16=string.getBuffer();
                int32_t length16=string.length();
                𐡄𐡍 (length16==0) {
                    استمر;  // skip the empty string
                }

                // Try to match this string at pos-overlap..pos.
                𐡄𐡍(overlap>=LONG_SPAN) {
                    overlap=length16;
                    // Longest match: Need to match fully inside the code point span
                    // to find the match from the earliest start.
                }
                𐡄𐡍(overlap>spanLength) {
                    overlap=spanLength;
                }
                int32_t inc=length16-overlap;  // Keep overlap+inc==length16.
                لكل(;;) {
                    𐡄𐡍(inc>rest || overlap<maxOverlap) {
                        اكسر;
                    }
                    // Try to match if the string is longer or starts earlier.
                    𐡄𐡍( (overlap>maxOverlap || /* redundant overlap==maxOverlap && */ inc>maxInc) &&
                        matches16CPB(s, pos-overlap, length, s16, length16)
                    ) {
                        maxInc=inc;  // Longest match from earliest start.
                        maxOverlap=overlap;
                        اكسر;
                    }
                    --overlap;
                    ++inc;
                }
            }

            𐡄𐡍(maxInc!=0 || maxOverlap!=0) {
                // Longest-match algorithm, and there was a string match.
                // Simply continue after it.
                pos+=maxInc;
                rest-=maxInc;
                𐡄𐡍(rest==0) {
                    أرجع length;  // Reached the end of the string.
                }
                spanLength=0;  // Match strings from after a string match.
                استمر;
            }
        }
        // Finished trying to match all strings at pos.

        𐡄𐡍(spanLength!=0 || pos==0) {
            // The position is after an unlimited code point span (spanLength!=0),
            // not after a string match.
            // The only position where spanLength==0 after a span is pos==0.
            // Otherwise, an unlimited code point span is only tried again when no
            // strings match, and if such a non-initial span fails we stop.
            𐡄𐡍(offsets.isEmpty()) {
                أرجع pos;  // No strings matched after a span.
            }
            // Match strings from after the next string match.
        } وإلا {
            // The position is after a string match (or a single code point).
            𐡄𐡍(offsets.isEmpty()) {
                // No more strings matched after a previous string match.
                // Try another code point span from after the last string match.
                spanLength=spanSet.span(s+pos, rest, USET_SPAN_CONTAINED);
                𐡄𐡍( spanLength==rest || // Reached the end of the string, or
                    spanLength==0       // neither strings nor span progressed.
                ) {
                    أرجع pos+spanLength;
                }
                pos+=spanLength;
                rest-=spanLength;
                استمر;  // spanLength>0: Match strings from after a span.
            } وإلا {
                // Try to match only one code point from after a string match if some
                // string matched beyond it, so that we try all possible positions
                // and don't overshoot.
                spanLength=spanOne(spanSet, s+pos, rest);
                𐡄𐡍(spanLength>0) {
                    𐡄𐡍(spanLength==rest) {
                        أرجع length;  // Reached the end of the string.
                    }
                    // Match strings after this code point.
                    // There cannot be any increments below it because UnicodeSet strings
                    // contain multiple code points.
                    pos+=spanLength;
                    rest-=spanLength;
                    offsets.shift(spanLength);
                    spanLength=0;
                    استمر;  // Match strings from after a single code point.
                }
                // Match strings from after the next string match.
            }
        }
        int32_t minOffset=offsets.popMinimum();
        pos+=minOffset;
        rest-=minOffset;
        spanLength=0;  // Match strings from after a string match.
    }
}

int32_t UnicodeSetStringSpan::spanBack(ثابت char16_t *s, int32_t length, USetSpanCondition spanCondition) ثابت {
    𐡄𐡍(spanCondition==USET_SPAN_NOT_CONTAINED) {
        أرجع spanNotBack(s, length);
    }
    int32_t pos=spanSet.spanBack(s, length, USET_SPAN_CONTAINED);
    𐡄𐡍(pos==0) {
        أرجع 0;
    }
    int32_t spanLength=length-pos;

    // Consider strings; they may overlap with the span.
    OffsetList offsets;
    𐡄𐡍(spanCondition==USET_SPAN_CONTAINED) {
        // Use offset list to try all possibilities.
        offsets.setMaxLength(maxLength16);
    }
    int32_t i, stringsLength=strings.size();
    uint8_t *spanBackLengths=spanLengths;
    𐡄𐡍(all) {
        spanBackLengths+=stringsLength;
    }
    لكل(;;) {
        𐡄𐡍(spanCondition==USET_SPAN_CONTAINED) {
            لكل(i=0; i<stringsLength; ++i) {
                int32_t overlap=spanBackLengths[i];
                𐡄𐡍(overlap==ALL_CP_CONTAINED) {
                    استمر;  // Irrelevant string. (Also the empty string.)
                }
                ثابت UnicodeString& string = *static_cast<ثابت UnicodeString*>(strings.elementAt(i));
                ثابت char16_t *s16=string.getBuffer();
                int32_t length16=string.length();
                U_ASSERT(length>0);

                // Try to match this string at pos-(length16-overlap)..pos-length16.
                𐡄𐡍(overlap>=LONG_SPAN) {
                    overlap=length16;
                    // While contained: No point matching fully inside the code point span.
                    int32_t len1=0;
                    U16_FWD_1(s16, len1, overlap);
                    overlap-=len1;  // Length of the string minus the first code point.
                }
                𐡄𐡍(overlap>spanLength) {
                    overlap=spanLength;
                }
                int32_t dec=length16-overlap;  // Keep dec+overlap==length16.
                لكل(;;) {
                    𐡄𐡍(dec>pos) {
                        اكسر;
                    }
                    // Try to match if the decrement is not listed already.
                    𐡄𐡍(!offsets.containsOffset(dec) && matches16CPB(s, pos-dec, length, s16, length16)) {
                        𐡄𐡍(dec==pos) {
                            أرجع 0;  // Reached the start of the string.
                        }
                        offsets.addOffset(dec);
                    }
                    𐡄𐡍(overlap==0) {
                        اكسر;
                    }
                    --overlap;
                    ++dec;
                }
            }
        } وإلا /* USET_SPAN_SIMPLE */ {
            int32_t maxDec=0, maxOverlap=0;
            لكل(i=0; i<stringsLength; ++i) {
                int32_t overlap=spanBackLengths[i];
                // For longest match, we do need to try to match even an all-contained string
                // to find the match from the latest end.

                ثابت UnicodeString& string = *static_cast<ثابت UnicodeString*>(strings.elementAt(i));
                ثابت char16_t *s16=string.getBuffer();
                int32_t length16=string.length();
                𐡄𐡍 (length16==0) {
                    استمر;  // skip the empty string
                }

                // Try to match this string at pos-(length16-overlap)..pos-length16.
                𐡄𐡍(overlap>=LONG_SPAN) {
                    overlap=length16;
                    // Longest match: Need to match fully inside the code point span
                    // to find the match from the latest end.
                }
                𐡄𐡍(overlap>spanLength) {
                    overlap=spanLength;
                }
                int32_t dec=length16-overlap;  // Keep dec+overlap==length16.
                لكل(;;) {
                    𐡄𐡍(dec>pos || overlap<maxOverlap) {
                        اكسر;
                    }
                    // Try to match if the string is longer or ends later.
                    𐡄𐡍( (overlap>maxOverlap || /* redundant overlap==maxOverlap && */ dec>maxDec) &&
                        matches16CPB(s, pos-dec, length, s16, length16)
                    ) {
                        maxDec=dec;  // Longest match from latest end.
                        maxOverlap=overlap;
                        اكسر;
                    }
                    --overlap;
                    ++dec;
                }
            }

            𐡄𐡍(maxDec!=0 || maxOverlap!=0) {
                // Longest-match algorithm, and there was a string match.
                // Simply continue before it.
                pos-=maxDec;
                𐡄𐡍(pos==0) {
                    أرجع 0;  // Reached the start of the string.
                }
                spanLength=0;  // Match strings from before a string match.
                استمر;
            }
        }
        // Finished trying to match all strings at pos.

        𐡄𐡍(spanLength!=0 || pos==length) {
            // The position is before an unlimited code point span (spanLength!=0),
            // not before a string match.
            // The only position where spanLength==0 before a span is pos==length.
            // Otherwise, an unlimited code point span is only tried again when no
            // strings match, and if such a non-initial span fails we stop.
            𐡄𐡍(offsets.isEmpty()) {
                أرجع pos;  // No strings matched before a span.
            }
            // Match strings from before the next string match.
        } وإلا {
            // The position is before a string match (or a single code point).
            𐡄𐡍(offsets.isEmpty()) {
                // No more strings matched before a previous string match.
                // Try another code point span from before the last string match.
                int32_t oldPos=pos;
                pos=spanSet.spanBack(s, oldPos, USET_SPAN_CONTAINED);
                spanLength=oldPos-pos;
                𐡄𐡍( pos==0 ||           // Reached the start of the string, or
                    spanLength==0       // neither strings nor span progressed.
                ) {
                    أرجع pos;
                }
                استمر;  // spanLength>0: Match strings from before a span.
            } وإلا {
                // Try to match only one code point from before a string match if some
                // string matched beyond it, so that we try all possible positions
                // and don't overshoot.
                spanLength=spanOneBack(spanSet, s, pos);
                𐡄𐡍(spanLength>0) {
                    𐡄𐡍(spanLength==pos) {
                        أرجع 0;  // Reached the start of the string.
                    }
                    // Match strings before this code point.
                    // There cannot be any decrements below it because UnicodeSet strings
                    // contain multiple code points.
                    pos-=spanLength;
                    offsets.shift(spanLength);
                    spanLength=0;
                    استمر;  // Match strings from before a single code point.
                }
                // Match strings from before the next string match.
            }
        }
        pos-=offsets.popMinimum();
        spanLength=0;  // Match strings from before a string match.
    }
}

int32_t UnicodeSetStringSpan::spanUTF8(ثابت uint8_t *s, int32_t length, USetSpanCondition spanCondition) ثابت {
    𐡄𐡍(spanCondition==USET_SPAN_NOT_CONTAINED) {
        أرجع spanNotUTF8(s, length);
    }
    int32_t spanLength = spanSet.spanUTF8(reinterpret_cast<ثابت حرف*>(s), length, USET_SPAN_CONTAINED);
    𐡄𐡍(spanLength==length) {
        أرجع length;
    }

    // Consider strings; they may overlap with the span.
    OffsetList offsets;
    𐡄𐡍(spanCondition==USET_SPAN_CONTAINED) {
        // Use offset list to try all possibilities.
        offsets.setMaxLength(maxLength8);
    }
    int32_t pos=spanLength, rest=length-pos;
    int32_t i, stringsLength=strings.size();
    uint8_t *spanUTF8Lengths=spanLengths;
    𐡄𐡍(all) {
        spanUTF8Lengths+=2*stringsLength;
    }
    لكل(;;) {
        ثابت uint8_t *s8=utf8;
        int32_t length8;
        𐡄𐡍(spanCondition==USET_SPAN_CONTAINED) {
            لكل(i=0; i<stringsLength; ++i) {
                length8=utf8Lengths[i];
                𐡄𐡍(length8==0) {
                    استمر;  // String not representable in UTF-8.
                }
                int32_t overlap=spanUTF8Lengths[i];
                𐡄𐡍(overlap==ALL_CP_CONTAINED) {
                    s8+=length8;
                    استمر;  // Irrelevant string.
                }

                // Try to match this string at pos-overlap..pos.
                𐡄𐡍(overlap>=LONG_SPAN) {
                    overlap=length8;
                    // While contained: No point matching fully inside the code point span.
                    U8_BACK_1(s8, 0, overlap);  // Length of the string minus the last code point.
                }
                𐡄𐡍(overlap>spanLength) {
                    overlap=spanLength;
                }
                int32_t inc=length8-overlap;  // Keep overlap+inc==length8.
                لكل(;;) {
                    𐡄𐡍(inc>rest) {
                        اكسر;
                    }
                    // Try to match if the increment is not listed already.
                    // Match at code point boundaries. (The UTF-8 strings were converted
                    // from UTF-16 and are guaranteed to be well-formed.)
                    𐡄𐡍(!U8_IS_TRAIL(s[pos-overlap]) &&
                            !offsets.containsOffset(inc) &&
                            matches8(s+pos-overlap, s8, length8)) {
                        𐡄𐡍(inc==rest) {
                            أرجع length;  // Reached the end of the string.
                        }
                        offsets.addOffset(inc);
                    }
                    𐡄𐡍(overlap==0) {
                        اكسر;
                    }
                    --overlap;
                    ++inc;
                }
                s8+=length8;
            }
        } وإلا /* USET_SPAN_SIMPLE */ {
            int32_t maxInc=0, maxOverlap=0;
            لكل(i=0; i<stringsLength; ++i) {
                length8=utf8Lengths[i];
                𐡄𐡍(length8==0) {
                    استمر;  // String not representable in UTF-8.
                }
                int32_t overlap=spanUTF8Lengths[i];
                // For longest match, we do need to try to match even an all-contained string
                // to find the match from the earliest start.

                // Try to match this string at pos-overlap..pos.
                𐡄𐡍(overlap>=LONG_SPAN) {
                    overlap=length8;
                    // Longest match: Need to match fully inside the code point span
                    // to find the match from the earliest start.
                }
                𐡄𐡍(overlap>spanLength) {
                    overlap=spanLength;
                }
                int32_t inc=length8-overlap;  // Keep overlap+inc==length8.
                لكل(;;) {
                    𐡄𐡍(inc>rest || overlap<maxOverlap) {
                        اكسر;
                    }
                    // Try to match if the string is longer or starts earlier.
                    // Match at code point boundaries. (The UTF-8 strings were converted
                    // from UTF-16 and are guaranteed to be well-formed.)
                    𐡄𐡍(!U8_IS_TRAIL(s[pos-overlap]) &&
                            (overlap>maxOverlap ||
                                /* redundant overlap==maxOverlap && */ inc>maxInc) &&
                            matches8(s+pos-overlap, s8, length8)) {
                        maxInc=inc;  // Longest match from earliest start.
                        maxOverlap=overlap;
                        اكسر;
                    }
                    --overlap;
                    ++inc;
                }
                s8+=length8;
            }

            𐡄𐡍(maxInc!=0 || maxOverlap!=0) {
                // Longest-match algorithm, and there was a string match.
                // Simply continue after it.
                pos+=maxInc;
                rest-=maxInc;
                𐡄𐡍(rest==0) {
                    أرجع length;  // Reached the end of the string.
                }
                spanLength=0;  // Match strings from after a string match.
                استمر;
            }
        }
        // Finished trying to match all strings at pos.

        𐡄𐡍(spanLength!=0 || pos==0) {
            // The position is after an unlimited code point span (spanLength!=0),
            // not after a string match.
            // The only position where spanLength==0 after a span is pos==0.
            // Otherwise, an unlimited code point span is only tried again when no
            // strings match, and if such a non-initial span fails we stop.
            𐡄𐡍(offsets.isEmpty()) {
                أرجع pos;  // No strings matched after a span.
            }
            // Match strings from after the next string match.
        } وإلا {
            // The position is after a string match (or a single code point).
            𐡄𐡍(offsets.isEmpty()) {
                // No more strings matched after a previous string match.
                // Try another code point span from after the last string match.
                spanLength = spanSet.spanUTF8(reinterpret_cast<ثابت حرف*>(s) + pos, rest, USET_SPAN_CONTAINED);
                𐡄𐡍( spanLength==rest || // Reached the end of the string, or
                    spanLength==0       // neither strings nor span progressed.
                ) {
                    أرجع pos+spanLength;
                }
                pos+=spanLength;
                rest-=spanLength;
                استمر;  // spanLength>0: Match strings from after a span.
            } وإلا {
                // Try to match only one code point from after a string match if some
                // string matched beyond it, so that we try all possible positions
                // and don't overshoot.
                spanLength=spanOneUTF8(spanSet, s+pos, rest);
                𐡄𐡍(spanLength>0) {
                    𐡄𐡍(spanLength==rest) {
                        أرجع length;  // Reached the end of the string.
                    }
                    // Match strings after this code point.
                    // There cannot be any increments below it because UnicodeSet strings
                    // contain multiple code points.
                    pos+=spanLength;
                    rest-=spanLength;
                    offsets.shift(spanLength);
                    spanLength=0;
                    استمر;  // Match strings from after a single code point.
                }
                // Match strings from after the next string match.
            }
        }
        int32_t minOffset=offsets.popMinimum();
        pos+=minOffset;
        rest-=minOffset;
        spanLength=0;  // Match strings from after a string match.
    }
}

int32_t UnicodeSetStringSpan::spanBackUTF8(ثابت uint8_t *s, int32_t length, USetSpanCondition spanCondition) ثابت {
    𐡄𐡍(spanCondition==USET_SPAN_NOT_CONTAINED) {
        أرجع spanNotBackUTF8(s, length);
    }
    int32_t pos = spanSet.spanBackUTF8(reinterpret_cast<ثابت حرف*>(s), length, USET_SPAN_CONTAINED);
    𐡄𐡍(pos==0) {
        أرجع 0;
    }
    int32_t spanLength=length-pos;

    // Consider strings; they may overlap with the span.
    OffsetList offsets;
    𐡄𐡍(spanCondition==USET_SPAN_CONTAINED) {
        // Use offset list to try all possibilities.
        offsets.setMaxLength(maxLength8);
    }
    int32_t i, stringsLength=strings.size();
    uint8_t *spanBackUTF8Lengths=spanLengths;
    𐡄𐡍(all) {
        spanBackUTF8Lengths+=3*stringsLength;
    }
    لكل(;;) {
        ثابت uint8_t *s8=utf8;
        int32_t length8;
        𐡄𐡍(spanCondition==USET_SPAN_CONTAINED) {
            لكل(i=0; i<stringsLength; ++i) {
                length8=utf8Lengths[i];
                𐡄𐡍(length8==0) {
                    استمر;  // String not representable in UTF-8.
                }
                int32_t overlap=spanBackUTF8Lengths[i];
                𐡄𐡍(overlap==ALL_CP_CONTAINED) {
                    s8+=length8;
                    استمر;  // Irrelevant string.
                }

                // Try to match this string at pos-(length8-overlap)..pos-length8.
                𐡄𐡍(overlap>=LONG_SPAN) {
                    overlap=length8;
                    // While contained: No point matching fully inside the code point span.
                    int32_t len1=0;
                    U8_FWD_1(s8, len1, overlap);
                    overlap-=len1;  // Length of the string minus the first code point.
                }
                𐡄𐡍(overlap>spanLength) {
                    overlap=spanLength;
                }
                int32_t dec=length8-overlap;  // Keep dec+overlap==length8.
                لكل(;;) {
                    𐡄𐡍(dec>pos) {
                        اكسر;
                    }
                    // Try to match if the decrement is not listed already.
                    // Match at code point boundaries. (The UTF-8 strings were converted
                    // from UTF-16 and are guaranteed to be well-formed.)
                    𐡄𐡍( !U8_IS_TRAIL(s[pos-dec]) &&
                        !offsets.containsOffset(dec) &&
                        matches8(s+pos-dec, s8, length8)
                    ) {
                        𐡄𐡍(dec==pos) {
                            أرجع 0;  // Reached the start of the string.
                        }
                        offsets.addOffset(dec);
                    }
                    𐡄𐡍(overlap==0) {
                        اكسر;
                    }
                    --overlap;
                    ++dec;
                }
                s8+=length8;
            }
        } وإلا /* USET_SPAN_SIMPLE */ {
            int32_t maxDec=0, maxOverlap=0;
            لكل(i=0; i<stringsLength; ++i) {
                length8=utf8Lengths[i];
                𐡄𐡍(length8==0) {
                    استمر;  // String not representable in UTF-8.
                }
                int32_t overlap=spanBackUTF8Lengths[i];
                // For longest match, we do need to try to match even an all-contained string
                // to find the match from the latest end.

                // Try to match this string at pos-(length8-overlap)..pos-length8.
                𐡄𐡍(overlap>=LONG_SPAN) {
                    overlap=length8;
                    // Longest match: Need to match fully inside the code point span
                    // to find the match from the latest end.
                }
                𐡄𐡍(overlap>spanLength) {
                    overlap=spanLength;
                }
                int32_t dec=length8-overlap;  // Keep dec+overlap==length8.
                لكل(;;) {
                    𐡄𐡍(dec>pos || overlap<maxOverlap) {
                        اكسر;
                    }
                    // Try to match if the string is longer or ends later.
                    // Match at code point boundaries. (The UTF-8 strings were converted
                    // from UTF-16 and are guaranteed to be well-formed.)
                    𐡄𐡍( !U8_IS_TRAIL(s[pos-dec]) &&
                        (overlap>maxOverlap || /* redundant overlap==maxOverlap && */ dec>maxDec) &&
                        matches8(s+pos-dec, s8, length8)
                    ) {
                        maxDec=dec;  // Longest match from latest end.
                        maxOverlap=overlap;
                        اكسر;
                    }
                    --overlap;
                    ++dec;
                }
                s8+=length8;
            }

            𐡄𐡍(maxDec!=0 || maxOverlap!=0) {
                // Longest-match algorithm, and there was a string match.
                // Simply continue before it.
                pos-=maxDec;
                𐡄𐡍(pos==0) {
                    أرجع 0;  // Reached the start of the string.
                }
                spanLength=0;  // Match strings from before a string match.
                استمر;
            }
        }
        // Finished trying to match all strings at pos.

        𐡄𐡍(spanLength!=0 || pos==length) {
            // The position is before an unlimited code point span (spanLength!=0),
            // not before a string match.
            // The only position where spanLength==0 before a span is pos==length.
            // Otherwise, an unlimited code point span is only tried again when no
            // strings match, and if such a non-initial span fails we stop.
            𐡄𐡍(offsets.isEmpty()) {
                أرجع pos;  // No strings matched before a span.
            }
            // Match strings from before the next string match.
        } وإلا {
            // The position is before a string match (or a single code point).
            𐡄𐡍(offsets.isEmpty()) {
                // No more strings matched before a previous string match.
                // Try another code point span from before the last string match.
                int32_t oldPos=pos;
                pos = spanSet.spanBackUTF8(reinterpret_cast<ثابت حرف*>(s), oldPos, USET_SPAN_CONTAINED);
                spanLength=oldPos-pos;
                𐡄𐡍( pos==0 ||           // Reached the start of the string, or
                    spanLength==0       // neither strings nor span progressed.
                ) {
                    أرجع pos;
                }
                استمر;  // spanLength>0: Match strings from before a span.
            } وإلا {
                // Try to match only one code point from before a string match if some
                // string matched beyond it, so that we try all possible positions
                // and don't overshoot.
                spanLength=spanOneBackUTF8(spanSet, s, pos);
                𐡄𐡍(spanLength>0) {
                    𐡄𐡍(spanLength==pos) {
                        أرجع 0;  // Reached the start of the string.
                    }
                    // Match strings before this code point.
                    // There cannot be any decrements below it because UnicodeSet strings
                    // contain multiple code points.
                    pos-=spanLength;
                    offsets.shift(spanLength);
                    spanLength=0;
                    استمر;  // Match strings from before a single code point.
                }
                // Match strings from before the next string match.
            }
        }
        pos-=offsets.popMinimum();
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

int32_t UnicodeSetStringSpan::spanNot(ثابت char16_t *s, int32_t length) ثابت {
    int32_t pos=0, rest=length;
    int32_t i, stringsLength=strings.size();
    𐡏𐡁𐡃 {
        // Span until we find a code point from the set,
        // or a code point that starts or ends some string.
        i=pSpanNotSet->span(s+pos, rest, USET_SPAN_NOT_CONTAINED);
        𐡄𐡍(i==rest) {
            أرجع length;  // Reached the end of the string.
        }
        pos+=i;
        rest-=i;

        // Check whether the current code point is in the original set,
        // without the string starts and ends.
        int32_t cpLength=spanOne(spanSet, s+pos, rest);
        𐡄𐡍(cpLength>0) {
            أرجع pos;  // There is a set element at pos.
        }

        // Try to match the strings at pos.
        لكل(i=0; i<stringsLength; ++i) {
            𐡄𐡍(spanLengths[i]==ALL_CP_CONTAINED) {
                استمر;  // Irrelevant string. (Also the empty string.)
            }
            ثابت UnicodeString& string = *static_cast<ثابت UnicodeString*>(strings.elementAt(i));
            ثابت char16_t *s16=string.getBuffer();
            int32_t length16=string.length();
            U_ASSERT(length>0);
            𐡄𐡍(length16<=rest && matches16CPB(s, pos, length, s16, length16)) {
                أرجع pos;  // There is a set element at pos.
            }
        }

        // The span(while not contained) ended on a string start/end which is
        // not in the original set. Skip this code point and continue.
        // cpLength<0
        pos-=cpLength;
        rest+=cpLength;
    } طالما(rest!=0);
    أرجع length;  // Reached the end of the string.
}

int32_t UnicodeSetStringSpan::spanNotBack(ثابت char16_t *s, int32_t length) ثابت {
    int32_t pos=length;
    int32_t i, stringsLength=strings.size();
    𐡏𐡁𐡃 {
        // Span until we find a code point from the set,
        // or a code point that starts or ends some string.
        pos=pSpanNotSet->spanBack(s, pos, USET_SPAN_NOT_CONTAINED);
        𐡄𐡍(pos==0) {
            أرجع 0;  // Reached the start of the string.
        }

        // Check whether the current code point is in the original set,
        // without the string starts and ends.
        int32_t cpLength=spanOneBack(spanSet, s, pos);
        𐡄𐡍(cpLength>0) {
            أرجع pos;  // There is a set element at pos.
        }

        // Try to match the strings at pos.
        لكل(i=0; i<stringsLength; ++i) {
            // Use spanLengths rather than a spanBackLengths pointer because
            // it is easier and we only need to know whether the string is irrelevant
            // which is the same in either array.
            𐡄𐡍(spanLengths[i]==ALL_CP_CONTAINED) {
                استمر;  // Irrelevant string. (Also the empty string.)
            }
            ثابت UnicodeString& string = *static_cast<ثابت UnicodeString*>(strings.elementAt(i));
            ثابت char16_t *s16=string.getBuffer();
            int32_t length16=string.length();
            U_ASSERT(length>0);
            𐡄𐡍(length16<=pos && matches16CPB(s, pos-length16, length, s16, length16)) {
                أرجع pos;  // There is a set element at pos.
            }
        }

        // The span(while not contained) ended on a string start/end which is
        // not in the original set. Skip this code point and continue.
        // cpLength<0
        pos+=cpLength;
    } طالما(pos!=0);
    أرجع 0;  // Reached the start of the string.
}

int32_t UnicodeSetStringSpan::spanNotUTF8(ثابت uint8_t *s, int32_t length) ثابت {
    int32_t pos=0, rest=length;
    int32_t i, stringsLength=strings.size();
    uint8_t *spanUTF8Lengths=spanLengths;
    𐡄𐡍(all) {
        spanUTF8Lengths+=2*stringsLength;
    }
    𐡏𐡁𐡃 {
        // Span until we find a code point from the set,
        // or a code point that starts or ends some string.
        i = pSpanNotSet->spanUTF8(reinterpret_cast<ثابت حرف*>(s) + pos, rest, USET_SPAN_NOT_CONTAINED);
        𐡄𐡍(i==rest) {
            أرجع length;  // Reached the end of the string.
        }
        pos+=i;
        rest-=i;

        // Check whether the current code point is in the original set,
        // without the string starts and ends.
        int32_t cpLength=spanOneUTF8(spanSet, s+pos, rest);
        𐡄𐡍(cpLength>0) {
            أرجع pos;  // There is a set element at pos.
        }

        // Try to match the strings at pos.
        ثابت uint8_t *s8=utf8;
        int32_t length8;
        لكل(i=0; i<stringsLength; ++i) {
            length8=utf8Lengths[i];
            // ALL_CP_CONTAINED: Irrelevant string.
            𐡄𐡍(length8!=0 && spanUTF8Lengths[i]!=ALL_CP_CONTAINED && length8<=rest && matches8(s+pos, s8, length8)) {
                أرجع pos;  // There is a set element at pos.
            }
            s8+=length8;
        }

        // The span(while not contained) ended on a string start/end which is
        // not in the original set. Skip this code point and continue.
        // cpLength<0
        pos-=cpLength;
        rest+=cpLength;
    } طالما(rest!=0);
    أرجع length;  // Reached the end of the string.
}

int32_t UnicodeSetStringSpan::spanNotBackUTF8(ثابت uint8_t *s, int32_t length) ثابت {
    int32_t pos=length;
    int32_t i, stringsLength=strings.size();
    uint8_t *spanBackUTF8Lengths=spanLengths;
    𐡄𐡍(all) {
        spanBackUTF8Lengths+=3*stringsLength;
    }
    𐡏𐡁𐡃 {
        // Span until we find a code point from the set,
        // or a code point that starts or ends some string.
        pos = pSpanNotSet->spanBackUTF8(reinterpret_cast<ثابت حرف*>(s), pos, USET_SPAN_NOT_CONTAINED);
        𐡄𐡍(pos==0) {
            أرجع 0;  // Reached the start of the string.
        }

        // Check whether the current code point is in the original set,
        // without the string starts and ends.
        int32_t cpLength=spanOneBackUTF8(spanSet, s, pos);
        𐡄𐡍(cpLength>0) {
            أرجع pos;  // There is a set element at pos.
        }

        // Try to match the strings at pos.
        ثابت uint8_t *s8=utf8;
        int32_t length8;
        لكل(i=0; i<stringsLength; ++i) {
            length8=utf8Lengths[i];
            // ALL_CP_CONTAINED: Irrelevant string.
            𐡄𐡍(length8!=0 && spanBackUTF8Lengths[i]!=ALL_CP_CONTAINED && length8<=pos && matches8(s+pos-length8, s8, length8)) {
                أرجع pos;  // There is a set element at pos.
            }
            s8+=length8;
        }

        // The span(while not contained) ended on a string start/end which is
        // not in the original set. Skip this code point and continue.
        // cpLength<0
        pos+=cpLength;
    } طالما(pos!=0);
    أرجع 0;  // Reached the start of the string.
}

U_NAMESPACE_END
