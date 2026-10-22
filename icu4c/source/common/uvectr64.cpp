// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
******************************************************************************
* Copyright (C) 1999-2015, International Business Machines Corporation and
* others. All Rights Reserved.
******************************************************************************
*/

#ଅନ୍ତର୍ଭାଵ "uvectr64.h"
#ଅନ୍ତର୍ଭାଵ "cmemory.h"
#ଅନ୍ତର୍ଭାଵ "putilimp.h"

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



ଶୂନ୍ୟ UVector64::_init(int32_t initialCapacity, UErrorCode &status) {
    // Fix bogus initialCapacity values; avoid malloc(0)
    ଯଦି (initialCapacity < 1) {
        initialCapacity = DEFAULT_CAPACITY;
    }
    ଯଦି (maxCapacity>0 && maxCapacity<initialCapacity) {
        initialCapacity = maxCapacity;
    }
    ଯଦି (initialCapacity > static_cast<int32_t>(INT32_MAX / ଆକାର(int64_t))) {
        initialCapacity = uprv_min(DEFAULT_CAPACITY, maxCapacity);
    }
    elements = static_cast<int64_t*>(uprv_malloc(ଆକାର(int64_t) * initialCapacity));
    ଯଦି (elements == nullptr) {
        status = U_MEMORY_ALLOCATION_ERROR;
    } ଅନ୍ୟଥା {
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
ଶୂନ୍ୟ UVector64::assign(ଅଚଳ UVector64& other, UErrorCode &ec) {
    ଯଦି (ensureCapacity(other.count, ec)) {
        setSize(other.count);
        ପାଇଁ (int32_t i=0; i<other.count; ++i) {
            elements[i] = other.elements[i];
        }
    }
}


ତାର୍କିକ UVector64::operator==(ଅଚଳ UVector64& other) {
    int32_t i;
    ଯଦି (count != other.count) ଫେରନ୍ତୁ ମିଥ୍ୟା;
    ପାଇଁ (i=0; i<count; ++i) {
        ଯଦି (elements[i] != other.elements[i]) {
            ଫେରନ୍ତୁ ମିଥ୍ୟା;
        }
    }
    ଫେରନ୍ତୁ ସତ୍ୟ;
}


ଶୂନ୍ୟ UVector64::setElementAt(int64_t elem, int32_t index) {
    ଯଦି (0 <= index && index < count) {
        elements[index] = elem;
    }
    /* else index out of range */
}

ଶୂନ୍ୟ UVector64::ଭର୍ତ୍ତି_Element_At(int64_t elem, int32_t index, UErrorCode &status) {
    // must have 0 <= index <= count
    ଯଦି (0 <= index && index <= count && ensureCapacity(count + 1, status)) {
        ପାଇଁ (int32_t i=count; i>index; --i) {
            elements[i] = elements[i-1];
        }
        elements[index] = elem;
        ++count;
    }
    /* else index out of range */
}

ଶୂନ୍ୟ UVector64::removeAllElements() {
    count = 0;
}

UBool UVector64::expandCapacity(int32_t minimumCapacity, UErrorCode &status) {
    ଯଦି (U_FAILURE(status)) {
        ଫେରନ୍ତୁ ମିଥ୍ୟା;
    }
    ଯଦି (minimumCapacity < 0) {
        status = U_ILLEGAL_ARGUMENT_ERROR;
        ଫେରନ୍ତୁ ମିଥ୍ୟା;
    }
    ଯଦି (capacity >= minimumCapacity) {
        ଫେରନ୍ତୁ ସତ୍ୟ;
    }
    ଯଦି (maxCapacity>0 && minimumCapacity>maxCapacity) {
        status = U_BUFFER_OVERFLOW_ERROR;
        ଫେରନ୍ତୁ ମିଥ୍ୟା;
    }
    ଯଦି (capacity > (INT32_MAX - 1) / 2) {  // integer overflow check
        status = U_ILLEGAL_ARGUMENT_ERROR;
        ଫେରନ୍ତୁ ମିଥ୍ୟା;
    }
    int32_t newCap = capacity * 2;
    ଯଦି (newCap < minimumCapacity) {
        newCap = minimumCapacity;
    }
    ଯଦି (maxCapacity > 0 && newCap > maxCapacity) {
        newCap = maxCapacity;
    }
    ଯଦି (newCap > static_cast<int32_t>(INT32_MAX / ଆକାର(int64_t))) { // integer overflow check
        // We keep the original memory contents on bad minimumCapacity/maxCapacity.
        status = U_ILLEGAL_ARGUMENT_ERROR;
        ଫେରନ୍ତୁ ମିଥ୍ୟା;
    }
    int64_t* newElems = static_cast<int64_t*>(uprv_realloc(elements, ଆକାର(int64_t) * newCap));
    ଯଦି (newElems == nullptr) {
        // We keep the original contents on the memory failure on realloc.
        status = U_MEMORY_ALLOCATION_ERROR;
        ଫେରନ୍ତୁ ମିଥ୍ୟା;
    }
    elements = newElems;
    capacity = newCap;
    ଫେରନ୍ତୁ ସତ୍ୟ;
}

ଶୂନ୍ୟ UVector64::setMaxCapacity(int32_t limit) {
    U_ASSERT(limit >= 0);
    ଯଦି (limit < 0) {
        limit = 0;
    }
    ଯଦି (limit > static_cast<int32_t>(INT32_MAX / ଆକାର(int64_t))) { // integer overflow check for realloc
        //  Something is very wrong, don't realloc, leave capacity and maxCapacity unchanged
        ଫେରନ୍ତୁ;
    }
    maxCapacity = limit;
    ଯଦି (capacity <= maxCapacity || maxCapacity == 0) {
        // Current capacity is within the new limit.
        ଫେରନ୍ତୁ;
    }
    
    // New maximum capacity is smaller than the current size.
    // Realloc the storage to the new, smaller size.
    int64_t* newElems = static_cast<int64_t*>(uprv_realloc(elements, ଆକାର(int64_t) * maxCapacity));
    ଯଦି (newElems == nullptr) {
        // Realloc to smaller failed.
        //   Just keep what we had.  No need to call it a failure.
        ଫେରନ୍ତୁ;
    }
    elements = newElems;
    capacity = maxCapacity;
    ଯଦି (count > capacity) {
        count = capacity;
    }
}

/**
 * Change the size of this vector as follows: If newSize is smaller,
 * then truncate the array, possibly deleting held elements for i >=
 * newSize.  If newSize is larger, grow the array, filling in new
 * slots with nullptr.
 */
ଶୂନ୍ୟ UVector64::setSize(int32_t newSize) {
    int32_t i;
    ଯଦି (newSize < 0) {
        ଫେରନ୍ତୁ;
    }
    ଯଦି (newSize > count) {
        UErrorCode ec = U_ZERO_ERROR;
        ଯଦି (!ensureCapacity(newSize, ec)) {
            ଫେରନ୍ତୁ;
        }
        ପାଇଁ (i=count; i<newSize; ++i) {
            elements[i] = 0;
        }
    } 
    count = newSize;
}

U_NAMESPACE_END

