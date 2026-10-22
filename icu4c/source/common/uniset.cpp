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

#अन्तर्भाव "unicode/utypes.h"
#अन्तर्भाव "unicode/parsepos.h"
#अन्तर्भाव "unicode/symtable.h"
#अन्तर्भाव "unicode/uniset.h"
#अन्तर्भाव "unicode/ustring.h"
#अन्तर्भाव "unicode/utf8.h"
#अन्तर्भाव "unicode/utf16.h"
#अन्तर्भाव "ruleiter.h"
#अन्तर्भाव "cmemory.h"
#अन्तर्भाव "cstring.h"
#अन्तर्भाव "patternprops.h"
#अन्तर्भाव "uelement.h"
#अन्तर्भाव "util.h"
#अन्तर्भाव "uvector.h"
#अन्तर्भाव "charstr.h"
#अन्तर्भाव "ustrfmt.h"
#अन्तर्भाव "uassert.h"
#अन्तर्भाव "bmpset.h"
#अन्तर्भाव "unisetspan.h"

// HIGH_VALUE > all valid values. 110000 for codepoints
#define UNICODESET_वरचा 0x0110000

// LOW <= all valid values. ZERO for codepoints
#define UNICODESET_खालचा 0x000000

/** Max list [0, 1, 2, ..., max code point, HIGH] */
constexpr int32_t MAX_LENGTH = UNICODESET_वरचा + 1;

U_NAMESPACE_BEGIN

चिन्ह_Table::~चिन्ह_Table() {}

UOBJECT_DEFINE_RTTI_IMPLEMENTATION(UnicodeSet)

/**
 * Modify the given UChar32 variable so that it is in range, by
 * pinning values < UNICODESET_LOW to UNICODESET_LOW, and
 * pinning values > UNICODESET_HIGH-1 to UNICODESET_HIGH-1.
 * It modifies its argument in-place and also returns it.
 */
स्थिर ओळीत UChar32 pinCodePoint(UChar32& c) {
    जर (c < UNICODESET_खालचा) {
        c = UNICODESET_खालचा;
    } अन्यथा जर (c > (UNICODESET_वरचा-1)) {
        c = (UNICODESET_वरचा-1);
    }
    परत c;
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
#अन्तर्भाव <stdio.h>
स्थिर int32_t dbg_मोजणी = 0;

स्थिर ओळीत शून्य _dbgct(UnicodeSet* set) {
    UnicodeString str;
    set->toPattern(str, सत्य);
    अक्षर buf[40];
    str.extract(0, 39, buf, "");
    म_लिखो("DEBUG UnicodeSet: ct 0x%08X; %d %s\n", set, ++dbg_मोजणी, buf);
}

स्थिर ओळीत शून्य _dbgdt(UnicodeSet* set) {
    UnicodeString str;
    set->toPattern(str, सत्य);
    अक्षर buf[40];
    str.extract(0, 39, buf, "");
    म_लिखो("DEBUG UnicodeSet: dt 0x%08X; %d %s\n", set, --dbg_मोजणी, buf);
}

#अन्यथा

#define _dbgct(set)
#define _dbgdt(set)

#endif

//----------------------------------------------------------------
// UnicodeString in UVector support
//----------------------------------------------------------------

स्थिर शून्य U_CALLCONV cloneUnicodeString(UElement *dst, UElement *src) {
    dst->pointer = नवीन UnicodeString(*static_cast<UnicodeString*>(src->pointer));
}

स्थिर int32_t U_CALLCONV compareUnicodeString(UElement t1, UElement t2) {
    स्थिरांक UnicodeString& a = *static_cast<स्थिरांक UnicodeString*>(t1.pointer);
    स्थिरांक UnicodeString& b = *static_cast<स्थिरांक UnicodeString*>(t2.pointer);
    परत a.compare(b);
}

UBool UnicodeSet::hasStrings() स्थिरांक {
    परत strings_ != nullptr && !strings_->isEmpty();
}

int32_t UnicodeSet::stringsSize() स्थिरांक {
    परत strings_ == nullptr ? 0 : strings_->size();
}

UBool UnicodeSet::stringsContains(स्थिरांक UnicodeString &s) स्थिरांक {
    परत strings_ != nullptr && strings_->contains((शून्य*) &s);
}

//----------------------------------------------------------------
// Constructors &c
//----------------------------------------------------------------

/**
 * Constructs an empty set.
 */
UnicodeSet::UnicodeSet() {
    list[0] = UNICODESET_वरचा;
    _dbgct(हे);
}

/**
 * Constructs a set containing the given range. If <code>end >
 * start</code> then an empty set is created.
 *
 * @param start first character, inclusive, of range
 * @param end last character, inclusive, of range
 */
UnicodeSet::UnicodeSet(UChar32 start, UChar32 end) {
    list[0] = UNICODESET_वरचा;
    add(start, end);
    _dbgct(हे);
}

/**
 * Constructs a set that is identical to the given UnicodeSet.
 */
UnicodeSet::UnicodeSet(स्थिरांक UnicodeSet& o) : UnicodeFilter(o) {
    *हे = o;
    _dbgct(हे);
}

// Copy-construct as thawed.
UnicodeSet::UnicodeSet(स्थिरांक UnicodeSet& o, UBool /* asThawed */) : UnicodeFilter(o) {
    जर (ensureCapacity(o.len)) {
        // *this = o except for bmpSet and stringSpan
        len = o.len;
        uprv_memcpy(list, o.list, (माप_प्रकार)len*आकार(UChar32));
        जर (o.hasStrings()) {
            UErrorCode status = U_ZERO_ERROR;
            जर (!allocateStrings(status) ||
                    (strings_->assign(*o.strings_, cloneUnicodeString, status), U_FAILURE(status))) {
                setToBogus();
                परत;
            }
        }
        जर (o.pat) {
            setPattern(o.pat, o.patLen);
        }
        _dbgct(हे);
    }
}

/**
 * Destructs the set.
 */
UnicodeSet::~UnicodeSet() {
    _dbgdt(हे); // first!
    जर (list != चळत_List) {
        uprv_free(list);
    }
    delete bmpSet;
    जर (buffer != चळत_List) {
        uprv_free(buffer);
    }
    delete strings_;
    delete stringSpan;
    releasePattern();
}

/**
 * Assigns this object to be a copy of another.
 */
UnicodeSet& UnicodeSet::चालक=(स्थिरांक UnicodeSet& o) U_LIFETIME_BOUND {
    परत copyFrom(o, असत्य);
}

UnicodeSet& UnicodeSet::copyFrom(स्थिरांक UnicodeSet& o, UBool asThawed) U_LIFETIME_BOUND {
    जर (हे == &o) {
        परत *हे;
    }
    जर (isFrozen()) {
        परत *हे;
    }
    जर (o.isBogus()) {
        setToBogus();
        परत *हे;
    }
    जर (!ensureCapacity(o.len)) {
        // ensureCapacity will mark the UnicodeSet as Bogus if OOM failure happens.
        परत *हे;
    }
    len = o.len;
    uprv_memcpy(list, o.list, (माप_प्रकार)len*आकार(UChar32));
    जर (o.bmpSet != nullptr && !asThawed) {
        bmpSet = नवीन BMPSet(*o.bmpSet, list, len);
        जर (bmpSet == nullptr) { // Check for memory allocation error.
            setToBogus();
            परत *हे;
        }
    }
    जर (o.hasStrings()) {
        UErrorCode status = U_ZERO_ERROR;
        जर ((strings_ == nullptr && !allocateStrings(status)) ||
                (strings_->assign(*o.strings_, cloneUnicodeString, status), U_FAILURE(status))) {
            setToBogus();
            परत *हे;
        }
    } अन्यथा जर (hasStrings()) {
        strings_->removeAllElements();
    }
    जर (o.stringSpan != nullptr && strings_ != nullptr && !asThawed) {
        stringSpan = नवीन UnicodeSetStringSpan(*o.stringSpan, *strings_);
        जर (stringSpan == nullptr) { // Check for memory allocation error.
            setToBogus();
            परत *हे;
        }
    }
    releasePattern();
    जर (o.pat) {
        setPattern(o.pat, o.patLen);
    }
    परत *हे;
}

/**
 * Returns a copy of this object.  All UnicodeMatcher objects have
 * to support cloning in order to allow classes using
 * UnicodeMatchers, such as Transliterator, to implement cloning.
 */
UnicodeSet* UnicodeSet::clone() स्थिरांक {
    परत नवीन UnicodeSet(*हे);
}

UnicodeSet *UnicodeSet::cloneAsThawed() स्थिरांक {
    परत नवीन UnicodeSet(*हे, सत्य);
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
तार्किक UnicodeSet::चालक==(स्थिरांक UnicodeSet& o) स्थिरांक {
    जर (len != o.len) परत असत्य;
    साठी (int32_t i = 0; i < len; ++i) {
        जर (list[i] != o.list[i]) परत असत्य;
    }
    जर (hasStrings() != o.hasStrings()) { परत असत्य; }
    जर (hasStrings() && *strings_ != *o.strings_) परत असत्य;
    परत सत्य;
}

/**
 * Returns the hash code value for this set.
 *
 * @return the hash code value for this set.
 * @see Object#hashCode()
 */
int32_t UnicodeSet::hashCode() स्थिरांक {
    uint32_t result = static_cast<uint32_t>(len);
    साठी (int32_t i = 0; i < len; ++i) {
        result *= 1000003u;
        result += list[i];
    }
    परत static_cast<int32_t>(result);
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
int32_t UnicodeSet::size() स्थिरांक {
    int32_t n = 0;
    int32_t count = get_Range_मोजणी();
    साठी (int32_t i = 0; i < count; ++i) {
        n += getRangeEnd(i) - getRangeStart(i) + 1;
    }
    परत n + stringsSize();
}

/**
 * Returns <tt>true</tt> if this set contains no elements.
 *
 * @return <tt>true</tt> if this set contains no elements.
 */
UBool UnicodeSet::isEmpty() स्थिरांक {
    परत len == 1 && !hasStrings();
}

/**
 * Returns true if this set contains the given character.
 * @param c character to be checked for containment
 * @return true if the test condition is met
 */
UBool UnicodeSet::contains(UChar32 c) स्थिरांक {
    // Set i to the index of the start item greater than ch
    // We know we will terminate without length test!
    // LATER: for large sets, add binary search
    //int32_t i = -1;
    //for (;;) {
    //    if (c < list[++i]) break;
    //}
    जर (bmpSet != nullptr) {
        परत bmpSet->contains(c);
    }
    जर (stringSpan != nullptr) {
        परत stringSpan->contains(c);
    }
    जर (c >= UNICODESET_वरचा) { // Don't need to check LOW bound
        परत असत्य;
    }
    int32_t i = findCodePoint(c);
    परत i & 1; // return true if odd
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
int32_t UnicodeSet::findCodePoint(UChar32 c) स्थिरांक {
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
    जर (c < list[0])
        परत 0;
    // High runner test.  c is often after the last range, so an
    // initial check for this condition pays off.
    int32_t lo = 0;
    int32_t hi = len - 1;
    जर (lo >= hi || c >= list[hi-1])
        परत hi;
    // invariant: c >= list[lo]
    // invariant: c < list[hi]
    साठी (;;) {
        int32_t i = (lo + hi) >> 1;
        जर (i == lo) {
            थांबा; // Found!
        } अन्यथा जर (c < list[i]) {
            hi = i;
        } अन्यथा {
            lo = i;
        }
    }
    परत hi;
}

/**
 * Returns true if this set contains every character
 * of the given range.
 * @param start first character, inclusive, of the range
 * @param end last character, inclusive, of the range
 * @return true if the test condition is met
 */
UBool UnicodeSet::contains(UChar32 start, UChar32 end) स्थिरांक {
    //int32_t i = -1;
    //for (;;) {
    //    if (start < list[++i]) break;
    //}
    int32_t i = findCodePoint(start);
    परत ((i & 1) != 0 && end < list[i]);
}

/**
 * Returns <tt>true</tt> if this set contains the given
 * multicharacter string.
 * @param s string to be checked for containment
 * @return <tt>true</tt> if this set contains the specified string
 */
UBool UnicodeSet::contains(स्थिरांक UnicodeString& s) स्थिरांक {
    int32_t cp = getSingleCP(s);
    जर (cp < 0) {
        परत stringsContains(s);
    } अन्यथा {
        परत contains(static_cast<UChar32>(cp));
    }
}

/**
 * Returns true if this set contains all the characters and strings
 * of the given set.
 * @param c set to be checked for containment
 * @return true if the test condition is met
 */
UBool UnicodeSet::containsAll(स्थिरांक UnicodeSet& c) स्थिरांक {
    // The specified set is a subset if all of its pairs are contained in
    // this set.  It's possible to code this more efficiently in terms of
    // direct manipulation of the inversion lists if the need arises.
    int32_t n = c.get_Range_मोजणी();
    साठी (पूर्णांक i=0; i<n; ++i) {
        जर (!contains(c.getRangeStart(i), c.getRangeEnd(i))) {
            परत असत्य;
        }
    }
    परत !c.hasStrings() || (strings_ != nullptr && strings_->containsAll(*c.strings_));
}

/**
 * Returns true if this set contains all the characters
 * of the given string.
 * @param s string containing characters to be checked for containment
 * @return true if the test condition is met
 */
UBool UnicodeSet::containsAll(स्थिरांक UnicodeString& s) स्थिरांक {
    परत span(s.getBuffer(), s.length(), USET_SPAN_CONTAINED) == s.length();
}

/**
 * Returns true if this set contains none of the characters
 * of the given range.
 * @param start first character, inclusive, of the range
 * @param end last character, inclusive, of the range
 * @return true if the test condition is met
 */
UBool UnicodeSet::containsNone(UChar32 start, UChar32 end) स्थिरांक {
    //int32_t i = -1;
    //for (;;) {
    //    if (start < list[++i]) break;
    //}
    int32_t i = findCodePoint(start);
    परत ((i & 1) == 0 && end < list[i]);
}

/**
 * Returns true if this set contains none of the characters and strings
 * of the given set.
 * @param c set to be checked for containment
 * @return true if the test condition is met
 */
UBool UnicodeSet::containsNone(स्थिरांक UnicodeSet& c) स्थिरांक {
    // The specified set is a subset if all of its pairs are contained in
    // this set.  It's possible to code this more efficiently in terms of
    // direct manipulation of the inversion lists if the need arises.
    int32_t n = c.get_Range_मोजणी();
    साठी (int32_t i=0; i<n; ++i) {
        जर (!containsNone(c.getRangeStart(i), c.getRangeEnd(i))) {
            परत असत्य;
        }
    }
    परत strings_ == nullptr || !c.hasStrings() || strings_->containsNone(*c.strings_);
}

/**
 * Returns true if this set contains none of the characters
 * of the given string.
 * @param s string containing characters to be checked for containment
 * @return true if the test condition is met
 */
UBool UnicodeSet::containsNone(स्थिरांक UnicodeString& s) स्थिरांक {
    परत span(s.getBuffer(), s.length(), USET_SPAN_NOT_CONTAINED) == s.length();
}

/**
 * Returns <tt>true</tt> if this set contains any character whose low byte
 * is the given value.  This is used by <tt>RuleBasedTransliterator</tt> for
 * indexing.
 */
UBool UnicodeSet::matches_Index_मूल्य(uint8_t v) स्थिरांक {
    /* The index value v, in the range [0,255], is contained in this set if
     * it is contained in any pair of this set.  Pairs either have the high
     * bytes equal, or unequal.  If the high bytes are equal, then we have
     * aaxx..aayy, where aa is the high byte.  Then v is contained if xx <=
     * v <= yy.  If the high bytes are unequal we have aaxx..bbyy, bb>aa.
     * Then v is contained if xx <= v || v <= yy.  (This is identical to the
     * time zone month containment logic.)
     */
    int32_t i;
    int32_t range_मोजणी=get_Range_मोजणी();
    साठी (i=0; i<range_मोजणी; ++i) {
        UChar32 खालचा = getRangeStart(i);
        UChar32 वरचा = getRangeEnd(i);
        जर ((खालचा & ~0xFF) == (वरचा & ~0xFF)) {
            जर ((खालचा & 0xFF) <= v && v <= (वरचा & 0xFF)) {
                परत सत्य;
            }
        } अन्यथा जर ((खालचा & 0xFF) <= v || v <= (वरचा & 0xFF)) {
            परत सत्य;
        }
    }
    जर (hasStrings()) {
        साठी (i=0; i<strings_->size(); ++i) {
            स्थिरांक UnicodeString& s = *static_cast<स्थिरांक UnicodeString*>(strings_->elementAt(i));
            जर (s.isEmpty()) {
                सुरूठेवा;  // skip the empty string
            }
            UChar32 c = s.char32At(0);
            जर ((c & 0xFF) == v) {
                परत सत्य;
            }
        }
    }
    परत असत्य;
}

/**
 * Implementation of UnicodeMatcher::matches().  Always matches the
 * longest possible multichar string.
 */
UMatchDegree UnicodeSet::matches(स्थिरांक Replaceable& text,
                                 int32_t& offset,
                                 int32_t limit,
                                 UBool incremental) {
    जर (offset == limit) {
        जर (contains(U_ETHER)) {
            परत incremental ? U_PARTIAL_MATCH : U_MATCH;
        } अन्यथा {
            परत U_MISMATCH;
        }
    } अन्यथा {
        जर (hasStrings()) { // try strings first

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
            int32_t वरचा_Water_Length = 0;

            साठी (i=0; i<strings_->size(); ++i) {
                स्थिरांक UnicodeString& trial = *static_cast<स्थिरांक UnicodeString*>(strings_->elementAt(i));
                जर (trial.isEmpty()) {
                    सुरूठेवा;  // skip the empty string
                }

                char16_t c = trial.charAt(forward ? 0 : trial.length() - 1);

                // Strings are sorted, so we can optimize in the
                // forward direction.
                जर (forward && c > firstChar) थांबा;
                जर (c != firstChar) सुरूठेवा;

                int32_t matchLen = matchRest(text, offset, limit, trial);

                जर (incremental) {
                    int32_t maxLen = forward ? limit-offset : offset-limit;
                    जर (matchLen == maxLen) {
                        // We have successfully matched but only up to limit.
                        परत U_PARTIAL_MATCH;
                    }
                }

                जर (matchLen == trial.length()) {
                    // We have successfully matched the whole string.
                    जर (matchLen > वरचा_Water_Length) {
                        वरचा_Water_Length = matchLen;
                    }
                    // In the forward direction we know strings
                    // are sorted so we can bail early.
                    जर (forward && matchLen < वरचा_Water_Length) {
                        थांबा;
                    }
                    सुरूठेवा;
                }
            }

            // We've checked all strings without a partial match.
            // If we have full matches, return the longest one.
            जर (वरचा_Water_Length != 0) {
                offset += forward ? वरचा_Water_Length : -वरचा_Water_Length;
                परत U_MATCH;
            }
        }
        परत UnicodeFilter::matches(text, offset, limit, incremental);
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
int32_t UnicodeSet::matchRest(स्थिरांक Replaceable& text,
                              int32_t start, int32_t limit,
                              स्थिरांक UnicodeString& s) {
    int32_t i;
    int32_t maxLen;
    int32_t slen = s.length();
    जर (start < limit) {
        maxLen = limit - start;
        जर (maxLen > slen) maxLen = slen;
        साठी (i = 1; i < maxLen; ++i) {
            जर (text.charAt(start + i) != s.charAt(i)) परत 0;
        }
    } अन्यथा {
        maxLen = start - limit;
        जर (maxLen > slen) maxLen = slen;
        --slen; // <=> slen = s.length() - 1;
        साठी (i = 1; i < maxLen; ++i) {
            जर (text.charAt(start - i) != s.charAt(slen - i)) परत 0;
        }
    }
    परत maxLen;
}

/**
 * Implement of UnicodeMatcher
 */
शून्य UnicodeSet::addMatchSetTo(UnicodeSet& toUnionTo) स्थिरांक {
    toUnionTo.addAll(*हे);
}

/**
 * Returns the index of the given character within this set, where
 * the set is ordered by ascending code point.  If the character
 * is not in this set, return -1.  The inverse of this method is
 * <code>charAt()</code>.
 * @return an index from 0..size()-1, or -1
 */
int32_t UnicodeSet::indexOf(UChar32 c) स्थिरांक {
    जर (c < MIN_मूल्य || c > MAX_मूल्य) {
        परत -1;
    }
    int32_t i = 0;
    int32_t n = 0;
    साठी (;;) {
        UChar32 start = list[i++];
        जर (c < start) {
            परत -1;
        }
        UChar32 limit = list[i++];
        जर (c < limit) {
            परत n + c - start;
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
UChar32 UnicodeSet::charAt(int32_t index) स्थिरांक {
    जर (index >= 0) {
        // len2 is the largest even integer <= len, that is, it is len
        // for even values and len-1 for odd values.  With odd values
        // the last entry is UNICODESET_HIGH.
        int32_t len2 = len & ~1;
        साठी (int32_t i=0; i < len2;) {
            UChar32 start = list[i++];
            int32_t count = list[i++] - start;
            जर (index < count) {
                परत static_cast<UChar32>(start + index);
            }
            index -= count;
        }
    }
    परत static_cast<UChar32>(-1);
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
    परत *हे;
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
    जर (pinCodePoint(start) < pinCodePoint(end)) {
        UChar32 limit = end + 1;
        // Fast path for adding a new range after the last one.
        // Odd list length: [..., lastStart, lastLimit, HIGH]
        जर ((len & 1) != 0) {
            // If the list is empty, set lastLimit low enough to not be adjacent to 0.
            UChar32 lastLimit = len == 1 ? -2 : list[len - 2];
            जर (lastLimit <= start && !isFrozen() && !isBogus()) {
                जर (lastLimit == start) {
                    // Extend the last range.
                    list[len - 2] = limit;
                    जर (limit == UNICODESET_वरचा) {
                        --len;
                    }
                } अन्यथा {
                    list[len - 1] = start;
                    जर (limit < UNICODESET_वरचा) {
                        जर (ensureCapacity(len + 2)) {
                            list[len++] = limit;
                            list[len++] = UNICODESET_वरचा;
                        }
                    } अन्यथा {  // limit == UNICODESET_HIGH
                        जर (ensureCapacity(len + 1)) {
                            list[len++] = UNICODESET_वरचा;
                        }
                    }
                }
                releasePattern();
                परत *हे;
            }
        }
        // This is slow. Could be much faster using findCodePoint(start)
        // and modifying the list, dealing with adjacent & overlapping ranges.
        UChar32 range[3] = { start, limit, UNICODESET_वरचा };
        add(range, 2, 0);
    } अन्यथा जर (start == end) {
        add(start);
    }
    परत *हे;
}

// #define DEBUG_US_ADD

#ifdef DEBUG_US_ADD
#अन्तर्भाव <stdio.h>
शून्य dump(UChar32 c) {
    जर (c <= 0xFF) {
        म_लिखो("%c", (अक्षर)c);
    } अन्यथा {
        म_लिखो("U+%04X", c);
    }
}
शून्य dump(स्थिरांक UChar32* list, int32_t len) {
    म_लिखो("[");
    साठी (int32_t i=0; i<len; ++i) {
        जर (i != 0) म_लिखो(", ");
        dump(list[i]);
    }
    म_लिखो("]");
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
    जर ((i & 1) != 0  || isFrozen() || isBogus()) परत *हे;

    // HIGH is 0x110000
    // assert(list[len-1] == HIGH);

    // empty = [HIGH]
    // [start_0, limit_0, start_1, limit_1, HIGH]

    // [..., start_k-1, limit_k-1, start_k, limit_k, ..., HIGH]
    //                             ^
    //                             list[i]

    // i == 0 means c is before the first range

#ifdef DEBUG_US_ADD
    म_लिखो("Add of ");
    dump(c);
    म_लिखो(" found at %d", i);
    म_लिखो(": ");
    dump(list, len);
    म_लिखो(" => ");
#endif

    जर (c == list[i]-1) {
        // c is before start of next range
        list[i] = c;
        // if we touched the HIGH mark, then add a new one
        जर (c == (UNICODESET_वरचा - 1)) {
            जर (!ensureCapacity(len+1)) {
                // ensureCapacity will mark the object as Bogus if OOM failure happens.
                परत *हे;
            }
            list[len++] = UNICODESET_वरचा;
        }
        जर (i > 0 && c == list[i-1]) {
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
            पर्यंत (src < srclimit) *(dst++) = *(src++);

            len -= 2;
        }
    }

    अन्यथा जर (i > 0 && c == list[i-1]) {
        // c is after end of prior range
        list[i-1]++;
        // no need to check for collapse here
    }

    अन्यथा {
        // At this point we know the new char is not adjacent to
        // any existing ranges, and it is not 10FFFF.


        // [..., start_k-1, limit_k-1, start_k, limit_k, ..., HIGH]
        //                             ^
        //                             list[i]

        // [..., start_k-1, limit_k-1, c, c+1, start_k, limit_k, ..., HIGH]
        //                             ^
        //                             list[i]

        जर (!ensureCapacity(len+2)) {
            // ensureCapacity will mark the object as Bogus if OOM failure happens.
            परत *हे;
        }

        UChar32 *p = list + i;
        uprv_memmove(p + 2, p, (len - i) * आकार(*p));
        list[i] = c;
        list[i+1] = c+1;
        len += 2;
    }

#ifdef DEBUG_US_ADD
    dump(list, len);
    म_लिखो("\n");

    साठी (i=1; i<len; ++i) {
        जर (list[i] <= list[i-1]) {
            // Corrupt array!
            म_लिखो("ERROR: list has been corrupted\n");
            निकास(1);
        }
    }
#endif

    releasePattern();
    परत *हे;
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
UnicodeSet& UnicodeSet::add(स्थिरांक UnicodeString& s) U_LIFETIME_BOUND {
    जर (isFrozen() || isBogus()) परत *हे;
    int32_t cp = getSingleCP(s);
    जर (cp < 0) {
        जर (!stringsContains(s)) {
            _add(s);
            releasePattern();
        }
    } अन्यथा {
        add(static_cast<UChar32>(cp));
    }
    परत *हे;
}

/**
 * Adds the given string, in order, to 'strings_'.  The given string
 * must have been checked by the caller to not already be in 'strings_'.
 */
शून्य UnicodeSet::_add(स्थिरांक UnicodeString& s) {
    जर (isFrozen() || isBogus()) {
        परत;
    }
    UErrorCode ec = U_ZERO_ERROR;
    जर (strings_ == nullptr && !allocateStrings(ec)) {
        setToBogus();
        परत;
    }
    LocalPointer<UnicodeString> t(नवीन UnicodeString(s));
    जर (t.isNull()) { // Check for memory allocation error.
        setToBogus();
        परत;
    }
    strings_->sorted_घाला(t.orphan(), compareUnicodeString, ec);
    जर (U_FAILURE(ec)) {
        setToBogus();
    }
}

/**
 * @return a code point IF the string consists of a single one.
 * otherwise returns -1.
 * @param string to test
 */
int32_t UnicodeSet::getSingleCP(स्थिरांक UnicodeString& s) {
    int32_t sLength = s.length();
    जर (sLength == 1) परत s.charAt(0);
    जर (sLength == 2) {
        UChar32 cp = s.char32At(0);
        जर (cp > 0xFFFF) { // is surrogate pair
            परत cp;
        }
    }
    परत -1;
}

/**
 * Adds each of the characters in this string to the set. Thus "ch" => {"c", "h"}
 * If this set already any particular character, it has no effect on that character.
 * @param the source string
 * @return the modified set, for chaining
 */
UnicodeSet& UnicodeSet::addAll(स्थिरांक UnicodeString& s) U_LIFETIME_BOUND {
    UChar32 cp;
    साठी (int32_t i = 0; i < s.length(); i += U16_LENGTH(cp)) {
        cp = s.char32At(i);
        add(cp);
    }
    परत *हे;
}

/**
 * Retains EACH of the characters in this string. Note: "ch" == {"c", "h"}
 * If this set already any particular character, it has no effect on that character.
 * @param the source string
 * @return the modified set, for chaining
 */
UnicodeSet& UnicodeSet::retainAll(स्थिरांक UnicodeString& s) U_LIFETIME_BOUND {
    UnicodeSet set;
    set.addAll(s);
    retainAll(set);
    परत *हे;
}

/**
 * Complement EACH of the characters in this string. Note: "ch" == {"c", "h"}
 * If this set already any particular character, it has no effect on that character.
 * @param the source string
 * @return the modified set, for chaining
 */
UnicodeSet& UnicodeSet::complementAll(स्थिरांक UnicodeString& s) U_LIFETIME_BOUND {
    UnicodeSet set;
    set.addAll(s);
    complementAll(set);
    परत *हे;
}

/**
 * Remove EACH of the characters in this string. Note: "ch" == {"c", "h"}
 * If this set already any particular character, it has no effect on that character.
 * @param the source string
 * @return the modified set, for chaining
 */
UnicodeSet& UnicodeSet::removeAll(स्थिरांक UnicodeString& s) U_LIFETIME_BOUND {
    UnicodeSet set;
    set.addAll(s);
    removeAll(set);
    परत *हे;
}

UnicodeSet& UnicodeSet::removeAllStrings() U_LIFETIME_BOUND {
    जर (!isFrozen() && hasStrings()) {
        strings_->removeAllElements();
        releasePattern();
    }
    परत *हे;
}


/**
 * Makes a set from a multicharacter string. Thus "ch" => {"ch"}
 * <br><b>Warning: you cannot add an empty string ("") to a UnicodeSet.</b>
 * @param the source string
 * @return a newly created set containing the given string
 */
UnicodeSet* U_EXPORT2 UnicodeSet::createFrom(स्थिरांक UnicodeString& s) {
    UnicodeSet *set = नवीन UnicodeSet();
    जर (set != nullptr) { // Check for memory allocation error.
        set->add(s);
    }
    परत set;
}


/**
 * Makes a set from each of the characters in the string. Thus "ch" => {"c", "h"}
 * @param the source string
 * @return a newly created set containing the given characters
 */
UnicodeSet* U_EXPORT2 UnicodeSet::createFromAll(स्थिरांक UnicodeString& s) {
    UnicodeSet *set = नवीन UnicodeSet();
    जर (set != nullptr) { // Check for memory allocation error.
        set->addAll(s);
    }
    परत set;
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
    जर (pinCodePoint(start) <= pinCodePoint(end)) {
        UChar32 range[3] = { start, end+1, UNICODESET_वरचा };
        retain(range, 2, 0);
    } अन्यथा {
        clear();
    }
    परत *हे;
}

UnicodeSet& UnicodeSet::retain(UChar32 c) U_LIFETIME_BOUND {
    परत retain(c, c);
}

UnicodeSet& UnicodeSet::retain(स्थिरांक UnicodeString &s) U_LIFETIME_BOUND {
    जर (isFrozen() || isBogus()) { परत *हे; }
    UChar32 cp = getSingleCP(s);
    जर (cp < 0) {
        तार्किक isIn = stringsContains(s);
        // Check for getRangeCount() first to avoid somewhat-expensive size()
        // when there are single code points.
        जर (isIn && get_Range_मोजणी() == 0 && size() == 1) {
            परत *हे;
        }
        clear();
        जर (isIn) {
            _add(s);
        }
    } अन्यथा {
        retain(cp, cp);
    }
    परत *हे;
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
UnicodeSet& UnicodeSet::हटाओ(UChar32 start, UChar32 end) U_LIFETIME_BOUND {
    जर (pinCodePoint(start) <= pinCodePoint(end)) {
        UChar32 range[3] = { start, end+1, UNICODESET_वरचा };
        retain(range, 2, 2);
    }
    परत *हे;
}

/**
 * Removes the specified character from this set if it is present.
 * The set will not contain the specified range once the call
 * returns.
 */
UnicodeSet& UnicodeSet::हटाओ(UChar32 c) U_LIFETIME_BOUND {
    परत हटाओ(c, c);
}

/**
 * Removes the specified string from this set if it is present.
 * The set will not contain the specified character once the call
 * returns.
 * @param the source string
 * @return the modified set, for chaining
 */
UnicodeSet& UnicodeSet::हटाओ(स्थिरांक UnicodeString& s) U_LIFETIME_BOUND {
    जर (isFrozen() || isBogus()) परत *हे;
    int32_t cp = getSingleCP(s);
    जर (cp < 0) {
        जर (strings_ != nullptr && strings_->removeElement((शून्य*) &s)) {
            releasePattern();
        }
    } अन्यथा {
        हटाओ(static_cast<UChar32>(cp), static_cast<UChar32>(cp));
    }
    परत *हे;
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
    जर (isFrozen() || isBogus()) {
        परत *हे;
    }
    जर (pinCodePoint(start) <= pinCodePoint(end)) {
        UChar32 range[3] = { start, end+1, UNICODESET_वरचा };
        exclusiveOr(range, 2, 0);
    }
    releasePattern();
    परत *हे;
}

UnicodeSet& UnicodeSet::complement(UChar32 c) U_LIFETIME_BOUND {
    परत complement(c, c);
}

/**
 * This is equivalent to
 * <code>complement(MIN_VALUE, MAX_VALUE)</code>.
 */
UnicodeSet& UnicodeSet::complement() U_LIFETIME_BOUND {
    जर (isFrozen() || isBogus()) {
        परत *हे;
    }
    जर (list[0] == UNICODESET_खालचा) {
        uprv_memmove(list, list + 1, (माप_प्रकार)(len-1)*आकार(UChar32));
        --len;
    } अन्यथा {
        जर (!ensureCapacity(len+1)) {
            परत *हे;
        }
        uprv_memmove(list + 1, list, (माप_प्रकार)len*आकार(UChar32));
        list[0] = UNICODESET_खालचा;
        ++len;
    }
    releasePattern();
    परत *हे;
}

/**
 * Complement the specified string in this set.
 * The set will not contain the specified string once the call
 * returns.
 *
 * @param s the string to complement
 * @return this object, for chaining
 */
UnicodeSet& UnicodeSet::complement(स्थिरांक UnicodeString& s) U_LIFETIME_BOUND {
    जर (isFrozen() || isBogus()) परत *हे;
    int32_t cp = getSingleCP(s);
    जर (cp < 0) {
        जर (stringsContains(s)) {
            strings_->removeElement((शून्य*) &s);
        } अन्यथा {
            _add(s);
        }
        releasePattern();
    } अन्यथा {
        complement(static_cast<UChar32>(cp), static_cast<UChar32>(cp));
    }
    परत *हे;
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
UnicodeSet& UnicodeSet::addAll(स्थिरांक UnicodeSet& c) U_LIFETIME_BOUND {
    जर ( c.len>0 && c.list!=nullptr ) {
        add(c.list, c.len, 0);
    }

    // Add strings in order
    जर ( c.strings_!=nullptr ) {
        साठी (int32_t i=0; i<c.strings_->size(); ++i) {
            स्थिरांक UnicodeString* s = static_cast<स्थिरांक UnicodeString*>(c.strings_->elementAt(i));
            जर (!stringsContains(*s)) {
                _add(*s);
            }
        }
    }
    परत *हे;
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
UnicodeSet& UnicodeSet::retainAll(स्थिरांक UnicodeSet& c) U_LIFETIME_BOUND {
    जर (isFrozen() || isBogus()) {
        परत *हे;
    }
    retain(c.list, c.len, 0);
    जर (hasStrings()) {
        जर (!c.hasStrings()) {
            strings_->removeAllElements();
        } अन्यथा {
            strings_->retainAll(*c.strings_);
        }
    }
    परत *हे;
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
UnicodeSet& UnicodeSet::removeAll(स्थिरांक UnicodeSet& c) U_LIFETIME_BOUND {
    जर (isFrozen() || isBogus()) {
        परत *हे;
    }
    retain(c.list, c.len, 2);
    जर (hasStrings() && c.hasStrings()) {
        strings_->removeAll(*c.strings_);
    }
    परत *हे;
}

/**
 * Complements in this set all elements contained in the specified
 * set.  Any character in the other set will be removed if it is
 * in this set, or will be added if it is not in this set.
 *
 * @param c set that defines which elements will be xor'ed from
 *          this set.
 */
UnicodeSet& UnicodeSet::complementAll(स्थिरांक UnicodeSet& c) U_LIFETIME_BOUND {
    जर (isFrozen() || isBogus()) {
        परत *हे;
    }
    exclusiveOr(c.list, c.len, 0);

    जर (c.strings_ != nullptr) {
        साठी (int32_t i=0; i<c.strings_->size(); ++i) {
            शून्य* e = c.strings_->elementAt(i);
            जर (strings_ == nullptr || !strings_->removeElement(e)) {
                _add(*static_cast<स्थिरांक UnicodeString*>(e));
            }
        }
    }
    परत *हे;
}

/**
 * Removes all of the elements from this set.  This set will be
 * empty after this call returns.
 */
UnicodeSet& UnicodeSet::clear() U_LIFETIME_BOUND {
    जर (isFrozen()) {
        परत *हे;
    }
    list[0] = UNICODESET_वरचा;
    len = 1;
    releasePattern();
    जर (strings_ != nullptr) {
        strings_->removeAllElements();
    }
    // Remove bogus
    fFlags = 0;
    परत *हे;
}

/**
 * Iteration method that returns the number of ranges contained in
 * this set.
 * @see #getRangeStart
 * @see #getRangeEnd
 */
int32_t UnicodeSet::get_Range_मोजणी() स्थिरांक {
    परत len/2;
}

/**
 * Iteration method that returns the first character in the
 * specified range of this set.
 * @see #getRangeCount
 * @see #getRangeEnd
 */
UChar32 UnicodeSet::getRangeStart(int32_t index) स्थिरांक {
    परत list[index*2];
}

/**
 * Iteration method that returns the last character in the
 * specified range of this set.
 * @see #getRangeStart
 * @see #getRangeEnd
 */
UChar32 UnicodeSet::getRangeEnd(int32_t index) स्थिरांक {
    परत list[index*2 + 1] - 1;
}

स्थिरांक UnicodeString* UnicodeSet::getString(int32_t index) स्थिरांक U_LIFETIME_BOUND {
    परत static_cast<स्थिरांक UnicodeString*>(strings_->elementAt(index));
}

/**
 * Reallocate this objects internal structures to take up the least
 * possible space, without changing this object's value.
 */
UnicodeSet& UnicodeSet::compact() U_LIFETIME_BOUND {
    जर (isFrozen() || isBogus()) {
        परत *हे;
    }
    // Delete buffer first to defragment memory less.
    जर (buffer != चळत_List) {
        uprv_free(buffer);
        buffer = nullptr;
        bufferCapacity = 0;
    }
    जर (list == चळत_List) {
        // pass
    } अन्यथा जर (len <= INITIAL_CAPACITY) {
        uprv_memcpy(चळत_List, list, len * आकार(UChar32));
        uprv_free(list);
        list = चळत_List;
        capacity = INITIAL_CAPACITY;
    } अन्यथा जर ((len + 7) < capacity) {
        // If we have more than a little unused capacity, shrink it to len.
        UChar32* temp = static_cast<UChar32*>(uprv_realloc(list, आकार(UChar32) * len));
        जर (temp) {
            list = temp;
            capacity = len;
        }
        // else what the heck happened?! We allocated less memory!
        // Oh well. We'll keep our original array.
    }
    जर (strings_ != nullptr && strings_->isEmpty()) {
        delete strings_;
        strings_ = nullptr;
    }
    परत *हे;
}

#ifdef DEBUG_SERIALIZE
#अन्तर्भाव <stdio.h>
#endif

/**
 * Deserialize constructor.
 */
UnicodeSet::UnicodeSet(स्थिरांक uint16_t data[], int32_t dataLen, ESerialization serialization,
                       UErrorCode &ec) {

  जर(U_FAILURE(ec)) {
    setToBogus();
    परत;
  }

  जर( (serialization != kSerialized)
      || (data==nullptr)
      || (dataLen < 1)) {
    ec = U_ILLEGAL_ARGUMENT_ERROR;
    setToBogus();
    परत;
  }

  // bmp?
  int32_t headerSize = ((data[0]&0x8000)) ?2:1;
  int32_t bmpLength = (headerSize==1)?data[0]:data[1];

  int32_t newLength = (((data[0]&0x7FFF)-bmpLength)/2)+bmpLength;
#ifdef DEBUG_SERIALIZE
  म_लिखो("dataLen %d headerSize %d bmpLen %d len %d. data[0]=%X/%X/%X/%X\n", dataLen,headerSize,bmpLength,newLength, data[0],data[1],data[2],data[3]);
#endif
  जर(!ensureCapacity(newLength + 1)) {  // +1 for HIGH
    परत;
  }
  // copy bmp
  int32_t i;
  साठी(i = 0; i< bmpLength;i++) {
    list[i] = data[i+headerSize];
#ifdef DEBUG_SERIALIZE
    म_लिखो("<<16@%d[%d] %X\n", i+headerSize, i, list[i]);
#endif
  }
  // copy smp
  साठी(i=bmpLength;i<newLength;i++) {
    list[i] = (static_cast<UChar32>(data[headerSize + bmpLength + (i - bmpLength) * 2 + 0]) << 16) +
               static_cast<UChar32>(data[headerSize + bmpLength + (i - bmpLength) * 2 + 1]);
#ifdef DEBUG_SERIALIZE
    म_लिखो("<<32@%d+[%d] %lX\n", headerSize+bmpLength+i, i, list[i]);
#endif
  }
  U_ASSERT(i == newLength);
  जर (i == 0 || list[i - 1] != UNICODESET_वरचा) {
    list[i++] = UNICODESET_वरचा;
  }
  len = i;
}


int32_t UnicodeSet::serialize(uint16_t *dest, int32_t destCapacity, UErrorCode& ec) स्थिरांक {
    int32_t bmpLength, length, destLength;

    जर (U_FAILURE(ec)) {
        परत 0;
    }

    जर (destCapacity<0 || (destCapacity>0 && dest==nullptr)) {
        ec=U_ILLEGAL_ARGUMENT_ERROR;
        परत 0;
    }

    /* count necessary 16-bit units */
    length=हे->len-1; // Subtract 1 to ignore final UNICODESET_HIGH
    // assert(length>=0);
    जर (length==0) {
        /* empty set */
        जर (destCapacity>0) {
            *dest=0;
        } अन्यथा {
            ec=U_BUFFER_OVERFLOW_ERROR;
        }
        परत 1;
    }
    /* now length>0 */

    जर (हे->list[length-1]<=0xffff) {
        /* all BMP */
        bmpLength=length;
    } अन्यथा जर (हे->list[0]>=0x10000) {
        /* all supplementary */
        bmpLength=0;
        length*=2;
    } अन्यथा {
        /* some BMP, some supplementary */
        साठी (bmpLength=0; bmpLength<length && हे->list[bmpLength]<=0xffff; ++bmpLength) {}
        length=bmpLength+2*(length-bmpLength);
    }
#ifdef DEBUG_SERIALIZE
    म_लिखो(">> bmpLength%d length%d len%d\n", bmpLength, length, len);
#endif
    /* length: number of 16-bit array units */
    जर (length>0x7fff) {
        /* there are only 15 bits for the length in the first serialized word */
        ec=U_INDEX_OUTOFBOUNDS_ERROR;
        परत 0;
    }

    /*
     * total serialized length:
     * number of 16-bit array units (length) +
     * 1 length unit (always) +
     * 1 bmpLength unit (if there are supplementary values)
     */
    destLength=length+((length>bmpLength)?2:1);
    जर (destLength<=destCapacity) {
        स्थिरांक UChar32 *p;
        int32_t i;

#ifdef DEBUG_SERIALIZE
        म_लिखो("writeHdr\n");
#endif
        *dest = static_cast<uint16_t>(length);
        जर (length>bmpLength) {
            *dest|=0x8000;
            *++dest = static_cast<uint16_t>(bmpLength);
        }
        ++dest;

        /* write the BMP part of the array */
        p=हे->list;
        साठी (i=0; i<bmpLength; ++i) {
#ifdef DEBUG_SERIALIZE
          म_लिखो("writebmp: %x\n", (पूर्णांक)*p);
#endif
            *dest++ = static_cast<uint16_t>(*p++);
        }

        /* write the supplementary part of the array */
        साठी (; i<length; i+=2) {
#ifdef DEBUG_SERIALIZE
          म_लिखो("write32: %x\n", (पूर्णांक)*p);
#endif
            *dest++ = static_cast<uint16_t>(*p >> 16);
            *dest++ = static_cast<uint16_t>(*p++);
        }
    } अन्यथा {
        ec=U_BUFFER_OVERFLOW_ERROR;
    }
    परत destLength;
}

//----------------------------------------------------------------
// Implementation: Utility methods
//----------------------------------------------------------------

/**
 * Allocate our strings vector and return true if successful.
 */
UBool UnicodeSet::allocateStrings(UErrorCode &status) {
    जर (U_FAILURE(status)) {
        परत असत्य;
    }
    strings_ = नवीन UVector(uprv_deleteUObject,
                          uhash_compareUnicodeString, 1, status);
    जर (strings_ == nullptr) { // Check for memory allocation error.
        status = U_MEMORY_ALLOCATION_ERROR;
        परत असत्य;
    }
    जर (U_FAILURE(status)) {
        delete strings_;
        strings_ = nullptr;
        परत असत्य;
    } 
    परत सत्य;
}

int32_t UnicodeSet::nextCapacity(int32_t minCapacity) {
    // Grow exponentially to reduce the frequency of allocations.
    जर (minCapacity < INITIAL_CAPACITY) {
        परत minCapacity + INITIAL_CAPACITY;
    } अन्यथा जर (minCapacity <= 2500) {
        परत 5 * minCapacity;
    } अन्यथा {
        int32_t newCapacity = 2 * minCapacity;
        जर (newCapacity > MAX_LENGTH) {
            newCapacity = MAX_LENGTH;
        }
        परत newCapacity;
    }
}

तार्किक UnicodeSet::ensureCapacity(int32_t newLen) {
    जर (newLen > MAX_LENGTH) {
        newLen = MAX_LENGTH;
    }
    जर (newLen <= capacity) {
        परत सत्य;
    }
    int32_t newCapacity = nextCapacity(newLen);
    UChar32* temp = static_cast<UChar32*>(uprv_malloc(newCapacity * आकार(UChar32)));
    जर (temp == nullptr) {
        setToBogus(); // set the object to bogus state if an OOM failure occurred.
        परत असत्य;
    }
    // Copy only the actual contents.
    uprv_memcpy(temp, list, len * आकार(UChar32));
    जर (list != चळत_List) {
        uprv_free(list);
    }
    list = temp;
    capacity = newCapacity;
    परत सत्य;
}

तार्किक UnicodeSet::ensureBufferCapacity(int32_t newLen) {
    जर (newLen > MAX_LENGTH) {
        newLen = MAX_LENGTH;
    }
    जर (newLen <= bufferCapacity) {
        परत सत्य;
    }
    int32_t newCapacity = nextCapacity(newLen);
    UChar32* temp = static_cast<UChar32*>(uprv_malloc(newCapacity * आकार(UChar32)));
    जर (temp == nullptr) {
        setToBogus();
        परत असत्य;
    }
    // The buffer has no contents to be copied.
    // It is always filled from scratch after this call.
    जर (buffer != चळत_List) {
        uprv_free(buffer);
    }
    buffer = temp;
    bufferCapacity = newCapacity;
    परत सत्य;
}

/**
 * Swap list and buffer.
 */
शून्य UnicodeSet::swapBuffers() {
    // swap list and buffer
    UChar32* temp = list;
    list = buffer;
    buffer = temp;

    int32_t c = capacity;
    capacity = bufferCapacity;
    bufferCapacity = c;
}

शून्य UnicodeSet::setToBogus() {
    clear(); // Remove everything in the set.
    fFlags = kIsBogus;
}

//----------------------------------------------------------------
// Implementation: Fundamental operators
//----------------------------------------------------------------

स्थिर ओळीत UChar32 max(UChar32 a, UChar32 b) {
    परत (a > b) ? a : b;
}

// polarity = 0, 3 is normal: x xor y
// polarity = 1, 2: x xor ~y == x === y

शून्य UnicodeSet::exclusiveOr(स्थिरांक UChar32* other, int32_t otherLen, int8_t polarity) {
    जर (isFrozen() || isBogus()) {
        परत;
    }
    जर (!ensureBufferCapacity(len + otherLen)) {
        परत;
    }

    int32_t i = 0, j = 0, k = 0;
    UChar32 a = list[i++];
    UChar32 b;
    जर (polarity == 1 || polarity == 2) {
        b = UNICODESET_खालचा;
        जर (other[j] == UNICODESET_खालचा) { // skip base if already LOW
            ++j;
            b = other[j];
        }
    } अन्यथा {
        b = other[j++];
    }
    // simplest of all the routines
    // sort the values, discarding identicals!
    साठी (;;) {
        जर (a < b) {
            buffer[k++] = a;
            a = list[i++];
        } अन्यथा जर (b < a) {
            buffer[k++] = b;
            b = other[j++];
        } अन्यथा जर (a != UNICODESET_वरचा) { // at this point, a == b
            // discard both values!
            a = list[i++];
            b = other[j++];
        } अन्यथा { // DONE!
            buffer[k++] = UNICODESET_वरचा;
            len = k;
            थांबा;
        }
    }
    swapBuffers();
    releasePattern();
}

// polarity = 0 is normal: x union y
// polarity = 2: x union ~y
// polarity = 1: ~x union y
// polarity = 3: ~x union ~y

शून्य UnicodeSet::add(स्थिरांक UChar32* other, int32_t otherLen, int8_t polarity) {
    जर (isFrozen() || isBogus() || other==nullptr) {
        परत;
    }
    जर (!ensureBufferCapacity(len + otherLen)) {
        परत;
    }

    int32_t i = 0, j = 0, k = 0;
    UChar32 a = list[i++];
    UChar32 b = other[j++];
    // change from xor is that we have to check overlapping pairs
    // polarity bit 1 means a is second, bit 2 means b is.
    साठी (;;) {
        निवड (polarity) {
          स्थिती 0: // both first; take lower if unequal
            जर (a < b) { // take a
                // Back up over overlapping ranges in buffer[]
                जर (k > 0 && a <= buffer[k-1]) {
                    // Pick latter end value in buffer[] vs. list[]
                    a = max(list[i], buffer[--k]);
                } अन्यथा {
                    // No overlap
                    buffer[k++] = a;
                    a = list[i];
                }
                i++; // Common if/else code factored out
                polarity ^= 1;
            } अन्यथा जर (b < a) { // take b
                जर (k > 0 && b <= buffer[k-1]) {
                    b = max(other[j], buffer[--k]);
                } अन्यथा {
                    buffer[k++] = b;
                    b = other[j];
                }
                j++;
                polarity ^= 2;
            } अन्यथा { // a == b, take a, drop b
                जर (a == UNICODESET_वरचा) जा loop_end;
                // This is symmetrical; it doesn't matter if
                // we backtrack with a or b. - liu
                जर (k > 0 && a <= buffer[k-1]) {
                    a = max(list[i], buffer[--k]);
                } अन्यथा {
                    // No overlap
                    buffer[k++] = a;
                    a = list[i];
                }
                i++;
                polarity ^= 1;
                b = other[j++];
                polarity ^= 2;
            }
            थांबा;
          स्थिती 3: // both second; take higher if unequal, and drop other
            जर (b <= a) { // take a
                जर (a == UNICODESET_वरचा) जा loop_end;
                buffer[k++] = a;
            } अन्यथा { // take b
                जर (b == UNICODESET_वरचा) जा loop_end;
                buffer[k++] = b;
            }
            a = list[i++];
            polarity ^= 1;   // factored common code
            b = other[j++];
            polarity ^= 2;
            थांबा;
          स्थिती 1: // a second, b first; if b < a, overlap
            जर (a < b) { // no overlap, take a
                buffer[k++] = a; a = list[i++]; polarity ^= 1;
            } अन्यथा जर (b < a) { // OVERLAP, drop b
                b = other[j++];
                polarity ^= 2;
            } अन्यथा { // a == b, drop both!
                जर (a == UNICODESET_वरचा) जा loop_end;
                a = list[i++];
                polarity ^= 1;
                b = other[j++];
                polarity ^= 2;
            }
            थांबा;
          स्थिती 2: // a first, b second; if a < b, overlap
            जर (b < a) { // no overlap, take b
                buffer[k++] = b;
                b = other[j++];
                polarity ^= 2;
            } अन्यथा  जर (a < b) { // OVERLAP, drop a
                a = list[i++];
                polarity ^= 1;
            } अन्यथा { // a == b, drop both!
                जर (a == UNICODESET_वरचा) जा loop_end;
                a = list[i++];
                polarity ^= 1;
                b = other[j++];
                polarity ^= 2;
            }
            थांबा;
        }
    }
 loop_end:
    buffer[k++] = UNICODESET_वरचा;    // terminate
    len = k;
    swapBuffers();
    releasePattern();
}

// polarity = 0 is normal: x intersect y
// polarity = 2: x intersect ~y == set-minus
// polarity = 1: ~x intersect y
// polarity = 3: ~x intersect ~y

शून्य UnicodeSet::retain(स्थिरांक UChar32* other, int32_t otherLen, int8_t polarity) {
    जर (isFrozen() || isBogus()) {
        परत;
    }
    जर (!ensureBufferCapacity(len + otherLen)) {
        परत;
    }

    int32_t i = 0, j = 0, k = 0;
    UChar32 a = list[i++];
    UChar32 b = other[j++];
    // change from xor is that we have to check overlapping pairs
    // polarity bit 1 means a is second, bit 2 means b is.
    साठी (;;) {
        निवड (polarity) {
          स्थिती 0: // both first; drop the smaller
            जर (a < b) { // drop a
                a = list[i++];
                polarity ^= 1;
            } अन्यथा जर (b < a) { // drop b
                b = other[j++];
                polarity ^= 2;
            } अन्यथा { // a == b, take one, drop other
                जर (a == UNICODESET_वरचा) जा loop_end;
                buffer[k++] = a;
                a = list[i++];
                polarity ^= 1;
                b = other[j++];
                polarity ^= 2;
            }
            थांबा;
          स्थिती 3: // both second; take lower if unequal
            जर (a < b) { // take a
                buffer[k++] = a;
                a = list[i++];
                polarity ^= 1;
            } अन्यथा जर (b < a) { // take b
                buffer[k++] = b;
                b = other[j++];
                polarity ^= 2;
            } अन्यथा { // a == b, take one, drop other
                जर (a == UNICODESET_वरचा) जा loop_end;
                buffer[k++] = a;
                a = list[i++];
                polarity ^= 1;
                b = other[j++];
                polarity ^= 2;
            }
            थांबा;
          स्थिती 1: // a second, b first;
            जर (a < b) { // NO OVERLAP, drop a
                a = list[i++];
                polarity ^= 1;
            } अन्यथा जर (b < a) { // OVERLAP, take b
                buffer[k++] = b;
                b = other[j++];
                polarity ^= 2;
            } अन्यथा { // a == b, drop both!
                जर (a == UNICODESET_वरचा) जा loop_end;
                a = list[i++];
                polarity ^= 1;
                b = other[j++];
                polarity ^= 2;
            }
            थांबा;
          स्थिती 2: // a first, b second; if a < b, overlap
            जर (b < a) { // no overlap, drop b
                b = other[j++];
                polarity ^= 2;
            } अन्यथा  जर (a < b) { // OVERLAP, take a
                buffer[k++] = a;
                a = list[i++];
                polarity ^= 1;
            } अन्यथा { // a == b, drop both!
                जर (a == UNICODESET_वरचा) जा loop_end;
                a = list[i++];
                polarity ^= 1;
                b = other[j++];
                polarity ^= 2;
            }
            थांबा;
        }
    }
 loop_end:
    buffer[k++] = UNICODESET_वरचा;    // terminate
    len = k;
    swapBuffers();
    releasePattern();
}

/**
 * Append the <code>toPattern()</code> representation of a
 * string to the given <code>StringBuffer</code>.
 */
शून्य UnicodeSet::_appendToPat(UnicodeString& buf, स्थिरांक UnicodeString& s, UBool escapeUnprintable) {
    UChar32 cp;
    साठी (int32_t i = 0; i < s.length(); i += U16_LENGTH(cp)) {
        _appendToPat(buf, cp = s.char32At(i), escapeUnprintable);
    }
}

/**
 * Append the <code>toPattern()</code> representation of a
 * character to the given <code>StringBuffer</code>.
 */
शून्य UnicodeSet::_appendToPat(UnicodeString& buf, UChar32 c, UBool escapeUnprintable) {
    जर (escapeUnprintable ? ICU_Utility::isUnprintable(c) : ICU_Utility::shouldAlwaysBeEscaped(c)) {
        // Use hex escape notation (\uxxxx or \Uxxxxxxxx) for anything
        // unprintable
        ICU_Utility::escape(buf, c);
        परत;
    }
    // Okay to let ':' pass through
    निवड (c) {
    स्थिती u'[':
    स्थिती u']':
    स्थिती u'-':
    स्थिती u'^':
    स्थिती u'&':
    स्थिती u'\\':
    स्थिती u'{':
    स्थिती u'}':
    स्थिती u':':
    स्थिती चिन्ह_Table::चिन्ह_REF:
        buf.append(u'\\');
        थांबा;
    पूर्वनिर्धारित:
        // Escape whitespace
        जर (PatternProps::isWhiteSpace(c)) {
            buf.append(u'\\');
        }
        थांबा;
    }
    buf.append(c);
}

शून्य UnicodeSet::_appendToPat(UnicodeString &result, UChar32 start, UChar32 end,
                              UBool escapeUnprintable) {
    _appendToPat(result, start, escapeUnprintable);
    जर (start != end) {
        जर ((start+1) != end ||
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
                                      UBool escapeUnprintable) स्थिरांक
{
    जर (pat != nullptr) {
        int32_t i;
        int32_t backslash_मोजणी = 0;
        साठी (i=0; i<patLen; ) {
            UChar32 c;
            U16_NEXT(pat, i, patLen, c);
            जर (escapeUnprintable ?
                    ICU_Utility::isUnprintable(c) : ICU_Utility::shouldAlwaysBeEscaped(c)) {
                // If the unprintable character is preceded by an odd
                // number of backslashes, then it has been escaped.
                // Before unescaping it, we delete the final
                // backslash.
                जर ((backslash_मोजणी % 2) == 1) {
                    result.truncate(result.length() - 1);
                }
                ICU_Utility::escape(result, c);
                backslash_मोजणी = 0;
            } अन्यथा {
                result.append(c);
                जर (c == u'\\') {
                    ++backslash_मोजणी;
                } अन्यथा {
                    backslash_मोजणी = 0;
                }
            }
        }
        परत result;
    }

    परत _generatePattern(result, escapeUnprintable);
}

/**
 * Returns a string representation of this set.  If the result of
 * calling this function is passed to a UnicodeSet constructor, it
 * will produce another set that is equal to this one.
 */
UnicodeString& UnicodeSet::toPattern(UnicodeString& result U_LIFETIME_BOUND,
                                     UBool escapeUnprintable) स्थिरांक
{
    result.truncate(0);
    परत _toPattern(result, escapeUnprintable);
}

/**
 * Generate and append a string representation of this set to result.
 * This does not use this.pat, the cleaned up copy of the string
 * passed to applyPattern().
 */
UnicodeString& UnicodeSet::_generatePattern(UnicodeString& result U_LIFETIME_BOUND,
                                            UBool escapeUnprintable) स्थिरांक
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
    जर (len >= 4 && list[0] == 0 && limit == len && !hasStrings()) {
        // Emit the inverse
        result.append(u'^');
        // Offsetting the inversion list index by one lets us
        // iterate over the ranges of the set complement.
        i = 1;
        --limit;
    }

    // Emit the ranges as pairs.
    पर्यंत (i < limit) {
        UChar32 start = list[i];  // getRangeStart()
        UChar32 end = list[i + 1] - 1;  // getRangeEnd() = range limit minus one
        जर (!(0xd800 <= end && end <= 0xdbff)) {
            _appendToPat(result, start, end, escapeUnprintable);
            i += 2;
        } अन्यथा {
            // The range ends with a lead surrogate.
            // Avoid writing what looks like a lead+trail surrogate pair.
            // 1. Postpone ranges that start with a lead surrogate code point.
            int32_t firstLead = i;
            पर्यंत ((i += 2) < limit && list[i] <= 0xdbff) {}
            int32_t firstAfterLead = i;
            // 2. Write following ranges that start with a trail surrogate code point.
            पर्यंत (i < limit && (start = list[i]) <= 0xdfff) {
                _appendToPat(result, start, list[i + 1] - 1, escapeUnprintable);
                i += 2;
            }
            // 3. Now write the postponed ranges.
            साठी (पूर्णांक j = firstLead; j < firstAfterLead; j += 2) {
                _appendToPat(result, list[j], list[j + 1] - 1, escapeUnprintable);
            }
        }
    }

    जर (strings_ != nullptr) {
        साठी (int32_t i = 0; i<strings_->size(); ++i) {
            result.append(u'{');
            _appendToPat(result,
                         *static_cast<स्थिरांक UnicodeString*>(strings_->elementAt(i)),
                         escapeUnprintable);
            result.append(u'}');
        }
    }
    परत result.append(u']');
}

/**
* Release existing cached pattern
*/
शून्य UnicodeSet::releasePattern() {
    जर (pat) {
        uprv_free(pat);
        pat = nullptr;
        patLen = 0;
    }
}

/**
* Set the new pattern to cache.
*/
शून्य UnicodeSet::setPattern(स्थिरांक char16_t *newPat, int32_t newPatLen) {
    releasePattern();
    pat = static_cast<char16_t*>(uprv_malloc((newPatLen + 1) * आकार(char16_t)));
    जर (pat) {
        patLen = newPatLen;
        u_memcpy(pat, newPat, patLen);
        pat[patLen] = 0;
    }
    // else we don't care if malloc failed. This was just a nice cache.
    // We can regenerate an equivalent pattern later when requested.
}

UnicodeSet* UnicodeSet::freeze() U_LIFETIME_BOUND {
    जर(!isFrozen() && !isBogus()) {
        compact();

        // Optimize contains() and span() and similar functions.
        जर (hasStrings()) {
            stringSpan = नवीन UnicodeSetStringSpan(*हे, *strings_, UnicodeSetStringSpan::ALL);
            जर (stringSpan == nullptr) {
                setToBogus();
                परत हे;
            } अन्यथा जर (!stringSpan->needsStringSpanUTF16()) {
                // All strings are irrelevant for span() etc. because
                // all of each string's code points are contained in this set.
                // Do not check needsStringSpanUTF8() because UTF-8 has at most as
                // many relevant strings as UTF-16.
                // (Thus needsStringSpanUTF8() implies needsStringSpanUTF16().)
                delete stringSpan;
                stringSpan = nullptr;
            }
        }
        जर (stringSpan == nullptr) {
            // No span-relevant strings: Optimize for code point spans.
            bmpSet=नवीन BMPSet(list, len);
            जर (bmpSet == nullptr) { // Check for memory allocation error.
                setToBogus();
            }
        }
    }
    परत हे;
}

int32_t UnicodeSet::span(स्थिरांक char16_t *s, int32_t length, USetSpanCondition spanCondition) स्थिरांक {
    जर(length>0 && bmpSet!=nullptr) {
        परत static_cast<int32_t>(bmpSet->span(s, s + length, spanCondition) - s);
    }
    जर(length<0) {
        length=u_strlen(s);
    }
    जर(length==0) {
        परत 0;
    }
    जर(stringSpan!=nullptr) {
        परत stringSpan->span(s, length, spanCondition);
    } अन्यथा जर(hasStrings()) {
        uint32_t which= spanCondition==USET_SPAN_NOT_CONTAINED ?
                            UnicodeSetStringSpan::FWD_UTF16_NOT_CONTAINED :
                            UnicodeSetStringSpan::FWD_UTF16_CONTAINED;
        UnicodeSetStringSpan strSpan(*हे, *strings_, which);
        जर(strSpan.needsStringSpanUTF16()) {
            परत strSpan.span(s, length, spanCondition);
        }
    }

    जर(spanCondition!=USET_SPAN_NOT_CONTAINED) {
        spanCondition=USET_SPAN_CONTAINED;  // Pin to 0/1 values.
    }

    UChar32 c;
    int32_t start=0, prev=0;
    करा {
        U16_NEXT(s, start, length, c);
        जर(spanCondition!=contains(c)) {
            थांबा;
        }
    } पर्यंत((prev=start)<length);
    परत prev;
}

int32_t UnicodeSet::spanBack(स्थिरांक char16_t *s, int32_t length, USetSpanCondition spanCondition) स्थिरांक {
    जर(length>0 && bmpSet!=nullptr) {
        परत static_cast<int32_t>(bmpSet->spanBack(s, s + length, spanCondition) - s);
    }
    जर(length<0) {
        length=u_strlen(s);
    }
    जर(length==0) {
        परत 0;
    }
    जर(stringSpan!=nullptr) {
        परत stringSpan->spanBack(s, length, spanCondition);
    } अन्यथा जर(hasStrings()) {
        uint32_t which= spanCondition==USET_SPAN_NOT_CONTAINED ?
                            UnicodeSetStringSpan::BACK_UTF16_NOT_CONTAINED :
                            UnicodeSetStringSpan::BACK_UTF16_CONTAINED;
        UnicodeSetStringSpan strSpan(*हे, *strings_, which);
        जर(strSpan.needsStringSpanUTF16()) {
            परत strSpan.spanBack(s, length, spanCondition);
        }
    }

    जर(spanCondition!=USET_SPAN_NOT_CONTAINED) {
        spanCondition=USET_SPAN_CONTAINED;  // Pin to 0/1 values.
    }

    UChar32 c;
    int32_t prev=length;
    करा {
        U16_PREV(s, 0, length, c);
        जर(spanCondition!=contains(c)) {
            थांबा;
        }
    } पर्यंत((prev=length)>0);
    परत prev;
}

int32_t UnicodeSet::spanUTF8(स्थिरांक अक्षर *s, int32_t length, USetSpanCondition spanCondition) स्थिरांक {
    जर(length>0 && bmpSet!=nullptr) {
        स्थिरांक uint8_t* s0 = reinterpret_cast<स्थिरांक uint8_t*>(s);
        परत static_cast<int32_t>(bmpSet->spanUTF8(s0, length, spanCondition) - s0);
    }
    जर(length<0) {
        length = static_cast<int32_t>(uprv_strlen(s));
    }
    जर(length==0) {
        परत 0;
    }
    जर(stringSpan!=nullptr) {
        परत stringSpan->spanUTF8(reinterpret_cast<स्थिरांक uint8_t*>(s), length, spanCondition);
    } अन्यथा जर(hasStrings()) {
        uint32_t which= spanCondition==USET_SPAN_NOT_CONTAINED ?
                            UnicodeSetStringSpan::FWD_UTF8_NOT_CONTAINED :
                            UnicodeSetStringSpan::FWD_UTF8_CONTAINED;
        UnicodeSetStringSpan strSpan(*हे, *strings_, which);
        जर(strSpan.needsStringSpanUTF8()) {
            परत strSpan.spanUTF8(reinterpret_cast<स्थिरांक uint8_t*>(s), length, spanCondition);
        }
    }

    जर(spanCondition!=USET_SPAN_NOT_CONTAINED) {
        spanCondition=USET_SPAN_CONTAINED;  // Pin to 0/1 values.
    }

    UChar32 c;
    int32_t start=0, prev=0;
    करा {
        U8_NEXT_OR_FFFD(s, start, length, c);
        जर(spanCondition!=contains(c)) {
            थांबा;
        }
    } पर्यंत((prev=start)<length);
    परत prev;
}

int32_t UnicodeSet::spanBackUTF8(स्थिरांक अक्षर *s, int32_t length, USetSpanCondition spanCondition) स्थिरांक {
    जर(length>0 && bmpSet!=nullptr) {
        स्थिरांक uint8_t* s0 = reinterpret_cast<स्थिरांक uint8_t*>(s);
        परत bmpSet->spanBackUTF8(s0, length, spanCondition);
    }
    जर(length<0) {
        length = static_cast<int32_t>(uprv_strlen(s));
    }
    जर(length==0) {
        परत 0;
    }
    जर(stringSpan!=nullptr) {
        परत stringSpan->spanBackUTF8(reinterpret_cast<स्थिरांक uint8_t*>(s), length, spanCondition);
    } अन्यथा जर(hasStrings()) {
        uint32_t which= spanCondition==USET_SPAN_NOT_CONTAINED ?
                            UnicodeSetStringSpan::BACK_UTF8_NOT_CONTAINED :
                            UnicodeSetStringSpan::BACK_UTF8_CONTAINED;
        UnicodeSetStringSpan strSpan(*हे, *strings_, which);
        जर(strSpan.needsStringSpanUTF8()) {
            परत strSpan.spanBackUTF8(reinterpret_cast<स्थिरांक uint8_t*>(s), length, spanCondition);
        }
    }

    जर(spanCondition!=USET_SPAN_NOT_CONTAINED) {
        spanCondition=USET_SPAN_CONTAINED;  // Pin to 0/1 values.
    }

    UChar32 c;
    int32_t prev=length;
    करा {
        U8_PREV_OR_FFFD(s, 0, length, c);
        जर(spanCondition!=contains(c)) {
            थांबा;
        }
    } पर्यंत((prev=length)>0);
    परत prev;
}

U_NAMESPACE_END
