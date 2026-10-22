// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
******************************************************************************
* Copyright (C) 1999-2013, International Business Machines Corporation and
* others. All Rights Reserved.
******************************************************************************
*   Date        Name        Description
*   10/22/99    alan        Creation.
**********************************************************************
*/

#अन्तर्भाव "uvector.h"
#अन्तर्भाव "cmemory.h"
#अन्तर्भाव "uarrsort.h"
#अन्तर्भाव "uelement.h"

U_NAMESPACE_BEGIN

constexpr int32_t DEFAULT_क्षमता = 8;

/*
 * Constants for hinting whether a key is an integer
 * or a pointer.  If a hint bit is zero, then the associated
 * token is assumed to be an integer. This is needed for iSeries
 */
constexpr int8_t HINT_कुञ्चिका_POINTER = 1;
constexpr int8_t HINT_कुञ्चिका_INTEGER = 0;
 
UOBJECT_DEFINE_RTTI_IMPLEMENTATION(UVector)

UVector::UVector(UError_संहिता &स्थितिः) :
        UVector(शून्यसूचकः, शून्यसूचकः, DEFAULT_क्षमता, स्थितिः) {
}

UVector::UVector(int32_t initial_क्षमता, UError_संहिता &स्थितिः) :
        UVector(शून्यसूचकः, शून्यसूचकः, initial_क्षमता, स्थितिः) {
}

UVector::UVector(UObjectDeleter *d, UElementsAreEqual *c, UError_संहिता &स्थितिः) :
        UVector(d, c, DEFAULT_क्षमता, स्थितिः) {
}

UVector::UVector(UObjectDeleter *d, UElementsAreEqual *c, int32_t initial_क्षमता, UError_संहिता &स्थितिः) :
    deleter(d),
    comparer(c)
{
    यदि (U_FAILURE(स्थितिः)) {
        प्रत्यागम;
    }
    // Fix bogus initialCapacity values; avoid malloc(0) and integer overflow
    यदि ((initial_क्षमता < 1) || (initial_क्षमता > static_cast<int32_t>(INT32_MAX / परिमाण(UElement)))) {
        initial_क्षमता = DEFAULT_क्षमता;
    }
    elements = static_cast<UElement*>(uprv_malloc(परिमाण(UElement) * initial_क्षमता));
    यदि (elements == शून्यसूचकः) {
        स्थितिः = U_स्मृतिः_आवण्टनम्_ERROR;
    } अन्यथा {
        capacity = initial_क्षमता;
    }
}

UVector::~UVector() {
    removeAllElements();
    uprv_free(elements);
    elements = शून्यसूचकः;
}

/**
 * Assign this object to another (make this a copy of 'other').
 * Use the 'assign' function to assign each element.
 */
शून्य UVector::assign(ध्रुव UVector& other, UElementAssigner *assign, UError_संहिता &ec) {
    यदि (ensure_क्षमता(other.count, ec)) {
        समुच्चयः_आकार(other.count, ec);
        यदि (U_SUCCESS(ec)) {
            प्रति (int32_t i=0; i<other.count; ++i) {
                यदि (elements[i].pointer != शून्यसूचकः && deleter != शून्यसूचकः) {
                    (*deleter)(elements[i].pointer);
                }
                (*assign)(&elements[i], &other.elements[i]);
            }
        }
    }
}

// This only does something sensible if this object has a non-null comparer
तार्किक UVector::संक्रियकः==(ध्रुव UVector& other) ध्रुव {
    U_ASSERT(comparer != शून्यसूचकः);
    यदि (count != other.count) प्रत्यागम असत्य;
    यदि (comparer != शून्यसूचकः) {
        // Compare using this object's comparer
        प्रति (int32_t i=0; i<count; ++i) {
            यदि (!(*comparer)(elements[i], other.elements[i])) {
                प्रत्यागम असत्य;
            }
        }
    }
    प्रत्यागम सत्य;
}

शून्य UVector::सङ्कलय_Element(शून्य* obj, UError_संहिता &स्थितिः) {
    U_ASSERT(deleter == शून्यसूचकः);
    यदि (ensure_क्षमता(count + 1, स्थितिः)) {
        elements[count++].pointer = obj;
    }
}

शून्य UVector::adoptElement(शून्य* obj, UError_संहिता &स्थितिः) {
    U_ASSERT(deleter != शून्यसूचकः);
    यदि (ensure_क्षमता(count + 1, स्थितिः)) {
        elements[count++].pointer = obj;
    } अन्यथा {
        (*deleter)(obj);
    }
}
शून्य UVector::सङ्कलय_Element(int32_t elem, UError_संहिता &स्थितिः) {
    U_ASSERT(deleter == शून्यसूचकः);  // Usage error. Mixing up ints and pointers.
    यदि (ensure_क्षमता(count + 1, स्थितिः)) {
        elements[count].pointer = शून्यसूचकः;     // Pointers may be bigger than ints.
        elements[count].integer = elem;
        count++;
    }
}

शून्य UVector::समुच्चयः_Element_At(शून्य* obj, int32_t सूचकांक) {
    यदि (0 <= सूचकांक && सूचकांक < count) {
        यदि (elements[सूचकांक].pointer != शून्यसूचकः && deleter != शून्यसूचकः) {
            (*deleter)(elements[सूचकांक].pointer);
        }
        elements[सूचकांक].pointer = obj;
    } अन्यथा {
        /* index out of range */
        यदि (deleter != शून्यसूचकः) {
            (*deleter)(obj);
        }
    }
}

शून्य UVector::समुच्चयः_Element_At(int32_t elem, int32_t सूचकांक) {
    U_ASSERT(deleter == शून्यसूचकः);  // Usage error. Mixing up ints and pointers.
    यदि (0 <= सूचकांक && सूचकांक < count) {
        elements[सूचकांक].pointer = शून्यसूचकः;
        elements[सूचकांक].integer = elem;
    }
    /* else index out of range */
}

शून्य UVector::निवेशय_Element_At(शून्य* obj, int32_t सूचकांक, UError_संहिता &स्थितिः) {
    यदि (ensure_क्षमता(count + 1, स्थितिः)) {
        यदि (0 <= सूचकांक && सूचकांक <= count) {
            प्रति (int32_t i=count; i>सूचकांक; --i) {
                elements[i] = elements[i-1];
            }
            elements[सूचकांक].pointer = obj;
            ++count;
        } अन्यथा {
            /* index out of range */
            स्थितिः = U_ILLEGAL_तर्कः_ERROR;
        }
    }
    यदि (U_FAILURE(स्थितिः) && deleter != शून्यसूचकः) {
        (*deleter)(obj);
    }
}

शून्य UVector::निवेशय_Element_At(int32_t elem, int32_t सूचकांक, UError_संहिता &स्थितिः) {
    U_ASSERT(deleter == शून्यसूचकः);  // Usage error. Mixing up ints and pointers.
    // must have 0 <= index <= count
    यदि (ensure_क्षमता(count + 1, स्थितिः)) {
        यदि (0 <= सूचकांक && सूचकांक <= count) {
            प्रति (int32_t i=count; i>सूचकांक; --i) {
                elements[i] = elements[i-1];
            }
            elements[सूचकांक].pointer = शून्यसूचकः;
            elements[सूचकांक].integer = elem;
            ++count;
        } अन्यथा {
            /* index out of range */
            स्थितिः = U_ILLEGAL_तर्कः_ERROR;
        }
    }
}

शून्य* UVector::elementAt(int32_t सूचकांक) ध्रुव {
    प्रत्यागम (0 <= सूचकांक && सूचकांक < count) ? elements[सूचकांक].pointer : शून्यसूचकः;
}

int32_t UVector::elementAti(int32_t सूचकांक) ध्रुव {
    प्रत्यागम (0 <= सूचकांक && सूचकांक < count) ? elements[सूचकांक].integer : 0;
}

UBool UVector::अन्तर्भवति_All(ध्रुव UVector& other) ध्रुव {
    प्रति (int32_t i=0; i<other.size(); ++i) {
        यदि (सूचकांक_सम्बन्धि(other.elements[i]) < 0) {
            प्रत्यागम असत्य;
        }
    }
    प्रत्यागम सत्य;
}

UBool UVector::अन्तर्भवति_None(ध्रुव UVector& other) ध्रुव {
    प्रति (int32_t i=0; i<other.size(); ++i) {
        यदि (सूचकांक_सम्बन्धि(other.elements[i]) >= 0) {
            प्रत्यागम असत्य;
        }
    }
    प्रत्यागम सत्य;
}

UBool UVector::removeAll(ध्रुव UVector& other) {
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

UBool UVector::retainAll(ध्रुव UVector& other) {
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

शून्य UVector::removeElementAt(int32_t सूचकांक) {
    शून्य* e = orphanElementAt(सूचकांक);
    यदि (e != शून्यसूचकः && deleter != शून्यसूचकः) {
        (*deleter)(e);
    }
}

UBool UVector::removeElement(शून्य* obj) {
    int32_t i = सूचकांक_सम्बन्धि(obj);
    यदि (i >= 0) {
        removeElementAt(i);
        प्रत्यागम सत्य;
    }
    प्रत्यागम असत्य;
}

शून्य UVector::removeAllElements() {
    यदि (deleter != शून्यसूचकः) {
        प्रति (int32_t i=0; i<count; ++i) {
            यदि (elements[i].pointer != शून्यसूचकः) {
                (*deleter)(elements[i].pointer);
            }
        }
    }
    count = 0;
}

UBool   UVector::equals(ध्रुव UVector &other) ध्रुव {
    पूर्णाङ्क      i;

    यदि (एतत्->count != other.count) {
        प्रत्यागम असत्य;
    }
    यदि (comparer == शून्यसूचकः) {
        प्रति (i=0; i<count; i++) {
            यदि (elements[i].pointer != other.elements[i].pointer) {
                प्रत्यागम असत्य;
            }
        }
    } अन्यथा {
        UElement कुञ्चिका;
        प्रति (i=0; i<count; i++) {
            कुञ्चिका.pointer = &other.elements[i];
            यदि (!(*comparer)(कुञ्चिका, elements[i])) {
                प्रत्यागम असत्य;
            }
        }
    }
    प्रत्यागम सत्य;
}



int32_t UVector::सूचकांक_सम्बन्धि(शून्य* obj, int32_t start_सूचकांक) ध्रुव {
    UElement कुञ्चिका;
    कुञ्चिका.pointer = obj;
    प्रत्यागम सूचकांक_सम्बन्धि(कुञ्चिका, start_सूचकांक, HINT_कुञ्चिका_POINTER);
}

int32_t UVector::सूचकांक_सम्बन्धि(int32_t obj, int32_t start_सूचकांक) ध्रुव {
    UElement कुञ्चिका;
    कुञ्चिका.integer = obj;
    प्रत्यागम सूचकांक_सम्बन्धि(कुञ्चिका, start_सूचकांक, HINT_कुञ्चिका_INTEGER);
}

int32_t UVector::सूचकांक_सम्बन्धि(UElement कुञ्चिका, int32_t start_सूचकांक, int8_t hint) ध्रुव {
    यदि (comparer != शून्यसूचकः) {
        प्रति (int32_t i=start_सूचकांक; i<count; ++i) {
            यदि ((*comparer)(कुञ्चिका, elements[i])) {
                प्रत्यागम i;
            }
        }
    } अन्यथा {
        प्रति (int32_t i=start_सूचकांक; i<count; ++i) {
            /* Pointers are not always the same size as ints so to perform
             * a valid comparison we need to know whether we are being
             * provided an int or a pointer. */
            यदि (hint & HINT_कुञ्चिका_POINTER) {
                यदि (कुञ्चिका.pointer == elements[i].pointer) {
                    प्रत्यागम i;
                }
            } अन्यथा {
                यदि (कुञ्चिका.integer == elements[i].integer) {
                    प्रत्यागम i;
                }
            }
        }
    }
    प्रत्यागम -1;
}

UBool UVector::ensure_क्षमता(int32_t minimum_क्षमता, UError_संहिता &स्थितिः) {
    यदि (U_FAILURE(स्थितिः)) {
        प्रत्यागम असत्य;
    }
    यदि (minimum_क्षमता < 0) {
        स्थितिः = U_ILLEGAL_तर्कः_ERROR;
        प्रत्यागम असत्य;
    }
    यदि (capacity < minimum_क्षमता) {
        यदि (capacity > (INT32_MAX - 1) / 2) {        	// integer overflow check
            स्थितिः = U_ILLEGAL_तर्कः_ERROR;
            प्रत्यागम असत्य;
        }
        int32_t newCap = capacity * 2;
        यदि (newCap < minimum_क्षमता) {
            newCap = minimum_क्षमता;
        }
        यदि (newCap > static_cast<int32_t>(INT32_MAX / परिमाण(UElement))) { // integer overflow check
            // We keep the original memory contents on bad minimumCapacity.
            स्थितिः = U_ILLEGAL_तर्कः_ERROR;
            प्रत्यागम असत्य;
        }
        UElement* newElems = static_cast<UElement*>(uprv_realloc(elements, परिमाण(UElement) * newCap));
        यदि (newElems == शून्यसूचकः) {
            // We keep the original contents on the memory failure on realloc or bad minimumCapacity.
            स्थितिः = U_स्मृतिः_आवण्टनम्_ERROR;
            प्रत्यागम असत्य;
        }
        elements = newElems;
        capacity = newCap;
    }
    प्रत्यागम सत्य;
}

/**
 * Change the size of this vector as follows: If newSize is smaller,
 * then truncate the array, possibly deleting held elements for i >=
 * newSize.  If newSize is larger, grow the array, filling in new
 * slots with nullptr.
 */
शून्य UVector::समुच्चयः_आकार(int32_t new_आकार, UError_संहिता &स्थितिः) {
    यदि (!ensure_क्षमता(new_आकार, स्थितिः)) {
        प्रत्यागम;
    }
    यदि (new_आकार > count) {
        UElement empty;
        empty.pointer = शून्यसूचकः;
        empty.integer = 0;
        प्रति (int32_t i=count; i<new_आकार; ++i) {
            elements[i] = empty;
        }
    } अन्यथा {
        /* Most efficient to count down */
        प्रति (int32_t i=count-1; i>=new_आकार; --i) {
            removeElementAt(i);
        }
    }
    count = new_आकार;
}

/**
 * Fill in the given array with all elements of this vector.
 */
शून्य** UVector::गन्तव्यम्_Array(शून्य** परिणाम) ध्रुव {
    शून्य** a = परिणाम;
    प्रति (पूर्णाङ्क i=0; i<count; ++i) {
        *a++ = elements[i].pointer;
    }
    प्रत्यागम परिणाम;
}

UObjectDeleter *UVector::समुच्चयः_Deleter(UObjectDeleter *d) {
    UObjectDeleter *old = deleter;
    deleter = d;
    प्रत्यागम old;
}

UElementsAreEqual *UVector::समुच्चयः_Comparer(UElementsAreEqual *d) {
    UElementsAreEqual *old = comparer;
    comparer = d;
    प्रत्यागम old;
}

/**
 * Removes the element at the given index from this vector and
 * transfer ownership of it to the caller.  After this call, the
 * caller owns the result and must delete it and the vector entry
 * at 'index' is removed, shifting all subsequent entries back by
 * one index and shortening the size of the vector by one.  If the
 * index is out of range or if there is no item at the given index
 * then 0 is returned and the vector is unchanged.
 */
शून्य* UVector::orphanElementAt(int32_t सूचकांक) {
    शून्य* e = शून्यसूचकः;
    यदि (0 <= सूचकांक && सूचकांक < count) {
        e = elements[सूचकांक].pointer;
        प्रति (int32_t i=सूचकांक; i<count-1; ++i) {
            elements[i] = elements[i+1];
        }
        --count;
    }
    /* else index out of range */
    प्रत्यागम e;
}

/**
 * Insert the given object into this vector at its sorted position
 * as defined by 'compare'.  The current elements are assumed to
 * be sorted already.
 */
शून्य UVector::sorted_निवेशय(शून्य* obj, UElementComparator *compare, UError_संहिता& ec) {
    UElement e;
    e.pointer = obj;
    sorted_निवेशय(e, compare, ec);
}

/**
 * Insert the given integer into this vector at its sorted position
 * as defined by 'compare'.  The current elements are assumed to
 * be sorted already.
 */
शून्य UVector::sorted_निवेशय(int32_t obj, UElementComparator *compare, UError_संहिता& ec) {
    U_ASSERT(deleter == शून्यसूचकः);
    UElement e {};
    e.integer = obj;
    sorted_निवेशय(e, compare, ec);
}

// ASSUME elements[] IS CURRENTLY SORTED
शून्य UVector::sorted_निवेशय(UElement e, UElementComparator *compare, UError_संहिता& ec) {
    // Perform a binary search for the location to insert tok at.  Tok
    // will be inserted between two elements a and b such that a <=
    // tok && tok < b, where there is a 'virtual' elements[-1] always
    // less than tok and a 'virtual' elements[count] always greater
    // than tok.
    यदि (!ensure_क्षमता(count + 1, ec)) {
        यदि (deleter != शून्यसूचकः) {
            (*deleter)(e.pointer);
        }
        प्रत्यागम;
    }
    int32_t min = 0, max = count;
    यावत् (min != max) {
        int32_t probe = (min + max) / 2;
        int32_t c = (*compare)(elements[probe], e);
        यदि (c > 0) {
            max = probe;
        } अन्यथा {
            // assert(c <= 0);
            min = probe + 1;
        }
    }
    प्रति (int32_t i=count; i>min; --i) {
        elements[i] = elements[i-1];
    }
    elements[min] = e;
    ++count;
}

/**
  *  Array sort comparator function.
  *  Used from UVector::sort()
  *  Conforms to function signature required for uprv_sortArray().
  *  This function is essentially just a wrapper, to make a
  *  UVector style comparator function usable with uprv_sortArray().
  *
  *  The context pointer to this function is a pointer back
  *  (with some extra indirection) to the user supplied comparator.
  *  
  */
स्थिर int32_t U_CALLCONV
क्रमय_Comparator(ध्रुव शून्य *context, ध्रुव शून्य *वामतः, ध्रुव शून्य *दक्षिणतः) {
    UElementComparator *compare = *static_cast<UElementComparator * ध्रुव *>(context);
    UElement e1 = *static_cast<ध्रुव UElement *>(वामतः);
    UElement e2 = *static_cast<ध्रुव UElement *>(दक्षिणतः);
    int32_t परिणाम = (*compare)(e1, e2);
    प्रत्यागम परिणाम;
}


/**
  *  Array sort comparison function for use from UVector::sorti()
  *  Compares int32_t vector elements.
  */
स्थिर int32_t U_CALLCONV
sortiComparator(ध्रुव शून्य * /*context */, ध्रुव शून्य *वामतः, ध्रुव शून्य *दक्षिणतः) {
    ध्रुव UElement *e1 = static_cast<ध्रुव UElement *>(वामतः);
    ध्रुव UElement *e2 = static_cast<ध्रुव UElement *>(दक्षिणतः);
    int32_t परिणाम = e1->integer < e2->integer? -1 :
                     e1->integer == e2->integer? 0 : 1;
    प्रत्यागम परिणाम;
}

/**
  * Sort the vector, assuming it contains ints.
  *     (A more general sort would take a comparison function, but it's
  *     not clear whether UVector's UElementComparator or
  *     UComparator from uprv_sortAray would be more appropriate.)
  */
शून्य UVector::sorti(UError_संहिता &ec) {
    यदि (U_SUCCESS(ec)) {
        uprv_क्रमय_Array(elements, count, परिमाण(UElement),
                       sortiComparator, शून्यसूचकः,  असत्य, &ec);
    }
}


/**
 *  Sort with a user supplied comparator.
 *
 *    The comparator function handling is confusing because the function type
 *    for UVector  (as defined for sortedInsert()) is different from the signature
 *    required by uprv_sortArray().  This is handled by passing the
 *    the UVector sort function pointer via the context pointer to a
 *    sortArray() comparator function, which can then call back to
 *    the original user function.
 *
 *    An additional twist is that it's not safe to pass a pointer-to-function
 *    as  a (void *) data pointer, so instead we pass a (data) pointer to a
 *    pointer-to-function variable.
 */
शून्य UVector::sort(UElementComparator *compare, UError_संहिता &ec) {
    यदि (U_SUCCESS(ec)) {
        uprv_क्रमय_Array(elements, count, परिमाण(UElement),
                       क्रमय_Comparator, &compare, असत्य, &ec);
    }
}


/**
 *  Stable sort with a user supplied comparator of type UComparator.
 */
शून्य UVector::क्रमय_With_UComparator(UComparator *compare, ध्रुव शून्य *context, UError_संहिता &ec) {
    यदि (U_SUCCESS(ec)) {
        uprv_क्रमय_Array(elements, count, परिमाण(UElement),
                       compare, context, सत्य, &ec);
    }
}

U_NAMESPACE_अन्तः

