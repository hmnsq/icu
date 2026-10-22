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

#अन्तर्भाव "uvectr32.h"
#अन्तर्भाव "cmemory.h"
#अन्तर्भाव "putilimp.h"

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



शून्य UVector32::_init(int32_t initialCapacity, UErrorCode &status) {
    // Fix bogus initialCapacity values; avoid malloc(0)
    सचे (initialCapacity < 1) {
        initialCapacity = DEFAULT_CAPACITY;
    }
    सचे (maxCapacity>0 && maxCapacity<initialCapacity) {
        initialCapacity = maxCapacity;
    }
    सचे (initialCapacity > static_cast<int32_t>(INT32_MAX / परिमाण(int32_t))) {
        initialCapacity = uprv_min(DEFAULT_CAPACITY, maxCapacity);
    }
    elements = static_cast<int32_t*>(uprv_malloc(परिमाण(int32_t) * initialCapacity));
    सचे (elements == nullptr) {
        status = U_MEMORY_ALLOCATION_ERROR;
    } अञ्ञथा {
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
शून्य UVector32::assign(ध्रुव UVector32& other, UErrorCode &ec) {
    सचे (ensureCapacity(other.count, ec)) {
        setSize(other.count);
        अत्थाय (int32_t i=0; i<other.count; ++i) {
            elements[i] = other.elements[i];
        }
    }
}


तार्किक UVector32::operator==(ध्रुव UVector32& other) ध्रुव {
    int32_t i;
    सचे (count != other.count) पटिनिवत्त मिच्छा;
    अत्थाय (i=0; i<count; ++i) {
        सचे (elements[i] != other.elements[i]) {
            पटिनिवत्त मिच्छा;
        }
    }
    पटिनिवत्त सच्च;
}


शून्य UVector32::setElementAt(int32_t elem, int32_t index) {
    सचे (0 <= index && index < count) {
        elements[index] = elem;
    }
    /* else index out of range */
}

शून्य UVector32::निवेशय_Element_At(int32_t elem, int32_t index, UErrorCode &status) {
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

UBool UVector32::containsAll(ध्रुव UVector32& other) ध्रुव {
    अत्थाय (int32_t i=0; i<other.size(); ++i) {
        सचे (indexOf(other.elements[i]) < 0) {
            पटिनिवत्त मिच्छा;
        }
    }
    पटिनिवत्त सच्च;
}

UBool UVector32::containsNone(ध्रुव UVector32& other) ध्रुव {
    अत्थाय (int32_t i=0; i<other.size(); ++i) {
        सचे (indexOf(other.elements[i]) >= 0) {
            पटिनिवत्त मिच्छा;
        }
    }
    पटिनिवत्त सच्च;
}

UBool UVector32::removeAll(ध्रुव UVector32& other) {
    UBool changed = मिच्छा;
    अत्थाय (int32_t i=0; i<other.size(); ++i) {
        int32_t j = indexOf(other.elements[i]);
        सचे (j >= 0) {
            removeElementAt(j);
            changed = सच्च;
        }
    }
    पटिनिवत्त changed;
}

UBool UVector32::retainAll(ध्रुव UVector32& other) {
    UBool changed = मिच्छा;
    अत्थाय (int32_t j=size()-1; j>=0; --j) {
        int32_t i = other.indexOf(elements[j]);
        सचे (i < 0) {
            removeElementAt(j);
            changed = सच्च;
        }
    }
    पटिनिवत्त changed;
}

शून्य UVector32::removeElementAt(int32_t index) {
    सचे (index >= 0) {
        अत्थाय (int32_t i=index; i<count-1; ++i) {
            elements[i] = elements[i+1];
        }
        --count;
    }
}

शून्य UVector32::removeAllElements() {
    count = 0;
}

UBool   UVector32::equals(ध्रुव UVector32 &other) ध्रुव {
    पूर्णाङ्क      i;

    सचे (एतत्->count != other.count) {
        पटिनिवत्त मिच्छा;
    }
    अत्थाय (i=0; i<count; i++) {
        सचे (elements[i] != other.elements[i]) {
            पटिनिवत्त मिच्छा;
        }
    }
    पटिनिवत्त सच्च;
}




int32_t UVector32::indexOf(int32_t कुञ्चिका, int32_t startIndex) ध्रुव {
    int32_t i;
    अत्थाय (i=startIndex; i<count; ++i) {
        सचे (कुञ्चिका == elements[i]) {
            पटिनिवत्त i;
        }
    }
    पटिनिवत्त -1;
}


UBool UVector32::expandCapacity(int32_t minimumCapacity, UErrorCode &status) {
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
    सचे (newCap > static_cast<int32_t>(INT32_MAX / परिमाण(int32_t))) { // integer overflow check
        // We keep the original memory contents on bad minimumCapacity/maxCapacity.
        status = U_ILLEGAL_ARGUMENT_ERROR;
        पटिनिवत्त मिच्छा;
    }
    int32_t* newElems = static_cast<int32_t*>(uprv_realloc(elements, परिमाण(int32_t) * newCap));
    सचे (newElems == nullptr) {
        // We keep the original contents on the memory failure on realloc.
        status = U_MEMORY_ALLOCATION_ERROR;
        पटिनिवत्त मिच्छा;
    }
    elements = newElems;
    capacity = newCap;
    पटिनिवत्त सच्च;
}

शून्य UVector32::setMaxCapacity(int32_t limit) {
    U_ASSERT(limit >= 0);
    सचे (limit < 0) {
        limit = 0;
    }
    सचे (limit > static_cast<int32_t>(INT32_MAX / परिमाण(int32_t))) { // integer overflow check for realloc
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
    int32_t* newElems = static_cast<int32_t*>(uprv_realloc(elements, परिमाण(int32_t) * maxCapacity));
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
शून्य UVector32::setSize(int32_t newSize) {
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




/**
 * Insert the given integer into this vector at its sorted position
 * as defined by 'compare'.  The current elements are assumed to
 * be sorted already.
 */
शून्य UVector32::sorted_निवेशय(int32_t tok, UErrorCode& ec) {
    // Perform a binary search for the location to insert tok at.  Tok
    // will be inserted between two elements a and b such that a <=
    // tok && tok < b, where there is a 'virtual' elements[-1] always
    // less than tok and a 'virtual' elements[count] always greater
    // than tok.
    int32_t min = 0, max = count;
    याव (min != max) {
        int32_t probe = (min + max) / 2;
        //int8_t c = (*compare)(elements[probe], tok);
        //if (c > 0) {
        सचे (elements[probe] > tok) {
            max = probe;
        } अञ्ञथा {
            // assert(c <= 0);
            min = probe + 1;
        }
    }
    सचे (ensureCapacity(count + 1, ec)) {
        अत्थाय (int32_t i=count; i>min; --i) {
            elements[i] = elements[i-1];
        }
        elements[min] = tok;
        ++count;
    }
}





U_NAMESPACE_END

