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
    ако (initialCapacity < 1) {
        initialCapacity = DEFAULT_CAPACITY;
    }
    ако (maxCapacity>0 && maxCapacity<initialCapacity) {
        initialCapacity = maxCapacity;
    }
    ако (initialCapacity > static_cast<int32_t>(INT32_MAX / sizeof(int64_t))) {
        initialCapacity = uprv_min(DEFAULT_CAPACITY, maxCapacity);
    }
    elements = static_cast<int64_t*>(uprv_malloc(sizeof(int64_t) * initialCapacity));
    ако (elements == nullptr) {
        status = U_MEMORY_ALLOCATION_ERROR;
    } инако {
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
    ако (ensureCapacity(other.count, ec)) {
        setSize(other.count);
        за (int32_t i=0; i<other.count; ++i) {
            elements[i] = other.elements[i];
        }
    }
}


bool UVector64::operator==(const UVector64& other) {
    int32_t i;
    ако (count != other.count) врати false;
    за (i=0; i<count; ++i) {
        ако (elements[i] != other.elements[i]) {
            врати false;
        }
    }
    врати true;
}


void UVector64::setElementAt(int64_t elem, int32_t index) {
    ако (0 <= index && index < count) {
        elements[index] = elem;
    }
    /* else index out of range */
}

void UVector64::insertElementAt(int64_t elem, int32_t index, UErrorCode &status) {
    // must have 0 <= index <= count
    ако (0 <= index && index <= count && ensureCapacity(count + 1, status)) {
        за (int32_t i=count; i>index; --i) {
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
    ако (U_FAILURE(status)) {
        врати false;
    }
    ако (minimumCapacity < 0) {
        status = U_ILLEGAL_ARGUMENT_ERROR;
        врати false;
    }
    ако (capacity >= minimumCapacity) {
        врати true;
    }
    ако (maxCapacity>0 && minimumCapacity>maxCapacity) {
        status = U_BUFFER_OVERFLOW_ERROR;
        врати false;
    }
    ако (capacity > (INT32_MAX - 1) / 2) {  // integer overflow check
        status = U_ILLEGAL_ARGUMENT_ERROR;
        врати false;
    }
    int32_t newCap = capacity * 2;
    ако (newCap < minimumCapacity) {
        newCap = minimumCapacity;
    }
    ако (maxCapacity > 0 && newCap > maxCapacity) {
        newCap = maxCapacity;
    }
    ако (newCap > static_cast<int32_t>(INT32_MAX / sizeof(int64_t))) { // integer overflow check
        // We keep the original memory contents on bad minimumCapacity/maxCapacity.
        status = U_ILLEGAL_ARGUMENT_ERROR;
        врати false;
    }
    int64_t* newElems = static_cast<int64_t*>(uprv_realloc(elements, sizeof(int64_t) * newCap));
    ако (newElems == nullptr) {
        // We keep the original contents on the memory failure on realloc.
        status = U_MEMORY_ALLOCATION_ERROR;
        врати false;
    }
    elements = newElems;
    capacity = newCap;
    врати true;
}

void UVector64::setMaxCapacity(int32_t limit) {
    U_ASSERT(limit >= 0);
    ако (limit < 0) {
        limit = 0;
    }
    ако (limit > static_cast<int32_t>(INT32_MAX / sizeof(int64_t))) { // integer overflow check for realloc
        //  Something is very wrong, don't realloc, leave capacity and maxCapacity unchanged
        врати;
    }
    maxCapacity = limit;
    ако (capacity <= maxCapacity || maxCapacity == 0) {
        // Current capacity is within the new limit.
        врати;
    }
    
    // New maximum capacity is smaller than the current size.
    // Realloc the storage to the new, smaller size.
    int64_t* newElems = static_cast<int64_t*>(uprv_realloc(elements, sizeof(int64_t) * maxCapacity));
    ако (newElems == nullptr) {
        // Realloc to smaller failed.
        //   Just keep what we had.  No need to call it a failure.
        врати;
    }
    elements = newElems;
    capacity = maxCapacity;
    ако (count > capacity) {
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
    ако (newSize < 0) {
        врати;
    }
    ако (newSize > count) {
        UErrorCode ec = U_ZERO_ERROR;
        ако (!ensureCapacity(newSize, ec)) {
            врати;
        }
        за (i=count; i<newSize; ++i) {
            elements[i] = 0;
        }
    } 
    count = newSize;
}

U_NAMESPACE_END

