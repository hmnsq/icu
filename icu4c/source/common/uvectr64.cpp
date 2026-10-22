// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
******************************************************************************
* Copyright (C) 1999-2015, International Business Machines Corporation and
* others. All Rights Reserved.
******************************************************************************
*/

#شامل "uvectr64.h"
#شامل "cmemory.h"
#شامل "putilimp.h"

U_NAMESPACE_BEGIN

#تعریف_کرو DEFAULT_استعداد 8

/*
 * Constants for hinting whether a key is an integer
 * or a pointer.  If a hint bit is zero, then the associated
 * token is assumed to be an integer. This is needed for iSeries
 */
 
UOBJECT_DEFINE_RTTI_IMPLEMENTATION(UVector64)

UVector64::UVector64(UError_رمز &حالت_2) :
    count(0),
    capacity(0),
    max_استعداد(0),
    elements(خالی_اشاریہ)
{
    _init(DEFAULT_استعداد, حالت_2);
}

UVector64::UVector64(int32_t initial_استعداد, UError_رمز &حالت_2) :
    count(0),
    capacity(0),
    max_استعداد(0),
    elements(خالی_اشاریہ)
{
    _init(initial_استعداد, حالت_2);
}



خالی UVector64::_init(int32_t initial_استعداد, UError_رمز &حالت_2) {
    // Fix bogus initialCapacity values; avoid malloc(0)
    اگر (initial_استعداد < 1) {
        initial_استعداد = DEFAULT_استعداد;
    }
    اگر (max_استعداد>0 && max_استعداد<initial_استعداد) {
        initial_استعداد = max_استعداد;
    }
    اگر (initial_استعداد > static_cast<int32_t>(INT32_MAX / حجم(int64_t))) {
        initial_استعداد = uprv_min(DEFAULT_استعداد, max_استعداد);
    }
    elements = static_cast<int64_t*>(uprv_malloc(حجم(int64_t) * initial_استعداد));
    اگر (elements == خالی_اشاریہ) {
        حالت_2 = U_حافظہ_تخصیص_ERROR;
    } ورنہ {
        capacity = initial_استعداد;
    }
}

UVector64::~UVector64() {
    uprv_free(elements);
    elements = خالی_اشاریہ;
}

/**
 * Assign this object to another (make this a copy of 'other').
 */
خالی UVector64::assign(ثابت UVector64& other, UError_رمز &ec) {
    اگر (ensure_استعداد(other.count, ec)) {
        سیٹ_سائز(other.count);
        برائے (int32_t i=0; i<other.count; ++i) {
            elements[i] = other.elements[i];
        }
    }
}


منطقی UVector64::عامل==(ثابت UVector64& other) {
    int32_t i;
    اگر (count != other.count) واپس جھوٹ;
    برائے (i=0; i<count; ++i) {
        اگر (elements[i] != other.elements[i]) {
            واپس جھوٹ;
        }
    }
    واپس سچ;
}


خالی UVector64::سیٹ_Element_At(int64_t elem, int32_t اشاریہ) {
    اگر (0 <= اشاریہ && اشاریہ < count) {
        elements[اشاریہ] = elem;
    }
    /* else index out of range */
}

خالی UVector64::داخل_کرو_Element_At(int64_t elem, int32_t اشاریہ, UError_رمز &حالت_2) {
    // must have 0 <= index <= count
    اگر (0 <= اشاریہ && اشاریہ <= count && ensure_استعداد(count + 1, حالت_2)) {
        برائے (int32_t i=count; i>اشاریہ; --i) {
            elements[i] = elements[i-1];
        }
        elements[اشاریہ] = elem;
        ++count;
    }
    /* else index out of range */
}

خالی UVector64::removeAllElements() {
    count = 0;
}

UBool UVector64::expand_استعداد(int32_t minimum_استعداد, UError_رمز &حالت_2) {
    اگر (U_FAILURE(حالت_2)) {
        واپس جھوٹ;
    }
    اگر (minimum_استعداد < 0) {
        حالت_2 = U_ILLEGAL_دلیل_ERROR;
        واپس جھوٹ;
    }
    اگر (capacity >= minimum_استعداد) {
        واپس سچ;
    }
    اگر (max_استعداد>0 && minimum_استعداد>max_استعداد) {
        حالت_2 = U_بفر_حد_سے_تجاوز_ERROR;
        واپس جھوٹ;
    }
    اگر (capacity > (INT32_MAX - 1) / 2) {  // integer overflow check
        حالت_2 = U_ILLEGAL_دلیل_ERROR;
        واپس جھوٹ;
    }
    int32_t newCap = capacity * 2;
    اگر (newCap < minimum_استعداد) {
        newCap = minimum_استعداد;
    }
    اگر (max_استعداد > 0 && newCap > max_استعداد) {
        newCap = max_استعداد;
    }
    اگر (newCap > static_cast<int32_t>(INT32_MAX / حجم(int64_t))) { // integer overflow check
        // We keep the original memory contents on bad minimumCapacity/maxCapacity.
        حالت_2 = U_ILLEGAL_دلیل_ERROR;
        واپس جھوٹ;
    }
    int64_t* newElems = static_cast<int64_t*>(uprv_realloc(elements, حجم(int64_t) * newCap));
    اگر (newElems == خالی_اشاریہ) {
        // We keep the original contents on the memory failure on realloc.
        حالت_2 = U_حافظہ_تخصیص_ERROR;
        واپس جھوٹ;
    }
    elements = newElems;
    capacity = newCap;
    واپس سچ;
}

خالی UVector64::سیٹ_Max_استعداد(int32_t حد_بندی) {
    U_ASSERT(حد_بندی >= 0);
    اگر (حد_بندی < 0) {
        حد_بندی = 0;
    }
    اگر (حد_بندی > static_cast<int32_t>(INT32_MAX / حجم(int64_t))) { // integer overflow check for realloc
        //  Something is very wrong, don't realloc, leave capacity and maxCapacity unchanged
        واپس;
    }
    max_استعداد = حد_بندی;
    اگر (capacity <= max_استعداد || max_استعداد == 0) {
        // Current capacity is within the new limit.
        واپس;
    }
    
    // New maximum capacity is smaller than the current size.
    // Realloc the storage to the new, smaller size.
    int64_t* newElems = static_cast<int64_t*>(uprv_realloc(elements, حجم(int64_t) * max_استعداد));
    اگر (newElems == خالی_اشاریہ) {
        // Realloc to smaller failed.
        //   Just keep what we had.  No need to call it a failure.
        واپس;
    }
    elements = newElems;
    capacity = max_استعداد;
    اگر (count > capacity) {
        count = capacity;
    }
}

/**
 * Change the size of this vector as follows: If newSize is smaller,
 * then truncate the array, possibly deleting held elements for i >=
 * newSize.  If newSize is larger, grow the array, filling in new
 * slots with nullptr.
 */
خالی UVector64::سیٹ_سائز(int32_t new_سائز) {
    int32_t i;
    اگر (new_سائز < 0) {
        واپس;
    }
    اگر (new_سائز > count) {
        UError_رمز ec = U_ZERO_ERROR;
        اگر (!ensure_استعداد(new_سائز, ec)) {
            واپس;
        }
        برائے (i=count; i<new_سائز; ++i) {
            elements[i] = 0;
        }
    } 
    count = new_سائز;
}

U_NAMESPACE_انتہا

