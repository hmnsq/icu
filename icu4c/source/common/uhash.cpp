// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
******************************************************************************
*   Copyright (C) 1997-2016, International Business Machines
*   Corporation and others.  All Rights Reserved.
******************************************************************************
*   Date        Name        Description
*   03/22/00    aliu        Adapted from original C++ ICU Hashtable.
*   07/06/01    aliu        Modified to support int32_t keys on
*                           platforms with sizeof(void*) < 32.
******************************************************************************
*/

#ਅਨ੍ਤਰ੍ਭਾਵ <string_view>

#ਅਨ੍ਤਰ੍ਭਾਵ "uhash.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/ustring.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "cstring.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "cmemory.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "uassert.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "ustr_imp.h"

/* This hashtable is implemented as a double hash.  All elements are
 * stored in a single array with no secondary storage for collision
 * resolution (no linked list, etc.).  When there is a hash collision
 * (when two unequal keys have the same hashcode) we resolve this by
 * using a secondary hash.  The secondary hash is an increment
 * computed as a hash function (a different one) of the primary
 * hashcode.  This increment is added to the initial hash value to
 * obtain further slots assigned to the same hash code.  For this to
 * work, the length of the array and the increment must be relatively
 * prime.  The easiest way to achieve this is to have the length of
 * the array be prime, and the increment be any value from
 * 1..length-1.
 *
 * Hashcodes are 32-bit integers.  We make sure all hashcodes are
 * non-negative by masking off the top bit.  This has two effects: (1)
 * modulo arithmetic is simplified.  If we allowed negative hashcodes,
 * then when we computed hashcode % length, we could get a negative
 * result, which we would then have to adjust back into range.  It's
 * simpler to just make hashcodes non-negative. (2) It makes it easy
 * to check for empty vs. occupied slots in the table.  We just mark
 * empty or deleted slots with a negative hashcode.
 *
 * The central function is _uhash_find().  This function looks for a
 * slot matching the given key and hashcode.  If one is found, it
 * returns a pointer to that slot.  If the table is full, and no match
 * is found, it returns nullptr -- in theory.  This would make the code
 * more complicated, since all callers of _uhash_find() would then
 * have to check for a nullptr result.  To keep this from happening, we
 * don't allow the table to fill.  When there is only one
 * empty/deleted slot left, uhash_put() will refuse to increase the
 * count, and fail.  This simplifies the code.  In practice, one will
 * seldom encounter this using default UHashtables.  However, if a
 * hashtable is set to a U_FIXED resize policy, or if memory is
 * exhausted, then the table may fill.
 *
 * High and low water ratios control rehashing.  They establish levels
 * of fullness (from 0 to 1) outside of which the data array is
 * reallocated and repopulated.  Setting the low water ratio to zero
 * means the table will never shrink.  Setting the high water ratio to
 * one means the table will never grow.  The ratios should be
 * coordinated with the ratio between successive elements of the
 * PRIMES table, so that when the primeIndex is incremented or
 * decremented during rehashing, it brings the ratio of count / length
 * back into the desired range (between low and high water ratios).
 */

/********************************************************************
 * PRIVATE Constants, Macros
 ********************************************************************/

/* This is a list of non-consecutive primes chosen such that
 * PRIMES[i+1] ~ 2*PRIMES[i].  (Currently, the ratio ranges from 1.81
 * to 2.18; the inverse ratio ranges from 0.459 to 0.552.)  If this
 * ratio is changed, the low and high water ratios should also be
 * adjusted to suit.
 *
 * These prime numbers were also chosen so that they are the largest
 * prime number while being less than a power of two.
 */
ਸਥਿਰ ਅਚਲ int32_t ਅਭਾਜ_ਸੰਖਿਆਵਾਂ[] = {
    7, 13, 31, 61, 127, 251, 509, 1021, 2039, 4093, 8191, 16381, 32749,
    65521, 131071, 262139, 524287, 1048573, 2097143, 4194301, 8388593,
    16777213, 33554393, 67108859, 134217689, 268435399, 536870909,
    1073741789, 2147483647 /*, 4294967291 */
};

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਅਭਾਜ_ਸੰਖਿਆਵਾਂ_ਲੰਮਾਈ UPRV_LENGTHOF(ਅਭਾਜ_ਸੰਖਿਆਵਾਂ)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ DEFAULT_ਅਭਾਜ_ਸੂਚਕਾਂਕ 4

/* These ratios are tuned to the PRIMES array such that a resize
 * places the table back into the zone of non-resizing.  That is,
 * after a call to _uhash_rehash(), a subsequent call to
 * _uhash_rehash() should do nothing (should not churn).  This is only
 * a potential problem with U_GROW_AND_SHRINK.
 */
ਸਥਿਰ ਅਚਲ ਦਸ਼ਮਲਵ RESIZE_POLICY_ਅਨੁਪਾਤ_ਸਾਰਣੀ[6] = {
    /* low, high water ratio */
    0.0F, 0.5F, /* U_GROW: Grow on demand, do not shrink */
    0.1F, 0.5F, /* U_GROW_AND_SHRINK: Grow and shrink on demand */
    0.0F, 1.0F  /* U_FIXED: Never change size */
};

/*
  Invariants for hashcode values:

  * DELETED < 0
  * EMPTY < 0
  * Real hashes >= 0

  Hashcodes may not start out this way, but internally they are
  adjusted so that they are always positive.  We assume 32-bit
  hashcodes; adjust these constants for other hashcode sizes.
*/
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ HASH_DELETED    ((int32_t) 0x80000000)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ HASH_ਖਾਲੀ_ਥਾਵਾਂ      ((int32_t) HASH_DELETED + 1)

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ IS_ਖਾਲੀ_ਥਾਵਾਂ_OR_DELETED(x) ((x) < 0)

/* This macro expects a UHashTok.pointer as its keypointer and
   valuepointer parameters */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ HASH_DELETE_ਕੁੰਜੀ_ਮੁੱਲ(hash, keypointer, valuepointer) UPRV_ਖੰਡ_MACRO_BEGIN { \
    ਜੇ (hash->ਕੁੰਜੀ_Deleter != ਖਾਲੀ_ਸੂਚਕ && keypointer != ਖਾਲੀ_ਸੂਚਕ) { \
        (*hash->ਕੁੰਜੀ_Deleter)(keypointer); \
    } \
    ਜੇ (hash->ਮੁੱਲ_Deleter != ਖਾਲੀ_ਸੂਚਕ && valuepointer != ਖਾਲੀ_ਸੂਚਕ) { \
        (*hash->ਮੁੱਲ_Deleter)(valuepointer); \
    } \
} UPRV_ਖੰਡ_MACRO_ਸਿਰਾ

/*
 * Constants for hinting whether a key or value is an integer
 * or a pointer.  If a hint bit is zero, then the associated
 * token is assumed to be an integer.
 */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ HINT_BOTH_INTEGERS (0)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ HINT_ਕੁੰਜੀ_ਸੂਚਕ   (1)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ HINT_ਮੁੱਲ_ਸੂਚਕ (2)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ HINT_ALLOW_ZERO    (4)

/********************************************************************
 * PRIVATE Implementation
 ********************************************************************/

ਸਥਿਰ UHashTok
uhash_ਸੈੱਟ_Element(UHashtable *hash, UHashElement* e,
                  int32_t hashcode,
                  UHashTok ਕੁੰਜੀ, UHashTok value, int8_t hint) {

    UHashTok old_ਮੁੱਲ = e->value;
    ਜੇ (hash->ਕੁੰਜੀ_Deleter != ਖਾਲੀ_ਸੂਚਕ && e->ਕੁੰਜੀ.ਸੂਚਕ != ਖਾਲੀ_ਸੂਚਕ &&
        e->ਕੁੰਜੀ.ਸੂਚਕ != ਕੁੰਜੀ.ਸੂਚਕ) { /* Avoid double deletion */
        (*hash->ਕੁੰਜੀ_Deleter)(e->ਕੁੰਜੀ.ਸੂਚਕ);
    }
    ਜੇ (hash->ਮੁੱਲ_Deleter != ਖਾਲੀ_ਸੂਚਕ) {
        ਜੇ (old_ਮੁੱਲ.ਸੂਚਕ != ਖਾਲੀ_ਸੂਚਕ &&
            old_ਮੁੱਲ.ਸੂਚਕ != value.ਸੂਚਕ) { /* Avoid double deletion */
            (*hash->ਮੁੱਲ_Deleter)(old_ਮੁੱਲ.ਸੂਚਕ);
        }
        old_ਮੁੱਲ.ਸੂਚਕ = ਖਾਲੀ_ਸੂਚਕ;
    }
    /* Compilers should copy the UHashTok union correctly, but even if
     * they do, memory heap tools (e.g. BoundsChecker) can get
     * confused when a pointer is cloaked in a union and then copied.
     * TO ALLEVIATE THIS, we use hints (based on what API the user is
     * calling) to copy pointers when we know the user thinks
     * something is a pointer. */
    ਜੇ (hint & HINT_ਕੁੰਜੀ_ਸੂਚਕ) {
        e->ਕੁੰਜੀ.ਸੂਚਕ = ਕੁੰਜੀ.ਸੂਚਕ;
    } ਨਹੀਂਤਾਂ {
        e->ਕੁੰਜੀ = ਕੁੰਜੀ;
    }
    ਜੇ (hint & HINT_ਮੁੱਲ_ਸੂਚਕ) {
        e->value.ਸੂਚਕ = value.ਸੂਚਕ;
    } ਨਹੀਂਤਾਂ {
        e->value = value;
    }
    e->hashcode = hashcode;
    ਵਾਪਸ old_ਮੁੱਲ;
}

/**
 * Assumes that the given element is not empty or deleted.
 */
ਸਥਿਰ UHashTok
uhash_ਅੰਦਰੂਨੀ_Remove_Element(UHashtable *hash, UHashElement* e) {
    UHashTok empty;
    U_ASSERT(!IS_ਖਾਲੀ_ਥਾਵਾਂ_OR_DELETED(e->hashcode));
    --hash->count;
    empty.ਸੂਚਕ = ਖਾਲੀ_ਸੂਚਕ; empty.integer = 0;
    ਵਾਪਸ uhash_ਸੈੱਟ_Element(hash, e, HASH_DELETED, empty, empty, 0);
}

ਸਥਿਰ ਖਾਲੀ
uhash_ਅੰਦਰੂਨੀ_ਸੈੱਟ_Resize_Policy(UHashtable *hash, ਗਿਣਤੀ UHashResizePolicy policy) {
    U_ASSERT(hash != ਖਾਲੀ_ਸੂਚਕ);
    U_ASSERT(((int32_t)policy) >= 0);
    U_ASSERT(((int32_t)policy) < 3);
    hash->ਨੀਵਾਂ_Water_ਅਨੁਪਾਤ  = RESIZE_POLICY_ਅਨੁਪਾਤ_ਸਾਰਣੀ[policy * 2];
    hash->ਉੱਚਾ_Water_ਅਨੁਪਾਤ = RESIZE_POLICY_ਅਨੁਪਾਤ_ਸਾਰਣੀ[policy * 2 + 1];
}

/**
 * Allocate internal data array of a size determined by the given
 * prime index.  If the index is out of range it is pinned into range.
 * If the allocation fails the status is set to
 * U_MEMORY_ALLOCATION_ERROR and all array storage is freed.  In
 * either case the previous array pointer is overwritten.
 *
 * Caller must ensure primeIndex is in range 0..PRIME_LENGTH-1.
 */
ਸਥਿਰ ਖਾਲੀ
_uhash_allocate(UHashtable *hash,
                int32_t ਅਭਾਜ_ਸੂਚਕਾਂਕ,
                UError_ਕੋਡ *ਸਥਿਤੀ) {

    UHashElement *p, *ਹੱਦਬੰਦੀ;
    UHashTok emptytok;

    ਜੇ (U_FAILURE(*ਸਥਿਤੀ)) ਵਾਪਸ;

    U_ASSERT(ਅਭਾਜ_ਸੂਚਕਾਂਕ >= 0 && ਅਭਾਜ_ਸੂਚਕਾਂਕ < ਅਭਾਜ_ਸੰਖਿਆਵਾਂ_ਲੰਮਾਈ);

    hash->ਅਭਾਜ_ਸੂਚਕਾਂਕ = static_cast<int8_t>(ਅਭਾਜ_ਸੂਚਕਾਂਕ);
    hash->length = ਅਭਾਜ_ਸੰਖਿਆਵਾਂ[ਅਭਾਜ_ਸੂਚਕਾਂਕ];

    p = hash->elements = static_cast<UHashElement*>(
        uprv_malloc(ਆਕਾਰ(UHashElement) * hash->length));

    ਜੇ (hash->elements == ਖਾਲੀ_ਸੂਚਕ) {
        *ਸਥਿਤੀ = U_ਸਮ੍ਰਿਤੀ_ਵੰਡ_ERROR;
        ਵਾਪਸ;
    }

    emptytok.ਸੂਚਕ = ਖਾਲੀ_ਸੂਚਕ; /* Only one of these two is needed */
    emptytok.integer = 0;    /* but we don't know which one. */

    ਹੱਦਬੰਦੀ = p + hash->length;
    ਜਦੋਂ (p < ਹੱਦਬੰਦੀ) {
        p->ਕੁੰਜੀ = emptytok;
        p->value = emptytok;
        p->hashcode = HASH_ਖਾਲੀ_ਥਾਵਾਂ;
        ++p;
    }

    hash->count = 0;
    hash->ਨੀਵਾਂ_Water_Mark = static_cast<int32_t>(hash->length * hash->ਨੀਵਾਂ_Water_ਅਨੁਪਾਤ);
    hash->ਉੱਚਾ_Water_Mark = static_cast<int32_t>(hash->length * hash->ਉੱਚਾ_Water_ਅਨੁਪਾਤ);
}

ਸਥਿਰ UHashtable*
_uhash_init(UHashtable *ਨਤੀਜਾ,
              UHashFunction *ਕੁੰਜੀ_Hash,
              UKeyComparator *ਕੁੰਜੀ_Comp,
              UValueComparator *ਮੁੱਲ_Comp,
              int32_t ਅਭਾਜ_ਸੂਚਕਾਂਕ,
              UError_ਕੋਡ *ਸਥਿਤੀ)
{
    ਜੇ (U_FAILURE(*ਸਥਿਤੀ)) ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
    U_ASSERT(ਕੁੰਜੀ_Hash != ਖਾਲੀ_ਸੂਚਕ);
    U_ASSERT(ਕੁੰਜੀ_Comp != ਖਾਲੀ_ਸੂਚਕ);

    ਨਤੀਜਾ->ਕੁੰਜੀ_Hasher       = ਕੁੰਜੀ_Hash;
    ਨਤੀਜਾ->ਕੁੰਜੀ_Comparator   = ਕੁੰਜੀ_Comp;
    ਨਤੀਜਾ->ਮੁੱਲ_Comparator = ਮੁੱਲ_Comp;
    ਨਤੀਜਾ->ਕੁੰਜੀ_Deleter      = ਖਾਲੀ_ਸੂਚਕ;
    ਨਤੀਜਾ->ਮੁੱਲ_Deleter    = ਖਾਲੀ_ਸੂਚਕ;
    ਨਤੀਜਾ->allocated       = ਝੂਠ;
    uhash_ਅੰਦਰੂਨੀ_ਸੈੱਟ_Resize_Policy(ਨਤੀਜਾ, U_GROW);

    _uhash_allocate(ਨਤੀਜਾ, ਅਭਾਜ_ਸੂਚਕਾਂਕ, ਸਥਿਤੀ);

    ਜੇ (U_FAILURE(*ਸਥਿਤੀ)) {
        ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
    }

    ਵਾਪਸ ਨਤੀਜਾ;
}

ਸਥਿਰ UHashtable*
_uhash_create(UHashFunction *ਕੁੰਜੀ_Hash,
              UKeyComparator *ਕੁੰਜੀ_Comp,
              UValueComparator *ਮੁੱਲ_Comp,
              int32_t ਅਭਾਜ_ਸੂਚਕਾਂਕ,
              UError_ਕੋਡ *ਸਥਿਤੀ) {
    UHashtable *ਨਤੀਜਾ;

    ਜੇ (U_FAILURE(*ਸਥਿਤੀ)) ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;

    ਨਤੀਜਾ = static_cast<UHashtable*>(uprv_malloc(ਆਕਾਰ(UHashtable)));
    ਜੇ (ਨਤੀਜਾ == ਖਾਲੀ_ਸੂਚਕ) {
        *ਸਥਿਤੀ = U_ਸਮ੍ਰਿਤੀ_ਵੰਡ_ERROR;
        ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
    }

    _uhash_init(ਨਤੀਜਾ, ਕੁੰਜੀ_Hash, ਕੁੰਜੀ_Comp, ਮੁੱਲ_Comp, ਅਭਾਜ_ਸੂਚਕਾਂਕ, ਸਥਿਤੀ);
    ਨਤੀਜਾ->allocated       = ਸੱਚ;

    ਜੇ (U_FAILURE(*ਸਥਿਤੀ)) {
        uprv_free(ਨਤੀਜਾ);
        ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
    }

    ਵਾਪਸ ਨਤੀਜਾ;
}

/**
 * Look for a key in the table, or if no such key exists, the first
 * empty slot matching the given hashcode.  Keys are compared using
 * the keyComparator function.
 *
 * First find the start position, which is the hashcode modulo
 * the length.  Test it to see if it is:
 *
 * a. identical:  First check the hash values for a quick check,
 *    then compare keys for equality using keyComparator.
 * b. deleted
 * c. empty
 *
 * Stop if it is identical or empty, otherwise continue by adding a
 * "jump" value (moduloing by the length again to keep it within
 * range) and retesting.  For efficiency, there need enough empty
 * values so that the searches stop within a reasonable amount of time.
 * This can be changed by changing the high/low water marks.
 *
 * In theory, this function can return nullptr, if it is full (no empty
 * or deleted slots) and if no matching key is found.  In practice, we
 * prevent this elsewhere (in uhash_put) by making sure the last slot
 * in the table is never filled.
 *
 * The size of the table should be prime for this algorithm to work;
 * otherwise we are not guaranteed that the jump value (the secondary
 * hash) is relatively prime to the table length.
 */
ਸਥਿਰ UHashElement*
uhash_ਲੱਭੋ_2(ਅਚਲ UHashtable *hash, UHashTok ਕੁੰਜੀ,
            int32_t hashcode) {

    int32_t ਪਹਿਲਾ_Deleted = -1;  /* assume invalid index */
    int32_t the_ਸੂਚਕਾਂਕ, ਸ਼ੁਰੂ_ਸੂਚਕਾਂਕ;
    int32_t jump = 0; /* lazy evaluate */
    int32_t ਸਾਰਣੀ_Hash;
    UHashElement *elements = hash->elements;

    hashcode &= 0x7FFFFFFF; /* must be positive */
    ਸ਼ੁਰੂ_ਸੂਚਕਾਂਕ = the_ਸੂਚਕਾਂਕ = (hashcode ^ 0x4000000) % hash->length;

    ਕਰੋ {
        ਸਾਰਣੀ_Hash = elements[the_ਸੂਚਕਾਂਕ].hashcode;
        ਜੇ (ਸਾਰਣੀ_Hash == hashcode) {          /* quick check */
            ਜੇ ((*hash->ਕੁੰਜੀ_Comparator)(ਕੁੰਜੀ, elements[the_ਸੂਚਕਾਂਕ].ਕੁੰਜੀ)) {
                ਵਾਪਸ &(elements[the_ਸੂਚਕਾਂਕ]);
            }
        } ਨਹੀਂਤਾਂ ਜੇ (!IS_ਖਾਲੀ_ਥਾਵਾਂ_OR_DELETED(ਸਾਰਣੀ_Hash)) {
            /* We have hit a slot which contains a key-value pair,
             * but for which the hash code does not match.  Keep
             * looking.
             */
        } ਨਹੀਂਤਾਂ ਜੇ (ਸਾਰਣੀ_Hash == HASH_ਖਾਲੀ_ਥਾਵਾਂ) { /* empty, end o' the line */
            ਰੋਕੋ;
        } ਨਹੀਂਤਾਂ ਜੇ (ਪਹਿਲਾ_Deleted < 0) { /* remember first deleted */
            ਪਹਿਲਾ_Deleted = the_ਸੂਚਕਾਂਕ;
        }
        ਜੇ (jump == 0) { /* lazy compute jump */
            /* The jump value must be relatively prime to the table
             * length.  As long as the length is prime, then any value
             * 1..length-1 will be relatively prime to it.
             */
            jump = (hashcode % (hash->length - 1)) + 1;
        }
        the_ਸੂਚਕਾਂਕ = (the_ਸੂਚਕਾਂਕ + jump) % hash->length;
    } ਜਦੋਂ (the_ਸੂਚਕਾਂਕ != ਸ਼ੁਰੂ_ਸੂਚਕਾਂਕ);

    ਜੇ (ਪਹਿਲਾ_Deleted >= 0) {
        the_ਸੂਚਕਾਂਕ = ਪਹਿਲਾ_Deleted; /* reset if had deleted slot */
    } ਨਹੀਂਤਾਂ ਜੇ (ਸਾਰਣੀ_Hash != HASH_ਖਾਲੀ_ਥਾਵਾਂ) {
        /* We get to this point if the hashtable is full (no empty or
         * deleted slots), and we've failed to find a match.  THIS
         * WILL NEVER HAPPEN as long as uhash_put() makes sure that
         * count is always < length.
         */
        UPRV_UNREACHABLE_EXIT;
    }
    ਵਾਪਸ &(elements[the_ਸੂਚਕਾਂਕ]);
}

/**
 * Attempt to grow or shrink the data arrays in order to make the
 * count fit between the high and low water marks.  hash_put() and
 * hash_remove() call this method when the count exceeds the high or
 * low water marks.  This method may do nothing, if memory allocation
 * fails, or if the count is already in range, or if the length is
 * already at the low or high limit.  In any case, upon return the
 * arrays will be valid.
 */
ਸਥਿਰ ਖਾਲੀ
_uhash_rehash(UHashtable *hash, UError_ਕੋਡ *ਸਥਿਤੀ) {

    UHashElement *old = hash->elements;
    int32_t old_ਲੰਮਾਈ = hash->length;
    int32_t new_ਅਭਾਜ_ਸੂਚਕਾਂਕ = hash->ਅਭਾਜ_ਸੂਚਕਾਂਕ;
    int32_t i;

    ਜੇ (hash->count > hash->ਉੱਚਾ_Water_Mark) {
        ਜੇ (++new_ਅਭਾਜ_ਸੂਚਕਾਂਕ >= ਅਭਾਜ_ਸੰਖਿਆਵਾਂ_ਲੰਮਾਈ) {
            ਵਾਪਸ;
        }
    } ਨਹੀਂਤਾਂ ਜੇ (hash->count < hash->ਨੀਵਾਂ_Water_Mark) {
        ਜੇ (--new_ਅਭਾਜ_ਸੂਚਕਾਂਕ < 0) {
            ਵਾਪਸ;
        }
    } ਨਹੀਂਤਾਂ {
        ਵਾਪਸ;
    }

    _uhash_allocate(hash, new_ਅਭਾਜ_ਸੂਚਕਾਂਕ, ਸਥਿਤੀ);

    ਜੇ (U_FAILURE(*ਸਥਿਤੀ)) {
        hash->elements = old;
        hash->length = old_ਲੰਮਾਈ;
        ਵਾਪਸ;
    }

    ਲਈ (i = old_ਲੰਮਾਈ - 1; i >= 0; --i) {
        ਜੇ (!IS_ਖਾਲੀ_ਥਾਵਾਂ_OR_DELETED(old[i].hashcode)) {
            UHashElement *e = uhash_ਲੱਭੋ_2(hash, old[i].ਕੁੰਜੀ, old[i].hashcode);
            U_ASSERT(e != ਖਾਲੀ_ਸੂਚਕ);
            U_ASSERT(e->hashcode == HASH_ਖਾਲੀ_ਥਾਵਾਂ);
            e->ਕੁੰਜੀ = old[i].ਕੁੰਜੀ;
            e->value = old[i].value;
            e->hashcode = old[i].hashcode;
            ++hash->count;
        }
    }

    uprv_free(old);
}

ਸਥਿਰ UHashTok
_uhash_remove(UHashtable *hash,
              UHashTok ਕੁੰਜੀ) {
    /* First find the position of the key in the table.  If the object
     * has not been removed already, remove it.  If the user wanted
     * keys deleted, then delete it also.  We have to put a special
     * hashcode in that position that means that something has been
     * deleted, since when we do a find, we have to continue PAST any
     * deleted values.
     */
    UHashTok ਨਤੀਜਾ;
    UHashElement* e = uhash_ਲੱਭੋ_2(hash, ਕੁੰਜੀ, hash->ਕੁੰਜੀ_Hasher(ਕੁੰਜੀ));
    U_ASSERT(e != ਖਾਲੀ_ਸੂਚਕ);
    ਨਤੀਜਾ.ਸੂਚਕ = ਖਾਲੀ_ਸੂਚਕ;
    ਨਤੀਜਾ.integer = 0;
    ਜੇ (!IS_ਖਾਲੀ_ਥਾਵਾਂ_OR_DELETED(e->hashcode)) {
        ਨਤੀਜਾ = uhash_ਅੰਦਰੂਨੀ_Remove_Element(hash, e);
        ਜੇ (hash->count < hash->ਨੀਵਾਂ_Water_Mark) {
            UError_ਕੋਡ ਸਥਿਤੀ = U_ZERO_ERROR;
            _uhash_rehash(hash, &ਸਥਿਤੀ);
        }
    }
    ਵਾਪਸ ਨਤੀਜਾ;
}

ਸਥਿਰ UHashTok
_uhash_put(UHashtable *hash,
           UHashTok ਕੁੰਜੀ,
           UHashTok value,
           int8_t hint,
           UError_ਕੋਡ *ਸਥਿਤੀ) {

    /* Put finds the position in the table for the new value.  If the
     * key is already in the table, it is deleted, if there is a
     * non-nullptr keyDeleter.  Then the key, the hash and the value are
     * all put at the position in their respective arrays.
     */
    int32_t hashcode;
    UHashElement* e;
    UHashTok emptytok;

    ਜੇ (U_FAILURE(*ਸਥਿਤੀ)) {
        ਜਾਓ err;
    }
    U_ASSERT(hash != ਖਾਲੀ_ਸੂਚਕ);
    ਜੇ ((hint & HINT_ਮੁੱਲ_ਸੂਚਕ) ?
            value.ਸੂਚਕ == ਖਾਲੀ_ਸੂਚਕ :
            value.integer == 0 && (hint & HINT_ALLOW_ZERO) == 0) {
        /* Disallow storage of nullptr values, since nullptr is returned by
         * get() to indicate an absent key.  Storing nullptr == removing.
         */
        ਵਾਪਸ _uhash_remove(hash, ਕੁੰਜੀ);
    }
    ਜੇ (hash->count > hash->ਉੱਚਾ_Water_Mark) {
        _uhash_rehash(hash, ਸਥਿਤੀ);
        ਜੇ (U_FAILURE(*ਸਥਿਤੀ)) {
            ਜਾਓ err;
        }
    }

    hashcode = (*hash->ਕੁੰਜੀ_Hasher)(ਕੁੰਜੀ);
    e = uhash_ਲੱਭੋ_2(hash, ਕੁੰਜੀ, hashcode);
    U_ASSERT(e != ਖਾਲੀ_ਸੂਚਕ);

    ਜੇ (IS_ਖਾਲੀ_ਥਾਵਾਂ_OR_DELETED(e->hashcode)) {
        /* Important: We must never actually fill the table up.  If we
         * do so, then _uhash_find() will return nullptr, and we'll have
         * to check for nullptr after every call to _uhash_find().  To
         * avoid this we make sure there is always at least one empty
         * or deleted slot in the table.  This only is a problem if we
         * are out of memory and rehash isn't working.
         */
        ++hash->count;
        ਜੇ (hash->count == hash->length) {
            /* Don't allow count to reach length */
            --hash->count;
            *ਸਥਿਤੀ = U_ਸਮ੍ਰਿਤੀ_ਵੰਡ_ERROR;
            ਜਾਓ err;
        }
    }

    /* We must in all cases handle storage properly.  If there was an
     * old key, then it must be deleted (if the deleter != nullptr).
     * Make hashcodes stored in table positive.
     */
    ਵਾਪਸ uhash_ਸੈੱਟ_Element(hash, e, hashcode & 0x7FFFFFFF, ਕੁੰਜੀ, value, hint);

 err:
    /* If the deleters are non-nullptr, this method adopts its key and/or
     * value arguments, and we must be sure to delete the key and/or
     * value in all cases, even upon failure.
     */
    HASH_DELETE_ਕੁੰਜੀ_ਮੁੱਲ(hash, ਕੁੰਜੀ.ਸੂਚਕ, value.ਸੂਚਕ);
    emptytok.ਸੂਚਕ = ਖਾਲੀ_ਸੂਚਕ; emptytok.integer = 0;
    ਵਾਪਸ emptytok;
}


/********************************************************************
 * PUBLIC API
 ********************************************************************/

U_CAPI UHashtable* U_EXPORT2
uhash_open(UHashFunction *ਕੁੰਜੀ_Hash,
           UKeyComparator *ਕੁੰਜੀ_Comp,
           UValueComparator *ਮੁੱਲ_Comp,
           UError_ਕੋਡ *ਸਥਿਤੀ) {

    ਵਾਪਸ _uhash_create(ਕੁੰਜੀ_Hash, ਕੁੰਜੀ_Comp, ਮੁੱਲ_Comp, DEFAULT_ਅਭਾਜ_ਸੂਚਕਾਂਕ, ਸਥਿਤੀ);
}

U_CAPI UHashtable* U_EXPORT2
uhash_open_ਮਾਪ(UHashFunction *ਕੁੰਜੀ_Hash,
               UKeyComparator *ਕੁੰਜੀ_Comp,
               UValueComparator *ਮੁੱਲ_Comp,
               int32_t size,
               UError_ਕੋਡ *ਸਥਿਤੀ) {

    /* Find the smallest index i for which PRIMES[i] >= size. */
    int32_t i = 0;
    ਜਦੋਂ (i<(ਅਭਾਜ_ਸੰਖਿਆਵਾਂ_ਲੰਮਾਈ-1) && ਅਭਾਜ_ਸੰਖਿਆਵਾਂ[i]<size) {
        ++i;
    }

    ਵਾਪਸ _uhash_create(ਕੁੰਜੀ_Hash, ਕੁੰਜੀ_Comp, ਮੁੱਲ_Comp, i, ਸਥਿਤੀ);
}

U_CAPI UHashtable* U_EXPORT2
uhash_init(UHashtable *fillin_ਨਤੀਜਾ,
           UHashFunction *ਕੁੰਜੀ_Hash,
           UKeyComparator *ਕੁੰਜੀ_Comp,
           UValueComparator *ਮੁੱਲ_Comp,
           UError_ਕੋਡ *ਸਥਿਤੀ) {

    ਵਾਪਸ _uhash_init(fillin_ਨਤੀਜਾ, ਕੁੰਜੀ_Hash, ਕੁੰਜੀ_Comp, ਮੁੱਲ_Comp, DEFAULT_ਅਭਾਜ_ਸੂਚਕਾਂਕ, ਸਥਿਤੀ);
}

U_CAPI UHashtable* U_EXPORT2
uhash_init_ਮਾਪ(UHashtable *fillin_ਨਤੀਜਾ,
               UHashFunction *ਕੁੰਜੀ_Hash,
               UKeyComparator *ਕੁੰਜੀ_Comp,
               UValueComparator *ਮੁੱਲ_Comp,
               int32_t size,
               UError_ਕੋਡ *ਸਥਿਤੀ) {

    // Find the smallest index i for which PRIMES[i] >= size.
    int32_t i = 0;
    ਜਦੋਂ (i<(ਅਭਾਜ_ਸੰਖਿਆਵਾਂ_ਲੰਮਾਈ-1) && ਅਭਾਜ_ਸੰਖਿਆਵਾਂ[i]<size) {
        ++i;
    }
    ਵਾਪਸ _uhash_init(fillin_ਨਤੀਜਾ, ਕੁੰਜੀ_Hash, ਕੁੰਜੀ_Comp, ਮੁੱਲ_Comp, i, ਸਥਿਤੀ);
}

U_CAPI ਖਾਲੀ U_EXPORT2
uhash_close(UHashtable *hash) {
    ਜੇ (hash == ਖਾਲੀ_ਸੂਚਕ) {
        ਵਾਪਸ;
    }
    ਜੇ (hash->elements != ਖਾਲੀ_ਸੂਚਕ) {
        ਜੇ (hash->ਕੁੰਜੀ_Deleter != ਖਾਲੀ_ਸੂਚਕ || hash->ਮੁੱਲ_Deleter != ਖਾਲੀ_ਸੂਚਕ) {
            int32_t pos=UHASH_ਪਹਿਲਾ;
            UHashElement *e;
            ਜਦੋਂ ((e = (UHashElement*) uhash_ਅਗਲਾ_Element(hash, &pos)) != ਖਾਲੀ_ਸੂਚਕ) {
                HASH_DELETE_ਕੁੰਜੀ_ਮੁੱਲ(hash, e->ਕੁੰਜੀ.ਸੂਚਕ, e->value.ਸੂਚਕ);
            }
        }
        uprv_free(hash->elements);
        hash->elements = ਖਾਲੀ_ਸੂਚਕ;
    }
    ਜੇ (hash->allocated) {
        uprv_free(hash);
    }
}

U_CAPI UHashFunction *U_EXPORT2
uhash_ਸੈੱਟ_ਕੁੰਜੀ_Hasher(UHashtable *hash, UHashFunction *fn) {
    UHashFunction *ਨਤੀਜਾ = hash->ਕੁੰਜੀ_Hasher;
    hash->ਕੁੰਜੀ_Hasher = fn;
    ਵਾਪਸ ਨਤੀਜਾ;
}

U_CAPI UKeyComparator *U_EXPORT2
uhash_ਸੈੱਟ_ਕੁੰਜੀ_Comparator(UHashtable *hash, UKeyComparator *fn) {
    UKeyComparator *ਨਤੀਜਾ = hash->ਕੁੰਜੀ_Comparator;
    hash->ਕੁੰਜੀ_Comparator = fn;
    ਵਾਪਸ ਨਤੀਜਾ;
}
U_CAPI UValueComparator *U_EXPORT2
uhash_ਸੈੱਟ_ਮੁੱਲ_Comparator(UHashtable *hash, UValueComparator *fn){
    UValueComparator *ਨਤੀਜਾ = hash->ਮੁੱਲ_Comparator;
    hash->ਮੁੱਲ_Comparator = fn;
    ਵਾਪਸ ਨਤੀਜਾ;
}

U_CAPI UObjectDeleter *U_EXPORT2
uhash_ਸੈੱਟ_ਕੁੰਜੀ_Deleter(UHashtable *hash, UObjectDeleter *fn) {
    UObjectDeleter *ਨਤੀਜਾ = hash->ਕੁੰਜੀ_Deleter;
    hash->ਕੁੰਜੀ_Deleter = fn;
    ਵਾਪਸ ਨਤੀਜਾ;
}

U_CAPI UObjectDeleter *U_EXPORT2
uhash_ਸੈੱਟ_ਮੁੱਲ_Deleter(UHashtable *hash, UObjectDeleter *fn) {
    UObjectDeleter *ਨਤੀਜਾ = hash->ਮੁੱਲ_Deleter;
    hash->ਮੁੱਲ_Deleter = fn;
    ਵਾਪਸ ਨਤੀਜਾ;
}

U_CAPI ਖਾਲੀ U_EXPORT2
uhash_ਸੈੱਟ_Resize_Policy(UHashtable *hash, ਗਿਣਤੀ UHashResizePolicy policy) {
    UError_ਕੋਡ ਸਥਿਤੀ = U_ZERO_ERROR;
    uhash_ਅੰਦਰੂਨੀ_ਸੈੱਟ_Resize_Policy(hash, policy);
    hash->ਨੀਵਾਂ_Water_Mark  = (int32_t)(hash->length * hash->ਨੀਵਾਂ_Water_ਅਨੁਪਾਤ);
    hash->ਉੱਚਾ_Water_Mark = (int32_t)(hash->length * hash->ਉੱਚਾ_Water_ਅਨੁਪਾਤ);
    _uhash_rehash(hash, &ਸਥਿਤੀ);
}

U_CAPI int32_t U_EXPORT2
uhash_ਗਿਣਤੀ(ਅਚਲ UHashtable *hash) {
    ਵਾਪਸ hash->count;
}

U_CAPI ਖਾਲੀ* U_EXPORT2
uhash_get(ਅਚਲ UHashtable *hash,
          ਅਚਲ ਖਾਲੀ* ਕੁੰਜੀ) {
    UHashTok keyholder;
    keyholder.ਸੂਚਕ = (ਖਾਲੀ*) ਕੁੰਜੀ;
    ਵਾਪਸ uhash_ਲੱਭੋ_2(hash, keyholder, hash->ਕੁੰਜੀ_Hasher(keyholder))->value.ਸੂਚਕ;
}

U_CAPI ਖਾਲੀ* U_EXPORT2
uhash_iget(ਅਚਲ UHashtable *hash,
           int32_t ਕੁੰਜੀ) {
    UHashTok keyholder;
    keyholder.integer = ਕੁੰਜੀ;
    ਵਾਪਸ uhash_ਲੱਭੋ_2(hash, keyholder, hash->ਕੁੰਜੀ_Hasher(keyholder))->value.ਸੂਚਕ;
}

U_CAPI int32_t U_EXPORT2
uhash_geti(ਅਚਲ UHashtable *hash,
           ਅਚਲ ਖਾਲੀ* ਕੁੰਜੀ) {
    UHashTok keyholder;
    keyholder.ਸੂਚਕ = (ਖਾਲੀ*) ਕੁੰਜੀ;
    ਵਾਪਸ uhash_ਲੱਭੋ_2(hash, keyholder, hash->ਕੁੰਜੀ_Hasher(keyholder))->value.integer;
}

U_CAPI int32_t U_EXPORT2
uhash_igeti(ਅਚਲ UHashtable *hash,
           int32_t ਕੁੰਜੀ) {
    UHashTok keyholder;
    keyholder.integer = ਕੁੰਜੀ;
    ਵਾਪਸ uhash_ਲੱਭੋ_2(hash, keyholder, hash->ਕੁੰਜੀ_Hasher(keyholder))->value.integer;
}

U_CAPI int32_t U_EXPORT2
uhash_geti_And_ਮਿਲਿਆ(ਅਚਲ UHashtable *hash,
                   ਅਚਲ ਖਾਲੀ *ਕੁੰਜੀ,
                   UBool *ਮਿਲਿਆ) {
    UHashTok keyholder;
    keyholder.ਸੂਚਕ = (ਖਾਲੀ *)ਕੁੰਜੀ;
    ਅਚਲ UHashElement *e = uhash_ਲੱਭੋ_2(hash, keyholder, hash->ਕੁੰਜੀ_Hasher(keyholder));
    *ਮਿਲਿਆ = !IS_ਖਾਲੀ_ਥਾਵਾਂ_OR_DELETED(e->hashcode);
    ਵਾਪਸ e->value.integer;
}

U_CAPI int32_t U_EXPORT2
uhash_igeti_And_ਮਿਲਿਆ(ਅਚਲ UHashtable *hash,
                    int32_t ਕੁੰਜੀ,
                    UBool *ਮਿਲਿਆ) {
    UHashTok keyholder;
    keyholder.integer = ਕੁੰਜੀ;
    ਅਚਲ UHashElement *e = uhash_ਲੱਭੋ_2(hash, keyholder, hash->ਕੁੰਜੀ_Hasher(keyholder));
    *ਮਿਲਿਆ = !IS_ਖਾਲੀ_ਥਾਵਾਂ_OR_DELETED(e->hashcode);
    ਵਾਪਸ e->value.integer;
}

U_CAPI ਖਾਲੀ* U_EXPORT2
uhash_put(UHashtable *hash,
          ਖਾਲੀ* ਕੁੰਜੀ,
          ਖਾਲੀ* value,
          UError_ਕੋਡ *ਸਥਿਤੀ) {
    UHashTok keyholder, valueholder;
    keyholder.ਸੂਚਕ = ਕੁੰਜੀ;
    valueholder.ਸੂਚਕ = value;
    ਵਾਪਸ _uhash_put(hash, keyholder, valueholder,
                      HINT_ਕੁੰਜੀ_ਸੂਚਕ | HINT_ਮੁੱਲ_ਸੂਚਕ,
                      ਸਥਿਤੀ).ਸੂਚਕ;
}

U_CAPI ਖਾਲੀ* U_EXPORT2
uhash_iput(UHashtable *hash,
           int32_t ਕੁੰਜੀ,
           ਖਾਲੀ* value,
           UError_ਕੋਡ *ਸਥਿਤੀ) {
    UHashTok keyholder, valueholder;
    keyholder.integer = ਕੁੰਜੀ;
    valueholder.ਸੂਚਕ = value;
    ਵਾਪਸ _uhash_put(hash, keyholder, valueholder,
                      HINT_ਮੁੱਲ_ਸੂਚਕ,
                      ਸਥਿਤੀ).ਸੂਚਕ;
}

U_CAPI int32_t U_EXPORT2
uhash_puti(UHashtable *hash,
           ਖਾਲੀ* ਕੁੰਜੀ,
           int32_t value,
           UError_ਕੋਡ *ਸਥਿਤੀ) {
    UHashTok keyholder, valueholder;
    keyholder.ਸੂਚਕ = ਕੁੰਜੀ;
    valueholder.integer = value;
    ਵਾਪਸ _uhash_put(hash, keyholder, valueholder,
                      HINT_ਕੁੰਜੀ_ਸੂਚਕ,
                      ਸਥਿਤੀ).integer;
}


U_CAPI int32_t U_EXPORT2
uhash_iputi(UHashtable *hash,
           int32_t ਕੁੰਜੀ,
           int32_t value,
           UError_ਕੋਡ *ਸਥਿਤੀ) {
    UHashTok keyholder, valueholder;
    keyholder.integer = ਕੁੰਜੀ;
    valueholder.integer = value;
    ਵਾਪਸ _uhash_put(hash, keyholder, valueholder,
                      HINT_BOTH_INTEGERS,
                      ਸਥਿਤੀ).integer;
}

U_CAPI int32_t U_EXPORT2
uhash_putiAllowZero(UHashtable *hash,
                    ਖਾਲੀ *ਕੁੰਜੀ,
                    int32_t value,
                    UError_ਕੋਡ *ਸਥਿਤੀ) {
    UHashTok keyholder, valueholder;
    keyholder.ਸੂਚਕ = ਕੁੰਜੀ;
    valueholder.integer = value;
    ਵਾਪਸ _uhash_put(hash, keyholder, valueholder,
                      HINT_ਕੁੰਜੀ_ਸੂਚਕ | HINT_ALLOW_ZERO,
                      ਸਥਿਤੀ).integer;
}


U_CAPI int32_t U_EXPORT2
uhash_iputiAllowZero(UHashtable *hash,
                     int32_t ਕੁੰਜੀ,
                     int32_t value,
                     UError_ਕੋਡ *ਸਥਿਤੀ) {
    UHashTok keyholder, valueholder;
    keyholder.integer = ਕੁੰਜੀ;
    valueholder.integer = value;
    ਵਾਪਸ _uhash_put(hash, keyholder, valueholder,
                      HINT_BOTH_INTEGERS | HINT_ALLOW_ZERO,
                      ਸਥਿਤੀ).integer;
}

U_CAPI ਖਾਲੀ* U_EXPORT2
uhash_remove(UHashtable *hash,
             ਅਚਲ ਖਾਲੀ* ਕੁੰਜੀ) {
    UHashTok keyholder;
    keyholder.ਸੂਚਕ = (ਖਾਲੀ*) ਕੁੰਜੀ;
    ਵਾਪਸ _uhash_remove(hash, keyholder).ਸੂਚਕ;
}

U_CAPI ਖਾਲੀ* U_EXPORT2
uhash_iremove(UHashtable *hash,
              int32_t ਕੁੰਜੀ) {
    UHashTok keyholder;
    keyholder.integer = ਕੁੰਜੀ;
    ਵਾਪਸ _uhash_remove(hash, keyholder).ਸੂਚਕ;
}

U_CAPI int32_t U_EXPORT2
uhash_removei(UHashtable *hash,
              ਅਚਲ ਖਾਲੀ* ਕੁੰਜੀ) {
    UHashTok keyholder;
    keyholder.ਸੂਚਕ = (ਖਾਲੀ*) ਕੁੰਜੀ;
    ਵਾਪਸ _uhash_remove(hash, keyholder).integer;
}

U_CAPI int32_t U_EXPORT2
uhash_iremovei(UHashtable *hash,
               int32_t ਕੁੰਜੀ) {
    UHashTok keyholder;
    keyholder.integer = ਕੁੰਜੀ;
    ਵਾਪਸ _uhash_remove(hash, keyholder).integer;
}

U_CAPI ਖਾਲੀ U_EXPORT2
uhash_removeAll(UHashtable *hash) {
    int32_t pos = UHASH_ਪਹਿਲਾ;
    ਅਚਲ UHashElement *e;
    U_ASSERT(hash != ਖਾਲੀ_ਸੂਚਕ);
    ਜੇ (hash->count != 0) {
        ਜਦੋਂ ((e = uhash_ਅਗਲਾ_Element(hash, &pos)) != ਖਾਲੀ_ਸੂਚਕ) {
            uhash_removeElement(hash, e);
        }
    }
    U_ASSERT(hash->count == 0);
}

U_CAPI UBool U_EXPORT2
uhash_ਸ਼ਾਮਲ_ਹੈ_ਕੁੰਜੀ(ਅਚਲ UHashtable *hash, ਅਚਲ ਖਾਲੀ *ਕੁੰਜੀ) {
    UHashTok keyholder;
    keyholder.ਸੂਚਕ = (ਖਾਲੀ *)ਕੁੰਜੀ;
    ਅਚਲ UHashElement *e = uhash_ਲੱਭੋ_2(hash, keyholder, hash->ਕੁੰਜੀ_Hasher(keyholder));
    ਵਾਪਸ !IS_ਖਾਲੀ_ਥਾਵਾਂ_OR_DELETED(e->hashcode);
}

/**
 * Returns true if the UHashtable contains an item with this integer key.
 *
 * @param hash The target UHashtable.
 * @param key An integer key stored in a hashtable
 * @return true if the key is found.
 */
U_CAPI UBool U_EXPORT2
uhash_icontains_ਕੁੰਜੀ(ਅਚਲ UHashtable *hash, int32_t ਕੁੰਜੀ) {
    UHashTok keyholder;
    keyholder.integer = ਕੁੰਜੀ;
    ਅਚਲ UHashElement *e = uhash_ਲੱਭੋ_2(hash, keyholder, hash->ਕੁੰਜੀ_Hasher(keyholder));
    ਵਾਪਸ !IS_ਖਾਲੀ_ਥਾਵਾਂ_OR_DELETED(e->hashcode);
}

U_CAPI ਅਚਲ UHashElement* U_EXPORT2
uhash_ਲੱਭੋ(ਅਚਲ UHashtable *hash, ਅਚਲ ਖਾਲੀ* ਕੁੰਜੀ) {
    UHashTok keyholder;
    ਅਚਲ UHashElement *e;
    keyholder.ਸੂਚਕ = (ਖਾਲੀ*) ਕੁੰਜੀ;
    e = uhash_ਲੱਭੋ_2(hash, keyholder, hash->ਕੁੰਜੀ_Hasher(keyholder));
    ਵਾਪਸ IS_ਖਾਲੀ_ਥਾਵਾਂ_OR_DELETED(e->hashcode) ? ਖਾਲੀ_ਸੂਚਕ : e;
}

U_CAPI ਅਚਲ UHashElement* U_EXPORT2
uhash_ਅਗਲਾ_Element(ਅਚਲ UHashtable *hash, int32_t *pos) {
    /* Walk through the array until we find an element that is not
     * EMPTY and not DELETED.
     */
    int32_t i;
    U_ASSERT(hash != ਖਾਲੀ_ਸੂਚਕ);
    ਲਈ (i = *pos + 1; i < hash->length; ++i) {
        ਜੇ (!IS_ਖਾਲੀ_ਥਾਵਾਂ_OR_DELETED(hash->elements[i].hashcode)) {
            *pos = i;
            ਵਾਪਸ &(hash->elements[i]);
        }
    }

    /* No more elements */
    ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
}

U_CAPI ਖਾਲੀ* U_EXPORT2
uhash_removeElement(UHashtable *hash, ਅਚਲ UHashElement* e) {
    U_ASSERT(hash != ਖਾਲੀ_ਸੂਚਕ);
    U_ASSERT(e != ਖਾਲੀ_ਸੂਚਕ);
    ਜੇ (!IS_ਖਾਲੀ_ਥਾਵਾਂ_OR_DELETED(e->hashcode)) {
        UHashElement *nce = (UHashElement *)e;
        ਵਾਪਸ uhash_ਅੰਦਰੂਨੀ_Remove_Element(hash, nce).ਸੂਚਕ;
    }
    ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
}

/********************************************************************
 * UHashTok convenience
 ********************************************************************/

/**
 * Return a UHashTok for an integer.
 */
/*U_CAPI UHashTok U_EXPORT2
uhash_toki(int32_t i) {
    UHashTok tok;
    tok.integer = i;
    return tok;
}*/

/**
 * Return a UHashTok for a pointer.
 */
/*U_CAPI UHashTok U_EXPORT2
uhash_tokp(void* p) {
    UHashTok tok;
    tok.pointer = p;
    return tok;
}*/

/********************************************************************
 * PUBLIC Key Hash Functions
 ********************************************************************/

U_CAPI int32_t U_EXPORT2
uhash_hashUChars(ਅਚਲ UHashTok ਕੁੰਜੀ) {
    ਅਚਲ char16_t *s = (ਅਚਲ char16_t *)ਕੁੰਜੀ.ਸੂਚਕ;
    ਵਾਪਸ s == ਖਾਲੀ_ਸੂਚਕ ? 0 : ustr_hashUCharsN(s, u_strlen(s));
}

U_CAPI int32_t U_EXPORT2
uhash_hashChars(ਅਚਲ UHashTok ਕੁੰਜੀ) {
    ਅਚਲ ਅੱਖਰ *s = (ਅਚਲ ਅੱਖਰ *)ਕੁੰਜੀ.ਸੂਚਕ;
    ਵਾਪਸ s == ਖਾਲੀ_ਸੂਚਕ ? 0 : static_cast<int32_t>(ustr_hashCharsN(s, static_cast<int32_t>(uprv_strlen(s))));
}

U_CAPI int32_t U_EXPORT2
uhash_hashIChars(ਅਚਲ UHashTok ਕੁੰਜੀ) {
    ਅਚਲ ਅੱਖਰ *s = (ਅਚਲ ਅੱਖਰ *)ਕੁੰਜੀ.ਸੂਚਕ;
    ਵਾਪਸ s == ਖਾਲੀ_ਸੂਚਕ ? 0 : ustr_hashICharsN(s, static_cast<int32_t>(uprv_strlen(s)));
}

U_CAPI int32_t U_EXPORT2
uhash_hashIStringView(ਅਚਲ UHashTok ਕੁੰਜੀ) {
    ਅਚਲ std::string_view* s = static_cast<std::string_view*>(ਕੁੰਜੀ.ਸੂਚਕ);
    ਵਾਪਸ s == ਖਾਲੀ_ਸੂਚਕ ? 0 : ustr_hashICharsN(s->ਅੰਕੜੇ(), static_cast<int32_t>(s->size()));
}

U_CAPI UBool U_EXPORT2
uhash_equals(ਅਚਲ UHashtable* hash1, ਅਚਲ UHashtable* hash2){
    int32_t count1, count2, pos, i;

    ਜੇ(hash1==hash2){
        ਵਾਪਸ ਸੱਚ;
    }

    /*
     * Make sure that we are comparing 2 valid hashes of the same type
     * with valid comparison functions.
     * Without valid comparison functions, a binary comparison
     * of the hash values will yield random results on machines
     * with 64-bit pointers and 32-bit integer hashes.
     * A valueComparator is normally optional.
     */
    ਜੇ (hash1==ਖਾਲੀ_ਸੂਚਕ || hash2==ਖਾਲੀ_ਸੂਚਕ ||
        hash1->ਕੁੰਜੀ_Comparator != hash2->ਕੁੰਜੀ_Comparator ||
        hash1->ਮੁੱਲ_Comparator != hash2->ਮੁੱਲ_Comparator ||
        hash1->ਮੁੱਲ_Comparator == ਖਾਲੀ_ਸੂਚਕ)
    {
        /*
        Normally we would return an error here about incompatible hash tables,
        but we return false instead.
        */
        ਵਾਪਸ ਝੂਠ;
    }

    count1 = uhash_ਗਿਣਤੀ(hash1);
    count2 = uhash_ਗਿਣਤੀ(hash2);
    ਜੇ(count1!=count2){
        ਵਾਪਸ ਝੂਠ;
    }

    pos=UHASH_ਪਹਿਲਾ;
    ਲਈ(i=0; i<count1; i++){
        ਅਚਲ UHashElement* elem1 = uhash_ਅਗਲਾ_Element(hash1, &pos);
        ਅਚਲ UHashTok key1 = elem1->ਕੁੰਜੀ;
        ਅਚਲ UHashTok val1 = elem1->value;
        /* here the keys are not compared, instead the key form hash1 is used to fetch
         * value from hash2. If the hashes are equal then then both hashes should
         * contain equal values for the same key!
         */
        ਅਚਲ UHashElement* elem2 = uhash_ਲੱਭੋ_2(hash2, key1, hash2->ਕੁੰਜੀ_Hasher(key1));
        ਅਚਲ UHashTok val2 = elem2->value;
        ਜੇ(hash1->ਮੁੱਲ_Comparator(val1, val2)==ਝੂਠ){
            ਵਾਪਸ ਝੂਠ;
        }
    }
    ਵਾਪਸ ਸੱਚ;
}

/********************************************************************
 * PUBLIC Comparator Functions
 ********************************************************************/

U_CAPI UBool U_EXPORT2
uhash_compareUChars(ਅਚਲ UHashTok key1, ਅਚਲ UHashTok key2) {
    ਅਚਲ char16_t *p1 = (ਅਚਲ char16_t*) key1.ਸੂਚਕ;
    ਅਚਲ char16_t *p2 = (ਅਚਲ char16_t*) key2.ਸੂਚਕ;
    ਜੇ (p1 == p2) {
        ਵਾਪਸ ਸੱਚ;
    }
    ਜੇ (p1 == ਖਾਲੀ_ਸੂਚਕ || p2 == ਖਾਲੀ_ਸੂਚਕ) {
        ਵਾਪਸ ਝੂਠ;
    }
    ਜਦੋਂ (*p1 != 0 && *p1 == *p2) {
        ++p1;
        ++p2;
    }
    ਵਾਪਸ *p1 == *p2;
}

U_CAPI UBool U_EXPORT2
uhash_compareChars(ਅਚਲ UHashTok key1, ਅਚਲ UHashTok key2) {
    ਅਚਲ ਅੱਖਰ *p1 = (ਅਚਲ ਅੱਖਰ*) key1.ਸੂਚਕ;
    ਅਚਲ ਅੱਖਰ *p2 = (ਅਚਲ ਅੱਖਰ*) key2.ਸੂਚਕ;
    ਜੇ (p1 == p2) {
        ਵਾਪਸ ਸੱਚ;
    }
    ਜੇ (p1 == ਖਾਲੀ_ਸੂਚਕ || p2 == ਖਾਲੀ_ਸੂਚਕ) {
        ਵਾਪਸ ਝੂਠ;
    }
    ਜਦੋਂ (*p1 != 0 && *p1 == *p2) {
        ++p1;
        ++p2;
    }
    ਵਾਪਸ *p1 == *p2;
}

U_CAPI UBool U_EXPORT2
uhash_compareIChars(ਅਚਲ UHashTok key1, ਅਚਲ UHashTok key2) {
    ਅਚਲ ਅੱਖਰ *p1 = (ਅਚਲ ਅੱਖਰ*) key1.ਸੂਚਕ;
    ਅਚਲ ਅੱਖਰ *p2 = (ਅਚਲ ਅੱਖਰ*) key2.ਸੂਚਕ;
    ਜੇ (p1 == p2) {
        ਵਾਪਸ ਸੱਚ;
    }
    ਜੇ (p1 == ਖਾਲੀ_ਸੂਚਕ || p2 == ਖਾਲੀ_ਸੂਚਕ) {
        ਵਾਪਸ ਝੂਠ;
    }
    ਜਦੋਂ (*p1 != 0 && uprv_tolower(*p1) == uprv_tolower(*p2)) {
        ++p1;
        ++p2;
    }
    ਵਾਪਸ *p1 == *p2;
}

U_CAPI UBool U_EXPORT2
uhash_compareIStringView(ਅਚਲ UHashTok key1, ਅਚਲ UHashTok key2) {
    ਅਚਲ std::string_view* p1 = static_cast<std::string_view*>(key1.ਸੂਚਕ);
    ਅਚਲ std::string_view* p2 = static_cast<std::string_view*>(key2.ਸੂਚਕ);
    ਜੇ (p1 == p2) {
        ਵਾਪਸ ਸੱਚ;
    }
    ਜੇ (p1 == ਖਾਲੀ_ਸੂਚਕ || p2 == ਖਾਲੀ_ਸੂਚਕ) {
        ਵਾਪਸ ਝੂਠ;
    }
    ਅਚਲ std::string_view& v1 = *p1;
    ਅਚਲ std::string_view& v2 = *p2;
    ਜੇ (v1.size() != v2.size()) {
        ਵਾਪਸ ਝੂਠ;
    }
    ਲਈ (size_t i = 0; i < v1.size(); ++i) {
        ਜੇ (uprv_tolower(v1[i]) != uprv_tolower(v2[i])) {
            ਵਾਪਸ ਝੂਠ;
        }
    }
    ਵਾਪਸ ਸੱਚ;
}

/********************************************************************
 * PUBLIC int32_t Support Functions
 ********************************************************************/

U_CAPI int32_t U_EXPORT2
uhash_hashLong(ਅਚਲ UHashTok ਕੁੰਜੀ) {
    ਵਾਪਸ ਕੁੰਜੀ.integer;
}

U_CAPI UBool U_EXPORT2
uhash_compareLong(ਅਚਲ UHashTok key1, ਅਚਲ UHashTok key2) {
    ਵਾਪਸ key1.integer == key2.integer;
}
