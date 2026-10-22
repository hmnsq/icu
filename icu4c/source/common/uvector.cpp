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
constexpr int8_t HINT_साँचो_POINTER = 1;
constexpr int8_t HINT_साँचो_INTEGER = 0;
 
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
    यदि (U_FAILURE(status)) {
        फर्काऊ;
    }
    // Fix bogus initialCapacity values; avoid malloc(0) and integer overflow
    यदि ((initialCapacity < 1) || (initialCapacity > static_cast<int32_t>(INT32_MAX / परिमाण(UElement)))) {
        initialCapacity = DEFAULT_CAPACITY;
    }
    elements = static_cast<UElement*>(uprv_malloc(परिमाण(UElement) * initialCapacity));
    यदि (elements == nullptr) {
        status = U_MEMORY_ALLOCATION_ERROR;
    } अन्यथा {
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
शून्य UVector::assign(अचर UVector& other, UElementAssigner *assign, UErrorCode &ec) {
    यदि (ensureCapacity(other.count, ec)) {
        setSize(other.count, ec);
        यदि (U_SUCCESS(ec)) {
            लागि (int32_t i=0; i<other.count; ++i) {
                यदि (elements[i].pointer != nullptr && deleter != nullptr) {
                    (*deleter)(elements[i].pointer);
                }
                (*assign)(&elements[i], &other.elements[i]);
            }
        }
    }
}

// This only does something sensible if this object has a non-null comparer
तार्किक UVector::चालक==(अचर UVector& other) अचर {
    U_ASSERT(comparer != nullptr);
    यदि (count != other.count) फर्काऊ असत्य;
    यदि (comparer != nullptr) {
        // Compare using this object's comparer
        लागि (int32_t i=0; i<count; ++i) {
            यदि (!(*comparer)(elements[i], other.elements[i])) {
                फर्काऊ असत्य;
            }
        }
    }
    फर्काऊ सत्य;
}

शून्य UVector::addElement(शून्य* obj, UErrorCode &status) {
    U_ASSERT(deleter == nullptr);
    यदि (ensureCapacity(count + 1, status)) {
        elements[count++].pointer = obj;
    }
}

शून्य UVector::adoptElement(शून्य* obj, UErrorCode &status) {
    U_ASSERT(deleter != nullptr);
    यदि (ensureCapacity(count + 1, status)) {
        elements[count++].pointer = obj;
    } अन्यथा {
        (*deleter)(obj);
    }
}
शून्य UVector::addElement(int32_t elem, UErrorCode &status) {
    U_ASSERT(deleter == nullptr);  // Usage error. Mixing up ints and pointers.
    यदि (ensureCapacity(count + 1, status)) {
        elements[count].pointer = nullptr;     // Pointers may be bigger than ints.
        elements[count].integer = elem;
        count++;
    }
}

शून्य UVector::setElementAt(शून्य* obj, int32_t index) {
    यदि (0 <= index && index < count) {
        यदि (elements[index].pointer != nullptr && deleter != nullptr) {
            (*deleter)(elements[index].pointer);
        }
        elements[index].pointer = obj;
    } अन्यथा {
        /* index out of range */
        यदि (deleter != nullptr) {
            (*deleter)(obj);
        }
    }
}

शून्य UVector::setElementAt(int32_t elem, int32_t index) {
    U_ASSERT(deleter == nullptr);  // Usage error. Mixing up ints and pointers.
    यदि (0 <= index && index < count) {
        elements[index].pointer = nullptr;
        elements[index].integer = elem;
    }
    /* else index out of range */
}

शून्य UVector::घुसाउ_Element_At(शून्य* obj, int32_t index, UErrorCode &status) {
    यदि (ensureCapacity(count + 1, status)) {
        यदि (0 <= index && index <= count) {
            लागि (int32_t i=count; i>index; --i) {
                elements[i] = elements[i-1];
            }
            elements[index].pointer = obj;
            ++count;
        } अन्यथा {
            /* index out of range */
            status = U_ILLEGAL_ARGUMENT_ERROR;
        }
    }
    यदि (U_FAILURE(status) && deleter != nullptr) {
        (*deleter)(obj);
    }
}

शून्य UVector::घुसाउ_Element_At(int32_t elem, int32_t index, UErrorCode &status) {
    U_ASSERT(deleter == nullptr);  // Usage error. Mixing up ints and pointers.
    // must have 0 <= index <= count
    यदि (ensureCapacity(count + 1, status)) {
        यदि (0 <= index && index <= count) {
            लागि (int32_t i=count; i>index; --i) {
                elements[i] = elements[i-1];
            }
            elements[index].pointer = nullptr;
            elements[index].integer = elem;
            ++count;
        } अन्यथा {
            /* index out of range */
            status = U_ILLEGAL_ARGUMENT_ERROR;
        }
    }
}

शून्य* UVector::elementAt(int32_t index) अचर {
    फर्काऊ (0 <= index && index < count) ? elements[index].pointer : nullptr;
}

int32_t UVector::elementAti(int32_t index) अचर {
    फर्काऊ (0 <= index && index < count) ? elements[index].integer : 0;
}

UBool UVector::containsAll(अचर UVector& other) अचर {
    लागि (int32_t i=0; i<other.size(); ++i) {
        यदि (indexOf(other.elements[i]) < 0) {
            फर्काऊ असत्य;
        }
    }
    फर्काऊ सत्य;
}

UBool UVector::containsNone(अचर UVector& other) अचर {
    लागि (int32_t i=0; i<other.size(); ++i) {
        यदि (indexOf(other.elements[i]) >= 0) {
            फर्काऊ असत्य;
        }
    }
    फर्काऊ सत्य;
}

UBool UVector::removeAll(अचर UVector& other) {
    UBool changed = असत्य;
    लागि (int32_t i=0; i<other.size(); ++i) {
        int32_t j = indexOf(other.elements[i]);
        यदि (j >= 0) {
            removeElementAt(j);
            changed = सत्य;
        }
    }
    फर्काऊ changed;
}

UBool UVector::retainAll(अचर UVector& other) {
    UBool changed = असत्य;
    लागि (int32_t j=size()-1; j>=0; --j) {
        int32_t i = other.indexOf(elements[j]);
        यदि (i < 0) {
            removeElementAt(j);
            changed = सत्य;
        }
    }
    फर्काऊ changed;
}

शून्य UVector::removeElementAt(int32_t index) {
    शून्य* e = orphanElementAt(index);
    यदि (e != nullptr && deleter != nullptr) {
        (*deleter)(e);
    }
}

UBool UVector::removeElement(शून्य* obj) {
    int32_t i = indexOf(obj);
    यदि (i >= 0) {
        removeElementAt(i);
        फर्काऊ सत्य;
    }
    फर्काऊ असत्य;
}

शून्य UVector::removeAllElements() {
    यदि (deleter != nullptr) {
        लागि (int32_t i=0; i<count; ++i) {
            यदि (elements[i].pointer != nullptr) {
                (*deleter)(elements[i].pointer);
            }
        }
    }
    count = 0;
}

UBool   UVector::equals(अचर UVector &other) अचर {
    पूर्णाङ्क      i;

    यदि (एतत्->count != other.count) {
        फर्काऊ असत्य;
    }
    यदि (comparer == nullptr) {
        लागि (i=0; i<count; i++) {
            यदि (elements[i].pointer != other.elements[i].pointer) {
                फर्काऊ असत्य;
            }
        }
    } अन्यथा {
        UElement साँचो;
        लागि (i=0; i<count; i++) {
            साँचो.pointer = &other.elements[i];
            यदि (!(*comparer)(साँचो, elements[i])) {
                फर्काऊ असत्य;
            }
        }
    }
    फर्काऊ सत्य;
}



int32_t UVector::indexOf(शून्य* obj, int32_t startIndex) अचर {
    UElement साँचो;
    साँचो.pointer = obj;
    फर्काऊ indexOf(साँचो, startIndex, HINT_साँचो_POINTER);
}

int32_t UVector::indexOf(int32_t obj, int32_t startIndex) अचर {
    UElement साँचो;
    साँचो.integer = obj;
    फर्काऊ indexOf(साँचो, startIndex, HINT_साँचो_INTEGER);
}

int32_t UVector::indexOf(UElement साँचो, int32_t startIndex, int8_t hint) अचर {
    यदि (comparer != nullptr) {
        लागि (int32_t i=startIndex; i<count; ++i) {
            यदि ((*comparer)(साँचो, elements[i])) {
                फर्काऊ i;
            }
        }
    } अन्यथा {
        लागि (int32_t i=startIndex; i<count; ++i) {
            /* Pointers are not always the same size as ints so to perform
             * a valid comparison we need to know whether we are being
             * provided an int or a pointer. */
            यदि (hint & HINT_साँचो_POINTER) {
                यदि (साँचो.pointer == elements[i].pointer) {
                    फर्काऊ i;
                }
            } अन्यथा {
                यदि (साँचो.integer == elements[i].integer) {
                    फर्काऊ i;
                }
            }
        }
    }
    फर्काऊ -1;
}

UBool UVector::ensureCapacity(int32_t minimumCapacity, UErrorCode &status) {
    यदि (U_FAILURE(status)) {
        फर्काऊ असत्य;
    }
    यदि (minimumCapacity < 0) {
        status = U_ILLEGAL_ARGUMENT_ERROR;
        फर्काऊ असत्य;
    }
    यदि (capacity < minimumCapacity) {
        यदि (capacity > (INT32_MAX - 1) / 2) {        	// integer overflow check
            status = U_ILLEGAL_ARGUMENT_ERROR;
            फर्काऊ असत्य;
        }
        int32_t newCap = capacity * 2;
        यदि (newCap < minimumCapacity) {
            newCap = minimumCapacity;
        }
        यदि (newCap > static_cast<int32_t>(INT32_MAX / परिमाण(UElement))) { // integer overflow check
            // We keep the original memory contents on bad minimumCapacity.
            status = U_ILLEGAL_ARGUMENT_ERROR;
            फर्काऊ असत्य;
        }
        UElement* newElems = static_cast<UElement*>(uprv_realloc(elements, परिमाण(UElement) * newCap));
        यदि (newElems == nullptr) {
            // We keep the original contents on the memory failure on realloc or bad minimumCapacity.
            status = U_MEMORY_ALLOCATION_ERROR;
            फर्काऊ असत्य;
        }
        elements = newElems;
        capacity = newCap;
    }
    फर्काऊ सत्य;
}

/**
 * Change the size of this vector as follows: If newSize is smaller,
 * then truncate the array, possibly deleting held elements for i >=
 * newSize.  If newSize is larger, grow the array, filling in new
 * slots with nullptr.
 */
शून्य UVector::setSize(int32_t newSize, UErrorCode &status) {
    यदि (!ensureCapacity(newSize, status)) {
        फर्काऊ;
    }
    यदि (newSize > count) {
        UElement empty;
        empty.pointer = nullptr;
        empty.integer = 0;
        लागि (int32_t i=count; i<newSize; ++i) {
            elements[i] = empty;
        }
    } अन्यथा {
        /* Most efficient to count down */
        लागि (int32_t i=count-1; i>=newSize; --i) {
            removeElementAt(i);
        }
    }
    count = newSize;
}

/**
 * Fill in the given array with all elements of this vector.
 */
शून्य** UVector::toArray(शून्य** result) अचर {
    शून्य** a = result;
    लागि (पूर्णाङ्क i=0; i<count; ++i) {
        *a++ = elements[i].pointer;
    }
    फर्काऊ result;
}

UObjectDeleter *UVector::setDeleter(UObjectDeleter *d) {
    UObjectDeleter *old = deleter;
    deleter = d;
    फर्काऊ old;
}

UElementsAreEqual *UVector::setComparer(UElementsAreEqual *d) {
    UElementsAreEqual *old = comparer;
    comparer = d;
    फर्काऊ old;
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
    यदि (0 <= index && index < count) {
        e = elements[index].pointer;
        लागि (int32_t i=index; i<count-1; ++i) {
            elements[i] = elements[i+1];
        }
        --count;
    }
    /* else index out of range */
    फर्काऊ e;
}

/**
 * Insert the given object into this vector at its sorted position
 * as defined by 'compare'.  The current elements are assumed to
 * be sorted already.
 */
शून्य UVector::sorted_घुसाउ(शून्य* obj, UElementComparator *compare, UErrorCode& ec) {
    UElement e;
    e.pointer = obj;
    sorted_घुसाउ(e, compare, ec);
}

/**
 * Insert the given integer into this vector at its sorted position
 * as defined by 'compare'.  The current elements are assumed to
 * be sorted already.
 */
शून्य UVector::sorted_घुसाउ(int32_t obj, UElementComparator *compare, UErrorCode& ec) {
    U_ASSERT(deleter == nullptr);
    UElement e {};
    e.integer = obj;
    sorted_घुसाउ(e, compare, ec);
}

// ASSUME elements[] IS CURRENTLY SORTED
शून्य UVector::sorted_घुसाउ(UElement e, UElementComparator *compare, UErrorCode& ec) {
    // Perform a binary search for the location to insert tok at.  Tok
    // will be inserted between two elements a and b such that a <=
    // tok && tok < b, where there is a 'virtual' elements[-1] always
    // less than tok and a 'virtual' elements[count] always greater
    // than tok.
    यदि (!ensureCapacity(count + 1, ec)) {
        यदि (deleter != nullptr) {
            (*deleter)(e.pointer);
        }
        फर्काऊ;
    }
    int32_t min = 0, max = count;
    जबसम्म (min != max) {
        int32_t probe = (min + max) / 2;
        int32_t c = (*compare)(elements[probe], e);
        यदि (c > 0) {
            max = probe;
        } अन्यथा {
            // assert(c <= 0);
            min = probe + 1;
        }
    }
    लागि (int32_t i=count; i>min; --i) {
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
sortComparator(अचर शून्य *context, अचर शून्य *left, अचर शून्य *दायाँ) {
    UElementComparator *compare = *static_cast<UElementComparator * अचर *>(context);
    UElement e1 = *static_cast<अचर UElement *>(left);
    UElement e2 = *static_cast<अचर UElement *>(दायाँ);
    int32_t result = (*compare)(e1, e2);
    फर्काऊ result;
}


/**
  *  Array sort comparison function for use from UVector::sorti()
  *  Compares int32_t vector elements.
  */
स्थिर int32_t U_CALLCONV
sortiComparator(अचर शून्य * /*context */, अचर शून्य *left, अचर शून्य *दायाँ) {
    अचर UElement *e1 = static_cast<अचर UElement *>(left);
    अचर UElement *e2 = static_cast<अचर UElement *>(दायाँ);
    int32_t result = e1->integer < e2->integer? -1 :
                     e1->integer == e2->integer? 0 : 1;
    फर्काऊ result;
}

/**
  * Sort the vector, assuming it contains ints.
  *     (A more general sort would take a comparison function, but it's
  *     not clear whether UVector's UElementComparator or
  *     UComparator from uprv_sortAray would be more appropriate.)
  */
शून्य UVector::sorti(UErrorCode &ec) {
    यदि (U_SUCCESS(ec)) {
        uprv_sortArray(elements, count, परिमाण(UElement),
                       sortiComparator, nullptr,  असत्य, &ec);
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
    यदि (U_SUCCESS(ec)) {
        uprv_sortArray(elements, count, परिमाण(UElement),
                       sortComparator, &compare, असत्य, &ec);
    }
}


/**
 *  Stable sort with a user supplied comparator of type UComparator.
 */
शून्य UVector::sortWithUComparator(UComparator *compare, अचर शून्य *context, UErrorCode &ec) {
    यदि (U_SUCCESS(ec)) {
        uprv_sortArray(elements, count, परिमाण(UElement),
                       compare, context, सत्य, &ec);
    }
}

U_NAMESPACE_END

