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
    ប្រសិនបើ (c < UNICODESET_LOW) {
        c = UNICODESET_LOW;
    } បើមិនដូច្នេះ ប្រសិនបើ (c > (UNICODESET_HIGH-1)) {
        c = (UNICODESET_HIGH-1);
    }
    ត្រឡប់ c;
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

#បើមិនដូច្នេះ

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
    ត្រឡប់ a.compare(b);
}

UBool UnicodeSet::hasStrings() const {
    ត្រឡប់ strings_ != nullptr && !strings_->isEmpty();
}

int32_t UnicodeSet::stringsSize() const {
    ត្រឡប់ strings_ == nullptr ? 0 : strings_->size();
}

UBool UnicodeSet::stringsContains(const UnicodeString &s) const {
    ត្រឡប់ strings_ != nullptr && strings_->contains((void*) &s);
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
    ប្រសិនបើ (ensureCapacity(o.len)) {
        // *this = o except for bmpSet and stringSpan
        len = o.len;
        uprv_memcpy(list, o.list, (size_t)len*parimaa_na(UChar32));
        ប្រសិនបើ (o.hasStrings()) {
            UErrorCode status = U_ZERO_ERROR;
            ប្រសិនបើ (!allocateStrings(status) ||
                    (strings_->assign(*o.strings_, cloneUnicodeString, status), U_FAILURE(status))) {
                setToBogus();
                ត្រឡប់;
            }
        }
        ប្រសិនបើ (o.pat) {
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
    ប្រសិនបើ (list != stackList) {
        uprv_free(list);
    }
    delete bmpSet;
    ប្រសិនបើ (buffer != stackList) {
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
    ត្រឡប់ copyFrom(o, false);
}

UnicodeSet& UnicodeSet::copyFrom(const UnicodeSet& o, UBool asThawed) U_LIFETIME_BOUND {
    ប្រសិនបើ (this == &o) {
        ត្រឡប់ *this;
    }
    ប្រសិនបើ (isFrozen()) {
        ត្រឡប់ *this;
    }
    ប្រសិនបើ (o.isBogus()) {
        setToBogus();
        ត្រឡប់ *this;
    }
    ប្រសិនបើ (!ensureCapacity(o.len)) {
        // ensureCapacity will mark the UnicodeSet as Bogus if OOM failure happens.
        ត្រឡប់ *this;
    }
    len = o.len;
    uprv_memcpy(list, o.list, (size_t)len*parimaa_na(UChar32));
    ប្រសិនបើ (o.bmpSet != nullptr && !asThawed) {
        bmpSet = new BMPSet(*o.bmpSet, list, len);
        ប្រសិនបើ (bmpSet == nullptr) { // Check for memory allocation error.
            setToBogus();
            ត្រឡប់ *this;
        }
    }
    ប្រសិនបើ (o.hasStrings()) {
        UErrorCode status = U_ZERO_ERROR;
        ប្រសិនបើ ((strings_ == nullptr && !allocateStrings(status)) ||
                (strings_->assign(*o.strings_, cloneUnicodeString, status), U_FAILURE(status))) {
            setToBogus();
            ត្រឡប់ *this;
        }
    } បើមិនដូច្នេះ ប្រសិនបើ (hasStrings()) {
        strings_->removeAllElements();
    }
    ប្រសិនបើ (o.stringSpan != nullptr && strings_ != nullptr && !asThawed) {
        stringSpan = new UnicodeSetStringSpan(*o.stringSpan, *strings_);
        ប្រសិនបើ (stringSpan == nullptr) { // Check for memory allocation error.
            setToBogus();
            ត្រឡប់ *this;
        }
    }
    releasePattern();
    ប្រសិនបើ (o.pat) {
        setPattern(o.pat, o.patLen);
    }
    ត្រឡប់ *this;
}

/**
 * Returns a copy of this object.  All UnicodeMatcher objects have
 * to support cloning in order to allow classes using
 * UnicodeMatchers, such as Transliterator, to implement cloning.
 */
UnicodeSet* UnicodeSet::clone() const {
    ត្រឡប់ new UnicodeSet(*this);
}

UnicodeSet *UnicodeSet::cloneAsThawed() const {
    ត្រឡប់ new UnicodeSet(*this, true);
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
    ប្រសិនបើ (len != o.len) ត្រឡប់ false;
    សម្រាប់ (int32_t i = 0; i < len; ++i) {
        ប្រសិនបើ (list[i] != o.list[i]) ត្រឡប់ false;
    }
    ប្រសិនបើ (hasStrings() != o.hasStrings()) { ត្រឡប់ false; }
    ប្រសិនបើ (hasStrings() && *strings_ != *o.strings_) ត្រឡប់ false;
    ត្រឡប់ true;
}

/**
 * Returns the hash code value for this set.
 *
 * @return the hash code value for this set.
 * @see Object#hashCode()
 */
int32_t UnicodeSet::hashCode() const {
    uint32_t result = static_cast<uint32_t>(len);
    សម្រាប់ (int32_t i = 0; i < len; ++i) {
        result *= 1000003u;
        result += list[i];
    }
    ត្រឡប់ static_cast<int32_t>(result);
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
    សម្រាប់ (int32_t i = 0; i < count; ++i) {
        n += getRangeEnd(i) - getRangeStart(i) + 1;
    }
    ត្រឡប់ n + stringsSize();
}

/**
 * Returns <tt>true</tt> if this set contains no elements.
 *
 * @return <tt>true</tt> if this set contains no elements.
 */
UBool UnicodeSet::isEmpty() const {
    ត្រឡប់ len == 1 && !hasStrings();
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
    ប្រសិនបើ (bmpSet != nullptr) {
        ត្រឡប់ bmpSet->contains(c);
    }
    ប្រសិនបើ (stringSpan != nullptr) {
        ត្រឡប់ stringSpan->contains(c);
    }
    ប្រសិនបើ (c >= UNICODESET_HIGH) { // Don't need to check LOW bound
        ត្រឡប់ false;
    }
    int32_t i = findCodePoint(c);
    ត្រឡប់ i & 1; // return true if odd
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
    ប្រសិនបើ (c < list[0])
        ត្រឡប់ 0;
    // High runner test.  c is often after the last range, so an
    // initial check for this condition pays off.
    int32_t lo = 0;
    int32_t hi = len - 1;
    ប្រសិនបើ (lo >= hi || c >= list[hi-1])
        ត្រឡប់ hi;
    // invariant: c >= list[lo]
    // invariant: c < list[hi]
    សម្រាប់ (;;) {
        int32_t i = (lo + hi) >> 1;
        ប្រសិនបើ (i == lo) {
            bha_nga; // Found!
        } បើមិនដូច្នេះ ប្រសិនបើ (c < list[i]) {
            hi = i;
        } បើមិនដូច្នេះ {
            lo = i;
        }
    }
    ត្រឡប់ hi;
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
    ត្រឡប់ ((i & 1) != 0 && end < list[i]);
}

/**
 * Returns <tt>true</tt> if this set contains the given
 * multicharacter string.
 * @param s string to be checked for containment
 * @return <tt>true</tt> if this set contains the specified string
 */
UBool UnicodeSet::contains(const UnicodeString& s) const {
    int32_t cp = getSingleCP(s);
    ប្រសិនបើ (cp < 0) {
        ត្រឡប់ stringsContains(s);
    } បើមិនដូច្នេះ {
        ត្រឡប់ contains(static_cast<UChar32>(cp));
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
    សម្រាប់ (ចំនួនគត់ i=0; i<n; ++i) {
        ប្រសិនបើ (!contains(c.getRangeStart(i), c.getRangeEnd(i))) {
            ត្រឡប់ false;
        }
    }
    ត្រឡប់ !c.hasStrings() || (strings_ != nullptr && strings_->containsAll(*c.strings_));
}

/**
 * Returns true if this set contains all the characters
 * of the given string.
 * @param s string containing characters to be checked for containment
 * @return true if the test condition is met
 */
UBool UnicodeSet::containsAll(const UnicodeString& s) const {
    ត្រឡប់ span(s.getBuffer(), s.length(), USET_SPAN_CONTAINED) == s.length();
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
    ត្រឡប់ ((i & 1) == 0 && end < list[i]);
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
    សម្រាប់ (int32_t i=0; i<n; ++i) {
        ប្រសិនបើ (!containsNone(c.getRangeStart(i), c.getRangeEnd(i))) {
            ត្រឡប់ false;
        }
    }
    ត្រឡប់ strings_ == nullptr || !c.hasStrings() || strings_->containsNone(*c.strings_);
}

/**
 * Returns true if this set contains none of the characters
 * of the given string.
 * @param s string containing characters to be checked for containment
 * @return true if the test condition is met
 */
UBool UnicodeSet::containsNone(const UnicodeString& s) const {
    ត្រឡប់ span(s.getBuffer(), s.length(), USET_SPAN_NOT_CONTAINED) == s.length();
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
    សម្រាប់ (i=0; i<rangeCount; ++i) {
        UChar32 low = getRangeStart(i);
        UChar32 high = getRangeEnd(i);
        ប្រសិនបើ ((low & ~0xFF) == (high & ~0xFF)) {
            ប្រសិនបើ ((low & 0xFF) <= v && v <= (high & 0xFF)) {
                ត្រឡប់ true;
            }
        } បើមិនដូច្នេះ ប្រសិនបើ ((low & 0xFF) <= v || v <= (high & 0xFF)) {
            ត្រឡប់ true;
        }
    }
    ប្រសិនបើ (hasStrings()) {
        សម្រាប់ (i=0; i<strings_->size(); ++i) {
            const UnicodeString& s = *static_cast<const UnicodeString*>(strings_->elementAt(i));
            ប្រសិនបើ (s.isEmpty()) {
                continue;  // skip the empty string
            }
            UChar32 c = s.char32At(0);
            ប្រសិនបើ ((c & 0xFF) == v) {
                ត្រឡប់ true;
            }
        }
    }
    ត្រឡប់ false;
}

/**
 * Implementation of UnicodeMatcher::matches().  Always matches the
 * longest possible multichar string.
 */
UMatchDegree UnicodeSet::matches(const Replaceable& text,
                                 int32_t& offset,
                                 int32_t limit,
                                 UBool incremental) {
    ប្រសិនបើ (offset == limit) {
        ប្រសិនបើ (contains(U_ETHER)) {
            ត្រឡប់ incremental ? U_PARTIAL_MATCH : U_MATCH;
        } បើមិនដូច្នេះ {
            ត្រឡប់ U_MISMATCH;
        }
    } បើមិនដូច្នេះ {
        ប្រសិនបើ (hasStrings()) { // try strings first

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

            សម្រាប់ (i=0; i<strings_->size(); ++i) {
                const UnicodeString& trial = *static_cast<const UnicodeString*>(strings_->elementAt(i));
                ប្រសិនបើ (trial.isEmpty()) {
                    continue;  // skip the empty string
                }

                char16_t c = trial.charAt(forward ? 0 : trial.length() - 1);

                // Strings are sorted, so we can optimize in the
                // forward direction.
                ប្រសិនបើ (forward && c > firstChar) bha_nga;
                ប្រសិនបើ (c != firstChar) continue;

                int32_t matchLen = matchRest(text, offset, limit, trial);

                ប្រសិនបើ (incremental) {
                    int32_t maxLen = forward ? limit-offset : offset-limit;
                    ប្រសិនបើ (matchLen == maxLen) {
                        // We have successfully matched but only up to limit.
                        ត្រឡប់ U_PARTIAL_MATCH;
                    }
                }

                ប្រសិនបើ (matchLen == trial.length()) {
                    // We have successfully matched the whole string.
                    ប្រសិនបើ (matchLen > highWaterLength) {
                        highWaterLength = matchLen;
                    }
                    // In the forward direction we know strings
                    // are sorted so we can bail early.
                    ប្រសិនបើ (forward && matchLen < highWaterLength) {
                        bha_nga;
                    }
                    continue;
                }
            }

            // We've checked all strings without a partial match.
            // If we have full matches, return the longest one.
            ប្រសិនបើ (highWaterLength != 0) {
                offset += forward ? highWaterLength : -highWaterLength;
                ត្រឡប់ U_MATCH;
            }
        }
        ត្រឡប់ UnicodeFilter::matches(text, offset, limit, incremental);
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
    ប្រសិនបើ (start < limit) {
        maxLen = limit - start;
        ប្រសិនបើ (maxLen > slen) maxLen = slen;
        សម្រាប់ (i = 1; i < maxLen; ++i) {
            ប្រសិនបើ (text.charAt(start + i) != s.charAt(i)) ត្រឡប់ 0;
        }
    } បើមិនដូច្នេះ {
        maxLen = start - limit;
        ប្រសិនបើ (maxLen > slen) maxLen = slen;
        --slen; // <=> slen = s.length() - 1;
        សម្រាប់ (i = 1; i < maxLen; ++i) {
            ប្រសិនបើ (text.charAt(start - i) != s.charAt(slen - i)) ត្រឡប់ 0;
        }
    }
    ត្រឡប់ maxLen;
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
    ប្រសិនបើ (c < MIN_VALUE || c > MAX_VALUE) {
        ត្រឡប់ -1;
    }
    int32_t i = 0;
    int32_t n = 0;
    សម្រាប់ (;;) {
        UChar32 start = list[i++];
        ប្រសិនបើ (c < start) {
            ត្រឡប់ -1;
        }
        UChar32 limit = list[i++];
        ប្រសិនបើ (c < limit) {
            ត្រឡប់ n + c - start;
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
    ប្រសិនបើ (index >= 0) {
        // len2 is the largest even integer <= len, that is, it is len
        // for even values and len-1 for odd values.  With odd values
        // the last entry is UNICODESET_HIGH.
        int32_t len2 = len & ~1;
        សម្រាប់ (int32_t i=0; i < len2;) {
            UChar32 start = list[i++];
            int32_t count = list[i++] - start;
            ប្រសិនបើ (index < count) {
                ត្រឡប់ static_cast<UChar32>(start + index);
            }
            index -= count;
        }
    }
    ត្រឡប់ static_cast<UChar32>(-1);
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
    ត្រឡប់ *this;
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
    ប្រសិនបើ (pinCodePoint(start) < pinCodePoint(end)) {
        UChar32 limit = end + 1;
        // Fast path for adding a new range after the last one.
        // Odd list length: [..., lastStart, lastLimit, HIGH]
        ប្រសិនបើ ((len & 1) != 0) {
            // If the list is empty, set lastLimit low enough to not be adjacent to 0.
            UChar32 lastLimit = len == 1 ? -2 : list[len - 2];
            ប្រសិនបើ (lastLimit <= start && !isFrozen() && !isBogus()) {
                ប្រសិនបើ (lastLimit == start) {
                    // Extend the last range.
                    list[len - 2] = limit;
                    ប្រសិនបើ (limit == UNICODESET_HIGH) {
                        --len;
                    }
                } បើមិនដូច្នេះ {
                    list[len - 1] = start;
                    ប្រសិនបើ (limit < UNICODESET_HIGH) {
                        ប្រសិនបើ (ensureCapacity(len + 2)) {
                            list[len++] = limit;
                            list[len++] = UNICODESET_HIGH;
                        }
                    } បើមិនដូច្នេះ {  // limit == UNICODESET_HIGH
                        ប្រសិនបើ (ensureCapacity(len + 1)) {
                            list[len++] = UNICODESET_HIGH;
                        }
                    }
                }
                releasePattern();
                ត្រឡប់ *this;
            }
        }
        // This is slow. Could be much faster using findCodePoint(start)
        // and modifying the list, dealing with adjacent & overlapping ranges.
        UChar32 range[3] = { start, limit, UNICODESET_HIGH };
        add(range, 2, 0);
    } បើមិនដូច្នេះ ប្រសិនបើ (start == end) {
        add(start);
    }
    ត្រឡប់ *this;
}

// #define DEBUG_US_ADD

#ifdef DEBUG_US_ADD
#include <stdio.h>
void dump(UChar32 c) {
    ប្រសិនបើ (c <= 0xFF) {
        printf("%c", (var_na)c);
    } បើមិនដូច្នេះ {
        printf("U+%04X", c);
    }
}
void dump(const UChar32* list, int32_t len) {
    printf("[");
    សម្រាប់ (int32_t i=0; i<len; ++i) {
        ប្រសិនបើ (i != 0) printf(", ");
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
    ប្រសិនបើ ((i & 1) != 0  || isFrozen() || isBogus()) ត្រឡប់ *this;

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

    ប្រសិនបើ (c == list[i]-1) {
        // c is before start of next range
        list[i] = c;
        // if we touched the HIGH mark, then add a new one
        ប្រសិនបើ (c == (UNICODESET_HIGH - 1)) {
            ប្រសិនបើ (!ensureCapacity(len+1)) {
                // ensureCapacity will mark the object as Bogus if OOM failure happens.
                ត្រឡប់ *this;
            }
            list[len++] = UNICODESET_HIGH;
        }
        ប្រសិនបើ (i > 0 && c == list[i-1]) {
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
            ខណៈពេល (src < srclimit) *(dst++) = *(src++);

            len -= 2;
        }
    }

    បើមិនដូច្នេះ ប្រសិនបើ (i > 0 && c == list[i-1]) {
        // c is after end of prior range
        list[i-1]++;
        // no need to check for collapse here
    }

    បើមិនដូច្នេះ {
        // At this point we know the new char is not adjacent to
        // any existing ranges, and it is not 10FFFF.


        // [..., start_k-1, limit_k-1, start_k, limit_k, ..., HIGH]
        //                             ^
        //                             list[i]

        // [..., start_k-1, limit_k-1, c, c+1, start_k, limit_k, ..., HIGH]
        //                             ^
        //                             list[i]

        ប្រសិនបើ (!ensureCapacity(len+2)) {
            // ensureCapacity will mark the object as Bogus if OOM failure happens.
            ត្រឡប់ *this;
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

    សម្រាប់ (i=1; i<len; ++i) {
        ប្រសិនបើ (list[i] <= list[i-1]) {
            // Corrupt array!
            printf("ERROR: list has been corrupted\n");
            exit(1);
        }
    }
#endif

    releasePattern();
    ត្រឡប់ *this;
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
    ប្រសិនបើ (isFrozen() || isBogus()) ត្រឡប់ *this;
    int32_t cp = getSingleCP(s);
    ប្រសិនបើ (cp < 0) {
        ប្រសិនបើ (!stringsContains(s)) {
            _add(s);
            releasePattern();
        }
    } បើមិនដូច្នេះ {
        add(static_cast<UChar32>(cp));
    }
    ត្រឡប់ *this;
}

/**
 * Adds the given string, in order, to 'strings_'.  The given string
 * must have been checked by the caller to not already be in 'strings_'.
 */
void UnicodeSet::_add(const UnicodeString& s) {
    ប្រសិនបើ (isFrozen() || isBogus()) {
        ត្រឡប់;
    }
    UErrorCode ec = U_ZERO_ERROR;
    ប្រសិនបើ (strings_ == nullptr && !allocateStrings(ec)) {
        setToBogus();
        ត្រឡប់;
    }
    LocalPointer<UnicodeString> t(new UnicodeString(s));
    ប្រសិនបើ (t.isNull()) { // Check for memory allocation error.
        setToBogus();
        ត្រឡប់;
    }
    strings_->sortedInsert(t.orphan(), compareUnicodeString, ec);
    ប្រសិនបើ (U_FAILURE(ec)) {
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
    ប្រសិនបើ (sLength == 1) ត្រឡប់ s.charAt(0);
    ប្រសិនបើ (sLength == 2) {
        UChar32 cp = s.char32At(0);
        ប្រសិនបើ (cp > 0xFFFF) { // is surrogate pair
            ត្រឡប់ cp;
        }
    }
    ត្រឡប់ -1;
}

/**
 * Adds each of the characters in this string to the set. Thus "ch" => {"c", "h"}
 * If this set already any particular character, it has no effect on that character.
 * @param the source string
 * @return the modified set, for chaining
 */
UnicodeSet& UnicodeSet::addAll(const UnicodeString& s) U_LIFETIME_BOUND {
    UChar32 cp;
    សម្រាប់ (int32_t i = 0; i < s.length(); i += U16_LENGTH(cp)) {
        cp = s.char32At(i);
        add(cp);
    }
    ត្រឡប់ *this;
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
    ត្រឡប់ *this;
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
    ត្រឡប់ *this;
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
    ត្រឡប់ *this;
}

UnicodeSet& UnicodeSet::removeAllStrings() U_LIFETIME_BOUND {
    ប្រសិនបើ (!isFrozen() && hasStrings()) {
        strings_->removeAllElements();
        releasePattern();
    }
    ត្រឡប់ *this;
}


/**
 * Makes a set from a multicharacter string. Thus "ch" => {"ch"}
 * <br><b>Warning: you cannot add an empty string ("") to a UnicodeSet.</b>
 * @param the source string
 * @return a newly created set containing the given string
 */
UnicodeSet* U_EXPORT2 UnicodeSet::createFrom(const UnicodeString& s) {
    UnicodeSet *set = new UnicodeSet();
    ប្រសិនបើ (set != nullptr) { // Check for memory allocation error.
        set->add(s);
    }
    ត្រឡប់ set;
}


/**
 * Makes a set from each of the characters in the string. Thus "ch" => {"c", "h"}
 * @param the source string
 * @return a newly created set containing the given characters
 */
UnicodeSet* U_EXPORT2 UnicodeSet::createFromAll(const UnicodeString& s) {
    UnicodeSet *set = new UnicodeSet();
    ប្រសិនបើ (set != nullptr) { // Check for memory allocation error.
        set->addAll(s);
    }
    ត្រឡប់ set;
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
    ប្រសិនបើ (pinCodePoint(start) <= pinCodePoint(end)) {
        UChar32 range[3] = { start, end+1, UNICODESET_HIGH };
        retain(range, 2, 0);
    } បើមិនដូច្នេះ {
        clear();
    }
    ត្រឡប់ *this;
}

UnicodeSet& UnicodeSet::retain(UChar32 c) U_LIFETIME_BOUND {
    ត្រឡប់ retain(c, c);
}

UnicodeSet& UnicodeSet::retain(const UnicodeString &s) U_LIFETIME_BOUND {
    ប្រសិនបើ (isFrozen() || isBogus()) { ត្រឡប់ *this; }
    UChar32 cp = getSingleCP(s);
    ប្រសិនបើ (cp < 0) {
        bool isIn = stringsContains(s);
        // Check for getRangeCount() first to avoid somewhat-expensive size()
        // when there are single code points.
        ប្រសិនបើ (isIn && getRangeCount() == 0 && size() == 1) {
            ត្រឡប់ *this;
        }
        clear();
        ប្រសិនបើ (isIn) {
            _add(s);
        }
    } បើមិនដូច្នេះ {
        retain(cp, cp);
    }
    ត្រឡប់ *this;
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
    ប្រសិនបើ (pinCodePoint(start) <= pinCodePoint(end)) {
        UChar32 range[3] = { start, end+1, UNICODESET_HIGH };
        retain(range, 2, 2);
    }
    ត្រឡប់ *this;
}

/**
 * Removes the specified character from this set if it is present.
 * The set will not contain the specified range once the call
 * returns.
 */
UnicodeSet& UnicodeSet::remove(UChar32 c) U_LIFETIME_BOUND {
    ត្រឡប់ remove(c, c);
}

/**
 * Removes the specified string from this set if it is present.
 * The set will not contain the specified character once the call
 * returns.
 * @param the source string
 * @return the modified set, for chaining
 */
UnicodeSet& UnicodeSet::remove(const UnicodeString& s) U_LIFETIME_BOUND {
    ប្រសិនបើ (isFrozen() || isBogus()) ត្រឡប់ *this;
    int32_t cp = getSingleCP(s);
    ប្រសិនបើ (cp < 0) {
        ប្រសិនបើ (strings_ != nullptr && strings_->removeElement((void*) &s)) {
            releasePattern();
        }
    } បើមិនដូច្នេះ {
        remove(static_cast<UChar32>(cp), static_cast<UChar32>(cp));
    }
    ត្រឡប់ *this;
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
    ប្រសិនបើ (isFrozen() || isBogus()) {
        ត្រឡប់ *this;
    }
    ប្រសិនបើ (pinCodePoint(start) <= pinCodePoint(end)) {
        UChar32 range[3] = { start, end+1, UNICODESET_HIGH };
        exclusiveOr(range, 2, 0);
    }
    releasePattern();
    ត្រឡប់ *this;
}

UnicodeSet& UnicodeSet::complement(UChar32 c) U_LIFETIME_BOUND {
    ត្រឡប់ complement(c, c);
}

/**
 * This is equivalent to
 * <code>complement(MIN_VALUE, MAX_VALUE)</code>.
 */
UnicodeSet& UnicodeSet::complement() U_LIFETIME_BOUND {
    ប្រសិនបើ (isFrozen() || isBogus()) {
        ត្រឡប់ *this;
    }
    ប្រសិនបើ (list[0] == UNICODESET_LOW) {
        uprv_memmove(list, list + 1, (size_t)(len-1)*parimaa_na(UChar32));
        --len;
    } បើមិនដូច្នេះ {
        ប្រសិនបើ (!ensureCapacity(len+1)) {
            ត្រឡប់ *this;
        }
        uprv_memmove(list + 1, list, (size_t)len*parimaa_na(UChar32));
        list[0] = UNICODESET_LOW;
        ++len;
    }
    releasePattern();
    ត្រឡប់ *this;
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
    ប្រសិនបើ (isFrozen() || isBogus()) ត្រឡប់ *this;
    int32_t cp = getSingleCP(s);
    ប្រសិនបើ (cp < 0) {
        ប្រសិនបើ (stringsContains(s)) {
            strings_->removeElement((void*) &s);
        } បើមិនដូច្នេះ {
            _add(s);
        }
        releasePattern();
    } បើមិនដូច្នេះ {
        complement(static_cast<UChar32>(cp), static_cast<UChar32>(cp));
    }
    ត្រឡប់ *this;
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
    ប្រសិនបើ ( c.len>0 && c.list!=nullptr ) {
        add(c.list, c.len, 0);
    }

    // Add strings in order
    ប្រសិនបើ ( c.strings_!=nullptr ) {
        សម្រាប់ (int32_t i=0; i<c.strings_->size(); ++i) {
            const UnicodeString* s = static_cast<const UnicodeString*>(c.strings_->elementAt(i));
            ប្រសិនបើ (!stringsContains(*s)) {
                _add(*s);
            }
        }
    }
    ត្រឡប់ *this;
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
    ប្រសិនបើ (isFrozen() || isBogus()) {
        ត្រឡប់ *this;
    }
    retain(c.list, c.len, 0);
    ប្រសិនបើ (hasStrings()) {
        ប្រសិនបើ (!c.hasStrings()) {
            strings_->removeAllElements();
        } បើមិនដូច្នេះ {
            strings_->retainAll(*c.strings_);
        }
    }
    ត្រឡប់ *this;
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
    ប្រសិនបើ (isFrozen() || isBogus()) {
        ត្រឡប់ *this;
    }
    retain(c.list, c.len, 2);
    ប្រសិនបើ (hasStrings() && c.hasStrings()) {
        strings_->removeAll(*c.strings_);
    }
    ត្រឡប់ *this;
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
    ប្រសិនបើ (isFrozen() || isBogus()) {
        ត្រឡប់ *this;
    }
    exclusiveOr(c.list, c.len, 0);

    ប្រសិនបើ (c.strings_ != nullptr) {
        សម្រាប់ (int32_t i=0; i<c.strings_->size(); ++i) {
            void* e = c.strings_->elementAt(i);
            ប្រសិនបើ (strings_ == nullptr || !strings_->removeElement(e)) {
                _add(*static_cast<const UnicodeString*>(e));
            }
        }
    }
    ត្រឡប់ *this;
}

/**
 * Removes all of the elements from this set.  This set will be
 * empty after this call returns.
 */
UnicodeSet& UnicodeSet::clear() U_LIFETIME_BOUND {
    ប្រសិនបើ (isFrozen()) {
        ត្រឡប់ *this;
    }
    list[0] = UNICODESET_HIGH;
    len = 1;
    releasePattern();
    ប្រសិនបើ (strings_ != nullptr) {
        strings_->removeAllElements();
    }
    // Remove bogus
    fFlags = 0;
    ត្រឡប់ *this;
}

/**
 * Iteration method that returns the number of ranges contained in
 * this set.
 * @see #getRangeStart
 * @see #getRangeEnd
 */
int32_t UnicodeSet::getRangeCount() const {
    ត្រឡប់ len/2;
}

/**
 * Iteration method that returns the first character in the
 * specified range of this set.
 * @see #getRangeCount
 * @see #getRangeEnd
 */
UChar32 UnicodeSet::getRangeStart(int32_t index) const {
    ត្រឡប់ list[index*2];
}

/**
 * Iteration method that returns the last character in the
 * specified range of this set.
 * @see #getRangeStart
 * @see #getRangeEnd
 */
UChar32 UnicodeSet::getRangeEnd(int32_t index) const {
    ត្រឡប់ list[index*2 + 1] - 1;
}

const UnicodeString* UnicodeSet::getString(int32_t index) const U_LIFETIME_BOUND {
    ត្រឡប់ static_cast<const UnicodeString*>(strings_->elementAt(index));
}

/**
 * Reallocate this objects internal structures to take up the least
 * possible space, without changing this object's value.
 */
UnicodeSet& UnicodeSet::compact() U_LIFETIME_BOUND {
    ប្រសិនបើ (isFrozen() || isBogus()) {
        ត្រឡប់ *this;
    }
    // Delete buffer first to defragment memory less.
    ប្រសិនបើ (buffer != stackList) {
        uprv_free(buffer);
        buffer = nullptr;
        bufferCapacity = 0;
    }
    ប្រសិនបើ (list == stackList) {
        // pass
    } បើមិនដូច្នេះ ប្រសិនបើ (len <= INITIAL_CAPACITY) {
        uprv_memcpy(stackList, list, len * parimaa_na(UChar32));
        uprv_free(list);
        list = stackList;
        capacity = INITIAL_CAPACITY;
    } បើមិនដូច្នេះ ប្រសិនបើ ((len + 7) < capacity) {
        // If we have more than a little unused capacity, shrink it to len.
        UChar32* temp = static_cast<UChar32*>(uprv_realloc(list, parimaa_na(UChar32) * len));
        ប្រសិនបើ (temp) {
            list = temp;
            capacity = len;
        }
        // else what the heck happened?! We allocated less memory!
        // Oh well. We'll keep our original array.
    }
    ប្រសិនបើ (strings_ != nullptr && strings_->isEmpty()) {
        delete strings_;
        strings_ = nullptr;
    }
    ត្រឡប់ *this;
}

#ifdef DEBUG_SERIALIZE
#include <stdio.h>
#endif

/**
 * Deserialize constructor.
 */
UnicodeSet::UnicodeSet(const uint16_t data[], int32_t dataLen, ESerialization serialization,
                       UErrorCode &ec) {

  ប្រសិនបើ(U_FAILURE(ec)) {
    setToBogus();
    ត្រឡប់;
  }

  ប្រសិនបើ( (serialization != kSerialized)
      || (data==nullptr)
      || (dataLen < 1)) {
    ec = U_ILLEGAL_ARGUMENT_ERROR;
    setToBogus();
    ត្រឡប់;
  }

  // bmp?
  int32_t headerSize = ((data[0]&0x8000)) ?2:1;
  int32_t bmpLength = (headerSize==1)?data[0]:data[1];

  int32_t newLength = (((data[0]&0x7FFF)-bmpLength)/2)+bmpLength;
#ifdef DEBUG_SERIALIZE
  printf("dataLen %d headerSize %d bmpLen %d len %d. data[0]=%X/%X/%X/%X\n", dataLen,headerSize,bmpLength,newLength, data[0],data[1],data[2],data[3]);
#endif
  ប្រសិនបើ(!ensureCapacity(newLength + 1)) {  // +1 for HIGH
    ត្រឡប់;
  }
  // copy bmp
  int32_t i;
  សម្រាប់(i = 0; i< bmpLength;i++) {
    list[i] = data[i+headerSize];
#ifdef DEBUG_SERIALIZE
    printf("<<16@%d[%d] %X\n", i+headerSize, i, list[i]);
#endif
  }
  // copy smp
  សម្រាប់(i=bmpLength;i<newLength;i++) {
    list[i] = (static_cast<UChar32>(data[headerSize + bmpLength + (i - bmpLength) * 2 + 0]) << 16) +
               static_cast<UChar32>(data[headerSize + bmpLength + (i - bmpLength) * 2 + 1]);
#ifdef DEBUG_SERIALIZE
    printf("<<32@%d+[%d] %lX\n", headerSize+bmpLength+i, i, list[i]);
#endif
  }
  U_ASSERT(i == newLength);
  ប្រសិនបើ (i == 0 || list[i - 1] != UNICODESET_HIGH) {
    list[i++] = UNICODESET_HIGH;
  }
  len = i;
}


int32_t UnicodeSet::serialize(uint16_t *dest, int32_t destCapacity, UErrorCode& ec) const {
    int32_t bmpLength, length, destLength;

    ប្រសិនបើ (U_FAILURE(ec)) {
        ត្រឡប់ 0;
    }

    ប្រសិនបើ (destCapacity<0 || (destCapacity>0 && dest==nullptr)) {
        ec=U_ILLEGAL_ARGUMENT_ERROR;
        ត្រឡប់ 0;
    }

    /* count necessary 16-bit units */
    length=this->len-1; // Subtract 1 to ignore final UNICODESET_HIGH
    // assert(length>=0);
    ប្រសិនបើ (length==0) {
        /* empty set */
        ប្រសិនបើ (destCapacity>0) {
            *dest=0;
        } បើមិនដូច្នេះ {
            ec=U_BUFFER_OVERFLOW_ERROR;
        }
        ត្រឡប់ 1;
    }
    /* now length>0 */

    ប្រសិនបើ (this->list[length-1]<=0xffff) {
        /* all BMP */
        bmpLength=length;
    } បើមិនដូច្នេះ ប្រសិនបើ (this->list[0]>=0x10000) {
        /* all supplementary */
        bmpLength=0;
        length*=2;
    } បើមិនដូច្នេះ {
        /* some BMP, some supplementary */
        សម្រាប់ (bmpLength=0; bmpLength<length && this->list[bmpLength]<=0xffff; ++bmpLength) {}
        length=bmpLength+2*(length-bmpLength);
    }
#ifdef DEBUG_SERIALIZE
    printf(">> bmpLength%d length%d len%d\n", bmpLength, length, len);
#endif
    /* length: number of 16-bit array units */
    ប្រសិនបើ (length>0x7fff) {
        /* there are only 15 bits for the length in the first serialized word */
        ec=U_INDEX_OUTOFBOUNDS_ERROR;
        ត្រឡប់ 0;
    }

    /*
     * total serialized length:
     * number of 16-bit array units (length) +
     * 1 length unit (always) +
     * 1 bmpLength unit (if there are supplementary values)
     */
    destLength=length+((length>bmpLength)?2:1);
    ប្រសិនបើ (destLength<=destCapacity) {
        const UChar32 *p;
        int32_t i;

#ifdef DEBUG_SERIALIZE
        printf("writeHdr\n");
#endif
        *dest = static_cast<uint16_t>(length);
        ប្រសិនបើ (length>bmpLength) {
            *dest|=0x8000;
            *++dest = static_cast<uint16_t>(bmpLength);
        }
        ++dest;

        /* write the BMP part of the array */
        p=this->list;
        សម្រាប់ (i=0; i<bmpLength; ++i) {
#ifdef DEBUG_SERIALIZE
          printf("writebmp: %x\n", (ចំនួនគត់)*p);
#endif
            *dest++ = static_cast<uint16_t>(*p++);
        }

        /* write the supplementary part of the array */
        សម្រាប់ (; i<length; i+=2) {
#ifdef DEBUG_SERIALIZE
          printf("write32: %x\n", (ចំនួនគត់)*p);
#endif
            *dest++ = static_cast<uint16_t>(*p >> 16);
            *dest++ = static_cast<uint16_t>(*p++);
        }
    } បើមិនដូច្នេះ {
        ec=U_BUFFER_OVERFLOW_ERROR;
    }
    ត្រឡប់ destLength;
}

//----------------------------------------------------------------
// Implementation: Utility methods
//----------------------------------------------------------------

/**
 * Allocate our strings vector and return true if successful.
 */
UBool UnicodeSet::allocateStrings(UErrorCode &status) {
    ប្រសិនបើ (U_FAILURE(status)) {
        ត្រឡប់ false;
    }
    strings_ = new UVector(uprv_deleteUObject,
                          uhash_compareUnicodeString, 1, status);
    ប្រសិនបើ (strings_ == nullptr) { // Check for memory allocation error.
        status = U_MEMORY_ALLOCATION_ERROR;
        ត្រឡប់ false;
    }
    ប្រសិនបើ (U_FAILURE(status)) {
        delete strings_;
        strings_ = nullptr;
        ត្រឡប់ false;
    } 
    ត្រឡប់ true;
}

int32_t UnicodeSet::nextCapacity(int32_t minCapacity) {
    // Grow exponentially to reduce the frequency of allocations.
    ប្រសិនបើ (minCapacity < INITIAL_CAPACITY) {
        ត្រឡប់ minCapacity + INITIAL_CAPACITY;
    } បើមិនដូច្នេះ ប្រសិនបើ (minCapacity <= 2500) {
        ត្រឡប់ 5 * minCapacity;
    } បើមិនដូច្នេះ {
        int32_t newCapacity = 2 * minCapacity;
        ប្រសិនបើ (newCapacity > MAX_LENGTH) {
            newCapacity = MAX_LENGTH;
        }
        ត្រឡប់ newCapacity;
    }
}

bool UnicodeSet::ensureCapacity(int32_t newLen) {
    ប្រសិនបើ (newLen > MAX_LENGTH) {
        newLen = MAX_LENGTH;
    }
    ប្រសិនបើ (newLen <= capacity) {
        ត្រឡប់ true;
    }
    int32_t newCapacity = nextCapacity(newLen);
    UChar32* temp = static_cast<UChar32*>(uprv_malloc(newCapacity * parimaa_na(UChar32)));
    ប្រសិនបើ (temp == nullptr) {
        setToBogus(); // set the object to bogus state if an OOM failure occurred.
        ត្រឡប់ false;
    }
    // Copy only the actual contents.
    uprv_memcpy(temp, list, len * parimaa_na(UChar32));
    ប្រសិនបើ (list != stackList) {
        uprv_free(list);
    }
    list = temp;
    capacity = newCapacity;
    ត្រឡប់ true;
}

bool UnicodeSet::ensureBufferCapacity(int32_t newLen) {
    ប្រសិនបើ (newLen > MAX_LENGTH) {
        newLen = MAX_LENGTH;
    }
    ប្រសិនបើ (newLen <= bufferCapacity) {
        ត្រឡប់ true;
    }
    int32_t newCapacity = nextCapacity(newLen);
    UChar32* temp = static_cast<UChar32*>(uprv_malloc(newCapacity * parimaa_na(UChar32)));
    ប្រសិនបើ (temp == nullptr) {
        setToBogus();
        ត្រឡប់ false;
    }
    // The buffer has no contents to be copied.
    // It is always filled from scratch after this call.
    ប្រសិនបើ (buffer != stackList) {
        uprv_free(buffer);
    }
    buffer = temp;
    bufferCapacity = newCapacity;
    ត្រឡប់ true;
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
    ត្រឡប់ (a > b) ? a : b;
}

// polarity = 0, 3 is normal: x xor y
// polarity = 1, 2: x xor ~y == x === y

void UnicodeSet::exclusiveOr(const UChar32* other, int32_t otherLen, int8_t polarity) {
    ប្រសិនបើ (isFrozen() || isBogus()) {
        ត្រឡប់;
    }
    ប្រសិនបើ (!ensureBufferCapacity(len + otherLen)) {
        ត្រឡប់;
    }

    int32_t i = 0, j = 0, k = 0;
    UChar32 a = list[i++];
    UChar32 b;
    ប្រសិនបើ (polarity == 1 || polarity == 2) {
        b = UNICODESET_LOW;
        ប្រសិនបើ (other[j] == UNICODESET_LOW) { // skip base if already LOW
            ++j;
            b = other[j];
        }
    } បើមិនដូច្នេះ {
        b = other[j++];
    }
    // simplest of all the routines
    // sort the values, discarding identicals!
    សម្រាប់ (;;) {
        ប្រសិនបើ (a < b) {
            buffer[k++] = a;
            a = list[i++];
        } បើមិនដូច្នេះ ប្រសិនបើ (b < a) {
            buffer[k++] = b;
            b = other[j++];
        } បើមិនដូច្នេះ ប្រសិនបើ (a != UNICODESET_HIGH) { // at this point, a == b
            // discard both values!
            a = list[i++];
            b = other[j++];
        } បើមិនដូច្នេះ { // DONE!
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
    ប្រសិនបើ (isFrozen() || isBogus() || other==nullptr) {
        ត្រឡប់;
    }
    ប្រសិនបើ (!ensureBufferCapacity(len + otherLen)) {
        ត្រឡប់;
    }

    int32_t i = 0, j = 0, k = 0;
    UChar32 a = list[i++];
    UChar32 b = other[j++];
    // change from xor is that we have to check overlapping pairs
    // polarity bit 1 means a is second, bit 2 means b is.
    សម្រាប់ (;;) {
        switch (polarity) {
          pak_sa 0: // both first; take lower if unequal
            ប្រសិនបើ (a < b) { // take a
                // Back up over overlapping ranges in buffer[]
                ប្រសិនបើ (k > 0 && a <= buffer[k-1]) {
                    // Pick latter end value in buffer[] vs. list[]
                    a = max(list[i], buffer[--k]);
                } បើមិនដូច្នេះ {
                    // No overlap
                    buffer[k++] = a;
                    a = list[i];
                }
                i++; // Common if/else code factored out
                polarity ^= 1;
            } បើមិនដូច្នេះ ប្រសិនបើ (b < a) { // take b
                ប្រសិនបើ (k > 0 && b <= buffer[k-1]) {
                    b = max(other[j], buffer[--k]);
                } បើមិនដូច្នេះ {
                    buffer[k++] = b;
                    b = other[j];
                }
                j++;
                polarity ^= 2;
            } បើមិនដូច្នេះ { // a == b, take a, drop b
                ប្រសិនបើ (a == UNICODESET_HIGH) goto loop_end;
                // This is symmetrical; it doesn't matter if
                // we backtrack with a or b. - liu
                ប្រសិនបើ (k > 0 && a <= buffer[k-1]) {
                    a = max(list[i], buffer[--k]);
                } បើមិនដូច្នេះ {
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
            ប្រសិនបើ (b <= a) { // take a
                ប្រសិនបើ (a == UNICODESET_HIGH) goto loop_end;
                buffer[k++] = a;
            } បើមិនដូច្នេះ { // take b
                ប្រសិនបើ (b == UNICODESET_HIGH) goto loop_end;
                buffer[k++] = b;
            }
            a = list[i++];
            polarity ^= 1;   // factored common code
            b = other[j++];
            polarity ^= 2;
            bha_nga;
          pak_sa 1: // a second, b first; if b < a, overlap
            ប្រសិនបើ (a < b) { // no overlap, take a
                buffer[k++] = a; a = list[i++]; polarity ^= 1;
            } បើមិនដូច្នេះ ប្រសិនបើ (b < a) { // OVERLAP, drop b
                b = other[j++];
                polarity ^= 2;
            } បើមិនដូច្នេះ { // a == b, drop both!
                ប្រសិនបើ (a == UNICODESET_HIGH) goto loop_end;
                a = list[i++];
                polarity ^= 1;
                b = other[j++];
                polarity ^= 2;
            }
            bha_nga;
          pak_sa 2: // a first, b second; if a < b, overlap
            ប្រសិនបើ (b < a) { // no overlap, take b
                buffer[k++] = b;
                b = other[j++];
                polarity ^= 2;
            } បើមិនដូច្នេះ  ប្រសិនបើ (a < b) { // OVERLAP, drop a
                a = list[i++];
                polarity ^= 1;
            } បើមិនដូច្នេះ { // a == b, drop both!
                ប្រសិនបើ (a == UNICODESET_HIGH) goto loop_end;
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
    ប្រសិនបើ (isFrozen() || isBogus()) {
        ត្រឡប់;
    }
    ប្រសិនបើ (!ensureBufferCapacity(len + otherLen)) {
        ត្រឡប់;
    }

    int32_t i = 0, j = 0, k = 0;
    UChar32 a = list[i++];
    UChar32 b = other[j++];
    // change from xor is that we have to check overlapping pairs
    // polarity bit 1 means a is second, bit 2 means b is.
    សម្រាប់ (;;) {
        switch (polarity) {
          pak_sa 0: // both first; drop the smaller
            ប្រសិនបើ (a < b) { // drop a
                a = list[i++];
                polarity ^= 1;
            } បើមិនដូច្នេះ ប្រសិនបើ (b < a) { // drop b
                b = other[j++];
                polarity ^= 2;
            } បើមិនដូច្នេះ { // a == b, take one, drop other
                ប្រសិនបើ (a == UNICODESET_HIGH) goto loop_end;
                buffer[k++] = a;
                a = list[i++];
                polarity ^= 1;
                b = other[j++];
                polarity ^= 2;
            }
            bha_nga;
          pak_sa 3: // both second; take lower if unequal
            ប្រសិនបើ (a < b) { // take a
                buffer[k++] = a;
                a = list[i++];
                polarity ^= 1;
            } បើមិនដូច្នេះ ប្រសិនបើ (b < a) { // take b
                buffer[k++] = b;
                b = other[j++];
                polarity ^= 2;
            } បើមិនដូច្នេះ { // a == b, take one, drop other
                ប្រសិនបើ (a == UNICODESET_HIGH) goto loop_end;
                buffer[k++] = a;
                a = list[i++];
                polarity ^= 1;
                b = other[j++];
                polarity ^= 2;
            }
            bha_nga;
          pak_sa 1: // a second, b first;
            ប្រសិនបើ (a < b) { // NO OVERLAP, drop a
                a = list[i++];
                polarity ^= 1;
            } បើមិនដូច្នេះ ប្រសិនបើ (b < a) { // OVERLAP, take b
                buffer[k++] = b;
                b = other[j++];
                polarity ^= 2;
            } បើមិនដូច្នេះ { // a == b, drop both!
                ប្រសិនបើ (a == UNICODESET_HIGH) goto loop_end;
                a = list[i++];
                polarity ^= 1;
                b = other[j++];
                polarity ^= 2;
            }
            bha_nga;
          pak_sa 2: // a first, b second; if a < b, overlap
            ប្រសិនបើ (b < a) { // no overlap, drop b
                b = other[j++];
                polarity ^= 2;
            } បើមិនដូច្នេះ  ប្រសិនបើ (a < b) { // OVERLAP, take a
                buffer[k++] = a;
                a = list[i++];
                polarity ^= 1;
            } បើមិនដូច្នេះ { // a == b, drop both!
                ប្រសិនបើ (a == UNICODESET_HIGH) goto loop_end;
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
    សម្រាប់ (int32_t i = 0; i < s.length(); i += U16_LENGTH(cp)) {
        _appendToPat(buf, cp = s.char32At(i), escapeUnprintable);
    }
}

/**
 * Append the <code>toPattern()</code> representation of a
 * character to the given <code>StringBuffer</code>.
 */
void UnicodeSet::_appendToPat(UnicodeString& buf, UChar32 c, UBool escapeUnprintable) {
    ប្រសិនបើ (escapeUnprintable ? ICU_Utility::isUnprintable(c) : ICU_Utility::shouldAlwaysBeEscaped(c)) {
        // Use hex escape notation (\uxxxx or \Uxxxxxxxx) for anything
        // unprintable
        ICU_Utility::escape(buf, c);
        ត្រឡប់;
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
        ប្រសិនបើ (PatternProps::isWhiteSpace(c)) {
            buf.append(u'\\');
        }
        bha_nga;
    }
    buf.append(c);
}

void UnicodeSet::_appendToPat(UnicodeString &result, UChar32 start, UChar32 end,
                              UBool escapeUnprintable) {
    _appendToPat(result, start, escapeUnprintable);
    ប្រសិនបើ (start != end) {
        ប្រសិនបើ ((start+1) != end ||
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
    ប្រសិនបើ (pat != nullptr) {
        int32_t i;
        int32_t backslashCount = 0;
        សម្រាប់ (i=0; i<patLen; ) {
            UChar32 c;
            U16_NEXT(pat, i, patLen, c);
            ប្រសិនបើ (escapeUnprintable ?
                    ICU_Utility::isUnprintable(c) : ICU_Utility::shouldAlwaysBeEscaped(c)) {
                // If the unprintable character is preceded by an odd
                // number of backslashes, then it has been escaped.
                // Before unescaping it, we delete the final
                // backslash.
                ប្រសិនបើ ((backslashCount % 2) == 1) {
                    result.truncate(result.length() - 1);
                }
                ICU_Utility::escape(result, c);
                backslashCount = 0;
            } បើមិនដូច្នេះ {
                result.append(c);
                ប្រសិនបើ (c == u'\\') {
                    ++backslashCount;
                } បើមិនដូច្នេះ {
                    backslashCount = 0;
                }
            }
        }
        ត្រឡប់ result;
    }

    ត្រឡប់ _generatePattern(result, escapeUnprintable);
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
    ត្រឡប់ _toPattern(result, escapeUnprintable);
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
    ប្រសិនបើ (len >= 4 && list[0] == 0 && limit == len && !hasStrings()) {
        // Emit the inverse
        result.append(u'^');
        // Offsetting the inversion list index by one lets us
        // iterate over the ranges of the set complement.
        i = 1;
        --limit;
    }

    // Emit the ranges as pairs.
    ខណៈពេល (i < limit) {
        UChar32 start = list[i];  // getRangeStart()
        UChar32 end = list[i + 1] - 1;  // getRangeEnd() = range limit minus one
        ប្រសិនបើ (!(0xd800 <= end && end <= 0xdbff)) {
            _appendToPat(result, start, end, escapeUnprintable);
            i += 2;
        } បើមិនដូច្នេះ {
            // The range ends with a lead surrogate.
            // Avoid writing what looks like a lead+trail surrogate pair.
            // 1. Postpone ranges that start with a lead surrogate code point.
            int32_t firstLead = i;
            ខណៈពេល ((i += 2) < limit && list[i] <= 0xdbff) {}
            int32_t firstAfterLead = i;
            // 2. Write following ranges that start with a trail surrogate code point.
            ខណៈពេល (i < limit && (start = list[i]) <= 0xdfff) {
                _appendToPat(result, start, list[i + 1] - 1, escapeUnprintable);
                i += 2;
            }
            // 3. Now write the postponed ranges.
            សម្រាប់ (ចំនួនគត់ j = firstLead; j < firstAfterLead; j += 2) {
                _appendToPat(result, list[j], list[j + 1] - 1, escapeUnprintable);
            }
        }
    }

    ប្រសិនបើ (strings_ != nullptr) {
        សម្រាប់ (int32_t i = 0; i<strings_->size(); ++i) {
            result.append(u'{');
            _appendToPat(result,
                         *static_cast<const UnicodeString*>(strings_->elementAt(i)),
                         escapeUnprintable);
            result.append(u'}');
        }
    }
    ត្រឡប់ result.append(u']');
}

/**
* Release existing cached pattern
*/
void UnicodeSet::releasePattern() {
    ប្រសិនបើ (pat) {
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
    ប្រសិនបើ (pat) {
        patLen = newPatLen;
        u_memcpy(pat, newPat, patLen);
        pat[patLen] = 0;
    }
    // else we don't care if malloc failed. This was just a nice cache.
    // We can regenerate an equivalent pattern later when requested.
}

UnicodeSet* UnicodeSet::freeze() U_LIFETIME_BOUND {
    ប្រសិនបើ(!isFrozen() && !isBogus()) {
        compact();

        // Optimize contains() and span() and similar functions.
        ប្រសិនបើ (hasStrings()) {
            stringSpan = new UnicodeSetStringSpan(*this, *strings_, UnicodeSetStringSpan::ALL);
            ប្រសិនបើ (stringSpan == nullptr) {
                setToBogus();
                ត្រឡប់ this;
            } បើមិនដូច្នេះ ប្រសិនបើ (!stringSpan->needsStringSpanUTF16()) {
                // All strings are irrelevant for span() etc. because
                // all of each string's code points are contained in this set.
                // Do not check needsStringSpanUTF8() because UTF-8 has at most as
                // many relevant strings as UTF-16.
                // (Thus needsStringSpanUTF8() implies needsStringSpanUTF16().)
                delete stringSpan;
                stringSpan = nullptr;
            }
        }
        ប្រសិនបើ (stringSpan == nullptr) {
            // No span-relevant strings: Optimize for code point spans.
            bmpSet=new BMPSet(list, len);
            ប្រសិនបើ (bmpSet == nullptr) { // Check for memory allocation error.
                setToBogus();
            }
        }
    }
    ត្រឡប់ this;
}

int32_t UnicodeSet::span(const char16_t *s, int32_t length, USetSpanCondition spanCondition) const {
    ប្រសិនបើ(length>0 && bmpSet!=nullptr) {
        ត្រឡប់ static_cast<int32_t>(bmpSet->span(s, s + length, spanCondition) - s);
    }
    ប្រសិនបើ(length<0) {
        length=u_strlen(s);
    }
    ប្រសិនបើ(length==0) {
        ត្រឡប់ 0;
    }
    ប្រសិនបើ(stringSpan!=nullptr) {
        ត្រឡប់ stringSpan->span(s, length, spanCondition);
    } បើមិនដូច្នេះ ប្រសិនបើ(hasStrings()) {
        uint32_t which= spanCondition==USET_SPAN_NOT_CONTAINED ?
                            UnicodeSetStringSpan::FWD_UTF16_NOT_CONTAINED :
                            UnicodeSetStringSpan::FWD_UTF16_CONTAINED;
        UnicodeSetStringSpan strSpan(*this, *strings_, which);
        ប្រសិនបើ(strSpan.needsStringSpanUTF16()) {
            ត្រឡប់ strSpan.span(s, length, spanCondition);
        }
    }

    ប្រសិនបើ(spanCondition!=USET_SPAN_NOT_CONTAINED) {
        spanCondition=USET_SPAN_CONTAINED;  // Pin to 0/1 values.
    }

    UChar32 c;
    int32_t start=0, prev=0;
    do {
        U16_NEXT(s, start, length, c);
        ប្រសិនបើ(spanCondition!=contains(c)) {
            bha_nga;
        }
    } ខណៈពេល((prev=start)<length);
    ត្រឡប់ prev;
}

int32_t UnicodeSet::spanBack(const char16_t *s, int32_t length, USetSpanCondition spanCondition) const {
    ប្រសិនបើ(length>0 && bmpSet!=nullptr) {
        ត្រឡប់ static_cast<int32_t>(bmpSet->spanBack(s, s + length, spanCondition) - s);
    }
    ប្រសិនបើ(length<0) {
        length=u_strlen(s);
    }
    ប្រសិនបើ(length==0) {
        ត្រឡប់ 0;
    }
    ប្រសិនបើ(stringSpan!=nullptr) {
        ត្រឡប់ stringSpan->spanBack(s, length, spanCondition);
    } បើមិនដូច្នេះ ប្រសិនបើ(hasStrings()) {
        uint32_t which= spanCondition==USET_SPAN_NOT_CONTAINED ?
                            UnicodeSetStringSpan::BACK_UTF16_NOT_CONTAINED :
                            UnicodeSetStringSpan::BACK_UTF16_CONTAINED;
        UnicodeSetStringSpan strSpan(*this, *strings_, which);
        ប្រសិនបើ(strSpan.needsStringSpanUTF16()) {
            ត្រឡប់ strSpan.spanBack(s, length, spanCondition);
        }
    }

    ប្រសិនបើ(spanCondition!=USET_SPAN_NOT_CONTAINED) {
        spanCondition=USET_SPAN_CONTAINED;  // Pin to 0/1 values.
    }

    UChar32 c;
    int32_t prev=length;
    do {
        U16_PREV(s, 0, length, c);
        ប្រសិនបើ(spanCondition!=contains(c)) {
            bha_nga;
        }
    } ខណៈពេល((prev=length)>0);
    ត្រឡប់ prev;
}

int32_t UnicodeSet::spanUTF8(const var_na *s, int32_t length, USetSpanCondition spanCondition) const {
    ប្រសិនបើ(length>0 && bmpSet!=nullptr) {
        const uint8_t* s0 = reinterpret_cast<const uint8_t*>(s);
        ត្រឡប់ static_cast<int32_t>(bmpSet->spanUTF8(s0, length, spanCondition) - s0);
    }
    ប្រសិនបើ(length<0) {
        length = static_cast<int32_t>(uprv_strlen(s));
    }
    ប្រសិនបើ(length==0) {
        ត្រឡប់ 0;
    }
    ប្រសិនបើ(stringSpan!=nullptr) {
        ត្រឡប់ stringSpan->spanUTF8(reinterpret_cast<const uint8_t*>(s), length, spanCondition);
    } បើមិនដូច្នេះ ប្រសិនបើ(hasStrings()) {
        uint32_t which= spanCondition==USET_SPAN_NOT_CONTAINED ?
                            UnicodeSetStringSpan::FWD_UTF8_NOT_CONTAINED :
                            UnicodeSetStringSpan::FWD_UTF8_CONTAINED;
        UnicodeSetStringSpan strSpan(*this, *strings_, which);
        ប្រសិនបើ(strSpan.needsStringSpanUTF8()) {
            ត្រឡប់ strSpan.spanUTF8(reinterpret_cast<const uint8_t*>(s), length, spanCondition);
        }
    }

    ប្រសិនបើ(spanCondition!=USET_SPAN_NOT_CONTAINED) {
        spanCondition=USET_SPAN_CONTAINED;  // Pin to 0/1 values.
    }

    UChar32 c;
    int32_t start=0, prev=0;
    do {
        U8_NEXT_OR_FFFD(s, start, length, c);
        ប្រសិនបើ(spanCondition!=contains(c)) {
            bha_nga;
        }
    } ខណៈពេល((prev=start)<length);
    ត្រឡប់ prev;
}

int32_t UnicodeSet::spanBackUTF8(const var_na *s, int32_t length, USetSpanCondition spanCondition) const {
    ប្រសិនបើ(length>0 && bmpSet!=nullptr) {
        const uint8_t* s0 = reinterpret_cast<const uint8_t*>(s);
        ត្រឡប់ bmpSet->spanBackUTF8(s0, length, spanCondition);
    }
    ប្រសិនបើ(length<0) {
        length = static_cast<int32_t>(uprv_strlen(s));
    }
    ប្រសិនបើ(length==0) {
        ត្រឡប់ 0;
    }
    ប្រសិនបើ(stringSpan!=nullptr) {
        ត្រឡប់ stringSpan->spanBackUTF8(reinterpret_cast<const uint8_t*>(s), length, spanCondition);
    } បើមិនដូច្នេះ ប្រសិនបើ(hasStrings()) {
        uint32_t which= spanCondition==USET_SPAN_NOT_CONTAINED ?
                            UnicodeSetStringSpan::BACK_UTF8_NOT_CONTAINED :
                            UnicodeSetStringSpan::BACK_UTF8_CONTAINED;
        UnicodeSetStringSpan strSpan(*this, *strings_, which);
        ប្រសិនបើ(strSpan.needsStringSpanUTF8()) {
            ត្រឡប់ strSpan.spanBackUTF8(reinterpret_cast<const uint8_t*>(s), length, spanCondition);
        }
    }

    ប្រសិនបើ(spanCondition!=USET_SPAN_NOT_CONTAINED) {
        spanCondition=USET_SPAN_CONTAINED;  // Pin to 0/1 values.
    }

    UChar32 c;
    int32_t prev=length;
    do {
        U8_PREV_OR_FFFD(s, 0, length, c);
        ប្រសិនបើ(spanCondition!=contains(c)) {
            bha_nga;
        }
    } ខណៈពេល((prev=length)>0);
    ត្រឡប់ prev;
}

U_NAMESPACE_END
