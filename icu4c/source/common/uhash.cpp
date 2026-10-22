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

#অন্তর্ভাব <string_view>

#অন্তর্ভাব "uhash.h"
#অন্তর্ভাব "unicode/ustring.h"
#অন্তর্ভাব "cstring.h"
#অন্তর্ভাব "cmemory.h"
#অন্তর্ভাব "uassert.h"
#অন্তর্ভাব "ustr_imp.h"

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
স্থির ধ্রুবক int32_t PRIMES[] = {
    7, 13, 31, 61, 127, 251, 509, 1021, 2039, 4093, 8191, 16381, 32749,
    65521, 131071, 262139, 524287, 1048573, 2097143, 4194301, 8388593,
    16777213, 33554393, 67108859, 134217689, 268435399, 536870909,
    1073741789, 2147483647 /*, 4294967291 */
};

#define PRIMES_দৈর্ঘ্য UPRV_LENGTHOF(PRIMES)
#define DEFAULT_মৌলিক_সূচক 4

/* These ratios are tuned to the PRIMES array such that a resize
 * places the table back into the zone of non-resizing.  That is,
 * after a call to _uhash_rehash(), a subsequent call to
 * _uhash_rehash() should do nothing (should not churn).  This is only
 * a potential problem with U_GROW_AND_SHRINK.
 */
স্থির ধ্রুবক দশমিক RESIZE_POLICY_RATIO_TABLE[6] = {
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
#define HASH_DELETE_চাবি_মান(hash, keypointer, valuepointer) UPRV_BLOCK_MACRO_BEGIN { \
    যদি (hash->চাবি_Deleter != nullptr && keypointer != nullptr) { \
        (*hash->চাবি_Deleter)(keypointer); \
    } \
    যদি (hash->মান_Deleter != nullptr && valuepointer != nullptr) { \
        (*hash->মান_Deleter)(valuepointer); \
    } \
} UPRV_BLOCK_MACRO_END

/*
 * Constants for hinting whether a key or value is an integer
 * or a pointer.  If a hint bit is zero, then the associated
 * token is assumed to be an integer.
 */
#define HINT_BOTH_INTEGERS (0)
#define HINT_চাবি_POINTER   (1)
#define HINT_মান_POINTER (2)
#define HINT_ALLOW_ZERO    (4)

/********************************************************************
 * PRIVATE Implementation
 ********************************************************************/

স্থির UHashTok
_uhash_setElement(UHashtable *hash, UHashElement* e,
                  int32_t hashcode,
                  UHashTok চাবি, UHashTok value, int8_t hint) {

    UHashTok old_মান = e->value;
    যদি (hash->চাবি_Deleter != nullptr && e->চাবি.pointer != nullptr &&
        e->চাবি.pointer != চাবি.pointer) { /* Avoid double deletion */
        (*hash->চাবি_Deleter)(e->চাবি.pointer);
    }
    যদি (hash->মান_Deleter != nullptr) {
        যদি (old_মান.pointer != nullptr &&
            old_মান.pointer != value.pointer) { /* Avoid double deletion */
            (*hash->মান_Deleter)(old_মান.pointer);
        }
        old_মান.pointer = nullptr;
    }
    /* Compilers should copy the UHashTok union correctly, but even if
     * they do, memory heap tools (e.g. BoundsChecker) can get
     * confused when a pointer is cloaked in a union and then copied.
     * TO ALLEVIATE THIS, we use hints (based on what API the user is
     * calling) to copy pointers when we know the user thinks
     * something is a pointer. */
    যদি (hint & HINT_চাবি_POINTER) {
        e->চাবি.pointer = চাবি.pointer;
    } অন্যথা {
        e->চাবি = চাবি;
    }
    যদি (hint & HINT_মান_POINTER) {
        e->value.pointer = value.pointer;
    } অন্যথা {
        e->value = value;
    }
    e->hashcode = hashcode;
    ফেরত old_মান;
}

/**
 * Assumes that the given element is not empty or deleted.
 */
স্থির UHashTok
_uhash_internalRemoveElement(UHashtable *hash, UHashElement* e) {
    UHashTok empty;
    U_ASSERT(!IS_EMPTY_OR_DELETED(e->hashcode));
    --hash->count;
    empty.pointer = nullptr; empty.integer = 0;
    ফেরত _uhash_setElement(hash, e, HASH_DELETED, empty, empty, 0);
}

স্থির শূন্য
_uhash_internalSetResizePolicy(UHashtable *hash, গণনা UHashResizePolicy policy) {
    U_ASSERT(hash != nullptr);
    U_ASSERT(((int32_t)policy) >= 0);
    U_ASSERT(((int32_t)policy) < 3);
    hash->নিচের_Water_Ratio  = RESIZE_POLICY_RATIO_TABLE[policy * 2];
    hash->উপরের_Water_Ratio = RESIZE_POLICY_RATIO_TABLE[policy * 2 + 1];
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
স্থির শূন্য
_uhash_allocate(UHashtable *hash,
                int32_t মৌলিক_সূচক,
                UErrorCode *status) {

    UHashElement *p, *limit;
    UHashTok emptytok;

    যদি (U_FAILURE(*status)) ফেরত;

    U_ASSERT(মৌলিক_সূচক >= 0 && মৌলিক_সূচক < PRIMES_দৈর্ঘ্য);

    hash->মৌলিক_সূচক = static_cast<int8_t>(মৌলিক_সূচক);
    hash->length = PRIMES[মৌলিক_সূচক];

    p = hash->elements = static_cast<UHashElement*>(
        uprv_malloc(আকার(UHashElement) * hash->length));

    যদি (hash->elements == nullptr) {
        *status = U_MEMORY_ALLOCATION_ERROR;
        ফেরত;
    }

    emptytok.pointer = nullptr; /* Only one of these two is needed */
    emptytok.integer = 0;    /* but we don't know which one. */

    limit = p + hash->length;
    যখন (p < limit) {
        p->চাবি = emptytok;
        p->value = emptytok;
        p->hashcode = HASH_EMPTY;
        ++p;
    }

    hash->count = 0;
    hash->নিচের_Water_Mark = static_cast<int32_t>(hash->length * hash->নিচের_Water_Ratio);
    hash->উপরের_Water_Mark = static_cast<int32_t>(hash->length * hash->উপরের_Water_Ratio);
}

স্থির UHashtable*
_uhash_init(UHashtable *ফলাফল,
              UHashFunction *চাবি_Hash,
              UKeyComparator *চাবি_Comp,
              UValueComparator *মান_Comp,
              int32_t মৌলিক_সূচক,
              UErrorCode *status)
{
    যদি (U_FAILURE(*status)) ফেরত nullptr;
    U_ASSERT(চাবি_Hash != nullptr);
    U_ASSERT(চাবি_Comp != nullptr);

    ফলাফল->চাবি_Hasher       = চাবি_Hash;
    ফলাফল->চাবি_Comparator   = চাবি_Comp;
    ফলাফল->মান_Comparator = মান_Comp;
    ফলাফল->চাবি_Deleter      = nullptr;
    ফলাফল->মান_Deleter    = nullptr;
    ফলাফল->allocated       = মিথ্যা;
    _uhash_internalSetResizePolicy(ফলাফল, U_GROW);

    _uhash_allocate(ফলাফল, মৌলিক_সূচক, status);

    যদি (U_FAILURE(*status)) {
        ফেরত nullptr;
    }

    ফেরত ফলাফল;
}

স্থির UHashtable*
_uhash_create(UHashFunction *চাবি_Hash,
              UKeyComparator *চাবি_Comp,
              UValueComparator *মান_Comp,
              int32_t মৌলিক_সূচক,
              UErrorCode *status) {
    UHashtable *ফলাফল;

    যদি (U_FAILURE(*status)) ফেরত nullptr;

    ফলাফল = static_cast<UHashtable*>(uprv_malloc(আকার(UHashtable)));
    যদি (ফলাফল == nullptr) {
        *status = U_MEMORY_ALLOCATION_ERROR;
        ফেরত nullptr;
    }

    _uhash_init(ফলাফল, চাবি_Hash, চাবি_Comp, মান_Comp, মৌলিক_সূচক, status);
    ফলাফল->allocated       = সত্য;

    যদি (U_FAILURE(*status)) {
        uprv_free(ফলাফল);
        ফেরত nullptr;
    }

    ফেরত ফলাফল;
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
স্থির UHashElement*
_uhash_find(ধ্রুবক UHashtable *hash, UHashTok চাবি,
            int32_t hashcode) {

    int32_t firstDeleted = -1;  /* assume invalid index */
    int32_t the_সূচক, start_সূচক;
    int32_t jump = 0; /* lazy evaluate */
    int32_t tableHash;
    UHashElement *elements = hash->elements;

    hashcode &= 0x7FFFFFFF; /* must be positive */
    start_সূচক = the_সূচক = (hashcode ^ 0x4000000) % hash->length;

    করো {
        tableHash = elements[the_সূচক].hashcode;
        যদি (tableHash == hashcode) {          /* quick check */
            যদি ((*hash->চাবি_Comparator)(চাবি, elements[the_সূচক].চাবি)) {
                ফেরত &(elements[the_সূচক]);
            }
        } অন্যথা যদি (!IS_EMPTY_OR_DELETED(tableHash)) {
            /* We have hit a slot which contains a key-value pair,
             * but for which the hash code does not match.  Keep
             * looking.
             */
        } অন্যথা যদি (tableHash == HASH_EMPTY) { /* empty, end o' the line */
            বিরতি;
        } অন্যথা যদি (firstDeleted < 0) { /* remember first deleted */
            firstDeleted = the_সূচক;
        }
        যদি (jump == 0) { /* lazy compute jump */
            /* The jump value must be relatively prime to the table
             * length.  As long as the length is prime, then any value
             * 1..length-1 will be relatively prime to it.
             */
            jump = (hashcode % (hash->length - 1)) + 1;
        }
        the_সূচক = (the_সূচক + jump) % hash->length;
    } যখন (the_সূচক != start_সূচক);

    যদি (firstDeleted >= 0) {
        the_সূচক = firstDeleted; /* reset if had deleted slot */
    } অন্যথা যদি (tableHash != HASH_EMPTY) {
        /* We get to this point if the hashtable is full (no empty or
         * deleted slots), and we've failed to find a match.  THIS
         * WILL NEVER HAPPEN as long as uhash_put() makes sure that
         * count is always < length.
         */
        UPRV_UNREACHABLE_EXIT;
    }
    ফেরত &(elements[the_সূচক]);
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
স্থির শূন্য
_uhash_rehash(UHashtable *hash, UErrorCode *status) {

    UHashElement *old = hash->elements;
    int32_t old_দৈর্ঘ্য = hash->length;
    int32_t new_মৌলিক_সূচক = hash->মৌলিক_সূচক;
    int32_t i;

    যদি (hash->count > hash->উপরের_Water_Mark) {
        যদি (++new_মৌলিক_সূচক >= PRIMES_দৈর্ঘ্য) {
            ফেরত;
        }
    } অন্যথা যদি (hash->count < hash->নিচের_Water_Mark) {
        যদি (--new_মৌলিক_সূচক < 0) {
            ফেরত;
        }
    } অন্যথা {
        ফেরত;
    }

    _uhash_allocate(hash, new_মৌলিক_সূচক, status);

    যদি (U_FAILURE(*status)) {
        hash->elements = old;
        hash->length = old_দৈর্ঘ্য;
        ফেরত;
    }

    জন্য (i = old_দৈর্ঘ্য - 1; i >= 0; --i) {
        যদি (!IS_EMPTY_OR_DELETED(old[i].hashcode)) {
            UHashElement *e = _uhash_find(hash, old[i].চাবি, old[i].hashcode);
            U_ASSERT(e != nullptr);
            U_ASSERT(e->hashcode == HASH_EMPTY);
            e->চাবি = old[i].চাবি;
            e->value = old[i].value;
            e->hashcode = old[i].hashcode;
            ++hash->count;
        }
    }

    uprv_free(old);
}

স্থির UHashTok
_uhash_remove(UHashtable *hash,
              UHashTok চাবি) {
    /* First find the position of the key in the table.  If the object
     * has not been removed already, remove it.  If the user wanted
     * keys deleted, then delete it also.  We have to put a special
     * hashcode in that position that means that something has been
     * deleted, since when we do a find, we have to continue PAST any
     * deleted values.
     */
    UHashTok ফলাফল;
    UHashElement* e = _uhash_find(hash, চাবি, hash->চাবি_Hasher(চাবি));
    U_ASSERT(e != nullptr);
    ফলাফল.pointer = nullptr;
    ফলাফল.integer = 0;
    যদি (!IS_EMPTY_OR_DELETED(e->hashcode)) {
        ফলাফল = _uhash_internalRemoveElement(hash, e);
        যদি (hash->count < hash->নিচের_Water_Mark) {
            UErrorCode status = U_ZERO_ERROR;
            _uhash_rehash(hash, &status);
        }
    }
    ফেরত ফলাফল;
}

স্থির UHashTok
_uhash_put(UHashtable *hash,
           UHashTok চাবি,
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

    যদি (U_FAILURE(*status)) {
        যাও err;
    }
    U_ASSERT(hash != nullptr);
    যদি ((hint & HINT_মান_POINTER) ?
            value.pointer == nullptr :
            value.integer == 0 && (hint & HINT_ALLOW_ZERO) == 0) {
        /* Disallow storage of nullptr values, since nullptr is returned by
         * get() to indicate an absent key.  Storing nullptr == removing.
         */
        ফেরত _uhash_remove(hash, চাবি);
    }
    যদি (hash->count > hash->উপরের_Water_Mark) {
        _uhash_rehash(hash, status);
        যদি (U_FAILURE(*status)) {
            যাও err;
        }
    }

    hashcode = (*hash->চাবি_Hasher)(চাবি);
    e = _uhash_find(hash, চাবি, hashcode);
    U_ASSERT(e != nullptr);

    যদি (IS_EMPTY_OR_DELETED(e->hashcode)) {
        /* Important: We must never actually fill the table up.  If we
         * do so, then _uhash_find() will return nullptr, and we'll have
         * to check for nullptr after every call to _uhash_find().  To
         * avoid this we make sure there is always at least one empty
         * or deleted slot in the table.  This only is a problem if we
         * are out of memory and rehash isn't working.
         */
        ++hash->count;
        যদি (hash->count == hash->length) {
            /* Don't allow count to reach length */
            --hash->count;
            *status = U_MEMORY_ALLOCATION_ERROR;
            যাও err;
        }
    }

    /* We must in all cases handle storage properly.  If there was an
     * old key, then it must be deleted (if the deleter != nullptr).
     * Make hashcodes stored in table positive.
     */
    ফেরত _uhash_setElement(hash, e, hashcode & 0x7FFFFFFF, চাবি, value, hint);

 err:
    /* If the deleters are non-nullptr, this method adopts its key and/or
     * value arguments, and we must be sure to delete the key and/or
     * value in all cases, even upon failure.
     */
    HASH_DELETE_চাবি_মান(hash, চাবি.pointer, value.pointer);
    emptytok.pointer = nullptr; emptytok.integer = 0;
    ফেরত emptytok;
}


/********************************************************************
 * PUBLIC API
 ********************************************************************/

U_CAPI UHashtable* U_EXPORT2
uhash_open(UHashFunction *চাবি_Hash,
           UKeyComparator *চাবি_Comp,
           UValueComparator *মান_Comp,
           UErrorCode *status) {

    ফেরত _uhash_create(চাবি_Hash, চাবি_Comp, মান_Comp, DEFAULT_মৌলিক_সূচক, status);
}

U_CAPI UHashtable* U_EXPORT2
uhash_open_আকার(UHashFunction *চাবি_Hash,
               UKeyComparator *চাবি_Comp,
               UValueComparator *মান_Comp,
               int32_t size,
               UErrorCode *status) {

    /* Find the smallest index i for which PRIMES[i] >= size. */
    int32_t i = 0;
    যখন (i<(PRIMES_দৈর্ঘ্য-1) && PRIMES[i]<size) {
        ++i;
    }

    ফেরত _uhash_create(চাবি_Hash, চাবি_Comp, মান_Comp, i, status);
}

U_CAPI UHashtable* U_EXPORT2
uhash_init(UHashtable *fillin_ফলাফল,
           UHashFunction *চাবি_Hash,
           UKeyComparator *চাবি_Comp,
           UValueComparator *মান_Comp,
           UErrorCode *status) {

    ফেরত _uhash_init(fillin_ফলাফল, চাবি_Hash, চাবি_Comp, মান_Comp, DEFAULT_মৌলিক_সূচক, status);
}

U_CAPI UHashtable* U_EXPORT2
uhash_init_আকার(UHashtable *fillin_ফলাফল,
               UHashFunction *চাবি_Hash,
               UKeyComparator *চাবি_Comp,
               UValueComparator *মান_Comp,
               int32_t size,
               UErrorCode *status) {

    // Find the smallest index i for which PRIMES[i] >= size.
    int32_t i = 0;
    যখন (i<(PRIMES_দৈর্ঘ্য-1) && PRIMES[i]<size) {
        ++i;
    }
    ফেরত _uhash_init(fillin_ফলাফল, চাবি_Hash, চাবি_Comp, মান_Comp, i, status);
}

U_CAPI শূন্য U_EXPORT2
uhash_close(UHashtable *hash) {
    যদি (hash == nullptr) {
        ফেরত;
    }
    যদি (hash->elements != nullptr) {
        যদি (hash->চাবি_Deleter != nullptr || hash->মান_Deleter != nullptr) {
            int32_t pos=UHASH_FIRST;
            UHashElement *e;
            যখন ((e = (UHashElement*) uhash_পরবর্তী_Element(hash, &pos)) != nullptr) {
                HASH_DELETE_চাবি_মান(hash, e->চাবি.pointer, e->value.pointer);
            }
        }
        uprv_free(hash->elements);
        hash->elements = nullptr;
    }
    যদি (hash->allocated) {
        uprv_free(hash);
    }
}

U_CAPI UHashFunction *U_EXPORT2
uhash_set_চাবি_Hasher(UHashtable *hash, UHashFunction *fn) {
    UHashFunction *ফলাফল = hash->চাবি_Hasher;
    hash->চাবি_Hasher = fn;
    ফেরত ফলাফল;
}

U_CAPI UKeyComparator *U_EXPORT2
uhash_set_চাবি_Comparator(UHashtable *hash, UKeyComparator *fn) {
    UKeyComparator *ফলাফল = hash->চাবি_Comparator;
    hash->চাবি_Comparator = fn;
    ফেরত ফলাফল;
}
U_CAPI UValueComparator *U_EXPORT2
uhash_set_মান_Comparator(UHashtable *hash, UValueComparator *fn){
    UValueComparator *ফলাফল = hash->মান_Comparator;
    hash->মান_Comparator = fn;
    ফেরত ফলাফল;
}

U_CAPI UObjectDeleter *U_EXPORT2
uhash_set_চাবি_Deleter(UHashtable *hash, UObjectDeleter *fn) {
    UObjectDeleter *ফলাফল = hash->চাবি_Deleter;
    hash->চাবি_Deleter = fn;
    ফেরত ফলাফল;
}

U_CAPI UObjectDeleter *U_EXPORT2
uhash_set_মান_Deleter(UHashtable *hash, UObjectDeleter *fn) {
    UObjectDeleter *ফলাফল = hash->মান_Deleter;
    hash->মান_Deleter = fn;
    ফেরত ফলাফল;
}

U_CAPI শূন্য U_EXPORT2
uhash_setResizePolicy(UHashtable *hash, গণনা UHashResizePolicy policy) {
    UErrorCode status = U_ZERO_ERROR;
    _uhash_internalSetResizePolicy(hash, policy);
    hash->নিচের_Water_Mark  = (int32_t)(hash->length * hash->নিচের_Water_Ratio);
    hash->উপরের_Water_Mark = (int32_t)(hash->length * hash->উপরের_Water_Ratio);
    _uhash_rehash(hash, &status);
}

U_CAPI int32_t U_EXPORT2
uhash_গুনতি(ধ্রুবক UHashtable *hash) {
    ফেরত hash->count;
}

U_CAPI শূন্য* U_EXPORT2
uhash_get(ধ্রুবক UHashtable *hash,
          ধ্রুবক শূন্য* চাবি) {
    UHashTok keyholder;
    keyholder.pointer = (শূন্য*) চাবি;
    ফেরত _uhash_find(hash, keyholder, hash->চাবি_Hasher(keyholder))->value.pointer;
}

U_CAPI শূন্য* U_EXPORT2
uhash_iget(ধ্রুবক UHashtable *hash,
           int32_t চাবি) {
    UHashTok keyholder;
    keyholder.integer = চাবি;
    ফেরত _uhash_find(hash, keyholder, hash->চাবি_Hasher(keyholder))->value.pointer;
}

U_CAPI int32_t U_EXPORT2
uhash_geti(ধ্রুবক UHashtable *hash,
           ধ্রুবক শূন্য* চাবি) {
    UHashTok keyholder;
    keyholder.pointer = (শূন্য*) চাবি;
    ফেরত _uhash_find(hash, keyholder, hash->চাবি_Hasher(keyholder))->value.integer;
}

U_CAPI int32_t U_EXPORT2
uhash_igeti(ধ্রুবক UHashtable *hash,
           int32_t চাবি) {
    UHashTok keyholder;
    keyholder.integer = চাবি;
    ফেরত _uhash_find(hash, keyholder, hash->চাবি_Hasher(keyholder))->value.integer;
}

U_CAPI int32_t U_EXPORT2
uhash_getiAndFound(ধ্রুবক UHashtable *hash,
                   ধ্রুবক শূন্য *চাবি,
                   UBool *found) {
    UHashTok keyholder;
    keyholder.pointer = (শূন্য *)চাবি;
    ধ্রুবক UHashElement *e = _uhash_find(hash, keyholder, hash->চাবি_Hasher(keyholder));
    *found = !IS_EMPTY_OR_DELETED(e->hashcode);
    ফেরত e->value.integer;
}

U_CAPI int32_t U_EXPORT2
uhash_igetiAndFound(ধ্রুবক UHashtable *hash,
                    int32_t চাবি,
                    UBool *found) {
    UHashTok keyholder;
    keyholder.integer = চাবি;
    ধ্রুবক UHashElement *e = _uhash_find(hash, keyholder, hash->চাবি_Hasher(keyholder));
    *found = !IS_EMPTY_OR_DELETED(e->hashcode);
    ফেরত e->value.integer;
}

U_CAPI শূন্য* U_EXPORT2
uhash_put(UHashtable *hash,
          শূন্য* চাবি,
          শূন্য* value,
          UErrorCode *status) {
    UHashTok keyholder, valueholder;
    keyholder.pointer = চাবি;
    valueholder.pointer = value;
    ফেরত _uhash_put(hash, keyholder, valueholder,
                      HINT_চাবি_POINTER | HINT_মান_POINTER,
                      status).pointer;
}

U_CAPI শূন্য* U_EXPORT2
uhash_iput(UHashtable *hash,
           int32_t চাবি,
           শূন্য* value,
           UErrorCode *status) {
    UHashTok keyholder, valueholder;
    keyholder.integer = চাবি;
    valueholder.pointer = value;
    ফেরত _uhash_put(hash, keyholder, valueholder,
                      HINT_মান_POINTER,
                      status).pointer;
}

U_CAPI int32_t U_EXPORT2
uhash_puti(UHashtable *hash,
           শূন্য* চাবি,
           int32_t value,
           UErrorCode *status) {
    UHashTok keyholder, valueholder;
    keyholder.pointer = চাবি;
    valueholder.integer = value;
    ফেরত _uhash_put(hash, keyholder, valueholder,
                      HINT_চাবি_POINTER,
                      status).integer;
}


U_CAPI int32_t U_EXPORT2
uhash_iputi(UHashtable *hash,
           int32_t চাবি,
           int32_t value,
           UErrorCode *status) {
    UHashTok keyholder, valueholder;
    keyholder.integer = চাবি;
    valueholder.integer = value;
    ফেরত _uhash_put(hash, keyholder, valueholder,
                      HINT_BOTH_INTEGERS,
                      status).integer;
}

U_CAPI int32_t U_EXPORT2
uhash_putiAllowZero(UHashtable *hash,
                    শূন্য *চাবি,
                    int32_t value,
                    UErrorCode *status) {
    UHashTok keyholder, valueholder;
    keyholder.pointer = চাবি;
    valueholder.integer = value;
    ফেরত _uhash_put(hash, keyholder, valueholder,
                      HINT_চাবি_POINTER | HINT_ALLOW_ZERO,
                      status).integer;
}


U_CAPI int32_t U_EXPORT2
uhash_iputiAllowZero(UHashtable *hash,
                     int32_t চাবি,
                     int32_t value,
                     UErrorCode *status) {
    UHashTok keyholder, valueholder;
    keyholder.integer = চাবি;
    valueholder.integer = value;
    ফেরত _uhash_put(hash, keyholder, valueholder,
                      HINT_BOTH_INTEGERS | HINT_ALLOW_ZERO,
                      status).integer;
}

U_CAPI শূন্য* U_EXPORT2
uhash_remove(UHashtable *hash,
             ধ্রুবক শূন্য* চাবি) {
    UHashTok keyholder;
    keyholder.pointer = (শূন্য*) চাবি;
    ফেরত _uhash_remove(hash, keyholder).pointer;
}

U_CAPI শূন্য* U_EXPORT2
uhash_iremove(UHashtable *hash,
              int32_t চাবি) {
    UHashTok keyholder;
    keyholder.integer = চাবি;
    ফেরত _uhash_remove(hash, keyholder).pointer;
}

U_CAPI int32_t U_EXPORT2
uhash_removei(UHashtable *hash,
              ধ্রুবক শূন্য* চাবি) {
    UHashTok keyholder;
    keyholder.pointer = (শূন্য*) চাবি;
    ফেরত _uhash_remove(hash, keyholder).integer;
}

U_CAPI int32_t U_EXPORT2
uhash_iremovei(UHashtable *hash,
               int32_t চাবি) {
    UHashTok keyholder;
    keyholder.integer = চাবি;
    ফেরত _uhash_remove(hash, keyholder).integer;
}

U_CAPI শূন্য U_EXPORT2
uhash_removeAll(UHashtable *hash) {
    int32_t pos = UHASH_FIRST;
    ধ্রুবক UHashElement *e;
    U_ASSERT(hash != nullptr);
    যদি (hash->count != 0) {
        যখন ((e = uhash_পরবর্তী_Element(hash, &pos)) != nullptr) {
            uhash_removeElement(hash, e);
        }
    }
    U_ASSERT(hash->count == 0);
}

U_CAPI UBool U_EXPORT2
uhash_contains_চাবি(ধ্রুবক UHashtable *hash, ধ্রুবক শূন্য *চাবি) {
    UHashTok keyholder;
    keyholder.pointer = (শূন্য *)চাবি;
    ধ্রুবক UHashElement *e = _uhash_find(hash, keyholder, hash->চাবি_Hasher(keyholder));
    ফেরত !IS_EMPTY_OR_DELETED(e->hashcode);
}

/**
 * Returns true if the UHashtable contains an item with this integer key.
 *
 * @param hash The target UHashtable.
 * @param key An integer key stored in a hashtable
 * @return true if the key is found.
 */
U_CAPI UBool U_EXPORT2
uhash_icontains_চাবি(ধ্রুবক UHashtable *hash, int32_t চাবি) {
    UHashTok keyholder;
    keyholder.integer = চাবি;
    ধ্রুবক UHashElement *e = _uhash_find(hash, keyholder, hash->চাবি_Hasher(keyholder));
    ফেরত !IS_EMPTY_OR_DELETED(e->hashcode);
}

U_CAPI ধ্রুবক UHashElement* U_EXPORT2
uhash_find(ধ্রুবক UHashtable *hash, ধ্রুবক শূন্য* চাবি) {
    UHashTok keyholder;
    ধ্রুবক UHashElement *e;
    keyholder.pointer = (শূন্য*) চাবি;
    e = _uhash_find(hash, keyholder, hash->চাবি_Hasher(keyholder));
    ফেরত IS_EMPTY_OR_DELETED(e->hashcode) ? nullptr : e;
}

U_CAPI ধ্রুবক UHashElement* U_EXPORT2
uhash_পরবর্তী_Element(ধ্রুবক UHashtable *hash, int32_t *pos) {
    /* Walk through the array until we find an element that is not
     * EMPTY and not DELETED.
     */
    int32_t i;
    U_ASSERT(hash != nullptr);
    জন্য (i = *pos + 1; i < hash->length; ++i) {
        যদি (!IS_EMPTY_OR_DELETED(hash->elements[i].hashcode)) {
            *pos = i;
            ফেরত &(hash->elements[i]);
        }
    }

    /* No more elements */
    ফেরত nullptr;
}

U_CAPI শূন্য* U_EXPORT2
uhash_removeElement(UHashtable *hash, ধ্রুবক UHashElement* e) {
    U_ASSERT(hash != nullptr);
    U_ASSERT(e != nullptr);
    যদি (!IS_EMPTY_OR_DELETED(e->hashcode)) {
        UHashElement *nce = (UHashElement *)e;
        ফেরত _uhash_internalRemoveElement(hash, nce).pointer;
    }
    ফেরত nullptr;
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
uhash_hashUChars(ধ্রুবক UHashTok চাবি) {
    ধ্রুবক char16_t *s = (ধ্রুবক char16_t *)চাবি.pointer;
    ফেরত s == nullptr ? 0 : ustr_hashUCharsN(s, u_strlen(s));
}

U_CAPI int32_t U_EXPORT2
uhash_hashChars(ধ্রুবক UHashTok চাবি) {
    ধ্রুবক অক্ষর *s = (ধ্রুবক অক্ষর *)চাবি.pointer;
    ফেরত s == nullptr ? 0 : static_cast<int32_t>(ustr_hashCharsN(s, static_cast<int32_t>(uprv_strlen(s))));
}

U_CAPI int32_t U_EXPORT2
uhash_hashIChars(ধ্রুবক UHashTok চাবি) {
    ধ্রুবক অক্ষর *s = (ধ্রুবক অক্ষর *)চাবি.pointer;
    ফেরত s == nullptr ? 0 : ustr_hashICharsN(s, static_cast<int32_t>(uprv_strlen(s)));
}

U_CAPI int32_t U_EXPORT2
uhash_hashIStringView(ধ্রুবক UHashTok চাবি) {
    ধ্রুবক std::string_view* s = static_cast<std::string_view*>(চাবি.pointer);
    ফেরত s == nullptr ? 0 : ustr_hashICharsN(s->তথ্য(), static_cast<int32_t>(s->size()));
}

U_CAPI UBool U_EXPORT2
uhash_equals(ধ্রুবক UHashtable* hash1, ধ্রুবক UHashtable* hash2){
    int32_t count1, count2, pos, i;

    যদি(hash1==hash2){
        ফেরত সত্য;
    }

    /*
     * Make sure that we are comparing 2 valid hashes of the same type
     * with valid comparison functions.
     * Without valid comparison functions, a binary comparison
     * of the hash values will yield random results on machines
     * with 64-bit pointers and 32-bit integer hashes.
     * A valueComparator is normally optional.
     */
    যদি (hash1==nullptr || hash2==nullptr ||
        hash1->চাবি_Comparator != hash2->চাবি_Comparator ||
        hash1->মান_Comparator != hash2->মান_Comparator ||
        hash1->মান_Comparator == nullptr)
    {
        /*
        Normally we would return an error here about incompatible hash tables,
        but we return false instead.
        */
        ফেরত মিথ্যা;
    }

    count1 = uhash_গুনতি(hash1);
    count2 = uhash_গুনতি(hash2);
    যদি(count1!=count2){
        ফেরত মিথ্যা;
    }

    pos=UHASH_FIRST;
    জন্য(i=0; i<count1; i++){
        ধ্রুবক UHashElement* elem1 = uhash_পরবর্তী_Element(hash1, &pos);
        ধ্রুবক UHashTok key1 = elem1->চাবি;
        ধ্রুবক UHashTok val1 = elem1->value;
        /* here the keys are not compared, instead the key form hash1 is used to fetch
         * value from hash2. If the hashes are equal then then both hashes should
         * contain equal values for the same key!
         */
        ধ্রুবক UHashElement* elem2 = _uhash_find(hash2, key1, hash2->চাবি_Hasher(key1));
        ধ্রুবক UHashTok val2 = elem2->value;
        যদি(hash1->মান_Comparator(val1, val2)==মিথ্যা){
            ফেরত মিথ্যা;
        }
    }
    ফেরত সত্য;
}

/********************************************************************
 * PUBLIC Comparator Functions
 ********************************************************************/

U_CAPI UBool U_EXPORT2
uhash_compareUChars(ধ্রুবক UHashTok key1, ধ্রুবক UHashTok key2) {
    ধ্রুবক char16_t *p1 = (ধ্রুবক char16_t*) key1.pointer;
    ধ্রুবক char16_t *p2 = (ধ্রুবক char16_t*) key2.pointer;
    যদি (p1 == p2) {
        ফেরত সত্য;
    }
    যদি (p1 == nullptr || p2 == nullptr) {
        ফেরত মিথ্যা;
    }
    যখন (*p1 != 0 && *p1 == *p2) {
        ++p1;
        ++p2;
    }
    ফেরত *p1 == *p2;
}

U_CAPI UBool U_EXPORT2
uhash_compareChars(ধ্রুবক UHashTok key1, ধ্রুবক UHashTok key2) {
    ধ্রুবক অক্ষর *p1 = (ধ্রুবক অক্ষর*) key1.pointer;
    ধ্রুবক অক্ষর *p2 = (ধ্রুবক অক্ষর*) key2.pointer;
    যদি (p1 == p2) {
        ফেরত সত্য;
    }
    যদি (p1 == nullptr || p2 == nullptr) {
        ফেরত মিথ্যা;
    }
    যখন (*p1 != 0 && *p1 == *p2) {
        ++p1;
        ++p2;
    }
    ফেরত *p1 == *p2;
}

U_CAPI UBool U_EXPORT2
uhash_compareIChars(ধ্রুবক UHashTok key1, ধ্রুবক UHashTok key2) {
    ধ্রুবক অক্ষর *p1 = (ধ্রুবক অক্ষর*) key1.pointer;
    ধ্রুবক অক্ষর *p2 = (ধ্রুবক অক্ষর*) key2.pointer;
    যদি (p1 == p2) {
        ফেরত সত্য;
    }
    যদি (p1 == nullptr || p2 == nullptr) {
        ফেরত মিথ্যা;
    }
    যখন (*p1 != 0 && uprv_tolower(*p1) == uprv_tolower(*p2)) {
        ++p1;
        ++p2;
    }
    ফেরত *p1 == *p2;
}

U_CAPI UBool U_EXPORT2
uhash_compareIStringView(ধ্রুবক UHashTok key1, ধ্রুবক UHashTok key2) {
    ধ্রুবক std::string_view* p1 = static_cast<std::string_view*>(key1.pointer);
    ধ্রুবক std::string_view* p2 = static_cast<std::string_view*>(key2.pointer);
    যদি (p1 == p2) {
        ফেরত সত্য;
    }
    যদি (p1 == nullptr || p2 == nullptr) {
        ফেরত মিথ্যা;
    }
    ধ্রুবক std::string_view& v1 = *p1;
    ধ্রুবক std::string_view& v2 = *p2;
    যদি (v1.size() != v2.size()) {
        ফেরত মিথ্যা;
    }
    জন্য (size_t i = 0; i < v1.size(); ++i) {
        যদি (uprv_tolower(v1[i]) != uprv_tolower(v2[i])) {
            ফেরত মিথ্যা;
        }
    }
    ফেরত সত্য;
}

/********************************************************************
 * PUBLIC int32_t Support Functions
 ********************************************************************/

U_CAPI int32_t U_EXPORT2
uhash_hashLong(ধ্রুবক UHashTok চাবি) {
    ফেরত চাবি.integer;
}

U_CAPI UBool U_EXPORT2
uhash_compareLong(ধ্রুবক UHashTok key1, ধ্রুবক UHashTok key2) {
    ফেরত key1.integer == key2.integer;
}
