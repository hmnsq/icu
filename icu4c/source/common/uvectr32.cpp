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

#अन्तर्भाव "uvectr32.h"
#अन्तर्भाव "cmemory.h"
#अन्तर्भाव "putilimp.h"

U_NAMESPACE_BEGIN

#परिभाषय DEFAULT_क्षमता 8

/*
 * Constants for hinting whether a key is an integer
 * or a pointer.  If a hint bit is zero, then the associated
 * token is assumed to be an integer. This is needed for iSeries
 */
 
UOBJECT_DEFINE_RTTI_IMPLEMENTATION(UVector32)

UVector32::UVector32(UError_संहिता &स्थितिः) :
    count(0),
    capacity(0),
    max_क्षमता(0),
    elements(शून्यसूचकः)
{
    _init(DEFAULT_क्षमता, स्थितिः);
}

UVector32::UVector32(int32_t initial_क्षमता, UError_संहिता &स्थितिः) :
    count(0),
    capacity(0),
    max_क्षमता(0),
    elements(शून्यसूचकः)
{
    _init(initial_क्षमता, स्थितिः);
}



शून्य UVector32::_init(int32_t initial_क्षमता, UError_संहिता &स्थितिः) {
    // Fix bogus initialCapacity values; avoid malloc(0)
    यदि (initial_क्षमता < 1) {
        initial_क्षमता = DEFAULT_क्षमता;
    }
    यदि (max_क्षमता>0 && max_क्षमता<initial_क्षमता) {
        initial_क्षमता = max_क्षमता;
    }
    यदि (initial_क्षमता > static_cast<int32_t>(INT32_MAX / परिमाण(int32_t))) {
        initial_क्षमता = uprv_min(DEFAULT_क्षमता, max_क्षमता);
    }
    elements = static_cast<int32_t*>(uprv_malloc(परिमाण(int32_t) * initial_क्षमता));
    यदि (elements == शून्यसूचकः) {
        स्थितिः = U_स्मृतिः_आवण्टनम्_ERROR;
    } अन्यथा {
        capacity = initial_क्षमता;
    }
}

UVector32::~UVector32() {
    uprv_free(elements);
    elements = शून्यसूचकः;
}

/**
 * Assign this object to another (make this a copy of 'other').
 */
शून्य UVector32::assign(ध्रुव UVector32& other, UError_संहिता &ec) {
    यदि (ensure_क्षमता(other.count, ec)) {
        समुच्चयः_आकार(other.count);
        प्रति (int32_t i=0; i<other.count; ++i) {
            elements[i] = other.elements[i];
        }
    }
}


तार्किक UVector32::संक्रियकः==(ध्रुव UVector32& other) ध्रुव {
    int32_t i;
    यदि (count != other.count) प्रत्यागम असत्य;
    प्रति (i=0; i<count; ++i) {
        यदि (elements[i] != other.elements[i]) {
            प्रत्यागम असत्य;
        }
    }
    प्रत्यागम सत्य;
}


शून्य UVector32::समुच्चयः_Element_At(int32_t elem, int32_t सूचकांक) {
    यदि (0 <= सूचकांक && सूचकांक < count) {
        elements[सूचकांक] = elem;
    }
    /* else index out of range */
}

शून्य UVector32::निवेशय_Element_At(int32_t elem, int32_t सूचकांक, UError_संहिता &स्थितिः) {
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

UBool UVector32::अन्तर्भवति_All(ध्रुव UVector32& other) ध्रुव {
    प्रति (int32_t i=0; i<other.size(); ++i) {
        यदि (सूचकांक_सम्बन्धि(other.elements[i]) < 0) {
            प्रत्यागम असत्य;
        }
    }
    प्रत्यागम सत्य;
}

UBool UVector32::अन्तर्भवति_None(ध्रुव UVector32& other) ध्रुव {
    प्रति (int32_t i=0; i<other.size(); ++i) {
        यदि (सूचकांक_सम्बन्धि(other.elements[i]) >= 0) {
            प्रत्यागम असत्य;
        }
    }
    प्रत्यागम सत्य;
}

UBool UVector32::removeAll(ध्रुव UVector32& other) {
    UBool परिवर्तितम् = असत्य;
    प्रति (int32_t i=0; i<other.size(); ++i) {
        int32_t j = सूचकांक_सम्बन्धि(other.elements[i]);
        यदि (j >= 0) {
            removeElementAt(j);
            परिवर्तितम् = सत्य;
        }
    }
    प्रत्यागम परिवर्तितम्;
}

UBool UVector32::retainAll(ध्रुव UVector32& other) {
    UBool परिवर्तितम् = असत्य;
    प्रति (int32_t j=size()-1; j>=0; --j) {
        int32_t i = other.सूचकांक_सम्बन्धि(elements[j]);
        यदि (i < 0) {
            removeElementAt(j);
            परिवर्तितम् = सत्य;
        }
    }
    प्रत्यागम परिवर्तितम्;
}

शून्य UVector32::removeElementAt(int32_t सूचकांक) {
    यदि (सूचकांक >= 0) {
        प्रति (int32_t i=सूचकांक; i<count-1; ++i) {
            elements[i] = elements[i+1];
        }
        --count;
    }
}

शून्य UVector32::removeAllElements() {
    count = 0;
}

UBool   UVector32::equals(ध्रुव UVector32 &other) ध्रुव {
    पूर्णाङ्क      i;

    यदि (एतत्->count != other.count) {
        प्रत्यागम असत्य;
    }
    प्रति (i=0; i<count; i++) {
        यदि (elements[i] != other.elements[i]) {
            प्रत्यागम असत्य;
        }
    }
    प्रत्यागम सत्य;
}




int32_t UVector32::सूचकांक_सम्बन्धि(int32_t कुञ्चिका, int32_t start_सूचकांक) ध्रुव {
    int32_t i;
    प्रति (i=start_सूचकांक; i<count; ++i) {
        यदि (कुञ्चिका == elements[i]) {
            प्रत्यागम i;
        }
    }
    प्रत्यागम -1;
}


UBool UVector32::expand_क्षमता(int32_t minimum_क्षमता, UError_संहिता &स्थितिः) {
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
    यदि (newCap > static_cast<int32_t>(INT32_MAX / परिमाण(int32_t))) { // integer overflow check
        // We keep the original memory contents on bad minimumCapacity/maxCapacity.
        स्थितिः = U_ILLEGAL_तर्कः_ERROR;
        प्रत्यागम असत्य;
    }
    int32_t* newElems = static_cast<int32_t*>(uprv_realloc(elements, परिमाण(int32_t) * newCap));
    यदि (newElems == शून्यसूचकः) {
        // We keep the original contents on the memory failure on realloc.
        स्थितिः = U_स्मृतिः_आवण्टनम्_ERROR;
        प्रत्यागम असत्य;
    }
    elements = newElems;
    capacity = newCap;
    प्रत्यागम सत्य;
}

शून्य UVector32::समुच्चयः_Max_क्षमता(int32_t सीमा) {
    U_ASSERT(सीमा >= 0);
    यदि (सीमा < 0) {
        सीमा = 0;
    }
    यदि (सीमा > static_cast<int32_t>(INT32_MAX / परिमाण(int32_t))) { // integer overflow check for realloc
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
    int32_t* newElems = static_cast<int32_t*>(uprv_realloc(elements, परिमाण(int32_t) * max_क्षमता));
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
शून्य UVector32::समुच्चयः_आकार(int32_t new_आकार) {
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




/**
 * Insert the given integer into this vector at its sorted position
 * as defined by 'compare'.  The current elements are assumed to
 * be sorted already.
 */
शून्य UVector32::sorted_निवेशय(int32_t tok, UError_संहिता& ec) {
    // Perform a binary search for the location to insert tok at.  Tok
    // will be inserted between two elements a and b such that a <=
    // tok && tok < b, where there is a 'virtual' elements[-1] always
    // less than tok and a 'virtual' elements[count] always greater
    // than tok.
    int32_t min = 0, max = count;
    यावत् (min != max) {
        int32_t probe = (min + max) / 2;
        //int8_t c = (*compare)(elements[probe], tok);
        //if (c > 0) {
        यदि (elements[probe] > tok) {
            max = probe;
        } अन्यथा {
            // assert(c <= 0);
            min = probe + 1;
        }
    }
    यदि (ensure_क्षमता(count + 1, ec)) {
        प्रति (int32_t i=count; i>min; --i) {
            elements[i] = elements[i-1];
        }
        elements[min] = tok;
        ++count;
    }
}





U_NAMESPACE_अन्तः

