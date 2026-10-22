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

#অন্তর্ভাব "unicode/utypes.h"
#অন্তর্ভাব "unicode/parsepos.h"
#অন্তর্ভাব "unicode/symtable.h"
#অন্তর্ভাব "unicode/uniset.h"
#অন্তর্ভাব "unicode/ustring.h"
#অন্তর্ভাব "unicode/utf8.h"
#অন্তর্ভাব "unicode/utf16.h"
#অন্তর্ভাব "ruleiter.h"
#অন্তর্ভাব "cmemory.h"
#অন্তর্ভাব "cstring.h"
#অন্তর্ভাব "patternprops.h"
#অন্তর্ভাব "uelement.h"
#অন্তর্ভাব "util.h"
#অন্তর্ভাব "uvector.h"
#অন্তর্ভাব "charstr.h"
#অন্তর্ভাব "ustrfmt.h"
#অন্তর্ভাব "uassert.h"
#অন্তর্ভাব "bmpset.h"
#অন্তর্ভাব "unisetspan.h"

// HIGH_VALUE > all valid values. 110000 for codepoints
#define UNICODESET_উপরের 0x0110000

// LOW <= all valid values. ZERO for codepoints
#define UNICODESET_নিচের 0x000000

/** Max list [0, 1, 2, ..., max code point, HIGH] */
constexpr int32_t MAX_LENGTH = UNICODESET_উপরের + 1;

U_NAMESPACE_BEGIN

প্রতীক_Table::~প্রতীক_Table() {}

UOBJECT_DEFINE_RTTI_IMPLEMENTATION(UnicodeSet)

/**
 * Modify the given UChar32 variable so that it is in range, by
 * pinning values < UNICODESET_LOW to UNICODESET_LOW, and
 * pinning values > UNICODESET_HIGH-1 to UNICODESET_HIGH-1.
 * It modifies its argument in-place and also returns it.
 */
স্থির অন্তঃস্থ UChar32 pinCodePoint(UChar32& c) {
    যদি (c < UNICODESET_নিচের) {
        c = UNICODESET_নিচের;
    } অন্যথা যদি (c > (UNICODESET_উপরের-1)) {
        c = (UNICODESET_উপরের-1);
    }
    প্রত্যাগম c;
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
#অন্তর্ভাব <stdio.h>
স্থির int32_t dbg_গুনতি = 0;

স্থির অন্তঃস্থ শূন্য _dbgct(UnicodeSet* set) {
    UnicodeString str;
    set->toPattern(str, সত্য);
    বর্ণ buf[40];
    str.extract(0, 39, buf, "");
    printf("DEBUG UnicodeSet: ct 0x%08X; %d %s\n", set, ++dbg_গুনতি, buf);
}

স্থির অন্তঃস্থ শূন্য _dbgdt(UnicodeSet* set) {
    UnicodeString str;
    set->toPattern(str, সত্য);
    বর্ণ buf[40];
    str.extract(0, 39, buf, "");
    printf("DEBUG UnicodeSet: dt 0x%08X; %d %s\n", set, --dbg_গুনতি, buf);
}

#অন্যথা

#define _dbgct(set)
#define _dbgdt(set)

#endif

//----------------------------------------------------------------
// UnicodeString in UVector support
//----------------------------------------------------------------

স্থির শূন্য U_CALLCONV cloneUnicodeString(UElement *dst, UElement *src) {
    dst->pointer = নূতন UnicodeString(*static_cast<UnicodeString*>(src->pointer));
}

স্থির int32_t U_CALLCONV compareUnicodeString(UElement t1, UElement t2) {
    ধ্রুব UnicodeString& a = *static_cast<ধ্রুব UnicodeString*>(t1.pointer);
    ধ্রুব UnicodeString& b = *static_cast<ধ্রুব UnicodeString*>(t2.pointer);
    প্রত্যাগম a.compare(b);
}

UBool UnicodeSet::hasStrings() ধ্রুব {
    প্রত্যাগম strings_ != nullptr && !strings_->isEmpty();
}

int32_t UnicodeSet::stringsSize() ধ্রুব {
    প্রত্যাগম strings_ == nullptr ? 0 : strings_->size();
}

UBool UnicodeSet::stringsContains(ধ্রুব UnicodeString &s) ধ্রুব {
    প্রত্যাগম strings_ != nullptr && strings_->contains((শূন্য*) &s);
}

//----------------------------------------------------------------
// Constructors &c
//----------------------------------------------------------------

/**
 * Constructs an empty set.
 */
UnicodeSet::UnicodeSet() {
    list[0] = UNICODESET_উপরের;
    _dbgct(এতত্);
}

/**
 * Constructs a set containing the given range. If <code>end >
 * start</code> then an empty set is created.
 *
 * @param start first character, inclusive, of range
 * @param end last character, inclusive, of range
 */
UnicodeSet::UnicodeSet(UChar32 start, UChar32 end) {
    list[0] = UNICODESET_উপরের;
    add(start, end);
    _dbgct(এতত্);
}

/**
 * Constructs a set that is identical to the given UnicodeSet.
 */
UnicodeSet::UnicodeSet(ধ্রুব UnicodeSet& o) : UnicodeFilter(o) {
    *এতত্ = o;
    _dbgct(এতত্);
}

// Copy-construct as thawed.
UnicodeSet::UnicodeSet(ধ্রুব UnicodeSet& o, UBool /* asThawed */) : UnicodeFilter(o) {
    যদি (ensureCapacity(o.len)) {
        // *this = o except for bmpSet and stringSpan
        len = o.len;
        uprv_memcpy(list, o.list, (size_t)len*পরিমাণ(UChar32));
        যদি (o.hasStrings()) {
            UErrorCode status = U_ZERO_ERROR;
            যদি (!allocateStrings(status) ||
                    (strings_->assign(*o.strings_, cloneUnicodeString, status), U_FAILURE(status))) {
                setToBogus();
                প্রত্যাগম;
            }
        }
        যদি (o.pat) {
            setPattern(o.pat, o.patLen);
        }
        _dbgct(এতত্);
    }
}

/**
 * Destructs the set.
 */
UnicodeSet::~UnicodeSet() {
    _dbgdt(এতত্); // first!
    যদি (list != স্তূপ_List) {
        uprv_free(list);
    }
    delete bmpSet;
    যদি (buffer != স্তূপ_List) {
        uprv_free(buffer);
    }
    delete strings_;
    delete stringSpan;
    releasePattern();
}

/**
 * Assigns this object to be a copy of another.
 */
UnicodeSet& UnicodeSet::operator=(ধ্রুব UnicodeSet& o) U_LIFETIME_BOUND {
    প্রত্যাগম copyFrom(o, অসত্য);
}

UnicodeSet& UnicodeSet::copyFrom(ধ্রুব UnicodeSet& o, UBool asThawed) U_LIFETIME_BOUND {
    যদি (এতত্ == &o) {
        প্রত্যাগম *এতত্;
    }
    যদি (isFrozen()) {
        প্রত্যাগম *এতত্;
    }
    যদি (o.isBogus()) {
        setToBogus();
        প্রত্যাগম *এতত্;
    }
    যদি (!ensureCapacity(o.len)) {
        // ensureCapacity will mark the UnicodeSet as Bogus if OOM failure happens.
        প্রত্যাগম *এতত্;
    }
    len = o.len;
    uprv_memcpy(list, o.list, (size_t)len*পরিমাণ(UChar32));
    যদি (o.bmpSet != nullptr && !asThawed) {
        bmpSet = নূতন BMPSet(*o.bmpSet, list, len);
        যদি (bmpSet == nullptr) { // Check for memory allocation error.
            setToBogus();
            প্রত্যাগম *এতত্;
        }
    }
    যদি (o.hasStrings()) {
        UErrorCode status = U_ZERO_ERROR;
        যদি ((strings_ == nullptr && !allocateStrings(status)) ||
                (strings_->assign(*o.strings_, cloneUnicodeString, status), U_FAILURE(status))) {
            setToBogus();
            প্রত্যাগম *এতত্;
        }
    } অন্যথা যদি (hasStrings()) {
        strings_->removeAllElements();
    }
    যদি (o.stringSpan != nullptr && strings_ != nullptr && !asThawed) {
        stringSpan = নূতন UnicodeSetStringSpan(*o.stringSpan, *strings_);
        যদি (stringSpan == nullptr) { // Check for memory allocation error.
            setToBogus();
            প্রত্যাগম *এতত্;
        }
    }
    releasePattern();
    যদি (o.pat) {
        setPattern(o.pat, o.patLen);
    }
    প্রত্যাগম *এতত্;
}

/**
 * Returns a copy of this object.  All UnicodeMatcher objects have
 * to support cloning in order to allow classes using
 * UnicodeMatchers, such as Transliterator, to implement cloning.
 */
UnicodeSet* UnicodeSet::clone() ধ্রুব {
    প্রত্যাগম নূতন UnicodeSet(*এতত্);
}

UnicodeSet *UnicodeSet::cloneAsThawed() ধ্রুব {
    প্রত্যাগম নূতন UnicodeSet(*এতত্, সত্য);
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
তার্কিক UnicodeSet::operator==(ধ্রুব UnicodeSet& o) ধ্রুব {
    যদি (len != o.len) প্রত্যাগম অসত্য;
    প্রতি (int32_t i = 0; i < len; ++i) {
        যদি (list[i] != o.list[i]) প্রত্যাগম অসত্য;
    }
    যদি (hasStrings() != o.hasStrings()) { প্রত্যাগম অসত্য; }
    যদি (hasStrings() && *strings_ != *o.strings_) প্রত্যাগম অসত্য;
    প্রত্যাগম সত্য;
}

/**
 * Returns the hash code value for this set.
 *
 * @return the hash code value for this set.
 * @see Object#hashCode()
 */
int32_t UnicodeSet::hashCode() ধ্রুব {
    uint32_t result = static_cast<uint32_t>(len);
    প্রতি (int32_t i = 0; i < len; ++i) {
        result *= 1000003u;
        result += list[i];
    }
    প্রত্যাগম static_cast<int32_t>(result);
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
int32_t UnicodeSet::size() ধ্রুব {
    int32_t n = 0;
    int32_t count = get_Range_গুনতি();
    প্রতি (int32_t i = 0; i < count; ++i) {
        n += getRangeEnd(i) - getRangeStart(i) + 1;
    }
    প্রত্যাগম n + stringsSize();
}

/**
 * Returns <tt>true</tt> if this set contains no elements.
 *
 * @return <tt>true</tt> if this set contains no elements.
 */
UBool UnicodeSet::isEmpty() ধ্রুব {
    প্রত্যাগম len == 1 && !hasStrings();
}

/**
 * Returns true if this set contains the given character.
 * @param c character to be checked for containment
 * @return true if the test condition is met
 */
UBool UnicodeSet::contains(UChar32 c) ধ্রুব {
    // Set i to the index of the start item greater than ch
    // We know we will terminate without length test!
    // LATER: for large sets, add binary search
    //int32_t i = -1;
    //for (;;) {
    //    if (c < list[++i]) break;
    //}
    যদি (bmpSet != nullptr) {
        প্রত্যাগম bmpSet->contains(c);
    }
    যদি (stringSpan != nullptr) {
        প্রত্যাগম stringSpan->contains(c);
    }
    যদি (c >= UNICODESET_উপরের) { // Don't need to check LOW bound
        প্রত্যাগম অসত্য;
    }
    int32_t i = findCodePoint(c);
    প্রত্যাগম i & 1; // return true if odd
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
int32_t UnicodeSet::findCodePoint(UChar32 c) ধ্রুব {
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
    যদি (c < list[0])
        প্রত্যাগম 0;
    // High runner test.  c is often after the last range, so an
    // initial check for this condition pays off.
    int32_t lo = 0;
    int32_t hi = len - 1;
    যদি (lo >= hi || c >= list[hi-1])
        প্রত্যাগম hi;
    // invariant: c >= list[lo]
    // invariant: c < list[hi]
    প্রতি (;;) {
        int32_t i = (lo + hi) >> 1;
        যদি (i == lo) {
            ভঙ্গ; // Found!
        } অন্যথা যদি (c < list[i]) {
            hi = i;
        } অন্যথা {
            lo = i;
        }
    }
    প্রত্যাগম hi;
}

/**
 * Returns true if this set contains every character
 * of the given range.
 * @param start first character, inclusive, of the range
 * @param end last character, inclusive, of the range
 * @return true if the test condition is met
 */
UBool UnicodeSet::contains(UChar32 start, UChar32 end) ধ্রুব {
    //int32_t i = -1;
    //for (;;) {
    //    if (start < list[++i]) break;
    //}
    int32_t i = findCodePoint(start);
    প্রত্যাগম ((i & 1) != 0 && end < list[i]);
}

/**
 * Returns <tt>true</tt> if this set contains the given
 * multicharacter string.
 * @param s string to be checked for containment
 * @return <tt>true</tt> if this set contains the specified string
 */
UBool UnicodeSet::contains(ধ্রুব UnicodeString& s) ধ্রুব {
    int32_t cp = getSingleCP(s);
    যদি (cp < 0) {
        প্রত্যাগম stringsContains(s);
    } অন্যথা {
        প্রত্যাগম contains(static_cast<UChar32>(cp));
    }
}

/**
 * Returns true if this set contains all the characters and strings
 * of the given set.
 * @param c set to be checked for containment
 * @return true if the test condition is met
 */
UBool UnicodeSet::containsAll(ধ্রুব UnicodeSet& c) ধ্রুব {
    // The specified set is a subset if all of its pairs are contained in
    // this set.  It's possible to code this more efficiently in terms of
    // direct manipulation of the inversion lists if the need arises.
    int32_t n = c.get_Range_গুনতি();
    প্রতি (পূর্ণাঙ্ক i=0; i<n; ++i) {
        যদি (!contains(c.getRangeStart(i), c.getRangeEnd(i))) {
            প্রত্যাগম অসত্য;
        }
    }
    প্রত্যাগম !c.hasStrings() || (strings_ != nullptr && strings_->containsAll(*c.strings_));
}

/**
 * Returns true if this set contains all the characters
 * of the given string.
 * @param s string containing characters to be checked for containment
 * @return true if the test condition is met
 */
UBool UnicodeSet::containsAll(ধ্রুব UnicodeString& s) ধ্রুব {
    প্রত্যাগম span(s.getBuffer(), s.length(), USET_SPAN_CONTAINED) == s.length();
}

/**
 * Returns true if this set contains none of the characters
 * of the given range.
 * @param start first character, inclusive, of the range
 * @param end last character, inclusive, of the range
 * @return true if the test condition is met
 */
UBool UnicodeSet::containsNone(UChar32 start, UChar32 end) ধ্রুব {
    //int32_t i = -1;
    //for (;;) {
    //    if (start < list[++i]) break;
    //}
    int32_t i = findCodePoint(start);
    প্রত্যাগম ((i & 1) == 0 && end < list[i]);
}

/**
 * Returns true if this set contains none of the characters and strings
 * of the given set.
 * @param c set to be checked for containment
 * @return true if the test condition is met
 */
UBool UnicodeSet::containsNone(ধ্রুব UnicodeSet& c) ধ্রুব {
    // The specified set is a subset if all of its pairs are contained in
    // this set.  It's possible to code this more efficiently in terms of
    // direct manipulation of the inversion lists if the need arises.
    int32_t n = c.get_Range_গুনতি();
    প্রতি (int32_t i=0; i<n; ++i) {
        যদি (!containsNone(c.getRangeStart(i), c.getRangeEnd(i))) {
            প্রত্যাগম অসত্য;
        }
    }
    প্রত্যাগম strings_ == nullptr || !c.hasStrings() || strings_->containsNone(*c.strings_);
}

/**
 * Returns true if this set contains none of the characters
 * of the given string.
 * @param s string containing characters to be checked for containment
 * @return true if the test condition is met
 */
UBool UnicodeSet::containsNone(ধ্রুব UnicodeString& s) ধ্রুব {
    প্রত্যাগম span(s.getBuffer(), s.length(), USET_SPAN_NOT_CONTAINED) == s.length();
}

/**
 * Returns <tt>true</tt> if this set contains any character whose low byte
 * is the given value.  This is used by <tt>RuleBasedTransliterator</tt> for
 * indexing.
 */
UBool UnicodeSet::matches_Index_মান(uint8_t v) ধ্রুব {
    /* The index value v, in the range [0,255], is contained in this set if
     * it is contained in any pair of this set.  Pairs either have the high
     * bytes equal, or unequal.  If the high bytes are equal, then we have
     * aaxx..aayy, where aa is the high byte.  Then v is contained if xx <=
     * v <= yy.  If the high bytes are unequal we have aaxx..bbyy, bb>aa.
     * Then v is contained if xx <= v || v <= yy.  (This is identical to the
     * time zone month containment logic.)
     */
    int32_t i;
    int32_t range_গুনতি=get_Range_গুনতি();
    প্রতি (i=0; i<range_গুনতি; ++i) {
        UChar32 নিচের = getRangeStart(i);
        UChar32 উপরের = getRangeEnd(i);
        যদি ((নিচের & ~0xFF) == (উপরের & ~0xFF)) {
            যদি ((নিচের & 0xFF) <= v && v <= (উপরের & 0xFF)) {
                প্রত্যাগম সত্য;
            }
        } অন্যথা যদি ((নিচের & 0xFF) <= v || v <= (উপরের & 0xFF)) {
            প্রত্যাগম সত্য;
        }
    }
    যদি (hasStrings()) {
        প্রতি (i=0; i<strings_->size(); ++i) {
            ধ্রুব UnicodeString& s = *static_cast<ধ্রুব UnicodeString*>(strings_->elementAt(i));
            যদি (s.isEmpty()) {
                অনুবর্ত;  // skip the empty string
            }
            UChar32 c = s.char32At(0);
            যদি ((c & 0xFF) == v) {
                প্রত্যাগম সত্য;
            }
        }
    }
    প্রত্যাগম অসত্য;
}

/**
 * Implementation of UnicodeMatcher::matches().  Always matches the
 * longest possible multichar string.
 */
UMatchDegree UnicodeSet::matches(ধ্রুব Replaceable& text,
                                 int32_t& offset,
                                 int32_t limit,
                                 UBool incremental) {
    যদি (offset == limit) {
        যদি (contains(U_ETHER)) {
            প্রত্যাগম incremental ? U_PARTIAL_MATCH : U_MATCH;
        } অন্যথা {
            প্রত্যাগম U_MISMATCH;
        }
    } অন্যথা {
        যদি (hasStrings()) { // try strings first

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
            int32_t উপরের_Water_Length = 0;

            প্রতি (i=0; i<strings_->size(); ++i) {
                ধ্রুব UnicodeString& trial = *static_cast<ধ্রুব UnicodeString*>(strings_->elementAt(i));
                যদি (trial.isEmpty()) {
                    অনুবর্ত;  // skip the empty string
                }

                char16_t c = trial.charAt(forward ? 0 : trial.length() - 1);

                // Strings are sorted, so we can optimize in the
                // forward direction.
                যদি (forward && c > firstChar) ভঙ্গ;
                যদি (c != firstChar) অনুবর্ত;

                int32_t matchLen = matchRest(text, offset, limit, trial);

                যদি (incremental) {
                    int32_t maxLen = forward ? limit-offset : offset-limit;
                    যদি (matchLen == maxLen) {
                        // We have successfully matched but only up to limit.
                        প্রত্যাগম U_PARTIAL_MATCH;
                    }
                }

                যদি (matchLen == trial.length()) {
                    // We have successfully matched the whole string.
                    যদি (matchLen > উপরের_Water_Length) {
                        উপরের_Water_Length = matchLen;
                    }
                    // In the forward direction we know strings
                    // are sorted so we can bail early.
                    যদি (forward && matchLen < উপরের_Water_Length) {
                        ভঙ্গ;
                    }
                    অনুবর্ত;
                }
            }

            // We've checked all strings without a partial match.
            // If we have full matches, return the longest one.
            যদি (উপরের_Water_Length != 0) {
                offset += forward ? উপরের_Water_Length : -উপরের_Water_Length;
                প্রত্যাগম U_MATCH;
            }
        }
        প্রত্যাগম UnicodeFilter::matches(text, offset, limit, incremental);
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
int32_t UnicodeSet::matchRest(ধ্রুব Replaceable& text,
                              int32_t start, int32_t limit,
                              ধ্রুব UnicodeString& s) {
    int32_t i;
    int32_t maxLen;
    int32_t slen = s.length();
    যদি (start < limit) {
        maxLen = limit - start;
        যদি (maxLen > slen) maxLen = slen;
        প্রতি (i = 1; i < maxLen; ++i) {
            যদি (text.charAt(start + i) != s.charAt(i)) প্রত্যাগম 0;
        }
    } অন্যথা {
        maxLen = start - limit;
        যদি (maxLen > slen) maxLen = slen;
        --slen; // <=> slen = s.length() - 1;
        প্রতি (i = 1; i < maxLen; ++i) {
            যদি (text.charAt(start - i) != s.charAt(slen - i)) প্রত্যাগম 0;
        }
    }
    প্রত্যাগম maxLen;
}

/**
 * Implement of UnicodeMatcher
 */
শূন্য UnicodeSet::addMatchSetTo(UnicodeSet& toUnionTo) ধ্রুব {
    toUnionTo.addAll(*এতত্);
}

/**
 * Returns the index of the given character within this set, where
 * the set is ordered by ascending code point.  If the character
 * is not in this set, return -1.  The inverse of this method is
 * <code>charAt()</code>.
 * @return an index from 0..size()-1, or -1
 */
int32_t UnicodeSet::indexOf(UChar32 c) ধ্রুব {
    যদি (c < MIN_মান || c > MAX_মান) {
        প্রত্যাগম -1;
    }
    int32_t i = 0;
    int32_t n = 0;
    প্রতি (;;) {
        UChar32 start = list[i++];
        যদি (c < start) {
            প্রত্যাগম -1;
        }
        UChar32 limit = list[i++];
        যদি (c < limit) {
            প্রত্যাগম n + c - start;
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
UChar32 UnicodeSet::charAt(int32_t index) ধ্রুব {
    যদি (index >= 0) {
        // len2 is the largest even integer <= len, that is, it is len
        // for even values and len-1 for odd values.  With odd values
        // the last entry is UNICODESET_HIGH.
        int32_t len2 = len & ~1;
        প্রতি (int32_t i=0; i < len2;) {
            UChar32 start = list[i++];
            int32_t count = list[i++] - start;
            যদি (index < count) {
                প্রত্যাগম static_cast<UChar32>(start + index);
            }
            index -= count;
        }
    }
    প্রত্যাগম static_cast<UChar32>(-1);
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
    প্রত্যাগম *এতত্;
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
    যদি (pinCodePoint(start) < pinCodePoint(end)) {
        UChar32 limit = end + 1;
        // Fast path for adding a new range after the last one.
        // Odd list length: [..., lastStart, lastLimit, HIGH]
        যদি ((len & 1) != 0) {
            // If the list is empty, set lastLimit low enough to not be adjacent to 0.
            UChar32 lastLimit = len == 1 ? -2 : list[len - 2];
            যদি (lastLimit <= start && !isFrozen() && !isBogus()) {
                যদি (lastLimit == start) {
                    // Extend the last range.
                    list[len - 2] = limit;
                    যদি (limit == UNICODESET_উপরের) {
                        --len;
                    }
                } অন্যথা {
                    list[len - 1] = start;
                    যদি (limit < UNICODESET_উপরের) {
                        যদি (ensureCapacity(len + 2)) {
                            list[len++] = limit;
                            list[len++] = UNICODESET_উপরের;
                        }
                    } অন্যথা {  // limit == UNICODESET_HIGH
                        যদি (ensureCapacity(len + 1)) {
                            list[len++] = UNICODESET_উপরের;
                        }
                    }
                }
                releasePattern();
                প্রত্যাগম *এতত্;
            }
        }
        // This is slow. Could be much faster using findCodePoint(start)
        // and modifying the list, dealing with adjacent & overlapping ranges.
        UChar32 range[3] = { start, limit, UNICODESET_উপরের };
        add(range, 2, 0);
    } অন্যথা যদি (start == end) {
        add(start);
    }
    প্রত্যাগম *এতত্;
}

// #define DEBUG_US_ADD

#ifdef DEBUG_US_ADD
#অন্তর্ভাব <stdio.h>
শূন্য dump(UChar32 c) {
    যদি (c <= 0xFF) {
        printf("%c", (বর্ণ)c);
    } অন্যথা {
        printf("U+%04X", c);
    }
}
শূন্য dump(ধ্রুব UChar32* list, int32_t len) {
    printf("[");
    প্রতি (int32_t i=0; i<len; ++i) {
        যদি (i != 0) printf(", ");
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
    যদি ((i & 1) != 0  || isFrozen() || isBogus()) প্রত্যাগম *এতত্;

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

    যদি (c == list[i]-1) {
        // c is before start of next range
        list[i] = c;
        // if we touched the HIGH mark, then add a new one
        যদি (c == (UNICODESET_উপরের - 1)) {
            যদি (!ensureCapacity(len+1)) {
                // ensureCapacity will mark the object as Bogus if OOM failure happens.
                প্রত্যাগম *এতত্;
            }
            list[len++] = UNICODESET_উপরের;
        }
        যদি (i > 0 && c == list[i-1]) {
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
            যাবত্ (src < srclimit) *(dst++) = *(src++);

            len -= 2;
        }
    }

    অন্যথা যদি (i > 0 && c == list[i-1]) {
        // c is after end of prior range
        list[i-1]++;
        // no need to check for collapse here
    }

    অন্যথা {
        // At this point we know the new char is not adjacent to
        // any existing ranges, and it is not 10FFFF.


        // [..., start_k-1, limit_k-1, start_k, limit_k, ..., HIGH]
        //                             ^
        //                             list[i]

        // [..., start_k-1, limit_k-1, c, c+1, start_k, limit_k, ..., HIGH]
        //                             ^
        //                             list[i]

        যদি (!ensureCapacity(len+2)) {
            // ensureCapacity will mark the object as Bogus if OOM failure happens.
            প্রত্যাগম *এতত্;
        }

        UChar32 *p = list + i;
        uprv_memmove(p + 2, p, (len - i) * পরিমাণ(*p));
        list[i] = c;
        list[i+1] = c+1;
        len += 2;
    }

#ifdef DEBUG_US_ADD
    dump(list, len);
    printf("\n");

    প্রতি (i=1; i<len; ++i) {
        যদি (list[i] <= list[i-1]) {
            // Corrupt array!
            printf("ERROR: list has been corrupted\n");
            exit(1);
        }
    }
#endif

    releasePattern();
    প্রত্যাগম *এতত্;
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
UnicodeSet& UnicodeSet::add(ধ্রুব UnicodeString& s) U_LIFETIME_BOUND {
    যদি (isFrozen() || isBogus()) প্রত্যাগম *এতত্;
    int32_t cp = getSingleCP(s);
    যদি (cp < 0) {
        যদি (!stringsContains(s)) {
            _add(s);
            releasePattern();
        }
    } অন্যথা {
        add(static_cast<UChar32>(cp));
    }
    প্রত্যাগম *এতত্;
}

/**
 * Adds the given string, in order, to 'strings_'.  The given string
 * must have been checked by the caller to not already be in 'strings_'.
 */
শূন্য UnicodeSet::_add(ধ্রুব UnicodeString& s) {
    যদি (isFrozen() || isBogus()) {
        প্রত্যাগম;
    }
    UErrorCode ec = U_ZERO_ERROR;
    যদি (strings_ == nullptr && !allocateStrings(ec)) {
        setToBogus();
        প্রত্যাগম;
    }
    LocalPointer<UnicodeString> t(নূতন UnicodeString(s));
    যদি (t.isNull()) { // Check for memory allocation error.
        setToBogus();
        প্রত্যাগম;
    }
    strings_->sorted_ঢোকাও(t.orphan(), compareUnicodeString, ec);
    যদি (U_FAILURE(ec)) {
        setToBogus();
    }
}

/**
 * @return a code point IF the string consists of a single one.
 * otherwise returns -1.
 * @param string to test
 */
int32_t UnicodeSet::getSingleCP(ধ্রুব UnicodeString& s) {
    int32_t sLength = s.length();
    যদি (sLength == 1) প্রত্যাগম s.charAt(0);
    যদি (sLength == 2) {
        UChar32 cp = s.char32At(0);
        যদি (cp > 0xFFFF) { // is surrogate pair
            প্রত্যাগম cp;
        }
    }
    প্রত্যাগম -1;
}

/**
 * Adds each of the characters in this string to the set. Thus "ch" => {"c", "h"}
 * If this set already any particular character, it has no effect on that character.
 * @param the source string
 * @return the modified set, for chaining
 */
UnicodeSet& UnicodeSet::addAll(ধ্রুব UnicodeString& s) U_LIFETIME_BOUND {
    UChar32 cp;
    প্রতি (int32_t i = 0; i < s.length(); i += U16_LENGTH(cp)) {
        cp = s.char32At(i);
        add(cp);
    }
    প্রত্যাগম *এতত্;
}

/**
 * Retains EACH of the characters in this string. Note: "ch" == {"c", "h"}
 * If this set already any particular character, it has no effect on that character.
 * @param the source string
 * @return the modified set, for chaining
 */
UnicodeSet& UnicodeSet::retainAll(ধ্রুব UnicodeString& s) U_LIFETIME_BOUND {
    UnicodeSet set;
    set.addAll(s);
    retainAll(set);
    প্রত্যাগম *এতত্;
}

/**
 * Complement EACH of the characters in this string. Note: "ch" == {"c", "h"}
 * If this set already any particular character, it has no effect on that character.
 * @param the source string
 * @return the modified set, for chaining
 */
UnicodeSet& UnicodeSet::complementAll(ধ্রুব UnicodeString& s) U_LIFETIME_BOUND {
    UnicodeSet set;
    set.addAll(s);
    complementAll(set);
    প্রত্যাগম *এতত্;
}

/**
 * Remove EACH of the characters in this string. Note: "ch" == {"c", "h"}
 * If this set already any particular character, it has no effect on that character.
 * @param the source string
 * @return the modified set, for chaining
 */
UnicodeSet& UnicodeSet::removeAll(ধ্রুব UnicodeString& s) U_LIFETIME_BOUND {
    UnicodeSet set;
    set.addAll(s);
    removeAll(set);
    প্রত্যাগম *এতত্;
}

UnicodeSet& UnicodeSet::removeAllStrings() U_LIFETIME_BOUND {
    যদি (!isFrozen() && hasStrings()) {
        strings_->removeAllElements();
        releasePattern();
    }
    প্রত্যাগম *এতত্;
}


/**
 * Makes a set from a multicharacter string. Thus "ch" => {"ch"}
 * <br><b>Warning: you cannot add an empty string ("") to a UnicodeSet.</b>
 * @param the source string
 * @return a newly created set containing the given string
 */
UnicodeSet* U_EXPORT2 UnicodeSet::createFrom(ধ্রুব UnicodeString& s) {
    UnicodeSet *set = নূতন UnicodeSet();
    যদি (set != nullptr) { // Check for memory allocation error.
        set->add(s);
    }
    প্রত্যাগম set;
}


/**
 * Makes a set from each of the characters in the string. Thus "ch" => {"c", "h"}
 * @param the source string
 * @return a newly created set containing the given characters
 */
UnicodeSet* U_EXPORT2 UnicodeSet::createFromAll(ধ্রুব UnicodeString& s) {
    UnicodeSet *set = নূতন UnicodeSet();
    যদি (set != nullptr) { // Check for memory allocation error.
        set->addAll(s);
    }
    প্রত্যাগম set;
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
    যদি (pinCodePoint(start) <= pinCodePoint(end)) {
        UChar32 range[3] = { start, end+1, UNICODESET_উপরের };
        retain(range, 2, 0);
    } অন্যথা {
        clear();
    }
    প্রত্যাগম *এতত্;
}

UnicodeSet& UnicodeSet::retain(UChar32 c) U_LIFETIME_BOUND {
    প্রত্যাগম retain(c, c);
}

UnicodeSet& UnicodeSet::retain(ধ্রুব UnicodeString &s) U_LIFETIME_BOUND {
    যদি (isFrozen() || isBogus()) { প্রত্যাগম *এতত্; }
    UChar32 cp = getSingleCP(s);
    যদি (cp < 0) {
        তার্কিক isIn = stringsContains(s);
        // Check for getRangeCount() first to avoid somewhat-expensive size()
        // when there are single code points.
        যদি (isIn && get_Range_গুনতি() == 0 && size() == 1) {
            প্রত্যাগম *এতত্;
        }
        clear();
        যদি (isIn) {
            _add(s);
        }
    } অন্যথা {
        retain(cp, cp);
    }
    প্রত্যাগম *এতত্;
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
    যদি (pinCodePoint(start) <= pinCodePoint(end)) {
        UChar32 range[3] = { start, end+1, UNICODESET_উপরের };
        retain(range, 2, 2);
    }
    প্রত্যাগম *এতত্;
}

/**
 * Removes the specified character from this set if it is present.
 * The set will not contain the specified range once the call
 * returns.
 */
UnicodeSet& UnicodeSet::remove(UChar32 c) U_LIFETIME_BOUND {
    প্রত্যাগম remove(c, c);
}

/**
 * Removes the specified string from this set if it is present.
 * The set will not contain the specified character once the call
 * returns.
 * @param the source string
 * @return the modified set, for chaining
 */
UnicodeSet& UnicodeSet::remove(ধ্রুব UnicodeString& s) U_LIFETIME_BOUND {
    যদি (isFrozen() || isBogus()) প্রত্যাগম *এতত্;
    int32_t cp = getSingleCP(s);
    যদি (cp < 0) {
        যদি (strings_ != nullptr && strings_->removeElement((শূন্য*) &s)) {
            releasePattern();
        }
    } অন্যথা {
        remove(static_cast<UChar32>(cp), static_cast<UChar32>(cp));
    }
    প্রত্যাগম *এতত্;
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
    যদি (isFrozen() || isBogus()) {
        প্রত্যাগম *এতত্;
    }
    যদি (pinCodePoint(start) <= pinCodePoint(end)) {
        UChar32 range[3] = { start, end+1, UNICODESET_উপরের };
        exclusiveOr(range, 2, 0);
    }
    releasePattern();
    প্রত্যাগম *এতত্;
}

UnicodeSet& UnicodeSet::complement(UChar32 c) U_LIFETIME_BOUND {
    প্রত্যাগম complement(c, c);
}

/**
 * This is equivalent to
 * <code>complement(MIN_VALUE, MAX_VALUE)</code>.
 */
UnicodeSet& UnicodeSet::complement() U_LIFETIME_BOUND {
    যদি (isFrozen() || isBogus()) {
        প্রত্যাগম *এতত্;
    }
    যদি (list[0] == UNICODESET_নিচের) {
        uprv_memmove(list, list + 1, (size_t)(len-1)*পরিমাণ(UChar32));
        --len;
    } অন্যথা {
        যদি (!ensureCapacity(len+1)) {
            প্রত্যাগম *এতত্;
        }
        uprv_memmove(list + 1, list, (size_t)len*পরিমাণ(UChar32));
        list[0] = UNICODESET_নিচের;
        ++len;
    }
    releasePattern();
    প্রত্যাগম *এতত্;
}

/**
 * Complement the specified string in this set.
 * The set will not contain the specified string once the call
 * returns.
 *
 * @param s the string to complement
 * @return this object, for chaining
 */
UnicodeSet& UnicodeSet::complement(ধ্রুব UnicodeString& s) U_LIFETIME_BOUND {
    যদি (isFrozen() || isBogus()) প্রত্যাগম *এতত্;
    int32_t cp = getSingleCP(s);
    যদি (cp < 0) {
        যদি (stringsContains(s)) {
            strings_->removeElement((শূন্য*) &s);
        } অন্যথা {
            _add(s);
        }
        releasePattern();
    } অন্যথা {
        complement(static_cast<UChar32>(cp), static_cast<UChar32>(cp));
    }
    প্রত্যাগম *এতত্;
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
UnicodeSet& UnicodeSet::addAll(ধ্রুব UnicodeSet& c) U_LIFETIME_BOUND {
    যদি ( c.len>0 && c.list!=nullptr ) {
        add(c.list, c.len, 0);
    }

    // Add strings in order
    যদি ( c.strings_!=nullptr ) {
        প্রতি (int32_t i=0; i<c.strings_->size(); ++i) {
            ধ্রুব UnicodeString* s = static_cast<ধ্রুব UnicodeString*>(c.strings_->elementAt(i));
            যদি (!stringsContains(*s)) {
                _add(*s);
            }
        }
    }
    প্রত্যাগম *এতত্;
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
UnicodeSet& UnicodeSet::retainAll(ধ্রুব UnicodeSet& c) U_LIFETIME_BOUND {
    যদি (isFrozen() || isBogus()) {
        প্রত্যাগম *এতত্;
    }
    retain(c.list, c.len, 0);
    যদি (hasStrings()) {
        যদি (!c.hasStrings()) {
            strings_->removeAllElements();
        } অন্যথা {
            strings_->retainAll(*c.strings_);
        }
    }
    প্রত্যাগম *এতত্;
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
UnicodeSet& UnicodeSet::removeAll(ধ্রুব UnicodeSet& c) U_LIFETIME_BOUND {
    যদি (isFrozen() || isBogus()) {
        প্রত্যাগম *এতত্;
    }
    retain(c.list, c.len, 2);
    যদি (hasStrings() && c.hasStrings()) {
        strings_->removeAll(*c.strings_);
    }
    প্রত্যাগম *এতত্;
}

/**
 * Complements in this set all elements contained in the specified
 * set.  Any character in the other set will be removed if it is
 * in this set, or will be added if it is not in this set.
 *
 * @param c set that defines which elements will be xor'ed from
 *          this set.
 */
UnicodeSet& UnicodeSet::complementAll(ধ্রুব UnicodeSet& c) U_LIFETIME_BOUND {
    যদি (isFrozen() || isBogus()) {
        প্রত্যাগম *এতত্;
    }
    exclusiveOr(c.list, c.len, 0);

    যদি (c.strings_ != nullptr) {
        প্রতি (int32_t i=0; i<c.strings_->size(); ++i) {
            শূন্য* e = c.strings_->elementAt(i);
            যদি (strings_ == nullptr || !strings_->removeElement(e)) {
                _add(*static_cast<ধ্রুব UnicodeString*>(e));
            }
        }
    }
    প্রত্যাগম *এতত্;
}

/**
 * Removes all of the elements from this set.  This set will be
 * empty after this call returns.
 */
UnicodeSet& UnicodeSet::clear() U_LIFETIME_BOUND {
    যদি (isFrozen()) {
        প্রত্যাগম *এতত্;
    }
    list[0] = UNICODESET_উপরের;
    len = 1;
    releasePattern();
    যদি (strings_ != nullptr) {
        strings_->removeAllElements();
    }
    // Remove bogus
    fFlags = 0;
    প্রত্যাগম *এতত্;
}

/**
 * Iteration method that returns the number of ranges contained in
 * this set.
 * @see #getRangeStart
 * @see #getRangeEnd
 */
int32_t UnicodeSet::get_Range_গুনতি() ধ্রুব {
    প্রত্যাগম len/2;
}

/**
 * Iteration method that returns the first character in the
 * specified range of this set.
 * @see #getRangeCount
 * @see #getRangeEnd
 */
UChar32 UnicodeSet::getRangeStart(int32_t index) ধ্রুব {
    প্রত্যাগম list[index*2];
}

/**
 * Iteration method that returns the last character in the
 * specified range of this set.
 * @see #getRangeStart
 * @see #getRangeEnd
 */
UChar32 UnicodeSet::getRangeEnd(int32_t index) ধ্রুব {
    প্রত্যাগম list[index*2 + 1] - 1;
}

ধ্রুব UnicodeString* UnicodeSet::getString(int32_t index) ধ্রুব U_LIFETIME_BOUND {
    প্রত্যাগম static_cast<ধ্রুব UnicodeString*>(strings_->elementAt(index));
}

/**
 * Reallocate this objects internal structures to take up the least
 * possible space, without changing this object's value.
 */
UnicodeSet& UnicodeSet::compact() U_LIFETIME_BOUND {
    যদি (isFrozen() || isBogus()) {
        প্রত্যাগম *এতত্;
    }
    // Delete buffer first to defragment memory less.
    যদি (buffer != স্তূপ_List) {
        uprv_free(buffer);
        buffer = nullptr;
        bufferCapacity = 0;
    }
    যদি (list == স্তূপ_List) {
        // pass
    } অন্যথা যদি (len <= INITIAL_CAPACITY) {
        uprv_memcpy(স্তূপ_List, list, len * পরিমাণ(UChar32));
        uprv_free(list);
        list = স্তূপ_List;
        capacity = INITIAL_CAPACITY;
    } অন্যথা যদি ((len + 7) < capacity) {
        // If we have more than a little unused capacity, shrink it to len.
        UChar32* temp = static_cast<UChar32*>(uprv_realloc(list, পরিমাণ(UChar32) * len));
        যদি (temp) {
            list = temp;
            capacity = len;
        }
        // else what the heck happened?! We allocated less memory!
        // Oh well. We'll keep our original array.
    }
    যদি (strings_ != nullptr && strings_->isEmpty()) {
        delete strings_;
        strings_ = nullptr;
    }
    প্রত্যাগম *এতত্;
}

#ifdef DEBUG_SERIALIZE
#অন্তর্ভাব <stdio.h>
#endif

/**
 * Deserialize constructor.
 */
UnicodeSet::UnicodeSet(ধ্রুব uint16_t data[], int32_t dataLen, ESerialization serialization,
                       UErrorCode &ec) {

  যদি(U_FAILURE(ec)) {
    setToBogus();
    প্রত্যাগম;
  }

  যদি( (serialization != kSerialized)
      || (data==nullptr)
      || (dataLen < 1)) {
    ec = U_ILLEGAL_ARGUMENT_ERROR;
    setToBogus();
    প্রত্যাগম;
  }

  // bmp?
  int32_t headerSize = ((data[0]&0x8000)) ?2:1;
  int32_t bmpLength = (headerSize==1)?data[0]:data[1];

  int32_t newLength = (((data[0]&0x7FFF)-bmpLength)/2)+bmpLength;
#ifdef DEBUG_SERIALIZE
  printf("dataLen %d headerSize %d bmpLen %d len %d. data[0]=%X/%X/%X/%X\n", dataLen,headerSize,bmpLength,newLength, data[0],data[1],data[2],data[3]);
#endif
  যদি(!ensureCapacity(newLength + 1)) {  // +1 for HIGH
    প্রত্যাগম;
  }
  // copy bmp
  int32_t i;
  প্রতি(i = 0; i< bmpLength;i++) {
    list[i] = data[i+headerSize];
#ifdef DEBUG_SERIALIZE
    printf("<<16@%d[%d] %X\n", i+headerSize, i, list[i]);
#endif
  }
  // copy smp
  প্রতি(i=bmpLength;i<newLength;i++) {
    list[i] = (static_cast<UChar32>(data[headerSize + bmpLength + (i - bmpLength) * 2 + 0]) << 16) +
               static_cast<UChar32>(data[headerSize + bmpLength + (i - bmpLength) * 2 + 1]);
#ifdef DEBUG_SERIALIZE
    printf("<<32@%d+[%d] %lX\n", headerSize+bmpLength+i, i, list[i]);
#endif
  }
  U_ASSERT(i == newLength);
  যদি (i == 0 || list[i - 1] != UNICODESET_উপরের) {
    list[i++] = UNICODESET_উপরের;
  }
  len = i;
}


int32_t UnicodeSet::serialize(uint16_t *dest, int32_t destCapacity, UErrorCode& ec) ধ্রুব {
    int32_t bmpLength, length, destLength;

    যদি (U_FAILURE(ec)) {
        প্রত্যাগম 0;
    }

    যদি (destCapacity<0 || (destCapacity>0 && dest==nullptr)) {
        ec=U_ILLEGAL_ARGUMENT_ERROR;
        প্রত্যাগম 0;
    }

    /* count necessary 16-bit units */
    length=এতত্->len-1; // Subtract 1 to ignore final UNICODESET_HIGH
    // assert(length>=0);
    যদি (length==0) {
        /* empty set */
        যদি (destCapacity>0) {
            *dest=0;
        } অন্যথা {
            ec=U_BUFFER_OVERFLOW_ERROR;
        }
        প্রত্যাগম 1;
    }
    /* now length>0 */

    যদি (এতত্->list[length-1]<=0xffff) {
        /* all BMP */
        bmpLength=length;
    } অন্যথা যদি (এতত্->list[0]>=0x10000) {
        /* all supplementary */
        bmpLength=0;
        length*=2;
    } অন্যথা {
        /* some BMP, some supplementary */
        প্রতি (bmpLength=0; bmpLength<length && এতত্->list[bmpLength]<=0xffff; ++bmpLength) {}
        length=bmpLength+2*(length-bmpLength);
    }
#ifdef DEBUG_SERIALIZE
    printf(">> bmpLength%d length%d len%d\n", bmpLength, length, len);
#endif
    /* length: number of 16-bit array units */
    যদি (length>0x7fff) {
        /* there are only 15 bits for the length in the first serialized word */
        ec=U_INDEX_OUTOFBOUNDS_ERROR;
        প্রত্যাগম 0;
    }

    /*
     * total serialized length:
     * number of 16-bit array units (length) +
     * 1 length unit (always) +
     * 1 bmpLength unit (if there are supplementary values)
     */
    destLength=length+((length>bmpLength)?2:1);
    যদি (destLength<=destCapacity) {
        ধ্রুব UChar32 *p;
        int32_t i;

#ifdef DEBUG_SERIALIZE
        printf("writeHdr\n");
#endif
        *dest = static_cast<uint16_t>(length);
        যদি (length>bmpLength) {
            *dest|=0x8000;
            *++dest = static_cast<uint16_t>(bmpLength);
        }
        ++dest;

        /* write the BMP part of the array */
        p=এতত্->list;
        প্রতি (i=0; i<bmpLength; ++i) {
#ifdef DEBUG_SERIALIZE
          printf("writebmp: %x\n", (পূর্ণাঙ্ক)*p);
#endif
            *dest++ = static_cast<uint16_t>(*p++);
        }

        /* write the supplementary part of the array */
        প্রতি (; i<length; i+=2) {
#ifdef DEBUG_SERIALIZE
          printf("write32: %x\n", (পূর্ণাঙ্ক)*p);
#endif
            *dest++ = static_cast<uint16_t>(*p >> 16);
            *dest++ = static_cast<uint16_t>(*p++);
        }
    } অন্যথা {
        ec=U_BUFFER_OVERFLOW_ERROR;
    }
    প্রত্যাগম destLength;
}

//----------------------------------------------------------------
// Implementation: Utility methods
//----------------------------------------------------------------

/**
 * Allocate our strings vector and return true if successful.
 */
UBool UnicodeSet::allocateStrings(UErrorCode &status) {
    যদি (U_FAILURE(status)) {
        প্রত্যাগম অসত্য;
    }
    strings_ = নূতন UVector(uprv_deleteUObject,
                          uhash_compareUnicodeString, 1, status);
    যদি (strings_ == nullptr) { // Check for memory allocation error.
        status = U_MEMORY_ALLOCATION_ERROR;
        প্রত্যাগম অসত্য;
    }
    যদি (U_FAILURE(status)) {
        delete strings_;
        strings_ = nullptr;
        প্রত্যাগম অসত্য;
    } 
    প্রত্যাগম সত্য;
}

int32_t UnicodeSet::nextCapacity(int32_t minCapacity) {
    // Grow exponentially to reduce the frequency of allocations.
    যদি (minCapacity < INITIAL_CAPACITY) {
        প্রত্যাগম minCapacity + INITIAL_CAPACITY;
    } অন্যথা যদি (minCapacity <= 2500) {
        প্রত্যাগম 5 * minCapacity;
    } অন্যথা {
        int32_t newCapacity = 2 * minCapacity;
        যদি (newCapacity > MAX_LENGTH) {
            newCapacity = MAX_LENGTH;
        }
        প্রত্যাগম newCapacity;
    }
}

তার্কিক UnicodeSet::ensureCapacity(int32_t newLen) {
    যদি (newLen > MAX_LENGTH) {
        newLen = MAX_LENGTH;
    }
    যদি (newLen <= capacity) {
        প্রত্যাগম সত্য;
    }
    int32_t newCapacity = nextCapacity(newLen);
    UChar32* temp = static_cast<UChar32*>(uprv_malloc(newCapacity * পরিমাণ(UChar32)));
    যদি (temp == nullptr) {
        setToBogus(); // set the object to bogus state if an OOM failure occurred.
        প্রত্যাগম অসত্য;
    }
    // Copy only the actual contents.
    uprv_memcpy(temp, list, len * পরিমাণ(UChar32));
    যদি (list != স্তূপ_List) {
        uprv_free(list);
    }
    list = temp;
    capacity = newCapacity;
    প্রত্যাগম সত্য;
}

তার্কিক UnicodeSet::ensureBufferCapacity(int32_t newLen) {
    যদি (newLen > MAX_LENGTH) {
        newLen = MAX_LENGTH;
    }
    যদি (newLen <= bufferCapacity) {
        প্রত্যাগম সত্য;
    }
    int32_t newCapacity = nextCapacity(newLen);
    UChar32* temp = static_cast<UChar32*>(uprv_malloc(newCapacity * পরিমাণ(UChar32)));
    যদি (temp == nullptr) {
        setToBogus();
        প্রত্যাগম অসত্য;
    }
    // The buffer has no contents to be copied.
    // It is always filled from scratch after this call.
    যদি (buffer != স্তূপ_List) {
        uprv_free(buffer);
    }
    buffer = temp;
    bufferCapacity = newCapacity;
    প্রত্যাগম সত্য;
}

/**
 * Swap list and buffer.
 */
শূন্য UnicodeSet::swapBuffers() {
    // swap list and buffer
    UChar32* temp = list;
    list = buffer;
    buffer = temp;

    int32_t c = capacity;
    capacity = bufferCapacity;
    bufferCapacity = c;
}

শূন্য UnicodeSet::setToBogus() {
    clear(); // Remove everything in the set.
    fFlags = kIsBogus;
}

//----------------------------------------------------------------
// Implementation: Fundamental operators
//----------------------------------------------------------------

স্থির অন্তঃস্থ UChar32 max(UChar32 a, UChar32 b) {
    প্রত্যাগম (a > b) ? a : b;
}

// polarity = 0, 3 is normal: x xor y
// polarity = 1, 2: x xor ~y == x === y

শূন্য UnicodeSet::exclusiveOr(ধ্রুব UChar32* other, int32_t otherLen, int8_t polarity) {
    যদি (isFrozen() || isBogus()) {
        প্রত্যাগম;
    }
    যদি (!ensureBufferCapacity(len + otherLen)) {
        প্রত্যাগম;
    }

    int32_t i = 0, j = 0, k = 0;
    UChar32 a = list[i++];
    UChar32 b;
    যদি (polarity == 1 || polarity == 2) {
        b = UNICODESET_নিচের;
        যদি (other[j] == UNICODESET_নিচের) { // skip base if already LOW
            ++j;
            b = other[j];
        }
    } অন্যথা {
        b = other[j++];
    }
    // simplest of all the routines
    // sort the values, discarding identicals!
    প্রতি (;;) {
        যদি (a < b) {
            buffer[k++] = a;
            a = list[i++];
        } অন্যথা যদি (b < a) {
            buffer[k++] = b;
            b = other[j++];
        } অন্যথা যদি (a != UNICODESET_উপরের) { // at this point, a == b
            // discard both values!
            a = list[i++];
            b = other[j++];
        } অন্যথা { // DONE!
            buffer[k++] = UNICODESET_উপরের;
            len = k;
            ভঙ্গ;
        }
    }
    swapBuffers();
    releasePattern();
}

// polarity = 0 is normal: x union y
// polarity = 2: x union ~y
// polarity = 1: ~x union y
// polarity = 3: ~x union ~y

শূন্য UnicodeSet::add(ধ্রুব UChar32* other, int32_t otherLen, int8_t polarity) {
    যদি (isFrozen() || isBogus() || other==nullptr) {
        প্রত্যাগম;
    }
    যদি (!ensureBufferCapacity(len + otherLen)) {
        প্রত্যাগম;
    }

    int32_t i = 0, j = 0, k = 0;
    UChar32 a = list[i++];
    UChar32 b = other[j++];
    // change from xor is that we have to check overlapping pairs
    // polarity bit 1 means a is second, bit 2 means b is.
    প্রতি (;;) {
        বিকল্প (polarity) {
          পক্ষ 0: // both first; take lower if unequal
            যদি (a < b) { // take a
                // Back up over overlapping ranges in buffer[]
                যদি (k > 0 && a <= buffer[k-1]) {
                    // Pick latter end value in buffer[] vs. list[]
                    a = max(list[i], buffer[--k]);
                } অন্যথা {
                    // No overlap
                    buffer[k++] = a;
                    a = list[i];
                }
                i++; // Common if/else code factored out
                polarity ^= 1;
            } অন্যথা যদি (b < a) { // take b
                যদি (k > 0 && b <= buffer[k-1]) {
                    b = max(other[j], buffer[--k]);
                } অন্যথা {
                    buffer[k++] = b;
                    b = other[j];
                }
                j++;
                polarity ^= 2;
            } অন্যথা { // a == b, take a, drop b
                যদি (a == UNICODESET_উপরের) গচ্ছ loop_end;
                // This is symmetrical; it doesn't matter if
                // we backtrack with a or b. - liu
                যদি (k > 0 && a <= buffer[k-1]) {
                    a = max(list[i], buffer[--k]);
                } অন্যথা {
                    // No overlap
                    buffer[k++] = a;
                    a = list[i];
                }
                i++;
                polarity ^= 1;
                b = other[j++];
                polarity ^= 2;
            }
            ভঙ্গ;
          পক্ষ 3: // both second; take higher if unequal, and drop other
            যদি (b <= a) { // take a
                যদি (a == UNICODESET_উপরের) গচ্ছ loop_end;
                buffer[k++] = a;
            } অন্যথা { // take b
                যদি (b == UNICODESET_উপরের) গচ্ছ loop_end;
                buffer[k++] = b;
            }
            a = list[i++];
            polarity ^= 1;   // factored common code
            b = other[j++];
            polarity ^= 2;
            ভঙ্গ;
          পক্ষ 1: // a second, b first; if b < a, overlap
            যদি (a < b) { // no overlap, take a
                buffer[k++] = a; a = list[i++]; polarity ^= 1;
            } অন্যথা যদি (b < a) { // OVERLAP, drop b
                b = other[j++];
                polarity ^= 2;
            } অন্যথা { // a == b, drop both!
                যদি (a == UNICODESET_উপরের) গচ্ছ loop_end;
                a = list[i++];
                polarity ^= 1;
                b = other[j++];
                polarity ^= 2;
            }
            ভঙ্গ;
          পক্ষ 2: // a first, b second; if a < b, overlap
            যদি (b < a) { // no overlap, take b
                buffer[k++] = b;
                b = other[j++];
                polarity ^= 2;
            } অন্যথা  যদি (a < b) { // OVERLAP, drop a
                a = list[i++];
                polarity ^= 1;
            } অন্যথা { // a == b, drop both!
                যদি (a == UNICODESET_উপরের) গচ্ছ loop_end;
                a = list[i++];
                polarity ^= 1;
                b = other[j++];
                polarity ^= 2;
            }
            ভঙ্গ;
        }
    }
 loop_end:
    buffer[k++] = UNICODESET_উপরের;    // terminate
    len = k;
    swapBuffers();
    releasePattern();
}

// polarity = 0 is normal: x intersect y
// polarity = 2: x intersect ~y == set-minus
// polarity = 1: ~x intersect y
// polarity = 3: ~x intersect ~y

শূন্য UnicodeSet::retain(ধ্রুব UChar32* other, int32_t otherLen, int8_t polarity) {
    যদি (isFrozen() || isBogus()) {
        প্রত্যাগম;
    }
    যদি (!ensureBufferCapacity(len + otherLen)) {
        প্রত্যাগম;
    }

    int32_t i = 0, j = 0, k = 0;
    UChar32 a = list[i++];
    UChar32 b = other[j++];
    // change from xor is that we have to check overlapping pairs
    // polarity bit 1 means a is second, bit 2 means b is.
    প্রতি (;;) {
        বিকল্প (polarity) {
          পক্ষ 0: // both first; drop the smaller
            যদি (a < b) { // drop a
                a = list[i++];
                polarity ^= 1;
            } অন্যথা যদি (b < a) { // drop b
                b = other[j++];
                polarity ^= 2;
            } অন্যথা { // a == b, take one, drop other
                যদি (a == UNICODESET_উপরের) গচ্ছ loop_end;
                buffer[k++] = a;
                a = list[i++];
                polarity ^= 1;
                b = other[j++];
                polarity ^= 2;
            }
            ভঙ্গ;
          পক্ষ 3: // both second; take lower if unequal
            যদি (a < b) { // take a
                buffer[k++] = a;
                a = list[i++];
                polarity ^= 1;
            } অন্যথা যদি (b < a) { // take b
                buffer[k++] = b;
                b = other[j++];
                polarity ^= 2;
            } অন্যথা { // a == b, take one, drop other
                যদি (a == UNICODESET_উপরের) গচ্ছ loop_end;
                buffer[k++] = a;
                a = list[i++];
                polarity ^= 1;
                b = other[j++];
                polarity ^= 2;
            }
            ভঙ্গ;
          পক্ষ 1: // a second, b first;
            যদি (a < b) { // NO OVERLAP, drop a
                a = list[i++];
                polarity ^= 1;
            } অন্যথা যদি (b < a) { // OVERLAP, take b
                buffer[k++] = b;
                b = other[j++];
                polarity ^= 2;
            } অন্যথা { // a == b, drop both!
                যদি (a == UNICODESET_উপরের) গচ্ছ loop_end;
                a = list[i++];
                polarity ^= 1;
                b = other[j++];
                polarity ^= 2;
            }
            ভঙ্গ;
          পক্ষ 2: // a first, b second; if a < b, overlap
            যদি (b < a) { // no overlap, drop b
                b = other[j++];
                polarity ^= 2;
            } অন্যথা  যদি (a < b) { // OVERLAP, take a
                buffer[k++] = a;
                a = list[i++];
                polarity ^= 1;
            } অন্যথা { // a == b, drop both!
                যদি (a == UNICODESET_উপরের) গচ্ছ loop_end;
                a = list[i++];
                polarity ^= 1;
                b = other[j++];
                polarity ^= 2;
            }
            ভঙ্গ;
        }
    }
 loop_end:
    buffer[k++] = UNICODESET_উপরের;    // terminate
    len = k;
    swapBuffers();
    releasePattern();
}

/**
 * Append the <code>toPattern()</code> representation of a
 * string to the given <code>StringBuffer</code>.
 */
শূন্য UnicodeSet::_appendToPat(UnicodeString& buf, ধ্রুব UnicodeString& s, UBool escapeUnprintable) {
    UChar32 cp;
    প্রতি (int32_t i = 0; i < s.length(); i += U16_LENGTH(cp)) {
        _appendToPat(buf, cp = s.char32At(i), escapeUnprintable);
    }
}

/**
 * Append the <code>toPattern()</code> representation of a
 * character to the given <code>StringBuffer</code>.
 */
শূন্য UnicodeSet::_appendToPat(UnicodeString& buf, UChar32 c, UBool escapeUnprintable) {
    যদি (escapeUnprintable ? ICU_Utility::isUnprintable(c) : ICU_Utility::shouldAlwaysBeEscaped(c)) {
        // Use hex escape notation (\uxxxx or \Uxxxxxxxx) for anything
        // unprintable
        ICU_Utility::escape(buf, c);
        প্রত্যাগম;
    }
    // Okay to let ':' pass through
    বিকল্প (c) {
    পক্ষ u'[':
    পক্ষ u']':
    পক্ষ u'-':
    পক্ষ u'^':
    পক্ষ u'&':
    পক্ষ u'\\':
    পক্ষ u'{':
    পক্ষ u'}':
    পক্ষ u':':
    পক্ষ প্রতীক_Table::প্রতীক_REF:
        buf.append(u'\\');
        ভঙ্গ;
    স্বভাব:
        // Escape whitespace
        যদি (PatternProps::isWhiteSpace(c)) {
            buf.append(u'\\');
        }
        ভঙ্গ;
    }
    buf.append(c);
}

শূন্য UnicodeSet::_appendToPat(UnicodeString &result, UChar32 start, UChar32 end,
                              UBool escapeUnprintable) {
    _appendToPat(result, start, escapeUnprintable);
    যদি (start != end) {
        যদি ((start+1) != end ||
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
                                      UBool escapeUnprintable) ধ্রুব
{
    যদি (pat != nullptr) {
        int32_t i;
        int32_t backslash_গুনতি = 0;
        প্রতি (i=0; i<patLen; ) {
            UChar32 c;
            U16_NEXT(pat, i, patLen, c);
            যদি (escapeUnprintable ?
                    ICU_Utility::isUnprintable(c) : ICU_Utility::shouldAlwaysBeEscaped(c)) {
                // If the unprintable character is preceded by an odd
                // number of backslashes, then it has been escaped.
                // Before unescaping it, we delete the final
                // backslash.
                যদি ((backslash_গুনতি % 2) == 1) {
                    result.truncate(result.length() - 1);
                }
                ICU_Utility::escape(result, c);
                backslash_গুনতি = 0;
            } অন্যথা {
                result.append(c);
                যদি (c == u'\\') {
                    ++backslash_গুনতি;
                } অন্যথা {
                    backslash_গুনতি = 0;
                }
            }
        }
        প্রত্যাগম result;
    }

    প্রত্যাগম _generatePattern(result, escapeUnprintable);
}

/**
 * Returns a string representation of this set.  If the result of
 * calling this function is passed to a UnicodeSet constructor, it
 * will produce another set that is equal to this one.
 */
UnicodeString& UnicodeSet::toPattern(UnicodeString& result U_LIFETIME_BOUND,
                                     UBool escapeUnprintable) ধ্রুব
{
    result.truncate(0);
    প্রত্যাগম _toPattern(result, escapeUnprintable);
}

/**
 * Generate and append a string representation of this set to result.
 * This does not use this.pat, the cleaned up copy of the string
 * passed to applyPattern().
 */
UnicodeString& UnicodeSet::_generatePattern(UnicodeString& result U_LIFETIME_BOUND,
                                            UBool escapeUnprintable) ধ্রুব
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
    যদি (len >= 4 && list[0] == 0 && limit == len && !hasStrings()) {
        // Emit the inverse
        result.append(u'^');
        // Offsetting the inversion list index by one lets us
        // iterate over the ranges of the set complement.
        i = 1;
        --limit;
    }

    // Emit the ranges as pairs.
    যাবত্ (i < limit) {
        UChar32 start = list[i];  // getRangeStart()
        UChar32 end = list[i + 1] - 1;  // getRangeEnd() = range limit minus one
        যদি (!(0xd800 <= end && end <= 0xdbff)) {
            _appendToPat(result, start, end, escapeUnprintable);
            i += 2;
        } অন্যথা {
            // The range ends with a lead surrogate.
            // Avoid writing what looks like a lead+trail surrogate pair.
            // 1. Postpone ranges that start with a lead surrogate code point.
            int32_t firstLead = i;
            যাবত্ ((i += 2) < limit && list[i] <= 0xdbff) {}
            int32_t firstAfterLead = i;
            // 2. Write following ranges that start with a trail surrogate code point.
            যাবত্ (i < limit && (start = list[i]) <= 0xdfff) {
                _appendToPat(result, start, list[i + 1] - 1, escapeUnprintable);
                i += 2;
            }
            // 3. Now write the postponed ranges.
            প্রতি (পূর্ণাঙ্ক j = firstLead; j < firstAfterLead; j += 2) {
                _appendToPat(result, list[j], list[j + 1] - 1, escapeUnprintable);
            }
        }
    }

    যদি (strings_ != nullptr) {
        প্রতি (int32_t i = 0; i<strings_->size(); ++i) {
            result.append(u'{');
            _appendToPat(result,
                         *static_cast<ধ্রুব UnicodeString*>(strings_->elementAt(i)),
                         escapeUnprintable);
            result.append(u'}');
        }
    }
    প্রত্যাগম result.append(u']');
}

/**
* Release existing cached pattern
*/
শূন্য UnicodeSet::releasePattern() {
    যদি (pat) {
        uprv_free(pat);
        pat = nullptr;
        patLen = 0;
    }
}

/**
* Set the new pattern to cache.
*/
শূন্য UnicodeSet::setPattern(ধ্রুব char16_t *newPat, int32_t newPatLen) {
    releasePattern();
    pat = static_cast<char16_t*>(uprv_malloc((newPatLen + 1) * পরিমাণ(char16_t)));
    যদি (pat) {
        patLen = newPatLen;
        u_memcpy(pat, newPat, patLen);
        pat[patLen] = 0;
    }
    // else we don't care if malloc failed. This was just a nice cache.
    // We can regenerate an equivalent pattern later when requested.
}

UnicodeSet* UnicodeSet::freeze() U_LIFETIME_BOUND {
    যদি(!isFrozen() && !isBogus()) {
        compact();

        // Optimize contains() and span() and similar functions.
        যদি (hasStrings()) {
            stringSpan = নূতন UnicodeSetStringSpan(*এতত্, *strings_, UnicodeSetStringSpan::ALL);
            যদি (stringSpan == nullptr) {
                setToBogus();
                প্রত্যাগম এতত্;
            } অন্যথা যদি (!stringSpan->needsStringSpanUTF16()) {
                // All strings are irrelevant for span() etc. because
                // all of each string's code points are contained in this set.
                // Do not check needsStringSpanUTF8() because UTF-8 has at most as
                // many relevant strings as UTF-16.
                // (Thus needsStringSpanUTF8() implies needsStringSpanUTF16().)
                delete stringSpan;
                stringSpan = nullptr;
            }
        }
        যদি (stringSpan == nullptr) {
            // No span-relevant strings: Optimize for code point spans.
            bmpSet=নূতন BMPSet(list, len);
            যদি (bmpSet == nullptr) { // Check for memory allocation error.
                setToBogus();
            }
        }
    }
    প্রত্যাগম এতত্;
}

int32_t UnicodeSet::span(ধ্রুব char16_t *s, int32_t length, USetSpanCondition spanCondition) ধ্রুব {
    যদি(length>0 && bmpSet!=nullptr) {
        প্রত্যাগম static_cast<int32_t>(bmpSet->span(s, s + length, spanCondition) - s);
    }
    যদি(length<0) {
        length=u_strlen(s);
    }
    যদি(length==0) {
        প্রত্যাগম 0;
    }
    যদি(stringSpan!=nullptr) {
        প্রত্যাগম stringSpan->span(s, length, spanCondition);
    } অন্যথা যদি(hasStrings()) {
        uint32_t which= spanCondition==USET_SPAN_NOT_CONTAINED ?
                            UnicodeSetStringSpan::FWD_UTF16_NOT_CONTAINED :
                            UnicodeSetStringSpan::FWD_UTF16_CONTAINED;
        UnicodeSetStringSpan strSpan(*এতত্, *strings_, which);
        যদি(strSpan.needsStringSpanUTF16()) {
            প্রত্যাগম strSpan.span(s, length, spanCondition);
        }
    }

    যদি(spanCondition!=USET_SPAN_NOT_CONTAINED) {
        spanCondition=USET_SPAN_CONTAINED;  // Pin to 0/1 values.
    }

    UChar32 c;
    int32_t start=0, prev=0;
    কুরু {
        U16_NEXT(s, start, length, c);
        যদি(spanCondition!=contains(c)) {
            ভঙ্গ;
        }
    } যাবত্((prev=start)<length);
    প্রত্যাগম prev;
}

int32_t UnicodeSet::spanBack(ধ্রুব char16_t *s, int32_t length, USetSpanCondition spanCondition) ধ্রুব {
    যদি(length>0 && bmpSet!=nullptr) {
        প্রত্যাগম static_cast<int32_t>(bmpSet->spanBack(s, s + length, spanCondition) - s);
    }
    যদি(length<0) {
        length=u_strlen(s);
    }
    যদি(length==0) {
        প্রত্যাগম 0;
    }
    যদি(stringSpan!=nullptr) {
        প্রত্যাগম stringSpan->spanBack(s, length, spanCondition);
    } অন্যথা যদি(hasStrings()) {
        uint32_t which= spanCondition==USET_SPAN_NOT_CONTAINED ?
                            UnicodeSetStringSpan::BACK_UTF16_NOT_CONTAINED :
                            UnicodeSetStringSpan::BACK_UTF16_CONTAINED;
        UnicodeSetStringSpan strSpan(*এতত্, *strings_, which);
        যদি(strSpan.needsStringSpanUTF16()) {
            প্রত্যাগম strSpan.spanBack(s, length, spanCondition);
        }
    }

    যদি(spanCondition!=USET_SPAN_NOT_CONTAINED) {
        spanCondition=USET_SPAN_CONTAINED;  // Pin to 0/1 values.
    }

    UChar32 c;
    int32_t prev=length;
    কুরু {
        U16_PREV(s, 0, length, c);
        যদি(spanCondition!=contains(c)) {
            ভঙ্গ;
        }
    } যাবত্((prev=length)>0);
    প্রত্যাগম prev;
}

int32_t UnicodeSet::spanUTF8(ধ্রুব বর্ণ *s, int32_t length, USetSpanCondition spanCondition) ধ্রুব {
    যদি(length>0 && bmpSet!=nullptr) {
        ধ্রুব uint8_t* s0 = reinterpret_cast<ধ্রুব uint8_t*>(s);
        প্রত্যাগম static_cast<int32_t>(bmpSet->spanUTF8(s0, length, spanCondition) - s0);
    }
    যদি(length<0) {
        length = static_cast<int32_t>(uprv_strlen(s));
    }
    যদি(length==0) {
        প্রত্যাগম 0;
    }
    যদি(stringSpan!=nullptr) {
        প্রত্যাগম stringSpan->spanUTF8(reinterpret_cast<ধ্রুব uint8_t*>(s), length, spanCondition);
    } অন্যথা যদি(hasStrings()) {
        uint32_t which= spanCondition==USET_SPAN_NOT_CONTAINED ?
                            UnicodeSetStringSpan::FWD_UTF8_NOT_CONTAINED :
                            UnicodeSetStringSpan::FWD_UTF8_CONTAINED;
        UnicodeSetStringSpan strSpan(*এতত্, *strings_, which);
        যদি(strSpan.needsStringSpanUTF8()) {
            প্রত্যাগম strSpan.spanUTF8(reinterpret_cast<ধ্রুব uint8_t*>(s), length, spanCondition);
        }
    }

    যদি(spanCondition!=USET_SPAN_NOT_CONTAINED) {
        spanCondition=USET_SPAN_CONTAINED;  // Pin to 0/1 values.
    }

    UChar32 c;
    int32_t start=0, prev=0;
    কুরু {
        U8_NEXT_OR_FFFD(s, start, length, c);
        যদি(spanCondition!=contains(c)) {
            ভঙ্গ;
        }
    } যাবত্((prev=start)<length);
    প্রত্যাগম prev;
}

int32_t UnicodeSet::spanBackUTF8(ধ্রুব বর্ণ *s, int32_t length, USetSpanCondition spanCondition) ধ্রুব {
    যদি(length>0 && bmpSet!=nullptr) {
        ধ্রুব uint8_t* s0 = reinterpret_cast<ধ্রুব uint8_t*>(s);
        প্রত্যাগম bmpSet->spanBackUTF8(s0, length, spanCondition);
    }
    যদি(length<0) {
        length = static_cast<int32_t>(uprv_strlen(s));
    }
    যদি(length==0) {
        প্রত্যাগম 0;
    }
    যদি(stringSpan!=nullptr) {
        প্রত্যাগম stringSpan->spanBackUTF8(reinterpret_cast<ধ্রুব uint8_t*>(s), length, spanCondition);
    } অন্যথা যদি(hasStrings()) {
        uint32_t which= spanCondition==USET_SPAN_NOT_CONTAINED ?
                            UnicodeSetStringSpan::BACK_UTF8_NOT_CONTAINED :
                            UnicodeSetStringSpan::BACK_UTF8_CONTAINED;
        UnicodeSetStringSpan strSpan(*এতত্, *strings_, which);
        যদি(strSpan.needsStringSpanUTF8()) {
            প্রত্যাগম strSpan.spanBackUTF8(reinterpret_cast<ধ্রুব uint8_t*>(s), length, spanCondition);
        }
    }

    যদি(spanCondition!=USET_SPAN_NOT_CONTAINED) {
        spanCondition=USET_SPAN_CONTAINED;  // Pin to 0/1 values.
    }

    UChar32 c;
    int32_t prev=length;
    কুরু {
        U8_PREV_OR_FFFD(s, 0, length, c);
        যদি(spanCondition!=contains(c)) {
            ভঙ্গ;
        }
    } যাবত্((prev=length)>0);
    প্রত্যাগম prev;
}

U_NAMESPACE_END
