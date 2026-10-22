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

#ಅನ್ತರ್ಭಾವ "unicode/utypes.h"
#ಅನ್ತರ್ಭಾವ "unicode/parsepos.h"
#ಅನ್ತರ್ಭಾವ "unicode/symtable.h"
#ಅನ್ತರ್ಭಾವ "unicode/uniset.h"
#ಅನ್ತರ್ಭಾವ "unicode/ustring.h"
#ಅನ್ತರ್ಭಾವ "unicode/utf8.h"
#ಅನ್ತರ್ಭಾವ "unicode/utf16.h"
#ಅನ್ತರ್ಭಾವ "ruleiter.h"
#ಅನ್ತರ್ಭಾವ "cmemory.h"
#ಅನ್ತರ್ಭಾವ "cstring.h"
#ಅನ್ತರ್ಭಾವ "patternprops.h"
#ಅನ್ತರ್ಭಾವ "uelement.h"
#ಅನ್ತರ್ಭಾವ "util.h"
#ಅನ್ತರ್ಭಾವ "uvector.h"
#ಅನ್ತರ್ಭಾವ "charstr.h"
#ಅನ್ತರ್ಭಾವ "ustrfmt.h"
#ಅನ್ತರ್ಭಾವ "uassert.h"
#ಅನ್ತರ್ಭಾವ "bmpset.h"
#ಅನ್ತರ್ಭಾವ "unisetspan.h"

// HIGH_VALUE > all valid values. 110000 for codepoints
#define UNICODESET_ಮೇಲಿನ 0x0110000

// LOW <= all valid values. ZERO for codepoints
#define UNICODESET_ಕೆಳಗಿನ 0x000000

/** Max list [0, 1, 2, ..., max code point, HIGH] */
constexpr int32_t MAX_LENGTH = UNICODESET_ಮೇಲಿನ + 1;

U_NAMESPACE_BEGIN

ಚಿಹ್ನೆ_Table::~ಚಿಹ್ನೆ_Table() {}

UOBJECT_DEFINE_RTTI_IMPLEMENTATION(UnicodeSet)

/**
 * Modify the given UChar32 variable so that it is in range, by
 * pinning values < UNICODESET_LOW to UNICODESET_LOW, and
 * pinning values > UNICODESET_HIGH-1 to UNICODESET_HIGH-1.
 * It modifies its argument in-place and also returns it.
 */
ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ UChar32 pinCodePoint(UChar32& c) {
    ಯದಿ (c < UNICODESET_ಕೆಳಗಿನ) {
        c = UNICODESET_ಕೆಳಗಿನ;
    } ಅನ್ಯಥಾ ಯದಿ (c > (UNICODESET_ಮೇಲಿನ-1)) {
        c = (UNICODESET_ಮೇಲಿನ-1);
    }
    ಪ್ರತ್ಯಾಗಮ c;
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
#ಅನ್ತರ್ಭಾವ <stdio.h>
ಸ್ಥಿರ int32_t dbg_ಎಣಿಕೆ = 0;

ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ ಶೂನ್ಯ _dbgct(UnicodeSet* set) {
    UnicodeString str;
    set->toPattern(str, ಸತ್ಯ);
    ವರ್ಣ buf[40];
    str.extract(0, 39, buf, "");
    printf("DEBUG UnicodeSet: ct 0x%08X; %d %s\n", set, ++dbg_ಎಣಿಕೆ, buf);
}

ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ ಶೂನ್ಯ _dbgdt(UnicodeSet* set) {
    UnicodeString str;
    set->toPattern(str, ಸತ್ಯ);
    ವರ್ಣ buf[40];
    str.extract(0, 39, buf, "");
    printf("DEBUG UnicodeSet: dt 0x%08X; %d %s\n", set, --dbg_ಎಣಿಕೆ, buf);
}

#ಅನ್ಯಥಾ

#define _dbgct(set)
#define _dbgdt(set)

#endif

//----------------------------------------------------------------
// UnicodeString in UVector support
//----------------------------------------------------------------

ಸ್ಥಿರ ಶೂನ್ಯ U_CALLCONV cloneUnicodeString(UElement *dst, UElement *src) {
    dst->pointer = ನೂತನ UnicodeString(*static_cast<UnicodeString*>(src->pointer));
}

ಸ್ಥಿರ int32_t U_CALLCONV compareUnicodeString(UElement t1, UElement t2) {
    ಧ್ರುವ UnicodeString& a = *static_cast<ಧ್ರುವ UnicodeString*>(t1.pointer);
    ಧ್ರುವ UnicodeString& b = *static_cast<ಧ್ರುವ UnicodeString*>(t2.pointer);
    ಪ್ರತ್ಯಾಗಮ a.compare(b);
}

UBool UnicodeSet::hasStrings() ಧ್ರುವ {
    ಪ್ರತ್ಯಾಗಮ strings_ != nullptr && !strings_->isEmpty();
}

int32_t UnicodeSet::stringsSize() ಧ್ರುವ {
    ಪ್ರತ್ಯಾಗಮ strings_ == nullptr ? 0 : strings_->size();
}

UBool UnicodeSet::stringsContains(ಧ್ರುವ UnicodeString &s) ಧ್ರುವ {
    ಪ್ರತ್ಯಾಗಮ strings_ != nullptr && strings_->contains((ಶೂನ್ಯ*) &s);
}

//----------------------------------------------------------------
// Constructors &c
//----------------------------------------------------------------

/**
 * Constructs an empty set.
 */
UnicodeSet::UnicodeSet() {
    list[0] = UNICODESET_ಮೇಲಿನ;
    _dbgct(ಏತತ್);
}

/**
 * Constructs a set containing the given range. If <code>end >
 * start</code> then an empty set is created.
 *
 * @param start first character, inclusive, of range
 * @param end last character, inclusive, of range
 */
UnicodeSet::UnicodeSet(UChar32 start, UChar32 end) {
    list[0] = UNICODESET_ಮೇಲಿನ;
    add(start, end);
    _dbgct(ಏತತ್);
}

/**
 * Constructs a set that is identical to the given UnicodeSet.
 */
UnicodeSet::UnicodeSet(ಧ್ರುವ UnicodeSet& o) : UnicodeFilter(o) {
    *ಏತತ್ = o;
    _dbgct(ಏತತ್);
}

// Copy-construct as thawed.
UnicodeSet::UnicodeSet(ಧ್ರುವ UnicodeSet& o, UBool /* asThawed */) : UnicodeFilter(o) {
    ಯದಿ (ensureCapacity(o.len)) {
        // *this = o except for bmpSet and stringSpan
        len = o.len;
        uprv_memcpy(list, o.list, (size_t)len*ಪರಿಮಾಣ(UChar32));
        ಯದಿ (o.hasStrings()) {
            UErrorCode status = U_ZERO_ERROR;
            ಯದಿ (!allocateStrings(status) ||
                    (strings_->assign(*o.strings_, cloneUnicodeString, status), U_FAILURE(status))) {
                setToBogus();
                ಪ್ರತ್ಯಾಗಮ;
            }
        }
        ಯದಿ (o.pat) {
            setPattern(o.pat, o.patLen);
        }
        _dbgct(ಏತತ್);
    }
}

/**
 * Destructs the set.
 */
UnicodeSet::~UnicodeSet() {
    _dbgdt(ಏತತ್); // first!
    ಯದಿ (list != ರಾಶಿ_List) {
        uprv_free(list);
    }
    delete bmpSet;
    ಯದಿ (buffer != ರಾಶಿ_List) {
        uprv_free(buffer);
    }
    delete strings_;
    delete stringSpan;
    releasePattern();
}

/**
 * Assigns this object to be a copy of another.
 */
UnicodeSet& UnicodeSet::operator=(ಧ್ರುವ UnicodeSet& o) U_LIFETIME_BOUND {
    ಪ್ರತ್ಯಾಗಮ copyFrom(o, ಅಸತ್ಯ);
}

UnicodeSet& UnicodeSet::copyFrom(ಧ್ರುವ UnicodeSet& o, UBool asThawed) U_LIFETIME_BOUND {
    ಯದಿ (ಏತತ್ == &o) {
        ಪ್ರತ್ಯಾಗಮ *ಏತತ್;
    }
    ಯದಿ (isFrozen()) {
        ಪ್ರತ್ಯಾಗಮ *ಏತತ್;
    }
    ಯದಿ (o.isBogus()) {
        setToBogus();
        ಪ್ರತ್ಯಾಗಮ *ಏತತ್;
    }
    ಯದಿ (!ensureCapacity(o.len)) {
        // ensureCapacity will mark the UnicodeSet as Bogus if OOM failure happens.
        ಪ್ರತ್ಯಾಗಮ *ಏತತ್;
    }
    len = o.len;
    uprv_memcpy(list, o.list, (size_t)len*ಪರಿಮಾಣ(UChar32));
    ಯದಿ (o.bmpSet != nullptr && !asThawed) {
        bmpSet = ನೂತನ BMPSet(*o.bmpSet, list, len);
        ಯದಿ (bmpSet == nullptr) { // Check for memory allocation error.
            setToBogus();
            ಪ್ರತ್ಯಾಗಮ *ಏತತ್;
        }
    }
    ಯದಿ (o.hasStrings()) {
        UErrorCode status = U_ZERO_ERROR;
        ಯದಿ ((strings_ == nullptr && !allocateStrings(status)) ||
                (strings_->assign(*o.strings_, cloneUnicodeString, status), U_FAILURE(status))) {
            setToBogus();
            ಪ್ರತ್ಯಾಗಮ *ಏತತ್;
        }
    } ಅನ್ಯಥಾ ಯದಿ (hasStrings()) {
        strings_->removeAllElements();
    }
    ಯದಿ (o.stringSpan != nullptr && strings_ != nullptr && !asThawed) {
        stringSpan = ನೂತನ UnicodeSetStringSpan(*o.stringSpan, *strings_);
        ಯದಿ (stringSpan == nullptr) { // Check for memory allocation error.
            setToBogus();
            ಪ್ರತ್ಯಾಗಮ *ಏತತ್;
        }
    }
    releasePattern();
    ಯದಿ (o.pat) {
        setPattern(o.pat, o.patLen);
    }
    ಪ್ರತ್ಯಾಗಮ *ಏತತ್;
}

/**
 * Returns a copy of this object.  All UnicodeMatcher objects have
 * to support cloning in order to allow classes using
 * UnicodeMatchers, such as Transliterator, to implement cloning.
 */
UnicodeSet* UnicodeSet::clone() ಧ್ರುವ {
    ಪ್ರತ್ಯಾಗಮ ನೂತನ UnicodeSet(*ಏತತ್);
}

UnicodeSet *UnicodeSet::cloneAsThawed() ಧ್ರುವ {
    ಪ್ರತ್ಯಾಗಮ ನೂತನ UnicodeSet(*ಏತತ್, ಸತ್ಯ);
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
ತಾರ್ಕಿಕ UnicodeSet::operator==(ಧ್ರುವ UnicodeSet& o) ಧ್ರುವ {
    ಯದಿ (len != o.len) ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;
    ಪ್ರತಿ (int32_t i = 0; i < len; ++i) {
        ಯದಿ (list[i] != o.list[i]) ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;
    }
    ಯದಿ (hasStrings() != o.hasStrings()) { ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ; }
    ಯದಿ (hasStrings() && *strings_ != *o.strings_) ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;
    ಪ್ರತ್ಯಾಗಮ ಸತ್ಯ;
}

/**
 * Returns the hash code value for this set.
 *
 * @return the hash code value for this set.
 * @see Object#hashCode()
 */
int32_t UnicodeSet::hashCode() ಧ್ರುವ {
    uint32_t result = static_cast<uint32_t>(len);
    ಪ್ರತಿ (int32_t i = 0; i < len; ++i) {
        result *= 1000003u;
        result += list[i];
    }
    ಪ್ರತ್ಯಾಗಮ static_cast<int32_t>(result);
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
int32_t UnicodeSet::size() ಧ್ರುವ {
    int32_t n = 0;
    int32_t count = get_Range_ಎಣಿಕೆ();
    ಪ್ರತಿ (int32_t i = 0; i < count; ++i) {
        n += getRangeEnd(i) - getRangeStart(i) + 1;
    }
    ಪ್ರತ್ಯಾಗಮ n + stringsSize();
}

/**
 * Returns <tt>true</tt> if this set contains no elements.
 *
 * @return <tt>true</tt> if this set contains no elements.
 */
UBool UnicodeSet::isEmpty() ಧ್ರುವ {
    ಪ್ರತ್ಯಾಗಮ len == 1 && !hasStrings();
}

/**
 * Returns true if this set contains the given character.
 * @param c character to be checked for containment
 * @return true if the test condition is met
 */
UBool UnicodeSet::contains(UChar32 c) ಧ್ರುವ {
    // Set i to the index of the start item greater than ch
    // We know we will terminate without length test!
    // LATER: for large sets, add binary search
    //int32_t i = -1;
    //for (;;) {
    //    if (c < list[++i]) break;
    //}
    ಯದಿ (bmpSet != nullptr) {
        ಪ್ರತ್ಯಾಗಮ bmpSet->contains(c);
    }
    ಯದಿ (stringSpan != nullptr) {
        ಪ್ರತ್ಯಾಗಮ stringSpan->contains(c);
    }
    ಯದಿ (c >= UNICODESET_ಮೇಲಿನ) { // Don't need to check LOW bound
        ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;
    }
    int32_t i = findCodePoint(c);
    ಪ್ರತ್ಯಾಗಮ i & 1; // return true if odd
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
int32_t UnicodeSet::findCodePoint(UChar32 c) ಧ್ರುವ {
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
    ಯದಿ (c < list[0])
        ಪ್ರತ್ಯಾಗಮ 0;
    // High runner test.  c is often after the last range, so an
    // initial check for this condition pays off.
    int32_t lo = 0;
    int32_t hi = len - 1;
    ಯದಿ (lo >= hi || c >= list[hi-1])
        ಪ್ರತ್ಯಾಗಮ hi;
    // invariant: c >= list[lo]
    // invariant: c < list[hi]
    ಪ್ರತಿ (;;) {
        int32_t i = (lo + hi) >> 1;
        ಯದಿ (i == lo) {
            ಭಙ್ಗ; // Found!
        } ಅನ್ಯಥಾ ಯದಿ (c < list[i]) {
            hi = i;
        } ಅನ್ಯಥಾ {
            lo = i;
        }
    }
    ಪ್ರತ್ಯಾಗಮ hi;
}

/**
 * Returns true if this set contains every character
 * of the given range.
 * @param start first character, inclusive, of the range
 * @param end last character, inclusive, of the range
 * @return true if the test condition is met
 */
UBool UnicodeSet::contains(UChar32 start, UChar32 end) ಧ್ರುವ {
    //int32_t i = -1;
    //for (;;) {
    //    if (start < list[++i]) break;
    //}
    int32_t i = findCodePoint(start);
    ಪ್ರತ್ಯಾಗಮ ((i & 1) != 0 && end < list[i]);
}

/**
 * Returns <tt>true</tt> if this set contains the given
 * multicharacter string.
 * @param s string to be checked for containment
 * @return <tt>true</tt> if this set contains the specified string
 */
UBool UnicodeSet::contains(ಧ್ರುವ UnicodeString& s) ಧ್ರುವ {
    int32_t cp = getSingleCP(s);
    ಯದಿ (cp < 0) {
        ಪ್ರತ್ಯಾಗಮ stringsContains(s);
    } ಅನ್ಯಥಾ {
        ಪ್ರತ್ಯಾಗಮ contains(static_cast<UChar32>(cp));
    }
}

/**
 * Returns true if this set contains all the characters and strings
 * of the given set.
 * @param c set to be checked for containment
 * @return true if the test condition is met
 */
UBool UnicodeSet::containsAll(ಧ್ರುವ UnicodeSet& c) ಧ್ರುವ {
    // The specified set is a subset if all of its pairs are contained in
    // this set.  It's possible to code this more efficiently in terms of
    // direct manipulation of the inversion lists if the need arises.
    int32_t n = c.get_Range_ಎಣಿಕೆ();
    ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ i=0; i<n; ++i) {
        ಯದಿ (!contains(c.getRangeStart(i), c.getRangeEnd(i))) {
            ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;
        }
    }
    ಪ್ರತ್ಯಾಗಮ !c.hasStrings() || (strings_ != nullptr && strings_->containsAll(*c.strings_));
}

/**
 * Returns true if this set contains all the characters
 * of the given string.
 * @param s string containing characters to be checked for containment
 * @return true if the test condition is met
 */
UBool UnicodeSet::containsAll(ಧ್ರುವ UnicodeString& s) ಧ್ರುವ {
    ಪ್ರತ್ಯಾಗಮ span(s.getBuffer(), s.length(), USET_SPAN_CONTAINED) == s.length();
}

/**
 * Returns true if this set contains none of the characters
 * of the given range.
 * @param start first character, inclusive, of the range
 * @param end last character, inclusive, of the range
 * @return true if the test condition is met
 */
UBool UnicodeSet::containsNone(UChar32 start, UChar32 end) ಧ್ರುವ {
    //int32_t i = -1;
    //for (;;) {
    //    if (start < list[++i]) break;
    //}
    int32_t i = findCodePoint(start);
    ಪ್ರತ್ಯಾಗಮ ((i & 1) == 0 && end < list[i]);
}

/**
 * Returns true if this set contains none of the characters and strings
 * of the given set.
 * @param c set to be checked for containment
 * @return true if the test condition is met
 */
UBool UnicodeSet::containsNone(ಧ್ರುವ UnicodeSet& c) ಧ್ರುವ {
    // The specified set is a subset if all of its pairs are contained in
    // this set.  It's possible to code this more efficiently in terms of
    // direct manipulation of the inversion lists if the need arises.
    int32_t n = c.get_Range_ಎಣಿಕೆ();
    ಪ್ರತಿ (int32_t i=0; i<n; ++i) {
        ಯದಿ (!containsNone(c.getRangeStart(i), c.getRangeEnd(i))) {
            ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;
        }
    }
    ಪ್ರತ್ಯಾಗಮ strings_ == nullptr || !c.hasStrings() || strings_->containsNone(*c.strings_);
}

/**
 * Returns true if this set contains none of the characters
 * of the given string.
 * @param s string containing characters to be checked for containment
 * @return true if the test condition is met
 */
UBool UnicodeSet::containsNone(ಧ್ರುವ UnicodeString& s) ಧ್ರುವ {
    ಪ್ರತ್ಯಾಗಮ span(s.getBuffer(), s.length(), USET_SPAN_NOT_CONTAINED) == s.length();
}

/**
 * Returns <tt>true</tt> if this set contains any character whose low byte
 * is the given value.  This is used by <tt>RuleBasedTransliterator</tt> for
 * indexing.
 */
UBool UnicodeSet::matches_Index_ಮೌಲ್ಯ(uint8_t v) ಧ್ರುವ {
    /* The index value v, in the range [0,255], is contained in this set if
     * it is contained in any pair of this set.  Pairs either have the high
     * bytes equal, or unequal.  If the high bytes are equal, then we have
     * aaxx..aayy, where aa is the high byte.  Then v is contained if xx <=
     * v <= yy.  If the high bytes are unequal we have aaxx..bbyy, bb>aa.
     * Then v is contained if xx <= v || v <= yy.  (This is identical to the
     * time zone month containment logic.)
     */
    int32_t i;
    int32_t range_ಎಣಿಕೆ=get_Range_ಎಣಿಕೆ();
    ಪ್ರತಿ (i=0; i<range_ಎಣಿಕೆ; ++i) {
        UChar32 ಕೆಳಗಿನ = getRangeStart(i);
        UChar32 ಮೇಲಿನ = getRangeEnd(i);
        ಯದಿ ((ಕೆಳಗಿನ & ~0xFF) == (ಮೇಲಿನ & ~0xFF)) {
            ಯದಿ ((ಕೆಳಗಿನ & 0xFF) <= v && v <= (ಮೇಲಿನ & 0xFF)) {
                ಪ್ರತ್ಯಾಗಮ ಸತ್ಯ;
            }
        } ಅನ್ಯಥಾ ಯದಿ ((ಕೆಳಗಿನ & 0xFF) <= v || v <= (ಮೇಲಿನ & 0xFF)) {
            ಪ್ರತ್ಯಾಗಮ ಸತ್ಯ;
        }
    }
    ಯದಿ (hasStrings()) {
        ಪ್ರತಿ (i=0; i<strings_->size(); ++i) {
            ಧ್ರುವ UnicodeString& s = *static_cast<ಧ್ರುವ UnicodeString*>(strings_->elementAt(i));
            ಯದಿ (s.isEmpty()) {
                ಅನುವರ್ತ;  // skip the empty string
            }
            UChar32 c = s.char32At(0);
            ಯದಿ ((c & 0xFF) == v) {
                ಪ್ರತ್ಯಾಗಮ ಸತ್ಯ;
            }
        }
    }
    ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;
}

/**
 * Implementation of UnicodeMatcher::matches().  Always matches the
 * longest possible multichar string.
 */
UMatchDegree UnicodeSet::matches(ಧ್ರುವ Replaceable& text,
                                 int32_t& offset,
                                 int32_t limit,
                                 UBool incremental) {
    ಯದಿ (offset == limit) {
        ಯದಿ (contains(U_ETHER)) {
            ಪ್ರತ್ಯಾಗಮ incremental ? U_PARTIAL_MATCH : U_MATCH;
        } ಅನ್ಯಥಾ {
            ಪ್ರತ್ಯಾಗಮ U_MISMATCH;
        }
    } ಅನ್ಯಥಾ {
        ಯದಿ (hasStrings()) { // try strings first

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
            int32_t ಮೇಲಿನ_Water_Length = 0;

            ಪ್ರತಿ (i=0; i<strings_->size(); ++i) {
                ಧ್ರುವ UnicodeString& trial = *static_cast<ಧ್ರುವ UnicodeString*>(strings_->elementAt(i));
                ಯದಿ (trial.isEmpty()) {
                    ಅನುವರ್ತ;  // skip the empty string
                }

                char16_t c = trial.charAt(forward ? 0 : trial.length() - 1);

                // Strings are sorted, so we can optimize in the
                // forward direction.
                ಯದಿ (forward && c > firstChar) ಭಙ್ಗ;
                ಯದಿ (c != firstChar) ಅನುವರ್ತ;

                int32_t matchLen = matchRest(text, offset, limit, trial);

                ಯದಿ (incremental) {
                    int32_t maxLen = forward ? limit-offset : offset-limit;
                    ಯದಿ (matchLen == maxLen) {
                        // We have successfully matched but only up to limit.
                        ಪ್ರತ್ಯಾಗಮ U_PARTIAL_MATCH;
                    }
                }

                ಯದಿ (matchLen == trial.length()) {
                    // We have successfully matched the whole string.
                    ಯದಿ (matchLen > ಮೇಲಿನ_Water_Length) {
                        ಮೇಲಿನ_Water_Length = matchLen;
                    }
                    // In the forward direction we know strings
                    // are sorted so we can bail early.
                    ಯದಿ (forward && matchLen < ಮೇಲಿನ_Water_Length) {
                        ಭಙ್ಗ;
                    }
                    ಅನುವರ್ತ;
                }
            }

            // We've checked all strings without a partial match.
            // If we have full matches, return the longest one.
            ಯದಿ (ಮೇಲಿನ_Water_Length != 0) {
                offset += forward ? ಮೇಲಿನ_Water_Length : -ಮೇಲಿನ_Water_Length;
                ಪ್ರತ್ಯಾಗಮ U_MATCH;
            }
        }
        ಪ್ರತ್ಯಾಗಮ UnicodeFilter::matches(text, offset, limit, incremental);
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
int32_t UnicodeSet::matchRest(ಧ್ರುವ Replaceable& text,
                              int32_t start, int32_t limit,
                              ಧ್ರುವ UnicodeString& s) {
    int32_t i;
    int32_t maxLen;
    int32_t slen = s.length();
    ಯದಿ (start < limit) {
        maxLen = limit - start;
        ಯದಿ (maxLen > slen) maxLen = slen;
        ಪ್ರತಿ (i = 1; i < maxLen; ++i) {
            ಯದಿ (text.charAt(start + i) != s.charAt(i)) ಪ್ರತ್ಯಾಗಮ 0;
        }
    } ಅನ್ಯಥಾ {
        maxLen = start - limit;
        ಯದಿ (maxLen > slen) maxLen = slen;
        --slen; // <=> slen = s.length() - 1;
        ಪ್ರತಿ (i = 1; i < maxLen; ++i) {
            ಯದಿ (text.charAt(start - i) != s.charAt(slen - i)) ಪ್ರತ್ಯಾಗಮ 0;
        }
    }
    ಪ್ರತ್ಯಾಗಮ maxLen;
}

/**
 * Implement of UnicodeMatcher
 */
ಶೂನ್ಯ UnicodeSet::addMatchSetTo(UnicodeSet& toUnionTo) ಧ್ರುವ {
    toUnionTo.addAll(*ಏತತ್);
}

/**
 * Returns the index of the given character within this set, where
 * the set is ordered by ascending code point.  If the character
 * is not in this set, return -1.  The inverse of this method is
 * <code>charAt()</code>.
 * @return an index from 0..size()-1, or -1
 */
int32_t UnicodeSet::indexOf(UChar32 c) ಧ್ರುವ {
    ಯದಿ (c < MIN_ಮೌಲ್ಯ || c > MAX_ಮೌಲ್ಯ) {
        ಪ್ರತ್ಯಾಗಮ -1;
    }
    int32_t i = 0;
    int32_t n = 0;
    ಪ್ರತಿ (;;) {
        UChar32 start = list[i++];
        ಯದಿ (c < start) {
            ಪ್ರತ್ಯಾಗಮ -1;
        }
        UChar32 limit = list[i++];
        ಯದಿ (c < limit) {
            ಪ್ರತ್ಯಾಗಮ n + c - start;
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
UChar32 UnicodeSet::charAt(int32_t index) ಧ್ರುವ {
    ಯದಿ (index >= 0) {
        // len2 is the largest even integer <= len, that is, it is len
        // for even values and len-1 for odd values.  With odd values
        // the last entry is UNICODESET_HIGH.
        int32_t len2 = len & ~1;
        ಪ್ರತಿ (int32_t i=0; i < len2;) {
            UChar32 start = list[i++];
            int32_t count = list[i++] - start;
            ಯದಿ (index < count) {
                ಪ್ರತ್ಯಾಗಮ static_cast<UChar32>(start + index);
            }
            index -= count;
        }
    }
    ಪ್ರತ್ಯಾಗಮ static_cast<UChar32>(-1);
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
    ಪ್ರತ್ಯಾಗಮ *ಏತತ್;
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
    ಯದಿ (pinCodePoint(start) < pinCodePoint(end)) {
        UChar32 limit = end + 1;
        // Fast path for adding a new range after the last one.
        // Odd list length: [..., lastStart, lastLimit, HIGH]
        ಯದಿ ((len & 1) != 0) {
            // If the list is empty, set lastLimit low enough to not be adjacent to 0.
            UChar32 lastLimit = len == 1 ? -2 : list[len - 2];
            ಯದಿ (lastLimit <= start && !isFrozen() && !isBogus()) {
                ಯದಿ (lastLimit == start) {
                    // Extend the last range.
                    list[len - 2] = limit;
                    ಯದಿ (limit == UNICODESET_ಮೇಲಿನ) {
                        --len;
                    }
                } ಅನ್ಯಥಾ {
                    list[len - 1] = start;
                    ಯದಿ (limit < UNICODESET_ಮೇಲಿನ) {
                        ಯದಿ (ensureCapacity(len + 2)) {
                            list[len++] = limit;
                            list[len++] = UNICODESET_ಮೇಲಿನ;
                        }
                    } ಅನ್ಯಥಾ {  // limit == UNICODESET_HIGH
                        ಯದಿ (ensureCapacity(len + 1)) {
                            list[len++] = UNICODESET_ಮೇಲಿನ;
                        }
                    }
                }
                releasePattern();
                ಪ್ರತ್ಯಾಗಮ *ಏತತ್;
            }
        }
        // This is slow. Could be much faster using findCodePoint(start)
        // and modifying the list, dealing with adjacent & overlapping ranges.
        UChar32 range[3] = { start, limit, UNICODESET_ಮೇಲಿನ };
        add(range, 2, 0);
    } ಅನ್ಯಥಾ ಯದಿ (start == end) {
        add(start);
    }
    ಪ್ರತ್ಯಾಗಮ *ಏತತ್;
}

// #define DEBUG_US_ADD

#ifdef DEBUG_US_ADD
#ಅನ್ತರ್ಭಾವ <stdio.h>
ಶೂನ್ಯ dump(UChar32 c) {
    ಯದಿ (c <= 0xFF) {
        printf("%c", (ವರ್ಣ)c);
    } ಅನ್ಯಥಾ {
        printf("U+%04X", c);
    }
}
ಶೂನ್ಯ dump(ಧ್ರುವ UChar32* list, int32_t len) {
    printf("[");
    ಪ್ರತಿ (int32_t i=0; i<len; ++i) {
        ಯದಿ (i != 0) printf(", ");
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
    ಯದಿ ((i & 1) != 0  || isFrozen() || isBogus()) ಪ್ರತ್ಯಾಗಮ *ಏತತ್;

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

    ಯದಿ (c == list[i]-1) {
        // c is before start of next range
        list[i] = c;
        // if we touched the HIGH mark, then add a new one
        ಯದಿ (c == (UNICODESET_ಮೇಲಿನ - 1)) {
            ಯದಿ (!ensureCapacity(len+1)) {
                // ensureCapacity will mark the object as Bogus if OOM failure happens.
                ಪ್ರತ್ಯಾಗಮ *ಏತತ್;
            }
            list[len++] = UNICODESET_ಮೇಲಿನ;
        }
        ಯದಿ (i > 0 && c == list[i-1]) {
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
            ಯಾವತ್ (src < srclimit) *(dst++) = *(src++);

            len -= 2;
        }
    }

    ಅನ್ಯಥಾ ಯದಿ (i > 0 && c == list[i-1]) {
        // c is after end of prior range
        list[i-1]++;
        // no need to check for collapse here
    }

    ಅನ್ಯಥಾ {
        // At this point we know the new char is not adjacent to
        // any existing ranges, and it is not 10FFFF.


        // [..., start_k-1, limit_k-1, start_k, limit_k, ..., HIGH]
        //                             ^
        //                             list[i]

        // [..., start_k-1, limit_k-1, c, c+1, start_k, limit_k, ..., HIGH]
        //                             ^
        //                             list[i]

        ಯದಿ (!ensureCapacity(len+2)) {
            // ensureCapacity will mark the object as Bogus if OOM failure happens.
            ಪ್ರತ್ಯಾಗಮ *ಏತತ್;
        }

        UChar32 *p = list + i;
        uprv_memmove(p + 2, p, (len - i) * ಪರಿಮಾಣ(*p));
        list[i] = c;
        list[i+1] = c+1;
        len += 2;
    }

#ifdef DEBUG_US_ADD
    dump(list, len);
    printf("\n");

    ಪ್ರತಿ (i=1; i<len; ++i) {
        ಯದಿ (list[i] <= list[i-1]) {
            // Corrupt array!
            printf("ERROR: list has been corrupted\n");
            exit(1);
        }
    }
#endif

    releasePattern();
    ಪ್ರತ್ಯಾಗಮ *ಏತತ್;
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
UnicodeSet& UnicodeSet::add(ಧ್ರುವ UnicodeString& s) U_LIFETIME_BOUND {
    ಯದಿ (isFrozen() || isBogus()) ಪ್ರತ್ಯಾಗಮ *ಏತತ್;
    int32_t cp = getSingleCP(s);
    ಯದಿ (cp < 0) {
        ಯದಿ (!stringsContains(s)) {
            _add(s);
            releasePattern();
        }
    } ಅನ್ಯಥಾ {
        add(static_cast<UChar32>(cp));
    }
    ಪ್ರತ್ಯಾಗಮ *ಏತತ್;
}

/**
 * Adds the given string, in order, to 'strings_'.  The given string
 * must have been checked by the caller to not already be in 'strings_'.
 */
ಶೂನ್ಯ UnicodeSet::_add(ಧ್ರುವ UnicodeString& s) {
    ಯದಿ (isFrozen() || isBogus()) {
        ಪ್ರತ್ಯಾಗಮ;
    }
    UErrorCode ec = U_ZERO_ERROR;
    ಯದಿ (strings_ == nullptr && !allocateStrings(ec)) {
        setToBogus();
        ಪ್ರತ್ಯಾಗಮ;
    }
    LocalPointer<UnicodeString> t(ನೂತನ UnicodeString(s));
    ಯದಿ (t.isNull()) { // Check for memory allocation error.
        setToBogus();
        ಪ್ರತ್ಯಾಗಮ;
    }
    strings_->sorted_ಸೇರಿಸು(t.orphan(), compareUnicodeString, ec);
    ಯದಿ (U_FAILURE(ec)) {
        setToBogus();
    }
}

/**
 * @return a code point IF the string consists of a single one.
 * otherwise returns -1.
 * @param string to test
 */
int32_t UnicodeSet::getSingleCP(ಧ್ರುವ UnicodeString& s) {
    int32_t sLength = s.length();
    ಯದಿ (sLength == 1) ಪ್ರತ್ಯಾಗಮ s.charAt(0);
    ಯದಿ (sLength == 2) {
        UChar32 cp = s.char32At(0);
        ಯದಿ (cp > 0xFFFF) { // is surrogate pair
            ಪ್ರತ್ಯಾಗಮ cp;
        }
    }
    ಪ್ರತ್ಯಾಗಮ -1;
}

/**
 * Adds each of the characters in this string to the set. Thus "ch" => {"c", "h"}
 * If this set already any particular character, it has no effect on that character.
 * @param the source string
 * @return the modified set, for chaining
 */
UnicodeSet& UnicodeSet::addAll(ಧ್ರುವ UnicodeString& s) U_LIFETIME_BOUND {
    UChar32 cp;
    ಪ್ರತಿ (int32_t i = 0; i < s.length(); i += U16_LENGTH(cp)) {
        cp = s.char32At(i);
        add(cp);
    }
    ಪ್ರತ್ಯಾಗಮ *ಏತತ್;
}

/**
 * Retains EACH of the characters in this string. Note: "ch" == {"c", "h"}
 * If this set already any particular character, it has no effect on that character.
 * @param the source string
 * @return the modified set, for chaining
 */
UnicodeSet& UnicodeSet::retainAll(ಧ್ರುವ UnicodeString& s) U_LIFETIME_BOUND {
    UnicodeSet set;
    set.addAll(s);
    retainAll(set);
    ಪ್ರತ್ಯಾಗಮ *ಏತತ್;
}

/**
 * Complement EACH of the characters in this string. Note: "ch" == {"c", "h"}
 * If this set already any particular character, it has no effect on that character.
 * @param the source string
 * @return the modified set, for chaining
 */
UnicodeSet& UnicodeSet::complementAll(ಧ್ರುವ UnicodeString& s) U_LIFETIME_BOUND {
    UnicodeSet set;
    set.addAll(s);
    complementAll(set);
    ಪ್ರತ್ಯಾಗಮ *ಏತತ್;
}

/**
 * Remove EACH of the characters in this string. Note: "ch" == {"c", "h"}
 * If this set already any particular character, it has no effect on that character.
 * @param the source string
 * @return the modified set, for chaining
 */
UnicodeSet& UnicodeSet::removeAll(ಧ್ರುವ UnicodeString& s) U_LIFETIME_BOUND {
    UnicodeSet set;
    set.addAll(s);
    removeAll(set);
    ಪ್ರತ್ಯಾಗಮ *ಏತತ್;
}

UnicodeSet& UnicodeSet::removeAllStrings() U_LIFETIME_BOUND {
    ಯದಿ (!isFrozen() && hasStrings()) {
        strings_->removeAllElements();
        releasePattern();
    }
    ಪ್ರತ್ಯಾಗಮ *ಏತತ್;
}


/**
 * Makes a set from a multicharacter string. Thus "ch" => {"ch"}
 * <br><b>Warning: you cannot add an empty string ("") to a UnicodeSet.</b>
 * @param the source string
 * @return a newly created set containing the given string
 */
UnicodeSet* U_EXPORT2 UnicodeSet::createFrom(ಧ್ರುವ UnicodeString& s) {
    UnicodeSet *set = ನೂತನ UnicodeSet();
    ಯದಿ (set != nullptr) { // Check for memory allocation error.
        set->add(s);
    }
    ಪ್ರತ್ಯಾಗಮ set;
}


/**
 * Makes a set from each of the characters in the string. Thus "ch" => {"c", "h"}
 * @param the source string
 * @return a newly created set containing the given characters
 */
UnicodeSet* U_EXPORT2 UnicodeSet::createFromAll(ಧ್ರುವ UnicodeString& s) {
    UnicodeSet *set = ನೂತನ UnicodeSet();
    ಯದಿ (set != nullptr) { // Check for memory allocation error.
        set->addAll(s);
    }
    ಪ್ರತ್ಯಾಗಮ set;
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
    ಯದಿ (pinCodePoint(start) <= pinCodePoint(end)) {
        UChar32 range[3] = { start, end+1, UNICODESET_ಮೇಲಿನ };
        retain(range, 2, 0);
    } ಅನ್ಯಥಾ {
        clear();
    }
    ಪ್ರತ್ಯಾಗಮ *ಏತತ್;
}

UnicodeSet& UnicodeSet::retain(UChar32 c) U_LIFETIME_BOUND {
    ಪ್ರತ್ಯಾಗಮ retain(c, c);
}

UnicodeSet& UnicodeSet::retain(ಧ್ರುವ UnicodeString &s) U_LIFETIME_BOUND {
    ಯದಿ (isFrozen() || isBogus()) { ಪ್ರತ್ಯಾಗಮ *ಏತತ್; }
    UChar32 cp = getSingleCP(s);
    ಯದಿ (cp < 0) {
        ತಾರ್ಕಿಕ isIn = stringsContains(s);
        // Check for getRangeCount() first to avoid somewhat-expensive size()
        // when there are single code points.
        ಯದಿ (isIn && get_Range_ಎಣಿಕೆ() == 0 && size() == 1) {
            ಪ್ರತ್ಯಾಗಮ *ಏತತ್;
        }
        clear();
        ಯದಿ (isIn) {
            _add(s);
        }
    } ಅನ್ಯಥಾ {
        retain(cp, cp);
    }
    ಪ್ರತ್ಯಾಗಮ *ಏತತ್;
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
    ಯದಿ (pinCodePoint(start) <= pinCodePoint(end)) {
        UChar32 range[3] = { start, end+1, UNICODESET_ಮೇಲಿನ };
        retain(range, 2, 2);
    }
    ಪ್ರತ್ಯಾಗಮ *ಏತತ್;
}

/**
 * Removes the specified character from this set if it is present.
 * The set will not contain the specified range once the call
 * returns.
 */
UnicodeSet& UnicodeSet::remove(UChar32 c) U_LIFETIME_BOUND {
    ಪ್ರತ್ಯಾಗಮ remove(c, c);
}

/**
 * Removes the specified string from this set if it is present.
 * The set will not contain the specified character once the call
 * returns.
 * @param the source string
 * @return the modified set, for chaining
 */
UnicodeSet& UnicodeSet::remove(ಧ್ರುವ UnicodeString& s) U_LIFETIME_BOUND {
    ಯದಿ (isFrozen() || isBogus()) ಪ್ರತ್ಯಾಗಮ *ಏತತ್;
    int32_t cp = getSingleCP(s);
    ಯದಿ (cp < 0) {
        ಯದಿ (strings_ != nullptr && strings_->removeElement((ಶೂನ್ಯ*) &s)) {
            releasePattern();
        }
    } ಅನ್ಯಥಾ {
        remove(static_cast<UChar32>(cp), static_cast<UChar32>(cp));
    }
    ಪ್ರತ್ಯಾಗಮ *ಏತತ್;
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
    ಯದಿ (isFrozen() || isBogus()) {
        ಪ್ರತ್ಯಾಗಮ *ಏತತ್;
    }
    ಯದಿ (pinCodePoint(start) <= pinCodePoint(end)) {
        UChar32 range[3] = { start, end+1, UNICODESET_ಮೇಲಿನ };
        exclusiveOr(range, 2, 0);
    }
    releasePattern();
    ಪ್ರತ್ಯಾಗಮ *ಏತತ್;
}

UnicodeSet& UnicodeSet::complement(UChar32 c) U_LIFETIME_BOUND {
    ಪ್ರತ್ಯಾಗಮ complement(c, c);
}

/**
 * This is equivalent to
 * <code>complement(MIN_VALUE, MAX_VALUE)</code>.
 */
UnicodeSet& UnicodeSet::complement() U_LIFETIME_BOUND {
    ಯದಿ (isFrozen() || isBogus()) {
        ಪ್ರತ್ಯಾಗಮ *ಏತತ್;
    }
    ಯದಿ (list[0] == UNICODESET_ಕೆಳಗಿನ) {
        uprv_memmove(list, list + 1, (size_t)(len-1)*ಪರಿಮಾಣ(UChar32));
        --len;
    } ಅನ್ಯಥಾ {
        ಯದಿ (!ensureCapacity(len+1)) {
            ಪ್ರತ್ಯಾಗಮ *ಏತತ್;
        }
        uprv_memmove(list + 1, list, (size_t)len*ಪರಿಮಾಣ(UChar32));
        list[0] = UNICODESET_ಕೆಳಗಿನ;
        ++len;
    }
    releasePattern();
    ಪ್ರತ್ಯಾಗಮ *ಏತತ್;
}

/**
 * Complement the specified string in this set.
 * The set will not contain the specified string once the call
 * returns.
 *
 * @param s the string to complement
 * @return this object, for chaining
 */
UnicodeSet& UnicodeSet::complement(ಧ್ರುವ UnicodeString& s) U_LIFETIME_BOUND {
    ಯದಿ (isFrozen() || isBogus()) ಪ್ರತ್ಯಾಗಮ *ಏತತ್;
    int32_t cp = getSingleCP(s);
    ಯದಿ (cp < 0) {
        ಯದಿ (stringsContains(s)) {
            strings_->removeElement((ಶೂನ್ಯ*) &s);
        } ಅನ್ಯಥಾ {
            _add(s);
        }
        releasePattern();
    } ಅನ್ಯಥಾ {
        complement(static_cast<UChar32>(cp), static_cast<UChar32>(cp));
    }
    ಪ್ರತ್ಯಾಗಮ *ಏತತ್;
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
UnicodeSet& UnicodeSet::addAll(ಧ್ರುವ UnicodeSet& c) U_LIFETIME_BOUND {
    ಯದಿ ( c.len>0 && c.list!=nullptr ) {
        add(c.list, c.len, 0);
    }

    // Add strings in order
    ಯದಿ ( c.strings_!=nullptr ) {
        ಪ್ರತಿ (int32_t i=0; i<c.strings_->size(); ++i) {
            ಧ್ರುವ UnicodeString* s = static_cast<ಧ್ರುವ UnicodeString*>(c.strings_->elementAt(i));
            ಯದಿ (!stringsContains(*s)) {
                _add(*s);
            }
        }
    }
    ಪ್ರತ್ಯಾಗಮ *ಏತತ್;
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
UnicodeSet& UnicodeSet::retainAll(ಧ್ರುವ UnicodeSet& c) U_LIFETIME_BOUND {
    ಯದಿ (isFrozen() || isBogus()) {
        ಪ್ರತ್ಯಾಗಮ *ಏತತ್;
    }
    retain(c.list, c.len, 0);
    ಯದಿ (hasStrings()) {
        ಯದಿ (!c.hasStrings()) {
            strings_->removeAllElements();
        } ಅನ್ಯಥಾ {
            strings_->retainAll(*c.strings_);
        }
    }
    ಪ್ರತ್ಯಾಗಮ *ಏತತ್;
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
UnicodeSet& UnicodeSet::removeAll(ಧ್ರುವ UnicodeSet& c) U_LIFETIME_BOUND {
    ಯದಿ (isFrozen() || isBogus()) {
        ಪ್ರತ್ಯಾಗಮ *ಏತತ್;
    }
    retain(c.list, c.len, 2);
    ಯದಿ (hasStrings() && c.hasStrings()) {
        strings_->removeAll(*c.strings_);
    }
    ಪ್ರತ್ಯಾಗಮ *ಏತತ್;
}

/**
 * Complements in this set all elements contained in the specified
 * set.  Any character in the other set will be removed if it is
 * in this set, or will be added if it is not in this set.
 *
 * @param c set that defines which elements will be xor'ed from
 *          this set.
 */
UnicodeSet& UnicodeSet::complementAll(ಧ್ರುವ UnicodeSet& c) U_LIFETIME_BOUND {
    ಯದಿ (isFrozen() || isBogus()) {
        ಪ್ರತ್ಯಾಗಮ *ಏತತ್;
    }
    exclusiveOr(c.list, c.len, 0);

    ಯದಿ (c.strings_ != nullptr) {
        ಪ್ರತಿ (int32_t i=0; i<c.strings_->size(); ++i) {
            ಶೂನ್ಯ* e = c.strings_->elementAt(i);
            ಯದಿ (strings_ == nullptr || !strings_->removeElement(e)) {
                _add(*static_cast<ಧ್ರುವ UnicodeString*>(e));
            }
        }
    }
    ಪ್ರತ್ಯಾಗಮ *ಏತತ್;
}

/**
 * Removes all of the elements from this set.  This set will be
 * empty after this call returns.
 */
UnicodeSet& UnicodeSet::clear() U_LIFETIME_BOUND {
    ಯದಿ (isFrozen()) {
        ಪ್ರತ್ಯಾಗಮ *ಏತತ್;
    }
    list[0] = UNICODESET_ಮೇಲಿನ;
    len = 1;
    releasePattern();
    ಯದಿ (strings_ != nullptr) {
        strings_->removeAllElements();
    }
    // Remove bogus
    fFlags = 0;
    ಪ್ರತ್ಯಾಗಮ *ಏತತ್;
}

/**
 * Iteration method that returns the number of ranges contained in
 * this set.
 * @see #getRangeStart
 * @see #getRangeEnd
 */
int32_t UnicodeSet::get_Range_ಎಣಿಕೆ() ಧ್ರುವ {
    ಪ್ರತ್ಯಾಗಮ len/2;
}

/**
 * Iteration method that returns the first character in the
 * specified range of this set.
 * @see #getRangeCount
 * @see #getRangeEnd
 */
UChar32 UnicodeSet::getRangeStart(int32_t index) ಧ್ರುವ {
    ಪ್ರತ್ಯಾಗಮ list[index*2];
}

/**
 * Iteration method that returns the last character in the
 * specified range of this set.
 * @see #getRangeStart
 * @see #getRangeEnd
 */
UChar32 UnicodeSet::getRangeEnd(int32_t index) ಧ್ರುವ {
    ಪ್ರತ್ಯಾಗಮ list[index*2 + 1] - 1;
}

ಧ್ರುವ UnicodeString* UnicodeSet::getString(int32_t index) ಧ್ರುವ U_LIFETIME_BOUND {
    ಪ್ರತ್ಯಾಗಮ static_cast<ಧ್ರುವ UnicodeString*>(strings_->elementAt(index));
}

/**
 * Reallocate this objects internal structures to take up the least
 * possible space, without changing this object's value.
 */
UnicodeSet& UnicodeSet::compact() U_LIFETIME_BOUND {
    ಯದಿ (isFrozen() || isBogus()) {
        ಪ್ರತ್ಯಾಗಮ *ಏತತ್;
    }
    // Delete buffer first to defragment memory less.
    ಯದಿ (buffer != ರಾಶಿ_List) {
        uprv_free(buffer);
        buffer = nullptr;
        bufferCapacity = 0;
    }
    ಯದಿ (list == ರಾಶಿ_List) {
        // pass
    } ಅನ್ಯಥಾ ಯದಿ (len <= INITIAL_CAPACITY) {
        uprv_memcpy(ರಾಶಿ_List, list, len * ಪರಿಮಾಣ(UChar32));
        uprv_free(list);
        list = ರಾಶಿ_List;
        capacity = INITIAL_CAPACITY;
    } ಅನ್ಯಥಾ ಯದಿ ((len + 7) < capacity) {
        // If we have more than a little unused capacity, shrink it to len.
        UChar32* temp = static_cast<UChar32*>(uprv_realloc(list, ಪರಿಮಾಣ(UChar32) * len));
        ಯದಿ (temp) {
            list = temp;
            capacity = len;
        }
        // else what the heck happened?! We allocated less memory!
        // Oh well. We'll keep our original array.
    }
    ಯದಿ (strings_ != nullptr && strings_->isEmpty()) {
        delete strings_;
        strings_ = nullptr;
    }
    ಪ್ರತ್ಯಾಗಮ *ಏತತ್;
}

#ifdef DEBUG_SERIALIZE
#ಅನ್ತರ್ಭಾವ <stdio.h>
#endif

/**
 * Deserialize constructor.
 */
UnicodeSet::UnicodeSet(ಧ್ರುವ uint16_t data[], int32_t dataLen, ESerialization serialization,
                       UErrorCode &ec) {

  ಯದಿ(U_FAILURE(ec)) {
    setToBogus();
    ಪ್ರತ್ಯಾಗಮ;
  }

  ಯದಿ( (serialization != kSerialized)
      || (data==nullptr)
      || (dataLen < 1)) {
    ec = U_ILLEGAL_ARGUMENT_ERROR;
    setToBogus();
    ಪ್ರತ್ಯಾಗಮ;
  }

  // bmp?
  int32_t headerSize = ((data[0]&0x8000)) ?2:1;
  int32_t bmpLength = (headerSize==1)?data[0]:data[1];

  int32_t newLength = (((data[0]&0x7FFF)-bmpLength)/2)+bmpLength;
#ifdef DEBUG_SERIALIZE
  printf("dataLen %d headerSize %d bmpLen %d len %d. data[0]=%X/%X/%X/%X\n", dataLen,headerSize,bmpLength,newLength, data[0],data[1],data[2],data[3]);
#endif
  ಯದಿ(!ensureCapacity(newLength + 1)) {  // +1 for HIGH
    ಪ್ರತ್ಯಾಗಮ;
  }
  // copy bmp
  int32_t i;
  ಪ್ರತಿ(i = 0; i< bmpLength;i++) {
    list[i] = data[i+headerSize];
#ifdef DEBUG_SERIALIZE
    printf("<<16@%d[%d] %X\n", i+headerSize, i, list[i]);
#endif
  }
  // copy smp
  ಪ್ರತಿ(i=bmpLength;i<newLength;i++) {
    list[i] = (static_cast<UChar32>(data[headerSize + bmpLength + (i - bmpLength) * 2 + 0]) << 16) +
               static_cast<UChar32>(data[headerSize + bmpLength + (i - bmpLength) * 2 + 1]);
#ifdef DEBUG_SERIALIZE
    printf("<<32@%d+[%d] %lX\n", headerSize+bmpLength+i, i, list[i]);
#endif
  }
  U_ASSERT(i == newLength);
  ಯದಿ (i == 0 || list[i - 1] != UNICODESET_ಮೇಲಿನ) {
    list[i++] = UNICODESET_ಮೇಲಿನ;
  }
  len = i;
}


int32_t UnicodeSet::serialize(uint16_t *dest, int32_t destCapacity, UErrorCode& ec) ಧ್ರುವ {
    int32_t bmpLength, length, destLength;

    ಯದಿ (U_FAILURE(ec)) {
        ಪ್ರತ್ಯಾಗಮ 0;
    }

    ಯದಿ (destCapacity<0 || (destCapacity>0 && dest==nullptr)) {
        ec=U_ILLEGAL_ARGUMENT_ERROR;
        ಪ್ರತ್ಯಾಗಮ 0;
    }

    /* count necessary 16-bit units */
    length=ಏತತ್->len-1; // Subtract 1 to ignore final UNICODESET_HIGH
    // assert(length>=0);
    ಯದಿ (length==0) {
        /* empty set */
        ಯದಿ (destCapacity>0) {
            *dest=0;
        } ಅನ್ಯಥಾ {
            ec=U_BUFFER_OVERFLOW_ERROR;
        }
        ಪ್ರತ್ಯಾಗಮ 1;
    }
    /* now length>0 */

    ಯದಿ (ಏತತ್->list[length-1]<=0xffff) {
        /* all BMP */
        bmpLength=length;
    } ಅನ್ಯಥಾ ಯದಿ (ಏತತ್->list[0]>=0x10000) {
        /* all supplementary */
        bmpLength=0;
        length*=2;
    } ಅನ್ಯಥಾ {
        /* some BMP, some supplementary */
        ಪ್ರತಿ (bmpLength=0; bmpLength<length && ಏತತ್->list[bmpLength]<=0xffff; ++bmpLength) {}
        length=bmpLength+2*(length-bmpLength);
    }
#ifdef DEBUG_SERIALIZE
    printf(">> bmpLength%d length%d len%d\n", bmpLength, length, len);
#endif
    /* length: number of 16-bit array units */
    ಯದಿ (length>0x7fff) {
        /* there are only 15 bits for the length in the first serialized word */
        ec=U_INDEX_OUTOFBOUNDS_ERROR;
        ಪ್ರತ್ಯಾಗಮ 0;
    }

    /*
     * total serialized length:
     * number of 16-bit array units (length) +
     * 1 length unit (always) +
     * 1 bmpLength unit (if there are supplementary values)
     */
    destLength=length+((length>bmpLength)?2:1);
    ಯದಿ (destLength<=destCapacity) {
        ಧ್ರುವ UChar32 *p;
        int32_t i;

#ifdef DEBUG_SERIALIZE
        printf("writeHdr\n");
#endif
        *dest = static_cast<uint16_t>(length);
        ಯದಿ (length>bmpLength) {
            *dest|=0x8000;
            *++dest = static_cast<uint16_t>(bmpLength);
        }
        ++dest;

        /* write the BMP part of the array */
        p=ಏತತ್->list;
        ಪ್ರತಿ (i=0; i<bmpLength; ++i) {
#ifdef DEBUG_SERIALIZE
          printf("writebmp: %x\n", (ಪೂರ್ಣಾಙ್ಕ)*p);
#endif
            *dest++ = static_cast<uint16_t>(*p++);
        }

        /* write the supplementary part of the array */
        ಪ್ರತಿ (; i<length; i+=2) {
#ifdef DEBUG_SERIALIZE
          printf("write32: %x\n", (ಪೂರ್ಣಾಙ್ಕ)*p);
#endif
            *dest++ = static_cast<uint16_t>(*p >> 16);
            *dest++ = static_cast<uint16_t>(*p++);
        }
    } ಅನ್ಯಥಾ {
        ec=U_BUFFER_OVERFLOW_ERROR;
    }
    ಪ್ರತ್ಯಾಗಮ destLength;
}

//----------------------------------------------------------------
// Implementation: Utility methods
//----------------------------------------------------------------

/**
 * Allocate our strings vector and return true if successful.
 */
UBool UnicodeSet::allocateStrings(UErrorCode &status) {
    ಯದಿ (U_FAILURE(status)) {
        ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;
    }
    strings_ = ನೂತನ UVector(uprv_deleteUObject,
                          uhash_compareUnicodeString, 1, status);
    ಯದಿ (strings_ == nullptr) { // Check for memory allocation error.
        status = U_MEMORY_ALLOCATION_ERROR;
        ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;
    }
    ಯದಿ (U_FAILURE(status)) {
        delete strings_;
        strings_ = nullptr;
        ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;
    } 
    ಪ್ರತ್ಯಾಗಮ ಸತ್ಯ;
}

int32_t UnicodeSet::nextCapacity(int32_t minCapacity) {
    // Grow exponentially to reduce the frequency of allocations.
    ಯದಿ (minCapacity < INITIAL_CAPACITY) {
        ಪ್ರತ್ಯಾಗಮ minCapacity + INITIAL_CAPACITY;
    } ಅನ್ಯಥಾ ಯದಿ (minCapacity <= 2500) {
        ಪ್ರತ್ಯಾಗಮ 5 * minCapacity;
    } ಅನ್ಯಥಾ {
        int32_t newCapacity = 2 * minCapacity;
        ಯದಿ (newCapacity > MAX_LENGTH) {
            newCapacity = MAX_LENGTH;
        }
        ಪ್ರತ್ಯಾಗಮ newCapacity;
    }
}

ತಾರ್ಕಿಕ UnicodeSet::ensureCapacity(int32_t newLen) {
    ಯದಿ (newLen > MAX_LENGTH) {
        newLen = MAX_LENGTH;
    }
    ಯದಿ (newLen <= capacity) {
        ಪ್ರತ್ಯಾಗಮ ಸತ್ಯ;
    }
    int32_t newCapacity = nextCapacity(newLen);
    UChar32* temp = static_cast<UChar32*>(uprv_malloc(newCapacity * ಪರಿಮಾಣ(UChar32)));
    ಯದಿ (temp == nullptr) {
        setToBogus(); // set the object to bogus state if an OOM failure occurred.
        ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;
    }
    // Copy only the actual contents.
    uprv_memcpy(temp, list, len * ಪರಿಮಾಣ(UChar32));
    ಯದಿ (list != ರಾಶಿ_List) {
        uprv_free(list);
    }
    list = temp;
    capacity = newCapacity;
    ಪ್ರತ್ಯಾಗಮ ಸತ್ಯ;
}

ತಾರ್ಕಿಕ UnicodeSet::ensureBufferCapacity(int32_t newLen) {
    ಯದಿ (newLen > MAX_LENGTH) {
        newLen = MAX_LENGTH;
    }
    ಯದಿ (newLen <= bufferCapacity) {
        ಪ್ರತ್ಯಾಗಮ ಸತ್ಯ;
    }
    int32_t newCapacity = nextCapacity(newLen);
    UChar32* temp = static_cast<UChar32*>(uprv_malloc(newCapacity * ಪರಿಮಾಣ(UChar32)));
    ಯದಿ (temp == nullptr) {
        setToBogus();
        ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;
    }
    // The buffer has no contents to be copied.
    // It is always filled from scratch after this call.
    ಯದಿ (buffer != ರಾಶಿ_List) {
        uprv_free(buffer);
    }
    buffer = temp;
    bufferCapacity = newCapacity;
    ಪ್ರತ್ಯಾಗಮ ಸತ್ಯ;
}

/**
 * Swap list and buffer.
 */
ಶೂನ್ಯ UnicodeSet::swapBuffers() {
    // swap list and buffer
    UChar32* temp = list;
    list = buffer;
    buffer = temp;

    int32_t c = capacity;
    capacity = bufferCapacity;
    bufferCapacity = c;
}

ಶೂನ್ಯ UnicodeSet::setToBogus() {
    clear(); // Remove everything in the set.
    fFlags = kIsBogus;
}

//----------------------------------------------------------------
// Implementation: Fundamental operators
//----------------------------------------------------------------

ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ UChar32 max(UChar32 a, UChar32 b) {
    ಪ್ರತ್ಯಾಗಮ (a > b) ? a : b;
}

// polarity = 0, 3 is normal: x xor y
// polarity = 1, 2: x xor ~y == x === y

ಶೂನ್ಯ UnicodeSet::exclusiveOr(ಧ್ರುವ UChar32* other, int32_t otherLen, int8_t polarity) {
    ಯದಿ (isFrozen() || isBogus()) {
        ಪ್ರತ್ಯಾಗಮ;
    }
    ಯದಿ (!ensureBufferCapacity(len + otherLen)) {
        ಪ್ರತ್ಯಾಗಮ;
    }

    int32_t i = 0, j = 0, k = 0;
    UChar32 a = list[i++];
    UChar32 b;
    ಯದಿ (polarity == 1 || polarity == 2) {
        b = UNICODESET_ಕೆಳಗಿನ;
        ಯದಿ (other[j] == UNICODESET_ಕೆಳಗಿನ) { // skip base if already LOW
            ++j;
            b = other[j];
        }
    } ಅನ್ಯಥಾ {
        b = other[j++];
    }
    // simplest of all the routines
    // sort the values, discarding identicals!
    ಪ್ರತಿ (;;) {
        ಯದಿ (a < b) {
            buffer[k++] = a;
            a = list[i++];
        } ಅನ್ಯಥಾ ಯದಿ (b < a) {
            buffer[k++] = b;
            b = other[j++];
        } ಅನ್ಯಥಾ ಯದಿ (a != UNICODESET_ಮೇಲಿನ) { // at this point, a == b
            // discard both values!
            a = list[i++];
            b = other[j++];
        } ಅನ್ಯಥಾ { // DONE!
            buffer[k++] = UNICODESET_ಮೇಲಿನ;
            len = k;
            ಭಙ್ಗ;
        }
    }
    swapBuffers();
    releasePattern();
}

// polarity = 0 is normal: x union y
// polarity = 2: x union ~y
// polarity = 1: ~x union y
// polarity = 3: ~x union ~y

ಶೂನ್ಯ UnicodeSet::add(ಧ್ರುವ UChar32* other, int32_t otherLen, int8_t polarity) {
    ಯದಿ (isFrozen() || isBogus() || other==nullptr) {
        ಪ್ರತ್ಯಾಗಮ;
    }
    ಯದಿ (!ensureBufferCapacity(len + otherLen)) {
        ಪ್ರತ್ಯಾಗಮ;
    }

    int32_t i = 0, j = 0, k = 0;
    UChar32 a = list[i++];
    UChar32 b = other[j++];
    // change from xor is that we have to check overlapping pairs
    // polarity bit 1 means a is second, bit 2 means b is.
    ಪ್ರತಿ (;;) {
        ವಿಕಲ್ಪ (polarity) {
          ಪಕ್ಷ 0: // both first; take lower if unequal
            ಯದಿ (a < b) { // take a
                // Back up over overlapping ranges in buffer[]
                ಯದಿ (k > 0 && a <= buffer[k-1]) {
                    // Pick latter end value in buffer[] vs. list[]
                    a = max(list[i], buffer[--k]);
                } ಅನ್ಯಥಾ {
                    // No overlap
                    buffer[k++] = a;
                    a = list[i];
                }
                i++; // Common if/else code factored out
                polarity ^= 1;
            } ಅನ್ಯಥಾ ಯದಿ (b < a) { // take b
                ಯದಿ (k > 0 && b <= buffer[k-1]) {
                    b = max(other[j], buffer[--k]);
                } ಅನ್ಯಥಾ {
                    buffer[k++] = b;
                    b = other[j];
                }
                j++;
                polarity ^= 2;
            } ಅನ್ಯಥಾ { // a == b, take a, drop b
                ಯದಿ (a == UNICODESET_ಮೇಲಿನ) ಗಚ್ಛ loop_end;
                // This is symmetrical; it doesn't matter if
                // we backtrack with a or b. - liu
                ಯದಿ (k > 0 && a <= buffer[k-1]) {
                    a = max(list[i], buffer[--k]);
                } ಅನ್ಯಥಾ {
                    // No overlap
                    buffer[k++] = a;
                    a = list[i];
                }
                i++;
                polarity ^= 1;
                b = other[j++];
                polarity ^= 2;
            }
            ಭಙ್ಗ;
          ಪಕ್ಷ 3: // both second; take higher if unequal, and drop other
            ಯದಿ (b <= a) { // take a
                ಯದಿ (a == UNICODESET_ಮೇಲಿನ) ಗಚ್ಛ loop_end;
                buffer[k++] = a;
            } ಅನ್ಯಥಾ { // take b
                ಯದಿ (b == UNICODESET_ಮೇಲಿನ) ಗಚ್ಛ loop_end;
                buffer[k++] = b;
            }
            a = list[i++];
            polarity ^= 1;   // factored common code
            b = other[j++];
            polarity ^= 2;
            ಭಙ್ಗ;
          ಪಕ್ಷ 1: // a second, b first; if b < a, overlap
            ಯದಿ (a < b) { // no overlap, take a
                buffer[k++] = a; a = list[i++]; polarity ^= 1;
            } ಅನ್ಯಥಾ ಯದಿ (b < a) { // OVERLAP, drop b
                b = other[j++];
                polarity ^= 2;
            } ಅನ್ಯಥಾ { // a == b, drop both!
                ಯದಿ (a == UNICODESET_ಮೇಲಿನ) ಗಚ್ಛ loop_end;
                a = list[i++];
                polarity ^= 1;
                b = other[j++];
                polarity ^= 2;
            }
            ಭಙ್ಗ;
          ಪಕ್ಷ 2: // a first, b second; if a < b, overlap
            ಯದಿ (b < a) { // no overlap, take b
                buffer[k++] = b;
                b = other[j++];
                polarity ^= 2;
            } ಅನ್ಯಥಾ  ಯದಿ (a < b) { // OVERLAP, drop a
                a = list[i++];
                polarity ^= 1;
            } ಅನ್ಯಥಾ { // a == b, drop both!
                ಯದಿ (a == UNICODESET_ಮೇಲಿನ) ಗಚ್ಛ loop_end;
                a = list[i++];
                polarity ^= 1;
                b = other[j++];
                polarity ^= 2;
            }
            ಭಙ್ಗ;
        }
    }
 loop_end:
    buffer[k++] = UNICODESET_ಮೇಲಿನ;    // terminate
    len = k;
    swapBuffers();
    releasePattern();
}

// polarity = 0 is normal: x intersect y
// polarity = 2: x intersect ~y == set-minus
// polarity = 1: ~x intersect y
// polarity = 3: ~x intersect ~y

ಶೂನ್ಯ UnicodeSet::retain(ಧ್ರುವ UChar32* other, int32_t otherLen, int8_t polarity) {
    ಯದಿ (isFrozen() || isBogus()) {
        ಪ್ರತ್ಯಾಗಮ;
    }
    ಯದಿ (!ensureBufferCapacity(len + otherLen)) {
        ಪ್ರತ್ಯಾಗಮ;
    }

    int32_t i = 0, j = 0, k = 0;
    UChar32 a = list[i++];
    UChar32 b = other[j++];
    // change from xor is that we have to check overlapping pairs
    // polarity bit 1 means a is second, bit 2 means b is.
    ಪ್ರತಿ (;;) {
        ವಿಕಲ್ಪ (polarity) {
          ಪಕ್ಷ 0: // both first; drop the smaller
            ಯದಿ (a < b) { // drop a
                a = list[i++];
                polarity ^= 1;
            } ಅನ್ಯಥಾ ಯದಿ (b < a) { // drop b
                b = other[j++];
                polarity ^= 2;
            } ಅನ್ಯಥಾ { // a == b, take one, drop other
                ಯದಿ (a == UNICODESET_ಮೇಲಿನ) ಗಚ್ಛ loop_end;
                buffer[k++] = a;
                a = list[i++];
                polarity ^= 1;
                b = other[j++];
                polarity ^= 2;
            }
            ಭಙ್ಗ;
          ಪಕ್ಷ 3: // both second; take lower if unequal
            ಯದಿ (a < b) { // take a
                buffer[k++] = a;
                a = list[i++];
                polarity ^= 1;
            } ಅನ್ಯಥಾ ಯದಿ (b < a) { // take b
                buffer[k++] = b;
                b = other[j++];
                polarity ^= 2;
            } ಅನ್ಯಥಾ { // a == b, take one, drop other
                ಯದಿ (a == UNICODESET_ಮೇಲಿನ) ಗಚ್ಛ loop_end;
                buffer[k++] = a;
                a = list[i++];
                polarity ^= 1;
                b = other[j++];
                polarity ^= 2;
            }
            ಭಙ್ಗ;
          ಪಕ್ಷ 1: // a second, b first;
            ಯದಿ (a < b) { // NO OVERLAP, drop a
                a = list[i++];
                polarity ^= 1;
            } ಅನ್ಯಥಾ ಯದಿ (b < a) { // OVERLAP, take b
                buffer[k++] = b;
                b = other[j++];
                polarity ^= 2;
            } ಅನ್ಯಥಾ { // a == b, drop both!
                ಯದಿ (a == UNICODESET_ಮೇಲಿನ) ಗಚ್ಛ loop_end;
                a = list[i++];
                polarity ^= 1;
                b = other[j++];
                polarity ^= 2;
            }
            ಭಙ್ಗ;
          ಪಕ್ಷ 2: // a first, b second; if a < b, overlap
            ಯದಿ (b < a) { // no overlap, drop b
                b = other[j++];
                polarity ^= 2;
            } ಅನ್ಯಥಾ  ಯದಿ (a < b) { // OVERLAP, take a
                buffer[k++] = a;
                a = list[i++];
                polarity ^= 1;
            } ಅನ್ಯಥಾ { // a == b, drop both!
                ಯದಿ (a == UNICODESET_ಮೇಲಿನ) ಗಚ್ಛ loop_end;
                a = list[i++];
                polarity ^= 1;
                b = other[j++];
                polarity ^= 2;
            }
            ಭಙ್ಗ;
        }
    }
 loop_end:
    buffer[k++] = UNICODESET_ಮೇಲಿನ;    // terminate
    len = k;
    swapBuffers();
    releasePattern();
}

/**
 * Append the <code>toPattern()</code> representation of a
 * string to the given <code>StringBuffer</code>.
 */
ಶೂನ್ಯ UnicodeSet::_appendToPat(UnicodeString& buf, ಧ್ರುವ UnicodeString& s, UBool escapeUnprintable) {
    UChar32 cp;
    ಪ್ರತಿ (int32_t i = 0; i < s.length(); i += U16_LENGTH(cp)) {
        _appendToPat(buf, cp = s.char32At(i), escapeUnprintable);
    }
}

/**
 * Append the <code>toPattern()</code> representation of a
 * character to the given <code>StringBuffer</code>.
 */
ಶೂನ್ಯ UnicodeSet::_appendToPat(UnicodeString& buf, UChar32 c, UBool escapeUnprintable) {
    ಯದಿ (escapeUnprintable ? ICU_Utility::isUnprintable(c) : ICU_Utility::shouldAlwaysBeEscaped(c)) {
        // Use hex escape notation (\uxxxx or \Uxxxxxxxx) for anything
        // unprintable
        ICU_Utility::escape(buf, c);
        ಪ್ರತ್ಯಾಗಮ;
    }
    // Okay to let ':' pass through
    ವಿಕಲ್ಪ (c) {
    ಪಕ್ಷ u'[':
    ಪಕ್ಷ u']':
    ಪಕ್ಷ u'-':
    ಪಕ್ಷ u'^':
    ಪಕ್ಷ u'&':
    ಪಕ್ಷ u'\\':
    ಪಕ್ಷ u'{':
    ಪಕ್ಷ u'}':
    ಪಕ್ಷ u':':
    ಪಕ್ಷ ಚಿಹ್ನೆ_Table::ಚಿಹ್ನೆ_REF:
        buf.append(u'\\');
        ಭಙ್ಗ;
    ಸ್ವಭಾವ:
        // Escape whitespace
        ಯದಿ (PatternProps::isWhiteSpace(c)) {
            buf.append(u'\\');
        }
        ಭಙ್ಗ;
    }
    buf.append(c);
}

ಶೂನ್ಯ UnicodeSet::_appendToPat(UnicodeString &result, UChar32 start, UChar32 end,
                              UBool escapeUnprintable) {
    _appendToPat(result, start, escapeUnprintable);
    ಯದಿ (start != end) {
        ಯದಿ ((start+1) != end ||
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
                                      UBool escapeUnprintable) ಧ್ರುವ
{
    ಯದಿ (pat != nullptr) {
        int32_t i;
        int32_t backslash_ಎಣಿಕೆ = 0;
        ಪ್ರತಿ (i=0; i<patLen; ) {
            UChar32 c;
            U16_NEXT(pat, i, patLen, c);
            ಯದಿ (escapeUnprintable ?
                    ICU_Utility::isUnprintable(c) : ICU_Utility::shouldAlwaysBeEscaped(c)) {
                // If the unprintable character is preceded by an odd
                // number of backslashes, then it has been escaped.
                // Before unescaping it, we delete the final
                // backslash.
                ಯದಿ ((backslash_ಎಣಿಕೆ % 2) == 1) {
                    result.truncate(result.length() - 1);
                }
                ICU_Utility::escape(result, c);
                backslash_ಎಣಿಕೆ = 0;
            } ಅನ್ಯಥಾ {
                result.append(c);
                ಯದಿ (c == u'\\') {
                    ++backslash_ಎಣಿಕೆ;
                } ಅನ್ಯಥಾ {
                    backslash_ಎಣಿಕೆ = 0;
                }
            }
        }
        ಪ್ರತ್ಯಾಗಮ result;
    }

    ಪ್ರತ್ಯಾಗಮ _generatePattern(result, escapeUnprintable);
}

/**
 * Returns a string representation of this set.  If the result of
 * calling this function is passed to a UnicodeSet constructor, it
 * will produce another set that is equal to this one.
 */
UnicodeString& UnicodeSet::toPattern(UnicodeString& result U_LIFETIME_BOUND,
                                     UBool escapeUnprintable) ಧ್ರುವ
{
    result.truncate(0);
    ಪ್ರತ್ಯಾಗಮ _toPattern(result, escapeUnprintable);
}

/**
 * Generate and append a string representation of this set to result.
 * This does not use this.pat, the cleaned up copy of the string
 * passed to applyPattern().
 */
UnicodeString& UnicodeSet::_generatePattern(UnicodeString& result U_LIFETIME_BOUND,
                                            UBool escapeUnprintable) ಧ್ರುವ
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
    ಯದಿ (len >= 4 && list[0] == 0 && limit == len && !hasStrings()) {
        // Emit the inverse
        result.append(u'^');
        // Offsetting the inversion list index by one lets us
        // iterate over the ranges of the set complement.
        i = 1;
        --limit;
    }

    // Emit the ranges as pairs.
    ಯಾವತ್ (i < limit) {
        UChar32 start = list[i];  // getRangeStart()
        UChar32 end = list[i + 1] - 1;  // getRangeEnd() = range limit minus one
        ಯದಿ (!(0xd800 <= end && end <= 0xdbff)) {
            _appendToPat(result, start, end, escapeUnprintable);
            i += 2;
        } ಅನ್ಯಥಾ {
            // The range ends with a lead surrogate.
            // Avoid writing what looks like a lead+trail surrogate pair.
            // 1. Postpone ranges that start with a lead surrogate code point.
            int32_t firstLead = i;
            ಯಾವತ್ ((i += 2) < limit && list[i] <= 0xdbff) {}
            int32_t firstAfterLead = i;
            // 2. Write following ranges that start with a trail surrogate code point.
            ಯಾವತ್ (i < limit && (start = list[i]) <= 0xdfff) {
                _appendToPat(result, start, list[i + 1] - 1, escapeUnprintable);
                i += 2;
            }
            // 3. Now write the postponed ranges.
            ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ j = firstLead; j < firstAfterLead; j += 2) {
                _appendToPat(result, list[j], list[j + 1] - 1, escapeUnprintable);
            }
        }
    }

    ಯದಿ (strings_ != nullptr) {
        ಪ್ರತಿ (int32_t i = 0; i<strings_->size(); ++i) {
            result.append(u'{');
            _appendToPat(result,
                         *static_cast<ಧ್ರುವ UnicodeString*>(strings_->elementAt(i)),
                         escapeUnprintable);
            result.append(u'}');
        }
    }
    ಪ್ರತ್ಯಾಗಮ result.append(u']');
}

/**
* Release existing cached pattern
*/
ಶೂನ್ಯ UnicodeSet::releasePattern() {
    ಯದಿ (pat) {
        uprv_free(pat);
        pat = nullptr;
        patLen = 0;
    }
}

/**
* Set the new pattern to cache.
*/
ಶೂನ್ಯ UnicodeSet::setPattern(ಧ್ರುವ char16_t *newPat, int32_t newPatLen) {
    releasePattern();
    pat = static_cast<char16_t*>(uprv_malloc((newPatLen + 1) * ಪರಿಮಾಣ(char16_t)));
    ಯದಿ (pat) {
        patLen = newPatLen;
        u_memcpy(pat, newPat, patLen);
        pat[patLen] = 0;
    }
    // else we don't care if malloc failed. This was just a nice cache.
    // We can regenerate an equivalent pattern later when requested.
}

UnicodeSet* UnicodeSet::freeze() U_LIFETIME_BOUND {
    ಯದಿ(!isFrozen() && !isBogus()) {
        compact();

        // Optimize contains() and span() and similar functions.
        ಯದಿ (hasStrings()) {
            stringSpan = ನೂತನ UnicodeSetStringSpan(*ಏತತ್, *strings_, UnicodeSetStringSpan::ALL);
            ಯದಿ (stringSpan == nullptr) {
                setToBogus();
                ಪ್ರತ್ಯಾಗಮ ಏತತ್;
            } ಅನ್ಯಥಾ ಯದಿ (!stringSpan->needsStringSpanUTF16()) {
                // All strings are irrelevant for span() etc. because
                // all of each string's code points are contained in this set.
                // Do not check needsStringSpanUTF8() because UTF-8 has at most as
                // many relevant strings as UTF-16.
                // (Thus needsStringSpanUTF8() implies needsStringSpanUTF16().)
                delete stringSpan;
                stringSpan = nullptr;
            }
        }
        ಯದಿ (stringSpan == nullptr) {
            // No span-relevant strings: Optimize for code point spans.
            bmpSet=ನೂತನ BMPSet(list, len);
            ಯದಿ (bmpSet == nullptr) { // Check for memory allocation error.
                setToBogus();
            }
        }
    }
    ಪ್ರತ್ಯಾಗಮ ಏತತ್;
}

int32_t UnicodeSet::span(ಧ್ರುವ char16_t *s, int32_t length, USetSpanCondition spanCondition) ಧ್ರುವ {
    ಯದಿ(length>0 && bmpSet!=nullptr) {
        ಪ್ರತ್ಯಾಗಮ static_cast<int32_t>(bmpSet->span(s, s + length, spanCondition) - s);
    }
    ಯದಿ(length<0) {
        length=u_strlen(s);
    }
    ಯದಿ(length==0) {
        ಪ್ರತ್ಯಾಗಮ 0;
    }
    ಯದಿ(stringSpan!=nullptr) {
        ಪ್ರತ್ಯಾಗಮ stringSpan->span(s, length, spanCondition);
    } ಅನ್ಯಥಾ ಯದಿ(hasStrings()) {
        uint32_t which= spanCondition==USET_SPAN_NOT_CONTAINED ?
                            UnicodeSetStringSpan::FWD_UTF16_NOT_CONTAINED :
                            UnicodeSetStringSpan::FWD_UTF16_CONTAINED;
        UnicodeSetStringSpan strSpan(*ಏತತ್, *strings_, which);
        ಯದಿ(strSpan.needsStringSpanUTF16()) {
            ಪ್ರತ್ಯಾಗಮ strSpan.span(s, length, spanCondition);
        }
    }

    ಯದಿ(spanCondition!=USET_SPAN_NOT_CONTAINED) {
        spanCondition=USET_SPAN_CONTAINED;  // Pin to 0/1 values.
    }

    UChar32 c;
    int32_t start=0, prev=0;
    ಕುರು {
        U16_NEXT(s, start, length, c);
        ಯದಿ(spanCondition!=contains(c)) {
            ಭಙ್ಗ;
        }
    } ಯಾವತ್((prev=start)<length);
    ಪ್ರತ್ಯಾಗಮ prev;
}

int32_t UnicodeSet::spanBack(ಧ್ರುವ char16_t *s, int32_t length, USetSpanCondition spanCondition) ಧ್ರುವ {
    ಯದಿ(length>0 && bmpSet!=nullptr) {
        ಪ್ರತ್ಯಾಗಮ static_cast<int32_t>(bmpSet->spanBack(s, s + length, spanCondition) - s);
    }
    ಯದಿ(length<0) {
        length=u_strlen(s);
    }
    ಯದಿ(length==0) {
        ಪ್ರತ್ಯಾಗಮ 0;
    }
    ಯದಿ(stringSpan!=nullptr) {
        ಪ್ರತ್ಯಾಗಮ stringSpan->spanBack(s, length, spanCondition);
    } ಅನ್ಯಥಾ ಯದಿ(hasStrings()) {
        uint32_t which= spanCondition==USET_SPAN_NOT_CONTAINED ?
                            UnicodeSetStringSpan::BACK_UTF16_NOT_CONTAINED :
                            UnicodeSetStringSpan::BACK_UTF16_CONTAINED;
        UnicodeSetStringSpan strSpan(*ಏತತ್, *strings_, which);
        ಯದಿ(strSpan.needsStringSpanUTF16()) {
            ಪ್ರತ್ಯಾಗಮ strSpan.spanBack(s, length, spanCondition);
        }
    }

    ಯದಿ(spanCondition!=USET_SPAN_NOT_CONTAINED) {
        spanCondition=USET_SPAN_CONTAINED;  // Pin to 0/1 values.
    }

    UChar32 c;
    int32_t prev=length;
    ಕುರು {
        U16_PREV(s, 0, length, c);
        ಯದಿ(spanCondition!=contains(c)) {
            ಭಙ್ಗ;
        }
    } ಯಾವತ್((prev=length)>0);
    ಪ್ರತ್ಯಾಗಮ prev;
}

int32_t UnicodeSet::spanUTF8(ಧ್ರುವ ವರ್ಣ *s, int32_t length, USetSpanCondition spanCondition) ಧ್ರುವ {
    ಯದಿ(length>0 && bmpSet!=nullptr) {
        ಧ್ರುವ uint8_t* s0 = reinterpret_cast<ಧ್ರುವ uint8_t*>(s);
        ಪ್ರತ್ಯಾಗಮ static_cast<int32_t>(bmpSet->spanUTF8(s0, length, spanCondition) - s0);
    }
    ಯದಿ(length<0) {
        length = static_cast<int32_t>(uprv_strlen(s));
    }
    ಯದಿ(length==0) {
        ಪ್ರತ್ಯಾಗಮ 0;
    }
    ಯದಿ(stringSpan!=nullptr) {
        ಪ್ರತ್ಯಾಗಮ stringSpan->spanUTF8(reinterpret_cast<ಧ್ರುವ uint8_t*>(s), length, spanCondition);
    } ಅನ್ಯಥಾ ಯದಿ(hasStrings()) {
        uint32_t which= spanCondition==USET_SPAN_NOT_CONTAINED ?
                            UnicodeSetStringSpan::FWD_UTF8_NOT_CONTAINED :
                            UnicodeSetStringSpan::FWD_UTF8_CONTAINED;
        UnicodeSetStringSpan strSpan(*ಏತತ್, *strings_, which);
        ಯದಿ(strSpan.needsStringSpanUTF8()) {
            ಪ್ರತ್ಯಾಗಮ strSpan.spanUTF8(reinterpret_cast<ಧ್ರುವ uint8_t*>(s), length, spanCondition);
        }
    }

    ಯದಿ(spanCondition!=USET_SPAN_NOT_CONTAINED) {
        spanCondition=USET_SPAN_CONTAINED;  // Pin to 0/1 values.
    }

    UChar32 c;
    int32_t start=0, prev=0;
    ಕುರು {
        U8_NEXT_OR_FFFD(s, start, length, c);
        ಯದಿ(spanCondition!=contains(c)) {
            ಭಙ್ಗ;
        }
    } ಯಾವತ್((prev=start)<length);
    ಪ್ರತ್ಯಾಗಮ prev;
}

int32_t UnicodeSet::spanBackUTF8(ಧ್ರುವ ವರ್ಣ *s, int32_t length, USetSpanCondition spanCondition) ಧ್ರುವ {
    ಯದಿ(length>0 && bmpSet!=nullptr) {
        ಧ್ರುವ uint8_t* s0 = reinterpret_cast<ಧ್ರುವ uint8_t*>(s);
        ಪ್ರತ್ಯಾಗಮ bmpSet->spanBackUTF8(s0, length, spanCondition);
    }
    ಯದಿ(length<0) {
        length = static_cast<int32_t>(uprv_strlen(s));
    }
    ಯದಿ(length==0) {
        ಪ್ರತ್ಯಾಗಮ 0;
    }
    ಯದಿ(stringSpan!=nullptr) {
        ಪ್ರತ್ಯಾಗಮ stringSpan->spanBackUTF8(reinterpret_cast<ಧ್ರುವ uint8_t*>(s), length, spanCondition);
    } ಅನ್ಯಥಾ ಯದಿ(hasStrings()) {
        uint32_t which= spanCondition==USET_SPAN_NOT_CONTAINED ?
                            UnicodeSetStringSpan::BACK_UTF8_NOT_CONTAINED :
                            UnicodeSetStringSpan::BACK_UTF8_CONTAINED;
        UnicodeSetStringSpan strSpan(*ಏತತ್, *strings_, which);
        ಯದಿ(strSpan.needsStringSpanUTF8()) {
            ಪ್ರತ್ಯಾಗಮ strSpan.spanBackUTF8(reinterpret_cast<ಧ್ರುವ uint8_t*>(s), length, spanCondition);
        }
    }

    ಯದಿ(spanCondition!=USET_SPAN_NOT_CONTAINED) {
        spanCondition=USET_SPAN_CONTAINED;  // Pin to 0/1 values.
    }

    UChar32 c;
    int32_t prev=length;
    ಕುರು {
        U8_PREV_OR_FFFD(s, 0, length, c);
        ಯದಿ(spanCondition!=contains(c)) {
            ಭಙ್ಗ;
        }
    } ಯಾವತ್((prev=length)>0);
    ಪ್ರತ್ಯಾಗಮ prev;
}

U_NAMESPACE_END
