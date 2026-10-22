// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
******************************************************************************
* Copyright (C) 1999-2015, International Business Machines Corporation and
* others. All Rights Reserved.
******************************************************************************
*   Date        Name        Description
*   10/22/99    alan        Creation.
**********************************************************************
*/

#include "uvectr32.h"
#include "cmemory.h"
#include "putilimp.h"

U_NAMESPACE_BEGIN

#define DEFAULT_CAPACITY 8

/*
 * Constants for hinting whether a key is an integer
 * or a pointer.  If a hint bit is zero, then the associated
 * token is assumed to be an integer. This is needed for iSeries
 */
 
UOBJECT_DEFINE_RTTI_IMPLEMENTATION(UVector32)

UVector32::UVector32(UErrorCode &status) :
    count(0),
    capacity(0),
    maxCapacity(0),
    elements(nullptr)
{
    _init(DEFAULT_CAPACITY, status);
}

UVector32::UVector32(int32_t initialCapacity, UErrorCode &status) :
    count(0),
    capacity(0),
    maxCapacity(0),
    elements(nullptr)
{
    _init(initialCapacity, status);
}



void UVector32::_init(int32_t initialCapacity, UErrorCode &status) {
    // Fix bogus initialCapacity values; avoid malloc(0)
    만약 (initialCapacity < 1) {
        initialCapacity = DEFAULT_CAPACITY;
    }
    만약 (maxCapacity>0 && maxCapacity<initialCapacity) {
        initialCapacity = maxCapacity;
    }
    만약 (initialCapacity > static_cast<int32_t>(INT32_MAX / sizeof(int32_t))) {
        initialCapacity = uprv_min(DEFAULT_CAPACITY, maxCapacity);
    }
    elements = static_cast<int32_t*>(uprv_malloc(sizeof(int32_t) * initialCapacity));
    만약 (elements == nullptr) {
        status = U_MEMORY_ALLOCATION_ERROR;
    } 아니면 {
        capacity = initialCapacity;
    }
}

UVector32::~UVector32() {
    uprv_free(elements);
    elements = nullptr;
}

/**
 * Assign this object to another (make this a copy of 'other').
 */
void UVector32::assign(const UVector32& other, UErrorCode &ec) {
    만약 (ensureCapacity(other.count, ec)) {
        setSize(other.count);
        위해 (int32_t i=0; i<other.count; ++i) {
            elements[i] = other.elements[i];
        }
    }
}


bool UVector32::operator==(const UVector32& other) const {
    int32_t i;
    만약 (count != other.count) 반환 false;
    위해 (i=0; i<count; ++i) {
        만약 (elements[i] != other.elements[i]) {
            반환 false;
        }
    }
    반환 true;
}


void UVector32::setElementAt(int32_t elem, int32_t index) {
    만약 (0 <= index && index < count) {
        elements[index] = elem;
    }
    /* else index out of range */
}

void UVector32::insertElementAt(int32_t elem, int32_t index, UErrorCode &status) {
    // must have 0 <= index <= count
    만약 (0 <= index && index <= count && ensureCapacity(count + 1, status)) {
        위해 (int32_t i=count; i>index; --i) {
            elements[i] = elements[i-1];
        }
        elements[index] = elem;
        ++count;
    }
    /* else index out of range */
}

UBool UVector32::containsAll(const UVector32& other) const {
    위해 (int32_t i=0; i<other.size(); ++i) {
        만약 (indexOf(other.elements[i]) < 0) {
            반환 false;
        }
    }
    반환 true;
}

UBool UVector32::containsNone(const UVector32& other) const {
    위해 (int32_t i=0; i<other.size(); ++i) {
        만약 (indexOf(other.elements[i]) >= 0) {
            반환 false;
        }
    }
    반환 true;
}

UBool UVector32::removeAll(const UVector32& other) {
    UBool changed = false;
    위해 (int32_t i=0; i<other.size(); ++i) {
        int32_t j = indexOf(other.elements[i]);
        만약 (j >= 0) {
            removeElementAt(j);
            changed = true;
        }
    }
    반환 changed;
}

UBool UVector32::retainAll(const UVector32& other) {
    UBool changed = false;
    위해 (int32_t j=size()-1; j>=0; --j) {
        int32_t i = other.indexOf(elements[j]);
        만약 (i < 0) {
            removeElementAt(j);
            changed = true;
        }
    }
    반환 changed;
}

void UVector32::removeElementAt(int32_t index) {
    만약 (index >= 0) {
        위해 (int32_t i=index; i<count-1; ++i) {
            elements[i] = elements[i+1];
        }
        --count;
    }
}

void UVector32::removeAllElements() {
    count = 0;
}

UBool   UVector32::equals(const UVector32 &other) const {
    정수      i;

    만약 (this->count != other.count) {
        반환 false;
    }
    위해 (i=0; i<count; i++) {
        만약 (elements[i] != other.elements[i]) {
            반환 false;
        }
    }
    반환 true;
}




int32_t UVector32::indexOf(int32_t key, int32_t startIndex) const {
    int32_t i;
    위해 (i=startIndex; i<count; ++i) {
        만약 (key == elements[i]) {
            반환 i;
        }
    }
    반환 -1;
}


UBool UVector32::expandCapacity(int32_t minimumCapacity, UErrorCode &status) {
    만약 (U_FAILURE(status)) {
        반환 false;
    }
    만약 (minimumCapacity < 0) {
        status = U_ILLEGAL_ARGUMENT_ERROR;
        반환 false;
    }
    만약 (capacity >= minimumCapacity) {
        반환 true;
    }
    만약 (maxCapacity>0 && minimumCapacity>maxCapacity) {
        status = U_BUFFER_OVERFLOW_ERROR;
        반환 false;
    }
    만약 (capacity > (INT32_MAX - 1) / 2) {  // integer overflow check
        status = U_ILLEGAL_ARGUMENT_ERROR;
        반환 false;
    }
    int32_t newCap = capacity * 2;
    만약 (newCap < minimumCapacity) {
        newCap = minimumCapacity;
    }
    만약 (maxCapacity > 0 && newCap > maxCapacity) {
        newCap = maxCapacity;
    }
    만약 (newCap > static_cast<int32_t>(INT32_MAX / sizeof(int32_t))) { // integer overflow check
        // We keep the original memory contents on bad minimumCapacity/maxCapacity.
        status = U_ILLEGAL_ARGUMENT_ERROR;
        반환 false;
    }
    int32_t* newElems = static_cast<int32_t*>(uprv_realloc(elements, sizeof(int32_t) * newCap));
    만약 (newElems == nullptr) {
        // We keep the original contents on the memory failure on realloc.
        status = U_MEMORY_ALLOCATION_ERROR;
        반환 false;
    }
    elements = newElems;
    capacity = newCap;
    반환 true;
}

void UVector32::setMaxCapacity(int32_t limit) {
    U_ASSERT(limit >= 0);
    만약 (limit < 0) {
        limit = 0;
    }
    만약 (limit > static_cast<int32_t>(INT32_MAX / sizeof(int32_t))) { // integer overflow check for realloc
        //  Something is very wrong, don't realloc, leave capacity and maxCapacity unchanged
        반환;
    }
    maxCapacity = limit;
    만약 (capacity <= maxCapacity || maxCapacity == 0) {
        // Current capacity is within the new limit.
        반환;
    }
    
    // New maximum capacity is smaller than the current size.
    // Realloc the storage to the new, smaller size.
    int32_t* newElems = static_cast<int32_t*>(uprv_realloc(elements, sizeof(int32_t) * maxCapacity));
    만약 (newElems == nullptr) {
        // Realloc to smaller failed.
        //   Just keep what we had.  No need to call it a failure.
        반환;
    }
    elements = newElems;
    capacity = maxCapacity;
    만약 (count > capacity) {
        count = capacity;
    }
}

/**
 * Change the size of this vector as follows: If newSize is smaller,
 * then truncate the array, possibly deleting held elements for i >=
 * newSize.  If newSize is larger, grow the array, filling in new
 * slots with nullptr.
 */
void UVector32::setSize(int32_t newSize) {
    int32_t i;
    만약 (newSize < 0) {
        반환;
    }
    만약 (newSize > count) {
        UErrorCode ec = U_ZERO_ERROR;
        만약 (!ensureCapacity(newSize, ec)) {
            반환;
        }
        위해 (i=count; i<newSize; ++i) {
            elements[i] = 0;
        }
    } 
    count = newSize;
}




/**
 * Insert the given integer into this vector at its sorted position
 * as defined by 'compare'.  The current elements are assumed to
 * be sorted already.
 */
void UVector32::sortedInsert(int32_t tok, UErrorCode& ec) {
    // Perform a binary search for the location to insert tok at.  Tok
    // will be inserted between two elements a and b such that a <=
    // tok && tok < b, where there is a 'virtual' elements[-1] always
    // less than tok and a 'virtual' elements[count] always greater
    // than tok.
    int32_t min = 0, max = count;
    동안 (min != max) {
        int32_t probe = (min + max) / 2;
        //int8_t c = (*compare)(elements[probe], tok);
        //if (c > 0) {
        만약 (elements[probe] > tok) {
            max = probe;
        } 아니면 {
            // assert(c <= 0);
            min = probe + 1;
        }
    }
    만약 (ensureCapacity(count + 1, ec)) {
        위해 (int32_t i=count; i>min; --i) {
            elements[i] = elements[i-1];
        }
        elements[min] = tok;
        ++count;
    }
}





U_NAMESPACE_END

