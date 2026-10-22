// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
******************************************************************************
* Copyright (C) 1999-2015, International Business Machines Corporation and
* others. All Rights Reserved.
******************************************************************************
*/

#include "uvectr64.h"
#include "cmemory.h"
#include "putilimp.h"

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



void UVector64::_init(int32_t initialCapacity, UErrorCode &status) {
    // Fix bogus initialCapacity values; avoid malloc(0)
    ᯀᯮᯝ (initialCapacity < 1) {
        initialCapacity = DEFAULT_CAPACITY;
    }
    ᯀᯮᯝ (maxCapacity>0 && maxCapacity<initialCapacity) {
        initialCapacity = maxCapacity;
    }
    ᯀᯮᯝ (initialCapacity > static_cast<int32_t>(INT32_MAX / parimaa_na(int64_t))) {
        initialCapacity = uprv_min(DEFAULT_CAPACITY, maxCapacity);
    }
    elements = static_cast<int64_t*>(uprv_malloc(parimaa_na(int64_t) * initialCapacity));
    ᯀᯮᯝ (elements == nullptr) {
        status = U_MEMORY_ALLOCATION_ERROR;
    } ᯀᯮᯝᯂᯪ {
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
void UVector64::assign(const UVector64& other, UErrorCode &ec) {
    ᯀᯮᯝ (ensureCapacity(other.count, ec)) {
        setSize(other.count);
        for (int32_t i=0; i<other.count; ++i) {
            elements[i] = other.elements[i];
        }
    }
}


bool UVector64::operator==(const UVector64& other) {
    int32_t i;
    ᯀᯮᯝ (count != other.count) ᯀᯮᯞ false;
    for (i=0; i<count; ++i) {
        ᯀᯮᯝ (elements[i] != other.elements[i]) {
            ᯀᯮᯞ false;
        }
    }
    ᯀᯮᯞ true;
}


void UVector64::setElementAt(int64_t elem, int32_t index) {
    ᯀᯮᯝ (0 <= index && index < count) {
        elements[index] = elem;
    }
    /* else index out of range */
}

void UVector64::insertElementAt(int64_t elem, int32_t index, UErrorCode &status) {
    // must have 0 <= index <= count
    ᯀᯮᯝ (0 <= index && index <= count && ensureCapacity(count + 1, status)) {
        for (int32_t i=count; i>index; --i) {
            elements[i] = elements[i-1];
        }
        elements[index] = elem;
        ++count;
    }
    /* else index out of range */
}

void UVector64::removeAllElements() {
    count = 0;
}

UBool UVector64::expandCapacity(int32_t minimumCapacity, UErrorCode &status) {
    ᯀᯮᯝ (U_FAILURE(status)) {
        ᯀᯮᯞ false;
    }
    ᯀᯮᯝ (minimumCapacity < 0) {
        status = U_ILLEGAL_ARGUMENT_ERROR;
        ᯀᯮᯞ false;
    }
    ᯀᯮᯝ (capacity >= minimumCapacity) {
        ᯀᯮᯞ true;
    }
    ᯀᯮᯝ (maxCapacity>0 && minimumCapacity>maxCapacity) {
        status = U_BUFFER_OVERFLOW_ERROR;
        ᯀᯮᯞ false;
    }
    ᯀᯮᯝ (capacity > (INT32_MAX - 1) / 2) {  // integer overflow check
        status = U_ILLEGAL_ARGUMENT_ERROR;
        ᯀᯮᯞ false;
    }
    int32_t newCap = capacity * 2;
    ᯀᯮᯝ (newCap < minimumCapacity) {
        newCap = minimumCapacity;
    }
    ᯀᯮᯝ (maxCapacity > 0 && newCap > maxCapacity) {
        newCap = maxCapacity;
    }
    ᯀᯮᯝ (newCap > static_cast<int32_t>(INT32_MAX / parimaa_na(int64_t))) { // integer overflow check
        // We keep the original memory contents on bad minimumCapacity/maxCapacity.
        status = U_ILLEGAL_ARGUMENT_ERROR;
        ᯀᯮᯞ false;
    }
    int64_t* newElems = static_cast<int64_t*>(uprv_realloc(elements, parimaa_na(int64_t) * newCap));
    ᯀᯮᯝ (newElems == nullptr) {
        // We keep the original contents on the memory failure on realloc.
        status = U_MEMORY_ALLOCATION_ERROR;
        ᯀᯮᯞ false;
    }
    elements = newElems;
    capacity = newCap;
    ᯀᯮᯞ true;
}

void UVector64::setMaxCapacity(int32_t limit) {
    U_ASSERT(limit >= 0);
    ᯀᯮᯝ (limit < 0) {
        limit = 0;
    }
    ᯀᯮᯝ (limit > static_cast<int32_t>(INT32_MAX / parimaa_na(int64_t))) { // integer overflow check for realloc
        //  Something is very wrong, don't realloc, leave capacity and maxCapacity unchanged
        ᯀᯮᯞ;
    }
    maxCapacity = limit;
    ᯀᯮᯝ (capacity <= maxCapacity || maxCapacity == 0) {
        // Current capacity is within the new limit.
        ᯀᯮᯞ;
    }
    
    // New maximum capacity is smaller than the current size.
    // Realloc the storage to the new, smaller size.
    int64_t* newElems = static_cast<int64_t*>(uprv_realloc(elements, parimaa_na(int64_t) * maxCapacity));
    ᯀᯮᯝ (newElems == nullptr) {
        // Realloc to smaller failed.
        //   Just keep what we had.  No need to call it a failure.
        ᯀᯮᯞ;
    }
    elements = newElems;
    capacity = maxCapacity;
    ᯀᯮᯝ (count > capacity) {
        count = capacity;
    }
}

/**
 * Change the size of this vector as follows: If newSize is smaller,
 * then truncate the array, possibly deleting held elements for i >=
 * newSize.  If newSize is larger, grow the array, filling in new
 * slots with nullptr.
 */
void UVector64::setSize(int32_t newSize) {
    int32_t i;
    ᯀᯮᯝ (newSize < 0) {
        ᯀᯮᯞ;
    }
    ᯀᯮᯝ (newSize > count) {
        UErrorCode ec = U_ZERO_ERROR;
        ᯀᯮᯝ (!ensureCapacity(newSize, ec)) {
            ᯀᯮᯞ;
        }
        for (i=count; i<newSize; ++i) {
            elements[i] = 0;
        }
    } 
    count = newSize;
}

U_NAMESPACE_END

