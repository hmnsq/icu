// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
******************************************************************************
* Copyright (C) 1999-2015, International Business Machines Corporation and
* others. All Rights Reserved.
******************************************************************************
*/

#ಅನ್ತರ್ಭಾವ "uvectr64.h"
#ಅನ್ತರ್ಭಾವ "cmemory.h"
#ಅನ್ತರ್ಭಾವ "putilimp.h"

U_NAMESPACE_BEGIN

#ವ್ಯಾಖ್ಯಾನಿಸು DEFAULT_ಸಾಮರ್ಥ್ಯ 8

/*
 * Constants for hinting whether a key is an integer
 * or a pointer.  If a hint bit is zero, then the associated
 * token is assumed to be an integer. This is needed for iSeries
 */
 
UOBJECT_DEFINE_RTTI_IMPLEMENTATION(UVector64)

UVector64::UVector64(UError_ಸಂಕೇತ &ಸ್ಥಿತಿ_2) :
    count(0),
    capacity(0),
    max_ಸಾಮರ್ಥ್ಯ(0),
    elements(ಶೂನ್ಯಸೂಚಕ)
{
    _init(DEFAULT_ಸಾಮರ್ಥ್ಯ, ಸ್ಥಿತಿ_2);
}

UVector64::UVector64(int32_t initial_ಸಾಮರ್ಥ್ಯ, UError_ಸಂಕೇತ &ಸ್ಥಿತಿ_2) :
    count(0),
    capacity(0),
    max_ಸಾಮರ್ಥ್ಯ(0),
    elements(ಶೂನ್ಯಸೂಚಕ)
{
    _init(initial_ಸಾಮರ್ಥ್ಯ, ಸ್ಥಿತಿ_2);
}



ಶೂನ್ಯ UVector64::_init(int32_t initial_ಸಾಮರ್ಥ್ಯ, UError_ಸಂಕೇತ &ಸ್ಥಿತಿ_2) {
    // Fix bogus initialCapacity values; avoid malloc(0)
    ಆದರೆ (initial_ಸಾಮರ್ಥ್ಯ < 1) {
        initial_ಸಾಮರ್ಥ್ಯ = DEFAULT_ಸಾಮರ್ಥ್ಯ;
    }
    ಆದರೆ (max_ಸಾಮರ್ಥ್ಯ>0 && max_ಸಾಮರ್ಥ್ಯ<initial_ಸಾಮರ್ಥ್ಯ) {
        initial_ಸಾಮರ್ಥ್ಯ = max_ಸಾಮರ್ಥ್ಯ;
    }
    ಆದರೆ (initial_ಸಾಮರ್ಥ್ಯ > static_cast<int32_t>(INT32_MAX / ಗಾತ್ರ(int64_t))) {
        initial_ಸಾಮರ್ಥ್ಯ = uprv_min(DEFAULT_ಸಾಮರ್ಥ್ಯ, max_ಸಾಮರ್ಥ್ಯ);
    }
    elements = static_cast<int64_t*>(uprv_malloc(ಗಾತ್ರ(int64_t) * initial_ಸಾಮರ್ಥ್ಯ));
    ಆದರೆ (elements == ಶೂನ್ಯಸೂಚಕ) {
        ಸ್ಥಿತಿ_2 = U_ಸ್ಮರಣೆ_ಹಂಚಿಕೆ_ERROR;
    } ಇಲ್ಲದಿದ್ದರೆ {
        capacity = initial_ಸಾಮರ್ಥ್ಯ;
    }
}

UVector64::~UVector64() {
    uprv_free(elements);
    elements = ಶೂನ್ಯಸೂಚಕ;
}

/**
 * Assign this object to another (make this a copy of 'other').
 */
ಶೂನ್ಯ UVector64::assign(ಸ್ಥಿರಾಂಕ UVector64& other, UError_ಸಂಕೇತ &ec) {
    ಆದರೆ (ensure_ಸಾಮರ್ಥ್ಯ(other.count, ec)) {
        ಗಣ_ಗಾತ್ರ(other.count);
        ಗಾಗಿ (int32_t i=0; i<other.count; ++i) {
            elements[i] = other.elements[i];
        }
    }
}


ತಾರ್ಕಿಕ UVector64::ಕ್ರಿಯಾಕಾರಕ==(ಸ್ಥಿರಾಂಕ UVector64& other) {
    int32_t i;
    ಆದರೆ (count != other.count) ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
    ಗಾಗಿ (i=0; i<count; ++i) {
        ಆದರೆ (elements[i] != other.elements[i]) {
            ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
        }
    }
    ಹಿಂತಿರುಗಿ ನಿಜ;
}


ಶೂನ್ಯ UVector64::ಗಣ_Element_At(int64_t elem, int32_t ಸೂಚ್ಯಂಕ) {
    ಆದರೆ (0 <= ಸೂಚ್ಯಂಕ && ಸೂಚ್ಯಂಕ < count) {
        elements[ಸೂಚ್ಯಂಕ] = elem;
    }
    /* else index out of range */
}

ಶೂನ್ಯ UVector64::ಸೇರಿಸು_Element_At(int64_t elem, int32_t ಸೂಚ್ಯಂಕ, UError_ಸಂಕೇತ &ಸ್ಥಿತಿ_2) {
    // must have 0 <= index <= count
    ಆದರೆ (0 <= ಸೂಚ್ಯಂಕ && ಸೂಚ್ಯಂಕ <= count && ensure_ಸಾಮರ್ಥ್ಯ(count + 1, ಸ್ಥಿತಿ_2)) {
        ಗಾಗಿ (int32_t i=count; i>ಸೂಚ್ಯಂಕ; --i) {
            elements[i] = elements[i-1];
        }
        elements[ಸೂಚ್ಯಂಕ] = elem;
        ++count;
    }
    /* else index out of range */
}

ಶೂನ್ಯ UVector64::removeAllElements() {
    count = 0;
}

UBool UVector64::expand_ಸಾಮರ್ಥ್ಯ(int32_t minimum_ಸಾಮರ್ಥ್ಯ, UError_ಸಂಕೇತ &ಸ್ಥಿತಿ_2) {
    ಆದರೆ (U_FAILURE(ಸ್ಥಿತಿ_2)) {
        ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
    }
    ಆದರೆ (minimum_ಸಾಮರ್ಥ್ಯ < 0) {
        ಸ್ಥಿತಿ_2 = U_ILLEGAL_ವಾದ_ERROR;
        ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
    }
    ಆದರೆ (capacity >= minimum_ಸಾಮರ್ಥ್ಯ) {
        ಹಿಂತಿರುಗಿ ನಿಜ;
    }
    ಆದರೆ (max_ಸಾಮರ್ಥ್ಯ>0 && minimum_ಸಾಮರ್ಥ್ಯ>max_ಸಾಮರ್ಥ್ಯ) {
        ಸ್ಥಿತಿ_2 = U_ಬಫರ್_ಅತಿಪ್ರವಾಹ_ERROR;
        ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
    }
    ಆದರೆ (capacity > (INT32_MAX - 1) / 2) {  // integer overflow check
        ಸ್ಥಿತಿ_2 = U_ILLEGAL_ವಾದ_ERROR;
        ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
    }
    int32_t newCap = capacity * 2;
    ಆದರೆ (newCap < minimum_ಸಾಮರ್ಥ್ಯ) {
        newCap = minimum_ಸಾಮರ್ಥ್ಯ;
    }
    ಆದರೆ (max_ಸಾಮರ್ಥ್ಯ > 0 && newCap > max_ಸಾಮರ್ಥ್ಯ) {
        newCap = max_ಸಾಮರ್ಥ್ಯ;
    }
    ಆದರೆ (newCap > static_cast<int32_t>(INT32_MAX / ಗಾತ್ರ(int64_t))) { // integer overflow check
        // We keep the original memory contents on bad minimumCapacity/maxCapacity.
        ಸ್ಥಿತಿ_2 = U_ILLEGAL_ವಾದ_ERROR;
        ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
    }
    int64_t* newElems = static_cast<int64_t*>(uprv_realloc(elements, ಗಾತ್ರ(int64_t) * newCap));
    ಆದರೆ (newElems == ಶೂನ್ಯಸೂಚಕ) {
        // We keep the original contents on the memory failure on realloc.
        ಸ್ಥಿತಿ_2 = U_ಸ್ಮರಣೆ_ಹಂಚಿಕೆ_ERROR;
        ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
    }
    elements = newElems;
    capacity = newCap;
    ಹಿಂತಿರುಗಿ ನಿಜ;
}

ಶೂನ್ಯ UVector64::ಗಣ_Max_ಸಾಮರ್ಥ್ಯ(int32_t ಎಲ್ಲೆ) {
    U_ASSERT(ಎಲ್ಲೆ >= 0);
    ಆದರೆ (ಎಲ್ಲೆ < 0) {
        ಎಲ್ಲೆ = 0;
    }
    ಆದರೆ (ಎಲ್ಲೆ > static_cast<int32_t>(INT32_MAX / ಗಾತ್ರ(int64_t))) { // integer overflow check for realloc
        //  Something is very wrong, don't realloc, leave capacity and maxCapacity unchanged
        ಹಿಂತಿರುಗಿ;
    }
    max_ಸಾಮರ್ಥ್ಯ = ಎಲ್ಲೆ;
    ಆದರೆ (capacity <= max_ಸಾಮರ್ಥ್ಯ || max_ಸಾಮರ್ಥ್ಯ == 0) {
        // Current capacity is within the new limit.
        ಹಿಂತಿರುಗಿ;
    }
    
    // New maximum capacity is smaller than the current size.
    // Realloc the storage to the new, smaller size.
    int64_t* newElems = static_cast<int64_t*>(uprv_realloc(elements, ಗಾತ್ರ(int64_t) * max_ಸಾಮರ್ಥ್ಯ));
    ಆದರೆ (newElems == ಶೂನ್ಯಸೂಚಕ) {
        // Realloc to smaller failed.
        //   Just keep what we had.  No need to call it a failure.
        ಹಿಂತಿರುಗಿ;
    }
    elements = newElems;
    capacity = max_ಸಾಮರ್ಥ್ಯ;
    ಆದರೆ (count > capacity) {
        count = capacity;
    }
}

/**
 * Change the size of this vector as follows: If newSize is smaller,
 * then truncate the array, possibly deleting held elements for i >=
 * newSize.  If newSize is larger, grow the array, filling in new
 * slots with nullptr.
 */
ಶೂನ್ಯ UVector64::ಗಣ_ಗಾತ್ರ(int32_t new_ಗಾತ್ರ) {
    int32_t i;
    ಆದರೆ (new_ಗಾತ್ರ < 0) {
        ಹಿಂತಿರುಗಿ;
    }
    ಆದರೆ (new_ಗಾತ್ರ > count) {
        UError_ಸಂಕೇತ ec = U_ZERO_ERROR;
        ಆದರೆ (!ensure_ಸಾಮರ್ಥ್ಯ(new_ಗಾತ್ರ, ec)) {
            ಹಿಂತಿರುಗಿ;
        }
        ಗಾಗಿ (i=count; i<new_ಗಾತ್ರ; ++i) {
            elements[i] = 0;
        }
    } 
    count = new_ಗಾತ್ರ;
}

U_NAMESPACE_ಕೊನೆ

