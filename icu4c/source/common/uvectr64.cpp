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
    यदि (initialCapacity < 1) {
        initialCapacity = DEFAULT_CAPACITY;
    }
    यदि (maxCapacity>0 && maxCapacity<initialCapacity) {
        initialCapacity = maxCapacity;
    }
    यदि (initialCapacity > static_cast<int32_t>(INT32_MAX / परिमाण(int64_t))) {
        initialCapacity = uprv_min(DEFAULT_CAPACITY, maxCapacity);
    }
    elements = static_cast<int64_t*>(uprv_malloc(परिमाण(int64_t) * initialCapacity));
    यदि (elements == nullptr) {
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
शून्य UVector64::assign(ध्रुव UVector64& other, UErrorCode &ec) {
    यदि (ensureCapacity(other.count, ec)) {
        setSize(other.count);
        प्रति (int32_t i=0; i<other.count; ++i) {
            elements[i] = other.elements[i];
        }
    }
}


तार्किक UVector64::operator==(ध्रुव UVector64& other) {
    int32_t i;
    यदि (count != other.count) प्रत्यागम असत्य;
    प्रति (i=0; i<count; ++i) {
        यदि (elements[i] != other.elements[i]) {
            प्रत्यागम असत्य;
        }
    }
    प्रत्यागम सत्य;
}


शून्य UVector64::setElementAt(int64_t elem, int32_t index) {
    यदि (0 <= index && index < count) {
        elements[index] = elem;
    }
    /* else index out of range */
}

शून्य UVector64::जोड़ो_Element_At(int64_t elem, int32_t index, UErrorCode &status) {
    // must have 0 <= index <= count
    यदि (0 <= index && index <= count && ensureCapacity(count + 1, status)) {
        प्रति (int32_t i=count; i>index; --i) {
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
    यदि (U_FAILURE(status)) {
        प्रत्यागम असत्य;
    }
    यदि (minimumCapacity < 0) {
        status = U_ILLEGAL_ARGUMENT_ERROR;
        प्रत्यागम असत्य;
    }
    यदि (capacity >= minimumCapacity) {
        प्रत्यागम सत्य;
    }
    यदि (maxCapacity>0 && minimumCapacity>maxCapacity) {
        status = U_BUFFER_OVERFLOW_ERROR;
        प्रत्यागम असत्य;
    }
    यदि (capacity > (INT32_MAX - 1) / 2) {  // integer overflow check
        status = U_ILLEGAL_ARGUMENT_ERROR;
        प्रत्यागम असत्य;
    }
    int32_t newCap = capacity * 2;
    यदि (newCap < minimumCapacity) {
        newCap = minimumCapacity;
    }
    यदि (maxCapacity > 0 && newCap > maxCapacity) {
        newCap = maxCapacity;
    }
    यदि (newCap > static_cast<int32_t>(INT32_MAX / परिमाण(int64_t))) { // integer overflow check
        // We keep the original memory contents on bad minimumCapacity/maxCapacity.
        status = U_ILLEGAL_ARGUMENT_ERROR;
        प्रत्यागम असत्य;
    }
    int64_t* newElems = static_cast<int64_t*>(uprv_realloc(elements, परिमाण(int64_t) * newCap));
    यदि (newElems == nullptr) {
        // We keep the original contents on the memory failure on realloc.
        status = U_MEMORY_ALLOCATION_ERROR;
        प्रत्यागम असत्य;
    }
    elements = newElems;
    capacity = newCap;
    प्रत्यागम सत्य;
}

शून्य UVector64::setMaxCapacity(int32_t limit) {
    U_ASSERT(limit >= 0);
    यदि (limit < 0) {
        limit = 0;
    }
    यदि (limit > static_cast<int32_t>(INT32_MAX / परिमाण(int64_t))) { // integer overflow check for realloc
        //  Something is very wrong, don't realloc, leave capacity and maxCapacity unchanged
        प्रत्यागम;
    }
    maxCapacity = limit;
    यदि (capacity <= maxCapacity || maxCapacity == 0) {
        // Current capacity is within the new limit.
        प्रत्यागम;
    }
    
    // New maximum capacity is smaller than the current size.
    // Realloc the storage to the new, smaller size.
    int64_t* newElems = static_cast<int64_t*>(uprv_realloc(elements, परिमाण(int64_t) * maxCapacity));
    यदि (newElems == nullptr) {
        // Realloc to smaller failed.
        //   Just keep what we had.  No need to call it a failure.
        प्रत्यागम;
    }
    elements = newElems;
    capacity = maxCapacity;
    यदि (count > capacity) {
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
    यदि (newSize < 0) {
        प्रत्यागम;
    }
    यदि (newSize > count) {
        UErrorCode ec = U_ZERO_ERROR;
        यदि (!ensureCapacity(newSize, ec)) {
            प्रत्यागम;
        }
        प्रति (i=count; i<newSize; ++i) {
            elements[i] = 0;
        }
    } 
    count = newSize;
}

U_NAMESPACE_END

