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

#ଅନ୍ତର୍ଭାଵ "uvector.h"
#ଅନ୍ତର୍ଭାଵ "cmemory.h"
#ଅନ୍ତର୍ଭାଵ "uarrsort.h"
#ଅନ୍ତର୍ଭାଵ "uelement.h"

U_NAMESPACE_BEGIN

constexpr int32_t DEFAULT_CAPACITY = 8;

/*
 * Constants for hinting whether a key is an integer
 * or a pointer.  If a hint bit is zero, then the associated
 * token is assumed to be an integer. This is needed for iSeries
 */
constexpr int8_t HINT_ଚାବି_POINTER = 1;
constexpr int8_t HINT_ଚାବି_INTEGER = 0;
 
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
    ଯଦି (U_FAILURE(status)) {
        ଫେରନ୍ତୁ;
    }
    // Fix bogus initialCapacity values; avoid malloc(0) and integer overflow
    ଯଦି ((initialCapacity < 1) || (initialCapacity > static_cast<int32_t>(INT32_MAX / ଆକାର(UElement)))) {
        initialCapacity = DEFAULT_CAPACITY;
    }
    elements = static_cast<UElement*>(uprv_malloc(ଆକାର(UElement) * initialCapacity));
    ଯଦି (elements == nullptr) {
        status = U_MEMORY_ALLOCATION_ERROR;
    } ଅନ୍ୟଥା {
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
ଶୂନ୍ୟ UVector::assign(ଅଚଳ UVector& other, UElementAssigner *assign, UErrorCode &ec) {
    ଯଦି (ensureCapacity(other.count, ec)) {
        setSize(other.count, ec);
        ଯଦି (U_SUCCESS(ec)) {
            ପାଇଁ (int32_t i=0; i<other.count; ++i) {
                ଯଦି (elements[i].pointer != nullptr && deleter != nullptr) {
                    (*deleter)(elements[i].pointer);
                }
                (*assign)(&elements[i], &other.elements[i]);
            }
        }
    }
}

// This only does something sensible if this object has a non-null comparer
ତାର୍କିକ UVector::operator==(ଅଚଳ UVector& other) ଅଚଳ {
    U_ASSERT(comparer != nullptr);
    ଯଦି (count != other.count) ଫେରନ୍ତୁ ମିଥ୍ୟା;
    ଯଦି (comparer != nullptr) {
        // Compare using this object's comparer
        ପାଇଁ (int32_t i=0; i<count; ++i) {
            ଯଦି (!(*comparer)(elements[i], other.elements[i])) {
                ଫେରନ୍ତୁ ମିଥ୍ୟା;
            }
        }
    }
    ଫେରନ୍ତୁ ସତ୍ୟ;
}

ଶୂନ୍ୟ UVector::addElement(ଶୂନ୍ୟ* obj, UErrorCode &status) {
    U_ASSERT(deleter == nullptr);
    ଯଦି (ensureCapacity(count + 1, status)) {
        elements[count++].pointer = obj;
    }
}

ଶୂନ୍ୟ UVector::adoptElement(ଶୂନ୍ୟ* obj, UErrorCode &status) {
    U_ASSERT(deleter != nullptr);
    ଯଦି (ensureCapacity(count + 1, status)) {
        elements[count++].pointer = obj;
    } ଅନ୍ୟଥା {
        (*deleter)(obj);
    }
}
ଶୂନ୍ୟ UVector::addElement(int32_t elem, UErrorCode &status) {
    U_ASSERT(deleter == nullptr);  // Usage error. Mixing up ints and pointers.
    ଯଦି (ensureCapacity(count + 1, status)) {
        elements[count].pointer = nullptr;     // Pointers may be bigger than ints.
        elements[count].integer = elem;
        count++;
    }
}

ଶୂନ୍ୟ UVector::setElementAt(ଶୂନ୍ୟ* obj, int32_t index) {
    ଯଦି (0 <= index && index < count) {
        ଯଦି (elements[index].pointer != nullptr && deleter != nullptr) {
            (*deleter)(elements[index].pointer);
        }
        elements[index].pointer = obj;
    } ଅନ୍ୟଥା {
        /* index out of range */
        ଯଦି (deleter != nullptr) {
            (*deleter)(obj);
        }
    }
}

ଶୂନ୍ୟ UVector::setElementAt(int32_t elem, int32_t index) {
    U_ASSERT(deleter == nullptr);  // Usage error. Mixing up ints and pointers.
    ଯଦି (0 <= index && index < count) {
        elements[index].pointer = nullptr;
        elements[index].integer = elem;
    }
    /* else index out of range */
}

ଶୂନ୍ୟ UVector::ଭର୍ତ୍ତି_Element_At(ଶୂନ୍ୟ* obj, int32_t index, UErrorCode &status) {
    ଯଦି (ensureCapacity(count + 1, status)) {
        ଯଦି (0 <= index && index <= count) {
            ପାଇଁ (int32_t i=count; i>index; --i) {
                elements[i] = elements[i-1];
            }
            elements[index].pointer = obj;
            ++count;
        } ଅନ୍ୟଥା {
            /* index out of range */
            status = U_ILLEGAL_ARGUMENT_ERROR;
        }
    }
    ଯଦି (U_FAILURE(status) && deleter != nullptr) {
        (*deleter)(obj);
    }
}

ଶୂନ୍ୟ UVector::ଭର୍ତ୍ତି_Element_At(int32_t elem, int32_t index, UErrorCode &status) {
    U_ASSERT(deleter == nullptr);  // Usage error. Mixing up ints and pointers.
    // must have 0 <= index <= count
    ଯଦି (ensureCapacity(count + 1, status)) {
        ଯଦି (0 <= index && index <= count) {
            ପାଇଁ (int32_t i=count; i>index; --i) {
                elements[i] = elements[i-1];
            }
            elements[index].pointer = nullptr;
            elements[index].integer = elem;
            ++count;
        } ଅନ୍ୟଥା {
            /* index out of range */
            status = U_ILLEGAL_ARGUMENT_ERROR;
        }
    }
}

ଶୂନ୍ୟ* UVector::elementAt(int32_t index) ଅଚଳ {
    ଫେରନ୍ତୁ (0 <= index && index < count) ? elements[index].pointer : nullptr;
}

int32_t UVector::elementAti(int32_t index) ଅଚଳ {
    ଫେରନ୍ତୁ (0 <= index && index < count) ? elements[index].integer : 0;
}

UBool UVector::containsAll(ଅଚଳ UVector& other) ଅଚଳ {
    ପାଇଁ (int32_t i=0; i<other.size(); ++i) {
        ଯଦି (indexOf(other.elements[i]) < 0) {
            ଫେରନ୍ତୁ ମିଥ୍ୟା;
        }
    }
    ଫେରନ୍ତୁ ସତ୍ୟ;
}

UBool UVector::containsNone(ଅଚଳ UVector& other) ଅଚଳ {
    ପାଇଁ (int32_t i=0; i<other.size(); ++i) {
        ଯଦି (indexOf(other.elements[i]) >= 0) {
            ଫେରନ୍ତୁ ମିଥ୍ୟା;
        }
    }
    ଫେରନ୍ତୁ ସତ୍ୟ;
}

UBool UVector::removeAll(ଅଚଳ UVector& other) {
    UBool changed = ମିଥ୍ୟା;
    ପାଇଁ (int32_t i=0; i<other.size(); ++i) {
        int32_t j = indexOf(other.elements[i]);
        ଯଦି (j >= 0) {
            removeElementAt(j);
            changed = ସତ୍ୟ;
        }
    }
    ଫେରନ୍ତୁ changed;
}

UBool UVector::retainAll(ଅଚଳ UVector& other) {
    UBool changed = ମିଥ୍ୟା;
    ପାଇଁ (int32_t j=size()-1; j>=0; --j) {
        int32_t i = other.indexOf(elements[j]);
        ଯଦି (i < 0) {
            removeElementAt(j);
            changed = ସତ୍ୟ;
        }
    }
    ଫେରନ୍ତୁ changed;
}

ଶୂନ୍ୟ UVector::removeElementAt(int32_t index) {
    ଶୂନ୍ୟ* e = orphanElementAt(index);
    ଯଦି (e != nullptr && deleter != nullptr) {
        (*deleter)(e);
    }
}

UBool UVector::removeElement(ଶୂନ୍ୟ* obj) {
    int32_t i = indexOf(obj);
    ଯଦି (i >= 0) {
        removeElementAt(i);
        ଫେରନ୍ତୁ ସତ୍ୟ;
    }
    ଫେରନ୍ତୁ ମିଥ୍ୟା;
}

ଶୂନ୍ୟ UVector::removeAllElements() {
    ଯଦି (deleter != nullptr) {
        ପାଇଁ (int32_t i=0; i<count; ++i) {
            ଯଦି (elements[i].pointer != nullptr) {
                (*deleter)(elements[i].pointer);
            }
        }
    }
    count = 0;
}

UBool   UVector::equals(ଅଚଳ UVector &other) ଅଚଳ {
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା      i;

    ଯଦି (ଏହି->count != other.count) {
        ଫେରନ୍ତୁ ମିଥ୍ୟା;
    }
    ଯଦି (comparer == nullptr) {
        ପାଇଁ (i=0; i<count; i++) {
            ଯଦି (elements[i].pointer != other.elements[i].pointer) {
                ଫେରନ୍ତୁ ମିଥ୍ୟା;
            }
        }
    } ଅନ୍ୟଥା {
        UElement ଚାବି;
        ପାଇଁ (i=0; i<count; i++) {
            ଚାବି.pointer = &other.elements[i];
            ଯଦି (!(*comparer)(ଚାବି, elements[i])) {
                ଫେରନ୍ତୁ ମିଥ୍ୟା;
            }
        }
    }
    ଫେରନ୍ତୁ ସତ୍ୟ;
}



int32_t UVector::indexOf(ଶୂନ୍ୟ* obj, int32_t startIndex) ଅଚଳ {
    UElement ଚାବି;
    ଚାବି.pointer = obj;
    ଫେରନ୍ତୁ indexOf(ଚାବି, startIndex, HINT_ଚାବି_POINTER);
}

int32_t UVector::indexOf(int32_t obj, int32_t startIndex) ଅଚଳ {
    UElement ଚାବି;
    ଚାବି.integer = obj;
    ଫେରନ୍ତୁ indexOf(ଚାବି, startIndex, HINT_ଚାବି_INTEGER);
}

int32_t UVector::indexOf(UElement ଚାବି, int32_t startIndex, int8_t hint) ଅଚଳ {
    ଯଦି (comparer != nullptr) {
        ପାଇଁ (int32_t i=startIndex; i<count; ++i) {
            ଯଦି ((*comparer)(ଚାବି, elements[i])) {
                ଫେରନ୍ତୁ i;
            }
        }
    } ଅନ୍ୟଥା {
        ପାଇଁ (int32_t i=startIndex; i<count; ++i) {
            /* Pointers are not always the same size as ints so to perform
             * a valid comparison we need to know whether we are being
             * provided an int or a pointer. */
            ଯଦି (hint & HINT_ଚାବି_POINTER) {
                ଯଦି (ଚାବି.pointer == elements[i].pointer) {
                    ଫେରନ୍ତୁ i;
                }
            } ଅନ୍ୟଥା {
                ଯଦି (ଚାବି.integer == elements[i].integer) {
                    ଫେରନ୍ତୁ i;
                }
            }
        }
    }
    ଫେରନ୍ତୁ -1;
}

UBool UVector::ensureCapacity(int32_t minimumCapacity, UErrorCode &status) {
    ଯଦି (U_FAILURE(status)) {
        ଫେରନ୍ତୁ ମିଥ୍ୟା;
    }
    ଯଦି (minimumCapacity < 0) {
        status = U_ILLEGAL_ARGUMENT_ERROR;
        ଫେରନ୍ତୁ ମିଥ୍ୟା;
    }
    ଯଦି (capacity < minimumCapacity) {
        ଯଦି (capacity > (INT32_MAX - 1) / 2) {        	// integer overflow check
            status = U_ILLEGAL_ARGUMENT_ERROR;
            ଫେରନ୍ତୁ ମିଥ୍ୟା;
        }
        int32_t newCap = capacity * 2;
        ଯଦି (newCap < minimumCapacity) {
            newCap = minimumCapacity;
        }
        ଯଦି (newCap > static_cast<int32_t>(INT32_MAX / ଆକାର(UElement))) { // integer overflow check
            // We keep the original memory contents on bad minimumCapacity.
            status = U_ILLEGAL_ARGUMENT_ERROR;
            ଫେରନ୍ତୁ ମିଥ୍ୟା;
        }
        UElement* newElems = static_cast<UElement*>(uprv_realloc(elements, ଆକାର(UElement) * newCap));
        ଯଦି (newElems == nullptr) {
            // We keep the original contents on the memory failure on realloc or bad minimumCapacity.
            status = U_MEMORY_ALLOCATION_ERROR;
            ଫେରନ୍ତୁ ମିଥ୍ୟା;
        }
        elements = newElems;
        capacity = newCap;
    }
    ଫେରନ୍ତୁ ସତ୍ୟ;
}

/**
 * Change the size of this vector as follows: If newSize is smaller,
 * then truncate the array, possibly deleting held elements for i >=
 * newSize.  If newSize is larger, grow the array, filling in new
 * slots with nullptr.
 */
ଶୂନ୍ୟ UVector::setSize(int32_t newSize, UErrorCode &status) {
    ଯଦି (!ensureCapacity(newSize, status)) {
        ଫେରନ୍ତୁ;
    }
    ଯଦି (newSize > count) {
        UElement empty;
        empty.pointer = nullptr;
        empty.integer = 0;
        ପାଇଁ (int32_t i=count; i<newSize; ++i) {
            elements[i] = empty;
        }
    } ଅନ୍ୟଥା {
        /* Most efficient to count down */
        ପାଇଁ (int32_t i=count-1; i>=newSize; --i) {
            removeElementAt(i);
        }
    }
    count = newSize;
}

/**
 * Fill in the given array with all elements of this vector.
 */
ଶୂନ୍ୟ** UVector::toArray(ଶୂନ୍ୟ** result) ଅଚଳ {
    ଶୂନ୍ୟ** a = result;
    ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା i=0; i<count; ++i) {
        *a++ = elements[i].pointer;
    }
    ଫେରନ୍ତୁ result;
}

UObjectDeleter *UVector::setDeleter(UObjectDeleter *d) {
    UObjectDeleter *old = deleter;
    deleter = d;
    ଫେରନ୍ତୁ old;
}

UElementsAreEqual *UVector::setComparer(UElementsAreEqual *d) {
    UElementsAreEqual *old = comparer;
    comparer = d;
    ଫେରନ୍ତୁ old;
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
ଶୂନ୍ୟ* UVector::orphanElementAt(int32_t index) {
    ଶୂନ୍ୟ* e = nullptr;
    ଯଦି (0 <= index && index < count) {
        e = elements[index].pointer;
        ପାଇଁ (int32_t i=index; i<count-1; ++i) {
            elements[i] = elements[i+1];
        }
        --count;
    }
    /* else index out of range */
    ଫେରନ୍ତୁ e;
}

/**
 * Insert the given object into this vector at its sorted position
 * as defined by 'compare'.  The current elements are assumed to
 * be sorted already.
 */
ଶୂନ୍ୟ UVector::sorted_ଭର୍ତ୍ତି(ଶୂନ୍ୟ* obj, UElementComparator *compare, UErrorCode& ec) {
    UElement e;
    e.pointer = obj;
    sorted_ଭର୍ତ୍ତି(e, compare, ec);
}

/**
 * Insert the given integer into this vector at its sorted position
 * as defined by 'compare'.  The current elements are assumed to
 * be sorted already.
 */
ଶୂନ୍ୟ UVector::sorted_ଭର୍ତ୍ତି(int32_t obj, UElementComparator *compare, UErrorCode& ec) {
    U_ASSERT(deleter == nullptr);
    UElement e {};
    e.integer = obj;
    sorted_ଭର୍ତ୍ତି(e, compare, ec);
}

// ASSUME elements[] IS CURRENTLY SORTED
ଶୂନ୍ୟ UVector::sorted_ଭର୍ତ୍ତି(UElement e, UElementComparator *compare, UErrorCode& ec) {
    // Perform a binary search for the location to insert tok at.  Tok
    // will be inserted between two elements a and b such that a <=
    // tok && tok < b, where there is a 'virtual' elements[-1] always
    // less than tok and a 'virtual' elements[count] always greater
    // than tok.
    ଯଦି (!ensureCapacity(count + 1, ec)) {
        ଯଦି (deleter != nullptr) {
            (*deleter)(e.pointer);
        }
        ଫେରନ୍ତୁ;
    }
    int32_t min = 0, max = count;
    ଯେତେବେଳେ (min != max) {
        int32_t probe = (min + max) / 2;
        int32_t c = (*compare)(elements[probe], e);
        ଯଦି (c > 0) {
            max = probe;
        } ଅନ୍ୟଥା {
            // assert(c <= 0);
            min = probe + 1;
        }
    }
    ପାଇଁ (int32_t i=count; i>min; --i) {
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
ସ୍ଥିର int32_t U_CALLCONV
sortComparator(ଅଚଳ ଶୂନ୍ୟ *context, ଅଚଳ ଶୂନ୍ୟ *left, ଅଚଳ ଶୂନ୍ୟ *ଡାହାଣ) {
    UElementComparator *compare = *static_cast<UElementComparator * ଅଚଳ *>(context);
    UElement e1 = *static_cast<ଅଚଳ UElement *>(left);
    UElement e2 = *static_cast<ଅଚଳ UElement *>(ଡାହାଣ);
    int32_t result = (*compare)(e1, e2);
    ଫେରନ୍ତୁ result;
}


/**
  *  Array sort comparison function for use from UVector::sorti()
  *  Compares int32_t vector elements.
  */
ସ୍ଥିର int32_t U_CALLCONV
sortiComparator(ଅଚଳ ଶୂନ୍ୟ * /*context */, ଅଚଳ ଶୂନ୍ୟ *left, ଅଚଳ ଶୂନ୍ୟ *ଡାହାଣ) {
    ଅଚଳ UElement *e1 = static_cast<ଅଚଳ UElement *>(left);
    ଅଚଳ UElement *e2 = static_cast<ଅଚଳ UElement *>(ଡାହାଣ);
    int32_t result = e1->integer < e2->integer? -1 :
                     e1->integer == e2->integer? 0 : 1;
    ଫେରନ୍ତୁ result;
}

/**
  * Sort the vector, assuming it contains ints.
  *     (A more general sort would take a comparison function, but it's
  *     not clear whether UVector's UElementComparator or
  *     UComparator from uprv_sortAray would be more appropriate.)
  */
ଶୂନ୍ୟ UVector::sorti(UErrorCode &ec) {
    ଯଦି (U_SUCCESS(ec)) {
        uprv_sortArray(elements, count, ଆକାର(UElement),
                       sortiComparator, nullptr,  ମିଥ୍ୟା, &ec);
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
ଶୂନ୍ୟ UVector::sort(UElementComparator *compare, UErrorCode &ec) {
    ଯଦି (U_SUCCESS(ec)) {
        uprv_sortArray(elements, count, ଆକାର(UElement),
                       sortComparator, &compare, ମିଥ୍ୟା, &ec);
    }
}


/**
 *  Stable sort with a user supplied comparator of type UComparator.
 */
ଶୂନ୍ୟ UVector::sortWithUComparator(UComparator *compare, ଅଚଳ ଶୂନ୍ୟ *context, UErrorCode &ec) {
    ଯଦି (U_SUCCESS(ec)) {
        uprv_sortArray(elements, count, ଆକାର(UElement),
                       compare, context, ସତ୍ୟ, &ec);
    }
}

U_NAMESPACE_END

