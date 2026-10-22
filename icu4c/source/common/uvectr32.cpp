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

#تعریف_کرو DEFAULT_سمرتھا 8

/*
 * Constants for hinting whether a key is an integer
 * or a pointer.  If a hint bit is zero, then the associated
 * token is assumed to be an integer. This is needed for iSeries
 */
 
UOBJECT_DEFINE_RTTI_IMPLEMENTATION(UVector32)

UVector32::UVector32(UError_کوڈ &حالت) :
    count(0),
    capacity(0),
    max_سمرتھا(0),
    elements(خالی_سوچک)
{
    _init(DEFAULT_سمرتھا, حالت);
}

UVector32::UVector32(int32_t initial_سمرتھا, UError_کوڈ &حالت) :
    count(0),
    capacity(0),
    max_سمرتھا(0),
    elements(خالی_سوچک)
{
    _init(initial_سمرتھا, حالت);
}



فراغ UVector32::_init(int32_t initial_سمرتھا, UError_کوڈ &حالت) {
    // Fix bogus initialCapacity values; avoid malloc(0)
    إذا (initial_سمرتھا < 1) {
        initial_سمرتھا = DEFAULT_سمرتھا;
    }
    إذا (max_سمرتھا>0 && max_سمرتھا<initial_سمرتھا) {
        initial_سمرتھا = max_سمرتھا;
    }
    إذا (initial_سمرتھا > static_cast<int32_t>(INT32_MAX / حجم(int32_t))) {
        initial_سمرتھا = uprv_min(DEFAULT_سمرتھا, max_سمرتھا);
    }
    elements = static_cast<int32_t*>(uprv_malloc(حجم(int32_t) * initial_سمرتھا));
    إذا (elements == خالی_سوچک) {
        حالت = U_حافظہ_ونڈ_ERROR;
    } وإلا {
        capacity = initial_سمرتھا;
    }
}

UVector32::~UVector32() {
    uprv_free(elements);
    elements = خالی_سوچک;
}

/**
 * Assign this object to another (make this a copy of 'other').
 */
فراغ UVector32::assign(ثابت UVector32& other, UError_کوڈ &ec) {
    إذا (ensure_سمرتھا(other.count, ec)) {
        سیٹ_ناپ(other.count);
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


فراغ UVector32::سیٹ_Element_At(int32_t elem, int32_t اشاریہ) {
    إذا (0 <= اشاریہ && اشاریہ < count) {
        elements[اشاریہ] = elem;
    }
    /* else index out of range */
}

فراغ UVector32::داخل_کرو_Element_At(int32_t elem, int32_t اشاریہ, UError_کوڈ &حالت) {
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

UBool UVector32::شامل_اے_All(ثابت UVector32& other) ثابت {
    لكل (int32_t i=0; i<other.size(); ++i) {
        إذا (اشاریہ_دا(other.elements[i]) < 0) {
            أرجع خطأ;
        }
    }
    أرجع سچ;
}

UBool UVector32::شامل_اے_None(ثابت UVector32& other) ثابت {
    لكل (int32_t i=0; i<other.size(); ++i) {
        إذا (اشاریہ_دا(other.elements[i]) >= 0) {
            أرجع خطأ;
        }
    }
    أرجع سچ;
}

UBool UVector32::removeAll(ثابت UVector32& other) {
    UBool بدلیا = خطأ;
    لكل (int32_t i=0; i<other.size(); ++i) {
        int32_t j = اشاریہ_دا(other.elements[i]);
        إذا (j >= 0) {
            removeElementAt(j);
            بدلیا = سچ;
        }
    }
    أرجع بدلیا;
}

UBool UVector32::retainAll(ثابت UVector32& other) {
    UBool بدلیا = خطأ;
    لكل (int32_t j=size()-1; j>=0; --j) {
        int32_t i = other.اشاریہ_دا(elements[j]);
        إذا (i < 0) {
            removeElementAt(j);
            بدلیا = سچ;
        }
    }
    أرجع بدلیا;
}

فراغ UVector32::removeElementAt(int32_t اشاریہ) {
    إذا (اشاریہ >= 0) {
        لكل (int32_t i=اشاریہ; i<count-1; ++i) {
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




int32_t UVector32::اشاریہ_دا(int32_t کلید, int32_t شروع_اشاریہ) ثابت {
    int32_t i;
    لكل (i=شروع_اشاریہ; i<count; ++i) {
        إذا (کلید == elements[i]) {
            أرجع i;
        }
    }
    أرجع -1;
}


UBool UVector32::expand_سمرتھا(int32_t minimum_سمرتھا, UError_کوڈ &حالت) {
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
    إذا (newCap > static_cast<int32_t>(INT32_MAX / حجم(int32_t))) { // integer overflow check
        // We keep the original memory contents on bad minimumCapacity/maxCapacity.
        حالت = U_ILLEGAL_دلیل_ERROR;
        أرجع خطأ;
    }
    int32_t* newElems = static_cast<int32_t*>(uprv_realloc(elements, حجم(int32_t) * newCap));
    إذا (newElems == خالی_سوچک) {
        // We keep the original contents on the memory failure on realloc.
        حالت = U_حافظہ_ونڈ_ERROR;
        أرجع خطأ;
    }
    elements = newElems;
    capacity = newCap;
    أرجع سچ;
}

فراغ UVector32::سیٹ_Max_سمرتھا(int32_t حد_بندی) {
    U_ASSERT(حد_بندی >= 0);
    إذا (حد_بندی < 0) {
        حد_بندی = 0;
    }
    إذا (حد_بندی > static_cast<int32_t>(INT32_MAX / حجم(int32_t))) { // integer overflow check for realloc
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
    int32_t* newElems = static_cast<int32_t*>(uprv_realloc(elements, حجم(int32_t) * max_سمرتھا));
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
فراغ UVector32::سیٹ_ناپ(int32_t new_ناپ) {
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




/**
 * Insert the given integer into this vector at its sorted position
 * as defined by 'compare'.  The current elements are assumed to
 * be sorted already.
 */
فراغ UVector32::sorted_داخل_کرو(int32_t tok, UError_کوڈ& ec) {
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
    إذا (ensure_سمرتھا(count + 1, ec)) {
        لكل (int32_t i=count; i>min; --i) {
            elements[i] = elements[i-1];
        }
        elements[min] = tok;
        ++count;
    }
}





U_NAMESPACE_سرا

