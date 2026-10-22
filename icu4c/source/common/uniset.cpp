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
#define UNICODESET_माथिल्लो 0x0110000

// LOW <= all valid values. ZERO for codepoints
#define UNICODESET_तल्लो 0x000000

/** Max list [0, 1, 2, ..., max code point, HIGH] */
constexpr int32_t MAX_LENGTH = UNICODESET_माथिल्लो + 1;

U_NAMESPACE_BEGIN

चिह्न_Table::~चिह्न_Table() {}

UOBJECT_DEFINE_RTTI_IMPLEMENTATION(UnicodeSet)

/**
 * Modify the given UChar32 variable so that it is in range, by
 * pinning values < UNICODESET_LOW to UNICODESET_LOW, and
 * pinning values > UNICODESET_HIGH-1 to UNICODESET_HIGH-1.
 * It modifies its argument in-place and also returns it.
 */
स्थिर अन्तःस्थ UChar32 pinCodePoint(UChar32& c) {
    यदि (c < UNICODESET_तल्लो) {
        c = UNICODESET_तल्लो;
    } अन्यथा यदि (c > (UNICODESET_माथिल्लो-1)) {
        c = (UNICODESET_माथिल्लो-1);
    }
    फर्काऊ c;
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
स्थिर int32_t dbg_गन्ती = 0;

स्थिर अन्तःस्थ शून्य _dbgct(UnicodeSet* set) {
    UnicodeString str;
    set->toPattern(str, सत्य);
    वर्ण buf[40];
    str.extract(0, 39, buf, "");
    म_लिखो("DEBUG UnicodeSet: ct 0x%08X; %d %s\n", set, ++dbg_गन्ती, buf);
}

स्थिर अन्तःस्थ शून्य _dbgdt(UnicodeSet* set) {
    UnicodeString str;
    set->toPattern(str, सत्य);
    वर्ण buf[40];
    str.extract(0, 39, buf, "");
    म_लिखो("DEBUG UnicodeSet: dt 0x%08X; %d %s\n", set, --dbg_गन्ती, buf);
}

#अन्यथा

#define _dbgct(set)
#define _dbgdt(set)

#endif

//----------------------------------------------------------------
// UnicodeString in UVector support
//----------------------------------------------------------------

स्थिर शून्य U_CALLCONV cloneUnicodeString(UElement *dst, UElement *src) {
    dst->pointer = नूतन UnicodeString(*static_cast<UnicodeString*>(src->pointer));
}

स्थिर int32_t U_CALLCONV compareUnicodeString(UElement t1, UElement t2) {
    अचर UnicodeString& a = *static_cast<अचर UnicodeString*>(t1.pointer);
    अचर UnicodeString& b = *static_cast<अचर UnicodeString*>(t2.pointer);
    फर्काऊ a.compare(b);
}

UBool UnicodeSet::hasStrings() अचर {
    फर्काऊ strings_ != nullptr && !strings_->isEmpty();
}

int32_t UnicodeSet::stringsSize() अचर {
    फर्काऊ strings_ == nullptr ? 0 : strings_->size();
}

UBool UnicodeSet::stringsContains(अचर UnicodeString &s) अचर {
    फर्काऊ strings_ != nullptr && strings_->contains((शून्य*) &s);
}

//----------------------------------------------------------------
// Constructors &c
//----------------------------------------------------------------

/**
 * Constructs an empty set.
 */
UnicodeSet::UnicodeSet() {
    list[0] = UNICODESET_माथिल्लो;
    _dbgct(एतत्);
}

/**
 * Constructs a set containing the given range. If <code>end >
 * start</code> then an empty set is created.
 *
 * @param start first character, inclusive, of range
 * @param end last character, inclusive, of range
 */
UnicodeSet::UnicodeSet(UChar32 start, UChar32 end) {
    list[0] = UNICODESET_माथिल्लो;
    add(start, end);
    _dbgct(एतत्);
}

/**
 * Constructs a set that is identical to the given UnicodeSet.
 */
UnicodeSet::UnicodeSet(अचर UnicodeSet& o) : UnicodeFilter(o) {
    *एतत् = o;
    _dbgct(एतत्);
}

// Copy-construct as thawed.
UnicodeSet::UnicodeSet(अचर UnicodeSet& o, UBool /* asThawed */) : UnicodeFilter(o) {
    यदि (ensureCapacity(o.len)) {
        // *this = o except for bmpSet and stringSpan
        len = o.len;
        uprv_memcpy(list, o.list, (माप_प्रकार)len*परिमाण(UChar32));
        यदि (o.hasStrings()) {
            UErrorCode status = U_ZERO_ERROR;
            यदि (!allocateStrings(status) ||
                    (strings_->assign(*o.strings_, cloneUnicodeString, status), U_FAILURE(status))) {
                setToBogus();
                फर्काऊ;
            }
        }
        यदि (o.pat) {
            setPattern(o.pat, o.patLen);
        }
        _dbgct(एतत्);
    }
}

/**
 * Destructs the set.
 */
UnicodeSet::~UnicodeSet() {
    _dbgdt(एतत्); // first!
    यदि (list != थाक_List) {
        uprv_free(list);
    }
    delete bmpSet;
    यदि (buffer != थाक_List) {
        uprv_free(buffer);
    }
    delete strings_;
    delete stringSpan;
    releasePattern();
}

/**
 * Assigns this object to be a copy of another.
 */
UnicodeSet& UnicodeSet::चालक=(अचर UnicodeSet& o) U_LIFETIME_BOUND {
    फर्काऊ copyFrom(o, असत्य);
}

UnicodeSet& UnicodeSet::copyFrom(अचर UnicodeSet& o, UBool asThawed) U_LIFETIME_BOUND {
    यदि (एतत् == &o) {
        फर्काऊ *एतत्;
    }
    यदि (isFrozen()) {
        फर्काऊ *एतत्;
    }
    यदि (o.isBogus()) {
        setToBogus();
        फर्काऊ *एतत्;
    }
    यदि (!ensureCapacity(o.len)) {
        // ensureCapacity will mark the UnicodeSet as Bogus if OOM failure happens.
        फर्काऊ *एतत्;
    }
    len = o.len;
    uprv_memcpy(list, o.list, (माप_प्रकार)len*परिमाण(UChar32));
    यदि (o.bmpSet != nullptr && !asThawed) {
        bmpSet = नूतन BMPSet(*o.bmpSet, list, len);
        यदि (bmpSet == nullptr) { // Check for memory allocation error.
            setToBogus();
            फर्काऊ *एतत्;
        }
    }
    यदि (o.hasStrings()) {
        UErrorCode status = U_ZERO_ERROR;
        यदि ((strings_ == nullptr && !allocateStrings(status)) ||
                (strings_->assign(*o.strings_, cloneUnicodeString, status), U_FAILURE(status))) {
            setToBogus();
            फर्काऊ *एतत्;
        }
    } अन्यथा यदि (hasStrings()) {
        strings_->removeAllElements();
    }
    यदि (o.stringSpan != nullptr && strings_ != nullptr && !asThawed) {
        stringSpan = नूतन UnicodeSetStringSpan(*o.stringSpan, *strings_);
        यदि (stringSpan == nullptr) { // Check for memory allocation error.
            setToBogus();
            फर्काऊ *एतत्;
        }
    }
    releasePattern();
    यदि (o.pat) {
        setPattern(o.pat, o.patLen);
    }
    फर्काऊ *एतत्;
}

/**
 * Returns a copy of this object.  All UnicodeMatcher objects have
 * to support cloning in order to allow classes using
 * UnicodeMatchers, such as Transliterator, to implement cloning.
 */
UnicodeSet* UnicodeSet::clone() अचर {
    फर्काऊ नूतन UnicodeSet(*एतत्);
}

UnicodeSet *UnicodeSet::cloneAsThawed() अचर {
    फर्काऊ नूतन UnicodeSet(*एतत्, सत्य);
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
तार्किक UnicodeSet::चालक==(अचर UnicodeSet& o) अचर {
    यदि (len != o.len) फर्काऊ असत्य;
    लागि (int32_t i = 0; i < len; ++i) {
        यदि (list[i] != o.list[i]) फर्काऊ असत्य;
    }
    यदि (hasStrings() != o.hasStrings()) { फर्काऊ असत्य; }
    यदि (hasStrings() && *strings_ != *o.strings_) फर्काऊ असत्य;
    फर्काऊ सत्य;
}

/**
 * Returns the hash code value for this set.
 *
 * @return the hash code value for this set.
 * @see Object#hashCode()
 */
int32_t UnicodeSet::hashCode() अचर {
    uint32_t result = static_cast<uint32_t>(len);
    लागि (int32_t i = 0; i < len; ++i) {
        result *= 1000003u;
        result += list[i];
    }
    फर्काऊ static_cast<int32_t>(result);
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
int32_t UnicodeSet::size() अचर {
    int32_t n = 0;
    int32_t count = get_Range_गन्ती();
    लागि (int32_t i = 0; i < count; ++i) {
        n += getRangeEnd(i) - getRangeStart(i) + 1;
    }
    फर्काऊ n + stringsSize();
}

/**
 * Returns <tt>true</tt> if this set contains no elements.
 *
 * @return <tt>true</tt> if this set contains no elements.
 */
UBool UnicodeSet::isEmpty() अचर {
    फर्काऊ len == 1 && !hasStrings();
}

/**
 * Returns true if this set contains the given character.
 * @param c character to be checked for containment
 * @return true if the test condition is met
 */
UBool UnicodeSet::contains(UChar32 c) अचर {
    // Set i to the index of the start item greater than ch
    // We know we will terminate without length test!
    // LATER: for large sets, add binary search
    //int32_t i = -1;
    //for (;;) {
    //    if (c < list[++i]) break;
    //}
    यदि (bmpSet != nullptr) {
        फर्काऊ bmpSet->contains(c);
    }
    यदि (stringSpan != nullptr) {
        फर्काऊ stringSpan->contains(c);
    }
    यदि (c >= UNICODESET_माथिल्लो) { // Don't need to check LOW bound
        फर्काऊ असत्य;
    }
    int32_t i = findCodePoint(c);
    फर्काऊ i & 1; // return true if odd
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
int32_t UnicodeSet::findCodePoint(UChar32 c) अचर {
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
    यदि (c < list[0])
        फर्काऊ 0;
    // High runner test.  c is often after the last range, so an
    // initial check for this condition pays off.
    int32_t lo = 0;
    int32_t hi = len - 1;
    यदि (lo >= hi || c >= list[hi-1])
        फर्काऊ hi;
    // invariant: c >= list[lo]
    // invariant: c < list[hi]
    लागि (;;) {
        int32_t i = (lo + hi) >> 1;
        यदि (i == lo) {
            रोक; // Found!
        } अन्यथा यदि (c < list[i]) {
            hi = i;
        } अन्यथा {
            lo = i;
        }
    }
    फर्काऊ hi;
}

/**
 * Returns true if this set contains every character
 * of the given range.
 * @param start first character, inclusive, of the range
 * @param end last character, inclusive, of the range
 * @return true if the test condition is met
 */
UBool UnicodeSet::contains(UChar32 start, UChar32 end) अचर {
    //int32_t i = -1;
    //for (;;) {
    //    if (start < list[++i]) break;
    //}
    int32_t i = findCodePoint(start);
    फर्काऊ ((i & 1) != 0 && end < list[i]);
}

/**
 * Returns <tt>true</tt> if this set contains the given
 * multicharacter string.
 * @param s string to be checked for containment
 * @return <tt>true</tt> if this set contains the specified string
 */
UBool UnicodeSet::contains(अचर UnicodeString& s) अचर {
    int32_t cp = getSingleCP(s);
    यदि (cp < 0) {
        फर्काऊ stringsContains(s);
    } अन्यथा {
        फर्काऊ contains(static_cast<UChar32>(cp));
    }
}

/**
 * Returns true if this set contains all the characters and strings
 * of the given set.
 * @param c set to be checked for containment
 * @return true if the test condition is met
 */
UBool UnicodeSet::containsAll(अचर UnicodeSet& c) अचर {
    // The specified set is a subset if all of its pairs are contained in
    // this set.  It's possible to code this more efficiently in terms of
    // direct manipulation of the inversion lists if the need arises.
    int32_t n = c.get_Range_गन्ती();
    लागि (पूर्णाङ्क i=0; i<n; ++i) {
        यदि (!contains(c.getRangeStart(i), c.getRangeEnd(i))) {
            फर्काऊ असत्य;
        }
    }
    फर्काऊ !c.hasStrings() || (strings_ != nullptr && strings_->containsAll(*c.strings_));
}

/**
 * Returns true if this set contains all the characters
 * of the given string.
 * @param s string containing characters to be checked for containment
 * @return true if the test condition is met
 */
UBool UnicodeSet::containsAll(अचर UnicodeString& s) अचर {
    फर्काऊ span(s.getBuffer(), s.length(), USET_SPAN_CONTAINED) == s.length();
}

/**
 * Returns true if this set contains none of the characters
 * of the given range.
 * @param start first character, inclusive, of the range
 * @param end last character, inclusive, of the range
 * @return true if the test condition is met
 */
UBool UnicodeSet::containsNone(UChar32 start, UChar32 end) अचर {
    //int32_t i = -1;
    //for (;;) {
    //    if (start < list[++i]) break;
    //}
    int32_t i = findCodePoint(start);
    फर्काऊ ((i & 1) == 0 && end < list[i]);
}

/**
 * Returns true if this set contains none of the characters and strings
 * of the given set.
 * @param c set to be checked for containment
 * @return true if the test condition is met
 */
UBool UnicodeSet::containsNone(अचर UnicodeSet& c) अचर {
    // The specified set is a subset if all of its pairs are contained in
    // this set.  It's possible to code this more efficiently in terms of
    // direct manipulation of the inversion lists if the need arises.
    int32_t n = c.get_Range_गन्ती();
    लागि (int32_t i=0; i<n; ++i) {
        यदि (!containsNone(c.getRangeStart(i), c.getRangeEnd(i))) {
            फर्काऊ असत्य;
        }
    }
    फर्काऊ strings_ == nullptr || !c.hasStrings() || strings_->containsNone(*c.strings_);
}

/**
 * Returns true if this set contains none of the characters
 * of the given string.
 * @param s string containing characters to be checked for containment
 * @return true if the test condition is met
 */
UBool UnicodeSet::containsNone(अचर UnicodeString& s) अचर {
    फर्काऊ span(s.getBuffer(), s.length(), USET_SPAN_NOT_CONTAINED) == s.length();
}

/**
 * Returns <tt>true</tt> if this set contains any character whose low byte
 * is the given value.  This is used by <tt>RuleBasedTransliterator</tt> for
 * indexing.
 */
UBool UnicodeSet::matches_Index_मान(uint8_t v) अचर {
    /* The index value v, in the range [0,255], is contained in this set if
     * it is contained in any pair of this set.  Pairs either have the high
     * bytes equal, or unequal.  If the high bytes are equal, then we have
     * aaxx..aayy, where aa is the high byte.  Then v is contained if xx <=
     * v <= yy.  If the high bytes are unequal we have aaxx..bbyy, bb>aa.
     * Then v is contained if xx <= v || v <= yy.  (This is identical to the
     * time zone month containment logic.)
     */
    int32_t i;
    int32_t range_गन्ती=get_Range_गन्ती();
    लागि (i=0; i<range_गन्ती; ++i) {
        UChar32 तल्लो = getRangeStart(i);
        UChar32 माथिल्लो = getRangeEnd(i);
        यदि ((तल्लो & ~0xFF) == (माथिल्लो & ~0xFF)) {
            यदि ((तल्लो & 0xFF) <= v && v <= (माथिल्लो & 0xFF)) {
                फर्काऊ सत्य;
            }
        } अन्यथा यदि ((तल्लो & 0xFF) <= v || v <= (माथिल्लो & 0xFF)) {
            फर्काऊ सत्य;
        }
    }
    यदि (hasStrings()) {
        लागि (i=0; i<strings_->size(); ++i) {
            अचर UnicodeString& s = *static_cast<अचर UnicodeString*>(strings_->elementAt(i));
            यदि (s.isEmpty()) {
                जारी;  // skip the empty string
            }
            UChar32 c = s.char32At(0);
            यदि ((c & 0xFF) == v) {
                फर्काऊ सत्य;
            }
        }
    }
    फर्काऊ असत्य;
}

/**
 * Implementation of UnicodeMatcher::matches().  Always matches the
 * longest possible multichar string.
 */
UMatchDegree UnicodeSet::matches(अचर Replaceable& text,
                                 int32_t& offset,
                                 int32_t limit,
                                 UBool incremental) {
    यदि (offset == limit) {
        यदि (contains(U_ETHER)) {
            फर्काऊ incremental ? U_PARTIAL_MATCH : U_MATCH;
        } अन्यथा {
            फर्काऊ U_MISMATCH;
        }
    } अन्यथा {
        यदि (hasStrings()) { // try strings first

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
            int32_t माथिल्लो_Water_Length = 0;

            लागि (i=0; i<strings_->size(); ++i) {
                अचर UnicodeString& trial = *static_cast<अचर UnicodeString*>(strings_->elementAt(i));
                यदि (trial.isEmpty()) {
                    जारी;  // skip the empty string
                }

                char16_t c = trial.charAt(forward ? 0 : trial.length() - 1);

                // Strings are sorted, so we can optimize in the
                // forward direction.
                यदि (forward && c > firstChar) रोक;
                यदि (c != firstChar) जारी;

                int32_t matchLen = matchRest(text, offset, limit, trial);

                यदि (incremental) {
                    int32_t maxLen = forward ? limit-offset : offset-limit;
                    यदि (matchLen == maxLen) {
                        // We have successfully matched but only up to limit.
                        फर्काऊ U_PARTIAL_MATCH;
                    }
                }

                यदि (matchLen == trial.length()) {
                    // We have successfully matched the whole string.
                    यदि (matchLen > माथिल्लो_Water_Length) {
                        माथिल्लो_Water_Length = matchLen;
                    }
                    // In the forward direction we know strings
                    // are sorted so we can bail early.
                    यदि (forward && matchLen < माथिल्लो_Water_Length) {
                        रोक;
                    }
                    जारी;
                }
            }

            // We've checked all strings without a partial match.
            // If we have full matches, return the longest one.
            यदि (माथिल्लो_Water_Length != 0) {
                offset += forward ? माथिल्लो_Water_Length : -माथिल्लो_Water_Length;
                फर्काऊ U_MATCH;
            }
        }
        फर्काऊ UnicodeFilter::matches(text, offset, limit, incremental);
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
int32_t UnicodeSet::matchRest(अचर Replaceable& text,
                              int32_t start, int32_t limit,
                              अचर UnicodeString& s) {
    int32_t i;
    int32_t maxLen;
    int32_t slen = s.length();
    यदि (start < limit) {
        maxLen = limit - start;
        यदि (maxLen > slen) maxLen = slen;
        लागि (i = 1; i < maxLen; ++i) {
            यदि (text.charAt(start + i) != s.charAt(i)) फर्काऊ 0;
        }
    } अन्यथा {
        maxLen = start - limit;
        यदि (maxLen > slen) maxLen = slen;
        --slen; // <=> slen = s.length() - 1;
        लागि (i = 1; i < maxLen; ++i) {
            यदि (text.charAt(start - i) != s.charAt(slen - i)) फर्काऊ 0;
        }
    }
    फर्काऊ maxLen;
}

/**
 * Implement of UnicodeMatcher
 */
शून्य UnicodeSet::addMatchSetTo(UnicodeSet& toUnionTo) अचर {
    toUnionTo.addAll(*एतत्);
}

/**
 * Returns the index of the given character within this set, where
 * the set is ordered by ascending code point.  If the character
 * is not in this set, return -1.  The inverse of this method is
 * <code>charAt()</code>.
 * @return an index from 0..size()-1, or -1
 */
int32_t UnicodeSet::indexOf(UChar32 c) अचर {
    यदि (c < MIN_मान || c > MAX_मान) {
        फर्काऊ -1;
    }
    int32_t i = 0;
    int32_t n = 0;
    लागि (;;) {
        UChar32 start = list[i++];
        यदि (c < start) {
            फर्काऊ -1;
        }
        UChar32 limit = list[i++];
        यदि (c < limit) {
            फर्काऊ n + c - start;
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
UChar32 UnicodeSet::charAt(int32_t index) अचर {
    यदि (index >= 0) {
        // len2 is the largest even integer <= len, that is, it is len
        // for even values and len-1 for odd values.  With odd values
        // the last entry is UNICODESET_HIGH.
        int32_t len2 = len & ~1;
        लागि (int32_t i=0; i < len2;) {
            UChar32 start = list[i++];
            int32_t count = list[i++] - start;
            यदि (index < count) {
                फर्काऊ static_cast<UChar32>(start + index);
            }
            index -= count;
        }
    }
    फर्काऊ static_cast<UChar32>(-1);
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
    फर्काऊ *एतत्;
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
    यदि (pinCodePoint(start) < pinCodePoint(end)) {
        UChar32 limit = end + 1;
        // Fast path for adding a new range after the last one.
        // Odd list length: [..., lastStart, lastLimit, HIGH]
        यदि ((len & 1) != 0) {
            // If the list is empty, set lastLimit low enough to not be adjacent to 0.
            UChar32 lastLimit = len == 1 ? -2 : list[len - 2];
            यदि (lastLimit <= start && !isFrozen() && !isBogus()) {
                यदि (lastLimit == start) {
                    // Extend the last range.
                    list[len - 2] = limit;
                    यदि (limit == UNICODESET_माथिल्लो) {
                        --len;
                    }
                } अन्यथा {
                    list[len - 1] = start;
                    यदि (limit < UNICODESET_माथिल्लो) {
                        यदि (ensureCapacity(len + 2)) {
                            list[len++] = limit;
                            list[len++] = UNICODESET_माथिल्लो;
                        }
                    } अन्यथा {  // limit == UNICODESET_HIGH
                        यदि (ensureCapacity(len + 1)) {
                            list[len++] = UNICODESET_माथिल्लो;
                        }
                    }
                }
                releasePattern();
                फर्काऊ *एतत्;
            }
        }
        // This is slow. Could be much faster using findCodePoint(start)
        // and modifying the list, dealing with adjacent & overlapping ranges.
        UChar32 range[3] = { start, limit, UNICODESET_माथिल्लो };
        add(range, 2, 0);
    } अन्यथा यदि (start == end) {
        add(start);
    }
    फर्काऊ *एतत्;
}

// #define DEBUG_US_ADD

#ifdef DEBUG_US_ADD
#अन्तर्भाव <stdio.h>
शून्य dump(UChar32 c) {
    यदि (c <= 0xFF) {
        म_लिखो("%c", (वर्ण)c);
    } अन्यथा {
        म_लिखो("U+%04X", c);
    }
}
शून्य dump(अचर UChar32* list, int32_t len) {
    म_लिखो("[");
    लागि (int32_t i=0; i<len; ++i) {
        यदि (i != 0) म_लिखो(", ");
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
    यदि ((i & 1) != 0  || isFrozen() || isBogus()) फर्काऊ *एतत्;

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

    यदि (c == list[i]-1) {
        // c is before start of next range
        list[i] = c;
        // if we touched the HIGH mark, then add a new one
        यदि (c == (UNICODESET_माथिल्लो - 1)) {
            यदि (!ensureCapacity(len+1)) {
                // ensureCapacity will mark the object as Bogus if OOM failure happens.
                फर्काऊ *एतत्;
            }
            list[len++] = UNICODESET_माथिल्लो;
        }
        यदि (i > 0 && c == list[i-1]) {
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
            जबसम्म (src < srclimit) *(dst++) = *(src++);

            len -= 2;
        }
    }

    अन्यथा यदि (i > 0 && c == list[i-1]) {
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

        यदि (!ensureCapacity(len+2)) {
            // ensureCapacity will mark the object as Bogus if OOM failure happens.
            फर्काऊ *एतत्;
        }

        UChar32 *p = list + i;
        uprv_memmove(p + 2, p, (len - i) * परिमाण(*p));
        list[i] = c;
        list[i+1] = c+1;
        len += 2;
    }

#ifdef DEBUG_US_ADD
    dump(list, len);
    म_लिखो("\n");

    लागि (i=1; i<len; ++i) {
        यदि (list[i] <= list[i-1]) {
            // Corrupt array!
            म_लिखो("ERROR: list has been corrupted\n");
            निकास(1);
        }
    }
#endif

    releasePattern();
    फर्काऊ *एतत्;
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
UnicodeSet& UnicodeSet::add(अचर UnicodeString& s) U_LIFETIME_BOUND {
    यदि (isFrozen() || isBogus()) फर्काऊ *एतत्;
    int32_t cp = getSingleCP(s);
    यदि (cp < 0) {
        यदि (!stringsContains(s)) {
            _add(s);
            releasePattern();
        }
    } अन्यथा {
        add(static_cast<UChar32>(cp));
    }
    फर्काऊ *एतत्;
}

/**
 * Adds the given string, in order, to 'strings_'.  The given string
 * must have been checked by the caller to not already be in 'strings_'.
 */
शून्य UnicodeSet::_add(अचर UnicodeString& s) {
    यदि (isFrozen() || isBogus()) {
        फर्काऊ;
    }
    UErrorCode ec = U_ZERO_ERROR;
    यदि (strings_ == nullptr && !allocateStrings(ec)) {
        setToBogus();
        फर्काऊ;
    }
    LocalPointer<UnicodeString> t(नूतन UnicodeString(s));
    यदि (t.isNull()) { // Check for memory allocation error.
        setToBogus();
        फर्काऊ;
    }
    strings_->sorted_घुसाउ(t.orphan(), compareUnicodeString, ec);
    यदि (U_FAILURE(ec)) {
        setToBogus();
    }
}

/**
 * @return a code point IF the string consists of a single one.
 * otherwise returns -1.
 * @param string to test
 */
int32_t UnicodeSet::getSingleCP(अचर UnicodeString& s) {
    int32_t sLength = s.length();
    यदि (sLength == 1) फर्काऊ s.charAt(0);
    यदि (sLength == 2) {
        UChar32 cp = s.char32At(0);
        यदि (cp > 0xFFFF) { // is surrogate pair
            फर्काऊ cp;
        }
    }
    फर्काऊ -1;
}

/**
 * Adds each of the characters in this string to the set. Thus "ch" => {"c", "h"}
 * If this set already any particular character, it has no effect on that character.
 * @param the source string
 * @return the modified set, for chaining
 */
UnicodeSet& UnicodeSet::addAll(अचर UnicodeString& s) U_LIFETIME_BOUND {
    UChar32 cp;
    लागि (int32_t i = 0; i < s.length(); i += U16_LENGTH(cp)) {
        cp = s.char32At(i);
        add(cp);
    }
    फर्काऊ *एतत्;
}

/**
 * Retains EACH of the characters in this string. Note: "ch" == {"c", "h"}
 * If this set already any particular character, it has no effect on that character.
 * @param the source string
 * @return the modified set, for chaining
 */
UnicodeSet& UnicodeSet::retainAll(अचर UnicodeString& s) U_LIFETIME_BOUND {
    UnicodeSet set;
    set.addAll(s);
    retainAll(set);
    फर्काऊ *एतत्;
}

/**
 * Complement EACH of the characters in this string. Note: "ch" == {"c", "h"}
 * If this set already any particular character, it has no effect on that character.
 * @param the source string
 * @return the modified set, for chaining
 */
UnicodeSet& UnicodeSet::complementAll(अचर UnicodeString& s) U_LIFETIME_BOUND {
    UnicodeSet set;
    set.addAll(s);
    complementAll(set);
    फर्काऊ *एतत्;
}

/**
 * Remove EACH of the characters in this string. Note: "ch" == {"c", "h"}
 * If this set already any particular character, it has no effect on that character.
 * @param the source string
 * @return the modified set, for chaining
 */
UnicodeSet& UnicodeSet::removeAll(अचर UnicodeString& s) U_LIFETIME_BOUND {
    UnicodeSet set;
    set.addAll(s);
    removeAll(set);
    फर्काऊ *एतत्;
}

UnicodeSet& UnicodeSet::removeAllStrings() U_LIFETIME_BOUND {
    यदि (!isFrozen() && hasStrings()) {
        strings_->removeAllElements();
        releasePattern();
    }
    फर्काऊ *एतत्;
}


/**
 * Makes a set from a multicharacter string. Thus "ch" => {"ch"}
 * <br><b>Warning: you cannot add an empty string ("") to a UnicodeSet.</b>
 * @param the source string
 * @return a newly created set containing the given string
 */
UnicodeSet* U_EXPORT2 UnicodeSet::createFrom(अचर UnicodeString& s) {
    UnicodeSet *set = नूतन UnicodeSet();
    यदि (set != nullptr) { // Check for memory allocation error.
        set->add(s);
    }
    फर्काऊ set;
}


/**
 * Makes a set from each of the characters in the string. Thus "ch" => {"c", "h"}
 * @param the source string
 * @return a newly created set containing the given characters
 */
UnicodeSet* U_EXPORT2 UnicodeSet::createFromAll(अचर UnicodeString& s) {
    UnicodeSet *set = नूतन UnicodeSet();
    यदि (set != nullptr) { // Check for memory allocation error.
        set->addAll(s);
    }
    फर्काऊ set;
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
    यदि (pinCodePoint(start) <= pinCodePoint(end)) {
        UChar32 range[3] = { start, end+1, UNICODESET_माथिल्लो };
        retain(range, 2, 0);
    } अन्यथा {
        clear();
    }
    फर्काऊ *एतत्;
}

UnicodeSet& UnicodeSet::retain(UChar32 c) U_LIFETIME_BOUND {
    फर्काऊ retain(c, c);
}

UnicodeSet& UnicodeSet::retain(अचर UnicodeString &s) U_LIFETIME_BOUND {
    यदि (isFrozen() || isBogus()) { फर्काऊ *एतत्; }
    UChar32 cp = getSingleCP(s);
    यदि (cp < 0) {
        तार्किक isIn = stringsContains(s);
        // Check for getRangeCount() first to avoid somewhat-expensive size()
        // when there are single code points.
        यदि (isIn && get_Range_गन्ती() == 0 && size() == 1) {
            फर्काऊ *एतत्;
        }
        clear();
        यदि (isIn) {
            _add(s);
        }
    } अन्यथा {
        retain(cp, cp);
    }
    फर्काऊ *एतत्;
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
    यदि (pinCodePoint(start) <= pinCodePoint(end)) {
        UChar32 range[3] = { start, end+1, UNICODESET_माथिल्लो };
        retain(range, 2, 2);
    }
    फर्काऊ *एतत्;
}

/**
 * Removes the specified character from this set if it is present.
 * The set will not contain the specified range once the call
 * returns.
 */
UnicodeSet& UnicodeSet::हटाओ(UChar32 c) U_LIFETIME_BOUND {
    फर्काऊ हटाओ(c, c);
}

/**
 * Removes the specified string from this set if it is present.
 * The set will not contain the specified character once the call
 * returns.
 * @param the source string
 * @return the modified set, for chaining
 */
UnicodeSet& UnicodeSet::हटाओ(अचर UnicodeString& s) U_LIFETIME_BOUND {
    यदि (isFrozen() || isBogus()) फर्काऊ *एतत्;
    int32_t cp = getSingleCP(s);
    यदि (cp < 0) {
        यदि (strings_ != nullptr && strings_->removeElement((शून्य*) &s)) {
            releasePattern();
        }
    } अन्यथा {
        हटाओ(static_cast<UChar32>(cp), static_cast<UChar32>(cp));
    }
    फर्काऊ *एतत्;
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
    यदि (isFrozen() || isBogus()) {
        फर्काऊ *एतत्;
    }
    यदि (pinCodePoint(start) <= pinCodePoint(end)) {
        UChar32 range[3] = { start, end+1, UNICODESET_माथिल्लो };
        exclusiveOr(range, 2, 0);
    }
    releasePattern();
    फर्काऊ *एतत्;
}

UnicodeSet& UnicodeSet::complement(UChar32 c) U_LIFETIME_BOUND {
    फर्काऊ complement(c, c);
}

/**
 * This is equivalent to
 * <code>complement(MIN_VALUE, MAX_VALUE)</code>.
 */
UnicodeSet& UnicodeSet::complement() U_LIFETIME_BOUND {
    यदि (isFrozen() || isBogus()) {
        फर्काऊ *एतत्;
    }
    यदि (list[0] == UNICODESET_तल्लो) {
        uprv_memmove(list, list + 1, (माप_प्रकार)(len-1)*परिमाण(UChar32));
        --len;
    } अन्यथा {
        यदि (!ensureCapacity(len+1)) {
            फर्काऊ *एतत्;
        }
        uprv_memmove(list + 1, list, (माप_प्रकार)len*परिमाण(UChar32));
        list[0] = UNICODESET_तल्लो;
        ++len;
    }
    releasePattern();
    फर्काऊ *एतत्;
}

/**
 * Complement the specified string in this set.
 * The set will not contain the specified string once the call
 * returns.
 *
 * @param s the string to complement
 * @return this object, for chaining
 */
UnicodeSet& UnicodeSet::complement(अचर UnicodeString& s) U_LIFETIME_BOUND {
    यदि (isFrozen() || isBogus()) फर्काऊ *एतत्;
    int32_t cp = getSingleCP(s);
    यदि (cp < 0) {
        यदि (stringsContains(s)) {
            strings_->removeElement((शून्य*) &s);
        } अन्यथा {
            _add(s);
        }
        releasePattern();
    } अन्यथा {
        complement(static_cast<UChar32>(cp), static_cast<UChar32>(cp));
    }
    फर्काऊ *एतत्;
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
UnicodeSet& UnicodeSet::addAll(अचर UnicodeSet& c) U_LIFETIME_BOUND {
    यदि ( c.len>0 && c.list!=nullptr ) {
        add(c.list, c.len, 0);
    }

    // Add strings in order
    यदि ( c.strings_!=nullptr ) {
        लागि (int32_t i=0; i<c.strings_->size(); ++i) {
            अचर UnicodeString* s = static_cast<अचर UnicodeString*>(c.strings_->elementAt(i));
            यदि (!stringsContains(*s)) {
                _add(*s);
            }
        }
    }
    फर्काऊ *एतत्;
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
UnicodeSet& UnicodeSet::retainAll(अचर UnicodeSet& c) U_LIFETIME_BOUND {
    यदि (isFrozen() || isBogus()) {
        फर्काऊ *एतत्;
    }
    retain(c.list, c.len, 0);
    यदि (hasStrings()) {
        यदि (!c.hasStrings()) {
            strings_->removeAllElements();
        } अन्यथा {
            strings_->retainAll(*c.strings_);
        }
    }
    फर्काऊ *एतत्;
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
UnicodeSet& UnicodeSet::removeAll(अचर UnicodeSet& c) U_LIFETIME_BOUND {
    यदि (isFrozen() || isBogus()) {
        फर्काऊ *एतत्;
    }
    retain(c.list, c.len, 2);
    यदि (hasStrings() && c.hasStrings()) {
        strings_->removeAll(*c.strings_);
    }
    फर्काऊ *एतत्;
}

/**
 * Complements in this set all elements contained in the specified
 * set.  Any character in the other set will be removed if it is
 * in this set, or will be added if it is not in this set.
 *
 * @param c set that defines which elements will be xor'ed from
 *          this set.
 */
UnicodeSet& UnicodeSet::complementAll(अचर UnicodeSet& c) U_LIFETIME_BOUND {
    यदि (isFrozen() || isBogus()) {
        फर्काऊ *एतत्;
    }
    exclusiveOr(c.list, c.len, 0);

    यदि (c.strings_ != nullptr) {
        लागि (int32_t i=0; i<c.strings_->size(); ++i) {
            शून्य* e = c.strings_->elementAt(i);
            यदि (strings_ == nullptr || !strings_->removeElement(e)) {
                _add(*static_cast<अचर UnicodeString*>(e));
            }
        }
    }
    फर्काऊ *एतत्;
}

/**
 * Removes all of the elements from this set.  This set will be
 * empty after this call returns.
 */
UnicodeSet& UnicodeSet::clear() U_LIFETIME_BOUND {
    यदि (isFrozen()) {
        फर्काऊ *एतत्;
    }
    list[0] = UNICODESET_माथिल्लो;
    len = 1;
    releasePattern();
    यदि (strings_ != nullptr) {
        strings_->removeAllElements();
    }
    // Remove bogus
    fFlags = 0;
    फर्काऊ *एतत्;
}

/**
 * Iteration method that returns the number of ranges contained in
 * this set.
 * @see #getRangeStart
 * @see #getRangeEnd
 */
int32_t UnicodeSet::get_Range_गन्ती() अचर {
    फर्काऊ len/2;
}

/**
 * Iteration method that returns the first character in the
 * specified range of this set.
 * @see #getRangeCount
 * @see #getRangeEnd
 */
UChar32 UnicodeSet::getRangeStart(int32_t index) अचर {
    फर्काऊ list[index*2];
}

/**
 * Iteration method that returns the last character in the
 * specified range of this set.
 * @see #getRangeStart
 * @see #getRangeEnd
 */
UChar32 UnicodeSet::getRangeEnd(int32_t index) अचर {
    फर्काऊ list[index*2 + 1] - 1;
}

अचर UnicodeString* UnicodeSet::getString(int32_t index) अचर U_LIFETIME_BOUND {
    फर्काऊ static_cast<अचर UnicodeString*>(strings_->elementAt(index));
}

/**
 * Reallocate this objects internal structures to take up the least
 * possible space, without changing this object's value.
 */
UnicodeSet& UnicodeSet::compact() U_LIFETIME_BOUND {
    यदि (isFrozen() || isBogus()) {
        फर्काऊ *एतत्;
    }
    // Delete buffer first to defragment memory less.
    यदि (buffer != थाक_List) {
        uprv_free(buffer);
        buffer = nullptr;
        bufferCapacity = 0;
    }
    यदि (list == थाक_List) {
        // pass
    } अन्यथा यदि (len <= INITIAL_CAPACITY) {
        uprv_memcpy(थाक_List, list, len * परिमाण(UChar32));
        uprv_free(list);
        list = थाक_List;
        capacity = INITIAL_CAPACITY;
    } अन्यथा यदि ((len + 7) < capacity) {
        // If we have more than a little unused capacity, shrink it to len.
        UChar32* temp = static_cast<UChar32*>(uprv_realloc(list, परिमाण(UChar32) * len));
        यदि (temp) {
            list = temp;
            capacity = len;
        }
        // else what the heck happened?! We allocated less memory!
        // Oh well. We'll keep our original array.
    }
    यदि (strings_ != nullptr && strings_->isEmpty()) {
        delete strings_;
        strings_ = nullptr;
    }
    फर्काऊ *एतत्;
}

#ifdef DEBUG_SERIALIZE
#अन्तर्भाव <stdio.h>
#endif

/**
 * Deserialize constructor.
 */
UnicodeSet::UnicodeSet(अचर uint16_t data[], int32_t dataLen, ESerialization serialization,
                       UErrorCode &ec) {

  यदि(U_FAILURE(ec)) {
    setToBogus();
    फर्काऊ;
  }

  यदि( (serialization != kSerialized)
      || (data==nullptr)
      || (dataLen < 1)) {
    ec = U_ILLEGAL_ARGUMENT_ERROR;
    setToBogus();
    फर्काऊ;
  }

  // bmp?
  int32_t headerSize = ((data[0]&0x8000)) ?2:1;
  int32_t bmpLength = (headerSize==1)?data[0]:data[1];

  int32_t newLength = (((data[0]&0x7FFF)-bmpLength)/2)+bmpLength;
#ifdef DEBUG_SERIALIZE
  म_लिखो("dataLen %d headerSize %d bmpLen %d len %d. data[0]=%X/%X/%X/%X\n", dataLen,headerSize,bmpLength,newLength, data[0],data[1],data[2],data[3]);
#endif
  यदि(!ensureCapacity(newLength + 1)) {  // +1 for HIGH
    फर्काऊ;
  }
  // copy bmp
  int32_t i;
  लागि(i = 0; i< bmpLength;i++) {
    list[i] = data[i+headerSize];
#ifdef DEBUG_SERIALIZE
    म_लिखो("<<16@%d[%d] %X\n", i+headerSize, i, list[i]);
#endif
  }
  // copy smp
  लागि(i=bmpLength;i<newLength;i++) {
    list[i] = (static_cast<UChar32>(data[headerSize + bmpLength + (i - bmpLength) * 2 + 0]) << 16) +
               static_cast<UChar32>(data[headerSize + bmpLength + (i - bmpLength) * 2 + 1]);
#ifdef DEBUG_SERIALIZE
    म_लिखो("<<32@%d+[%d] %lX\n", headerSize+bmpLength+i, i, list[i]);
#endif
  }
  U_ASSERT(i == newLength);
  यदि (i == 0 || list[i - 1] != UNICODESET_माथिल्लो) {
    list[i++] = UNICODESET_माथिल्लो;
  }
  len = i;
}


int32_t UnicodeSet::serialize(uint16_t *dest, int32_t destCapacity, UErrorCode& ec) अचर {
    int32_t bmpLength, length, destLength;

    यदि (U_FAILURE(ec)) {
        फर्काऊ 0;
    }

    यदि (destCapacity<0 || (destCapacity>0 && dest==nullptr)) {
        ec=U_ILLEGAL_ARGUMENT_ERROR;
        फर्काऊ 0;
    }

    /* count necessary 16-bit units */
    length=एतत्->len-1; // Subtract 1 to ignore final UNICODESET_HIGH
    // assert(length>=0);
    यदि (length==0) {
        /* empty set */
        यदि (destCapacity>0) {
            *dest=0;
        } अन्यथा {
            ec=U_BUFFER_OVERFLOW_ERROR;
        }
        फर्काऊ 1;
    }
    /* now length>0 */

    यदि (एतत्->list[length-1]<=0xffff) {
        /* all BMP */
        bmpLength=length;
    } अन्यथा यदि (एतत्->list[0]>=0x10000) {
        /* all supplementary */
        bmpLength=0;
        length*=2;
    } अन्यथा {
        /* some BMP, some supplementary */
        लागि (bmpLength=0; bmpLength<length && एतत्->list[bmpLength]<=0xffff; ++bmpLength) {}
        length=bmpLength+2*(length-bmpLength);
    }
#ifdef DEBUG_SERIALIZE
    म_लिखो(">> bmpLength%d length%d len%d\n", bmpLength, length, len);
#endif
    /* length: number of 16-bit array units */
    यदि (length>0x7fff) {
        /* there are only 15 bits for the length in the first serialized word */
        ec=U_INDEX_OUTOFBOUNDS_ERROR;
        फर्काऊ 0;
    }

    /*
     * total serialized length:
     * number of 16-bit array units (length) +
     * 1 length unit (always) +
     * 1 bmpLength unit (if there are supplementary values)
     */
    destLength=length+((length>bmpLength)?2:1);
    यदि (destLength<=destCapacity) {
        अचर UChar32 *p;
        int32_t i;

#ifdef DEBUG_SERIALIZE
        म_लिखो("writeHdr\n");
#endif
        *dest = static_cast<uint16_t>(length);
        यदि (length>bmpLength) {
            *dest|=0x8000;
            *++dest = static_cast<uint16_t>(bmpLength);
        }
        ++dest;

        /* write the BMP part of the array */
        p=एतत्->list;
        लागि (i=0; i<bmpLength; ++i) {
#ifdef DEBUG_SERIALIZE
          म_लिखो("writebmp: %x\n", (पूर्णाङ्क)*p);
#endif
            *dest++ = static_cast<uint16_t>(*p++);
        }

        /* write the supplementary part of the array */
        लागि (; i<length; i+=2) {
#ifdef DEBUG_SERIALIZE
          म_लिखो("write32: %x\n", (पूर्णाङ्क)*p);
#endif
            *dest++ = static_cast<uint16_t>(*p >> 16);
            *dest++ = static_cast<uint16_t>(*p++);
        }
    } अन्यथा {
        ec=U_BUFFER_OVERFLOW_ERROR;
    }
    फर्काऊ destLength;
}

//----------------------------------------------------------------
// Implementation: Utility methods
//----------------------------------------------------------------

/**
 * Allocate our strings vector and return true if successful.
 */
UBool UnicodeSet::allocateStrings(UErrorCode &status) {
    यदि (U_FAILURE(status)) {
        फर्काऊ असत्य;
    }
    strings_ = नूतन UVector(uprv_deleteUObject,
                          uhash_compareUnicodeString, 1, status);
    यदि (strings_ == nullptr) { // Check for memory allocation error.
        status = U_MEMORY_ALLOCATION_ERROR;
        फर्काऊ असत्य;
    }
    यदि (U_FAILURE(status)) {
        delete strings_;
        strings_ = nullptr;
        फर्काऊ असत्य;
    } 
    फर्काऊ सत्य;
}

int32_t UnicodeSet::nextCapacity(int32_t minCapacity) {
    // Grow exponentially to reduce the frequency of allocations.
    यदि (minCapacity < INITIAL_CAPACITY) {
        फर्काऊ minCapacity + INITIAL_CAPACITY;
    } अन्यथा यदि (minCapacity <= 2500) {
        फर्काऊ 5 * minCapacity;
    } अन्यथा {
        int32_t newCapacity = 2 * minCapacity;
        यदि (newCapacity > MAX_LENGTH) {
            newCapacity = MAX_LENGTH;
        }
        फर्काऊ newCapacity;
    }
}

तार्किक UnicodeSet::ensureCapacity(int32_t newLen) {
    यदि (newLen > MAX_LENGTH) {
        newLen = MAX_LENGTH;
    }
    यदि (newLen <= capacity) {
        फर्काऊ सत्य;
    }
    int32_t newCapacity = nextCapacity(newLen);
    UChar32* temp = static_cast<UChar32*>(uprv_malloc(newCapacity * परिमाण(UChar32)));
    यदि (temp == nullptr) {
        setToBogus(); // set the object to bogus state if an OOM failure occurred.
        फर्काऊ असत्य;
    }
    // Copy only the actual contents.
    uprv_memcpy(temp, list, len * परिमाण(UChar32));
    यदि (list != थाक_List) {
        uprv_free(list);
    }
    list = temp;
    capacity = newCapacity;
    फर्काऊ सत्य;
}

तार्किक UnicodeSet::ensureBufferCapacity(int32_t newLen) {
    यदि (newLen > MAX_LENGTH) {
        newLen = MAX_LENGTH;
    }
    यदि (newLen <= bufferCapacity) {
        फर्काऊ सत्य;
    }
    int32_t newCapacity = nextCapacity(newLen);
    UChar32* temp = static_cast<UChar32*>(uprv_malloc(newCapacity * परिमाण(UChar32)));
    यदि (temp == nullptr) {
        setToBogus();
        फर्काऊ असत्य;
    }
    // The buffer has no contents to be copied.
    // It is always filled from scratch after this call.
    यदि (buffer != थाक_List) {
        uprv_free(buffer);
    }
    buffer = temp;
    bufferCapacity = newCapacity;
    फर्काऊ सत्य;
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

स्थिर अन्तःस्थ UChar32 max(UChar32 a, UChar32 b) {
    फर्काऊ (a > b) ? a : b;
}

// polarity = 0, 3 is normal: x xor y
// polarity = 1, 2: x xor ~y == x === y

शून्य UnicodeSet::exclusiveOr(अचर UChar32* other, int32_t otherLen, int8_t polarity) {
    यदि (isFrozen() || isBogus()) {
        फर्काऊ;
    }
    यदि (!ensureBufferCapacity(len + otherLen)) {
        फर्काऊ;
    }

    int32_t i = 0, j = 0, k = 0;
    UChar32 a = list[i++];
    UChar32 b;
    यदि (polarity == 1 || polarity == 2) {
        b = UNICODESET_तल्लो;
        यदि (other[j] == UNICODESET_तल्लो) { // skip base if already LOW
            ++j;
            b = other[j];
        }
    } अन्यथा {
        b = other[j++];
    }
    // simplest of all the routines
    // sort the values, discarding identicals!
    लागि (;;) {
        यदि (a < b) {
            buffer[k++] = a;
            a = list[i++];
        } अन्यथा यदि (b < a) {
            buffer[k++] = b;
            b = other[j++];
        } अन्यथा यदि (a != UNICODESET_माथिल्लो) { // at this point, a == b
            // discard both values!
            a = list[i++];
            b = other[j++];
        } अन्यथा { // DONE!
            buffer[k++] = UNICODESET_माथिल्लो;
            len = k;
            रोक;
        }
    }
    swapBuffers();
    releasePattern();
}

// polarity = 0 is normal: x union y
// polarity = 2: x union ~y
// polarity = 1: ~x union y
// polarity = 3: ~x union ~y

शून्य UnicodeSet::add(अचर UChar32* other, int32_t otherLen, int8_t polarity) {
    यदि (isFrozen() || isBogus() || other==nullptr) {
        फर्काऊ;
    }
    यदि (!ensureBufferCapacity(len + otherLen)) {
        फर्काऊ;
    }

    int32_t i = 0, j = 0, k = 0;
    UChar32 a = list[i++];
    UChar32 b = other[j++];
    // change from xor is that we have to check overlapping pairs
    // polarity bit 1 means a is second, bit 2 means b is.
    लागि (;;) {
        विकल्प (polarity) {
          पक्ष 0: // both first; take lower if unequal
            यदि (a < b) { // take a
                // Back up over overlapping ranges in buffer[]
                यदि (k > 0 && a <= buffer[k-1]) {
                    // Pick latter end value in buffer[] vs. list[]
                    a = max(list[i], buffer[--k]);
                } अन्यथा {
                    // No overlap
                    buffer[k++] = a;
                    a = list[i];
                }
                i++; // Common if/else code factored out
                polarity ^= 1;
            } अन्यथा यदि (b < a) { // take b
                यदि (k > 0 && b <= buffer[k-1]) {
                    b = max(other[j], buffer[--k]);
                } अन्यथा {
                    buffer[k++] = b;
                    b = other[j];
                }
                j++;
                polarity ^= 2;
            } अन्यथा { // a == b, take a, drop b
                यदि (a == UNICODESET_माथिल्लो) गच्छ loop_end;
                // This is symmetrical; it doesn't matter if
                // we backtrack with a or b. - liu
                यदि (k > 0 && a <= buffer[k-1]) {
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
            रोक;
          पक्ष 3: // both second; take higher if unequal, and drop other
            यदि (b <= a) { // take a
                यदि (a == UNICODESET_माथिल्लो) गच्छ loop_end;
                buffer[k++] = a;
            } अन्यथा { // take b
                यदि (b == UNICODESET_माथिल्लो) गच्छ loop_end;
                buffer[k++] = b;
            }
            a = list[i++];
            polarity ^= 1;   // factored common code
            b = other[j++];
            polarity ^= 2;
            रोक;
          पक्ष 1: // a second, b first; if b < a, overlap
            यदि (a < b) { // no overlap, take a
                buffer[k++] = a; a = list[i++]; polarity ^= 1;
            } अन्यथा यदि (b < a) { // OVERLAP, drop b
                b = other[j++];
                polarity ^= 2;
            } अन्यथा { // a == b, drop both!
                यदि (a == UNICODESET_माथिल्लो) गच्छ loop_end;
                a = list[i++];
                polarity ^= 1;
                b = other[j++];
                polarity ^= 2;
            }
            रोक;
          पक्ष 2: // a first, b second; if a < b, overlap
            यदि (b < a) { // no overlap, take b
                buffer[k++] = b;
                b = other[j++];
                polarity ^= 2;
            } अन्यथा  यदि (a < b) { // OVERLAP, drop a
                a = list[i++];
                polarity ^= 1;
            } अन्यथा { // a == b, drop both!
                यदि (a == UNICODESET_माथिल्लो) गच्छ loop_end;
                a = list[i++];
                polarity ^= 1;
                b = other[j++];
                polarity ^= 2;
            }
            रोक;
        }
    }
 loop_end:
    buffer[k++] = UNICODESET_माथिल्लो;    // terminate
    len = k;
    swapBuffers();
    releasePattern();
}

// polarity = 0 is normal: x intersect y
// polarity = 2: x intersect ~y == set-minus
// polarity = 1: ~x intersect y
// polarity = 3: ~x intersect ~y

शून्य UnicodeSet::retain(अचर UChar32* other, int32_t otherLen, int8_t polarity) {
    यदि (isFrozen() || isBogus()) {
        फर्काऊ;
    }
    यदि (!ensureBufferCapacity(len + otherLen)) {
        फर्काऊ;
    }

    int32_t i = 0, j = 0, k = 0;
    UChar32 a = list[i++];
    UChar32 b = other[j++];
    // change from xor is that we have to check overlapping pairs
    // polarity bit 1 means a is second, bit 2 means b is.
    लागि (;;) {
        विकल्प (polarity) {
          पक्ष 0: // both first; drop the smaller
            यदि (a < b) { // drop a
                a = list[i++];
                polarity ^= 1;
            } अन्यथा यदि (b < a) { // drop b
                b = other[j++];
                polarity ^= 2;
            } अन्यथा { // a == b, take one, drop other
                यदि (a == UNICODESET_माथिल्लो) गच्छ loop_end;
                buffer[k++] = a;
                a = list[i++];
                polarity ^= 1;
                b = other[j++];
                polarity ^= 2;
            }
            रोक;
          पक्ष 3: // both second; take lower if unequal
            यदि (a < b) { // take a
                buffer[k++] = a;
                a = list[i++];
                polarity ^= 1;
            } अन्यथा यदि (b < a) { // take b
                buffer[k++] = b;
                b = other[j++];
                polarity ^= 2;
            } अन्यथा { // a == b, take one, drop other
                यदि (a == UNICODESET_माथिल्लो) गच्छ loop_end;
                buffer[k++] = a;
                a = list[i++];
                polarity ^= 1;
                b = other[j++];
                polarity ^= 2;
            }
            रोक;
          पक्ष 1: // a second, b first;
            यदि (a < b) { // NO OVERLAP, drop a
                a = list[i++];
                polarity ^= 1;
            } अन्यथा यदि (b < a) { // OVERLAP, take b
                buffer[k++] = b;
                b = other[j++];
                polarity ^= 2;
            } अन्यथा { // a == b, drop both!
                यदि (a == UNICODESET_माथिल्लो) गच्छ loop_end;
                a = list[i++];
                polarity ^= 1;
                b = other[j++];
                polarity ^= 2;
            }
            रोक;
          पक्ष 2: // a first, b second; if a < b, overlap
            यदि (b < a) { // no overlap, drop b
                b = other[j++];
                polarity ^= 2;
            } अन्यथा  यदि (a < b) { // OVERLAP, take a
                buffer[k++] = a;
                a = list[i++];
                polarity ^= 1;
            } अन्यथा { // a == b, drop both!
                यदि (a == UNICODESET_माथिल्लो) गच्छ loop_end;
                a = list[i++];
                polarity ^= 1;
                b = other[j++];
                polarity ^= 2;
            }
            रोक;
        }
    }
 loop_end:
    buffer[k++] = UNICODESET_माथिल्लो;    // terminate
    len = k;
    swapBuffers();
    releasePattern();
}

/**
 * Append the <code>toPattern()</code> representation of a
 * string to the given <code>StringBuffer</code>.
 */
शून्य UnicodeSet::_appendToPat(UnicodeString& buf, अचर UnicodeString& s, UBool escapeUnprintable) {
    UChar32 cp;
    लागि (int32_t i = 0; i < s.length(); i += U16_LENGTH(cp)) {
        _appendToPat(buf, cp = s.char32At(i), escapeUnprintable);
    }
}

/**
 * Append the <code>toPattern()</code> representation of a
 * character to the given <code>StringBuffer</code>.
 */
शून्य UnicodeSet::_appendToPat(UnicodeString& buf, UChar32 c, UBool escapeUnprintable) {
    यदि (escapeUnprintable ? ICU_Utility::isUnprintable(c) : ICU_Utility::shouldAlwaysBeEscaped(c)) {
        // Use hex escape notation (\uxxxx or \Uxxxxxxxx) for anything
        // unprintable
        ICU_Utility::escape(buf, c);
        फर्काऊ;
    }
    // Okay to let ':' pass through
    विकल्प (c) {
    पक्ष u'[':
    पक्ष u']':
    पक्ष u'-':
    पक्ष u'^':
    पक्ष u'&':
    पक्ष u'\\':
    पक्ष u'{':
    पक्ष u'}':
    पक्ष u':':
    पक्ष चिह्न_Table::चिह्न_REF:
        buf.append(u'\\');
        रोक;
    स्वभाव:
        // Escape whitespace
        यदि (PatternProps::isWhiteSpace(c)) {
            buf.append(u'\\');
        }
        रोक;
    }
    buf.append(c);
}

शून्य UnicodeSet::_appendToPat(UnicodeString &result, UChar32 start, UChar32 end,
                              UBool escapeUnprintable) {
    _appendToPat(result, start, escapeUnprintable);
    यदि (start != end) {
        यदि ((start+1) != end ||
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
                                      UBool escapeUnprintable) अचर
{
    यदि (pat != nullptr) {
        int32_t i;
        int32_t backslash_गन्ती = 0;
        लागि (i=0; i<patLen; ) {
            UChar32 c;
            U16_NEXT(pat, i, patLen, c);
            यदि (escapeUnprintable ?
                    ICU_Utility::isUnprintable(c) : ICU_Utility::shouldAlwaysBeEscaped(c)) {
                // If the unprintable character is preceded by an odd
                // number of backslashes, then it has been escaped.
                // Before unescaping it, we delete the final
                // backslash.
                यदि ((backslash_गन्ती % 2) == 1) {
                    result.truncate(result.length() - 1);
                }
                ICU_Utility::escape(result, c);
                backslash_गन्ती = 0;
            } अन्यथा {
                result.append(c);
                यदि (c == u'\\') {
                    ++backslash_गन्ती;
                } अन्यथा {
                    backslash_गन्ती = 0;
                }
            }
        }
        फर्काऊ result;
    }

    फर्काऊ _generatePattern(result, escapeUnprintable);
}

/**
 * Returns a string representation of this set.  If the result of
 * calling this function is passed to a UnicodeSet constructor, it
 * will produce another set that is equal to this one.
 */
UnicodeString& UnicodeSet::toPattern(UnicodeString& result U_LIFETIME_BOUND,
                                     UBool escapeUnprintable) अचर
{
    result.truncate(0);
    फर्काऊ _toPattern(result, escapeUnprintable);
}

/**
 * Generate and append a string representation of this set to result.
 * This does not use this.pat, the cleaned up copy of the string
 * passed to applyPattern().
 */
UnicodeString& UnicodeSet::_generatePattern(UnicodeString& result U_LIFETIME_BOUND,
                                            UBool escapeUnprintable) अचर
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
    यदि (len >= 4 && list[0] == 0 && limit == len && !hasStrings()) {
        // Emit the inverse
        result.append(u'^');
        // Offsetting the inversion list index by one lets us
        // iterate over the ranges of the set complement.
        i = 1;
        --limit;
    }

    // Emit the ranges as pairs.
    जबसम्म (i < limit) {
        UChar32 start = list[i];  // getRangeStart()
        UChar32 end = list[i + 1] - 1;  // getRangeEnd() = range limit minus one
        यदि (!(0xd800 <= end && end <= 0xdbff)) {
            _appendToPat(result, start, end, escapeUnprintable);
            i += 2;
        } अन्यथा {
            // The range ends with a lead surrogate.
            // Avoid writing what looks like a lead+trail surrogate pair.
            // 1. Postpone ranges that start with a lead surrogate code point.
            int32_t firstLead = i;
            जबसम्म ((i += 2) < limit && list[i] <= 0xdbff) {}
            int32_t firstAfterLead = i;
            // 2. Write following ranges that start with a trail surrogate code point.
            जबसम्म (i < limit && (start = list[i]) <= 0xdfff) {
                _appendToPat(result, start, list[i + 1] - 1, escapeUnprintable);
                i += 2;
            }
            // 3. Now write the postponed ranges.
            लागि (पूर्णाङ्क j = firstLead; j < firstAfterLead; j += 2) {
                _appendToPat(result, list[j], list[j + 1] - 1, escapeUnprintable);
            }
        }
    }

    यदि (strings_ != nullptr) {
        लागि (int32_t i = 0; i<strings_->size(); ++i) {
            result.append(u'{');
            _appendToPat(result,
                         *static_cast<अचर UnicodeString*>(strings_->elementAt(i)),
                         escapeUnprintable);
            result.append(u'}');
        }
    }
    फर्काऊ result.append(u']');
}

/**
* Release existing cached pattern
*/
शून्य UnicodeSet::releasePattern() {
    यदि (pat) {
        uprv_free(pat);
        pat = nullptr;
        patLen = 0;
    }
}

/**
* Set the new pattern to cache.
*/
शून्य UnicodeSet::setPattern(अचर char16_t *newPat, int32_t newPatLen) {
    releasePattern();
    pat = static_cast<char16_t*>(uprv_malloc((newPatLen + 1) * परिमाण(char16_t)));
    यदि (pat) {
        patLen = newPatLen;
        u_memcpy(pat, newPat, patLen);
        pat[patLen] = 0;
    }
    // else we don't care if malloc failed. This was just a nice cache.
    // We can regenerate an equivalent pattern later when requested.
}

UnicodeSet* UnicodeSet::freeze() U_LIFETIME_BOUND {
    यदि(!isFrozen() && !isBogus()) {
        compact();

        // Optimize contains() and span() and similar functions.
        यदि (hasStrings()) {
            stringSpan = नूतन UnicodeSetStringSpan(*एतत्, *strings_, UnicodeSetStringSpan::ALL);
            यदि (stringSpan == nullptr) {
                setToBogus();
                फर्काऊ एतत्;
            } अन्यथा यदि (!stringSpan->needsStringSpanUTF16()) {
                // All strings are irrelevant for span() etc. because
                // all of each string's code points are contained in this set.
                // Do not check needsStringSpanUTF8() because UTF-8 has at most as
                // many relevant strings as UTF-16.
                // (Thus needsStringSpanUTF8() implies needsStringSpanUTF16().)
                delete stringSpan;
                stringSpan = nullptr;
            }
        }
        यदि (stringSpan == nullptr) {
            // No span-relevant strings: Optimize for code point spans.
            bmpSet=नूतन BMPSet(list, len);
            यदि (bmpSet == nullptr) { // Check for memory allocation error.
                setToBogus();
            }
        }
    }
    फर्काऊ एतत्;
}

int32_t UnicodeSet::span(अचर char16_t *s, int32_t length, USetSpanCondition spanCondition) अचर {
    यदि(length>0 && bmpSet!=nullptr) {
        फर्काऊ static_cast<int32_t>(bmpSet->span(s, s + length, spanCondition) - s);
    }
    यदि(length<0) {
        length=u_strlen(s);
    }
    यदि(length==0) {
        फर्काऊ 0;
    }
    यदि(stringSpan!=nullptr) {
        फर्काऊ stringSpan->span(s, length, spanCondition);
    } अन्यथा यदि(hasStrings()) {
        uint32_t which= spanCondition==USET_SPAN_NOT_CONTAINED ?
                            UnicodeSetStringSpan::FWD_UTF16_NOT_CONTAINED :
                            UnicodeSetStringSpan::FWD_UTF16_CONTAINED;
        UnicodeSetStringSpan strSpan(*एतत्, *strings_, which);
        यदि(strSpan.needsStringSpanUTF16()) {
            फर्काऊ strSpan.span(s, length, spanCondition);
        }
    }

    यदि(spanCondition!=USET_SPAN_NOT_CONTAINED) {
        spanCondition=USET_SPAN_CONTAINED;  // Pin to 0/1 values.
    }

    UChar32 c;
    int32_t start=0, prev=0;
    गर {
        U16_NEXT(s, start, length, c);
        यदि(spanCondition!=contains(c)) {
            रोक;
        }
    } जबसम्म((prev=start)<length);
    फर्काऊ prev;
}

int32_t UnicodeSet::spanBack(अचर char16_t *s, int32_t length, USetSpanCondition spanCondition) अचर {
    यदि(length>0 && bmpSet!=nullptr) {
        फर्काऊ static_cast<int32_t>(bmpSet->spanBack(s, s + length, spanCondition) - s);
    }
    यदि(length<0) {
        length=u_strlen(s);
    }
    यदि(length==0) {
        फर्काऊ 0;
    }
    यदि(stringSpan!=nullptr) {
        फर्काऊ stringSpan->spanBack(s, length, spanCondition);
    } अन्यथा यदि(hasStrings()) {
        uint32_t which= spanCondition==USET_SPAN_NOT_CONTAINED ?
                            UnicodeSetStringSpan::BACK_UTF16_NOT_CONTAINED :
                            UnicodeSetStringSpan::BACK_UTF16_CONTAINED;
        UnicodeSetStringSpan strSpan(*एतत्, *strings_, which);
        यदि(strSpan.needsStringSpanUTF16()) {
            फर्काऊ strSpan.spanBack(s, length, spanCondition);
        }
    }

    यदि(spanCondition!=USET_SPAN_NOT_CONTAINED) {
        spanCondition=USET_SPAN_CONTAINED;  // Pin to 0/1 values.
    }

    UChar32 c;
    int32_t prev=length;
    गर {
        U16_PREV(s, 0, length, c);
        यदि(spanCondition!=contains(c)) {
            रोक;
        }
    } जबसम्म((prev=length)>0);
    फर्काऊ prev;
}

int32_t UnicodeSet::spanUTF8(अचर वर्ण *s, int32_t length, USetSpanCondition spanCondition) अचर {
    यदि(length>0 && bmpSet!=nullptr) {
        अचर uint8_t* s0 = reinterpret_cast<अचर uint8_t*>(s);
        फर्काऊ static_cast<int32_t>(bmpSet->spanUTF8(s0, length, spanCondition) - s0);
    }
    यदि(length<0) {
        length = static_cast<int32_t>(uprv_strlen(s));
    }
    यदि(length==0) {
        फर्काऊ 0;
    }
    यदि(stringSpan!=nullptr) {
        फर्काऊ stringSpan->spanUTF8(reinterpret_cast<अचर uint8_t*>(s), length, spanCondition);
    } अन्यथा यदि(hasStrings()) {
        uint32_t which= spanCondition==USET_SPAN_NOT_CONTAINED ?
                            UnicodeSetStringSpan::FWD_UTF8_NOT_CONTAINED :
                            UnicodeSetStringSpan::FWD_UTF8_CONTAINED;
        UnicodeSetStringSpan strSpan(*एतत्, *strings_, which);
        यदि(strSpan.needsStringSpanUTF8()) {
            फर्काऊ strSpan.spanUTF8(reinterpret_cast<अचर uint8_t*>(s), length, spanCondition);
        }
    }

    यदि(spanCondition!=USET_SPAN_NOT_CONTAINED) {
        spanCondition=USET_SPAN_CONTAINED;  // Pin to 0/1 values.
    }

    UChar32 c;
    int32_t start=0, prev=0;
    गर {
        U8_NEXT_OR_FFFD(s, start, length, c);
        यदि(spanCondition!=contains(c)) {
            रोक;
        }
    } जबसम्म((prev=start)<length);
    फर्काऊ prev;
}

int32_t UnicodeSet::spanBackUTF8(अचर वर्ण *s, int32_t length, USetSpanCondition spanCondition) अचर {
    यदि(length>0 && bmpSet!=nullptr) {
        अचर uint8_t* s0 = reinterpret_cast<अचर uint8_t*>(s);
        फर्काऊ bmpSet->spanBackUTF8(s0, length, spanCondition);
    }
    यदि(length<0) {
        length = static_cast<int32_t>(uprv_strlen(s));
    }
    यदि(length==0) {
        फर्काऊ 0;
    }
    यदि(stringSpan!=nullptr) {
        फर्काऊ stringSpan->spanBackUTF8(reinterpret_cast<अचर uint8_t*>(s), length, spanCondition);
    } अन्यथा यदि(hasStrings()) {
        uint32_t which= spanCondition==USET_SPAN_NOT_CONTAINED ?
                            UnicodeSetStringSpan::BACK_UTF8_NOT_CONTAINED :
                            UnicodeSetStringSpan::BACK_UTF8_CONTAINED;
        UnicodeSetStringSpan strSpan(*एतत्, *strings_, which);
        यदि(strSpan.needsStringSpanUTF8()) {
            फर्काऊ strSpan.spanBackUTF8(reinterpret_cast<अचर uint8_t*>(s), length, spanCondition);
        }
    }

    यदि(spanCondition!=USET_SPAN_NOT_CONTAINED) {
        spanCondition=USET_SPAN_CONTAINED;  // Pin to 0/1 values.
    }

    UChar32 c;
    int32_t prev=length;
    गर {
        U8_PREV_OR_FFFD(s, 0, length, c);
        यदि(spanCondition!=contains(c)) {
            रोक;
        }
    } जबसम्म((prev=length)>0);
    फर्काऊ prev;
}

U_NAMESPACE_END
