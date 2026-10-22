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

#அந்தர்பாவ <string_view>

#அந்தர்பாவ "uhash.h"
#அந்தர்பாவ "unicode/ustring.h"
#அந்தர்பாவ "cstring.h"
#அந்தர்பாவ "cmemory.h"
#அந்தர்பாவ "uassert.h"
#அந்தர்பாவ "ustr_imp.h"

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
நிலையான மாறிலி int32_t PRIMES[] = {
    7, 13, 31, 61, 127, 251, 509, 1021, 2039, 4093, 8191, 16381, 32749,
    65521, 131071, 262139, 524287, 1048573, 2097143, 4194301, 8388593,
    16777213, 33554393, 67108859, 134217689, 268435399, 536870909,
    1073741789, 2147483647 /*, 4294967291 */
};

#define PRIMES_LENGTH UPRV_LENGTHOF(PRIMES)
#define DEFAULT_பகாஎண்_INDEX 4

/* These ratios are tuned to the PRIMES array such that a resize
 * places the table back into the zone of non-resizing.  That is,
 * after a call to _uhash_rehash(), a subsequent call to
 * _uhash_rehash() should do nothing (should not churn).  This is only
 * a potential problem with U_GROW_AND_SHRINK.
 */
நிலையான மாறிலி தசம RESIZE_POLICY_RATIO_TABLE[6] = {
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
#define HASH_DELETE_சாவி_மதிப்பு(hash, keypointer, valuepointer) UPRV_BLOCK_MACRO_BEGIN { \
    எனில் (hash->சாவி_Deleter != nullptr && keypointer != nullptr) { \
        (*hash->சாவி_Deleter)(keypointer); \
    } \
    எனில் (hash->மதிப்பு_Deleter != nullptr && valuepointer != nullptr) { \
        (*hash->மதிப்பு_Deleter)(valuepointer); \
    } \
} UPRV_BLOCK_MACRO_END

/*
 * Constants for hinting whether a key or value is an integer
 * or a pointer.  If a hint bit is zero, then the associated
 * token is assumed to be an integer.
 */
#define HINT_BOTH_INTEGERS (0)
#define HINT_சாவி_POINTER   (1)
#define HINT_மதிப்பு_POINTER (2)
#define HINT_ALLOW_ZERO    (4)

/********************************************************************
 * PRIVATE Implementation
 ********************************************************************/

நிலையான UHashTok
_uhash_setElement(UHashtable *hash, UHashElement* e,
                  int32_t hashcode,
                  UHashTok சாவி, UHashTok value, int8_t hint) {

    UHashTok old_மதிப்பு = e->value;
    எனில் (hash->சாவி_Deleter != nullptr && e->சாவி.pointer != nullptr &&
        e->சாவி.pointer != சாவி.pointer) { /* Avoid double deletion */
        (*hash->சாவி_Deleter)(e->சாவி.pointer);
    }
    எனில் (hash->மதிப்பு_Deleter != nullptr) {
        எனில் (old_மதிப்பு.pointer != nullptr &&
            old_மதிப்பு.pointer != value.pointer) { /* Avoid double deletion */
            (*hash->மதிப்பு_Deleter)(old_மதிப்பு.pointer);
        }
        old_மதிப்பு.pointer = nullptr;
    }
    /* Compilers should copy the UHashTok union correctly, but even if
     * they do, memory heap tools (e.g. BoundsChecker) can get
     * confused when a pointer is cloaked in a union and then copied.
     * TO ALLEVIATE THIS, we use hints (based on what API the user is
     * calling) to copy pointers when we know the user thinks
     * something is a pointer. */
    எனில் (hint & HINT_சாவி_POINTER) {
        e->சாவி.pointer = சாவி.pointer;
    } இல்லையெனில் {
        e->சாவி = சாவி;
    }
    எனில் (hint & HINT_மதிப்பு_POINTER) {
        e->value.pointer = value.pointer;
    } இல்லையெனில் {
        e->value = value;
    }
    e->hashcode = hashcode;
    திரும்பு old_மதிப்பு;
}

/**
 * Assumes that the given element is not empty or deleted.
 */
நிலையான UHashTok
_uhash_internalRemoveElement(UHashtable *hash, UHashElement* e) {
    UHashTok empty;
    U_ASSERT(!IS_EMPTY_OR_DELETED(e->hashcode));
    --hash->count;
    empty.pointer = nullptr; empty.integer = 0;
    திரும்பு _uhash_setElement(hash, e, HASH_DELETED, empty, empty, 0);
}

நிலையான வெறுமை
_uhash_internalSetResizePolicy(UHashtable *hash, எண்ணிக்கை UHashResizePolicy policy) {
    U_ASSERT(hash != nullptr);
    U_ASSERT(((int32_t)policy) >= 0);
    U_ASSERT(((int32_t)policy) < 3);
    hash->கீழ்_Water_Ratio  = RESIZE_POLICY_RATIO_TABLE[policy * 2];
    hash->உயர்வு_Water_Ratio = RESIZE_POLICY_RATIO_TABLE[policy * 2 + 1];
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
நிலையான வெறுமை
_uhash_allocate(UHashtable *hash,
                int32_t பகாஎண்_Index,
                UErrorCode *status) {

    UHashElement *p, *limit;
    UHashTok emptytok;

    எனில் (U_FAILURE(*status)) திரும்பு;

    U_ASSERT(பகாஎண்_Index >= 0 && பகாஎண்_Index < PRIMES_LENGTH);

    hash->பகாஎண்_Index = static_cast<int8_t>(பகாஎண்_Index);
    hash->length = PRIMES[பகாஎண்_Index];

    p = hash->elements = static_cast<UHashElement*>(
        uprv_malloc(அளவு(UHashElement) * hash->length));

    எனில் (hash->elements == nullptr) {
        *status = U_MEMORY_ALLOCATION_ERROR;
        திரும்பு;
    }

    emptytok.pointer = nullptr; /* Only one of these two is needed */
    emptytok.integer = 0;    /* but we don't know which one. */

    limit = p + hash->length;
    போது (p < limit) {
        p->சாவி = emptytok;
        p->value = emptytok;
        p->hashcode = HASH_EMPTY;
        ++p;
    }

    hash->count = 0;
    hash->கீழ்_Water_Mark = static_cast<int32_t>(hash->length * hash->கீழ்_Water_Ratio);
    hash->உயர்வு_Water_Mark = static_cast<int32_t>(hash->length * hash->உயர்வு_Water_Ratio);
}

நிலையான UHashtable*
_uhash_init(UHashtable *result,
              UHashFunction *சாவி_Hash,
              UKeyComparator *சாவி_Comp,
              UValueComparator *மதிப்பு_Comp,
              int32_t பகாஎண்_Index,
              UErrorCode *status)
{
    எனில் (U_FAILURE(*status)) திரும்பு nullptr;
    U_ASSERT(சாவி_Hash != nullptr);
    U_ASSERT(சாவி_Comp != nullptr);

    result->சாவி_Hasher       = சாவி_Hash;
    result->சாவி_Comparator   = சாவி_Comp;
    result->மதிப்பு_Comparator = மதிப்பு_Comp;
    result->சாவி_Deleter      = nullptr;
    result->மதிப்பு_Deleter    = nullptr;
    result->allocated       = பொய்;
    _uhash_internalSetResizePolicy(result, U_GROW);

    _uhash_allocate(result, பகாஎண்_Index, status);

    எனில் (U_FAILURE(*status)) {
        திரும்பு nullptr;
    }

    திரும்பு result;
}

நிலையான UHashtable*
_uhash_create(UHashFunction *சாவி_Hash,
              UKeyComparator *சாவி_Comp,
              UValueComparator *மதிப்பு_Comp,
              int32_t பகாஎண்_Index,
              UErrorCode *status) {
    UHashtable *result;

    எனில் (U_FAILURE(*status)) திரும்பு nullptr;

    result = static_cast<UHashtable*>(uprv_malloc(அளவு(UHashtable)));
    எனில் (result == nullptr) {
        *status = U_MEMORY_ALLOCATION_ERROR;
        திரும்பு nullptr;
    }

    _uhash_init(result, சாவி_Hash, சாவி_Comp, மதிப்பு_Comp, பகாஎண்_Index, status);
    result->allocated       = உண்மை;

    எனில் (U_FAILURE(*status)) {
        uprv_free(result);
        திரும்பு nullptr;
    }

    திரும்பு result;
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
நிலையான UHashElement*
_uhash_find(மாறிலி UHashtable *hash, UHashTok சாவி,
            int32_t hashcode) {

    int32_t firstDeleted = -1;  /* assume invalid index */
    int32_t theIndex, startIndex;
    int32_t jump = 0; /* lazy evaluate */
    int32_t tableHash;
    UHashElement *elements = hash->elements;

    hashcode &= 0x7FFFFFFF; /* must be positive */
    startIndex = theIndex = (hashcode ^ 0x4000000) % hash->length;

    செய் {
        tableHash = elements[theIndex].hashcode;
        எனில் (tableHash == hashcode) {          /* quick check */
            எனில் ((*hash->சாவி_Comparator)(சாவி, elements[theIndex].சாவி)) {
                திரும்பு &(elements[theIndex]);
            }
        } இல்லையெனில் எனில் (!IS_EMPTY_OR_DELETED(tableHash)) {
            /* We have hit a slot which contains a key-value pair,
             * but for which the hash code does not match.  Keep
             * looking.
             */
        } இல்லையெனில் எனில் (tableHash == HASH_EMPTY) { /* empty, end o' the line */
            நிறுத்து;
        } இல்லையெனில் எனில் (firstDeleted < 0) { /* remember first deleted */
            firstDeleted = theIndex;
        }
        எனில் (jump == 0) { /* lazy compute jump */
            /* The jump value must be relatively prime to the table
             * length.  As long as the length is prime, then any value
             * 1..length-1 will be relatively prime to it.
             */
            jump = (hashcode % (hash->length - 1)) + 1;
        }
        theIndex = (theIndex + jump) % hash->length;
    } போது (theIndex != startIndex);

    எனில் (firstDeleted >= 0) {
        theIndex = firstDeleted; /* reset if had deleted slot */
    } இல்லையெனில் எனில் (tableHash != HASH_EMPTY) {
        /* We get to this point if the hashtable is full (no empty or
         * deleted slots), and we've failed to find a match.  THIS
         * WILL NEVER HAPPEN as long as uhash_put() makes sure that
         * count is always < length.
         */
        UPRV_UNREACHABLE_EXIT;
    }
    திரும்பு &(elements[theIndex]);
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
நிலையான வெறுமை
_uhash_rehash(UHashtable *hash, UErrorCode *status) {

    UHashElement *old = hash->elements;
    int32_t oldLength = hash->length;
    int32_t new_பகாஎண்_Index = hash->பகாஎண்_Index;
    int32_t i;

    எனில் (hash->count > hash->உயர்வு_Water_Mark) {
        எனில் (++new_பகாஎண்_Index >= PRIMES_LENGTH) {
            திரும்பு;
        }
    } இல்லையெனில் எனில் (hash->count < hash->கீழ்_Water_Mark) {
        எனில் (--new_பகாஎண்_Index < 0) {
            திரும்பு;
        }
    } இல்லையெனில் {
        திரும்பு;
    }

    _uhash_allocate(hash, new_பகாஎண்_Index, status);

    எனில் (U_FAILURE(*status)) {
        hash->elements = old;
        hash->length = oldLength;
        திரும்பு;
    }

    க்கு (i = oldLength - 1; i >= 0; --i) {
        எனில் (!IS_EMPTY_OR_DELETED(old[i].hashcode)) {
            UHashElement *e = _uhash_find(hash, old[i].சாவி, old[i].hashcode);
            U_ASSERT(e != nullptr);
            U_ASSERT(e->hashcode == HASH_EMPTY);
            e->சாவி = old[i].சாவி;
            e->value = old[i].value;
            e->hashcode = old[i].hashcode;
            ++hash->count;
        }
    }

    uprv_free(old);
}

நிலையான UHashTok
_uhash_remove(UHashtable *hash,
              UHashTok சாவி) {
    /* First find the position of the key in the table.  If the object
     * has not been removed already, remove it.  If the user wanted
     * keys deleted, then delete it also.  We have to put a special
     * hashcode in that position that means that something has been
     * deleted, since when we do a find, we have to continue PAST any
     * deleted values.
     */
    UHashTok result;
    UHashElement* e = _uhash_find(hash, சாவி, hash->சாவி_Hasher(சாவி));
    U_ASSERT(e != nullptr);
    result.pointer = nullptr;
    result.integer = 0;
    எனில் (!IS_EMPTY_OR_DELETED(e->hashcode)) {
        result = _uhash_internalRemoveElement(hash, e);
        எனில் (hash->count < hash->கீழ்_Water_Mark) {
            UErrorCode status = U_ZERO_ERROR;
            _uhash_rehash(hash, &status);
        }
    }
    திரும்பு result;
}

நிலையான UHashTok
_uhash_put(UHashtable *hash,
           UHashTok சாவி,
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

    எனில் (U_FAILURE(*status)) {
        செல் err;
    }
    U_ASSERT(hash != nullptr);
    எனில் ((hint & HINT_மதிப்பு_POINTER) ?
            value.pointer == nullptr :
            value.integer == 0 && (hint & HINT_ALLOW_ZERO) == 0) {
        /* Disallow storage of nullptr values, since nullptr is returned by
         * get() to indicate an absent key.  Storing nullptr == removing.
         */
        திரும்பு _uhash_remove(hash, சாவி);
    }
    எனில் (hash->count > hash->உயர்வு_Water_Mark) {
        _uhash_rehash(hash, status);
        எனில் (U_FAILURE(*status)) {
            செல் err;
        }
    }

    hashcode = (*hash->சாவி_Hasher)(சாவி);
    e = _uhash_find(hash, சாவி, hashcode);
    U_ASSERT(e != nullptr);

    எனில் (IS_EMPTY_OR_DELETED(e->hashcode)) {
        /* Important: We must never actually fill the table up.  If we
         * do so, then _uhash_find() will return nullptr, and we'll have
         * to check for nullptr after every call to _uhash_find().  To
         * avoid this we make sure there is always at least one empty
         * or deleted slot in the table.  This only is a problem if we
         * are out of memory and rehash isn't working.
         */
        ++hash->count;
        எனில் (hash->count == hash->length) {
            /* Don't allow count to reach length */
            --hash->count;
            *status = U_MEMORY_ALLOCATION_ERROR;
            செல் err;
        }
    }

    /* We must in all cases handle storage properly.  If there was an
     * old key, then it must be deleted (if the deleter != nullptr).
     * Make hashcodes stored in table positive.
     */
    திரும்பு _uhash_setElement(hash, e, hashcode & 0x7FFFFFFF, சாவி, value, hint);

 err:
    /* If the deleters are non-nullptr, this method adopts its key and/or
     * value arguments, and we must be sure to delete the key and/or
     * value in all cases, even upon failure.
     */
    HASH_DELETE_சாவி_மதிப்பு(hash, சாவி.pointer, value.pointer);
    emptytok.pointer = nullptr; emptytok.integer = 0;
    திரும்பு emptytok;
}


/********************************************************************
 * PUBLIC API
 ********************************************************************/

U_CAPI UHashtable* U_EXPORT2
uhash_open(UHashFunction *சாவி_Hash,
           UKeyComparator *சாவி_Comp,
           UValueComparator *மதிப்பு_Comp,
           UErrorCode *status) {

    திரும்பு _uhash_create(சாவி_Hash, சாவி_Comp, மதிப்பு_Comp, DEFAULT_பகாஎண்_INDEX, status);
}

U_CAPI UHashtable* U_EXPORT2
uhash_openSize(UHashFunction *சாவி_Hash,
               UKeyComparator *சாவி_Comp,
               UValueComparator *மதிப்பு_Comp,
               int32_t size,
               UErrorCode *status) {

    /* Find the smallest index i for which PRIMES[i] >= size. */
    int32_t i = 0;
    போது (i<(PRIMES_LENGTH-1) && PRIMES[i]<size) {
        ++i;
    }

    திரும்பு _uhash_create(சாவி_Hash, சாவி_Comp, மதிப்பு_Comp, i, status);
}

U_CAPI UHashtable* U_EXPORT2
uhash_init(UHashtable *fillinResult,
           UHashFunction *சாவி_Hash,
           UKeyComparator *சாவி_Comp,
           UValueComparator *மதிப்பு_Comp,
           UErrorCode *status) {

    திரும்பு _uhash_init(fillinResult, சாவி_Hash, சாவி_Comp, மதிப்பு_Comp, DEFAULT_பகாஎண்_INDEX, status);
}

U_CAPI UHashtable* U_EXPORT2
uhash_initSize(UHashtable *fillinResult,
               UHashFunction *சாவி_Hash,
               UKeyComparator *சாவி_Comp,
               UValueComparator *மதிப்பு_Comp,
               int32_t size,
               UErrorCode *status) {

    // Find the smallest index i for which PRIMES[i] >= size.
    int32_t i = 0;
    போது (i<(PRIMES_LENGTH-1) && PRIMES[i]<size) {
        ++i;
    }
    திரும்பு _uhash_init(fillinResult, சாவி_Hash, சாவி_Comp, மதிப்பு_Comp, i, status);
}

U_CAPI வெறுமை U_EXPORT2
uhash_close(UHashtable *hash) {
    எனில் (hash == nullptr) {
        திரும்பு;
    }
    எனில் (hash->elements != nullptr) {
        எனில் (hash->சாவி_Deleter != nullptr || hash->மதிப்பு_Deleter != nullptr) {
            int32_t pos=UHASH_FIRST;
            UHashElement *e;
            போது ((e = (UHashElement*) uhash_nextElement(hash, &pos)) != nullptr) {
                HASH_DELETE_சாவி_மதிப்பு(hash, e->சாவி.pointer, e->value.pointer);
            }
        }
        uprv_free(hash->elements);
        hash->elements = nullptr;
    }
    எனில் (hash->allocated) {
        uprv_free(hash);
    }
}

U_CAPI UHashFunction *U_EXPORT2
uhash_set_சாவி_Hasher(UHashtable *hash, UHashFunction *fn) {
    UHashFunction *result = hash->சாவி_Hasher;
    hash->சாவி_Hasher = fn;
    திரும்பு result;
}

U_CAPI UKeyComparator *U_EXPORT2
uhash_set_சாவி_Comparator(UHashtable *hash, UKeyComparator *fn) {
    UKeyComparator *result = hash->சாவி_Comparator;
    hash->சாவி_Comparator = fn;
    திரும்பு result;
}
U_CAPI UValueComparator *U_EXPORT2
uhash_set_மதிப்பு_Comparator(UHashtable *hash, UValueComparator *fn){
    UValueComparator *result = hash->மதிப்பு_Comparator;
    hash->மதிப்பு_Comparator = fn;
    திரும்பு result;
}

U_CAPI UObjectDeleter *U_EXPORT2
uhash_set_சாவி_Deleter(UHashtable *hash, UObjectDeleter *fn) {
    UObjectDeleter *result = hash->சாவி_Deleter;
    hash->சாவி_Deleter = fn;
    திரும்பு result;
}

U_CAPI UObjectDeleter *U_EXPORT2
uhash_set_மதிப்பு_Deleter(UHashtable *hash, UObjectDeleter *fn) {
    UObjectDeleter *result = hash->மதிப்பு_Deleter;
    hash->மதிப்பு_Deleter = fn;
    திரும்பு result;
}

U_CAPI வெறுமை U_EXPORT2
uhash_setResizePolicy(UHashtable *hash, எண்ணிக்கை UHashResizePolicy policy) {
    UErrorCode status = U_ZERO_ERROR;
    _uhash_internalSetResizePolicy(hash, policy);
    hash->கீழ்_Water_Mark  = (int32_t)(hash->length * hash->கீழ்_Water_Ratio);
    hash->உயர்வு_Water_Mark = (int32_t)(hash->length * hash->உயர்வு_Water_Ratio);
    _uhash_rehash(hash, &status);
}

U_CAPI int32_t U_EXPORT2
uhash_மொத்தம்(மாறிலி UHashtable *hash) {
    திரும்பு hash->count;
}

U_CAPI வெறுமை* U_EXPORT2
uhash_get(மாறிலி UHashtable *hash,
          மாறிலி வெறுமை* சாவி) {
    UHashTok keyholder;
    keyholder.pointer = (வெறுமை*) சாவி;
    திரும்பு _uhash_find(hash, keyholder, hash->சாவி_Hasher(keyholder))->value.pointer;
}

U_CAPI வெறுமை* U_EXPORT2
uhash_iget(மாறிலி UHashtable *hash,
           int32_t சாவி) {
    UHashTok keyholder;
    keyholder.integer = சாவி;
    திரும்பு _uhash_find(hash, keyholder, hash->சாவி_Hasher(keyholder))->value.pointer;
}

U_CAPI int32_t U_EXPORT2
uhash_geti(மாறிலி UHashtable *hash,
           மாறிலி வெறுமை* சாவி) {
    UHashTok keyholder;
    keyholder.pointer = (வெறுமை*) சாவி;
    திரும்பு _uhash_find(hash, keyholder, hash->சாவி_Hasher(keyholder))->value.integer;
}

U_CAPI int32_t U_EXPORT2
uhash_igeti(மாறிலி UHashtable *hash,
           int32_t சாவி) {
    UHashTok keyholder;
    keyholder.integer = சாவி;
    திரும்பு _uhash_find(hash, keyholder, hash->சாவி_Hasher(keyholder))->value.integer;
}

U_CAPI int32_t U_EXPORT2
uhash_getiAndFound(மாறிலி UHashtable *hash,
                   மாறிலி வெறுமை *சாவி,
                   UBool *found) {
    UHashTok keyholder;
    keyholder.pointer = (வெறுமை *)சாவி;
    மாறிலி UHashElement *e = _uhash_find(hash, keyholder, hash->சாவி_Hasher(keyholder));
    *found = !IS_EMPTY_OR_DELETED(e->hashcode);
    திரும்பு e->value.integer;
}

U_CAPI int32_t U_EXPORT2
uhash_igetiAndFound(மாறிலி UHashtable *hash,
                    int32_t சாவி,
                    UBool *found) {
    UHashTok keyholder;
    keyholder.integer = சாவி;
    மாறிலி UHashElement *e = _uhash_find(hash, keyholder, hash->சாவி_Hasher(keyholder));
    *found = !IS_EMPTY_OR_DELETED(e->hashcode);
    திரும்பு e->value.integer;
}

U_CAPI வெறுமை* U_EXPORT2
uhash_put(UHashtable *hash,
          வெறுமை* சாவி,
          வெறுமை* value,
          UErrorCode *status) {
    UHashTok keyholder, valueholder;
    keyholder.pointer = சாவி;
    valueholder.pointer = value;
    திரும்பு _uhash_put(hash, keyholder, valueholder,
                      HINT_சாவி_POINTER | HINT_மதிப்பு_POINTER,
                      status).pointer;
}

U_CAPI வெறுமை* U_EXPORT2
uhash_iput(UHashtable *hash,
           int32_t சாவி,
           வெறுமை* value,
           UErrorCode *status) {
    UHashTok keyholder, valueholder;
    keyholder.integer = சாவி;
    valueholder.pointer = value;
    திரும்பு _uhash_put(hash, keyholder, valueholder,
                      HINT_மதிப்பு_POINTER,
                      status).pointer;
}

U_CAPI int32_t U_EXPORT2
uhash_puti(UHashtable *hash,
           வெறுமை* சாவி,
           int32_t value,
           UErrorCode *status) {
    UHashTok keyholder, valueholder;
    keyholder.pointer = சாவி;
    valueholder.integer = value;
    திரும்பு _uhash_put(hash, keyholder, valueholder,
                      HINT_சாவி_POINTER,
                      status).integer;
}


U_CAPI int32_t U_EXPORT2
uhash_iputi(UHashtable *hash,
           int32_t சாவி,
           int32_t value,
           UErrorCode *status) {
    UHashTok keyholder, valueholder;
    keyholder.integer = சாவி;
    valueholder.integer = value;
    திரும்பு _uhash_put(hash, keyholder, valueholder,
                      HINT_BOTH_INTEGERS,
                      status).integer;
}

U_CAPI int32_t U_EXPORT2
uhash_putiAllowZero(UHashtable *hash,
                    வெறுமை *சாவி,
                    int32_t value,
                    UErrorCode *status) {
    UHashTok keyholder, valueholder;
    keyholder.pointer = சாவி;
    valueholder.integer = value;
    திரும்பு _uhash_put(hash, keyholder, valueholder,
                      HINT_சாவி_POINTER | HINT_ALLOW_ZERO,
                      status).integer;
}


U_CAPI int32_t U_EXPORT2
uhash_iputiAllowZero(UHashtable *hash,
                     int32_t சாவி,
                     int32_t value,
                     UErrorCode *status) {
    UHashTok keyholder, valueholder;
    keyholder.integer = சாவி;
    valueholder.integer = value;
    திரும்பு _uhash_put(hash, keyholder, valueholder,
                      HINT_BOTH_INTEGERS | HINT_ALLOW_ZERO,
                      status).integer;
}

U_CAPI வெறுமை* U_EXPORT2
uhash_remove(UHashtable *hash,
             மாறிலி வெறுமை* சாவி) {
    UHashTok keyholder;
    keyholder.pointer = (வெறுமை*) சாவி;
    திரும்பு _uhash_remove(hash, keyholder).pointer;
}

U_CAPI வெறுமை* U_EXPORT2
uhash_iremove(UHashtable *hash,
              int32_t சாவி) {
    UHashTok keyholder;
    keyholder.integer = சாவி;
    திரும்பு _uhash_remove(hash, keyholder).pointer;
}

U_CAPI int32_t U_EXPORT2
uhash_removei(UHashtable *hash,
              மாறிலி வெறுமை* சாவி) {
    UHashTok keyholder;
    keyholder.pointer = (வெறுமை*) சாவி;
    திரும்பு _uhash_remove(hash, keyholder).integer;
}

U_CAPI int32_t U_EXPORT2
uhash_iremovei(UHashtable *hash,
               int32_t சாவி) {
    UHashTok keyholder;
    keyholder.integer = சாவி;
    திரும்பு _uhash_remove(hash, keyholder).integer;
}

U_CAPI வெறுமை U_EXPORT2
uhash_removeAll(UHashtable *hash) {
    int32_t pos = UHASH_FIRST;
    மாறிலி UHashElement *e;
    U_ASSERT(hash != nullptr);
    எனில் (hash->count != 0) {
        போது ((e = uhash_nextElement(hash, &pos)) != nullptr) {
            uhash_removeElement(hash, e);
        }
    }
    U_ASSERT(hash->count == 0);
}

U_CAPI UBool U_EXPORT2
uhash_contains_சாவி(மாறிலி UHashtable *hash, மாறிலி வெறுமை *சாவி) {
    UHashTok keyholder;
    keyholder.pointer = (வெறுமை *)சாவி;
    மாறிலி UHashElement *e = _uhash_find(hash, keyholder, hash->சாவி_Hasher(keyholder));
    திரும்பு !IS_EMPTY_OR_DELETED(e->hashcode);
}

/**
 * Returns true if the UHashtable contains an item with this integer key.
 *
 * @param hash The target UHashtable.
 * @param key An integer key stored in a hashtable
 * @return true if the key is found.
 */
U_CAPI UBool U_EXPORT2
uhash_icontains_சாவி(மாறிலி UHashtable *hash, int32_t சாவி) {
    UHashTok keyholder;
    keyholder.integer = சாவி;
    மாறிலி UHashElement *e = _uhash_find(hash, keyholder, hash->சாவி_Hasher(keyholder));
    திரும்பு !IS_EMPTY_OR_DELETED(e->hashcode);
}

U_CAPI மாறிலி UHashElement* U_EXPORT2
uhash_find(மாறிலி UHashtable *hash, மாறிலி வெறுமை* சாவி) {
    UHashTok keyholder;
    மாறிலி UHashElement *e;
    keyholder.pointer = (வெறுமை*) சாவி;
    e = _uhash_find(hash, keyholder, hash->சாவி_Hasher(keyholder));
    திரும்பு IS_EMPTY_OR_DELETED(e->hashcode) ? nullptr : e;
}

U_CAPI மாறிலி UHashElement* U_EXPORT2
uhash_nextElement(மாறிலி UHashtable *hash, int32_t *pos) {
    /* Walk through the array until we find an element that is not
     * EMPTY and not DELETED.
     */
    int32_t i;
    U_ASSERT(hash != nullptr);
    க்கு (i = *pos + 1; i < hash->length; ++i) {
        எனில் (!IS_EMPTY_OR_DELETED(hash->elements[i].hashcode)) {
            *pos = i;
            திரும்பு &(hash->elements[i]);
        }
    }

    /* No more elements */
    திரும்பு nullptr;
}

U_CAPI வெறுமை* U_EXPORT2
uhash_removeElement(UHashtable *hash, மாறிலி UHashElement* e) {
    U_ASSERT(hash != nullptr);
    U_ASSERT(e != nullptr);
    எனில் (!IS_EMPTY_OR_DELETED(e->hashcode)) {
        UHashElement *nce = (UHashElement *)e;
        திரும்பு _uhash_internalRemoveElement(hash, nce).pointer;
    }
    திரும்பு nullptr;
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
uhash_hashUChars(மாறிலி UHashTok சாவி) {
    மாறிலி char16_t *s = (மாறிலி char16_t *)சாவி.pointer;
    திரும்பு s == nullptr ? 0 : ustr_hashUCharsN(s, u_strlen(s));
}

U_CAPI int32_t U_EXPORT2
uhash_hashChars(மாறிலி UHashTok சாவி) {
    மாறிலி எழுத்து *s = (மாறிலி எழுத்து *)சாவி.pointer;
    திரும்பு s == nullptr ? 0 : static_cast<int32_t>(ustr_hashCharsN(s, static_cast<int32_t>(uprv_strlen(s))));
}

U_CAPI int32_t U_EXPORT2
uhash_hashIChars(மாறிலி UHashTok சாவி) {
    மாறிலி எழுத்து *s = (மாறிலி எழுத்து *)சாவி.pointer;
    திரும்பு s == nullptr ? 0 : ustr_hashICharsN(s, static_cast<int32_t>(uprv_strlen(s)));
}

U_CAPI int32_t U_EXPORT2
uhash_hashIStringView(மாறிலி UHashTok சாவி) {
    மாறிலி std::string_view* s = static_cast<std::string_view*>(சாவி.pointer);
    திரும்பு s == nullptr ? 0 : ustr_hashICharsN(s->data(), static_cast<int32_t>(s->size()));
}

U_CAPI UBool U_EXPORT2
uhash_equals(மாறிலி UHashtable* hash1, மாறிலி UHashtable* hash2){
    int32_t count1, count2, pos, i;

    எனில்(hash1==hash2){
        திரும்பு உண்மை;
    }

    /*
     * Make sure that we are comparing 2 valid hashes of the same type
     * with valid comparison functions.
     * Without valid comparison functions, a binary comparison
     * of the hash values will yield random results on machines
     * with 64-bit pointers and 32-bit integer hashes.
     * A valueComparator is normally optional.
     */
    எனில் (hash1==nullptr || hash2==nullptr ||
        hash1->சாவி_Comparator != hash2->சாவி_Comparator ||
        hash1->மதிப்பு_Comparator != hash2->மதிப்பு_Comparator ||
        hash1->மதிப்பு_Comparator == nullptr)
    {
        /*
        Normally we would return an error here about incompatible hash tables,
        but we return false instead.
        */
        திரும்பு பொய்;
    }

    count1 = uhash_மொத்தம்(hash1);
    count2 = uhash_மொத்தம்(hash2);
    எனில்(count1!=count2){
        திரும்பு பொய்;
    }

    pos=UHASH_FIRST;
    க்கு(i=0; i<count1; i++){
        மாறிலி UHashElement* elem1 = uhash_nextElement(hash1, &pos);
        மாறிலி UHashTok key1 = elem1->சாவி;
        மாறிலி UHashTok val1 = elem1->value;
        /* here the keys are not compared, instead the key form hash1 is used to fetch
         * value from hash2. If the hashes are equal then then both hashes should
         * contain equal values for the same key!
         */
        மாறிலி UHashElement* elem2 = _uhash_find(hash2, key1, hash2->சாவி_Hasher(key1));
        மாறிலி UHashTok val2 = elem2->value;
        எனில்(hash1->மதிப்பு_Comparator(val1, val2)==பொய்){
            திரும்பு பொய்;
        }
    }
    திரும்பு உண்மை;
}

/********************************************************************
 * PUBLIC Comparator Functions
 ********************************************************************/

U_CAPI UBool U_EXPORT2
uhash_compareUChars(மாறிலி UHashTok key1, மாறிலி UHashTok key2) {
    மாறிலி char16_t *p1 = (மாறிலி char16_t*) key1.pointer;
    மாறிலி char16_t *p2 = (மாறிலி char16_t*) key2.pointer;
    எனில் (p1 == p2) {
        திரும்பு உண்மை;
    }
    எனில் (p1 == nullptr || p2 == nullptr) {
        திரும்பு பொய்;
    }
    போது (*p1 != 0 && *p1 == *p2) {
        ++p1;
        ++p2;
    }
    திரும்பு *p1 == *p2;
}

U_CAPI UBool U_EXPORT2
uhash_compareChars(மாறிலி UHashTok key1, மாறிலி UHashTok key2) {
    மாறிலி எழுத்து *p1 = (மாறிலி எழுத்து*) key1.pointer;
    மாறிலி எழுத்து *p2 = (மாறிலி எழுத்து*) key2.pointer;
    எனில் (p1 == p2) {
        திரும்பு உண்மை;
    }
    எனில் (p1 == nullptr || p2 == nullptr) {
        திரும்பு பொய்;
    }
    போது (*p1 != 0 && *p1 == *p2) {
        ++p1;
        ++p2;
    }
    திரும்பு *p1 == *p2;
}

U_CAPI UBool U_EXPORT2
uhash_compareIChars(மாறிலி UHashTok key1, மாறிலி UHashTok key2) {
    மாறிலி எழுத்து *p1 = (மாறிலி எழுத்து*) key1.pointer;
    மாறிலி எழுத்து *p2 = (மாறிலி எழுத்து*) key2.pointer;
    எனில் (p1 == p2) {
        திரும்பு உண்மை;
    }
    எனில் (p1 == nullptr || p2 == nullptr) {
        திரும்பு பொய்;
    }
    போது (*p1 != 0 && uprv_tolower(*p1) == uprv_tolower(*p2)) {
        ++p1;
        ++p2;
    }
    திரும்பு *p1 == *p2;
}

U_CAPI UBool U_EXPORT2
uhash_compareIStringView(மாறிலி UHashTok key1, மாறிலி UHashTok key2) {
    மாறிலி std::string_view* p1 = static_cast<std::string_view*>(key1.pointer);
    மாறிலி std::string_view* p2 = static_cast<std::string_view*>(key2.pointer);
    எனில் (p1 == p2) {
        திரும்பு உண்மை;
    }
    எனில் (p1 == nullptr || p2 == nullptr) {
        திரும்பு பொய்;
    }
    மாறிலி std::string_view& v1 = *p1;
    மாறிலி std::string_view& v2 = *p2;
    எனில் (v1.size() != v2.size()) {
        திரும்பு பொய்;
    }
    க்கு (size_t i = 0; i < v1.size(); ++i) {
        எனில் (uprv_tolower(v1[i]) != uprv_tolower(v2[i])) {
            திரும்பு பொய்;
        }
    }
    திரும்பு உண்மை;
}

/********************************************************************
 * PUBLIC int32_t Support Functions
 ********************************************************************/

U_CAPI int32_t U_EXPORT2
uhash_hashLong(மாறிலி UHashTok சாவி) {
    திரும்பு சாவி.integer;
}

U_CAPI UBool U_EXPORT2
uhash_compareLong(மாறிலி UHashTok key1, மாறிலி UHashTok key2) {
    திரும்பு key1.integer == key2.integer;
}
