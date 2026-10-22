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

#അന്തര്ഭാവ <string_view>

#അന്തര്ഭാവ "uhash.h"
#അന്തര്ഭാവ "unicode/ustring.h"
#അന്തര്ഭാവ "cstring.h"
#അന്തര്ഭാവ "cmemory.h"
#അന്തര്ഭാവ "uassert.h"
#അന്തര്ഭാവ "ustr_imp.h"

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
സ്ഥിതം സ്ഥിരം int32_t PRIMES[] = {
    7, 13, 31, 61, 127, 251, 509, 1021, 2039, 4093, 8191, 16381, 32749,
    65521, 131071, 262139, 524287, 1048573, 2097143, 4194301, 8388593,
    16777213, 33554393, 67108859, 134217689, 268435399, 536870909,
    1073741789, 2147483647 /*, 4294967291 */
};

#define PRIMES_LENGTH UPRV_LENGTHOF(PRIMES)
#define DEFAULT_അഭാജ്യം_INDEX 4

/* These ratios are tuned to the PRIMES array such that a resize
 * places the table back into the zone of non-resizing.  That is,
 * after a call to _uhash_rehash(), a subsequent call to
 * _uhash_rehash() should do nothing (should not churn).  This is only
 * a potential problem with U_GROW_AND_SHRINK.
 */
സ്ഥിതം സ്ഥിരം ദശാംശം RESIZE_POLICY_RATIO_TABLE[6] = {
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
#define HASH_DELETE_താക്കോൽ_മൂല്യം(hash, keypointer, valuepointer) UPRV_BLOCK_MACRO_BEGIN { \
    എങ്കിൽ (hash->താക്കോൽ_Deleter != nullptr && keypointer != nullptr) { \
        (*hash->താക്കോൽ_Deleter)(keypointer); \
    } \
    എങ്കിൽ (hash->മൂല്യം_Deleter != nullptr && valuepointer != nullptr) { \
        (*hash->മൂല്യം_Deleter)(valuepointer); \
    } \
} UPRV_BLOCK_MACRO_END

/*
 * Constants for hinting whether a key or value is an integer
 * or a pointer.  If a hint bit is zero, then the associated
 * token is assumed to be an integer.
 */
#define HINT_BOTH_INTEGERS (0)
#define HINT_താക്കോൽ_POINTER   (1)
#define HINT_മൂല്യം_POINTER (2)
#define HINT_ALLOW_ZERO    (4)

/********************************************************************
 * PRIVATE Implementation
 ********************************************************************/

സ്ഥിതം UHashTok
_uhash_setElement(UHashtable *hash, UHashElement* e,
                  int32_t hashcode,
                  UHashTok താക്കോൽ, UHashTok value, int8_t hint) {

    UHashTok old_മൂല്യം = e->value;
    എങ്കിൽ (hash->താക്കോൽ_Deleter != nullptr && e->താക്കോൽ.pointer != nullptr &&
        e->താക്കോൽ.pointer != താക്കോൽ.pointer) { /* Avoid double deletion */
        (*hash->താക്കോൽ_Deleter)(e->താക്കോൽ.pointer);
    }
    എങ്കിൽ (hash->മൂല്യം_Deleter != nullptr) {
        എങ്കിൽ (old_മൂല്യം.pointer != nullptr &&
            old_മൂല്യം.pointer != value.pointer) { /* Avoid double deletion */
            (*hash->മൂല്യം_Deleter)(old_മൂല്യം.pointer);
        }
        old_മൂല്യം.pointer = nullptr;
    }
    /* Compilers should copy the UHashTok union correctly, but even if
     * they do, memory heap tools (e.g. BoundsChecker) can get
     * confused when a pointer is cloaked in a union and then copied.
     * TO ALLEVIATE THIS, we use hints (based on what API the user is
     * calling) to copy pointers when we know the user thinks
     * something is a pointer. */
    എങ്കിൽ (hint & HINT_താക്കോൽ_POINTER) {
        e->താക്കോൽ.pointer = താക്കോൽ.pointer;
    } അല്ലെങ്കിൽ {
        e->താക്കോൽ = താക്കോൽ;
    }
    എങ്കിൽ (hint & HINT_മൂല്യം_POINTER) {
        e->value.pointer = value.pointer;
    } അല്ലെങ്കിൽ {
        e->value = value;
    }
    e->hashcode = hashcode;
    തിരികെ old_മൂല്യം;
}

/**
 * Assumes that the given element is not empty or deleted.
 */
സ്ഥിതം UHashTok
_uhash_internalRemoveElement(UHashtable *hash, UHashElement* e) {
    UHashTok empty;
    U_ASSERT(!IS_EMPTY_OR_DELETED(e->hashcode));
    --hash->count;
    empty.pointer = nullptr; empty.integer = 0;
    തിരികെ _uhash_setElement(hash, e, HASH_DELETED, empty, empty, 0);
}

സ്ഥിതം ശൂന്യം
_uhash_internalSetResizePolicy(UHashtable *hash, എണ്ണൽ UHashResizePolicy policy) {
    U_ASSERT(hash != nullptr);
    U_ASSERT(((int32_t)policy) >= 0);
    U_ASSERT(((int32_t)policy) < 3);
    hash->താഴെയുള്ള_Water_Ratio  = RESIZE_POLICY_RATIO_TABLE[policy * 2];
    hash->മുകളിലെ_Water_Ratio = RESIZE_POLICY_RATIO_TABLE[policy * 2 + 1];
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
സ്ഥിതം ശൂന്യം
_uhash_allocate(UHashtable *hash,
                int32_t അഭാജ്യം_Index,
                UErrorCode *status) {

    UHashElement *p, *limit;
    UHashTok emptytok;

    എങ്കിൽ (U_FAILURE(*status)) തിരികെ;

    U_ASSERT(അഭാജ്യം_Index >= 0 && അഭാജ്യം_Index < PRIMES_LENGTH);

    hash->അഭാജ്യം_Index = static_cast<int8_t>(അഭാജ്യം_Index);
    hash->length = PRIMES[അഭാജ്യം_Index];

    p = hash->elements = static_cast<UHashElement*>(
        uprv_malloc(വലുപ്പം(UHashElement) * hash->length));

    എങ്കിൽ (hash->elements == nullptr) {
        *status = U_MEMORY_ALLOCATION_ERROR;
        തിരികെ;
    }

    emptytok.pointer = nullptr; /* Only one of these two is needed */
    emptytok.integer = 0;    /* but we don't know which one. */

    limit = p + hash->length;
    ആകുമ്പോൾ (p < limit) {
        p->താക്കോൽ = emptytok;
        p->value = emptytok;
        p->hashcode = HASH_EMPTY;
        ++p;
    }

    hash->count = 0;
    hash->താഴെയുള്ള_Water_Mark = static_cast<int32_t>(hash->length * hash->താഴെയുള്ള_Water_Ratio);
    hash->മുകളിലെ_Water_Mark = static_cast<int32_t>(hash->length * hash->മുകളിലെ_Water_Ratio);
}

സ്ഥിതം UHashtable*
_uhash_init(UHashtable *result,
              UHashFunction *താക്കോൽ_Hash,
              UKeyComparator *താക്കോൽ_Comp,
              UValueComparator *മൂല്യം_Comp,
              int32_t അഭാജ്യം_Index,
              UErrorCode *status)
{
    എങ്കിൽ (U_FAILURE(*status)) തിരികെ nullptr;
    U_ASSERT(താക്കോൽ_Hash != nullptr);
    U_ASSERT(താക്കോൽ_Comp != nullptr);

    result->താക്കോൽ_Hasher       = താക്കോൽ_Hash;
    result->താക്കോൽ_Comparator   = താക്കോൽ_Comp;
    result->മൂല്യം_Comparator = മൂല്യം_Comp;
    result->താക്കോൽ_Deleter      = nullptr;
    result->മൂല്യം_Deleter    = nullptr;
    result->allocated       = അസത്യം;
    _uhash_internalSetResizePolicy(result, U_GROW);

    _uhash_allocate(result, അഭാജ്യം_Index, status);

    എങ്കിൽ (U_FAILURE(*status)) {
        തിരികെ nullptr;
    }

    തിരികെ result;
}

സ്ഥിതം UHashtable*
_uhash_create(UHashFunction *താക്കോൽ_Hash,
              UKeyComparator *താക്കോൽ_Comp,
              UValueComparator *മൂല്യം_Comp,
              int32_t അഭാജ്യം_Index,
              UErrorCode *status) {
    UHashtable *result;

    എങ്കിൽ (U_FAILURE(*status)) തിരികെ nullptr;

    result = static_cast<UHashtable*>(uprv_malloc(വലുപ്പം(UHashtable)));
    എങ്കിൽ (result == nullptr) {
        *status = U_MEMORY_ALLOCATION_ERROR;
        തിരികെ nullptr;
    }

    _uhash_init(result, താക്കോൽ_Hash, താക്കോൽ_Comp, മൂല്യം_Comp, അഭാജ്യം_Index, status);
    result->allocated       = സത്യം;

    എങ്കിൽ (U_FAILURE(*status)) {
        uprv_free(result);
        തിരികെ nullptr;
    }

    തിരികെ result;
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
സ്ഥിതം UHashElement*
_uhash_find(സ്ഥിരം UHashtable *hash, UHashTok താക്കോൽ,
            int32_t hashcode) {

    int32_t firstDeleted = -1;  /* assume invalid index */
    int32_t theIndex, startIndex;
    int32_t jump = 0; /* lazy evaluate */
    int32_t tableHash;
    UHashElement *elements = hash->elements;

    hashcode &= 0x7FFFFFFF; /* must be positive */
    startIndex = theIndex = (hashcode ^ 0x4000000) % hash->length;

    ചെയ്യുക {
        tableHash = elements[theIndex].hashcode;
        എങ്കിൽ (tableHash == hashcode) {          /* quick check */
            എങ്കിൽ ((*hash->താക്കോൽ_Comparator)(താക്കോൽ, elements[theIndex].താക്കോൽ)) {
                തിരികെ &(elements[theIndex]);
            }
        } അല്ലെങ്കിൽ എങ്കിൽ (!IS_EMPTY_OR_DELETED(tableHash)) {
            /* We have hit a slot which contains a key-value pair,
             * but for which the hash code does not match.  Keep
             * looking.
             */
        } അല്ലെങ്കിൽ എങ്കിൽ (tableHash == HASH_EMPTY) { /* empty, end o' the line */
            നിർത്തുക;
        } അല്ലെങ്കിൽ എങ്കിൽ (firstDeleted < 0) { /* remember first deleted */
            firstDeleted = theIndex;
        }
        എങ്കിൽ (jump == 0) { /* lazy compute jump */
            /* The jump value must be relatively prime to the table
             * length.  As long as the length is prime, then any value
             * 1..length-1 will be relatively prime to it.
             */
            jump = (hashcode % (hash->length - 1)) + 1;
        }
        theIndex = (theIndex + jump) % hash->length;
    } ആകുമ്പോൾ (theIndex != startIndex);

    എങ്കിൽ (firstDeleted >= 0) {
        theIndex = firstDeleted; /* reset if had deleted slot */
    } അല്ലെങ്കിൽ എങ്കിൽ (tableHash != HASH_EMPTY) {
        /* We get to this point if the hashtable is full (no empty or
         * deleted slots), and we've failed to find a match.  THIS
         * WILL NEVER HAPPEN as long as uhash_put() makes sure that
         * count is always < length.
         */
        UPRV_UNREACHABLE_EXIT;
    }
    തിരികെ &(elements[theIndex]);
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
സ്ഥിതം ശൂന്യം
_uhash_rehash(UHashtable *hash, UErrorCode *status) {

    UHashElement *old = hash->elements;
    int32_t oldLength = hash->length;
    int32_t new_അഭാജ്യം_Index = hash->അഭാജ്യം_Index;
    int32_t i;

    എങ്കിൽ (hash->count > hash->മുകളിലെ_Water_Mark) {
        എങ്കിൽ (++new_അഭാജ്യം_Index >= PRIMES_LENGTH) {
            തിരികെ;
        }
    } അല്ലെങ്കിൽ എങ്കിൽ (hash->count < hash->താഴെയുള്ള_Water_Mark) {
        എങ്കിൽ (--new_അഭാജ്യം_Index < 0) {
            തിരികെ;
        }
    } അല്ലെങ്കിൽ {
        തിരികെ;
    }

    _uhash_allocate(hash, new_അഭാജ്യം_Index, status);

    എങ്കിൽ (U_FAILURE(*status)) {
        hash->elements = old;
        hash->length = oldLength;
        തിരികെ;
    }

    വേണ്ടി (i = oldLength - 1; i >= 0; --i) {
        എങ്കിൽ (!IS_EMPTY_OR_DELETED(old[i].hashcode)) {
            UHashElement *e = _uhash_find(hash, old[i].താക്കോൽ, old[i].hashcode);
            U_ASSERT(e != nullptr);
            U_ASSERT(e->hashcode == HASH_EMPTY);
            e->താക്കോൽ = old[i].താക്കോൽ;
            e->value = old[i].value;
            e->hashcode = old[i].hashcode;
            ++hash->count;
        }
    }

    uprv_free(old);
}

സ്ഥിതം UHashTok
_uhash_remove(UHashtable *hash,
              UHashTok താക്കോൽ) {
    /* First find the position of the key in the table.  If the object
     * has not been removed already, remove it.  If the user wanted
     * keys deleted, then delete it also.  We have to put a special
     * hashcode in that position that means that something has been
     * deleted, since when we do a find, we have to continue PAST any
     * deleted values.
     */
    UHashTok result;
    UHashElement* e = _uhash_find(hash, താക്കോൽ, hash->താക്കോൽ_Hasher(താക്കോൽ));
    U_ASSERT(e != nullptr);
    result.pointer = nullptr;
    result.integer = 0;
    എങ്കിൽ (!IS_EMPTY_OR_DELETED(e->hashcode)) {
        result = _uhash_internalRemoveElement(hash, e);
        എങ്കിൽ (hash->count < hash->താഴെയുള്ള_Water_Mark) {
            UErrorCode status = U_ZERO_ERROR;
            _uhash_rehash(hash, &status);
        }
    }
    തിരികെ result;
}

സ്ഥിതം UHashTok
_uhash_put(UHashtable *hash,
           UHashTok താക്കോൽ,
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

    എങ്കിൽ (U_FAILURE(*status)) {
        പോകുക err;
    }
    U_ASSERT(hash != nullptr);
    എങ്കിൽ ((hint & HINT_മൂല്യം_POINTER) ?
            value.pointer == nullptr :
            value.integer == 0 && (hint & HINT_ALLOW_ZERO) == 0) {
        /* Disallow storage of nullptr values, since nullptr is returned by
         * get() to indicate an absent key.  Storing nullptr == removing.
         */
        തിരികെ _uhash_remove(hash, താക്കോൽ);
    }
    എങ്കിൽ (hash->count > hash->മുകളിലെ_Water_Mark) {
        _uhash_rehash(hash, status);
        എങ്കിൽ (U_FAILURE(*status)) {
            പോകുക err;
        }
    }

    hashcode = (*hash->താക്കോൽ_Hasher)(താക്കോൽ);
    e = _uhash_find(hash, താക്കോൽ, hashcode);
    U_ASSERT(e != nullptr);

    എങ്കിൽ (IS_EMPTY_OR_DELETED(e->hashcode)) {
        /* Important: We must never actually fill the table up.  If we
         * do so, then _uhash_find() will return nullptr, and we'll have
         * to check for nullptr after every call to _uhash_find().  To
         * avoid this we make sure there is always at least one empty
         * or deleted slot in the table.  This only is a problem if we
         * are out of memory and rehash isn't working.
         */
        ++hash->count;
        എങ്കിൽ (hash->count == hash->length) {
            /* Don't allow count to reach length */
            --hash->count;
            *status = U_MEMORY_ALLOCATION_ERROR;
            പോകുക err;
        }
    }

    /* We must in all cases handle storage properly.  If there was an
     * old key, then it must be deleted (if the deleter != nullptr).
     * Make hashcodes stored in table positive.
     */
    തിരികെ _uhash_setElement(hash, e, hashcode & 0x7FFFFFFF, താക്കോൽ, value, hint);

 err:
    /* If the deleters are non-nullptr, this method adopts its key and/or
     * value arguments, and we must be sure to delete the key and/or
     * value in all cases, even upon failure.
     */
    HASH_DELETE_താക്കോൽ_മൂല്യം(hash, താക്കോൽ.pointer, value.pointer);
    emptytok.pointer = nullptr; emptytok.integer = 0;
    തിരികെ emptytok;
}


/********************************************************************
 * PUBLIC API
 ********************************************************************/

U_CAPI UHashtable* U_EXPORT2
uhash_open(UHashFunction *താക്കോൽ_Hash,
           UKeyComparator *താക്കോൽ_Comp,
           UValueComparator *മൂല്യം_Comp,
           UErrorCode *status) {

    തിരികെ _uhash_create(താക്കോൽ_Hash, താക്കോൽ_Comp, മൂല്യം_Comp, DEFAULT_അഭാജ്യം_INDEX, status);
}

U_CAPI UHashtable* U_EXPORT2
uhash_openSize(UHashFunction *താക്കോൽ_Hash,
               UKeyComparator *താക്കോൽ_Comp,
               UValueComparator *മൂല്യം_Comp,
               int32_t size,
               UErrorCode *status) {

    /* Find the smallest index i for which PRIMES[i] >= size. */
    int32_t i = 0;
    ആകുമ്പോൾ (i<(PRIMES_LENGTH-1) && PRIMES[i]<size) {
        ++i;
    }

    തിരികെ _uhash_create(താക്കോൽ_Hash, താക്കോൽ_Comp, മൂല്യം_Comp, i, status);
}

U_CAPI UHashtable* U_EXPORT2
uhash_init(UHashtable *fillinResult,
           UHashFunction *താക്കോൽ_Hash,
           UKeyComparator *താക്കോൽ_Comp,
           UValueComparator *മൂല്യം_Comp,
           UErrorCode *status) {

    തിരികെ _uhash_init(fillinResult, താക്കോൽ_Hash, താക്കോൽ_Comp, മൂല്യം_Comp, DEFAULT_അഭാജ്യം_INDEX, status);
}

U_CAPI UHashtable* U_EXPORT2
uhash_initSize(UHashtable *fillinResult,
               UHashFunction *താക്കോൽ_Hash,
               UKeyComparator *താക്കോൽ_Comp,
               UValueComparator *മൂല്യം_Comp,
               int32_t size,
               UErrorCode *status) {

    // Find the smallest index i for which PRIMES[i] >= size.
    int32_t i = 0;
    ആകുമ്പോൾ (i<(PRIMES_LENGTH-1) && PRIMES[i]<size) {
        ++i;
    }
    തിരികെ _uhash_init(fillinResult, താക്കോൽ_Hash, താക്കോൽ_Comp, മൂല്യം_Comp, i, status);
}

U_CAPI ശൂന്യം U_EXPORT2
uhash_close(UHashtable *hash) {
    എങ്കിൽ (hash == nullptr) {
        തിരികെ;
    }
    എങ്കിൽ (hash->elements != nullptr) {
        എങ്കിൽ (hash->താക്കോൽ_Deleter != nullptr || hash->മൂല്യം_Deleter != nullptr) {
            int32_t pos=UHASH_FIRST;
            UHashElement *e;
            ആകുമ്പോൾ ((e = (UHashElement*) uhash_nextElement(hash, &pos)) != nullptr) {
                HASH_DELETE_താക്കോൽ_മൂല്യം(hash, e->താക്കോൽ.pointer, e->value.pointer);
            }
        }
        uprv_free(hash->elements);
        hash->elements = nullptr;
    }
    എങ്കിൽ (hash->allocated) {
        uprv_free(hash);
    }
}

U_CAPI UHashFunction *U_EXPORT2
uhash_set_താക്കോൽ_Hasher(UHashtable *hash, UHashFunction *fn) {
    UHashFunction *result = hash->താക്കോൽ_Hasher;
    hash->താക്കോൽ_Hasher = fn;
    തിരികെ result;
}

U_CAPI UKeyComparator *U_EXPORT2
uhash_set_താക്കോൽ_Comparator(UHashtable *hash, UKeyComparator *fn) {
    UKeyComparator *result = hash->താക്കോൽ_Comparator;
    hash->താക്കോൽ_Comparator = fn;
    തിരികെ result;
}
U_CAPI UValueComparator *U_EXPORT2
uhash_set_മൂല്യം_Comparator(UHashtable *hash, UValueComparator *fn){
    UValueComparator *result = hash->മൂല്യം_Comparator;
    hash->മൂല്യം_Comparator = fn;
    തിരികെ result;
}

U_CAPI UObjectDeleter *U_EXPORT2
uhash_set_താക്കോൽ_Deleter(UHashtable *hash, UObjectDeleter *fn) {
    UObjectDeleter *result = hash->താക്കോൽ_Deleter;
    hash->താക്കോൽ_Deleter = fn;
    തിരികെ result;
}

U_CAPI UObjectDeleter *U_EXPORT2
uhash_set_മൂല്യം_Deleter(UHashtable *hash, UObjectDeleter *fn) {
    UObjectDeleter *result = hash->മൂല്യം_Deleter;
    hash->മൂല്യം_Deleter = fn;
    തിരികെ result;
}

U_CAPI ശൂന്യം U_EXPORT2
uhash_setResizePolicy(UHashtable *hash, എണ്ണൽ UHashResizePolicy policy) {
    UErrorCode status = U_ZERO_ERROR;
    _uhash_internalSetResizePolicy(hash, policy);
    hash->താഴെയുള്ള_Water_Mark  = (int32_t)(hash->length * hash->താഴെയുള്ള_Water_Ratio);
    hash->മുകളിലെ_Water_Mark = (int32_t)(hash->length * hash->മുകളിലെ_Water_Ratio);
    _uhash_rehash(hash, &status);
}

U_CAPI int32_t U_EXPORT2
uhash_എണ്ണം(സ്ഥിരം UHashtable *hash) {
    തിരികെ hash->count;
}

U_CAPI ശൂന്യം* U_EXPORT2
uhash_get(സ്ഥിരം UHashtable *hash,
          സ്ഥിരം ശൂന്യം* താക്കോൽ) {
    UHashTok keyholder;
    keyholder.pointer = (ശൂന്യം*) താക്കോൽ;
    തിരികെ _uhash_find(hash, keyholder, hash->താക്കോൽ_Hasher(keyholder))->value.pointer;
}

U_CAPI ശൂന്യം* U_EXPORT2
uhash_iget(സ്ഥിരം UHashtable *hash,
           int32_t താക്കോൽ) {
    UHashTok keyholder;
    keyholder.integer = താക്കോൽ;
    തിരികെ _uhash_find(hash, keyholder, hash->താക്കോൽ_Hasher(keyholder))->value.pointer;
}

U_CAPI int32_t U_EXPORT2
uhash_geti(സ്ഥിരം UHashtable *hash,
           സ്ഥിരം ശൂന്യം* താക്കോൽ) {
    UHashTok keyholder;
    keyholder.pointer = (ശൂന്യം*) താക്കോൽ;
    തിരികെ _uhash_find(hash, keyholder, hash->താക്കോൽ_Hasher(keyholder))->value.integer;
}

U_CAPI int32_t U_EXPORT2
uhash_igeti(സ്ഥിരം UHashtable *hash,
           int32_t താക്കോൽ) {
    UHashTok keyholder;
    keyholder.integer = താക്കോൽ;
    തിരികെ _uhash_find(hash, keyholder, hash->താക്കോൽ_Hasher(keyholder))->value.integer;
}

U_CAPI int32_t U_EXPORT2
uhash_getiAndFound(സ്ഥിരം UHashtable *hash,
                   സ്ഥിരം ശൂന്യം *താക്കോൽ,
                   UBool *found) {
    UHashTok keyholder;
    keyholder.pointer = (ശൂന്യം *)താക്കോൽ;
    സ്ഥിരം UHashElement *e = _uhash_find(hash, keyholder, hash->താക്കോൽ_Hasher(keyholder));
    *found = !IS_EMPTY_OR_DELETED(e->hashcode);
    തിരികെ e->value.integer;
}

U_CAPI int32_t U_EXPORT2
uhash_igetiAndFound(സ്ഥിരം UHashtable *hash,
                    int32_t താക്കോൽ,
                    UBool *found) {
    UHashTok keyholder;
    keyholder.integer = താക്കോൽ;
    സ്ഥിരം UHashElement *e = _uhash_find(hash, keyholder, hash->താക്കോൽ_Hasher(keyholder));
    *found = !IS_EMPTY_OR_DELETED(e->hashcode);
    തിരികെ e->value.integer;
}

U_CAPI ശൂന്യം* U_EXPORT2
uhash_put(UHashtable *hash,
          ശൂന്യം* താക്കോൽ,
          ശൂന്യം* value,
          UErrorCode *status) {
    UHashTok keyholder, valueholder;
    keyholder.pointer = താക്കോൽ;
    valueholder.pointer = value;
    തിരികെ _uhash_put(hash, keyholder, valueholder,
                      HINT_താക്കോൽ_POINTER | HINT_മൂല്യം_POINTER,
                      status).pointer;
}

U_CAPI ശൂന്യം* U_EXPORT2
uhash_iput(UHashtable *hash,
           int32_t താക്കോൽ,
           ശൂന്യം* value,
           UErrorCode *status) {
    UHashTok keyholder, valueholder;
    keyholder.integer = താക്കോൽ;
    valueholder.pointer = value;
    തിരികെ _uhash_put(hash, keyholder, valueholder,
                      HINT_മൂല്യം_POINTER,
                      status).pointer;
}

U_CAPI int32_t U_EXPORT2
uhash_puti(UHashtable *hash,
           ശൂന്യം* താക്കോൽ,
           int32_t value,
           UErrorCode *status) {
    UHashTok keyholder, valueholder;
    keyholder.pointer = താക്കോൽ;
    valueholder.integer = value;
    തിരികെ _uhash_put(hash, keyholder, valueholder,
                      HINT_താക്കോൽ_POINTER,
                      status).integer;
}


U_CAPI int32_t U_EXPORT2
uhash_iputi(UHashtable *hash,
           int32_t താക്കോൽ,
           int32_t value,
           UErrorCode *status) {
    UHashTok keyholder, valueholder;
    keyholder.integer = താക്കോൽ;
    valueholder.integer = value;
    തിരികെ _uhash_put(hash, keyholder, valueholder,
                      HINT_BOTH_INTEGERS,
                      status).integer;
}

U_CAPI int32_t U_EXPORT2
uhash_putiAllowZero(UHashtable *hash,
                    ശൂന്യം *താക്കോൽ,
                    int32_t value,
                    UErrorCode *status) {
    UHashTok keyholder, valueholder;
    keyholder.pointer = താക്കോൽ;
    valueholder.integer = value;
    തിരികെ _uhash_put(hash, keyholder, valueholder,
                      HINT_താക്കോൽ_POINTER | HINT_ALLOW_ZERO,
                      status).integer;
}


U_CAPI int32_t U_EXPORT2
uhash_iputiAllowZero(UHashtable *hash,
                     int32_t താക്കോൽ,
                     int32_t value,
                     UErrorCode *status) {
    UHashTok keyholder, valueholder;
    keyholder.integer = താക്കോൽ;
    valueholder.integer = value;
    തിരികെ _uhash_put(hash, keyholder, valueholder,
                      HINT_BOTH_INTEGERS | HINT_ALLOW_ZERO,
                      status).integer;
}

U_CAPI ശൂന്യം* U_EXPORT2
uhash_remove(UHashtable *hash,
             സ്ഥിരം ശൂന്യം* താക്കോൽ) {
    UHashTok keyholder;
    keyholder.pointer = (ശൂന്യം*) താക്കോൽ;
    തിരികെ _uhash_remove(hash, keyholder).pointer;
}

U_CAPI ശൂന്യം* U_EXPORT2
uhash_iremove(UHashtable *hash,
              int32_t താക്കോൽ) {
    UHashTok keyholder;
    keyholder.integer = താക്കോൽ;
    തിരികെ _uhash_remove(hash, keyholder).pointer;
}

U_CAPI int32_t U_EXPORT2
uhash_removei(UHashtable *hash,
              സ്ഥിരം ശൂന്യം* താക്കോൽ) {
    UHashTok keyholder;
    keyholder.pointer = (ശൂന്യം*) താക്കോൽ;
    തിരികെ _uhash_remove(hash, keyholder).integer;
}

U_CAPI int32_t U_EXPORT2
uhash_iremovei(UHashtable *hash,
               int32_t താക്കോൽ) {
    UHashTok keyholder;
    keyholder.integer = താക്കോൽ;
    തിരികെ _uhash_remove(hash, keyholder).integer;
}

U_CAPI ശൂന്യം U_EXPORT2
uhash_removeAll(UHashtable *hash) {
    int32_t pos = UHASH_FIRST;
    സ്ഥിരം UHashElement *e;
    U_ASSERT(hash != nullptr);
    എങ്കിൽ (hash->count != 0) {
        ആകുമ്പോൾ ((e = uhash_nextElement(hash, &pos)) != nullptr) {
            uhash_removeElement(hash, e);
        }
    }
    U_ASSERT(hash->count == 0);
}

U_CAPI UBool U_EXPORT2
uhash_contains_താക്കോൽ(സ്ഥിരം UHashtable *hash, സ്ഥിരം ശൂന്യം *താക്കോൽ) {
    UHashTok keyholder;
    keyholder.pointer = (ശൂന്യം *)താക്കോൽ;
    സ്ഥിരം UHashElement *e = _uhash_find(hash, keyholder, hash->താക്കോൽ_Hasher(keyholder));
    തിരികെ !IS_EMPTY_OR_DELETED(e->hashcode);
}

/**
 * Returns true if the UHashtable contains an item with this integer key.
 *
 * @param hash The target UHashtable.
 * @param key An integer key stored in a hashtable
 * @return true if the key is found.
 */
U_CAPI UBool U_EXPORT2
uhash_icontains_താക്കോൽ(സ്ഥിരം UHashtable *hash, int32_t താക്കോൽ) {
    UHashTok keyholder;
    keyholder.integer = താക്കോൽ;
    സ്ഥിരം UHashElement *e = _uhash_find(hash, keyholder, hash->താക്കോൽ_Hasher(keyholder));
    തിരികെ !IS_EMPTY_OR_DELETED(e->hashcode);
}

U_CAPI സ്ഥിരം UHashElement* U_EXPORT2
uhash_find(സ്ഥിരം UHashtable *hash, സ്ഥിരം ശൂന്യം* താക്കോൽ) {
    UHashTok keyholder;
    സ്ഥിരം UHashElement *e;
    keyholder.pointer = (ശൂന്യം*) താക്കോൽ;
    e = _uhash_find(hash, keyholder, hash->താക്കോൽ_Hasher(keyholder));
    തിരികെ IS_EMPTY_OR_DELETED(e->hashcode) ? nullptr : e;
}

U_CAPI സ്ഥിരം UHashElement* U_EXPORT2
uhash_nextElement(സ്ഥിരം UHashtable *hash, int32_t *pos) {
    /* Walk through the array until we find an element that is not
     * EMPTY and not DELETED.
     */
    int32_t i;
    U_ASSERT(hash != nullptr);
    വേണ്ടി (i = *pos + 1; i < hash->length; ++i) {
        എങ്കിൽ (!IS_EMPTY_OR_DELETED(hash->elements[i].hashcode)) {
            *pos = i;
            തിരികെ &(hash->elements[i]);
        }
    }

    /* No more elements */
    തിരികെ nullptr;
}

U_CAPI ശൂന്യം* U_EXPORT2
uhash_removeElement(UHashtable *hash, സ്ഥിരം UHashElement* e) {
    U_ASSERT(hash != nullptr);
    U_ASSERT(e != nullptr);
    എങ്കിൽ (!IS_EMPTY_OR_DELETED(e->hashcode)) {
        UHashElement *nce = (UHashElement *)e;
        തിരികെ _uhash_internalRemoveElement(hash, nce).pointer;
    }
    തിരികെ nullptr;
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
uhash_hashUChars(സ്ഥിരം UHashTok താക്കോൽ) {
    സ്ഥിരം char16_t *s = (സ്ഥിരം char16_t *)താക്കോൽ.pointer;
    തിരികെ s == nullptr ? 0 : ustr_hashUCharsN(s, u_strlen(s));
}

U_CAPI int32_t U_EXPORT2
uhash_hashChars(സ്ഥിരം UHashTok താക്കോൽ) {
    സ്ഥിരം അക്ഷരം *s = (സ്ഥിരം അക്ഷരം *)താക്കോൽ.pointer;
    തിരികെ s == nullptr ? 0 : static_cast<int32_t>(ustr_hashCharsN(s, static_cast<int32_t>(uprv_strlen(s))));
}

U_CAPI int32_t U_EXPORT2
uhash_hashIChars(സ്ഥിരം UHashTok താക്കോൽ) {
    സ്ഥിരം അക്ഷരം *s = (സ്ഥിരം അക്ഷരം *)താക്കോൽ.pointer;
    തിരികെ s == nullptr ? 0 : ustr_hashICharsN(s, static_cast<int32_t>(uprv_strlen(s)));
}

U_CAPI int32_t U_EXPORT2
uhash_hashIStringView(സ്ഥിരം UHashTok താക്കോൽ) {
    സ്ഥിരം std::string_view* s = static_cast<std::string_view*>(താക്കോൽ.pointer);
    തിരികെ s == nullptr ? 0 : ustr_hashICharsN(s->data(), static_cast<int32_t>(s->size()));
}

U_CAPI UBool U_EXPORT2
uhash_equals(സ്ഥിരം UHashtable* hash1, സ്ഥിരം UHashtable* hash2){
    int32_t count1, count2, pos, i;

    എങ്കിൽ(hash1==hash2){
        തിരികെ സത്യം;
    }

    /*
     * Make sure that we are comparing 2 valid hashes of the same type
     * with valid comparison functions.
     * Without valid comparison functions, a binary comparison
     * of the hash values will yield random results on machines
     * with 64-bit pointers and 32-bit integer hashes.
     * A valueComparator is normally optional.
     */
    എങ്കിൽ (hash1==nullptr || hash2==nullptr ||
        hash1->താക്കോൽ_Comparator != hash2->താക്കോൽ_Comparator ||
        hash1->മൂല്യം_Comparator != hash2->മൂല്യം_Comparator ||
        hash1->മൂല്യം_Comparator == nullptr)
    {
        /*
        Normally we would return an error here about incompatible hash tables,
        but we return false instead.
        */
        തിരികെ അസത്യം;
    }

    count1 = uhash_എണ്ണം(hash1);
    count2 = uhash_എണ്ണം(hash2);
    എങ്കിൽ(count1!=count2){
        തിരികെ അസത്യം;
    }

    pos=UHASH_FIRST;
    വേണ്ടി(i=0; i<count1; i++){
        സ്ഥിരം UHashElement* elem1 = uhash_nextElement(hash1, &pos);
        സ്ഥിരം UHashTok key1 = elem1->താക്കോൽ;
        സ്ഥിരം UHashTok val1 = elem1->value;
        /* here the keys are not compared, instead the key form hash1 is used to fetch
         * value from hash2. If the hashes are equal then then both hashes should
         * contain equal values for the same key!
         */
        സ്ഥിരം UHashElement* elem2 = _uhash_find(hash2, key1, hash2->താക്കോൽ_Hasher(key1));
        സ്ഥിരം UHashTok val2 = elem2->value;
        എങ്കിൽ(hash1->മൂല്യം_Comparator(val1, val2)==അസത്യം){
            തിരികെ അസത്യം;
        }
    }
    തിരികെ സത്യം;
}

/********************************************************************
 * PUBLIC Comparator Functions
 ********************************************************************/

U_CAPI UBool U_EXPORT2
uhash_compareUChars(സ്ഥിരം UHashTok key1, സ്ഥിരം UHashTok key2) {
    സ്ഥിരം char16_t *p1 = (സ്ഥിരം char16_t*) key1.pointer;
    സ്ഥിരം char16_t *p2 = (സ്ഥിരം char16_t*) key2.pointer;
    എങ്കിൽ (p1 == p2) {
        തിരികെ സത്യം;
    }
    എങ്കിൽ (p1 == nullptr || p2 == nullptr) {
        തിരികെ അസത്യം;
    }
    ആകുമ്പോൾ (*p1 != 0 && *p1 == *p2) {
        ++p1;
        ++p2;
    }
    തിരികെ *p1 == *p2;
}

U_CAPI UBool U_EXPORT2
uhash_compareChars(സ്ഥിരം UHashTok key1, സ്ഥിരം UHashTok key2) {
    സ്ഥിരം അക്ഷരം *p1 = (സ്ഥിരം അക്ഷരം*) key1.pointer;
    സ്ഥിരം അക്ഷരം *p2 = (സ്ഥിരം അക്ഷരം*) key2.pointer;
    എങ്കിൽ (p1 == p2) {
        തിരികെ സത്യം;
    }
    എങ്കിൽ (p1 == nullptr || p2 == nullptr) {
        തിരികെ അസത്യം;
    }
    ആകുമ്പോൾ (*p1 != 0 && *p1 == *p2) {
        ++p1;
        ++p2;
    }
    തിരികെ *p1 == *p2;
}

U_CAPI UBool U_EXPORT2
uhash_compareIChars(സ്ഥിരം UHashTok key1, സ്ഥിരം UHashTok key2) {
    സ്ഥിരം അക്ഷരം *p1 = (സ്ഥിരം അക്ഷരം*) key1.pointer;
    സ്ഥിരം അക്ഷരം *p2 = (സ്ഥിരം അക്ഷരം*) key2.pointer;
    എങ്കിൽ (p1 == p2) {
        തിരികെ സത്യം;
    }
    എങ്കിൽ (p1 == nullptr || p2 == nullptr) {
        തിരികെ അസത്യം;
    }
    ആകുമ്പോൾ (*p1 != 0 && uprv_tolower(*p1) == uprv_tolower(*p2)) {
        ++p1;
        ++p2;
    }
    തിരികെ *p1 == *p2;
}

U_CAPI UBool U_EXPORT2
uhash_compareIStringView(സ്ഥിരം UHashTok key1, സ്ഥിരം UHashTok key2) {
    സ്ഥിരം std::string_view* p1 = static_cast<std::string_view*>(key1.pointer);
    സ്ഥിരം std::string_view* p2 = static_cast<std::string_view*>(key2.pointer);
    എങ്കിൽ (p1 == p2) {
        തിരികെ സത്യം;
    }
    എങ്കിൽ (p1 == nullptr || p2 == nullptr) {
        തിരികെ അസത്യം;
    }
    സ്ഥിരം std::string_view& v1 = *p1;
    സ്ഥിരം std::string_view& v2 = *p2;
    എങ്കിൽ (v1.size() != v2.size()) {
        തിരികെ അസത്യം;
    }
    വേണ്ടി (size_t i = 0; i < v1.size(); ++i) {
        എങ്കിൽ (uprv_tolower(v1[i]) != uprv_tolower(v2[i])) {
            തിരികെ അസത്യം;
        }
    }
    തിരികെ സത്യം;
}

/********************************************************************
 * PUBLIC int32_t Support Functions
 ********************************************************************/

U_CAPI int32_t U_EXPORT2
uhash_hashLong(സ്ഥിരം UHashTok താക്കോൽ) {
    തിരികെ താക്കോൽ.integer;
}

U_CAPI UBool U_EXPORT2
uhash_compareLong(സ്ഥിരം UHashTok key1, സ്ഥിരം UHashTok key2) {
    തിരികെ key1.integer == key2.integer;
}
