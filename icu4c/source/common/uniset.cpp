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
#परिभाषय UNICODESET_उच्चः 0x0110000

// LOW <= all valid values. ZERO for codepoints
#परिभाषय UNICODESET_नीचः 0x000000

/** Max list [0, 1, 2, ..., max code point, HIGH] */
constexpr int32_t MAX_दीर्घता = UNICODESET_उच्चः + 1;

U_NAMESPACE_BEGIN

चिह्नम्_सारणी::~चिह्नम्_सारणी() {}

UOBJECT_DEFINE_RTTI_IMPLEMENTATION(Unicode_समुच्चयः)

/**
 * Modify the given UChar32 variable so that it is in range, by
 * pinning values < UNICODESET_LOW to UNICODESET_LOW, and
 * pinning values > UNICODESET_HIGH-1 to UNICODESET_HIGH-1.
 * It modifies its argument in-place and also returns it.
 */
स्थिर अन्तःस्थ UChar32 pin_संहिता_Point(UChar32& c) {
    यदि (c < UNICODESET_नीचः) {
        c = UNICODESET_नीचः;
    } अन्यथा यदि (c > (UNICODESET_उच्चः-1)) {
        c = (UNICODESET_उच्चः-1);
    }
    प्रत्यागम c;
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

#यदि_परिभाषितम् DEBUG_MEM
#अन्तर्भाव <stdio.h>
स्थिर int32_t dbg_गणनम् = 0;

स्थिर अन्तःस्थ शून्य _dbgct(Unicode_समुच्चयः* समुच्चयः) {
    UnicodeString str;
    समुच्चयः->गन्तव्यम्_आदर्शः(str, सत्य);
    वर्ण buf[40];
    str.extract(0, 39, buf, "");
    प्रारूप_लिख("DEBUG UnicodeSet: ct 0x%08X; %d %s\n", समुच्चयः, ++dbg_गणनम्, buf);
}

स्थिर अन्तःस्थ शून्य _dbgdt(Unicode_समुच्चयः* समुच्चयः) {
    UnicodeString str;
    समुच्चयः->गन्तव्यम्_आदर्शः(str, सत्य);
    वर्ण buf[40];
    str.extract(0, 39, buf, "");
    प्रारूप_लिख("DEBUG UnicodeSet: dt 0x%08X; %d %s\n", समुच्चयः, --dbg_गणनम्, buf);
}

#अन्यथा

#परिभाषय _dbgct(समुच्चयः)
#परिभाषय _dbgdt(समुच्चयः)

#यदि_अन्तः

//----------------------------------------------------------------
// UnicodeString in UVector support
//----------------------------------------------------------------

स्थिर शून्य U_CALLCONV cloneUnicodeString(UElement *dst, UElement *src) {
    dst->pointer = नूतन UnicodeString(*static_cast<UnicodeString*>(src->pointer));
}

स्थिर int32_t U_CALLCONV compareUnicodeString(UElement t1, UElement t2) {
    ध्रुव UnicodeString& a = *static_cast<ध्रुव UnicodeString*>(t1.pointer);
    ध्रुव UnicodeString& b = *static_cast<ध्रुव UnicodeString*>(t2.pointer);
    प्रत्यागम a.compare(b);
}

UBool Unicode_समुच्चयः::has_वाक्यानि() ध्रुव {
    प्रत्यागम वाक्यानि_2 != शून्यसूचकः && !वाक्यानि_2->is_रिक्तानि();
}

int32_t Unicode_समुच्चयः::वाक्यानि_आकार() ध्रुव {
    प्रत्यागम वाक्यानि_2 == शून्यसूचकः ? 0 : वाक्यानि_2->size();
}

UBool Unicode_समुच्चयः::वाक्यानि_अन्तर्भवति(ध्रुव UnicodeString &s) ध्रुव {
    प्रत्यागम वाक्यानि_2 != शून्यसूचकः && वाक्यानि_2->अन्तर्भवति((शून्य*) &s);
}

//----------------------------------------------------------------
// Constructors &c
//----------------------------------------------------------------

/**
 * Constructs an empty set.
 */
Unicode_समुच्चयः::Unicode_समुच्चयः() {
    list[0] = UNICODESET_उच्चः;
    _dbgct(एतत्);
}

/**
 * Constructs a set containing the given range. If <code>end >
 * start</code> then an empty set is created.
 *
 * @param start first character, inclusive, of range
 * @param end last character, inclusive, of range
 */
Unicode_समुच्चयः::Unicode_समुच्चयः(UChar32 start, UChar32 अन्तः) {
    list[0] = UNICODESET_उच्चः;
    सङ्कलय(start, अन्तः);
    _dbgct(एतत्);
}

/**
 * Constructs a set that is identical to the given UnicodeSet.
 */
Unicode_समुच्चयः::Unicode_समुच्चयः(ध्रुव Unicode_समुच्चयः& o) : UnicodeFilter(o) {
    *एतत् = o;
    _dbgct(एतत्);
}

// Copy-construct as thawed.
Unicode_समुच्चयः::Unicode_समुच्चयः(ध्रुव Unicode_समुच्चयः& o, UBool /* asThawed */) : UnicodeFilter(o) {
    यदि (ensure_क्षमता(o.len)) {
        // *this = o except for bmpSet and stringSpan
        len = o.len;
        uprv_memcpy(list, o.list, (size_t)len*परिमाण(UChar32));
        यदि (o.has_वाक्यानि()) {
            UError_संहिता स्थितिः = U_ZERO_ERROR;
            यदि (!allocate_वाक्यानि(स्थितिः) ||
                    (वाक्यानि_2->assign(*o.वाक्यानि_2, cloneUnicodeString, स्थितिः), U_FAILURE(स्थितिः))) {
                समुच्चयः_गन्तव्यम्_Bogus();
                प्रत्यागम;
            }
        }
        यदि (o.pat) {
            समुच्चयः_आदर्शः(o.pat, o.patLen);
        }
        _dbgct(एतत्);
    }
}

/**
 * Destructs the set.
 */
Unicode_समुच्चयः::~Unicode_समुच्चयः() {
    _dbgdt(एतत्); // first!
    यदि (list != राशिः_List) {
        uprv_free(list);
    }
    विलोपय bmp_समुच्चयः;
    यदि (बफर != राशिः_List) {
        uprv_free(बफर);
    }
    विलोपय वाक्यानि_2;
    विलोपय string_विस्तृतिः;
    release_आदर्शः();
}

/**
 * Assigns this object to be a copy of another.
 */
Unicode_समुच्चयः& Unicode_समुच्चयः::संक्रियकः=(ध्रुव Unicode_समुच्चयः& o) U_LIFETIME_BOUND {
    प्रत्यागम copyFrom(o, असत्य);
}

Unicode_समुच्चयः& Unicode_समुच्चयः::copyFrom(ध्रुव Unicode_समुच्चयः& o, UBool asThawed) U_LIFETIME_BOUND {
    यदि (एतत् == &o) {
        प्रत्यागम *एतत्;
    }
    यदि (isFrozen()) {
        प्रत्यागम *एतत्;
    }
    यदि (o.isBogus()) {
        समुच्चयः_गन्तव्यम्_Bogus();
        प्रत्यागम *एतत्;
    }
    यदि (!ensure_क्षमता(o.len)) {
        // ensureCapacity will mark the UnicodeSet as Bogus if OOM failure happens.
        प्रत्यागम *एतत्;
    }
    len = o.len;
    uprv_memcpy(list, o.list, (size_t)len*परिमाण(UChar32));
    यदि (o.bmp_समुच्चयः != शून्यसूचकः && !asThawed) {
        bmp_समुच्चयः = नूतन BMPSet(*o.bmp_समुच्चयः, list, len);
        यदि (bmp_समुच्चयः == शून्यसूचकः) { // Check for memory allocation error.
            समुच्चयः_गन्तव्यम्_Bogus();
            प्रत्यागम *एतत्;
        }
    }
    यदि (o.has_वाक्यानि()) {
        UError_संहिता स्थितिः = U_ZERO_ERROR;
        यदि ((वाक्यानि_2 == शून्यसूचकः && !allocate_वाक्यानि(स्थितिः)) ||
                (वाक्यानि_2->assign(*o.वाक्यानि_2, cloneUnicodeString, स्थितिः), U_FAILURE(स्थितिः))) {
            समुच्चयः_गन्तव्यम्_Bogus();
            प्रत्यागम *एतत्;
        }
    } अन्यथा यदि (has_वाक्यानि()) {
        वाक्यानि_2->removeAllElements();
    }
    यदि (o.string_विस्तृतिः != शून्यसूचकः && वाक्यानि_2 != शून्यसूचकः && !asThawed) {
        string_विस्तृतिः = नूतन Unicode_समुच्चयः_String_विस्तृतिः(*o.string_विस्तृतिः, *वाक्यानि_2);
        यदि (string_विस्तृतिः == शून्यसूचकः) { // Check for memory allocation error.
            समुच्चयः_गन्तव्यम्_Bogus();
            प्रत्यागम *एतत्;
        }
    }
    release_आदर्शः();
    यदि (o.pat) {
        समुच्चयः_आदर्शः(o.pat, o.patLen);
    }
    प्रत्यागम *एतत्;
}

/**
 * Returns a copy of this object.  All UnicodeMatcher objects have
 * to support cloning in order to allow classes using
 * UnicodeMatchers, such as Transliterator, to implement cloning.
 */
Unicode_समुच्चयः* Unicode_समुच्चयः::clone() ध्रुव {
    प्रत्यागम नूतन Unicode_समुच्चयः(*एतत्);
}

Unicode_समुच्चयः *Unicode_समुच्चयः::cloneAsThawed() ध्रुव {
    प्रत्यागम नूतन Unicode_समुच्चयः(*एतत्, सत्य);
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
तार्किक Unicode_समुच्चयः::संक्रियकः==(ध्रुव Unicode_समुच्चयः& o) ध्रुव {
    यदि (len != o.len) प्रत्यागम असत्य;
    प्रति (int32_t i = 0; i < len; ++i) {
        यदि (list[i] != o.list[i]) प्रत्यागम असत्य;
    }
    यदि (has_वाक्यानि() != o.has_वाक्यानि()) { प्रत्यागम असत्य; }
    यदि (has_वाक्यानि() && *वाक्यानि_2 != *o.वाक्यानि_2) प्रत्यागम असत्य;
    प्रत्यागम सत्य;
}

/**
 * Returns the hash code value for this set.
 *
 * @return the hash code value for this set.
 * @see Object#hashCode()
 */
int32_t Unicode_समुच्चयः::hash_संहिता() ध्रुव {
    uint32_t परिणाम = static_cast<uint32_t>(len);
    प्रति (int32_t i = 0; i < len; ++i) {
        परिणाम *= 1000003u;
        परिणाम += list[i];
    }
    प्रत्यागम static_cast<int32_t>(परिणाम);
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
int32_t Unicode_समुच्चयः::size() ध्रुव {
    int32_t n = 0;
    int32_t count = get_Range_गणनम्();
    प्रति (int32_t i = 0; i < count; ++i) {
        n += get_Range_अन्तः(i) - getRangeStart(i) + 1;
    }
    प्रत्यागम n + वाक्यानि_आकार();
}

/**
 * Returns <tt>true</tt> if this set contains no elements.
 *
 * @return <tt>true</tt> if this set contains no elements.
 */
UBool Unicode_समुच्चयः::is_रिक्तानि() ध्रुव {
    प्रत्यागम len == 1 && !has_वाक्यानि();
}

/**
 * Returns true if this set contains the given character.
 * @param c character to be checked for containment
 * @return true if the test condition is met
 */
UBool Unicode_समुच्चयः::अन्तर्भवति(UChar32 c) ध्रुव {
    // Set i to the index of the start item greater than ch
    // We know we will terminate without length test!
    // LATER: for large sets, add binary search
    //int32_t i = -1;
    //for (;;) {
    //    if (c < list[++i]) break;
    //}
    यदि (bmp_समुच्चयः != शून्यसूचकः) {
        प्रत्यागम bmp_समुच्चयः->अन्तर्भवति(c);
    }
    यदि (string_विस्तृतिः != शून्यसूचकः) {
        प्रत्यागम string_विस्तृतिः->अन्तर्भवति(c);
    }
    यदि (c >= UNICODESET_उच्चः) { // Don't need to check LOW bound
        प्रत्यागम असत्य;
    }
    int32_t i = find_संहिता_Point(c);
    प्रत्यागम i & 1; // return true if odd
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
int32_t Unicode_समुच्चयः::find_संहिता_Point(UChar32 c) ध्रुव {
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
        प्रत्यागम 0;
    // High runner test.  c is often after the last range, so an
    // initial check for this condition pays off.
    int32_t lo = 0;
    int32_t hi = len - 1;
    यदि (lo >= hi || c >= list[hi-1])
        प्रत्यागम hi;
    // invariant: c >= list[lo]
    // invariant: c < list[hi]
    प्रति (;;) {
        int32_t i = (lo + hi) >> 1;
        यदि (i == lo) {
            भङ्ग; // Found!
        } अन्यथा यदि (c < list[i]) {
            hi = i;
        } अन्यथा {
            lo = i;
        }
    }
    प्रत्यागम hi;
}

/**
 * Returns true if this set contains every character
 * of the given range.
 * @param start first character, inclusive, of the range
 * @param end last character, inclusive, of the range
 * @return true if the test condition is met
 */
UBool Unicode_समुच्चयः::अन्तर्भवति(UChar32 start, UChar32 अन्तः) ध्रुव {
    //int32_t i = -1;
    //for (;;) {
    //    if (start < list[++i]) break;
    //}
    int32_t i = find_संहिता_Point(start);
    प्रत्यागम ((i & 1) != 0 && अन्तः < list[i]);
}

/**
 * Returns <tt>true</tt> if this set contains the given
 * multicharacter string.
 * @param s string to be checked for containment
 * @return <tt>true</tt> if this set contains the specified string
 */
UBool Unicode_समुच्चयः::अन्तर्भवति(ध्रुव UnicodeString& s) ध्रुव {
    int32_t cp = getSingleCP(s);
    यदि (cp < 0) {
        प्रत्यागम वाक्यानि_अन्तर्भवति(s);
    } अन्यथा {
        प्रत्यागम अन्तर्भवति(static_cast<UChar32>(cp));
    }
}

/**
 * Returns true if this set contains all the characters and strings
 * of the given set.
 * @param c set to be checked for containment
 * @return true if the test condition is met
 */
UBool Unicode_समुच्चयः::अन्तर्भवति_All(ध्रुव Unicode_समुच्चयः& c) ध्रुव {
    // The specified set is a subset if all of its pairs are contained in
    // this set.  It's possible to code this more efficiently in terms of
    // direct manipulation of the inversion lists if the need arises.
    int32_t n = c.get_Range_गणनम्();
    प्रति (पूर्णाङ्क i=0; i<n; ++i) {
        यदि (!अन्तर्भवति(c.getRangeStart(i), c.get_Range_अन्तः(i))) {
            प्रत्यागम असत्य;
        }
    }
    प्रत्यागम !c.has_वाक्यानि() || (वाक्यानि_2 != शून्यसूचकः && वाक्यानि_2->अन्तर्भवति_All(*c.वाक्यानि_2));
}

/**
 * Returns true if this set contains all the characters
 * of the given string.
 * @param s string containing characters to be checked for containment
 * @return true if the test condition is met
 */
UBool Unicode_समुच्चयः::अन्तर्भवति_All(ध्रुव UnicodeString& s) ध्रुव {
    प्रत्यागम विस्तृतिः(s.get_बफर(), s.length(), USET_विस्तृतिः_CONTAINED) == s.length();
}

/**
 * Returns true if this set contains none of the characters
 * of the given range.
 * @param start first character, inclusive, of the range
 * @param end last character, inclusive, of the range
 * @return true if the test condition is met
 */
UBool Unicode_समुच्चयः::अन्तर्भवति_None(UChar32 start, UChar32 अन्तः) ध्रुव {
    //int32_t i = -1;
    //for (;;) {
    //    if (start < list[++i]) break;
    //}
    int32_t i = find_संहिता_Point(start);
    प्रत्यागम ((i & 1) == 0 && अन्तः < list[i]);
}

/**
 * Returns true if this set contains none of the characters and strings
 * of the given set.
 * @param c set to be checked for containment
 * @return true if the test condition is met
 */
UBool Unicode_समुच्चयः::अन्तर्भवति_None(ध्रुव Unicode_समुच्चयः& c) ध्रुव {
    // The specified set is a subset if all of its pairs are contained in
    // this set.  It's possible to code this more efficiently in terms of
    // direct manipulation of the inversion lists if the need arises.
    int32_t n = c.get_Range_गणनम्();
    प्रति (int32_t i=0; i<n; ++i) {
        यदि (!अन्तर्भवति_None(c.getRangeStart(i), c.get_Range_अन्तः(i))) {
            प्रत्यागम असत्य;
        }
    }
    प्रत्यागम वाक्यानि_2 == शून्यसूचकः || !c.has_वाक्यानि() || वाक्यानि_2->अन्तर्भवति_None(*c.वाक्यानि_2);
}

/**
 * Returns true if this set contains none of the characters
 * of the given string.
 * @param s string containing characters to be checked for containment
 * @return true if the test condition is met
 */
UBool Unicode_समुच्चयः::अन्तर्भवति_None(ध्रुव UnicodeString& s) ध्रुव {
    प्रत्यागम विस्तृतिः(s.get_बफर(), s.length(), USET_विस्तृतिः_NOT_CONTAINED) == s.length();
}

/**
 * Returns <tt>true</tt> if this set contains any character whose low byte
 * is the given value.  This is used by <tt>RuleBasedTransliterator</tt> for
 * indexing.
 */
UBool Unicode_समुच्चयः::matches_सूचकांक_मानम्(uint8_t v) ध्रुव {
    /* The index value v, in the range [0,255], is contained in this set if
     * it is contained in any pair of this set.  Pairs either have the high
     * bytes equal, or unequal.  If the high bytes are equal, then we have
     * aaxx..aayy, where aa is the high byte.  Then v is contained if xx <=
     * v <= yy.  If the high bytes are unequal we have aaxx..bbyy, bb>aa.
     * Then v is contained if xx <= v || v <= yy.  (This is identical to the
     * time zone month containment logic.)
     */
    int32_t i;
    int32_t range_गणनम्=get_Range_गणनम्();
    प्रति (i=0; i<range_गणनम्; ++i) {
        UChar32 नीचः = getRangeStart(i);
        UChar32 उच्चः = get_Range_अन्तः(i);
        यदि ((नीचः & ~0xFF) == (उच्चः & ~0xFF)) {
            यदि ((नीचः & 0xFF) <= v && v <= (उच्चः & 0xFF)) {
                प्रत्यागम सत्य;
            }
        } अन्यथा यदि ((नीचः & 0xFF) <= v || v <= (उच्चः & 0xFF)) {
            प्रत्यागम सत्य;
        }
    }
    यदि (has_वाक्यानि()) {
        प्रति (i=0; i<वाक्यानि_2->size(); ++i) {
            ध्रुव UnicodeString& s = *static_cast<ध्रुव UnicodeString*>(वाक्यानि_2->elementAt(i));
            यदि (s.is_रिक्तानि()) {
                अनुवर्त;  // skip the empty string
            }
            UChar32 c = s.char32At(0);
            यदि ((c & 0xFF) == v) {
                प्रत्यागम सत्य;
            }
        }
    }
    प्रत्यागम असत्य;
}

/**
 * Implementation of UnicodeMatcher::matches().  Always matches the
 * longest possible multichar string.
 */
UMatchDegree Unicode_समुच्चयः::matches(ध्रुव Replaceable& text,
                                 int32_t& विस्थापनम्,
                                 int32_t सीमा,
                                 UBool incremental) {
    यदि (विस्थापनम् == सीमा) {
        यदि (अन्तर्भवति(U_ETHER)) {
            प्रत्यागम incremental ? U_PARTIAL_मेलनम् : U_मेलनम्;
        } अन्यथा {
            प्रत्यागम U_MISMATCH;
        }
    } अन्यथा {
        यदि (has_वाक्यानि()) { // try strings first

            // might separate forward and backward loops later
            // for now they are combined

            // TODO Improve efficiency of this, at least in the forward
            // direction, if not in both.  In the forward direction we
            // can assume the strings are sorted.

            int32_t i;
            UBool forward = विस्थापनम् < सीमा;

            // firstChar is the leftmost char to match in the
            // forward direction or the rightmost char to match in
            // the reverse direction.
            char16_t firstChar = text.charAt(विस्थापनम्);

            // If there are multiple strings that can match we
            // return the longest match.
            int32_t उच्चः_Water_दीर्घता = 0;

            प्रति (i=0; i<वाक्यानि_2->size(); ++i) {
                ध्रुव UnicodeString& trial = *static_cast<ध्रुव UnicodeString*>(वाक्यानि_2->elementAt(i));
                यदि (trial.is_रिक्तानि()) {
                    अनुवर्त;  // skip the empty string
                }

                char16_t c = trial.charAt(forward ? 0 : trial.length() - 1);

                // Strings are sorted, so we can optimize in the
                // forward direction.
                यदि (forward && c > firstChar) भङ्ग;
                यदि (c != firstChar) अनुवर्त;

                int32_t मेलनम्_Len = मेलनम्_Rest(text, विस्थापनम्, सीमा, trial);

                यदि (incremental) {
                    int32_t maxLen = forward ? सीमा-विस्थापनम् : विस्थापनम्-सीमा;
                    यदि (मेलनम्_Len == maxLen) {
                        // We have successfully matched but only up to limit.
                        प्रत्यागम U_PARTIAL_मेलनम्;
                    }
                }

                यदि (मेलनम्_Len == trial.length()) {
                    // We have successfully matched the whole string.
                    यदि (मेलनम्_Len > उच्चः_Water_दीर्घता) {
                        उच्चः_Water_दीर्घता = मेलनम्_Len;
                    }
                    // In the forward direction we know strings
                    // are sorted so we can bail early.
                    यदि (forward && मेलनम्_Len < उच्चः_Water_दीर्घता) {
                        भङ्ग;
                    }
                    अनुवर्त;
                }
            }

            // We've checked all strings without a partial match.
            // If we have full matches, return the longest one.
            यदि (उच्चः_Water_दीर्घता != 0) {
                विस्थापनम् += forward ? उच्चः_Water_दीर्घता : -उच्चः_Water_दीर्घता;
                प्रत्यागम U_मेलनम्;
            }
        }
        प्रत्यागम UnicodeFilter::matches(text, विस्थापनम्, सीमा, incremental);
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
int32_t Unicode_समुच्चयः::मेलनम्_Rest(ध्रुव Replaceable& text,
                              int32_t start, int32_t सीमा,
                              ध्रुव UnicodeString& s) {
    int32_t i;
    int32_t maxLen;
    int32_t slen = s.length();
    यदि (start < सीमा) {
        maxLen = सीमा - start;
        यदि (maxLen > slen) maxLen = slen;
        प्रति (i = 1; i < maxLen; ++i) {
            यदि (text.charAt(start + i) != s.charAt(i)) प्रत्यागम 0;
        }
    } अन्यथा {
        maxLen = start - सीमा;
        यदि (maxLen > slen) maxLen = slen;
        --slen; // <=> slen = s.length() - 1;
        प्रति (i = 1; i < maxLen; ++i) {
            यदि (text.charAt(start - i) != s.charAt(slen - i)) प्रत्यागम 0;
        }
    }
    प्रत्यागम maxLen;
}

/**
 * Implement of UnicodeMatcher
 */
शून्य Unicode_समुच्चयः::सङ्कलय_मेलनम्_समुच्चयः_गन्तव्यम्(Unicode_समुच्चयः& गन्तव्यम्_Union_गन्तव्यम्) ध्रुव {
    गन्तव्यम्_Union_गन्तव्यम्.सङ्कलय_All(*एतत्);
}

/**
 * Returns the index of the given character within this set, where
 * the set is ordered by ascending code point.  If the character
 * is not in this set, return -1.  The inverse of this method is
 * <code>charAt()</code>.
 * @return an index from 0..size()-1, or -1
 */
int32_t Unicode_समुच्चयः::सूचकांक_सम्बन्धि(UChar32 c) ध्रुव {
    यदि (c < MIN_मानम् || c > MAX_मानम्) {
        प्रत्यागम -1;
    }
    int32_t i = 0;
    int32_t n = 0;
    प्रति (;;) {
        UChar32 start = list[i++];
        यदि (c < start) {
            प्रत्यागम -1;
        }
        UChar32 सीमा = list[i++];
        यदि (c < सीमा) {
            प्रत्यागम n + c - start;
        }
        n += सीमा - start;
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
UChar32 Unicode_समुच्चयः::charAt(int32_t सूचकांक) ध्रुव {
    यदि (सूचकांक >= 0) {
        // len2 is the largest even integer <= len, that is, it is len
        // for even values and len-1 for odd values.  With odd values
        // the last entry is UNICODESET_HIGH.
        int32_t len2 = len & ~1;
        प्रति (int32_t i=0; i < len2;) {
            UChar32 start = list[i++];
            int32_t count = list[i++] - start;
            यदि (सूचकांक < count) {
                प्रत्यागम static_cast<UChar32>(start + सूचकांक);
            }
            सूचकांक -= count;
        }
    }
    प्रत्यागम static_cast<UChar32>(-1);
}

/**
 * Make this object represent the range <code>start - end</code>.
 * If <code>end > start</code> then this object is set to an
 * an empty range.
 *
 * @param start first character in the set, inclusive
 * @rparam end last character in the set, inclusive
 */
Unicode_समुच्चयः& Unicode_समुच्चयः::समुच्चयः(UChar32 start, UChar32 अन्तः) U_LIFETIME_BOUND {
    clear();
    complement(start, अन्तः);
    प्रत्यागम *एतत्;
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
Unicode_समुच्चयः& Unicode_समुच्चयः::सङ्कलय(UChar32 start, UChar32 अन्तः) U_LIFETIME_BOUND {
    यदि (pin_संहिता_Point(start) < pin_संहिता_Point(अन्तः)) {
        UChar32 सीमा = अन्तः + 1;
        // Fast path for adding a new range after the last one.
        // Odd list length: [..., lastStart, lastLimit, HIGH]
        यदि ((len & 1) != 0) {
            // If the list is empty, set lastLimit low enough to not be adjacent to 0.
            UChar32 चरमः_सीमा = len == 1 ? -2 : list[len - 2];
            यदि (चरमः_सीमा <= start && !isFrozen() && !isBogus()) {
                यदि (चरमः_सीमा == start) {
                    // Extend the last range.
                    list[len - 2] = सीमा;
                    यदि (सीमा == UNICODESET_उच्चः) {
                        --len;
                    }
                } अन्यथा {
                    list[len - 1] = start;
                    यदि (सीमा < UNICODESET_उच्चः) {
                        यदि (ensure_क्षमता(len + 2)) {
                            list[len++] = सीमा;
                            list[len++] = UNICODESET_उच्चः;
                        }
                    } अन्यथा {  // limit == UNICODESET_HIGH
                        यदि (ensure_क्षमता(len + 1)) {
                            list[len++] = UNICODESET_उच्चः;
                        }
                    }
                }
                release_आदर्शः();
                प्रत्यागम *एतत्;
            }
        }
        // This is slow. Could be much faster using findCodePoint(start)
        // and modifying the list, dealing with adjacent & overlapping ranges.
        UChar32 range[3] = { start, सीमा, UNICODESET_उच्चः };
        सङ्कलय(range, 2, 0);
    } अन्यथा यदि (start == अन्तः) {
        सङ्कलय(start);
    }
    प्रत्यागम *एतत्;
}

// #define DEBUG_US_ADD

#यदि_परिभाषितम् DEBUG_US_सङ्कलय
#अन्तर्भाव <stdio.h>
शून्य dump(UChar32 c) {
    यदि (c <= 0xFF) {
        प्रारूप_लिख("%c", (वर्ण)c);
    } अन्यथा {
        प्रारूप_लिख("U+%04X", c);
    }
}
शून्य dump(ध्रुव UChar32* list, int32_t len) {
    प्रारूप_लिख("[");
    प्रति (int32_t i=0; i<len; ++i) {
        यदि (i != 0) प्रारूप_लिख(", ");
        dump(list[i]);
    }
    प्रारूप_लिख("]");
}
#यदि_अन्तः

/**
 * Adds the specified character to this set if it is not already
 * present.  If this set already contains the specified character,
 * the call leaves this set unchanged.
 */
Unicode_समुच्चयः& Unicode_समुच्चयः::सङ्कलय(UChar32 c) U_LIFETIME_BOUND {
    // find smallest i such that c < list[i]
    // if odd, then it is IN the set
    // if even, then it is OUT of the set
    int32_t i = find_संहिता_Point(pin_संहिता_Point(c));

    // already in set?
    यदि ((i & 1) != 0  || isFrozen() || isBogus()) प्रत्यागम *एतत्;

    // HIGH is 0x110000
    // assert(list[len-1] == HIGH);

    // empty = [HIGH]
    // [start_0, limit_0, start_1, limit_1, HIGH]

    // [..., start_k-1, limit_k-1, start_k, limit_k, ..., HIGH]
    //                             ^
    //                             list[i]

    // i == 0 means c is before the first range

#यदि_परिभाषितम् DEBUG_US_सङ्कलय
    प्रारूप_लिख("Add of ");
    dump(c);
    प्रारूप_लिख(" found at %d", i);
    प्रारूप_लिख(": ");
    dump(list, len);
    प्रारूप_लिख(" => ");
#यदि_अन्तः

    यदि (c == list[i]-1) {
        // c is before start of next range
        list[i] = c;
        // if we touched the HIGH mark, then add a new one
        यदि (c == (UNICODESET_उच्चः - 1)) {
            यदि (!ensure_क्षमता(len+1)) {
                // ensureCapacity will mark the object as Bogus if OOM failure happens.
                प्रत्यागम *एतत्;
            }
            list[len++] = UNICODESET_उच्चः;
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
            यावत् (src < srclimit) *(dst++) = *(src++);

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

        यदि (!ensure_क्षमता(len+2)) {
            // ensureCapacity will mark the object as Bogus if OOM failure happens.
            प्रत्यागम *एतत्;
        }

        UChar32 *p = list + i;
        uprv_memmove(p + 2, p, (len - i) * परिमाण(*p));
        list[i] = c;
        list[i+1] = c+1;
        len += 2;
    }

#यदि_परिभाषितम् DEBUG_US_सङ्कलय
    dump(list, len);
    प्रारूप_लिख("\n");

    प्रति (i=1; i<len; ++i) {
        यदि (list[i] <= list[i-1]) {
            // Corrupt array!
            प्रारूप_लिख("ERROR: list has been corrupted\n");
            निर्गच्छ(1);
        }
    }
#यदि_अन्तः

    release_आदर्शः();
    प्रत्यागम *एतत्;
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
Unicode_समुच्चयः& Unicode_समुच्चयः::सङ्कलय(ध्रुव UnicodeString& s) U_LIFETIME_BOUND {
    यदि (isFrozen() || isBogus()) प्रत्यागम *एतत्;
    int32_t cp = getSingleCP(s);
    यदि (cp < 0) {
        यदि (!वाक्यानि_अन्तर्भवति(s)) {
            सङ्कलय_2(s);
            release_आदर्शः();
        }
    } अन्यथा {
        सङ्कलय(static_cast<UChar32>(cp));
    }
    प्रत्यागम *एतत्;
}

/**
 * Adds the given string, in order, to 'strings_'.  The given string
 * must have been checked by the caller to not already be in 'strings_'.
 */
शून्य Unicode_समुच्चयः::सङ्कलय_2(ध्रुव UnicodeString& s) {
    यदि (isFrozen() || isBogus()) {
        प्रत्यागम;
    }
    UError_संहिता ec = U_ZERO_ERROR;
    यदि (वाक्यानि_2 == शून्यसूचकः && !allocate_वाक्यानि(ec)) {
        समुच्चयः_गन्तव्यम्_Bogus();
        प्रत्यागम;
    }
    LocalPointer<UnicodeString> t(नूतन UnicodeString(s));
    यदि (t.isNull()) { // Check for memory allocation error.
        समुच्चयः_गन्तव्यम्_Bogus();
        प्रत्यागम;
    }
    वाक्यानि_2->sorted_निवेशय(t.orphan(), compareUnicodeString, ec);
    यदि (U_FAILURE(ec)) {
        समुच्चयः_गन्तव्यम्_Bogus();
    }
}

/**
 * @return a code point IF the string consists of a single one.
 * otherwise returns -1.
 * @param string to test
 */
int32_t Unicode_समुच्चयः::getSingleCP(ध्रुव UnicodeString& s) {
    int32_t s_दीर्घता = s.length();
    यदि (s_दीर्घता == 1) प्रत्यागम s.charAt(0);
    यदि (s_दीर्घता == 2) {
        UChar32 cp = s.char32At(0);
        यदि (cp > 0xFFFF) { // is surrogate pair
            प्रत्यागम cp;
        }
    }
    प्रत्यागम -1;
}

/**
 * Adds each of the characters in this string to the set. Thus "ch" => {"c", "h"}
 * If this set already any particular character, it has no effect on that character.
 * @param the source string
 * @return the modified set, for chaining
 */
Unicode_समुच्चयः& Unicode_समुच्चयः::सङ्कलय_All(ध्रुव UnicodeString& s) U_LIFETIME_BOUND {
    UChar32 cp;
    प्रति (int32_t i = 0; i < s.length(); i += U16_दीर्घता(cp)) {
        cp = s.char32At(i);
        सङ्कलय(cp);
    }
    प्रत्यागम *एतत्;
}

/**
 * Retains EACH of the characters in this string. Note: "ch" == {"c", "h"}
 * If this set already any particular character, it has no effect on that character.
 * @param the source string
 * @return the modified set, for chaining
 */
Unicode_समुच्चयः& Unicode_समुच्चयः::retainAll(ध्रुव UnicodeString& s) U_LIFETIME_BOUND {
    Unicode_समुच्चयः समुच्चयः;
    समुच्चयः.सङ्कलय_All(s);
    retainAll(समुच्चयः);
    प्रत्यागम *एतत्;
}

/**
 * Complement EACH of the characters in this string. Note: "ch" == {"c", "h"}
 * If this set already any particular character, it has no effect on that character.
 * @param the source string
 * @return the modified set, for chaining
 */
Unicode_समुच्चयः& Unicode_समुच्चयः::complementAll(ध्रुव UnicodeString& s) U_LIFETIME_BOUND {
    Unicode_समुच्चयः समुच्चयः;
    समुच्चयः.सङ्कलय_All(s);
    complementAll(समुच्चयः);
    प्रत्यागम *एतत्;
}

/**
 * Remove EACH of the characters in this string. Note: "ch" == {"c", "h"}
 * If this set already any particular character, it has no effect on that character.
 * @param the source string
 * @return the modified set, for chaining
 */
Unicode_समुच्चयः& Unicode_समुच्चयः::removeAll(ध्रुव UnicodeString& s) U_LIFETIME_BOUND {
    Unicode_समुच्चयः समुच्चयः;
    समुच्चयः.सङ्कलय_All(s);
    removeAll(समुच्चयः);
    प्रत्यागम *एतत्;
}

Unicode_समुच्चयः& Unicode_समुच्चयः::remove_All_वाक्यानि() U_LIFETIME_BOUND {
    यदि (!isFrozen() && has_वाक्यानि()) {
        वाक्यानि_2->removeAllElements();
        release_आदर्शः();
    }
    प्रत्यागम *एतत्;
}


/**
 * Makes a set from a multicharacter string. Thus "ch" => {"ch"}
 * <br><b>Warning: you cannot add an empty string ("") to a UnicodeSet.</b>
 * @param the source string
 * @return a newly created set containing the given string
 */
Unicode_समुच्चयः* U_EXPORT2 Unicode_समुच्चयः::createFrom(ध्रुव UnicodeString& s) {
    Unicode_समुच्चयः *समुच्चयः = नूतन Unicode_समुच्चयः();
    यदि (समुच्चयः != शून्यसूचकः) { // Check for memory allocation error.
        समुच्चयः->सङ्कलय(s);
    }
    प्रत्यागम समुच्चयः;
}


/**
 * Makes a set from each of the characters in the string. Thus "ch" => {"c", "h"}
 * @param the source string
 * @return a newly created set containing the given characters
 */
Unicode_समुच्चयः* U_EXPORT2 Unicode_समुच्चयः::createFromAll(ध्रुव UnicodeString& s) {
    Unicode_समुच्चयः *समुच्चयः = नूतन Unicode_समुच्चयः();
    यदि (समुच्चयः != शून्यसूचकः) { // Check for memory allocation error.
        समुच्चयः->सङ्कलय_All(s);
    }
    प्रत्यागम समुच्चयः;
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
Unicode_समुच्चयः& Unicode_समुच्चयः::retain(UChar32 start, UChar32 अन्तः) U_LIFETIME_BOUND {
    यदि (pin_संहिता_Point(start) <= pin_संहिता_Point(अन्तः)) {
        UChar32 range[3] = { start, अन्तः+1, UNICODESET_उच्चः };
        retain(range, 2, 0);
    } अन्यथा {
        clear();
    }
    प्रत्यागम *एतत्;
}

Unicode_समुच्चयः& Unicode_समुच्चयः::retain(UChar32 c) U_LIFETIME_BOUND {
    प्रत्यागम retain(c, c);
}

Unicode_समुच्चयः& Unicode_समुच्चयः::retain(ध्रुव UnicodeString &s) U_LIFETIME_BOUND {
    यदि (isFrozen() || isBogus()) { प्रत्यागम *एतत्; }
    UChar32 cp = getSingleCP(s);
    यदि (cp < 0) {
        तार्किक is_अन्तर् = वाक्यानि_अन्तर्भवति(s);
        // Check for getRangeCount() first to avoid somewhat-expensive size()
        // when there are single code points.
        यदि (is_अन्तर् && get_Range_गणनम्() == 0 && size() == 1) {
            प्रत्यागम *एतत्;
        }
        clear();
        यदि (is_अन्तर्) {
            सङ्कलय_2(s);
        }
    } अन्यथा {
        retain(cp, cp);
    }
    प्रत्यागम *एतत्;
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
Unicode_समुच्चयः& Unicode_समुच्चयः::remove(UChar32 start, UChar32 अन्तः) U_LIFETIME_BOUND {
    यदि (pin_संहिता_Point(start) <= pin_संहिता_Point(अन्तः)) {
        UChar32 range[3] = { start, अन्तः+1, UNICODESET_उच्चः };
        retain(range, 2, 2);
    }
    प्रत्यागम *एतत्;
}

/**
 * Removes the specified character from this set if it is present.
 * The set will not contain the specified range once the call
 * returns.
 */
Unicode_समुच्चयः& Unicode_समुच्चयः::remove(UChar32 c) U_LIFETIME_BOUND {
    प्रत्यागम remove(c, c);
}

/**
 * Removes the specified string from this set if it is present.
 * The set will not contain the specified character once the call
 * returns.
 * @param the source string
 * @return the modified set, for chaining
 */
Unicode_समुच्चयः& Unicode_समुच्चयः::remove(ध्रुव UnicodeString& s) U_LIFETIME_BOUND {
    यदि (isFrozen() || isBogus()) प्रत्यागम *एतत्;
    int32_t cp = getSingleCP(s);
    यदि (cp < 0) {
        यदि (वाक्यानि_2 != शून्यसूचकः && वाक्यानि_2->removeElement((शून्य*) &s)) {
            release_आदर्शः();
        }
    } अन्यथा {
        remove(static_cast<UChar32>(cp), static_cast<UChar32>(cp));
    }
    प्रत्यागम *एतत्;
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
Unicode_समुच्चयः& Unicode_समुच्चयः::complement(UChar32 start, UChar32 अन्तः) U_LIFETIME_BOUND {
    यदि (isFrozen() || isBogus()) {
        प्रत्यागम *एतत्;
    }
    यदि (pin_संहिता_Point(start) <= pin_संहिता_Point(अन्तः)) {
        UChar32 range[3] = { start, अन्तः+1, UNICODESET_उच्चः };
        exclusiveOr(range, 2, 0);
    }
    release_आदर्शः();
    प्रत्यागम *एतत्;
}

Unicode_समुच्चयः& Unicode_समुच्चयः::complement(UChar32 c) U_LIFETIME_BOUND {
    प्रत्यागम complement(c, c);
}

/**
 * This is equivalent to
 * <code>complement(MIN_VALUE, MAX_VALUE)</code>.
 */
Unicode_समुच्चयः& Unicode_समुच्चयः::complement() U_LIFETIME_BOUND {
    यदि (isFrozen() || isBogus()) {
        प्रत्यागम *एतत्;
    }
    यदि (list[0] == UNICODESET_नीचः) {
        uprv_memmove(list, list + 1, (size_t)(len-1)*परिमाण(UChar32));
        --len;
    } अन्यथा {
        यदि (!ensure_क्षमता(len+1)) {
            प्रत्यागम *एतत्;
        }
        uprv_memmove(list + 1, list, (size_t)len*परिमाण(UChar32));
        list[0] = UNICODESET_नीचः;
        ++len;
    }
    release_आदर्शः();
    प्रत्यागम *एतत्;
}

/**
 * Complement the specified string in this set.
 * The set will not contain the specified string once the call
 * returns.
 *
 * @param s the string to complement
 * @return this object, for chaining
 */
Unicode_समुच्चयः& Unicode_समुच्चयः::complement(ध्रुव UnicodeString& s) U_LIFETIME_BOUND {
    यदि (isFrozen() || isBogus()) प्रत्यागम *एतत्;
    int32_t cp = getSingleCP(s);
    यदि (cp < 0) {
        यदि (वाक्यानि_अन्तर्भवति(s)) {
            वाक्यानि_2->removeElement((शून्य*) &s);
        } अन्यथा {
            सङ्कलय_2(s);
        }
        release_आदर्शः();
    } अन्यथा {
        complement(static_cast<UChar32>(cp), static_cast<UChar32>(cp));
    }
    प्रत्यागम *एतत्;
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
Unicode_समुच्चयः& Unicode_समुच्चयः::सङ्कलय_All(ध्रुव Unicode_समुच्चयः& c) U_LIFETIME_BOUND {
    यदि ( c.len>0 && c.list!=शून्यसूचकः ) {
        सङ्कलय(c.list, c.len, 0);
    }

    // Add strings in order
    यदि ( c.वाक्यानि_2!=शून्यसूचकः ) {
        प्रति (int32_t i=0; i<c.वाक्यानि_2->size(); ++i) {
            ध्रुव UnicodeString* s = static_cast<ध्रुव UnicodeString*>(c.वाक्यानि_2->elementAt(i));
            यदि (!वाक्यानि_अन्तर्भवति(*s)) {
                सङ्कलय_2(*s);
            }
        }
    }
    प्रत्यागम *एतत्;
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
Unicode_समुच्चयः& Unicode_समुच्चयः::retainAll(ध्रुव Unicode_समुच्चयः& c) U_LIFETIME_BOUND {
    यदि (isFrozen() || isBogus()) {
        प्रत्यागम *एतत्;
    }
    retain(c.list, c.len, 0);
    यदि (has_वाक्यानि()) {
        यदि (!c.has_वाक्यानि()) {
            वाक्यानि_2->removeAllElements();
        } अन्यथा {
            वाक्यानि_2->retainAll(*c.वाक्यानि_2);
        }
    }
    प्रत्यागम *एतत्;
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
Unicode_समुच्चयः& Unicode_समुच्चयः::removeAll(ध्रुव Unicode_समुच्चयः& c) U_LIFETIME_BOUND {
    यदि (isFrozen() || isBogus()) {
        प्रत्यागम *एतत्;
    }
    retain(c.list, c.len, 2);
    यदि (has_वाक्यानि() && c.has_वाक्यानि()) {
        वाक्यानि_2->removeAll(*c.वाक्यानि_2);
    }
    प्रत्यागम *एतत्;
}

/**
 * Complements in this set all elements contained in the specified
 * set.  Any character in the other set will be removed if it is
 * in this set, or will be added if it is not in this set.
 *
 * @param c set that defines which elements will be xor'ed from
 *          this set.
 */
Unicode_समुच्चयः& Unicode_समुच्चयः::complementAll(ध्रुव Unicode_समुच्चयः& c) U_LIFETIME_BOUND {
    यदि (isFrozen() || isBogus()) {
        प्रत्यागम *एतत्;
    }
    exclusiveOr(c.list, c.len, 0);

    यदि (c.वाक्यानि_2 != शून्यसूचकः) {
        प्रति (int32_t i=0; i<c.वाक्यानि_2->size(); ++i) {
            शून्य* e = c.वाक्यानि_2->elementAt(i);
            यदि (वाक्यानि_2 == शून्यसूचकः || !वाक्यानि_2->removeElement(e)) {
                सङ्कलय_2(*static_cast<ध्रुव UnicodeString*>(e));
            }
        }
    }
    प्रत्यागम *एतत्;
}

/**
 * Removes all of the elements from this set.  This set will be
 * empty after this call returns.
 */
Unicode_समुच्चयः& Unicode_समुच्चयः::clear() U_LIFETIME_BOUND {
    यदि (isFrozen()) {
        प्रत्यागम *एतत्;
    }
    list[0] = UNICODESET_उच्चः;
    len = 1;
    release_आदर्शः();
    यदि (वाक्यानि_2 != शून्यसूचकः) {
        वाक्यानि_2->removeAllElements();
    }
    // Remove bogus
    fFlags = 0;
    प्रत्यागम *एतत्;
}

/**
 * Iteration method that returns the number of ranges contained in
 * this set.
 * @see #getRangeStart
 * @see #getRangeEnd
 */
int32_t Unicode_समुच्चयः::get_Range_गणनम्() ध्रुव {
    प्रत्यागम len/2;
}

/**
 * Iteration method that returns the first character in the
 * specified range of this set.
 * @see #getRangeCount
 * @see #getRangeEnd
 */
UChar32 Unicode_समुच्चयः::getRangeStart(int32_t सूचकांक) ध्रुव {
    प्रत्यागम list[सूचकांक*2];
}

/**
 * Iteration method that returns the last character in the
 * specified range of this set.
 * @see #getRangeStart
 * @see #getRangeEnd
 */
UChar32 Unicode_समुच्चयः::get_Range_अन्तः(int32_t सूचकांक) ध्रुव {
    प्रत्यागम list[सूचकांक*2 + 1] - 1;
}

ध्रुव UnicodeString* Unicode_समुच्चयः::getString(int32_t सूचकांक) ध्रुव U_LIFETIME_BOUND {
    प्रत्यागम static_cast<ध्रुव UnicodeString*>(वाक्यानि_2->elementAt(सूचकांक));
}

/**
 * Reallocate this objects internal structures to take up the least
 * possible space, without changing this object's value.
 */
Unicode_समुच्चयः& Unicode_समुच्चयः::compact() U_LIFETIME_BOUND {
    यदि (isFrozen() || isBogus()) {
        प्रत्यागम *एतत्;
    }
    // Delete buffer first to defragment memory less.
    यदि (बफर != राशिः_List) {
        uprv_free(बफर);
        बफर = शून्यसूचकः;
        बफर_क्षमता = 0;
    }
    यदि (list == राशिः_List) {
        // pass
    } अन्यथा यदि (len <= INITIAL_क्षमता) {
        uprv_memcpy(राशिः_List, list, len * परिमाण(UChar32));
        uprv_free(list);
        list = राशिः_List;
        capacity = INITIAL_क्षमता;
    } अन्यथा यदि ((len + 7) < capacity) {
        // If we have more than a little unused capacity, shrink it to len.
        UChar32* अस्थायी = static_cast<UChar32*>(uprv_realloc(list, परिमाण(UChar32) * len));
        यदि (अस्थायी) {
            list = अस्थायी;
            capacity = len;
        }
        // else what the heck happened?! We allocated less memory!
        // Oh well. We'll keep our original array.
    }
    यदि (वाक्यानि_2 != शून्यसूचकः && वाक्यानि_2->is_रिक्तानि()) {
        विलोपय वाक्यानि_2;
        वाक्यानि_2 = शून्यसूचकः;
    }
    प्रत्यागम *एतत्;
}

#यदि_परिभाषितम् DEBUG_SERIALIZE
#अन्तर्भाव <stdio.h>
#यदि_अन्तः

/**
 * Deserialize constructor.
 */
Unicode_समुच्चयः::Unicode_समुच्चयः(ध्रुव uint16_t दत्त[], int32_t दत्त_Len, ESerialization serialization,
                       UError_संहिता &ec) {

  यदि(U_FAILURE(ec)) {
    समुच्चयः_गन्तव्यम्_Bogus();
    प्रत्यागम;
  }

  यदि( (serialization != kSerialized)
      || (दत्त==शून्यसूचकः)
      || (दत्त_Len < 1)) {
    ec = U_ILLEGAL_तर्कः_ERROR;
    समुच्चयः_गन्तव्यम्_Bogus();
    प्रत्यागम;
  }

  // bmp?
  int32_t शीर्षकम्_आकार = ((दत्त[0]&0x8000)) ?2:1;
  int32_t bmp_दीर्घता = (शीर्षकम्_आकार==1)?दत्त[0]:दत्त[1];

  int32_t new_दीर्घता = (((दत्त[0]&0x7FFF)-bmp_दीर्घता)/2)+bmp_दीर्घता;
#यदि_परिभाषितम् DEBUG_SERIALIZE
  प्रारूप_लिख("dataLen %d headerSize %d bmpLen %d len %d. data[0]=%X/%X/%X/%X\n", दत्त_Len,शीर्षकम्_आकार,bmp_दीर्घता,new_दीर्घता, दत्त[0],दत्त[1],दत्त[2],दत्त[3]);
#यदि_अन्तः
  यदि(!ensure_क्षमता(new_दीर्घता + 1)) {  // +1 for HIGH
    प्रत्यागम;
  }
  // copy bmp
  int32_t i;
  प्रति(i = 0; i< bmp_दीर्घता;i++) {
    list[i] = दत्त[i+शीर्षकम्_आकार];
#यदि_परिभाषितम् DEBUG_SERIALIZE
    प्रारूप_लिख("<<16@%d[%d] %X\n", i+शीर्षकम्_आकार, i, list[i]);
#यदि_अन्तः
  }
  // copy smp
  प्रति(i=bmp_दीर्घता;i<new_दीर्घता;i++) {
    list[i] = (static_cast<UChar32>(दत्त[शीर्षकम्_आकार + bmp_दीर्घता + (i - bmp_दीर्घता) * 2 + 0]) << 16) +
               static_cast<UChar32>(दत्त[शीर्षकम्_आकार + bmp_दीर्घता + (i - bmp_दीर्घता) * 2 + 1]);
#यदि_परिभाषितम् DEBUG_SERIALIZE
    प्रारूप_लिख("<<32@%d+[%d] %lX\n", शीर्षकम्_आकार+bmp_दीर्घता+i, i, list[i]);
#यदि_अन्तः
  }
  U_ASSERT(i == new_दीर्घता);
  यदि (i == 0 || list[i - 1] != UNICODESET_उच्चः) {
    list[i++] = UNICODESET_उच्चः;
  }
  len = i;
}


int32_t Unicode_समुच्चयः::serialize(uint16_t *dest, int32_t dest_क्षमता, UError_संहिता& ec) ध्रुव {
    int32_t bmp_दीर्घता, length, dest_दीर्घता;

    यदि (U_FAILURE(ec)) {
        प्रत्यागम 0;
    }

    यदि (dest_क्षमता<0 || (dest_क्षमता>0 && dest==शून्यसूचकः)) {
        ec=U_ILLEGAL_तर्कः_ERROR;
        प्रत्यागम 0;
    }

    /* count necessary 16-bit units */
    length=एतत्->len-1; // Subtract 1 to ignore final UNICODESET_HIGH
    // assert(length>=0);
    यदि (length==0) {
        /* empty set */
        यदि (dest_क्षमता>0) {
            *dest=0;
        } अन्यथा {
            ec=U_बफर_अतिप्रवाहः_ERROR;
        }
        प्रत्यागम 1;
    }
    /* now length>0 */

    यदि (एतत्->list[length-1]<=0xffff) {
        /* all BMP */
        bmp_दीर्घता=length;
    } अन्यथा यदि (एतत्->list[0]>=0x10000) {
        /* all supplementary */
        bmp_दीर्घता=0;
        length*=2;
    } अन्यथा {
        /* some BMP, some supplementary */
        प्रति (bmp_दीर्घता=0; bmp_दीर्घता<length && एतत्->list[bmp_दीर्घता]<=0xffff; ++bmp_दीर्घता) {}
        length=bmp_दीर्घता+2*(length-bmp_दीर्घता);
    }
#यदि_परिभाषितम् DEBUG_SERIALIZE
    प्रारूप_लिख(">> bmpLength%d length%d len%d\n", bmp_दीर्घता, length, len);
#यदि_अन्तः
    /* length: number of 16-bit array units */
    यदि (length>0x7fff) {
        /* there are only 15 bits for the length in the first serialized word */
        ec=U_सूचकांक_OUTOFBOUNDS_ERROR;
        प्रत्यागम 0;
    }

    /*
     * total serialized length:
     * number of 16-bit array units (length) +
     * 1 length unit (always) +
     * 1 bmpLength unit (if there are supplementary values)
     */
    dest_दीर्घता=length+((length>bmp_दीर्घता)?2:1);
    यदि (dest_दीर्घता<=dest_क्षमता) {
        ध्रुव UChar32 *p;
        int32_t i;

#यदि_परिभाषितम् DEBUG_SERIALIZE
        प्रारूप_लिख("writeHdr\n");
#यदि_अन्तः
        *dest = static_cast<uint16_t>(length);
        यदि (length>bmp_दीर्घता) {
            *dest|=0x8000;
            *++dest = static_cast<uint16_t>(bmp_दीर्घता);
        }
        ++dest;

        /* write the BMP part of the array */
        p=एतत्->list;
        प्रति (i=0; i<bmp_दीर्घता; ++i) {
#यदि_परिभाषितम् DEBUG_SERIALIZE
          प्रारूप_लिख("writebmp: %x\n", (पूर्णाङ्क)*p);
#यदि_अन्तः
            *dest++ = static_cast<uint16_t>(*p++);
        }

        /* write the supplementary part of the array */
        प्रति (; i<length; i+=2) {
#यदि_परिभाषितम् DEBUG_SERIALIZE
          प्रारूप_लिख("write32: %x\n", (पूर्णाङ्क)*p);
#यदि_अन्तः
            *dest++ = static_cast<uint16_t>(*p >> 16);
            *dest++ = static_cast<uint16_t>(*p++);
        }
    } अन्यथा {
        ec=U_बफर_अतिप्रवाहः_ERROR;
    }
    प्रत्यागम dest_दीर्घता;
}

//----------------------------------------------------------------
// Implementation: Utility methods
//----------------------------------------------------------------

/**
 * Allocate our strings vector and return true if successful.
 */
UBool Unicode_समुच्चयः::allocate_वाक्यानि(UError_संहिता &स्थितिः) {
    यदि (U_FAILURE(स्थितिः)) {
        प्रत्यागम असत्य;
    }
    वाक्यानि_2 = नूतन UVector(uprv_deleteUObject,
                          uhash_compareUnicodeString, 1, स्थितिः);
    यदि (वाक्यानि_2 == शून्यसूचकः) { // Check for memory allocation error.
        स्थितिः = U_स्मृतिः_आवण्टनम्_ERROR;
        प्रत्यागम असत्य;
    }
    यदि (U_FAILURE(स्थितिः)) {
        विलोपय वाक्यानि_2;
        वाक्यानि_2 = शून्यसूचकः;
        प्रत्यागम असत्य;
    } 
    प्रत्यागम सत्य;
}

int32_t Unicode_समुच्चयः::अग्र_क्षमता(int32_t min_क्षमता) {
    // Grow exponentially to reduce the frequency of allocations.
    यदि (min_क्षमता < INITIAL_क्षमता) {
        प्रत्यागम min_क्षमता + INITIAL_क्षमता;
    } अन्यथा यदि (min_क्षमता <= 2500) {
        प्रत्यागम 5 * min_क्षमता;
    } अन्यथा {
        int32_t new_क्षमता = 2 * min_क्षमता;
        यदि (new_क्षमता > MAX_दीर्घता) {
            new_क्षमता = MAX_दीर्घता;
        }
        प्रत्यागम new_क्षमता;
    }
}

तार्किक Unicode_समुच्चयः::ensure_क्षमता(int32_t newLen) {
    यदि (newLen > MAX_दीर्घता) {
        newLen = MAX_दीर्घता;
    }
    यदि (newLen <= capacity) {
        प्रत्यागम सत्य;
    }
    int32_t new_क्षमता = अग्र_क्षमता(newLen);
    UChar32* अस्थायी = static_cast<UChar32*>(uprv_malloc(new_क्षमता * परिमाण(UChar32)));
    यदि (अस्थायी == शून्यसूचकः) {
        समुच्चयः_गन्तव्यम्_Bogus(); // set the object to bogus state if an OOM failure occurred.
        प्रत्यागम असत्य;
    }
    // Copy only the actual contents.
    uprv_memcpy(अस्थायी, list, len * परिमाण(UChar32));
    यदि (list != राशिः_List) {
        uprv_free(list);
    }
    list = अस्थायी;
    capacity = new_क्षमता;
    प्रत्यागम सत्य;
}

तार्किक Unicode_समुच्चयः::ensure_बफर_क्षमता(int32_t newLen) {
    यदि (newLen > MAX_दीर्घता) {
        newLen = MAX_दीर्घता;
    }
    यदि (newLen <= बफर_क्षमता) {
        प्रत्यागम सत्य;
    }
    int32_t new_क्षमता = अग्र_क्षमता(newLen);
    UChar32* अस्थायी = static_cast<UChar32*>(uprv_malloc(new_क्षमता * परिमाण(UChar32)));
    यदि (अस्थायी == शून्यसूचकः) {
        समुच्चयः_गन्तव्यम्_Bogus();
        प्रत्यागम असत्य;
    }
    // The buffer has no contents to be copied.
    // It is always filled from scratch after this call.
    यदि (बफर != राशिः_List) {
        uprv_free(बफर);
    }
    बफर = अस्थायी;
    बफर_क्षमता = new_क्षमता;
    प्रत्यागम सत्य;
}

/**
 * Swap list and buffer.
 */
शून्य Unicode_समुच्चयः::swapBuffers() {
    // swap list and buffer
    UChar32* अस्थायी = list;
    list = बफर;
    बफर = अस्थायी;

    int32_t c = capacity;
    capacity = बफर_क्षमता;
    बफर_क्षमता = c;
}

शून्य Unicode_समुच्चयः::समुच्चयः_गन्तव्यम्_Bogus() {
    clear(); // Remove everything in the set.
    fFlags = kIsBogus;
}

//----------------------------------------------------------------
// Implementation: Fundamental operators
//----------------------------------------------------------------

स्थिर अन्तःस्थ UChar32 max(UChar32 a, UChar32 b) {
    प्रत्यागम (a > b) ? a : b;
}

// polarity = 0, 3 is normal: x xor y
// polarity = 1, 2: x xor ~y == x === y

शून्य Unicode_समुच्चयः::exclusiveOr(ध्रुव UChar32* other, int32_t otherLen, int8_t polarity) {
    यदि (isFrozen() || isBogus()) {
        प्रत्यागम;
    }
    यदि (!ensure_बफर_क्षमता(len + otherLen)) {
        प्रत्यागम;
    }

    int32_t i = 0, j = 0, k = 0;
    UChar32 a = list[i++];
    UChar32 b;
    यदि (polarity == 1 || polarity == 2) {
        b = UNICODESET_नीचः;
        यदि (other[j] == UNICODESET_नीचः) { // skip base if already LOW
            ++j;
            b = other[j];
        }
    } अन्यथा {
        b = other[j++];
    }
    // simplest of all the routines
    // sort the values, discarding identicals!
    प्रति (;;) {
        यदि (a < b) {
            बफर[k++] = a;
            a = list[i++];
        } अन्यथा यदि (b < a) {
            बफर[k++] = b;
            b = other[j++];
        } अन्यथा यदि (a != UNICODESET_उच्चः) { // at this point, a == b
            // discard both values!
            a = list[i++];
            b = other[j++];
        } अन्यथा { // DONE!
            बफर[k++] = UNICODESET_उच्चः;
            len = k;
            भङ्ग;
        }
    }
    swapBuffers();
    release_आदर्शः();
}

// polarity = 0 is normal: x union y
// polarity = 2: x union ~y
// polarity = 1: ~x union y
// polarity = 3: ~x union ~y

शून्य Unicode_समुच्चयः::सङ्कलय(ध्रुव UChar32* other, int32_t otherLen, int8_t polarity) {
    यदि (isFrozen() || isBogus() || other==शून्यसूचकः) {
        प्रत्यागम;
    }
    यदि (!ensure_बफर_क्षमता(len + otherLen)) {
        प्रत्यागम;
    }

    int32_t i = 0, j = 0, k = 0;
    UChar32 a = list[i++];
    UChar32 b = other[j++];
    // change from xor is that we have to check overlapping pairs
    // polarity bit 1 means a is second, bit 2 means b is.
    प्रति (;;) {
        विकल्प (polarity) {
          पक्ष 0: // both first; take lower if unequal
            यदि (a < b) { // take a
                // Back up over overlapping ranges in buffer[]
                यदि (k > 0 && a <= बफर[k-1]) {
                    // Pick latter end value in buffer[] vs. list[]
                    a = max(list[i], बफर[--k]);
                } अन्यथा {
                    // No overlap
                    बफर[k++] = a;
                    a = list[i];
                }
                i++; // Common if/else code factored out
                polarity ^= 1;
            } अन्यथा यदि (b < a) { // take b
                यदि (k > 0 && b <= बफर[k-1]) {
                    b = max(other[j], बफर[--k]);
                } अन्यथा {
                    बफर[k++] = b;
                    b = other[j];
                }
                j++;
                polarity ^= 2;
            } अन्यथा { // a == b, take a, drop b
                यदि (a == UNICODESET_उच्चः) गच्छ loop_अन्तः;
                // This is symmetrical; it doesn't matter if
                // we backtrack with a or b. - liu
                यदि (k > 0 && a <= बफर[k-1]) {
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
            भङ्ग;
          पक्ष 3: // both second; take higher if unequal, and drop other
            यदि (b <= a) { // take a
                यदि (a == UNICODESET_उच्चः) गच्छ loop_अन्तः;
                बफर[k++] = a;
            } अन्यथा { // take b
                यदि (b == UNICODESET_उच्चः) गच्छ loop_अन्तः;
                बफर[k++] = b;
            }
            a = list[i++];
            polarity ^= 1;   // factored common code
            b = other[j++];
            polarity ^= 2;
            भङ्ग;
          पक्ष 1: // a second, b first; if b < a, overlap
            यदि (a < b) { // no overlap, take a
                बफर[k++] = a; a = list[i++]; polarity ^= 1;
            } अन्यथा यदि (b < a) { // OVERLAP, drop b
                b = other[j++];
                polarity ^= 2;
            } अन्यथा { // a == b, drop both!
                यदि (a == UNICODESET_उच्चः) गच्छ loop_अन्तः;
                a = list[i++];
                polarity ^= 1;
                b = other[j++];
                polarity ^= 2;
            }
            भङ्ग;
          पक्ष 2: // a first, b second; if a < b, overlap
            यदि (b < a) { // no overlap, take b
                बफर[k++] = b;
                b = other[j++];
                polarity ^= 2;
            } अन्यथा  यदि (a < b) { // OVERLAP, drop a
                a = list[i++];
                polarity ^= 1;
            } अन्यथा { // a == b, drop both!
                यदि (a == UNICODESET_उच्चः) गच्छ loop_अन्तः;
                a = list[i++];
                polarity ^= 1;
                b = other[j++];
                polarity ^= 2;
            }
            भङ्ग;
        }
    }
 loop_अन्तः:
    बफर[k++] = UNICODESET_उच्चः;    // terminate
    len = k;
    swapBuffers();
    release_आदर्शः();
}

// polarity = 0 is normal: x intersect y
// polarity = 2: x intersect ~y == set-minus
// polarity = 1: ~x intersect y
// polarity = 3: ~x intersect ~y

शून्य Unicode_समुच्चयः::retain(ध्रुव UChar32* other, int32_t otherLen, int8_t polarity) {
    यदि (isFrozen() || isBogus()) {
        प्रत्यागम;
    }
    यदि (!ensure_बफर_क्षमता(len + otherLen)) {
        प्रत्यागम;
    }

    int32_t i = 0, j = 0, k = 0;
    UChar32 a = list[i++];
    UChar32 b = other[j++];
    // change from xor is that we have to check overlapping pairs
    // polarity bit 1 means a is second, bit 2 means b is.
    प्रति (;;) {
        विकल्प (polarity) {
          पक्ष 0: // both first; drop the smaller
            यदि (a < b) { // drop a
                a = list[i++];
                polarity ^= 1;
            } अन्यथा यदि (b < a) { // drop b
                b = other[j++];
                polarity ^= 2;
            } अन्यथा { // a == b, take one, drop other
                यदि (a == UNICODESET_उच्चः) गच्छ loop_अन्तः;
                बफर[k++] = a;
                a = list[i++];
                polarity ^= 1;
                b = other[j++];
                polarity ^= 2;
            }
            भङ्ग;
          पक्ष 3: // both second; take lower if unequal
            यदि (a < b) { // take a
                बफर[k++] = a;
                a = list[i++];
                polarity ^= 1;
            } अन्यथा यदि (b < a) { // take b
                बफर[k++] = b;
                b = other[j++];
                polarity ^= 2;
            } अन्यथा { // a == b, take one, drop other
                यदि (a == UNICODESET_उच्चः) गच्छ loop_अन्तः;
                बफर[k++] = a;
                a = list[i++];
                polarity ^= 1;
                b = other[j++];
                polarity ^= 2;
            }
            भङ्ग;
          पक्ष 1: // a second, b first;
            यदि (a < b) { // NO OVERLAP, drop a
                a = list[i++];
                polarity ^= 1;
            } अन्यथा यदि (b < a) { // OVERLAP, take b
                बफर[k++] = b;
                b = other[j++];
                polarity ^= 2;
            } अन्यथा { // a == b, drop both!
                यदि (a == UNICODESET_उच्चः) गच्छ loop_अन्तः;
                a = list[i++];
                polarity ^= 1;
                b = other[j++];
                polarity ^= 2;
            }
            भङ्ग;
          पक्ष 2: // a first, b second; if a < b, overlap
            यदि (b < a) { // no overlap, drop b
                b = other[j++];
                polarity ^= 2;
            } अन्यथा  यदि (a < b) { // OVERLAP, take a
                बफर[k++] = a;
                a = list[i++];
                polarity ^= 1;
            } अन्यथा { // a == b, drop both!
                यदि (a == UNICODESET_उच्चः) गच्छ loop_अन्तः;
                a = list[i++];
                polarity ^= 1;
                b = other[j++];
                polarity ^= 2;
            }
            भङ्ग;
        }
    }
 loop_अन्तः:
    बफर[k++] = UNICODESET_उच्चः;    // terminate
    len = k;
    swapBuffers();
    release_आदर्शः();
}

/**
 * Append the <code>toPattern()</code> representation of a
 * string to the given <code>StringBuffer</code>.
 */
शून्य Unicode_समुच्चयः::append_गन्तव्यम्_Pat(UnicodeString& buf, ध्रुव UnicodeString& s, UBool escapeUnprintable) {
    UChar32 cp;
    प्रति (int32_t i = 0; i < s.length(); i += U16_दीर्घता(cp)) {
        append_गन्तव्यम्_Pat(buf, cp = s.char32At(i), escapeUnprintable);
    }
}

/**
 * Append the <code>toPattern()</code> representation of a
 * character to the given <code>StringBuffer</code>.
 */
शून्य Unicode_समुच्चयः::append_गन्तव्यम्_Pat(UnicodeString& buf, UChar32 c, UBool escapeUnprintable) {
    यदि (escapeUnprintable ? ICU_Utility::isUnprintable(c) : ICU_Utility::shouldAlwaysBeEscaped(c)) {
        // Use hex escape notation (\uxxxx or \Uxxxxxxxx) for anything
        // unprintable
        ICU_Utility::escape(buf, c);
        प्रत्यागम;
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
    पक्ष चिह्नम्_सारणी::चिह्नम्_REF:
        buf.append(u'\\');
        भङ्ग;
    स्वभाव:
        // Escape whitespace
        यदि (आदर्शः_Props::isWhiteSpace(c)) {
            buf.append(u'\\');
        }
        भङ्ग;
    }
    buf.append(c);
}

शून्य Unicode_समुच्चयः::append_गन्तव्यम्_Pat(UnicodeString &परिणाम, UChar32 start, UChar32 अन्तः,
                              UBool escapeUnprintable) {
    append_गन्तव्यम्_Pat(परिणाम, start, escapeUnprintable);
    यदि (start != अन्तः) {
        यदि ((start+1) != अन्तः ||
                // Avoid writing what looks like a lead+trail surrogate pair.
                start == 0xdbff) {
            परिणाम.append(u'-');
        }
        append_गन्तव्यम्_Pat(परिणाम, अन्तः, escapeUnprintable);
    }
}

/**
 * Append a string representation of this set to result.  This will be
 * a cleaned version of the string passed to applyPattern(), if there
 * is one.  Otherwise it will be generated.
 */
UnicodeString& Unicode_समुच्चयः::गन्तव्यम्_आदर्शः_2(UnicodeString& परिणाम U_LIFETIME_BOUND,
                                      UBool escapeUnprintable) ध्रुव
{
    यदि (pat != शून्यसूचकः) {
        int32_t i;
        int32_t backslash_गणनम् = 0;
        प्रति (i=0; i<patLen; ) {
            UChar32 c;
            U16_अग्र(pat, i, patLen, c);
            यदि (escapeUnprintable ?
                    ICU_Utility::isUnprintable(c) : ICU_Utility::shouldAlwaysBeEscaped(c)) {
                // If the unprintable character is preceded by an odd
                // number of backslashes, then it has been escaped.
                // Before unescaping it, we delete the final
                // backslash.
                यदि ((backslash_गणनम् % 2) == 1) {
                    परिणाम.truncate(परिणाम.length() - 1);
                }
                ICU_Utility::escape(परिणाम, c);
                backslash_गणनम् = 0;
            } अन्यथा {
                परिणाम.append(c);
                यदि (c == u'\\') {
                    ++backslash_गणनम्;
                } अन्यथा {
                    backslash_गणनम् = 0;
                }
            }
        }
        प्रत्यागम परिणाम;
    }

    प्रत्यागम generate_आदर्शः(परिणाम, escapeUnprintable);
}

/**
 * Returns a string representation of this set.  If the result of
 * calling this function is passed to a UnicodeSet constructor, it
 * will produce another set that is equal to this one.
 */
UnicodeString& Unicode_समुच्चयः::गन्तव्यम्_आदर्शः(UnicodeString& परिणाम U_LIFETIME_BOUND,
                                     UBool escapeUnprintable) ध्रुव
{
    परिणाम.truncate(0);
    प्रत्यागम गन्तव्यम्_आदर्शः_2(परिणाम, escapeUnprintable);
}

/**
 * Generate and append a string representation of this set to result.
 * This does not use this.pat, the cleaned up copy of the string
 * passed to applyPattern().
 */
UnicodeString& Unicode_समुच्चयः::generate_आदर्शः(UnicodeString& परिणाम U_LIFETIME_BOUND,
                                            UBool escapeUnprintable) ध्रुव
{
    परिणाम.append(u'[');

    int32_t i = 0;
    int32_t सीमा = len & ~1;  // = 2 * getRangeCount()

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
    यदि (len >= 4 && list[0] == 0 && सीमा == len && !has_वाक्यानि()) {
        // Emit the inverse
        परिणाम.append(u'^');
        // Offsetting the inversion list index by one lets us
        // iterate over the ranges of the set complement.
        i = 1;
        --सीमा;
    }

    // Emit the ranges as pairs.
    यावत् (i < सीमा) {
        UChar32 start = list[i];  // getRangeStart()
        UChar32 अन्तः = list[i + 1] - 1;  // getRangeEnd() = range limit minus one
        यदि (!(0xd800 <= अन्तः && अन्तः <= 0xdbff)) {
            append_गन्तव्यम्_Pat(परिणाम, start, अन्तः, escapeUnprintable);
            i += 2;
        } अन्यथा {
            // The range ends with a lead surrogate.
            // Avoid writing what looks like a lead+trail surrogate pair.
            // 1. Postpone ranges that start with a lead surrogate code point.
            int32_t firstLead = i;
            यावत् ((i += 2) < सीमा && list[i] <= 0xdbff) {}
            int32_t firstAfterLead = i;
            // 2. Write following ranges that start with a trail surrogate code point.
            यावत् (i < सीमा && (start = list[i]) <= 0xdfff) {
                append_गन्तव्यम्_Pat(परिणाम, start, list[i + 1] - 1, escapeUnprintable);
                i += 2;
            }
            // 3. Now write the postponed ranges.
            प्रति (पूर्णाङ्क j = firstLead; j < firstAfterLead; j += 2) {
                append_गन्तव्यम्_Pat(परिणाम, list[j], list[j + 1] - 1, escapeUnprintable);
            }
        }
    }

    यदि (वाक्यानि_2 != शून्यसूचकः) {
        प्रति (int32_t i = 0; i<वाक्यानि_2->size(); ++i) {
            परिणाम.append(u'{');
            append_गन्तव्यम्_Pat(परिणाम,
                         *static_cast<ध्रुव UnicodeString*>(वाक्यानि_2->elementAt(i)),
                         escapeUnprintable);
            परिणाम.append(u'}');
        }
    }
    प्रत्यागम परिणाम.append(u']');
}

/**
* Release existing cached pattern
*/
शून्य Unicode_समुच्चयः::release_आदर्शः() {
    यदि (pat) {
        uprv_free(pat);
        pat = शून्यसूचकः;
        patLen = 0;
    }
}

/**
* Set the new pattern to cache.
*/
शून्य Unicode_समुच्चयः::समुच्चयः_आदर्शः(ध्रुव char16_t *newPat, int32_t newPatLen) {
    release_आदर्शः();
    pat = static_cast<char16_t*>(uprv_malloc((newPatLen + 1) * परिमाण(char16_t)));
    यदि (pat) {
        patLen = newPatLen;
        u_memcpy(pat, newPat, patLen);
        pat[patLen] = 0;
    }
    // else we don't care if malloc failed. This was just a nice cache.
    // We can regenerate an equivalent pattern later when requested.
}

Unicode_समुच्चयः* Unicode_समुच्चयः::freeze() U_LIFETIME_BOUND {
    यदि(!isFrozen() && !isBogus()) {
        compact();

        // Optimize contains() and span() and similar functions.
        यदि (has_वाक्यानि()) {
            string_विस्तृतिः = नूतन Unicode_समुच्चयः_String_विस्तृतिः(*एतत्, *वाक्यानि_2, Unicode_समुच्चयः_String_विस्तृतिः::ALL);
            यदि (string_विस्तृतिः == शून्यसूचकः) {
                समुच्चयः_गन्तव्यम्_Bogus();
                प्रत्यागम एतत्;
            } अन्यथा यदि (!string_विस्तृतिः->needs_String_विस्तृतिः_UTF16()) {
                // All strings are irrelevant for span() etc. because
                // all of each string's code points are contained in this set.
                // Do not check needsStringSpanUTF8() because UTF-8 has at most as
                // many relevant strings as UTF-16.
                // (Thus needsStringSpanUTF8() implies needsStringSpanUTF16().)
                विलोपय string_विस्तृतिः;
                string_विस्तृतिः = शून्यसूचकः;
            }
        }
        यदि (string_विस्तृतिः == शून्यसूचकः) {
            // No span-relevant strings: Optimize for code point spans.
            bmp_समुच्चयः=नूतन BMPSet(list, len);
            यदि (bmp_समुच्चयः == शून्यसूचकः) { // Check for memory allocation error.
                समुच्चयः_गन्तव्यम्_Bogus();
            }
        }
    }
    प्रत्यागम एतत्;
}

int32_t Unicode_समुच्चयः::विस्तृतिः(ध्रुव char16_t *s, int32_t length, USet_विस्तृतिः_Condition विस्तृतिः_Condition) ध्रुव {
    यदि(length>0 && bmp_समुच्चयः!=शून्यसूचकः) {
        प्रत्यागम static_cast<int32_t>(bmp_समुच्चयः->विस्तृतिः(s, s + length, विस्तृतिः_Condition) - s);
    }
    यदि(length<0) {
        length=u_strlen(s);
    }
    यदि(length==0) {
        प्रत्यागम 0;
    }
    यदि(string_विस्तृतिः!=शून्यसूचकः) {
        प्रत्यागम string_विस्तृतिः->विस्तृतिः(s, length, विस्तृतिः_Condition);
    } अन्यथा यदि(has_वाक्यानि()) {
        uint32_t which= विस्तृतिः_Condition==USET_विस्तृतिः_NOT_CONTAINED ?
                            Unicode_समुच्चयः_String_विस्तृतिः::FWD_UTF16_NOT_CONTAINED :
                            Unicode_समुच्चयः_String_विस्तृतिः::FWD_UTF16_CONTAINED;
        Unicode_समुच्चयः_String_विस्तृतिः str_विस्तृतिः(*एतत्, *वाक्यानि_2, which);
        यदि(str_विस्तृतिः.needs_String_विस्तृतिः_UTF16()) {
            प्रत्यागम str_विस्तृतिः.विस्तृतिः(s, length, विस्तृतिः_Condition);
        }
    }

    यदि(विस्तृतिः_Condition!=USET_विस्तृतिः_NOT_CONTAINED) {
        विस्तृतिः_Condition=USET_विस्तृतिः_CONTAINED;  // Pin to 0/1 values.
    }

    UChar32 c;
    int32_t start=0, पूर्व=0;
    कुरु {
        U16_अग्र(s, start, length, c);
        यदि(विस्तृतिः_Condition!=अन्तर्भवति(c)) {
            भङ्ग;
        }
    } यावत्((पूर्व=start)<length);
    प्रत्यागम पूर्व;
}

int32_t Unicode_समुच्चयः::विस्तृतिः_पृष्ठभागः(ध्रुव char16_t *s, int32_t length, USet_विस्तृतिः_Condition विस्तृतिः_Condition) ध्रुव {
    यदि(length>0 && bmp_समुच्चयः!=शून्यसूचकः) {
        प्रत्यागम static_cast<int32_t>(bmp_समुच्चयः->विस्तृतिः_पृष्ठभागः(s, s + length, विस्तृतिः_Condition) - s);
    }
    यदि(length<0) {
        length=u_strlen(s);
    }
    यदि(length==0) {
        प्रत्यागम 0;
    }
    यदि(string_विस्तृतिः!=शून्यसूचकः) {
        प्रत्यागम string_विस्तृतिः->विस्तृतिः_पृष्ठभागः(s, length, विस्तृतिः_Condition);
    } अन्यथा यदि(has_वाक्यानि()) {
        uint32_t which= विस्तृतिः_Condition==USET_विस्तृतिः_NOT_CONTAINED ?
                            Unicode_समुच्चयः_String_विस्तृतिः::पृष्ठभागः_UTF16_NOT_CONTAINED :
                            Unicode_समुच्चयः_String_विस्तृतिः::पृष्ठभागः_UTF16_CONTAINED;
        Unicode_समुच्चयः_String_विस्तृतिः str_विस्तृतिः(*एतत्, *वाक्यानि_2, which);
        यदि(str_विस्तृतिः.needs_String_विस्तृतिः_UTF16()) {
            प्रत्यागम str_विस्तृतिः.विस्तृतिः_पृष्ठभागः(s, length, विस्तृतिः_Condition);
        }
    }

    यदि(विस्तृतिः_Condition!=USET_विस्तृतिः_NOT_CONTAINED) {
        विस्तृतिः_Condition=USET_विस्तृतिः_CONTAINED;  // Pin to 0/1 values.
    }

    UChar32 c;
    int32_t पूर्व=length;
    कुरु {
        U16_पूर्व(s, 0, length, c);
        यदि(विस्तृतिः_Condition!=अन्तर्भवति(c)) {
            भङ्ग;
        }
    } यावत्((पूर्व=length)>0);
    प्रत्यागम पूर्व;
}

int32_t Unicode_समुच्चयः::विस्तृतिः_UTF8(ध्रुव वर्ण *s, int32_t length, USet_विस्तृतिः_Condition विस्तृतिः_Condition) ध्रुव {
    यदि(length>0 && bmp_समुच्चयः!=शून्यसूचकः) {
        ध्रुव uint8_t* s0 = reinterpret_cast<ध्रुव uint8_t*>(s);
        प्रत्यागम static_cast<int32_t>(bmp_समुच्चयः->विस्तृतिः_UTF8(s0, length, विस्तृतिः_Condition) - s0);
    }
    यदि(length<0) {
        length = static_cast<int32_t>(uprv_strlen(s));
    }
    यदि(length==0) {
        प्रत्यागम 0;
    }
    यदि(string_विस्तृतिः!=शून्यसूचकः) {
        प्रत्यागम string_विस्तृतिः->विस्तृतिः_UTF8(reinterpret_cast<ध्रुव uint8_t*>(s), length, विस्तृतिः_Condition);
    } अन्यथा यदि(has_वाक्यानि()) {
        uint32_t which= विस्तृतिः_Condition==USET_विस्तृतिः_NOT_CONTAINED ?
                            Unicode_समुच्चयः_String_विस्तृतिः::FWD_UTF8_NOT_CONTAINED :
                            Unicode_समुच्चयः_String_विस्तृतिः::FWD_UTF8_CONTAINED;
        Unicode_समुच्चयः_String_विस्तृतिः str_विस्तृतिः(*एतत्, *वाक्यानि_2, which);
        यदि(str_विस्तृतिः.needs_String_विस्तृतिः_UTF8()) {
            प्रत्यागम str_विस्तृतिः.विस्तृतिः_UTF8(reinterpret_cast<ध्रुव uint8_t*>(s), length, विस्तृतिः_Condition);
        }
    }

    यदि(विस्तृतिः_Condition!=USET_विस्तृतिः_NOT_CONTAINED) {
        विस्तृतिः_Condition=USET_विस्तृतिः_CONTAINED;  // Pin to 0/1 values.
    }

    UChar32 c;
    int32_t start=0, पूर्व=0;
    कुरु {
        U8_अग्र_OR_FFFD(s, start, length, c);
        यदि(विस्तृतिः_Condition!=अन्तर्भवति(c)) {
            भङ्ग;
        }
    } यावत्((पूर्व=start)<length);
    प्रत्यागम पूर्व;
}

int32_t Unicode_समुच्चयः::विस्तृतिः_पृष्ठभागः_UTF8(ध्रुव वर्ण *s, int32_t length, USet_विस्तृतिः_Condition विस्तृतिः_Condition) ध्रुव {
    यदि(length>0 && bmp_समुच्चयः!=शून्यसूचकः) {
        ध्रुव uint8_t* s0 = reinterpret_cast<ध्रुव uint8_t*>(s);
        प्रत्यागम bmp_समुच्चयः->विस्तृतिः_पृष्ठभागः_UTF8(s0, length, विस्तृतिः_Condition);
    }
    यदि(length<0) {
        length = static_cast<int32_t>(uprv_strlen(s));
    }
    यदि(length==0) {
        प्रत्यागम 0;
    }
    यदि(string_विस्तृतिः!=शून्यसूचकः) {
        प्रत्यागम string_विस्तृतिः->विस्तृतिः_पृष्ठभागः_UTF8(reinterpret_cast<ध्रुव uint8_t*>(s), length, विस्तृतिः_Condition);
    } अन्यथा यदि(has_वाक्यानि()) {
        uint32_t which= विस्तृतिः_Condition==USET_विस्तृतिः_NOT_CONTAINED ?
                            Unicode_समुच्चयः_String_विस्तृतिः::पृष्ठभागः_UTF8_NOT_CONTAINED :
                            Unicode_समुच्चयः_String_विस्तृतिः::पृष्ठभागः_UTF8_CONTAINED;
        Unicode_समुच्चयः_String_विस्तृतिः str_विस्तृतिः(*एतत्, *वाक्यानि_2, which);
        यदि(str_विस्तृतिः.needs_String_विस्तृतिः_UTF8()) {
            प्रत्यागम str_विस्तृतिः.विस्तृतिः_पृष्ठभागः_UTF8(reinterpret_cast<ध्रुव uint8_t*>(s), length, विस्तृतिः_Condition);
        }
    }

    यदि(विस्तृतिः_Condition!=USET_विस्तृतिः_NOT_CONTAINED) {
        विस्तृतिः_Condition=USET_विस्तृतिः_CONTAINED;  // Pin to 0/1 values.
    }

    UChar32 c;
    int32_t पूर्व=length;
    कुरु {
        U8_पूर्व_OR_FFFD(s, 0, length, c);
        यदि(विस्तृतिः_Condition!=अन्तर्भवति(c)) {
            भङ्ग;
        }
    } यावत्((पूर्व=length)>0);
    प्रत्यागम पूर्व;
}

U_NAMESPACE_अन्तः
