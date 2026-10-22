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

#অন্তর্ভাব "uvectr32.h"
#অন্তর্ভাব "cmemory.h"
#অন্তর্ভাব "putilimp.h"

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



শূন্য UVector32::_init(int32_t initialCapacity, UErrorCode &status) {
    // Fix bogus initialCapacity values; avoid malloc(0)
    যদি (initialCapacity < 1) {
        initialCapacity = DEFAULT_CAPACITY;
    }
    যদি (maxCapacity>0 && maxCapacity<initialCapacity) {
        initialCapacity = maxCapacity;
    }
    যদি (initialCapacity > static_cast<int32_t>(INT32_MAX / আকার(int32_t))) {
        initialCapacity = uprv_min(DEFAULT_CAPACITY, maxCapacity);
    }
    elements = static_cast<int32_t*>(uprv_malloc(আকার(int32_t) * initialCapacity));
    যদি (elements == nullptr) {
        status = U_MEMORY_ALLOCATION_ERROR;
    } অন্যথা {
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
শূন্য UVector32::assign(ধ্রুবক UVector32& other, UErrorCode &ec) {
    যদি (ensureCapacity(other.count, ec)) {
        set_আকার(other.count);
        জন্য (int32_t i=0; i<other.count; ++i) {
            elements[i] = other.elements[i];
        }
    }
}


যৌক্তিক UVector32::operator==(ধ্রুবক UVector32& other) ধ্রুবক {
    int32_t i;
    যদি (count != other.count) ফেরত মিথ্যা;
    জন্য (i=0; i<count; ++i) {
        যদি (elements[i] != other.elements[i]) {
            ফেরত মিথ্যা;
        }
    }
    ফেরত সত্য;
}


শূন্য UVector32::setElementAt(int32_t elem, int32_t সূচক) {
    যদি (0 <= সূচক && সূচক < count) {
        elements[সূচক] = elem;
    }
    /* else index out of range */
}

শূন্য UVector32::ঢোকাও_Element_At(int32_t elem, int32_t সূচক, UErrorCode &status) {
    // must have 0 <= index <= count
    যদি (0 <= সূচক && সূচক <= count && ensureCapacity(count + 1, status)) {
        জন্য (int32_t i=count; i>সূচক; --i) {
            elements[i] = elements[i-1];
        }
        elements[সূচক] = elem;
        ++count;
    }
    /* else index out of range */
}

UBool UVector32::containsAll(ধ্রুবক UVector32& other) ধ্রুবক {
    জন্য (int32_t i=0; i<other.size(); ++i) {
        যদি (সূচক_Of(other.elements[i]) < 0) {
            ফেরত মিথ্যা;
        }
    }
    ফেরত সত্য;
}

UBool UVector32::containsNone(ধ্রুবক UVector32& other) ধ্রুবক {
    জন্য (int32_t i=0; i<other.size(); ++i) {
        যদি (সূচক_Of(other.elements[i]) >= 0) {
            ফেরত মিথ্যা;
        }
    }
    ফেরত সত্য;
}

UBool UVector32::removeAll(ধ্রুবক UVector32& other) {
    UBool changed = মিথ্যা;
    জন্য (int32_t i=0; i<other.size(); ++i) {
        int32_t j = সূচক_Of(other.elements[i]);
        যদি (j >= 0) {
            removeElementAt(j);
            changed = সত্য;
        }
    }
    ফেরত changed;
}

UBool UVector32::retainAll(ধ্রুবক UVector32& other) {
    UBool changed = মিথ্যা;
    জন্য (int32_t j=size()-1; j>=0; --j) {
        int32_t i = other.সূচক_Of(elements[j]);
        যদি (i < 0) {
            removeElementAt(j);
            changed = সত্য;
        }
    }
    ফেরত changed;
}

শূন্য UVector32::removeElementAt(int32_t সূচক) {
    যদি (সূচক >= 0) {
        জন্য (int32_t i=সূচক; i<count-1; ++i) {
            elements[i] = elements[i+1];
        }
        --count;
    }
}

শূন্য UVector32::removeAllElements() {
    count = 0;
}

UBool   UVector32::equals(ধ্রুবক UVector32 &other) ধ্রুবক {
    পূর্ণসংখ্যা      i;

    যদি (এই->count != other.count) {
        ফেরত মিথ্যা;
    }
    জন্য (i=0; i<count; i++) {
        যদি (elements[i] != other.elements[i]) {
            ফেরত মিথ্যা;
        }
    }
    ফেরত সত্য;
}




int32_t UVector32::সূচক_Of(int32_t চাবি, int32_t start_সূচক) ধ্রুবক {
    int32_t i;
    জন্য (i=start_সূচক; i<count; ++i) {
        যদি (চাবি == elements[i]) {
            ফেরত i;
        }
    }
    ফেরত -1;
}


UBool UVector32::expandCapacity(int32_t minimumCapacity, UErrorCode &status) {
    যদি (U_FAILURE(status)) {
        ফেরত মিথ্যা;
    }
    যদি (minimumCapacity < 0) {
        status = U_ILLEGAL_ARGUMENT_ERROR;
        ফেরত মিথ্যা;
    }
    যদি (capacity >= minimumCapacity) {
        ফেরত সত্য;
    }
    যদি (maxCapacity>0 && minimumCapacity>maxCapacity) {
        status = U_বাফার_OVERFLOW_ERROR;
        ফেরত মিথ্যা;
    }
    যদি (capacity > (INT32_MAX - 1) / 2) {  // integer overflow check
        status = U_ILLEGAL_ARGUMENT_ERROR;
        ফেরত মিথ্যা;
    }
    int32_t newCap = capacity * 2;
    যদি (newCap < minimumCapacity) {
        newCap = minimumCapacity;
    }
    যদি (maxCapacity > 0 && newCap > maxCapacity) {
        newCap = maxCapacity;
    }
    যদি (newCap > static_cast<int32_t>(INT32_MAX / আকার(int32_t))) { // integer overflow check
        // We keep the original memory contents on bad minimumCapacity/maxCapacity.
        status = U_ILLEGAL_ARGUMENT_ERROR;
        ফেরত মিথ্যা;
    }
    int32_t* newElems = static_cast<int32_t*>(uprv_realloc(elements, আকার(int32_t) * newCap));
    যদি (newElems == nullptr) {
        // We keep the original contents on the memory failure on realloc.
        status = U_MEMORY_ALLOCATION_ERROR;
        ফেরত মিথ্যা;
    }
    elements = newElems;
    capacity = newCap;
    ফেরত সত্য;
}

শূন্য UVector32::setMaxCapacity(int32_t limit) {
    U_ASSERT(limit >= 0);
    যদি (limit < 0) {
        limit = 0;
    }
    যদি (limit > static_cast<int32_t>(INT32_MAX / আকার(int32_t))) { // integer overflow check for realloc
        //  Something is very wrong, don't realloc, leave capacity and maxCapacity unchanged
        ফেরত;
    }
    maxCapacity = limit;
    যদি (capacity <= maxCapacity || maxCapacity == 0) {
        // Current capacity is within the new limit.
        ফেরত;
    }
    
    // New maximum capacity is smaller than the current size.
    // Realloc the storage to the new, smaller size.
    int32_t* newElems = static_cast<int32_t*>(uprv_realloc(elements, আকার(int32_t) * maxCapacity));
    যদি (newElems == nullptr) {
        // Realloc to smaller failed.
        //   Just keep what we had.  No need to call it a failure.
        ফেরত;
    }
    elements = newElems;
    capacity = maxCapacity;
    যদি (count > capacity) {
        count = capacity;
    }
}

/**
 * Change the size of this vector as follows: If newSize is smaller,
 * then truncate the array, possibly deleting held elements for i >=
 * newSize.  If newSize is larger, grow the array, filling in new
 * slots with nullptr.
 */
শূন্য UVector32::set_আকার(int32_t new_আকার) {
    int32_t i;
    যদি (new_আকার < 0) {
        ফেরত;
    }
    যদি (new_আকার > count) {
        UErrorCode ec = U_ZERO_ERROR;
        যদি (!ensureCapacity(new_আকার, ec)) {
            ফেরত;
        }
        জন্য (i=count; i<new_আকার; ++i) {
            elements[i] = 0;
        }
    } 
    count = new_আকার;
}




/**
 * Insert the given integer into this vector at its sorted position
 * as defined by 'compare'.  The current elements are assumed to
 * be sorted already.
 */
শূন্য UVector32::sorted_ঢোকাও(int32_t tok, UErrorCode& ec) {
    // Perform a binary search for the location to insert tok at.  Tok
    // will be inserted between two elements a and b such that a <=
    // tok && tok < b, where there is a 'virtual' elements[-1] always
    // less than tok and a 'virtual' elements[count] always greater
    // than tok.
    int32_t min = 0, max = count;
    যখন (min != max) {
        int32_t probe = (min + max) / 2;
        //int8_t c = (*compare)(elements[probe], tok);
        //if (c > 0) {
        যদি (elements[probe] > tok) {
            max = probe;
        } অন্যথা {
            // assert(c <= 0);
            min = probe + 1;
        }
    }
    যদি (ensureCapacity(count + 1, ec)) {
        জন্য (int32_t i=count; i>min; --i) {
            elements[i] = elements[i-1];
        }
        elements[min] = tok;
        ++count;
    }
}





U_NAMESPACE_END

