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
#تعریف_کړه UNICODESET_لوړ 0x0110000

// LOW <= all valid values. ZERO for codepoints
#تعریف_کړه UNICODESET_ټیټ 0x000000

/** Max list [0, 1, 2, ..., max code point, HIGH] */
constexpr int32_t MAX_اوږدتیا = UNICODESET_لوړ + 1;

U_NAMESPACE_BEGIN

نښه_جدول::~نښه_جدول() {}

UOBJECT_DEFINE_RTTI_IMPLEMENTATION(Unicode_سېټ)

/**
 * Modify the given UChar32 variable so that it is in range, by
 * pinning values < UNICODESET_LOW to UNICODESET_LOW, and
 * pinning values > UNICODESET_HIGH-1 to UNICODESET_HIGH-1.
 * It modifies its argument in-place and also returns it.
 */
ساكن مضمن UChar32 pin_کوډ_ټکی(UChar32& c) {
    إذا (c < UNICODESET_ټیټ) {
        c = UNICODESET_ټیټ;
    } وإلا إذا (c > (UNICODESET_لوړ-1)) {
        c = (UNICODESET_لوړ-1);
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

#که_تعریف_شوی DEBUG_MEM
#ضمن <stdio.h>
ساكن int32_t dbg_شمېر = 0;

ساكن مضمن فراغ _dbgct(Unicode_سېټ* سېټ) {
    UnicodeString str;
    سېټ->منزل_نمونه(str, ریښتیا);
    حرف buf[40];
    str.extract(0, 39, buf, "");
    چاپ_کړه("DEBUG UnicodeSet: ct 0x%08X; %d %s\n", سېټ, ++dbg_شمېر, buf);
}

ساكن مضمن فراغ _dbgdt(Unicode_سېټ* سېټ) {
    UnicodeString str;
    سېټ->منزل_نمونه(str, ریښتیا);
    حرف buf[40];
    str.extract(0, 39, buf, "");
    چاپ_کړه("DEBUG UnicodeSet: dt 0x%08X; %d %s\n", سېټ, --dbg_شمېر, buf);
}

#وإلا

#تعریف_کړه _dbgct(سېټ)
#تعریف_کړه _dbgdt(سېټ)

#د_که_پای

//----------------------------------------------------------------
// UnicodeString in UVector support
//----------------------------------------------------------------

ساكن فراغ U_CALLCONV cloneUnicodeString(UElement *dst, UElement *src) {
    dst->اشاره_ګر = جديد UnicodeString(*static_cast<UnicodeString*>(src->اشاره_ګر));
}

ساكن int32_t U_CALLCONV compareUnicodeString(UElement t1, UElement t2) {
    ثابت UnicodeString& a = *static_cast<ثابت UnicodeString*>(t1.اشاره_ګر);
    ثابت UnicodeString& b = *static_cast<ثابت UnicodeString*>(t2.اشاره_ګر);
    أرجع a.compare(b);
}

UBool Unicode_سېټ::has_مزي_ګانې() ثابت {
    أرجع مزي_ګانې_2 != تش_اشاره && !مزي_ګانې_2->is_تشه();
}

int32_t Unicode_سېټ::مزي_ګانې_اندازه() ثابت {
    أرجع مزي_ګانې_2 == تش_اشاره ? 0 : مزي_ګانې_2->size();
}

UBool Unicode_سېټ::مزي_ګانې_لري(ثابت UnicodeString &s) ثابت {
    أرجع مزي_ګانې_2 != تش_اشاره && مزي_ګانې_2->لري((فراغ*) &s);
}

//----------------------------------------------------------------
// Constructors &c
//----------------------------------------------------------------

/**
 * Constructs an empty set.
 */
Unicode_سېټ::Unicode_سېټ() {
    list[0] = UNICODESET_لوړ;
    _dbgct(هذا);
}

/**
 * Constructs a set containing the given range. If <code>end >
 * start</code> then an empty set is created.
 *
 * @param start first character, inclusive, of range
 * @param end last character, inclusive, of range
 */
Unicode_سېټ::Unicode_سېټ(UChar32 پیل, UChar32 پای_ټکی) {
    list[0] = UNICODESET_لوړ;
    جمع_کړه(پیل, پای_ټکی);
    _dbgct(هذا);
}

/**
 * Constructs a set that is identical to the given UnicodeSet.
 */
Unicode_سېټ::Unicode_سېټ(ثابت Unicode_سېټ& o) : UnicodeFilter(o) {
    *هذا = o;
    _dbgct(هذا);
}

// Copy-construct as thawed.
Unicode_سېټ::Unicode_سېټ(ثابت Unicode_سېټ& o, UBool /* asThawed */) : UnicodeFilter(o) {
    إذا (ensure_ظرفیت(o.len)) {
        // *this = o except for bmpSet and stringSpan
        len = o.len;
        uprv_memcpy(list, o.list, (size_t)len*حجم(UChar32));
        إذا (o.has_مزي_ګانې()) {
            UError_کوډ حالت = U_ZERO_ERROR;
            إذا (!allocate_مزي_ګانې(حالت) ||
                    (مزي_ګانې_2->assign(*o.مزي_ګانې_2, cloneUnicodeString, حالت), U_FAILURE(حالت))) {
                سېټ_منزل_Bogus();
                أرجع;
            }
        }
        إذا (o.pat) {
            سېټ_نمونه(o.pat, o.patLen);
        }
        _dbgct(هذا);
    }
}

/**
 * Destructs the set.
 */
Unicode_سېټ::~Unicode_سېټ() {
    _dbgdt(هذا); // first!
    إذا (list != ډېری_List) {
        uprv_free(list);
    }
    ړنګول bmp_سېټ;
    إذا (بفر != ډېری_List) {
        uprv_free(بفر);
    }
    ړنګول مزي_ګانې_2;
    ړنګول string_پراختیا;
    release_نمونه();
}

/**
 * Assigns this object to be a copy of another.
 */
Unicode_سېټ& Unicode_سېټ::چلوونکی=(ثابت Unicode_سېټ& o) U_LIFETIME_BOUND {
    أرجع copyFrom(o, خطأ);
}

Unicode_سېټ& Unicode_سېټ::copyFrom(ثابت Unicode_سېټ& o, UBool asThawed) U_LIFETIME_BOUND {
    إذا (هذا == &o) {
        أرجع *هذا;
    }
    إذا (isFrozen()) {
        أرجع *هذا;
    }
    إذا (o.isBogus()) {
        سېټ_منزل_Bogus();
        أرجع *هذا;
    }
    إذا (!ensure_ظرفیت(o.len)) {
        // ensureCapacity will mark the UnicodeSet as Bogus if OOM failure happens.
        أرجع *هذا;
    }
    len = o.len;
    uprv_memcpy(list, o.list, (size_t)len*حجم(UChar32));
    إذا (o.bmp_سېټ != تش_اشاره && !asThawed) {
        bmp_سېټ = جديد BMPSet(*o.bmp_سېټ, list, len);
        إذا (bmp_سېټ == تش_اشاره) { // Check for memory allocation error.
            سېټ_منزل_Bogus();
            أرجع *هذا;
        }
    }
    إذا (o.has_مزي_ګانې()) {
        UError_کوډ حالت = U_ZERO_ERROR;
        إذا ((مزي_ګانې_2 == تش_اشاره && !allocate_مزي_ګانې(حالت)) ||
                (مزي_ګانې_2->assign(*o.مزي_ګانې_2, cloneUnicodeString, حالت), U_FAILURE(حالت))) {
            سېټ_منزل_Bogus();
            أرجع *هذا;
        }
    } وإلا إذا (has_مزي_ګانې()) {
        مزي_ګانې_2->removeAllElements();
    }
    إذا (o.string_پراختیا != تش_اشاره && مزي_ګانې_2 != تش_اشاره && !asThawed) {
        string_پراختیا = جديد Unicode_سېټ_String_پراختیا(*o.string_پراختیا, *مزي_ګانې_2);
        إذا (string_پراختیا == تش_اشاره) { // Check for memory allocation error.
            سېټ_منزل_Bogus();
            أرجع *هذا;
        }
    }
    release_نمونه();
    إذا (o.pat) {
        سېټ_نمونه(o.pat, o.patLen);
    }
    أرجع *هذا;
}

/**
 * Returns a copy of this object.  All UnicodeMatcher objects have
 * to support cloning in order to allow classes using
 * UnicodeMatchers, such as Transliterator, to implement cloning.
 */
Unicode_سېټ* Unicode_سېټ::clone() ثابت {
    أرجع جديد Unicode_سېټ(*هذا);
}

Unicode_سېټ *Unicode_سېټ::cloneAsThawed() ثابت {
    أرجع جديد Unicode_سېټ(*هذا, ریښتیا);
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
منطقي Unicode_سېټ::چلوونکی==(ثابت Unicode_سېټ& o) ثابت {
    إذا (len != o.len) أرجع خطأ;
    لكل (int32_t i = 0; i < len; ++i) {
        إذا (list[i] != o.list[i]) أرجع خطأ;
    }
    إذا (has_مزي_ګانې() != o.has_مزي_ګانې()) { أرجع خطأ; }
    إذا (has_مزي_ګانې() && *مزي_ګانې_2 != *o.مزي_ګانې_2) أرجع خطأ;
    أرجع ریښتیا;
}

/**
 * Returns the hash code value for this set.
 *
 * @return the hash code value for this set.
 * @see Object#hashCode()
 */
int32_t Unicode_سېټ::hash_کوډ() ثابت {
    uint32_t پایله = static_cast<uint32_t>(len);
    لكل (int32_t i = 0; i < len; ++i) {
        پایله *= 1000003u;
        پایله += list[i];
    }
    أرجع static_cast<int32_t>(پایله);
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
int32_t Unicode_سېټ::size() ثابت {
    int32_t n = 0;
    int32_t count = get_Range_شمېر();
    لكل (int32_t i = 0; i < count; ++i) {
        n += get_Range_پای_ټکی(i) - get_Range_پیل(i) + 1;
    }
    أرجع n + مزي_ګانې_اندازه();
}

/**
 * Returns <tt>true</tt> if this set contains no elements.
 *
 * @return <tt>true</tt> if this set contains no elements.
 */
UBool Unicode_سېټ::is_تشه() ثابت {
    أرجع len == 1 && !has_مزي_ګانې();
}

/**
 * Returns true if this set contains the given character.
 * @param c character to be checked for containment
 * @return true if the test condition is met
 */
UBool Unicode_سېټ::لري(UChar32 c) ثابت {
    // Set i to the index of the start item greater than ch
    // We know we will terminate without length test!
    // LATER: for large sets, add binary search
    //int32_t i = -1;
    //for (;;) {
    //    if (c < list[++i]) break;
    //}
    إذا (bmp_سېټ != تش_اشاره) {
        أرجع bmp_سېټ->لري(c);
    }
    إذا (string_پراختیا != تش_اشاره) {
        أرجع string_پراختیا->لري(c);
    }
    إذا (c >= UNICODESET_لوړ) { // Don't need to check LOW bound
        أرجع خطأ;
    }
    int32_t i = پیدا_کړه_کوډ_ټکی(c);
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
int32_t Unicode_سېټ::پیدا_کړه_کوډ_ټکی(UChar32 c) ثابت {
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
UBool Unicode_سېټ::لري(UChar32 پیل, UChar32 پای_ټکی) ثابت {
    //int32_t i = -1;
    //for (;;) {
    //    if (start < list[++i]) break;
    //}
    int32_t i = پیدا_کړه_کوډ_ټکی(پیل);
    أرجع ((i & 1) != 0 && پای_ټکی < list[i]);
}

/**
 * Returns <tt>true</tt> if this set contains the given
 * multicharacter string.
 * @param s string to be checked for containment
 * @return <tt>true</tt> if this set contains the specified string
 */
UBool Unicode_سېټ::لري(ثابت UnicodeString& s) ثابت {
    int32_t cp = getSingleCP(s);
    إذا (cp < 0) {
        أرجع مزي_ګانې_لري(s);
    } وإلا {
        أرجع لري(static_cast<UChar32>(cp));
    }
}

/**
 * Returns true if this set contains all the characters and strings
 * of the given set.
 * @param c set to be checked for containment
 * @return true if the test condition is met
 */
UBool Unicode_سېټ::لري_All(ثابت Unicode_سېټ& c) ثابت {
    // The specified set is a subset if all of its pairs are contained in
    // this set.  It's possible to code this more efficiently in terms of
    // direct manipulation of the inversion lists if the need arises.
    int32_t n = c.get_Range_شمېر();
    لكل (صحيح i=0; i<n; ++i) {
        إذا (!لري(c.get_Range_پیل(i), c.get_Range_پای_ټکی(i))) {
            أرجع خطأ;
        }
    }
    أرجع !c.has_مزي_ګانې() || (مزي_ګانې_2 != تش_اشاره && مزي_ګانې_2->لري_All(*c.مزي_ګانې_2));
}

/**
 * Returns true if this set contains all the characters
 * of the given string.
 * @param s string containing characters to be checked for containment
 * @return true if the test condition is met
 */
UBool Unicode_سېټ::لري_All(ثابت UnicodeString& s) ثابت {
    أرجع پراختیا(s.get_بفر(), s.length(), USET_پراختیا_CONTAINED) == s.length();
}

/**
 * Returns true if this set contains none of the characters
 * of the given range.
 * @param start first character, inclusive, of the range
 * @param end last character, inclusive, of the range
 * @return true if the test condition is met
 */
UBool Unicode_سېټ::لري_None(UChar32 پیل, UChar32 پای_ټکی) ثابت {
    //int32_t i = -1;
    //for (;;) {
    //    if (start < list[++i]) break;
    //}
    int32_t i = پیدا_کړه_کوډ_ټکی(پیل);
    أرجع ((i & 1) == 0 && پای_ټکی < list[i]);
}

/**
 * Returns true if this set contains none of the characters and strings
 * of the given set.
 * @param c set to be checked for containment
 * @return true if the test condition is met
 */
UBool Unicode_سېټ::لري_None(ثابت Unicode_سېټ& c) ثابت {
    // The specified set is a subset if all of its pairs are contained in
    // this set.  It's possible to code this more efficiently in terms of
    // direct manipulation of the inversion lists if the need arises.
    int32_t n = c.get_Range_شمېر();
    لكل (int32_t i=0; i<n; ++i) {
        إذا (!لري_None(c.get_Range_پیل(i), c.get_Range_پای_ټکی(i))) {
            أرجع خطأ;
        }
    }
    أرجع مزي_ګانې_2 == تش_اشاره || !c.has_مزي_ګانې() || مزي_ګانې_2->لري_None(*c.مزي_ګانې_2);
}

/**
 * Returns true if this set contains none of the characters
 * of the given string.
 * @param s string containing characters to be checked for containment
 * @return true if the test condition is met
 */
UBool Unicode_سېټ::لري_None(ثابت UnicodeString& s) ثابت {
    أرجع پراختیا(s.get_بفر(), s.length(), USET_پراختیا_NOT_CONTAINED) == s.length();
}

/**
 * Returns <tt>true</tt> if this set contains any character whose low byte
 * is the given value.  This is used by <tt>RuleBasedTransliterator</tt> for
 * indexing.
 */
UBool Unicode_سېټ::matches_شاخص_ارزښت(uint8_t v) ثابت {
    /* The index value v, in the range [0,255], is contained in this set if
     * it is contained in any pair of this set.  Pairs either have the high
     * bytes equal, or unequal.  If the high bytes are equal, then we have
     * aaxx..aayy, where aa is the high byte.  Then v is contained if xx <=
     * v <= yy.  If the high bytes are unequal we have aaxx..bbyy, bb>aa.
     * Then v is contained if xx <= v || v <= yy.  (This is identical to the
     * time zone month containment logic.)
     */
    int32_t i;
    int32_t range_شمېر=get_Range_شمېر();
    لكل (i=0; i<range_شمېر; ++i) {
        UChar32 ټیټ = get_Range_پیل(i);
        UChar32 لوړ = get_Range_پای_ټکی(i);
        إذا ((ټیټ & ~0xFF) == (لوړ & ~0xFF)) {
            إذا ((ټیټ & 0xFF) <= v && v <= (لوړ & 0xFF)) {
                أرجع ریښتیا;
            }
        } وإلا إذا ((ټیټ & 0xFF) <= v || v <= (لوړ & 0xFF)) {
            أرجع ریښتیا;
        }
    }
    إذا (has_مزي_ګانې()) {
        لكل (i=0; i<مزي_ګانې_2->size(); ++i) {
            ثابت UnicodeString& s = *static_cast<ثابت UnicodeString*>(مزي_ګانې_2->elementAt(i));
            إذا (s.is_تشه()) {
                استمر;  // skip the empty string
            }
            UChar32 c = s.char32At(0);
            إذا ((c & 0xFF) == v) {
                أرجع ریښتیا;
            }
        }
    }
    أرجع خطأ;
}

/**
 * Implementation of UnicodeMatcher::matches().  Always matches the
 * longest possible multichar string.
 */
UMatchDegree Unicode_سېټ::matches(ثابت Replaceable& متن,
                                 int32_t& دننه_واټن,
                                 int32_t حد,
                                 UBool incremental) {
    إذا (دننه_واټن == حد) {
        إذا (لري(U_ETHER)) {
            أرجع incremental ? U_PARTIAL_سمون : U_سمون;
        } وإلا {
            أرجع U_MISMATCH;
        }
    } وإلا {
        إذا (has_مزي_ګانې()) { // try strings first

            // might separate forward and backward loops later
            // for now they are combined

            // TODO Improve efficiency of this, at least in the forward
            // direction, if not in both.  In the forward direction we
            // can assume the strings are sorted.

            int32_t i;
            UBool forward = دننه_واټن < حد;

            // firstChar is the leftmost char to match in the
            // forward direction or the rightmost char to match in
            // the reverse direction.
            char16_t لومړی_Char = متن.charAt(دننه_واټن);

            // If there are multiple strings that can match we
            // return the longest match.
            int32_t لوړ_Water_اوږدتیا = 0;

            لكل (i=0; i<مزي_ګانې_2->size(); ++i) {
                ثابت UnicodeString& trial = *static_cast<ثابت UnicodeString*>(مزي_ګانې_2->elementAt(i));
                إذا (trial.is_تشه()) {
                    استمر;  // skip the empty string
                }

                char16_t c = trial.charAt(forward ? 0 : trial.length() - 1);

                // Strings are sorted, so we can optimize in the
                // forward direction.
                إذا (forward && c > لومړی_Char) اكسر;
                إذا (c != لومړی_Char) استمر;

                int32_t سمون_Len = سمون_Rest(متن, دننه_واټن, حد, trial);

                إذا (incremental) {
                    int32_t maxLen = forward ? حد-دننه_واټن : دننه_واټن-حد;
                    إذا (سمون_Len == maxLen) {
                        // We have successfully matched but only up to limit.
                        أرجع U_PARTIAL_سمون;
                    }
                }

                إذا (سمون_Len == trial.length()) {
                    // We have successfully matched the whole string.
                    إذا (سمون_Len > لوړ_Water_اوږدتیا) {
                        لوړ_Water_اوږدتیا = سمون_Len;
                    }
                    // In the forward direction we know strings
                    // are sorted so we can bail early.
                    إذا (forward && سمون_Len < لوړ_Water_اوږدتیا) {
                        اكسر;
                    }
                    استمر;
                }
            }

            // We've checked all strings without a partial match.
            // If we have full matches, return the longest one.
            إذا (لوړ_Water_اوږدتیا != 0) {
                دننه_واټن += forward ? لوړ_Water_اوږدتیا : -لوړ_Water_اوږدتیا;
                أرجع U_سمون;
            }
        }
        أرجع UnicodeFilter::matches(متن, دننه_واټن, حد, incremental);
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
int32_t Unicode_سېټ::سمون_Rest(ثابت Replaceable& متن,
                              int32_t پیل, int32_t حد,
                              ثابت UnicodeString& s) {
    int32_t i;
    int32_t maxLen;
    int32_t slen = s.length();
    إذا (پیل < حد) {
        maxLen = حد - پیل;
        إذا (maxLen > slen) maxLen = slen;
        لكل (i = 1; i < maxLen; ++i) {
            إذا (متن.charAt(پیل + i) != s.charAt(i)) أرجع 0;
        }
    } وإلا {
        maxLen = پیل - حد;
        إذا (maxLen > slen) maxLen = slen;
        --slen; // <=> slen = s.length() - 1;
        لكل (i = 1; i < maxLen; ++i) {
            إذا (متن.charAt(پیل - i) != s.charAt(slen - i)) أرجع 0;
        }
    }
    أرجع maxLen;
}

/**
 * Implement of UnicodeMatcher
 */
فراغ Unicode_سېټ::جمع_کړه_سمون_سېټ_منزل(Unicode_سېټ& منزل_Union_منزل) ثابت {
    منزل_Union_منزل.جمع_کړه_All(*هذا);
}

/**
 * Returns the index of the given character within this set, where
 * the set is ordered by ascending code point.  If the character
 * is not in this set, return -1.  The inverse of this method is
 * <code>charAt()</code>.
 * @return an index from 0..size()-1, or -1
 */
int32_t Unicode_سېټ::شاخص_د(UChar32 c) ثابت {
    إذا (c < MIN_ارزښت || c > MAX_ارزښت) {
        أرجع -1;
    }
    int32_t i = 0;
    int32_t n = 0;
    لكل (;;) {
        UChar32 پیل = list[i++];
        إذا (c < پیل) {
            أرجع -1;
        }
        UChar32 حد = list[i++];
        إذا (c < حد) {
            أرجع n + c - پیل;
        }
        n += حد - پیل;
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
UChar32 Unicode_سېټ::charAt(int32_t شاخص) ثابت {
    إذا (شاخص >= 0) {
        // len2 is the largest even integer <= len, that is, it is len
        // for even values and len-1 for odd values.  With odd values
        // the last entry is UNICODESET_HIGH.
        int32_t len2 = len & ~1;
        لكل (int32_t i=0; i < len2;) {
            UChar32 پیل = list[i++];
            int32_t count = list[i++] - پیل;
            إذا (شاخص < count) {
                أرجع static_cast<UChar32>(پیل + شاخص);
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
Unicode_سېټ& Unicode_سېټ::سېټ(UChar32 پیل, UChar32 پای_ټکی) U_LIFETIME_BOUND {
    clear();
    complement(پیل, پای_ټکی);
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
Unicode_سېټ& Unicode_سېټ::جمع_کړه(UChar32 پیل, UChar32 پای_ټکی) U_LIFETIME_BOUND {
    إذا (pin_کوډ_ټکی(پیل) < pin_کوډ_ټکی(پای_ټکی)) {
        UChar32 حد = پای_ټکی + 1;
        // Fast path for adding a new range after the last one.
        // Odd list length: [..., lastStart, lastLimit, HIGH]
        إذا ((len & 1) != 0) {
            // If the list is empty, set lastLimit low enough to not be adjacent to 0.
            UChar32 وروستی_حد = len == 1 ? -2 : list[len - 2];
            إذا (وروستی_حد <= پیل && !isFrozen() && !isBogus()) {
                إذا (وروستی_حد == پیل) {
                    // Extend the last range.
                    list[len - 2] = حد;
                    إذا (حد == UNICODESET_لوړ) {
                        --len;
                    }
                } وإلا {
                    list[len - 1] = پیل;
                    إذا (حد < UNICODESET_لوړ) {
                        إذا (ensure_ظرفیت(len + 2)) {
                            list[len++] = حد;
                            list[len++] = UNICODESET_لوړ;
                        }
                    } وإلا {  // limit == UNICODESET_HIGH
                        إذا (ensure_ظرفیت(len + 1)) {
                            list[len++] = UNICODESET_لوړ;
                        }
                    }
                }
                release_نمونه();
                أرجع *هذا;
            }
        }
        // This is slow. Could be much faster using findCodePoint(start)
        // and modifying the list, dealing with adjacent & overlapping ranges.
        UChar32 range[3] = { پیل, حد, UNICODESET_لوړ };
        جمع_کړه(range, 2, 0);
    } وإلا إذا (پیل == پای_ټکی) {
        جمع_کړه(پیل);
    }
    أرجع *هذا;
}

// #define DEBUG_US_ADD

#که_تعریف_شوی DEBUG_US_جمع_کړه
#ضمن <stdio.h>
فراغ dump(UChar32 c) {
    إذا (c <= 0xFF) {
        چاپ_کړه("%c", (حرف)c);
    } وإلا {
        چاپ_کړه("U+%04X", c);
    }
}
فراغ dump(ثابت UChar32* list, int32_t len) {
    چاپ_کړه("[");
    لكل (int32_t i=0; i<len; ++i) {
        إذا (i != 0) چاپ_کړه(", ");
        dump(list[i]);
    }
    چاپ_کړه("]");
}
#د_که_پای

/**
 * Adds the specified character to this set if it is not already
 * present.  If this set already contains the specified character,
 * the call leaves this set unchanged.
 */
Unicode_سېټ& Unicode_سېټ::جمع_کړه(UChar32 c) U_LIFETIME_BOUND {
    // find smallest i such that c < list[i]
    // if odd, then it is IN the set
    // if even, then it is OUT of the set
    int32_t i = پیدا_کړه_کوډ_ټکی(pin_کوډ_ټکی(c));

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

#که_تعریف_شوی DEBUG_US_جمع_کړه
    چاپ_کړه("Add of ");
    dump(c);
    چاپ_کړه(" found at %d", i);
    چاپ_کړه(": ");
    dump(list, len);
    چاپ_کړه(" => ");
#د_که_پای

    إذا (c == list[i]-1) {
        // c is before start of next range
        list[i] = c;
        // if we touched the HIGH mark, then add a new one
        إذا (c == (UNICODESET_لوړ - 1)) {
            إذا (!ensure_ظرفیت(len+1)) {
                // ensureCapacity will mark the object as Bogus if OOM failure happens.
                أرجع *هذا;
            }
            list[len++] = UNICODESET_لوړ;
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

#که_تعریف_شوی DEBUG_US_جمع_کړه
    dump(list, len);
    چاپ_کړه("\n");

    لكل (i=1; i<len; ++i) {
        إذا (list[i] <= list[i-1]) {
            // Corrupt array!
            چاپ_کړه("ERROR: list has been corrupted\n");
            وتل(1);
        }
    }
#د_که_پای

    release_نمونه();
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
Unicode_سېټ& Unicode_سېټ::جمع_کړه(ثابت UnicodeString& s) U_LIFETIME_BOUND {
    إذا (isFrozen() || isBogus()) أرجع *هذا;
    int32_t cp = getSingleCP(s);
    إذا (cp < 0) {
        إذا (!مزي_ګانې_لري(s)) {
            جمع_کړه_2(s);
            release_نمونه();
        }
    } وإلا {
        جمع_کړه(static_cast<UChar32>(cp));
    }
    أرجع *هذا;
}

/**
 * Adds the given string, in order, to 'strings_'.  The given string
 * must have been checked by the caller to not already be in 'strings_'.
 */
فراغ Unicode_سېټ::جمع_کړه_2(ثابت UnicodeString& s) {
    إذا (isFrozen() || isBogus()) {
        أرجع;
    }
    UError_کوډ ec = U_ZERO_ERROR;
    إذا (مزي_ګانې_2 == تش_اشاره && !allocate_مزي_ګانې(ec)) {
        سېټ_منزل_Bogus();
        أرجع;
    }
    Local_اشاره_ګر<UnicodeString> t(جديد UnicodeString(s));
    إذا (t.isNull()) { // Check for memory allocation error.
        سېټ_منزل_Bogus();
        أرجع;
    }
    مزي_ګانې_2->sorted_ورننباسه(t.orphan(), compareUnicodeString, ec);
    إذا (U_FAILURE(ec)) {
        سېټ_منزل_Bogus();
    }
}

/**
 * @return a code point IF the string consists of a single one.
 * otherwise returns -1.
 * @param string to test
 */
int32_t Unicode_سېټ::getSingleCP(ثابت UnicodeString& s) {
    int32_t s_اوږدتیا = s.length();
    إذا (s_اوږدتیا == 1) أرجع s.charAt(0);
    إذا (s_اوږدتیا == 2) {
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
Unicode_سېټ& Unicode_سېټ::جمع_کړه_All(ثابت UnicodeString& s) U_LIFETIME_BOUND {
    UChar32 cp;
    لكل (int32_t i = 0; i < s.length(); i += U16_اوږدتیا(cp)) {
        cp = s.char32At(i);
        جمع_کړه(cp);
    }
    أرجع *هذا;
}

/**
 * Retains EACH of the characters in this string. Note: "ch" == {"c", "h"}
 * If this set already any particular character, it has no effect on that character.
 * @param the source string
 * @return the modified set, for chaining
 */
Unicode_سېټ& Unicode_سېټ::retainAll(ثابت UnicodeString& s) U_LIFETIME_BOUND {
    Unicode_سېټ سېټ;
    سېټ.جمع_کړه_All(s);
    retainAll(سېټ);
    أرجع *هذا;
}

/**
 * Complement EACH of the characters in this string. Note: "ch" == {"c", "h"}
 * If this set already any particular character, it has no effect on that character.
 * @param the source string
 * @return the modified set, for chaining
 */
Unicode_سېټ& Unicode_سېټ::complementAll(ثابت UnicodeString& s) U_LIFETIME_BOUND {
    Unicode_سېټ سېټ;
    سېټ.جمع_کړه_All(s);
    complementAll(سېټ);
    أرجع *هذا;
}

/**
 * Remove EACH of the characters in this string. Note: "ch" == {"c", "h"}
 * If this set already any particular character, it has no effect on that character.
 * @param the source string
 * @return the modified set, for chaining
 */
Unicode_سېټ& Unicode_سېټ::removeAll(ثابت UnicodeString& s) U_LIFETIME_BOUND {
    Unicode_سېټ سېټ;
    سېټ.جمع_کړه_All(s);
    removeAll(سېټ);
    أرجع *هذا;
}

Unicode_سېټ& Unicode_سېټ::remove_All_مزي_ګانې() U_LIFETIME_BOUND {
    إذا (!isFrozen() && has_مزي_ګانې()) {
        مزي_ګانې_2->removeAllElements();
        release_نمونه();
    }
    أرجع *هذا;
}


/**
 * Makes a set from a multicharacter string. Thus "ch" => {"ch"}
 * <br><b>Warning: you cannot add an empty string ("") to a UnicodeSet.</b>
 * @param the source string
 * @return a newly created set containing the given string
 */
Unicode_سېټ* U_EXPORT2 Unicode_سېټ::createFrom(ثابت UnicodeString& s) {
    Unicode_سېټ *سېټ = جديد Unicode_سېټ();
    إذا (سېټ != تش_اشاره) { // Check for memory allocation error.
        سېټ->جمع_کړه(s);
    }
    أرجع سېټ;
}


/**
 * Makes a set from each of the characters in the string. Thus "ch" => {"c", "h"}
 * @param the source string
 * @return a newly created set containing the given characters
 */
Unicode_سېټ* U_EXPORT2 Unicode_سېټ::createFromAll(ثابت UnicodeString& s) {
    Unicode_سېټ *سېټ = جديد Unicode_سېټ();
    إذا (سېټ != تش_اشاره) { // Check for memory allocation error.
        سېټ->جمع_کړه_All(s);
    }
    أرجع سېټ;
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
Unicode_سېټ& Unicode_سېټ::retain(UChar32 پیل, UChar32 پای_ټکی) U_LIFETIME_BOUND {
    إذا (pin_کوډ_ټکی(پیل) <= pin_کوډ_ټکی(پای_ټکی)) {
        UChar32 range[3] = { پیل, پای_ټکی+1, UNICODESET_لوړ };
        retain(range, 2, 0);
    } وإلا {
        clear();
    }
    أرجع *هذا;
}

Unicode_سېټ& Unicode_سېټ::retain(UChar32 c) U_LIFETIME_BOUND {
    أرجع retain(c, c);
}

Unicode_سېټ& Unicode_سېټ::retain(ثابت UnicodeString &s) U_LIFETIME_BOUND {
    إذا (isFrozen() || isBogus()) { أرجع *هذا; }
    UChar32 cp = getSingleCP(s);
    إذا (cp < 0) {
        منطقي is_دننه = مزي_ګانې_لري(s);
        // Check for getRangeCount() first to avoid somewhat-expensive size()
        // when there are single code points.
        إذا (is_دننه && get_Range_شمېر() == 0 && size() == 1) {
            أرجع *هذا;
        }
        clear();
        إذا (is_دننه) {
            جمع_کړه_2(s);
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
Unicode_سېټ& Unicode_سېټ::remove(UChar32 پیل, UChar32 پای_ټکی) U_LIFETIME_BOUND {
    إذا (pin_کوډ_ټکی(پیل) <= pin_کوډ_ټکی(پای_ټکی)) {
        UChar32 range[3] = { پیل, پای_ټکی+1, UNICODESET_لوړ };
        retain(range, 2, 2);
    }
    أرجع *هذا;
}

/**
 * Removes the specified character from this set if it is present.
 * The set will not contain the specified range once the call
 * returns.
 */
Unicode_سېټ& Unicode_سېټ::remove(UChar32 c) U_LIFETIME_BOUND {
    أرجع remove(c, c);
}

/**
 * Removes the specified string from this set if it is present.
 * The set will not contain the specified character once the call
 * returns.
 * @param the source string
 * @return the modified set, for chaining
 */
Unicode_سېټ& Unicode_سېټ::remove(ثابت UnicodeString& s) U_LIFETIME_BOUND {
    إذا (isFrozen() || isBogus()) أرجع *هذا;
    int32_t cp = getSingleCP(s);
    إذا (cp < 0) {
        إذا (مزي_ګانې_2 != تش_اشاره && مزي_ګانې_2->removeElement((فراغ*) &s)) {
            release_نمونه();
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
Unicode_سېټ& Unicode_سېټ::complement(UChar32 پیل, UChar32 پای_ټکی) U_LIFETIME_BOUND {
    إذا (isFrozen() || isBogus()) {
        أرجع *هذا;
    }
    إذا (pin_کوډ_ټکی(پیل) <= pin_کوډ_ټکی(پای_ټکی)) {
        UChar32 range[3] = { پیل, پای_ټکی+1, UNICODESET_لوړ };
        exclusiveOr(range, 2, 0);
    }
    release_نمونه();
    أرجع *هذا;
}

Unicode_سېټ& Unicode_سېټ::complement(UChar32 c) U_LIFETIME_BOUND {
    أرجع complement(c, c);
}

/**
 * This is equivalent to
 * <code>complement(MIN_VALUE, MAX_VALUE)</code>.
 */
Unicode_سېټ& Unicode_سېټ::complement() U_LIFETIME_BOUND {
    إذا (isFrozen() || isBogus()) {
        أرجع *هذا;
    }
    إذا (list[0] == UNICODESET_ټیټ) {
        uprv_memmove(list, list + 1, (size_t)(len-1)*حجم(UChar32));
        --len;
    } وإلا {
        إذا (!ensure_ظرفیت(len+1)) {
            أرجع *هذا;
        }
        uprv_memmove(list + 1, list, (size_t)len*حجم(UChar32));
        list[0] = UNICODESET_ټیټ;
        ++len;
    }
    release_نمونه();
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
Unicode_سېټ& Unicode_سېټ::complement(ثابت UnicodeString& s) U_LIFETIME_BOUND {
    إذا (isFrozen() || isBogus()) أرجع *هذا;
    int32_t cp = getSingleCP(s);
    إذا (cp < 0) {
        إذا (مزي_ګانې_لري(s)) {
            مزي_ګانې_2->removeElement((فراغ*) &s);
        } وإلا {
            جمع_کړه_2(s);
        }
        release_نمونه();
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
Unicode_سېټ& Unicode_سېټ::جمع_کړه_All(ثابت Unicode_سېټ& c) U_LIFETIME_BOUND {
    إذا ( c.len>0 && c.list!=تش_اشاره ) {
        جمع_کړه(c.list, c.len, 0);
    }

    // Add strings in order
    إذا ( c.مزي_ګانې_2!=تش_اشاره ) {
        لكل (int32_t i=0; i<c.مزي_ګانې_2->size(); ++i) {
            ثابت UnicodeString* s = static_cast<ثابت UnicodeString*>(c.مزي_ګانې_2->elementAt(i));
            إذا (!مزي_ګانې_لري(*s)) {
                جمع_کړه_2(*s);
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
Unicode_سېټ& Unicode_سېټ::retainAll(ثابت Unicode_سېټ& c) U_LIFETIME_BOUND {
    إذا (isFrozen() || isBogus()) {
        أرجع *هذا;
    }
    retain(c.list, c.len, 0);
    إذا (has_مزي_ګانې()) {
        إذا (!c.has_مزي_ګانې()) {
            مزي_ګانې_2->removeAllElements();
        } وإلا {
            مزي_ګانې_2->retainAll(*c.مزي_ګانې_2);
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
Unicode_سېټ& Unicode_سېټ::removeAll(ثابت Unicode_سېټ& c) U_LIFETIME_BOUND {
    إذا (isFrozen() || isBogus()) {
        أرجع *هذا;
    }
    retain(c.list, c.len, 2);
    إذا (has_مزي_ګانې() && c.has_مزي_ګانې()) {
        مزي_ګانې_2->removeAll(*c.مزي_ګانې_2);
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
Unicode_سېټ& Unicode_سېټ::complementAll(ثابت Unicode_سېټ& c) U_LIFETIME_BOUND {
    إذا (isFrozen() || isBogus()) {
        أرجع *هذا;
    }
    exclusiveOr(c.list, c.len, 0);

    إذا (c.مزي_ګانې_2 != تش_اشاره) {
        لكل (int32_t i=0; i<c.مزي_ګانې_2->size(); ++i) {
            فراغ* e = c.مزي_ګانې_2->elementAt(i);
            إذا (مزي_ګانې_2 == تش_اشاره || !مزي_ګانې_2->removeElement(e)) {
                جمع_کړه_2(*static_cast<ثابت UnicodeString*>(e));
            }
        }
    }
    أرجع *هذا;
}

/**
 * Removes all of the elements from this set.  This set will be
 * empty after this call returns.
 */
Unicode_سېټ& Unicode_سېټ::clear() U_LIFETIME_BOUND {
    إذا (isFrozen()) {
        أرجع *هذا;
    }
    list[0] = UNICODESET_لوړ;
    len = 1;
    release_نمونه();
    إذا (مزي_ګانې_2 != تش_اشاره) {
        مزي_ګانې_2->removeAllElements();
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
int32_t Unicode_سېټ::get_Range_شمېر() ثابت {
    أرجع len/2;
}

/**
 * Iteration method that returns the first character in the
 * specified range of this set.
 * @see #getRangeCount
 * @see #getRangeEnd
 */
UChar32 Unicode_سېټ::get_Range_پیل(int32_t شاخص) ثابت {
    أرجع list[شاخص*2];
}

/**
 * Iteration method that returns the last character in the
 * specified range of this set.
 * @see #getRangeStart
 * @see #getRangeEnd
 */
UChar32 Unicode_سېټ::get_Range_پای_ټکی(int32_t شاخص) ثابت {
    أرجع list[شاخص*2 + 1] - 1;
}

ثابت UnicodeString* Unicode_سېټ::getString(int32_t شاخص) ثابت U_LIFETIME_BOUND {
    أرجع static_cast<ثابت UnicodeString*>(مزي_ګانې_2->elementAt(شاخص));
}

/**
 * Reallocate this objects internal structures to take up the least
 * possible space, without changing this object's value.
 */
Unicode_سېټ& Unicode_سېټ::compact() U_LIFETIME_BOUND {
    إذا (isFrozen() || isBogus()) {
        أرجع *هذا;
    }
    // Delete buffer first to defragment memory less.
    إذا (بفر != ډېری_List) {
        uprv_free(بفر);
        بفر = تش_اشاره;
        بفر_ظرفیت = 0;
    }
    إذا (list == ډېری_List) {
        // pass
    } وإلا إذا (len <= INITIAL_ظرفیت) {
        uprv_memcpy(ډېری_List, list, len * حجم(UChar32));
        uprv_free(list);
        list = ډېری_List;
        capacity = INITIAL_ظرفیت;
    } وإلا إذا ((len + 7) < capacity) {
        // If we have more than a little unused capacity, shrink it to len.
        UChar32* لنډمهاله = static_cast<UChar32*>(uprv_realloc(list, حجم(UChar32) * len));
        إذا (لنډمهاله) {
            list = لنډمهاله;
            capacity = len;
        }
        // else what the heck happened?! We allocated less memory!
        // Oh well. We'll keep our original array.
    }
    إذا (مزي_ګانې_2 != تش_اشاره && مزي_ګانې_2->is_تشه()) {
        ړنګول مزي_ګانې_2;
        مزي_ګانې_2 = تش_اشاره;
    }
    أرجع *هذا;
}

#که_تعریف_شوی DEBUG_SERIALIZE
#ضمن <stdio.h>
#د_که_پای

/**
 * Deserialize constructor.
 */
Unicode_سېټ::Unicode_سېټ(ثابت uint16_t معلومات[], int32_t معلومات_Len, ESerialization serialization,
                       UError_کوډ &ec) {

  إذا(U_FAILURE(ec)) {
    سېټ_منزل_Bogus();
    أرجع;
  }

  إذا( (serialization != kSerialized)
      || (معلومات==تش_اشاره)
      || (معلومات_Len < 1)) {
    ec = U_ILLEGAL_دلیل_ERROR;
    سېټ_منزل_Bogus();
    أرجع;
  }

  // bmp?
  int32_t سرلیک_اندازه = ((معلومات[0]&0x8000)) ?2:1;
  int32_t bmp_اوږدتیا = (سرلیک_اندازه==1)?معلومات[0]:معلومات[1];

  int32_t new_اوږدتیا = (((معلومات[0]&0x7FFF)-bmp_اوږدتیا)/2)+bmp_اوږدتیا;
#که_تعریف_شوی DEBUG_SERIALIZE
  چاپ_کړه("dataLen %d headerSize %d bmpLen %d len %d. data[0]=%X/%X/%X/%X\n", معلومات_Len,سرلیک_اندازه,bmp_اوږدتیا,new_اوږدتیا, معلومات[0],معلومات[1],معلومات[2],معلومات[3]);
#د_که_پای
  إذا(!ensure_ظرفیت(new_اوږدتیا + 1)) {  // +1 for HIGH
    أرجع;
  }
  // copy bmp
  int32_t i;
  لكل(i = 0; i< bmp_اوږدتیا;i++) {
    list[i] = معلومات[i+سرلیک_اندازه];
#که_تعریف_شوی DEBUG_SERIALIZE
    چاپ_کړه("<<16@%d[%d] %X\n", i+سرلیک_اندازه, i, list[i]);
#د_که_پای
  }
  // copy smp
  لكل(i=bmp_اوږدتیا;i<new_اوږدتیا;i++) {
    list[i] = (static_cast<UChar32>(معلومات[سرلیک_اندازه + bmp_اوږدتیا + (i - bmp_اوږدتیا) * 2 + 0]) << 16) +
               static_cast<UChar32>(معلومات[سرلیک_اندازه + bmp_اوږدتیا + (i - bmp_اوږدتیا) * 2 + 1]);
#که_تعریف_شوی DEBUG_SERIALIZE
    چاپ_کړه("<<32@%d+[%d] %lX\n", سرلیک_اندازه+bmp_اوږدتیا+i, i, list[i]);
#د_که_پای
  }
  U_ASSERT(i == new_اوږدتیا);
  إذا (i == 0 || list[i - 1] != UNICODESET_لوړ) {
    list[i++] = UNICODESET_لوړ;
  }
  len = i;
}


int32_t Unicode_سېټ::serialize(uint16_t *dest, int32_t dest_ظرفیت, UError_کوډ& ec) ثابت {
    int32_t bmp_اوږدتیا, length, dest_اوږدتیا;

    إذا (U_FAILURE(ec)) {
        أرجع 0;
    }

    إذا (dest_ظرفیت<0 || (dest_ظرفیت>0 && dest==تش_اشاره)) {
        ec=U_ILLEGAL_دلیل_ERROR;
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
            ec=U_بفر_ډکېدنه_ERROR;
        }
        أرجع 1;
    }
    /* now length>0 */

    إذا (هذا->list[length-1]<=0xffff) {
        /* all BMP */
        bmp_اوږدتیا=length;
    } وإلا إذا (هذا->list[0]>=0x10000) {
        /* all supplementary */
        bmp_اوږدتیا=0;
        length*=2;
    } وإلا {
        /* some BMP, some supplementary */
        لكل (bmp_اوږدتیا=0; bmp_اوږدتیا<length && هذا->list[bmp_اوږدتیا]<=0xffff; ++bmp_اوږدتیا) {}
        length=bmp_اوږدتیا+2*(length-bmp_اوږدتیا);
    }
#که_تعریف_شوی DEBUG_SERIALIZE
    چاپ_کړه(">> bmpLength%d length%d len%d\n", bmp_اوږدتیا, length, len);
#د_که_پای
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
    dest_اوږدتیا=length+((length>bmp_اوږدتیا)?2:1);
    إذا (dest_اوږدتیا<=dest_ظرفیت) {
        ثابت UChar32 *p;
        int32_t i;

#که_تعریف_شوی DEBUG_SERIALIZE
        چاپ_کړه("writeHdr\n");
#د_که_پای
        *dest = static_cast<uint16_t>(length);
        إذا (length>bmp_اوږدتیا) {
            *dest|=0x8000;
            *++dest = static_cast<uint16_t>(bmp_اوږدتیا);
        }
        ++dest;

        /* write the BMP part of the array */
        p=هذا->list;
        لكل (i=0; i<bmp_اوږدتیا; ++i) {
#که_تعریف_شوی DEBUG_SERIALIZE
          چاپ_کړه("writebmp: %x\n", (صحيح)*p);
#د_که_پای
            *dest++ = static_cast<uint16_t>(*p++);
        }

        /* write the supplementary part of the array */
        لكل (; i<length; i+=2) {
#که_تعریف_شوی DEBUG_SERIALIZE
          چاپ_کړه("write32: %x\n", (صحيح)*p);
#د_که_پای
            *dest++ = static_cast<uint16_t>(*p >> 16);
            *dest++ = static_cast<uint16_t>(*p++);
        }
    } وإلا {
        ec=U_بفر_ډکېدنه_ERROR;
    }
    أرجع dest_اوږدتیا;
}

//----------------------------------------------------------------
// Implementation: Utility methods
//----------------------------------------------------------------

/**
 * Allocate our strings vector and return true if successful.
 */
UBool Unicode_سېټ::allocate_مزي_ګانې(UError_کوډ &حالت) {
    إذا (U_FAILURE(حالت)) {
        أرجع خطأ;
    }
    مزي_ګانې_2 = جديد UVector(uprv_deleteUObject,
                          uhash_compareUnicodeString, 1, حالت);
    إذا (مزي_ګانې_2 == تش_اشاره) { // Check for memory allocation error.
        حالت = U_حافظه_ویش_ERROR;
        أرجع خطأ;
    }
    إذا (U_FAILURE(حالت)) {
        ړنګول مزي_ګانې_2;
        مزي_ګانې_2 = تش_اشاره;
        أرجع خطأ;
    } 
    أرجع ریښتیا;
}

int32_t Unicode_سېټ::بل_ظرفیت(int32_t min_ظرفیت) {
    // Grow exponentially to reduce the frequency of allocations.
    إذا (min_ظرفیت < INITIAL_ظرفیت) {
        أرجع min_ظرفیت + INITIAL_ظرفیت;
    } وإلا إذا (min_ظرفیت <= 2500) {
        أرجع 5 * min_ظرفیت;
    } وإلا {
        int32_t new_ظرفیت = 2 * min_ظرفیت;
        إذا (new_ظرفیت > MAX_اوږدتیا) {
            new_ظرفیت = MAX_اوږدتیا;
        }
        أرجع new_ظرفیت;
    }
}

منطقي Unicode_سېټ::ensure_ظرفیت(int32_t newLen) {
    إذا (newLen > MAX_اوږدتیا) {
        newLen = MAX_اوږدتیا;
    }
    إذا (newLen <= capacity) {
        أرجع ریښتیا;
    }
    int32_t new_ظرفیت = بل_ظرفیت(newLen);
    UChar32* لنډمهاله = static_cast<UChar32*>(uprv_malloc(new_ظرفیت * حجم(UChar32)));
    إذا (لنډمهاله == تش_اشاره) {
        سېټ_منزل_Bogus(); // set the object to bogus state if an OOM failure occurred.
        أرجع خطأ;
    }
    // Copy only the actual contents.
    uprv_memcpy(لنډمهاله, list, len * حجم(UChar32));
    إذا (list != ډېری_List) {
        uprv_free(list);
    }
    list = لنډمهاله;
    capacity = new_ظرفیت;
    أرجع ریښتیا;
}

منطقي Unicode_سېټ::ensure_بفر_ظرفیت(int32_t newLen) {
    إذا (newLen > MAX_اوږدتیا) {
        newLen = MAX_اوږدتیا;
    }
    إذا (newLen <= بفر_ظرفیت) {
        أرجع ریښتیا;
    }
    int32_t new_ظرفیت = بل_ظرفیت(newLen);
    UChar32* لنډمهاله = static_cast<UChar32*>(uprv_malloc(new_ظرفیت * حجم(UChar32)));
    إذا (لنډمهاله == تش_اشاره) {
        سېټ_منزل_Bogus();
        أرجع خطأ;
    }
    // The buffer has no contents to be copied.
    // It is always filled from scratch after this call.
    إذا (بفر != ډېری_List) {
        uprv_free(بفر);
    }
    بفر = لنډمهاله;
    بفر_ظرفیت = new_ظرفیت;
    أرجع ریښتیا;
}

/**
 * Swap list and buffer.
 */
فراغ Unicode_سېټ::بدلون_Buffers() {
    // swap list and buffer
    UChar32* لنډمهاله = list;
    list = بفر;
    بفر = لنډمهاله;

    int32_t c = capacity;
    capacity = بفر_ظرفیت;
    بفر_ظرفیت = c;
}

فراغ Unicode_سېټ::سېټ_منزل_Bogus() {
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

فراغ Unicode_سېټ::exclusiveOr(ثابت UChar32* other, int32_t otherLen, int8_t polarity) {
    إذا (isFrozen() || isBogus()) {
        أرجع;
    }
    إذا (!ensure_بفر_ظرفیت(len + otherLen)) {
        أرجع;
    }

    int32_t i = 0, j = 0, k = 0;
    UChar32 a = list[i++];
    UChar32 b;
    إذا (polarity == 1 || polarity == 2) {
        b = UNICODESET_ټیټ;
        إذا (other[j] == UNICODESET_ټیټ) { // skip base if already LOW
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
        } وإلا إذا (a != UNICODESET_لوړ) { // at this point, a == b
            // discard both values!
            a = list[i++];
            b = other[j++];
        } وإلا { // DONE!
            بفر[k++] = UNICODESET_لوړ;
            len = k;
            اكسر;
        }
    }
    بدلون_Buffers();
    release_نمونه();
}

// polarity = 0 is normal: x union y
// polarity = 2: x union ~y
// polarity = 1: ~x union y
// polarity = 3: ~x union ~y

فراغ Unicode_سېټ::جمع_کړه(ثابت UChar32* other, int32_t otherLen, int8_t polarity) {
    إذا (isFrozen() || isBogus() || other==تش_اشاره) {
        أرجع;
    }
    إذا (!ensure_بفر_ظرفیت(len + otherLen)) {
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
                إذا (a == UNICODESET_لوړ) اذهب loop_پای_ټکی;
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
                إذا (a == UNICODESET_لوړ) اذهب loop_پای_ټکی;
                بفر[k++] = a;
            } وإلا { // take b
                إذا (b == UNICODESET_لوړ) اذهب loop_پای_ټکی;
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
                إذا (a == UNICODESET_لوړ) اذهب loop_پای_ټکی;
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
                إذا (a == UNICODESET_لوړ) اذهب loop_پای_ټکی;
                a = list[i++];
                polarity ^= 1;
                b = other[j++];
                polarity ^= 2;
            }
            اكسر;
        }
    }
 loop_پای_ټکی:
    بفر[k++] = UNICODESET_لوړ;    // terminate
    len = k;
    بدلون_Buffers();
    release_نمونه();
}

// polarity = 0 is normal: x intersect y
// polarity = 2: x intersect ~y == set-minus
// polarity = 1: ~x intersect y
// polarity = 3: ~x intersect ~y

فراغ Unicode_سېټ::retain(ثابت UChar32* other, int32_t otherLen, int8_t polarity) {
    إذا (isFrozen() || isBogus()) {
        أرجع;
    }
    إذا (!ensure_بفر_ظرفیت(len + otherLen)) {
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
                إذا (a == UNICODESET_لوړ) اذهب loop_پای_ټکی;
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
                إذا (a == UNICODESET_لوړ) اذهب loop_پای_ټکی;
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
                إذا (a == UNICODESET_لوړ) اذهب loop_پای_ټکی;
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
                إذا (a == UNICODESET_لوړ) اذهب loop_پای_ټکی;
                a = list[i++];
                polarity ^= 1;
                b = other[j++];
                polarity ^= 2;
            }
            اكسر;
        }
    }
 loop_پای_ټکی:
    بفر[k++] = UNICODESET_لوړ;    // terminate
    len = k;
    بدلون_Buffers();
    release_نمونه();
}

/**
 * Append the <code>toPattern()</code> representation of a
 * string to the given <code>StringBuffer</code>.
 */
فراغ Unicode_سېټ::append_منزل_Pat(UnicodeString& buf, ثابت UnicodeString& s, UBool escapeUnprintable) {
    UChar32 cp;
    لكل (int32_t i = 0; i < s.length(); i += U16_اوږدتیا(cp)) {
        append_منزل_Pat(buf, cp = s.char32At(i), escapeUnprintable);
    }
}

/**
 * Append the <code>toPattern()</code> representation of a
 * character to the given <code>StringBuffer</code>.
 */
فراغ Unicode_سېټ::append_منزل_Pat(UnicodeString& buf, UChar32 c, UBool escapeUnprintable) {
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
    حالة نښه_جدول::نښه_REF:
        buf.append(u'\\');
        اكسر;
    افتراضي:
        // Escape whitespace
        إذا (نمونه_Props::isWhiteSpace(c)) {
            buf.append(u'\\');
        }
        اكسر;
    }
    buf.append(c);
}

فراغ Unicode_سېټ::append_منزل_Pat(UnicodeString &پایله, UChar32 پیل, UChar32 پای_ټکی,
                              UBool escapeUnprintable) {
    append_منزل_Pat(پایله, پیل, escapeUnprintable);
    إذا (پیل != پای_ټکی) {
        إذا ((پیل+1) != پای_ټکی ||
                // Avoid writing what looks like a lead+trail surrogate pair.
                پیل == 0xdbff) {
            پایله.append(u'-');
        }
        append_منزل_Pat(پایله, پای_ټکی, escapeUnprintable);
    }
}

/**
 * Append a string representation of this set to result.  This will be
 * a cleaned version of the string passed to applyPattern(), if there
 * is one.  Otherwise it will be generated.
 */
UnicodeString& Unicode_سېټ::منزل_نمونه_2(UnicodeString& پایله U_LIFETIME_BOUND,
                                      UBool escapeUnprintable) ثابت
{
    إذا (pat != تش_اشاره) {
        int32_t i;
        int32_t backslash_شمېر = 0;
        لكل (i=0; i<patLen; ) {
            UChar32 c;
            U16_بل(pat, i, patLen, c);
            إذا (escapeUnprintable ?
                    ICU_Utility::isUnprintable(c) : ICU_Utility::shouldAlwaysBeEscaped(c)) {
                // If the unprintable character is preceded by an odd
                // number of backslashes, then it has been escaped.
                // Before unescaping it, we delete the final
                // backslash.
                إذا ((backslash_شمېر % 2) == 1) {
                    پایله.truncate(پایله.length() - 1);
                }
                ICU_Utility::escape(پایله, c);
                backslash_شمېر = 0;
            } وإلا {
                پایله.append(c);
                إذا (c == u'\\') {
                    ++backslash_شمېر;
                } وإلا {
                    backslash_شمېر = 0;
                }
            }
        }
        أرجع پایله;
    }

    أرجع generate_نمونه(پایله, escapeUnprintable);
}

/**
 * Returns a string representation of this set.  If the result of
 * calling this function is passed to a UnicodeSet constructor, it
 * will produce another set that is equal to this one.
 */
UnicodeString& Unicode_سېټ::منزل_نمونه(UnicodeString& پایله U_LIFETIME_BOUND,
                                     UBool escapeUnprintable) ثابت
{
    پایله.truncate(0);
    أرجع منزل_نمونه_2(پایله, escapeUnprintable);
}

/**
 * Generate and append a string representation of this set to result.
 * This does not use this.pat, the cleaned up copy of the string
 * passed to applyPattern().
 */
UnicodeString& Unicode_سېټ::generate_نمونه(UnicodeString& پایله U_LIFETIME_BOUND,
                                            UBool escapeUnprintable) ثابت
{
    پایله.append(u'[');

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
    إذا (len >= 4 && list[0] == 0 && حد == len && !has_مزي_ګانې()) {
        // Emit the inverse
        پایله.append(u'^');
        // Offsetting the inversion list index by one lets us
        // iterate over the ranges of the set complement.
        i = 1;
        --حد;
    }

    // Emit the ranges as pairs.
    طالما (i < حد) {
        UChar32 پیل = list[i];  // getRangeStart()
        UChar32 پای_ټکی = list[i + 1] - 1;  // getRangeEnd() = range limit minus one
        إذا (!(0xd800 <= پای_ټکی && پای_ټکی <= 0xdbff)) {
            append_منزل_Pat(پایله, پیل, پای_ټکی, escapeUnprintable);
            i += 2;
        } وإلا {
            // The range ends with a lead surrogate.
            // Avoid writing what looks like a lead+trail surrogate pair.
            // 1. Postpone ranges that start with a lead surrogate code point.
            int32_t لومړی_Lead = i;
            طالما ((i += 2) < حد && list[i] <= 0xdbff) {}
            int32_t لومړی_After_Lead = i;
            // 2. Write following ranges that start with a trail surrogate code point.
            طالما (i < حد && (پیل = list[i]) <= 0xdfff) {
                append_منزل_Pat(پایله, پیل, list[i + 1] - 1, escapeUnprintable);
                i += 2;
            }
            // 3. Now write the postponed ranges.
            لكل (صحيح j = لومړی_Lead; j < لومړی_After_Lead; j += 2) {
                append_منزل_Pat(پایله, list[j], list[j + 1] - 1, escapeUnprintable);
            }
        }
    }

    إذا (مزي_ګانې_2 != تش_اشاره) {
        لكل (int32_t i = 0; i<مزي_ګانې_2->size(); ++i) {
            پایله.append(u'{');
            append_منزل_Pat(پایله,
                         *static_cast<ثابت UnicodeString*>(مزي_ګانې_2->elementAt(i)),
                         escapeUnprintable);
            پایله.append(u'}');
        }
    }
    أرجع پایله.append(u']');
}

/**
* Release existing cached pattern
*/
فراغ Unicode_سېټ::release_نمونه() {
    إذا (pat) {
        uprv_free(pat);
        pat = تش_اشاره;
        patLen = 0;
    }
}

/**
* Set the new pattern to cache.
*/
فراغ Unicode_سېټ::سېټ_نمونه(ثابت char16_t *newPat, int32_t newPatLen) {
    release_نمونه();
    pat = static_cast<char16_t*>(uprv_malloc((newPatLen + 1) * حجم(char16_t)));
    إذا (pat) {
        patLen = newPatLen;
        u_memcpy(pat, newPat, patLen);
        pat[patLen] = 0;
    }
    // else we don't care if malloc failed. This was just a nice cache.
    // We can regenerate an equivalent pattern later when requested.
}

Unicode_سېټ* Unicode_سېټ::freeze() U_LIFETIME_BOUND {
    إذا(!isFrozen() && !isBogus()) {
        compact();

        // Optimize contains() and span() and similar functions.
        إذا (has_مزي_ګانې()) {
            string_پراختیا = جديد Unicode_سېټ_String_پراختیا(*هذا, *مزي_ګانې_2, Unicode_سېټ_String_پراختیا::ALL);
            إذا (string_پراختیا == تش_اشاره) {
                سېټ_منزل_Bogus();
                أرجع هذا;
            } وإلا إذا (!string_پراختیا->needs_String_پراختیا_UTF16()) {
                // All strings are irrelevant for span() etc. because
                // all of each string's code points are contained in this set.
                // Do not check needsStringSpanUTF8() because UTF-8 has at most as
                // many relevant strings as UTF-16.
                // (Thus needsStringSpanUTF8() implies needsStringSpanUTF16().)
                ړنګول string_پراختیا;
                string_پراختیا = تش_اشاره;
            }
        }
        إذا (string_پراختیا == تش_اشاره) {
            // No span-relevant strings: Optimize for code point spans.
            bmp_سېټ=جديد BMPSet(list, len);
            إذا (bmp_سېټ == تش_اشاره) { // Check for memory allocation error.
                سېټ_منزل_Bogus();
            }
        }
    }
    أرجع هذا;
}

int32_t Unicode_سېټ::پراختیا(ثابت char16_t *s, int32_t length, USet_پراختیا_Condition پراختیا_Condition) ثابت {
    إذا(length>0 && bmp_سېټ!=تش_اشاره) {
        أرجع static_cast<int32_t>(bmp_سېټ->پراختیا(s, s + length, پراختیا_Condition) - s);
    }
    إذا(length<0) {
        length=u_strlen(s);
    }
    إذا(length==0) {
        أرجع 0;
    }
    إذا(string_پراختیا!=تش_اشاره) {
        أرجع string_پراختیا->پراختیا(s, length, پراختیا_Condition);
    } وإلا إذا(has_مزي_ګانې()) {
        uint32_t which= پراختیا_Condition==USET_پراختیا_NOT_CONTAINED ?
                            Unicode_سېټ_String_پراختیا::FWD_UTF16_NOT_CONTAINED :
                            Unicode_سېټ_String_پراختیا::FWD_UTF16_CONTAINED;
        Unicode_سېټ_String_پراختیا str_پراختیا(*هذا, *مزي_ګانې_2, which);
        إذا(str_پراختیا.needs_String_پراختیا_UTF16()) {
            أرجع str_پراختیا.پراختیا(s, length, پراختیا_Condition);
        }
    }

    إذا(پراختیا_Condition!=USET_پراختیا_NOT_CONTAINED) {
        پراختیا_Condition=USET_پراختیا_CONTAINED;  // Pin to 0/1 values.
    }

    UChar32 c;
    int32_t پیل=0, prev=0;
    افعل {
        U16_بل(s, پیل, length, c);
        إذا(پراختیا_Condition!=لري(c)) {
            اكسر;
        }
    } طالما((prev=پیل)<length);
    أرجع prev;
}

int32_t Unicode_سېټ::پراختیا_شا(ثابت char16_t *s, int32_t length, USet_پراختیا_Condition پراختیا_Condition) ثابت {
    إذا(length>0 && bmp_سېټ!=تش_اشاره) {
        أرجع static_cast<int32_t>(bmp_سېټ->پراختیا_شا(s, s + length, پراختیا_Condition) - s);
    }
    إذا(length<0) {
        length=u_strlen(s);
    }
    إذا(length==0) {
        أرجع 0;
    }
    إذا(string_پراختیا!=تش_اشاره) {
        أرجع string_پراختیا->پراختیا_شا(s, length, پراختیا_Condition);
    } وإلا إذا(has_مزي_ګانې()) {
        uint32_t which= پراختیا_Condition==USET_پراختیا_NOT_CONTAINED ?
                            Unicode_سېټ_String_پراختیا::شا_UTF16_NOT_CONTAINED :
                            Unicode_سېټ_String_پراختیا::شا_UTF16_CONTAINED;
        Unicode_سېټ_String_پراختیا str_پراختیا(*هذا, *مزي_ګانې_2, which);
        إذا(str_پراختیا.needs_String_پراختیا_UTF16()) {
            أرجع str_پراختیا.پراختیا_شا(s, length, پراختیا_Condition);
        }
    }

    إذا(پراختیا_Condition!=USET_پراختیا_NOT_CONTAINED) {
        پراختیا_Condition=USET_پراختیا_CONTAINED;  // Pin to 0/1 values.
    }

    UChar32 c;
    int32_t prev=length;
    افعل {
        U16_PREV(s, 0, length, c);
        إذا(پراختیا_Condition!=لري(c)) {
            اكسر;
        }
    } طالما((prev=length)>0);
    أرجع prev;
}

int32_t Unicode_سېټ::پراختیا_UTF8(ثابت حرف *s, int32_t length, USet_پراختیا_Condition پراختیا_Condition) ثابت {
    إذا(length>0 && bmp_سېټ!=تش_اشاره) {
        ثابت uint8_t* s0 = reinterpret_cast<ثابت uint8_t*>(s);
        أرجع static_cast<int32_t>(bmp_سېټ->پراختیا_UTF8(s0, length, پراختیا_Condition) - s0);
    }
    إذا(length<0) {
        length = static_cast<int32_t>(uprv_strlen(s));
    }
    إذا(length==0) {
        أرجع 0;
    }
    إذا(string_پراختیا!=تش_اشاره) {
        أرجع string_پراختیا->پراختیا_UTF8(reinterpret_cast<ثابت uint8_t*>(s), length, پراختیا_Condition);
    } وإلا إذا(has_مزي_ګانې()) {
        uint32_t which= پراختیا_Condition==USET_پراختیا_NOT_CONTAINED ?
                            Unicode_سېټ_String_پراختیا::FWD_UTF8_NOT_CONTAINED :
                            Unicode_سېټ_String_پراختیا::FWD_UTF8_CONTAINED;
        Unicode_سېټ_String_پراختیا str_پراختیا(*هذا, *مزي_ګانې_2, which);
        إذا(str_پراختیا.needs_String_پراختیا_UTF8()) {
            أرجع str_پراختیا.پراختیا_UTF8(reinterpret_cast<ثابت uint8_t*>(s), length, پراختیا_Condition);
        }
    }

    إذا(پراختیا_Condition!=USET_پراختیا_NOT_CONTAINED) {
        پراختیا_Condition=USET_پراختیا_CONTAINED;  // Pin to 0/1 values.
    }

    UChar32 c;
    int32_t پیل=0, prev=0;
    افعل {
        U8_بل_OR_FFFD(s, پیل, length, c);
        إذا(پراختیا_Condition!=لري(c)) {
            اكسر;
        }
    } طالما((prev=پیل)<length);
    أرجع prev;
}

int32_t Unicode_سېټ::پراختیا_شا_UTF8(ثابت حرف *s, int32_t length, USet_پراختیا_Condition پراختیا_Condition) ثابت {
    إذا(length>0 && bmp_سېټ!=تش_اشاره) {
        ثابت uint8_t* s0 = reinterpret_cast<ثابت uint8_t*>(s);
        أرجع bmp_سېټ->پراختیا_شا_UTF8(s0, length, پراختیا_Condition);
    }
    إذا(length<0) {
        length = static_cast<int32_t>(uprv_strlen(s));
    }
    إذا(length==0) {
        أرجع 0;
    }
    إذا(string_پراختیا!=تش_اشاره) {
        أرجع string_پراختیا->پراختیا_شا_UTF8(reinterpret_cast<ثابت uint8_t*>(s), length, پراختیا_Condition);
    } وإلا إذا(has_مزي_ګانې()) {
        uint32_t which= پراختیا_Condition==USET_پراختیا_NOT_CONTAINED ?
                            Unicode_سېټ_String_پراختیا::شا_UTF8_NOT_CONTAINED :
                            Unicode_سېټ_String_پراختیا::شا_UTF8_CONTAINED;
        Unicode_سېټ_String_پراختیا str_پراختیا(*هذا, *مزي_ګانې_2, which);
        إذا(str_پراختیا.needs_String_پراختیا_UTF8()) {
            أرجع str_پراختیا.پراختیا_شا_UTF8(reinterpret_cast<ثابت uint8_t*>(s), length, پراختیا_Condition);
        }
    }

    إذا(پراختیا_Condition!=USET_پراختیا_NOT_CONTAINED) {
        پراختیا_Condition=USET_پراختیا_CONTAINED;  // Pin to 0/1 values.
    }

    UChar32 c;
    int32_t prev=length;
    افعل {
        U8_PREV_OR_FFFD(s, 0, length, c);
        إذا(پراختیا_Condition!=لري(c)) {
            اكسر;
        }
    } طالما((prev=length)>0);
    أرجع prev;
}

U_NAMESPACE_پای_ټکی
