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

#അന്തര്ഭാവ "uvectr32.h"
#അന്തര്ഭാവ "cmemory.h"
#അന്തര്ഭാവ "putilimp.h"

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



ശൂന്യം UVector32::_init(int32_t initialCapacity, UErrorCode &status) {
    // Fix bogus initialCapacity values; avoid malloc(0)
    എങ്കിൽ (initialCapacity < 1) {
        initialCapacity = DEFAULT_CAPACITY;
    }
    എങ്കിൽ (maxCapacity>0 && maxCapacity<initialCapacity) {
        initialCapacity = maxCapacity;
    }
    എങ്കിൽ (initialCapacity > static_cast<int32_t>(INT32_MAX / വലുപ്പം(int32_t))) {
        initialCapacity = uprv_min(DEFAULT_CAPACITY, maxCapacity);
    }
    elements = static_cast<int32_t*>(uprv_malloc(വലുപ്പം(int32_t) * initialCapacity));
    എങ്കിൽ (elements == nullptr) {
        status = U_MEMORY_ALLOCATION_ERROR;
    } അല്ലെങ്കിൽ {
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
ശൂന്യം UVector32::assign(സ്ഥിരം UVector32& other, UErrorCode &ec) {
    എങ്കിൽ (ensureCapacity(other.count, ec)) {
        setSize(other.count);
        വേണ്ടി (int32_t i=0; i<other.count; ++i) {
            elements[i] = other.elements[i];
        }
    }
}


തർക്കം UVector32::operator==(സ്ഥിരം UVector32& other) സ്ഥിരം {
    int32_t i;
    എങ്കിൽ (count != other.count) തിരികെ അസത്യം;
    വേണ്ടി (i=0; i<count; ++i) {
        എങ്കിൽ (elements[i] != other.elements[i]) {
            തിരികെ അസത്യം;
        }
    }
    തിരികെ സത്യം;
}


ശൂന്യം UVector32::setElementAt(int32_t elem, int32_t index) {
    എങ്കിൽ (0 <= index && index < count) {
        elements[index] = elem;
    }
    /* else index out of range */
}

ശൂന്യം UVector32::തിരുകുക_Element_At(int32_t elem, int32_t index, UErrorCode &status) {
    // must have 0 <= index <= count
    എങ്കിൽ (0 <= index && index <= count && ensureCapacity(count + 1, status)) {
        വേണ്ടി (int32_t i=count; i>index; --i) {
            elements[i] = elements[i-1];
        }
        elements[index] = elem;
        ++count;
    }
    /* else index out of range */
}

UBool UVector32::containsAll(സ്ഥിരം UVector32& other) സ്ഥിരം {
    വേണ്ടി (int32_t i=0; i<other.size(); ++i) {
        എങ്കിൽ (indexOf(other.elements[i]) < 0) {
            തിരികെ അസത്യം;
        }
    }
    തിരികെ സത്യം;
}

UBool UVector32::containsNone(സ്ഥിരം UVector32& other) സ്ഥിരം {
    വേണ്ടി (int32_t i=0; i<other.size(); ++i) {
        എങ്കിൽ (indexOf(other.elements[i]) >= 0) {
            തിരികെ അസത്യം;
        }
    }
    തിരികെ സത്യം;
}

UBool UVector32::removeAll(സ്ഥിരം UVector32& other) {
    UBool changed = അസത്യം;
    വേണ്ടി (int32_t i=0; i<other.size(); ++i) {
        int32_t j = indexOf(other.elements[i]);
        എങ്കിൽ (j >= 0) {
            removeElementAt(j);
            changed = സത്യം;
        }
    }
    തിരികെ changed;
}

UBool UVector32::retainAll(സ്ഥിരം UVector32& other) {
    UBool changed = അസത്യം;
    വേണ്ടി (int32_t j=size()-1; j>=0; --j) {
        int32_t i = other.indexOf(elements[j]);
        എങ്കിൽ (i < 0) {
            removeElementAt(j);
            changed = സത്യം;
        }
    }
    തിരികെ changed;
}

ശൂന്യം UVector32::removeElementAt(int32_t index) {
    എങ്കിൽ (index >= 0) {
        വേണ്ടി (int32_t i=index; i<count-1; ++i) {
            elements[i] = elements[i+1];
        }
        --count;
    }
}

ശൂന്യം UVector32::removeAllElements() {
    count = 0;
}

UBool   UVector32::equals(സ്ഥിരം UVector32 &other) സ്ഥിരം {
    പൂർണ്ണസംഖ്യ      i;

    എങ്കിൽ (ഇത്->count != other.count) {
        തിരികെ അസത്യം;
    }
    വേണ്ടി (i=0; i<count; i++) {
        എങ്കിൽ (elements[i] != other.elements[i]) {
            തിരികെ അസത്യം;
        }
    }
    തിരികെ സത്യം;
}




int32_t UVector32::indexOf(int32_t താക്കോൽ, int32_t startIndex) സ്ഥിരം {
    int32_t i;
    വേണ്ടി (i=startIndex; i<count; ++i) {
        എങ്കിൽ (താക്കോൽ == elements[i]) {
            തിരികെ i;
        }
    }
    തിരികെ -1;
}


UBool UVector32::expandCapacity(int32_t minimumCapacity, UErrorCode &status) {
    എങ്കിൽ (U_FAILURE(status)) {
        തിരികെ അസത്യം;
    }
    എങ്കിൽ (minimumCapacity < 0) {
        status = U_ILLEGAL_ARGUMENT_ERROR;
        തിരികെ അസത്യം;
    }
    എങ്കിൽ (capacity >= minimumCapacity) {
        തിരികെ സത്യം;
    }
    എങ്കിൽ (maxCapacity>0 && minimumCapacity>maxCapacity) {
        status = U_BUFFER_OVERFLOW_ERROR;
        തിരികെ അസത്യം;
    }
    എങ്കിൽ (capacity > (INT32_MAX - 1) / 2) {  // integer overflow check
        status = U_ILLEGAL_ARGUMENT_ERROR;
        തിരികെ അസത്യം;
    }
    int32_t newCap = capacity * 2;
    എങ്കിൽ (newCap < minimumCapacity) {
        newCap = minimumCapacity;
    }
    എങ്കിൽ (maxCapacity > 0 && newCap > maxCapacity) {
        newCap = maxCapacity;
    }
    എങ്കിൽ (newCap > static_cast<int32_t>(INT32_MAX / വലുപ്പം(int32_t))) { // integer overflow check
        // We keep the original memory contents on bad minimumCapacity/maxCapacity.
        status = U_ILLEGAL_ARGUMENT_ERROR;
        തിരികെ അസത്യം;
    }
    int32_t* newElems = static_cast<int32_t*>(uprv_realloc(elements, വലുപ്പം(int32_t) * newCap));
    എങ്കിൽ (newElems == nullptr) {
        // We keep the original contents on the memory failure on realloc.
        status = U_MEMORY_ALLOCATION_ERROR;
        തിരികെ അസത്യം;
    }
    elements = newElems;
    capacity = newCap;
    തിരികെ സത്യം;
}

ശൂന്യം UVector32::setMaxCapacity(int32_t limit) {
    U_ASSERT(limit >= 0);
    എങ്കിൽ (limit < 0) {
        limit = 0;
    }
    എങ്കിൽ (limit > static_cast<int32_t>(INT32_MAX / വലുപ്പം(int32_t))) { // integer overflow check for realloc
        //  Something is very wrong, don't realloc, leave capacity and maxCapacity unchanged
        തിരികെ;
    }
    maxCapacity = limit;
    എങ്കിൽ (capacity <= maxCapacity || maxCapacity == 0) {
        // Current capacity is within the new limit.
        തിരികെ;
    }
    
    // New maximum capacity is smaller than the current size.
    // Realloc the storage to the new, smaller size.
    int32_t* newElems = static_cast<int32_t*>(uprv_realloc(elements, വലുപ്പം(int32_t) * maxCapacity));
    എങ്കിൽ (newElems == nullptr) {
        // Realloc to smaller failed.
        //   Just keep what we had.  No need to call it a failure.
        തിരികെ;
    }
    elements = newElems;
    capacity = maxCapacity;
    എങ്കിൽ (count > capacity) {
        count = capacity;
    }
}

/**
 * Change the size of this vector as follows: If newSize is smaller,
 * then truncate the array, possibly deleting held elements for i >=
 * newSize.  If newSize is larger, grow the array, filling in new
 * slots with nullptr.
 */
ശൂന്യം UVector32::setSize(int32_t newSize) {
    int32_t i;
    എങ്കിൽ (newSize < 0) {
        തിരികെ;
    }
    എങ്കിൽ (newSize > count) {
        UErrorCode ec = U_ZERO_ERROR;
        എങ്കിൽ (!ensureCapacity(newSize, ec)) {
            തിരികെ;
        }
        വേണ്ടി (i=count; i<newSize; ++i) {
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
ശൂന്യം UVector32::sorted_തിരുകുക(int32_t tok, UErrorCode& ec) {
    // Perform a binary search for the location to insert tok at.  Tok
    // will be inserted between two elements a and b such that a <=
    // tok && tok < b, where there is a 'virtual' elements[-1] always
    // less than tok and a 'virtual' elements[count] always greater
    // than tok.
    int32_t min = 0, max = count;
    ആകുമ്പോൾ (min != max) {
        int32_t probe = (min + max) / 2;
        //int8_t c = (*compare)(elements[probe], tok);
        //if (c > 0) {
        എങ്കിൽ (elements[probe] > tok) {
            max = probe;
        } അല്ലെങ്കിൽ {
            // assert(c <= 0);
            min = probe + 1;
        }
    }
    എങ്കിൽ (ensureCapacity(count + 1, ec)) {
        വേണ്ടി (int32_t i=count; i>min; --i) {
            elements[i] = elements[i-1];
        }
        elements[min] = tok;
        ++count;
    }
}





U_NAMESPACE_END

