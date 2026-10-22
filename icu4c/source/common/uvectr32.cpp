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

#అన్తర్భావ "uvectr32.h"
#అన్తర్భావ "cmemory.h"
#అన్తర్భావ "putilimp.h"

U_NAMESPACE_BEGIN

#నిర్వచించు DEFAULT_సామర్థ్యం 8

/*
 * Constants for hinting whether a key is an integer
 * or a pointer.  If a hint bit is zero, then the associated
 * token is assumed to be an integer. This is needed for iSeries
 */
 
UOBJECT_DEFINE_RTTI_IMPLEMENTATION(UVector32)

UVector32::UVector32(UError_సంకేతం &స్థితి_2) :
    count(0),
    capacity(0),
    max_సామర్థ్యం(0),
    elements(శూన్యసూచిక)
{
    _init(DEFAULT_సామర్థ్యం, స్థితి_2);
}

UVector32::UVector32(int32_t initial_సామర్థ్యం, UError_సంకేతం &స్థితి_2) :
    count(0),
    capacity(0),
    max_సామర్థ్యం(0),
    elements(శూన్యసూచిక)
{
    _init(initial_సామర్థ్యం, స్థితి_2);
}



శూన్య UVector32::_init(int32_t initial_సామర్థ్యం, UError_సంకేతం &స్థితి_2) {
    // Fix bogus initialCapacity values; avoid malloc(0)
    ఒకవేళ (initial_సామర్థ్యం < 1) {
        initial_సామర్థ్యం = DEFAULT_సామర్థ్యం;
    }
    ఒకవేళ (max_సామర్థ్యం>0 && max_సామర్థ్యం<initial_సామర్థ్యం) {
        initial_సామర్థ్యం = max_సామర్థ్యం;
    }
    ఒకవేళ (initial_సామర్థ్యం > static_cast<int32_t>(INT32_MAX / పరిమాణం(int32_t))) {
        initial_సామర్థ్యం = uprv_min(DEFAULT_సామర్థ్యం, max_సామర్థ్యం);
    }
    elements = static_cast<int32_t*>(uprv_malloc(పరిమాణం(int32_t) * initial_సామర్థ్యం));
    ఒకవేళ (elements == శూన్యసూచిక) {
        స్థితి_2 = U_జ్ఞాపకం_కేటాయింపు_ERROR;
    } లేకపోతే {
        capacity = initial_సామర్థ్యం;
    }
}

UVector32::~UVector32() {
    uprv_free(elements);
    elements = శూన్యసూచిక;
}

/**
 * Assign this object to another (make this a copy of 'other').
 */
శూన్య UVector32::assign(స్థిరరాశి UVector32& other, UError_సంకేతం &ec) {
    ఒకవేళ (ensure_సామర్థ్యం(other.count, ec)) {
        సమితి_కొలత(other.count);
        కొరకు (int32_t i=0; i<other.count; ++i) {
            elements[i] = other.elements[i];
        }
    }
}


తార్కిక UVector32::క్రియాకారకం==(స్థిరరాశి UVector32& other) స్థిరరాశి {
    int32_t i;
    ఒకవేళ (count != other.count) తిరిగి అబద్ధం;
    కొరకు (i=0; i<count; ++i) {
        ఒకవేళ (elements[i] != other.elements[i]) {
            తిరిగి అబద్ధం;
        }
    }
    తిరిగి నిజం;
}


శూన్య UVector32::సమితి_Element_At(int32_t elem, int32_t సూచీ) {
    ఒకవేళ (0 <= సూచీ && సూచీ < count) {
        elements[సూచీ] = elem;
    }
    /* else index out of range */
}

శూన్య UVector32::చొప్పించు_Element_At(int32_t elem, int32_t సూచీ, UError_సంకేతం &స్థితి_2) {
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

UBool UVector32::కలిగియుంది_All(స్థిరరాశి UVector32& other) స్థిరరాశి {
    కొరకు (int32_t i=0; i<other.size(); ++i) {
        ఒకవేళ (సూచీ_యొక్క(other.elements[i]) < 0) {
            తిరిగి అబద్ధం;
        }
    }
    తిరిగి నిజం;
}

UBool UVector32::కలిగియుంది_None(స్థిరరాశి UVector32& other) స్థిరరాశి {
    కొరకు (int32_t i=0; i<other.size(); ++i) {
        ఒకవేళ (సూచీ_యొక్క(other.elements[i]) >= 0) {
            తిరిగి అబద్ధం;
        }
    }
    తిరిగి నిజం;
}

UBool UVector32::removeAll(స్థిరరాశి UVector32& other) {
    UBool మారింది = అబద్ధం;
    కొరకు (int32_t i=0; i<other.size(); ++i) {
        int32_t j = సూచీ_యొక్క(other.elements[i]);
        ఒకవేళ (j >= 0) {
            removeElementAt(j);
            మారింది = నిజం;
        }
    }
    తిరిగి మారింది;
}

UBool UVector32::retainAll(స్థిరరాశి UVector32& other) {
    UBool మారింది = అబద్ధం;
    కొరకు (int32_t j=size()-1; j>=0; --j) {
        int32_t i = other.సూచీ_యొక్క(elements[j]);
        ఒకవేళ (i < 0) {
            removeElementAt(j);
            మారింది = నిజం;
        }
    }
    తిరిగి మారింది;
}

శూన్య UVector32::removeElementAt(int32_t సూచీ) {
    ఒకవేళ (సూచీ >= 0) {
        కొరకు (int32_t i=సూచీ; i<count-1; ++i) {
            elements[i] = elements[i+1];
        }
        --count;
    }
}

శూన్య UVector32::removeAllElements() {
    count = 0;
}

UBool   UVector32::equals(స్థిరరాశి UVector32 &other) స్థిరరాశి {
    పూర్ణాంకం      i;

    ఒకవేళ (ఇది->count != other.count) {
        తిరిగి అబద్ధం;
    }
    కొరకు (i=0; i<count; i++) {
        ఒకవేళ (elements[i] != other.elements[i]) {
            తిరిగి అబద్ధం;
        }
    }
    తిరిగి నిజం;
}




int32_t UVector32::సూచీ_యొక్క(int32_t కీలకం, int32_t ప్రారంభం_సూచీ) స్థిరరాశి {
    int32_t i;
    కొరకు (i=ప్రారంభం_సూచీ; i<count; ++i) {
        ఒకవేళ (కీలకం == elements[i]) {
            తిరిగి i;
        }
    }
    తిరిగి -1;
}


UBool UVector32::expand_సామర్థ్యం(int32_t minimum_సామర్థ్యం, UError_సంకేతం &స్థితి_2) {
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
    ఒకవేళ (newCap > static_cast<int32_t>(INT32_MAX / పరిమాణం(int32_t))) { // integer overflow check
        // We keep the original memory contents on bad minimumCapacity/maxCapacity.
        స్థితి_2 = U_ILLEGAL_వాదం_ERROR;
        తిరిగి అబద్ధం;
    }
    int32_t* newElems = static_cast<int32_t*>(uprv_realloc(elements, పరిమాణం(int32_t) * newCap));
    ఒకవేళ (newElems == శూన్యసూచిక) {
        // We keep the original contents on the memory failure on realloc.
        స్థితి_2 = U_జ్ఞాపకం_కేటాయింపు_ERROR;
        తిరిగి అబద్ధం;
    }
    elements = newElems;
    capacity = newCap;
    తిరిగి నిజం;
}

శూన్య UVector32::సమితి_Max_సామర్థ్యం(int32_t హద్దు) {
    U_ASSERT(హద్దు >= 0);
    ఒకవేళ (హద్దు < 0) {
        హద్దు = 0;
    }
    ఒకవేళ (హద్దు > static_cast<int32_t>(INT32_MAX / పరిమాణం(int32_t))) { // integer overflow check for realloc
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
    int32_t* newElems = static_cast<int32_t*>(uprv_realloc(elements, పరిమాణం(int32_t) * max_సామర్థ్యం));
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
శూన్య UVector32::సమితి_కొలత(int32_t new_కొలత) {
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




/**
 * Insert the given integer into this vector at its sorted position
 * as defined by 'compare'.  The current elements are assumed to
 * be sorted already.
 */
శూన్య UVector32::sorted_చొప్పించు(int32_t tok, UError_సంకేతం& ec) {
    // Perform a binary search for the location to insert tok at.  Tok
    // will be inserted between two elements a and b such that a <=
    // tok && tok < b, where there is a 'virtual' elements[-1] always
    // less than tok and a 'virtual' elements[count] always greater
    // than tok.
    int32_t min = 0, max = count;
    అయితే (min != max) {
        int32_t probe = (min + max) / 2;
        //int8_t c = (*compare)(elements[probe], tok);
        //if (c > 0) {
        ఒకవేళ (elements[probe] > tok) {
            max = probe;
        } లేకపోతే {
            // assert(c <= 0);
            min = probe + 1;
        }
    }
    ఒకవేళ (ensure_సామర్థ్యం(count + 1, ec)) {
        కొరకు (int32_t i=count; i>min; --i) {
            elements[i] = elements[i-1];
        }
        elements[min] = tok;
        ++count;
    }
}





U_NAMESPACE_చివర

