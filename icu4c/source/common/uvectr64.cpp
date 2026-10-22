// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
******************************************************************************
* Copyright (C) 1999-2015, International Business Machines Corporation and
* others. All Rights Reserved.
******************************************************************************
*/

#அந்தர்பாவ "uvectr64.h"
#அந்தர்பாவ "cmemory.h"
#அந்தர்பாவ "putilimp.h"

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



வெறுமை UVector64::_init(int32_t initialCapacity, UErrorCode &status) {
    // Fix bogus initialCapacity values; avoid malloc(0)
    எனில் (initialCapacity < 1) {
        initialCapacity = DEFAULT_CAPACITY;
    }
    எனில் (maxCapacity>0 && maxCapacity<initialCapacity) {
        initialCapacity = maxCapacity;
    }
    எனில் (initialCapacity > static_cast<int32_t>(INT32_MAX / அளவு(int64_t))) {
        initialCapacity = uprv_min(DEFAULT_CAPACITY, maxCapacity);
    }
    elements = static_cast<int64_t*>(uprv_malloc(அளவு(int64_t) * initialCapacity));
    எனில் (elements == nullptr) {
        status = U_MEMORY_ALLOCATION_ERROR;
    } இல்லையெனில் {
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
வெறுமை UVector64::assign(மாறிலி UVector64& other, UErrorCode &ec) {
    எனில் (ensureCapacity(other.count, ec)) {
        setSize(other.count);
        க்கு (int32_t i=0; i<other.count; ++i) {
            elements[i] = other.elements[i];
        }
    }
}


தருக்க UVector64::operator==(மாறிலி UVector64& other) {
    int32_t i;
    எனில் (count != other.count) திரும்பு பொய்;
    க்கு (i=0; i<count; ++i) {
        எனில் (elements[i] != other.elements[i]) {
            திரும்பு பொய்;
        }
    }
    திரும்பு உண்மை;
}


வெறுமை UVector64::setElementAt(int64_t elem, int32_t index) {
    எனில் (0 <= index && index < count) {
        elements[index] = elem;
    }
    /* else index out of range */
}

வெறுமை UVector64::செருகு_Element_At(int64_t elem, int32_t index, UErrorCode &status) {
    // must have 0 <= index <= count
    எனில் (0 <= index && index <= count && ensureCapacity(count + 1, status)) {
        க்கு (int32_t i=count; i>index; --i) {
            elements[i] = elements[i-1];
        }
        elements[index] = elem;
        ++count;
    }
    /* else index out of range */
}

வெறுமை UVector64::removeAllElements() {
    count = 0;
}

UBool UVector64::expandCapacity(int32_t minimumCapacity, UErrorCode &status) {
    எனில் (U_FAILURE(status)) {
        திரும்பு பொய்;
    }
    எனில் (minimumCapacity < 0) {
        status = U_ILLEGAL_ARGUMENT_ERROR;
        திரும்பு பொய்;
    }
    எனில் (capacity >= minimumCapacity) {
        திரும்பு உண்மை;
    }
    எனில் (maxCapacity>0 && minimumCapacity>maxCapacity) {
        status = U_BUFFER_OVERFLOW_ERROR;
        திரும்பு பொய்;
    }
    எனில் (capacity > (INT32_MAX - 1) / 2) {  // integer overflow check
        status = U_ILLEGAL_ARGUMENT_ERROR;
        திரும்பு பொய்;
    }
    int32_t newCap = capacity * 2;
    எனில் (newCap < minimumCapacity) {
        newCap = minimumCapacity;
    }
    எனில் (maxCapacity > 0 && newCap > maxCapacity) {
        newCap = maxCapacity;
    }
    எனில் (newCap > static_cast<int32_t>(INT32_MAX / அளவு(int64_t))) { // integer overflow check
        // We keep the original memory contents on bad minimumCapacity/maxCapacity.
        status = U_ILLEGAL_ARGUMENT_ERROR;
        திரும்பு பொய்;
    }
    int64_t* newElems = static_cast<int64_t*>(uprv_realloc(elements, அளவு(int64_t) * newCap));
    எனில் (newElems == nullptr) {
        // We keep the original contents on the memory failure on realloc.
        status = U_MEMORY_ALLOCATION_ERROR;
        திரும்பு பொய்;
    }
    elements = newElems;
    capacity = newCap;
    திரும்பு உண்மை;
}

வெறுமை UVector64::setMaxCapacity(int32_t limit) {
    U_ASSERT(limit >= 0);
    எனில் (limit < 0) {
        limit = 0;
    }
    எனில் (limit > static_cast<int32_t>(INT32_MAX / அளவு(int64_t))) { // integer overflow check for realloc
        //  Something is very wrong, don't realloc, leave capacity and maxCapacity unchanged
        திரும்பு;
    }
    maxCapacity = limit;
    எனில் (capacity <= maxCapacity || maxCapacity == 0) {
        // Current capacity is within the new limit.
        திரும்பு;
    }
    
    // New maximum capacity is smaller than the current size.
    // Realloc the storage to the new, smaller size.
    int64_t* newElems = static_cast<int64_t*>(uprv_realloc(elements, அளவு(int64_t) * maxCapacity));
    எனில் (newElems == nullptr) {
        // Realloc to smaller failed.
        //   Just keep what we had.  No need to call it a failure.
        திரும்பு;
    }
    elements = newElems;
    capacity = maxCapacity;
    எனில் (count > capacity) {
        count = capacity;
    }
}

/**
 * Change the size of this vector as follows: If newSize is smaller,
 * then truncate the array, possibly deleting held elements for i >=
 * newSize.  If newSize is larger, grow the array, filling in new
 * slots with nullptr.
 */
வெறுமை UVector64::setSize(int32_t newSize) {
    int32_t i;
    எனில் (newSize < 0) {
        திரும்பு;
    }
    எனில் (newSize > count) {
        UErrorCode ec = U_ZERO_ERROR;
        எனில் (!ensureCapacity(newSize, ec)) {
            திரும்பு;
        }
        க்கு (i=count; i<newSize; ++i) {
            elements[i] = 0;
        }
    } 
    count = newSize;
}

U_NAMESPACE_END

