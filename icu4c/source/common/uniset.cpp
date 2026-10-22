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

#ضمن "unicode/utypes.h"
#ضمن "unicode/parsepos.h"
#ضمن "unicode/symtable.h"
#ضمن "unicode/uniset.h"
#ضمن "unicode/ustring.h"
#ضمن "unicode/utf8.h"
#ضمن "unicode/utf16.h"
#ضمن "ruleiter.h"
#ضمن "cmemory.h"
#ضمن "cstring.h"
#ضمن "patternprops.h"
#ضمن "uelement.h"
#ضمن "util.h"
#ضمن "uvector.h"
#ضمن "charstr.h"
#ضمن "ustrfmt.h"
#ضمن "uassert.h"
#ضمن "bmpset.h"
#ضمن "unisetspan.h"

// HIGH_VALUE > all valid values. 110000 for codepoints
#define UNICODESET_بلند 0x0110000

// LOW <= all valid values. ZERO for codepoints
#define UNICODESET_کم 0x000000

/** Max list [0, 1, 2, ..., max code point, HIGH] */
constexpr int32_t MAX_LENGTH = UNICODESET_بلند + 1;

U_NAMESPACE_BEGIN

علامت_Table::~علامت_Table() {}

UOBJECT_DEFINE_RTTI_IMPLEMENTATION(UnicodeSet)

/**
 * Modify the given UChar32 variable so that it is in range, by
 * pinning values < UNICODESET_LOW to UNICODESET_LOW, and
 * pinning values > UNICODESET_HIGH-1 to UNICODESET_HIGH-1.
 * It modifies its argument in-place and also returns it.
 */
ساكن مضمن UChar32 pinCodePoint(UChar32& c) {
    إذا (c < UNICODESET_کم) {
        c = UNICODESET_کم;
    } وإلا إذا (c > (UNICODESET_بلند-1)) {
        c = (UNICODESET_بلند-1);
    }
    أرجع c;
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
#ضمن <stdio.h>
ساكن int32_t dbg_گنتی = 0;

ساكن مضمن فراغ _dbgct(UnicodeSet* set) {
    UnicodeString str;
    set->toPattern(str, true);
    حرف buf[40];
    str.extract(0, 39, buf, "");
    printf("DEBUG UnicodeSet: ct 0x%08X; %d %s\n", set, ++dbg_گنتی, buf);
}

ساكن مضمن فراغ _dbgdt(UnicodeSet* set) {
    UnicodeString str;
    set->toPattern(str, true);
    حرف buf[40];
    str.extract(0, 39, buf, "");
    printf("DEBUG UnicodeSet: dt 0x%08X; %d %s\n", set, --dbg_گنتی, buf);
}

#وإلا

#define _dbgct(set)
#define _dbgdt(set)

#endif

//----------------------------------------------------------------
// UnicodeString in UVector support
//----------------------------------------------------------------

ساكن فراغ U_CALLCONV cloneUnicodeString(UElement *dst, UElement *src) {
    dst->pointer = جديد UnicodeString(*static_cast<UnicodeString*>(src->pointer));
}

ساكن int32_t U_CALLCONV compareUnicodeString(UElement t1, UElement t2) {
    ثابت UnicodeString& a = *static_cast<ثابت UnicodeString*>(t1.pointer);
    ثابت UnicodeString& b = *static_cast<ثابت UnicodeString*>(t2.pointer);
    أرجع a.compare(b);
}

UBool UnicodeSet::hasStrings() ثابت {
    أرجع strings_ != nullptr && !strings_->isEmpty();
}

int32_t UnicodeSet::stringsSize() ثابت {
    أرجع strings_ == nullptr ? 0 : strings_->size();
}

UBool UnicodeSet::stringsContains(ثابت UnicodeString &s) ثابت {
    أرجع strings_ != nullptr && strings_->contains((فراغ*) &s);
}

//----------------------------------------------------------------
// Constructors &c
//----------------------------------------------------------------

/**
 * Constructs an empty set.
 */
UnicodeSet::UnicodeSet() {
    list[0] = UNICODESET_بلند;
    _dbgct(هذا);
}

/**
 * Constructs a set containing the given range. If <code>end >
 * start</code> then an empty set is created.
 *
 * @param start first character, inclusive, of range
 * @param end last character, inclusive, of range
 */
UnicodeSet::UnicodeSet(UChar32 start, UChar32 end) {
    list[0] = UNICODESET_بلند;
    add(start, end);
    _dbgct(هذا);
}

/**
 * Constructs a set that is identical to the given UnicodeSet.
 */
UnicodeSet::UnicodeSet(ثابت UnicodeSet& o) : UnicodeFilter(o) {
    *هذا = o;
    _dbgct(هذا);
}

// Copy-construct as thawed.
UnicodeSet::UnicodeSet(ثابت UnicodeSet& o, UBool /* asThawed */) : UnicodeFilter(o) {
    إذا (ensureCapacity(o.len)) {
        // *this = o except for bmpSet and stringSpan
        len = o.len;
        uprv_memcpy(list, o.list, (size_t)len*حجم(UChar32));
        إذا (o.hasStrings()) {
            UErrorCode status = U_ZERO_ERROR;
            إذا (!allocateStrings(status) ||
                    (strings_->assign(*o.strings_, cloneUnicodeString, status), U_FAILURE(status))) {
                setToBogus();
                أرجع;
            }
        }
        إذا (o.pat) {
            setPattern(o.pat, o.patLen);
        }
        _dbgct(هذا);
    }
}

/**
 * Destructs the set.
 */
UnicodeSet::~UnicodeSet() {
    _dbgdt(هذا); // first!
    إذا (list != ڈھیر_List) {
        uprv_free(list);
    }
    delete bmpSet;
    إذا (buffer != ڈھیر_List) {
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
    أرجع copyFrom(o, خطأ);
}

UnicodeSet& UnicodeSet::copyFrom(ثابت UnicodeSet& o, UBool asThawed) U_LIFETIME_BOUND {
    إذا (هذا == &o) {
        أرجع *هذا;
    }
    إذا (isFrozen()) {
        أرجع *هذا;
    }
    إذا (o.isBogus()) {
        setToBogus();
        أرجع *هذا;
    }
    إذا (!ensureCapacity(o.len)) {
        // ensureCapacity will mark the UnicodeSet as Bogus if OOM failure happens.
        أرجع *هذا;
    }
    len = o.len;
    uprv_memcpy(list, o.list, (size_t)len*حجم(UChar32));
    إذا (o.bmpSet != nullptr && !asThawed) {
        bmpSet = جديد BMPSet(*o.bmpSet, list, len);
        إذا (bmpSet == nullptr) { // Check for memory allocation error.
            setToBogus();
            أرجع *هذا;
        }
    }
    إذا (o.hasStrings()) {
        UErrorCode status = U_ZERO_ERROR;
        إذا ((strings_ == nullptr && !allocateStrings(status)) ||
                (strings_->assign(*o.strings_, cloneUnicodeString, status), U_FAILURE(status))) {
            setToBogus();
            أرجع *هذا;
        }
    } وإلا إذا (hasStrings()) {
        strings_->removeAllElements();
    }
    إذا (o.stringSpan != nullptr && strings_ != nullptr && !asThawed) {
        stringSpan = جديد UnicodeSetStringSpan(*o.stringSpan, *strings_);
        إذا (stringSpan == nullptr) { // Check for memory allocation error.
            setToBogus();
            أرجع *هذا;
        }
    }
    releasePattern();
    إذا (o.pat) {
        setPattern(o.pat, o.patLen);
    }
    أرجع *هذا;
}

/**
 * Returns a copy of this object.  All UnicodeMatcher objects have
 * to support cloning in order to allow classes using
 * UnicodeMatchers, such as Transliterator, to implement cloning.
 */
UnicodeSet* UnicodeSet::clone() ثابت {
    أرجع جديد UnicodeSet(*هذا);
}

UnicodeSet *UnicodeSet::cloneAsThawed() ثابت {
    أرجع جديد UnicodeSet(*هذا, true);
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
منطقي UnicodeSet::operator==(ثابت UnicodeSet& o) ثابت {
    إذا (len != o.len) أرجع خطأ;
    لكل (int32_t i = 0; i < len; ++i) {
        إذا (list[i] != o.list[i]) أرجع خطأ;
    }
    إذا (hasStrings() != o.hasStrings()) { أرجع خطأ; }
    إذا (hasStrings() && *strings_ != *o.strings_) أرجع خطأ;
    أرجع true;
}

/**
 * Returns the hash code value for this set.
 *
 * @return the hash code value for this set.
 * @see Object#hashCode()
 */
int32_t UnicodeSet::hashCode() ثابت {
    uint32_t result = static_cast<uint32_t>(len);
    لكل (int32_t i = 0; i < len; ++i) {
        result *= 1000003u;
        result += list[i];
    }
    أرجع static_cast<int32_t>(result);
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
    int32_t count = get_Range_گنتی();
    لكل (int32_t i = 0; i < count; ++i) {
        n += getRangeEnd(i) - getRangeStart(i) + 1;
    }
    أرجع n + stringsSize();
}

/**
 * Returns <tt>true</tt> if this set contains no elements.
 *
 * @return <tt>true</tt> if this set contains no elements.
 */
UBool UnicodeSet::isEmpty() ثابت {
    أرجع len == 1 && !hasStrings();
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
    إذا (bmpSet != nullptr) {
        أرجع bmpSet->contains(c);
    }
    إذا (stringSpan != nullptr) {
        أرجع stringSpan->contains(c);
    }
    إذا (c >= UNICODESET_بلند) { // Don't need to check LOW bound
        أرجع خطأ;
    }
    int32_t i = findCodePoint(c);
    أرجع i & 1; // return true if odd
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
    إذا (c < list[0])
        أرجع 0;
    // High runner test.  c is often after the last range, so an
    // initial check for this condition pays off.
    int32_t lo = 0;
    int32_t hi = len - 1;
    إذا (lo >= hi || c >= list[hi-1])
        أرجع hi;
    // invariant: c >= list[lo]
    // invariant: c < list[hi]
    لكل (;;) {
        int32_t i = (lo + hi) >> 1;
        إذا (i == lo) {
            اكسر; // Found!
        } وإلا إذا (c < list[i]) {
            hi = i;
        } وإلا {
            lo = i;
        }
    }
    أرجع hi;
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
    أرجع ((i & 1) != 0 && end < list[i]);
}

/**
 * Returns <tt>true</tt> if this set contains the given
 * multicharacter string.
 * @param s string to be checked for containment
 * @return <tt>true</tt> if this set contains the specified string
 */
UBool UnicodeSet::contains(ثابت UnicodeString& s) ثابت {
    int32_t cp = getSingleCP(s);
    إذا (cp < 0) {
        أرجع stringsContains(s);
    } وإلا {
        أرجع contains(static_cast<UChar32>(cp));
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
    int32_t n = c.get_Range_گنتی();
    لكل (صحيح i=0; i<n; ++i) {
        إذا (!contains(c.getRangeStart(i), c.getRangeEnd(i))) {
            أرجع خطأ;
        }
    }
    أرجع !c.hasStrings() || (strings_ != nullptr && strings_->containsAll(*c.strings_));
}

/**
 * Returns true if this set contains all the characters
 * of the given string.
 * @param s string containing characters to be checked for containment
 * @return true if the test condition is met
 */
UBool UnicodeSet::containsAll(ثابت UnicodeString& s) ثابت {
    أرجع span(s.getBuffer(), s.length(), USET_SPAN_CONTAINED) == s.length();
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
    أرجع ((i & 1) == 0 && end < list[i]);
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
    int32_t n = c.get_Range_گنتی();
    لكل (int32_t i=0; i<n; ++i) {
        إذا (!containsNone(c.getRangeStart(i), c.getRangeEnd(i))) {
            أرجع خطأ;
        }
    }
    أرجع strings_ == nullptr || !c.hasStrings() || strings_->containsNone(*c.strings_);
}

/**
 * Returns true if this set contains none of the characters
 * of the given string.
 * @param s string containing characters to be checked for containment
 * @return true if the test condition is met
 */
UBool UnicodeSet::containsNone(ثابت UnicodeString& s) ثابت {
    أرجع span(s.getBuffer(), s.length(), USET_SPAN_NOT_CONTAINED) == s.length();
}

/**
 * Returns <tt>true</tt> if this set contains any character whose low byte
 * is the given value.  This is used by <tt>RuleBasedTransliterator</tt> for
 * indexing.
 */
UBool UnicodeSet::matches_Index_قدر(uint8_t v) ثابت {
    /* The index value v, in the range [0,255], is contained in this set if
     * it is contained in any pair of this set.  Pairs either have the high
     * bytes equal, or unequal.  If the high bytes are equal, then we have
     * aaxx..aayy, where aa is the high byte.  Then v is contained if xx <=
     * v <= yy.  If the high bytes are unequal we have aaxx..bbyy, bb>aa.
     * Then v is contained if xx <= v || v <= yy.  (This is identical to the
     * time zone month containment logic.)
     */
    int32_t i;
    int32_t range_گنتی=get_Range_گنتی();
    لكل (i=0; i<range_گنتی; ++i) {
        UChar32 کم = getRangeStart(i);
        UChar32 بلند = getRangeEnd(i);
        إذا ((کم & ~0xFF) == (بلند & ~0xFF)) {
            إذا ((کم & 0xFF) <= v && v <= (بلند & 0xFF)) {
                أرجع true;
            }
        } وإلا إذا ((کم & 0xFF) <= v || v <= (بلند & 0xFF)) {
            أرجع true;
        }
    }
    إذا (hasStrings()) {
        لكل (i=0; i<strings_->size(); ++i) {
            ثابت UnicodeString& s = *static_cast<ثابت UnicodeString*>(strings_->elementAt(i));
            إذا (s.isEmpty()) {
                استمر;  // skip the empty string
            }
            UChar32 c = s.char32At(0);
            إذا ((c & 0xFF) == v) {
                أرجع true;
            }
        }
    }
    أرجع خطأ;
}

/**
 * Implementation of UnicodeMatcher::matches().  Always matches the
 * longest possible multichar string.
 */
UMatchDegree UnicodeSet::matches(ثابت Replaceable& text,
                                 int32_t& offset,
                                 int32_t limit,
                                 UBool incremental) {
    إذا (offset == limit) {
        إذا (contains(U_ETHER)) {
            أرجع incremental ? U_PARTIAL_MATCH : U_MATCH;
        } وإلا {
            أرجع U_MISMATCH;
        }
    } وإلا {
        إذا (hasStrings()) { // try strings first

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
            int32_t بلند_Water_Length = 0;

            لكل (i=0; i<strings_->size(); ++i) {
                ثابت UnicodeString& trial = *static_cast<ثابت UnicodeString*>(strings_->elementAt(i));
                إذا (trial.isEmpty()) {
                    استمر;  // skip the empty string
                }

                char16_t c = trial.charAt(forward ? 0 : trial.length() - 1);

                // Strings are sorted, so we can optimize in the
                // forward direction.
                إذا (forward && c > firstChar) اكسر;
                إذا (c != firstChar) استمر;

                int32_t matchLen = matchRest(text, offset, limit, trial);

                إذا (incremental) {
                    int32_t maxLen = forward ? limit-offset : offset-limit;
                    إذا (matchLen == maxLen) {
                        // We have successfully matched but only up to limit.
                        أرجع U_PARTIAL_MATCH;
                    }
                }

                إذا (matchLen == trial.length()) {
                    // We have successfully matched the whole string.
                    إذا (matchLen > بلند_Water_Length) {
                        بلند_Water_Length = matchLen;
                    }
                    // In the forward direction we know strings
                    // are sorted so we can bail early.
                    إذا (forward && matchLen < بلند_Water_Length) {
                        اكسر;
                    }
                    استمر;
                }
            }

            // We've checked all strings without a partial match.
            // If we have full matches, return the longest one.
            إذا (بلند_Water_Length != 0) {
                offset += forward ? بلند_Water_Length : -بلند_Water_Length;
                أرجع U_MATCH;
            }
        }
        أرجع UnicodeFilter::matches(text, offset, limit, incremental);
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
    إذا (start < limit) {
        maxLen = limit - start;
        إذا (maxLen > slen) maxLen = slen;
        لكل (i = 1; i < maxLen; ++i) {
            إذا (text.charAt(start + i) != s.charAt(i)) أرجع 0;
        }
    } وإلا {
        maxLen = start - limit;
        إذا (maxLen > slen) maxLen = slen;
        --slen; // <=> slen = s.length() - 1;
        لكل (i = 1; i < maxLen; ++i) {
            إذا (text.charAt(start - i) != s.charAt(slen - i)) أرجع 0;
        }
    }
    أرجع maxLen;
}

/**
 * Implement of UnicodeMatcher
 */
فراغ UnicodeSet::addMatchSetTo(UnicodeSet& toUnionTo) ثابت {
    toUnionTo.addAll(*هذا);
}

/**
 * Returns the index of the given character within this set, where
 * the set is ordered by ascending code point.  If the character
 * is not in this set, return -1.  The inverse of this method is
 * <code>charAt()</code>.
 * @return an index from 0..size()-1, or -1
 */
int32_t UnicodeSet::indexOf(UChar32 c) ثابت {
    إذا (c < MIN_قدر || c > MAX_قدر) {
        أرجع -1;
    }
    int32_t i = 0;
    int32_t n = 0;
    لكل (;;) {
        UChar32 start = list[i++];
        إذا (c < start) {
            أرجع -1;
        }
        UChar32 limit = list[i++];
        إذا (c < limit) {
            أرجع n + c - start;
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
    إذا (index >= 0) {
        // len2 is the largest even integer <= len, that is, it is len
        // for even values and len-1 for odd values.  With odd values
        // the last entry is UNICODESET_HIGH.
        int32_t len2 = len & ~1;
        لكل (int32_t i=0; i < len2;) {
            UChar32 start = list[i++];
            int32_t count = list[i++] - start;
            إذا (index < count) {
                أرجع static_cast<UChar32>(start + index);
            }
            index -= count;
        }
    }
    أرجع static_cast<UChar32>(-1);
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
    أرجع *هذا;
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
    إذا (pinCodePoint(start) < pinCodePoint(end)) {
        UChar32 limit = end + 1;
        // Fast path for adding a new range after the last one.
        // Odd list length: [..., lastStart, lastLimit, HIGH]
        إذا ((len & 1) != 0) {
            // If the list is empty, set lastLimit low enough to not be adjacent to 0.
            UChar32 lastLimit = len == 1 ? -2 : list[len - 2];
            إذا (lastLimit <= start && !isFrozen() && !isBogus()) {
                إذا (lastLimit == start) {
                    // Extend the last range.
                    list[len - 2] = limit;
                    إذا (limit == UNICODESET_بلند) {
                        --len;
                    }
                } وإلا {
                    list[len - 1] = start;
                    إذا (limit < UNICODESET_بلند) {
                        إذا (ensureCapacity(len + 2)) {
                            list[len++] = limit;
                            list[len++] = UNICODESET_بلند;
                        }
                    } وإلا {  // limit == UNICODESET_HIGH
                        إذا (ensureCapacity(len + 1)) {
                            list[len++] = UNICODESET_بلند;
                        }
                    }
                }
                releasePattern();
                أرجع *هذا;
            }
        }
        // This is slow. Could be much faster using findCodePoint(start)
        // and modifying the list, dealing with adjacent & overlapping ranges.
        UChar32 range[3] = { start, limit, UNICODESET_بلند };
        add(range, 2, 0);
    } وإلا إذا (start == end) {
        add(start);
    }
    أرجع *هذا;
}

// #define DEBUG_US_ADD

#ifdef DEBUG_US_ADD
#ضمن <stdio.h>
فراغ dump(UChar32 c) {
    إذا (c <= 0xFF) {
        printf("%c", (حرف)c);
    } وإلا {
        printf("U+%04X", c);
    }
}
فراغ dump(ثابت UChar32* list, int32_t len) {
    printf("[");
    لكل (int32_t i=0; i<len; ++i) {
        إذا (i != 0) printf(", ");
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
    إذا ((i & 1) != 0  || isFrozen() || isBogus()) أرجع *هذا;

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

    إذا (c == list[i]-1) {
        // c is before start of next range
        list[i] = c;
        // if we touched the HIGH mark, then add a new one
        إذا (c == (UNICODESET_بلند - 1)) {
            إذا (!ensureCapacity(len+1)) {
                // ensureCapacity will mark the object as Bogus if OOM failure happens.
                أرجع *هذا;
            }
            list[len++] = UNICODESET_بلند;
        }
        إذا (i > 0 && c == list[i-1]) {
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
            طالما (src < srclimit) *(dst++) = *(src++);

            len -= 2;
        }
    }

    وإلا إذا (i > 0 && c == list[i-1]) {
        // c is after end of prior range
        list[i-1]++;
        // no need to check for collapse here
    }

    وإلا {
        // At this point we know the new char is not adjacent to
        // any existing ranges, and it is not 10FFFF.


        // [..., start_k-1, limit_k-1, start_k, limit_k, ..., HIGH]
        //                             ^
        //                             list[i]

        // [..., start_k-1, limit_k-1, c, c+1, start_k, limit_k, ..., HIGH]
        //                             ^
        //                             list[i]

        إذا (!ensureCapacity(len+2)) {
            // ensureCapacity will mark the object as Bogus if OOM failure happens.
            أرجع *هذا;
        }

        UChar32 *p = list + i;
        uprv_memmove(p + 2, p, (len - i) * حجم(*p));
        list[i] = c;
        list[i+1] = c+1;
        len += 2;
    }

#ifdef DEBUG_US_ADD
    dump(list, len);
    printf("\n");

    لكل (i=1; i<len; ++i) {
        إذا (list[i] <= list[i-1]) {
            // Corrupt array!
            printf("ERROR: list has been corrupted\n");
            exit(1);
        }
    }
#endif

    releasePattern();
    أرجع *هذا;
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
    إذا (isFrozen() || isBogus()) أرجع *هذا;
    int32_t cp = getSingleCP(s);
    إذا (cp < 0) {
        إذا (!stringsContains(s)) {
            _add(s);
            releasePattern();
        }
    } وإلا {
        add(static_cast<UChar32>(cp));
    }
    أرجع *هذا;
}

/**
 * Adds the given string, in order, to 'strings_'.  The given string
 * must have been checked by the caller to not already be in 'strings_'.
 */
فراغ UnicodeSet::_add(ثابت UnicodeString& s) {
    إذا (isFrozen() || isBogus()) {
        أرجع;
    }
    UErrorCode ec = U_ZERO_ERROR;
    إذا (strings_ == nullptr && !allocateStrings(ec)) {
        setToBogus();
        أرجع;
    }
    LocalPointer<UnicodeString> t(جديد UnicodeString(s));
    إذا (t.isNull()) { // Check for memory allocation error.
        setToBogus();
        أرجع;
    }
    strings_->sorted_داخل_کرو(t.orphan(), compareUnicodeString, ec);
    إذا (U_FAILURE(ec)) {
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
    إذا (sLength == 1) أرجع s.charAt(0);
    إذا (sLength == 2) {
        UChar32 cp = s.char32At(0);
        إذا (cp > 0xFFFF) { // is surrogate pair
            أرجع cp;
        }
    }
    أرجع -1;
}

/**
 * Adds each of the characters in this string to the set. Thus "ch" => {"c", "h"}
 * If this set already any particular character, it has no effect on that character.
 * @param the source string
 * @return the modified set, for chaining
 */
UnicodeSet& UnicodeSet::addAll(ثابت UnicodeString& s) U_LIFETIME_BOUND {
    UChar32 cp;
    لكل (int32_t i = 0; i < s.length(); i += U16_LENGTH(cp)) {
        cp = s.char32At(i);
        add(cp);
    }
    أرجع *هذا;
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
    أرجع *هذا;
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
    أرجع *هذا;
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
    أرجع *هذا;
}

UnicodeSet& UnicodeSet::removeAllStrings() U_LIFETIME_BOUND {
    إذا (!isFrozen() && hasStrings()) {
        strings_->removeAllElements();
        releasePattern();
    }
    أرجع *هذا;
}


/**
 * Makes a set from a multicharacter string. Thus "ch" => {"ch"}
 * <br><b>Warning: you cannot add an empty string ("") to a UnicodeSet.</b>
 * @param the source string
 * @return a newly created set containing the given string
 */
UnicodeSet* U_EXPORT2 UnicodeSet::createFrom(ثابت UnicodeString& s) {
    UnicodeSet *set = جديد UnicodeSet();
    إذا (set != nullptr) { // Check for memory allocation error.
        set->add(s);
    }
    أرجع set;
}


/**
 * Makes a set from each of the characters in the string. Thus "ch" => {"c", "h"}
 * @param the source string
 * @return a newly created set containing the given characters
 */
UnicodeSet* U_EXPORT2 UnicodeSet::createFromAll(ثابت UnicodeString& s) {
    UnicodeSet *set = جديد UnicodeSet();
    إذا (set != nullptr) { // Check for memory allocation error.
        set->addAll(s);
    }
    أرجع set;
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
    إذا (pinCodePoint(start) <= pinCodePoint(end)) {
        UChar32 range[3] = { start, end+1, UNICODESET_بلند };
        retain(range, 2, 0);
    } وإلا {
        clear();
    }
    أرجع *هذا;
}

UnicodeSet& UnicodeSet::retain(UChar32 c) U_LIFETIME_BOUND {
    أرجع retain(c, c);
}

UnicodeSet& UnicodeSet::retain(ثابت UnicodeString &s) U_LIFETIME_BOUND {
    إذا (isFrozen() || isBogus()) { أرجع *هذا; }
    UChar32 cp = getSingleCP(s);
    إذا (cp < 0) {
        منطقي isIn = stringsContains(s);
        // Check for getRangeCount() first to avoid somewhat-expensive size()
        // when there are single code points.
        إذا (isIn && get_Range_گنتی() == 0 && size() == 1) {
            أرجع *هذا;
        }
        clear();
        إذا (isIn) {
            _add(s);
        }
    } وإلا {
        retain(cp, cp);
    }
    أرجع *هذا;
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
    إذا (pinCodePoint(start) <= pinCodePoint(end)) {
        UChar32 range[3] = { start, end+1, UNICODESET_بلند };
        retain(range, 2, 2);
    }
    أرجع *هذا;
}

/**
 * Removes the specified character from this set if it is present.
 * The set will not contain the specified range once the call
 * returns.
 */
UnicodeSet& UnicodeSet::remove(UChar32 c) U_LIFETIME_BOUND {
    أرجع remove(c, c);
}

/**
 * Removes the specified string from this set if it is present.
 * The set will not contain the specified character once the call
 * returns.
 * @param the source string
 * @return the modified set, for chaining
 */
UnicodeSet& UnicodeSet::remove(ثابت UnicodeString& s) U_LIFETIME_BOUND {
    إذا (isFrozen() || isBogus()) أرجع *هذا;
    int32_t cp = getSingleCP(s);
    إذا (cp < 0) {
        إذا (strings_ != nullptr && strings_->removeElement((فراغ*) &s)) {
            releasePattern();
        }
    } وإلا {
        remove(static_cast<UChar32>(cp), static_cast<UChar32>(cp));
    }
    أرجع *هذا;
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
    إذا (isFrozen() || isBogus()) {
        أرجع *هذا;
    }
    إذا (pinCodePoint(start) <= pinCodePoint(end)) {
        UChar32 range[3] = { start, end+1, UNICODESET_بلند };
        exclusiveOr(range, 2, 0);
    }
    releasePattern();
    أرجع *هذا;
}

UnicodeSet& UnicodeSet::complement(UChar32 c) U_LIFETIME_BOUND {
    أرجع complement(c, c);
}

/**
 * This is equivalent to
 * <code>complement(MIN_VALUE, MAX_VALUE)</code>.
 */
UnicodeSet& UnicodeSet::complement() U_LIFETIME_BOUND {
    إذا (isFrozen() || isBogus()) {
        أرجع *هذا;
    }
    إذا (list[0] == UNICODESET_کم) {
        uprv_memmove(list, list + 1, (size_t)(len-1)*حجم(UChar32));
        --len;
    } وإلا {
        إذا (!ensureCapacity(len+1)) {
            أرجع *هذا;
        }
        uprv_memmove(list + 1, list, (size_t)len*حجم(UChar32));
        list[0] = UNICODESET_کم;
        ++len;
    }
    releasePattern();
    أرجع *هذا;
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
    إذا (isFrozen() || isBogus()) أرجع *هذا;
    int32_t cp = getSingleCP(s);
    إذا (cp < 0) {
        إذا (stringsContains(s)) {
            strings_->removeElement((فراغ*) &s);
        } وإلا {
            _add(s);
        }
        releasePattern();
    } وإلا {
        complement(static_cast<UChar32>(cp), static_cast<UChar32>(cp));
    }
    أرجع *هذا;
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
    إذا ( c.len>0 && c.list!=nullptr ) {
        add(c.list, c.len, 0);
    }

    // Add strings in order
    إذا ( c.strings_!=nullptr ) {
        لكل (int32_t i=0; i<c.strings_->size(); ++i) {
            ثابت UnicodeString* s = static_cast<ثابت UnicodeString*>(c.strings_->elementAt(i));
            إذا (!stringsContains(*s)) {
                _add(*s);
            }
        }
    }
    أرجع *هذا;
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
    إذا (isFrozen() || isBogus()) {
        أرجع *هذا;
    }
    retain(c.list, c.len, 0);
    إذا (hasStrings()) {
        إذا (!c.hasStrings()) {
            strings_->removeAllElements();
        } وإلا {
            strings_->retainAll(*c.strings_);
        }
    }
    أرجع *هذا;
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
    إذا (isFrozen() || isBogus()) {
        أرجع *هذا;
    }
    retain(c.list, c.len, 2);
    إذا (hasStrings() && c.hasStrings()) {
        strings_->removeAll(*c.strings_);
    }
    أرجع *هذا;
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
    إذا (isFrozen() || isBogus()) {
        أرجع *هذا;
    }
    exclusiveOr(c.list, c.len, 0);

    إذا (c.strings_ != nullptr) {
        لكل (int32_t i=0; i<c.strings_->size(); ++i) {
            فراغ* e = c.strings_->elementAt(i);
            إذا (strings_ == nullptr || !strings_->removeElement(e)) {
                _add(*static_cast<ثابت UnicodeString*>(e));
            }
        }
    }
    أرجع *هذا;
}

/**
 * Removes all of the elements from this set.  This set will be
 * empty after this call returns.
 */
UnicodeSet& UnicodeSet::clear() U_LIFETIME_BOUND {
    إذا (isFrozen()) {
        أرجع *هذا;
    }
    list[0] = UNICODESET_بلند;
    len = 1;
    releasePattern();
    إذا (strings_ != nullptr) {
        strings_->removeAllElements();
    }
    // Remove bogus
    fFlags = 0;
    أرجع *هذا;
}

/**
 * Iteration method that returns the number of ranges contained in
 * this set.
 * @see #getRangeStart
 * @see #getRangeEnd
 */
int32_t UnicodeSet::get_Range_گنتی() ثابت {
    أرجع len/2;
}

/**
 * Iteration method that returns the first character in the
 * specified range of this set.
 * @see #getRangeCount
 * @see #getRangeEnd
 */
UChar32 UnicodeSet::getRangeStart(int32_t index) ثابت {
    أرجع list[index*2];
}

/**
 * Iteration method that returns the last character in the
 * specified range of this set.
 * @see #getRangeStart
 * @see #getRangeEnd
 */
UChar32 UnicodeSet::getRangeEnd(int32_t index) ثابت {
    أرجع list[index*2 + 1] - 1;
}

ثابت UnicodeString* UnicodeSet::getString(int32_t index) ثابت U_LIFETIME_BOUND {
    أرجع static_cast<ثابت UnicodeString*>(strings_->elementAt(index));
}

/**
 * Reallocate this objects internal structures to take up the least
 * possible space, without changing this object's value.
 */
UnicodeSet& UnicodeSet::compact() U_LIFETIME_BOUND {
    إذا (isFrozen() || isBogus()) {
        أرجع *هذا;
    }
    // Delete buffer first to defragment memory less.
    إذا (buffer != ڈھیر_List) {
        uprv_free(buffer);
        buffer = nullptr;
        bufferCapacity = 0;
    }
    إذا (list == ڈھیر_List) {
        // pass
    } وإلا إذا (len <= INITIAL_CAPACITY) {
        uprv_memcpy(ڈھیر_List, list, len * حجم(UChar32));
        uprv_free(list);
        list = ڈھیر_List;
        capacity = INITIAL_CAPACITY;
    } وإلا إذا ((len + 7) < capacity) {
        // If we have more than a little unused capacity, shrink it to len.
        UChar32* temp = static_cast<UChar32*>(uprv_realloc(list, حجم(UChar32) * len));
        إذا (temp) {
            list = temp;
            capacity = len;
        }
        // else what the heck happened?! We allocated less memory!
        // Oh well. We'll keep our original array.
    }
    إذا (strings_ != nullptr && strings_->isEmpty()) {
        delete strings_;
        strings_ = nullptr;
    }
    أرجع *هذا;
}

#ifdef DEBUG_SERIALIZE
#ضمن <stdio.h>
#endif

/**
 * Deserialize constructor.
 */
UnicodeSet::UnicodeSet(ثابت uint16_t data[], int32_t dataLen, ESerialization serialization,
                       UErrorCode &ec) {

  إذا(U_FAILURE(ec)) {
    setToBogus();
    أرجع;
  }

  إذا( (serialization != kSerialized)
      || (data==nullptr)
      || (dataLen < 1)) {
    ec = U_ILLEGAL_ARGUMENT_ERROR;
    setToBogus();
    أرجع;
  }

  // bmp?
  int32_t headerSize = ((data[0]&0x8000)) ?2:1;
  int32_t bmpLength = (headerSize==1)?data[0]:data[1];

  int32_t newLength = (((data[0]&0x7FFF)-bmpLength)/2)+bmpLength;
#ifdef DEBUG_SERIALIZE
  printf("dataLen %d headerSize %d bmpLen %d len %d. data[0]=%X/%X/%X/%X\n", dataLen,headerSize,bmpLength,newLength, data[0],data[1],data[2],data[3]);
#endif
  إذا(!ensureCapacity(newLength + 1)) {  // +1 for HIGH
    أرجع;
  }
  // copy bmp
  int32_t i;
  لكل(i = 0; i< bmpLength;i++) {
    list[i] = data[i+headerSize];
#ifdef DEBUG_SERIALIZE
    printf("<<16@%d[%d] %X\n", i+headerSize, i, list[i]);
#endif
  }
  // copy smp
  لكل(i=bmpLength;i<newLength;i++) {
    list[i] = (static_cast<UChar32>(data[headerSize + bmpLength + (i - bmpLength) * 2 + 0]) << 16) +
               static_cast<UChar32>(data[headerSize + bmpLength + (i - bmpLength) * 2 + 1]);
#ifdef DEBUG_SERIALIZE
    printf("<<32@%d+[%d] %lX\n", headerSize+bmpLength+i, i, list[i]);
#endif
  }
  U_ASSERT(i == newLength);
  إذا (i == 0 || list[i - 1] != UNICODESET_بلند) {
    list[i++] = UNICODESET_بلند;
  }
  len = i;
}


int32_t UnicodeSet::serialize(uint16_t *dest, int32_t destCapacity, UErrorCode& ec) ثابت {
    int32_t bmpLength, length, destLength;

    إذا (U_FAILURE(ec)) {
        أرجع 0;
    }

    إذا (destCapacity<0 || (destCapacity>0 && dest==nullptr)) {
        ec=U_ILLEGAL_ARGUMENT_ERROR;
        أرجع 0;
    }

    /* count necessary 16-bit units */
    length=هذا->len-1; // Subtract 1 to ignore final UNICODESET_HIGH
    // assert(length>=0);
    إذا (length==0) {
        /* empty set */
        إذا (destCapacity>0) {
            *dest=0;
        } وإلا {
            ec=U_BUFFER_OVERFLOW_ERROR;
        }
        أرجع 1;
    }
    /* now length>0 */

    إذا (هذا->list[length-1]<=0xffff) {
        /* all BMP */
        bmpLength=length;
    } وإلا إذا (هذا->list[0]>=0x10000) {
        /* all supplementary */
        bmpLength=0;
        length*=2;
    } وإلا {
        /* some BMP, some supplementary */
        لكل (bmpLength=0; bmpLength<length && هذا->list[bmpLength]<=0xffff; ++bmpLength) {}
        length=bmpLength+2*(length-bmpLength);
    }
#ifdef DEBUG_SERIALIZE
    printf(">> bmpLength%d length%d len%d\n", bmpLength, length, len);
#endif
    /* length: number of 16-bit array units */
    إذا (length>0x7fff) {
        /* there are only 15 bits for the length in the first serialized word */
        ec=U_INDEX_OUTOFBOUNDS_ERROR;
        أرجع 0;
    }

    /*
     * total serialized length:
     * number of 16-bit array units (length) +
     * 1 length unit (always) +
     * 1 bmpLength unit (if there are supplementary values)
     */
    destLength=length+((length>bmpLength)?2:1);
    إذا (destLength<=destCapacity) {
        ثابت UChar32 *p;
        int32_t i;

#ifdef DEBUG_SERIALIZE
        printf("writeHdr\n");
#endif
        *dest = static_cast<uint16_t>(length);
        إذا (length>bmpLength) {
            *dest|=0x8000;
            *++dest = static_cast<uint16_t>(bmpLength);
        }
        ++dest;

        /* write the BMP part of the array */
        p=هذا->list;
        لكل (i=0; i<bmpLength; ++i) {
#ifdef DEBUG_SERIALIZE
          printf("writebmp: %x\n", (صحيح)*p);
#endif
            *dest++ = static_cast<uint16_t>(*p++);
        }

        /* write the supplementary part of the array */
        لكل (; i<length; i+=2) {
#ifdef DEBUG_SERIALIZE
          printf("write32: %x\n", (صحيح)*p);
#endif
            *dest++ = static_cast<uint16_t>(*p >> 16);
            *dest++ = static_cast<uint16_t>(*p++);
        }
    } وإلا {
        ec=U_BUFFER_OVERFLOW_ERROR;
    }
    أرجع destLength;
}

//----------------------------------------------------------------
// Implementation: Utility methods
//----------------------------------------------------------------

/**
 * Allocate our strings vector and return true if successful.
 */
UBool UnicodeSet::allocateStrings(UErrorCode &status) {
    إذا (U_FAILURE(status)) {
        أرجع خطأ;
    }
    strings_ = جديد UVector(uprv_deleteUObject,
                          uhash_compareUnicodeString, 1, status);
    إذا (strings_ == nullptr) { // Check for memory allocation error.
        status = U_MEMORY_ALLOCATION_ERROR;
        أرجع خطأ;
    }
    إذا (U_FAILURE(status)) {
        delete strings_;
        strings_ = nullptr;
        أرجع خطأ;
    } 
    أرجع true;
}

int32_t UnicodeSet::nextCapacity(int32_t minCapacity) {
    // Grow exponentially to reduce the frequency of allocations.
    إذا (minCapacity < INITIAL_CAPACITY) {
        أرجع minCapacity + INITIAL_CAPACITY;
    } وإلا إذا (minCapacity <= 2500) {
        أرجع 5 * minCapacity;
    } وإلا {
        int32_t newCapacity = 2 * minCapacity;
        إذا (newCapacity > MAX_LENGTH) {
            newCapacity = MAX_LENGTH;
        }
        أرجع newCapacity;
    }
}

منطقي UnicodeSet::ensureCapacity(int32_t newLen) {
    إذا (newLen > MAX_LENGTH) {
        newLen = MAX_LENGTH;
    }
    إذا (newLen <= capacity) {
        أرجع true;
    }
    int32_t newCapacity = nextCapacity(newLen);
    UChar32* temp = static_cast<UChar32*>(uprv_malloc(newCapacity * حجم(UChar32)));
    إذا (temp == nullptr) {
        setToBogus(); // set the object to bogus state if an OOM failure occurred.
        أرجع خطأ;
    }
    // Copy only the actual contents.
    uprv_memcpy(temp, list, len * حجم(UChar32));
    إذا (list != ڈھیر_List) {
        uprv_free(list);
    }
    list = temp;
    capacity = newCapacity;
    أرجع true;
}

منطقي UnicodeSet::ensureBufferCapacity(int32_t newLen) {
    إذا (newLen > MAX_LENGTH) {
        newLen = MAX_LENGTH;
    }
    إذا (newLen <= bufferCapacity) {
        أرجع true;
    }
    int32_t newCapacity = nextCapacity(newLen);
    UChar32* temp = static_cast<UChar32*>(uprv_malloc(newCapacity * حجم(UChar32)));
    إذا (temp == nullptr) {
        setToBogus();
        أرجع خطأ;
    }
    // The buffer has no contents to be copied.
    // It is always filled from scratch after this call.
    إذا (buffer != ڈھیر_List) {
        uprv_free(buffer);
    }
    buffer = temp;
    bufferCapacity = newCapacity;
    أرجع true;
}

/**
 * Swap list and buffer.
 */
فراغ UnicodeSet::swapBuffers() {
    // swap list and buffer
    UChar32* temp = list;
    list = buffer;
    buffer = temp;

    int32_t c = capacity;
    capacity = bufferCapacity;
    bufferCapacity = c;
}

فراغ UnicodeSet::setToBogus() {
    clear(); // Remove everything in the set.
    fFlags = kIsBogus;
}

//----------------------------------------------------------------
// Implementation: Fundamental operators
//----------------------------------------------------------------

ساكن مضمن UChar32 max(UChar32 a, UChar32 b) {
    أرجع (a > b) ? a : b;
}

// polarity = 0, 3 is normal: x xor y
// polarity = 1, 2: x xor ~y == x === y

فراغ UnicodeSet::exclusiveOr(ثابت UChar32* other, int32_t otherLen, int8_t polarity) {
    إذا (isFrozen() || isBogus()) {
        أرجع;
    }
    إذا (!ensureBufferCapacity(len + otherLen)) {
        أرجع;
    }

    int32_t i = 0, j = 0, k = 0;
    UChar32 a = list[i++];
    UChar32 b;
    إذا (polarity == 1 || polarity == 2) {
        b = UNICODESET_کم;
        إذا (other[j] == UNICODESET_کم) { // skip base if already LOW
            ++j;
            b = other[j];
        }
    } وإلا {
        b = other[j++];
    }
    // simplest of all the routines
    // sort the values, discarding identicals!
    لكل (;;) {
        إذا (a < b) {
            buffer[k++] = a;
            a = list[i++];
        } وإلا إذا (b < a) {
            buffer[k++] = b;
            b = other[j++];
        } وإلا إذا (a != UNICODESET_بلند) { // at this point, a == b
            // discard both values!
            a = list[i++];
            b = other[j++];
        } وإلا { // DONE!
            buffer[k++] = UNICODESET_بلند;
            len = k;
            اكسر;
        }
    }
    swapBuffers();
    releasePattern();
}

// polarity = 0 is normal: x union y
// polarity = 2: x union ~y
// polarity = 1: ~x union y
// polarity = 3: ~x union ~y

فراغ UnicodeSet::add(ثابت UChar32* other, int32_t otherLen, int8_t polarity) {
    إذا (isFrozen() || isBogus() || other==nullptr) {
        أرجع;
    }
    إذا (!ensureBufferCapacity(len + otherLen)) {
        أرجع;
    }

    int32_t i = 0, j = 0, k = 0;
    UChar32 a = list[i++];
    UChar32 b = other[j++];
    // change from xor is that we have to check overlapping pairs
    // polarity bit 1 means a is second, bit 2 means b is.
    لكل (;;) {
        بدل (polarity) {
          حالة 0: // both first; take lower if unequal
            إذا (a < b) { // take a
                // Back up over overlapping ranges in buffer[]
                إذا (k > 0 && a <= buffer[k-1]) {
                    // Pick latter end value in buffer[] vs. list[]
                    a = max(list[i], buffer[--k]);
                } وإلا {
                    // No overlap
                    buffer[k++] = a;
                    a = list[i];
                }
                i++; // Common if/else code factored out
                polarity ^= 1;
            } وإلا إذا (b < a) { // take b
                إذا (k > 0 && b <= buffer[k-1]) {
                    b = max(other[j], buffer[--k]);
                } وإلا {
                    buffer[k++] = b;
                    b = other[j];
                }
                j++;
                polarity ^= 2;
            } وإلا { // a == b, take a, drop b
                إذا (a == UNICODESET_بلند) اذهب loop_end;
                // This is symmetrical; it doesn't matter if
                // we backtrack with a or b. - liu
                إذا (k > 0 && a <= buffer[k-1]) {
                    a = max(list[i], buffer[--k]);
                } وإلا {
                    // No overlap
                    buffer[k++] = a;
                    a = list[i];
                }
                i++;
                polarity ^= 1;
                b = other[j++];
                polarity ^= 2;
            }
            اكسر;
          حالة 3: // both second; take higher if unequal, and drop other
            إذا (b <= a) { // take a
                إذا (a == UNICODESET_بلند) اذهب loop_end;
                buffer[k++] = a;
            } وإلا { // take b
                إذا (b == UNICODESET_بلند) اذهب loop_end;
                buffer[k++] = b;
            }
            a = list[i++];
            polarity ^= 1;   // factored common code
            b = other[j++];
            polarity ^= 2;
            اكسر;
          حالة 1: // a second, b first; if b < a, overlap
            إذا (a < b) { // no overlap, take a
                buffer[k++] = a; a = list[i++]; polarity ^= 1;
            } وإلا إذا (b < a) { // OVERLAP, drop b
                b = other[j++];
                polarity ^= 2;
            } وإلا { // a == b, drop both!
                إذا (a == UNICODESET_بلند) اذهب loop_end;
                a = list[i++];
                polarity ^= 1;
                b = other[j++];
                polarity ^= 2;
            }
            اكسر;
          حالة 2: // a first, b second; if a < b, overlap
            إذا (b < a) { // no overlap, take b
                buffer[k++] = b;
                b = other[j++];
                polarity ^= 2;
            } وإلا  إذا (a < b) { // OVERLAP, drop a
                a = list[i++];
                polarity ^= 1;
            } وإلا { // a == b, drop both!
                إذا (a == UNICODESET_بلند) اذهب loop_end;
                a = list[i++];
                polarity ^= 1;
                b = other[j++];
                polarity ^= 2;
            }
            اكسر;
        }
    }
 loop_end:
    buffer[k++] = UNICODESET_بلند;    // terminate
    len = k;
    swapBuffers();
    releasePattern();
}

// polarity = 0 is normal: x intersect y
// polarity = 2: x intersect ~y == set-minus
// polarity = 1: ~x intersect y
// polarity = 3: ~x intersect ~y

فراغ UnicodeSet::retain(ثابت UChar32* other, int32_t otherLen, int8_t polarity) {
    إذا (isFrozen() || isBogus()) {
        أرجع;
    }
    إذا (!ensureBufferCapacity(len + otherLen)) {
        أرجع;
    }

    int32_t i = 0, j = 0, k = 0;
    UChar32 a = list[i++];
    UChar32 b = other[j++];
    // change from xor is that we have to check overlapping pairs
    // polarity bit 1 means a is second, bit 2 means b is.
    لكل (;;) {
        بدل (polarity) {
          حالة 0: // both first; drop the smaller
            إذا (a < b) { // drop a
                a = list[i++];
                polarity ^= 1;
            } وإلا إذا (b < a) { // drop b
                b = other[j++];
                polarity ^= 2;
            } وإلا { // a == b, take one, drop other
                إذا (a == UNICODESET_بلند) اذهب loop_end;
                buffer[k++] = a;
                a = list[i++];
                polarity ^= 1;
                b = other[j++];
                polarity ^= 2;
            }
            اكسر;
          حالة 3: // both second; take lower if unequal
            إذا (a < b) { // take a
                buffer[k++] = a;
                a = list[i++];
                polarity ^= 1;
            } وإلا إذا (b < a) { // take b
                buffer[k++] = b;
                b = other[j++];
                polarity ^= 2;
            } وإلا { // a == b, take one, drop other
                إذا (a == UNICODESET_بلند) اذهب loop_end;
                buffer[k++] = a;
                a = list[i++];
                polarity ^= 1;
                b = other[j++];
                polarity ^= 2;
            }
            اكسر;
          حالة 1: // a second, b first;
            إذا (a < b) { // NO OVERLAP, drop a
                a = list[i++];
                polarity ^= 1;
            } وإلا إذا (b < a) { // OVERLAP, take b
                buffer[k++] = b;
                b = other[j++];
                polarity ^= 2;
            } وإلا { // a == b, drop both!
                إذا (a == UNICODESET_بلند) اذهب loop_end;
                a = list[i++];
                polarity ^= 1;
                b = other[j++];
                polarity ^= 2;
            }
            اكسر;
          حالة 2: // a first, b second; if a < b, overlap
            إذا (b < a) { // no overlap, drop b
                b = other[j++];
                polarity ^= 2;
            } وإلا  إذا (a < b) { // OVERLAP, take a
                buffer[k++] = a;
                a = list[i++];
                polarity ^= 1;
            } وإلا { // a == b, drop both!
                إذا (a == UNICODESET_بلند) اذهب loop_end;
                a = list[i++];
                polarity ^= 1;
                b = other[j++];
                polarity ^= 2;
            }
            اكسر;
        }
    }
 loop_end:
    buffer[k++] = UNICODESET_بلند;    // terminate
    len = k;
    swapBuffers();
    releasePattern();
}

/**
 * Append the <code>toPattern()</code> representation of a
 * string to the given <code>StringBuffer</code>.
 */
فراغ UnicodeSet::_appendToPat(UnicodeString& buf, ثابت UnicodeString& s, UBool escapeUnprintable) {
    UChar32 cp;
    لكل (int32_t i = 0; i < s.length(); i += U16_LENGTH(cp)) {
        _appendToPat(buf, cp = s.char32At(i), escapeUnprintable);
    }
}

/**
 * Append the <code>toPattern()</code> representation of a
 * character to the given <code>StringBuffer</code>.
 */
فراغ UnicodeSet::_appendToPat(UnicodeString& buf, UChar32 c, UBool escapeUnprintable) {
    إذا (escapeUnprintable ? ICU_Utility::isUnprintable(c) : ICU_Utility::shouldAlwaysBeEscaped(c)) {
        // Use hex escape notation (\uxxxx or \Uxxxxxxxx) for anything
        // unprintable
        ICU_Utility::escape(buf, c);
        أرجع;
    }
    // Okay to let ':' pass through
    بدل (c) {
    حالة u'[':
    حالة u']':
    حالة u'-':
    حالة u'^':
    حالة u'&':
    حالة u'\\':
    حالة u'{':
    حالة u'}':
    حالة u':':
    حالة علامت_Table::علامت_REF:
        buf.append(u'\\');
        اكسر;
    افتراضي:
        // Escape whitespace
        إذا (PatternProps::isWhiteSpace(c)) {
            buf.append(u'\\');
        }
        اكسر;
    }
    buf.append(c);
}

فراغ UnicodeSet::_appendToPat(UnicodeString &result, UChar32 start, UChar32 end,
                              UBool escapeUnprintable) {
    _appendToPat(result, start, escapeUnprintable);
    إذا (start != end) {
        إذا ((start+1) != end ||
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
    إذا (pat != nullptr) {
        int32_t i;
        int32_t backslash_گنتی = 0;
        لكل (i=0; i<patLen; ) {
            UChar32 c;
            U16_NEXT(pat, i, patLen, c);
            إذا (escapeUnprintable ?
                    ICU_Utility::isUnprintable(c) : ICU_Utility::shouldAlwaysBeEscaped(c)) {
                // If the unprintable character is preceded by an odd
                // number of backslashes, then it has been escaped.
                // Before unescaping it, we delete the final
                // backslash.
                إذا ((backslash_گنتی % 2) == 1) {
                    result.truncate(result.length() - 1);
                }
                ICU_Utility::escape(result, c);
                backslash_گنتی = 0;
            } وإلا {
                result.append(c);
                إذا (c == u'\\') {
                    ++backslash_گنتی;
                } وإلا {
                    backslash_گنتی = 0;
                }
            }
        }
        أرجع result;
    }

    أرجع _generatePattern(result, escapeUnprintable);
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
    أرجع _toPattern(result, escapeUnprintable);
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
    إذا (len >= 4 && list[0] == 0 && limit == len && !hasStrings()) {
        // Emit the inverse
        result.append(u'^');
        // Offsetting the inversion list index by one lets us
        // iterate over the ranges of the set complement.
        i = 1;
        --limit;
    }

    // Emit the ranges as pairs.
    طالما (i < limit) {
        UChar32 start = list[i];  // getRangeStart()
        UChar32 end = list[i + 1] - 1;  // getRangeEnd() = range limit minus one
        إذا (!(0xd800 <= end && end <= 0xdbff)) {
            _appendToPat(result, start, end, escapeUnprintable);
            i += 2;
        } وإلا {
            // The range ends with a lead surrogate.
            // Avoid writing what looks like a lead+trail surrogate pair.
            // 1. Postpone ranges that start with a lead surrogate code point.
            int32_t firstLead = i;
            طالما ((i += 2) < limit && list[i] <= 0xdbff) {}
            int32_t firstAfterLead = i;
            // 2. Write following ranges that start with a trail surrogate code point.
            طالما (i < limit && (start = list[i]) <= 0xdfff) {
                _appendToPat(result, start, list[i + 1] - 1, escapeUnprintable);
                i += 2;
            }
            // 3. Now write the postponed ranges.
            لكل (صحيح j = firstLead; j < firstAfterLead; j += 2) {
                _appendToPat(result, list[j], list[j + 1] - 1, escapeUnprintable);
            }
        }
    }

    إذا (strings_ != nullptr) {
        لكل (int32_t i = 0; i<strings_->size(); ++i) {
            result.append(u'{');
            _appendToPat(result,
                         *static_cast<ثابت UnicodeString*>(strings_->elementAt(i)),
                         escapeUnprintable);
            result.append(u'}');
        }
    }
    أرجع result.append(u']');
}

/**
* Release existing cached pattern
*/
فراغ UnicodeSet::releasePattern() {
    إذا (pat) {
        uprv_free(pat);
        pat = nullptr;
        patLen = 0;
    }
}

/**
* Set the new pattern to cache.
*/
فراغ UnicodeSet::setPattern(ثابت char16_t *newPat, int32_t newPatLen) {
    releasePattern();
    pat = static_cast<char16_t*>(uprv_malloc((newPatLen + 1) * حجم(char16_t)));
    إذا (pat) {
        patLen = newPatLen;
        u_memcpy(pat, newPat, patLen);
        pat[patLen] = 0;
    }
    // else we don't care if malloc failed. This was just a nice cache.
    // We can regenerate an equivalent pattern later when requested.
}

UnicodeSet* UnicodeSet::freeze() U_LIFETIME_BOUND {
    إذا(!isFrozen() && !isBogus()) {
        compact();

        // Optimize contains() and span() and similar functions.
        إذا (hasStrings()) {
            stringSpan = جديد UnicodeSetStringSpan(*هذا, *strings_, UnicodeSetStringSpan::ALL);
            إذا (stringSpan == nullptr) {
                setToBogus();
                أرجع هذا;
            } وإلا إذا (!stringSpan->needsStringSpanUTF16()) {
                // All strings are irrelevant for span() etc. because
                // all of each string's code points are contained in this set.
                // Do not check needsStringSpanUTF8() because UTF-8 has at most as
                // many relevant strings as UTF-16.
                // (Thus needsStringSpanUTF8() implies needsStringSpanUTF16().)
                delete stringSpan;
                stringSpan = nullptr;
            }
        }
        إذا (stringSpan == nullptr) {
            // No span-relevant strings: Optimize for code point spans.
            bmpSet=جديد BMPSet(list, len);
            إذا (bmpSet == nullptr) { // Check for memory allocation error.
                setToBogus();
            }
        }
    }
    أرجع هذا;
}

int32_t UnicodeSet::span(ثابت char16_t *s, int32_t length, USetSpanCondition spanCondition) ثابت {
    إذا(length>0 && bmpSet!=nullptr) {
        أرجع static_cast<int32_t>(bmpSet->span(s, s + length, spanCondition) - s);
    }
    إذا(length<0) {
        length=u_strlen(s);
    }
    إذا(length==0) {
        أرجع 0;
    }
    إذا(stringSpan!=nullptr) {
        أرجع stringSpan->span(s, length, spanCondition);
    } وإلا إذا(hasStrings()) {
        uint32_t which= spanCondition==USET_SPAN_NOT_CONTAINED ?
                            UnicodeSetStringSpan::FWD_UTF16_NOT_CONTAINED :
                            UnicodeSetStringSpan::FWD_UTF16_CONTAINED;
        UnicodeSetStringSpan strSpan(*هذا, *strings_, which);
        إذا(strSpan.needsStringSpanUTF16()) {
            أرجع strSpan.span(s, length, spanCondition);
        }
    }

    إذا(spanCondition!=USET_SPAN_NOT_CONTAINED) {
        spanCondition=USET_SPAN_CONTAINED;  // Pin to 0/1 values.
    }

    UChar32 c;
    int32_t start=0, prev=0;
    افعل {
        U16_NEXT(s, start, length, c);
        إذا(spanCondition!=contains(c)) {
            اكسر;
        }
    } طالما((prev=start)<length);
    أرجع prev;
}

int32_t UnicodeSet::spanBack(ثابت char16_t *s, int32_t length, USetSpanCondition spanCondition) ثابت {
    إذا(length>0 && bmpSet!=nullptr) {
        أرجع static_cast<int32_t>(bmpSet->spanBack(s, s + length, spanCondition) - s);
    }
    إذا(length<0) {
        length=u_strlen(s);
    }
    إذا(length==0) {
        أرجع 0;
    }
    إذا(stringSpan!=nullptr) {
        أرجع stringSpan->spanBack(s, length, spanCondition);
    } وإلا إذا(hasStrings()) {
        uint32_t which= spanCondition==USET_SPAN_NOT_CONTAINED ?
                            UnicodeSetStringSpan::BACK_UTF16_NOT_CONTAINED :
                            UnicodeSetStringSpan::BACK_UTF16_CONTAINED;
        UnicodeSetStringSpan strSpan(*هذا, *strings_, which);
        إذا(strSpan.needsStringSpanUTF16()) {
            أرجع strSpan.spanBack(s, length, spanCondition);
        }
    }

    إذا(spanCondition!=USET_SPAN_NOT_CONTAINED) {
        spanCondition=USET_SPAN_CONTAINED;  // Pin to 0/1 values.
    }

    UChar32 c;
    int32_t prev=length;
    افعل {
        U16_PREV(s, 0, length, c);
        إذا(spanCondition!=contains(c)) {
            اكسر;
        }
    } طالما((prev=length)>0);
    أرجع prev;
}

int32_t UnicodeSet::spanUTF8(ثابت حرف *s, int32_t length, USetSpanCondition spanCondition) ثابت {
    إذا(length>0 && bmpSet!=nullptr) {
        ثابت uint8_t* s0 = reinterpret_cast<ثابت uint8_t*>(s);
        أرجع static_cast<int32_t>(bmpSet->spanUTF8(s0, length, spanCondition) - s0);
    }
    إذا(length<0) {
        length = static_cast<int32_t>(uprv_strlen(s));
    }
    إذا(length==0) {
        أرجع 0;
    }
    إذا(stringSpan!=nullptr) {
        أرجع stringSpan->spanUTF8(reinterpret_cast<ثابت uint8_t*>(s), length, spanCondition);
    } وإلا إذا(hasStrings()) {
        uint32_t which= spanCondition==USET_SPAN_NOT_CONTAINED ?
                            UnicodeSetStringSpan::FWD_UTF8_NOT_CONTAINED :
                            UnicodeSetStringSpan::FWD_UTF8_CONTAINED;
        UnicodeSetStringSpan strSpan(*هذا, *strings_, which);
        إذا(strSpan.needsStringSpanUTF8()) {
            أرجع strSpan.spanUTF8(reinterpret_cast<ثابت uint8_t*>(s), length, spanCondition);
        }
    }

    إذا(spanCondition!=USET_SPAN_NOT_CONTAINED) {
        spanCondition=USET_SPAN_CONTAINED;  // Pin to 0/1 values.
    }

    UChar32 c;
    int32_t start=0, prev=0;
    افعل {
        U8_NEXT_OR_FFFD(s, start, length, c);
        إذا(spanCondition!=contains(c)) {
            اكسر;
        }
    } طالما((prev=start)<length);
    أرجع prev;
}

int32_t UnicodeSet::spanBackUTF8(ثابت حرف *s, int32_t length, USetSpanCondition spanCondition) ثابت {
    إذا(length>0 && bmpSet!=nullptr) {
        ثابت uint8_t* s0 = reinterpret_cast<ثابت uint8_t*>(s);
        أرجع bmpSet->spanBackUTF8(s0, length, spanCondition);
    }
    إذا(length<0) {
        length = static_cast<int32_t>(uprv_strlen(s));
    }
    إذا(length==0) {
        أرجع 0;
    }
    إذا(stringSpan!=nullptr) {
        أرجع stringSpan->spanBackUTF8(reinterpret_cast<ثابت uint8_t*>(s), length, spanCondition);
    } وإلا إذا(hasStrings()) {
        uint32_t which= spanCondition==USET_SPAN_NOT_CONTAINED ?
                            UnicodeSetStringSpan::BACK_UTF8_NOT_CONTAINED :
                            UnicodeSetStringSpan::BACK_UTF8_CONTAINED;
        UnicodeSetStringSpan strSpan(*هذا, *strings_, which);
        إذا(strSpan.needsStringSpanUTF8()) {
            أرجع strSpan.spanBackUTF8(reinterpret_cast<ثابت uint8_t*>(s), length, spanCondition);
        }
    }

    إذا(spanCondition!=USET_SPAN_NOT_CONTAINED) {
        spanCondition=USET_SPAN_CONTAINED;  // Pin to 0/1 values.
    }

    UChar32 c;
    int32_t prev=length;
    افعل {
        U8_PREV_OR_FFFD(s, 0, length, c);
        إذا(spanCondition!=contains(c)) {
            اكسر;
        }
    } طالما((prev=length)>0);
    أرجع prev;
}

U_NAMESPACE_END
