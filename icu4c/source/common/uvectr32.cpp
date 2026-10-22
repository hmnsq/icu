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
    जइ (initialCapacity < 1) {
        initialCapacity = DEFAULT_CAPACITY;
    }
    जइ (maxCapacity>0 && maxCapacity<initialCapacity) {
        initialCapacity = maxCapacity;
    }
    जइ (initialCapacity > static_cast<int32_t>(INT32_MAX / परिमाण(int32_t))) {
        initialCapacity = uprv_min(DEFAULT_CAPACITY, maxCapacity);
    }
    elements = static_cast<int32_t*>(uprv_malloc(परिमाण(int32_t) * initialCapacity));
    जइ (elements == nullptr) {
        status = U_MEMORY_ALLOCATION_ERROR;
    } अण्णहा {
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
    जइ (ensureCapacity(other.count, ec)) {
        setSize(other.count);
        प्रति (int32_t i=0; i<other.count; ++i) {
            elements[i] = other.elements[i];
        }
    }
}


तार्किक UVector32::operator==(ध्रुव UVector32& other) ध्रुव {
    int32_t i;
    जइ (count != other.count) प्रत्यागम मिच्छ;
    प्रति (i=0; i<count; ++i) {
        जइ (elements[i] != other.elements[i]) {
            प्रत्यागम मिच्छ;
        }
    }
    प्रत्यागम सच्च;
}


शून्य UVector32::setElementAt(int32_t elem, int32_t index) {
    जइ (0 <= index && index < count) {
        elements[index] = elem;
    }
    /* else index out of range */
}

शून्य UVector32::निवेशय_Element_At(int32_t elem, int32_t index, UErrorCode &status) {
    // must have 0 <= index <= count
    जइ (0 <= index && index <= count && ensureCapacity(count + 1, status)) {
        प्रति (int32_t i=count; i>index; --i) {
            elements[i] = elements[i-1];
        }
        elements[index] = elem;
        ++count;
    }
    /* else index out of range */
}

UBool UVector32::containsAll(ध्रुव UVector32& other) ध्रुव {
    प्रति (int32_t i=0; i<other.size(); ++i) {
        जइ (indexOf(other.elements[i]) < 0) {
            प्रत्यागम मिच्छ;
        }
    }
    प्रत्यागम सच्च;
}

UBool UVector32::containsNone(ध्रुव UVector32& other) ध्रुव {
    प्रति (int32_t i=0; i<other.size(); ++i) {
        जइ (indexOf(other.elements[i]) >= 0) {
            प्रत्यागम मिच्छ;
        }
    }
    प्रत्यागम सच्च;
}

UBool UVector32::removeAll(ध्रुव UVector32& other) {
    UBool changed = मिच्छ;
    प्रति (int32_t i=0; i<other.size(); ++i) {
        int32_t j = indexOf(other.elements[i]);
        जइ (j >= 0) {
            removeElementAt(j);
            changed = सच्च;
        }
    }
    प्रत्यागम changed;
}

UBool UVector32::retainAll(ध्रुव UVector32& other) {
    UBool changed = मिच्छ;
    प्रति (int32_t j=size()-1; j>=0; --j) {
        int32_t i = other.indexOf(elements[j]);
        जइ (i < 0) {
            removeElementAt(j);
            changed = सच्च;
        }
    }
    प्रत्यागम changed;
}

शून्य UVector32::removeElementAt(int32_t index) {
    जइ (index >= 0) {
        प्रति (int32_t i=index; i<count-1; ++i) {
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

    जइ (एतत्->count != other.count) {
        प्रत्यागम मिच्छ;
    }
    प्रति (i=0; i<count; i++) {
        जइ (elements[i] != other.elements[i]) {
            प्रत्यागम मिच्छ;
        }
    }
    प्रत्यागम सच्च;
}




int32_t UVector32::indexOf(int32_t कुञ्चिका, int32_t startIndex) ध्रुव {
    int32_t i;
    प्रति (i=startIndex; i<count; ++i) {
        जइ (कुञ्चिका == elements[i]) {
            प्रत्यागम i;
        }
    }
    प्रत्यागम -1;
}


UBool UVector32::expandCapacity(int32_t minimumCapacity, UErrorCode &status) {
    जइ (U_FAILURE(status)) {
        प्रत्यागम मिच्छ;
    }
    जइ (minimumCapacity < 0) {
        status = U_ILLEGAL_ARGUMENT_ERROR;
        प्रत्यागम मिच्छ;
    }
    जइ (capacity >= minimumCapacity) {
        प्रत्यागम सच्च;
    }
    जइ (maxCapacity>0 && minimumCapacity>maxCapacity) {
        status = U_BUFFER_OVERFLOW_ERROR;
        प्रत्यागम मिच्छ;
    }
    जइ (capacity > (INT32_MAX - 1) / 2) {  // integer overflow check
        status = U_ILLEGAL_ARGUMENT_ERROR;
        प्रत्यागम मिच्छ;
    }
    int32_t newCap = capacity * 2;
    जइ (newCap < minimumCapacity) {
        newCap = minimumCapacity;
    }
    जइ (maxCapacity > 0 && newCap > maxCapacity) {
        newCap = maxCapacity;
    }
    जइ (newCap > static_cast<int32_t>(INT32_MAX / परिमाण(int32_t))) { // integer overflow check
        // We keep the original memory contents on bad minimumCapacity/maxCapacity.
        status = U_ILLEGAL_ARGUMENT_ERROR;
        प्रत्यागम मिच्छ;
    }
    int32_t* newElems = static_cast<int32_t*>(uprv_realloc(elements, परिमाण(int32_t) * newCap));
    जइ (newElems == nullptr) {
        // We keep the original contents on the memory failure on realloc.
        status = U_MEMORY_ALLOCATION_ERROR;
        प्रत्यागम मिच्छ;
    }
    elements = newElems;
    capacity = newCap;
    प्रत्यागम सच्च;
}

शून्य UVector32::setMaxCapacity(int32_t limit) {
    U_ASSERT(limit >= 0);
    जइ (limit < 0) {
        limit = 0;
    }
    जइ (limit > static_cast<int32_t>(INT32_MAX / परिमाण(int32_t))) { // integer overflow check for realloc
        //  Something is very wrong, don't realloc, leave capacity and maxCapacity unchanged
        प्रत्यागम;
    }
    maxCapacity = limit;
    जइ (capacity <= maxCapacity || maxCapacity == 0) {
        // Current capacity is within the new limit.
        प्रत्यागम;
    }
    
    // New maximum capacity is smaller than the current size.
    // Realloc the storage to the new, smaller size.
    int32_t* newElems = static_cast<int32_t*>(uprv_realloc(elements, परिमाण(int32_t) * maxCapacity));
    जइ (newElems == nullptr) {
        // Realloc to smaller failed.
        //   Just keep what we had.  No need to call it a failure.
        प्रत्यागम;
    }
    elements = newElems;
    capacity = maxCapacity;
    जइ (count > capacity) {
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
    जइ (newSize < 0) {
        प्रत्यागम;
    }
    जइ (newSize > count) {
        UErrorCode ec = U_ZERO_ERROR;
        जइ (!ensureCapacity(newSize, ec)) {
            प्रत्यागम;
        }
        प्रति (i=count; i<newSize; ++i) {
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
    जाव (min != max) {
        int32_t probe = (min + max) / 2;
        //int8_t c = (*compare)(elements[probe], tok);
        //if (c > 0) {
        जइ (elements[probe] > tok) {
            max = probe;
        } अण्णहा {
            // assert(c <= 0);
            min = probe + 1;
        }
    }
    जइ (ensureCapacity(count + 1, ec)) {
        प्रति (int32_t i=count; i>min; --i) {
            elements[i] = elements[i-1];
        }
        elements[min] = tok;
        ++count;
    }
}





U_NAMESPACE_END

