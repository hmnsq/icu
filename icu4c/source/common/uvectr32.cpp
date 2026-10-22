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
    གལ་ཏེ (initialCapacity < 1) {
        initialCapacity = DEFAULT_CAPACITY;
    }
    གལ་ཏེ (maxCapacity>0 && maxCapacity<initialCapacity) {
        initialCapacity = maxCapacity;
    }
    གལ་ཏེ (initialCapacity > static_cast<int32_t>(INT32_MAX / parimaa_na(int32_t))) {
        initialCapacity = uprv_min(DEFAULT_CAPACITY, maxCapacity);
    }
    elements = static_cast<int32_t*>(uprv_malloc(parimaa_na(int32_t) * initialCapacity));
    གལ་ཏེ (elements == nullptr) {
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
    གལ་ཏེ (ensureCapacity(other.count, ec)) {
        setSize(other.count);
        ཆེད་དུ (int32_t i=0; i<other.count; ++i) {
            elements[i] = other.elements[i];
        }
    }
}


bool UVector32::operator==(const UVector32& other) const {
    int32_t i;
    གལ་ཏེ (count != other.count) ཕྱིར false;
    ཆེད་དུ (i=0; i<count; ++i) {
        གལ་ཏེ (elements[i] != other.elements[i]) {
            ཕྱིར false;
        }
    }
    ཕྱིར true;
}


void UVector32::setElementAt(int32_t elem, int32_t index) {
    གལ་ཏེ (0 <= index && index < count) {
        elements[index] = elem;
    }
    /* else index out of range */
}

void UVector32::insertElementAt(int32_t elem, int32_t index, UErrorCode &status) {
    // must have 0 <= index <= count
    གལ་ཏེ (0 <= index && index <= count && ensureCapacity(count + 1, status)) {
        ཆེད་དུ (int32_t i=count; i>index; --i) {
            elements[i] = elements[i-1];
        }
        elements[index] = elem;
        ++count;
    }
    /* else index out of range */
}

UBool UVector32::containsAll(const UVector32& other) const {
    ཆེད་དུ (int32_t i=0; i<other.size(); ++i) {
        གལ་ཏེ (indexOf(other.elements[i]) < 0) {
            ཕྱིར false;
        }
    }
    ཕྱིར true;
}

UBool UVector32::containsNone(const UVector32& other) const {
    ཆེད་དུ (int32_t i=0; i<other.size(); ++i) {
        གལ་ཏེ (indexOf(other.elements[i]) >= 0) {
            ཕྱིར false;
        }
    }
    ཕྱིར true;
}

UBool UVector32::removeAll(const UVector32& other) {
    UBool changed = false;
    ཆེད་དུ (int32_t i=0; i<other.size(); ++i) {
        int32_t j = indexOf(other.elements[i]);
        གལ་ཏེ (j >= 0) {
            removeElementAt(j);
            changed = true;
        }
    }
    ཕྱིར changed;
}

UBool UVector32::retainAll(const UVector32& other) {
    UBool changed = false;
    ཆེད་དུ (int32_t j=size()-1; j>=0; --j) {
        int32_t i = other.indexOf(elements[j]);
        གལ་ཏེ (i < 0) {
            removeElementAt(j);
            changed = true;
        }
    }
    ཕྱིར changed;
}

void UVector32::removeElementAt(int32_t index) {
    གལ་ཏེ (index >= 0) {
        ཆེད་དུ (int32_t i=index; i<count-1; ++i) {
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

    གལ་ཏེ (this->count != other.count) {
        ཕྱིར false;
    }
    ཆེད་དུ (i=0; i<count; i++) {
        གལ་ཏེ (elements[i] != other.elements[i]) {
            ཕྱིར false;
        }
    }
    ཕྱིར true;
}




int32_t UVector32::indexOf(int32_t key, int32_t startIndex) const {
    int32_t i;
    ཆེད་དུ (i=startIndex; i<count; ++i) {
        གལ་ཏེ (key == elements[i]) {
            ཕྱིར i;
        }
    }
    ཕྱིར -1;
}


UBool UVector32::expandCapacity(int32_t minimumCapacity, UErrorCode &status) {
    གལ་ཏེ (U_FAILURE(status)) {
        ཕྱིར false;
    }
    གལ་ཏེ (minimumCapacity < 0) {
        status = U_ILLEGAL_ARGUMENT_ERROR;
        ཕྱིར false;
    }
    གལ་ཏེ (capacity >= minimumCapacity) {
        ཕྱིར true;
    }
    གལ་ཏེ (maxCapacity>0 && minimumCapacity>maxCapacity) {
        status = U_BUFFER_OVERFLOW_ERROR;
        ཕྱིར false;
    }
    གལ་ཏེ (capacity > (INT32_MAX - 1) / 2) {  // integer overflow check
        status = U_ILLEGAL_ARGUMENT_ERROR;
        ཕྱིར false;
    }
    int32_t newCap = capacity * 2;
    གལ་ཏེ (newCap < minimumCapacity) {
        newCap = minimumCapacity;
    }
    གལ་ཏེ (maxCapacity > 0 && newCap > maxCapacity) {
        newCap = maxCapacity;
    }
    གལ་ཏེ (newCap > static_cast<int32_t>(INT32_MAX / parimaa_na(int32_t))) { // integer overflow check
        // We keep the original memory contents on bad minimumCapacity/maxCapacity.
        status = U_ILLEGAL_ARGUMENT_ERROR;
        ཕྱིར false;
    }
    int32_t* newElems = static_cast<int32_t*>(uprv_realloc(elements, parimaa_na(int32_t) * newCap));
    གལ་ཏེ (newElems == nullptr) {
        // We keep the original contents on the memory failure on realloc.
        status = U_MEMORY_ALLOCATION_ERROR;
        ཕྱིར false;
    }
    elements = newElems;
    capacity = newCap;
    ཕྱིར true;
}

void UVector32::setMaxCapacity(int32_t limit) {
    U_ASSERT(limit >= 0);
    གལ་ཏེ (limit < 0) {
        limit = 0;
    }
    གལ་ཏེ (limit > static_cast<int32_t>(INT32_MAX / parimaa_na(int32_t))) { // integer overflow check for realloc
        //  Something is very wrong, don't realloc, leave capacity and maxCapacity unchanged
        ཕྱིར;
    }
    maxCapacity = limit;
    གལ་ཏེ (capacity <= maxCapacity || maxCapacity == 0) {
        // Current capacity is within the new limit.
        ཕྱིར;
    }
    
    // New maximum capacity is smaller than the current size.
    // Realloc the storage to the new, smaller size.
    int32_t* newElems = static_cast<int32_t*>(uprv_realloc(elements, parimaa_na(int32_t) * maxCapacity));
    གལ་ཏེ (newElems == nullptr) {
        // Realloc to smaller failed.
        //   Just keep what we had.  No need to call it a failure.
        ཕྱིར;
    }
    elements = newElems;
    capacity = maxCapacity;
    གལ་ཏེ (count > capacity) {
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
    གལ་ཏེ (newSize < 0) {
        ཕྱིར;
    }
    གལ་ཏེ (newSize > count) {
        UErrorCode ec = U_ZERO_ERROR;
        གལ་ཏེ (!ensureCapacity(newSize, ec)) {
            ཕྱིར;
        }
        ཆེད་དུ (i=count; i<newSize; ++i) {
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
        གལ་ཏེ (elements[probe] > tok) {
            max = probe;
        } དེ་མིན {
            // assert(c <= 0);
            min = probe + 1;
        }
    }
    གལ་ཏེ (ensureCapacity(count + 1, ec)) {
        ཆེད་དུ (int32_t i=count; i>min; --i) {
            elements[i] = elements[i-1];
        }
        elements[min] = tok;
        ++count;
    }
}





U_NAMESPACE_END

