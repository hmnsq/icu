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

#define DEFAULT_CAPACITY 8

/*
 * Constants for hinting whether a key is an integer
 * or a pointer.  If a hint bit is zero, then the associated
 * token is assumed to be an integer. This is needed for iSeries
 */
 
UOBJECT_DEFINE_RTTI_IMPLEMENTATION(UVector64)

UVector64::UVector64(UErrorCode &status) :
    count(0),
    capacity(0),
    maxCapacity(0),
    elements(nullptr)
{
    _init(DEFAULT_CAPACITY, status);
}

UVector64::UVector64(int32_t initialCapacity, UErrorCode &status) :
    count(0),
    capacity(0),
    maxCapacity(0),
    elements(nullptr)
{
    _init(initialCapacity, status);
}



ಶೂನ್ಯ UVector64::_init(int32_t initialCapacity, UErrorCode &status) {
    // Fix bogus initialCapacity values; avoid malloc(0)
    ಯದಿ (initialCapacity < 1) {
        initialCapacity = DEFAULT_CAPACITY;
    }
    ಯದಿ (maxCapacity>0 && maxCapacity<initialCapacity) {
        initialCapacity = maxCapacity;
    }
    ಯದಿ (initialCapacity > static_cast<int32_t>(INT32_MAX / ಪರಿಮಾಣ(int64_t))) {
        initialCapacity = uprv_min(DEFAULT_CAPACITY, maxCapacity);
    }
    elements = static_cast<int64_t*>(uprv_malloc(ಪರಿಮಾಣ(int64_t) * initialCapacity));
    ಯದಿ (elements == nullptr) {
        status = U_MEMORY_ALLOCATION_ERROR;
    } ಅನ್ಯಥಾ {
        capacity = initialCapacity;
    }
}

UVector64::~UVector64() {
    uprv_free(elements);
    elements = nullptr;
}

/**
 * Assign this object to another (make this a copy of 'other').
 */
ಶೂನ್ಯ UVector64::assign(ಧ್ರುವ UVector64& other, UErrorCode &ec) {
    ಯದಿ (ensureCapacity(other.count, ec)) {
        setSize(other.count);
        ಪ್ರತಿ (int32_t i=0; i<other.count; ++i) {
            elements[i] = other.elements[i];
        }
    }
}


ತಾರ್ಕಿಕ UVector64::operator==(ಧ್ರುವ UVector64& other) {
    int32_t i;
    ಯದಿ (count != other.count) ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;
    ಪ್ರತಿ (i=0; i<count; ++i) {
        ಯದಿ (elements[i] != other.elements[i]) {
            ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;
        }
    }
    ಪ್ರತ್ಯಾಗಮ ಸತ್ಯ;
}


ಶೂನ್ಯ UVector64::setElementAt(int64_t elem, int32_t index) {
    ಯದಿ (0 <= index && index < count) {
        elements[index] = elem;
    }
    /* else index out of range */
}

ಶೂನ್ಯ UVector64::ಸೇರಿಸು_Element_At(int64_t elem, int32_t index, UErrorCode &status) {
    // must have 0 <= index <= count
    ಯದಿ (0 <= index && index <= count && ensureCapacity(count + 1, status)) {
        ಪ್ರತಿ (int32_t i=count; i>index; --i) {
            elements[i] = elements[i-1];
        }
        elements[index] = elem;
        ++count;
    }
    /* else index out of range */
}

ಶೂನ್ಯ UVector64::removeAllElements() {
    count = 0;
}

UBool UVector64::expandCapacity(int32_t minimumCapacity, UErrorCode &status) {
    ಯದಿ (U_FAILURE(status)) {
        ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;
    }
    ಯದಿ (minimumCapacity < 0) {
        status = U_ILLEGAL_ARGUMENT_ERROR;
        ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;
    }
    ಯದಿ (capacity >= minimumCapacity) {
        ಪ್ರತ್ಯಾಗಮ ಸತ್ಯ;
    }
    ಯದಿ (maxCapacity>0 && minimumCapacity>maxCapacity) {
        status = U_BUFFER_OVERFLOW_ERROR;
        ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;
    }
    ಯದಿ (capacity > (INT32_MAX - 1) / 2) {  // integer overflow check
        status = U_ILLEGAL_ARGUMENT_ERROR;
        ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;
    }
    int32_t newCap = capacity * 2;
    ಯದಿ (newCap < minimumCapacity) {
        newCap = minimumCapacity;
    }
    ಯದಿ (maxCapacity > 0 && newCap > maxCapacity) {
        newCap = maxCapacity;
    }
    ಯದಿ (newCap > static_cast<int32_t>(INT32_MAX / ಪರಿಮಾಣ(int64_t))) { // integer overflow check
        // We keep the original memory contents on bad minimumCapacity/maxCapacity.
        status = U_ILLEGAL_ARGUMENT_ERROR;
        ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;
    }
    int64_t* newElems = static_cast<int64_t*>(uprv_realloc(elements, ಪರಿಮಾಣ(int64_t) * newCap));
    ಯದಿ (newElems == nullptr) {
        // We keep the original contents on the memory failure on realloc.
        status = U_MEMORY_ALLOCATION_ERROR;
        ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;
    }
    elements = newElems;
    capacity = newCap;
    ಪ್ರತ್ಯಾಗಮ ಸತ್ಯ;
}

ಶೂನ್ಯ UVector64::setMaxCapacity(int32_t limit) {
    U_ASSERT(limit >= 0);
    ಯದಿ (limit < 0) {
        limit = 0;
    }
    ಯದಿ (limit > static_cast<int32_t>(INT32_MAX / ಪರಿಮಾಣ(int64_t))) { // integer overflow check for realloc
        //  Something is very wrong, don't realloc, leave capacity and maxCapacity unchanged
        ಪ್ರತ್ಯಾಗಮ;
    }
    maxCapacity = limit;
    ಯದಿ (capacity <= maxCapacity || maxCapacity == 0) {
        // Current capacity is within the new limit.
        ಪ್ರತ್ಯಾಗಮ;
    }
    
    // New maximum capacity is smaller than the current size.
    // Realloc the storage to the new, smaller size.
    int64_t* newElems = static_cast<int64_t*>(uprv_realloc(elements, ಪರಿಮಾಣ(int64_t) * maxCapacity));
    ಯದಿ (newElems == nullptr) {
        // Realloc to smaller failed.
        //   Just keep what we had.  No need to call it a failure.
        ಪ್ರತ್ಯಾಗಮ;
    }
    elements = newElems;
    capacity = maxCapacity;
    ಯದಿ (count > capacity) {
        count = capacity;
    }
}

/**
 * Change the size of this vector as follows: If newSize is smaller,
 * then truncate the array, possibly deleting held elements for i >=
 * newSize.  If newSize is larger, grow the array, filling in new
 * slots with nullptr.
 */
ಶೂನ್ಯ UVector64::setSize(int32_t newSize) {
    int32_t i;
    ಯದಿ (newSize < 0) {
        ಪ್ರತ್ಯಾಗಮ;
    }
    ಯದಿ (newSize > count) {
        UErrorCode ec = U_ZERO_ERROR;
        ಯದಿ (!ensureCapacity(newSize, ec)) {
            ಪ್ರತ್ಯಾಗಮ;
        }
        ಪ್ರತಿ (i=count; i<newSize; ++i) {
            elements[i] = 0;
        }
    } 
    count = newSize;
}

U_NAMESPACE_END

