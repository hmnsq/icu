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

#समावेश "uvectr32.h"
#समावेश "cmemory.h"
#समावेश "putilimp.h"

U_NAMESPACE_BEGIN

#घोषणा DEFAULT_CAPACITY 8

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



व्योम UVector32::_init(int32_t initialCapacity, UErrorCode &status) {
    // Fix bogus initialCapacity values; avoid malloc(0)
    अगर (initialCapacity < 1) {
        initialCapacity = DEFAULT_CAPACITY;
    }
    अगर (maxCapacity>0 && maxCapacity<initialCapacity) {
        initialCapacity = maxCapacity;
    }
    अगर (initialCapacity > static_cast<int32_t>(INT32_MAX / माप(int32_t))) {
        initialCapacity = uprv_min(DEFAULT_CAPACITY, maxCapacity);
    }
    elements = static_cast<int32_t*>(uprv_malloc(माप(int32_t) * initialCapacity));
    अगर (elements == nullptr) {
        status = U_MEMORY_ALLOCATION_ERROR;
    } अन्यथा {
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
व्योम UVector32::assign(स्थिर UVector32& other, UErrorCode &ec) {
    अगर (ensureCapacity(other.count, ec)) {
        set_आकार(other.count);
        क्रम (int32_t i=0; i<other.count; ++i) {
            elements[i] = other.elements[i];
        }
    }
}


bool UVector32::चालक==(स्थिर UVector32& other) स्थिर {
    int32_t i;
    अगर (count != other.count) वापस false;
    क्रम (i=0; i<count; ++i) {
        अगर (elements[i] != other.elements[i]) {
            वापस false;
        }
    }
    वापस true;
}


व्योम UVector32::setElementAt(int32_t elem, int32_t सूचकांक) {
    अगर (0 <= सूचकांक && सूचकांक < count) {
        elements[सूचकांक] = elem;
    }
    /* else index out of range */
}

व्योम UVector32::जोड़ो_Element_At(int32_t elem, int32_t सूचकांक, UErrorCode &status) {
    // must have 0 <= index <= count
    अगर (0 <= सूचकांक && सूचकांक <= count && ensureCapacity(count + 1, status)) {
        क्रम (int32_t i=count; i>सूचकांक; --i) {
            elements[i] = elements[i-1];
        }
        elements[सूचकांक] = elem;
        ++count;
    }
    /* else index out of range */
}

UBool UVector32::containsAll(स्थिर UVector32& other) स्थिर {
    क्रम (int32_t i=0; i<other.size(); ++i) {
        अगर (सूचकांक_Of(other.elements[i]) < 0) {
            वापस false;
        }
    }
    वापस true;
}

UBool UVector32::containsNone(स्थिर UVector32& other) स्थिर {
    क्रम (int32_t i=0; i<other.size(); ++i) {
        अगर (सूचकांक_Of(other.elements[i]) >= 0) {
            वापस false;
        }
    }
    वापस true;
}

UBool UVector32::removeAll(स्थिर UVector32& other) {
    UBool changed = false;
    क्रम (int32_t i=0; i<other.size(); ++i) {
        int32_t j = सूचकांक_Of(other.elements[i]);
        अगर (j >= 0) {
            removeElementAt(j);
            changed = true;
        }
    }
    वापस changed;
}

UBool UVector32::retainAll(स्थिर UVector32& other) {
    UBool changed = false;
    क्रम (int32_t j=size()-1; j>=0; --j) {
        int32_t i = other.सूचकांक_Of(elements[j]);
        अगर (i < 0) {
            removeElementAt(j);
            changed = true;
        }
    }
    वापस changed;
}

व्योम UVector32::removeElementAt(int32_t सूचकांक) {
    अगर (सूचकांक >= 0) {
        क्रम (int32_t i=सूचकांक; i<count-1; ++i) {
            elements[i] = elements[i+1];
        }
        --count;
    }
}

व्योम UVector32::removeAllElements() {
    count = 0;
}

UBool   UVector32::equals(स्थिर UVector32 &other) स्थिर {
    पूर्णांक      i;

    अगर (यह->count != other.count) {
        वापस false;
    }
    क्रम (i=0; i<count; i++) {
        अगर (elements[i] != other.elements[i]) {
            वापस false;
        }
    }
    वापस true;
}




int32_t UVector32::सूचकांक_Of(int32_t कुंजी, int32_t start_सूचकांक) स्थिर {
    int32_t i;
    क्रम (i=start_सूचकांक; i<count; ++i) {
        अगर (कुंजी == elements[i]) {
            वापस i;
        }
    }
    वापस -1;
}


UBool UVector32::expandCapacity(int32_t minimumCapacity, UErrorCode &status) {
    अगर (U_FAILURE(status)) {
        वापस false;
    }
    अगर (minimumCapacity < 0) {
        status = U_ILLEGAL_ARGUMENT_ERROR;
        वापस false;
    }
    अगर (capacity >= minimumCapacity) {
        वापस true;
    }
    अगर (maxCapacity>0 && minimumCapacity>maxCapacity) {
        status = U_बफर_OVERFLOW_ERROR;
        वापस false;
    }
    अगर (capacity > (INT32_MAX - 1) / 2) {  // integer overflow check
        status = U_ILLEGAL_ARGUMENT_ERROR;
        वापस false;
    }
    int32_t newCap = capacity * 2;
    अगर (newCap < minimumCapacity) {
        newCap = minimumCapacity;
    }
    अगर (maxCapacity > 0 && newCap > maxCapacity) {
        newCap = maxCapacity;
    }
    अगर (newCap > static_cast<int32_t>(INT32_MAX / माप(int32_t))) { // integer overflow check
        // We keep the original memory contents on bad minimumCapacity/maxCapacity.
        status = U_ILLEGAL_ARGUMENT_ERROR;
        वापस false;
    }
    int32_t* newElems = static_cast<int32_t*>(uprv_realloc(elements, माप(int32_t) * newCap));
    अगर (newElems == nullptr) {
        // We keep the original contents on the memory failure on realloc.
        status = U_MEMORY_ALLOCATION_ERROR;
        वापस false;
    }
    elements = newElems;
    capacity = newCap;
    वापस true;
}

व्योम UVector32::setMaxCapacity(int32_t limit) {
    U_ASSERT(limit >= 0);
    अगर (limit < 0) {
        limit = 0;
    }
    अगर (limit > static_cast<int32_t>(INT32_MAX / माप(int32_t))) { // integer overflow check for realloc
        //  Something is very wrong, don't realloc, leave capacity and maxCapacity unchanged
        वापस;
    }
    maxCapacity = limit;
    अगर (capacity <= maxCapacity || maxCapacity == 0) {
        // Current capacity is within the new limit.
        वापस;
    }
    
    // New maximum capacity is smaller than the current size.
    // Realloc the storage to the new, smaller size.
    int32_t* newElems = static_cast<int32_t*>(uprv_realloc(elements, माप(int32_t) * maxCapacity));
    अगर (newElems == nullptr) {
        // Realloc to smaller failed.
        //   Just keep what we had.  No need to call it a failure.
        वापस;
    }
    elements = newElems;
    capacity = maxCapacity;
    अगर (count > capacity) {
        count = capacity;
    }
}

/**
 * Change the size of this vector as follows: If newSize is smaller,
 * then truncate the array, possibly deleting held elements for i >=
 * newSize.  If newSize is larger, grow the array, filling in new
 * slots with nullptr.
 */
व्योम UVector32::set_आकार(int32_t new_आकार) {
    int32_t i;
    अगर (new_आकार < 0) {
        वापस;
    }
    अगर (new_आकार > count) {
        UErrorCode ec = U_ZERO_ERROR;
        अगर (!ensureCapacity(new_आकार, ec)) {
            वापस;
        }
        क्रम (i=count; i<new_आकार; ++i) {
            elements[i] = 0;
        }
    } 
    count = new_आकार;
}




/**
 * Insert the given integer into this vector at its sorted position
 * as defined by 'compare'.  The current elements are assumed to
 * be sorted already.
 */
व्योम UVector32::sorted_जोड़ो(int32_t tok, UErrorCode& ec) {
    // Perform a binary search for the location to insert tok at.  Tok
    // will be inserted between two elements a and b such that a <=
    // tok && tok < b, where there is a 'virtual' elements[-1] always
    // less than tok and a 'virtual' elements[count] always greater
    // than tok.
    int32_t min = 0, max = count;
    जबतक (min != max) {
        int32_t probe = (min + max) / 2;
        //int8_t c = (*compare)(elements[probe], tok);
        //if (c > 0) {
        अगर (elements[probe] > tok) {
            max = probe;
        } अन्यथा {
            // assert(c <= 0);
            min = probe + 1;
        }
    }
    अगर (ensureCapacity(count + 1, ec)) {
        क्रम (int32_t i=count; i>min; --i) {
            elements[i] = elements[i-1];
        }
        elements[min] = tok;
        ++count;
    }
}





U_NAMESPACE_END

