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

#அந்தர்பாவ "unicode/utypes.h"
#அந்தர்பாவ "unicode/parsepos.h"
#அந்தர்பாவ "unicode/symtable.h"
#அந்தர்பாவ "unicode/uniset.h"
#அந்தர்பாவ "unicode/ustring.h"
#அந்தர்பாவ "unicode/utf8.h"
#அந்தர்பாவ "unicode/utf16.h"
#அந்தர்பாவ "ruleiter.h"
#அந்தர்பாவ "cmemory.h"
#அந்தர்பாவ "cstring.h"
#அந்தர்பாவ "patternprops.h"
#அந்தர்பாவ "uelement.h"
#அந்தர்பாவ "util.h"
#அந்தர்பாவ "uvector.h"
#அந்தர்பாவ "charstr.h"
#அந்தர்பாவ "ustrfmt.h"
#அந்தர்பாவ "uassert.h"
#அந்தர்பாவ "bmpset.h"
#அந்தர்பாவ "unisetspan.h"

// HIGH_VALUE > all valid values. 110000 for codepoints
#define UNICODESET_உயர்வு 0x0110000

// LOW <= all valid values. ZERO for codepoints
#define UNICODESET_கீழ் 0x000000

/** Max list [0, 1, 2, ..., max code point, HIGH] */
constexpr int32_t MAX_LENGTH = UNICODESET_உயர்வு + 1;

U_NAMESPACE_BEGIN

குறியீடு_Table::~குறியீடு_Table() {}

UOBJECT_DEFINE_RTTI_IMPLEMENTATION(UnicodeSet)

/**
 * Modify the given UChar32 variable so that it is in range, by
 * pinning values < UNICODESET_LOW to UNICODESET_LOW, and
 * pinning values > UNICODESET_HIGH-1 to UNICODESET_HIGH-1.
 * It modifies its argument in-place and also returns it.
 */
நிலையான உள்வரி UChar32 pinCodePoint(UChar32& c) {
    எனில் (c < UNICODESET_கீழ்) {
        c = UNICODESET_கீழ்;
    } இல்லையெனில் எனில் (c > (UNICODESET_உயர்வு-1)) {
        c = (UNICODESET_உயர்வு-1);
    }
    திரும்பு c;
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
#அந்தர்பாவ <stdio.h>
நிலையான int32_t dbg_மொத்தம் = 0;

நிலையான உள்வரி வெறுமை _dbgct(UnicodeSet* set) {
    UnicodeString str;
    set->toPattern(str, உண்மை);
    எழுத்து buf[40];
    str.extract(0, 39, buf, "");
    printf("DEBUG UnicodeSet: ct 0x%08X; %d %s\n", set, ++dbg_மொத்தம், buf);
}

நிலையான உள்வரி வெறுமை _dbgdt(UnicodeSet* set) {
    UnicodeString str;
    set->toPattern(str, உண்மை);
    எழுத்து buf[40];
    str.extract(0, 39, buf, "");
    printf("DEBUG UnicodeSet: dt 0x%08X; %d %s\n", set, --dbg_மொத்தம், buf);
}

#இல்லையெனில்

#define _dbgct(set)
#define _dbgdt(set)

#endif

//----------------------------------------------------------------
// UnicodeString in UVector support
//----------------------------------------------------------------

நிலையான வெறுமை U_CALLCONV cloneUnicodeString(UElement *dst, UElement *src) {
    dst->pointer = புதிய UnicodeString(*static_cast<UnicodeString*>(src->pointer));
}

நிலையான int32_t U_CALLCONV compareUnicodeString(UElement t1, UElement t2) {
    மாறிலி UnicodeString& a = *static_cast<மாறிலி UnicodeString*>(t1.pointer);
    மாறிலி UnicodeString& b = *static_cast<மாறிலி UnicodeString*>(t2.pointer);
    திரும்பு a.compare(b);
}

UBool UnicodeSet::hasStrings() மாறிலி {
    திரும்பு strings_ != nullptr && !strings_->isEmpty();
}

int32_t UnicodeSet::stringsSize() மாறிலி {
    திரும்பு strings_ == nullptr ? 0 : strings_->size();
}

UBool UnicodeSet::stringsContains(மாறிலி UnicodeString &s) மாறிலி {
    திரும்பு strings_ != nullptr && strings_->contains((வெறுமை*) &s);
}

//----------------------------------------------------------------
// Constructors &c
//----------------------------------------------------------------

/**
 * Constructs an empty set.
 */
UnicodeSet::UnicodeSet() {
    list[0] = UNICODESET_உயர்வு;
    _dbgct(இது);
}

/**
 * Constructs a set containing the given range. If <code>end >
 * start</code> then an empty set is created.
 *
 * @param start first character, inclusive, of range
 * @param end last character, inclusive, of range
 */
UnicodeSet::UnicodeSet(UChar32 start, UChar32 end) {
    list[0] = UNICODESET_உயர்வு;
    add(start, end);
    _dbgct(இது);
}

/**
 * Constructs a set that is identical to the given UnicodeSet.
 */
UnicodeSet::UnicodeSet(மாறிலி UnicodeSet& o) : UnicodeFilter(o) {
    *இது = o;
    _dbgct(இது);
}

// Copy-construct as thawed.
UnicodeSet::UnicodeSet(மாறிலி UnicodeSet& o, UBool /* asThawed */) : UnicodeFilter(o) {
    எனில் (ensureCapacity(o.len)) {
        // *this = o except for bmpSet and stringSpan
        len = o.len;
        uprv_memcpy(list, o.list, (size_t)len*அளவு(UChar32));
        எனில் (o.hasStrings()) {
            UErrorCode status = U_ZERO_ERROR;
            எனில் (!allocateStrings(status) ||
                    (strings_->assign(*o.strings_, cloneUnicodeString, status), U_FAILURE(status))) {
                setToBogus();
                திரும்பு;
            }
        }
        எனில் (o.pat) {
            setPattern(o.pat, o.patLen);
        }
        _dbgct(இது);
    }
}

/**
 * Destructs the set.
 */
UnicodeSet::~UnicodeSet() {
    _dbgdt(இது); // first!
    எனில் (list != அடுக்கு_List) {
        uprv_free(list);
    }
    delete bmpSet;
    எனில் (buffer != அடுக்கு_List) {
        uprv_free(buffer);
    }
    delete strings_;
    delete stringSpan;
    releasePattern();
}

/**
 * Assigns this object to be a copy of another.
 */
UnicodeSet& UnicodeSet::operator=(மாறிலி UnicodeSet& o) U_LIFETIME_BOUND {
    திரும்பு copyFrom(o, பொய்);
}

UnicodeSet& UnicodeSet::copyFrom(மாறிலி UnicodeSet& o, UBool asThawed) U_LIFETIME_BOUND {
    எனில் (இது == &o) {
        திரும்பு *இது;
    }
    எனில் (isFrozen()) {
        திரும்பு *இது;
    }
    எனில் (o.isBogus()) {
        setToBogus();
        திரும்பு *இது;
    }
    எனில் (!ensureCapacity(o.len)) {
        // ensureCapacity will mark the UnicodeSet as Bogus if OOM failure happens.
        திரும்பு *இது;
    }
    len = o.len;
    uprv_memcpy(list, o.list, (size_t)len*அளவு(UChar32));
    எனில் (o.bmpSet != nullptr && !asThawed) {
        bmpSet = புதிய BMPSet(*o.bmpSet, list, len);
        எனில் (bmpSet == nullptr) { // Check for memory allocation error.
            setToBogus();
            திரும்பு *இது;
        }
    }
    எனில் (o.hasStrings()) {
        UErrorCode status = U_ZERO_ERROR;
        எனில் ((strings_ == nullptr && !allocateStrings(status)) ||
                (strings_->assign(*o.strings_, cloneUnicodeString, status), U_FAILURE(status))) {
            setToBogus();
            திரும்பு *இது;
        }
    } இல்லையெனில் எனில் (hasStrings()) {
        strings_->removeAllElements();
    }
    எனில் (o.stringSpan != nullptr && strings_ != nullptr && !asThawed) {
        stringSpan = புதிய UnicodeSetStringSpan(*o.stringSpan, *strings_);
        எனில் (stringSpan == nullptr) { // Check for memory allocation error.
            setToBogus();
            திரும்பு *இது;
        }
    }
    releasePattern();
    எனில் (o.pat) {
        setPattern(o.pat, o.patLen);
    }
    திரும்பு *இது;
}

/**
 * Returns a copy of this object.  All UnicodeMatcher objects have
 * to support cloning in order to allow classes using
 * UnicodeMatchers, such as Transliterator, to implement cloning.
 */
UnicodeSet* UnicodeSet::clone() மாறிலி {
    திரும்பு புதிய UnicodeSet(*இது);
}

UnicodeSet *UnicodeSet::cloneAsThawed() மாறிலி {
    திரும்பு புதிய UnicodeSet(*இது, உண்மை);
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
தருக்க UnicodeSet::operator==(மாறிலி UnicodeSet& o) மாறிலி {
    எனில் (len != o.len) திரும்பு பொய்;
    க்கு (int32_t i = 0; i < len; ++i) {
        எனில் (list[i] != o.list[i]) திரும்பு பொய்;
    }
    எனில் (hasStrings() != o.hasStrings()) { திரும்பு பொய்; }
    எனில் (hasStrings() && *strings_ != *o.strings_) திரும்பு பொய்;
    திரும்பு உண்மை;
}

/**
 * Returns the hash code value for this set.
 *
 * @return the hash code value for this set.
 * @see Object#hashCode()
 */
int32_t UnicodeSet::hashCode() மாறிலி {
    uint32_t result = static_cast<uint32_t>(len);
    க்கு (int32_t i = 0; i < len; ++i) {
        result *= 1000003u;
        result += list[i];
    }
    திரும்பு static_cast<int32_t>(result);
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
int32_t UnicodeSet::size() மாறிலி {
    int32_t n = 0;
    int32_t count = get_Range_மொத்தம்();
    க்கு (int32_t i = 0; i < count; ++i) {
        n += getRangeEnd(i) - getRangeStart(i) + 1;
    }
    திரும்பு n + stringsSize();
}

/**
 * Returns <tt>true</tt> if this set contains no elements.
 *
 * @return <tt>true</tt> if this set contains no elements.
 */
UBool UnicodeSet::isEmpty() மாறிலி {
    திரும்பு len == 1 && !hasStrings();
}

/**
 * Returns true if this set contains the given character.
 * @param c character to be checked for containment
 * @return true if the test condition is met
 */
UBool UnicodeSet::contains(UChar32 c) மாறிலி {
    // Set i to the index of the start item greater than ch
    // We know we will terminate without length test!
    // LATER: for large sets, add binary search
    //int32_t i = -1;
    //for (;;) {
    //    if (c < list[++i]) break;
    //}
    எனில் (bmpSet != nullptr) {
        திரும்பு bmpSet->contains(c);
    }
    எனில் (stringSpan != nullptr) {
        திரும்பு stringSpan->contains(c);
    }
    எனில் (c >= UNICODESET_உயர்வு) { // Don't need to check LOW bound
        திரும்பு பொய்;
    }
    int32_t i = findCodePoint(c);
    திரும்பு i & 1; // return true if odd
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
int32_t UnicodeSet::findCodePoint(UChar32 c) மாறிலி {
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
    எனில் (c < list[0])
        திரும்பு 0;
    // High runner test.  c is often after the last range, so an
    // initial check for this condition pays off.
    int32_t lo = 0;
    int32_t hi = len - 1;
    எனில் (lo >= hi || c >= list[hi-1])
        திரும்பு hi;
    // invariant: c >= list[lo]
    // invariant: c < list[hi]
    க்கு (;;) {
        int32_t i = (lo + hi) >> 1;
        எனில் (i == lo) {
            நிறுத்து; // Found!
        } இல்லையெனில் எனில் (c < list[i]) {
            hi = i;
        } இல்லையெனில் {
            lo = i;
        }
    }
    திரும்பு hi;
}

/**
 * Returns true if this set contains every character
 * of the given range.
 * @param start first character, inclusive, of the range
 * @param end last character, inclusive, of the range
 * @return true if the test condition is met
 */
UBool UnicodeSet::contains(UChar32 start, UChar32 end) மாறிலி {
    //int32_t i = -1;
    //for (;;) {
    //    if (start < list[++i]) break;
    //}
    int32_t i = findCodePoint(start);
    திரும்பு ((i & 1) != 0 && end < list[i]);
}

/**
 * Returns <tt>true</tt> if this set contains the given
 * multicharacter string.
 * @param s string to be checked for containment
 * @return <tt>true</tt> if this set contains the specified string
 */
UBool UnicodeSet::contains(மாறிலி UnicodeString& s) மாறிலி {
    int32_t cp = getSingleCP(s);
    எனில் (cp < 0) {
        திரும்பு stringsContains(s);
    } இல்லையெனில் {
        திரும்பு contains(static_cast<UChar32>(cp));
    }
}

/**
 * Returns true if this set contains all the characters and strings
 * of the given set.
 * @param c set to be checked for containment
 * @return true if the test condition is met
 */
UBool UnicodeSet::containsAll(மாறிலி UnicodeSet& c) மாறிலி {
    // The specified set is a subset if all of its pairs are contained in
    // this set.  It's possible to code this more efficiently in terms of
    // direct manipulation of the inversion lists if the need arises.
    int32_t n = c.get_Range_மொத்தம்();
    க்கு (முழு i=0; i<n; ++i) {
        எனில் (!contains(c.getRangeStart(i), c.getRangeEnd(i))) {
            திரும்பு பொய்;
        }
    }
    திரும்பு !c.hasStrings() || (strings_ != nullptr && strings_->containsAll(*c.strings_));
}

/**
 * Returns true if this set contains all the characters
 * of the given string.
 * @param s string containing characters to be checked for containment
 * @return true if the test condition is met
 */
UBool UnicodeSet::containsAll(மாறிலி UnicodeString& s) மாறிலி {
    திரும்பு span(s.getBuffer(), s.length(), USET_SPAN_CONTAINED) == s.length();
}

/**
 * Returns true if this set contains none of the characters
 * of the given range.
 * @param start first character, inclusive, of the range
 * @param end last character, inclusive, of the range
 * @return true if the test condition is met
 */
UBool UnicodeSet::containsNone(UChar32 start, UChar32 end) மாறிலி {
    //int32_t i = -1;
    //for (;;) {
    //    if (start < list[++i]) break;
    //}
    int32_t i = findCodePoint(start);
    திரும்பு ((i & 1) == 0 && end < list[i]);
}

/**
 * Returns true if this set contains none of the characters and strings
 * of the given set.
 * @param c set to be checked for containment
 * @return true if the test condition is met
 */
UBool UnicodeSet::containsNone(மாறிலி UnicodeSet& c) மாறிலி {
    // The specified set is a subset if all of its pairs are contained in
    // this set.  It's possible to code this more efficiently in terms of
    // direct manipulation of the inversion lists if the need arises.
    int32_t n = c.get_Range_மொத்தம்();
    க்கு (int32_t i=0; i<n; ++i) {
        எனில் (!containsNone(c.getRangeStart(i), c.getRangeEnd(i))) {
            திரும்பு பொய்;
        }
    }
    திரும்பு strings_ == nullptr || !c.hasStrings() || strings_->containsNone(*c.strings_);
}

/**
 * Returns true if this set contains none of the characters
 * of the given string.
 * @param s string containing characters to be checked for containment
 * @return true if the test condition is met
 */
UBool UnicodeSet::containsNone(மாறிலி UnicodeString& s) மாறிலி {
    திரும்பு span(s.getBuffer(), s.length(), USET_SPAN_NOT_CONTAINED) == s.length();
}

/**
 * Returns <tt>true</tt> if this set contains any character whose low byte
 * is the given value.  This is used by <tt>RuleBasedTransliterator</tt> for
 * indexing.
 */
UBool UnicodeSet::matches_Index_மதிப்பு(uint8_t v) மாறிலி {
    /* The index value v, in the range [0,255], is contained in this set if
     * it is contained in any pair of this set.  Pairs either have the high
     * bytes equal, or unequal.  If the high bytes are equal, then we have
     * aaxx..aayy, where aa is the high byte.  Then v is contained if xx <=
     * v <= yy.  If the high bytes are unequal we have aaxx..bbyy, bb>aa.
     * Then v is contained if xx <= v || v <= yy.  (This is identical to the
     * time zone month containment logic.)
     */
    int32_t i;
    int32_t range_மொத்தம்=get_Range_மொத்தம்();
    க்கு (i=0; i<range_மொத்தம்; ++i) {
        UChar32 கீழ் = getRangeStart(i);
        UChar32 உயர்வு = getRangeEnd(i);
        எனில் ((கீழ் & ~0xFF) == (உயர்வு & ~0xFF)) {
            எனில் ((கீழ் & 0xFF) <= v && v <= (உயர்வு & 0xFF)) {
                திரும்பு உண்மை;
            }
        } இல்லையெனில் எனில் ((கீழ் & 0xFF) <= v || v <= (உயர்வு & 0xFF)) {
            திரும்பு உண்மை;
        }
    }
    எனில் (hasStrings()) {
        க்கு (i=0; i<strings_->size(); ++i) {
            மாறிலி UnicodeString& s = *static_cast<மாறிலி UnicodeString*>(strings_->elementAt(i));
            எனில் (s.isEmpty()) {
                தொடர்;  // skip the empty string
            }
            UChar32 c = s.char32At(0);
            எனில் ((c & 0xFF) == v) {
                திரும்பு உண்மை;
            }
        }
    }
    திரும்பு பொய்;
}

/**
 * Implementation of UnicodeMatcher::matches().  Always matches the
 * longest possible multichar string.
 */
UMatchDegree UnicodeSet::matches(மாறிலி Replaceable& text,
                                 int32_t& offset,
                                 int32_t limit,
                                 UBool incremental) {
    எனில் (offset == limit) {
        எனில் (contains(U_ETHER)) {
            திரும்பு incremental ? U_PARTIAL_MATCH : U_MATCH;
        } இல்லையெனில் {
            திரும்பு U_MISMATCH;
        }
    } இல்லையெனில் {
        எனில் (hasStrings()) { // try strings first

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
            int32_t உயர்வு_Water_Length = 0;

            க்கு (i=0; i<strings_->size(); ++i) {
                மாறிலி UnicodeString& trial = *static_cast<மாறிலி UnicodeString*>(strings_->elementAt(i));
                எனில் (trial.isEmpty()) {
                    தொடர்;  // skip the empty string
                }

                char16_t c = trial.charAt(forward ? 0 : trial.length() - 1);

                // Strings are sorted, so we can optimize in the
                // forward direction.
                எனில் (forward && c > firstChar) நிறுத்து;
                எனில் (c != firstChar) தொடர்;

                int32_t matchLen = matchRest(text, offset, limit, trial);

                எனில் (incremental) {
                    int32_t maxLen = forward ? limit-offset : offset-limit;
                    எனில் (matchLen == maxLen) {
                        // We have successfully matched but only up to limit.
                        திரும்பு U_PARTIAL_MATCH;
                    }
                }

                எனில் (matchLen == trial.length()) {
                    // We have successfully matched the whole string.
                    எனில் (matchLen > உயர்வு_Water_Length) {
                        உயர்வு_Water_Length = matchLen;
                    }
                    // In the forward direction we know strings
                    // are sorted so we can bail early.
                    எனில் (forward && matchLen < உயர்வு_Water_Length) {
                        நிறுத்து;
                    }
                    தொடர்;
                }
            }

            // We've checked all strings without a partial match.
            // If we have full matches, return the longest one.
            எனில் (உயர்வு_Water_Length != 0) {
                offset += forward ? உயர்வு_Water_Length : -உயர்வு_Water_Length;
                திரும்பு U_MATCH;
            }
        }
        திரும்பு UnicodeFilter::matches(text, offset, limit, incremental);
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
int32_t UnicodeSet::matchRest(மாறிலி Replaceable& text,
                              int32_t start, int32_t limit,
                              மாறிலி UnicodeString& s) {
    int32_t i;
    int32_t maxLen;
    int32_t slen = s.length();
    எனில் (start < limit) {
        maxLen = limit - start;
        எனில் (maxLen > slen) maxLen = slen;
        க்கு (i = 1; i < maxLen; ++i) {
            எனில் (text.charAt(start + i) != s.charAt(i)) திரும்பு 0;
        }
    } இல்லையெனில் {
        maxLen = start - limit;
        எனில் (maxLen > slen) maxLen = slen;
        --slen; // <=> slen = s.length() - 1;
        க்கு (i = 1; i < maxLen; ++i) {
            எனில் (text.charAt(start - i) != s.charAt(slen - i)) திரும்பு 0;
        }
    }
    திரும்பு maxLen;
}

/**
 * Implement of UnicodeMatcher
 */
வெறுமை UnicodeSet::addMatchSetTo(UnicodeSet& toUnionTo) மாறிலி {
    toUnionTo.addAll(*இது);
}

/**
 * Returns the index of the given character within this set, where
 * the set is ordered by ascending code point.  If the character
 * is not in this set, return -1.  The inverse of this method is
 * <code>charAt()</code>.
 * @return an index from 0..size()-1, or -1
 */
int32_t UnicodeSet::indexOf(UChar32 c) மாறிலி {
    எனில் (c < MIN_மதிப்பு || c > MAX_மதிப்பு) {
        திரும்பு -1;
    }
    int32_t i = 0;
    int32_t n = 0;
    க்கு (;;) {
        UChar32 start = list[i++];
        எனில் (c < start) {
            திரும்பு -1;
        }
        UChar32 limit = list[i++];
        எனில் (c < limit) {
            திரும்பு n + c - start;
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
UChar32 UnicodeSet::charAt(int32_t index) மாறிலி {
    எனில் (index >= 0) {
        // len2 is the largest even integer <= len, that is, it is len
        // for even values and len-1 for odd values.  With odd values
        // the last entry is UNICODESET_HIGH.
        int32_t len2 = len & ~1;
        க்கு (int32_t i=0; i < len2;) {
            UChar32 start = list[i++];
            int32_t count = list[i++] - start;
            எனில் (index < count) {
                திரும்பு static_cast<UChar32>(start + index);
            }
            index -= count;
        }
    }
    திரும்பு static_cast<UChar32>(-1);
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
    திரும்பு *இது;
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
    எனில் (pinCodePoint(start) < pinCodePoint(end)) {
        UChar32 limit = end + 1;
        // Fast path for adding a new range after the last one.
        // Odd list length: [..., lastStart, lastLimit, HIGH]
        எனில் ((len & 1) != 0) {
            // If the list is empty, set lastLimit low enough to not be adjacent to 0.
            UChar32 lastLimit = len == 1 ? -2 : list[len - 2];
            எனில் (lastLimit <= start && !isFrozen() && !isBogus()) {
                எனில் (lastLimit == start) {
                    // Extend the last range.
                    list[len - 2] = limit;
                    எனில் (limit == UNICODESET_உயர்வு) {
                        --len;
                    }
                } இல்லையெனில் {
                    list[len - 1] = start;
                    எனில் (limit < UNICODESET_உயர்வு) {
                        எனில் (ensureCapacity(len + 2)) {
                            list[len++] = limit;
                            list[len++] = UNICODESET_உயர்வு;
                        }
                    } இல்லையெனில் {  // limit == UNICODESET_HIGH
                        எனில் (ensureCapacity(len + 1)) {
                            list[len++] = UNICODESET_உயர்வு;
                        }
                    }
                }
                releasePattern();
                திரும்பு *இது;
            }
        }
        // This is slow. Could be much faster using findCodePoint(start)
        // and modifying the list, dealing with adjacent & overlapping ranges.
        UChar32 range[3] = { start, limit, UNICODESET_உயர்வு };
        add(range, 2, 0);
    } இல்லையெனில் எனில் (start == end) {
        add(start);
    }
    திரும்பு *இது;
}

// #define DEBUG_US_ADD

#ifdef DEBUG_US_ADD
#அந்தர்பாவ <stdio.h>
வெறுமை dump(UChar32 c) {
    எனில் (c <= 0xFF) {
        printf("%c", (எழுத்து)c);
    } இல்லையெனில் {
        printf("U+%04X", c);
    }
}
வெறுமை dump(மாறிலி UChar32* list, int32_t len) {
    printf("[");
    க்கு (int32_t i=0; i<len; ++i) {
        எனில் (i != 0) printf(", ");
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
    எனில் ((i & 1) != 0  || isFrozen() || isBogus()) திரும்பு *இது;

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

    எனில் (c == list[i]-1) {
        // c is before start of next range
        list[i] = c;
        // if we touched the HIGH mark, then add a new one
        எனில் (c == (UNICODESET_உயர்வு - 1)) {
            எனில் (!ensureCapacity(len+1)) {
                // ensureCapacity will mark the object as Bogus if OOM failure happens.
                திரும்பு *இது;
            }
            list[len++] = UNICODESET_உயர்வு;
        }
        எனில் (i > 0 && c == list[i-1]) {
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
            போது (src < srclimit) *(dst++) = *(src++);

            len -= 2;
        }
    }

    இல்லையெனில் எனில் (i > 0 && c == list[i-1]) {
        // c is after end of prior range
        list[i-1]++;
        // no need to check for collapse here
    }

    இல்லையெனில் {
        // At this point we know the new char is not adjacent to
        // any existing ranges, and it is not 10FFFF.


        // [..., start_k-1, limit_k-1, start_k, limit_k, ..., HIGH]
        //                             ^
        //                             list[i]

        // [..., start_k-1, limit_k-1, c, c+1, start_k, limit_k, ..., HIGH]
        //                             ^
        //                             list[i]

        எனில் (!ensureCapacity(len+2)) {
            // ensureCapacity will mark the object as Bogus if OOM failure happens.
            திரும்பு *இது;
        }

        UChar32 *p = list + i;
        uprv_memmove(p + 2, p, (len - i) * அளவு(*p));
        list[i] = c;
        list[i+1] = c+1;
        len += 2;
    }

#ifdef DEBUG_US_ADD
    dump(list, len);
    printf("\n");

    க்கு (i=1; i<len; ++i) {
        எனில் (list[i] <= list[i-1]) {
            // Corrupt array!
            printf("ERROR: list has been corrupted\n");
            exit(1);
        }
    }
#endif

    releasePattern();
    திரும்பு *இது;
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
UnicodeSet& UnicodeSet::add(மாறிலி UnicodeString& s) U_LIFETIME_BOUND {
    எனில் (isFrozen() || isBogus()) திரும்பு *இது;
    int32_t cp = getSingleCP(s);
    எனில் (cp < 0) {
        எனில் (!stringsContains(s)) {
            _add(s);
            releasePattern();
        }
    } இல்லையெனில் {
        add(static_cast<UChar32>(cp));
    }
    திரும்பு *இது;
}

/**
 * Adds the given string, in order, to 'strings_'.  The given string
 * must have been checked by the caller to not already be in 'strings_'.
 */
வெறுமை UnicodeSet::_add(மாறிலி UnicodeString& s) {
    எனில் (isFrozen() || isBogus()) {
        திரும்பு;
    }
    UErrorCode ec = U_ZERO_ERROR;
    எனில் (strings_ == nullptr && !allocateStrings(ec)) {
        setToBogus();
        திரும்பு;
    }
    LocalPointer<UnicodeString> t(புதிய UnicodeString(s));
    எனில் (t.isNull()) { // Check for memory allocation error.
        setToBogus();
        திரும்பு;
    }
    strings_->sorted_செருகு(t.orphan(), compareUnicodeString, ec);
    எனில் (U_FAILURE(ec)) {
        setToBogus();
    }
}

/**
 * @return a code point IF the string consists of a single one.
 * otherwise returns -1.
 * @param string to test
 */
int32_t UnicodeSet::getSingleCP(மாறிலி UnicodeString& s) {
    int32_t sLength = s.length();
    எனில் (sLength == 1) திரும்பு s.charAt(0);
    எனில் (sLength == 2) {
        UChar32 cp = s.char32At(0);
        எனில் (cp > 0xFFFF) { // is surrogate pair
            திரும்பு cp;
        }
    }
    திரும்பு -1;
}

/**
 * Adds each of the characters in this string to the set. Thus "ch" => {"c", "h"}
 * If this set already any particular character, it has no effect on that character.
 * @param the source string
 * @return the modified set, for chaining
 */
UnicodeSet& UnicodeSet::addAll(மாறிலி UnicodeString& s) U_LIFETIME_BOUND {
    UChar32 cp;
    க்கு (int32_t i = 0; i < s.length(); i += U16_LENGTH(cp)) {
        cp = s.char32At(i);
        add(cp);
    }
    திரும்பு *இது;
}

/**
 * Retains EACH of the characters in this string. Note: "ch" == {"c", "h"}
 * If this set already any particular character, it has no effect on that character.
 * @param the source string
 * @return the modified set, for chaining
 */
UnicodeSet& UnicodeSet::retainAll(மாறிலி UnicodeString& s) U_LIFETIME_BOUND {
    UnicodeSet set;
    set.addAll(s);
    retainAll(set);
    திரும்பு *இது;
}

/**
 * Complement EACH of the characters in this string. Note: "ch" == {"c", "h"}
 * If this set already any particular character, it has no effect on that character.
 * @param the source string
 * @return the modified set, for chaining
 */
UnicodeSet& UnicodeSet::complementAll(மாறிலி UnicodeString& s) U_LIFETIME_BOUND {
    UnicodeSet set;
    set.addAll(s);
    complementAll(set);
    திரும்பு *இது;
}

/**
 * Remove EACH of the characters in this string. Note: "ch" == {"c", "h"}
 * If this set already any particular character, it has no effect on that character.
 * @param the source string
 * @return the modified set, for chaining
 */
UnicodeSet& UnicodeSet::removeAll(மாறிலி UnicodeString& s) U_LIFETIME_BOUND {
    UnicodeSet set;
    set.addAll(s);
    removeAll(set);
    திரும்பு *இது;
}

UnicodeSet& UnicodeSet::removeAllStrings() U_LIFETIME_BOUND {
    எனில் (!isFrozen() && hasStrings()) {
        strings_->removeAllElements();
        releasePattern();
    }
    திரும்பு *இது;
}


/**
 * Makes a set from a multicharacter string. Thus "ch" => {"ch"}
 * <br><b>Warning: you cannot add an empty string ("") to a UnicodeSet.</b>
 * @param the source string
 * @return a newly created set containing the given string
 */
UnicodeSet* U_EXPORT2 UnicodeSet::createFrom(மாறிலி UnicodeString& s) {
    UnicodeSet *set = புதிய UnicodeSet();
    எனில் (set != nullptr) { // Check for memory allocation error.
        set->add(s);
    }
    திரும்பு set;
}


/**
 * Makes a set from each of the characters in the string. Thus "ch" => {"c", "h"}
 * @param the source string
 * @return a newly created set containing the given characters
 */
UnicodeSet* U_EXPORT2 UnicodeSet::createFromAll(மாறிலி UnicodeString& s) {
    UnicodeSet *set = புதிய UnicodeSet();
    எனில் (set != nullptr) { // Check for memory allocation error.
        set->addAll(s);
    }
    திரும்பு set;
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
    எனில் (pinCodePoint(start) <= pinCodePoint(end)) {
        UChar32 range[3] = { start, end+1, UNICODESET_உயர்வு };
        retain(range, 2, 0);
    } இல்லையெனில் {
        clear();
    }
    திரும்பு *இது;
}

UnicodeSet& UnicodeSet::retain(UChar32 c) U_LIFETIME_BOUND {
    திரும்பு retain(c, c);
}

UnicodeSet& UnicodeSet::retain(மாறிலி UnicodeString &s) U_LIFETIME_BOUND {
    எனில் (isFrozen() || isBogus()) { திரும்பு *இது; }
    UChar32 cp = getSingleCP(s);
    எனில் (cp < 0) {
        தருக்க isIn = stringsContains(s);
        // Check for getRangeCount() first to avoid somewhat-expensive size()
        // when there are single code points.
        எனில் (isIn && get_Range_மொத்தம்() == 0 && size() == 1) {
            திரும்பு *இது;
        }
        clear();
        எனில் (isIn) {
            _add(s);
        }
    } இல்லையெனில் {
        retain(cp, cp);
    }
    திரும்பு *இது;
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
    எனில் (pinCodePoint(start) <= pinCodePoint(end)) {
        UChar32 range[3] = { start, end+1, UNICODESET_உயர்வு };
        retain(range, 2, 2);
    }
    திரும்பு *இது;
}

/**
 * Removes the specified character from this set if it is present.
 * The set will not contain the specified range once the call
 * returns.
 */
UnicodeSet& UnicodeSet::remove(UChar32 c) U_LIFETIME_BOUND {
    திரும்பு remove(c, c);
}

/**
 * Removes the specified string from this set if it is present.
 * The set will not contain the specified character once the call
 * returns.
 * @param the source string
 * @return the modified set, for chaining
 */
UnicodeSet& UnicodeSet::remove(மாறிலி UnicodeString& s) U_LIFETIME_BOUND {
    எனில் (isFrozen() || isBogus()) திரும்பு *இது;
    int32_t cp = getSingleCP(s);
    எனில் (cp < 0) {
        எனில் (strings_ != nullptr && strings_->removeElement((வெறுமை*) &s)) {
            releasePattern();
        }
    } இல்லையெனில் {
        remove(static_cast<UChar32>(cp), static_cast<UChar32>(cp));
    }
    திரும்பு *இது;
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
    எனில் (isFrozen() || isBogus()) {
        திரும்பு *இது;
    }
    எனில் (pinCodePoint(start) <= pinCodePoint(end)) {
        UChar32 range[3] = { start, end+1, UNICODESET_உயர்வு };
        exclusiveOr(range, 2, 0);
    }
    releasePattern();
    திரும்பு *இது;
}

UnicodeSet& UnicodeSet::complement(UChar32 c) U_LIFETIME_BOUND {
    திரும்பு complement(c, c);
}

/**
 * This is equivalent to
 * <code>complement(MIN_VALUE, MAX_VALUE)</code>.
 */
UnicodeSet& UnicodeSet::complement() U_LIFETIME_BOUND {
    எனில் (isFrozen() || isBogus()) {
        திரும்பு *இது;
    }
    எனில் (list[0] == UNICODESET_கீழ்) {
        uprv_memmove(list, list + 1, (size_t)(len-1)*அளவு(UChar32));
        --len;
    } இல்லையெனில் {
        எனில் (!ensureCapacity(len+1)) {
            திரும்பு *இது;
        }
        uprv_memmove(list + 1, list, (size_t)len*அளவு(UChar32));
        list[0] = UNICODESET_கீழ்;
        ++len;
    }
    releasePattern();
    திரும்பு *இது;
}

/**
 * Complement the specified string in this set.
 * The set will not contain the specified string once the call
 * returns.
 *
 * @param s the string to complement
 * @return this object, for chaining
 */
UnicodeSet& UnicodeSet::complement(மாறிலி UnicodeString& s) U_LIFETIME_BOUND {
    எனில் (isFrozen() || isBogus()) திரும்பு *இது;
    int32_t cp = getSingleCP(s);
    எனில் (cp < 0) {
        எனில் (stringsContains(s)) {
            strings_->removeElement((வெறுமை*) &s);
        } இல்லையெனில் {
            _add(s);
        }
        releasePattern();
    } இல்லையெனில் {
        complement(static_cast<UChar32>(cp), static_cast<UChar32>(cp));
    }
    திரும்பு *இது;
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
UnicodeSet& UnicodeSet::addAll(மாறிலி UnicodeSet& c) U_LIFETIME_BOUND {
    எனில் ( c.len>0 && c.list!=nullptr ) {
        add(c.list, c.len, 0);
    }

    // Add strings in order
    எனில் ( c.strings_!=nullptr ) {
        க்கு (int32_t i=0; i<c.strings_->size(); ++i) {
            மாறிலி UnicodeString* s = static_cast<மாறிலி UnicodeString*>(c.strings_->elementAt(i));
            எனில் (!stringsContains(*s)) {
                _add(*s);
            }
        }
    }
    திரும்பு *இது;
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
UnicodeSet& UnicodeSet::retainAll(மாறிலி UnicodeSet& c) U_LIFETIME_BOUND {
    எனில் (isFrozen() || isBogus()) {
        திரும்பு *இது;
    }
    retain(c.list, c.len, 0);
    எனில் (hasStrings()) {
        எனில் (!c.hasStrings()) {
            strings_->removeAllElements();
        } இல்லையெனில் {
            strings_->retainAll(*c.strings_);
        }
    }
    திரும்பு *இது;
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
UnicodeSet& UnicodeSet::removeAll(மாறிலி UnicodeSet& c) U_LIFETIME_BOUND {
    எனில் (isFrozen() || isBogus()) {
        திரும்பு *இது;
    }
    retain(c.list, c.len, 2);
    எனில் (hasStrings() && c.hasStrings()) {
        strings_->removeAll(*c.strings_);
    }
    திரும்பு *இது;
}

/**
 * Complements in this set all elements contained in the specified
 * set.  Any character in the other set will be removed if it is
 * in this set, or will be added if it is not in this set.
 *
 * @param c set that defines which elements will be xor'ed from
 *          this set.
 */
UnicodeSet& UnicodeSet::complementAll(மாறிலி UnicodeSet& c) U_LIFETIME_BOUND {
    எனில் (isFrozen() || isBogus()) {
        திரும்பு *இது;
    }
    exclusiveOr(c.list, c.len, 0);

    எனில் (c.strings_ != nullptr) {
        க்கு (int32_t i=0; i<c.strings_->size(); ++i) {
            வெறுமை* e = c.strings_->elementAt(i);
            எனில் (strings_ == nullptr || !strings_->removeElement(e)) {
                _add(*static_cast<மாறிலி UnicodeString*>(e));
            }
        }
    }
    திரும்பு *இது;
}

/**
 * Removes all of the elements from this set.  This set will be
 * empty after this call returns.
 */
UnicodeSet& UnicodeSet::clear() U_LIFETIME_BOUND {
    எனில் (isFrozen()) {
        திரும்பு *இது;
    }
    list[0] = UNICODESET_உயர்வு;
    len = 1;
    releasePattern();
    எனில் (strings_ != nullptr) {
        strings_->removeAllElements();
    }
    // Remove bogus
    fFlags = 0;
    திரும்பு *இது;
}

/**
 * Iteration method that returns the number of ranges contained in
 * this set.
 * @see #getRangeStart
 * @see #getRangeEnd
 */
int32_t UnicodeSet::get_Range_மொத்தம்() மாறிலி {
    திரும்பு len/2;
}

/**
 * Iteration method that returns the first character in the
 * specified range of this set.
 * @see #getRangeCount
 * @see #getRangeEnd
 */
UChar32 UnicodeSet::getRangeStart(int32_t index) மாறிலி {
    திரும்பு list[index*2];
}

/**
 * Iteration method that returns the last character in the
 * specified range of this set.
 * @see #getRangeStart
 * @see #getRangeEnd
 */
UChar32 UnicodeSet::getRangeEnd(int32_t index) மாறிலி {
    திரும்பு list[index*2 + 1] - 1;
}

மாறிலி UnicodeString* UnicodeSet::getString(int32_t index) மாறிலி U_LIFETIME_BOUND {
    திரும்பு static_cast<மாறிலி UnicodeString*>(strings_->elementAt(index));
}

/**
 * Reallocate this objects internal structures to take up the least
 * possible space, without changing this object's value.
 */
UnicodeSet& UnicodeSet::compact() U_LIFETIME_BOUND {
    எனில் (isFrozen() || isBogus()) {
        திரும்பு *இது;
    }
    // Delete buffer first to defragment memory less.
    எனில் (buffer != அடுக்கு_List) {
        uprv_free(buffer);
        buffer = nullptr;
        bufferCapacity = 0;
    }
    எனில் (list == அடுக்கு_List) {
        // pass
    } இல்லையெனில் எனில் (len <= INITIAL_CAPACITY) {
        uprv_memcpy(அடுக்கு_List, list, len * அளவு(UChar32));
        uprv_free(list);
        list = அடுக்கு_List;
        capacity = INITIAL_CAPACITY;
    } இல்லையெனில் எனில் ((len + 7) < capacity) {
        // If we have more than a little unused capacity, shrink it to len.
        UChar32* temp = static_cast<UChar32*>(uprv_realloc(list, அளவு(UChar32) * len));
        எனில் (temp) {
            list = temp;
            capacity = len;
        }
        // else what the heck happened?! We allocated less memory!
        // Oh well. We'll keep our original array.
    }
    எனில் (strings_ != nullptr && strings_->isEmpty()) {
        delete strings_;
        strings_ = nullptr;
    }
    திரும்பு *இது;
}

#ifdef DEBUG_SERIALIZE
#அந்தர்பாவ <stdio.h>
#endif

/**
 * Deserialize constructor.
 */
UnicodeSet::UnicodeSet(மாறிலி uint16_t data[], int32_t dataLen, ESerialization serialization,
                       UErrorCode &ec) {

  எனில்(U_FAILURE(ec)) {
    setToBogus();
    திரும்பு;
  }

  எனில்( (serialization != kSerialized)
      || (data==nullptr)
      || (dataLen < 1)) {
    ec = U_ILLEGAL_ARGUMENT_ERROR;
    setToBogus();
    திரும்பு;
  }

  // bmp?
  int32_t headerSize = ((data[0]&0x8000)) ?2:1;
  int32_t bmpLength = (headerSize==1)?data[0]:data[1];

  int32_t newLength = (((data[0]&0x7FFF)-bmpLength)/2)+bmpLength;
#ifdef DEBUG_SERIALIZE
  printf("dataLen %d headerSize %d bmpLen %d len %d. data[0]=%X/%X/%X/%X\n", dataLen,headerSize,bmpLength,newLength, data[0],data[1],data[2],data[3]);
#endif
  எனில்(!ensureCapacity(newLength + 1)) {  // +1 for HIGH
    திரும்பு;
  }
  // copy bmp
  int32_t i;
  க்கு(i = 0; i< bmpLength;i++) {
    list[i] = data[i+headerSize];
#ifdef DEBUG_SERIALIZE
    printf("<<16@%d[%d] %X\n", i+headerSize, i, list[i]);
#endif
  }
  // copy smp
  க்கு(i=bmpLength;i<newLength;i++) {
    list[i] = (static_cast<UChar32>(data[headerSize + bmpLength + (i - bmpLength) * 2 + 0]) << 16) +
               static_cast<UChar32>(data[headerSize + bmpLength + (i - bmpLength) * 2 + 1]);
#ifdef DEBUG_SERIALIZE
    printf("<<32@%d+[%d] %lX\n", headerSize+bmpLength+i, i, list[i]);
#endif
  }
  U_ASSERT(i == newLength);
  எனில் (i == 0 || list[i - 1] != UNICODESET_உயர்வு) {
    list[i++] = UNICODESET_உயர்வு;
  }
  len = i;
}


int32_t UnicodeSet::serialize(uint16_t *dest, int32_t destCapacity, UErrorCode& ec) மாறிலி {
    int32_t bmpLength, length, destLength;

    எனில் (U_FAILURE(ec)) {
        திரும்பு 0;
    }

    எனில் (destCapacity<0 || (destCapacity>0 && dest==nullptr)) {
        ec=U_ILLEGAL_ARGUMENT_ERROR;
        திரும்பு 0;
    }

    /* count necessary 16-bit units */
    length=இது->len-1; // Subtract 1 to ignore final UNICODESET_HIGH
    // assert(length>=0);
    எனில் (length==0) {
        /* empty set */
        எனில் (destCapacity>0) {
            *dest=0;
        } இல்லையெனில் {
            ec=U_BUFFER_OVERFLOW_ERROR;
        }
        திரும்பு 1;
    }
    /* now length>0 */

    எனில் (இது->list[length-1]<=0xffff) {
        /* all BMP */
        bmpLength=length;
    } இல்லையெனில் எனில் (இது->list[0]>=0x10000) {
        /* all supplementary */
        bmpLength=0;
        length*=2;
    } இல்லையெனில் {
        /* some BMP, some supplementary */
        க்கு (bmpLength=0; bmpLength<length && இது->list[bmpLength]<=0xffff; ++bmpLength) {}
        length=bmpLength+2*(length-bmpLength);
    }
#ifdef DEBUG_SERIALIZE
    printf(">> bmpLength%d length%d len%d\n", bmpLength, length, len);
#endif
    /* length: number of 16-bit array units */
    எனில் (length>0x7fff) {
        /* there are only 15 bits for the length in the first serialized word */
        ec=U_INDEX_OUTOFBOUNDS_ERROR;
        திரும்பு 0;
    }

    /*
     * total serialized length:
     * number of 16-bit array units (length) +
     * 1 length unit (always) +
     * 1 bmpLength unit (if there are supplementary values)
     */
    destLength=length+((length>bmpLength)?2:1);
    எனில் (destLength<=destCapacity) {
        மாறிலி UChar32 *p;
        int32_t i;

#ifdef DEBUG_SERIALIZE
        printf("writeHdr\n");
#endif
        *dest = static_cast<uint16_t>(length);
        எனில் (length>bmpLength) {
            *dest|=0x8000;
            *++dest = static_cast<uint16_t>(bmpLength);
        }
        ++dest;

        /* write the BMP part of the array */
        p=இது->list;
        க்கு (i=0; i<bmpLength; ++i) {
#ifdef DEBUG_SERIALIZE
          printf("writebmp: %x\n", (முழு)*p);
#endif
            *dest++ = static_cast<uint16_t>(*p++);
        }

        /* write the supplementary part of the array */
        க்கு (; i<length; i+=2) {
#ifdef DEBUG_SERIALIZE
          printf("write32: %x\n", (முழு)*p);
#endif
            *dest++ = static_cast<uint16_t>(*p >> 16);
            *dest++ = static_cast<uint16_t>(*p++);
        }
    } இல்லையெனில் {
        ec=U_BUFFER_OVERFLOW_ERROR;
    }
    திரும்பு destLength;
}

//----------------------------------------------------------------
// Implementation: Utility methods
//----------------------------------------------------------------

/**
 * Allocate our strings vector and return true if successful.
 */
UBool UnicodeSet::allocateStrings(UErrorCode &status) {
    எனில் (U_FAILURE(status)) {
        திரும்பு பொய்;
    }
    strings_ = புதிய UVector(uprv_deleteUObject,
                          uhash_compareUnicodeString, 1, status);
    எனில் (strings_ == nullptr) { // Check for memory allocation error.
        status = U_MEMORY_ALLOCATION_ERROR;
        திரும்பு பொய்;
    }
    எனில் (U_FAILURE(status)) {
        delete strings_;
        strings_ = nullptr;
        திரும்பு பொய்;
    } 
    திரும்பு உண்மை;
}

int32_t UnicodeSet::nextCapacity(int32_t minCapacity) {
    // Grow exponentially to reduce the frequency of allocations.
    எனில் (minCapacity < INITIAL_CAPACITY) {
        திரும்பு minCapacity + INITIAL_CAPACITY;
    } இல்லையெனில் எனில் (minCapacity <= 2500) {
        திரும்பு 5 * minCapacity;
    } இல்லையெனில் {
        int32_t newCapacity = 2 * minCapacity;
        எனில் (newCapacity > MAX_LENGTH) {
            newCapacity = MAX_LENGTH;
        }
        திரும்பு newCapacity;
    }
}

தருக்க UnicodeSet::ensureCapacity(int32_t newLen) {
    எனில் (newLen > MAX_LENGTH) {
        newLen = MAX_LENGTH;
    }
    எனில் (newLen <= capacity) {
        திரும்பு உண்மை;
    }
    int32_t newCapacity = nextCapacity(newLen);
    UChar32* temp = static_cast<UChar32*>(uprv_malloc(newCapacity * அளவு(UChar32)));
    எனில் (temp == nullptr) {
        setToBogus(); // set the object to bogus state if an OOM failure occurred.
        திரும்பு பொய்;
    }
    // Copy only the actual contents.
    uprv_memcpy(temp, list, len * அளவு(UChar32));
    எனில் (list != அடுக்கு_List) {
        uprv_free(list);
    }
    list = temp;
    capacity = newCapacity;
    திரும்பு உண்மை;
}

தருக்க UnicodeSet::ensureBufferCapacity(int32_t newLen) {
    எனில் (newLen > MAX_LENGTH) {
        newLen = MAX_LENGTH;
    }
    எனில் (newLen <= bufferCapacity) {
        திரும்பு உண்மை;
    }
    int32_t newCapacity = nextCapacity(newLen);
    UChar32* temp = static_cast<UChar32*>(uprv_malloc(newCapacity * அளவு(UChar32)));
    எனில் (temp == nullptr) {
        setToBogus();
        திரும்பு பொய்;
    }
    // The buffer has no contents to be copied.
    // It is always filled from scratch after this call.
    எனில் (buffer != அடுக்கு_List) {
        uprv_free(buffer);
    }
    buffer = temp;
    bufferCapacity = newCapacity;
    திரும்பு உண்மை;
}

/**
 * Swap list and buffer.
 */
வெறுமை UnicodeSet::swapBuffers() {
    // swap list and buffer
    UChar32* temp = list;
    list = buffer;
    buffer = temp;

    int32_t c = capacity;
    capacity = bufferCapacity;
    bufferCapacity = c;
}

வெறுமை UnicodeSet::setToBogus() {
    clear(); // Remove everything in the set.
    fFlags = kIsBogus;
}

//----------------------------------------------------------------
// Implementation: Fundamental operators
//----------------------------------------------------------------

நிலையான உள்வரி UChar32 max(UChar32 a, UChar32 b) {
    திரும்பு (a > b) ? a : b;
}

// polarity = 0, 3 is normal: x xor y
// polarity = 1, 2: x xor ~y == x === y

வெறுமை UnicodeSet::exclusiveOr(மாறிலி UChar32* other, int32_t otherLen, int8_t polarity) {
    எனில் (isFrozen() || isBogus()) {
        திரும்பு;
    }
    எனில் (!ensureBufferCapacity(len + otherLen)) {
        திரும்பு;
    }

    int32_t i = 0, j = 0, k = 0;
    UChar32 a = list[i++];
    UChar32 b;
    எனில் (polarity == 1 || polarity == 2) {
        b = UNICODESET_கீழ்;
        எனில் (other[j] == UNICODESET_கீழ்) { // skip base if already LOW
            ++j;
            b = other[j];
        }
    } இல்லையெனில் {
        b = other[j++];
    }
    // simplest of all the routines
    // sort the values, discarding identicals!
    க்கு (;;) {
        எனில் (a < b) {
            buffer[k++] = a;
            a = list[i++];
        } இல்லையெனில் எனில் (b < a) {
            buffer[k++] = b;
            b = other[j++];
        } இல்லையெனில் எனில் (a != UNICODESET_உயர்வு) { // at this point, a == b
            // discard both values!
            a = list[i++];
            b = other[j++];
        } இல்லையெனில் { // DONE!
            buffer[k++] = UNICODESET_உயர்வு;
            len = k;
            நிறுத்து;
        }
    }
    swapBuffers();
    releasePattern();
}

// polarity = 0 is normal: x union y
// polarity = 2: x union ~y
// polarity = 1: ~x union y
// polarity = 3: ~x union ~y

வெறுமை UnicodeSet::add(மாறிலி UChar32* other, int32_t otherLen, int8_t polarity) {
    எனில் (isFrozen() || isBogus() || other==nullptr) {
        திரும்பு;
    }
    எனில் (!ensureBufferCapacity(len + otherLen)) {
        திரும்பு;
    }

    int32_t i = 0, j = 0, k = 0;
    UChar32 a = list[i++];
    UChar32 b = other[j++];
    // change from xor is that we have to check overlapping pairs
    // polarity bit 1 means a is second, bit 2 means b is.
    க்கு (;;) {
        தேர்வு (polarity) {
          நிலை 0: // both first; take lower if unequal
            எனில் (a < b) { // take a
                // Back up over overlapping ranges in buffer[]
                எனில் (k > 0 && a <= buffer[k-1]) {
                    // Pick latter end value in buffer[] vs. list[]
                    a = max(list[i], buffer[--k]);
                } இல்லையெனில் {
                    // No overlap
                    buffer[k++] = a;
                    a = list[i];
                }
                i++; // Common if/else code factored out
                polarity ^= 1;
            } இல்லையெனில் எனில் (b < a) { // take b
                எனில் (k > 0 && b <= buffer[k-1]) {
                    b = max(other[j], buffer[--k]);
                } இல்லையெனில் {
                    buffer[k++] = b;
                    b = other[j];
                }
                j++;
                polarity ^= 2;
            } இல்லையெனில் { // a == b, take a, drop b
                எனில் (a == UNICODESET_உயர்வு) செல் loop_end;
                // This is symmetrical; it doesn't matter if
                // we backtrack with a or b. - liu
                எனில் (k > 0 && a <= buffer[k-1]) {
                    a = max(list[i], buffer[--k]);
                } இல்லையெனில் {
                    // No overlap
                    buffer[k++] = a;
                    a = list[i];
                }
                i++;
                polarity ^= 1;
                b = other[j++];
                polarity ^= 2;
            }
            நிறுத்து;
          நிலை 3: // both second; take higher if unequal, and drop other
            எனில் (b <= a) { // take a
                எனில் (a == UNICODESET_உயர்வு) செல் loop_end;
                buffer[k++] = a;
            } இல்லையெனில் { // take b
                எனில் (b == UNICODESET_உயர்வு) செல் loop_end;
                buffer[k++] = b;
            }
            a = list[i++];
            polarity ^= 1;   // factored common code
            b = other[j++];
            polarity ^= 2;
            நிறுத்து;
          நிலை 1: // a second, b first; if b < a, overlap
            எனில் (a < b) { // no overlap, take a
                buffer[k++] = a; a = list[i++]; polarity ^= 1;
            } இல்லையெனில் எனில் (b < a) { // OVERLAP, drop b
                b = other[j++];
                polarity ^= 2;
            } இல்லையெனில் { // a == b, drop both!
                எனில் (a == UNICODESET_உயர்வு) செல் loop_end;
                a = list[i++];
                polarity ^= 1;
                b = other[j++];
                polarity ^= 2;
            }
            நிறுத்து;
          நிலை 2: // a first, b second; if a < b, overlap
            எனில் (b < a) { // no overlap, take b
                buffer[k++] = b;
                b = other[j++];
                polarity ^= 2;
            } இல்லையெனில்  எனில் (a < b) { // OVERLAP, drop a
                a = list[i++];
                polarity ^= 1;
            } இல்லையெனில் { // a == b, drop both!
                எனில் (a == UNICODESET_உயர்வு) செல் loop_end;
                a = list[i++];
                polarity ^= 1;
                b = other[j++];
                polarity ^= 2;
            }
            நிறுத்து;
        }
    }
 loop_end:
    buffer[k++] = UNICODESET_உயர்வு;    // terminate
    len = k;
    swapBuffers();
    releasePattern();
}

// polarity = 0 is normal: x intersect y
// polarity = 2: x intersect ~y == set-minus
// polarity = 1: ~x intersect y
// polarity = 3: ~x intersect ~y

வெறுமை UnicodeSet::retain(மாறிலி UChar32* other, int32_t otherLen, int8_t polarity) {
    எனில் (isFrozen() || isBogus()) {
        திரும்பு;
    }
    எனில் (!ensureBufferCapacity(len + otherLen)) {
        திரும்பு;
    }

    int32_t i = 0, j = 0, k = 0;
    UChar32 a = list[i++];
    UChar32 b = other[j++];
    // change from xor is that we have to check overlapping pairs
    // polarity bit 1 means a is second, bit 2 means b is.
    க்கு (;;) {
        தேர்வு (polarity) {
          நிலை 0: // both first; drop the smaller
            எனில் (a < b) { // drop a
                a = list[i++];
                polarity ^= 1;
            } இல்லையெனில் எனில் (b < a) { // drop b
                b = other[j++];
                polarity ^= 2;
            } இல்லையெனில் { // a == b, take one, drop other
                எனில் (a == UNICODESET_உயர்வு) செல் loop_end;
                buffer[k++] = a;
                a = list[i++];
                polarity ^= 1;
                b = other[j++];
                polarity ^= 2;
            }
            நிறுத்து;
          நிலை 3: // both second; take lower if unequal
            எனில் (a < b) { // take a
                buffer[k++] = a;
                a = list[i++];
                polarity ^= 1;
            } இல்லையெனில் எனில் (b < a) { // take b
                buffer[k++] = b;
                b = other[j++];
                polarity ^= 2;
            } இல்லையெனில் { // a == b, take one, drop other
                எனில் (a == UNICODESET_உயர்வு) செல் loop_end;
                buffer[k++] = a;
                a = list[i++];
                polarity ^= 1;
                b = other[j++];
                polarity ^= 2;
            }
            நிறுத்து;
          நிலை 1: // a second, b first;
            எனில் (a < b) { // NO OVERLAP, drop a
                a = list[i++];
                polarity ^= 1;
            } இல்லையெனில் எனில் (b < a) { // OVERLAP, take b
                buffer[k++] = b;
                b = other[j++];
                polarity ^= 2;
            } இல்லையெனில் { // a == b, drop both!
                எனில் (a == UNICODESET_உயர்வு) செல் loop_end;
                a = list[i++];
                polarity ^= 1;
                b = other[j++];
                polarity ^= 2;
            }
            நிறுத்து;
          நிலை 2: // a first, b second; if a < b, overlap
            எனில் (b < a) { // no overlap, drop b
                b = other[j++];
                polarity ^= 2;
            } இல்லையெனில்  எனில் (a < b) { // OVERLAP, take a
                buffer[k++] = a;
                a = list[i++];
                polarity ^= 1;
            } இல்லையெனில் { // a == b, drop both!
                எனில் (a == UNICODESET_உயர்வு) செல் loop_end;
                a = list[i++];
                polarity ^= 1;
                b = other[j++];
                polarity ^= 2;
            }
            நிறுத்து;
        }
    }
 loop_end:
    buffer[k++] = UNICODESET_உயர்வு;    // terminate
    len = k;
    swapBuffers();
    releasePattern();
}

/**
 * Append the <code>toPattern()</code> representation of a
 * string to the given <code>StringBuffer</code>.
 */
வெறுமை UnicodeSet::_appendToPat(UnicodeString& buf, மாறிலி UnicodeString& s, UBool escapeUnprintable) {
    UChar32 cp;
    க்கு (int32_t i = 0; i < s.length(); i += U16_LENGTH(cp)) {
        _appendToPat(buf, cp = s.char32At(i), escapeUnprintable);
    }
}

/**
 * Append the <code>toPattern()</code> representation of a
 * character to the given <code>StringBuffer</code>.
 */
வெறுமை UnicodeSet::_appendToPat(UnicodeString& buf, UChar32 c, UBool escapeUnprintable) {
    எனில் (escapeUnprintable ? ICU_Utility::isUnprintable(c) : ICU_Utility::shouldAlwaysBeEscaped(c)) {
        // Use hex escape notation (\uxxxx or \Uxxxxxxxx) for anything
        // unprintable
        ICU_Utility::escape(buf, c);
        திரும்பு;
    }
    // Okay to let ':' pass through
    தேர்வு (c) {
    நிலை u'[':
    நிலை u']':
    நிலை u'-':
    நிலை u'^':
    நிலை u'&':
    நிலை u'\\':
    நிலை u'{':
    நிலை u'}':
    நிலை u':':
    நிலை குறியீடு_Table::குறியீடு_REF:
        buf.append(u'\\');
        நிறுத்து;
    இயல்பு:
        // Escape whitespace
        எனில் (PatternProps::isWhiteSpace(c)) {
            buf.append(u'\\');
        }
        நிறுத்து;
    }
    buf.append(c);
}

வெறுமை UnicodeSet::_appendToPat(UnicodeString &result, UChar32 start, UChar32 end,
                              UBool escapeUnprintable) {
    _appendToPat(result, start, escapeUnprintable);
    எனில் (start != end) {
        எனில் ((start+1) != end ||
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
                                      UBool escapeUnprintable) மாறிலி
{
    எனில் (pat != nullptr) {
        int32_t i;
        int32_t backslash_மொத்தம் = 0;
        க்கு (i=0; i<patLen; ) {
            UChar32 c;
            U16_NEXT(pat, i, patLen, c);
            எனில் (escapeUnprintable ?
                    ICU_Utility::isUnprintable(c) : ICU_Utility::shouldAlwaysBeEscaped(c)) {
                // If the unprintable character is preceded by an odd
                // number of backslashes, then it has been escaped.
                // Before unescaping it, we delete the final
                // backslash.
                எனில் ((backslash_மொத்தம் % 2) == 1) {
                    result.truncate(result.length() - 1);
                }
                ICU_Utility::escape(result, c);
                backslash_மொத்தம் = 0;
            } இல்லையெனில் {
                result.append(c);
                எனில் (c == u'\\') {
                    ++backslash_மொத்தம்;
                } இல்லையெனில் {
                    backslash_மொத்தம் = 0;
                }
            }
        }
        திரும்பு result;
    }

    திரும்பு _generatePattern(result, escapeUnprintable);
}

/**
 * Returns a string representation of this set.  If the result of
 * calling this function is passed to a UnicodeSet constructor, it
 * will produce another set that is equal to this one.
 */
UnicodeString& UnicodeSet::toPattern(UnicodeString& result U_LIFETIME_BOUND,
                                     UBool escapeUnprintable) மாறிலி
{
    result.truncate(0);
    திரும்பு _toPattern(result, escapeUnprintable);
}

/**
 * Generate and append a string representation of this set to result.
 * This does not use this.pat, the cleaned up copy of the string
 * passed to applyPattern().
 */
UnicodeString& UnicodeSet::_generatePattern(UnicodeString& result U_LIFETIME_BOUND,
                                            UBool escapeUnprintable) மாறிலி
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
    எனில் (len >= 4 && list[0] == 0 && limit == len && !hasStrings()) {
        // Emit the inverse
        result.append(u'^');
        // Offsetting the inversion list index by one lets us
        // iterate over the ranges of the set complement.
        i = 1;
        --limit;
    }

    // Emit the ranges as pairs.
    போது (i < limit) {
        UChar32 start = list[i];  // getRangeStart()
        UChar32 end = list[i + 1] - 1;  // getRangeEnd() = range limit minus one
        எனில் (!(0xd800 <= end && end <= 0xdbff)) {
            _appendToPat(result, start, end, escapeUnprintable);
            i += 2;
        } இல்லையெனில் {
            // The range ends with a lead surrogate.
            // Avoid writing what looks like a lead+trail surrogate pair.
            // 1. Postpone ranges that start with a lead surrogate code point.
            int32_t firstLead = i;
            போது ((i += 2) < limit && list[i] <= 0xdbff) {}
            int32_t firstAfterLead = i;
            // 2. Write following ranges that start with a trail surrogate code point.
            போது (i < limit && (start = list[i]) <= 0xdfff) {
                _appendToPat(result, start, list[i + 1] - 1, escapeUnprintable);
                i += 2;
            }
            // 3. Now write the postponed ranges.
            க்கு (முழு j = firstLead; j < firstAfterLead; j += 2) {
                _appendToPat(result, list[j], list[j + 1] - 1, escapeUnprintable);
            }
        }
    }

    எனில் (strings_ != nullptr) {
        க்கு (int32_t i = 0; i<strings_->size(); ++i) {
            result.append(u'{');
            _appendToPat(result,
                         *static_cast<மாறிலி UnicodeString*>(strings_->elementAt(i)),
                         escapeUnprintable);
            result.append(u'}');
        }
    }
    திரும்பு result.append(u']');
}

/**
* Release existing cached pattern
*/
வெறுமை UnicodeSet::releasePattern() {
    எனில் (pat) {
        uprv_free(pat);
        pat = nullptr;
        patLen = 0;
    }
}

/**
* Set the new pattern to cache.
*/
வெறுமை UnicodeSet::setPattern(மாறிலி char16_t *newPat, int32_t newPatLen) {
    releasePattern();
    pat = static_cast<char16_t*>(uprv_malloc((newPatLen + 1) * அளவு(char16_t)));
    எனில் (pat) {
        patLen = newPatLen;
        u_memcpy(pat, newPat, patLen);
        pat[patLen] = 0;
    }
    // else we don't care if malloc failed. This was just a nice cache.
    // We can regenerate an equivalent pattern later when requested.
}

UnicodeSet* UnicodeSet::freeze() U_LIFETIME_BOUND {
    எனில்(!isFrozen() && !isBogus()) {
        compact();

        // Optimize contains() and span() and similar functions.
        எனில் (hasStrings()) {
            stringSpan = புதிய UnicodeSetStringSpan(*இது, *strings_, UnicodeSetStringSpan::ALL);
            எனில் (stringSpan == nullptr) {
                setToBogus();
                திரும்பு இது;
            } இல்லையெனில் எனில் (!stringSpan->needsStringSpanUTF16()) {
                // All strings are irrelevant for span() etc. because
                // all of each string's code points are contained in this set.
                // Do not check needsStringSpanUTF8() because UTF-8 has at most as
                // many relevant strings as UTF-16.
                // (Thus needsStringSpanUTF8() implies needsStringSpanUTF16().)
                delete stringSpan;
                stringSpan = nullptr;
            }
        }
        எனில் (stringSpan == nullptr) {
            // No span-relevant strings: Optimize for code point spans.
            bmpSet=புதிய BMPSet(list, len);
            எனில் (bmpSet == nullptr) { // Check for memory allocation error.
                setToBogus();
            }
        }
    }
    திரும்பு இது;
}

int32_t UnicodeSet::span(மாறிலி char16_t *s, int32_t length, USetSpanCondition spanCondition) மாறிலி {
    எனில்(length>0 && bmpSet!=nullptr) {
        திரும்பு static_cast<int32_t>(bmpSet->span(s, s + length, spanCondition) - s);
    }
    எனில்(length<0) {
        length=u_strlen(s);
    }
    எனில்(length==0) {
        திரும்பு 0;
    }
    எனில்(stringSpan!=nullptr) {
        திரும்பு stringSpan->span(s, length, spanCondition);
    } இல்லையெனில் எனில்(hasStrings()) {
        uint32_t which= spanCondition==USET_SPAN_NOT_CONTAINED ?
                            UnicodeSetStringSpan::FWD_UTF16_NOT_CONTAINED :
                            UnicodeSetStringSpan::FWD_UTF16_CONTAINED;
        UnicodeSetStringSpan strSpan(*இது, *strings_, which);
        எனில்(strSpan.needsStringSpanUTF16()) {
            திரும்பு strSpan.span(s, length, spanCondition);
        }
    }

    எனில்(spanCondition!=USET_SPAN_NOT_CONTAINED) {
        spanCondition=USET_SPAN_CONTAINED;  // Pin to 0/1 values.
    }

    UChar32 c;
    int32_t start=0, prev=0;
    செய் {
        U16_NEXT(s, start, length, c);
        எனில்(spanCondition!=contains(c)) {
            நிறுத்து;
        }
    } போது((prev=start)<length);
    திரும்பு prev;
}

int32_t UnicodeSet::spanBack(மாறிலி char16_t *s, int32_t length, USetSpanCondition spanCondition) மாறிலி {
    எனில்(length>0 && bmpSet!=nullptr) {
        திரும்பு static_cast<int32_t>(bmpSet->spanBack(s, s + length, spanCondition) - s);
    }
    எனில்(length<0) {
        length=u_strlen(s);
    }
    எனில்(length==0) {
        திரும்பு 0;
    }
    எனில்(stringSpan!=nullptr) {
        திரும்பு stringSpan->spanBack(s, length, spanCondition);
    } இல்லையெனில் எனில்(hasStrings()) {
        uint32_t which= spanCondition==USET_SPAN_NOT_CONTAINED ?
                            UnicodeSetStringSpan::BACK_UTF16_NOT_CONTAINED :
                            UnicodeSetStringSpan::BACK_UTF16_CONTAINED;
        UnicodeSetStringSpan strSpan(*இது, *strings_, which);
        எனில்(strSpan.needsStringSpanUTF16()) {
            திரும்பு strSpan.spanBack(s, length, spanCondition);
        }
    }

    எனில்(spanCondition!=USET_SPAN_NOT_CONTAINED) {
        spanCondition=USET_SPAN_CONTAINED;  // Pin to 0/1 values.
    }

    UChar32 c;
    int32_t prev=length;
    செய் {
        U16_PREV(s, 0, length, c);
        எனில்(spanCondition!=contains(c)) {
            நிறுத்து;
        }
    } போது((prev=length)>0);
    திரும்பு prev;
}

int32_t UnicodeSet::spanUTF8(மாறிலி எழுத்து *s, int32_t length, USetSpanCondition spanCondition) மாறிலி {
    எனில்(length>0 && bmpSet!=nullptr) {
        மாறிலி uint8_t* s0 = reinterpret_cast<மாறிலி uint8_t*>(s);
        திரும்பு static_cast<int32_t>(bmpSet->spanUTF8(s0, length, spanCondition) - s0);
    }
    எனில்(length<0) {
        length = static_cast<int32_t>(uprv_strlen(s));
    }
    எனில்(length==0) {
        திரும்பு 0;
    }
    எனில்(stringSpan!=nullptr) {
        திரும்பு stringSpan->spanUTF8(reinterpret_cast<மாறிலி uint8_t*>(s), length, spanCondition);
    } இல்லையெனில் எனில்(hasStrings()) {
        uint32_t which= spanCondition==USET_SPAN_NOT_CONTAINED ?
                            UnicodeSetStringSpan::FWD_UTF8_NOT_CONTAINED :
                            UnicodeSetStringSpan::FWD_UTF8_CONTAINED;
        UnicodeSetStringSpan strSpan(*இது, *strings_, which);
        எனில்(strSpan.needsStringSpanUTF8()) {
            திரும்பு strSpan.spanUTF8(reinterpret_cast<மாறிலி uint8_t*>(s), length, spanCondition);
        }
    }

    எனில்(spanCondition!=USET_SPAN_NOT_CONTAINED) {
        spanCondition=USET_SPAN_CONTAINED;  // Pin to 0/1 values.
    }

    UChar32 c;
    int32_t start=0, prev=0;
    செய் {
        U8_NEXT_OR_FFFD(s, start, length, c);
        எனில்(spanCondition!=contains(c)) {
            நிறுத்து;
        }
    } போது((prev=start)<length);
    திரும்பு prev;
}

int32_t UnicodeSet::spanBackUTF8(மாறிலி எழுத்து *s, int32_t length, USetSpanCondition spanCondition) மாறிலி {
    எனில்(length>0 && bmpSet!=nullptr) {
        மாறிலி uint8_t* s0 = reinterpret_cast<மாறிலி uint8_t*>(s);
        திரும்பு bmpSet->spanBackUTF8(s0, length, spanCondition);
    }
    எனில்(length<0) {
        length = static_cast<int32_t>(uprv_strlen(s));
    }
    எனில்(length==0) {
        திரும்பு 0;
    }
    எனில்(stringSpan!=nullptr) {
        திரும்பு stringSpan->spanBackUTF8(reinterpret_cast<மாறிலி uint8_t*>(s), length, spanCondition);
    } இல்லையெனில் எனில்(hasStrings()) {
        uint32_t which= spanCondition==USET_SPAN_NOT_CONTAINED ?
                            UnicodeSetStringSpan::BACK_UTF8_NOT_CONTAINED :
                            UnicodeSetStringSpan::BACK_UTF8_CONTAINED;
        UnicodeSetStringSpan strSpan(*இது, *strings_, which);
        எனில்(strSpan.needsStringSpanUTF8()) {
            திரும்பு strSpan.spanBackUTF8(reinterpret_cast<மாறிலி uint8_t*>(s), length, spanCondition);
        }
    }

    எனில்(spanCondition!=USET_SPAN_NOT_CONTAINED) {
        spanCondition=USET_SPAN_CONTAINED;  // Pin to 0/1 values.
    }

    UChar32 c;
    int32_t prev=length;
    செய் {
        U8_PREV_OR_FFFD(s, 0, length, c);
        எனில்(spanCondition!=contains(c)) {
            நிறுத்து;
        }
    } போது((prev=length)>0);
    திரும்பு prev;
}

U_NAMESPACE_END
