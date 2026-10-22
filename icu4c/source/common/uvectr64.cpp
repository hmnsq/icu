// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
******************************************************************************
* Copyright (C) 1999-2015, International Business Machines Corporation and
* others. All Rights Reserved.
******************************************************************************
*/

#समावेश "uvectr64.h"
#समावेश "cmemory.h"
#समावेश "putilimp.h"

U_NAMESPACE_BEGIN

#घोषणा DEFAULT_CAPACITY 8

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



व्योम UVector64::_init(int32_t initialCapacity, UErrorCode &status) {
    // Fix bogus initialCapacity values; avoid malloc(0)
    अगर (initialCapacity < 1) {
        initialCapacity = DEFAULT_CAPACITY;
    }
    अगर (maxCapacity>0 && maxCapacity<initialCapacity) {
        initialCapacity = maxCapacity;
    }
    अगर (initialCapacity > static_cast<int32_t>(INT32_MAX / माप(int64_t))) {
        initialCapacity = uprv_min(DEFAULT_CAPACITY, maxCapacity);
    }
    elements = static_cast<int64_t*>(uprv_malloc(माप(int64_t) * initialCapacity));
    अगर (elements == nullptr) {
        status = U_MEMORY_ALLOCATION_ERROR;
    } अन्यथा {
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
व्योम UVector64::assign(स्थिर UVector64& other, UErrorCode &ec) {
    अगर (ensureCapacity(other.count, ec)) {
        set_आकार(other.count);
        क्रम (int32_t i=0; i<other.count; ++i) {
            elements[i] = other.elements[i];
        }
    }
}


bool UVector64::चालक==(स्थिर UVector64& other) {
    int32_t i;
    अगर (count != other.count) वापस false;
    क्रम (i=0; i<count; ++i) {
        अगर (elements[i] != other.elements[i]) {
            वापस false;
        }
    }
    वापस true;
}


व्योम UVector64::setElementAt(int64_t elem, int32_t सूचकांक) {
    अगर (0 <= सूचकांक && सूचकांक < count) {
        elements[सूचकांक] = elem;
    }
    /* else index out of range */
}

व्योम UVector64::जोड़ो_Element_At(int64_t elem, int32_t सूचकांक, UErrorCode &status) {
    // must have 0 <= index <= count
    अगर (0 <= सूचकांक && सूचकांक <= count && ensureCapacity(count + 1, status)) {
        क्रम (int32_t i=count; i>सूचकांक; --i) {
            elements[i] = elements[i-1];
        }
        elements[सूचकांक] = elem;
        ++count;
    }
    /* else index out of range */
}

व्योम UVector64::removeAllElements() {
    count = 0;
}

UBool UVector64::expandCapacity(int32_t minimumCapacity, UErrorCode &status) {
    अगर (U_FAILURE(status)) {
        वापस false;
    }
    अगर (minimumCapacity < 0) {
        status = U_ILLEGAL_ARGUMENT_ERROR;
        वापस false;
    }
    अगर (capacity >= minimumCapacity) {
        वापस true;
    }
    अगर (maxCapacity>0 && minimumCapacity>maxCapacity) {
        status = U_बफर_OVERFLOW_ERROR;
        वापस false;
    }
    अगर (capacity > (INT32_MAX - 1) / 2) {  // integer overflow check
        status = U_ILLEGAL_ARGUMENT_ERROR;
        वापस false;
    }
    int32_t newCap = capacity * 2;
    अगर (newCap < minimumCapacity) {
        newCap = minimumCapacity;
    }
    अगर (maxCapacity > 0 && newCap > maxCapacity) {
        newCap = maxCapacity;
    }
    अगर (newCap > static_cast<int32_t>(INT32_MAX / माप(int64_t))) { // integer overflow check
        // We keep the original memory contents on bad minimumCapacity/maxCapacity.
        status = U_ILLEGAL_ARGUMENT_ERROR;
        वापस false;
    }
    int64_t* newElems = static_cast<int64_t*>(uprv_realloc(elements, माप(int64_t) * newCap));
    अगर (newElems == nullptr) {
        // We keep the original contents on the memory failure on realloc.
        status = U_MEMORY_ALLOCATION_ERROR;
        वापस false;
    }
    elements = newElems;
    capacity = newCap;
    वापस true;
}

व्योम UVector64::setMaxCapacity(int32_t limit) {
    U_ASSERT(limit >= 0);
    अगर (limit < 0) {
        limit = 0;
    }
    अगर (limit > static_cast<int32_t>(INT32_MAX / माप(int64_t))) { // integer overflow check for realloc
        //  Something is very wrong, don't realloc, leave capacity and maxCapacity unchanged
        वापस;
    }
    maxCapacity = limit;
    अगर (capacity <= maxCapacity || maxCapacity == 0) {
        // Current capacity is within the new limit.
        वापस;
    }
    
    // New maximum capacity is smaller than the current size.
    // Realloc the storage to the new, smaller size.
    int64_t* newElems = static_cast<int64_t*>(uprv_realloc(elements, माप(int64_t) * maxCapacity));
    अगर (newElems == nullptr) {
        // Realloc to smaller failed.
        //   Just keep what we had.  No need to call it a failure.
        वापस;
    }
    elements = newElems;
    capacity = maxCapacity;
    अगर (count > capacity) {
        count = capacity;
    }
}

/**
 * Change the size of this vector as follows: If newSize is smaller,
 * then truncate the array, possibly deleting held elements for i >=
 * newSize.  If newSize is larger, grow the array, filling in new
 * slots with nullptr.
 */
व्योम UVector64::set_आकार(int32_t new_आकार) {
    int32_t i;
    अगर (new_आकार < 0) {
        वापस;
    }
    अगर (new_आकार > count) {
        UErrorCode ec = U_ZERO_ERROR;
        अगर (!ensureCapacity(new_आकार, ec)) {
            वापस;
        }
        क्रम (i=count; i<new_आकार; ++i) {
            elements[i] = 0;
        }
    } 
    count = new_आकार;
}

U_NAMESPACE_END

