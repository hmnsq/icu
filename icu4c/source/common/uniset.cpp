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

#شامل "unicode/utypes.h"
#شامل "unicode/parsepos.h"
#شامل "unicode/symtable.h"
#شامل "unicode/uniset.h"
#شامل "unicode/ustring.h"
#شامل "unicode/utf8.h"
#شامل "unicode/utf16.h"
#شامل "ruleiter.h"
#شامل "cmemory.h"
#شامل "cstring.h"
#شامل "patternprops.h"
#شامل "uelement.h"
#شامل "util.h"
#شامل "uvector.h"
#شامل "charstr.h"
#شامل "ustrfmt.h"
#شامل "uassert.h"
#شامل "bmpset.h"
#شامل "unisetspan.h"

// HIGH_VALUE > all valid values. 110000 for codepoints
#define UNICODESET_بالا 0x0110000

// LOW <= all valid values. ZERO for codepoints
#define UNICODESET_کم 0x000000

/** Max list [0, 1, 2, ..., max code point, HIGH] */
constexpr int32_t MAX_LENGTH = UNICODESET_بالا + 1;

U_NAMESPACE_BEGIN

نماد_Table::~نماد_Table() {}

UOBJECT_DEFINE_RTTI_IMPLEMENTATION(UnicodeSet)

/**
 * Modify the given UChar32 variable so that it is in range, by
 * pinning values < UNICODESET_LOW to UNICODESET_LOW, and
 * pinning values > UNICODESET_HIGH-1 to UNICODESET_HIGH-1.
 * It modifies its argument in-place and also returns it.
 */
ایستا درونخطی UChar32 pinCodePoint(UChar32& c) {
    اگر (c < UNICODESET_کم) {
        c = UNICODESET_کم;
    } وگرنه اگر (c > (UNICODESET_بالا-1)) {
        c = (UNICODESET_بالا-1);
    }
    بازگشت c;
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
#شامل <stdio.h>
ایستا int32_t dbg_شمار = 0;

ایستا درونخطی تهی _dbgct(UnicodeSet* set) {
    UnicodeString str;
    set->toPattern(str, درست);
    نویسه buf[40];
    str.extract(0, 39, buf, "");
    printf("DEBUG UnicodeSet: ct 0x%08X; %d %s\n", set, ++dbg_شمار, buf);
}

ایستا درونخطی تهی _dbgdt(UnicodeSet* set) {
    UnicodeString str;
    set->toPattern(str, درست);
    نویسه buf[40];
    str.extract(0, 39, buf, "");
    printf("DEBUG UnicodeSet: dt 0x%08X; %d %s\n", set, --dbg_شمار, buf);
}

#وگرنه

#define _dbgct(set)
#define _dbgdt(set)

#endif

//----------------------------------------------------------------
// UnicodeString in UVector support
//----------------------------------------------------------------

ایستا تهی U_CALLCONV cloneUnicodeString(UElement *dst, UElement *src) {
    dst->pointer = جدید UnicodeString(*static_cast<UnicodeString*>(src->pointer));
}

ایستا int32_t U_CALLCONV compareUnicodeString(UElement t1, UElement t2) {
    ثابت UnicodeString& a = *static_cast<ثابت UnicodeString*>(t1.pointer);
    ثابت UnicodeString& b = *static_cast<ثابت UnicodeString*>(t2.pointer);
    بازگشت a.compare(b);
}

UBool UnicodeSet::hasStrings() ثابت {
    بازگشت strings_ != nullptr && !strings_->isEmpty();
}

int32_t UnicodeSet::stringsSize() ثابت {
    بازگشت strings_ == nullptr ? 0 : strings_->size();
}

UBool UnicodeSet::stringsContains(ثابت UnicodeString &s) ثابت {
    بازگشت strings_ != nullptr && strings_->contains((تهی*) &s);
}

//----------------------------------------------------------------
// Constructors &c
//----------------------------------------------------------------

/**
 * Constructs an empty set.
 */
UnicodeSet::UnicodeSet() {
    list[0] = UNICODESET_بالا;
    _dbgct(این);
}

/**
 * Constructs a set containing the given range. If <code>end >
 * start</code> then an empty set is created.
 *
 * @param start first character, inclusive, of range
 * @param end last character, inclusive, of range
 */
UnicodeSet::UnicodeSet(UChar32 start, UChar32 end) {
    list[0] = UNICODESET_بالا;
    add(start, end);
    _dbgct(این);
}

/**
 * Constructs a set that is identical to the given UnicodeSet.
 */
UnicodeSet::UnicodeSet(ثابت UnicodeSet& o) : UnicodeFilter(o) {
    *این = o;
    _dbgct(این);
}

// Copy-construct as thawed.
UnicodeSet::UnicodeSet(ثابت UnicodeSet& o, UBool /* asThawed */) : UnicodeFilter(o) {
    اگر (ensureCapacity(o.len)) {
        // *this = o except for bmpSet and stringSpan
        len = o.len;
        uprv_memcpy(list, o.list, (size_t)len*اندازه(UChar32));
        اگر (o.hasStrings()) {
            UErrorCode status = U_ZERO_ERROR;
            اگر (!allocateStrings(status) ||
                    (strings_->assign(*o.strings_, cloneUnicodeString, status), U_FAILURE(status))) {
                setToBogus();
                بازگشت;
            }
        }
        اگر (o.pat) {
            setPattern(o.pat, o.patLen);
        }
        _dbgct(این);
    }
}

/**
 * Destructs the set.
 */
UnicodeSet::~UnicodeSet() {
    _dbgdt(این); // first!
    اگر (list != پشته_List) {
        uprv_free(list);
    }
    delete bmpSet;
    اگر (buffer != پشته_List) {
        uprv_free(buffer);
    }
    delete strings_;
    delete stringSpan;
    releasePattern();
}

/**
 * Assigns this object to be a copy of another.
 */
UnicodeSet& UnicodeSet::operator=(ثابت UnicodeSet& o) U_LIFETIME_BOUND {
    بازگشت copyFrom(o, نادرست);
}

UnicodeSet& UnicodeSet::copyFrom(ثابت UnicodeSet& o, UBool asThawed) U_LIFETIME_BOUND {
    اگر (این == &o) {
        بازگشت *این;
    }
    اگر (isFrozen()) {
        بازگشت *این;
    }
    اگر (o.isBogus()) {
        setToBogus();
        بازگشت *این;
    }
    اگر (!ensureCapacity(o.len)) {
        // ensureCapacity will mark the UnicodeSet as Bogus if OOM failure happens.
        بازگشت *این;
    }
    len = o.len;
    uprv_memcpy(list, o.list, (size_t)len*اندازه(UChar32));
    اگر (o.bmpSet != nullptr && !asThawed) {
        bmpSet = جدید BMPSet(*o.bmpSet, list, len);
        اگر (bmpSet == nullptr) { // Check for memory allocation error.
            setToBogus();
            بازگشت *این;
        }
    }
    اگر (o.hasStrings()) {
        UErrorCode status = U_ZERO_ERROR;
        اگر ((strings_ == nullptr && !allocateStrings(status)) ||
                (strings_->assign(*o.strings_, cloneUnicodeString, status), U_FAILURE(status))) {
            setToBogus();
            بازگشت *این;
        }
    } وگرنه اگر (hasStrings()) {
        strings_->removeAllElements();
    }
    اگر (o.stringSpan != nullptr && strings_ != nullptr && !asThawed) {
        stringSpan = جدید UnicodeSetStringSpan(*o.stringSpan, *strings_);
        اگر (stringSpan == nullptr) { // Check for memory allocation error.
            setToBogus();
            بازگشت *این;
        }
    }
    releasePattern();
    اگر (o.pat) {
        setPattern(o.pat, o.patLen);
    }
    بازگشت *این;
}

/**
 * Returns a copy of this object.  All UnicodeMatcher objects have
 * to support cloning in order to allow classes using
 * UnicodeMatchers, such as Transliterator, to implement cloning.
 */
UnicodeSet* UnicodeSet::clone() ثابت {
    بازگشت جدید UnicodeSet(*این);
}

UnicodeSet *UnicodeSet::cloneAsThawed() ثابت {
    بازگشت جدید UnicodeSet(*این, درست);
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
منطقی UnicodeSet::operator==(ثابت UnicodeSet& o) ثابت {
    اگر (len != o.len) بازگشت نادرست;
    برای (int32_t i = 0; i < len; ++i) {
        اگر (list[i] != o.list[i]) بازگشت نادرست;
    }
    اگر (hasStrings() != o.hasStrings()) { بازگشت نادرست; }
    اگر (hasStrings() && *strings_ != *o.strings_) بازگشت نادرست;
    بازگشت درست;
}

/**
 * Returns the hash code value for this set.
 *
 * @return the hash code value for this set.
 * @see Object#hashCode()
 */
int32_t UnicodeSet::hashCode() ثابت {
    uint32_t result = static_cast<uint32_t>(len);
    برای (int32_t i = 0; i < len; ++i) {
        result *= 1000003u;
        result += list[i];
    }
    بازگشت static_cast<int32_t>(result);
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
int32_t UnicodeSet::size() ثابت {
    int32_t n = 0;
    int32_t count = get_Range_شمار();
    برای (int32_t i = 0; i < count; ++i) {
        n += getRangeEnd(i) - getRangeStart(i) + 1;
    }
    بازگشت n + stringsSize();
}

/**
 * Returns <tt>true</tt> if this set contains no elements.
 *
 * @return <tt>true</tt> if this set contains no elements.
 */
UBool UnicodeSet::isEmpty() ثابت {
    بازگشت len == 1 && !hasStrings();
}

/**
 * Returns true if this set contains the given character.
 * @param c character to be checked for containment
 * @return true if the test condition is met
 */
UBool UnicodeSet::contains(UChar32 c) ثابت {
    // Set i to the index of the start item greater than ch
    // We know we will terminate without length test!
    // LATER: for large sets, add binary search
    //int32_t i = -1;
    //for (;;) {
    //    if (c < list[++i]) break;
    //}
    اگر (bmpSet != nullptr) {
        بازگشت bmpSet->contains(c);
    }
    اگر (stringSpan != nullptr) {
        بازگشت stringSpan->contains(c);
    }
    اگر (c >= UNICODESET_بالا) { // Don't need to check LOW bound
        بازگشت نادرست;
    }
    int32_t i = findCodePoint(c);
    بازگشت i & 1; // return true if odd
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
int32_t UnicodeSet::findCodePoint(UChar32 c) ثابت {
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
    اگر (c < list[0])
        بازگشت 0;
    // High runner test.  c is often after the last range, so an
    // initial check for this condition pays off.
    int32_t lo = 0;
    int32_t hi = len - 1;
    اگر (lo >= hi || c >= list[hi-1])
        بازگشت hi;
    // invariant: c >= list[lo]
    // invariant: c < list[hi]
    برای (;;) {
        int32_t i = (lo + hi) >> 1;
        اگر (i == lo) {
            بشکن; // Found!
        } وگرنه اگر (c < list[i]) {
            hi = i;
        } وگرنه {
            lo = i;
        }
    }
    بازگشت hi;
}

/**
 * Returns true if this set contains every character
 * of the given range.
 * @param start first character, inclusive, of the range
 * @param end last character, inclusive, of the range
 * @return true if the test condition is met
 */
UBool UnicodeSet::contains(UChar32 start, UChar32 end) ثابت {
    //int32_t i = -1;
    //for (;;) {
    //    if (start < list[++i]) break;
    //}
    int32_t i = findCodePoint(start);
    بازگشت ((i & 1) != 0 && end < list[i]);
}

/**
 * Returns <tt>true</tt> if this set contains the given
 * multicharacter string.
 * @param s string to be checked for containment
 * @return <tt>true</tt> if this set contains the specified string
 */
UBool UnicodeSet::contains(ثابت UnicodeString& s) ثابت {
    int32_t cp = getSingleCP(s);
    اگر (cp < 0) {
        بازگشت stringsContains(s);
    } وگرنه {
        بازگشت contains(static_cast<UChar32>(cp));
    }
}

/**
 * Returns true if this set contains all the characters and strings
 * of the given set.
 * @param c set to be checked for containment
 * @return true if the test condition is met
 */
UBool UnicodeSet::containsAll(ثابت UnicodeSet& c) ثابت {
    // The specified set is a subset if all of its pairs are contained in
    // this set.  It's possible to code this more efficiently in terms of
    // direct manipulation of the inversion lists if the need arises.
    int32_t n = c.get_Range_شمار();
    برای (صحیح i=0; i<n; ++i) {
        اگر (!contains(c.getRangeStart(i), c.getRangeEnd(i))) {
            بازگشت نادرست;
        }
    }
    بازگشت !c.hasStrings() || (strings_ != nullptr && strings_->containsAll(*c.strings_));
}

/**
 * Returns true if this set contains all the characters
 * of the given string.
 * @param s string containing characters to be checked for containment
 * @return true if the test condition is met
 */
UBool UnicodeSet::containsAll(ثابت UnicodeString& s) ثابت {
    بازگشت span(s.getBuffer(), s.length(), USET_SPAN_CONTAINED) == s.length();
}

/**
 * Returns true if this set contains none of the characters
 * of the given range.
 * @param start first character, inclusive, of the range
 * @param end last character, inclusive, of the range
 * @return true if the test condition is met
 */
UBool UnicodeSet::containsNone(UChar32 start, UChar32 end) ثابت {
    //int32_t i = -1;
    //for (;;) {
    //    if (start < list[++i]) break;
    //}
    int32_t i = findCodePoint(start);
    بازگشت ((i & 1) == 0 && end < list[i]);
}

/**
 * Returns true if this set contains none of the characters and strings
 * of the given set.
 * @param c set to be checked for containment
 * @return true if the test condition is met
 */
UBool UnicodeSet::containsNone(ثابت UnicodeSet& c) ثابت {
    // The specified set is a subset if all of its pairs are contained in
    // this set.  It's possible to code this more efficiently in terms of
    // direct manipulation of the inversion lists if the need arises.
    int32_t n = c.get_Range_شمار();
    برای (int32_t i=0; i<n; ++i) {
        اگر (!containsNone(c.getRangeStart(i), c.getRangeEnd(i))) {
            بازگشت نادرست;
        }
    }
    بازگشت strings_ == nullptr || !c.hasStrings() || strings_->containsNone(*c.strings_);
}

/**
 * Returns true if this set contains none of the characters
 * of the given string.
 * @param s string containing characters to be checked for containment
 * @return true if the test condition is met
 */
UBool UnicodeSet::containsNone(ثابت UnicodeString& s) ثابت {
    بازگشت span(s.getBuffer(), s.length(), USET_SPAN_NOT_CONTAINED) == s.length();
}

/**
 * Returns <tt>true</tt> if this set contains any character whose low byte
 * is the given value.  This is used by <tt>RuleBasedTransliterator</tt> for
 * indexing.
 */
UBool UnicodeSet::matches_Index_مقدار(uint8_t v) ثابت {
    /* The index value v, in the range [0,255], is contained in this set if
     * it is contained in any pair of this set.  Pairs either have the high
     * bytes equal, or unequal.  If the high bytes are equal, then we have
     * aaxx..aayy, where aa is the high byte.  Then v is contained if xx <=
     * v <= yy.  If the high bytes are unequal we have aaxx..bbyy, bb>aa.
     * Then v is contained if xx <= v || v <= yy.  (This is identical to the
     * time zone month containment logic.)
     */
    int32_t i;
    int32_t range_شمار=get_Range_شمار();
    برای (i=0; i<range_شمار; ++i) {
        UChar32 کم = getRangeStart(i);
        UChar32 بالا = getRangeEnd(i);
        اگر ((کم & ~0xFF) == (بالا & ~0xFF)) {
            اگر ((کم & 0xFF) <= v && v <= (بالا & 0xFF)) {
                بازگشت درست;
            }
        } وگرنه اگر ((کم & 0xFF) <= v || v <= (بالا & 0xFF)) {
            بازگشت درست;
        }
    }
    اگر (hasStrings()) {
        برای (i=0; i<strings_->size(); ++i) {
            ثابت UnicodeString& s = *static_cast<ثابت UnicodeString*>(strings_->elementAt(i));
            اگر (s.isEmpty()) {
                ادامه;  // skip the empty string
            }
            UChar32 c = s.char32At(0);
            اگر ((c & 0xFF) == v) {
                بازگشت درست;
            }
        }
    }
    بازگشت نادرست;
}

/**
 * Implementation of UnicodeMatcher::matches().  Always matches the
 * longest possible multichar string.
 */
UMatchDegree UnicodeSet::matches(ثابت Replaceable& text,
                                 int32_t& offset,
                                 int32_t limit,
                                 UBool incremental) {
    اگر (offset == limit) {
        اگر (contains(U_ETHER)) {
            بازگشت incremental ? U_PARTIAL_MATCH : U_MATCH;
        } وگرنه {
            بازگشت U_MISMATCH;
        }
    } وگرنه {
        اگر (hasStrings()) { // try strings first

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
            int32_t بالا_Water_Length = 0;

            برای (i=0; i<strings_->size(); ++i) {
                ثابت UnicodeString& trial = *static_cast<ثابت UnicodeString*>(strings_->elementAt(i));
                اگر (trial.isEmpty()) {
                    ادامه;  // skip the empty string
                }

                char16_t c = trial.charAt(forward ? 0 : trial.length() - 1);

                // Strings are sorted, so we can optimize in the
                // forward direction.
                اگر (forward && c > firstChar) بشکن;
                اگر (c != firstChar) ادامه;

                int32_t matchLen = matchRest(text, offset, limit, trial);

                اگر (incremental) {
                    int32_t maxLen = forward ? limit-offset : offset-limit;
                    اگر (matchLen == maxLen) {
                        // We have successfully matched but only up to limit.
                        بازگشت U_PARTIAL_MATCH;
                    }
                }

                اگر (matchLen == trial.length()) {
                    // We have successfully matched the whole string.
                    اگر (matchLen > بالا_Water_Length) {
                        بالا_Water_Length = matchLen;
                    }
                    // In the forward direction we know strings
                    // are sorted so we can bail early.
                    اگر (forward && matchLen < بالا_Water_Length) {
                        بشکن;
                    }
                    ادامه;
                }
            }

            // We've checked all strings without a partial match.
            // If we have full matches, return the longest one.
            اگر (بالا_Water_Length != 0) {
                offset += forward ? بالا_Water_Length : -بالا_Water_Length;
                بازگشت U_MATCH;
            }
        }
        بازگشت UnicodeFilter::matches(text, offset, limit, incremental);
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
int32_t UnicodeSet::matchRest(ثابت Replaceable& text,
                              int32_t start, int32_t limit,
                              ثابت UnicodeString& s) {
    int32_t i;
    int32_t maxLen;
    int32_t slen = s.length();
    اگر (start < limit) {
        maxLen = limit - start;
        اگر (maxLen > slen) maxLen = slen;
        برای (i = 1; i < maxLen; ++i) {
            اگر (text.charAt(start + i) != s.charAt(i)) بازگشت 0;
        }
    } وگرنه {
        maxLen = start - limit;
        اگر (maxLen > slen) maxLen = slen;
        --slen; // <=> slen = s.length() - 1;
        برای (i = 1; i < maxLen; ++i) {
            اگر (text.charAt(start - i) != s.charAt(slen - i)) بازگشت 0;
        }
    }
    بازگشت maxLen;
}

/**
 * Implement of UnicodeMatcher
 */
تهی UnicodeSet::addMatchSetTo(UnicodeSet& toUnionTo) ثابت {
    toUnionTo.addAll(*این);
}

/**
 * Returns the index of the given character within this set, where
 * the set is ordered by ascending code point.  If the character
 * is not in this set, return -1.  The inverse of this method is
 * <code>charAt()</code>.
 * @return an index from 0..size()-1, or -1
 */
int32_t UnicodeSet::indexOf(UChar32 c) ثابت {
    اگر (c < MIN_مقدار || c > MAX_مقدار) {
        بازگشت -1;
    }
    int32_t i = 0;
    int32_t n = 0;
    برای (;;) {
        UChar32 start = list[i++];
        اگر (c < start) {
            بازگشت -1;
        }
        UChar32 limit = list[i++];
        اگر (c < limit) {
            بازگشت n + c - start;
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
UChar32 UnicodeSet::charAt(int32_t index) ثابت {
    اگر (index >= 0) {
        // len2 is the largest even integer <= len, that is, it is len
        // for even values and len-1 for odd values.  With odd values
        // the last entry is UNICODESET_HIGH.
        int32_t len2 = len & ~1;
        برای (int32_t i=0; i < len2;) {
            UChar32 start = list[i++];
            int32_t count = list[i++] - start;
            اگر (index < count) {
                بازگشت static_cast<UChar32>(start + index);
            }
            index -= count;
        }
    }
    بازگشت static_cast<UChar32>(-1);
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
    بازگشت *این;
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
    اگر (pinCodePoint(start) < pinCodePoint(end)) {
        UChar32 limit = end + 1;
        // Fast path for adding a new range after the last one.
        // Odd list length: [..., lastStart, lastLimit, HIGH]
        اگر ((len & 1) != 0) {
            // If the list is empty, set lastLimit low enough to not be adjacent to 0.
            UChar32 lastLimit = len == 1 ? -2 : list[len - 2];
            اگر (lastLimit <= start && !isFrozen() && !isBogus()) {
                اگر (lastLimit == start) {
                    // Extend the last range.
                    list[len - 2] = limit;
                    اگر (limit == UNICODESET_بالا) {
                        --len;
                    }
                } وگرنه {
                    list[len - 1] = start;
                    اگر (limit < UNICODESET_بالا) {
                        اگر (ensureCapacity(len + 2)) {
                            list[len++] = limit;
                            list[len++] = UNICODESET_بالا;
                        }
                    } وگرنه {  // limit == UNICODESET_HIGH
                        اگر (ensureCapacity(len + 1)) {
                            list[len++] = UNICODESET_بالا;
                        }
                    }
                }
                releasePattern();
                بازگشت *این;
            }
        }
        // This is slow. Could be much faster using findCodePoint(start)
        // and modifying the list, dealing with adjacent & overlapping ranges.
        UChar32 range[3] = { start, limit, UNICODESET_بالا };
        add(range, 2, 0);
    } وگرنه اگر (start == end) {
        add(start);
    }
    بازگشت *این;
}

// #define DEBUG_US_ADD

#ifdef DEBUG_US_ADD
#شامل <stdio.h>
تهی dump(UChar32 c) {
    اگر (c <= 0xFF) {
        printf("%c", (نویسه)c);
    } وگرنه {
        printf("U+%04X", c);
    }
}
تهی dump(ثابت UChar32* list, int32_t len) {
    printf("[");
    برای (int32_t i=0; i<len; ++i) {
        اگر (i != 0) printf(", ");
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
    اگر ((i & 1) != 0  || isFrozen() || isBogus()) بازگشت *این;

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

    اگر (c == list[i]-1) {
        // c is before start of next range
        list[i] = c;
        // if we touched the HIGH mark, then add a new one
        اگر (c == (UNICODESET_بالا - 1)) {
            اگر (!ensureCapacity(len+1)) {
                // ensureCapacity will mark the object as Bogus if OOM failure happens.
                بازگشت *این;
            }
            list[len++] = UNICODESET_بالا;
        }
        اگر (i > 0 && c == list[i-1]) {
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
            وقتی (src < srclimit) *(dst++) = *(src++);

            len -= 2;
        }
    }

    وگرنه اگر (i > 0 && c == list[i-1]) {
        // c is after end of prior range
        list[i-1]++;
        // no need to check for collapse here
    }

    وگرنه {
        // At this point we know the new char is not adjacent to
        // any existing ranges, and it is not 10FFFF.


        // [..., start_k-1, limit_k-1, start_k, limit_k, ..., HIGH]
        //                             ^
        //                             list[i]

        // [..., start_k-1, limit_k-1, c, c+1, start_k, limit_k, ..., HIGH]
        //                             ^
        //                             list[i]

        اگر (!ensureCapacity(len+2)) {
            // ensureCapacity will mark the object as Bogus if OOM failure happens.
            بازگشت *این;
        }

        UChar32 *p = list + i;
        uprv_memmove(p + 2, p, (len - i) * اندازه(*p));
        list[i] = c;
        list[i+1] = c+1;
        len += 2;
    }

#ifdef DEBUG_US_ADD
    dump(list, len);
    printf("\n");

    برای (i=1; i<len; ++i) {
        اگر (list[i] <= list[i-1]) {
            // Corrupt array!
            printf("ERROR: list has been corrupted\n");
            exit(1);
        }
    }
#endif

    releasePattern();
    بازگشت *این;
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
UnicodeSet& UnicodeSet::add(ثابت UnicodeString& s) U_LIFETIME_BOUND {
    اگر (isFrozen() || isBogus()) بازگشت *این;
    int32_t cp = getSingleCP(s);
    اگر (cp < 0) {
        اگر (!stringsContains(s)) {
            _add(s);
            releasePattern();
        }
    } وگرنه {
        add(static_cast<UChar32>(cp));
    }
    بازگشت *این;
}

/**
 * Adds the given string, in order, to 'strings_'.  The given string
 * must have been checked by the caller to not already be in 'strings_'.
 */
تهی UnicodeSet::_add(ثابت UnicodeString& s) {
    اگر (isFrozen() || isBogus()) {
        بازگشت;
    }
    UErrorCode ec = U_ZERO_ERROR;
    اگر (strings_ == nullptr && !allocateStrings(ec)) {
        setToBogus();
        بازگشت;
    }
    LocalPointer<UnicodeString> t(جدید UnicodeString(s));
    اگر (t.isNull()) { // Check for memory allocation error.
        setToBogus();
        بازگشت;
    }
    strings_->sorted_درج(t.orphan(), compareUnicodeString, ec);
    اگر (U_FAILURE(ec)) {
        setToBogus();
    }
}

/**
 * @return a code point IF the string consists of a single one.
 * otherwise returns -1.
 * @param string to test
 */
int32_t UnicodeSet::getSingleCP(ثابت UnicodeString& s) {
    int32_t sLength = s.length();
    اگر (sLength == 1) بازگشت s.charAt(0);
    اگر (sLength == 2) {
        UChar32 cp = s.char32At(0);
        اگر (cp > 0xFFFF) { // is surrogate pair
            بازگشت cp;
        }
    }
    بازگشت -1;
}

/**
 * Adds each of the characters in this string to the set. Thus "ch" => {"c", "h"}
 * If this set already any particular character, it has no effect on that character.
 * @param the source string
 * @return the modified set, for chaining
 */
UnicodeSet& UnicodeSet::addAll(ثابت UnicodeString& s) U_LIFETIME_BOUND {
    UChar32 cp;
    برای (int32_t i = 0; i < s.length(); i += U16_LENGTH(cp)) {
        cp = s.char32At(i);
        add(cp);
    }
    بازگشت *این;
}

/**
 * Retains EACH of the characters in this string. Note: "ch" == {"c", "h"}
 * If this set already any particular character, it has no effect on that character.
 * @param the source string
 * @return the modified set, for chaining
 */
UnicodeSet& UnicodeSet::retainAll(ثابت UnicodeString& s) U_LIFETIME_BOUND {
    UnicodeSet set;
    set.addAll(s);
    retainAll(set);
    بازگشت *این;
}

/**
 * Complement EACH of the characters in this string. Note: "ch" == {"c", "h"}
 * If this set already any particular character, it has no effect on that character.
 * @param the source string
 * @return the modified set, for chaining
 */
UnicodeSet& UnicodeSet::complementAll(ثابت UnicodeString& s) U_LIFETIME_BOUND {
    UnicodeSet set;
    set.addAll(s);
    complementAll(set);
    بازگشت *این;
}

/**
 * Remove EACH of the characters in this string. Note: "ch" == {"c", "h"}
 * If this set already any particular character, it has no effect on that character.
 * @param the source string
 * @return the modified set, for chaining
 */
UnicodeSet& UnicodeSet::removeAll(ثابت UnicodeString& s) U_LIFETIME_BOUND {
    UnicodeSet set;
    set.addAll(s);
    removeAll(set);
    بازگشت *این;
}

UnicodeSet& UnicodeSet::removeAllStrings() U_LIFETIME_BOUND {
    اگر (!isFrozen() && hasStrings()) {
        strings_->removeAllElements();
        releasePattern();
    }
    بازگشت *این;
}


/**
 * Makes a set from a multicharacter string. Thus "ch" => {"ch"}
 * <br><b>Warning: you cannot add an empty string ("") to a UnicodeSet.</b>
 * @param the source string
 * @return a newly created set containing the given string
 */
UnicodeSet* U_EXPORT2 UnicodeSet::createFrom(ثابت UnicodeString& s) {
    UnicodeSet *set = جدید UnicodeSet();
    اگر (set != nullptr) { // Check for memory allocation error.
        set->add(s);
    }
    بازگشت set;
}


/**
 * Makes a set from each of the characters in the string. Thus "ch" => {"c", "h"}
 * @param the source string
 * @return a newly created set containing the given characters
 */
UnicodeSet* U_EXPORT2 UnicodeSet::createFromAll(ثابت UnicodeString& s) {
    UnicodeSet *set = جدید UnicodeSet();
    اگر (set != nullptr) { // Check for memory allocation error.
        set->addAll(s);
    }
    بازگشت set;
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
    اگر (pinCodePoint(start) <= pinCodePoint(end)) {
        UChar32 range[3] = { start, end+1, UNICODESET_بالا };
        retain(range, 2, 0);
    } وگرنه {
        clear();
    }
    بازگشت *این;
}

UnicodeSet& UnicodeSet::retain(UChar32 c) U_LIFETIME_BOUND {
    بازگشت retain(c, c);
}

UnicodeSet& UnicodeSet::retain(ثابت UnicodeString &s) U_LIFETIME_BOUND {
    اگر (isFrozen() || isBogus()) { بازگشت *این; }
    UChar32 cp = getSingleCP(s);
    اگر (cp < 0) {
        منطقی isIn = stringsContains(s);
        // Check for getRangeCount() first to avoid somewhat-expensive size()
        // when there are single code points.
        اگر (isIn && get_Range_شمار() == 0 && size() == 1) {
            بازگشت *این;
        }
        clear();
        اگر (isIn) {
            _add(s);
        }
    } وگرنه {
        retain(cp, cp);
    }
    بازگشت *این;
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
    اگر (pinCodePoint(start) <= pinCodePoint(end)) {
        UChar32 range[3] = { start, end+1, UNICODESET_بالا };
        retain(range, 2, 2);
    }
    بازگشت *این;
}

/**
 * Removes the specified character from this set if it is present.
 * The set will not contain the specified range once the call
 * returns.
 */
UnicodeSet& UnicodeSet::remove(UChar32 c) U_LIFETIME_BOUND {
    بازگشت remove(c, c);
}

/**
 * Removes the specified string from this set if it is present.
 * The set will not contain the specified character once the call
 * returns.
 * @param the source string
 * @return the modified set, for chaining
 */
UnicodeSet& UnicodeSet::remove(ثابت UnicodeString& s) U_LIFETIME_BOUND {
    اگر (isFrozen() || isBogus()) بازگشت *این;
    int32_t cp = getSingleCP(s);
    اگر (cp < 0) {
        اگر (strings_ != nullptr && strings_->removeElement((تهی*) &s)) {
            releasePattern();
        }
    } وگرنه {
        remove(static_cast<UChar32>(cp), static_cast<UChar32>(cp));
    }
    بازگشت *این;
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
    اگر (isFrozen() || isBogus()) {
        بازگشت *این;
    }
    اگر (pinCodePoint(start) <= pinCodePoint(end)) {
        UChar32 range[3] = { start, end+1, UNICODESET_بالا };
        exclusiveOr(range, 2, 0);
    }
    releasePattern();
    بازگشت *این;
}

UnicodeSet& UnicodeSet::complement(UChar32 c) U_LIFETIME_BOUND {
    بازگشت complement(c, c);
}

/**
 * This is equivalent to
 * <code>complement(MIN_VALUE, MAX_VALUE)</code>.
 */
UnicodeSet& UnicodeSet::complement() U_LIFETIME_BOUND {
    اگر (isFrozen() || isBogus()) {
        بازگشت *این;
    }
    اگر (list[0] == UNICODESET_کم) {
        uprv_memmove(list, list + 1, (size_t)(len-1)*اندازه(UChar32));
        --len;
    } وگرنه {
        اگر (!ensureCapacity(len+1)) {
            بازگشت *این;
        }
        uprv_memmove(list + 1, list, (size_t)len*اندازه(UChar32));
        list[0] = UNICODESET_کم;
        ++len;
    }
    releasePattern();
    بازگشت *این;
}

/**
 * Complement the specified string in this set.
 * The set will not contain the specified string once the call
 * returns.
 *
 * @param s the string to complement
 * @return this object, for chaining
 */
UnicodeSet& UnicodeSet::complement(ثابت UnicodeString& s) U_LIFETIME_BOUND {
    اگر (isFrozen() || isBogus()) بازگشت *این;
    int32_t cp = getSingleCP(s);
    اگر (cp < 0) {
        اگر (stringsContains(s)) {
            strings_->removeElement((تهی*) &s);
        } وگرنه {
            _add(s);
        }
        releasePattern();
    } وگرنه {
        complement(static_cast<UChar32>(cp), static_cast<UChar32>(cp));
    }
    بازگشت *این;
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
UnicodeSet& UnicodeSet::addAll(ثابت UnicodeSet& c) U_LIFETIME_BOUND {
    اگر ( c.len>0 && c.list!=nullptr ) {
        add(c.list, c.len, 0);
    }

    // Add strings in order
    اگر ( c.strings_!=nullptr ) {
        برای (int32_t i=0; i<c.strings_->size(); ++i) {
            ثابت UnicodeString* s = static_cast<ثابت UnicodeString*>(c.strings_->elementAt(i));
            اگر (!stringsContains(*s)) {
                _add(*s);
            }
        }
    }
    بازگشت *این;
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
UnicodeSet& UnicodeSet::retainAll(ثابت UnicodeSet& c) U_LIFETIME_BOUND {
    اگر (isFrozen() || isBogus()) {
        بازگشت *این;
    }
    retain(c.list, c.len, 0);
    اگر (hasStrings()) {
        اگر (!c.hasStrings()) {
            strings_->removeAllElements();
        } وگرنه {
            strings_->retainAll(*c.strings_);
        }
    }
    بازگشت *این;
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
UnicodeSet& UnicodeSet::removeAll(ثابت UnicodeSet& c) U_LIFETIME_BOUND {
    اگر (isFrozen() || isBogus()) {
        بازگشت *این;
    }
    retain(c.list, c.len, 2);
    اگر (hasStrings() && c.hasStrings()) {
        strings_->removeAll(*c.strings_);
    }
    بازگشت *این;
}

/**
 * Complements in this set all elements contained in the specified
 * set.  Any character in the other set will be removed if it is
 * in this set, or will be added if it is not in this set.
 *
 * @param c set that defines which elements will be xor'ed from
 *          this set.
 */
UnicodeSet& UnicodeSet::complementAll(ثابت UnicodeSet& c) U_LIFETIME_BOUND {
    اگر (isFrozen() || isBogus()) {
        بازگشت *این;
    }
    exclusiveOr(c.list, c.len, 0);

    اگر (c.strings_ != nullptr) {
        برای (int32_t i=0; i<c.strings_->size(); ++i) {
            تهی* e = c.strings_->elementAt(i);
            اگر (strings_ == nullptr || !strings_->removeElement(e)) {
                _add(*static_cast<ثابت UnicodeString*>(e));
            }
        }
    }
    بازگشت *این;
}

/**
 * Removes all of the elements from this set.  This set will be
 * empty after this call returns.
 */
UnicodeSet& UnicodeSet::clear() U_LIFETIME_BOUND {
    اگر (isFrozen()) {
        بازگشت *این;
    }
    list[0] = UNICODESET_بالا;
    len = 1;
    releasePattern();
    اگر (strings_ != nullptr) {
        strings_->removeAllElements();
    }
    // Remove bogus
    fFlags = 0;
    بازگشت *این;
}

/**
 * Iteration method that returns the number of ranges contained in
 * this set.
 * @see #getRangeStart
 * @see #getRangeEnd
 */
int32_t UnicodeSet::get_Range_شمار() ثابت {
    بازگشت len/2;
}

/**
 * Iteration method that returns the first character in the
 * specified range of this set.
 * @see #getRangeCount
 * @see #getRangeEnd
 */
UChar32 UnicodeSet::getRangeStart(int32_t index) ثابت {
    بازگشت list[index*2];
}

/**
 * Iteration method that returns the last character in the
 * specified range of this set.
 * @see #getRangeStart
 * @see #getRangeEnd
 */
UChar32 UnicodeSet::getRangeEnd(int32_t index) ثابت {
    بازگشت list[index*2 + 1] - 1;
}

ثابت UnicodeString* UnicodeSet::getString(int32_t index) ثابت U_LIFETIME_BOUND {
    بازگشت static_cast<ثابت UnicodeString*>(strings_->elementAt(index));
}

/**
 * Reallocate this objects internal structures to take up the least
 * possible space, without changing this object's value.
 */
UnicodeSet& UnicodeSet::compact() U_LIFETIME_BOUND {
    اگر (isFrozen() || isBogus()) {
        بازگشت *این;
    }
    // Delete buffer first to defragment memory less.
    اگر (buffer != پشته_List) {
        uprv_free(buffer);
        buffer = nullptr;
        bufferCapacity = 0;
    }
    اگر (list == پشته_List) {
        // pass
    } وگرنه اگر (len <= INITIAL_CAPACITY) {
        uprv_memcpy(پشته_List, list, len * اندازه(UChar32));
        uprv_free(list);
        list = پشته_List;
        capacity = INITIAL_CAPACITY;
    } وگرنه اگر ((len + 7) < capacity) {
        // If we have more than a little unused capacity, shrink it to len.
        UChar32* temp = static_cast<UChar32*>(uprv_realloc(list, اندازه(UChar32) * len));
        اگر (temp) {
            list = temp;
            capacity = len;
        }
        // else what the heck happened?! We allocated less memory!
        // Oh well. We'll keep our original array.
    }
    اگر (strings_ != nullptr && strings_->isEmpty()) {
        delete strings_;
        strings_ = nullptr;
    }
    بازگشت *این;
}

#ifdef DEBUG_SERIALIZE
#شامل <stdio.h>
#endif

/**
 * Deserialize constructor.
 */
UnicodeSet::UnicodeSet(ثابت uint16_t data[], int32_t dataLen, ESerialization serialization,
                       UErrorCode &ec) {

  اگر(U_FAILURE(ec)) {
    setToBogus();
    بازگشت;
  }

  اگر( (serialization != kSerialized)
      || (data==nullptr)
      || (dataLen < 1)) {
    ec = U_ILLEGAL_ARGUMENT_ERROR;
    setToBogus();
    بازگشت;
  }

  // bmp?
  int32_t headerSize = ((data[0]&0x8000)) ?2:1;
  int32_t bmpLength = (headerSize==1)?data[0]:data[1];

  int32_t newLength = (((data[0]&0x7FFF)-bmpLength)/2)+bmpLength;
#ifdef DEBUG_SERIALIZE
  printf("dataLen %d headerSize %d bmpLen %d len %d. data[0]=%X/%X/%X/%X\n", dataLen,headerSize,bmpLength,newLength, data[0],data[1],data[2],data[3]);
#endif
  اگر(!ensureCapacity(newLength + 1)) {  // +1 for HIGH
    بازگشت;
  }
  // copy bmp
  int32_t i;
  برای(i = 0; i< bmpLength;i++) {
    list[i] = data[i+headerSize];
#ifdef DEBUG_SERIALIZE
    printf("<<16@%d[%d] %X\n", i+headerSize, i, list[i]);
#endif
  }
  // copy smp
  برای(i=bmpLength;i<newLength;i++) {
    list[i] = (static_cast<UChar32>(data[headerSize + bmpLength + (i - bmpLength) * 2 + 0]) << 16) +
               static_cast<UChar32>(data[headerSize + bmpLength + (i - bmpLength) * 2 + 1]);
#ifdef DEBUG_SERIALIZE
    printf("<<32@%d+[%d] %lX\n", headerSize+bmpLength+i, i, list[i]);
#endif
  }
  U_ASSERT(i == newLength);
  اگر (i == 0 || list[i - 1] != UNICODESET_بالا) {
    list[i++] = UNICODESET_بالا;
  }
  len = i;
}


int32_t UnicodeSet::serialize(uint16_t *dest, int32_t destCapacity, UErrorCode& ec) ثابت {
    int32_t bmpLength, length, destLength;

    اگر (U_FAILURE(ec)) {
        بازگشت 0;
    }

    اگر (destCapacity<0 || (destCapacity>0 && dest==nullptr)) {
        ec=U_ILLEGAL_ARGUMENT_ERROR;
        بازگشت 0;
    }

    /* count necessary 16-bit units */
    length=این->len-1; // Subtract 1 to ignore final UNICODESET_HIGH
    // assert(length>=0);
    اگر (length==0) {
        /* empty set */
        اگر (destCapacity>0) {
            *dest=0;
        } وگرنه {
            ec=U_BUFFER_OVERFLOW_ERROR;
        }
        بازگشت 1;
    }
    /* now length>0 */

    اگر (این->list[length-1]<=0xffff) {
        /* all BMP */
        bmpLength=length;
    } وگرنه اگر (این->list[0]>=0x10000) {
        /* all supplementary */
        bmpLength=0;
        length*=2;
    } وگرنه {
        /* some BMP, some supplementary */
        برای (bmpLength=0; bmpLength<length && این->list[bmpLength]<=0xffff; ++bmpLength) {}
        length=bmpLength+2*(length-bmpLength);
    }
#ifdef DEBUG_SERIALIZE
    printf(">> bmpLength%d length%d len%d\n", bmpLength, length, len);
#endif
    /* length: number of 16-bit array units */
    اگر (length>0x7fff) {
        /* there are only 15 bits for the length in the first serialized word */
        ec=U_INDEX_OUTOFBOUNDS_ERROR;
        بازگشت 0;
    }

    /*
     * total serialized length:
     * number of 16-bit array units (length) +
     * 1 length unit (always) +
     * 1 bmpLength unit (if there are supplementary values)
     */
    destLength=length+((length>bmpLength)?2:1);
    اگر (destLength<=destCapacity) {
        ثابت UChar32 *p;
        int32_t i;

#ifdef DEBUG_SERIALIZE
        printf("writeHdr\n");
#endif
        *dest = static_cast<uint16_t>(length);
        اگر (length>bmpLength) {
            *dest|=0x8000;
            *++dest = static_cast<uint16_t>(bmpLength);
        }
        ++dest;

        /* write the BMP part of the array */
        p=این->list;
        برای (i=0; i<bmpLength; ++i) {
#ifdef DEBUG_SERIALIZE
          printf("writebmp: %x\n", (صحیح)*p);
#endif
            *dest++ = static_cast<uint16_t>(*p++);
        }

        /* write the supplementary part of the array */
        برای (; i<length; i+=2) {
#ifdef DEBUG_SERIALIZE
          printf("write32: %x\n", (صحیح)*p);
#endif
            *dest++ = static_cast<uint16_t>(*p >> 16);
            *dest++ = static_cast<uint16_t>(*p++);
        }
    } وگرنه {
        ec=U_BUFFER_OVERFLOW_ERROR;
    }
    بازگشت destLength;
}

//----------------------------------------------------------------
// Implementation: Utility methods
//----------------------------------------------------------------

/**
 * Allocate our strings vector and return true if successful.
 */
UBool UnicodeSet::allocateStrings(UErrorCode &status) {
    اگر (U_FAILURE(status)) {
        بازگشت نادرست;
    }
    strings_ = جدید UVector(uprv_deleteUObject,
                          uhash_compareUnicodeString, 1, status);
    اگر (strings_ == nullptr) { // Check for memory allocation error.
        status = U_MEMORY_ALLOCATION_ERROR;
        بازگشت نادرست;
    }
    اگر (U_FAILURE(status)) {
        delete strings_;
        strings_ = nullptr;
        بازگشت نادرست;
    } 
    بازگشت درست;
}

int32_t UnicodeSet::nextCapacity(int32_t minCapacity) {
    // Grow exponentially to reduce the frequency of allocations.
    اگر (minCapacity < INITIAL_CAPACITY) {
        بازگشت minCapacity + INITIAL_CAPACITY;
    } وگرنه اگر (minCapacity <= 2500) {
        بازگشت 5 * minCapacity;
    } وگرنه {
        int32_t newCapacity = 2 * minCapacity;
        اگر (newCapacity > MAX_LENGTH) {
            newCapacity = MAX_LENGTH;
        }
        بازگشت newCapacity;
    }
}

منطقی UnicodeSet::ensureCapacity(int32_t newLen) {
    اگر (newLen > MAX_LENGTH) {
        newLen = MAX_LENGTH;
    }
    اگر (newLen <= capacity) {
        بازگشت درست;
    }
    int32_t newCapacity = nextCapacity(newLen);
    UChar32* temp = static_cast<UChar32*>(uprv_malloc(newCapacity * اندازه(UChar32)));
    اگر (temp == nullptr) {
        setToBogus(); // set the object to bogus state if an OOM failure occurred.
        بازگشت نادرست;
    }
    // Copy only the actual contents.
    uprv_memcpy(temp, list, len * اندازه(UChar32));
    اگر (list != پشته_List) {
        uprv_free(list);
    }
    list = temp;
    capacity = newCapacity;
    بازگشت درست;
}

منطقی UnicodeSet::ensureBufferCapacity(int32_t newLen) {
    اگر (newLen > MAX_LENGTH) {
        newLen = MAX_LENGTH;
    }
    اگر (newLen <= bufferCapacity) {
        بازگشت درست;
    }
    int32_t newCapacity = nextCapacity(newLen);
    UChar32* temp = static_cast<UChar32*>(uprv_malloc(newCapacity * اندازه(UChar32)));
    اگر (temp == nullptr) {
        setToBogus();
        بازگشت نادرست;
    }
    // The buffer has no contents to be copied.
    // It is always filled from scratch after this call.
    اگر (buffer != پشته_List) {
        uprv_free(buffer);
    }
    buffer = temp;
    bufferCapacity = newCapacity;
    بازگشت درست;
}

/**
 * Swap list and buffer.
 */
تهی UnicodeSet::swapBuffers() {
    // swap list and buffer
    UChar32* temp = list;
    list = buffer;
    buffer = temp;

    int32_t c = capacity;
    capacity = bufferCapacity;
    bufferCapacity = c;
}

تهی UnicodeSet::setToBogus() {
    clear(); // Remove everything in the set.
    fFlags = kIsBogus;
}

//----------------------------------------------------------------
// Implementation: Fundamental operators
//----------------------------------------------------------------

ایستا درونخطی UChar32 max(UChar32 a, UChar32 b) {
    بازگشت (a > b) ? a : b;
}

// polarity = 0, 3 is normal: x xor y
// polarity = 1, 2: x xor ~y == x === y

تهی UnicodeSet::exclusiveOr(ثابت UChar32* other, int32_t otherLen, int8_t polarity) {
    اگر (isFrozen() || isBogus()) {
        بازگشت;
    }
    اگر (!ensureBufferCapacity(len + otherLen)) {
        بازگشت;
    }

    int32_t i = 0, j = 0, k = 0;
    UChar32 a = list[i++];
    UChar32 b;
    اگر (polarity == 1 || polarity == 2) {
        b = UNICODESET_کم;
        اگر (other[j] == UNICODESET_کم) { // skip base if already LOW
            ++j;
            b = other[j];
        }
    } وگرنه {
        b = other[j++];
    }
    // simplest of all the routines
    // sort the values, discarding identicals!
    برای (;;) {
        اگر (a < b) {
            buffer[k++] = a;
            a = list[i++];
        } وگرنه اگر (b < a) {
            buffer[k++] = b;
            b = other[j++];
        } وگرنه اگر (a != UNICODESET_بالا) { // at this point, a == b
            // discard both values!
            a = list[i++];
            b = other[j++];
        } وگرنه { // DONE!
            buffer[k++] = UNICODESET_بالا;
            len = k;
            بشکن;
        }
    }
    swapBuffers();
    releasePattern();
}

// polarity = 0 is normal: x union y
// polarity = 2: x union ~y
// polarity = 1: ~x union y
// polarity = 3: ~x union ~y

تهی UnicodeSet::add(ثابت UChar32* other, int32_t otherLen, int8_t polarity) {
    اگر (isFrozen() || isBogus() || other==nullptr) {
        بازگشت;
    }
    اگر (!ensureBufferCapacity(len + otherLen)) {
        بازگشت;
    }

    int32_t i = 0, j = 0, k = 0;
    UChar32 a = list[i++];
    UChar32 b = other[j++];
    // change from xor is that we have to check overlapping pairs
    // polarity bit 1 means a is second, bit 2 means b is.
    برای (;;) {
        انتخاب (polarity) {
          حالت 0: // both first; take lower if unequal
            اگر (a < b) { // take a
                // Back up over overlapping ranges in buffer[]
                اگر (k > 0 && a <= buffer[k-1]) {
                    // Pick latter end value in buffer[] vs. list[]
                    a = max(list[i], buffer[--k]);
                } وگرنه {
                    // No overlap
                    buffer[k++] = a;
                    a = list[i];
                }
                i++; // Common if/else code factored out
                polarity ^= 1;
            } وگرنه اگر (b < a) { // take b
                اگر (k > 0 && b <= buffer[k-1]) {
                    b = max(other[j], buffer[--k]);
                } وگرنه {
                    buffer[k++] = b;
                    b = other[j];
                }
                j++;
                polarity ^= 2;
            } وگرنه { // a == b, take a, drop b
                اگر (a == UNICODESET_بالا) برو loop_end;
                // This is symmetrical; it doesn't matter if
                // we backtrack with a or b. - liu
                اگر (k > 0 && a <= buffer[k-1]) {
                    a = max(list[i], buffer[--k]);
                } وگرنه {
                    // No overlap
                    buffer[k++] = a;
                    a = list[i];
                }
                i++;
                polarity ^= 1;
                b = other[j++];
                polarity ^= 2;
            }
            بشکن;
          حالت 3: // both second; take higher if unequal, and drop other
            اگر (b <= a) { // take a
                اگر (a == UNICODESET_بالا) برو loop_end;
                buffer[k++] = a;
            } وگرنه { // take b
                اگر (b == UNICODESET_بالا) برو loop_end;
                buffer[k++] = b;
            }
            a = list[i++];
            polarity ^= 1;   // factored common code
            b = other[j++];
            polarity ^= 2;
            بشکن;
          حالت 1: // a second, b first; if b < a, overlap
            اگر (a < b) { // no overlap, take a
                buffer[k++] = a; a = list[i++]; polarity ^= 1;
            } وگرنه اگر (b < a) { // OVERLAP, drop b
                b = other[j++];
                polarity ^= 2;
            } وگرنه { // a == b, drop both!
                اگر (a == UNICODESET_بالا) برو loop_end;
                a = list[i++];
                polarity ^= 1;
                b = other[j++];
                polarity ^= 2;
            }
            بشکن;
          حالت 2: // a first, b second; if a < b, overlap
            اگر (b < a) { // no overlap, take b
                buffer[k++] = b;
                b = other[j++];
                polarity ^= 2;
            } وگرنه  اگر (a < b) { // OVERLAP, drop a
                a = list[i++];
                polarity ^= 1;
            } وگرنه { // a == b, drop both!
                اگر (a == UNICODESET_بالا) برو loop_end;
                a = list[i++];
                polarity ^= 1;
                b = other[j++];
                polarity ^= 2;
            }
            بشکن;
        }
    }
 loop_end:
    buffer[k++] = UNICODESET_بالا;    // terminate
    len = k;
    swapBuffers();
    releasePattern();
}

// polarity = 0 is normal: x intersect y
// polarity = 2: x intersect ~y == set-minus
// polarity = 1: ~x intersect y
// polarity = 3: ~x intersect ~y

تهی UnicodeSet::retain(ثابت UChar32* other, int32_t otherLen, int8_t polarity) {
    اگر (isFrozen() || isBogus()) {
        بازگشت;
    }
    اگر (!ensureBufferCapacity(len + otherLen)) {
        بازگشت;
    }

    int32_t i = 0, j = 0, k = 0;
    UChar32 a = list[i++];
    UChar32 b = other[j++];
    // change from xor is that we have to check overlapping pairs
    // polarity bit 1 means a is second, bit 2 means b is.
    برای (;;) {
        انتخاب (polarity) {
          حالت 0: // both first; drop the smaller
            اگر (a < b) { // drop a
                a = list[i++];
                polarity ^= 1;
            } وگرنه اگر (b < a) { // drop b
                b = other[j++];
                polarity ^= 2;
            } وگرنه { // a == b, take one, drop other
                اگر (a == UNICODESET_بالا) برو loop_end;
                buffer[k++] = a;
                a = list[i++];
                polarity ^= 1;
                b = other[j++];
                polarity ^= 2;
            }
            بشکن;
          حالت 3: // both second; take lower if unequal
            اگر (a < b) { // take a
                buffer[k++] = a;
                a = list[i++];
                polarity ^= 1;
            } وگرنه اگر (b < a) { // take b
                buffer[k++] = b;
                b = other[j++];
                polarity ^= 2;
            } وگرنه { // a == b, take one, drop other
                اگر (a == UNICODESET_بالا) برو loop_end;
                buffer[k++] = a;
                a = list[i++];
                polarity ^= 1;
                b = other[j++];
                polarity ^= 2;
            }
            بشکن;
          حالت 1: // a second, b first;
            اگر (a < b) { // NO OVERLAP, drop a
                a = list[i++];
                polarity ^= 1;
            } وگرنه اگر (b < a) { // OVERLAP, take b
                buffer[k++] = b;
                b = other[j++];
                polarity ^= 2;
            } وگرنه { // a == b, drop both!
                اگر (a == UNICODESET_بالا) برو loop_end;
                a = list[i++];
                polarity ^= 1;
                b = other[j++];
                polarity ^= 2;
            }
            بشکن;
          حالت 2: // a first, b second; if a < b, overlap
            اگر (b < a) { // no overlap, drop b
                b = other[j++];
                polarity ^= 2;
            } وگرنه  اگر (a < b) { // OVERLAP, take a
                buffer[k++] = a;
                a = list[i++];
                polarity ^= 1;
            } وگرنه { // a == b, drop both!
                اگر (a == UNICODESET_بالا) برو loop_end;
                a = list[i++];
                polarity ^= 1;
                b = other[j++];
                polarity ^= 2;
            }
            بشکن;
        }
    }
 loop_end:
    buffer[k++] = UNICODESET_بالا;    // terminate
    len = k;
    swapBuffers();
    releasePattern();
}

/**
 * Append the <code>toPattern()</code> representation of a
 * string to the given <code>StringBuffer</code>.
 */
تهی UnicodeSet::_appendToPat(UnicodeString& buf, ثابت UnicodeString& s, UBool escapeUnprintable) {
    UChar32 cp;
    برای (int32_t i = 0; i < s.length(); i += U16_LENGTH(cp)) {
        _appendToPat(buf, cp = s.char32At(i), escapeUnprintable);
    }
}

/**
 * Append the <code>toPattern()</code> representation of a
 * character to the given <code>StringBuffer</code>.
 */
تهی UnicodeSet::_appendToPat(UnicodeString& buf, UChar32 c, UBool escapeUnprintable) {
    اگر (escapeUnprintable ? ICU_Utility::isUnprintable(c) : ICU_Utility::shouldAlwaysBeEscaped(c)) {
        // Use hex escape notation (\uxxxx or \Uxxxxxxxx) for anything
        // unprintable
        ICU_Utility::escape(buf, c);
        بازگشت;
    }
    // Okay to let ':' pass through
    انتخاب (c) {
    حالت u'[':
    حالت u']':
    حالت u'-':
    حالت u'^':
    حالت u'&':
    حالت u'\\':
    حالت u'{':
    حالت u'}':
    حالت u':':
    حالت نماد_Table::نماد_REF:
        buf.append(u'\\');
        بشکن;
    پیشفرض:
        // Escape whitespace
        اگر (PatternProps::isWhiteSpace(c)) {
            buf.append(u'\\');
        }
        بشکن;
    }
    buf.append(c);
}

تهی UnicodeSet::_appendToPat(UnicodeString &result, UChar32 start, UChar32 end,
                              UBool escapeUnprintable) {
    _appendToPat(result, start, escapeUnprintable);
    اگر (start != end) {
        اگر ((start+1) != end ||
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
                                      UBool escapeUnprintable) ثابت
{
    اگر (pat != nullptr) {
        int32_t i;
        int32_t backslash_شمار = 0;
        برای (i=0; i<patLen; ) {
            UChar32 c;
            U16_NEXT(pat, i, patLen, c);
            اگر (escapeUnprintable ?
                    ICU_Utility::isUnprintable(c) : ICU_Utility::shouldAlwaysBeEscaped(c)) {
                // If the unprintable character is preceded by an odd
                // number of backslashes, then it has been escaped.
                // Before unescaping it, we delete the final
                // backslash.
                اگر ((backslash_شمار % 2) == 1) {
                    result.truncate(result.length() - 1);
                }
                ICU_Utility::escape(result, c);
                backslash_شمار = 0;
            } وگرنه {
                result.append(c);
                اگر (c == u'\\') {
                    ++backslash_شمار;
                } وگرنه {
                    backslash_شمار = 0;
                }
            }
        }
        بازگشت result;
    }

    بازگشت _generatePattern(result, escapeUnprintable);
}

/**
 * Returns a string representation of this set.  If the result of
 * calling this function is passed to a UnicodeSet constructor, it
 * will produce another set that is equal to this one.
 */
UnicodeString& UnicodeSet::toPattern(UnicodeString& result U_LIFETIME_BOUND,
                                     UBool escapeUnprintable) ثابت
{
    result.truncate(0);
    بازگشت _toPattern(result, escapeUnprintable);
}

/**
 * Generate and append a string representation of this set to result.
 * This does not use this.pat, the cleaned up copy of the string
 * passed to applyPattern().
 */
UnicodeString& UnicodeSet::_generatePattern(UnicodeString& result U_LIFETIME_BOUND,
                                            UBool escapeUnprintable) ثابت
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
    اگر (len >= 4 && list[0] == 0 && limit == len && !hasStrings()) {
        // Emit the inverse
        result.append(u'^');
        // Offsetting the inversion list index by one lets us
        // iterate over the ranges of the set complement.
        i = 1;
        --limit;
    }

    // Emit the ranges as pairs.
    وقتی (i < limit) {
        UChar32 start = list[i];  // getRangeStart()
        UChar32 end = list[i + 1] - 1;  // getRangeEnd() = range limit minus one
        اگر (!(0xd800 <= end && end <= 0xdbff)) {
            _appendToPat(result, start, end, escapeUnprintable);
            i += 2;
        } وگرنه {
            // The range ends with a lead surrogate.
            // Avoid writing what looks like a lead+trail surrogate pair.
            // 1. Postpone ranges that start with a lead surrogate code point.
            int32_t firstLead = i;
            وقتی ((i += 2) < limit && list[i] <= 0xdbff) {}
            int32_t firstAfterLead = i;
            // 2. Write following ranges that start with a trail surrogate code point.
            وقتی (i < limit && (start = list[i]) <= 0xdfff) {
                _appendToPat(result, start, list[i + 1] - 1, escapeUnprintable);
                i += 2;
            }
            // 3. Now write the postponed ranges.
            برای (صحیح j = firstLead; j < firstAfterLead; j += 2) {
                _appendToPat(result, list[j], list[j + 1] - 1, escapeUnprintable);
            }
        }
    }

    اگر (strings_ != nullptr) {
        برای (int32_t i = 0; i<strings_->size(); ++i) {
            result.append(u'{');
            _appendToPat(result,
                         *static_cast<ثابت UnicodeString*>(strings_->elementAt(i)),
                         escapeUnprintable);
            result.append(u'}');
        }
    }
    بازگشت result.append(u']');
}

/**
* Release existing cached pattern
*/
تهی UnicodeSet::releasePattern() {
    اگر (pat) {
        uprv_free(pat);
        pat = nullptr;
        patLen = 0;
    }
}

/**
* Set the new pattern to cache.
*/
تهی UnicodeSet::setPattern(ثابت char16_t *newPat, int32_t newPatLen) {
    releasePattern();
    pat = static_cast<char16_t*>(uprv_malloc((newPatLen + 1) * اندازه(char16_t)));
    اگر (pat) {
        patLen = newPatLen;
        u_memcpy(pat, newPat, patLen);
        pat[patLen] = 0;
    }
    // else we don't care if malloc failed. This was just a nice cache.
    // We can regenerate an equivalent pattern later when requested.
}

UnicodeSet* UnicodeSet::freeze() U_LIFETIME_BOUND {
    اگر(!isFrozen() && !isBogus()) {
        compact();

        // Optimize contains() and span() and similar functions.
        اگر (hasStrings()) {
            stringSpan = جدید UnicodeSetStringSpan(*این, *strings_, UnicodeSetStringSpan::ALL);
            اگر (stringSpan == nullptr) {
                setToBogus();
                بازگشت این;
            } وگرنه اگر (!stringSpan->needsStringSpanUTF16()) {
                // All strings are irrelevant for span() etc. because
                // all of each string's code points are contained in this set.
                // Do not check needsStringSpanUTF8() because UTF-8 has at most as
                // many relevant strings as UTF-16.
                // (Thus needsStringSpanUTF8() implies needsStringSpanUTF16().)
                delete stringSpan;
                stringSpan = nullptr;
            }
        }
        اگر (stringSpan == nullptr) {
            // No span-relevant strings: Optimize for code point spans.
            bmpSet=جدید BMPSet(list, len);
            اگر (bmpSet == nullptr) { // Check for memory allocation error.
                setToBogus();
            }
        }
    }
    بازگشت این;
}

int32_t UnicodeSet::span(ثابت char16_t *s, int32_t length, USetSpanCondition spanCondition) ثابت {
    اگر(length>0 && bmpSet!=nullptr) {
        بازگشت static_cast<int32_t>(bmpSet->span(s, s + length, spanCondition) - s);
    }
    اگر(length<0) {
        length=u_strlen(s);
    }
    اگر(length==0) {
        بازگشت 0;
    }
    اگر(stringSpan!=nullptr) {
        بازگشت stringSpan->span(s, length, spanCondition);
    } وگرنه اگر(hasStrings()) {
        uint32_t which= spanCondition==USET_SPAN_NOT_CONTAINED ?
                            UnicodeSetStringSpan::FWD_UTF16_NOT_CONTAINED :
                            UnicodeSetStringSpan::FWD_UTF16_CONTAINED;
        UnicodeSetStringSpan strSpan(*این, *strings_, which);
        اگر(strSpan.needsStringSpanUTF16()) {
            بازگشت strSpan.span(s, length, spanCondition);
        }
    }

    اگر(spanCondition!=USET_SPAN_NOT_CONTAINED) {
        spanCondition=USET_SPAN_CONTAINED;  // Pin to 0/1 values.
    }

    UChar32 c;
    int32_t start=0, prev=0;
    انجامبده {
        U16_NEXT(s, start, length, c);
        اگر(spanCondition!=contains(c)) {
            بشکن;
        }
    } وقتی((prev=start)<length);
    بازگشت prev;
}

int32_t UnicodeSet::spanBack(ثابت char16_t *s, int32_t length, USetSpanCondition spanCondition) ثابت {
    اگر(length>0 && bmpSet!=nullptr) {
        بازگشت static_cast<int32_t>(bmpSet->spanBack(s, s + length, spanCondition) - s);
    }
    اگر(length<0) {
        length=u_strlen(s);
    }
    اگر(length==0) {
        بازگشت 0;
    }
    اگر(stringSpan!=nullptr) {
        بازگشت stringSpan->spanBack(s, length, spanCondition);
    } وگرنه اگر(hasStrings()) {
        uint32_t which= spanCondition==USET_SPAN_NOT_CONTAINED ?
                            UnicodeSetStringSpan::BACK_UTF16_NOT_CONTAINED :
                            UnicodeSetStringSpan::BACK_UTF16_CONTAINED;
        UnicodeSetStringSpan strSpan(*این, *strings_, which);
        اگر(strSpan.needsStringSpanUTF16()) {
            بازگشت strSpan.spanBack(s, length, spanCondition);
        }
    }

    اگر(spanCondition!=USET_SPAN_NOT_CONTAINED) {
        spanCondition=USET_SPAN_CONTAINED;  // Pin to 0/1 values.
    }

    UChar32 c;
    int32_t prev=length;
    انجامبده {
        U16_PREV(s, 0, length, c);
        اگر(spanCondition!=contains(c)) {
            بشکن;
        }
    } وقتی((prev=length)>0);
    بازگشت prev;
}

int32_t UnicodeSet::spanUTF8(ثابت نویسه *s, int32_t length, USetSpanCondition spanCondition) ثابت {
    اگر(length>0 && bmpSet!=nullptr) {
        ثابت uint8_t* s0 = reinterpret_cast<ثابت uint8_t*>(s);
        بازگشت static_cast<int32_t>(bmpSet->spanUTF8(s0, length, spanCondition) - s0);
    }
    اگر(length<0) {
        length = static_cast<int32_t>(uprv_strlen(s));
    }
    اگر(length==0) {
        بازگشت 0;
    }
    اگر(stringSpan!=nullptr) {
        بازگشت stringSpan->spanUTF8(reinterpret_cast<ثابت uint8_t*>(s), length, spanCondition);
    } وگرنه اگر(hasStrings()) {
        uint32_t which= spanCondition==USET_SPAN_NOT_CONTAINED ?
                            UnicodeSetStringSpan::FWD_UTF8_NOT_CONTAINED :
                            UnicodeSetStringSpan::FWD_UTF8_CONTAINED;
        UnicodeSetStringSpan strSpan(*این, *strings_, which);
        اگر(strSpan.needsStringSpanUTF8()) {
            بازگشت strSpan.spanUTF8(reinterpret_cast<ثابت uint8_t*>(s), length, spanCondition);
        }
    }

    اگر(spanCondition!=USET_SPAN_NOT_CONTAINED) {
        spanCondition=USET_SPAN_CONTAINED;  // Pin to 0/1 values.
    }

    UChar32 c;
    int32_t start=0, prev=0;
    انجامبده {
        U8_NEXT_OR_FFFD(s, start, length, c);
        اگر(spanCondition!=contains(c)) {
            بشکن;
        }
    } وقتی((prev=start)<length);
    بازگشت prev;
}

int32_t UnicodeSet::spanBackUTF8(ثابت نویسه *s, int32_t length, USetSpanCondition spanCondition) ثابت {
    اگر(length>0 && bmpSet!=nullptr) {
        ثابت uint8_t* s0 = reinterpret_cast<ثابت uint8_t*>(s);
        بازگشت bmpSet->spanBackUTF8(s0, length, spanCondition);
    }
    اگر(length<0) {
        length = static_cast<int32_t>(uprv_strlen(s));
    }
    اگر(length==0) {
        بازگشت 0;
    }
    اگر(stringSpan!=nullptr) {
        بازگشت stringSpan->spanBackUTF8(reinterpret_cast<ثابت uint8_t*>(s), length, spanCondition);
    } وگرنه اگر(hasStrings()) {
        uint32_t which= spanCondition==USET_SPAN_NOT_CONTAINED ?
                            UnicodeSetStringSpan::BACK_UTF8_NOT_CONTAINED :
                            UnicodeSetStringSpan::BACK_UTF8_CONTAINED;
        UnicodeSetStringSpan strSpan(*این, *strings_, which);
        اگر(strSpan.needsStringSpanUTF8()) {
            بازگشت strSpan.spanBackUTF8(reinterpret_cast<ثابت uint8_t*>(s), length, spanCondition);
        }
    }

    اگر(spanCondition!=USET_SPAN_NOT_CONTAINED) {
        spanCondition=USET_SPAN_CONTAINED;  // Pin to 0/1 values.
    }

    UChar32 c;
    int32_t prev=length;
    انجامبده {
        U8_PREV_OR_FFFD(s, 0, length, c);
        اگر(spanCondition!=contains(c)) {
            بشکن;
        }
    } وقتی((prev=length)>0);
    بازگشت prev;
}

U_NAMESPACE_END
