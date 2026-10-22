// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
******************************************************************************
* Copyright (C) 1999-2015, International Business Machines Corporation and
* others. All Rights Reserved.
******************************************************************************
*/

#ضمن "uvectr64.h"
#ضمن "cmemory.h"
#ضمن "putilimp.h"

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



فراغ UVector64::_init(int32_t initialCapacity, UErrorCode &status) {
    // Fix bogus initialCapacity values; avoid malloc(0)
    ܐܢ (initialCapacity < 1) {
        initialCapacity = DEFAULT_CAPACITY;
    }
    ܐܢ (maxCapacity>0 && maxCapacity<initialCapacity) {
        initialCapacity = maxCapacity;
    }
    ܐܢ (initialCapacity > static_cast<int32_t>(INT32_MAX / حجم(int64_t))) {
        initialCapacity = uprv_min(DEFAULT_CAPACITY, maxCapacity);
    }
    elements = static_cast<int64_t*>(uprv_malloc(حجم(int64_t) * initialCapacity));
    ܐܢ (elements == nullptr) {
        status = U_MEMORY_ALLOCATION_ERROR;
    } ܐܚܪܢܐ {
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
فراغ UVector64::assign(ثابت UVector64& other, UErrorCode &ec) {
    ܐܢ (ensureCapacity(other.count, ec)) {
        setSize(other.count);
        لكل (int32_t i=0; i<other.count; ++i) {
            elements[i] = other.elements[i];
        }
    }
}


منطقي UVector64::operator==(ثابت UVector64& other) {
    int32_t i;
    ܐܢ (count != other.count) ܦܢܐ ܕܓܠܐ;
    لكل (i=0; i<count; ++i) {
        ܐܢ (elements[i] != other.elements[i]) {
            ܦܢܐ ܕܓܠܐ;
        }
    }
    ܦܢܐ ܫܪܪܐ;
}


فراغ UVector64::setElementAt(int64_t elem, int32_t index) {
    ܐܢ (0 <= index && index < count) {
        elements[index] = elem;
    }
    /* else index out of range */
}

فراغ UVector64::insertElementAt(int64_t elem, int32_t index, UErrorCode &status) {
    // must have 0 <= index <= count
    ܐܢ (0 <= index && index <= count && ensureCapacity(count + 1, status)) {
        لكل (int32_t i=count; i>index; --i) {
            elements[i] = elements[i-1];
        }
        elements[index] = elem;
        ++count;
    }
    /* else index out of range */
}

فراغ UVector64::removeAllElements() {
    count = 0;
}

UBool UVector64::expandCapacity(int32_t minimumCapacity, UErrorCode &status) {
    ܐܢ (U_FAILURE(status)) {
        ܦܢܐ ܕܓܠܐ;
    }
    ܐܢ (minimumCapacity < 0) {
        status = U_ILLEGAL_ARGUMENT_ERROR;
        ܦܢܐ ܕܓܠܐ;
    }
    ܐܢ (capacity >= minimumCapacity) {
        ܦܢܐ ܫܪܪܐ;
    }
    ܐܢ (maxCapacity>0 && minimumCapacity>maxCapacity) {
        status = U_BUFFER_OVERFLOW_ERROR;
        ܦܢܐ ܕܓܠܐ;
    }
    ܐܢ (capacity > (INT32_MAX - 1) / 2) {  // integer overflow check
        status = U_ILLEGAL_ARGUMENT_ERROR;
        ܦܢܐ ܕܓܠܐ;
    }
    int32_t newCap = capacity * 2;
    ܐܢ (newCap < minimumCapacity) {
        newCap = minimumCapacity;
    }
    ܐܢ (maxCapacity > 0 && newCap > maxCapacity) {
        newCap = maxCapacity;
    }
    ܐܢ (newCap > static_cast<int32_t>(INT32_MAX / حجم(int64_t))) { // integer overflow check
        // We keep the original memory contents on bad minimumCapacity/maxCapacity.
        status = U_ILLEGAL_ARGUMENT_ERROR;
        ܦܢܐ ܕܓܠܐ;
    }
    int64_t* newElems = static_cast<int64_t*>(uprv_realloc(elements, حجم(int64_t) * newCap));
    ܐܢ (newElems == nullptr) {
        // We keep the original contents on the memory failure on realloc.
        status = U_MEMORY_ALLOCATION_ERROR;
        ܦܢܐ ܕܓܠܐ;
    }
    elements = newElems;
    capacity = newCap;
    ܦܢܐ ܫܪܪܐ;
}

فراغ UVector64::setMaxCapacity(int32_t limit) {
    U_ASSERT(limit >= 0);
    ܐܢ (limit < 0) {
        limit = 0;
    }
    ܐܢ (limit > static_cast<int32_t>(INT32_MAX / حجم(int64_t))) { // integer overflow check for realloc
        //  Something is very wrong, don't realloc, leave capacity and maxCapacity unchanged
        ܦܢܐ;
    }
    maxCapacity = limit;
    ܐܢ (capacity <= maxCapacity || maxCapacity == 0) {
        // Current capacity is within the new limit.
        ܦܢܐ;
    }
    
    // New maximum capacity is smaller than the current size.
    // Realloc the storage to the new, smaller size.
    int64_t* newElems = static_cast<int64_t*>(uprv_realloc(elements, حجم(int64_t) * maxCapacity));
    ܐܢ (newElems == nullptr) {
        // Realloc to smaller failed.
        //   Just keep what we had.  No need to call it a failure.
        ܦܢܐ;
    }
    elements = newElems;
    capacity = maxCapacity;
    ܐܢ (count > capacity) {
        count = capacity;
    }
}

/**
 * Change the size of this vector as follows: If newSize is smaller,
 * then truncate the array, possibly deleting held elements for i >=
 * newSize.  If newSize is larger, grow the array, filling in new
 * slots with nullptr.
 */
فراغ UVector64::setSize(int32_t newSize) {
    int32_t i;
    ܐܢ (newSize < 0) {
        ܦܢܐ;
    }
    ܐܢ (newSize > count) {
        UErrorCode ec = U_ZERO_ERROR;
        ܐܢ (!ensureCapacity(newSize, ec)) {
            ܦܢܐ;
        }
        لكل (i=count; i<newSize; ++i) {
            elements[i] = 0;
        }
    } 
    count = newSize;
}

U_NAMESPACE_END

