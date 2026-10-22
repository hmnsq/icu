// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
******************************************************************************
* Copyright (C) 1999-2015, International Business Machines Corporation and
* others. All Rights Reserved.
******************************************************************************
*   Date        Name        Description
*   10/22/99    alan        Creation.
**********************************************************************
*/

#ਅਨ੍ਤਰ੍ਭਾਵ "uvectr32.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "cmemory.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "putilimp.h"

U_NAMESPACE_BEGIN

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ DEFAULT_ਸਮਰੱਥਾ 8

/*
 * Constants for hinting whether a key is an integer
 * or a pointer.  If a hint bit is zero, then the associated
 * token is assumed to be an integer. This is needed for iSeries
 */
 
UOBJECT_DEFINE_RTTI_IMPLEMENTATION(UVector32)

UVector32::UVector32(UError_ਕੋਡ &ਸਥਿਤੀ) :
    count(0),
    capacity(0),
    max_ਸਮਰੱਥਾ(0),
    elements(ਖਾਲੀ_ਸੂਚਕ)
{
    _init(DEFAULT_ਸਮਰੱਥਾ, ਸਥਿਤੀ);
}

UVector32::UVector32(int32_t initial_ਸਮਰੱਥਾ, UError_ਕੋਡ &ਸਥਿਤੀ) :
    count(0),
    capacity(0),
    max_ਸਮਰੱਥਾ(0),
    elements(ਖਾਲੀ_ਸੂਚਕ)
{
    _init(initial_ਸਮਰੱਥਾ, ਸਥਿਤੀ);
}



ਖਾਲੀ UVector32::_init(int32_t initial_ਸਮਰੱਥਾ, UError_ਕੋਡ &ਸਥਿਤੀ) {
    // Fix bogus initialCapacity values; avoid malloc(0)
    ਜੇ (initial_ਸਮਰੱਥਾ < 1) {
        initial_ਸਮਰੱਥਾ = DEFAULT_ਸਮਰੱਥਾ;
    }
    ਜੇ (max_ਸਮਰੱਥਾ>0 && max_ਸਮਰੱਥਾ<initial_ਸਮਰੱਥਾ) {
        initial_ਸਮਰੱਥਾ = max_ਸਮਰੱਥਾ;
    }
    ਜੇ (initial_ਸਮਰੱਥਾ > static_cast<int32_t>(INT32_MAX / ਆਕਾਰ(int32_t))) {
        initial_ਸਮਰੱਥਾ = uprv_min(DEFAULT_ਸਮਰੱਥਾ, max_ਸਮਰੱਥਾ);
    }
    elements = static_cast<int32_t*>(uprv_malloc(ਆਕਾਰ(int32_t) * initial_ਸਮਰੱਥਾ));
    ਜੇ (elements == ਖਾਲੀ_ਸੂਚਕ) {
        ਸਥਿਤੀ = U_ਸਮ੍ਰਿਤੀ_ਵੰਡ_ERROR;
    } ਨਹੀਂਤਾਂ {
        capacity = initial_ਸਮਰੱਥਾ;
    }
}

UVector32::~UVector32() {
    uprv_free(elements);
    elements = ਖਾਲੀ_ਸੂਚਕ;
}

/**
 * Assign this object to another (make this a copy of 'other').
 */
ਖਾਲੀ UVector32::assign(ਅਚਲ UVector32& other, UError_ਕੋਡ &ec) {
    ਜੇ (ensure_ਸਮਰੱਥਾ(other.count, ec)) {
        ਸੈੱਟ_ਮਾਪ(other.count);
        ਲਈ (int32_t i=0; i<other.count; ++i) {
            elements[i] = other.elements[i];
        }
    }
}


ਤਾਰਕਿਕ UVector32::ਸੰਚਾਲਕ==(ਅਚਲ UVector32& other) ਅਚਲ {
    int32_t i;
    ਜੇ (count != other.count) ਵਾਪਸ ਝੂਠ;
    ਲਈ (i=0; i<count; ++i) {
        ਜੇ (elements[i] != other.elements[i]) {
            ਵਾਪਸ ਝੂਠ;
        }
    }
    ਵਾਪਸ ਸੱਚ;
}


ਖਾਲੀ UVector32::ਸੈੱਟ_Element_At(int32_t elem, int32_t ਸੂਚਕਾਂਕ) {
    ਜੇ (0 <= ਸੂਚਕਾਂਕ && ਸੂਚਕਾਂਕ < count) {
        elements[ਸੂਚਕਾਂਕ] = elem;
    }
    /* else index out of range */
}

ਖਾਲੀ UVector32::ਪਾਓ_Element_At(int32_t elem, int32_t ਸੂਚਕਾਂਕ, UError_ਕੋਡ &ਸਥਿਤੀ) {
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

UBool UVector32::ਸ਼ਾਮਲ_ਹੈ_All(ਅਚਲ UVector32& other) ਅਚਲ {
    ਲਈ (int32_t i=0; i<other.size(); ++i) {
        ਜੇ (ਸੂਚਕਾਂਕ_ਦਾ(other.elements[i]) < 0) {
            ਵਾਪਸ ਝੂਠ;
        }
    }
    ਵਾਪਸ ਸੱਚ;
}

UBool UVector32::ਸ਼ਾਮਲ_ਹੈ_None(ਅਚਲ UVector32& other) ਅਚਲ {
    ਲਈ (int32_t i=0; i<other.size(); ++i) {
        ਜੇ (ਸੂਚਕਾਂਕ_ਦਾ(other.elements[i]) >= 0) {
            ਵਾਪਸ ਝੂਠ;
        }
    }
    ਵਾਪਸ ਸੱਚ;
}

UBool UVector32::removeAll(ਅਚਲ UVector32& other) {
    UBool ਬਦਲਿਆ = ਝੂਠ;
    ਲਈ (int32_t i=0; i<other.size(); ++i) {
        int32_t j = ਸੂਚਕਾਂਕ_ਦਾ(other.elements[i]);
        ਜੇ (j >= 0) {
            removeElementAt(j);
            ਬਦਲਿਆ = ਸੱਚ;
        }
    }
    ਵਾਪਸ ਬਦਲਿਆ;
}

UBool UVector32::retainAll(ਅਚਲ UVector32& other) {
    UBool ਬਦਲਿਆ = ਝੂਠ;
    ਲਈ (int32_t j=size()-1; j>=0; --j) {
        int32_t i = other.ਸੂਚਕਾਂਕ_ਦਾ(elements[j]);
        ਜੇ (i < 0) {
            removeElementAt(j);
            ਬਦਲਿਆ = ਸੱਚ;
        }
    }
    ਵਾਪਸ ਬਦਲਿਆ;
}

ਖਾਲੀ UVector32::removeElementAt(int32_t ਸੂਚਕਾਂਕ) {
    ਜੇ (ਸੂਚਕਾਂਕ >= 0) {
        ਲਈ (int32_t i=ਸੂਚਕਾਂਕ; i<count-1; ++i) {
            elements[i] = elements[i+1];
        }
        --count;
    }
}

ਖਾਲੀ UVector32::removeAllElements() {
    count = 0;
}

UBool   UVector32::equals(ਅਚਲ UVector32 &other) ਅਚਲ {
    ਪੂਰਨ      i;

    ਜੇ (ਇਹ->count != other.count) {
        ਵਾਪਸ ਝੂਠ;
    }
    ਲਈ (i=0; i<count; i++) {
        ਜੇ (elements[i] != other.elements[i]) {
            ਵਾਪਸ ਝੂਠ;
        }
    }
    ਵਾਪਸ ਸੱਚ;
}




int32_t UVector32::ਸੂਚਕਾਂਕ_ਦਾ(int32_t ਕੁੰਜੀ, int32_t ਸ਼ੁਰੂ_ਸੂਚਕਾਂਕ) ਅਚਲ {
    int32_t i;
    ਲਈ (i=ਸ਼ੁਰੂ_ਸੂਚਕਾਂਕ; i<count; ++i) {
        ਜੇ (ਕੁੰਜੀ == elements[i]) {
            ਵਾਪਸ i;
        }
    }
    ਵਾਪਸ -1;
}


UBool UVector32::expand_ਸਮਰੱਥਾ(int32_t minimum_ਸਮਰੱਥਾ, UError_ਕੋਡ &ਸਥਿਤੀ) {
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
    ਜੇ (newCap > static_cast<int32_t>(INT32_MAX / ਆਕਾਰ(int32_t))) { // integer overflow check
        // We keep the original memory contents on bad minimumCapacity/maxCapacity.
        ਸਥਿਤੀ = U_ILLEGAL_ਦਲੀਲ_ERROR;
        ਵਾਪਸ ਝੂਠ;
    }
    int32_t* newElems = static_cast<int32_t*>(uprv_realloc(elements, ਆਕਾਰ(int32_t) * newCap));
    ਜੇ (newElems == ਖਾਲੀ_ਸੂਚਕ) {
        // We keep the original contents on the memory failure on realloc.
        ਸਥਿਤੀ = U_ਸਮ੍ਰਿਤੀ_ਵੰਡ_ERROR;
        ਵਾਪਸ ਝੂਠ;
    }
    elements = newElems;
    capacity = newCap;
    ਵਾਪਸ ਸੱਚ;
}

ਖਾਲੀ UVector32::ਸੈੱਟ_Max_ਸਮਰੱਥਾ(int32_t ਹੱਦਬੰਦੀ) {
    U_ASSERT(ਹੱਦਬੰਦੀ >= 0);
    ਜੇ (ਹੱਦਬੰਦੀ < 0) {
        ਹੱਦਬੰਦੀ = 0;
    }
    ਜੇ (ਹੱਦਬੰਦੀ > static_cast<int32_t>(INT32_MAX / ਆਕਾਰ(int32_t))) { // integer overflow check for realloc
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
    int32_t* newElems = static_cast<int32_t*>(uprv_realloc(elements, ਆਕਾਰ(int32_t) * max_ਸਮਰੱਥਾ));
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
ਖਾਲੀ UVector32::ਸੈੱਟ_ਮਾਪ(int32_t new_ਮਾਪ) {
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




/**
 * Insert the given integer into this vector at its sorted position
 * as defined by 'compare'.  The current elements are assumed to
 * be sorted already.
 */
ਖਾਲੀ UVector32::sorted_ਪਾਓ(int32_t tok, UError_ਕੋਡ& ec) {
    // Perform a binary search for the location to insert tok at.  Tok
    // will be inserted between two elements a and b such that a <=
    // tok && tok < b, where there is a 'virtual' elements[-1] always
    // less than tok and a 'virtual' elements[count] always greater
    // than tok.
    int32_t min = 0, max = count;
    ਜਦੋਂ (min != max) {
        int32_t probe = (min + max) / 2;
        //int8_t c = (*compare)(elements[probe], tok);
        //if (c > 0) {
        ਜੇ (elements[probe] > tok) {
            max = probe;
        } ਨਹੀਂਤਾਂ {
            // assert(c <= 0);
            min = probe + 1;
        }
    }
    ਜੇ (ensure_ਸਮਰੱਥਾ(count + 1, ec)) {
        ਲਈ (int32_t i=count; i>min; --i) {
            elements[i] = elements[i-1];
        }
        elements[min] = tok;
        ++count;
    }
}





U_NAMESPACE_ਸਿਰਾ

