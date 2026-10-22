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
    གལ་སྲིད (c < UNICODESET_LOW) {
        c = UNICODESET_LOW;
    } དེ་མིན གལ་སྲིད (c > (UNICODESET_HIGH-1)) {
        c = (UNICODESET_HIGH-1);
    }
    ལོག c;
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

#དེ་མིན

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
    ལོག a.compare(b);
}

UBool UnicodeSet::hasStrings() const {
    ལོག strings_ != nullptr && !strings_->isEmpty();
}

int32_t UnicodeSet::stringsSize() const {
    ལོག strings_ == nullptr ? 0 : strings_->size();
}

UBool UnicodeSet::stringsContains(const UnicodeString &s) const {
    ལོག strings_ != nullptr && strings_->contains((void*) &s);
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
    གལ་སྲིད (ensureCapacity(o.len)) {
        // *this = o except for bmpSet and stringSpan
        len = o.len;
        uprv_memcpy(list, o.list, (size_t)len*parimaa_na(UChar32));
        གལ་སྲིད (o.hasStrings()) {
            UErrorCode status = U_ZERO_ERROR;
            གལ་སྲིད (!allocateStrings(status) ||
                    (strings_->assign(*o.strings_, cloneUnicodeString, status), U_FAILURE(status))) {
                setToBogus();
                ལོག;
            }
        }
        གལ་སྲིད (o.pat) {
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
    གལ་སྲིད (list != stackList) {
        uprv_free(list);
    }
    delete bmpSet;
    གལ་སྲིད (buffer != stackList) {
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
    ལོག copyFrom(o, false);
}

UnicodeSet& UnicodeSet::copyFrom(const UnicodeSet& o, UBool asThawed) U_LIFETIME_BOUND {
    གལ་སྲིད (this == &o) {
        ལོག *this;
    }
    གལ་སྲིད (isFrozen()) {
        ལོག *this;
    }
    གལ་སྲིད (o.isBogus()) {
        setToBogus();
        ལོག *this;
    }
    གལ་སྲིད (!ensureCapacity(o.len)) {
        // ensureCapacity will mark the UnicodeSet as Bogus if OOM failure happens.
        ལོག *this;
    }
    len = o.len;
    uprv_memcpy(list, o.list, (size_t)len*parimaa_na(UChar32));
    གལ་སྲིད (o.bmpSet != nullptr && !asThawed) {
        bmpSet = new BMPSet(*o.bmpSet, list, len);
        གལ་སྲིད (bmpSet == nullptr) { // Check for memory allocation error.
            setToBogus();
            ལོག *this;
        }
    }
    གལ་སྲིད (o.hasStrings()) {
        UErrorCode status = U_ZERO_ERROR;
        གལ་སྲིད ((strings_ == nullptr && !allocateStrings(status)) ||
                (strings_->assign(*o.strings_, cloneUnicodeString, status), U_FAILURE(status))) {
            setToBogus();
            ལོག *this;
        }
    } དེ་མིན གལ་སྲིད (hasStrings()) {
        strings_->removeAllElements();
    }
    གལ་སྲིད (o.stringSpan != nullptr && strings_ != nullptr && !asThawed) {
        stringSpan = new UnicodeSetStringSpan(*o.stringSpan, *strings_);
        གལ་སྲིད (stringSpan == nullptr) { // Check for memory allocation error.
            setToBogus();
            ལོག *this;
        }
    }
    releasePattern();
    གལ་སྲིད (o.pat) {
        setPattern(o.pat, o.patLen);
    }
    ལོག *this;
}

/**
 * Returns a copy of this object.  All UnicodeMatcher objects have
 * to support cloning in order to allow classes using
 * UnicodeMatchers, such as Transliterator, to implement cloning.
 */
UnicodeSet* UnicodeSet::clone() const {
    ལོག new UnicodeSet(*this);
}

UnicodeSet *UnicodeSet::cloneAsThawed() const {
    ལོག new UnicodeSet(*this, true);
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
    གལ་སྲིད (len != o.len) ལོག false;
    དོན་ལུ (int32_t i = 0; i < len; ++i) {
        གལ་སྲིད (list[i] != o.list[i]) ལོག false;
    }
    གལ་སྲིད (hasStrings() != o.hasStrings()) { ལོག false; }
    གལ་སྲིད (hasStrings() && *strings_ != *o.strings_) ལོག false;
    ལོག true;
}

/**
 * Returns the hash code value for this set.
 *
 * @return the hash code value for this set.
 * @see Object#hashCode()
 */
int32_t UnicodeSet::hashCode() const {
    uint32_t result = static_cast<uint32_t>(len);
    དོན་ལུ (int32_t i = 0; i < len; ++i) {
        result *= 1000003u;
        result += list[i];
    }
    ལོག static_cast<int32_t>(result);
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
    དོན་ལུ (int32_t i = 0; i < count; ++i) {
        n += getRangeEnd(i) - getRangeStart(i) + 1;
    }
    ལོག n + stringsSize();
}

/**
 * Returns <tt>true</tt> if this set contains no elements.
 *
 * @return <tt>true</tt> if this set contains no elements.
 */
UBool UnicodeSet::isEmpty() const {
    ལོག len == 1 && !hasStrings();
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
    གལ་སྲིད (bmpSet != nullptr) {
        ལོག bmpSet->contains(c);
    }
    གལ་སྲིད (stringSpan != nullptr) {
        ལོག stringSpan->contains(c);
    }
    གལ་སྲིད (c >= UNICODESET_HIGH) { // Don't need to check LOW bound
        ལོག false;
    }
    int32_t i = findCodePoint(c);
    ལོག i & 1; // return true if odd
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
    གལ་སྲིད (c < list[0])
        ལོག 0;
    // High runner test.  c is often after the last range, so an
    // initial check for this condition pays off.
    int32_t lo = 0;
    int32_t hi = len - 1;
    གལ་སྲིད (lo >= hi || c >= list[hi-1])
        ལོག hi;
    // invariant: c >= list[lo]
    // invariant: c < list[hi]
    དོན་ལུ (;;) {
        int32_t i = (lo + hi) >> 1;
        གལ་སྲིད (i == lo) {
            bha_nga; // Found!
        } དེ་མིན གལ་སྲིད (c < list[i]) {
            hi = i;
        } དེ་མིན {
            lo = i;
        }
    }
    ལོག hi;
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
    ལོག ((i & 1) != 0 && end < list[i]);
}

/**
 * Returns <tt>true</tt> if this set contains the given
 * multicharacter string.
 * @param s string to be checked for containment
 * @return <tt>true</tt> if this set contains the specified string
 */
UBool UnicodeSet::contains(const UnicodeString& s) const {
    int32_t cp = getSingleCP(s);
    གལ་སྲིད (cp < 0) {
        ལོག stringsContains(s);
    } དེ་མིན {
        ལོག contains(static_cast<UChar32>(cp));
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
    དོན་ལུ (ཧྲིལ i=0; i<n; ++i) {
        གལ་སྲིད (!contains(c.getRangeStart(i), c.getRangeEnd(i))) {
            ལོག false;
        }
    }
    ལོག !c.hasStrings() || (strings_ != nullptr && strings_->containsAll(*c.strings_));
}

/**
 * Returns true if this set contains all the characters
 * of the given string.
 * @param s string containing characters to be checked for containment
 * @return true if the test condition is met
 */
UBool UnicodeSet::containsAll(const UnicodeString& s) const {
    ལོག span(s.getBuffer(), s.length(), USET_SPAN_CONTAINED) == s.length();
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
    ལོག ((i & 1) == 0 && end < list[i]);
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
    དོན་ལུ (int32_t i=0; i<n; ++i) {
        གལ་སྲིད (!containsNone(c.getRangeStart(i), c.getRangeEnd(i))) {
            ལོག false;
        }
    }
    ལོག strings_ == nullptr || !c.hasStrings() || strings_->containsNone(*c.strings_);
}

/**
 * Returns true if this set contains none of the characters
 * of the given string.
 * @param s string containing characters to be checked for containment
 * @return true if the test condition is met
 */
UBool UnicodeSet::containsNone(const UnicodeString& s) const {
    ལོག span(s.getBuffer(), s.length(), USET_SPAN_NOT_CONTAINED) == s.length();
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
    དོན་ལུ (i=0; i<rangeCount; ++i) {
        UChar32 low = getRangeStart(i);
        UChar32 high = getRangeEnd(i);
        གལ་སྲིད ((low & ~0xFF) == (high & ~0xFF)) {
            གལ་སྲིད ((low & 0xFF) <= v && v <= (high & 0xFF)) {
                ལོག true;
            }
        } དེ་མིན གལ་སྲིད ((low & 0xFF) <= v || v <= (high & 0xFF)) {
            ལོག true;
        }
    }
    གལ་སྲིད (hasStrings()) {
        དོན་ལུ (i=0; i<strings_->size(); ++i) {
            const UnicodeString& s = *static_cast<const UnicodeString*>(strings_->elementAt(i));
            གལ་སྲིད (s.isEmpty()) {
                continue;  // skip the empty string
            }
            UChar32 c = s.char32At(0);
            གལ་སྲིད ((c & 0xFF) == v) {
                ལོག true;
            }
        }
    }
    ལོག false;
}

/**
 * Implementation of UnicodeMatcher::matches().  Always matches the
 * longest possible multichar string.
 */
UMatchDegree UnicodeSet::matches(const Replaceable& text,
                                 int32_t& offset,
                                 int32_t limit,
                                 UBool incremental) {
    གལ་སྲིད (offset == limit) {
        གལ་སྲིད (contains(U_ETHER)) {
            ལོག incremental ? U_PARTIAL_MATCH : U_MATCH;
        } དེ་མིན {
            ལོག U_MISMATCH;
        }
    } དེ་མིན {
        གལ་སྲིད (hasStrings()) { // try strings first

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

            དོན་ལུ (i=0; i<strings_->size(); ++i) {
                const UnicodeString& trial = *static_cast<const UnicodeString*>(strings_->elementAt(i));
                གལ་སྲིད (trial.isEmpty()) {
                    continue;  // skip the empty string
                }

                char16_t c = trial.charAt(forward ? 0 : trial.length() - 1);

                // Strings are sorted, so we can optimize in the
                // forward direction.
                གལ་སྲིད (forward && c > firstChar) bha_nga;
                གལ་སྲིད (c != firstChar) continue;

                int32_t matchLen = matchRest(text, offset, limit, trial);

                གལ་སྲིད (incremental) {
                    int32_t maxLen = forward ? limit-offset : offset-limit;
                    གལ་སྲིད (matchLen == maxLen) {
                        // We have successfully matched but only up to limit.
                        ལོག U_PARTIAL_MATCH;
                    }
                }

                གལ་སྲིད (matchLen == trial.length()) {
                    // We have successfully matched the whole string.
                    གལ་སྲིད (matchLen > highWaterLength) {
                        highWaterLength = matchLen;
                    }
                    // In the forward direction we know strings
                    // are sorted so we can bail early.
                    གལ་སྲིད (forward && matchLen < highWaterLength) {
                        bha_nga;
                    }
                    continue;
                }
            }

            // We've checked all strings without a partial match.
            // If we have full matches, return the longest one.
            གལ་སྲིད (highWaterLength != 0) {
                offset += forward ? highWaterLength : -highWaterLength;
                ལོག U_MATCH;
            }
        }
        ལོག UnicodeFilter::matches(text, offset, limit, incremental);
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
    གལ་སྲིད (start < limit) {
        maxLen = limit - start;
        གལ་སྲིད (maxLen > slen) maxLen = slen;
        དོན་ལུ (i = 1; i < maxLen; ++i) {
            གལ་སྲིད (text.charAt(start + i) != s.charAt(i)) ལོག 0;
        }
    } དེ་མིན {
        maxLen = start - limit;
        གལ་སྲིད (maxLen > slen) maxLen = slen;
        --slen; // <=> slen = s.length() - 1;
        དོན་ལུ (i = 1; i < maxLen; ++i) {
            གལ་སྲིད (text.charAt(start - i) != s.charAt(slen - i)) ལོག 0;
        }
    }
    ལོག maxLen;
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
    གལ་སྲིད (c < MIN_VALUE || c > MAX_VALUE) {
        ལོག -1;
    }
    int32_t i = 0;
    int32_t n = 0;
    དོན་ལུ (;;) {
        UChar32 start = list[i++];
        གལ་སྲིད (c < start) {
            ལོག -1;
        }
        UChar32 limit = list[i++];
        གལ་སྲིད (c < limit) {
            ལོག n + c - start;
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
    གལ་སྲིད (index >= 0) {
        // len2 is the largest even integer <= len, that is, it is len
        // for even values and len-1 for odd values.  With odd values
        // the last entry is UNICODESET_HIGH.
        int32_t len2 = len & ~1;
        དོན་ལུ (int32_t i=0; i < len2;) {
            UChar32 start = list[i++];
            int32_t count = list[i++] - start;
            གལ་སྲིད (index < count) {
                ལོག static_cast<UChar32>(start + index);
            }
            index -= count;
        }
    }
    ལོག static_cast<UChar32>(-1);
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
    ལོག *this;
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
    གལ་སྲིད (pinCodePoint(start) < pinCodePoint(end)) {
        UChar32 limit = end + 1;
        // Fast path for adding a new range after the last one.
        // Odd list length: [..., lastStart, lastLimit, HIGH]
        གལ་སྲིད ((len & 1) != 0) {
            // If the list is empty, set lastLimit low enough to not be adjacent to 0.
            UChar32 lastLimit = len == 1 ? -2 : list[len - 2];
            གལ་སྲིད (lastLimit <= start && !isFrozen() && !isBogus()) {
                གལ་སྲིད (lastLimit == start) {
                    // Extend the last range.
                    list[len - 2] = limit;
                    གལ་སྲིད (limit == UNICODESET_HIGH) {
                        --len;
                    }
                } དེ་མིན {
                    list[len - 1] = start;
                    གལ་སྲིད (limit < UNICODESET_HIGH) {
                        གལ་སྲིད (ensureCapacity(len + 2)) {
                            list[len++] = limit;
                            list[len++] = UNICODESET_HIGH;
                        }
                    } དེ་མིན {  // limit == UNICODESET_HIGH
                        གལ་སྲིད (ensureCapacity(len + 1)) {
                            list[len++] = UNICODESET_HIGH;
                        }
                    }
                }
                releasePattern();
                ལོག *this;
            }
        }
        // This is slow. Could be much faster using findCodePoint(start)
        // and modifying the list, dealing with adjacent & overlapping ranges.
        UChar32 range[3] = { start, limit, UNICODESET_HIGH };
        add(range, 2, 0);
    } དེ་མིན གལ་སྲིད (start == end) {
        add(start);
    }
    ལོག *this;
}

// #define DEBUG_US_ADD

#ifdef DEBUG_US_ADD
#include <stdio.h>
void dump(UChar32 c) {
    གལ་སྲིད (c <= 0xFF) {
        printf("%c", (var_na)c);
    } དེ་མིན {
        printf("U+%04X", c);
    }
}
void dump(const UChar32* list, int32_t len) {
    printf("[");
    དོན་ལུ (int32_t i=0; i<len; ++i) {
        གལ་སྲིད (i != 0) printf(", ");
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
    གལ་སྲིད ((i & 1) != 0  || isFrozen() || isBogus()) ལོག *this;

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

    གལ་སྲིད (c == list[i]-1) {
        // c is before start of next range
        list[i] = c;
        // if we touched the HIGH mark, then add a new one
        གལ་སྲིད (c == (UNICODESET_HIGH - 1)) {
            གལ་སྲིད (!ensureCapacity(len+1)) {
                // ensureCapacity will mark the object as Bogus if OOM failure happens.
                ལོག *this;
            }
            list[len++] = UNICODESET_HIGH;
        }
        གལ་སྲིད (i > 0 && c == list[i-1]) {
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
            སྐབས (src < srclimit) *(dst++) = *(src++);

            len -= 2;
        }
    }

    དེ་མིན གལ་སྲིད (i > 0 && c == list[i-1]) {
        // c is after end of prior range
        list[i-1]++;
        // no need to check for collapse here
    }

    དེ་མིན {
        // At this point we know the new char is not adjacent to
        // any existing ranges, and it is not 10FFFF.


        // [..., start_k-1, limit_k-1, start_k, limit_k, ..., HIGH]
        //                             ^
        //                             list[i]

        // [..., start_k-1, limit_k-1, c, c+1, start_k, limit_k, ..., HIGH]
        //                             ^
        //                             list[i]

        གལ་སྲིད (!ensureCapacity(len+2)) {
            // ensureCapacity will mark the object as Bogus if OOM failure happens.
            ལོག *this;
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

    དོན་ལུ (i=1; i<len; ++i) {
        གལ་སྲིད (list[i] <= list[i-1]) {
            // Corrupt array!
            printf("ERROR: list has been corrupted\n");
            exit(1);
        }
    }
#endif

    releasePattern();
    ལོག *this;
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
    གལ་སྲིད (isFrozen() || isBogus()) ལོག *this;
    int32_t cp = getSingleCP(s);
    གལ་སྲིད (cp < 0) {
        གལ་སྲིད (!stringsContains(s)) {
            _add(s);
            releasePattern();
        }
    } དེ་མིན {
        add(static_cast<UChar32>(cp));
    }
    ལོག *this;
}

/**
 * Adds the given string, in order, to 'strings_'.  The given string
 * must have been checked by the caller to not already be in 'strings_'.
 */
void UnicodeSet::_add(const UnicodeString& s) {
    གལ་སྲིད (isFrozen() || isBogus()) {
        ལོག;
    }
    UErrorCode ec = U_ZERO_ERROR;
    གལ་སྲིད (strings_ == nullptr && !allocateStrings(ec)) {
        setToBogus();
        ལོག;
    }
    LocalPointer<UnicodeString> t(new UnicodeString(s));
    གལ་སྲིད (t.isNull()) { // Check for memory allocation error.
        setToBogus();
        ལོག;
    }
    strings_->sortedInsert(t.orphan(), compareUnicodeString, ec);
    གལ་སྲིད (U_FAILURE(ec)) {
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
    གལ་སྲིད (sLength == 1) ལོག s.charAt(0);
    གལ་སྲིད (sLength == 2) {
        UChar32 cp = s.char32At(0);
        གལ་སྲིད (cp > 0xFFFF) { // is surrogate pair
            ལོག cp;
        }
    }
    ལོག -1;
}

/**
 * Adds each of the characters in this string to the set. Thus "ch" => {"c", "h"}
 * If this set already any particular character, it has no effect on that character.
 * @param the source string
 * @return the modified set, for chaining
 */
UnicodeSet& UnicodeSet::addAll(const UnicodeString& s) U_LIFETIME_BOUND {
    UChar32 cp;
    དོན་ལུ (int32_t i = 0; i < s.length(); i += U16_LENGTH(cp)) {
        cp = s.char32At(i);
        add(cp);
    }
    ལོག *this;
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
    ལོག *this;
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
    ལོག *this;
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
    ལོག *this;
}

UnicodeSet& UnicodeSet::removeAllStrings() U_LIFETIME_BOUND {
    གལ་སྲིད (!isFrozen() && hasStrings()) {
        strings_->removeAllElements();
        releasePattern();
    }
    ལོག *this;
}


/**
 * Makes a set from a multicharacter string. Thus "ch" => {"ch"}
 * <br><b>Warning: you cannot add an empty string ("") to a UnicodeSet.</b>
 * @param the source string
 * @return a newly created set containing the given string
 */
UnicodeSet* U_EXPORT2 UnicodeSet::createFrom(const UnicodeString& s) {
    UnicodeSet *set = new UnicodeSet();
    གལ་སྲིད (set != nullptr) { // Check for memory allocation error.
        set->add(s);
    }
    ལོག set;
}


/**
 * Makes a set from each of the characters in the string. Thus "ch" => {"c", "h"}
 * @param the source string
 * @return a newly created set containing the given characters
 */
UnicodeSet* U_EXPORT2 UnicodeSet::createFromAll(const UnicodeString& s) {
    UnicodeSet *set = new UnicodeSet();
    གལ་སྲིད (set != nullptr) { // Check for memory allocation error.
        set->addAll(s);
    }
    ལོག set;
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
    གལ་སྲིད (pinCodePoint(start) <= pinCodePoint(end)) {
        UChar32 range[3] = { start, end+1, UNICODESET_HIGH };
        retain(range, 2, 0);
    } དེ་མིན {
        clear();
    }
    ལོག *this;
}

UnicodeSet& UnicodeSet::retain(UChar32 c) U_LIFETIME_BOUND {
    ལོག retain(c, c);
}

UnicodeSet& UnicodeSet::retain(const UnicodeString &s) U_LIFETIME_BOUND {
    གལ་སྲིད (isFrozen() || isBogus()) { ལོག *this; }
    UChar32 cp = getSingleCP(s);
    གལ་སྲིད (cp < 0) {
        bool isIn = stringsContains(s);
        // Check for getRangeCount() first to avoid somewhat-expensive size()
        // when there are single code points.
        གལ་སྲིད (isIn && getRangeCount() == 0 && size() == 1) {
            ལོག *this;
        }
        clear();
        གལ་སྲིད (isIn) {
            _add(s);
        }
    } དེ་མིན {
        retain(cp, cp);
    }
    ལོག *this;
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
    གལ་སྲིད (pinCodePoint(start) <= pinCodePoint(end)) {
        UChar32 range[3] = { start, end+1, UNICODESET_HIGH };
        retain(range, 2, 2);
    }
    ལོག *this;
}

/**
 * Removes the specified character from this set if it is present.
 * The set will not contain the specified range once the call
 * returns.
 */
UnicodeSet& UnicodeSet::remove(UChar32 c) U_LIFETIME_BOUND {
    ལོག remove(c, c);
}

/**
 * Removes the specified string from this set if it is present.
 * The set will not contain the specified character once the call
 * returns.
 * @param the source string
 * @return the modified set, for chaining
 */
UnicodeSet& UnicodeSet::remove(const UnicodeString& s) U_LIFETIME_BOUND {
    གལ་སྲིད (isFrozen() || isBogus()) ལོག *this;
    int32_t cp = getSingleCP(s);
    གལ་སྲིད (cp < 0) {
        གལ་སྲིད (strings_ != nullptr && strings_->removeElement((void*) &s)) {
            releasePattern();
        }
    } དེ་མིན {
        remove(static_cast<UChar32>(cp), static_cast<UChar32>(cp));
    }
    ལོག *this;
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
    གལ་སྲིད (isFrozen() || isBogus()) {
        ལོག *this;
    }
    གལ་སྲིད (pinCodePoint(start) <= pinCodePoint(end)) {
        UChar32 range[3] = { start, end+1, UNICODESET_HIGH };
        exclusiveOr(range, 2, 0);
    }
    releasePattern();
    ལོག *this;
}

UnicodeSet& UnicodeSet::complement(UChar32 c) U_LIFETIME_BOUND {
    ལོག complement(c, c);
}

/**
 * This is equivalent to
 * <code>complement(MIN_VALUE, MAX_VALUE)</code>.
 */
UnicodeSet& UnicodeSet::complement() U_LIFETIME_BOUND {
    གལ་སྲིད (isFrozen() || isBogus()) {
        ལོག *this;
    }
    གལ་སྲིད (list[0] == UNICODESET_LOW) {
        uprv_memmove(list, list + 1, (size_t)(len-1)*parimaa_na(UChar32));
        --len;
    } དེ་མིན {
        གལ་སྲིད (!ensureCapacity(len+1)) {
            ལོག *this;
        }
        uprv_memmove(list + 1, list, (size_t)len*parimaa_na(UChar32));
        list[0] = UNICODESET_LOW;
        ++len;
    }
    releasePattern();
    ལོག *this;
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
    གལ་སྲིད (isFrozen() || isBogus()) ལོག *this;
    int32_t cp = getSingleCP(s);
    གལ་སྲིད (cp < 0) {
        གལ་སྲིད (stringsContains(s)) {
            strings_->removeElement((void*) &s);
        } དེ་མིན {
            _add(s);
        }
        releasePattern();
    } དེ་མིན {
        complement(static_cast<UChar32>(cp), static_cast<UChar32>(cp));
    }
    ལོག *this;
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
    གལ་སྲིད ( c.len>0 && c.list!=nullptr ) {
        add(c.list, c.len, 0);
    }

    // Add strings in order
    གལ་སྲིད ( c.strings_!=nullptr ) {
        དོན་ལུ (int32_t i=0; i<c.strings_->size(); ++i) {
            const UnicodeString* s = static_cast<const UnicodeString*>(c.strings_->elementAt(i));
            གལ་སྲིད (!stringsContains(*s)) {
                _add(*s);
            }
        }
    }
    ལོག *this;
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
    གལ་སྲིད (isFrozen() || isBogus()) {
        ལོག *this;
    }
    retain(c.list, c.len, 0);
    གལ་སྲིད (hasStrings()) {
        གལ་སྲིད (!c.hasStrings()) {
            strings_->removeAllElements();
        } དེ་མིན {
            strings_->retainAll(*c.strings_);
        }
    }
    ལོག *this;
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
    གལ་སྲིད (isFrozen() || isBogus()) {
        ལོག *this;
    }
    retain(c.list, c.len, 2);
    གལ་སྲིད (hasStrings() && c.hasStrings()) {
        strings_->removeAll(*c.strings_);
    }
    ལོག *this;
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
    གལ་སྲིད (isFrozen() || isBogus()) {
        ལོག *this;
    }
    exclusiveOr(c.list, c.len, 0);

    གལ་སྲིད (c.strings_ != nullptr) {
        དོན་ལུ (int32_t i=0; i<c.strings_->size(); ++i) {
            void* e = c.strings_->elementAt(i);
            གལ་སྲིད (strings_ == nullptr || !strings_->removeElement(e)) {
                _add(*static_cast<const UnicodeString*>(e));
            }
        }
    }
    ལོག *this;
}

/**
 * Removes all of the elements from this set.  This set will be
 * empty after this call returns.
 */
UnicodeSet& UnicodeSet::clear() U_LIFETIME_BOUND {
    གལ་སྲིད (isFrozen()) {
        ལོག *this;
    }
    list[0] = UNICODESET_HIGH;
    len = 1;
    releasePattern();
    གལ་སྲིད (strings_ != nullptr) {
        strings_->removeAllElements();
    }
    // Remove bogus
    fFlags = 0;
    ལོག *this;
}

/**
 * Iteration method that returns the number of ranges contained in
 * this set.
 * @see #getRangeStart
 * @see #getRangeEnd
 */
int32_t UnicodeSet::getRangeCount() const {
    ལོག len/2;
}

/**
 * Iteration method that returns the first character in the
 * specified range of this set.
 * @see #getRangeCount
 * @see #getRangeEnd
 */
UChar32 UnicodeSet::getRangeStart(int32_t index) const {
    ལོག list[index*2];
}

/**
 * Iteration method that returns the last character in the
 * specified range of this set.
 * @see #getRangeStart
 * @see #getRangeEnd
 */
UChar32 UnicodeSet::getRangeEnd(int32_t index) const {
    ལོག list[index*2 + 1] - 1;
}

const UnicodeString* UnicodeSet::getString(int32_t index) const U_LIFETIME_BOUND {
    ལོག static_cast<const UnicodeString*>(strings_->elementAt(index));
}

/**
 * Reallocate this objects internal structures to take up the least
 * possible space, without changing this object's value.
 */
UnicodeSet& UnicodeSet::compact() U_LIFETIME_BOUND {
    གལ་སྲིད (isFrozen() || isBogus()) {
        ལོག *this;
    }
    // Delete buffer first to defragment memory less.
    གལ་སྲིད (buffer != stackList) {
        uprv_free(buffer);
        buffer = nullptr;
        bufferCapacity = 0;
    }
    གལ་སྲིད (list == stackList) {
        // pass
    } དེ་མིན གལ་སྲིད (len <= INITIAL_CAPACITY) {
        uprv_memcpy(stackList, list, len * parimaa_na(UChar32));
        uprv_free(list);
        list = stackList;
        capacity = INITIAL_CAPACITY;
    } དེ་མིན གལ་སྲིད ((len + 7) < capacity) {
        // If we have more than a little unused capacity, shrink it to len.
        UChar32* temp = static_cast<UChar32*>(uprv_realloc(list, parimaa_na(UChar32) * len));
        གལ་སྲིད (temp) {
            list = temp;
            capacity = len;
        }
        // else what the heck happened?! We allocated less memory!
        // Oh well. We'll keep our original array.
    }
    གལ་སྲིད (strings_ != nullptr && strings_->isEmpty()) {
        delete strings_;
        strings_ = nullptr;
    }
    ལོག *this;
}

#ifdef DEBUG_SERIALIZE
#include <stdio.h>
#endif

/**
 * Deserialize constructor.
 */
UnicodeSet::UnicodeSet(const uint16_t data[], int32_t dataLen, ESerialization serialization,
                       UErrorCode &ec) {

  གལ་སྲིད(U_FAILURE(ec)) {
    setToBogus();
    ལོག;
  }

  གལ་སྲིད( (serialization != kSerialized)
      || (data==nullptr)
      || (dataLen < 1)) {
    ec = U_ILLEGAL_ARGUMENT_ERROR;
    setToBogus();
    ལོག;
  }

  // bmp?
  int32_t headerSize = ((data[0]&0x8000)) ?2:1;
  int32_t bmpLength = (headerSize==1)?data[0]:data[1];

  int32_t newLength = (((data[0]&0x7FFF)-bmpLength)/2)+bmpLength;
#ifdef DEBUG_SERIALIZE
  printf("dataLen %d headerSize %d bmpLen %d len %d. data[0]=%X/%X/%X/%X\n", dataLen,headerSize,bmpLength,newLength, data[0],data[1],data[2],data[3]);
#endif
  གལ་སྲིད(!ensureCapacity(newLength + 1)) {  // +1 for HIGH
    ལོག;
  }
  // copy bmp
  int32_t i;
  དོན་ལུ(i = 0; i< bmpLength;i++) {
    list[i] = data[i+headerSize];
#ifdef DEBUG_SERIALIZE
    printf("<<16@%d[%d] %X\n", i+headerSize, i, list[i]);
#endif
  }
  // copy smp
  དོན་ལུ(i=bmpLength;i<newLength;i++) {
    list[i] = (static_cast<UChar32>(data[headerSize + bmpLength + (i - bmpLength) * 2 + 0]) << 16) +
               static_cast<UChar32>(data[headerSize + bmpLength + (i - bmpLength) * 2 + 1]);
#ifdef DEBUG_SERIALIZE
    printf("<<32@%d+[%d] %lX\n", headerSize+bmpLength+i, i, list[i]);
#endif
  }
  U_ASSERT(i == newLength);
  གལ་སྲིད (i == 0 || list[i - 1] != UNICODESET_HIGH) {
    list[i++] = UNICODESET_HIGH;
  }
  len = i;
}


int32_t UnicodeSet::serialize(uint16_t *dest, int32_t destCapacity, UErrorCode& ec) const {
    int32_t bmpLength, length, destLength;

    གལ་སྲིད (U_FAILURE(ec)) {
        ལོག 0;
    }

    གལ་སྲིད (destCapacity<0 || (destCapacity>0 && dest==nullptr)) {
        ec=U_ILLEGAL_ARGUMENT_ERROR;
        ལོག 0;
    }

    /* count necessary 16-bit units */
    length=this->len-1; // Subtract 1 to ignore final UNICODESET_HIGH
    // assert(length>=0);
    གལ་སྲིད (length==0) {
        /* empty set */
        གལ་སྲིད (destCapacity>0) {
            *dest=0;
        } དེ་མིན {
            ec=U_BUFFER_OVERFLOW_ERROR;
        }
        ལོག 1;
    }
    /* now length>0 */

    གལ་སྲིད (this->list[length-1]<=0xffff) {
        /* all BMP */
        bmpLength=length;
    } དེ་མིན གལ་སྲིད (this->list[0]>=0x10000) {
        /* all supplementary */
        bmpLength=0;
        length*=2;
    } དེ་མིན {
        /* some BMP, some supplementary */
        དོན་ལུ (bmpLength=0; bmpLength<length && this->list[bmpLength]<=0xffff; ++bmpLength) {}
        length=bmpLength+2*(length-bmpLength);
    }
#ifdef DEBUG_SERIALIZE
    printf(">> bmpLength%d length%d len%d\n", bmpLength, length, len);
#endif
    /* length: number of 16-bit array units */
    གལ་སྲིད (length>0x7fff) {
        /* there are only 15 bits for the length in the first serialized word */
        ec=U_INDEX_OUTOFBOUNDS_ERROR;
        ལོག 0;
    }

    /*
     * total serialized length:
     * number of 16-bit array units (length) +
     * 1 length unit (always) +
     * 1 bmpLength unit (if there are supplementary values)
     */
    destLength=length+((length>bmpLength)?2:1);
    གལ་སྲིད (destLength<=destCapacity) {
        const UChar32 *p;
        int32_t i;

#ifdef DEBUG_SERIALIZE
        printf("writeHdr\n");
#endif
        *dest = static_cast<uint16_t>(length);
        གལ་སྲིད (length>bmpLength) {
            *dest|=0x8000;
            *++dest = static_cast<uint16_t>(bmpLength);
        }
        ++dest;

        /* write the BMP part of the array */
        p=this->list;
        དོན་ལུ (i=0; i<bmpLength; ++i) {
#ifdef DEBUG_SERIALIZE
          printf("writebmp: %x\n", (ཧྲིལ)*p);
#endif
            *dest++ = static_cast<uint16_t>(*p++);
        }

        /* write the supplementary part of the array */
        དོན་ལུ (; i<length; i+=2) {
#ifdef DEBUG_SERIALIZE
          printf("write32: %x\n", (ཧྲིལ)*p);
#endif
            *dest++ = static_cast<uint16_t>(*p >> 16);
            *dest++ = static_cast<uint16_t>(*p++);
        }
    } དེ་མིན {
        ec=U_BUFFER_OVERFLOW_ERROR;
    }
    ལོག destLength;
}

//----------------------------------------------------------------
// Implementation: Utility methods
//----------------------------------------------------------------

/**
 * Allocate our strings vector and return true if successful.
 */
UBool UnicodeSet::allocateStrings(UErrorCode &status) {
    གལ་སྲིད (U_FAILURE(status)) {
        ལོག false;
    }
    strings_ = new UVector(uprv_deleteUObject,
                          uhash_compareUnicodeString, 1, status);
    གལ་སྲིད (strings_ == nullptr) { // Check for memory allocation error.
        status = U_MEMORY_ALLOCATION_ERROR;
        ལོག false;
    }
    གལ་སྲིད (U_FAILURE(status)) {
        delete strings_;
        strings_ = nullptr;
        ལོག false;
    } 
    ལོག true;
}

int32_t UnicodeSet::nextCapacity(int32_t minCapacity) {
    // Grow exponentially to reduce the frequency of allocations.
    གལ་སྲིད (minCapacity < INITIAL_CAPACITY) {
        ལོག minCapacity + INITIAL_CAPACITY;
    } དེ་མིན གལ་སྲིད (minCapacity <= 2500) {
        ལོག 5 * minCapacity;
    } དེ་མིན {
        int32_t newCapacity = 2 * minCapacity;
        གལ་སྲིད (newCapacity > MAX_LENGTH) {
            newCapacity = MAX_LENGTH;
        }
        ལོག newCapacity;
    }
}

bool UnicodeSet::ensureCapacity(int32_t newLen) {
    གལ་སྲིད (newLen > MAX_LENGTH) {
        newLen = MAX_LENGTH;
    }
    གལ་སྲིད (newLen <= capacity) {
        ལོག true;
    }
    int32_t newCapacity = nextCapacity(newLen);
    UChar32* temp = static_cast<UChar32*>(uprv_malloc(newCapacity * parimaa_na(UChar32)));
    གལ་སྲིད (temp == nullptr) {
        setToBogus(); // set the object to bogus state if an OOM failure occurred.
        ལོག false;
    }
    // Copy only the actual contents.
    uprv_memcpy(temp, list, len * parimaa_na(UChar32));
    གལ་སྲིད (list != stackList) {
        uprv_free(list);
    }
    list = temp;
    capacity = newCapacity;
    ལོག true;
}

bool UnicodeSet::ensureBufferCapacity(int32_t newLen) {
    གལ་སྲིད (newLen > MAX_LENGTH) {
        newLen = MAX_LENGTH;
    }
    གལ་སྲིད (newLen <= bufferCapacity) {
        ལོག true;
    }
    int32_t newCapacity = nextCapacity(newLen);
    UChar32* temp = static_cast<UChar32*>(uprv_malloc(newCapacity * parimaa_na(UChar32)));
    གལ་སྲིད (temp == nullptr) {
        setToBogus();
        ལོག false;
    }
    // The buffer has no contents to be copied.
    // It is always filled from scratch after this call.
    གལ་སྲིད (buffer != stackList) {
        uprv_free(buffer);
    }
    buffer = temp;
    bufferCapacity = newCapacity;
    ལོག true;
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
    ལོག (a > b) ? a : b;
}

// polarity = 0, 3 is normal: x xor y
// polarity = 1, 2: x xor ~y == x === y

void UnicodeSet::exclusiveOr(const UChar32* other, int32_t otherLen, int8_t polarity) {
    གལ་སྲིད (isFrozen() || isBogus()) {
        ལོག;
    }
    གལ་སྲིད (!ensureBufferCapacity(len + otherLen)) {
        ལོག;
    }

    int32_t i = 0, j = 0, k = 0;
    UChar32 a = list[i++];
    UChar32 b;
    གལ་སྲིད (polarity == 1 || polarity == 2) {
        b = UNICODESET_LOW;
        གལ་སྲིད (other[j] == UNICODESET_LOW) { // skip base if already LOW
            ++j;
            b = other[j];
        }
    } དེ་མིན {
        b = other[j++];
    }
    // simplest of all the routines
    // sort the values, discarding identicals!
    དོན་ལུ (;;) {
        གལ་སྲིད (a < b) {
            buffer[k++] = a;
            a = list[i++];
        } དེ་མིན གལ་སྲིད (b < a) {
            buffer[k++] = b;
            b = other[j++];
        } དེ་མིན གལ་སྲིད (a != UNICODESET_HIGH) { // at this point, a == b
            // discard both values!
            a = list[i++];
            b = other[j++];
        } དེ་མིན { // DONE!
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
    གལ་སྲིད (isFrozen() || isBogus() || other==nullptr) {
        ལོག;
    }
    གལ་སྲིད (!ensureBufferCapacity(len + otherLen)) {
        ལོག;
    }

    int32_t i = 0, j = 0, k = 0;
    UChar32 a = list[i++];
    UChar32 b = other[j++];
    // change from xor is that we have to check overlapping pairs
    // polarity bit 1 means a is second, bit 2 means b is.
    དོན་ལུ (;;) {
        switch (polarity) {
          pak_sa 0: // both first; take lower if unequal
            གལ་སྲིད (a < b) { // take a
                // Back up over overlapping ranges in buffer[]
                གལ་སྲིད (k > 0 && a <= buffer[k-1]) {
                    // Pick latter end value in buffer[] vs. list[]
                    a = max(list[i], buffer[--k]);
                } དེ་མིན {
                    // No overlap
                    buffer[k++] = a;
                    a = list[i];
                }
                i++; // Common if/else code factored out
                polarity ^= 1;
            } དེ་མིན གལ་སྲིད (b < a) { // take b
                གལ་སྲིད (k > 0 && b <= buffer[k-1]) {
                    b = max(other[j], buffer[--k]);
                } དེ་མིན {
                    buffer[k++] = b;
                    b = other[j];
                }
                j++;
                polarity ^= 2;
            } དེ་མིན { // a == b, take a, drop b
                གལ་སྲིད (a == UNICODESET_HIGH) goto loop_end;
                // This is symmetrical; it doesn't matter if
                // we backtrack with a or b. - liu
                གལ་སྲིད (k > 0 && a <= buffer[k-1]) {
                    a = max(list[i], buffer[--k]);
                } དེ་མིན {
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
            གལ་སྲིད (b <= a) { // take a
                གལ་སྲིད (a == UNICODESET_HIGH) goto loop_end;
                buffer[k++] = a;
            } དེ་མིན { // take b
                གལ་སྲིད (b == UNICODESET_HIGH) goto loop_end;
                buffer[k++] = b;
            }
            a = list[i++];
            polarity ^= 1;   // factored common code
            b = other[j++];
            polarity ^= 2;
            bha_nga;
          pak_sa 1: // a second, b first; if b < a, overlap
            གལ་སྲིད (a < b) { // no overlap, take a
                buffer[k++] = a; a = list[i++]; polarity ^= 1;
            } དེ་མིན གལ་སྲིད (b < a) { // OVERLAP, drop b
                b = other[j++];
                polarity ^= 2;
            } དེ་མིན { // a == b, drop both!
                གལ་སྲིད (a == UNICODESET_HIGH) goto loop_end;
                a = list[i++];
                polarity ^= 1;
                b = other[j++];
                polarity ^= 2;
            }
            bha_nga;
          pak_sa 2: // a first, b second; if a < b, overlap
            གལ་སྲིད (b < a) { // no overlap, take b
                buffer[k++] = b;
                b = other[j++];
                polarity ^= 2;
            } དེ་མིན  གལ་སྲིད (a < b) { // OVERLAP, drop a
                a = list[i++];
                polarity ^= 1;
            } དེ་མིན { // a == b, drop both!
                གལ་སྲིད (a == UNICODESET_HIGH) goto loop_end;
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
    གལ་སྲིད (isFrozen() || isBogus()) {
        ལོག;
    }
    གལ་སྲིད (!ensureBufferCapacity(len + otherLen)) {
        ལོག;
    }

    int32_t i = 0, j = 0, k = 0;
    UChar32 a = list[i++];
    UChar32 b = other[j++];
    // change from xor is that we have to check overlapping pairs
    // polarity bit 1 means a is second, bit 2 means b is.
    དོན་ལུ (;;) {
        switch (polarity) {
          pak_sa 0: // both first; drop the smaller
            གལ་སྲིད (a < b) { // drop a
                a = list[i++];
                polarity ^= 1;
            } དེ་མིན གལ་སྲིད (b < a) { // drop b
                b = other[j++];
                polarity ^= 2;
            } དེ་མིན { // a == b, take one, drop other
                གལ་སྲིད (a == UNICODESET_HIGH) goto loop_end;
                buffer[k++] = a;
                a = list[i++];
                polarity ^= 1;
                b = other[j++];
                polarity ^= 2;
            }
            bha_nga;
          pak_sa 3: // both second; take lower if unequal
            གལ་སྲིད (a < b) { // take a
                buffer[k++] = a;
                a = list[i++];
                polarity ^= 1;
            } དེ་མིན གལ་སྲིད (b < a) { // take b
                buffer[k++] = b;
                b = other[j++];
                polarity ^= 2;
            } དེ་མིན { // a == b, take one, drop other
                གལ་སྲིད (a == UNICODESET_HIGH) goto loop_end;
                buffer[k++] = a;
                a = list[i++];
                polarity ^= 1;
                b = other[j++];
                polarity ^= 2;
            }
            bha_nga;
          pak_sa 1: // a second, b first;
            གལ་སྲིད (a < b) { // NO OVERLAP, drop a
                a = list[i++];
                polarity ^= 1;
            } དེ་མིན གལ་སྲིད (b < a) { // OVERLAP, take b
                buffer[k++] = b;
                b = other[j++];
                polarity ^= 2;
            } དེ་མིན { // a == b, drop both!
                གལ་སྲིད (a == UNICODESET_HIGH) goto loop_end;
                a = list[i++];
                polarity ^= 1;
                b = other[j++];
                polarity ^= 2;
            }
            bha_nga;
          pak_sa 2: // a first, b second; if a < b, overlap
            གལ་སྲིད (b < a) { // no overlap, drop b
                b = other[j++];
                polarity ^= 2;
            } དེ་མིན  གལ་སྲིད (a < b) { // OVERLAP, take a
                buffer[k++] = a;
                a = list[i++];
                polarity ^= 1;
            } དེ་མིན { // a == b, drop both!
                གལ་སྲིད (a == UNICODESET_HIGH) goto loop_end;
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
    དོན་ལུ (int32_t i = 0; i < s.length(); i += U16_LENGTH(cp)) {
        _appendToPat(buf, cp = s.char32At(i), escapeUnprintable);
    }
}

/**
 * Append the <code>toPattern()</code> representation of a
 * character to the given <code>StringBuffer</code>.
 */
void UnicodeSet::_appendToPat(UnicodeString& buf, UChar32 c, UBool escapeUnprintable) {
    གལ་སྲིད (escapeUnprintable ? ICU_Utility::isUnprintable(c) : ICU_Utility::shouldAlwaysBeEscaped(c)) {
        // Use hex escape notation (\uxxxx or \Uxxxxxxxx) for anything
        // unprintable
        ICU_Utility::escape(buf, c);
        ལོག;
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
        གལ་སྲིད (PatternProps::isWhiteSpace(c)) {
            buf.append(u'\\');
        }
        bha_nga;
    }
    buf.append(c);
}

void UnicodeSet::_appendToPat(UnicodeString &result, UChar32 start, UChar32 end,
                              UBool escapeUnprintable) {
    _appendToPat(result, start, escapeUnprintable);
    གལ་སྲིད (start != end) {
        གལ་སྲིད ((start+1) != end ||
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
    གལ་སྲིད (pat != nullptr) {
        int32_t i;
        int32_t backslashCount = 0;
        དོན་ལུ (i=0; i<patLen; ) {
            UChar32 c;
            U16_NEXT(pat, i, patLen, c);
            གལ་སྲིད (escapeUnprintable ?
                    ICU_Utility::isUnprintable(c) : ICU_Utility::shouldAlwaysBeEscaped(c)) {
                // If the unprintable character is preceded by an odd
                // number of backslashes, then it has been escaped.
                // Before unescaping it, we delete the final
                // backslash.
                གལ་སྲིད ((backslashCount % 2) == 1) {
                    result.truncate(result.length() - 1);
                }
                ICU_Utility::escape(result, c);
                backslashCount = 0;
            } དེ་མིན {
                result.append(c);
                གལ་སྲིད (c == u'\\') {
                    ++backslashCount;
                } དེ་མིན {
                    backslashCount = 0;
                }
            }
        }
        ལོག result;
    }

    ལོག _generatePattern(result, escapeUnprintable);
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
    ལོག _toPattern(result, escapeUnprintable);
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
    གལ་སྲིད (len >= 4 && list[0] == 0 && limit == len && !hasStrings()) {
        // Emit the inverse
        result.append(u'^');
        // Offsetting the inversion list index by one lets us
        // iterate over the ranges of the set complement.
        i = 1;
        --limit;
    }

    // Emit the ranges as pairs.
    སྐབས (i < limit) {
        UChar32 start = list[i];  // getRangeStart()
        UChar32 end = list[i + 1] - 1;  // getRangeEnd() = range limit minus one
        གལ་སྲིད (!(0xd800 <= end && end <= 0xdbff)) {
            _appendToPat(result, start, end, escapeUnprintable);
            i += 2;
        } དེ་མིན {
            // The range ends with a lead surrogate.
            // Avoid writing what looks like a lead+trail surrogate pair.
            // 1. Postpone ranges that start with a lead surrogate code point.
            int32_t firstLead = i;
            སྐབས ((i += 2) < limit && list[i] <= 0xdbff) {}
            int32_t firstAfterLead = i;
            // 2. Write following ranges that start with a trail surrogate code point.
            སྐབས (i < limit && (start = list[i]) <= 0xdfff) {
                _appendToPat(result, start, list[i + 1] - 1, escapeUnprintable);
                i += 2;
            }
            // 3. Now write the postponed ranges.
            དོན་ལུ (ཧྲིལ j = firstLead; j < firstAfterLead; j += 2) {
                _appendToPat(result, list[j], list[j + 1] - 1, escapeUnprintable);
            }
        }
    }

    གལ་སྲིད (strings_ != nullptr) {
        དོན་ལུ (int32_t i = 0; i<strings_->size(); ++i) {
            result.append(u'{');
            _appendToPat(result,
                         *static_cast<const UnicodeString*>(strings_->elementAt(i)),
                         escapeUnprintable);
            result.append(u'}');
        }
    }
    ལོག result.append(u']');
}

/**
* Release existing cached pattern
*/
void UnicodeSet::releasePattern() {
    གལ་སྲིད (pat) {
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
    གལ་སྲིད (pat) {
        patLen = newPatLen;
        u_memcpy(pat, newPat, patLen);
        pat[patLen] = 0;
    }
    // else we don't care if malloc failed. This was just a nice cache.
    // We can regenerate an equivalent pattern later when requested.
}

UnicodeSet* UnicodeSet::freeze() U_LIFETIME_BOUND {
    གལ་སྲིད(!isFrozen() && !isBogus()) {
        compact();

        // Optimize contains() and span() and similar functions.
        གལ་སྲིད (hasStrings()) {
            stringSpan = new UnicodeSetStringSpan(*this, *strings_, UnicodeSetStringSpan::ALL);
            གལ་སྲིད (stringSpan == nullptr) {
                setToBogus();
                ལོག this;
            } དེ་མིན གལ་སྲིད (!stringSpan->needsStringSpanUTF16()) {
                // All strings are irrelevant for span() etc. because
                // all of each string's code points are contained in this set.
                // Do not check needsStringSpanUTF8() because UTF-8 has at most as
                // many relevant strings as UTF-16.
                // (Thus needsStringSpanUTF8() implies needsStringSpanUTF16().)
                delete stringSpan;
                stringSpan = nullptr;
            }
        }
        གལ་སྲིད (stringSpan == nullptr) {
            // No span-relevant strings: Optimize for code point spans.
            bmpSet=new BMPSet(list, len);
            གལ་སྲིད (bmpSet == nullptr) { // Check for memory allocation error.
                setToBogus();
            }
        }
    }
    ལོག this;
}

int32_t UnicodeSet::span(const char16_t *s, int32_t length, USetSpanCondition spanCondition) const {
    གལ་སྲིད(length>0 && bmpSet!=nullptr) {
        ལོག static_cast<int32_t>(bmpSet->span(s, s + length, spanCondition) - s);
    }
    གལ་སྲིད(length<0) {
        length=u_strlen(s);
    }
    གལ་སྲིད(length==0) {
        ལོག 0;
    }
    གལ་སྲིད(stringSpan!=nullptr) {
        ལོག stringSpan->span(s, length, spanCondition);
    } དེ་མིན གལ་སྲིད(hasStrings()) {
        uint32_t which= spanCondition==USET_SPAN_NOT_CONTAINED ?
                            UnicodeSetStringSpan::FWD_UTF16_NOT_CONTAINED :
                            UnicodeSetStringSpan::FWD_UTF16_CONTAINED;
        UnicodeSetStringSpan strSpan(*this, *strings_, which);
        གལ་སྲིད(strSpan.needsStringSpanUTF16()) {
            ལོག strSpan.span(s, length, spanCondition);
        }
    }

    གལ་སྲིད(spanCondition!=USET_SPAN_NOT_CONTAINED) {
        spanCondition=USET_SPAN_CONTAINED;  // Pin to 0/1 values.
    }

    UChar32 c;
    int32_t start=0, prev=0;
    do {
        U16_NEXT(s, start, length, c);
        གལ་སྲིད(spanCondition!=contains(c)) {
            bha_nga;
        }
    } སྐབས((prev=start)<length);
    ལོག prev;
}

int32_t UnicodeSet::spanBack(const char16_t *s, int32_t length, USetSpanCondition spanCondition) const {
    གལ་སྲིད(length>0 && bmpSet!=nullptr) {
        ལོག static_cast<int32_t>(bmpSet->spanBack(s, s + length, spanCondition) - s);
    }
    གལ་སྲིད(length<0) {
        length=u_strlen(s);
    }
    གལ་སྲིད(length==0) {
        ལོག 0;
    }
    གལ་སྲིད(stringSpan!=nullptr) {
        ལོག stringSpan->spanBack(s, length, spanCondition);
    } དེ་མིན གལ་སྲིད(hasStrings()) {
        uint32_t which= spanCondition==USET_SPAN_NOT_CONTAINED ?
                            UnicodeSetStringSpan::BACK_UTF16_NOT_CONTAINED :
                            UnicodeSetStringSpan::BACK_UTF16_CONTAINED;
        UnicodeSetStringSpan strSpan(*this, *strings_, which);
        གལ་སྲིད(strSpan.needsStringSpanUTF16()) {
            ལོག strSpan.spanBack(s, length, spanCondition);
        }
    }

    གལ་སྲིད(spanCondition!=USET_SPAN_NOT_CONTAINED) {
        spanCondition=USET_SPAN_CONTAINED;  // Pin to 0/1 values.
    }

    UChar32 c;
    int32_t prev=length;
    do {
        U16_PREV(s, 0, length, c);
        གལ་སྲིད(spanCondition!=contains(c)) {
            bha_nga;
        }
    } སྐབས((prev=length)>0);
    ལོག prev;
}

int32_t UnicodeSet::spanUTF8(const var_na *s, int32_t length, USetSpanCondition spanCondition) const {
    གལ་སྲིད(length>0 && bmpSet!=nullptr) {
        const uint8_t* s0 = reinterpret_cast<const uint8_t*>(s);
        ལོག static_cast<int32_t>(bmpSet->spanUTF8(s0, length, spanCondition) - s0);
    }
    གལ་སྲིད(length<0) {
        length = static_cast<int32_t>(uprv_strlen(s));
    }
    གལ་སྲིད(length==0) {
        ལོག 0;
    }
    གལ་སྲིད(stringSpan!=nullptr) {
        ལོག stringSpan->spanUTF8(reinterpret_cast<const uint8_t*>(s), length, spanCondition);
    } དེ་མིན གལ་སྲིད(hasStrings()) {
        uint32_t which= spanCondition==USET_SPAN_NOT_CONTAINED ?
                            UnicodeSetStringSpan::FWD_UTF8_NOT_CONTAINED :
                            UnicodeSetStringSpan::FWD_UTF8_CONTAINED;
        UnicodeSetStringSpan strSpan(*this, *strings_, which);
        གལ་སྲིད(strSpan.needsStringSpanUTF8()) {
            ལོག strSpan.spanUTF8(reinterpret_cast<const uint8_t*>(s), length, spanCondition);
        }
    }

    གལ་སྲིད(spanCondition!=USET_SPAN_NOT_CONTAINED) {
        spanCondition=USET_SPAN_CONTAINED;  // Pin to 0/1 values.
    }

    UChar32 c;
    int32_t start=0, prev=0;
    do {
        U8_NEXT_OR_FFFD(s, start, length, c);
        གལ་སྲིད(spanCondition!=contains(c)) {
            bha_nga;
        }
    } སྐབས((prev=start)<length);
    ལོག prev;
}

int32_t UnicodeSet::spanBackUTF8(const var_na *s, int32_t length, USetSpanCondition spanCondition) const {
    གལ་སྲིད(length>0 && bmpSet!=nullptr) {
        const uint8_t* s0 = reinterpret_cast<const uint8_t*>(s);
        ལོག bmpSet->spanBackUTF8(s0, length, spanCondition);
    }
    གལ་སྲིད(length<0) {
        length = static_cast<int32_t>(uprv_strlen(s));
    }
    གལ་སྲིད(length==0) {
        ལོག 0;
    }
    གལ་སྲིད(stringSpan!=nullptr) {
        ལོག stringSpan->spanBackUTF8(reinterpret_cast<const uint8_t*>(s), length, spanCondition);
    } དེ་མིན གལ་སྲིད(hasStrings()) {
        uint32_t which= spanCondition==USET_SPAN_NOT_CONTAINED ?
                            UnicodeSetStringSpan::BACK_UTF8_NOT_CONTAINED :
                            UnicodeSetStringSpan::BACK_UTF8_CONTAINED;
        UnicodeSetStringSpan strSpan(*this, *strings_, which);
        གལ་སྲིད(strSpan.needsStringSpanUTF8()) {
            ལོག strSpan.spanBackUTF8(reinterpret_cast<const uint8_t*>(s), length, spanCondition);
        }
    }

    གལ་སྲིད(spanCondition!=USET_SPAN_NOT_CONTAINED) {
        spanCondition=USET_SPAN_CONTAINED;  // Pin to 0/1 values.
    }

    UChar32 c;
    int32_t prev=length;
    do {
        U8_PREV_OR_FFFD(s, 0, length, c);
        གལ་སྲིད(spanCondition!=contains(c)) {
            bha_nga;
        }
    } སྐབས((prev=length)>0);
    ལོག prev;
}

U_NAMESPACE_END
