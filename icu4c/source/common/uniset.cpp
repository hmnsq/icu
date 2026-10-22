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

#ଅନ୍ତର୍ଭାଵ "unicode/utypes.h"
#ଅନ୍ତର୍ଭାଵ "unicode/parsepos.h"
#ଅନ୍ତର୍ଭାଵ "unicode/symtable.h"
#ଅନ୍ତର୍ଭାଵ "unicode/uniset.h"
#ଅନ୍ତର୍ଭାଵ "unicode/ustring.h"
#ଅନ୍ତର୍ଭାଵ "unicode/utf8.h"
#ଅନ୍ତର୍ଭାଵ "unicode/utf16.h"
#ଅନ୍ତର୍ଭାଵ "ruleiter.h"
#ଅନ୍ତର୍ଭାଵ "cmemory.h"
#ଅନ୍ତର୍ଭାଵ "cstring.h"
#ଅନ୍ତର୍ଭାଵ "patternprops.h"
#ଅନ୍ତର୍ଭାଵ "uelement.h"
#ଅନ୍ତର୍ଭାଵ "util.h"
#ଅନ୍ତର୍ଭାଵ "uvector.h"
#ଅନ୍ତର୍ଭାଵ "charstr.h"
#ଅନ୍ତର୍ଭାଵ "ustrfmt.h"
#ଅନ୍ତର୍ଭାଵ "uassert.h"
#ଅନ୍ତର୍ଭାଵ "bmpset.h"
#ଅନ୍ତର୍ଭାଵ "unisetspan.h"

// HIGH_VALUE > all valid values. 110000 for codepoints
#define UNICODESET_ଉପର 0x0110000

// LOW <= all valid values. ZERO for codepoints
#define UNICODESET_ତଳ 0x000000

/** Max list [0, 1, 2, ..., max code point, HIGH] */
constexpr int32_t MAX_LENGTH = UNICODESET_ଉପର + 1;

U_NAMESPACE_BEGIN

ପ୍ରତୀକ_Table::~ପ୍ରତୀକ_Table() {}

UOBJECT_DEFINE_RTTI_IMPLEMENTATION(UnicodeSet)

/**
 * Modify the given UChar32 variable so that it is in range, by
 * pinning values < UNICODESET_LOW to UNICODESET_LOW, and
 * pinning values > UNICODESET_HIGH-1 to UNICODESET_HIGH-1.
 * It modifies its argument in-place and also returns it.
 */
ସ୍ଥିର ଧାଡ଼ିଭିତରେ UChar32 pinCodePoint(UChar32& c) {
    ଯଦି (c < UNICODESET_ତଳ) {
        c = UNICODESET_ତଳ;
    } ଅନ୍ୟଥା ଯଦି (c > (UNICODESET_ଉପର-1)) {
        c = (UNICODESET_ଉପର-1);
    }
    ଫେରନ୍ତୁ c;
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
#ଅନ୍ତର୍ଭାଵ <stdio.h>
ସ୍ଥିର int32_t dbg_ଗଣନା = 0;

ସ୍ଥିର ଧାଡ଼ିଭିତରେ ଶୂନ୍ୟ _dbgct(UnicodeSet* set) {
    UnicodeString str;
    set->toPattern(str, ସତ୍ୟ);
    ଅକ୍ଷର buf[40];
    str.extract(0, 39, buf, "");
    printf("DEBUG UnicodeSet: ct 0x%08X; %d %s\n", set, ++dbg_ଗଣନା, buf);
}

ସ୍ଥିର ଧାଡ଼ିଭିତରେ ଶୂନ୍ୟ _dbgdt(UnicodeSet* set) {
    UnicodeString str;
    set->toPattern(str, ସତ୍ୟ);
    ଅକ୍ଷର buf[40];
    str.extract(0, 39, buf, "");
    printf("DEBUG UnicodeSet: dt 0x%08X; %d %s\n", set, --dbg_ଗଣନା, buf);
}

#ଅନ୍ୟଥା

#define _dbgct(set)
#define _dbgdt(set)

#endif

//----------------------------------------------------------------
// UnicodeString in UVector support
//----------------------------------------------------------------

ସ୍ଥିର ଶୂନ୍ୟ U_CALLCONV cloneUnicodeString(UElement *dst, UElement *src) {
    dst->pointer = ନୂତନ UnicodeString(*static_cast<UnicodeString*>(src->pointer));
}

ସ୍ଥିର int32_t U_CALLCONV compareUnicodeString(UElement t1, UElement t2) {
    ଅଚଳ UnicodeString& a = *static_cast<ଅଚଳ UnicodeString*>(t1.pointer);
    ଅଚଳ UnicodeString& b = *static_cast<ଅଚଳ UnicodeString*>(t2.pointer);
    ଫେରନ୍ତୁ a.compare(b);
}

UBool UnicodeSet::hasStrings() ଅଚଳ {
    ଫେରନ୍ତୁ strings_ != nullptr && !strings_->isEmpty();
}

int32_t UnicodeSet::stringsSize() ଅଚଳ {
    ଫେରନ୍ତୁ strings_ == nullptr ? 0 : strings_->size();
}

UBool UnicodeSet::stringsContains(ଅଚଳ UnicodeString &s) ଅଚଳ {
    ଫେରନ୍ତୁ strings_ != nullptr && strings_->contains((ଶୂନ୍ୟ*) &s);
}

//----------------------------------------------------------------
// Constructors &c
//----------------------------------------------------------------

/**
 * Constructs an empty set.
 */
UnicodeSet::UnicodeSet() {
    list[0] = UNICODESET_ଉପର;
    _dbgct(ଏହି);
}

/**
 * Constructs a set containing the given range. If <code>end >
 * start</code> then an empty set is created.
 *
 * @param start first character, inclusive, of range
 * @param end last character, inclusive, of range
 */
UnicodeSet::UnicodeSet(UChar32 start, UChar32 end) {
    list[0] = UNICODESET_ଉପର;
    add(start, end);
    _dbgct(ଏହି);
}

/**
 * Constructs a set that is identical to the given UnicodeSet.
 */
UnicodeSet::UnicodeSet(ଅଚଳ UnicodeSet& o) : UnicodeFilter(o) {
    *ଏହି = o;
    _dbgct(ଏହି);
}

// Copy-construct as thawed.
UnicodeSet::UnicodeSet(ଅଚଳ UnicodeSet& o, UBool /* asThawed */) : UnicodeFilter(o) {
    ଯଦି (ensureCapacity(o.len)) {
        // *this = o except for bmpSet and stringSpan
        len = o.len;
        uprv_memcpy(list, o.list, (size_t)len*ଆକାର(UChar32));
        ଯଦି (o.hasStrings()) {
            UErrorCode status = U_ZERO_ERROR;
            ଯଦି (!allocateStrings(status) ||
                    (strings_->assign(*o.strings_, cloneUnicodeString, status), U_FAILURE(status))) {
                setToBogus();
                ଫେରନ୍ତୁ;
            }
        }
        ଯଦି (o.pat) {
            setPattern(o.pat, o.patLen);
        }
        _dbgct(ଏହି);
    }
}

/**
 * Destructs the set.
 */
UnicodeSet::~UnicodeSet() {
    _dbgdt(ଏହି); // first!
    ଯଦି (list != ଗଦା_List) {
        uprv_free(list);
    }
    delete bmpSet;
    ଯଦି (buffer != ଗଦା_List) {
        uprv_free(buffer);
    }
    delete strings_;
    delete stringSpan;
    releasePattern();
}

/**
 * Assigns this object to be a copy of another.
 */
UnicodeSet& UnicodeSet::operator=(ଅଚଳ UnicodeSet& o) U_LIFETIME_BOUND {
    ଫେରନ୍ତୁ copyFrom(o, ମିଥ୍ୟା);
}

UnicodeSet& UnicodeSet::copyFrom(ଅଚଳ UnicodeSet& o, UBool asThawed) U_LIFETIME_BOUND {
    ଯଦି (ଏହି == &o) {
        ଫେରନ୍ତୁ *ଏହି;
    }
    ଯଦି (isFrozen()) {
        ଫେରନ୍ତୁ *ଏହି;
    }
    ଯଦି (o.isBogus()) {
        setToBogus();
        ଫେରନ୍ତୁ *ଏହି;
    }
    ଯଦି (!ensureCapacity(o.len)) {
        // ensureCapacity will mark the UnicodeSet as Bogus if OOM failure happens.
        ଫେରନ୍ତୁ *ଏହି;
    }
    len = o.len;
    uprv_memcpy(list, o.list, (size_t)len*ଆକାର(UChar32));
    ଯଦି (o.bmpSet != nullptr && !asThawed) {
        bmpSet = ନୂତନ BMPSet(*o.bmpSet, list, len);
        ଯଦି (bmpSet == nullptr) { // Check for memory allocation error.
            setToBogus();
            ଫେରନ୍ତୁ *ଏହି;
        }
    }
    ଯଦି (o.hasStrings()) {
        UErrorCode status = U_ZERO_ERROR;
        ଯଦି ((strings_ == nullptr && !allocateStrings(status)) ||
                (strings_->assign(*o.strings_, cloneUnicodeString, status), U_FAILURE(status))) {
            setToBogus();
            ଫେରନ୍ତୁ *ଏହି;
        }
    } ଅନ୍ୟଥା ଯଦି (hasStrings()) {
        strings_->removeAllElements();
    }
    ଯଦି (o.stringSpan != nullptr && strings_ != nullptr && !asThawed) {
        stringSpan = ନୂତନ UnicodeSetStringSpan(*o.stringSpan, *strings_);
        ଯଦି (stringSpan == nullptr) { // Check for memory allocation error.
            setToBogus();
            ଫେରନ୍ତୁ *ଏହି;
        }
    }
    releasePattern();
    ଯଦି (o.pat) {
        setPattern(o.pat, o.patLen);
    }
    ଫେରନ୍ତୁ *ଏହି;
}

/**
 * Returns a copy of this object.  All UnicodeMatcher objects have
 * to support cloning in order to allow classes using
 * UnicodeMatchers, such as Transliterator, to implement cloning.
 */
UnicodeSet* UnicodeSet::clone() ଅଚଳ {
    ଫେରନ୍ତୁ ନୂତନ UnicodeSet(*ଏହି);
}

UnicodeSet *UnicodeSet::cloneAsThawed() ଅଚଳ {
    ଫେରନ୍ତୁ ନୂତନ UnicodeSet(*ଏହି, ସତ୍ୟ);
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
ତାର୍କିକ UnicodeSet::operator==(ଅଚଳ UnicodeSet& o) ଅଚଳ {
    ଯଦି (len != o.len) ଫେରନ୍ତୁ ମିଥ୍ୟା;
    ପାଇଁ (int32_t i = 0; i < len; ++i) {
        ଯଦି (list[i] != o.list[i]) ଫେରନ୍ତୁ ମିଥ୍ୟା;
    }
    ଯଦି (hasStrings() != o.hasStrings()) { ଫେରନ୍ତୁ ମିଥ୍ୟା; }
    ଯଦି (hasStrings() && *strings_ != *o.strings_) ଫେରନ୍ତୁ ମିଥ୍ୟା;
    ଫେରନ୍ତୁ ସତ୍ୟ;
}

/**
 * Returns the hash code value for this set.
 *
 * @return the hash code value for this set.
 * @see Object#hashCode()
 */
int32_t UnicodeSet::hashCode() ଅଚଳ {
    uint32_t result = static_cast<uint32_t>(len);
    ପାଇଁ (int32_t i = 0; i < len; ++i) {
        result *= 1000003u;
        result += list[i];
    }
    ଫେରନ୍ତୁ static_cast<int32_t>(result);
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
int32_t UnicodeSet::size() ଅଚଳ {
    int32_t n = 0;
    int32_t count = get_Range_ଗଣନା();
    ପାଇଁ (int32_t i = 0; i < count; ++i) {
        n += getRangeEnd(i) - getRangeStart(i) + 1;
    }
    ଫେରନ୍ତୁ n + stringsSize();
}

/**
 * Returns <tt>true</tt> if this set contains no elements.
 *
 * @return <tt>true</tt> if this set contains no elements.
 */
UBool UnicodeSet::isEmpty() ଅଚଳ {
    ଫେରନ୍ତୁ len == 1 && !hasStrings();
}

/**
 * Returns true if this set contains the given character.
 * @param c character to be checked for containment
 * @return true if the test condition is met
 */
UBool UnicodeSet::contains(UChar32 c) ଅଚଳ {
    // Set i to the index of the start item greater than ch
    // We know we will terminate without length test!
    // LATER: for large sets, add binary search
    //int32_t i = -1;
    //for (;;) {
    //    if (c < list[++i]) break;
    //}
    ଯଦି (bmpSet != nullptr) {
        ଫେରନ୍ତୁ bmpSet->contains(c);
    }
    ଯଦି (stringSpan != nullptr) {
        ଫେରନ୍ତୁ stringSpan->contains(c);
    }
    ଯଦି (c >= UNICODESET_ଉପର) { // Don't need to check LOW bound
        ଫେରନ୍ତୁ ମିଥ୍ୟା;
    }
    int32_t i = findCodePoint(c);
    ଫେରନ୍ତୁ i & 1; // return true if odd
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
int32_t UnicodeSet::findCodePoint(UChar32 c) ଅଚଳ {
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
    ଯଦି (c < list[0])
        ଫେରନ୍ତୁ 0;
    // High runner test.  c is often after the last range, so an
    // initial check for this condition pays off.
    int32_t lo = 0;
    int32_t hi = len - 1;
    ଯଦି (lo >= hi || c >= list[hi-1])
        ଫେରନ୍ତୁ hi;
    // invariant: c >= list[lo]
    // invariant: c < list[hi]
    ପାଇଁ (;;) {
        int32_t i = (lo + hi) >> 1;
        ଯଦି (i == lo) {
            ଭାଙ୍ଗ; // Found!
        } ଅନ୍ୟଥା ଯଦି (c < list[i]) {
            hi = i;
        } ଅନ୍ୟଥା {
            lo = i;
        }
    }
    ଫେରନ୍ତୁ hi;
}

/**
 * Returns true if this set contains every character
 * of the given range.
 * @param start first character, inclusive, of the range
 * @param end last character, inclusive, of the range
 * @return true if the test condition is met
 */
UBool UnicodeSet::contains(UChar32 start, UChar32 end) ଅଚଳ {
    //int32_t i = -1;
    //for (;;) {
    //    if (start < list[++i]) break;
    //}
    int32_t i = findCodePoint(start);
    ଫେରନ୍ତୁ ((i & 1) != 0 && end < list[i]);
}

/**
 * Returns <tt>true</tt> if this set contains the given
 * multicharacter string.
 * @param s string to be checked for containment
 * @return <tt>true</tt> if this set contains the specified string
 */
UBool UnicodeSet::contains(ଅଚଳ UnicodeString& s) ଅଚଳ {
    int32_t cp = getSingleCP(s);
    ଯଦି (cp < 0) {
        ଫେରନ୍ତୁ stringsContains(s);
    } ଅନ୍ୟଥା {
        ଫେରନ୍ତୁ contains(static_cast<UChar32>(cp));
    }
}

/**
 * Returns true if this set contains all the characters and strings
 * of the given set.
 * @param c set to be checked for containment
 * @return true if the test condition is met
 */
UBool UnicodeSet::containsAll(ଅଚଳ UnicodeSet& c) ଅଚଳ {
    // The specified set is a subset if all of its pairs are contained in
    // this set.  It's possible to code this more efficiently in terms of
    // direct manipulation of the inversion lists if the need arises.
    int32_t n = c.get_Range_ଗଣନା();
    ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା i=0; i<n; ++i) {
        ଯଦି (!contains(c.getRangeStart(i), c.getRangeEnd(i))) {
            ଫେରନ୍ତୁ ମିଥ୍ୟା;
        }
    }
    ଫେରନ୍ତୁ !c.hasStrings() || (strings_ != nullptr && strings_->containsAll(*c.strings_));
}

/**
 * Returns true if this set contains all the characters
 * of the given string.
 * @param s string containing characters to be checked for containment
 * @return true if the test condition is met
 */
UBool UnicodeSet::containsAll(ଅଚଳ UnicodeString& s) ଅଚଳ {
    ଫେରନ୍ତୁ span(s.getBuffer(), s.length(), USET_SPAN_CONTAINED) == s.length();
}

/**
 * Returns true if this set contains none of the characters
 * of the given range.
 * @param start first character, inclusive, of the range
 * @param end last character, inclusive, of the range
 * @return true if the test condition is met
 */
UBool UnicodeSet::containsNone(UChar32 start, UChar32 end) ଅଚଳ {
    //int32_t i = -1;
    //for (;;) {
    //    if (start < list[++i]) break;
    //}
    int32_t i = findCodePoint(start);
    ଫେରନ୍ତୁ ((i & 1) == 0 && end < list[i]);
}

/**
 * Returns true if this set contains none of the characters and strings
 * of the given set.
 * @param c set to be checked for containment
 * @return true if the test condition is met
 */
UBool UnicodeSet::containsNone(ଅଚଳ UnicodeSet& c) ଅଚଳ {
    // The specified set is a subset if all of its pairs are contained in
    // this set.  It's possible to code this more efficiently in terms of
    // direct manipulation of the inversion lists if the need arises.
    int32_t n = c.get_Range_ଗଣନା();
    ପାଇଁ (int32_t i=0; i<n; ++i) {
        ଯଦି (!containsNone(c.getRangeStart(i), c.getRangeEnd(i))) {
            ଫେରନ୍ତୁ ମିଥ୍ୟା;
        }
    }
    ଫେରନ୍ତୁ strings_ == nullptr || !c.hasStrings() || strings_->containsNone(*c.strings_);
}

/**
 * Returns true if this set contains none of the characters
 * of the given string.
 * @param s string containing characters to be checked for containment
 * @return true if the test condition is met
 */
UBool UnicodeSet::containsNone(ଅଚଳ UnicodeString& s) ଅଚଳ {
    ଫେରନ୍ତୁ span(s.getBuffer(), s.length(), USET_SPAN_NOT_CONTAINED) == s.length();
}

/**
 * Returns <tt>true</tt> if this set contains any character whose low byte
 * is the given value.  This is used by <tt>RuleBasedTransliterator</tt> for
 * indexing.
 */
UBool UnicodeSet::matches_Index_ମୂଲ୍ୟ(uint8_t v) ଅଚଳ {
    /* The index value v, in the range [0,255], is contained in this set if
     * it is contained in any pair of this set.  Pairs either have the high
     * bytes equal, or unequal.  If the high bytes are equal, then we have
     * aaxx..aayy, where aa is the high byte.  Then v is contained if xx <=
     * v <= yy.  If the high bytes are unequal we have aaxx..bbyy, bb>aa.
     * Then v is contained if xx <= v || v <= yy.  (This is identical to the
     * time zone month containment logic.)
     */
    int32_t i;
    int32_t range_ଗଣନା=get_Range_ଗଣନା();
    ପାଇଁ (i=0; i<range_ଗଣନା; ++i) {
        UChar32 ତଳ = getRangeStart(i);
        UChar32 ଉପର = getRangeEnd(i);
        ଯଦି ((ତଳ & ~0xFF) == (ଉପର & ~0xFF)) {
            ଯଦି ((ତଳ & 0xFF) <= v && v <= (ଉପର & 0xFF)) {
                ଫେରନ୍ତୁ ସତ୍ୟ;
            }
        } ଅନ୍ୟଥା ଯଦି ((ତଳ & 0xFF) <= v || v <= (ଉପର & 0xFF)) {
            ଫେରନ୍ତୁ ସତ୍ୟ;
        }
    }
    ଯଦି (hasStrings()) {
        ପାଇଁ (i=0; i<strings_->size(); ++i) {
            ଅଚଳ UnicodeString& s = *static_cast<ଅଚଳ UnicodeString*>(strings_->elementAt(i));
            ଯଦି (s.isEmpty()) {
                ଜାରି;  // skip the empty string
            }
            UChar32 c = s.char32At(0);
            ଯଦି ((c & 0xFF) == v) {
                ଫେରନ୍ତୁ ସତ୍ୟ;
            }
        }
    }
    ଫେରନ୍ତୁ ମିଥ୍ୟା;
}

/**
 * Implementation of UnicodeMatcher::matches().  Always matches the
 * longest possible multichar string.
 */
UMatchDegree UnicodeSet::matches(ଅଚଳ Replaceable& text,
                                 int32_t& offset,
                                 int32_t limit,
                                 UBool incremental) {
    ଯଦି (offset == limit) {
        ଯଦି (contains(U_ETHER)) {
            ଫେରନ୍ତୁ incremental ? U_PARTIAL_MATCH : U_MATCH;
        } ଅନ୍ୟଥା {
            ଫେରନ୍ତୁ U_MISMATCH;
        }
    } ଅନ୍ୟଥା {
        ଯଦି (hasStrings()) { // try strings first

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
            int32_t ଉପର_Water_Length = 0;

            ପାଇଁ (i=0; i<strings_->size(); ++i) {
                ଅଚଳ UnicodeString& trial = *static_cast<ଅଚଳ UnicodeString*>(strings_->elementAt(i));
                ଯଦି (trial.isEmpty()) {
                    ଜାରି;  // skip the empty string
                }

                char16_t c = trial.charAt(forward ? 0 : trial.length() - 1);

                // Strings are sorted, so we can optimize in the
                // forward direction.
                ଯଦି (forward && c > firstChar) ଭାଙ୍ଗ;
                ଯଦି (c != firstChar) ଜାରି;

                int32_t matchLen = matchRest(text, offset, limit, trial);

                ଯଦି (incremental) {
                    int32_t maxLen = forward ? limit-offset : offset-limit;
                    ଯଦି (matchLen == maxLen) {
                        // We have successfully matched but only up to limit.
                        ଫେରନ୍ତୁ U_PARTIAL_MATCH;
                    }
                }

                ଯଦି (matchLen == trial.length()) {
                    // We have successfully matched the whole string.
                    ଯଦି (matchLen > ଉପର_Water_Length) {
                        ଉପର_Water_Length = matchLen;
                    }
                    // In the forward direction we know strings
                    // are sorted so we can bail early.
                    ଯଦି (forward && matchLen < ଉପର_Water_Length) {
                        ଭାଙ୍ଗ;
                    }
                    ଜାରି;
                }
            }

            // We've checked all strings without a partial match.
            // If we have full matches, return the longest one.
            ଯଦି (ଉପର_Water_Length != 0) {
                offset += forward ? ଉପର_Water_Length : -ଉପର_Water_Length;
                ଫେରନ୍ତୁ U_MATCH;
            }
        }
        ଫେରନ୍ତୁ UnicodeFilter::matches(text, offset, limit, incremental);
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
int32_t UnicodeSet::matchRest(ଅଚଳ Replaceable& text,
                              int32_t start, int32_t limit,
                              ଅଚଳ UnicodeString& s) {
    int32_t i;
    int32_t maxLen;
    int32_t slen = s.length();
    ଯଦି (start < limit) {
        maxLen = limit - start;
        ଯଦି (maxLen > slen) maxLen = slen;
        ପାଇଁ (i = 1; i < maxLen; ++i) {
            ଯଦି (text.charAt(start + i) != s.charAt(i)) ଫେରନ୍ତୁ 0;
        }
    } ଅନ୍ୟଥା {
        maxLen = start - limit;
        ଯଦି (maxLen > slen) maxLen = slen;
        --slen; // <=> slen = s.length() - 1;
        ପାଇଁ (i = 1; i < maxLen; ++i) {
            ଯଦି (text.charAt(start - i) != s.charAt(slen - i)) ଫେରନ୍ତୁ 0;
        }
    }
    ଫେରନ୍ତୁ maxLen;
}

/**
 * Implement of UnicodeMatcher
 */
ଶୂନ୍ୟ UnicodeSet::addMatchSetTo(UnicodeSet& toUnionTo) ଅଚଳ {
    toUnionTo.addAll(*ଏହି);
}

/**
 * Returns the index of the given character within this set, where
 * the set is ordered by ascending code point.  If the character
 * is not in this set, return -1.  The inverse of this method is
 * <code>charAt()</code>.
 * @return an index from 0..size()-1, or -1
 */
int32_t UnicodeSet::indexOf(UChar32 c) ଅଚଳ {
    ଯଦି (c < MIN_ମୂଲ୍ୟ || c > MAX_ମୂଲ୍ୟ) {
        ଫେରନ୍ତୁ -1;
    }
    int32_t i = 0;
    int32_t n = 0;
    ପାଇଁ (;;) {
        UChar32 start = list[i++];
        ଯଦି (c < start) {
            ଫେରନ୍ତୁ -1;
        }
        UChar32 limit = list[i++];
        ଯଦି (c < limit) {
            ଫେରନ୍ତୁ n + c - start;
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
UChar32 UnicodeSet::charAt(int32_t index) ଅଚଳ {
    ଯଦି (index >= 0) {
        // len2 is the largest even integer <= len, that is, it is len
        // for even values and len-1 for odd values.  With odd values
        // the last entry is UNICODESET_HIGH.
        int32_t len2 = len & ~1;
        ପାଇଁ (int32_t i=0; i < len2;) {
            UChar32 start = list[i++];
            int32_t count = list[i++] - start;
            ଯଦି (index < count) {
                ଫେରନ୍ତୁ static_cast<UChar32>(start + index);
            }
            index -= count;
        }
    }
    ଫେରନ୍ତୁ static_cast<UChar32>(-1);
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
    ଫେରନ୍ତୁ *ଏହି;
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
    ଯଦି (pinCodePoint(start) < pinCodePoint(end)) {
        UChar32 limit = end + 1;
        // Fast path for adding a new range after the last one.
        // Odd list length: [..., lastStart, lastLimit, HIGH]
        ଯଦି ((len & 1) != 0) {
            // If the list is empty, set lastLimit low enough to not be adjacent to 0.
            UChar32 lastLimit = len == 1 ? -2 : list[len - 2];
            ଯଦି (lastLimit <= start && !isFrozen() && !isBogus()) {
                ଯଦି (lastLimit == start) {
                    // Extend the last range.
                    list[len - 2] = limit;
                    ଯଦି (limit == UNICODESET_ଉପର) {
                        --len;
                    }
                } ଅନ୍ୟଥା {
                    list[len - 1] = start;
                    ଯଦି (limit < UNICODESET_ଉପର) {
                        ଯଦି (ensureCapacity(len + 2)) {
                            list[len++] = limit;
                            list[len++] = UNICODESET_ଉପର;
                        }
                    } ଅନ୍ୟଥା {  // limit == UNICODESET_HIGH
                        ଯଦି (ensureCapacity(len + 1)) {
                            list[len++] = UNICODESET_ଉପର;
                        }
                    }
                }
                releasePattern();
                ଫେରନ୍ତୁ *ଏହି;
            }
        }
        // This is slow. Could be much faster using findCodePoint(start)
        // and modifying the list, dealing with adjacent & overlapping ranges.
        UChar32 range[3] = { start, limit, UNICODESET_ଉପର };
        add(range, 2, 0);
    } ଅନ୍ୟଥା ଯଦି (start == end) {
        add(start);
    }
    ଫେରନ୍ତୁ *ଏହି;
}

// #define DEBUG_US_ADD

#ifdef DEBUG_US_ADD
#ଅନ୍ତର୍ଭାଵ <stdio.h>
ଶୂନ୍ୟ dump(UChar32 c) {
    ଯଦି (c <= 0xFF) {
        printf("%c", (ଅକ୍ଷର)c);
    } ଅନ୍ୟଥା {
        printf("U+%04X", c);
    }
}
ଶୂନ୍ୟ dump(ଅଚଳ UChar32* list, int32_t len) {
    printf("[");
    ପାଇଁ (int32_t i=0; i<len; ++i) {
        ଯଦି (i != 0) printf(", ");
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
    ଯଦି ((i & 1) != 0  || isFrozen() || isBogus()) ଫେରନ୍ତୁ *ଏହି;

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

    ଯଦି (c == list[i]-1) {
        // c is before start of next range
        list[i] = c;
        // if we touched the HIGH mark, then add a new one
        ଯଦି (c == (UNICODESET_ଉପର - 1)) {
            ଯଦି (!ensureCapacity(len+1)) {
                // ensureCapacity will mark the object as Bogus if OOM failure happens.
                ଫେରନ୍ତୁ *ଏହି;
            }
            list[len++] = UNICODESET_ଉପର;
        }
        ଯଦି (i > 0 && c == list[i-1]) {
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
            ଯେତେବେଳେ (src < srclimit) *(dst++) = *(src++);

            len -= 2;
        }
    }

    ଅନ୍ୟଥା ଯଦି (i > 0 && c == list[i-1]) {
        // c is after end of prior range
        list[i-1]++;
        // no need to check for collapse here
    }

    ଅନ୍ୟଥା {
        // At this point we know the new char is not adjacent to
        // any existing ranges, and it is not 10FFFF.


        // [..., start_k-1, limit_k-1, start_k, limit_k, ..., HIGH]
        //                             ^
        //                             list[i]

        // [..., start_k-1, limit_k-1, c, c+1, start_k, limit_k, ..., HIGH]
        //                             ^
        //                             list[i]

        ଯଦି (!ensureCapacity(len+2)) {
            // ensureCapacity will mark the object as Bogus if OOM failure happens.
            ଫେରନ୍ତୁ *ଏହି;
        }

        UChar32 *p = list + i;
        uprv_memmove(p + 2, p, (len - i) * ଆକାର(*p));
        list[i] = c;
        list[i+1] = c+1;
        len += 2;
    }

#ifdef DEBUG_US_ADD
    dump(list, len);
    printf("\n");

    ପାଇଁ (i=1; i<len; ++i) {
        ଯଦି (list[i] <= list[i-1]) {
            // Corrupt array!
            printf("ERROR: list has been corrupted\n");
            exit(1);
        }
    }
#endif

    releasePattern();
    ଫେରନ୍ତୁ *ଏହି;
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
UnicodeSet& UnicodeSet::add(ଅଚଳ UnicodeString& s) U_LIFETIME_BOUND {
    ଯଦି (isFrozen() || isBogus()) ଫେରନ୍ତୁ *ଏହି;
    int32_t cp = getSingleCP(s);
    ଯଦି (cp < 0) {
        ଯଦି (!stringsContains(s)) {
            _add(s);
            releasePattern();
        }
    } ଅନ୍ୟଥା {
        add(static_cast<UChar32>(cp));
    }
    ଫେରନ୍ତୁ *ଏହି;
}

/**
 * Adds the given string, in order, to 'strings_'.  The given string
 * must have been checked by the caller to not already be in 'strings_'.
 */
ଶୂନ୍ୟ UnicodeSet::_add(ଅଚଳ UnicodeString& s) {
    ଯଦି (isFrozen() || isBogus()) {
        ଫେରନ୍ତୁ;
    }
    UErrorCode ec = U_ZERO_ERROR;
    ଯଦି (strings_ == nullptr && !allocateStrings(ec)) {
        setToBogus();
        ଫେରନ୍ତୁ;
    }
    LocalPointer<UnicodeString> t(ନୂତନ UnicodeString(s));
    ଯଦି (t.isNull()) { // Check for memory allocation error.
        setToBogus();
        ଫେରନ୍ତୁ;
    }
    strings_->sorted_ଭର୍ତ୍ତି(t.orphan(), compareUnicodeString, ec);
    ଯଦି (U_FAILURE(ec)) {
        setToBogus();
    }
}

/**
 * @return a code point IF the string consists of a single one.
 * otherwise returns -1.
 * @param string to test
 */
int32_t UnicodeSet::getSingleCP(ଅଚଳ UnicodeString& s) {
    int32_t sLength = s.length();
    ଯଦି (sLength == 1) ଫେରନ୍ତୁ s.charAt(0);
    ଯଦି (sLength == 2) {
        UChar32 cp = s.char32At(0);
        ଯଦି (cp > 0xFFFF) { // is surrogate pair
            ଫେରନ୍ତୁ cp;
        }
    }
    ଫେରନ୍ତୁ -1;
}

/**
 * Adds each of the characters in this string to the set. Thus "ch" => {"c", "h"}
 * If this set already any particular character, it has no effect on that character.
 * @param the source string
 * @return the modified set, for chaining
 */
UnicodeSet& UnicodeSet::addAll(ଅଚଳ UnicodeString& s) U_LIFETIME_BOUND {
    UChar32 cp;
    ପାଇଁ (int32_t i = 0; i < s.length(); i += U16_LENGTH(cp)) {
        cp = s.char32At(i);
        add(cp);
    }
    ଫେରନ୍ତୁ *ଏହି;
}

/**
 * Retains EACH of the characters in this string. Note: "ch" == {"c", "h"}
 * If this set already any particular character, it has no effect on that character.
 * @param the source string
 * @return the modified set, for chaining
 */
UnicodeSet& UnicodeSet::retainAll(ଅଚଳ UnicodeString& s) U_LIFETIME_BOUND {
    UnicodeSet set;
    set.addAll(s);
    retainAll(set);
    ଫେରନ୍ତୁ *ଏହି;
}

/**
 * Complement EACH of the characters in this string. Note: "ch" == {"c", "h"}
 * If this set already any particular character, it has no effect on that character.
 * @param the source string
 * @return the modified set, for chaining
 */
UnicodeSet& UnicodeSet::complementAll(ଅଚଳ UnicodeString& s) U_LIFETIME_BOUND {
    UnicodeSet set;
    set.addAll(s);
    complementAll(set);
    ଫେରନ୍ତୁ *ଏହି;
}

/**
 * Remove EACH of the characters in this string. Note: "ch" == {"c", "h"}
 * If this set already any particular character, it has no effect on that character.
 * @param the source string
 * @return the modified set, for chaining
 */
UnicodeSet& UnicodeSet::removeAll(ଅଚଳ UnicodeString& s) U_LIFETIME_BOUND {
    UnicodeSet set;
    set.addAll(s);
    removeAll(set);
    ଫେରନ୍ତୁ *ଏହି;
}

UnicodeSet& UnicodeSet::removeAllStrings() U_LIFETIME_BOUND {
    ଯଦି (!isFrozen() && hasStrings()) {
        strings_->removeAllElements();
        releasePattern();
    }
    ଫେରନ୍ତୁ *ଏହି;
}


/**
 * Makes a set from a multicharacter string. Thus "ch" => {"ch"}
 * <br><b>Warning: you cannot add an empty string ("") to a UnicodeSet.</b>
 * @param the source string
 * @return a newly created set containing the given string
 */
UnicodeSet* U_EXPORT2 UnicodeSet::createFrom(ଅଚଳ UnicodeString& s) {
    UnicodeSet *set = ନୂତନ UnicodeSet();
    ଯଦି (set != nullptr) { // Check for memory allocation error.
        set->add(s);
    }
    ଫେରନ୍ତୁ set;
}


/**
 * Makes a set from each of the characters in the string. Thus "ch" => {"c", "h"}
 * @param the source string
 * @return a newly created set containing the given characters
 */
UnicodeSet* U_EXPORT2 UnicodeSet::createFromAll(ଅଚଳ UnicodeString& s) {
    UnicodeSet *set = ନୂତନ UnicodeSet();
    ଯଦି (set != nullptr) { // Check for memory allocation error.
        set->addAll(s);
    }
    ଫେରନ୍ତୁ set;
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
    ଯଦି (pinCodePoint(start) <= pinCodePoint(end)) {
        UChar32 range[3] = { start, end+1, UNICODESET_ଉପର };
        retain(range, 2, 0);
    } ଅନ୍ୟଥା {
        clear();
    }
    ଫେରନ୍ତୁ *ଏହି;
}

UnicodeSet& UnicodeSet::retain(UChar32 c) U_LIFETIME_BOUND {
    ଫେରନ୍ତୁ retain(c, c);
}

UnicodeSet& UnicodeSet::retain(ଅଚଳ UnicodeString &s) U_LIFETIME_BOUND {
    ଯଦି (isFrozen() || isBogus()) { ଫେରନ୍ତୁ *ଏହି; }
    UChar32 cp = getSingleCP(s);
    ଯଦି (cp < 0) {
        ତାର୍କିକ isIn = stringsContains(s);
        // Check for getRangeCount() first to avoid somewhat-expensive size()
        // when there are single code points.
        ଯଦି (isIn && get_Range_ଗଣନା() == 0 && size() == 1) {
            ଫେରନ୍ତୁ *ଏହି;
        }
        clear();
        ଯଦି (isIn) {
            _add(s);
        }
    } ଅନ୍ୟଥା {
        retain(cp, cp);
    }
    ଫେରନ୍ତୁ *ଏହି;
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
    ଯଦି (pinCodePoint(start) <= pinCodePoint(end)) {
        UChar32 range[3] = { start, end+1, UNICODESET_ଉପର };
        retain(range, 2, 2);
    }
    ଫେରନ୍ତୁ *ଏହି;
}

/**
 * Removes the specified character from this set if it is present.
 * The set will not contain the specified range once the call
 * returns.
 */
UnicodeSet& UnicodeSet::remove(UChar32 c) U_LIFETIME_BOUND {
    ଫେରନ୍ତୁ remove(c, c);
}

/**
 * Removes the specified string from this set if it is present.
 * The set will not contain the specified character once the call
 * returns.
 * @param the source string
 * @return the modified set, for chaining
 */
UnicodeSet& UnicodeSet::remove(ଅଚଳ UnicodeString& s) U_LIFETIME_BOUND {
    ଯଦି (isFrozen() || isBogus()) ଫେରନ୍ତୁ *ଏହି;
    int32_t cp = getSingleCP(s);
    ଯଦି (cp < 0) {
        ଯଦି (strings_ != nullptr && strings_->removeElement((ଶୂନ୍ୟ*) &s)) {
            releasePattern();
        }
    } ଅନ୍ୟଥା {
        remove(static_cast<UChar32>(cp), static_cast<UChar32>(cp));
    }
    ଫେରନ୍ତୁ *ଏହି;
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
    ଯଦି (isFrozen() || isBogus()) {
        ଫେରନ୍ତୁ *ଏହି;
    }
    ଯଦି (pinCodePoint(start) <= pinCodePoint(end)) {
        UChar32 range[3] = { start, end+1, UNICODESET_ଉପର };
        exclusiveOr(range, 2, 0);
    }
    releasePattern();
    ଫେରନ୍ତୁ *ଏହି;
}

UnicodeSet& UnicodeSet::complement(UChar32 c) U_LIFETIME_BOUND {
    ଫେରନ୍ତୁ complement(c, c);
}

/**
 * This is equivalent to
 * <code>complement(MIN_VALUE, MAX_VALUE)</code>.
 */
UnicodeSet& UnicodeSet::complement() U_LIFETIME_BOUND {
    ଯଦି (isFrozen() || isBogus()) {
        ଫେରନ୍ତୁ *ଏହି;
    }
    ଯଦି (list[0] == UNICODESET_ତଳ) {
        uprv_memmove(list, list + 1, (size_t)(len-1)*ଆକାର(UChar32));
        --len;
    } ଅନ୍ୟଥା {
        ଯଦି (!ensureCapacity(len+1)) {
            ଫେରନ୍ତୁ *ଏହି;
        }
        uprv_memmove(list + 1, list, (size_t)len*ଆକାର(UChar32));
        list[0] = UNICODESET_ତଳ;
        ++len;
    }
    releasePattern();
    ଫେରନ୍ତୁ *ଏହି;
}

/**
 * Complement the specified string in this set.
 * The set will not contain the specified string once the call
 * returns.
 *
 * @param s the string to complement
 * @return this object, for chaining
 */
UnicodeSet& UnicodeSet::complement(ଅଚଳ UnicodeString& s) U_LIFETIME_BOUND {
    ଯଦି (isFrozen() || isBogus()) ଫେରନ୍ତୁ *ଏହି;
    int32_t cp = getSingleCP(s);
    ଯଦି (cp < 0) {
        ଯଦି (stringsContains(s)) {
            strings_->removeElement((ଶୂନ୍ୟ*) &s);
        } ଅନ୍ୟଥା {
            _add(s);
        }
        releasePattern();
    } ଅନ୍ୟଥା {
        complement(static_cast<UChar32>(cp), static_cast<UChar32>(cp));
    }
    ଫେରନ୍ତୁ *ଏହି;
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
UnicodeSet& UnicodeSet::addAll(ଅଚଳ UnicodeSet& c) U_LIFETIME_BOUND {
    ଯଦି ( c.len>0 && c.list!=nullptr ) {
        add(c.list, c.len, 0);
    }

    // Add strings in order
    ଯଦି ( c.strings_!=nullptr ) {
        ପାଇଁ (int32_t i=0; i<c.strings_->size(); ++i) {
            ଅଚଳ UnicodeString* s = static_cast<ଅଚଳ UnicodeString*>(c.strings_->elementAt(i));
            ଯଦି (!stringsContains(*s)) {
                _add(*s);
            }
        }
    }
    ଫେରନ୍ତୁ *ଏହି;
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
UnicodeSet& UnicodeSet::retainAll(ଅଚଳ UnicodeSet& c) U_LIFETIME_BOUND {
    ଯଦି (isFrozen() || isBogus()) {
        ଫେରନ୍ତୁ *ଏହି;
    }
    retain(c.list, c.len, 0);
    ଯଦି (hasStrings()) {
        ଯଦି (!c.hasStrings()) {
            strings_->removeAllElements();
        } ଅନ୍ୟଥା {
            strings_->retainAll(*c.strings_);
        }
    }
    ଫେରନ୍ତୁ *ଏହି;
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
UnicodeSet& UnicodeSet::removeAll(ଅଚଳ UnicodeSet& c) U_LIFETIME_BOUND {
    ଯଦି (isFrozen() || isBogus()) {
        ଫେରନ୍ତୁ *ଏହି;
    }
    retain(c.list, c.len, 2);
    ଯଦି (hasStrings() && c.hasStrings()) {
        strings_->removeAll(*c.strings_);
    }
    ଫେରନ୍ତୁ *ଏହି;
}

/**
 * Complements in this set all elements contained in the specified
 * set.  Any character in the other set will be removed if it is
 * in this set, or will be added if it is not in this set.
 *
 * @param c set that defines which elements will be xor'ed from
 *          this set.
 */
UnicodeSet& UnicodeSet::complementAll(ଅଚଳ UnicodeSet& c) U_LIFETIME_BOUND {
    ଯଦି (isFrozen() || isBogus()) {
        ଫେରନ୍ତୁ *ଏହି;
    }
    exclusiveOr(c.list, c.len, 0);

    ଯଦି (c.strings_ != nullptr) {
        ପାଇଁ (int32_t i=0; i<c.strings_->size(); ++i) {
            ଶୂନ୍ୟ* e = c.strings_->elementAt(i);
            ଯଦି (strings_ == nullptr || !strings_->removeElement(e)) {
                _add(*static_cast<ଅଚଳ UnicodeString*>(e));
            }
        }
    }
    ଫେରନ୍ତୁ *ଏହି;
}

/**
 * Removes all of the elements from this set.  This set will be
 * empty after this call returns.
 */
UnicodeSet& UnicodeSet::clear() U_LIFETIME_BOUND {
    ଯଦି (isFrozen()) {
        ଫେରନ୍ତୁ *ଏହି;
    }
    list[0] = UNICODESET_ଉପର;
    len = 1;
    releasePattern();
    ଯଦି (strings_ != nullptr) {
        strings_->removeAllElements();
    }
    // Remove bogus
    fFlags = 0;
    ଫେରନ୍ତୁ *ଏହି;
}

/**
 * Iteration method that returns the number of ranges contained in
 * this set.
 * @see #getRangeStart
 * @see #getRangeEnd
 */
int32_t UnicodeSet::get_Range_ଗଣନା() ଅଚଳ {
    ଫେରନ୍ତୁ len/2;
}

/**
 * Iteration method that returns the first character in the
 * specified range of this set.
 * @see #getRangeCount
 * @see #getRangeEnd
 */
UChar32 UnicodeSet::getRangeStart(int32_t index) ଅଚଳ {
    ଫେରନ୍ତୁ list[index*2];
}

/**
 * Iteration method that returns the last character in the
 * specified range of this set.
 * @see #getRangeStart
 * @see #getRangeEnd
 */
UChar32 UnicodeSet::getRangeEnd(int32_t index) ଅଚଳ {
    ଫେରନ୍ତୁ list[index*2 + 1] - 1;
}

ଅଚଳ UnicodeString* UnicodeSet::getString(int32_t index) ଅଚଳ U_LIFETIME_BOUND {
    ଫେରନ୍ତୁ static_cast<ଅଚଳ UnicodeString*>(strings_->elementAt(index));
}

/**
 * Reallocate this objects internal structures to take up the least
 * possible space, without changing this object's value.
 */
UnicodeSet& UnicodeSet::compact() U_LIFETIME_BOUND {
    ଯଦି (isFrozen() || isBogus()) {
        ଫେରନ୍ତୁ *ଏହି;
    }
    // Delete buffer first to defragment memory less.
    ଯଦି (buffer != ଗଦା_List) {
        uprv_free(buffer);
        buffer = nullptr;
        bufferCapacity = 0;
    }
    ଯଦି (list == ଗଦା_List) {
        // pass
    } ଅନ୍ୟଥା ଯଦି (len <= INITIAL_CAPACITY) {
        uprv_memcpy(ଗଦା_List, list, len * ଆକାର(UChar32));
        uprv_free(list);
        list = ଗଦା_List;
        capacity = INITIAL_CAPACITY;
    } ଅନ୍ୟଥା ଯଦି ((len + 7) < capacity) {
        // If we have more than a little unused capacity, shrink it to len.
        UChar32* temp = static_cast<UChar32*>(uprv_realloc(list, ଆକାର(UChar32) * len));
        ଯଦି (temp) {
            list = temp;
            capacity = len;
        }
        // else what the heck happened?! We allocated less memory!
        // Oh well. We'll keep our original array.
    }
    ଯଦି (strings_ != nullptr && strings_->isEmpty()) {
        delete strings_;
        strings_ = nullptr;
    }
    ଫେରନ୍ତୁ *ଏହି;
}

#ifdef DEBUG_SERIALIZE
#ଅନ୍ତର୍ଭାଵ <stdio.h>
#endif

/**
 * Deserialize constructor.
 */
UnicodeSet::UnicodeSet(ଅଚଳ uint16_t data[], int32_t dataLen, ESerialization serialization,
                       UErrorCode &ec) {

  ଯଦି(U_FAILURE(ec)) {
    setToBogus();
    ଫେରନ୍ତୁ;
  }

  ଯଦି( (serialization != kSerialized)
      || (data==nullptr)
      || (dataLen < 1)) {
    ec = U_ILLEGAL_ARGUMENT_ERROR;
    setToBogus();
    ଫେରନ୍ତୁ;
  }

  // bmp?
  int32_t headerSize = ((data[0]&0x8000)) ?2:1;
  int32_t bmpLength = (headerSize==1)?data[0]:data[1];

  int32_t newLength = (((data[0]&0x7FFF)-bmpLength)/2)+bmpLength;
#ifdef DEBUG_SERIALIZE
  printf("dataLen %d headerSize %d bmpLen %d len %d. data[0]=%X/%X/%X/%X\n", dataLen,headerSize,bmpLength,newLength, data[0],data[1],data[2],data[3]);
#endif
  ଯଦି(!ensureCapacity(newLength + 1)) {  // +1 for HIGH
    ଫେରନ୍ତୁ;
  }
  // copy bmp
  int32_t i;
  ପାଇଁ(i = 0; i< bmpLength;i++) {
    list[i] = data[i+headerSize];
#ifdef DEBUG_SERIALIZE
    printf("<<16@%d[%d] %X\n", i+headerSize, i, list[i]);
#endif
  }
  // copy smp
  ପାଇଁ(i=bmpLength;i<newLength;i++) {
    list[i] = (static_cast<UChar32>(data[headerSize + bmpLength + (i - bmpLength) * 2 + 0]) << 16) +
               static_cast<UChar32>(data[headerSize + bmpLength + (i - bmpLength) * 2 + 1]);
#ifdef DEBUG_SERIALIZE
    printf("<<32@%d+[%d] %lX\n", headerSize+bmpLength+i, i, list[i]);
#endif
  }
  U_ASSERT(i == newLength);
  ଯଦି (i == 0 || list[i - 1] != UNICODESET_ଉପର) {
    list[i++] = UNICODESET_ଉପର;
  }
  len = i;
}


int32_t UnicodeSet::serialize(uint16_t *dest, int32_t destCapacity, UErrorCode& ec) ଅଚଳ {
    int32_t bmpLength, length, destLength;

    ଯଦି (U_FAILURE(ec)) {
        ଫେରନ୍ତୁ 0;
    }

    ଯଦି (destCapacity<0 || (destCapacity>0 && dest==nullptr)) {
        ec=U_ILLEGAL_ARGUMENT_ERROR;
        ଫେରନ୍ତୁ 0;
    }

    /* count necessary 16-bit units */
    length=ଏହି->len-1; // Subtract 1 to ignore final UNICODESET_HIGH
    // assert(length>=0);
    ଯଦି (length==0) {
        /* empty set */
        ଯଦି (destCapacity>0) {
            *dest=0;
        } ଅନ୍ୟଥା {
            ec=U_BUFFER_OVERFLOW_ERROR;
        }
        ଫେରନ୍ତୁ 1;
    }
    /* now length>0 */

    ଯଦି (ଏହି->list[length-1]<=0xffff) {
        /* all BMP */
        bmpLength=length;
    } ଅନ୍ୟଥା ଯଦି (ଏହି->list[0]>=0x10000) {
        /* all supplementary */
        bmpLength=0;
        length*=2;
    } ଅନ୍ୟଥା {
        /* some BMP, some supplementary */
        ପାଇଁ (bmpLength=0; bmpLength<length && ଏହି->list[bmpLength]<=0xffff; ++bmpLength) {}
        length=bmpLength+2*(length-bmpLength);
    }
#ifdef DEBUG_SERIALIZE
    printf(">> bmpLength%d length%d len%d\n", bmpLength, length, len);
#endif
    /* length: number of 16-bit array units */
    ଯଦି (length>0x7fff) {
        /* there are only 15 bits for the length in the first serialized word */
        ec=U_INDEX_OUTOFBOUNDS_ERROR;
        ଫେରନ୍ତୁ 0;
    }

    /*
     * total serialized length:
     * number of 16-bit array units (length) +
     * 1 length unit (always) +
     * 1 bmpLength unit (if there are supplementary values)
     */
    destLength=length+((length>bmpLength)?2:1);
    ଯଦି (destLength<=destCapacity) {
        ଅଚଳ UChar32 *p;
        int32_t i;

#ifdef DEBUG_SERIALIZE
        printf("writeHdr\n");
#endif
        *dest = static_cast<uint16_t>(length);
        ଯଦି (length>bmpLength) {
            *dest|=0x8000;
            *++dest = static_cast<uint16_t>(bmpLength);
        }
        ++dest;

        /* write the BMP part of the array */
        p=ଏହି->list;
        ପାଇଁ (i=0; i<bmpLength; ++i) {
#ifdef DEBUG_SERIALIZE
          printf("writebmp: %x\n", (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)*p);
#endif
            *dest++ = static_cast<uint16_t>(*p++);
        }

        /* write the supplementary part of the array */
        ପାଇଁ (; i<length; i+=2) {
#ifdef DEBUG_SERIALIZE
          printf("write32: %x\n", (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)*p);
#endif
            *dest++ = static_cast<uint16_t>(*p >> 16);
            *dest++ = static_cast<uint16_t>(*p++);
        }
    } ଅନ୍ୟଥା {
        ec=U_BUFFER_OVERFLOW_ERROR;
    }
    ଫେରନ୍ତୁ destLength;
}

//----------------------------------------------------------------
// Implementation: Utility methods
//----------------------------------------------------------------

/**
 * Allocate our strings vector and return true if successful.
 */
UBool UnicodeSet::allocateStrings(UErrorCode &status) {
    ଯଦି (U_FAILURE(status)) {
        ଫେରନ୍ତୁ ମିଥ୍ୟା;
    }
    strings_ = ନୂତନ UVector(uprv_deleteUObject,
                          uhash_compareUnicodeString, 1, status);
    ଯଦି (strings_ == nullptr) { // Check for memory allocation error.
        status = U_MEMORY_ALLOCATION_ERROR;
        ଫେରନ୍ତୁ ମିଥ୍ୟା;
    }
    ଯଦି (U_FAILURE(status)) {
        delete strings_;
        strings_ = nullptr;
        ଫେରନ୍ତୁ ମିଥ୍ୟା;
    } 
    ଫେରନ୍ତୁ ସତ୍ୟ;
}

int32_t UnicodeSet::nextCapacity(int32_t minCapacity) {
    // Grow exponentially to reduce the frequency of allocations.
    ଯଦି (minCapacity < INITIAL_CAPACITY) {
        ଫେରନ୍ତୁ minCapacity + INITIAL_CAPACITY;
    } ଅନ୍ୟଥା ଯଦି (minCapacity <= 2500) {
        ଫେରନ୍ତୁ 5 * minCapacity;
    } ଅନ୍ୟଥା {
        int32_t newCapacity = 2 * minCapacity;
        ଯଦି (newCapacity > MAX_LENGTH) {
            newCapacity = MAX_LENGTH;
        }
        ଫେରନ୍ତୁ newCapacity;
    }
}

ତାର୍କିକ UnicodeSet::ensureCapacity(int32_t newLen) {
    ଯଦି (newLen > MAX_LENGTH) {
        newLen = MAX_LENGTH;
    }
    ଯଦି (newLen <= capacity) {
        ଫେରନ୍ତୁ ସତ୍ୟ;
    }
    int32_t newCapacity = nextCapacity(newLen);
    UChar32* temp = static_cast<UChar32*>(uprv_malloc(newCapacity * ଆକାର(UChar32)));
    ଯଦି (temp == nullptr) {
        setToBogus(); // set the object to bogus state if an OOM failure occurred.
        ଫେରନ୍ତୁ ମିଥ୍ୟା;
    }
    // Copy only the actual contents.
    uprv_memcpy(temp, list, len * ଆକାର(UChar32));
    ଯଦି (list != ଗଦା_List) {
        uprv_free(list);
    }
    list = temp;
    capacity = newCapacity;
    ଫେରନ୍ତୁ ସତ୍ୟ;
}

ତାର୍କିକ UnicodeSet::ensureBufferCapacity(int32_t newLen) {
    ଯଦି (newLen > MAX_LENGTH) {
        newLen = MAX_LENGTH;
    }
    ଯଦି (newLen <= bufferCapacity) {
        ଫେରନ୍ତୁ ସତ୍ୟ;
    }
    int32_t newCapacity = nextCapacity(newLen);
    UChar32* temp = static_cast<UChar32*>(uprv_malloc(newCapacity * ଆକାର(UChar32)));
    ଯଦି (temp == nullptr) {
        setToBogus();
        ଫେରନ୍ତୁ ମିଥ୍ୟା;
    }
    // The buffer has no contents to be copied.
    // It is always filled from scratch after this call.
    ଯଦି (buffer != ଗଦା_List) {
        uprv_free(buffer);
    }
    buffer = temp;
    bufferCapacity = newCapacity;
    ଫେରନ୍ତୁ ସତ୍ୟ;
}

/**
 * Swap list and buffer.
 */
ଶୂନ୍ୟ UnicodeSet::swapBuffers() {
    // swap list and buffer
    UChar32* temp = list;
    list = buffer;
    buffer = temp;

    int32_t c = capacity;
    capacity = bufferCapacity;
    bufferCapacity = c;
}

ଶୂନ୍ୟ UnicodeSet::setToBogus() {
    clear(); // Remove everything in the set.
    fFlags = kIsBogus;
}

//----------------------------------------------------------------
// Implementation: Fundamental operators
//----------------------------------------------------------------

ସ୍ଥିର ଧାଡ଼ିଭିତରେ UChar32 max(UChar32 a, UChar32 b) {
    ଫେରନ୍ତୁ (a > b) ? a : b;
}

// polarity = 0, 3 is normal: x xor y
// polarity = 1, 2: x xor ~y == x === y

ଶୂନ୍ୟ UnicodeSet::exclusiveOr(ଅଚଳ UChar32* other, int32_t otherLen, int8_t polarity) {
    ଯଦି (isFrozen() || isBogus()) {
        ଫେରନ୍ତୁ;
    }
    ଯଦି (!ensureBufferCapacity(len + otherLen)) {
        ଫେରନ୍ତୁ;
    }

    int32_t i = 0, j = 0, k = 0;
    UChar32 a = list[i++];
    UChar32 b;
    ଯଦି (polarity == 1 || polarity == 2) {
        b = UNICODESET_ତଳ;
        ଯଦି (other[j] == UNICODESET_ତଳ) { // skip base if already LOW
            ++j;
            b = other[j];
        }
    } ଅନ୍ୟଥା {
        b = other[j++];
    }
    // simplest of all the routines
    // sort the values, discarding identicals!
    ପାଇଁ (;;) {
        ଯଦି (a < b) {
            buffer[k++] = a;
            a = list[i++];
        } ଅନ୍ୟଥା ଯଦି (b < a) {
            buffer[k++] = b;
            b = other[j++];
        } ଅନ୍ୟଥା ଯଦି (a != UNICODESET_ଉପର) { // at this point, a == b
            // discard both values!
            a = list[i++];
            b = other[j++];
        } ଅନ୍ୟଥା { // DONE!
            buffer[k++] = UNICODESET_ଉପର;
            len = k;
            ଭାଙ୍ଗ;
        }
    }
    swapBuffers();
    releasePattern();
}

// polarity = 0 is normal: x union y
// polarity = 2: x union ~y
// polarity = 1: ~x union y
// polarity = 3: ~x union ~y

ଶୂନ୍ୟ UnicodeSet::add(ଅଚଳ UChar32* other, int32_t otherLen, int8_t polarity) {
    ଯଦି (isFrozen() || isBogus() || other==nullptr) {
        ଫେରନ୍ତୁ;
    }
    ଯଦି (!ensureBufferCapacity(len + otherLen)) {
        ଫେରନ୍ତୁ;
    }

    int32_t i = 0, j = 0, k = 0;
    UChar32 a = list[i++];
    UChar32 b = other[j++];
    // change from xor is that we have to check overlapping pairs
    // polarity bit 1 means a is second, bit 2 means b is.
    ପାଇଁ (;;) {
        ବାଛ (polarity) {
          ସ୍ଥିତି 0: // both first; take lower if unequal
            ଯଦି (a < b) { // take a
                // Back up over overlapping ranges in buffer[]
                ଯଦି (k > 0 && a <= buffer[k-1]) {
                    // Pick latter end value in buffer[] vs. list[]
                    a = max(list[i], buffer[--k]);
                } ଅନ୍ୟଥା {
                    // No overlap
                    buffer[k++] = a;
                    a = list[i];
                }
                i++; // Common if/else code factored out
                polarity ^= 1;
            } ଅନ୍ୟଥା ଯଦି (b < a) { // take b
                ଯଦି (k > 0 && b <= buffer[k-1]) {
                    b = max(other[j], buffer[--k]);
                } ଅନ୍ୟଥା {
                    buffer[k++] = b;
                    b = other[j];
                }
                j++;
                polarity ^= 2;
            } ଅନ୍ୟଥା { // a == b, take a, drop b
                ଯଦି (a == UNICODESET_ଉପର) ଯାଅ loop_end;
                // This is symmetrical; it doesn't matter if
                // we backtrack with a or b. - liu
                ଯଦି (k > 0 && a <= buffer[k-1]) {
                    a = max(list[i], buffer[--k]);
                } ଅନ୍ୟଥା {
                    // No overlap
                    buffer[k++] = a;
                    a = list[i];
                }
                i++;
                polarity ^= 1;
                b = other[j++];
                polarity ^= 2;
            }
            ଭାଙ୍ଗ;
          ସ୍ଥିତି 3: // both second; take higher if unequal, and drop other
            ଯଦି (b <= a) { // take a
                ଯଦି (a == UNICODESET_ଉପର) ଯାଅ loop_end;
                buffer[k++] = a;
            } ଅନ୍ୟଥା { // take b
                ଯଦି (b == UNICODESET_ଉପର) ଯାଅ loop_end;
                buffer[k++] = b;
            }
            a = list[i++];
            polarity ^= 1;   // factored common code
            b = other[j++];
            polarity ^= 2;
            ଭାଙ୍ଗ;
          ସ୍ଥିତି 1: // a second, b first; if b < a, overlap
            ଯଦି (a < b) { // no overlap, take a
                buffer[k++] = a; a = list[i++]; polarity ^= 1;
            } ଅନ୍ୟଥା ଯଦି (b < a) { // OVERLAP, drop b
                b = other[j++];
                polarity ^= 2;
            } ଅନ୍ୟଥା { // a == b, drop both!
                ଯଦି (a == UNICODESET_ଉପର) ଯାଅ loop_end;
                a = list[i++];
                polarity ^= 1;
                b = other[j++];
                polarity ^= 2;
            }
            ଭାଙ୍ଗ;
          ସ୍ଥିତି 2: // a first, b second; if a < b, overlap
            ଯଦି (b < a) { // no overlap, take b
                buffer[k++] = b;
                b = other[j++];
                polarity ^= 2;
            } ଅନ୍ୟଥା  ଯଦି (a < b) { // OVERLAP, drop a
                a = list[i++];
                polarity ^= 1;
            } ଅନ୍ୟଥା { // a == b, drop both!
                ଯଦି (a == UNICODESET_ଉପର) ଯାଅ loop_end;
                a = list[i++];
                polarity ^= 1;
                b = other[j++];
                polarity ^= 2;
            }
            ଭାଙ୍ଗ;
        }
    }
 loop_end:
    buffer[k++] = UNICODESET_ଉପର;    // terminate
    len = k;
    swapBuffers();
    releasePattern();
}

// polarity = 0 is normal: x intersect y
// polarity = 2: x intersect ~y == set-minus
// polarity = 1: ~x intersect y
// polarity = 3: ~x intersect ~y

ଶୂନ୍ୟ UnicodeSet::retain(ଅଚଳ UChar32* other, int32_t otherLen, int8_t polarity) {
    ଯଦି (isFrozen() || isBogus()) {
        ଫେରନ୍ତୁ;
    }
    ଯଦି (!ensureBufferCapacity(len + otherLen)) {
        ଫେରନ୍ତୁ;
    }

    int32_t i = 0, j = 0, k = 0;
    UChar32 a = list[i++];
    UChar32 b = other[j++];
    // change from xor is that we have to check overlapping pairs
    // polarity bit 1 means a is second, bit 2 means b is.
    ପାଇଁ (;;) {
        ବାଛ (polarity) {
          ସ୍ଥିତି 0: // both first; drop the smaller
            ଯଦି (a < b) { // drop a
                a = list[i++];
                polarity ^= 1;
            } ଅନ୍ୟଥା ଯଦି (b < a) { // drop b
                b = other[j++];
                polarity ^= 2;
            } ଅନ୍ୟଥା { // a == b, take one, drop other
                ଯଦି (a == UNICODESET_ଉପର) ଯାଅ loop_end;
                buffer[k++] = a;
                a = list[i++];
                polarity ^= 1;
                b = other[j++];
                polarity ^= 2;
            }
            ଭାଙ୍ଗ;
          ସ୍ଥିତି 3: // both second; take lower if unequal
            ଯଦି (a < b) { // take a
                buffer[k++] = a;
                a = list[i++];
                polarity ^= 1;
            } ଅନ୍ୟଥା ଯଦି (b < a) { // take b
                buffer[k++] = b;
                b = other[j++];
                polarity ^= 2;
            } ଅନ୍ୟଥା { // a == b, take one, drop other
                ଯଦି (a == UNICODESET_ଉପର) ଯାଅ loop_end;
                buffer[k++] = a;
                a = list[i++];
                polarity ^= 1;
                b = other[j++];
                polarity ^= 2;
            }
            ଭାଙ୍ଗ;
          ସ୍ଥିତି 1: // a second, b first;
            ଯଦି (a < b) { // NO OVERLAP, drop a
                a = list[i++];
                polarity ^= 1;
            } ଅନ୍ୟଥା ଯଦି (b < a) { // OVERLAP, take b
                buffer[k++] = b;
                b = other[j++];
                polarity ^= 2;
            } ଅନ୍ୟଥା { // a == b, drop both!
                ଯଦି (a == UNICODESET_ଉପର) ଯାଅ loop_end;
                a = list[i++];
                polarity ^= 1;
                b = other[j++];
                polarity ^= 2;
            }
            ଭାଙ୍ଗ;
          ସ୍ଥିତି 2: // a first, b second; if a < b, overlap
            ଯଦି (b < a) { // no overlap, drop b
                b = other[j++];
                polarity ^= 2;
            } ଅନ୍ୟଥା  ଯଦି (a < b) { // OVERLAP, take a
                buffer[k++] = a;
                a = list[i++];
                polarity ^= 1;
            } ଅନ୍ୟଥା { // a == b, drop both!
                ଯଦି (a == UNICODESET_ଉପର) ଯାଅ loop_end;
                a = list[i++];
                polarity ^= 1;
                b = other[j++];
                polarity ^= 2;
            }
            ଭାଙ୍ଗ;
        }
    }
 loop_end:
    buffer[k++] = UNICODESET_ଉପର;    // terminate
    len = k;
    swapBuffers();
    releasePattern();
}

/**
 * Append the <code>toPattern()</code> representation of a
 * string to the given <code>StringBuffer</code>.
 */
ଶୂନ୍ୟ UnicodeSet::_appendToPat(UnicodeString& buf, ଅଚଳ UnicodeString& s, UBool escapeUnprintable) {
    UChar32 cp;
    ପାଇଁ (int32_t i = 0; i < s.length(); i += U16_LENGTH(cp)) {
        _appendToPat(buf, cp = s.char32At(i), escapeUnprintable);
    }
}

/**
 * Append the <code>toPattern()</code> representation of a
 * character to the given <code>StringBuffer</code>.
 */
ଶୂନ୍ୟ UnicodeSet::_appendToPat(UnicodeString& buf, UChar32 c, UBool escapeUnprintable) {
    ଯଦି (escapeUnprintable ? ICU_Utility::isUnprintable(c) : ICU_Utility::shouldAlwaysBeEscaped(c)) {
        // Use hex escape notation (\uxxxx or \Uxxxxxxxx) for anything
        // unprintable
        ICU_Utility::escape(buf, c);
        ଫେରନ୍ତୁ;
    }
    // Okay to let ':' pass through
    ବାଛ (c) {
    ସ୍ଥିତି u'[':
    ସ୍ଥିତି u']':
    ସ୍ଥିତି u'-':
    ସ୍ଥିତି u'^':
    ସ୍ଥିତି u'&':
    ସ୍ଥିତି u'\\':
    ସ୍ଥିତି u'{':
    ସ୍ଥିତି u'}':
    ସ୍ଥିତି u':':
    ସ୍ଥିତି ପ୍ରତୀକ_Table::ପ୍ରତୀକ_REF:
        buf.append(u'\\');
        ଭାଙ୍ଗ;
    ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
        // Escape whitespace
        ଯଦି (PatternProps::isWhiteSpace(c)) {
            buf.append(u'\\');
        }
        ଭାଙ୍ଗ;
    }
    buf.append(c);
}

ଶୂନ୍ୟ UnicodeSet::_appendToPat(UnicodeString &result, UChar32 start, UChar32 end,
                              UBool escapeUnprintable) {
    _appendToPat(result, start, escapeUnprintable);
    ଯଦି (start != end) {
        ଯଦି ((start+1) != end ||
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
                                      UBool escapeUnprintable) ଅଚଳ
{
    ଯଦି (pat != nullptr) {
        int32_t i;
        int32_t backslash_ଗଣନା = 0;
        ପାଇଁ (i=0; i<patLen; ) {
            UChar32 c;
            U16_NEXT(pat, i, patLen, c);
            ଯଦି (escapeUnprintable ?
                    ICU_Utility::isUnprintable(c) : ICU_Utility::shouldAlwaysBeEscaped(c)) {
                // If the unprintable character is preceded by an odd
                // number of backslashes, then it has been escaped.
                // Before unescaping it, we delete the final
                // backslash.
                ଯଦି ((backslash_ଗଣନା % 2) == 1) {
                    result.truncate(result.length() - 1);
                }
                ICU_Utility::escape(result, c);
                backslash_ଗଣନା = 0;
            } ଅନ୍ୟଥା {
                result.append(c);
                ଯଦି (c == u'\\') {
                    ++backslash_ଗଣନା;
                } ଅନ୍ୟଥା {
                    backslash_ଗଣନା = 0;
                }
            }
        }
        ଫେରନ୍ତୁ result;
    }

    ଫେରନ୍ତୁ _generatePattern(result, escapeUnprintable);
}

/**
 * Returns a string representation of this set.  If the result of
 * calling this function is passed to a UnicodeSet constructor, it
 * will produce another set that is equal to this one.
 */
UnicodeString& UnicodeSet::toPattern(UnicodeString& result U_LIFETIME_BOUND,
                                     UBool escapeUnprintable) ଅଚଳ
{
    result.truncate(0);
    ଫେରନ୍ତୁ _toPattern(result, escapeUnprintable);
}

/**
 * Generate and append a string representation of this set to result.
 * This does not use this.pat, the cleaned up copy of the string
 * passed to applyPattern().
 */
UnicodeString& UnicodeSet::_generatePattern(UnicodeString& result U_LIFETIME_BOUND,
                                            UBool escapeUnprintable) ଅଚଳ
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
    ଯଦି (len >= 4 && list[0] == 0 && limit == len && !hasStrings()) {
        // Emit the inverse
        result.append(u'^');
        // Offsetting the inversion list index by one lets us
        // iterate over the ranges of the set complement.
        i = 1;
        --limit;
    }

    // Emit the ranges as pairs.
    ଯେତେବେଳେ (i < limit) {
        UChar32 start = list[i];  // getRangeStart()
        UChar32 end = list[i + 1] - 1;  // getRangeEnd() = range limit minus one
        ଯଦି (!(0xd800 <= end && end <= 0xdbff)) {
            _appendToPat(result, start, end, escapeUnprintable);
            i += 2;
        } ଅନ୍ୟଥା {
            // The range ends with a lead surrogate.
            // Avoid writing what looks like a lead+trail surrogate pair.
            // 1. Postpone ranges that start with a lead surrogate code point.
            int32_t firstLead = i;
            ଯେତେବେଳେ ((i += 2) < limit && list[i] <= 0xdbff) {}
            int32_t firstAfterLead = i;
            // 2. Write following ranges that start with a trail surrogate code point.
            ଯେତେବେଳେ (i < limit && (start = list[i]) <= 0xdfff) {
                _appendToPat(result, start, list[i + 1] - 1, escapeUnprintable);
                i += 2;
            }
            // 3. Now write the postponed ranges.
            ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା j = firstLead; j < firstAfterLead; j += 2) {
                _appendToPat(result, list[j], list[j + 1] - 1, escapeUnprintable);
            }
        }
    }

    ଯଦି (strings_ != nullptr) {
        ପାଇଁ (int32_t i = 0; i<strings_->size(); ++i) {
            result.append(u'{');
            _appendToPat(result,
                         *static_cast<ଅଚଳ UnicodeString*>(strings_->elementAt(i)),
                         escapeUnprintable);
            result.append(u'}');
        }
    }
    ଫେରନ୍ତୁ result.append(u']');
}

/**
* Release existing cached pattern
*/
ଶୂନ୍ୟ UnicodeSet::releasePattern() {
    ଯଦି (pat) {
        uprv_free(pat);
        pat = nullptr;
        patLen = 0;
    }
}

/**
* Set the new pattern to cache.
*/
ଶୂନ୍ୟ UnicodeSet::setPattern(ଅଚଳ char16_t *newPat, int32_t newPatLen) {
    releasePattern();
    pat = static_cast<char16_t*>(uprv_malloc((newPatLen + 1) * ଆକାର(char16_t)));
    ଯଦି (pat) {
        patLen = newPatLen;
        u_memcpy(pat, newPat, patLen);
        pat[patLen] = 0;
    }
    // else we don't care if malloc failed. This was just a nice cache.
    // We can regenerate an equivalent pattern later when requested.
}

UnicodeSet* UnicodeSet::freeze() U_LIFETIME_BOUND {
    ଯଦି(!isFrozen() && !isBogus()) {
        compact();

        // Optimize contains() and span() and similar functions.
        ଯଦି (hasStrings()) {
            stringSpan = ନୂତନ UnicodeSetStringSpan(*ଏହି, *strings_, UnicodeSetStringSpan::ALL);
            ଯଦି (stringSpan == nullptr) {
                setToBogus();
                ଫେରନ୍ତୁ ଏହି;
            } ଅନ୍ୟଥା ଯଦି (!stringSpan->needsStringSpanUTF16()) {
                // All strings are irrelevant for span() etc. because
                // all of each string's code points are contained in this set.
                // Do not check needsStringSpanUTF8() because UTF-8 has at most as
                // many relevant strings as UTF-16.
                // (Thus needsStringSpanUTF8() implies needsStringSpanUTF16().)
                delete stringSpan;
                stringSpan = nullptr;
            }
        }
        ଯଦି (stringSpan == nullptr) {
            // No span-relevant strings: Optimize for code point spans.
            bmpSet=ନୂତନ BMPSet(list, len);
            ଯଦି (bmpSet == nullptr) { // Check for memory allocation error.
                setToBogus();
            }
        }
    }
    ଫେରନ୍ତୁ ଏହି;
}

int32_t UnicodeSet::span(ଅଚଳ char16_t *s, int32_t length, USetSpanCondition spanCondition) ଅଚଳ {
    ଯଦି(length>0 && bmpSet!=nullptr) {
        ଫେରନ୍ତୁ static_cast<int32_t>(bmpSet->span(s, s + length, spanCondition) - s);
    }
    ଯଦି(length<0) {
        length=u_strlen(s);
    }
    ଯଦି(length==0) {
        ଫେରନ୍ତୁ 0;
    }
    ଯଦି(stringSpan!=nullptr) {
        ଫେରନ୍ତୁ stringSpan->span(s, length, spanCondition);
    } ଅନ୍ୟଥା ଯଦି(hasStrings()) {
        uint32_t which= spanCondition==USET_SPAN_NOT_CONTAINED ?
                            UnicodeSetStringSpan::FWD_UTF16_NOT_CONTAINED :
                            UnicodeSetStringSpan::FWD_UTF16_CONTAINED;
        UnicodeSetStringSpan strSpan(*ଏହି, *strings_, which);
        ଯଦି(strSpan.needsStringSpanUTF16()) {
            ଫେରନ୍ତୁ strSpan.span(s, length, spanCondition);
        }
    }

    ଯଦି(spanCondition!=USET_SPAN_NOT_CONTAINED) {
        spanCondition=USET_SPAN_CONTAINED;  // Pin to 0/1 values.
    }

    UChar32 c;
    int32_t start=0, prev=0;
    କର {
        U16_NEXT(s, start, length, c);
        ଯଦି(spanCondition!=contains(c)) {
            ଭାଙ୍ଗ;
        }
    } ଯେତେବେଳେ((prev=start)<length);
    ଫେରନ୍ତୁ prev;
}

int32_t UnicodeSet::spanBack(ଅଚଳ char16_t *s, int32_t length, USetSpanCondition spanCondition) ଅଚଳ {
    ଯଦି(length>0 && bmpSet!=nullptr) {
        ଫେରନ୍ତୁ static_cast<int32_t>(bmpSet->spanBack(s, s + length, spanCondition) - s);
    }
    ଯଦି(length<0) {
        length=u_strlen(s);
    }
    ଯଦି(length==0) {
        ଫେରନ୍ତୁ 0;
    }
    ଯଦି(stringSpan!=nullptr) {
        ଫେରନ୍ତୁ stringSpan->spanBack(s, length, spanCondition);
    } ଅନ୍ୟଥା ଯଦି(hasStrings()) {
        uint32_t which= spanCondition==USET_SPAN_NOT_CONTAINED ?
                            UnicodeSetStringSpan::BACK_UTF16_NOT_CONTAINED :
                            UnicodeSetStringSpan::BACK_UTF16_CONTAINED;
        UnicodeSetStringSpan strSpan(*ଏହି, *strings_, which);
        ଯଦି(strSpan.needsStringSpanUTF16()) {
            ଫେରନ୍ତୁ strSpan.spanBack(s, length, spanCondition);
        }
    }

    ଯଦି(spanCondition!=USET_SPAN_NOT_CONTAINED) {
        spanCondition=USET_SPAN_CONTAINED;  // Pin to 0/1 values.
    }

    UChar32 c;
    int32_t prev=length;
    କର {
        U16_PREV(s, 0, length, c);
        ଯଦି(spanCondition!=contains(c)) {
            ଭାଙ୍ଗ;
        }
    } ଯେତେବେଳେ((prev=length)>0);
    ଫେରନ୍ତୁ prev;
}

int32_t UnicodeSet::spanUTF8(ଅଚଳ ଅକ୍ଷର *s, int32_t length, USetSpanCondition spanCondition) ଅଚଳ {
    ଯଦି(length>0 && bmpSet!=nullptr) {
        ଅଚଳ uint8_t* s0 = reinterpret_cast<ଅଚଳ uint8_t*>(s);
        ଫେରନ୍ତୁ static_cast<int32_t>(bmpSet->spanUTF8(s0, length, spanCondition) - s0);
    }
    ଯଦି(length<0) {
        length = static_cast<int32_t>(uprv_strlen(s));
    }
    ଯଦି(length==0) {
        ଫେରନ୍ତୁ 0;
    }
    ଯଦି(stringSpan!=nullptr) {
        ଫେରନ୍ତୁ stringSpan->spanUTF8(reinterpret_cast<ଅଚଳ uint8_t*>(s), length, spanCondition);
    } ଅନ୍ୟଥା ଯଦି(hasStrings()) {
        uint32_t which= spanCondition==USET_SPAN_NOT_CONTAINED ?
                            UnicodeSetStringSpan::FWD_UTF8_NOT_CONTAINED :
                            UnicodeSetStringSpan::FWD_UTF8_CONTAINED;
        UnicodeSetStringSpan strSpan(*ଏହି, *strings_, which);
        ଯଦି(strSpan.needsStringSpanUTF8()) {
            ଫେରନ୍ତୁ strSpan.spanUTF8(reinterpret_cast<ଅଚଳ uint8_t*>(s), length, spanCondition);
        }
    }

    ଯଦି(spanCondition!=USET_SPAN_NOT_CONTAINED) {
        spanCondition=USET_SPAN_CONTAINED;  // Pin to 0/1 values.
    }

    UChar32 c;
    int32_t start=0, prev=0;
    କର {
        U8_NEXT_OR_FFFD(s, start, length, c);
        ଯଦି(spanCondition!=contains(c)) {
            ଭାଙ୍ଗ;
        }
    } ଯେତେବେଳେ((prev=start)<length);
    ଫେରନ୍ତୁ prev;
}

int32_t UnicodeSet::spanBackUTF8(ଅଚଳ ଅକ୍ଷର *s, int32_t length, USetSpanCondition spanCondition) ଅଚଳ {
    ଯଦି(length>0 && bmpSet!=nullptr) {
        ଅଚଳ uint8_t* s0 = reinterpret_cast<ଅଚଳ uint8_t*>(s);
        ଫେରନ୍ତୁ bmpSet->spanBackUTF8(s0, length, spanCondition);
    }
    ଯଦି(length<0) {
        length = static_cast<int32_t>(uprv_strlen(s));
    }
    ଯଦି(length==0) {
        ଫେରନ୍ତୁ 0;
    }
    ଯଦି(stringSpan!=nullptr) {
        ଫେରନ୍ତୁ stringSpan->spanBackUTF8(reinterpret_cast<ଅଚଳ uint8_t*>(s), length, spanCondition);
    } ଅନ୍ୟଥା ଯଦି(hasStrings()) {
        uint32_t which= spanCondition==USET_SPAN_NOT_CONTAINED ?
                            UnicodeSetStringSpan::BACK_UTF8_NOT_CONTAINED :
                            UnicodeSetStringSpan::BACK_UTF8_CONTAINED;
        UnicodeSetStringSpan strSpan(*ଏହି, *strings_, which);
        ଯଦି(strSpan.needsStringSpanUTF8()) {
            ଫେରନ୍ତୁ strSpan.spanBackUTF8(reinterpret_cast<ଅଚଳ uint8_t*>(s), length, spanCondition);
        }
    }

    ଯଦି(spanCondition!=USET_SPAN_NOT_CONTAINED) {
        spanCondition=USET_SPAN_CONTAINED;  // Pin to 0/1 values.
    }

    UChar32 c;
    int32_t prev=length;
    କର {
        U8_PREV_OR_FFFD(s, 0, length, c);
        ଯଦି(spanCondition!=contains(c)) {
            ଭାଙ୍ଗ;
        }
    } ଯେତେବେଳେ((prev=length)>0);
    ଫେରନ୍ତୁ prev;
}

U_NAMESPACE_END
