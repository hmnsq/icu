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

#ಅನ್ತರ್ಭಾವ "unicode/utypes.h"
#ಅನ್ತರ್ಭಾವ "unicode/parsepos.h"
#ಅನ್ತರ್ಭಾವ "unicode/symtable.h"
#ಅನ್ತರ್ಭಾವ "unicode/uniset.h"
#ಅನ್ತರ್ಭಾವ "unicode/ustring.h"
#ಅನ್ತರ್ಭಾವ "unicode/utf8.h"
#ಅನ್ತರ್ಭಾವ "unicode/utf16.h"
#ಅನ್ತರ್ಭಾವ "ruleiter.h"
#ಅನ್ತರ್ಭಾವ "cmemory.h"
#ಅನ್ತರ್ಭಾವ "cstring.h"
#ಅನ್ತರ್ಭಾವ "patternprops.h"
#ಅನ್ತರ್ಭಾವ "uelement.h"
#ಅನ್ತರ್ಭಾವ "util.h"
#ಅನ್ತರ್ಭಾವ "uvector.h"
#ಅನ್ತರ್ಭಾವ "charstr.h"
#ಅನ್ತರ್ಭಾವ "ustrfmt.h"
#ಅನ್ತರ್ಭಾವ "uassert.h"
#ಅನ್ತರ್ಭಾವ "bmpset.h"
#ಅನ್ತರ್ಭಾವ "unisetspan.h"

// HIGH_VALUE > all valid values. 110000 for codepoints
#ವ್ಯಾಖ್ಯಾನಿಸು UNICODESET_ಮೇಲಿನ 0x0110000

// LOW <= all valid values. ZERO for codepoints
#ವ್ಯಾಖ್ಯಾನಿಸು UNICODESET_ಕೆಳಗಿನ 0x000000

/** Max list [0, 1, 2, ..., max code point, HIGH] */
constexpr int32_t MAX_ಉದ್ದಳತೆ = UNICODESET_ಮೇಲಿನ + 1;

U_NAMESPACE_BEGIN

ಚಿಹ್ನೆ_ಕೋಷ್ಟಕ::~ಚಿಹ್ನೆ_ಕೋಷ್ಟಕ() {}

UOBJECT_DEFINE_RTTI_IMPLEMENTATION(Unicode_ಗಣ)

/**
 * Modify the given UChar32 variable so that it is in range, by
 * pinning values < UNICODESET_LOW to UNICODESET_LOW, and
 * pinning values > UNICODESET_HIGH-1 to UNICODESET_HIGH-1.
 * It modifies its argument in-place and also returns it.
 */
ಸ್ಥಿರ ಸಾಲೊಳಗೆ UChar32 pin_ಸಂಕೇತ_ಬಿಂದು(UChar32& c) {
    ಆದರೆ (c < UNICODESET_ಕೆಳಗಿನ) {
        c = UNICODESET_ಕೆಳಗಿನ;
    } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (c > (UNICODESET_ಮೇಲಿನ-1)) {
        c = (UNICODESET_ಮೇಲಿನ-1);
    }
    ಹಿಂತಿರುಗಿ c;
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

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ DEBUG_MEM
#ಅನ್ತರ್ಭಾವ <stdio.h>
ಸ್ಥಿರ int32_t dbg_ಎಣಿಕೆ = 0;

ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಶೂನ್ಯ _dbgct(Unicode_ಗಣ* ಗಣ) {
    UnicodeString str;
    ಗಣ->ಗಮ್ಯ_ನಮೂನೆ(str, ನಿಜ);
    ಅಕ್ಷರ buf[40];
    str.extract(0, 39, buf, "");
    ಬರೆ("DEBUG UnicodeSet: ct 0x%08X; %d %s\n", ಗಣ, ++dbg_ಎಣಿಕೆ, buf);
}

ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಶೂನ್ಯ _dbgdt(Unicode_ಗಣ* ಗಣ) {
    UnicodeString str;
    ಗಣ->ಗಮ್ಯ_ನಮೂನೆ(str, ನಿಜ);
    ಅಕ್ಷರ buf[40];
    str.extract(0, 39, buf, "");
    ಬರೆ("DEBUG UnicodeSet: dt 0x%08X; %d %s\n", ಗಣ, --dbg_ಎಣಿಕೆ, buf);
}

#ಇಲ್ಲದಿದ್ದರೆ

#ವ್ಯಾಖ್ಯಾನಿಸು _dbgct(ಗಣ)
#ವ್ಯಾಖ್ಯಾನಿಸು _dbgdt(ಗಣ)

#ಆದರೆ_ಅಂತ್ಯ

//----------------------------------------------------------------
// UnicodeString in UVector support
//----------------------------------------------------------------

ಸ್ಥಿರ ಶೂನ್ಯ U_CALLCONV cloneUnicodeString(UElement *dst, UElement *src) {
    dst->ಸೂಚಕ = ಹೊಸ UnicodeString(*static_cast<UnicodeString*>(src->ಸೂಚಕ));
}

ಸ್ಥಿರ int32_t U_CALLCONV compareUnicodeString(UElement t1, UElement t2) {
    ಸ್ಥಿರಾಂಕ UnicodeString& a = *static_cast<ಸ್ಥಿರಾಂಕ UnicodeString*>(t1.ಸೂಚಕ);
    ಸ್ಥಿರಾಂಕ UnicodeString& b = *static_cast<ಸ್ಥಿರಾಂಕ UnicodeString*>(t2.ಸೂಚಕ);
    ಹಿಂತಿರುಗಿ a.compare(b);
}

UBool Unicode_ಗಣ::has_ಅಕ್ಷರಸರಣಿಗಳು() ಸ್ಥಿರಾಂಕ {
    ಹಿಂತಿರುಗಿ ಅಕ್ಷರಸರಣಿಗಳು_2 != ಶೂನ್ಯಸೂಚಕ && !ಅಕ್ಷರಸರಣಿಗಳು_2->is_ಖಾಲಿಗಳು();
}

int32_t Unicode_ಗಣ::ಅಕ್ಷರಸರಣಿಗಳು_ಗಾತ್ರ() ಸ್ಥಿರಾಂಕ {
    ಹಿಂತಿರುಗಿ ಅಕ್ಷರಸರಣಿಗಳು_2 == ಶೂನ್ಯಸೂಚಕ ? 0 : ಅಕ್ಷರಸರಣಿಗಳು_2->size();
}

UBool Unicode_ಗಣ::ಅಕ್ಷರಸರಣಿಗಳು_ಒಳಗೊಂಡಿದೆ(ಸ್ಥಿರಾಂಕ UnicodeString &s) ಸ್ಥಿರಾಂಕ {
    ಹಿಂತಿರುಗಿ ಅಕ್ಷರಸರಣಿಗಳು_2 != ಶೂನ್ಯಸೂಚಕ && ಅಕ್ಷರಸರಣಿಗಳು_2->ಒಳಗೊಂಡಿದೆ((ಶೂನ್ಯ*) &s);
}

//----------------------------------------------------------------
// Constructors &c
//----------------------------------------------------------------

/**
 * Constructs an empty set.
 */
Unicode_ಗಣ::Unicode_ಗಣ() {
    list[0] = UNICODESET_ಮೇಲಿನ;
    _dbgct(ಇದು);
}

/**
 * Constructs a set containing the given range. If <code>end >
 * start</code> then an empty set is created.
 *
 * @param start first character, inclusive, of range
 * @param end last character, inclusive, of range
 */
Unicode_ಗಣ::Unicode_ಗಣ(UChar32 ಆರಂಭ, UChar32 ಕೊನೆ) {
    list[0] = UNICODESET_ಮೇಲಿನ;
    ಕೂಡಿಸು(ಆರಂಭ, ಕೊನೆ);
    _dbgct(ಇದು);
}

/**
 * Constructs a set that is identical to the given UnicodeSet.
 */
Unicode_ಗಣ::Unicode_ಗಣ(ಸ್ಥಿರಾಂಕ Unicode_ಗಣ& o) : UnicodeFilter(o) {
    *ಇದು = o;
    _dbgct(ಇದು);
}

// Copy-construct as thawed.
Unicode_ಗಣ::Unicode_ಗಣ(ಸ್ಥಿರಾಂಕ Unicode_ಗಣ& o, UBool /* asThawed */) : UnicodeFilter(o) {
    ಆದರೆ (ensure_ಸಾಮರ್ಥ್ಯ(o.len)) {
        // *this = o except for bmpSet and stringSpan
        len = o.len;
        uprv_memcpy(list, o.list, (size_t)len*ಗಾತ್ರ(UChar32));
        ಆದರೆ (o.has_ಅಕ್ಷರಸರಣಿಗಳು()) {
            UError_ಸಂಕೇತ ಸ್ಥಿತಿ_2 = U_ZERO_ERROR;
            ಆದರೆ (!allocate_ಅಕ್ಷರಸರಣಿಗಳು(ಸ್ಥಿತಿ_2) ||
                    (ಅಕ್ಷರಸರಣಿಗಳು_2->assign(*o.ಅಕ್ಷರಸರಣಿಗಳು_2, cloneUnicodeString, ಸ್ಥಿತಿ_2), U_FAILURE(ಸ್ಥಿತಿ_2))) {
                ಗಣ_ಗಮ್ಯ_Bogus();
                ಹಿಂತಿರುಗಿ;
            }
        }
        ಆದರೆ (o.pat) {
            ಗಣ_ನಮೂನೆ(o.pat, o.patLen);
        }
        _dbgct(ಇದು);
    }
}

/**
 * Destructs the set.
 */
Unicode_ಗಣ::~Unicode_ಗಣ() {
    _dbgdt(ಇದು); // first!
    ಆದರೆ (list != ರಾಶಿ_List) {
        uprv_free(list);
    }
    ಅಳಿಸು bmp_ಗಣ;
    ಆದರೆ (ಬಫರ್ != ರಾಶಿ_List) {
        uprv_free(ಬಫರ್);
    }
    ಅಳಿಸು ಅಕ್ಷರಸರಣಿಗಳು_2;
    ಅಳಿಸು string_ಹರವು;
    release_ನಮೂನೆ();
}

/**
 * Assigns this object to be a copy of another.
 */
Unicode_ಗಣ& Unicode_ಗಣ::ಕ್ರಿಯಾಕಾರಕ=(ಸ್ಥಿರಾಂಕ Unicode_ಗಣ& o) U_LIFETIME_BOUND {
    ಹಿಂತಿರುಗಿ copyFrom(o, ಸುಳ್ಳು);
}

Unicode_ಗಣ& Unicode_ಗಣ::copyFrom(ಸ್ಥಿರಾಂಕ Unicode_ಗಣ& o, UBool asThawed) U_LIFETIME_BOUND {
    ಆದರೆ (ಇದು == &o) {
        ಹಿಂತಿರುಗಿ *ಇದು;
    }
    ಆದರೆ (isFrozen()) {
        ಹಿಂತಿರುಗಿ *ಇದು;
    }
    ಆದರೆ (o.isBogus()) {
        ಗಣ_ಗಮ್ಯ_Bogus();
        ಹಿಂತಿರುಗಿ *ಇದು;
    }
    ಆದರೆ (!ensure_ಸಾಮರ್ಥ್ಯ(o.len)) {
        // ensureCapacity will mark the UnicodeSet as Bogus if OOM failure happens.
        ಹಿಂತಿರುಗಿ *ಇದು;
    }
    len = o.len;
    uprv_memcpy(list, o.list, (size_t)len*ಗಾತ್ರ(UChar32));
    ಆದರೆ (o.bmp_ಗಣ != ಶೂನ್ಯಸೂಚಕ && !asThawed) {
        bmp_ಗಣ = ಹೊಸ BMPSet(*o.bmp_ಗಣ, list, len);
        ಆದರೆ (bmp_ಗಣ == ಶೂನ್ಯಸೂಚಕ) { // Check for memory allocation error.
            ಗಣ_ಗಮ್ಯ_Bogus();
            ಹಿಂತಿರುಗಿ *ಇದು;
        }
    }
    ಆದರೆ (o.has_ಅಕ್ಷರಸರಣಿಗಳು()) {
        UError_ಸಂಕೇತ ಸ್ಥಿತಿ_2 = U_ZERO_ERROR;
        ಆದರೆ ((ಅಕ್ಷರಸರಣಿಗಳು_2 == ಶೂನ್ಯಸೂಚಕ && !allocate_ಅಕ್ಷರಸರಣಿಗಳು(ಸ್ಥಿತಿ_2)) ||
                (ಅಕ್ಷರಸರಣಿಗಳು_2->assign(*o.ಅಕ್ಷರಸರಣಿಗಳು_2, cloneUnicodeString, ಸ್ಥಿತಿ_2), U_FAILURE(ಸ್ಥಿತಿ_2))) {
            ಗಣ_ಗಮ್ಯ_Bogus();
            ಹಿಂತಿರುಗಿ *ಇದು;
        }
    } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (has_ಅಕ್ಷರಸರಣಿಗಳು()) {
        ಅಕ್ಷರಸರಣಿಗಳು_2->removeAllElements();
    }
    ಆದರೆ (o.string_ಹರವು != ಶೂನ್ಯಸೂಚಕ && ಅಕ್ಷರಸರಣಿಗಳು_2 != ಶೂನ್ಯಸೂಚಕ && !asThawed) {
        string_ಹರವು = ಹೊಸ Unicode_ಗಣ_String_ಹರವು(*o.string_ಹರವು, *ಅಕ್ಷರಸರಣಿಗಳು_2);
        ಆದರೆ (string_ಹರವು == ಶೂನ್ಯಸೂಚಕ) { // Check for memory allocation error.
            ಗಣ_ಗಮ್ಯ_Bogus();
            ಹಿಂತಿರುಗಿ *ಇದು;
        }
    }
    release_ನಮೂನೆ();
    ಆದರೆ (o.pat) {
        ಗಣ_ನಮೂನೆ(o.pat, o.patLen);
    }
    ಹಿಂತಿರುಗಿ *ಇದು;
}

/**
 * Returns a copy of this object.  All UnicodeMatcher objects have
 * to support cloning in order to allow classes using
 * UnicodeMatchers, such as Transliterator, to implement cloning.
 */
Unicode_ಗಣ* Unicode_ಗಣ::clone() ಸ್ಥಿರಾಂಕ {
    ಹಿಂತಿರುಗಿ ಹೊಸ Unicode_ಗಣ(*ಇದು);
}

Unicode_ಗಣ *Unicode_ಗಣ::cloneAsThawed() ಸ್ಥಿರಾಂಕ {
    ಹಿಂತಿರುಗಿ ಹೊಸ Unicode_ಗಣ(*ಇದು, ನಿಜ);
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
ತಾರ್ಕಿಕ Unicode_ಗಣ::ಕ್ರಿಯಾಕಾರಕ==(ಸ್ಥಿರಾಂಕ Unicode_ಗಣ& o) ಸ್ಥಿರಾಂಕ {
    ಆದರೆ (len != o.len) ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
    ಗಾಗಿ (int32_t i = 0; i < len; ++i) {
        ಆದರೆ (list[i] != o.list[i]) ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
    }
    ಆದರೆ (has_ಅಕ್ಷರಸರಣಿಗಳು() != o.has_ಅಕ್ಷರಸರಣಿಗಳು()) { ಹಿಂತಿರುಗಿ ಸುಳ್ಳು; }
    ಆದರೆ (has_ಅಕ್ಷರಸರಣಿಗಳು() && *ಅಕ್ಷರಸರಣಿಗಳು_2 != *o.ಅಕ್ಷರಸರಣಿಗಳು_2) ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
    ಹಿಂತಿರುಗಿ ನಿಜ;
}

/**
 * Returns the hash code value for this set.
 *
 * @return the hash code value for this set.
 * @see Object#hashCode()
 */
int32_t Unicode_ಗಣ::hash_ಸಂಕೇತ() ಸ್ಥಿರಾಂಕ {
    uint32_t ಫಲಿತಾಂಶ = static_cast<uint32_t>(len);
    ಗಾಗಿ (int32_t i = 0; i < len; ++i) {
        ಫಲಿತಾಂಶ *= 1000003u;
        ಫಲಿತಾಂಶ += list[i];
    }
    ಹಿಂತಿರುಗಿ static_cast<int32_t>(ಫಲಿತಾಂಶ);
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
int32_t Unicode_ಗಣ::size() ಸ್ಥಿರಾಂಕ {
    int32_t n = 0;
    int32_t count = get_Range_ಎಣಿಕೆ();
    ಗಾಗಿ (int32_t i = 0; i < count; ++i) {
        n += get_Range_ಕೊನೆ(i) - get_Range_ಆರಂಭ(i) + 1;
    }
    ಹಿಂತಿರುಗಿ n + ಅಕ್ಷರಸರಣಿಗಳು_ಗಾತ್ರ();
}

/**
 * Returns <tt>true</tt> if this set contains no elements.
 *
 * @return <tt>true</tt> if this set contains no elements.
 */
UBool Unicode_ಗಣ::is_ಖಾಲಿಗಳು() ಸ್ಥಿರಾಂಕ {
    ಹಿಂತಿರುಗಿ len == 1 && !has_ಅಕ್ಷರಸರಣಿಗಳು();
}

/**
 * Returns true if this set contains the given character.
 * @param c character to be checked for containment
 * @return true if the test condition is met
 */
UBool Unicode_ಗಣ::ಒಳಗೊಂಡಿದೆ(UChar32 c) ಸ್ಥಿರಾಂಕ {
    // Set i to the index of the start item greater than ch
    // We know we will terminate without length test!
    // LATER: for large sets, add binary search
    //int32_t i = -1;
    //for (;;) {
    //    if (c < list[++i]) break;
    //}
    ಆದರೆ (bmp_ಗಣ != ಶೂನ್ಯಸೂಚಕ) {
        ಹಿಂತಿರುಗಿ bmp_ಗಣ->ಒಳಗೊಂಡಿದೆ(c);
    }
    ಆದರೆ (string_ಹರವು != ಶೂನ್ಯಸೂಚಕ) {
        ಹಿಂತಿರುಗಿ string_ಹರವು->ಒಳಗೊಂಡಿದೆ(c);
    }
    ಆದರೆ (c >= UNICODESET_ಮೇಲಿನ) { // Don't need to check LOW bound
        ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
    }
    int32_t i = ಕಂಡುಹಿಡಿ_ಸಂಕೇತ_ಬಿಂದು(c);
    ಹಿಂತಿರುಗಿ i & 1; // return true if odd
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
int32_t Unicode_ಗಣ::ಕಂಡುಹಿಡಿ_ಸಂಕೇತ_ಬಿಂದು(UChar32 c) ಸ್ಥಿರಾಂಕ {
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
    ಆದರೆ (c < list[0])
        ಹಿಂತಿರುಗಿ 0;
    // High runner test.  c is often after the last range, so an
    // initial check for this condition pays off.
    int32_t lo = 0;
    int32_t hi = len - 1;
    ಆದರೆ (lo >= hi || c >= list[hi-1])
        ಹಿಂತಿರುಗಿ hi;
    // invariant: c >= list[lo]
    // invariant: c < list[hi]
    ಗಾಗಿ (;;) {
        int32_t i = (lo + hi) >> 1;
        ಆದರೆ (i == lo) {
            ನಿಲ್ಲಿಸು; // Found!
        } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (c < list[i]) {
            hi = i;
        } ಇಲ್ಲದಿದ್ದರೆ {
            lo = i;
        }
    }
    ಹಿಂತಿರುಗಿ hi;
}

/**
 * Returns true if this set contains every character
 * of the given range.
 * @param start first character, inclusive, of the range
 * @param end last character, inclusive, of the range
 * @return true if the test condition is met
 */
UBool Unicode_ಗಣ::ಒಳಗೊಂಡಿದೆ(UChar32 ಆರಂಭ, UChar32 ಕೊನೆ) ಸ್ಥಿರಾಂಕ {
    //int32_t i = -1;
    //for (;;) {
    //    if (start < list[++i]) break;
    //}
    int32_t i = ಕಂಡುಹಿಡಿ_ಸಂಕೇತ_ಬಿಂದು(ಆರಂಭ);
    ಹಿಂತಿರುಗಿ ((i & 1) != 0 && ಕೊನೆ < list[i]);
}

/**
 * Returns <tt>true</tt> if this set contains the given
 * multicharacter string.
 * @param s string to be checked for containment
 * @return <tt>true</tt> if this set contains the specified string
 */
UBool Unicode_ಗಣ::ಒಳಗೊಂಡಿದೆ(ಸ್ಥಿರಾಂಕ UnicodeString& s) ಸ್ಥಿರಾಂಕ {
    int32_t cp = getSingleCP(s);
    ಆದರೆ (cp < 0) {
        ಹಿಂತಿರುಗಿ ಅಕ್ಷರಸರಣಿಗಳು_ಒಳಗೊಂಡಿದೆ(s);
    } ಇಲ್ಲದಿದ್ದರೆ {
        ಹಿಂತಿರುಗಿ ಒಳಗೊಂಡಿದೆ(static_cast<UChar32>(cp));
    }
}

/**
 * Returns true if this set contains all the characters and strings
 * of the given set.
 * @param c set to be checked for containment
 * @return true if the test condition is met
 */
UBool Unicode_ಗಣ::ಒಳಗೊಂಡಿದೆ_All(ಸ್ಥಿರಾಂಕ Unicode_ಗಣ& c) ಸ್ಥಿರಾಂಕ {
    // The specified set is a subset if all of its pairs are contained in
    // this set.  It's possible to code this more efficiently in terms of
    // direct manipulation of the inversion lists if the need arises.
    int32_t n = c.get_Range_ಎಣಿಕೆ();
    ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i=0; i<n; ++i) {
        ಆದರೆ (!ಒಳಗೊಂಡಿದೆ(c.get_Range_ಆರಂಭ(i), c.get_Range_ಕೊನೆ(i))) {
            ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
        }
    }
    ಹಿಂತಿರುಗಿ !c.has_ಅಕ್ಷರಸರಣಿಗಳು() || (ಅಕ್ಷರಸರಣಿಗಳು_2 != ಶೂನ್ಯಸೂಚಕ && ಅಕ್ಷರಸರಣಿಗಳು_2->ಒಳಗೊಂಡಿದೆ_All(*c.ಅಕ್ಷರಸರಣಿಗಳು_2));
}

/**
 * Returns true if this set contains all the characters
 * of the given string.
 * @param s string containing characters to be checked for containment
 * @return true if the test condition is met
 */
UBool Unicode_ಗಣ::ಒಳಗೊಂಡಿದೆ_All(ಸ್ಥಿರಾಂಕ UnicodeString& s) ಸ್ಥಿರಾಂಕ {
    ಹಿಂತಿರುಗಿ ಹರವು(s.get_ಬಫರ್(), s.length(), USET_ಹರವು_CONTAINED) == s.length();
}

/**
 * Returns true if this set contains none of the characters
 * of the given range.
 * @param start first character, inclusive, of the range
 * @param end last character, inclusive, of the range
 * @return true if the test condition is met
 */
UBool Unicode_ಗಣ::ಒಳಗೊಂಡಿದೆ_None(UChar32 ಆರಂಭ, UChar32 ಕೊನೆ) ಸ್ಥಿರಾಂಕ {
    //int32_t i = -1;
    //for (;;) {
    //    if (start < list[++i]) break;
    //}
    int32_t i = ಕಂಡುಹಿಡಿ_ಸಂಕೇತ_ಬಿಂದು(ಆರಂಭ);
    ಹಿಂತಿರುಗಿ ((i & 1) == 0 && ಕೊನೆ < list[i]);
}

/**
 * Returns true if this set contains none of the characters and strings
 * of the given set.
 * @param c set to be checked for containment
 * @return true if the test condition is met
 */
UBool Unicode_ಗಣ::ಒಳಗೊಂಡಿದೆ_None(ಸ್ಥಿರಾಂಕ Unicode_ಗಣ& c) ಸ್ಥಿರಾಂಕ {
    // The specified set is a subset if all of its pairs are contained in
    // this set.  It's possible to code this more efficiently in terms of
    // direct manipulation of the inversion lists if the need arises.
    int32_t n = c.get_Range_ಎಣಿಕೆ();
    ಗಾಗಿ (int32_t i=0; i<n; ++i) {
        ಆದರೆ (!ಒಳಗೊಂಡಿದೆ_None(c.get_Range_ಆರಂಭ(i), c.get_Range_ಕೊನೆ(i))) {
            ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
        }
    }
    ಹಿಂತಿರುಗಿ ಅಕ್ಷರಸರಣಿಗಳು_2 == ಶೂನ್ಯಸೂಚಕ || !c.has_ಅಕ್ಷರಸರಣಿಗಳು() || ಅಕ್ಷರಸರಣಿಗಳು_2->ಒಳಗೊಂಡಿದೆ_None(*c.ಅಕ್ಷರಸರಣಿಗಳು_2);
}

/**
 * Returns true if this set contains none of the characters
 * of the given string.
 * @param s string containing characters to be checked for containment
 * @return true if the test condition is met
 */
UBool Unicode_ಗಣ::ಒಳಗೊಂಡಿದೆ_None(ಸ್ಥಿರಾಂಕ UnicodeString& s) ಸ್ಥಿರಾಂಕ {
    ಹಿಂತಿರುಗಿ ಹರವು(s.get_ಬಫರ್(), s.length(), USET_ಹರವು_NOT_CONTAINED) == s.length();
}

/**
 * Returns <tt>true</tt> if this set contains any character whose low byte
 * is the given value.  This is used by <tt>RuleBasedTransliterator</tt> for
 * indexing.
 */
UBool Unicode_ಗಣ::matches_ಸೂಚ್ಯಂಕ_ಮೌಲ್ಯ(uint8_t v) ಸ್ಥಿರಾಂಕ {
    /* The index value v, in the range [0,255], is contained in this set if
     * it is contained in any pair of this set.  Pairs either have the high
     * bytes equal, or unequal.  If the high bytes are equal, then we have
     * aaxx..aayy, where aa is the high byte.  Then v is contained if xx <=
     * v <= yy.  If the high bytes are unequal we have aaxx..bbyy, bb>aa.
     * Then v is contained if xx <= v || v <= yy.  (This is identical to the
     * time zone month containment logic.)
     */
    int32_t i;
    int32_t range_ಎಣಿಕೆ=get_Range_ಎಣಿಕೆ();
    ಗಾಗಿ (i=0; i<range_ಎಣಿಕೆ; ++i) {
        UChar32 ಕೆಳಗಿನ = get_Range_ಆರಂಭ(i);
        UChar32 ಮೇಲಿನ = get_Range_ಕೊನೆ(i);
        ಆದರೆ ((ಕೆಳಗಿನ & ~0xFF) == (ಮೇಲಿನ & ~0xFF)) {
            ಆದರೆ ((ಕೆಳಗಿನ & 0xFF) <= v && v <= (ಮೇಲಿನ & 0xFF)) {
                ಹಿಂತಿರುಗಿ ನಿಜ;
            }
        } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ ((ಕೆಳಗಿನ & 0xFF) <= v || v <= (ಮೇಲಿನ & 0xFF)) {
            ಹಿಂತಿರುಗಿ ನಿಜ;
        }
    }
    ಆದರೆ (has_ಅಕ್ಷರಸರಣಿಗಳು()) {
        ಗಾಗಿ (i=0; i<ಅಕ್ಷರಸರಣಿಗಳು_2->size(); ++i) {
            ಸ್ಥಿರಾಂಕ UnicodeString& s = *static_cast<ಸ್ಥಿರಾಂಕ UnicodeString*>(ಅಕ್ಷರಸರಣಿಗಳು_2->elementAt(i));
            ಆದರೆ (s.is_ಖಾಲಿಗಳು()) {
                ಮುಂದುವರಿ;  // skip the empty string
            }
            UChar32 c = s.char32At(0);
            ಆದರೆ ((c & 0xFF) == v) {
                ಹಿಂತಿರುಗಿ ನಿಜ;
            }
        }
    }
    ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
}

/**
 * Implementation of UnicodeMatcher::matches().  Always matches the
 * longest possible multichar string.
 */
UMatchDegree Unicode_ಗಣ::matches(ಸ್ಥಿರಾಂಕ Replaceable& ಪಠ್ಯ,
                                 int32_t& ಸ್ಥಾನಾಂತರ,
                                 int32_t ಎಲ್ಲೆ,
                                 UBool incremental) {
    ಆದರೆ (ಸ್ಥಾನಾಂತರ == ಎಲ್ಲೆ) {
        ಆದರೆ (ಒಳಗೊಂಡಿದೆ(U_ETHER)) {
            ಹಿಂತಿರುಗಿ incremental ? U_PARTIAL_ಹೊಂದಿಕೆ : U_ಹೊಂದಿಕೆ;
        } ಇಲ್ಲದಿದ್ದರೆ {
            ಹಿಂತಿರುಗಿ U_MISMATCH;
        }
    } ಇಲ್ಲದಿದ್ದರೆ {
        ಆದರೆ (has_ಅಕ್ಷರಸರಣಿಗಳು()) { // try strings first

            // might separate forward and backward loops later
            // for now they are combined

            // TODO Improve efficiency of this, at least in the forward
            // direction, if not in both.  In the forward direction we
            // can assume the strings are sorted.

            int32_t i;
            UBool forward = ಸ್ಥಾನಾಂತರ < ಎಲ್ಲೆ;

            // firstChar is the leftmost char to match in the
            // forward direction or the rightmost char to match in
            // the reverse direction.
            char16_t ಮೊದಲ_Char = ಪಠ್ಯ.charAt(ಸ್ಥಾನಾಂತರ);

            // If there are multiple strings that can match we
            // return the longest match.
            int32_t ಮೇಲಿನ_Water_ಉದ್ದಳತೆ = 0;

            ಗಾಗಿ (i=0; i<ಅಕ್ಷರಸರಣಿಗಳು_2->size(); ++i) {
                ಸ್ಥಿರಾಂಕ UnicodeString& trial = *static_cast<ಸ್ಥಿರಾಂಕ UnicodeString*>(ಅಕ್ಷರಸರಣಿಗಳು_2->elementAt(i));
                ಆದರೆ (trial.is_ಖಾಲಿಗಳು()) {
                    ಮುಂದುವರಿ;  // skip the empty string
                }

                char16_t c = trial.charAt(forward ? 0 : trial.length() - 1);

                // Strings are sorted, so we can optimize in the
                // forward direction.
                ಆದರೆ (forward && c > ಮೊದಲ_Char) ನಿಲ್ಲಿಸು;
                ಆದರೆ (c != ಮೊದಲ_Char) ಮುಂದುವರಿ;

                int32_t ಹೊಂದಿಕೆ_Len = ಹೊಂದಿಕೆ_Rest(ಪಠ್ಯ, ಸ್ಥಾನಾಂತರ, ಎಲ್ಲೆ, trial);

                ಆದರೆ (incremental) {
                    int32_t maxLen = forward ? ಎಲ್ಲೆ-ಸ್ಥಾನಾಂತರ : ಸ್ಥಾನಾಂತರ-ಎಲ್ಲೆ;
                    ಆದರೆ (ಹೊಂದಿಕೆ_Len == maxLen) {
                        // We have successfully matched but only up to limit.
                        ಹಿಂತಿರುಗಿ U_PARTIAL_ಹೊಂದಿಕೆ;
                    }
                }

                ಆದರೆ (ಹೊಂದಿಕೆ_Len == trial.length()) {
                    // We have successfully matched the whole string.
                    ಆದರೆ (ಹೊಂದಿಕೆ_Len > ಮೇಲಿನ_Water_ಉದ್ದಳತೆ) {
                        ಮೇಲಿನ_Water_ಉದ್ದಳತೆ = ಹೊಂದಿಕೆ_Len;
                    }
                    // In the forward direction we know strings
                    // are sorted so we can bail early.
                    ಆದರೆ (forward && ಹೊಂದಿಕೆ_Len < ಮೇಲಿನ_Water_ಉದ್ದಳತೆ) {
                        ನಿಲ್ಲಿಸು;
                    }
                    ಮುಂದುವರಿ;
                }
            }

            // We've checked all strings without a partial match.
            // If we have full matches, return the longest one.
            ಆದರೆ (ಮೇಲಿನ_Water_ಉದ್ದಳತೆ != 0) {
                ಸ್ಥಾನಾಂತರ += forward ? ಮೇಲಿನ_Water_ಉದ್ದಳತೆ : -ಮೇಲಿನ_Water_ಉದ್ದಳತೆ;
                ಹಿಂತಿರುಗಿ U_ಹೊಂದಿಕೆ;
            }
        }
        ಹಿಂತಿರುಗಿ UnicodeFilter::matches(ಪಠ್ಯ, ಸ್ಥಾನಾಂತರ, ಎಲ್ಲೆ, incremental);
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
int32_t Unicode_ಗಣ::ಹೊಂದಿಕೆ_Rest(ಸ್ಥಿರಾಂಕ Replaceable& ಪಠ್ಯ,
                              int32_t ಆರಂಭ, int32_t ಎಲ್ಲೆ,
                              ಸ್ಥಿರಾಂಕ UnicodeString& s) {
    int32_t i;
    int32_t maxLen;
    int32_t slen = s.length();
    ಆದರೆ (ಆರಂಭ < ಎಲ್ಲೆ) {
        maxLen = ಎಲ್ಲೆ - ಆರಂಭ;
        ಆದರೆ (maxLen > slen) maxLen = slen;
        ಗಾಗಿ (i = 1; i < maxLen; ++i) {
            ಆದರೆ (ಪಠ್ಯ.charAt(ಆರಂಭ + i) != s.charAt(i)) ಹಿಂತಿರುಗಿ 0;
        }
    } ಇಲ್ಲದಿದ್ದರೆ {
        maxLen = ಆರಂಭ - ಎಲ್ಲೆ;
        ಆದರೆ (maxLen > slen) maxLen = slen;
        --slen; // <=> slen = s.length() - 1;
        ಗಾಗಿ (i = 1; i < maxLen; ++i) {
            ಆದರೆ (ಪಠ್ಯ.charAt(ಆರಂಭ - i) != s.charAt(slen - i)) ಹಿಂತಿರುಗಿ 0;
        }
    }
    ಹಿಂತಿರುಗಿ maxLen;
}

/**
 * Implement of UnicodeMatcher
 */
ಶೂನ್ಯ Unicode_ಗಣ::ಕೂಡಿಸು_ಹೊಂದಿಕೆ_ಗಣ_ಗಮ್ಯ(Unicode_ಗಣ& ಗಮ್ಯ_Union_ಗಮ್ಯ) ಸ್ಥಿರಾಂಕ {
    ಗಮ್ಯ_Union_ಗಮ್ಯ.ಕೂಡಿಸು_All(*ಇದು);
}

/**
 * Returns the index of the given character within this set, where
 * the set is ordered by ascending code point.  If the character
 * is not in this set, return -1.  The inverse of this method is
 * <code>charAt()</code>.
 * @return an index from 0..size()-1, or -1
 */
int32_t Unicode_ಗಣ::ಸೂಚ್ಯಂಕ_ದ(UChar32 c) ಸ್ಥಿರಾಂಕ {
    ಆದರೆ (c < MIN_ಮೌಲ್ಯ || c > MAX_ಮೌಲ್ಯ) {
        ಹಿಂತಿರುಗಿ -1;
    }
    int32_t i = 0;
    int32_t n = 0;
    ಗಾಗಿ (;;) {
        UChar32 ಆರಂಭ = list[i++];
        ಆದರೆ (c < ಆರಂಭ) {
            ಹಿಂತಿರುಗಿ -1;
        }
        UChar32 ಎಲ್ಲೆ = list[i++];
        ಆದರೆ (c < ಎಲ್ಲೆ) {
            ಹಿಂತಿರುಗಿ n + c - ಆರಂಭ;
        }
        n += ಎಲ್ಲೆ - ಆರಂಭ;
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
UChar32 Unicode_ಗಣ::charAt(int32_t ಸೂಚ್ಯಂಕ) ಸ್ಥಿರಾಂಕ {
    ಆದರೆ (ಸೂಚ್ಯಂಕ >= 0) {
        // len2 is the largest even integer <= len, that is, it is len
        // for even values and len-1 for odd values.  With odd values
        // the last entry is UNICODESET_HIGH.
        int32_t len2 = len & ~1;
        ಗಾಗಿ (int32_t i=0; i < len2;) {
            UChar32 ಆರಂಭ = list[i++];
            int32_t count = list[i++] - ಆರಂಭ;
            ಆದರೆ (ಸೂಚ್ಯಂಕ < count) {
                ಹಿಂತಿರುಗಿ static_cast<UChar32>(ಆರಂಭ + ಸೂಚ್ಯಂಕ);
            }
            ಸೂಚ್ಯಂಕ -= count;
        }
    }
    ಹಿಂತಿರುಗಿ static_cast<UChar32>(-1);
}

/**
 * Make this object represent the range <code>start - end</code>.
 * If <code>end > start</code> then this object is set to an
 * an empty range.
 *
 * @param start first character in the set, inclusive
 * @rparam end last character in the set, inclusive
 */
Unicode_ಗಣ& Unicode_ಗಣ::ಗಣ(UChar32 ಆರಂಭ, UChar32 ಕೊನೆ) U_LIFETIME_BOUND {
    clear();
    complement(ಆರಂಭ, ಕೊನೆ);
    ಹಿಂತಿರುಗಿ *ಇದು;
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
Unicode_ಗಣ& Unicode_ಗಣ::ಕೂಡಿಸು(UChar32 ಆರಂಭ, UChar32 ಕೊನೆ) U_LIFETIME_BOUND {
    ಆದರೆ (pin_ಸಂಕೇತ_ಬಿಂದು(ಆರಂಭ) < pin_ಸಂಕೇತ_ಬಿಂದು(ಕೊನೆ)) {
        UChar32 ಎಲ್ಲೆ = ಕೊನೆ + 1;
        // Fast path for adding a new range after the last one.
        // Odd list length: [..., lastStart, lastLimit, HIGH]
        ಆದರೆ ((len & 1) != 0) {
            // If the list is empty, set lastLimit low enough to not be adjacent to 0.
            UChar32 ಕೊನೆಯ_ಎಲ್ಲೆ = len == 1 ? -2 : list[len - 2];
            ಆದರೆ (ಕೊನೆಯ_ಎಲ್ಲೆ <= ಆರಂಭ && !isFrozen() && !isBogus()) {
                ಆದರೆ (ಕೊನೆಯ_ಎಲ್ಲೆ == ಆರಂಭ) {
                    // Extend the last range.
                    list[len - 2] = ಎಲ್ಲೆ;
                    ಆದರೆ (ಎಲ್ಲೆ == UNICODESET_ಮೇಲಿನ) {
                        --len;
                    }
                } ಇಲ್ಲದಿದ್ದರೆ {
                    list[len - 1] = ಆರಂಭ;
                    ಆದರೆ (ಎಲ್ಲೆ < UNICODESET_ಮೇಲಿನ) {
                        ಆದರೆ (ensure_ಸಾಮರ್ಥ್ಯ(len + 2)) {
                            list[len++] = ಎಲ್ಲೆ;
                            list[len++] = UNICODESET_ಮೇಲಿನ;
                        }
                    } ಇಲ್ಲದಿದ್ದರೆ {  // limit == UNICODESET_HIGH
                        ಆದರೆ (ensure_ಸಾಮರ್ಥ್ಯ(len + 1)) {
                            list[len++] = UNICODESET_ಮೇಲಿನ;
                        }
                    }
                }
                release_ನಮೂನೆ();
                ಹಿಂತಿರುಗಿ *ಇದು;
            }
        }
        // This is slow. Could be much faster using findCodePoint(start)
        // and modifying the list, dealing with adjacent & overlapping ranges.
        UChar32 range[3] = { ಆರಂಭ, ಎಲ್ಲೆ, UNICODESET_ಮೇಲಿನ };
        ಕೂಡಿಸು(range, 2, 0);
    } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (ಆರಂಭ == ಕೊನೆ) {
        ಕೂಡಿಸು(ಆರಂಭ);
    }
    ಹಿಂತಿರುಗಿ *ಇದು;
}

// #define DEBUG_US_ADD

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ DEBUG_US_ಕೂಡಿಸು
#ಅನ್ತರ್ಭಾವ <stdio.h>
ಶೂನ್ಯ dump(UChar32 c) {
    ಆದರೆ (c <= 0xFF) {
        ಬರೆ("%c", (ಅಕ್ಷರ)c);
    } ಇಲ್ಲದಿದ್ದರೆ {
        ಬರೆ("U+%04X", c);
    }
}
ಶೂನ್ಯ dump(ಸ್ಥಿರಾಂಕ UChar32* list, int32_t len) {
    ಬರೆ("[");
    ಗಾಗಿ (int32_t i=0; i<len; ++i) {
        ಆದರೆ (i != 0) ಬರೆ(", ");
        dump(list[i]);
    }
    ಬರೆ("]");
}
#ಆದರೆ_ಅಂತ್ಯ

/**
 * Adds the specified character to this set if it is not already
 * present.  If this set already contains the specified character,
 * the call leaves this set unchanged.
 */
Unicode_ಗಣ& Unicode_ಗಣ::ಕೂಡಿಸು(UChar32 c) U_LIFETIME_BOUND {
    // find smallest i such that c < list[i]
    // if odd, then it is IN the set
    // if even, then it is OUT of the set
    int32_t i = ಕಂಡುಹಿಡಿ_ಸಂಕೇತ_ಬಿಂದು(pin_ಸಂಕೇತ_ಬಿಂದು(c));

    // already in set?
    ಆದರೆ ((i & 1) != 0  || isFrozen() || isBogus()) ಹಿಂತಿರುಗಿ *ಇದು;

    // HIGH is 0x110000
    // assert(list[len-1] == HIGH);

    // empty = [HIGH]
    // [start_0, limit_0, start_1, limit_1, HIGH]

    // [..., start_k-1, limit_k-1, start_k, limit_k, ..., HIGH]
    //                             ^
    //                             list[i]

    // i == 0 means c is before the first range

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ DEBUG_US_ಕೂಡಿಸು
    ಬರೆ("Add of ");
    dump(c);
    ಬರೆ(" found at %d", i);
    ಬರೆ(": ");
    dump(list, len);
    ಬರೆ(" => ");
#ಆದರೆ_ಅಂತ್ಯ

    ಆದರೆ (c == list[i]-1) {
        // c is before start of next range
        list[i] = c;
        // if we touched the HIGH mark, then add a new one
        ಆದರೆ (c == (UNICODESET_ಮೇಲಿನ - 1)) {
            ಆದರೆ (!ensure_ಸಾಮರ್ಥ್ಯ(len+1)) {
                // ensureCapacity will mark the object as Bogus if OOM failure happens.
                ಹಿಂತಿರುಗಿ *ಇದು;
            }
            list[len++] = UNICODESET_ಮೇಲಿನ;
        }
        ಆದರೆ (i > 0 && c == list[i-1]) {
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
            ಆಗ (src < srclimit) *(dst++) = *(src++);

            len -= 2;
        }
    }

    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (i > 0 && c == list[i-1]) {
        // c is after end of prior range
        list[i-1]++;
        // no need to check for collapse here
    }

    ಇಲ್ಲದಿದ್ದರೆ {
        // At this point we know the new char is not adjacent to
        // any existing ranges, and it is not 10FFFF.


        // [..., start_k-1, limit_k-1, start_k, limit_k, ..., HIGH]
        //                             ^
        //                             list[i]

        // [..., start_k-1, limit_k-1, c, c+1, start_k, limit_k, ..., HIGH]
        //                             ^
        //                             list[i]

        ಆದರೆ (!ensure_ಸಾಮರ್ಥ್ಯ(len+2)) {
            // ensureCapacity will mark the object as Bogus if OOM failure happens.
            ಹಿಂತಿರುಗಿ *ಇದು;
        }

        UChar32 *p = list + i;
        uprv_memmove(p + 2, p, (len - i) * ಗಾತ್ರ(*p));
        list[i] = c;
        list[i+1] = c+1;
        len += 2;
    }

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ DEBUG_US_ಕೂಡಿಸು
    dump(list, len);
    ಬರೆ("\n");

    ಗಾಗಿ (i=1; i<len; ++i) {
        ಆದರೆ (list[i] <= list[i-1]) {
            // Corrupt array!
            ಬರೆ("ERROR: list has been corrupted\n");
            ನಿರ್ಗಮಿಸು(1);
        }
    }
#ಆದರೆ_ಅಂತ್ಯ

    release_ನಮೂನೆ();
    ಹಿಂತಿರುಗಿ *ಇದು;
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
Unicode_ಗಣ& Unicode_ಗಣ::ಕೂಡಿಸು(ಸ್ಥಿರಾಂಕ UnicodeString& s) U_LIFETIME_BOUND {
    ಆದರೆ (isFrozen() || isBogus()) ಹಿಂತಿರುಗಿ *ಇದು;
    int32_t cp = getSingleCP(s);
    ಆದರೆ (cp < 0) {
        ಆದರೆ (!ಅಕ್ಷರಸರಣಿಗಳು_ಒಳಗೊಂಡಿದೆ(s)) {
            ಕೂಡಿಸು_2(s);
            release_ನಮೂನೆ();
        }
    } ಇಲ್ಲದಿದ್ದರೆ {
        ಕೂಡಿಸು(static_cast<UChar32>(cp));
    }
    ಹಿಂತಿರುಗಿ *ಇದು;
}

/**
 * Adds the given string, in order, to 'strings_'.  The given string
 * must have been checked by the caller to not already be in 'strings_'.
 */
ಶೂನ್ಯ Unicode_ಗಣ::ಕೂಡಿಸು_2(ಸ್ಥಿರಾಂಕ UnicodeString& s) {
    ಆದರೆ (isFrozen() || isBogus()) {
        ಹಿಂತಿರುಗಿ;
    }
    UError_ಸಂಕೇತ ec = U_ZERO_ERROR;
    ಆದರೆ (ಅಕ್ಷರಸರಣಿಗಳು_2 == ಶೂನ್ಯಸೂಚಕ && !allocate_ಅಕ್ಷರಸರಣಿಗಳು(ec)) {
        ಗಣ_ಗಮ್ಯ_Bogus();
        ಹಿಂತಿರುಗಿ;
    }
    Local_ಸೂಚಕ<UnicodeString> t(ಹೊಸ UnicodeString(s));
    ಆದರೆ (t.isNull()) { // Check for memory allocation error.
        ಗಣ_ಗಮ್ಯ_Bogus();
        ಹಿಂತಿರುಗಿ;
    }
    ಅಕ್ಷರಸರಣಿಗಳು_2->sorted_ಸೇರಿಸು(t.orphan(), compareUnicodeString, ec);
    ಆದರೆ (U_FAILURE(ec)) {
        ಗಣ_ಗಮ್ಯ_Bogus();
    }
}

/**
 * @return a code point IF the string consists of a single one.
 * otherwise returns -1.
 * @param string to test
 */
int32_t Unicode_ಗಣ::getSingleCP(ಸ್ಥಿರಾಂಕ UnicodeString& s) {
    int32_t s_ಉದ್ದಳತೆ = s.length();
    ಆದರೆ (s_ಉದ್ದಳತೆ == 1) ಹಿಂತಿರುಗಿ s.charAt(0);
    ಆದರೆ (s_ಉದ್ದಳತೆ == 2) {
        UChar32 cp = s.char32At(0);
        ಆದರೆ (cp > 0xFFFF) { // is surrogate pair
            ಹಿಂತಿರುಗಿ cp;
        }
    }
    ಹಿಂತಿರುಗಿ -1;
}

/**
 * Adds each of the characters in this string to the set. Thus "ch" => {"c", "h"}
 * If this set already any particular character, it has no effect on that character.
 * @param the source string
 * @return the modified set, for chaining
 */
Unicode_ಗಣ& Unicode_ಗಣ::ಕೂಡಿಸು_All(ಸ್ಥಿರಾಂಕ UnicodeString& s) U_LIFETIME_BOUND {
    UChar32 cp;
    ಗಾಗಿ (int32_t i = 0; i < s.length(); i += U16_ಉದ್ದಳತೆ(cp)) {
        cp = s.char32At(i);
        ಕೂಡಿಸು(cp);
    }
    ಹಿಂತಿರುಗಿ *ಇದು;
}

/**
 * Retains EACH of the characters in this string. Note: "ch" == {"c", "h"}
 * If this set already any particular character, it has no effect on that character.
 * @param the source string
 * @return the modified set, for chaining
 */
Unicode_ಗಣ& Unicode_ಗಣ::retainAll(ಸ್ಥಿರಾಂಕ UnicodeString& s) U_LIFETIME_BOUND {
    Unicode_ಗಣ ಗಣ;
    ಗಣ.ಕೂಡಿಸು_All(s);
    retainAll(ಗಣ);
    ಹಿಂತಿರುಗಿ *ಇದು;
}

/**
 * Complement EACH of the characters in this string. Note: "ch" == {"c", "h"}
 * If this set already any particular character, it has no effect on that character.
 * @param the source string
 * @return the modified set, for chaining
 */
Unicode_ಗಣ& Unicode_ಗಣ::complementAll(ಸ್ಥಿರಾಂಕ UnicodeString& s) U_LIFETIME_BOUND {
    Unicode_ಗಣ ಗಣ;
    ಗಣ.ಕೂಡಿಸು_All(s);
    complementAll(ಗಣ);
    ಹಿಂತಿರುಗಿ *ಇದು;
}

/**
 * Remove EACH of the characters in this string. Note: "ch" == {"c", "h"}
 * If this set already any particular character, it has no effect on that character.
 * @param the source string
 * @return the modified set, for chaining
 */
Unicode_ಗಣ& Unicode_ಗಣ::removeAll(ಸ್ಥಿರಾಂಕ UnicodeString& s) U_LIFETIME_BOUND {
    Unicode_ಗಣ ಗಣ;
    ಗಣ.ಕೂಡಿಸು_All(s);
    removeAll(ಗಣ);
    ಹಿಂತಿರುಗಿ *ಇದು;
}

Unicode_ಗಣ& Unicode_ಗಣ::remove_All_ಅಕ್ಷರಸರಣಿಗಳು() U_LIFETIME_BOUND {
    ಆದರೆ (!isFrozen() && has_ಅಕ್ಷರಸರಣಿಗಳು()) {
        ಅಕ್ಷರಸರಣಿಗಳು_2->removeAllElements();
        release_ನಮೂನೆ();
    }
    ಹಿಂತಿರುಗಿ *ಇದು;
}


/**
 * Makes a set from a multicharacter string. Thus "ch" => {"ch"}
 * <br><b>Warning: you cannot add an empty string ("") to a UnicodeSet.</b>
 * @param the source string
 * @return a newly created set containing the given string
 */
Unicode_ಗಣ* U_EXPORT2 Unicode_ಗಣ::createFrom(ಸ್ಥಿರಾಂಕ UnicodeString& s) {
    Unicode_ಗಣ *ಗಣ = ಹೊಸ Unicode_ಗಣ();
    ಆದರೆ (ಗಣ != ಶೂನ್ಯಸೂಚಕ) { // Check for memory allocation error.
        ಗಣ->ಕೂಡಿಸು(s);
    }
    ಹಿಂತಿರುಗಿ ಗಣ;
}


/**
 * Makes a set from each of the characters in the string. Thus "ch" => {"c", "h"}
 * @param the source string
 * @return a newly created set containing the given characters
 */
Unicode_ಗಣ* U_EXPORT2 Unicode_ಗಣ::createFromAll(ಸ್ಥಿರಾಂಕ UnicodeString& s) {
    Unicode_ಗಣ *ಗಣ = ಹೊಸ Unicode_ಗಣ();
    ಆದರೆ (ಗಣ != ಶೂನ್ಯಸೂಚಕ) { // Check for memory allocation error.
        ಗಣ->ಕೂಡಿಸು_All(s);
    }
    ಹಿಂತಿರುಗಿ ಗಣ;
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
Unicode_ಗಣ& Unicode_ಗಣ::retain(UChar32 ಆರಂಭ, UChar32 ಕೊನೆ) U_LIFETIME_BOUND {
    ಆದರೆ (pin_ಸಂಕೇತ_ಬಿಂದು(ಆರಂಭ) <= pin_ಸಂಕೇತ_ಬಿಂದು(ಕೊನೆ)) {
        UChar32 range[3] = { ಆರಂಭ, ಕೊನೆ+1, UNICODESET_ಮೇಲಿನ };
        retain(range, 2, 0);
    } ಇಲ್ಲದಿದ್ದರೆ {
        clear();
    }
    ಹಿಂತಿರುಗಿ *ಇದು;
}

Unicode_ಗಣ& Unicode_ಗಣ::retain(UChar32 c) U_LIFETIME_BOUND {
    ಹಿಂತಿರುಗಿ retain(c, c);
}

Unicode_ಗಣ& Unicode_ಗಣ::retain(ಸ್ಥಿರಾಂಕ UnicodeString &s) U_LIFETIME_BOUND {
    ಆದರೆ (isFrozen() || isBogus()) { ಹಿಂತಿರುಗಿ *ಇದು; }
    UChar32 cp = getSingleCP(s);
    ಆದರೆ (cp < 0) {
        ತಾರ್ಕಿಕ is_ಒಳ = ಅಕ್ಷರಸರಣಿಗಳು_ಒಳಗೊಂಡಿದೆ(s);
        // Check for getRangeCount() first to avoid somewhat-expensive size()
        // when there are single code points.
        ಆದರೆ (is_ಒಳ && get_Range_ಎಣಿಕೆ() == 0 && size() == 1) {
            ಹಿಂತಿರುಗಿ *ಇದು;
        }
        clear();
        ಆದರೆ (is_ಒಳ) {
            ಕೂಡಿಸು_2(s);
        }
    } ಇಲ್ಲದಿದ್ದರೆ {
        retain(cp, cp);
    }
    ಹಿಂತಿರುಗಿ *ಇದು;
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
Unicode_ಗಣ& Unicode_ಗಣ::remove(UChar32 ಆರಂಭ, UChar32 ಕೊನೆ) U_LIFETIME_BOUND {
    ಆದರೆ (pin_ಸಂಕೇತ_ಬಿಂದು(ಆರಂಭ) <= pin_ಸಂಕೇತ_ಬಿಂದು(ಕೊನೆ)) {
        UChar32 range[3] = { ಆರಂಭ, ಕೊನೆ+1, UNICODESET_ಮೇಲಿನ };
        retain(range, 2, 2);
    }
    ಹಿಂತಿರುಗಿ *ಇದು;
}

/**
 * Removes the specified character from this set if it is present.
 * The set will not contain the specified range once the call
 * returns.
 */
Unicode_ಗಣ& Unicode_ಗಣ::remove(UChar32 c) U_LIFETIME_BOUND {
    ಹಿಂತಿರುಗಿ remove(c, c);
}

/**
 * Removes the specified string from this set if it is present.
 * The set will not contain the specified character once the call
 * returns.
 * @param the source string
 * @return the modified set, for chaining
 */
Unicode_ಗಣ& Unicode_ಗಣ::remove(ಸ್ಥಿರಾಂಕ UnicodeString& s) U_LIFETIME_BOUND {
    ಆದರೆ (isFrozen() || isBogus()) ಹಿಂತಿರುಗಿ *ಇದು;
    int32_t cp = getSingleCP(s);
    ಆದರೆ (cp < 0) {
        ಆದರೆ (ಅಕ್ಷರಸರಣಿಗಳು_2 != ಶೂನ್ಯಸೂಚಕ && ಅಕ್ಷರಸರಣಿಗಳು_2->removeElement((ಶೂನ್ಯ*) &s)) {
            release_ನಮೂನೆ();
        }
    } ಇಲ್ಲದಿದ್ದರೆ {
        remove(static_cast<UChar32>(cp), static_cast<UChar32>(cp));
    }
    ಹಿಂತಿರುಗಿ *ಇದು;
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
Unicode_ಗಣ& Unicode_ಗಣ::complement(UChar32 ಆರಂಭ, UChar32 ಕೊನೆ) U_LIFETIME_BOUND {
    ಆದರೆ (isFrozen() || isBogus()) {
        ಹಿಂತಿರುಗಿ *ಇದು;
    }
    ಆದರೆ (pin_ಸಂಕೇತ_ಬಿಂದು(ಆರಂಭ) <= pin_ಸಂಕೇತ_ಬಿಂದು(ಕೊನೆ)) {
        UChar32 range[3] = { ಆರಂಭ, ಕೊನೆ+1, UNICODESET_ಮೇಲಿನ };
        exclusiveOr(range, 2, 0);
    }
    release_ನಮೂನೆ();
    ಹಿಂತಿರುಗಿ *ಇದು;
}

Unicode_ಗಣ& Unicode_ಗಣ::complement(UChar32 c) U_LIFETIME_BOUND {
    ಹಿಂತಿರುಗಿ complement(c, c);
}

/**
 * This is equivalent to
 * <code>complement(MIN_VALUE, MAX_VALUE)</code>.
 */
Unicode_ಗಣ& Unicode_ಗಣ::complement() U_LIFETIME_BOUND {
    ಆದರೆ (isFrozen() || isBogus()) {
        ಹಿಂತಿರುಗಿ *ಇದು;
    }
    ಆದರೆ (list[0] == UNICODESET_ಕೆಳಗಿನ) {
        uprv_memmove(list, list + 1, (size_t)(len-1)*ಗಾತ್ರ(UChar32));
        --len;
    } ಇಲ್ಲದಿದ್ದರೆ {
        ಆದರೆ (!ensure_ಸಾಮರ್ಥ್ಯ(len+1)) {
            ಹಿಂತಿರುಗಿ *ಇದು;
        }
        uprv_memmove(list + 1, list, (size_t)len*ಗಾತ್ರ(UChar32));
        list[0] = UNICODESET_ಕೆಳಗಿನ;
        ++len;
    }
    release_ನಮೂನೆ();
    ಹಿಂತಿರುಗಿ *ಇದು;
}

/**
 * Complement the specified string in this set.
 * The set will not contain the specified string once the call
 * returns.
 *
 * @param s the string to complement
 * @return this object, for chaining
 */
Unicode_ಗಣ& Unicode_ಗಣ::complement(ಸ್ಥಿರಾಂಕ UnicodeString& s) U_LIFETIME_BOUND {
    ಆದರೆ (isFrozen() || isBogus()) ಹಿಂತಿರುಗಿ *ಇದು;
    int32_t cp = getSingleCP(s);
    ಆದರೆ (cp < 0) {
        ಆದರೆ (ಅಕ್ಷರಸರಣಿಗಳು_ಒಳಗೊಂಡಿದೆ(s)) {
            ಅಕ್ಷರಸರಣಿಗಳು_2->removeElement((ಶೂನ್ಯ*) &s);
        } ಇಲ್ಲದಿದ್ದರೆ {
            ಕೂಡಿಸು_2(s);
        }
        release_ನಮೂನೆ();
    } ಇಲ್ಲದಿದ್ದರೆ {
        complement(static_cast<UChar32>(cp), static_cast<UChar32>(cp));
    }
    ಹಿಂತಿರುಗಿ *ಇದು;
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
Unicode_ಗಣ& Unicode_ಗಣ::ಕೂಡಿಸು_All(ಸ್ಥಿರಾಂಕ Unicode_ಗಣ& c) U_LIFETIME_BOUND {
    ಆದರೆ ( c.len>0 && c.list!=ಶೂನ್ಯಸೂಚಕ ) {
        ಕೂಡಿಸು(c.list, c.len, 0);
    }

    // Add strings in order
    ಆದರೆ ( c.ಅಕ್ಷರಸರಣಿಗಳು_2!=ಶೂನ್ಯಸೂಚಕ ) {
        ಗಾಗಿ (int32_t i=0; i<c.ಅಕ್ಷರಸರಣಿಗಳು_2->size(); ++i) {
            ಸ್ಥಿರಾಂಕ UnicodeString* s = static_cast<ಸ್ಥಿರಾಂಕ UnicodeString*>(c.ಅಕ್ಷರಸರಣಿಗಳು_2->elementAt(i));
            ಆದರೆ (!ಅಕ್ಷರಸರಣಿಗಳು_ಒಳಗೊಂಡಿದೆ(*s)) {
                ಕೂಡಿಸು_2(*s);
            }
        }
    }
    ಹಿಂತಿರುಗಿ *ಇದು;
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
Unicode_ಗಣ& Unicode_ಗಣ::retainAll(ಸ್ಥಿರಾಂಕ Unicode_ಗಣ& c) U_LIFETIME_BOUND {
    ಆದರೆ (isFrozen() || isBogus()) {
        ಹಿಂತಿರುಗಿ *ಇದು;
    }
    retain(c.list, c.len, 0);
    ಆದರೆ (has_ಅಕ್ಷರಸರಣಿಗಳು()) {
        ಆದರೆ (!c.has_ಅಕ್ಷರಸರಣಿಗಳು()) {
            ಅಕ್ಷರಸರಣಿಗಳು_2->removeAllElements();
        } ಇಲ್ಲದಿದ್ದರೆ {
            ಅಕ್ಷರಸರಣಿಗಳು_2->retainAll(*c.ಅಕ್ಷರಸರಣಿಗಳು_2);
        }
    }
    ಹಿಂತಿರುಗಿ *ಇದು;
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
Unicode_ಗಣ& Unicode_ಗಣ::removeAll(ಸ್ಥಿರಾಂಕ Unicode_ಗಣ& c) U_LIFETIME_BOUND {
    ಆದರೆ (isFrozen() || isBogus()) {
        ಹಿಂತಿರುಗಿ *ಇದು;
    }
    retain(c.list, c.len, 2);
    ಆದರೆ (has_ಅಕ್ಷರಸರಣಿಗಳು() && c.has_ಅಕ್ಷರಸರಣಿಗಳು()) {
        ಅಕ್ಷರಸರಣಿಗಳು_2->removeAll(*c.ಅಕ್ಷರಸರಣಿಗಳು_2);
    }
    ಹಿಂತಿರುಗಿ *ಇದು;
}

/**
 * Complements in this set all elements contained in the specified
 * set.  Any character in the other set will be removed if it is
 * in this set, or will be added if it is not in this set.
 *
 * @param c set that defines which elements will be xor'ed from
 *          this set.
 */
Unicode_ಗಣ& Unicode_ಗಣ::complementAll(ಸ್ಥಿರಾಂಕ Unicode_ಗಣ& c) U_LIFETIME_BOUND {
    ಆದರೆ (isFrozen() || isBogus()) {
        ಹಿಂತಿರುಗಿ *ಇದು;
    }
    exclusiveOr(c.list, c.len, 0);

    ಆದರೆ (c.ಅಕ್ಷರಸರಣಿಗಳು_2 != ಶೂನ್ಯಸೂಚಕ) {
        ಗಾಗಿ (int32_t i=0; i<c.ಅಕ್ಷರಸರಣಿಗಳು_2->size(); ++i) {
            ಶೂನ್ಯ* e = c.ಅಕ್ಷರಸರಣಿಗಳು_2->elementAt(i);
            ಆದರೆ (ಅಕ್ಷರಸರಣಿಗಳು_2 == ಶೂನ್ಯಸೂಚಕ || !ಅಕ್ಷರಸರಣಿಗಳು_2->removeElement(e)) {
                ಕೂಡಿಸು_2(*static_cast<ಸ್ಥಿರಾಂಕ UnicodeString*>(e));
            }
        }
    }
    ಹಿಂತಿರುಗಿ *ಇದು;
}

/**
 * Removes all of the elements from this set.  This set will be
 * empty after this call returns.
 */
Unicode_ಗಣ& Unicode_ಗಣ::clear() U_LIFETIME_BOUND {
    ಆದರೆ (isFrozen()) {
        ಹಿಂತಿರುಗಿ *ಇದು;
    }
    list[0] = UNICODESET_ಮೇಲಿನ;
    len = 1;
    release_ನಮೂನೆ();
    ಆದರೆ (ಅಕ್ಷರಸರಣಿಗಳು_2 != ಶೂನ್ಯಸೂಚಕ) {
        ಅಕ್ಷರಸರಣಿಗಳು_2->removeAllElements();
    }
    // Remove bogus
    fFlags = 0;
    ಹಿಂತಿರುಗಿ *ಇದು;
}

/**
 * Iteration method that returns the number of ranges contained in
 * this set.
 * @see #getRangeStart
 * @see #getRangeEnd
 */
int32_t Unicode_ಗಣ::get_Range_ಎಣಿಕೆ() ಸ್ಥಿರಾಂಕ {
    ಹಿಂತಿರುಗಿ len/2;
}

/**
 * Iteration method that returns the first character in the
 * specified range of this set.
 * @see #getRangeCount
 * @see #getRangeEnd
 */
UChar32 Unicode_ಗಣ::get_Range_ಆರಂಭ(int32_t ಸೂಚ್ಯಂಕ) ಸ್ಥಿರಾಂಕ {
    ಹಿಂತಿರುಗಿ list[ಸೂಚ್ಯಂಕ*2];
}

/**
 * Iteration method that returns the last character in the
 * specified range of this set.
 * @see #getRangeStart
 * @see #getRangeEnd
 */
UChar32 Unicode_ಗಣ::get_Range_ಕೊನೆ(int32_t ಸೂಚ್ಯಂಕ) ಸ್ಥಿರಾಂಕ {
    ಹಿಂತಿರುಗಿ list[ಸೂಚ್ಯಂಕ*2 + 1] - 1;
}

ಸ್ಥಿರಾಂಕ UnicodeString* Unicode_ಗಣ::getString(int32_t ಸೂಚ್ಯಂಕ) ಸ್ಥಿರಾಂಕ U_LIFETIME_BOUND {
    ಹಿಂತಿರುಗಿ static_cast<ಸ್ಥಿರಾಂಕ UnicodeString*>(ಅಕ್ಷರಸರಣಿಗಳು_2->elementAt(ಸೂಚ್ಯಂಕ));
}

/**
 * Reallocate this objects internal structures to take up the least
 * possible space, without changing this object's value.
 */
Unicode_ಗಣ& Unicode_ಗಣ::compact() U_LIFETIME_BOUND {
    ಆದರೆ (isFrozen() || isBogus()) {
        ಹಿಂತಿರುಗಿ *ಇದು;
    }
    // Delete buffer first to defragment memory less.
    ಆದರೆ (ಬಫರ್ != ರಾಶಿ_List) {
        uprv_free(ಬಫರ್);
        ಬಫರ್ = ಶೂನ್ಯಸೂಚಕ;
        ಬಫರ್_ಸಾಮರ್ಥ್ಯ = 0;
    }
    ಆದರೆ (list == ರಾಶಿ_List) {
        // pass
    } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (len <= INITIAL_ಸಾಮರ್ಥ್ಯ) {
        uprv_memcpy(ರಾಶಿ_List, list, len * ಗಾತ್ರ(UChar32));
        uprv_free(list);
        list = ರಾಶಿ_List;
        capacity = INITIAL_ಸಾಮರ್ಥ್ಯ;
    } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ ((len + 7) < capacity) {
        // If we have more than a little unused capacity, shrink it to len.
        UChar32* ತಾತ್ಕಾಲಿಕ = static_cast<UChar32*>(uprv_realloc(list, ಗಾತ್ರ(UChar32) * len));
        ಆದರೆ (ತಾತ್ಕಾಲಿಕ) {
            list = ತಾತ್ಕಾಲಿಕ;
            capacity = len;
        }
        // else what the heck happened?! We allocated less memory!
        // Oh well. We'll keep our original array.
    }
    ಆದರೆ (ಅಕ್ಷರಸರಣಿಗಳು_2 != ಶೂನ್ಯಸೂಚಕ && ಅಕ್ಷರಸರಣಿಗಳು_2->is_ಖಾಲಿಗಳು()) {
        ಅಳಿಸು ಅಕ್ಷರಸರಣಿಗಳು_2;
        ಅಕ್ಷರಸರಣಿಗಳು_2 = ಶೂನ್ಯಸೂಚಕ;
    }
    ಹಿಂತಿರುಗಿ *ಇದು;
}

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ DEBUG_SERIALIZE
#ಅನ್ತರ್ಭಾವ <stdio.h>
#ಆದರೆ_ಅಂತ್ಯ

/**
 * Deserialize constructor.
 */
Unicode_ಗಣ::Unicode_ಗಣ(ಸ್ಥಿರಾಂಕ uint16_t ದತ್ತಾಂಶ[], int32_t ದತ್ತಾಂಶ_Len, ESerialization serialization,
                       UError_ಸಂಕೇತ &ec) {

  ಆದರೆ(U_FAILURE(ec)) {
    ಗಣ_ಗಮ್ಯ_Bogus();
    ಹಿಂತಿರುಗಿ;
  }

  ಆದರೆ( (serialization != kSerialized)
      || (ದತ್ತಾಂಶ==ಶೂನ್ಯಸೂಚಕ)
      || (ದತ್ತಾಂಶ_Len < 1)) {
    ec = U_ILLEGAL_ವಾದ_ERROR;
    ಗಣ_ಗಮ್ಯ_Bogus();
    ಹಿಂತಿರುಗಿ;
  }

  // bmp?
  int32_t ಶೀರ್ಷಿಕೆ_ಗಾತ್ರ = ((ದತ್ತಾಂಶ[0]&0x8000)) ?2:1;
  int32_t bmp_ಉದ್ದಳತೆ = (ಶೀರ್ಷಿಕೆ_ಗಾತ್ರ==1)?ದತ್ತಾಂಶ[0]:ದತ್ತಾಂಶ[1];

  int32_t new_ಉದ್ದಳತೆ = (((ದತ್ತಾಂಶ[0]&0x7FFF)-bmp_ಉದ್ದಳತೆ)/2)+bmp_ಉದ್ದಳತೆ;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ DEBUG_SERIALIZE
  ಬರೆ("dataLen %d headerSize %d bmpLen %d len %d. data[0]=%X/%X/%X/%X\n", ದತ್ತಾಂಶ_Len,ಶೀರ್ಷಿಕೆ_ಗಾತ್ರ,bmp_ಉದ್ದಳತೆ,new_ಉದ್ದಳತೆ, ದತ್ತಾಂಶ[0],ದತ್ತಾಂಶ[1],ದತ್ತಾಂಶ[2],ದತ್ತಾಂಶ[3]);
#ಆದರೆ_ಅಂತ್ಯ
  ಆದರೆ(!ensure_ಸಾಮರ್ಥ್ಯ(new_ಉದ್ದಳತೆ + 1)) {  // +1 for HIGH
    ಹಿಂತಿರುಗಿ;
  }
  // copy bmp
  int32_t i;
  ಗಾಗಿ(i = 0; i< bmp_ಉದ್ದಳತೆ;i++) {
    list[i] = ದತ್ತಾಂಶ[i+ಶೀರ್ಷಿಕೆ_ಗಾತ್ರ];
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ DEBUG_SERIALIZE
    ಬರೆ("<<16@%d[%d] %X\n", i+ಶೀರ್ಷಿಕೆ_ಗಾತ್ರ, i, list[i]);
#ಆದರೆ_ಅಂತ್ಯ
  }
  // copy smp
  ಗಾಗಿ(i=bmp_ಉದ್ದಳತೆ;i<new_ಉದ್ದಳತೆ;i++) {
    list[i] = (static_cast<UChar32>(ದತ್ತಾಂಶ[ಶೀರ್ಷಿಕೆ_ಗಾತ್ರ + bmp_ಉದ್ದಳತೆ + (i - bmp_ಉದ್ದಳತೆ) * 2 + 0]) << 16) +
               static_cast<UChar32>(ದತ್ತಾಂಶ[ಶೀರ್ಷಿಕೆ_ಗಾತ್ರ + bmp_ಉದ್ದಳತೆ + (i - bmp_ಉದ್ದಳತೆ) * 2 + 1]);
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ DEBUG_SERIALIZE
    ಬರೆ("<<32@%d+[%d] %lX\n", ಶೀರ್ಷಿಕೆ_ಗಾತ್ರ+bmp_ಉದ್ದಳತೆ+i, i, list[i]);
#ಆದರೆ_ಅಂತ್ಯ
  }
  U_ASSERT(i == new_ಉದ್ದಳತೆ);
  ಆದರೆ (i == 0 || list[i - 1] != UNICODESET_ಮೇಲಿನ) {
    list[i++] = UNICODESET_ಮೇಲಿನ;
  }
  len = i;
}


int32_t Unicode_ಗಣ::serialize(uint16_t *dest, int32_t dest_ಸಾಮರ್ಥ್ಯ, UError_ಸಂಕೇತ& ec) ಸ್ಥಿರಾಂಕ {
    int32_t bmp_ಉದ್ದಳತೆ, length, dest_ಉದ್ದಳತೆ;

    ಆದರೆ (U_FAILURE(ec)) {
        ಹಿಂತಿರುಗಿ 0;
    }

    ಆದರೆ (dest_ಸಾಮರ್ಥ್ಯ<0 || (dest_ಸಾಮರ್ಥ್ಯ>0 && dest==ಶೂನ್ಯಸೂಚಕ)) {
        ec=U_ILLEGAL_ವಾದ_ERROR;
        ಹಿಂತಿರುಗಿ 0;
    }

    /* count necessary 16-bit units */
    length=ಇದು->len-1; // Subtract 1 to ignore final UNICODESET_HIGH
    // assert(length>=0);
    ಆದರೆ (length==0) {
        /* empty set */
        ಆದರೆ (dest_ಸಾಮರ್ಥ್ಯ>0) {
            *dest=0;
        } ಇಲ್ಲದಿದ್ದರೆ {
            ec=U_ಬಫರ್_ಅತಿಪ್ರವಾಹ_ERROR;
        }
        ಹಿಂತಿರುಗಿ 1;
    }
    /* now length>0 */

    ಆದರೆ (ಇದು->list[length-1]<=0xffff) {
        /* all BMP */
        bmp_ಉದ್ದಳತೆ=length;
    } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (ಇದು->list[0]>=0x10000) {
        /* all supplementary */
        bmp_ಉದ್ದಳತೆ=0;
        length*=2;
    } ಇಲ್ಲದಿದ್ದರೆ {
        /* some BMP, some supplementary */
        ಗಾಗಿ (bmp_ಉದ್ದಳತೆ=0; bmp_ಉದ್ದಳತೆ<length && ಇದು->list[bmp_ಉದ್ದಳತೆ]<=0xffff; ++bmp_ಉದ್ದಳತೆ) {}
        length=bmp_ಉದ್ದಳತೆ+2*(length-bmp_ಉದ್ದಳತೆ);
    }
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ DEBUG_SERIALIZE
    ಬರೆ(">> bmpLength%d length%d len%d\n", bmp_ಉದ್ದಳತೆ, length, len);
#ಆದರೆ_ಅಂತ್ಯ
    /* length: number of 16-bit array units */
    ಆದರೆ (length>0x7fff) {
        /* there are only 15 bits for the length in the first serialized word */
        ec=U_ಸೂಚ್ಯಂಕ_OUTOFBOUNDS_ERROR;
        ಹಿಂತಿರುಗಿ 0;
    }

    /*
     * total serialized length:
     * number of 16-bit array units (length) +
     * 1 length unit (always) +
     * 1 bmpLength unit (if there are supplementary values)
     */
    dest_ಉದ್ದಳತೆ=length+((length>bmp_ಉದ್ದಳತೆ)?2:1);
    ಆದರೆ (dest_ಉದ್ದಳತೆ<=dest_ಸಾಮರ್ಥ್ಯ) {
        ಸ್ಥಿರಾಂಕ UChar32 *p;
        int32_t i;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ DEBUG_SERIALIZE
        ಬರೆ("writeHdr\n");
#ಆದರೆ_ಅಂತ್ಯ
        *dest = static_cast<uint16_t>(length);
        ಆದರೆ (length>bmp_ಉದ್ದಳತೆ) {
            *dest|=0x8000;
            *++dest = static_cast<uint16_t>(bmp_ಉದ್ದಳತೆ);
        }
        ++dest;

        /* write the BMP part of the array */
        p=ಇದು->list;
        ಗಾಗಿ (i=0; i<bmp_ಉದ್ದಳತೆ; ++i) {
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ DEBUG_SERIALIZE
          ಬರೆ("writebmp: %x\n", (ಪೂರ್ಣಾಂಕ)*p);
#ಆದರೆ_ಅಂತ್ಯ
            *dest++ = static_cast<uint16_t>(*p++);
        }

        /* write the supplementary part of the array */
        ಗಾಗಿ (; i<length; i+=2) {
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ DEBUG_SERIALIZE
          ಬರೆ("write32: %x\n", (ಪೂರ್ಣಾಂಕ)*p);
#ಆದರೆ_ಅಂತ್ಯ
            *dest++ = static_cast<uint16_t>(*p >> 16);
            *dest++ = static_cast<uint16_t>(*p++);
        }
    } ಇಲ್ಲದಿದ್ದರೆ {
        ec=U_ಬಫರ್_ಅತಿಪ್ರವಾಹ_ERROR;
    }
    ಹಿಂತಿರುಗಿ dest_ಉದ್ದಳತೆ;
}

//----------------------------------------------------------------
// Implementation: Utility methods
//----------------------------------------------------------------

/**
 * Allocate our strings vector and return true if successful.
 */
UBool Unicode_ಗಣ::allocate_ಅಕ್ಷರಸರಣಿಗಳು(UError_ಸಂಕೇತ &ಸ್ಥಿತಿ_2) {
    ಆದರೆ (U_FAILURE(ಸ್ಥಿತಿ_2)) {
        ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
    }
    ಅಕ್ಷರಸರಣಿಗಳು_2 = ಹೊಸ UVector(uprv_deleteUObject,
                          uhash_compareUnicodeString, 1, ಸ್ಥಿತಿ_2);
    ಆದರೆ (ಅಕ್ಷರಸರಣಿಗಳು_2 == ಶೂನ್ಯಸೂಚಕ) { // Check for memory allocation error.
        ಸ್ಥಿತಿ_2 = U_ಸ್ಮರಣೆ_ಹಂಚಿಕೆ_ERROR;
        ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
    }
    ಆದರೆ (U_FAILURE(ಸ್ಥಿತಿ_2)) {
        ಅಳಿಸು ಅಕ್ಷರಸರಣಿಗಳು_2;
        ಅಕ್ಷರಸರಣಿಗಳು_2 = ಶೂನ್ಯಸೂಚಕ;
        ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
    } 
    ಹಿಂತಿರುಗಿ ನಿಜ;
}

int32_t Unicode_ಗಣ::ಮುಂದಿನ_ಸಾಮರ್ಥ್ಯ(int32_t min_ಸಾಮರ್ಥ್ಯ) {
    // Grow exponentially to reduce the frequency of allocations.
    ಆದರೆ (min_ಸಾಮರ್ಥ್ಯ < INITIAL_ಸಾಮರ್ಥ್ಯ) {
        ಹಿಂತಿರುಗಿ min_ಸಾಮರ್ಥ್ಯ + INITIAL_ಸಾಮರ್ಥ್ಯ;
    } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (min_ಸಾಮರ್ಥ್ಯ <= 2500) {
        ಹಿಂತಿರುಗಿ 5 * min_ಸಾಮರ್ಥ್ಯ;
    } ಇಲ್ಲದಿದ್ದರೆ {
        int32_t new_ಸಾಮರ್ಥ್ಯ = 2 * min_ಸಾಮರ್ಥ್ಯ;
        ಆದರೆ (new_ಸಾಮರ್ಥ್ಯ > MAX_ಉದ್ದಳತೆ) {
            new_ಸಾಮರ್ಥ್ಯ = MAX_ಉದ್ದಳತೆ;
        }
        ಹಿಂತಿರುಗಿ new_ಸಾಮರ್ಥ್ಯ;
    }
}

ತಾರ್ಕಿಕ Unicode_ಗಣ::ensure_ಸಾಮರ್ಥ್ಯ(int32_t newLen) {
    ಆದರೆ (newLen > MAX_ಉದ್ದಳತೆ) {
        newLen = MAX_ಉದ್ದಳತೆ;
    }
    ಆದರೆ (newLen <= capacity) {
        ಹಿಂತಿರುಗಿ ನಿಜ;
    }
    int32_t new_ಸಾಮರ್ಥ್ಯ = ಮುಂದಿನ_ಸಾಮರ್ಥ್ಯ(newLen);
    UChar32* ತಾತ್ಕಾಲಿಕ = static_cast<UChar32*>(uprv_malloc(new_ಸಾಮರ್ಥ್ಯ * ಗಾತ್ರ(UChar32)));
    ಆದರೆ (ತಾತ್ಕಾಲಿಕ == ಶೂನ್ಯಸೂಚಕ) {
        ಗಣ_ಗಮ್ಯ_Bogus(); // set the object to bogus state if an OOM failure occurred.
        ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
    }
    // Copy only the actual contents.
    uprv_memcpy(ತಾತ್ಕಾಲಿಕ, list, len * ಗಾತ್ರ(UChar32));
    ಆದರೆ (list != ರಾಶಿ_List) {
        uprv_free(list);
    }
    list = ತಾತ್ಕಾಲಿಕ;
    capacity = new_ಸಾಮರ್ಥ್ಯ;
    ಹಿಂತಿರುಗಿ ನಿಜ;
}

ತಾರ್ಕಿಕ Unicode_ಗಣ::ensure_ಬಫರ್_ಸಾಮರ್ಥ್ಯ(int32_t newLen) {
    ಆದರೆ (newLen > MAX_ಉದ್ದಳತೆ) {
        newLen = MAX_ಉದ್ದಳತೆ;
    }
    ಆದರೆ (newLen <= ಬಫರ್_ಸಾಮರ್ಥ್ಯ) {
        ಹಿಂತಿರುಗಿ ನಿಜ;
    }
    int32_t new_ಸಾಮರ್ಥ್ಯ = ಮುಂದಿನ_ಸಾಮರ್ಥ್ಯ(newLen);
    UChar32* ತಾತ್ಕಾಲಿಕ = static_cast<UChar32*>(uprv_malloc(new_ಸಾಮರ್ಥ್ಯ * ಗಾತ್ರ(UChar32)));
    ಆದರೆ (ತಾತ್ಕಾಲಿಕ == ಶೂನ್ಯಸೂಚಕ) {
        ಗಣ_ಗಮ್ಯ_Bogus();
        ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
    }
    // The buffer has no contents to be copied.
    // It is always filled from scratch after this call.
    ಆದರೆ (ಬಫರ್ != ರಾಶಿ_List) {
        uprv_free(ಬಫರ್);
    }
    ಬಫರ್ = ತಾತ್ಕಾಲಿಕ;
    ಬಫರ್_ಸಾಮರ್ಥ್ಯ = new_ಸಾಮರ್ಥ್ಯ;
    ಹಿಂತಿರುಗಿ ನಿಜ;
}

/**
 * Swap list and buffer.
 */
ಶೂನ್ಯ Unicode_ಗಣ::ಅದಲುಬದಲು_Buffers() {
    // swap list and buffer
    UChar32* ತಾತ್ಕಾಲಿಕ = list;
    list = ಬಫರ್;
    ಬಫರ್ = ತಾತ್ಕಾಲಿಕ;

    int32_t c = capacity;
    capacity = ಬಫರ್_ಸಾಮರ್ಥ್ಯ;
    ಬಫರ್_ಸಾಮರ್ಥ್ಯ = c;
}

ಶೂನ್ಯ Unicode_ಗಣ::ಗಣ_ಗಮ್ಯ_Bogus() {
    clear(); // Remove everything in the set.
    fFlags = kIsBogus;
}

//----------------------------------------------------------------
// Implementation: Fundamental operators
//----------------------------------------------------------------

ಸ್ಥಿರ ಸಾಲೊಳಗೆ UChar32 max(UChar32 a, UChar32 b) {
    ಹಿಂತಿರುಗಿ (a > b) ? a : b;
}

// polarity = 0, 3 is normal: x xor y
// polarity = 1, 2: x xor ~y == x === y

ಶೂನ್ಯ Unicode_ಗಣ::exclusiveOr(ಸ್ಥಿರಾಂಕ UChar32* other, int32_t otherLen, int8_t polarity) {
    ಆದರೆ (isFrozen() || isBogus()) {
        ಹಿಂತಿರುಗಿ;
    }
    ಆದರೆ (!ensure_ಬಫರ್_ಸಾಮರ್ಥ್ಯ(len + otherLen)) {
        ಹಿಂತಿರುಗಿ;
    }

    int32_t i = 0, j = 0, k = 0;
    UChar32 a = list[i++];
    UChar32 b;
    ಆದರೆ (polarity == 1 || polarity == 2) {
        b = UNICODESET_ಕೆಳಗಿನ;
        ಆದರೆ (other[j] == UNICODESET_ಕೆಳಗಿನ) { // skip base if already LOW
            ++j;
            b = other[j];
        }
    } ಇಲ್ಲದಿದ್ದರೆ {
        b = other[j++];
    }
    // simplest of all the routines
    // sort the values, discarding identicals!
    ಗಾಗಿ (;;) {
        ಆದರೆ (a < b) {
            ಬಫರ್[k++] = a;
            a = list[i++];
        } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (b < a) {
            ಬಫರ್[k++] = b;
            b = other[j++];
        } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (a != UNICODESET_ಮೇಲಿನ) { // at this point, a == b
            // discard both values!
            a = list[i++];
            b = other[j++];
        } ಇಲ್ಲದಿದ್ದರೆ { // DONE!
            ಬಫರ್[k++] = UNICODESET_ಮೇಲಿನ;
            len = k;
            ನಿಲ್ಲಿಸು;
        }
    }
    ಅದಲುಬದಲು_Buffers();
    release_ನಮೂನೆ();
}

// polarity = 0 is normal: x union y
// polarity = 2: x union ~y
// polarity = 1: ~x union y
// polarity = 3: ~x union ~y

ಶೂನ್ಯ Unicode_ಗಣ::ಕೂಡಿಸು(ಸ್ಥಿರಾಂಕ UChar32* other, int32_t otherLen, int8_t polarity) {
    ಆದರೆ (isFrozen() || isBogus() || other==ಶೂನ್ಯಸೂಚಕ) {
        ಹಿಂತಿರುಗಿ;
    }
    ಆದರೆ (!ensure_ಬಫರ್_ಸಾಮರ್ಥ್ಯ(len + otherLen)) {
        ಹಿಂತಿರುಗಿ;
    }

    int32_t i = 0, j = 0, k = 0;
    UChar32 a = list[i++];
    UChar32 b = other[j++];
    // change from xor is that we have to check overlapping pairs
    // polarity bit 1 means a is second, bit 2 means b is.
    ಗಾಗಿ (;;) {
        ಆಯ್ಕೆ (polarity) {
          ಸ್ಥಿತಿ 0: // both first; take lower if unequal
            ಆದರೆ (a < b) { // take a
                // Back up over overlapping ranges in buffer[]
                ಆದರೆ (k > 0 && a <= ಬಫರ್[k-1]) {
                    // Pick latter end value in buffer[] vs. list[]
                    a = max(list[i], ಬಫರ್[--k]);
                } ಇಲ್ಲದಿದ್ದರೆ {
                    // No overlap
                    ಬಫರ್[k++] = a;
                    a = list[i];
                }
                i++; // Common if/else code factored out
                polarity ^= 1;
            } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (b < a) { // take b
                ಆದರೆ (k > 0 && b <= ಬಫರ್[k-1]) {
                    b = max(other[j], ಬಫರ್[--k]);
                } ಇಲ್ಲದಿದ್ದರೆ {
                    ಬಫರ್[k++] = b;
                    b = other[j];
                }
                j++;
                polarity ^= 2;
            } ಇಲ್ಲದಿದ್ದರೆ { // a == b, take a, drop b
                ಆದರೆ (a == UNICODESET_ಮೇಲಿನ) ಹೋಗು loop_ಕೊನೆ;
                // This is symmetrical; it doesn't matter if
                // we backtrack with a or b. - liu
                ಆದರೆ (k > 0 && a <= ಬಫರ್[k-1]) {
                    a = max(list[i], ಬಫರ್[--k]);
                } ಇಲ್ಲದಿದ್ದರೆ {
                    // No overlap
                    ಬಫರ್[k++] = a;
                    a = list[i];
                }
                i++;
                polarity ^= 1;
                b = other[j++];
                polarity ^= 2;
            }
            ನಿಲ್ಲಿಸು;
          ಸ್ಥಿತಿ 3: // both second; take higher if unequal, and drop other
            ಆದರೆ (b <= a) { // take a
                ಆದರೆ (a == UNICODESET_ಮೇಲಿನ) ಹೋಗು loop_ಕೊನೆ;
                ಬಫರ್[k++] = a;
            } ಇಲ್ಲದಿದ್ದರೆ { // take b
                ಆದರೆ (b == UNICODESET_ಮೇಲಿನ) ಹೋಗು loop_ಕೊನೆ;
                ಬಫರ್[k++] = b;
            }
            a = list[i++];
            polarity ^= 1;   // factored common code
            b = other[j++];
            polarity ^= 2;
            ನಿಲ್ಲಿಸು;
          ಸ್ಥಿತಿ 1: // a second, b first; if b < a, overlap
            ಆದರೆ (a < b) { // no overlap, take a
                ಬಫರ್[k++] = a; a = list[i++]; polarity ^= 1;
            } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (b < a) { // OVERLAP, drop b
                b = other[j++];
                polarity ^= 2;
            } ಇಲ್ಲದಿದ್ದರೆ { // a == b, drop both!
                ಆದರೆ (a == UNICODESET_ಮೇಲಿನ) ಹೋಗು loop_ಕೊನೆ;
                a = list[i++];
                polarity ^= 1;
                b = other[j++];
                polarity ^= 2;
            }
            ನಿಲ್ಲಿಸು;
          ಸ್ಥಿತಿ 2: // a first, b second; if a < b, overlap
            ಆದರೆ (b < a) { // no overlap, take b
                ಬಫರ್[k++] = b;
                b = other[j++];
                polarity ^= 2;
            } ಇಲ್ಲದಿದ್ದರೆ  ಆದರೆ (a < b) { // OVERLAP, drop a
                a = list[i++];
                polarity ^= 1;
            } ಇಲ್ಲದಿದ್ದರೆ { // a == b, drop both!
                ಆದರೆ (a == UNICODESET_ಮೇಲಿನ) ಹೋಗು loop_ಕೊನೆ;
                a = list[i++];
                polarity ^= 1;
                b = other[j++];
                polarity ^= 2;
            }
            ನಿಲ್ಲಿಸು;
        }
    }
 loop_ಕೊನೆ:
    ಬಫರ್[k++] = UNICODESET_ಮೇಲಿನ;    // terminate
    len = k;
    ಅದಲುಬದಲು_Buffers();
    release_ನಮೂನೆ();
}

// polarity = 0 is normal: x intersect y
// polarity = 2: x intersect ~y == set-minus
// polarity = 1: ~x intersect y
// polarity = 3: ~x intersect ~y

ಶೂನ್ಯ Unicode_ಗಣ::retain(ಸ್ಥಿರಾಂಕ UChar32* other, int32_t otherLen, int8_t polarity) {
    ಆದರೆ (isFrozen() || isBogus()) {
        ಹಿಂತಿರುಗಿ;
    }
    ಆದರೆ (!ensure_ಬಫರ್_ಸಾಮರ್ಥ್ಯ(len + otherLen)) {
        ಹಿಂತಿರುಗಿ;
    }

    int32_t i = 0, j = 0, k = 0;
    UChar32 a = list[i++];
    UChar32 b = other[j++];
    // change from xor is that we have to check overlapping pairs
    // polarity bit 1 means a is second, bit 2 means b is.
    ಗಾಗಿ (;;) {
        ಆಯ್ಕೆ (polarity) {
          ಸ್ಥಿತಿ 0: // both first; drop the smaller
            ಆದರೆ (a < b) { // drop a
                a = list[i++];
                polarity ^= 1;
            } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (b < a) { // drop b
                b = other[j++];
                polarity ^= 2;
            } ಇಲ್ಲದಿದ್ದರೆ { // a == b, take one, drop other
                ಆದರೆ (a == UNICODESET_ಮೇಲಿನ) ಹೋಗು loop_ಕೊನೆ;
                ಬಫರ್[k++] = a;
                a = list[i++];
                polarity ^= 1;
                b = other[j++];
                polarity ^= 2;
            }
            ನಿಲ್ಲಿಸು;
          ಸ್ಥಿತಿ 3: // both second; take lower if unequal
            ಆದರೆ (a < b) { // take a
                ಬಫರ್[k++] = a;
                a = list[i++];
                polarity ^= 1;
            } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (b < a) { // take b
                ಬಫರ್[k++] = b;
                b = other[j++];
                polarity ^= 2;
            } ಇಲ್ಲದಿದ್ದರೆ { // a == b, take one, drop other
                ಆದರೆ (a == UNICODESET_ಮೇಲಿನ) ಹೋಗು loop_ಕೊನೆ;
                ಬಫರ್[k++] = a;
                a = list[i++];
                polarity ^= 1;
                b = other[j++];
                polarity ^= 2;
            }
            ನಿಲ್ಲಿಸು;
          ಸ್ಥಿತಿ 1: // a second, b first;
            ಆದರೆ (a < b) { // NO OVERLAP, drop a
                a = list[i++];
                polarity ^= 1;
            } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (b < a) { // OVERLAP, take b
                ಬಫರ್[k++] = b;
                b = other[j++];
                polarity ^= 2;
            } ಇಲ್ಲದಿದ್ದರೆ { // a == b, drop both!
                ಆದರೆ (a == UNICODESET_ಮೇಲಿನ) ಹೋಗು loop_ಕೊನೆ;
                a = list[i++];
                polarity ^= 1;
                b = other[j++];
                polarity ^= 2;
            }
            ನಿಲ್ಲಿಸು;
          ಸ್ಥಿತಿ 2: // a first, b second; if a < b, overlap
            ಆದರೆ (b < a) { // no overlap, drop b
                b = other[j++];
                polarity ^= 2;
            } ಇಲ್ಲದಿದ್ದರೆ  ಆದರೆ (a < b) { // OVERLAP, take a
                ಬಫರ್[k++] = a;
                a = list[i++];
                polarity ^= 1;
            } ಇಲ್ಲದಿದ್ದರೆ { // a == b, drop both!
                ಆದರೆ (a == UNICODESET_ಮೇಲಿನ) ಹೋಗು loop_ಕೊನೆ;
                a = list[i++];
                polarity ^= 1;
                b = other[j++];
                polarity ^= 2;
            }
            ನಿಲ್ಲಿಸು;
        }
    }
 loop_ಕೊನೆ:
    ಬಫರ್[k++] = UNICODESET_ಮೇಲಿನ;    // terminate
    len = k;
    ಅದಲುಬದಲು_Buffers();
    release_ನಮೂನೆ();
}

/**
 * Append the <code>toPattern()</code> representation of a
 * string to the given <code>StringBuffer</code>.
 */
ಶೂನ್ಯ Unicode_ಗಣ::append_ಗಮ್ಯ_Pat(UnicodeString& buf, ಸ್ಥಿರಾಂಕ UnicodeString& s, UBool escapeUnprintable) {
    UChar32 cp;
    ಗಾಗಿ (int32_t i = 0; i < s.length(); i += U16_ಉದ್ದಳತೆ(cp)) {
        append_ಗಮ್ಯ_Pat(buf, cp = s.char32At(i), escapeUnprintable);
    }
}

/**
 * Append the <code>toPattern()</code> representation of a
 * character to the given <code>StringBuffer</code>.
 */
ಶೂನ್ಯ Unicode_ಗಣ::append_ಗಮ್ಯ_Pat(UnicodeString& buf, UChar32 c, UBool escapeUnprintable) {
    ಆದರೆ (escapeUnprintable ? ICU_Utility::isUnprintable(c) : ICU_Utility::shouldAlwaysBeEscaped(c)) {
        // Use hex escape notation (\uxxxx or \Uxxxxxxxx) for anything
        // unprintable
        ICU_Utility::escape(buf, c);
        ಹಿಂತಿರುಗಿ;
    }
    // Okay to let ':' pass through
    ಆಯ್ಕೆ (c) {
    ಸ್ಥಿತಿ u'[':
    ಸ್ಥಿತಿ u']':
    ಸ್ಥಿತಿ u'-':
    ಸ್ಥಿತಿ u'^':
    ಸ್ಥಿತಿ u'&':
    ಸ್ಥಿತಿ u'\\':
    ಸ್ಥಿತಿ u'{':
    ಸ್ಥಿತಿ u'}':
    ಸ್ಥಿತಿ u':':
    ಸ್ಥಿತಿ ಚಿಹ್ನೆ_ಕೋಷ್ಟಕ::ಚಿಹ್ನೆ_REF:
        buf.append(u'\\');
        ನಿಲ್ಲಿಸು;
    ಪೂರ್ವನಿಯೋಜಿತ:
        // Escape whitespace
        ಆದರೆ (ನಮೂನೆ_Props::isWhiteSpace(c)) {
            buf.append(u'\\');
        }
        ನಿಲ್ಲಿಸು;
    }
    buf.append(c);
}

ಶೂನ್ಯ Unicode_ಗಣ::append_ಗಮ್ಯ_Pat(UnicodeString &ಫಲಿತಾಂಶ, UChar32 ಆರಂಭ, UChar32 ಕೊನೆ,
                              UBool escapeUnprintable) {
    append_ಗಮ್ಯ_Pat(ಫಲಿತಾಂಶ, ಆರಂಭ, escapeUnprintable);
    ಆದರೆ (ಆರಂಭ != ಕೊನೆ) {
        ಆದರೆ ((ಆರಂಭ+1) != ಕೊನೆ ||
                // Avoid writing what looks like a lead+trail surrogate pair.
                ಆರಂಭ == 0xdbff) {
            ಫಲಿತಾಂಶ.append(u'-');
        }
        append_ಗಮ್ಯ_Pat(ಫಲಿತಾಂಶ, ಕೊನೆ, escapeUnprintable);
    }
}

/**
 * Append a string representation of this set to result.  This will be
 * a cleaned version of the string passed to applyPattern(), if there
 * is one.  Otherwise it will be generated.
 */
UnicodeString& Unicode_ಗಣ::ಗಮ್ಯ_ನಮೂನೆ_2(UnicodeString& ಫಲಿತಾಂಶ U_LIFETIME_BOUND,
                                      UBool escapeUnprintable) ಸ್ಥಿರಾಂಕ
{
    ಆದರೆ (pat != ಶೂನ್ಯಸೂಚಕ) {
        int32_t i;
        int32_t backslash_ಎಣಿಕೆ = 0;
        ಗಾಗಿ (i=0; i<patLen; ) {
            UChar32 c;
            U16_ಮುಂದಿನ(pat, i, patLen, c);
            ಆದರೆ (escapeUnprintable ?
                    ICU_Utility::isUnprintable(c) : ICU_Utility::shouldAlwaysBeEscaped(c)) {
                // If the unprintable character is preceded by an odd
                // number of backslashes, then it has been escaped.
                // Before unescaping it, we delete the final
                // backslash.
                ಆದರೆ ((backslash_ಎಣಿಕೆ % 2) == 1) {
                    ಫಲಿತಾಂಶ.truncate(ಫಲಿತಾಂಶ.length() - 1);
                }
                ICU_Utility::escape(ಫಲಿತಾಂಶ, c);
                backslash_ಎಣಿಕೆ = 0;
            } ಇಲ್ಲದಿದ್ದರೆ {
                ಫಲಿತಾಂಶ.append(c);
                ಆದರೆ (c == u'\\') {
                    ++backslash_ಎಣಿಕೆ;
                } ಇಲ್ಲದಿದ್ದರೆ {
                    backslash_ಎಣಿಕೆ = 0;
                }
            }
        }
        ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
    }

    ಹಿಂತಿರುಗಿ generate_ನಮೂನೆ(ಫಲಿತಾಂಶ, escapeUnprintable);
}

/**
 * Returns a string representation of this set.  If the result of
 * calling this function is passed to a UnicodeSet constructor, it
 * will produce another set that is equal to this one.
 */
UnicodeString& Unicode_ಗಣ::ಗಮ್ಯ_ನಮೂನೆ(UnicodeString& ಫಲಿತಾಂಶ U_LIFETIME_BOUND,
                                     UBool escapeUnprintable) ಸ್ಥಿರಾಂಕ
{
    ಫಲಿತಾಂಶ.truncate(0);
    ಹಿಂತಿರುಗಿ ಗಮ್ಯ_ನಮೂನೆ_2(ಫಲಿತಾಂಶ, escapeUnprintable);
}

/**
 * Generate and append a string representation of this set to result.
 * This does not use this.pat, the cleaned up copy of the string
 * passed to applyPattern().
 */
UnicodeString& Unicode_ಗಣ::generate_ನಮೂನೆ(UnicodeString& ಫಲಿತಾಂಶ U_LIFETIME_BOUND,
                                            UBool escapeUnprintable) ಸ್ಥಿರಾಂಕ
{
    ಫಲಿತಾಂಶ.append(u'[');

    int32_t i = 0;
    int32_t ಎಲ್ಲೆ = len & ~1;  // = 2 * getRangeCount()

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
    ಆದರೆ (len >= 4 && list[0] == 0 && ಎಲ್ಲೆ == len && !has_ಅಕ್ಷರಸರಣಿಗಳು()) {
        // Emit the inverse
        ಫಲಿತಾಂಶ.append(u'^');
        // Offsetting the inversion list index by one lets us
        // iterate over the ranges of the set complement.
        i = 1;
        --ಎಲ್ಲೆ;
    }

    // Emit the ranges as pairs.
    ಆಗ (i < ಎಲ್ಲೆ) {
        UChar32 ಆರಂಭ = list[i];  // getRangeStart()
        UChar32 ಕೊನೆ = list[i + 1] - 1;  // getRangeEnd() = range limit minus one
        ಆದರೆ (!(0xd800 <= ಕೊನೆ && ಕೊನೆ <= 0xdbff)) {
            append_ಗಮ್ಯ_Pat(ಫಲಿತಾಂಶ, ಆರಂಭ, ಕೊನೆ, escapeUnprintable);
            i += 2;
        } ಇಲ್ಲದಿದ್ದರೆ {
            // The range ends with a lead surrogate.
            // Avoid writing what looks like a lead+trail surrogate pair.
            // 1. Postpone ranges that start with a lead surrogate code point.
            int32_t ಮೊದಲ_Lead = i;
            ಆಗ ((i += 2) < ಎಲ್ಲೆ && list[i] <= 0xdbff) {}
            int32_t ಮೊದಲ_After_Lead = i;
            // 2. Write following ranges that start with a trail surrogate code point.
            ಆಗ (i < ಎಲ್ಲೆ && (ಆರಂಭ = list[i]) <= 0xdfff) {
                append_ಗಮ್ಯ_Pat(ಫಲಿತಾಂಶ, ಆರಂಭ, list[i + 1] - 1, escapeUnprintable);
                i += 2;
            }
            // 3. Now write the postponed ranges.
            ಗಾಗಿ (ಪೂರ್ಣಾಂಕ j = ಮೊದಲ_Lead; j < ಮೊದಲ_After_Lead; j += 2) {
                append_ಗಮ್ಯ_Pat(ಫಲಿತಾಂಶ, list[j], list[j + 1] - 1, escapeUnprintable);
            }
        }
    }

    ಆದರೆ (ಅಕ್ಷರಸರಣಿಗಳು_2 != ಶೂನ್ಯಸೂಚಕ) {
        ಗಾಗಿ (int32_t i = 0; i<ಅಕ್ಷರಸರಣಿಗಳು_2->size(); ++i) {
            ಫಲಿತಾಂಶ.append(u'{');
            append_ಗಮ್ಯ_Pat(ಫಲಿತಾಂಶ,
                         *static_cast<ಸ್ಥಿರಾಂಕ UnicodeString*>(ಅಕ್ಷರಸರಣಿಗಳು_2->elementAt(i)),
                         escapeUnprintable);
            ಫಲಿತಾಂಶ.append(u'}');
        }
    }
    ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ.append(u']');
}

/**
* Release existing cached pattern
*/
ಶೂನ್ಯ Unicode_ಗಣ::release_ನಮೂನೆ() {
    ಆದರೆ (pat) {
        uprv_free(pat);
        pat = ಶೂನ್ಯಸೂಚಕ;
        patLen = 0;
    }
}

/**
* Set the new pattern to cache.
*/
ಶೂನ್ಯ Unicode_ಗಣ::ಗಣ_ನಮೂನೆ(ಸ್ಥಿರಾಂಕ char16_t *newPat, int32_t newPatLen) {
    release_ನಮೂನೆ();
    pat = static_cast<char16_t*>(uprv_malloc((newPatLen + 1) * ಗಾತ್ರ(char16_t)));
    ಆದರೆ (pat) {
        patLen = newPatLen;
        u_memcpy(pat, newPat, patLen);
        pat[patLen] = 0;
    }
    // else we don't care if malloc failed. This was just a nice cache.
    // We can regenerate an equivalent pattern later when requested.
}

Unicode_ಗಣ* Unicode_ಗಣ::freeze() U_LIFETIME_BOUND {
    ಆದರೆ(!isFrozen() && !isBogus()) {
        compact();

        // Optimize contains() and span() and similar functions.
        ಆದರೆ (has_ಅಕ್ಷರಸರಣಿಗಳು()) {
            string_ಹರವು = ಹೊಸ Unicode_ಗಣ_String_ಹರವು(*ಇದು, *ಅಕ್ಷರಸರಣಿಗಳು_2, Unicode_ಗಣ_String_ಹರವು::ALL);
            ಆದರೆ (string_ಹರವು == ಶೂನ್ಯಸೂಚಕ) {
                ಗಣ_ಗಮ್ಯ_Bogus();
                ಹಿಂತಿರುಗಿ ಇದು;
            } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (!string_ಹರವು->needs_String_ಹರವು_UTF16()) {
                // All strings are irrelevant for span() etc. because
                // all of each string's code points are contained in this set.
                // Do not check needsStringSpanUTF8() because UTF-8 has at most as
                // many relevant strings as UTF-16.
                // (Thus needsStringSpanUTF8() implies needsStringSpanUTF16().)
                ಅಳಿಸು string_ಹರವು;
                string_ಹರವು = ಶೂನ್ಯಸೂಚಕ;
            }
        }
        ಆದರೆ (string_ಹರವು == ಶೂನ್ಯಸೂಚಕ) {
            // No span-relevant strings: Optimize for code point spans.
            bmp_ಗಣ=ಹೊಸ BMPSet(list, len);
            ಆದರೆ (bmp_ಗಣ == ಶೂನ್ಯಸೂಚಕ) { // Check for memory allocation error.
                ಗಣ_ಗಮ್ಯ_Bogus();
            }
        }
    }
    ಹಿಂತಿರುಗಿ ಇದು;
}

int32_t Unicode_ಗಣ::ಹರವು(ಸ್ಥಿರಾಂಕ char16_t *s, int32_t length, USet_ಹರವು_Condition ಹರವು_Condition) ಸ್ಥಿರಾಂಕ {
    ಆದರೆ(length>0 && bmp_ಗಣ!=ಶೂನ್ಯಸೂಚಕ) {
        ಹಿಂತಿರುಗಿ static_cast<int32_t>(bmp_ಗಣ->ಹರವು(s, s + length, ಹರವು_Condition) - s);
    }
    ಆದರೆ(length<0) {
        length=u_strlen(s);
    }
    ಆದರೆ(length==0) {
        ಹಿಂತಿರುಗಿ 0;
    }
    ಆದರೆ(string_ಹರವು!=ಶೂನ್ಯಸೂಚಕ) {
        ಹಿಂತಿರುಗಿ string_ಹರವು->ಹರವು(s, length, ಹರವು_Condition);
    } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(has_ಅಕ್ಷರಸರಣಿಗಳು()) {
        uint32_t which= ಹರವು_Condition==USET_ಹರವು_NOT_CONTAINED ?
                            Unicode_ಗಣ_String_ಹರವು::FWD_UTF16_NOT_CONTAINED :
                            Unicode_ಗಣ_String_ಹರವು::FWD_UTF16_CONTAINED;
        Unicode_ಗಣ_String_ಹರವು str_ಹರವು(*ಇದು, *ಅಕ್ಷರಸರಣಿಗಳು_2, which);
        ಆದರೆ(str_ಹರವು.needs_String_ಹರವು_UTF16()) {
            ಹಿಂತಿರುಗಿ str_ಹರವು.ಹರವು(s, length, ಹರವು_Condition);
        }
    }

    ಆದರೆ(ಹರವು_Condition!=USET_ಹರವು_NOT_CONTAINED) {
        ಹರವು_Condition=USET_ಹರವು_CONTAINED;  // Pin to 0/1 values.
    }

    UChar32 c;
    int32_t ಆರಂಭ=0, prev=0;
    ಮಾಡು {
        U16_ಮುಂದಿನ(s, ಆರಂಭ, length, c);
        ಆದರೆ(ಹರವು_Condition!=ಒಳಗೊಂಡಿದೆ(c)) {
            ನಿಲ್ಲಿಸು;
        }
    } ಆಗ((prev=ಆರಂಭ)<length);
    ಹಿಂತಿರುಗಿ prev;
}

int32_t Unicode_ಗಣ::ಹರವು_ಹಿಂಭಾಗ(ಸ್ಥಿರಾಂಕ char16_t *s, int32_t length, USet_ಹರವು_Condition ಹರವು_Condition) ಸ್ಥಿರಾಂಕ {
    ಆದರೆ(length>0 && bmp_ಗಣ!=ಶೂನ್ಯಸೂಚಕ) {
        ಹಿಂತಿರುಗಿ static_cast<int32_t>(bmp_ಗಣ->ಹರವು_ಹಿಂಭಾಗ(s, s + length, ಹರವು_Condition) - s);
    }
    ಆದರೆ(length<0) {
        length=u_strlen(s);
    }
    ಆದರೆ(length==0) {
        ಹಿಂತಿರುಗಿ 0;
    }
    ಆದರೆ(string_ಹರವು!=ಶೂನ್ಯಸೂಚಕ) {
        ಹಿಂತಿರುಗಿ string_ಹರವು->ಹರವು_ಹಿಂಭಾಗ(s, length, ಹರವು_Condition);
    } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(has_ಅಕ್ಷರಸರಣಿಗಳು()) {
        uint32_t which= ಹರವು_Condition==USET_ಹರವು_NOT_CONTAINED ?
                            Unicode_ಗಣ_String_ಹರವು::ಹಿಂಭಾಗ_UTF16_NOT_CONTAINED :
                            Unicode_ಗಣ_String_ಹರವು::ಹಿಂಭಾಗ_UTF16_CONTAINED;
        Unicode_ಗಣ_String_ಹರವು str_ಹರವು(*ಇದು, *ಅಕ್ಷರಸರಣಿಗಳು_2, which);
        ಆದರೆ(str_ಹರವು.needs_String_ಹರವು_UTF16()) {
            ಹಿಂತಿರುಗಿ str_ಹರವು.ಹರವು_ಹಿಂಭಾಗ(s, length, ಹರವು_Condition);
        }
    }

    ಆದರೆ(ಹರವು_Condition!=USET_ಹರವು_NOT_CONTAINED) {
        ಹರವು_Condition=USET_ಹರವು_CONTAINED;  // Pin to 0/1 values.
    }

    UChar32 c;
    int32_t prev=length;
    ಮಾಡು {
        U16_PREV(s, 0, length, c);
        ಆದರೆ(ಹರವು_Condition!=ಒಳಗೊಂಡಿದೆ(c)) {
            ನಿಲ್ಲಿಸು;
        }
    } ಆಗ((prev=length)>0);
    ಹಿಂತಿರುಗಿ prev;
}

int32_t Unicode_ಗಣ::ಹರವು_UTF8(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *s, int32_t length, USet_ಹರವು_Condition ಹರವು_Condition) ಸ್ಥಿರಾಂಕ {
    ಆದರೆ(length>0 && bmp_ಗಣ!=ಶೂನ್ಯಸೂಚಕ) {
        ಸ್ಥಿರಾಂಕ uint8_t* s0 = reinterpret_cast<ಸ್ಥಿರಾಂಕ uint8_t*>(s);
        ಹಿಂತಿರುಗಿ static_cast<int32_t>(bmp_ಗಣ->ಹರವು_UTF8(s0, length, ಹರವು_Condition) - s0);
    }
    ಆದರೆ(length<0) {
        length = static_cast<int32_t>(uprv_strlen(s));
    }
    ಆದರೆ(length==0) {
        ಹಿಂತಿರುಗಿ 0;
    }
    ಆದರೆ(string_ಹರವು!=ಶೂನ್ಯಸೂಚಕ) {
        ಹಿಂತಿರುಗಿ string_ಹರವು->ಹರವು_UTF8(reinterpret_cast<ಸ್ಥಿರಾಂಕ uint8_t*>(s), length, ಹರವು_Condition);
    } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(has_ಅಕ್ಷರಸರಣಿಗಳು()) {
        uint32_t which= ಹರವು_Condition==USET_ಹರವು_NOT_CONTAINED ?
                            Unicode_ಗಣ_String_ಹರವು::FWD_UTF8_NOT_CONTAINED :
                            Unicode_ಗಣ_String_ಹರವು::FWD_UTF8_CONTAINED;
        Unicode_ಗಣ_String_ಹರವು str_ಹರವು(*ಇದು, *ಅಕ್ಷರಸರಣಿಗಳು_2, which);
        ಆದರೆ(str_ಹರವು.needs_String_ಹರವು_UTF8()) {
            ಹಿಂತಿರುಗಿ str_ಹರವು.ಹರವು_UTF8(reinterpret_cast<ಸ್ಥಿರಾಂಕ uint8_t*>(s), length, ಹರವು_Condition);
        }
    }

    ಆದರೆ(ಹರವು_Condition!=USET_ಹರವು_NOT_CONTAINED) {
        ಹರವು_Condition=USET_ಹರವು_CONTAINED;  // Pin to 0/1 values.
    }

    UChar32 c;
    int32_t ಆರಂಭ=0, prev=0;
    ಮಾಡು {
        U8_ಮುಂದಿನ_OR_FFFD(s, ಆರಂಭ, length, c);
        ಆದರೆ(ಹರವು_Condition!=ಒಳಗೊಂಡಿದೆ(c)) {
            ನಿಲ್ಲಿಸು;
        }
    } ಆಗ((prev=ಆರಂಭ)<length);
    ಹಿಂತಿರುಗಿ prev;
}

int32_t Unicode_ಗಣ::ಹರವು_ಹಿಂಭಾಗ_UTF8(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *s, int32_t length, USet_ಹರವು_Condition ಹರವು_Condition) ಸ್ಥಿರಾಂಕ {
    ಆದರೆ(length>0 && bmp_ಗಣ!=ಶೂನ್ಯಸೂಚಕ) {
        ಸ್ಥಿರಾಂಕ uint8_t* s0 = reinterpret_cast<ಸ್ಥಿರಾಂಕ uint8_t*>(s);
        ಹಿಂತಿರುಗಿ bmp_ಗಣ->ಹರವು_ಹಿಂಭಾಗ_UTF8(s0, length, ಹರವು_Condition);
    }
    ಆದರೆ(length<0) {
        length = static_cast<int32_t>(uprv_strlen(s));
    }
    ಆದರೆ(length==0) {
        ಹಿಂತಿರುಗಿ 0;
    }
    ಆದರೆ(string_ಹರವು!=ಶೂನ್ಯಸೂಚಕ) {
        ಹಿಂತಿರುಗಿ string_ಹರವು->ಹರವು_ಹಿಂಭಾಗ_UTF8(reinterpret_cast<ಸ್ಥಿರಾಂಕ uint8_t*>(s), length, ಹರವು_Condition);
    } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(has_ಅಕ್ಷರಸರಣಿಗಳು()) {
        uint32_t which= ಹರವು_Condition==USET_ಹರವು_NOT_CONTAINED ?
                            Unicode_ಗಣ_String_ಹರವು::ಹಿಂಭಾಗ_UTF8_NOT_CONTAINED :
                            Unicode_ಗಣ_String_ಹರವು::ಹಿಂಭಾಗ_UTF8_CONTAINED;
        Unicode_ಗಣ_String_ಹರವು str_ಹರವು(*ಇದು, *ಅಕ್ಷರಸರಣಿಗಳು_2, which);
        ಆದರೆ(str_ಹರವು.needs_String_ಹರವು_UTF8()) {
            ಹಿಂತಿರುಗಿ str_ಹರವು.ಹರವು_ಹಿಂಭಾಗ_UTF8(reinterpret_cast<ಸ್ಥಿರಾಂಕ uint8_t*>(s), length, ಹರವು_Condition);
        }
    }

    ಆದರೆ(ಹರವು_Condition!=USET_ಹರವು_NOT_CONTAINED) {
        ಹರವು_Condition=USET_ಹರವು_CONTAINED;  // Pin to 0/1 values.
    }

    UChar32 c;
    int32_t prev=length;
    ಮಾಡು {
        U8_PREV_OR_FFFD(s, 0, length, c);
        ಆದರೆ(ಹರವು_Condition!=ಒಳಗೊಂಡಿದೆ(c)) {
            ನಿಲ್ಲಿಸು;
        }
    } ಆಗ((prev=length)>0);
    ಹಿಂತಿರುಗಿ prev;
}

U_NAMESPACE_ಕೊನೆ
