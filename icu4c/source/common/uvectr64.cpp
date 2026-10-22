// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
******************************************************************************
* Copyright (C) 1999-2015, International Business Machines Corporation and
* others. All Rights Reserved.
******************************************************************************
*/

#অন্তর্ভাৱ "uvectr64.h"
#অন্তর্ভাৱ "cmemory.h"
#অন্তর্ভাৱ "putilimp.h"

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



শূন্য UVector64::_init(int32_t initialCapacity, UErrorCode &status) {
    // Fix bogus initialCapacity values; avoid malloc(0)
    যদি (initialCapacity < 1) {
        initialCapacity = DEFAULT_CAPACITY;
    }
    যদি (maxCapacity>0 && maxCapacity<initialCapacity) {
        initialCapacity = maxCapacity;
    }
    যদি (initialCapacity > static_cast<int32_t>(INT32_MAX / পরিমাণ(int64_t))) {
        initialCapacity = uprv_min(DEFAULT_CAPACITY, maxCapacity);
    }
    elements = static_cast<int64_t*>(uprv_malloc(পরিমাণ(int64_t) * initialCapacity));
    যদি (elements == nullptr) {
        status = U_MEMORY_ALLOCATION_ERROR;
    } অন্যথা {
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
শূন্য UVector64::assign(ধ্রুৱ UVector64& other, UErrorCode &ec) {
    যদি (ensureCapacity(other.count, ec)) {
        setSize(other.count);
        বাবে (int32_t i=0; i<other.count; ++i) {
            elements[i] = other.elements[i];
        }
    }
}


তার্কিক UVector64::operator==(ধ্রুৱ UVector64& other) {
    int32_t i;
    যদি (count != other.count) ঘূৰাই মিছা;
    বাবে (i=0; i<count; ++i) {
        যদি (elements[i] != other.elements[i]) {
            ঘূৰাই মিছা;
        }
    }
    ঘূৰাই সঁচা;
}


শূন্য UVector64::setElementAt(int64_t elem, int32_t index) {
    যদি (0 <= index && index < count) {
        elements[index] = elem;
    }
    /* else index out of range */
}

শূন্য UVector64::ঢোকাও_Element_At(int64_t elem, int32_t index, UErrorCode &status) {
    // must have 0 <= index <= count
    যদি (0 <= index && index <= count && ensureCapacity(count + 1, status)) {
        বাবে (int32_t i=count; i>index; --i) {
            elements[i] = elements[i-1];
        }
        elements[index] = elem;
        ++count;
    }
    /* else index out of range */
}

শূন্য UVector64::removeAllElements() {
    count = 0;
}

UBool UVector64::expandCapacity(int32_t minimumCapacity, UErrorCode &status) {
    যদি (U_FAILURE(status)) {
        ঘূৰাই মিছা;
    }
    যদি (minimumCapacity < 0) {
        status = U_ILLEGAL_ARGUMENT_ERROR;
        ঘূৰাই মিছা;
    }
    যদি (capacity >= minimumCapacity) {
        ঘূৰাই সঁচা;
    }
    যদি (maxCapacity>0 && minimumCapacity>maxCapacity) {
        status = U_BUFFER_OVERFLOW_ERROR;
        ঘূৰাই মিছা;
    }
    যদি (capacity > (INT32_MAX - 1) / 2) {  // integer overflow check
        status = U_ILLEGAL_ARGUMENT_ERROR;
        ঘূৰাই মিছা;
    }
    int32_t newCap = capacity * 2;
    যদি (newCap < minimumCapacity) {
        newCap = minimumCapacity;
    }
    যদি (maxCapacity > 0 && newCap > maxCapacity) {
        newCap = maxCapacity;
    }
    যদি (newCap > static_cast<int32_t>(INT32_MAX / পরিমাণ(int64_t))) { // integer overflow check
        // We keep the original memory contents on bad minimumCapacity/maxCapacity.
        status = U_ILLEGAL_ARGUMENT_ERROR;
        ঘূৰাই মিছা;
    }
    int64_t* newElems = static_cast<int64_t*>(uprv_realloc(elements, পরিমাণ(int64_t) * newCap));
    যদি (newElems == nullptr) {
        // We keep the original contents on the memory failure on realloc.
        status = U_MEMORY_ALLOCATION_ERROR;
        ঘূৰাই মিছা;
    }
    elements = newElems;
    capacity = newCap;
    ঘূৰাই সঁচা;
}

শূন্য UVector64::setMaxCapacity(int32_t limit) {
    U_ASSERT(limit >= 0);
    যদি (limit < 0) {
        limit = 0;
    }
    যদি (limit > static_cast<int32_t>(INT32_MAX / পরিমাণ(int64_t))) { // integer overflow check for realloc
        //  Something is very wrong, don't realloc, leave capacity and maxCapacity unchanged
        ঘূৰাই;
    }
    maxCapacity = limit;
    যদি (capacity <= maxCapacity || maxCapacity == 0) {
        // Current capacity is within the new limit.
        ঘূৰাই;
    }
    
    // New maximum capacity is smaller than the current size.
    // Realloc the storage to the new, smaller size.
    int64_t* newElems = static_cast<int64_t*>(uprv_realloc(elements, পরিমাণ(int64_t) * maxCapacity));
    যদি (newElems == nullptr) {
        // Realloc to smaller failed.
        //   Just keep what we had.  No need to call it a failure.
        ঘূৰাই;
    }
    elements = newElems;
    capacity = maxCapacity;
    যদি (count > capacity) {
        count = capacity;
    }
}

/**
 * Change the size of this vector as follows: If newSize is smaller,
 * then truncate the array, possibly deleting held elements for i >=
 * newSize.  If newSize is larger, grow the array, filling in new
 * slots with nullptr.
 */
শূন্য UVector64::setSize(int32_t newSize) {
    int32_t i;
    যদি (newSize < 0) {
        ঘূৰাই;
    }
    যদি (newSize > count) {
        UErrorCode ec = U_ZERO_ERROR;
        যদি (!ensureCapacity(newSize, ec)) {
            ঘূৰাই;
        }
        বাবে (i=count; i<newSize; ++i) {
            elements[i] = 0;
        }
    } 
    count = newSize;
}

U_NAMESPACE_END

