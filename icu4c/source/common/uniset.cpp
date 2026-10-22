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
#وصف_ڪريو UNICODESET_مٿاهون 0x0110000

// LOW <= all valid values. ZERO for codepoints
#وصف_ڪريو UNICODESET_هيٺيون 0x000000

/** Max list [0, 1, 2, ..., max code point, HIGH] */
constexpr int32_t MAX_ڊيگهائي = UNICODESET_مٿاهون + 1;

U_NAMESPACE_BEGIN

نشان_جدول::~نشان_جدول() {}

UOBJECT_DEFINE_RTTI_IMPLEMENTATION(Unicode_سيٽ)

/**
 * Modify the given UChar32 variable so that it is in range, by
 * pinning values < UNICODESET_LOW to UNICODESET_LOW, and
 * pinning values > UNICODESET_HIGH-1 to UNICODESET_HIGH-1.
 * It modifies its argument in-place and also returns it.
 */
ساكن مضمن UChar32 pin_ڪوڊ_نقطو(UChar32& c) {
    إذا (c < UNICODESET_هيٺيون) {
        c = UNICODESET_هيٺيون;
    } وإلا إذا (c > (UNICODESET_مٿاهون-1)) {
        c = (UNICODESET_مٿاهون-1);
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

#جيڪڏهن_وصف_ٿيل DEBUG_MEM
#ضمن <stdio.h>
ساكن int32_t dbg_ڳڻپ = 0;

ساكن مضمن فراغ _dbgct(Unicode_سيٽ* سيٽ) {
    UnicodeString str;
    سيٽ->منزل_نمونو(str, سچ);
    حرف buf[40];
    str.extract(0, 39, buf, "");
    لکو("DEBUG UnicodeSet: ct 0x%08X; %d %s\n", سيٽ, ++dbg_ڳڻپ, buf);
}

ساكن مضمن فراغ _dbgdt(Unicode_سيٽ* سيٽ) {
    UnicodeString str;
    سيٽ->منزل_نمونو(str, سچ);
    حرف buf[40];
    str.extract(0, 39, buf, "");
    لکو("DEBUG UnicodeSet: dt 0x%08X; %d %s\n", سيٽ, --dbg_ڳڻپ, buf);
}

#وإلا

#وصف_ڪريو _dbgct(سيٽ)
#وصف_ڪريو _dbgdt(سيٽ)

#جيڪڏهن_پورو

//----------------------------------------------------------------
// UnicodeString in UVector support
//----------------------------------------------------------------

ساكن فراغ U_CALLCONV cloneUnicodeString(UElement *dst, UElement *src) {
    dst->اشارو = جديد UnicodeString(*static_cast<UnicodeString*>(src->اشارو));
}

ساكن int32_t U_CALLCONV compareUnicodeString(UElement t1, UElement t2) {
    ثابت UnicodeString& a = *static_cast<ثابت UnicodeString*>(t1.اشارو);
    ثابت UnicodeString& b = *static_cast<ثابت UnicodeString*>(t2.اشارو);
    أرجع a.compare(b);
}

UBool Unicode_سيٽ::has_لڙيون() ثابت {
    أرجع لڙيون_2 != خالي_اشارو && !لڙيون_2->is_خالي_جايون();
}

int32_t Unicode_سيٽ::لڙيون_سائيز() ثابت {
    أرجع لڙيون_2 == خالي_اشارو ? 0 : لڙيون_2->size();
}

UBool Unicode_سيٽ::لڙيون_شامل_آهي(ثابت UnicodeString &s) ثابت {
    أرجع لڙيون_2 != خالي_اشارو && لڙيون_2->شامل_آهي((فراغ*) &s);
}

//----------------------------------------------------------------
// Constructors &c
//----------------------------------------------------------------

/**
 * Constructs an empty set.
 */
Unicode_سيٽ::Unicode_سيٽ() {
    list[0] = UNICODESET_مٿاهون;
    _dbgct(هذا);
}

/**
 * Constructs a set containing the given range. If <code>end >
 * start</code> then an empty set is created.
 *
 * @param start first character, inclusive, of range
 * @param end last character, inclusive, of range
 */
Unicode_سيٽ::Unicode_سيٽ(UChar32 شروعات, UChar32 پڇاڙي) {
    list[0] = UNICODESET_مٿاهون;
    جوڙيو(شروعات, پڇاڙي);
    _dbgct(هذا);
}

/**
 * Constructs a set that is identical to the given UnicodeSet.
 */
Unicode_سيٽ::Unicode_سيٽ(ثابت Unicode_سيٽ& o) : UnicodeFilter(o) {
    *هذا = o;
    _dbgct(هذا);
}

// Copy-construct as thawed.
Unicode_سيٽ::Unicode_سيٽ(ثابت Unicode_سيٽ& o, UBool /* asThawed */) : UnicodeFilter(o) {
    إذا (ensure_صلاحيت(o.len)) {
        // *this = o except for bmpSet and stringSpan
        len = o.len;
        uprv_memcpy(list, o.list, (size_t)len*حجم(UChar32));
        إذا (o.has_لڙيون()) {
            UError_ڪوڊ حالت = U_ZERO_ERROR;
            إذا (!allocate_لڙيون(حالت) ||
                    (لڙيون_2->assign(*o.لڙيون_2, cloneUnicodeString, حالت), U_FAILURE(حالت))) {
                سيٽ_منزل_Bogus();
                أرجع;
            }
        }
        إذا (o.pat) {
            سيٽ_نمونو(o.pat, o.patLen);
        }
        _dbgct(هذا);
    }
}

/**
 * Destructs the set.
 */
Unicode_سيٽ::~Unicode_سيٽ() {
    _dbgdt(هذا); // first!
    إذا (list != ڍير_List) {
        uprv_free(list);
    }
    ميٽيو bmp_سيٽ;
    إذا (بفر != ڍير_List) {
        uprv_free(بفر);
    }
    ميٽيو لڙيون_2;
    ميٽيو string_وسعت;
    release_نمونو();
}

/**
 * Assigns this object to be a copy of another.
 */
Unicode_سيٽ& Unicode_سيٽ::عامل=(ثابت Unicode_سيٽ& o) U_LIFETIME_BOUND {
    أرجع copyFrom(o, خطأ);
}

Unicode_سيٽ& Unicode_سيٽ::copyFrom(ثابت Unicode_سيٽ& o, UBool asThawed) U_LIFETIME_BOUND {
    إذا (هذا == &o) {
        أرجع *هذا;
    }
    إذا (isFrozen()) {
        أرجع *هذا;
    }
    إذا (o.isBogus()) {
        سيٽ_منزل_Bogus();
        أرجع *هذا;
    }
    إذا (!ensure_صلاحيت(o.len)) {
        // ensureCapacity will mark the UnicodeSet as Bogus if OOM failure happens.
        أرجع *هذا;
    }
    len = o.len;
    uprv_memcpy(list, o.list, (size_t)len*حجم(UChar32));
    إذا (o.bmp_سيٽ != خالي_اشارو && !asThawed) {
        bmp_سيٽ = جديد BMPSet(*o.bmp_سيٽ, list, len);
        إذا (bmp_سيٽ == خالي_اشارو) { // Check for memory allocation error.
            سيٽ_منزل_Bogus();
            أرجع *هذا;
        }
    }
    إذا (o.has_لڙيون()) {
        UError_ڪوڊ حالت = U_ZERO_ERROR;
        إذا ((لڙيون_2 == خالي_اشارو && !allocate_لڙيون(حالت)) ||
                (لڙيون_2->assign(*o.لڙيون_2, cloneUnicodeString, حالت), U_FAILURE(حالت))) {
            سيٽ_منزل_Bogus();
            أرجع *هذا;
        }
    } وإلا إذا (has_لڙيون()) {
        لڙيون_2->removeAllElements();
    }
    إذا (o.string_وسعت != خالي_اشارو && لڙيون_2 != خالي_اشارو && !asThawed) {
        string_وسعت = جديد Unicode_سيٽ_String_وسعت(*o.string_وسعت, *لڙيون_2);
        إذا (string_وسعت == خالي_اشارو) { // Check for memory allocation error.
            سيٽ_منزل_Bogus();
            أرجع *هذا;
        }
    }
    release_نمونو();
    إذا (o.pat) {
        سيٽ_نمونو(o.pat, o.patLen);
    }
    أرجع *هذا;
}

/**
 * Returns a copy of this object.  All UnicodeMatcher objects have
 * to support cloning in order to allow classes using
 * UnicodeMatchers, such as Transliterator, to implement cloning.
 */
Unicode_سيٽ* Unicode_سيٽ::clone() ثابت {
    أرجع جديد Unicode_سيٽ(*هذا);
}

Unicode_سيٽ *Unicode_سيٽ::cloneAsThawed() ثابت {
    أرجع جديد Unicode_سيٽ(*هذا, سچ);
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
منطقي Unicode_سيٽ::عامل==(ثابت Unicode_سيٽ& o) ثابت {
    إذا (len != o.len) أرجع خطأ;
    لكل (int32_t i = 0; i < len; ++i) {
        إذا (list[i] != o.list[i]) أرجع خطأ;
    }
    إذا (has_لڙيون() != o.has_لڙيون()) { أرجع خطأ; }
    إذا (has_لڙيون() && *لڙيون_2 != *o.لڙيون_2) أرجع خطأ;
    أرجع سچ;
}

/**
 * Returns the hash code value for this set.
 *
 * @return the hash code value for this set.
 * @see Object#hashCode()
 */
int32_t Unicode_سيٽ::hash_ڪوڊ() ثابت {
    uint32_t نتيجو = static_cast<uint32_t>(len);
    لكل (int32_t i = 0; i < len; ++i) {
        نتيجو *= 1000003u;
        نتيجو += list[i];
    }
    أرجع static_cast<int32_t>(نتيجو);
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
int32_t Unicode_سيٽ::size() ثابت {
    int32_t n = 0;
    int32_t count = get_Range_ڳڻپ();
    لكل (int32_t i = 0; i < count; ++i) {
        n += get_Range_پڇاڙي(i) - get_Range_شروعات(i) + 1;
    }
    أرجع n + لڙيون_سائيز();
}

/**
 * Returns <tt>true</tt> if this set contains no elements.
 *
 * @return <tt>true</tt> if this set contains no elements.
 */
UBool Unicode_سيٽ::is_خالي_جايون() ثابت {
    أرجع len == 1 && !has_لڙيون();
}

/**
 * Returns true if this set contains the given character.
 * @param c character to be checked for containment
 * @return true if the test condition is met
 */
UBool Unicode_سيٽ::شامل_آهي(UChar32 c) ثابت {
    // Set i to the index of the start item greater than ch
    // We know we will terminate without length test!
    // LATER: for large sets, add binary search
    //int32_t i = -1;
    //for (;;) {
    //    if (c < list[++i]) break;
    //}
    إذا (bmp_سيٽ != خالي_اشارو) {
        أرجع bmp_سيٽ->شامل_آهي(c);
    }
    إذا (string_وسعت != خالي_اشارو) {
        أرجع string_وسعت->شامل_آهي(c);
    }
    إذا (c >= UNICODESET_مٿاهون) { // Don't need to check LOW bound
        أرجع خطأ;
    }
    int32_t i = ڳوليو_ڪوڊ_نقطو(c);
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
int32_t Unicode_سيٽ::ڳوليو_ڪوڊ_نقطو(UChar32 c) ثابت {
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
UBool Unicode_سيٽ::شامل_آهي(UChar32 شروعات, UChar32 پڇاڙي) ثابت {
    //int32_t i = -1;
    //for (;;) {
    //    if (start < list[++i]) break;
    //}
    int32_t i = ڳوليو_ڪوڊ_نقطو(شروعات);
    أرجع ((i & 1) != 0 && پڇاڙي < list[i]);
}

/**
 * Returns <tt>true</tt> if this set contains the given
 * multicharacter string.
 * @param s string to be checked for containment
 * @return <tt>true</tt> if this set contains the specified string
 */
UBool Unicode_سيٽ::شامل_آهي(ثابت UnicodeString& s) ثابت {
    int32_t cp = getSingleCP(s);
    إذا (cp < 0) {
        أرجع لڙيون_شامل_آهي(s);
    } وإلا {
        أرجع شامل_آهي(static_cast<UChar32>(cp));
    }
}

/**
 * Returns true if this set contains all the characters and strings
 * of the given set.
 * @param c set to be checked for containment
 * @return true if the test condition is met
 */
UBool Unicode_سيٽ::شامل_آهي_All(ثابت Unicode_سيٽ& c) ثابت {
    // The specified set is a subset if all of its pairs are contained in
    // this set.  It's possible to code this more efficiently in terms of
    // direct manipulation of the inversion lists if the need arises.
    int32_t n = c.get_Range_ڳڻپ();
    لكل (صحيح i=0; i<n; ++i) {
        إذا (!شامل_آهي(c.get_Range_شروعات(i), c.get_Range_پڇاڙي(i))) {
            أرجع خطأ;
        }
    }
    أرجع !c.has_لڙيون() || (لڙيون_2 != خالي_اشارو && لڙيون_2->شامل_آهي_All(*c.لڙيون_2));
}

/**
 * Returns true if this set contains all the characters
 * of the given string.
 * @param s string containing characters to be checked for containment
 * @return true if the test condition is met
 */
UBool Unicode_سيٽ::شامل_آهي_All(ثابت UnicodeString& s) ثابت {
    أرجع وسعت(s.get_بفر(), s.length(), USET_وسعت_CONTAINED) == s.length();
}

/**
 * Returns true if this set contains none of the characters
 * of the given range.
 * @param start first character, inclusive, of the range
 * @param end last character, inclusive, of the range
 * @return true if the test condition is met
 */
UBool Unicode_سيٽ::شامل_آهي_None(UChar32 شروعات, UChar32 پڇاڙي) ثابت {
    //int32_t i = -1;
    //for (;;) {
    //    if (start < list[++i]) break;
    //}
    int32_t i = ڳوليو_ڪوڊ_نقطو(شروعات);
    أرجع ((i & 1) == 0 && پڇاڙي < list[i]);
}

/**
 * Returns true if this set contains none of the characters and strings
 * of the given set.
 * @param c set to be checked for containment
 * @return true if the test condition is met
 */
UBool Unicode_سيٽ::شامل_آهي_None(ثابت Unicode_سيٽ& c) ثابت {
    // The specified set is a subset if all of its pairs are contained in
    // this set.  It's possible to code this more efficiently in terms of
    // direct manipulation of the inversion lists if the need arises.
    int32_t n = c.get_Range_ڳڻپ();
    لكل (int32_t i=0; i<n; ++i) {
        إذا (!شامل_آهي_None(c.get_Range_شروعات(i), c.get_Range_پڇاڙي(i))) {
            أرجع خطأ;
        }
    }
    أرجع لڙيون_2 == خالي_اشارو || !c.has_لڙيون() || لڙيون_2->شامل_آهي_None(*c.لڙيون_2);
}

/**
 * Returns true if this set contains none of the characters
 * of the given string.
 * @param s string containing characters to be checked for containment
 * @return true if the test condition is met
 */
UBool Unicode_سيٽ::شامل_آهي_None(ثابت UnicodeString& s) ثابت {
    أرجع وسعت(s.get_بفر(), s.length(), USET_وسعت_NOT_CONTAINED) == s.length();
}

/**
 * Returns <tt>true</tt> if this set contains any character whose low byte
 * is the given value.  This is used by <tt>RuleBasedTransliterator</tt> for
 * indexing.
 */
UBool Unicode_سيٽ::matches_اشاريو_قدر(uint8_t v) ثابت {
    /* The index value v, in the range [0,255], is contained in this set if
     * it is contained in any pair of this set.  Pairs either have the high
     * bytes equal, or unequal.  If the high bytes are equal, then we have
     * aaxx..aayy, where aa is the high byte.  Then v is contained if xx <=
     * v <= yy.  If the high bytes are unequal we have aaxx..bbyy, bb>aa.
     * Then v is contained if xx <= v || v <= yy.  (This is identical to the
     * time zone month containment logic.)
     */
    int32_t i;
    int32_t range_ڳڻپ=get_Range_ڳڻپ();
    لكل (i=0; i<range_ڳڻپ; ++i) {
        UChar32 هيٺيون = get_Range_شروعات(i);
        UChar32 مٿاهون = get_Range_پڇاڙي(i);
        إذا ((هيٺيون & ~0xFF) == (مٿاهون & ~0xFF)) {
            إذا ((هيٺيون & 0xFF) <= v && v <= (مٿاهون & 0xFF)) {
                أرجع سچ;
            }
        } وإلا إذا ((هيٺيون & 0xFF) <= v || v <= (مٿاهون & 0xFF)) {
            أرجع سچ;
        }
    }
    إذا (has_لڙيون()) {
        لكل (i=0; i<لڙيون_2->size(); ++i) {
            ثابت UnicodeString& s = *static_cast<ثابت UnicodeString*>(لڙيون_2->elementAt(i));
            إذا (s.is_خالي_جايون()) {
                استمر;  // skip the empty string
            }
            UChar32 c = s.char32At(0);
            إذا ((c & 0xFF) == v) {
                أرجع سچ;
            }
        }
    }
    أرجع خطأ;
}

/**
 * Implementation of UnicodeMatcher::matches().  Always matches the
 * longest possible multichar string.
 */
UMatchDegree Unicode_سيٽ::matches(ثابت Replaceable& متن,
                                 int32_t& وٿي,
                                 int32_t حد_بندي,
                                 UBool incremental) {
    إذا (وٿي == حد_بندي) {
        إذا (شامل_آهي(U_ETHER)) {
            أرجع incremental ? U_PARTIAL_ميلاپ : U_ميلاپ;
        } وإلا {
            أرجع U_MISMATCH;
        }
    } وإلا {
        إذا (has_لڙيون()) { // try strings first

            // might separate forward and backward loops later
            // for now they are combined

            // TODO Improve efficiency of this, at least in the forward
            // direction, if not in both.  In the forward direction we
            // can assume the strings are sorted.

            int32_t i;
            UBool forward = وٿي < حد_بندي;

            // firstChar is the leftmost char to match in the
            // forward direction or the rightmost char to match in
            // the reverse direction.
            char16_t پهريون_Char = متن.charAt(وٿي);

            // If there are multiple strings that can match we
            // return the longest match.
            int32_t مٿاهون_Water_ڊيگهائي = 0;

            لكل (i=0; i<لڙيون_2->size(); ++i) {
                ثابت UnicodeString& trial = *static_cast<ثابت UnicodeString*>(لڙيون_2->elementAt(i));
                إذا (trial.is_خالي_جايون()) {
                    استمر;  // skip the empty string
                }

                char16_t c = trial.charAt(forward ? 0 : trial.length() - 1);

                // Strings are sorted, so we can optimize in the
                // forward direction.
                إذا (forward && c > پهريون_Char) اكسر;
                إذا (c != پهريون_Char) استمر;

                int32_t ميلاپ_Len = ميلاپ_Rest(متن, وٿي, حد_بندي, trial);

                إذا (incremental) {
                    int32_t maxLen = forward ? حد_بندي-وٿي : وٿي-حد_بندي;
                    إذا (ميلاپ_Len == maxLen) {
                        // We have successfully matched but only up to limit.
                        أرجع U_PARTIAL_ميلاپ;
                    }
                }

                إذا (ميلاپ_Len == trial.length()) {
                    // We have successfully matched the whole string.
                    إذا (ميلاپ_Len > مٿاهون_Water_ڊيگهائي) {
                        مٿاهون_Water_ڊيگهائي = ميلاپ_Len;
                    }
                    // In the forward direction we know strings
                    // are sorted so we can bail early.
                    إذا (forward && ميلاپ_Len < مٿاهون_Water_ڊيگهائي) {
                        اكسر;
                    }
                    استمر;
                }
            }

            // We've checked all strings without a partial match.
            // If we have full matches, return the longest one.
            إذا (مٿاهون_Water_ڊيگهائي != 0) {
                وٿي += forward ? مٿاهون_Water_ڊيگهائي : -مٿاهون_Water_ڊيگهائي;
                أرجع U_ميلاپ;
            }
        }
        أرجع UnicodeFilter::matches(متن, وٿي, حد_بندي, incremental);
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
int32_t Unicode_سيٽ::ميلاپ_Rest(ثابت Replaceable& متن,
                              int32_t شروعات, int32_t حد_بندي,
                              ثابت UnicodeString& s) {
    int32_t i;
    int32_t maxLen;
    int32_t slen = s.length();
    إذا (شروعات < حد_بندي) {
        maxLen = حد_بندي - شروعات;
        إذا (maxLen > slen) maxLen = slen;
        لكل (i = 1; i < maxLen; ++i) {
            إذا (متن.charAt(شروعات + i) != s.charAt(i)) أرجع 0;
        }
    } وإلا {
        maxLen = شروعات - حد_بندي;
        إذا (maxLen > slen) maxLen = slen;
        --slen; // <=> slen = s.length() - 1;
        لكل (i = 1; i < maxLen; ++i) {
            إذا (متن.charAt(شروعات - i) != s.charAt(slen - i)) أرجع 0;
        }
    }
    أرجع maxLen;
}

/**
 * Implement of UnicodeMatcher
 */
فراغ Unicode_سيٽ::جوڙيو_ميلاپ_سيٽ_منزل(Unicode_سيٽ& منزل_Union_منزل) ثابت {
    منزل_Union_منزل.جوڙيو_All(*هذا);
}

/**
 * Returns the index of the given character within this set, where
 * the set is ordered by ascending code point.  If the character
 * is not in this set, return -1.  The inverse of this method is
 * <code>charAt()</code>.
 * @return an index from 0..size()-1, or -1
 */
int32_t Unicode_سيٽ::اشاريو_جو(UChar32 c) ثابت {
    إذا (c < MIN_قدر || c > MAX_قدر) {
        أرجع -1;
    }
    int32_t i = 0;
    int32_t n = 0;
    لكل (;;) {
        UChar32 شروعات = list[i++];
        إذا (c < شروعات) {
            أرجع -1;
        }
        UChar32 حد_بندي = list[i++];
        إذا (c < حد_بندي) {
            أرجع n + c - شروعات;
        }
        n += حد_بندي - شروعات;
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
UChar32 Unicode_سيٽ::charAt(int32_t اشاريو) ثابت {
    إذا (اشاريو >= 0) {
        // len2 is the largest even integer <= len, that is, it is len
        // for even values and len-1 for odd values.  With odd values
        // the last entry is UNICODESET_HIGH.
        int32_t len2 = len & ~1;
        لكل (int32_t i=0; i < len2;) {
            UChar32 شروعات = list[i++];
            int32_t count = list[i++] - شروعات;
            إذا (اشاريو < count) {
                أرجع static_cast<UChar32>(شروعات + اشاريو);
            }
            اشاريو -= count;
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
Unicode_سيٽ& Unicode_سيٽ::سيٽ(UChar32 شروعات, UChar32 پڇاڙي) U_LIFETIME_BOUND {
    clear();
    complement(شروعات, پڇاڙي);
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
Unicode_سيٽ& Unicode_سيٽ::جوڙيو(UChar32 شروعات, UChar32 پڇاڙي) U_LIFETIME_BOUND {
    إذا (pin_ڪوڊ_نقطو(شروعات) < pin_ڪوڊ_نقطو(پڇاڙي)) {
        UChar32 حد_بندي = پڇاڙي + 1;
        // Fast path for adding a new range after the last one.
        // Odd list length: [..., lastStart, lastLimit, HIGH]
        إذا ((len & 1) != 0) {
            // If the list is empty, set lastLimit low enough to not be adjacent to 0.
            UChar32 آخري_حد_بندي = len == 1 ? -2 : list[len - 2];
            إذا (آخري_حد_بندي <= شروعات && !isFrozen() && !isBogus()) {
                إذا (آخري_حد_بندي == شروعات) {
                    // Extend the last range.
                    list[len - 2] = حد_بندي;
                    إذا (حد_بندي == UNICODESET_مٿاهون) {
                        --len;
                    }
                } وإلا {
                    list[len - 1] = شروعات;
                    إذا (حد_بندي < UNICODESET_مٿاهون) {
                        إذا (ensure_صلاحيت(len + 2)) {
                            list[len++] = حد_بندي;
                            list[len++] = UNICODESET_مٿاهون;
                        }
                    } وإلا {  // limit == UNICODESET_HIGH
                        إذا (ensure_صلاحيت(len + 1)) {
                            list[len++] = UNICODESET_مٿاهون;
                        }
                    }
                }
                release_نمونو();
                أرجع *هذا;
            }
        }
        // This is slow. Could be much faster using findCodePoint(start)
        // and modifying the list, dealing with adjacent & overlapping ranges.
        UChar32 range[3] = { شروعات, حد_بندي, UNICODESET_مٿاهون };
        جوڙيو(range, 2, 0);
    } وإلا إذا (شروعات == پڇاڙي) {
        جوڙيو(شروعات);
    }
    أرجع *هذا;
}

// #define DEBUG_US_ADD

#جيڪڏهن_وصف_ٿيل DEBUG_US_جوڙيو
#ضمن <stdio.h>
فراغ dump(UChar32 c) {
    إذا (c <= 0xFF) {
        لکو("%c", (حرف)c);
    } وإلا {
        لکو("U+%04X", c);
    }
}
فراغ dump(ثابت UChar32* list, int32_t len) {
    لکو("[");
    لكل (int32_t i=0; i<len; ++i) {
        إذا (i != 0) لکو(", ");
        dump(list[i]);
    }
    لکو("]");
}
#جيڪڏهن_پورو

/**
 * Adds the specified character to this set if it is not already
 * present.  If this set already contains the specified character,
 * the call leaves this set unchanged.
 */
Unicode_سيٽ& Unicode_سيٽ::جوڙيو(UChar32 c) U_LIFETIME_BOUND {
    // find smallest i such that c < list[i]
    // if odd, then it is IN the set
    // if even, then it is OUT of the set
    int32_t i = ڳوليو_ڪوڊ_نقطو(pin_ڪوڊ_نقطو(c));

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

#جيڪڏهن_وصف_ٿيل DEBUG_US_جوڙيو
    لکو("Add of ");
    dump(c);
    لکو(" found at %d", i);
    لکو(": ");
    dump(list, len);
    لکو(" => ");
#جيڪڏهن_پورو

    إذا (c == list[i]-1) {
        // c is before start of next range
        list[i] = c;
        // if we touched the HIGH mark, then add a new one
        إذا (c == (UNICODESET_مٿاهون - 1)) {
            إذا (!ensure_صلاحيت(len+1)) {
                // ensureCapacity will mark the object as Bogus if OOM failure happens.
                أرجع *هذا;
            }
            list[len++] = UNICODESET_مٿاهون;
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

        إذا (!ensure_صلاحيت(len+2)) {
            // ensureCapacity will mark the object as Bogus if OOM failure happens.
            أرجع *هذا;
        }

        UChar32 *p = list + i;
        uprv_memmove(p + 2, p, (len - i) * حجم(*p));
        list[i] = c;
        list[i+1] = c+1;
        len += 2;
    }

#جيڪڏهن_وصف_ٿيل DEBUG_US_جوڙيو
    dump(list, len);
    لکو("\n");

    لكل (i=1; i<len; ++i) {
        إذا (list[i] <= list[i-1]) {
            // Corrupt array!
            لکو("ERROR: list has been corrupted\n");
            ٻاهر_نڪرو(1);
        }
    }
#جيڪڏهن_پورو

    release_نمونو();
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
Unicode_سيٽ& Unicode_سيٽ::جوڙيو(ثابت UnicodeString& s) U_LIFETIME_BOUND {
    إذا (isFrozen() || isBogus()) أرجع *هذا;
    int32_t cp = getSingleCP(s);
    إذا (cp < 0) {
        إذا (!لڙيون_شامل_آهي(s)) {
            جوڙيو_2(s);
            release_نمونو();
        }
    } وإلا {
        جوڙيو(static_cast<UChar32>(cp));
    }
    أرجع *هذا;
}

/**
 * Adds the given string, in order, to 'strings_'.  The given string
 * must have been checked by the caller to not already be in 'strings_'.
 */
فراغ Unicode_سيٽ::جوڙيو_2(ثابت UnicodeString& s) {
    إذا (isFrozen() || isBogus()) {
        أرجع;
    }
    UError_ڪوڊ ec = U_ZERO_ERROR;
    إذا (لڙيون_2 == خالي_اشارو && !allocate_لڙيون(ec)) {
        سيٽ_منزل_Bogus();
        أرجع;
    }
    Local_اشارو<UnicodeString> t(جديد UnicodeString(s));
    إذا (t.isNull()) { // Check for memory allocation error.
        سيٽ_منزل_Bogus();
        أرجع;
    }
    لڙيون_2->sorted_وجهو(t.orphan(), compareUnicodeString, ec);
    إذا (U_FAILURE(ec)) {
        سيٽ_منزل_Bogus();
    }
}

/**
 * @return a code point IF the string consists of a single one.
 * otherwise returns -1.
 * @param string to test
 */
int32_t Unicode_سيٽ::getSingleCP(ثابت UnicodeString& s) {
    int32_t s_ڊيگهائي = s.length();
    إذا (s_ڊيگهائي == 1) أرجع s.charAt(0);
    إذا (s_ڊيگهائي == 2) {
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
Unicode_سيٽ& Unicode_سيٽ::جوڙيو_All(ثابت UnicodeString& s) U_LIFETIME_BOUND {
    UChar32 cp;
    لكل (int32_t i = 0; i < s.length(); i += U16_ڊيگهائي(cp)) {
        cp = s.char32At(i);
        جوڙيو(cp);
    }
    أرجع *هذا;
}

/**
 * Retains EACH of the characters in this string. Note: "ch" == {"c", "h"}
 * If this set already any particular character, it has no effect on that character.
 * @param the source string
 * @return the modified set, for chaining
 */
Unicode_سيٽ& Unicode_سيٽ::retainAll(ثابت UnicodeString& s) U_LIFETIME_BOUND {
    Unicode_سيٽ سيٽ;
    سيٽ.جوڙيو_All(s);
    retainAll(سيٽ);
    أرجع *هذا;
}

/**
 * Complement EACH of the characters in this string. Note: "ch" == {"c", "h"}
 * If this set already any particular character, it has no effect on that character.
 * @param the source string
 * @return the modified set, for chaining
 */
Unicode_سيٽ& Unicode_سيٽ::complementAll(ثابت UnicodeString& s) U_LIFETIME_BOUND {
    Unicode_سيٽ سيٽ;
    سيٽ.جوڙيو_All(s);
    complementAll(سيٽ);
    أرجع *هذا;
}

/**
 * Remove EACH of the characters in this string. Note: "ch" == {"c", "h"}
 * If this set already any particular character, it has no effect on that character.
 * @param the source string
 * @return the modified set, for chaining
 */
Unicode_سيٽ& Unicode_سيٽ::removeAll(ثابت UnicodeString& s) U_LIFETIME_BOUND {
    Unicode_سيٽ سيٽ;
    سيٽ.جوڙيو_All(s);
    removeAll(سيٽ);
    أرجع *هذا;
}

Unicode_سيٽ& Unicode_سيٽ::remove_All_لڙيون() U_LIFETIME_BOUND {
    إذا (!isFrozen() && has_لڙيون()) {
        لڙيون_2->removeAllElements();
        release_نمونو();
    }
    أرجع *هذا;
}


/**
 * Makes a set from a multicharacter string. Thus "ch" => {"ch"}
 * <br><b>Warning: you cannot add an empty string ("") to a UnicodeSet.</b>
 * @param the source string
 * @return a newly created set containing the given string
 */
Unicode_سيٽ* U_EXPORT2 Unicode_سيٽ::createFrom(ثابت UnicodeString& s) {
    Unicode_سيٽ *سيٽ = جديد Unicode_سيٽ();
    إذا (سيٽ != خالي_اشارو) { // Check for memory allocation error.
        سيٽ->جوڙيو(s);
    }
    أرجع سيٽ;
}


/**
 * Makes a set from each of the characters in the string. Thus "ch" => {"c", "h"}
 * @param the source string
 * @return a newly created set containing the given characters
 */
Unicode_سيٽ* U_EXPORT2 Unicode_سيٽ::createFromAll(ثابت UnicodeString& s) {
    Unicode_سيٽ *سيٽ = جديد Unicode_سيٽ();
    إذا (سيٽ != خالي_اشارو) { // Check for memory allocation error.
        سيٽ->جوڙيو_All(s);
    }
    أرجع سيٽ;
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
Unicode_سيٽ& Unicode_سيٽ::retain(UChar32 شروعات, UChar32 پڇاڙي) U_LIFETIME_BOUND {
    إذا (pin_ڪوڊ_نقطو(شروعات) <= pin_ڪوڊ_نقطو(پڇاڙي)) {
        UChar32 range[3] = { شروعات, پڇاڙي+1, UNICODESET_مٿاهون };
        retain(range, 2, 0);
    } وإلا {
        clear();
    }
    أرجع *هذا;
}

Unicode_سيٽ& Unicode_سيٽ::retain(UChar32 c) U_LIFETIME_BOUND {
    أرجع retain(c, c);
}

Unicode_سيٽ& Unicode_سيٽ::retain(ثابت UnicodeString &s) U_LIFETIME_BOUND {
    إذا (isFrozen() || isBogus()) { أرجع *هذا; }
    UChar32 cp = getSingleCP(s);
    إذا (cp < 0) {
        منطقي is_اندريون = لڙيون_شامل_آهي(s);
        // Check for getRangeCount() first to avoid somewhat-expensive size()
        // when there are single code points.
        إذا (is_اندريون && get_Range_ڳڻپ() == 0 && size() == 1) {
            أرجع *هذا;
        }
        clear();
        إذا (is_اندريون) {
            جوڙيو_2(s);
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
Unicode_سيٽ& Unicode_سيٽ::remove(UChar32 شروعات, UChar32 پڇاڙي) U_LIFETIME_BOUND {
    إذا (pin_ڪوڊ_نقطو(شروعات) <= pin_ڪوڊ_نقطو(پڇاڙي)) {
        UChar32 range[3] = { شروعات, پڇاڙي+1, UNICODESET_مٿاهون };
        retain(range, 2, 2);
    }
    أرجع *هذا;
}

/**
 * Removes the specified character from this set if it is present.
 * The set will not contain the specified range once the call
 * returns.
 */
Unicode_سيٽ& Unicode_سيٽ::remove(UChar32 c) U_LIFETIME_BOUND {
    أرجع remove(c, c);
}

/**
 * Removes the specified string from this set if it is present.
 * The set will not contain the specified character once the call
 * returns.
 * @param the source string
 * @return the modified set, for chaining
 */
Unicode_سيٽ& Unicode_سيٽ::remove(ثابت UnicodeString& s) U_LIFETIME_BOUND {
    إذا (isFrozen() || isBogus()) أرجع *هذا;
    int32_t cp = getSingleCP(s);
    إذا (cp < 0) {
        إذا (لڙيون_2 != خالي_اشارو && لڙيون_2->removeElement((فراغ*) &s)) {
            release_نمونو();
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
Unicode_سيٽ& Unicode_سيٽ::complement(UChar32 شروعات, UChar32 پڇاڙي) U_LIFETIME_BOUND {
    إذا (isFrozen() || isBogus()) {
        أرجع *هذا;
    }
    إذا (pin_ڪوڊ_نقطو(شروعات) <= pin_ڪوڊ_نقطو(پڇاڙي)) {
        UChar32 range[3] = { شروعات, پڇاڙي+1, UNICODESET_مٿاهون };
        exclusiveOr(range, 2, 0);
    }
    release_نمونو();
    أرجع *هذا;
}

Unicode_سيٽ& Unicode_سيٽ::complement(UChar32 c) U_LIFETIME_BOUND {
    أرجع complement(c, c);
}

/**
 * This is equivalent to
 * <code>complement(MIN_VALUE, MAX_VALUE)</code>.
 */
Unicode_سيٽ& Unicode_سيٽ::complement() U_LIFETIME_BOUND {
    إذا (isFrozen() || isBogus()) {
        أرجع *هذا;
    }
    إذا (list[0] == UNICODESET_هيٺيون) {
        uprv_memmove(list, list + 1, (size_t)(len-1)*حجم(UChar32));
        --len;
    } وإلا {
        إذا (!ensure_صلاحيت(len+1)) {
            أرجع *هذا;
        }
        uprv_memmove(list + 1, list, (size_t)len*حجم(UChar32));
        list[0] = UNICODESET_هيٺيون;
        ++len;
    }
    release_نمونو();
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
Unicode_سيٽ& Unicode_سيٽ::complement(ثابت UnicodeString& s) U_LIFETIME_BOUND {
    إذا (isFrozen() || isBogus()) أرجع *هذا;
    int32_t cp = getSingleCP(s);
    إذا (cp < 0) {
        إذا (لڙيون_شامل_آهي(s)) {
            لڙيون_2->removeElement((فراغ*) &s);
        } وإلا {
            جوڙيو_2(s);
        }
        release_نمونو();
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
Unicode_سيٽ& Unicode_سيٽ::جوڙيو_All(ثابت Unicode_سيٽ& c) U_LIFETIME_BOUND {
    إذا ( c.len>0 && c.list!=خالي_اشارو ) {
        جوڙيو(c.list, c.len, 0);
    }

    // Add strings in order
    إذا ( c.لڙيون_2!=خالي_اشارو ) {
        لكل (int32_t i=0; i<c.لڙيون_2->size(); ++i) {
            ثابت UnicodeString* s = static_cast<ثابت UnicodeString*>(c.لڙيون_2->elementAt(i));
            إذا (!لڙيون_شامل_آهي(*s)) {
                جوڙيو_2(*s);
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
Unicode_سيٽ& Unicode_سيٽ::retainAll(ثابت Unicode_سيٽ& c) U_LIFETIME_BOUND {
    إذا (isFrozen() || isBogus()) {
        أرجع *هذا;
    }
    retain(c.list, c.len, 0);
    إذا (has_لڙيون()) {
        إذا (!c.has_لڙيون()) {
            لڙيون_2->removeAllElements();
        } وإلا {
            لڙيون_2->retainAll(*c.لڙيون_2);
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
Unicode_سيٽ& Unicode_سيٽ::removeAll(ثابت Unicode_سيٽ& c) U_LIFETIME_BOUND {
    إذا (isFrozen() || isBogus()) {
        أرجع *هذا;
    }
    retain(c.list, c.len, 2);
    إذا (has_لڙيون() && c.has_لڙيون()) {
        لڙيون_2->removeAll(*c.لڙيون_2);
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
Unicode_سيٽ& Unicode_سيٽ::complementAll(ثابت Unicode_سيٽ& c) U_LIFETIME_BOUND {
    إذا (isFrozen() || isBogus()) {
        أرجع *هذا;
    }
    exclusiveOr(c.list, c.len, 0);

    إذا (c.لڙيون_2 != خالي_اشارو) {
        لكل (int32_t i=0; i<c.لڙيون_2->size(); ++i) {
            فراغ* e = c.لڙيون_2->elementAt(i);
            إذا (لڙيون_2 == خالي_اشارو || !لڙيون_2->removeElement(e)) {
                جوڙيو_2(*static_cast<ثابت UnicodeString*>(e));
            }
        }
    }
    أرجع *هذا;
}

/**
 * Removes all of the elements from this set.  This set will be
 * empty after this call returns.
 */
Unicode_سيٽ& Unicode_سيٽ::clear() U_LIFETIME_BOUND {
    إذا (isFrozen()) {
        أرجع *هذا;
    }
    list[0] = UNICODESET_مٿاهون;
    len = 1;
    release_نمونو();
    إذا (لڙيون_2 != خالي_اشارو) {
        لڙيون_2->removeAllElements();
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
int32_t Unicode_سيٽ::get_Range_ڳڻپ() ثابت {
    أرجع len/2;
}

/**
 * Iteration method that returns the first character in the
 * specified range of this set.
 * @see #getRangeCount
 * @see #getRangeEnd
 */
UChar32 Unicode_سيٽ::get_Range_شروعات(int32_t اشاريو) ثابت {
    أرجع list[اشاريو*2];
}

/**
 * Iteration method that returns the last character in the
 * specified range of this set.
 * @see #getRangeStart
 * @see #getRangeEnd
 */
UChar32 Unicode_سيٽ::get_Range_پڇاڙي(int32_t اشاريو) ثابت {
    أرجع list[اشاريو*2 + 1] - 1;
}

ثابت UnicodeString* Unicode_سيٽ::getString(int32_t اشاريو) ثابت U_LIFETIME_BOUND {
    أرجع static_cast<ثابت UnicodeString*>(لڙيون_2->elementAt(اشاريو));
}

/**
 * Reallocate this objects internal structures to take up the least
 * possible space, without changing this object's value.
 */
Unicode_سيٽ& Unicode_سيٽ::compact() U_LIFETIME_BOUND {
    إذا (isFrozen() || isBogus()) {
        أرجع *هذا;
    }
    // Delete buffer first to defragment memory less.
    إذا (بفر != ڍير_List) {
        uprv_free(بفر);
        بفر = خالي_اشارو;
        بفر_صلاحيت = 0;
    }
    إذا (list == ڍير_List) {
        // pass
    } وإلا إذا (len <= INITIAL_صلاحيت) {
        uprv_memcpy(ڍير_List, list, len * حجم(UChar32));
        uprv_free(list);
        list = ڍير_List;
        capacity = INITIAL_صلاحيت;
    } وإلا إذا ((len + 7) < capacity) {
        // If we have more than a little unused capacity, shrink it to len.
        UChar32* عارضي = static_cast<UChar32*>(uprv_realloc(list, حجم(UChar32) * len));
        إذا (عارضي) {
            list = عارضي;
            capacity = len;
        }
        // else what the heck happened?! We allocated less memory!
        // Oh well. We'll keep our original array.
    }
    إذا (لڙيون_2 != خالي_اشارو && لڙيون_2->is_خالي_جايون()) {
        ميٽيو لڙيون_2;
        لڙيون_2 = خالي_اشارو;
    }
    أرجع *هذا;
}

#جيڪڏهن_وصف_ٿيل DEBUG_SERIALIZE
#ضمن <stdio.h>
#جيڪڏهن_پورو

/**
 * Deserialize constructor.
 */
Unicode_سيٽ::Unicode_سيٽ(ثابت uint16_t معلومات[], int32_t معلومات_Len, ESerialization serialization,
                       UError_ڪوڊ &ec) {

  إذا(U_FAILURE(ec)) {
    سيٽ_منزل_Bogus();
    أرجع;
  }

  إذا( (serialization != kSerialized)
      || (معلومات==خالي_اشارو)
      || (معلومات_Len < 1)) {
    ec = U_ILLEGAL_دليل_ERROR;
    سيٽ_منزل_Bogus();
    أرجع;
  }

  // bmp?
  int32_t سرو_سائيز = ((معلومات[0]&0x8000)) ?2:1;
  int32_t bmp_ڊيگهائي = (سرو_سائيز==1)?معلومات[0]:معلومات[1];

  int32_t new_ڊيگهائي = (((معلومات[0]&0x7FFF)-bmp_ڊيگهائي)/2)+bmp_ڊيگهائي;
#جيڪڏهن_وصف_ٿيل DEBUG_SERIALIZE
  لکو("dataLen %d headerSize %d bmpLen %d len %d. data[0]=%X/%X/%X/%X\n", معلومات_Len,سرو_سائيز,bmp_ڊيگهائي,new_ڊيگهائي, معلومات[0],معلومات[1],معلومات[2],معلومات[3]);
#جيڪڏهن_پورو
  إذا(!ensure_صلاحيت(new_ڊيگهائي + 1)) {  // +1 for HIGH
    أرجع;
  }
  // copy bmp
  int32_t i;
  لكل(i = 0; i< bmp_ڊيگهائي;i++) {
    list[i] = معلومات[i+سرو_سائيز];
#جيڪڏهن_وصف_ٿيل DEBUG_SERIALIZE
    لکو("<<16@%d[%d] %X\n", i+سرو_سائيز, i, list[i]);
#جيڪڏهن_پورو
  }
  // copy smp
  لكل(i=bmp_ڊيگهائي;i<new_ڊيگهائي;i++) {
    list[i] = (static_cast<UChar32>(معلومات[سرو_سائيز + bmp_ڊيگهائي + (i - bmp_ڊيگهائي) * 2 + 0]) << 16) +
               static_cast<UChar32>(معلومات[سرو_سائيز + bmp_ڊيگهائي + (i - bmp_ڊيگهائي) * 2 + 1]);
#جيڪڏهن_وصف_ٿيل DEBUG_SERIALIZE
    لکو("<<32@%d+[%d] %lX\n", سرو_سائيز+bmp_ڊيگهائي+i, i, list[i]);
#جيڪڏهن_پورو
  }
  U_ASSERT(i == new_ڊيگهائي);
  إذا (i == 0 || list[i - 1] != UNICODESET_مٿاهون) {
    list[i++] = UNICODESET_مٿاهون;
  }
  len = i;
}


int32_t Unicode_سيٽ::serialize(uint16_t *dest, int32_t dest_صلاحيت, UError_ڪوڊ& ec) ثابت {
    int32_t bmp_ڊيگهائي, length, dest_ڊيگهائي;

    إذا (U_FAILURE(ec)) {
        أرجع 0;
    }

    إذا (dest_صلاحيت<0 || (dest_صلاحيت>0 && dest==خالي_اشارو)) {
        ec=U_ILLEGAL_دليل_ERROR;
        أرجع 0;
    }

    /* count necessary 16-bit units */
    length=هذا->len-1; // Subtract 1 to ignore final UNICODESET_HIGH
    // assert(length>=0);
    إذا (length==0) {
        /* empty set */
        إذا (dest_صلاحيت>0) {
            *dest=0;
        } وإلا {
            ec=U_بفر_حد_کان_مٿي_ERROR;
        }
        أرجع 1;
    }
    /* now length>0 */

    إذا (هذا->list[length-1]<=0xffff) {
        /* all BMP */
        bmp_ڊيگهائي=length;
    } وإلا إذا (هذا->list[0]>=0x10000) {
        /* all supplementary */
        bmp_ڊيگهائي=0;
        length*=2;
    } وإلا {
        /* some BMP, some supplementary */
        لكل (bmp_ڊيگهائي=0; bmp_ڊيگهائي<length && هذا->list[bmp_ڊيگهائي]<=0xffff; ++bmp_ڊيگهائي) {}
        length=bmp_ڊيگهائي+2*(length-bmp_ڊيگهائي);
    }
#جيڪڏهن_وصف_ٿيل DEBUG_SERIALIZE
    لکو(">> bmpLength%d length%d len%d\n", bmp_ڊيگهائي, length, len);
#جيڪڏهن_پورو
    /* length: number of 16-bit array units */
    إذا (length>0x7fff) {
        /* there are only 15 bits for the length in the first serialized word */
        ec=U_اشاريو_OUTOFBOUNDS_ERROR;
        أرجع 0;
    }

    /*
     * total serialized length:
     * number of 16-bit array units (length) +
     * 1 length unit (always) +
     * 1 bmpLength unit (if there are supplementary values)
     */
    dest_ڊيگهائي=length+((length>bmp_ڊيگهائي)?2:1);
    إذا (dest_ڊيگهائي<=dest_صلاحيت) {
        ثابت UChar32 *p;
        int32_t i;

#جيڪڏهن_وصف_ٿيل DEBUG_SERIALIZE
        لکو("writeHdr\n");
#جيڪڏهن_پورو
        *dest = static_cast<uint16_t>(length);
        إذا (length>bmp_ڊيگهائي) {
            *dest|=0x8000;
            *++dest = static_cast<uint16_t>(bmp_ڊيگهائي);
        }
        ++dest;

        /* write the BMP part of the array */
        p=هذا->list;
        لكل (i=0; i<bmp_ڊيگهائي; ++i) {
#جيڪڏهن_وصف_ٿيل DEBUG_SERIALIZE
          لکو("writebmp: %x\n", (صحيح)*p);
#جيڪڏهن_پورو
            *dest++ = static_cast<uint16_t>(*p++);
        }

        /* write the supplementary part of the array */
        لكل (; i<length; i+=2) {
#جيڪڏهن_وصف_ٿيل DEBUG_SERIALIZE
          لکو("write32: %x\n", (صحيح)*p);
#جيڪڏهن_پورو
            *dest++ = static_cast<uint16_t>(*p >> 16);
            *dest++ = static_cast<uint16_t>(*p++);
        }
    } وإلا {
        ec=U_بفر_حد_کان_مٿي_ERROR;
    }
    أرجع dest_ڊيگهائي;
}

//----------------------------------------------------------------
// Implementation: Utility methods
//----------------------------------------------------------------

/**
 * Allocate our strings vector and return true if successful.
 */
UBool Unicode_سيٽ::allocate_لڙيون(UError_ڪوڊ &حالت) {
    إذا (U_FAILURE(حالت)) {
        أرجع خطأ;
    }
    لڙيون_2 = جديد UVector(uprv_deleteUObject,
                          uhash_compareUnicodeString, 1, حالت);
    إذا (لڙيون_2 == خالي_اشارو) { // Check for memory allocation error.
        حالت = U_يادگيري_ورهاست_ERROR;
        أرجع خطأ;
    }
    إذا (U_FAILURE(حالت)) {
        ميٽيو لڙيون_2;
        لڙيون_2 = خالي_اشارو;
        أرجع خطأ;
    } 
    أرجع سچ;
}

int32_t Unicode_سيٽ::ايندڙ_صلاحيت(int32_t min_صلاحيت) {
    // Grow exponentially to reduce the frequency of allocations.
    إذا (min_صلاحيت < INITIAL_صلاحيت) {
        أرجع min_صلاحيت + INITIAL_صلاحيت;
    } وإلا إذا (min_صلاحيت <= 2500) {
        أرجع 5 * min_صلاحيت;
    } وإلا {
        int32_t new_صلاحيت = 2 * min_صلاحيت;
        إذا (new_صلاحيت > MAX_ڊيگهائي) {
            new_صلاحيت = MAX_ڊيگهائي;
        }
        أرجع new_صلاحيت;
    }
}

منطقي Unicode_سيٽ::ensure_صلاحيت(int32_t newLen) {
    إذا (newLen > MAX_ڊيگهائي) {
        newLen = MAX_ڊيگهائي;
    }
    إذا (newLen <= capacity) {
        أرجع سچ;
    }
    int32_t new_صلاحيت = ايندڙ_صلاحيت(newLen);
    UChar32* عارضي = static_cast<UChar32*>(uprv_malloc(new_صلاحيت * حجم(UChar32)));
    إذا (عارضي == خالي_اشارو) {
        سيٽ_منزل_Bogus(); // set the object to bogus state if an OOM failure occurred.
        أرجع خطأ;
    }
    // Copy only the actual contents.
    uprv_memcpy(عارضي, list, len * حجم(UChar32));
    إذا (list != ڍير_List) {
        uprv_free(list);
    }
    list = عارضي;
    capacity = new_صلاحيت;
    أرجع سچ;
}

منطقي Unicode_سيٽ::ensure_بفر_صلاحيت(int32_t newLen) {
    إذا (newLen > MAX_ڊيگهائي) {
        newLen = MAX_ڊيگهائي;
    }
    إذا (newLen <= بفر_صلاحيت) {
        أرجع سچ;
    }
    int32_t new_صلاحيت = ايندڙ_صلاحيت(newLen);
    UChar32* عارضي = static_cast<UChar32*>(uprv_malloc(new_صلاحيت * حجم(UChar32)));
    إذا (عارضي == خالي_اشارو) {
        سيٽ_منزل_Bogus();
        أرجع خطأ;
    }
    // The buffer has no contents to be copied.
    // It is always filled from scratch after this call.
    إذا (بفر != ڍير_List) {
        uprv_free(بفر);
    }
    بفر = عارضي;
    بفر_صلاحيت = new_صلاحيت;
    أرجع سچ;
}

/**
 * Swap list and buffer.
 */
فراغ Unicode_سيٽ::مٽا_سٽا_Buffers() {
    // swap list and buffer
    UChar32* عارضي = list;
    list = بفر;
    بفر = عارضي;

    int32_t c = capacity;
    capacity = بفر_صلاحيت;
    بفر_صلاحيت = c;
}

فراغ Unicode_سيٽ::سيٽ_منزل_Bogus() {
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

فراغ Unicode_سيٽ::exclusiveOr(ثابت UChar32* other, int32_t otherLen, int8_t polarity) {
    إذا (isFrozen() || isBogus()) {
        أرجع;
    }
    إذا (!ensure_بفر_صلاحيت(len + otherLen)) {
        أرجع;
    }

    int32_t i = 0, j = 0, k = 0;
    UChar32 a = list[i++];
    UChar32 b;
    إذا (polarity == 1 || polarity == 2) {
        b = UNICODESET_هيٺيون;
        إذا (other[j] == UNICODESET_هيٺيون) { // skip base if already LOW
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
            بفر[k++] = a;
            a = list[i++];
        } وإلا إذا (b < a) {
            بفر[k++] = b;
            b = other[j++];
        } وإلا إذا (a != UNICODESET_مٿاهون) { // at this point, a == b
            // discard both values!
            a = list[i++];
            b = other[j++];
        } وإلا { // DONE!
            بفر[k++] = UNICODESET_مٿاهون;
            len = k;
            اكسر;
        }
    }
    مٽا_سٽا_Buffers();
    release_نمونو();
}

// polarity = 0 is normal: x union y
// polarity = 2: x union ~y
// polarity = 1: ~x union y
// polarity = 3: ~x union ~y

فراغ Unicode_سيٽ::جوڙيو(ثابت UChar32* other, int32_t otherLen, int8_t polarity) {
    إذا (isFrozen() || isBogus() || other==خالي_اشارو) {
        أرجع;
    }
    إذا (!ensure_بفر_صلاحيت(len + otherLen)) {
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
                إذا (k > 0 && a <= بفر[k-1]) {
                    // Pick latter end value in buffer[] vs. list[]
                    a = max(list[i], بفر[--k]);
                } وإلا {
                    // No overlap
                    بفر[k++] = a;
                    a = list[i];
                }
                i++; // Common if/else code factored out
                polarity ^= 1;
            } وإلا إذا (b < a) { // take b
                إذا (k > 0 && b <= بفر[k-1]) {
                    b = max(other[j], بفر[--k]);
                } وإلا {
                    بفر[k++] = b;
                    b = other[j];
                }
                j++;
                polarity ^= 2;
            } وإلا { // a == b, take a, drop b
                إذا (a == UNICODESET_مٿاهون) اذهب loop_پڇاڙي;
                // This is symmetrical; it doesn't matter if
                // we backtrack with a or b. - liu
                إذا (k > 0 && a <= بفر[k-1]) {
                    a = max(list[i], بفر[--k]);
                } وإلا {
                    // No overlap
                    بفر[k++] = a;
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
                إذا (a == UNICODESET_مٿاهون) اذهب loop_پڇاڙي;
                بفر[k++] = a;
            } وإلا { // take b
                إذا (b == UNICODESET_مٿاهون) اذهب loop_پڇاڙي;
                بفر[k++] = b;
            }
            a = list[i++];
            polarity ^= 1;   // factored common code
            b = other[j++];
            polarity ^= 2;
            اكسر;
          حالة 1: // a second, b first; if b < a, overlap
            إذا (a < b) { // no overlap, take a
                بفر[k++] = a; a = list[i++]; polarity ^= 1;
            } وإلا إذا (b < a) { // OVERLAP, drop b
                b = other[j++];
                polarity ^= 2;
            } وإلا { // a == b, drop both!
                إذا (a == UNICODESET_مٿاهون) اذهب loop_پڇاڙي;
                a = list[i++];
                polarity ^= 1;
                b = other[j++];
                polarity ^= 2;
            }
            اكسر;
          حالة 2: // a first, b second; if a < b, overlap
            إذا (b < a) { // no overlap, take b
                بفر[k++] = b;
                b = other[j++];
                polarity ^= 2;
            } وإلا  إذا (a < b) { // OVERLAP, drop a
                a = list[i++];
                polarity ^= 1;
            } وإلا { // a == b, drop both!
                إذا (a == UNICODESET_مٿاهون) اذهب loop_پڇاڙي;
                a = list[i++];
                polarity ^= 1;
                b = other[j++];
                polarity ^= 2;
            }
            اكسر;
        }
    }
 loop_پڇاڙي:
    بفر[k++] = UNICODESET_مٿاهون;    // terminate
    len = k;
    مٽا_سٽا_Buffers();
    release_نمونو();
}

// polarity = 0 is normal: x intersect y
// polarity = 2: x intersect ~y == set-minus
// polarity = 1: ~x intersect y
// polarity = 3: ~x intersect ~y

فراغ Unicode_سيٽ::retain(ثابت UChar32* other, int32_t otherLen, int8_t polarity) {
    إذا (isFrozen() || isBogus()) {
        أرجع;
    }
    إذا (!ensure_بفر_صلاحيت(len + otherLen)) {
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
                إذا (a == UNICODESET_مٿاهون) اذهب loop_پڇاڙي;
                بفر[k++] = a;
                a = list[i++];
                polarity ^= 1;
                b = other[j++];
                polarity ^= 2;
            }
            اكسر;
          حالة 3: // both second; take lower if unequal
            إذا (a < b) { // take a
                بفر[k++] = a;
                a = list[i++];
                polarity ^= 1;
            } وإلا إذا (b < a) { // take b
                بفر[k++] = b;
                b = other[j++];
                polarity ^= 2;
            } وإلا { // a == b, take one, drop other
                إذا (a == UNICODESET_مٿاهون) اذهب loop_پڇاڙي;
                بفر[k++] = a;
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
                بفر[k++] = b;
                b = other[j++];
                polarity ^= 2;
            } وإلا { // a == b, drop both!
                إذا (a == UNICODESET_مٿاهون) اذهب loop_پڇاڙي;
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
                بفر[k++] = a;
                a = list[i++];
                polarity ^= 1;
            } وإلا { // a == b, drop both!
                إذا (a == UNICODESET_مٿاهون) اذهب loop_پڇاڙي;
                a = list[i++];
                polarity ^= 1;
                b = other[j++];
                polarity ^= 2;
            }
            اكسر;
        }
    }
 loop_پڇاڙي:
    بفر[k++] = UNICODESET_مٿاهون;    // terminate
    len = k;
    مٽا_سٽا_Buffers();
    release_نمونو();
}

/**
 * Append the <code>toPattern()</code> representation of a
 * string to the given <code>StringBuffer</code>.
 */
فراغ Unicode_سيٽ::append_منزل_Pat(UnicodeString& buf, ثابت UnicodeString& s, UBool escapeUnprintable) {
    UChar32 cp;
    لكل (int32_t i = 0; i < s.length(); i += U16_ڊيگهائي(cp)) {
        append_منزل_Pat(buf, cp = s.char32At(i), escapeUnprintable);
    }
}

/**
 * Append the <code>toPattern()</code> representation of a
 * character to the given <code>StringBuffer</code>.
 */
فراغ Unicode_سيٽ::append_منزل_Pat(UnicodeString& buf, UChar32 c, UBool escapeUnprintable) {
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
    حالة نشان_جدول::نشان_REF:
        buf.append(u'\\');
        اكسر;
    افتراضي:
        // Escape whitespace
        إذا (نمونو_Props::isWhiteSpace(c)) {
            buf.append(u'\\');
        }
        اكسر;
    }
    buf.append(c);
}

فراغ Unicode_سيٽ::append_منزل_Pat(UnicodeString &نتيجو, UChar32 شروعات, UChar32 پڇاڙي,
                              UBool escapeUnprintable) {
    append_منزل_Pat(نتيجو, شروعات, escapeUnprintable);
    إذا (شروعات != پڇاڙي) {
        إذا ((شروعات+1) != پڇاڙي ||
                // Avoid writing what looks like a lead+trail surrogate pair.
                شروعات == 0xdbff) {
            نتيجو.append(u'-');
        }
        append_منزل_Pat(نتيجو, پڇاڙي, escapeUnprintable);
    }
}

/**
 * Append a string representation of this set to result.  This will be
 * a cleaned version of the string passed to applyPattern(), if there
 * is one.  Otherwise it will be generated.
 */
UnicodeString& Unicode_سيٽ::منزل_نمونو_2(UnicodeString& نتيجو U_LIFETIME_BOUND,
                                      UBool escapeUnprintable) ثابت
{
    إذا (pat != خالي_اشارو) {
        int32_t i;
        int32_t backslash_ڳڻپ = 0;
        لكل (i=0; i<patLen; ) {
            UChar32 c;
            U16_ايندڙ(pat, i, patLen, c);
            إذا (escapeUnprintable ?
                    ICU_Utility::isUnprintable(c) : ICU_Utility::shouldAlwaysBeEscaped(c)) {
                // If the unprintable character is preceded by an odd
                // number of backslashes, then it has been escaped.
                // Before unescaping it, we delete the final
                // backslash.
                إذا ((backslash_ڳڻپ % 2) == 1) {
                    نتيجو.truncate(نتيجو.length() - 1);
                }
                ICU_Utility::escape(نتيجو, c);
                backslash_ڳڻپ = 0;
            } وإلا {
                نتيجو.append(c);
                إذا (c == u'\\') {
                    ++backslash_ڳڻپ;
                } وإلا {
                    backslash_ڳڻپ = 0;
                }
            }
        }
        أرجع نتيجو;
    }

    أرجع generate_نمونو(نتيجو, escapeUnprintable);
}

/**
 * Returns a string representation of this set.  If the result of
 * calling this function is passed to a UnicodeSet constructor, it
 * will produce another set that is equal to this one.
 */
UnicodeString& Unicode_سيٽ::منزل_نمونو(UnicodeString& نتيجو U_LIFETIME_BOUND,
                                     UBool escapeUnprintable) ثابت
{
    نتيجو.truncate(0);
    أرجع منزل_نمونو_2(نتيجو, escapeUnprintable);
}

/**
 * Generate and append a string representation of this set to result.
 * This does not use this.pat, the cleaned up copy of the string
 * passed to applyPattern().
 */
UnicodeString& Unicode_سيٽ::generate_نمونو(UnicodeString& نتيجو U_LIFETIME_BOUND,
                                            UBool escapeUnprintable) ثابت
{
    نتيجو.append(u'[');

    int32_t i = 0;
    int32_t حد_بندي = len & ~1;  // = 2 * getRangeCount()

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
    إذا (len >= 4 && list[0] == 0 && حد_بندي == len && !has_لڙيون()) {
        // Emit the inverse
        نتيجو.append(u'^');
        // Offsetting the inversion list index by one lets us
        // iterate over the ranges of the set complement.
        i = 1;
        --حد_بندي;
    }

    // Emit the ranges as pairs.
    طالما (i < حد_بندي) {
        UChar32 شروعات = list[i];  // getRangeStart()
        UChar32 پڇاڙي = list[i + 1] - 1;  // getRangeEnd() = range limit minus one
        إذا (!(0xd800 <= پڇاڙي && پڇاڙي <= 0xdbff)) {
            append_منزل_Pat(نتيجو, شروعات, پڇاڙي, escapeUnprintable);
            i += 2;
        } وإلا {
            // The range ends with a lead surrogate.
            // Avoid writing what looks like a lead+trail surrogate pair.
            // 1. Postpone ranges that start with a lead surrogate code point.
            int32_t پهريون_Lead = i;
            طالما ((i += 2) < حد_بندي && list[i] <= 0xdbff) {}
            int32_t پهريون_After_Lead = i;
            // 2. Write following ranges that start with a trail surrogate code point.
            طالما (i < حد_بندي && (شروعات = list[i]) <= 0xdfff) {
                append_منزل_Pat(نتيجو, شروعات, list[i + 1] - 1, escapeUnprintable);
                i += 2;
            }
            // 3. Now write the postponed ranges.
            لكل (صحيح j = پهريون_Lead; j < پهريون_After_Lead; j += 2) {
                append_منزل_Pat(نتيجو, list[j], list[j + 1] - 1, escapeUnprintable);
            }
        }
    }

    إذا (لڙيون_2 != خالي_اشارو) {
        لكل (int32_t i = 0; i<لڙيون_2->size(); ++i) {
            نتيجو.append(u'{');
            append_منزل_Pat(نتيجو,
                         *static_cast<ثابت UnicodeString*>(لڙيون_2->elementAt(i)),
                         escapeUnprintable);
            نتيجو.append(u'}');
        }
    }
    أرجع نتيجو.append(u']');
}

/**
* Release existing cached pattern
*/
فراغ Unicode_سيٽ::release_نمونو() {
    إذا (pat) {
        uprv_free(pat);
        pat = خالي_اشارو;
        patLen = 0;
    }
}

/**
* Set the new pattern to cache.
*/
فراغ Unicode_سيٽ::سيٽ_نمونو(ثابت char16_t *newPat, int32_t newPatLen) {
    release_نمونو();
    pat = static_cast<char16_t*>(uprv_malloc((newPatLen + 1) * حجم(char16_t)));
    إذا (pat) {
        patLen = newPatLen;
        u_memcpy(pat, newPat, patLen);
        pat[patLen] = 0;
    }
    // else we don't care if malloc failed. This was just a nice cache.
    // We can regenerate an equivalent pattern later when requested.
}

Unicode_سيٽ* Unicode_سيٽ::freeze() U_LIFETIME_BOUND {
    إذا(!isFrozen() && !isBogus()) {
        compact();

        // Optimize contains() and span() and similar functions.
        إذا (has_لڙيون()) {
            string_وسعت = جديد Unicode_سيٽ_String_وسعت(*هذا, *لڙيون_2, Unicode_سيٽ_String_وسعت::ALL);
            إذا (string_وسعت == خالي_اشارو) {
                سيٽ_منزل_Bogus();
                أرجع هذا;
            } وإلا إذا (!string_وسعت->needs_String_وسعت_UTF16()) {
                // All strings are irrelevant for span() etc. because
                // all of each string's code points are contained in this set.
                // Do not check needsStringSpanUTF8() because UTF-8 has at most as
                // many relevant strings as UTF-16.
                // (Thus needsStringSpanUTF8() implies needsStringSpanUTF16().)
                ميٽيو string_وسعت;
                string_وسعت = خالي_اشارو;
            }
        }
        إذا (string_وسعت == خالي_اشارو) {
            // No span-relevant strings: Optimize for code point spans.
            bmp_سيٽ=جديد BMPSet(list, len);
            إذا (bmp_سيٽ == خالي_اشارو) { // Check for memory allocation error.
                سيٽ_منزل_Bogus();
            }
        }
    }
    أرجع هذا;
}

int32_t Unicode_سيٽ::وسعت(ثابت char16_t *s, int32_t length, USet_وسعت_Condition وسعت_Condition) ثابت {
    إذا(length>0 && bmp_سيٽ!=خالي_اشارو) {
        أرجع static_cast<int32_t>(bmp_سيٽ->وسعت(s, s + length, وسعت_Condition) - s);
    }
    إذا(length<0) {
        length=u_strlen(s);
    }
    إذا(length==0) {
        أرجع 0;
    }
    إذا(string_وسعت!=خالي_اشارو) {
        أرجع string_وسعت->وسعت(s, length, وسعت_Condition);
    } وإلا إذا(has_لڙيون()) {
        uint32_t which= وسعت_Condition==USET_وسعت_NOT_CONTAINED ?
                            Unicode_سيٽ_String_وسعت::FWD_UTF16_NOT_CONTAINED :
                            Unicode_سيٽ_String_وسعت::FWD_UTF16_CONTAINED;
        Unicode_سيٽ_String_وسعت str_وسعت(*هذا, *لڙيون_2, which);
        إذا(str_وسعت.needs_String_وسعت_UTF16()) {
            أرجع str_وسعت.وسعت(s, length, وسعت_Condition);
        }
    }

    إذا(وسعت_Condition!=USET_وسعت_NOT_CONTAINED) {
        وسعت_Condition=USET_وسعت_CONTAINED;  // Pin to 0/1 values.
    }

    UChar32 c;
    int32_t شروعات=0, prev=0;
    افعل {
        U16_ايندڙ(s, شروعات, length, c);
        إذا(وسعت_Condition!=شامل_آهي(c)) {
            اكسر;
        }
    } طالما((prev=شروعات)<length);
    أرجع prev;
}

int32_t Unicode_سيٽ::وسعت_پٺ(ثابت char16_t *s, int32_t length, USet_وسعت_Condition وسعت_Condition) ثابت {
    إذا(length>0 && bmp_سيٽ!=خالي_اشارو) {
        أرجع static_cast<int32_t>(bmp_سيٽ->وسعت_پٺ(s, s + length, وسعت_Condition) - s);
    }
    إذا(length<0) {
        length=u_strlen(s);
    }
    إذا(length==0) {
        أرجع 0;
    }
    إذا(string_وسعت!=خالي_اشارو) {
        أرجع string_وسعت->وسعت_پٺ(s, length, وسعت_Condition);
    } وإلا إذا(has_لڙيون()) {
        uint32_t which= وسعت_Condition==USET_وسعت_NOT_CONTAINED ?
                            Unicode_سيٽ_String_وسعت::پٺ_UTF16_NOT_CONTAINED :
                            Unicode_سيٽ_String_وسعت::پٺ_UTF16_CONTAINED;
        Unicode_سيٽ_String_وسعت str_وسعت(*هذا, *لڙيون_2, which);
        إذا(str_وسعت.needs_String_وسعت_UTF16()) {
            أرجع str_وسعت.وسعت_پٺ(s, length, وسعت_Condition);
        }
    }

    إذا(وسعت_Condition!=USET_وسعت_NOT_CONTAINED) {
        وسعت_Condition=USET_وسعت_CONTAINED;  // Pin to 0/1 values.
    }

    UChar32 c;
    int32_t prev=length;
    افعل {
        U16_PREV(s, 0, length, c);
        إذا(وسعت_Condition!=شامل_آهي(c)) {
            اكسر;
        }
    } طالما((prev=length)>0);
    أرجع prev;
}

int32_t Unicode_سيٽ::وسعت_UTF8(ثابت حرف *s, int32_t length, USet_وسعت_Condition وسعت_Condition) ثابت {
    إذا(length>0 && bmp_سيٽ!=خالي_اشارو) {
        ثابت uint8_t* s0 = reinterpret_cast<ثابت uint8_t*>(s);
        أرجع static_cast<int32_t>(bmp_سيٽ->وسعت_UTF8(s0, length, وسعت_Condition) - s0);
    }
    إذا(length<0) {
        length = static_cast<int32_t>(uprv_strlen(s));
    }
    إذا(length==0) {
        أرجع 0;
    }
    إذا(string_وسعت!=خالي_اشارو) {
        أرجع string_وسعت->وسعت_UTF8(reinterpret_cast<ثابت uint8_t*>(s), length, وسعت_Condition);
    } وإلا إذا(has_لڙيون()) {
        uint32_t which= وسعت_Condition==USET_وسعت_NOT_CONTAINED ?
                            Unicode_سيٽ_String_وسعت::FWD_UTF8_NOT_CONTAINED :
                            Unicode_سيٽ_String_وسعت::FWD_UTF8_CONTAINED;
        Unicode_سيٽ_String_وسعت str_وسعت(*هذا, *لڙيون_2, which);
        إذا(str_وسعت.needs_String_وسعت_UTF8()) {
            أرجع str_وسعت.وسعت_UTF8(reinterpret_cast<ثابت uint8_t*>(s), length, وسعت_Condition);
        }
    }

    إذا(وسعت_Condition!=USET_وسعت_NOT_CONTAINED) {
        وسعت_Condition=USET_وسعت_CONTAINED;  // Pin to 0/1 values.
    }

    UChar32 c;
    int32_t شروعات=0, prev=0;
    افعل {
        U8_ايندڙ_OR_FFFD(s, شروعات, length, c);
        إذا(وسعت_Condition!=شامل_آهي(c)) {
            اكسر;
        }
    } طالما((prev=شروعات)<length);
    أرجع prev;
}

int32_t Unicode_سيٽ::وسعت_پٺ_UTF8(ثابت حرف *s, int32_t length, USet_وسعت_Condition وسعت_Condition) ثابت {
    إذا(length>0 && bmp_سيٽ!=خالي_اشارو) {
        ثابت uint8_t* s0 = reinterpret_cast<ثابت uint8_t*>(s);
        أرجع bmp_سيٽ->وسعت_پٺ_UTF8(s0, length, وسعت_Condition);
    }
    إذا(length<0) {
        length = static_cast<int32_t>(uprv_strlen(s));
    }
    إذا(length==0) {
        أرجع 0;
    }
    إذا(string_وسعت!=خالي_اشارو) {
        أرجع string_وسعت->وسعت_پٺ_UTF8(reinterpret_cast<ثابت uint8_t*>(s), length, وسعت_Condition);
    } وإلا إذا(has_لڙيون()) {
        uint32_t which= وسعت_Condition==USET_وسعت_NOT_CONTAINED ?
                            Unicode_سيٽ_String_وسعت::پٺ_UTF8_NOT_CONTAINED :
                            Unicode_سيٽ_String_وسعت::پٺ_UTF8_CONTAINED;
        Unicode_سيٽ_String_وسعت str_وسعت(*هذا, *لڙيون_2, which);
        إذا(str_وسعت.needs_String_وسعت_UTF8()) {
            أرجع str_وسعت.وسعت_پٺ_UTF8(reinterpret_cast<ثابت uint8_t*>(s), length, وسعت_Condition);
        }
    }

    إذا(وسعت_Condition!=USET_وسعت_NOT_CONTAINED) {
        وسعت_Condition=USET_وسعت_CONTAINED;  // Pin to 0/1 values.
    }

    UChar32 c;
    int32_t prev=length;
    افعل {
        U8_PREV_OR_FFFD(s, 0, length, c);
        إذا(وسعت_Condition!=شامل_آهي(c)) {
            اكسر;
        }
    } طالما((prev=length)>0);
    أرجع prev;
}

U_NAMESPACE_پڇاڙي
