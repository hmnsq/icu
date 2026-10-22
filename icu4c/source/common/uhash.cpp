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
ಸ್ಥಿರ ಧ್ರುವ int32_t PRIMES[] = {
    7, 13, 31, 61, 127, 251, 509, 1021, 2039, 4093, 8191, 16381, 32749,
    65521, 131071, 262139, 524287, 1048573, 2097143, 4194301, 8388593,
    16777213, 33554393, 67108859, 134217689, 268435399, 536870909,
    1073741789, 2147483647 /*, 4294967291 */
};

#define PRIMES_LENGTH UPRV_LENGTHOF(PRIMES)
#define DEFAULT_ಅವಿಭಾಜ್ಯ_INDEX 4

/* These ratios are tuned to the PRIMES array such that a resize
 * places the table back into the zone of non-resizing.  That is,
 * after a call to _uhash_rehash(), a subsequent call to
 * _uhash_rehash() should do nothing (should not churn).  This is only
 * a potential problem with U_GROW_AND_SHRINK.
 */
ಸ್ಥಿರ ಧ್ರುವ ಭಿನ್ನ RESIZE_POLICY_RATIO_TABLE[6] = {
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
#define HASH_DELETED    ((int32_t) 0x80000000)
#define HASH_EMPTY      ((int32_t) HASH_DELETED + 1)

#define IS_EMPTY_OR_DELETED(x) ((x) < 0)

/* This macro expects a UHashTok.pointer as its keypointer and
   valuepointer parameters */
#define HASH_DELETE_ಕೀಲಿ_ಮೌಲ್ಯ(hash, keypointer, valuepointer) UPRV_BLOCK_MACRO_BEGIN { \
    ಯದಿ (hash->ಕೀಲಿ_Deleter != nullptr && keypointer != nullptr) { \
        (*hash->ಕೀಲಿ_Deleter)(keypointer); \
    } \
    ಯದಿ (hash->ಮೌಲ್ಯ_Deleter != nullptr && valuepointer != nullptr) { \
        (*hash->ಮೌಲ್ಯ_Deleter)(valuepointer); \
    } \
} UPRV_BLOCK_MACRO_END

/*
 * Constants for hinting whether a key or value is an integer
 * or a pointer.  If a hint bit is zero, then the associated
 * token is assumed to be an integer.
 */
#define HINT_BOTH_INTEGERS (0)
#define HINT_ಕೀಲಿ_POINTER   (1)
#define HINT_ಮೌಲ್ಯ_POINTER (2)
#define HINT_ALLOW_ZERO    (4)

/********************************************************************
 * PRIVATE Implementation
 ********************************************************************/

ಸ್ಥಿರ UHashTok
_uhash_setElement(UHashtable *hash, UHashElement* e,
                  int32_t hashcode,
                  UHashTok ಕೀಲಿ, UHashTok value, int8_t hint) {

    UHashTok old_ಮೌಲ್ಯ = e->value;
    ಯದಿ (hash->ಕೀಲಿ_Deleter != nullptr && e->ಕೀಲಿ.pointer != nullptr &&
        e->ಕೀಲಿ.pointer != ಕೀಲಿ.pointer) { /* Avoid double deletion */
        (*hash->ಕೀಲಿ_Deleter)(e->ಕೀಲಿ.pointer);
    }
    ಯದಿ (hash->ಮೌಲ್ಯ_Deleter != nullptr) {
        ಯದಿ (old_ಮೌಲ್ಯ.pointer != nullptr &&
            old_ಮೌಲ್ಯ.pointer != value.pointer) { /* Avoid double deletion */
            (*hash->ಮೌಲ್ಯ_Deleter)(old_ಮೌಲ್ಯ.pointer);
        }
        old_ಮೌಲ್ಯ.pointer = nullptr;
    }
    /* Compilers should copy the UHashTok union correctly, but even if
     * they do, memory heap tools (e.g. BoundsChecker) can get
     * confused when a pointer is cloaked in a union and then copied.
     * TO ALLEVIATE THIS, we use hints (based on what API the user is
     * calling) to copy pointers when we know the user thinks
     * something is a pointer. */
    ಯದಿ (hint & HINT_ಕೀಲಿ_POINTER) {
        e->ಕೀಲಿ.pointer = ಕೀಲಿ.pointer;
    } ಅನ್ಯಥಾ {
        e->ಕೀಲಿ = ಕೀಲಿ;
    }
    ಯದಿ (hint & HINT_ಮೌಲ್ಯ_POINTER) {
        e->value.pointer = value.pointer;
    } ಅನ್ಯಥಾ {
        e->value = value;
    }
    e->hashcode = hashcode;
    ಪ್ರತ್ಯಾಗಮ old_ಮೌಲ್ಯ;
}

/**
 * Assumes that the given element is not empty or deleted.
 */
ಸ್ಥಿರ UHashTok
_uhash_internalRemoveElement(UHashtable *hash, UHashElement* e) {
    UHashTok empty;
    U_ASSERT(!IS_EMPTY_OR_DELETED(e->hashcode));
    --hash->count;
    empty.pointer = nullptr; empty.integer = 0;
    ಪ್ರತ್ಯಾಗಮ _uhash_setElement(hash, e, HASH_DELETED, empty, empty, 0);
}

ಸ್ಥಿರ ಶೂನ್ಯ
_uhash_internalSetResizePolicy(UHashtable *hash, ಗಣನಾ UHashResizePolicy policy) {
    U_ASSERT(hash != nullptr);
    U_ASSERT(((int32_t)policy) >= 0);
    U_ASSERT(((int32_t)policy) < 3);
    hash->ಕೆಳಗಿನ_Water_Ratio  = RESIZE_POLICY_RATIO_TABLE[policy * 2];
    hash->ಮೇಲಿನ_Water_Ratio = RESIZE_POLICY_RATIO_TABLE[policy * 2 + 1];
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
                int32_t ಅವಿಭಾಜ್ಯ_Index,
                UErrorCode *status) {

    UHashElement *p, *limit;
    UHashTok emptytok;

    ಯದಿ (U_FAILURE(*status)) ಪ್ರತ್ಯಾಗಮ;

    U_ASSERT(ಅವಿಭಾಜ್ಯ_Index >= 0 && ಅವಿಭಾಜ್ಯ_Index < PRIMES_LENGTH);

    hash->ಅವಿಭಾಜ್ಯ_Index = static_cast<int8_t>(ಅವಿಭಾಜ್ಯ_Index);
    hash->length = PRIMES[ಅವಿಭಾಜ್ಯ_Index];

    p = hash->elements = static_cast<UHashElement*>(
        uprv_malloc(ಪರಿಮಾಣ(UHashElement) * hash->length));

    ಯದಿ (hash->elements == nullptr) {
        *status = U_MEMORY_ALLOCATION_ERROR;
        ಪ್ರತ್ಯಾಗಮ;
    }

    emptytok.pointer = nullptr; /* Only one of these two is needed */
    emptytok.integer = 0;    /* but we don't know which one. */

    limit = p + hash->length;
    ಯಾವತ್ (p < limit) {
        p->ಕೀಲಿ = emptytok;
        p->value = emptytok;
        p->hashcode = HASH_EMPTY;
        ++p;
    }

    hash->count = 0;
    hash->ಕೆಳಗಿನ_Water_Mark = static_cast<int32_t>(hash->length * hash->ಕೆಳಗಿನ_Water_Ratio);
    hash->ಮೇಲಿನ_Water_Mark = static_cast<int32_t>(hash->length * hash->ಮೇಲಿನ_Water_Ratio);
}

ಸ್ಥಿರ UHashtable*
_uhash_init(UHashtable *result,
              UHashFunction *ಕೀಲಿ_Hash,
              UKeyComparator *ಕೀಲಿ_Comp,
              UValueComparator *ಮೌಲ್ಯ_Comp,
              int32_t ಅವಿಭಾಜ್ಯ_Index,
              UErrorCode *status)
{
    ಯದಿ (U_FAILURE(*status)) ಪ್ರತ್ಯಾಗಮ nullptr;
    U_ASSERT(ಕೀಲಿ_Hash != nullptr);
    U_ASSERT(ಕೀಲಿ_Comp != nullptr);

    result->ಕೀಲಿ_Hasher       = ಕೀಲಿ_Hash;
    result->ಕೀಲಿ_Comparator   = ಕೀಲಿ_Comp;
    result->ಮೌಲ್ಯ_Comparator = ಮೌಲ್ಯ_Comp;
    result->ಕೀಲಿ_Deleter      = nullptr;
    result->ಮೌಲ್ಯ_Deleter    = nullptr;
    result->allocated       = ಅಸತ್ಯ;
    _uhash_internalSetResizePolicy(result, U_GROW);

    _uhash_allocate(result, ಅವಿಭಾಜ್ಯ_Index, status);

    ಯದಿ (U_FAILURE(*status)) {
        ಪ್ರತ್ಯಾಗಮ nullptr;
    }

    ಪ್ರತ್ಯಾಗಮ result;
}

ಸ್ಥಿರ UHashtable*
_uhash_create(UHashFunction *ಕೀಲಿ_Hash,
              UKeyComparator *ಕೀಲಿ_Comp,
              UValueComparator *ಮೌಲ್ಯ_Comp,
              int32_t ಅವಿಭಾಜ್ಯ_Index,
              UErrorCode *status) {
    UHashtable *result;

    ಯದಿ (U_FAILURE(*status)) ಪ್ರತ್ಯಾಗಮ nullptr;

    result = static_cast<UHashtable*>(uprv_malloc(ಪರಿಮಾಣ(UHashtable)));
    ಯದಿ (result == nullptr) {
        *status = U_MEMORY_ALLOCATION_ERROR;
        ಪ್ರತ್ಯಾಗಮ nullptr;
    }

    _uhash_init(result, ಕೀಲಿ_Hash, ಕೀಲಿ_Comp, ಮೌಲ್ಯ_Comp, ಅವಿಭಾಜ್ಯ_Index, status);
    result->allocated       = ಸತ್ಯ;

    ಯದಿ (U_FAILURE(*status)) {
        uprv_free(result);
        ಪ್ರತ್ಯಾಗಮ nullptr;
    }

    ಪ್ರತ್ಯಾಗಮ result;
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
_uhash_find(ಧ್ರುವ UHashtable *hash, UHashTok ಕೀಲಿ,
            int32_t hashcode) {

    int32_t firstDeleted = -1;  /* assume invalid index */
    int32_t theIndex, startIndex;
    int32_t jump = 0; /* lazy evaluate */
    int32_t tableHash;
    UHashElement *elements = hash->elements;

    hashcode &= 0x7FFFFFFF; /* must be positive */
    startIndex = theIndex = (hashcode ^ 0x4000000) % hash->length;

    ಕುರು {
        tableHash = elements[theIndex].hashcode;
        ಯದಿ (tableHash == hashcode) {          /* quick check */
            ಯದಿ ((*hash->ಕೀಲಿ_Comparator)(ಕೀಲಿ, elements[theIndex].ಕೀಲಿ)) {
                ಪ್ರತ್ಯಾಗಮ &(elements[theIndex]);
            }
        } ಅನ್ಯಥಾ ಯದಿ (!IS_EMPTY_OR_DELETED(tableHash)) {
            /* We have hit a slot which contains a key-value pair,
             * but for which the hash code does not match.  Keep
             * looking.
             */
        } ಅನ್ಯಥಾ ಯದಿ (tableHash == HASH_EMPTY) { /* empty, end o' the line */
            ಭಙ್ಗ;
        } ಅನ್ಯಥಾ ಯದಿ (firstDeleted < 0) { /* remember first deleted */
            firstDeleted = theIndex;
        }
        ಯದಿ (jump == 0) { /* lazy compute jump */
            /* The jump value must be relatively prime to the table
             * length.  As long as the length is prime, then any value
             * 1..length-1 will be relatively prime to it.
             */
            jump = (hashcode % (hash->length - 1)) + 1;
        }
        theIndex = (theIndex + jump) % hash->length;
    } ಯಾವತ್ (theIndex != startIndex);

    ಯದಿ (firstDeleted >= 0) {
        theIndex = firstDeleted; /* reset if had deleted slot */
    } ಅನ್ಯಥಾ ಯದಿ (tableHash != HASH_EMPTY) {
        /* We get to this point if the hashtable is full (no empty or
         * deleted slots), and we've failed to find a match.  THIS
         * WILL NEVER HAPPEN as long as uhash_put() makes sure that
         * count is always < length.
         */
        UPRV_UNREACHABLE_EXIT;
    }
    ಪ್ರತ್ಯಾಗಮ &(elements[theIndex]);
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
_uhash_rehash(UHashtable *hash, UErrorCode *status) {

    UHashElement *old = hash->elements;
    int32_t oldLength = hash->length;
    int32_t new_ಅವಿಭಾಜ್ಯ_Index = hash->ಅವಿಭಾಜ್ಯ_Index;
    int32_t i;

    ಯದಿ (hash->count > hash->ಮೇಲಿನ_Water_Mark) {
        ಯದಿ (++new_ಅವಿಭಾಜ್ಯ_Index >= PRIMES_LENGTH) {
            ಪ್ರತ್ಯಾಗಮ;
        }
    } ಅನ್ಯಥಾ ಯದಿ (hash->count < hash->ಕೆಳಗಿನ_Water_Mark) {
        ಯದಿ (--new_ಅವಿಭಾಜ್ಯ_Index < 0) {
            ಪ್ರತ್ಯಾಗಮ;
        }
    } ಅನ್ಯಥಾ {
        ಪ್ರತ್ಯಾಗಮ;
    }

    _uhash_allocate(hash, new_ಅವಿಭಾಜ್ಯ_Index, status);

    ಯದಿ (U_FAILURE(*status)) {
        hash->elements = old;
        hash->length = oldLength;
        ಪ್ರತ್ಯಾಗಮ;
    }

    ಪ್ರತಿ (i = oldLength - 1; i >= 0; --i) {
        ಯದಿ (!IS_EMPTY_OR_DELETED(old[i].hashcode)) {
            UHashElement *e = _uhash_find(hash, old[i].ಕೀಲಿ, old[i].hashcode);
            U_ASSERT(e != nullptr);
            U_ASSERT(e->hashcode == HASH_EMPTY);
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
    UHashTok result;
    UHashElement* e = _uhash_find(hash, ಕೀಲಿ, hash->ಕೀಲಿ_Hasher(ಕೀಲಿ));
    U_ASSERT(e != nullptr);
    result.pointer = nullptr;
    result.integer = 0;
    ಯದಿ (!IS_EMPTY_OR_DELETED(e->hashcode)) {
        result = _uhash_internalRemoveElement(hash, e);
        ಯದಿ (hash->count < hash->ಕೆಳಗಿನ_Water_Mark) {
            UErrorCode status = U_ZERO_ERROR;
            _uhash_rehash(hash, &status);
        }
    }
    ಪ್ರತ್ಯಾಗಮ result;
}

ಸ್ಥಿರ UHashTok
_uhash_put(UHashtable *hash,
           UHashTok ಕೀಲಿ,
           UHashTok value,
           int8_t hint,
           UErrorCode *status) {

    /* Put finds the position in the table for the new value.  If the
     * key is already in the table, it is deleted, if there is a
     * non-nullptr keyDeleter.  Then the key, the hash and the value are
     * all put at the position in their respective arrays.
     */
    int32_t hashcode;
    UHashElement* e;
    UHashTok emptytok;

    ಯದಿ (U_FAILURE(*status)) {
        ಗಚ್ಛ err;
    }
    U_ASSERT(hash != nullptr);
    ಯದಿ ((hint & HINT_ಮೌಲ್ಯ_POINTER) ?
            value.pointer == nullptr :
            value.integer == 0 && (hint & HINT_ALLOW_ZERO) == 0) {
        /* Disallow storage of nullptr values, since nullptr is returned by
         * get() to indicate an absent key.  Storing nullptr == removing.
         */
        ಪ್ರತ್ಯಾಗಮ _uhash_remove(hash, ಕೀಲಿ);
    }
    ಯದಿ (hash->count > hash->ಮೇಲಿನ_Water_Mark) {
        _uhash_rehash(hash, status);
        ಯದಿ (U_FAILURE(*status)) {
            ಗಚ್ಛ err;
        }
    }

    hashcode = (*hash->ಕೀಲಿ_Hasher)(ಕೀಲಿ);
    e = _uhash_find(hash, ಕೀಲಿ, hashcode);
    U_ASSERT(e != nullptr);

    ಯದಿ (IS_EMPTY_OR_DELETED(e->hashcode)) {
        /* Important: We must never actually fill the table up.  If we
         * do so, then _uhash_find() will return nullptr, and we'll have
         * to check for nullptr after every call to _uhash_find().  To
         * avoid this we make sure there is always at least one empty
         * or deleted slot in the table.  This only is a problem if we
         * are out of memory and rehash isn't working.
         */
        ++hash->count;
        ಯದಿ (hash->count == hash->length) {
            /* Don't allow count to reach length */
            --hash->count;
            *status = U_MEMORY_ALLOCATION_ERROR;
            ಗಚ್ಛ err;
        }
    }

    /* We must in all cases handle storage properly.  If there was an
     * old key, then it must be deleted (if the deleter != nullptr).
     * Make hashcodes stored in table positive.
     */
    ಪ್ರತ್ಯಾಗಮ _uhash_setElement(hash, e, hashcode & 0x7FFFFFFF, ಕೀಲಿ, value, hint);

 err:
    /* If the deleters are non-nullptr, this method adopts its key and/or
     * value arguments, and we must be sure to delete the key and/or
     * value in all cases, even upon failure.
     */
    HASH_DELETE_ಕೀಲಿ_ಮೌಲ್ಯ(hash, ಕೀಲಿ.pointer, value.pointer);
    emptytok.pointer = nullptr; emptytok.integer = 0;
    ಪ್ರತ್ಯಾಗಮ emptytok;
}


/********************************************************************
 * PUBLIC API
 ********************************************************************/

U_CAPI UHashtable* U_EXPORT2
uhash_open(UHashFunction *ಕೀಲಿ_Hash,
           UKeyComparator *ಕೀಲಿ_Comp,
           UValueComparator *ಮೌಲ್ಯ_Comp,
           UErrorCode *status) {

    ಪ್ರತ್ಯಾಗಮ _uhash_create(ಕೀಲಿ_Hash, ಕೀಲಿ_Comp, ಮೌಲ್ಯ_Comp, DEFAULT_ಅವಿಭಾಜ್ಯ_INDEX, status);
}

U_CAPI UHashtable* U_EXPORT2
uhash_openSize(UHashFunction *ಕೀಲಿ_Hash,
               UKeyComparator *ಕೀಲಿ_Comp,
               UValueComparator *ಮೌಲ್ಯ_Comp,
               int32_t size,
               UErrorCode *status) {

    /* Find the smallest index i for which PRIMES[i] >= size. */
    int32_t i = 0;
    ಯಾವತ್ (i<(PRIMES_LENGTH-1) && PRIMES[i]<size) {
        ++i;
    }

    ಪ್ರತ್ಯಾಗಮ _uhash_create(ಕೀಲಿ_Hash, ಕೀಲಿ_Comp, ಮೌಲ್ಯ_Comp, i, status);
}

U_CAPI UHashtable* U_EXPORT2
uhash_init(UHashtable *fillinResult,
           UHashFunction *ಕೀಲಿ_Hash,
           UKeyComparator *ಕೀಲಿ_Comp,
           UValueComparator *ಮೌಲ್ಯ_Comp,
           UErrorCode *status) {

    ಪ್ರತ್ಯಾಗಮ _uhash_init(fillinResult, ಕೀಲಿ_Hash, ಕೀಲಿ_Comp, ಮೌಲ್ಯ_Comp, DEFAULT_ಅವಿಭಾಜ್ಯ_INDEX, status);
}

U_CAPI UHashtable* U_EXPORT2
uhash_initSize(UHashtable *fillinResult,
               UHashFunction *ಕೀಲಿ_Hash,
               UKeyComparator *ಕೀಲಿ_Comp,
               UValueComparator *ಮೌಲ್ಯ_Comp,
               int32_t size,
               UErrorCode *status) {

    // Find the smallest index i for which PRIMES[i] >= size.
    int32_t i = 0;
    ಯಾವತ್ (i<(PRIMES_LENGTH-1) && PRIMES[i]<size) {
        ++i;
    }
    ಪ್ರತ್ಯಾಗಮ _uhash_init(fillinResult, ಕೀಲಿ_Hash, ಕೀಲಿ_Comp, ಮೌಲ್ಯ_Comp, i, status);
}

U_CAPI ಶೂನ್ಯ U_EXPORT2
uhash_close(UHashtable *hash) {
    ಯದಿ (hash == nullptr) {
        ಪ್ರತ್ಯಾಗಮ;
    }
    ಯದಿ (hash->elements != nullptr) {
        ಯದಿ (hash->ಕೀಲಿ_Deleter != nullptr || hash->ಮೌಲ್ಯ_Deleter != nullptr) {
            int32_t pos=UHASH_FIRST;
            UHashElement *e;
            ಯಾವತ್ ((e = (UHashElement*) uhash_nextElement(hash, &pos)) != nullptr) {
                HASH_DELETE_ಕೀಲಿ_ಮೌಲ್ಯ(hash, e->ಕೀಲಿ.pointer, e->value.pointer);
            }
        }
        uprv_free(hash->elements);
        hash->elements = nullptr;
    }
    ಯದಿ (hash->allocated) {
        uprv_free(hash);
    }
}

U_CAPI UHashFunction *U_EXPORT2
uhash_set_ಕೀಲಿ_Hasher(UHashtable *hash, UHashFunction *fn) {
    UHashFunction *result = hash->ಕೀಲಿ_Hasher;
    hash->ಕೀಲಿ_Hasher = fn;
    ಪ್ರತ್ಯಾಗಮ result;
}

U_CAPI UKeyComparator *U_EXPORT2
uhash_set_ಕೀಲಿ_Comparator(UHashtable *hash, UKeyComparator *fn) {
    UKeyComparator *result = hash->ಕೀಲಿ_Comparator;
    hash->ಕೀಲಿ_Comparator = fn;
    ಪ್ರತ್ಯಾಗಮ result;
}
U_CAPI UValueComparator *U_EXPORT2
uhash_set_ಮೌಲ್ಯ_Comparator(UHashtable *hash, UValueComparator *fn){
    UValueComparator *result = hash->ಮೌಲ್ಯ_Comparator;
    hash->ಮೌಲ್ಯ_Comparator = fn;
    ಪ್ರತ್ಯಾಗಮ result;
}

U_CAPI UObjectDeleter *U_EXPORT2
uhash_set_ಕೀಲಿ_Deleter(UHashtable *hash, UObjectDeleter *fn) {
    UObjectDeleter *result = hash->ಕೀಲಿ_Deleter;
    hash->ಕೀಲಿ_Deleter = fn;
    ಪ್ರತ್ಯಾಗಮ result;
}

U_CAPI UObjectDeleter *U_EXPORT2
uhash_set_ಮೌಲ್ಯ_Deleter(UHashtable *hash, UObjectDeleter *fn) {
    UObjectDeleter *result = hash->ಮೌಲ್ಯ_Deleter;
    hash->ಮೌಲ್ಯ_Deleter = fn;
    ಪ್ರತ್ಯಾಗಮ result;
}

U_CAPI ಶೂನ್ಯ U_EXPORT2
uhash_setResizePolicy(UHashtable *hash, ಗಣನಾ UHashResizePolicy policy) {
    UErrorCode status = U_ZERO_ERROR;
    _uhash_internalSetResizePolicy(hash, policy);
    hash->ಕೆಳಗಿನ_Water_Mark  = (int32_t)(hash->length * hash->ಕೆಳಗಿನ_Water_Ratio);
    hash->ಮೇಲಿನ_Water_Mark = (int32_t)(hash->length * hash->ಮೇಲಿನ_Water_Ratio);
    _uhash_rehash(hash, &status);
}

U_CAPI int32_t U_EXPORT2
uhash_ಎಣಿಕೆ(ಧ್ರುವ UHashtable *hash) {
    ಪ್ರತ್ಯಾಗಮ hash->count;
}

U_CAPI ಶೂನ್ಯ* U_EXPORT2
uhash_get(ಧ್ರುವ UHashtable *hash,
          ಧ್ರುವ ಶೂನ್ಯ* ಕೀಲಿ) {
    UHashTok keyholder;
    keyholder.pointer = (ಶೂನ್ಯ*) ಕೀಲಿ;
    ಪ್ರತ್ಯಾಗಮ _uhash_find(hash, keyholder, hash->ಕೀಲಿ_Hasher(keyholder))->value.pointer;
}

U_CAPI ಶೂನ್ಯ* U_EXPORT2
uhash_iget(ಧ್ರುವ UHashtable *hash,
           int32_t ಕೀಲಿ) {
    UHashTok keyholder;
    keyholder.integer = ಕೀಲಿ;
    ಪ್ರತ್ಯಾಗಮ _uhash_find(hash, keyholder, hash->ಕೀಲಿ_Hasher(keyholder))->value.pointer;
}

U_CAPI int32_t U_EXPORT2
uhash_geti(ಧ್ರುವ UHashtable *hash,
           ಧ್ರುವ ಶೂನ್ಯ* ಕೀಲಿ) {
    UHashTok keyholder;
    keyholder.pointer = (ಶೂನ್ಯ*) ಕೀಲಿ;
    ಪ್ರತ್ಯಾಗಮ _uhash_find(hash, keyholder, hash->ಕೀಲಿ_Hasher(keyholder))->value.integer;
}

U_CAPI int32_t U_EXPORT2
uhash_igeti(ಧ್ರುವ UHashtable *hash,
           int32_t ಕೀಲಿ) {
    UHashTok keyholder;
    keyholder.integer = ಕೀಲಿ;
    ಪ್ರತ್ಯಾಗಮ _uhash_find(hash, keyholder, hash->ಕೀಲಿ_Hasher(keyholder))->value.integer;
}

U_CAPI int32_t U_EXPORT2
uhash_getiAndFound(ಧ್ರುವ UHashtable *hash,
                   ಧ್ರುವ ಶೂನ್ಯ *ಕೀಲಿ,
                   UBool *found) {
    UHashTok keyholder;
    keyholder.pointer = (ಶೂನ್ಯ *)ಕೀಲಿ;
    ಧ್ರುವ UHashElement *e = _uhash_find(hash, keyholder, hash->ಕೀಲಿ_Hasher(keyholder));
    *found = !IS_EMPTY_OR_DELETED(e->hashcode);
    ಪ್ರತ್ಯಾಗಮ e->value.integer;
}

U_CAPI int32_t U_EXPORT2
uhash_igetiAndFound(ಧ್ರುವ UHashtable *hash,
                    int32_t ಕೀಲಿ,
                    UBool *found) {
    UHashTok keyholder;
    keyholder.integer = ಕೀಲಿ;
    ಧ್ರುವ UHashElement *e = _uhash_find(hash, keyholder, hash->ಕೀಲಿ_Hasher(keyholder));
    *found = !IS_EMPTY_OR_DELETED(e->hashcode);
    ಪ್ರತ್ಯಾಗಮ e->value.integer;
}

U_CAPI ಶೂನ್ಯ* U_EXPORT2
uhash_put(UHashtable *hash,
          ಶೂನ್ಯ* ಕೀಲಿ,
          ಶೂನ್ಯ* value,
          UErrorCode *status) {
    UHashTok keyholder, valueholder;
    keyholder.pointer = ಕೀಲಿ;
    valueholder.pointer = value;
    ಪ್ರತ್ಯಾಗಮ _uhash_put(hash, keyholder, valueholder,
                      HINT_ಕೀಲಿ_POINTER | HINT_ಮೌಲ್ಯ_POINTER,
                      status).pointer;
}

U_CAPI ಶೂನ್ಯ* U_EXPORT2
uhash_iput(UHashtable *hash,
           int32_t ಕೀಲಿ,
           ಶೂನ್ಯ* value,
           UErrorCode *status) {
    UHashTok keyholder, valueholder;
    keyholder.integer = ಕೀಲಿ;
    valueholder.pointer = value;
    ಪ್ರತ್ಯಾಗಮ _uhash_put(hash, keyholder, valueholder,
                      HINT_ಮೌಲ್ಯ_POINTER,
                      status).pointer;
}

U_CAPI int32_t U_EXPORT2
uhash_puti(UHashtable *hash,
           ಶೂನ್ಯ* ಕೀಲಿ,
           int32_t value,
           UErrorCode *status) {
    UHashTok keyholder, valueholder;
    keyholder.pointer = ಕೀಲಿ;
    valueholder.integer = value;
    ಪ್ರತ್ಯಾಗಮ _uhash_put(hash, keyholder, valueholder,
                      HINT_ಕೀಲಿ_POINTER,
                      status).integer;
}


U_CAPI int32_t U_EXPORT2
uhash_iputi(UHashtable *hash,
           int32_t ಕೀಲಿ,
           int32_t value,
           UErrorCode *status) {
    UHashTok keyholder, valueholder;
    keyholder.integer = ಕೀಲಿ;
    valueholder.integer = value;
    ಪ್ರತ್ಯಾಗಮ _uhash_put(hash, keyholder, valueholder,
                      HINT_BOTH_INTEGERS,
                      status).integer;
}

U_CAPI int32_t U_EXPORT2
uhash_putiAllowZero(UHashtable *hash,
                    ಶೂನ್ಯ *ಕೀಲಿ,
                    int32_t value,
                    UErrorCode *status) {
    UHashTok keyholder, valueholder;
    keyholder.pointer = ಕೀಲಿ;
    valueholder.integer = value;
    ಪ್ರತ್ಯಾಗಮ _uhash_put(hash, keyholder, valueholder,
                      HINT_ಕೀಲಿ_POINTER | HINT_ALLOW_ZERO,
                      status).integer;
}


U_CAPI int32_t U_EXPORT2
uhash_iputiAllowZero(UHashtable *hash,
                     int32_t ಕೀಲಿ,
                     int32_t value,
                     UErrorCode *status) {
    UHashTok keyholder, valueholder;
    keyholder.integer = ಕೀಲಿ;
    valueholder.integer = value;
    ಪ್ರತ್ಯಾಗಮ _uhash_put(hash, keyholder, valueholder,
                      HINT_BOTH_INTEGERS | HINT_ALLOW_ZERO,
                      status).integer;
}

U_CAPI ಶೂನ್ಯ* U_EXPORT2
uhash_remove(UHashtable *hash,
             ಧ್ರುವ ಶೂನ್ಯ* ಕೀಲಿ) {
    UHashTok keyholder;
    keyholder.pointer = (ಶೂನ್ಯ*) ಕೀಲಿ;
    ಪ್ರತ್ಯಾಗಮ _uhash_remove(hash, keyholder).pointer;
}

U_CAPI ಶೂನ್ಯ* U_EXPORT2
uhash_iremove(UHashtable *hash,
              int32_t ಕೀಲಿ) {
    UHashTok keyholder;
    keyholder.integer = ಕೀಲಿ;
    ಪ್ರತ್ಯಾಗಮ _uhash_remove(hash, keyholder).pointer;
}

U_CAPI int32_t U_EXPORT2
uhash_removei(UHashtable *hash,
              ಧ್ರುವ ಶೂನ್ಯ* ಕೀಲಿ) {
    UHashTok keyholder;
    keyholder.pointer = (ಶೂನ್ಯ*) ಕೀಲಿ;
    ಪ್ರತ್ಯಾಗಮ _uhash_remove(hash, keyholder).integer;
}

U_CAPI int32_t U_EXPORT2
uhash_iremovei(UHashtable *hash,
               int32_t ಕೀಲಿ) {
    UHashTok keyholder;
    keyholder.integer = ಕೀಲಿ;
    ಪ್ರತ್ಯಾಗಮ _uhash_remove(hash, keyholder).integer;
}

U_CAPI ಶೂನ್ಯ U_EXPORT2
uhash_removeAll(UHashtable *hash) {
    int32_t pos = UHASH_FIRST;
    ಧ್ರುವ UHashElement *e;
    U_ASSERT(hash != nullptr);
    ಯದಿ (hash->count != 0) {
        ಯಾವತ್ ((e = uhash_nextElement(hash, &pos)) != nullptr) {
            uhash_removeElement(hash, e);
        }
    }
    U_ASSERT(hash->count == 0);
}

U_CAPI UBool U_EXPORT2
uhash_contains_ಕೀಲಿ(ಧ್ರುವ UHashtable *hash, ಧ್ರುವ ಶೂನ್ಯ *ಕೀಲಿ) {
    UHashTok keyholder;
    keyholder.pointer = (ಶೂನ್ಯ *)ಕೀಲಿ;
    ಧ್ರುವ UHashElement *e = _uhash_find(hash, keyholder, hash->ಕೀಲಿ_Hasher(keyholder));
    ಪ್ರತ್ಯಾಗಮ !IS_EMPTY_OR_DELETED(e->hashcode);
}

/**
 * Returns true if the UHashtable contains an item with this integer key.
 *
 * @param hash The target UHashtable.
 * @param key An integer key stored in a hashtable
 * @return true if the key is found.
 */
U_CAPI UBool U_EXPORT2
uhash_icontains_ಕೀಲಿ(ಧ್ರುವ UHashtable *hash, int32_t ಕೀಲಿ) {
    UHashTok keyholder;
    keyholder.integer = ಕೀಲಿ;
    ಧ್ರುವ UHashElement *e = _uhash_find(hash, keyholder, hash->ಕೀಲಿ_Hasher(keyholder));
    ಪ್ರತ್ಯಾಗಮ !IS_EMPTY_OR_DELETED(e->hashcode);
}

U_CAPI ಧ್ರುವ UHashElement* U_EXPORT2
uhash_find(ಧ್ರುವ UHashtable *hash, ಧ್ರುವ ಶೂನ್ಯ* ಕೀಲಿ) {
    UHashTok keyholder;
    ಧ್ರುವ UHashElement *e;
    keyholder.pointer = (ಶೂನ್ಯ*) ಕೀಲಿ;
    e = _uhash_find(hash, keyholder, hash->ಕೀಲಿ_Hasher(keyholder));
    ಪ್ರತ್ಯಾಗಮ IS_EMPTY_OR_DELETED(e->hashcode) ? nullptr : e;
}

U_CAPI ಧ್ರುವ UHashElement* U_EXPORT2
uhash_nextElement(ಧ್ರುವ UHashtable *hash, int32_t *pos) {
    /* Walk through the array until we find an element that is not
     * EMPTY and not DELETED.
     */
    int32_t i;
    U_ASSERT(hash != nullptr);
    ಪ್ರತಿ (i = *pos + 1; i < hash->length; ++i) {
        ಯದಿ (!IS_EMPTY_OR_DELETED(hash->elements[i].hashcode)) {
            *pos = i;
            ಪ್ರತ್ಯಾಗಮ &(hash->elements[i]);
        }
    }

    /* No more elements */
    ಪ್ರತ್ಯಾಗಮ nullptr;
}

U_CAPI ಶೂನ್ಯ* U_EXPORT2
uhash_removeElement(UHashtable *hash, ಧ್ರುವ UHashElement* e) {
    U_ASSERT(hash != nullptr);
    U_ASSERT(e != nullptr);
    ಯದಿ (!IS_EMPTY_OR_DELETED(e->hashcode)) {
        UHashElement *nce = (UHashElement *)e;
        ಪ್ರತ್ಯಾಗಮ _uhash_internalRemoveElement(hash, nce).pointer;
    }
    ಪ್ರತ್ಯಾಗಮ nullptr;
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
uhash_hashUChars(ಧ್ರುವ UHashTok ಕೀಲಿ) {
    ಧ್ರುವ char16_t *s = (ಧ್ರುವ char16_t *)ಕೀಲಿ.pointer;
    ಪ್ರತ್ಯಾಗಮ s == nullptr ? 0 : ustr_hashUCharsN(s, u_strlen(s));
}

U_CAPI int32_t U_EXPORT2
uhash_hashChars(ಧ್ರುವ UHashTok ಕೀಲಿ) {
    ಧ್ರುವ ವರ್ಣ *s = (ಧ್ರುವ ವರ್ಣ *)ಕೀಲಿ.pointer;
    ಪ್ರತ್ಯಾಗಮ s == nullptr ? 0 : static_cast<int32_t>(ustr_hashCharsN(s, static_cast<int32_t>(uprv_strlen(s))));
}

U_CAPI int32_t U_EXPORT2
uhash_hashIChars(ಧ್ರುವ UHashTok ಕೀಲಿ) {
    ಧ್ರುವ ವರ್ಣ *s = (ಧ್ರುವ ವರ್ಣ *)ಕೀಲಿ.pointer;
    ಪ್ರತ್ಯಾಗಮ s == nullptr ? 0 : ustr_hashICharsN(s, static_cast<int32_t>(uprv_strlen(s)));
}

U_CAPI int32_t U_EXPORT2
uhash_hashIStringView(ಧ್ರುವ UHashTok ಕೀಲಿ) {
    ಧ್ರುವ std::string_view* s = static_cast<std::string_view*>(ಕೀಲಿ.pointer);
    ಪ್ರತ್ಯಾಗಮ s == nullptr ? 0 : ustr_hashICharsN(s->data(), static_cast<int32_t>(s->size()));
}

U_CAPI UBool U_EXPORT2
uhash_equals(ಧ್ರುವ UHashtable* hash1, ಧ್ರುವ UHashtable* hash2){
    int32_t count1, count2, pos, i;

    ಯದಿ(hash1==hash2){
        ಪ್ರತ್ಯಾಗಮ ಸತ್ಯ;
    }

    /*
     * Make sure that we are comparing 2 valid hashes of the same type
     * with valid comparison functions.
     * Without valid comparison functions, a binary comparison
     * of the hash values will yield random results on machines
     * with 64-bit pointers and 32-bit integer hashes.
     * A valueComparator is normally optional.
     */
    ಯದಿ (hash1==nullptr || hash2==nullptr ||
        hash1->ಕೀಲಿ_Comparator != hash2->ಕೀಲಿ_Comparator ||
        hash1->ಮೌಲ್ಯ_Comparator != hash2->ಮೌಲ್ಯ_Comparator ||
        hash1->ಮೌಲ್ಯ_Comparator == nullptr)
    {
        /*
        Normally we would return an error here about incompatible hash tables,
        but we return false instead.
        */
        ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;
    }

    count1 = uhash_ಎಣಿಕೆ(hash1);
    count2 = uhash_ಎಣಿಕೆ(hash2);
    ಯದಿ(count1!=count2){
        ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;
    }

    pos=UHASH_FIRST;
    ಪ್ರತಿ(i=0; i<count1; i++){
        ಧ್ರುವ UHashElement* elem1 = uhash_nextElement(hash1, &pos);
        ಧ್ರುವ UHashTok key1 = elem1->ಕೀಲಿ;
        ಧ್ರುವ UHashTok val1 = elem1->value;
        /* here the keys are not compared, instead the key form hash1 is used to fetch
         * value from hash2. If the hashes are equal then then both hashes should
         * contain equal values for the same key!
         */
        ಧ್ರುವ UHashElement* elem2 = _uhash_find(hash2, key1, hash2->ಕೀಲಿ_Hasher(key1));
        ಧ್ರುವ UHashTok val2 = elem2->value;
        ಯದಿ(hash1->ಮೌಲ್ಯ_Comparator(val1, val2)==ಅಸತ್ಯ){
            ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;
        }
    }
    ಪ್ರತ್ಯಾಗಮ ಸತ್ಯ;
}

/********************************************************************
 * PUBLIC Comparator Functions
 ********************************************************************/

U_CAPI UBool U_EXPORT2
uhash_compareUChars(ಧ್ರುವ UHashTok key1, ಧ್ರುವ UHashTok key2) {
    ಧ್ರುವ char16_t *p1 = (ಧ್ರುವ char16_t*) key1.pointer;
    ಧ್ರುವ char16_t *p2 = (ಧ್ರುವ char16_t*) key2.pointer;
    ಯದಿ (p1 == p2) {
        ಪ್ರತ್ಯಾಗಮ ಸತ್ಯ;
    }
    ಯದಿ (p1 == nullptr || p2 == nullptr) {
        ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;
    }
    ಯಾವತ್ (*p1 != 0 && *p1 == *p2) {
        ++p1;
        ++p2;
    }
    ಪ್ರತ್ಯಾಗಮ *p1 == *p2;
}

U_CAPI UBool U_EXPORT2
uhash_compareChars(ಧ್ರುವ UHashTok key1, ಧ್ರುವ UHashTok key2) {
    ಧ್ರುವ ವರ್ಣ *p1 = (ಧ್ರುವ ವರ್ಣ*) key1.pointer;
    ಧ್ರುವ ವರ್ಣ *p2 = (ಧ್ರುವ ವರ್ಣ*) key2.pointer;
    ಯದಿ (p1 == p2) {
        ಪ್ರತ್ಯಾಗಮ ಸತ್ಯ;
    }
    ಯದಿ (p1 == nullptr || p2 == nullptr) {
        ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;
    }
    ಯಾವತ್ (*p1 != 0 && *p1 == *p2) {
        ++p1;
        ++p2;
    }
    ಪ್ರತ್ಯಾಗಮ *p1 == *p2;
}

U_CAPI UBool U_EXPORT2
uhash_compareIChars(ಧ್ರುವ UHashTok key1, ಧ್ರುವ UHashTok key2) {
    ಧ್ರುವ ವರ್ಣ *p1 = (ಧ್ರುವ ವರ್ಣ*) key1.pointer;
    ಧ್ರುವ ವರ್ಣ *p2 = (ಧ್ರುವ ವರ್ಣ*) key2.pointer;
    ಯದಿ (p1 == p2) {
        ಪ್ರತ್ಯಾಗಮ ಸತ್ಯ;
    }
    ಯದಿ (p1 == nullptr || p2 == nullptr) {
        ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;
    }
    ಯಾವತ್ (*p1 != 0 && uprv_tolower(*p1) == uprv_tolower(*p2)) {
        ++p1;
        ++p2;
    }
    ಪ್ರತ್ಯಾಗಮ *p1 == *p2;
}

U_CAPI UBool U_EXPORT2
uhash_compareIStringView(ಧ್ರುವ UHashTok key1, ಧ್ರುವ UHashTok key2) {
    ಧ್ರುವ std::string_view* p1 = static_cast<std::string_view*>(key1.pointer);
    ಧ್ರುವ std::string_view* p2 = static_cast<std::string_view*>(key2.pointer);
    ಯದಿ (p1 == p2) {
        ಪ್ರತ್ಯಾಗಮ ಸತ್ಯ;
    }
    ಯದಿ (p1 == nullptr || p2 == nullptr) {
        ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;
    }
    ಧ್ರುವ std::string_view& v1 = *p1;
    ಧ್ರುವ std::string_view& v2 = *p2;
    ಯದಿ (v1.size() != v2.size()) {
        ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;
    }
    ಪ್ರತಿ (size_t i = 0; i < v1.size(); ++i) {
        ಯದಿ (uprv_tolower(v1[i]) != uprv_tolower(v2[i])) {
            ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;
        }
    }
    ಪ್ರತ್ಯಾಗಮ ಸತ್ಯ;
}

/********************************************************************
 * PUBLIC int32_t Support Functions
 ********************************************************************/

U_CAPI int32_t U_EXPORT2
uhash_hashLong(ಧ್ರುವ UHashTok ಕೀಲಿ) {
    ಪ್ರತ್ಯಾಗಮ ಕೀಲಿ.integer;
}

U_CAPI UBool U_EXPORT2
uhash_compareLong(ಧ್ರುವ UHashTok key1, ಧ್ರುವ UHashTok key2) {
    ಪ್ರತ್ಯಾಗಮ key1.integer == key2.integer;
}
