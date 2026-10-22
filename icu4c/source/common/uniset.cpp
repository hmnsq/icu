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

#അന്തര്ഭാവ "unicode/utypes.h"
#അന്തര്ഭാവ "unicode/parsepos.h"
#അന്തര്ഭാവ "unicode/symtable.h"
#അന്തര്ഭാവ "unicode/uniset.h"
#അന്തര്ഭാവ "unicode/ustring.h"
#അന്തര്ഭാവ "unicode/utf8.h"
#അന്തര്ഭാവ "unicode/utf16.h"
#അന്തര്ഭാവ "ruleiter.h"
#അന്തര്ഭാവ "cmemory.h"
#അന്തര്ഭാവ "cstring.h"
#അന്തര്ഭാവ "patternprops.h"
#അന്തര്ഭാവ "uelement.h"
#അന്തര്ഭാവ "util.h"
#അന്തര്ഭാവ "uvector.h"
#അന്തര്ഭാവ "charstr.h"
#അന്തര്ഭാവ "ustrfmt.h"
#അന്തര്ഭാവ "uassert.h"
#അന്തര്ഭാവ "bmpset.h"
#അന്തര്ഭാവ "unisetspan.h"

// HIGH_VALUE > all valid values. 110000 for codepoints
#define UNICODESET_മുകളിലെ 0x0110000

// LOW <= all valid values. ZERO for codepoints
#define UNICODESET_താഴെയുള്ള 0x000000

/** Max list [0, 1, 2, ..., max code point, HIGH] */
constexpr int32_t MAX_LENGTH = UNICODESET_മുകളിലെ + 1;

U_NAMESPACE_BEGIN

ചിഹ്നം_Table::~ചിഹ്നം_Table() {}

UOBJECT_DEFINE_RTTI_IMPLEMENTATION(UnicodeSet)

/**
 * Modify the given UChar32 variable so that it is in range, by
 * pinning values < UNICODESET_LOW to UNICODESET_LOW, and
 * pinning values > UNICODESET_HIGH-1 to UNICODESET_HIGH-1.
 * It modifies its argument in-place and also returns it.
 */
സ്ഥിതം വരിയിൽ UChar32 pinCodePoint(UChar32& c) {
    എങ്കിൽ (c < UNICODESET_താഴെയുള്ള) {
        c = UNICODESET_താഴെയുള്ള;
    } അല്ലെങ്കിൽ എങ്കിൽ (c > (UNICODESET_മുകളിലെ-1)) {
        c = (UNICODESET_മുകളിലെ-1);
    }
    തിരികെ c;
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
#അന്തര്ഭാവ <stdio.h>
സ്ഥിതം int32_t dbg_എണ്ണം = 0;

സ്ഥിതം വരിയിൽ ശൂന്യം _dbgct(UnicodeSet* set) {
    UnicodeString str;
    set->toPattern(str, സത്യം);
    അക്ഷരം buf[40];
    str.extract(0, 39, buf, "");
    printf("DEBUG UnicodeSet: ct 0x%08X; %d %s\n", set, ++dbg_എണ്ണം, buf);
}

സ്ഥിതം വരിയിൽ ശൂന്യം _dbgdt(UnicodeSet* set) {
    UnicodeString str;
    set->toPattern(str, സത്യം);
    അക്ഷരം buf[40];
    str.extract(0, 39, buf, "");
    printf("DEBUG UnicodeSet: dt 0x%08X; %d %s\n", set, --dbg_എണ്ണം, buf);
}

#അല്ലെങ്കിൽ

#define _dbgct(set)
#define _dbgdt(set)

#endif

//----------------------------------------------------------------
// UnicodeString in UVector support
//----------------------------------------------------------------

സ്ഥിതം ശൂന്യം U_CALLCONV cloneUnicodeString(UElement *dst, UElement *src) {
    dst->pointer = പുതിയ UnicodeString(*static_cast<UnicodeString*>(src->pointer));
}

സ്ഥിതം int32_t U_CALLCONV compareUnicodeString(UElement t1, UElement t2) {
    സ്ഥിരം UnicodeString& a = *static_cast<സ്ഥിരം UnicodeString*>(t1.pointer);
    സ്ഥിരം UnicodeString& b = *static_cast<സ്ഥിരം UnicodeString*>(t2.pointer);
    തിരികെ a.compare(b);
}

UBool UnicodeSet::hasStrings() സ്ഥിരം {
    തിരികെ strings_ != nullptr && !strings_->isEmpty();
}

int32_t UnicodeSet::stringsSize() സ്ഥിരം {
    തിരികെ strings_ == nullptr ? 0 : strings_->size();
}

UBool UnicodeSet::stringsContains(സ്ഥിരം UnicodeString &s) സ്ഥിരം {
    തിരികെ strings_ != nullptr && strings_->contains((ശൂന്യം*) &s);
}

//----------------------------------------------------------------
// Constructors &c
//----------------------------------------------------------------

/**
 * Constructs an empty set.
 */
UnicodeSet::UnicodeSet() {
    list[0] = UNICODESET_മുകളിലെ;
    _dbgct(ഇത്);
}

/**
 * Constructs a set containing the given range. If <code>end >
 * start</code> then an empty set is created.
 *
 * @param start first character, inclusive, of range
 * @param end last character, inclusive, of range
 */
UnicodeSet::UnicodeSet(UChar32 start, UChar32 end) {
    list[0] = UNICODESET_മുകളിലെ;
    add(start, end);
    _dbgct(ഇത്);
}

/**
 * Constructs a set that is identical to the given UnicodeSet.
 */
UnicodeSet::UnicodeSet(സ്ഥിരം UnicodeSet& o) : UnicodeFilter(o) {
    *ഇത് = o;
    _dbgct(ഇത്);
}

// Copy-construct as thawed.
UnicodeSet::UnicodeSet(സ്ഥിരം UnicodeSet& o, UBool /* asThawed */) : UnicodeFilter(o) {
    എങ്കിൽ (ensureCapacity(o.len)) {
        // *this = o except for bmpSet and stringSpan
        len = o.len;
        uprv_memcpy(list, o.list, (size_t)len*വലുപ്പം(UChar32));
        എങ്കിൽ (o.hasStrings()) {
            UErrorCode status = U_ZERO_ERROR;
            എങ്കിൽ (!allocateStrings(status) ||
                    (strings_->assign(*o.strings_, cloneUnicodeString, status), U_FAILURE(status))) {
                setToBogus();
                തിരികെ;
            }
        }
        എങ്കിൽ (o.pat) {
            setPattern(o.pat, o.patLen);
        }
        _dbgct(ഇത്);
    }
}

/**
 * Destructs the set.
 */
UnicodeSet::~UnicodeSet() {
    _dbgdt(ഇത്); // first!
    എങ്കിൽ (list != അട്ടി_List) {
        uprv_free(list);
    }
    delete bmpSet;
    എങ്കിൽ (buffer != അട്ടി_List) {
        uprv_free(buffer);
    }
    delete strings_;
    delete stringSpan;
    releasePattern();
}

/**
 * Assigns this object to be a copy of another.
 */
UnicodeSet& UnicodeSet::operator=(സ്ഥിരം UnicodeSet& o) U_LIFETIME_BOUND {
    തിരികെ copyFrom(o, അസത്യം);
}

UnicodeSet& UnicodeSet::copyFrom(സ്ഥിരം UnicodeSet& o, UBool asThawed) U_LIFETIME_BOUND {
    എങ്കിൽ (ഇത് == &o) {
        തിരികെ *ഇത്;
    }
    എങ്കിൽ (isFrozen()) {
        തിരികെ *ഇത്;
    }
    എങ്കിൽ (o.isBogus()) {
        setToBogus();
        തിരികെ *ഇത്;
    }
    എങ്കിൽ (!ensureCapacity(o.len)) {
        // ensureCapacity will mark the UnicodeSet as Bogus if OOM failure happens.
        തിരികെ *ഇത്;
    }
    len = o.len;
    uprv_memcpy(list, o.list, (size_t)len*വലുപ്പം(UChar32));
    എങ്കിൽ (o.bmpSet != nullptr && !asThawed) {
        bmpSet = പുതിയ BMPSet(*o.bmpSet, list, len);
        എങ്കിൽ (bmpSet == nullptr) { // Check for memory allocation error.
            setToBogus();
            തിരികെ *ഇത്;
        }
    }
    എങ്കിൽ (o.hasStrings()) {
        UErrorCode status = U_ZERO_ERROR;
        എങ്കിൽ ((strings_ == nullptr && !allocateStrings(status)) ||
                (strings_->assign(*o.strings_, cloneUnicodeString, status), U_FAILURE(status))) {
            setToBogus();
            തിരികെ *ഇത്;
        }
    } അല്ലെങ്കിൽ എങ്കിൽ (hasStrings()) {
        strings_->removeAllElements();
    }
    എങ്കിൽ (o.stringSpan != nullptr && strings_ != nullptr && !asThawed) {
        stringSpan = പുതിയ UnicodeSetStringSpan(*o.stringSpan, *strings_);
        എങ്കിൽ (stringSpan == nullptr) { // Check for memory allocation error.
            setToBogus();
            തിരികെ *ഇത്;
        }
    }
    releasePattern();
    എങ്കിൽ (o.pat) {
        setPattern(o.pat, o.patLen);
    }
    തിരികെ *ഇത്;
}

/**
 * Returns a copy of this object.  All UnicodeMatcher objects have
 * to support cloning in order to allow classes using
 * UnicodeMatchers, such as Transliterator, to implement cloning.
 */
UnicodeSet* UnicodeSet::clone() സ്ഥിരം {
    തിരികെ പുതിയ UnicodeSet(*ഇത്);
}

UnicodeSet *UnicodeSet::cloneAsThawed() സ്ഥിരം {
    തിരികെ പുതിയ UnicodeSet(*ഇത്, സത്യം);
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
തർക്കം UnicodeSet::operator==(സ്ഥിരം UnicodeSet& o) സ്ഥിരം {
    എങ്കിൽ (len != o.len) തിരികെ അസത്യം;
    വേണ്ടി (int32_t i = 0; i < len; ++i) {
        എങ്കിൽ (list[i] != o.list[i]) തിരികെ അസത്യം;
    }
    എങ്കിൽ (hasStrings() != o.hasStrings()) { തിരികെ അസത്യം; }
    എങ്കിൽ (hasStrings() && *strings_ != *o.strings_) തിരികെ അസത്യം;
    തിരികെ സത്യം;
}

/**
 * Returns the hash code value for this set.
 *
 * @return the hash code value for this set.
 * @see Object#hashCode()
 */
int32_t UnicodeSet::hashCode() സ്ഥിരം {
    uint32_t result = static_cast<uint32_t>(len);
    വേണ്ടി (int32_t i = 0; i < len; ++i) {
        result *= 1000003u;
        result += list[i];
    }
    തിരികെ static_cast<int32_t>(result);
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
int32_t UnicodeSet::size() സ്ഥിരം {
    int32_t n = 0;
    int32_t count = get_Range_എണ്ണം();
    വേണ്ടി (int32_t i = 0; i < count; ++i) {
        n += getRangeEnd(i) - getRangeStart(i) + 1;
    }
    തിരികെ n + stringsSize();
}

/**
 * Returns <tt>true</tt> if this set contains no elements.
 *
 * @return <tt>true</tt> if this set contains no elements.
 */
UBool UnicodeSet::isEmpty() സ്ഥിരം {
    തിരികെ len == 1 && !hasStrings();
}

/**
 * Returns true if this set contains the given character.
 * @param c character to be checked for containment
 * @return true if the test condition is met
 */
UBool UnicodeSet::contains(UChar32 c) സ്ഥിരം {
    // Set i to the index of the start item greater than ch
    // We know we will terminate without length test!
    // LATER: for large sets, add binary search
    //int32_t i = -1;
    //for (;;) {
    //    if (c < list[++i]) break;
    //}
    എങ്കിൽ (bmpSet != nullptr) {
        തിരികെ bmpSet->contains(c);
    }
    എങ്കിൽ (stringSpan != nullptr) {
        തിരികെ stringSpan->contains(c);
    }
    എങ്കിൽ (c >= UNICODESET_മുകളിലെ) { // Don't need to check LOW bound
        തിരികെ അസത്യം;
    }
    int32_t i = findCodePoint(c);
    തിരികെ i & 1; // return true if odd
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
int32_t UnicodeSet::findCodePoint(UChar32 c) സ്ഥിരം {
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
    എങ്കിൽ (c < list[0])
        തിരികെ 0;
    // High runner test.  c is often after the last range, so an
    // initial check for this condition pays off.
    int32_t lo = 0;
    int32_t hi = len - 1;
    എങ്കിൽ (lo >= hi || c >= list[hi-1])
        തിരികെ hi;
    // invariant: c >= list[lo]
    // invariant: c < list[hi]
    വേണ്ടി (;;) {
        int32_t i = (lo + hi) >> 1;
        എങ്കിൽ (i == lo) {
            നിർത്തുക; // Found!
        } അല്ലെങ്കിൽ എങ്കിൽ (c < list[i]) {
            hi = i;
        } അല്ലെങ്കിൽ {
            lo = i;
        }
    }
    തിരികെ hi;
}

/**
 * Returns true if this set contains every character
 * of the given range.
 * @param start first character, inclusive, of the range
 * @param end last character, inclusive, of the range
 * @return true if the test condition is met
 */
UBool UnicodeSet::contains(UChar32 start, UChar32 end) സ്ഥിരം {
    //int32_t i = -1;
    //for (;;) {
    //    if (start < list[++i]) break;
    //}
    int32_t i = findCodePoint(start);
    തിരികെ ((i & 1) != 0 && end < list[i]);
}

/**
 * Returns <tt>true</tt> if this set contains the given
 * multicharacter string.
 * @param s string to be checked for containment
 * @return <tt>true</tt> if this set contains the specified string
 */
UBool UnicodeSet::contains(സ്ഥിരം UnicodeString& s) സ്ഥിരം {
    int32_t cp = getSingleCP(s);
    എങ്കിൽ (cp < 0) {
        തിരികെ stringsContains(s);
    } അല്ലെങ്കിൽ {
        തിരികെ contains(static_cast<UChar32>(cp));
    }
}

/**
 * Returns true if this set contains all the characters and strings
 * of the given set.
 * @param c set to be checked for containment
 * @return true if the test condition is met
 */
UBool UnicodeSet::containsAll(സ്ഥിരം UnicodeSet& c) സ്ഥിരം {
    // The specified set is a subset if all of its pairs are contained in
    // this set.  It's possible to code this more efficiently in terms of
    // direct manipulation of the inversion lists if the need arises.
    int32_t n = c.get_Range_എണ്ണം();
    വേണ്ടി (പൂർണ്ണസംഖ്യ i=0; i<n; ++i) {
        എങ്കിൽ (!contains(c.getRangeStart(i), c.getRangeEnd(i))) {
            തിരികെ അസത്യം;
        }
    }
    തിരികെ !c.hasStrings() || (strings_ != nullptr && strings_->containsAll(*c.strings_));
}

/**
 * Returns true if this set contains all the characters
 * of the given string.
 * @param s string containing characters to be checked for containment
 * @return true if the test condition is met
 */
UBool UnicodeSet::containsAll(സ്ഥിരം UnicodeString& s) സ്ഥിരം {
    തിരികെ span(s.getBuffer(), s.length(), USET_SPAN_CONTAINED) == s.length();
}

/**
 * Returns true if this set contains none of the characters
 * of the given range.
 * @param start first character, inclusive, of the range
 * @param end last character, inclusive, of the range
 * @return true if the test condition is met
 */
UBool UnicodeSet::containsNone(UChar32 start, UChar32 end) സ്ഥിരം {
    //int32_t i = -1;
    //for (;;) {
    //    if (start < list[++i]) break;
    //}
    int32_t i = findCodePoint(start);
    തിരികെ ((i & 1) == 0 && end < list[i]);
}

/**
 * Returns true if this set contains none of the characters and strings
 * of the given set.
 * @param c set to be checked for containment
 * @return true if the test condition is met
 */
UBool UnicodeSet::containsNone(സ്ഥിരം UnicodeSet& c) സ്ഥിരം {
    // The specified set is a subset if all of its pairs are contained in
    // this set.  It's possible to code this more efficiently in terms of
    // direct manipulation of the inversion lists if the need arises.
    int32_t n = c.get_Range_എണ്ണം();
    വേണ്ടി (int32_t i=0; i<n; ++i) {
        എങ്കിൽ (!containsNone(c.getRangeStart(i), c.getRangeEnd(i))) {
            തിരികെ അസത്യം;
        }
    }
    തിരികെ strings_ == nullptr || !c.hasStrings() || strings_->containsNone(*c.strings_);
}

/**
 * Returns true if this set contains none of the characters
 * of the given string.
 * @param s string containing characters to be checked for containment
 * @return true if the test condition is met
 */
UBool UnicodeSet::containsNone(സ്ഥിരം UnicodeString& s) സ്ഥിരം {
    തിരികെ span(s.getBuffer(), s.length(), USET_SPAN_NOT_CONTAINED) == s.length();
}

/**
 * Returns <tt>true</tt> if this set contains any character whose low byte
 * is the given value.  This is used by <tt>RuleBasedTransliterator</tt> for
 * indexing.
 */
UBool UnicodeSet::matches_Index_മൂല്യം(uint8_t v) സ്ഥിരം {
    /* The index value v, in the range [0,255], is contained in this set if
     * it is contained in any pair of this set.  Pairs either have the high
     * bytes equal, or unequal.  If the high bytes are equal, then we have
     * aaxx..aayy, where aa is the high byte.  Then v is contained if xx <=
     * v <= yy.  If the high bytes are unequal we have aaxx..bbyy, bb>aa.
     * Then v is contained if xx <= v || v <= yy.  (This is identical to the
     * time zone month containment logic.)
     */
    int32_t i;
    int32_t range_എണ്ണം=get_Range_എണ്ണം();
    വേണ്ടി (i=0; i<range_എണ്ണം; ++i) {
        UChar32 താഴെയുള്ള = getRangeStart(i);
        UChar32 മുകളിലെ = getRangeEnd(i);
        എങ്കിൽ ((താഴെയുള്ള & ~0xFF) == (മുകളിലെ & ~0xFF)) {
            എങ്കിൽ ((താഴെയുള്ള & 0xFF) <= v && v <= (മുകളിലെ & 0xFF)) {
                തിരികെ സത്യം;
            }
        } അല്ലെങ്കിൽ എങ്കിൽ ((താഴെയുള്ള & 0xFF) <= v || v <= (മുകളിലെ & 0xFF)) {
            തിരികെ സത്യം;
        }
    }
    എങ്കിൽ (hasStrings()) {
        വേണ്ടി (i=0; i<strings_->size(); ++i) {
            സ്ഥിരം UnicodeString& s = *static_cast<സ്ഥിരം UnicodeString*>(strings_->elementAt(i));
            എങ്കിൽ (s.isEmpty()) {
                തുടരുക;  // skip the empty string
            }
            UChar32 c = s.char32At(0);
            എങ്കിൽ ((c & 0xFF) == v) {
                തിരികെ സത്യം;
            }
        }
    }
    തിരികെ അസത്യം;
}

/**
 * Implementation of UnicodeMatcher::matches().  Always matches the
 * longest possible multichar string.
 */
UMatchDegree UnicodeSet::matches(സ്ഥിരം Replaceable& text,
                                 int32_t& offset,
                                 int32_t limit,
                                 UBool incremental) {
    എങ്കിൽ (offset == limit) {
        എങ്കിൽ (contains(U_ETHER)) {
            തിരികെ incremental ? U_PARTIAL_MATCH : U_MATCH;
        } അല്ലെങ്കിൽ {
            തിരികെ U_MISMATCH;
        }
    } അല്ലെങ്കിൽ {
        എങ്കിൽ (hasStrings()) { // try strings first

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
            int32_t മുകളിലെ_Water_Length = 0;

            വേണ്ടി (i=0; i<strings_->size(); ++i) {
                സ്ഥിരം UnicodeString& trial = *static_cast<സ്ഥിരം UnicodeString*>(strings_->elementAt(i));
                എങ്കിൽ (trial.isEmpty()) {
                    തുടരുക;  // skip the empty string
                }

                char16_t c = trial.charAt(forward ? 0 : trial.length() - 1);

                // Strings are sorted, so we can optimize in the
                // forward direction.
                എങ്കിൽ (forward && c > firstChar) നിർത്തുക;
                എങ്കിൽ (c != firstChar) തുടരുക;

                int32_t matchLen = matchRest(text, offset, limit, trial);

                എങ്കിൽ (incremental) {
                    int32_t maxLen = forward ? limit-offset : offset-limit;
                    എങ്കിൽ (matchLen == maxLen) {
                        // We have successfully matched but only up to limit.
                        തിരികെ U_PARTIAL_MATCH;
                    }
                }

                എങ്കിൽ (matchLen == trial.length()) {
                    // We have successfully matched the whole string.
                    എങ്കിൽ (matchLen > മുകളിലെ_Water_Length) {
                        മുകളിലെ_Water_Length = matchLen;
                    }
                    // In the forward direction we know strings
                    // are sorted so we can bail early.
                    എങ്കിൽ (forward && matchLen < മുകളിലെ_Water_Length) {
                        നിർത്തുക;
                    }
                    തുടരുക;
                }
            }

            // We've checked all strings without a partial match.
            // If we have full matches, return the longest one.
            എങ്കിൽ (മുകളിലെ_Water_Length != 0) {
                offset += forward ? മുകളിലെ_Water_Length : -മുകളിലെ_Water_Length;
                തിരികെ U_MATCH;
            }
        }
        തിരികെ UnicodeFilter::matches(text, offset, limit, incremental);
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
int32_t UnicodeSet::matchRest(സ്ഥിരം Replaceable& text,
                              int32_t start, int32_t limit,
                              സ്ഥിരം UnicodeString& s) {
    int32_t i;
    int32_t maxLen;
    int32_t slen = s.length();
    എങ്കിൽ (start < limit) {
        maxLen = limit - start;
        എങ്കിൽ (maxLen > slen) maxLen = slen;
        വേണ്ടി (i = 1; i < maxLen; ++i) {
            എങ്കിൽ (text.charAt(start + i) != s.charAt(i)) തിരികെ 0;
        }
    } അല്ലെങ്കിൽ {
        maxLen = start - limit;
        എങ്കിൽ (maxLen > slen) maxLen = slen;
        --slen; // <=> slen = s.length() - 1;
        വേണ്ടി (i = 1; i < maxLen; ++i) {
            എങ്കിൽ (text.charAt(start - i) != s.charAt(slen - i)) തിരികെ 0;
        }
    }
    തിരികെ maxLen;
}

/**
 * Implement of UnicodeMatcher
 */
ശൂന്യം UnicodeSet::addMatchSetTo(UnicodeSet& toUnionTo) സ്ഥിരം {
    toUnionTo.addAll(*ഇത്);
}

/**
 * Returns the index of the given character within this set, where
 * the set is ordered by ascending code point.  If the character
 * is not in this set, return -1.  The inverse of this method is
 * <code>charAt()</code>.
 * @return an index from 0..size()-1, or -1
 */
int32_t UnicodeSet::indexOf(UChar32 c) സ്ഥിരം {
    എങ്കിൽ (c < MIN_മൂല്യം || c > MAX_മൂല്യം) {
        തിരികെ -1;
    }
    int32_t i = 0;
    int32_t n = 0;
    വേണ്ടി (;;) {
        UChar32 start = list[i++];
        എങ്കിൽ (c < start) {
            തിരികെ -1;
        }
        UChar32 limit = list[i++];
        എങ്കിൽ (c < limit) {
            തിരികെ n + c - start;
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
UChar32 UnicodeSet::charAt(int32_t index) സ്ഥിരം {
    എങ്കിൽ (index >= 0) {
        // len2 is the largest even integer <= len, that is, it is len
        // for even values and len-1 for odd values.  With odd values
        // the last entry is UNICODESET_HIGH.
        int32_t len2 = len & ~1;
        വേണ്ടി (int32_t i=0; i < len2;) {
            UChar32 start = list[i++];
            int32_t count = list[i++] - start;
            എങ്കിൽ (index < count) {
                തിരികെ static_cast<UChar32>(start + index);
            }
            index -= count;
        }
    }
    തിരികെ static_cast<UChar32>(-1);
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
    തിരികെ *ഇത്;
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
    എങ്കിൽ (pinCodePoint(start) < pinCodePoint(end)) {
        UChar32 limit = end + 1;
        // Fast path for adding a new range after the last one.
        // Odd list length: [..., lastStart, lastLimit, HIGH]
        എങ്കിൽ ((len & 1) != 0) {
            // If the list is empty, set lastLimit low enough to not be adjacent to 0.
            UChar32 lastLimit = len == 1 ? -2 : list[len - 2];
            എങ്കിൽ (lastLimit <= start && !isFrozen() && !isBogus()) {
                എങ്കിൽ (lastLimit == start) {
                    // Extend the last range.
                    list[len - 2] = limit;
                    എങ്കിൽ (limit == UNICODESET_മുകളിലെ) {
                        --len;
                    }
                } അല്ലെങ്കിൽ {
                    list[len - 1] = start;
                    എങ്കിൽ (limit < UNICODESET_മുകളിലെ) {
                        എങ്കിൽ (ensureCapacity(len + 2)) {
                            list[len++] = limit;
                            list[len++] = UNICODESET_മുകളിലെ;
                        }
                    } അല്ലെങ്കിൽ {  // limit == UNICODESET_HIGH
                        എങ്കിൽ (ensureCapacity(len + 1)) {
                            list[len++] = UNICODESET_മുകളിലെ;
                        }
                    }
                }
                releasePattern();
                തിരികെ *ഇത്;
            }
        }
        // This is slow. Could be much faster using findCodePoint(start)
        // and modifying the list, dealing with adjacent & overlapping ranges.
        UChar32 range[3] = { start, limit, UNICODESET_മുകളിലെ };
        add(range, 2, 0);
    } അല്ലെങ്കിൽ എങ്കിൽ (start == end) {
        add(start);
    }
    തിരികെ *ഇത്;
}

// #define DEBUG_US_ADD

#ifdef DEBUG_US_ADD
#അന്തര്ഭാവ <stdio.h>
ശൂന്യം dump(UChar32 c) {
    എങ്കിൽ (c <= 0xFF) {
        printf("%c", (അക്ഷരം)c);
    } അല്ലെങ്കിൽ {
        printf("U+%04X", c);
    }
}
ശൂന്യം dump(സ്ഥിരം UChar32* list, int32_t len) {
    printf("[");
    വേണ്ടി (int32_t i=0; i<len; ++i) {
        എങ്കിൽ (i != 0) printf(", ");
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
    എങ്കിൽ ((i & 1) != 0  || isFrozen() || isBogus()) തിരികെ *ഇത്;

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

    എങ്കിൽ (c == list[i]-1) {
        // c is before start of next range
        list[i] = c;
        // if we touched the HIGH mark, then add a new one
        എങ്കിൽ (c == (UNICODESET_മുകളിലെ - 1)) {
            എങ്കിൽ (!ensureCapacity(len+1)) {
                // ensureCapacity will mark the object as Bogus if OOM failure happens.
                തിരികെ *ഇത്;
            }
            list[len++] = UNICODESET_മുകളിലെ;
        }
        എങ്കിൽ (i > 0 && c == list[i-1]) {
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
            ആകുമ്പോൾ (src < srclimit) *(dst++) = *(src++);

            len -= 2;
        }
    }

    അല്ലെങ്കിൽ എങ്കിൽ (i > 0 && c == list[i-1]) {
        // c is after end of prior range
        list[i-1]++;
        // no need to check for collapse here
    }

    അല്ലെങ്കിൽ {
        // At this point we know the new char is not adjacent to
        // any existing ranges, and it is not 10FFFF.


        // [..., start_k-1, limit_k-1, start_k, limit_k, ..., HIGH]
        //                             ^
        //                             list[i]

        // [..., start_k-1, limit_k-1, c, c+1, start_k, limit_k, ..., HIGH]
        //                             ^
        //                             list[i]

        എങ്കിൽ (!ensureCapacity(len+2)) {
            // ensureCapacity will mark the object as Bogus if OOM failure happens.
            തിരികെ *ഇത്;
        }

        UChar32 *p = list + i;
        uprv_memmove(p + 2, p, (len - i) * വലുപ്പം(*p));
        list[i] = c;
        list[i+1] = c+1;
        len += 2;
    }

#ifdef DEBUG_US_ADD
    dump(list, len);
    printf("\n");

    വേണ്ടി (i=1; i<len; ++i) {
        എങ്കിൽ (list[i] <= list[i-1]) {
            // Corrupt array!
            printf("ERROR: list has been corrupted\n");
            exit(1);
        }
    }
#endif

    releasePattern();
    തിരികെ *ഇത്;
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
UnicodeSet& UnicodeSet::add(സ്ഥിരം UnicodeString& s) U_LIFETIME_BOUND {
    എങ്കിൽ (isFrozen() || isBogus()) തിരികെ *ഇത്;
    int32_t cp = getSingleCP(s);
    എങ്കിൽ (cp < 0) {
        എങ്കിൽ (!stringsContains(s)) {
            _add(s);
            releasePattern();
        }
    } അല്ലെങ്കിൽ {
        add(static_cast<UChar32>(cp));
    }
    തിരികെ *ഇത്;
}

/**
 * Adds the given string, in order, to 'strings_'.  The given string
 * must have been checked by the caller to not already be in 'strings_'.
 */
ശൂന്യം UnicodeSet::_add(സ്ഥിരം UnicodeString& s) {
    എങ്കിൽ (isFrozen() || isBogus()) {
        തിരികെ;
    }
    UErrorCode ec = U_ZERO_ERROR;
    എങ്കിൽ (strings_ == nullptr && !allocateStrings(ec)) {
        setToBogus();
        തിരികെ;
    }
    LocalPointer<UnicodeString> t(പുതിയ UnicodeString(s));
    എങ്കിൽ (t.isNull()) { // Check for memory allocation error.
        setToBogus();
        തിരികെ;
    }
    strings_->sorted_തിരുകുക(t.orphan(), compareUnicodeString, ec);
    എങ്കിൽ (U_FAILURE(ec)) {
        setToBogus();
    }
}

/**
 * @return a code point IF the string consists of a single one.
 * otherwise returns -1.
 * @param string to test
 */
int32_t UnicodeSet::getSingleCP(സ്ഥിരം UnicodeString& s) {
    int32_t sLength = s.length();
    എങ്കിൽ (sLength == 1) തിരികെ s.charAt(0);
    എങ്കിൽ (sLength == 2) {
        UChar32 cp = s.char32At(0);
        എങ്കിൽ (cp > 0xFFFF) { // is surrogate pair
            തിരികെ cp;
        }
    }
    തിരികെ -1;
}

/**
 * Adds each of the characters in this string to the set. Thus "ch" => {"c", "h"}
 * If this set already any particular character, it has no effect on that character.
 * @param the source string
 * @return the modified set, for chaining
 */
UnicodeSet& UnicodeSet::addAll(സ്ഥിരം UnicodeString& s) U_LIFETIME_BOUND {
    UChar32 cp;
    വേണ്ടി (int32_t i = 0; i < s.length(); i += U16_LENGTH(cp)) {
        cp = s.char32At(i);
        add(cp);
    }
    തിരികെ *ഇത്;
}

/**
 * Retains EACH of the characters in this string. Note: "ch" == {"c", "h"}
 * If this set already any particular character, it has no effect on that character.
 * @param the source string
 * @return the modified set, for chaining
 */
UnicodeSet& UnicodeSet::retainAll(സ്ഥിരം UnicodeString& s) U_LIFETIME_BOUND {
    UnicodeSet set;
    set.addAll(s);
    retainAll(set);
    തിരികെ *ഇത്;
}

/**
 * Complement EACH of the characters in this string. Note: "ch" == {"c", "h"}
 * If this set already any particular character, it has no effect on that character.
 * @param the source string
 * @return the modified set, for chaining
 */
UnicodeSet& UnicodeSet::complementAll(സ്ഥിരം UnicodeString& s) U_LIFETIME_BOUND {
    UnicodeSet set;
    set.addAll(s);
    complementAll(set);
    തിരികെ *ഇത്;
}

/**
 * Remove EACH of the characters in this string. Note: "ch" == {"c", "h"}
 * If this set already any particular character, it has no effect on that character.
 * @param the source string
 * @return the modified set, for chaining
 */
UnicodeSet& UnicodeSet::removeAll(സ്ഥിരം UnicodeString& s) U_LIFETIME_BOUND {
    UnicodeSet set;
    set.addAll(s);
    removeAll(set);
    തിരികെ *ഇത്;
}

UnicodeSet& UnicodeSet::removeAllStrings() U_LIFETIME_BOUND {
    എങ്കിൽ (!isFrozen() && hasStrings()) {
        strings_->removeAllElements();
        releasePattern();
    }
    തിരികെ *ഇത്;
}


/**
 * Makes a set from a multicharacter string. Thus "ch" => {"ch"}
 * <br><b>Warning: you cannot add an empty string ("") to a UnicodeSet.</b>
 * @param the source string
 * @return a newly created set containing the given string
 */
UnicodeSet* U_EXPORT2 UnicodeSet::createFrom(സ്ഥിരം UnicodeString& s) {
    UnicodeSet *set = പുതിയ UnicodeSet();
    എങ്കിൽ (set != nullptr) { // Check for memory allocation error.
        set->add(s);
    }
    തിരികെ set;
}


/**
 * Makes a set from each of the characters in the string. Thus "ch" => {"c", "h"}
 * @param the source string
 * @return a newly created set containing the given characters
 */
UnicodeSet* U_EXPORT2 UnicodeSet::createFromAll(സ്ഥിരം UnicodeString& s) {
    UnicodeSet *set = പുതിയ UnicodeSet();
    എങ്കിൽ (set != nullptr) { // Check for memory allocation error.
        set->addAll(s);
    }
    തിരികെ set;
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
    എങ്കിൽ (pinCodePoint(start) <= pinCodePoint(end)) {
        UChar32 range[3] = { start, end+1, UNICODESET_മുകളിലെ };
        retain(range, 2, 0);
    } അല്ലെങ്കിൽ {
        clear();
    }
    തിരികെ *ഇത്;
}

UnicodeSet& UnicodeSet::retain(UChar32 c) U_LIFETIME_BOUND {
    തിരികെ retain(c, c);
}

UnicodeSet& UnicodeSet::retain(സ്ഥിരം UnicodeString &s) U_LIFETIME_BOUND {
    എങ്കിൽ (isFrozen() || isBogus()) { തിരികെ *ഇത്; }
    UChar32 cp = getSingleCP(s);
    എങ്കിൽ (cp < 0) {
        തർക്കം isIn = stringsContains(s);
        // Check for getRangeCount() first to avoid somewhat-expensive size()
        // when there are single code points.
        എങ്കിൽ (isIn && get_Range_എണ്ണം() == 0 && size() == 1) {
            തിരികെ *ഇത്;
        }
        clear();
        എങ്കിൽ (isIn) {
            _add(s);
        }
    } അല്ലെങ്കിൽ {
        retain(cp, cp);
    }
    തിരികെ *ഇത്;
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
    എങ്കിൽ (pinCodePoint(start) <= pinCodePoint(end)) {
        UChar32 range[3] = { start, end+1, UNICODESET_മുകളിലെ };
        retain(range, 2, 2);
    }
    തിരികെ *ഇത്;
}

/**
 * Removes the specified character from this set if it is present.
 * The set will not contain the specified range once the call
 * returns.
 */
UnicodeSet& UnicodeSet::remove(UChar32 c) U_LIFETIME_BOUND {
    തിരികെ remove(c, c);
}

/**
 * Removes the specified string from this set if it is present.
 * The set will not contain the specified character once the call
 * returns.
 * @param the source string
 * @return the modified set, for chaining
 */
UnicodeSet& UnicodeSet::remove(സ്ഥിരം UnicodeString& s) U_LIFETIME_BOUND {
    എങ്കിൽ (isFrozen() || isBogus()) തിരികെ *ഇത്;
    int32_t cp = getSingleCP(s);
    എങ്കിൽ (cp < 0) {
        എങ്കിൽ (strings_ != nullptr && strings_->removeElement((ശൂന്യം*) &s)) {
            releasePattern();
        }
    } അല്ലെങ്കിൽ {
        remove(static_cast<UChar32>(cp), static_cast<UChar32>(cp));
    }
    തിരികെ *ഇത്;
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
    എങ്കിൽ (isFrozen() || isBogus()) {
        തിരികെ *ഇത്;
    }
    എങ്കിൽ (pinCodePoint(start) <= pinCodePoint(end)) {
        UChar32 range[3] = { start, end+1, UNICODESET_മുകളിലെ };
        exclusiveOr(range, 2, 0);
    }
    releasePattern();
    തിരികെ *ഇത്;
}

UnicodeSet& UnicodeSet::complement(UChar32 c) U_LIFETIME_BOUND {
    തിരികെ complement(c, c);
}

/**
 * This is equivalent to
 * <code>complement(MIN_VALUE, MAX_VALUE)</code>.
 */
UnicodeSet& UnicodeSet::complement() U_LIFETIME_BOUND {
    എങ്കിൽ (isFrozen() || isBogus()) {
        തിരികെ *ഇത്;
    }
    എങ്കിൽ (list[0] == UNICODESET_താഴെയുള്ള) {
        uprv_memmove(list, list + 1, (size_t)(len-1)*വലുപ്പം(UChar32));
        --len;
    } അല്ലെങ്കിൽ {
        എങ്കിൽ (!ensureCapacity(len+1)) {
            തിരികെ *ഇത്;
        }
        uprv_memmove(list + 1, list, (size_t)len*വലുപ്പം(UChar32));
        list[0] = UNICODESET_താഴെയുള്ള;
        ++len;
    }
    releasePattern();
    തിരികെ *ഇത്;
}

/**
 * Complement the specified string in this set.
 * The set will not contain the specified string once the call
 * returns.
 *
 * @param s the string to complement
 * @return this object, for chaining
 */
UnicodeSet& UnicodeSet::complement(സ്ഥിരം UnicodeString& s) U_LIFETIME_BOUND {
    എങ്കിൽ (isFrozen() || isBogus()) തിരികെ *ഇത്;
    int32_t cp = getSingleCP(s);
    എങ്കിൽ (cp < 0) {
        എങ്കിൽ (stringsContains(s)) {
            strings_->removeElement((ശൂന്യം*) &s);
        } അല്ലെങ്കിൽ {
            _add(s);
        }
        releasePattern();
    } അല്ലെങ്കിൽ {
        complement(static_cast<UChar32>(cp), static_cast<UChar32>(cp));
    }
    തിരികെ *ഇത്;
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
UnicodeSet& UnicodeSet::addAll(സ്ഥിരം UnicodeSet& c) U_LIFETIME_BOUND {
    എങ്കിൽ ( c.len>0 && c.list!=nullptr ) {
        add(c.list, c.len, 0);
    }

    // Add strings in order
    എങ്കിൽ ( c.strings_!=nullptr ) {
        വേണ്ടി (int32_t i=0; i<c.strings_->size(); ++i) {
            സ്ഥിരം UnicodeString* s = static_cast<സ്ഥിരം UnicodeString*>(c.strings_->elementAt(i));
            എങ്കിൽ (!stringsContains(*s)) {
                _add(*s);
            }
        }
    }
    തിരികെ *ഇത്;
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
UnicodeSet& UnicodeSet::retainAll(സ്ഥിരം UnicodeSet& c) U_LIFETIME_BOUND {
    എങ്കിൽ (isFrozen() || isBogus()) {
        തിരികെ *ഇത്;
    }
    retain(c.list, c.len, 0);
    എങ്കിൽ (hasStrings()) {
        എങ്കിൽ (!c.hasStrings()) {
            strings_->removeAllElements();
        } അല്ലെങ്കിൽ {
            strings_->retainAll(*c.strings_);
        }
    }
    തിരികെ *ഇത്;
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
UnicodeSet& UnicodeSet::removeAll(സ്ഥിരം UnicodeSet& c) U_LIFETIME_BOUND {
    എങ്കിൽ (isFrozen() || isBogus()) {
        തിരികെ *ഇത്;
    }
    retain(c.list, c.len, 2);
    എങ്കിൽ (hasStrings() && c.hasStrings()) {
        strings_->removeAll(*c.strings_);
    }
    തിരികെ *ഇത്;
}

/**
 * Complements in this set all elements contained in the specified
 * set.  Any character in the other set will be removed if it is
 * in this set, or will be added if it is not in this set.
 *
 * @param c set that defines which elements will be xor'ed from
 *          this set.
 */
UnicodeSet& UnicodeSet::complementAll(സ്ഥിരം UnicodeSet& c) U_LIFETIME_BOUND {
    എങ്കിൽ (isFrozen() || isBogus()) {
        തിരികെ *ഇത്;
    }
    exclusiveOr(c.list, c.len, 0);

    എങ്കിൽ (c.strings_ != nullptr) {
        വേണ്ടി (int32_t i=0; i<c.strings_->size(); ++i) {
            ശൂന്യം* e = c.strings_->elementAt(i);
            എങ്കിൽ (strings_ == nullptr || !strings_->removeElement(e)) {
                _add(*static_cast<സ്ഥിരം UnicodeString*>(e));
            }
        }
    }
    തിരികെ *ഇത്;
}

/**
 * Removes all of the elements from this set.  This set will be
 * empty after this call returns.
 */
UnicodeSet& UnicodeSet::clear() U_LIFETIME_BOUND {
    എങ്കിൽ (isFrozen()) {
        തിരികെ *ഇത്;
    }
    list[0] = UNICODESET_മുകളിലെ;
    len = 1;
    releasePattern();
    എങ്കിൽ (strings_ != nullptr) {
        strings_->removeAllElements();
    }
    // Remove bogus
    fFlags = 0;
    തിരികെ *ഇത്;
}

/**
 * Iteration method that returns the number of ranges contained in
 * this set.
 * @see #getRangeStart
 * @see #getRangeEnd
 */
int32_t UnicodeSet::get_Range_എണ്ണം() സ്ഥിരം {
    തിരികെ len/2;
}

/**
 * Iteration method that returns the first character in the
 * specified range of this set.
 * @see #getRangeCount
 * @see #getRangeEnd
 */
UChar32 UnicodeSet::getRangeStart(int32_t index) സ്ഥിരം {
    തിരികെ list[index*2];
}

/**
 * Iteration method that returns the last character in the
 * specified range of this set.
 * @see #getRangeStart
 * @see #getRangeEnd
 */
UChar32 UnicodeSet::getRangeEnd(int32_t index) സ്ഥിരം {
    തിരികെ list[index*2 + 1] - 1;
}

സ്ഥിരം UnicodeString* UnicodeSet::getString(int32_t index) സ്ഥിരം U_LIFETIME_BOUND {
    തിരികെ static_cast<സ്ഥിരം UnicodeString*>(strings_->elementAt(index));
}

/**
 * Reallocate this objects internal structures to take up the least
 * possible space, without changing this object's value.
 */
UnicodeSet& UnicodeSet::compact() U_LIFETIME_BOUND {
    എങ്കിൽ (isFrozen() || isBogus()) {
        തിരികെ *ഇത്;
    }
    // Delete buffer first to defragment memory less.
    എങ്കിൽ (buffer != അട്ടി_List) {
        uprv_free(buffer);
        buffer = nullptr;
        bufferCapacity = 0;
    }
    എങ്കിൽ (list == അട്ടി_List) {
        // pass
    } അല്ലെങ്കിൽ എങ്കിൽ (len <= INITIAL_CAPACITY) {
        uprv_memcpy(അട്ടി_List, list, len * വലുപ്പം(UChar32));
        uprv_free(list);
        list = അട്ടി_List;
        capacity = INITIAL_CAPACITY;
    } അല്ലെങ്കിൽ എങ്കിൽ ((len + 7) < capacity) {
        // If we have more than a little unused capacity, shrink it to len.
        UChar32* temp = static_cast<UChar32*>(uprv_realloc(list, വലുപ്പം(UChar32) * len));
        എങ്കിൽ (temp) {
            list = temp;
            capacity = len;
        }
        // else what the heck happened?! We allocated less memory!
        // Oh well. We'll keep our original array.
    }
    എങ്കിൽ (strings_ != nullptr && strings_->isEmpty()) {
        delete strings_;
        strings_ = nullptr;
    }
    തിരികെ *ഇത്;
}

#ifdef DEBUG_SERIALIZE
#അന്തര്ഭാവ <stdio.h>
#endif

/**
 * Deserialize constructor.
 */
UnicodeSet::UnicodeSet(സ്ഥിരം uint16_t data[], int32_t dataLen, ESerialization serialization,
                       UErrorCode &ec) {

  എങ്കിൽ(U_FAILURE(ec)) {
    setToBogus();
    തിരികെ;
  }

  എങ്കിൽ( (serialization != kSerialized)
      || (data==nullptr)
      || (dataLen < 1)) {
    ec = U_ILLEGAL_ARGUMENT_ERROR;
    setToBogus();
    തിരികെ;
  }

  // bmp?
  int32_t headerSize = ((data[0]&0x8000)) ?2:1;
  int32_t bmpLength = (headerSize==1)?data[0]:data[1];

  int32_t newLength = (((data[0]&0x7FFF)-bmpLength)/2)+bmpLength;
#ifdef DEBUG_SERIALIZE
  printf("dataLen %d headerSize %d bmpLen %d len %d. data[0]=%X/%X/%X/%X\n", dataLen,headerSize,bmpLength,newLength, data[0],data[1],data[2],data[3]);
#endif
  എങ്കിൽ(!ensureCapacity(newLength + 1)) {  // +1 for HIGH
    തിരികെ;
  }
  // copy bmp
  int32_t i;
  വേണ്ടി(i = 0; i< bmpLength;i++) {
    list[i] = data[i+headerSize];
#ifdef DEBUG_SERIALIZE
    printf("<<16@%d[%d] %X\n", i+headerSize, i, list[i]);
#endif
  }
  // copy smp
  വേണ്ടി(i=bmpLength;i<newLength;i++) {
    list[i] = (static_cast<UChar32>(data[headerSize + bmpLength + (i - bmpLength) * 2 + 0]) << 16) +
               static_cast<UChar32>(data[headerSize + bmpLength + (i - bmpLength) * 2 + 1]);
#ifdef DEBUG_SERIALIZE
    printf("<<32@%d+[%d] %lX\n", headerSize+bmpLength+i, i, list[i]);
#endif
  }
  U_ASSERT(i == newLength);
  എങ്കിൽ (i == 0 || list[i - 1] != UNICODESET_മുകളിലെ) {
    list[i++] = UNICODESET_മുകളിലെ;
  }
  len = i;
}


int32_t UnicodeSet::serialize(uint16_t *dest, int32_t destCapacity, UErrorCode& ec) സ്ഥിരം {
    int32_t bmpLength, length, destLength;

    എങ്കിൽ (U_FAILURE(ec)) {
        തിരികെ 0;
    }

    എങ്കിൽ (destCapacity<0 || (destCapacity>0 && dest==nullptr)) {
        ec=U_ILLEGAL_ARGUMENT_ERROR;
        തിരികെ 0;
    }

    /* count necessary 16-bit units */
    length=ഇത്->len-1; // Subtract 1 to ignore final UNICODESET_HIGH
    // assert(length>=0);
    എങ്കിൽ (length==0) {
        /* empty set */
        എങ്കിൽ (destCapacity>0) {
            *dest=0;
        } അല്ലെങ്കിൽ {
            ec=U_BUFFER_OVERFLOW_ERROR;
        }
        തിരികെ 1;
    }
    /* now length>0 */

    എങ്കിൽ (ഇത്->list[length-1]<=0xffff) {
        /* all BMP */
        bmpLength=length;
    } അല്ലെങ്കിൽ എങ്കിൽ (ഇത്->list[0]>=0x10000) {
        /* all supplementary */
        bmpLength=0;
        length*=2;
    } അല്ലെങ്കിൽ {
        /* some BMP, some supplementary */
        വേണ്ടി (bmpLength=0; bmpLength<length && ഇത്->list[bmpLength]<=0xffff; ++bmpLength) {}
        length=bmpLength+2*(length-bmpLength);
    }
#ifdef DEBUG_SERIALIZE
    printf(">> bmpLength%d length%d len%d\n", bmpLength, length, len);
#endif
    /* length: number of 16-bit array units */
    എങ്കിൽ (length>0x7fff) {
        /* there are only 15 bits for the length in the first serialized word */
        ec=U_INDEX_OUTOFBOUNDS_ERROR;
        തിരികെ 0;
    }

    /*
     * total serialized length:
     * number of 16-bit array units (length) +
     * 1 length unit (always) +
     * 1 bmpLength unit (if there are supplementary values)
     */
    destLength=length+((length>bmpLength)?2:1);
    എങ്കിൽ (destLength<=destCapacity) {
        സ്ഥിരം UChar32 *p;
        int32_t i;

#ifdef DEBUG_SERIALIZE
        printf("writeHdr\n");
#endif
        *dest = static_cast<uint16_t>(length);
        എങ്കിൽ (length>bmpLength) {
            *dest|=0x8000;
            *++dest = static_cast<uint16_t>(bmpLength);
        }
        ++dest;

        /* write the BMP part of the array */
        p=ഇത്->list;
        വേണ്ടി (i=0; i<bmpLength; ++i) {
#ifdef DEBUG_SERIALIZE
          printf("writebmp: %x\n", (പൂർണ്ണസംഖ്യ)*p);
#endif
            *dest++ = static_cast<uint16_t>(*p++);
        }

        /* write the supplementary part of the array */
        വേണ്ടി (; i<length; i+=2) {
#ifdef DEBUG_SERIALIZE
          printf("write32: %x\n", (പൂർണ്ണസംഖ്യ)*p);
#endif
            *dest++ = static_cast<uint16_t>(*p >> 16);
            *dest++ = static_cast<uint16_t>(*p++);
        }
    } അല്ലെങ്കിൽ {
        ec=U_BUFFER_OVERFLOW_ERROR;
    }
    തിരികെ destLength;
}

//----------------------------------------------------------------
// Implementation: Utility methods
//----------------------------------------------------------------

/**
 * Allocate our strings vector and return true if successful.
 */
UBool UnicodeSet::allocateStrings(UErrorCode &status) {
    എങ്കിൽ (U_FAILURE(status)) {
        തിരികെ അസത്യം;
    }
    strings_ = പുതിയ UVector(uprv_deleteUObject,
                          uhash_compareUnicodeString, 1, status);
    എങ്കിൽ (strings_ == nullptr) { // Check for memory allocation error.
        status = U_MEMORY_ALLOCATION_ERROR;
        തിരികെ അസത്യം;
    }
    എങ്കിൽ (U_FAILURE(status)) {
        delete strings_;
        strings_ = nullptr;
        തിരികെ അസത്യം;
    } 
    തിരികെ സത്യം;
}

int32_t UnicodeSet::nextCapacity(int32_t minCapacity) {
    // Grow exponentially to reduce the frequency of allocations.
    എങ്കിൽ (minCapacity < INITIAL_CAPACITY) {
        തിരികെ minCapacity + INITIAL_CAPACITY;
    } അല്ലെങ്കിൽ എങ്കിൽ (minCapacity <= 2500) {
        തിരികെ 5 * minCapacity;
    } അല്ലെങ്കിൽ {
        int32_t newCapacity = 2 * minCapacity;
        എങ്കിൽ (newCapacity > MAX_LENGTH) {
            newCapacity = MAX_LENGTH;
        }
        തിരികെ newCapacity;
    }
}

തർക്കം UnicodeSet::ensureCapacity(int32_t newLen) {
    എങ്കിൽ (newLen > MAX_LENGTH) {
        newLen = MAX_LENGTH;
    }
    എങ്കിൽ (newLen <= capacity) {
        തിരികെ സത്യം;
    }
    int32_t newCapacity = nextCapacity(newLen);
    UChar32* temp = static_cast<UChar32*>(uprv_malloc(newCapacity * വലുപ്പം(UChar32)));
    എങ്കിൽ (temp == nullptr) {
        setToBogus(); // set the object to bogus state if an OOM failure occurred.
        തിരികെ അസത്യം;
    }
    // Copy only the actual contents.
    uprv_memcpy(temp, list, len * വലുപ്പം(UChar32));
    എങ്കിൽ (list != അട്ടി_List) {
        uprv_free(list);
    }
    list = temp;
    capacity = newCapacity;
    തിരികെ സത്യം;
}

തർക്കം UnicodeSet::ensureBufferCapacity(int32_t newLen) {
    എങ്കിൽ (newLen > MAX_LENGTH) {
        newLen = MAX_LENGTH;
    }
    എങ്കിൽ (newLen <= bufferCapacity) {
        തിരികെ സത്യം;
    }
    int32_t newCapacity = nextCapacity(newLen);
    UChar32* temp = static_cast<UChar32*>(uprv_malloc(newCapacity * വലുപ്പം(UChar32)));
    എങ്കിൽ (temp == nullptr) {
        setToBogus();
        തിരികെ അസത്യം;
    }
    // The buffer has no contents to be copied.
    // It is always filled from scratch after this call.
    എങ്കിൽ (buffer != അട്ടി_List) {
        uprv_free(buffer);
    }
    buffer = temp;
    bufferCapacity = newCapacity;
    തിരികെ സത്യം;
}

/**
 * Swap list and buffer.
 */
ശൂന്യം UnicodeSet::swapBuffers() {
    // swap list and buffer
    UChar32* temp = list;
    list = buffer;
    buffer = temp;

    int32_t c = capacity;
    capacity = bufferCapacity;
    bufferCapacity = c;
}

ശൂന്യം UnicodeSet::setToBogus() {
    clear(); // Remove everything in the set.
    fFlags = kIsBogus;
}

//----------------------------------------------------------------
// Implementation: Fundamental operators
//----------------------------------------------------------------

സ്ഥിതം വരിയിൽ UChar32 max(UChar32 a, UChar32 b) {
    തിരികെ (a > b) ? a : b;
}

// polarity = 0, 3 is normal: x xor y
// polarity = 1, 2: x xor ~y == x === y

ശൂന്യം UnicodeSet::exclusiveOr(സ്ഥിരം UChar32* other, int32_t otherLen, int8_t polarity) {
    എങ്കിൽ (isFrozen() || isBogus()) {
        തിരികെ;
    }
    എങ്കിൽ (!ensureBufferCapacity(len + otherLen)) {
        തിരികെ;
    }

    int32_t i = 0, j = 0, k = 0;
    UChar32 a = list[i++];
    UChar32 b;
    എങ്കിൽ (polarity == 1 || polarity == 2) {
        b = UNICODESET_താഴെയുള്ള;
        എങ്കിൽ (other[j] == UNICODESET_താഴെയുള്ള) { // skip base if already LOW
            ++j;
            b = other[j];
        }
    } അല്ലെങ്കിൽ {
        b = other[j++];
    }
    // simplest of all the routines
    // sort the values, discarding identicals!
    വേണ്ടി (;;) {
        എങ്കിൽ (a < b) {
            buffer[k++] = a;
            a = list[i++];
        } അല്ലെങ്കിൽ എങ്കിൽ (b < a) {
            buffer[k++] = b;
            b = other[j++];
        } അല്ലെങ്കിൽ എങ്കിൽ (a != UNICODESET_മുകളിലെ) { // at this point, a == b
            // discard both values!
            a = list[i++];
            b = other[j++];
        } അല്ലെങ്കിൽ { // DONE!
            buffer[k++] = UNICODESET_മുകളിലെ;
            len = k;
            നിർത്തുക;
        }
    }
    swapBuffers();
    releasePattern();
}

// polarity = 0 is normal: x union y
// polarity = 2: x union ~y
// polarity = 1: ~x union y
// polarity = 3: ~x union ~y

ശൂന്യം UnicodeSet::add(സ്ഥിരം UChar32* other, int32_t otherLen, int8_t polarity) {
    എങ്കിൽ (isFrozen() || isBogus() || other==nullptr) {
        തിരികെ;
    }
    എങ്കിൽ (!ensureBufferCapacity(len + otherLen)) {
        തിരികെ;
    }

    int32_t i = 0, j = 0, k = 0;
    UChar32 a = list[i++];
    UChar32 b = other[j++];
    // change from xor is that we have to check overlapping pairs
    // polarity bit 1 means a is second, bit 2 means b is.
    വേണ്ടി (;;) {
        തിരഞ്ഞെടുപ്പ് (polarity) {
          അവസ്ഥ 0: // both first; take lower if unequal
            എങ്കിൽ (a < b) { // take a
                // Back up over overlapping ranges in buffer[]
                എങ്കിൽ (k > 0 && a <= buffer[k-1]) {
                    // Pick latter end value in buffer[] vs. list[]
                    a = max(list[i], buffer[--k]);
                } അല്ലെങ്കിൽ {
                    // No overlap
                    buffer[k++] = a;
                    a = list[i];
                }
                i++; // Common if/else code factored out
                polarity ^= 1;
            } അല്ലെങ്കിൽ എങ്കിൽ (b < a) { // take b
                എങ്കിൽ (k > 0 && b <= buffer[k-1]) {
                    b = max(other[j], buffer[--k]);
                } അല്ലെങ്കിൽ {
                    buffer[k++] = b;
                    b = other[j];
                }
                j++;
                polarity ^= 2;
            } അല്ലെങ്കിൽ { // a == b, take a, drop b
                എങ്കിൽ (a == UNICODESET_മുകളിലെ) പോകുക loop_end;
                // This is symmetrical; it doesn't matter if
                // we backtrack with a or b. - liu
                എങ്കിൽ (k > 0 && a <= buffer[k-1]) {
                    a = max(list[i], buffer[--k]);
                } അല്ലെങ്കിൽ {
                    // No overlap
                    buffer[k++] = a;
                    a = list[i];
                }
                i++;
                polarity ^= 1;
                b = other[j++];
                polarity ^= 2;
            }
            നിർത്തുക;
          അവസ്ഥ 3: // both second; take higher if unequal, and drop other
            എങ്കിൽ (b <= a) { // take a
                എങ്കിൽ (a == UNICODESET_മുകളിലെ) പോകുക loop_end;
                buffer[k++] = a;
            } അല്ലെങ്കിൽ { // take b
                എങ്കിൽ (b == UNICODESET_മുകളിലെ) പോകുക loop_end;
                buffer[k++] = b;
            }
            a = list[i++];
            polarity ^= 1;   // factored common code
            b = other[j++];
            polarity ^= 2;
            നിർത്തുക;
          അവസ്ഥ 1: // a second, b first; if b < a, overlap
            എങ്കിൽ (a < b) { // no overlap, take a
                buffer[k++] = a; a = list[i++]; polarity ^= 1;
            } അല്ലെങ്കിൽ എങ്കിൽ (b < a) { // OVERLAP, drop b
                b = other[j++];
                polarity ^= 2;
            } അല്ലെങ്കിൽ { // a == b, drop both!
                എങ്കിൽ (a == UNICODESET_മുകളിലെ) പോകുക loop_end;
                a = list[i++];
                polarity ^= 1;
                b = other[j++];
                polarity ^= 2;
            }
            നിർത്തുക;
          അവസ്ഥ 2: // a first, b second; if a < b, overlap
            എങ്കിൽ (b < a) { // no overlap, take b
                buffer[k++] = b;
                b = other[j++];
                polarity ^= 2;
            } അല്ലെങ്കിൽ  എങ്കിൽ (a < b) { // OVERLAP, drop a
                a = list[i++];
                polarity ^= 1;
            } അല്ലെങ്കിൽ { // a == b, drop both!
                എങ്കിൽ (a == UNICODESET_മുകളിലെ) പോകുക loop_end;
                a = list[i++];
                polarity ^= 1;
                b = other[j++];
                polarity ^= 2;
            }
            നിർത്തുക;
        }
    }
 loop_end:
    buffer[k++] = UNICODESET_മുകളിലെ;    // terminate
    len = k;
    swapBuffers();
    releasePattern();
}

// polarity = 0 is normal: x intersect y
// polarity = 2: x intersect ~y == set-minus
// polarity = 1: ~x intersect y
// polarity = 3: ~x intersect ~y

ശൂന്യം UnicodeSet::retain(സ്ഥിരം UChar32* other, int32_t otherLen, int8_t polarity) {
    എങ്കിൽ (isFrozen() || isBogus()) {
        തിരികെ;
    }
    എങ്കിൽ (!ensureBufferCapacity(len + otherLen)) {
        തിരികെ;
    }

    int32_t i = 0, j = 0, k = 0;
    UChar32 a = list[i++];
    UChar32 b = other[j++];
    // change from xor is that we have to check overlapping pairs
    // polarity bit 1 means a is second, bit 2 means b is.
    വേണ്ടി (;;) {
        തിരഞ്ഞെടുപ്പ് (polarity) {
          അവസ്ഥ 0: // both first; drop the smaller
            എങ്കിൽ (a < b) { // drop a
                a = list[i++];
                polarity ^= 1;
            } അല്ലെങ്കിൽ എങ്കിൽ (b < a) { // drop b
                b = other[j++];
                polarity ^= 2;
            } അല്ലെങ്കിൽ { // a == b, take one, drop other
                എങ്കിൽ (a == UNICODESET_മുകളിലെ) പോകുക loop_end;
                buffer[k++] = a;
                a = list[i++];
                polarity ^= 1;
                b = other[j++];
                polarity ^= 2;
            }
            നിർത്തുക;
          അവസ്ഥ 3: // both second; take lower if unequal
            എങ്കിൽ (a < b) { // take a
                buffer[k++] = a;
                a = list[i++];
                polarity ^= 1;
            } അല്ലെങ്കിൽ എങ്കിൽ (b < a) { // take b
                buffer[k++] = b;
                b = other[j++];
                polarity ^= 2;
            } അല്ലെങ്കിൽ { // a == b, take one, drop other
                എങ്കിൽ (a == UNICODESET_മുകളിലെ) പോകുക loop_end;
                buffer[k++] = a;
                a = list[i++];
                polarity ^= 1;
                b = other[j++];
                polarity ^= 2;
            }
            നിർത്തുക;
          അവസ്ഥ 1: // a second, b first;
            എങ്കിൽ (a < b) { // NO OVERLAP, drop a
                a = list[i++];
                polarity ^= 1;
            } അല്ലെങ്കിൽ എങ്കിൽ (b < a) { // OVERLAP, take b
                buffer[k++] = b;
                b = other[j++];
                polarity ^= 2;
            } അല്ലെങ്കിൽ { // a == b, drop both!
                എങ്കിൽ (a == UNICODESET_മുകളിലെ) പോകുക loop_end;
                a = list[i++];
                polarity ^= 1;
                b = other[j++];
                polarity ^= 2;
            }
            നിർത്തുക;
          അവസ്ഥ 2: // a first, b second; if a < b, overlap
            എങ്കിൽ (b < a) { // no overlap, drop b
                b = other[j++];
                polarity ^= 2;
            } അല്ലെങ്കിൽ  എങ്കിൽ (a < b) { // OVERLAP, take a
                buffer[k++] = a;
                a = list[i++];
                polarity ^= 1;
            } അല്ലെങ്കിൽ { // a == b, drop both!
                എങ്കിൽ (a == UNICODESET_മുകളിലെ) പോകുക loop_end;
                a = list[i++];
                polarity ^= 1;
                b = other[j++];
                polarity ^= 2;
            }
            നിർത്തുക;
        }
    }
 loop_end:
    buffer[k++] = UNICODESET_മുകളിലെ;    // terminate
    len = k;
    swapBuffers();
    releasePattern();
}

/**
 * Append the <code>toPattern()</code> representation of a
 * string to the given <code>StringBuffer</code>.
 */
ശൂന്യം UnicodeSet::_appendToPat(UnicodeString& buf, സ്ഥിരം UnicodeString& s, UBool escapeUnprintable) {
    UChar32 cp;
    വേണ്ടി (int32_t i = 0; i < s.length(); i += U16_LENGTH(cp)) {
        _appendToPat(buf, cp = s.char32At(i), escapeUnprintable);
    }
}

/**
 * Append the <code>toPattern()</code> representation of a
 * character to the given <code>StringBuffer</code>.
 */
ശൂന്യം UnicodeSet::_appendToPat(UnicodeString& buf, UChar32 c, UBool escapeUnprintable) {
    എങ്കിൽ (escapeUnprintable ? ICU_Utility::isUnprintable(c) : ICU_Utility::shouldAlwaysBeEscaped(c)) {
        // Use hex escape notation (\uxxxx or \Uxxxxxxxx) for anything
        // unprintable
        ICU_Utility::escape(buf, c);
        തിരികെ;
    }
    // Okay to let ':' pass through
    തിരഞ്ഞെടുപ്പ് (c) {
    അവസ്ഥ u'[':
    അവസ്ഥ u']':
    അവസ്ഥ u'-':
    അവസ്ഥ u'^':
    അവസ്ഥ u'&':
    അവസ്ഥ u'\\':
    അവസ്ഥ u'{':
    അവസ്ഥ u'}':
    അവസ്ഥ u':':
    അവസ്ഥ ചിഹ്നം_Table::ചിഹ്നം_REF:
        buf.append(u'\\');
        നിർത്തുക;
    സ്ഥിരസ്ഥിതി:
        // Escape whitespace
        എങ്കിൽ (PatternProps::isWhiteSpace(c)) {
            buf.append(u'\\');
        }
        നിർത്തുക;
    }
    buf.append(c);
}

ശൂന്യം UnicodeSet::_appendToPat(UnicodeString &result, UChar32 start, UChar32 end,
                              UBool escapeUnprintable) {
    _appendToPat(result, start, escapeUnprintable);
    എങ്കിൽ (start != end) {
        എങ്കിൽ ((start+1) != end ||
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
                                      UBool escapeUnprintable) സ്ഥിരം
{
    എങ്കിൽ (pat != nullptr) {
        int32_t i;
        int32_t backslash_എണ്ണം = 0;
        വേണ്ടി (i=0; i<patLen; ) {
            UChar32 c;
            U16_NEXT(pat, i, patLen, c);
            എങ്കിൽ (escapeUnprintable ?
                    ICU_Utility::isUnprintable(c) : ICU_Utility::shouldAlwaysBeEscaped(c)) {
                // If the unprintable character is preceded by an odd
                // number of backslashes, then it has been escaped.
                // Before unescaping it, we delete the final
                // backslash.
                എങ്കിൽ ((backslash_എണ്ണം % 2) == 1) {
                    result.truncate(result.length() - 1);
                }
                ICU_Utility::escape(result, c);
                backslash_എണ്ണം = 0;
            } അല്ലെങ്കിൽ {
                result.append(c);
                എങ്കിൽ (c == u'\\') {
                    ++backslash_എണ്ണം;
                } അല്ലെങ്കിൽ {
                    backslash_എണ്ണം = 0;
                }
            }
        }
        തിരികെ result;
    }

    തിരികെ _generatePattern(result, escapeUnprintable);
}

/**
 * Returns a string representation of this set.  If the result of
 * calling this function is passed to a UnicodeSet constructor, it
 * will produce another set that is equal to this one.
 */
UnicodeString& UnicodeSet::toPattern(UnicodeString& result U_LIFETIME_BOUND,
                                     UBool escapeUnprintable) സ്ഥിരം
{
    result.truncate(0);
    തിരികെ _toPattern(result, escapeUnprintable);
}

/**
 * Generate and append a string representation of this set to result.
 * This does not use this.pat, the cleaned up copy of the string
 * passed to applyPattern().
 */
UnicodeString& UnicodeSet::_generatePattern(UnicodeString& result U_LIFETIME_BOUND,
                                            UBool escapeUnprintable) സ്ഥിരം
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
    എങ്കിൽ (len >= 4 && list[0] == 0 && limit == len && !hasStrings()) {
        // Emit the inverse
        result.append(u'^');
        // Offsetting the inversion list index by one lets us
        // iterate over the ranges of the set complement.
        i = 1;
        --limit;
    }

    // Emit the ranges as pairs.
    ആകുമ്പോൾ (i < limit) {
        UChar32 start = list[i];  // getRangeStart()
        UChar32 end = list[i + 1] - 1;  // getRangeEnd() = range limit minus one
        എങ്കിൽ (!(0xd800 <= end && end <= 0xdbff)) {
            _appendToPat(result, start, end, escapeUnprintable);
            i += 2;
        } അല്ലെങ്കിൽ {
            // The range ends with a lead surrogate.
            // Avoid writing what looks like a lead+trail surrogate pair.
            // 1. Postpone ranges that start with a lead surrogate code point.
            int32_t firstLead = i;
            ആകുമ്പോൾ ((i += 2) < limit && list[i] <= 0xdbff) {}
            int32_t firstAfterLead = i;
            // 2. Write following ranges that start with a trail surrogate code point.
            ആകുമ്പോൾ (i < limit && (start = list[i]) <= 0xdfff) {
                _appendToPat(result, start, list[i + 1] - 1, escapeUnprintable);
                i += 2;
            }
            // 3. Now write the postponed ranges.
            വേണ്ടി (പൂർണ്ണസംഖ്യ j = firstLead; j < firstAfterLead; j += 2) {
                _appendToPat(result, list[j], list[j + 1] - 1, escapeUnprintable);
            }
        }
    }

    എങ്കിൽ (strings_ != nullptr) {
        വേണ്ടി (int32_t i = 0; i<strings_->size(); ++i) {
            result.append(u'{');
            _appendToPat(result,
                         *static_cast<സ്ഥിരം UnicodeString*>(strings_->elementAt(i)),
                         escapeUnprintable);
            result.append(u'}');
        }
    }
    തിരികെ result.append(u']');
}

/**
* Release existing cached pattern
*/
ശൂന്യം UnicodeSet::releasePattern() {
    എങ്കിൽ (pat) {
        uprv_free(pat);
        pat = nullptr;
        patLen = 0;
    }
}

/**
* Set the new pattern to cache.
*/
ശൂന്യം UnicodeSet::setPattern(സ്ഥിരം char16_t *newPat, int32_t newPatLen) {
    releasePattern();
    pat = static_cast<char16_t*>(uprv_malloc((newPatLen + 1) * വലുപ്പം(char16_t)));
    എങ്കിൽ (pat) {
        patLen = newPatLen;
        u_memcpy(pat, newPat, patLen);
        pat[patLen] = 0;
    }
    // else we don't care if malloc failed. This was just a nice cache.
    // We can regenerate an equivalent pattern later when requested.
}

UnicodeSet* UnicodeSet::freeze() U_LIFETIME_BOUND {
    എങ്കിൽ(!isFrozen() && !isBogus()) {
        compact();

        // Optimize contains() and span() and similar functions.
        എങ്കിൽ (hasStrings()) {
            stringSpan = പുതിയ UnicodeSetStringSpan(*ഇത്, *strings_, UnicodeSetStringSpan::ALL);
            എങ്കിൽ (stringSpan == nullptr) {
                setToBogus();
                തിരികെ ഇത്;
            } അല്ലെങ്കിൽ എങ്കിൽ (!stringSpan->needsStringSpanUTF16()) {
                // All strings are irrelevant for span() etc. because
                // all of each string's code points are contained in this set.
                // Do not check needsStringSpanUTF8() because UTF-8 has at most as
                // many relevant strings as UTF-16.
                // (Thus needsStringSpanUTF8() implies needsStringSpanUTF16().)
                delete stringSpan;
                stringSpan = nullptr;
            }
        }
        എങ്കിൽ (stringSpan == nullptr) {
            // No span-relevant strings: Optimize for code point spans.
            bmpSet=പുതിയ BMPSet(list, len);
            എങ്കിൽ (bmpSet == nullptr) { // Check for memory allocation error.
                setToBogus();
            }
        }
    }
    തിരികെ ഇത്;
}

int32_t UnicodeSet::span(സ്ഥിരം char16_t *s, int32_t length, USetSpanCondition spanCondition) സ്ഥിരം {
    എങ്കിൽ(length>0 && bmpSet!=nullptr) {
        തിരികെ static_cast<int32_t>(bmpSet->span(s, s + length, spanCondition) - s);
    }
    എങ്കിൽ(length<0) {
        length=u_strlen(s);
    }
    എങ്കിൽ(length==0) {
        തിരികെ 0;
    }
    എങ്കിൽ(stringSpan!=nullptr) {
        തിരികെ stringSpan->span(s, length, spanCondition);
    } അല്ലെങ്കിൽ എങ്കിൽ(hasStrings()) {
        uint32_t which= spanCondition==USET_SPAN_NOT_CONTAINED ?
                            UnicodeSetStringSpan::FWD_UTF16_NOT_CONTAINED :
                            UnicodeSetStringSpan::FWD_UTF16_CONTAINED;
        UnicodeSetStringSpan strSpan(*ഇത്, *strings_, which);
        എങ്കിൽ(strSpan.needsStringSpanUTF16()) {
            തിരികെ strSpan.span(s, length, spanCondition);
        }
    }

    എങ്കിൽ(spanCondition!=USET_SPAN_NOT_CONTAINED) {
        spanCondition=USET_SPAN_CONTAINED;  // Pin to 0/1 values.
    }

    UChar32 c;
    int32_t start=0, prev=0;
    ചെയ്യുക {
        U16_NEXT(s, start, length, c);
        എങ്കിൽ(spanCondition!=contains(c)) {
            നിർത്തുക;
        }
    } ആകുമ്പോൾ((prev=start)<length);
    തിരികെ prev;
}

int32_t UnicodeSet::spanBack(സ്ഥിരം char16_t *s, int32_t length, USetSpanCondition spanCondition) സ്ഥിരം {
    എങ്കിൽ(length>0 && bmpSet!=nullptr) {
        തിരികെ static_cast<int32_t>(bmpSet->spanBack(s, s + length, spanCondition) - s);
    }
    എങ്കിൽ(length<0) {
        length=u_strlen(s);
    }
    എങ്കിൽ(length==0) {
        തിരികെ 0;
    }
    എങ്കിൽ(stringSpan!=nullptr) {
        തിരികെ stringSpan->spanBack(s, length, spanCondition);
    } അല്ലെങ്കിൽ എങ്കിൽ(hasStrings()) {
        uint32_t which= spanCondition==USET_SPAN_NOT_CONTAINED ?
                            UnicodeSetStringSpan::BACK_UTF16_NOT_CONTAINED :
                            UnicodeSetStringSpan::BACK_UTF16_CONTAINED;
        UnicodeSetStringSpan strSpan(*ഇത്, *strings_, which);
        എങ്കിൽ(strSpan.needsStringSpanUTF16()) {
            തിരികെ strSpan.spanBack(s, length, spanCondition);
        }
    }

    എങ്കിൽ(spanCondition!=USET_SPAN_NOT_CONTAINED) {
        spanCondition=USET_SPAN_CONTAINED;  // Pin to 0/1 values.
    }

    UChar32 c;
    int32_t prev=length;
    ചെയ്യുക {
        U16_PREV(s, 0, length, c);
        എങ്കിൽ(spanCondition!=contains(c)) {
            നിർത്തുക;
        }
    } ആകുമ്പോൾ((prev=length)>0);
    തിരികെ prev;
}

int32_t UnicodeSet::spanUTF8(സ്ഥിരം അക്ഷരം *s, int32_t length, USetSpanCondition spanCondition) സ്ഥിരം {
    എങ്കിൽ(length>0 && bmpSet!=nullptr) {
        സ്ഥിരം uint8_t* s0 = reinterpret_cast<സ്ഥിരം uint8_t*>(s);
        തിരികെ static_cast<int32_t>(bmpSet->spanUTF8(s0, length, spanCondition) - s0);
    }
    എങ്കിൽ(length<0) {
        length = static_cast<int32_t>(uprv_strlen(s));
    }
    എങ്കിൽ(length==0) {
        തിരികെ 0;
    }
    എങ്കിൽ(stringSpan!=nullptr) {
        തിരികെ stringSpan->spanUTF8(reinterpret_cast<സ്ഥിരം uint8_t*>(s), length, spanCondition);
    } അല്ലെങ്കിൽ എങ്കിൽ(hasStrings()) {
        uint32_t which= spanCondition==USET_SPAN_NOT_CONTAINED ?
                            UnicodeSetStringSpan::FWD_UTF8_NOT_CONTAINED :
                            UnicodeSetStringSpan::FWD_UTF8_CONTAINED;
        UnicodeSetStringSpan strSpan(*ഇത്, *strings_, which);
        എങ്കിൽ(strSpan.needsStringSpanUTF8()) {
            തിരികെ strSpan.spanUTF8(reinterpret_cast<സ്ഥിരം uint8_t*>(s), length, spanCondition);
        }
    }

    എങ്കിൽ(spanCondition!=USET_SPAN_NOT_CONTAINED) {
        spanCondition=USET_SPAN_CONTAINED;  // Pin to 0/1 values.
    }

    UChar32 c;
    int32_t start=0, prev=0;
    ചെയ്യുക {
        U8_NEXT_OR_FFFD(s, start, length, c);
        എങ്കിൽ(spanCondition!=contains(c)) {
            നിർത്തുക;
        }
    } ആകുമ്പോൾ((prev=start)<length);
    തിരികെ prev;
}

int32_t UnicodeSet::spanBackUTF8(സ്ഥിരം അക്ഷരം *s, int32_t length, USetSpanCondition spanCondition) സ്ഥിരം {
    എങ്കിൽ(length>0 && bmpSet!=nullptr) {
        സ്ഥിരം uint8_t* s0 = reinterpret_cast<സ്ഥിരം uint8_t*>(s);
        തിരികെ bmpSet->spanBackUTF8(s0, length, spanCondition);
    }
    എങ്കിൽ(length<0) {
        length = static_cast<int32_t>(uprv_strlen(s));
    }
    എങ്കിൽ(length==0) {
        തിരികെ 0;
    }
    എങ്കിൽ(stringSpan!=nullptr) {
        തിരികെ stringSpan->spanBackUTF8(reinterpret_cast<സ്ഥിരം uint8_t*>(s), length, spanCondition);
    } അല്ലെങ്കിൽ എങ്കിൽ(hasStrings()) {
        uint32_t which= spanCondition==USET_SPAN_NOT_CONTAINED ?
                            UnicodeSetStringSpan::BACK_UTF8_NOT_CONTAINED :
                            UnicodeSetStringSpan::BACK_UTF8_CONTAINED;
        UnicodeSetStringSpan strSpan(*ഇത്, *strings_, which);
        എങ്കിൽ(strSpan.needsStringSpanUTF8()) {
            തിരികെ strSpan.spanBackUTF8(reinterpret_cast<സ്ഥിരം uint8_t*>(s), length, spanCondition);
        }
    }

    എങ്കിൽ(spanCondition!=USET_SPAN_NOT_CONTAINED) {
        spanCondition=USET_SPAN_CONTAINED;  // Pin to 0/1 values.
    }

    UChar32 c;
    int32_t prev=length;
    ചെയ്യുക {
        U8_PREV_OR_FFFD(s, 0, length, c);
        എങ്കിൽ(spanCondition!=contains(c)) {
            നിർത്തുക;
        }
    } ആകുമ്പോൾ((prev=length)>0);
    തിരികെ prev;
}

U_NAMESPACE_END
