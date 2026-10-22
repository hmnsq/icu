// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
**********************************************************************
*   Copyright (C) 1999-2015, International Business Machines
*   Corporation and others.  All Rights Reserved.
**********************************************************************
*   Date        Name        Description
*   10/20/99    alan        Creation.
**********************************************************************
*/

#अन्तर्भाव "unicode/utypes.h"
#अन्तर्भाव "unicode/parsepos.h"
#अन्तर्भाव "unicode/symtable.h"
#अन्तर्भाव "unicode/uniset.h"
#अन्तर्भाव "unicode/ustring.h"
#अन्तर्भाव "unicode/utf8.h"
#अन्तर्भाव "unicode/utf16.h"
#अन्तर्भाव "ruleiter.h"
#अन्तर्भाव "cmemory.h"
#अन्तर्भाव "cstring.h"
#अन्तर्भाव "patternprops.h"
#अन्तर्भाव "uelement.h"
#अन्तर्भाव "util.h"
#अन्तर्भाव "uvector.h"
#अन्तर्भाव "charstr.h"
#अन्तर्भाव "ustrfmt.h"
#अन्तर्भाव "uassert.h"
#अन्तर्भाव "bmpset.h"
#अन्तर्भाव "unisetspan.h"

// HIGH_VALUE > all valid values. 110000 for codepoints
#define UNICODESET_उच्चः 0x0110000

// LOW <= all valid values. ZERO for codepoints
#define UNICODESET_नीचः 0x000000

/** Max list [0, 1, 2, ..., max code point, HIGH] */
constexpr int32_t MAX_LENGTH = UNICODESET_उच्चः + 1;

U_NAMESPACE_BEGIN

चिह्नम्_Table::~चिह्नम्_Table() {}

UOBJECT_DEFINE_RTTI_IMPLEMENTATION(UnicodeSet)

/**
 * Modify the given UChar32 variable so that it is in range, by
 * pinning values < UNICODESET_LOW to UNICODESET_LOW, and
 * pinning values > UNICODESET_HIGH-1 to UNICODESET_HIGH-1.
 * It modifies its argument in-place and also returns it.
 */
स्थिर अन्तःस्थ UChar32 pinCodePoint(UChar32& c) {
    सचे (c < UNICODESET_नीचः) {
        c = UNICODESET_नीचः;
    } अञ्ञथा सचे (c > (UNICODESET_उच्चः-1)) {
        c = (UNICODESET_उच्चः-1);
    }
    पटिनिवत्त c;
}

//----------------------------------------------------------------
// Debugging
//----------------------------------------------------------------

// DO NOT DELETE THIS CODE.  This code is used to debug memory leaks.
// To enable the debugging, define the symbol DEBUG_MEM in the line
// below.  This will result in text being sent to stdout that looks
// like this:
//   DEBUG UnicodeSet: ct 0x00A39B20; 397 [\u0A81-\u0A83\u0A85-
//   DEBUG UnicodeSet: dt 0x00A39B20; 396 [\u0A81-\u0A83\u0A85-
// Each line lists a construction (ct) or destruction (dt) event, the
// object address, the number of outstanding objects after the event,
// and the pattern of the object in question.

// #define DEBUG_MEM

#ifdef DEBUG_MEM
#अन्तर्भाव <stdio.h>
स्थिर int32_t dbg_गणनम् = 0;

स्थिर अन्तःस्थ शून्य _dbgct(UnicodeSet* set) {
    UnicodeString str;
    set->toPattern(str, सच्च);
    वर्ण buf[40];
    str.extract(0, 39, buf, "");
    printf("DEBUG UnicodeSet: ct 0x%08X; %d %s\n", set, ++dbg_गणनम्, buf);
}

स्थिर अन्तःस्थ शून्य _dbgdt(UnicodeSet* set) {
    UnicodeString str;
    set->toPattern(str, सच्च);
    वर्ण buf[40];
    str.extract(0, 39, buf, "");
    printf("DEBUG UnicodeSet: dt 0x%08X; %d %s\n", set, --dbg_गणनम्, buf);
}

#अञ्ञथा

#define _dbgct(set)
#define _dbgdt(set)

#endif

//----------------------------------------------------------------
// UnicodeString in UVector support
//----------------------------------------------------------------

स्थिर शून्य U_CALLCONV cloneUnicodeString(UElement *dst, UElement *src) {
    dst->pointer = नूतन UnicodeString(*static_cast<UnicodeString*>(src->pointer));
}

स्थिर int32_t U_CALLCONV compareUnicodeString(UElement t1, UElement t2) {
    ध्रुव UnicodeString& a = *static_cast<ध्रुव UnicodeString*>(t1.pointer);
    ध्रुव UnicodeString& b = *static_cast<ध्रुव UnicodeString*>(t2.pointer);
    पटिनिवत्त a.compare(b);
}

UBool UnicodeSet::hasStrings() ध्रुव {
    पटिनिवत्त strings_ != nullptr && !strings_->isEmpty();
}

int32_t UnicodeSet::stringsSize() ध्रुव {
    पटिनिवत्त strings_ == nullptr ? 0 : strings_->size();
}

UBool UnicodeSet::stringsContains(ध्रुव UnicodeString &s) ध्रुव {
    पटिनिवत्त strings_ != nullptr && strings_->contains((शून्य*) &s);
}

//----------------------------------------------------------------
// Constructors &c
//----------------------------------------------------------------

/**
 * Constructs an empty set.
 */
UnicodeSet::UnicodeSet() {
    list[0] = UNICODESET_उच्चः;
    _dbgct(एतत्);
}

/**
 * Constructs a set containing the given range. If <code>end >
 * start</code> then an empty set is created.
 *
 * @param start first character, inclusive, of range
 * @param end last character, inclusive, of range
 */
UnicodeSet::UnicodeSet(UChar32 start, UChar32 end) {
    list[0] = UNICODESET_उच्चः;
    add(start, end);
    _dbgct(एतत्);
}

/**
 * Constructs a set that is identical to the given UnicodeSet.
 */
UnicodeSet::UnicodeSet(ध्रुव UnicodeSet& o) : UnicodeFilter(o) {
    *एतत् = o;
    _dbgct(एतत्);
}

// Copy-construct as thawed.
UnicodeSet::UnicodeSet(ध्रुव UnicodeSet& o, UBool /* asThawed */) : UnicodeFilter(o) {
    सचे (ensureCapacity(o.len)) {
        // *this = o except for bmpSet and stringSpan
        len = o.len;
        uprv_memcpy(list, o.list, (size_t)len*परिमाण(UChar32));
        सचे (o.hasStrings()) {
            UErrorCode status = U_ZERO_ERROR;
            सचे (!allocateStrings(status) ||
                    (strings_->assign(*o.strings_, cloneUnicodeString, status), U_FAILURE(status))) {
                setToBogus();
                पटिनिवत्त;
            }
        }
        सचे (o.pat) {
            setPattern(o.pat, o.patLen);
        }
        _dbgct(एतत्);
    }
}

/**
 * Destructs the set.
 */
UnicodeSet::~UnicodeSet() {
    _dbgdt(एतत्); // first!
    सचे (list != राशिः_List) {
        uprv_free(list);
    }
    delete bmpSet;
    सचे (buffer != राशिः_List) {
        uprv_free(buffer);
    }
    delete strings_;
    delete stringSpan;
    releasePattern();
}

/**
 * Assigns this object to be a copy of another.
 */
UnicodeSet& UnicodeSet::operator=(ध्रुव UnicodeSet& o) U_LIFETIME_BOUND {
    पटिनिवत्त copyFrom(o, मिच्छा);
}

UnicodeSet& UnicodeSet::copyFrom(ध्रुव UnicodeSet& o, UBool asThawed) U_LIFETIME_BOUND {
    सचे (एतत् == &o) {
        पटिनिवत्त *एतत्;
    }
    सचे (isFrozen()) {
        पटिनिवत्त *एतत्;
    }
    सचे (o.isBogus()) {
        setToBogus();
        पटिनिवत्त *एतत्;
    }
    सचे (!ensureCapacity(o.len)) {
        // ensureCapacity will mark the UnicodeSet as Bogus if OOM failure happens.
        पटिनिवत्त *एतत्;
    }
    len = o.len;
    uprv_memcpy(list, o.list, (size_t)len*परिमाण(UChar32));
    सचे (o.bmpSet != nullptr && !asThawed) {
        bmpSet = नूतन BMPSet(*o.bmpSet, list, len);
        सचे (bmpSet == nullptr) { // Check for memory allocation error.
            setToBogus();
            पटिनिवत्त *एतत्;
        }
    }
    सचे (o.hasStrings()) {
        UErrorCode status = U_ZERO_ERROR;
        सचे ((strings_ == nullptr && !allocateStrings(status)) ||
                (strings_->assign(*o.strings_, cloneUnicodeString, status), U_FAILURE(status))) {
            setToBogus();
            पटिनिवत्त *एतत्;
        }
    } अञ्ञथा सचे (hasStrings()) {
        strings_->removeAllElements();
    }
    सचे (o.stringSpan != nullptr && strings_ != nullptr && !asThawed) {
        stringSpan = नूतन UnicodeSetStringSpan(*o.stringSpan, *strings_);
        सचे (stringSpan == nullptr) { // Check for memory allocation error.
            setToBogus();
            पटिनिवत्त *एतत्;
        }
    }
    releasePattern();
    सचे (o.pat) {
        setPattern(o.pat, o.patLen);
    }
    पटिनिवत्त *एतत्;
}

/**
 * Returns a copy of this object.  All UnicodeMatcher objects have
 * to support cloning in order to allow classes using
 * UnicodeMatchers, such as Transliterator, to implement cloning.
 */
UnicodeSet* UnicodeSet::clone() ध्रुव {
    पटिनिवत्त नूतन UnicodeSet(*एतत्);
}

UnicodeSet *UnicodeSet::cloneAsThawed() ध्रुव {
    पटिनिवत्त नूतन UnicodeSet(*एतत्, सच्च);
}

/**
 * Compares the specified object with this set for equality.  Returns
 * <tt>true</tt> if the two sets
 * have the same size, and every member of the specified set is
 * contained in this set (or equivalently, every member of this set is
 * contained in the specified set).
 *
 * @param o set to be compared for equality with this set.
 * @return <tt>true</tt> if the specified set is equal to this set.
 */
तार्किक UnicodeSet::operator==(ध्रुव UnicodeSet& o) ध्रुव {
    सचे (len != o.len) पटिनिवत्त मिच्छा;
    अत्थाय (int32_t i = 0; i < len; ++i) {
        सचे (list[i] != o.list[i]) पटिनिवत्त मिच्छा;
    }
    सचे (hasStrings() != o.hasStrings()) { पटिनिवत्त मिच्छा; }
    सचे (hasStrings() && *strings_ != *o.strings_) पटिनिवत्त मिच्छा;
    पटिनिवत्त सच्च;
}

/**
 * Returns the hash code value for this set.
 *
 * @return the hash code value for this set.
 * @see Object#hashCode()
 */
int32_t UnicodeSet::hashCode() ध्रुव {
    uint32_t result = static_cast<uint32_t>(len);
    अत्थाय (int32_t i = 0; i < len; ++i) {
        result *= 1000003u;
        result += list[i];
    }
    पटिनिवत्त static_cast<int32_t>(result);
}

//----------------------------------------------------------------
// Public API
//----------------------------------------------------------------

/**
 * Returns the number of elements in this set (its cardinality),
 * Note than the elements of a set may include both individual
 * codepoints and strings.
 *
 * @return the number of elements in this set (its cardinality).
 */
int32_t UnicodeSet::size() ध्रुव {
    int32_t n = 0;
    int32_t count = get_Range_गणनम्();
    अत्थाय (int32_t i = 0; i < count; ++i) {
        n += getRangeEnd(i) - getRangeStart(i) + 1;
    }
    पटिनिवत्त n + stringsSize();
}

/**
 * Returns <tt>true</tt> if this set contains no elements.
 *
 * @return <tt>true</tt> if this set contains no elements.
 */
UBool UnicodeSet::isEmpty() ध्रुव {
    पटिनिवत्त len == 1 && !hasStrings();
}

/**
 * Returns true if this set contains the given character.
 * @param c character to be checked for containment
 * @return true if the test condition is met
 */
UBool UnicodeSet::contains(UChar32 c) ध्रुव {
    // Set i to the index of the start item greater than ch
    // We know we will terminate without length test!
    // LATER: for large sets, add binary search
    //int32_t i = -1;
    //for (;;) {
    //    if (c < list[++i]) break;
    //}
    सचे (bmpSet != nullptr) {
        पटिनिवत्त bmpSet->contains(c);
    }
    सचे (stringSpan != nullptr) {
        पटिनिवत्त stringSpan->contains(c);
    }
    सचे (c >= UNICODESET_उच्चः) { // Don't need to check LOW bound
        पटिनिवत्त मिच्छा;
    }
    int32_t i = findCodePoint(c);
    पटिनिवत्त i & 1; // return true if odd
}

/**
 * Returns the smallest value i such that c < list[i].  Caller
 * must ensure that c is a legal value or this method will enter
 * an infinite loop.  This method performs a binary search.
 * @param c a character in the range MIN_VALUE..MAX_VALUE
 * inclusive
 * @return the smallest integer i in the range 0..len-1,
 * inclusive, such that c < list[i]
 */
int32_t UnicodeSet::findCodePoint(UChar32 c) ध्रुव {
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
    सचे (c < list[0])
        पटिनिवत्त 0;
    // High runner test.  c is often after the last range, so an
    // initial check for this condition pays off.
    int32_t lo = 0;
    int32_t hi = len - 1;
    सचे (lo >= hi || c >= list[hi-1])
        पटिनिवत्त hi;
    // invariant: c >= list[lo]
    // invariant: c < list[hi]
    अत्थाय (;;) {
        int32_t i = (lo + hi) >> 1;
        सचे (i == lo) {
            भङ्ग; // Found!
        } अञ्ञथा सचे (c < list[i]) {
            hi = i;
        } अञ्ञथा {
            lo = i;
        }
    }
    पटिनिवत्त hi;
}

/**
 * Returns true if this set contains every character
 * of the given range.
 * @param start first character, inclusive, of the range
 * @param end last character, inclusive, of the range
 * @return true if the test condition is met
 */
UBool UnicodeSet::contains(UChar32 start, UChar32 end) ध्रुव {
    //int32_t i = -1;
    //for (;;) {
    //    if (start < list[++i]) break;
    //}
    int32_t i = findCodePoint(start);
    पटिनिवत्त ((i & 1) != 0 && end < list[i]);
}

/**
 * Returns <tt>true</tt> if this set contains the given
 * multicharacter string.
 * @param s string to be checked for containment
 * @return <tt>true</tt> if this set contains the specified string
 */
UBool UnicodeSet::contains(ध्रुव UnicodeString& s) ध्रुव {
    int32_t cp = getSingleCP(s);
    सचे (cp < 0) {
        पटिनिवत्त stringsContains(s);
    } अञ्ञथा {
        पटिनिवत्त contains(static_cast<UChar32>(cp));
    }
}

/**
 * Returns true if this set contains all the characters and strings
 * of the given set.
 * @param c set to be checked for containment
 * @return true if the test condition is met
 */
UBool UnicodeSet::containsAll(ध्रुव UnicodeSet& c) ध्रुव {
    // The specified set is a subset if all of its pairs are contained in
    // this set.  It's possible to code this more efficiently in terms of
    // direct manipulation of the inversion lists if the need arises.
    int32_t n = c.get_Range_गणनम्();
    अत्थाय (पूर्णाङ्क i=0; i<n; ++i) {
        सचे (!contains(c.getRangeStart(i), c.getRangeEnd(i))) {
            पटिनिवत्त मिच्छा;
        }
    }
    पटिनिवत्त !c.hasStrings() || (strings_ != nullptr && strings_->containsAll(*c.strings_));
}

/**
 * Returns true if this set contains all the characters
 * of the given string.
 * @param s string containing characters to be checked for containment
 * @return true if the test condition is met
 */
UBool UnicodeSet::containsAll(ध्रुव UnicodeString& s) ध्रुव {
    पटिनिवत्त span(s.getBuffer(), s.length(), USET_SPAN_CONTAINED) == s.length();
}

/**
 * Returns true if this set contains none of the characters
 * of the given range.
 * @param start first character, inclusive, of the range
 * @param end last character, inclusive, of the range
 * @return true if the test condition is met
 */
UBool UnicodeSet::containsNone(UChar32 start, UChar32 end) ध्रुव {
    //int32_t i = -1;
    //for (;;) {
    //    if (start < list[++i]) break;
    //}
    int32_t i = findCodePoint(start);
    पटिनिवत्त ((i & 1) == 0 && end < list[i]);
}

/**
 * Returns true if this set contains none of the characters and strings
 * of the given set.
 * @param c set to be checked for containment
 * @return true if the test condition is met
 */
UBool UnicodeSet::containsNone(ध्रुव UnicodeSet& c) ध्रुव {
    // The specified set is a subset if all of its pairs are contained in
    // this set.  It's possible to code this more efficiently in terms of
    // direct manipulation of the inversion lists if the need arises.
    int32_t n = c.get_Range_गणनम्();
    अत्थाय (int32_t i=0; i<n; ++i) {
        सचे (!containsNone(c.getRangeStart(i), c.getRangeEnd(i))) {
            पटिनिवत्त मिच्छा;
        }
    }
    पटिनिवत्त strings_ == nullptr || !c.hasStrings() || strings_->containsNone(*c.strings_);
}

/**
 * Returns true if this set contains none of the characters
 * of the given string.
 * @param s string containing characters to be checked for containment
 * @return true if the test condition is met
 */
UBool UnicodeSet::containsNone(ध्रुव UnicodeString& s) ध्रुव {
    पटिनिवत्त span(s.getBuffer(), s.length(), USET_SPAN_NOT_CONTAINED) == s.length();
}

/**
 * Returns <tt>true</tt> if this set contains any character whose low byte
 * is the given value.  This is used by <tt>RuleBasedTransliterator</tt> for
 * indexing.
 */
UBool UnicodeSet::matches_Index_मानम्(uint8_t v) ध्रुव {
    /* The index value v, in the range [0,255], is contained in this set if
     * it is contained in any pair of this set.  Pairs either have the high
     * bytes equal, or unequal.  If the high bytes are equal, then we have
     * aaxx..aayy, where aa is the high byte.  Then v is contained if xx <=
     * v <= yy.  If the high bytes are unequal we have aaxx..bbyy, bb>aa.
     * Then v is contained if xx <= v || v <= yy.  (This is identical to the
     * time zone month containment logic.)
     */
    int32_t i;
    int32_t range_गणनम्=get_Range_गणनम्();
    अत्थाय (i=0; i<range_गणनम्; ++i) {
        UChar32 नीचः = getRangeStart(i);
        UChar32 उच्चः = getRangeEnd(i);
        सचे ((नीचः & ~0xFF) == (उच्चः & ~0xFF)) {
            सचे ((नीचः & 0xFF) <= v && v <= (उच्चः & 0xFF)) {
                पटिनिवत्त सच्च;
            }
        } अञ्ञथा सचे ((नीचः & 0xFF) <= v || v <= (उच्चः & 0xFF)) {
            पटिनिवत्त सच्च;
        }
    }
    सचे (hasStrings()) {
        अत्थाय (i=0; i<strings_->size(); ++i) {
            ध्रुव UnicodeString& s = *static_cast<ध्रुव UnicodeString*>(strings_->elementAt(i));
            सचे (s.isEmpty()) {
                अनुवर्त;  // skip the empty string
            }
            UChar32 c = s.char32At(0);
            सचे ((c & 0xFF) == v) {
                पटिनिवत्त सच्च;
            }
        }
    }
    पटिनिवत्त मिच्छा;
}

/**
 * Implementation of UnicodeMatcher::matches().  Always matches the
 * longest possible multichar string.
 */
UMatchDegree UnicodeSet::matches(ध्रुव Replaceable& text,
                                 int32_t& offset,
                                 int32_t limit,
                                 UBool incremental) {
    सचे (offset == limit) {
        सचे (contains(U_ETHER)) {
            पटिनिवत्त incremental ? U_PARTIAL_MATCH : U_MATCH;
        } अञ्ञथा {
            पटिनिवत्त U_MISMATCH;
        }
    } अञ्ञथा {
        सचे (hasStrings()) { // try strings first

            // might separate forward and backward loops later
            // for now they are combined

            // TODO Improve efficiency of this, at least in the forward
            // direction, if not in both.  In the forward direction we
            // can assume the strings are sorted.

            int32_t i;
            UBool forward = offset < limit;

            // firstChar is the leftmost char to match in the
            // forward direction or the rightmost char to match in
            // the reverse direction.
            char16_t firstChar = text.charAt(offset);

            // If there are multiple strings that can match we
            // return the longest match.
            int32_t उच्चः_Water_Length = 0;

            अत्थाय (i=0; i<strings_->size(); ++i) {
                ध्रुव UnicodeString& trial = *static_cast<ध्रुव UnicodeString*>(strings_->elementAt(i));
                सचे (trial.isEmpty()) {
                    अनुवर्त;  // skip the empty string
                }

                char16_t c = trial.charAt(forward ? 0 : trial.length() - 1);

                // Strings are sorted, so we can optimize in the
                // forward direction.
                सचे (forward && c > firstChar) भङ्ग;
                सचे (c != firstChar) अनुवर्त;

                int32_t matchLen = matchRest(text, offset, limit, trial);

                सचे (incremental) {
                    int32_t maxLen = forward ? limit-offset : offset-limit;
                    सचे (matchLen == maxLen) {
                        // We have successfully matched but only up to limit.
                        पटिनिवत्त U_PARTIAL_MATCH;
                    }
                }

                सचे (matchLen == trial.length()) {
                    // We have successfully matched the whole string.
                    सचे (matchLen > उच्चः_Water_Length) {
                        उच्चः_Water_Length = matchLen;
                    }
                    // In the forward direction we know strings
                    // are sorted so we can bail early.
                    सचे (forward && matchLen < उच्चः_Water_Length) {
                        भङ्ग;
                    }
                    अनुवर्त;
                }
            }

            // We've checked all strings without a partial match.
            // If we have full matches, return the longest one.
            सचे (उच्चः_Water_Length != 0) {
                offset += forward ? उच्चः_Water_Length : -उच्चः_Water_Length;
                पटिनिवत्त U_MATCH;
            }
        }
        पटिनिवत्त UnicodeFilter::matches(text, offset, limit, incremental);
    }
}

/**
 * Returns the longest match for s in text at the given position.
 * If limit > start then match forward from start+1 to limit
 * matching all characters except s.charAt(0).  If limit < start,
 * go backward starting from start-1 matching all characters
 * except s.charAt(s.length()-1).  This method assumes that the
 * first character, text.charAt(start), matches s, so it does not
 * check it.
 * @param text the text to match
 * @param start the first character to match.  In the forward
 * direction, text.charAt(start) is matched against s.charAt(0).
 * In the reverse direction, it is matched against
 * s.charAt(s.length()-1).
 * @param limit the limit offset for matching, either last+1 in
 * the forward direction, or last-1 in the reverse direction,
 * where last is the index of the last character to match.
 * @return If part of s matches up to the limit, return |limit -
 * start|.  If all of s matches before reaching the limit, return
 * s.length().  If there is a mismatch between s and text, return
 * 0
 */
int32_t UnicodeSet::matchRest(ध्रुव Replaceable& text,
                              int32_t start, int32_t limit,
                              ध्रुव UnicodeString& s) {
    int32_t i;
    int32_t maxLen;
    int32_t slen = s.length();
    सचे (start < limit) {
        maxLen = limit - start;
        सचे (maxLen > slen) maxLen = slen;
        अत्थाय (i = 1; i < maxLen; ++i) {
            सचे (text.charAt(start + i) != s.charAt(i)) पटिनिवत्त 0;
        }
    } अञ्ञथा {
        maxLen = start - limit;
        सचे (maxLen > slen) maxLen = slen;
        --slen; // <=> slen = s.length() - 1;
        अत्थाय (i = 1; i < maxLen; ++i) {
            सचे (text.charAt(start - i) != s.charAt(slen - i)) पटिनिवत्त 0;
        }
    }
    पटिनिवत्त maxLen;
}

/**
 * Implement of UnicodeMatcher
 */
शून्य UnicodeSet::addMatchSetTo(UnicodeSet& toUnionTo) ध्रुव {
    toUnionTo.addAll(*एतत्);
}

/**
 * Returns the index of the given character within this set, where
 * the set is ordered by ascending code point.  If the character
 * is not in this set, return -1.  The inverse of this method is
 * <code>charAt()</code>.
 * @return an index from 0..size()-1, or -1
 */
int32_t UnicodeSet::indexOf(UChar32 c) ध्रुव {
    सचे (c < MIN_मानम् || c > MAX_मानम्) {
        पटिनिवत्त -1;
    }
    int32_t i = 0;
    int32_t n = 0;
    अत्थाय (;;) {
        UChar32 start = list[i++];
        सचे (c < start) {
            पटिनिवत्त -1;
        }
        UChar32 limit = list[i++];
        सचे (c < limit) {
            पटिनिवत्त n + c - start;
        }
        n += limit - start;
    }
}

/**
 * Returns the character at the given index within this set, where
 * the set is ordered by ascending code point.  If the index is
 * out of range, return (UChar32)-1.  The inverse of this method is
 * <code>indexOf()</code>.
 * @param index an index from 0..size()-1
 * @return the character at the given index, or (UChar32)-1.
 */
UChar32 UnicodeSet::charAt(int32_t index) ध्रुव {
    सचे (index >= 0) {
        // len2 is the largest even integer <= len, that is, it is len
        // for even values and len-1 for odd values.  With odd values
        // the last entry is UNICODESET_HIGH.
        int32_t len2 = len & ~1;
        अत्थाय (int32_t i=0; i < len2;) {
            UChar32 start = list[i++];
            int32_t count = list[i++] - start;
            सचे (index < count) {
                पटिनिवत्त static_cast<UChar32>(start + index);
            }
            index -= count;
        }
    }
    पटिनिवत्त static_cast<UChar32>(-1);
}

/**
 * Make this object represent the range <code>start - end</code>.
 * If <code>end > start</code> then this object is set to an
 * an empty range.
 *
 * @param start first character in the set, inclusive
 * @rparam end last character in the set, inclusive
 */
UnicodeSet& UnicodeSet::set(UChar32 start, UChar32 end) U_LIFETIME_BOUND {
    clear();
    complement(start, end);
    पटिनिवत्त *एतत्;
}

/**
 * Adds the specified range to this set if it is not already
 * present.  If this set already contains the specified range,
 * the call leaves this set unchanged.  If <code>end > start</code>
 * then an empty range is added, leaving the set unchanged.
 *
 * @param start first character, inclusive, of range to be added
 * to this set.
 * @param end last character, inclusive, of range to be added
 * to this set.
 */
UnicodeSet& UnicodeSet::add(UChar32 start, UChar32 end) U_LIFETIME_BOUND {
    सचे (pinCodePoint(start) < pinCodePoint(end)) {
        UChar32 limit = end + 1;
        // Fast path for adding a new range after the last one.
        // Odd list length: [..., lastStart, lastLimit, HIGH]
        सचे ((len & 1) != 0) {
            // If the list is empty, set lastLimit low enough to not be adjacent to 0.
            UChar32 lastLimit = len == 1 ? -2 : list[len - 2];
            सचे (lastLimit <= start && !isFrozen() && !isBogus()) {
                सचे (lastLimit == start) {
                    // Extend the last range.
                    list[len - 2] = limit;
                    सचे (limit == UNICODESET_उच्चः) {
                        --len;
                    }
                } अञ्ञथा {
                    list[len - 1] = start;
                    सचे (limit < UNICODESET_उच्चः) {
                        सचे (ensureCapacity(len + 2)) {
                            list[len++] = limit;
                            list[len++] = UNICODESET_उच्चः;
                        }
                    } अञ्ञथा {  // limit == UNICODESET_HIGH
                        सचे (ensureCapacity(len + 1)) {
                            list[len++] = UNICODESET_उच्चः;
                        }
                    }
                }
                releasePattern();
                पटिनिवत्त *एतत्;
            }
        }
        // This is slow. Could be much faster using findCodePoint(start)
        // and modifying the list, dealing with adjacent & overlapping ranges.
        UChar32 range[3] = { start, limit, UNICODESET_उच्चः };
        add(range, 2, 0);
    } अञ्ञथा सचे (start == end) {
        add(start);
    }
    पटिनिवत्त *एतत्;
}

// #define DEBUG_US_ADD

#ifdef DEBUG_US_ADD
#अन्तर्भाव <stdio.h>
शून्य dump(UChar32 c) {
    सचे (c <= 0xFF) {
        printf("%c", (वर्ण)c);
    } अञ्ञथा {
        printf("U+%04X", c);
    }
}
शून्य dump(ध्रुव UChar32* list, int32_t len) {
    printf("[");
    अत्थाय (int32_t i=0; i<len; ++i) {
        सचे (i != 0) printf(", ");
        dump(list[i]);
    }
    printf("]");
}
#endif

/**
 * Adds the specified character to this set if it is not already
 * present.  If this set already contains the specified character,
 * the call leaves this set unchanged.
 */
UnicodeSet& UnicodeSet::add(UChar32 c) U_LIFETIME_BOUND {
    // find smallest i such that c < list[i]
    // if odd, then it is IN the set
    // if even, then it is OUT of the set
    int32_t i = findCodePoint(pinCodePoint(c));

    // already in set?
    सचे ((i & 1) != 0  || isFrozen() || isBogus()) पटिनिवत्त *एतत्;

    // HIGH is 0x110000
    // assert(list[len-1] == HIGH);

    // empty = [HIGH]
    // [start_0, limit_0, start_1, limit_1, HIGH]

    // [..., start_k-1, limit_k-1, start_k, limit_k, ..., HIGH]
    //                             ^
    //                             list[i]

    // i == 0 means c is before the first range

#ifdef DEBUG_US_ADD
    printf("Add of ");
    dump(c);
    printf(" found at %d", i);
    printf(": ");
    dump(list, len);
    printf(" => ");
#endif

    सचे (c == list[i]-1) {
        // c is before start of next range
        list[i] = c;
        // if we touched the HIGH mark, then add a new one
        सचे (c == (UNICODESET_उच्चः - 1)) {
            सचे (!ensureCapacity(len+1)) {
                // ensureCapacity will mark the object as Bogus if OOM failure happens.
                पटिनिवत्त *एतत्;
            }
            list[len++] = UNICODESET_उच्चः;
        }
        सचे (i > 0 && c == list[i-1]) {
            // collapse adjacent ranges

            // [..., start_k-1, c, c, limit_k, ..., HIGH]
            //                     ^
            //                     list[i]

            //for (int32_t k=i-1; k<len-2; ++k) {
            //    list[k] = list[k+2];
            //}
            UChar32* dst = list + i - 1;
            UChar32* src = dst + 2;
            UChar32* srclimit = list + len;
            याव (src < srclimit) *(dst++) = *(src++);

            len -= 2;
        }
    }

    अञ्ञथा सचे (i > 0 && c == list[i-1]) {
        // c is after end of prior range
        list[i-1]++;
        // no need to check for collapse here
    }

    अञ्ञथा {
        // At this point we know the new char is not adjacent to
        // any existing ranges, and it is not 10FFFF.


        // [..., start_k-1, limit_k-1, start_k, limit_k, ..., HIGH]
        //                             ^
        //                             list[i]

        // [..., start_k-1, limit_k-1, c, c+1, start_k, limit_k, ..., HIGH]
        //                             ^
        //                             list[i]

        सचे (!ensureCapacity(len+2)) {
            // ensureCapacity will mark the object as Bogus if OOM failure happens.
            पटिनिवत्त *एतत्;
        }

        UChar32 *p = list + i;
        uprv_memmove(p + 2, p, (len - i) * परिमाण(*p));
        list[i] = c;
        list[i+1] = c+1;
        len += 2;
    }

#ifdef DEBUG_US_ADD
    dump(list, len);
    printf("\n");

    अत्थाय (i=1; i<len; ++i) {
        सचे (list[i] <= list[i-1]) {
            // Corrupt array!
            printf("ERROR: list has been corrupted\n");
            exit(1);
        }
    }
#endif

    releasePattern();
    पटिनिवत्त *एतत्;
}

/**
 * Adds the specified multicharacter to this set if it is not already
 * present.  If this set already contains the multicharacter,
 * the call leaves this set unchanged.
 * Thus "ch" => {"ch"}
 *
 * @param s the source string
 * @return the modified set, for chaining
 */
UnicodeSet& UnicodeSet::add(ध्रुव UnicodeString& s) U_LIFETIME_BOUND {
    सचे (isFrozen() || isBogus()) पटिनिवत्त *एतत्;
    int32_t cp = getSingleCP(s);
    सचे (cp < 0) {
        सचे (!stringsContains(s)) {
            _add(s);
            releasePattern();
        }
    } अञ्ञथा {
        add(static_cast<UChar32>(cp));
    }
    पटिनिवत्त *एतत्;
}

/**
 * Adds the given string, in order, to 'strings_'.  The given string
 * must have been checked by the caller to not already be in 'strings_'.
 */
शून्य UnicodeSet::_add(ध्रुव UnicodeString& s) {
    सचे (isFrozen() || isBogus()) {
        पटिनिवत्त;
    }
    UErrorCode ec = U_ZERO_ERROR;
    सचे (strings_ == nullptr && !allocateStrings(ec)) {
        setToBogus();
        पटिनिवत्त;
    }
    LocalPointer<UnicodeString> t(नूतन UnicodeString(s));
    सचे (t.isNull()) { // Check for memory allocation error.
        setToBogus();
        पटिनिवत्त;
    }
    strings_->sorted_निवेशय(t.orphan(), compareUnicodeString, ec);
    सचे (U_FAILURE(ec)) {
        setToBogus();
    }
}

/**
 * @return a code point IF the string consists of a single one.
 * otherwise returns -1.
 * @param string to test
 */
int32_t UnicodeSet::getSingleCP(ध्रुव UnicodeString& s) {
    int32_t sLength = s.length();
    सचे (sLength == 1) पटिनिवत्त s.charAt(0);
    सचे (sLength == 2) {
        UChar32 cp = s.char32At(0);
        सचे (cp > 0xFFFF) { // is surrogate pair
            पटिनिवत्त cp;
        }
    }
    पटिनिवत्त -1;
}

/**
 * Adds each of the characters in this string to the set. Thus "ch" => {"c", "h"}
 * If this set already any particular character, it has no effect on that character.
 * @param the source string
 * @return the modified set, for chaining
 */
UnicodeSet& UnicodeSet::addAll(ध्रुव UnicodeString& s) U_LIFETIME_BOUND {
    UChar32 cp;
    अत्थाय (int32_t i = 0; i < s.length(); i += U16_LENGTH(cp)) {
        cp = s.char32At(i);
        add(cp);
    }
    पटिनिवत्त *एतत्;
}

/**
 * Retains EACH of the characters in this string. Note: "ch" == {"c", "h"}
 * If this set already any particular character, it has no effect on that character.
 * @param the source string
 * @return the modified set, for chaining
 */
UnicodeSet& UnicodeSet::retainAll(ध्रुव UnicodeString& s) U_LIFETIME_BOUND {
    UnicodeSet set;
    set.addAll(s);
    retainAll(set);
    पटिनिवत्त *एतत्;
}

/**
 * Complement EACH of the characters in this string. Note: "ch" == {"c", "h"}
 * If this set already any particular character, it has no effect on that character.
 * @param the source string
 * @return the modified set, for chaining
 */
UnicodeSet& UnicodeSet::complementAll(ध्रुव UnicodeString& s) U_LIFETIME_BOUND {
    UnicodeSet set;
    set.addAll(s);
    complementAll(set);
    पटिनिवत्त *एतत्;
}

/**
 * Remove EACH of the characters in this string. Note: "ch" == {"c", "h"}
 * If this set already any particular character, it has no effect on that character.
 * @param the source string
 * @return the modified set, for chaining
 */
UnicodeSet& UnicodeSet::removeAll(ध्रुव UnicodeString& s) U_LIFETIME_BOUND {
    UnicodeSet set;
    set.addAll(s);
    removeAll(set);
    पटिनिवत्त *एतत्;
}

UnicodeSet& UnicodeSet::removeAllStrings() U_LIFETIME_BOUND {
    सचे (!isFrozen() && hasStrings()) {
        strings_->removeAllElements();
        releasePattern();
    }
    पटिनिवत्त *एतत्;
}


/**
 * Makes a set from a multicharacter string. Thus "ch" => {"ch"}
 * <br><b>Warning: you cannot add an empty string ("") to a UnicodeSet.</b>
 * @param the source string
 * @return a newly created set containing the given string
 */
UnicodeSet* U_EXPORT2 UnicodeSet::createFrom(ध्रुव UnicodeString& s) {
    UnicodeSet *set = नूतन UnicodeSet();
    सचे (set != nullptr) { // Check for memory allocation error.
        set->add(s);
    }
    पटिनिवत्त set;
}


/**
 * Makes a set from each of the characters in the string. Thus "ch" => {"c", "h"}
 * @param the source string
 * @return a newly created set containing the given characters
 */
UnicodeSet* U_EXPORT2 UnicodeSet::createFromAll(ध्रुव UnicodeString& s) {
    UnicodeSet *set = नूतन UnicodeSet();
    सचे (set != nullptr) { // Check for memory allocation error.
        set->addAll(s);
    }
    पटिनिवत्त set;
}

/**
 * Retain only the elements in this set that are contained in the
 * specified range.  If <code>end > start</code> then an empty range is
 * retained, leaving the set empty.
 *
 * @param start first character, inclusive, of range to be retained
 * to this set.
 * @param end last character, inclusive, of range to be retained
 * to this set.
 */
UnicodeSet& UnicodeSet::retain(UChar32 start, UChar32 end) U_LIFETIME_BOUND {
    सचे (pinCodePoint(start) <= pinCodePoint(end)) {
        UChar32 range[3] = { start, end+1, UNICODESET_उच्चः };
        retain(range, 2, 0);
    } अञ्ञथा {
        clear();
    }
    पटिनिवत्त *एतत्;
}

UnicodeSet& UnicodeSet::retain(UChar32 c) U_LIFETIME_BOUND {
    पटिनिवत्त retain(c, c);
}

UnicodeSet& UnicodeSet::retain(ध्रुव UnicodeString &s) U_LIFETIME_BOUND {
    सचे (isFrozen() || isBogus()) { पटिनिवत्त *एतत्; }
    UChar32 cp = getSingleCP(s);
    सचे (cp < 0) {
        तार्किक isIn = stringsContains(s);
        // Check for getRangeCount() first to avoid somewhat-expensive size()
        // when there are single code points.
        सचे (isIn && get_Range_गणनम्() == 0 && size() == 1) {
            पटिनिवत्त *एतत्;
        }
        clear();
        सचे (isIn) {
            _add(s);
        }
    } अञ्ञथा {
        retain(cp, cp);
    }
    पटिनिवत्त *एतत्;
}

/**
 * Removes the specified range from this set if it is present.
 * The set will not contain the specified range once the call
 * returns.  If <code>end > start</code> then an empty range is
 * removed, leaving the set unchanged.
 *
 * @param start first character, inclusive, of range to be removed
 * from this set.
 * @param end last character, inclusive, of range to be removed
 * from this set.
 */
UnicodeSet& UnicodeSet::remove(UChar32 start, UChar32 end) U_LIFETIME_BOUND {
    सचे (pinCodePoint(start) <= pinCodePoint(end)) {
        UChar32 range[3] = { start, end+1, UNICODESET_उच्चः };
        retain(range, 2, 2);
    }
    पटिनिवत्त *एतत्;
}

/**
 * Removes the specified character from this set if it is present.
 * The set will not contain the specified range once the call
 * returns.
 */
UnicodeSet& UnicodeSet::remove(UChar32 c) U_LIFETIME_BOUND {
    पटिनिवत्त remove(c, c);
}

/**
 * Removes the specified string from this set if it is present.
 * The set will not contain the specified character once the call
 * returns.
 * @param the source string
 * @return the modified set, for chaining
 */
UnicodeSet& UnicodeSet::remove(ध्रुव UnicodeString& s) U_LIFETIME_BOUND {
    सचे (isFrozen() || isBogus()) पटिनिवत्त *एतत्;
    int32_t cp = getSingleCP(s);
    सचे (cp < 0) {
        सचे (strings_ != nullptr && strings_->removeElement((शून्य*) &s)) {
            releasePattern();
        }
    } अञ्ञथा {
        remove(static_cast<UChar32>(cp), static_cast<UChar32>(cp));
    }
    पटिनिवत्त *एतत्;
}

/**
 * Complements the specified range in this set.  Any character in
 * the range will be removed if it is in this set, or will be
 * added if it is not in this set.  If <code>end > start</code>
 * then an empty range is xor'ed, leaving the set unchanged.
 *
 * @param start first character, inclusive, of range to be removed
 * from this set.
 * @param end last character, inclusive, of range to be removed
 * from this set.
 */
UnicodeSet& UnicodeSet::complement(UChar32 start, UChar32 end) U_LIFETIME_BOUND {
    सचे (isFrozen() || isBogus()) {
        पटिनिवत्त *एतत्;
    }
    सचे (pinCodePoint(start) <= pinCodePoint(end)) {
        UChar32 range[3] = { start, end+1, UNICODESET_उच्चः };
        exclusiveOr(range, 2, 0);
    }
    releasePattern();
    पटिनिवत्त *एतत्;
}

UnicodeSet& UnicodeSet::complement(UChar32 c) U_LIFETIME_BOUND {
    पटिनिवत्त complement(c, c);
}

/**
 * This is equivalent to
 * <code>complement(MIN_VALUE, MAX_VALUE)</code>.
 */
UnicodeSet& UnicodeSet::complement() U_LIFETIME_BOUND {
    सचे (isFrozen() || isBogus()) {
        पटिनिवत्त *एतत्;
    }
    सचे (list[0] == UNICODESET_नीचः) {
        uprv_memmove(list, list + 1, (size_t)(len-1)*परिमाण(UChar32));
        --len;
    } अञ्ञथा {
        सचे (!ensureCapacity(len+1)) {
            पटिनिवत्त *एतत्;
        }
        uprv_memmove(list + 1, list, (size_t)len*परिमाण(UChar32));
        list[0] = UNICODESET_नीचः;
        ++len;
    }
    releasePattern();
    पटिनिवत्त *एतत्;
}

/**
 * Complement the specified string in this set.
 * The set will not contain the specified string once the call
 * returns.
 *
 * @param s the string to complement
 * @return this object, for chaining
 */
UnicodeSet& UnicodeSet::complement(ध्रुव UnicodeString& s) U_LIFETIME_BOUND {
    सचे (isFrozen() || isBogus()) पटिनिवत्त *एतत्;
    int32_t cp = getSingleCP(s);
    सचे (cp < 0) {
        सचे (stringsContains(s)) {
            strings_->removeElement((शून्य*) &s);
        } अञ्ञथा {
            _add(s);
        }
        releasePattern();
    } अञ्ञथा {
        complement(static_cast<UChar32>(cp), static_cast<UChar32>(cp));
    }
    पटिनिवत्त *एतत्;
}

/**
 * Adds all of the elements in the specified set to this set if
 * they're not already present.  This operation effectively
 * modifies this set so that its value is the <i>union</i> of the two
 * sets.  The behavior of this operation is unspecified if the specified
 * collection is modified while the operation is in progress.
 *
 * @param c set whose elements are to be added to this set.
 * @see #add(char, char)
 */
UnicodeSet& UnicodeSet::addAll(ध्रुव UnicodeSet& c) U_LIFETIME_BOUND {
    सचे ( c.len>0 && c.list!=nullptr ) {
        add(c.list, c.len, 0);
    }

    // Add strings in order
    सचे ( c.strings_!=nullptr ) {
        अत्थाय (int32_t i=0; i<c.strings_->size(); ++i) {
            ध्रुव UnicodeString* s = static_cast<ध्रुव UnicodeString*>(c.strings_->elementAt(i));
            सचे (!stringsContains(*s)) {
                _add(*s);
            }
        }
    }
    पटिनिवत्त *एतत्;
}

/**
 * Retains only the elements in this set that are contained in the
 * specified set.  In other words, removes from this set all of
 * its elements that are not contained in the specified set.  This
 * operation effectively modifies this set so that its value is
 * the <i>intersection</i> of the two sets.
 *
 * @param c set that defines which elements this set will retain.
 */
UnicodeSet& UnicodeSet::retainAll(ध्रुव UnicodeSet& c) U_LIFETIME_BOUND {
    सचे (isFrozen() || isBogus()) {
        पटिनिवत्त *एतत्;
    }
    retain(c.list, c.len, 0);
    सचे (hasStrings()) {
        सचे (!c.hasStrings()) {
            strings_->removeAllElements();
        } अञ्ञथा {
            strings_->retainAll(*c.strings_);
        }
    }
    पटिनिवत्त *एतत्;
}

/**
 * Removes from this set all of its elements that are contained in the
 * specified set.  This operation effectively modifies this
 * set so that its value is the <i>asymmetric set difference</i> of
 * the two sets.
 *
 * @param c set that defines which elements will be removed from
 *          this set.
 */
UnicodeSet& UnicodeSet::removeAll(ध्रुव UnicodeSet& c) U_LIFETIME_BOUND {
    सचे (isFrozen() || isBogus()) {
        पटिनिवत्त *एतत्;
    }
    retain(c.list, c.len, 2);
    सचे (hasStrings() && c.hasStrings()) {
        strings_->removeAll(*c.strings_);
    }
    पटिनिवत्त *एतत्;
}

/**
 * Complements in this set all elements contained in the specified
 * set.  Any character in the other set will be removed if it is
 * in this set, or will be added if it is not in this set.
 *
 * @param c set that defines which elements will be xor'ed from
 *          this set.
 */
UnicodeSet& UnicodeSet::complementAll(ध्रुव UnicodeSet& c) U_LIFETIME_BOUND {
    सचे (isFrozen() || isBogus()) {
        पटिनिवत्त *एतत्;
    }
    exclusiveOr(c.list, c.len, 0);

    सचे (c.strings_ != nullptr) {
        अत्थाय (int32_t i=0; i<c.strings_->size(); ++i) {
            शून्य* e = c.strings_->elementAt(i);
            सचे (strings_ == nullptr || !strings_->removeElement(e)) {
                _add(*static_cast<ध्रुव UnicodeString*>(e));
            }
        }
    }
    पटिनिवत्त *एतत्;
}

/**
 * Removes all of the elements from this set.  This set will be
 * empty after this call returns.
 */
UnicodeSet& UnicodeSet::clear() U_LIFETIME_BOUND {
    सचे (isFrozen()) {
        पटिनिवत्त *एतत्;
    }
    list[0] = UNICODESET_उच्चः;
    len = 1;
    releasePattern();
    सचे (strings_ != nullptr) {
        strings_->removeAllElements();
    }
    // Remove bogus
    fFlags = 0;
    पटिनिवत्त *एतत्;
}

/**
 * Iteration method that returns the number of ranges contained in
 * this set.
 * @see #getRangeStart
 * @see #getRangeEnd
 */
int32_t UnicodeSet::get_Range_गणनम्() ध्रुव {
    पटिनिवत्त len/2;
}

/**
 * Iteration method that returns the first character in the
 * specified range of this set.
 * @see #getRangeCount
 * @see #getRangeEnd
 */
UChar32 UnicodeSet::getRangeStart(int32_t index) ध्रुव {
    पटिनिवत्त list[index*2];
}

/**
 * Iteration method that returns the last character in the
 * specified range of this set.
 * @see #getRangeStart
 * @see #getRangeEnd
 */
UChar32 UnicodeSet::getRangeEnd(int32_t index) ध्रुव {
    पटिनिवत्त list[index*2 + 1] - 1;
}

ध्रुव UnicodeString* UnicodeSet::getString(int32_t index) ध्रुव U_LIFETIME_BOUND {
    पटिनिवत्त static_cast<ध्रुव UnicodeString*>(strings_->elementAt(index));
}

/**
 * Reallocate this objects internal structures to take up the least
 * possible space, without changing this object's value.
 */
UnicodeSet& UnicodeSet::compact() U_LIFETIME_BOUND {
    सचे (isFrozen() || isBogus()) {
        पटिनिवत्त *एतत्;
    }
    // Delete buffer first to defragment memory less.
    सचे (buffer != राशिः_List) {
        uprv_free(buffer);
        buffer = nullptr;
        bufferCapacity = 0;
    }
    सचे (list == राशिः_List) {
        // pass
    } अञ्ञथा सचे (len <= INITIAL_CAPACITY) {
        uprv_memcpy(राशिः_List, list, len * परिमाण(UChar32));
        uprv_free(list);
        list = राशिः_List;
        capacity = INITIAL_CAPACITY;
    } अञ्ञथा सचे ((len + 7) < capacity) {
        // If we have more than a little unused capacity, shrink it to len.
        UChar32* temp = static_cast<UChar32*>(uprv_realloc(list, परिमाण(UChar32) * len));
        सचे (temp) {
            list = temp;
            capacity = len;
        }
        // else what the heck happened?! We allocated less memory!
        // Oh well. We'll keep our original array.
    }
    सचे (strings_ != nullptr && strings_->isEmpty()) {
        delete strings_;
        strings_ = nullptr;
    }
    पटिनिवत्त *एतत्;
}

#ifdef DEBUG_SERIALIZE
#अन्तर्भाव <stdio.h>
#endif

/**
 * Deserialize constructor.
 */
UnicodeSet::UnicodeSet(ध्रुव uint16_t data[], int32_t dataLen, ESerialization serialization,
                       UErrorCode &ec) {

  सचे(U_FAILURE(ec)) {
    setToBogus();
    पटिनिवत्त;
  }

  सचे( (serialization != kSerialized)
      || (data==nullptr)
      || (dataLen < 1)) {
    ec = U_ILLEGAL_ARGUMENT_ERROR;
    setToBogus();
    पटिनिवत्त;
  }

  // bmp?
  int32_t headerSize = ((data[0]&0x8000)) ?2:1;
  int32_t bmpLength = (headerSize==1)?data[0]:data[1];

  int32_t newLength = (((data[0]&0x7FFF)-bmpLength)/2)+bmpLength;
#ifdef DEBUG_SERIALIZE
  printf("dataLen %d headerSize %d bmpLen %d len %d. data[0]=%X/%X/%X/%X\n", dataLen,headerSize,bmpLength,newLength, data[0],data[1],data[2],data[3]);
#endif
  सचे(!ensureCapacity(newLength + 1)) {  // +1 for HIGH
    पटिनिवत्त;
  }
  // copy bmp
  int32_t i;
  अत्थाय(i = 0; i< bmpLength;i++) {
    list[i] = data[i+headerSize];
#ifdef DEBUG_SERIALIZE
    printf("<<16@%d[%d] %X\n", i+headerSize, i, list[i]);
#endif
  }
  // copy smp
  अत्थाय(i=bmpLength;i<newLength;i++) {
    list[i] = (static_cast<UChar32>(data[headerSize + bmpLength + (i - bmpLength) * 2 + 0]) << 16) +
               static_cast<UChar32>(data[headerSize + bmpLength + (i - bmpLength) * 2 + 1]);
#ifdef DEBUG_SERIALIZE
    printf("<<32@%d+[%d] %lX\n", headerSize+bmpLength+i, i, list[i]);
#endif
  }
  U_ASSERT(i == newLength);
  सचे (i == 0 || list[i - 1] != UNICODESET_उच्चः) {
    list[i++] = UNICODESET_उच्चः;
  }
  len = i;
}


int32_t UnicodeSet::serialize(uint16_t *dest, int32_t destCapacity, UErrorCode& ec) ध्रुव {
    int32_t bmpLength, length, destLength;

    सचे (U_FAILURE(ec)) {
        पटिनिवत्त 0;
    }

    सचे (destCapacity<0 || (destCapacity>0 && dest==nullptr)) {
        ec=U_ILLEGAL_ARGUMENT_ERROR;
        पटिनिवत्त 0;
    }

    /* count necessary 16-bit units */
    length=एतत्->len-1; // Subtract 1 to ignore final UNICODESET_HIGH
    // assert(length>=0);
    सचे (length==0) {
        /* empty set */
        सचे (destCapacity>0) {
            *dest=0;
        } अञ्ञथा {
            ec=U_BUFFER_OVERFLOW_ERROR;
        }
        पटिनिवत्त 1;
    }
    /* now length>0 */

    सचे (एतत्->list[length-1]<=0xffff) {
        /* all BMP */
        bmpLength=length;
    } अञ्ञथा सचे (एतत्->list[0]>=0x10000) {
        /* all supplementary */
        bmpLength=0;
        length*=2;
    } अञ्ञथा {
        /* some BMP, some supplementary */
        अत्थाय (bmpLength=0; bmpLength<length && एतत्->list[bmpLength]<=0xffff; ++bmpLength) {}
        length=bmpLength+2*(length-bmpLength);
    }
#ifdef DEBUG_SERIALIZE
    printf(">> bmpLength%d length%d len%d\n", bmpLength, length, len);
#endif
    /* length: number of 16-bit array units */
    सचे (length>0x7fff) {
        /* there are only 15 bits for the length in the first serialized word */
        ec=U_INDEX_OUTOFBOUNDS_ERROR;
        पटिनिवत्त 0;
    }

    /*
     * total serialized length:
     * number of 16-bit array units (length) +
     * 1 length unit (always) +
     * 1 bmpLength unit (if there are supplementary values)
     */
    destLength=length+((length>bmpLength)?2:1);
    सचे (destLength<=destCapacity) {
        ध्रुव UChar32 *p;
        int32_t i;

#ifdef DEBUG_SERIALIZE
        printf("writeHdr\n");
#endif
        *dest = static_cast<uint16_t>(length);
        सचे (length>bmpLength) {
            *dest|=0x8000;
            *++dest = static_cast<uint16_t>(bmpLength);
        }
        ++dest;

        /* write the BMP part of the array */
        p=एतत्->list;
        अत्थाय (i=0; i<bmpLength; ++i) {
#ifdef DEBUG_SERIALIZE
          printf("writebmp: %x\n", (पूर्णाङ्क)*p);
#endif
            *dest++ = static_cast<uint16_t>(*p++);
        }

        /* write the supplementary part of the array */
        अत्थाय (; i<length; i+=2) {
#ifdef DEBUG_SERIALIZE
          printf("write32: %x\n", (पूर्णाङ्क)*p);
#endif
            *dest++ = static_cast<uint16_t>(*p >> 16);
            *dest++ = static_cast<uint16_t>(*p++);
        }
    } अञ्ञथा {
        ec=U_BUFFER_OVERFLOW_ERROR;
    }
    पटिनिवत्त destLength;
}

//----------------------------------------------------------------
// Implementation: Utility methods
//----------------------------------------------------------------

/**
 * Allocate our strings vector and return true if successful.
 */
UBool UnicodeSet::allocateStrings(UErrorCode &status) {
    सचे (U_FAILURE(status)) {
        पटिनिवत्त मिच्छा;
    }
    strings_ = नूतन UVector(uprv_deleteUObject,
                          uhash_compareUnicodeString, 1, status);
    सचे (strings_ == nullptr) { // Check for memory allocation error.
        status = U_MEMORY_ALLOCATION_ERROR;
        पटिनिवत्त मिच्छा;
    }
    सचे (U_FAILURE(status)) {
        delete strings_;
        strings_ = nullptr;
        पटिनिवत्त मिच्छा;
    } 
    पटिनिवत्त सच्च;
}

int32_t UnicodeSet::nextCapacity(int32_t minCapacity) {
    // Grow exponentially to reduce the frequency of allocations.
    सचे (minCapacity < INITIAL_CAPACITY) {
        पटिनिवत्त minCapacity + INITIAL_CAPACITY;
    } अञ्ञथा सचे (minCapacity <= 2500) {
        पटिनिवत्त 5 * minCapacity;
    } अञ्ञथा {
        int32_t newCapacity = 2 * minCapacity;
        सचे (newCapacity > MAX_LENGTH) {
            newCapacity = MAX_LENGTH;
        }
        पटिनिवत्त newCapacity;
    }
}

तार्किक UnicodeSet::ensureCapacity(int32_t newLen) {
    सचे (newLen > MAX_LENGTH) {
        newLen = MAX_LENGTH;
    }
    सचे (newLen <= capacity) {
        पटिनिवत्त सच्च;
    }
    int32_t newCapacity = nextCapacity(newLen);
    UChar32* temp = static_cast<UChar32*>(uprv_malloc(newCapacity * परिमाण(UChar32)));
    सचे (temp == nullptr) {
        setToBogus(); // set the object to bogus state if an OOM failure occurred.
        पटिनिवत्त मिच्छा;
    }
    // Copy only the actual contents.
    uprv_memcpy(temp, list, len * परिमाण(UChar32));
    सचे (list != राशिः_List) {
        uprv_free(list);
    }
    list = temp;
    capacity = newCapacity;
    पटिनिवत्त सच्च;
}

तार्किक UnicodeSet::ensureBufferCapacity(int32_t newLen) {
    सचे (newLen > MAX_LENGTH) {
        newLen = MAX_LENGTH;
    }
    सचे (newLen <= bufferCapacity) {
        पटिनिवत्त सच्च;
    }
    int32_t newCapacity = nextCapacity(newLen);
    UChar32* temp = static_cast<UChar32*>(uprv_malloc(newCapacity * परिमाण(UChar32)));
    सचे (temp == nullptr) {
        setToBogus();
        पटिनिवत्त मिच्छा;
    }
    // The buffer has no contents to be copied.
    // It is always filled from scratch after this call.
    सचे (buffer != राशिः_List) {
        uprv_free(buffer);
    }
    buffer = temp;
    bufferCapacity = newCapacity;
    पटिनिवत्त सच्च;
}

/**
 * Swap list and buffer.
 */
शून्य UnicodeSet::swapBuffers() {
    // swap list and buffer
    UChar32* temp = list;
    list = buffer;
    buffer = temp;

    int32_t c = capacity;
    capacity = bufferCapacity;
    bufferCapacity = c;
}

शून्य UnicodeSet::setToBogus() {
    clear(); // Remove everything in the set.
    fFlags = kIsBogus;
}

//----------------------------------------------------------------
// Implementation: Fundamental operators
//----------------------------------------------------------------

स्थिर अन्तःस्थ UChar32 max(UChar32 a, UChar32 b) {
    पटिनिवत्त (a > b) ? a : b;
}

// polarity = 0, 3 is normal: x xor y
// polarity = 1, 2: x xor ~y == x === y

शून्य UnicodeSet::exclusiveOr(ध्रुव UChar32* other, int32_t otherLen, int8_t polarity) {
    सचे (isFrozen() || isBogus()) {
        पटिनिवत्त;
    }
    सचे (!ensureBufferCapacity(len + otherLen)) {
        पटिनिवत्त;
    }

    int32_t i = 0, j = 0, k = 0;
    UChar32 a = list[i++];
    UChar32 b;
    सचे (polarity == 1 || polarity == 2) {
        b = UNICODESET_नीचः;
        सचे (other[j] == UNICODESET_नीचः) { // skip base if already LOW
            ++j;
            b = other[j];
        }
    } अञ्ञथा {
        b = other[j++];
    }
    // simplest of all the routines
    // sort the values, discarding identicals!
    अत्थाय (;;) {
        सचे (a < b) {
            buffer[k++] = a;
            a = list[i++];
        } अञ्ञथा सचे (b < a) {
            buffer[k++] = b;
            b = other[j++];
        } अञ्ञथा सचे (a != UNICODESET_उच्चः) { // at this point, a == b
            // discard both values!
            a = list[i++];
            b = other[j++];
        } अञ्ञथा { // DONE!
            buffer[k++] = UNICODESET_उच्चः;
            len = k;
            भङ्ग;
        }
    }
    swapBuffers();
    releasePattern();
}

// polarity = 0 is normal: x union y
// polarity = 2: x union ~y
// polarity = 1: ~x union y
// polarity = 3: ~x union ~y

शून्य UnicodeSet::add(ध्रुव UChar32* other, int32_t otherLen, int8_t polarity) {
    सचे (isFrozen() || isBogus() || other==nullptr) {
        पटिनिवत्त;
    }
    सचे (!ensureBufferCapacity(len + otherLen)) {
        पटिनिवत्त;
    }

    int32_t i = 0, j = 0, k = 0;
    UChar32 a = list[i++];
    UChar32 b = other[j++];
    // change from xor is that we have to check overlapping pairs
    // polarity bit 1 means a is second, bit 2 means b is.
    अत्थाय (;;) {
        विकल्प (polarity) {
          पक्ष 0: // both first; take lower if unequal
            सचे (a < b) { // take a
                // Back up over overlapping ranges in buffer[]
                सचे (k > 0 && a <= buffer[k-1]) {
                    // Pick latter end value in buffer[] vs. list[]
                    a = max(list[i], buffer[--k]);
                } अञ्ञथा {
                    // No overlap
                    buffer[k++] = a;
                    a = list[i];
                }
                i++; // Common if/else code factored out
                polarity ^= 1;
            } अञ्ञथा सचे (b < a) { // take b
                सचे (k > 0 && b <= buffer[k-1]) {
                    b = max(other[j], buffer[--k]);
                } अञ्ञथा {
                    buffer[k++] = b;
                    b = other[j];
                }
                j++;
                polarity ^= 2;
            } अञ्ञथा { // a == b, take a, drop b
                सचे (a == UNICODESET_उच्चः) गच्छ loop_end;
                // This is symmetrical; it doesn't matter if
                // we backtrack with a or b. - liu
                सचे (k > 0 && a <= buffer[k-1]) {
                    a = max(list[i], buffer[--k]);
                } अञ्ञथा {
                    // No overlap
                    buffer[k++] = a;
                    a = list[i];
                }
                i++;
                polarity ^= 1;
                b = other[j++];
                polarity ^= 2;
            }
            भङ्ग;
          पक्ष 3: // both second; take higher if unequal, and drop other
            सचे (b <= a) { // take a
                सचे (a == UNICODESET_उच्चः) गच्छ loop_end;
                buffer[k++] = a;
            } अञ्ञथा { // take b
                सचे (b == UNICODESET_उच्चः) गच्छ loop_end;
                buffer[k++] = b;
            }
            a = list[i++];
            polarity ^= 1;   // factored common code
            b = other[j++];
            polarity ^= 2;
            भङ्ग;
          पक्ष 1: // a second, b first; if b < a, overlap
            सचे (a < b) { // no overlap, take a
                buffer[k++] = a; a = list[i++]; polarity ^= 1;
            } अञ्ञथा सचे (b < a) { // OVERLAP, drop b
                b = other[j++];
                polarity ^= 2;
            } अञ्ञथा { // a == b, drop both!
                सचे (a == UNICODESET_उच्चः) गच्छ loop_end;
                a = list[i++];
                polarity ^= 1;
                b = other[j++];
                polarity ^= 2;
            }
            भङ्ग;
          पक्ष 2: // a first, b second; if a < b, overlap
            सचे (b < a) { // no overlap, take b
                buffer[k++] = b;
                b = other[j++];
                polarity ^= 2;
            } अञ्ञथा  सचे (a < b) { // OVERLAP, drop a
                a = list[i++];
                polarity ^= 1;
            } अञ्ञथा { // a == b, drop both!
                सचे (a == UNICODESET_उच्चः) गच्छ loop_end;
                a = list[i++];
                polarity ^= 1;
                b = other[j++];
                polarity ^= 2;
            }
            भङ्ग;
        }
    }
 loop_end:
    buffer[k++] = UNICODESET_उच्चः;    // terminate
    len = k;
    swapBuffers();
    releasePattern();
}

// polarity = 0 is normal: x intersect y
// polarity = 2: x intersect ~y == set-minus
// polarity = 1: ~x intersect y
// polarity = 3: ~x intersect ~y

शून्य UnicodeSet::retain(ध्रुव UChar32* other, int32_t otherLen, int8_t polarity) {
    सचे (isFrozen() || isBogus()) {
        पटिनिवत्त;
    }
    सचे (!ensureBufferCapacity(len + otherLen)) {
        पटिनिवत्त;
    }

    int32_t i = 0, j = 0, k = 0;
    UChar32 a = list[i++];
    UChar32 b = other[j++];
    // change from xor is that we have to check overlapping pairs
    // polarity bit 1 means a is second, bit 2 means b is.
    अत्थाय (;;) {
        विकल्प (polarity) {
          पक्ष 0: // both first; drop the smaller
            सचे (a < b) { // drop a
                a = list[i++];
                polarity ^= 1;
            } अञ्ञथा सचे (b < a) { // drop b
                b = other[j++];
                polarity ^= 2;
            } अञ्ञथा { // a == b, take one, drop other
                सचे (a == UNICODESET_उच्चः) गच्छ loop_end;
                buffer[k++] = a;
                a = list[i++];
                polarity ^= 1;
                b = other[j++];
                polarity ^= 2;
            }
            भङ्ग;
          पक्ष 3: // both second; take lower if unequal
            सचे (a < b) { // take a
                buffer[k++] = a;
                a = list[i++];
                polarity ^= 1;
            } अञ्ञथा सचे (b < a) { // take b
                buffer[k++] = b;
                b = other[j++];
                polarity ^= 2;
            } अञ्ञथा { // a == b, take one, drop other
                सचे (a == UNICODESET_उच्चः) गच्छ loop_end;
                buffer[k++] = a;
                a = list[i++];
                polarity ^= 1;
                b = other[j++];
                polarity ^= 2;
            }
            भङ्ग;
          पक्ष 1: // a second, b first;
            सचे (a < b) { // NO OVERLAP, drop a
                a = list[i++];
                polarity ^= 1;
            } अञ्ञथा सचे (b < a) { // OVERLAP, take b
                buffer[k++] = b;
                b = other[j++];
                polarity ^= 2;
            } अञ्ञथा { // a == b, drop both!
                सचे (a == UNICODESET_उच्चः) गच्छ loop_end;
                a = list[i++];
                polarity ^= 1;
                b = other[j++];
                polarity ^= 2;
            }
            भङ्ग;
          पक्ष 2: // a first, b second; if a < b, overlap
            सचे (b < a) { // no overlap, drop b
                b = other[j++];
                polarity ^= 2;
            } अञ्ञथा  सचे (a < b) { // OVERLAP, take a
                buffer[k++] = a;
                a = list[i++];
                polarity ^= 1;
            } अञ्ञथा { // a == b, drop both!
                सचे (a == UNICODESET_उच्चः) गच्छ loop_end;
                a = list[i++];
                polarity ^= 1;
                b = other[j++];
                polarity ^= 2;
            }
            भङ्ग;
        }
    }
 loop_end:
    buffer[k++] = UNICODESET_उच्चः;    // terminate
    len = k;
    swapBuffers();
    releasePattern();
}

/**
 * Append the <code>toPattern()</code> representation of a
 * string to the given <code>StringBuffer</code>.
 */
शून्य UnicodeSet::_appendToPat(UnicodeString& buf, ध्रुव UnicodeString& s, UBool escapeUnprintable) {
    UChar32 cp;
    अत्थाय (int32_t i = 0; i < s.length(); i += U16_LENGTH(cp)) {
        _appendToPat(buf, cp = s.char32At(i), escapeUnprintable);
    }
}

/**
 * Append the <code>toPattern()</code> representation of a
 * character to the given <code>StringBuffer</code>.
 */
शून्य UnicodeSet::_appendToPat(UnicodeString& buf, UChar32 c, UBool escapeUnprintable) {
    सचे (escapeUnprintable ? ICU_Utility::isUnprintable(c) : ICU_Utility::shouldAlwaysBeEscaped(c)) {
        // Use hex escape notation (\uxxxx or \Uxxxxxxxx) for anything
        // unprintable
        ICU_Utility::escape(buf, c);
        पटिनिवत्त;
    }
    // Okay to let ':' pass through
    विकल्प (c) {
    पक्ष u'[':
    पक्ष u']':
    पक्ष u'-':
    पक्ष u'^':
    पक्ष u'&':
    पक्ष u'\\':
    पक्ष u'{':
    पक्ष u'}':
    पक्ष u':':
    पक्ष चिह्नम्_Table::चिह्नम्_REF:
        buf.append(u'\\');
        भङ्ग;
    स्वभाव:
        // Escape whitespace
        सचे (PatternProps::isWhiteSpace(c)) {
            buf.append(u'\\');
        }
        भङ्ग;
    }
    buf.append(c);
}

शून्य UnicodeSet::_appendToPat(UnicodeString &result, UChar32 start, UChar32 end,
                              UBool escapeUnprintable) {
    _appendToPat(result, start, escapeUnprintable);
    सचे (start != end) {
        सचे ((start+1) != end ||
                // Avoid writing what looks like a lead+trail surrogate pair.
                start == 0xdbff) {
            result.append(u'-');
        }
        _appendToPat(result, end, escapeUnprintable);
    }
}

/**
 * Append a string representation of this set to result.  This will be
 * a cleaned version of the string passed to applyPattern(), if there
 * is one.  Otherwise it will be generated.
 */
UnicodeString& UnicodeSet::_toPattern(UnicodeString& result U_LIFETIME_BOUND,
                                      UBool escapeUnprintable) ध्रुव
{
    सचे (pat != nullptr) {
        int32_t i;
        int32_t backslash_गणनम् = 0;
        अत्थाय (i=0; i<patLen; ) {
            UChar32 c;
            U16_NEXT(pat, i, patLen, c);
            सचे (escapeUnprintable ?
                    ICU_Utility::isUnprintable(c) : ICU_Utility::shouldAlwaysBeEscaped(c)) {
                // If the unprintable character is preceded by an odd
                // number of backslashes, then it has been escaped.
                // Before unescaping it, we delete the final
                // backslash.
                सचे ((backslash_गणनम् % 2) == 1) {
                    result.truncate(result.length() - 1);
                }
                ICU_Utility::escape(result, c);
                backslash_गणनम् = 0;
            } अञ्ञथा {
                result.append(c);
                सचे (c == u'\\') {
                    ++backslash_गणनम्;
                } अञ्ञथा {
                    backslash_गणनम् = 0;
                }
            }
        }
        पटिनिवत्त result;
    }

    पटिनिवत्त _generatePattern(result, escapeUnprintable);
}

/**
 * Returns a string representation of this set.  If the result of
 * calling this function is passed to a UnicodeSet constructor, it
 * will produce another set that is equal to this one.
 */
UnicodeString& UnicodeSet::toPattern(UnicodeString& result U_LIFETIME_BOUND,
                                     UBool escapeUnprintable) ध्रुव
{
    result.truncate(0);
    पटिनिवत्त _toPattern(result, escapeUnprintable);
}

/**
 * Generate and append a string representation of this set to result.
 * This does not use this.pat, the cleaned up copy of the string
 * passed to applyPattern().
 */
UnicodeString& UnicodeSet::_generatePattern(UnicodeString& result U_LIFETIME_BOUND,
                                            UBool escapeUnprintable) ध्रुव
{
    result.append(u'[');

    int32_t i = 0;
    int32_t limit = len & ~1;  // = 2 * getRangeCount()

    // If the set contains at least 2 intervals and includes both
    // MIN_VALUE and MAX_VALUE, then the inverse representation will
    // be more economical.
    //     if (getRangeCount() >= 2 &&
    //             getRangeStart(0) == MIN_VALUE &&
    //             getRangeEnd(last) == MAX_VALUE)
    // Invariant: list[len-1] == HIGH == MAX_VALUE + 1
    // If limit == len then len is even and the last range ends with MAX_VALUE.
    //
    // *But* do not write the inverse (complement) if there are strings.
    // Since ICU 70, the '^' performs a code point complement which removes all strings.
    सचे (len >= 4 && list[0] == 0 && limit == len && !hasStrings()) {
        // Emit the inverse
        result.append(u'^');
        // Offsetting the inversion list index by one lets us
        // iterate over the ranges of the set complement.
        i = 1;
        --limit;
    }

    // Emit the ranges as pairs.
    याव (i < limit) {
        UChar32 start = list[i];  // getRangeStart()
        UChar32 end = list[i + 1] - 1;  // getRangeEnd() = range limit minus one
        सचे (!(0xd800 <= end && end <= 0xdbff)) {
            _appendToPat(result, start, end, escapeUnprintable);
            i += 2;
        } अञ्ञथा {
            // The range ends with a lead surrogate.
            // Avoid writing what looks like a lead+trail surrogate pair.
            // 1. Postpone ranges that start with a lead surrogate code point.
            int32_t firstLead = i;
            याव ((i += 2) < limit && list[i] <= 0xdbff) {}
            int32_t firstAfterLead = i;
            // 2. Write following ranges that start with a trail surrogate code point.
            याव (i < limit && (start = list[i]) <= 0xdfff) {
                _appendToPat(result, start, list[i + 1] - 1, escapeUnprintable);
                i += 2;
            }
            // 3. Now write the postponed ranges.
            अत्थाय (पूर्णाङ्क j = firstLead; j < firstAfterLead; j += 2) {
                _appendToPat(result, list[j], list[j + 1] - 1, escapeUnprintable);
            }
        }
    }

    सचे (strings_ != nullptr) {
        अत्थाय (int32_t i = 0; i<strings_->size(); ++i) {
            result.append(u'{');
            _appendToPat(result,
                         *static_cast<ध्रुव UnicodeString*>(strings_->elementAt(i)),
                         escapeUnprintable);
            result.append(u'}');
        }
    }
    पटिनिवत्त result.append(u']');
}

/**
* Release existing cached pattern
*/
शून्य UnicodeSet::releasePattern() {
    सचे (pat) {
        uprv_free(pat);
        pat = nullptr;
        patLen = 0;
    }
}

/**
* Set the new pattern to cache.
*/
शून्य UnicodeSet::setPattern(ध्रुव char16_t *newPat, int32_t newPatLen) {
    releasePattern();
    pat = static_cast<char16_t*>(uprv_malloc((newPatLen + 1) * परिमाण(char16_t)));
    सचे (pat) {
        patLen = newPatLen;
        u_memcpy(pat, newPat, patLen);
        pat[patLen] = 0;
    }
    // else we don't care if malloc failed. This was just a nice cache.
    // We can regenerate an equivalent pattern later when requested.
}

UnicodeSet* UnicodeSet::freeze() U_LIFETIME_BOUND {
    सचे(!isFrozen() && !isBogus()) {
        compact();

        // Optimize contains() and span() and similar functions.
        सचे (hasStrings()) {
            stringSpan = नूतन UnicodeSetStringSpan(*एतत्, *strings_, UnicodeSetStringSpan::ALL);
            सचे (stringSpan == nullptr) {
                setToBogus();
                पटिनिवत्त एतत्;
            } अञ्ञथा सचे (!stringSpan->needsStringSpanUTF16()) {
                // All strings are irrelevant for span() etc. because
                // all of each string's code points are contained in this set.
                // Do not check needsStringSpanUTF8() because UTF-8 has at most as
                // many relevant strings as UTF-16.
                // (Thus needsStringSpanUTF8() implies needsStringSpanUTF16().)
                delete stringSpan;
                stringSpan = nullptr;
            }
        }
        सचे (stringSpan == nullptr) {
            // No span-relevant strings: Optimize for code point spans.
            bmpSet=नूतन BMPSet(list, len);
            सचे (bmpSet == nullptr) { // Check for memory allocation error.
                setToBogus();
            }
        }
    }
    पटिनिवत्त एतत्;
}

int32_t UnicodeSet::span(ध्रुव char16_t *s, int32_t length, USetSpanCondition spanCondition) ध्रुव {
    सचे(length>0 && bmpSet!=nullptr) {
        पटिनिवत्त static_cast<int32_t>(bmpSet->span(s, s + length, spanCondition) - s);
    }
    सचे(length<0) {
        length=u_strlen(s);
    }
    सचे(length==0) {
        पटिनिवत्त 0;
    }
    सचे(stringSpan!=nullptr) {
        पटिनिवत्त stringSpan->span(s, length, spanCondition);
    } अञ्ञथा सचे(hasStrings()) {
        uint32_t which= spanCondition==USET_SPAN_NOT_CONTAINED ?
                            UnicodeSetStringSpan::FWD_UTF16_NOT_CONTAINED :
                            UnicodeSetStringSpan::FWD_UTF16_CONTAINED;
        UnicodeSetStringSpan strSpan(*एतत्, *strings_, which);
        सचे(strSpan.needsStringSpanUTF16()) {
            पटिनिवत्त strSpan.span(s, length, spanCondition);
        }
    }

    सचे(spanCondition!=USET_SPAN_NOT_CONTAINED) {
        spanCondition=USET_SPAN_CONTAINED;  // Pin to 0/1 values.
    }

    UChar32 c;
    int32_t start=0, prev=0;
    करोहि {
        U16_NEXT(s, start, length, c);
        सचे(spanCondition!=contains(c)) {
            भङ्ग;
        }
    } याव((prev=start)<length);
    पटिनिवत्त prev;
}

int32_t UnicodeSet::spanBack(ध्रुव char16_t *s, int32_t length, USetSpanCondition spanCondition) ध्रुव {
    सचे(length>0 && bmpSet!=nullptr) {
        पटिनिवत्त static_cast<int32_t>(bmpSet->spanBack(s, s + length, spanCondition) - s);
    }
    सचे(length<0) {
        length=u_strlen(s);
    }
    सचे(length==0) {
        पटिनिवत्त 0;
    }
    सचे(stringSpan!=nullptr) {
        पटिनिवत्त stringSpan->spanBack(s, length, spanCondition);
    } अञ्ञथा सचे(hasStrings()) {
        uint32_t which= spanCondition==USET_SPAN_NOT_CONTAINED ?
                            UnicodeSetStringSpan::BACK_UTF16_NOT_CONTAINED :
                            UnicodeSetStringSpan::BACK_UTF16_CONTAINED;
        UnicodeSetStringSpan strSpan(*एतत्, *strings_, which);
        सचे(strSpan.needsStringSpanUTF16()) {
            पटिनिवत्त strSpan.spanBack(s, length, spanCondition);
        }
    }

    सचे(spanCondition!=USET_SPAN_NOT_CONTAINED) {
        spanCondition=USET_SPAN_CONTAINED;  // Pin to 0/1 values.
    }

    UChar32 c;
    int32_t prev=length;
    करोहि {
        U16_PREV(s, 0, length, c);
        सचे(spanCondition!=contains(c)) {
            भङ्ग;
        }
    } याव((prev=length)>0);
    पटिनिवत्त prev;
}

int32_t UnicodeSet::spanUTF8(ध्रुव वर्ण *s, int32_t length, USetSpanCondition spanCondition) ध्रुव {
    सचे(length>0 && bmpSet!=nullptr) {
        ध्रुव uint8_t* s0 = reinterpret_cast<ध्रुव uint8_t*>(s);
        पटिनिवत्त static_cast<int32_t>(bmpSet->spanUTF8(s0, length, spanCondition) - s0);
    }
    सचे(length<0) {
        length = static_cast<int32_t>(uprv_strlen(s));
    }
    सचे(length==0) {
        पटिनिवत्त 0;
    }
    सचे(stringSpan!=nullptr) {
        पटिनिवत्त stringSpan->spanUTF8(reinterpret_cast<ध्रुव uint8_t*>(s), length, spanCondition);
    } अञ्ञथा सचे(hasStrings()) {
        uint32_t which= spanCondition==USET_SPAN_NOT_CONTAINED ?
                            UnicodeSetStringSpan::FWD_UTF8_NOT_CONTAINED :
                            UnicodeSetStringSpan::FWD_UTF8_CONTAINED;
        UnicodeSetStringSpan strSpan(*एतत्, *strings_, which);
        सचे(strSpan.needsStringSpanUTF8()) {
            पटिनिवत्त strSpan.spanUTF8(reinterpret_cast<ध्रुव uint8_t*>(s), length, spanCondition);
        }
    }

    सचे(spanCondition!=USET_SPAN_NOT_CONTAINED) {
        spanCondition=USET_SPAN_CONTAINED;  // Pin to 0/1 values.
    }

    UChar32 c;
    int32_t start=0, prev=0;
    करोहि {
        U8_NEXT_OR_FFFD(s, start, length, c);
        सचे(spanCondition!=contains(c)) {
            भङ्ग;
        }
    } याव((prev=start)<length);
    पटिनिवत्त prev;
}

int32_t UnicodeSet::spanBackUTF8(ध्रुव वर्ण *s, int32_t length, USetSpanCondition spanCondition) ध्रुव {
    सचे(length>0 && bmpSet!=nullptr) {
        ध्रुव uint8_t* s0 = reinterpret_cast<ध्रुव uint8_t*>(s);
        पटिनिवत्त bmpSet->spanBackUTF8(s0, length, spanCondition);
    }
    सचे(length<0) {
        length = static_cast<int32_t>(uprv_strlen(s));
    }
    सचे(length==0) {
        पटिनिवत्त 0;
    }
    सचे(stringSpan!=nullptr) {
        पटिनिवत्त stringSpan->spanBackUTF8(reinterpret_cast<ध्रुव uint8_t*>(s), length, spanCondition);
    } अञ्ञथा सचे(hasStrings()) {
        uint32_t which= spanCondition==USET_SPAN_NOT_CONTAINED ?
                            UnicodeSetStringSpan::BACK_UTF8_NOT_CONTAINED :
                            UnicodeSetStringSpan::BACK_UTF8_CONTAINED;
        UnicodeSetStringSpan strSpan(*एतत्, *strings_, which);
        सचे(strSpan.needsStringSpanUTF8()) {
            पटिनिवत्त strSpan.spanBackUTF8(reinterpret_cast<ध्रुव uint8_t*>(s), length, spanCondition);
        }
    }

    सचे(spanCondition!=USET_SPAN_NOT_CONTAINED) {
        spanCondition=USET_SPAN_CONTAINED;  // Pin to 0/1 values.
    }

    UChar32 c;
    int32_t prev=length;
    करोहि {
        U8_PREV_OR_FFFD(s, 0, length, c);
        सचे(spanCondition!=contains(c)) {
            भङ्ग;
        }
    } याव((prev=length)>0);
    पटिनिवत्त prev;
}

U_NAMESPACE_END
