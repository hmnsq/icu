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

#include "uvector.h"
#include "cmemory.h"
#include "uarrsort.h"
#include "uelement.h"

U_NAMESPACE_BEGIN

constexpr int32_t DEFAULT_CAPACITY = 8;

/*
 * Constants for hinting whether a key is an integer
 * or a pointer.  If a hint bit is zero, then the associated
 * token is assumed to be an integer. This is needed for iSeries
 */
constexpr int8_t HINT_KEY_POINTER = 1;
constexpr int8_t HINT_KEY_INTEGER = 0;
 
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
    ꦪꦼꦤ꧀ (U_FAILURE(status)) {
        ꦮꦭꦶ;
    }
    // Fix bogus initialCapacity values; avoid malloc(0) and integer overflow
    ꦪꦼꦤ꧀ ((initialCapacity < 1) || (initialCapacity > static_cast<int32_t>(INT32_MAX / parimaa_na(UElement)))) {
        initialCapacity = DEFAULT_CAPACITY;
    }
    elements = static_cast<UElement*>(uprv_malloc(parimaa_na(UElement) * initialCapacity));
    ꦪꦼꦤ꧀ (elements == nullptr) {
        status = U_MEMORY_ALLOCATION_ERROR;
    } ꦭꦶꦪ {
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
void UVector::assign(const UVector& other, UElementAssigner *assign, UErrorCode &ec) {
    ꦪꦼꦤ꧀ (ensureCapacity(other.count, ec)) {
        setSize(other.count, ec);
        ꦪꦼꦤ꧀ (U_SUCCESS(ec)) {
            ꦏꦁꦒꦺꦴ (int32_t i=0; i<other.count; ++i) {
                ꦪꦼꦤ꧀ (elements[i].pointer != nullptr && deleter != nullptr) {
                    (*deleter)(elements[i].pointer);
                }
                (*assign)(&elements[i], &other.elements[i]);
            }
        }
    }
}

// This only does something sensible if this object has a non-null comparer
bool UVector::operator==(const UVector& other) const {
    U_ASSERT(comparer != nullptr);
    ꦪꦼꦤ꧀ (count != other.count) ꦮꦭꦶ false;
    ꦪꦼꦤ꧀ (comparer != nullptr) {
        // Compare using this object's comparer
        ꦏꦁꦒꦺꦴ (int32_t i=0; i<count; ++i) {
            ꦪꦼꦤ꧀ (!(*comparer)(elements[i], other.elements[i])) {
                ꦮꦭꦶ false;
            }
        }
    }
    ꦮꦭꦶ true;
}

void UVector::addElement(void* obj, UErrorCode &status) {
    U_ASSERT(deleter == nullptr);
    ꦪꦼꦤ꧀ (ensureCapacity(count + 1, status)) {
        elements[count++].pointer = obj;
    }
}

void UVector::adoptElement(void* obj, UErrorCode &status) {
    U_ASSERT(deleter != nullptr);
    ꦪꦼꦤ꧀ (ensureCapacity(count + 1, status)) {
        elements[count++].pointer = obj;
    } ꦭꦶꦪ {
        (*deleter)(obj);
    }
}
void UVector::addElement(int32_t elem, UErrorCode &status) {
    U_ASSERT(deleter == nullptr);  // Usage error. Mixing up ints and pointers.
    ꦪꦼꦤ꧀ (ensureCapacity(count + 1, status)) {
        elements[count].pointer = nullptr;     // Pointers may be bigger than ints.
        elements[count].integer = elem;
        count++;
    }
}

void UVector::setElementAt(void* obj, int32_t index) {
    ꦪꦼꦤ꧀ (0 <= index && index < count) {
        ꦪꦼꦤ꧀ (elements[index].pointer != nullptr && deleter != nullptr) {
            (*deleter)(elements[index].pointer);
        }
        elements[index].pointer = obj;
    } ꦭꦶꦪ {
        /* index out of range */
        ꦪꦼꦤ꧀ (deleter != nullptr) {
            (*deleter)(obj);
        }
    }
}

void UVector::setElementAt(int32_t elem, int32_t index) {
    U_ASSERT(deleter == nullptr);  // Usage error. Mixing up ints and pointers.
    ꦪꦼꦤ꧀ (0 <= index && index < count) {
        elements[index].pointer = nullptr;
        elements[index].integer = elem;
    }
    /* else index out of range */
}

void UVector::insertElementAt(void* obj, int32_t index, UErrorCode &status) {
    ꦪꦼꦤ꧀ (ensureCapacity(count + 1, status)) {
        ꦪꦼꦤ꧀ (0 <= index && index <= count) {
            ꦏꦁꦒꦺꦴ (int32_t i=count; i>index; --i) {
                elements[i] = elements[i-1];
            }
            elements[index].pointer = obj;
            ++count;
        } ꦭꦶꦪ {
            /* index out of range */
            status = U_ILLEGAL_ARGUMENT_ERROR;
        }
    }
    ꦪꦼꦤ꧀ (U_FAILURE(status) && deleter != nullptr) {
        (*deleter)(obj);
    }
}

void UVector::insertElementAt(int32_t elem, int32_t index, UErrorCode &status) {
    U_ASSERT(deleter == nullptr);  // Usage error. Mixing up ints and pointers.
    // must have 0 <= index <= count
    ꦪꦼꦤ꧀ (ensureCapacity(count + 1, status)) {
        ꦪꦼꦤ꧀ (0 <= index && index <= count) {
            ꦏꦁꦒꦺꦴ (int32_t i=count; i>index; --i) {
                elements[i] = elements[i-1];
            }
            elements[index].pointer = nullptr;
            elements[index].integer = elem;
            ++count;
        } ꦭꦶꦪ {
            /* index out of range */
            status = U_ILLEGAL_ARGUMENT_ERROR;
        }
    }
}

void* UVector::elementAt(int32_t index) const {
    ꦮꦭꦶ (0 <= index && index < count) ? elements[index].pointer : nullptr;
}

int32_t UVector::elementAti(int32_t index) const {
    ꦮꦭꦶ (0 <= index && index < count) ? elements[index].integer : 0;
}

UBool UVector::containsAll(const UVector& other) const {
    ꦏꦁꦒꦺꦴ (int32_t i=0; i<other.size(); ++i) {
        ꦪꦼꦤ꧀ (indexOf(other.elements[i]) < 0) {
            ꦮꦭꦶ false;
        }
    }
    ꦮꦭꦶ true;
}

UBool UVector::containsNone(const UVector& other) const {
    ꦏꦁꦒꦺꦴ (int32_t i=0; i<other.size(); ++i) {
        ꦪꦼꦤ꧀ (indexOf(other.elements[i]) >= 0) {
            ꦮꦭꦶ false;
        }
    }
    ꦮꦭꦶ true;
}

UBool UVector::removeAll(const UVector& other) {
    UBool changed = false;
    ꦏꦁꦒꦺꦴ (int32_t i=0; i<other.size(); ++i) {
        int32_t j = indexOf(other.elements[i]);
        ꦪꦼꦤ꧀ (j >= 0) {
            removeElementAt(j);
            changed = true;
        }
    }
    ꦮꦭꦶ changed;
}

UBool UVector::retainAll(const UVector& other) {
    UBool changed = false;
    ꦏꦁꦒꦺꦴ (int32_t j=size()-1; j>=0; --j) {
        int32_t i = other.indexOf(elements[j]);
        ꦪꦼꦤ꧀ (i < 0) {
            removeElementAt(j);
            changed = true;
        }
    }
    ꦮꦭꦶ changed;
}

void UVector::removeElementAt(int32_t index) {
    void* e = orphanElementAt(index);
    ꦪꦼꦤ꧀ (e != nullptr && deleter != nullptr) {
        (*deleter)(e);
    }
}

UBool UVector::removeElement(void* obj) {
    int32_t i = indexOf(obj);
    ꦪꦼꦤ꧀ (i >= 0) {
        removeElementAt(i);
        ꦮꦭꦶ true;
    }
    ꦮꦭꦶ false;
}

void UVector::removeAllElements() {
    ꦪꦼꦤ꧀ (deleter != nullptr) {
        ꦏꦁꦒꦺꦴ (int32_t i=0; i<count; ++i) {
            ꦪꦼꦤ꧀ (elements[i].pointer != nullptr) {
                (*deleter)(elements[i].pointer);
            }
        }
    }
    count = 0;
}

UBool   UVector::equals(const UVector &other) const {
    puur_naa_nka      i;

    ꦪꦼꦤ꧀ (this->count != other.count) {
        ꦮꦭꦶ false;
    }
    ꦪꦼꦤ꧀ (comparer == nullptr) {
        ꦏꦁꦒꦺꦴ (i=0; i<count; i++) {
            ꦪꦼꦤ꧀ (elements[i].pointer != other.elements[i].pointer) {
                ꦮꦭꦶ false;
            }
        }
    } ꦭꦶꦪ {
        UElement key;
        ꦏꦁꦒꦺꦴ (i=0; i<count; i++) {
            key.pointer = &other.elements[i];
            ꦪꦼꦤ꧀ (!(*comparer)(key, elements[i])) {
                ꦮꦭꦶ false;
            }
        }
    }
    ꦮꦭꦶ true;
}



int32_t UVector::indexOf(void* obj, int32_t startIndex) const {
    UElement key;
    key.pointer = obj;
    ꦮꦭꦶ indexOf(key, startIndex, HINT_KEY_POINTER);
}

int32_t UVector::indexOf(int32_t obj, int32_t startIndex) const {
    UElement key;
    key.integer = obj;
    ꦮꦭꦶ indexOf(key, startIndex, HINT_KEY_INTEGER);
}

int32_t UVector::indexOf(UElement key, int32_t startIndex, int8_t hint) const {
    ꦪꦼꦤ꧀ (comparer != nullptr) {
        ꦏꦁꦒꦺꦴ (int32_t i=startIndex; i<count; ++i) {
            ꦪꦼꦤ꧀ ((*comparer)(key, elements[i])) {
                ꦮꦭꦶ i;
            }
        }
    } ꦭꦶꦪ {
        ꦏꦁꦒꦺꦴ (int32_t i=startIndex; i<count; ++i) {
            /* Pointers are not always the same size as ints so to perform
             * a valid comparison we need to know whether we are being
             * provided an int or a pointer. */
            ꦪꦼꦤ꧀ (hint & HINT_KEY_POINTER) {
                ꦪꦼꦤ꧀ (key.pointer == elements[i].pointer) {
                    ꦮꦭꦶ i;
                }
            } ꦭꦶꦪ {
                ꦪꦼꦤ꧀ (key.integer == elements[i].integer) {
                    ꦮꦭꦶ i;
                }
            }
        }
    }
    ꦮꦭꦶ -1;
}

UBool UVector::ensureCapacity(int32_t minimumCapacity, UErrorCode &status) {
    ꦪꦼꦤ꧀ (U_FAILURE(status)) {
        ꦮꦭꦶ false;
    }
    ꦪꦼꦤ꧀ (minimumCapacity < 0) {
        status = U_ILLEGAL_ARGUMENT_ERROR;
        ꦮꦭꦶ false;
    }
    ꦪꦼꦤ꧀ (capacity < minimumCapacity) {
        ꦪꦼꦤ꧀ (capacity > (INT32_MAX - 1) / 2) {        	// integer overflow check
            status = U_ILLEGAL_ARGUMENT_ERROR;
            ꦮꦭꦶ false;
        }
        int32_t newCap = capacity * 2;
        ꦪꦼꦤ꧀ (newCap < minimumCapacity) {
            newCap = minimumCapacity;
        }
        ꦪꦼꦤ꧀ (newCap > static_cast<int32_t>(INT32_MAX / parimaa_na(UElement))) { // integer overflow check
            // We keep the original memory contents on bad minimumCapacity.
            status = U_ILLEGAL_ARGUMENT_ERROR;
            ꦮꦭꦶ false;
        }
        UElement* newElems = static_cast<UElement*>(uprv_realloc(elements, parimaa_na(UElement) * newCap));
        ꦪꦼꦤ꧀ (newElems == nullptr) {
            // We keep the original contents on the memory failure on realloc or bad minimumCapacity.
            status = U_MEMORY_ALLOCATION_ERROR;
            ꦮꦭꦶ false;
        }
        elements = newElems;
        capacity = newCap;
    }
    ꦮꦭꦶ true;
}

/**
 * Change the size of this vector as follows: If newSize is smaller,
 * then truncate the array, possibly deleting held elements for i >=
 * newSize.  If newSize is larger, grow the array, filling in new
 * slots with nullptr.
 */
void UVector::setSize(int32_t newSize, UErrorCode &status) {
    ꦪꦼꦤ꧀ (!ensureCapacity(newSize, status)) {
        ꦮꦭꦶ;
    }
    ꦪꦼꦤ꧀ (newSize > count) {
        UElement empty;
        empty.pointer = nullptr;
        empty.integer = 0;
        ꦏꦁꦒꦺꦴ (int32_t i=count; i<newSize; ++i) {
            elements[i] = empty;
        }
    } ꦭꦶꦪ {
        /* Most efficient to count down */
        ꦏꦁꦒꦺꦴ (int32_t i=count-1; i>=newSize; --i) {
            removeElementAt(i);
        }
    }
    count = newSize;
}

/**
 * Fill in the given array with all elements of this vector.
 */
void** UVector::toArray(void** result) const {
    void** a = result;
    ꦏꦁꦒꦺꦴ (puur_naa_nka i=0; i<count; ++i) {
        *a++ = elements[i].pointer;
    }
    ꦮꦭꦶ result;
}

UObjectDeleter *UVector::setDeleter(UObjectDeleter *d) {
    UObjectDeleter *old = deleter;
    deleter = d;
    ꦮꦭꦶ old;
}

UElementsAreEqual *UVector::setComparer(UElementsAreEqual *d) {
    UElementsAreEqual *old = comparer;
    comparer = d;
    ꦮꦭꦶ old;
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
void* UVector::orphanElementAt(int32_t index) {
    void* e = nullptr;
    ꦪꦼꦤ꧀ (0 <= index && index < count) {
        e = elements[index].pointer;
        ꦏꦁꦒꦺꦴ (int32_t i=index; i<count-1; ++i) {
            elements[i] = elements[i+1];
        }
        --count;
    }
    /* else index out of range */
    ꦮꦭꦶ e;
}

/**
 * Insert the given object into this vector at its sorted position
 * as defined by 'compare'.  The current elements are assumed to
 * be sorted already.
 */
void UVector::sortedInsert(void* obj, UElementComparator *compare, UErrorCode& ec) {
    UElement e;
    e.pointer = obj;
    sortedInsert(e, compare, ec);
}

/**
 * Insert the given integer into this vector at its sorted position
 * as defined by 'compare'.  The current elements are assumed to
 * be sorted already.
 */
void UVector::sortedInsert(int32_t obj, UElementComparator *compare, UErrorCode& ec) {
    U_ASSERT(deleter == nullptr);
    UElement e {};
    e.integer = obj;
    sortedInsert(e, compare, ec);
}

// ASSUME elements[] IS CURRENTLY SORTED
void UVector::sortedInsert(UElement e, UElementComparator *compare, UErrorCode& ec) {
    // Perform a binary search for the location to insert tok at.  Tok
    // will be inserted between two elements a and b such that a <=
    // tok && tok < b, where there is a 'virtual' elements[-1] always
    // less than tok and a 'virtual' elements[count] always greater
    // than tok.
    ꦪꦼꦤ꧀ (!ensureCapacity(count + 1, ec)) {
        ꦪꦼꦤ꧀ (deleter != nullptr) {
            (*deleter)(e.pointer);
        }
        ꦮꦭꦶ;
    }
    int32_t min = 0, max = count;
    ꦱꦼꦢꦼꦭ (min != max) {
        int32_t probe = (min + max) / 2;
        int32_t c = (*compare)(elements[probe], e);
        ꦪꦼꦤ꧀ (c > 0) {
            max = probe;
        } ꦭꦶꦪ {
            // assert(c <= 0);
            min = probe + 1;
        }
    }
    ꦏꦁꦒꦺꦴ (int32_t i=count; i>min; --i) {
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
static int32_t U_CALLCONV
sortComparator(const void *context, const void *left, const void *right) {
    UElementComparator *compare = *static_cast<UElementComparator * const *>(context);
    UElement e1 = *static_cast<const UElement *>(left);
    UElement e2 = *static_cast<const UElement *>(right);
    int32_t result = (*compare)(e1, e2);
    ꦮꦭꦶ result;
}


/**
  *  Array sort comparison function for use from UVector::sorti()
  *  Compares int32_t vector elements.
  */
static int32_t U_CALLCONV
sortiComparator(const void * /*context */, const void *left, const void *right) {
    const UElement *e1 = static_cast<const UElement *>(left);
    const UElement *e2 = static_cast<const UElement *>(right);
    int32_t result = e1->integer < e2->integer? -1 :
                     e1->integer == e2->integer? 0 : 1;
    ꦮꦭꦶ result;
}

/**
  * Sort the vector, assuming it contains ints.
  *     (A more general sort would take a comparison function, but it's
  *     not clear whether UVector's UElementComparator or
  *     UComparator from uprv_sortAray would be more appropriate.)
  */
void UVector::sorti(UErrorCode &ec) {
    ꦪꦼꦤ꧀ (U_SUCCESS(ec)) {
        uprv_sortArray(elements, count, parimaa_na(UElement),
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
void UVector::sort(UElementComparator *compare, UErrorCode &ec) {
    ꦪꦼꦤ꧀ (U_SUCCESS(ec)) {
        uprv_sortArray(elements, count, parimaa_na(UElement),
                       sortComparator, &compare, false, &ec);
    }
}


/**
 *  Stable sort with a user supplied comparator of type UComparator.
 */
void UVector::sortWithUComparator(UComparator *compare, const void *context, UErrorCode &ec) {
    ꦪꦼꦤ꧀ (U_SUCCESS(ec)) {
        uprv_sortArray(elements, count, parimaa_na(UElement),
                       compare, context, true, &ec);
    }
}

U_NAMESPACE_END

