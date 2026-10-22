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

#അന്തര്ഭാവ "uvector.h"
#അന്തര്ഭാവ "cmemory.h"
#അന്തര്ഭാവ "uarrsort.h"
#അന്തര്ഭാവ "uelement.h"

U_NAMESPACE_BEGIN

constexpr int32_t DEFAULT_CAPACITY = 8;

/*
 * Constants for hinting whether a key is an integer
 * or a pointer.  If a hint bit is zero, then the associated
 * token is assumed to be an integer. This is needed for iSeries
 */
constexpr int8_t HINT_താക്കോൽ_POINTER = 1;
constexpr int8_t HINT_താക്കോൽ_INTEGER = 0;
 
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
    എങ്കിൽ (U_FAILURE(status)) {
        തിരികെ;
    }
    // Fix bogus initialCapacity values; avoid malloc(0) and integer overflow
    എങ്കിൽ ((initialCapacity < 1) || (initialCapacity > static_cast<int32_t>(INT32_MAX / വലുപ്പം(UElement)))) {
        initialCapacity = DEFAULT_CAPACITY;
    }
    elements = static_cast<UElement*>(uprv_malloc(വലുപ്പം(UElement) * initialCapacity));
    എങ്കിൽ (elements == nullptr) {
        status = U_MEMORY_ALLOCATION_ERROR;
    } അല്ലെങ്കിൽ {
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
ശൂന്യം UVector::assign(സ്ഥിരം UVector& other, UElementAssigner *assign, UErrorCode &ec) {
    എങ്കിൽ (ensureCapacity(other.count, ec)) {
        setSize(other.count, ec);
        എങ്കിൽ (U_SUCCESS(ec)) {
            വേണ്ടി (int32_t i=0; i<other.count; ++i) {
                എങ്കിൽ (elements[i].pointer != nullptr && deleter != nullptr) {
                    (*deleter)(elements[i].pointer);
                }
                (*assign)(&elements[i], &other.elements[i]);
            }
        }
    }
}

// This only does something sensible if this object has a non-null comparer
തർക്കം UVector::operator==(സ്ഥിരം UVector& other) സ്ഥിരം {
    U_ASSERT(comparer != nullptr);
    എങ്കിൽ (count != other.count) തിരികെ അസത്യം;
    എങ്കിൽ (comparer != nullptr) {
        // Compare using this object's comparer
        വേണ്ടി (int32_t i=0; i<count; ++i) {
            എങ്കിൽ (!(*comparer)(elements[i], other.elements[i])) {
                തിരികെ അസത്യം;
            }
        }
    }
    തിരികെ സത്യം;
}

ശൂന്യം UVector::addElement(ശൂന്യം* obj, UErrorCode &status) {
    U_ASSERT(deleter == nullptr);
    എങ്കിൽ (ensureCapacity(count + 1, status)) {
        elements[count++].pointer = obj;
    }
}

ശൂന്യം UVector::adoptElement(ശൂന്യം* obj, UErrorCode &status) {
    U_ASSERT(deleter != nullptr);
    എങ്കിൽ (ensureCapacity(count + 1, status)) {
        elements[count++].pointer = obj;
    } അല്ലെങ്കിൽ {
        (*deleter)(obj);
    }
}
ശൂന്യം UVector::addElement(int32_t elem, UErrorCode &status) {
    U_ASSERT(deleter == nullptr);  // Usage error. Mixing up ints and pointers.
    എങ്കിൽ (ensureCapacity(count + 1, status)) {
        elements[count].pointer = nullptr;     // Pointers may be bigger than ints.
        elements[count].integer = elem;
        count++;
    }
}

ശൂന്യം UVector::setElementAt(ശൂന്യം* obj, int32_t index) {
    എങ്കിൽ (0 <= index && index < count) {
        എങ്കിൽ (elements[index].pointer != nullptr && deleter != nullptr) {
            (*deleter)(elements[index].pointer);
        }
        elements[index].pointer = obj;
    } അല്ലെങ്കിൽ {
        /* index out of range */
        എങ്കിൽ (deleter != nullptr) {
            (*deleter)(obj);
        }
    }
}

ശൂന്യം UVector::setElementAt(int32_t elem, int32_t index) {
    U_ASSERT(deleter == nullptr);  // Usage error. Mixing up ints and pointers.
    എങ്കിൽ (0 <= index && index < count) {
        elements[index].pointer = nullptr;
        elements[index].integer = elem;
    }
    /* else index out of range */
}

ശൂന്യം UVector::തിരുകുക_Element_At(ശൂന്യം* obj, int32_t index, UErrorCode &status) {
    എങ്കിൽ (ensureCapacity(count + 1, status)) {
        എങ്കിൽ (0 <= index && index <= count) {
            വേണ്ടി (int32_t i=count; i>index; --i) {
                elements[i] = elements[i-1];
            }
            elements[index].pointer = obj;
            ++count;
        } അല്ലെങ്കിൽ {
            /* index out of range */
            status = U_ILLEGAL_ARGUMENT_ERROR;
        }
    }
    എങ്കിൽ (U_FAILURE(status) && deleter != nullptr) {
        (*deleter)(obj);
    }
}

ശൂന്യം UVector::തിരുകുക_Element_At(int32_t elem, int32_t index, UErrorCode &status) {
    U_ASSERT(deleter == nullptr);  // Usage error. Mixing up ints and pointers.
    // must have 0 <= index <= count
    എങ്കിൽ (ensureCapacity(count + 1, status)) {
        എങ്കിൽ (0 <= index && index <= count) {
            വേണ്ടി (int32_t i=count; i>index; --i) {
                elements[i] = elements[i-1];
            }
            elements[index].pointer = nullptr;
            elements[index].integer = elem;
            ++count;
        } അല്ലെങ്കിൽ {
            /* index out of range */
            status = U_ILLEGAL_ARGUMENT_ERROR;
        }
    }
}

ശൂന്യം* UVector::elementAt(int32_t index) സ്ഥിരം {
    തിരികെ (0 <= index && index < count) ? elements[index].pointer : nullptr;
}

int32_t UVector::elementAti(int32_t index) സ്ഥിരം {
    തിരികെ (0 <= index && index < count) ? elements[index].integer : 0;
}

UBool UVector::containsAll(സ്ഥിരം UVector& other) സ്ഥിരം {
    വേണ്ടി (int32_t i=0; i<other.size(); ++i) {
        എങ്കിൽ (indexOf(other.elements[i]) < 0) {
            തിരികെ അസത്യം;
        }
    }
    തിരികെ സത്യം;
}

UBool UVector::containsNone(സ്ഥിരം UVector& other) സ്ഥിരം {
    വേണ്ടി (int32_t i=0; i<other.size(); ++i) {
        എങ്കിൽ (indexOf(other.elements[i]) >= 0) {
            തിരികെ അസത്യം;
        }
    }
    തിരികെ സത്യം;
}

UBool UVector::removeAll(സ്ഥിരം UVector& other) {
    UBool changed = അസത്യം;
    വേണ്ടി (int32_t i=0; i<other.size(); ++i) {
        int32_t j = indexOf(other.elements[i]);
        എങ്കിൽ (j >= 0) {
            removeElementAt(j);
            changed = സത്യം;
        }
    }
    തിരികെ changed;
}

UBool UVector::retainAll(സ്ഥിരം UVector& other) {
    UBool changed = അസത്യം;
    വേണ്ടി (int32_t j=size()-1; j>=0; --j) {
        int32_t i = other.indexOf(elements[j]);
        എങ്കിൽ (i < 0) {
            removeElementAt(j);
            changed = സത്യം;
        }
    }
    തിരികെ changed;
}

ശൂന്യം UVector::removeElementAt(int32_t index) {
    ശൂന്യം* e = orphanElementAt(index);
    എങ്കിൽ (e != nullptr && deleter != nullptr) {
        (*deleter)(e);
    }
}

UBool UVector::removeElement(ശൂന്യം* obj) {
    int32_t i = indexOf(obj);
    എങ്കിൽ (i >= 0) {
        removeElementAt(i);
        തിരികെ സത്യം;
    }
    തിരികെ അസത്യം;
}

ശൂന്യം UVector::removeAllElements() {
    എങ്കിൽ (deleter != nullptr) {
        വേണ്ടി (int32_t i=0; i<count; ++i) {
            എങ്കിൽ (elements[i].pointer != nullptr) {
                (*deleter)(elements[i].pointer);
            }
        }
    }
    count = 0;
}

UBool   UVector::equals(സ്ഥിരം UVector &other) സ്ഥിരം {
    പൂർണ്ണസംഖ്യ      i;

    എങ്കിൽ (ഇത്->count != other.count) {
        തിരികെ അസത്യം;
    }
    എങ്കിൽ (comparer == nullptr) {
        വേണ്ടി (i=0; i<count; i++) {
            എങ്കിൽ (elements[i].pointer != other.elements[i].pointer) {
                തിരികെ അസത്യം;
            }
        }
    } അല്ലെങ്കിൽ {
        UElement താക്കോൽ;
        വേണ്ടി (i=0; i<count; i++) {
            താക്കോൽ.pointer = &other.elements[i];
            എങ്കിൽ (!(*comparer)(താക്കോൽ, elements[i])) {
                തിരികെ അസത്യം;
            }
        }
    }
    തിരികെ സത്യം;
}



int32_t UVector::indexOf(ശൂന്യം* obj, int32_t startIndex) സ്ഥിരം {
    UElement താക്കോൽ;
    താക്കോൽ.pointer = obj;
    തിരികെ indexOf(താക്കോൽ, startIndex, HINT_താക്കോൽ_POINTER);
}

int32_t UVector::indexOf(int32_t obj, int32_t startIndex) സ്ഥിരം {
    UElement താക്കോൽ;
    താക്കോൽ.integer = obj;
    തിരികെ indexOf(താക്കോൽ, startIndex, HINT_താക്കോൽ_INTEGER);
}

int32_t UVector::indexOf(UElement താക്കോൽ, int32_t startIndex, int8_t hint) സ്ഥിരം {
    എങ്കിൽ (comparer != nullptr) {
        വേണ്ടി (int32_t i=startIndex; i<count; ++i) {
            എങ്കിൽ ((*comparer)(താക്കോൽ, elements[i])) {
                തിരികെ i;
            }
        }
    } അല്ലെങ്കിൽ {
        വേണ്ടി (int32_t i=startIndex; i<count; ++i) {
            /* Pointers are not always the same size as ints so to perform
             * a valid comparison we need to know whether we are being
             * provided an int or a pointer. */
            എങ്കിൽ (hint & HINT_താക്കോൽ_POINTER) {
                എങ്കിൽ (താക്കോൽ.pointer == elements[i].pointer) {
                    തിരികെ i;
                }
            } അല്ലെങ്കിൽ {
                എങ്കിൽ (താക്കോൽ.integer == elements[i].integer) {
                    തിരികെ i;
                }
            }
        }
    }
    തിരികെ -1;
}

UBool UVector::ensureCapacity(int32_t minimumCapacity, UErrorCode &status) {
    എങ്കിൽ (U_FAILURE(status)) {
        തിരികെ അസത്യം;
    }
    എങ്കിൽ (minimumCapacity < 0) {
        status = U_ILLEGAL_ARGUMENT_ERROR;
        തിരികെ അസത്യം;
    }
    എങ്കിൽ (capacity < minimumCapacity) {
        എങ്കിൽ (capacity > (INT32_MAX - 1) / 2) {        	// integer overflow check
            status = U_ILLEGAL_ARGUMENT_ERROR;
            തിരികെ അസത്യം;
        }
        int32_t newCap = capacity * 2;
        എങ്കിൽ (newCap < minimumCapacity) {
            newCap = minimumCapacity;
        }
        എങ്കിൽ (newCap > static_cast<int32_t>(INT32_MAX / വലുപ്പം(UElement))) { // integer overflow check
            // We keep the original memory contents on bad minimumCapacity.
            status = U_ILLEGAL_ARGUMENT_ERROR;
            തിരികെ അസത്യം;
        }
        UElement* newElems = static_cast<UElement*>(uprv_realloc(elements, വലുപ്പം(UElement) * newCap));
        എങ്കിൽ (newElems == nullptr) {
            // We keep the original contents on the memory failure on realloc or bad minimumCapacity.
            status = U_MEMORY_ALLOCATION_ERROR;
            തിരികെ അസത്യം;
        }
        elements = newElems;
        capacity = newCap;
    }
    തിരികെ സത്യം;
}

/**
 * Change the size of this vector as follows: If newSize is smaller,
 * then truncate the array, possibly deleting held elements for i >=
 * newSize.  If newSize is larger, grow the array, filling in new
 * slots with nullptr.
 */
ശൂന്യം UVector::setSize(int32_t newSize, UErrorCode &status) {
    എങ്കിൽ (!ensureCapacity(newSize, status)) {
        തിരികെ;
    }
    എങ്കിൽ (newSize > count) {
        UElement empty;
        empty.pointer = nullptr;
        empty.integer = 0;
        വേണ്ടി (int32_t i=count; i<newSize; ++i) {
            elements[i] = empty;
        }
    } അല്ലെങ്കിൽ {
        /* Most efficient to count down */
        വേണ്ടി (int32_t i=count-1; i>=newSize; --i) {
            removeElementAt(i);
        }
    }
    count = newSize;
}

/**
 * Fill in the given array with all elements of this vector.
 */
ശൂന്യം** UVector::toArray(ശൂന്യം** result) സ്ഥിരം {
    ശൂന്യം** a = result;
    വേണ്ടി (പൂർണ്ണസംഖ്യ i=0; i<count; ++i) {
        *a++ = elements[i].pointer;
    }
    തിരികെ result;
}

UObjectDeleter *UVector::setDeleter(UObjectDeleter *d) {
    UObjectDeleter *old = deleter;
    deleter = d;
    തിരികെ old;
}

UElementsAreEqual *UVector::setComparer(UElementsAreEqual *d) {
    UElementsAreEqual *old = comparer;
    comparer = d;
    തിരികെ old;
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
ശൂന്യം* UVector::orphanElementAt(int32_t index) {
    ശൂന്യം* e = nullptr;
    എങ്കിൽ (0 <= index && index < count) {
        e = elements[index].pointer;
        വേണ്ടി (int32_t i=index; i<count-1; ++i) {
            elements[i] = elements[i+1];
        }
        --count;
    }
    /* else index out of range */
    തിരികെ e;
}

/**
 * Insert the given object into this vector at its sorted position
 * as defined by 'compare'.  The current elements are assumed to
 * be sorted already.
 */
ശൂന്യം UVector::sorted_തിരുകുക(ശൂന്യം* obj, UElementComparator *compare, UErrorCode& ec) {
    UElement e;
    e.pointer = obj;
    sorted_തിരുകുക(e, compare, ec);
}

/**
 * Insert the given integer into this vector at its sorted position
 * as defined by 'compare'.  The current elements are assumed to
 * be sorted already.
 */
ശൂന്യം UVector::sorted_തിരുകുക(int32_t obj, UElementComparator *compare, UErrorCode& ec) {
    U_ASSERT(deleter == nullptr);
    UElement e {};
    e.integer = obj;
    sorted_തിരുകുക(e, compare, ec);
}

// ASSUME elements[] IS CURRENTLY SORTED
ശൂന്യം UVector::sorted_തിരുകുക(UElement e, UElementComparator *compare, UErrorCode& ec) {
    // Perform a binary search for the location to insert tok at.  Tok
    // will be inserted between two elements a and b such that a <=
    // tok && tok < b, where there is a 'virtual' elements[-1] always
    // less than tok and a 'virtual' elements[count] always greater
    // than tok.
    എങ്കിൽ (!ensureCapacity(count + 1, ec)) {
        എങ്കിൽ (deleter != nullptr) {
            (*deleter)(e.pointer);
        }
        തിരികെ;
    }
    int32_t min = 0, max = count;
    ആകുമ്പോൾ (min != max) {
        int32_t probe = (min + max) / 2;
        int32_t c = (*compare)(elements[probe], e);
        എങ്കിൽ (c > 0) {
            max = probe;
        } അല്ലെങ്കിൽ {
            // assert(c <= 0);
            min = probe + 1;
        }
    }
    വേണ്ടി (int32_t i=count; i>min; --i) {
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
സ്ഥിതം int32_t U_CALLCONV
sortComparator(സ്ഥിരം ശൂന്യം *context, സ്ഥിരം ശൂന്യം *left, സ്ഥിരം ശൂന്യം *വലത്) {
    UElementComparator *compare = *static_cast<UElementComparator * സ്ഥിരം *>(context);
    UElement e1 = *static_cast<സ്ഥിരം UElement *>(left);
    UElement e2 = *static_cast<സ്ഥിരം UElement *>(വലത്);
    int32_t result = (*compare)(e1, e2);
    തിരികെ result;
}


/**
  *  Array sort comparison function for use from UVector::sorti()
  *  Compares int32_t vector elements.
  */
സ്ഥിതം int32_t U_CALLCONV
sortiComparator(സ്ഥിരം ശൂന്യം * /*context */, സ്ഥിരം ശൂന്യം *left, സ്ഥിരം ശൂന്യം *വലത്) {
    സ്ഥിരം UElement *e1 = static_cast<സ്ഥിരം UElement *>(left);
    സ്ഥിരം UElement *e2 = static_cast<സ്ഥിരം UElement *>(വലത്);
    int32_t result = e1->integer < e2->integer? -1 :
                     e1->integer == e2->integer? 0 : 1;
    തിരികെ result;
}

/**
  * Sort the vector, assuming it contains ints.
  *     (A more general sort would take a comparison function, but it's
  *     not clear whether UVector's UElementComparator or
  *     UComparator from uprv_sortAray would be more appropriate.)
  */
ശൂന്യം UVector::sorti(UErrorCode &ec) {
    എങ്കിൽ (U_SUCCESS(ec)) {
        uprv_sortArray(elements, count, വലുപ്പം(UElement),
                       sortiComparator, nullptr,  അസത്യം, &ec);
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
ശൂന്യം UVector::sort(UElementComparator *compare, UErrorCode &ec) {
    എങ്കിൽ (U_SUCCESS(ec)) {
        uprv_sortArray(elements, count, വലുപ്പം(UElement),
                       sortComparator, &compare, അസത്യം, &ec);
    }
}


/**
 *  Stable sort with a user supplied comparator of type UComparator.
 */
ശൂന്യം UVector::sortWithUComparator(UComparator *compare, സ്ഥിരം ശൂന്യം *context, UErrorCode &ec) {
    എങ്കിൽ (U_SUCCESS(ec)) {
        uprv_sortArray(elements, count, വലുപ്പം(UElement),
                       compare, context, സത്യം, &ec);
    }
}

U_NAMESPACE_END

