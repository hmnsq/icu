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
    གལ་སྲིད (initialCapacity < 1) {
        initialCapacity = DEFAULT_CAPACITY;
    }
    གལ་སྲིད (maxCapacity>0 && maxCapacity<initialCapacity) {
        initialCapacity = maxCapacity;
    }
    གལ་སྲིད (initialCapacity > static_cast<int32_t>(INT32_MAX / parimaa_na(int32_t))) {
        initialCapacity = uprv_min(DEFAULT_CAPACITY, maxCapacity);
    }
    elements = static_cast<int32_t*>(uprv_malloc(parimaa_na(int32_t) * initialCapacity));
    གལ་སྲིད (elements == nullptr) {
        status = U_MEMORY_ALLOCATION_ERROR;
    } དེ་མིན {
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
    གལ་སྲིད (ensureCapacity(other.count, ec)) {
        setSize(other.count);
        དོན་ལུ (int32_t i=0; i<other.count; ++i) {
            elements[i] = other.elements[i];
        }
    }
}


bool UVector32::operator==(const UVector32& other) const {
    int32_t i;
    གལ་སྲིད (count != other.count) ལོག false;
    དོན་ལུ (i=0; i<count; ++i) {
        གལ་སྲིད (elements[i] != other.elements[i]) {
            ལོག false;
        }
    }
    ལོག true;
}


void UVector32::setElementAt(int32_t elem, int32_t index) {
    གལ་སྲིད (0 <= index && index < count) {
        elements[index] = elem;
    }
    /* else index out of range */
}

void UVector32::insertElementAt(int32_t elem, int32_t index, UErrorCode &status) {
    // must have 0 <= index <= count
    གལ་སྲིད (0 <= index && index <= count && ensureCapacity(count + 1, status)) {
        དོན་ལུ (int32_t i=count; i>index; --i) {
            elements[i] = elements[i-1];
        }
        elements[index] = elem;
        ++count;
    }
    /* else index out of range */
}

UBool UVector32::containsAll(const UVector32& other) const {
    དོན་ལུ (int32_t i=0; i<other.size(); ++i) {
        གལ་སྲིད (indexOf(other.elements[i]) < 0) {
            ལོག false;
        }
    }
    ལོག true;
}

UBool UVector32::containsNone(const UVector32& other) const {
    དོན་ལུ (int32_t i=0; i<other.size(); ++i) {
        གལ་སྲིད (indexOf(other.elements[i]) >= 0) {
            ལོག false;
        }
    }
    ལོག true;
}

UBool UVector32::removeAll(const UVector32& other) {
    UBool changed = false;
    དོན་ལུ (int32_t i=0; i<other.size(); ++i) {
        int32_t j = indexOf(other.elements[i]);
        གལ་སྲིད (j >= 0) {
            removeElementAt(j);
            changed = true;
        }
    }
    ལོག changed;
}

UBool UVector32::retainAll(const UVector32& other) {
    UBool changed = false;
    དོན་ལུ (int32_t j=size()-1; j>=0; --j) {
        int32_t i = other.indexOf(elements[j]);
        གལ་སྲིད (i < 0) {
            removeElementAt(j);
            changed = true;
        }
    }
    ལོག changed;
}

void UVector32::removeElementAt(int32_t index) {
    གལ་སྲིད (index >= 0) {
        དོན་ལུ (int32_t i=index; i<count-1; ++i) {
            elements[i] = elements[i+1];
        }
        --count;
    }
}

void UVector32::removeAllElements() {
    count = 0;
}

UBool   UVector32::equals(const UVector32 &other) const {
    ཧྲིལ      i;

    གལ་སྲིད (this->count != other.count) {
        ལོག false;
    }
    དོན་ལུ (i=0; i<count; i++) {
        གལ་སྲིད (elements[i] != other.elements[i]) {
            ལོག false;
        }
    }
    ལོག true;
}




int32_t UVector32::indexOf(int32_t key, int32_t startIndex) const {
    int32_t i;
    དོན་ལུ (i=startIndex; i<count; ++i) {
        གལ་སྲིད (key == elements[i]) {
            ལོག i;
        }
    }
    ལོག -1;
}


UBool UVector32::expandCapacity(int32_t minimumCapacity, UErrorCode &status) {
    གལ་སྲིད (U_FAILURE(status)) {
        ལོག false;
    }
    གལ་སྲིད (minimumCapacity < 0) {
        status = U_ILLEGAL_ARGUMENT_ERROR;
        ལོག false;
    }
    གལ་སྲིད (capacity >= minimumCapacity) {
        ལོག true;
    }
    གལ་སྲིད (maxCapacity>0 && minimumCapacity>maxCapacity) {
        status = U_BUFFER_OVERFLOW_ERROR;
        ལོག false;
    }
    གལ་སྲིད (capacity > (INT32_MAX - 1) / 2) {  // integer overflow check
        status = U_ILLEGAL_ARGUMENT_ERROR;
        ལོག false;
    }
    int32_t newCap = capacity * 2;
    གལ་སྲིད (newCap < minimumCapacity) {
        newCap = minimumCapacity;
    }
    གལ་སྲིད (maxCapacity > 0 && newCap > maxCapacity) {
        newCap = maxCapacity;
    }
    གལ་སྲིད (newCap > static_cast<int32_t>(INT32_MAX / parimaa_na(int32_t))) { // integer overflow check
        // We keep the original memory contents on bad minimumCapacity/maxCapacity.
        status = U_ILLEGAL_ARGUMENT_ERROR;
        ལོག false;
    }
    int32_t* newElems = static_cast<int32_t*>(uprv_realloc(elements, parimaa_na(int32_t) * newCap));
    གལ་སྲིད (newElems == nullptr) {
        // We keep the original contents on the memory failure on realloc.
        status = U_MEMORY_ALLOCATION_ERROR;
        ལོག false;
    }
    elements = newElems;
    capacity = newCap;
    ལོག true;
}

void UVector32::setMaxCapacity(int32_t limit) {
    U_ASSERT(limit >= 0);
    གལ་སྲིད (limit < 0) {
        limit = 0;
    }
    གལ་སྲིད (limit > static_cast<int32_t>(INT32_MAX / parimaa_na(int32_t))) { // integer overflow check for realloc
        //  Something is very wrong, don't realloc, leave capacity and maxCapacity unchanged
        ལོག;
    }
    maxCapacity = limit;
    གལ་སྲིད (capacity <= maxCapacity || maxCapacity == 0) {
        // Current capacity is within the new limit.
        ལོག;
    }
    
    // New maximum capacity is smaller than the current size.
    // Realloc the storage to the new, smaller size.
    int32_t* newElems = static_cast<int32_t*>(uprv_realloc(elements, parimaa_na(int32_t) * maxCapacity));
    གལ་སྲིད (newElems == nullptr) {
        // Realloc to smaller failed.
        //   Just keep what we had.  No need to call it a failure.
        ལོག;
    }
    elements = newElems;
    capacity = maxCapacity;
    གལ་སྲིད (count > capacity) {
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
    གལ་སྲིད (newSize < 0) {
        ལོག;
    }
    གལ་སྲིད (newSize > count) {
        UErrorCode ec = U_ZERO_ERROR;
        གལ་སྲིད (!ensureCapacity(newSize, ec)) {
            ལོག;
        }
        དོན་ལུ (i=count; i<newSize; ++i) {
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
    སྐབས (min != max) {
        int32_t probe = (min + max) / 2;
        //int8_t c = (*compare)(elements[probe], tok);
        //if (c > 0) {
        གལ་སྲིད (elements[probe] > tok) {
            max = probe;
        } དེ་མིན {
            // assert(c <= 0);
            min = probe + 1;
        }
    }
    གལ་སྲིད (ensureCapacity(count + 1, ec)) {
        དོན་ལུ (int32_t i=count; i>min; --i) {
            elements[i] = elements[i-1];
        }
        elements[min] = tok;
        ++count;
    }
}





U_NAMESPACE_END

