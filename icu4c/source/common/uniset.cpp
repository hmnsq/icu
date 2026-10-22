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

#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/utypes.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/parsepos.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/symtable.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/uniset.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/ustring.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/utf8.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/utf16.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "ruleiter.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "cmemory.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "cstring.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "patternprops.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "uelement.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "uvector.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "charstr.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "ustrfmt.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "uassert.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "bmpset.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unisetspan.h"

// HIGH_VALUE > all valid values. 110000 for codepoints
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ UNICODESET_ਉੱਚਾ 0x0110000

// LOW <= all valid values. ZERO for codepoints
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ UNICODESET_ਨੀਵਾਂ 0x000000

/** Max list [0, 1, 2, ..., max code point, HIGH] */
constexpr int32_t MAX_ਲੰਮਾਈ = UNICODESET_ਉੱਚਾ + 1;

U_NAMESPACE_BEGIN

ਚਿੰਨ੍ਹ_ਸਾਰਣੀ::~ਚਿੰਨ੍ਹ_ਸਾਰਣੀ() {}

UOBJECT_DEFINE_RTTI_IMPLEMENTATION(Unicode_ਸੈੱਟ)

/**
 * Modify the given UChar32 variable so that it is in range, by
 * pinning values < UNICODESET_LOW to UNICODESET_LOW, and
 * pinning values > UNICODESET_HIGH-1 to UNICODESET_HIGH-1.
 * It modifies its argument in-place and also returns it.
 */
ਸਥਿਰ ਸਤਰਅੰਦਰ UChar32 pin_ਕੋਡ_ਬਿੰਦੂ(UChar32& c) {
    ਜੇ (c < UNICODESET_ਨੀਵਾਂ) {
        c = UNICODESET_ਨੀਵਾਂ;
    } ਨਹੀਂਤਾਂ ਜੇ (c > (UNICODESET_ਉੱਚਾ-1)) {
        c = (UNICODESET_ਉੱਚਾ-1);
    }
    ਵਾਪਸ c;
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

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ DEBUG_MEM
#ਅਨ੍ਤਰ੍ਭਾਵ <stdio.h>
ਸਥਿਰ int32_t dbg_ਗਿਣਤੀ = 0;

ਸਥਿਰ ਸਤਰਅੰਦਰ ਖਾਲੀ _dbgct(Unicode_ਸੈੱਟ* ਸੈੱਟ) {
    UnicodeString str;
    ਸੈੱਟ->ਮੰਜ਼ਿਲ_ਨਮੂਨਾ(str, ਸੱਚ);
    ਅੱਖਰ buf[40];
    str.extract(0, 39, buf, "");
    ਲਿਖੋ("DEBUG UnicodeSet: ct 0x%08X; %d %s\n", ਸੈੱਟ, ++dbg_ਗਿਣਤੀ, buf);
}

ਸਥਿਰ ਸਤਰਅੰਦਰ ਖਾਲੀ _dbgdt(Unicode_ਸੈੱਟ* ਸੈੱਟ) {
    UnicodeString str;
    ਸੈੱਟ->ਮੰਜ਼ਿਲ_ਨਮੂਨਾ(str, ਸੱਚ);
    ਅੱਖਰ buf[40];
    str.extract(0, 39, buf, "");
    ਲਿਖੋ("DEBUG UnicodeSet: dt 0x%08X; %d %s\n", ਸੈੱਟ, --dbg_ਗਿਣਤੀ, buf);
}

#ਨਹੀਂਤਾਂ

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ _dbgct(ਸੈੱਟ)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ _dbgdt(ਸੈੱਟ)

#ਜੇ_ਅੰਤ

//----------------------------------------------------------------
// UnicodeString in UVector support
//----------------------------------------------------------------

ਸਥਿਰ ਖਾਲੀ U_CALLCONV cloneUnicodeString(UElement *dst, UElement *src) {
    dst->ਸੂਚਕ = ਨਵਾਂ UnicodeString(*static_cast<UnicodeString*>(src->ਸੂਚਕ));
}

ਸਥਿਰ int32_t U_CALLCONV compareUnicodeString(UElement t1, UElement t2) {
    ਅਚਲ UnicodeString& a = *static_cast<ਅਚਲ UnicodeString*>(t1.ਸੂਚਕ);
    ਅਚਲ UnicodeString& b = *static_cast<ਅਚਲ UnicodeString*>(t2.ਸੂਚਕ);
    ਵਾਪਸ a.compare(b);
}

UBool Unicode_ਸੈੱਟ::has_ਲੜੀਆਂ() ਅਚਲ {
    ਵਾਪਸ ਲੜੀਆਂ_2 != ਖਾਲੀ_ਸੂਚਕ && !ਲੜੀਆਂ_2->is_ਖਾਲੀ_ਥਾਵਾਂ();
}

int32_t Unicode_ਸੈੱਟ::ਲੜੀਆਂ_ਮਾਪ() ਅਚਲ {
    ਵਾਪਸ ਲੜੀਆਂ_2 == ਖਾਲੀ_ਸੂਚਕ ? 0 : ਲੜੀਆਂ_2->size();
}

UBool Unicode_ਸੈੱਟ::ਲੜੀਆਂ_ਸ਼ਾਮਲ_ਹੈ(ਅਚਲ UnicodeString &s) ਅਚਲ {
    ਵਾਪਸ ਲੜੀਆਂ_2 != ਖਾਲੀ_ਸੂਚਕ && ਲੜੀਆਂ_2->ਸ਼ਾਮਲ_ਹੈ((ਖਾਲੀ*) &s);
}

//----------------------------------------------------------------
// Constructors &c
//----------------------------------------------------------------

/**
 * Constructs an empty set.
 */
Unicode_ਸੈੱਟ::Unicode_ਸੈੱਟ() {
    list[0] = UNICODESET_ਉੱਚਾ;
    _dbgct(ਇਹ);
}

/**
 * Constructs a set containing the given range. If <code>end >
 * start</code> then an empty set is created.
 *
 * @param start first character, inclusive, of range
 * @param end last character, inclusive, of range
 */
Unicode_ਸੈੱਟ::Unicode_ਸੈੱਟ(UChar32 ਸ਼ੁਰੂ, UChar32 ਸਿਰਾ) {
    list[0] = UNICODESET_ਉੱਚਾ;
    ਜੋੜੋ(ਸ਼ੁਰੂ, ਸਿਰਾ);
    _dbgct(ਇਹ);
}

/**
 * Constructs a set that is identical to the given UnicodeSet.
 */
Unicode_ਸੈੱਟ::Unicode_ਸੈੱਟ(ਅਚਲ Unicode_ਸੈੱਟ& o) : UnicodeFilter(o) {
    *ਇਹ = o;
    _dbgct(ਇਹ);
}

// Copy-construct as thawed.
Unicode_ਸੈੱਟ::Unicode_ਸੈੱਟ(ਅਚਲ Unicode_ਸੈੱਟ& o, UBool /* asThawed */) : UnicodeFilter(o) {
    ਜੇ (ensure_ਸਮਰੱਥਾ(o.len)) {
        // *this = o except for bmpSet and stringSpan
        len = o.len;
        uprv_memcpy(list, o.list, (size_t)len*ਆਕਾਰ(UChar32));
        ਜੇ (o.has_ਲੜੀਆਂ()) {
            UError_ਕੋਡ ਸਥਿਤੀ = U_ZERO_ERROR;
            ਜੇ (!allocate_ਲੜੀਆਂ(ਸਥਿਤੀ) ||
                    (ਲੜੀਆਂ_2->assign(*o.ਲੜੀਆਂ_2, cloneUnicodeString, ਸਥਿਤੀ), U_FAILURE(ਸਥਿਤੀ))) {
                ਸੈੱਟ_ਮੰਜ਼ਿਲ_Bogus();
                ਵਾਪਸ;
            }
        }
        ਜੇ (o.pat) {
            ਸੈੱਟ_ਨਮੂਨਾ(o.pat, o.patLen);
        }
        _dbgct(ਇਹ);
    }
}

/**
 * Destructs the set.
 */
Unicode_ਸੈੱਟ::~Unicode_ਸੈੱਟ() {
    _dbgdt(ਇਹ); // first!
    ਜੇ (list != ਢੇਰ_List) {
        uprv_free(list);
    }
    ਮਿਟਾਓ bmp_ਸੈੱਟ;
    ਜੇ (ਬਫਰ != ਢੇਰ_List) {
        uprv_free(ਬਫਰ);
    }
    ਮਿਟਾਓ ਲੜੀਆਂ_2;
    ਮਿਟਾਓ string_ਵਿਸਤਾਰ;
    release_ਨਮੂਨਾ();
}

/**
 * Assigns this object to be a copy of another.
 */
Unicode_ਸੈੱਟ& Unicode_ਸੈੱਟ::ਸੰਚਾਲਕ=(ਅਚਲ Unicode_ਸੈੱਟ& o) U_LIFETIME_BOUND {
    ਵਾਪਸ copyFrom(o, ਝੂਠ);
}

Unicode_ਸੈੱਟ& Unicode_ਸੈੱਟ::copyFrom(ਅਚਲ Unicode_ਸੈੱਟ& o, UBool asThawed) U_LIFETIME_BOUND {
    ਜੇ (ਇਹ == &o) {
        ਵਾਪਸ *ਇਹ;
    }
    ਜੇ (isFrozen()) {
        ਵਾਪਸ *ਇਹ;
    }
    ਜੇ (o.isBogus()) {
        ਸੈੱਟ_ਮੰਜ਼ਿਲ_Bogus();
        ਵਾਪਸ *ਇਹ;
    }
    ਜੇ (!ensure_ਸਮਰੱਥਾ(o.len)) {
        // ensureCapacity will mark the UnicodeSet as Bogus if OOM failure happens.
        ਵਾਪਸ *ਇਹ;
    }
    len = o.len;
    uprv_memcpy(list, o.list, (size_t)len*ਆਕਾਰ(UChar32));
    ਜੇ (o.bmp_ਸੈੱਟ != ਖਾਲੀ_ਸੂਚਕ && !asThawed) {
        bmp_ਸੈੱਟ = ਨਵਾਂ BMPSet(*o.bmp_ਸੈੱਟ, list, len);
        ਜੇ (bmp_ਸੈੱਟ == ਖਾਲੀ_ਸੂਚਕ) { // Check for memory allocation error.
            ਸੈੱਟ_ਮੰਜ਼ਿਲ_Bogus();
            ਵਾਪਸ *ਇਹ;
        }
    }
    ਜੇ (o.has_ਲੜੀਆਂ()) {
        UError_ਕੋਡ ਸਥਿਤੀ = U_ZERO_ERROR;
        ਜੇ ((ਲੜੀਆਂ_2 == ਖਾਲੀ_ਸੂਚਕ && !allocate_ਲੜੀਆਂ(ਸਥਿਤੀ)) ||
                (ਲੜੀਆਂ_2->assign(*o.ਲੜੀਆਂ_2, cloneUnicodeString, ਸਥਿਤੀ), U_FAILURE(ਸਥਿਤੀ))) {
            ਸੈੱਟ_ਮੰਜ਼ਿਲ_Bogus();
            ਵਾਪਸ *ਇਹ;
        }
    } ਨਹੀਂਤਾਂ ਜੇ (has_ਲੜੀਆਂ()) {
        ਲੜੀਆਂ_2->removeAllElements();
    }
    ਜੇ (o.string_ਵਿਸਤਾਰ != ਖਾਲੀ_ਸੂਚਕ && ਲੜੀਆਂ_2 != ਖਾਲੀ_ਸੂਚਕ && !asThawed) {
        string_ਵਿਸਤਾਰ = ਨਵਾਂ Unicode_ਸੈੱਟ_String_ਵਿਸਤਾਰ(*o.string_ਵਿਸਤਾਰ, *ਲੜੀਆਂ_2);
        ਜੇ (string_ਵਿਸਤਾਰ == ਖਾਲੀ_ਸੂਚਕ) { // Check for memory allocation error.
            ਸੈੱਟ_ਮੰਜ਼ਿਲ_Bogus();
            ਵਾਪਸ *ਇਹ;
        }
    }
    release_ਨਮੂਨਾ();
    ਜੇ (o.pat) {
        ਸੈੱਟ_ਨਮੂਨਾ(o.pat, o.patLen);
    }
    ਵਾਪਸ *ਇਹ;
}

/**
 * Returns a copy of this object.  All UnicodeMatcher objects have
 * to support cloning in order to allow classes using
 * UnicodeMatchers, such as Transliterator, to implement cloning.
 */
Unicode_ਸੈੱਟ* Unicode_ਸੈੱਟ::clone() ਅਚਲ {
    ਵਾਪਸ ਨਵਾਂ Unicode_ਸੈੱਟ(*ਇਹ);
}

Unicode_ਸੈੱਟ *Unicode_ਸੈੱਟ::cloneAsThawed() ਅਚਲ {
    ਵਾਪਸ ਨਵਾਂ Unicode_ਸੈੱਟ(*ਇਹ, ਸੱਚ);
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
ਤਾਰਕਿਕ Unicode_ਸੈੱਟ::ਸੰਚਾਲਕ==(ਅਚਲ Unicode_ਸੈੱਟ& o) ਅਚਲ {
    ਜੇ (len != o.len) ਵਾਪਸ ਝੂਠ;
    ਲਈ (int32_t i = 0; i < len; ++i) {
        ਜੇ (list[i] != o.list[i]) ਵਾਪਸ ਝੂਠ;
    }
    ਜੇ (has_ਲੜੀਆਂ() != o.has_ਲੜੀਆਂ()) { ਵਾਪਸ ਝੂਠ; }
    ਜੇ (has_ਲੜੀਆਂ() && *ਲੜੀਆਂ_2 != *o.ਲੜੀਆਂ_2) ਵਾਪਸ ਝੂਠ;
    ਵਾਪਸ ਸੱਚ;
}

/**
 * Returns the hash code value for this set.
 *
 * @return the hash code value for this set.
 * @see Object#hashCode()
 */
int32_t Unicode_ਸੈੱਟ::hash_ਕੋਡ() ਅਚਲ {
    uint32_t ਨਤੀਜਾ = static_cast<uint32_t>(len);
    ਲਈ (int32_t i = 0; i < len; ++i) {
        ਨਤੀਜਾ *= 1000003u;
        ਨਤੀਜਾ += list[i];
    }
    ਵਾਪਸ static_cast<int32_t>(ਨਤੀਜਾ);
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
int32_t Unicode_ਸੈੱਟ::size() ਅਚਲ {
    int32_t n = 0;
    int32_t count = get_Range_ਗਿਣਤੀ();
    ਲਈ (int32_t i = 0; i < count; ++i) {
        n += get_Range_ਸਿਰਾ(i) - get_Range_ਸ਼ੁਰੂ(i) + 1;
    }
    ਵਾਪਸ n + ਲੜੀਆਂ_ਮਾਪ();
}

/**
 * Returns <tt>true</tt> if this set contains no elements.
 *
 * @return <tt>true</tt> if this set contains no elements.
 */
UBool Unicode_ਸੈੱਟ::is_ਖਾਲੀ_ਥਾਵਾਂ() ਅਚਲ {
    ਵਾਪਸ len == 1 && !has_ਲੜੀਆਂ();
}

/**
 * Returns true if this set contains the given character.
 * @param c character to be checked for containment
 * @return true if the test condition is met
 */
UBool Unicode_ਸੈੱਟ::ਸ਼ਾਮਲ_ਹੈ(UChar32 c) ਅਚਲ {
    // Set i to the index of the start item greater than ch
    // We know we will terminate without length test!
    // LATER: for large sets, add binary search
    //int32_t i = -1;
    //for (;;) {
    //    if (c < list[++i]) break;
    //}
    ਜੇ (bmp_ਸੈੱਟ != ਖਾਲੀ_ਸੂਚਕ) {
        ਵਾਪਸ bmp_ਸੈੱਟ->ਸ਼ਾਮਲ_ਹੈ(c);
    }
    ਜੇ (string_ਵਿਸਤਾਰ != ਖਾਲੀ_ਸੂਚਕ) {
        ਵਾਪਸ string_ਵਿਸਤਾਰ->ਸ਼ਾਮਲ_ਹੈ(c);
    }
    ਜੇ (c >= UNICODESET_ਉੱਚਾ) { // Don't need to check LOW bound
        ਵਾਪਸ ਝੂਠ;
    }
    int32_t i = ਲੱਭੋ_ਕੋਡ_ਬਿੰਦੂ(c);
    ਵਾਪਸ i & 1; // return true if odd
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
int32_t Unicode_ਸੈੱਟ::ਲੱਭੋ_ਕੋਡ_ਬਿੰਦੂ(UChar32 c) ਅਚਲ {
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
    ਜੇ (c < list[0])
        ਵਾਪਸ 0;
    // High runner test.  c is often after the last range, so an
    // initial check for this condition pays off.
    int32_t lo = 0;
    int32_t hi = len - 1;
    ਜੇ (lo >= hi || c >= list[hi-1])
        ਵਾਪਸ hi;
    // invariant: c >= list[lo]
    // invariant: c < list[hi]
    ਲਈ (;;) {
        int32_t i = (lo + hi) >> 1;
        ਜੇ (i == lo) {
            ਰੋਕੋ; // Found!
        } ਨਹੀਂਤਾਂ ਜੇ (c < list[i]) {
            hi = i;
        } ਨਹੀਂਤਾਂ {
            lo = i;
        }
    }
    ਵਾਪਸ hi;
}

/**
 * Returns true if this set contains every character
 * of the given range.
 * @param start first character, inclusive, of the range
 * @param end last character, inclusive, of the range
 * @return true if the test condition is met
 */
UBool Unicode_ਸੈੱਟ::ਸ਼ਾਮਲ_ਹੈ(UChar32 ਸ਼ੁਰੂ, UChar32 ਸਿਰਾ) ਅਚਲ {
    //int32_t i = -1;
    //for (;;) {
    //    if (start < list[++i]) break;
    //}
    int32_t i = ਲੱਭੋ_ਕੋਡ_ਬਿੰਦੂ(ਸ਼ੁਰੂ);
    ਵਾਪਸ ((i & 1) != 0 && ਸਿਰਾ < list[i]);
}

/**
 * Returns <tt>true</tt> if this set contains the given
 * multicharacter string.
 * @param s string to be checked for containment
 * @return <tt>true</tt> if this set contains the specified string
 */
UBool Unicode_ਸੈੱਟ::ਸ਼ਾਮਲ_ਹੈ(ਅਚਲ UnicodeString& s) ਅਚਲ {
    int32_t cp = getSingleCP(s);
    ਜੇ (cp < 0) {
        ਵਾਪਸ ਲੜੀਆਂ_ਸ਼ਾਮਲ_ਹੈ(s);
    } ਨਹੀਂਤਾਂ {
        ਵਾਪਸ ਸ਼ਾਮਲ_ਹੈ(static_cast<UChar32>(cp));
    }
}

/**
 * Returns true if this set contains all the characters and strings
 * of the given set.
 * @param c set to be checked for containment
 * @return true if the test condition is met
 */
UBool Unicode_ਸੈੱਟ::ਸ਼ਾਮਲ_ਹੈ_All(ਅਚਲ Unicode_ਸੈੱਟ& c) ਅਚਲ {
    // The specified set is a subset if all of its pairs are contained in
    // this set.  It's possible to code this more efficiently in terms of
    // direct manipulation of the inversion lists if the need arises.
    int32_t n = c.get_Range_ਗਿਣਤੀ();
    ਲਈ (ਪੂਰਨ i=0; i<n; ++i) {
        ਜੇ (!ਸ਼ਾਮਲ_ਹੈ(c.get_Range_ਸ਼ੁਰੂ(i), c.get_Range_ਸਿਰਾ(i))) {
            ਵਾਪਸ ਝੂਠ;
        }
    }
    ਵਾਪਸ !c.has_ਲੜੀਆਂ() || (ਲੜੀਆਂ_2 != ਖਾਲੀ_ਸੂਚਕ && ਲੜੀਆਂ_2->ਸ਼ਾਮਲ_ਹੈ_All(*c.ਲੜੀਆਂ_2));
}

/**
 * Returns true if this set contains all the characters
 * of the given string.
 * @param s string containing characters to be checked for containment
 * @return true if the test condition is met
 */
UBool Unicode_ਸੈੱਟ::ਸ਼ਾਮਲ_ਹੈ_All(ਅਚਲ UnicodeString& s) ਅਚਲ {
    ਵਾਪਸ ਵਿਸਤਾਰ(s.get_ਬਫਰ(), s.length(), USET_ਵਿਸਤਾਰ_CONTAINED) == s.length();
}

/**
 * Returns true if this set contains none of the characters
 * of the given range.
 * @param start first character, inclusive, of the range
 * @param end last character, inclusive, of the range
 * @return true if the test condition is met
 */
UBool Unicode_ਸੈੱਟ::ਸ਼ਾਮਲ_ਹੈ_None(UChar32 ਸ਼ੁਰੂ, UChar32 ਸਿਰਾ) ਅਚਲ {
    //int32_t i = -1;
    //for (;;) {
    //    if (start < list[++i]) break;
    //}
    int32_t i = ਲੱਭੋ_ਕੋਡ_ਬਿੰਦੂ(ਸ਼ੁਰੂ);
    ਵਾਪਸ ((i & 1) == 0 && ਸਿਰਾ < list[i]);
}

/**
 * Returns true if this set contains none of the characters and strings
 * of the given set.
 * @param c set to be checked for containment
 * @return true if the test condition is met
 */
UBool Unicode_ਸੈੱਟ::ਸ਼ਾਮਲ_ਹੈ_None(ਅਚਲ Unicode_ਸੈੱਟ& c) ਅਚਲ {
    // The specified set is a subset if all of its pairs are contained in
    // this set.  It's possible to code this more efficiently in terms of
    // direct manipulation of the inversion lists if the need arises.
    int32_t n = c.get_Range_ਗਿਣਤੀ();
    ਲਈ (int32_t i=0; i<n; ++i) {
        ਜੇ (!ਸ਼ਾਮਲ_ਹੈ_None(c.get_Range_ਸ਼ੁਰੂ(i), c.get_Range_ਸਿਰਾ(i))) {
            ਵਾਪਸ ਝੂਠ;
        }
    }
    ਵਾਪਸ ਲੜੀਆਂ_2 == ਖਾਲੀ_ਸੂਚਕ || !c.has_ਲੜੀਆਂ() || ਲੜੀਆਂ_2->ਸ਼ਾਮਲ_ਹੈ_None(*c.ਲੜੀਆਂ_2);
}

/**
 * Returns true if this set contains none of the characters
 * of the given string.
 * @param s string containing characters to be checked for containment
 * @return true if the test condition is met
 */
UBool Unicode_ਸੈੱਟ::ਸ਼ਾਮਲ_ਹੈ_None(ਅਚਲ UnicodeString& s) ਅਚਲ {
    ਵਾਪਸ ਵਿਸਤਾਰ(s.get_ਬਫਰ(), s.length(), USET_ਵਿਸਤਾਰ_NOT_CONTAINED) == s.length();
}

/**
 * Returns <tt>true</tt> if this set contains any character whose low byte
 * is the given value.  This is used by <tt>RuleBasedTransliterator</tt> for
 * indexing.
 */
UBool Unicode_ਸੈੱਟ::matches_ਸੂਚਕਾਂਕ_ਮੁੱਲ(uint8_t v) ਅਚਲ {
    /* The index value v, in the range [0,255], is contained in this set if
     * it is contained in any pair of this set.  Pairs either have the high
     * bytes equal, or unequal.  If the high bytes are equal, then we have
     * aaxx..aayy, where aa is the high byte.  Then v is contained if xx <=
     * v <= yy.  If the high bytes are unequal we have aaxx..bbyy, bb>aa.
     * Then v is contained if xx <= v || v <= yy.  (This is identical to the
     * time zone month containment logic.)
     */
    int32_t i;
    int32_t range_ਗਿਣਤੀ=get_Range_ਗਿਣਤੀ();
    ਲਈ (i=0; i<range_ਗਿਣਤੀ; ++i) {
        UChar32 ਨੀਵਾਂ = get_Range_ਸ਼ੁਰੂ(i);
        UChar32 ਉੱਚਾ = get_Range_ਸਿਰਾ(i);
        ਜੇ ((ਨੀਵਾਂ & ~0xFF) == (ਉੱਚਾ & ~0xFF)) {
            ਜੇ ((ਨੀਵਾਂ & 0xFF) <= v && v <= (ਉੱਚਾ & 0xFF)) {
                ਵਾਪਸ ਸੱਚ;
            }
        } ਨਹੀਂਤਾਂ ਜੇ ((ਨੀਵਾਂ & 0xFF) <= v || v <= (ਉੱਚਾ & 0xFF)) {
            ਵਾਪਸ ਸੱਚ;
        }
    }
    ਜੇ (has_ਲੜੀਆਂ()) {
        ਲਈ (i=0; i<ਲੜੀਆਂ_2->size(); ++i) {
            ਅਚਲ UnicodeString& s = *static_cast<ਅਚਲ UnicodeString*>(ਲੜੀਆਂ_2->elementAt(i));
            ਜੇ (s.is_ਖਾਲੀ_ਥਾਵਾਂ()) {
                ਜਾਰੀ;  // skip the empty string
            }
            UChar32 c = s.char32At(0);
            ਜੇ ((c & 0xFF) == v) {
                ਵਾਪਸ ਸੱਚ;
            }
        }
    }
    ਵਾਪਸ ਝੂਠ;
}

/**
 * Implementation of UnicodeMatcher::matches().  Always matches the
 * longest possible multichar string.
 */
UMatchDegree Unicode_ਸੈੱਟ::matches(ਅਚਲ Replaceable& ਪਾਠ,
                                 int32_t& ਵਿੱਥ,
                                 int32_t ਹੱਦਬੰਦੀ,
                                 UBool incremental) {
    ਜੇ (ਵਿੱਥ == ਹੱਦਬੰਦੀ) {
        ਜੇ (ਸ਼ਾਮਲ_ਹੈ(U_ETHER)) {
            ਵਾਪਸ incremental ? U_PARTIAL_ਮੇਲ : U_ਮੇਲ;
        } ਨਹੀਂਤਾਂ {
            ਵਾਪਸ U_MISMATCH;
        }
    } ਨਹੀਂਤਾਂ {
        ਜੇ (has_ਲੜੀਆਂ()) { // try strings first

            // might separate forward and backward loops later
            // for now they are combined

            // TODO Improve efficiency of this, at least in the forward
            // direction, if not in both.  In the forward direction we
            // can assume the strings are sorted.

            int32_t i;
            UBool forward = ਵਿੱਥ < ਹੱਦਬੰਦੀ;

            // firstChar is the leftmost char to match in the
            // forward direction or the rightmost char to match in
            // the reverse direction.
            char16_t ਪਹਿਲਾ_Char = ਪਾਠ.charAt(ਵਿੱਥ);

            // If there are multiple strings that can match we
            // return the longest match.
            int32_t ਉੱਚਾ_Water_ਲੰਮਾਈ = 0;

            ਲਈ (i=0; i<ਲੜੀਆਂ_2->size(); ++i) {
                ਅਚਲ UnicodeString& trial = *static_cast<ਅਚਲ UnicodeString*>(ਲੜੀਆਂ_2->elementAt(i));
                ਜੇ (trial.is_ਖਾਲੀ_ਥਾਵਾਂ()) {
                    ਜਾਰੀ;  // skip the empty string
                }

                char16_t c = trial.charAt(forward ? 0 : trial.length() - 1);

                // Strings are sorted, so we can optimize in the
                // forward direction.
                ਜੇ (forward && c > ਪਹਿਲਾ_Char) ਰੋਕੋ;
                ਜੇ (c != ਪਹਿਲਾ_Char) ਜਾਰੀ;

                int32_t ਮੇਲ_Len = ਮੇਲ_Rest(ਪਾਠ, ਵਿੱਥ, ਹੱਦਬੰਦੀ, trial);

                ਜੇ (incremental) {
                    int32_t maxLen = forward ? ਹੱਦਬੰਦੀ-ਵਿੱਥ : ਵਿੱਥ-ਹੱਦਬੰਦੀ;
                    ਜੇ (ਮੇਲ_Len == maxLen) {
                        // We have successfully matched but only up to limit.
                        ਵਾਪਸ U_PARTIAL_ਮੇਲ;
                    }
                }

                ਜੇ (ਮੇਲ_Len == trial.length()) {
                    // We have successfully matched the whole string.
                    ਜੇ (ਮੇਲ_Len > ਉੱਚਾ_Water_ਲੰਮਾਈ) {
                        ਉੱਚਾ_Water_ਲੰਮਾਈ = ਮੇਲ_Len;
                    }
                    // In the forward direction we know strings
                    // are sorted so we can bail early.
                    ਜੇ (forward && ਮੇਲ_Len < ਉੱਚਾ_Water_ਲੰਮਾਈ) {
                        ਰੋਕੋ;
                    }
                    ਜਾਰੀ;
                }
            }

            // We've checked all strings without a partial match.
            // If we have full matches, return the longest one.
            ਜੇ (ਉੱਚਾ_Water_ਲੰਮਾਈ != 0) {
                ਵਿੱਥ += forward ? ਉੱਚਾ_Water_ਲੰਮਾਈ : -ਉੱਚਾ_Water_ਲੰਮਾਈ;
                ਵਾਪਸ U_ਮੇਲ;
            }
        }
        ਵਾਪਸ UnicodeFilter::matches(ਪਾਠ, ਵਿੱਥ, ਹੱਦਬੰਦੀ, incremental);
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
int32_t Unicode_ਸੈੱਟ::ਮੇਲ_Rest(ਅਚਲ Replaceable& ਪਾਠ,
                              int32_t ਸ਼ੁਰੂ, int32_t ਹੱਦਬੰਦੀ,
                              ਅਚਲ UnicodeString& s) {
    int32_t i;
    int32_t maxLen;
    int32_t slen = s.length();
    ਜੇ (ਸ਼ੁਰੂ < ਹੱਦਬੰਦੀ) {
        maxLen = ਹੱਦਬੰਦੀ - ਸ਼ੁਰੂ;
        ਜੇ (maxLen > slen) maxLen = slen;
        ਲਈ (i = 1; i < maxLen; ++i) {
            ਜੇ (ਪਾਠ.charAt(ਸ਼ੁਰੂ + i) != s.charAt(i)) ਵਾਪਸ 0;
        }
    } ਨਹੀਂਤਾਂ {
        maxLen = ਸ਼ੁਰੂ - ਹੱਦਬੰਦੀ;
        ਜੇ (maxLen > slen) maxLen = slen;
        --slen; // <=> slen = s.length() - 1;
        ਲਈ (i = 1; i < maxLen; ++i) {
            ਜੇ (ਪਾਠ.charAt(ਸ਼ੁਰੂ - i) != s.charAt(slen - i)) ਵਾਪਸ 0;
        }
    }
    ਵਾਪਸ maxLen;
}

/**
 * Implement of UnicodeMatcher
 */
ਖਾਲੀ Unicode_ਸੈੱਟ::ਜੋੜੋ_ਮੇਲ_ਸੈੱਟ_ਮੰਜ਼ਿਲ(Unicode_ਸੈੱਟ& ਮੰਜ਼ਿਲ_Union_ਮੰਜ਼ਿਲ) ਅਚਲ {
    ਮੰਜ਼ਿਲ_Union_ਮੰਜ਼ਿਲ.ਜੋੜੋ_All(*ਇਹ);
}

/**
 * Returns the index of the given character within this set, where
 * the set is ordered by ascending code point.  If the character
 * is not in this set, return -1.  The inverse of this method is
 * <code>charAt()</code>.
 * @return an index from 0..size()-1, or -1
 */
int32_t Unicode_ਸੈੱਟ::ਸੂਚਕਾਂਕ_ਦਾ(UChar32 c) ਅਚਲ {
    ਜੇ (c < MIN_ਮੁੱਲ || c > MAX_ਮੁੱਲ) {
        ਵਾਪਸ -1;
    }
    int32_t i = 0;
    int32_t n = 0;
    ਲਈ (;;) {
        UChar32 ਸ਼ੁਰੂ = list[i++];
        ਜੇ (c < ਸ਼ੁਰੂ) {
            ਵਾਪਸ -1;
        }
        UChar32 ਹੱਦਬੰਦੀ = list[i++];
        ਜੇ (c < ਹੱਦਬੰਦੀ) {
            ਵਾਪਸ n + c - ਸ਼ੁਰੂ;
        }
        n += ਹੱਦਬੰਦੀ - ਸ਼ੁਰੂ;
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
UChar32 Unicode_ਸੈੱਟ::charAt(int32_t ਸੂਚਕਾਂਕ) ਅਚਲ {
    ਜੇ (ਸੂਚਕਾਂਕ >= 0) {
        // len2 is the largest even integer <= len, that is, it is len
        // for even values and len-1 for odd values.  With odd values
        // the last entry is UNICODESET_HIGH.
        int32_t len2 = len & ~1;
        ਲਈ (int32_t i=0; i < len2;) {
            UChar32 ਸ਼ੁਰੂ = list[i++];
            int32_t count = list[i++] - ਸ਼ੁਰੂ;
            ਜੇ (ਸੂਚਕਾਂਕ < count) {
                ਵਾਪਸ static_cast<UChar32>(ਸ਼ੁਰੂ + ਸੂਚਕਾਂਕ);
            }
            ਸੂਚਕਾਂਕ -= count;
        }
    }
    ਵਾਪਸ static_cast<UChar32>(-1);
}

/**
 * Make this object represent the range <code>start - end</code>.
 * If <code>end > start</code> then this object is set to an
 * an empty range.
 *
 * @param start first character in the set, inclusive
 * @rparam end last character in the set, inclusive
 */
Unicode_ਸੈੱਟ& Unicode_ਸੈੱਟ::ਸੈੱਟ(UChar32 ਸ਼ੁਰੂ, UChar32 ਸਿਰਾ) U_LIFETIME_BOUND {
    clear();
    complement(ਸ਼ੁਰੂ, ਸਿਰਾ);
    ਵਾਪਸ *ਇਹ;
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
Unicode_ਸੈੱਟ& Unicode_ਸੈੱਟ::ਜੋੜੋ(UChar32 ਸ਼ੁਰੂ, UChar32 ਸਿਰਾ) U_LIFETIME_BOUND {
    ਜੇ (pin_ਕੋਡ_ਬਿੰਦੂ(ਸ਼ੁਰੂ) < pin_ਕੋਡ_ਬਿੰਦੂ(ਸਿਰਾ)) {
        UChar32 ਹੱਦਬੰਦੀ = ਸਿਰਾ + 1;
        // Fast path for adding a new range after the last one.
        // Odd list length: [..., lastStart, lastLimit, HIGH]
        ਜੇ ((len & 1) != 0) {
            // If the list is empty, set lastLimit low enough to not be adjacent to 0.
            UChar32 ਆਖਰੀ_ਹੱਦਬੰਦੀ = len == 1 ? -2 : list[len - 2];
            ਜੇ (ਆਖਰੀ_ਹੱਦਬੰਦੀ <= ਸ਼ੁਰੂ && !isFrozen() && !isBogus()) {
                ਜੇ (ਆਖਰੀ_ਹੱਦਬੰਦੀ == ਸ਼ੁਰੂ) {
                    // Extend the last range.
                    list[len - 2] = ਹੱਦਬੰਦੀ;
                    ਜੇ (ਹੱਦਬੰਦੀ == UNICODESET_ਉੱਚਾ) {
                        --len;
                    }
                } ਨਹੀਂਤਾਂ {
                    list[len - 1] = ਸ਼ੁਰੂ;
                    ਜੇ (ਹੱਦਬੰਦੀ < UNICODESET_ਉੱਚਾ) {
                        ਜੇ (ensure_ਸਮਰੱਥਾ(len + 2)) {
                            list[len++] = ਹੱਦਬੰਦੀ;
                            list[len++] = UNICODESET_ਉੱਚਾ;
                        }
                    } ਨਹੀਂਤਾਂ {  // limit == UNICODESET_HIGH
                        ਜੇ (ensure_ਸਮਰੱਥਾ(len + 1)) {
                            list[len++] = UNICODESET_ਉੱਚਾ;
                        }
                    }
                }
                release_ਨਮੂਨਾ();
                ਵਾਪਸ *ਇਹ;
            }
        }
        // This is slow. Could be much faster using findCodePoint(start)
        // and modifying the list, dealing with adjacent & overlapping ranges.
        UChar32 range[3] = { ਸ਼ੁਰੂ, ਹੱਦਬੰਦੀ, UNICODESET_ਉੱਚਾ };
        ਜੋੜੋ(range, 2, 0);
    } ਨਹੀਂਤਾਂ ਜੇ (ਸ਼ੁਰੂ == ਸਿਰਾ) {
        ਜੋੜੋ(ਸ਼ੁਰੂ);
    }
    ਵਾਪਸ *ਇਹ;
}

// #define DEBUG_US_ADD

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ DEBUG_US_ਜੋੜੋ
#ਅਨ੍ਤਰ੍ਭਾਵ <stdio.h>
ਖਾਲੀ dump(UChar32 c) {
    ਜੇ (c <= 0xFF) {
        ਲਿਖੋ("%c", (ਅੱਖਰ)c);
    } ਨਹੀਂਤਾਂ {
        ਲਿਖੋ("U+%04X", c);
    }
}
ਖਾਲੀ dump(ਅਚਲ UChar32* list, int32_t len) {
    ਲਿਖੋ("[");
    ਲਈ (int32_t i=0; i<len; ++i) {
        ਜੇ (i != 0) ਲਿਖੋ(", ");
        dump(list[i]);
    }
    ਲਿਖੋ("]");
}
#ਜੇ_ਅੰਤ

/**
 * Adds the specified character to this set if it is not already
 * present.  If this set already contains the specified character,
 * the call leaves this set unchanged.
 */
Unicode_ਸੈੱਟ& Unicode_ਸੈੱਟ::ਜੋੜੋ(UChar32 c) U_LIFETIME_BOUND {
    // find smallest i such that c < list[i]
    // if odd, then it is IN the set
    // if even, then it is OUT of the set
    int32_t i = ਲੱਭੋ_ਕੋਡ_ਬਿੰਦੂ(pin_ਕੋਡ_ਬਿੰਦੂ(c));

    // already in set?
    ਜੇ ((i & 1) != 0  || isFrozen() || isBogus()) ਵਾਪਸ *ਇਹ;

    // HIGH is 0x110000
    // assert(list[len-1] == HIGH);

    // empty = [HIGH]
    // [start_0, limit_0, start_1, limit_1, HIGH]

    // [..., start_k-1, limit_k-1, start_k, limit_k, ..., HIGH]
    //                             ^
    //                             list[i]

    // i == 0 means c is before the first range

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ DEBUG_US_ਜੋੜੋ
    ਲਿਖੋ("Add of ");
    dump(c);
    ਲਿਖੋ(" found at %d", i);
    ਲਿਖੋ(": ");
    dump(list, len);
    ਲਿਖੋ(" => ");
#ਜੇ_ਅੰਤ

    ਜੇ (c == list[i]-1) {
        // c is before start of next range
        list[i] = c;
        // if we touched the HIGH mark, then add a new one
        ਜੇ (c == (UNICODESET_ਉੱਚਾ - 1)) {
            ਜੇ (!ensure_ਸਮਰੱਥਾ(len+1)) {
                // ensureCapacity will mark the object as Bogus if OOM failure happens.
                ਵਾਪਸ *ਇਹ;
            }
            list[len++] = UNICODESET_ਉੱਚਾ;
        }
        ਜੇ (i > 0 && c == list[i-1]) {
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
            ਜਦੋਂ (src < srclimit) *(dst++) = *(src++);

            len -= 2;
        }
    }

    ਨਹੀਂਤਾਂ ਜੇ (i > 0 && c == list[i-1]) {
        // c is after end of prior range
        list[i-1]++;
        // no need to check for collapse here
    }

    ਨਹੀਂਤਾਂ {
        // At this point we know the new char is not adjacent to
        // any existing ranges, and it is not 10FFFF.


        // [..., start_k-1, limit_k-1, start_k, limit_k, ..., HIGH]
        //                             ^
        //                             list[i]

        // [..., start_k-1, limit_k-1, c, c+1, start_k, limit_k, ..., HIGH]
        //                             ^
        //                             list[i]

        ਜੇ (!ensure_ਸਮਰੱਥਾ(len+2)) {
            // ensureCapacity will mark the object as Bogus if OOM failure happens.
            ਵਾਪਸ *ਇਹ;
        }

        UChar32 *p = list + i;
        uprv_memmove(p + 2, p, (len - i) * ਆਕਾਰ(*p));
        list[i] = c;
        list[i+1] = c+1;
        len += 2;
    }

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ DEBUG_US_ਜੋੜੋ
    dump(list, len);
    ਲਿਖੋ("\n");

    ਲਈ (i=1; i<len; ++i) {
        ਜੇ (list[i] <= list[i-1]) {
            // Corrupt array!
            ਲਿਖੋ("ERROR: list has been corrupted\n");
            ਬਾਹਰ_ਨਿਕਲੋ(1);
        }
    }
#ਜੇ_ਅੰਤ

    release_ਨਮੂਨਾ();
    ਵਾਪਸ *ਇਹ;
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
Unicode_ਸੈੱਟ& Unicode_ਸੈੱਟ::ਜੋੜੋ(ਅਚਲ UnicodeString& s) U_LIFETIME_BOUND {
    ਜੇ (isFrozen() || isBogus()) ਵਾਪਸ *ਇਹ;
    int32_t cp = getSingleCP(s);
    ਜੇ (cp < 0) {
        ਜੇ (!ਲੜੀਆਂ_ਸ਼ਾਮਲ_ਹੈ(s)) {
            ਜੋੜੋ_2(s);
            release_ਨਮੂਨਾ();
        }
    } ਨਹੀਂਤਾਂ {
        ਜੋੜੋ(static_cast<UChar32>(cp));
    }
    ਵਾਪਸ *ਇਹ;
}

/**
 * Adds the given string, in order, to 'strings_'.  The given string
 * must have been checked by the caller to not already be in 'strings_'.
 */
ਖਾਲੀ Unicode_ਸੈੱਟ::ਜੋੜੋ_2(ਅਚਲ UnicodeString& s) {
    ਜੇ (isFrozen() || isBogus()) {
        ਵਾਪਸ;
    }
    UError_ਕੋਡ ec = U_ZERO_ERROR;
    ਜੇ (ਲੜੀਆਂ_2 == ਖਾਲੀ_ਸੂਚਕ && !allocate_ਲੜੀਆਂ(ec)) {
        ਸੈੱਟ_ਮੰਜ਼ਿਲ_Bogus();
        ਵਾਪਸ;
    }
    Local_ਸੂਚਕ<UnicodeString> t(ਨਵਾਂ UnicodeString(s));
    ਜੇ (t.isNull()) { // Check for memory allocation error.
        ਸੈੱਟ_ਮੰਜ਼ਿਲ_Bogus();
        ਵਾਪਸ;
    }
    ਲੜੀਆਂ_2->sorted_ਪਾਓ(t.orphan(), compareUnicodeString, ec);
    ਜੇ (U_FAILURE(ec)) {
        ਸੈੱਟ_ਮੰਜ਼ਿਲ_Bogus();
    }
}

/**
 * @return a code point IF the string consists of a single one.
 * otherwise returns -1.
 * @param string to test
 */
int32_t Unicode_ਸੈੱਟ::getSingleCP(ਅਚਲ UnicodeString& s) {
    int32_t s_ਲੰਮਾਈ = s.length();
    ਜੇ (s_ਲੰਮਾਈ == 1) ਵਾਪਸ s.charAt(0);
    ਜੇ (s_ਲੰਮਾਈ == 2) {
        UChar32 cp = s.char32At(0);
        ਜੇ (cp > 0xFFFF) { // is surrogate pair
            ਵਾਪਸ cp;
        }
    }
    ਵਾਪਸ -1;
}

/**
 * Adds each of the characters in this string to the set. Thus "ch" => {"c", "h"}
 * If this set already any particular character, it has no effect on that character.
 * @param the source string
 * @return the modified set, for chaining
 */
Unicode_ਸੈੱਟ& Unicode_ਸੈੱਟ::ਜੋੜੋ_All(ਅਚਲ UnicodeString& s) U_LIFETIME_BOUND {
    UChar32 cp;
    ਲਈ (int32_t i = 0; i < s.length(); i += U16_ਲੰਮਾਈ(cp)) {
        cp = s.char32At(i);
        ਜੋੜੋ(cp);
    }
    ਵਾਪਸ *ਇਹ;
}

/**
 * Retains EACH of the characters in this string. Note: "ch" == {"c", "h"}
 * If this set already any particular character, it has no effect on that character.
 * @param the source string
 * @return the modified set, for chaining
 */
Unicode_ਸੈੱਟ& Unicode_ਸੈੱਟ::retainAll(ਅਚਲ UnicodeString& s) U_LIFETIME_BOUND {
    Unicode_ਸੈੱਟ ਸੈੱਟ;
    ਸੈੱਟ.ਜੋੜੋ_All(s);
    retainAll(ਸੈੱਟ);
    ਵਾਪਸ *ਇਹ;
}

/**
 * Complement EACH of the characters in this string. Note: "ch" == {"c", "h"}
 * If this set already any particular character, it has no effect on that character.
 * @param the source string
 * @return the modified set, for chaining
 */
Unicode_ਸੈੱਟ& Unicode_ਸੈੱਟ::complementAll(ਅਚਲ UnicodeString& s) U_LIFETIME_BOUND {
    Unicode_ਸੈੱਟ ਸੈੱਟ;
    ਸੈੱਟ.ਜੋੜੋ_All(s);
    complementAll(ਸੈੱਟ);
    ਵਾਪਸ *ਇਹ;
}

/**
 * Remove EACH of the characters in this string. Note: "ch" == {"c", "h"}
 * If this set already any particular character, it has no effect on that character.
 * @param the source string
 * @return the modified set, for chaining
 */
Unicode_ਸੈੱਟ& Unicode_ਸੈੱਟ::removeAll(ਅਚਲ UnicodeString& s) U_LIFETIME_BOUND {
    Unicode_ਸੈੱਟ ਸੈੱਟ;
    ਸੈੱਟ.ਜੋੜੋ_All(s);
    removeAll(ਸੈੱਟ);
    ਵਾਪਸ *ਇਹ;
}

Unicode_ਸੈੱਟ& Unicode_ਸੈੱਟ::remove_All_ਲੜੀਆਂ() U_LIFETIME_BOUND {
    ਜੇ (!isFrozen() && has_ਲੜੀਆਂ()) {
        ਲੜੀਆਂ_2->removeAllElements();
        release_ਨਮੂਨਾ();
    }
    ਵਾਪਸ *ਇਹ;
}


/**
 * Makes a set from a multicharacter string. Thus "ch" => {"ch"}
 * <br><b>Warning: you cannot add an empty string ("") to a UnicodeSet.</b>
 * @param the source string
 * @return a newly created set containing the given string
 */
Unicode_ਸੈੱਟ* U_EXPORT2 Unicode_ਸੈੱਟ::createFrom(ਅਚਲ UnicodeString& s) {
    Unicode_ਸੈੱਟ *ਸੈੱਟ = ਨਵਾਂ Unicode_ਸੈੱਟ();
    ਜੇ (ਸੈੱਟ != ਖਾਲੀ_ਸੂਚਕ) { // Check for memory allocation error.
        ਸੈੱਟ->ਜੋੜੋ(s);
    }
    ਵਾਪਸ ਸੈੱਟ;
}


/**
 * Makes a set from each of the characters in the string. Thus "ch" => {"c", "h"}
 * @param the source string
 * @return a newly created set containing the given characters
 */
Unicode_ਸੈੱਟ* U_EXPORT2 Unicode_ਸੈੱਟ::createFromAll(ਅਚਲ UnicodeString& s) {
    Unicode_ਸੈੱਟ *ਸੈੱਟ = ਨਵਾਂ Unicode_ਸੈੱਟ();
    ਜੇ (ਸੈੱਟ != ਖਾਲੀ_ਸੂਚਕ) { // Check for memory allocation error.
        ਸੈੱਟ->ਜੋੜੋ_All(s);
    }
    ਵਾਪਸ ਸੈੱਟ;
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
Unicode_ਸੈੱਟ& Unicode_ਸੈੱਟ::retain(UChar32 ਸ਼ੁਰੂ, UChar32 ਸਿਰਾ) U_LIFETIME_BOUND {
    ਜੇ (pin_ਕੋਡ_ਬਿੰਦੂ(ਸ਼ੁਰੂ) <= pin_ਕੋਡ_ਬਿੰਦੂ(ਸਿਰਾ)) {
        UChar32 range[3] = { ਸ਼ੁਰੂ, ਸਿਰਾ+1, UNICODESET_ਉੱਚਾ };
        retain(range, 2, 0);
    } ਨਹੀਂਤਾਂ {
        clear();
    }
    ਵਾਪਸ *ਇਹ;
}

Unicode_ਸੈੱਟ& Unicode_ਸੈੱਟ::retain(UChar32 c) U_LIFETIME_BOUND {
    ਵਾਪਸ retain(c, c);
}

Unicode_ਸੈੱਟ& Unicode_ਸੈੱਟ::retain(ਅਚਲ UnicodeString &s) U_LIFETIME_BOUND {
    ਜੇ (isFrozen() || isBogus()) { ਵਾਪਸ *ਇਹ; }
    UChar32 cp = getSingleCP(s);
    ਜੇ (cp < 0) {
        ਤਾਰਕਿਕ is_ਵਿੱਚ = ਲੜੀਆਂ_ਸ਼ਾਮਲ_ਹੈ(s);
        // Check for getRangeCount() first to avoid somewhat-expensive size()
        // when there are single code points.
        ਜੇ (is_ਵਿੱਚ && get_Range_ਗਿਣਤੀ() == 0 && size() == 1) {
            ਵਾਪਸ *ਇਹ;
        }
        clear();
        ਜੇ (is_ਵਿੱਚ) {
            ਜੋੜੋ_2(s);
        }
    } ਨਹੀਂਤਾਂ {
        retain(cp, cp);
    }
    ਵਾਪਸ *ਇਹ;
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
Unicode_ਸੈੱਟ& Unicode_ਸੈੱਟ::remove(UChar32 ਸ਼ੁਰੂ, UChar32 ਸਿਰਾ) U_LIFETIME_BOUND {
    ਜੇ (pin_ਕੋਡ_ਬਿੰਦੂ(ਸ਼ੁਰੂ) <= pin_ਕੋਡ_ਬਿੰਦੂ(ਸਿਰਾ)) {
        UChar32 range[3] = { ਸ਼ੁਰੂ, ਸਿਰਾ+1, UNICODESET_ਉੱਚਾ };
        retain(range, 2, 2);
    }
    ਵਾਪਸ *ਇਹ;
}

/**
 * Removes the specified character from this set if it is present.
 * The set will not contain the specified range once the call
 * returns.
 */
Unicode_ਸੈੱਟ& Unicode_ਸੈੱਟ::remove(UChar32 c) U_LIFETIME_BOUND {
    ਵਾਪਸ remove(c, c);
}

/**
 * Removes the specified string from this set if it is present.
 * The set will not contain the specified character once the call
 * returns.
 * @param the source string
 * @return the modified set, for chaining
 */
Unicode_ਸੈੱਟ& Unicode_ਸੈੱਟ::remove(ਅਚਲ UnicodeString& s) U_LIFETIME_BOUND {
    ਜੇ (isFrozen() || isBogus()) ਵਾਪਸ *ਇਹ;
    int32_t cp = getSingleCP(s);
    ਜੇ (cp < 0) {
        ਜੇ (ਲੜੀਆਂ_2 != ਖਾਲੀ_ਸੂਚਕ && ਲੜੀਆਂ_2->removeElement((ਖਾਲੀ*) &s)) {
            release_ਨਮੂਨਾ();
        }
    } ਨਹੀਂਤਾਂ {
        remove(static_cast<UChar32>(cp), static_cast<UChar32>(cp));
    }
    ਵਾਪਸ *ਇਹ;
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
Unicode_ਸੈੱਟ& Unicode_ਸੈੱਟ::complement(UChar32 ਸ਼ੁਰੂ, UChar32 ਸਿਰਾ) U_LIFETIME_BOUND {
    ਜੇ (isFrozen() || isBogus()) {
        ਵਾਪਸ *ਇਹ;
    }
    ਜੇ (pin_ਕੋਡ_ਬਿੰਦੂ(ਸ਼ੁਰੂ) <= pin_ਕੋਡ_ਬਿੰਦੂ(ਸਿਰਾ)) {
        UChar32 range[3] = { ਸ਼ੁਰੂ, ਸਿਰਾ+1, UNICODESET_ਉੱਚਾ };
        exclusiveOr(range, 2, 0);
    }
    release_ਨਮੂਨਾ();
    ਵਾਪਸ *ਇਹ;
}

Unicode_ਸੈੱਟ& Unicode_ਸੈੱਟ::complement(UChar32 c) U_LIFETIME_BOUND {
    ਵਾਪਸ complement(c, c);
}

/**
 * This is equivalent to
 * <code>complement(MIN_VALUE, MAX_VALUE)</code>.
 */
Unicode_ਸੈੱਟ& Unicode_ਸੈੱਟ::complement() U_LIFETIME_BOUND {
    ਜੇ (isFrozen() || isBogus()) {
        ਵਾਪਸ *ਇਹ;
    }
    ਜੇ (list[0] == UNICODESET_ਨੀਵਾਂ) {
        uprv_memmove(list, list + 1, (size_t)(len-1)*ਆਕਾਰ(UChar32));
        --len;
    } ਨਹੀਂਤਾਂ {
        ਜੇ (!ensure_ਸਮਰੱਥਾ(len+1)) {
            ਵਾਪਸ *ਇਹ;
        }
        uprv_memmove(list + 1, list, (size_t)len*ਆਕਾਰ(UChar32));
        list[0] = UNICODESET_ਨੀਵਾਂ;
        ++len;
    }
    release_ਨਮੂਨਾ();
    ਵਾਪਸ *ਇਹ;
}

/**
 * Complement the specified string in this set.
 * The set will not contain the specified string once the call
 * returns.
 *
 * @param s the string to complement
 * @return this object, for chaining
 */
Unicode_ਸੈੱਟ& Unicode_ਸੈੱਟ::complement(ਅਚਲ UnicodeString& s) U_LIFETIME_BOUND {
    ਜੇ (isFrozen() || isBogus()) ਵਾਪਸ *ਇਹ;
    int32_t cp = getSingleCP(s);
    ਜੇ (cp < 0) {
        ਜੇ (ਲੜੀਆਂ_ਸ਼ਾਮਲ_ਹੈ(s)) {
            ਲੜੀਆਂ_2->removeElement((ਖਾਲੀ*) &s);
        } ਨਹੀਂਤਾਂ {
            ਜੋੜੋ_2(s);
        }
        release_ਨਮੂਨਾ();
    } ਨਹੀਂਤਾਂ {
        complement(static_cast<UChar32>(cp), static_cast<UChar32>(cp));
    }
    ਵਾਪਸ *ਇਹ;
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
Unicode_ਸੈੱਟ& Unicode_ਸੈੱਟ::ਜੋੜੋ_All(ਅਚਲ Unicode_ਸੈੱਟ& c) U_LIFETIME_BOUND {
    ਜੇ ( c.len>0 && c.list!=ਖਾਲੀ_ਸੂਚਕ ) {
        ਜੋੜੋ(c.list, c.len, 0);
    }

    // Add strings in order
    ਜੇ ( c.ਲੜੀਆਂ_2!=ਖਾਲੀ_ਸੂਚਕ ) {
        ਲਈ (int32_t i=0; i<c.ਲੜੀਆਂ_2->size(); ++i) {
            ਅਚਲ UnicodeString* s = static_cast<ਅਚਲ UnicodeString*>(c.ਲੜੀਆਂ_2->elementAt(i));
            ਜੇ (!ਲੜੀਆਂ_ਸ਼ਾਮਲ_ਹੈ(*s)) {
                ਜੋੜੋ_2(*s);
            }
        }
    }
    ਵਾਪਸ *ਇਹ;
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
Unicode_ਸੈੱਟ& Unicode_ਸੈੱਟ::retainAll(ਅਚਲ Unicode_ਸੈੱਟ& c) U_LIFETIME_BOUND {
    ਜੇ (isFrozen() || isBogus()) {
        ਵਾਪਸ *ਇਹ;
    }
    retain(c.list, c.len, 0);
    ਜੇ (has_ਲੜੀਆਂ()) {
        ਜੇ (!c.has_ਲੜੀਆਂ()) {
            ਲੜੀਆਂ_2->removeAllElements();
        } ਨਹੀਂਤਾਂ {
            ਲੜੀਆਂ_2->retainAll(*c.ਲੜੀਆਂ_2);
        }
    }
    ਵਾਪਸ *ਇਹ;
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
Unicode_ਸੈੱਟ& Unicode_ਸੈੱਟ::removeAll(ਅਚਲ Unicode_ਸੈੱਟ& c) U_LIFETIME_BOUND {
    ਜੇ (isFrozen() || isBogus()) {
        ਵਾਪਸ *ਇਹ;
    }
    retain(c.list, c.len, 2);
    ਜੇ (has_ਲੜੀਆਂ() && c.has_ਲੜੀਆਂ()) {
        ਲੜੀਆਂ_2->removeAll(*c.ਲੜੀਆਂ_2);
    }
    ਵਾਪਸ *ਇਹ;
}

/**
 * Complements in this set all elements contained in the specified
 * set.  Any character in the other set will be removed if it is
 * in this set, or will be added if it is not in this set.
 *
 * @param c set that defines which elements will be xor'ed from
 *          this set.
 */
Unicode_ਸੈੱਟ& Unicode_ਸੈੱਟ::complementAll(ਅਚਲ Unicode_ਸੈੱਟ& c) U_LIFETIME_BOUND {
    ਜੇ (isFrozen() || isBogus()) {
        ਵਾਪਸ *ਇਹ;
    }
    exclusiveOr(c.list, c.len, 0);

    ਜੇ (c.ਲੜੀਆਂ_2 != ਖਾਲੀ_ਸੂਚਕ) {
        ਲਈ (int32_t i=0; i<c.ਲੜੀਆਂ_2->size(); ++i) {
            ਖਾਲੀ* e = c.ਲੜੀਆਂ_2->elementAt(i);
            ਜੇ (ਲੜੀਆਂ_2 == ਖਾਲੀ_ਸੂਚਕ || !ਲੜੀਆਂ_2->removeElement(e)) {
                ਜੋੜੋ_2(*static_cast<ਅਚਲ UnicodeString*>(e));
            }
        }
    }
    ਵਾਪਸ *ਇਹ;
}

/**
 * Removes all of the elements from this set.  This set will be
 * empty after this call returns.
 */
Unicode_ਸੈੱਟ& Unicode_ਸੈੱਟ::clear() U_LIFETIME_BOUND {
    ਜੇ (isFrozen()) {
        ਵਾਪਸ *ਇਹ;
    }
    list[0] = UNICODESET_ਉੱਚਾ;
    len = 1;
    release_ਨਮੂਨਾ();
    ਜੇ (ਲੜੀਆਂ_2 != ਖਾਲੀ_ਸੂਚਕ) {
        ਲੜੀਆਂ_2->removeAllElements();
    }
    // Remove bogus
    fFlags = 0;
    ਵਾਪਸ *ਇਹ;
}

/**
 * Iteration method that returns the number of ranges contained in
 * this set.
 * @see #getRangeStart
 * @see #getRangeEnd
 */
int32_t Unicode_ਸੈੱਟ::get_Range_ਗਿਣਤੀ() ਅਚਲ {
    ਵਾਪਸ len/2;
}

/**
 * Iteration method that returns the first character in the
 * specified range of this set.
 * @see #getRangeCount
 * @see #getRangeEnd
 */
UChar32 Unicode_ਸੈੱਟ::get_Range_ਸ਼ੁਰੂ(int32_t ਸੂਚਕਾਂਕ) ਅਚਲ {
    ਵਾਪਸ list[ਸੂਚਕਾਂਕ*2];
}

/**
 * Iteration method that returns the last character in the
 * specified range of this set.
 * @see #getRangeStart
 * @see #getRangeEnd
 */
UChar32 Unicode_ਸੈੱਟ::get_Range_ਸਿਰਾ(int32_t ਸੂਚਕਾਂਕ) ਅਚਲ {
    ਵਾਪਸ list[ਸੂਚਕਾਂਕ*2 + 1] - 1;
}

ਅਚਲ UnicodeString* Unicode_ਸੈੱਟ::getString(int32_t ਸੂਚਕਾਂਕ) ਅਚਲ U_LIFETIME_BOUND {
    ਵਾਪਸ static_cast<ਅਚਲ UnicodeString*>(ਲੜੀਆਂ_2->elementAt(ਸੂਚਕਾਂਕ));
}

/**
 * Reallocate this objects internal structures to take up the least
 * possible space, without changing this object's value.
 */
Unicode_ਸੈੱਟ& Unicode_ਸੈੱਟ::compact() U_LIFETIME_BOUND {
    ਜੇ (isFrozen() || isBogus()) {
        ਵਾਪਸ *ਇਹ;
    }
    // Delete buffer first to defragment memory less.
    ਜੇ (ਬਫਰ != ਢੇਰ_List) {
        uprv_free(ਬਫਰ);
        ਬਫਰ = ਖਾਲੀ_ਸੂਚਕ;
        ਬਫਰ_ਸਮਰੱਥਾ = 0;
    }
    ਜੇ (list == ਢੇਰ_List) {
        // pass
    } ਨਹੀਂਤਾਂ ਜੇ (len <= INITIAL_ਸਮਰੱਥਾ) {
        uprv_memcpy(ਢੇਰ_List, list, len * ਆਕਾਰ(UChar32));
        uprv_free(list);
        list = ਢੇਰ_List;
        capacity = INITIAL_ਸਮਰੱਥਾ;
    } ਨਹੀਂਤਾਂ ਜੇ ((len + 7) < capacity) {
        // If we have more than a little unused capacity, shrink it to len.
        UChar32* ਆਰਜ਼ੀ = static_cast<UChar32*>(uprv_realloc(list, ਆਕਾਰ(UChar32) * len));
        ਜੇ (ਆਰਜ਼ੀ) {
            list = ਆਰਜ਼ੀ;
            capacity = len;
        }
        // else what the heck happened?! We allocated less memory!
        // Oh well. We'll keep our original array.
    }
    ਜੇ (ਲੜੀਆਂ_2 != ਖਾਲੀ_ਸੂਚਕ && ਲੜੀਆਂ_2->is_ਖਾਲੀ_ਥਾਵਾਂ()) {
        ਮਿਟਾਓ ਲੜੀਆਂ_2;
        ਲੜੀਆਂ_2 = ਖਾਲੀ_ਸੂਚਕ;
    }
    ਵਾਪਸ *ਇਹ;
}

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ DEBUG_SERIALIZE
#ਅਨ੍ਤਰ੍ਭਾਵ <stdio.h>
#ਜੇ_ਅੰਤ

/**
 * Deserialize constructor.
 */
Unicode_ਸੈੱਟ::Unicode_ਸੈੱਟ(ਅਚਲ uint16_t ਅੰਕੜੇ[], int32_t ਅੰਕੜੇ_Len, ESerialization serialization,
                       UError_ਕੋਡ &ec) {

  ਜੇ(U_FAILURE(ec)) {
    ਸੈੱਟ_ਮੰਜ਼ਿਲ_Bogus();
    ਵਾਪਸ;
  }

  ਜੇ( (serialization != kSerialized)
      || (ਅੰਕੜੇ==ਖਾਲੀ_ਸੂਚਕ)
      || (ਅੰਕੜੇ_Len < 1)) {
    ec = U_ILLEGAL_ਦਲੀਲ_ERROR;
    ਸੈੱਟ_ਮੰਜ਼ਿਲ_Bogus();
    ਵਾਪਸ;
  }

  // bmp?
  int32_t ਸਿਰਲੇਖ_ਮਾਪ = ((ਅੰਕੜੇ[0]&0x8000)) ?2:1;
  int32_t bmp_ਲੰਮਾਈ = (ਸਿਰਲੇਖ_ਮਾਪ==1)?ਅੰਕੜੇ[0]:ਅੰਕੜੇ[1];

  int32_t new_ਲੰਮਾਈ = (((ਅੰਕੜੇ[0]&0x7FFF)-bmp_ਲੰਮਾਈ)/2)+bmp_ਲੰਮਾਈ;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ DEBUG_SERIALIZE
  ਲਿਖੋ("dataLen %d headerSize %d bmpLen %d len %d. data[0]=%X/%X/%X/%X\n", ਅੰਕੜੇ_Len,ਸਿਰਲੇਖ_ਮਾਪ,bmp_ਲੰਮਾਈ,new_ਲੰਮਾਈ, ਅੰਕੜੇ[0],ਅੰਕੜੇ[1],ਅੰਕੜੇ[2],ਅੰਕੜੇ[3]);
#ਜੇ_ਅੰਤ
  ਜੇ(!ensure_ਸਮਰੱਥਾ(new_ਲੰਮਾਈ + 1)) {  // +1 for HIGH
    ਵਾਪਸ;
  }
  // copy bmp
  int32_t i;
  ਲਈ(i = 0; i< bmp_ਲੰਮਾਈ;i++) {
    list[i] = ਅੰਕੜੇ[i+ਸਿਰਲੇਖ_ਮਾਪ];
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ DEBUG_SERIALIZE
    ਲਿਖੋ("<<16@%d[%d] %X\n", i+ਸਿਰਲੇਖ_ਮਾਪ, i, list[i]);
#ਜੇ_ਅੰਤ
  }
  // copy smp
  ਲਈ(i=bmp_ਲੰਮਾਈ;i<new_ਲੰਮਾਈ;i++) {
    list[i] = (static_cast<UChar32>(ਅੰਕੜੇ[ਸਿਰਲੇਖ_ਮਾਪ + bmp_ਲੰਮਾਈ + (i - bmp_ਲੰਮਾਈ) * 2 + 0]) << 16) +
               static_cast<UChar32>(ਅੰਕੜੇ[ਸਿਰਲੇਖ_ਮਾਪ + bmp_ਲੰਮਾਈ + (i - bmp_ਲੰਮਾਈ) * 2 + 1]);
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ DEBUG_SERIALIZE
    ਲਿਖੋ("<<32@%d+[%d] %lX\n", ਸਿਰਲੇਖ_ਮਾਪ+bmp_ਲੰਮਾਈ+i, i, list[i]);
#ਜੇ_ਅੰਤ
  }
  U_ASSERT(i == new_ਲੰਮਾਈ);
  ਜੇ (i == 0 || list[i - 1] != UNICODESET_ਉੱਚਾ) {
    list[i++] = UNICODESET_ਉੱਚਾ;
  }
  len = i;
}


int32_t Unicode_ਸੈੱਟ::serialize(uint16_t *dest, int32_t dest_ਸਮਰੱਥਾ, UError_ਕੋਡ& ec) ਅਚਲ {
    int32_t bmp_ਲੰਮਾਈ, length, dest_ਲੰਮਾਈ;

    ਜੇ (U_FAILURE(ec)) {
        ਵਾਪਸ 0;
    }

    ਜੇ (dest_ਸਮਰੱਥਾ<0 || (dest_ਸਮਰੱਥਾ>0 && dest==ਖਾਲੀ_ਸੂਚਕ)) {
        ec=U_ILLEGAL_ਦਲੀਲ_ERROR;
        ਵਾਪਸ 0;
    }

    /* count necessary 16-bit units */
    length=ਇਹ->len-1; // Subtract 1 to ignore final UNICODESET_HIGH
    // assert(length>=0);
    ਜੇ (length==0) {
        /* empty set */
        ਜੇ (dest_ਸਮਰੱਥਾ>0) {
            *dest=0;
        } ਨਹੀਂਤਾਂ {
            ec=U_ਬਫਰ_ਉਛਾਲ_ERROR;
        }
        ਵਾਪਸ 1;
    }
    /* now length>0 */

    ਜੇ (ਇਹ->list[length-1]<=0xffff) {
        /* all BMP */
        bmp_ਲੰਮਾਈ=length;
    } ਨਹੀਂਤਾਂ ਜੇ (ਇਹ->list[0]>=0x10000) {
        /* all supplementary */
        bmp_ਲੰਮਾਈ=0;
        length*=2;
    } ਨਹੀਂਤਾਂ {
        /* some BMP, some supplementary */
        ਲਈ (bmp_ਲੰਮਾਈ=0; bmp_ਲੰਮਾਈ<length && ਇਹ->list[bmp_ਲੰਮਾਈ]<=0xffff; ++bmp_ਲੰਮਾਈ) {}
        length=bmp_ਲੰਮਾਈ+2*(length-bmp_ਲੰਮਾਈ);
    }
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ DEBUG_SERIALIZE
    ਲਿਖੋ(">> bmpLength%d length%d len%d\n", bmp_ਲੰਮਾਈ, length, len);
#ਜੇ_ਅੰਤ
    /* length: number of 16-bit array units */
    ਜੇ (length>0x7fff) {
        /* there are only 15 bits for the length in the first serialized word */
        ec=U_ਸੂਚਕਾਂਕ_OUTOFBOUNDS_ERROR;
        ਵਾਪਸ 0;
    }

    /*
     * total serialized length:
     * number of 16-bit array units (length) +
     * 1 length unit (always) +
     * 1 bmpLength unit (if there are supplementary values)
     */
    dest_ਲੰਮਾਈ=length+((length>bmp_ਲੰਮਾਈ)?2:1);
    ਜੇ (dest_ਲੰਮਾਈ<=dest_ਸਮਰੱਥਾ) {
        ਅਚਲ UChar32 *p;
        int32_t i;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ DEBUG_SERIALIZE
        ਲਿਖੋ("writeHdr\n");
#ਜੇ_ਅੰਤ
        *dest = static_cast<uint16_t>(length);
        ਜੇ (length>bmp_ਲੰਮਾਈ) {
            *dest|=0x8000;
            *++dest = static_cast<uint16_t>(bmp_ਲੰਮਾਈ);
        }
        ++dest;

        /* write the BMP part of the array */
        p=ਇਹ->list;
        ਲਈ (i=0; i<bmp_ਲੰਮਾਈ; ++i) {
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ DEBUG_SERIALIZE
          ਲਿਖੋ("writebmp: %x\n", (ਪੂਰਨ)*p);
#ਜੇ_ਅੰਤ
            *dest++ = static_cast<uint16_t>(*p++);
        }

        /* write the supplementary part of the array */
        ਲਈ (; i<length; i+=2) {
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ DEBUG_SERIALIZE
          ਲਿਖੋ("write32: %x\n", (ਪੂਰਨ)*p);
#ਜੇ_ਅੰਤ
            *dest++ = static_cast<uint16_t>(*p >> 16);
            *dest++ = static_cast<uint16_t>(*p++);
        }
    } ਨਹੀਂਤਾਂ {
        ec=U_ਬਫਰ_ਉਛਾਲ_ERROR;
    }
    ਵਾਪਸ dest_ਲੰਮਾਈ;
}

//----------------------------------------------------------------
// Implementation: Utility methods
//----------------------------------------------------------------

/**
 * Allocate our strings vector and return true if successful.
 */
UBool Unicode_ਸੈੱਟ::allocate_ਲੜੀਆਂ(UError_ਕੋਡ &ਸਥਿਤੀ) {
    ਜੇ (U_FAILURE(ਸਥਿਤੀ)) {
        ਵਾਪਸ ਝੂਠ;
    }
    ਲੜੀਆਂ_2 = ਨਵਾਂ UVector(uprv_deleteUObject,
                          uhash_compareUnicodeString, 1, ਸਥਿਤੀ);
    ਜੇ (ਲੜੀਆਂ_2 == ਖਾਲੀ_ਸੂਚਕ) { // Check for memory allocation error.
        ਸਥਿਤੀ = U_ਸਮ੍ਰਿਤੀ_ਵੰਡ_ERROR;
        ਵਾਪਸ ਝੂਠ;
    }
    ਜੇ (U_FAILURE(ਸਥਿਤੀ)) {
        ਮਿਟਾਓ ਲੜੀਆਂ_2;
        ਲੜੀਆਂ_2 = ਖਾਲੀ_ਸੂਚਕ;
        ਵਾਪਸ ਝੂਠ;
    } 
    ਵਾਪਸ ਸੱਚ;
}

int32_t Unicode_ਸੈੱਟ::ਅਗਲਾ_ਸਮਰੱਥਾ(int32_t min_ਸਮਰੱਥਾ) {
    // Grow exponentially to reduce the frequency of allocations.
    ਜੇ (min_ਸਮਰੱਥਾ < INITIAL_ਸਮਰੱਥਾ) {
        ਵਾਪਸ min_ਸਮਰੱਥਾ + INITIAL_ਸਮਰੱਥਾ;
    } ਨਹੀਂਤਾਂ ਜੇ (min_ਸਮਰੱਥਾ <= 2500) {
        ਵਾਪਸ 5 * min_ਸਮਰੱਥਾ;
    } ਨਹੀਂਤਾਂ {
        int32_t new_ਸਮਰੱਥਾ = 2 * min_ਸਮਰੱਥਾ;
        ਜੇ (new_ਸਮਰੱਥਾ > MAX_ਲੰਮਾਈ) {
            new_ਸਮਰੱਥਾ = MAX_ਲੰਮਾਈ;
        }
        ਵਾਪਸ new_ਸਮਰੱਥਾ;
    }
}

ਤਾਰਕਿਕ Unicode_ਸੈੱਟ::ensure_ਸਮਰੱਥਾ(int32_t newLen) {
    ਜੇ (newLen > MAX_ਲੰਮਾਈ) {
        newLen = MAX_ਲੰਮਾਈ;
    }
    ਜੇ (newLen <= capacity) {
        ਵਾਪਸ ਸੱਚ;
    }
    int32_t new_ਸਮਰੱਥਾ = ਅਗਲਾ_ਸਮਰੱਥਾ(newLen);
    UChar32* ਆਰਜ਼ੀ = static_cast<UChar32*>(uprv_malloc(new_ਸਮਰੱਥਾ * ਆਕਾਰ(UChar32)));
    ਜੇ (ਆਰਜ਼ੀ == ਖਾਲੀ_ਸੂਚਕ) {
        ਸੈੱਟ_ਮੰਜ਼ਿਲ_Bogus(); // set the object to bogus state if an OOM failure occurred.
        ਵਾਪਸ ਝੂਠ;
    }
    // Copy only the actual contents.
    uprv_memcpy(ਆਰਜ਼ੀ, list, len * ਆਕਾਰ(UChar32));
    ਜੇ (list != ਢੇਰ_List) {
        uprv_free(list);
    }
    list = ਆਰਜ਼ੀ;
    capacity = new_ਸਮਰੱਥਾ;
    ਵਾਪਸ ਸੱਚ;
}

ਤਾਰਕਿਕ Unicode_ਸੈੱਟ::ensure_ਬਫਰ_ਸਮਰੱਥਾ(int32_t newLen) {
    ਜੇ (newLen > MAX_ਲੰਮਾਈ) {
        newLen = MAX_ਲੰਮਾਈ;
    }
    ਜੇ (newLen <= ਬਫਰ_ਸਮਰੱਥਾ) {
        ਵਾਪਸ ਸੱਚ;
    }
    int32_t new_ਸਮਰੱਥਾ = ਅਗਲਾ_ਸਮਰੱਥਾ(newLen);
    UChar32* ਆਰਜ਼ੀ = static_cast<UChar32*>(uprv_malloc(new_ਸਮਰੱਥਾ * ਆਕਾਰ(UChar32)));
    ਜੇ (ਆਰਜ਼ੀ == ਖਾਲੀ_ਸੂਚਕ) {
        ਸੈੱਟ_ਮੰਜ਼ਿਲ_Bogus();
        ਵਾਪਸ ਝੂਠ;
    }
    // The buffer has no contents to be copied.
    // It is always filled from scratch after this call.
    ਜੇ (ਬਫਰ != ਢੇਰ_List) {
        uprv_free(ਬਫਰ);
    }
    ਬਫਰ = ਆਰਜ਼ੀ;
    ਬਫਰ_ਸਮਰੱਥਾ = new_ਸਮਰੱਥਾ;
    ਵਾਪਸ ਸੱਚ;
}

/**
 * Swap list and buffer.
 */
ਖਾਲੀ Unicode_ਸੈੱਟ::ਅਦਲਾ_ਬਦਲੀ_Buffers() {
    // swap list and buffer
    UChar32* ਆਰਜ਼ੀ = list;
    list = ਬਫਰ;
    ਬਫਰ = ਆਰਜ਼ੀ;

    int32_t c = capacity;
    capacity = ਬਫਰ_ਸਮਰੱਥਾ;
    ਬਫਰ_ਸਮਰੱਥਾ = c;
}

ਖਾਲੀ Unicode_ਸੈੱਟ::ਸੈੱਟ_ਮੰਜ਼ਿਲ_Bogus() {
    clear(); // Remove everything in the set.
    fFlags = kIsBogus;
}

//----------------------------------------------------------------
// Implementation: Fundamental operators
//----------------------------------------------------------------

ਸਥਿਰ ਸਤਰਅੰਦਰ UChar32 max(UChar32 a, UChar32 b) {
    ਵਾਪਸ (a > b) ? a : b;
}

// polarity = 0, 3 is normal: x xor y
// polarity = 1, 2: x xor ~y == x === y

ਖਾਲੀ Unicode_ਸੈੱਟ::exclusiveOr(ਅਚਲ UChar32* other, int32_t otherLen, int8_t polarity) {
    ਜੇ (isFrozen() || isBogus()) {
        ਵਾਪਸ;
    }
    ਜੇ (!ensure_ਬਫਰ_ਸਮਰੱਥਾ(len + otherLen)) {
        ਵਾਪਸ;
    }

    int32_t i = 0, j = 0, k = 0;
    UChar32 a = list[i++];
    UChar32 b;
    ਜੇ (polarity == 1 || polarity == 2) {
        b = UNICODESET_ਨੀਵਾਂ;
        ਜੇ (other[j] == UNICODESET_ਨੀਵਾਂ) { // skip base if already LOW
            ++j;
            b = other[j];
        }
    } ਨਹੀਂਤਾਂ {
        b = other[j++];
    }
    // simplest of all the routines
    // sort the values, discarding identicals!
    ਲਈ (;;) {
        ਜੇ (a < b) {
            ਬਫਰ[k++] = a;
            a = list[i++];
        } ਨਹੀਂਤਾਂ ਜੇ (b < a) {
            ਬਫਰ[k++] = b;
            b = other[j++];
        } ਨਹੀਂਤਾਂ ਜੇ (a != UNICODESET_ਉੱਚਾ) { // at this point, a == b
            // discard both values!
            a = list[i++];
            b = other[j++];
        } ਨਹੀਂਤਾਂ { // DONE!
            ਬਫਰ[k++] = UNICODESET_ਉੱਚਾ;
            len = k;
            ਰੋਕੋ;
        }
    }
    ਅਦਲਾ_ਬਦਲੀ_Buffers();
    release_ਨਮੂਨਾ();
}

// polarity = 0 is normal: x union y
// polarity = 2: x union ~y
// polarity = 1: ~x union y
// polarity = 3: ~x union ~y

ਖਾਲੀ Unicode_ਸੈੱਟ::ਜੋੜੋ(ਅਚਲ UChar32* other, int32_t otherLen, int8_t polarity) {
    ਜੇ (isFrozen() || isBogus() || other==ਖਾਲੀ_ਸੂਚਕ) {
        ਵਾਪਸ;
    }
    ਜੇ (!ensure_ਬਫਰ_ਸਮਰੱਥਾ(len + otherLen)) {
        ਵਾਪਸ;
    }

    int32_t i = 0, j = 0, k = 0;
    UChar32 a = list[i++];
    UChar32 b = other[j++];
    // change from xor is that we have to check overlapping pairs
    // polarity bit 1 means a is second, bit 2 means b is.
    ਲਈ (;;) {
        ਚੋਣ (polarity) {
          ਹਾਲਤ 0: // both first; take lower if unequal
            ਜੇ (a < b) { // take a
                // Back up over overlapping ranges in buffer[]
                ਜੇ (k > 0 && a <= ਬਫਰ[k-1]) {
                    // Pick latter end value in buffer[] vs. list[]
                    a = max(list[i], ਬਫਰ[--k]);
                } ਨਹੀਂਤਾਂ {
                    // No overlap
                    ਬਫਰ[k++] = a;
                    a = list[i];
                }
                i++; // Common if/else code factored out
                polarity ^= 1;
            } ਨਹੀਂਤਾਂ ਜੇ (b < a) { // take b
                ਜੇ (k > 0 && b <= ਬਫਰ[k-1]) {
                    b = max(other[j], ਬਫਰ[--k]);
                } ਨਹੀਂਤਾਂ {
                    ਬਫਰ[k++] = b;
                    b = other[j];
                }
                j++;
                polarity ^= 2;
            } ਨਹੀਂਤਾਂ { // a == b, take a, drop b
                ਜੇ (a == UNICODESET_ਉੱਚਾ) ਜਾਓ loop_ਸਿਰਾ;
                // This is symmetrical; it doesn't matter if
                // we backtrack with a or b. - liu
                ਜੇ (k > 0 && a <= ਬਫਰ[k-1]) {
                    a = max(list[i], ਬਫਰ[--k]);
                } ਨਹੀਂਤਾਂ {
                    // No overlap
                    ਬਫਰ[k++] = a;
                    a = list[i];
                }
                i++;
                polarity ^= 1;
                b = other[j++];
                polarity ^= 2;
            }
            ਰੋਕੋ;
          ਹਾਲਤ 3: // both second; take higher if unequal, and drop other
            ਜੇ (b <= a) { // take a
                ਜੇ (a == UNICODESET_ਉੱਚਾ) ਜਾਓ loop_ਸਿਰਾ;
                ਬਫਰ[k++] = a;
            } ਨਹੀਂਤਾਂ { // take b
                ਜੇ (b == UNICODESET_ਉੱਚਾ) ਜਾਓ loop_ਸਿਰਾ;
                ਬਫਰ[k++] = b;
            }
            a = list[i++];
            polarity ^= 1;   // factored common code
            b = other[j++];
            polarity ^= 2;
            ਰੋਕੋ;
          ਹਾਲਤ 1: // a second, b first; if b < a, overlap
            ਜੇ (a < b) { // no overlap, take a
                ਬਫਰ[k++] = a; a = list[i++]; polarity ^= 1;
            } ਨਹੀਂਤਾਂ ਜੇ (b < a) { // OVERLAP, drop b
                b = other[j++];
                polarity ^= 2;
            } ਨਹੀਂਤਾਂ { // a == b, drop both!
                ਜੇ (a == UNICODESET_ਉੱਚਾ) ਜਾਓ loop_ਸਿਰਾ;
                a = list[i++];
                polarity ^= 1;
                b = other[j++];
                polarity ^= 2;
            }
            ਰੋਕੋ;
          ਹਾਲਤ 2: // a first, b second; if a < b, overlap
            ਜੇ (b < a) { // no overlap, take b
                ਬਫਰ[k++] = b;
                b = other[j++];
                polarity ^= 2;
            } ਨਹੀਂਤਾਂ  ਜੇ (a < b) { // OVERLAP, drop a
                a = list[i++];
                polarity ^= 1;
            } ਨਹੀਂਤਾਂ { // a == b, drop both!
                ਜੇ (a == UNICODESET_ਉੱਚਾ) ਜਾਓ loop_ਸਿਰਾ;
                a = list[i++];
                polarity ^= 1;
                b = other[j++];
                polarity ^= 2;
            }
            ਰੋਕੋ;
        }
    }
 loop_ਸਿਰਾ:
    ਬਫਰ[k++] = UNICODESET_ਉੱਚਾ;    // terminate
    len = k;
    ਅਦਲਾ_ਬਦਲੀ_Buffers();
    release_ਨਮੂਨਾ();
}

// polarity = 0 is normal: x intersect y
// polarity = 2: x intersect ~y == set-minus
// polarity = 1: ~x intersect y
// polarity = 3: ~x intersect ~y

ਖਾਲੀ Unicode_ਸੈੱਟ::retain(ਅਚਲ UChar32* other, int32_t otherLen, int8_t polarity) {
    ਜੇ (isFrozen() || isBogus()) {
        ਵਾਪਸ;
    }
    ਜੇ (!ensure_ਬਫਰ_ਸਮਰੱਥਾ(len + otherLen)) {
        ਵਾਪਸ;
    }

    int32_t i = 0, j = 0, k = 0;
    UChar32 a = list[i++];
    UChar32 b = other[j++];
    // change from xor is that we have to check overlapping pairs
    // polarity bit 1 means a is second, bit 2 means b is.
    ਲਈ (;;) {
        ਚੋਣ (polarity) {
          ਹਾਲਤ 0: // both first; drop the smaller
            ਜੇ (a < b) { // drop a
                a = list[i++];
                polarity ^= 1;
            } ਨਹੀਂਤਾਂ ਜੇ (b < a) { // drop b
                b = other[j++];
                polarity ^= 2;
            } ਨਹੀਂਤਾਂ { // a == b, take one, drop other
                ਜੇ (a == UNICODESET_ਉੱਚਾ) ਜਾਓ loop_ਸਿਰਾ;
                ਬਫਰ[k++] = a;
                a = list[i++];
                polarity ^= 1;
                b = other[j++];
                polarity ^= 2;
            }
            ਰੋਕੋ;
          ਹਾਲਤ 3: // both second; take lower if unequal
            ਜੇ (a < b) { // take a
                ਬਫਰ[k++] = a;
                a = list[i++];
                polarity ^= 1;
            } ਨਹੀਂਤਾਂ ਜੇ (b < a) { // take b
                ਬਫਰ[k++] = b;
                b = other[j++];
                polarity ^= 2;
            } ਨਹੀਂਤਾਂ { // a == b, take one, drop other
                ਜੇ (a == UNICODESET_ਉੱਚਾ) ਜਾਓ loop_ਸਿਰਾ;
                ਬਫਰ[k++] = a;
                a = list[i++];
                polarity ^= 1;
                b = other[j++];
                polarity ^= 2;
            }
            ਰੋਕੋ;
          ਹਾਲਤ 1: // a second, b first;
            ਜੇ (a < b) { // NO OVERLAP, drop a
                a = list[i++];
                polarity ^= 1;
            } ਨਹੀਂਤਾਂ ਜੇ (b < a) { // OVERLAP, take b
                ਬਫਰ[k++] = b;
                b = other[j++];
                polarity ^= 2;
            } ਨਹੀਂਤਾਂ { // a == b, drop both!
                ਜੇ (a == UNICODESET_ਉੱਚਾ) ਜਾਓ loop_ਸਿਰਾ;
                a = list[i++];
                polarity ^= 1;
                b = other[j++];
                polarity ^= 2;
            }
            ਰੋਕੋ;
          ਹਾਲਤ 2: // a first, b second; if a < b, overlap
            ਜੇ (b < a) { // no overlap, drop b
                b = other[j++];
                polarity ^= 2;
            } ਨਹੀਂਤਾਂ  ਜੇ (a < b) { // OVERLAP, take a
                ਬਫਰ[k++] = a;
                a = list[i++];
                polarity ^= 1;
            } ਨਹੀਂਤਾਂ { // a == b, drop both!
                ਜੇ (a == UNICODESET_ਉੱਚਾ) ਜਾਓ loop_ਸਿਰਾ;
                a = list[i++];
                polarity ^= 1;
                b = other[j++];
                polarity ^= 2;
            }
            ਰੋਕੋ;
        }
    }
 loop_ਸਿਰਾ:
    ਬਫਰ[k++] = UNICODESET_ਉੱਚਾ;    // terminate
    len = k;
    ਅਦਲਾ_ਬਦਲੀ_Buffers();
    release_ਨਮੂਨਾ();
}

/**
 * Append the <code>toPattern()</code> representation of a
 * string to the given <code>StringBuffer</code>.
 */
ਖਾਲੀ Unicode_ਸੈੱਟ::append_ਮੰਜ਼ਿਲ_Pat(UnicodeString& buf, ਅਚਲ UnicodeString& s, UBool escapeUnprintable) {
    UChar32 cp;
    ਲਈ (int32_t i = 0; i < s.length(); i += U16_ਲੰਮਾਈ(cp)) {
        append_ਮੰਜ਼ਿਲ_Pat(buf, cp = s.char32At(i), escapeUnprintable);
    }
}

/**
 * Append the <code>toPattern()</code> representation of a
 * character to the given <code>StringBuffer</code>.
 */
ਖਾਲੀ Unicode_ਸੈੱਟ::append_ਮੰਜ਼ਿਲ_Pat(UnicodeString& buf, UChar32 c, UBool escapeUnprintable) {
    ਜੇ (escapeUnprintable ? ICU_Utility::isUnprintable(c) : ICU_Utility::shouldAlwaysBeEscaped(c)) {
        // Use hex escape notation (\uxxxx or \Uxxxxxxxx) for anything
        // unprintable
        ICU_Utility::escape(buf, c);
        ਵਾਪਸ;
    }
    // Okay to let ':' pass through
    ਚੋਣ (c) {
    ਹਾਲਤ u'[':
    ਹਾਲਤ u']':
    ਹਾਲਤ u'-':
    ਹਾਲਤ u'^':
    ਹਾਲਤ u'&':
    ਹਾਲਤ u'\\':
    ਹਾਲਤ u'{':
    ਹਾਲਤ u'}':
    ਹਾਲਤ u':':
    ਹਾਲਤ ਚਿੰਨ੍ਹ_ਸਾਰਣੀ::ਚਿੰਨ੍ਹ_REF:
        buf.append(u'\\');
        ਰੋਕੋ;
    ਮੂਲ:
        // Escape whitespace
        ਜੇ (ਨਮੂਨਾ_Props::isWhiteSpace(c)) {
            buf.append(u'\\');
        }
        ਰੋਕੋ;
    }
    buf.append(c);
}

ਖਾਲੀ Unicode_ਸੈੱਟ::append_ਮੰਜ਼ਿਲ_Pat(UnicodeString &ਨਤੀਜਾ, UChar32 ਸ਼ੁਰੂ, UChar32 ਸਿਰਾ,
                              UBool escapeUnprintable) {
    append_ਮੰਜ਼ਿਲ_Pat(ਨਤੀਜਾ, ਸ਼ੁਰੂ, escapeUnprintable);
    ਜੇ (ਸ਼ੁਰੂ != ਸਿਰਾ) {
        ਜੇ ((ਸ਼ੁਰੂ+1) != ਸਿਰਾ ||
                // Avoid writing what looks like a lead+trail surrogate pair.
                ਸ਼ੁਰੂ == 0xdbff) {
            ਨਤੀਜਾ.append(u'-');
        }
        append_ਮੰਜ਼ਿਲ_Pat(ਨਤੀਜਾ, ਸਿਰਾ, escapeUnprintable);
    }
}

/**
 * Append a string representation of this set to result.  This will be
 * a cleaned version of the string passed to applyPattern(), if there
 * is one.  Otherwise it will be generated.
 */
UnicodeString& Unicode_ਸੈੱਟ::ਮੰਜ਼ਿਲ_ਨਮੂਨਾ_2(UnicodeString& ਨਤੀਜਾ U_LIFETIME_BOUND,
                                      UBool escapeUnprintable) ਅਚਲ
{
    ਜੇ (pat != ਖਾਲੀ_ਸੂਚਕ) {
        int32_t i;
        int32_t backslash_ਗਿਣਤੀ = 0;
        ਲਈ (i=0; i<patLen; ) {
            UChar32 c;
            U16_ਅਗਲਾ(pat, i, patLen, c);
            ਜੇ (escapeUnprintable ?
                    ICU_Utility::isUnprintable(c) : ICU_Utility::shouldAlwaysBeEscaped(c)) {
                // If the unprintable character is preceded by an odd
                // number of backslashes, then it has been escaped.
                // Before unescaping it, we delete the final
                // backslash.
                ਜੇ ((backslash_ਗਿਣਤੀ % 2) == 1) {
                    ਨਤੀਜਾ.truncate(ਨਤੀਜਾ.length() - 1);
                }
                ICU_Utility::escape(ਨਤੀਜਾ, c);
                backslash_ਗਿਣਤੀ = 0;
            } ਨਹੀਂਤਾਂ {
                ਨਤੀਜਾ.append(c);
                ਜੇ (c == u'\\') {
                    ++backslash_ਗਿਣਤੀ;
                } ਨਹੀਂਤਾਂ {
                    backslash_ਗਿਣਤੀ = 0;
                }
            }
        }
        ਵਾਪਸ ਨਤੀਜਾ;
    }

    ਵਾਪਸ generate_ਨਮੂਨਾ(ਨਤੀਜਾ, escapeUnprintable);
}

/**
 * Returns a string representation of this set.  If the result of
 * calling this function is passed to a UnicodeSet constructor, it
 * will produce another set that is equal to this one.
 */
UnicodeString& Unicode_ਸੈੱਟ::ਮੰਜ਼ਿਲ_ਨਮੂਨਾ(UnicodeString& ਨਤੀਜਾ U_LIFETIME_BOUND,
                                     UBool escapeUnprintable) ਅਚਲ
{
    ਨਤੀਜਾ.truncate(0);
    ਵਾਪਸ ਮੰਜ਼ਿਲ_ਨਮੂਨਾ_2(ਨਤੀਜਾ, escapeUnprintable);
}

/**
 * Generate and append a string representation of this set to result.
 * This does not use this.pat, the cleaned up copy of the string
 * passed to applyPattern().
 */
UnicodeString& Unicode_ਸੈੱਟ::generate_ਨਮੂਨਾ(UnicodeString& ਨਤੀਜਾ U_LIFETIME_BOUND,
                                            UBool escapeUnprintable) ਅਚਲ
{
    ਨਤੀਜਾ.append(u'[');

    int32_t i = 0;
    int32_t ਹੱਦਬੰਦੀ = len & ~1;  // = 2 * getRangeCount()

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
    ਜੇ (len >= 4 && list[0] == 0 && ਹੱਦਬੰਦੀ == len && !has_ਲੜੀਆਂ()) {
        // Emit the inverse
        ਨਤੀਜਾ.append(u'^');
        // Offsetting the inversion list index by one lets us
        // iterate over the ranges of the set complement.
        i = 1;
        --ਹੱਦਬੰਦੀ;
    }

    // Emit the ranges as pairs.
    ਜਦੋਂ (i < ਹੱਦਬੰਦੀ) {
        UChar32 ਸ਼ੁਰੂ = list[i];  // getRangeStart()
        UChar32 ਸਿਰਾ = list[i + 1] - 1;  // getRangeEnd() = range limit minus one
        ਜੇ (!(0xd800 <= ਸਿਰਾ && ਸਿਰਾ <= 0xdbff)) {
            append_ਮੰਜ਼ਿਲ_Pat(ਨਤੀਜਾ, ਸ਼ੁਰੂ, ਸਿਰਾ, escapeUnprintable);
            i += 2;
        } ਨਹੀਂਤਾਂ {
            // The range ends with a lead surrogate.
            // Avoid writing what looks like a lead+trail surrogate pair.
            // 1. Postpone ranges that start with a lead surrogate code point.
            int32_t ਪਹਿਲਾ_Lead = i;
            ਜਦੋਂ ((i += 2) < ਹੱਦਬੰਦੀ && list[i] <= 0xdbff) {}
            int32_t ਪਹਿਲਾ_After_Lead = i;
            // 2. Write following ranges that start with a trail surrogate code point.
            ਜਦੋਂ (i < ਹੱਦਬੰਦੀ && (ਸ਼ੁਰੂ = list[i]) <= 0xdfff) {
                append_ਮੰਜ਼ਿਲ_Pat(ਨਤੀਜਾ, ਸ਼ੁਰੂ, list[i + 1] - 1, escapeUnprintable);
                i += 2;
            }
            // 3. Now write the postponed ranges.
            ਲਈ (ਪੂਰਨ j = ਪਹਿਲਾ_Lead; j < ਪਹਿਲਾ_After_Lead; j += 2) {
                append_ਮੰਜ਼ਿਲ_Pat(ਨਤੀਜਾ, list[j], list[j + 1] - 1, escapeUnprintable);
            }
        }
    }

    ਜੇ (ਲੜੀਆਂ_2 != ਖਾਲੀ_ਸੂਚਕ) {
        ਲਈ (int32_t i = 0; i<ਲੜੀਆਂ_2->size(); ++i) {
            ਨਤੀਜਾ.append(u'{');
            append_ਮੰਜ਼ਿਲ_Pat(ਨਤੀਜਾ,
                         *static_cast<ਅਚਲ UnicodeString*>(ਲੜੀਆਂ_2->elementAt(i)),
                         escapeUnprintable);
            ਨਤੀਜਾ.append(u'}');
        }
    }
    ਵਾਪਸ ਨਤੀਜਾ.append(u']');
}

/**
* Release existing cached pattern
*/
ਖਾਲੀ Unicode_ਸੈੱਟ::release_ਨਮੂਨਾ() {
    ਜੇ (pat) {
        uprv_free(pat);
        pat = ਖਾਲੀ_ਸੂਚਕ;
        patLen = 0;
    }
}

/**
* Set the new pattern to cache.
*/
ਖਾਲੀ Unicode_ਸੈੱਟ::ਸੈੱਟ_ਨਮੂਨਾ(ਅਚਲ char16_t *newPat, int32_t newPatLen) {
    release_ਨਮੂਨਾ();
    pat = static_cast<char16_t*>(uprv_malloc((newPatLen + 1) * ਆਕਾਰ(char16_t)));
    ਜੇ (pat) {
        patLen = newPatLen;
        u_memcpy(pat, newPat, patLen);
        pat[patLen] = 0;
    }
    // else we don't care if malloc failed. This was just a nice cache.
    // We can regenerate an equivalent pattern later when requested.
}

Unicode_ਸੈੱਟ* Unicode_ਸੈੱਟ::freeze() U_LIFETIME_BOUND {
    ਜੇ(!isFrozen() && !isBogus()) {
        compact();

        // Optimize contains() and span() and similar functions.
        ਜੇ (has_ਲੜੀਆਂ()) {
            string_ਵਿਸਤਾਰ = ਨਵਾਂ Unicode_ਸੈੱਟ_String_ਵਿਸਤਾਰ(*ਇਹ, *ਲੜੀਆਂ_2, Unicode_ਸੈੱਟ_String_ਵਿਸਤਾਰ::ALL);
            ਜੇ (string_ਵਿਸਤਾਰ == ਖਾਲੀ_ਸੂਚਕ) {
                ਸੈੱਟ_ਮੰਜ਼ਿਲ_Bogus();
                ਵਾਪਸ ਇਹ;
            } ਨਹੀਂਤਾਂ ਜੇ (!string_ਵਿਸਤਾਰ->needs_String_ਵਿਸਤਾਰ_UTF16()) {
                // All strings are irrelevant for span() etc. because
                // all of each string's code points are contained in this set.
                // Do not check needsStringSpanUTF8() because UTF-8 has at most as
                // many relevant strings as UTF-16.
                // (Thus needsStringSpanUTF8() implies needsStringSpanUTF16().)
                ਮਿਟਾਓ string_ਵਿਸਤਾਰ;
                string_ਵਿਸਤਾਰ = ਖਾਲੀ_ਸੂਚਕ;
            }
        }
        ਜੇ (string_ਵਿਸਤਾਰ == ਖਾਲੀ_ਸੂਚਕ) {
            // No span-relevant strings: Optimize for code point spans.
            bmp_ਸੈੱਟ=ਨਵਾਂ BMPSet(list, len);
            ਜੇ (bmp_ਸੈੱਟ == ਖਾਲੀ_ਸੂਚਕ) { // Check for memory allocation error.
                ਸੈੱਟ_ਮੰਜ਼ਿਲ_Bogus();
            }
        }
    }
    ਵਾਪਸ ਇਹ;
}

int32_t Unicode_ਸੈੱਟ::ਵਿਸਤਾਰ(ਅਚਲ char16_t *s, int32_t length, USet_ਵਿਸਤਾਰ_Condition ਵਿਸਤਾਰ_Condition) ਅਚਲ {
    ਜੇ(length>0 && bmp_ਸੈੱਟ!=ਖਾਲੀ_ਸੂਚਕ) {
        ਵਾਪਸ static_cast<int32_t>(bmp_ਸੈੱਟ->ਵਿਸਤਾਰ(s, s + length, ਵਿਸਤਾਰ_Condition) - s);
    }
    ਜੇ(length<0) {
        length=u_strlen(s);
    }
    ਜੇ(length==0) {
        ਵਾਪਸ 0;
    }
    ਜੇ(string_ਵਿਸਤਾਰ!=ਖਾਲੀ_ਸੂਚਕ) {
        ਵਾਪਸ string_ਵਿਸਤਾਰ->ਵਿਸਤਾਰ(s, length, ਵਿਸਤਾਰ_Condition);
    } ਨਹੀਂਤਾਂ ਜੇ(has_ਲੜੀਆਂ()) {
        uint32_t which= ਵਿਸਤਾਰ_Condition==USET_ਵਿਸਤਾਰ_NOT_CONTAINED ?
                            Unicode_ਸੈੱਟ_String_ਵਿਸਤਾਰ::FWD_UTF16_NOT_CONTAINED :
                            Unicode_ਸੈੱਟ_String_ਵਿਸਤਾਰ::FWD_UTF16_CONTAINED;
        Unicode_ਸੈੱਟ_String_ਵਿਸਤਾਰ str_ਵਿਸਤਾਰ(*ਇਹ, *ਲੜੀਆਂ_2, which);
        ਜੇ(str_ਵਿਸਤਾਰ.needs_String_ਵਿਸਤਾਰ_UTF16()) {
            ਵਾਪਸ str_ਵਿਸਤਾਰ.ਵਿਸਤਾਰ(s, length, ਵਿਸਤਾਰ_Condition);
        }
    }

    ਜੇ(ਵਿਸਤਾਰ_Condition!=USET_ਵਿਸਤਾਰ_NOT_CONTAINED) {
        ਵਿਸਤਾਰ_Condition=USET_ਵਿਸਤਾਰ_CONTAINED;  // Pin to 0/1 values.
    }

    UChar32 c;
    int32_t ਸ਼ੁਰੂ=0, prev=0;
    ਕਰੋ {
        U16_ਅਗਲਾ(s, ਸ਼ੁਰੂ, length, c);
        ਜੇ(ਵਿਸਤਾਰ_Condition!=ਸ਼ਾਮਲ_ਹੈ(c)) {
            ਰੋਕੋ;
        }
    } ਜਦੋਂ((prev=ਸ਼ੁਰੂ)<length);
    ਵਾਪਸ prev;
}

int32_t Unicode_ਸੈੱਟ::ਵਿਸਤਾਰ_ਪਿੱਠ(ਅਚਲ char16_t *s, int32_t length, USet_ਵਿਸਤਾਰ_Condition ਵਿਸਤਾਰ_Condition) ਅਚਲ {
    ਜੇ(length>0 && bmp_ਸੈੱਟ!=ਖਾਲੀ_ਸੂਚਕ) {
        ਵਾਪਸ static_cast<int32_t>(bmp_ਸੈੱਟ->ਵਿਸਤਾਰ_ਪਿੱਠ(s, s + length, ਵਿਸਤਾਰ_Condition) - s);
    }
    ਜੇ(length<0) {
        length=u_strlen(s);
    }
    ਜੇ(length==0) {
        ਵਾਪਸ 0;
    }
    ਜੇ(string_ਵਿਸਤਾਰ!=ਖਾਲੀ_ਸੂਚਕ) {
        ਵਾਪਸ string_ਵਿਸਤਾਰ->ਵਿਸਤਾਰ_ਪਿੱਠ(s, length, ਵਿਸਤਾਰ_Condition);
    } ਨਹੀਂਤਾਂ ਜੇ(has_ਲੜੀਆਂ()) {
        uint32_t which= ਵਿਸਤਾਰ_Condition==USET_ਵਿਸਤਾਰ_NOT_CONTAINED ?
                            Unicode_ਸੈੱਟ_String_ਵਿਸਤਾਰ::ਪਿੱਠ_UTF16_NOT_CONTAINED :
                            Unicode_ਸੈੱਟ_String_ਵਿਸਤਾਰ::ਪਿੱਠ_UTF16_CONTAINED;
        Unicode_ਸੈੱਟ_String_ਵਿਸਤਾਰ str_ਵਿਸਤਾਰ(*ਇਹ, *ਲੜੀਆਂ_2, which);
        ਜੇ(str_ਵਿਸਤਾਰ.needs_String_ਵਿਸਤਾਰ_UTF16()) {
            ਵਾਪਸ str_ਵਿਸਤਾਰ.ਵਿਸਤਾਰ_ਪਿੱਠ(s, length, ਵਿਸਤਾਰ_Condition);
        }
    }

    ਜੇ(ਵਿਸਤਾਰ_Condition!=USET_ਵਿਸਤਾਰ_NOT_CONTAINED) {
        ਵਿਸਤਾਰ_Condition=USET_ਵਿਸਤਾਰ_CONTAINED;  // Pin to 0/1 values.
    }

    UChar32 c;
    int32_t prev=length;
    ਕਰੋ {
        U16_PREV(s, 0, length, c);
        ਜੇ(ਵਿਸਤਾਰ_Condition!=ਸ਼ਾਮਲ_ਹੈ(c)) {
            ਰੋਕੋ;
        }
    } ਜਦੋਂ((prev=length)>0);
    ਵਾਪਸ prev;
}

int32_t Unicode_ਸੈੱਟ::ਵਿਸਤਾਰ_UTF8(ਅਚਲ ਅੱਖਰ *s, int32_t length, USet_ਵਿਸਤਾਰ_Condition ਵਿਸਤਾਰ_Condition) ਅਚਲ {
    ਜੇ(length>0 && bmp_ਸੈੱਟ!=ਖਾਲੀ_ਸੂਚਕ) {
        ਅਚਲ uint8_t* s0 = reinterpret_cast<ਅਚਲ uint8_t*>(s);
        ਵਾਪਸ static_cast<int32_t>(bmp_ਸੈੱਟ->ਵਿਸਤਾਰ_UTF8(s0, length, ਵਿਸਤਾਰ_Condition) - s0);
    }
    ਜੇ(length<0) {
        length = static_cast<int32_t>(uprv_strlen(s));
    }
    ਜੇ(length==0) {
        ਵਾਪਸ 0;
    }
    ਜੇ(string_ਵਿਸਤਾਰ!=ਖਾਲੀ_ਸੂਚਕ) {
        ਵਾਪਸ string_ਵਿਸਤਾਰ->ਵਿਸਤਾਰ_UTF8(reinterpret_cast<ਅਚਲ uint8_t*>(s), length, ਵਿਸਤਾਰ_Condition);
    } ਨਹੀਂਤਾਂ ਜੇ(has_ਲੜੀਆਂ()) {
        uint32_t which= ਵਿਸਤਾਰ_Condition==USET_ਵਿਸਤਾਰ_NOT_CONTAINED ?
                            Unicode_ਸੈੱਟ_String_ਵਿਸਤਾਰ::FWD_UTF8_NOT_CONTAINED :
                            Unicode_ਸੈੱਟ_String_ਵਿਸਤਾਰ::FWD_UTF8_CONTAINED;
        Unicode_ਸੈੱਟ_String_ਵਿਸਤਾਰ str_ਵਿਸਤਾਰ(*ਇਹ, *ਲੜੀਆਂ_2, which);
        ਜੇ(str_ਵਿਸਤਾਰ.needs_String_ਵਿਸਤਾਰ_UTF8()) {
            ਵਾਪਸ str_ਵਿਸਤਾਰ.ਵਿਸਤਾਰ_UTF8(reinterpret_cast<ਅਚਲ uint8_t*>(s), length, ਵਿਸਤਾਰ_Condition);
        }
    }

    ਜੇ(ਵਿਸਤਾਰ_Condition!=USET_ਵਿਸਤਾਰ_NOT_CONTAINED) {
        ਵਿਸਤਾਰ_Condition=USET_ਵਿਸਤਾਰ_CONTAINED;  // Pin to 0/1 values.
    }

    UChar32 c;
    int32_t ਸ਼ੁਰੂ=0, prev=0;
    ਕਰੋ {
        U8_ਅਗਲਾ_OR_FFFD(s, ਸ਼ੁਰੂ, length, c);
        ਜੇ(ਵਿਸਤਾਰ_Condition!=ਸ਼ਾਮਲ_ਹੈ(c)) {
            ਰੋਕੋ;
        }
    } ਜਦੋਂ((prev=ਸ਼ੁਰੂ)<length);
    ਵਾਪਸ prev;
}

int32_t Unicode_ਸੈੱਟ::ਵਿਸਤਾਰ_ਪਿੱਠ_UTF8(ਅਚਲ ਅੱਖਰ *s, int32_t length, USet_ਵਿਸਤਾਰ_Condition ਵਿਸਤਾਰ_Condition) ਅਚਲ {
    ਜੇ(length>0 && bmp_ਸੈੱਟ!=ਖਾਲੀ_ਸੂਚਕ) {
        ਅਚਲ uint8_t* s0 = reinterpret_cast<ਅਚਲ uint8_t*>(s);
        ਵਾਪਸ bmp_ਸੈੱਟ->ਵਿਸਤਾਰ_ਪਿੱਠ_UTF8(s0, length, ਵਿਸਤਾਰ_Condition);
    }
    ਜੇ(length<0) {
        length = static_cast<int32_t>(uprv_strlen(s));
    }
    ਜੇ(length==0) {
        ਵਾਪਸ 0;
    }
    ਜੇ(string_ਵਿਸਤਾਰ!=ਖਾਲੀ_ਸੂਚਕ) {
        ਵਾਪਸ string_ਵਿਸਤਾਰ->ਵਿਸਤਾਰ_ਪਿੱਠ_UTF8(reinterpret_cast<ਅਚਲ uint8_t*>(s), length, ਵਿਸਤਾਰ_Condition);
    } ਨਹੀਂਤਾਂ ਜੇ(has_ਲੜੀਆਂ()) {
        uint32_t which= ਵਿਸਤਾਰ_Condition==USET_ਵਿਸਤਾਰ_NOT_CONTAINED ?
                            Unicode_ਸੈੱਟ_String_ਵਿਸਤਾਰ::ਪਿੱਠ_UTF8_NOT_CONTAINED :
                            Unicode_ਸੈੱਟ_String_ਵਿਸਤਾਰ::ਪਿੱਠ_UTF8_CONTAINED;
        Unicode_ਸੈੱਟ_String_ਵਿਸਤਾਰ str_ਵਿਸਤਾਰ(*ਇਹ, *ਲੜੀਆਂ_2, which);
        ਜੇ(str_ਵਿਸਤਾਰ.needs_String_ਵਿਸਤਾਰ_UTF8()) {
            ਵਾਪਸ str_ਵਿਸਤਾਰ.ਵਿਸਤਾਰ_ਪਿੱਠ_UTF8(reinterpret_cast<ਅਚਲ uint8_t*>(s), length, ਵਿਸਤਾਰ_Condition);
        }
    }

    ਜੇ(ਵਿਸਤਾਰ_Condition!=USET_ਵਿਸਤਾਰ_NOT_CONTAINED) {
        ਵਿਸਤਾਰ_Condition=USET_ਵਿਸਤਾਰ_CONTAINED;  // Pin to 0/1 values.
    }

    UChar32 c;
    int32_t prev=length;
    ਕਰੋ {
        U8_PREV_OR_FFFD(s, 0, length, c);
        ਜੇ(ਵਿਸਤਾਰ_Condition!=ਸ਼ਾਮਲ_ਹੈ(c)) {
            ਰੋਕੋ;
        }
    } ਜਦੋਂ((prev=length)>0);
    ਵਾਪਸ prev;
}

U_NAMESPACE_ਸਿਰਾ
