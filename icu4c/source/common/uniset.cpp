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

#include "unicode/utypes.h"
#include "unicode/parsepos.h"
#include "unicode/symtable.h"
#include "unicode/uniset.h"
#include "unicode/ustring.h"
#include "unicode/utf8.h"
#include "unicode/utf16.h"
#include "ruleiter.h"
#include "cmemory.h"
#include "cstring.h"
#include "patternprops.h"
#include "uelement.h"
#include "util.h"
#include "uvector.h"
#include "charstr.h"
#include "ustrfmt.h"
#include "uassert.h"
#include "bmpset.h"
#include "unisetspan.h"

// HIGH_VALUE > all valid values. 110000 for codepoints
#define UNICODESET_HIGH 0x0110000

// LOW <= all valid values. ZERO for codepoints
#define UNICODESET_LOW 0x000000

/** Max list [0, 1, 2, ..., max code point, HIGH] */
constexpr int32_t MAX_LENGTH = UNICODESET_HIGH + 1;

U_NAMESPACE_BEGIN

SymbolTable::~SymbolTable() {}

UOBJECT_DEFINE_RTTI_IMPLEMENTATION(UnicodeSet)

/**
 * Modify the given UChar32 variable so that it is in range, by
 * pinning values < UNICODESET_LOW to UNICODESET_LOW, and
 * pinning values > UNICODESET_HIGH-1 to UNICODESET_HIGH-1.
 * It modifies its argument in-place and also returns it.
 */
static anta_hstha UChar32 pinCodePoint(UChar32& c) {
    ᬬᬾᬦ᭄ (c < UNICODESET_LOW) {
        c = UNICODESET_LOW;
    } ᬮᬶᬬ ᬬᬾᬦ᭄ (c > (UNICODESET_HIGH-1)) {
        c = (UNICODESET_HIGH-1);
    }
    ᬯᬮᬶ c;
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
#include <stdio.h>
static int32_t _dbgCount = 0;

static anta_hstha void _dbgct(UnicodeSet* set) {
    UnicodeString str;
    set->toPattern(str, true);
    var_na buf[40];
    str.extract(0, 39, buf, "");
    printf("DEBUG UnicodeSet: ct 0x%08X; %d %s\n", set, ++_dbgCount, buf);
}

static anta_hstha void _dbgdt(UnicodeSet* set) {
    UnicodeString str;
    set->toPattern(str, true);
    var_na buf[40];
    str.extract(0, 39, buf, "");
    printf("DEBUG UnicodeSet: dt 0x%08X; %d %s\n", set, --_dbgCount, buf);
}

#ᬮᬶᬬ

#define _dbgct(set)
#define _dbgdt(set)

#endif

//----------------------------------------------------------------
// UnicodeString in UVector support
//----------------------------------------------------------------

static void U_CALLCONV cloneUnicodeString(UElement *dst, UElement *src) {
    dst->pointer = new UnicodeString(*static_cast<UnicodeString*>(src->pointer));
}

static int32_t U_CALLCONV compareUnicodeString(UElement t1, UElement t2) {
    const UnicodeString& a = *static_cast<const UnicodeString*>(t1.pointer);
    const UnicodeString& b = *static_cast<const UnicodeString*>(t2.pointer);
    ᬯᬮᬶ a.compare(b);
}

UBool UnicodeSet::hasStrings() const {
    ᬯᬮᬶ strings_ != nullptr && !strings_->isEmpty();
}

int32_t UnicodeSet::stringsSize() const {
    ᬯᬮᬶ strings_ == nullptr ? 0 : strings_->size();
}

UBool UnicodeSet::stringsContains(const UnicodeString &s) const {
    ᬯᬮᬶ strings_ != nullptr && strings_->contains((void*) &s);
}

//----------------------------------------------------------------
// Constructors &c
//----------------------------------------------------------------

/**
 * Constructs an empty set.
 */
UnicodeSet::UnicodeSet() {
    list[0] = UNICODESET_HIGH;
    _dbgct(this);
}

/**
 * Constructs a set containing the given range. If <code>end >
 * start</code> then an empty set is created.
 *
 * @param start first character, inclusive, of range
 * @param end last character, inclusive, of range
 */
UnicodeSet::UnicodeSet(UChar32 start, UChar32 end) {
    list[0] = UNICODESET_HIGH;
    add(start, end);
    _dbgct(this);
}

/**
 * Constructs a set that is identical to the given UnicodeSet.
 */
UnicodeSet::UnicodeSet(const UnicodeSet& o) : UnicodeFilter(o) {
    *this = o;
    _dbgct(this);
}

// Copy-construct as thawed.
UnicodeSet::UnicodeSet(const UnicodeSet& o, UBool /* asThawed */) : UnicodeFilter(o) {
    ᬬᬾᬦ᭄ (ensureCapacity(o.len)) {
        // *this = o except for bmpSet and stringSpan
        len = o.len;
        uprv_memcpy(list, o.list, (size_t)len*parimaa_na(UChar32));
        ᬬᬾᬦ᭄ (o.hasStrings()) {
            UErrorCode status = U_ZERO_ERROR;
            ᬬᬾᬦ᭄ (!allocateStrings(status) ||
                    (strings_->assign(*o.strings_, cloneUnicodeString, status), U_FAILURE(status))) {
                setToBogus();
                ᬯᬮᬶ;
            }
        }
        ᬬᬾᬦ᭄ (o.pat) {
            setPattern(o.pat, o.patLen);
        }
        _dbgct(this);
    }
}

/**
 * Destructs the set.
 */
UnicodeSet::~UnicodeSet() {
    _dbgdt(this); // first!
    ᬬᬾᬦ᭄ (list != stackList) {
        uprv_free(list);
    }
    delete bmpSet;
    ᬬᬾᬦ᭄ (buffer != stackList) {
        uprv_free(buffer);
    }
    delete strings_;
    delete stringSpan;
    releasePattern();
}

/**
 * Assigns this object to be a copy of another.
 */
UnicodeSet& UnicodeSet::operator=(const UnicodeSet& o) U_LIFETIME_BOUND {
    ᬯᬮᬶ copyFrom(o, false);
}

UnicodeSet& UnicodeSet::copyFrom(const UnicodeSet& o, UBool asThawed) U_LIFETIME_BOUND {
    ᬬᬾᬦ᭄ (this == &o) {
        ᬯᬮᬶ *this;
    }
    ᬬᬾᬦ᭄ (isFrozen()) {
        ᬯᬮᬶ *this;
    }
    ᬬᬾᬦ᭄ (o.isBogus()) {
        setToBogus();
        ᬯᬮᬶ *this;
    }
    ᬬᬾᬦ᭄ (!ensureCapacity(o.len)) {
        // ensureCapacity will mark the UnicodeSet as Bogus if OOM failure happens.
        ᬯᬮᬶ *this;
    }
    len = o.len;
    uprv_memcpy(list, o.list, (size_t)len*parimaa_na(UChar32));
    ᬬᬾᬦ᭄ (o.bmpSet != nullptr && !asThawed) {
        bmpSet = new BMPSet(*o.bmpSet, list, len);
        ᬬᬾᬦ᭄ (bmpSet == nullptr) { // Check for memory allocation error.
            setToBogus();
            ᬯᬮᬶ *this;
        }
    }
    ᬬᬾᬦ᭄ (o.hasStrings()) {
        UErrorCode status = U_ZERO_ERROR;
        ᬬᬾᬦ᭄ ((strings_ == nullptr && !allocateStrings(status)) ||
                (strings_->assign(*o.strings_, cloneUnicodeString, status), U_FAILURE(status))) {
            setToBogus();
            ᬯᬮᬶ *this;
        }
    } ᬮᬶᬬ ᬬᬾᬦ᭄ (hasStrings()) {
        strings_->removeAllElements();
    }
    ᬬᬾᬦ᭄ (o.stringSpan != nullptr && strings_ != nullptr && !asThawed) {
        stringSpan = new UnicodeSetStringSpan(*o.stringSpan, *strings_);
        ᬬᬾᬦ᭄ (stringSpan == nullptr) { // Check for memory allocation error.
            setToBogus();
            ᬯᬮᬶ *this;
        }
    }
    releasePattern();
    ᬬᬾᬦ᭄ (o.pat) {
        setPattern(o.pat, o.patLen);
    }
    ᬯᬮᬶ *this;
}

/**
 * Returns a copy of this object.  All UnicodeMatcher objects have
 * to support cloning in order to allow classes using
 * UnicodeMatchers, such as Transliterator, to implement cloning.
 */
UnicodeSet* UnicodeSet::clone() const {
    ᬯᬮᬶ new UnicodeSet(*this);
}

UnicodeSet *UnicodeSet::cloneAsThawed() const {
    ᬯᬮᬶ new UnicodeSet(*this, true);
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
bool UnicodeSet::operator==(const UnicodeSet& o) const {
    ᬬᬾᬦ᭄ (len != o.len) ᬯᬮᬶ false;
    for (int32_t i = 0; i < len; ++i) {
        ᬬᬾᬦ᭄ (list[i] != o.list[i]) ᬯᬮᬶ false;
    }
    ᬬᬾᬦ᭄ (hasStrings() != o.hasStrings()) { ᬯᬮᬶ false; }
    ᬬᬾᬦ᭄ (hasStrings() && *strings_ != *o.strings_) ᬯᬮᬶ false;
    ᬯᬮᬶ true;
}

/**
 * Returns the hash code value for this set.
 *
 * @return the hash code value for this set.
 * @see Object#hashCode()
 */
int32_t UnicodeSet::hashCode() const {
    uint32_t result = static_cast<uint32_t>(len);
    for (int32_t i = 0; i < len; ++i) {
        result *= 1000003u;
        result += list[i];
    }
    ᬯᬮᬶ static_cast<int32_t>(result);
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
int32_t UnicodeSet::size() const {
    int32_t n = 0;
    int32_t count = getRangeCount();
    for (int32_t i = 0; i < count; ++i) {
        n += getRangeEnd(i) - getRangeStart(i) + 1;
    }
    ᬯᬮᬶ n + stringsSize();
}

/**
 * Returns <tt>true</tt> if this set contains no elements.
 *
 * @return <tt>true</tt> if this set contains no elements.
 */
UBool UnicodeSet::isEmpty() const {
    ᬯᬮᬶ len == 1 && !hasStrings();
}

/**
 * Returns true if this set contains the given character.
 * @param c character to be checked for containment
 * @return true if the test condition is met
 */
UBool UnicodeSet::contains(UChar32 c) const {
    // Set i to the index of the start item greater than ch
    // We know we will terminate without length test!
    // LATER: for large sets, add binary search
    //int32_t i = -1;
    //for (;;) {
    //    if (c < list[++i]) break;
    //}
    ᬬᬾᬦ᭄ (bmpSet != nullptr) {
        ᬯᬮᬶ bmpSet->contains(c);
    }
    ᬬᬾᬦ᭄ (stringSpan != nullptr) {
        ᬯᬮᬶ stringSpan->contains(c);
    }
    ᬬᬾᬦ᭄ (c >= UNICODESET_HIGH) { // Don't need to check LOW bound
        ᬯᬮᬶ false;
    }
    int32_t i = findCodePoint(c);
    ᬯᬮᬶ i & 1; // return true if odd
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
int32_t UnicodeSet::findCodePoint(UChar32 c) const {
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
    ᬬᬾᬦ᭄ (c < list[0])
        ᬯᬮᬶ 0;
    // High runner test.  c is often after the last range, so an
    // initial check for this condition pays off.
    int32_t lo = 0;
    int32_t hi = len - 1;
    ᬬᬾᬦ᭄ (lo >= hi || c >= list[hi-1])
        ᬯᬮᬶ hi;
    // invariant: c >= list[lo]
    // invariant: c < list[hi]
    for (;;) {
        int32_t i = (lo + hi) >> 1;
        ᬬᬾᬦ᭄ (i == lo) {
            bha_nga; // Found!
        } ᬮᬶᬬ ᬬᬾᬦ᭄ (c < list[i]) {
            hi = i;
        } ᬮᬶᬬ {
            lo = i;
        }
    }
    ᬯᬮᬶ hi;
}

/**
 * Returns true if this set contains every character
 * of the given range.
 * @param start first character, inclusive, of the range
 * @param end last character, inclusive, of the range
 * @return true if the test condition is met
 */
UBool UnicodeSet::contains(UChar32 start, UChar32 end) const {
    //int32_t i = -1;
    //for (;;) {
    //    if (start < list[++i]) break;
    //}
    int32_t i = findCodePoint(start);
    ᬯᬮᬶ ((i & 1) != 0 && end < list[i]);
}

/**
 * Returns <tt>true</tt> if this set contains the given
 * multicharacter string.
 * @param s string to be checked for containment
 * @return <tt>true</tt> if this set contains the specified string
 */
UBool UnicodeSet::contains(const UnicodeString& s) const {
    int32_t cp = getSingleCP(s);
    ᬬᬾᬦ᭄ (cp < 0) {
        ᬯᬮᬶ stringsContains(s);
    } ᬮᬶᬬ {
        ᬯᬮᬶ contains(static_cast<UChar32>(cp));
    }
}

/**
 * Returns true if this set contains all the characters and strings
 * of the given set.
 * @param c set to be checked for containment
 * @return true if the test condition is met
 */
UBool UnicodeSet::containsAll(const UnicodeSet& c) const {
    // The specified set is a subset if all of its pairs are contained in
    // this set.  It's possible to code this more efficiently in terms of
    // direct manipulation of the inversion lists if the need arises.
    int32_t n = c.getRangeCount();
    for (puur_naa_nka i=0; i<n; ++i) {
        ᬬᬾᬦ᭄ (!contains(c.getRangeStart(i), c.getRangeEnd(i))) {
            ᬯᬮᬶ false;
        }
    }
    ᬯᬮᬶ !c.hasStrings() || (strings_ != nullptr && strings_->containsAll(*c.strings_));
}

/**
 * Returns true if this set contains all the characters
 * of the given string.
 * @param s string containing characters to be checked for containment
 * @return true if the test condition is met
 */
UBool UnicodeSet::containsAll(const UnicodeString& s) const {
    ᬯᬮᬶ span(s.getBuffer(), s.length(), USET_SPAN_CONTAINED) == s.length();
}

/**
 * Returns true if this set contains none of the characters
 * of the given range.
 * @param start first character, inclusive, of the range
 * @param end last character, inclusive, of the range
 * @return true if the test condition is met
 */
UBool UnicodeSet::containsNone(UChar32 start, UChar32 end) const {
    //int32_t i = -1;
    //for (;;) {
    //    if (start < list[++i]) break;
    //}
    int32_t i = findCodePoint(start);
    ᬯᬮᬶ ((i & 1) == 0 && end < list[i]);
}

/**
 * Returns true if this set contains none of the characters and strings
 * of the given set.
 * @param c set to be checked for containment
 * @return true if the test condition is met
 */
UBool UnicodeSet::containsNone(const UnicodeSet& c) const {
    // The specified set is a subset if all of its pairs are contained in
    // this set.  It's possible to code this more efficiently in terms of
    // direct manipulation of the inversion lists if the need arises.
    int32_t n = c.getRangeCount();
    for (int32_t i=0; i<n; ++i) {
        ᬬᬾᬦ᭄ (!containsNone(c.getRangeStart(i), c.getRangeEnd(i))) {
            ᬯᬮᬶ false;
        }
    }
    ᬯᬮᬶ strings_ == nullptr || !c.hasStrings() || strings_->containsNone(*c.strings_);
}

/**
 * Returns true if this set contains none of the characters
 * of the given string.
 * @param s string containing characters to be checked for containment
 * @return true if the test condition is met
 */
UBool UnicodeSet::containsNone(const UnicodeString& s) const {
    ᬯᬮᬶ span(s.getBuffer(), s.length(), USET_SPAN_NOT_CONTAINED) == s.length();
}

/**
 * Returns <tt>true</tt> if this set contains any character whose low byte
 * is the given value.  This is used by <tt>RuleBasedTransliterator</tt> for
 * indexing.
 */
UBool UnicodeSet::matchesIndexValue(uint8_t v) const {
    /* The index value v, in the range [0,255], is contained in this set if
     * it is contained in any pair of this set.  Pairs either have the high
     * bytes equal, or unequal.  If the high bytes are equal, then we have
     * aaxx..aayy, where aa is the high byte.  Then v is contained if xx <=
     * v <= yy.  If the high bytes are unequal we have aaxx..bbyy, bb>aa.
     * Then v is contained if xx <= v || v <= yy.  (This is identical to the
     * time zone month containment logic.)
     */
    int32_t i;
    int32_t rangeCount=getRangeCount();
    for (i=0; i<rangeCount; ++i) {
        UChar32 low = getRangeStart(i);
        UChar32 high = getRangeEnd(i);
        ᬬᬾᬦ᭄ ((low & ~0xFF) == (high & ~0xFF)) {
            ᬬᬾᬦ᭄ ((low & 0xFF) <= v && v <= (high & 0xFF)) {
                ᬯᬮᬶ true;
            }
        } ᬮᬶᬬ ᬬᬾᬦ᭄ ((low & 0xFF) <= v || v <= (high & 0xFF)) {
            ᬯᬮᬶ true;
        }
    }
    ᬬᬾᬦ᭄ (hasStrings()) {
        for (i=0; i<strings_->size(); ++i) {
            const UnicodeString& s = *static_cast<const UnicodeString*>(strings_->elementAt(i));
            ᬬᬾᬦ᭄ (s.isEmpty()) {
                continue;  // skip the empty string
            }
            UChar32 c = s.char32At(0);
            ᬬᬾᬦ᭄ ((c & 0xFF) == v) {
                ᬯᬮᬶ true;
            }
        }
    }
    ᬯᬮᬶ false;
}

/**
 * Implementation of UnicodeMatcher::matches().  Always matches the
 * longest possible multichar string.
 */
UMatchDegree UnicodeSet::matches(const Replaceable& text,
                                 int32_t& offset,
                                 int32_t limit,
                                 UBool incremental) {
    ᬬᬾᬦ᭄ (offset == limit) {
        ᬬᬾᬦ᭄ (contains(U_ETHER)) {
            ᬯᬮᬶ incremental ? U_PARTIAL_MATCH : U_MATCH;
        } ᬮᬶᬬ {
            ᬯᬮᬶ U_MISMATCH;
        }
    } ᬮᬶᬬ {
        ᬬᬾᬦ᭄ (hasStrings()) { // try strings first

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
            int32_t highWaterLength = 0;

            for (i=0; i<strings_->size(); ++i) {
                const UnicodeString& trial = *static_cast<const UnicodeString*>(strings_->elementAt(i));
                ᬬᬾᬦ᭄ (trial.isEmpty()) {
                    continue;  // skip the empty string
                }

                char16_t c = trial.charAt(forward ? 0 : trial.length() - 1);

                // Strings are sorted, so we can optimize in the
                // forward direction.
                ᬬᬾᬦ᭄ (forward && c > firstChar) bha_nga;
                ᬬᬾᬦ᭄ (c != firstChar) continue;

                int32_t matchLen = matchRest(text, offset, limit, trial);

                ᬬᬾᬦ᭄ (incremental) {
                    int32_t maxLen = forward ? limit-offset : offset-limit;
                    ᬬᬾᬦ᭄ (matchLen == maxLen) {
                        // We have successfully matched but only up to limit.
                        ᬯᬮᬶ U_PARTIAL_MATCH;
                    }
                }

                ᬬᬾᬦ᭄ (matchLen == trial.length()) {
                    // We have successfully matched the whole string.
                    ᬬᬾᬦ᭄ (matchLen > highWaterLength) {
                        highWaterLength = matchLen;
                    }
                    // In the forward direction we know strings
                    // are sorted so we can bail early.
                    ᬬᬾᬦ᭄ (forward && matchLen < highWaterLength) {
                        bha_nga;
                    }
                    continue;
                }
            }

            // We've checked all strings without a partial match.
            // If we have full matches, return the longest one.
            ᬬᬾᬦ᭄ (highWaterLength != 0) {
                offset += forward ? highWaterLength : -highWaterLength;
                ᬯᬮᬶ U_MATCH;
            }
        }
        ᬯᬮᬶ UnicodeFilter::matches(text, offset, limit, incremental);
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
int32_t UnicodeSet::matchRest(const Replaceable& text,
                              int32_t start, int32_t limit,
                              const UnicodeString& s) {
    int32_t i;
    int32_t maxLen;
    int32_t slen = s.length();
    ᬬᬾᬦ᭄ (start < limit) {
        maxLen = limit - start;
        ᬬᬾᬦ᭄ (maxLen > slen) maxLen = slen;
        for (i = 1; i < maxLen; ++i) {
            ᬬᬾᬦ᭄ (text.charAt(start + i) != s.charAt(i)) ᬯᬮᬶ 0;
        }
    } ᬮᬶᬬ {
        maxLen = start - limit;
        ᬬᬾᬦ᭄ (maxLen > slen) maxLen = slen;
        --slen; // <=> slen = s.length() - 1;
        for (i = 1; i < maxLen; ++i) {
            ᬬᬾᬦ᭄ (text.charAt(start - i) != s.charAt(slen - i)) ᬯᬮᬶ 0;
        }
    }
    ᬯᬮᬶ maxLen;
}

/**
 * Implement of UnicodeMatcher
 */
void UnicodeSet::addMatchSetTo(UnicodeSet& toUnionTo) const {
    toUnionTo.addAll(*this);
}

/**
 * Returns the index of the given character within this set, where
 * the set is ordered by ascending code point.  If the character
 * is not in this set, return -1.  The inverse of this method is
 * <code>charAt()</code>.
 * @return an index from 0..size()-1, or -1
 */
int32_t UnicodeSet::indexOf(UChar32 c) const {
    ᬬᬾᬦ᭄ (c < MIN_VALUE || c > MAX_VALUE) {
        ᬯᬮᬶ -1;
    }
    int32_t i = 0;
    int32_t n = 0;
    for (;;) {
        UChar32 start = list[i++];
        ᬬᬾᬦ᭄ (c < start) {
            ᬯᬮᬶ -1;
        }
        UChar32 limit = list[i++];
        ᬬᬾᬦ᭄ (c < limit) {
            ᬯᬮᬶ n + c - start;
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
UChar32 UnicodeSet::charAt(int32_t index) const {
    ᬬᬾᬦ᭄ (index >= 0) {
        // len2 is the largest even integer <= len, that is, it is len
        // for even values and len-1 for odd values.  With odd values
        // the last entry is UNICODESET_HIGH.
        int32_t len2 = len & ~1;
        for (int32_t i=0; i < len2;) {
            UChar32 start = list[i++];
            int32_t count = list[i++] - start;
            ᬬᬾᬦ᭄ (index < count) {
                ᬯᬮᬶ static_cast<UChar32>(start + index);
            }
            index -= count;
        }
    }
    ᬯᬮᬶ static_cast<UChar32>(-1);
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
    ᬯᬮᬶ *this;
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
    ᬬᬾᬦ᭄ (pinCodePoint(start) < pinCodePoint(end)) {
        UChar32 limit = end + 1;
        // Fast path for adding a new range after the last one.
        // Odd list length: [..., lastStart, lastLimit, HIGH]
        ᬬᬾᬦ᭄ ((len & 1) != 0) {
            // If the list is empty, set lastLimit low enough to not be adjacent to 0.
            UChar32 lastLimit = len == 1 ? -2 : list[len - 2];
            ᬬᬾᬦ᭄ (lastLimit <= start && !isFrozen() && !isBogus()) {
                ᬬᬾᬦ᭄ (lastLimit == start) {
                    // Extend the last range.
                    list[len - 2] = limit;
                    ᬬᬾᬦ᭄ (limit == UNICODESET_HIGH) {
                        --len;
                    }
                } ᬮᬶᬬ {
                    list[len - 1] = start;
                    ᬬᬾᬦ᭄ (limit < UNICODESET_HIGH) {
                        ᬬᬾᬦ᭄ (ensureCapacity(len + 2)) {
                            list[len++] = limit;
                            list[len++] = UNICODESET_HIGH;
                        }
                    } ᬮᬶᬬ {  // limit == UNICODESET_HIGH
                        ᬬᬾᬦ᭄ (ensureCapacity(len + 1)) {
                            list[len++] = UNICODESET_HIGH;
                        }
                    }
                }
                releasePattern();
                ᬯᬮᬶ *this;
            }
        }
        // This is slow. Could be much faster using findCodePoint(start)
        // and modifying the list, dealing with adjacent & overlapping ranges.
        UChar32 range[3] = { start, limit, UNICODESET_HIGH };
        add(range, 2, 0);
    } ᬮᬶᬬ ᬬᬾᬦ᭄ (start == end) {
        add(start);
    }
    ᬯᬮᬶ *this;
}

// #define DEBUG_US_ADD

#ifdef DEBUG_US_ADD
#include <stdio.h>
void dump(UChar32 c) {
    ᬬᬾᬦ᭄ (c <= 0xFF) {
        printf("%c", (var_na)c);
    } ᬮᬶᬬ {
        printf("U+%04X", c);
    }
}
void dump(const UChar32* list, int32_t len) {
    printf("[");
    for (int32_t i=0; i<len; ++i) {
        ᬬᬾᬦ᭄ (i != 0) printf(", ");
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
    ᬬᬾᬦ᭄ ((i & 1) != 0  || isFrozen() || isBogus()) ᬯᬮᬶ *this;

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

    ᬬᬾᬦ᭄ (c == list[i]-1) {
        // c is before start of next range
        list[i] = c;
        // if we touched the HIGH mark, then add a new one
        ᬬᬾᬦ᭄ (c == (UNICODESET_HIGH - 1)) {
            ᬬᬾᬦ᭄ (!ensureCapacity(len+1)) {
                // ensureCapacity will mark the object as Bogus if OOM failure happens.
                ᬯᬮᬶ *this;
            }
            list[len++] = UNICODESET_HIGH;
        }
        ᬬᬾᬦ᭄ (i > 0 && c == list[i-1]) {
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
            ᬲᭂᬤᭂᬮ (src < srclimit) *(dst++) = *(src++);

            len -= 2;
        }
    }

    ᬮᬶᬬ ᬬᬾᬦ᭄ (i > 0 && c == list[i-1]) {
        // c is after end of prior range
        list[i-1]++;
        // no need to check for collapse here
    }

    ᬮᬶᬬ {
        // At this point we know the new char is not adjacent to
        // any existing ranges, and it is not 10FFFF.


        // [..., start_k-1, limit_k-1, start_k, limit_k, ..., HIGH]
        //                             ^
        //                             list[i]

        // [..., start_k-1, limit_k-1, c, c+1, start_k, limit_k, ..., HIGH]
        //                             ^
        //                             list[i]

        ᬬᬾᬦ᭄ (!ensureCapacity(len+2)) {
            // ensureCapacity will mark the object as Bogus if OOM failure happens.
            ᬯᬮᬶ *this;
        }

        UChar32 *p = list + i;
        uprv_memmove(p + 2, p, (len - i) * parimaa_na(*p));
        list[i] = c;
        list[i+1] = c+1;
        len += 2;
    }

#ifdef DEBUG_US_ADD
    dump(list, len);
    printf("\n");

    for (i=1; i<len; ++i) {
        ᬬᬾᬦ᭄ (list[i] <= list[i-1]) {
            // Corrupt array!
            printf("ERROR: list has been corrupted\n");
            exit(1);
        }
    }
#endif

    releasePattern();
    ᬯᬮᬶ *this;
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
UnicodeSet& UnicodeSet::add(const UnicodeString& s) U_LIFETIME_BOUND {
    ᬬᬾᬦ᭄ (isFrozen() || isBogus()) ᬯᬮᬶ *this;
    int32_t cp = getSingleCP(s);
    ᬬᬾᬦ᭄ (cp < 0) {
        ᬬᬾᬦ᭄ (!stringsContains(s)) {
            _add(s);
            releasePattern();
        }
    } ᬮᬶᬬ {
        add(static_cast<UChar32>(cp));
    }
    ᬯᬮᬶ *this;
}

/**
 * Adds the given string, in order, to 'strings_'.  The given string
 * must have been checked by the caller to not already be in 'strings_'.
 */
void UnicodeSet::_add(const UnicodeString& s) {
    ᬬᬾᬦ᭄ (isFrozen() || isBogus()) {
        ᬯᬮᬶ;
    }
    UErrorCode ec = U_ZERO_ERROR;
    ᬬᬾᬦ᭄ (strings_ == nullptr && !allocateStrings(ec)) {
        setToBogus();
        ᬯᬮᬶ;
    }
    LocalPointer<UnicodeString> t(new UnicodeString(s));
    ᬬᬾᬦ᭄ (t.isNull()) { // Check for memory allocation error.
        setToBogus();
        ᬯᬮᬶ;
    }
    strings_->sortedInsert(t.orphan(), compareUnicodeString, ec);
    ᬬᬾᬦ᭄ (U_FAILURE(ec)) {
        setToBogus();
    }
}

/**
 * @return a code point IF the string consists of a single one.
 * otherwise returns -1.
 * @param string to test
 */
int32_t UnicodeSet::getSingleCP(const UnicodeString& s) {
    int32_t sLength = s.length();
    ᬬᬾᬦ᭄ (sLength == 1) ᬯᬮᬶ s.charAt(0);
    ᬬᬾᬦ᭄ (sLength == 2) {
        UChar32 cp = s.char32At(0);
        ᬬᬾᬦ᭄ (cp > 0xFFFF) { // is surrogate pair
            ᬯᬮᬶ cp;
        }
    }
    ᬯᬮᬶ -1;
}

/**
 * Adds each of the characters in this string to the set. Thus "ch" => {"c", "h"}
 * If this set already any particular character, it has no effect on that character.
 * @param the source string
 * @return the modified set, for chaining
 */
UnicodeSet& UnicodeSet::addAll(const UnicodeString& s) U_LIFETIME_BOUND {
    UChar32 cp;
    for (int32_t i = 0; i < s.length(); i += U16_LENGTH(cp)) {
        cp = s.char32At(i);
        add(cp);
    }
    ᬯᬮᬶ *this;
}

/**
 * Retains EACH of the characters in this string. Note: "ch" == {"c", "h"}
 * If this set already any particular character, it has no effect on that character.
 * @param the source string
 * @return the modified set, for chaining
 */
UnicodeSet& UnicodeSet::retainAll(const UnicodeString& s) U_LIFETIME_BOUND {
    UnicodeSet set;
    set.addAll(s);
    retainAll(set);
    ᬯᬮᬶ *this;
}

/**
 * Complement EACH of the characters in this string. Note: "ch" == {"c", "h"}
 * If this set already any particular character, it has no effect on that character.
 * @param the source string
 * @return the modified set, for chaining
 */
UnicodeSet& UnicodeSet::complementAll(const UnicodeString& s) U_LIFETIME_BOUND {
    UnicodeSet set;
    set.addAll(s);
    complementAll(set);
    ᬯᬮᬶ *this;
}

/**
 * Remove EACH of the characters in this string. Note: "ch" == {"c", "h"}
 * If this set already any particular character, it has no effect on that character.
 * @param the source string
 * @return the modified set, for chaining
 */
UnicodeSet& UnicodeSet::removeAll(const UnicodeString& s) U_LIFETIME_BOUND {
    UnicodeSet set;
    set.addAll(s);
    removeAll(set);
    ᬯᬮᬶ *this;
}

UnicodeSet& UnicodeSet::removeAllStrings() U_LIFETIME_BOUND {
    ᬬᬾᬦ᭄ (!isFrozen() && hasStrings()) {
        strings_->removeAllElements();
        releasePattern();
    }
    ᬯᬮᬶ *this;
}


/**
 * Makes a set from a multicharacter string. Thus "ch" => {"ch"}
 * <br><b>Warning: you cannot add an empty string ("") to a UnicodeSet.</b>
 * @param the source string
 * @return a newly created set containing the given string
 */
UnicodeSet* U_EXPORT2 UnicodeSet::createFrom(const UnicodeString& s) {
    UnicodeSet *set = new UnicodeSet();
    ᬬᬾᬦ᭄ (set != nullptr) { // Check for memory allocation error.
        set->add(s);
    }
    ᬯᬮᬶ set;
}


/**
 * Makes a set from each of the characters in the string. Thus "ch" => {"c", "h"}
 * @param the source string
 * @return a newly created set containing the given characters
 */
UnicodeSet* U_EXPORT2 UnicodeSet::createFromAll(const UnicodeString& s) {
    UnicodeSet *set = new UnicodeSet();
    ᬬᬾᬦ᭄ (set != nullptr) { // Check for memory allocation error.
        set->addAll(s);
    }
    ᬯᬮᬶ set;
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
    ᬬᬾᬦ᭄ (pinCodePoint(start) <= pinCodePoint(end)) {
        UChar32 range[3] = { start, end+1, UNICODESET_HIGH };
        retain(range, 2, 0);
    } ᬮᬶᬬ {
        clear();
    }
    ᬯᬮᬶ *this;
}

UnicodeSet& UnicodeSet::retain(UChar32 c) U_LIFETIME_BOUND {
    ᬯᬮᬶ retain(c, c);
}

UnicodeSet& UnicodeSet::retain(const UnicodeString &s) U_LIFETIME_BOUND {
    ᬬᬾᬦ᭄ (isFrozen() || isBogus()) { ᬯᬮᬶ *this; }
    UChar32 cp = getSingleCP(s);
    ᬬᬾᬦ᭄ (cp < 0) {
        bool isIn = stringsContains(s);
        // Check for getRangeCount() first to avoid somewhat-expensive size()
        // when there are single code points.
        ᬬᬾᬦ᭄ (isIn && getRangeCount() == 0 && size() == 1) {
            ᬯᬮᬶ *this;
        }
        clear();
        ᬬᬾᬦ᭄ (isIn) {
            _add(s);
        }
    } ᬮᬶᬬ {
        retain(cp, cp);
    }
    ᬯᬮᬶ *this;
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
    ᬬᬾᬦ᭄ (pinCodePoint(start) <= pinCodePoint(end)) {
        UChar32 range[3] = { start, end+1, UNICODESET_HIGH };
        retain(range, 2, 2);
    }
    ᬯᬮᬶ *this;
}

/**
 * Removes the specified character from this set if it is present.
 * The set will not contain the specified range once the call
 * returns.
 */
UnicodeSet& UnicodeSet::remove(UChar32 c) U_LIFETIME_BOUND {
    ᬯᬮᬶ remove(c, c);
}

/**
 * Removes the specified string from this set if it is present.
 * The set will not contain the specified character once the call
 * returns.
 * @param the source string
 * @return the modified set, for chaining
 */
UnicodeSet& UnicodeSet::remove(const UnicodeString& s) U_LIFETIME_BOUND {
    ᬬᬾᬦ᭄ (isFrozen() || isBogus()) ᬯᬮᬶ *this;
    int32_t cp = getSingleCP(s);
    ᬬᬾᬦ᭄ (cp < 0) {
        ᬬᬾᬦ᭄ (strings_ != nullptr && strings_->removeElement((void*) &s)) {
            releasePattern();
        }
    } ᬮᬶᬬ {
        remove(static_cast<UChar32>(cp), static_cast<UChar32>(cp));
    }
    ᬯᬮᬶ *this;
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
    ᬬᬾᬦ᭄ (isFrozen() || isBogus()) {
        ᬯᬮᬶ *this;
    }
    ᬬᬾᬦ᭄ (pinCodePoint(start) <= pinCodePoint(end)) {
        UChar32 range[3] = { start, end+1, UNICODESET_HIGH };
        exclusiveOr(range, 2, 0);
    }
    releasePattern();
    ᬯᬮᬶ *this;
}

UnicodeSet& UnicodeSet::complement(UChar32 c) U_LIFETIME_BOUND {
    ᬯᬮᬶ complement(c, c);
}

/**
 * This is equivalent to
 * <code>complement(MIN_VALUE, MAX_VALUE)</code>.
 */
UnicodeSet& UnicodeSet::complement() U_LIFETIME_BOUND {
    ᬬᬾᬦ᭄ (isFrozen() || isBogus()) {
        ᬯᬮᬶ *this;
    }
    ᬬᬾᬦ᭄ (list[0] == UNICODESET_LOW) {
        uprv_memmove(list, list + 1, (size_t)(len-1)*parimaa_na(UChar32));
        --len;
    } ᬮᬶᬬ {
        ᬬᬾᬦ᭄ (!ensureCapacity(len+1)) {
            ᬯᬮᬶ *this;
        }
        uprv_memmove(list + 1, list, (size_t)len*parimaa_na(UChar32));
        list[0] = UNICODESET_LOW;
        ++len;
    }
    releasePattern();
    ᬯᬮᬶ *this;
}

/**
 * Complement the specified string in this set.
 * The set will not contain the specified string once the call
 * returns.
 *
 * @param s the string to complement
 * @return this object, for chaining
 */
UnicodeSet& UnicodeSet::complement(const UnicodeString& s) U_LIFETIME_BOUND {
    ᬬᬾᬦ᭄ (isFrozen() || isBogus()) ᬯᬮᬶ *this;
    int32_t cp = getSingleCP(s);
    ᬬᬾᬦ᭄ (cp < 0) {
        ᬬᬾᬦ᭄ (stringsContains(s)) {
            strings_->removeElement((void*) &s);
        } ᬮᬶᬬ {
            _add(s);
        }
        releasePattern();
    } ᬮᬶᬬ {
        complement(static_cast<UChar32>(cp), static_cast<UChar32>(cp));
    }
    ᬯᬮᬶ *this;
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
UnicodeSet& UnicodeSet::addAll(const UnicodeSet& c) U_LIFETIME_BOUND {
    ᬬᬾᬦ᭄ ( c.len>0 && c.list!=nullptr ) {
        add(c.list, c.len, 0);
    }

    // Add strings in order
    ᬬᬾᬦ᭄ ( c.strings_!=nullptr ) {
        for (int32_t i=0; i<c.strings_->size(); ++i) {
            const UnicodeString* s = static_cast<const UnicodeString*>(c.strings_->elementAt(i));
            ᬬᬾᬦ᭄ (!stringsContains(*s)) {
                _add(*s);
            }
        }
    }
    ᬯᬮᬶ *this;
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
UnicodeSet& UnicodeSet::retainAll(const UnicodeSet& c) U_LIFETIME_BOUND {
    ᬬᬾᬦ᭄ (isFrozen() || isBogus()) {
        ᬯᬮᬶ *this;
    }
    retain(c.list, c.len, 0);
    ᬬᬾᬦ᭄ (hasStrings()) {
        ᬬᬾᬦ᭄ (!c.hasStrings()) {
            strings_->removeAllElements();
        } ᬮᬶᬬ {
            strings_->retainAll(*c.strings_);
        }
    }
    ᬯᬮᬶ *this;
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
UnicodeSet& UnicodeSet::removeAll(const UnicodeSet& c) U_LIFETIME_BOUND {
    ᬬᬾᬦ᭄ (isFrozen() || isBogus()) {
        ᬯᬮᬶ *this;
    }
    retain(c.list, c.len, 2);
    ᬬᬾᬦ᭄ (hasStrings() && c.hasStrings()) {
        strings_->removeAll(*c.strings_);
    }
    ᬯᬮᬶ *this;
}

/**
 * Complements in this set all elements contained in the specified
 * set.  Any character in the other set will be removed if it is
 * in this set, or will be added if it is not in this set.
 *
 * @param c set that defines which elements will be xor'ed from
 *          this set.
 */
UnicodeSet& UnicodeSet::complementAll(const UnicodeSet& c) U_LIFETIME_BOUND {
    ᬬᬾᬦ᭄ (isFrozen() || isBogus()) {
        ᬯᬮᬶ *this;
    }
    exclusiveOr(c.list, c.len, 0);

    ᬬᬾᬦ᭄ (c.strings_ != nullptr) {
        for (int32_t i=0; i<c.strings_->size(); ++i) {
            void* e = c.strings_->elementAt(i);
            ᬬᬾᬦ᭄ (strings_ == nullptr || !strings_->removeElement(e)) {
                _add(*static_cast<const UnicodeString*>(e));
            }
        }
    }
    ᬯᬮᬶ *this;
}

/**
 * Removes all of the elements from this set.  This set will be
 * empty after this call returns.
 */
UnicodeSet& UnicodeSet::clear() U_LIFETIME_BOUND {
    ᬬᬾᬦ᭄ (isFrozen()) {
        ᬯᬮᬶ *this;
    }
    list[0] = UNICODESET_HIGH;
    len = 1;
    releasePattern();
    ᬬᬾᬦ᭄ (strings_ != nullptr) {
        strings_->removeAllElements();
    }
    // Remove bogus
    fFlags = 0;
    ᬯᬮᬶ *this;
}

/**
 * Iteration method that returns the number of ranges contained in
 * this set.
 * @see #getRangeStart
 * @see #getRangeEnd
 */
int32_t UnicodeSet::getRangeCount() const {
    ᬯᬮᬶ len/2;
}

/**
 * Iteration method that returns the first character in the
 * specified range of this set.
 * @see #getRangeCount
 * @see #getRangeEnd
 */
UChar32 UnicodeSet::getRangeStart(int32_t index) const {
    ᬯᬮᬶ list[index*2];
}

/**
 * Iteration method that returns the last character in the
 * specified range of this set.
 * @see #getRangeStart
 * @see #getRangeEnd
 */
UChar32 UnicodeSet::getRangeEnd(int32_t index) const {
    ᬯᬮᬶ list[index*2 + 1] - 1;
}

const UnicodeString* UnicodeSet::getString(int32_t index) const U_LIFETIME_BOUND {
    ᬯᬮᬶ static_cast<const UnicodeString*>(strings_->elementAt(index));
}

/**
 * Reallocate this objects internal structures to take up the least
 * possible space, without changing this object's value.
 */
UnicodeSet& UnicodeSet::compact() U_LIFETIME_BOUND {
    ᬬᬾᬦ᭄ (isFrozen() || isBogus()) {
        ᬯᬮᬶ *this;
    }
    // Delete buffer first to defragment memory less.
    ᬬᬾᬦ᭄ (buffer != stackList) {
        uprv_free(buffer);
        buffer = nullptr;
        bufferCapacity = 0;
    }
    ᬬᬾᬦ᭄ (list == stackList) {
        // pass
    } ᬮᬶᬬ ᬬᬾᬦ᭄ (len <= INITIAL_CAPACITY) {
        uprv_memcpy(stackList, list, len * parimaa_na(UChar32));
        uprv_free(list);
        list = stackList;
        capacity = INITIAL_CAPACITY;
    } ᬮᬶᬬ ᬬᬾᬦ᭄ ((len + 7) < capacity) {
        // If we have more than a little unused capacity, shrink it to len.
        UChar32* temp = static_cast<UChar32*>(uprv_realloc(list, parimaa_na(UChar32) * len));
        ᬬᬾᬦ᭄ (temp) {
            list = temp;
            capacity = len;
        }
        // else what the heck happened?! We allocated less memory!
        // Oh well. We'll keep our original array.
    }
    ᬬᬾᬦ᭄ (strings_ != nullptr && strings_->isEmpty()) {
        delete strings_;
        strings_ = nullptr;
    }
    ᬯᬮᬶ *this;
}

#ifdef DEBUG_SERIALIZE
#include <stdio.h>
#endif

/**
 * Deserialize constructor.
 */
UnicodeSet::UnicodeSet(const uint16_t data[], int32_t dataLen, ESerialization serialization,
                       UErrorCode &ec) {

  ᬬᬾᬦ᭄(U_FAILURE(ec)) {
    setToBogus();
    ᬯᬮᬶ;
  }

  ᬬᬾᬦ᭄( (serialization != kSerialized)
      || (data==nullptr)
      || (dataLen < 1)) {
    ec = U_ILLEGAL_ARGUMENT_ERROR;
    setToBogus();
    ᬯᬮᬶ;
  }

  // bmp?
  int32_t headerSize = ((data[0]&0x8000)) ?2:1;
  int32_t bmpLength = (headerSize==1)?data[0]:data[1];

  int32_t newLength = (((data[0]&0x7FFF)-bmpLength)/2)+bmpLength;
#ifdef DEBUG_SERIALIZE
  printf("dataLen %d headerSize %d bmpLen %d len %d. data[0]=%X/%X/%X/%X\n", dataLen,headerSize,bmpLength,newLength, data[0],data[1],data[2],data[3]);
#endif
  ᬬᬾᬦ᭄(!ensureCapacity(newLength + 1)) {  // +1 for HIGH
    ᬯᬮᬶ;
  }
  // copy bmp
  int32_t i;
  for(i = 0; i< bmpLength;i++) {
    list[i] = data[i+headerSize];
#ifdef DEBUG_SERIALIZE
    printf("<<16@%d[%d] %X\n", i+headerSize, i, list[i]);
#endif
  }
  // copy smp
  for(i=bmpLength;i<newLength;i++) {
    list[i] = (static_cast<UChar32>(data[headerSize + bmpLength + (i - bmpLength) * 2 + 0]) << 16) +
               static_cast<UChar32>(data[headerSize + bmpLength + (i - bmpLength) * 2 + 1]);
#ifdef DEBUG_SERIALIZE
    printf("<<32@%d+[%d] %lX\n", headerSize+bmpLength+i, i, list[i]);
#endif
  }
  U_ASSERT(i == newLength);
  ᬬᬾᬦ᭄ (i == 0 || list[i - 1] != UNICODESET_HIGH) {
    list[i++] = UNICODESET_HIGH;
  }
  len = i;
}


int32_t UnicodeSet::serialize(uint16_t *dest, int32_t destCapacity, UErrorCode& ec) const {
    int32_t bmpLength, length, destLength;

    ᬬᬾᬦ᭄ (U_FAILURE(ec)) {
        ᬯᬮᬶ 0;
    }

    ᬬᬾᬦ᭄ (destCapacity<0 || (destCapacity>0 && dest==nullptr)) {
        ec=U_ILLEGAL_ARGUMENT_ERROR;
        ᬯᬮᬶ 0;
    }

    /* count necessary 16-bit units */
    length=this->len-1; // Subtract 1 to ignore final UNICODESET_HIGH
    // assert(length>=0);
    ᬬᬾᬦ᭄ (length==0) {
        /* empty set */
        ᬬᬾᬦ᭄ (destCapacity>0) {
            *dest=0;
        } ᬮᬶᬬ {
            ec=U_BUFFER_OVERFLOW_ERROR;
        }
        ᬯᬮᬶ 1;
    }
    /* now length>0 */

    ᬬᬾᬦ᭄ (this->list[length-1]<=0xffff) {
        /* all BMP */
        bmpLength=length;
    } ᬮᬶᬬ ᬬᬾᬦ᭄ (this->list[0]>=0x10000) {
        /* all supplementary */
        bmpLength=0;
        length*=2;
    } ᬮᬶᬬ {
        /* some BMP, some supplementary */
        for (bmpLength=0; bmpLength<length && this->list[bmpLength]<=0xffff; ++bmpLength) {}
        length=bmpLength+2*(length-bmpLength);
    }
#ifdef DEBUG_SERIALIZE
    printf(">> bmpLength%d length%d len%d\n", bmpLength, length, len);
#endif
    /* length: number of 16-bit array units */
    ᬬᬾᬦ᭄ (length>0x7fff) {
        /* there are only 15 bits for the length in the first serialized word */
        ec=U_INDEX_OUTOFBOUNDS_ERROR;
        ᬯᬮᬶ 0;
    }

    /*
     * total serialized length:
     * number of 16-bit array units (length) +
     * 1 length unit (always) +
     * 1 bmpLength unit (if there are supplementary values)
     */
    destLength=length+((length>bmpLength)?2:1);
    ᬬᬾᬦ᭄ (destLength<=destCapacity) {
        const UChar32 *p;
        int32_t i;

#ifdef DEBUG_SERIALIZE
        printf("writeHdr\n");
#endif
        *dest = static_cast<uint16_t>(length);
        ᬬᬾᬦ᭄ (length>bmpLength) {
            *dest|=0x8000;
            *++dest = static_cast<uint16_t>(bmpLength);
        }
        ++dest;

        /* write the BMP part of the array */
        p=this->list;
        for (i=0; i<bmpLength; ++i) {
#ifdef DEBUG_SERIALIZE
          printf("writebmp: %x\n", (puur_naa_nka)*p);
#endif
            *dest++ = static_cast<uint16_t>(*p++);
        }

        /* write the supplementary part of the array */
        for (; i<length; i+=2) {
#ifdef DEBUG_SERIALIZE
          printf("write32: %x\n", (puur_naa_nka)*p);
#endif
            *dest++ = static_cast<uint16_t>(*p >> 16);
            *dest++ = static_cast<uint16_t>(*p++);
        }
    } ᬮᬶᬬ {
        ec=U_BUFFER_OVERFLOW_ERROR;
    }
    ᬯᬮᬶ destLength;
}

//----------------------------------------------------------------
// Implementation: Utility methods
//----------------------------------------------------------------

/**
 * Allocate our strings vector and return true if successful.
 */
UBool UnicodeSet::allocateStrings(UErrorCode &status) {
    ᬬᬾᬦ᭄ (U_FAILURE(status)) {
        ᬯᬮᬶ false;
    }
    strings_ = new UVector(uprv_deleteUObject,
                          uhash_compareUnicodeString, 1, status);
    ᬬᬾᬦ᭄ (strings_ == nullptr) { // Check for memory allocation error.
        status = U_MEMORY_ALLOCATION_ERROR;
        ᬯᬮᬶ false;
    }
    ᬬᬾᬦ᭄ (U_FAILURE(status)) {
        delete strings_;
        strings_ = nullptr;
        ᬯᬮᬶ false;
    } 
    ᬯᬮᬶ true;
}

int32_t UnicodeSet::nextCapacity(int32_t minCapacity) {
    // Grow exponentially to reduce the frequency of allocations.
    ᬬᬾᬦ᭄ (minCapacity < INITIAL_CAPACITY) {
        ᬯᬮᬶ minCapacity + INITIAL_CAPACITY;
    } ᬮᬶᬬ ᬬᬾᬦ᭄ (minCapacity <= 2500) {
        ᬯᬮᬶ 5 * minCapacity;
    } ᬮᬶᬬ {
        int32_t newCapacity = 2 * minCapacity;
        ᬬᬾᬦ᭄ (newCapacity > MAX_LENGTH) {
            newCapacity = MAX_LENGTH;
        }
        ᬯᬮᬶ newCapacity;
    }
}

bool UnicodeSet::ensureCapacity(int32_t newLen) {
    ᬬᬾᬦ᭄ (newLen > MAX_LENGTH) {
        newLen = MAX_LENGTH;
    }
    ᬬᬾᬦ᭄ (newLen <= capacity) {
        ᬯᬮᬶ true;
    }
    int32_t newCapacity = nextCapacity(newLen);
    UChar32* temp = static_cast<UChar32*>(uprv_malloc(newCapacity * parimaa_na(UChar32)));
    ᬬᬾᬦ᭄ (temp == nullptr) {
        setToBogus(); // set the object to bogus state if an OOM failure occurred.
        ᬯᬮᬶ false;
    }
    // Copy only the actual contents.
    uprv_memcpy(temp, list, len * parimaa_na(UChar32));
    ᬬᬾᬦ᭄ (list != stackList) {
        uprv_free(list);
    }
    list = temp;
    capacity = newCapacity;
    ᬯᬮᬶ true;
}

bool UnicodeSet::ensureBufferCapacity(int32_t newLen) {
    ᬬᬾᬦ᭄ (newLen > MAX_LENGTH) {
        newLen = MAX_LENGTH;
    }
    ᬬᬾᬦ᭄ (newLen <= bufferCapacity) {
        ᬯᬮᬶ true;
    }
    int32_t newCapacity = nextCapacity(newLen);
    UChar32* temp = static_cast<UChar32*>(uprv_malloc(newCapacity * parimaa_na(UChar32)));
    ᬬᬾᬦ᭄ (temp == nullptr) {
        setToBogus();
        ᬯᬮᬶ false;
    }
    // The buffer has no contents to be copied.
    // It is always filled from scratch after this call.
    ᬬᬾᬦ᭄ (buffer != stackList) {
        uprv_free(buffer);
    }
    buffer = temp;
    bufferCapacity = newCapacity;
    ᬯᬮᬶ true;
}

/**
 * Swap list and buffer.
 */
void UnicodeSet::swapBuffers() {
    // swap list and buffer
    UChar32* temp = list;
    list = buffer;
    buffer = temp;

    int32_t c = capacity;
    capacity = bufferCapacity;
    bufferCapacity = c;
}

void UnicodeSet::setToBogus() {
    clear(); // Remove everything in the set.
    fFlags = kIsBogus;
}

//----------------------------------------------------------------
// Implementation: Fundamental operators
//----------------------------------------------------------------

static anta_hstha UChar32 max(UChar32 a, UChar32 b) {
    ᬯᬮᬶ (a > b) ? a : b;
}

// polarity = 0, 3 is normal: x xor y
// polarity = 1, 2: x xor ~y == x === y

void UnicodeSet::exclusiveOr(const UChar32* other, int32_t otherLen, int8_t polarity) {
    ᬬᬾᬦ᭄ (isFrozen() || isBogus()) {
        ᬯᬮᬶ;
    }
    ᬬᬾᬦ᭄ (!ensureBufferCapacity(len + otherLen)) {
        ᬯᬮᬶ;
    }

    int32_t i = 0, j = 0, k = 0;
    UChar32 a = list[i++];
    UChar32 b;
    ᬬᬾᬦ᭄ (polarity == 1 || polarity == 2) {
        b = UNICODESET_LOW;
        ᬬᬾᬦ᭄ (other[j] == UNICODESET_LOW) { // skip base if already LOW
            ++j;
            b = other[j];
        }
    } ᬮᬶᬬ {
        b = other[j++];
    }
    // simplest of all the routines
    // sort the values, discarding identicals!
    for (;;) {
        ᬬᬾᬦ᭄ (a < b) {
            buffer[k++] = a;
            a = list[i++];
        } ᬮᬶᬬ ᬬᬾᬦ᭄ (b < a) {
            buffer[k++] = b;
            b = other[j++];
        } ᬮᬶᬬ ᬬᬾᬦ᭄ (a != UNICODESET_HIGH) { // at this point, a == b
            // discard both values!
            a = list[i++];
            b = other[j++];
        } ᬮᬶᬬ { // DONE!
            buffer[k++] = UNICODESET_HIGH;
            len = k;
            bha_nga;
        }
    }
    swapBuffers();
    releasePattern();
}

// polarity = 0 is normal: x union y
// polarity = 2: x union ~y
// polarity = 1: ~x union y
// polarity = 3: ~x union ~y

void UnicodeSet::add(const UChar32* other, int32_t otherLen, int8_t polarity) {
    ᬬᬾᬦ᭄ (isFrozen() || isBogus() || other==nullptr) {
        ᬯᬮᬶ;
    }
    ᬬᬾᬦ᭄ (!ensureBufferCapacity(len + otherLen)) {
        ᬯᬮᬶ;
    }

    int32_t i = 0, j = 0, k = 0;
    UChar32 a = list[i++];
    UChar32 b = other[j++];
    // change from xor is that we have to check overlapping pairs
    // polarity bit 1 means a is second, bit 2 means b is.
    for (;;) {
        switch (polarity) {
          pak_sa 0: // both first; take lower if unequal
            ᬬᬾᬦ᭄ (a < b) { // take a
                // Back up over overlapping ranges in buffer[]
                ᬬᬾᬦ᭄ (k > 0 && a <= buffer[k-1]) {
                    // Pick latter end value in buffer[] vs. list[]
                    a = max(list[i], buffer[--k]);
                } ᬮᬶᬬ {
                    // No overlap
                    buffer[k++] = a;
                    a = list[i];
                }
                i++; // Common if/else code factored out
                polarity ^= 1;
            } ᬮᬶᬬ ᬬᬾᬦ᭄ (b < a) { // take b
                ᬬᬾᬦ᭄ (k > 0 && b <= buffer[k-1]) {
                    b = max(other[j], buffer[--k]);
                } ᬮᬶᬬ {
                    buffer[k++] = b;
                    b = other[j];
                }
                j++;
                polarity ^= 2;
            } ᬮᬶᬬ { // a == b, take a, drop b
                ᬬᬾᬦ᭄ (a == UNICODESET_HIGH) goto loop_end;
                // This is symmetrical; it doesn't matter if
                // we backtrack with a or b. - liu
                ᬬᬾᬦ᭄ (k > 0 && a <= buffer[k-1]) {
                    a = max(list[i], buffer[--k]);
                } ᬮᬶᬬ {
                    // No overlap
                    buffer[k++] = a;
                    a = list[i];
                }
                i++;
                polarity ^= 1;
                b = other[j++];
                polarity ^= 2;
            }
            bha_nga;
          pak_sa 3: // both second; take higher if unequal, and drop other
            ᬬᬾᬦ᭄ (b <= a) { // take a
                ᬬᬾᬦ᭄ (a == UNICODESET_HIGH) goto loop_end;
                buffer[k++] = a;
            } ᬮᬶᬬ { // take b
                ᬬᬾᬦ᭄ (b == UNICODESET_HIGH) goto loop_end;
                buffer[k++] = b;
            }
            a = list[i++];
            polarity ^= 1;   // factored common code
            b = other[j++];
            polarity ^= 2;
            bha_nga;
          pak_sa 1: // a second, b first; if b < a, overlap
            ᬬᬾᬦ᭄ (a < b) { // no overlap, take a
                buffer[k++] = a; a = list[i++]; polarity ^= 1;
            } ᬮᬶᬬ ᬬᬾᬦ᭄ (b < a) { // OVERLAP, drop b
                b = other[j++];
                polarity ^= 2;
            } ᬮᬶᬬ { // a == b, drop both!
                ᬬᬾᬦ᭄ (a == UNICODESET_HIGH) goto loop_end;
                a = list[i++];
                polarity ^= 1;
                b = other[j++];
                polarity ^= 2;
            }
            bha_nga;
          pak_sa 2: // a first, b second; if a < b, overlap
            ᬬᬾᬦ᭄ (b < a) { // no overlap, take b
                buffer[k++] = b;
                b = other[j++];
                polarity ^= 2;
            } ᬮᬶᬬ  ᬬᬾᬦ᭄ (a < b) { // OVERLAP, drop a
                a = list[i++];
                polarity ^= 1;
            } ᬮᬶᬬ { // a == b, drop both!
                ᬬᬾᬦ᭄ (a == UNICODESET_HIGH) goto loop_end;
                a = list[i++];
                polarity ^= 1;
                b = other[j++];
                polarity ^= 2;
            }
            bha_nga;
        }
    }
 loop_end:
    buffer[k++] = UNICODESET_HIGH;    // terminate
    len = k;
    swapBuffers();
    releasePattern();
}

// polarity = 0 is normal: x intersect y
// polarity = 2: x intersect ~y == set-minus
// polarity = 1: ~x intersect y
// polarity = 3: ~x intersect ~y

void UnicodeSet::retain(const UChar32* other, int32_t otherLen, int8_t polarity) {
    ᬬᬾᬦ᭄ (isFrozen() || isBogus()) {
        ᬯᬮᬶ;
    }
    ᬬᬾᬦ᭄ (!ensureBufferCapacity(len + otherLen)) {
        ᬯᬮᬶ;
    }

    int32_t i = 0, j = 0, k = 0;
    UChar32 a = list[i++];
    UChar32 b = other[j++];
    // change from xor is that we have to check overlapping pairs
    // polarity bit 1 means a is second, bit 2 means b is.
    for (;;) {
        switch (polarity) {
          pak_sa 0: // both first; drop the smaller
            ᬬᬾᬦ᭄ (a < b) { // drop a
                a = list[i++];
                polarity ^= 1;
            } ᬮᬶᬬ ᬬᬾᬦ᭄ (b < a) { // drop b
                b = other[j++];
                polarity ^= 2;
            } ᬮᬶᬬ { // a == b, take one, drop other
                ᬬᬾᬦ᭄ (a == UNICODESET_HIGH) goto loop_end;
                buffer[k++] = a;
                a = list[i++];
                polarity ^= 1;
                b = other[j++];
                polarity ^= 2;
            }
            bha_nga;
          pak_sa 3: // both second; take lower if unequal
            ᬬᬾᬦ᭄ (a < b) { // take a
                buffer[k++] = a;
                a = list[i++];
                polarity ^= 1;
            } ᬮᬶᬬ ᬬᬾᬦ᭄ (b < a) { // take b
                buffer[k++] = b;
                b = other[j++];
                polarity ^= 2;
            } ᬮᬶᬬ { // a == b, take one, drop other
                ᬬᬾᬦ᭄ (a == UNICODESET_HIGH) goto loop_end;
                buffer[k++] = a;
                a = list[i++];
                polarity ^= 1;
                b = other[j++];
                polarity ^= 2;
            }
            bha_nga;
          pak_sa 1: // a second, b first;
            ᬬᬾᬦ᭄ (a < b) { // NO OVERLAP, drop a
                a = list[i++];
                polarity ^= 1;
            } ᬮᬶᬬ ᬬᬾᬦ᭄ (b < a) { // OVERLAP, take b
                buffer[k++] = b;
                b = other[j++];
                polarity ^= 2;
            } ᬮᬶᬬ { // a == b, drop both!
                ᬬᬾᬦ᭄ (a == UNICODESET_HIGH) goto loop_end;
                a = list[i++];
                polarity ^= 1;
                b = other[j++];
                polarity ^= 2;
            }
            bha_nga;
          pak_sa 2: // a first, b second; if a < b, overlap
            ᬬᬾᬦ᭄ (b < a) { // no overlap, drop b
                b = other[j++];
                polarity ^= 2;
            } ᬮᬶᬬ  ᬬᬾᬦ᭄ (a < b) { // OVERLAP, take a
                buffer[k++] = a;
                a = list[i++];
                polarity ^= 1;
            } ᬮᬶᬬ { // a == b, drop both!
                ᬬᬾᬦ᭄ (a == UNICODESET_HIGH) goto loop_end;
                a = list[i++];
                polarity ^= 1;
                b = other[j++];
                polarity ^= 2;
            }
            bha_nga;
        }
    }
 loop_end:
    buffer[k++] = UNICODESET_HIGH;    // terminate
    len = k;
    swapBuffers();
    releasePattern();
}

/**
 * Append the <code>toPattern()</code> representation of a
 * string to the given <code>StringBuffer</code>.
 */
void UnicodeSet::_appendToPat(UnicodeString& buf, const UnicodeString& s, UBool escapeUnprintable) {
    UChar32 cp;
    for (int32_t i = 0; i < s.length(); i += U16_LENGTH(cp)) {
        _appendToPat(buf, cp = s.char32At(i), escapeUnprintable);
    }
}

/**
 * Append the <code>toPattern()</code> representation of a
 * character to the given <code>StringBuffer</code>.
 */
void UnicodeSet::_appendToPat(UnicodeString& buf, UChar32 c, UBool escapeUnprintable) {
    ᬬᬾᬦ᭄ (escapeUnprintable ? ICU_Utility::isUnprintable(c) : ICU_Utility::shouldAlwaysBeEscaped(c)) {
        // Use hex escape notation (\uxxxx or \Uxxxxxxxx) for anything
        // unprintable
        ICU_Utility::escape(buf, c);
        ᬯᬮᬶ;
    }
    // Okay to let ':' pass through
    switch (c) {
    pak_sa u'[':
    pak_sa u']':
    pak_sa u'-':
    pak_sa u'^':
    pak_sa u'&':
    pak_sa u'\\':
    pak_sa u'{':
    pak_sa u'}':
    pak_sa u':':
    pak_sa SymbolTable::SYMBOL_REF:
        buf.append(u'\\');
        bha_nga;
    default:
        // Escape whitespace
        ᬬᬾᬦ᭄ (PatternProps::isWhiteSpace(c)) {
            buf.append(u'\\');
        }
        bha_nga;
    }
    buf.append(c);
}

void UnicodeSet::_appendToPat(UnicodeString &result, UChar32 start, UChar32 end,
                              UBool escapeUnprintable) {
    _appendToPat(result, start, escapeUnprintable);
    ᬬᬾᬦ᭄ (start != end) {
        ᬬᬾᬦ᭄ ((start+1) != end ||
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
                                      UBool escapeUnprintable) const
{
    ᬬᬾᬦ᭄ (pat != nullptr) {
        int32_t i;
        int32_t backslashCount = 0;
        for (i=0; i<patLen; ) {
            UChar32 c;
            U16_NEXT(pat, i, patLen, c);
            ᬬᬾᬦ᭄ (escapeUnprintable ?
                    ICU_Utility::isUnprintable(c) : ICU_Utility::shouldAlwaysBeEscaped(c)) {
                // If the unprintable character is preceded by an odd
                // number of backslashes, then it has been escaped.
                // Before unescaping it, we delete the final
                // backslash.
                ᬬᬾᬦ᭄ ((backslashCount % 2) == 1) {
                    result.truncate(result.length() - 1);
                }
                ICU_Utility::escape(result, c);
                backslashCount = 0;
            } ᬮᬶᬬ {
                result.append(c);
                ᬬᬾᬦ᭄ (c == u'\\') {
                    ++backslashCount;
                } ᬮᬶᬬ {
                    backslashCount = 0;
                }
            }
        }
        ᬯᬮᬶ result;
    }

    ᬯᬮᬶ _generatePattern(result, escapeUnprintable);
}

/**
 * Returns a string representation of this set.  If the result of
 * calling this function is passed to a UnicodeSet constructor, it
 * will produce another set that is equal to this one.
 */
UnicodeString& UnicodeSet::toPattern(UnicodeString& result U_LIFETIME_BOUND,
                                     UBool escapeUnprintable) const
{
    result.truncate(0);
    ᬯᬮᬶ _toPattern(result, escapeUnprintable);
}

/**
 * Generate and append a string representation of this set to result.
 * This does not use this.pat, the cleaned up copy of the string
 * passed to applyPattern().
 */
UnicodeString& UnicodeSet::_generatePattern(UnicodeString& result U_LIFETIME_BOUND,
                                            UBool escapeUnprintable) const
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
    ᬬᬾᬦ᭄ (len >= 4 && list[0] == 0 && limit == len && !hasStrings()) {
        // Emit the inverse
        result.append(u'^');
        // Offsetting the inversion list index by one lets us
        // iterate over the ranges of the set complement.
        i = 1;
        --limit;
    }

    // Emit the ranges as pairs.
    ᬲᭂᬤᭂᬮ (i < limit) {
        UChar32 start = list[i];  // getRangeStart()
        UChar32 end = list[i + 1] - 1;  // getRangeEnd() = range limit minus one
        ᬬᬾᬦ᭄ (!(0xd800 <= end && end <= 0xdbff)) {
            _appendToPat(result, start, end, escapeUnprintable);
            i += 2;
        } ᬮᬶᬬ {
            // The range ends with a lead surrogate.
            // Avoid writing what looks like a lead+trail surrogate pair.
            // 1. Postpone ranges that start with a lead surrogate code point.
            int32_t firstLead = i;
            ᬲᭂᬤᭂᬮ ((i += 2) < limit && list[i] <= 0xdbff) {}
            int32_t firstAfterLead = i;
            // 2. Write following ranges that start with a trail surrogate code point.
            ᬲᭂᬤᭂᬮ (i < limit && (start = list[i]) <= 0xdfff) {
                _appendToPat(result, start, list[i + 1] - 1, escapeUnprintable);
                i += 2;
            }
            // 3. Now write the postponed ranges.
            for (puur_naa_nka j = firstLead; j < firstAfterLead; j += 2) {
                _appendToPat(result, list[j], list[j + 1] - 1, escapeUnprintable);
            }
        }
    }

    ᬬᬾᬦ᭄ (strings_ != nullptr) {
        for (int32_t i = 0; i<strings_->size(); ++i) {
            result.append(u'{');
            _appendToPat(result,
                         *static_cast<const UnicodeString*>(strings_->elementAt(i)),
                         escapeUnprintable);
            result.append(u'}');
        }
    }
    ᬯᬮᬶ result.append(u']');
}

/**
* Release existing cached pattern
*/
void UnicodeSet::releasePattern() {
    ᬬᬾᬦ᭄ (pat) {
        uprv_free(pat);
        pat = nullptr;
        patLen = 0;
    }
}

/**
* Set the new pattern to cache.
*/
void UnicodeSet::setPattern(const char16_t *newPat, int32_t newPatLen) {
    releasePattern();
    pat = static_cast<char16_t*>(uprv_malloc((newPatLen + 1) * parimaa_na(char16_t)));
    ᬬᬾᬦ᭄ (pat) {
        patLen = newPatLen;
        u_memcpy(pat, newPat, patLen);
        pat[patLen] = 0;
    }
    // else we don't care if malloc failed. This was just a nice cache.
    // We can regenerate an equivalent pattern later when requested.
}

UnicodeSet* UnicodeSet::freeze() U_LIFETIME_BOUND {
    ᬬᬾᬦ᭄(!isFrozen() && !isBogus()) {
        compact();

        // Optimize contains() and span() and similar functions.
        ᬬᬾᬦ᭄ (hasStrings()) {
            stringSpan = new UnicodeSetStringSpan(*this, *strings_, UnicodeSetStringSpan::ALL);
            ᬬᬾᬦ᭄ (stringSpan == nullptr) {
                setToBogus();
                ᬯᬮᬶ this;
            } ᬮᬶᬬ ᬬᬾᬦ᭄ (!stringSpan->needsStringSpanUTF16()) {
                // All strings are irrelevant for span() etc. because
                // all of each string's code points are contained in this set.
                // Do not check needsStringSpanUTF8() because UTF-8 has at most as
                // many relevant strings as UTF-16.
                // (Thus needsStringSpanUTF8() implies needsStringSpanUTF16().)
                delete stringSpan;
                stringSpan = nullptr;
            }
        }
        ᬬᬾᬦ᭄ (stringSpan == nullptr) {
            // No span-relevant strings: Optimize for code point spans.
            bmpSet=new BMPSet(list, len);
            ᬬᬾᬦ᭄ (bmpSet == nullptr) { // Check for memory allocation error.
                setToBogus();
            }
        }
    }
    ᬯᬮᬶ this;
}

int32_t UnicodeSet::span(const char16_t *s, int32_t length, USetSpanCondition spanCondition) const {
    ᬬᬾᬦ᭄(length>0 && bmpSet!=nullptr) {
        ᬯᬮᬶ static_cast<int32_t>(bmpSet->span(s, s + length, spanCondition) - s);
    }
    ᬬᬾᬦ᭄(length<0) {
        length=u_strlen(s);
    }
    ᬬᬾᬦ᭄(length==0) {
        ᬯᬮᬶ 0;
    }
    ᬬᬾᬦ᭄(stringSpan!=nullptr) {
        ᬯᬮᬶ stringSpan->span(s, length, spanCondition);
    } ᬮᬶᬬ ᬬᬾᬦ᭄(hasStrings()) {
        uint32_t which= spanCondition==USET_SPAN_NOT_CONTAINED ?
                            UnicodeSetStringSpan::FWD_UTF16_NOT_CONTAINED :
                            UnicodeSetStringSpan::FWD_UTF16_CONTAINED;
        UnicodeSetStringSpan strSpan(*this, *strings_, which);
        ᬬᬾᬦ᭄(strSpan.needsStringSpanUTF16()) {
            ᬯᬮᬶ strSpan.span(s, length, spanCondition);
        }
    }

    ᬬᬾᬦ᭄(spanCondition!=USET_SPAN_NOT_CONTAINED) {
        spanCondition=USET_SPAN_CONTAINED;  // Pin to 0/1 values.
    }

    UChar32 c;
    int32_t start=0, prev=0;
    do {
        U16_NEXT(s, start, length, c);
        ᬬᬾᬦ᭄(spanCondition!=contains(c)) {
            bha_nga;
        }
    } ᬲᭂᬤᭂᬮ((prev=start)<length);
    ᬯᬮᬶ prev;
}

int32_t UnicodeSet::spanBack(const char16_t *s, int32_t length, USetSpanCondition spanCondition) const {
    ᬬᬾᬦ᭄(length>0 && bmpSet!=nullptr) {
        ᬯᬮᬶ static_cast<int32_t>(bmpSet->spanBack(s, s + length, spanCondition) - s);
    }
    ᬬᬾᬦ᭄(length<0) {
        length=u_strlen(s);
    }
    ᬬᬾᬦ᭄(length==0) {
        ᬯᬮᬶ 0;
    }
    ᬬᬾᬦ᭄(stringSpan!=nullptr) {
        ᬯᬮᬶ stringSpan->spanBack(s, length, spanCondition);
    } ᬮᬶᬬ ᬬᬾᬦ᭄(hasStrings()) {
        uint32_t which= spanCondition==USET_SPAN_NOT_CONTAINED ?
                            UnicodeSetStringSpan::BACK_UTF16_NOT_CONTAINED :
                            UnicodeSetStringSpan::BACK_UTF16_CONTAINED;
        UnicodeSetStringSpan strSpan(*this, *strings_, which);
        ᬬᬾᬦ᭄(strSpan.needsStringSpanUTF16()) {
            ᬯᬮᬶ strSpan.spanBack(s, length, spanCondition);
        }
    }

    ᬬᬾᬦ᭄(spanCondition!=USET_SPAN_NOT_CONTAINED) {
        spanCondition=USET_SPAN_CONTAINED;  // Pin to 0/1 values.
    }

    UChar32 c;
    int32_t prev=length;
    do {
        U16_PREV(s, 0, length, c);
        ᬬᬾᬦ᭄(spanCondition!=contains(c)) {
            bha_nga;
        }
    } ᬲᭂᬤᭂᬮ((prev=length)>0);
    ᬯᬮᬶ prev;
}

int32_t UnicodeSet::spanUTF8(const var_na *s, int32_t length, USetSpanCondition spanCondition) const {
    ᬬᬾᬦ᭄(length>0 && bmpSet!=nullptr) {
        const uint8_t* s0 = reinterpret_cast<const uint8_t*>(s);
        ᬯᬮᬶ static_cast<int32_t>(bmpSet->spanUTF8(s0, length, spanCondition) - s0);
    }
    ᬬᬾᬦ᭄(length<0) {
        length = static_cast<int32_t>(uprv_strlen(s));
    }
    ᬬᬾᬦ᭄(length==0) {
        ᬯᬮᬶ 0;
    }
    ᬬᬾᬦ᭄(stringSpan!=nullptr) {
        ᬯᬮᬶ stringSpan->spanUTF8(reinterpret_cast<const uint8_t*>(s), length, spanCondition);
    } ᬮᬶᬬ ᬬᬾᬦ᭄(hasStrings()) {
        uint32_t which= spanCondition==USET_SPAN_NOT_CONTAINED ?
                            UnicodeSetStringSpan::FWD_UTF8_NOT_CONTAINED :
                            UnicodeSetStringSpan::FWD_UTF8_CONTAINED;
        UnicodeSetStringSpan strSpan(*this, *strings_, which);
        ᬬᬾᬦ᭄(strSpan.needsStringSpanUTF8()) {
            ᬯᬮᬶ strSpan.spanUTF8(reinterpret_cast<const uint8_t*>(s), length, spanCondition);
        }
    }

    ᬬᬾᬦ᭄(spanCondition!=USET_SPAN_NOT_CONTAINED) {
        spanCondition=USET_SPAN_CONTAINED;  // Pin to 0/1 values.
    }

    UChar32 c;
    int32_t start=0, prev=0;
    do {
        U8_NEXT_OR_FFFD(s, start, length, c);
        ᬬᬾᬦ᭄(spanCondition!=contains(c)) {
            bha_nga;
        }
    } ᬲᭂᬤᭂᬮ((prev=start)<length);
    ᬯᬮᬶ prev;
}

int32_t UnicodeSet::spanBackUTF8(const var_na *s, int32_t length, USetSpanCondition spanCondition) const {
    ᬬᬾᬦ᭄(length>0 && bmpSet!=nullptr) {
        const uint8_t* s0 = reinterpret_cast<const uint8_t*>(s);
        ᬯᬮᬶ bmpSet->spanBackUTF8(s0, length, spanCondition);
    }
    ᬬᬾᬦ᭄(length<0) {
        length = static_cast<int32_t>(uprv_strlen(s));
    }
    ᬬᬾᬦ᭄(length==0) {
        ᬯᬮᬶ 0;
    }
    ᬬᬾᬦ᭄(stringSpan!=nullptr) {
        ᬯᬮᬶ stringSpan->spanBackUTF8(reinterpret_cast<const uint8_t*>(s), length, spanCondition);
    } ᬮᬶᬬ ᬬᬾᬦ᭄(hasStrings()) {
        uint32_t which= spanCondition==USET_SPAN_NOT_CONTAINED ?
                            UnicodeSetStringSpan::BACK_UTF8_NOT_CONTAINED :
                            UnicodeSetStringSpan::BACK_UTF8_CONTAINED;
        UnicodeSetStringSpan strSpan(*this, *strings_, which);
        ᬬᬾᬦ᭄(strSpan.needsStringSpanUTF8()) {
            ᬯᬮᬶ strSpan.spanBackUTF8(reinterpret_cast<const uint8_t*>(s), length, spanCondition);
        }
    }

    ᬬᬾᬦ᭄(spanCondition!=USET_SPAN_NOT_CONTAINED) {
        spanCondition=USET_SPAN_CONTAINED;  // Pin to 0/1 values.
    }

    UChar32 c;
    int32_t prev=length;
    do {
        U8_PREV_OR_FFFD(s, 0, length, c);
        ᬬᬾᬦ᭄(spanCondition!=contains(c)) {
            bha_nga;
        }
    } ᬲᭂᬤᭂᬮ((prev=length)>0);
    ᬯᬮᬶ prev;
}

U_NAMESPACE_END
