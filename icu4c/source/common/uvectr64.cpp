// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
******************************************************************************
* Copyright (C) 1999-2015, International Business Machines Corporation and
* others. All Rights Reserved.
******************************************************************************
*/

#شامل "uvectr64.h"
#شامل "cmemory.h"
#شامل "putilimp.h"

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



تهی UVector64::_init(int32_t initialCapacity, UErrorCode &status) {
    // Fix bogus initialCapacity values; avoid malloc(0)
    اگر (initialCapacity < 1) {
        initialCapacity = DEFAULT_CAPACITY;
    }
    اگر (maxCapacity>0 && maxCapacity<initialCapacity) {
        initialCapacity = maxCapacity;
    }
    اگر (initialCapacity > static_cast<int32_t>(INT32_MAX / اندازه(int64_t))) {
        initialCapacity = uprv_min(DEFAULT_CAPACITY, maxCapacity);
    }
    elements = static_cast<int64_t*>(uprv_malloc(اندازه(int64_t) * initialCapacity));
    اگر (elements == nullptr) {
        status = U_MEMORY_ALLOCATION_ERROR;
    } وگرنه {
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
تهی UVector64::assign(ثابت UVector64& other, UErrorCode &ec) {
    اگر (ensureCapacity(other.count, ec)) {
        setSize(other.count);
        برای (int32_t i=0; i<other.count; ++i) {
            elements[i] = other.elements[i];
        }
    }
}


منطقی UVector64::operator==(ثابت UVector64& other) {
    int32_t i;
    اگر (count != other.count) بازگشت نادرست;
    برای (i=0; i<count; ++i) {
        اگر (elements[i] != other.elements[i]) {
            بازگشت نادرست;
        }
    }
    بازگشت درست;
}


تهی UVector64::setElementAt(int64_t elem, int32_t index) {
    اگر (0 <= index && index < count) {
        elements[index] = elem;
    }
    /* else index out of range */
}

تهی UVector64::درج_Element_At(int64_t elem, int32_t index, UErrorCode &status) {
    // must have 0 <= index <= count
    اگر (0 <= index && index <= count && ensureCapacity(count + 1, status)) {
        برای (int32_t i=count; i>index; --i) {
            elements[i] = elements[i-1];
        }
        elements[index] = elem;
        ++count;
    }
    /* else index out of range */
}

تهی UVector64::removeAllElements() {
    count = 0;
}

UBool UVector64::expandCapacity(int32_t minimumCapacity, UErrorCode &status) {
    اگر (U_FAILURE(status)) {
        بازگشت نادرست;
    }
    اگر (minimumCapacity < 0) {
        status = U_ILLEGAL_ARGUMENT_ERROR;
        بازگشت نادرست;
    }
    اگر (capacity >= minimumCapacity) {
        بازگشت درست;
    }
    اگر (maxCapacity>0 && minimumCapacity>maxCapacity) {
        status = U_BUFFER_OVERFLOW_ERROR;
        بازگشت نادرست;
    }
    اگر (capacity > (INT32_MAX - 1) / 2) {  // integer overflow check
        status = U_ILLEGAL_ARGUMENT_ERROR;
        بازگشت نادرست;
    }
    int32_t newCap = capacity * 2;
    اگر (newCap < minimumCapacity) {
        newCap = minimumCapacity;
    }
    اگر (maxCapacity > 0 && newCap > maxCapacity) {
        newCap = maxCapacity;
    }
    اگر (newCap > static_cast<int32_t>(INT32_MAX / اندازه(int64_t))) { // integer overflow check
        // We keep the original memory contents on bad minimumCapacity/maxCapacity.
        status = U_ILLEGAL_ARGUMENT_ERROR;
        بازگشت نادرست;
    }
    int64_t* newElems = static_cast<int64_t*>(uprv_realloc(elements, اندازه(int64_t) * newCap));
    اگر (newElems == nullptr) {
        // We keep the original contents on the memory failure on realloc.
        status = U_MEMORY_ALLOCATION_ERROR;
        بازگشت نادرست;
    }
    elements = newElems;
    capacity = newCap;
    بازگشت درست;
}

تهی UVector64::setMaxCapacity(int32_t limit) {
    U_ASSERT(limit >= 0);
    اگر (limit < 0) {
        limit = 0;
    }
    اگر (limit > static_cast<int32_t>(INT32_MAX / اندازه(int64_t))) { // integer overflow check for realloc
        //  Something is very wrong, don't realloc, leave capacity and maxCapacity unchanged
        بازگشت;
    }
    maxCapacity = limit;
    اگر (capacity <= maxCapacity || maxCapacity == 0) {
        // Current capacity is within the new limit.
        بازگشت;
    }
    
    // New maximum capacity is smaller than the current size.
    // Realloc the storage to the new, smaller size.
    int64_t* newElems = static_cast<int64_t*>(uprv_realloc(elements, اندازه(int64_t) * maxCapacity));
    اگر (newElems == nullptr) {
        // Realloc to smaller failed.
        //   Just keep what we had.  No need to call it a failure.
        بازگشت;
    }
    elements = newElems;
    capacity = maxCapacity;
    اگر (count > capacity) {
        count = capacity;
    }
}

/**
 * Change the size of this vector as follows: If newSize is smaller,
 * then truncate the array, possibly deleting held elements for i >=
 * newSize.  If newSize is larger, grow the array, filling in new
 * slots with nullptr.
 */
تهی UVector64::setSize(int32_t newSize) {
    int32_t i;
    اگر (newSize < 0) {
        بازگشت;
    }
    اگر (newSize > count) {
        UErrorCode ec = U_ZERO_ERROR;
        اگر (!ensureCapacity(newSize, ec)) {
            بازگشت;
        }
        برای (i=count; i<newSize; ++i) {
            elements[i] = 0;
        }
    } 
    count = newSize;
}

U_NAMESPACE_END

