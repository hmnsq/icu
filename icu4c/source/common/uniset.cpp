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

#અન્તર્ભાવ "unicode/utypes.h"
#અન્તર્ભાવ "unicode/parsepos.h"
#અન્તર્ભાવ "unicode/symtable.h"
#અન્તર્ભાવ "unicode/uniset.h"
#અન્તર્ભાવ "unicode/ustring.h"
#અન્તર્ભાવ "unicode/utf8.h"
#અન્તર્ભાવ "unicode/utf16.h"
#અન્તર્ભાવ "ruleiter.h"
#અન્તર્ભાવ "cmemory.h"
#અન્તર્ભાવ "cstring.h"
#અન્તર્ભાવ "patternprops.h"
#અન્તર્ભાવ "uelement.h"
#અન્તર્ભાવ "util.h"
#અન્તર્ભાવ "uvector.h"
#અન્તર્ભાવ "charstr.h"
#અન્તર્ભાવ "ustrfmt.h"
#અન્તર્ભાવ "uassert.h"
#અન્તર્ભાવ "bmpset.h"
#અન્તર્ભાવ "unisetspan.h"

// HIGH_VALUE > all valid values. 110000 for codepoints
#define UNICODESET_ઉપલું 0x0110000

// LOW <= all valid values. ZERO for codepoints
#define UNICODESET_નીચલું 0x000000

/** Max list [0, 1, 2, ..., max code point, HIGH] */
constexpr int32_t MAX_LENGTH = UNICODESET_ઉપલું + 1;

U_NAMESPACE_BEGIN

પ્રતીક_Table::~પ્રતીક_Table() {}

UOBJECT_DEFINE_RTTI_IMPLEMENTATION(UnicodeSet)

/**
 * Modify the given UChar32 variable so that it is in range, by
 * pinning values < UNICODESET_LOW to UNICODESET_LOW, and
 * pinning values > UNICODESET_HIGH-1 to UNICODESET_HIGH-1.
 * It modifies its argument in-place and also returns it.
 */
સ્થિર અંતઃરેખ UChar32 pinCodePoint(UChar32& c) {
    જો (c < UNICODESET_નીચલું) {
        c = UNICODESET_નીચલું;
    } નહીંતર જો (c > (UNICODESET_ઉપલું-1)) {
        c = (UNICODESET_ઉપલું-1);
    }
    પાછા c;
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
#અન્તર્ભાવ <stdio.h>
સ્થિર int32_t dbg_ગણતરી = 0;

સ્થિર અંતઃરેખ શૂન્ય _dbgct(UnicodeSet* set) {
    UnicodeString str;
    set->toPattern(str, સાચું);
    અક્ષર buf[40];
    str.extract(0, 39, buf, "");
    printf("DEBUG UnicodeSet: ct 0x%08X; %d %s\n", set, ++dbg_ગણતરી, buf);
}

સ્થિર અંતઃરેખ શૂન્ય _dbgdt(UnicodeSet* set) {
    UnicodeString str;
    set->toPattern(str, સાચું);
    અક્ષર buf[40];
    str.extract(0, 39, buf, "");
    printf("DEBUG UnicodeSet: dt 0x%08X; %d %s\n", set, --dbg_ગણતરી, buf);
}

#નહીંતર

#define _dbgct(set)
#define _dbgdt(set)

#endif

//----------------------------------------------------------------
// UnicodeString in UVector support
//----------------------------------------------------------------

સ્થિર શૂન્ય U_CALLCONV cloneUnicodeString(UElement *dst, UElement *src) {
    dst->pointer = નવું UnicodeString(*static_cast<UnicodeString*>(src->pointer));
}

સ્થિર int32_t U_CALLCONV compareUnicodeString(UElement t1, UElement t2) {
    અચલ UnicodeString& a = *static_cast<અચલ UnicodeString*>(t1.pointer);
    અચલ UnicodeString& b = *static_cast<અચલ UnicodeString*>(t2.pointer);
    પાછા a.compare(b);
}

UBool UnicodeSet::hasStrings() અચલ {
    પાછા strings_ != nullptr && !strings_->isEmpty();
}

int32_t UnicodeSet::stringsSize() અચલ {
    પાછા strings_ == nullptr ? 0 : strings_->size();
}

UBool UnicodeSet::stringsContains(અચલ UnicodeString &s) અચલ {
    પાછા strings_ != nullptr && strings_->contains((શૂન્ય*) &s);
}

//----------------------------------------------------------------
// Constructors &c
//----------------------------------------------------------------

/**
 * Constructs an empty set.
 */
UnicodeSet::UnicodeSet() {
    list[0] = UNICODESET_ઉપલું;
    _dbgct(આ);
}

/**
 * Constructs a set containing the given range. If <code>end >
 * start</code> then an empty set is created.
 *
 * @param start first character, inclusive, of range
 * @param end last character, inclusive, of range
 */
UnicodeSet::UnicodeSet(UChar32 start, UChar32 end) {
    list[0] = UNICODESET_ઉપલું;
    add(start, end);
    _dbgct(આ);
}

/**
 * Constructs a set that is identical to the given UnicodeSet.
 */
UnicodeSet::UnicodeSet(અચલ UnicodeSet& o) : UnicodeFilter(o) {
    *આ = o;
    _dbgct(આ);
}

// Copy-construct as thawed.
UnicodeSet::UnicodeSet(અચલ UnicodeSet& o, UBool /* asThawed */) : UnicodeFilter(o) {
    જો (ensureCapacity(o.len)) {
        // *this = o except for bmpSet and stringSpan
        len = o.len;
        uprv_memcpy(list, o.list, (size_t)len*કદ(UChar32));
        જો (o.hasStrings()) {
            UErrorCode status = U_ZERO_ERROR;
            જો (!allocateStrings(status) ||
                    (strings_->assign(*o.strings_, cloneUnicodeString, status), U_FAILURE(status))) {
                setToBogus();
                પાછા;
            }
        }
        જો (o.pat) {
            setPattern(o.pat, o.patLen);
        }
        _dbgct(આ);
    }
}

/**
 * Destructs the set.
 */
UnicodeSet::~UnicodeSet() {
    _dbgdt(આ); // first!
    જો (list != ઢગલો_List) {
        uprv_free(list);
    }
    delete bmpSet;
    જો (buffer != ઢગલો_List) {
        uprv_free(buffer);
    }
    delete strings_;
    delete stringSpan;
    releasePattern();
}

/**
 * Assigns this object to be a copy of another.
 */
UnicodeSet& UnicodeSet::operator=(અચલ UnicodeSet& o) U_LIFETIME_BOUND {
    પાછા copyFrom(o, ખોટું);
}

UnicodeSet& UnicodeSet::copyFrom(અચલ UnicodeSet& o, UBool asThawed) U_LIFETIME_BOUND {
    જો (આ == &o) {
        પાછા *આ;
    }
    જો (isFrozen()) {
        પાછા *આ;
    }
    જો (o.isBogus()) {
        setToBogus();
        પાછા *આ;
    }
    જો (!ensureCapacity(o.len)) {
        // ensureCapacity will mark the UnicodeSet as Bogus if OOM failure happens.
        પાછા *આ;
    }
    len = o.len;
    uprv_memcpy(list, o.list, (size_t)len*કદ(UChar32));
    જો (o.bmpSet != nullptr && !asThawed) {
        bmpSet = નવું BMPSet(*o.bmpSet, list, len);
        જો (bmpSet == nullptr) { // Check for memory allocation error.
            setToBogus();
            પાછા *આ;
        }
    }
    જો (o.hasStrings()) {
        UErrorCode status = U_ZERO_ERROR;
        જો ((strings_ == nullptr && !allocateStrings(status)) ||
                (strings_->assign(*o.strings_, cloneUnicodeString, status), U_FAILURE(status))) {
            setToBogus();
            પાછા *આ;
        }
    } નહીંતર જો (hasStrings()) {
        strings_->removeAllElements();
    }
    જો (o.stringSpan != nullptr && strings_ != nullptr && !asThawed) {
        stringSpan = નવું UnicodeSetStringSpan(*o.stringSpan, *strings_);
        જો (stringSpan == nullptr) { // Check for memory allocation error.
            setToBogus();
            પાછા *આ;
        }
    }
    releasePattern();
    જો (o.pat) {
        setPattern(o.pat, o.patLen);
    }
    પાછા *આ;
}

/**
 * Returns a copy of this object.  All UnicodeMatcher objects have
 * to support cloning in order to allow classes using
 * UnicodeMatchers, such as Transliterator, to implement cloning.
 */
UnicodeSet* UnicodeSet::clone() અચલ {
    પાછા નવું UnicodeSet(*આ);
}

UnicodeSet *UnicodeSet::cloneAsThawed() અચલ {
    પાછા નવું UnicodeSet(*આ, સાચું);
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
તાર્કિક UnicodeSet::operator==(અચલ UnicodeSet& o) અચલ {
    જો (len != o.len) પાછા ખોટું;
    માટે (int32_t i = 0; i < len; ++i) {
        જો (list[i] != o.list[i]) પાછા ખોટું;
    }
    જો (hasStrings() != o.hasStrings()) { પાછા ખોટું; }
    જો (hasStrings() && *strings_ != *o.strings_) પાછા ખોટું;
    પાછા સાચું;
}

/**
 * Returns the hash code value for this set.
 *
 * @return the hash code value for this set.
 * @see Object#hashCode()
 */
int32_t UnicodeSet::hashCode() અચલ {
    uint32_t result = static_cast<uint32_t>(len);
    માટે (int32_t i = 0; i < len; ++i) {
        result *= 1000003u;
        result += list[i];
    }
    પાછા static_cast<int32_t>(result);
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
int32_t UnicodeSet::size() અચલ {
    int32_t n = 0;
    int32_t count = get_Range_ગણતરી();
    માટે (int32_t i = 0; i < count; ++i) {
        n += getRangeEnd(i) - getRangeStart(i) + 1;
    }
    પાછા n + stringsSize();
}

/**
 * Returns <tt>true</tt> if this set contains no elements.
 *
 * @return <tt>true</tt> if this set contains no elements.
 */
UBool UnicodeSet::isEmpty() અચલ {
    પાછા len == 1 && !hasStrings();
}

/**
 * Returns true if this set contains the given character.
 * @param c character to be checked for containment
 * @return true if the test condition is met
 */
UBool UnicodeSet::contains(UChar32 c) અચલ {
    // Set i to the index of the start item greater than ch
    // We know we will terminate without length test!
    // LATER: for large sets, add binary search
    //int32_t i = -1;
    //for (;;) {
    //    if (c < list[++i]) break;
    //}
    જો (bmpSet != nullptr) {
        પાછા bmpSet->contains(c);
    }
    જો (stringSpan != nullptr) {
        પાછા stringSpan->contains(c);
    }
    જો (c >= UNICODESET_ઉપલું) { // Don't need to check LOW bound
        પાછા ખોટું;
    }
    int32_t i = findCodePoint(c);
    પાછા i & 1; // return true if odd
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
int32_t UnicodeSet::findCodePoint(UChar32 c) અચલ {
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
    જો (c < list[0])
        પાછા 0;
    // High runner test.  c is often after the last range, so an
    // initial check for this condition pays off.
    int32_t lo = 0;
    int32_t hi = len - 1;
    જો (lo >= hi || c >= list[hi-1])
        પાછા hi;
    // invariant: c >= list[lo]
    // invariant: c < list[hi]
    માટે (;;) {
        int32_t i = (lo + hi) >> 1;
        જો (i == lo) {
            તોડો; // Found!
        } નહીંતર જો (c < list[i]) {
            hi = i;
        } નહીંતર {
            lo = i;
        }
    }
    પાછા hi;
}

/**
 * Returns true if this set contains every character
 * of the given range.
 * @param start first character, inclusive, of the range
 * @param end last character, inclusive, of the range
 * @return true if the test condition is met
 */
UBool UnicodeSet::contains(UChar32 start, UChar32 end) અચલ {
    //int32_t i = -1;
    //for (;;) {
    //    if (start < list[++i]) break;
    //}
    int32_t i = findCodePoint(start);
    પાછા ((i & 1) != 0 && end < list[i]);
}

/**
 * Returns <tt>true</tt> if this set contains the given
 * multicharacter string.
 * @param s string to be checked for containment
 * @return <tt>true</tt> if this set contains the specified string
 */
UBool UnicodeSet::contains(અચલ UnicodeString& s) અચલ {
    int32_t cp = getSingleCP(s);
    જો (cp < 0) {
        પાછા stringsContains(s);
    } નહીંતર {
        પાછા contains(static_cast<UChar32>(cp));
    }
}

/**
 * Returns true if this set contains all the characters and strings
 * of the given set.
 * @param c set to be checked for containment
 * @return true if the test condition is met
 */
UBool UnicodeSet::containsAll(અચલ UnicodeSet& c) અચલ {
    // The specified set is a subset if all of its pairs are contained in
    // this set.  It's possible to code this more efficiently in terms of
    // direct manipulation of the inversion lists if the need arises.
    int32_t n = c.get_Range_ગણતરી();
    માટે (પૂર્ણાંક i=0; i<n; ++i) {
        જો (!contains(c.getRangeStart(i), c.getRangeEnd(i))) {
            પાછા ખોટું;
        }
    }
    પાછા !c.hasStrings() || (strings_ != nullptr && strings_->containsAll(*c.strings_));
}

/**
 * Returns true if this set contains all the characters
 * of the given string.
 * @param s string containing characters to be checked for containment
 * @return true if the test condition is met
 */
UBool UnicodeSet::containsAll(અચલ UnicodeString& s) અચલ {
    પાછા span(s.getBuffer(), s.length(), USET_SPAN_CONTAINED) == s.length();
}

/**
 * Returns true if this set contains none of the characters
 * of the given range.
 * @param start first character, inclusive, of the range
 * @param end last character, inclusive, of the range
 * @return true if the test condition is met
 */
UBool UnicodeSet::containsNone(UChar32 start, UChar32 end) અચલ {
    //int32_t i = -1;
    //for (;;) {
    //    if (start < list[++i]) break;
    //}
    int32_t i = findCodePoint(start);
    પાછા ((i & 1) == 0 && end < list[i]);
}

/**
 * Returns true if this set contains none of the characters and strings
 * of the given set.
 * @param c set to be checked for containment
 * @return true if the test condition is met
 */
UBool UnicodeSet::containsNone(અચલ UnicodeSet& c) અચલ {
    // The specified set is a subset if all of its pairs are contained in
    // this set.  It's possible to code this more efficiently in terms of
    // direct manipulation of the inversion lists if the need arises.
    int32_t n = c.get_Range_ગણતરી();
    માટે (int32_t i=0; i<n; ++i) {
        જો (!containsNone(c.getRangeStart(i), c.getRangeEnd(i))) {
            પાછા ખોટું;
        }
    }
    પાછા strings_ == nullptr || !c.hasStrings() || strings_->containsNone(*c.strings_);
}

/**
 * Returns true if this set contains none of the characters
 * of the given string.
 * @param s string containing characters to be checked for containment
 * @return true if the test condition is met
 */
UBool UnicodeSet::containsNone(અચલ UnicodeString& s) અચલ {
    પાછા span(s.getBuffer(), s.length(), USET_SPAN_NOT_CONTAINED) == s.length();
}

/**
 * Returns <tt>true</tt> if this set contains any character whose low byte
 * is the given value.  This is used by <tt>RuleBasedTransliterator</tt> for
 * indexing.
 */
UBool UnicodeSet::matches_Index_કિંમત(uint8_t v) અચલ {
    /* The index value v, in the range [0,255], is contained in this set if
     * it is contained in any pair of this set.  Pairs either have the high
     * bytes equal, or unequal.  If the high bytes are equal, then we have
     * aaxx..aayy, where aa is the high byte.  Then v is contained if xx <=
     * v <= yy.  If the high bytes are unequal we have aaxx..bbyy, bb>aa.
     * Then v is contained if xx <= v || v <= yy.  (This is identical to the
     * time zone month containment logic.)
     */
    int32_t i;
    int32_t range_ગણતરી=get_Range_ગણતરી();
    માટે (i=0; i<range_ગણતરી; ++i) {
        UChar32 નીચલું = getRangeStart(i);
        UChar32 ઉપલું = getRangeEnd(i);
        જો ((નીચલું & ~0xFF) == (ઉપલું & ~0xFF)) {
            જો ((નીચલું & 0xFF) <= v && v <= (ઉપલું & 0xFF)) {
                પાછા સાચું;
            }
        } નહીંતર જો ((નીચલું & 0xFF) <= v || v <= (ઉપલું & 0xFF)) {
            પાછા સાચું;
        }
    }
    જો (hasStrings()) {
        માટે (i=0; i<strings_->size(); ++i) {
            અચલ UnicodeString& s = *static_cast<અચલ UnicodeString*>(strings_->elementAt(i));
            જો (s.isEmpty()) {
                ચાલુ;  // skip the empty string
            }
            UChar32 c = s.char32At(0);
            જો ((c & 0xFF) == v) {
                પાછા સાચું;
            }
        }
    }
    પાછા ખોટું;
}

/**
 * Implementation of UnicodeMatcher::matches().  Always matches the
 * longest possible multichar string.
 */
UMatchDegree UnicodeSet::matches(અચલ Replaceable& text,
                                 int32_t& offset,
                                 int32_t limit,
                                 UBool incremental) {
    જો (offset == limit) {
        જો (contains(U_ETHER)) {
            પાછા incremental ? U_PARTIAL_MATCH : U_MATCH;
        } નહીંતર {
            પાછા U_MISMATCH;
        }
    } નહીંતર {
        જો (hasStrings()) { // try strings first

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
            int32_t ઉપલું_Water_Length = 0;

            માટે (i=0; i<strings_->size(); ++i) {
                અચલ UnicodeString& trial = *static_cast<અચલ UnicodeString*>(strings_->elementAt(i));
                જો (trial.isEmpty()) {
                    ચાલુ;  // skip the empty string
                }

                char16_t c = trial.charAt(forward ? 0 : trial.length() - 1);

                // Strings are sorted, so we can optimize in the
                // forward direction.
                જો (forward && c > firstChar) તોડો;
                જો (c != firstChar) ચાલુ;

                int32_t matchLen = matchRest(text, offset, limit, trial);

                જો (incremental) {
                    int32_t maxLen = forward ? limit-offset : offset-limit;
                    જો (matchLen == maxLen) {
                        // We have successfully matched but only up to limit.
                        પાછા U_PARTIAL_MATCH;
                    }
                }

                જો (matchLen == trial.length()) {
                    // We have successfully matched the whole string.
                    જો (matchLen > ઉપલું_Water_Length) {
                        ઉપલું_Water_Length = matchLen;
                    }
                    // In the forward direction we know strings
                    // are sorted so we can bail early.
                    જો (forward && matchLen < ઉપલું_Water_Length) {
                        તોડો;
                    }
                    ચાલુ;
                }
            }

            // We've checked all strings without a partial match.
            // If we have full matches, return the longest one.
            જો (ઉપલું_Water_Length != 0) {
                offset += forward ? ઉપલું_Water_Length : -ઉપલું_Water_Length;
                પાછા U_MATCH;
            }
        }
        પાછા UnicodeFilter::matches(text, offset, limit, incremental);
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
int32_t UnicodeSet::matchRest(અચલ Replaceable& text,
                              int32_t start, int32_t limit,
                              અચલ UnicodeString& s) {
    int32_t i;
    int32_t maxLen;
    int32_t slen = s.length();
    જો (start < limit) {
        maxLen = limit - start;
        જો (maxLen > slen) maxLen = slen;
        માટે (i = 1; i < maxLen; ++i) {
            જો (text.charAt(start + i) != s.charAt(i)) પાછા 0;
        }
    } નહીંતર {
        maxLen = start - limit;
        જો (maxLen > slen) maxLen = slen;
        --slen; // <=> slen = s.length() - 1;
        માટે (i = 1; i < maxLen; ++i) {
            જો (text.charAt(start - i) != s.charAt(slen - i)) પાછા 0;
        }
    }
    પાછા maxLen;
}

/**
 * Implement of UnicodeMatcher
 */
શૂન્ય UnicodeSet::addMatchSetTo(UnicodeSet& toUnionTo) અચલ {
    toUnionTo.addAll(*આ);
}

/**
 * Returns the index of the given character within this set, where
 * the set is ordered by ascending code point.  If the character
 * is not in this set, return -1.  The inverse of this method is
 * <code>charAt()</code>.
 * @return an index from 0..size()-1, or -1
 */
int32_t UnicodeSet::indexOf(UChar32 c) અચલ {
    જો (c < MIN_કિંમત || c > MAX_કિંમત) {
        પાછા -1;
    }
    int32_t i = 0;
    int32_t n = 0;
    માટે (;;) {
        UChar32 start = list[i++];
        જો (c < start) {
            પાછા -1;
        }
        UChar32 limit = list[i++];
        જો (c < limit) {
            પાછા n + c - start;
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
UChar32 UnicodeSet::charAt(int32_t index) અચલ {
    જો (index >= 0) {
        // len2 is the largest even integer <= len, that is, it is len
        // for even values and len-1 for odd values.  With odd values
        // the last entry is UNICODESET_HIGH.
        int32_t len2 = len & ~1;
        માટે (int32_t i=0; i < len2;) {
            UChar32 start = list[i++];
            int32_t count = list[i++] - start;
            જો (index < count) {
                પાછા static_cast<UChar32>(start + index);
            }
            index -= count;
        }
    }
    પાછા static_cast<UChar32>(-1);
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
    પાછા *આ;
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
    જો (pinCodePoint(start) < pinCodePoint(end)) {
        UChar32 limit = end + 1;
        // Fast path for adding a new range after the last one.
        // Odd list length: [..., lastStart, lastLimit, HIGH]
        જો ((len & 1) != 0) {
            // If the list is empty, set lastLimit low enough to not be adjacent to 0.
            UChar32 lastLimit = len == 1 ? -2 : list[len - 2];
            જો (lastLimit <= start && !isFrozen() && !isBogus()) {
                જો (lastLimit == start) {
                    // Extend the last range.
                    list[len - 2] = limit;
                    જો (limit == UNICODESET_ઉપલું) {
                        --len;
                    }
                } નહીંતર {
                    list[len - 1] = start;
                    જો (limit < UNICODESET_ઉપલું) {
                        જો (ensureCapacity(len + 2)) {
                            list[len++] = limit;
                            list[len++] = UNICODESET_ઉપલું;
                        }
                    } નહીંતર {  // limit == UNICODESET_HIGH
                        જો (ensureCapacity(len + 1)) {
                            list[len++] = UNICODESET_ઉપલું;
                        }
                    }
                }
                releasePattern();
                પાછા *આ;
            }
        }
        // This is slow. Could be much faster using findCodePoint(start)
        // and modifying the list, dealing with adjacent & overlapping ranges.
        UChar32 range[3] = { start, limit, UNICODESET_ઉપલું };
        add(range, 2, 0);
    } નહીંતર જો (start == end) {
        add(start);
    }
    પાછા *આ;
}

// #define DEBUG_US_ADD

#ifdef DEBUG_US_ADD
#અન્તર્ભાવ <stdio.h>
શૂન્ય dump(UChar32 c) {
    જો (c <= 0xFF) {
        printf("%c", (અક્ષર)c);
    } નહીંતર {
        printf("U+%04X", c);
    }
}
શૂન્ય dump(અચલ UChar32* list, int32_t len) {
    printf("[");
    માટે (int32_t i=0; i<len; ++i) {
        જો (i != 0) printf(", ");
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
    જો ((i & 1) != 0  || isFrozen() || isBogus()) પાછા *આ;

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

    જો (c == list[i]-1) {
        // c is before start of next range
        list[i] = c;
        // if we touched the HIGH mark, then add a new one
        જો (c == (UNICODESET_ઉપલું - 1)) {
            જો (!ensureCapacity(len+1)) {
                // ensureCapacity will mark the object as Bogus if OOM failure happens.
                પાછા *આ;
            }
            list[len++] = UNICODESET_ઉપલું;
        }
        જો (i > 0 && c == list[i-1]) {
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
            જ્યારે (src < srclimit) *(dst++) = *(src++);

            len -= 2;
        }
    }

    નહીંતર જો (i > 0 && c == list[i-1]) {
        // c is after end of prior range
        list[i-1]++;
        // no need to check for collapse here
    }

    નહીંતર {
        // At this point we know the new char is not adjacent to
        // any existing ranges, and it is not 10FFFF.


        // [..., start_k-1, limit_k-1, start_k, limit_k, ..., HIGH]
        //                             ^
        //                             list[i]

        // [..., start_k-1, limit_k-1, c, c+1, start_k, limit_k, ..., HIGH]
        //                             ^
        //                             list[i]

        જો (!ensureCapacity(len+2)) {
            // ensureCapacity will mark the object as Bogus if OOM failure happens.
            પાછા *આ;
        }

        UChar32 *p = list + i;
        uprv_memmove(p + 2, p, (len - i) * કદ(*p));
        list[i] = c;
        list[i+1] = c+1;
        len += 2;
    }

#ifdef DEBUG_US_ADD
    dump(list, len);
    printf("\n");

    માટે (i=1; i<len; ++i) {
        જો (list[i] <= list[i-1]) {
            // Corrupt array!
            printf("ERROR: list has been corrupted\n");
            exit(1);
        }
    }
#endif

    releasePattern();
    પાછા *આ;
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
UnicodeSet& UnicodeSet::add(અચલ UnicodeString& s) U_LIFETIME_BOUND {
    જો (isFrozen() || isBogus()) પાછા *આ;
    int32_t cp = getSingleCP(s);
    જો (cp < 0) {
        જો (!stringsContains(s)) {
            _add(s);
            releasePattern();
        }
    } નહીંતર {
        add(static_cast<UChar32>(cp));
    }
    પાછા *આ;
}

/**
 * Adds the given string, in order, to 'strings_'.  The given string
 * must have been checked by the caller to not already be in 'strings_'.
 */
શૂન્ય UnicodeSet::_add(અચલ UnicodeString& s) {
    જો (isFrozen() || isBogus()) {
        પાછા;
    }
    UErrorCode ec = U_ZERO_ERROR;
    જો (strings_ == nullptr && !allocateStrings(ec)) {
        setToBogus();
        પાછા;
    }
    LocalPointer<UnicodeString> t(નવું UnicodeString(s));
    જો (t.isNull()) { // Check for memory allocation error.
        setToBogus();
        પાછા;
    }
    strings_->sorted_ઉમેરો(t.orphan(), compareUnicodeString, ec);
    જો (U_FAILURE(ec)) {
        setToBogus();
    }
}

/**
 * @return a code point IF the string consists of a single one.
 * otherwise returns -1.
 * @param string to test
 */
int32_t UnicodeSet::getSingleCP(અચલ UnicodeString& s) {
    int32_t sLength = s.length();
    જો (sLength == 1) પાછા s.charAt(0);
    જો (sLength == 2) {
        UChar32 cp = s.char32At(0);
        જો (cp > 0xFFFF) { // is surrogate pair
            પાછા cp;
        }
    }
    પાછા -1;
}

/**
 * Adds each of the characters in this string to the set. Thus "ch" => {"c", "h"}
 * If this set already any particular character, it has no effect on that character.
 * @param the source string
 * @return the modified set, for chaining
 */
UnicodeSet& UnicodeSet::addAll(અચલ UnicodeString& s) U_LIFETIME_BOUND {
    UChar32 cp;
    માટે (int32_t i = 0; i < s.length(); i += U16_LENGTH(cp)) {
        cp = s.char32At(i);
        add(cp);
    }
    પાછા *આ;
}

/**
 * Retains EACH of the characters in this string. Note: "ch" == {"c", "h"}
 * If this set already any particular character, it has no effect on that character.
 * @param the source string
 * @return the modified set, for chaining
 */
UnicodeSet& UnicodeSet::retainAll(અચલ UnicodeString& s) U_LIFETIME_BOUND {
    UnicodeSet set;
    set.addAll(s);
    retainAll(set);
    પાછા *આ;
}

/**
 * Complement EACH of the characters in this string. Note: "ch" == {"c", "h"}
 * If this set already any particular character, it has no effect on that character.
 * @param the source string
 * @return the modified set, for chaining
 */
UnicodeSet& UnicodeSet::complementAll(અચલ UnicodeString& s) U_LIFETIME_BOUND {
    UnicodeSet set;
    set.addAll(s);
    complementAll(set);
    પાછા *આ;
}

/**
 * Remove EACH of the characters in this string. Note: "ch" == {"c", "h"}
 * If this set already any particular character, it has no effect on that character.
 * @param the source string
 * @return the modified set, for chaining
 */
UnicodeSet& UnicodeSet::removeAll(અચલ UnicodeString& s) U_LIFETIME_BOUND {
    UnicodeSet set;
    set.addAll(s);
    removeAll(set);
    પાછા *આ;
}

UnicodeSet& UnicodeSet::removeAllStrings() U_LIFETIME_BOUND {
    જો (!isFrozen() && hasStrings()) {
        strings_->removeAllElements();
        releasePattern();
    }
    પાછા *આ;
}


/**
 * Makes a set from a multicharacter string. Thus "ch" => {"ch"}
 * <br><b>Warning: you cannot add an empty string ("") to a UnicodeSet.</b>
 * @param the source string
 * @return a newly created set containing the given string
 */
UnicodeSet* U_EXPORT2 UnicodeSet::createFrom(અચલ UnicodeString& s) {
    UnicodeSet *set = નવું UnicodeSet();
    જો (set != nullptr) { // Check for memory allocation error.
        set->add(s);
    }
    પાછા set;
}


/**
 * Makes a set from each of the characters in the string. Thus "ch" => {"c", "h"}
 * @param the source string
 * @return a newly created set containing the given characters
 */
UnicodeSet* U_EXPORT2 UnicodeSet::createFromAll(અચલ UnicodeString& s) {
    UnicodeSet *set = નવું UnicodeSet();
    જો (set != nullptr) { // Check for memory allocation error.
        set->addAll(s);
    }
    પાછા set;
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
    જો (pinCodePoint(start) <= pinCodePoint(end)) {
        UChar32 range[3] = { start, end+1, UNICODESET_ઉપલું };
        retain(range, 2, 0);
    } નહીંતર {
        clear();
    }
    પાછા *આ;
}

UnicodeSet& UnicodeSet::retain(UChar32 c) U_LIFETIME_BOUND {
    પાછા retain(c, c);
}

UnicodeSet& UnicodeSet::retain(અચલ UnicodeString &s) U_LIFETIME_BOUND {
    જો (isFrozen() || isBogus()) { પાછા *આ; }
    UChar32 cp = getSingleCP(s);
    જો (cp < 0) {
        તાર્કિક isIn = stringsContains(s);
        // Check for getRangeCount() first to avoid somewhat-expensive size()
        // when there are single code points.
        જો (isIn && get_Range_ગણતરી() == 0 && size() == 1) {
            પાછા *આ;
        }
        clear();
        જો (isIn) {
            _add(s);
        }
    } નહીંતર {
        retain(cp, cp);
    }
    પાછા *આ;
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
    જો (pinCodePoint(start) <= pinCodePoint(end)) {
        UChar32 range[3] = { start, end+1, UNICODESET_ઉપલું };
        retain(range, 2, 2);
    }
    પાછા *આ;
}

/**
 * Removes the specified character from this set if it is present.
 * The set will not contain the specified range once the call
 * returns.
 */
UnicodeSet& UnicodeSet::remove(UChar32 c) U_LIFETIME_BOUND {
    પાછા remove(c, c);
}

/**
 * Removes the specified string from this set if it is present.
 * The set will not contain the specified character once the call
 * returns.
 * @param the source string
 * @return the modified set, for chaining
 */
UnicodeSet& UnicodeSet::remove(અચલ UnicodeString& s) U_LIFETIME_BOUND {
    જો (isFrozen() || isBogus()) પાછા *આ;
    int32_t cp = getSingleCP(s);
    જો (cp < 0) {
        જો (strings_ != nullptr && strings_->removeElement((શૂન્ય*) &s)) {
            releasePattern();
        }
    } નહીંતર {
        remove(static_cast<UChar32>(cp), static_cast<UChar32>(cp));
    }
    પાછા *આ;
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
    જો (isFrozen() || isBogus()) {
        પાછા *આ;
    }
    જો (pinCodePoint(start) <= pinCodePoint(end)) {
        UChar32 range[3] = { start, end+1, UNICODESET_ઉપલું };
        exclusiveOr(range, 2, 0);
    }
    releasePattern();
    પાછા *આ;
}

UnicodeSet& UnicodeSet::complement(UChar32 c) U_LIFETIME_BOUND {
    પાછા complement(c, c);
}

/**
 * This is equivalent to
 * <code>complement(MIN_VALUE, MAX_VALUE)</code>.
 */
UnicodeSet& UnicodeSet::complement() U_LIFETIME_BOUND {
    જો (isFrozen() || isBogus()) {
        પાછા *આ;
    }
    જો (list[0] == UNICODESET_નીચલું) {
        uprv_memmove(list, list + 1, (size_t)(len-1)*કદ(UChar32));
        --len;
    } નહીંતર {
        જો (!ensureCapacity(len+1)) {
            પાછા *આ;
        }
        uprv_memmove(list + 1, list, (size_t)len*કદ(UChar32));
        list[0] = UNICODESET_નીચલું;
        ++len;
    }
    releasePattern();
    પાછા *આ;
}

/**
 * Complement the specified string in this set.
 * The set will not contain the specified string once the call
 * returns.
 *
 * @param s the string to complement
 * @return this object, for chaining
 */
UnicodeSet& UnicodeSet::complement(અચલ UnicodeString& s) U_LIFETIME_BOUND {
    જો (isFrozen() || isBogus()) પાછા *આ;
    int32_t cp = getSingleCP(s);
    જો (cp < 0) {
        જો (stringsContains(s)) {
            strings_->removeElement((શૂન્ય*) &s);
        } નહીંતર {
            _add(s);
        }
        releasePattern();
    } નહીંતર {
        complement(static_cast<UChar32>(cp), static_cast<UChar32>(cp));
    }
    પાછા *આ;
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
UnicodeSet& UnicodeSet::addAll(અચલ UnicodeSet& c) U_LIFETIME_BOUND {
    જો ( c.len>0 && c.list!=nullptr ) {
        add(c.list, c.len, 0);
    }

    // Add strings in order
    જો ( c.strings_!=nullptr ) {
        માટે (int32_t i=0; i<c.strings_->size(); ++i) {
            અચલ UnicodeString* s = static_cast<અચલ UnicodeString*>(c.strings_->elementAt(i));
            જો (!stringsContains(*s)) {
                _add(*s);
            }
        }
    }
    પાછા *આ;
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
UnicodeSet& UnicodeSet::retainAll(અચલ UnicodeSet& c) U_LIFETIME_BOUND {
    જો (isFrozen() || isBogus()) {
        પાછા *આ;
    }
    retain(c.list, c.len, 0);
    જો (hasStrings()) {
        જો (!c.hasStrings()) {
            strings_->removeAllElements();
        } નહીંતર {
            strings_->retainAll(*c.strings_);
        }
    }
    પાછા *આ;
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
UnicodeSet& UnicodeSet::removeAll(અચલ UnicodeSet& c) U_LIFETIME_BOUND {
    જો (isFrozen() || isBogus()) {
        પાછા *આ;
    }
    retain(c.list, c.len, 2);
    જો (hasStrings() && c.hasStrings()) {
        strings_->removeAll(*c.strings_);
    }
    પાછા *આ;
}

/**
 * Complements in this set all elements contained in the specified
 * set.  Any character in the other set will be removed if it is
 * in this set, or will be added if it is not in this set.
 *
 * @param c set that defines which elements will be xor'ed from
 *          this set.
 */
UnicodeSet& UnicodeSet::complementAll(અચલ UnicodeSet& c) U_LIFETIME_BOUND {
    જો (isFrozen() || isBogus()) {
        પાછા *આ;
    }
    exclusiveOr(c.list, c.len, 0);

    જો (c.strings_ != nullptr) {
        માટે (int32_t i=0; i<c.strings_->size(); ++i) {
            શૂન્ય* e = c.strings_->elementAt(i);
            જો (strings_ == nullptr || !strings_->removeElement(e)) {
                _add(*static_cast<અચલ UnicodeString*>(e));
            }
        }
    }
    પાછા *આ;
}

/**
 * Removes all of the elements from this set.  This set will be
 * empty after this call returns.
 */
UnicodeSet& UnicodeSet::clear() U_LIFETIME_BOUND {
    જો (isFrozen()) {
        પાછા *આ;
    }
    list[0] = UNICODESET_ઉપલું;
    len = 1;
    releasePattern();
    જો (strings_ != nullptr) {
        strings_->removeAllElements();
    }
    // Remove bogus
    fFlags = 0;
    પાછા *આ;
}

/**
 * Iteration method that returns the number of ranges contained in
 * this set.
 * @see #getRangeStart
 * @see #getRangeEnd
 */
int32_t UnicodeSet::get_Range_ગણતરી() અચલ {
    પાછા len/2;
}

/**
 * Iteration method that returns the first character in the
 * specified range of this set.
 * @see #getRangeCount
 * @see #getRangeEnd
 */
UChar32 UnicodeSet::getRangeStart(int32_t index) અચલ {
    પાછા list[index*2];
}

/**
 * Iteration method that returns the last character in the
 * specified range of this set.
 * @see #getRangeStart
 * @see #getRangeEnd
 */
UChar32 UnicodeSet::getRangeEnd(int32_t index) અચલ {
    પાછા list[index*2 + 1] - 1;
}

અચલ UnicodeString* UnicodeSet::getString(int32_t index) અચલ U_LIFETIME_BOUND {
    પાછા static_cast<અચલ UnicodeString*>(strings_->elementAt(index));
}

/**
 * Reallocate this objects internal structures to take up the least
 * possible space, without changing this object's value.
 */
UnicodeSet& UnicodeSet::compact() U_LIFETIME_BOUND {
    જો (isFrozen() || isBogus()) {
        પાછા *આ;
    }
    // Delete buffer first to defragment memory less.
    જો (buffer != ઢગલો_List) {
        uprv_free(buffer);
        buffer = nullptr;
        bufferCapacity = 0;
    }
    જો (list == ઢગલો_List) {
        // pass
    } નહીંતર જો (len <= INITIAL_CAPACITY) {
        uprv_memcpy(ઢગલો_List, list, len * કદ(UChar32));
        uprv_free(list);
        list = ઢગલો_List;
        capacity = INITIAL_CAPACITY;
    } નહીંતર જો ((len + 7) < capacity) {
        // If we have more than a little unused capacity, shrink it to len.
        UChar32* temp = static_cast<UChar32*>(uprv_realloc(list, કદ(UChar32) * len));
        જો (temp) {
            list = temp;
            capacity = len;
        }
        // else what the heck happened?! We allocated less memory!
        // Oh well. We'll keep our original array.
    }
    જો (strings_ != nullptr && strings_->isEmpty()) {
        delete strings_;
        strings_ = nullptr;
    }
    પાછા *આ;
}

#ifdef DEBUG_SERIALIZE
#અન્તર્ભાવ <stdio.h>
#endif

/**
 * Deserialize constructor.
 */
UnicodeSet::UnicodeSet(અચલ uint16_t data[], int32_t dataLen, ESerialization serialization,
                       UErrorCode &ec) {

  જો(U_FAILURE(ec)) {
    setToBogus();
    પાછા;
  }

  જો( (serialization != kSerialized)
      || (data==nullptr)
      || (dataLen < 1)) {
    ec = U_ILLEGAL_ARGUMENT_ERROR;
    setToBogus();
    પાછા;
  }

  // bmp?
  int32_t headerSize = ((data[0]&0x8000)) ?2:1;
  int32_t bmpLength = (headerSize==1)?data[0]:data[1];

  int32_t newLength = (((data[0]&0x7FFF)-bmpLength)/2)+bmpLength;
#ifdef DEBUG_SERIALIZE
  printf("dataLen %d headerSize %d bmpLen %d len %d. data[0]=%X/%X/%X/%X\n", dataLen,headerSize,bmpLength,newLength, data[0],data[1],data[2],data[3]);
#endif
  જો(!ensureCapacity(newLength + 1)) {  // +1 for HIGH
    પાછા;
  }
  // copy bmp
  int32_t i;
  માટે(i = 0; i< bmpLength;i++) {
    list[i] = data[i+headerSize];
#ifdef DEBUG_SERIALIZE
    printf("<<16@%d[%d] %X\n", i+headerSize, i, list[i]);
#endif
  }
  // copy smp
  માટે(i=bmpLength;i<newLength;i++) {
    list[i] = (static_cast<UChar32>(data[headerSize + bmpLength + (i - bmpLength) * 2 + 0]) << 16) +
               static_cast<UChar32>(data[headerSize + bmpLength + (i - bmpLength) * 2 + 1]);
#ifdef DEBUG_SERIALIZE
    printf("<<32@%d+[%d] %lX\n", headerSize+bmpLength+i, i, list[i]);
#endif
  }
  U_ASSERT(i == newLength);
  જો (i == 0 || list[i - 1] != UNICODESET_ઉપલું) {
    list[i++] = UNICODESET_ઉપલું;
  }
  len = i;
}


int32_t UnicodeSet::serialize(uint16_t *dest, int32_t destCapacity, UErrorCode& ec) અચલ {
    int32_t bmpLength, length, destLength;

    જો (U_FAILURE(ec)) {
        પાછા 0;
    }

    જો (destCapacity<0 || (destCapacity>0 && dest==nullptr)) {
        ec=U_ILLEGAL_ARGUMENT_ERROR;
        પાછા 0;
    }

    /* count necessary 16-bit units */
    length=આ->len-1; // Subtract 1 to ignore final UNICODESET_HIGH
    // assert(length>=0);
    જો (length==0) {
        /* empty set */
        જો (destCapacity>0) {
            *dest=0;
        } નહીંતર {
            ec=U_BUFFER_OVERFLOW_ERROR;
        }
        પાછા 1;
    }
    /* now length>0 */

    જો (આ->list[length-1]<=0xffff) {
        /* all BMP */
        bmpLength=length;
    } નહીંતર જો (આ->list[0]>=0x10000) {
        /* all supplementary */
        bmpLength=0;
        length*=2;
    } નહીંતર {
        /* some BMP, some supplementary */
        માટે (bmpLength=0; bmpLength<length && આ->list[bmpLength]<=0xffff; ++bmpLength) {}
        length=bmpLength+2*(length-bmpLength);
    }
#ifdef DEBUG_SERIALIZE
    printf(">> bmpLength%d length%d len%d\n", bmpLength, length, len);
#endif
    /* length: number of 16-bit array units */
    જો (length>0x7fff) {
        /* there are only 15 bits for the length in the first serialized word */
        ec=U_INDEX_OUTOFBOUNDS_ERROR;
        પાછા 0;
    }

    /*
     * total serialized length:
     * number of 16-bit array units (length) +
     * 1 length unit (always) +
     * 1 bmpLength unit (if there are supplementary values)
     */
    destLength=length+((length>bmpLength)?2:1);
    જો (destLength<=destCapacity) {
        અચલ UChar32 *p;
        int32_t i;

#ifdef DEBUG_SERIALIZE
        printf("writeHdr\n");
#endif
        *dest = static_cast<uint16_t>(length);
        જો (length>bmpLength) {
            *dest|=0x8000;
            *++dest = static_cast<uint16_t>(bmpLength);
        }
        ++dest;

        /* write the BMP part of the array */
        p=આ->list;
        માટે (i=0; i<bmpLength; ++i) {
#ifdef DEBUG_SERIALIZE
          printf("writebmp: %x\n", (પૂર્ણાંક)*p);
#endif
            *dest++ = static_cast<uint16_t>(*p++);
        }

        /* write the supplementary part of the array */
        માટે (; i<length; i+=2) {
#ifdef DEBUG_SERIALIZE
          printf("write32: %x\n", (પૂર્ણાંક)*p);
#endif
            *dest++ = static_cast<uint16_t>(*p >> 16);
            *dest++ = static_cast<uint16_t>(*p++);
        }
    } નહીંતર {
        ec=U_BUFFER_OVERFLOW_ERROR;
    }
    પાછા destLength;
}

//----------------------------------------------------------------
// Implementation: Utility methods
//----------------------------------------------------------------

/**
 * Allocate our strings vector and return true if successful.
 */
UBool UnicodeSet::allocateStrings(UErrorCode &status) {
    જો (U_FAILURE(status)) {
        પાછા ખોટું;
    }
    strings_ = નવું UVector(uprv_deleteUObject,
                          uhash_compareUnicodeString, 1, status);
    જો (strings_ == nullptr) { // Check for memory allocation error.
        status = U_MEMORY_ALLOCATION_ERROR;
        પાછા ખોટું;
    }
    જો (U_FAILURE(status)) {
        delete strings_;
        strings_ = nullptr;
        પાછા ખોટું;
    } 
    પાછા સાચું;
}

int32_t UnicodeSet::nextCapacity(int32_t minCapacity) {
    // Grow exponentially to reduce the frequency of allocations.
    જો (minCapacity < INITIAL_CAPACITY) {
        પાછા minCapacity + INITIAL_CAPACITY;
    } નહીંતર જો (minCapacity <= 2500) {
        પાછા 5 * minCapacity;
    } નહીંતર {
        int32_t newCapacity = 2 * minCapacity;
        જો (newCapacity > MAX_LENGTH) {
            newCapacity = MAX_LENGTH;
        }
        પાછા newCapacity;
    }
}

તાર્કિક UnicodeSet::ensureCapacity(int32_t newLen) {
    જો (newLen > MAX_LENGTH) {
        newLen = MAX_LENGTH;
    }
    જો (newLen <= capacity) {
        પાછા સાચું;
    }
    int32_t newCapacity = nextCapacity(newLen);
    UChar32* temp = static_cast<UChar32*>(uprv_malloc(newCapacity * કદ(UChar32)));
    જો (temp == nullptr) {
        setToBogus(); // set the object to bogus state if an OOM failure occurred.
        પાછા ખોટું;
    }
    // Copy only the actual contents.
    uprv_memcpy(temp, list, len * કદ(UChar32));
    જો (list != ઢગલો_List) {
        uprv_free(list);
    }
    list = temp;
    capacity = newCapacity;
    પાછા સાચું;
}

તાર્કિક UnicodeSet::ensureBufferCapacity(int32_t newLen) {
    જો (newLen > MAX_LENGTH) {
        newLen = MAX_LENGTH;
    }
    જો (newLen <= bufferCapacity) {
        પાછા સાચું;
    }
    int32_t newCapacity = nextCapacity(newLen);
    UChar32* temp = static_cast<UChar32*>(uprv_malloc(newCapacity * કદ(UChar32)));
    જો (temp == nullptr) {
        setToBogus();
        પાછા ખોટું;
    }
    // The buffer has no contents to be copied.
    // It is always filled from scratch after this call.
    જો (buffer != ઢગલો_List) {
        uprv_free(buffer);
    }
    buffer = temp;
    bufferCapacity = newCapacity;
    પાછા સાચું;
}

/**
 * Swap list and buffer.
 */
શૂન્ય UnicodeSet::swapBuffers() {
    // swap list and buffer
    UChar32* temp = list;
    list = buffer;
    buffer = temp;

    int32_t c = capacity;
    capacity = bufferCapacity;
    bufferCapacity = c;
}

શૂન્ય UnicodeSet::setToBogus() {
    clear(); // Remove everything in the set.
    fFlags = kIsBogus;
}

//----------------------------------------------------------------
// Implementation: Fundamental operators
//----------------------------------------------------------------

સ્થિર અંતઃરેખ UChar32 max(UChar32 a, UChar32 b) {
    પાછા (a > b) ? a : b;
}

// polarity = 0, 3 is normal: x xor y
// polarity = 1, 2: x xor ~y == x === y

શૂન્ય UnicodeSet::exclusiveOr(અચલ UChar32* other, int32_t otherLen, int8_t polarity) {
    જો (isFrozen() || isBogus()) {
        પાછા;
    }
    જો (!ensureBufferCapacity(len + otherLen)) {
        પાછા;
    }

    int32_t i = 0, j = 0, k = 0;
    UChar32 a = list[i++];
    UChar32 b;
    જો (polarity == 1 || polarity == 2) {
        b = UNICODESET_નીચલું;
        જો (other[j] == UNICODESET_નીચલું) { // skip base if already LOW
            ++j;
            b = other[j];
        }
    } નહીંતર {
        b = other[j++];
    }
    // simplest of all the routines
    // sort the values, discarding identicals!
    માટે (;;) {
        જો (a < b) {
            buffer[k++] = a;
            a = list[i++];
        } નહીંતર જો (b < a) {
            buffer[k++] = b;
            b = other[j++];
        } નહીંતર જો (a != UNICODESET_ઉપલું) { // at this point, a == b
            // discard both values!
            a = list[i++];
            b = other[j++];
        } નહીંતર { // DONE!
            buffer[k++] = UNICODESET_ઉપલું;
            len = k;
            તોડો;
        }
    }
    swapBuffers();
    releasePattern();
}

// polarity = 0 is normal: x union y
// polarity = 2: x union ~y
// polarity = 1: ~x union y
// polarity = 3: ~x union ~y

શૂન્ય UnicodeSet::add(અચલ UChar32* other, int32_t otherLen, int8_t polarity) {
    જો (isFrozen() || isBogus() || other==nullptr) {
        પાછા;
    }
    જો (!ensureBufferCapacity(len + otherLen)) {
        પાછા;
    }

    int32_t i = 0, j = 0, k = 0;
    UChar32 a = list[i++];
    UChar32 b = other[j++];
    // change from xor is that we have to check overlapping pairs
    // polarity bit 1 means a is second, bit 2 means b is.
    માટે (;;) {
        પસંદગી (polarity) {
          સ્થિતિ 0: // both first; take lower if unequal
            જો (a < b) { // take a
                // Back up over overlapping ranges in buffer[]
                જો (k > 0 && a <= buffer[k-1]) {
                    // Pick latter end value in buffer[] vs. list[]
                    a = max(list[i], buffer[--k]);
                } નહીંતર {
                    // No overlap
                    buffer[k++] = a;
                    a = list[i];
                }
                i++; // Common if/else code factored out
                polarity ^= 1;
            } નહીંતર જો (b < a) { // take b
                જો (k > 0 && b <= buffer[k-1]) {
                    b = max(other[j], buffer[--k]);
                } નહીંતર {
                    buffer[k++] = b;
                    b = other[j];
                }
                j++;
                polarity ^= 2;
            } નહીંતર { // a == b, take a, drop b
                જો (a == UNICODESET_ઉપલું) જાઓ loop_end;
                // This is symmetrical; it doesn't matter if
                // we backtrack with a or b. - liu
                જો (k > 0 && a <= buffer[k-1]) {
                    a = max(list[i], buffer[--k]);
                } નહીંતર {
                    // No overlap
                    buffer[k++] = a;
                    a = list[i];
                }
                i++;
                polarity ^= 1;
                b = other[j++];
                polarity ^= 2;
            }
            તોડો;
          સ્થિતિ 3: // both second; take higher if unequal, and drop other
            જો (b <= a) { // take a
                જો (a == UNICODESET_ઉપલું) જાઓ loop_end;
                buffer[k++] = a;
            } નહીંતર { // take b
                જો (b == UNICODESET_ઉપલું) જાઓ loop_end;
                buffer[k++] = b;
            }
            a = list[i++];
            polarity ^= 1;   // factored common code
            b = other[j++];
            polarity ^= 2;
            તોડો;
          સ્થિતિ 1: // a second, b first; if b < a, overlap
            જો (a < b) { // no overlap, take a
                buffer[k++] = a; a = list[i++]; polarity ^= 1;
            } નહીંતર જો (b < a) { // OVERLAP, drop b
                b = other[j++];
                polarity ^= 2;
            } નહીંતર { // a == b, drop both!
                જો (a == UNICODESET_ઉપલું) જાઓ loop_end;
                a = list[i++];
                polarity ^= 1;
                b = other[j++];
                polarity ^= 2;
            }
            તોડો;
          સ્થિતિ 2: // a first, b second; if a < b, overlap
            જો (b < a) { // no overlap, take b
                buffer[k++] = b;
                b = other[j++];
                polarity ^= 2;
            } નહીંતર  જો (a < b) { // OVERLAP, drop a
                a = list[i++];
                polarity ^= 1;
            } નહીંતર { // a == b, drop both!
                જો (a == UNICODESET_ઉપલું) જાઓ loop_end;
                a = list[i++];
                polarity ^= 1;
                b = other[j++];
                polarity ^= 2;
            }
            તોડો;
        }
    }
 loop_end:
    buffer[k++] = UNICODESET_ઉપલું;    // terminate
    len = k;
    swapBuffers();
    releasePattern();
}

// polarity = 0 is normal: x intersect y
// polarity = 2: x intersect ~y == set-minus
// polarity = 1: ~x intersect y
// polarity = 3: ~x intersect ~y

શૂન્ય UnicodeSet::retain(અચલ UChar32* other, int32_t otherLen, int8_t polarity) {
    જો (isFrozen() || isBogus()) {
        પાછા;
    }
    જો (!ensureBufferCapacity(len + otherLen)) {
        પાછા;
    }

    int32_t i = 0, j = 0, k = 0;
    UChar32 a = list[i++];
    UChar32 b = other[j++];
    // change from xor is that we have to check overlapping pairs
    // polarity bit 1 means a is second, bit 2 means b is.
    માટે (;;) {
        પસંદગી (polarity) {
          સ્થિતિ 0: // both first; drop the smaller
            જો (a < b) { // drop a
                a = list[i++];
                polarity ^= 1;
            } નહીંતર જો (b < a) { // drop b
                b = other[j++];
                polarity ^= 2;
            } નહીંતર { // a == b, take one, drop other
                જો (a == UNICODESET_ઉપલું) જાઓ loop_end;
                buffer[k++] = a;
                a = list[i++];
                polarity ^= 1;
                b = other[j++];
                polarity ^= 2;
            }
            તોડો;
          સ્થિતિ 3: // both second; take lower if unequal
            જો (a < b) { // take a
                buffer[k++] = a;
                a = list[i++];
                polarity ^= 1;
            } નહીંતર જો (b < a) { // take b
                buffer[k++] = b;
                b = other[j++];
                polarity ^= 2;
            } નહીંતર { // a == b, take one, drop other
                જો (a == UNICODESET_ઉપલું) જાઓ loop_end;
                buffer[k++] = a;
                a = list[i++];
                polarity ^= 1;
                b = other[j++];
                polarity ^= 2;
            }
            તોડો;
          સ્થિતિ 1: // a second, b first;
            જો (a < b) { // NO OVERLAP, drop a
                a = list[i++];
                polarity ^= 1;
            } નહીંતર જો (b < a) { // OVERLAP, take b
                buffer[k++] = b;
                b = other[j++];
                polarity ^= 2;
            } નહીંતર { // a == b, drop both!
                જો (a == UNICODESET_ઉપલું) જાઓ loop_end;
                a = list[i++];
                polarity ^= 1;
                b = other[j++];
                polarity ^= 2;
            }
            તોડો;
          સ્થિતિ 2: // a first, b second; if a < b, overlap
            જો (b < a) { // no overlap, drop b
                b = other[j++];
                polarity ^= 2;
            } નહીંતર  જો (a < b) { // OVERLAP, take a
                buffer[k++] = a;
                a = list[i++];
                polarity ^= 1;
            } નહીંતર { // a == b, drop both!
                જો (a == UNICODESET_ઉપલું) જાઓ loop_end;
                a = list[i++];
                polarity ^= 1;
                b = other[j++];
                polarity ^= 2;
            }
            તોડો;
        }
    }
 loop_end:
    buffer[k++] = UNICODESET_ઉપલું;    // terminate
    len = k;
    swapBuffers();
    releasePattern();
}

/**
 * Append the <code>toPattern()</code> representation of a
 * string to the given <code>StringBuffer</code>.
 */
શૂન્ય UnicodeSet::_appendToPat(UnicodeString& buf, અચલ UnicodeString& s, UBool escapeUnprintable) {
    UChar32 cp;
    માટે (int32_t i = 0; i < s.length(); i += U16_LENGTH(cp)) {
        _appendToPat(buf, cp = s.char32At(i), escapeUnprintable);
    }
}

/**
 * Append the <code>toPattern()</code> representation of a
 * character to the given <code>StringBuffer</code>.
 */
શૂન્ય UnicodeSet::_appendToPat(UnicodeString& buf, UChar32 c, UBool escapeUnprintable) {
    જો (escapeUnprintable ? ICU_Utility::isUnprintable(c) : ICU_Utility::shouldAlwaysBeEscaped(c)) {
        // Use hex escape notation (\uxxxx or \Uxxxxxxxx) for anything
        // unprintable
        ICU_Utility::escape(buf, c);
        પાછા;
    }
    // Okay to let ':' pass through
    પસંદગી (c) {
    સ્થિતિ u'[':
    સ્થિતિ u']':
    સ્થિતિ u'-':
    સ્થિતિ u'^':
    સ્થિતિ u'&':
    સ્થિતિ u'\\':
    સ્થિતિ u'{':
    સ્થિતિ u'}':
    સ્થિતિ u':':
    સ્થિતિ પ્રતીક_Table::પ્રતીક_REF:
        buf.append(u'\\');
        તોડો;
    મૂળભૂત:
        // Escape whitespace
        જો (PatternProps::isWhiteSpace(c)) {
            buf.append(u'\\');
        }
        તોડો;
    }
    buf.append(c);
}

શૂન્ય UnicodeSet::_appendToPat(UnicodeString &result, UChar32 start, UChar32 end,
                              UBool escapeUnprintable) {
    _appendToPat(result, start, escapeUnprintable);
    જો (start != end) {
        જો ((start+1) != end ||
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
                                      UBool escapeUnprintable) અચલ
{
    જો (pat != nullptr) {
        int32_t i;
        int32_t backslash_ગણતરી = 0;
        માટે (i=0; i<patLen; ) {
            UChar32 c;
            U16_NEXT(pat, i, patLen, c);
            જો (escapeUnprintable ?
                    ICU_Utility::isUnprintable(c) : ICU_Utility::shouldAlwaysBeEscaped(c)) {
                // If the unprintable character is preceded by an odd
                // number of backslashes, then it has been escaped.
                // Before unescaping it, we delete the final
                // backslash.
                જો ((backslash_ગણતરી % 2) == 1) {
                    result.truncate(result.length() - 1);
                }
                ICU_Utility::escape(result, c);
                backslash_ગણતરી = 0;
            } નહીંતર {
                result.append(c);
                જો (c == u'\\') {
                    ++backslash_ગણતરી;
                } નહીંતર {
                    backslash_ગણતરી = 0;
                }
            }
        }
        પાછા result;
    }

    પાછા _generatePattern(result, escapeUnprintable);
}

/**
 * Returns a string representation of this set.  If the result of
 * calling this function is passed to a UnicodeSet constructor, it
 * will produce another set that is equal to this one.
 */
UnicodeString& UnicodeSet::toPattern(UnicodeString& result U_LIFETIME_BOUND,
                                     UBool escapeUnprintable) અચલ
{
    result.truncate(0);
    પાછા _toPattern(result, escapeUnprintable);
}

/**
 * Generate and append a string representation of this set to result.
 * This does not use this.pat, the cleaned up copy of the string
 * passed to applyPattern().
 */
UnicodeString& UnicodeSet::_generatePattern(UnicodeString& result U_LIFETIME_BOUND,
                                            UBool escapeUnprintable) અચલ
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
    જો (len >= 4 && list[0] == 0 && limit == len && !hasStrings()) {
        // Emit the inverse
        result.append(u'^');
        // Offsetting the inversion list index by one lets us
        // iterate over the ranges of the set complement.
        i = 1;
        --limit;
    }

    // Emit the ranges as pairs.
    જ્યારે (i < limit) {
        UChar32 start = list[i];  // getRangeStart()
        UChar32 end = list[i + 1] - 1;  // getRangeEnd() = range limit minus one
        જો (!(0xd800 <= end && end <= 0xdbff)) {
            _appendToPat(result, start, end, escapeUnprintable);
            i += 2;
        } નહીંતર {
            // The range ends with a lead surrogate.
            // Avoid writing what looks like a lead+trail surrogate pair.
            // 1. Postpone ranges that start with a lead surrogate code point.
            int32_t firstLead = i;
            જ્યારે ((i += 2) < limit && list[i] <= 0xdbff) {}
            int32_t firstAfterLead = i;
            // 2. Write following ranges that start with a trail surrogate code point.
            જ્યારે (i < limit && (start = list[i]) <= 0xdfff) {
                _appendToPat(result, start, list[i + 1] - 1, escapeUnprintable);
                i += 2;
            }
            // 3. Now write the postponed ranges.
            માટે (પૂર્ણાંક j = firstLead; j < firstAfterLead; j += 2) {
                _appendToPat(result, list[j], list[j + 1] - 1, escapeUnprintable);
            }
        }
    }

    જો (strings_ != nullptr) {
        માટે (int32_t i = 0; i<strings_->size(); ++i) {
            result.append(u'{');
            _appendToPat(result,
                         *static_cast<અચલ UnicodeString*>(strings_->elementAt(i)),
                         escapeUnprintable);
            result.append(u'}');
        }
    }
    પાછા result.append(u']');
}

/**
* Release existing cached pattern
*/
શૂન્ય UnicodeSet::releasePattern() {
    જો (pat) {
        uprv_free(pat);
        pat = nullptr;
        patLen = 0;
    }
}

/**
* Set the new pattern to cache.
*/
શૂન્ય UnicodeSet::setPattern(અચલ char16_t *newPat, int32_t newPatLen) {
    releasePattern();
    pat = static_cast<char16_t*>(uprv_malloc((newPatLen + 1) * કદ(char16_t)));
    જો (pat) {
        patLen = newPatLen;
        u_memcpy(pat, newPat, patLen);
        pat[patLen] = 0;
    }
    // else we don't care if malloc failed. This was just a nice cache.
    // We can regenerate an equivalent pattern later when requested.
}

UnicodeSet* UnicodeSet::freeze() U_LIFETIME_BOUND {
    જો(!isFrozen() && !isBogus()) {
        compact();

        // Optimize contains() and span() and similar functions.
        જો (hasStrings()) {
            stringSpan = નવું UnicodeSetStringSpan(*આ, *strings_, UnicodeSetStringSpan::ALL);
            જો (stringSpan == nullptr) {
                setToBogus();
                પાછા આ;
            } નહીંતર જો (!stringSpan->needsStringSpanUTF16()) {
                // All strings are irrelevant for span() etc. because
                // all of each string's code points are contained in this set.
                // Do not check needsStringSpanUTF8() because UTF-8 has at most as
                // many relevant strings as UTF-16.
                // (Thus needsStringSpanUTF8() implies needsStringSpanUTF16().)
                delete stringSpan;
                stringSpan = nullptr;
            }
        }
        જો (stringSpan == nullptr) {
            // No span-relevant strings: Optimize for code point spans.
            bmpSet=નવું BMPSet(list, len);
            જો (bmpSet == nullptr) { // Check for memory allocation error.
                setToBogus();
            }
        }
    }
    પાછા આ;
}

int32_t UnicodeSet::span(અચલ char16_t *s, int32_t length, USetSpanCondition spanCondition) અચલ {
    જો(length>0 && bmpSet!=nullptr) {
        પાછા static_cast<int32_t>(bmpSet->span(s, s + length, spanCondition) - s);
    }
    જો(length<0) {
        length=u_strlen(s);
    }
    જો(length==0) {
        પાછા 0;
    }
    જો(stringSpan!=nullptr) {
        પાછા stringSpan->span(s, length, spanCondition);
    } નહીંતર જો(hasStrings()) {
        uint32_t which= spanCondition==USET_SPAN_NOT_CONTAINED ?
                            UnicodeSetStringSpan::FWD_UTF16_NOT_CONTAINED :
                            UnicodeSetStringSpan::FWD_UTF16_CONTAINED;
        UnicodeSetStringSpan strSpan(*આ, *strings_, which);
        જો(strSpan.needsStringSpanUTF16()) {
            પાછા strSpan.span(s, length, spanCondition);
        }
    }

    જો(spanCondition!=USET_SPAN_NOT_CONTAINED) {
        spanCondition=USET_SPAN_CONTAINED;  // Pin to 0/1 values.
    }

    UChar32 c;
    int32_t start=0, prev=0;
    કરો {
        U16_NEXT(s, start, length, c);
        જો(spanCondition!=contains(c)) {
            તોડો;
        }
    } જ્યારે((prev=start)<length);
    પાછા prev;
}

int32_t UnicodeSet::spanBack(અચલ char16_t *s, int32_t length, USetSpanCondition spanCondition) અચલ {
    જો(length>0 && bmpSet!=nullptr) {
        પાછા static_cast<int32_t>(bmpSet->spanBack(s, s + length, spanCondition) - s);
    }
    જો(length<0) {
        length=u_strlen(s);
    }
    જો(length==0) {
        પાછા 0;
    }
    જો(stringSpan!=nullptr) {
        પાછા stringSpan->spanBack(s, length, spanCondition);
    } નહીંતર જો(hasStrings()) {
        uint32_t which= spanCondition==USET_SPAN_NOT_CONTAINED ?
                            UnicodeSetStringSpan::BACK_UTF16_NOT_CONTAINED :
                            UnicodeSetStringSpan::BACK_UTF16_CONTAINED;
        UnicodeSetStringSpan strSpan(*આ, *strings_, which);
        જો(strSpan.needsStringSpanUTF16()) {
            પાછા strSpan.spanBack(s, length, spanCondition);
        }
    }

    જો(spanCondition!=USET_SPAN_NOT_CONTAINED) {
        spanCondition=USET_SPAN_CONTAINED;  // Pin to 0/1 values.
    }

    UChar32 c;
    int32_t prev=length;
    કરો {
        U16_PREV(s, 0, length, c);
        જો(spanCondition!=contains(c)) {
            તોડો;
        }
    } જ્યારે((prev=length)>0);
    પાછા prev;
}

int32_t UnicodeSet::spanUTF8(અચલ અક્ષર *s, int32_t length, USetSpanCondition spanCondition) અચલ {
    જો(length>0 && bmpSet!=nullptr) {
        અચલ uint8_t* s0 = reinterpret_cast<અચલ uint8_t*>(s);
        પાછા static_cast<int32_t>(bmpSet->spanUTF8(s0, length, spanCondition) - s0);
    }
    જો(length<0) {
        length = static_cast<int32_t>(uprv_strlen(s));
    }
    જો(length==0) {
        પાછા 0;
    }
    જો(stringSpan!=nullptr) {
        પાછા stringSpan->spanUTF8(reinterpret_cast<અચલ uint8_t*>(s), length, spanCondition);
    } નહીંતર જો(hasStrings()) {
        uint32_t which= spanCondition==USET_SPAN_NOT_CONTAINED ?
                            UnicodeSetStringSpan::FWD_UTF8_NOT_CONTAINED :
                            UnicodeSetStringSpan::FWD_UTF8_CONTAINED;
        UnicodeSetStringSpan strSpan(*આ, *strings_, which);
        જો(strSpan.needsStringSpanUTF8()) {
            પાછા strSpan.spanUTF8(reinterpret_cast<અચલ uint8_t*>(s), length, spanCondition);
        }
    }

    જો(spanCondition!=USET_SPAN_NOT_CONTAINED) {
        spanCondition=USET_SPAN_CONTAINED;  // Pin to 0/1 values.
    }

    UChar32 c;
    int32_t start=0, prev=0;
    કરો {
        U8_NEXT_OR_FFFD(s, start, length, c);
        જો(spanCondition!=contains(c)) {
            તોડો;
        }
    } જ્યારે((prev=start)<length);
    પાછા prev;
}

int32_t UnicodeSet::spanBackUTF8(અચલ અક્ષર *s, int32_t length, USetSpanCondition spanCondition) અચલ {
    જો(length>0 && bmpSet!=nullptr) {
        અચલ uint8_t* s0 = reinterpret_cast<અચલ uint8_t*>(s);
        પાછા bmpSet->spanBackUTF8(s0, length, spanCondition);
    }
    જો(length<0) {
        length = static_cast<int32_t>(uprv_strlen(s));
    }
    જો(length==0) {
        પાછા 0;
    }
    જો(stringSpan!=nullptr) {
        પાછા stringSpan->spanBackUTF8(reinterpret_cast<અચલ uint8_t*>(s), length, spanCondition);
    } નહીંતર જો(hasStrings()) {
        uint32_t which= spanCondition==USET_SPAN_NOT_CONTAINED ?
                            UnicodeSetStringSpan::BACK_UTF8_NOT_CONTAINED :
                            UnicodeSetStringSpan::BACK_UTF8_CONTAINED;
        UnicodeSetStringSpan strSpan(*આ, *strings_, which);
        જો(strSpan.needsStringSpanUTF8()) {
            પાછા strSpan.spanBackUTF8(reinterpret_cast<અચલ uint8_t*>(s), length, spanCondition);
        }
    }

    જો(spanCondition!=USET_SPAN_NOT_CONTAINED) {
        spanCondition=USET_SPAN_CONTAINED;  // Pin to 0/1 values.
    }

    UChar32 c;
    int32_t prev=length;
    કરો {
        U8_PREV_OR_FFFD(s, 0, length, c);
        જો(spanCondition!=contains(c)) {
            તોડો;
        }
    } જ્યારે((prev=length)>0);
    પાછા prev;
}

U_NAMESPACE_END
