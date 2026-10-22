// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
******************************************************************************
* Copyright (C) 1999-2015, International Business Machines Corporation and
* others. All Rights Reserved.
******************************************************************************
*/

#અન્તર્ભાવ "uvectr64.h"
#અન્તર્ભાવ "cmemory.h"
#અન્તર્ભાવ "putilimp.h"

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



શૂન્ય UVector64::_init(int32_t initialCapacity, UErrorCode &status) {
    // Fix bogus initialCapacity values; avoid malloc(0)
    જો (initialCapacity < 1) {
        initialCapacity = DEFAULT_CAPACITY;
    }
    જો (maxCapacity>0 && maxCapacity<initialCapacity) {
        initialCapacity = maxCapacity;
    }
    જો (initialCapacity > static_cast<int32_t>(INT32_MAX / કદ(int64_t))) {
        initialCapacity = uprv_min(DEFAULT_CAPACITY, maxCapacity);
    }
    elements = static_cast<int64_t*>(uprv_malloc(કદ(int64_t) * initialCapacity));
    જો (elements == nullptr) {
        status = U_MEMORY_ALLOCATION_ERROR;
    } નહીંતર {
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
શૂન્ય UVector64::assign(અચલ UVector64& other, UErrorCode &ec) {
    જો (ensureCapacity(other.count, ec)) {
        setSize(other.count);
        માટે (int32_t i=0; i<other.count; ++i) {
            elements[i] = other.elements[i];
        }
    }
}


તાર્કિક UVector64::operator==(અચલ UVector64& other) {
    int32_t i;
    જો (count != other.count) પાછા ખોટું;
    માટે (i=0; i<count; ++i) {
        જો (elements[i] != other.elements[i]) {
            પાછા ખોટું;
        }
    }
    પાછા સાચું;
}


શૂન્ય UVector64::setElementAt(int64_t elem, int32_t index) {
    જો (0 <= index && index < count) {
        elements[index] = elem;
    }
    /* else index out of range */
}

શૂન્ય UVector64::ઉમેરો_Element_At(int64_t elem, int32_t index, UErrorCode &status) {
    // must have 0 <= index <= count
    જો (0 <= index && index <= count && ensureCapacity(count + 1, status)) {
        માટે (int32_t i=count; i>index; --i) {
            elements[i] = elements[i-1];
        }
        elements[index] = elem;
        ++count;
    }
    /* else index out of range */
}

શૂન્ય UVector64::removeAllElements() {
    count = 0;
}

UBool UVector64::expandCapacity(int32_t minimumCapacity, UErrorCode &status) {
    જો (U_FAILURE(status)) {
        પાછા ખોટું;
    }
    જો (minimumCapacity < 0) {
        status = U_ILLEGAL_ARGUMENT_ERROR;
        પાછા ખોટું;
    }
    જો (capacity >= minimumCapacity) {
        પાછા સાચું;
    }
    જો (maxCapacity>0 && minimumCapacity>maxCapacity) {
        status = U_BUFFER_OVERFLOW_ERROR;
        પાછા ખોટું;
    }
    જો (capacity > (INT32_MAX - 1) / 2) {  // integer overflow check
        status = U_ILLEGAL_ARGUMENT_ERROR;
        પાછા ખોટું;
    }
    int32_t newCap = capacity * 2;
    જો (newCap < minimumCapacity) {
        newCap = minimumCapacity;
    }
    જો (maxCapacity > 0 && newCap > maxCapacity) {
        newCap = maxCapacity;
    }
    જો (newCap > static_cast<int32_t>(INT32_MAX / કદ(int64_t))) { // integer overflow check
        // We keep the original memory contents on bad minimumCapacity/maxCapacity.
        status = U_ILLEGAL_ARGUMENT_ERROR;
        પાછા ખોટું;
    }
    int64_t* newElems = static_cast<int64_t*>(uprv_realloc(elements, કદ(int64_t) * newCap));
    જો (newElems == nullptr) {
        // We keep the original contents on the memory failure on realloc.
        status = U_MEMORY_ALLOCATION_ERROR;
        પાછા ખોટું;
    }
    elements = newElems;
    capacity = newCap;
    પાછા સાચું;
}

શૂન્ય UVector64::setMaxCapacity(int32_t limit) {
    U_ASSERT(limit >= 0);
    જો (limit < 0) {
        limit = 0;
    }
    જો (limit > static_cast<int32_t>(INT32_MAX / કદ(int64_t))) { // integer overflow check for realloc
        //  Something is very wrong, don't realloc, leave capacity and maxCapacity unchanged
        પાછા;
    }
    maxCapacity = limit;
    જો (capacity <= maxCapacity || maxCapacity == 0) {
        // Current capacity is within the new limit.
        પાછા;
    }
    
    // New maximum capacity is smaller than the current size.
    // Realloc the storage to the new, smaller size.
    int64_t* newElems = static_cast<int64_t*>(uprv_realloc(elements, કદ(int64_t) * maxCapacity));
    જો (newElems == nullptr) {
        // Realloc to smaller failed.
        //   Just keep what we had.  No need to call it a failure.
        પાછા;
    }
    elements = newElems;
    capacity = maxCapacity;
    જો (count > capacity) {
        count = capacity;
    }
}

/**
 * Change the size of this vector as follows: If newSize is smaller,
 * then truncate the array, possibly deleting held elements for i >=
 * newSize.  If newSize is larger, grow the array, filling in new
 * slots with nullptr.
 */
શૂન્ય UVector64::setSize(int32_t newSize) {
    int32_t i;
    જો (newSize < 0) {
        પાછા;
    }
    જો (newSize > count) {
        UErrorCode ec = U_ZERO_ERROR;
        જો (!ensureCapacity(newSize, ec)) {
            પાછા;
        }
        માટે (i=count; i<newSize; ++i) {
            elements[i] = 0;
        }
    } 
    count = newSize;
}

U_NAMESPACE_END

