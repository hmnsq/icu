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
#تعریف_کن UNICODESET_بالا 0x0110000

// LOW <= all valid values. ZERO for codepoints
#تعریف_کن UNICODESET_کم 0x000000

/** Max list [0, 1, 2, ..., max code point, HIGH] */
constexpr int32_t MAX_درازی = UNICODESET_بالا + 1;

U_NAMESPACE_BEGIN

نماد_جدول::~نماد_جدول() {}

UOBJECT_DEFINE_RTTI_IMPLEMENTATION(Unicode_مجموعه)

/**
 * Modify the given UChar32 variable so that it is in range, by
 * pinning values < UNICODESET_LOW to UNICODESET_LOW, and
 * pinning values > UNICODESET_HIGH-1 to UNICODESET_HIGH-1.
 * It modifies its argument in-place and also returns it.
 */
ساكن مضمن UChar32 pin_کد_نقطه(UChar32& c) {
    إذا (c < UNICODESET_کم) {
        c = UNICODESET_کم;
    } وإلا إذا (c > (UNICODESET_بالا-1)) {
        c = (UNICODESET_بالا-1);
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

#اگر_تعریف_شده DEBUG_MEM
#ضمن <stdio.h>
ساكن int32_t dbg_شمار = 0;

ساكن مضمن فراغ _dbgct(Unicode_مجموعه* مجموعه) {
    UnicodeString str;
    مجموعه->مقصد_الگو(str, درست);
    حرف buf[40];
    str.extract(0, 39, buf, "");
    چاپ_کن("DEBUG UnicodeSet: ct 0x%08X; %d %s\n", مجموعه, ++dbg_شمار, buf);
}

ساكن مضمن فراغ _dbgdt(Unicode_مجموعه* مجموعه) {
    UnicodeString str;
    مجموعه->مقصد_الگو(str, درست);
    حرف buf[40];
    str.extract(0, 39, buf, "");
    چاپ_کن("DEBUG UnicodeSet: dt 0x%08X; %d %s\n", مجموعه, --dbg_شمار, buf);
}

#وإلا

#تعریف_کن _dbgct(مجموعه)
#تعریف_کن _dbgdt(مجموعه)

#پایان_اگر

//----------------------------------------------------------------
// UnicodeString in UVector support
//----------------------------------------------------------------

ساكن فراغ U_CALLCONV cloneUnicodeString(UElement *dst, UElement *src) {
    dst->اشاره_گر = جديد UnicodeString(*static_cast<UnicodeString*>(src->اشاره_گر));
}

ساكن int32_t U_CALLCONV compareUnicodeString(UElement t1, UElement t2) {
    ثابت UnicodeString& a = *static_cast<ثابت UnicodeString*>(t1.اشاره_گر);
    ثابت UnicodeString& b = *static_cast<ثابت UnicodeString*>(t2.اشاره_گر);
    أرجع a.compare(b);
}

UBool Unicode_مجموعه::has_رشتهها() ثابت {
    أرجع رشتهها_2 != اشاره_تهی && !رشتهها_2->is_خالی();
}

int32_t Unicode_مجموعه::رشتهها_اندازه() ثابت {
    أرجع رشتهها_2 == اشاره_تهی ? 0 : رشتهها_2->size();
}

UBool Unicode_مجموعه::رشتهها_شامل_است(ثابت UnicodeString &s) ثابت {
    أرجع رشتهها_2 != اشاره_تهی && رشتهها_2->شامل_است((فراغ*) &s);
}

//----------------------------------------------------------------
// Constructors &c
//----------------------------------------------------------------

/**
 * Constructs an empty set.
 */
Unicode_مجموعه::Unicode_مجموعه() {
    list[0] = UNICODESET_بالا;
    _dbgct(هذا);
}

/**
 * Constructs a set containing the given range. If <code>end >
 * start</code> then an empty set is created.
 *
 * @param start first character, inclusive, of range
 * @param end last character, inclusive, of range
 */
Unicode_مجموعه::Unicode_مجموعه(UChar32 شروع, UChar32 انتها) {
    list[0] = UNICODESET_بالا;
    جمع_کن(شروع, انتها);
    _dbgct(هذا);
}

/**
 * Constructs a set that is identical to the given UnicodeSet.
 */
Unicode_مجموعه::Unicode_مجموعه(ثابت Unicode_مجموعه& o) : UnicodeFilter(o) {
    *هذا = o;
    _dbgct(هذا);
}

// Copy-construct as thawed.
Unicode_مجموعه::Unicode_مجموعه(ثابت Unicode_مجموعه& o, UBool /* asThawed */) : UnicodeFilter(o) {
    إذا (ensure_ظرفیت(o.len)) {
        // *this = o except for bmpSet and stringSpan
        len = o.len;
        uprv_memcpy(list, o.list, (size_t)len*حجم(UChar32));
        إذا (o.has_رشتهها()) {
            UError_کد وضعیت = U_ZERO_ERROR;
            إذا (!allocate_رشتهها(وضعیت) ||
                    (رشتهها_2->assign(*o.رشتهها_2, cloneUnicodeString, وضعیت), U_FAILURE(وضعیت))) {
                مجموعه_مقصد_Bogus();
                أرجع;
            }
        }
        إذا (o.pat) {
            مجموعه_الگو(o.pat, o.patLen);
        }
        _dbgct(هذا);
    }
}

/**
 * Destructs the set.
 */
Unicode_مجموعه::~Unicode_مجموعه() {
    _dbgdt(هذا); // first!
    إذا (list != پشته_List) {
        uprv_free(list);
    }
    حذف bmp_مجموعه;
    إذا (میانگیر != پشته_List) {
        uprv_free(میانگیر);
    }
    حذف رشتهها_2;
    حذف string_بازه;
    release_الگو();
}

/**
 * Assigns this object to be a copy of another.
 */
Unicode_مجموعه& Unicode_مجموعه::عملگر=(ثابت Unicode_مجموعه& o) U_LIFETIME_BOUND {
    أرجع copyFrom(o, خطأ);
}

Unicode_مجموعه& Unicode_مجموعه::copyFrom(ثابت Unicode_مجموعه& o, UBool asThawed) U_LIFETIME_BOUND {
    إذا (هذا == &o) {
        أرجع *هذا;
    }
    إذا (isFrozen()) {
        أرجع *هذا;
    }
    إذا (o.isBogus()) {
        مجموعه_مقصد_Bogus();
        أرجع *هذا;
    }
    إذا (!ensure_ظرفیت(o.len)) {
        // ensureCapacity will mark the UnicodeSet as Bogus if OOM failure happens.
        أرجع *هذا;
    }
    len = o.len;
    uprv_memcpy(list, o.list, (size_t)len*حجم(UChar32));
    إذا (o.bmp_مجموعه != اشاره_تهی && !asThawed) {
        bmp_مجموعه = جديد BMPSet(*o.bmp_مجموعه, list, len);
        إذا (bmp_مجموعه == اشاره_تهی) { // Check for memory allocation error.
            مجموعه_مقصد_Bogus();
            أرجع *هذا;
        }
    }
    إذا (o.has_رشتهها()) {
        UError_کد وضعیت = U_ZERO_ERROR;
        إذا ((رشتهها_2 == اشاره_تهی && !allocate_رشتهها(وضعیت)) ||
                (رشتهها_2->assign(*o.رشتهها_2, cloneUnicodeString, وضعیت), U_FAILURE(وضعیت))) {
            مجموعه_مقصد_Bogus();
            أرجع *هذا;
        }
    } وإلا إذا (has_رشتهها()) {
        رشتهها_2->removeAllElements();
    }
    إذا (o.string_بازه != اشاره_تهی && رشتهها_2 != اشاره_تهی && !asThawed) {
        string_بازه = جديد Unicode_مجموعه_String_بازه(*o.string_بازه, *رشتهها_2);
        إذا (string_بازه == اشاره_تهی) { // Check for memory allocation error.
            مجموعه_مقصد_Bogus();
            أرجع *هذا;
        }
    }
    release_الگو();
    إذا (o.pat) {
        مجموعه_الگو(o.pat, o.patLen);
    }
    أرجع *هذا;
}

/**
 * Returns a copy of this object.  All UnicodeMatcher objects have
 * to support cloning in order to allow classes using
 * UnicodeMatchers, such as Transliterator, to implement cloning.
 */
Unicode_مجموعه* Unicode_مجموعه::clone() ثابت {
    أرجع جديد Unicode_مجموعه(*هذا);
}

Unicode_مجموعه *Unicode_مجموعه::cloneAsThawed() ثابت {
    أرجع جديد Unicode_مجموعه(*هذا, درست);
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
منطقي Unicode_مجموعه::عملگر==(ثابت Unicode_مجموعه& o) ثابت {
    إذا (len != o.len) أرجع خطأ;
    لكل (int32_t i = 0; i < len; ++i) {
        إذا (list[i] != o.list[i]) أرجع خطأ;
    }
    إذا (has_رشتهها() != o.has_رشتهها()) { أرجع خطأ; }
    إذا (has_رشتهها() && *رشتهها_2 != *o.رشتهها_2) أرجع خطأ;
    أرجع درست;
}

/**
 * Returns the hash code value for this set.
 *
 * @return the hash code value for this set.
 * @see Object#hashCode()
 */
int32_t Unicode_مجموعه::hash_کد() ثابت {
    uint32_t نتیجه = static_cast<uint32_t>(len);
    لكل (int32_t i = 0; i < len; ++i) {
        نتیجه *= 1000003u;
        نتیجه += list[i];
    }
    أرجع static_cast<int32_t>(نتیجه);
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
int32_t Unicode_مجموعه::size() ثابت {
    int32_t n = 0;
    int32_t count = get_Range_شمار();
    لكل (int32_t i = 0; i < count; ++i) {
        n += get_Range_انتها(i) - get_Range_شروع(i) + 1;
    }
    أرجع n + رشتهها_اندازه();
}

/**
 * Returns <tt>true</tt> if this set contains no elements.
 *
 * @return <tt>true</tt> if this set contains no elements.
 */
UBool Unicode_مجموعه::is_خالی() ثابت {
    أرجع len == 1 && !has_رشتهها();
}

/**
 * Returns true if this set contains the given character.
 * @param c character to be checked for containment
 * @return true if the test condition is met
 */
UBool Unicode_مجموعه::شامل_است(UChar32 c) ثابت {
    // Set i to the index of the start item greater than ch
    // We know we will terminate without length test!
    // LATER: for large sets, add binary search
    //int32_t i = -1;
    //for (;;) {
    //    if (c < list[++i]) break;
    //}
    إذا (bmp_مجموعه != اشاره_تهی) {
        أرجع bmp_مجموعه->شامل_است(c);
    }
    إذا (string_بازه != اشاره_تهی) {
        أرجع string_بازه->شامل_است(c);
    }
    إذا (c >= UNICODESET_بالا) { // Don't need to check LOW bound
        أرجع خطأ;
    }
    int32_t i = پیدا_کن_کد_نقطه(c);
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
int32_t Unicode_مجموعه::پیدا_کن_کد_نقطه(UChar32 c) ثابت {
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
UBool Unicode_مجموعه::شامل_است(UChar32 شروع, UChar32 انتها) ثابت {
    //int32_t i = -1;
    //for (;;) {
    //    if (start < list[++i]) break;
    //}
    int32_t i = پیدا_کن_کد_نقطه(شروع);
    أرجع ((i & 1) != 0 && انتها < list[i]);
}

/**
 * Returns <tt>true</tt> if this set contains the given
 * multicharacter string.
 * @param s string to be checked for containment
 * @return <tt>true</tt> if this set contains the specified string
 */
UBool Unicode_مجموعه::شامل_است(ثابت UnicodeString& s) ثابت {
    int32_t cp = getSingleCP(s);
    إذا (cp < 0) {
        أرجع رشتهها_شامل_است(s);
    } وإلا {
        أرجع شامل_است(static_cast<UChar32>(cp));
    }
}

/**
 * Returns true if this set contains all the characters and strings
 * of the given set.
 * @param c set to be checked for containment
 * @return true if the test condition is met
 */
UBool Unicode_مجموعه::شامل_است_All(ثابت Unicode_مجموعه& c) ثابت {
    // The specified set is a subset if all of its pairs are contained in
    // this set.  It's possible to code this more efficiently in terms of
    // direct manipulation of the inversion lists if the need arises.
    int32_t n = c.get_Range_شمار();
    لكل (صحيح i=0; i<n; ++i) {
        إذا (!شامل_است(c.get_Range_شروع(i), c.get_Range_انتها(i))) {
            أرجع خطأ;
        }
    }
    أرجع !c.has_رشتهها() || (رشتهها_2 != اشاره_تهی && رشتهها_2->شامل_است_All(*c.رشتهها_2));
}

/**
 * Returns true if this set contains all the characters
 * of the given string.
 * @param s string containing characters to be checked for containment
 * @return true if the test condition is met
 */
UBool Unicode_مجموعه::شامل_است_All(ثابت UnicodeString& s) ثابت {
    أرجع بازه(s.get_میانگیر(), s.length(), USET_بازه_CONTAINED) == s.length();
}

/**
 * Returns true if this set contains none of the characters
 * of the given range.
 * @param start first character, inclusive, of the range
 * @param end last character, inclusive, of the range
 * @return true if the test condition is met
 */
UBool Unicode_مجموعه::شامل_است_None(UChar32 شروع, UChar32 انتها) ثابت {
    //int32_t i = -1;
    //for (;;) {
    //    if (start < list[++i]) break;
    //}
    int32_t i = پیدا_کن_کد_نقطه(شروع);
    أرجع ((i & 1) == 0 && انتها < list[i]);
}

/**
 * Returns true if this set contains none of the characters and strings
 * of the given set.
 * @param c set to be checked for containment
 * @return true if the test condition is met
 */
UBool Unicode_مجموعه::شامل_است_None(ثابت Unicode_مجموعه& c) ثابت {
    // The specified set is a subset if all of its pairs are contained in
    // this set.  It's possible to code this more efficiently in terms of
    // direct manipulation of the inversion lists if the need arises.
    int32_t n = c.get_Range_شمار();
    لكل (int32_t i=0; i<n; ++i) {
        إذا (!شامل_است_None(c.get_Range_شروع(i), c.get_Range_انتها(i))) {
            أرجع خطأ;
        }
    }
    أرجع رشتهها_2 == اشاره_تهی || !c.has_رشتهها() || رشتهها_2->شامل_است_None(*c.رشتهها_2);
}

/**
 * Returns true if this set contains none of the characters
 * of the given string.
 * @param s string containing characters to be checked for containment
 * @return true if the test condition is met
 */
UBool Unicode_مجموعه::شامل_است_None(ثابت UnicodeString& s) ثابت {
    أرجع بازه(s.get_میانگیر(), s.length(), USET_بازه_NOT_CONTAINED) == s.length();
}

/**
 * Returns <tt>true</tt> if this set contains any character whose low byte
 * is the given value.  This is used by <tt>RuleBasedTransliterator</tt> for
 * indexing.
 */
UBool Unicode_مجموعه::matches_شاخص_مقدار(uint8_t v) ثابت {
    /* The index value v, in the range [0,255], is contained in this set if
     * it is contained in any pair of this set.  Pairs either have the high
     * bytes equal, or unequal.  If the high bytes are equal, then we have
     * aaxx..aayy, where aa is the high byte.  Then v is contained if xx <=
     * v <= yy.  If the high bytes are unequal we have aaxx..bbyy, bb>aa.
     * Then v is contained if xx <= v || v <= yy.  (This is identical to the
     * time zone month containment logic.)
     */
    int32_t i;
    int32_t range_شمار=get_Range_شمار();
    لكل (i=0; i<range_شمار; ++i) {
        UChar32 کم = get_Range_شروع(i);
        UChar32 بالا = get_Range_انتها(i);
        إذا ((کم & ~0xFF) == (بالا & ~0xFF)) {
            إذا ((کم & 0xFF) <= v && v <= (بالا & 0xFF)) {
                أرجع درست;
            }
        } وإلا إذا ((کم & 0xFF) <= v || v <= (بالا & 0xFF)) {
            أرجع درست;
        }
    }
    إذا (has_رشتهها()) {
        لكل (i=0; i<رشتهها_2->size(); ++i) {
            ثابت UnicodeString& s = *static_cast<ثابت UnicodeString*>(رشتهها_2->elementAt(i));
            إذا (s.is_خالی()) {
                استمر;  // skip the empty string
            }
            UChar32 c = s.char32At(0);
            إذا ((c & 0xFF) == v) {
                أرجع درست;
            }
        }
    }
    أرجع خطأ;
}

/**
 * Implementation of UnicodeMatcher::matches().  Always matches the
 * longest possible multichar string.
 */
UMatchDegree Unicode_مجموعه::matches(ثابت Replaceable& متن,
                                 int32_t& جابه_جایی,
                                 int32_t حد,
                                 UBool incremental) {
    إذا (جابه_جایی == حد) {
        إذا (شامل_است(U_ETHER)) {
            أرجع incremental ? U_PARTIAL_تطابق : U_تطابق;
        } وإلا {
            أرجع U_MISMATCH;
        }
    } وإلا {
        إذا (has_رشتهها()) { // try strings first

            // might separate forward and backward loops later
            // for now they are combined

            // TODO Improve efficiency of this, at least in the forward
            // direction, if not in both.  In the forward direction we
            // can assume the strings are sorted.

            int32_t i;
            UBool forward = جابه_جایی < حد;

            // firstChar is the leftmost char to match in the
            // forward direction or the rightmost char to match in
            // the reverse direction.
            char16_t اولی_Char = متن.charAt(جابه_جایی);

            // If there are multiple strings that can match we
            // return the longest match.
            int32_t بالا_Water_درازی = 0;

            لكل (i=0; i<رشتهها_2->size(); ++i) {
                ثابت UnicodeString& trial = *static_cast<ثابت UnicodeString*>(رشتهها_2->elementAt(i));
                إذا (trial.is_خالی()) {
                    استمر;  // skip the empty string
                }

                char16_t c = trial.charAt(forward ? 0 : trial.length() - 1);

                // Strings are sorted, so we can optimize in the
                // forward direction.
                إذا (forward && c > اولی_Char) اكسر;
                إذا (c != اولی_Char) استمر;

                int32_t تطابق_Len = تطابق_Rest(متن, جابه_جایی, حد, trial);

                إذا (incremental) {
                    int32_t maxLen = forward ? حد-جابه_جایی : جابه_جایی-حد;
                    إذا (تطابق_Len == maxLen) {
                        // We have successfully matched but only up to limit.
                        أرجع U_PARTIAL_تطابق;
                    }
                }

                إذا (تطابق_Len == trial.length()) {
                    // We have successfully matched the whole string.
                    إذا (تطابق_Len > بالا_Water_درازی) {
                        بالا_Water_درازی = تطابق_Len;
                    }
                    // In the forward direction we know strings
                    // are sorted so we can bail early.
                    إذا (forward && تطابق_Len < بالا_Water_درازی) {
                        اكسر;
                    }
                    استمر;
                }
            }

            // We've checked all strings without a partial match.
            // If we have full matches, return the longest one.
            إذا (بالا_Water_درازی != 0) {
                جابه_جایی += forward ? بالا_Water_درازی : -بالا_Water_درازی;
                أرجع U_تطابق;
            }
        }
        أرجع UnicodeFilter::matches(متن, جابه_جایی, حد, incremental);
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
int32_t Unicode_مجموعه::تطابق_Rest(ثابت Replaceable& متن,
                              int32_t شروع, int32_t حد,
                              ثابت UnicodeString& s) {
    int32_t i;
    int32_t maxLen;
    int32_t slen = s.length();
    إذا (شروع < حد) {
        maxLen = حد - شروع;
        إذا (maxLen > slen) maxLen = slen;
        لكل (i = 1; i < maxLen; ++i) {
            إذا (متن.charAt(شروع + i) != s.charAt(i)) أرجع 0;
        }
    } وإلا {
        maxLen = شروع - حد;
        إذا (maxLen > slen) maxLen = slen;
        --slen; // <=> slen = s.length() - 1;
        لكل (i = 1; i < maxLen; ++i) {
            إذا (متن.charAt(شروع - i) != s.charAt(slen - i)) أرجع 0;
        }
    }
    أرجع maxLen;
}

/**
 * Implement of UnicodeMatcher
 */
فراغ Unicode_مجموعه::جمع_کن_تطابق_مجموعه_مقصد(Unicode_مجموعه& مقصد_Union_مقصد) ثابت {
    مقصد_Union_مقصد.جمع_کن_All(*هذا);
}

/**
 * Returns the index of the given character within this set, where
 * the set is ordered by ascending code point.  If the character
 * is not in this set, return -1.  The inverse of this method is
 * <code>charAt()</code>.
 * @return an index from 0..size()-1, or -1
 */
int32_t Unicode_مجموعه::شاخص_از(UChar32 c) ثابت {
    إذا (c < MIN_مقدار || c > MAX_مقدار) {
        أرجع -1;
    }
    int32_t i = 0;
    int32_t n = 0;
    لكل (;;) {
        UChar32 شروع = list[i++];
        إذا (c < شروع) {
            أرجع -1;
        }
        UChar32 حد = list[i++];
        إذا (c < حد) {
            أرجع n + c - شروع;
        }
        n += حد - شروع;
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
UChar32 Unicode_مجموعه::charAt(int32_t شاخص) ثابت {
    إذا (شاخص >= 0) {
        // len2 is the largest even integer <= len, that is, it is len
        // for even values and len-1 for odd values.  With odd values
        // the last entry is UNICODESET_HIGH.
        int32_t len2 = len & ~1;
        لكل (int32_t i=0; i < len2;) {
            UChar32 شروع = list[i++];
            int32_t count = list[i++] - شروع;
            إذا (شاخص < count) {
                أرجع static_cast<UChar32>(شروع + شاخص);
            }
            شاخص -= count;
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
Unicode_مجموعه& Unicode_مجموعه::مجموعه(UChar32 شروع, UChar32 انتها) U_LIFETIME_BOUND {
    clear();
    complement(شروع, انتها);
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
Unicode_مجموعه& Unicode_مجموعه::جمع_کن(UChar32 شروع, UChar32 انتها) U_LIFETIME_BOUND {
    إذا (pin_کد_نقطه(شروع) < pin_کد_نقطه(انتها)) {
        UChar32 حد = انتها + 1;
        // Fast path for adding a new range after the last one.
        // Odd list length: [..., lastStart, lastLimit, HIGH]
        إذا ((len & 1) != 0) {
            // If the list is empty, set lastLimit low enough to not be adjacent to 0.
            UChar32 آخرین_حد = len == 1 ? -2 : list[len - 2];
            إذا (آخرین_حد <= شروع && !isFrozen() && !isBogus()) {
                إذا (آخرین_حد == شروع) {
                    // Extend the last range.
                    list[len - 2] = حد;
                    إذا (حد == UNICODESET_بالا) {
                        --len;
                    }
                } وإلا {
                    list[len - 1] = شروع;
                    إذا (حد < UNICODESET_بالا) {
                        إذا (ensure_ظرفیت(len + 2)) {
                            list[len++] = حد;
                            list[len++] = UNICODESET_بالا;
                        }
                    } وإلا {  // limit == UNICODESET_HIGH
                        إذا (ensure_ظرفیت(len + 1)) {
                            list[len++] = UNICODESET_بالا;
                        }
                    }
                }
                release_الگو();
                أرجع *هذا;
            }
        }
        // This is slow. Could be much faster using findCodePoint(start)
        // and modifying the list, dealing with adjacent & overlapping ranges.
        UChar32 range[3] = { شروع, حد, UNICODESET_بالا };
        جمع_کن(range, 2, 0);
    } وإلا إذا (شروع == انتها) {
        جمع_کن(شروع);
    }
    أرجع *هذا;
}

// #define DEBUG_US_ADD

#اگر_تعریف_شده DEBUG_US_جمع_کن
#ضمن <stdio.h>
فراغ dump(UChar32 c) {
    إذا (c <= 0xFF) {
        چاپ_کن("%c", (حرف)c);
    } وإلا {
        چاپ_کن("U+%04X", c);
    }
}
فراغ dump(ثابت UChar32* list, int32_t len) {
    چاپ_کن("[");
    لكل (int32_t i=0; i<len; ++i) {
        إذا (i != 0) چاپ_کن(", ");
        dump(list[i]);
    }
    چاپ_کن("]");
}
#پایان_اگر

/**
 * Adds the specified character to this set if it is not already
 * present.  If this set already contains the specified character,
 * the call leaves this set unchanged.
 */
Unicode_مجموعه& Unicode_مجموعه::جمع_کن(UChar32 c) U_LIFETIME_BOUND {
    // find smallest i such that c < list[i]
    // if odd, then it is IN the set
    // if even, then it is OUT of the set
    int32_t i = پیدا_کن_کد_نقطه(pin_کد_نقطه(c));

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

#اگر_تعریف_شده DEBUG_US_جمع_کن
    چاپ_کن("Add of ");
    dump(c);
    چاپ_کن(" found at %d", i);
    چاپ_کن(": ");
    dump(list, len);
    چاپ_کن(" => ");
#پایان_اگر

    إذا (c == list[i]-1) {
        // c is before start of next range
        list[i] = c;
        // if we touched the HIGH mark, then add a new one
        إذا (c == (UNICODESET_بالا - 1)) {
            إذا (!ensure_ظرفیت(len+1)) {
                // ensureCapacity will mark the object as Bogus if OOM failure happens.
                أرجع *هذا;
            }
            list[len++] = UNICODESET_بالا;
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

        إذا (!ensure_ظرفیت(len+2)) {
            // ensureCapacity will mark the object as Bogus if OOM failure happens.
            أرجع *هذا;
        }

        UChar32 *p = list + i;
        uprv_memmove(p + 2, p, (len - i) * حجم(*p));
        list[i] = c;
        list[i+1] = c+1;
        len += 2;
    }

#اگر_تعریف_شده DEBUG_US_جمع_کن
    dump(list, len);
    چاپ_کن("\n");

    لكل (i=1; i<len; ++i) {
        إذا (list[i] <= list[i-1]) {
            // Corrupt array!
            چاپ_کن("ERROR: list has been corrupted\n");
            خروج(1);
        }
    }
#پایان_اگر

    release_الگو();
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
Unicode_مجموعه& Unicode_مجموعه::جمع_کن(ثابت UnicodeString& s) U_LIFETIME_BOUND {
    إذا (isFrozen() || isBogus()) أرجع *هذا;
    int32_t cp = getSingleCP(s);
    إذا (cp < 0) {
        إذا (!رشتهها_شامل_است(s)) {
            جمع_کن_2(s);
            release_الگو();
        }
    } وإلا {
        جمع_کن(static_cast<UChar32>(cp));
    }
    أرجع *هذا;
}

/**
 * Adds the given string, in order, to 'strings_'.  The given string
 * must have been checked by the caller to not already be in 'strings_'.
 */
فراغ Unicode_مجموعه::جمع_کن_2(ثابت UnicodeString& s) {
    إذا (isFrozen() || isBogus()) {
        أرجع;
    }
    UError_کد ec = U_ZERO_ERROR;
    إذا (رشتهها_2 == اشاره_تهی && !allocate_رشتهها(ec)) {
        مجموعه_مقصد_Bogus();
        أرجع;
    }
    Local_اشاره_گر<UnicodeString> t(جديد UnicodeString(s));
    إذا (t.isNull()) { // Check for memory allocation error.
        مجموعه_مقصد_Bogus();
        أرجع;
    }
    رشتهها_2->sorted_درج(t.orphan(), compareUnicodeString, ec);
    إذا (U_FAILURE(ec)) {
        مجموعه_مقصد_Bogus();
    }
}

/**
 * @return a code point IF the string consists of a single one.
 * otherwise returns -1.
 * @param string to test
 */
int32_t Unicode_مجموعه::getSingleCP(ثابت UnicodeString& s) {
    int32_t s_درازی = s.length();
    إذا (s_درازی == 1) أرجع s.charAt(0);
    إذا (s_درازی == 2) {
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
Unicode_مجموعه& Unicode_مجموعه::جمع_کن_All(ثابت UnicodeString& s) U_LIFETIME_BOUND {
    UChar32 cp;
    لكل (int32_t i = 0; i < s.length(); i += U16_درازی(cp)) {
        cp = s.char32At(i);
        جمع_کن(cp);
    }
    أرجع *هذا;
}

/**
 * Retains EACH of the characters in this string. Note: "ch" == {"c", "h"}
 * If this set already any particular character, it has no effect on that character.
 * @param the source string
 * @return the modified set, for chaining
 */
Unicode_مجموعه& Unicode_مجموعه::retainAll(ثابت UnicodeString& s) U_LIFETIME_BOUND {
    Unicode_مجموعه مجموعه;
    مجموعه.جمع_کن_All(s);
    retainAll(مجموعه);
    أرجع *هذا;
}

/**
 * Complement EACH of the characters in this string. Note: "ch" == {"c", "h"}
 * If this set already any particular character, it has no effect on that character.
 * @param the source string
 * @return the modified set, for chaining
 */
Unicode_مجموعه& Unicode_مجموعه::complementAll(ثابت UnicodeString& s) U_LIFETIME_BOUND {
    Unicode_مجموعه مجموعه;
    مجموعه.جمع_کن_All(s);
    complementAll(مجموعه);
    أرجع *هذا;
}

/**
 * Remove EACH of the characters in this string. Note: "ch" == {"c", "h"}
 * If this set already any particular character, it has no effect on that character.
 * @param the source string
 * @return the modified set, for chaining
 */
Unicode_مجموعه& Unicode_مجموعه::removeAll(ثابت UnicodeString& s) U_LIFETIME_BOUND {
    Unicode_مجموعه مجموعه;
    مجموعه.جمع_کن_All(s);
    removeAll(مجموعه);
    أرجع *هذا;
}

Unicode_مجموعه& Unicode_مجموعه::remove_All_رشتهها() U_LIFETIME_BOUND {
    إذا (!isFrozen() && has_رشتهها()) {
        رشتهها_2->removeAllElements();
        release_الگو();
    }
    أرجع *هذا;
}


/**
 * Makes a set from a multicharacter string. Thus "ch" => {"ch"}
 * <br><b>Warning: you cannot add an empty string ("") to a UnicodeSet.</b>
 * @param the source string
 * @return a newly created set containing the given string
 */
Unicode_مجموعه* U_EXPORT2 Unicode_مجموعه::createFrom(ثابت UnicodeString& s) {
    Unicode_مجموعه *مجموعه = جديد Unicode_مجموعه();
    إذا (مجموعه != اشاره_تهی) { // Check for memory allocation error.
        مجموعه->جمع_کن(s);
    }
    أرجع مجموعه;
}


/**
 * Makes a set from each of the characters in the string. Thus "ch" => {"c", "h"}
 * @param the source string
 * @return a newly created set containing the given characters
 */
Unicode_مجموعه* U_EXPORT2 Unicode_مجموعه::createFromAll(ثابت UnicodeString& s) {
    Unicode_مجموعه *مجموعه = جديد Unicode_مجموعه();
    إذا (مجموعه != اشاره_تهی) { // Check for memory allocation error.
        مجموعه->جمع_کن_All(s);
    }
    أرجع مجموعه;
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
Unicode_مجموعه& Unicode_مجموعه::retain(UChar32 شروع, UChar32 انتها) U_LIFETIME_BOUND {
    إذا (pin_کد_نقطه(شروع) <= pin_کد_نقطه(انتها)) {
        UChar32 range[3] = { شروع, انتها+1, UNICODESET_بالا };
        retain(range, 2, 0);
    } وإلا {
        clear();
    }
    أرجع *هذا;
}

Unicode_مجموعه& Unicode_مجموعه::retain(UChar32 c) U_LIFETIME_BOUND {
    أرجع retain(c, c);
}

Unicode_مجموعه& Unicode_مجموعه::retain(ثابت UnicodeString &s) U_LIFETIME_BOUND {
    إذا (isFrozen() || isBogus()) { أرجع *هذا; }
    UChar32 cp = getSingleCP(s);
    إذا (cp < 0) {
        منطقي is_درون = رشتهها_شامل_است(s);
        // Check for getRangeCount() first to avoid somewhat-expensive size()
        // when there are single code points.
        إذا (is_درون && get_Range_شمار() == 0 && size() == 1) {
            أرجع *هذا;
        }
        clear();
        إذا (is_درون) {
            جمع_کن_2(s);
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
Unicode_مجموعه& Unicode_مجموعه::remove(UChar32 شروع, UChar32 انتها) U_LIFETIME_BOUND {
    إذا (pin_کد_نقطه(شروع) <= pin_کد_نقطه(انتها)) {
        UChar32 range[3] = { شروع, انتها+1, UNICODESET_بالا };
        retain(range, 2, 2);
    }
    أرجع *هذا;
}

/**
 * Removes the specified character from this set if it is present.
 * The set will not contain the specified range once the call
 * returns.
 */
Unicode_مجموعه& Unicode_مجموعه::remove(UChar32 c) U_LIFETIME_BOUND {
    أرجع remove(c, c);
}

/**
 * Removes the specified string from this set if it is present.
 * The set will not contain the specified character once the call
 * returns.
 * @param the source string
 * @return the modified set, for chaining
 */
Unicode_مجموعه& Unicode_مجموعه::remove(ثابت UnicodeString& s) U_LIFETIME_BOUND {
    إذا (isFrozen() || isBogus()) أرجع *هذا;
    int32_t cp = getSingleCP(s);
    إذا (cp < 0) {
        إذا (رشتهها_2 != اشاره_تهی && رشتهها_2->removeElement((فراغ*) &s)) {
            release_الگو();
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
Unicode_مجموعه& Unicode_مجموعه::complement(UChar32 شروع, UChar32 انتها) U_LIFETIME_BOUND {
    إذا (isFrozen() || isBogus()) {
        أرجع *هذا;
    }
    إذا (pin_کد_نقطه(شروع) <= pin_کد_نقطه(انتها)) {
        UChar32 range[3] = { شروع, انتها+1, UNICODESET_بالا };
        exclusiveOr(range, 2, 0);
    }
    release_الگو();
    أرجع *هذا;
}

Unicode_مجموعه& Unicode_مجموعه::complement(UChar32 c) U_LIFETIME_BOUND {
    أرجع complement(c, c);
}

/**
 * This is equivalent to
 * <code>complement(MIN_VALUE, MAX_VALUE)</code>.
 */
Unicode_مجموعه& Unicode_مجموعه::complement() U_LIFETIME_BOUND {
    إذا (isFrozen() || isBogus()) {
        أرجع *هذا;
    }
    إذا (list[0] == UNICODESET_کم) {
        uprv_memmove(list, list + 1, (size_t)(len-1)*حجم(UChar32));
        --len;
    } وإلا {
        إذا (!ensure_ظرفیت(len+1)) {
            أرجع *هذا;
        }
        uprv_memmove(list + 1, list, (size_t)len*حجم(UChar32));
        list[0] = UNICODESET_کم;
        ++len;
    }
    release_الگو();
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
Unicode_مجموعه& Unicode_مجموعه::complement(ثابت UnicodeString& s) U_LIFETIME_BOUND {
    إذا (isFrozen() || isBogus()) أرجع *هذا;
    int32_t cp = getSingleCP(s);
    إذا (cp < 0) {
        إذا (رشتهها_شامل_است(s)) {
            رشتهها_2->removeElement((فراغ*) &s);
        } وإلا {
            جمع_کن_2(s);
        }
        release_الگو();
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
Unicode_مجموعه& Unicode_مجموعه::جمع_کن_All(ثابت Unicode_مجموعه& c) U_LIFETIME_BOUND {
    إذا ( c.len>0 && c.list!=اشاره_تهی ) {
        جمع_کن(c.list, c.len, 0);
    }

    // Add strings in order
    إذا ( c.رشتهها_2!=اشاره_تهی ) {
        لكل (int32_t i=0; i<c.رشتهها_2->size(); ++i) {
            ثابت UnicodeString* s = static_cast<ثابت UnicodeString*>(c.رشتهها_2->elementAt(i));
            إذا (!رشتهها_شامل_است(*s)) {
                جمع_کن_2(*s);
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
Unicode_مجموعه& Unicode_مجموعه::retainAll(ثابت Unicode_مجموعه& c) U_LIFETIME_BOUND {
    إذا (isFrozen() || isBogus()) {
        أرجع *هذا;
    }
    retain(c.list, c.len, 0);
    إذا (has_رشتهها()) {
        إذا (!c.has_رشتهها()) {
            رشتهها_2->removeAllElements();
        } وإلا {
            رشتهها_2->retainAll(*c.رشتهها_2);
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
Unicode_مجموعه& Unicode_مجموعه::removeAll(ثابت Unicode_مجموعه& c) U_LIFETIME_BOUND {
    إذا (isFrozen() || isBogus()) {
        أرجع *هذا;
    }
    retain(c.list, c.len, 2);
    إذا (has_رشتهها() && c.has_رشتهها()) {
        رشتهها_2->removeAll(*c.رشتهها_2);
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
Unicode_مجموعه& Unicode_مجموعه::complementAll(ثابت Unicode_مجموعه& c) U_LIFETIME_BOUND {
    إذا (isFrozen() || isBogus()) {
        أرجع *هذا;
    }
    exclusiveOr(c.list, c.len, 0);

    إذا (c.رشتهها_2 != اشاره_تهی) {
        لكل (int32_t i=0; i<c.رشتهها_2->size(); ++i) {
            فراغ* e = c.رشتهها_2->elementAt(i);
            إذا (رشتهها_2 == اشاره_تهی || !رشتهها_2->removeElement(e)) {
                جمع_کن_2(*static_cast<ثابت UnicodeString*>(e));
            }
        }
    }
    أرجع *هذا;
}

/**
 * Removes all of the elements from this set.  This set will be
 * empty after this call returns.
 */
Unicode_مجموعه& Unicode_مجموعه::clear() U_LIFETIME_BOUND {
    إذا (isFrozen()) {
        أرجع *هذا;
    }
    list[0] = UNICODESET_بالا;
    len = 1;
    release_الگو();
    إذا (رشتهها_2 != اشاره_تهی) {
        رشتهها_2->removeAllElements();
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
int32_t Unicode_مجموعه::get_Range_شمار() ثابت {
    أرجع len/2;
}

/**
 * Iteration method that returns the first character in the
 * specified range of this set.
 * @see #getRangeCount
 * @see #getRangeEnd
 */
UChar32 Unicode_مجموعه::get_Range_شروع(int32_t شاخص) ثابت {
    أرجع list[شاخص*2];
}

/**
 * Iteration method that returns the last character in the
 * specified range of this set.
 * @see #getRangeStart
 * @see #getRangeEnd
 */
UChar32 Unicode_مجموعه::get_Range_انتها(int32_t شاخص) ثابت {
    أرجع list[شاخص*2 + 1] - 1;
}

ثابت UnicodeString* Unicode_مجموعه::getString(int32_t شاخص) ثابت U_LIFETIME_BOUND {
    أرجع static_cast<ثابت UnicodeString*>(رشتهها_2->elementAt(شاخص));
}

/**
 * Reallocate this objects internal structures to take up the least
 * possible space, without changing this object's value.
 */
Unicode_مجموعه& Unicode_مجموعه::compact() U_LIFETIME_BOUND {
    إذا (isFrozen() || isBogus()) {
        أرجع *هذا;
    }
    // Delete buffer first to defragment memory less.
    إذا (میانگیر != پشته_List) {
        uprv_free(میانگیر);
        میانگیر = اشاره_تهی;
        میانگیر_ظرفیت = 0;
    }
    إذا (list == پشته_List) {
        // pass
    } وإلا إذا (len <= INITIAL_ظرفیت) {
        uprv_memcpy(پشته_List, list, len * حجم(UChar32));
        uprv_free(list);
        list = پشته_List;
        capacity = INITIAL_ظرفیت;
    } وإلا إذا ((len + 7) < capacity) {
        // If we have more than a little unused capacity, shrink it to len.
        UChar32* موقت = static_cast<UChar32*>(uprv_realloc(list, حجم(UChar32) * len));
        إذا (موقت) {
            list = موقت;
            capacity = len;
        }
        // else what the heck happened?! We allocated less memory!
        // Oh well. We'll keep our original array.
    }
    إذا (رشتهها_2 != اشاره_تهی && رشتهها_2->is_خالی()) {
        حذف رشتهها_2;
        رشتهها_2 = اشاره_تهی;
    }
    أرجع *هذا;
}

#اگر_تعریف_شده DEBUG_SERIALIZE
#ضمن <stdio.h>
#پایان_اگر

/**
 * Deserialize constructor.
 */
Unicode_مجموعه::Unicode_مجموعه(ثابت uint16_t داده[], int32_t داده_Len, ESerialization serialization,
                       UError_کد &ec) {

  إذا(U_FAILURE(ec)) {
    مجموعه_مقصد_Bogus();
    أرجع;
  }

  إذا( (serialization != kSerialized)
      || (داده==اشاره_تهی)
      || (داده_Len < 1)) {
    ec = U_ILLEGAL_آرگومان_ERROR;
    مجموعه_مقصد_Bogus();
    أرجع;
  }

  // bmp?
  int32_t سرآیند_اندازه = ((داده[0]&0x8000)) ?2:1;
  int32_t bmp_درازی = (سرآیند_اندازه==1)?داده[0]:داده[1];

  int32_t new_درازی = (((داده[0]&0x7FFF)-bmp_درازی)/2)+bmp_درازی;
#اگر_تعریف_شده DEBUG_SERIALIZE
  چاپ_کن("dataLen %d headerSize %d bmpLen %d len %d. data[0]=%X/%X/%X/%X\n", داده_Len,سرآیند_اندازه,bmp_درازی,new_درازی, داده[0],داده[1],داده[2],داده[3]);
#پایان_اگر
  إذا(!ensure_ظرفیت(new_درازی + 1)) {  // +1 for HIGH
    أرجع;
  }
  // copy bmp
  int32_t i;
  لكل(i = 0; i< bmp_درازی;i++) {
    list[i] = داده[i+سرآیند_اندازه];
#اگر_تعریف_شده DEBUG_SERIALIZE
    چاپ_کن("<<16@%d[%d] %X\n", i+سرآیند_اندازه, i, list[i]);
#پایان_اگر
  }
  // copy smp
  لكل(i=bmp_درازی;i<new_درازی;i++) {
    list[i] = (static_cast<UChar32>(داده[سرآیند_اندازه + bmp_درازی + (i - bmp_درازی) * 2 + 0]) << 16) +
               static_cast<UChar32>(داده[سرآیند_اندازه + bmp_درازی + (i - bmp_درازی) * 2 + 1]);
#اگر_تعریف_شده DEBUG_SERIALIZE
    چاپ_کن("<<32@%d+[%d] %lX\n", سرآیند_اندازه+bmp_درازی+i, i, list[i]);
#پایان_اگر
  }
  U_ASSERT(i == new_درازی);
  إذا (i == 0 || list[i - 1] != UNICODESET_بالا) {
    list[i++] = UNICODESET_بالا;
  }
  len = i;
}


int32_t Unicode_مجموعه::serialize(uint16_t *dest, int32_t dest_ظرفیت, UError_کد& ec) ثابت {
    int32_t bmp_درازی, length, dest_درازی;

    إذا (U_FAILURE(ec)) {
        أرجع 0;
    }

    إذا (dest_ظرفیت<0 || (dest_ظرفیت>0 && dest==اشاره_تهی)) {
        ec=U_ILLEGAL_آرگومان_ERROR;
        أرجع 0;
    }

    /* count necessary 16-bit units */
    length=هذا->len-1; // Subtract 1 to ignore final UNICODESET_HIGH
    // assert(length>=0);
    إذا (length==0) {
        /* empty set */
        إذا (dest_ظرفیت>0) {
            *dest=0;
        } وإلا {
            ec=U_میانگیر_سرریز_ERROR;
        }
        أرجع 1;
    }
    /* now length>0 */

    إذا (هذا->list[length-1]<=0xffff) {
        /* all BMP */
        bmp_درازی=length;
    } وإلا إذا (هذا->list[0]>=0x10000) {
        /* all supplementary */
        bmp_درازی=0;
        length*=2;
    } وإلا {
        /* some BMP, some supplementary */
        لكل (bmp_درازی=0; bmp_درازی<length && هذا->list[bmp_درازی]<=0xffff; ++bmp_درازی) {}
        length=bmp_درازی+2*(length-bmp_درازی);
    }
#اگر_تعریف_شده DEBUG_SERIALIZE
    چاپ_کن(">> bmpLength%d length%d len%d\n", bmp_درازی, length, len);
#پایان_اگر
    /* length: number of 16-bit array units */
    إذا (length>0x7fff) {
        /* there are only 15 bits for the length in the first serialized word */
        ec=U_شاخص_OUTOFBOUNDS_ERROR;
        أرجع 0;
    }

    /*
     * total serialized length:
     * number of 16-bit array units (length) +
     * 1 length unit (always) +
     * 1 bmpLength unit (if there are supplementary values)
     */
    dest_درازی=length+((length>bmp_درازی)?2:1);
    إذا (dest_درازی<=dest_ظرفیت) {
        ثابت UChar32 *p;
        int32_t i;

#اگر_تعریف_شده DEBUG_SERIALIZE
        چاپ_کن("writeHdr\n");
#پایان_اگر
        *dest = static_cast<uint16_t>(length);
        إذا (length>bmp_درازی) {
            *dest|=0x8000;
            *++dest = static_cast<uint16_t>(bmp_درازی);
        }
        ++dest;

        /* write the BMP part of the array */
        p=هذا->list;
        لكل (i=0; i<bmp_درازی; ++i) {
#اگر_تعریف_شده DEBUG_SERIALIZE
          چاپ_کن("writebmp: %x\n", (صحيح)*p);
#پایان_اگر
            *dest++ = static_cast<uint16_t>(*p++);
        }

        /* write the supplementary part of the array */
        لكل (; i<length; i+=2) {
#اگر_تعریف_شده DEBUG_SERIALIZE
          چاپ_کن("write32: %x\n", (صحيح)*p);
#پایان_اگر
            *dest++ = static_cast<uint16_t>(*p >> 16);
            *dest++ = static_cast<uint16_t>(*p++);
        }
    } وإلا {
        ec=U_میانگیر_سرریز_ERROR;
    }
    أرجع dest_درازی;
}

//----------------------------------------------------------------
// Implementation: Utility methods
//----------------------------------------------------------------

/**
 * Allocate our strings vector and return true if successful.
 */
UBool Unicode_مجموعه::allocate_رشتهها(UError_کد &وضعیت) {
    إذا (U_FAILURE(وضعیت)) {
        أرجع خطأ;
    }
    رشتهها_2 = جديد UVector(uprv_deleteUObject,
                          uhash_compareUnicodeString, 1, وضعیت);
    إذا (رشتهها_2 == اشاره_تهی) { // Check for memory allocation error.
        وضعیت = U_حافظه_تخصیص_ERROR;
        أرجع خطأ;
    }
    إذا (U_FAILURE(وضعیت)) {
        حذف رشتهها_2;
        رشتهها_2 = اشاره_تهی;
        أرجع خطأ;
    } 
    أرجع درست;
}

int32_t Unicode_مجموعه::بعدی_ظرفیت(int32_t min_ظرفیت) {
    // Grow exponentially to reduce the frequency of allocations.
    إذا (min_ظرفیت < INITIAL_ظرفیت) {
        أرجع min_ظرفیت + INITIAL_ظرفیت;
    } وإلا إذا (min_ظرفیت <= 2500) {
        أرجع 5 * min_ظرفیت;
    } وإلا {
        int32_t new_ظرفیت = 2 * min_ظرفیت;
        إذا (new_ظرفیت > MAX_درازی) {
            new_ظرفیت = MAX_درازی;
        }
        أرجع new_ظرفیت;
    }
}

منطقي Unicode_مجموعه::ensure_ظرفیت(int32_t newLen) {
    إذا (newLen > MAX_درازی) {
        newLen = MAX_درازی;
    }
    إذا (newLen <= capacity) {
        أرجع درست;
    }
    int32_t new_ظرفیت = بعدی_ظرفیت(newLen);
    UChar32* موقت = static_cast<UChar32*>(uprv_malloc(new_ظرفیت * حجم(UChar32)));
    إذا (موقت == اشاره_تهی) {
        مجموعه_مقصد_Bogus(); // set the object to bogus state if an OOM failure occurred.
        أرجع خطأ;
    }
    // Copy only the actual contents.
    uprv_memcpy(موقت, list, len * حجم(UChar32));
    إذا (list != پشته_List) {
        uprv_free(list);
    }
    list = موقت;
    capacity = new_ظرفیت;
    أرجع درست;
}

منطقي Unicode_مجموعه::ensure_میانگیر_ظرفیت(int32_t newLen) {
    إذا (newLen > MAX_درازی) {
        newLen = MAX_درازی;
    }
    إذا (newLen <= میانگیر_ظرفیت) {
        أرجع درست;
    }
    int32_t new_ظرفیت = بعدی_ظرفیت(newLen);
    UChar32* موقت = static_cast<UChar32*>(uprv_malloc(new_ظرفیت * حجم(UChar32)));
    إذا (موقت == اشاره_تهی) {
        مجموعه_مقصد_Bogus();
        أرجع خطأ;
    }
    // The buffer has no contents to be copied.
    // It is always filled from scratch after this call.
    إذا (میانگیر != پشته_List) {
        uprv_free(میانگیر);
    }
    میانگیر = موقت;
    میانگیر_ظرفیت = new_ظرفیت;
    أرجع درست;
}

/**
 * Swap list and buffer.
 */
فراغ Unicode_مجموعه::جابجا_Buffers() {
    // swap list and buffer
    UChar32* موقت = list;
    list = میانگیر;
    میانگیر = موقت;

    int32_t c = capacity;
    capacity = میانگیر_ظرفیت;
    میانگیر_ظرفیت = c;
}

فراغ Unicode_مجموعه::مجموعه_مقصد_Bogus() {
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

فراغ Unicode_مجموعه::exclusiveOr(ثابت UChar32* other, int32_t otherLen, int8_t polarity) {
    إذا (isFrozen() || isBogus()) {
        أرجع;
    }
    إذا (!ensure_میانگیر_ظرفیت(len + otherLen)) {
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
            میانگیر[k++] = a;
            a = list[i++];
        } وإلا إذا (b < a) {
            میانگیر[k++] = b;
            b = other[j++];
        } وإلا إذا (a != UNICODESET_بالا) { // at this point, a == b
            // discard both values!
            a = list[i++];
            b = other[j++];
        } وإلا { // DONE!
            میانگیر[k++] = UNICODESET_بالا;
            len = k;
            اكسر;
        }
    }
    جابجا_Buffers();
    release_الگو();
}

// polarity = 0 is normal: x union y
// polarity = 2: x union ~y
// polarity = 1: ~x union y
// polarity = 3: ~x union ~y

فراغ Unicode_مجموعه::جمع_کن(ثابت UChar32* other, int32_t otherLen, int8_t polarity) {
    إذا (isFrozen() || isBogus() || other==اشاره_تهی) {
        أرجع;
    }
    إذا (!ensure_میانگیر_ظرفیت(len + otherLen)) {
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
                إذا (k > 0 && a <= میانگیر[k-1]) {
                    // Pick latter end value in buffer[] vs. list[]
                    a = max(list[i], میانگیر[--k]);
                } وإلا {
                    // No overlap
                    میانگیر[k++] = a;
                    a = list[i];
                }
                i++; // Common if/else code factored out
                polarity ^= 1;
            } وإلا إذا (b < a) { // take b
                إذا (k > 0 && b <= میانگیر[k-1]) {
                    b = max(other[j], میانگیر[--k]);
                } وإلا {
                    میانگیر[k++] = b;
                    b = other[j];
                }
                j++;
                polarity ^= 2;
            } وإلا { // a == b, take a, drop b
                إذا (a == UNICODESET_بالا) اذهب loop_انتها;
                // This is symmetrical; it doesn't matter if
                // we backtrack with a or b. - liu
                إذا (k > 0 && a <= میانگیر[k-1]) {
                    a = max(list[i], میانگیر[--k]);
                } وإلا {
                    // No overlap
                    میانگیر[k++] = a;
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
                إذا (a == UNICODESET_بالا) اذهب loop_انتها;
                میانگیر[k++] = a;
            } وإلا { // take b
                إذا (b == UNICODESET_بالا) اذهب loop_انتها;
                میانگیر[k++] = b;
            }
            a = list[i++];
            polarity ^= 1;   // factored common code
            b = other[j++];
            polarity ^= 2;
            اكسر;
          حالة 1: // a second, b first; if b < a, overlap
            إذا (a < b) { // no overlap, take a
                میانگیر[k++] = a; a = list[i++]; polarity ^= 1;
            } وإلا إذا (b < a) { // OVERLAP, drop b
                b = other[j++];
                polarity ^= 2;
            } وإلا { // a == b, drop both!
                إذا (a == UNICODESET_بالا) اذهب loop_انتها;
                a = list[i++];
                polarity ^= 1;
                b = other[j++];
                polarity ^= 2;
            }
            اكسر;
          حالة 2: // a first, b second; if a < b, overlap
            إذا (b < a) { // no overlap, take b
                میانگیر[k++] = b;
                b = other[j++];
                polarity ^= 2;
            } وإلا  إذا (a < b) { // OVERLAP, drop a
                a = list[i++];
                polarity ^= 1;
            } وإلا { // a == b, drop both!
                إذا (a == UNICODESET_بالا) اذهب loop_انتها;
                a = list[i++];
                polarity ^= 1;
                b = other[j++];
                polarity ^= 2;
            }
            اكسر;
        }
    }
 loop_انتها:
    میانگیر[k++] = UNICODESET_بالا;    // terminate
    len = k;
    جابجا_Buffers();
    release_الگو();
}

// polarity = 0 is normal: x intersect y
// polarity = 2: x intersect ~y == set-minus
// polarity = 1: ~x intersect y
// polarity = 3: ~x intersect ~y

فراغ Unicode_مجموعه::retain(ثابت UChar32* other, int32_t otherLen, int8_t polarity) {
    إذا (isFrozen() || isBogus()) {
        أرجع;
    }
    إذا (!ensure_میانگیر_ظرفیت(len + otherLen)) {
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
                إذا (a == UNICODESET_بالا) اذهب loop_انتها;
                میانگیر[k++] = a;
                a = list[i++];
                polarity ^= 1;
                b = other[j++];
                polarity ^= 2;
            }
            اكسر;
          حالة 3: // both second; take lower if unequal
            إذا (a < b) { // take a
                میانگیر[k++] = a;
                a = list[i++];
                polarity ^= 1;
            } وإلا إذا (b < a) { // take b
                میانگیر[k++] = b;
                b = other[j++];
                polarity ^= 2;
            } وإلا { // a == b, take one, drop other
                إذا (a == UNICODESET_بالا) اذهب loop_انتها;
                میانگیر[k++] = a;
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
                میانگیر[k++] = b;
                b = other[j++];
                polarity ^= 2;
            } وإلا { // a == b, drop both!
                إذا (a == UNICODESET_بالا) اذهب loop_انتها;
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
                میانگیر[k++] = a;
                a = list[i++];
                polarity ^= 1;
            } وإلا { // a == b, drop both!
                إذا (a == UNICODESET_بالا) اذهب loop_انتها;
                a = list[i++];
                polarity ^= 1;
                b = other[j++];
                polarity ^= 2;
            }
            اكسر;
        }
    }
 loop_انتها:
    میانگیر[k++] = UNICODESET_بالا;    // terminate
    len = k;
    جابجا_Buffers();
    release_الگو();
}

/**
 * Append the <code>toPattern()</code> representation of a
 * string to the given <code>StringBuffer</code>.
 */
فراغ Unicode_مجموعه::append_مقصد_Pat(UnicodeString& buf, ثابت UnicodeString& s, UBool escapeUnprintable) {
    UChar32 cp;
    لكل (int32_t i = 0; i < s.length(); i += U16_درازی(cp)) {
        append_مقصد_Pat(buf, cp = s.char32At(i), escapeUnprintable);
    }
}

/**
 * Append the <code>toPattern()</code> representation of a
 * character to the given <code>StringBuffer</code>.
 */
فراغ Unicode_مجموعه::append_مقصد_Pat(UnicodeString& buf, UChar32 c, UBool escapeUnprintable) {
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
    حالة نماد_جدول::نماد_REF:
        buf.append(u'\\');
        اكسر;
    افتراضي:
        // Escape whitespace
        إذا (الگو_Props::isWhiteSpace(c)) {
            buf.append(u'\\');
        }
        اكسر;
    }
    buf.append(c);
}

فراغ Unicode_مجموعه::append_مقصد_Pat(UnicodeString &نتیجه, UChar32 شروع, UChar32 انتها,
                              UBool escapeUnprintable) {
    append_مقصد_Pat(نتیجه, شروع, escapeUnprintable);
    إذا (شروع != انتها) {
        إذا ((شروع+1) != انتها ||
                // Avoid writing what looks like a lead+trail surrogate pair.
                شروع == 0xdbff) {
            نتیجه.append(u'-');
        }
        append_مقصد_Pat(نتیجه, انتها, escapeUnprintable);
    }
}

/**
 * Append a string representation of this set to result.  This will be
 * a cleaned version of the string passed to applyPattern(), if there
 * is one.  Otherwise it will be generated.
 */
UnicodeString& Unicode_مجموعه::مقصد_الگو_2(UnicodeString& نتیجه U_LIFETIME_BOUND,
                                      UBool escapeUnprintable) ثابت
{
    إذا (pat != اشاره_تهی) {
        int32_t i;
        int32_t backslash_شمار = 0;
        لكل (i=0; i<patLen; ) {
            UChar32 c;
            U16_بعدی(pat, i, patLen, c);
            إذا (escapeUnprintable ?
                    ICU_Utility::isUnprintable(c) : ICU_Utility::shouldAlwaysBeEscaped(c)) {
                // If the unprintable character is preceded by an odd
                // number of backslashes, then it has been escaped.
                // Before unescaping it, we delete the final
                // backslash.
                إذا ((backslash_شمار % 2) == 1) {
                    نتیجه.truncate(نتیجه.length() - 1);
                }
                ICU_Utility::escape(نتیجه, c);
                backslash_شمار = 0;
            } وإلا {
                نتیجه.append(c);
                إذا (c == u'\\') {
                    ++backslash_شمار;
                } وإلا {
                    backslash_شمار = 0;
                }
            }
        }
        أرجع نتیجه;
    }

    أرجع generate_الگو(نتیجه, escapeUnprintable);
}

/**
 * Returns a string representation of this set.  If the result of
 * calling this function is passed to a UnicodeSet constructor, it
 * will produce another set that is equal to this one.
 */
UnicodeString& Unicode_مجموعه::مقصد_الگو(UnicodeString& نتیجه U_LIFETIME_BOUND,
                                     UBool escapeUnprintable) ثابت
{
    نتیجه.truncate(0);
    أرجع مقصد_الگو_2(نتیجه, escapeUnprintable);
}

/**
 * Generate and append a string representation of this set to result.
 * This does not use this.pat, the cleaned up copy of the string
 * passed to applyPattern().
 */
UnicodeString& Unicode_مجموعه::generate_الگو(UnicodeString& نتیجه U_LIFETIME_BOUND,
                                            UBool escapeUnprintable) ثابت
{
    نتیجه.append(u'[');

    int32_t i = 0;
    int32_t حد = len & ~1;  // = 2 * getRangeCount()

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
    إذا (len >= 4 && list[0] == 0 && حد == len && !has_رشتهها()) {
        // Emit the inverse
        نتیجه.append(u'^');
        // Offsetting the inversion list index by one lets us
        // iterate over the ranges of the set complement.
        i = 1;
        --حد;
    }

    // Emit the ranges as pairs.
    طالما (i < حد) {
        UChar32 شروع = list[i];  // getRangeStart()
        UChar32 انتها = list[i + 1] - 1;  // getRangeEnd() = range limit minus one
        إذا (!(0xd800 <= انتها && انتها <= 0xdbff)) {
            append_مقصد_Pat(نتیجه, شروع, انتها, escapeUnprintable);
            i += 2;
        } وإلا {
            // The range ends with a lead surrogate.
            // Avoid writing what looks like a lead+trail surrogate pair.
            // 1. Postpone ranges that start with a lead surrogate code point.
            int32_t اولی_Lead = i;
            طالما ((i += 2) < حد && list[i] <= 0xdbff) {}
            int32_t اولی_After_Lead = i;
            // 2. Write following ranges that start with a trail surrogate code point.
            طالما (i < حد && (شروع = list[i]) <= 0xdfff) {
                append_مقصد_Pat(نتیجه, شروع, list[i + 1] - 1, escapeUnprintable);
                i += 2;
            }
            // 3. Now write the postponed ranges.
            لكل (صحيح j = اولی_Lead; j < اولی_After_Lead; j += 2) {
                append_مقصد_Pat(نتیجه, list[j], list[j + 1] - 1, escapeUnprintable);
            }
        }
    }

    إذا (رشتهها_2 != اشاره_تهی) {
        لكل (int32_t i = 0; i<رشتهها_2->size(); ++i) {
            نتیجه.append(u'{');
            append_مقصد_Pat(نتیجه,
                         *static_cast<ثابت UnicodeString*>(رشتهها_2->elementAt(i)),
                         escapeUnprintable);
            نتیجه.append(u'}');
        }
    }
    أرجع نتیجه.append(u']');
}

/**
* Release existing cached pattern
*/
فراغ Unicode_مجموعه::release_الگو() {
    إذا (pat) {
        uprv_free(pat);
        pat = اشاره_تهی;
        patLen = 0;
    }
}

/**
* Set the new pattern to cache.
*/
فراغ Unicode_مجموعه::مجموعه_الگو(ثابت char16_t *newPat, int32_t newPatLen) {
    release_الگو();
    pat = static_cast<char16_t*>(uprv_malloc((newPatLen + 1) * حجم(char16_t)));
    إذا (pat) {
        patLen = newPatLen;
        u_memcpy(pat, newPat, patLen);
        pat[patLen] = 0;
    }
    // else we don't care if malloc failed. This was just a nice cache.
    // We can regenerate an equivalent pattern later when requested.
}

Unicode_مجموعه* Unicode_مجموعه::freeze() U_LIFETIME_BOUND {
    إذا(!isFrozen() && !isBogus()) {
        compact();

        // Optimize contains() and span() and similar functions.
        إذا (has_رشتهها()) {
            string_بازه = جديد Unicode_مجموعه_String_بازه(*هذا, *رشتهها_2, Unicode_مجموعه_String_بازه::ALL);
            إذا (string_بازه == اشاره_تهی) {
                مجموعه_مقصد_Bogus();
                أرجع هذا;
            } وإلا إذا (!string_بازه->needs_String_بازه_UTF16()) {
                // All strings are irrelevant for span() etc. because
                // all of each string's code points are contained in this set.
                // Do not check needsStringSpanUTF8() because UTF-8 has at most as
                // many relevant strings as UTF-16.
                // (Thus needsStringSpanUTF8() implies needsStringSpanUTF16().)
                حذف string_بازه;
                string_بازه = اشاره_تهی;
            }
        }
        إذا (string_بازه == اشاره_تهی) {
            // No span-relevant strings: Optimize for code point spans.
            bmp_مجموعه=جديد BMPSet(list, len);
            إذا (bmp_مجموعه == اشاره_تهی) { // Check for memory allocation error.
                مجموعه_مقصد_Bogus();
            }
        }
    }
    أرجع هذا;
}

int32_t Unicode_مجموعه::بازه(ثابت char16_t *s, int32_t length, USet_بازه_Condition بازه_Condition) ثابت {
    إذا(length>0 && bmp_مجموعه!=اشاره_تهی) {
        أرجع static_cast<int32_t>(bmp_مجموعه->بازه(s, s + length, بازه_Condition) - s);
    }
    إذا(length<0) {
        length=u_strlen(s);
    }
    إذا(length==0) {
        أرجع 0;
    }
    إذا(string_بازه!=اشاره_تهی) {
        أرجع string_بازه->بازه(s, length, بازه_Condition);
    } وإلا إذا(has_رشتهها()) {
        uint32_t which= بازه_Condition==USET_بازه_NOT_CONTAINED ?
                            Unicode_مجموعه_String_بازه::FWD_UTF16_NOT_CONTAINED :
                            Unicode_مجموعه_String_بازه::FWD_UTF16_CONTAINED;
        Unicode_مجموعه_String_بازه str_بازه(*هذا, *رشتهها_2, which);
        إذا(str_بازه.needs_String_بازه_UTF16()) {
            أرجع str_بازه.بازه(s, length, بازه_Condition);
        }
    }

    إذا(بازه_Condition!=USET_بازه_NOT_CONTAINED) {
        بازه_Condition=USET_بازه_CONTAINED;  // Pin to 0/1 values.
    }

    UChar32 c;
    int32_t شروع=0, prev=0;
    افعل {
        U16_بعدی(s, شروع, length, c);
        إذا(بازه_Condition!=شامل_است(c)) {
            اكسر;
        }
    } طالما((prev=شروع)<length);
    أرجع prev;
}

int32_t Unicode_مجموعه::بازه_پشت(ثابت char16_t *s, int32_t length, USet_بازه_Condition بازه_Condition) ثابت {
    إذا(length>0 && bmp_مجموعه!=اشاره_تهی) {
        أرجع static_cast<int32_t>(bmp_مجموعه->بازه_پشت(s, s + length, بازه_Condition) - s);
    }
    إذا(length<0) {
        length=u_strlen(s);
    }
    إذا(length==0) {
        أرجع 0;
    }
    إذا(string_بازه!=اشاره_تهی) {
        أرجع string_بازه->بازه_پشت(s, length, بازه_Condition);
    } وإلا إذا(has_رشتهها()) {
        uint32_t which= بازه_Condition==USET_بازه_NOT_CONTAINED ?
                            Unicode_مجموعه_String_بازه::پشت_UTF16_NOT_CONTAINED :
                            Unicode_مجموعه_String_بازه::پشت_UTF16_CONTAINED;
        Unicode_مجموعه_String_بازه str_بازه(*هذا, *رشتهها_2, which);
        إذا(str_بازه.needs_String_بازه_UTF16()) {
            أرجع str_بازه.بازه_پشت(s, length, بازه_Condition);
        }
    }

    إذا(بازه_Condition!=USET_بازه_NOT_CONTAINED) {
        بازه_Condition=USET_بازه_CONTAINED;  // Pin to 0/1 values.
    }

    UChar32 c;
    int32_t prev=length;
    افعل {
        U16_PREV(s, 0, length, c);
        إذا(بازه_Condition!=شامل_است(c)) {
            اكسر;
        }
    } طالما((prev=length)>0);
    أرجع prev;
}

int32_t Unicode_مجموعه::بازه_UTF8(ثابت حرف *s, int32_t length, USet_بازه_Condition بازه_Condition) ثابت {
    إذا(length>0 && bmp_مجموعه!=اشاره_تهی) {
        ثابت uint8_t* s0 = reinterpret_cast<ثابت uint8_t*>(s);
        أرجع static_cast<int32_t>(bmp_مجموعه->بازه_UTF8(s0, length, بازه_Condition) - s0);
    }
    إذا(length<0) {
        length = static_cast<int32_t>(uprv_strlen(s));
    }
    إذا(length==0) {
        أرجع 0;
    }
    إذا(string_بازه!=اشاره_تهی) {
        أرجع string_بازه->بازه_UTF8(reinterpret_cast<ثابت uint8_t*>(s), length, بازه_Condition);
    } وإلا إذا(has_رشتهها()) {
        uint32_t which= بازه_Condition==USET_بازه_NOT_CONTAINED ?
                            Unicode_مجموعه_String_بازه::FWD_UTF8_NOT_CONTAINED :
                            Unicode_مجموعه_String_بازه::FWD_UTF8_CONTAINED;
        Unicode_مجموعه_String_بازه str_بازه(*هذا, *رشتهها_2, which);
        إذا(str_بازه.needs_String_بازه_UTF8()) {
            أرجع str_بازه.بازه_UTF8(reinterpret_cast<ثابت uint8_t*>(s), length, بازه_Condition);
        }
    }

    إذا(بازه_Condition!=USET_بازه_NOT_CONTAINED) {
        بازه_Condition=USET_بازه_CONTAINED;  // Pin to 0/1 values.
    }

    UChar32 c;
    int32_t شروع=0, prev=0;
    افعل {
        U8_بعدی_OR_FFFD(s, شروع, length, c);
        إذا(بازه_Condition!=شامل_است(c)) {
            اكسر;
        }
    } طالما((prev=شروع)<length);
    أرجع prev;
}

int32_t Unicode_مجموعه::بازه_پشت_UTF8(ثابت حرف *s, int32_t length, USet_بازه_Condition بازه_Condition) ثابت {
    إذا(length>0 && bmp_مجموعه!=اشاره_تهی) {
        ثابت uint8_t* s0 = reinterpret_cast<ثابت uint8_t*>(s);
        أرجع bmp_مجموعه->بازه_پشت_UTF8(s0, length, بازه_Condition);
    }
    إذا(length<0) {
        length = static_cast<int32_t>(uprv_strlen(s));
    }
    إذا(length==0) {
        أرجع 0;
    }
    إذا(string_بازه!=اشاره_تهی) {
        أرجع string_بازه->بازه_پشت_UTF8(reinterpret_cast<ثابت uint8_t*>(s), length, بازه_Condition);
    } وإلا إذا(has_رشتهها()) {
        uint32_t which= بازه_Condition==USET_بازه_NOT_CONTAINED ?
                            Unicode_مجموعه_String_بازه::پشت_UTF8_NOT_CONTAINED :
                            Unicode_مجموعه_String_بازه::پشت_UTF8_CONTAINED;
        Unicode_مجموعه_String_بازه str_بازه(*هذا, *رشتهها_2, which);
        إذا(str_بازه.needs_String_بازه_UTF8()) {
            أرجع str_بازه.بازه_پشت_UTF8(reinterpret_cast<ثابت uint8_t*>(s), length, بازه_Condition);
        }
    }

    إذا(بازه_Condition!=USET_بازه_NOT_CONTAINED) {
        بازه_Condition=USET_بازه_CONTAINED;  // Pin to 0/1 values.
    }

    UChar32 c;
    int32_t prev=length;
    افعل {
        U8_PREV_OR_FFFD(s, 0, length, c);
        إذا(بازه_Condition!=شامل_است(c)) {
            اكسر;
        }
    } طالما((prev=length)>0);
    أرجع prev;
}

U_NAMESPACE_انتها
