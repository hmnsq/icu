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

#ಅನ್ತರ್ಭಾವ <string_view>

#ಅನ್ತರ್ಭಾವ "uhash.h"
#ಅನ್ತರ್ಭಾವ "unicode/ustring.h"
#ಅನ್ತರ್ಭಾವ "cstring.h"
#ಅನ್ತರ್ಭಾವ "cmemory.h"
#ಅನ್ತರ್ಭಾವ "uassert.h"
#ಅನ್ತರ್ಭಾವ "ustr_imp.h"

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
ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ int32_t ಅವಿಭಾಜ್ಯಗಳು[] = {
    7, 13, 31, 61, 127, 251, 509, 1021, 2039, 4093, 8191, 16381, 32749,
    65521, 131071, 262139, 524287, 1048573, 2097143, 4194301, 8388593,
    16777213, 33554393, 67108859, 134217689, 268435399, 536870909,
    1073741789, 2147483647 /*, 4294967291 */
};

#ವ್ಯಾಖ್ಯಾನಿಸು ಅವಿಭಾಜ್ಯಗಳು_ಉದ್ದಳತೆ UPRV_LENGTHOF(ಅವಿಭಾಜ್ಯಗಳು)
#ವ್ಯಾಖ್ಯಾನಿಸು DEFAULT_ಅವಿಭಾಜ್ಯ_ಸೂಚ್ಯಂಕ 4

/* These ratios are tuned to the PRIMES array such that a resize
 * places the table back into the zone of non-resizing.  That is,
 * after a call to _uhash_rehash(), a subsequent call to
 * _uhash_rehash() should do nothing (should not churn).  This is only
 * a potential problem with U_GROW_AND_SHRINK.
 */
ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ದಶಮಾಂಶ RESIZE_POLICY_ಅನುಪಾತ_ಕೋಷ್ಟಕ[6] = {
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
#ವ್ಯಾಖ್ಯಾನಿಸು HASH_DELETED    ((int32_t) 0x80000000)
#ವ್ಯಾಖ್ಯಾನಿಸು HASH_ಖಾಲಿಗಳು      ((int32_t) HASH_DELETED + 1)

#ವ್ಯಾಖ್ಯಾನಿಸು IS_ಖಾಲಿಗಳು_OR_DELETED(x) ((x) < 0)

/* This macro expects a UHashTok.pointer as its keypointer and
   valuepointer parameters */
#ವ್ಯಾಖ್ಯಾನಿಸು HASH_DELETE_ಕೀಲಿ_ಮೌಲ್ಯ(hash, keypointer, valuepointer) UPRV_ಖಂಡ_MACRO_BEGIN { \
    ಆದರೆ (hash->ಕೀಲಿ_Deleter != ಶೂನ್ಯಸೂಚಕ && keypointer != ಶೂನ್ಯಸೂಚಕ) { \
        (*hash->ಕೀಲಿ_Deleter)(keypointer); \
    } \
    ಆದರೆ (hash->ಮೌಲ್ಯ_Deleter != ಶೂನ್ಯಸೂಚಕ && valuepointer != ಶೂನ್ಯಸೂಚಕ) { \
        (*hash->ಮೌಲ್ಯ_Deleter)(valuepointer); \
    } \
} UPRV_ಖಂಡ_MACRO_ಕೊನೆ

/*
 * Constants for hinting whether a key or value is an integer
 * or a pointer.  If a hint bit is zero, then the associated
 * token is assumed to be an integer.
 */
#ವ್ಯಾಖ್ಯಾನಿಸು HINT_BOTH_INTEGERS (0)
#ವ್ಯಾಖ್ಯಾನಿಸು HINT_ಕೀಲಿ_ಸೂಚಕ   (1)
#ವ್ಯಾಖ್ಯಾನಿಸು HINT_ಮೌಲ್ಯ_ಸೂಚಕ (2)
#ವ್ಯಾಖ್ಯಾನಿಸು HINT_ALLOW_ZERO    (4)

/********************************************************************
 * PRIVATE Implementation
 ********************************************************************/

ಸ್ಥಿರ UHashTok
uhash_ಗಣ_Element(UHashtable *hash, UHashElement* e,
                  int32_t hashcode,
                  UHashTok ಕೀಲಿ, UHashTok value, int8_t hint) {

    UHashTok old_ಮೌಲ್ಯ = e->value;
    ಆದರೆ (hash->ಕೀಲಿ_Deleter != ಶೂನ್ಯಸೂಚಕ && e->ಕೀಲಿ.ಸೂಚಕ != ಶೂನ್ಯಸೂಚಕ &&
        e->ಕೀಲಿ.ಸೂಚಕ != ಕೀಲಿ.ಸೂಚಕ) { /* Avoid double deletion */
        (*hash->ಕೀಲಿ_Deleter)(e->ಕೀಲಿ.ಸೂಚಕ);
    }
    ಆದರೆ (hash->ಮೌಲ್ಯ_Deleter != ಶೂನ್ಯಸೂಚಕ) {
        ಆದರೆ (old_ಮೌಲ್ಯ.ಸೂಚಕ != ಶೂನ್ಯಸೂಚಕ &&
            old_ಮೌಲ್ಯ.ಸೂಚಕ != value.ಸೂಚಕ) { /* Avoid double deletion */
            (*hash->ಮೌಲ್ಯ_Deleter)(old_ಮೌಲ್ಯ.ಸೂಚಕ);
        }
        old_ಮೌಲ್ಯ.ಸೂಚಕ = ಶೂನ್ಯಸೂಚಕ;
    }
    /* Compilers should copy the UHashTok union correctly, but even if
     * they do, memory heap tools (e.g. BoundsChecker) can get
     * confused when a pointer is cloaked in a union and then copied.
     * TO ALLEVIATE THIS, we use hints (based on what API the user is
     * calling) to copy pointers when we know the user thinks
     * something is a pointer. */
    ಆದರೆ (hint & HINT_ಕೀಲಿ_ಸೂಚಕ) {
        e->ಕೀಲಿ.ಸೂಚಕ = ಕೀಲಿ.ಸೂಚಕ;
    } ಇಲ್ಲದಿದ್ದರೆ {
        e->ಕೀಲಿ = ಕೀಲಿ;
    }
    ಆದರೆ (hint & HINT_ಮೌಲ್ಯ_ಸೂಚಕ) {
        e->value.ಸೂಚಕ = value.ಸೂಚಕ;
    } ಇಲ್ಲದಿದ್ದರೆ {
        e->value = value;
    }
    e->hashcode = hashcode;
    ಹಿಂತಿರುಗಿ old_ಮೌಲ್ಯ;
}

/**
 * Assumes that the given element is not empty or deleted.
 */
ಸ್ಥಿರ UHashTok
uhash_ಆಂತರಿಕ_Remove_Element(UHashtable *hash, UHashElement* e) {
    UHashTok empty;
    U_ASSERT(!IS_ಖಾಲಿಗಳು_OR_DELETED(e->hashcode));
    --hash->count;
    empty.ಸೂಚಕ = ಶೂನ್ಯಸೂಚಕ; empty.integer = 0;
    ಹಿಂತಿರುಗಿ uhash_ಗಣ_Element(hash, e, HASH_DELETED, empty, empty, 0);
}

ಸ್ಥಿರ ಶೂನ್ಯ
uhash_ಆಂತರಿಕ_ಗಣ_Resize_Policy(UHashtable *hash, ಎಣಿಕೆ UHashResizePolicy policy) {
    U_ASSERT(hash != ಶೂನ್ಯಸೂಚಕ);
    U_ASSERT(((int32_t)policy) >= 0);
    U_ASSERT(((int32_t)policy) < 3);
    hash->ಕೆಳಗಿನ_Water_ಅನುಪಾತ  = RESIZE_POLICY_ಅನುಪಾತ_ಕೋಷ್ಟಕ[policy * 2];
    hash->ಮೇಲಿನ_Water_ಅನುಪಾತ = RESIZE_POLICY_ಅನುಪಾತ_ಕೋಷ್ಟಕ[policy * 2 + 1];
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
ಸ್ಥಿರ ಶೂನ್ಯ
_uhash_allocate(UHashtable *hash,
                int32_t ಅವಿಭಾಜ್ಯ_ಸೂಚ್ಯಂಕ,
                UError_ಸಂಕೇತ *ಸ್ಥಿತಿ_2) {

    UHashElement *p, *ಎಲ್ಲೆ;
    UHashTok emptytok;

    ಆದರೆ (U_FAILURE(*ಸ್ಥಿತಿ_2)) ಹಿಂತಿರುಗಿ;

    U_ASSERT(ಅವಿಭಾಜ್ಯ_ಸೂಚ್ಯಂಕ >= 0 && ಅವಿಭಾಜ್ಯ_ಸೂಚ್ಯಂಕ < ಅವಿಭಾಜ್ಯಗಳು_ಉದ್ದಳತೆ);

    hash->ಅವಿಭಾಜ್ಯ_ಸೂಚ್ಯಂಕ = static_cast<int8_t>(ಅವಿಭಾಜ್ಯ_ಸೂಚ್ಯಂಕ);
    hash->length = ಅವಿಭಾಜ್ಯಗಳು[ಅವಿಭಾಜ್ಯ_ಸೂಚ್ಯಂಕ];

    p = hash->elements = static_cast<UHashElement*>(
        uprv_malloc(ಗಾತ್ರ(UHashElement) * hash->length));

    ಆದರೆ (hash->elements == ಶೂನ್ಯಸೂಚಕ) {
        *ಸ್ಥಿತಿ_2 = U_ಸ್ಮರಣೆ_ಹಂಚಿಕೆ_ERROR;
        ಹಿಂತಿರುಗಿ;
    }

    emptytok.ಸೂಚಕ = ಶೂನ್ಯಸೂಚಕ; /* Only one of these two is needed */
    emptytok.integer = 0;    /* but we don't know which one. */

    ಎಲ್ಲೆ = p + hash->length;
    ಆಗ (p < ಎಲ್ಲೆ) {
        p->ಕೀಲಿ = emptytok;
        p->value = emptytok;
        p->hashcode = HASH_ಖಾಲಿಗಳು;
        ++p;
    }

    hash->count = 0;
    hash->ಕೆಳಗಿನ_Water_Mark = static_cast<int32_t>(hash->length * hash->ಕೆಳಗಿನ_Water_ಅನುಪಾತ);
    hash->ಮೇಲಿನ_Water_Mark = static_cast<int32_t>(hash->length * hash->ಮೇಲಿನ_Water_ಅನುಪಾತ);
}

ಸ್ಥಿರ UHashtable*
_uhash_init(UHashtable *ಫಲಿತಾಂಶ,
              UHashFunction *ಕೀಲಿ_Hash,
              UKeyComparator *ಕೀಲಿ_Comp,
              UValueComparator *ಮೌಲ್ಯ_Comp,
              int32_t ಅವಿಭಾಜ್ಯ_ಸೂಚ್ಯಂಕ,
              UError_ಸಂಕೇತ *ಸ್ಥಿತಿ_2)
{
    ಆದರೆ (U_FAILURE(*ಸ್ಥಿತಿ_2)) ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
    U_ASSERT(ಕೀಲಿ_Hash != ಶೂನ್ಯಸೂಚಕ);
    U_ASSERT(ಕೀಲಿ_Comp != ಶೂನ್ಯಸೂಚಕ);

    ಫಲಿತಾಂಶ->ಕೀಲಿ_Hasher       = ಕೀಲಿ_Hash;
    ಫಲಿತಾಂಶ->ಕೀಲಿ_Comparator   = ಕೀಲಿ_Comp;
    ಫಲಿತಾಂಶ->ಮೌಲ್ಯ_Comparator = ಮೌಲ್ಯ_Comp;
    ಫಲಿತಾಂಶ->ಕೀಲಿ_Deleter      = ಶೂನ್ಯಸೂಚಕ;
    ಫಲಿತಾಂಶ->ಮೌಲ್ಯ_Deleter    = ಶೂನ್ಯಸೂಚಕ;
    ಫಲಿತಾಂಶ->allocated       = ಸುಳ್ಳು;
    uhash_ಆಂತರಿಕ_ಗಣ_Resize_Policy(ಫಲಿತಾಂಶ, U_GROW);

    _uhash_allocate(ಫಲಿತಾಂಶ, ಅವಿಭಾಜ್ಯ_ಸೂಚ್ಯಂಕ, ಸ್ಥಿತಿ_2);

    ಆದರೆ (U_FAILURE(*ಸ್ಥಿತಿ_2)) {
        ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
    }

    ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

ಸ್ಥಿರ UHashtable*
_uhash_create(UHashFunction *ಕೀಲಿ_Hash,
              UKeyComparator *ಕೀಲಿ_Comp,
              UValueComparator *ಮೌಲ್ಯ_Comp,
              int32_t ಅವಿಭಾಜ್ಯ_ಸೂಚ್ಯಂಕ,
              UError_ಸಂಕೇತ *ಸ್ಥಿತಿ_2) {
    UHashtable *ಫಲಿತಾಂಶ;

    ಆದರೆ (U_FAILURE(*ಸ್ಥಿತಿ_2)) ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;

    ಫಲಿತಾಂಶ = static_cast<UHashtable*>(uprv_malloc(ಗಾತ್ರ(UHashtable)));
    ಆದರೆ (ಫಲಿತಾಂಶ == ಶೂನ್ಯಸೂಚಕ) {
        *ಸ್ಥಿತಿ_2 = U_ಸ್ಮರಣೆ_ಹಂಚಿಕೆ_ERROR;
        ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
    }

    _uhash_init(ಫಲಿತಾಂಶ, ಕೀಲಿ_Hash, ಕೀಲಿ_Comp, ಮೌಲ್ಯ_Comp, ಅವಿಭಾಜ್ಯ_ಸೂಚ್ಯಂಕ, ಸ್ಥಿತಿ_2);
    ಫಲಿತಾಂಶ->allocated       = ನಿಜ;

    ಆದರೆ (U_FAILURE(*ಸ್ಥಿತಿ_2)) {
        uprv_free(ಫಲಿತಾಂಶ);
        ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
    }

    ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
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
ಸ್ಥಿರ UHashElement*
uhash_ಕಂಡುಹಿಡಿ_2(ಸ್ಥಿರಾಂಕ UHashtable *hash, UHashTok ಕೀಲಿ,
            int32_t hashcode) {

    int32_t ಮೊದಲ_Deleted = -1;  /* assume invalid index */
    int32_t the_ಸೂಚ್ಯಂಕ, ಆರಂಭ_ಸೂಚ್ಯಂಕ;
    int32_t jump = 0; /* lazy evaluate */
    int32_t ಕೋಷ್ಟಕ_Hash;
    UHashElement *elements = hash->elements;

    hashcode &= 0x7FFFFFFF; /* must be positive */
    ಆರಂಭ_ಸೂಚ್ಯಂಕ = the_ಸೂಚ್ಯಂಕ = (hashcode ^ 0x4000000) % hash->length;

    ಮಾಡು {
        ಕೋಷ್ಟಕ_Hash = elements[the_ಸೂಚ್ಯಂಕ].hashcode;
        ಆದರೆ (ಕೋಷ್ಟಕ_Hash == hashcode) {          /* quick check */
            ಆದರೆ ((*hash->ಕೀಲಿ_Comparator)(ಕೀಲಿ, elements[the_ಸೂಚ್ಯಂಕ].ಕೀಲಿ)) {
                ಹಿಂತಿರುಗಿ &(elements[the_ಸೂಚ್ಯಂಕ]);
            }
        } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (!IS_ಖಾಲಿಗಳು_OR_DELETED(ಕೋಷ್ಟಕ_Hash)) {
            /* We have hit a slot which contains a key-value pair,
             * but for which the hash code does not match.  Keep
             * looking.
             */
        } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (ಕೋಷ್ಟಕ_Hash == HASH_ಖಾಲಿಗಳು) { /* empty, end o' the line */
            ನಿಲ್ಲಿಸು;
        } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (ಮೊದಲ_Deleted < 0) { /* remember first deleted */
            ಮೊದಲ_Deleted = the_ಸೂಚ್ಯಂಕ;
        }
        ಆದರೆ (jump == 0) { /* lazy compute jump */
            /* The jump value must be relatively prime to the table
             * length.  As long as the length is prime, then any value
             * 1..length-1 will be relatively prime to it.
             */
            jump = (hashcode % (hash->length - 1)) + 1;
        }
        the_ಸೂಚ್ಯಂಕ = (the_ಸೂಚ್ಯಂಕ + jump) % hash->length;
    } ಆಗ (the_ಸೂಚ್ಯಂಕ != ಆರಂಭ_ಸೂಚ್ಯಂಕ);

    ಆದರೆ (ಮೊದಲ_Deleted >= 0) {
        the_ಸೂಚ್ಯಂಕ = ಮೊದಲ_Deleted; /* reset if had deleted slot */
    } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (ಕೋಷ್ಟಕ_Hash != HASH_ಖಾಲಿಗಳು) {
        /* We get to this point if the hashtable is full (no empty or
         * deleted slots), and we've failed to find a match.  THIS
         * WILL NEVER HAPPEN as long as uhash_put() makes sure that
         * count is always < length.
         */
        UPRV_UNREACHABLE_EXIT;
    }
    ಹಿಂತಿರುಗಿ &(elements[the_ಸೂಚ್ಯಂಕ]);
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
ಸ್ಥಿರ ಶೂನ್ಯ
_uhash_rehash(UHashtable *hash, UError_ಸಂಕೇತ *ಸ್ಥಿತಿ_2) {

    UHashElement *old = hash->elements;
    int32_t old_ಉದ್ದಳತೆ = hash->length;
    int32_t new_ಅವಿಭಾಜ್ಯ_ಸೂಚ್ಯಂಕ = hash->ಅವಿಭಾಜ್ಯ_ಸೂಚ್ಯಂಕ;
    int32_t i;

    ಆದರೆ (hash->count > hash->ಮೇಲಿನ_Water_Mark) {
        ಆದರೆ (++new_ಅವಿಭಾಜ್ಯ_ಸೂಚ್ಯಂಕ >= ಅವಿಭಾಜ್ಯಗಳು_ಉದ್ದಳತೆ) {
            ಹಿಂತಿರುಗಿ;
        }
    } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (hash->count < hash->ಕೆಳಗಿನ_Water_Mark) {
        ಆದರೆ (--new_ಅವಿಭಾಜ್ಯ_ಸೂಚ್ಯಂಕ < 0) {
            ಹಿಂತಿರುಗಿ;
        }
    } ಇಲ್ಲದಿದ್ದರೆ {
        ಹಿಂತಿರುಗಿ;
    }

    _uhash_allocate(hash, new_ಅವಿಭಾಜ್ಯ_ಸೂಚ್ಯಂಕ, ಸ್ಥಿತಿ_2);

    ಆದರೆ (U_FAILURE(*ಸ್ಥಿತಿ_2)) {
        hash->elements = old;
        hash->length = old_ಉದ್ದಳತೆ;
        ಹಿಂತಿರುಗಿ;
    }

    ಗಾಗಿ (i = old_ಉದ್ದಳತೆ - 1; i >= 0; --i) {
        ಆದರೆ (!IS_ಖಾಲಿಗಳು_OR_DELETED(old[i].hashcode)) {
            UHashElement *e = uhash_ಕಂಡುಹಿಡಿ_2(hash, old[i].ಕೀಲಿ, old[i].hashcode);
            U_ASSERT(e != ಶೂನ್ಯಸೂಚಕ);
            U_ASSERT(e->hashcode == HASH_ಖಾಲಿಗಳು);
            e->ಕೀಲಿ = old[i].ಕೀಲಿ;
            e->value = old[i].value;
            e->hashcode = old[i].hashcode;
            ++hash->count;
        }
    }

    uprv_free(old);
}

ಸ್ಥಿರ UHashTok
_uhash_remove(UHashtable *hash,
              UHashTok ಕೀಲಿ) {
    /* First find the position of the key in the table.  If the object
     * has not been removed already, remove it.  If the user wanted
     * keys deleted, then delete it also.  We have to put a special
     * hashcode in that position that means that something has been
     * deleted, since when we do a find, we have to continue PAST any
     * deleted values.
     */
    UHashTok ಫಲಿತಾಂಶ;
    UHashElement* e = uhash_ಕಂಡುಹಿಡಿ_2(hash, ಕೀಲಿ, hash->ಕೀಲಿ_Hasher(ಕೀಲಿ));
    U_ASSERT(e != ಶೂನ್ಯಸೂಚಕ);
    ಫಲಿತಾಂಶ.ಸೂಚಕ = ಶೂನ್ಯಸೂಚಕ;
    ಫಲಿತಾಂಶ.integer = 0;
    ಆದರೆ (!IS_ಖಾಲಿಗಳು_OR_DELETED(e->hashcode)) {
        ಫಲಿತಾಂಶ = uhash_ಆಂತರಿಕ_Remove_Element(hash, e);
        ಆದರೆ (hash->count < hash->ಕೆಳಗಿನ_Water_Mark) {
            UError_ಸಂಕೇತ ಸ್ಥಿತಿ_2 = U_ZERO_ERROR;
            _uhash_rehash(hash, &ಸ್ಥಿತಿ_2);
        }
    }
    ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

ಸ್ಥಿರ UHashTok
_uhash_put(UHashtable *hash,
           UHashTok ಕೀಲಿ,
           UHashTok value,
           int8_t hint,
           UError_ಸಂಕೇತ *ಸ್ಥಿತಿ_2) {

    /* Put finds the position in the table for the new value.  If the
     * key is already in the table, it is deleted, if there is a
     * non-nullptr keyDeleter.  Then the key, the hash and the value are
     * all put at the position in their respective arrays.
     */
    int32_t hashcode;
    UHashElement* e;
    UHashTok emptytok;

    ಆದರೆ (U_FAILURE(*ಸ್ಥಿತಿ_2)) {
        ಹೋಗು err;
    }
    U_ASSERT(hash != ಶೂನ್ಯಸೂಚಕ);
    ಆದರೆ ((hint & HINT_ಮೌಲ್ಯ_ಸೂಚಕ) ?
            value.ಸೂಚಕ == ಶೂನ್ಯಸೂಚಕ :
            value.integer == 0 && (hint & HINT_ALLOW_ZERO) == 0) {
        /* Disallow storage of nullptr values, since nullptr is returned by
         * get() to indicate an absent key.  Storing nullptr == removing.
         */
        ಹಿಂತಿರುಗಿ _uhash_remove(hash, ಕೀಲಿ);
    }
    ಆದರೆ (hash->count > hash->ಮೇಲಿನ_Water_Mark) {
        _uhash_rehash(hash, ಸ್ಥಿತಿ_2);
        ಆದರೆ (U_FAILURE(*ಸ್ಥಿತಿ_2)) {
            ಹೋಗು err;
        }
    }

    hashcode = (*hash->ಕೀಲಿ_Hasher)(ಕೀಲಿ);
    e = uhash_ಕಂಡುಹಿಡಿ_2(hash, ಕೀಲಿ, hashcode);
    U_ASSERT(e != ಶೂನ್ಯಸೂಚಕ);

    ಆದರೆ (IS_ಖಾಲಿಗಳು_OR_DELETED(e->hashcode)) {
        /* Important: We must never actually fill the table up.  If we
         * do so, then _uhash_find() will return nullptr, and we'll have
         * to check for nullptr after every call to _uhash_find().  To
         * avoid this we make sure there is always at least one empty
         * or deleted slot in the table.  This only is a problem if we
         * are out of memory and rehash isn't working.
         */
        ++hash->count;
        ಆದರೆ (hash->count == hash->length) {
            /* Don't allow count to reach length */
            --hash->count;
            *ಸ್ಥಿತಿ_2 = U_ಸ್ಮರಣೆ_ಹಂಚಿಕೆ_ERROR;
            ಹೋಗು err;
        }
    }

    /* We must in all cases handle storage properly.  If there was an
     * old key, then it must be deleted (if the deleter != nullptr).
     * Make hashcodes stored in table positive.
     */
    ಹಿಂತಿರುಗಿ uhash_ಗಣ_Element(hash, e, hashcode & 0x7FFFFFFF, ಕೀಲಿ, value, hint);

 err:
    /* If the deleters are non-nullptr, this method adopts its key and/or
     * value arguments, and we must be sure to delete the key and/or
     * value in all cases, even upon failure.
     */
    HASH_DELETE_ಕೀಲಿ_ಮೌಲ್ಯ(hash, ಕೀಲಿ.ಸೂಚಕ, value.ಸೂಚಕ);
    emptytok.ಸೂಚಕ = ಶೂನ್ಯಸೂಚಕ; emptytok.integer = 0;
    ಹಿಂತಿರುಗಿ emptytok;
}


/********************************************************************
 * PUBLIC API
 ********************************************************************/

U_CAPI UHashtable* U_EXPORT2
uhash_open(UHashFunction *ಕೀಲಿ_Hash,
           UKeyComparator *ಕೀಲಿ_Comp,
           UValueComparator *ಮೌಲ್ಯ_Comp,
           UError_ಸಂಕೇತ *ಸ್ಥಿತಿ_2) {

    ಹಿಂತಿರುಗಿ _uhash_create(ಕೀಲಿ_Hash, ಕೀಲಿ_Comp, ಮೌಲ್ಯ_Comp, DEFAULT_ಅವಿಭಾಜ್ಯ_ಸೂಚ್ಯಂಕ, ಸ್ಥಿತಿ_2);
}

U_CAPI UHashtable* U_EXPORT2
uhash_open_ಗಾತ್ರ(UHashFunction *ಕೀಲಿ_Hash,
               UKeyComparator *ಕೀಲಿ_Comp,
               UValueComparator *ಮೌಲ್ಯ_Comp,
               int32_t size,
               UError_ಸಂಕೇತ *ಸ್ಥಿತಿ_2) {

    /* Find the smallest index i for which PRIMES[i] >= size. */
    int32_t i = 0;
    ಆಗ (i<(ಅವಿಭಾಜ್ಯಗಳು_ಉದ್ದಳತೆ-1) && ಅವಿಭಾಜ್ಯಗಳು[i]<size) {
        ++i;
    }

    ಹಿಂತಿರುಗಿ _uhash_create(ಕೀಲಿ_Hash, ಕೀಲಿ_Comp, ಮೌಲ್ಯ_Comp, i, ಸ್ಥಿತಿ_2);
}

U_CAPI UHashtable* U_EXPORT2
uhash_init(UHashtable *fillin_ಫಲಿತಾಂಶ,
           UHashFunction *ಕೀಲಿ_Hash,
           UKeyComparator *ಕೀಲಿ_Comp,
           UValueComparator *ಮೌಲ್ಯ_Comp,
           UError_ಸಂಕೇತ *ಸ್ಥಿತಿ_2) {

    ಹಿಂತಿರುಗಿ _uhash_init(fillin_ಫಲಿತಾಂಶ, ಕೀಲಿ_Hash, ಕೀಲಿ_Comp, ಮೌಲ್ಯ_Comp, DEFAULT_ಅವಿಭಾಜ್ಯ_ಸೂಚ್ಯಂಕ, ಸ್ಥಿತಿ_2);
}

U_CAPI UHashtable* U_EXPORT2
uhash_init_ಗಾತ್ರ(UHashtable *fillin_ಫಲಿತಾಂಶ,
               UHashFunction *ಕೀಲಿ_Hash,
               UKeyComparator *ಕೀಲಿ_Comp,
               UValueComparator *ಮೌಲ್ಯ_Comp,
               int32_t size,
               UError_ಸಂಕೇತ *ಸ್ಥಿತಿ_2) {

    // Find the smallest index i for which PRIMES[i] >= size.
    int32_t i = 0;
    ಆಗ (i<(ಅವಿಭಾಜ್ಯಗಳು_ಉದ್ದಳತೆ-1) && ಅವಿಭಾಜ್ಯಗಳು[i]<size) {
        ++i;
    }
    ಹಿಂತಿರುಗಿ _uhash_init(fillin_ಫಲಿತಾಂಶ, ಕೀಲಿ_Hash, ಕೀಲಿ_Comp, ಮೌಲ್ಯ_Comp, i, ಸ್ಥಿತಿ_2);
}

U_CAPI ಶೂನ್ಯ U_EXPORT2
uhash_close(UHashtable *hash) {
    ಆದರೆ (hash == ಶೂನ್ಯಸೂಚಕ) {
        ಹಿಂತಿರುಗಿ;
    }
    ಆದರೆ (hash->elements != ಶೂನ್ಯಸೂಚಕ) {
        ಆದರೆ (hash->ಕೀಲಿ_Deleter != ಶೂನ್ಯಸೂಚಕ || hash->ಮೌಲ್ಯ_Deleter != ಶೂನ್ಯಸೂಚಕ) {
            int32_t pos=UHASH_ಮೊದಲ;
            UHashElement *e;
            ಆಗ ((e = (UHashElement*) uhash_ಮುಂದಿನ_Element(hash, &pos)) != ಶೂನ್ಯಸೂಚಕ) {
                HASH_DELETE_ಕೀಲಿ_ಮೌಲ್ಯ(hash, e->ಕೀಲಿ.ಸೂಚಕ, e->value.ಸೂಚಕ);
            }
        }
        uprv_free(hash->elements);
        hash->elements = ಶೂನ್ಯಸೂಚಕ;
    }
    ಆದರೆ (hash->allocated) {
        uprv_free(hash);
    }
}

U_CAPI UHashFunction *U_EXPORT2
uhash_ಗಣ_ಕೀಲಿ_Hasher(UHashtable *hash, UHashFunction *fn) {
    UHashFunction *ಫಲಿತಾಂಶ = hash->ಕೀಲಿ_Hasher;
    hash->ಕೀಲಿ_Hasher = fn;
    ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

U_CAPI UKeyComparator *U_EXPORT2
uhash_ಗಣ_ಕೀಲಿ_Comparator(UHashtable *hash, UKeyComparator *fn) {
    UKeyComparator *ಫಲಿತಾಂಶ = hash->ಕೀಲಿ_Comparator;
    hash->ಕೀಲಿ_Comparator = fn;
    ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}
U_CAPI UValueComparator *U_EXPORT2
uhash_ಗಣ_ಮೌಲ್ಯ_Comparator(UHashtable *hash, UValueComparator *fn){
    UValueComparator *ಫಲಿತಾಂಶ = hash->ಮೌಲ್ಯ_Comparator;
    hash->ಮೌಲ್ಯ_Comparator = fn;
    ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

U_CAPI UObjectDeleter *U_EXPORT2
uhash_ಗಣ_ಕೀಲಿ_Deleter(UHashtable *hash, UObjectDeleter *fn) {
    UObjectDeleter *ಫಲಿತಾಂಶ = hash->ಕೀಲಿ_Deleter;
    hash->ಕೀಲಿ_Deleter = fn;
    ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

U_CAPI UObjectDeleter *U_EXPORT2
uhash_ಗಣ_ಮೌಲ್ಯ_Deleter(UHashtable *hash, UObjectDeleter *fn) {
    UObjectDeleter *ಫಲಿತಾಂಶ = hash->ಮೌಲ್ಯ_Deleter;
    hash->ಮೌಲ್ಯ_Deleter = fn;
    ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

U_CAPI ಶೂನ್ಯ U_EXPORT2
uhash_ಗಣ_Resize_Policy(UHashtable *hash, ಎಣಿಕೆ UHashResizePolicy policy) {
    UError_ಸಂಕೇತ ಸ್ಥಿತಿ_2 = U_ZERO_ERROR;
    uhash_ಆಂತರಿಕ_ಗಣ_Resize_Policy(hash, policy);
    hash->ಕೆಳಗಿನ_Water_Mark  = (int32_t)(hash->length * hash->ಕೆಳಗಿನ_Water_ಅನುಪಾತ);
    hash->ಮೇಲಿನ_Water_Mark = (int32_t)(hash->length * hash->ಮೇಲಿನ_Water_ಅನುಪಾತ);
    _uhash_rehash(hash, &ಸ್ಥಿತಿ_2);
}

U_CAPI int32_t U_EXPORT2
uhash_ಎಣಿಕೆ(ಸ್ಥಿರಾಂಕ UHashtable *hash) {
    ಹಿಂತಿರುಗಿ hash->count;
}

U_CAPI ಶೂನ್ಯ* U_EXPORT2
uhash_get(ಸ್ಥಿರಾಂಕ UHashtable *hash,
          ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ* ಕೀಲಿ) {
    UHashTok keyholder;
    keyholder.ಸೂಚಕ = (ಶೂನ್ಯ*) ಕೀಲಿ;
    ಹಿಂತಿರುಗಿ uhash_ಕಂಡುಹಿಡಿ_2(hash, keyholder, hash->ಕೀಲಿ_Hasher(keyholder))->value.ಸೂಚಕ;
}

U_CAPI ಶೂನ್ಯ* U_EXPORT2
uhash_iget(ಸ್ಥಿರಾಂಕ UHashtable *hash,
           int32_t ಕೀಲಿ) {
    UHashTok keyholder;
    keyholder.integer = ಕೀಲಿ;
    ಹಿಂತಿರುಗಿ uhash_ಕಂಡುಹಿಡಿ_2(hash, keyholder, hash->ಕೀಲಿ_Hasher(keyholder))->value.ಸೂಚಕ;
}

U_CAPI int32_t U_EXPORT2
uhash_geti(ಸ್ಥಿರಾಂಕ UHashtable *hash,
           ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ* ಕೀಲಿ) {
    UHashTok keyholder;
    keyholder.ಸೂಚಕ = (ಶೂನ್ಯ*) ಕೀಲಿ;
    ಹಿಂತಿರುಗಿ uhash_ಕಂಡುಹಿಡಿ_2(hash, keyholder, hash->ಕೀಲಿ_Hasher(keyholder))->value.integer;
}

U_CAPI int32_t U_EXPORT2
uhash_igeti(ಸ್ಥಿರಾಂಕ UHashtable *hash,
           int32_t ಕೀಲಿ) {
    UHashTok keyholder;
    keyholder.integer = ಕೀಲಿ;
    ಹಿಂತಿರುಗಿ uhash_ಕಂಡುಹಿಡಿ_2(hash, keyholder, hash->ಕೀಲಿ_Hasher(keyholder))->value.integer;
}

U_CAPI int32_t U_EXPORT2
uhash_geti_And_ಸಿಕ್ಕಿತು(ಸ್ಥಿರಾಂಕ UHashtable *hash,
                   ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *ಕೀಲಿ,
                   UBool *ಸಿಕ್ಕಿತು) {
    UHashTok keyholder;
    keyholder.ಸೂಚಕ = (ಶೂನ್ಯ *)ಕೀಲಿ;
    ಸ್ಥಿರಾಂಕ UHashElement *e = uhash_ಕಂಡುಹಿಡಿ_2(hash, keyholder, hash->ಕೀಲಿ_Hasher(keyholder));
    *ಸಿಕ್ಕಿತು = !IS_ಖಾಲಿಗಳು_OR_DELETED(e->hashcode);
    ಹಿಂತಿರುಗಿ e->value.integer;
}

U_CAPI int32_t U_EXPORT2
uhash_igeti_And_ಸಿಕ್ಕಿತು(ಸ್ಥಿರಾಂಕ UHashtable *hash,
                    int32_t ಕೀಲಿ,
                    UBool *ಸಿಕ್ಕಿತು) {
    UHashTok keyholder;
    keyholder.integer = ಕೀಲಿ;
    ಸ್ಥಿರಾಂಕ UHashElement *e = uhash_ಕಂಡುಹಿಡಿ_2(hash, keyholder, hash->ಕೀಲಿ_Hasher(keyholder));
    *ಸಿಕ್ಕಿತು = !IS_ಖಾಲಿಗಳು_OR_DELETED(e->hashcode);
    ಹಿಂತಿರುಗಿ e->value.integer;
}

U_CAPI ಶೂನ್ಯ* U_EXPORT2
uhash_put(UHashtable *hash,
          ಶೂನ್ಯ* ಕೀಲಿ,
          ಶೂನ್ಯ* value,
          UError_ಸಂಕೇತ *ಸ್ಥಿತಿ_2) {
    UHashTok keyholder, valueholder;
    keyholder.ಸೂಚಕ = ಕೀಲಿ;
    valueholder.ಸೂಚಕ = value;
    ಹಿಂತಿರುಗಿ _uhash_put(hash, keyholder, valueholder,
                      HINT_ಕೀಲಿ_ಸೂಚಕ | HINT_ಮೌಲ್ಯ_ಸೂಚಕ,
                      ಸ್ಥಿತಿ_2).ಸೂಚಕ;
}

U_CAPI ಶೂನ್ಯ* U_EXPORT2
uhash_iput(UHashtable *hash,
           int32_t ಕೀಲಿ,
           ಶೂನ್ಯ* value,
           UError_ಸಂಕೇತ *ಸ್ಥಿತಿ_2) {
    UHashTok keyholder, valueholder;
    keyholder.integer = ಕೀಲಿ;
    valueholder.ಸೂಚಕ = value;
    ಹಿಂತಿರುಗಿ _uhash_put(hash, keyholder, valueholder,
                      HINT_ಮೌಲ್ಯ_ಸೂಚಕ,
                      ಸ್ಥಿತಿ_2).ಸೂಚಕ;
}

U_CAPI int32_t U_EXPORT2
uhash_puti(UHashtable *hash,
           ಶೂನ್ಯ* ಕೀಲಿ,
           int32_t value,
           UError_ಸಂಕೇತ *ಸ್ಥಿತಿ_2) {
    UHashTok keyholder, valueholder;
    keyholder.ಸೂಚಕ = ಕೀಲಿ;
    valueholder.integer = value;
    ಹಿಂತಿರುಗಿ _uhash_put(hash, keyholder, valueholder,
                      HINT_ಕೀಲಿ_ಸೂಚಕ,
                      ಸ್ಥಿತಿ_2).integer;
}


U_CAPI int32_t U_EXPORT2
uhash_iputi(UHashtable *hash,
           int32_t ಕೀಲಿ,
           int32_t value,
           UError_ಸಂಕೇತ *ಸ್ಥಿತಿ_2) {
    UHashTok keyholder, valueholder;
    keyholder.integer = ಕೀಲಿ;
    valueholder.integer = value;
    ಹಿಂತಿರುಗಿ _uhash_put(hash, keyholder, valueholder,
                      HINT_BOTH_INTEGERS,
                      ಸ್ಥಿತಿ_2).integer;
}

U_CAPI int32_t U_EXPORT2
uhash_putiAllowZero(UHashtable *hash,
                    ಶೂನ್ಯ *ಕೀಲಿ,
                    int32_t value,
                    UError_ಸಂಕೇತ *ಸ್ಥಿತಿ_2) {
    UHashTok keyholder, valueholder;
    keyholder.ಸೂಚಕ = ಕೀಲಿ;
    valueholder.integer = value;
    ಹಿಂತಿರುಗಿ _uhash_put(hash, keyholder, valueholder,
                      HINT_ಕೀಲಿ_ಸೂಚಕ | HINT_ALLOW_ZERO,
                      ಸ್ಥಿತಿ_2).integer;
}


U_CAPI int32_t U_EXPORT2
uhash_iputiAllowZero(UHashtable *hash,
                     int32_t ಕೀಲಿ,
                     int32_t value,
                     UError_ಸಂಕೇತ *ಸ್ಥಿತಿ_2) {
    UHashTok keyholder, valueholder;
    keyholder.integer = ಕೀಲಿ;
    valueholder.integer = value;
    ಹಿಂತಿರುಗಿ _uhash_put(hash, keyholder, valueholder,
                      HINT_BOTH_INTEGERS | HINT_ALLOW_ZERO,
                      ಸ್ಥಿತಿ_2).integer;
}

U_CAPI ಶೂನ್ಯ* U_EXPORT2
uhash_remove(UHashtable *hash,
             ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ* ಕೀಲಿ) {
    UHashTok keyholder;
    keyholder.ಸೂಚಕ = (ಶೂನ್ಯ*) ಕೀಲಿ;
    ಹಿಂತಿರುಗಿ _uhash_remove(hash, keyholder).ಸೂಚಕ;
}

U_CAPI ಶೂನ್ಯ* U_EXPORT2
uhash_iremove(UHashtable *hash,
              int32_t ಕೀಲಿ) {
    UHashTok keyholder;
    keyholder.integer = ಕೀಲಿ;
    ಹಿಂತಿರುಗಿ _uhash_remove(hash, keyholder).ಸೂಚಕ;
}

U_CAPI int32_t U_EXPORT2
uhash_removei(UHashtable *hash,
              ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ* ಕೀಲಿ) {
    UHashTok keyholder;
    keyholder.ಸೂಚಕ = (ಶೂನ್ಯ*) ಕೀಲಿ;
    ಹಿಂತಿರುಗಿ _uhash_remove(hash, keyholder).integer;
}

U_CAPI int32_t U_EXPORT2
uhash_iremovei(UHashtable *hash,
               int32_t ಕೀಲಿ) {
    UHashTok keyholder;
    keyholder.integer = ಕೀಲಿ;
    ಹಿಂತಿರುಗಿ _uhash_remove(hash, keyholder).integer;
}

U_CAPI ಶೂನ್ಯ U_EXPORT2
uhash_removeAll(UHashtable *hash) {
    int32_t pos = UHASH_ಮೊದಲ;
    ಸ್ಥಿರಾಂಕ UHashElement *e;
    U_ASSERT(hash != ಶೂನ್ಯಸೂಚಕ);
    ಆದರೆ (hash->count != 0) {
        ಆಗ ((e = uhash_ಮುಂದಿನ_Element(hash, &pos)) != ಶೂನ್ಯಸೂಚಕ) {
            uhash_removeElement(hash, e);
        }
    }
    U_ASSERT(hash->count == 0);
}

U_CAPI UBool U_EXPORT2
uhash_ಒಳಗೊಂಡಿದೆ_ಕೀಲಿ(ಸ್ಥಿರಾಂಕ UHashtable *hash, ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *ಕೀಲಿ) {
    UHashTok keyholder;
    keyholder.ಸೂಚಕ = (ಶೂನ್ಯ *)ಕೀಲಿ;
    ಸ್ಥಿರಾಂಕ UHashElement *e = uhash_ಕಂಡುಹಿಡಿ_2(hash, keyholder, hash->ಕೀಲಿ_Hasher(keyholder));
    ಹಿಂತಿರುಗಿ !IS_ಖಾಲಿಗಳು_OR_DELETED(e->hashcode);
}

/**
 * Returns true if the UHashtable contains an item with this integer key.
 *
 * @param hash The target UHashtable.
 * @param key An integer key stored in a hashtable
 * @return true if the key is found.
 */
U_CAPI UBool U_EXPORT2
uhash_icontains_ಕೀಲಿ(ಸ್ಥಿರಾಂಕ UHashtable *hash, int32_t ಕೀಲಿ) {
    UHashTok keyholder;
    keyholder.integer = ಕೀಲಿ;
    ಸ್ಥಿರಾಂಕ UHashElement *e = uhash_ಕಂಡುಹಿಡಿ_2(hash, keyholder, hash->ಕೀಲಿ_Hasher(keyholder));
    ಹಿಂತಿರುಗಿ !IS_ಖಾಲಿಗಳು_OR_DELETED(e->hashcode);
}

U_CAPI ಸ್ಥಿರಾಂಕ UHashElement* U_EXPORT2
uhash_ಕಂಡುಹಿಡಿ(ಸ್ಥಿರಾಂಕ UHashtable *hash, ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ* ಕೀಲಿ) {
    UHashTok keyholder;
    ಸ್ಥಿರಾಂಕ UHashElement *e;
    keyholder.ಸೂಚಕ = (ಶೂನ್ಯ*) ಕೀಲಿ;
    e = uhash_ಕಂಡುಹಿಡಿ_2(hash, keyholder, hash->ಕೀಲಿ_Hasher(keyholder));
    ಹಿಂತಿರುಗಿ IS_ಖಾಲಿಗಳು_OR_DELETED(e->hashcode) ? ಶೂನ್ಯಸೂಚಕ : e;
}

U_CAPI ಸ್ಥಿರಾಂಕ UHashElement* U_EXPORT2
uhash_ಮುಂದಿನ_Element(ಸ್ಥಿರಾಂಕ UHashtable *hash, int32_t *pos) {
    /* Walk through the array until we find an element that is not
     * EMPTY and not DELETED.
     */
    int32_t i;
    U_ASSERT(hash != ಶೂನ್ಯಸೂಚಕ);
    ಗಾಗಿ (i = *pos + 1; i < hash->length; ++i) {
        ಆದರೆ (!IS_ಖಾಲಿಗಳು_OR_DELETED(hash->elements[i].hashcode)) {
            *pos = i;
            ಹಿಂತಿರುಗಿ &(hash->elements[i]);
        }
    }

    /* No more elements */
    ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
}

U_CAPI ಶೂನ್ಯ* U_EXPORT2
uhash_removeElement(UHashtable *hash, ಸ್ಥಿರಾಂಕ UHashElement* e) {
    U_ASSERT(hash != ಶೂನ್ಯಸೂಚಕ);
    U_ASSERT(e != ಶೂನ್ಯಸೂಚಕ);
    ಆದರೆ (!IS_ಖಾಲಿಗಳು_OR_DELETED(e->hashcode)) {
        UHashElement *nce = (UHashElement *)e;
        ಹಿಂತಿರುಗಿ uhash_ಆಂತರಿಕ_Remove_Element(hash, nce).ಸೂಚಕ;
    }
    ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
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
uhash_hashUChars(ಸ್ಥಿರಾಂಕ UHashTok ಕೀಲಿ) {
    ಸ್ಥಿರಾಂಕ char16_t *s = (ಸ್ಥಿರಾಂಕ char16_t *)ಕೀಲಿ.ಸೂಚಕ;
    ಹಿಂತಿರುಗಿ s == ಶೂನ್ಯಸೂಚಕ ? 0 : ustr_hashUCharsN(s, u_strlen(s));
}

U_CAPI int32_t U_EXPORT2
uhash_hashChars(ಸ್ಥಿರಾಂಕ UHashTok ಕೀಲಿ) {
    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *s = (ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *)ಕೀಲಿ.ಸೂಚಕ;
    ಹಿಂತಿರುಗಿ s == ಶೂನ್ಯಸೂಚಕ ? 0 : static_cast<int32_t>(ustr_hashCharsN(s, static_cast<int32_t>(uprv_strlen(s))));
}

U_CAPI int32_t U_EXPORT2
uhash_hashIChars(ಸ್ಥಿರಾಂಕ UHashTok ಕೀಲಿ) {
    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *s = (ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *)ಕೀಲಿ.ಸೂಚಕ;
    ಹಿಂತಿರುಗಿ s == ಶೂನ್ಯಸೂಚಕ ? 0 : ustr_hashICharsN(s, static_cast<int32_t>(uprv_strlen(s)));
}

U_CAPI int32_t U_EXPORT2
uhash_hashIStringView(ಸ್ಥಿರಾಂಕ UHashTok ಕೀಲಿ) {
    ಸ್ಥಿರಾಂಕ std::string_view* s = static_cast<std::string_view*>(ಕೀಲಿ.ಸೂಚಕ);
    ಹಿಂತಿರುಗಿ s == ಶೂನ್ಯಸೂಚಕ ? 0 : ustr_hashICharsN(s->ದತ್ತಾಂಶ(), static_cast<int32_t>(s->size()));
}

U_CAPI UBool U_EXPORT2
uhash_equals(ಸ್ಥಿರಾಂಕ UHashtable* hash1, ಸ್ಥಿರಾಂಕ UHashtable* hash2){
    int32_t count1, count2, pos, i;

    ಆದರೆ(hash1==hash2){
        ಹಿಂತಿರುಗಿ ನಿಜ;
    }

    /*
     * Make sure that we are comparing 2 valid hashes of the same type
     * with valid comparison functions.
     * Without valid comparison functions, a binary comparison
     * of the hash values will yield random results on machines
     * with 64-bit pointers and 32-bit integer hashes.
     * A valueComparator is normally optional.
     */
    ಆದರೆ (hash1==ಶೂನ್ಯಸೂಚಕ || hash2==ಶೂನ್ಯಸೂಚಕ ||
        hash1->ಕೀಲಿ_Comparator != hash2->ಕೀಲಿ_Comparator ||
        hash1->ಮೌಲ್ಯ_Comparator != hash2->ಮೌಲ್ಯ_Comparator ||
        hash1->ಮೌಲ್ಯ_Comparator == ಶೂನ್ಯಸೂಚಕ)
    {
        /*
        Normally we would return an error here about incompatible hash tables,
        but we return false instead.
        */
        ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
    }

    count1 = uhash_ಎಣಿಕೆ(hash1);
    count2 = uhash_ಎಣಿಕೆ(hash2);
    ಆದರೆ(count1!=count2){
        ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
    }

    pos=UHASH_ಮೊದಲ;
    ಗಾಗಿ(i=0; i<count1; i++){
        ಸ್ಥಿರಾಂಕ UHashElement* elem1 = uhash_ಮುಂದಿನ_Element(hash1, &pos);
        ಸ್ಥಿರಾಂಕ UHashTok key1 = elem1->ಕೀಲಿ;
        ಸ್ಥಿರಾಂಕ UHashTok val1 = elem1->value;
        /* here the keys are not compared, instead the key form hash1 is used to fetch
         * value from hash2. If the hashes are equal then then both hashes should
         * contain equal values for the same key!
         */
        ಸ್ಥಿರಾಂಕ UHashElement* elem2 = uhash_ಕಂಡುಹಿಡಿ_2(hash2, key1, hash2->ಕೀಲಿ_Hasher(key1));
        ಸ್ಥಿರಾಂಕ UHashTok val2 = elem2->value;
        ಆದರೆ(hash1->ಮೌಲ್ಯ_Comparator(val1, val2)==ಸುಳ್ಳು){
            ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
        }
    }
    ಹಿಂತಿರುಗಿ ನಿಜ;
}

/********************************************************************
 * PUBLIC Comparator Functions
 ********************************************************************/

U_CAPI UBool U_EXPORT2
uhash_compareUChars(ಸ್ಥಿರಾಂಕ UHashTok key1, ಸ್ಥಿರಾಂಕ UHashTok key2) {
    ಸ್ಥಿರಾಂಕ char16_t *p1 = (ಸ್ಥಿರಾಂಕ char16_t*) key1.ಸೂಚಕ;
    ಸ್ಥಿರಾಂಕ char16_t *p2 = (ಸ್ಥಿರಾಂಕ char16_t*) key2.ಸೂಚಕ;
    ಆದರೆ (p1 == p2) {
        ಹಿಂತಿರುಗಿ ನಿಜ;
    }
    ಆದರೆ (p1 == ಶೂನ್ಯಸೂಚಕ || p2 == ಶೂನ್ಯಸೂಚಕ) {
        ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
    }
    ಆಗ (*p1 != 0 && *p1 == *p2) {
        ++p1;
        ++p2;
    }
    ಹಿಂತಿರುಗಿ *p1 == *p2;
}

U_CAPI UBool U_EXPORT2
uhash_compareChars(ಸ್ಥಿರಾಂಕ UHashTok key1, ಸ್ಥಿರಾಂಕ UHashTok key2) {
    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *p1 = (ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ*) key1.ಸೂಚಕ;
    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *p2 = (ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ*) key2.ಸೂಚಕ;
    ಆದರೆ (p1 == p2) {
        ಹಿಂತಿರುಗಿ ನಿಜ;
    }
    ಆದರೆ (p1 == ಶೂನ್ಯಸೂಚಕ || p2 == ಶೂನ್ಯಸೂಚಕ) {
        ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
    }
    ಆಗ (*p1 != 0 && *p1 == *p2) {
        ++p1;
        ++p2;
    }
    ಹಿಂತಿರುಗಿ *p1 == *p2;
}

U_CAPI UBool U_EXPORT2
uhash_compareIChars(ಸ್ಥಿರಾಂಕ UHashTok key1, ಸ್ಥಿರಾಂಕ UHashTok key2) {
    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *p1 = (ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ*) key1.ಸೂಚಕ;
    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *p2 = (ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ*) key2.ಸೂಚಕ;
    ಆದರೆ (p1 == p2) {
        ಹಿಂತಿರುಗಿ ನಿಜ;
    }
    ಆದರೆ (p1 == ಶೂನ್ಯಸೂಚಕ || p2 == ಶೂನ್ಯಸೂಚಕ) {
        ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
    }
    ಆಗ (*p1 != 0 && uprv_tolower(*p1) == uprv_tolower(*p2)) {
        ++p1;
        ++p2;
    }
    ಹಿಂತಿರುಗಿ *p1 == *p2;
}

U_CAPI UBool U_EXPORT2
uhash_compareIStringView(ಸ್ಥಿರಾಂಕ UHashTok key1, ಸ್ಥಿರಾಂಕ UHashTok key2) {
    ಸ್ಥಿರಾಂಕ std::string_view* p1 = static_cast<std::string_view*>(key1.ಸೂಚಕ);
    ಸ್ಥಿರಾಂಕ std::string_view* p2 = static_cast<std::string_view*>(key2.ಸೂಚಕ);
    ಆದರೆ (p1 == p2) {
        ಹಿಂತಿರುಗಿ ನಿಜ;
    }
    ಆದರೆ (p1 == ಶೂನ್ಯಸೂಚಕ || p2 == ಶೂನ್ಯಸೂಚಕ) {
        ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
    }
    ಸ್ಥಿರಾಂಕ std::string_view& v1 = *p1;
    ಸ್ಥಿರಾಂಕ std::string_view& v2 = *p2;
    ಆದರೆ (v1.size() != v2.size()) {
        ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
    }
    ಗಾಗಿ (size_t i = 0; i < v1.size(); ++i) {
        ಆದರೆ (uprv_tolower(v1[i]) != uprv_tolower(v2[i])) {
            ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
        }
    }
    ಹಿಂತಿರುಗಿ ನಿಜ;
}

/********************************************************************
 * PUBLIC int32_t Support Functions
 ********************************************************************/

U_CAPI int32_t U_EXPORT2
uhash_hashLong(ಸ್ಥಿರಾಂಕ UHashTok ಕೀಲಿ) {
    ಹಿಂತಿರುಗಿ ಕೀಲಿ.integer;
}

U_CAPI UBool U_EXPORT2
uhash_compareLong(ಸ್ಥಿರಾಂಕ UHashTok key1, ಸ್ಥಿರಾಂಕ UHashTok key2) {
    ಹಿಂತಿರುಗಿ key1.integer == key2.integer;
}
