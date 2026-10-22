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

#అన్తర్భావ "unicode/utypes.h"
#అన్తర్భావ "unicode/parsepos.h"
#అన్తర్భావ "unicode/symtable.h"
#అన్తర్భావ "unicode/uniset.h"
#అన్తర్భావ "unicode/ustring.h"
#అన్తర్భావ "unicode/utf8.h"
#అన్తర్భావ "unicode/utf16.h"
#అన్తర్భావ "ruleiter.h"
#అన్తర్భావ "cmemory.h"
#అన్తర్భావ "cstring.h"
#అన్తర్భావ "patternprops.h"
#అన్తర్భావ "uelement.h"
#అన్తర్భావ "util.h"
#అన్తర్భావ "uvector.h"
#అన్తర్భావ "charstr.h"
#అన్తర్భావ "ustrfmt.h"
#అన్తర్భావ "uassert.h"
#అన్తర్భావ "bmpset.h"
#అన్తర్భావ "unisetspan.h"

// HIGH_VALUE > all valid values. 110000 for codepoints
#నిర్వచించు UNICODESET_ఎగువ 0x0110000

// LOW <= all valid values. ZERO for codepoints
#నిర్వచించు UNICODESET_దిగువ 0x000000

/** Max list [0, 1, 2, ..., max code point, HIGH] */
constexpr int32_t MAX_నిడివి = UNICODESET_ఎగువ + 1;

U_NAMESPACE_BEGIN

చిహ్నం_పట్టిక::~చిహ్నం_పట్టిక() {}

UOBJECT_DEFINE_RTTI_IMPLEMENTATION(Unicode_సమితి)

/**
 * Modify the given UChar32 variable so that it is in range, by
 * pinning values < UNICODESET_LOW to UNICODESET_LOW, and
 * pinning values > UNICODESET_HIGH-1 to UNICODESET_HIGH-1.
 * It modifies its argument in-place and also returns it.
 */
స్థిర అంతఃవరుస UChar32 pin_సంకేతం_బిందువు(UChar32& c) {
    ఒకవేళ (c < UNICODESET_దిగువ) {
        c = UNICODESET_దిగువ;
    } లేకపోతే ఒకవేళ (c > (UNICODESET_ఎగువ-1)) {
        c = (UNICODESET_ఎగువ-1);
    }
    తిరిగి c;
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

#నిర్వచితమైతే DEBUG_MEM
#అన్తర్భావ <stdio.h>
స్థిర int32_t dbg_లెక్క = 0;

స్థిర అంతఃవరుస శూన్య _dbgct(Unicode_సమితి* సమితి) {
    UnicodeString str;
    సమితి->గమ్యం_నమూనా(str, నిజం);
    అక్షరం buf[40];
    str.extract(0, 39, buf, "");
    రాయి("DEBUG UnicodeSet: ct 0x%08X; %d %s\n", సమితి, ++dbg_లెక్క, buf);
}

స్థిర అంతఃవరుస శూన్య _dbgdt(Unicode_సమితి* సమితి) {
    UnicodeString str;
    సమితి->గమ్యం_నమూనా(str, నిజం);
    అక్షరం buf[40];
    str.extract(0, 39, buf, "");
    రాయి("DEBUG UnicodeSet: dt 0x%08X; %d %s\n", సమితి, --dbg_లెక్క, buf);
}

#లేకపోతే

#నిర్వచించు _dbgct(సమితి)
#నిర్వచించు _dbgdt(సమితి)

#అయితే_ముగింపు

//----------------------------------------------------------------
// UnicodeString in UVector support
//----------------------------------------------------------------

స్థిర శూన్య U_CALLCONV cloneUnicodeString(UElement *dst, UElement *src) {
    dst->సూచిక = కొత్త UnicodeString(*static_cast<UnicodeString*>(src->సూచిక));
}

స్థిర int32_t U_CALLCONV compareUnicodeString(UElement t1, UElement t2) {
    స్థిరరాశి UnicodeString& a = *static_cast<స్థిరరాశి UnicodeString*>(t1.సూచిక);
    స్థిరరాశి UnicodeString& b = *static_cast<స్థిరరాశి UnicodeString*>(t2.సూచిక);
    తిరిగి a.compare(b);
}

UBool Unicode_సమితి::has_అక్షరశ్రేణులు() స్థిరరాశి {
    తిరిగి అక్షరశ్రేణులు_2 != శూన్యసూచిక && !అక్షరశ్రేణులు_2->is_ఖాళీలు();
}

int32_t Unicode_సమితి::అక్షరశ్రేణులు_కొలత() స్థిరరాశి {
    తిరిగి అక్షరశ్రేణులు_2 == శూన్యసూచిక ? 0 : అక్షరశ్రేణులు_2->size();
}

UBool Unicode_సమితి::అక్షరశ్రేణులు_కలిగియుంది(స్థిరరాశి UnicodeString &s) స్థిరరాశి {
    తిరిగి అక్షరశ్రేణులు_2 != శూన్యసూచిక && అక్షరశ్రేణులు_2->కలిగియుంది((శూన్య*) &s);
}

//----------------------------------------------------------------
// Constructors &c
//----------------------------------------------------------------

/**
 * Constructs an empty set.
 */
Unicode_సమితి::Unicode_సమితి() {
    list[0] = UNICODESET_ఎగువ;
    _dbgct(ఇది);
}

/**
 * Constructs a set containing the given range. If <code>end >
 * start</code> then an empty set is created.
 *
 * @param start first character, inclusive, of range
 * @param end last character, inclusive, of range
 */
Unicode_సమితి::Unicode_సమితి(UChar32 ప్రారంభం, UChar32 చివర) {
    list[0] = UNICODESET_ఎగువ;
    కూడు(ప్రారంభం, చివర);
    _dbgct(ఇది);
}

/**
 * Constructs a set that is identical to the given UnicodeSet.
 */
Unicode_సమితి::Unicode_సమితి(స్థిరరాశి Unicode_సమితి& o) : UnicodeFilter(o) {
    *ఇది = o;
    _dbgct(ఇది);
}

// Copy-construct as thawed.
Unicode_సమితి::Unicode_సమితి(స్థిరరాశి Unicode_సమితి& o, UBool /* asThawed */) : UnicodeFilter(o) {
    ఒకవేళ (ensure_సామర్థ్యం(o.len)) {
        // *this = o except for bmpSet and stringSpan
        len = o.len;
        uprv_memcpy(list, o.list, (size_t)len*పరిమాణం(UChar32));
        ఒకవేళ (o.has_అక్షరశ్రేణులు()) {
            UError_సంకేతం స్థితి_2 = U_ZERO_ERROR;
            ఒకవేళ (!allocate_అక్షరశ్రేణులు(స్థితి_2) ||
                    (అక్షరశ్రేణులు_2->assign(*o.అక్షరశ్రేణులు_2, cloneUnicodeString, స్థితి_2), U_FAILURE(స్థితి_2))) {
                సమితి_గమ్యం_Bogus();
                తిరిగి;
            }
        }
        ఒకవేళ (o.pat) {
            సమితి_నమూనా(o.pat, o.patLen);
        }
        _dbgct(ఇది);
    }
}

/**
 * Destructs the set.
 */
Unicode_సమితి::~Unicode_సమితి() {
    _dbgdt(ఇది); // first!
    ఒకవేళ (list != దొంతర_List) {
        uprv_free(list);
    }
    తొలగించు bmp_సమితి;
    ఒకవేళ (బఫరు != దొంతర_List) {
        uprv_free(బఫరు);
    }
    తొలగించు అక్షరశ్రేణులు_2;
    తొలగించు string_పరిధి;
    release_నమూనా();
}

/**
 * Assigns this object to be a copy of another.
 */
Unicode_సమితి& Unicode_సమితి::క్రియాకారకం=(స్థిరరాశి Unicode_సమితి& o) U_LIFETIME_BOUND {
    తిరిగి copyFrom(o, అబద్ధం);
}

Unicode_సమితి& Unicode_సమితి::copyFrom(స్థిరరాశి Unicode_సమితి& o, UBool asThawed) U_LIFETIME_BOUND {
    ఒకవేళ (ఇది == &o) {
        తిరిగి *ఇది;
    }
    ఒకవేళ (isFrozen()) {
        తిరిగి *ఇది;
    }
    ఒకవేళ (o.isBogus()) {
        సమితి_గమ్యం_Bogus();
        తిరిగి *ఇది;
    }
    ఒకవేళ (!ensure_సామర్థ్యం(o.len)) {
        // ensureCapacity will mark the UnicodeSet as Bogus if OOM failure happens.
        తిరిగి *ఇది;
    }
    len = o.len;
    uprv_memcpy(list, o.list, (size_t)len*పరిమాణం(UChar32));
    ఒకవేళ (o.bmp_సమితి != శూన్యసూచిక && !asThawed) {
        bmp_సమితి = కొత్త BMPSet(*o.bmp_సమితి, list, len);
        ఒకవేళ (bmp_సమితి == శూన్యసూచిక) { // Check for memory allocation error.
            సమితి_గమ్యం_Bogus();
            తిరిగి *ఇది;
        }
    }
    ఒకవేళ (o.has_అక్షరశ్రేణులు()) {
        UError_సంకేతం స్థితి_2 = U_ZERO_ERROR;
        ఒకవేళ ((అక్షరశ్రేణులు_2 == శూన్యసూచిక && !allocate_అక్షరశ్రేణులు(స్థితి_2)) ||
                (అక్షరశ్రేణులు_2->assign(*o.అక్షరశ్రేణులు_2, cloneUnicodeString, స్థితి_2), U_FAILURE(స్థితి_2))) {
            సమితి_గమ్యం_Bogus();
            తిరిగి *ఇది;
        }
    } లేకపోతే ఒకవేళ (has_అక్షరశ్రేణులు()) {
        అక్షరశ్రేణులు_2->removeAllElements();
    }
    ఒకవేళ (o.string_పరిధి != శూన్యసూచిక && అక్షరశ్రేణులు_2 != శూన్యసూచిక && !asThawed) {
        string_పరిధి = కొత్త Unicode_సమితి_String_పరిధి(*o.string_పరిధి, *అక్షరశ్రేణులు_2);
        ఒకవేళ (string_పరిధి == శూన్యసూచిక) { // Check for memory allocation error.
            సమితి_గమ్యం_Bogus();
            తిరిగి *ఇది;
        }
    }
    release_నమూనా();
    ఒకవేళ (o.pat) {
        సమితి_నమూనా(o.pat, o.patLen);
    }
    తిరిగి *ఇది;
}

/**
 * Returns a copy of this object.  All UnicodeMatcher objects have
 * to support cloning in order to allow classes using
 * UnicodeMatchers, such as Transliterator, to implement cloning.
 */
Unicode_సమితి* Unicode_సమితి::clone() స్థిరరాశి {
    తిరిగి కొత్త Unicode_సమితి(*ఇది);
}

Unicode_సమితి *Unicode_సమితి::cloneAsThawed() స్థిరరాశి {
    తిరిగి కొత్త Unicode_సమితి(*ఇది, నిజం);
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
తార్కిక Unicode_సమితి::క్రియాకారకం==(స్థిరరాశి Unicode_సమితి& o) స్థిరరాశి {
    ఒకవేళ (len != o.len) తిరిగి అబద్ధం;
    కొరకు (int32_t i = 0; i < len; ++i) {
        ఒకవేళ (list[i] != o.list[i]) తిరిగి అబద్ధం;
    }
    ఒకవేళ (has_అక్షరశ్రేణులు() != o.has_అక్షరశ్రేణులు()) { తిరిగి అబద్ధం; }
    ఒకవేళ (has_అక్షరశ్రేణులు() && *అక్షరశ్రేణులు_2 != *o.అక్షరశ్రేణులు_2) తిరిగి అబద్ధం;
    తిరిగి నిజం;
}

/**
 * Returns the hash code value for this set.
 *
 * @return the hash code value for this set.
 * @see Object#hashCode()
 */
int32_t Unicode_సమితి::hash_సంకేతం() స్థిరరాశి {
    uint32_t ఫలితం = static_cast<uint32_t>(len);
    కొరకు (int32_t i = 0; i < len; ++i) {
        ఫలితం *= 1000003u;
        ఫలితం += list[i];
    }
    తిరిగి static_cast<int32_t>(ఫలితం);
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
int32_t Unicode_సమితి::size() స్థిరరాశి {
    int32_t n = 0;
    int32_t count = get_Range_లెక్క();
    కొరకు (int32_t i = 0; i < count; ++i) {
        n += get_Range_చివర(i) - get_Range_ప్రారంభం(i) + 1;
    }
    తిరిగి n + అక్షరశ్రేణులు_కొలత();
}

/**
 * Returns <tt>true</tt> if this set contains no elements.
 *
 * @return <tt>true</tt> if this set contains no elements.
 */
UBool Unicode_సమితి::is_ఖాళీలు() స్థిరరాశి {
    తిరిగి len == 1 && !has_అక్షరశ్రేణులు();
}

/**
 * Returns true if this set contains the given character.
 * @param c character to be checked for containment
 * @return true if the test condition is met
 */
UBool Unicode_సమితి::కలిగియుంది(UChar32 c) స్థిరరాశి {
    // Set i to the index of the start item greater than ch
    // We know we will terminate without length test!
    // LATER: for large sets, add binary search
    //int32_t i = -1;
    //for (;;) {
    //    if (c < list[++i]) break;
    //}
    ఒకవేళ (bmp_సమితి != శూన్యసూచిక) {
        తిరిగి bmp_సమితి->కలిగియుంది(c);
    }
    ఒకవేళ (string_పరిధి != శూన్యసూచిక) {
        తిరిగి string_పరిధి->కలిగియుంది(c);
    }
    ఒకవేళ (c >= UNICODESET_ఎగువ) { // Don't need to check LOW bound
        తిరిగి అబద్ధం;
    }
    int32_t i = కనుగొను_సంకేతం_బిందువు(c);
    తిరిగి i & 1; // return true if odd
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
int32_t Unicode_సమితి::కనుగొను_సంకేతం_బిందువు(UChar32 c) స్థిరరాశి {
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
    ఒకవేళ (c < list[0])
        తిరిగి 0;
    // High runner test.  c is often after the last range, so an
    // initial check for this condition pays off.
    int32_t lo = 0;
    int32_t hi = len - 1;
    ఒకవేళ (lo >= hi || c >= list[hi-1])
        తిరిగి hi;
    // invariant: c >= list[lo]
    // invariant: c < list[hi]
    కొరకు (;;) {
        int32_t i = (lo + hi) >> 1;
        ఒకవేళ (i == lo) {
            ఆపు; // Found!
        } లేకపోతే ఒకవేళ (c < list[i]) {
            hi = i;
        } లేకపోతే {
            lo = i;
        }
    }
    తిరిగి hi;
}

/**
 * Returns true if this set contains every character
 * of the given range.
 * @param start first character, inclusive, of the range
 * @param end last character, inclusive, of the range
 * @return true if the test condition is met
 */
UBool Unicode_సమితి::కలిగియుంది(UChar32 ప్రారంభం, UChar32 చివర) స్థిరరాశి {
    //int32_t i = -1;
    //for (;;) {
    //    if (start < list[++i]) break;
    //}
    int32_t i = కనుగొను_సంకేతం_బిందువు(ప్రారంభం);
    తిరిగి ((i & 1) != 0 && చివర < list[i]);
}

/**
 * Returns <tt>true</tt> if this set contains the given
 * multicharacter string.
 * @param s string to be checked for containment
 * @return <tt>true</tt> if this set contains the specified string
 */
UBool Unicode_సమితి::కలిగియుంది(స్థిరరాశి UnicodeString& s) స్థిరరాశి {
    int32_t cp = getSingleCP(s);
    ఒకవేళ (cp < 0) {
        తిరిగి అక్షరశ్రేణులు_కలిగియుంది(s);
    } లేకపోతే {
        తిరిగి కలిగియుంది(static_cast<UChar32>(cp));
    }
}

/**
 * Returns true if this set contains all the characters and strings
 * of the given set.
 * @param c set to be checked for containment
 * @return true if the test condition is met
 */
UBool Unicode_సమితి::కలిగియుంది_All(స్థిరరాశి Unicode_సమితి& c) స్థిరరాశి {
    // The specified set is a subset if all of its pairs are contained in
    // this set.  It's possible to code this more efficiently in terms of
    // direct manipulation of the inversion lists if the need arises.
    int32_t n = c.get_Range_లెక్క();
    కొరకు (పూర్ణాంకం i=0; i<n; ++i) {
        ఒకవేళ (!కలిగియుంది(c.get_Range_ప్రారంభం(i), c.get_Range_చివర(i))) {
            తిరిగి అబద్ధం;
        }
    }
    తిరిగి !c.has_అక్షరశ్రేణులు() || (అక్షరశ్రేణులు_2 != శూన్యసూచిక && అక్షరశ్రేణులు_2->కలిగియుంది_All(*c.అక్షరశ్రేణులు_2));
}

/**
 * Returns true if this set contains all the characters
 * of the given string.
 * @param s string containing characters to be checked for containment
 * @return true if the test condition is met
 */
UBool Unicode_సమితి::కలిగియుంది_All(స్థిరరాశి UnicodeString& s) స్థిరరాశి {
    తిరిగి పరిధి(s.get_బఫరు(), s.length(), USET_పరిధి_CONTAINED) == s.length();
}

/**
 * Returns true if this set contains none of the characters
 * of the given range.
 * @param start first character, inclusive, of the range
 * @param end last character, inclusive, of the range
 * @return true if the test condition is met
 */
UBool Unicode_సమితి::కలిగియుంది_None(UChar32 ప్రారంభం, UChar32 చివర) స్థిరరాశి {
    //int32_t i = -1;
    //for (;;) {
    //    if (start < list[++i]) break;
    //}
    int32_t i = కనుగొను_సంకేతం_బిందువు(ప్రారంభం);
    తిరిగి ((i & 1) == 0 && చివర < list[i]);
}

/**
 * Returns true if this set contains none of the characters and strings
 * of the given set.
 * @param c set to be checked for containment
 * @return true if the test condition is met
 */
UBool Unicode_సమితి::కలిగియుంది_None(స్థిరరాశి Unicode_సమితి& c) స్థిరరాశి {
    // The specified set is a subset if all of its pairs are contained in
    // this set.  It's possible to code this more efficiently in terms of
    // direct manipulation of the inversion lists if the need arises.
    int32_t n = c.get_Range_లెక్క();
    కొరకు (int32_t i=0; i<n; ++i) {
        ఒకవేళ (!కలిగియుంది_None(c.get_Range_ప్రారంభం(i), c.get_Range_చివర(i))) {
            తిరిగి అబద్ధం;
        }
    }
    తిరిగి అక్షరశ్రేణులు_2 == శూన్యసూచిక || !c.has_అక్షరశ్రేణులు() || అక్షరశ్రేణులు_2->కలిగియుంది_None(*c.అక్షరశ్రేణులు_2);
}

/**
 * Returns true if this set contains none of the characters
 * of the given string.
 * @param s string containing characters to be checked for containment
 * @return true if the test condition is met
 */
UBool Unicode_సమితి::కలిగియుంది_None(స్థిరరాశి UnicodeString& s) స్థిరరాశి {
    తిరిగి పరిధి(s.get_బఫరు(), s.length(), USET_పరిధి_NOT_CONTAINED) == s.length();
}

/**
 * Returns <tt>true</tt> if this set contains any character whose low byte
 * is the given value.  This is used by <tt>RuleBasedTransliterator</tt> for
 * indexing.
 */
UBool Unicode_సమితి::matches_సూచీ_విలువ(uint8_t v) స్థిరరాశి {
    /* The index value v, in the range [0,255], is contained in this set if
     * it is contained in any pair of this set.  Pairs either have the high
     * bytes equal, or unequal.  If the high bytes are equal, then we have
     * aaxx..aayy, where aa is the high byte.  Then v is contained if xx <=
     * v <= yy.  If the high bytes are unequal we have aaxx..bbyy, bb>aa.
     * Then v is contained if xx <= v || v <= yy.  (This is identical to the
     * time zone month containment logic.)
     */
    int32_t i;
    int32_t range_లెక్క=get_Range_లెక్క();
    కొరకు (i=0; i<range_లెక్క; ++i) {
        UChar32 దిగువ = get_Range_ప్రారంభం(i);
        UChar32 ఎగువ = get_Range_చివర(i);
        ఒకవేళ ((దిగువ & ~0xFF) == (ఎగువ & ~0xFF)) {
            ఒకవేళ ((దిగువ & 0xFF) <= v && v <= (ఎగువ & 0xFF)) {
                తిరిగి నిజం;
            }
        } లేకపోతే ఒకవేళ ((దిగువ & 0xFF) <= v || v <= (ఎగువ & 0xFF)) {
            తిరిగి నిజం;
        }
    }
    ఒకవేళ (has_అక్షరశ్రేణులు()) {
        కొరకు (i=0; i<అక్షరశ్రేణులు_2->size(); ++i) {
            స్థిరరాశి UnicodeString& s = *static_cast<స్థిరరాశి UnicodeString*>(అక్షరశ్రేణులు_2->elementAt(i));
            ఒకవేళ (s.is_ఖాళీలు()) {
                కొనసాగు;  // skip the empty string
            }
            UChar32 c = s.char32At(0);
            ఒకవేళ ((c & 0xFF) == v) {
                తిరిగి నిజం;
            }
        }
    }
    తిరిగి అబద్ధం;
}

/**
 * Implementation of UnicodeMatcher::matches().  Always matches the
 * longest possible multichar string.
 */
UMatchDegree Unicode_సమితి::matches(స్థిరరాశి Replaceable& పాఠ్యం,
                                 int32_t& స్థానభ్రంశం,
                                 int32_t హద్దు,
                                 UBool incremental) {
    ఒకవేళ (స్థానభ్రంశం == హద్దు) {
        ఒకవేళ (కలిగియుంది(U_ETHER)) {
            తిరిగి incremental ? U_PARTIAL_సరిపోలిక : U_సరిపోలిక;
        } లేకపోతే {
            తిరిగి U_MISMATCH;
        }
    } లేకపోతే {
        ఒకవేళ (has_అక్షరశ్రేణులు()) { // try strings first

            // might separate forward and backward loops later
            // for now they are combined

            // TODO Improve efficiency of this, at least in the forward
            // direction, if not in both.  In the forward direction we
            // can assume the strings are sorted.

            int32_t i;
            UBool forward = స్థానభ్రంశం < హద్దు;

            // firstChar is the leftmost char to match in the
            // forward direction or the rightmost char to match in
            // the reverse direction.
            char16_t మొదటి_Char = పాఠ్యం.charAt(స్థానభ్రంశం);

            // If there are multiple strings that can match we
            // return the longest match.
            int32_t ఎగువ_Water_నిడివి = 0;

            కొరకు (i=0; i<అక్షరశ్రేణులు_2->size(); ++i) {
                స్థిరరాశి UnicodeString& trial = *static_cast<స్థిరరాశి UnicodeString*>(అక్షరశ్రేణులు_2->elementAt(i));
                ఒకవేళ (trial.is_ఖాళీలు()) {
                    కొనసాగు;  // skip the empty string
                }

                char16_t c = trial.charAt(forward ? 0 : trial.length() - 1);

                // Strings are sorted, so we can optimize in the
                // forward direction.
                ఒకవేళ (forward && c > మొదటి_Char) ఆపు;
                ఒకవేళ (c != మొదటి_Char) కొనసాగు;

                int32_t సరిపోలిక_Len = సరిపోలిక_Rest(పాఠ్యం, స్థానభ్రంశం, హద్దు, trial);

                ఒకవేళ (incremental) {
                    int32_t maxLen = forward ? హద్దు-స్థానభ్రంశం : స్థానభ్రంశం-హద్దు;
                    ఒకవేళ (సరిపోలిక_Len == maxLen) {
                        // We have successfully matched but only up to limit.
                        తిరిగి U_PARTIAL_సరిపోలిక;
                    }
                }

                ఒకవేళ (సరిపోలిక_Len == trial.length()) {
                    // We have successfully matched the whole string.
                    ఒకవేళ (సరిపోలిక_Len > ఎగువ_Water_నిడివి) {
                        ఎగువ_Water_నిడివి = సరిపోలిక_Len;
                    }
                    // In the forward direction we know strings
                    // are sorted so we can bail early.
                    ఒకవేళ (forward && సరిపోలిక_Len < ఎగువ_Water_నిడివి) {
                        ఆపు;
                    }
                    కొనసాగు;
                }
            }

            // We've checked all strings without a partial match.
            // If we have full matches, return the longest one.
            ఒకవేళ (ఎగువ_Water_నిడివి != 0) {
                స్థానభ్రంశం += forward ? ఎగువ_Water_నిడివి : -ఎగువ_Water_నిడివి;
                తిరిగి U_సరిపోలిక;
            }
        }
        తిరిగి UnicodeFilter::matches(పాఠ్యం, స్థానభ్రంశం, హద్దు, incremental);
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
int32_t Unicode_సమితి::సరిపోలిక_Rest(స్థిరరాశి Replaceable& పాఠ్యం,
                              int32_t ప్రారంభం, int32_t హద్దు,
                              స్థిరరాశి UnicodeString& s) {
    int32_t i;
    int32_t maxLen;
    int32_t slen = s.length();
    ఒకవేళ (ప్రారంభం < హద్దు) {
        maxLen = హద్దు - ప్రారంభం;
        ఒకవేళ (maxLen > slen) maxLen = slen;
        కొరకు (i = 1; i < maxLen; ++i) {
            ఒకవేళ (పాఠ్యం.charAt(ప్రారంభం + i) != s.charAt(i)) తిరిగి 0;
        }
    } లేకపోతే {
        maxLen = ప్రారంభం - హద్దు;
        ఒకవేళ (maxLen > slen) maxLen = slen;
        --slen; // <=> slen = s.length() - 1;
        కొరకు (i = 1; i < maxLen; ++i) {
            ఒకవేళ (పాఠ్యం.charAt(ప్రారంభం - i) != s.charAt(slen - i)) తిరిగి 0;
        }
    }
    తిరిగి maxLen;
}

/**
 * Implement of UnicodeMatcher
 */
శూన్య Unicode_సమితి::కూడు_సరిపోలిక_సమితి_గమ్యం(Unicode_సమితి& గమ్యం_Union_గమ్యం) స్థిరరాశి {
    గమ్యం_Union_గమ్యం.కూడు_All(*ఇది);
}

/**
 * Returns the index of the given character within this set, where
 * the set is ordered by ascending code point.  If the character
 * is not in this set, return -1.  The inverse of this method is
 * <code>charAt()</code>.
 * @return an index from 0..size()-1, or -1
 */
int32_t Unicode_సమితి::సూచీ_యొక్క(UChar32 c) స్థిరరాశి {
    ఒకవేళ (c < MIN_విలువ || c > MAX_విలువ) {
        తిరిగి -1;
    }
    int32_t i = 0;
    int32_t n = 0;
    కొరకు (;;) {
        UChar32 ప్రారంభం = list[i++];
        ఒకవేళ (c < ప్రారంభం) {
            తిరిగి -1;
        }
        UChar32 హద్దు = list[i++];
        ఒకవేళ (c < హద్దు) {
            తిరిగి n + c - ప్రారంభం;
        }
        n += హద్దు - ప్రారంభం;
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
UChar32 Unicode_సమితి::charAt(int32_t సూచీ) స్థిరరాశి {
    ఒకవేళ (సూచీ >= 0) {
        // len2 is the largest even integer <= len, that is, it is len
        // for even values and len-1 for odd values.  With odd values
        // the last entry is UNICODESET_HIGH.
        int32_t len2 = len & ~1;
        కొరకు (int32_t i=0; i < len2;) {
            UChar32 ప్రారంభం = list[i++];
            int32_t count = list[i++] - ప్రారంభం;
            ఒకవేళ (సూచీ < count) {
                తిరిగి static_cast<UChar32>(ప్రారంభం + సూచీ);
            }
            సూచీ -= count;
        }
    }
    తిరిగి static_cast<UChar32>(-1);
}

/**
 * Make this object represent the range <code>start - end</code>.
 * If <code>end > start</code> then this object is set to an
 * an empty range.
 *
 * @param start first character in the set, inclusive
 * @rparam end last character in the set, inclusive
 */
Unicode_సమితి& Unicode_సమితి::సమితి(UChar32 ప్రారంభం, UChar32 చివర) U_LIFETIME_BOUND {
    clear();
    complement(ప్రారంభం, చివర);
    తిరిగి *ఇది;
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
Unicode_సమితి& Unicode_సమితి::కూడు(UChar32 ప్రారంభం, UChar32 చివర) U_LIFETIME_BOUND {
    ఒకవేళ (pin_సంకేతం_బిందువు(ప్రారంభం) < pin_సంకేతం_బిందువు(చివర)) {
        UChar32 హద్దు = చివర + 1;
        // Fast path for adding a new range after the last one.
        // Odd list length: [..., lastStart, lastLimit, HIGH]
        ఒకవేళ ((len & 1) != 0) {
            // If the list is empty, set lastLimit low enough to not be adjacent to 0.
            UChar32 చివరి_హద్దు = len == 1 ? -2 : list[len - 2];
            ఒకవేళ (చివరి_హద్దు <= ప్రారంభం && !isFrozen() && !isBogus()) {
                ఒకవేళ (చివరి_హద్దు == ప్రారంభం) {
                    // Extend the last range.
                    list[len - 2] = హద్దు;
                    ఒకవేళ (హద్దు == UNICODESET_ఎగువ) {
                        --len;
                    }
                } లేకపోతే {
                    list[len - 1] = ప్రారంభం;
                    ఒకవేళ (హద్దు < UNICODESET_ఎగువ) {
                        ఒకవేళ (ensure_సామర్థ్యం(len + 2)) {
                            list[len++] = హద్దు;
                            list[len++] = UNICODESET_ఎగువ;
                        }
                    } లేకపోతే {  // limit == UNICODESET_HIGH
                        ఒకవేళ (ensure_సామర్థ్యం(len + 1)) {
                            list[len++] = UNICODESET_ఎగువ;
                        }
                    }
                }
                release_నమూనా();
                తిరిగి *ఇది;
            }
        }
        // This is slow. Could be much faster using findCodePoint(start)
        // and modifying the list, dealing with adjacent & overlapping ranges.
        UChar32 range[3] = { ప్రారంభం, హద్దు, UNICODESET_ఎగువ };
        కూడు(range, 2, 0);
    } లేకపోతే ఒకవేళ (ప్రారంభం == చివర) {
        కూడు(ప్రారంభం);
    }
    తిరిగి *ఇది;
}

// #define DEBUG_US_ADD

#నిర్వచితమైతే DEBUG_US_కూడు
#అన్తర్భావ <stdio.h>
శూన్య dump(UChar32 c) {
    ఒకవేళ (c <= 0xFF) {
        రాయి("%c", (అక్షరం)c);
    } లేకపోతే {
        రాయి("U+%04X", c);
    }
}
శూన్య dump(స్థిరరాశి UChar32* list, int32_t len) {
    రాయి("[");
    కొరకు (int32_t i=0; i<len; ++i) {
        ఒకవేళ (i != 0) రాయి(", ");
        dump(list[i]);
    }
    రాయి("]");
}
#అయితే_ముగింపు

/**
 * Adds the specified character to this set if it is not already
 * present.  If this set already contains the specified character,
 * the call leaves this set unchanged.
 */
Unicode_సమితి& Unicode_సమితి::కూడు(UChar32 c) U_LIFETIME_BOUND {
    // find smallest i such that c < list[i]
    // if odd, then it is IN the set
    // if even, then it is OUT of the set
    int32_t i = కనుగొను_సంకేతం_బిందువు(pin_సంకేతం_బిందువు(c));

    // already in set?
    ఒకవేళ ((i & 1) != 0  || isFrozen() || isBogus()) తిరిగి *ఇది;

    // HIGH is 0x110000
    // assert(list[len-1] == HIGH);

    // empty = [HIGH]
    // [start_0, limit_0, start_1, limit_1, HIGH]

    // [..., start_k-1, limit_k-1, start_k, limit_k, ..., HIGH]
    //                             ^
    //                             list[i]

    // i == 0 means c is before the first range

#నిర్వచితమైతే DEBUG_US_కూడు
    రాయి("Add of ");
    dump(c);
    రాయి(" found at %d", i);
    రాయి(": ");
    dump(list, len);
    రాయి(" => ");
#అయితే_ముగింపు

    ఒకవేళ (c == list[i]-1) {
        // c is before start of next range
        list[i] = c;
        // if we touched the HIGH mark, then add a new one
        ఒకవేళ (c == (UNICODESET_ఎగువ - 1)) {
            ఒకవేళ (!ensure_సామర్థ్యం(len+1)) {
                // ensureCapacity will mark the object as Bogus if OOM failure happens.
                తిరిగి *ఇది;
            }
            list[len++] = UNICODESET_ఎగువ;
        }
        ఒకవేళ (i > 0 && c == list[i-1]) {
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
            అయితే (src < srclimit) *(dst++) = *(src++);

            len -= 2;
        }
    }

    లేకపోతే ఒకవేళ (i > 0 && c == list[i-1]) {
        // c is after end of prior range
        list[i-1]++;
        // no need to check for collapse here
    }

    లేకపోతే {
        // At this point we know the new char is not adjacent to
        // any existing ranges, and it is not 10FFFF.


        // [..., start_k-1, limit_k-1, start_k, limit_k, ..., HIGH]
        //                             ^
        //                             list[i]

        // [..., start_k-1, limit_k-1, c, c+1, start_k, limit_k, ..., HIGH]
        //                             ^
        //                             list[i]

        ఒకవేళ (!ensure_సామర్థ్యం(len+2)) {
            // ensureCapacity will mark the object as Bogus if OOM failure happens.
            తిరిగి *ఇది;
        }

        UChar32 *p = list + i;
        uprv_memmove(p + 2, p, (len - i) * పరిమాణం(*p));
        list[i] = c;
        list[i+1] = c+1;
        len += 2;
    }

#నిర్వచితమైతే DEBUG_US_కూడు
    dump(list, len);
    రాయి("\n");

    కొరకు (i=1; i<len; ++i) {
        ఒకవేళ (list[i] <= list[i-1]) {
            // Corrupt array!
            రాయి("ERROR: list has been corrupted\n");
            నిష్క్రమించు(1);
        }
    }
#అయితే_ముగింపు

    release_నమూనా();
    తిరిగి *ఇది;
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
Unicode_సమితి& Unicode_సమితి::కూడు(స్థిరరాశి UnicodeString& s) U_LIFETIME_BOUND {
    ఒకవేళ (isFrozen() || isBogus()) తిరిగి *ఇది;
    int32_t cp = getSingleCP(s);
    ఒకవేళ (cp < 0) {
        ఒకవేళ (!అక్షరశ్రేణులు_కలిగియుంది(s)) {
            కూడు_2(s);
            release_నమూనా();
        }
    } లేకపోతే {
        కూడు(static_cast<UChar32>(cp));
    }
    తిరిగి *ఇది;
}

/**
 * Adds the given string, in order, to 'strings_'.  The given string
 * must have been checked by the caller to not already be in 'strings_'.
 */
శూన్య Unicode_సమితి::కూడు_2(స్థిరరాశి UnicodeString& s) {
    ఒకవేళ (isFrozen() || isBogus()) {
        తిరిగి;
    }
    UError_సంకేతం ec = U_ZERO_ERROR;
    ఒకవేళ (అక్షరశ్రేణులు_2 == శూన్యసూచిక && !allocate_అక్షరశ్రేణులు(ec)) {
        సమితి_గమ్యం_Bogus();
        తిరిగి;
    }
    Local_సూచిక<UnicodeString> t(కొత్త UnicodeString(s));
    ఒకవేళ (t.isNull()) { // Check for memory allocation error.
        సమితి_గమ్యం_Bogus();
        తిరిగి;
    }
    అక్షరశ్రేణులు_2->sorted_చొప్పించు(t.orphan(), compareUnicodeString, ec);
    ఒకవేళ (U_FAILURE(ec)) {
        సమితి_గమ్యం_Bogus();
    }
}

/**
 * @return a code point IF the string consists of a single one.
 * otherwise returns -1.
 * @param string to test
 */
int32_t Unicode_సమితి::getSingleCP(స్థిరరాశి UnicodeString& s) {
    int32_t s_నిడివి = s.length();
    ఒకవేళ (s_నిడివి == 1) తిరిగి s.charAt(0);
    ఒకవేళ (s_నిడివి == 2) {
        UChar32 cp = s.char32At(0);
        ఒకవేళ (cp > 0xFFFF) { // is surrogate pair
            తిరిగి cp;
        }
    }
    తిరిగి -1;
}

/**
 * Adds each of the characters in this string to the set. Thus "ch" => {"c", "h"}
 * If this set already any particular character, it has no effect on that character.
 * @param the source string
 * @return the modified set, for chaining
 */
Unicode_సమితి& Unicode_సమితి::కూడు_All(స్థిరరాశి UnicodeString& s) U_LIFETIME_BOUND {
    UChar32 cp;
    కొరకు (int32_t i = 0; i < s.length(); i += U16_నిడివి(cp)) {
        cp = s.char32At(i);
        కూడు(cp);
    }
    తిరిగి *ఇది;
}

/**
 * Retains EACH of the characters in this string. Note: "ch" == {"c", "h"}
 * If this set already any particular character, it has no effect on that character.
 * @param the source string
 * @return the modified set, for chaining
 */
Unicode_సమితి& Unicode_సమితి::retainAll(స్థిరరాశి UnicodeString& s) U_LIFETIME_BOUND {
    Unicode_సమితి సమితి;
    సమితి.కూడు_All(s);
    retainAll(సమితి);
    తిరిగి *ఇది;
}

/**
 * Complement EACH of the characters in this string. Note: "ch" == {"c", "h"}
 * If this set already any particular character, it has no effect on that character.
 * @param the source string
 * @return the modified set, for chaining
 */
Unicode_సమితి& Unicode_సమితి::complementAll(స్థిరరాశి UnicodeString& s) U_LIFETIME_BOUND {
    Unicode_సమితి సమితి;
    సమితి.కూడు_All(s);
    complementAll(సమితి);
    తిరిగి *ఇది;
}

/**
 * Remove EACH of the characters in this string. Note: "ch" == {"c", "h"}
 * If this set already any particular character, it has no effect on that character.
 * @param the source string
 * @return the modified set, for chaining
 */
Unicode_సమితి& Unicode_సమితి::removeAll(స్థిరరాశి UnicodeString& s) U_LIFETIME_BOUND {
    Unicode_సమితి సమితి;
    సమితి.కూడు_All(s);
    removeAll(సమితి);
    తిరిగి *ఇది;
}

Unicode_సమితి& Unicode_సమితి::remove_All_అక్షరశ్రేణులు() U_LIFETIME_BOUND {
    ఒకవేళ (!isFrozen() && has_అక్షరశ్రేణులు()) {
        అక్షరశ్రేణులు_2->removeAllElements();
        release_నమూనా();
    }
    తిరిగి *ఇది;
}


/**
 * Makes a set from a multicharacter string. Thus "ch" => {"ch"}
 * <br><b>Warning: you cannot add an empty string ("") to a UnicodeSet.</b>
 * @param the source string
 * @return a newly created set containing the given string
 */
Unicode_సమితి* U_EXPORT2 Unicode_సమితి::createFrom(స్థిరరాశి UnicodeString& s) {
    Unicode_సమితి *సమితి = కొత్త Unicode_సమితి();
    ఒకవేళ (సమితి != శూన్యసూచిక) { // Check for memory allocation error.
        సమితి->కూడు(s);
    }
    తిరిగి సమితి;
}


/**
 * Makes a set from each of the characters in the string. Thus "ch" => {"c", "h"}
 * @param the source string
 * @return a newly created set containing the given characters
 */
Unicode_సమితి* U_EXPORT2 Unicode_సమితి::createFromAll(స్థిరరాశి UnicodeString& s) {
    Unicode_సమితి *సమితి = కొత్త Unicode_సమితి();
    ఒకవేళ (సమితి != శూన్యసూచిక) { // Check for memory allocation error.
        సమితి->కూడు_All(s);
    }
    తిరిగి సమితి;
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
Unicode_సమితి& Unicode_సమితి::retain(UChar32 ప్రారంభం, UChar32 చివర) U_LIFETIME_BOUND {
    ఒకవేళ (pin_సంకేతం_బిందువు(ప్రారంభం) <= pin_సంకేతం_బిందువు(చివర)) {
        UChar32 range[3] = { ప్రారంభం, చివర+1, UNICODESET_ఎగువ };
        retain(range, 2, 0);
    } లేకపోతే {
        clear();
    }
    తిరిగి *ఇది;
}

Unicode_సమితి& Unicode_సమితి::retain(UChar32 c) U_LIFETIME_BOUND {
    తిరిగి retain(c, c);
}

Unicode_సమితి& Unicode_సమితి::retain(స్థిరరాశి UnicodeString &s) U_LIFETIME_BOUND {
    ఒకవేళ (isFrozen() || isBogus()) { తిరిగి *ఇది; }
    UChar32 cp = getSingleCP(s);
    ఒకవేళ (cp < 0) {
        తార్కిక is_లోపల = అక్షరశ్రేణులు_కలిగియుంది(s);
        // Check for getRangeCount() first to avoid somewhat-expensive size()
        // when there are single code points.
        ఒకవేళ (is_లోపల && get_Range_లెక్క() == 0 && size() == 1) {
            తిరిగి *ఇది;
        }
        clear();
        ఒకవేళ (is_లోపల) {
            కూడు_2(s);
        }
    } లేకపోతే {
        retain(cp, cp);
    }
    తిరిగి *ఇది;
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
Unicode_సమితి& Unicode_సమితి::remove(UChar32 ప్రారంభం, UChar32 చివర) U_LIFETIME_BOUND {
    ఒకవేళ (pin_సంకేతం_బిందువు(ప్రారంభం) <= pin_సంకేతం_బిందువు(చివర)) {
        UChar32 range[3] = { ప్రారంభం, చివర+1, UNICODESET_ఎగువ };
        retain(range, 2, 2);
    }
    తిరిగి *ఇది;
}

/**
 * Removes the specified character from this set if it is present.
 * The set will not contain the specified range once the call
 * returns.
 */
Unicode_సమితి& Unicode_సమితి::remove(UChar32 c) U_LIFETIME_BOUND {
    తిరిగి remove(c, c);
}

/**
 * Removes the specified string from this set if it is present.
 * The set will not contain the specified character once the call
 * returns.
 * @param the source string
 * @return the modified set, for chaining
 */
Unicode_సమితి& Unicode_సమితి::remove(స్థిరరాశి UnicodeString& s) U_LIFETIME_BOUND {
    ఒకవేళ (isFrozen() || isBogus()) తిరిగి *ఇది;
    int32_t cp = getSingleCP(s);
    ఒకవేళ (cp < 0) {
        ఒకవేళ (అక్షరశ్రేణులు_2 != శూన్యసూచిక && అక్షరశ్రేణులు_2->removeElement((శూన్య*) &s)) {
            release_నమూనా();
        }
    } లేకపోతే {
        remove(static_cast<UChar32>(cp), static_cast<UChar32>(cp));
    }
    తిరిగి *ఇది;
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
Unicode_సమితి& Unicode_సమితి::complement(UChar32 ప్రారంభం, UChar32 చివర) U_LIFETIME_BOUND {
    ఒకవేళ (isFrozen() || isBogus()) {
        తిరిగి *ఇది;
    }
    ఒకవేళ (pin_సంకేతం_బిందువు(ప్రారంభం) <= pin_సంకేతం_బిందువు(చివర)) {
        UChar32 range[3] = { ప్రారంభం, చివర+1, UNICODESET_ఎగువ };
        exclusiveOr(range, 2, 0);
    }
    release_నమూనా();
    తిరిగి *ఇది;
}

Unicode_సమితి& Unicode_సమితి::complement(UChar32 c) U_LIFETIME_BOUND {
    తిరిగి complement(c, c);
}

/**
 * This is equivalent to
 * <code>complement(MIN_VALUE, MAX_VALUE)</code>.
 */
Unicode_సమితి& Unicode_సమితి::complement() U_LIFETIME_BOUND {
    ఒకవేళ (isFrozen() || isBogus()) {
        తిరిగి *ఇది;
    }
    ఒకవేళ (list[0] == UNICODESET_దిగువ) {
        uprv_memmove(list, list + 1, (size_t)(len-1)*పరిమాణం(UChar32));
        --len;
    } లేకపోతే {
        ఒకవేళ (!ensure_సామర్థ్యం(len+1)) {
            తిరిగి *ఇది;
        }
        uprv_memmove(list + 1, list, (size_t)len*పరిమాణం(UChar32));
        list[0] = UNICODESET_దిగువ;
        ++len;
    }
    release_నమూనా();
    తిరిగి *ఇది;
}

/**
 * Complement the specified string in this set.
 * The set will not contain the specified string once the call
 * returns.
 *
 * @param s the string to complement
 * @return this object, for chaining
 */
Unicode_సమితి& Unicode_సమితి::complement(స్థిరరాశి UnicodeString& s) U_LIFETIME_BOUND {
    ఒకవేళ (isFrozen() || isBogus()) తిరిగి *ఇది;
    int32_t cp = getSingleCP(s);
    ఒకవేళ (cp < 0) {
        ఒకవేళ (అక్షరశ్రేణులు_కలిగియుంది(s)) {
            అక్షరశ్రేణులు_2->removeElement((శూన్య*) &s);
        } లేకపోతే {
            కూడు_2(s);
        }
        release_నమూనా();
    } లేకపోతే {
        complement(static_cast<UChar32>(cp), static_cast<UChar32>(cp));
    }
    తిరిగి *ఇది;
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
Unicode_సమితి& Unicode_సమితి::కూడు_All(స్థిరరాశి Unicode_సమితి& c) U_LIFETIME_BOUND {
    ఒకవేళ ( c.len>0 && c.list!=శూన్యసూచిక ) {
        కూడు(c.list, c.len, 0);
    }

    // Add strings in order
    ఒకవేళ ( c.అక్షరశ్రేణులు_2!=శూన్యసూచిక ) {
        కొరకు (int32_t i=0; i<c.అక్షరశ్రేణులు_2->size(); ++i) {
            స్థిరరాశి UnicodeString* s = static_cast<స్థిరరాశి UnicodeString*>(c.అక్షరశ్రేణులు_2->elementAt(i));
            ఒకవేళ (!అక్షరశ్రేణులు_కలిగియుంది(*s)) {
                కూడు_2(*s);
            }
        }
    }
    తిరిగి *ఇది;
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
Unicode_సమితి& Unicode_సమితి::retainAll(స్థిరరాశి Unicode_సమితి& c) U_LIFETIME_BOUND {
    ఒకవేళ (isFrozen() || isBogus()) {
        తిరిగి *ఇది;
    }
    retain(c.list, c.len, 0);
    ఒకవేళ (has_అక్షరశ్రేణులు()) {
        ఒకవేళ (!c.has_అక్షరశ్రేణులు()) {
            అక్షరశ్రేణులు_2->removeAllElements();
        } లేకపోతే {
            అక్షరశ్రేణులు_2->retainAll(*c.అక్షరశ్రేణులు_2);
        }
    }
    తిరిగి *ఇది;
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
Unicode_సమితి& Unicode_సమితి::removeAll(స్థిరరాశి Unicode_సమితి& c) U_LIFETIME_BOUND {
    ఒకవేళ (isFrozen() || isBogus()) {
        తిరిగి *ఇది;
    }
    retain(c.list, c.len, 2);
    ఒకవేళ (has_అక్షరశ్రేణులు() && c.has_అక్షరశ్రేణులు()) {
        అక్షరశ్రేణులు_2->removeAll(*c.అక్షరశ్రేణులు_2);
    }
    తిరిగి *ఇది;
}

/**
 * Complements in this set all elements contained in the specified
 * set.  Any character in the other set will be removed if it is
 * in this set, or will be added if it is not in this set.
 *
 * @param c set that defines which elements will be xor'ed from
 *          this set.
 */
Unicode_సమితి& Unicode_సమితి::complementAll(స్థిరరాశి Unicode_సమితి& c) U_LIFETIME_BOUND {
    ఒకవేళ (isFrozen() || isBogus()) {
        తిరిగి *ఇది;
    }
    exclusiveOr(c.list, c.len, 0);

    ఒకవేళ (c.అక్షరశ్రేణులు_2 != శూన్యసూచిక) {
        కొరకు (int32_t i=0; i<c.అక్షరశ్రేణులు_2->size(); ++i) {
            శూన్య* e = c.అక్షరశ్రేణులు_2->elementAt(i);
            ఒకవేళ (అక్షరశ్రేణులు_2 == శూన్యసూచిక || !అక్షరశ్రేణులు_2->removeElement(e)) {
                కూడు_2(*static_cast<స్థిరరాశి UnicodeString*>(e));
            }
        }
    }
    తిరిగి *ఇది;
}

/**
 * Removes all of the elements from this set.  This set will be
 * empty after this call returns.
 */
Unicode_సమితి& Unicode_సమితి::clear() U_LIFETIME_BOUND {
    ఒకవేళ (isFrozen()) {
        తిరిగి *ఇది;
    }
    list[0] = UNICODESET_ఎగువ;
    len = 1;
    release_నమూనా();
    ఒకవేళ (అక్షరశ్రేణులు_2 != శూన్యసూచిక) {
        అక్షరశ్రేణులు_2->removeAllElements();
    }
    // Remove bogus
    fFlags = 0;
    తిరిగి *ఇది;
}

/**
 * Iteration method that returns the number of ranges contained in
 * this set.
 * @see #getRangeStart
 * @see #getRangeEnd
 */
int32_t Unicode_సమితి::get_Range_లెక్క() స్థిరరాశి {
    తిరిగి len/2;
}

/**
 * Iteration method that returns the first character in the
 * specified range of this set.
 * @see #getRangeCount
 * @see #getRangeEnd
 */
UChar32 Unicode_సమితి::get_Range_ప్రారంభం(int32_t సూచీ) స్థిరరాశి {
    తిరిగి list[సూచీ*2];
}

/**
 * Iteration method that returns the last character in the
 * specified range of this set.
 * @see #getRangeStart
 * @see #getRangeEnd
 */
UChar32 Unicode_సమితి::get_Range_చివర(int32_t సూచీ) స్థిరరాశి {
    తిరిగి list[సూచీ*2 + 1] - 1;
}

స్థిరరాశి UnicodeString* Unicode_సమితి::getString(int32_t సూచీ) స్థిరరాశి U_LIFETIME_BOUND {
    తిరిగి static_cast<స్థిరరాశి UnicodeString*>(అక్షరశ్రేణులు_2->elementAt(సూచీ));
}

/**
 * Reallocate this objects internal structures to take up the least
 * possible space, without changing this object's value.
 */
Unicode_సమితి& Unicode_సమితి::compact() U_LIFETIME_BOUND {
    ఒకవేళ (isFrozen() || isBogus()) {
        తిరిగి *ఇది;
    }
    // Delete buffer first to defragment memory less.
    ఒకవేళ (బఫరు != దొంతర_List) {
        uprv_free(బఫరు);
        బఫరు = శూన్యసూచిక;
        బఫరు_సామర్థ్యం = 0;
    }
    ఒకవేళ (list == దొంతర_List) {
        // pass
    } లేకపోతే ఒకవేళ (len <= INITIAL_సామర్థ్యం) {
        uprv_memcpy(దొంతర_List, list, len * పరిమాణం(UChar32));
        uprv_free(list);
        list = దొంతర_List;
        capacity = INITIAL_సామర్థ్యం;
    } లేకపోతే ఒకవేళ ((len + 7) < capacity) {
        // If we have more than a little unused capacity, shrink it to len.
        UChar32* తాత్కాలిక = static_cast<UChar32*>(uprv_realloc(list, పరిమాణం(UChar32) * len));
        ఒకవేళ (తాత్కాలిక) {
            list = తాత్కాలిక;
            capacity = len;
        }
        // else what the heck happened?! We allocated less memory!
        // Oh well. We'll keep our original array.
    }
    ఒకవేళ (అక్షరశ్రేణులు_2 != శూన్యసూచిక && అక్షరశ్రేణులు_2->is_ఖాళీలు()) {
        తొలగించు అక్షరశ్రేణులు_2;
        అక్షరశ్రేణులు_2 = శూన్యసూచిక;
    }
    తిరిగి *ఇది;
}

#నిర్వచితమైతే DEBUG_SERIALIZE
#అన్తర్భావ <stdio.h>
#అయితే_ముగింపు

/**
 * Deserialize constructor.
 */
Unicode_సమితి::Unicode_సమితి(స్థిరరాశి uint16_t దత్తాంశం[], int32_t దత్తాంశం_Len, ESerialization serialization,
                       UError_సంకేతం &ec) {

  ఒకవేళ(U_FAILURE(ec)) {
    సమితి_గమ్యం_Bogus();
    తిరిగి;
  }

  ఒకవేళ( (serialization != kSerialized)
      || (దత్తాంశం==శూన్యసూచిక)
      || (దత్తాంశం_Len < 1)) {
    ec = U_ILLEGAL_వాదం_ERROR;
    సమితి_గమ్యం_Bogus();
    తిరిగి;
  }

  // bmp?
  int32_t శీర్షిక_కొలత = ((దత్తాంశం[0]&0x8000)) ?2:1;
  int32_t bmp_నిడివి = (శీర్షిక_కొలత==1)?దత్తాంశం[0]:దత్తాంశం[1];

  int32_t new_నిడివి = (((దత్తాంశం[0]&0x7FFF)-bmp_నిడివి)/2)+bmp_నిడివి;
#నిర్వచితమైతే DEBUG_SERIALIZE
  రాయి("dataLen %d headerSize %d bmpLen %d len %d. data[0]=%X/%X/%X/%X\n", దత్తాంశం_Len,శీర్షిక_కొలత,bmp_నిడివి,new_నిడివి, దత్తాంశం[0],దత్తాంశం[1],దత్తాంశం[2],దత్తాంశం[3]);
#అయితే_ముగింపు
  ఒకవేళ(!ensure_సామర్థ్యం(new_నిడివి + 1)) {  // +1 for HIGH
    తిరిగి;
  }
  // copy bmp
  int32_t i;
  కొరకు(i = 0; i< bmp_నిడివి;i++) {
    list[i] = దత్తాంశం[i+శీర్షిక_కొలత];
#నిర్వచితమైతే DEBUG_SERIALIZE
    రాయి("<<16@%d[%d] %X\n", i+శీర్షిక_కొలత, i, list[i]);
#అయితే_ముగింపు
  }
  // copy smp
  కొరకు(i=bmp_నిడివి;i<new_నిడివి;i++) {
    list[i] = (static_cast<UChar32>(దత్తాంశం[శీర్షిక_కొలత + bmp_నిడివి + (i - bmp_నిడివి) * 2 + 0]) << 16) +
               static_cast<UChar32>(దత్తాంశం[శీర్షిక_కొలత + bmp_నిడివి + (i - bmp_నిడివి) * 2 + 1]);
#నిర్వచితమైతే DEBUG_SERIALIZE
    రాయి("<<32@%d+[%d] %lX\n", శీర్షిక_కొలత+bmp_నిడివి+i, i, list[i]);
#అయితే_ముగింపు
  }
  U_ASSERT(i == new_నిడివి);
  ఒకవేళ (i == 0 || list[i - 1] != UNICODESET_ఎగువ) {
    list[i++] = UNICODESET_ఎగువ;
  }
  len = i;
}


int32_t Unicode_సమితి::serialize(uint16_t *dest, int32_t dest_సామర్థ్యం, UError_సంకేతం& ec) స్థిరరాశి {
    int32_t bmp_నిడివి, length, dest_నిడివి;

    ఒకవేళ (U_FAILURE(ec)) {
        తిరిగి 0;
    }

    ఒకవేళ (dest_సామర్థ్యం<0 || (dest_సామర్థ్యం>0 && dest==శూన్యసూచిక)) {
        ec=U_ILLEGAL_వాదం_ERROR;
        తిరిగి 0;
    }

    /* count necessary 16-bit units */
    length=ఇది->len-1; // Subtract 1 to ignore final UNICODESET_HIGH
    // assert(length>=0);
    ఒకవేళ (length==0) {
        /* empty set */
        ఒకవేళ (dest_సామర్థ్యం>0) {
            *dest=0;
        } లేకపోతే {
            ec=U_బఫరు_అధికప్రవాహం_ERROR;
        }
        తిరిగి 1;
    }
    /* now length>0 */

    ఒకవేళ (ఇది->list[length-1]<=0xffff) {
        /* all BMP */
        bmp_నిడివి=length;
    } లేకపోతే ఒకవేళ (ఇది->list[0]>=0x10000) {
        /* all supplementary */
        bmp_నిడివి=0;
        length*=2;
    } లేకపోతే {
        /* some BMP, some supplementary */
        కొరకు (bmp_నిడివి=0; bmp_నిడివి<length && ఇది->list[bmp_నిడివి]<=0xffff; ++bmp_నిడివి) {}
        length=bmp_నిడివి+2*(length-bmp_నిడివి);
    }
#నిర్వచితమైతే DEBUG_SERIALIZE
    రాయి(">> bmpLength%d length%d len%d\n", bmp_నిడివి, length, len);
#అయితే_ముగింపు
    /* length: number of 16-bit array units */
    ఒకవేళ (length>0x7fff) {
        /* there are only 15 bits for the length in the first serialized word */
        ec=U_సూచీ_OUTOFBOUNDS_ERROR;
        తిరిగి 0;
    }

    /*
     * total serialized length:
     * number of 16-bit array units (length) +
     * 1 length unit (always) +
     * 1 bmpLength unit (if there are supplementary values)
     */
    dest_నిడివి=length+((length>bmp_నిడివి)?2:1);
    ఒకవేళ (dest_నిడివి<=dest_సామర్థ్యం) {
        స్థిరరాశి UChar32 *p;
        int32_t i;

#నిర్వచితమైతే DEBUG_SERIALIZE
        రాయి("writeHdr\n");
#అయితే_ముగింపు
        *dest = static_cast<uint16_t>(length);
        ఒకవేళ (length>bmp_నిడివి) {
            *dest|=0x8000;
            *++dest = static_cast<uint16_t>(bmp_నిడివి);
        }
        ++dest;

        /* write the BMP part of the array */
        p=ఇది->list;
        కొరకు (i=0; i<bmp_నిడివి; ++i) {
#నిర్వచితమైతే DEBUG_SERIALIZE
          రాయి("writebmp: %x\n", (పూర్ణాంకం)*p);
#అయితే_ముగింపు
            *dest++ = static_cast<uint16_t>(*p++);
        }

        /* write the supplementary part of the array */
        కొరకు (; i<length; i+=2) {
#నిర్వచితమైతే DEBUG_SERIALIZE
          రాయి("write32: %x\n", (పూర్ణాంకం)*p);
#అయితే_ముగింపు
            *dest++ = static_cast<uint16_t>(*p >> 16);
            *dest++ = static_cast<uint16_t>(*p++);
        }
    } లేకపోతే {
        ec=U_బఫరు_అధికప్రవాహం_ERROR;
    }
    తిరిగి dest_నిడివి;
}

//----------------------------------------------------------------
// Implementation: Utility methods
//----------------------------------------------------------------

/**
 * Allocate our strings vector and return true if successful.
 */
UBool Unicode_సమితి::allocate_అక్షరశ్రేణులు(UError_సంకేతం &స్థితి_2) {
    ఒకవేళ (U_FAILURE(స్థితి_2)) {
        తిరిగి అబద్ధం;
    }
    అక్షరశ్రేణులు_2 = కొత్త UVector(uprv_deleteUObject,
                          uhash_compareUnicodeString, 1, స్థితి_2);
    ఒకవేళ (అక్షరశ్రేణులు_2 == శూన్యసూచిక) { // Check for memory allocation error.
        స్థితి_2 = U_జ్ఞాపకం_కేటాయింపు_ERROR;
        తిరిగి అబద్ధం;
    }
    ఒకవేళ (U_FAILURE(స్థితి_2)) {
        తొలగించు అక్షరశ్రేణులు_2;
        అక్షరశ్రేణులు_2 = శూన్యసూచిక;
        తిరిగి అబద్ధం;
    } 
    తిరిగి నిజం;
}

int32_t Unicode_సమితి::తదుపరి_సామర్థ్యం(int32_t min_సామర్థ్యం) {
    // Grow exponentially to reduce the frequency of allocations.
    ఒకవేళ (min_సామర్థ్యం < INITIAL_సామర్థ్యం) {
        తిరిగి min_సామర్థ్యం + INITIAL_సామర్థ్యం;
    } లేకపోతే ఒకవేళ (min_సామర్థ్యం <= 2500) {
        తిరిగి 5 * min_సామర్థ్యం;
    } లేకపోతే {
        int32_t new_సామర్థ్యం = 2 * min_సామర్థ్యం;
        ఒకవేళ (new_సామర్థ్యం > MAX_నిడివి) {
            new_సామర్థ్యం = MAX_నిడివి;
        }
        తిరిగి new_సామర్థ్యం;
    }
}

తార్కిక Unicode_సమితి::ensure_సామర్థ్యం(int32_t newLen) {
    ఒకవేళ (newLen > MAX_నిడివి) {
        newLen = MAX_నిడివి;
    }
    ఒకవేళ (newLen <= capacity) {
        తిరిగి నిజం;
    }
    int32_t new_సామర్థ్యం = తదుపరి_సామర్థ్యం(newLen);
    UChar32* తాత్కాలిక = static_cast<UChar32*>(uprv_malloc(new_సామర్థ్యం * పరిమాణం(UChar32)));
    ఒకవేళ (తాత్కాలిక == శూన్యసూచిక) {
        సమితి_గమ్యం_Bogus(); // set the object to bogus state if an OOM failure occurred.
        తిరిగి అబద్ధం;
    }
    // Copy only the actual contents.
    uprv_memcpy(తాత్కాలిక, list, len * పరిమాణం(UChar32));
    ఒకవేళ (list != దొంతర_List) {
        uprv_free(list);
    }
    list = తాత్కాలిక;
    capacity = new_సామర్థ్యం;
    తిరిగి నిజం;
}

తార్కిక Unicode_సమితి::ensure_బఫరు_సామర్థ్యం(int32_t newLen) {
    ఒకవేళ (newLen > MAX_నిడివి) {
        newLen = MAX_నిడివి;
    }
    ఒకవేళ (newLen <= బఫరు_సామర్థ్యం) {
        తిరిగి నిజం;
    }
    int32_t new_సామర్థ్యం = తదుపరి_సామర్థ్యం(newLen);
    UChar32* తాత్కాలిక = static_cast<UChar32*>(uprv_malloc(new_సామర్థ్యం * పరిమాణం(UChar32)));
    ఒకవేళ (తాత్కాలిక == శూన్యసూచిక) {
        సమితి_గమ్యం_Bogus();
        తిరిగి అబద్ధం;
    }
    // The buffer has no contents to be copied.
    // It is always filled from scratch after this call.
    ఒకవేళ (బఫరు != దొంతర_List) {
        uprv_free(బఫరు);
    }
    బఫరు = తాత్కాలిక;
    బఫరు_సామర్థ్యం = new_సామర్థ్యం;
    తిరిగి నిజం;
}

/**
 * Swap list and buffer.
 */
శూన్య Unicode_సమితి::మార్పిడి_Buffers() {
    // swap list and buffer
    UChar32* తాత్కాలిక = list;
    list = బఫరు;
    బఫరు = తాత్కాలిక;

    int32_t c = capacity;
    capacity = బఫరు_సామర్థ్యం;
    బఫరు_సామర్థ్యం = c;
}

శూన్య Unicode_సమితి::సమితి_గమ్యం_Bogus() {
    clear(); // Remove everything in the set.
    fFlags = kIsBogus;
}

//----------------------------------------------------------------
// Implementation: Fundamental operators
//----------------------------------------------------------------

స్థిర అంతఃవరుస UChar32 max(UChar32 a, UChar32 b) {
    తిరిగి (a > b) ? a : b;
}

// polarity = 0, 3 is normal: x xor y
// polarity = 1, 2: x xor ~y == x === y

శూన్య Unicode_సమితి::exclusiveOr(స్థిరరాశి UChar32* other, int32_t otherLen, int8_t polarity) {
    ఒకవేళ (isFrozen() || isBogus()) {
        తిరిగి;
    }
    ఒకవేళ (!ensure_బఫరు_సామర్థ్యం(len + otherLen)) {
        తిరిగి;
    }

    int32_t i = 0, j = 0, k = 0;
    UChar32 a = list[i++];
    UChar32 b;
    ఒకవేళ (polarity == 1 || polarity == 2) {
        b = UNICODESET_దిగువ;
        ఒకవేళ (other[j] == UNICODESET_దిగువ) { // skip base if already LOW
            ++j;
            b = other[j];
        }
    } లేకపోతే {
        b = other[j++];
    }
    // simplest of all the routines
    // sort the values, discarding identicals!
    కొరకు (;;) {
        ఒకవేళ (a < b) {
            బఫరు[k++] = a;
            a = list[i++];
        } లేకపోతే ఒకవేళ (b < a) {
            బఫరు[k++] = b;
            b = other[j++];
        } లేకపోతే ఒకవేళ (a != UNICODESET_ఎగువ) { // at this point, a == b
            // discard both values!
            a = list[i++];
            b = other[j++];
        } లేకపోతే { // DONE!
            బఫరు[k++] = UNICODESET_ఎగువ;
            len = k;
            ఆపు;
        }
    }
    మార్పిడి_Buffers();
    release_నమూనా();
}

// polarity = 0 is normal: x union y
// polarity = 2: x union ~y
// polarity = 1: ~x union y
// polarity = 3: ~x union ~y

శూన్య Unicode_సమితి::కూడు(స్థిరరాశి UChar32* other, int32_t otherLen, int8_t polarity) {
    ఒకవేళ (isFrozen() || isBogus() || other==శూన్యసూచిక) {
        తిరిగి;
    }
    ఒకవేళ (!ensure_బఫరు_సామర్థ్యం(len + otherLen)) {
        తిరిగి;
    }

    int32_t i = 0, j = 0, k = 0;
    UChar32 a = list[i++];
    UChar32 b = other[j++];
    // change from xor is that we have to check overlapping pairs
    // polarity bit 1 means a is second, bit 2 means b is.
    కొరకు (;;) {
        ఎంపిక (polarity) {
          స్థితి 0: // both first; take lower if unequal
            ఒకవేళ (a < b) { // take a
                // Back up over overlapping ranges in buffer[]
                ఒకవేళ (k > 0 && a <= బఫరు[k-1]) {
                    // Pick latter end value in buffer[] vs. list[]
                    a = max(list[i], బఫరు[--k]);
                } లేకపోతే {
                    // No overlap
                    బఫరు[k++] = a;
                    a = list[i];
                }
                i++; // Common if/else code factored out
                polarity ^= 1;
            } లేకపోతే ఒకవేళ (b < a) { // take b
                ఒకవేళ (k > 0 && b <= బఫరు[k-1]) {
                    b = max(other[j], బఫరు[--k]);
                } లేకపోతే {
                    బఫరు[k++] = b;
                    b = other[j];
                }
                j++;
                polarity ^= 2;
            } లేకపోతే { // a == b, take a, drop b
                ఒకవేళ (a == UNICODESET_ఎగువ) వెళ్ళు loop_చివర;
                // This is symmetrical; it doesn't matter if
                // we backtrack with a or b. - liu
                ఒకవేళ (k > 0 && a <= బఫరు[k-1]) {
                    a = max(list[i], బఫరు[--k]);
                } లేకపోతే {
                    // No overlap
                    బఫరు[k++] = a;
                    a = list[i];
                }
                i++;
                polarity ^= 1;
                b = other[j++];
                polarity ^= 2;
            }
            ఆపు;
          స్థితి 3: // both second; take higher if unequal, and drop other
            ఒకవేళ (b <= a) { // take a
                ఒకవేళ (a == UNICODESET_ఎగువ) వెళ్ళు loop_చివర;
                బఫరు[k++] = a;
            } లేకపోతే { // take b
                ఒకవేళ (b == UNICODESET_ఎగువ) వెళ్ళు loop_చివర;
                బఫరు[k++] = b;
            }
            a = list[i++];
            polarity ^= 1;   // factored common code
            b = other[j++];
            polarity ^= 2;
            ఆపు;
          స్థితి 1: // a second, b first; if b < a, overlap
            ఒకవేళ (a < b) { // no overlap, take a
                బఫరు[k++] = a; a = list[i++]; polarity ^= 1;
            } లేకపోతే ఒకవేళ (b < a) { // OVERLAP, drop b
                b = other[j++];
                polarity ^= 2;
            } లేకపోతే { // a == b, drop both!
                ఒకవేళ (a == UNICODESET_ఎగువ) వెళ్ళు loop_చివర;
                a = list[i++];
                polarity ^= 1;
                b = other[j++];
                polarity ^= 2;
            }
            ఆపు;
          స్థితి 2: // a first, b second; if a < b, overlap
            ఒకవేళ (b < a) { // no overlap, take b
                బఫరు[k++] = b;
                b = other[j++];
                polarity ^= 2;
            } లేకపోతే  ఒకవేళ (a < b) { // OVERLAP, drop a
                a = list[i++];
                polarity ^= 1;
            } లేకపోతే { // a == b, drop both!
                ఒకవేళ (a == UNICODESET_ఎగువ) వెళ్ళు loop_చివర;
                a = list[i++];
                polarity ^= 1;
                b = other[j++];
                polarity ^= 2;
            }
            ఆపు;
        }
    }
 loop_చివర:
    బఫరు[k++] = UNICODESET_ఎగువ;    // terminate
    len = k;
    మార్పిడి_Buffers();
    release_నమూనా();
}

// polarity = 0 is normal: x intersect y
// polarity = 2: x intersect ~y == set-minus
// polarity = 1: ~x intersect y
// polarity = 3: ~x intersect ~y

శూన్య Unicode_సమితి::retain(స్థిరరాశి UChar32* other, int32_t otherLen, int8_t polarity) {
    ఒకవేళ (isFrozen() || isBogus()) {
        తిరిగి;
    }
    ఒకవేళ (!ensure_బఫరు_సామర్థ్యం(len + otherLen)) {
        తిరిగి;
    }

    int32_t i = 0, j = 0, k = 0;
    UChar32 a = list[i++];
    UChar32 b = other[j++];
    // change from xor is that we have to check overlapping pairs
    // polarity bit 1 means a is second, bit 2 means b is.
    కొరకు (;;) {
        ఎంపిక (polarity) {
          స్థితి 0: // both first; drop the smaller
            ఒకవేళ (a < b) { // drop a
                a = list[i++];
                polarity ^= 1;
            } లేకపోతే ఒకవేళ (b < a) { // drop b
                b = other[j++];
                polarity ^= 2;
            } లేకపోతే { // a == b, take one, drop other
                ఒకవేళ (a == UNICODESET_ఎగువ) వెళ్ళు loop_చివర;
                బఫరు[k++] = a;
                a = list[i++];
                polarity ^= 1;
                b = other[j++];
                polarity ^= 2;
            }
            ఆపు;
          స్థితి 3: // both second; take lower if unequal
            ఒకవేళ (a < b) { // take a
                బఫరు[k++] = a;
                a = list[i++];
                polarity ^= 1;
            } లేకపోతే ఒకవేళ (b < a) { // take b
                బఫరు[k++] = b;
                b = other[j++];
                polarity ^= 2;
            } లేకపోతే { // a == b, take one, drop other
                ఒకవేళ (a == UNICODESET_ఎగువ) వెళ్ళు loop_చివర;
                బఫరు[k++] = a;
                a = list[i++];
                polarity ^= 1;
                b = other[j++];
                polarity ^= 2;
            }
            ఆపు;
          స్థితి 1: // a second, b first;
            ఒకవేళ (a < b) { // NO OVERLAP, drop a
                a = list[i++];
                polarity ^= 1;
            } లేకపోతే ఒకవేళ (b < a) { // OVERLAP, take b
                బఫరు[k++] = b;
                b = other[j++];
                polarity ^= 2;
            } లేకపోతే { // a == b, drop both!
                ఒకవేళ (a == UNICODESET_ఎగువ) వెళ్ళు loop_చివర;
                a = list[i++];
                polarity ^= 1;
                b = other[j++];
                polarity ^= 2;
            }
            ఆపు;
          స్థితి 2: // a first, b second; if a < b, overlap
            ఒకవేళ (b < a) { // no overlap, drop b
                b = other[j++];
                polarity ^= 2;
            } లేకపోతే  ఒకవేళ (a < b) { // OVERLAP, take a
                బఫరు[k++] = a;
                a = list[i++];
                polarity ^= 1;
            } లేకపోతే { // a == b, drop both!
                ఒకవేళ (a == UNICODESET_ఎగువ) వెళ్ళు loop_చివర;
                a = list[i++];
                polarity ^= 1;
                b = other[j++];
                polarity ^= 2;
            }
            ఆపు;
        }
    }
 loop_చివర:
    బఫరు[k++] = UNICODESET_ఎగువ;    // terminate
    len = k;
    మార్పిడి_Buffers();
    release_నమూనా();
}

/**
 * Append the <code>toPattern()</code> representation of a
 * string to the given <code>StringBuffer</code>.
 */
శూన్య Unicode_సమితి::append_గమ్యం_Pat(UnicodeString& buf, స్థిరరాశి UnicodeString& s, UBool escapeUnprintable) {
    UChar32 cp;
    కొరకు (int32_t i = 0; i < s.length(); i += U16_నిడివి(cp)) {
        append_గమ్యం_Pat(buf, cp = s.char32At(i), escapeUnprintable);
    }
}

/**
 * Append the <code>toPattern()</code> representation of a
 * character to the given <code>StringBuffer</code>.
 */
శూన్య Unicode_సమితి::append_గమ్యం_Pat(UnicodeString& buf, UChar32 c, UBool escapeUnprintable) {
    ఒకవేళ (escapeUnprintable ? ICU_Utility::isUnprintable(c) : ICU_Utility::shouldAlwaysBeEscaped(c)) {
        // Use hex escape notation (\uxxxx or \Uxxxxxxxx) for anything
        // unprintable
        ICU_Utility::escape(buf, c);
        తిరిగి;
    }
    // Okay to let ':' pass through
    ఎంపిక (c) {
    స్థితి u'[':
    స్థితి u']':
    స్థితి u'-':
    స్థితి u'^':
    స్థితి u'&':
    స్థితి u'\\':
    స్థితి u'{':
    స్థితి u'}':
    స్థితి u':':
    స్థితి చిహ్నం_పట్టిక::చిహ్నం_REF:
        buf.append(u'\\');
        ఆపు;
    అప్రమేయ:
        // Escape whitespace
        ఒకవేళ (నమూనా_Props::isWhiteSpace(c)) {
            buf.append(u'\\');
        }
        ఆపు;
    }
    buf.append(c);
}

శూన్య Unicode_సమితి::append_గమ్యం_Pat(UnicodeString &ఫలితం, UChar32 ప్రారంభం, UChar32 చివర,
                              UBool escapeUnprintable) {
    append_గమ్యం_Pat(ఫలితం, ప్రారంభం, escapeUnprintable);
    ఒకవేళ (ప్రారంభం != చివర) {
        ఒకవేళ ((ప్రారంభం+1) != చివర ||
                // Avoid writing what looks like a lead+trail surrogate pair.
                ప్రారంభం == 0xdbff) {
            ఫలితం.append(u'-');
        }
        append_గమ్యం_Pat(ఫలితం, చివర, escapeUnprintable);
    }
}

/**
 * Append a string representation of this set to result.  This will be
 * a cleaned version of the string passed to applyPattern(), if there
 * is one.  Otherwise it will be generated.
 */
UnicodeString& Unicode_సమితి::గమ్యం_నమూనా_2(UnicodeString& ఫలితం U_LIFETIME_BOUND,
                                      UBool escapeUnprintable) స్థిరరాశి
{
    ఒకవేళ (pat != శూన్యసూచిక) {
        int32_t i;
        int32_t backslash_లెక్క = 0;
        కొరకు (i=0; i<patLen; ) {
            UChar32 c;
            U16_తదుపరి(pat, i, patLen, c);
            ఒకవేళ (escapeUnprintable ?
                    ICU_Utility::isUnprintable(c) : ICU_Utility::shouldAlwaysBeEscaped(c)) {
                // If the unprintable character is preceded by an odd
                // number of backslashes, then it has been escaped.
                // Before unescaping it, we delete the final
                // backslash.
                ఒకవేళ ((backslash_లెక్క % 2) == 1) {
                    ఫలితం.truncate(ఫలితం.length() - 1);
                }
                ICU_Utility::escape(ఫలితం, c);
                backslash_లెక్క = 0;
            } లేకపోతే {
                ఫలితం.append(c);
                ఒకవేళ (c == u'\\') {
                    ++backslash_లెక్క;
                } లేకపోతే {
                    backslash_లెక్క = 0;
                }
            }
        }
        తిరిగి ఫలితం;
    }

    తిరిగి generate_నమూనా(ఫలితం, escapeUnprintable);
}

/**
 * Returns a string representation of this set.  If the result of
 * calling this function is passed to a UnicodeSet constructor, it
 * will produce another set that is equal to this one.
 */
UnicodeString& Unicode_సమితి::గమ్యం_నమూనా(UnicodeString& ఫలితం U_LIFETIME_BOUND,
                                     UBool escapeUnprintable) స్థిరరాశి
{
    ఫలితం.truncate(0);
    తిరిగి గమ్యం_నమూనా_2(ఫలితం, escapeUnprintable);
}

/**
 * Generate and append a string representation of this set to result.
 * This does not use this.pat, the cleaned up copy of the string
 * passed to applyPattern().
 */
UnicodeString& Unicode_సమితి::generate_నమూనా(UnicodeString& ఫలితం U_LIFETIME_BOUND,
                                            UBool escapeUnprintable) స్థిరరాశి
{
    ఫలితం.append(u'[');

    int32_t i = 0;
    int32_t హద్దు = len & ~1;  // = 2 * getRangeCount()

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
    ఒకవేళ (len >= 4 && list[0] == 0 && హద్దు == len && !has_అక్షరశ్రేణులు()) {
        // Emit the inverse
        ఫలితం.append(u'^');
        // Offsetting the inversion list index by one lets us
        // iterate over the ranges of the set complement.
        i = 1;
        --హద్దు;
    }

    // Emit the ranges as pairs.
    అయితే (i < హద్దు) {
        UChar32 ప్రారంభం = list[i];  // getRangeStart()
        UChar32 చివర = list[i + 1] - 1;  // getRangeEnd() = range limit minus one
        ఒకవేళ (!(0xd800 <= చివర && చివర <= 0xdbff)) {
            append_గమ్యం_Pat(ఫలితం, ప్రారంభం, చివర, escapeUnprintable);
            i += 2;
        } లేకపోతే {
            // The range ends with a lead surrogate.
            // Avoid writing what looks like a lead+trail surrogate pair.
            // 1. Postpone ranges that start with a lead surrogate code point.
            int32_t మొదటి_Lead = i;
            అయితే ((i += 2) < హద్దు && list[i] <= 0xdbff) {}
            int32_t మొదటి_After_Lead = i;
            // 2. Write following ranges that start with a trail surrogate code point.
            అయితే (i < హద్దు && (ప్రారంభం = list[i]) <= 0xdfff) {
                append_గమ్యం_Pat(ఫలితం, ప్రారంభం, list[i + 1] - 1, escapeUnprintable);
                i += 2;
            }
            // 3. Now write the postponed ranges.
            కొరకు (పూర్ణాంకం j = మొదటి_Lead; j < మొదటి_After_Lead; j += 2) {
                append_గమ్యం_Pat(ఫలితం, list[j], list[j + 1] - 1, escapeUnprintable);
            }
        }
    }

    ఒకవేళ (అక్షరశ్రేణులు_2 != శూన్యసూచిక) {
        కొరకు (int32_t i = 0; i<అక్షరశ్రేణులు_2->size(); ++i) {
            ఫలితం.append(u'{');
            append_గమ్యం_Pat(ఫలితం,
                         *static_cast<స్థిరరాశి UnicodeString*>(అక్షరశ్రేణులు_2->elementAt(i)),
                         escapeUnprintable);
            ఫలితం.append(u'}');
        }
    }
    తిరిగి ఫలితం.append(u']');
}

/**
* Release existing cached pattern
*/
శూన్య Unicode_సమితి::release_నమూనా() {
    ఒకవేళ (pat) {
        uprv_free(pat);
        pat = శూన్యసూచిక;
        patLen = 0;
    }
}

/**
* Set the new pattern to cache.
*/
శూన్య Unicode_సమితి::సమితి_నమూనా(స్థిరరాశి char16_t *newPat, int32_t newPatLen) {
    release_నమూనా();
    pat = static_cast<char16_t*>(uprv_malloc((newPatLen + 1) * పరిమాణం(char16_t)));
    ఒకవేళ (pat) {
        patLen = newPatLen;
        u_memcpy(pat, newPat, patLen);
        pat[patLen] = 0;
    }
    // else we don't care if malloc failed. This was just a nice cache.
    // We can regenerate an equivalent pattern later when requested.
}

Unicode_సమితి* Unicode_సమితి::freeze() U_LIFETIME_BOUND {
    ఒకవేళ(!isFrozen() && !isBogus()) {
        compact();

        // Optimize contains() and span() and similar functions.
        ఒకవేళ (has_అక్షరశ్రేణులు()) {
            string_పరిధి = కొత్త Unicode_సమితి_String_పరిధి(*ఇది, *అక్షరశ్రేణులు_2, Unicode_సమితి_String_పరిధి::ALL);
            ఒకవేళ (string_పరిధి == శూన్యసూచిక) {
                సమితి_గమ్యం_Bogus();
                తిరిగి ఇది;
            } లేకపోతే ఒకవేళ (!string_పరిధి->needs_String_పరిధి_UTF16()) {
                // All strings are irrelevant for span() etc. because
                // all of each string's code points are contained in this set.
                // Do not check needsStringSpanUTF8() because UTF-8 has at most as
                // many relevant strings as UTF-16.
                // (Thus needsStringSpanUTF8() implies needsStringSpanUTF16().)
                తొలగించు string_పరిధి;
                string_పరిధి = శూన్యసూచిక;
            }
        }
        ఒకవేళ (string_పరిధి == శూన్యసూచిక) {
            // No span-relevant strings: Optimize for code point spans.
            bmp_సమితి=కొత్త BMPSet(list, len);
            ఒకవేళ (bmp_సమితి == శూన్యసూచిక) { // Check for memory allocation error.
                సమితి_గమ్యం_Bogus();
            }
        }
    }
    తిరిగి ఇది;
}

int32_t Unicode_సమితి::పరిధి(స్థిరరాశి char16_t *s, int32_t length, USet_పరిధి_Condition పరిధి_Condition) స్థిరరాశి {
    ఒకవేళ(length>0 && bmp_సమితి!=శూన్యసూచిక) {
        తిరిగి static_cast<int32_t>(bmp_సమితి->పరిధి(s, s + length, పరిధి_Condition) - s);
    }
    ఒకవేళ(length<0) {
        length=u_strlen(s);
    }
    ఒకవేళ(length==0) {
        తిరిగి 0;
    }
    ఒకవేళ(string_పరిధి!=శూన్యసూచిక) {
        తిరిగి string_పరిధి->పరిధి(s, length, పరిధి_Condition);
    } లేకపోతే ఒకవేళ(has_అక్షరశ్రేణులు()) {
        uint32_t which= పరిధి_Condition==USET_పరిధి_NOT_CONTAINED ?
                            Unicode_సమితి_String_పరిధి::FWD_UTF16_NOT_CONTAINED :
                            Unicode_సమితి_String_పరిధి::FWD_UTF16_CONTAINED;
        Unicode_సమితి_String_పరిధి str_పరిధి(*ఇది, *అక్షరశ్రేణులు_2, which);
        ఒకవేళ(str_పరిధి.needs_String_పరిధి_UTF16()) {
            తిరిగి str_పరిధి.పరిధి(s, length, పరిధి_Condition);
        }
    }

    ఒకవేళ(పరిధి_Condition!=USET_పరిధి_NOT_CONTAINED) {
        పరిధి_Condition=USET_పరిధి_CONTAINED;  // Pin to 0/1 values.
    }

    UChar32 c;
    int32_t ప్రారంభం=0, prev=0;
    చేయి {
        U16_తదుపరి(s, ప్రారంభం, length, c);
        ఒకవేళ(పరిధి_Condition!=కలిగియుంది(c)) {
            ఆపు;
        }
    } అయితే((prev=ప్రారంభం)<length);
    తిరిగి prev;
}

int32_t Unicode_సమితి::పరిధి_వెనుక(స్థిరరాశి char16_t *s, int32_t length, USet_పరిధి_Condition పరిధి_Condition) స్థిరరాశి {
    ఒకవేళ(length>0 && bmp_సమితి!=శూన్యసూచిక) {
        తిరిగి static_cast<int32_t>(bmp_సమితి->పరిధి_వెనుక(s, s + length, పరిధి_Condition) - s);
    }
    ఒకవేళ(length<0) {
        length=u_strlen(s);
    }
    ఒకవేళ(length==0) {
        తిరిగి 0;
    }
    ఒకవేళ(string_పరిధి!=శూన్యసూచిక) {
        తిరిగి string_పరిధి->పరిధి_వెనుక(s, length, పరిధి_Condition);
    } లేకపోతే ఒకవేళ(has_అక్షరశ్రేణులు()) {
        uint32_t which= పరిధి_Condition==USET_పరిధి_NOT_CONTAINED ?
                            Unicode_సమితి_String_పరిధి::వెనుక_UTF16_NOT_CONTAINED :
                            Unicode_సమితి_String_పరిధి::వెనుక_UTF16_CONTAINED;
        Unicode_సమితి_String_పరిధి str_పరిధి(*ఇది, *అక్షరశ్రేణులు_2, which);
        ఒకవేళ(str_పరిధి.needs_String_పరిధి_UTF16()) {
            తిరిగి str_పరిధి.పరిధి_వెనుక(s, length, పరిధి_Condition);
        }
    }

    ఒకవేళ(పరిధి_Condition!=USET_పరిధి_NOT_CONTAINED) {
        పరిధి_Condition=USET_పరిధి_CONTAINED;  // Pin to 0/1 values.
    }

    UChar32 c;
    int32_t prev=length;
    చేయి {
        U16_PREV(s, 0, length, c);
        ఒకవేళ(పరిధి_Condition!=కలిగియుంది(c)) {
            ఆపు;
        }
    } అయితే((prev=length)>0);
    తిరిగి prev;
}

int32_t Unicode_సమితి::పరిధి_UTF8(స్థిరరాశి అక్షరం *s, int32_t length, USet_పరిధి_Condition పరిధి_Condition) స్థిరరాశి {
    ఒకవేళ(length>0 && bmp_సమితి!=శూన్యసూచిక) {
        స్థిరరాశి uint8_t* s0 = reinterpret_cast<స్థిరరాశి uint8_t*>(s);
        తిరిగి static_cast<int32_t>(bmp_సమితి->పరిధి_UTF8(s0, length, పరిధి_Condition) - s0);
    }
    ఒకవేళ(length<0) {
        length = static_cast<int32_t>(uprv_strlen(s));
    }
    ఒకవేళ(length==0) {
        తిరిగి 0;
    }
    ఒకవేళ(string_పరిధి!=శూన్యసూచిక) {
        తిరిగి string_పరిధి->పరిధి_UTF8(reinterpret_cast<స్థిరరాశి uint8_t*>(s), length, పరిధి_Condition);
    } లేకపోతే ఒకవేళ(has_అక్షరశ్రేణులు()) {
        uint32_t which= పరిధి_Condition==USET_పరిధి_NOT_CONTAINED ?
                            Unicode_సమితి_String_పరిధి::FWD_UTF8_NOT_CONTAINED :
                            Unicode_సమితి_String_పరిధి::FWD_UTF8_CONTAINED;
        Unicode_సమితి_String_పరిధి str_పరిధి(*ఇది, *అక్షరశ్రేణులు_2, which);
        ఒకవేళ(str_పరిధి.needs_String_పరిధి_UTF8()) {
            తిరిగి str_పరిధి.పరిధి_UTF8(reinterpret_cast<స్థిరరాశి uint8_t*>(s), length, పరిధి_Condition);
        }
    }

    ఒకవేళ(పరిధి_Condition!=USET_పరిధి_NOT_CONTAINED) {
        పరిధి_Condition=USET_పరిధి_CONTAINED;  // Pin to 0/1 values.
    }

    UChar32 c;
    int32_t ప్రారంభం=0, prev=0;
    చేయి {
        U8_తదుపరి_OR_FFFD(s, ప్రారంభం, length, c);
        ఒకవేళ(పరిధి_Condition!=కలిగియుంది(c)) {
            ఆపు;
        }
    } అయితే((prev=ప్రారంభం)<length);
    తిరిగి prev;
}

int32_t Unicode_సమితి::పరిధి_వెనుక_UTF8(స్థిరరాశి అక్షరం *s, int32_t length, USet_పరిధి_Condition పరిధి_Condition) స్థిరరాశి {
    ఒకవేళ(length>0 && bmp_సమితి!=శూన్యసూచిక) {
        స్థిరరాశి uint8_t* s0 = reinterpret_cast<స్థిరరాశి uint8_t*>(s);
        తిరిగి bmp_సమితి->పరిధి_వెనుక_UTF8(s0, length, పరిధి_Condition);
    }
    ఒకవేళ(length<0) {
        length = static_cast<int32_t>(uprv_strlen(s));
    }
    ఒకవేళ(length==0) {
        తిరిగి 0;
    }
    ఒకవేళ(string_పరిధి!=శూన్యసూచిక) {
        తిరిగి string_పరిధి->పరిధి_వెనుక_UTF8(reinterpret_cast<స్థిరరాశి uint8_t*>(s), length, పరిధి_Condition);
    } లేకపోతే ఒకవేళ(has_అక్షరశ్రేణులు()) {
        uint32_t which= పరిధి_Condition==USET_పరిధి_NOT_CONTAINED ?
                            Unicode_సమితి_String_పరిధి::వెనుక_UTF8_NOT_CONTAINED :
                            Unicode_సమితి_String_పరిధి::వెనుక_UTF8_CONTAINED;
        Unicode_సమితి_String_పరిధి str_పరిధి(*ఇది, *అక్షరశ్రేణులు_2, which);
        ఒకవేళ(str_పరిధి.needs_String_పరిధి_UTF8()) {
            తిరిగి str_పరిధి.పరిధి_వెనుక_UTF8(reinterpret_cast<స్థిరరాశి uint8_t*>(s), length, పరిధి_Condition);
        }
    }

    ఒకవేళ(పరిధి_Condition!=USET_పరిధి_NOT_CONTAINED) {
        పరిధి_Condition=USET_పరిధి_CONTAINED;  // Pin to 0/1 values.
    }

    UChar32 c;
    int32_t prev=length;
    చేయి {
        U8_PREV_OR_FFFD(s, 0, length, c);
        ఒకవేళ(పరిధి_Condition!=కలిగియుంది(c)) {
            ఆపు;
        }
    } అయితే((prev=length)>0);
    తిరిగి prev;
}

U_NAMESPACE_చివర
