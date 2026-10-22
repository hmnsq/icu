// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
******************************************************************************
* Copyright (C) 1999-2015, International Business Machines Corporation and
* others. All Rights Reserved.
******************************************************************************
*/

#अन्तर्भाव "uvectr64.h"
#अन्तर्भाव "cmemory.h"
#अन्तर्भाव "putilimp.h"

U_NAMESPACE_BEGIN

#परिभाषय DEFAULT_क्षमता 8

/*
 * Constants for hinting whether a key is an integer
 * or a pointer.  If a hint bit is zero, then the associated
 * token is assumed to be an integer. This is needed for iSeries
 */
 
UOBJECT_DEFINE_RTTI_IMPLEMENTATION(UVector64)

UVector64::UVector64(UError_संहिता &स्थितिः) :
    count(0),
    capacity(0),
    max_क्षमता(0),
    elements(शून्यसूचकः)
{
    _init(DEFAULT_क्षमता, स्थितिः);
}

UVector64::UVector64(int32_t initial_क्षमता, UError_संहिता &स्थितिः) :
    count(0),
    capacity(0),
    max_क्षमता(0),
    elements(शून्यसूचकः)
{
    _init(initial_क्षमता, स्थितिः);
}



शून्य UVector64::_init(int32_t initial_क्षमता, UError_संहिता &स्थितिः) {
    // Fix bogus initialCapacity values; avoid malloc(0)
    यदि (initial_क्षमता < 1) {
        initial_क्षमता = DEFAULT_क्षमता;
    }
    यदि (max_क्षमता>0 && max_क्षमता<initial_क्षमता) {
        initial_क्षमता = max_क्षमता;
    }
    यदि (initial_क्षमता > static_cast<int32_t>(INT32_MAX / परिमाण(int64_t))) {
        initial_क्षमता = uprv_min(DEFAULT_क्षमता, max_क्षमता);
    }
    elements = static_cast<int64_t*>(uprv_malloc(परिमाण(int64_t) * initial_क्षमता));
    यदि (elements == शून्यसूचकः) {
        स्थितिः = U_स्मृतिः_आवण्टनम्_ERROR;
    } अन्यथा {
        capacity = initial_क्षमता;
    }
}

UVector64::~UVector64() {
    uprv_free(elements);
    elements = शून्यसूचकः;
}

/**
 * Assign this object to another (make this a copy of 'other').
 */
शून्य UVector64::assign(ध्रुव UVector64& other, UError_संहिता &ec) {
    यदि (ensure_क्षमता(other.count, ec)) {
        समुच्चयः_आकार(other.count);
        प्रति (int32_t i=0; i<other.count; ++i) {
            elements[i] = other.elements[i];
        }
    }
}


तार्किक UVector64::संक्रियकः==(ध्रुव UVector64& other) {
    int32_t i;
    यदि (count != other.count) प्रत्यागम असत्य;
    प्रति (i=0; i<count; ++i) {
        यदि (elements[i] != other.elements[i]) {
            प्रत्यागम असत्य;
        }
    }
    प्रत्यागम सत्य;
}


शून्य UVector64::समुच्चयः_Element_At(int64_t elem, int32_t सूचकांक) {
    यदि (0 <= सूचकांक && सूचकांक < count) {
        elements[सूचकांक] = elem;
    }
    /* else index out of range */
}

शून्य UVector64::निवेशय_Element_At(int64_t elem, int32_t सूचकांक, UError_संहिता &स्थितिः) {
    // must have 0 <= index <= count
    यदि (0 <= सूचकांक && सूचकांक <= count && ensure_क्षमता(count + 1, स्थितिः)) {
        प्रति (int32_t i=count; i>सूचकांक; --i) {
            elements[i] = elements[i-1];
        }
        elements[सूचकांक] = elem;
        ++count;
    }
    /* else index out of range */
}

शून्य UVector64::removeAllElements() {
    count = 0;
}

UBool UVector64::expand_क्षमता(int32_t minimum_क्षमता, UError_संहिता &स्थितिः) {
    यदि (U_FAILURE(स्थितिः)) {
        प्रत्यागम असत्य;
    }
    यदि (minimum_क्षमता < 0) {
        स्थितिः = U_ILLEGAL_तर्कः_ERROR;
        प्रत्यागम असत्य;
    }
    यदि (capacity >= minimum_क्षमता) {
        प्रत्यागम सत्य;
    }
    यदि (max_क्षमता>0 && minimum_क्षमता>max_क्षमता) {
        स्थितिः = U_बफर_अतिप्रवाहः_ERROR;
        प्रत्यागम असत्य;
    }
    यदि (capacity > (INT32_MAX - 1) / 2) {  // integer overflow check
        स्थितिः = U_ILLEGAL_तर्कः_ERROR;
        प्रत्यागम असत्य;
    }
    int32_t newCap = capacity * 2;
    यदि (newCap < minimum_क्षमता) {
        newCap = minimum_क्षमता;
    }
    यदि (max_क्षमता > 0 && newCap > max_क्षमता) {
        newCap = max_क्षमता;
    }
    यदि (newCap > static_cast<int32_t>(INT32_MAX / परिमाण(int64_t))) { // integer overflow check
        // We keep the original memory contents on bad minimumCapacity/maxCapacity.
        स्थितिः = U_ILLEGAL_तर्कः_ERROR;
        प्रत्यागम असत्य;
    }
    int64_t* newElems = static_cast<int64_t*>(uprv_realloc(elements, परिमाण(int64_t) * newCap));
    यदि (newElems == शून्यसूचकः) {
        // We keep the original contents on the memory failure on realloc.
        स्थितिः = U_स्मृतिः_आवण्टनम्_ERROR;
        प्रत्यागम असत्य;
    }
    elements = newElems;
    capacity = newCap;
    प्रत्यागम सत्य;
}

शून्य UVector64::समुच्चयः_Max_क्षमता(int32_t सीमा) {
    U_ASSERT(सीमा >= 0);
    यदि (सीमा < 0) {
        सीमा = 0;
    }
    यदि (सीमा > static_cast<int32_t>(INT32_MAX / परिमाण(int64_t))) { // integer overflow check for realloc
        //  Something is very wrong, don't realloc, leave capacity and maxCapacity unchanged
        प्रत्यागम;
    }
    max_क्षमता = सीमा;
    यदि (capacity <= max_क्षमता || max_क्षमता == 0) {
        // Current capacity is within the new limit.
        प्रत्यागम;
    }
    
    // New maximum capacity is smaller than the current size.
    // Realloc the storage to the new, smaller size.
    int64_t* newElems = static_cast<int64_t*>(uprv_realloc(elements, परिमाण(int64_t) * max_क्षमता));
    यदि (newElems == शून्यसूचकः) {
        // Realloc to smaller failed.
        //   Just keep what we had.  No need to call it a failure.
        प्रत्यागम;
    }
    elements = newElems;
    capacity = max_क्षमता;
    यदि (count > capacity) {
        count = capacity;
    }
}

/**
 * Change the size of this vector as follows: If newSize is smaller,
 * then truncate the array, possibly deleting held elements for i >=
 * newSize.  If newSize is larger, grow the array, filling in new
 * slots with nullptr.
 */
शून्य UVector64::समुच्चयः_आकार(int32_t new_आकार) {
    int32_t i;
    यदि (new_आकार < 0) {
        प्रत्यागम;
    }
    यदि (new_आकार > count) {
        UError_संहिता ec = U_ZERO_ERROR;
        यदि (!ensure_क्षमता(new_आकार, ec)) {
            प्रत्यागम;
        }
        प्रति (i=count; i<new_आकार; ++i) {
            elements[i] = 0;
        }
    } 
    count = new_आकार;
}

U_NAMESPACE_अन्तः

