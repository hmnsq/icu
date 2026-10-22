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

#अन्तर्भाव "unicode/utypes.h"
#अन्तर्भाव "unicode/uniset.h"
#अन्तर्भाव "unicode/ustring.h"
#अन्तर्भाव "unicode/utf8.h"
#अन्तर्भाव "unicode/utf16.h"
#अन्तर्भाव "cmemory.h"
#अन्तर्भाव "uvector.h"
#अन्तर्भाव "unisetspan.h"

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
वर्ग OffsetList {  // Only ever stack-allocated, does not need to inherit UMemory.
सार्वजनिक:
    OffsetList() : list(staticList), capacity(0), length(0), start(0) {}

    ~OffsetList() {
        जर(list!=staticList) {
            uprv_free(list);
        }
    }

    // Call exactly once if the list is to be used.
    शून्य setMaxLength(int32_t maxLength) {
        जर (maxLength <= static_cast<int32_t>(आकार(staticList))) {
            capacity = static_cast<int32_t>(आकार(staticList));
        } अन्यथा {
            UBool* l = static_cast<UBool*>(uprv_malloc(maxLength));
            जर(l!=nullptr) {
                list=l;
                capacity=maxLength;
            }
        }
        uprv_memset(list, 0, capacity);
    }

    शून्य clear() {
        uprv_memset(list, 0, capacity);
        start=length=0;
    }

    UBool isEmpty() स्थिरांक {
        परत length == 0;
    }

    // Reduce all stored offsets by delta, used when the current position
    // moves by delta.
    // There must not be any offsets lower than delta.
    // If there is an offset equal to delta, it is removed.
    // delta=[1..maxLength]
    शून्य shift(int32_t delta) {
        int32_t i=start+delta;
        जर(i>=capacity) {
            i-=capacity;
        }
        जर(list[i]) {
            list[i]=असत्य;
            --length;
        }
        start=i;
    }

    // Add an offset. The list must not contain it yet.
    // offset=[1..maxLength]
    शून्य addOffset(int32_t offset) {
        int32_t i=start+offset;
        जर(i>=capacity) {
            i-=capacity;
        }
        list[i]=सत्य;
        ++length;
    }

    // offset=[1..maxLength]
    UBool containsOffset(int32_t offset) स्थिरांक {
        int32_t i=start+offset;
        जर(i>=capacity) {
            i-=capacity;
        }
        परत list[i];
    }

    // Find the lowest stored offset from a non-empty list, remove it,
    // and reduce all other offsets by this minimum.
    // Returns [1..maxLength].
    int32_t काढा_Minimum() {
        // Look for the next offset in list[start+1..capacity-1].
        int32_t i=start, result;
        पर्यंत(++i<capacity) {
            जर(list[i]) {
                list[i]=असत्य;
                --length;
                result=i-start;
                start=i;
                परत result;
            }
        }
        // i==capacity

        // Wrap around and look for the next offset in list[0..start].
        // Since the list is not empty, there will be one.
        result=capacity-start;
        i=0;
        पर्यंत(!list[i]) {
            ++i;
        }
        list[i]=असत्य;
        --length;
        start=i;
        परत result+=i;
    }

खाजगी:
    UBool *list;
    int32_t capacity;
    int32_t length;
    int32_t start;

    UBool staticList[16];
};

// Get the number of UTF-8 bytes for a UTF-16 (sub)string.
स्थिर int32_t
getUTF8Length(स्थिरांक char16_t *s, int32_t length) {
    UErrorCode errorCode=U_ZERO_ERROR;
    int32_t length8=0;
    u_strToUTF8(nullptr, 0, &length8, s, length, &errorCode);
    जर(U_SUCCESS(errorCode) || errorCode==U_BUFFER_OVERFLOW_ERROR) {
        परत length8;
    } अन्यथा {
        // The string contains an unpaired surrogate.
        // Ignore this string.
        परत 0;
    }
}

// Append the UTF-8 version of the string to t and return the appended UTF-8 length.
स्थिर int32_t
appendUTF8(स्थिरांक char16_t *s, int32_t length, uint8_t *t, int32_t capacity) {
    UErrorCode errorCode=U_ZERO_ERROR;
    int32_t length8=0;
    u_strToUTF8(reinterpret_cast<अक्षर*>(t), capacity, &length8, s, length, &errorCode);
    जर(U_SUCCESS(errorCode)) {
        परत length8;
    } अन्यथा {
        // The string contains an unpaired surrogate.
        // Ignore this string.
        परत 0;
    }
}

स्थिर ओळीत uint8_t
makeSpanLengthByte(int32_t spanLength) {
    // 0xfe==UnicodeSetStringSpan::LONG_SPAN
    परत spanLength < 0xfe ? static_cast<uint8_t>(spanLength) : static_cast<uint8_t>(0xfe);
}

// Construct for all variants of span(), or only for any one variant.
// Initialize as little as possible, for single use.
UnicodeSetStringSpan::UnicodeSetStringSpan(स्थिरांक UnicodeSet &set,
                                           स्थिरांक UVector &setStrings,
                                           uint32_t which)
        : spanSet(0, 0x10ffff), pSpanNotSet(nullptr), strings(setStrings),
          utf8Lengths(nullptr), spanLengths(nullptr), utf8(nullptr),
          utf8Length(0),
          maxLength16(0), maxLength8(0),
          all(static_cast<UBool>(which == ALL)) {
    spanSet.retainAll(set);
    जर(which&NOT_CONTAINED) {
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
    UBool someRelevant=असत्य;
    साठी(i=0; i<stringsLength; ++i) {
        स्थिरांक UnicodeString& string = *static_cast<स्थिरांक UnicodeString*>(strings.elementAt(i));
        स्थिरांक char16_t *s16=string.getBuffer();
        int32_t length16=string.length();
        जर (length16==0) {
            सुरूठेवा;  // skip the empty string
        }
        UBool thisRelevant;
        spanLength=spanSet.span(s16, length16, USET_SPAN_CONTAINED);
        जर(spanLength<length16) {  // Relevant string.
            someRelevant=thisRelevant=सत्य;
        } अन्यथा {
            thisRelevant=असत्य;
        }
        जर((which&UTF16) && length16>maxLength16) {
            maxLength16=length16;
        }
        जर((which&UTF8) && (thisRelevant || (which&CONTAINED))) {
            int32_t length8=getUTF8Length(s16, length16);
            utf8Length+=length8;
            जर(length8>maxLength8) {
                maxLength8=length8;
            }
        }
    }
    जर(!someRelevant) {
        maxLength16=maxLength8=0;
        परत;
    }

    // Freeze after checking for the need to use strings at all because freezing
    // a set takes some time and memory which are wasted if there are no relevant strings.
    जर(all) {
        spanSet.freeze();
    }

    uint8_t *spanBackLengths;
    uint8_t *spanUTF8Lengths;
    uint8_t *spanBackUTF8Lengths;

    // Allocate a block of meta data.
    int32_t allocSize;
    जर(all) {
        // UTF-8 lengths, 4 sets of span lengths, UTF-8 strings.
        allocSize=stringsLength*(4+1+1+1+1)+utf8Length;
    } अन्यथा {
        allocSize=stringsLength;  // One set of span lengths.
        जर(which&UTF8) {
            // UTF-8 lengths and UTF-8 strings.
            allocSize+=stringsLength*4+utf8Length;
        }
    }
    जर (allocSize <= static_cast<int32_t>(आकार(staticLengths))) {
        utf8Lengths=staticLengths;
    } अन्यथा {
        utf8Lengths = static_cast<int32_t*>(uprv_malloc(allocSize));
        जर(utf8Lengths==nullptr) {
            maxLength16=maxLength8=0;  // Prevent usage by making needsStringSpanUTF16/8() return false.
            परत;  // Out of memory.
        }
    }

    जर(all) {
        // Store span lengths for all span() variants.
        spanLengths = reinterpret_cast<uint8_t*>(utf8Lengths + stringsLength);
        spanBackLengths=spanLengths+stringsLength;
        spanUTF8Lengths=spanBackLengths+stringsLength;
        spanBackUTF8Lengths=spanUTF8Lengths+stringsLength;
        utf8=spanBackUTF8Lengths+stringsLength;
    } अन्यथा {
        // Store span lengths for only one span() variant.
        जर(which&UTF8) {
            spanLengths = reinterpret_cast<uint8_t*>(utf8Lengths + stringsLength);
            utf8=spanLengths+stringsLength;
        } अन्यथा {
            spanLengths = reinterpret_cast<uint8_t*>(utf8Lengths);
        }
        spanBackLengths=spanUTF8Lengths=spanBackUTF8Lengths=spanLengths;
    }

    // Set the meta data and pSpanNotSet and write the UTF-8 strings.
    int32_t utf8_मोजणी=0;  // Count UTF-8 bytes written so far.

    साठी(i=0; i<stringsLength; ++i) {
        स्थिरांक UnicodeString& string = *static_cast<स्थिरांक UnicodeString*>(strings.elementAt(i));
        स्थिरांक char16_t *s16=string.getBuffer();
        int32_t length16=string.length();
        spanLength=spanSet.span(s16, length16, USET_SPAN_CONTAINED);
        जर(spanLength<length16 && length16>0) {  // Relevant string.
            जर(which&UTF16) {
                जर(which&CONTAINED) {
                    जर(which&FWD) {
                        spanLengths[i]=makeSpanLengthByte(spanLength);
                    }
                    जर(which&BACK) {
                        spanLength=length16-spanSet.spanBack(s16, length16, USET_SPAN_CONTAINED);
                        spanBackLengths[i]=makeSpanLengthByte(spanLength);
                    }
                } अन्यथा /* not CONTAINED, not all, but NOT_CONTAINED */ {
                    spanLengths[i]=spanBackLengths[i]=0;  // Only store a relevant/irrelevant flag.
                }
            }
            जर(which&UTF8) {
                uint8_t *s8=utf8+utf8_मोजणी;
                int32_t length8=appendUTF8(s16, length16, s8, utf8Length-utf8_मोजणी);
                utf8_मोजणी+=utf8Lengths[i]=length8;
                जर(length8==0) {  // Irrelevant for UTF-8 because not representable in UTF-8.
                    spanUTF8Lengths[i] = spanBackUTF8Lengths[i] = static_cast<uint8_t>(ALL_CP_CONTAINED);
                } अन्यथा {  // Relevant for UTF-8.
                    जर(which&CONTAINED) {
                        जर(which&FWD) {
                            spanLength = spanSet.spanUTF8(reinterpret_cast<स्थिरांक अक्षर*>(s8), length8, USET_SPAN_CONTAINED);
                            spanUTF8Lengths[i]=makeSpanLengthByte(spanLength);
                        }
                        जर(which&BACK) {
                            spanLength = length8 - spanSet.spanBackUTF8(reinterpret_cast<स्थिरांक अक्षर*>(s8), length8, USET_SPAN_CONTAINED);
                            spanBackUTF8Lengths[i]=makeSpanLengthByte(spanLength);
                        }
                    } अन्यथा /* not CONTAINED, not all, but NOT_CONTAINED */ {
                        spanUTF8Lengths[i]=spanBackUTF8Lengths[i]=0;  // Only store a relevant/irrelevant flag.
                    }
                }
            }
            जर(which&NOT_CONTAINED) {
                // Add string start and end code points to the spanNotSet so that
                // a span(while not contained) stops before any string.
                UChar32 c;
                जर(which&FWD) {
                    int32_t len=0;
                    U16_NEXT(s16, len, length16, c);
                    addToSpanNotSet(c);
                }
                जर(which&BACK) {
                    int32_t len=length16;
                    U16_PREV(s16, 0, len, c);
                    addToSpanNotSet(c);
                }
            }
        } अन्यथा {  // Irrelevant string. (Also the empty string.)
            जर(which&UTF8) {
                जर(which&CONTAINED) {  // Only necessary for LONGEST_MATCH.
                    uint8_t *s8=utf8+utf8_मोजणी;
                    int32_t length8=appendUTF8(s16, length16, s8, utf8Length-utf8_मोजणी);
                    utf8_मोजणी+=utf8Lengths[i]=length8;
                } अन्यथा {
                    utf8Lengths[i]=0;
                }
            }
            जर(all) {
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
    जर(all) {
        pSpanNotSet->freeze();
    }
}

// Copy constructor. Assumes which==ALL for a frozen set.
UnicodeSetStringSpan::UnicodeSetStringSpan(स्थिरांक UnicodeSetStringSpan &otherStringSpan,
                                           स्थिरांक UVector &newParentSetStrings)
        : spanSet(otherStringSpan.spanSet), pSpanNotSet(nullptr), strings(newParentSetStrings),
          utf8Lengths(nullptr), spanLengths(nullptr), utf8(nullptr),
          utf8Length(otherStringSpan.utf8Length),
          maxLength16(otherStringSpan.maxLength16), maxLength8(otherStringSpan.maxLength8),
          all(सत्य) {
    जर(otherStringSpan.pSpanNotSet==&otherStringSpan.spanSet) {
        pSpanNotSet=&spanSet;
    } अन्यथा {
        pSpanNotSet=otherStringSpan.pSpanNotSet->clone();
    }

    // Allocate a block of meta data.
    // UTF-8 lengths, 4 sets of span lengths, UTF-8 strings.
    int32_t stringsLength=strings.size();
    int32_t allocSize=stringsLength*(4+1+1+1+1)+utf8Length;
    जर (allocSize <= static_cast<int32_t>(आकार(staticLengths))) {
        utf8Lengths=staticLengths;
    } अन्यथा {
        utf8Lengths = static_cast<int32_t*>(uprv_malloc(allocSize));
        जर(utf8Lengths==nullptr) {
            maxLength16=maxLength8=0;  // Prevent usage by making needsStringSpanUTF16/8() return false.
            परत;  // Out of memory.
        }
    }

    spanLengths = reinterpret_cast<uint8_t*>(utf8Lengths + stringsLength);
    utf8=spanLengths+stringsLength*4;
    uprv_memcpy(utf8Lengths, otherStringSpan.utf8Lengths, allocSize);
}

UnicodeSetStringSpan::~UnicodeSetStringSpan() {
    जर(pSpanNotSet!=nullptr && pSpanNotSet!=&spanSet) {
        delete pSpanNotSet;
    }
    जर(utf8Lengths!=nullptr && utf8Lengths!=staticLengths) {
        uprv_free(utf8Lengths);
    }
}

शून्य UnicodeSetStringSpan::addToSpanNotSet(UChar32 c) {
    जर(pSpanNotSet==nullptr || pSpanNotSet==&spanSet) {
        जर(spanSet.contains(c)) {
            परत;  // Nothing to do.
        }
        UnicodeSet *newSet=spanSet.cloneAsThawed();
        जर(newSet==nullptr) {
            परत;  // Out of memory.
        } अन्यथा {
            pSpanNotSet=newSet;
        }
    }
    pSpanNotSet->add(c);
}

// Compare strings without any argument checks. Requires length>0.
स्थिर ओळीत UBool
matches16(स्थिरांक char16_t *s, स्थिरांक char16_t *t, int32_t length) {
    करा {
        जर(*s++!=*t++) {
            परत असत्य;
        }
    } पर्यंत(--length>0);
    परत सत्य;
}

स्थिर ओळीत UBool
matches8(स्थिरांक uint8_t *s, स्थिरांक uint8_t *t, int32_t length) {
    करा {
        जर(*s++!=*t++) {
            परत असत्य;
        }
    } पर्यंत(--length>0);
    परत सत्य;
}

// Compare 16-bit Unicode strings (which may be malformed UTF-16)
// at code point boundaries.
// That is, each edge of a match must not be in the middle of a surrogate pair.
स्थिर ओळीत UBool
matches16CPB(स्थिरांक char16_t *s, int32_t start, int32_t limit, स्थिरांक char16_t *t, int32_t length) {
    s+=start;
    limit-=start;
    परत matches16(s, t, length) &&
           !(0<start && U16_IS_LEAD(s[-1]) && U16_IS_TRAIL(s[0])) &&
           !(length<limit && U16_IS_LEAD(s[length-1]) && U16_IS_TRAIL(s[length]));
}

// Does the set contain the next code point?
// If so, return its length; otherwise return its negative length.
स्थिर ओळीत int32_t
spanOne(स्थिरांक UnicodeSet &set, स्थिरांक char16_t *s, int32_t length) {
    char16_t c=*s, c2;
    जर(c>=0xd800 && c<=0xdbff && length>=2 && U16_IS_TRAIL(c2=s[1])) {
        परत set.contains(U16_GET_SUPPLEMENTARY(c, c2)) ? 2 : -2;
    }
    परत set.contains(c) ? 1 : -1;
}

स्थिर ओळीत int32_t
spanOneBack(स्थिरांक UnicodeSet &set, स्थिरांक char16_t *s, int32_t length) {
    char16_t c=s[length-1], c2;
    जर(c>=0xdc00 && c<=0xdfff && length>=2 && U16_IS_LEAD(c2=s[length-2])) {
        परत set.contains(U16_GET_SUPPLEMENTARY(c2, c)) ? 2 : -2;
    }
    परत set.contains(c) ? 1 : -1;
}

स्थिर ओळीत int32_t
spanOneUTF8(स्थिरांक UnicodeSet &set, स्थिरांक uint8_t *s, int32_t length) {
    UChar32 c=*s;
    जर(U8_IS_SINGLE(c)) {
        परत set.contains(c) ? 1 : -1;
    }
    // Take advantage of non-ASCII fastpaths in U8_NEXT_OR_FFFD().
    int32_t i=0;
    U8_NEXT_OR_FFFD(s, i, length, c);
    परत set.contains(c) ? i : -i;
}

स्थिर ओळीत int32_t
spanOneBackUTF8(स्थिरांक UnicodeSet &set, स्थिरांक uint8_t *s, int32_t length) {
    UChar32 c=s[length-1];
    जर(U8_IS_SINGLE(c)) {
        परत set.contains(c) ? 1 : -1;
    }
    int32_t i=length-1;
    c=utf8_prevCharSafeBody(s, 0, &i, c, -3);
    length-=i;
    परत set.contains(c) ? length : -length;
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

int32_t UnicodeSetStringSpan::span(स्थिरांक char16_t *s, int32_t length, USetSpanCondition spanCondition) स्थिरांक {
    जर(spanCondition==USET_SPAN_NOT_CONTAINED) {
        परत spanNot(s, length);
    }
    int32_t spanLength=spanSet.span(s, length, USET_SPAN_CONTAINED);
    जर(spanLength==length) {
        परत length;
    }

    // Consider strings; they may overlap with the span.
    OffsetList offsets;
    जर(spanCondition==USET_SPAN_CONTAINED) {
        // Use offset list to try all possibilities.
        offsets.setMaxLength(maxLength16);
    }
    int32_t pos=spanLength, rest=length-pos;
    int32_t i, stringsLength=strings.size();
    साठी(;;) {
        जर(spanCondition==USET_SPAN_CONTAINED) {
            साठी(i=0; i<stringsLength; ++i) {
                int32_t overlap=spanLengths[i];
                जर(overlap==ALL_CP_CONTAINED) {
                    सुरूठेवा;  // Irrelevant string. (Also the empty string.)
                }
                स्थिरांक UnicodeString& string = *static_cast<स्थिरांक UnicodeString*>(strings.elementAt(i));
                स्थिरांक char16_t *s16=string.getBuffer();
                int32_t length16=string.length();
                U_ASSERT(length>0);

                // Try to match this string at pos-overlap..pos.
                जर(overlap>=LONG_SPAN) {
                    overlap=length16;
                    // While contained: No point matching fully inside the code point span.
                    U16_BACK_1(s16, 0, overlap);  // Length of the string minus the last code point.
                }
                जर(overlap>spanLength) {
                    overlap=spanLength;
                }
                int32_t inc=length16-overlap;  // Keep overlap+inc==length16.
                साठी(;;) {
                    जर(inc>rest) {
                        थांबा;
                    }
                    // Try to match if the increment is not listed already.
                    जर(!offsets.containsOffset(inc) && matches16CPB(s, pos-overlap, length, s16, length16)) {
                        जर(inc==rest) {
                            परत length;  // Reached the end of the string.
                        }
                        offsets.addOffset(inc);
                    }
                    जर(overlap==0) {
                        थांबा;
                    }
                    --overlap;
                    ++inc;
                }
            }
        } अन्यथा /* USET_SPAN_SIMPLE */ {
            int32_t maxInc=0, maxOverlap=0;
            साठी(i=0; i<stringsLength; ++i) {
                int32_t overlap=spanLengths[i];
                // For longest match, we do need to try to match even an all-contained string
                // to find the match from the earliest start.

                स्थिरांक UnicodeString& string = *static_cast<स्थिरांक UnicodeString*>(strings.elementAt(i));
                स्थिरांक char16_t *s16=string.getBuffer();
                int32_t length16=string.length();
                जर (length16==0) {
                    सुरूठेवा;  // skip the empty string
                }

                // Try to match this string at pos-overlap..pos.
                जर(overlap>=LONG_SPAN) {
                    overlap=length16;
                    // Longest match: Need to match fully inside the code point span
                    // to find the match from the earliest start.
                }
                जर(overlap>spanLength) {
                    overlap=spanLength;
                }
                int32_t inc=length16-overlap;  // Keep overlap+inc==length16.
                साठी(;;) {
                    जर(inc>rest || overlap<maxOverlap) {
                        थांबा;
                    }
                    // Try to match if the string is longer or starts earlier.
                    जर( (overlap>maxOverlap || /* redundant overlap==maxOverlap && */ inc>maxInc) &&
                        matches16CPB(s, pos-overlap, length, s16, length16)
                    ) {
                        maxInc=inc;  // Longest match from earliest start.
                        maxOverlap=overlap;
                        थांबा;
                    }
                    --overlap;
                    ++inc;
                }
            }

            जर(maxInc!=0 || maxOverlap!=0) {
                // Longest-match algorithm, and there was a string match.
                // Simply continue after it.
                pos+=maxInc;
                rest-=maxInc;
                जर(rest==0) {
                    परत length;  // Reached the end of the string.
                }
                spanLength=0;  // Match strings from after a string match.
                सुरूठेवा;
            }
        }
        // Finished trying to match all strings at pos.

        जर(spanLength!=0 || pos==0) {
            // The position is after an unlimited code point span (spanLength!=0),
            // not after a string match.
            // The only position where spanLength==0 after a span is pos==0.
            // Otherwise, an unlimited code point span is only tried again when no
            // strings match, and if such a non-initial span fails we stop.
            जर(offsets.isEmpty()) {
                परत pos;  // No strings matched after a span.
            }
            // Match strings from after the next string match.
        } अन्यथा {
            // The position is after a string match (or a single code point).
            जर(offsets.isEmpty()) {
                // No more strings matched after a previous string match.
                // Try another code point span from after the last string match.
                spanLength=spanSet.span(s+pos, rest, USET_SPAN_CONTAINED);
                जर( spanLength==rest || // Reached the end of the string, or
                    spanLength==0       // neither strings nor span progressed.
                ) {
                    परत pos+spanLength;
                }
                pos+=spanLength;
                rest-=spanLength;
                सुरूठेवा;  // spanLength>0: Match strings from after a span.
            } अन्यथा {
                // Try to match only one code point from after a string match if some
                // string matched beyond it, so that we try all possible positions
                // and don't overshoot.
                spanLength=spanOne(spanSet, s+pos, rest);
                जर(spanLength>0) {
                    जर(spanLength==rest) {
                        परत length;  // Reached the end of the string.
                    }
                    // Match strings after this code point.
                    // There cannot be any increments below it because UnicodeSet strings
                    // contain multiple code points.
                    pos+=spanLength;
                    rest-=spanLength;
                    offsets.shift(spanLength);
                    spanLength=0;
                    सुरूठेवा;  // Match strings from after a single code point.
                }
                // Match strings from after the next string match.
            }
        }
        int32_t minOffset=offsets.काढा_Minimum();
        pos+=minOffset;
        rest-=minOffset;
        spanLength=0;  // Match strings from after a string match.
    }
}

int32_t UnicodeSetStringSpan::spanBack(स्थिरांक char16_t *s, int32_t length, USetSpanCondition spanCondition) स्थिरांक {
    जर(spanCondition==USET_SPAN_NOT_CONTAINED) {
        परत spanNotBack(s, length);
    }
    int32_t pos=spanSet.spanBack(s, length, USET_SPAN_CONTAINED);
    जर(pos==0) {
        परत 0;
    }
    int32_t spanLength=length-pos;

    // Consider strings; they may overlap with the span.
    OffsetList offsets;
    जर(spanCondition==USET_SPAN_CONTAINED) {
        // Use offset list to try all possibilities.
        offsets.setMaxLength(maxLength16);
    }
    int32_t i, stringsLength=strings.size();
    uint8_t *spanBackLengths=spanLengths;
    जर(all) {
        spanBackLengths+=stringsLength;
    }
    साठी(;;) {
        जर(spanCondition==USET_SPAN_CONTAINED) {
            साठी(i=0; i<stringsLength; ++i) {
                int32_t overlap=spanBackLengths[i];
                जर(overlap==ALL_CP_CONTAINED) {
                    सुरूठेवा;  // Irrelevant string. (Also the empty string.)
                }
                स्थिरांक UnicodeString& string = *static_cast<स्थिरांक UnicodeString*>(strings.elementAt(i));
                स्थिरांक char16_t *s16=string.getBuffer();
                int32_t length16=string.length();
                U_ASSERT(length>0);

                // Try to match this string at pos-(length16-overlap)..pos-length16.
                जर(overlap>=LONG_SPAN) {
                    overlap=length16;
                    // While contained: No point matching fully inside the code point span.
                    int32_t len1=0;
                    U16_FWD_1(s16, len1, overlap);
                    overlap-=len1;  // Length of the string minus the first code point.
                }
                जर(overlap>spanLength) {
                    overlap=spanLength;
                }
                int32_t dec=length16-overlap;  // Keep dec+overlap==length16.
                साठी(;;) {
                    जर(dec>pos) {
                        थांबा;
                    }
                    // Try to match if the decrement is not listed already.
                    जर(!offsets.containsOffset(dec) && matches16CPB(s, pos-dec, length, s16, length16)) {
                        जर(dec==pos) {
                            परत 0;  // Reached the start of the string.
                        }
                        offsets.addOffset(dec);
                    }
                    जर(overlap==0) {
                        थांबा;
                    }
                    --overlap;
                    ++dec;
                }
            }
        } अन्यथा /* USET_SPAN_SIMPLE */ {
            int32_t maxDec=0, maxOverlap=0;
            साठी(i=0; i<stringsLength; ++i) {
                int32_t overlap=spanBackLengths[i];
                // For longest match, we do need to try to match even an all-contained string
                // to find the match from the latest end.

                स्थिरांक UnicodeString& string = *static_cast<स्थिरांक UnicodeString*>(strings.elementAt(i));
                स्थिरांक char16_t *s16=string.getBuffer();
                int32_t length16=string.length();
                जर (length16==0) {
                    सुरूठेवा;  // skip the empty string
                }

                // Try to match this string at pos-(length16-overlap)..pos-length16.
                जर(overlap>=LONG_SPAN) {
                    overlap=length16;
                    // Longest match: Need to match fully inside the code point span
                    // to find the match from the latest end.
                }
                जर(overlap>spanLength) {
                    overlap=spanLength;
                }
                int32_t dec=length16-overlap;  // Keep dec+overlap==length16.
                साठी(;;) {
                    जर(dec>pos || overlap<maxOverlap) {
                        थांबा;
                    }
                    // Try to match if the string is longer or ends later.
                    जर( (overlap>maxOverlap || /* redundant overlap==maxOverlap && */ dec>maxDec) &&
                        matches16CPB(s, pos-dec, length, s16, length16)
                    ) {
                        maxDec=dec;  // Longest match from latest end.
                        maxOverlap=overlap;
                        थांबा;
                    }
                    --overlap;
                    ++dec;
                }
            }

            जर(maxDec!=0 || maxOverlap!=0) {
                // Longest-match algorithm, and there was a string match.
                // Simply continue before it.
                pos-=maxDec;
                जर(pos==0) {
                    परत 0;  // Reached the start of the string.
                }
                spanLength=0;  // Match strings from before a string match.
                सुरूठेवा;
            }
        }
        // Finished trying to match all strings at pos.

        जर(spanLength!=0 || pos==length) {
            // The position is before an unlimited code point span (spanLength!=0),
            // not before a string match.
            // The only position where spanLength==0 before a span is pos==length.
            // Otherwise, an unlimited code point span is only tried again when no
            // strings match, and if such a non-initial span fails we stop.
            जर(offsets.isEmpty()) {
                परत pos;  // No strings matched before a span.
            }
            // Match strings from before the next string match.
        } अन्यथा {
            // The position is before a string match (or a single code point).
            जर(offsets.isEmpty()) {
                // No more strings matched before a previous string match.
                // Try another code point span from before the last string match.
                int32_t oldPos=pos;
                pos=spanSet.spanBack(s, oldPos, USET_SPAN_CONTAINED);
                spanLength=oldPos-pos;
                जर( pos==0 ||           // Reached the start of the string, or
                    spanLength==0       // neither strings nor span progressed.
                ) {
                    परत pos;
                }
                सुरूठेवा;  // spanLength>0: Match strings from before a span.
            } अन्यथा {
                // Try to match only one code point from before a string match if some
                // string matched beyond it, so that we try all possible positions
                // and don't overshoot.
                spanLength=spanOneBack(spanSet, s, pos);
                जर(spanLength>0) {
                    जर(spanLength==pos) {
                        परत 0;  // Reached the start of the string.
                    }
                    // Match strings before this code point.
                    // There cannot be any decrements below it because UnicodeSet strings
                    // contain multiple code points.
                    pos-=spanLength;
                    offsets.shift(spanLength);
                    spanLength=0;
                    सुरूठेवा;  // Match strings from before a single code point.
                }
                // Match strings from before the next string match.
            }
        }
        pos-=offsets.काढा_Minimum();
        spanLength=0;  // Match strings from before a string match.
    }
}

int32_t UnicodeSetStringSpan::spanUTF8(स्थिरांक uint8_t *s, int32_t length, USetSpanCondition spanCondition) स्थिरांक {
    जर(spanCondition==USET_SPAN_NOT_CONTAINED) {
        परत spanNotUTF8(s, length);
    }
    int32_t spanLength = spanSet.spanUTF8(reinterpret_cast<स्थिरांक अक्षर*>(s), length, USET_SPAN_CONTAINED);
    जर(spanLength==length) {
        परत length;
    }

    // Consider strings; they may overlap with the span.
    OffsetList offsets;
    जर(spanCondition==USET_SPAN_CONTAINED) {
        // Use offset list to try all possibilities.
        offsets.setMaxLength(maxLength8);
    }
    int32_t pos=spanLength, rest=length-pos;
    int32_t i, stringsLength=strings.size();
    uint8_t *spanUTF8Lengths=spanLengths;
    जर(all) {
        spanUTF8Lengths+=2*stringsLength;
    }
    साठी(;;) {
        स्थिरांक uint8_t *s8=utf8;
        int32_t length8;
        जर(spanCondition==USET_SPAN_CONTAINED) {
            साठी(i=0; i<stringsLength; ++i) {
                length8=utf8Lengths[i];
                जर(length8==0) {
                    सुरूठेवा;  // String not representable in UTF-8.
                }
                int32_t overlap=spanUTF8Lengths[i];
                जर(overlap==ALL_CP_CONTAINED) {
                    s8+=length8;
                    सुरूठेवा;  // Irrelevant string.
                }

                // Try to match this string at pos-overlap..pos.
                जर(overlap>=LONG_SPAN) {
                    overlap=length8;
                    // While contained: No point matching fully inside the code point span.
                    U8_BACK_1(s8, 0, overlap);  // Length of the string minus the last code point.
                }
                जर(overlap>spanLength) {
                    overlap=spanLength;
                }
                int32_t inc=length8-overlap;  // Keep overlap+inc==length8.
                साठी(;;) {
                    जर(inc>rest) {
                        थांबा;
                    }
                    // Try to match if the increment is not listed already.
                    // Match at code point boundaries. (The UTF-8 strings were converted
                    // from UTF-16 and are guaranteed to be well-formed.)
                    जर(!U8_IS_TRAIL(s[pos-overlap]) &&
                            !offsets.containsOffset(inc) &&
                            matches8(s+pos-overlap, s8, length8)) {
                        जर(inc==rest) {
                            परत length;  // Reached the end of the string.
                        }
                        offsets.addOffset(inc);
                    }
                    जर(overlap==0) {
                        थांबा;
                    }
                    --overlap;
                    ++inc;
                }
                s8+=length8;
            }
        } अन्यथा /* USET_SPAN_SIMPLE */ {
            int32_t maxInc=0, maxOverlap=0;
            साठी(i=0; i<stringsLength; ++i) {
                length8=utf8Lengths[i];
                जर(length8==0) {
                    सुरूठेवा;  // String not representable in UTF-8.
                }
                int32_t overlap=spanUTF8Lengths[i];
                // For longest match, we do need to try to match even an all-contained string
                // to find the match from the earliest start.

                // Try to match this string at pos-overlap..pos.
                जर(overlap>=LONG_SPAN) {
                    overlap=length8;
                    // Longest match: Need to match fully inside the code point span
                    // to find the match from the earliest start.
                }
                जर(overlap>spanLength) {
                    overlap=spanLength;
                }
                int32_t inc=length8-overlap;  // Keep overlap+inc==length8.
                साठी(;;) {
                    जर(inc>rest || overlap<maxOverlap) {
                        थांबा;
                    }
                    // Try to match if the string is longer or starts earlier.
                    // Match at code point boundaries. (The UTF-8 strings were converted
                    // from UTF-16 and are guaranteed to be well-formed.)
                    जर(!U8_IS_TRAIL(s[pos-overlap]) &&
                            (overlap>maxOverlap ||
                                /* redundant overlap==maxOverlap && */ inc>maxInc) &&
                            matches8(s+pos-overlap, s8, length8)) {
                        maxInc=inc;  // Longest match from earliest start.
                        maxOverlap=overlap;
                        थांबा;
                    }
                    --overlap;
                    ++inc;
                }
                s8+=length8;
            }

            जर(maxInc!=0 || maxOverlap!=0) {
                // Longest-match algorithm, and there was a string match.
                // Simply continue after it.
                pos+=maxInc;
                rest-=maxInc;
                जर(rest==0) {
                    परत length;  // Reached the end of the string.
                }
                spanLength=0;  // Match strings from after a string match.
                सुरूठेवा;
            }
        }
        // Finished trying to match all strings at pos.

        जर(spanLength!=0 || pos==0) {
            // The position is after an unlimited code point span (spanLength!=0),
            // not after a string match.
            // The only position where spanLength==0 after a span is pos==0.
            // Otherwise, an unlimited code point span is only tried again when no
            // strings match, and if such a non-initial span fails we stop.
            जर(offsets.isEmpty()) {
                परत pos;  // No strings matched after a span.
            }
            // Match strings from after the next string match.
        } अन्यथा {
            // The position is after a string match (or a single code point).
            जर(offsets.isEmpty()) {
                // No more strings matched after a previous string match.
                // Try another code point span from after the last string match.
                spanLength = spanSet.spanUTF8(reinterpret_cast<स्थिरांक अक्षर*>(s) + pos, rest, USET_SPAN_CONTAINED);
                जर( spanLength==rest || // Reached the end of the string, or
                    spanLength==0       // neither strings nor span progressed.
                ) {
                    परत pos+spanLength;
                }
                pos+=spanLength;
                rest-=spanLength;
                सुरूठेवा;  // spanLength>0: Match strings from after a span.
            } अन्यथा {
                // Try to match only one code point from after a string match if some
                // string matched beyond it, so that we try all possible positions
                // and don't overshoot.
                spanLength=spanOneUTF8(spanSet, s+pos, rest);
                जर(spanLength>0) {
                    जर(spanLength==rest) {
                        परत length;  // Reached the end of the string.
                    }
                    // Match strings after this code point.
                    // There cannot be any increments below it because UnicodeSet strings
                    // contain multiple code points.
                    pos+=spanLength;
                    rest-=spanLength;
                    offsets.shift(spanLength);
                    spanLength=0;
                    सुरूठेवा;  // Match strings from after a single code point.
                }
                // Match strings from after the next string match.
            }
        }
        int32_t minOffset=offsets.काढा_Minimum();
        pos+=minOffset;
        rest-=minOffset;
        spanLength=0;  // Match strings from after a string match.
    }
}

int32_t UnicodeSetStringSpan::spanBackUTF8(स्थिरांक uint8_t *s, int32_t length, USetSpanCondition spanCondition) स्थिरांक {
    जर(spanCondition==USET_SPAN_NOT_CONTAINED) {
        परत spanNotBackUTF8(s, length);
    }
    int32_t pos = spanSet.spanBackUTF8(reinterpret_cast<स्थिरांक अक्षर*>(s), length, USET_SPAN_CONTAINED);
    जर(pos==0) {
        परत 0;
    }
    int32_t spanLength=length-pos;

    // Consider strings; they may overlap with the span.
    OffsetList offsets;
    जर(spanCondition==USET_SPAN_CONTAINED) {
        // Use offset list to try all possibilities.
        offsets.setMaxLength(maxLength8);
    }
    int32_t i, stringsLength=strings.size();
    uint8_t *spanBackUTF8Lengths=spanLengths;
    जर(all) {
        spanBackUTF8Lengths+=3*stringsLength;
    }
    साठी(;;) {
        स्थिरांक uint8_t *s8=utf8;
        int32_t length8;
        जर(spanCondition==USET_SPAN_CONTAINED) {
            साठी(i=0; i<stringsLength; ++i) {
                length8=utf8Lengths[i];
                जर(length8==0) {
                    सुरूठेवा;  // String not representable in UTF-8.
                }
                int32_t overlap=spanBackUTF8Lengths[i];
                जर(overlap==ALL_CP_CONTAINED) {
                    s8+=length8;
                    सुरूठेवा;  // Irrelevant string.
                }

                // Try to match this string at pos-(length8-overlap)..pos-length8.
                जर(overlap>=LONG_SPAN) {
                    overlap=length8;
                    // While contained: No point matching fully inside the code point span.
                    int32_t len1=0;
                    U8_FWD_1(s8, len1, overlap);
                    overlap-=len1;  // Length of the string minus the first code point.
                }
                जर(overlap>spanLength) {
                    overlap=spanLength;
                }
                int32_t dec=length8-overlap;  // Keep dec+overlap==length8.
                साठी(;;) {
                    जर(dec>pos) {
                        थांबा;
                    }
                    // Try to match if the decrement is not listed already.
                    // Match at code point boundaries. (The UTF-8 strings were converted
                    // from UTF-16 and are guaranteed to be well-formed.)
                    जर( !U8_IS_TRAIL(s[pos-dec]) &&
                        !offsets.containsOffset(dec) &&
                        matches8(s+pos-dec, s8, length8)
                    ) {
                        जर(dec==pos) {
                            परत 0;  // Reached the start of the string.
                        }
                        offsets.addOffset(dec);
                    }
                    जर(overlap==0) {
                        थांबा;
                    }
                    --overlap;
                    ++dec;
                }
                s8+=length8;
            }
        } अन्यथा /* USET_SPAN_SIMPLE */ {
            int32_t maxDec=0, maxOverlap=0;
            साठी(i=0; i<stringsLength; ++i) {
                length8=utf8Lengths[i];
                जर(length8==0) {
                    सुरूठेवा;  // String not representable in UTF-8.
                }
                int32_t overlap=spanBackUTF8Lengths[i];
                // For longest match, we do need to try to match even an all-contained string
                // to find the match from the latest end.

                // Try to match this string at pos-(length8-overlap)..pos-length8.
                जर(overlap>=LONG_SPAN) {
                    overlap=length8;
                    // Longest match: Need to match fully inside the code point span
                    // to find the match from the latest end.
                }
                जर(overlap>spanLength) {
                    overlap=spanLength;
                }
                int32_t dec=length8-overlap;  // Keep dec+overlap==length8.
                साठी(;;) {
                    जर(dec>pos || overlap<maxOverlap) {
                        थांबा;
                    }
                    // Try to match if the string is longer or ends later.
                    // Match at code point boundaries. (The UTF-8 strings were converted
                    // from UTF-16 and are guaranteed to be well-formed.)
                    जर( !U8_IS_TRAIL(s[pos-dec]) &&
                        (overlap>maxOverlap || /* redundant overlap==maxOverlap && */ dec>maxDec) &&
                        matches8(s+pos-dec, s8, length8)
                    ) {
                        maxDec=dec;  // Longest match from latest end.
                        maxOverlap=overlap;
                        थांबा;
                    }
                    --overlap;
                    ++dec;
                }
                s8+=length8;
            }

            जर(maxDec!=0 || maxOverlap!=0) {
                // Longest-match algorithm, and there was a string match.
                // Simply continue before it.
                pos-=maxDec;
                जर(pos==0) {
                    परत 0;  // Reached the start of the string.
                }
                spanLength=0;  // Match strings from before a string match.
                सुरूठेवा;
            }
        }
        // Finished trying to match all strings at pos.

        जर(spanLength!=0 || pos==length) {
            // The position is before an unlimited code point span (spanLength!=0),
            // not before a string match.
            // The only position where spanLength==0 before a span is pos==length.
            // Otherwise, an unlimited code point span is only tried again when no
            // strings match, and if such a non-initial span fails we stop.
            जर(offsets.isEmpty()) {
                परत pos;  // No strings matched before a span.
            }
            // Match strings from before the next string match.
        } अन्यथा {
            // The position is before a string match (or a single code point).
            जर(offsets.isEmpty()) {
                // No more strings matched before a previous string match.
                // Try another code point span from before the last string match.
                int32_t oldPos=pos;
                pos = spanSet.spanBackUTF8(reinterpret_cast<स्थिरांक अक्षर*>(s), oldPos, USET_SPAN_CONTAINED);
                spanLength=oldPos-pos;
                जर( pos==0 ||           // Reached the start of the string, or
                    spanLength==0       // neither strings nor span progressed.
                ) {
                    परत pos;
                }
                सुरूठेवा;  // spanLength>0: Match strings from before a span.
            } अन्यथा {
                // Try to match only one code point from before a string match if some
                // string matched beyond it, so that we try all possible positions
                // and don't overshoot.
                spanLength=spanOneBackUTF8(spanSet, s, pos);
                जर(spanLength>0) {
                    जर(spanLength==pos) {
                        परत 0;  // Reached the start of the string.
                    }
                    // Match strings before this code point.
                    // There cannot be any decrements below it because UnicodeSet strings
                    // contain multiple code points.
                    pos-=spanLength;
                    offsets.shift(spanLength);
                    spanLength=0;
                    सुरूठेवा;  // Match strings from before a single code point.
                }
                // Match strings from before the next string match.
            }
        }
        pos-=offsets.काढा_Minimum();
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

int32_t UnicodeSetStringSpan::spanNot(स्थिरांक char16_t *s, int32_t length) स्थिरांक {
    int32_t pos=0, rest=length;
    int32_t i, stringsLength=strings.size();
    करा {
        // Span until we find a code point from the set,
        // or a code point that starts or ends some string.
        i=pSpanNotSet->span(s+pos, rest, USET_SPAN_NOT_CONTAINED);
        जर(i==rest) {
            परत length;  // Reached the end of the string.
        }
        pos+=i;
        rest-=i;

        // Check whether the current code point is in the original set,
        // without the string starts and ends.
        int32_t cpLength=spanOne(spanSet, s+pos, rest);
        जर(cpLength>0) {
            परत pos;  // There is a set element at pos.
        }

        // Try to match the strings at pos.
        साठी(i=0; i<stringsLength; ++i) {
            जर(spanLengths[i]==ALL_CP_CONTAINED) {
                सुरूठेवा;  // Irrelevant string. (Also the empty string.)
            }
            स्थिरांक UnicodeString& string = *static_cast<स्थिरांक UnicodeString*>(strings.elementAt(i));
            स्थिरांक char16_t *s16=string.getBuffer();
            int32_t length16=string.length();
            U_ASSERT(length>0);
            जर(length16<=rest && matches16CPB(s, pos, length, s16, length16)) {
                परत pos;  // There is a set element at pos.
            }
        }

        // The span(while not contained) ended on a string start/end which is
        // not in the original set. Skip this code point and continue.
        // cpLength<0
        pos-=cpLength;
        rest+=cpLength;
    } पर्यंत(rest!=0);
    परत length;  // Reached the end of the string.
}

int32_t UnicodeSetStringSpan::spanNotBack(स्थिरांक char16_t *s, int32_t length) स्थिरांक {
    int32_t pos=length;
    int32_t i, stringsLength=strings.size();
    करा {
        // Span until we find a code point from the set,
        // or a code point that starts or ends some string.
        pos=pSpanNotSet->spanBack(s, pos, USET_SPAN_NOT_CONTAINED);
        जर(pos==0) {
            परत 0;  // Reached the start of the string.
        }

        // Check whether the current code point is in the original set,
        // without the string starts and ends.
        int32_t cpLength=spanOneBack(spanSet, s, pos);
        जर(cpLength>0) {
            परत pos;  // There is a set element at pos.
        }

        // Try to match the strings at pos.
        साठी(i=0; i<stringsLength; ++i) {
            // Use spanLengths rather than a spanBackLengths pointer because
            // it is easier and we only need to know whether the string is irrelevant
            // which is the same in either array.
            जर(spanLengths[i]==ALL_CP_CONTAINED) {
                सुरूठेवा;  // Irrelevant string. (Also the empty string.)
            }
            स्थिरांक UnicodeString& string = *static_cast<स्थिरांक UnicodeString*>(strings.elementAt(i));
            स्थिरांक char16_t *s16=string.getBuffer();
            int32_t length16=string.length();
            U_ASSERT(length>0);
            जर(length16<=pos && matches16CPB(s, pos-length16, length, s16, length16)) {
                परत pos;  // There is a set element at pos.
            }
        }

        // The span(while not contained) ended on a string start/end which is
        // not in the original set. Skip this code point and continue.
        // cpLength<0
        pos+=cpLength;
    } पर्यंत(pos!=0);
    परत 0;  // Reached the start of the string.
}

int32_t UnicodeSetStringSpan::spanNotUTF8(स्थिरांक uint8_t *s, int32_t length) स्थिरांक {
    int32_t pos=0, rest=length;
    int32_t i, stringsLength=strings.size();
    uint8_t *spanUTF8Lengths=spanLengths;
    जर(all) {
        spanUTF8Lengths+=2*stringsLength;
    }
    करा {
        // Span until we find a code point from the set,
        // or a code point that starts or ends some string.
        i = pSpanNotSet->spanUTF8(reinterpret_cast<स्थिरांक अक्षर*>(s) + pos, rest, USET_SPAN_NOT_CONTAINED);
        जर(i==rest) {
            परत length;  // Reached the end of the string.
        }
        pos+=i;
        rest-=i;

        // Check whether the current code point is in the original set,
        // without the string starts and ends.
        int32_t cpLength=spanOneUTF8(spanSet, s+pos, rest);
        जर(cpLength>0) {
            परत pos;  // There is a set element at pos.
        }

        // Try to match the strings at pos.
        स्थिरांक uint8_t *s8=utf8;
        int32_t length8;
        साठी(i=0; i<stringsLength; ++i) {
            length8=utf8Lengths[i];
            // ALL_CP_CONTAINED: Irrelevant string.
            जर(length8!=0 && spanUTF8Lengths[i]!=ALL_CP_CONTAINED && length8<=rest && matches8(s+pos, s8, length8)) {
                परत pos;  // There is a set element at pos.
            }
            s8+=length8;
        }

        // The span(while not contained) ended on a string start/end which is
        // not in the original set. Skip this code point and continue.
        // cpLength<0
        pos-=cpLength;
        rest+=cpLength;
    } पर्यंत(rest!=0);
    परत length;  // Reached the end of the string.
}

int32_t UnicodeSetStringSpan::spanNotBackUTF8(स्थिरांक uint8_t *s, int32_t length) स्थिरांक {
    int32_t pos=length;
    int32_t i, stringsLength=strings.size();
    uint8_t *spanBackUTF8Lengths=spanLengths;
    जर(all) {
        spanBackUTF8Lengths+=3*stringsLength;
    }
    करा {
        // Span until we find a code point from the set,
        // or a code point that starts or ends some string.
        pos = pSpanNotSet->spanBackUTF8(reinterpret_cast<स्थिरांक अक्षर*>(s), pos, USET_SPAN_NOT_CONTAINED);
        जर(pos==0) {
            परत 0;  // Reached the start of the string.
        }

        // Check whether the current code point is in the original set,
        // without the string starts and ends.
        int32_t cpLength=spanOneBackUTF8(spanSet, s, pos);
        जर(cpLength>0) {
            परत pos;  // There is a set element at pos.
        }

        // Try to match the strings at pos.
        स्थिरांक uint8_t *s8=utf8;
        int32_t length8;
        साठी(i=0; i<stringsLength; ++i) {
            length8=utf8Lengths[i];
            // ALL_CP_CONTAINED: Irrelevant string.
            जर(length8!=0 && spanBackUTF8Lengths[i]!=ALL_CP_CONTAINED && length8<=pos && matches8(s+pos-length8, s8, length8)) {
                परत pos;  // There is a set element at pos.
            }
            s8+=length8;
        }

        // The span(while not contained) ended on a string start/end which is
        // not in the original set. Skip this code point and continue.
        // cpLength<0
        pos+=cpLength;
    } पर्यंत(pos!=0);
    परत 0;  // Reached the start of the string.
}

U_NAMESPACE_END
