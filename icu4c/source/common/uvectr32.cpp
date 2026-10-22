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

#અન્તર્ભાવ "uvectr32.h"
#અન્તર્ભાવ "cmemory.h"
#અન્તર્ભાવ "putilimp.h"

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



શૂન્ય UVector32::_init(int32_t initialCapacity, UErrorCode &status) {
    // Fix bogus initialCapacity values; avoid malloc(0)
    જો (initialCapacity < 1) {
        initialCapacity = DEFAULT_CAPACITY;
    }
    જો (maxCapacity>0 && maxCapacity<initialCapacity) {
        initialCapacity = maxCapacity;
    }
    જો (initialCapacity > static_cast<int32_t>(INT32_MAX / કદ(int32_t))) {
        initialCapacity = uprv_min(DEFAULT_CAPACITY, maxCapacity);
    }
    elements = static_cast<int32_t*>(uprv_malloc(કદ(int32_t) * initialCapacity));
    જો (elements == nullptr) {
        status = U_MEMORY_ALLOCATION_ERROR;
    } નહીંતર {
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
શૂન્ય UVector32::assign(અચલ UVector32& other, UErrorCode &ec) {
    જો (ensureCapacity(other.count, ec)) {
        setSize(other.count);
        માટે (int32_t i=0; i<other.count; ++i) {
            elements[i] = other.elements[i];
        }
    }
}


તાર્કિક UVector32::operator==(અચલ UVector32& other) અચલ {
    int32_t i;
    જો (count != other.count) પાછા ખોટું;
    માટે (i=0; i<count; ++i) {
        જો (elements[i] != other.elements[i]) {
            પાછા ખોટું;
        }
    }
    પાછા સાચું;
}


શૂન્ય UVector32::setElementAt(int32_t elem, int32_t index) {
    જો (0 <= index && index < count) {
        elements[index] = elem;
    }
    /* else index out of range */
}

શૂન્ય UVector32::ઉમેરો_Element_At(int32_t elem, int32_t index, UErrorCode &status) {
    // must have 0 <= index <= count
    જો (0 <= index && index <= count && ensureCapacity(count + 1, status)) {
        માટે (int32_t i=count; i>index; --i) {
            elements[i] = elements[i-1];
        }
        elements[index] = elem;
        ++count;
    }
    /* else index out of range */
}

UBool UVector32::containsAll(અચલ UVector32& other) અચલ {
    માટે (int32_t i=0; i<other.size(); ++i) {
        જો (indexOf(other.elements[i]) < 0) {
            પાછા ખોટું;
        }
    }
    પાછા સાચું;
}

UBool UVector32::containsNone(અચલ UVector32& other) અચલ {
    માટે (int32_t i=0; i<other.size(); ++i) {
        જો (indexOf(other.elements[i]) >= 0) {
            પાછા ખોટું;
        }
    }
    પાછા સાચું;
}

UBool UVector32::removeAll(અચલ UVector32& other) {
    UBool changed = ખોટું;
    માટે (int32_t i=0; i<other.size(); ++i) {
        int32_t j = indexOf(other.elements[i]);
        જો (j >= 0) {
            removeElementAt(j);
            changed = સાચું;
        }
    }
    પાછા changed;
}

UBool UVector32::retainAll(અચલ UVector32& other) {
    UBool changed = ખોટું;
    માટે (int32_t j=size()-1; j>=0; --j) {
        int32_t i = other.indexOf(elements[j]);
        જો (i < 0) {
            removeElementAt(j);
            changed = સાચું;
        }
    }
    પાછા changed;
}

શૂન્ય UVector32::removeElementAt(int32_t index) {
    જો (index >= 0) {
        માટે (int32_t i=index; i<count-1; ++i) {
            elements[i] = elements[i+1];
        }
        --count;
    }
}

શૂન્ય UVector32::removeAllElements() {
    count = 0;
}

UBool   UVector32::equals(અચલ UVector32 &other) અચલ {
    પૂર્ણાંક      i;

    જો (આ->count != other.count) {
        પાછા ખોટું;
    }
    માટે (i=0; i<count; i++) {
        જો (elements[i] != other.elements[i]) {
            પાછા ખોટું;
        }
    }
    પાછા સાચું;
}




int32_t UVector32::indexOf(int32_t ચાવી, int32_t startIndex) અચલ {
    int32_t i;
    માટે (i=startIndex; i<count; ++i) {
        જો (ચાવી == elements[i]) {
            પાછા i;
        }
    }
    પાછા -1;
}


UBool UVector32::expandCapacity(int32_t minimumCapacity, UErrorCode &status) {
    જો (U_FAILURE(status)) {
        પાછા ખોટું;
    }
    જો (minimumCapacity < 0) {
        status = U_ILLEGAL_ARGUMENT_ERROR;
        પાછા ખોટું;
    }
    જો (capacity >= minimumCapacity) {
        પાછા સાચું;
    }
    જો (maxCapacity>0 && minimumCapacity>maxCapacity) {
        status = U_BUFFER_OVERFLOW_ERROR;
        પાછા ખોટું;
    }
    જો (capacity > (INT32_MAX - 1) / 2) {  // integer overflow check
        status = U_ILLEGAL_ARGUMENT_ERROR;
        પાછા ખોટું;
    }
    int32_t newCap = capacity * 2;
    જો (newCap < minimumCapacity) {
        newCap = minimumCapacity;
    }
    જો (maxCapacity > 0 && newCap > maxCapacity) {
        newCap = maxCapacity;
    }
    જો (newCap > static_cast<int32_t>(INT32_MAX / કદ(int32_t))) { // integer overflow check
        // We keep the original memory contents on bad minimumCapacity/maxCapacity.
        status = U_ILLEGAL_ARGUMENT_ERROR;
        પાછા ખોટું;
    }
    int32_t* newElems = static_cast<int32_t*>(uprv_realloc(elements, કદ(int32_t) * newCap));
    જો (newElems == nullptr) {
        // We keep the original contents on the memory failure on realloc.
        status = U_MEMORY_ALLOCATION_ERROR;
        પાછા ખોટું;
    }
    elements = newElems;
    capacity = newCap;
    પાછા સાચું;
}

શૂન્ય UVector32::setMaxCapacity(int32_t limit) {
    U_ASSERT(limit >= 0);
    જો (limit < 0) {
        limit = 0;
    }
    જો (limit > static_cast<int32_t>(INT32_MAX / કદ(int32_t))) { // integer overflow check for realloc
        //  Something is very wrong, don't realloc, leave capacity and maxCapacity unchanged
        પાછા;
    }
    maxCapacity = limit;
    જો (capacity <= maxCapacity || maxCapacity == 0) {
        // Current capacity is within the new limit.
        પાછા;
    }
    
    // New maximum capacity is smaller than the current size.
    // Realloc the storage to the new, smaller size.
    int32_t* newElems = static_cast<int32_t*>(uprv_realloc(elements, કદ(int32_t) * maxCapacity));
    જો (newElems == nullptr) {
        // Realloc to smaller failed.
        //   Just keep what we had.  No need to call it a failure.
        પાછા;
    }
    elements = newElems;
    capacity = maxCapacity;
    જો (count > capacity) {
        count = capacity;
    }
}

/**
 * Change the size of this vector as follows: If newSize is smaller,
 * then truncate the array, possibly deleting held elements for i >=
 * newSize.  If newSize is larger, grow the array, filling in new
 * slots with nullptr.
 */
શૂન્ય UVector32::setSize(int32_t newSize) {
    int32_t i;
    જો (newSize < 0) {
        પાછા;
    }
    જો (newSize > count) {
        UErrorCode ec = U_ZERO_ERROR;
        જો (!ensureCapacity(newSize, ec)) {
            પાછા;
        }
        માટે (i=count; i<newSize; ++i) {
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
શૂન્ય UVector32::sorted_ઉમેરો(int32_t tok, UErrorCode& ec) {
    // Perform a binary search for the location to insert tok at.  Tok
    // will be inserted between two elements a and b such that a <=
    // tok && tok < b, where there is a 'virtual' elements[-1] always
    // less than tok and a 'virtual' elements[count] always greater
    // than tok.
    int32_t min = 0, max = count;
    જ્યારે (min != max) {
        int32_t probe = (min + max) / 2;
        //int8_t c = (*compare)(elements[probe], tok);
        //if (c > 0) {
        જો (elements[probe] > tok) {
            max = probe;
        } નહીંતર {
            // assert(c <= 0);
            min = probe + 1;
        }
    }
    જો (ensureCapacity(count + 1, ec)) {
        માટે (int32_t i=count; i>min; --i) {
            elements[i] = elements[i-1];
        }
        elements[min] = tok;
        ++count;
    }
}





U_NAMESPACE_END

