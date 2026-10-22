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

#समावेश <string_view>

#समावेश "uhash.h"
#समावेश "unicode/ustring.h"
#समावेश "cstring.h"
#समावेश "cmemory.h"
#समावेश "uassert.h"
#समावेश "ustr_imp.h"

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
अटल स्थिर int32_t PRIMES[] = {
    7, 13, 31, 61, 127, 251, 509, 1021, 2039, 4093, 8191, 16381, 32749,
    65521, 131071, 262139, 524287, 1048573, 2097143, 4194301, 8388593,
    16777213, 33554393, 67108859, 134217689, 268435399, 536870909,
    1073741789, 2147483647 /*, 4294967291 */
};

#घोषणा PRIMES_लंबाई UPRV_LENGTHOF(PRIMES)
#घोषणा DEFAULT_अभाज्य_सूचकांक 4

/* These ratios are tuned to the PRIMES array such that a resize
 * places the table back into the zone of non-resizing.  That is,
 * after a call to _uhash_rehash(), a subsequent call to
 * _uhash_rehash() should do nothing (should not churn).  This is only
 * a potential problem with U_GROW_AND_SHRINK.
 */
अटल स्थिर भग्न RESIZE_POLICY_RATIO_TABLE[6] = {
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
#घोषणा HASH_DELETED    ((int32_t) 0x80000000)
#घोषणा HASH_EMPTY      ((int32_t) HASH_DELETED + 1)

#घोषणा IS_EMPTY_OR_DELETED(x) ((x) < 0)

/* This macro expects a UHashTok.pointer as its keypointer and
   valuepointer parameters */
#घोषणा HASH_DELETE_कुंजी_मूल्य(hash, keypointer, valuepointer) UPRV_BLOCK_MACRO_BEGIN { \
    अगर (hash->कुंजी_Deleter != nullptr && keypointer != nullptr) { \
        (*hash->कुंजी_Deleter)(keypointer); \
    } \
    अगर (hash->मूल्य_Deleter != nullptr && valuepointer != nullptr) { \
        (*hash->मूल्य_Deleter)(valuepointer); \
    } \
} UPRV_BLOCK_MACRO_END

/*
 * Constants for hinting whether a key or value is an integer
 * or a pointer.  If a hint bit is zero, then the associated
 * token is assumed to be an integer.
 */
#घोषणा HINT_BOTH_INTEGERS (0)
#घोषणा HINT_कुंजी_POINTER   (1)
#घोषणा HINT_मूल्य_POINTER (2)
#घोषणा HINT_ALLOW_ZERO    (4)

/********************************************************************
 * PRIVATE Implementation
 ********************************************************************/

अटल UHashTok
_uhash_setElement(UHashtable *hash, UHashElement* e,
                  int32_t hashcode,
                  UHashTok कुंजी, UHashTok value, int8_t hint) {

    UHashTok old_मूल्य = e->value;
    अगर (hash->कुंजी_Deleter != nullptr && e->कुंजी.pointer != nullptr &&
        e->कुंजी.pointer != कुंजी.pointer) { /* Avoid double deletion */
        (*hash->कुंजी_Deleter)(e->कुंजी.pointer);
    }
    अगर (hash->मूल्य_Deleter != nullptr) {
        अगर (old_मूल्य.pointer != nullptr &&
            old_मूल्य.pointer != value.pointer) { /* Avoid double deletion */
            (*hash->मूल्य_Deleter)(old_मूल्य.pointer);
        }
        old_मूल्य.pointer = nullptr;
    }
    /* Compilers should copy the UHashTok union correctly, but even if
     * they do, memory heap tools (e.g. BoundsChecker) can get
     * confused when a pointer is cloaked in a union and then copied.
     * TO ALLEVIATE THIS, we use hints (based on what API the user is
     * calling) to copy pointers when we know the user thinks
     * something is a pointer. */
    अगर (hint & HINT_कुंजी_POINTER) {
        e->कुंजी.pointer = कुंजी.pointer;
    } अन्यथा {
        e->कुंजी = कुंजी;
    }
    अगर (hint & HINT_मूल्य_POINTER) {
        e->value.pointer = value.pointer;
    } अन्यथा {
        e->value = value;
    }
    e->hashcode = hashcode;
    वापस old_मूल्य;
}

/**
 * Assumes that the given element is not empty or deleted.
 */
अटल UHashTok
_uhash_internalRemoveElement(UHashtable *hash, UHashElement* e) {
    UHashTok empty;
    U_ASSERT(!IS_EMPTY_OR_DELETED(e->hashcode));
    --hash->count;
    empty.pointer = nullptr; empty.integer = 0;
    वापस _uhash_setElement(hash, e, HASH_DELETED, empty, empty, 0);
}

अटल व्योम
_uhash_internalSetResizePolicy(UHashtable *hash, क्रमागत UHashResizePolicy policy) {
    U_ASSERT(hash != nullptr);
    U_ASSERT(((int32_t)policy) >= 0);
    U_ASSERT(((int32_t)policy) < 3);
    hash->निचला_Water_Ratio  = RESIZE_POLICY_RATIO_TABLE[policy * 2];
    hash->ऊपरी_Water_Ratio = RESIZE_POLICY_RATIO_TABLE[policy * 2 + 1];
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
अटल व्योम
_uhash_allocate(UHashtable *hash,
                int32_t अभाज्य_सूचकांक,
                UErrorCode *status) {

    UHashElement *p, *limit;
    UHashTok emptytok;

    अगर (U_FAILURE(*status)) वापस;

    U_ASSERT(अभाज्य_सूचकांक >= 0 && अभाज्य_सूचकांक < PRIMES_लंबाई);

    hash->अभाज्य_सूचकांक = static_cast<int8_t>(अभाज्य_सूचकांक);
    hash->length = PRIMES[अभाज्य_सूचकांक];

    p = hash->elements = static_cast<UHashElement*>(
        uprv_malloc(माप(UHashElement) * hash->length));

    अगर (hash->elements == nullptr) {
        *status = U_MEMORY_ALLOCATION_ERROR;
        वापस;
    }

    emptytok.pointer = nullptr; /* Only one of these two is needed */
    emptytok.integer = 0;    /* but we don't know which one. */

    limit = p + hash->length;
    जबतक (p < limit) {
        p->कुंजी = emptytok;
        p->value = emptytok;
        p->hashcode = HASH_EMPTY;
        ++p;
    }

    hash->count = 0;
    hash->निचला_Water_Mark = static_cast<int32_t>(hash->length * hash->निचला_Water_Ratio);
    hash->ऊपरी_Water_Mark = static_cast<int32_t>(hash->length * hash->ऊपरी_Water_Ratio);
}

अटल UHashtable*
_uhash_init(UHashtable *परिणाम,
              UHashFunction *कुंजी_Hash,
              UKeyComparator *कुंजी_Comp,
              UValueComparator *मूल्य_Comp,
              int32_t अभाज्य_सूचकांक,
              UErrorCode *status)
{
    अगर (U_FAILURE(*status)) वापस nullptr;
    U_ASSERT(कुंजी_Hash != nullptr);
    U_ASSERT(कुंजी_Comp != nullptr);

    परिणाम->कुंजी_Hasher       = कुंजी_Hash;
    परिणाम->कुंजी_Comparator   = कुंजी_Comp;
    परिणाम->मूल्य_Comparator = मूल्य_Comp;
    परिणाम->कुंजी_Deleter      = nullptr;
    परिणाम->मूल्य_Deleter    = nullptr;
    परिणाम->allocated       = false;
    _uhash_internalSetResizePolicy(परिणाम, U_GROW);

    _uhash_allocate(परिणाम, अभाज्य_सूचकांक, status);

    अगर (U_FAILURE(*status)) {
        वापस nullptr;
    }

    वापस परिणाम;
}

अटल UHashtable*
_uhash_create(UHashFunction *कुंजी_Hash,
              UKeyComparator *कुंजी_Comp,
              UValueComparator *मूल्य_Comp,
              int32_t अभाज्य_सूचकांक,
              UErrorCode *status) {
    UHashtable *परिणाम;

    अगर (U_FAILURE(*status)) वापस nullptr;

    परिणाम = static_cast<UHashtable*>(uprv_malloc(माप(UHashtable)));
    अगर (परिणाम == nullptr) {
        *status = U_MEMORY_ALLOCATION_ERROR;
        वापस nullptr;
    }

    _uhash_init(परिणाम, कुंजी_Hash, कुंजी_Comp, मूल्य_Comp, अभाज्य_सूचकांक, status);
    परिणाम->allocated       = true;

    अगर (U_FAILURE(*status)) {
        uprv_free(परिणाम);
        वापस nullptr;
    }

    वापस परिणाम;
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
अटल UHashElement*
_uhash_find(स्थिर UHashtable *hash, UHashTok कुंजी,
            int32_t hashcode) {

    int32_t firstDeleted = -1;  /* assume invalid index */
    int32_t the_सूचकांक, start_सूचकांक;
    int32_t jump = 0; /* lazy evaluate */
    int32_t tableHash;
    UHashElement *elements = hash->elements;

    hashcode &= 0x7FFFFFFF; /* must be positive */
    start_सूचकांक = the_सूचकांक = (hashcode ^ 0x4000000) % hash->length;

    करो {
        tableHash = elements[the_सूचकांक].hashcode;
        अगर (tableHash == hashcode) {          /* quick check */
            अगर ((*hash->कुंजी_Comparator)(कुंजी, elements[the_सूचकांक].कुंजी)) {
                वापस &(elements[the_सूचकांक]);
            }
        } अन्यथा अगर (!IS_EMPTY_OR_DELETED(tableHash)) {
            /* We have hit a slot which contains a key-value pair,
             * but for which the hash code does not match.  Keep
             * looking.
             */
        } अन्यथा अगर (tableHash == HASH_EMPTY) { /* empty, end o' the line */
            अवरोध;
        } अन्यथा अगर (firstDeleted < 0) { /* remember first deleted */
            firstDeleted = the_सूचकांक;
        }
        अगर (jump == 0) { /* lazy compute jump */
            /* The jump value must be relatively prime to the table
             * length.  As long as the length is prime, then any value
             * 1..length-1 will be relatively prime to it.
             */
            jump = (hashcode % (hash->length - 1)) + 1;
        }
        the_सूचकांक = (the_सूचकांक + jump) % hash->length;
    } जबतक (the_सूचकांक != start_सूचकांक);

    अगर (firstDeleted >= 0) {
        the_सूचकांक = firstDeleted; /* reset if had deleted slot */
    } अन्यथा अगर (tableHash != HASH_EMPTY) {
        /* We get to this point if the hashtable is full (no empty or
         * deleted slots), and we've failed to find a match.  THIS
         * WILL NEVER HAPPEN as long as uhash_put() makes sure that
         * count is always < length.
         */
        UPRV_UNREACHABLE_EXIT;
    }
    वापस &(elements[the_सूचकांक]);
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
अटल व्योम
_uhash_rehash(UHashtable *hash, UErrorCode *status) {

    UHashElement *old = hash->elements;
    int32_t old_लंबाई = hash->length;
    int32_t new_अभाज्य_सूचकांक = hash->अभाज्य_सूचकांक;
    int32_t i;

    अगर (hash->count > hash->ऊपरी_Water_Mark) {
        अगर (++new_अभाज्य_सूचकांक >= PRIMES_लंबाई) {
            वापस;
        }
    } अन्यथा अगर (hash->count < hash->निचला_Water_Mark) {
        अगर (--new_अभाज्य_सूचकांक < 0) {
            वापस;
        }
    } अन्यथा {
        वापस;
    }

    _uhash_allocate(hash, new_अभाज्य_सूचकांक, status);

    अगर (U_FAILURE(*status)) {
        hash->elements = old;
        hash->length = old_लंबाई;
        वापस;
    }

    क्रम (i = old_लंबाई - 1; i >= 0; --i) {
        अगर (!IS_EMPTY_OR_DELETED(old[i].hashcode)) {
            UHashElement *e = _uhash_find(hash, old[i].कुंजी, old[i].hashcode);
            U_ASSERT(e != nullptr);
            U_ASSERT(e->hashcode == HASH_EMPTY);
            e->कुंजी = old[i].कुंजी;
            e->value = old[i].value;
            e->hashcode = old[i].hashcode;
            ++hash->count;
        }
    }

    uprv_free(old);
}

अटल UHashTok
_uhash_remove(UHashtable *hash,
              UHashTok कुंजी) {
    /* First find the position of the key in the table.  If the object
     * has not been removed already, remove it.  If the user wanted
     * keys deleted, then delete it also.  We have to put a special
     * hashcode in that position that means that something has been
     * deleted, since when we do a find, we have to continue PAST any
     * deleted values.
     */
    UHashTok परिणाम;
    UHashElement* e = _uhash_find(hash, कुंजी, hash->कुंजी_Hasher(कुंजी));
    U_ASSERT(e != nullptr);
    परिणाम.pointer = nullptr;
    परिणाम.integer = 0;
    अगर (!IS_EMPTY_OR_DELETED(e->hashcode)) {
        परिणाम = _uhash_internalRemoveElement(hash, e);
        अगर (hash->count < hash->निचला_Water_Mark) {
            UErrorCode status = U_ZERO_ERROR;
            _uhash_rehash(hash, &status);
        }
    }
    वापस परिणाम;
}

अटल UHashTok
_uhash_put(UHashtable *hash,
           UHashTok कुंजी,
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

    अगर (U_FAILURE(*status)) {
        जाओ err;
    }
    U_ASSERT(hash != nullptr);
    अगर ((hint & HINT_मूल्य_POINTER) ?
            value.pointer == nullptr :
            value.integer == 0 && (hint & HINT_ALLOW_ZERO) == 0) {
        /* Disallow storage of nullptr values, since nullptr is returned by
         * get() to indicate an absent key.  Storing nullptr == removing.
         */
        वापस _uhash_remove(hash, कुंजी);
    }
    अगर (hash->count > hash->ऊपरी_Water_Mark) {
        _uhash_rehash(hash, status);
        अगर (U_FAILURE(*status)) {
            जाओ err;
        }
    }

    hashcode = (*hash->कुंजी_Hasher)(कुंजी);
    e = _uhash_find(hash, कुंजी, hashcode);
    U_ASSERT(e != nullptr);

    अगर (IS_EMPTY_OR_DELETED(e->hashcode)) {
        /* Important: We must never actually fill the table up.  If we
         * do so, then _uhash_find() will return nullptr, and we'll have
         * to check for nullptr after every call to _uhash_find().  To
         * avoid this we make sure there is always at least one empty
         * or deleted slot in the table.  This only is a problem if we
         * are out of memory and rehash isn't working.
         */
        ++hash->count;
        अगर (hash->count == hash->length) {
            /* Don't allow count to reach length */
            --hash->count;
            *status = U_MEMORY_ALLOCATION_ERROR;
            जाओ err;
        }
    }

    /* We must in all cases handle storage properly.  If there was an
     * old key, then it must be deleted (if the deleter != nullptr).
     * Make hashcodes stored in table positive.
     */
    वापस _uhash_setElement(hash, e, hashcode & 0x7FFFFFFF, कुंजी, value, hint);

 err:
    /* If the deleters are non-nullptr, this method adopts its key and/or
     * value arguments, and we must be sure to delete the key and/or
     * value in all cases, even upon failure.
     */
    HASH_DELETE_कुंजी_मूल्य(hash, कुंजी.pointer, value.pointer);
    emptytok.pointer = nullptr; emptytok.integer = 0;
    वापस emptytok;
}


/********************************************************************
 * PUBLIC API
 ********************************************************************/

U_CAPI UHashtable* U_EXPORT2
uhash_open(UHashFunction *कुंजी_Hash,
           UKeyComparator *कुंजी_Comp,
           UValueComparator *मूल्य_Comp,
           UErrorCode *status) {

    वापस _uhash_create(कुंजी_Hash, कुंजी_Comp, मूल्य_Comp, DEFAULT_अभाज्य_सूचकांक, status);
}

U_CAPI UHashtable* U_EXPORT2
uhash_open_आकार(UHashFunction *कुंजी_Hash,
               UKeyComparator *कुंजी_Comp,
               UValueComparator *मूल्य_Comp,
               int32_t size,
               UErrorCode *status) {

    /* Find the smallest index i for which PRIMES[i] >= size. */
    int32_t i = 0;
    जबतक (i<(PRIMES_लंबाई-1) && PRIMES[i]<size) {
        ++i;
    }

    वापस _uhash_create(कुंजी_Hash, कुंजी_Comp, मूल्य_Comp, i, status);
}

U_CAPI UHashtable* U_EXPORT2
uhash_init(UHashtable *fillin_परिणाम,
           UHashFunction *कुंजी_Hash,
           UKeyComparator *कुंजी_Comp,
           UValueComparator *मूल्य_Comp,
           UErrorCode *status) {

    वापस _uhash_init(fillin_परिणाम, कुंजी_Hash, कुंजी_Comp, मूल्य_Comp, DEFAULT_अभाज्य_सूचकांक, status);
}

U_CAPI UHashtable* U_EXPORT2
uhash_init_आकार(UHashtable *fillin_परिणाम,
               UHashFunction *कुंजी_Hash,
               UKeyComparator *कुंजी_Comp,
               UValueComparator *मूल्य_Comp,
               int32_t size,
               UErrorCode *status) {

    // Find the smallest index i for which PRIMES[i] >= size.
    int32_t i = 0;
    जबतक (i<(PRIMES_लंबाई-1) && PRIMES[i]<size) {
        ++i;
    }
    वापस _uhash_init(fillin_परिणाम, कुंजी_Hash, कुंजी_Comp, मूल्य_Comp, i, status);
}

U_CAPI व्योम U_EXPORT2
uhash_close(UHashtable *hash) {
    अगर (hash == nullptr) {
        वापस;
    }
    अगर (hash->elements != nullptr) {
        अगर (hash->कुंजी_Deleter != nullptr || hash->मूल्य_Deleter != nullptr) {
            int32_t pos=UHASH_FIRST;
            UHashElement *e;
            जबतक ((e = (UHashElement*) uhash_अग्र_Element(hash, &pos)) != nullptr) {
                HASH_DELETE_कुंजी_मूल्य(hash, e->कुंजी.pointer, e->value.pointer);
            }
        }
        uprv_free(hash->elements);
        hash->elements = nullptr;
    }
    अगर (hash->allocated) {
        uprv_free(hash);
    }
}

U_CAPI UHashFunction *U_EXPORT2
uhash_set_कुंजी_Hasher(UHashtable *hash, UHashFunction *fn) {
    UHashFunction *परिणाम = hash->कुंजी_Hasher;
    hash->कुंजी_Hasher = fn;
    वापस परिणाम;
}

U_CAPI UKeyComparator *U_EXPORT2
uhash_set_कुंजी_Comparator(UHashtable *hash, UKeyComparator *fn) {
    UKeyComparator *परिणाम = hash->कुंजी_Comparator;
    hash->कुंजी_Comparator = fn;
    वापस परिणाम;
}
U_CAPI UValueComparator *U_EXPORT2
uhash_set_मूल्य_Comparator(UHashtable *hash, UValueComparator *fn){
    UValueComparator *परिणाम = hash->मूल्य_Comparator;
    hash->मूल्य_Comparator = fn;
    वापस परिणाम;
}

U_CAPI UObjectDeleter *U_EXPORT2
uhash_set_कुंजी_Deleter(UHashtable *hash, UObjectDeleter *fn) {
    UObjectDeleter *परिणाम = hash->कुंजी_Deleter;
    hash->कुंजी_Deleter = fn;
    वापस परिणाम;
}

U_CAPI UObjectDeleter *U_EXPORT2
uhash_set_मूल्य_Deleter(UHashtable *hash, UObjectDeleter *fn) {
    UObjectDeleter *परिणाम = hash->मूल्य_Deleter;
    hash->मूल्य_Deleter = fn;
    वापस परिणाम;
}

U_CAPI व्योम U_EXPORT2
uhash_setResizePolicy(UHashtable *hash, क्रमागत UHashResizePolicy policy) {
    UErrorCode status = U_ZERO_ERROR;
    _uhash_internalSetResizePolicy(hash, policy);
    hash->निचला_Water_Mark  = (int32_t)(hash->length * hash->निचला_Water_Ratio);
    hash->ऊपरी_Water_Mark = (int32_t)(hash->length * hash->ऊपरी_Water_Ratio);
    _uhash_rehash(hash, &status);
}

U_CAPI int32_t U_EXPORT2
uhash_गिनती(स्थिर UHashtable *hash) {
    वापस hash->count;
}

U_CAPI व्योम* U_EXPORT2
uhash_get(स्थिर UHashtable *hash,
          स्थिर व्योम* कुंजी) {
    UHashTok keyholder;
    keyholder.pointer = (व्योम*) कुंजी;
    वापस _uhash_find(hash, keyholder, hash->कुंजी_Hasher(keyholder))->value.pointer;
}

U_CAPI व्योम* U_EXPORT2
uhash_iget(स्थिर UHashtable *hash,
           int32_t कुंजी) {
    UHashTok keyholder;
    keyholder.integer = कुंजी;
    वापस _uhash_find(hash, keyholder, hash->कुंजी_Hasher(keyholder))->value.pointer;
}

U_CAPI int32_t U_EXPORT2
uhash_geti(स्थिर UHashtable *hash,
           स्थिर व्योम* कुंजी) {
    UHashTok keyholder;
    keyholder.pointer = (व्योम*) कुंजी;
    वापस _uhash_find(hash, keyholder, hash->कुंजी_Hasher(keyholder))->value.integer;
}

U_CAPI int32_t U_EXPORT2
uhash_igeti(स्थिर UHashtable *hash,
           int32_t कुंजी) {
    UHashTok keyholder;
    keyholder.integer = कुंजी;
    वापस _uhash_find(hash, keyholder, hash->कुंजी_Hasher(keyholder))->value.integer;
}

U_CAPI int32_t U_EXPORT2
uhash_getiAndFound(स्थिर UHashtable *hash,
                   स्थिर व्योम *कुंजी,
                   UBool *found) {
    UHashTok keyholder;
    keyholder.pointer = (व्योम *)कुंजी;
    स्थिर UHashElement *e = _uhash_find(hash, keyholder, hash->कुंजी_Hasher(keyholder));
    *found = !IS_EMPTY_OR_DELETED(e->hashcode);
    वापस e->value.integer;
}

U_CAPI int32_t U_EXPORT2
uhash_igetiAndFound(स्थिर UHashtable *hash,
                    int32_t कुंजी,
                    UBool *found) {
    UHashTok keyholder;
    keyholder.integer = कुंजी;
    स्थिर UHashElement *e = _uhash_find(hash, keyholder, hash->कुंजी_Hasher(keyholder));
    *found = !IS_EMPTY_OR_DELETED(e->hashcode);
    वापस e->value.integer;
}

U_CAPI व्योम* U_EXPORT2
uhash_put(UHashtable *hash,
          व्योम* कुंजी,
          व्योम* value,
          UErrorCode *status) {
    UHashTok keyholder, valueholder;
    keyholder.pointer = कुंजी;
    valueholder.pointer = value;
    वापस _uhash_put(hash, keyholder, valueholder,
                      HINT_कुंजी_POINTER | HINT_मूल्य_POINTER,
                      status).pointer;
}

U_CAPI व्योम* U_EXPORT2
uhash_iput(UHashtable *hash,
           int32_t कुंजी,
           व्योम* value,
           UErrorCode *status) {
    UHashTok keyholder, valueholder;
    keyholder.integer = कुंजी;
    valueholder.pointer = value;
    वापस _uhash_put(hash, keyholder, valueholder,
                      HINT_मूल्य_POINTER,
                      status).pointer;
}

U_CAPI int32_t U_EXPORT2
uhash_puti(UHashtable *hash,
           व्योम* कुंजी,
           int32_t value,
           UErrorCode *status) {
    UHashTok keyholder, valueholder;
    keyholder.pointer = कुंजी;
    valueholder.integer = value;
    वापस _uhash_put(hash, keyholder, valueholder,
                      HINT_कुंजी_POINTER,
                      status).integer;
}


U_CAPI int32_t U_EXPORT2
uhash_iputi(UHashtable *hash,
           int32_t कुंजी,
           int32_t value,
           UErrorCode *status) {
    UHashTok keyholder, valueholder;
    keyholder.integer = कुंजी;
    valueholder.integer = value;
    वापस _uhash_put(hash, keyholder, valueholder,
                      HINT_BOTH_INTEGERS,
                      status).integer;
}

U_CAPI int32_t U_EXPORT2
uhash_putiAllowZero(UHashtable *hash,
                    व्योम *कुंजी,
                    int32_t value,
                    UErrorCode *status) {
    UHashTok keyholder, valueholder;
    keyholder.pointer = कुंजी;
    valueholder.integer = value;
    वापस _uhash_put(hash, keyholder, valueholder,
                      HINT_कुंजी_POINTER | HINT_ALLOW_ZERO,
                      status).integer;
}


U_CAPI int32_t U_EXPORT2
uhash_iputiAllowZero(UHashtable *hash,
                     int32_t कुंजी,
                     int32_t value,
                     UErrorCode *status) {
    UHashTok keyholder, valueholder;
    keyholder.integer = कुंजी;
    valueholder.integer = value;
    वापस _uhash_put(hash, keyholder, valueholder,
                      HINT_BOTH_INTEGERS | HINT_ALLOW_ZERO,
                      status).integer;
}

U_CAPI व्योम* U_EXPORT2
uhash_remove(UHashtable *hash,
             स्थिर व्योम* कुंजी) {
    UHashTok keyholder;
    keyholder.pointer = (व्योम*) कुंजी;
    वापस _uhash_remove(hash, keyholder).pointer;
}

U_CAPI व्योम* U_EXPORT2
uhash_iremove(UHashtable *hash,
              int32_t कुंजी) {
    UHashTok keyholder;
    keyholder.integer = कुंजी;
    वापस _uhash_remove(hash, keyholder).pointer;
}

U_CAPI int32_t U_EXPORT2
uhash_removei(UHashtable *hash,
              स्थिर व्योम* कुंजी) {
    UHashTok keyholder;
    keyholder.pointer = (व्योम*) कुंजी;
    वापस _uhash_remove(hash, keyholder).integer;
}

U_CAPI int32_t U_EXPORT2
uhash_iremovei(UHashtable *hash,
               int32_t कुंजी) {
    UHashTok keyholder;
    keyholder.integer = कुंजी;
    वापस _uhash_remove(hash, keyholder).integer;
}

U_CAPI व्योम U_EXPORT2
uhash_removeAll(UHashtable *hash) {
    int32_t pos = UHASH_FIRST;
    स्थिर UHashElement *e;
    U_ASSERT(hash != nullptr);
    अगर (hash->count != 0) {
        जबतक ((e = uhash_अग्र_Element(hash, &pos)) != nullptr) {
            uhash_removeElement(hash, e);
        }
    }
    U_ASSERT(hash->count == 0);
}

U_CAPI UBool U_EXPORT2
uhash_contains_कुंजी(स्थिर UHashtable *hash, स्थिर व्योम *कुंजी) {
    UHashTok keyholder;
    keyholder.pointer = (व्योम *)कुंजी;
    स्थिर UHashElement *e = _uhash_find(hash, keyholder, hash->कुंजी_Hasher(keyholder));
    वापस !IS_EMPTY_OR_DELETED(e->hashcode);
}

/**
 * Returns true if the UHashtable contains an item with this integer key.
 *
 * @param hash The target UHashtable.
 * @param key An integer key stored in a hashtable
 * @return true if the key is found.
 */
U_CAPI UBool U_EXPORT2
uhash_icontains_कुंजी(स्थिर UHashtable *hash, int32_t कुंजी) {
    UHashTok keyholder;
    keyholder.integer = कुंजी;
    स्थिर UHashElement *e = _uhash_find(hash, keyholder, hash->कुंजी_Hasher(keyholder));
    वापस !IS_EMPTY_OR_DELETED(e->hashcode);
}

U_CAPI स्थिर UHashElement* U_EXPORT2
uhash_find(स्थिर UHashtable *hash, स्थिर व्योम* कुंजी) {
    UHashTok keyholder;
    स्थिर UHashElement *e;
    keyholder.pointer = (व्योम*) कुंजी;
    e = _uhash_find(hash, keyholder, hash->कुंजी_Hasher(keyholder));
    वापस IS_EMPTY_OR_DELETED(e->hashcode) ? nullptr : e;
}

U_CAPI स्थिर UHashElement* U_EXPORT2
uhash_अग्र_Element(स्थिर UHashtable *hash, int32_t *pos) {
    /* Walk through the array until we find an element that is not
     * EMPTY and not DELETED.
     */
    int32_t i;
    U_ASSERT(hash != nullptr);
    क्रम (i = *pos + 1; i < hash->length; ++i) {
        अगर (!IS_EMPTY_OR_DELETED(hash->elements[i].hashcode)) {
            *pos = i;
            वापस &(hash->elements[i]);
        }
    }

    /* No more elements */
    वापस nullptr;
}

U_CAPI व्योम* U_EXPORT2
uhash_removeElement(UHashtable *hash, स्थिर UHashElement* e) {
    U_ASSERT(hash != nullptr);
    U_ASSERT(e != nullptr);
    अगर (!IS_EMPTY_OR_DELETED(e->hashcode)) {
        UHashElement *nce = (UHashElement *)e;
        वापस _uhash_internalRemoveElement(hash, nce).pointer;
    }
    वापस nullptr;
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
uhash_hashUChars(स्थिर UHashTok कुंजी) {
    स्थिर char16_t *s = (स्थिर char16_t *)कुंजी.pointer;
    वापस s == nullptr ? 0 : ustr_hashUCharsN(s, u_strlen(s));
}

U_CAPI int32_t U_EXPORT2
uhash_hashChars(स्थिर UHashTok कुंजी) {
    स्थिर अक्षर *s = (स्थिर अक्षर *)कुंजी.pointer;
    वापस s == nullptr ? 0 : static_cast<int32_t>(ustr_hashCharsN(s, static_cast<int32_t>(uprv_strlen(s))));
}

U_CAPI int32_t U_EXPORT2
uhash_hashIChars(स्थिर UHashTok कुंजी) {
    स्थिर अक्षर *s = (स्थिर अक्षर *)कुंजी.pointer;
    वापस s == nullptr ? 0 : ustr_hashICharsN(s, static_cast<int32_t>(uprv_strlen(s)));
}

U_CAPI int32_t U_EXPORT2
uhash_hashIStringView(स्थिर UHashTok कुंजी) {
    स्थिर std::string_view* s = static_cast<std::string_view*>(कुंजी.pointer);
    वापस s == nullptr ? 0 : ustr_hashICharsN(s->तथ्य(), static_cast<int32_t>(s->size()));
}

U_CAPI UBool U_EXPORT2
uhash_equals(स्थिर UHashtable* hash1, स्थिर UHashtable* hash2){
    int32_t count1, count2, pos, i;

    अगर(hash1==hash2){
        वापस true;
    }

    /*
     * Make sure that we are comparing 2 valid hashes of the same type
     * with valid comparison functions.
     * Without valid comparison functions, a binary comparison
     * of the hash values will yield random results on machines
     * with 64-bit pointers and 32-bit integer hashes.
     * A valueComparator is normally optional.
     */
    अगर (hash1==nullptr || hash2==nullptr ||
        hash1->कुंजी_Comparator != hash2->कुंजी_Comparator ||
        hash1->मूल्य_Comparator != hash2->मूल्य_Comparator ||
        hash1->मूल्य_Comparator == nullptr)
    {
        /*
        Normally we would return an error here about incompatible hash tables,
        but we return false instead.
        */
        वापस false;
    }

    count1 = uhash_गिनती(hash1);
    count2 = uhash_गिनती(hash2);
    अगर(count1!=count2){
        वापस false;
    }

    pos=UHASH_FIRST;
    क्रम(i=0; i<count1; i++){
        स्थिर UHashElement* elem1 = uhash_अग्र_Element(hash1, &pos);
        स्थिर UHashTok key1 = elem1->कुंजी;
        स्थिर UHashTok val1 = elem1->value;
        /* here the keys are not compared, instead the key form hash1 is used to fetch
         * value from hash2. If the hashes are equal then then both hashes should
         * contain equal values for the same key!
         */
        स्थिर UHashElement* elem2 = _uhash_find(hash2, key1, hash2->कुंजी_Hasher(key1));
        स्थिर UHashTok val2 = elem2->value;
        अगर(hash1->मूल्य_Comparator(val1, val2)==false){
            वापस false;
        }
    }
    वापस true;
}

/********************************************************************
 * PUBLIC Comparator Functions
 ********************************************************************/

U_CAPI UBool U_EXPORT2
uhash_compareUChars(स्थिर UHashTok key1, स्थिर UHashTok key2) {
    स्थिर char16_t *p1 = (स्थिर char16_t*) key1.pointer;
    स्थिर char16_t *p2 = (स्थिर char16_t*) key2.pointer;
    अगर (p1 == p2) {
        वापस true;
    }
    अगर (p1 == nullptr || p2 == nullptr) {
        वापस false;
    }
    जबतक (*p1 != 0 && *p1 == *p2) {
        ++p1;
        ++p2;
    }
    वापस *p1 == *p2;
}

U_CAPI UBool U_EXPORT2
uhash_compareChars(स्थिर UHashTok key1, स्थिर UHashTok key2) {
    स्थिर अक्षर *p1 = (स्थिर अक्षर*) key1.pointer;
    स्थिर अक्षर *p2 = (स्थिर अक्षर*) key2.pointer;
    अगर (p1 == p2) {
        वापस true;
    }
    अगर (p1 == nullptr || p2 == nullptr) {
        वापस false;
    }
    जबतक (*p1 != 0 && *p1 == *p2) {
        ++p1;
        ++p2;
    }
    वापस *p1 == *p2;
}

U_CAPI UBool U_EXPORT2
uhash_compareIChars(स्थिर UHashTok key1, स्थिर UHashTok key2) {
    स्थिर अक्षर *p1 = (स्थिर अक्षर*) key1.pointer;
    स्थिर अक्षर *p2 = (स्थिर अक्षर*) key2.pointer;
    अगर (p1 == p2) {
        वापस true;
    }
    अगर (p1 == nullptr || p2 == nullptr) {
        वापस false;
    }
    जबतक (*p1 != 0 && uprv_tolower(*p1) == uprv_tolower(*p2)) {
        ++p1;
        ++p2;
    }
    वापस *p1 == *p2;
}

U_CAPI UBool U_EXPORT2
uhash_compareIStringView(स्थिर UHashTok key1, स्थिर UHashTok key2) {
    स्थिर std::string_view* p1 = static_cast<std::string_view*>(key1.pointer);
    स्थिर std::string_view* p2 = static_cast<std::string_view*>(key2.pointer);
    अगर (p1 == p2) {
        वापस true;
    }
    अगर (p1 == nullptr || p2 == nullptr) {
        वापस false;
    }
    स्थिर std::string_view& v1 = *p1;
    स्थिर std::string_view& v2 = *p2;
    अगर (v1.size() != v2.size()) {
        वापस false;
    }
    क्रम (माप_प्रकार i = 0; i < v1.size(); ++i) {
        अगर (uprv_tolower(v1[i]) != uprv_tolower(v2[i])) {
            वापस false;
        }
    }
    वापस true;
}

/********************************************************************
 * PUBLIC int32_t Support Functions
 ********************************************************************/

U_CAPI int32_t U_EXPORT2
uhash_hashLong(स्थिर UHashTok कुंजी) {
    वापस कुंजी.integer;
}

U_CAPI UBool U_EXPORT2
uhash_compareLong(स्थिर UHashTok key1, स्थिर UHashTok key2) {
    वापस key1.integer == key2.integer;
}
