// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
******************************************************************************
* Copyright (C) 1999-2015, International Business Machines Corporation and
* others. All Rights Reserved.
******************************************************************************
*/

#ਅਨ੍ਤਰ੍ਭਾਵ "uvectr64.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "cmemory.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "putilimp.h"

U_NAMESPACE_BEGIN

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ DEFAULT_ਸਮਰੱਥਾ 8

/*
 * Constants for hinting whether a key is an integer
 * or a pointer.  If a hint bit is zero, then the associated
 * token is assumed to be an integer. This is needed for iSeries
 */
 
UOBJECT_DEFINE_RTTI_IMPLEMENTATION(UVector64)

UVector64::UVector64(UError_ਕੋਡ &ਸਥਿਤੀ) :
    count(0),
    capacity(0),
    max_ਸਮਰੱਥਾ(0),
    elements(ਖਾਲੀ_ਸੂਚਕ)
{
    _init(DEFAULT_ਸਮਰੱਥਾ, ਸਥਿਤੀ);
}

UVector64::UVector64(int32_t initial_ਸਮਰੱਥਾ, UError_ਕੋਡ &ਸਥਿਤੀ) :
    count(0),
    capacity(0),
    max_ਸਮਰੱਥਾ(0),
    elements(ਖਾਲੀ_ਸੂਚਕ)
{
    _init(initial_ਸਮਰੱਥਾ, ਸਥਿਤੀ);
}



ਖਾਲੀ UVector64::_init(int32_t initial_ਸਮਰੱਥਾ, UError_ਕੋਡ &ਸਥਿਤੀ) {
    // Fix bogus initialCapacity values; avoid malloc(0)
    ਜੇ (initial_ਸਮਰੱਥਾ < 1) {
        initial_ਸਮਰੱਥਾ = DEFAULT_ਸਮਰੱਥਾ;
    }
    ਜੇ (max_ਸਮਰੱਥਾ>0 && max_ਸਮਰੱਥਾ<initial_ਸਮਰੱਥਾ) {
        initial_ਸਮਰੱਥਾ = max_ਸਮਰੱਥਾ;
    }
    ਜੇ (initial_ਸਮਰੱਥਾ > static_cast<int32_t>(INT32_MAX / ਆਕਾਰ(int64_t))) {
        initial_ਸਮਰੱਥਾ = uprv_min(DEFAULT_ਸਮਰੱਥਾ, max_ਸਮਰੱਥਾ);
    }
    elements = static_cast<int64_t*>(uprv_malloc(ਆਕਾਰ(int64_t) * initial_ਸਮਰੱਥਾ));
    ਜੇ (elements == ਖਾਲੀ_ਸੂਚਕ) {
        ਸਥਿਤੀ = U_ਸਮ੍ਰਿਤੀ_ਵੰਡ_ERROR;
    } ਨਹੀਂਤਾਂ {
        capacity = initial_ਸਮਰੱਥਾ;
    }
}

UVector64::~UVector64() {
    uprv_free(elements);
    elements = ਖਾਲੀ_ਸੂਚਕ;
}

/**
 * Assign this object to another (make this a copy of 'other').
 */
ਖਾਲੀ UVector64::assign(ਅਚਲ UVector64& other, UError_ਕੋਡ &ec) {
    ਜੇ (ensure_ਸਮਰੱਥਾ(other.count, ec)) {
        ਸੈੱਟ_ਮਾਪ(other.count);
        ਲਈ (int32_t i=0; i<other.count; ++i) {
            elements[i] = other.elements[i];
        }
    }
}


ਤਾਰਕਿਕ UVector64::ਸੰਚਾਲਕ==(ਅਚਲ UVector64& other) {
    int32_t i;
    ਜੇ (count != other.count) ਵਾਪਸ ਝੂਠ;
    ਲਈ (i=0; i<count; ++i) {
        ਜੇ (elements[i] != other.elements[i]) {
            ਵਾਪਸ ਝੂਠ;
        }
    }
    ਵਾਪਸ ਸੱਚ;
}


ਖਾਲੀ UVector64::ਸੈੱਟ_Element_At(int64_t elem, int32_t ਸੂਚਕਾਂਕ) {
    ਜੇ (0 <= ਸੂਚਕਾਂਕ && ਸੂਚਕਾਂਕ < count) {
        elements[ਸੂਚਕਾਂਕ] = elem;
    }
    /* else index out of range */
}

ਖਾਲੀ UVector64::ਪਾਓ_Element_At(int64_t elem, int32_t ਸੂਚਕਾਂਕ, UError_ਕੋਡ &ਸਥਿਤੀ) {
    // must have 0 <= index <= count
    ਜੇ (0 <= ਸੂਚਕਾਂਕ && ਸੂਚਕਾਂਕ <= count && ensure_ਸਮਰੱਥਾ(count + 1, ਸਥਿਤੀ)) {
        ਲਈ (int32_t i=count; i>ਸੂਚਕਾਂਕ; --i) {
            elements[i] = elements[i-1];
        }
        elements[ਸੂਚਕਾਂਕ] = elem;
        ++count;
    }
    /* else index out of range */
}

ਖਾਲੀ UVector64::removeAllElements() {
    count = 0;
}

UBool UVector64::expand_ਸਮਰੱਥਾ(int32_t minimum_ਸਮਰੱਥਾ, UError_ਕੋਡ &ਸਥਿਤੀ) {
    ਜੇ (U_FAILURE(ਸਥਿਤੀ)) {
        ਵਾਪਸ ਝੂਠ;
    }
    ਜੇ (minimum_ਸਮਰੱਥਾ < 0) {
        ਸਥਿਤੀ = U_ILLEGAL_ਦਲੀਲ_ERROR;
        ਵਾਪਸ ਝੂਠ;
    }
    ਜੇ (capacity >= minimum_ਸਮਰੱਥਾ) {
        ਵਾਪਸ ਸੱਚ;
    }
    ਜੇ (max_ਸਮਰੱਥਾ>0 && minimum_ਸਮਰੱਥਾ>max_ਸਮਰੱਥਾ) {
        ਸਥਿਤੀ = U_ਬਫਰ_ਉਛਾਲ_ERROR;
        ਵਾਪਸ ਝੂਠ;
    }
    ਜੇ (capacity > (INT32_MAX - 1) / 2) {  // integer overflow check
        ਸਥਿਤੀ = U_ILLEGAL_ਦਲੀਲ_ERROR;
        ਵਾਪਸ ਝੂਠ;
    }
    int32_t newCap = capacity * 2;
    ਜੇ (newCap < minimum_ਸਮਰੱਥਾ) {
        newCap = minimum_ਸਮਰੱਥਾ;
    }
    ਜੇ (max_ਸਮਰੱਥਾ > 0 && newCap > max_ਸਮਰੱਥਾ) {
        newCap = max_ਸਮਰੱਥਾ;
    }
    ਜੇ (newCap > static_cast<int32_t>(INT32_MAX / ਆਕਾਰ(int64_t))) { // integer overflow check
        // We keep the original memory contents on bad minimumCapacity/maxCapacity.
        ਸਥਿਤੀ = U_ILLEGAL_ਦਲੀਲ_ERROR;
        ਵਾਪਸ ਝੂਠ;
    }
    int64_t* newElems = static_cast<int64_t*>(uprv_realloc(elements, ਆਕਾਰ(int64_t) * newCap));
    ਜੇ (newElems == ਖਾਲੀ_ਸੂਚਕ) {
        // We keep the original contents on the memory failure on realloc.
        ਸਥਿਤੀ = U_ਸਮ੍ਰਿਤੀ_ਵੰਡ_ERROR;
        ਵਾਪਸ ਝੂਠ;
    }
    elements = newElems;
    capacity = newCap;
    ਵਾਪਸ ਸੱਚ;
}

ਖਾਲੀ UVector64::ਸੈੱਟ_Max_ਸਮਰੱਥਾ(int32_t ਹੱਦਬੰਦੀ) {
    U_ASSERT(ਹੱਦਬੰਦੀ >= 0);
    ਜੇ (ਹੱਦਬੰਦੀ < 0) {
        ਹੱਦਬੰਦੀ = 0;
    }
    ਜੇ (ਹੱਦਬੰਦੀ > static_cast<int32_t>(INT32_MAX / ਆਕਾਰ(int64_t))) { // integer overflow check for realloc
        //  Something is very wrong, don't realloc, leave capacity and maxCapacity unchanged
        ਵਾਪਸ;
    }
    max_ਸਮਰੱਥਾ = ਹੱਦਬੰਦੀ;
    ਜੇ (capacity <= max_ਸਮਰੱਥਾ || max_ਸਮਰੱਥਾ == 0) {
        // Current capacity is within the new limit.
        ਵਾਪਸ;
    }
    
    // New maximum capacity is smaller than the current size.
    // Realloc the storage to the new, smaller size.
    int64_t* newElems = static_cast<int64_t*>(uprv_realloc(elements, ਆਕਾਰ(int64_t) * max_ਸਮਰੱਥਾ));
    ਜੇ (newElems == ਖਾਲੀ_ਸੂਚਕ) {
        // Realloc to smaller failed.
        //   Just keep what we had.  No need to call it a failure.
        ਵਾਪਸ;
    }
    elements = newElems;
    capacity = max_ਸਮਰੱਥਾ;
    ਜੇ (count > capacity) {
        count = capacity;
    }
}

/**
 * Change the size of this vector as follows: If newSize is smaller,
 * then truncate the array, possibly deleting held elements for i >=
 * newSize.  If newSize is larger, grow the array, filling in new
 * slots with nullptr.
 */
ਖਾਲੀ UVector64::ਸੈੱਟ_ਮਾਪ(int32_t new_ਮਾਪ) {
    int32_t i;
    ਜੇ (new_ਮਾਪ < 0) {
        ਵਾਪਸ;
    }
    ਜੇ (new_ਮਾਪ > count) {
        UError_ਕੋਡ ec = U_ZERO_ERROR;
        ਜੇ (!ensure_ਸਮਰੱਥਾ(new_ਮਾਪ, ec)) {
            ਵਾਪਸ;
        }
        ਲਈ (i=count; i<new_ਮਾਪ; ++i) {
            elements[i] = 0;
        }
    } 
    count = new_ਮਾਪ;
}

U_NAMESPACE_ਸਿਰਾ

