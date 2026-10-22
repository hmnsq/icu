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

#ಅನ್ತರ್ಭಾವ "uvector.h"
#ಅನ್ತರ್ಭಾವ "cmemory.h"
#ಅನ್ತರ್ಭಾವ "uarrsort.h"
#ಅನ್ತರ್ಭಾವ "uelement.h"

U_NAMESPACE_BEGIN

constexpr int32_t DEFAULT_CAPACITY = 8;

/*
 * Constants for hinting whether a key is an integer
 * or a pointer.  If a hint bit is zero, then the associated
 * token is assumed to be an integer. This is needed for iSeries
 */
constexpr int8_t HINT_ಕೀಲಿ_POINTER = 1;
constexpr int8_t HINT_ಕೀಲಿ_INTEGER = 0;
 
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
    ಯದಿ (U_FAILURE(status)) {
        ಪ್ರತ್ಯಾಗಮ;
    }
    // Fix bogus initialCapacity values; avoid malloc(0) and integer overflow
    ಯದಿ ((initialCapacity < 1) || (initialCapacity > static_cast<int32_t>(INT32_MAX / ಪರಿಮಾಣ(UElement)))) {
        initialCapacity = DEFAULT_CAPACITY;
    }
    elements = static_cast<UElement*>(uprv_malloc(ಪರಿಮಾಣ(UElement) * initialCapacity));
    ಯದಿ (elements == nullptr) {
        status = U_MEMORY_ALLOCATION_ERROR;
    } ಅನ್ಯಥಾ {
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
ಶೂನ್ಯ UVector::assign(ಧ್ರುವ UVector& other, UElementAssigner *assign, UErrorCode &ec) {
    ಯದಿ (ensureCapacity(other.count, ec)) {
        setSize(other.count, ec);
        ಯದಿ (U_SUCCESS(ec)) {
            ಪ್ರತಿ (int32_t i=0; i<other.count; ++i) {
                ಯದಿ (elements[i].pointer != nullptr && deleter != nullptr) {
                    (*deleter)(elements[i].pointer);
                }
                (*assign)(&elements[i], &other.elements[i]);
            }
        }
    }
}

// This only does something sensible if this object has a non-null comparer
ತಾರ್ಕಿಕ UVector::operator==(ಧ್ರುವ UVector& other) ಧ್ರುವ {
    U_ASSERT(comparer != nullptr);
    ಯದಿ (count != other.count) ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;
    ಯದಿ (comparer != nullptr) {
        // Compare using this object's comparer
        ಪ್ರತಿ (int32_t i=0; i<count; ++i) {
            ಯದಿ (!(*comparer)(elements[i], other.elements[i])) {
                ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;
            }
        }
    }
    ಪ್ರತ್ಯಾಗಮ ಸತ್ಯ;
}

ಶೂನ್ಯ UVector::addElement(ಶೂನ್ಯ* obj, UErrorCode &status) {
    U_ASSERT(deleter == nullptr);
    ಯದಿ (ensureCapacity(count + 1, status)) {
        elements[count++].pointer = obj;
    }
}

ಶೂನ್ಯ UVector::adoptElement(ಶೂನ್ಯ* obj, UErrorCode &status) {
    U_ASSERT(deleter != nullptr);
    ಯದಿ (ensureCapacity(count + 1, status)) {
        elements[count++].pointer = obj;
    } ಅನ್ಯಥಾ {
        (*deleter)(obj);
    }
}
ಶೂನ್ಯ UVector::addElement(int32_t elem, UErrorCode &status) {
    U_ASSERT(deleter == nullptr);  // Usage error. Mixing up ints and pointers.
    ಯದಿ (ensureCapacity(count + 1, status)) {
        elements[count].pointer = nullptr;     // Pointers may be bigger than ints.
        elements[count].integer = elem;
        count++;
    }
}

ಶೂನ್ಯ UVector::setElementAt(ಶೂನ್ಯ* obj, int32_t index) {
    ಯದಿ (0 <= index && index < count) {
        ಯದಿ (elements[index].pointer != nullptr && deleter != nullptr) {
            (*deleter)(elements[index].pointer);
        }
        elements[index].pointer = obj;
    } ಅನ್ಯಥಾ {
        /* index out of range */
        ಯದಿ (deleter != nullptr) {
            (*deleter)(obj);
        }
    }
}

ಶೂನ್ಯ UVector::setElementAt(int32_t elem, int32_t index) {
    U_ASSERT(deleter == nullptr);  // Usage error. Mixing up ints and pointers.
    ಯದಿ (0 <= index && index < count) {
        elements[index].pointer = nullptr;
        elements[index].integer = elem;
    }
    /* else index out of range */
}

ಶೂನ್ಯ UVector::ಸೇರಿಸು_Element_At(ಶೂನ್ಯ* obj, int32_t index, UErrorCode &status) {
    ಯದಿ (ensureCapacity(count + 1, status)) {
        ಯದಿ (0 <= index && index <= count) {
            ಪ್ರತಿ (int32_t i=count; i>index; --i) {
                elements[i] = elements[i-1];
            }
            elements[index].pointer = obj;
            ++count;
        } ಅನ್ಯಥಾ {
            /* index out of range */
            status = U_ILLEGAL_ARGUMENT_ERROR;
        }
    }
    ಯದಿ (U_FAILURE(status) && deleter != nullptr) {
        (*deleter)(obj);
    }
}

ಶೂನ್ಯ UVector::ಸೇರಿಸು_Element_At(int32_t elem, int32_t index, UErrorCode &status) {
    U_ASSERT(deleter == nullptr);  // Usage error. Mixing up ints and pointers.
    // must have 0 <= index <= count
    ಯದಿ (ensureCapacity(count + 1, status)) {
        ಯದಿ (0 <= index && index <= count) {
            ಪ್ರತಿ (int32_t i=count; i>index; --i) {
                elements[i] = elements[i-1];
            }
            elements[index].pointer = nullptr;
            elements[index].integer = elem;
            ++count;
        } ಅನ್ಯಥಾ {
            /* index out of range */
            status = U_ILLEGAL_ARGUMENT_ERROR;
        }
    }
}

ಶೂನ್ಯ* UVector::elementAt(int32_t index) ಧ್ರುವ {
    ಪ್ರತ್ಯಾಗಮ (0 <= index && index < count) ? elements[index].pointer : nullptr;
}

int32_t UVector::elementAti(int32_t index) ಧ್ರುವ {
    ಪ್ರತ್ಯಾಗಮ (0 <= index && index < count) ? elements[index].integer : 0;
}

UBool UVector::containsAll(ಧ್ರುವ UVector& other) ಧ್ರುವ {
    ಪ್ರತಿ (int32_t i=0; i<other.size(); ++i) {
        ಯದಿ (indexOf(other.elements[i]) < 0) {
            ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;
        }
    }
    ಪ್ರತ್ಯಾಗಮ ಸತ್ಯ;
}

UBool UVector::containsNone(ಧ್ರುವ UVector& other) ಧ್ರುವ {
    ಪ್ರತಿ (int32_t i=0; i<other.size(); ++i) {
        ಯದಿ (indexOf(other.elements[i]) >= 0) {
            ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;
        }
    }
    ಪ್ರತ್ಯಾಗಮ ಸತ್ಯ;
}

UBool UVector::removeAll(ಧ್ರುವ UVector& other) {
    UBool changed = ಅಸತ್ಯ;
    ಪ್ರತಿ (int32_t i=0; i<other.size(); ++i) {
        int32_t j = indexOf(other.elements[i]);
        ಯದಿ (j >= 0) {
            removeElementAt(j);
            changed = ಸತ್ಯ;
        }
    }
    ಪ್ರತ್ಯಾಗಮ changed;
}

UBool UVector::retainAll(ಧ್ರುವ UVector& other) {
    UBool changed = ಅಸತ್ಯ;
    ಪ್ರತಿ (int32_t j=size()-1; j>=0; --j) {
        int32_t i = other.indexOf(elements[j]);
        ಯದಿ (i < 0) {
            removeElementAt(j);
            changed = ಸತ್ಯ;
        }
    }
    ಪ್ರತ್ಯಾಗಮ changed;
}

ಶೂನ್ಯ UVector::removeElementAt(int32_t index) {
    ಶೂನ್ಯ* e = orphanElementAt(index);
    ಯದಿ (e != nullptr && deleter != nullptr) {
        (*deleter)(e);
    }
}

UBool UVector::removeElement(ಶೂನ್ಯ* obj) {
    int32_t i = indexOf(obj);
    ಯದಿ (i >= 0) {
        removeElementAt(i);
        ಪ್ರತ್ಯಾಗಮ ಸತ್ಯ;
    }
    ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;
}

ಶೂನ್ಯ UVector::removeAllElements() {
    ಯದಿ (deleter != nullptr) {
        ಪ್ರತಿ (int32_t i=0; i<count; ++i) {
            ಯದಿ (elements[i].pointer != nullptr) {
                (*deleter)(elements[i].pointer);
            }
        }
    }
    count = 0;
}

UBool   UVector::equals(ಧ್ರುವ UVector &other) ಧ್ರುವ {
    ಪೂರ್ಣಾಙ್ಕ      i;

    ಯದಿ (ಏತತ್->count != other.count) {
        ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;
    }
    ಯದಿ (comparer == nullptr) {
        ಪ್ರತಿ (i=0; i<count; i++) {
            ಯದಿ (elements[i].pointer != other.elements[i].pointer) {
                ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;
            }
        }
    } ಅನ್ಯಥಾ {
        UElement ಕೀಲಿ;
        ಪ್ರತಿ (i=0; i<count; i++) {
            ಕೀಲಿ.pointer = &other.elements[i];
            ಯದಿ (!(*comparer)(ಕೀಲಿ, elements[i])) {
                ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;
            }
        }
    }
    ಪ್ರತ್ಯಾಗಮ ಸತ್ಯ;
}



int32_t UVector::indexOf(ಶೂನ್ಯ* obj, int32_t startIndex) ಧ್ರುವ {
    UElement ಕೀಲಿ;
    ಕೀಲಿ.pointer = obj;
    ಪ್ರತ್ಯಾಗಮ indexOf(ಕೀಲಿ, startIndex, HINT_ಕೀಲಿ_POINTER);
}

int32_t UVector::indexOf(int32_t obj, int32_t startIndex) ಧ್ರುವ {
    UElement ಕೀಲಿ;
    ಕೀಲಿ.integer = obj;
    ಪ್ರತ್ಯಾಗಮ indexOf(ಕೀಲಿ, startIndex, HINT_ಕೀಲಿ_INTEGER);
}

int32_t UVector::indexOf(UElement ಕೀಲಿ, int32_t startIndex, int8_t hint) ಧ್ರುವ {
    ಯದಿ (comparer != nullptr) {
        ಪ್ರತಿ (int32_t i=startIndex; i<count; ++i) {
            ಯದಿ ((*comparer)(ಕೀಲಿ, elements[i])) {
                ಪ್ರತ್ಯಾಗಮ i;
            }
        }
    } ಅನ್ಯಥಾ {
        ಪ್ರತಿ (int32_t i=startIndex; i<count; ++i) {
            /* Pointers are not always the same size as ints so to perform
             * a valid comparison we need to know whether we are being
             * provided an int or a pointer. */
            ಯದಿ (hint & HINT_ಕೀಲಿ_POINTER) {
                ಯದಿ (ಕೀಲಿ.pointer == elements[i].pointer) {
                    ಪ್ರತ್ಯಾಗಮ i;
                }
            } ಅನ್ಯಥಾ {
                ಯದಿ (ಕೀಲಿ.integer == elements[i].integer) {
                    ಪ್ರತ್ಯಾಗಮ i;
                }
            }
        }
    }
    ಪ್ರತ್ಯಾಗಮ -1;
}

UBool UVector::ensureCapacity(int32_t minimumCapacity, UErrorCode &status) {
    ಯದಿ (U_FAILURE(status)) {
        ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;
    }
    ಯದಿ (minimumCapacity < 0) {
        status = U_ILLEGAL_ARGUMENT_ERROR;
        ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;
    }
    ಯದಿ (capacity < minimumCapacity) {
        ಯದಿ (capacity > (INT32_MAX - 1) / 2) {        	// integer overflow check
            status = U_ILLEGAL_ARGUMENT_ERROR;
            ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;
        }
        int32_t newCap = capacity * 2;
        ಯದಿ (newCap < minimumCapacity) {
            newCap = minimumCapacity;
        }
        ಯದಿ (newCap > static_cast<int32_t>(INT32_MAX / ಪರಿಮಾಣ(UElement))) { // integer overflow check
            // We keep the original memory contents on bad minimumCapacity.
            status = U_ILLEGAL_ARGUMENT_ERROR;
            ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;
        }
        UElement* newElems = static_cast<UElement*>(uprv_realloc(elements, ಪರಿಮಾಣ(UElement) * newCap));
        ಯದಿ (newElems == nullptr) {
            // We keep the original contents on the memory failure on realloc or bad minimumCapacity.
            status = U_MEMORY_ALLOCATION_ERROR;
            ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;
        }
        elements = newElems;
        capacity = newCap;
    }
    ಪ್ರತ್ಯಾಗಮ ಸತ್ಯ;
}

/**
 * Change the size of this vector as follows: If newSize is smaller,
 * then truncate the array, possibly deleting held elements for i >=
 * newSize.  If newSize is larger, grow the array, filling in new
 * slots with nullptr.
 */
ಶೂನ್ಯ UVector::setSize(int32_t newSize, UErrorCode &status) {
    ಯದಿ (!ensureCapacity(newSize, status)) {
        ಪ್ರತ್ಯಾಗಮ;
    }
    ಯದಿ (newSize > count) {
        UElement empty;
        empty.pointer = nullptr;
        empty.integer = 0;
        ಪ್ರತಿ (int32_t i=count; i<newSize; ++i) {
            elements[i] = empty;
        }
    } ಅನ್ಯಥಾ {
        /* Most efficient to count down */
        ಪ್ರತಿ (int32_t i=count-1; i>=newSize; --i) {
            removeElementAt(i);
        }
    }
    count = newSize;
}

/**
 * Fill in the given array with all elements of this vector.
 */
ಶೂನ್ಯ** UVector::toArray(ಶೂನ್ಯ** result) ಧ್ರುವ {
    ಶೂನ್ಯ** a = result;
    ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ i=0; i<count; ++i) {
        *a++ = elements[i].pointer;
    }
    ಪ್ರತ್ಯಾಗಮ result;
}

UObjectDeleter *UVector::setDeleter(UObjectDeleter *d) {
    UObjectDeleter *old = deleter;
    deleter = d;
    ಪ್ರತ್ಯಾಗಮ old;
}

UElementsAreEqual *UVector::setComparer(UElementsAreEqual *d) {
    UElementsAreEqual *old = comparer;
    comparer = d;
    ಪ್ರತ್ಯಾಗಮ old;
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
ಶೂನ್ಯ* UVector::orphanElementAt(int32_t index) {
    ಶೂನ್ಯ* e = nullptr;
    ಯದಿ (0 <= index && index < count) {
        e = elements[index].pointer;
        ಪ್ರತಿ (int32_t i=index; i<count-1; ++i) {
            elements[i] = elements[i+1];
        }
        --count;
    }
    /* else index out of range */
    ಪ್ರತ್ಯಾಗಮ e;
}

/**
 * Insert the given object into this vector at its sorted position
 * as defined by 'compare'.  The current elements are assumed to
 * be sorted already.
 */
ಶೂನ್ಯ UVector::sorted_ಸೇರಿಸು(ಶೂನ್ಯ* obj, UElementComparator *compare, UErrorCode& ec) {
    UElement e;
    e.pointer = obj;
    sorted_ಸೇರಿಸು(e, compare, ec);
}

/**
 * Insert the given integer into this vector at its sorted position
 * as defined by 'compare'.  The current elements are assumed to
 * be sorted already.
 */
ಶೂನ್ಯ UVector::sorted_ಸೇರಿಸು(int32_t obj, UElementComparator *compare, UErrorCode& ec) {
    U_ASSERT(deleter == nullptr);
    UElement e {};
    e.integer = obj;
    sorted_ಸೇರಿಸು(e, compare, ec);
}

// ASSUME elements[] IS CURRENTLY SORTED
ಶೂನ್ಯ UVector::sorted_ಸೇರಿಸು(UElement e, UElementComparator *compare, UErrorCode& ec) {
    // Perform a binary search for the location to insert tok at.  Tok
    // will be inserted between two elements a and b such that a <=
    // tok && tok < b, where there is a 'virtual' elements[-1] always
    // less than tok and a 'virtual' elements[count] always greater
    // than tok.
    ಯದಿ (!ensureCapacity(count + 1, ec)) {
        ಯದಿ (deleter != nullptr) {
            (*deleter)(e.pointer);
        }
        ಪ್ರತ್ಯಾಗಮ;
    }
    int32_t min = 0, max = count;
    ಯಾವತ್ (min != max) {
        int32_t probe = (min + max) / 2;
        int32_t c = (*compare)(elements[probe], e);
        ಯದಿ (c > 0) {
            max = probe;
        } ಅನ್ಯಥಾ {
            // assert(c <= 0);
            min = probe + 1;
        }
    }
    ಪ್ರತಿ (int32_t i=count; i>min; --i) {
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
ಸ್ಥಿರ int32_t U_CALLCONV
sortComparator(ಧ್ರುವ ಶೂನ್ಯ *context, ಧ್ರುವ ಶೂನ್ಯ *left, ಧ್ರುವ ಶೂನ್ಯ *ಬಲ) {
    UElementComparator *compare = *static_cast<UElementComparator * ಧ್ರುವ *>(context);
    UElement e1 = *static_cast<ಧ್ರುವ UElement *>(left);
    UElement e2 = *static_cast<ಧ್ರುವ UElement *>(ಬಲ);
    int32_t result = (*compare)(e1, e2);
    ಪ್ರತ್ಯಾಗಮ result;
}


/**
  *  Array sort comparison function for use from UVector::sorti()
  *  Compares int32_t vector elements.
  */
ಸ್ಥಿರ int32_t U_CALLCONV
sortiComparator(ಧ್ರುವ ಶೂನ್ಯ * /*context */, ಧ್ರುವ ಶೂನ್ಯ *left, ಧ್ರುವ ಶೂನ್ಯ *ಬಲ) {
    ಧ್ರುವ UElement *e1 = static_cast<ಧ್ರುವ UElement *>(left);
    ಧ್ರುವ UElement *e2 = static_cast<ಧ್ರುವ UElement *>(ಬಲ);
    int32_t result = e1->integer < e2->integer? -1 :
                     e1->integer == e2->integer? 0 : 1;
    ಪ್ರತ್ಯಾಗಮ result;
}

/**
  * Sort the vector, assuming it contains ints.
  *     (A more general sort would take a comparison function, but it's
  *     not clear whether UVector's UElementComparator or
  *     UComparator from uprv_sortAray would be more appropriate.)
  */
ಶೂನ್ಯ UVector::sorti(UErrorCode &ec) {
    ಯದಿ (U_SUCCESS(ec)) {
        uprv_sortArray(elements, count, ಪರಿಮಾಣ(UElement),
                       sortiComparator, nullptr,  ಅಸತ್ಯ, &ec);
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
ಶೂನ್ಯ UVector::sort(UElementComparator *compare, UErrorCode &ec) {
    ಯದಿ (U_SUCCESS(ec)) {
        uprv_sortArray(elements, count, ಪರಿಮಾಣ(UElement),
                       sortComparator, &compare, ಅಸತ್ಯ, &ec);
    }
}


/**
 *  Stable sort with a user supplied comparator of type UComparator.
 */
ಶೂನ್ಯ UVector::sortWithUComparator(UComparator *compare, ಧ್ರುವ ಶೂನ್ಯ *context, UErrorCode &ec) {
    ಯದಿ (U_SUCCESS(ec)) {
        uprv_sortArray(elements, count, ಪರಿಮಾಣ(UElement),
                       compare, context, ಸತ್ಯ, &ec);
    }
}

U_NAMESPACE_END

