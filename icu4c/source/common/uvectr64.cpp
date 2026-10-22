// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
******************************************************************************
* Copyright (C) 1999-2015, International Business Machines Corporation and
* others. All Rights Reserved.
******************************************************************************
*/

#ضمن "uvectr64.h"
#ضمن "cmemory.h"
#ضمن "putilimp.h"

U_NAMESPACE_BEGIN

#تعریف_کرو DEFAULT_سمرتھا 8

/*
 * Constants for hinting whether a key is an integer
 * or a pointer.  If a hint bit is zero, then the associated
 * token is assumed to be an integer. This is needed for iSeries
 */
 
UOBJECT_DEFINE_RTTI_IMPLEMENTATION(UVector64)

UVector64::UVector64(UError_کوڈ &حالت) :
    count(0),
    capacity(0),
    max_سمرتھا(0),
    elements(خالی_سوچک)
{
    _init(DEFAULT_سمرتھا, حالت);
}

UVector64::UVector64(int32_t initial_سمرتھا, UError_کوڈ &حالت) :
    count(0),
    capacity(0),
    max_سمرتھا(0),
    elements(خالی_سوچک)
{
    _init(initial_سمرتھا, حالت);
}



فراغ UVector64::_init(int32_t initial_سمرتھا, UError_کوڈ &حالت) {
    // Fix bogus initialCapacity values; avoid malloc(0)
    إذا (initial_سمرتھا < 1) {
        initial_سمرتھا = DEFAULT_سمرتھا;
    }
    إذا (max_سمرتھا>0 && max_سمرتھا<initial_سمرتھا) {
        initial_سمرتھا = max_سمرتھا;
    }
    إذا (initial_سمرتھا > static_cast<int32_t>(INT32_MAX / حجم(int64_t))) {
        initial_سمرتھا = uprv_min(DEFAULT_سمرتھا, max_سمرتھا);
    }
    elements = static_cast<int64_t*>(uprv_malloc(حجم(int64_t) * initial_سمرتھا));
    إذا (elements == خالی_سوچک) {
        حالت = U_حافظہ_ونڈ_ERROR;
    } وإلا {
        capacity = initial_سمرتھا;
    }
}

UVector64::~UVector64() {
    uprv_free(elements);
    elements = خالی_سوچک;
}

/**
 * Assign this object to another (make this a copy of 'other').
 */
فراغ UVector64::assign(ثابت UVector64& other, UError_کوڈ &ec) {
    إذا (ensure_سمرتھا(other.count, ec)) {
        سیٹ_ناپ(other.count);
        لكل (int32_t i=0; i<other.count; ++i) {
            elements[i] = other.elements[i];
        }
    }
}


منطقي UVector64::عامل==(ثابت UVector64& other) {
    int32_t i;
    إذا (count != other.count) أرجع خطأ;
    لكل (i=0; i<count; ++i) {
        إذا (elements[i] != other.elements[i]) {
            أرجع خطأ;
        }
    }
    أرجع سچ;
}


فراغ UVector64::سیٹ_Element_At(int64_t elem, int32_t اشاریہ) {
    إذا (0 <= اشاریہ && اشاریہ < count) {
        elements[اشاریہ] = elem;
    }
    /* else index out of range */
}

فراغ UVector64::داخل_کرو_Element_At(int64_t elem, int32_t اشاریہ, UError_کوڈ &حالت) {
    // must have 0 <= index <= count
    إذا (0 <= اشاریہ && اشاریہ <= count && ensure_سمرتھا(count + 1, حالت)) {
        لكل (int32_t i=count; i>اشاریہ; --i) {
            elements[i] = elements[i-1];
        }
        elements[اشاریہ] = elem;
        ++count;
    }
    /* else index out of range */
}

فراغ UVector64::removeAllElements() {
    count = 0;
}

UBool UVector64::expand_سمرتھا(int32_t minimum_سمرتھا, UError_کوڈ &حالت) {
    إذا (U_FAILURE(حالت)) {
        أرجع خطأ;
    }
    إذا (minimum_سمرتھا < 0) {
        حالت = U_ILLEGAL_دلیل_ERROR;
        أرجع خطأ;
    }
    إذا (capacity >= minimum_سمرتھا) {
        أرجع سچ;
    }
    إذا (max_سمرتھا>0 && minimum_سمرتھا>max_سمرتھا) {
        حالت = U_بفر_حد_توں_ودھ_ERROR;
        أرجع خطأ;
    }
    إذا (capacity > (INT32_MAX - 1) / 2) {  // integer overflow check
        حالت = U_ILLEGAL_دلیل_ERROR;
        أرجع خطأ;
    }
    int32_t newCap = capacity * 2;
    إذا (newCap < minimum_سمرتھا) {
        newCap = minimum_سمرتھا;
    }
    إذا (max_سمرتھا > 0 && newCap > max_سمرتھا) {
        newCap = max_سمرتھا;
    }
    إذا (newCap > static_cast<int32_t>(INT32_MAX / حجم(int64_t))) { // integer overflow check
        // We keep the original memory contents on bad minimumCapacity/maxCapacity.
        حالت = U_ILLEGAL_دلیل_ERROR;
        أرجع خطأ;
    }
    int64_t* newElems = static_cast<int64_t*>(uprv_realloc(elements, حجم(int64_t) * newCap));
    إذا (newElems == خالی_سوچک) {
        // We keep the original contents on the memory failure on realloc.
        حالت = U_حافظہ_ونڈ_ERROR;
        أرجع خطأ;
    }
    elements = newElems;
    capacity = newCap;
    أرجع سچ;
}

فراغ UVector64::سیٹ_Max_سمرتھا(int32_t حد_بندی) {
    U_ASSERT(حد_بندی >= 0);
    إذا (حد_بندی < 0) {
        حد_بندی = 0;
    }
    إذا (حد_بندی > static_cast<int32_t>(INT32_MAX / حجم(int64_t))) { // integer overflow check for realloc
        //  Something is very wrong, don't realloc, leave capacity and maxCapacity unchanged
        أرجع;
    }
    max_سمرتھا = حد_بندی;
    إذا (capacity <= max_سمرتھا || max_سمرتھا == 0) {
        // Current capacity is within the new limit.
        أرجع;
    }
    
    // New maximum capacity is smaller than the current size.
    // Realloc the storage to the new, smaller size.
    int64_t* newElems = static_cast<int64_t*>(uprv_realloc(elements, حجم(int64_t) * max_سمرتھا));
    إذا (newElems == خالی_سوچک) {
        // Realloc to smaller failed.
        //   Just keep what we had.  No need to call it a failure.
        أرجع;
    }
    elements = newElems;
    capacity = max_سمرتھا;
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
فراغ UVector64::سیٹ_ناپ(int32_t new_ناپ) {
    int32_t i;
    إذا (new_ناپ < 0) {
        أرجع;
    }
    إذا (new_ناپ > count) {
        UError_کوڈ ec = U_ZERO_ERROR;
        إذا (!ensure_سمرتھا(new_ناپ, ec)) {
            أرجع;
        }
        لكل (i=count; i<new_ناپ; ++i) {
            elements[i] = 0;
        }
    } 
    count = new_ناپ;
}

U_NAMESPACE_سرا

