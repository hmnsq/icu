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

#وصف_ڪريو DEFAULT_صلاحيت 8

/*
 * Constants for hinting whether a key is an integer
 * or a pointer.  If a hint bit is zero, then the associated
 * token is assumed to be an integer. This is needed for iSeries
 */
 
UOBJECT_DEFINE_RTTI_IMPLEMENTATION(UVector32)

UVector32::UVector32(UError_ڪوڊ &حالت) :
    count(0),
    capacity(0),
    max_صلاحيت(0),
    elements(خالي_اشارو)
{
    _init(DEFAULT_صلاحيت, حالت);
}

UVector32::UVector32(int32_t initial_صلاحيت, UError_ڪوڊ &حالت) :
    count(0),
    capacity(0),
    max_صلاحيت(0),
    elements(خالي_اشارو)
{
    _init(initial_صلاحيت, حالت);
}



فراغ UVector32::_init(int32_t initial_صلاحيت, UError_ڪوڊ &حالت) {
    // Fix bogus initialCapacity values; avoid malloc(0)
    إذا (initial_صلاحيت < 1) {
        initial_صلاحيت = DEFAULT_صلاحيت;
    }
    إذا (max_صلاحيت>0 && max_صلاحيت<initial_صلاحيت) {
        initial_صلاحيت = max_صلاحيت;
    }
    إذا (initial_صلاحيت > static_cast<int32_t>(INT32_MAX / حجم(int32_t))) {
        initial_صلاحيت = uprv_min(DEFAULT_صلاحيت, max_صلاحيت);
    }
    elements = static_cast<int32_t*>(uprv_malloc(حجم(int32_t) * initial_صلاحيت));
    إذا (elements == خالي_اشارو) {
        حالت = U_يادگيري_ورهاست_ERROR;
    } وإلا {
        capacity = initial_صلاحيت;
    }
}

UVector32::~UVector32() {
    uprv_free(elements);
    elements = خالي_اشارو;
}

/**
 * Assign this object to another (make this a copy of 'other').
 */
فراغ UVector32::assign(ثابت UVector32& other, UError_ڪوڊ &ec) {
    إذا (ensure_صلاحيت(other.count, ec)) {
        سيٽ_سائيز(other.count);
        لكل (int32_t i=0; i<other.count; ++i) {
            elements[i] = other.elements[i];
        }
    }
}


منطقي UVector32::عامل==(ثابت UVector32& other) ثابت {
    int32_t i;
    إذا (count != other.count) أرجع خطأ;
    لكل (i=0; i<count; ++i) {
        إذا (elements[i] != other.elements[i]) {
            أرجع خطأ;
        }
    }
    أرجع سچ;
}


فراغ UVector32::سيٽ_Element_At(int32_t elem, int32_t اشاريو) {
    إذا (0 <= اشاريو && اشاريو < count) {
        elements[اشاريو] = elem;
    }
    /* else index out of range */
}

فراغ UVector32::وجهو_Element_At(int32_t elem, int32_t اشاريو, UError_ڪوڊ &حالت) {
    // must have 0 <= index <= count
    إذا (0 <= اشاريو && اشاريو <= count && ensure_صلاحيت(count + 1, حالت)) {
        لكل (int32_t i=count; i>اشاريو; --i) {
            elements[i] = elements[i-1];
        }
        elements[اشاريو] = elem;
        ++count;
    }
    /* else index out of range */
}

UBool UVector32::شامل_آهي_All(ثابت UVector32& other) ثابت {
    لكل (int32_t i=0; i<other.size(); ++i) {
        إذا (اشاريو_جو(other.elements[i]) < 0) {
            أرجع خطأ;
        }
    }
    أرجع سچ;
}

UBool UVector32::شامل_آهي_None(ثابت UVector32& other) ثابت {
    لكل (int32_t i=0; i<other.size(); ++i) {
        إذا (اشاريو_جو(other.elements[i]) >= 0) {
            أرجع خطأ;
        }
    }
    أرجع سچ;
}

UBool UVector32::removeAll(ثابت UVector32& other) {
    UBool بدليو = خطأ;
    لكل (int32_t i=0; i<other.size(); ++i) {
        int32_t j = اشاريو_جو(other.elements[i]);
        إذا (j >= 0) {
            removeElementAt(j);
            بدليو = سچ;
        }
    }
    أرجع بدليو;
}

UBool UVector32::retainAll(ثابت UVector32& other) {
    UBool بدليو = خطأ;
    لكل (int32_t j=size()-1; j>=0; --j) {
        int32_t i = other.اشاريو_جو(elements[j]);
        إذا (i < 0) {
            removeElementAt(j);
            بدليو = سچ;
        }
    }
    أرجع بدليو;
}

فراغ UVector32::removeElementAt(int32_t اشاريو) {
    إذا (اشاريو >= 0) {
        لكل (int32_t i=اشاريو; i<count-1; ++i) {
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
    أرجع سچ;
}




int32_t UVector32::اشاريو_جو(int32_t ڪنجي, int32_t شروعات_اشاريو) ثابت {
    int32_t i;
    لكل (i=شروعات_اشاريو; i<count; ++i) {
        إذا (ڪنجي == elements[i]) {
            أرجع i;
        }
    }
    أرجع -1;
}


UBool UVector32::expand_صلاحيت(int32_t minimum_صلاحيت, UError_ڪوڊ &حالت) {
    إذا (U_FAILURE(حالت)) {
        أرجع خطأ;
    }
    إذا (minimum_صلاحيت < 0) {
        حالت = U_ILLEGAL_دليل_ERROR;
        أرجع خطأ;
    }
    إذا (capacity >= minimum_صلاحيت) {
        أرجع سچ;
    }
    إذا (max_صلاحيت>0 && minimum_صلاحيت>max_صلاحيت) {
        حالت = U_بفر_حد_کان_مٿي_ERROR;
        أرجع خطأ;
    }
    إذا (capacity > (INT32_MAX - 1) / 2) {  // integer overflow check
        حالت = U_ILLEGAL_دليل_ERROR;
        أرجع خطأ;
    }
    int32_t newCap = capacity * 2;
    إذا (newCap < minimum_صلاحيت) {
        newCap = minimum_صلاحيت;
    }
    إذا (max_صلاحيت > 0 && newCap > max_صلاحيت) {
        newCap = max_صلاحيت;
    }
    إذا (newCap > static_cast<int32_t>(INT32_MAX / حجم(int32_t))) { // integer overflow check
        // We keep the original memory contents on bad minimumCapacity/maxCapacity.
        حالت = U_ILLEGAL_دليل_ERROR;
        أرجع خطأ;
    }
    int32_t* newElems = static_cast<int32_t*>(uprv_realloc(elements, حجم(int32_t) * newCap));
    إذا (newElems == خالي_اشارو) {
        // We keep the original contents on the memory failure on realloc.
        حالت = U_يادگيري_ورهاست_ERROR;
        أرجع خطأ;
    }
    elements = newElems;
    capacity = newCap;
    أرجع سچ;
}

فراغ UVector32::سيٽ_Max_صلاحيت(int32_t حد_بندي) {
    U_ASSERT(حد_بندي >= 0);
    إذا (حد_بندي < 0) {
        حد_بندي = 0;
    }
    إذا (حد_بندي > static_cast<int32_t>(INT32_MAX / حجم(int32_t))) { // integer overflow check for realloc
        //  Something is very wrong, don't realloc, leave capacity and maxCapacity unchanged
        أرجع;
    }
    max_صلاحيت = حد_بندي;
    إذا (capacity <= max_صلاحيت || max_صلاحيت == 0) {
        // Current capacity is within the new limit.
        أرجع;
    }
    
    // New maximum capacity is smaller than the current size.
    // Realloc the storage to the new, smaller size.
    int32_t* newElems = static_cast<int32_t*>(uprv_realloc(elements, حجم(int32_t) * max_صلاحيت));
    إذا (newElems == خالي_اشارو) {
        // Realloc to smaller failed.
        //   Just keep what we had.  No need to call it a failure.
        أرجع;
    }
    elements = newElems;
    capacity = max_صلاحيت;
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
فراغ UVector32::سيٽ_سائيز(int32_t new_سائيز) {
    int32_t i;
    إذا (new_سائيز < 0) {
        أرجع;
    }
    إذا (new_سائيز > count) {
        UError_ڪوڊ ec = U_ZERO_ERROR;
        إذا (!ensure_صلاحيت(new_سائيز, ec)) {
            أرجع;
        }
        لكل (i=count; i<new_سائيز; ++i) {
            elements[i] = 0;
        }
    } 
    count = new_سائيز;
}




/**
 * Insert the given integer into this vector at its sorted position
 * as defined by 'compare'.  The current elements are assumed to
 * be sorted already.
 */
فراغ UVector32::sorted_وجهو(int32_t tok, UError_ڪوڊ& ec) {
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
    إذا (ensure_صلاحيت(count + 1, ec)) {
        لكل (int32_t i=count; i>min; --i) {
            elements[i] = elements[i-1];
        }
        elements[min] = tok;
        ++count;
    }
}





U_NAMESPACE_پڇاڙي

