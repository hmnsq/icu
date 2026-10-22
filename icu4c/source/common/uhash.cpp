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

#అన్తర్భావ <string_view>

#అన్తర్భావ "uhash.h"
#అన్తర్భావ "unicode/ustring.h"
#అన్తర్భావ "cstring.h"
#అన్తర్భావ "cmemory.h"
#అన్తర్భావ "uassert.h"
#అన్తర్భావ "ustr_imp.h"

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
స్థిర స్థిరరాశి int32_t ప్రధానసంఖ్యలు[] = {
    7, 13, 31, 61, 127, 251, 509, 1021, 2039, 4093, 8191, 16381, 32749,
    65521, 131071, 262139, 524287, 1048573, 2097143, 4194301, 8388593,
    16777213, 33554393, 67108859, 134217689, 268435399, 536870909,
    1073741789, 2147483647 /*, 4294967291 */
};

#నిర్వచించు ప్రధానసంఖ్యలు_నిడివి UPRV_LENGTHOF(ప్రధానసంఖ్యలు)
#నిర్వచించు DEFAULT_ప్రధానసంఖ్య_సూచీ 4

/* These ratios are tuned to the PRIMES array such that a resize
 * places the table back into the zone of non-resizing.  That is,
 * after a call to _uhash_rehash(), a subsequent call to
 * _uhash_rehash() should do nothing (should not churn).  This is only
 * a potential problem with U_GROW_AND_SHRINK.
 */
స్థిర స్థిరరాశి దశాంశం RESIZE_POLICY_నిష్పత్తి_పట్టిక[6] = {
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
#నిర్వచించు HASH_DELETED    ((int32_t) 0x80000000)
#నిర్వచించు HASH_ఖాళీలు      ((int32_t) HASH_DELETED + 1)

#నిర్వచించు IS_ఖాళీలు_OR_DELETED(x) ((x) < 0)

/* This macro expects a UHashTok.pointer as its keypointer and
   valuepointer parameters */
#నిర్వచించు HASH_DELETE_కీలకం_విలువ(hash, keypointer, valuepointer) UPRV_ఖండం_MACRO_BEGIN { \
    ఒకవేళ (hash->కీలకం_Deleter != శూన్యసూచిక && keypointer != శూన్యసూచిక) { \
        (*hash->కీలకం_Deleter)(keypointer); \
    } \
    ఒకవేళ (hash->విలువ_Deleter != శూన్యసూచిక && valuepointer != శూన్యసూచిక) { \
        (*hash->విలువ_Deleter)(valuepointer); \
    } \
} UPRV_ఖండం_MACRO_చివర

/*
 * Constants for hinting whether a key or value is an integer
 * or a pointer.  If a hint bit is zero, then the associated
 * token is assumed to be an integer.
 */
#నిర్వచించు HINT_BOTH_INTEGERS (0)
#నిర్వచించు HINT_కీలకం_సూచిక   (1)
#నిర్వచించు HINT_విలువ_సూచిక (2)
#నిర్వచించు HINT_ALLOW_ZERO    (4)

/********************************************************************
 * PRIVATE Implementation
 ********************************************************************/

స్థిర UHashTok
uhash_సమితి_Element(UHashtable *hash, UHashElement* e,
                  int32_t hashcode,
                  UHashTok కీలకం, UHashTok value, int8_t hint) {

    UHashTok old_విలువ = e->value;
    ఒకవేళ (hash->కీలకం_Deleter != శూన్యసూచిక && e->కీలకం.సూచిక != శూన్యసూచిక &&
        e->కీలకం.సూచిక != కీలకం.సూచిక) { /* Avoid double deletion */
        (*hash->కీలకం_Deleter)(e->కీలకం.సూచిక);
    }
    ఒకవేళ (hash->విలువ_Deleter != శూన్యసూచిక) {
        ఒకవేళ (old_విలువ.సూచిక != శూన్యసూచిక &&
            old_విలువ.సూచిక != value.సూచిక) { /* Avoid double deletion */
            (*hash->విలువ_Deleter)(old_విలువ.సూచిక);
        }
        old_విలువ.సూచిక = శూన్యసూచిక;
    }
    /* Compilers should copy the UHashTok union correctly, but even if
     * they do, memory heap tools (e.g. BoundsChecker) can get
     * confused when a pointer is cloaked in a union and then copied.
     * TO ALLEVIATE THIS, we use hints (based on what API the user is
     * calling) to copy pointers when we know the user thinks
     * something is a pointer. */
    ఒకవేళ (hint & HINT_కీలకం_సూచిక) {
        e->కీలకం.సూచిక = కీలకం.సూచిక;
    } లేకపోతే {
        e->కీలకం = కీలకం;
    }
    ఒకవేళ (hint & HINT_విలువ_సూచిక) {
        e->value.సూచిక = value.సూచిక;
    } లేకపోతే {
        e->value = value;
    }
    e->hashcode = hashcode;
    తిరిగి old_విలువ;
}

/**
 * Assumes that the given element is not empty or deleted.
 */
స్థిర UHashTok
uhash_అంతర్గత_Remove_Element(UHashtable *hash, UHashElement* e) {
    UHashTok empty;
    U_ASSERT(!IS_ఖాళీలు_OR_DELETED(e->hashcode));
    --hash->count;
    empty.సూచిక = శూన్యసూచిక; empty.integer = 0;
    తిరిగి uhash_సమితి_Element(hash, e, HASH_DELETED, empty, empty, 0);
}

స్థిర శూన్య
uhash_అంతర్గత_సమితి_Resize_Policy(UHashtable *hash, గణన UHashResizePolicy policy) {
    U_ASSERT(hash != శూన్యసూచిక);
    U_ASSERT(((int32_t)policy) >= 0);
    U_ASSERT(((int32_t)policy) < 3);
    hash->దిగువ_Water_నిష్పత్తి  = RESIZE_POLICY_నిష్పత్తి_పట్టిక[policy * 2];
    hash->ఎగువ_Water_నిష్పత్తి = RESIZE_POLICY_నిష్పత్తి_పట్టిక[policy * 2 + 1];
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
స్థిర శూన్య
_uhash_allocate(UHashtable *hash,
                int32_t ప్రధానసంఖ్య_సూచీ,
                UError_సంకేతం *స్థితి_2) {

    UHashElement *p, *హద్దు;
    UHashTok emptytok;

    ఒకవేళ (U_FAILURE(*స్థితి_2)) తిరిగి;

    U_ASSERT(ప్రధానసంఖ్య_సూచీ >= 0 && ప్రధానసంఖ్య_సూచీ < ప్రధానసంఖ్యలు_నిడివి);

    hash->ప్రధానసంఖ్య_సూచీ = static_cast<int8_t>(ప్రధానసంఖ్య_సూచీ);
    hash->length = ప్రధానసంఖ్యలు[ప్రధానసంఖ్య_సూచీ];

    p = hash->elements = static_cast<UHashElement*>(
        uprv_malloc(పరిమాణం(UHashElement) * hash->length));

    ఒకవేళ (hash->elements == శూన్యసూచిక) {
        *స్థితి_2 = U_జ్ఞాపకం_కేటాయింపు_ERROR;
        తిరిగి;
    }

    emptytok.సూచిక = శూన్యసూచిక; /* Only one of these two is needed */
    emptytok.integer = 0;    /* but we don't know which one. */

    హద్దు = p + hash->length;
    అయితే (p < హద్దు) {
        p->కీలకం = emptytok;
        p->value = emptytok;
        p->hashcode = HASH_ఖాళీలు;
        ++p;
    }

    hash->count = 0;
    hash->దిగువ_Water_Mark = static_cast<int32_t>(hash->length * hash->దిగువ_Water_నిష్పత్తి);
    hash->ఎగువ_Water_Mark = static_cast<int32_t>(hash->length * hash->ఎగువ_Water_నిష్పత్తి);
}

స్థిర UHashtable*
_uhash_init(UHashtable *ఫలితం,
              UHashFunction *కీలకం_Hash,
              UKeyComparator *కీలకం_Comp,
              UValueComparator *విలువ_Comp,
              int32_t ప్రధానసంఖ్య_సూచీ,
              UError_సంకేతం *స్థితి_2)
{
    ఒకవేళ (U_FAILURE(*స్థితి_2)) తిరిగి శూన్యసూచిక;
    U_ASSERT(కీలకం_Hash != శూన్యసూచిక);
    U_ASSERT(కీలకం_Comp != శూన్యసూచిక);

    ఫలితం->కీలకం_Hasher       = కీలకం_Hash;
    ఫలితం->కీలకం_Comparator   = కీలకం_Comp;
    ఫలితం->విలువ_Comparator = విలువ_Comp;
    ఫలితం->కీలకం_Deleter      = శూన్యసూచిక;
    ఫలితం->విలువ_Deleter    = శూన్యసూచిక;
    ఫలితం->allocated       = అబద్ధం;
    uhash_అంతర్గత_సమితి_Resize_Policy(ఫలితం, U_GROW);

    _uhash_allocate(ఫలితం, ప్రధానసంఖ్య_సూచీ, స్థితి_2);

    ఒకవేళ (U_FAILURE(*స్థితి_2)) {
        తిరిగి శూన్యసూచిక;
    }

    తిరిగి ఫలితం;
}

స్థిర UHashtable*
_uhash_create(UHashFunction *కీలకం_Hash,
              UKeyComparator *కీలకం_Comp,
              UValueComparator *విలువ_Comp,
              int32_t ప్రధానసంఖ్య_సూచీ,
              UError_సంకేతం *స్థితి_2) {
    UHashtable *ఫలితం;

    ఒకవేళ (U_FAILURE(*స్థితి_2)) తిరిగి శూన్యసూచిక;

    ఫలితం = static_cast<UHashtable*>(uprv_malloc(పరిమాణం(UHashtable)));
    ఒకవేళ (ఫలితం == శూన్యసూచిక) {
        *స్థితి_2 = U_జ్ఞాపకం_కేటాయింపు_ERROR;
        తిరిగి శూన్యసూచిక;
    }

    _uhash_init(ఫలితం, కీలకం_Hash, కీలకం_Comp, విలువ_Comp, ప్రధానసంఖ్య_సూచీ, స్థితి_2);
    ఫలితం->allocated       = నిజం;

    ఒకవేళ (U_FAILURE(*స్థితి_2)) {
        uprv_free(ఫలితం);
        తిరిగి శూన్యసూచిక;
    }

    తిరిగి ఫలితం;
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
స్థిర UHashElement*
uhash_కనుగొను_2(స్థిరరాశి UHashtable *hash, UHashTok కీలకం,
            int32_t hashcode) {

    int32_t మొదటి_Deleted = -1;  /* assume invalid index */
    int32_t the_సూచీ, ప్రారంభం_సూచీ;
    int32_t jump = 0; /* lazy evaluate */
    int32_t పట్టిక_Hash;
    UHashElement *elements = hash->elements;

    hashcode &= 0x7FFFFFFF; /* must be positive */
    ప్రారంభం_సూచీ = the_సూచీ = (hashcode ^ 0x4000000) % hash->length;

    చేయి {
        పట్టిక_Hash = elements[the_సూచీ].hashcode;
        ఒకవేళ (పట్టిక_Hash == hashcode) {          /* quick check */
            ఒకవేళ ((*hash->కీలకం_Comparator)(కీలకం, elements[the_సూచీ].కీలకం)) {
                తిరిగి &(elements[the_సూచీ]);
            }
        } లేకపోతే ఒకవేళ (!IS_ఖాళీలు_OR_DELETED(పట్టిక_Hash)) {
            /* We have hit a slot which contains a key-value pair,
             * but for which the hash code does not match.  Keep
             * looking.
             */
        } లేకపోతే ఒకవేళ (పట్టిక_Hash == HASH_ఖాళీలు) { /* empty, end o' the line */
            ఆపు;
        } లేకపోతే ఒకవేళ (మొదటి_Deleted < 0) { /* remember first deleted */
            మొదటి_Deleted = the_సూచీ;
        }
        ఒకవేళ (jump == 0) { /* lazy compute jump */
            /* The jump value must be relatively prime to the table
             * length.  As long as the length is prime, then any value
             * 1..length-1 will be relatively prime to it.
             */
            jump = (hashcode % (hash->length - 1)) + 1;
        }
        the_సూచీ = (the_సూచీ + jump) % hash->length;
    } అయితే (the_సూచీ != ప్రారంభం_సూచీ);

    ఒకవేళ (మొదటి_Deleted >= 0) {
        the_సూచీ = మొదటి_Deleted; /* reset if had deleted slot */
    } లేకపోతే ఒకవేళ (పట్టిక_Hash != HASH_ఖాళీలు) {
        /* We get to this point if the hashtable is full (no empty or
         * deleted slots), and we've failed to find a match.  THIS
         * WILL NEVER HAPPEN as long as uhash_put() makes sure that
         * count is always < length.
         */
        UPRV_UNREACHABLE_EXIT;
    }
    తిరిగి &(elements[the_సూచీ]);
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
స్థిర శూన్య
_uhash_rehash(UHashtable *hash, UError_సంకేతం *స్థితి_2) {

    UHashElement *old = hash->elements;
    int32_t old_నిడివి = hash->length;
    int32_t new_ప్రధానసంఖ్య_సూచీ = hash->ప్రధానసంఖ్య_సూచీ;
    int32_t i;

    ఒకవేళ (hash->count > hash->ఎగువ_Water_Mark) {
        ఒకవేళ (++new_ప్రధానసంఖ్య_సూచీ >= ప్రధానసంఖ్యలు_నిడివి) {
            తిరిగి;
        }
    } లేకపోతే ఒకవేళ (hash->count < hash->దిగువ_Water_Mark) {
        ఒకవేళ (--new_ప్రధానసంఖ్య_సూచీ < 0) {
            తిరిగి;
        }
    } లేకపోతే {
        తిరిగి;
    }

    _uhash_allocate(hash, new_ప్రధానసంఖ్య_సూచీ, స్థితి_2);

    ఒకవేళ (U_FAILURE(*స్థితి_2)) {
        hash->elements = old;
        hash->length = old_నిడివి;
        తిరిగి;
    }

    కొరకు (i = old_నిడివి - 1; i >= 0; --i) {
        ఒకవేళ (!IS_ఖాళీలు_OR_DELETED(old[i].hashcode)) {
            UHashElement *e = uhash_కనుగొను_2(hash, old[i].కీలకం, old[i].hashcode);
            U_ASSERT(e != శూన్యసూచిక);
            U_ASSERT(e->hashcode == HASH_ఖాళీలు);
            e->కీలకం = old[i].కీలకం;
            e->value = old[i].value;
            e->hashcode = old[i].hashcode;
            ++hash->count;
        }
    }

    uprv_free(old);
}

స్థిర UHashTok
_uhash_remove(UHashtable *hash,
              UHashTok కీలకం) {
    /* First find the position of the key in the table.  If the object
     * has not been removed already, remove it.  If the user wanted
     * keys deleted, then delete it also.  We have to put a special
     * hashcode in that position that means that something has been
     * deleted, since when we do a find, we have to continue PAST any
     * deleted values.
     */
    UHashTok ఫలితం;
    UHashElement* e = uhash_కనుగొను_2(hash, కీలకం, hash->కీలకం_Hasher(కీలకం));
    U_ASSERT(e != శూన్యసూచిక);
    ఫలితం.సూచిక = శూన్యసూచిక;
    ఫలితం.integer = 0;
    ఒకవేళ (!IS_ఖాళీలు_OR_DELETED(e->hashcode)) {
        ఫలితం = uhash_అంతర్గత_Remove_Element(hash, e);
        ఒకవేళ (hash->count < hash->దిగువ_Water_Mark) {
            UError_సంకేతం స్థితి_2 = U_ZERO_ERROR;
            _uhash_rehash(hash, &స్థితి_2);
        }
    }
    తిరిగి ఫలితం;
}

స్థిర UHashTok
_uhash_put(UHashtable *hash,
           UHashTok కీలకం,
           UHashTok value,
           int8_t hint,
           UError_సంకేతం *స్థితి_2) {

    /* Put finds the position in the table for the new value.  If the
     * key is already in the table, it is deleted, if there is a
     * non-nullptr keyDeleter.  Then the key, the hash and the value are
     * all put at the position in their respective arrays.
     */
    int32_t hashcode;
    UHashElement* e;
    UHashTok emptytok;

    ఒకవేళ (U_FAILURE(*స్థితి_2)) {
        వెళ్ళు err;
    }
    U_ASSERT(hash != శూన్యసూచిక);
    ఒకవేళ ((hint & HINT_విలువ_సూచిక) ?
            value.సూచిక == శూన్యసూచిక :
            value.integer == 0 && (hint & HINT_ALLOW_ZERO) == 0) {
        /* Disallow storage of nullptr values, since nullptr is returned by
         * get() to indicate an absent key.  Storing nullptr == removing.
         */
        తిరిగి _uhash_remove(hash, కీలకం);
    }
    ఒకవేళ (hash->count > hash->ఎగువ_Water_Mark) {
        _uhash_rehash(hash, స్థితి_2);
        ఒకవేళ (U_FAILURE(*స్థితి_2)) {
            వెళ్ళు err;
        }
    }

    hashcode = (*hash->కీలకం_Hasher)(కీలకం);
    e = uhash_కనుగొను_2(hash, కీలకం, hashcode);
    U_ASSERT(e != శూన్యసూచిక);

    ఒకవేళ (IS_ఖాళీలు_OR_DELETED(e->hashcode)) {
        /* Important: We must never actually fill the table up.  If we
         * do so, then _uhash_find() will return nullptr, and we'll have
         * to check for nullptr after every call to _uhash_find().  To
         * avoid this we make sure there is always at least one empty
         * or deleted slot in the table.  This only is a problem if we
         * are out of memory and rehash isn't working.
         */
        ++hash->count;
        ఒకవేళ (hash->count == hash->length) {
            /* Don't allow count to reach length */
            --hash->count;
            *స్థితి_2 = U_జ్ఞాపకం_కేటాయింపు_ERROR;
            వెళ్ళు err;
        }
    }

    /* We must in all cases handle storage properly.  If there was an
     * old key, then it must be deleted (if the deleter != nullptr).
     * Make hashcodes stored in table positive.
     */
    తిరిగి uhash_సమితి_Element(hash, e, hashcode & 0x7FFFFFFF, కీలకం, value, hint);

 err:
    /* If the deleters are non-nullptr, this method adopts its key and/or
     * value arguments, and we must be sure to delete the key and/or
     * value in all cases, even upon failure.
     */
    HASH_DELETE_కీలకం_విలువ(hash, కీలకం.సూచిక, value.సూచిక);
    emptytok.సూచిక = శూన్యసూచిక; emptytok.integer = 0;
    తిరిగి emptytok;
}


/********************************************************************
 * PUBLIC API
 ********************************************************************/

U_CAPI UHashtable* U_EXPORT2
uhash_open(UHashFunction *కీలకం_Hash,
           UKeyComparator *కీలకం_Comp,
           UValueComparator *విలువ_Comp,
           UError_సంకేతం *స్థితి_2) {

    తిరిగి _uhash_create(కీలకం_Hash, కీలకం_Comp, విలువ_Comp, DEFAULT_ప్రధానసంఖ్య_సూచీ, స్థితి_2);
}

U_CAPI UHashtable* U_EXPORT2
uhash_open_కొలత(UHashFunction *కీలకం_Hash,
               UKeyComparator *కీలకం_Comp,
               UValueComparator *విలువ_Comp,
               int32_t size,
               UError_సంకేతం *స్థితి_2) {

    /* Find the smallest index i for which PRIMES[i] >= size. */
    int32_t i = 0;
    అయితే (i<(ప్రధానసంఖ్యలు_నిడివి-1) && ప్రధానసంఖ్యలు[i]<size) {
        ++i;
    }

    తిరిగి _uhash_create(కీలకం_Hash, కీలకం_Comp, విలువ_Comp, i, స్థితి_2);
}

U_CAPI UHashtable* U_EXPORT2
uhash_init(UHashtable *fillin_ఫలితం,
           UHashFunction *కీలకం_Hash,
           UKeyComparator *కీలకం_Comp,
           UValueComparator *విలువ_Comp,
           UError_సంకేతం *స్థితి_2) {

    తిరిగి _uhash_init(fillin_ఫలితం, కీలకం_Hash, కీలకం_Comp, విలువ_Comp, DEFAULT_ప్రధానసంఖ్య_సూచీ, స్థితి_2);
}

U_CAPI UHashtable* U_EXPORT2
uhash_init_కొలత(UHashtable *fillin_ఫలితం,
               UHashFunction *కీలకం_Hash,
               UKeyComparator *కీలకం_Comp,
               UValueComparator *విలువ_Comp,
               int32_t size,
               UError_సంకేతం *స్థితి_2) {

    // Find the smallest index i for which PRIMES[i] >= size.
    int32_t i = 0;
    అయితే (i<(ప్రధానసంఖ్యలు_నిడివి-1) && ప్రధానసంఖ్యలు[i]<size) {
        ++i;
    }
    తిరిగి _uhash_init(fillin_ఫలితం, కీలకం_Hash, కీలకం_Comp, విలువ_Comp, i, స్థితి_2);
}

U_CAPI శూన్య U_EXPORT2
uhash_close(UHashtable *hash) {
    ఒకవేళ (hash == శూన్యసూచిక) {
        తిరిగి;
    }
    ఒకవేళ (hash->elements != శూన్యసూచిక) {
        ఒకవేళ (hash->కీలకం_Deleter != శూన్యసూచిక || hash->విలువ_Deleter != శూన్యసూచిక) {
            int32_t pos=UHASH_మొదటి;
            UHashElement *e;
            అయితే ((e = (UHashElement*) uhash_తదుపరి_Element(hash, &pos)) != శూన్యసూచిక) {
                HASH_DELETE_కీలకం_విలువ(hash, e->కీలకం.సూచిక, e->value.సూచిక);
            }
        }
        uprv_free(hash->elements);
        hash->elements = శూన్యసూచిక;
    }
    ఒకవేళ (hash->allocated) {
        uprv_free(hash);
    }
}

U_CAPI UHashFunction *U_EXPORT2
uhash_సమితి_కీలకం_Hasher(UHashtable *hash, UHashFunction *fn) {
    UHashFunction *ఫలితం = hash->కీలకం_Hasher;
    hash->కీలకం_Hasher = fn;
    తిరిగి ఫలితం;
}

U_CAPI UKeyComparator *U_EXPORT2
uhash_సమితి_కీలకం_Comparator(UHashtable *hash, UKeyComparator *fn) {
    UKeyComparator *ఫలితం = hash->కీలకం_Comparator;
    hash->కీలకం_Comparator = fn;
    తిరిగి ఫలితం;
}
U_CAPI UValueComparator *U_EXPORT2
uhash_సమితి_విలువ_Comparator(UHashtable *hash, UValueComparator *fn){
    UValueComparator *ఫలితం = hash->విలువ_Comparator;
    hash->విలువ_Comparator = fn;
    తిరిగి ఫలితం;
}

U_CAPI UObjectDeleter *U_EXPORT2
uhash_సమితి_కీలకం_Deleter(UHashtable *hash, UObjectDeleter *fn) {
    UObjectDeleter *ఫలితం = hash->కీలకం_Deleter;
    hash->కీలకం_Deleter = fn;
    తిరిగి ఫలితం;
}

U_CAPI UObjectDeleter *U_EXPORT2
uhash_సమితి_విలువ_Deleter(UHashtable *hash, UObjectDeleter *fn) {
    UObjectDeleter *ఫలితం = hash->విలువ_Deleter;
    hash->విలువ_Deleter = fn;
    తిరిగి ఫలితం;
}

U_CAPI శూన్య U_EXPORT2
uhash_సమితి_Resize_Policy(UHashtable *hash, గణన UHashResizePolicy policy) {
    UError_సంకేతం స్థితి_2 = U_ZERO_ERROR;
    uhash_అంతర్గత_సమితి_Resize_Policy(hash, policy);
    hash->దిగువ_Water_Mark  = (int32_t)(hash->length * hash->దిగువ_Water_నిష్పత్తి);
    hash->ఎగువ_Water_Mark = (int32_t)(hash->length * hash->ఎగువ_Water_నిష్పత్తి);
    _uhash_rehash(hash, &స్థితి_2);
}

U_CAPI int32_t U_EXPORT2
uhash_లెక్క(స్థిరరాశి UHashtable *hash) {
    తిరిగి hash->count;
}

U_CAPI శూన్య* U_EXPORT2
uhash_get(స్థిరరాశి UHashtable *hash,
          స్థిరరాశి శూన్య* కీలకం) {
    UHashTok keyholder;
    keyholder.సూచిక = (శూన్య*) కీలకం;
    తిరిగి uhash_కనుగొను_2(hash, keyholder, hash->కీలకం_Hasher(keyholder))->value.సూచిక;
}

U_CAPI శూన్య* U_EXPORT2
uhash_iget(స్థిరరాశి UHashtable *hash,
           int32_t కీలకం) {
    UHashTok keyholder;
    keyholder.integer = కీలకం;
    తిరిగి uhash_కనుగొను_2(hash, keyholder, hash->కీలకం_Hasher(keyholder))->value.సూచిక;
}

U_CAPI int32_t U_EXPORT2
uhash_geti(స్థిరరాశి UHashtable *hash,
           స్థిరరాశి శూన్య* కీలకం) {
    UHashTok keyholder;
    keyholder.సూచిక = (శూన్య*) కీలకం;
    తిరిగి uhash_కనుగొను_2(hash, keyholder, hash->కీలకం_Hasher(keyholder))->value.integer;
}

U_CAPI int32_t U_EXPORT2
uhash_igeti(స్థిరరాశి UHashtable *hash,
           int32_t కీలకం) {
    UHashTok keyholder;
    keyholder.integer = కీలకం;
    తిరిగి uhash_కనుగొను_2(hash, keyholder, hash->కీలకం_Hasher(keyholder))->value.integer;
}

U_CAPI int32_t U_EXPORT2
uhash_geti_And_దొరికింది(స్థిరరాశి UHashtable *hash,
                   స్థిరరాశి శూన్య *కీలకం,
                   UBool *దొరికింది) {
    UHashTok keyholder;
    keyholder.సూచిక = (శూన్య *)కీలకం;
    స్థిరరాశి UHashElement *e = uhash_కనుగొను_2(hash, keyholder, hash->కీలకం_Hasher(keyholder));
    *దొరికింది = !IS_ఖాళీలు_OR_DELETED(e->hashcode);
    తిరిగి e->value.integer;
}

U_CAPI int32_t U_EXPORT2
uhash_igeti_And_దొరికింది(స్థిరరాశి UHashtable *hash,
                    int32_t కీలకం,
                    UBool *దొరికింది) {
    UHashTok keyholder;
    keyholder.integer = కీలకం;
    స్థిరరాశి UHashElement *e = uhash_కనుగొను_2(hash, keyholder, hash->కీలకం_Hasher(keyholder));
    *దొరికింది = !IS_ఖాళీలు_OR_DELETED(e->hashcode);
    తిరిగి e->value.integer;
}

U_CAPI శూన్య* U_EXPORT2
uhash_put(UHashtable *hash,
          శూన్య* కీలకం,
          శూన్య* value,
          UError_సంకేతం *స్థితి_2) {
    UHashTok keyholder, valueholder;
    keyholder.సూచిక = కీలకం;
    valueholder.సూచిక = value;
    తిరిగి _uhash_put(hash, keyholder, valueholder,
                      HINT_కీలకం_సూచిక | HINT_విలువ_సూచిక,
                      స్థితి_2).సూచిక;
}

U_CAPI శూన్య* U_EXPORT2
uhash_iput(UHashtable *hash,
           int32_t కీలకం,
           శూన్య* value,
           UError_సంకేతం *స్థితి_2) {
    UHashTok keyholder, valueholder;
    keyholder.integer = కీలకం;
    valueholder.సూచిక = value;
    తిరిగి _uhash_put(hash, keyholder, valueholder,
                      HINT_విలువ_సూచిక,
                      స్థితి_2).సూచిక;
}

U_CAPI int32_t U_EXPORT2
uhash_puti(UHashtable *hash,
           శూన్య* కీలకం,
           int32_t value,
           UError_సంకేతం *స్థితి_2) {
    UHashTok keyholder, valueholder;
    keyholder.సూచిక = కీలకం;
    valueholder.integer = value;
    తిరిగి _uhash_put(hash, keyholder, valueholder,
                      HINT_కీలకం_సూచిక,
                      స్థితి_2).integer;
}


U_CAPI int32_t U_EXPORT2
uhash_iputi(UHashtable *hash,
           int32_t కీలకం,
           int32_t value,
           UError_సంకేతం *స్థితి_2) {
    UHashTok keyholder, valueholder;
    keyholder.integer = కీలకం;
    valueholder.integer = value;
    తిరిగి _uhash_put(hash, keyholder, valueholder,
                      HINT_BOTH_INTEGERS,
                      స్థితి_2).integer;
}

U_CAPI int32_t U_EXPORT2
uhash_putiAllowZero(UHashtable *hash,
                    శూన్య *కీలకం,
                    int32_t value,
                    UError_సంకేతం *స్థితి_2) {
    UHashTok keyholder, valueholder;
    keyholder.సూచిక = కీలకం;
    valueholder.integer = value;
    తిరిగి _uhash_put(hash, keyholder, valueholder,
                      HINT_కీలకం_సూచిక | HINT_ALLOW_ZERO,
                      స్థితి_2).integer;
}


U_CAPI int32_t U_EXPORT2
uhash_iputiAllowZero(UHashtable *hash,
                     int32_t కీలకం,
                     int32_t value,
                     UError_సంకేతం *స్థితి_2) {
    UHashTok keyholder, valueholder;
    keyholder.integer = కీలకం;
    valueholder.integer = value;
    తిరిగి _uhash_put(hash, keyholder, valueholder,
                      HINT_BOTH_INTEGERS | HINT_ALLOW_ZERO,
                      స్థితి_2).integer;
}

U_CAPI శూన్య* U_EXPORT2
uhash_remove(UHashtable *hash,
             స్థిరరాశి శూన్య* కీలకం) {
    UHashTok keyholder;
    keyholder.సూచిక = (శూన్య*) కీలకం;
    తిరిగి _uhash_remove(hash, keyholder).సూచిక;
}

U_CAPI శూన్య* U_EXPORT2
uhash_iremove(UHashtable *hash,
              int32_t కీలకం) {
    UHashTok keyholder;
    keyholder.integer = కీలకం;
    తిరిగి _uhash_remove(hash, keyholder).సూచిక;
}

U_CAPI int32_t U_EXPORT2
uhash_removei(UHashtable *hash,
              స్థిరరాశి శూన్య* కీలకం) {
    UHashTok keyholder;
    keyholder.సూచిక = (శూన్య*) కీలకం;
    తిరిగి _uhash_remove(hash, keyholder).integer;
}

U_CAPI int32_t U_EXPORT2
uhash_iremovei(UHashtable *hash,
               int32_t కీలకం) {
    UHashTok keyholder;
    keyholder.integer = కీలకం;
    తిరిగి _uhash_remove(hash, keyholder).integer;
}

U_CAPI శూన్య U_EXPORT2
uhash_removeAll(UHashtable *hash) {
    int32_t pos = UHASH_మొదటి;
    స్థిరరాశి UHashElement *e;
    U_ASSERT(hash != శూన్యసూచిక);
    ఒకవేళ (hash->count != 0) {
        అయితే ((e = uhash_తదుపరి_Element(hash, &pos)) != శూన్యసూచిక) {
            uhash_removeElement(hash, e);
        }
    }
    U_ASSERT(hash->count == 0);
}

U_CAPI UBool U_EXPORT2
uhash_కలిగియుంది_కీలకం(స్థిరరాశి UHashtable *hash, స్థిరరాశి శూన్య *కీలకం) {
    UHashTok keyholder;
    keyholder.సూచిక = (శూన్య *)కీలకం;
    స్థిరరాశి UHashElement *e = uhash_కనుగొను_2(hash, keyholder, hash->కీలకం_Hasher(keyholder));
    తిరిగి !IS_ఖాళీలు_OR_DELETED(e->hashcode);
}

/**
 * Returns true if the UHashtable contains an item with this integer key.
 *
 * @param hash The target UHashtable.
 * @param key An integer key stored in a hashtable
 * @return true if the key is found.
 */
U_CAPI UBool U_EXPORT2
uhash_icontains_కీలకం(స్థిరరాశి UHashtable *hash, int32_t కీలకం) {
    UHashTok keyholder;
    keyholder.integer = కీలకం;
    స్థిరరాశి UHashElement *e = uhash_కనుగొను_2(hash, keyholder, hash->కీలకం_Hasher(keyholder));
    తిరిగి !IS_ఖాళీలు_OR_DELETED(e->hashcode);
}

U_CAPI స్థిరరాశి UHashElement* U_EXPORT2
uhash_కనుగొను(స్థిరరాశి UHashtable *hash, స్థిరరాశి శూన్య* కీలకం) {
    UHashTok keyholder;
    స్థిరరాశి UHashElement *e;
    keyholder.సూచిక = (శూన్య*) కీలకం;
    e = uhash_కనుగొను_2(hash, keyholder, hash->కీలకం_Hasher(keyholder));
    తిరిగి IS_ఖాళీలు_OR_DELETED(e->hashcode) ? శూన్యసూచిక : e;
}

U_CAPI స్థిరరాశి UHashElement* U_EXPORT2
uhash_తదుపరి_Element(స్థిరరాశి UHashtable *hash, int32_t *pos) {
    /* Walk through the array until we find an element that is not
     * EMPTY and not DELETED.
     */
    int32_t i;
    U_ASSERT(hash != శూన్యసూచిక);
    కొరకు (i = *pos + 1; i < hash->length; ++i) {
        ఒకవేళ (!IS_ఖాళీలు_OR_DELETED(hash->elements[i].hashcode)) {
            *pos = i;
            తిరిగి &(hash->elements[i]);
        }
    }

    /* No more elements */
    తిరిగి శూన్యసూచిక;
}

U_CAPI శూన్య* U_EXPORT2
uhash_removeElement(UHashtable *hash, స్థిరరాశి UHashElement* e) {
    U_ASSERT(hash != శూన్యసూచిక);
    U_ASSERT(e != శూన్యసూచిక);
    ఒకవేళ (!IS_ఖాళీలు_OR_DELETED(e->hashcode)) {
        UHashElement *nce = (UHashElement *)e;
        తిరిగి uhash_అంతర్గత_Remove_Element(hash, nce).సూచిక;
    }
    తిరిగి శూన్యసూచిక;
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
uhash_hashUChars(స్థిరరాశి UHashTok కీలకం) {
    స్థిరరాశి char16_t *s = (స్థిరరాశి char16_t *)కీలకం.సూచిక;
    తిరిగి s == శూన్యసూచిక ? 0 : ustr_hashUCharsN(s, u_strlen(s));
}

U_CAPI int32_t U_EXPORT2
uhash_hashChars(స్థిరరాశి UHashTok కీలకం) {
    స్థిరరాశి అక్షరం *s = (స్థిరరాశి అక్షరం *)కీలకం.సూచిక;
    తిరిగి s == శూన్యసూచిక ? 0 : static_cast<int32_t>(ustr_hashCharsN(s, static_cast<int32_t>(uprv_strlen(s))));
}

U_CAPI int32_t U_EXPORT2
uhash_hashIChars(స్థిరరాశి UHashTok కీలకం) {
    స్థిరరాశి అక్షరం *s = (స్థిరరాశి అక్షరం *)కీలకం.సూచిక;
    తిరిగి s == శూన్యసూచిక ? 0 : ustr_hashICharsN(s, static_cast<int32_t>(uprv_strlen(s)));
}

U_CAPI int32_t U_EXPORT2
uhash_hashIStringView(స్థిరరాశి UHashTok కీలకం) {
    స్థిరరాశి std::string_view* s = static_cast<std::string_view*>(కీలకం.సూచిక);
    తిరిగి s == శూన్యసూచిక ? 0 : ustr_hashICharsN(s->దత్తాంశం(), static_cast<int32_t>(s->size()));
}

U_CAPI UBool U_EXPORT2
uhash_equals(స్థిరరాశి UHashtable* hash1, స్థిరరాశి UHashtable* hash2){
    int32_t count1, count2, pos, i;

    ఒకవేళ(hash1==hash2){
        తిరిగి నిజం;
    }

    /*
     * Make sure that we are comparing 2 valid hashes of the same type
     * with valid comparison functions.
     * Without valid comparison functions, a binary comparison
     * of the hash values will yield random results on machines
     * with 64-bit pointers and 32-bit integer hashes.
     * A valueComparator is normally optional.
     */
    ఒకవేళ (hash1==శూన్యసూచిక || hash2==శూన్యసూచిక ||
        hash1->కీలకం_Comparator != hash2->కీలకం_Comparator ||
        hash1->విలువ_Comparator != hash2->విలువ_Comparator ||
        hash1->విలువ_Comparator == శూన్యసూచిక)
    {
        /*
        Normally we would return an error here about incompatible hash tables,
        but we return false instead.
        */
        తిరిగి అబద్ధం;
    }

    count1 = uhash_లెక్క(hash1);
    count2 = uhash_లెక్క(hash2);
    ఒకవేళ(count1!=count2){
        తిరిగి అబద్ధం;
    }

    pos=UHASH_మొదటి;
    కొరకు(i=0; i<count1; i++){
        స్థిరరాశి UHashElement* elem1 = uhash_తదుపరి_Element(hash1, &pos);
        స్థిరరాశి UHashTok key1 = elem1->కీలకం;
        స్థిరరాశి UHashTok val1 = elem1->value;
        /* here the keys are not compared, instead the key form hash1 is used to fetch
         * value from hash2. If the hashes are equal then then both hashes should
         * contain equal values for the same key!
         */
        స్థిరరాశి UHashElement* elem2 = uhash_కనుగొను_2(hash2, key1, hash2->కీలకం_Hasher(key1));
        స్థిరరాశి UHashTok val2 = elem2->value;
        ఒకవేళ(hash1->విలువ_Comparator(val1, val2)==అబద్ధం){
            తిరిగి అబద్ధం;
        }
    }
    తిరిగి నిజం;
}

/********************************************************************
 * PUBLIC Comparator Functions
 ********************************************************************/

U_CAPI UBool U_EXPORT2
uhash_compareUChars(స్థిరరాశి UHashTok key1, స్థిరరాశి UHashTok key2) {
    స్థిరరాశి char16_t *p1 = (స్థిరరాశి char16_t*) key1.సూచిక;
    స్థిరరాశి char16_t *p2 = (స్థిరరాశి char16_t*) key2.సూచిక;
    ఒకవేళ (p1 == p2) {
        తిరిగి నిజం;
    }
    ఒకవేళ (p1 == శూన్యసూచిక || p2 == శూన్యసూచిక) {
        తిరిగి అబద్ధం;
    }
    అయితే (*p1 != 0 && *p1 == *p2) {
        ++p1;
        ++p2;
    }
    తిరిగి *p1 == *p2;
}

U_CAPI UBool U_EXPORT2
uhash_compareChars(స్థిరరాశి UHashTok key1, స్థిరరాశి UHashTok key2) {
    స్థిరరాశి అక్షరం *p1 = (స్థిరరాశి అక్షరం*) key1.సూచిక;
    స్థిరరాశి అక్షరం *p2 = (స్థిరరాశి అక్షరం*) key2.సూచిక;
    ఒకవేళ (p1 == p2) {
        తిరిగి నిజం;
    }
    ఒకవేళ (p1 == శూన్యసూచిక || p2 == శూన్యసూచిక) {
        తిరిగి అబద్ధం;
    }
    అయితే (*p1 != 0 && *p1 == *p2) {
        ++p1;
        ++p2;
    }
    తిరిగి *p1 == *p2;
}

U_CAPI UBool U_EXPORT2
uhash_compareIChars(స్థిరరాశి UHashTok key1, స్థిరరాశి UHashTok key2) {
    స్థిరరాశి అక్షరం *p1 = (స్థిరరాశి అక్షరం*) key1.సూచిక;
    స్థిరరాశి అక్షరం *p2 = (స్థిరరాశి అక్షరం*) key2.సూచిక;
    ఒకవేళ (p1 == p2) {
        తిరిగి నిజం;
    }
    ఒకవేళ (p1 == శూన్యసూచిక || p2 == శూన్యసూచిక) {
        తిరిగి అబద్ధం;
    }
    అయితే (*p1 != 0 && uprv_tolower(*p1) == uprv_tolower(*p2)) {
        ++p1;
        ++p2;
    }
    తిరిగి *p1 == *p2;
}

U_CAPI UBool U_EXPORT2
uhash_compareIStringView(స్థిరరాశి UHashTok key1, స్థిరరాశి UHashTok key2) {
    స్థిరరాశి std::string_view* p1 = static_cast<std::string_view*>(key1.సూచిక);
    స్థిరరాశి std::string_view* p2 = static_cast<std::string_view*>(key2.సూచిక);
    ఒకవేళ (p1 == p2) {
        తిరిగి నిజం;
    }
    ఒకవేళ (p1 == శూన్యసూచిక || p2 == శూన్యసూచిక) {
        తిరిగి అబద్ధం;
    }
    స్థిరరాశి std::string_view& v1 = *p1;
    స్థిరరాశి std::string_view& v2 = *p2;
    ఒకవేళ (v1.size() != v2.size()) {
        తిరిగి అబద్ధం;
    }
    కొరకు (size_t i = 0; i < v1.size(); ++i) {
        ఒకవేళ (uprv_tolower(v1[i]) != uprv_tolower(v2[i])) {
            తిరిగి అబద్ధం;
        }
    }
    తిరిగి నిజం;
}

/********************************************************************
 * PUBLIC int32_t Support Functions
 ********************************************************************/

U_CAPI int32_t U_EXPORT2
uhash_hashLong(స్థిరరాశి UHashTok కీలకం) {
    తిరిగి కీలకం.integer;
}

U_CAPI UBool U_EXPORT2
uhash_compareLong(స్థిరరాశి UHashTok key1, స్థిరరాశి UHashTok key2) {
    తిరిగి key1.integer == key2.integer;
}
