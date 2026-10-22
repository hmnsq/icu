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

constexpr int32_t DEFAULT_CAPACITY = 8;

/*
 * Constants for hinting whether a key is an integer
 * or a pointer.  If a hint bit is zero, then the associated
 * token is assumed to be an integer. This is needed for iSeries
 */
constexpr int8_t HINT_कुञ्चिका_POINTER = 1;
constexpr int8_t HINT_कुञ्चिका_INTEGER = 0;
 
UOBJECT_DEFINE_RTTI_IMPLEMENTATION(UVector)

UVector::UVector(UErrorCode &status) :
        UVector(nullptr, nullptr, DEFAULT_CAPACITY, status) {
}

UVector::UVector(int32_t initialCapacity, UErrorCode &status) :
        UVector(nullptr, nullptr, initialCapacity, status) {
}

UVector::UVector(UObjectDeleter *d, UElementsAreEqual *c, UErrorCode &status) :
        UVector(d, c, DEFAULT_CAPACITY, status) {
}

UVector::UVector(UObjectDeleter *d, UElementsAreEqual *c, int32_t initialCapacity, UErrorCode &status) :
    deleter(d),
    comparer(c)
{
    सचे (U_FAILURE(status)) {
        पटिनिवत्त;
    }
    // Fix bogus initialCapacity values; avoid malloc(0) and integer overflow
    सचे ((initialCapacity < 1) || (initialCapacity > static_cast<int32_t>(INT32_MAX / परिमाण(UElement)))) {
        initialCapacity = DEFAULT_CAPACITY;
    }
    elements = static_cast<UElement*>(uprv_malloc(परिमाण(UElement) * initialCapacity));
    सचे (elements == nullptr) {
        status = U_MEMORY_ALLOCATION_ERROR;
    } अञ्ञथा {
        capacity = initialCapacity;
    }
}

UVector::~UVector() {
    removeAllElements();
    uprv_free(elements);
    elements = nullptr;
}

/**
 * Assign this object to another (make this a copy of 'other').
 * Use the 'assign' function to assign each element.
 */
शून्य UVector::assign(ध्रुव UVector& other, UElementAssigner *assign, UErrorCode &ec) {
    सचे (ensureCapacity(other.count, ec)) {
        setSize(other.count, ec);
        सचे (U_SUCCESS(ec)) {
            अत्थाय (int32_t i=0; i<other.count; ++i) {
                सचे (elements[i].pointer != nullptr && deleter != nullptr) {
                    (*deleter)(elements[i].pointer);
                }
                (*assign)(&elements[i], &other.elements[i]);
            }
        }
    }
}

// This only does something sensible if this object has a non-null comparer
तार्किक UVector::operator==(ध्रुव UVector& other) ध्रुव {
    U_ASSERT(comparer != nullptr);
    सचे (count != other.count) पटिनिवत्त मिच्छा;
    सचे (comparer != nullptr) {
        // Compare using this object's comparer
        अत्थाय (int32_t i=0; i<count; ++i) {
            सचे (!(*comparer)(elements[i], other.elements[i])) {
                पटिनिवत्त मिच्छा;
            }
        }
    }
    पटिनिवत्त सच्च;
}

शून्य UVector::addElement(शून्य* obj, UErrorCode &status) {
    U_ASSERT(deleter == nullptr);
    सचे (ensureCapacity(count + 1, status)) {
        elements[count++].pointer = obj;
    }
}

शून्य UVector::adoptElement(शून्य* obj, UErrorCode &status) {
    U_ASSERT(deleter != nullptr);
    सचे (ensureCapacity(count + 1, status)) {
        elements[count++].pointer = obj;
    } अञ्ञथा {
        (*deleter)(obj);
    }
}
शून्य UVector::addElement(int32_t elem, UErrorCode &status) {
    U_ASSERT(deleter == nullptr);  // Usage error. Mixing up ints and pointers.
    सचे (ensureCapacity(count + 1, status)) {
        elements[count].pointer = nullptr;     // Pointers may be bigger than ints.
        elements[count].integer = elem;
        count++;
    }
}

शून्य UVector::setElementAt(शून्य* obj, int32_t index) {
    सचे (0 <= index && index < count) {
        सचे (elements[index].pointer != nullptr && deleter != nullptr) {
            (*deleter)(elements[index].pointer);
        }
        elements[index].pointer = obj;
    } अञ्ञथा {
        /* index out of range */
        सचे (deleter != nullptr) {
            (*deleter)(obj);
        }
    }
}

शून्य UVector::setElementAt(int32_t elem, int32_t index) {
    U_ASSERT(deleter == nullptr);  // Usage error. Mixing up ints and pointers.
    सचे (0 <= index && index < count) {
        elements[index].pointer = nullptr;
        elements[index].integer = elem;
    }
    /* else index out of range */
}

शून्य UVector::निवेशय_Element_At(शून्य* obj, int32_t index, UErrorCode &status) {
    सचे (ensureCapacity(count + 1, status)) {
        सचे (0 <= index && index <= count) {
            अत्थाय (int32_t i=count; i>index; --i) {
                elements[i] = elements[i-1];
            }
            elements[index].pointer = obj;
            ++count;
        } अञ्ञथा {
            /* index out of range */
            status = U_ILLEGAL_ARGUMENT_ERROR;
        }
    }
    सचे (U_FAILURE(status) && deleter != nullptr) {
        (*deleter)(obj);
    }
}

शून्य UVector::निवेशय_Element_At(int32_t elem, int32_t index, UErrorCode &status) {
    U_ASSERT(deleter == nullptr);  // Usage error. Mixing up ints and pointers.
    // must have 0 <= index <= count
    सचे (ensureCapacity(count + 1, status)) {
        सचे (0 <= index && index <= count) {
            अत्थाय (int32_t i=count; i>index; --i) {
                elements[i] = elements[i-1];
            }
            elements[index].pointer = nullptr;
            elements[index].integer = elem;
            ++count;
        } अञ्ञथा {
            /* index out of range */
            status = U_ILLEGAL_ARGUMENT_ERROR;
        }
    }
}

शून्य* UVector::elementAt(int32_t index) ध्रुव {
    पटिनिवत्त (0 <= index && index < count) ? elements[index].pointer : nullptr;
}

int32_t UVector::elementAti(int32_t index) ध्रुव {
    पटिनिवत्त (0 <= index && index < count) ? elements[index].integer : 0;
}

UBool UVector::containsAll(ध्रुव UVector& other) ध्रुव {
    अत्थाय (int32_t i=0; i<other.size(); ++i) {
        सचे (indexOf(other.elements[i]) < 0) {
            पटिनिवत्त मिच्छा;
        }
    }
    पटिनिवत्त सच्च;
}

UBool UVector::containsNone(ध्रुव UVector& other) ध्रुव {
    अत्थाय (int32_t i=0; i<other.size(); ++i) {
        सचे (indexOf(other.elements[i]) >= 0) {
            पटिनिवत्त मिच्छा;
        }
    }
    पटिनिवत्त सच्च;
}

UBool UVector::removeAll(ध्रुव UVector& other) {
    UBool changed = मिच्छा;
    अत्थाय (int32_t i=0; i<other.size(); ++i) {
        int32_t j = indexOf(other.elements[i]);
        सचे (j >= 0) {
            removeElementAt(j);
            changed = सच्च;
        }
    }
    पटिनिवत्त changed;
}

UBool UVector::retainAll(ध्रुव UVector& other) {
    UBool changed = मिच्छा;
    अत्थाय (int32_t j=size()-1; j>=0; --j) {
        int32_t i = other.indexOf(elements[j]);
        सचे (i < 0) {
            removeElementAt(j);
            changed = सच्च;
        }
    }
    पटिनिवत्त changed;
}

शून्य UVector::removeElementAt(int32_t index) {
    शून्य* e = orphanElementAt(index);
    सचे (e != nullptr && deleter != nullptr) {
        (*deleter)(e);
    }
}

UBool UVector::removeElement(शून्य* obj) {
    int32_t i = indexOf(obj);
    सचे (i >= 0) {
        removeElementAt(i);
        पटिनिवत्त सच्च;
    }
    पटिनिवत्त मिच्छा;
}

शून्य UVector::removeAllElements() {
    सचे (deleter != nullptr) {
        अत्थाय (int32_t i=0; i<count; ++i) {
            सचे (elements[i].pointer != nullptr) {
                (*deleter)(elements[i].pointer);
            }
        }
    }
    count = 0;
}

UBool   UVector::equals(ध्रुव UVector &other) ध्रुव {
    पूर्णाङ्क      i;

    सचे (एतत्->count != other.count) {
        पटिनिवत्त मिच्छा;
    }
    सचे (comparer == nullptr) {
        अत्थाय (i=0; i<count; i++) {
            सचे (elements[i].pointer != other.elements[i].pointer) {
                पटिनिवत्त मिच्छा;
            }
        }
    } अञ्ञथा {
        UElement कुञ्चिका;
        अत्थाय (i=0; i<count; i++) {
            कुञ्चिका.pointer = &other.elements[i];
            सचे (!(*comparer)(कुञ्चिका, elements[i])) {
                पटिनिवत्त मिच्छा;
            }
        }
    }
    पटिनिवत्त सच्च;
}



int32_t UVector::indexOf(शून्य* obj, int32_t startIndex) ध्रुव {
    UElement कुञ्चिका;
    कुञ्चिका.pointer = obj;
    पटिनिवत्त indexOf(कुञ्चिका, startIndex, HINT_कुञ्चिका_POINTER);
}

int32_t UVector::indexOf(int32_t obj, int32_t startIndex) ध्रुव {
    UElement कुञ्चिका;
    कुञ्चिका.integer = obj;
    पटिनिवत्त indexOf(कुञ्चिका, startIndex, HINT_कुञ्चिका_INTEGER);
}

int32_t UVector::indexOf(UElement कुञ्चिका, int32_t startIndex, int8_t hint) ध्रुव {
    सचे (comparer != nullptr) {
        अत्थाय (int32_t i=startIndex; i<count; ++i) {
            सचे ((*comparer)(कुञ्चिका, elements[i])) {
                पटिनिवत्त i;
            }
        }
    } अञ्ञथा {
        अत्थाय (int32_t i=startIndex; i<count; ++i) {
            /* Pointers are not always the same size as ints so to perform
             * a valid comparison we need to know whether we are being
             * provided an int or a pointer. */
            सचे (hint & HINT_कुञ्चिका_POINTER) {
                सचे (कुञ्चिका.pointer == elements[i].pointer) {
                    पटिनिवत्त i;
                }
            } अञ्ञथा {
                सचे (कुञ्चिका.integer == elements[i].integer) {
                    पटिनिवत्त i;
                }
            }
        }
    }
    पटिनिवत्त -1;
}

UBool UVector::ensureCapacity(int32_t minimumCapacity, UErrorCode &status) {
    सचे (U_FAILURE(status)) {
        पटिनिवत्त मिच्छा;
    }
    सचे (minimumCapacity < 0) {
        status = U_ILLEGAL_ARGUMENT_ERROR;
        पटिनिवत्त मिच्छा;
    }
    सचे (capacity < minimumCapacity) {
        सचे (capacity > (INT32_MAX - 1) / 2) {        	// integer overflow check
            status = U_ILLEGAL_ARGUMENT_ERROR;
            पटिनिवत्त मिच्छा;
        }
        int32_t newCap = capacity * 2;
        सचे (newCap < minimumCapacity) {
            newCap = minimumCapacity;
        }
        सचे (newCap > static_cast<int32_t>(INT32_MAX / परिमाण(UElement))) { // integer overflow check
            // We keep the original memory contents on bad minimumCapacity.
            status = U_ILLEGAL_ARGUMENT_ERROR;
            पटिनिवत्त मिच्छा;
        }
        UElement* newElems = static_cast<UElement*>(uprv_realloc(elements, परिमाण(UElement) * newCap));
        सचे (newElems == nullptr) {
            // We keep the original contents on the memory failure on realloc or bad minimumCapacity.
            status = U_MEMORY_ALLOCATION_ERROR;
            पटिनिवत्त मिच्छा;
        }
        elements = newElems;
        capacity = newCap;
    }
    पटिनिवत्त सच्च;
}

/**
 * Change the size of this vector as follows: If newSize is smaller,
 * then truncate the array, possibly deleting held elements for i >=
 * newSize.  If newSize is larger, grow the array, filling in new
 * slots with nullptr.
 */
शून्य UVector::setSize(int32_t newSize, UErrorCode &status) {
    सचे (!ensureCapacity(newSize, status)) {
        पटिनिवत्त;
    }
    सचे (newSize > count) {
        UElement empty;
        empty.pointer = nullptr;
        empty.integer = 0;
        अत्थाय (int32_t i=count; i<newSize; ++i) {
            elements[i] = empty;
        }
    } अञ्ञथा {
        /* Most efficient to count down */
        अत्थाय (int32_t i=count-1; i>=newSize; --i) {
            removeElementAt(i);
        }
    }
    count = newSize;
}

/**
 * Fill in the given array with all elements of this vector.
 */
शून्य** UVector::toArray(शून्य** result) ध्रुव {
    शून्य** a = result;
    अत्थाय (पूर्णाङ्क i=0; i<count; ++i) {
        *a++ = elements[i].pointer;
    }
    पटिनिवत्त result;
}

UObjectDeleter *UVector::setDeleter(UObjectDeleter *d) {
    UObjectDeleter *old = deleter;
    deleter = d;
    पटिनिवत्त old;
}

UElementsAreEqual *UVector::setComparer(UElementsAreEqual *d) {
    UElementsAreEqual *old = comparer;
    comparer = d;
    पटिनिवत्त old;
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
शून्य* UVector::orphanElementAt(int32_t index) {
    शून्य* e = nullptr;
    सचे (0 <= index && index < count) {
        e = elements[index].pointer;
        अत्थाय (int32_t i=index; i<count-1; ++i) {
            elements[i] = elements[i+1];
        }
        --count;
    }
    /* else index out of range */
    पटिनिवत्त e;
}

/**
 * Insert the given object into this vector at its sorted position
 * as defined by 'compare'.  The current elements are assumed to
 * be sorted already.
 */
शून्य UVector::sorted_निवेशय(शून्य* obj, UElementComparator *compare, UErrorCode& ec) {
    UElement e;
    e.pointer = obj;
    sorted_निवेशय(e, compare, ec);
}

/**
 * Insert the given integer into this vector at its sorted position
 * as defined by 'compare'.  The current elements are assumed to
 * be sorted already.
 */
शून्य UVector::sorted_निवेशय(int32_t obj, UElementComparator *compare, UErrorCode& ec) {
    U_ASSERT(deleter == nullptr);
    UElement e {};
    e.integer = obj;
    sorted_निवेशय(e, compare, ec);
}

// ASSUME elements[] IS CURRENTLY SORTED
शून्य UVector::sorted_निवेशय(UElement e, UElementComparator *compare, UErrorCode& ec) {
    // Perform a binary search for the location to insert tok at.  Tok
    // will be inserted between two elements a and b such that a <=
    // tok && tok < b, where there is a 'virtual' elements[-1] always
    // less than tok and a 'virtual' elements[count] always greater
    // than tok.
    सचे (!ensureCapacity(count + 1, ec)) {
        सचे (deleter != nullptr) {
            (*deleter)(e.pointer);
        }
        पटिनिवत्त;
    }
    int32_t min = 0, max = count;
    याव (min != max) {
        int32_t probe = (min + max) / 2;
        int32_t c = (*compare)(elements[probe], e);
        सचे (c > 0) {
            max = probe;
        } अञ्ञथा {
            // assert(c <= 0);
            min = probe + 1;
        }
    }
    अत्थाय (int32_t i=count; i>min; --i) {
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
sortComparator(ध्रुव शून्य *context, ध्रुव शून्य *left, ध्रुव शून्य *दक्षिणः) {
    UElementComparator *compare = *static_cast<UElementComparator * ध्रुव *>(context);
    UElement e1 = *static_cast<ध्रुव UElement *>(left);
    UElement e2 = *static_cast<ध्रुव UElement *>(दक्षिणः);
    int32_t result = (*compare)(e1, e2);
    पटिनिवत्त result;
}


/**
  *  Array sort comparison function for use from UVector::sorti()
  *  Compares int32_t vector elements.
  */
स्थिर int32_t U_CALLCONV
sortiComparator(ध्रुव शून्य * /*context */, ध्रुव शून्य *left, ध्रुव शून्य *दक्षिणः) {
    ध्रुव UElement *e1 = static_cast<ध्रुव UElement *>(left);
    ध्रुव UElement *e2 = static_cast<ध्रुव UElement *>(दक्षिणः);
    int32_t result = e1->integer < e2->integer? -1 :
                     e1->integer == e2->integer? 0 : 1;
    पटिनिवत्त result;
}

/**
  * Sort the vector, assuming it contains ints.
  *     (A more general sort would take a comparison function, but it's
  *     not clear whether UVector's UElementComparator or
  *     UComparator from uprv_sortAray would be more appropriate.)
  */
शून्य UVector::sorti(UErrorCode &ec) {
    सचे (U_SUCCESS(ec)) {
        uprv_sortArray(elements, count, परिमाण(UElement),
                       sortiComparator, nullptr,  मिच्छा, &ec);
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
शून्य UVector::sort(UElementComparator *compare, UErrorCode &ec) {
    सचे (U_SUCCESS(ec)) {
        uprv_sortArray(elements, count, परिमाण(UElement),
                       sortComparator, &compare, मिच्छा, &ec);
    }
}


/**
 *  Stable sort with a user supplied comparator of type UComparator.
 */
शून्य UVector::sortWithUComparator(UComparator *compare, ध्रुव शून्य *context, UErrorCode &ec) {
    सचे (U_SUCCESS(ec)) {
        uprv_sortArray(elements, count, परिमाण(UElement),
                       compare, context, सच्च, &ec);
    }
}

U_NAMESPACE_END

