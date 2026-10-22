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
    𐡄𐡍 (initialCapacity < 1) {
        initialCapacity = DEFAULT_CAPACITY;
    }
    𐡄𐡍 (maxCapacity>0 && maxCapacity<initialCapacity) {
        initialCapacity = maxCapacity;
    }
    𐡄𐡍 (initialCapacity > static_cast<int32_t>(INT32_MAX / حجم(int32_t))) {
        initialCapacity = uprv_min(DEFAULT_CAPACITY, maxCapacity);
    }
    elements = static_cast<int32_t*>(uprv_malloc(حجم(int32_t) * initialCapacity));
    𐡄𐡍 (elements == nullptr) {
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
    𐡄𐡍 (ensureCapacity(other.count, ec)) {
        setSize(other.count);
        لكل (int32_t i=0; i<other.count; ++i) {
            elements[i] = other.elements[i];
        }
    }
}


منطقي UVector32::operator==(ثابت UVector32& other) ثابت {
    int32_t i;
    𐡄𐡍 (count != other.count) أرجع خطأ;
    لكل (i=0; i<count; ++i) {
        𐡄𐡍 (elements[i] != other.elements[i]) {
            أرجع خطأ;
        }
    }
    أرجع 𐡒𐡔𐡈;
}


فراغ UVector32::setElementAt(int32_t elem, int32_t index) {
    𐡄𐡍 (0 <= index && index < count) {
        elements[index] = elem;
    }
    /* else index out of range */
}

فراغ UVector32::insertElementAt(int32_t elem, int32_t index, UErrorCode &status) {
    // must have 0 <= index <= count
    𐡄𐡍 (0 <= index && index <= count && ensureCapacity(count + 1, status)) {
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
        𐡄𐡍 (indexOf(other.elements[i]) < 0) {
            أرجع خطأ;
        }
    }
    أرجع 𐡒𐡔𐡈;
}

UBool UVector32::containsNone(ثابت UVector32& other) ثابت {
    لكل (int32_t i=0; i<other.size(); ++i) {
        𐡄𐡍 (indexOf(other.elements[i]) >= 0) {
            أرجع خطأ;
        }
    }
    أرجع 𐡒𐡔𐡈;
}

UBool UVector32::removeAll(ثابت UVector32& other) {
    UBool changed = خطأ;
    لكل (int32_t i=0; i<other.size(); ++i) {
        int32_t j = indexOf(other.elements[i]);
        𐡄𐡍 (j >= 0) {
            removeElementAt(j);
            changed = 𐡒𐡔𐡈;
        }
    }
    أرجع changed;
}

UBool UVector32::retainAll(ثابت UVector32& other) {
    UBool changed = خطأ;
    لكل (int32_t j=size()-1; j>=0; --j) {
        int32_t i = other.indexOf(elements[j]);
        𐡄𐡍 (i < 0) {
            removeElementAt(j);
            changed = 𐡒𐡔𐡈;
        }
    }
    أرجع changed;
}

فراغ UVector32::removeElementAt(int32_t index) {
    𐡄𐡍 (index >= 0) {
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

    𐡄𐡍 (هذا->count != other.count) {
        أرجع خطأ;
    }
    لكل (i=0; i<count; i++) {
        𐡄𐡍 (elements[i] != other.elements[i]) {
            أرجع خطأ;
        }
    }
    أرجع 𐡒𐡔𐡈;
}




int32_t UVector32::indexOf(int32_t key, int32_t startIndex) ثابت {
    int32_t i;
    لكل (i=startIndex; i<count; ++i) {
        𐡄𐡍 (key == elements[i]) {
            أرجع i;
        }
    }
    أرجع -1;
}


UBool UVector32::expandCapacity(int32_t minimumCapacity, UErrorCode &status) {
    𐡄𐡍 (U_FAILURE(status)) {
        أرجع خطأ;
    }
    𐡄𐡍 (minimumCapacity < 0) {
        status = U_ILLEGAL_ARGUMENT_ERROR;
        أرجع خطأ;
    }
    𐡄𐡍 (capacity >= minimumCapacity) {
        أرجع 𐡒𐡔𐡈;
    }
    𐡄𐡍 (maxCapacity>0 && minimumCapacity>maxCapacity) {
        status = U_BUFFER_OVERFLOW_ERROR;
        أرجع خطأ;
    }
    𐡄𐡍 (capacity > (INT32_MAX - 1) / 2) {  // integer overflow check
        status = U_ILLEGAL_ARGUMENT_ERROR;
        أرجع خطأ;
    }
    int32_t newCap = capacity * 2;
    𐡄𐡍 (newCap < minimumCapacity) {
        newCap = minimumCapacity;
    }
    𐡄𐡍 (maxCapacity > 0 && newCap > maxCapacity) {
        newCap = maxCapacity;
    }
    𐡄𐡍 (newCap > static_cast<int32_t>(INT32_MAX / حجم(int32_t))) { // integer overflow check
        // We keep the original memory contents on bad minimumCapacity/maxCapacity.
        status = U_ILLEGAL_ARGUMENT_ERROR;
        أرجع خطأ;
    }
    int32_t* newElems = static_cast<int32_t*>(uprv_realloc(elements, حجم(int32_t) * newCap));
    𐡄𐡍 (newElems == nullptr) {
        // We keep the original contents on the memory failure on realloc.
        status = U_MEMORY_ALLOCATION_ERROR;
        أرجع خطأ;
    }
    elements = newElems;
    capacity = newCap;
    أرجع 𐡒𐡔𐡈;
}

فراغ UVector32::setMaxCapacity(int32_t limit) {
    U_ASSERT(limit >= 0);
    𐡄𐡍 (limit < 0) {
        limit = 0;
    }
    𐡄𐡍 (limit > static_cast<int32_t>(INT32_MAX / حجم(int32_t))) { // integer overflow check for realloc
        //  Something is very wrong, don't realloc, leave capacity and maxCapacity unchanged
        أرجع;
    }
    maxCapacity = limit;
    𐡄𐡍 (capacity <= maxCapacity || maxCapacity == 0) {
        // Current capacity is within the new limit.
        أرجع;
    }
    
    // New maximum capacity is smaller than the current size.
    // Realloc the storage to the new, smaller size.
    int32_t* newElems = static_cast<int32_t*>(uprv_realloc(elements, حجم(int32_t) * maxCapacity));
    𐡄𐡍 (newElems == nullptr) {
        // Realloc to smaller failed.
        //   Just keep what we had.  No need to call it a failure.
        أرجع;
    }
    elements = newElems;
    capacity = maxCapacity;
    𐡄𐡍 (count > capacity) {
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
    𐡄𐡍 (newSize < 0) {
        أرجع;
    }
    𐡄𐡍 (newSize > count) {
        UErrorCode ec = U_ZERO_ERROR;
        𐡄𐡍 (!ensureCapacity(newSize, ec)) {
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
فراغ UVector32::sortedInsert(int32_t tok, UErrorCode& ec) {
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
        𐡄𐡍 (elements[probe] > tok) {
            max = probe;
        } وإلا {
            // assert(c <= 0);
            min = probe + 1;
        }
    }
    𐡄𐡍 (ensureCapacity(count + 1, ec)) {
        لكل (int32_t i=count; i>min; --i) {
            elements[i] = elements[i-1];
        }
        elements[min] = tok;
        ++count;
    }
}





U_NAMESPACE_END

