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

#ଅନ୍ତର୍ଭାଵ <string_view>

#ଅନ୍ତର୍ଭାଵ "uhash.h"
#ଅନ୍ତର୍ଭାଵ "unicode/ustring.h"
#ଅନ୍ତର୍ଭାଵ "cstring.h"
#ଅନ୍ତର୍ଭାଵ "cmemory.h"
#ଅନ୍ତର୍ଭାଵ "uassert.h"
#ଅନ୍ତର୍ଭାଵ "ustr_imp.h"

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
ସ୍ଥିର ଅଚଳ int32_t PRIMES[] = {
    7, 13, 31, 61, 127, 251, 509, 1021, 2039, 4093, 8191, 16381, 32749,
    65521, 131071, 262139, 524287, 1048573, 2097143, 4194301, 8388593,
    16777213, 33554393, 67108859, 134217689, 268435399, 536870909,
    1073741789, 2147483647 /*, 4294967291 */
};

#define PRIMES_LENGTH UPRV_LENGTHOF(PRIMES)
#define DEFAULT_ମୌଳିକ_INDEX 4

/* These ratios are tuned to the PRIMES array such that a resize
 * places the table back into the zone of non-resizing.  That is,
 * after a call to _uhash_rehash(), a subsequent call to
 * _uhash_rehash() should do nothing (should not churn).  This is only
 * a potential problem with U_GROW_AND_SHRINK.
 */
ସ୍ଥିର ଅଚଳ ଦଶମିକ RESIZE_POLICY_RATIO_TABLE[6] = {
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
#define HASH_DELETE_ଚାବି_ମୂଲ୍ୟ(hash, keypointer, valuepointer) UPRV_BLOCK_MACRO_BEGIN { \
    ଯଦି (hash->ଚାବି_Deleter != nullptr && keypointer != nullptr) { \
        (*hash->ଚାବି_Deleter)(keypointer); \
    } \
    ଯଦି (hash->ମୂଲ୍ୟ_Deleter != nullptr && valuepointer != nullptr) { \
        (*hash->ମୂଲ୍ୟ_Deleter)(valuepointer); \
    } \
} UPRV_BLOCK_MACRO_END

/*
 * Constants for hinting whether a key or value is an integer
 * or a pointer.  If a hint bit is zero, then the associated
 * token is assumed to be an integer.
 */
#define HINT_BOTH_INTEGERS (0)
#define HINT_ଚାବି_POINTER   (1)
#define HINT_ମୂଲ୍ୟ_POINTER (2)
#define HINT_ALLOW_ZERO    (4)

/********************************************************************
 * PRIVATE Implementation
 ********************************************************************/

ସ୍ଥିର UHashTok
_uhash_setElement(UHashtable *hash, UHashElement* e,
                  int32_t hashcode,
                  UHashTok ଚାବି, UHashTok value, int8_t hint) {

    UHashTok old_ମୂଲ୍ୟ = e->value;
    ଯଦି (hash->ଚାବି_Deleter != nullptr && e->ଚାବି.pointer != nullptr &&
        e->ଚାବି.pointer != ଚାବି.pointer) { /* Avoid double deletion */
        (*hash->ଚାବି_Deleter)(e->ଚାବି.pointer);
    }
    ଯଦି (hash->ମୂଲ୍ୟ_Deleter != nullptr) {
        ଯଦି (old_ମୂଲ୍ୟ.pointer != nullptr &&
            old_ମୂଲ୍ୟ.pointer != value.pointer) { /* Avoid double deletion */
            (*hash->ମୂଲ୍ୟ_Deleter)(old_ମୂଲ୍ୟ.pointer);
        }
        old_ମୂଲ୍ୟ.pointer = nullptr;
    }
    /* Compilers should copy the UHashTok union correctly, but even if
     * they do, memory heap tools (e.g. BoundsChecker) can get
     * confused when a pointer is cloaked in a union and then copied.
     * TO ALLEVIATE THIS, we use hints (based on what API the user is
     * calling) to copy pointers when we know the user thinks
     * something is a pointer. */
    ଯଦି (hint & HINT_ଚାବି_POINTER) {
        e->ଚାବି.pointer = ଚାବି.pointer;
    } ଅନ୍ୟଥା {
        e->ଚାବି = ଚାବି;
    }
    ଯଦି (hint & HINT_ମୂଲ୍ୟ_POINTER) {
        e->value.pointer = value.pointer;
    } ଅନ୍ୟଥା {
        e->value = value;
    }
    e->hashcode = hashcode;
    ଫେରନ୍ତୁ old_ମୂଲ୍ୟ;
}

/**
 * Assumes that the given element is not empty or deleted.
 */
ସ୍ଥିର UHashTok
_uhash_internalRemoveElement(UHashtable *hash, UHashElement* e) {
    UHashTok empty;
    U_ASSERT(!IS_EMPTY_OR_DELETED(e->hashcode));
    --hash->count;
    empty.pointer = nullptr; empty.integer = 0;
    ଫେରନ୍ତୁ _uhash_setElement(hash, e, HASH_DELETED, empty, empty, 0);
}

ସ୍ଥିର ଶୂନ୍ୟ
_uhash_internalSetResizePolicy(UHashtable *hash, ଗଣନା UHashResizePolicy policy) {
    U_ASSERT(hash != nullptr);
    U_ASSERT(((int32_t)policy) >= 0);
    U_ASSERT(((int32_t)policy) < 3);
    hash->ତଳ_Water_Ratio  = RESIZE_POLICY_RATIO_TABLE[policy * 2];
    hash->ଉପର_Water_Ratio = RESIZE_POLICY_RATIO_TABLE[policy * 2 + 1];
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
ସ୍ଥିର ଶୂନ୍ୟ
_uhash_allocate(UHashtable *hash,
                int32_t ମୌଳିକ_Index,
                UErrorCode *status) {

    UHashElement *p, *limit;
    UHashTok emptytok;

    ଯଦି (U_FAILURE(*status)) ଫେରନ୍ତୁ;

    U_ASSERT(ମୌଳିକ_Index >= 0 && ମୌଳିକ_Index < PRIMES_LENGTH);

    hash->ମୌଳିକ_Index = static_cast<int8_t>(ମୌଳିକ_Index);
    hash->length = PRIMES[ମୌଳିକ_Index];

    p = hash->elements = static_cast<UHashElement*>(
        uprv_malloc(ଆକାର(UHashElement) * hash->length));

    ଯଦି (hash->elements == nullptr) {
        *status = U_MEMORY_ALLOCATION_ERROR;
        ଫେରନ୍ତୁ;
    }

    emptytok.pointer = nullptr; /* Only one of these two is needed */
    emptytok.integer = 0;    /* but we don't know which one. */

    limit = p + hash->length;
    ଯେତେବେଳେ (p < limit) {
        p->ଚାବି = emptytok;
        p->value = emptytok;
        p->hashcode = HASH_EMPTY;
        ++p;
    }

    hash->count = 0;
    hash->ତଳ_Water_Mark = static_cast<int32_t>(hash->length * hash->ତଳ_Water_Ratio);
    hash->ଉପର_Water_Mark = static_cast<int32_t>(hash->length * hash->ଉପର_Water_Ratio);
}

ସ୍ଥିର UHashtable*
_uhash_init(UHashtable *result,
              UHashFunction *ଚାବି_Hash,
              UKeyComparator *ଚାବି_Comp,
              UValueComparator *ମୂଲ୍ୟ_Comp,
              int32_t ମୌଳିକ_Index,
              UErrorCode *status)
{
    ଯଦି (U_FAILURE(*status)) ଫେରନ୍ତୁ nullptr;
    U_ASSERT(ଚାବି_Hash != nullptr);
    U_ASSERT(ଚାବି_Comp != nullptr);

    result->ଚାବି_Hasher       = ଚାବି_Hash;
    result->ଚାବି_Comparator   = ଚାବି_Comp;
    result->ମୂଲ୍ୟ_Comparator = ମୂଲ୍ୟ_Comp;
    result->ଚାବି_Deleter      = nullptr;
    result->ମୂଲ୍ୟ_Deleter    = nullptr;
    result->allocated       = ମିଥ୍ୟା;
    _uhash_internalSetResizePolicy(result, U_GROW);

    _uhash_allocate(result, ମୌଳିକ_Index, status);

    ଯଦି (U_FAILURE(*status)) {
        ଫେରନ୍ତୁ nullptr;
    }

    ଫେରନ୍ତୁ result;
}

ସ୍ଥିର UHashtable*
_uhash_create(UHashFunction *ଚାବି_Hash,
              UKeyComparator *ଚାବି_Comp,
              UValueComparator *ମୂଲ୍ୟ_Comp,
              int32_t ମୌଳିକ_Index,
              UErrorCode *status) {
    UHashtable *result;

    ଯଦି (U_FAILURE(*status)) ଫେରନ୍ତୁ nullptr;

    result = static_cast<UHashtable*>(uprv_malloc(ଆକାର(UHashtable)));
    ଯଦି (result == nullptr) {
        *status = U_MEMORY_ALLOCATION_ERROR;
        ଫେରନ୍ତୁ nullptr;
    }

    _uhash_init(result, ଚାବି_Hash, ଚାବି_Comp, ମୂଲ୍ୟ_Comp, ମୌଳିକ_Index, status);
    result->allocated       = ସତ୍ୟ;

    ଯଦି (U_FAILURE(*status)) {
        uprv_free(result);
        ଫେରନ୍ତୁ nullptr;
    }

    ଫେରନ୍ତୁ result;
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
ସ୍ଥିର UHashElement*
_uhash_find(ଅଚଳ UHashtable *hash, UHashTok ଚାବି,
            int32_t hashcode) {

    int32_t firstDeleted = -1;  /* assume invalid index */
    int32_t theIndex, startIndex;
    int32_t jump = 0; /* lazy evaluate */
    int32_t tableHash;
    UHashElement *elements = hash->elements;

    hashcode &= 0x7FFFFFFF; /* must be positive */
    startIndex = theIndex = (hashcode ^ 0x4000000) % hash->length;

    କର {
        tableHash = elements[theIndex].hashcode;
        ଯଦି (tableHash == hashcode) {          /* quick check */
            ଯଦି ((*hash->ଚାବି_Comparator)(ଚାବି, elements[theIndex].ଚାବି)) {
                ଫେରନ୍ତୁ &(elements[theIndex]);
            }
        } ଅନ୍ୟଥା ଯଦି (!IS_EMPTY_OR_DELETED(tableHash)) {
            /* We have hit a slot which contains a key-value pair,
             * but for which the hash code does not match.  Keep
             * looking.
             */
        } ଅନ୍ୟଥା ଯଦି (tableHash == HASH_EMPTY) { /* empty, end o' the line */
            ଭାଙ୍ଗ;
        } ଅନ୍ୟଥା ଯଦି (firstDeleted < 0) { /* remember first deleted */
            firstDeleted = theIndex;
        }
        ଯଦି (jump == 0) { /* lazy compute jump */
            /* The jump value must be relatively prime to the table
             * length.  As long as the length is prime, then any value
             * 1..length-1 will be relatively prime to it.
             */
            jump = (hashcode % (hash->length - 1)) + 1;
        }
        theIndex = (theIndex + jump) % hash->length;
    } ଯେତେବେଳେ (theIndex != startIndex);

    ଯଦି (firstDeleted >= 0) {
        theIndex = firstDeleted; /* reset if had deleted slot */
    } ଅନ୍ୟଥା ଯଦି (tableHash != HASH_EMPTY) {
        /* We get to this point if the hashtable is full (no empty or
         * deleted slots), and we've failed to find a match.  THIS
         * WILL NEVER HAPPEN as long as uhash_put() makes sure that
         * count is always < length.
         */
        UPRV_UNREACHABLE_EXIT;
    }
    ଫେରନ୍ତୁ &(elements[theIndex]);
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
ସ୍ଥିର ଶୂନ୍ୟ
_uhash_rehash(UHashtable *hash, UErrorCode *status) {

    UHashElement *old = hash->elements;
    int32_t oldLength = hash->length;
    int32_t new_ମୌଳିକ_Index = hash->ମୌଳିକ_Index;
    int32_t i;

    ଯଦି (hash->count > hash->ଉପର_Water_Mark) {
        ଯଦି (++new_ମୌଳିକ_Index >= PRIMES_LENGTH) {
            ଫେରନ୍ତୁ;
        }
    } ଅନ୍ୟଥା ଯଦି (hash->count < hash->ତଳ_Water_Mark) {
        ଯଦି (--new_ମୌଳିକ_Index < 0) {
            ଫେରନ୍ତୁ;
        }
    } ଅନ୍ୟଥା {
        ଫେରନ୍ତୁ;
    }

    _uhash_allocate(hash, new_ମୌଳିକ_Index, status);

    ଯଦି (U_FAILURE(*status)) {
        hash->elements = old;
        hash->length = oldLength;
        ଫେରନ୍ତୁ;
    }

    ପାଇଁ (i = oldLength - 1; i >= 0; --i) {
        ଯଦି (!IS_EMPTY_OR_DELETED(old[i].hashcode)) {
            UHashElement *e = _uhash_find(hash, old[i].ଚାବି, old[i].hashcode);
            U_ASSERT(e != nullptr);
            U_ASSERT(e->hashcode == HASH_EMPTY);
            e->ଚାବି = old[i].ଚାବି;
            e->value = old[i].value;
            e->hashcode = old[i].hashcode;
            ++hash->count;
        }
    }

    uprv_free(old);
}

ସ୍ଥିର UHashTok
_uhash_remove(UHashtable *hash,
              UHashTok ଚାବି) {
    /* First find the position of the key in the table.  If the object
     * has not been removed already, remove it.  If the user wanted
     * keys deleted, then delete it also.  We have to put a special
     * hashcode in that position that means that something has been
     * deleted, since when we do a find, we have to continue PAST any
     * deleted values.
     */
    UHashTok result;
    UHashElement* e = _uhash_find(hash, ଚାବି, hash->ଚାବି_Hasher(ଚାବି));
    U_ASSERT(e != nullptr);
    result.pointer = nullptr;
    result.integer = 0;
    ଯଦି (!IS_EMPTY_OR_DELETED(e->hashcode)) {
        result = _uhash_internalRemoveElement(hash, e);
        ଯଦି (hash->count < hash->ତଳ_Water_Mark) {
            UErrorCode status = U_ZERO_ERROR;
            _uhash_rehash(hash, &status);
        }
    }
    ଫେରନ୍ତୁ result;
}

ସ୍ଥିର UHashTok
_uhash_put(UHashtable *hash,
           UHashTok ଚାବି,
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

    ଯଦି (U_FAILURE(*status)) {
        ଯାଅ err;
    }
    U_ASSERT(hash != nullptr);
    ଯଦି ((hint & HINT_ମୂଲ୍ୟ_POINTER) ?
            value.pointer == nullptr :
            value.integer == 0 && (hint & HINT_ALLOW_ZERO) == 0) {
        /* Disallow storage of nullptr values, since nullptr is returned by
         * get() to indicate an absent key.  Storing nullptr == removing.
         */
        ଫେରନ୍ତୁ _uhash_remove(hash, ଚାବି);
    }
    ଯଦି (hash->count > hash->ଉପର_Water_Mark) {
        _uhash_rehash(hash, status);
        ଯଦି (U_FAILURE(*status)) {
            ଯାଅ err;
        }
    }

    hashcode = (*hash->ଚାବି_Hasher)(ଚାବି);
    e = _uhash_find(hash, ଚାବି, hashcode);
    U_ASSERT(e != nullptr);

    ଯଦି (IS_EMPTY_OR_DELETED(e->hashcode)) {
        /* Important: We must never actually fill the table up.  If we
         * do so, then _uhash_find() will return nullptr, and we'll have
         * to check for nullptr after every call to _uhash_find().  To
         * avoid this we make sure there is always at least one empty
         * or deleted slot in the table.  This only is a problem if we
         * are out of memory and rehash isn't working.
         */
        ++hash->count;
        ଯଦି (hash->count == hash->length) {
            /* Don't allow count to reach length */
            --hash->count;
            *status = U_MEMORY_ALLOCATION_ERROR;
            ଯାଅ err;
        }
    }

    /* We must in all cases handle storage properly.  If there was an
     * old key, then it must be deleted (if the deleter != nullptr).
     * Make hashcodes stored in table positive.
     */
    ଫେରନ୍ତୁ _uhash_setElement(hash, e, hashcode & 0x7FFFFFFF, ଚାବି, value, hint);

 err:
    /* If the deleters are non-nullptr, this method adopts its key and/or
     * value arguments, and we must be sure to delete the key and/or
     * value in all cases, even upon failure.
     */
    HASH_DELETE_ଚାବି_ମୂଲ୍ୟ(hash, ଚାବି.pointer, value.pointer);
    emptytok.pointer = nullptr; emptytok.integer = 0;
    ଫେରନ୍ତୁ emptytok;
}


/********************************************************************
 * PUBLIC API
 ********************************************************************/

U_CAPI UHashtable* U_EXPORT2
uhash_open(UHashFunction *ଚାବି_Hash,
           UKeyComparator *ଚାବି_Comp,
           UValueComparator *ମୂଲ୍ୟ_Comp,
           UErrorCode *status) {

    ଫେରନ୍ତୁ _uhash_create(ଚାବି_Hash, ଚାବି_Comp, ମୂଲ୍ୟ_Comp, DEFAULT_ମୌଳିକ_INDEX, status);
}

U_CAPI UHashtable* U_EXPORT2
uhash_openSize(UHashFunction *ଚାବି_Hash,
               UKeyComparator *ଚାବି_Comp,
               UValueComparator *ମୂଲ୍ୟ_Comp,
               int32_t size,
               UErrorCode *status) {

    /* Find the smallest index i for which PRIMES[i] >= size. */
    int32_t i = 0;
    ଯେତେବେଳେ (i<(PRIMES_LENGTH-1) && PRIMES[i]<size) {
        ++i;
    }

    ଫେରନ୍ତୁ _uhash_create(ଚାବି_Hash, ଚାବି_Comp, ମୂଲ୍ୟ_Comp, i, status);
}

U_CAPI UHashtable* U_EXPORT2
uhash_init(UHashtable *fillinResult,
           UHashFunction *ଚାବି_Hash,
           UKeyComparator *ଚାବି_Comp,
           UValueComparator *ମୂଲ୍ୟ_Comp,
           UErrorCode *status) {

    ଫେରନ୍ତୁ _uhash_init(fillinResult, ଚାବି_Hash, ଚାବି_Comp, ମୂଲ୍ୟ_Comp, DEFAULT_ମୌଳିକ_INDEX, status);
}

U_CAPI UHashtable* U_EXPORT2
uhash_initSize(UHashtable *fillinResult,
               UHashFunction *ଚାବି_Hash,
               UKeyComparator *ଚାବି_Comp,
               UValueComparator *ମୂଲ୍ୟ_Comp,
               int32_t size,
               UErrorCode *status) {

    // Find the smallest index i for which PRIMES[i] >= size.
    int32_t i = 0;
    ଯେତେବେଳେ (i<(PRIMES_LENGTH-1) && PRIMES[i]<size) {
        ++i;
    }
    ଫେରନ୍ତୁ _uhash_init(fillinResult, ଚାବି_Hash, ଚାବି_Comp, ମୂଲ୍ୟ_Comp, i, status);
}

U_CAPI ଶୂନ୍ୟ U_EXPORT2
uhash_close(UHashtable *hash) {
    ଯଦି (hash == nullptr) {
        ଫେରନ୍ତୁ;
    }
    ଯଦି (hash->elements != nullptr) {
        ଯଦି (hash->ଚାବି_Deleter != nullptr || hash->ମୂଲ୍ୟ_Deleter != nullptr) {
            int32_t pos=UHASH_FIRST;
            UHashElement *e;
            ଯେତେବେଳେ ((e = (UHashElement*) uhash_nextElement(hash, &pos)) != nullptr) {
                HASH_DELETE_ଚାବି_ମୂଲ୍ୟ(hash, e->ଚାବି.pointer, e->value.pointer);
            }
        }
        uprv_free(hash->elements);
        hash->elements = nullptr;
    }
    ଯଦି (hash->allocated) {
        uprv_free(hash);
    }
}

U_CAPI UHashFunction *U_EXPORT2
uhash_set_ଚାବି_Hasher(UHashtable *hash, UHashFunction *fn) {
    UHashFunction *result = hash->ଚାବି_Hasher;
    hash->ଚାବି_Hasher = fn;
    ଫେରନ୍ତୁ result;
}

U_CAPI UKeyComparator *U_EXPORT2
uhash_set_ଚାବି_Comparator(UHashtable *hash, UKeyComparator *fn) {
    UKeyComparator *result = hash->ଚାବି_Comparator;
    hash->ଚାବି_Comparator = fn;
    ଫେରନ୍ତୁ result;
}
U_CAPI UValueComparator *U_EXPORT2
uhash_set_ମୂଲ୍ୟ_Comparator(UHashtable *hash, UValueComparator *fn){
    UValueComparator *result = hash->ମୂଲ୍ୟ_Comparator;
    hash->ମୂଲ୍ୟ_Comparator = fn;
    ଫେରନ୍ତୁ result;
}

U_CAPI UObjectDeleter *U_EXPORT2
uhash_set_ଚାବି_Deleter(UHashtable *hash, UObjectDeleter *fn) {
    UObjectDeleter *result = hash->ଚାବି_Deleter;
    hash->ଚାବି_Deleter = fn;
    ଫେରନ୍ତୁ result;
}

U_CAPI UObjectDeleter *U_EXPORT2
uhash_set_ମୂଲ୍ୟ_Deleter(UHashtable *hash, UObjectDeleter *fn) {
    UObjectDeleter *result = hash->ମୂଲ୍ୟ_Deleter;
    hash->ମୂଲ୍ୟ_Deleter = fn;
    ଫେରନ୍ତୁ result;
}

U_CAPI ଶୂନ୍ୟ U_EXPORT2
uhash_setResizePolicy(UHashtable *hash, ଗଣନା UHashResizePolicy policy) {
    UErrorCode status = U_ZERO_ERROR;
    _uhash_internalSetResizePolicy(hash, policy);
    hash->ତଳ_Water_Mark  = (int32_t)(hash->length * hash->ତଳ_Water_Ratio);
    hash->ଉପର_Water_Mark = (int32_t)(hash->length * hash->ଉପର_Water_Ratio);
    _uhash_rehash(hash, &status);
}

U_CAPI int32_t U_EXPORT2
uhash_ଗଣନା(ଅଚଳ UHashtable *hash) {
    ଫେରନ୍ତୁ hash->count;
}

U_CAPI ଶୂନ୍ୟ* U_EXPORT2
uhash_get(ଅଚଳ UHashtable *hash,
          ଅଚଳ ଶୂନ୍ୟ* ଚାବି) {
    UHashTok keyholder;
    keyholder.pointer = (ଶୂନ୍ୟ*) ଚାବି;
    ଫେରନ୍ତୁ _uhash_find(hash, keyholder, hash->ଚାବି_Hasher(keyholder))->value.pointer;
}

U_CAPI ଶୂନ୍ୟ* U_EXPORT2
uhash_iget(ଅଚଳ UHashtable *hash,
           int32_t ଚାବି) {
    UHashTok keyholder;
    keyholder.integer = ଚାବି;
    ଫେରନ୍ତୁ _uhash_find(hash, keyholder, hash->ଚାବି_Hasher(keyholder))->value.pointer;
}

U_CAPI int32_t U_EXPORT2
uhash_geti(ଅଚଳ UHashtable *hash,
           ଅଚଳ ଶୂନ୍ୟ* ଚାବି) {
    UHashTok keyholder;
    keyholder.pointer = (ଶୂନ୍ୟ*) ଚାବି;
    ଫେରନ୍ତୁ _uhash_find(hash, keyholder, hash->ଚାବି_Hasher(keyholder))->value.integer;
}

U_CAPI int32_t U_EXPORT2
uhash_igeti(ଅଚଳ UHashtable *hash,
           int32_t ଚାବି) {
    UHashTok keyholder;
    keyholder.integer = ଚାବି;
    ଫେରନ୍ତୁ _uhash_find(hash, keyholder, hash->ଚାବି_Hasher(keyholder))->value.integer;
}

U_CAPI int32_t U_EXPORT2
uhash_getiAndFound(ଅଚଳ UHashtable *hash,
                   ଅଚଳ ଶୂନ୍ୟ *ଚାବି,
                   UBool *found) {
    UHashTok keyholder;
    keyholder.pointer = (ଶୂନ୍ୟ *)ଚାବି;
    ଅଚଳ UHashElement *e = _uhash_find(hash, keyholder, hash->ଚାବି_Hasher(keyholder));
    *found = !IS_EMPTY_OR_DELETED(e->hashcode);
    ଫେରନ୍ତୁ e->value.integer;
}

U_CAPI int32_t U_EXPORT2
uhash_igetiAndFound(ଅଚଳ UHashtable *hash,
                    int32_t ଚାବି,
                    UBool *found) {
    UHashTok keyholder;
    keyholder.integer = ଚାବି;
    ଅଚଳ UHashElement *e = _uhash_find(hash, keyholder, hash->ଚାବି_Hasher(keyholder));
    *found = !IS_EMPTY_OR_DELETED(e->hashcode);
    ଫେରନ୍ତୁ e->value.integer;
}

U_CAPI ଶୂନ୍ୟ* U_EXPORT2
uhash_put(UHashtable *hash,
          ଶୂନ୍ୟ* ଚାବି,
          ଶୂନ୍ୟ* value,
          UErrorCode *status) {
    UHashTok keyholder, valueholder;
    keyholder.pointer = ଚାବି;
    valueholder.pointer = value;
    ଫେରନ୍ତୁ _uhash_put(hash, keyholder, valueholder,
                      HINT_ଚାବି_POINTER | HINT_ମୂଲ୍ୟ_POINTER,
                      status).pointer;
}

U_CAPI ଶୂନ୍ୟ* U_EXPORT2
uhash_iput(UHashtable *hash,
           int32_t ଚାବି,
           ଶୂନ୍ୟ* value,
           UErrorCode *status) {
    UHashTok keyholder, valueholder;
    keyholder.integer = ଚାବି;
    valueholder.pointer = value;
    ଫେରନ୍ତୁ _uhash_put(hash, keyholder, valueholder,
                      HINT_ମୂଲ୍ୟ_POINTER,
                      status).pointer;
}

U_CAPI int32_t U_EXPORT2
uhash_puti(UHashtable *hash,
           ଶୂନ୍ୟ* ଚାବି,
           int32_t value,
           UErrorCode *status) {
    UHashTok keyholder, valueholder;
    keyholder.pointer = ଚାବି;
    valueholder.integer = value;
    ଫେରନ୍ତୁ _uhash_put(hash, keyholder, valueholder,
                      HINT_ଚାବି_POINTER,
                      status).integer;
}


U_CAPI int32_t U_EXPORT2
uhash_iputi(UHashtable *hash,
           int32_t ଚାବି,
           int32_t value,
           UErrorCode *status) {
    UHashTok keyholder, valueholder;
    keyholder.integer = ଚାବି;
    valueholder.integer = value;
    ଫେରନ୍ତୁ _uhash_put(hash, keyholder, valueholder,
                      HINT_BOTH_INTEGERS,
                      status).integer;
}

U_CAPI int32_t U_EXPORT2
uhash_putiAllowZero(UHashtable *hash,
                    ଶୂନ୍ୟ *ଚାବି,
                    int32_t value,
                    UErrorCode *status) {
    UHashTok keyholder, valueholder;
    keyholder.pointer = ଚାବି;
    valueholder.integer = value;
    ଫେରନ୍ତୁ _uhash_put(hash, keyholder, valueholder,
                      HINT_ଚାବି_POINTER | HINT_ALLOW_ZERO,
                      status).integer;
}


U_CAPI int32_t U_EXPORT2
uhash_iputiAllowZero(UHashtable *hash,
                     int32_t ଚାବି,
                     int32_t value,
                     UErrorCode *status) {
    UHashTok keyholder, valueholder;
    keyholder.integer = ଚାବି;
    valueholder.integer = value;
    ଫେରନ୍ତୁ _uhash_put(hash, keyholder, valueholder,
                      HINT_BOTH_INTEGERS | HINT_ALLOW_ZERO,
                      status).integer;
}

U_CAPI ଶୂନ୍ୟ* U_EXPORT2
uhash_remove(UHashtable *hash,
             ଅଚଳ ଶୂନ୍ୟ* ଚାବି) {
    UHashTok keyholder;
    keyholder.pointer = (ଶୂନ୍ୟ*) ଚାବି;
    ଫେରନ୍ତୁ _uhash_remove(hash, keyholder).pointer;
}

U_CAPI ଶୂନ୍ୟ* U_EXPORT2
uhash_iremove(UHashtable *hash,
              int32_t ଚାବି) {
    UHashTok keyholder;
    keyholder.integer = ଚାବି;
    ଫେରନ୍ତୁ _uhash_remove(hash, keyholder).pointer;
}

U_CAPI int32_t U_EXPORT2
uhash_removei(UHashtable *hash,
              ଅଚଳ ଶୂନ୍ୟ* ଚାବି) {
    UHashTok keyholder;
    keyholder.pointer = (ଶୂନ୍ୟ*) ଚାବି;
    ଫେରନ୍ତୁ _uhash_remove(hash, keyholder).integer;
}

U_CAPI int32_t U_EXPORT2
uhash_iremovei(UHashtable *hash,
               int32_t ଚାବି) {
    UHashTok keyholder;
    keyholder.integer = ଚାବି;
    ଫେରନ୍ତୁ _uhash_remove(hash, keyholder).integer;
}

U_CAPI ଶୂନ୍ୟ U_EXPORT2
uhash_removeAll(UHashtable *hash) {
    int32_t pos = UHASH_FIRST;
    ଅଚଳ UHashElement *e;
    U_ASSERT(hash != nullptr);
    ଯଦି (hash->count != 0) {
        ଯେତେବେଳେ ((e = uhash_nextElement(hash, &pos)) != nullptr) {
            uhash_removeElement(hash, e);
        }
    }
    U_ASSERT(hash->count == 0);
}

U_CAPI UBool U_EXPORT2
uhash_contains_ଚାବି(ଅଚଳ UHashtable *hash, ଅଚଳ ଶୂନ୍ୟ *ଚାବି) {
    UHashTok keyholder;
    keyholder.pointer = (ଶୂନ୍ୟ *)ଚାବି;
    ଅଚଳ UHashElement *e = _uhash_find(hash, keyholder, hash->ଚାବି_Hasher(keyholder));
    ଫେରନ୍ତୁ !IS_EMPTY_OR_DELETED(e->hashcode);
}

/**
 * Returns true if the UHashtable contains an item with this integer key.
 *
 * @param hash The target UHashtable.
 * @param key An integer key stored in a hashtable
 * @return true if the key is found.
 */
U_CAPI UBool U_EXPORT2
uhash_icontains_ଚାବି(ଅଚଳ UHashtable *hash, int32_t ଚାବି) {
    UHashTok keyholder;
    keyholder.integer = ଚାବି;
    ଅଚଳ UHashElement *e = _uhash_find(hash, keyholder, hash->ଚାବି_Hasher(keyholder));
    ଫେରନ୍ତୁ !IS_EMPTY_OR_DELETED(e->hashcode);
}

U_CAPI ଅଚଳ UHashElement* U_EXPORT2
uhash_find(ଅଚଳ UHashtable *hash, ଅଚଳ ଶୂନ୍ୟ* ଚାବି) {
    UHashTok keyholder;
    ଅଚଳ UHashElement *e;
    keyholder.pointer = (ଶୂନ୍ୟ*) ଚାବି;
    e = _uhash_find(hash, keyholder, hash->ଚାବି_Hasher(keyholder));
    ଫେରନ୍ତୁ IS_EMPTY_OR_DELETED(e->hashcode) ? nullptr : e;
}

U_CAPI ଅଚଳ UHashElement* U_EXPORT2
uhash_nextElement(ଅଚଳ UHashtable *hash, int32_t *pos) {
    /* Walk through the array until we find an element that is not
     * EMPTY and not DELETED.
     */
    int32_t i;
    U_ASSERT(hash != nullptr);
    ପାଇଁ (i = *pos + 1; i < hash->length; ++i) {
        ଯଦି (!IS_EMPTY_OR_DELETED(hash->elements[i].hashcode)) {
            *pos = i;
            ଫେରନ୍ତୁ &(hash->elements[i]);
        }
    }

    /* No more elements */
    ଫେରନ୍ତୁ nullptr;
}

U_CAPI ଶୂନ୍ୟ* U_EXPORT2
uhash_removeElement(UHashtable *hash, ଅଚଳ UHashElement* e) {
    U_ASSERT(hash != nullptr);
    U_ASSERT(e != nullptr);
    ଯଦି (!IS_EMPTY_OR_DELETED(e->hashcode)) {
        UHashElement *nce = (UHashElement *)e;
        ଫେରନ୍ତୁ _uhash_internalRemoveElement(hash, nce).pointer;
    }
    ଫେରନ୍ତୁ nullptr;
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
uhash_hashUChars(ଅଚଳ UHashTok ଚାବି) {
    ଅଚଳ char16_t *s = (ଅଚଳ char16_t *)ଚାବି.pointer;
    ଫେରନ୍ତୁ s == nullptr ? 0 : ustr_hashUCharsN(s, u_strlen(s));
}

U_CAPI int32_t U_EXPORT2
uhash_hashChars(ଅଚଳ UHashTok ଚାବି) {
    ଅଚଳ ଅକ୍ଷର *s = (ଅଚଳ ଅକ୍ଷର *)ଚାବି.pointer;
    ଫେରନ୍ତୁ s == nullptr ? 0 : static_cast<int32_t>(ustr_hashCharsN(s, static_cast<int32_t>(uprv_strlen(s))));
}

U_CAPI int32_t U_EXPORT2
uhash_hashIChars(ଅଚଳ UHashTok ଚାବି) {
    ଅଚଳ ଅକ୍ଷର *s = (ଅଚଳ ଅକ୍ଷର *)ଚାବି.pointer;
    ଫେରନ୍ତୁ s == nullptr ? 0 : ustr_hashICharsN(s, static_cast<int32_t>(uprv_strlen(s)));
}

U_CAPI int32_t U_EXPORT2
uhash_hashIStringView(ଅଚଳ UHashTok ଚାବି) {
    ଅଚଳ std::string_view* s = static_cast<std::string_view*>(ଚାବି.pointer);
    ଫେରନ୍ତୁ s == nullptr ? 0 : ustr_hashICharsN(s->data(), static_cast<int32_t>(s->size()));
}

U_CAPI UBool U_EXPORT2
uhash_equals(ଅଚଳ UHashtable* hash1, ଅଚଳ UHashtable* hash2){
    int32_t count1, count2, pos, i;

    ଯଦି(hash1==hash2){
        ଫେରନ୍ତୁ ସତ୍ୟ;
    }

    /*
     * Make sure that we are comparing 2 valid hashes of the same type
     * with valid comparison functions.
     * Without valid comparison functions, a binary comparison
     * of the hash values will yield random results on machines
     * with 64-bit pointers and 32-bit integer hashes.
     * A valueComparator is normally optional.
     */
    ଯଦି (hash1==nullptr || hash2==nullptr ||
        hash1->ଚାବି_Comparator != hash2->ଚାବି_Comparator ||
        hash1->ମୂଲ୍ୟ_Comparator != hash2->ମୂଲ୍ୟ_Comparator ||
        hash1->ମୂଲ୍ୟ_Comparator == nullptr)
    {
        /*
        Normally we would return an error here about incompatible hash tables,
        but we return false instead.
        */
        ଫେରନ୍ତୁ ମିଥ୍ୟା;
    }

    count1 = uhash_ଗଣନା(hash1);
    count2 = uhash_ଗଣନା(hash2);
    ଯଦି(count1!=count2){
        ଫେରନ୍ତୁ ମିଥ୍ୟା;
    }

    pos=UHASH_FIRST;
    ପାଇଁ(i=0; i<count1; i++){
        ଅଚଳ UHashElement* elem1 = uhash_nextElement(hash1, &pos);
        ଅଚଳ UHashTok key1 = elem1->ଚାବି;
        ଅଚଳ UHashTok val1 = elem1->value;
        /* here the keys are not compared, instead the key form hash1 is used to fetch
         * value from hash2. If the hashes are equal then then both hashes should
         * contain equal values for the same key!
         */
        ଅଚଳ UHashElement* elem2 = _uhash_find(hash2, key1, hash2->ଚାବି_Hasher(key1));
        ଅଚଳ UHashTok val2 = elem2->value;
        ଯଦି(hash1->ମୂଲ୍ୟ_Comparator(val1, val2)==ମିଥ୍ୟା){
            ଫେରନ୍ତୁ ମିଥ୍ୟା;
        }
    }
    ଫେରନ୍ତୁ ସତ୍ୟ;
}

/********************************************************************
 * PUBLIC Comparator Functions
 ********************************************************************/

U_CAPI UBool U_EXPORT2
uhash_compareUChars(ଅଚଳ UHashTok key1, ଅଚଳ UHashTok key2) {
    ଅଚଳ char16_t *p1 = (ଅଚଳ char16_t*) key1.pointer;
    ଅଚଳ char16_t *p2 = (ଅଚଳ char16_t*) key2.pointer;
    ଯଦି (p1 == p2) {
        ଫେରନ୍ତୁ ସତ୍ୟ;
    }
    ଯଦି (p1 == nullptr || p2 == nullptr) {
        ଫେରନ୍ତୁ ମିଥ୍ୟା;
    }
    ଯେତେବେଳେ (*p1 != 0 && *p1 == *p2) {
        ++p1;
        ++p2;
    }
    ଫେରନ୍ତୁ *p1 == *p2;
}

U_CAPI UBool U_EXPORT2
uhash_compareChars(ଅଚଳ UHashTok key1, ଅଚଳ UHashTok key2) {
    ଅଚଳ ଅକ୍ଷର *p1 = (ଅଚଳ ଅକ୍ଷର*) key1.pointer;
    ଅଚଳ ଅକ୍ଷର *p2 = (ଅଚଳ ଅକ୍ଷର*) key2.pointer;
    ଯଦି (p1 == p2) {
        ଫେରନ୍ତୁ ସତ୍ୟ;
    }
    ଯଦି (p1 == nullptr || p2 == nullptr) {
        ଫେରନ୍ତୁ ମିଥ୍ୟା;
    }
    ଯେତେବେଳେ (*p1 != 0 && *p1 == *p2) {
        ++p1;
        ++p2;
    }
    ଫେରନ୍ତୁ *p1 == *p2;
}

U_CAPI UBool U_EXPORT2
uhash_compareIChars(ଅଚଳ UHashTok key1, ଅଚଳ UHashTok key2) {
    ଅଚଳ ଅକ୍ଷର *p1 = (ଅଚଳ ଅକ୍ଷର*) key1.pointer;
    ଅଚଳ ଅକ୍ଷର *p2 = (ଅଚଳ ଅକ୍ଷର*) key2.pointer;
    ଯଦି (p1 == p2) {
        ଫେରନ୍ତୁ ସତ୍ୟ;
    }
    ଯଦି (p1 == nullptr || p2 == nullptr) {
        ଫେରନ୍ତୁ ମିଥ୍ୟା;
    }
    ଯେତେବେଳେ (*p1 != 0 && uprv_tolower(*p1) == uprv_tolower(*p2)) {
        ++p1;
        ++p2;
    }
    ଫେରନ୍ତୁ *p1 == *p2;
}

U_CAPI UBool U_EXPORT2
uhash_compareIStringView(ଅଚଳ UHashTok key1, ଅଚଳ UHashTok key2) {
    ଅଚଳ std::string_view* p1 = static_cast<std::string_view*>(key1.pointer);
    ଅଚଳ std::string_view* p2 = static_cast<std::string_view*>(key2.pointer);
    ଯଦି (p1 == p2) {
        ଫେରନ୍ତୁ ସତ୍ୟ;
    }
    ଯଦି (p1 == nullptr || p2 == nullptr) {
        ଫେରନ୍ତୁ ମିଥ୍ୟା;
    }
    ଅଚଳ std::string_view& v1 = *p1;
    ଅଚଳ std::string_view& v2 = *p2;
    ଯଦି (v1.size() != v2.size()) {
        ଫେରନ୍ତୁ ମିଥ୍ୟା;
    }
    ପାଇଁ (size_t i = 0; i < v1.size(); ++i) {
        ଯଦି (uprv_tolower(v1[i]) != uprv_tolower(v2[i])) {
            ଫେରନ୍ତୁ ମିଥ୍ୟା;
        }
    }
    ଫେରନ୍ତୁ ସତ୍ୟ;
}

/********************************************************************
 * PUBLIC int32_t Support Functions
 ********************************************************************/

U_CAPI int32_t U_EXPORT2
uhash_hashLong(ଅଚଳ UHashTok ଚାବି) {
    ଫେରନ୍ତୁ ଚାବି.integer;
}

U_CAPI UBool U_EXPORT2
uhash_compareLong(ଅଚଳ UHashTok key1, ଅଚଳ UHashTok key2) {
    ଫେରନ୍ତୁ key1.integer == key2.integer;
}
