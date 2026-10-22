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

#समावेश "uvector.h"
#समावेश "cmemory.h"
#समावेश "uarrsort.h"
#समावेश "uelement.h"

U_NAMESPACE_BEGIN

constexpr int32_t DEFAULT_CAPACITY = 8;

/*
 * Constants for hinting whether a key is an integer
 * or a pointer.  If a hint bit is zero, then the associated
 * token is assumed to be an integer. This is needed for iSeries
 */
constexpr int8_t HINT_कुंजी_POINTER = 1;
constexpr int8_t HINT_कुंजी_INTEGER = 0;
 
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
    अगर (U_FAILURE(status)) {
        वापस;
    }
    // Fix bogus initialCapacity values; avoid malloc(0) and integer overflow
    अगर ((initialCapacity < 1) || (initialCapacity > static_cast<int32_t>(INT32_MAX / माप(UElement)))) {
        initialCapacity = DEFAULT_CAPACITY;
    }
    elements = static_cast<UElement*>(uprv_malloc(माप(UElement) * initialCapacity));
    अगर (elements == nullptr) {
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
व्योम UVector::assign(स्थिर UVector& other, UElementAssigner *assign, UErrorCode &ec) {
    अगर (ensureCapacity(other.count, ec)) {
        set_आकार(other.count, ec);
        अगर (U_SUCCESS(ec)) {
            क्रम (int32_t i=0; i<other.count; ++i) {
                अगर (elements[i].pointer != nullptr && deleter != nullptr) {
                    (*deleter)(elements[i].pointer);
                }
                (*assign)(&elements[i], &other.elements[i]);
            }
        }
    }
}

// This only does something sensible if this object has a non-null comparer
bool UVector::चालक==(स्थिर UVector& other) स्थिर {
    U_ASSERT(comparer != nullptr);
    अगर (count != other.count) वापस false;
    अगर (comparer != nullptr) {
        // Compare using this object's comparer
        क्रम (int32_t i=0; i<count; ++i) {
            अगर (!(*comparer)(elements[i], other.elements[i])) {
                वापस false;
            }
        }
    }
    वापस true;
}

व्योम UVector::addElement(व्योम* obj, UErrorCode &status) {
    U_ASSERT(deleter == nullptr);
    अगर (ensureCapacity(count + 1, status)) {
        elements[count++].pointer = obj;
    }
}

व्योम UVector::adoptElement(व्योम* obj, UErrorCode &status) {
    U_ASSERT(deleter != nullptr);
    अगर (ensureCapacity(count + 1, status)) {
        elements[count++].pointer = obj;
    } अन्यथा {
        (*deleter)(obj);
    }
}
व्योम UVector::addElement(int32_t elem, UErrorCode &status) {
    U_ASSERT(deleter == nullptr);  // Usage error. Mixing up ints and pointers.
    अगर (ensureCapacity(count + 1, status)) {
        elements[count].pointer = nullptr;     // Pointers may be bigger than ints.
        elements[count].integer = elem;
        count++;
    }
}

व्योम UVector::setElementAt(व्योम* obj, int32_t सूचकांक) {
    अगर (0 <= सूचकांक && सूचकांक < count) {
        अगर (elements[सूचकांक].pointer != nullptr && deleter != nullptr) {
            (*deleter)(elements[सूचकांक].pointer);
        }
        elements[सूचकांक].pointer = obj;
    } अन्यथा {
        /* index out of range */
        अगर (deleter != nullptr) {
            (*deleter)(obj);
        }
    }
}

व्योम UVector::setElementAt(int32_t elem, int32_t सूचकांक) {
    U_ASSERT(deleter == nullptr);  // Usage error. Mixing up ints and pointers.
    अगर (0 <= सूचकांक && सूचकांक < count) {
        elements[सूचकांक].pointer = nullptr;
        elements[सूचकांक].integer = elem;
    }
    /* else index out of range */
}

व्योम UVector::जोड़ो_Element_At(व्योम* obj, int32_t सूचकांक, UErrorCode &status) {
    अगर (ensureCapacity(count + 1, status)) {
        अगर (0 <= सूचकांक && सूचकांक <= count) {
            क्रम (int32_t i=count; i>सूचकांक; --i) {
                elements[i] = elements[i-1];
            }
            elements[सूचकांक].pointer = obj;
            ++count;
        } अन्यथा {
            /* index out of range */
            status = U_ILLEGAL_ARGUMENT_ERROR;
        }
    }
    अगर (U_FAILURE(status) && deleter != nullptr) {
        (*deleter)(obj);
    }
}

व्योम UVector::जोड़ो_Element_At(int32_t elem, int32_t सूचकांक, UErrorCode &status) {
    U_ASSERT(deleter == nullptr);  // Usage error. Mixing up ints and pointers.
    // must have 0 <= index <= count
    अगर (ensureCapacity(count + 1, status)) {
        अगर (0 <= सूचकांक && सूचकांक <= count) {
            क्रम (int32_t i=count; i>सूचकांक; --i) {
                elements[i] = elements[i-1];
            }
            elements[सूचकांक].pointer = nullptr;
            elements[सूचकांक].integer = elem;
            ++count;
        } अन्यथा {
            /* index out of range */
            status = U_ILLEGAL_ARGUMENT_ERROR;
        }
    }
}

व्योम* UVector::elementAt(int32_t सूचकांक) स्थिर {
    वापस (0 <= सूचकांक && सूचकांक < count) ? elements[सूचकांक].pointer : nullptr;
}

int32_t UVector::elementAti(int32_t सूचकांक) स्थिर {
    वापस (0 <= सूचकांक && सूचकांक < count) ? elements[सूचकांक].integer : 0;
}

UBool UVector::containsAll(स्थिर UVector& other) स्थिर {
    क्रम (int32_t i=0; i<other.size(); ++i) {
        अगर (सूचकांक_Of(other.elements[i]) < 0) {
            वापस false;
        }
    }
    वापस true;
}

UBool UVector::containsNone(स्थिर UVector& other) स्थिर {
    क्रम (int32_t i=0; i<other.size(); ++i) {
        अगर (सूचकांक_Of(other.elements[i]) >= 0) {
            वापस false;
        }
    }
    वापस true;
}

UBool UVector::removeAll(स्थिर UVector& other) {
    UBool changed = false;
    क्रम (int32_t i=0; i<other.size(); ++i) {
        int32_t j = सूचकांक_Of(other.elements[i]);
        अगर (j >= 0) {
            removeElementAt(j);
            changed = true;
        }
    }
    वापस changed;
}

UBool UVector::retainAll(स्थिर UVector& other) {
    UBool changed = false;
    क्रम (int32_t j=size()-1; j>=0; --j) {
        int32_t i = other.सूचकांक_Of(elements[j]);
        अगर (i < 0) {
            removeElementAt(j);
            changed = true;
        }
    }
    वापस changed;
}

व्योम UVector::removeElementAt(int32_t सूचकांक) {
    व्योम* e = orphanElementAt(सूचकांक);
    अगर (e != nullptr && deleter != nullptr) {
        (*deleter)(e);
    }
}

UBool UVector::removeElement(व्योम* obj) {
    int32_t i = सूचकांक_Of(obj);
    अगर (i >= 0) {
        removeElementAt(i);
        वापस true;
    }
    वापस false;
}

व्योम UVector::removeAllElements() {
    अगर (deleter != nullptr) {
        क्रम (int32_t i=0; i<count; ++i) {
            अगर (elements[i].pointer != nullptr) {
                (*deleter)(elements[i].pointer);
            }
        }
    }
    count = 0;
}

UBool   UVector::equals(स्थिर UVector &other) स्थिर {
    पूर्णांक      i;

    अगर (यह->count != other.count) {
        वापस false;
    }
    अगर (comparer == nullptr) {
        क्रम (i=0; i<count; i++) {
            अगर (elements[i].pointer != other.elements[i].pointer) {
                वापस false;
            }
        }
    } अन्यथा {
        UElement कुंजी;
        क्रम (i=0; i<count; i++) {
            कुंजी.pointer = &other.elements[i];
            अगर (!(*comparer)(कुंजी, elements[i])) {
                वापस false;
            }
        }
    }
    वापस true;
}



int32_t UVector::सूचकांक_Of(व्योम* obj, int32_t start_सूचकांक) स्थिर {
    UElement कुंजी;
    कुंजी.pointer = obj;
    वापस सूचकांक_Of(कुंजी, start_सूचकांक, HINT_कुंजी_POINTER);
}

int32_t UVector::सूचकांक_Of(int32_t obj, int32_t start_सूचकांक) स्थिर {
    UElement कुंजी;
    कुंजी.integer = obj;
    वापस सूचकांक_Of(कुंजी, start_सूचकांक, HINT_कुंजी_INTEGER);
}

int32_t UVector::सूचकांक_Of(UElement कुंजी, int32_t start_सूचकांक, int8_t hint) स्थिर {
    अगर (comparer != nullptr) {
        क्रम (int32_t i=start_सूचकांक; i<count; ++i) {
            अगर ((*comparer)(कुंजी, elements[i])) {
                वापस i;
            }
        }
    } अन्यथा {
        क्रम (int32_t i=start_सूचकांक; i<count; ++i) {
            /* Pointers are not always the same size as ints so to perform
             * a valid comparison we need to know whether we are being
             * provided an int or a pointer. */
            अगर (hint & HINT_कुंजी_POINTER) {
                अगर (कुंजी.pointer == elements[i].pointer) {
                    वापस i;
                }
            } अन्यथा {
                अगर (कुंजी.integer == elements[i].integer) {
                    वापस i;
                }
            }
        }
    }
    वापस -1;
}

UBool UVector::ensureCapacity(int32_t minimumCapacity, UErrorCode &status) {
    अगर (U_FAILURE(status)) {
        वापस false;
    }
    अगर (minimumCapacity < 0) {
        status = U_ILLEGAL_ARGUMENT_ERROR;
        वापस false;
    }
    अगर (capacity < minimumCapacity) {
        अगर (capacity > (INT32_MAX - 1) / 2) {        	// integer overflow check
            status = U_ILLEGAL_ARGUMENT_ERROR;
            वापस false;
        }
        int32_t newCap = capacity * 2;
        अगर (newCap < minimumCapacity) {
            newCap = minimumCapacity;
        }
        अगर (newCap > static_cast<int32_t>(INT32_MAX / माप(UElement))) { // integer overflow check
            // We keep the original memory contents on bad minimumCapacity.
            status = U_ILLEGAL_ARGUMENT_ERROR;
            वापस false;
        }
        UElement* newElems = static_cast<UElement*>(uprv_realloc(elements, माप(UElement) * newCap));
        अगर (newElems == nullptr) {
            // We keep the original contents on the memory failure on realloc or bad minimumCapacity.
            status = U_MEMORY_ALLOCATION_ERROR;
            वापस false;
        }
        elements = newElems;
        capacity = newCap;
    }
    वापस true;
}

/**
 * Change the size of this vector as follows: If newSize is smaller,
 * then truncate the array, possibly deleting held elements for i >=
 * newSize.  If newSize is larger, grow the array, filling in new
 * slots with nullptr.
 */
व्योम UVector::set_आकार(int32_t new_आकार, UErrorCode &status) {
    अगर (!ensureCapacity(new_आकार, status)) {
        वापस;
    }
    अगर (new_आकार > count) {
        UElement empty;
        empty.pointer = nullptr;
        empty.integer = 0;
        क्रम (int32_t i=count; i<new_आकार; ++i) {
            elements[i] = empty;
        }
    } अन्यथा {
        /* Most efficient to count down */
        क्रम (int32_t i=count-1; i>=new_आकार; --i) {
            removeElementAt(i);
        }
    }
    count = new_आकार;
}

/**
 * Fill in the given array with all elements of this vector.
 */
व्योम** UVector::toArray(व्योम** परिणाम) स्थिर {
    व्योम** a = परिणाम;
    क्रम (पूर्णांक i=0; i<count; ++i) {
        *a++ = elements[i].pointer;
    }
    वापस परिणाम;
}

UObjectDeleter *UVector::setDeleter(UObjectDeleter *d) {
    UObjectDeleter *old = deleter;
    deleter = d;
    वापस old;
}

UElementsAreEqual *UVector::setComparer(UElementsAreEqual *d) {
    UElementsAreEqual *old = comparer;
    comparer = d;
    वापस old;
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
व्योम* UVector::orphanElementAt(int32_t सूचकांक) {
    व्योम* e = nullptr;
    अगर (0 <= सूचकांक && सूचकांक < count) {
        e = elements[सूचकांक].pointer;
        क्रम (int32_t i=सूचकांक; i<count-1; ++i) {
            elements[i] = elements[i+1];
        }
        --count;
    }
    /* else index out of range */
    वापस e;
}

/**
 * Insert the given object into this vector at its sorted position
 * as defined by 'compare'.  The current elements are assumed to
 * be sorted already.
 */
व्योम UVector::sorted_जोड़ो(व्योम* obj, UElementComparator *compare, UErrorCode& ec) {
    UElement e;
    e.pointer = obj;
    sorted_जोड़ो(e, compare, ec);
}

/**
 * Insert the given integer into this vector at its sorted position
 * as defined by 'compare'.  The current elements are assumed to
 * be sorted already.
 */
व्योम UVector::sorted_जोड़ो(int32_t obj, UElementComparator *compare, UErrorCode& ec) {
    U_ASSERT(deleter == nullptr);
    UElement e {};
    e.integer = obj;
    sorted_जोड़ो(e, compare, ec);
}

// ASSUME elements[] IS CURRENTLY SORTED
व्योम UVector::sorted_जोड़ो(UElement e, UElementComparator *compare, UErrorCode& ec) {
    // Perform a binary search for the location to insert tok at.  Tok
    // will be inserted between two elements a and b such that a <=
    // tok && tok < b, where there is a 'virtual' elements[-1] always
    // less than tok and a 'virtual' elements[count] always greater
    // than tok.
    अगर (!ensureCapacity(count + 1, ec)) {
        अगर (deleter != nullptr) {
            (*deleter)(e.pointer);
        }
        वापस;
    }
    int32_t min = 0, max = count;
    जबतक (min != max) {
        int32_t probe = (min + max) / 2;
        int32_t c = (*compare)(elements[probe], e);
        अगर (c > 0) {
            max = probe;
        } अन्यथा {
            // assert(c <= 0);
            min = probe + 1;
        }
    }
    क्रम (int32_t i=count; i>min; --i) {
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
अटल int32_t U_CALLCONV
sortComparator(स्थिर व्योम *context, स्थिर व्योम *बाँए, स्थिर व्योम *दाँए) {
    UElementComparator *compare = *static_cast<UElementComparator * स्थिर *>(context);
    UElement e1 = *static_cast<स्थिर UElement *>(बाँए);
    UElement e2 = *static_cast<स्थिर UElement *>(दाँए);
    int32_t परिणाम = (*compare)(e1, e2);
    वापस परिणाम;
}


/**
  *  Array sort comparison function for use from UVector::sorti()
  *  Compares int32_t vector elements.
  */
अटल int32_t U_CALLCONV
sortiComparator(स्थिर व्योम * /*context */, स्थिर व्योम *बाँए, स्थिर व्योम *दाँए) {
    स्थिर UElement *e1 = static_cast<स्थिर UElement *>(बाँए);
    स्थिर UElement *e2 = static_cast<स्थिर UElement *>(दाँए);
    int32_t परिणाम = e1->integer < e2->integer? -1 :
                     e1->integer == e2->integer? 0 : 1;
    वापस परिणाम;
}

/**
  * Sort the vector, assuming it contains ints.
  *     (A more general sort would take a comparison function, but it's
  *     not clear whether UVector's UElementComparator or
  *     UComparator from uprv_sortAray would be more appropriate.)
  */
व्योम UVector::sorti(UErrorCode &ec) {
    अगर (U_SUCCESS(ec)) {
        uprv_sortArray(elements, count, माप(UElement),
                       sortiComparator, nullptr,  false, &ec);
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
व्योम UVector::sort(UElementComparator *compare, UErrorCode &ec) {
    अगर (U_SUCCESS(ec)) {
        uprv_sortArray(elements, count, माप(UElement),
                       sortComparator, &compare, false, &ec);
    }
}


/**
 *  Stable sort with a user supplied comparator of type UComparator.
 */
व्योम UVector::sortWithUComparator(UComparator *compare, स्थिर व्योम *context, UErrorCode &ec) {
    अगर (U_SUCCESS(ec)) {
        uprv_sortArray(elements, count, माप(UElement),
                       compare, context, true, &ec);
    }
}

U_NAMESPACE_END

