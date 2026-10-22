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

#ضمن <string_view>

#ضمن "uhash.h"
#ضمن "unicode/ustring.h"
#ضمن "cstring.h"
#ضمن "cmemory.h"
#ضمن "uassert.h"
#ضمن "ustr_imp.h"

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
ساكن ثابت int32_t مفرد_انگ[] = {
    7, 13, 31, 61, 127, 251, 509, 1021, 2039, 4093, 8191, 16381, 32749,
    65521, 131071, 262139, 524287, 1048573, 2097143, 4194301, 8388593,
    16777213, 33554393, 67108859, 134217689, 268435399, 536870909,
    1073741789, 2147483647 /*, 4294967291 */
};

#وصف_ڪريو مفرد_انگ_ڊيگهائي UPRV_LENGTHOF(مفرد_انگ)
#وصف_ڪريو DEFAULT_مفرد_اشاريو 4

/* These ratios are tuned to the PRIMES array such that a resize
 * places the table back into the zone of non-resizing.  That is,
 * after a call to _uhash_rehash(), a subsequent call to
 * _uhash_rehash() should do nothing (should not churn).  This is only
 * a potential problem with U_GROW_AND_SHRINK.
 */
ساكن ثابت عائم RESIZE_POLICY_تناسب_جدول[6] = {
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
#وصف_ڪريو HASH_DELETED    ((int32_t) 0x80000000)
#وصف_ڪريو HASH_خالي_جايون      ((int32_t) HASH_DELETED + 1)

#وصف_ڪريو IS_خالي_جايون_OR_DELETED(x) ((x) < 0)

/* This macro expects a UHashTok.pointer as its keypointer and
   valuepointer parameters */
#وصف_ڪريو HASH_DELETE_ڪنجي_قدر(hash, keypointer, valuepointer) UPRV_بلاڪ_MACRO_BEGIN { \
    إذا (hash->ڪنجي_Deleter != خالي_اشارو && keypointer != خالي_اشارو) { \
        (*hash->ڪنجي_Deleter)(keypointer); \
    } \
    إذا (hash->قدر_Deleter != خالي_اشارو && valuepointer != خالي_اشارو) { \
        (*hash->قدر_Deleter)(valuepointer); \
    } \
} UPRV_بلاڪ_MACRO_پڇاڙي

/*
 * Constants for hinting whether a key or value is an integer
 * or a pointer.  If a hint bit is zero, then the associated
 * token is assumed to be an integer.
 */
#وصف_ڪريو HINT_BOTH_INTEGERS (0)
#وصف_ڪريو HINT_ڪنجي_اشارو   (1)
#وصف_ڪريو HINT_قدر_اشارو (2)
#وصف_ڪريو HINT_ALLOW_ZERO    (4)

/********************************************************************
 * PRIVATE Implementation
 ********************************************************************/

ساكن UHashTok
uhash_سيٽ_Element(UHashtable *hash, UHashElement* e,
                  int32_t hashcode,
                  UHashTok ڪنجي, UHashTok value, int8_t hint) {

    UHashTok old_قدر = e->value;
    إذا (hash->ڪنجي_Deleter != خالي_اشارو && e->ڪنجي.اشارو != خالي_اشارو &&
        e->ڪنجي.اشارو != ڪنجي.اشارو) { /* Avoid double deletion */
        (*hash->ڪنجي_Deleter)(e->ڪنجي.اشارو);
    }
    إذا (hash->قدر_Deleter != خالي_اشارو) {
        إذا (old_قدر.اشارو != خالي_اشارو &&
            old_قدر.اشارو != value.اشارو) { /* Avoid double deletion */
            (*hash->قدر_Deleter)(old_قدر.اشارو);
        }
        old_قدر.اشارو = خالي_اشارو;
    }
    /* Compilers should copy the UHashTok union correctly, but even if
     * they do, memory heap tools (e.g. BoundsChecker) can get
     * confused when a pointer is cloaked in a union and then copied.
     * TO ALLEVIATE THIS, we use hints (based on what API the user is
     * calling) to copy pointers when we know the user thinks
     * something is a pointer. */
    إذا (hint & HINT_ڪنجي_اشارو) {
        e->ڪنجي.اشارو = ڪنجي.اشارو;
    } وإلا {
        e->ڪنجي = ڪنجي;
    }
    إذا (hint & HINT_قدر_اشارو) {
        e->value.اشارو = value.اشارو;
    } وإلا {
        e->value = value;
    }
    e->hashcode = hashcode;
    أرجع old_قدر;
}

/**
 * Assumes that the given element is not empty or deleted.
 */
ساكن UHashTok
uhash_اندروني_Remove_Element(UHashtable *hash, UHashElement* e) {
    UHashTok empty;
    U_ASSERT(!IS_خالي_جايون_OR_DELETED(e->hashcode));
    --hash->count;
    empty.اشارو = خالي_اشارو; empty.integer = 0;
    أرجع uhash_سيٽ_Element(hash, e, HASH_DELETED, empty, empty, 0);
}

ساكن فراغ
uhash_اندروني_سيٽ_Resize_Policy(UHashtable *hash, تعداد UHashResizePolicy policy) {
    U_ASSERT(hash != خالي_اشارو);
    U_ASSERT(((int32_t)policy) >= 0);
    U_ASSERT(((int32_t)policy) < 3);
    hash->هيٺيون_Water_تناسب  = RESIZE_POLICY_تناسب_جدول[policy * 2];
    hash->مٿاهون_Water_تناسب = RESIZE_POLICY_تناسب_جدول[policy * 2 + 1];
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
ساكن فراغ
_uhash_allocate(UHashtable *hash,
                int32_t مفرد_اشاريو,
                UError_ڪوڊ *حالت) {

    UHashElement *p, *حد_بندي;
    UHashTok emptytok;

    إذا (U_FAILURE(*حالت)) أرجع;

    U_ASSERT(مفرد_اشاريو >= 0 && مفرد_اشاريو < مفرد_انگ_ڊيگهائي);

    hash->مفرد_اشاريو = static_cast<int8_t>(مفرد_اشاريو);
    hash->length = مفرد_انگ[مفرد_اشاريو];

    p = hash->elements = static_cast<UHashElement*>(
        uprv_malloc(حجم(UHashElement) * hash->length));

    إذا (hash->elements == خالي_اشارو) {
        *حالت = U_يادگيري_ورهاست_ERROR;
        أرجع;
    }

    emptytok.اشارو = خالي_اشارو; /* Only one of these two is needed */
    emptytok.integer = 0;    /* but we don't know which one. */

    حد_بندي = p + hash->length;
    طالما (p < حد_بندي) {
        p->ڪنجي = emptytok;
        p->value = emptytok;
        p->hashcode = HASH_خالي_جايون;
        ++p;
    }

    hash->count = 0;
    hash->هيٺيون_Water_Mark = static_cast<int32_t>(hash->length * hash->هيٺيون_Water_تناسب);
    hash->مٿاهون_Water_Mark = static_cast<int32_t>(hash->length * hash->مٿاهون_Water_تناسب);
}

ساكن UHashtable*
_uhash_init(UHashtable *نتيجو,
              UHashFunction *ڪنجي_Hash,
              UKeyComparator *ڪنجي_Comp,
              UValueComparator *قدر_Comp,
              int32_t مفرد_اشاريو,
              UError_ڪوڊ *حالت)
{
    إذا (U_FAILURE(*حالت)) أرجع خالي_اشارو;
    U_ASSERT(ڪنجي_Hash != خالي_اشارو);
    U_ASSERT(ڪنجي_Comp != خالي_اشارو);

    نتيجو->ڪنجي_Hasher       = ڪنجي_Hash;
    نتيجو->ڪنجي_Comparator   = ڪنجي_Comp;
    نتيجو->قدر_Comparator = قدر_Comp;
    نتيجو->ڪنجي_Deleter      = خالي_اشارو;
    نتيجو->قدر_Deleter    = خالي_اشارو;
    نتيجو->allocated       = خطأ;
    uhash_اندروني_سيٽ_Resize_Policy(نتيجو, U_GROW);

    _uhash_allocate(نتيجو, مفرد_اشاريو, حالت);

    إذا (U_FAILURE(*حالت)) {
        أرجع خالي_اشارو;
    }

    أرجع نتيجو;
}

ساكن UHashtable*
_uhash_create(UHashFunction *ڪنجي_Hash,
              UKeyComparator *ڪنجي_Comp,
              UValueComparator *قدر_Comp,
              int32_t مفرد_اشاريو,
              UError_ڪوڊ *حالت) {
    UHashtable *نتيجو;

    إذا (U_FAILURE(*حالت)) أرجع خالي_اشارو;

    نتيجو = static_cast<UHashtable*>(uprv_malloc(حجم(UHashtable)));
    إذا (نتيجو == خالي_اشارو) {
        *حالت = U_يادگيري_ورهاست_ERROR;
        أرجع خالي_اشارو;
    }

    _uhash_init(نتيجو, ڪنجي_Hash, ڪنجي_Comp, قدر_Comp, مفرد_اشاريو, حالت);
    نتيجو->allocated       = سچ;

    إذا (U_FAILURE(*حالت)) {
        uprv_free(نتيجو);
        أرجع خالي_اشارو;
    }

    أرجع نتيجو;
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
ساكن UHashElement*
uhash_ڳوليو_2(ثابت UHashtable *hash, UHashTok ڪنجي,
            int32_t hashcode) {

    int32_t پهريون_Deleted = -1;  /* assume invalid index */
    int32_t the_اشاريو, شروعات_اشاريو;
    int32_t jump = 0; /* lazy evaluate */
    int32_t جدول_Hash;
    UHashElement *elements = hash->elements;

    hashcode &= 0x7FFFFFFF; /* must be positive */
    شروعات_اشاريو = the_اشاريو = (hashcode ^ 0x4000000) % hash->length;

    افعل {
        جدول_Hash = elements[the_اشاريو].hashcode;
        إذا (جدول_Hash == hashcode) {          /* quick check */
            إذا ((*hash->ڪنجي_Comparator)(ڪنجي, elements[the_اشاريو].ڪنجي)) {
                أرجع &(elements[the_اشاريو]);
            }
        } وإلا إذا (!IS_خالي_جايون_OR_DELETED(جدول_Hash)) {
            /* We have hit a slot which contains a key-value pair,
             * but for which the hash code does not match.  Keep
             * looking.
             */
        } وإلا إذا (جدول_Hash == HASH_خالي_جايون) { /* empty, end o' the line */
            اكسر;
        } وإلا إذا (پهريون_Deleted < 0) { /* remember first deleted */
            پهريون_Deleted = the_اشاريو;
        }
        إذا (jump == 0) { /* lazy compute jump */
            /* The jump value must be relatively prime to the table
             * length.  As long as the length is prime, then any value
             * 1..length-1 will be relatively prime to it.
             */
            jump = (hashcode % (hash->length - 1)) + 1;
        }
        the_اشاريو = (the_اشاريو + jump) % hash->length;
    } طالما (the_اشاريو != شروعات_اشاريو);

    إذا (پهريون_Deleted >= 0) {
        the_اشاريو = پهريون_Deleted; /* reset if had deleted slot */
    } وإلا إذا (جدول_Hash != HASH_خالي_جايون) {
        /* We get to this point if the hashtable is full (no empty or
         * deleted slots), and we've failed to find a match.  THIS
         * WILL NEVER HAPPEN as long as uhash_put() makes sure that
         * count is always < length.
         */
        UPRV_UNREACHABLE_EXIT;
    }
    أرجع &(elements[the_اشاريو]);
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
ساكن فراغ
_uhash_rehash(UHashtable *hash, UError_ڪوڊ *حالت) {

    UHashElement *old = hash->elements;
    int32_t old_ڊيگهائي = hash->length;
    int32_t new_مفرد_اشاريو = hash->مفرد_اشاريو;
    int32_t i;

    إذا (hash->count > hash->مٿاهون_Water_Mark) {
        إذا (++new_مفرد_اشاريو >= مفرد_انگ_ڊيگهائي) {
            أرجع;
        }
    } وإلا إذا (hash->count < hash->هيٺيون_Water_Mark) {
        إذا (--new_مفرد_اشاريو < 0) {
            أرجع;
        }
    } وإلا {
        أرجع;
    }

    _uhash_allocate(hash, new_مفرد_اشاريو, حالت);

    إذا (U_FAILURE(*حالت)) {
        hash->elements = old;
        hash->length = old_ڊيگهائي;
        أرجع;
    }

    لكل (i = old_ڊيگهائي - 1; i >= 0; --i) {
        إذا (!IS_خالي_جايون_OR_DELETED(old[i].hashcode)) {
            UHashElement *e = uhash_ڳوليو_2(hash, old[i].ڪنجي, old[i].hashcode);
            U_ASSERT(e != خالي_اشارو);
            U_ASSERT(e->hashcode == HASH_خالي_جايون);
            e->ڪنجي = old[i].ڪنجي;
            e->value = old[i].value;
            e->hashcode = old[i].hashcode;
            ++hash->count;
        }
    }

    uprv_free(old);
}

ساكن UHashTok
_uhash_remove(UHashtable *hash,
              UHashTok ڪنجي) {
    /* First find the position of the key in the table.  If the object
     * has not been removed already, remove it.  If the user wanted
     * keys deleted, then delete it also.  We have to put a special
     * hashcode in that position that means that something has been
     * deleted, since when we do a find, we have to continue PAST any
     * deleted values.
     */
    UHashTok نتيجو;
    UHashElement* e = uhash_ڳوليو_2(hash, ڪنجي, hash->ڪنجي_Hasher(ڪنجي));
    U_ASSERT(e != خالي_اشارو);
    نتيجو.اشارو = خالي_اشارو;
    نتيجو.integer = 0;
    إذا (!IS_خالي_جايون_OR_DELETED(e->hashcode)) {
        نتيجو = uhash_اندروني_Remove_Element(hash, e);
        إذا (hash->count < hash->هيٺيون_Water_Mark) {
            UError_ڪوڊ حالت = U_ZERO_ERROR;
            _uhash_rehash(hash, &حالت);
        }
    }
    أرجع نتيجو;
}

ساكن UHashTok
_uhash_put(UHashtable *hash,
           UHashTok ڪنجي,
           UHashTok value,
           int8_t hint,
           UError_ڪوڊ *حالت) {

    /* Put finds the position in the table for the new value.  If the
     * key is already in the table, it is deleted, if there is a
     * non-nullptr keyDeleter.  Then the key, the hash and the value are
     * all put at the position in their respective arrays.
     */
    int32_t hashcode;
    UHashElement* e;
    UHashTok emptytok;

    إذا (U_FAILURE(*حالت)) {
        اذهب err;
    }
    U_ASSERT(hash != خالي_اشارو);
    إذا ((hint & HINT_قدر_اشارو) ?
            value.اشارو == خالي_اشارو :
            value.integer == 0 && (hint & HINT_ALLOW_ZERO) == 0) {
        /* Disallow storage of nullptr values, since nullptr is returned by
         * get() to indicate an absent key.  Storing nullptr == removing.
         */
        أرجع _uhash_remove(hash, ڪنجي);
    }
    إذا (hash->count > hash->مٿاهون_Water_Mark) {
        _uhash_rehash(hash, حالت);
        إذا (U_FAILURE(*حالت)) {
            اذهب err;
        }
    }

    hashcode = (*hash->ڪنجي_Hasher)(ڪنجي);
    e = uhash_ڳوليو_2(hash, ڪنجي, hashcode);
    U_ASSERT(e != خالي_اشارو);

    إذا (IS_خالي_جايون_OR_DELETED(e->hashcode)) {
        /* Important: We must never actually fill the table up.  If we
         * do so, then _uhash_find() will return nullptr, and we'll have
         * to check for nullptr after every call to _uhash_find().  To
         * avoid this we make sure there is always at least one empty
         * or deleted slot in the table.  This only is a problem if we
         * are out of memory and rehash isn't working.
         */
        ++hash->count;
        إذا (hash->count == hash->length) {
            /* Don't allow count to reach length */
            --hash->count;
            *حالت = U_يادگيري_ورهاست_ERROR;
            اذهب err;
        }
    }

    /* We must in all cases handle storage properly.  If there was an
     * old key, then it must be deleted (if the deleter != nullptr).
     * Make hashcodes stored in table positive.
     */
    أرجع uhash_سيٽ_Element(hash, e, hashcode & 0x7FFFFFFF, ڪنجي, value, hint);

 err:
    /* If the deleters are non-nullptr, this method adopts its key and/or
     * value arguments, and we must be sure to delete the key and/or
     * value in all cases, even upon failure.
     */
    HASH_DELETE_ڪنجي_قدر(hash, ڪنجي.اشارو, value.اشارو);
    emptytok.اشارو = خالي_اشارو; emptytok.integer = 0;
    أرجع emptytok;
}


/********************************************************************
 * PUBLIC API
 ********************************************************************/

U_CAPI UHashtable* U_EXPORT2
uhash_open(UHashFunction *ڪنجي_Hash,
           UKeyComparator *ڪنجي_Comp,
           UValueComparator *قدر_Comp,
           UError_ڪوڊ *حالت) {

    أرجع _uhash_create(ڪنجي_Hash, ڪنجي_Comp, قدر_Comp, DEFAULT_مفرد_اشاريو, حالت);
}

U_CAPI UHashtable* U_EXPORT2
uhash_open_سائيز(UHashFunction *ڪنجي_Hash,
               UKeyComparator *ڪنجي_Comp,
               UValueComparator *قدر_Comp,
               int32_t size,
               UError_ڪوڊ *حالت) {

    /* Find the smallest index i for which PRIMES[i] >= size. */
    int32_t i = 0;
    طالما (i<(مفرد_انگ_ڊيگهائي-1) && مفرد_انگ[i]<size) {
        ++i;
    }

    أرجع _uhash_create(ڪنجي_Hash, ڪنجي_Comp, قدر_Comp, i, حالت);
}

U_CAPI UHashtable* U_EXPORT2
uhash_init(UHashtable *fillin_نتيجو,
           UHashFunction *ڪنجي_Hash,
           UKeyComparator *ڪنجي_Comp,
           UValueComparator *قدر_Comp,
           UError_ڪوڊ *حالت) {

    أرجع _uhash_init(fillin_نتيجو, ڪنجي_Hash, ڪنجي_Comp, قدر_Comp, DEFAULT_مفرد_اشاريو, حالت);
}

U_CAPI UHashtable* U_EXPORT2
uhash_init_سائيز(UHashtable *fillin_نتيجو,
               UHashFunction *ڪنجي_Hash,
               UKeyComparator *ڪنجي_Comp,
               UValueComparator *قدر_Comp,
               int32_t size,
               UError_ڪوڊ *حالت) {

    // Find the smallest index i for which PRIMES[i] >= size.
    int32_t i = 0;
    طالما (i<(مفرد_انگ_ڊيگهائي-1) && مفرد_انگ[i]<size) {
        ++i;
    }
    أرجع _uhash_init(fillin_نتيجو, ڪنجي_Hash, ڪنجي_Comp, قدر_Comp, i, حالت);
}

U_CAPI فراغ U_EXPORT2
uhash_close(UHashtable *hash) {
    إذا (hash == خالي_اشارو) {
        أرجع;
    }
    إذا (hash->elements != خالي_اشارو) {
        إذا (hash->ڪنجي_Deleter != خالي_اشارو || hash->قدر_Deleter != خالي_اشارو) {
            int32_t pos=UHASH_پهريون;
            UHashElement *e;
            طالما ((e = (UHashElement*) uhash_ايندڙ_Element(hash, &pos)) != خالي_اشارو) {
                HASH_DELETE_ڪنجي_قدر(hash, e->ڪنجي.اشارو, e->value.اشارو);
            }
        }
        uprv_free(hash->elements);
        hash->elements = خالي_اشارو;
    }
    إذا (hash->allocated) {
        uprv_free(hash);
    }
}

U_CAPI UHashFunction *U_EXPORT2
uhash_سيٽ_ڪنجي_Hasher(UHashtable *hash, UHashFunction *fn) {
    UHashFunction *نتيجو = hash->ڪنجي_Hasher;
    hash->ڪنجي_Hasher = fn;
    أرجع نتيجو;
}

U_CAPI UKeyComparator *U_EXPORT2
uhash_سيٽ_ڪنجي_Comparator(UHashtable *hash, UKeyComparator *fn) {
    UKeyComparator *نتيجو = hash->ڪنجي_Comparator;
    hash->ڪنجي_Comparator = fn;
    أرجع نتيجو;
}
U_CAPI UValueComparator *U_EXPORT2
uhash_سيٽ_قدر_Comparator(UHashtable *hash, UValueComparator *fn){
    UValueComparator *نتيجو = hash->قدر_Comparator;
    hash->قدر_Comparator = fn;
    أرجع نتيجو;
}

U_CAPI UObjectDeleter *U_EXPORT2
uhash_سيٽ_ڪنجي_Deleter(UHashtable *hash, UObjectDeleter *fn) {
    UObjectDeleter *نتيجو = hash->ڪنجي_Deleter;
    hash->ڪنجي_Deleter = fn;
    أرجع نتيجو;
}

U_CAPI UObjectDeleter *U_EXPORT2
uhash_سيٽ_قدر_Deleter(UHashtable *hash, UObjectDeleter *fn) {
    UObjectDeleter *نتيجو = hash->قدر_Deleter;
    hash->قدر_Deleter = fn;
    أرجع نتيجو;
}

U_CAPI فراغ U_EXPORT2
uhash_سيٽ_Resize_Policy(UHashtable *hash, تعداد UHashResizePolicy policy) {
    UError_ڪوڊ حالت = U_ZERO_ERROR;
    uhash_اندروني_سيٽ_Resize_Policy(hash, policy);
    hash->هيٺيون_Water_Mark  = (int32_t)(hash->length * hash->هيٺيون_Water_تناسب);
    hash->مٿاهون_Water_Mark = (int32_t)(hash->length * hash->مٿاهون_Water_تناسب);
    _uhash_rehash(hash, &حالت);
}

U_CAPI int32_t U_EXPORT2
uhash_ڳڻپ(ثابت UHashtable *hash) {
    أرجع hash->count;
}

U_CAPI فراغ* U_EXPORT2
uhash_get(ثابت UHashtable *hash,
          ثابت فراغ* ڪنجي) {
    UHashTok keyholder;
    keyholder.اشارو = (فراغ*) ڪنجي;
    أرجع uhash_ڳوليو_2(hash, keyholder, hash->ڪنجي_Hasher(keyholder))->value.اشارو;
}

U_CAPI فراغ* U_EXPORT2
uhash_iget(ثابت UHashtable *hash,
           int32_t ڪنجي) {
    UHashTok keyholder;
    keyholder.integer = ڪنجي;
    أرجع uhash_ڳوليو_2(hash, keyholder, hash->ڪنجي_Hasher(keyholder))->value.اشارو;
}

U_CAPI int32_t U_EXPORT2
uhash_geti(ثابت UHashtable *hash,
           ثابت فراغ* ڪنجي) {
    UHashTok keyholder;
    keyholder.اشارو = (فراغ*) ڪنجي;
    أرجع uhash_ڳوليو_2(hash, keyholder, hash->ڪنجي_Hasher(keyholder))->value.integer;
}

U_CAPI int32_t U_EXPORT2
uhash_igeti(ثابت UHashtable *hash,
           int32_t ڪنجي) {
    UHashTok keyholder;
    keyholder.integer = ڪنجي;
    أرجع uhash_ڳوليو_2(hash, keyholder, hash->ڪنجي_Hasher(keyholder))->value.integer;
}

U_CAPI int32_t U_EXPORT2
uhash_geti_And_مليو(ثابت UHashtable *hash,
                   ثابت فراغ *ڪنجي,
                   UBool *مليو) {
    UHashTok keyholder;
    keyholder.اشارو = (فراغ *)ڪنجي;
    ثابت UHashElement *e = uhash_ڳوليو_2(hash, keyholder, hash->ڪنجي_Hasher(keyholder));
    *مليو = !IS_خالي_جايون_OR_DELETED(e->hashcode);
    أرجع e->value.integer;
}

U_CAPI int32_t U_EXPORT2
uhash_igeti_And_مليو(ثابت UHashtable *hash,
                    int32_t ڪنجي,
                    UBool *مليو) {
    UHashTok keyholder;
    keyholder.integer = ڪنجي;
    ثابت UHashElement *e = uhash_ڳوليو_2(hash, keyholder, hash->ڪنجي_Hasher(keyholder));
    *مليو = !IS_خالي_جايون_OR_DELETED(e->hashcode);
    أرجع e->value.integer;
}

U_CAPI فراغ* U_EXPORT2
uhash_put(UHashtable *hash,
          فراغ* ڪنجي,
          فراغ* value,
          UError_ڪوڊ *حالت) {
    UHashTok keyholder, valueholder;
    keyholder.اشارو = ڪنجي;
    valueholder.اشارو = value;
    أرجع _uhash_put(hash, keyholder, valueholder,
                      HINT_ڪنجي_اشارو | HINT_قدر_اشارو,
                      حالت).اشارو;
}

U_CAPI فراغ* U_EXPORT2
uhash_iput(UHashtable *hash,
           int32_t ڪنجي,
           فراغ* value,
           UError_ڪوڊ *حالت) {
    UHashTok keyholder, valueholder;
    keyholder.integer = ڪنجي;
    valueholder.اشارو = value;
    أرجع _uhash_put(hash, keyholder, valueholder,
                      HINT_قدر_اشارو,
                      حالت).اشارو;
}

U_CAPI int32_t U_EXPORT2
uhash_puti(UHashtable *hash,
           فراغ* ڪنجي,
           int32_t value,
           UError_ڪوڊ *حالت) {
    UHashTok keyholder, valueholder;
    keyholder.اشارو = ڪنجي;
    valueholder.integer = value;
    أرجع _uhash_put(hash, keyholder, valueholder,
                      HINT_ڪنجي_اشارو,
                      حالت).integer;
}


U_CAPI int32_t U_EXPORT2
uhash_iputi(UHashtable *hash,
           int32_t ڪنجي,
           int32_t value,
           UError_ڪوڊ *حالت) {
    UHashTok keyholder, valueholder;
    keyholder.integer = ڪنجي;
    valueholder.integer = value;
    أرجع _uhash_put(hash, keyholder, valueholder,
                      HINT_BOTH_INTEGERS,
                      حالت).integer;
}

U_CAPI int32_t U_EXPORT2
uhash_putiAllowZero(UHashtable *hash,
                    فراغ *ڪنجي,
                    int32_t value,
                    UError_ڪوڊ *حالت) {
    UHashTok keyholder, valueholder;
    keyholder.اشارو = ڪنجي;
    valueholder.integer = value;
    أرجع _uhash_put(hash, keyholder, valueholder,
                      HINT_ڪنجي_اشارو | HINT_ALLOW_ZERO,
                      حالت).integer;
}


U_CAPI int32_t U_EXPORT2
uhash_iputiAllowZero(UHashtable *hash,
                     int32_t ڪنجي,
                     int32_t value,
                     UError_ڪوڊ *حالت) {
    UHashTok keyholder, valueholder;
    keyholder.integer = ڪنجي;
    valueholder.integer = value;
    أرجع _uhash_put(hash, keyholder, valueholder,
                      HINT_BOTH_INTEGERS | HINT_ALLOW_ZERO,
                      حالت).integer;
}

U_CAPI فراغ* U_EXPORT2
uhash_remove(UHashtable *hash,
             ثابت فراغ* ڪنجي) {
    UHashTok keyholder;
    keyholder.اشارو = (فراغ*) ڪنجي;
    أرجع _uhash_remove(hash, keyholder).اشارو;
}

U_CAPI فراغ* U_EXPORT2
uhash_iremove(UHashtable *hash,
              int32_t ڪنجي) {
    UHashTok keyholder;
    keyholder.integer = ڪنجي;
    أرجع _uhash_remove(hash, keyholder).اشارو;
}

U_CAPI int32_t U_EXPORT2
uhash_removei(UHashtable *hash,
              ثابت فراغ* ڪنجي) {
    UHashTok keyholder;
    keyholder.اشارو = (فراغ*) ڪنجي;
    أرجع _uhash_remove(hash, keyholder).integer;
}

U_CAPI int32_t U_EXPORT2
uhash_iremovei(UHashtable *hash,
               int32_t ڪنجي) {
    UHashTok keyholder;
    keyholder.integer = ڪنجي;
    أرجع _uhash_remove(hash, keyholder).integer;
}

U_CAPI فراغ U_EXPORT2
uhash_removeAll(UHashtable *hash) {
    int32_t pos = UHASH_پهريون;
    ثابت UHashElement *e;
    U_ASSERT(hash != خالي_اشارو);
    إذا (hash->count != 0) {
        طالما ((e = uhash_ايندڙ_Element(hash, &pos)) != خالي_اشارو) {
            uhash_removeElement(hash, e);
        }
    }
    U_ASSERT(hash->count == 0);
}

U_CAPI UBool U_EXPORT2
uhash_شامل_آهي_ڪنجي(ثابت UHashtable *hash, ثابت فراغ *ڪنجي) {
    UHashTok keyholder;
    keyholder.اشارو = (فراغ *)ڪنجي;
    ثابت UHashElement *e = uhash_ڳوليو_2(hash, keyholder, hash->ڪنجي_Hasher(keyholder));
    أرجع !IS_خالي_جايون_OR_DELETED(e->hashcode);
}

/**
 * Returns true if the UHashtable contains an item with this integer key.
 *
 * @param hash The target UHashtable.
 * @param key An integer key stored in a hashtable
 * @return true if the key is found.
 */
U_CAPI UBool U_EXPORT2
uhash_icontains_ڪنجي(ثابت UHashtable *hash, int32_t ڪنجي) {
    UHashTok keyholder;
    keyholder.integer = ڪنجي;
    ثابت UHashElement *e = uhash_ڳوليو_2(hash, keyholder, hash->ڪنجي_Hasher(keyholder));
    أرجع !IS_خالي_جايون_OR_DELETED(e->hashcode);
}

U_CAPI ثابت UHashElement* U_EXPORT2
uhash_ڳوليو(ثابت UHashtable *hash, ثابت فراغ* ڪنجي) {
    UHashTok keyholder;
    ثابت UHashElement *e;
    keyholder.اشارو = (فراغ*) ڪنجي;
    e = uhash_ڳوليو_2(hash, keyholder, hash->ڪنجي_Hasher(keyholder));
    أرجع IS_خالي_جايون_OR_DELETED(e->hashcode) ? خالي_اشارو : e;
}

U_CAPI ثابت UHashElement* U_EXPORT2
uhash_ايندڙ_Element(ثابت UHashtable *hash, int32_t *pos) {
    /* Walk through the array until we find an element that is not
     * EMPTY and not DELETED.
     */
    int32_t i;
    U_ASSERT(hash != خالي_اشارو);
    لكل (i = *pos + 1; i < hash->length; ++i) {
        إذا (!IS_خالي_جايون_OR_DELETED(hash->elements[i].hashcode)) {
            *pos = i;
            أرجع &(hash->elements[i]);
        }
    }

    /* No more elements */
    أرجع خالي_اشارو;
}

U_CAPI فراغ* U_EXPORT2
uhash_removeElement(UHashtable *hash, ثابت UHashElement* e) {
    U_ASSERT(hash != خالي_اشارو);
    U_ASSERT(e != خالي_اشارو);
    إذا (!IS_خالي_جايون_OR_DELETED(e->hashcode)) {
        UHashElement *nce = (UHashElement *)e;
        أرجع uhash_اندروني_Remove_Element(hash, nce).اشارو;
    }
    أرجع خالي_اشارو;
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
uhash_hashUChars(ثابت UHashTok ڪنجي) {
    ثابت char16_t *s = (ثابت char16_t *)ڪنجي.اشارو;
    أرجع s == خالي_اشارو ? 0 : ustr_hashUCharsN(s, u_strlen(s));
}

U_CAPI int32_t U_EXPORT2
uhash_hashChars(ثابت UHashTok ڪنجي) {
    ثابت حرف *s = (ثابت حرف *)ڪنجي.اشارو;
    أرجع s == خالي_اشارو ? 0 : static_cast<int32_t>(ustr_hashCharsN(s, static_cast<int32_t>(uprv_strlen(s))));
}

U_CAPI int32_t U_EXPORT2
uhash_hashIChars(ثابت UHashTok ڪنجي) {
    ثابت حرف *s = (ثابت حرف *)ڪنجي.اشارو;
    أرجع s == خالي_اشارو ? 0 : ustr_hashICharsN(s, static_cast<int32_t>(uprv_strlen(s)));
}

U_CAPI int32_t U_EXPORT2
uhash_hashIStringView(ثابت UHashTok ڪنجي) {
    ثابت std::string_view* s = static_cast<std::string_view*>(ڪنجي.اشارو);
    أرجع s == خالي_اشارو ? 0 : ustr_hashICharsN(s->معلومات(), static_cast<int32_t>(s->size()));
}

U_CAPI UBool U_EXPORT2
uhash_equals(ثابت UHashtable* hash1, ثابت UHashtable* hash2){
    int32_t count1, count2, pos, i;

    إذا(hash1==hash2){
        أرجع سچ;
    }

    /*
     * Make sure that we are comparing 2 valid hashes of the same type
     * with valid comparison functions.
     * Without valid comparison functions, a binary comparison
     * of the hash values will yield random results on machines
     * with 64-bit pointers and 32-bit integer hashes.
     * A valueComparator is normally optional.
     */
    إذا (hash1==خالي_اشارو || hash2==خالي_اشارو ||
        hash1->ڪنجي_Comparator != hash2->ڪنجي_Comparator ||
        hash1->قدر_Comparator != hash2->قدر_Comparator ||
        hash1->قدر_Comparator == خالي_اشارو)
    {
        /*
        Normally we would return an error here about incompatible hash tables,
        but we return false instead.
        */
        أرجع خطأ;
    }

    count1 = uhash_ڳڻپ(hash1);
    count2 = uhash_ڳڻپ(hash2);
    إذا(count1!=count2){
        أرجع خطأ;
    }

    pos=UHASH_پهريون;
    لكل(i=0; i<count1; i++){
        ثابت UHashElement* elem1 = uhash_ايندڙ_Element(hash1, &pos);
        ثابت UHashTok key1 = elem1->ڪنجي;
        ثابت UHashTok val1 = elem1->value;
        /* here the keys are not compared, instead the key form hash1 is used to fetch
         * value from hash2. If the hashes are equal then then both hashes should
         * contain equal values for the same key!
         */
        ثابت UHashElement* elem2 = uhash_ڳوليو_2(hash2, key1, hash2->ڪنجي_Hasher(key1));
        ثابت UHashTok val2 = elem2->value;
        إذا(hash1->قدر_Comparator(val1, val2)==خطأ){
            أرجع خطأ;
        }
    }
    أرجع سچ;
}

/********************************************************************
 * PUBLIC Comparator Functions
 ********************************************************************/

U_CAPI UBool U_EXPORT2
uhash_compareUChars(ثابت UHashTok key1, ثابت UHashTok key2) {
    ثابت char16_t *p1 = (ثابت char16_t*) key1.اشارو;
    ثابت char16_t *p2 = (ثابت char16_t*) key2.اشارو;
    إذا (p1 == p2) {
        أرجع سچ;
    }
    إذا (p1 == خالي_اشارو || p2 == خالي_اشارو) {
        أرجع خطأ;
    }
    طالما (*p1 != 0 && *p1 == *p2) {
        ++p1;
        ++p2;
    }
    أرجع *p1 == *p2;
}

U_CAPI UBool U_EXPORT2
uhash_compareChars(ثابت UHashTok key1, ثابت UHashTok key2) {
    ثابت حرف *p1 = (ثابت حرف*) key1.اشارو;
    ثابت حرف *p2 = (ثابت حرف*) key2.اشارو;
    إذا (p1 == p2) {
        أرجع سچ;
    }
    إذا (p1 == خالي_اشارو || p2 == خالي_اشارو) {
        أرجع خطأ;
    }
    طالما (*p1 != 0 && *p1 == *p2) {
        ++p1;
        ++p2;
    }
    أرجع *p1 == *p2;
}

U_CAPI UBool U_EXPORT2
uhash_compareIChars(ثابت UHashTok key1, ثابت UHashTok key2) {
    ثابت حرف *p1 = (ثابت حرف*) key1.اشارو;
    ثابت حرف *p2 = (ثابت حرف*) key2.اشارو;
    إذا (p1 == p2) {
        أرجع سچ;
    }
    إذا (p1 == خالي_اشارو || p2 == خالي_اشارو) {
        أرجع خطأ;
    }
    طالما (*p1 != 0 && uprv_tolower(*p1) == uprv_tolower(*p2)) {
        ++p1;
        ++p2;
    }
    أرجع *p1 == *p2;
}

U_CAPI UBool U_EXPORT2
uhash_compareIStringView(ثابت UHashTok key1, ثابت UHashTok key2) {
    ثابت std::string_view* p1 = static_cast<std::string_view*>(key1.اشارو);
    ثابت std::string_view* p2 = static_cast<std::string_view*>(key2.اشارو);
    إذا (p1 == p2) {
        أرجع سچ;
    }
    إذا (p1 == خالي_اشارو || p2 == خالي_اشارو) {
        أرجع خطأ;
    }
    ثابت std::string_view& v1 = *p1;
    ثابت std::string_view& v2 = *p2;
    إذا (v1.size() != v2.size()) {
        أرجع خطأ;
    }
    لكل (size_t i = 0; i < v1.size(); ++i) {
        إذا (uprv_tolower(v1[i]) != uprv_tolower(v2[i])) {
            أرجع خطأ;
        }
    }
    أرجع سچ;
}

/********************************************************************
 * PUBLIC int32_t Support Functions
 ********************************************************************/

U_CAPI int32_t U_EXPORT2
uhash_hashLong(ثابت UHashTok ڪنجي) {
    أرجع ڪنجي.integer;
}

U_CAPI UBool U_EXPORT2
uhash_compareLong(ثابت UHashTok key1, ثابت UHashTok key2) {
    أرجع key1.integer == key2.integer;
}
