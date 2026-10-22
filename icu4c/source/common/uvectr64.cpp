// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
******************************************************************************
* Copyright (C) 1999-2015, International Business Machines Corporation and
* others. All Rights Reserved.
******************************************************************************
*/

#అన్తర్భావ "uvectr64.h"
#అన్తర్భావ "cmemory.h"
#అన్తర్భావ "putilimp.h"

U_NAMESPACE_BEGIN

#నిర్వచించు DEFAULT_సామర్థ్యం 8

/*
 * Constants for hinting whether a key is an integer
 * or a pointer.  If a hint bit is zero, then the associated
 * token is assumed to be an integer. This is needed for iSeries
 */
 
UOBJECT_DEFINE_RTTI_IMPLEMENTATION(UVector64)

UVector64::UVector64(UError_సంకేతం &స్థితి_2) :
    count(0),
    capacity(0),
    max_సామర్థ్యం(0),
    elements(శూన్యసూచిక)
{
    _init(DEFAULT_సామర్థ్యం, స్థితి_2);
}

UVector64::UVector64(int32_t initial_సామర్థ్యం, UError_సంకేతం &స్థితి_2) :
    count(0),
    capacity(0),
    max_సామర్థ్యం(0),
    elements(శూన్యసూచిక)
{
    _init(initial_సామర్థ్యం, స్థితి_2);
}



శూన్య UVector64::_init(int32_t initial_సామర్థ్యం, UError_సంకేతం &స్థితి_2) {
    // Fix bogus initialCapacity values; avoid malloc(0)
    ఒకవేళ (initial_సామర్థ్యం < 1) {
        initial_సామర్థ్యం = DEFAULT_సామర్థ్యం;
    }
    ఒకవేళ (max_సామర్థ్యం>0 && max_సామర్థ్యం<initial_సామర్థ్యం) {
        initial_సామర్థ్యం = max_సామర్థ్యం;
    }
    ఒకవేళ (initial_సామర్థ్యం > static_cast<int32_t>(INT32_MAX / పరిమాణం(int64_t))) {
        initial_సామర్థ్యం = uprv_min(DEFAULT_సామర్థ్యం, max_సామర్థ్యం);
    }
    elements = static_cast<int64_t*>(uprv_malloc(పరిమాణం(int64_t) * initial_సామర్థ్యం));
    ఒకవేళ (elements == శూన్యసూచిక) {
        స్థితి_2 = U_జ్ఞాపకం_కేటాయింపు_ERROR;
    } లేకపోతే {
        capacity = initial_సామర్థ్యం;
    }
}

UVector64::~UVector64() {
    uprv_free(elements);
    elements = శూన్యసూచిక;
}

/**
 * Assign this object to another (make this a copy of 'other').
 */
శూన్య UVector64::assign(స్థిరరాశి UVector64& other, UError_సంకేతం &ec) {
    ఒకవేళ (ensure_సామర్థ్యం(other.count, ec)) {
        సమితి_కొలత(other.count);
        కొరకు (int32_t i=0; i<other.count; ++i) {
            elements[i] = other.elements[i];
        }
    }
}


తార్కిక UVector64::క్రియాకారకం==(స్థిరరాశి UVector64& other) {
    int32_t i;
    ఒకవేళ (count != other.count) తిరిగి అబద్ధం;
    కొరకు (i=0; i<count; ++i) {
        ఒకవేళ (elements[i] != other.elements[i]) {
            తిరిగి అబద్ధం;
        }
    }
    తిరిగి నిజం;
}


శూన్య UVector64::సమితి_Element_At(int64_t elem, int32_t సూచీ) {
    ఒకవేళ (0 <= సూచీ && సూచీ < count) {
        elements[సూచీ] = elem;
    }
    /* else index out of range */
}

శూన్య UVector64::చొప్పించు_Element_At(int64_t elem, int32_t సూచీ, UError_సంకేతం &స్థితి_2) {
    // must have 0 <= index <= count
    ఒకవేళ (0 <= సూచీ && సూచీ <= count && ensure_సామర్థ్యం(count + 1, స్థితి_2)) {
        కొరకు (int32_t i=count; i>సూచీ; --i) {
            elements[i] = elements[i-1];
        }
        elements[సూచీ] = elem;
        ++count;
    }
    /* else index out of range */
}

శూన్య UVector64::removeAllElements() {
    count = 0;
}

UBool UVector64::expand_సామర్థ్యం(int32_t minimum_సామర్థ్యం, UError_సంకేతం &స్థితి_2) {
    ఒకవేళ (U_FAILURE(స్థితి_2)) {
        తిరిగి అబద్ధం;
    }
    ఒకవేళ (minimum_సామర్థ్యం < 0) {
        స్థితి_2 = U_ILLEGAL_వాదం_ERROR;
        తిరిగి అబద్ధం;
    }
    ఒకవేళ (capacity >= minimum_సామర్థ్యం) {
        తిరిగి నిజం;
    }
    ఒకవేళ (max_సామర్థ్యం>0 && minimum_సామర్థ్యం>max_సామర్థ్యం) {
        స్థితి_2 = U_బఫరు_అధికప్రవాహం_ERROR;
        తిరిగి అబద్ధం;
    }
    ఒకవేళ (capacity > (INT32_MAX - 1) / 2) {  // integer overflow check
        స్థితి_2 = U_ILLEGAL_వాదం_ERROR;
        తిరిగి అబద్ధం;
    }
    int32_t newCap = capacity * 2;
    ఒకవేళ (newCap < minimum_సామర్థ్యం) {
        newCap = minimum_సామర్థ్యం;
    }
    ఒకవేళ (max_సామర్థ్యం > 0 && newCap > max_సామర్థ్యం) {
        newCap = max_సామర్థ్యం;
    }
    ఒకవేళ (newCap > static_cast<int32_t>(INT32_MAX / పరిమాణం(int64_t))) { // integer overflow check
        // We keep the original memory contents on bad minimumCapacity/maxCapacity.
        స్థితి_2 = U_ILLEGAL_వాదం_ERROR;
        తిరిగి అబద్ధం;
    }
    int64_t* newElems = static_cast<int64_t*>(uprv_realloc(elements, పరిమాణం(int64_t) * newCap));
    ఒకవేళ (newElems == శూన్యసూచిక) {
        // We keep the original contents on the memory failure on realloc.
        స్థితి_2 = U_జ్ఞాపకం_కేటాయింపు_ERROR;
        తిరిగి అబద్ధం;
    }
    elements = newElems;
    capacity = newCap;
    తిరిగి నిజం;
}

శూన్య UVector64::సమితి_Max_సామర్థ్యం(int32_t హద్దు) {
    U_ASSERT(హద్దు >= 0);
    ఒకవేళ (హద్దు < 0) {
        హద్దు = 0;
    }
    ఒకవేళ (హద్దు > static_cast<int32_t>(INT32_MAX / పరిమాణం(int64_t))) { // integer overflow check for realloc
        //  Something is very wrong, don't realloc, leave capacity and maxCapacity unchanged
        తిరిగి;
    }
    max_సామర్థ్యం = హద్దు;
    ఒకవేళ (capacity <= max_సామర్థ్యం || max_సామర్థ్యం == 0) {
        // Current capacity is within the new limit.
        తిరిగి;
    }
    
    // New maximum capacity is smaller than the current size.
    // Realloc the storage to the new, smaller size.
    int64_t* newElems = static_cast<int64_t*>(uprv_realloc(elements, పరిమాణం(int64_t) * max_సామర్థ్యం));
    ఒకవేళ (newElems == శూన్యసూచిక) {
        // Realloc to smaller failed.
        //   Just keep what we had.  No need to call it a failure.
        తిరిగి;
    }
    elements = newElems;
    capacity = max_సామర్థ్యం;
    ఒకవేళ (count > capacity) {
        count = capacity;
    }
}

/**
 * Change the size of this vector as follows: If newSize is smaller,
 * then truncate the array, possibly deleting held elements for i >=
 * newSize.  If newSize is larger, grow the array, filling in new
 * slots with nullptr.
 */
శూన్య UVector64::సమితి_కొలత(int32_t new_కొలత) {
    int32_t i;
    ఒకవేళ (new_కొలత < 0) {
        తిరిగి;
    }
    ఒకవేళ (new_కొలత > count) {
        UError_సంకేతం ec = U_ZERO_ERROR;
        ఒకవేళ (!ensure_సామర్థ్యం(new_కొలత, ec)) {
            తిరిగి;
        }
        కొరకు (i=count; i<new_కొలత; ++i) {
            elements[i] = 0;
        }
    } 
    count = new_కొలత;
}

U_NAMESPACE_చివర

