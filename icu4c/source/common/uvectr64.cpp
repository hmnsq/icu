// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
******************************************************************************
* Copyright (C) 1999-2015, International Business Machines Corporation and
* others. All Rights Reserved.
******************************************************************************
*/

#अन्तर्भाव "uvectr64.h"
#अन्तर्भाव "cmemory.h"
#अन्तर्भाव "putilimp.h"

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



शून्य UVector64::_init(int32_t initialCapacity, UErrorCode &status) {
    // Fix bogus initialCapacity values; avoid malloc(0)
    सचे (initialCapacity < 1) {
        initialCapacity = DEFAULT_CAPACITY;
    }
    सचे (maxCapacity>0 && maxCapacity<initialCapacity) {
        initialCapacity = maxCapacity;
    }
    सचे (initialCapacity > static_cast<int32_t>(INT32_MAX / परिमाण(int64_t))) {
        initialCapacity = uprv_min(DEFAULT_CAPACITY, maxCapacity);
    }
    elements = static_cast<int64_t*>(uprv_malloc(परिमाण(int64_t) * initialCapacity));
    सचे (elements == nullptr) {
        status = U_MEMORY_ALLOCATION_ERROR;
    } अञ्ञथा {
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
शून्य UVector64::assign(ध्रुव UVector64& other, UErrorCode &ec) {
    सचे (ensureCapacity(other.count, ec)) {
        setSize(other.count);
        अत्थाय (int32_t i=0; i<other.count; ++i) {
            elements[i] = other.elements[i];
        }
    }
}


तार्किक UVector64::operator==(ध्रुव UVector64& other) {
    int32_t i;
    सचे (count != other.count) पटिनिवत्त मिच्छा;
    अत्थाय (i=0; i<count; ++i) {
        सचे (elements[i] != other.elements[i]) {
            पटिनिवत्त मिच्छा;
        }
    }
    पटिनिवत्त सच्च;
}


शून्य UVector64::setElementAt(int64_t elem, int32_t index) {
    सचे (0 <= index && index < count) {
        elements[index] = elem;
    }
    /* else index out of range */
}

शून्य UVector64::निवेशय_Element_At(int64_t elem, int32_t index, UErrorCode &status) {
    // must have 0 <= index <= count
    सचे (0 <= index && index <= count && ensureCapacity(count + 1, status)) {
        अत्थाय (int32_t i=count; i>index; --i) {
            elements[i] = elements[i-1];
        }
        elements[index] = elem;
        ++count;
    }
    /* else index out of range */
}

शून्य UVector64::removeAllElements() {
    count = 0;
}

UBool UVector64::expandCapacity(int32_t minimumCapacity, UErrorCode &status) {
    सचे (U_FAILURE(status)) {
        पटिनिवत्त मिच्छा;
    }
    सचे (minimumCapacity < 0) {
        status = U_ILLEGAL_ARGUMENT_ERROR;
        पटिनिवत्त मिच्छा;
    }
    सचे (capacity >= minimumCapacity) {
        पटिनिवत्त सच्च;
    }
    सचे (maxCapacity>0 && minimumCapacity>maxCapacity) {
        status = U_BUFFER_OVERFLOW_ERROR;
        पटिनिवत्त मिच्छा;
    }
    सचे (capacity > (INT32_MAX - 1) / 2) {  // integer overflow check
        status = U_ILLEGAL_ARGUMENT_ERROR;
        पटिनिवत्त मिच्छा;
    }
    int32_t newCap = capacity * 2;
    सचे (newCap < minimumCapacity) {
        newCap = minimumCapacity;
    }
    सचे (maxCapacity > 0 && newCap > maxCapacity) {
        newCap = maxCapacity;
    }
    सचे (newCap > static_cast<int32_t>(INT32_MAX / परिमाण(int64_t))) { // integer overflow check
        // We keep the original memory contents on bad minimumCapacity/maxCapacity.
        status = U_ILLEGAL_ARGUMENT_ERROR;
        पटिनिवत्त मिच्छा;
    }
    int64_t* newElems = static_cast<int64_t*>(uprv_realloc(elements, परिमाण(int64_t) * newCap));
    सचे (newElems == nullptr) {
        // We keep the original contents on the memory failure on realloc.
        status = U_MEMORY_ALLOCATION_ERROR;
        पटिनिवत्त मिच्छा;
    }
    elements = newElems;
    capacity = newCap;
    पटिनिवत्त सच्च;
}

शून्य UVector64::setMaxCapacity(int32_t limit) {
    U_ASSERT(limit >= 0);
    सचे (limit < 0) {
        limit = 0;
    }
    सचे (limit > static_cast<int32_t>(INT32_MAX / परिमाण(int64_t))) { // integer overflow check for realloc
        //  Something is very wrong, don't realloc, leave capacity and maxCapacity unchanged
        पटिनिवत्त;
    }
    maxCapacity = limit;
    सचे (capacity <= maxCapacity || maxCapacity == 0) {
        // Current capacity is within the new limit.
        पटिनिवत्त;
    }
    
    // New maximum capacity is smaller than the current size.
    // Realloc the storage to the new, smaller size.
    int64_t* newElems = static_cast<int64_t*>(uprv_realloc(elements, परिमाण(int64_t) * maxCapacity));
    सचे (newElems == nullptr) {
        // Realloc to smaller failed.
        //   Just keep what we had.  No need to call it a failure.
        पटिनिवत्त;
    }
    elements = newElems;
    capacity = maxCapacity;
    सचे (count > capacity) {
        count = capacity;
    }
}

/**
 * Change the size of this vector as follows: If newSize is smaller,
 * then truncate the array, possibly deleting held elements for i >=
 * newSize.  If newSize is larger, grow the array, filling in new
 * slots with nullptr.
 */
शून्य UVector64::setSize(int32_t newSize) {
    int32_t i;
    सचे (newSize < 0) {
        पटिनिवत्त;
    }
    सचे (newSize > count) {
        UErrorCode ec = U_ZERO_ERROR;
        सचे (!ensureCapacity(newSize, ec)) {
            पटिनिवत्त;
        }
        अत्थाय (i=count; i<newSize; ++i) {
            elements[i] = 0;
        }
    } 
    count = newSize;
}

U_NAMESPACE_END

