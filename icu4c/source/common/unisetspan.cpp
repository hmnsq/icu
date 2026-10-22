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

#समावेश "unicode/utypes.h"
#समावेश "unicode/uniset.h"
#समावेश "unicode/ustring.h"
#समावेश "unicode/utf8.h"
#समावेश "unicode/utf16.h"
#समावेश "cmemory.h"
#समावेश "uvector.h"
#समावेश "unisetspan.h"

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
श्रेणी OffsetList {  // Only ever stack-allocated, does not need to inherit UMemory.
खुला:
    OffsetList() : list(staticList), capacity(0), length(0), start(0) {}

    ~OffsetList() {
        अगर(list!=staticList) {
            uprv_free(list);
        }
    }

    // Call exactly once if the list is to be used.
    व्योम set_Max_लंबाई(int32_t max_लंबाई_2) {
        अगर (max_लंबाई_2 <= static_cast<int32_t>(माप(staticList))) {
            capacity = static_cast<int32_t>(माप(staticList));
        } अन्यथा {
            UBool* l = static_cast<UBool*>(uprv_malloc(max_लंबाई_2));
            अगर(l!=nullptr) {
                list=l;
                capacity=max_लंबाई_2;
            }
        }
        uprv_memset(list, 0, capacity);
    }

    व्योम सफा() {
        uprv_memset(list, 0, capacity);
        start=length=0;
    }

    UBool isEmpty() स्थिर {
        वापस length == 0;
    }

    // Reduce all stored offsets by delta, used when the current position
    // moves by delta.
    // There must not be any offsets lower than delta.
    // If there is an offset equal to delta, it is removed.
    // delta=[1..maxLength]
    व्योम shift(int32_t delta) {
        int32_t i=start+delta;
        अगर(i>=capacity) {
            i-=capacity;
        }
        अगर(list[i]) {
            list[i]=false;
            --length;
        }
        start=i;
    }

    // Add an offset. The list must not contain it yet.
    // offset=[1..maxLength]
    व्योम addOffset(int32_t offset) {
        int32_t i=start+offset;
        अगर(i>=capacity) {
            i-=capacity;
        }
        list[i]=true;
        ++length;
    }

    // offset=[1..maxLength]
    UBool containsOffset(int32_t offset) स्थिर {
        int32_t i=start+offset;
        अगर(i>=capacity) {
            i-=capacity;
        }
        वापस list[i];
    }

    // Find the lowest stored offset from a non-empty list, remove it,
    // and reduce all other offsets by this minimum.
    // Returns [1..maxLength].
    int32_t निकालो_Minimum() {
        // Look for the next offset in list[start+1..capacity-1].
        int32_t i=start, परिणाम;
        जबतक(++i<capacity) {
            अगर(list[i]) {
                list[i]=false;
                --length;
                परिणाम=i-start;
                start=i;
                वापस परिणाम;
            }
        }
        // i==capacity

        // Wrap around and look for the next offset in list[0..start].
        // Since the list is not empty, there will be one.
        परिणाम=capacity-start;
        i=0;
        जबतक(!list[i]) {
            ++i;
        }
        list[i]=false;
        --length;
        start=i;
        वापस परिणाम+=i;
    }

निजी:
    UBool *list;
    int32_t capacity;
    int32_t length;
    int32_t start;

    UBool staticList[16];
};

// Get the number of UTF-8 bytes for a UTF-16 (sub)string.
अटल int32_t
get_UTF8_लंबाई(स्थिर char16_t *s, int32_t length) {
    UErrorCode errorCode=U_ZERO_ERROR;
    int32_t length8=0;
    u_strToUTF8(nullptr, 0, &length8, s, length, &errorCode);
    अगर(U_SUCCESS(errorCode) || errorCode==U_बफर_OVERFLOW_ERROR) {
        वापस length8;
    } अन्यथा {
        // The string contains an unpaired surrogate.
        // Ignore this string.
        वापस 0;
    }
}

// Append the UTF-8 version of the string to t and return the appended UTF-8 length.
अटल int32_t
appendUTF8(स्थिर char16_t *s, int32_t length, uint8_t *t, int32_t capacity) {
    UErrorCode errorCode=U_ZERO_ERROR;
    int32_t length8=0;
    u_strToUTF8(reinterpret_cast<अक्षर*>(t), capacity, &length8, s, length, &errorCode);
    अगर(U_SUCCESS(errorCode)) {
        वापस length8;
    } अन्यथा {
        // The string contains an unpaired surrogate.
        // Ignore this string.
        वापस 0;
    }
}

अटल अंतरभूत uint8_t
make_Span_लंबाई_Byte(int32_t span_लंबाई) {
    // 0xfe==UnicodeSetStringSpan::LONG_SPAN
    वापस span_लंबाई < 0xfe ? static_cast<uint8_t>(span_लंबाई) : static_cast<uint8_t>(0xfe);
}

// Construct for all variants of span(), or only for any one variant.
// Initialize as little as possible, for single use.
UnicodeSetStringSpan::UnicodeSetStringSpan(स्थिर UnicodeSet &set,
                                           स्थिर UVector &setStrings,
                                           uint32_t which)
        : spanSet(0, 0x10ffff), pSpanNotSet(nullptr), strings(setStrings),
          utf8Lengths(nullptr), spanLengths(nullptr), utf8(nullptr),
          utf8_लंबाई(0),
          maxLength16(0), maxLength8(0),
          all(static_cast<UBool>(which == ALL)) {
    spanSet.retainAll(set);
    अगर(which&NOT_CONTAINED) {
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
    int32_t strings_लंबाई=strings.size();

    int32_t i, span_लंबाई;
    UBool someRelevant=false;
    क्रम(i=0; i<strings_लंबाई; ++i) {
        स्थिर UnicodeString& string = *static_cast<स्थिर UnicodeString*>(strings.elementAt(i));
        स्थिर char16_t *s16=string.get_बफर();
        int32_t length16=string.length();
        अगर (length16==0) {
            जारी;  // skip the empty string
        }
        UBool thisRelevant;
        span_लंबाई=spanSet.span(s16, length16, USET_SPAN_CONTAINED);
        अगर(span_लंबाई<length16) {  // Relevant string.
            someRelevant=thisRelevant=true;
        } अन्यथा {
            thisRelevant=false;
        }
        अगर((which&UTF16) && length16>maxLength16) {
            maxLength16=length16;
        }
        अगर((which&UTF8) && (thisRelevant || (which&CONTAINED))) {
            int32_t length8=get_UTF8_लंबाई(s16, length16);
            utf8_लंबाई+=length8;
            अगर(length8>maxLength8) {
                maxLength8=length8;
            }
        }
    }
    अगर(!someRelevant) {
        maxLength16=maxLength8=0;
        वापस;
    }

    // Freeze after checking for the need to use strings at all because freezing
    // a set takes some time and memory which are wasted if there are no relevant strings.
    अगर(all) {
        spanSet.freeze();
    }

    uint8_t *spanBackLengths;
    uint8_t *spanUTF8Lengths;
    uint8_t *spanBackUTF8Lengths;

    // Allocate a block of meta data.
    int32_t alloc_आकार_2;
    अगर(all) {
        // UTF-8 lengths, 4 sets of span lengths, UTF-8 strings.
        alloc_आकार_2=strings_लंबाई*(4+1+1+1+1)+utf8_लंबाई;
    } अन्यथा {
        alloc_आकार_2=strings_लंबाई;  // One set of span lengths.
        अगर(which&UTF8) {
            // UTF-8 lengths and UTF-8 strings.
            alloc_आकार_2+=strings_लंबाई*4+utf8_लंबाई;
        }
    }
    अगर (alloc_आकार_2 <= static_cast<int32_t>(माप(staticLengths))) {
        utf8Lengths=staticLengths;
    } अन्यथा {
        utf8Lengths = static_cast<int32_t*>(uprv_malloc(alloc_आकार_2));
        अगर(utf8Lengths==nullptr) {
            maxLength16=maxLength8=0;  // Prevent usage by making needsStringSpanUTF16/8() return false.
            वापस;  // Out of memory.
        }
    }

    अगर(all) {
        // Store span lengths for all span() variants.
        spanLengths = reinterpret_cast<uint8_t*>(utf8Lengths + strings_लंबाई);
        spanBackLengths=spanLengths+strings_लंबाई;
        spanUTF8Lengths=spanBackLengths+strings_लंबाई;
        spanBackUTF8Lengths=spanUTF8Lengths+strings_लंबाई;
        utf8=spanBackUTF8Lengths+strings_लंबाई;
    } अन्यथा {
        // Store span lengths for only one span() variant.
        अगर(which&UTF8) {
            spanLengths = reinterpret_cast<uint8_t*>(utf8Lengths + strings_लंबाई);
            utf8=spanLengths+strings_लंबाई;
        } अन्यथा {
            spanLengths = reinterpret_cast<uint8_t*>(utf8Lengths);
        }
        spanBackLengths=spanUTF8Lengths=spanBackUTF8Lengths=spanLengths;
    }

    // Set the meta data and pSpanNotSet and write the UTF-8 strings.
    int32_t utf8_गिनती=0;  // Count UTF-8 bytes written so far.

    क्रम(i=0; i<strings_लंबाई; ++i) {
        स्थिर UnicodeString& string = *static_cast<स्थिर UnicodeString*>(strings.elementAt(i));
        स्थिर char16_t *s16=string.get_बफर();
        int32_t length16=string.length();
        span_लंबाई=spanSet.span(s16, length16, USET_SPAN_CONTAINED);
        अगर(span_लंबाई<length16 && length16>0) {  // Relevant string.
            अगर(which&UTF16) {
                अगर(which&CONTAINED) {
                    अगर(which&FWD) {
                        spanLengths[i]=make_Span_लंबाई_Byte(span_लंबाई);
                    }
                    अगर(which&BACK) {
                        span_लंबाई=length16-spanSet.spanBack(s16, length16, USET_SPAN_CONTAINED);
                        spanBackLengths[i]=make_Span_लंबाई_Byte(span_लंबाई);
                    }
                } अन्यथा /* not CONTAINED, not all, but NOT_CONTAINED */ {
                    spanLengths[i]=spanBackLengths[i]=0;  // Only store a relevant/irrelevant flag.
                }
            }
            अगर(which&UTF8) {
                uint8_t *s8=utf8+utf8_गिनती;
                int32_t length8=appendUTF8(s16, length16, s8, utf8_लंबाई-utf8_गिनती);
                utf8_गिनती+=utf8Lengths[i]=length8;
                अगर(length8==0) {  // Irrelevant for UTF-8 because not representable in UTF-8.
                    spanUTF8Lengths[i] = spanBackUTF8Lengths[i] = static_cast<uint8_t>(ALL_CP_CONTAINED);
                } अन्यथा {  // Relevant for UTF-8.
                    अगर(which&CONTAINED) {
                        अगर(which&FWD) {
                            span_लंबाई = spanSet.spanUTF8(reinterpret_cast<स्थिर अक्षर*>(s8), length8, USET_SPAN_CONTAINED);
                            spanUTF8Lengths[i]=make_Span_लंबाई_Byte(span_लंबाई);
                        }
                        अगर(which&BACK) {
                            span_लंबाई = length8 - spanSet.spanBackUTF8(reinterpret_cast<स्थिर अक्षर*>(s8), length8, USET_SPAN_CONTAINED);
                            spanBackUTF8Lengths[i]=make_Span_लंबाई_Byte(span_लंबाई);
                        }
                    } अन्यथा /* not CONTAINED, not all, but NOT_CONTAINED */ {
                        spanUTF8Lengths[i]=spanBackUTF8Lengths[i]=0;  // Only store a relevant/irrelevant flag.
                    }
                }
            }
            अगर(which&NOT_CONTAINED) {
                // Add string start and end code points to the spanNotSet so that
                // a span(while not contained) stops before any string.
                UChar32 c;
                अगर(which&FWD) {
                    int32_t len=0;
                    U16_अग्र(s16, len, length16, c);
                    addToSpanNotSet(c);
                }
                अगर(which&BACK) {
                    int32_t len=length16;
                    U16_पूर्व(s16, 0, len, c);
                    addToSpanNotSet(c);
                }
            }
        } अन्यथा {  // Irrelevant string. (Also the empty string.)
            अगर(which&UTF8) {
                अगर(which&CONTAINED) {  // Only necessary for LONGEST_MATCH.
                    uint8_t *s8=utf8+utf8_गिनती;
                    int32_t length8=appendUTF8(s16, length16, s8, utf8_लंबाई-utf8_गिनती);
                    utf8_गिनती+=utf8Lengths[i]=length8;
                } अन्यथा {
                    utf8Lengths[i]=0;
                }
            }
            अगर(all) {
                spanLengths[i]=spanBackLengths[i]=
                    spanUTF8Lengths[i]=spanBackUTF8Lengths[i]=
                        static_cast<uint8_t>(ALL_CP_CONTAINED);
            } अन्यथा {
                // All spanXYZLengths pointers contain the same address.
                spanLengths[i] = static_cast<uint8_t>(ALL_CP_CONTAINED);
            }
        }
    }

    // Finish.
    अगर(all) {
        pSpanNotSet->freeze();
    }
}

// Copy constructor. Assumes which==ALL for a frozen set.
UnicodeSetStringSpan::UnicodeSetStringSpan(स्थिर UnicodeSetStringSpan &otherStringSpan,
                                           स्थिर UVector &newParentSetStrings)
        : spanSet(otherStringSpan.spanSet), pSpanNotSet(nullptr), strings(newParentSetStrings),
          utf8Lengths(nullptr), spanLengths(nullptr), utf8(nullptr),
          utf8_लंबाई(otherStringSpan.utf8_लंबाई),
          maxLength16(otherStringSpan.maxLength16), maxLength8(otherStringSpan.maxLength8),
          all(true) {
    अगर(otherStringSpan.pSpanNotSet==&otherStringSpan.spanSet) {
        pSpanNotSet=&spanSet;
    } अन्यथा {
        pSpanNotSet=otherStringSpan.pSpanNotSet->clone();
    }

    // Allocate a block of meta data.
    // UTF-8 lengths, 4 sets of span lengths, UTF-8 strings.
    int32_t strings_लंबाई=strings.size();
    int32_t alloc_आकार_2=strings_लंबाई*(4+1+1+1+1)+utf8_लंबाई;
    अगर (alloc_आकार_2 <= static_cast<int32_t>(माप(staticLengths))) {
        utf8Lengths=staticLengths;
    } अन्यथा {
        utf8Lengths = static_cast<int32_t*>(uprv_malloc(alloc_आकार_2));
        अगर(utf8Lengths==nullptr) {
            maxLength16=maxLength8=0;  // Prevent usage by making needsStringSpanUTF16/8() return false.
            वापस;  // Out of memory.
        }
    }

    spanLengths = reinterpret_cast<uint8_t*>(utf8Lengths + strings_लंबाई);
    utf8=spanLengths+strings_लंबाई*4;
    uprv_memcpy(utf8Lengths, otherStringSpan.utf8Lengths, alloc_आकार_2);
}

UnicodeSetStringSpan::~UnicodeSetStringSpan() {
    अगर(pSpanNotSet!=nullptr && pSpanNotSet!=&spanSet) {
        मिटाओ pSpanNotSet;
    }
    अगर(utf8Lengths!=nullptr && utf8Lengths!=staticLengths) {
        uprv_free(utf8Lengths);
    }
}

व्योम UnicodeSetStringSpan::addToSpanNotSet(UChar32 c) {
    अगर(pSpanNotSet==nullptr || pSpanNotSet==&spanSet) {
        अगर(spanSet.contains(c)) {
            वापस;  // Nothing to do.
        }
        UnicodeSet *newSet=spanSet.cloneAsThawed();
        अगर(newSet==nullptr) {
            वापस;  // Out of memory.
        } अन्यथा {
            pSpanNotSet=newSet;
        }
    }
    pSpanNotSet->add(c);
}

// Compare strings without any argument checks. Requires length>0.
अटल अंतरभूत UBool
matches16(स्थिर char16_t *s, स्थिर char16_t *t, int32_t length) {
    करो {
        अगर(*s++!=*t++) {
            वापस false;
        }
    } जबतक(--length>0);
    वापस true;
}

अटल अंतरभूत UBool
matches8(स्थिर uint8_t *s, स्थिर uint8_t *t, int32_t length) {
    करो {
        अगर(*s++!=*t++) {
            वापस false;
        }
    } जबतक(--length>0);
    वापस true;
}

// Compare 16-bit Unicode strings (which may be malformed UTF-16)
// at code point boundaries.
// That is, each edge of a match must not be in the middle of a surrogate pair.
अटल अंतरभूत UBool
matches16CPB(स्थिर char16_t *s, int32_t start, int32_t limit, स्थिर char16_t *t, int32_t length) {
    s+=start;
    limit-=start;
    वापस matches16(s, t, length) &&
           !(0<start && U16_IS_LEAD(s[-1]) && U16_IS_TRAIL(s[0])) &&
           !(length<limit && U16_IS_LEAD(s[length-1]) && U16_IS_TRAIL(s[length]));
}

// Does the set contain the next code point?
// If so, return its length; otherwise return its negative length.
अटल अंतरभूत int32_t
spanOne(स्थिर UnicodeSet &set, स्थिर char16_t *s, int32_t length) {
    char16_t c=*s, c2;
    अगर(c>=0xd800 && c<=0xdbff && length>=2 && U16_IS_TRAIL(c2=s[1])) {
        वापस set.contains(U16_GET_SUPPLEMENTARY(c, c2)) ? 2 : -2;
    }
    वापस set.contains(c) ? 1 : -1;
}

अटल अंतरभूत int32_t
spanOneBack(स्थिर UnicodeSet &set, स्थिर char16_t *s, int32_t length) {
    char16_t c=s[length-1], c2;
    अगर(c>=0xdc00 && c<=0xdfff && length>=2 && U16_IS_LEAD(c2=s[length-2])) {
        वापस set.contains(U16_GET_SUPPLEMENTARY(c2, c)) ? 2 : -2;
    }
    वापस set.contains(c) ? 1 : -1;
}

अटल अंतरभूत int32_t
spanOneUTF8(स्थिर UnicodeSet &set, स्थिर uint8_t *s, int32_t length) {
    UChar32 c=*s;
    अगर(U8_IS_SINGLE(c)) {
        वापस set.contains(c) ? 1 : -1;
    }
    // Take advantage of non-ASCII fastpaths in U8_NEXT_OR_FFFD().
    int32_t i=0;
    U8_अग्र_OR_FFFD(s, i, length, c);
    वापस set.contains(c) ? i : -i;
}

अटल अंतरभूत int32_t
spanOneBackUTF8(स्थिर UnicodeSet &set, स्थिर uint8_t *s, int32_t length) {
    UChar32 c=s[length-1];
    अगर(U8_IS_SINGLE(c)) {
        वापस set.contains(c) ? 1 : -1;
    }
    int32_t i=length-1;
    c=utf8_पूर्व_Char_Safe_Body(s, 0, &i, c, -3);
    length-=i;
    वापस set.contains(c) ? length : -length;
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

int32_t UnicodeSetStringSpan::span(स्थिर char16_t *s, int32_t length, USetSpanCondition spanCondition) स्थिर {
    अगर(spanCondition==USET_SPAN_NOT_CONTAINED) {
        वापस spanNot(s, length);
    }
    int32_t span_लंबाई=spanSet.span(s, length, USET_SPAN_CONTAINED);
    अगर(span_लंबाई==length) {
        वापस length;
    }

    // Consider strings; they may overlap with the span.
    OffsetList offsets;
    अगर(spanCondition==USET_SPAN_CONTAINED) {
        // Use offset list to try all possibilities.
        offsets.set_Max_लंबाई(maxLength16);
    }
    int32_t pos=span_लंबाई, rest=length-pos;
    int32_t i, strings_लंबाई=strings.size();
    क्रम(;;) {
        अगर(spanCondition==USET_SPAN_CONTAINED) {
            क्रम(i=0; i<strings_लंबाई; ++i) {
                int32_t overlap=spanLengths[i];
                अगर(overlap==ALL_CP_CONTAINED) {
                    जारी;  // Irrelevant string. (Also the empty string.)
                }
                स्थिर UnicodeString& string = *static_cast<स्थिर UnicodeString*>(strings.elementAt(i));
                स्थिर char16_t *s16=string.get_बफर();
                int32_t length16=string.length();
                U_ASSERT(length>0);

                // Try to match this string at pos-overlap..pos.
                अगर(overlap>=LONG_SPAN) {
                    overlap=length16;
                    // While contained: No point matching fully inside the code point span.
                    U16_BACK_1(s16, 0, overlap);  // Length of the string minus the last code point.
                }
                अगर(overlap>span_लंबाई) {
                    overlap=span_लंबाई;
                }
                int32_t inc=length16-overlap;  // Keep overlap+inc==length16.
                क्रम(;;) {
                    अगर(inc>rest) {
                        अवरोध;
                    }
                    // Try to match if the increment is not listed already.
                    अगर(!offsets.containsOffset(inc) && matches16CPB(s, pos-overlap, length, s16, length16)) {
                        अगर(inc==rest) {
                            वापस length;  // Reached the end of the string.
                        }
                        offsets.addOffset(inc);
                    }
                    अगर(overlap==0) {
                        अवरोध;
                    }
                    --overlap;
                    ++inc;
                }
            }
        } अन्यथा /* USET_SPAN_SIMPLE */ {
            int32_t maxInc=0, maxOverlap=0;
            क्रम(i=0; i<strings_लंबाई; ++i) {
                int32_t overlap=spanLengths[i];
                // For longest match, we do need to try to match even an all-contained string
                // to find the match from the earliest start.

                स्थिर UnicodeString& string = *static_cast<स्थिर UnicodeString*>(strings.elementAt(i));
                स्थिर char16_t *s16=string.get_बफर();
                int32_t length16=string.length();
                अगर (length16==0) {
                    जारी;  // skip the empty string
                }

                // Try to match this string at pos-overlap..pos.
                अगर(overlap>=LONG_SPAN) {
                    overlap=length16;
                    // Longest match: Need to match fully inside the code point span
                    // to find the match from the earliest start.
                }
                अगर(overlap>span_लंबाई) {
                    overlap=span_लंबाई;
                }
                int32_t inc=length16-overlap;  // Keep overlap+inc==length16.
                क्रम(;;) {
                    अगर(inc>rest || overlap<maxOverlap) {
                        अवरोध;
                    }
                    // Try to match if the string is longer or starts earlier.
                    अगर( (overlap>maxOverlap || /* redundant overlap==maxOverlap && */ inc>maxInc) &&
                        matches16CPB(s, pos-overlap, length, s16, length16)
                    ) {
                        maxInc=inc;  // Longest match from earliest start.
                        maxOverlap=overlap;
                        अवरोध;
                    }
                    --overlap;
                    ++inc;
                }
            }

            अगर(maxInc!=0 || maxOverlap!=0) {
                // Longest-match algorithm, and there was a string match.
                // Simply continue after it.
                pos+=maxInc;
                rest-=maxInc;
                अगर(rest==0) {
                    वापस length;  // Reached the end of the string.
                }
                span_लंबाई=0;  // Match strings from after a string match.
                जारी;
            }
        }
        // Finished trying to match all strings at pos.

        अगर(span_लंबाई!=0 || pos==0) {
            // The position is after an unlimited code point span (spanLength!=0),
            // not after a string match.
            // The only position where spanLength==0 after a span is pos==0.
            // Otherwise, an unlimited code point span is only tried again when no
            // strings match, and if such a non-initial span fails we stop.
            अगर(offsets.isEmpty()) {
                वापस pos;  // No strings matched after a span.
            }
            // Match strings from after the next string match.
        } अन्यथा {
            // The position is after a string match (or a single code point).
            अगर(offsets.isEmpty()) {
                // No more strings matched after a previous string match.
                // Try another code point span from after the last string match.
                span_लंबाई=spanSet.span(s+pos, rest, USET_SPAN_CONTAINED);
                अगर( span_लंबाई==rest || // Reached the end of the string, or
                    span_लंबाई==0       // neither strings nor span progressed.
                ) {
                    वापस pos+span_लंबाई;
                }
                pos+=span_लंबाई;
                rest-=span_लंबाई;
                जारी;  // spanLength>0: Match strings from after a span.
            } अन्यथा {
                // Try to match only one code point from after a string match if some
                // string matched beyond it, so that we try all possible positions
                // and don't overshoot.
                span_लंबाई=spanOne(spanSet, s+pos, rest);
                अगर(span_लंबाई>0) {
                    अगर(span_लंबाई==rest) {
                        वापस length;  // Reached the end of the string.
                    }
                    // Match strings after this code point.
                    // There cannot be any increments below it because UnicodeSet strings
                    // contain multiple code points.
                    pos+=span_लंबाई;
                    rest-=span_लंबाई;
                    offsets.shift(span_लंबाई);
                    span_लंबाई=0;
                    जारी;  // Match strings from after a single code point.
                }
                // Match strings from after the next string match.
            }
        }
        int32_t minOffset=offsets.निकालो_Minimum();
        pos+=minOffset;
        rest-=minOffset;
        span_लंबाई=0;  // Match strings from after a string match.
    }
}

int32_t UnicodeSetStringSpan::spanBack(स्थिर char16_t *s, int32_t length, USetSpanCondition spanCondition) स्थिर {
    अगर(spanCondition==USET_SPAN_NOT_CONTAINED) {
        वापस spanNotBack(s, length);
    }
    int32_t pos=spanSet.spanBack(s, length, USET_SPAN_CONTAINED);
    अगर(pos==0) {
        वापस 0;
    }
    int32_t span_लंबाई=length-pos;

    // Consider strings; they may overlap with the span.
    OffsetList offsets;
    अगर(spanCondition==USET_SPAN_CONTAINED) {
        // Use offset list to try all possibilities.
        offsets.set_Max_लंबाई(maxLength16);
    }
    int32_t i, strings_लंबाई=strings.size();
    uint8_t *spanBackLengths=spanLengths;
    अगर(all) {
        spanBackLengths+=strings_लंबाई;
    }
    क्रम(;;) {
        अगर(spanCondition==USET_SPAN_CONTAINED) {
            क्रम(i=0; i<strings_लंबाई; ++i) {
                int32_t overlap=spanBackLengths[i];
                अगर(overlap==ALL_CP_CONTAINED) {
                    जारी;  // Irrelevant string. (Also the empty string.)
                }
                स्थिर UnicodeString& string = *static_cast<स्थिर UnicodeString*>(strings.elementAt(i));
                स्थिर char16_t *s16=string.get_बफर();
                int32_t length16=string.length();
                U_ASSERT(length>0);

                // Try to match this string at pos-(length16-overlap)..pos-length16.
                अगर(overlap>=LONG_SPAN) {
                    overlap=length16;
                    // While contained: No point matching fully inside the code point span.
                    int32_t len1=0;
                    U16_FWD_1(s16, len1, overlap);
                    overlap-=len1;  // Length of the string minus the first code point.
                }
                अगर(overlap>span_लंबाई) {
                    overlap=span_लंबाई;
                }
                int32_t डेसी=length16-overlap;  // Keep dec+overlap==length16.
                क्रम(;;) {
                    अगर(डेसी>pos) {
                        अवरोध;
                    }
                    // Try to match if the decrement is not listed already.
                    अगर(!offsets.containsOffset(डेसी) && matches16CPB(s, pos-डेसी, length, s16, length16)) {
                        अगर(डेसी==pos) {
                            वापस 0;  // Reached the start of the string.
                        }
                        offsets.addOffset(डेसी);
                    }
                    अगर(overlap==0) {
                        अवरोध;
                    }
                    --overlap;
                    ++डेसी;
                }
            }
        } अन्यथा /* USET_SPAN_SIMPLE */ {
            int32_t maxDec=0, maxOverlap=0;
            क्रम(i=0; i<strings_लंबाई; ++i) {
                int32_t overlap=spanBackLengths[i];
                // For longest match, we do need to try to match even an all-contained string
                // to find the match from the latest end.

                स्थिर UnicodeString& string = *static_cast<स्थिर UnicodeString*>(strings.elementAt(i));
                स्थिर char16_t *s16=string.get_बफर();
                int32_t length16=string.length();
                अगर (length16==0) {
                    जारी;  // skip the empty string
                }

                // Try to match this string at pos-(length16-overlap)..pos-length16.
                अगर(overlap>=LONG_SPAN) {
                    overlap=length16;
                    // Longest match: Need to match fully inside the code point span
                    // to find the match from the latest end.
                }
                अगर(overlap>span_लंबाई) {
                    overlap=span_लंबाई;
                }
                int32_t डेसी=length16-overlap;  // Keep dec+overlap==length16.
                क्रम(;;) {
                    अगर(डेसी>pos || overlap<maxOverlap) {
                        अवरोध;
                    }
                    // Try to match if the string is longer or ends later.
                    अगर( (overlap>maxOverlap || /* redundant overlap==maxOverlap && */ डेसी>maxDec) &&
                        matches16CPB(s, pos-डेसी, length, s16, length16)
                    ) {
                        maxDec=डेसी;  // Longest match from latest end.
                        maxOverlap=overlap;
                        अवरोध;
                    }
                    --overlap;
                    ++डेसी;
                }
            }

            अगर(maxDec!=0 || maxOverlap!=0) {
                // Longest-match algorithm, and there was a string match.
                // Simply continue before it.
                pos-=maxDec;
                अगर(pos==0) {
                    वापस 0;  // Reached the start of the string.
                }
                span_लंबाई=0;  // Match strings from before a string match.
                जारी;
            }
        }
        // Finished trying to match all strings at pos.

        अगर(span_लंबाई!=0 || pos==length) {
            // The position is before an unlimited code point span (spanLength!=0),
            // not before a string match.
            // The only position where spanLength==0 before a span is pos==length.
            // Otherwise, an unlimited code point span is only tried again when no
            // strings match, and if such a non-initial span fails we stop.
            अगर(offsets.isEmpty()) {
                वापस pos;  // No strings matched before a span.
            }
            // Match strings from before the next string match.
        } अन्यथा {
            // The position is before a string match (or a single code point).
            अगर(offsets.isEmpty()) {
                // No more strings matched before a previous string match.
                // Try another code point span from before the last string match.
                int32_t oldPos=pos;
                pos=spanSet.spanBack(s, oldPos, USET_SPAN_CONTAINED);
                span_लंबाई=oldPos-pos;
                अगर( pos==0 ||           // Reached the start of the string, or
                    span_लंबाई==0       // neither strings nor span progressed.
                ) {
                    वापस pos;
                }
                जारी;  // spanLength>0: Match strings from before a span.
            } अन्यथा {
                // Try to match only one code point from before a string match if some
                // string matched beyond it, so that we try all possible positions
                // and don't overshoot.
                span_लंबाई=spanOneBack(spanSet, s, pos);
                अगर(span_लंबाई>0) {
                    अगर(span_लंबाई==pos) {
                        वापस 0;  // Reached the start of the string.
                    }
                    // Match strings before this code point.
                    // There cannot be any decrements below it because UnicodeSet strings
                    // contain multiple code points.
                    pos-=span_लंबाई;
                    offsets.shift(span_लंबाई);
                    span_लंबाई=0;
                    जारी;  // Match strings from before a single code point.
                }
                // Match strings from before the next string match.
            }
        }
        pos-=offsets.निकालो_Minimum();
        span_लंबाई=0;  // Match strings from before a string match.
    }
}

int32_t UnicodeSetStringSpan::spanUTF8(स्थिर uint8_t *s, int32_t length, USetSpanCondition spanCondition) स्थिर {
    अगर(spanCondition==USET_SPAN_NOT_CONTAINED) {
        वापस spanNotUTF8(s, length);
    }
    int32_t span_लंबाई = spanSet.spanUTF8(reinterpret_cast<स्थिर अक्षर*>(s), length, USET_SPAN_CONTAINED);
    अगर(span_लंबाई==length) {
        वापस length;
    }

    // Consider strings; they may overlap with the span.
    OffsetList offsets;
    अगर(spanCondition==USET_SPAN_CONTAINED) {
        // Use offset list to try all possibilities.
        offsets.set_Max_लंबाई(maxLength8);
    }
    int32_t pos=span_लंबाई, rest=length-pos;
    int32_t i, strings_लंबाई=strings.size();
    uint8_t *spanUTF8Lengths=spanLengths;
    अगर(all) {
        spanUTF8Lengths+=2*strings_लंबाई;
    }
    क्रम(;;) {
        स्थिर uint8_t *s8=utf8;
        int32_t length8;
        अगर(spanCondition==USET_SPAN_CONTAINED) {
            क्रम(i=0; i<strings_लंबाई; ++i) {
                length8=utf8Lengths[i];
                अगर(length8==0) {
                    जारी;  // String not representable in UTF-8.
                }
                int32_t overlap=spanUTF8Lengths[i];
                अगर(overlap==ALL_CP_CONTAINED) {
                    s8+=length8;
                    जारी;  // Irrelevant string.
                }

                // Try to match this string at pos-overlap..pos.
                अगर(overlap>=LONG_SPAN) {
                    overlap=length8;
                    // While contained: No point matching fully inside the code point span.
                    U8_BACK_1(s8, 0, overlap);  // Length of the string minus the last code point.
                }
                अगर(overlap>span_लंबाई) {
                    overlap=span_लंबाई;
                }
                int32_t inc=length8-overlap;  // Keep overlap+inc==length8.
                क्रम(;;) {
                    अगर(inc>rest) {
                        अवरोध;
                    }
                    // Try to match if the increment is not listed already.
                    // Match at code point boundaries. (The UTF-8 strings were converted
                    // from UTF-16 and are guaranteed to be well-formed.)
                    अगर(!U8_IS_TRAIL(s[pos-overlap]) &&
                            !offsets.containsOffset(inc) &&
                            matches8(s+pos-overlap, s8, length8)) {
                        अगर(inc==rest) {
                            वापस length;  // Reached the end of the string.
                        }
                        offsets.addOffset(inc);
                    }
                    अगर(overlap==0) {
                        अवरोध;
                    }
                    --overlap;
                    ++inc;
                }
                s8+=length8;
            }
        } अन्यथा /* USET_SPAN_SIMPLE */ {
            int32_t maxInc=0, maxOverlap=0;
            क्रम(i=0; i<strings_लंबाई; ++i) {
                length8=utf8Lengths[i];
                अगर(length8==0) {
                    जारी;  // String not representable in UTF-8.
                }
                int32_t overlap=spanUTF8Lengths[i];
                // For longest match, we do need to try to match even an all-contained string
                // to find the match from the earliest start.

                // Try to match this string at pos-overlap..pos.
                अगर(overlap>=LONG_SPAN) {
                    overlap=length8;
                    // Longest match: Need to match fully inside the code point span
                    // to find the match from the earliest start.
                }
                अगर(overlap>span_लंबाई) {
                    overlap=span_लंबाई;
                }
                int32_t inc=length8-overlap;  // Keep overlap+inc==length8.
                क्रम(;;) {
                    अगर(inc>rest || overlap<maxOverlap) {
                        अवरोध;
                    }
                    // Try to match if the string is longer or starts earlier.
                    // Match at code point boundaries. (The UTF-8 strings were converted
                    // from UTF-16 and are guaranteed to be well-formed.)
                    अगर(!U8_IS_TRAIL(s[pos-overlap]) &&
                            (overlap>maxOverlap ||
                                /* redundant overlap==maxOverlap && */ inc>maxInc) &&
                            matches8(s+pos-overlap, s8, length8)) {
                        maxInc=inc;  // Longest match from earliest start.
                        maxOverlap=overlap;
                        अवरोध;
                    }
                    --overlap;
                    ++inc;
                }
                s8+=length8;
            }

            अगर(maxInc!=0 || maxOverlap!=0) {
                // Longest-match algorithm, and there was a string match.
                // Simply continue after it.
                pos+=maxInc;
                rest-=maxInc;
                अगर(rest==0) {
                    वापस length;  // Reached the end of the string.
                }
                span_लंबाई=0;  // Match strings from after a string match.
                जारी;
            }
        }
        // Finished trying to match all strings at pos.

        अगर(span_लंबाई!=0 || pos==0) {
            // The position is after an unlimited code point span (spanLength!=0),
            // not after a string match.
            // The only position where spanLength==0 after a span is pos==0.
            // Otherwise, an unlimited code point span is only tried again when no
            // strings match, and if such a non-initial span fails we stop.
            अगर(offsets.isEmpty()) {
                वापस pos;  // No strings matched after a span.
            }
            // Match strings from after the next string match.
        } अन्यथा {
            // The position is after a string match (or a single code point).
            अगर(offsets.isEmpty()) {
                // No more strings matched after a previous string match.
                // Try another code point span from after the last string match.
                span_लंबाई = spanSet.spanUTF8(reinterpret_cast<स्थिर अक्षर*>(s) + pos, rest, USET_SPAN_CONTAINED);
                अगर( span_लंबाई==rest || // Reached the end of the string, or
                    span_लंबाई==0       // neither strings nor span progressed.
                ) {
                    वापस pos+span_लंबाई;
                }
                pos+=span_लंबाई;
                rest-=span_लंबाई;
                जारी;  // spanLength>0: Match strings from after a span.
            } अन्यथा {
                // Try to match only one code point from after a string match if some
                // string matched beyond it, so that we try all possible positions
                // and don't overshoot.
                span_लंबाई=spanOneUTF8(spanSet, s+pos, rest);
                अगर(span_लंबाई>0) {
                    अगर(span_लंबाई==rest) {
                        वापस length;  // Reached the end of the string.
                    }
                    // Match strings after this code point.
                    // There cannot be any increments below it because UnicodeSet strings
                    // contain multiple code points.
                    pos+=span_लंबाई;
                    rest-=span_लंबाई;
                    offsets.shift(span_लंबाई);
                    span_लंबाई=0;
                    जारी;  // Match strings from after a single code point.
                }
                // Match strings from after the next string match.
            }
        }
        int32_t minOffset=offsets.निकालो_Minimum();
        pos+=minOffset;
        rest-=minOffset;
        span_लंबाई=0;  // Match strings from after a string match.
    }
}

int32_t UnicodeSetStringSpan::spanBackUTF8(स्थिर uint8_t *s, int32_t length, USetSpanCondition spanCondition) स्थिर {
    अगर(spanCondition==USET_SPAN_NOT_CONTAINED) {
        वापस spanNotBackUTF8(s, length);
    }
    int32_t pos = spanSet.spanBackUTF8(reinterpret_cast<स्थिर अक्षर*>(s), length, USET_SPAN_CONTAINED);
    अगर(pos==0) {
        वापस 0;
    }
    int32_t span_लंबाई=length-pos;

    // Consider strings; they may overlap with the span.
    OffsetList offsets;
    अगर(spanCondition==USET_SPAN_CONTAINED) {
        // Use offset list to try all possibilities.
        offsets.set_Max_लंबाई(maxLength8);
    }
    int32_t i, strings_लंबाई=strings.size();
    uint8_t *spanBackUTF8Lengths=spanLengths;
    अगर(all) {
        spanBackUTF8Lengths+=3*strings_लंबाई;
    }
    क्रम(;;) {
        स्थिर uint8_t *s8=utf8;
        int32_t length8;
        अगर(spanCondition==USET_SPAN_CONTAINED) {
            क्रम(i=0; i<strings_लंबाई; ++i) {
                length8=utf8Lengths[i];
                अगर(length8==0) {
                    जारी;  // String not representable in UTF-8.
                }
                int32_t overlap=spanBackUTF8Lengths[i];
                अगर(overlap==ALL_CP_CONTAINED) {
                    s8+=length8;
                    जारी;  // Irrelevant string.
                }

                // Try to match this string at pos-(length8-overlap)..pos-length8.
                अगर(overlap>=LONG_SPAN) {
                    overlap=length8;
                    // While contained: No point matching fully inside the code point span.
                    int32_t len1=0;
                    U8_FWD_1(s8, len1, overlap);
                    overlap-=len1;  // Length of the string minus the first code point.
                }
                अगर(overlap>span_लंबाई) {
                    overlap=span_लंबाई;
                }
                int32_t डेसी=length8-overlap;  // Keep dec+overlap==length8.
                क्रम(;;) {
                    अगर(डेसी>pos) {
                        अवरोध;
                    }
                    // Try to match if the decrement is not listed already.
                    // Match at code point boundaries. (The UTF-8 strings were converted
                    // from UTF-16 and are guaranteed to be well-formed.)
                    अगर( !U8_IS_TRAIL(s[pos-डेसी]) &&
                        !offsets.containsOffset(डेसी) &&
                        matches8(s+pos-डेसी, s8, length8)
                    ) {
                        अगर(डेसी==pos) {
                            वापस 0;  // Reached the start of the string.
                        }
                        offsets.addOffset(डेसी);
                    }
                    अगर(overlap==0) {
                        अवरोध;
                    }
                    --overlap;
                    ++डेसी;
                }
                s8+=length8;
            }
        } अन्यथा /* USET_SPAN_SIMPLE */ {
            int32_t maxDec=0, maxOverlap=0;
            क्रम(i=0; i<strings_लंबाई; ++i) {
                length8=utf8Lengths[i];
                अगर(length8==0) {
                    जारी;  // String not representable in UTF-8.
                }
                int32_t overlap=spanBackUTF8Lengths[i];
                // For longest match, we do need to try to match even an all-contained string
                // to find the match from the latest end.

                // Try to match this string at pos-(length8-overlap)..pos-length8.
                अगर(overlap>=LONG_SPAN) {
                    overlap=length8;
                    // Longest match: Need to match fully inside the code point span
                    // to find the match from the latest end.
                }
                अगर(overlap>span_लंबाई) {
                    overlap=span_लंबाई;
                }
                int32_t डेसी=length8-overlap;  // Keep dec+overlap==length8.
                क्रम(;;) {
                    अगर(डेसी>pos || overlap<maxOverlap) {
                        अवरोध;
                    }
                    // Try to match if the string is longer or ends later.
                    // Match at code point boundaries. (The UTF-8 strings were converted
                    // from UTF-16 and are guaranteed to be well-formed.)
                    अगर( !U8_IS_TRAIL(s[pos-डेसी]) &&
                        (overlap>maxOverlap || /* redundant overlap==maxOverlap && */ डेसी>maxDec) &&
                        matches8(s+pos-डेसी, s8, length8)
                    ) {
                        maxDec=डेसी;  // Longest match from latest end.
                        maxOverlap=overlap;
                        अवरोध;
                    }
                    --overlap;
                    ++डेसी;
                }
                s8+=length8;
            }

            अगर(maxDec!=0 || maxOverlap!=0) {
                // Longest-match algorithm, and there was a string match.
                // Simply continue before it.
                pos-=maxDec;
                अगर(pos==0) {
                    वापस 0;  // Reached the start of the string.
                }
                span_लंबाई=0;  // Match strings from before a string match.
                जारी;
            }
        }
        // Finished trying to match all strings at pos.

        अगर(span_लंबाई!=0 || pos==length) {
            // The position is before an unlimited code point span (spanLength!=0),
            // not before a string match.
            // The only position where spanLength==0 before a span is pos==length.
            // Otherwise, an unlimited code point span is only tried again when no
            // strings match, and if such a non-initial span fails we stop.
            अगर(offsets.isEmpty()) {
                वापस pos;  // No strings matched before a span.
            }
            // Match strings from before the next string match.
        } अन्यथा {
            // The position is before a string match (or a single code point).
            अगर(offsets.isEmpty()) {
                // No more strings matched before a previous string match.
                // Try another code point span from before the last string match.
                int32_t oldPos=pos;
                pos = spanSet.spanBackUTF8(reinterpret_cast<स्थिर अक्षर*>(s), oldPos, USET_SPAN_CONTAINED);
                span_लंबाई=oldPos-pos;
                अगर( pos==0 ||           // Reached the start of the string, or
                    span_लंबाई==0       // neither strings nor span progressed.
                ) {
                    वापस pos;
                }
                जारी;  // spanLength>0: Match strings from before a span.
            } अन्यथा {
                // Try to match only one code point from before a string match if some
                // string matched beyond it, so that we try all possible positions
                // and don't overshoot.
                span_लंबाई=spanOneBackUTF8(spanSet, s, pos);
                अगर(span_लंबाई>0) {
                    अगर(span_लंबाई==pos) {
                        वापस 0;  // Reached the start of the string.
                    }
                    // Match strings before this code point.
                    // There cannot be any decrements below it because UnicodeSet strings
                    // contain multiple code points.
                    pos-=span_लंबाई;
                    offsets.shift(span_लंबाई);
                    span_लंबाई=0;
                    जारी;  // Match strings from before a single code point.
                }
                // Match strings from before the next string match.
            }
        }
        pos-=offsets.निकालो_Minimum();
        span_लंबाई=0;  // Match strings from before a string match.
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

int32_t UnicodeSetStringSpan::spanNot(स्थिर char16_t *s, int32_t length) स्थिर {
    int32_t pos=0, rest=length;
    int32_t i, strings_लंबाई=strings.size();
    करो {
        // Span until we find a code point from the set,
        // or a code point that starts or ends some string.
        i=pSpanNotSet->span(s+pos, rest, USET_SPAN_NOT_CONTAINED);
        अगर(i==rest) {
            वापस length;  // Reached the end of the string.
        }
        pos+=i;
        rest-=i;

        // Check whether the current code point is in the original set,
        // without the string starts and ends.
        int32_t cp_लंबाई=spanOne(spanSet, s+pos, rest);
        अगर(cp_लंबाई>0) {
            वापस pos;  // There is a set element at pos.
        }

        // Try to match the strings at pos.
        क्रम(i=0; i<strings_लंबाई; ++i) {
            अगर(spanLengths[i]==ALL_CP_CONTAINED) {
                जारी;  // Irrelevant string. (Also the empty string.)
            }
            स्थिर UnicodeString& string = *static_cast<स्थिर UnicodeString*>(strings.elementAt(i));
            स्थिर char16_t *s16=string.get_बफर();
            int32_t length16=string.length();
            U_ASSERT(length>0);
            अगर(length16<=rest && matches16CPB(s, pos, length, s16, length16)) {
                वापस pos;  // There is a set element at pos.
            }
        }

        // The span(while not contained) ended on a string start/end which is
        // not in the original set. Skip this code point and continue.
        // cpLength<0
        pos-=cp_लंबाई;
        rest+=cp_लंबाई;
    } जबतक(rest!=0);
    वापस length;  // Reached the end of the string.
}

int32_t UnicodeSetStringSpan::spanNotBack(स्थिर char16_t *s, int32_t length) स्थिर {
    int32_t pos=length;
    int32_t i, strings_लंबाई=strings.size();
    करो {
        // Span until we find a code point from the set,
        // or a code point that starts or ends some string.
        pos=pSpanNotSet->spanBack(s, pos, USET_SPAN_NOT_CONTAINED);
        अगर(pos==0) {
            वापस 0;  // Reached the start of the string.
        }

        // Check whether the current code point is in the original set,
        // without the string starts and ends.
        int32_t cp_लंबाई=spanOneBack(spanSet, s, pos);
        अगर(cp_लंबाई>0) {
            वापस pos;  // There is a set element at pos.
        }

        // Try to match the strings at pos.
        क्रम(i=0; i<strings_लंबाई; ++i) {
            // Use spanLengths rather than a spanBackLengths pointer because
            // it is easier and we only need to know whether the string is irrelevant
            // which is the same in either array.
            अगर(spanLengths[i]==ALL_CP_CONTAINED) {
                जारी;  // Irrelevant string. (Also the empty string.)
            }
            स्थिर UnicodeString& string = *static_cast<स्थिर UnicodeString*>(strings.elementAt(i));
            स्थिर char16_t *s16=string.get_बफर();
            int32_t length16=string.length();
            U_ASSERT(length>0);
            अगर(length16<=pos && matches16CPB(s, pos-length16, length, s16, length16)) {
                वापस pos;  // There is a set element at pos.
            }
        }

        // The span(while not contained) ended on a string start/end which is
        // not in the original set. Skip this code point and continue.
        // cpLength<0
        pos+=cp_लंबाई;
    } जबतक(pos!=0);
    वापस 0;  // Reached the start of the string.
}

int32_t UnicodeSetStringSpan::spanNotUTF8(स्थिर uint8_t *s, int32_t length) स्थिर {
    int32_t pos=0, rest=length;
    int32_t i, strings_लंबाई=strings.size();
    uint8_t *spanUTF8Lengths=spanLengths;
    अगर(all) {
        spanUTF8Lengths+=2*strings_लंबाई;
    }
    करो {
        // Span until we find a code point from the set,
        // or a code point that starts or ends some string.
        i = pSpanNotSet->spanUTF8(reinterpret_cast<स्थिर अक्षर*>(s) + pos, rest, USET_SPAN_NOT_CONTAINED);
        अगर(i==rest) {
            वापस length;  // Reached the end of the string.
        }
        pos+=i;
        rest-=i;

        // Check whether the current code point is in the original set,
        // without the string starts and ends.
        int32_t cp_लंबाई=spanOneUTF8(spanSet, s+pos, rest);
        अगर(cp_लंबाई>0) {
            वापस pos;  // There is a set element at pos.
        }

        // Try to match the strings at pos.
        स्थिर uint8_t *s8=utf8;
        int32_t length8;
        क्रम(i=0; i<strings_लंबाई; ++i) {
            length8=utf8Lengths[i];
            // ALL_CP_CONTAINED: Irrelevant string.
            अगर(length8!=0 && spanUTF8Lengths[i]!=ALL_CP_CONTAINED && length8<=rest && matches8(s+pos, s8, length8)) {
                वापस pos;  // There is a set element at pos.
            }
            s8+=length8;
        }

        // The span(while not contained) ended on a string start/end which is
        // not in the original set. Skip this code point and continue.
        // cpLength<0
        pos-=cp_लंबाई;
        rest+=cp_लंबाई;
    } जबतक(rest!=0);
    वापस length;  // Reached the end of the string.
}

int32_t UnicodeSetStringSpan::spanNotBackUTF8(स्थिर uint8_t *s, int32_t length) स्थिर {
    int32_t pos=length;
    int32_t i, strings_लंबाई=strings.size();
    uint8_t *spanBackUTF8Lengths=spanLengths;
    अगर(all) {
        spanBackUTF8Lengths+=3*strings_लंबाई;
    }
    करो {
        // Span until we find a code point from the set,
        // or a code point that starts or ends some string.
        pos = pSpanNotSet->spanBackUTF8(reinterpret_cast<स्थिर अक्षर*>(s), pos, USET_SPAN_NOT_CONTAINED);
        अगर(pos==0) {
            वापस 0;  // Reached the start of the string.
        }

        // Check whether the current code point is in the original set,
        // without the string starts and ends.
        int32_t cp_लंबाई=spanOneBackUTF8(spanSet, s, pos);
        अगर(cp_लंबाई>0) {
            वापस pos;  // There is a set element at pos.
        }

        // Try to match the strings at pos.
        स्थिर uint8_t *s8=utf8;
        int32_t length8;
        क्रम(i=0; i<strings_लंबाई; ++i) {
            length8=utf8Lengths[i];
            // ALL_CP_CONTAINED: Irrelevant string.
            अगर(length8!=0 && spanBackUTF8Lengths[i]!=ALL_CP_CONTAINED && length8<=pos && matches8(s+pos-length8, s8, length8)) {
                वापस pos;  // There is a set element at pos.
            }
            s8+=length8;
        }

        // The span(while not contained) ended on a string start/end which is
        // not in the original set. Skip this code point and continue.
        // cpLength<0
        pos+=cp_लंबाई;
    } जबतक(pos!=0);
    वापस 0;  // Reached the start of the string.
}

U_NAMESPACE_END
