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

#شامل <string_view>

#شامل "uhash.h"
#شامل "unicode/ustring.h"
#شامل "cstring.h"
#شامل "cmemory.h"
#شامل "uassert.h"
#شامل "ustr_imp.h"

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
ساکن ثابت int32_t مفرد_اعداد[] = {
    7, 13, 31, 61, 127, 251, 509, 1021, 2039, 4093, 8191, 16381, 32749,
    65521, 131071, 262139, 524287, 1048573, 2097143, 4194301, 8388593,
    16777213, 33554393, 67108859, 134217689, 268435399, 536870909,
    1073741789, 2147483647 /*, 4294967291 */
};

#تعریف_کرو مفرد_اعداد_لمبائی UPRV_LENGTHOF(مفرد_اعداد)
#تعریف_کرو DEFAULT_مفرد_اشاریہ 4

/* These ratios are tuned to the PRIMES array such that a resize
 * places the table back into the zone of non-resizing.  That is,
 * after a call to _uhash_rehash(), a subsequent call to
 * _uhash_rehash() should do nothing (should not churn).  This is only
 * a potential problem with U_GROW_AND_SHRINK.
 */
ساکن ثابت اعشاری RESIZE_POLICY_تناسب_جدول[6] = {
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
#تعریف_کرو HASH_DELETED    ((int32_t) 0x80000000)
#تعریف_کرو HASH_خالی_خانے      ((int32_t) HASH_DELETED + 1)

#تعریف_کرو IS_خالی_خانے_OR_DELETED(x) ((x) < 0)

/* This macro expects a UHashTok.pointer as its keypointer and
   valuepointer parameters */
#تعریف_کرو HASH_DELETE_کلید_قدر(hash, keypointer, valuepointer) UPRV_بلاک_MACRO_BEGIN { \
    اگر (hash->کلید_Deleter != خالی_اشاریہ && keypointer != خالی_اشاریہ) { \
        (*hash->کلید_Deleter)(keypointer); \
    } \
    اگر (hash->قدر_Deleter != خالی_اشاریہ && valuepointer != خالی_اشاریہ) { \
        (*hash->قدر_Deleter)(valuepointer); \
    } \
} UPRV_بلاک_MACRO_انتہا

/*
 * Constants for hinting whether a key or value is an integer
 * or a pointer.  If a hint bit is zero, then the associated
 * token is assumed to be an integer.
 */
#تعریف_کرو HINT_BOTH_INTEGERS (0)
#تعریف_کرو HINT_کلید_اشارہ_گر   (1)
#تعریف_کرو HINT_قدر_اشارہ_گر (2)
#تعریف_کرو HINT_ALLOW_ZERO    (4)

/********************************************************************
 * PRIVATE Implementation
 ********************************************************************/

ساکن UHashTok
uhash_سیٹ_Element(UHashtable *hash, UHashElement* e,
                  int32_t hashcode,
                  UHashTok کلید, UHashTok value, int8_t hint) {

    UHashTok old_قدر = e->value;
    اگر (hash->کلید_Deleter != خالی_اشاریہ && e->کلید.اشارہ_گر != خالی_اشاریہ &&
        e->کلید.اشارہ_گر != کلید.اشارہ_گر) { /* Avoid double deletion */
        (*hash->کلید_Deleter)(e->کلید.اشارہ_گر);
    }
    اگر (hash->قدر_Deleter != خالی_اشاریہ) {
        اگر (old_قدر.اشارہ_گر != خالی_اشاریہ &&
            old_قدر.اشارہ_گر != value.اشارہ_گر) { /* Avoid double deletion */
            (*hash->قدر_Deleter)(old_قدر.اشارہ_گر);
        }
        old_قدر.اشارہ_گر = خالی_اشاریہ;
    }
    /* Compilers should copy the UHashTok union correctly, but even if
     * they do, memory heap tools (e.g. BoundsChecker) can get
     * confused when a pointer is cloaked in a union and then copied.
     * TO ALLEVIATE THIS, we use hints (based on what API the user is
     * calling) to copy pointers when we know the user thinks
     * something is a pointer. */
    اگر (hint & HINT_کلید_اشارہ_گر) {
        e->کلید.اشارہ_گر = کلید.اشارہ_گر;
    } ورنہ {
        e->کلید = کلید;
    }
    اگر (hint & HINT_قدر_اشارہ_گر) {
        e->value.اشارہ_گر = value.اشارہ_گر;
    } ورنہ {
        e->value = value;
    }
    e->hashcode = hashcode;
    واپس old_قدر;
}

/**
 * Assumes that the given element is not empty or deleted.
 */
ساکن UHashTok
uhash_اندرونی_Remove_Element(UHashtable *hash, UHashElement* e) {
    UHashTok empty;
    U_ASSERT(!IS_خالی_خانے_OR_DELETED(e->hashcode));
    --hash->count;
    empty.اشارہ_گر = خالی_اشاریہ; empty.integer = 0;
    واپس uhash_سیٹ_Element(hash, e, HASH_DELETED, empty, empty, 0);
}

ساکن خالی
uhash_اندرونی_سیٹ_Resize_Policy(UHashtable *hash, شمار UHashResizePolicy policy) {
    U_ASSERT(hash != خالی_اشاریہ);
    U_ASSERT(((int32_t)policy) >= 0);
    U_ASSERT(((int32_t)policy) < 3);
    hash->کم_Water_تناسب  = RESIZE_POLICY_تناسب_جدول[policy * 2];
    hash->بلند_Water_تناسب = RESIZE_POLICY_تناسب_جدول[policy * 2 + 1];
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
ساکن خالی
_uhash_allocate(UHashtable *hash,
                int32_t مفرد_اشاریہ,
                UError_رمز *حالت_2) {

    UHashElement *p, *حد_بندی;
    UHashTok emptytok;

    اگر (U_FAILURE(*حالت_2)) واپس;

    U_ASSERT(مفرد_اشاریہ >= 0 && مفرد_اشاریہ < مفرد_اعداد_لمبائی);

    hash->مفرد_اشاریہ = static_cast<int8_t>(مفرد_اشاریہ);
    hash->length = مفرد_اعداد[مفرد_اشاریہ];

    p = hash->elements = static_cast<UHashElement*>(
        uprv_malloc(حجم(UHashElement) * hash->length));

    اگر (hash->elements == خالی_اشاریہ) {
        *حالت_2 = U_حافظہ_تخصیص_ERROR;
        واپس;
    }

    emptytok.اشارہ_گر = خالی_اشاریہ; /* Only one of these two is needed */
    emptytok.integer = 0;    /* but we don't know which one. */

    حد_بندی = p + hash->length;
    جب (p < حد_بندی) {
        p->کلید = emptytok;
        p->value = emptytok;
        p->hashcode = HASH_خالی_خانے;
        ++p;
    }

    hash->count = 0;
    hash->کم_Water_Mark = static_cast<int32_t>(hash->length * hash->کم_Water_تناسب);
    hash->بلند_Water_Mark = static_cast<int32_t>(hash->length * hash->بلند_Water_تناسب);
}

ساکن UHashtable*
_uhash_init(UHashtable *نتیجہ,
              UHashFunction *کلید_Hash,
              UKeyComparator *کلید_Comp,
              UValueComparator *قدر_Comp,
              int32_t مفرد_اشاریہ,
              UError_رمز *حالت_2)
{
    اگر (U_FAILURE(*حالت_2)) واپس خالی_اشاریہ;
    U_ASSERT(کلید_Hash != خالی_اشاریہ);
    U_ASSERT(کلید_Comp != خالی_اشاریہ);

    نتیجہ->کلید_Hasher       = کلید_Hash;
    نتیجہ->کلید_Comparator   = کلید_Comp;
    نتیجہ->قدر_Comparator = قدر_Comp;
    نتیجہ->کلید_Deleter      = خالی_اشاریہ;
    نتیجہ->قدر_Deleter    = خالی_اشاریہ;
    نتیجہ->allocated       = جھوٹ;
    uhash_اندرونی_سیٹ_Resize_Policy(نتیجہ, U_GROW);

    _uhash_allocate(نتیجہ, مفرد_اشاریہ, حالت_2);

    اگر (U_FAILURE(*حالت_2)) {
        واپس خالی_اشاریہ;
    }

    واپس نتیجہ;
}

ساکن UHashtable*
_uhash_create(UHashFunction *کلید_Hash,
              UKeyComparator *کلید_Comp,
              UValueComparator *قدر_Comp,
              int32_t مفرد_اشاریہ,
              UError_رمز *حالت_2) {
    UHashtable *نتیجہ;

    اگر (U_FAILURE(*حالت_2)) واپس خالی_اشاریہ;

    نتیجہ = static_cast<UHashtable*>(uprv_malloc(حجم(UHashtable)));
    اگر (نتیجہ == خالی_اشاریہ) {
        *حالت_2 = U_حافظہ_تخصیص_ERROR;
        واپس خالی_اشاریہ;
    }

    _uhash_init(نتیجہ, کلید_Hash, کلید_Comp, قدر_Comp, مفرد_اشاریہ, حالت_2);
    نتیجہ->allocated       = سچ;

    اگر (U_FAILURE(*حالت_2)) {
        uprv_free(نتیجہ);
        واپس خالی_اشاریہ;
    }

    واپس نتیجہ;
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
ساکن UHashElement*
uhash_ڈھونڈو_2(ثابت UHashtable *hash, UHashTok کلید,
            int32_t hashcode) {

    int32_t پہلا_Deleted = -1;  /* assume invalid index */
    int32_t the_اشاریہ, آغاز_اشاریہ;
    int32_t jump = 0; /* lazy evaluate */
    int32_t جدول_Hash;
    UHashElement *elements = hash->elements;

    hashcode &= 0x7FFFFFFF; /* must be positive */
    آغاز_اشاریہ = the_اشاریہ = (hashcode ^ 0x4000000) % hash->length;

    کرو {
        جدول_Hash = elements[the_اشاریہ].hashcode;
        اگر (جدول_Hash == hashcode) {          /* quick check */
            اگر ((*hash->کلید_Comparator)(کلید, elements[the_اشاریہ].کلید)) {
                واپس &(elements[the_اشاریہ]);
            }
        } ورنہ اگر (!IS_خالی_خانے_OR_DELETED(جدول_Hash)) {
            /* We have hit a slot which contains a key-value pair,
             * but for which the hash code does not match.  Keep
             * looking.
             */
        } ورنہ اگر (جدول_Hash == HASH_خالی_خانے) { /* empty, end o' the line */
            توڑو;
        } ورنہ اگر (پہلا_Deleted < 0) { /* remember first deleted */
            پہلا_Deleted = the_اشاریہ;
        }
        اگر (jump == 0) { /* lazy compute jump */
            /* The jump value must be relatively prime to the table
             * length.  As long as the length is prime, then any value
             * 1..length-1 will be relatively prime to it.
             */
            jump = (hashcode % (hash->length - 1)) + 1;
        }
        the_اشاریہ = (the_اشاریہ + jump) % hash->length;
    } جب (the_اشاریہ != آغاز_اشاریہ);

    اگر (پہلا_Deleted >= 0) {
        the_اشاریہ = پہلا_Deleted; /* reset if had deleted slot */
    } ورنہ اگر (جدول_Hash != HASH_خالی_خانے) {
        /* We get to this point if the hashtable is full (no empty or
         * deleted slots), and we've failed to find a match.  THIS
         * WILL NEVER HAPPEN as long as uhash_put() makes sure that
         * count is always < length.
         */
        UPRV_UNREACHABLE_EXIT;
    }
    واپس &(elements[the_اشاریہ]);
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
ساکن خالی
_uhash_rehash(UHashtable *hash, UError_رمز *حالت_2) {

    UHashElement *old = hash->elements;
    int32_t old_لمبائی = hash->length;
    int32_t new_مفرد_اشاریہ = hash->مفرد_اشاریہ;
    int32_t i;

    اگر (hash->count > hash->بلند_Water_Mark) {
        اگر (++new_مفرد_اشاریہ >= مفرد_اعداد_لمبائی) {
            واپس;
        }
    } ورنہ اگر (hash->count < hash->کم_Water_Mark) {
        اگر (--new_مفرد_اشاریہ < 0) {
            واپس;
        }
    } ورنہ {
        واپس;
    }

    _uhash_allocate(hash, new_مفرد_اشاریہ, حالت_2);

    اگر (U_FAILURE(*حالت_2)) {
        hash->elements = old;
        hash->length = old_لمبائی;
        واپس;
    }

    برائے (i = old_لمبائی - 1; i >= 0; --i) {
        اگر (!IS_خالی_خانے_OR_DELETED(old[i].hashcode)) {
            UHashElement *e = uhash_ڈھونڈو_2(hash, old[i].کلید, old[i].hashcode);
            U_ASSERT(e != خالی_اشاریہ);
            U_ASSERT(e->hashcode == HASH_خالی_خانے);
            e->کلید = old[i].کلید;
            e->value = old[i].value;
            e->hashcode = old[i].hashcode;
            ++hash->count;
        }
    }

    uprv_free(old);
}

ساکن UHashTok
_uhash_remove(UHashtable *hash,
              UHashTok کلید) {
    /* First find the position of the key in the table.  If the object
     * has not been removed already, remove it.  If the user wanted
     * keys deleted, then delete it also.  We have to put a special
     * hashcode in that position that means that something has been
     * deleted, since when we do a find, we have to continue PAST any
     * deleted values.
     */
    UHashTok نتیجہ;
    UHashElement* e = uhash_ڈھونڈو_2(hash, کلید, hash->کلید_Hasher(کلید));
    U_ASSERT(e != خالی_اشاریہ);
    نتیجہ.اشارہ_گر = خالی_اشاریہ;
    نتیجہ.integer = 0;
    اگر (!IS_خالی_خانے_OR_DELETED(e->hashcode)) {
        نتیجہ = uhash_اندرونی_Remove_Element(hash, e);
        اگر (hash->count < hash->کم_Water_Mark) {
            UError_رمز حالت_2 = U_ZERO_ERROR;
            _uhash_rehash(hash, &حالت_2);
        }
    }
    واپس نتیجہ;
}

ساکن UHashTok
_uhash_put(UHashtable *hash,
           UHashTok کلید,
           UHashTok value,
           int8_t hint,
           UError_رمز *حالت_2) {

    /* Put finds the position in the table for the new value.  If the
     * key is already in the table, it is deleted, if there is a
     * non-nullptr keyDeleter.  Then the key, the hash and the value are
     * all put at the position in their respective arrays.
     */
    int32_t hashcode;
    UHashElement* e;
    UHashTok emptytok;

    اگر (U_FAILURE(*حالت_2)) {
        جاؤ err;
    }
    U_ASSERT(hash != خالی_اشاریہ);
    اگر ((hint & HINT_قدر_اشارہ_گر) ?
            value.اشارہ_گر == خالی_اشاریہ :
            value.integer == 0 && (hint & HINT_ALLOW_ZERO) == 0) {
        /* Disallow storage of nullptr values, since nullptr is returned by
         * get() to indicate an absent key.  Storing nullptr == removing.
         */
        واپس _uhash_remove(hash, کلید);
    }
    اگر (hash->count > hash->بلند_Water_Mark) {
        _uhash_rehash(hash, حالت_2);
        اگر (U_FAILURE(*حالت_2)) {
            جاؤ err;
        }
    }

    hashcode = (*hash->کلید_Hasher)(کلید);
    e = uhash_ڈھونڈو_2(hash, کلید, hashcode);
    U_ASSERT(e != خالی_اشاریہ);

    اگر (IS_خالی_خانے_OR_DELETED(e->hashcode)) {
        /* Important: We must never actually fill the table up.  If we
         * do so, then _uhash_find() will return nullptr, and we'll have
         * to check for nullptr after every call to _uhash_find().  To
         * avoid this we make sure there is always at least one empty
         * or deleted slot in the table.  This only is a problem if we
         * are out of memory and rehash isn't working.
         */
        ++hash->count;
        اگر (hash->count == hash->length) {
            /* Don't allow count to reach length */
            --hash->count;
            *حالت_2 = U_حافظہ_تخصیص_ERROR;
            جاؤ err;
        }
    }

    /* We must in all cases handle storage properly.  If there was an
     * old key, then it must be deleted (if the deleter != nullptr).
     * Make hashcodes stored in table positive.
     */
    واپس uhash_سیٹ_Element(hash, e, hashcode & 0x7FFFFFFF, کلید, value, hint);

 err:
    /* If the deleters are non-nullptr, this method adopts its key and/or
     * value arguments, and we must be sure to delete the key and/or
     * value in all cases, even upon failure.
     */
    HASH_DELETE_کلید_قدر(hash, کلید.اشارہ_گر, value.اشارہ_گر);
    emptytok.اشارہ_گر = خالی_اشاریہ; emptytok.integer = 0;
    واپس emptytok;
}


/********************************************************************
 * PUBLIC API
 ********************************************************************/

U_CAPI UHashtable* U_EXPORT2
uhash_open(UHashFunction *کلید_Hash,
           UKeyComparator *کلید_Comp,
           UValueComparator *قدر_Comp,
           UError_رمز *حالت_2) {

    واپس _uhash_create(کلید_Hash, کلید_Comp, قدر_Comp, DEFAULT_مفرد_اشاریہ, حالت_2);
}

U_CAPI UHashtable* U_EXPORT2
uhash_open_سائز(UHashFunction *کلید_Hash,
               UKeyComparator *کلید_Comp,
               UValueComparator *قدر_Comp,
               int32_t size,
               UError_رمز *حالت_2) {

    /* Find the smallest index i for which PRIMES[i] >= size. */
    int32_t i = 0;
    جب (i<(مفرد_اعداد_لمبائی-1) && مفرد_اعداد[i]<size) {
        ++i;
    }

    واپس _uhash_create(کلید_Hash, کلید_Comp, قدر_Comp, i, حالت_2);
}

U_CAPI UHashtable* U_EXPORT2
uhash_init(UHashtable *fillin_نتیجہ,
           UHashFunction *کلید_Hash,
           UKeyComparator *کلید_Comp,
           UValueComparator *قدر_Comp,
           UError_رمز *حالت_2) {

    واپس _uhash_init(fillin_نتیجہ, کلید_Hash, کلید_Comp, قدر_Comp, DEFAULT_مفرد_اشاریہ, حالت_2);
}

U_CAPI UHashtable* U_EXPORT2
uhash_init_سائز(UHashtable *fillin_نتیجہ,
               UHashFunction *کلید_Hash,
               UKeyComparator *کلید_Comp,
               UValueComparator *قدر_Comp,
               int32_t size,
               UError_رمز *حالت_2) {

    // Find the smallest index i for which PRIMES[i] >= size.
    int32_t i = 0;
    جب (i<(مفرد_اعداد_لمبائی-1) && مفرد_اعداد[i]<size) {
        ++i;
    }
    واپس _uhash_init(fillin_نتیجہ, کلید_Hash, کلید_Comp, قدر_Comp, i, حالت_2);
}

U_CAPI خالی U_EXPORT2
uhash_close(UHashtable *hash) {
    اگر (hash == خالی_اشاریہ) {
        واپس;
    }
    اگر (hash->elements != خالی_اشاریہ) {
        اگر (hash->کلید_Deleter != خالی_اشاریہ || hash->قدر_Deleter != خالی_اشاریہ) {
            int32_t pos=UHASH_پہلا;
            UHashElement *e;
            جب ((e = (UHashElement*) uhash_اگلا_Element(hash, &pos)) != خالی_اشاریہ) {
                HASH_DELETE_کلید_قدر(hash, e->کلید.اشارہ_گر, e->value.اشارہ_گر);
            }
        }
        uprv_free(hash->elements);
        hash->elements = خالی_اشاریہ;
    }
    اگر (hash->allocated) {
        uprv_free(hash);
    }
}

U_CAPI UHashFunction *U_EXPORT2
uhash_سیٹ_کلید_Hasher(UHashtable *hash, UHashFunction *fn) {
    UHashFunction *نتیجہ = hash->کلید_Hasher;
    hash->کلید_Hasher = fn;
    واپس نتیجہ;
}

U_CAPI UKeyComparator *U_EXPORT2
uhash_سیٹ_کلید_Comparator(UHashtable *hash, UKeyComparator *fn) {
    UKeyComparator *نتیجہ = hash->کلید_Comparator;
    hash->کلید_Comparator = fn;
    واپس نتیجہ;
}
U_CAPI UValueComparator *U_EXPORT2
uhash_سیٹ_قدر_Comparator(UHashtable *hash, UValueComparator *fn){
    UValueComparator *نتیجہ = hash->قدر_Comparator;
    hash->قدر_Comparator = fn;
    واپس نتیجہ;
}

U_CAPI UObjectDeleter *U_EXPORT2
uhash_سیٹ_کلید_Deleter(UHashtable *hash, UObjectDeleter *fn) {
    UObjectDeleter *نتیجہ = hash->کلید_Deleter;
    hash->کلید_Deleter = fn;
    واپس نتیجہ;
}

U_CAPI UObjectDeleter *U_EXPORT2
uhash_سیٹ_قدر_Deleter(UHashtable *hash, UObjectDeleter *fn) {
    UObjectDeleter *نتیجہ = hash->قدر_Deleter;
    hash->قدر_Deleter = fn;
    واپس نتیجہ;
}

U_CAPI خالی U_EXPORT2
uhash_سیٹ_Resize_Policy(UHashtable *hash, شمار UHashResizePolicy policy) {
    UError_رمز حالت_2 = U_ZERO_ERROR;
    uhash_اندرونی_سیٹ_Resize_Policy(hash, policy);
    hash->کم_Water_Mark  = (int32_t)(hash->length * hash->کم_Water_تناسب);
    hash->بلند_Water_Mark = (int32_t)(hash->length * hash->بلند_Water_تناسب);
    _uhash_rehash(hash, &حالت_2);
}

U_CAPI int32_t U_EXPORT2
uhash_گنتی(ثابت UHashtable *hash) {
    واپس hash->count;
}

U_CAPI خالی* U_EXPORT2
uhash_get(ثابت UHashtable *hash,
          ثابت خالی* کلید) {
    UHashTok keyholder;
    keyholder.اشارہ_گر = (خالی*) کلید;
    واپس uhash_ڈھونڈو_2(hash, keyholder, hash->کلید_Hasher(keyholder))->value.اشارہ_گر;
}

U_CAPI خالی* U_EXPORT2
uhash_iget(ثابت UHashtable *hash,
           int32_t کلید) {
    UHashTok keyholder;
    keyholder.integer = کلید;
    واپس uhash_ڈھونڈو_2(hash, keyholder, hash->کلید_Hasher(keyholder))->value.اشارہ_گر;
}

U_CAPI int32_t U_EXPORT2
uhash_geti(ثابت UHashtable *hash,
           ثابت خالی* کلید) {
    UHashTok keyholder;
    keyholder.اشارہ_گر = (خالی*) کلید;
    واپس uhash_ڈھونڈو_2(hash, keyholder, hash->کلید_Hasher(keyholder))->value.integer;
}

U_CAPI int32_t U_EXPORT2
uhash_igeti(ثابت UHashtable *hash,
           int32_t کلید) {
    UHashTok keyholder;
    keyholder.integer = کلید;
    واپس uhash_ڈھونڈو_2(hash, keyholder, hash->کلید_Hasher(keyholder))->value.integer;
}

U_CAPI int32_t U_EXPORT2
uhash_geti_And_ملا(ثابت UHashtable *hash,
                   ثابت خالی *کلید,
                   UBool *ملا) {
    UHashTok keyholder;
    keyholder.اشارہ_گر = (خالی *)کلید;
    ثابت UHashElement *e = uhash_ڈھونڈو_2(hash, keyholder, hash->کلید_Hasher(keyholder));
    *ملا = !IS_خالی_خانے_OR_DELETED(e->hashcode);
    واپس e->value.integer;
}

U_CAPI int32_t U_EXPORT2
uhash_igeti_And_ملا(ثابت UHashtable *hash,
                    int32_t کلید,
                    UBool *ملا) {
    UHashTok keyholder;
    keyholder.integer = کلید;
    ثابت UHashElement *e = uhash_ڈھونڈو_2(hash, keyholder, hash->کلید_Hasher(keyholder));
    *ملا = !IS_خالی_خانے_OR_DELETED(e->hashcode);
    واپس e->value.integer;
}

U_CAPI خالی* U_EXPORT2
uhash_put(UHashtable *hash,
          خالی* کلید,
          خالی* value,
          UError_رمز *حالت_2) {
    UHashTok keyholder, valueholder;
    keyholder.اشارہ_گر = کلید;
    valueholder.اشارہ_گر = value;
    واپس _uhash_put(hash, keyholder, valueholder,
                      HINT_کلید_اشارہ_گر | HINT_قدر_اشارہ_گر,
                      حالت_2).اشارہ_گر;
}

U_CAPI خالی* U_EXPORT2
uhash_iput(UHashtable *hash,
           int32_t کلید,
           خالی* value,
           UError_رمز *حالت_2) {
    UHashTok keyholder, valueholder;
    keyholder.integer = کلید;
    valueholder.اشارہ_گر = value;
    واپس _uhash_put(hash, keyholder, valueholder,
                      HINT_قدر_اشارہ_گر,
                      حالت_2).اشارہ_گر;
}

U_CAPI int32_t U_EXPORT2
uhash_puti(UHashtable *hash,
           خالی* کلید,
           int32_t value,
           UError_رمز *حالت_2) {
    UHashTok keyholder, valueholder;
    keyholder.اشارہ_گر = کلید;
    valueholder.integer = value;
    واپس _uhash_put(hash, keyholder, valueholder,
                      HINT_کلید_اشارہ_گر,
                      حالت_2).integer;
}


U_CAPI int32_t U_EXPORT2
uhash_iputi(UHashtable *hash,
           int32_t کلید,
           int32_t value,
           UError_رمز *حالت_2) {
    UHashTok keyholder, valueholder;
    keyholder.integer = کلید;
    valueholder.integer = value;
    واپس _uhash_put(hash, keyholder, valueholder,
                      HINT_BOTH_INTEGERS,
                      حالت_2).integer;
}

U_CAPI int32_t U_EXPORT2
uhash_putiAllowZero(UHashtable *hash,
                    خالی *کلید,
                    int32_t value,
                    UError_رمز *حالت_2) {
    UHashTok keyholder, valueholder;
    keyholder.اشارہ_گر = کلید;
    valueholder.integer = value;
    واپس _uhash_put(hash, keyholder, valueholder,
                      HINT_کلید_اشارہ_گر | HINT_ALLOW_ZERO,
                      حالت_2).integer;
}


U_CAPI int32_t U_EXPORT2
uhash_iputiAllowZero(UHashtable *hash,
                     int32_t کلید,
                     int32_t value,
                     UError_رمز *حالت_2) {
    UHashTok keyholder, valueholder;
    keyholder.integer = کلید;
    valueholder.integer = value;
    واپس _uhash_put(hash, keyholder, valueholder,
                      HINT_BOTH_INTEGERS | HINT_ALLOW_ZERO,
                      حالت_2).integer;
}

U_CAPI خالی* U_EXPORT2
uhash_remove(UHashtable *hash,
             ثابت خالی* کلید) {
    UHashTok keyholder;
    keyholder.اشارہ_گر = (خالی*) کلید;
    واپس _uhash_remove(hash, keyholder).اشارہ_گر;
}

U_CAPI خالی* U_EXPORT2
uhash_iremove(UHashtable *hash,
              int32_t کلید) {
    UHashTok keyholder;
    keyholder.integer = کلید;
    واپس _uhash_remove(hash, keyholder).اشارہ_گر;
}

U_CAPI int32_t U_EXPORT2
uhash_removei(UHashtable *hash,
              ثابت خالی* کلید) {
    UHashTok keyholder;
    keyholder.اشارہ_گر = (خالی*) کلید;
    واپس _uhash_remove(hash, keyholder).integer;
}

U_CAPI int32_t U_EXPORT2
uhash_iremovei(UHashtable *hash,
               int32_t کلید) {
    UHashTok keyholder;
    keyholder.integer = کلید;
    واپس _uhash_remove(hash, keyholder).integer;
}

U_CAPI خالی U_EXPORT2
uhash_removeAll(UHashtable *hash) {
    int32_t pos = UHASH_پہلا;
    ثابت UHashElement *e;
    U_ASSERT(hash != خالی_اشاریہ);
    اگر (hash->count != 0) {
        جب ((e = uhash_اگلا_Element(hash, &pos)) != خالی_اشاریہ) {
            uhash_removeElement(hash, e);
        }
    }
    U_ASSERT(hash->count == 0);
}

U_CAPI UBool U_EXPORT2
uhash_شامل_ہے_کلید(ثابت UHashtable *hash, ثابت خالی *کلید) {
    UHashTok keyholder;
    keyholder.اشارہ_گر = (خالی *)کلید;
    ثابت UHashElement *e = uhash_ڈھونڈو_2(hash, keyholder, hash->کلید_Hasher(keyholder));
    واپس !IS_خالی_خانے_OR_DELETED(e->hashcode);
}

/**
 * Returns true if the UHashtable contains an item with this integer key.
 *
 * @param hash The target UHashtable.
 * @param key An integer key stored in a hashtable
 * @return true if the key is found.
 */
U_CAPI UBool U_EXPORT2
uhash_icontains_کلید(ثابت UHashtable *hash, int32_t کلید) {
    UHashTok keyholder;
    keyholder.integer = کلید;
    ثابت UHashElement *e = uhash_ڈھونڈو_2(hash, keyholder, hash->کلید_Hasher(keyholder));
    واپس !IS_خالی_خانے_OR_DELETED(e->hashcode);
}

U_CAPI ثابت UHashElement* U_EXPORT2
uhash_ڈھونڈو(ثابت UHashtable *hash, ثابت خالی* کلید) {
    UHashTok keyholder;
    ثابت UHashElement *e;
    keyholder.اشارہ_گر = (خالی*) کلید;
    e = uhash_ڈھونڈو_2(hash, keyholder, hash->کلید_Hasher(keyholder));
    واپس IS_خالی_خانے_OR_DELETED(e->hashcode) ? خالی_اشاریہ : e;
}

U_CAPI ثابت UHashElement* U_EXPORT2
uhash_اگلا_Element(ثابت UHashtable *hash, int32_t *pos) {
    /* Walk through the array until we find an element that is not
     * EMPTY and not DELETED.
     */
    int32_t i;
    U_ASSERT(hash != خالی_اشاریہ);
    برائے (i = *pos + 1; i < hash->length; ++i) {
        اگر (!IS_خالی_خانے_OR_DELETED(hash->elements[i].hashcode)) {
            *pos = i;
            واپس &(hash->elements[i]);
        }
    }

    /* No more elements */
    واپس خالی_اشاریہ;
}

U_CAPI خالی* U_EXPORT2
uhash_removeElement(UHashtable *hash, ثابت UHashElement* e) {
    U_ASSERT(hash != خالی_اشاریہ);
    U_ASSERT(e != خالی_اشاریہ);
    اگر (!IS_خالی_خانے_OR_DELETED(e->hashcode)) {
        UHashElement *nce = (UHashElement *)e;
        واپس uhash_اندرونی_Remove_Element(hash, nce).اشارہ_گر;
    }
    واپس خالی_اشاریہ;
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
uhash_hashUChars(ثابت UHashTok کلید) {
    ثابت char16_t *s = (ثابت char16_t *)کلید.اشارہ_گر;
    واپس s == خالی_اشاریہ ? 0 : ustr_hashUCharsN(s, u_strlen(s));
}

U_CAPI int32_t U_EXPORT2
uhash_hashChars(ثابت UHashTok کلید) {
    ثابت حرف *s = (ثابت حرف *)کلید.اشارہ_گر;
    واپس s == خالی_اشاریہ ? 0 : static_cast<int32_t>(ustr_hashCharsN(s, static_cast<int32_t>(uprv_strlen(s))));
}

U_CAPI int32_t U_EXPORT2
uhash_hashIChars(ثابت UHashTok کلید) {
    ثابت حرف *s = (ثابت حرف *)کلید.اشارہ_گر;
    واپس s == خالی_اشاریہ ? 0 : ustr_hashICharsN(s, static_cast<int32_t>(uprv_strlen(s)));
}

U_CAPI int32_t U_EXPORT2
uhash_hashIStringView(ثابت UHashTok کلید) {
    ثابت std::string_view* s = static_cast<std::string_view*>(کلید.اشارہ_گر);
    واپس s == خالی_اشاریہ ? 0 : ustr_hashICharsN(s->ڈیٹا(), static_cast<int32_t>(s->size()));
}

U_CAPI UBool U_EXPORT2
uhash_equals(ثابت UHashtable* hash1, ثابت UHashtable* hash2){
    int32_t count1, count2, pos, i;

    اگر(hash1==hash2){
        واپس سچ;
    }

    /*
     * Make sure that we are comparing 2 valid hashes of the same type
     * with valid comparison functions.
     * Without valid comparison functions, a binary comparison
     * of the hash values will yield random results on machines
     * with 64-bit pointers and 32-bit integer hashes.
     * A valueComparator is normally optional.
     */
    اگر (hash1==خالی_اشاریہ || hash2==خالی_اشاریہ ||
        hash1->کلید_Comparator != hash2->کلید_Comparator ||
        hash1->قدر_Comparator != hash2->قدر_Comparator ||
        hash1->قدر_Comparator == خالی_اشاریہ)
    {
        /*
        Normally we would return an error here about incompatible hash tables,
        but we return false instead.
        */
        واپس جھوٹ;
    }

    count1 = uhash_گنتی(hash1);
    count2 = uhash_گنتی(hash2);
    اگر(count1!=count2){
        واپس جھوٹ;
    }

    pos=UHASH_پہلا;
    برائے(i=0; i<count1; i++){
        ثابت UHashElement* elem1 = uhash_اگلا_Element(hash1, &pos);
        ثابت UHashTok key1 = elem1->کلید;
        ثابت UHashTok val1 = elem1->value;
        /* here the keys are not compared, instead the key form hash1 is used to fetch
         * value from hash2. If the hashes are equal then then both hashes should
         * contain equal values for the same key!
         */
        ثابت UHashElement* elem2 = uhash_ڈھونڈو_2(hash2, key1, hash2->کلید_Hasher(key1));
        ثابت UHashTok val2 = elem2->value;
        اگر(hash1->قدر_Comparator(val1, val2)==جھوٹ){
            واپس جھوٹ;
        }
    }
    واپس سچ;
}

/********************************************************************
 * PUBLIC Comparator Functions
 ********************************************************************/

U_CAPI UBool U_EXPORT2
uhash_compareUChars(ثابت UHashTok key1, ثابت UHashTok key2) {
    ثابت char16_t *p1 = (ثابت char16_t*) key1.اشارہ_گر;
    ثابت char16_t *p2 = (ثابت char16_t*) key2.اشارہ_گر;
    اگر (p1 == p2) {
        واپس سچ;
    }
    اگر (p1 == خالی_اشاریہ || p2 == خالی_اشاریہ) {
        واپس جھوٹ;
    }
    جب (*p1 != 0 && *p1 == *p2) {
        ++p1;
        ++p2;
    }
    واپس *p1 == *p2;
}

U_CAPI UBool U_EXPORT2
uhash_compareChars(ثابت UHashTok key1, ثابت UHashTok key2) {
    ثابت حرف *p1 = (ثابت حرف*) key1.اشارہ_گر;
    ثابت حرف *p2 = (ثابت حرف*) key2.اشارہ_گر;
    اگر (p1 == p2) {
        واپس سچ;
    }
    اگر (p1 == خالی_اشاریہ || p2 == خالی_اشاریہ) {
        واپس جھوٹ;
    }
    جب (*p1 != 0 && *p1 == *p2) {
        ++p1;
        ++p2;
    }
    واپس *p1 == *p2;
}

U_CAPI UBool U_EXPORT2
uhash_compareIChars(ثابت UHashTok key1, ثابت UHashTok key2) {
    ثابت حرف *p1 = (ثابت حرف*) key1.اشارہ_گر;
    ثابت حرف *p2 = (ثابت حرف*) key2.اشارہ_گر;
    اگر (p1 == p2) {
        واپس سچ;
    }
    اگر (p1 == خالی_اشاریہ || p2 == خالی_اشاریہ) {
        واپس جھوٹ;
    }
    جب (*p1 != 0 && uprv_tolower(*p1) == uprv_tolower(*p2)) {
        ++p1;
        ++p2;
    }
    واپس *p1 == *p2;
}

U_CAPI UBool U_EXPORT2
uhash_compareIStringView(ثابت UHashTok key1, ثابت UHashTok key2) {
    ثابت std::string_view* p1 = static_cast<std::string_view*>(key1.اشارہ_گر);
    ثابت std::string_view* p2 = static_cast<std::string_view*>(key2.اشارہ_گر);
    اگر (p1 == p2) {
        واپس سچ;
    }
    اگر (p1 == خالی_اشاریہ || p2 == خالی_اشاریہ) {
        واپس جھوٹ;
    }
    ثابت std::string_view& v1 = *p1;
    ثابت std::string_view& v2 = *p2;
    اگر (v1.size() != v2.size()) {
        واپس جھوٹ;
    }
    برائے (size_t i = 0; i < v1.size(); ++i) {
        اگر (uprv_tolower(v1[i]) != uprv_tolower(v2[i])) {
            واپس جھوٹ;
        }
    }
    واپس سچ;
}

/********************************************************************
 * PUBLIC int32_t Support Functions
 ********************************************************************/

U_CAPI int32_t U_EXPORT2
uhash_hashLong(ثابت UHashTok کلید) {
    واپس کلید.integer;
}

U_CAPI UBool U_EXPORT2
uhash_compareLong(ثابت UHashTok key1, ثابت UHashTok key2) {
    واپس key1.integer == key2.integer;
}
