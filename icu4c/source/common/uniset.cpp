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

#समावेश "unicode/utypes.h"
#समावेश "unicode/parsepos.h"
#समावेश "unicode/symtable.h"
#समावेश "unicode/uniset.h"
#समावेश "unicode/ustring.h"
#समावेश "unicode/utf8.h"
#समावेश "unicode/utf16.h"
#समावेश "ruleiter.h"
#समावेश "cmemory.h"
#समावेश "cstring.h"
#समावेश "patternprops.h"
#समावेश "uelement.h"
#समावेश "util.h"
#समावेश "uvector.h"
#समावेश "charstr.h"
#समावेश "ustrfmt.h"
#समावेश "uassert.h"
#समावेश "bmpset.h"
#समावेश "unisetspan.h"

// HIGH_VALUE > all valid values. 110000 for codepoints
#घोषणा UNICODESET_ऊपरी 0x0110000

// LOW <= all valid values. ZERO for codepoints
#घोषणा UNICODESET_निचला 0x000000

/** Max list [0, 1, 2, ..., max code point, HIGH] */
constexpr int32_t MAX_लंबाई = UNICODESET_ऊपरी + 1;

U_NAMESPACE_BEGIN

प्रतीक_Table::~प्रतीक_Table() {}

UOBJECT_DEFINE_RTTI_IMPLEMENTATION(UnicodeSet)

/**
 * Modify the given UChar32 variable so that it is in range, by
 * pinning values < UNICODESET_LOW to UNICODESET_LOW, and
 * pinning values > UNICODESET_HIGH-1 to UNICODESET_HIGH-1.
 * It modifies its argument in-place and also returns it.
 */
अटल अंतरभूत UChar32 pinCodePoint(UChar32& c) {
    अगर (c < UNICODESET_निचला) {
        c = UNICODESET_निचला;
    } अन्यथा अगर (c > (UNICODESET_ऊपरी-1)) {
        c = (UNICODESET_ऊपरी-1);
    }
    वापस c;
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

#अगर_घोषित DEBUG_MEM
#समावेश <stdio.h>
अटल int32_t dbg_गिनती = 0;

अटल अंतरभूत व्योम _dbgct(UnicodeSet* set) {
    UnicodeString str;
    set->toPattern(str, true);
    अक्षर buf[40];
    str.extract(0, 39, buf, "");
    म_लिखो("DEBUG UnicodeSet: ct 0x%08X; %d %s\n", set, ++dbg_गिनती, buf);
}

अटल अंतरभूत व्योम _dbgdt(UnicodeSet* set) {
    UnicodeString str;
    set->toPattern(str, true);
    अक्षर buf[40];
    str.extract(0, 39, buf, "");
    म_लिखो("DEBUG UnicodeSet: dt 0x%08X; %d %s\n", set, --dbg_गिनती, buf);
}

#अन्यथा

#घोषणा _dbgct(set)
#घोषणा _dbgdt(set)

#पूर्ण_अगर

//----------------------------------------------------------------
// UnicodeString in UVector support
//----------------------------------------------------------------

अटल व्योम U_CALLCONV cloneUnicodeString(UElement *dst, UElement *src) {
    dst->pointer = नया UnicodeString(*static_cast<UnicodeString*>(src->pointer));
}

अटल int32_t U_CALLCONV compareUnicodeString(UElement t1, UElement t2) {
    स्थिर UnicodeString& a = *static_cast<स्थिर UnicodeString*>(t1.pointer);
    स्थिर UnicodeString& b = *static_cast<स्थिर UnicodeString*>(t2.pointer);
    वापस a.compare(b);
}

UBool UnicodeSet::hasStrings() स्थिर {
    वापस strings_ != nullptr && !strings_->isEmpty();
}

int32_t UnicodeSet::strings_आकार() स्थिर {
    वापस strings_ == nullptr ? 0 : strings_->size();
}

UBool UnicodeSet::stringsContains(स्थिर UnicodeString &s) स्थिर {
    वापस strings_ != nullptr && strings_->contains((व्योम*) &s);
}

//----------------------------------------------------------------
// Constructors &c
//----------------------------------------------------------------

/**
 * Constructs an empty set.
 */
UnicodeSet::UnicodeSet() {
    list[0] = UNICODESET_ऊपरी;
    _dbgct(यह);
}

/**
 * Constructs a set containing the given range. If <code>end >
 * start</code> then an empty set is created.
 *
 * @param start first character, inclusive, of range
 * @param end last character, inclusive, of range
 */
UnicodeSet::UnicodeSet(UChar32 start, UChar32 end) {
    list[0] = UNICODESET_ऊपरी;
    add(start, end);
    _dbgct(यह);
}

/**
 * Constructs a set that is identical to the given UnicodeSet.
 */
UnicodeSet::UnicodeSet(स्थिर UnicodeSet& o) : UnicodeFilter(o) {
    *यह = o;
    _dbgct(यह);
}

// Copy-construct as thawed.
UnicodeSet::UnicodeSet(स्थिर UnicodeSet& o, UBool /* asThawed */) : UnicodeFilter(o) {
    अगर (ensureCapacity(o.len)) {
        // *this = o except for bmpSet and stringSpan
        len = o.len;
        uprv_memcpy(list, o.list, (माप_प्रकार)len*माप(UChar32));
        अगर (o.hasStrings()) {
            UErrorCode status = U_ZERO_ERROR;
            अगर (!allocateStrings(status) ||
                    (strings_->assign(*o.strings_, cloneUnicodeString, status), U_FAILURE(status))) {
                setToBogus();
                वापस;
            }
        }
        अगर (o.pat) {
            setPattern(o.pat, o.patLen);
        }
        _dbgct(यह);
    }
}

/**
 * Destructs the set.
 */
UnicodeSet::~UnicodeSet() {
    _dbgdt(यह); // first!
    अगर (list != ढेर_List) {
        uprv_free(list);
    }
    मिटाओ bmpSet;
    अगर (बफर != ढेर_List) {
        uprv_free(बफर);
    }
    मिटाओ strings_;
    मिटाओ stringSpan;
    releasePattern();
}

/**
 * Assigns this object to be a copy of another.
 */
UnicodeSet& UnicodeSet::चालक=(स्थिर UnicodeSet& o) U_LIFETIME_BOUND {
    वापस copyFrom(o, false);
}

UnicodeSet& UnicodeSet::copyFrom(स्थिर UnicodeSet& o, UBool asThawed) U_LIFETIME_BOUND {
    अगर (यह == &o) {
        वापस *यह;
    }
    अगर (isFrozen()) {
        वापस *यह;
    }
    अगर (o.isBogus()) {
        setToBogus();
        वापस *यह;
    }
    अगर (!ensureCapacity(o.len)) {
        // ensureCapacity will mark the UnicodeSet as Bogus if OOM failure happens.
        वापस *यह;
    }
    len = o.len;
    uprv_memcpy(list, o.list, (माप_प्रकार)len*माप(UChar32));
    अगर (o.bmpSet != nullptr && !asThawed) {
        bmpSet = नया BMPSet(*o.bmpSet, list, len);
        अगर (bmpSet == nullptr) { // Check for memory allocation error.
            setToBogus();
            वापस *यह;
        }
    }
    अगर (o.hasStrings()) {
        UErrorCode status = U_ZERO_ERROR;
        अगर ((strings_ == nullptr && !allocateStrings(status)) ||
                (strings_->assign(*o.strings_, cloneUnicodeString, status), U_FAILURE(status))) {
            setToBogus();
            वापस *यह;
        }
    } अन्यथा अगर (hasStrings()) {
        strings_->removeAllElements();
    }
    अगर (o.stringSpan != nullptr && strings_ != nullptr && !asThawed) {
        stringSpan = नया UnicodeSetStringSpan(*o.stringSpan, *strings_);
        अगर (stringSpan == nullptr) { // Check for memory allocation error.
            setToBogus();
            वापस *यह;
        }
    }
    releasePattern();
    अगर (o.pat) {
        setPattern(o.pat, o.patLen);
    }
    वापस *यह;
}

/**
 * Returns a copy of this object.  All UnicodeMatcher objects have
 * to support cloning in order to allow classes using
 * UnicodeMatchers, such as Transliterator, to implement cloning.
 */
UnicodeSet* UnicodeSet::clone() स्थिर {
    वापस नया UnicodeSet(*यह);
}

UnicodeSet *UnicodeSet::cloneAsThawed() स्थिर {
    वापस नया UnicodeSet(*यह, true);
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
bool UnicodeSet::चालक==(स्थिर UnicodeSet& o) स्थिर {
    अगर (len != o.len) वापस false;
    क्रम (int32_t i = 0; i < len; ++i) {
        अगर (list[i] != o.list[i]) वापस false;
    }
    अगर (hasStrings() != o.hasStrings()) { वापस false; }
    अगर (hasStrings() && *strings_ != *o.strings_) वापस false;
    वापस true;
}

/**
 * Returns the hash code value for this set.
 *
 * @return the hash code value for this set.
 * @see Object#hashCode()
 */
int32_t UnicodeSet::hashCode() स्थिर {
    uint32_t परिणाम = static_cast<uint32_t>(len);
    क्रम (int32_t i = 0; i < len; ++i) {
        परिणाम *= 1000003u;
        परिणाम += list[i];
    }
    वापस static_cast<int32_t>(परिणाम);
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
int32_t UnicodeSet::size() स्थिर {
    int32_t n = 0;
    int32_t count = get_Range_गिनती();
    क्रम (int32_t i = 0; i < count; ++i) {
        n += getRangeEnd(i) - getRangeStart(i) + 1;
    }
    वापस n + strings_आकार();
}

/**
 * Returns <tt>true</tt> if this set contains no elements.
 *
 * @return <tt>true</tt> if this set contains no elements.
 */
UBool UnicodeSet::isEmpty() स्थिर {
    वापस len == 1 && !hasStrings();
}

/**
 * Returns true if this set contains the given character.
 * @param c character to be checked for containment
 * @return true if the test condition is met
 */
UBool UnicodeSet::contains(UChar32 c) स्थिर {
    // Set i to the index of the start item greater than ch
    // We know we will terminate without length test!
    // LATER: for large sets, add binary search
    //int32_t i = -1;
    //for (;;) {
    //    if (c < list[++i]) break;
    //}
    अगर (bmpSet != nullptr) {
        वापस bmpSet->contains(c);
    }
    अगर (stringSpan != nullptr) {
        वापस stringSpan->contains(c);
    }
    अगर (c >= UNICODESET_ऊपरी) { // Don't need to check LOW bound
        वापस false;
    }
    int32_t i = findCodePoint(c);
    वापस i & 1; // return true if odd
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
int32_t UnicodeSet::findCodePoint(UChar32 c) स्थिर {
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
    अगर (c < list[0])
        वापस 0;
    // High runner test.  c is often after the last range, so an
    // initial check for this condition pays off.
    int32_t lo = 0;
    int32_t hi = len - 1;
    अगर (lo >= hi || c >= list[hi-1])
        वापस hi;
    // invariant: c >= list[lo]
    // invariant: c < list[hi]
    क्रम (;;) {
        int32_t i = (lo + hi) >> 1;
        अगर (i == lo) {
            अवरोध; // Found!
        } अन्यथा अगर (c < list[i]) {
            hi = i;
        } अन्यथा {
            lo = i;
        }
    }
    वापस hi;
}

/**
 * Returns true if this set contains every character
 * of the given range.
 * @param start first character, inclusive, of the range
 * @param end last character, inclusive, of the range
 * @return true if the test condition is met
 */
UBool UnicodeSet::contains(UChar32 start, UChar32 end) स्थिर {
    //int32_t i = -1;
    //for (;;) {
    //    if (start < list[++i]) break;
    //}
    int32_t i = findCodePoint(start);
    वापस ((i & 1) != 0 && end < list[i]);
}

/**
 * Returns <tt>true</tt> if this set contains the given
 * multicharacter string.
 * @param s string to be checked for containment
 * @return <tt>true</tt> if this set contains the specified string
 */
UBool UnicodeSet::contains(स्थिर UnicodeString& s) स्थिर {
    int32_t cp = getSingleCP(s);
    अगर (cp < 0) {
        वापस stringsContains(s);
    } अन्यथा {
        वापस contains(static_cast<UChar32>(cp));
    }
}

/**
 * Returns true if this set contains all the characters and strings
 * of the given set.
 * @param c set to be checked for containment
 * @return true if the test condition is met
 */
UBool UnicodeSet::containsAll(स्थिर UnicodeSet& c) स्थिर {
    // The specified set is a subset if all of its pairs are contained in
    // this set.  It's possible to code this more efficiently in terms of
    // direct manipulation of the inversion lists if the need arises.
    int32_t n = c.get_Range_गिनती();
    क्रम (पूर्णांक i=0; i<n; ++i) {
        अगर (!contains(c.getRangeStart(i), c.getRangeEnd(i))) {
            वापस false;
        }
    }
    वापस !c.hasStrings() || (strings_ != nullptr && strings_->containsAll(*c.strings_));
}

/**
 * Returns true if this set contains all the characters
 * of the given string.
 * @param s string containing characters to be checked for containment
 * @return true if the test condition is met
 */
UBool UnicodeSet::containsAll(स्थिर UnicodeString& s) स्थिर {
    वापस span(s.get_बफर(), s.length(), USET_SPAN_CONTAINED) == s.length();
}

/**
 * Returns true if this set contains none of the characters
 * of the given range.
 * @param start first character, inclusive, of the range
 * @param end last character, inclusive, of the range
 * @return true if the test condition is met
 */
UBool UnicodeSet::containsNone(UChar32 start, UChar32 end) स्थिर {
    //int32_t i = -1;
    //for (;;) {
    //    if (start < list[++i]) break;
    //}
    int32_t i = findCodePoint(start);
    वापस ((i & 1) == 0 && end < list[i]);
}

/**
 * Returns true if this set contains none of the characters and strings
 * of the given set.
 * @param c set to be checked for containment
 * @return true if the test condition is met
 */
UBool UnicodeSet::containsNone(स्थिर UnicodeSet& c) स्थिर {
    // The specified set is a subset if all of its pairs are contained in
    // this set.  It's possible to code this more efficiently in terms of
    // direct manipulation of the inversion lists if the need arises.
    int32_t n = c.get_Range_गिनती();
    क्रम (int32_t i=0; i<n; ++i) {
        अगर (!containsNone(c.getRangeStart(i), c.getRangeEnd(i))) {
            वापस false;
        }
    }
    वापस strings_ == nullptr || !c.hasStrings() || strings_->containsNone(*c.strings_);
}

/**
 * Returns true if this set contains none of the characters
 * of the given string.
 * @param s string containing characters to be checked for containment
 * @return true if the test condition is met
 */
UBool UnicodeSet::containsNone(स्थिर UnicodeString& s) स्थिर {
    वापस span(s.get_बफर(), s.length(), USET_SPAN_NOT_CONTAINED) == s.length();
}

/**
 * Returns <tt>true</tt> if this set contains any character whose low byte
 * is the given value.  This is used by <tt>RuleBasedTransliterator</tt> for
 * indexing.
 */
UBool UnicodeSet::matches_सूचकांक_मूल्य(uint8_t v) स्थिर {
    /* The index value v, in the range [0,255], is contained in this set if
     * it is contained in any pair of this set.  Pairs either have the high
     * bytes equal, or unequal.  If the high bytes are equal, then we have
     * aaxx..aayy, where aa is the high byte.  Then v is contained if xx <=
     * v <= yy.  If the high bytes are unequal we have aaxx..bbyy, bb>aa.
     * Then v is contained if xx <= v || v <= yy.  (This is identical to the
     * time zone month containment logic.)
     */
    int32_t i;
    int32_t range_गिनती=get_Range_गिनती();
    क्रम (i=0; i<range_गिनती; ++i) {
        UChar32 निचला = getRangeStart(i);
        UChar32 ऊपरी = getRangeEnd(i);
        अगर ((निचला & ~0xFF) == (ऊपरी & ~0xFF)) {
            अगर ((निचला & 0xFF) <= v && v <= (ऊपरी & 0xFF)) {
                वापस true;
            }
        } अन्यथा अगर ((निचला & 0xFF) <= v || v <= (ऊपरी & 0xFF)) {
            वापस true;
        }
    }
    अगर (hasStrings()) {
        क्रम (i=0; i<strings_->size(); ++i) {
            स्थिर UnicodeString& s = *static_cast<स्थिर UnicodeString*>(strings_->elementAt(i));
            अगर (s.isEmpty()) {
                जारी;  // skip the empty string
            }
            UChar32 c = s.char32At(0);
            अगर ((c & 0xFF) == v) {
                वापस true;
            }
        }
    }
    वापस false;
}

/**
 * Implementation of UnicodeMatcher::matches().  Always matches the
 * longest possible multichar string.
 */
UMatchDegree UnicodeSet::matches(स्थिर Replaceable& text,
                                 int32_t& offset,
                                 int32_t limit,
                                 UBool incremental) {
    अगर (offset == limit) {
        अगर (contains(U_ETHER)) {
            वापस incremental ? U_PARTIAL_MATCH : U_MATCH;
        } अन्यथा {
            वापस U_MISMATCH;
        }
    } अन्यथा {
        अगर (hasStrings()) { // try strings first

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
            int32_t ऊपरी_Water_लंबाई = 0;

            क्रम (i=0; i<strings_->size(); ++i) {
                स्थिर UnicodeString& trial = *static_cast<स्थिर UnicodeString*>(strings_->elementAt(i));
                अगर (trial.isEmpty()) {
                    जारी;  // skip the empty string
                }

                char16_t c = trial.charAt(forward ? 0 : trial.length() - 1);

                // Strings are sorted, so we can optimize in the
                // forward direction.
                अगर (forward && c > firstChar) अवरोध;
                अगर (c != firstChar) जारी;

                int32_t matchLen = matchRest(text, offset, limit, trial);

                अगर (incremental) {
                    int32_t maxLen = forward ? limit-offset : offset-limit;
                    अगर (matchLen == maxLen) {
                        // We have successfully matched but only up to limit.
                        वापस U_PARTIAL_MATCH;
                    }
                }

                अगर (matchLen == trial.length()) {
                    // We have successfully matched the whole string.
                    अगर (matchLen > ऊपरी_Water_लंबाई) {
                        ऊपरी_Water_लंबाई = matchLen;
                    }
                    // In the forward direction we know strings
                    // are sorted so we can bail early.
                    अगर (forward && matchLen < ऊपरी_Water_लंबाई) {
                        अवरोध;
                    }
                    जारी;
                }
            }

            // We've checked all strings without a partial match.
            // If we have full matches, return the longest one.
            अगर (ऊपरी_Water_लंबाई != 0) {
                offset += forward ? ऊपरी_Water_लंबाई : -ऊपरी_Water_लंबाई;
                वापस U_MATCH;
            }
        }
        वापस UnicodeFilter::matches(text, offset, limit, incremental);
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
int32_t UnicodeSet::matchRest(स्थिर Replaceable& text,
                              int32_t start, int32_t limit,
                              स्थिर UnicodeString& s) {
    int32_t i;
    int32_t maxLen;
    int32_t slen = s.length();
    अगर (start < limit) {
        maxLen = limit - start;
        अगर (maxLen > slen) maxLen = slen;
        क्रम (i = 1; i < maxLen; ++i) {
            अगर (text.charAt(start + i) != s.charAt(i)) वापस 0;
        }
    } अन्यथा {
        maxLen = start - limit;
        अगर (maxLen > slen) maxLen = slen;
        --slen; // <=> slen = s.length() - 1;
        क्रम (i = 1; i < maxLen; ++i) {
            अगर (text.charAt(start - i) != s.charAt(slen - i)) वापस 0;
        }
    }
    वापस maxLen;
}

/**
 * Implement of UnicodeMatcher
 */
व्योम UnicodeSet::addMatchSetTo(UnicodeSet& toUnionTo) स्थिर {
    toUnionTo.addAll(*यह);
}

/**
 * Returns the index of the given character within this set, where
 * the set is ordered by ascending code point.  If the character
 * is not in this set, return -1.  The inverse of this method is
 * <code>charAt()</code>.
 * @return an index from 0..size()-1, or -1
 */
int32_t UnicodeSet::सूचकांक_Of(UChar32 c) स्थिर {
    अगर (c < MIN_मूल्य || c > MAX_मूल्य) {
        वापस -1;
    }
    int32_t i = 0;
    int32_t n = 0;
    क्रम (;;) {
        UChar32 start = list[i++];
        अगर (c < start) {
            वापस -1;
        }
        UChar32 limit = list[i++];
        अगर (c < limit) {
            वापस n + c - start;
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
UChar32 UnicodeSet::charAt(int32_t सूचकांक) स्थिर {
    अगर (सूचकांक >= 0) {
        // len2 is the largest even integer <= len, that is, it is len
        // for even values and len-1 for odd values.  With odd values
        // the last entry is UNICODESET_HIGH.
        int32_t len2 = len & ~1;
        क्रम (int32_t i=0; i < len2;) {
            UChar32 start = list[i++];
            int32_t count = list[i++] - start;
            अगर (सूचकांक < count) {
                वापस static_cast<UChar32>(start + सूचकांक);
            }
            सूचकांक -= count;
        }
    }
    वापस static_cast<UChar32>(-1);
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
    सफा();
    complement(start, end);
    वापस *यह;
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
    अगर (pinCodePoint(start) < pinCodePoint(end)) {
        UChar32 limit = end + 1;
        // Fast path for adding a new range after the last one.
        // Odd list length: [..., lastStart, lastLimit, HIGH]
        अगर ((len & 1) != 0) {
            // If the list is empty, set lastLimit low enough to not be adjacent to 0.
            UChar32 lastLimit = len == 1 ? -2 : list[len - 2];
            अगर (lastLimit <= start && !isFrozen() && !isBogus()) {
                अगर (lastLimit == start) {
                    // Extend the last range.
                    list[len - 2] = limit;
                    अगर (limit == UNICODESET_ऊपरी) {
                        --len;
                    }
                } अन्यथा {
                    list[len - 1] = start;
                    अगर (limit < UNICODESET_ऊपरी) {
                        अगर (ensureCapacity(len + 2)) {
                            list[len++] = limit;
                            list[len++] = UNICODESET_ऊपरी;
                        }
                    } अन्यथा {  // limit == UNICODESET_HIGH
                        अगर (ensureCapacity(len + 1)) {
                            list[len++] = UNICODESET_ऊपरी;
                        }
                    }
                }
                releasePattern();
                वापस *यह;
            }
        }
        // This is slow. Could be much faster using findCodePoint(start)
        // and modifying the list, dealing with adjacent & overlapping ranges.
        UChar32 range[3] = { start, limit, UNICODESET_ऊपरी };
        add(range, 2, 0);
    } अन्यथा अगर (start == end) {
        add(start);
    }
    वापस *यह;
}

// #define DEBUG_US_ADD

#अगर_घोषित DEBUG_US_ADD
#समावेश <stdio.h>
व्योम dump(UChar32 c) {
    अगर (c <= 0xFF) {
        म_लिखो("%c", (अक्षर)c);
    } अन्यथा {
        म_लिखो("U+%04X", c);
    }
}
व्योम dump(स्थिर UChar32* list, int32_t len) {
    म_लिखो("[");
    क्रम (int32_t i=0; i<len; ++i) {
        अगर (i != 0) म_लिखो(", ");
        dump(list[i]);
    }
    म_लिखो("]");
}
#पूर्ण_अगर

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
    अगर ((i & 1) != 0  || isFrozen() || isBogus()) वापस *यह;

    // HIGH is 0x110000
    // assert(list[len-1] == HIGH);

    // empty = [HIGH]
    // [start_0, limit_0, start_1, limit_1, HIGH]

    // [..., start_k-1, limit_k-1, start_k, limit_k, ..., HIGH]
    //                             ^
    //                             list[i]

    // i == 0 means c is before the first range

#अगर_घोषित DEBUG_US_ADD
    म_लिखो("Add of ");
    dump(c);
    म_लिखो(" found at %d", i);
    म_लिखो(": ");
    dump(list, len);
    म_लिखो(" => ");
#पूर्ण_अगर

    अगर (c == list[i]-1) {
        // c is before start of next range
        list[i] = c;
        // if we touched the HIGH mark, then add a new one
        अगर (c == (UNICODESET_ऊपरी - 1)) {
            अगर (!ensureCapacity(len+1)) {
                // ensureCapacity will mark the object as Bogus if OOM failure happens.
                वापस *यह;
            }
            list[len++] = UNICODESET_ऊपरी;
        }
        अगर (i > 0 && c == list[i-1]) {
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
            जबतक (src < srclimit) *(dst++) = *(src++);

            len -= 2;
        }
    }

    अन्यथा अगर (i > 0 && c == list[i-1]) {
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

        अगर (!ensureCapacity(len+2)) {
            // ensureCapacity will mark the object as Bogus if OOM failure happens.
            वापस *यह;
        }

        UChar32 *p = list + i;
        uprv_memmove(p + 2, p, (len - i) * माप(*p));
        list[i] = c;
        list[i+1] = c+1;
        len += 2;
    }

#अगर_घोषित DEBUG_US_ADD
    dump(list, len);
    म_लिखो("\n");

    क्रम (i=1; i<len; ++i) {
        अगर (list[i] <= list[i-1]) {
            // Corrupt array!
            म_लिखो("ERROR: list has been corrupted\n");
            निकास(1);
        }
    }
#पूर्ण_अगर

    releasePattern();
    वापस *यह;
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
UnicodeSet& UnicodeSet::add(स्थिर UnicodeString& s) U_LIFETIME_BOUND {
    अगर (isFrozen() || isBogus()) वापस *यह;
    int32_t cp = getSingleCP(s);
    अगर (cp < 0) {
        अगर (!stringsContains(s)) {
            _add(s);
            releasePattern();
        }
    } अन्यथा {
        add(static_cast<UChar32>(cp));
    }
    वापस *यह;
}

/**
 * Adds the given string, in order, to 'strings_'.  The given string
 * must have been checked by the caller to not already be in 'strings_'.
 */
व्योम UnicodeSet::_add(स्थिर UnicodeString& s) {
    अगर (isFrozen() || isBogus()) {
        वापस;
    }
    UErrorCode ec = U_ZERO_ERROR;
    अगर (strings_ == nullptr && !allocateStrings(ec)) {
        setToBogus();
        वापस;
    }
    LocalPointer<UnicodeString> t(नया UnicodeString(s));
    अगर (t.isNull()) { // Check for memory allocation error.
        setToBogus();
        वापस;
    }
    strings_->sorted_जोड़ो(t.orphan(), compareUnicodeString, ec);
    अगर (U_FAILURE(ec)) {
        setToBogus();
    }
}

/**
 * @return a code point IF the string consists of a single one.
 * otherwise returns -1.
 * @param string to test
 */
int32_t UnicodeSet::getSingleCP(स्थिर UnicodeString& s) {
    int32_t s_लंबाई = s.length();
    अगर (s_लंबाई == 1) वापस s.charAt(0);
    अगर (s_लंबाई == 2) {
        UChar32 cp = s.char32At(0);
        अगर (cp > 0xFFFF) { // is surrogate pair
            वापस cp;
        }
    }
    वापस -1;
}

/**
 * Adds each of the characters in this string to the set. Thus "ch" => {"c", "h"}
 * If this set already any particular character, it has no effect on that character.
 * @param the source string
 * @return the modified set, for chaining
 */
UnicodeSet& UnicodeSet::addAll(स्थिर UnicodeString& s) U_LIFETIME_BOUND {
    UChar32 cp;
    क्रम (int32_t i = 0; i < s.length(); i += U16_लंबाई(cp)) {
        cp = s.char32At(i);
        add(cp);
    }
    वापस *यह;
}

/**
 * Retains EACH of the characters in this string. Note: "ch" == {"c", "h"}
 * If this set already any particular character, it has no effect on that character.
 * @param the source string
 * @return the modified set, for chaining
 */
UnicodeSet& UnicodeSet::retainAll(स्थिर UnicodeString& s) U_LIFETIME_BOUND {
    UnicodeSet set;
    set.addAll(s);
    retainAll(set);
    वापस *यह;
}

/**
 * Complement EACH of the characters in this string. Note: "ch" == {"c", "h"}
 * If this set already any particular character, it has no effect on that character.
 * @param the source string
 * @return the modified set, for chaining
 */
UnicodeSet& UnicodeSet::complementAll(स्थिर UnicodeString& s) U_LIFETIME_BOUND {
    UnicodeSet set;
    set.addAll(s);
    complementAll(set);
    वापस *यह;
}

/**
 * Remove EACH of the characters in this string. Note: "ch" == {"c", "h"}
 * If this set already any particular character, it has no effect on that character.
 * @param the source string
 * @return the modified set, for chaining
 */
UnicodeSet& UnicodeSet::removeAll(स्थिर UnicodeString& s) U_LIFETIME_BOUND {
    UnicodeSet set;
    set.addAll(s);
    removeAll(set);
    वापस *यह;
}

UnicodeSet& UnicodeSet::removeAllStrings() U_LIFETIME_BOUND {
    अगर (!isFrozen() && hasStrings()) {
        strings_->removeAllElements();
        releasePattern();
    }
    वापस *यह;
}


/**
 * Makes a set from a multicharacter string. Thus "ch" => {"ch"}
 * <br><b>Warning: you cannot add an empty string ("") to a UnicodeSet.</b>
 * @param the source string
 * @return a newly created set containing the given string
 */
UnicodeSet* U_EXPORT2 UnicodeSet::createFrom(स्थिर UnicodeString& s) {
    UnicodeSet *set = नया UnicodeSet();
    अगर (set != nullptr) { // Check for memory allocation error.
        set->add(s);
    }
    वापस set;
}


/**
 * Makes a set from each of the characters in the string. Thus "ch" => {"c", "h"}
 * @param the source string
 * @return a newly created set containing the given characters
 */
UnicodeSet* U_EXPORT2 UnicodeSet::createFromAll(स्थिर UnicodeString& s) {
    UnicodeSet *set = नया UnicodeSet();
    अगर (set != nullptr) { // Check for memory allocation error.
        set->addAll(s);
    }
    वापस set;
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
    अगर (pinCodePoint(start) <= pinCodePoint(end)) {
        UChar32 range[3] = { start, end+1, UNICODESET_ऊपरी };
        retain(range, 2, 0);
    } अन्यथा {
        सफा();
    }
    वापस *यह;
}

UnicodeSet& UnicodeSet::retain(UChar32 c) U_LIFETIME_BOUND {
    वापस retain(c, c);
}

UnicodeSet& UnicodeSet::retain(स्थिर UnicodeString &s) U_LIFETIME_BOUND {
    अगर (isFrozen() || isBogus()) { वापस *यह; }
    UChar32 cp = getSingleCP(s);
    अगर (cp < 0) {
        bool isIn = stringsContains(s);
        // Check for getRangeCount() first to avoid somewhat-expensive size()
        // when there are single code points.
        अगर (isIn && get_Range_गिनती() == 0 && size() == 1) {
            वापस *यह;
        }
        सफा();
        अगर (isIn) {
            _add(s);
        }
    } अन्यथा {
        retain(cp, cp);
    }
    वापस *यह;
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
    अगर (pinCodePoint(start) <= pinCodePoint(end)) {
        UChar32 range[3] = { start, end+1, UNICODESET_ऊपरी };
        retain(range, 2, 2);
    }
    वापस *यह;
}

/**
 * Removes the specified character from this set if it is present.
 * The set will not contain the specified range once the call
 * returns.
 */
UnicodeSet& UnicodeSet::हटाओ(UChar32 c) U_LIFETIME_BOUND {
    वापस हटाओ(c, c);
}

/**
 * Removes the specified string from this set if it is present.
 * The set will not contain the specified character once the call
 * returns.
 * @param the source string
 * @return the modified set, for chaining
 */
UnicodeSet& UnicodeSet::हटाओ(स्थिर UnicodeString& s) U_LIFETIME_BOUND {
    अगर (isFrozen() || isBogus()) वापस *यह;
    int32_t cp = getSingleCP(s);
    अगर (cp < 0) {
        अगर (strings_ != nullptr && strings_->removeElement((व्योम*) &s)) {
            releasePattern();
        }
    } अन्यथा {
        हटाओ(static_cast<UChar32>(cp), static_cast<UChar32>(cp));
    }
    वापस *यह;
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
    अगर (isFrozen() || isBogus()) {
        वापस *यह;
    }
    अगर (pinCodePoint(start) <= pinCodePoint(end)) {
        UChar32 range[3] = { start, end+1, UNICODESET_ऊपरी };
        exclusiveOr(range, 2, 0);
    }
    releasePattern();
    वापस *यह;
}

UnicodeSet& UnicodeSet::complement(UChar32 c) U_LIFETIME_BOUND {
    वापस complement(c, c);
}

/**
 * This is equivalent to
 * <code>complement(MIN_VALUE, MAX_VALUE)</code>.
 */
UnicodeSet& UnicodeSet::complement() U_LIFETIME_BOUND {
    अगर (isFrozen() || isBogus()) {
        वापस *यह;
    }
    अगर (list[0] == UNICODESET_निचला) {
        uprv_memmove(list, list + 1, (माप_प्रकार)(len-1)*माप(UChar32));
        --len;
    } अन्यथा {
        अगर (!ensureCapacity(len+1)) {
            वापस *यह;
        }
        uprv_memmove(list + 1, list, (माप_प्रकार)len*माप(UChar32));
        list[0] = UNICODESET_निचला;
        ++len;
    }
    releasePattern();
    वापस *यह;
}

/**
 * Complement the specified string in this set.
 * The set will not contain the specified string once the call
 * returns.
 *
 * @param s the string to complement
 * @return this object, for chaining
 */
UnicodeSet& UnicodeSet::complement(स्थिर UnicodeString& s) U_LIFETIME_BOUND {
    अगर (isFrozen() || isBogus()) वापस *यह;
    int32_t cp = getSingleCP(s);
    अगर (cp < 0) {
        अगर (stringsContains(s)) {
            strings_->removeElement((व्योम*) &s);
        } अन्यथा {
            _add(s);
        }
        releasePattern();
    } अन्यथा {
        complement(static_cast<UChar32>(cp), static_cast<UChar32>(cp));
    }
    वापस *यह;
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
UnicodeSet& UnicodeSet::addAll(स्थिर UnicodeSet& c) U_LIFETIME_BOUND {
    अगर ( c.len>0 && c.list!=nullptr ) {
        add(c.list, c.len, 0);
    }

    // Add strings in order
    अगर ( c.strings_!=nullptr ) {
        क्रम (int32_t i=0; i<c.strings_->size(); ++i) {
            स्थिर UnicodeString* s = static_cast<स्थिर UnicodeString*>(c.strings_->elementAt(i));
            अगर (!stringsContains(*s)) {
                _add(*s);
            }
        }
    }
    वापस *यह;
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
UnicodeSet& UnicodeSet::retainAll(स्थिर UnicodeSet& c) U_LIFETIME_BOUND {
    अगर (isFrozen() || isBogus()) {
        वापस *यह;
    }
    retain(c.list, c.len, 0);
    अगर (hasStrings()) {
        अगर (!c.hasStrings()) {
            strings_->removeAllElements();
        } अन्यथा {
            strings_->retainAll(*c.strings_);
        }
    }
    वापस *यह;
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
UnicodeSet& UnicodeSet::removeAll(स्थिर UnicodeSet& c) U_LIFETIME_BOUND {
    अगर (isFrozen() || isBogus()) {
        वापस *यह;
    }
    retain(c.list, c.len, 2);
    अगर (hasStrings() && c.hasStrings()) {
        strings_->removeAll(*c.strings_);
    }
    वापस *यह;
}

/**
 * Complements in this set all elements contained in the specified
 * set.  Any character in the other set will be removed if it is
 * in this set, or will be added if it is not in this set.
 *
 * @param c set that defines which elements will be xor'ed from
 *          this set.
 */
UnicodeSet& UnicodeSet::complementAll(स्थिर UnicodeSet& c) U_LIFETIME_BOUND {
    अगर (isFrozen() || isBogus()) {
        वापस *यह;
    }
    exclusiveOr(c.list, c.len, 0);

    अगर (c.strings_ != nullptr) {
        क्रम (int32_t i=0; i<c.strings_->size(); ++i) {
            व्योम* e = c.strings_->elementAt(i);
            अगर (strings_ == nullptr || !strings_->removeElement(e)) {
                _add(*static_cast<स्थिर UnicodeString*>(e));
            }
        }
    }
    वापस *यह;
}

/**
 * Removes all of the elements from this set.  This set will be
 * empty after this call returns.
 */
UnicodeSet& UnicodeSet::सफा() U_LIFETIME_BOUND {
    अगर (isFrozen()) {
        वापस *यह;
    }
    list[0] = UNICODESET_ऊपरी;
    len = 1;
    releasePattern();
    अगर (strings_ != nullptr) {
        strings_->removeAllElements();
    }
    // Remove bogus
    fFlags = 0;
    वापस *यह;
}

/**
 * Iteration method that returns the number of ranges contained in
 * this set.
 * @see #getRangeStart
 * @see #getRangeEnd
 */
int32_t UnicodeSet::get_Range_गिनती() स्थिर {
    वापस len/2;
}

/**
 * Iteration method that returns the first character in the
 * specified range of this set.
 * @see #getRangeCount
 * @see #getRangeEnd
 */
UChar32 UnicodeSet::getRangeStart(int32_t सूचकांक) स्थिर {
    वापस list[सूचकांक*2];
}

/**
 * Iteration method that returns the last character in the
 * specified range of this set.
 * @see #getRangeStart
 * @see #getRangeEnd
 */
UChar32 UnicodeSet::getRangeEnd(int32_t सूचकांक) स्थिर {
    वापस list[सूचकांक*2 + 1] - 1;
}

स्थिर UnicodeString* UnicodeSet::getString(int32_t सूचकांक) स्थिर U_LIFETIME_BOUND {
    वापस static_cast<स्थिर UnicodeString*>(strings_->elementAt(सूचकांक));
}

/**
 * Reallocate this objects internal structures to take up the least
 * possible space, without changing this object's value.
 */
UnicodeSet& UnicodeSet::compact() U_LIFETIME_BOUND {
    अगर (isFrozen() || isBogus()) {
        वापस *यह;
    }
    // Delete buffer first to defragment memory less.
    अगर (बफर != ढेर_List) {
        uprv_free(बफर);
        बफर = nullptr;
        बफर_Capacity = 0;
    }
    अगर (list == ढेर_List) {
        // pass
    } अन्यथा अगर (len <= INITIAL_CAPACITY) {
        uprv_memcpy(ढेर_List, list, len * माप(UChar32));
        uprv_free(list);
        list = ढेर_List;
        capacity = INITIAL_CAPACITY;
    } अन्यथा अगर ((len + 7) < capacity) {
        // If we have more than a little unused capacity, shrink it to len.
        UChar32* अस्थायी = static_cast<UChar32*>(uprv_realloc(list, माप(UChar32) * len));
        अगर (अस्थायी) {
            list = अस्थायी;
            capacity = len;
        }
        // else what the heck happened?! We allocated less memory!
        // Oh well. We'll keep our original array.
    }
    अगर (strings_ != nullptr && strings_->isEmpty()) {
        मिटाओ strings_;
        strings_ = nullptr;
    }
    वापस *यह;
}

#अगर_घोषित DEBUG_SERIALIZE
#समावेश <stdio.h>
#पूर्ण_अगर

/**
 * Deserialize constructor.
 */
UnicodeSet::UnicodeSet(स्थिर uint16_t तथ्य[], int32_t तथ्य_Len, ESerialization serialization,
                       UErrorCode &ec) {

  अगर(U_FAILURE(ec)) {
    setToBogus();
    वापस;
  }

  अगर( (serialization != kSerialized)
      || (तथ्य==nullptr)
      || (तथ्य_Len < 1)) {
    ec = U_ILLEGAL_ARGUMENT_ERROR;
    setToBogus();
    वापस;
  }

  // bmp?
  int32_t header_आकार = ((तथ्य[0]&0x8000)) ?2:1;
  int32_t bmp_लंबाई = (header_आकार==1)?तथ्य[0]:तथ्य[1];

  int32_t new_लंबाई = (((तथ्य[0]&0x7FFF)-bmp_लंबाई)/2)+bmp_लंबाई;
#अगर_घोषित DEBUG_SERIALIZE
  म_लिखो("dataLen %d headerSize %d bmpLen %d len %d. data[0]=%X/%X/%X/%X\n", तथ्य_Len,header_आकार,bmp_लंबाई,new_लंबाई, तथ्य[0],तथ्य[1],तथ्य[2],तथ्य[3]);
#पूर्ण_अगर
  अगर(!ensureCapacity(new_लंबाई + 1)) {  // +1 for HIGH
    वापस;
  }
  // copy bmp
  int32_t i;
  क्रम(i = 0; i< bmp_लंबाई;i++) {
    list[i] = तथ्य[i+header_आकार];
#अगर_घोषित DEBUG_SERIALIZE
    म_लिखो("<<16@%d[%d] %X\n", i+header_आकार, i, list[i]);
#पूर्ण_अगर
  }
  // copy smp
  क्रम(i=bmp_लंबाई;i<new_लंबाई;i++) {
    list[i] = (static_cast<UChar32>(तथ्य[header_आकार + bmp_लंबाई + (i - bmp_लंबाई) * 2 + 0]) << 16) +
               static_cast<UChar32>(तथ्य[header_आकार + bmp_लंबाई + (i - bmp_लंबाई) * 2 + 1]);
#अगर_घोषित DEBUG_SERIALIZE
    म_लिखो("<<32@%d+[%d] %lX\n", header_आकार+bmp_लंबाई+i, i, list[i]);
#पूर्ण_अगर
  }
  U_ASSERT(i == new_लंबाई);
  अगर (i == 0 || list[i - 1] != UNICODESET_ऊपरी) {
    list[i++] = UNICODESET_ऊपरी;
  }
  len = i;
}


int32_t UnicodeSet::serialize(uint16_t *dest, int32_t destCapacity, UErrorCode& ec) स्थिर {
    int32_t bmp_लंबाई, length, dest_लंबाई;

    अगर (U_FAILURE(ec)) {
        वापस 0;
    }

    अगर (destCapacity<0 || (destCapacity>0 && dest==nullptr)) {
        ec=U_ILLEGAL_ARGUMENT_ERROR;
        वापस 0;
    }

    /* count necessary 16-bit units */
    length=यह->len-1; // Subtract 1 to ignore final UNICODESET_HIGH
    // assert(length>=0);
    अगर (length==0) {
        /* empty set */
        अगर (destCapacity>0) {
            *dest=0;
        } अन्यथा {
            ec=U_बफर_OVERFLOW_ERROR;
        }
        वापस 1;
    }
    /* now length>0 */

    अगर (यह->list[length-1]<=0xffff) {
        /* all BMP */
        bmp_लंबाई=length;
    } अन्यथा अगर (यह->list[0]>=0x10000) {
        /* all supplementary */
        bmp_लंबाई=0;
        length*=2;
    } अन्यथा {
        /* some BMP, some supplementary */
        क्रम (bmp_लंबाई=0; bmp_लंबाई<length && यह->list[bmp_लंबाई]<=0xffff; ++bmp_लंबाई) {}
        length=bmp_लंबाई+2*(length-bmp_लंबाई);
    }
#अगर_घोषित DEBUG_SERIALIZE
    म_लिखो(">> bmpLength%d length%d len%d\n", bmp_लंबाई, length, len);
#पूर्ण_अगर
    /* length: number of 16-bit array units */
    अगर (length>0x7fff) {
        /* there are only 15 bits for the length in the first serialized word */
        ec=U_सूचकांक_OUTOFBOUNDS_ERROR;
        वापस 0;
    }

    /*
     * total serialized length:
     * number of 16-bit array units (length) +
     * 1 length unit (always) +
     * 1 bmpLength unit (if there are supplementary values)
     */
    dest_लंबाई=length+((length>bmp_लंबाई)?2:1);
    अगर (dest_लंबाई<=destCapacity) {
        स्थिर UChar32 *p;
        int32_t i;

#अगर_घोषित DEBUG_SERIALIZE
        म_लिखो("writeHdr\n");
#पूर्ण_अगर
        *dest = static_cast<uint16_t>(length);
        अगर (length>bmp_लंबाई) {
            *dest|=0x8000;
            *++dest = static_cast<uint16_t>(bmp_लंबाई);
        }
        ++dest;

        /* write the BMP part of the array */
        p=यह->list;
        क्रम (i=0; i<bmp_लंबाई; ++i) {
#अगर_घोषित DEBUG_SERIALIZE
          म_लिखो("writebmp: %x\n", (पूर्णांक)*p);
#पूर्ण_अगर
            *dest++ = static_cast<uint16_t>(*p++);
        }

        /* write the supplementary part of the array */
        क्रम (; i<length; i+=2) {
#अगर_घोषित DEBUG_SERIALIZE
          म_लिखो("write32: %x\n", (पूर्णांक)*p);
#पूर्ण_अगर
            *dest++ = static_cast<uint16_t>(*p >> 16);
            *dest++ = static_cast<uint16_t>(*p++);
        }
    } अन्यथा {
        ec=U_बफर_OVERFLOW_ERROR;
    }
    वापस dest_लंबाई;
}

//----------------------------------------------------------------
// Implementation: Utility methods
//----------------------------------------------------------------

/**
 * Allocate our strings vector and return true if successful.
 */
UBool UnicodeSet::allocateStrings(UErrorCode &status) {
    अगर (U_FAILURE(status)) {
        वापस false;
    }
    strings_ = नया UVector(uprv_deleteUObject,
                          uhash_compareUnicodeString, 1, status);
    अगर (strings_ == nullptr) { // Check for memory allocation error.
        status = U_MEMORY_ALLOCATION_ERROR;
        वापस false;
    }
    अगर (U_FAILURE(status)) {
        मिटाओ strings_;
        strings_ = nullptr;
        वापस false;
    } 
    वापस true;
}

int32_t UnicodeSet::अग्र_Capacity(int32_t minCapacity) {
    // Grow exponentially to reduce the frequency of allocations.
    अगर (minCapacity < INITIAL_CAPACITY) {
        वापस minCapacity + INITIAL_CAPACITY;
    } अन्यथा अगर (minCapacity <= 2500) {
        वापस 5 * minCapacity;
    } अन्यथा {
        int32_t newCapacity = 2 * minCapacity;
        अगर (newCapacity > MAX_लंबाई) {
            newCapacity = MAX_लंबाई;
        }
        वापस newCapacity;
    }
}

bool UnicodeSet::ensureCapacity(int32_t newLen) {
    अगर (newLen > MAX_लंबाई) {
        newLen = MAX_लंबाई;
    }
    अगर (newLen <= capacity) {
        वापस true;
    }
    int32_t newCapacity = अग्र_Capacity(newLen);
    UChar32* अस्थायी = static_cast<UChar32*>(uprv_malloc(newCapacity * माप(UChar32)));
    अगर (अस्थायी == nullptr) {
        setToBogus(); // set the object to bogus state if an OOM failure occurred.
        वापस false;
    }
    // Copy only the actual contents.
    uprv_memcpy(अस्थायी, list, len * माप(UChar32));
    अगर (list != ढेर_List) {
        uprv_free(list);
    }
    list = अस्थायी;
    capacity = newCapacity;
    वापस true;
}

bool UnicodeSet::ensure_बफर_Capacity(int32_t newLen) {
    अगर (newLen > MAX_लंबाई) {
        newLen = MAX_लंबाई;
    }
    अगर (newLen <= बफर_Capacity) {
        वापस true;
    }
    int32_t newCapacity = अग्र_Capacity(newLen);
    UChar32* अस्थायी = static_cast<UChar32*>(uprv_malloc(newCapacity * माप(UChar32)));
    अगर (अस्थायी == nullptr) {
        setToBogus();
        वापस false;
    }
    // The buffer has no contents to be copied.
    // It is always filled from scratch after this call.
    अगर (बफर != ढेर_List) {
        uprv_free(बफर);
    }
    बफर = अस्थायी;
    बफर_Capacity = newCapacity;
    वापस true;
}

/**
 * Swap list and buffer.
 */
व्योम UnicodeSet::swapBuffers() {
    // swap list and buffer
    UChar32* अस्थायी = list;
    list = बफर;
    बफर = अस्थायी;

    int32_t c = capacity;
    capacity = बफर_Capacity;
    बफर_Capacity = c;
}

व्योम UnicodeSet::setToBogus() {
    सफा(); // Remove everything in the set.
    fFlags = kIsBogus;
}

//----------------------------------------------------------------
// Implementation: Fundamental operators
//----------------------------------------------------------------

अटल अंतरभूत UChar32 max(UChar32 a, UChar32 b) {
    वापस (a > b) ? a : b;
}

// polarity = 0, 3 is normal: x xor y
// polarity = 1, 2: x xor ~y == x === y

व्योम UnicodeSet::exclusiveOr(स्थिर UChar32* other, int32_t otherLen, int8_t polarity) {
    अगर (isFrozen() || isBogus()) {
        वापस;
    }
    अगर (!ensure_बफर_Capacity(len + otherLen)) {
        वापस;
    }

    int32_t i = 0, j = 0, k = 0;
    UChar32 a = list[i++];
    UChar32 b;
    अगर (polarity == 1 || polarity == 2) {
        b = UNICODESET_निचला;
        अगर (other[j] == UNICODESET_निचला) { // skip base if already LOW
            ++j;
            b = other[j];
        }
    } अन्यथा {
        b = other[j++];
    }
    // simplest of all the routines
    // sort the values, discarding identicals!
    क्रम (;;) {
        अगर (a < b) {
            बफर[k++] = a;
            a = list[i++];
        } अन्यथा अगर (b < a) {
            बफर[k++] = b;
            b = other[j++];
        } अन्यथा अगर (a != UNICODESET_ऊपरी) { // at this point, a == b
            // discard both values!
            a = list[i++];
            b = other[j++];
        } अन्यथा { // DONE!
            बफर[k++] = UNICODESET_ऊपरी;
            len = k;
            अवरोध;
        }
    }
    swapBuffers();
    releasePattern();
}

// polarity = 0 is normal: x union y
// polarity = 2: x union ~y
// polarity = 1: ~x union y
// polarity = 3: ~x union ~y

व्योम UnicodeSet::add(स्थिर UChar32* other, int32_t otherLen, int8_t polarity) {
    अगर (isFrozen() || isBogus() || other==nullptr) {
        वापस;
    }
    अगर (!ensure_बफर_Capacity(len + otherLen)) {
        वापस;
    }

    int32_t i = 0, j = 0, k = 0;
    UChar32 a = list[i++];
    UChar32 b = other[j++];
    // change from xor is that we have to check overlapping pairs
    // polarity bit 1 means a is second, bit 2 means b is.
    क्रम (;;) {
        चयन (polarity) {
          हाल 0: // both first; take lower if unequal
            अगर (a < b) { // take a
                // Back up over overlapping ranges in buffer[]
                अगर (k > 0 && a <= बफर[k-1]) {
                    // Pick latter end value in buffer[] vs. list[]
                    a = max(list[i], बफर[--k]);
                } अन्यथा {
                    // No overlap
                    बफर[k++] = a;
                    a = list[i];
                }
                i++; // Common if/else code factored out
                polarity ^= 1;
            } अन्यथा अगर (b < a) { // take b
                अगर (k > 0 && b <= बफर[k-1]) {
                    b = max(other[j], बफर[--k]);
                } अन्यथा {
                    बफर[k++] = b;
                    b = other[j];
                }
                j++;
                polarity ^= 2;
            } अन्यथा { // a == b, take a, drop b
                अगर (a == UNICODESET_ऊपरी) जाओ loop_end;
                // This is symmetrical; it doesn't matter if
                // we backtrack with a or b. - liu
                अगर (k > 0 && a <= बफर[k-1]) {
                    a = max(list[i], बफर[--k]);
                } अन्यथा {
                    // No overlap
                    बफर[k++] = a;
                    a = list[i];
                }
                i++;
                polarity ^= 1;
                b = other[j++];
                polarity ^= 2;
            }
            अवरोध;
          हाल 3: // both second; take higher if unequal, and drop other
            अगर (b <= a) { // take a
                अगर (a == UNICODESET_ऊपरी) जाओ loop_end;
                बफर[k++] = a;
            } अन्यथा { // take b
                अगर (b == UNICODESET_ऊपरी) जाओ loop_end;
                बफर[k++] = b;
            }
            a = list[i++];
            polarity ^= 1;   // factored common code
            b = other[j++];
            polarity ^= 2;
            अवरोध;
          हाल 1: // a second, b first; if b < a, overlap
            अगर (a < b) { // no overlap, take a
                बफर[k++] = a; a = list[i++]; polarity ^= 1;
            } अन्यथा अगर (b < a) { // OVERLAP, drop b
                b = other[j++];
                polarity ^= 2;
            } अन्यथा { // a == b, drop both!
                अगर (a == UNICODESET_ऊपरी) जाओ loop_end;
                a = list[i++];
                polarity ^= 1;
                b = other[j++];
                polarity ^= 2;
            }
            अवरोध;
          हाल 2: // a first, b second; if a < b, overlap
            अगर (b < a) { // no overlap, take b
                बफर[k++] = b;
                b = other[j++];
                polarity ^= 2;
            } अन्यथा  अगर (a < b) { // OVERLAP, drop a
                a = list[i++];
                polarity ^= 1;
            } अन्यथा { // a == b, drop both!
                अगर (a == UNICODESET_ऊपरी) जाओ loop_end;
                a = list[i++];
                polarity ^= 1;
                b = other[j++];
                polarity ^= 2;
            }
            अवरोध;
        }
    }
 loop_end:
    बफर[k++] = UNICODESET_ऊपरी;    // terminate
    len = k;
    swapBuffers();
    releasePattern();
}

// polarity = 0 is normal: x intersect y
// polarity = 2: x intersect ~y == set-minus
// polarity = 1: ~x intersect y
// polarity = 3: ~x intersect ~y

व्योम UnicodeSet::retain(स्थिर UChar32* other, int32_t otherLen, int8_t polarity) {
    अगर (isFrozen() || isBogus()) {
        वापस;
    }
    अगर (!ensure_बफर_Capacity(len + otherLen)) {
        वापस;
    }

    int32_t i = 0, j = 0, k = 0;
    UChar32 a = list[i++];
    UChar32 b = other[j++];
    // change from xor is that we have to check overlapping pairs
    // polarity bit 1 means a is second, bit 2 means b is.
    क्रम (;;) {
        चयन (polarity) {
          हाल 0: // both first; drop the smaller
            अगर (a < b) { // drop a
                a = list[i++];
                polarity ^= 1;
            } अन्यथा अगर (b < a) { // drop b
                b = other[j++];
                polarity ^= 2;
            } अन्यथा { // a == b, take one, drop other
                अगर (a == UNICODESET_ऊपरी) जाओ loop_end;
                बफर[k++] = a;
                a = list[i++];
                polarity ^= 1;
                b = other[j++];
                polarity ^= 2;
            }
            अवरोध;
          हाल 3: // both second; take lower if unequal
            अगर (a < b) { // take a
                बफर[k++] = a;
                a = list[i++];
                polarity ^= 1;
            } अन्यथा अगर (b < a) { // take b
                बफर[k++] = b;
                b = other[j++];
                polarity ^= 2;
            } अन्यथा { // a == b, take one, drop other
                अगर (a == UNICODESET_ऊपरी) जाओ loop_end;
                बफर[k++] = a;
                a = list[i++];
                polarity ^= 1;
                b = other[j++];
                polarity ^= 2;
            }
            अवरोध;
          हाल 1: // a second, b first;
            अगर (a < b) { // NO OVERLAP, drop a
                a = list[i++];
                polarity ^= 1;
            } अन्यथा अगर (b < a) { // OVERLAP, take b
                बफर[k++] = b;
                b = other[j++];
                polarity ^= 2;
            } अन्यथा { // a == b, drop both!
                अगर (a == UNICODESET_ऊपरी) जाओ loop_end;
                a = list[i++];
                polarity ^= 1;
                b = other[j++];
                polarity ^= 2;
            }
            अवरोध;
          हाल 2: // a first, b second; if a < b, overlap
            अगर (b < a) { // no overlap, drop b
                b = other[j++];
                polarity ^= 2;
            } अन्यथा  अगर (a < b) { // OVERLAP, take a
                बफर[k++] = a;
                a = list[i++];
                polarity ^= 1;
            } अन्यथा { // a == b, drop both!
                अगर (a == UNICODESET_ऊपरी) जाओ loop_end;
                a = list[i++];
                polarity ^= 1;
                b = other[j++];
                polarity ^= 2;
            }
            अवरोध;
        }
    }
 loop_end:
    बफर[k++] = UNICODESET_ऊपरी;    // terminate
    len = k;
    swapBuffers();
    releasePattern();
}

/**
 * Append the <code>toPattern()</code> representation of a
 * string to the given <code>StringBuffer</code>.
 */
व्योम UnicodeSet::_appendToPat(UnicodeString& buf, स्थिर UnicodeString& s, UBool escapeUnprintable) {
    UChar32 cp;
    क्रम (int32_t i = 0; i < s.length(); i += U16_लंबाई(cp)) {
        _appendToPat(buf, cp = s.char32At(i), escapeUnprintable);
    }
}

/**
 * Append the <code>toPattern()</code> representation of a
 * character to the given <code>StringBuffer</code>.
 */
व्योम UnicodeSet::_appendToPat(UnicodeString& buf, UChar32 c, UBool escapeUnprintable) {
    अगर (escapeUnprintable ? ICU_Utility::isUnprintable(c) : ICU_Utility::shouldAlwaysBeEscaped(c)) {
        // Use hex escape notation (\uxxxx or \Uxxxxxxxx) for anything
        // unprintable
        ICU_Utility::escape(buf, c);
        वापस;
    }
    // Okay to let ':' pass through
    चयन (c) {
    हाल u'[':
    हाल u']':
    हाल u'-':
    हाल u'^':
    हाल u'&':
    हाल u'\\':
    हाल u'{':
    हाल u'}':
    हाल u':':
    हाल प्रतीक_Table::प्रतीक_REF:
        buf.append(u'\\');
        अवरोध;
    शेष:
        // Escape whitespace
        अगर (PatternProps::isWhiteSpace(c)) {
            buf.append(u'\\');
        }
        अवरोध;
    }
    buf.append(c);
}

व्योम UnicodeSet::_appendToPat(UnicodeString &परिणाम, UChar32 start, UChar32 end,
                              UBool escapeUnprintable) {
    _appendToPat(परिणाम, start, escapeUnprintable);
    अगर (start != end) {
        अगर ((start+1) != end ||
                // Avoid writing what looks like a lead+trail surrogate pair.
                start == 0xdbff) {
            परिणाम.append(u'-');
        }
        _appendToPat(परिणाम, end, escapeUnprintable);
    }
}

/**
 * Append a string representation of this set to result.  This will be
 * a cleaned version of the string passed to applyPattern(), if there
 * is one.  Otherwise it will be generated.
 */
UnicodeString& UnicodeSet::_toPattern(UnicodeString& परिणाम U_LIFETIME_BOUND,
                                      UBool escapeUnprintable) स्थिर
{
    अगर (pat != nullptr) {
        int32_t i;
        int32_t backslash_गिनती = 0;
        क्रम (i=0; i<patLen; ) {
            UChar32 c;
            U16_अग्र(pat, i, patLen, c);
            अगर (escapeUnprintable ?
                    ICU_Utility::isUnprintable(c) : ICU_Utility::shouldAlwaysBeEscaped(c)) {
                // If the unprintable character is preceded by an odd
                // number of backslashes, then it has been escaped.
                // Before unescaping it, we delete the final
                // backslash.
                अगर ((backslash_गिनती % 2) == 1) {
                    परिणाम.truncate(परिणाम.length() - 1);
                }
                ICU_Utility::escape(परिणाम, c);
                backslash_गिनती = 0;
            } अन्यथा {
                परिणाम.append(c);
                अगर (c == u'\\') {
                    ++backslash_गिनती;
                } अन्यथा {
                    backslash_गिनती = 0;
                }
            }
        }
        वापस परिणाम;
    }

    वापस _generatePattern(परिणाम, escapeUnprintable);
}

/**
 * Returns a string representation of this set.  If the result of
 * calling this function is passed to a UnicodeSet constructor, it
 * will produce another set that is equal to this one.
 */
UnicodeString& UnicodeSet::toPattern(UnicodeString& परिणाम U_LIFETIME_BOUND,
                                     UBool escapeUnprintable) स्थिर
{
    परिणाम.truncate(0);
    वापस _toPattern(परिणाम, escapeUnprintable);
}

/**
 * Generate and append a string representation of this set to result.
 * This does not use this.pat, the cleaned up copy of the string
 * passed to applyPattern().
 */
UnicodeString& UnicodeSet::_generatePattern(UnicodeString& परिणाम U_LIFETIME_BOUND,
                                            UBool escapeUnprintable) स्थिर
{
    परिणाम.append(u'[');

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
    अगर (len >= 4 && list[0] == 0 && limit == len && !hasStrings()) {
        // Emit the inverse
        परिणाम.append(u'^');
        // Offsetting the inversion list index by one lets us
        // iterate over the ranges of the set complement.
        i = 1;
        --limit;
    }

    // Emit the ranges as pairs.
    जबतक (i < limit) {
        UChar32 start = list[i];  // getRangeStart()
        UChar32 end = list[i + 1] - 1;  // getRangeEnd() = range limit minus one
        अगर (!(0xd800 <= end && end <= 0xdbff)) {
            _appendToPat(परिणाम, start, end, escapeUnprintable);
            i += 2;
        } अन्यथा {
            // The range ends with a lead surrogate.
            // Avoid writing what looks like a lead+trail surrogate pair.
            // 1. Postpone ranges that start with a lead surrogate code point.
            int32_t firstLead = i;
            जबतक ((i += 2) < limit && list[i] <= 0xdbff) {}
            int32_t firstAfterLead = i;
            // 2. Write following ranges that start with a trail surrogate code point.
            जबतक (i < limit && (start = list[i]) <= 0xdfff) {
                _appendToPat(परिणाम, start, list[i + 1] - 1, escapeUnprintable);
                i += 2;
            }
            // 3. Now write the postponed ranges.
            क्रम (पूर्णांक j = firstLead; j < firstAfterLead; j += 2) {
                _appendToPat(परिणाम, list[j], list[j + 1] - 1, escapeUnprintable);
            }
        }
    }

    अगर (strings_ != nullptr) {
        क्रम (int32_t i = 0; i<strings_->size(); ++i) {
            परिणाम.append(u'{');
            _appendToPat(परिणाम,
                         *static_cast<स्थिर UnicodeString*>(strings_->elementAt(i)),
                         escapeUnprintable);
            परिणाम.append(u'}');
        }
    }
    वापस परिणाम.append(u']');
}

/**
* Release existing cached pattern
*/
व्योम UnicodeSet::releasePattern() {
    अगर (pat) {
        uprv_free(pat);
        pat = nullptr;
        patLen = 0;
    }
}

/**
* Set the new pattern to cache.
*/
व्योम UnicodeSet::setPattern(स्थिर char16_t *newPat, int32_t newPatLen) {
    releasePattern();
    pat = static_cast<char16_t*>(uprv_malloc((newPatLen + 1) * माप(char16_t)));
    अगर (pat) {
        patLen = newPatLen;
        u_memcpy(pat, newPat, patLen);
        pat[patLen] = 0;
    }
    // else we don't care if malloc failed. This was just a nice cache.
    // We can regenerate an equivalent pattern later when requested.
}

UnicodeSet* UnicodeSet::freeze() U_LIFETIME_BOUND {
    अगर(!isFrozen() && !isBogus()) {
        compact();

        // Optimize contains() and span() and similar functions.
        अगर (hasStrings()) {
            stringSpan = नया UnicodeSetStringSpan(*यह, *strings_, UnicodeSetStringSpan::ALL);
            अगर (stringSpan == nullptr) {
                setToBogus();
                वापस यह;
            } अन्यथा अगर (!stringSpan->needsStringSpanUTF16()) {
                // All strings are irrelevant for span() etc. because
                // all of each string's code points are contained in this set.
                // Do not check needsStringSpanUTF8() because UTF-8 has at most as
                // many relevant strings as UTF-16.
                // (Thus needsStringSpanUTF8() implies needsStringSpanUTF16().)
                मिटाओ stringSpan;
                stringSpan = nullptr;
            }
        }
        अगर (stringSpan == nullptr) {
            // No span-relevant strings: Optimize for code point spans.
            bmpSet=नया BMPSet(list, len);
            अगर (bmpSet == nullptr) { // Check for memory allocation error.
                setToBogus();
            }
        }
    }
    वापस यह;
}

int32_t UnicodeSet::span(स्थिर char16_t *s, int32_t length, USetSpanCondition spanCondition) स्थिर {
    अगर(length>0 && bmpSet!=nullptr) {
        वापस static_cast<int32_t>(bmpSet->span(s, s + length, spanCondition) - s);
    }
    अगर(length<0) {
        length=u_strlen(s);
    }
    अगर(length==0) {
        वापस 0;
    }
    अगर(stringSpan!=nullptr) {
        वापस stringSpan->span(s, length, spanCondition);
    } अन्यथा अगर(hasStrings()) {
        uint32_t which= spanCondition==USET_SPAN_NOT_CONTAINED ?
                            UnicodeSetStringSpan::FWD_UTF16_NOT_CONTAINED :
                            UnicodeSetStringSpan::FWD_UTF16_CONTAINED;
        UnicodeSetStringSpan strSpan(*यह, *strings_, which);
        अगर(strSpan.needsStringSpanUTF16()) {
            वापस strSpan.span(s, length, spanCondition);
        }
    }

    अगर(spanCondition!=USET_SPAN_NOT_CONTAINED) {
        spanCondition=USET_SPAN_CONTAINED;  // Pin to 0/1 values.
    }

    UChar32 c;
    int32_t start=0, पूर्व=0;
    करो {
        U16_अग्र(s, start, length, c);
        अगर(spanCondition!=contains(c)) {
            अवरोध;
        }
    } जबतक((पूर्व=start)<length);
    वापस पूर्व;
}

int32_t UnicodeSet::spanBack(स्थिर char16_t *s, int32_t length, USetSpanCondition spanCondition) स्थिर {
    अगर(length>0 && bmpSet!=nullptr) {
        वापस static_cast<int32_t>(bmpSet->spanBack(s, s + length, spanCondition) - s);
    }
    अगर(length<0) {
        length=u_strlen(s);
    }
    अगर(length==0) {
        वापस 0;
    }
    अगर(stringSpan!=nullptr) {
        वापस stringSpan->spanBack(s, length, spanCondition);
    } अन्यथा अगर(hasStrings()) {
        uint32_t which= spanCondition==USET_SPAN_NOT_CONTAINED ?
                            UnicodeSetStringSpan::BACK_UTF16_NOT_CONTAINED :
                            UnicodeSetStringSpan::BACK_UTF16_CONTAINED;
        UnicodeSetStringSpan strSpan(*यह, *strings_, which);
        अगर(strSpan.needsStringSpanUTF16()) {
            वापस strSpan.spanBack(s, length, spanCondition);
        }
    }

    अगर(spanCondition!=USET_SPAN_NOT_CONTAINED) {
        spanCondition=USET_SPAN_CONTAINED;  // Pin to 0/1 values.
    }

    UChar32 c;
    int32_t पूर्व=length;
    करो {
        U16_पूर्व(s, 0, length, c);
        अगर(spanCondition!=contains(c)) {
            अवरोध;
        }
    } जबतक((पूर्व=length)>0);
    वापस पूर्व;
}

int32_t UnicodeSet::spanUTF8(स्थिर अक्षर *s, int32_t length, USetSpanCondition spanCondition) स्थिर {
    अगर(length>0 && bmpSet!=nullptr) {
        स्थिर uint8_t* s0 = reinterpret_cast<स्थिर uint8_t*>(s);
        वापस static_cast<int32_t>(bmpSet->spanUTF8(s0, length, spanCondition) - s0);
    }
    अगर(length<0) {
        length = static_cast<int32_t>(uprv_strlen(s));
    }
    अगर(length==0) {
        वापस 0;
    }
    अगर(stringSpan!=nullptr) {
        वापस stringSpan->spanUTF8(reinterpret_cast<स्थिर uint8_t*>(s), length, spanCondition);
    } अन्यथा अगर(hasStrings()) {
        uint32_t which= spanCondition==USET_SPAN_NOT_CONTAINED ?
                            UnicodeSetStringSpan::FWD_UTF8_NOT_CONTAINED :
                            UnicodeSetStringSpan::FWD_UTF8_CONTAINED;
        UnicodeSetStringSpan strSpan(*यह, *strings_, which);
        अगर(strSpan.needsStringSpanUTF8()) {
            वापस strSpan.spanUTF8(reinterpret_cast<स्थिर uint8_t*>(s), length, spanCondition);
        }
    }

    अगर(spanCondition!=USET_SPAN_NOT_CONTAINED) {
        spanCondition=USET_SPAN_CONTAINED;  // Pin to 0/1 values.
    }

    UChar32 c;
    int32_t start=0, पूर्व=0;
    करो {
        U8_अग्र_OR_FFFD(s, start, length, c);
        अगर(spanCondition!=contains(c)) {
            अवरोध;
        }
    } जबतक((पूर्व=start)<length);
    वापस पूर्व;
}

int32_t UnicodeSet::spanBackUTF8(स्थिर अक्षर *s, int32_t length, USetSpanCondition spanCondition) स्थिर {
    अगर(length>0 && bmpSet!=nullptr) {
        स्थिर uint8_t* s0 = reinterpret_cast<स्थिर uint8_t*>(s);
        वापस bmpSet->spanBackUTF8(s0, length, spanCondition);
    }
    अगर(length<0) {
        length = static_cast<int32_t>(uprv_strlen(s));
    }
    अगर(length==0) {
        वापस 0;
    }
    अगर(stringSpan!=nullptr) {
        वापस stringSpan->spanBackUTF8(reinterpret_cast<स्थिर uint8_t*>(s), length, spanCondition);
    } अन्यथा अगर(hasStrings()) {
        uint32_t which= spanCondition==USET_SPAN_NOT_CONTAINED ?
                            UnicodeSetStringSpan::BACK_UTF8_NOT_CONTAINED :
                            UnicodeSetStringSpan::BACK_UTF8_CONTAINED;
        UnicodeSetStringSpan strSpan(*यह, *strings_, which);
        अगर(strSpan.needsStringSpanUTF8()) {
            वापस strSpan.spanBackUTF8(reinterpret_cast<स्थिर uint8_t*>(s), length, spanCondition);
        }
    }

    अगर(spanCondition!=USET_SPAN_NOT_CONTAINED) {
        spanCondition=USET_SPAN_CONTAINED;  // Pin to 0/1 values.
    }

    UChar32 c;
    int32_t पूर्व=length;
    करो {
        U8_पूर्व_OR_FFFD(s, 0, length, c);
        अगर(spanCondition!=contains(c)) {
            अवरोध;
        }
    } जबतक((पूर्व=length)>0);
    वापस पूर्व;
}

U_NAMESPACE_END
