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

#ضمن "uvectr32.h"
#ضمن "cmemory.h"
#ضمن "putilimp.h"

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



فراغ UVector32::_init(int32_t initialCapacity, UErrorCode &status) {
    // Fix bogus initialCapacity values; avoid malloc(0)
    إذا (initialCapacity < 1) {
        initialCapacity = DEFAULT_CAPACITY;
    }
    إذا (maxCapacity>0 && maxCapacity<initialCapacity) {
        initialCapacity = maxCapacity;
    }
    إذا (initialCapacity > static_cast<int32_t>(INT32_MAX / حجم(int32_t))) {
        initialCapacity = uprv_min(DEFAULT_CAPACITY, maxCapacity);
    }
    elements = static_cast<int32_t*>(uprv_malloc(حجم(int32_t) * initialCapacity));
    إذا (elements == nullptr) {
        status = U_MEMORY_ALLOCATION_ERROR;
    } وإلا {
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
فراغ UVector32::assign(ثابت UVector32& other, UErrorCode &ec) {
    إذا (ensureCapacity(other.count, ec)) {
        setSize(other.count);
        لكل (int32_t i=0; i<other.count; ++i) {
            elements[i] = other.elements[i];
        }
    }
}


منطقي UVector32::operator==(ثابت UVector32& other) ثابت {
    int32_t i;
    إذا (count != other.count) أرجع خطأ;
    لكل (i=0; i<count; ++i) {
        إذا (elements[i] != other.elements[i]) {
            أرجع خطأ;
        }
    }
    أرجع true;
}


فراغ UVector32::setElementAt(int32_t elem, int32_t index) {
    إذا (0 <= index && index < count) {
        elements[index] = elem;
    }
    /* else index out of range */
}

فراغ UVector32::قىستۇرۇش_Element_At(int32_t elem, int32_t index, UErrorCode &status) {
    // must have 0 <= index <= count
    إذا (0 <= index && index <= count && ensureCapacity(count + 1, status)) {
        لكل (int32_t i=count; i>index; --i) {
            elements[i] = elements[i-1];
        }
        elements[index] = elem;
        ++count;
    }
    /* else index out of range */
}

UBool UVector32::containsAll(ثابت UVector32& other) ثابت {
    لكل (int32_t i=0; i<other.size(); ++i) {
        إذا (indexOf(other.elements[i]) < 0) {
            أرجع خطأ;
        }
    }
    أرجع true;
}

UBool UVector32::containsNone(ثابت UVector32& other) ثابت {
    لكل (int32_t i=0; i<other.size(); ++i) {
        إذا (indexOf(other.elements[i]) >= 0) {
            أرجع خطأ;
        }
    }
    أرجع true;
}

UBool UVector32::removeAll(ثابت UVector32& other) {
    UBool changed = خطأ;
    لكل (int32_t i=0; i<other.size(); ++i) {
        int32_t j = indexOf(other.elements[i]);
        إذا (j >= 0) {
            removeElementAt(j);
            changed = true;
        }
    }
    أرجع changed;
}

UBool UVector32::retainAll(ثابت UVector32& other) {
    UBool changed = خطأ;
    لكل (int32_t j=size()-1; j>=0; --j) {
        int32_t i = other.indexOf(elements[j]);
        إذا (i < 0) {
            removeElementAt(j);
            changed = true;
        }
    }
    أرجع changed;
}

فراغ UVector32::removeElementAt(int32_t index) {
    إذا (index >= 0) {
        لكل (int32_t i=index; i<count-1; ++i) {
            elements[i] = elements[i+1];
        }
        --count;
    }
}

فراغ UVector32::removeAllElements() {
    count = 0;
}

UBool   UVector32::equals(ثابت UVector32 &other) ثابت {
    صحيح      i;

    إذا (هذا->count != other.count) {
        أرجع خطأ;
    }
    لكل (i=0; i<count; i++) {
        إذا (elements[i] != other.elements[i]) {
            أرجع خطأ;
        }
    }
    أرجع true;
}




int32_t UVector32::indexOf(int32_t ئاچقۇچ, int32_t startIndex) ثابت {
    int32_t i;
    لكل (i=startIndex; i<count; ++i) {
        إذا (ئاچقۇچ == elements[i]) {
            أرجع i;
        }
    }
    أرجع -1;
}


UBool UVector32::expandCapacity(int32_t minimumCapacity, UErrorCode &status) {
    إذا (U_FAILURE(status)) {
        أرجع خطأ;
    }
    إذا (minimumCapacity < 0) {
        status = U_ILLEGAL_ARGUMENT_ERROR;
        أرجع خطأ;
    }
    إذا (capacity >= minimumCapacity) {
        أرجع true;
    }
    إذا (maxCapacity>0 && minimumCapacity>maxCapacity) {
        status = U_BUFFER_OVERFLOW_ERROR;
        أرجع خطأ;
    }
    إذا (capacity > (INT32_MAX - 1) / 2) {  // integer overflow check
        status = U_ILLEGAL_ARGUMENT_ERROR;
        أرجع خطأ;
    }
    int32_t newCap = capacity * 2;
    إذا (newCap < minimumCapacity) {
        newCap = minimumCapacity;
    }
    إذا (maxCapacity > 0 && newCap > maxCapacity) {
        newCap = maxCapacity;
    }
    إذا (newCap > static_cast<int32_t>(INT32_MAX / حجم(int32_t))) { // integer overflow check
        // We keep the original memory contents on bad minimumCapacity/maxCapacity.
        status = U_ILLEGAL_ARGUMENT_ERROR;
        أرجع خطأ;
    }
    int32_t* newElems = static_cast<int32_t*>(uprv_realloc(elements, حجم(int32_t) * newCap));
    إذا (newElems == nullptr) {
        // We keep the original contents on the memory failure on realloc.
        status = U_MEMORY_ALLOCATION_ERROR;
        أرجع خطأ;
    }
    elements = newElems;
    capacity = newCap;
    أرجع true;
}

فراغ UVector32::setMaxCapacity(int32_t limit) {
    U_ASSERT(limit >= 0);
    إذا (limit < 0) {
        limit = 0;
    }
    إذا (limit > static_cast<int32_t>(INT32_MAX / حجم(int32_t))) { // integer overflow check for realloc
        //  Something is very wrong, don't realloc, leave capacity and maxCapacity unchanged
        أرجع;
    }
    maxCapacity = limit;
    إذا (capacity <= maxCapacity || maxCapacity == 0) {
        // Current capacity is within the new limit.
        أرجع;
    }
    
    // New maximum capacity is smaller than the current size.
    // Realloc the storage to the new, smaller size.
    int32_t* newElems = static_cast<int32_t*>(uprv_realloc(elements, حجم(int32_t) * maxCapacity));
    إذا (newElems == nullptr) {
        // Realloc to smaller failed.
        //   Just keep what we had.  No need to call it a failure.
        أرجع;
    }
    elements = newElems;
    capacity = maxCapacity;
    إذا (count > capacity) {
        count = capacity;
    }
}

/**
 * Change the size of this vector as follows: If newSize is smaller,
 * then truncate the array, possibly deleting held elements for i >=
 * newSize.  If newSize is larger, grow the array, filling in new
 * slots with nullptr.
 */
فراغ UVector32::setSize(int32_t newSize) {
    int32_t i;
    إذا (newSize < 0) {
        أرجع;
    }
    إذا (newSize > count) {
        UErrorCode ec = U_ZERO_ERROR;
        إذا (!ensureCapacity(newSize, ec)) {
            أرجع;
        }
        لكل (i=count; i<newSize; ++i) {
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
فراغ UVector32::sorted_قىستۇرۇش(int32_t tok, UErrorCode& ec) {
    // Perform a binary search for the location to insert tok at.  Tok
    // will be inserted between two elements a and b such that a <=
    // tok && tok < b, where there is a 'virtual' elements[-1] always
    // less than tok and a 'virtual' elements[count] always greater
    // than tok.
    int32_t min = 0, max = count;
    طالما (min != max) {
        int32_t probe = (min + max) / 2;
        //int8_t c = (*compare)(elements[probe], tok);
        //if (c > 0) {
        إذا (elements[probe] > tok) {
            max = probe;
        } وإلا {
            // assert(c <= 0);
            min = probe + 1;
        }
    }
    إذا (ensureCapacity(count + 1, ec)) {
        لكل (int32_t i=count; i>min; --i) {
            elements[i] = elements[i-1];
        }
        elements[min] = tok;
        ++count;
    }
}





U_NAMESPACE_END

