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

#تعریف_کړه DEFAULT_ظرفیت 8

/*
 * Constants for hinting whether a key is an integer
 * or a pointer.  If a hint bit is zero, then the associated
 * token is assumed to be an integer. This is needed for iSeries
 */
 
UOBJECT_DEFINE_RTTI_IMPLEMENTATION(UVector32)

UVector32::UVector32(UError_کوډ &حالت) :
    count(0),
    capacity(0),
    max_ظرفیت(0),
    elements(تش_اشاره)
{
    _init(DEFAULT_ظرفیت, حالت);
}

UVector32::UVector32(int32_t initial_ظرفیت, UError_کوډ &حالت) :
    count(0),
    capacity(0),
    max_ظرفیت(0),
    elements(تش_اشاره)
{
    _init(initial_ظرفیت, حالت);
}



فراغ UVector32::_init(int32_t initial_ظرفیت, UError_کوډ &حالت) {
    // Fix bogus initialCapacity values; avoid malloc(0)
    إذا (initial_ظرفیت < 1) {
        initial_ظرفیت = DEFAULT_ظرفیت;
    }
    إذا (max_ظرفیت>0 && max_ظرفیت<initial_ظرفیت) {
        initial_ظرفیت = max_ظرفیت;
    }
    إذا (initial_ظرفیت > static_cast<int32_t>(INT32_MAX / حجم(int32_t))) {
        initial_ظرفیت = uprv_min(DEFAULT_ظرفیت, max_ظرفیت);
    }
    elements = static_cast<int32_t*>(uprv_malloc(حجم(int32_t) * initial_ظرفیت));
    إذا (elements == تش_اشاره) {
        حالت = U_حافظه_ویش_ERROR;
    } وإلا {
        capacity = initial_ظرفیت;
    }
}

UVector32::~UVector32() {
    uprv_free(elements);
    elements = تش_اشاره;
}

/**
 * Assign this object to another (make this a copy of 'other').
 */
فراغ UVector32::assign(ثابت UVector32& other, UError_کوډ &ec) {
    إذا (ensure_ظرفیت(other.count, ec)) {
        سېټ_اندازه(other.count);
        لكل (int32_t i=0; i<other.count; ++i) {
            elements[i] = other.elements[i];
        }
    }
}


منطقي UVector32::چلوونکی==(ثابت UVector32& other) ثابت {
    int32_t i;
    إذا (count != other.count) أرجع خطأ;
    لكل (i=0; i<count; ++i) {
        إذا (elements[i] != other.elements[i]) {
            أرجع خطأ;
        }
    }
    أرجع ریښتیا;
}


فراغ UVector32::سېټ_Element_At(int32_t elem, int32_t شاخص) {
    إذا (0 <= شاخص && شاخص < count) {
        elements[شاخص] = elem;
    }
    /* else index out of range */
}

فراغ UVector32::ورننباسه_Element_At(int32_t elem, int32_t شاخص, UError_کوډ &حالت) {
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

UBool UVector32::لري_All(ثابت UVector32& other) ثابت {
    لكل (int32_t i=0; i<other.size(); ++i) {
        إذا (شاخص_د(other.elements[i]) < 0) {
            أرجع خطأ;
        }
    }
    أرجع ریښتیا;
}

UBool UVector32::لري_None(ثابت UVector32& other) ثابت {
    لكل (int32_t i=0; i<other.size(); ++i) {
        إذا (شاخص_د(other.elements[i]) >= 0) {
            أرجع خطأ;
        }
    }
    أرجع ریښتیا;
}

UBool UVector32::removeAll(ثابت UVector32& other) {
    UBool بدل_شوی = خطأ;
    لكل (int32_t i=0; i<other.size(); ++i) {
        int32_t j = شاخص_د(other.elements[i]);
        إذا (j >= 0) {
            removeElementAt(j);
            بدل_شوی = ریښتیا;
        }
    }
    أرجع بدل_شوی;
}

UBool UVector32::retainAll(ثابت UVector32& other) {
    UBool بدل_شوی = خطأ;
    لكل (int32_t j=size()-1; j>=0; --j) {
        int32_t i = other.شاخص_د(elements[j]);
        إذا (i < 0) {
            removeElementAt(j);
            بدل_شوی = ریښتیا;
        }
    }
    أرجع بدل_شوی;
}

فراغ UVector32::removeElementAt(int32_t شاخص) {
    إذا (شاخص >= 0) {
        لكل (int32_t i=شاخص; i<count-1; ++i) {
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
    أرجع ریښتیا;
}




int32_t UVector32::شاخص_د(int32_t کلی, int32_t پیل_شاخص) ثابت {
    int32_t i;
    لكل (i=پیل_شاخص; i<count; ++i) {
        إذا (کلی == elements[i]) {
            أرجع i;
        }
    }
    أرجع -1;
}


UBool UVector32::expand_ظرفیت(int32_t minimum_ظرفیت, UError_کوډ &حالت) {
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
    إذا (newCap > static_cast<int32_t>(INT32_MAX / حجم(int32_t))) { // integer overflow check
        // We keep the original memory contents on bad minimumCapacity/maxCapacity.
        حالت = U_ILLEGAL_دلیل_ERROR;
        أرجع خطأ;
    }
    int32_t* newElems = static_cast<int32_t*>(uprv_realloc(elements, حجم(int32_t) * newCap));
    إذا (newElems == تش_اشاره) {
        // We keep the original contents on the memory failure on realloc.
        حالت = U_حافظه_ویش_ERROR;
        أرجع خطأ;
    }
    elements = newElems;
    capacity = newCap;
    أرجع ریښتیا;
}

فراغ UVector32::سېټ_Max_ظرفیت(int32_t حد) {
    U_ASSERT(حد >= 0);
    إذا (حد < 0) {
        حد = 0;
    }
    إذا (حد > static_cast<int32_t>(INT32_MAX / حجم(int32_t))) { // integer overflow check for realloc
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
    int32_t* newElems = static_cast<int32_t*>(uprv_realloc(elements, حجم(int32_t) * max_ظرفیت));
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
فراغ UVector32::سېټ_اندازه(int32_t new_اندازه) {
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




/**
 * Insert the given integer into this vector at its sorted position
 * as defined by 'compare'.  The current elements are assumed to
 * be sorted already.
 */
فراغ UVector32::sorted_ورننباسه(int32_t tok, UError_کوډ& ec) {
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
    إذا (ensure_ظرفیت(count + 1, ec)) {
        لكل (int32_t i=count; i>min; --i) {
            elements[i] = elements[i-1];
        }
        elements[min] = tok;
        ++count;
    }
}





U_NAMESPACE_پای_ټکی

