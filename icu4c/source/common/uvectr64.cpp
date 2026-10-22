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

#تعریف_کړه DEFAULT_ظرفیت 8

/*
 * Constants for hinting whether a key is an integer
 * or a pointer.  If a hint bit is zero, then the associated
 * token is assumed to be an integer. This is needed for iSeries
 */
 
UOBJECT_DEFINE_RTTI_IMPLEMENTATION(UVector64)

UVector64::UVector64(UError_کوډ &حالت) :
    count(0),
    capacity(0),
    max_ظرفیت(0),
    elements(تش_اشاره)
{
    _init(DEFAULT_ظرفیت, حالت);
}

UVector64::UVector64(int32_t initial_ظرفیت, UError_کوډ &حالت) :
    count(0),
    capacity(0),
    max_ظرفیت(0),
    elements(تش_اشاره)
{
    _init(initial_ظرفیت, حالت);
}



فراغ UVector64::_init(int32_t initial_ظرفیت, UError_کوډ &حالت) {
    // Fix bogus initialCapacity values; avoid malloc(0)
    إذا (initial_ظرفیت < 1) {
        initial_ظرفیت = DEFAULT_ظرفیت;
    }
    إذا (max_ظرفیت>0 && max_ظرفیت<initial_ظرفیت) {
        initial_ظرفیت = max_ظرفیت;
    }
    إذا (initial_ظرفیت > static_cast<int32_t>(INT32_MAX / حجم(int64_t))) {
        initial_ظرفیت = uprv_min(DEFAULT_ظرفیت, max_ظرفیت);
    }
    elements = static_cast<int64_t*>(uprv_malloc(حجم(int64_t) * initial_ظرفیت));
    إذا (elements == تش_اشاره) {
        حالت = U_حافظه_ویش_ERROR;
    } وإلا {
        capacity = initial_ظرفیت;
    }
}

UVector64::~UVector64() {
    uprv_free(elements);
    elements = تش_اشاره;
}

/**
 * Assign this object to another (make this a copy of 'other').
 */
فراغ UVector64::assign(ثابت UVector64& other, UError_کوډ &ec) {
    إذا (ensure_ظرفیت(other.count, ec)) {
        سېټ_اندازه(other.count);
        لكل (int32_t i=0; i<other.count; ++i) {
            elements[i] = other.elements[i];
        }
    }
}


منطقي UVector64::چلوونکی==(ثابت UVector64& other) {
    int32_t i;
    إذا (count != other.count) أرجع خطأ;
    لكل (i=0; i<count; ++i) {
        إذا (elements[i] != other.elements[i]) {
            أرجع خطأ;
        }
    }
    أرجع ریښتیا;
}


فراغ UVector64::سېټ_Element_At(int64_t elem, int32_t شاخص) {
    إذا (0 <= شاخص && شاخص < count) {
        elements[شاخص] = elem;
    }
    /* else index out of range */
}

فراغ UVector64::ورننباسه_Element_At(int64_t elem, int32_t شاخص, UError_کوډ &حالت) {
    // must have 0 <= index <= count
    إذا (0 <= شاخص && شاخص <= count && ensure_ظرفیت(count + 1, حالت)) {
        لكل (int32_t i=count; i>شاخص; --i) {
            elements[i] = elements[i-1];
        }
        elements[شاخص] = elem;
        ++count;
    }
    /* else index out of range */
}

فراغ UVector64::removeAllElements() {
    count = 0;
}

UBool UVector64::expand_ظرفیت(int32_t minimum_ظرفیت, UError_کوډ &حالت) {
    إذا (U_FAILURE(حالت)) {
        أرجع خطأ;
    }
    إذا (minimum_ظرفیت < 0) {
        حالت = U_ILLEGAL_دلیل_ERROR;
        أرجع خطأ;
    }
    إذا (capacity >= minimum_ظرفیت) {
        أرجع ریښتیا;
    }
    إذا (max_ظرفیت>0 && minimum_ظرفیت>max_ظرفیت) {
        حالت = U_بفر_ډکېدنه_ERROR;
        أرجع خطأ;
    }
    إذا (capacity > (INT32_MAX - 1) / 2) {  // integer overflow check
        حالت = U_ILLEGAL_دلیل_ERROR;
        أرجع خطأ;
    }
    int32_t newCap = capacity * 2;
    إذا (newCap < minimum_ظرفیت) {
        newCap = minimum_ظرفیت;
    }
    إذا (max_ظرفیت > 0 && newCap > max_ظرفیت) {
        newCap = max_ظرفیت;
    }
    إذا (newCap > static_cast<int32_t>(INT32_MAX / حجم(int64_t))) { // integer overflow check
        // We keep the original memory contents on bad minimumCapacity/maxCapacity.
        حالت = U_ILLEGAL_دلیل_ERROR;
        أرجع خطأ;
    }
    int64_t* newElems = static_cast<int64_t*>(uprv_realloc(elements, حجم(int64_t) * newCap));
    إذا (newElems == تش_اشاره) {
        // We keep the original contents on the memory failure on realloc.
        حالت = U_حافظه_ویش_ERROR;
        أرجع خطأ;
    }
    elements = newElems;
    capacity = newCap;
    أرجع ریښتیا;
}

فراغ UVector64::سېټ_Max_ظرفیت(int32_t حد) {
    U_ASSERT(حد >= 0);
    إذا (حد < 0) {
        حد = 0;
    }
    إذا (حد > static_cast<int32_t>(INT32_MAX / حجم(int64_t))) { // integer overflow check for realloc
        //  Something is very wrong, don't realloc, leave capacity and maxCapacity unchanged
        أرجع;
    }
    max_ظرفیت = حد;
    إذا (capacity <= max_ظرفیت || max_ظرفیت == 0) {
        // Current capacity is within the new limit.
        أرجع;
    }
    
    // New maximum capacity is smaller than the current size.
    // Realloc the storage to the new, smaller size.
    int64_t* newElems = static_cast<int64_t*>(uprv_realloc(elements, حجم(int64_t) * max_ظرفیت));
    إذا (newElems == تش_اشاره) {
        // Realloc to smaller failed.
        //   Just keep what we had.  No need to call it a failure.
        أرجع;
    }
    elements = newElems;
    capacity = max_ظرفیت;
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
فراغ UVector64::سېټ_اندازه(int32_t new_اندازه) {
    int32_t i;
    إذا (new_اندازه < 0) {
        أرجع;
    }
    إذا (new_اندازه > count) {
        UError_کوډ ec = U_ZERO_ERROR;
        إذا (!ensure_ظرفیت(new_اندازه, ec)) {
            أرجع;
        }
        لكل (i=count; i<new_اندازه; ++i) {
            elements[i] = 0;
        }
    } 
    count = new_اندازه;
}

U_NAMESPACE_پای_ټکی

