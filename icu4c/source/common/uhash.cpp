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

#अन्तर्भाव <string_view>

#अन्तर्भाव "uhash.h"
#अन्तर्भाव "unicode/ustring.h"
#अन्तर्भाव "cstring.h"
#अन्तर्भाव "cmemory.h"
#अन्तर्भाव "uassert.h"
#अन्तर्भाव "ustr_imp.h"

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
स्थिर ध्रुव int32_t अभाज्याः[] = {
    7, 13, 31, 61, 127, 251, 509, 1021, 2039, 4093, 8191, 16381, 32749,
    65521, 131071, 262139, 524287, 1048573, 2097143, 4194301, 8388593,
    16777213, 33554393, 67108859, 134217689, 268435399, 536870909,
    1073741789, 2147483647 /*, 4294967291 */
};

#परिभाषय अभाज्याः_दीर्घता UPRV_LENGTHOF(अभाज्याः)
#परिभाषय DEFAULT_अविभाज्यः_सूचकांक 4

/* These ratios are tuned to the PRIMES array such that a resize
 * places the table back into the zone of non-resizing.  That is,
 * after a call to _uhash_rehash(), a subsequent call to
 * _uhash_rehash() should do nothing (should not churn).  This is only
 * a potential problem with U_GROW_AND_SHRINK.
 */
स्थिर ध्रुव भिन्न RESIZE_POLICY_अनुपातः_सारणी[6] = {
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
#परिभाषय HASH_DELETED    ((int32_t) 0x80000000)
#परिभाषय HASH_रिक्तानि      ((int32_t) HASH_DELETED + 1)

#परिभाषय IS_रिक्तानि_OR_DELETED(x) ((x) < 0)

/* This macro expects a UHashTok.pointer as its keypointer and
   valuepointer parameters */
#परिभाषय HASH_DELETE_कुञ्चिका_मानम्(hash, keypointer, valuepointer) UPRV_खण्डम्_MACRO_BEGIN { \
    यदि (hash->कुञ्चिका_Deleter != शून्यसूचकः && keypointer != शून्यसूचकः) { \
        (*hash->कुञ्चिका_Deleter)(keypointer); \
    } \
    यदि (hash->मानम्_Deleter != शून्यसूचकः && valuepointer != शून्यसूचकः) { \
        (*hash->मानम्_Deleter)(valuepointer); \
    } \
} UPRV_खण्डम्_MACRO_अन्तः

/*
 * Constants for hinting whether a key or value is an integer
 * or a pointer.  If a hint bit is zero, then the associated
 * token is assumed to be an integer.
 */
#परिभाषय HINT_BOTH_INTEGERS (0)
#परिभाषय HINT_कुञ्चिका_POINTER   (1)
#परिभाषय HINT_मानम्_POINTER (2)
#परिभाषय HINT_ALLOW_ZERO    (4)

/********************************************************************
 * PRIVATE Implementation
 ********************************************************************/

स्थिर UHashTok
uhash_समुच्चयः_Element(UHashtable *hash, UHashElement* e,
                  int32_t hashcode,
                  UHashTok कुञ्चिका, UHashTok value, int8_t hint) {

    UHashTok old_मानम् = e->value;
    यदि (hash->कुञ्चिका_Deleter != शून्यसूचकः && e->कुञ्चिका.pointer != शून्यसूचकः &&
        e->कुञ्चिका.pointer != कुञ्चिका.pointer) { /* Avoid double deletion */
        (*hash->कुञ्चिका_Deleter)(e->कुञ्चिका.pointer);
    }
    यदि (hash->मानम्_Deleter != शून्यसूचकः) {
        यदि (old_मानम्.pointer != शून्यसूचकः &&
            old_मानम्.pointer != value.pointer) { /* Avoid double deletion */
            (*hash->मानम्_Deleter)(old_मानम्.pointer);
        }
        old_मानम्.pointer = शून्यसूचकः;
    }
    /* Compilers should copy the UHashTok union correctly, but even if
     * they do, memory heap tools (e.g. BoundsChecker) can get
     * confused when a pointer is cloaked in a union and then copied.
     * TO ALLEVIATE THIS, we use hints (based on what API the user is
     * calling) to copy pointers when we know the user thinks
     * something is a pointer. */
    यदि (hint & HINT_कुञ्चिका_POINTER) {
        e->कुञ्चिका.pointer = कुञ्चिका.pointer;
    } अन्यथा {
        e->कुञ्चिका = कुञ्चिका;
    }
    यदि (hint & HINT_मानम्_POINTER) {
        e->value.pointer = value.pointer;
    } अन्यथा {
        e->value = value;
    }
    e->hashcode = hashcode;
    प्रत्यागम old_मानम्;
}

/**
 * Assumes that the given element is not empty or deleted.
 */
स्थिर UHashTok
uhash_आन्तरिकम्_Remove_Element(UHashtable *hash, UHashElement* e) {
    UHashTok empty;
    U_ASSERT(!IS_रिक्तानि_OR_DELETED(e->hashcode));
    --hash->count;
    empty.pointer = शून्यसूचकः; empty.integer = 0;
    प्रत्यागम uhash_समुच्चयः_Element(hash, e, HASH_DELETED, empty, empty, 0);
}

स्थिर शून्य
uhash_आन्तरिकम्_समुच्चयः_Resize_Policy(UHashtable *hash, गणना UHashResizePolicy policy) {
    U_ASSERT(hash != शून्यसूचकः);
    U_ASSERT(((int32_t)policy) >= 0);
    U_ASSERT(((int32_t)policy) < 3);
    hash->नीचः_Water_अनुपातः  = RESIZE_POLICY_अनुपातः_सारणी[policy * 2];
    hash->उच्चः_Water_अनुपातः = RESIZE_POLICY_अनुपातः_सारणी[policy * 2 + 1];
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
स्थिर शून्य
_uhash_allocate(UHashtable *hash,
                int32_t अविभाज्यः_सूचकांक,
                UError_संहिता *स्थितिः) {

    UHashElement *p, *सीमा;
    UHashTok emptytok;

    यदि (U_FAILURE(*स्थितिः)) प्रत्यागम;

    U_ASSERT(अविभाज्यः_सूचकांक >= 0 && अविभाज्यः_सूचकांक < अभाज्याः_दीर्घता);

    hash->अविभाज्यः_सूचकांक = static_cast<int8_t>(अविभाज्यः_सूचकांक);
    hash->length = अभाज्याः[अविभाज्यः_सूचकांक];

    p = hash->elements = static_cast<UHashElement*>(
        uprv_malloc(परिमाण(UHashElement) * hash->length));

    यदि (hash->elements == शून्यसूचकः) {
        *स्थितिः = U_स्मृतिः_आवण्टनम्_ERROR;
        प्रत्यागम;
    }

    emptytok.pointer = शून्यसूचकः; /* Only one of these two is needed */
    emptytok.integer = 0;    /* but we don't know which one. */

    सीमा = p + hash->length;
    यावत् (p < सीमा) {
        p->कुञ्चिका = emptytok;
        p->value = emptytok;
        p->hashcode = HASH_रिक्तानि;
        ++p;
    }

    hash->count = 0;
    hash->नीचः_Water_Mark = static_cast<int32_t>(hash->length * hash->नीचः_Water_अनुपातः);
    hash->उच्चः_Water_Mark = static_cast<int32_t>(hash->length * hash->उच्चः_Water_अनुपातः);
}

स्थिर UHashtable*
_uhash_init(UHashtable *परिणाम,
              UHashFunction *कुञ्चिका_Hash,
              UKeyComparator *कुञ्चिका_Comp,
              UValueComparator *मानम्_Comp,
              int32_t अविभाज्यः_सूचकांक,
              UError_संहिता *स्थितिः)
{
    यदि (U_FAILURE(*स्थितिः)) प्रत्यागम शून्यसूचकः;
    U_ASSERT(कुञ्चिका_Hash != शून्यसूचकः);
    U_ASSERT(कुञ्चिका_Comp != शून्यसूचकः);

    परिणाम->कुञ्चिका_Hasher       = कुञ्चिका_Hash;
    परिणाम->कुञ्चिका_Comparator   = कुञ्चिका_Comp;
    परिणाम->मानम्_Comparator = मानम्_Comp;
    परिणाम->कुञ्चिका_Deleter      = शून्यसूचकः;
    परिणाम->मानम्_Deleter    = शून्यसूचकः;
    परिणाम->allocated       = असत्य;
    uhash_आन्तरिकम्_समुच्चयः_Resize_Policy(परिणाम, U_GROW);

    _uhash_allocate(परिणाम, अविभाज्यः_सूचकांक, स्थितिः);

    यदि (U_FAILURE(*स्थितिः)) {
        प्रत्यागम शून्यसूचकः;
    }

    प्रत्यागम परिणाम;
}

स्थिर UHashtable*
_uhash_create(UHashFunction *कुञ्चिका_Hash,
              UKeyComparator *कुञ्चिका_Comp,
              UValueComparator *मानम्_Comp,
              int32_t अविभाज्यः_सूचकांक,
              UError_संहिता *स्थितिः) {
    UHashtable *परिणाम;

    यदि (U_FAILURE(*स्थितिः)) प्रत्यागम शून्यसूचकः;

    परिणाम = static_cast<UHashtable*>(uprv_malloc(परिमाण(UHashtable)));
    यदि (परिणाम == शून्यसूचकः) {
        *स्थितिः = U_स्मृतिः_आवण्टनम्_ERROR;
        प्रत्यागम शून्यसूचकः;
    }

    _uhash_init(परिणाम, कुञ्चिका_Hash, कुञ्चिका_Comp, मानम्_Comp, अविभाज्यः_सूचकांक, स्थितिः);
    परिणाम->allocated       = सत्य;

    यदि (U_FAILURE(*स्थितिः)) {
        uprv_free(परिणाम);
        प्रत्यागम शून्यसूचकः;
    }

    प्रत्यागम परिणाम;
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
स्थिर UHashElement*
_uhash_find(ध्रुव UHashtable *hash, UHashTok कुञ्चिका,
            int32_t hashcode) {

    int32_t firstDeleted = -1;  /* assume invalid index */
    int32_t the_सूचकांक, start_सूचकांक;
    int32_t jump = 0; /* lazy evaluate */
    int32_t सारणी_Hash;
    UHashElement *elements = hash->elements;

    hashcode &= 0x7FFFFFFF; /* must be positive */
    start_सूचकांक = the_सूचकांक = (hashcode ^ 0x4000000) % hash->length;

    कुरु {
        सारणी_Hash = elements[the_सूचकांक].hashcode;
        यदि (सारणी_Hash == hashcode) {          /* quick check */
            यदि ((*hash->कुञ्चिका_Comparator)(कुञ्चिका, elements[the_सूचकांक].कुञ्चिका)) {
                प्रत्यागम &(elements[the_सूचकांक]);
            }
        } अन्यथा यदि (!IS_रिक्तानि_OR_DELETED(सारणी_Hash)) {
            /* We have hit a slot which contains a key-value pair,
             * but for which the hash code does not match.  Keep
             * looking.
             */
        } अन्यथा यदि (सारणी_Hash == HASH_रिक्तानि) { /* empty, end o' the line */
            भङ्ग;
        } अन्यथा यदि (firstDeleted < 0) { /* remember first deleted */
            firstDeleted = the_सूचकांक;
        }
        यदि (jump == 0) { /* lazy compute jump */
            /* The jump value must be relatively prime to the table
             * length.  As long as the length is prime, then any value
             * 1..length-1 will be relatively prime to it.
             */
            jump = (hashcode % (hash->length - 1)) + 1;
        }
        the_सूचकांक = (the_सूचकांक + jump) % hash->length;
    } यावत् (the_सूचकांक != start_सूचकांक);

    यदि (firstDeleted >= 0) {
        the_सूचकांक = firstDeleted; /* reset if had deleted slot */
    } अन्यथा यदि (सारणी_Hash != HASH_रिक्तानि) {
        /* We get to this point if the hashtable is full (no empty or
         * deleted slots), and we've failed to find a match.  THIS
         * WILL NEVER HAPPEN as long as uhash_put() makes sure that
         * count is always < length.
         */
        UPRV_UNREACHABLE_EXIT;
    }
    प्रत्यागम &(elements[the_सूचकांक]);
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
स्थिर शून्य
_uhash_rehash(UHashtable *hash, UError_संहिता *स्थितिः) {

    UHashElement *old = hash->elements;
    int32_t old_दीर्घता = hash->length;
    int32_t new_अविभाज्यः_सूचकांक = hash->अविभाज्यः_सूचकांक;
    int32_t i;

    यदि (hash->count > hash->उच्चः_Water_Mark) {
        यदि (++new_अविभाज्यः_सूचकांक >= अभाज्याः_दीर्घता) {
            प्रत्यागम;
        }
    } अन्यथा यदि (hash->count < hash->नीचः_Water_Mark) {
        यदि (--new_अविभाज्यः_सूचकांक < 0) {
            प्रत्यागम;
        }
    } अन्यथा {
        प्रत्यागम;
    }

    _uhash_allocate(hash, new_अविभाज्यः_सूचकांक, स्थितिः);

    यदि (U_FAILURE(*स्थितिः)) {
        hash->elements = old;
        hash->length = old_दीर्घता;
        प्रत्यागम;
    }

    प्रति (i = old_दीर्घता - 1; i >= 0; --i) {
        यदि (!IS_रिक्तानि_OR_DELETED(old[i].hashcode)) {
            UHashElement *e = _uhash_find(hash, old[i].कुञ्चिका, old[i].hashcode);
            U_ASSERT(e != शून्यसूचकः);
            U_ASSERT(e->hashcode == HASH_रिक्तानि);
            e->कुञ्चिका = old[i].कुञ्चिका;
            e->value = old[i].value;
            e->hashcode = old[i].hashcode;
            ++hash->count;
        }
    }

    uprv_free(old);
}

स्थिर UHashTok
_uhash_remove(UHashtable *hash,
              UHashTok कुञ्चिका) {
    /* First find the position of the key in the table.  If the object
     * has not been removed already, remove it.  If the user wanted
     * keys deleted, then delete it also.  We have to put a special
     * hashcode in that position that means that something has been
     * deleted, since when we do a find, we have to continue PAST any
     * deleted values.
     */
    UHashTok परिणाम;
    UHashElement* e = _uhash_find(hash, कुञ्चिका, hash->कुञ्चिका_Hasher(कुञ्चिका));
    U_ASSERT(e != शून्यसूचकः);
    परिणाम.pointer = शून्यसूचकः;
    परिणाम.integer = 0;
    यदि (!IS_रिक्तानि_OR_DELETED(e->hashcode)) {
        परिणाम = uhash_आन्तरिकम्_Remove_Element(hash, e);
        यदि (hash->count < hash->नीचः_Water_Mark) {
            UError_संहिता स्थितिः = U_ZERO_ERROR;
            _uhash_rehash(hash, &स्थितिः);
        }
    }
    प्रत्यागम परिणाम;
}

स्थिर UHashTok
_uhash_put(UHashtable *hash,
           UHashTok कुञ्चिका,
           UHashTok value,
           int8_t hint,
           UError_संहिता *स्थितिः) {

    /* Put finds the position in the table for the new value.  If the
     * key is already in the table, it is deleted, if there is a
     * non-nullptr keyDeleter.  Then the key, the hash and the value are
     * all put at the position in their respective arrays.
     */
    int32_t hashcode;
    UHashElement* e;
    UHashTok emptytok;

    यदि (U_FAILURE(*स्थितिः)) {
        गच्छ err;
    }
    U_ASSERT(hash != शून्यसूचकः);
    यदि ((hint & HINT_मानम्_POINTER) ?
            value.pointer == शून्यसूचकः :
            value.integer == 0 && (hint & HINT_ALLOW_ZERO) == 0) {
        /* Disallow storage of nullptr values, since nullptr is returned by
         * get() to indicate an absent key.  Storing nullptr == removing.
         */
        प्रत्यागम _uhash_remove(hash, कुञ्चिका);
    }
    यदि (hash->count > hash->उच्चः_Water_Mark) {
        _uhash_rehash(hash, स्थितिः);
        यदि (U_FAILURE(*स्थितिः)) {
            गच्छ err;
        }
    }

    hashcode = (*hash->कुञ्चिका_Hasher)(कुञ्चिका);
    e = _uhash_find(hash, कुञ्चिका, hashcode);
    U_ASSERT(e != शून्यसूचकः);

    यदि (IS_रिक्तानि_OR_DELETED(e->hashcode)) {
        /* Important: We must never actually fill the table up.  If we
         * do so, then _uhash_find() will return nullptr, and we'll have
         * to check for nullptr after every call to _uhash_find().  To
         * avoid this we make sure there is always at least one empty
         * or deleted slot in the table.  This only is a problem if we
         * are out of memory and rehash isn't working.
         */
        ++hash->count;
        यदि (hash->count == hash->length) {
            /* Don't allow count to reach length */
            --hash->count;
            *स्थितिः = U_स्मृतिः_आवण्टनम्_ERROR;
            गच्छ err;
        }
    }

    /* We must in all cases handle storage properly.  If there was an
     * old key, then it must be deleted (if the deleter != nullptr).
     * Make hashcodes stored in table positive.
     */
    प्रत्यागम uhash_समुच्चयः_Element(hash, e, hashcode & 0x7FFFFFFF, कुञ्चिका, value, hint);

 err:
    /* If the deleters are non-nullptr, this method adopts its key and/or
     * value arguments, and we must be sure to delete the key and/or
     * value in all cases, even upon failure.
     */
    HASH_DELETE_कुञ्चिका_मानम्(hash, कुञ्चिका.pointer, value.pointer);
    emptytok.pointer = शून्यसूचकः; emptytok.integer = 0;
    प्रत्यागम emptytok;
}


/********************************************************************
 * PUBLIC API
 ********************************************************************/

U_CAPI UHashtable* U_EXPORT2
uhash_open(UHashFunction *कुञ्चिका_Hash,
           UKeyComparator *कुञ्चिका_Comp,
           UValueComparator *मानम्_Comp,
           UError_संहिता *स्थितिः) {

    प्रत्यागम _uhash_create(कुञ्चिका_Hash, कुञ्चिका_Comp, मानम्_Comp, DEFAULT_अविभाज्यः_सूचकांक, स्थितिः);
}

U_CAPI UHashtable* U_EXPORT2
uhash_open_आकार(UHashFunction *कुञ्चिका_Hash,
               UKeyComparator *कुञ्चिका_Comp,
               UValueComparator *मानम्_Comp,
               int32_t size,
               UError_संहिता *स्थितिः) {

    /* Find the smallest index i for which PRIMES[i] >= size. */
    int32_t i = 0;
    यावत् (i<(अभाज्याः_दीर्घता-1) && अभाज्याः[i]<size) {
        ++i;
    }

    प्रत्यागम _uhash_create(कुञ्चिका_Hash, कुञ्चिका_Comp, मानम्_Comp, i, स्थितिः);
}

U_CAPI UHashtable* U_EXPORT2
uhash_init(UHashtable *fillin_परिणाम,
           UHashFunction *कुञ्चिका_Hash,
           UKeyComparator *कुञ्चिका_Comp,
           UValueComparator *मानम्_Comp,
           UError_संहिता *स्थितिः) {

    प्रत्यागम _uhash_init(fillin_परिणाम, कुञ्चिका_Hash, कुञ्चिका_Comp, मानम्_Comp, DEFAULT_अविभाज्यः_सूचकांक, स्थितिः);
}

U_CAPI UHashtable* U_EXPORT2
uhash_init_आकार(UHashtable *fillin_परिणाम,
               UHashFunction *कुञ्चिका_Hash,
               UKeyComparator *कुञ्चिका_Comp,
               UValueComparator *मानम्_Comp,
               int32_t size,
               UError_संहिता *स्थितिः) {

    // Find the smallest index i for which PRIMES[i] >= size.
    int32_t i = 0;
    यावत् (i<(अभाज्याः_दीर्घता-1) && अभाज्याः[i]<size) {
        ++i;
    }
    प्रत्यागम _uhash_init(fillin_परिणाम, कुञ्चिका_Hash, कुञ्चिका_Comp, मानम्_Comp, i, स्थितिः);
}

U_CAPI शून्य U_EXPORT2
uhash_close(UHashtable *hash) {
    यदि (hash == शून्यसूचकः) {
        प्रत्यागम;
    }
    यदि (hash->elements != शून्यसूचकः) {
        यदि (hash->कुञ्चिका_Deleter != शून्यसूचकः || hash->मानम्_Deleter != शून्यसूचकः) {
            int32_t pos=UHASH_FIRST;
            UHashElement *e;
            यावत् ((e = (UHashElement*) uhash_अग्र_Element(hash, &pos)) != शून्यसूचकः) {
                HASH_DELETE_कुञ्चिका_मानम्(hash, e->कुञ्चिका.pointer, e->value.pointer);
            }
        }
        uprv_free(hash->elements);
        hash->elements = शून्यसूचकः;
    }
    यदि (hash->allocated) {
        uprv_free(hash);
    }
}

U_CAPI UHashFunction *U_EXPORT2
uhash_समुच्चयः_कुञ्चिका_Hasher(UHashtable *hash, UHashFunction *fn) {
    UHashFunction *परिणाम = hash->कुञ्चिका_Hasher;
    hash->कुञ्चिका_Hasher = fn;
    प्रत्यागम परिणाम;
}

U_CAPI UKeyComparator *U_EXPORT2
uhash_समुच्चयः_कुञ्चिका_Comparator(UHashtable *hash, UKeyComparator *fn) {
    UKeyComparator *परिणाम = hash->कुञ्चिका_Comparator;
    hash->कुञ्चिका_Comparator = fn;
    प्रत्यागम परिणाम;
}
U_CAPI UValueComparator *U_EXPORT2
uhash_समुच्चयः_मानम्_Comparator(UHashtable *hash, UValueComparator *fn){
    UValueComparator *परिणाम = hash->मानम्_Comparator;
    hash->मानम्_Comparator = fn;
    प्रत्यागम परिणाम;
}

U_CAPI UObjectDeleter *U_EXPORT2
uhash_समुच्चयः_कुञ्चिका_Deleter(UHashtable *hash, UObjectDeleter *fn) {
    UObjectDeleter *परिणाम = hash->कुञ्चिका_Deleter;
    hash->कुञ्चिका_Deleter = fn;
    प्रत्यागम परिणाम;
}

U_CAPI UObjectDeleter *U_EXPORT2
uhash_समुच्चयः_मानम्_Deleter(UHashtable *hash, UObjectDeleter *fn) {
    UObjectDeleter *परिणाम = hash->मानम्_Deleter;
    hash->मानम्_Deleter = fn;
    प्रत्यागम परिणाम;
}

U_CAPI शून्य U_EXPORT2
uhash_समुच्चयः_Resize_Policy(UHashtable *hash, गणना UHashResizePolicy policy) {
    UError_संहिता स्थितिः = U_ZERO_ERROR;
    uhash_आन्तरिकम्_समुच्चयः_Resize_Policy(hash, policy);
    hash->नीचः_Water_Mark  = (int32_t)(hash->length * hash->नीचः_Water_अनुपातः);
    hash->उच्चः_Water_Mark = (int32_t)(hash->length * hash->उच्चः_Water_अनुपातः);
    _uhash_rehash(hash, &स्थितिः);
}

U_CAPI int32_t U_EXPORT2
uhash_गणनम्(ध्रुव UHashtable *hash) {
    प्रत्यागम hash->count;
}

U_CAPI शून्य* U_EXPORT2
uhash_get(ध्रुव UHashtable *hash,
          ध्रुव शून्य* कुञ्चिका) {
    UHashTok keyholder;
    keyholder.pointer = (शून्य*) कुञ्चिका;
    प्रत्यागम _uhash_find(hash, keyholder, hash->कुञ्चिका_Hasher(keyholder))->value.pointer;
}

U_CAPI शून्य* U_EXPORT2
uhash_iget(ध्रुव UHashtable *hash,
           int32_t कुञ्चिका) {
    UHashTok keyholder;
    keyholder.integer = कुञ्चिका;
    प्रत्यागम _uhash_find(hash, keyholder, hash->कुञ्चिका_Hasher(keyholder))->value.pointer;
}

U_CAPI int32_t U_EXPORT2
uhash_geti(ध्रुव UHashtable *hash,
           ध्रुव शून्य* कुञ्चिका) {
    UHashTok keyholder;
    keyholder.pointer = (शून्य*) कुञ्चिका;
    प्रत्यागम _uhash_find(hash, keyholder, hash->कुञ्चिका_Hasher(keyholder))->value.integer;
}

U_CAPI int32_t U_EXPORT2
uhash_igeti(ध्रुव UHashtable *hash,
           int32_t कुञ्चिका) {
    UHashTok keyholder;
    keyholder.integer = कुञ्चिका;
    प्रत्यागम _uhash_find(hash, keyholder, hash->कुञ्चिका_Hasher(keyholder))->value.integer;
}

U_CAPI int32_t U_EXPORT2
uhash_geti_And_प्राप्तम्(ध्रुव UHashtable *hash,
                   ध्रुव शून्य *कुञ्चिका,
                   UBool *प्राप्तम्) {
    UHashTok keyholder;
    keyholder.pointer = (शून्य *)कुञ्चिका;
    ध्रुव UHashElement *e = _uhash_find(hash, keyholder, hash->कुञ्चिका_Hasher(keyholder));
    *प्राप्तम् = !IS_रिक्तानि_OR_DELETED(e->hashcode);
    प्रत्यागम e->value.integer;
}

U_CAPI int32_t U_EXPORT2
uhash_igeti_And_प्राप्तम्(ध्रुव UHashtable *hash,
                    int32_t कुञ्चिका,
                    UBool *प्राप्तम्) {
    UHashTok keyholder;
    keyholder.integer = कुञ्चिका;
    ध्रुव UHashElement *e = _uhash_find(hash, keyholder, hash->कुञ्चिका_Hasher(keyholder));
    *प्राप्तम् = !IS_रिक्तानि_OR_DELETED(e->hashcode);
    प्रत्यागम e->value.integer;
}

U_CAPI शून्य* U_EXPORT2
uhash_put(UHashtable *hash,
          शून्य* कुञ्चिका,
          शून्य* value,
          UError_संहिता *स्थितिः) {
    UHashTok keyholder, valueholder;
    keyholder.pointer = कुञ्चिका;
    valueholder.pointer = value;
    प्रत्यागम _uhash_put(hash, keyholder, valueholder,
                      HINT_कुञ्चिका_POINTER | HINT_मानम्_POINTER,
                      स्थितिः).pointer;
}

U_CAPI शून्य* U_EXPORT2
uhash_iput(UHashtable *hash,
           int32_t कुञ्चिका,
           शून्य* value,
           UError_संहिता *स्थितिः) {
    UHashTok keyholder, valueholder;
    keyholder.integer = कुञ्चिका;
    valueholder.pointer = value;
    प्रत्यागम _uhash_put(hash, keyholder, valueholder,
                      HINT_मानम्_POINTER,
                      स्थितिः).pointer;
}

U_CAPI int32_t U_EXPORT2
uhash_puti(UHashtable *hash,
           शून्य* कुञ्चिका,
           int32_t value,
           UError_संहिता *स्थितिः) {
    UHashTok keyholder, valueholder;
    keyholder.pointer = कुञ्चिका;
    valueholder.integer = value;
    प्रत्यागम _uhash_put(hash, keyholder, valueholder,
                      HINT_कुञ्चिका_POINTER,
                      स्थितिः).integer;
}


U_CAPI int32_t U_EXPORT2
uhash_iputi(UHashtable *hash,
           int32_t कुञ्चिका,
           int32_t value,
           UError_संहिता *स्थितिः) {
    UHashTok keyholder, valueholder;
    keyholder.integer = कुञ्चिका;
    valueholder.integer = value;
    प्रत्यागम _uhash_put(hash, keyholder, valueholder,
                      HINT_BOTH_INTEGERS,
                      स्थितिः).integer;
}

U_CAPI int32_t U_EXPORT2
uhash_putiAllowZero(UHashtable *hash,
                    शून्य *कुञ्चिका,
                    int32_t value,
                    UError_संहिता *स्थितिः) {
    UHashTok keyholder, valueholder;
    keyholder.pointer = कुञ्चिका;
    valueholder.integer = value;
    प्रत्यागम _uhash_put(hash, keyholder, valueholder,
                      HINT_कुञ्चिका_POINTER | HINT_ALLOW_ZERO,
                      स्थितिः).integer;
}


U_CAPI int32_t U_EXPORT2
uhash_iputiAllowZero(UHashtable *hash,
                     int32_t कुञ्चिका,
                     int32_t value,
                     UError_संहिता *स्थितिः) {
    UHashTok keyholder, valueholder;
    keyholder.integer = कुञ्चिका;
    valueholder.integer = value;
    प्रत्यागम _uhash_put(hash, keyholder, valueholder,
                      HINT_BOTH_INTEGERS | HINT_ALLOW_ZERO,
                      स्थितिः).integer;
}

U_CAPI शून्य* U_EXPORT2
uhash_remove(UHashtable *hash,
             ध्रुव शून्य* कुञ्चिका) {
    UHashTok keyholder;
    keyholder.pointer = (शून्य*) कुञ्चिका;
    प्रत्यागम _uhash_remove(hash, keyholder).pointer;
}

U_CAPI शून्य* U_EXPORT2
uhash_iremove(UHashtable *hash,
              int32_t कुञ्चिका) {
    UHashTok keyholder;
    keyholder.integer = कुञ्चिका;
    प्रत्यागम _uhash_remove(hash, keyholder).pointer;
}

U_CAPI int32_t U_EXPORT2
uhash_removei(UHashtable *hash,
              ध्रुव शून्य* कुञ्चिका) {
    UHashTok keyholder;
    keyholder.pointer = (शून्य*) कुञ्चिका;
    प्रत्यागम _uhash_remove(hash, keyholder).integer;
}

U_CAPI int32_t U_EXPORT2
uhash_iremovei(UHashtable *hash,
               int32_t कुञ्चिका) {
    UHashTok keyholder;
    keyholder.integer = कुञ्चिका;
    प्रत्यागम _uhash_remove(hash, keyholder).integer;
}

U_CAPI शून्य U_EXPORT2
uhash_removeAll(UHashtable *hash) {
    int32_t pos = UHASH_FIRST;
    ध्रुव UHashElement *e;
    U_ASSERT(hash != शून्यसूचकः);
    यदि (hash->count != 0) {
        यावत् ((e = uhash_अग्र_Element(hash, &pos)) != शून्यसूचकः) {
            uhash_removeElement(hash, e);
        }
    }
    U_ASSERT(hash->count == 0);
}

U_CAPI UBool U_EXPORT2
uhash_अन्तर्भवति_कुञ्चिका(ध्रुव UHashtable *hash, ध्रुव शून्य *कुञ्चिका) {
    UHashTok keyholder;
    keyholder.pointer = (शून्य *)कुञ्चिका;
    ध्रुव UHashElement *e = _uhash_find(hash, keyholder, hash->कुञ्चिका_Hasher(keyholder));
    प्रत्यागम !IS_रिक्तानि_OR_DELETED(e->hashcode);
}

/**
 * Returns true if the UHashtable contains an item with this integer key.
 *
 * @param hash The target UHashtable.
 * @param key An integer key stored in a hashtable
 * @return true if the key is found.
 */
U_CAPI UBool U_EXPORT2
uhash_icontains_कुञ्चिका(ध्रुव UHashtable *hash, int32_t कुञ्चिका) {
    UHashTok keyholder;
    keyholder.integer = कुञ्चिका;
    ध्रुव UHashElement *e = _uhash_find(hash, keyholder, hash->कुञ्चिका_Hasher(keyholder));
    प्रत्यागम !IS_रिक्तानि_OR_DELETED(e->hashcode);
}

U_CAPI ध्रुव UHashElement* U_EXPORT2
uhash_find(ध्रुव UHashtable *hash, ध्रुव शून्य* कुञ्चिका) {
    UHashTok keyholder;
    ध्रुव UHashElement *e;
    keyholder.pointer = (शून्य*) कुञ्चिका;
    e = _uhash_find(hash, keyholder, hash->कुञ्चिका_Hasher(keyholder));
    प्रत्यागम IS_रिक्तानि_OR_DELETED(e->hashcode) ? शून्यसूचकः : e;
}

U_CAPI ध्रुव UHashElement* U_EXPORT2
uhash_अग्र_Element(ध्रुव UHashtable *hash, int32_t *pos) {
    /* Walk through the array until we find an element that is not
     * EMPTY and not DELETED.
     */
    int32_t i;
    U_ASSERT(hash != शून्यसूचकः);
    प्रति (i = *pos + 1; i < hash->length; ++i) {
        यदि (!IS_रिक्तानि_OR_DELETED(hash->elements[i].hashcode)) {
            *pos = i;
            प्रत्यागम &(hash->elements[i]);
        }
    }

    /* No more elements */
    प्रत्यागम शून्यसूचकः;
}

U_CAPI शून्य* U_EXPORT2
uhash_removeElement(UHashtable *hash, ध्रुव UHashElement* e) {
    U_ASSERT(hash != शून्यसूचकः);
    U_ASSERT(e != शून्यसूचकः);
    यदि (!IS_रिक्तानि_OR_DELETED(e->hashcode)) {
        UHashElement *nce = (UHashElement *)e;
        प्रत्यागम uhash_आन्तरिकम्_Remove_Element(hash, nce).pointer;
    }
    प्रत्यागम शून्यसूचकः;
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
uhash_hashUChars(ध्रुव UHashTok कुञ्चिका) {
    ध्रुव char16_t *s = (ध्रुव char16_t *)कुञ्चिका.pointer;
    प्रत्यागम s == शून्यसूचकः ? 0 : ustr_hashUCharsN(s, u_strlen(s));
}

U_CAPI int32_t U_EXPORT2
uhash_hashChars(ध्रुव UHashTok कुञ्चिका) {
    ध्रुव वर्ण *s = (ध्रुव वर्ण *)कुञ्चिका.pointer;
    प्रत्यागम s == शून्यसूचकः ? 0 : static_cast<int32_t>(ustr_hashCharsN(s, static_cast<int32_t>(uprv_strlen(s))));
}

U_CAPI int32_t U_EXPORT2
uhash_hashIChars(ध्रुव UHashTok कुञ्चिका) {
    ध्रुव वर्ण *s = (ध्रुव वर्ण *)कुञ्चिका.pointer;
    प्रत्यागम s == शून्यसूचकः ? 0 : ustr_hashICharsN(s, static_cast<int32_t>(uprv_strlen(s)));
}

U_CAPI int32_t U_EXPORT2
uhash_hashIStringView(ध्रुव UHashTok कुञ्चिका) {
    ध्रुव std::string_view* s = static_cast<std::string_view*>(कुञ्चिका.pointer);
    प्रत्यागम s == शून्यसूचकः ? 0 : ustr_hashICharsN(s->दत्त(), static_cast<int32_t>(s->size()));
}

U_CAPI UBool U_EXPORT2
uhash_equals(ध्रुव UHashtable* hash1, ध्रुव UHashtable* hash2){
    int32_t count1, count2, pos, i;

    यदि(hash1==hash2){
        प्रत्यागम सत्य;
    }

    /*
     * Make sure that we are comparing 2 valid hashes of the same type
     * with valid comparison functions.
     * Without valid comparison functions, a binary comparison
     * of the hash values will yield random results on machines
     * with 64-bit pointers and 32-bit integer hashes.
     * A valueComparator is normally optional.
     */
    यदि (hash1==शून्यसूचकः || hash2==शून्यसूचकः ||
        hash1->कुञ्चिका_Comparator != hash2->कुञ्चिका_Comparator ||
        hash1->मानम्_Comparator != hash2->मानम्_Comparator ||
        hash1->मानम्_Comparator == शून्यसूचकः)
    {
        /*
        Normally we would return an error here about incompatible hash tables,
        but we return false instead.
        */
        प्रत्यागम असत्य;
    }

    count1 = uhash_गणनम्(hash1);
    count2 = uhash_गणनम्(hash2);
    यदि(count1!=count2){
        प्रत्यागम असत्य;
    }

    pos=UHASH_FIRST;
    प्रति(i=0; i<count1; i++){
        ध्रुव UHashElement* elem1 = uhash_अग्र_Element(hash1, &pos);
        ध्रुव UHashTok key1 = elem1->कुञ्चिका;
        ध्रुव UHashTok val1 = elem1->value;
        /* here the keys are not compared, instead the key form hash1 is used to fetch
         * value from hash2. If the hashes are equal then then both hashes should
         * contain equal values for the same key!
         */
        ध्रुव UHashElement* elem2 = _uhash_find(hash2, key1, hash2->कुञ्चिका_Hasher(key1));
        ध्रुव UHashTok val2 = elem2->value;
        यदि(hash1->मानम्_Comparator(val1, val2)==असत्य){
            प्रत्यागम असत्य;
        }
    }
    प्रत्यागम सत्य;
}

/********************************************************************
 * PUBLIC Comparator Functions
 ********************************************************************/

U_CAPI UBool U_EXPORT2
uhash_compareUChars(ध्रुव UHashTok key1, ध्रुव UHashTok key2) {
    ध्रुव char16_t *p1 = (ध्रुव char16_t*) key1.pointer;
    ध्रुव char16_t *p2 = (ध्रुव char16_t*) key2.pointer;
    यदि (p1 == p2) {
        प्रत्यागम सत्य;
    }
    यदि (p1 == शून्यसूचकः || p2 == शून्यसूचकः) {
        प्रत्यागम असत्य;
    }
    यावत् (*p1 != 0 && *p1 == *p2) {
        ++p1;
        ++p2;
    }
    प्रत्यागम *p1 == *p2;
}

U_CAPI UBool U_EXPORT2
uhash_compareChars(ध्रुव UHashTok key1, ध्रुव UHashTok key2) {
    ध्रुव वर्ण *p1 = (ध्रुव वर्ण*) key1.pointer;
    ध्रुव वर्ण *p2 = (ध्रुव वर्ण*) key2.pointer;
    यदि (p1 == p2) {
        प्रत्यागम सत्य;
    }
    यदि (p1 == शून्यसूचकः || p2 == शून्यसूचकः) {
        प्रत्यागम असत्य;
    }
    यावत् (*p1 != 0 && *p1 == *p2) {
        ++p1;
        ++p2;
    }
    प्रत्यागम *p1 == *p2;
}

U_CAPI UBool U_EXPORT2
uhash_compareIChars(ध्रुव UHashTok key1, ध्रुव UHashTok key2) {
    ध्रुव वर्ण *p1 = (ध्रुव वर्ण*) key1.pointer;
    ध्रुव वर्ण *p2 = (ध्रुव वर्ण*) key2.pointer;
    यदि (p1 == p2) {
        प्रत्यागम सत्य;
    }
    यदि (p1 == शून्यसूचकः || p2 == शून्यसूचकः) {
        प्रत्यागम असत्य;
    }
    यावत् (*p1 != 0 && uprv_tolower(*p1) == uprv_tolower(*p2)) {
        ++p1;
        ++p2;
    }
    प्रत्यागम *p1 == *p2;
}

U_CAPI UBool U_EXPORT2
uhash_compareIStringView(ध्रुव UHashTok key1, ध्रुव UHashTok key2) {
    ध्रुव std::string_view* p1 = static_cast<std::string_view*>(key1.pointer);
    ध्रुव std::string_view* p2 = static_cast<std::string_view*>(key2.pointer);
    यदि (p1 == p2) {
        प्रत्यागम सत्य;
    }
    यदि (p1 == शून्यसूचकः || p2 == शून्यसूचकः) {
        प्रत्यागम असत्य;
    }
    ध्रुव std::string_view& v1 = *p1;
    ध्रुव std::string_view& v2 = *p2;
    यदि (v1.size() != v2.size()) {
        प्रत्यागम असत्य;
    }
    प्रति (size_t i = 0; i < v1.size(); ++i) {
        यदि (uprv_tolower(v1[i]) != uprv_tolower(v2[i])) {
            प्रत्यागम असत्य;
        }
    }
    प्रत्यागम सत्य;
}

/********************************************************************
 * PUBLIC int32_t Support Functions
 ********************************************************************/

U_CAPI int32_t U_EXPORT2
uhash_hashLong(ध्रुव UHashTok कुञ्चिका) {
    प्रत्यागम कुञ्चिका.integer;
}

U_CAPI UBool U_EXPORT2
uhash_compareLong(ध्रुव UHashTok key1, ध्रुव UHashTok key2) {
    प्रत्यागम key1.integer == key2.integer;
}
