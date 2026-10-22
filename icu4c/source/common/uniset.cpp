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

#شامل "unicode/utypes.h"
#شامل "unicode/parsepos.h"
#شامل "unicode/symtable.h"
#شامل "unicode/uniset.h"
#شامل "unicode/ustring.h"
#شامل "unicode/utf8.h"
#شامل "unicode/utf16.h"
#شامل "ruleiter.h"
#شامل "cmemory.h"
#شامل "cstring.h"
#شامل "patternprops.h"
#شامل "uelement.h"
#شامل "util.h"
#شامل "uvector.h"
#شامل "charstr.h"
#شامل "ustrfmt.h"
#شامل "uassert.h"
#شامل "bmpset.h"
#شامل "unisetspan.h"

// HIGH_VALUE > all valid values. 110000 for codepoints
#تعریف_کرو UNICODESET_بلند 0x0110000

// LOW <= all valid values. ZERO for codepoints
#تعریف_کرو UNICODESET_کم 0x000000

/** Max list [0, 1, 2, ..., max code point, HIGH] */
constexpr int32_t MAX_لمبائی = UNICODESET_بلند + 1;

U_NAMESPACE_BEGIN

علامت_جدول::~علامت_جدول() {}

UOBJECT_DEFINE_RTTI_IMPLEMENTATION(Unicode_سیٹ)

/**
 * Modify the given UChar32 variable so that it is in range, by
 * pinning values < UNICODESET_LOW to UNICODESET_LOW, and
 * pinning values > UNICODESET_HIGH-1 to UNICODESET_HIGH-1.
 * It modifies its argument in-place and also returns it.
 */
ساکن اندرونی UChar32 pin_رمز_نقطہ(UChar32& c) {
    اگر (c < UNICODESET_کم) {
        c = UNICODESET_کم;
    } ورنہ اگر (c > (UNICODESET_بلند-1)) {
        c = (UNICODESET_بلند-1);
    }
    واپس c;
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

#اگر_تعریف_شدہ DEBUG_MEM
#شامل <stdio.h>
ساکن int32_t dbg_گنتی = 0;

ساکن اندرونی خالی _dbgct(Unicode_سیٹ* سیٹ) {
    UnicodeString str;
    سیٹ->مقصد_نمونہ(str, سچ);
    حرف buf[40];
    str.extract(0, 39, buf, "");
    لکھو("DEBUG UnicodeSet: ct 0x%08X; %d %s\n", سیٹ, ++dbg_گنتی, buf);
}

ساکن اندرونی خالی _dbgdt(Unicode_سیٹ* سیٹ) {
    UnicodeString str;
    سیٹ->مقصد_نمونہ(str, سچ);
    حرف buf[40];
    str.extract(0, 39, buf, "");
    لکھو("DEBUG UnicodeSet: dt 0x%08X; %d %s\n", سیٹ, --dbg_گنتی, buf);
}

#ورنہ

#تعریف_کرو _dbgct(سیٹ)
#تعریف_کرو _dbgdt(سیٹ)

#اگر_ختم

//----------------------------------------------------------------
// UnicodeString in UVector support
//----------------------------------------------------------------

ساکن خالی U_CALLCONV cloneUnicodeString(UElement *dst, UElement *src) {
    dst->اشارہ_گر = نیا UnicodeString(*static_cast<UnicodeString*>(src->اشارہ_گر));
}

ساکن int32_t U_CALLCONV compareUnicodeString(UElement t1, UElement t2) {
    ثابت UnicodeString& a = *static_cast<ثابت UnicodeString*>(t1.اشارہ_گر);
    ثابت UnicodeString& b = *static_cast<ثابت UnicodeString*>(t2.اشارہ_گر);
    واپس a.compare(b);
}

UBool Unicode_سیٹ::has_لڑیاں() ثابت {
    واپس لڑیاں_2 != خالی_اشاریہ && !لڑیاں_2->is_خالی_خانے();
}

int32_t Unicode_سیٹ::لڑیاں_سائز() ثابت {
    واپس لڑیاں_2 == خالی_اشاریہ ? 0 : لڑیاں_2->size();
}

UBool Unicode_سیٹ::لڑیاں_شامل_ہے(ثابت UnicodeString &s) ثابت {
    واپس لڑیاں_2 != خالی_اشاریہ && لڑیاں_2->شامل_ہے((خالی*) &s);
}

//----------------------------------------------------------------
// Constructors &c
//----------------------------------------------------------------

/**
 * Constructs an empty set.
 */
Unicode_سیٹ::Unicode_سیٹ() {
    list[0] = UNICODESET_بلند;
    _dbgct(یہ);
}

/**
 * Constructs a set containing the given range. If <code>end >
 * start</code> then an empty set is created.
 *
 * @param start first character, inclusive, of range
 * @param end last character, inclusive, of range
 */
Unicode_سیٹ::Unicode_سیٹ(UChar32 آغاز, UChar32 انتہا) {
    list[0] = UNICODESET_بلند;
    جوڑو(آغاز, انتہا);
    _dbgct(یہ);
}

/**
 * Constructs a set that is identical to the given UnicodeSet.
 */
Unicode_سیٹ::Unicode_سیٹ(ثابت Unicode_سیٹ& o) : UnicodeFilter(o) {
    *یہ = o;
    _dbgct(یہ);
}

// Copy-construct as thawed.
Unicode_سیٹ::Unicode_سیٹ(ثابت Unicode_سیٹ& o, UBool /* asThawed */) : UnicodeFilter(o) {
    اگر (ensure_استعداد(o.len)) {
        // *this = o except for bmpSet and stringSpan
        len = o.len;
        uprv_memcpy(list, o.list, (size_t)len*حجم(UChar32));
        اگر (o.has_لڑیاں()) {
            UError_رمز حالت_2 = U_ZERO_ERROR;
            اگر (!allocate_لڑیاں(حالت_2) ||
                    (لڑیاں_2->assign(*o.لڑیاں_2, cloneUnicodeString, حالت_2), U_FAILURE(حالت_2))) {
                سیٹ_مقصد_Bogus();
                واپس;
            }
        }
        اگر (o.pat) {
            سیٹ_نمونہ(o.pat, o.patLen);
        }
        _dbgct(یہ);
    }
}

/**
 * Destructs the set.
 */
Unicode_سیٹ::~Unicode_سیٹ() {
    _dbgdt(یہ); // first!
    اگر (list != ڈھیر_List) {
        uprv_free(list);
    }
    حذف bmp_سیٹ;
    اگر (بفر != ڈھیر_List) {
        uprv_free(بفر);
    }
    حذف لڑیاں_2;
    حذف string_وسعت;
    release_نمونہ();
}

/**
 * Assigns this object to be a copy of another.
 */
Unicode_سیٹ& Unicode_سیٹ::عامل=(ثابت Unicode_سیٹ& o) U_LIFETIME_BOUND {
    واپس copyFrom(o, جھوٹ);
}

Unicode_سیٹ& Unicode_سیٹ::copyFrom(ثابت Unicode_سیٹ& o, UBool asThawed) U_LIFETIME_BOUND {
    اگر (یہ == &o) {
        واپس *یہ;
    }
    اگر (isFrozen()) {
        واپس *یہ;
    }
    اگر (o.isBogus()) {
        سیٹ_مقصد_Bogus();
        واپس *یہ;
    }
    اگر (!ensure_استعداد(o.len)) {
        // ensureCapacity will mark the UnicodeSet as Bogus if OOM failure happens.
        واپس *یہ;
    }
    len = o.len;
    uprv_memcpy(list, o.list, (size_t)len*حجم(UChar32));
    اگر (o.bmp_سیٹ != خالی_اشاریہ && !asThawed) {
        bmp_سیٹ = نیا BMPSet(*o.bmp_سیٹ, list, len);
        اگر (bmp_سیٹ == خالی_اشاریہ) { // Check for memory allocation error.
            سیٹ_مقصد_Bogus();
            واپس *یہ;
        }
    }
    اگر (o.has_لڑیاں()) {
        UError_رمز حالت_2 = U_ZERO_ERROR;
        اگر ((لڑیاں_2 == خالی_اشاریہ && !allocate_لڑیاں(حالت_2)) ||
                (لڑیاں_2->assign(*o.لڑیاں_2, cloneUnicodeString, حالت_2), U_FAILURE(حالت_2))) {
            سیٹ_مقصد_Bogus();
            واپس *یہ;
        }
    } ورنہ اگر (has_لڑیاں()) {
        لڑیاں_2->removeAllElements();
    }
    اگر (o.string_وسعت != خالی_اشاریہ && لڑیاں_2 != خالی_اشاریہ && !asThawed) {
        string_وسعت = نیا Unicode_سیٹ_String_وسعت(*o.string_وسعت, *لڑیاں_2);
        اگر (string_وسعت == خالی_اشاریہ) { // Check for memory allocation error.
            سیٹ_مقصد_Bogus();
            واپس *یہ;
        }
    }
    release_نمونہ();
    اگر (o.pat) {
        سیٹ_نمونہ(o.pat, o.patLen);
    }
    واپس *یہ;
}

/**
 * Returns a copy of this object.  All UnicodeMatcher objects have
 * to support cloning in order to allow classes using
 * UnicodeMatchers, such as Transliterator, to implement cloning.
 */
Unicode_سیٹ* Unicode_سیٹ::clone() ثابت {
    واپس نیا Unicode_سیٹ(*یہ);
}

Unicode_سیٹ *Unicode_سیٹ::cloneAsThawed() ثابت {
    واپس نیا Unicode_سیٹ(*یہ, سچ);
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
منطقی Unicode_سیٹ::عامل==(ثابت Unicode_سیٹ& o) ثابت {
    اگر (len != o.len) واپس جھوٹ;
    برائے (int32_t i = 0; i < len; ++i) {
        اگر (list[i] != o.list[i]) واپس جھوٹ;
    }
    اگر (has_لڑیاں() != o.has_لڑیاں()) { واپس جھوٹ; }
    اگر (has_لڑیاں() && *لڑیاں_2 != *o.لڑیاں_2) واپس جھوٹ;
    واپس سچ;
}

/**
 * Returns the hash code value for this set.
 *
 * @return the hash code value for this set.
 * @see Object#hashCode()
 */
int32_t Unicode_سیٹ::hash_رمز() ثابت {
    uint32_t نتیجہ = static_cast<uint32_t>(len);
    برائے (int32_t i = 0; i < len; ++i) {
        نتیجہ *= 1000003u;
        نتیجہ += list[i];
    }
    واپس static_cast<int32_t>(نتیجہ);
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
int32_t Unicode_سیٹ::size() ثابت {
    int32_t n = 0;
    int32_t count = get_Range_گنتی();
    برائے (int32_t i = 0; i < count; ++i) {
        n += get_Range_انتہا(i) - get_Range_آغاز(i) + 1;
    }
    واپس n + لڑیاں_سائز();
}

/**
 * Returns <tt>true</tt> if this set contains no elements.
 *
 * @return <tt>true</tt> if this set contains no elements.
 */
UBool Unicode_سیٹ::is_خالی_خانے() ثابت {
    واپس len == 1 && !has_لڑیاں();
}

/**
 * Returns true if this set contains the given character.
 * @param c character to be checked for containment
 * @return true if the test condition is met
 */
UBool Unicode_سیٹ::شامل_ہے(UChar32 c) ثابت {
    // Set i to the index of the start item greater than ch
    // We know we will terminate without length test!
    // LATER: for large sets, add binary search
    //int32_t i = -1;
    //for (;;) {
    //    if (c < list[++i]) break;
    //}
    اگر (bmp_سیٹ != خالی_اشاریہ) {
        واپس bmp_سیٹ->شامل_ہے(c);
    }
    اگر (string_وسعت != خالی_اشاریہ) {
        واپس string_وسعت->شامل_ہے(c);
    }
    اگر (c >= UNICODESET_بلند) { // Don't need to check LOW bound
        واپس جھوٹ;
    }
    int32_t i = ڈھونڈو_رمز_نقطہ(c);
    واپس i & 1; // return true if odd
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
int32_t Unicode_سیٹ::ڈھونڈو_رمز_نقطہ(UChar32 c) ثابت {
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
    اگر (c < list[0])
        واپس 0;
    // High runner test.  c is often after the last range, so an
    // initial check for this condition pays off.
    int32_t lo = 0;
    int32_t hi = len - 1;
    اگر (lo >= hi || c >= list[hi-1])
        واپس hi;
    // invariant: c >= list[lo]
    // invariant: c < list[hi]
    برائے (;;) {
        int32_t i = (lo + hi) >> 1;
        اگر (i == lo) {
            توڑو; // Found!
        } ورنہ اگر (c < list[i]) {
            hi = i;
        } ورنہ {
            lo = i;
        }
    }
    واپس hi;
}

/**
 * Returns true if this set contains every character
 * of the given range.
 * @param start first character, inclusive, of the range
 * @param end last character, inclusive, of the range
 * @return true if the test condition is met
 */
UBool Unicode_سیٹ::شامل_ہے(UChar32 آغاز, UChar32 انتہا) ثابت {
    //int32_t i = -1;
    //for (;;) {
    //    if (start < list[++i]) break;
    //}
    int32_t i = ڈھونڈو_رمز_نقطہ(آغاز);
    واپس ((i & 1) != 0 && انتہا < list[i]);
}

/**
 * Returns <tt>true</tt> if this set contains the given
 * multicharacter string.
 * @param s string to be checked for containment
 * @return <tt>true</tt> if this set contains the specified string
 */
UBool Unicode_سیٹ::شامل_ہے(ثابت UnicodeString& s) ثابت {
    int32_t cp = getSingleCP(s);
    اگر (cp < 0) {
        واپس لڑیاں_شامل_ہے(s);
    } ورنہ {
        واپس شامل_ہے(static_cast<UChar32>(cp));
    }
}

/**
 * Returns true if this set contains all the characters and strings
 * of the given set.
 * @param c set to be checked for containment
 * @return true if the test condition is met
 */
UBool Unicode_سیٹ::شامل_ہے_All(ثابت Unicode_سیٹ& c) ثابت {
    // The specified set is a subset if all of its pairs are contained in
    // this set.  It's possible to code this more efficiently in terms of
    // direct manipulation of the inversion lists if the need arises.
    int32_t n = c.get_Range_گنتی();
    برائے (صحیح i=0; i<n; ++i) {
        اگر (!شامل_ہے(c.get_Range_آغاز(i), c.get_Range_انتہا(i))) {
            واپس جھوٹ;
        }
    }
    واپس !c.has_لڑیاں() || (لڑیاں_2 != خالی_اشاریہ && لڑیاں_2->شامل_ہے_All(*c.لڑیاں_2));
}

/**
 * Returns true if this set contains all the characters
 * of the given string.
 * @param s string containing characters to be checked for containment
 * @return true if the test condition is met
 */
UBool Unicode_سیٹ::شامل_ہے_All(ثابت UnicodeString& s) ثابت {
    واپس وسعت(s.get_بفر(), s.length(), USET_وسعت_CONTAINED) == s.length();
}

/**
 * Returns true if this set contains none of the characters
 * of the given range.
 * @param start first character, inclusive, of the range
 * @param end last character, inclusive, of the range
 * @return true if the test condition is met
 */
UBool Unicode_سیٹ::شامل_ہے_None(UChar32 آغاز, UChar32 انتہا) ثابت {
    //int32_t i = -1;
    //for (;;) {
    //    if (start < list[++i]) break;
    //}
    int32_t i = ڈھونڈو_رمز_نقطہ(آغاز);
    واپس ((i & 1) == 0 && انتہا < list[i]);
}

/**
 * Returns true if this set contains none of the characters and strings
 * of the given set.
 * @param c set to be checked for containment
 * @return true if the test condition is met
 */
UBool Unicode_سیٹ::شامل_ہے_None(ثابت Unicode_سیٹ& c) ثابت {
    // The specified set is a subset if all of its pairs are contained in
    // this set.  It's possible to code this more efficiently in terms of
    // direct manipulation of the inversion lists if the need arises.
    int32_t n = c.get_Range_گنتی();
    برائے (int32_t i=0; i<n; ++i) {
        اگر (!شامل_ہے_None(c.get_Range_آغاز(i), c.get_Range_انتہا(i))) {
            واپس جھوٹ;
        }
    }
    واپس لڑیاں_2 == خالی_اشاریہ || !c.has_لڑیاں() || لڑیاں_2->شامل_ہے_None(*c.لڑیاں_2);
}

/**
 * Returns true if this set contains none of the characters
 * of the given string.
 * @param s string containing characters to be checked for containment
 * @return true if the test condition is met
 */
UBool Unicode_سیٹ::شامل_ہے_None(ثابت UnicodeString& s) ثابت {
    واپس وسعت(s.get_بفر(), s.length(), USET_وسعت_NOT_CONTAINED) == s.length();
}

/**
 * Returns <tt>true</tt> if this set contains any character whose low byte
 * is the given value.  This is used by <tt>RuleBasedTransliterator</tt> for
 * indexing.
 */
UBool Unicode_سیٹ::matches_اشاریہ_قدر(uint8_t v) ثابت {
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
    برائے (i=0; i<range_گنتی; ++i) {
        UChar32 کم = get_Range_آغاز(i);
        UChar32 بلند = get_Range_انتہا(i);
        اگر ((کم & ~0xFF) == (بلند & ~0xFF)) {
            اگر ((کم & 0xFF) <= v && v <= (بلند & 0xFF)) {
                واپس سچ;
            }
        } ورنہ اگر ((کم & 0xFF) <= v || v <= (بلند & 0xFF)) {
            واپس سچ;
        }
    }
    اگر (has_لڑیاں()) {
        برائے (i=0; i<لڑیاں_2->size(); ++i) {
            ثابت UnicodeString& s = *static_cast<ثابت UnicodeString*>(لڑیاں_2->elementAt(i));
            اگر (s.is_خالی_خانے()) {
                جاری;  // skip the empty string
            }
            UChar32 c = s.char32At(0);
            اگر ((c & 0xFF) == v) {
                واپس سچ;
            }
        }
    }
    واپس جھوٹ;
}

/**
 * Implementation of UnicodeMatcher::matches().  Always matches the
 * longest possible multichar string.
 */
UMatchDegree Unicode_سیٹ::matches(ثابت Replaceable& متن,
                                 int32_t& انحراف,
                                 int32_t حد_بندی,
                                 UBool incremental) {
    اگر (انحراف == حد_بندی) {
        اگر (شامل_ہے(U_ETHER)) {
            واپس incremental ? U_PARTIAL_مطابقت : U_مطابقت;
        } ورنہ {
            واپس U_MISMATCH;
        }
    } ورنہ {
        اگر (has_لڑیاں()) { // try strings first

            // might separate forward and backward loops later
            // for now they are combined

            // TODO Improve efficiency of this, at least in the forward
            // direction, if not in both.  In the forward direction we
            // can assume the strings are sorted.

            int32_t i;
            UBool forward = انحراف < حد_بندی;

            // firstChar is the leftmost char to match in the
            // forward direction or the rightmost char to match in
            // the reverse direction.
            char16_t پہلا_Char = متن.charAt(انحراف);

            // If there are multiple strings that can match we
            // return the longest match.
            int32_t بلند_Water_لمبائی = 0;

            برائے (i=0; i<لڑیاں_2->size(); ++i) {
                ثابت UnicodeString& trial = *static_cast<ثابت UnicodeString*>(لڑیاں_2->elementAt(i));
                اگر (trial.is_خالی_خانے()) {
                    جاری;  // skip the empty string
                }

                char16_t c = trial.charAt(forward ? 0 : trial.length() - 1);

                // Strings are sorted, so we can optimize in the
                // forward direction.
                اگر (forward && c > پہلا_Char) توڑو;
                اگر (c != پہلا_Char) جاری;

                int32_t مطابقت_Len = مطابقت_Rest(متن, انحراف, حد_بندی, trial);

                اگر (incremental) {
                    int32_t maxLen = forward ? حد_بندی-انحراف : انحراف-حد_بندی;
                    اگر (مطابقت_Len == maxLen) {
                        // We have successfully matched but only up to limit.
                        واپس U_PARTIAL_مطابقت;
                    }
                }

                اگر (مطابقت_Len == trial.length()) {
                    // We have successfully matched the whole string.
                    اگر (مطابقت_Len > بلند_Water_لمبائی) {
                        بلند_Water_لمبائی = مطابقت_Len;
                    }
                    // In the forward direction we know strings
                    // are sorted so we can bail early.
                    اگر (forward && مطابقت_Len < بلند_Water_لمبائی) {
                        توڑو;
                    }
                    جاری;
                }
            }

            // We've checked all strings without a partial match.
            // If we have full matches, return the longest one.
            اگر (بلند_Water_لمبائی != 0) {
                انحراف += forward ? بلند_Water_لمبائی : -بلند_Water_لمبائی;
                واپس U_مطابقت;
            }
        }
        واپس UnicodeFilter::matches(متن, انحراف, حد_بندی, incremental);
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
int32_t Unicode_سیٹ::مطابقت_Rest(ثابت Replaceable& متن,
                              int32_t آغاز, int32_t حد_بندی,
                              ثابت UnicodeString& s) {
    int32_t i;
    int32_t maxLen;
    int32_t slen = s.length();
    اگر (آغاز < حد_بندی) {
        maxLen = حد_بندی - آغاز;
        اگر (maxLen > slen) maxLen = slen;
        برائے (i = 1; i < maxLen; ++i) {
            اگر (متن.charAt(آغاز + i) != s.charAt(i)) واپس 0;
        }
    } ورنہ {
        maxLen = آغاز - حد_بندی;
        اگر (maxLen > slen) maxLen = slen;
        --slen; // <=> slen = s.length() - 1;
        برائے (i = 1; i < maxLen; ++i) {
            اگر (متن.charAt(آغاز - i) != s.charAt(slen - i)) واپس 0;
        }
    }
    واپس maxLen;
}

/**
 * Implement of UnicodeMatcher
 */
خالی Unicode_سیٹ::جوڑو_مطابقت_سیٹ_مقصد(Unicode_سیٹ& مقصد_Union_مقصد) ثابت {
    مقصد_Union_مقصد.جوڑو_All(*یہ);
}

/**
 * Returns the index of the given character within this set, where
 * the set is ordered by ascending code point.  If the character
 * is not in this set, return -1.  The inverse of this method is
 * <code>charAt()</code>.
 * @return an index from 0..size()-1, or -1
 */
int32_t Unicode_سیٹ::اشاریہ_کا(UChar32 c) ثابت {
    اگر (c < MIN_قدر || c > MAX_قدر) {
        واپس -1;
    }
    int32_t i = 0;
    int32_t n = 0;
    برائے (;;) {
        UChar32 آغاز = list[i++];
        اگر (c < آغاز) {
            واپس -1;
        }
        UChar32 حد_بندی = list[i++];
        اگر (c < حد_بندی) {
            واپس n + c - آغاز;
        }
        n += حد_بندی - آغاز;
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
UChar32 Unicode_سیٹ::charAt(int32_t اشاریہ) ثابت {
    اگر (اشاریہ >= 0) {
        // len2 is the largest even integer <= len, that is, it is len
        // for even values and len-1 for odd values.  With odd values
        // the last entry is UNICODESET_HIGH.
        int32_t len2 = len & ~1;
        برائے (int32_t i=0; i < len2;) {
            UChar32 آغاز = list[i++];
            int32_t count = list[i++] - آغاز;
            اگر (اشاریہ < count) {
                واپس static_cast<UChar32>(آغاز + اشاریہ);
            }
            اشاریہ -= count;
        }
    }
    واپس static_cast<UChar32>(-1);
}

/**
 * Make this object represent the range <code>start - end</code>.
 * If <code>end > start</code> then this object is set to an
 * an empty range.
 *
 * @param start first character in the set, inclusive
 * @rparam end last character in the set, inclusive
 */
Unicode_سیٹ& Unicode_سیٹ::سیٹ(UChar32 آغاز, UChar32 انتہا) U_LIFETIME_BOUND {
    clear();
    complement(آغاز, انتہا);
    واپس *یہ;
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
Unicode_سیٹ& Unicode_سیٹ::جوڑو(UChar32 آغاز, UChar32 انتہا) U_LIFETIME_BOUND {
    اگر (pin_رمز_نقطہ(آغاز) < pin_رمز_نقطہ(انتہا)) {
        UChar32 حد_بندی = انتہا + 1;
        // Fast path for adding a new range after the last one.
        // Odd list length: [..., lastStart, lastLimit, HIGH]
        اگر ((len & 1) != 0) {
            // If the list is empty, set lastLimit low enough to not be adjacent to 0.
            UChar32 آخری_حد_بندی = len == 1 ? -2 : list[len - 2];
            اگر (آخری_حد_بندی <= آغاز && !isFrozen() && !isBogus()) {
                اگر (آخری_حد_بندی == آغاز) {
                    // Extend the last range.
                    list[len - 2] = حد_بندی;
                    اگر (حد_بندی == UNICODESET_بلند) {
                        --len;
                    }
                } ورنہ {
                    list[len - 1] = آغاز;
                    اگر (حد_بندی < UNICODESET_بلند) {
                        اگر (ensure_استعداد(len + 2)) {
                            list[len++] = حد_بندی;
                            list[len++] = UNICODESET_بلند;
                        }
                    } ورنہ {  // limit == UNICODESET_HIGH
                        اگر (ensure_استعداد(len + 1)) {
                            list[len++] = UNICODESET_بلند;
                        }
                    }
                }
                release_نمونہ();
                واپس *یہ;
            }
        }
        // This is slow. Could be much faster using findCodePoint(start)
        // and modifying the list, dealing with adjacent & overlapping ranges.
        UChar32 range[3] = { آغاز, حد_بندی, UNICODESET_بلند };
        جوڑو(range, 2, 0);
    } ورنہ اگر (آغاز == انتہا) {
        جوڑو(آغاز);
    }
    واپس *یہ;
}

// #define DEBUG_US_ADD

#اگر_تعریف_شدہ DEBUG_US_جوڑو
#شامل <stdio.h>
خالی dump(UChar32 c) {
    اگر (c <= 0xFF) {
        لکھو("%c", (حرف)c);
    } ورنہ {
        لکھو("U+%04X", c);
    }
}
خالی dump(ثابت UChar32* list, int32_t len) {
    لکھو("[");
    برائے (int32_t i=0; i<len; ++i) {
        اگر (i != 0) لکھو(", ");
        dump(list[i]);
    }
    لکھو("]");
}
#اگر_ختم

/**
 * Adds the specified character to this set if it is not already
 * present.  If this set already contains the specified character,
 * the call leaves this set unchanged.
 */
Unicode_سیٹ& Unicode_سیٹ::جوڑو(UChar32 c) U_LIFETIME_BOUND {
    // find smallest i such that c < list[i]
    // if odd, then it is IN the set
    // if even, then it is OUT of the set
    int32_t i = ڈھونڈو_رمز_نقطہ(pin_رمز_نقطہ(c));

    // already in set?
    اگر ((i & 1) != 0  || isFrozen() || isBogus()) واپس *یہ;

    // HIGH is 0x110000
    // assert(list[len-1] == HIGH);

    // empty = [HIGH]
    // [start_0, limit_0, start_1, limit_1, HIGH]

    // [..., start_k-1, limit_k-1, start_k, limit_k, ..., HIGH]
    //                             ^
    //                             list[i]

    // i == 0 means c is before the first range

#اگر_تعریف_شدہ DEBUG_US_جوڑو
    لکھو("Add of ");
    dump(c);
    لکھو(" found at %d", i);
    لکھو(": ");
    dump(list, len);
    لکھو(" => ");
#اگر_ختم

    اگر (c == list[i]-1) {
        // c is before start of next range
        list[i] = c;
        // if we touched the HIGH mark, then add a new one
        اگر (c == (UNICODESET_بلند - 1)) {
            اگر (!ensure_استعداد(len+1)) {
                // ensureCapacity will mark the object as Bogus if OOM failure happens.
                واپس *یہ;
            }
            list[len++] = UNICODESET_بلند;
        }
        اگر (i > 0 && c == list[i-1]) {
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
            جب (src < srclimit) *(dst++) = *(src++);

            len -= 2;
        }
    }

    ورنہ اگر (i > 0 && c == list[i-1]) {
        // c is after end of prior range
        list[i-1]++;
        // no need to check for collapse here
    }

    ورنہ {
        // At this point we know the new char is not adjacent to
        // any existing ranges, and it is not 10FFFF.


        // [..., start_k-1, limit_k-1, start_k, limit_k, ..., HIGH]
        //                             ^
        //                             list[i]

        // [..., start_k-1, limit_k-1, c, c+1, start_k, limit_k, ..., HIGH]
        //                             ^
        //                             list[i]

        اگر (!ensure_استعداد(len+2)) {
            // ensureCapacity will mark the object as Bogus if OOM failure happens.
            واپس *یہ;
        }

        UChar32 *p = list + i;
        uprv_memmove(p + 2, p, (len - i) * حجم(*p));
        list[i] = c;
        list[i+1] = c+1;
        len += 2;
    }

#اگر_تعریف_شدہ DEBUG_US_جوڑو
    dump(list, len);
    لکھو("\n");

    برائے (i=1; i<len; ++i) {
        اگر (list[i] <= list[i-1]) {
            // Corrupt array!
            لکھو("ERROR: list has been corrupted\n");
            باہر_نکلو(1);
        }
    }
#اگر_ختم

    release_نمونہ();
    واپس *یہ;
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
Unicode_سیٹ& Unicode_سیٹ::جوڑو(ثابت UnicodeString& s) U_LIFETIME_BOUND {
    اگر (isFrozen() || isBogus()) واپس *یہ;
    int32_t cp = getSingleCP(s);
    اگر (cp < 0) {
        اگر (!لڑیاں_شامل_ہے(s)) {
            جوڑو_2(s);
            release_نمونہ();
        }
    } ورنہ {
        جوڑو(static_cast<UChar32>(cp));
    }
    واپس *یہ;
}

/**
 * Adds the given string, in order, to 'strings_'.  The given string
 * must have been checked by the caller to not already be in 'strings_'.
 */
خالی Unicode_سیٹ::جوڑو_2(ثابت UnicodeString& s) {
    اگر (isFrozen() || isBogus()) {
        واپس;
    }
    UError_رمز ec = U_ZERO_ERROR;
    اگر (لڑیاں_2 == خالی_اشاریہ && !allocate_لڑیاں(ec)) {
        سیٹ_مقصد_Bogus();
        واپس;
    }
    Local_اشارہ_گر<UnicodeString> t(نیا UnicodeString(s));
    اگر (t.isNull()) { // Check for memory allocation error.
        سیٹ_مقصد_Bogus();
        واپس;
    }
    لڑیاں_2->sorted_داخل_کرو(t.orphan(), compareUnicodeString, ec);
    اگر (U_FAILURE(ec)) {
        سیٹ_مقصد_Bogus();
    }
}

/**
 * @return a code point IF the string consists of a single one.
 * otherwise returns -1.
 * @param string to test
 */
int32_t Unicode_سیٹ::getSingleCP(ثابت UnicodeString& s) {
    int32_t s_لمبائی = s.length();
    اگر (s_لمبائی == 1) واپس s.charAt(0);
    اگر (s_لمبائی == 2) {
        UChar32 cp = s.char32At(0);
        اگر (cp > 0xFFFF) { // is surrogate pair
            واپس cp;
        }
    }
    واپس -1;
}

/**
 * Adds each of the characters in this string to the set. Thus "ch" => {"c", "h"}
 * If this set already any particular character, it has no effect on that character.
 * @param the source string
 * @return the modified set, for chaining
 */
Unicode_سیٹ& Unicode_سیٹ::جوڑو_All(ثابت UnicodeString& s) U_LIFETIME_BOUND {
    UChar32 cp;
    برائے (int32_t i = 0; i < s.length(); i += U16_لمبائی(cp)) {
        cp = s.char32At(i);
        جوڑو(cp);
    }
    واپس *یہ;
}

/**
 * Retains EACH of the characters in this string. Note: "ch" == {"c", "h"}
 * If this set already any particular character, it has no effect on that character.
 * @param the source string
 * @return the modified set, for chaining
 */
Unicode_سیٹ& Unicode_سیٹ::retainAll(ثابت UnicodeString& s) U_LIFETIME_BOUND {
    Unicode_سیٹ سیٹ;
    سیٹ.جوڑو_All(s);
    retainAll(سیٹ);
    واپس *یہ;
}

/**
 * Complement EACH of the characters in this string. Note: "ch" == {"c", "h"}
 * If this set already any particular character, it has no effect on that character.
 * @param the source string
 * @return the modified set, for chaining
 */
Unicode_سیٹ& Unicode_سیٹ::complementAll(ثابت UnicodeString& s) U_LIFETIME_BOUND {
    Unicode_سیٹ سیٹ;
    سیٹ.جوڑو_All(s);
    complementAll(سیٹ);
    واپس *یہ;
}

/**
 * Remove EACH of the characters in this string. Note: "ch" == {"c", "h"}
 * If this set already any particular character, it has no effect on that character.
 * @param the source string
 * @return the modified set, for chaining
 */
Unicode_سیٹ& Unicode_سیٹ::removeAll(ثابت UnicodeString& s) U_LIFETIME_BOUND {
    Unicode_سیٹ سیٹ;
    سیٹ.جوڑو_All(s);
    removeAll(سیٹ);
    واپس *یہ;
}

Unicode_سیٹ& Unicode_سیٹ::remove_All_لڑیاں() U_LIFETIME_BOUND {
    اگر (!isFrozen() && has_لڑیاں()) {
        لڑیاں_2->removeAllElements();
        release_نمونہ();
    }
    واپس *یہ;
}


/**
 * Makes a set from a multicharacter string. Thus "ch" => {"ch"}
 * <br><b>Warning: you cannot add an empty string ("") to a UnicodeSet.</b>
 * @param the source string
 * @return a newly created set containing the given string
 */
Unicode_سیٹ* U_EXPORT2 Unicode_سیٹ::createFrom(ثابت UnicodeString& s) {
    Unicode_سیٹ *سیٹ = نیا Unicode_سیٹ();
    اگر (سیٹ != خالی_اشاریہ) { // Check for memory allocation error.
        سیٹ->جوڑو(s);
    }
    واپس سیٹ;
}


/**
 * Makes a set from each of the characters in the string. Thus "ch" => {"c", "h"}
 * @param the source string
 * @return a newly created set containing the given characters
 */
Unicode_سیٹ* U_EXPORT2 Unicode_سیٹ::createFromAll(ثابت UnicodeString& s) {
    Unicode_سیٹ *سیٹ = نیا Unicode_سیٹ();
    اگر (سیٹ != خالی_اشاریہ) { // Check for memory allocation error.
        سیٹ->جوڑو_All(s);
    }
    واپس سیٹ;
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
Unicode_سیٹ& Unicode_سیٹ::retain(UChar32 آغاز, UChar32 انتہا) U_LIFETIME_BOUND {
    اگر (pin_رمز_نقطہ(آغاز) <= pin_رمز_نقطہ(انتہا)) {
        UChar32 range[3] = { آغاز, انتہا+1, UNICODESET_بلند };
        retain(range, 2, 0);
    } ورنہ {
        clear();
    }
    واپس *یہ;
}

Unicode_سیٹ& Unicode_سیٹ::retain(UChar32 c) U_LIFETIME_BOUND {
    واپس retain(c, c);
}

Unicode_سیٹ& Unicode_سیٹ::retain(ثابت UnicodeString &s) U_LIFETIME_BOUND {
    اگر (isFrozen() || isBogus()) { واپس *یہ; }
    UChar32 cp = getSingleCP(s);
    اگر (cp < 0) {
        منطقی is_اندر = لڑیاں_شامل_ہے(s);
        // Check for getRangeCount() first to avoid somewhat-expensive size()
        // when there are single code points.
        اگر (is_اندر && get_Range_گنتی() == 0 && size() == 1) {
            واپس *یہ;
        }
        clear();
        اگر (is_اندر) {
            جوڑو_2(s);
        }
    } ورنہ {
        retain(cp, cp);
    }
    واپس *یہ;
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
Unicode_سیٹ& Unicode_سیٹ::remove(UChar32 آغاز, UChar32 انتہا) U_LIFETIME_BOUND {
    اگر (pin_رمز_نقطہ(آغاز) <= pin_رمز_نقطہ(انتہا)) {
        UChar32 range[3] = { آغاز, انتہا+1, UNICODESET_بلند };
        retain(range, 2, 2);
    }
    واپس *یہ;
}

/**
 * Removes the specified character from this set if it is present.
 * The set will not contain the specified range once the call
 * returns.
 */
Unicode_سیٹ& Unicode_سیٹ::remove(UChar32 c) U_LIFETIME_BOUND {
    واپس remove(c, c);
}

/**
 * Removes the specified string from this set if it is present.
 * The set will not contain the specified character once the call
 * returns.
 * @param the source string
 * @return the modified set, for chaining
 */
Unicode_سیٹ& Unicode_سیٹ::remove(ثابت UnicodeString& s) U_LIFETIME_BOUND {
    اگر (isFrozen() || isBogus()) واپس *یہ;
    int32_t cp = getSingleCP(s);
    اگر (cp < 0) {
        اگر (لڑیاں_2 != خالی_اشاریہ && لڑیاں_2->removeElement((خالی*) &s)) {
            release_نمونہ();
        }
    } ورنہ {
        remove(static_cast<UChar32>(cp), static_cast<UChar32>(cp));
    }
    واپس *یہ;
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
Unicode_سیٹ& Unicode_سیٹ::complement(UChar32 آغاز, UChar32 انتہا) U_LIFETIME_BOUND {
    اگر (isFrozen() || isBogus()) {
        واپس *یہ;
    }
    اگر (pin_رمز_نقطہ(آغاز) <= pin_رمز_نقطہ(انتہا)) {
        UChar32 range[3] = { آغاز, انتہا+1, UNICODESET_بلند };
        exclusiveOr(range, 2, 0);
    }
    release_نمونہ();
    واپس *یہ;
}

Unicode_سیٹ& Unicode_سیٹ::complement(UChar32 c) U_LIFETIME_BOUND {
    واپس complement(c, c);
}

/**
 * This is equivalent to
 * <code>complement(MIN_VALUE, MAX_VALUE)</code>.
 */
Unicode_سیٹ& Unicode_سیٹ::complement() U_LIFETIME_BOUND {
    اگر (isFrozen() || isBogus()) {
        واپس *یہ;
    }
    اگر (list[0] == UNICODESET_کم) {
        uprv_memmove(list, list + 1, (size_t)(len-1)*حجم(UChar32));
        --len;
    } ورنہ {
        اگر (!ensure_استعداد(len+1)) {
            واپس *یہ;
        }
        uprv_memmove(list + 1, list, (size_t)len*حجم(UChar32));
        list[0] = UNICODESET_کم;
        ++len;
    }
    release_نمونہ();
    واپس *یہ;
}

/**
 * Complement the specified string in this set.
 * The set will not contain the specified string once the call
 * returns.
 *
 * @param s the string to complement
 * @return this object, for chaining
 */
Unicode_سیٹ& Unicode_سیٹ::complement(ثابت UnicodeString& s) U_LIFETIME_BOUND {
    اگر (isFrozen() || isBogus()) واپس *یہ;
    int32_t cp = getSingleCP(s);
    اگر (cp < 0) {
        اگر (لڑیاں_شامل_ہے(s)) {
            لڑیاں_2->removeElement((خالی*) &s);
        } ورنہ {
            جوڑو_2(s);
        }
        release_نمونہ();
    } ورنہ {
        complement(static_cast<UChar32>(cp), static_cast<UChar32>(cp));
    }
    واپس *یہ;
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
Unicode_سیٹ& Unicode_سیٹ::جوڑو_All(ثابت Unicode_سیٹ& c) U_LIFETIME_BOUND {
    اگر ( c.len>0 && c.list!=خالی_اشاریہ ) {
        جوڑو(c.list, c.len, 0);
    }

    // Add strings in order
    اگر ( c.لڑیاں_2!=خالی_اشاریہ ) {
        برائے (int32_t i=0; i<c.لڑیاں_2->size(); ++i) {
            ثابت UnicodeString* s = static_cast<ثابت UnicodeString*>(c.لڑیاں_2->elementAt(i));
            اگر (!لڑیاں_شامل_ہے(*s)) {
                جوڑو_2(*s);
            }
        }
    }
    واپس *یہ;
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
Unicode_سیٹ& Unicode_سیٹ::retainAll(ثابت Unicode_سیٹ& c) U_LIFETIME_BOUND {
    اگر (isFrozen() || isBogus()) {
        واپس *یہ;
    }
    retain(c.list, c.len, 0);
    اگر (has_لڑیاں()) {
        اگر (!c.has_لڑیاں()) {
            لڑیاں_2->removeAllElements();
        } ورنہ {
            لڑیاں_2->retainAll(*c.لڑیاں_2);
        }
    }
    واپس *یہ;
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
Unicode_سیٹ& Unicode_سیٹ::removeAll(ثابت Unicode_سیٹ& c) U_LIFETIME_BOUND {
    اگر (isFrozen() || isBogus()) {
        واپس *یہ;
    }
    retain(c.list, c.len, 2);
    اگر (has_لڑیاں() && c.has_لڑیاں()) {
        لڑیاں_2->removeAll(*c.لڑیاں_2);
    }
    واپس *یہ;
}

/**
 * Complements in this set all elements contained in the specified
 * set.  Any character in the other set will be removed if it is
 * in this set, or will be added if it is not in this set.
 *
 * @param c set that defines which elements will be xor'ed from
 *          this set.
 */
Unicode_سیٹ& Unicode_سیٹ::complementAll(ثابت Unicode_سیٹ& c) U_LIFETIME_BOUND {
    اگر (isFrozen() || isBogus()) {
        واپس *یہ;
    }
    exclusiveOr(c.list, c.len, 0);

    اگر (c.لڑیاں_2 != خالی_اشاریہ) {
        برائے (int32_t i=0; i<c.لڑیاں_2->size(); ++i) {
            خالی* e = c.لڑیاں_2->elementAt(i);
            اگر (لڑیاں_2 == خالی_اشاریہ || !لڑیاں_2->removeElement(e)) {
                جوڑو_2(*static_cast<ثابت UnicodeString*>(e));
            }
        }
    }
    واپس *یہ;
}

/**
 * Removes all of the elements from this set.  This set will be
 * empty after this call returns.
 */
Unicode_سیٹ& Unicode_سیٹ::clear() U_LIFETIME_BOUND {
    اگر (isFrozen()) {
        واپس *یہ;
    }
    list[0] = UNICODESET_بلند;
    len = 1;
    release_نمونہ();
    اگر (لڑیاں_2 != خالی_اشاریہ) {
        لڑیاں_2->removeAllElements();
    }
    // Remove bogus
    fFlags = 0;
    واپس *یہ;
}

/**
 * Iteration method that returns the number of ranges contained in
 * this set.
 * @see #getRangeStart
 * @see #getRangeEnd
 */
int32_t Unicode_سیٹ::get_Range_گنتی() ثابت {
    واپس len/2;
}

/**
 * Iteration method that returns the first character in the
 * specified range of this set.
 * @see #getRangeCount
 * @see #getRangeEnd
 */
UChar32 Unicode_سیٹ::get_Range_آغاز(int32_t اشاریہ) ثابت {
    واپس list[اشاریہ*2];
}

/**
 * Iteration method that returns the last character in the
 * specified range of this set.
 * @see #getRangeStart
 * @see #getRangeEnd
 */
UChar32 Unicode_سیٹ::get_Range_انتہا(int32_t اشاریہ) ثابت {
    واپس list[اشاریہ*2 + 1] - 1;
}

ثابت UnicodeString* Unicode_سیٹ::getString(int32_t اشاریہ) ثابت U_LIFETIME_BOUND {
    واپس static_cast<ثابت UnicodeString*>(لڑیاں_2->elementAt(اشاریہ));
}

/**
 * Reallocate this objects internal structures to take up the least
 * possible space, without changing this object's value.
 */
Unicode_سیٹ& Unicode_سیٹ::compact() U_LIFETIME_BOUND {
    اگر (isFrozen() || isBogus()) {
        واپس *یہ;
    }
    // Delete buffer first to defragment memory less.
    اگر (بفر != ڈھیر_List) {
        uprv_free(بفر);
        بفر = خالی_اشاریہ;
        بفر_استعداد = 0;
    }
    اگر (list == ڈھیر_List) {
        // pass
    } ورنہ اگر (len <= INITIAL_استعداد) {
        uprv_memcpy(ڈھیر_List, list, len * حجم(UChar32));
        uprv_free(list);
        list = ڈھیر_List;
        capacity = INITIAL_استعداد;
    } ورنہ اگر ((len + 7) < capacity) {
        // If we have more than a little unused capacity, shrink it to len.
        UChar32* عارضی = static_cast<UChar32*>(uprv_realloc(list, حجم(UChar32) * len));
        اگر (عارضی) {
            list = عارضی;
            capacity = len;
        }
        // else what the heck happened?! We allocated less memory!
        // Oh well. We'll keep our original array.
    }
    اگر (لڑیاں_2 != خالی_اشاریہ && لڑیاں_2->is_خالی_خانے()) {
        حذف لڑیاں_2;
        لڑیاں_2 = خالی_اشاریہ;
    }
    واپس *یہ;
}

#اگر_تعریف_شدہ DEBUG_SERIALIZE
#شامل <stdio.h>
#اگر_ختم

/**
 * Deserialize constructor.
 */
Unicode_سیٹ::Unicode_سیٹ(ثابت uint16_t ڈیٹا[], int32_t ڈیٹا_Len, ESerialization serialization,
                       UError_رمز &ec) {

  اگر(U_FAILURE(ec)) {
    سیٹ_مقصد_Bogus();
    واپس;
  }

  اگر( (serialization != kSerialized)
      || (ڈیٹا==خالی_اشاریہ)
      || (ڈیٹا_Len < 1)) {
    ec = U_ILLEGAL_دلیل_ERROR;
    سیٹ_مقصد_Bogus();
    واپس;
  }

  // bmp?
  int32_t سرخی_سائز = ((ڈیٹا[0]&0x8000)) ?2:1;
  int32_t bmp_لمبائی = (سرخی_سائز==1)?ڈیٹا[0]:ڈیٹا[1];

  int32_t new_لمبائی = (((ڈیٹا[0]&0x7FFF)-bmp_لمبائی)/2)+bmp_لمبائی;
#اگر_تعریف_شدہ DEBUG_SERIALIZE
  لکھو("dataLen %d headerSize %d bmpLen %d len %d. data[0]=%X/%X/%X/%X\n", ڈیٹا_Len,سرخی_سائز,bmp_لمبائی,new_لمبائی, ڈیٹا[0],ڈیٹا[1],ڈیٹا[2],ڈیٹا[3]);
#اگر_ختم
  اگر(!ensure_استعداد(new_لمبائی + 1)) {  // +1 for HIGH
    واپس;
  }
  // copy bmp
  int32_t i;
  برائے(i = 0; i< bmp_لمبائی;i++) {
    list[i] = ڈیٹا[i+سرخی_سائز];
#اگر_تعریف_شدہ DEBUG_SERIALIZE
    لکھو("<<16@%d[%d] %X\n", i+سرخی_سائز, i, list[i]);
#اگر_ختم
  }
  // copy smp
  برائے(i=bmp_لمبائی;i<new_لمبائی;i++) {
    list[i] = (static_cast<UChar32>(ڈیٹا[سرخی_سائز + bmp_لمبائی + (i - bmp_لمبائی) * 2 + 0]) << 16) +
               static_cast<UChar32>(ڈیٹا[سرخی_سائز + bmp_لمبائی + (i - bmp_لمبائی) * 2 + 1]);
#اگر_تعریف_شدہ DEBUG_SERIALIZE
    لکھو("<<32@%d+[%d] %lX\n", سرخی_سائز+bmp_لمبائی+i, i, list[i]);
#اگر_ختم
  }
  U_ASSERT(i == new_لمبائی);
  اگر (i == 0 || list[i - 1] != UNICODESET_بلند) {
    list[i++] = UNICODESET_بلند;
  }
  len = i;
}


int32_t Unicode_سیٹ::serialize(uint16_t *dest, int32_t dest_استعداد, UError_رمز& ec) ثابت {
    int32_t bmp_لمبائی, length, dest_لمبائی;

    اگر (U_FAILURE(ec)) {
        واپس 0;
    }

    اگر (dest_استعداد<0 || (dest_استعداد>0 && dest==خالی_اشاریہ)) {
        ec=U_ILLEGAL_دلیل_ERROR;
        واپس 0;
    }

    /* count necessary 16-bit units */
    length=یہ->len-1; // Subtract 1 to ignore final UNICODESET_HIGH
    // assert(length>=0);
    اگر (length==0) {
        /* empty set */
        اگر (dest_استعداد>0) {
            *dest=0;
        } ورنہ {
            ec=U_بفر_حد_سے_تجاوز_ERROR;
        }
        واپس 1;
    }
    /* now length>0 */

    اگر (یہ->list[length-1]<=0xffff) {
        /* all BMP */
        bmp_لمبائی=length;
    } ورنہ اگر (یہ->list[0]>=0x10000) {
        /* all supplementary */
        bmp_لمبائی=0;
        length*=2;
    } ورنہ {
        /* some BMP, some supplementary */
        برائے (bmp_لمبائی=0; bmp_لمبائی<length && یہ->list[bmp_لمبائی]<=0xffff; ++bmp_لمبائی) {}
        length=bmp_لمبائی+2*(length-bmp_لمبائی);
    }
#اگر_تعریف_شدہ DEBUG_SERIALIZE
    لکھو(">> bmpLength%d length%d len%d\n", bmp_لمبائی, length, len);
#اگر_ختم
    /* length: number of 16-bit array units */
    اگر (length>0x7fff) {
        /* there are only 15 bits for the length in the first serialized word */
        ec=U_اشاریہ_OUTOFBOUNDS_ERROR;
        واپس 0;
    }

    /*
     * total serialized length:
     * number of 16-bit array units (length) +
     * 1 length unit (always) +
     * 1 bmpLength unit (if there are supplementary values)
     */
    dest_لمبائی=length+((length>bmp_لمبائی)?2:1);
    اگر (dest_لمبائی<=dest_استعداد) {
        ثابت UChar32 *p;
        int32_t i;

#اگر_تعریف_شدہ DEBUG_SERIALIZE
        لکھو("writeHdr\n");
#اگر_ختم
        *dest = static_cast<uint16_t>(length);
        اگر (length>bmp_لمبائی) {
            *dest|=0x8000;
            *++dest = static_cast<uint16_t>(bmp_لمبائی);
        }
        ++dest;

        /* write the BMP part of the array */
        p=یہ->list;
        برائے (i=0; i<bmp_لمبائی; ++i) {
#اگر_تعریف_شدہ DEBUG_SERIALIZE
          لکھو("writebmp: %x\n", (صحیح)*p);
#اگر_ختم
            *dest++ = static_cast<uint16_t>(*p++);
        }

        /* write the supplementary part of the array */
        برائے (; i<length; i+=2) {
#اگر_تعریف_شدہ DEBUG_SERIALIZE
          لکھو("write32: %x\n", (صحیح)*p);
#اگر_ختم
            *dest++ = static_cast<uint16_t>(*p >> 16);
            *dest++ = static_cast<uint16_t>(*p++);
        }
    } ورنہ {
        ec=U_بفر_حد_سے_تجاوز_ERROR;
    }
    واپس dest_لمبائی;
}

//----------------------------------------------------------------
// Implementation: Utility methods
//----------------------------------------------------------------

/**
 * Allocate our strings vector and return true if successful.
 */
UBool Unicode_سیٹ::allocate_لڑیاں(UError_رمز &حالت_2) {
    اگر (U_FAILURE(حالت_2)) {
        واپس جھوٹ;
    }
    لڑیاں_2 = نیا UVector(uprv_deleteUObject,
                          uhash_compareUnicodeString, 1, حالت_2);
    اگر (لڑیاں_2 == خالی_اشاریہ) { // Check for memory allocation error.
        حالت_2 = U_حافظہ_تخصیص_ERROR;
        واپس جھوٹ;
    }
    اگر (U_FAILURE(حالت_2)) {
        حذف لڑیاں_2;
        لڑیاں_2 = خالی_اشاریہ;
        واپس جھوٹ;
    } 
    واپس سچ;
}

int32_t Unicode_سیٹ::اگلا_استعداد(int32_t min_استعداد) {
    // Grow exponentially to reduce the frequency of allocations.
    اگر (min_استعداد < INITIAL_استعداد) {
        واپس min_استعداد + INITIAL_استعداد;
    } ورنہ اگر (min_استعداد <= 2500) {
        واپس 5 * min_استعداد;
    } ورنہ {
        int32_t new_استعداد = 2 * min_استعداد;
        اگر (new_استعداد > MAX_لمبائی) {
            new_استعداد = MAX_لمبائی;
        }
        واپس new_استعداد;
    }
}

منطقی Unicode_سیٹ::ensure_استعداد(int32_t newLen) {
    اگر (newLen > MAX_لمبائی) {
        newLen = MAX_لمبائی;
    }
    اگر (newLen <= capacity) {
        واپس سچ;
    }
    int32_t new_استعداد = اگلا_استعداد(newLen);
    UChar32* عارضی = static_cast<UChar32*>(uprv_malloc(new_استعداد * حجم(UChar32)));
    اگر (عارضی == خالی_اشاریہ) {
        سیٹ_مقصد_Bogus(); // set the object to bogus state if an OOM failure occurred.
        واپس جھوٹ;
    }
    // Copy only the actual contents.
    uprv_memcpy(عارضی, list, len * حجم(UChar32));
    اگر (list != ڈھیر_List) {
        uprv_free(list);
    }
    list = عارضی;
    capacity = new_استعداد;
    واپس سچ;
}

منطقی Unicode_سیٹ::ensure_بفر_استعداد(int32_t newLen) {
    اگر (newLen > MAX_لمبائی) {
        newLen = MAX_لمبائی;
    }
    اگر (newLen <= بفر_استعداد) {
        واپس سچ;
    }
    int32_t new_استعداد = اگلا_استعداد(newLen);
    UChar32* عارضی = static_cast<UChar32*>(uprv_malloc(new_استعداد * حجم(UChar32)));
    اگر (عارضی == خالی_اشاریہ) {
        سیٹ_مقصد_Bogus();
        واپس جھوٹ;
    }
    // The buffer has no contents to be copied.
    // It is always filled from scratch after this call.
    اگر (بفر != ڈھیر_List) {
        uprv_free(بفر);
    }
    بفر = عارضی;
    بفر_استعداد = new_استعداد;
    واپس سچ;
}

/**
 * Swap list and buffer.
 */
خالی Unicode_سیٹ::تبادلہ_Buffers() {
    // swap list and buffer
    UChar32* عارضی = list;
    list = بفر;
    بفر = عارضی;

    int32_t c = capacity;
    capacity = بفر_استعداد;
    بفر_استعداد = c;
}

خالی Unicode_سیٹ::سیٹ_مقصد_Bogus() {
    clear(); // Remove everything in the set.
    fFlags = kIsBogus;
}

//----------------------------------------------------------------
// Implementation: Fundamental operators
//----------------------------------------------------------------

ساکن اندرونی UChar32 max(UChar32 a, UChar32 b) {
    واپس (a > b) ? a : b;
}

// polarity = 0, 3 is normal: x xor y
// polarity = 1, 2: x xor ~y == x === y

خالی Unicode_سیٹ::exclusiveOr(ثابت UChar32* other, int32_t otherLen, int8_t polarity) {
    اگر (isFrozen() || isBogus()) {
        واپس;
    }
    اگر (!ensure_بفر_استعداد(len + otherLen)) {
        واپس;
    }

    int32_t i = 0, j = 0, k = 0;
    UChar32 a = list[i++];
    UChar32 b;
    اگر (polarity == 1 || polarity == 2) {
        b = UNICODESET_کم;
        اگر (other[j] == UNICODESET_کم) { // skip base if already LOW
            ++j;
            b = other[j];
        }
    } ورنہ {
        b = other[j++];
    }
    // simplest of all the routines
    // sort the values, discarding identicals!
    برائے (;;) {
        اگر (a < b) {
            بفر[k++] = a;
            a = list[i++];
        } ورنہ اگر (b < a) {
            بفر[k++] = b;
            b = other[j++];
        } ورنہ اگر (a != UNICODESET_بلند) { // at this point, a == b
            // discard both values!
            a = list[i++];
            b = other[j++];
        } ورنہ { // DONE!
            بفر[k++] = UNICODESET_بلند;
            len = k;
            توڑو;
        }
    }
    تبادلہ_Buffers();
    release_نمونہ();
}

// polarity = 0 is normal: x union y
// polarity = 2: x union ~y
// polarity = 1: ~x union y
// polarity = 3: ~x union ~y

خالی Unicode_سیٹ::جوڑو(ثابت UChar32* other, int32_t otherLen, int8_t polarity) {
    اگر (isFrozen() || isBogus() || other==خالی_اشاریہ) {
        واپس;
    }
    اگر (!ensure_بفر_استعداد(len + otherLen)) {
        واپس;
    }

    int32_t i = 0, j = 0, k = 0;
    UChar32 a = list[i++];
    UChar32 b = other[j++];
    // change from xor is that we have to check overlapping pairs
    // polarity bit 1 means a is second, bit 2 means b is.
    برائے (;;) {
        انتخاب (polarity) {
          حالت 0: // both first; take lower if unequal
            اگر (a < b) { // take a
                // Back up over overlapping ranges in buffer[]
                اگر (k > 0 && a <= بفر[k-1]) {
                    // Pick latter end value in buffer[] vs. list[]
                    a = max(list[i], بفر[--k]);
                } ورنہ {
                    // No overlap
                    بفر[k++] = a;
                    a = list[i];
                }
                i++; // Common if/else code factored out
                polarity ^= 1;
            } ورنہ اگر (b < a) { // take b
                اگر (k > 0 && b <= بفر[k-1]) {
                    b = max(other[j], بفر[--k]);
                } ورنہ {
                    بفر[k++] = b;
                    b = other[j];
                }
                j++;
                polarity ^= 2;
            } ورنہ { // a == b, take a, drop b
                اگر (a == UNICODESET_بلند) جاؤ loop_انتہا;
                // This is symmetrical; it doesn't matter if
                // we backtrack with a or b. - liu
                اگر (k > 0 && a <= بفر[k-1]) {
                    a = max(list[i], بفر[--k]);
                } ورنہ {
                    // No overlap
                    بفر[k++] = a;
                    a = list[i];
                }
                i++;
                polarity ^= 1;
                b = other[j++];
                polarity ^= 2;
            }
            توڑو;
          حالت 3: // both second; take higher if unequal, and drop other
            اگر (b <= a) { // take a
                اگر (a == UNICODESET_بلند) جاؤ loop_انتہا;
                بفر[k++] = a;
            } ورنہ { // take b
                اگر (b == UNICODESET_بلند) جاؤ loop_انتہا;
                بفر[k++] = b;
            }
            a = list[i++];
            polarity ^= 1;   // factored common code
            b = other[j++];
            polarity ^= 2;
            توڑو;
          حالت 1: // a second, b first; if b < a, overlap
            اگر (a < b) { // no overlap, take a
                بفر[k++] = a; a = list[i++]; polarity ^= 1;
            } ورنہ اگر (b < a) { // OVERLAP, drop b
                b = other[j++];
                polarity ^= 2;
            } ورنہ { // a == b, drop both!
                اگر (a == UNICODESET_بلند) جاؤ loop_انتہا;
                a = list[i++];
                polarity ^= 1;
                b = other[j++];
                polarity ^= 2;
            }
            توڑو;
          حالت 2: // a first, b second; if a < b, overlap
            اگر (b < a) { // no overlap, take b
                بفر[k++] = b;
                b = other[j++];
                polarity ^= 2;
            } ورنہ  اگر (a < b) { // OVERLAP, drop a
                a = list[i++];
                polarity ^= 1;
            } ورنہ { // a == b, drop both!
                اگر (a == UNICODESET_بلند) جاؤ loop_انتہا;
                a = list[i++];
                polarity ^= 1;
                b = other[j++];
                polarity ^= 2;
            }
            توڑو;
        }
    }
 loop_انتہا:
    بفر[k++] = UNICODESET_بلند;    // terminate
    len = k;
    تبادلہ_Buffers();
    release_نمونہ();
}

// polarity = 0 is normal: x intersect y
// polarity = 2: x intersect ~y == set-minus
// polarity = 1: ~x intersect y
// polarity = 3: ~x intersect ~y

خالی Unicode_سیٹ::retain(ثابت UChar32* other, int32_t otherLen, int8_t polarity) {
    اگر (isFrozen() || isBogus()) {
        واپس;
    }
    اگر (!ensure_بفر_استعداد(len + otherLen)) {
        واپس;
    }

    int32_t i = 0, j = 0, k = 0;
    UChar32 a = list[i++];
    UChar32 b = other[j++];
    // change from xor is that we have to check overlapping pairs
    // polarity bit 1 means a is second, bit 2 means b is.
    برائے (;;) {
        انتخاب (polarity) {
          حالت 0: // both first; drop the smaller
            اگر (a < b) { // drop a
                a = list[i++];
                polarity ^= 1;
            } ورنہ اگر (b < a) { // drop b
                b = other[j++];
                polarity ^= 2;
            } ورنہ { // a == b, take one, drop other
                اگر (a == UNICODESET_بلند) جاؤ loop_انتہا;
                بفر[k++] = a;
                a = list[i++];
                polarity ^= 1;
                b = other[j++];
                polarity ^= 2;
            }
            توڑو;
          حالت 3: // both second; take lower if unequal
            اگر (a < b) { // take a
                بفر[k++] = a;
                a = list[i++];
                polarity ^= 1;
            } ورنہ اگر (b < a) { // take b
                بفر[k++] = b;
                b = other[j++];
                polarity ^= 2;
            } ورنہ { // a == b, take one, drop other
                اگر (a == UNICODESET_بلند) جاؤ loop_انتہا;
                بفر[k++] = a;
                a = list[i++];
                polarity ^= 1;
                b = other[j++];
                polarity ^= 2;
            }
            توڑو;
          حالت 1: // a second, b first;
            اگر (a < b) { // NO OVERLAP, drop a
                a = list[i++];
                polarity ^= 1;
            } ورنہ اگر (b < a) { // OVERLAP, take b
                بفر[k++] = b;
                b = other[j++];
                polarity ^= 2;
            } ورنہ { // a == b, drop both!
                اگر (a == UNICODESET_بلند) جاؤ loop_انتہا;
                a = list[i++];
                polarity ^= 1;
                b = other[j++];
                polarity ^= 2;
            }
            توڑو;
          حالت 2: // a first, b second; if a < b, overlap
            اگر (b < a) { // no overlap, drop b
                b = other[j++];
                polarity ^= 2;
            } ورنہ  اگر (a < b) { // OVERLAP, take a
                بفر[k++] = a;
                a = list[i++];
                polarity ^= 1;
            } ورنہ { // a == b, drop both!
                اگر (a == UNICODESET_بلند) جاؤ loop_انتہا;
                a = list[i++];
                polarity ^= 1;
                b = other[j++];
                polarity ^= 2;
            }
            توڑو;
        }
    }
 loop_انتہا:
    بفر[k++] = UNICODESET_بلند;    // terminate
    len = k;
    تبادلہ_Buffers();
    release_نمونہ();
}

/**
 * Append the <code>toPattern()</code> representation of a
 * string to the given <code>StringBuffer</code>.
 */
خالی Unicode_سیٹ::append_مقصد_Pat(UnicodeString& buf, ثابت UnicodeString& s, UBool escapeUnprintable) {
    UChar32 cp;
    برائے (int32_t i = 0; i < s.length(); i += U16_لمبائی(cp)) {
        append_مقصد_Pat(buf, cp = s.char32At(i), escapeUnprintable);
    }
}

/**
 * Append the <code>toPattern()</code> representation of a
 * character to the given <code>StringBuffer</code>.
 */
خالی Unicode_سیٹ::append_مقصد_Pat(UnicodeString& buf, UChar32 c, UBool escapeUnprintable) {
    اگر (escapeUnprintable ? ICU_Utility::isUnprintable(c) : ICU_Utility::shouldAlwaysBeEscaped(c)) {
        // Use hex escape notation (\uxxxx or \Uxxxxxxxx) for anything
        // unprintable
        ICU_Utility::escape(buf, c);
        واپس;
    }
    // Okay to let ':' pass through
    انتخاب (c) {
    حالت u'[':
    حالت u']':
    حالت u'-':
    حالت u'^':
    حالت u'&':
    حالت u'\\':
    حالت u'{':
    حالت u'}':
    حالت u':':
    حالت علامت_جدول::علامت_REF:
        buf.append(u'\\');
        توڑو;
    طےشدہ:
        // Escape whitespace
        اگر (نمونہ_Props::isWhiteSpace(c)) {
            buf.append(u'\\');
        }
        توڑو;
    }
    buf.append(c);
}

خالی Unicode_سیٹ::append_مقصد_Pat(UnicodeString &نتیجہ, UChar32 آغاز, UChar32 انتہا,
                              UBool escapeUnprintable) {
    append_مقصد_Pat(نتیجہ, آغاز, escapeUnprintable);
    اگر (آغاز != انتہا) {
        اگر ((آغاز+1) != انتہا ||
                // Avoid writing what looks like a lead+trail surrogate pair.
                آغاز == 0xdbff) {
            نتیجہ.append(u'-');
        }
        append_مقصد_Pat(نتیجہ, انتہا, escapeUnprintable);
    }
}

/**
 * Append a string representation of this set to result.  This will be
 * a cleaned version of the string passed to applyPattern(), if there
 * is one.  Otherwise it will be generated.
 */
UnicodeString& Unicode_سیٹ::مقصد_نمونہ_2(UnicodeString& نتیجہ U_LIFETIME_BOUND,
                                      UBool escapeUnprintable) ثابت
{
    اگر (pat != خالی_اشاریہ) {
        int32_t i;
        int32_t backslash_گنتی = 0;
        برائے (i=0; i<patLen; ) {
            UChar32 c;
            U16_اگلا(pat, i, patLen, c);
            اگر (escapeUnprintable ?
                    ICU_Utility::isUnprintable(c) : ICU_Utility::shouldAlwaysBeEscaped(c)) {
                // If the unprintable character is preceded by an odd
                // number of backslashes, then it has been escaped.
                // Before unescaping it, we delete the final
                // backslash.
                اگر ((backslash_گنتی % 2) == 1) {
                    نتیجہ.truncate(نتیجہ.length() - 1);
                }
                ICU_Utility::escape(نتیجہ, c);
                backslash_گنتی = 0;
            } ورنہ {
                نتیجہ.append(c);
                اگر (c == u'\\') {
                    ++backslash_گنتی;
                } ورنہ {
                    backslash_گنتی = 0;
                }
            }
        }
        واپس نتیجہ;
    }

    واپس generate_نمونہ(نتیجہ, escapeUnprintable);
}

/**
 * Returns a string representation of this set.  If the result of
 * calling this function is passed to a UnicodeSet constructor, it
 * will produce another set that is equal to this one.
 */
UnicodeString& Unicode_سیٹ::مقصد_نمونہ(UnicodeString& نتیجہ U_LIFETIME_BOUND,
                                     UBool escapeUnprintable) ثابت
{
    نتیجہ.truncate(0);
    واپس مقصد_نمونہ_2(نتیجہ, escapeUnprintable);
}

/**
 * Generate and append a string representation of this set to result.
 * This does not use this.pat, the cleaned up copy of the string
 * passed to applyPattern().
 */
UnicodeString& Unicode_سیٹ::generate_نمونہ(UnicodeString& نتیجہ U_LIFETIME_BOUND,
                                            UBool escapeUnprintable) ثابت
{
    نتیجہ.append(u'[');

    int32_t i = 0;
    int32_t حد_بندی = len & ~1;  // = 2 * getRangeCount()

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
    اگر (len >= 4 && list[0] == 0 && حد_بندی == len && !has_لڑیاں()) {
        // Emit the inverse
        نتیجہ.append(u'^');
        // Offsetting the inversion list index by one lets us
        // iterate over the ranges of the set complement.
        i = 1;
        --حد_بندی;
    }

    // Emit the ranges as pairs.
    جب (i < حد_بندی) {
        UChar32 آغاز = list[i];  // getRangeStart()
        UChar32 انتہا = list[i + 1] - 1;  // getRangeEnd() = range limit minus one
        اگر (!(0xd800 <= انتہا && انتہا <= 0xdbff)) {
            append_مقصد_Pat(نتیجہ, آغاز, انتہا, escapeUnprintable);
            i += 2;
        } ورنہ {
            // The range ends with a lead surrogate.
            // Avoid writing what looks like a lead+trail surrogate pair.
            // 1. Postpone ranges that start with a lead surrogate code point.
            int32_t پہلا_Lead = i;
            جب ((i += 2) < حد_بندی && list[i] <= 0xdbff) {}
            int32_t پہلا_After_Lead = i;
            // 2. Write following ranges that start with a trail surrogate code point.
            جب (i < حد_بندی && (آغاز = list[i]) <= 0xdfff) {
                append_مقصد_Pat(نتیجہ, آغاز, list[i + 1] - 1, escapeUnprintable);
                i += 2;
            }
            // 3. Now write the postponed ranges.
            برائے (صحیح j = پہلا_Lead; j < پہلا_After_Lead; j += 2) {
                append_مقصد_Pat(نتیجہ, list[j], list[j + 1] - 1, escapeUnprintable);
            }
        }
    }

    اگر (لڑیاں_2 != خالی_اشاریہ) {
        برائے (int32_t i = 0; i<لڑیاں_2->size(); ++i) {
            نتیجہ.append(u'{');
            append_مقصد_Pat(نتیجہ,
                         *static_cast<ثابت UnicodeString*>(لڑیاں_2->elementAt(i)),
                         escapeUnprintable);
            نتیجہ.append(u'}');
        }
    }
    واپس نتیجہ.append(u']');
}

/**
* Release existing cached pattern
*/
خالی Unicode_سیٹ::release_نمونہ() {
    اگر (pat) {
        uprv_free(pat);
        pat = خالی_اشاریہ;
        patLen = 0;
    }
}

/**
* Set the new pattern to cache.
*/
خالی Unicode_سیٹ::سیٹ_نمونہ(ثابت char16_t *newPat, int32_t newPatLen) {
    release_نمونہ();
    pat = static_cast<char16_t*>(uprv_malloc((newPatLen + 1) * حجم(char16_t)));
    اگر (pat) {
        patLen = newPatLen;
        u_memcpy(pat, newPat, patLen);
        pat[patLen] = 0;
    }
    // else we don't care if malloc failed. This was just a nice cache.
    // We can regenerate an equivalent pattern later when requested.
}

Unicode_سیٹ* Unicode_سیٹ::freeze() U_LIFETIME_BOUND {
    اگر(!isFrozen() && !isBogus()) {
        compact();

        // Optimize contains() and span() and similar functions.
        اگر (has_لڑیاں()) {
            string_وسعت = نیا Unicode_سیٹ_String_وسعت(*یہ, *لڑیاں_2, Unicode_سیٹ_String_وسعت::ALL);
            اگر (string_وسعت == خالی_اشاریہ) {
                سیٹ_مقصد_Bogus();
                واپس یہ;
            } ورنہ اگر (!string_وسعت->needs_String_وسعت_UTF16()) {
                // All strings are irrelevant for span() etc. because
                // all of each string's code points are contained in this set.
                // Do not check needsStringSpanUTF8() because UTF-8 has at most as
                // many relevant strings as UTF-16.
                // (Thus needsStringSpanUTF8() implies needsStringSpanUTF16().)
                حذف string_وسعت;
                string_وسعت = خالی_اشاریہ;
            }
        }
        اگر (string_وسعت == خالی_اشاریہ) {
            // No span-relevant strings: Optimize for code point spans.
            bmp_سیٹ=نیا BMPSet(list, len);
            اگر (bmp_سیٹ == خالی_اشاریہ) { // Check for memory allocation error.
                سیٹ_مقصد_Bogus();
            }
        }
    }
    واپس یہ;
}

int32_t Unicode_سیٹ::وسعت(ثابت char16_t *s, int32_t length, USet_وسعت_Condition وسعت_Condition) ثابت {
    اگر(length>0 && bmp_سیٹ!=خالی_اشاریہ) {
        واپس static_cast<int32_t>(bmp_سیٹ->وسعت(s, s + length, وسعت_Condition) - s);
    }
    اگر(length<0) {
        length=u_strlen(s);
    }
    اگر(length==0) {
        واپس 0;
    }
    اگر(string_وسعت!=خالی_اشاریہ) {
        واپس string_وسعت->وسعت(s, length, وسعت_Condition);
    } ورنہ اگر(has_لڑیاں()) {
        uint32_t which= وسعت_Condition==USET_وسعت_NOT_CONTAINED ?
                            Unicode_سیٹ_String_وسعت::FWD_UTF16_NOT_CONTAINED :
                            Unicode_سیٹ_String_وسعت::FWD_UTF16_CONTAINED;
        Unicode_سیٹ_String_وسعت str_وسعت(*یہ, *لڑیاں_2, which);
        اگر(str_وسعت.needs_String_وسعت_UTF16()) {
            واپس str_وسعت.وسعت(s, length, وسعت_Condition);
        }
    }

    اگر(وسعت_Condition!=USET_وسعت_NOT_CONTAINED) {
        وسعت_Condition=USET_وسعت_CONTAINED;  // Pin to 0/1 values.
    }

    UChar32 c;
    int32_t آغاز=0, پچھلا=0;
    کرو {
        U16_اگلا(s, آغاز, length, c);
        اگر(وسعت_Condition!=شامل_ہے(c)) {
            توڑو;
        }
    } جب((پچھلا=آغاز)<length);
    واپس پچھلا;
}

int32_t Unicode_سیٹ::وسعت_عقب(ثابت char16_t *s, int32_t length, USet_وسعت_Condition وسعت_Condition) ثابت {
    اگر(length>0 && bmp_سیٹ!=خالی_اشاریہ) {
        واپس static_cast<int32_t>(bmp_سیٹ->وسعت_عقب(s, s + length, وسعت_Condition) - s);
    }
    اگر(length<0) {
        length=u_strlen(s);
    }
    اگر(length==0) {
        واپس 0;
    }
    اگر(string_وسعت!=خالی_اشاریہ) {
        واپس string_وسعت->وسعت_عقب(s, length, وسعت_Condition);
    } ورنہ اگر(has_لڑیاں()) {
        uint32_t which= وسعت_Condition==USET_وسعت_NOT_CONTAINED ?
                            Unicode_سیٹ_String_وسعت::عقب_UTF16_NOT_CONTAINED :
                            Unicode_سیٹ_String_وسعت::عقب_UTF16_CONTAINED;
        Unicode_سیٹ_String_وسعت str_وسعت(*یہ, *لڑیاں_2, which);
        اگر(str_وسعت.needs_String_وسعت_UTF16()) {
            واپس str_وسعت.وسعت_عقب(s, length, وسعت_Condition);
        }
    }

    اگر(وسعت_Condition!=USET_وسعت_NOT_CONTAINED) {
        وسعت_Condition=USET_وسعت_CONTAINED;  // Pin to 0/1 values.
    }

    UChar32 c;
    int32_t پچھلا=length;
    کرو {
        U16_پچھلا(s, 0, length, c);
        اگر(وسعت_Condition!=شامل_ہے(c)) {
            توڑو;
        }
    } جب((پچھلا=length)>0);
    واپس پچھلا;
}

int32_t Unicode_سیٹ::وسعت_UTF8(ثابت حرف *s, int32_t length, USet_وسعت_Condition وسعت_Condition) ثابت {
    اگر(length>0 && bmp_سیٹ!=خالی_اشاریہ) {
        ثابت uint8_t* s0 = reinterpret_cast<ثابت uint8_t*>(s);
        واپس static_cast<int32_t>(bmp_سیٹ->وسعت_UTF8(s0, length, وسعت_Condition) - s0);
    }
    اگر(length<0) {
        length = static_cast<int32_t>(uprv_strlen(s));
    }
    اگر(length==0) {
        واپس 0;
    }
    اگر(string_وسعت!=خالی_اشاریہ) {
        واپس string_وسعت->وسعت_UTF8(reinterpret_cast<ثابت uint8_t*>(s), length, وسعت_Condition);
    } ورنہ اگر(has_لڑیاں()) {
        uint32_t which= وسعت_Condition==USET_وسعت_NOT_CONTAINED ?
                            Unicode_سیٹ_String_وسعت::FWD_UTF8_NOT_CONTAINED :
                            Unicode_سیٹ_String_وسعت::FWD_UTF8_CONTAINED;
        Unicode_سیٹ_String_وسعت str_وسعت(*یہ, *لڑیاں_2, which);
        اگر(str_وسعت.needs_String_وسعت_UTF8()) {
            واپس str_وسعت.وسعت_UTF8(reinterpret_cast<ثابت uint8_t*>(s), length, وسعت_Condition);
        }
    }

    اگر(وسعت_Condition!=USET_وسعت_NOT_CONTAINED) {
        وسعت_Condition=USET_وسعت_CONTAINED;  // Pin to 0/1 values.
    }

    UChar32 c;
    int32_t آغاز=0, پچھلا=0;
    کرو {
        U8_اگلا_OR_FFFD(s, آغاز, length, c);
        اگر(وسعت_Condition!=شامل_ہے(c)) {
            توڑو;
        }
    } جب((پچھلا=آغاز)<length);
    واپس پچھلا;
}

int32_t Unicode_سیٹ::وسعت_عقب_UTF8(ثابت حرف *s, int32_t length, USet_وسعت_Condition وسعت_Condition) ثابت {
    اگر(length>0 && bmp_سیٹ!=خالی_اشاریہ) {
        ثابت uint8_t* s0 = reinterpret_cast<ثابت uint8_t*>(s);
        واپس bmp_سیٹ->وسعت_عقب_UTF8(s0, length, وسعت_Condition);
    }
    اگر(length<0) {
        length = static_cast<int32_t>(uprv_strlen(s));
    }
    اگر(length==0) {
        واپس 0;
    }
    اگر(string_وسعت!=خالی_اشاریہ) {
        واپس string_وسعت->وسعت_عقب_UTF8(reinterpret_cast<ثابت uint8_t*>(s), length, وسعت_Condition);
    } ورنہ اگر(has_لڑیاں()) {
        uint32_t which= وسعت_Condition==USET_وسعت_NOT_CONTAINED ?
                            Unicode_سیٹ_String_وسعت::عقب_UTF8_NOT_CONTAINED :
                            Unicode_سیٹ_String_وسعت::عقب_UTF8_CONTAINED;
        Unicode_سیٹ_String_وسعت str_وسعت(*یہ, *لڑیاں_2, which);
        اگر(str_وسعت.needs_String_وسعت_UTF8()) {
            واپس str_وسعت.وسعت_عقب_UTF8(reinterpret_cast<ثابت uint8_t*>(s), length, وسعت_Condition);
        }
    }

    اگر(وسعت_Condition!=USET_وسعت_NOT_CONTAINED) {
        وسعت_Condition=USET_وسعت_CONTAINED;  // Pin to 0/1 values.
    }

    UChar32 c;
    int32_t پچھلا=length;
    کرو {
        U8_پچھلا_OR_FFFD(s, 0, length, c);
        اگر(وسعت_Condition!=شامل_ہے(c)) {
            توڑو;
        }
    } جب((پچھلا=length)>0);
    واپس پچھلا;
}

U_NAMESPACE_انتہا
