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

#ضمن "uvector.h"
#ضمن "cmemory.h"
#ضمن "uarrsort.h"
#ضمن "uelement.h"

U_NAMESPACE_BEGIN

constexpr int32_t DEFAULT_CAPACITY = 8;

/*
 * Constants for hinting whether a key is an integer
 * or a pointer.  If a hint bit is zero, then the associated
 * token is assumed to be an integer. This is needed for iSeries
 */
constexpr int8_t HINT_مفتاح_POINTER = 1;
constexpr int8_t HINT_مفتاح_INTEGER = 0;
 
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
    إذا (U_FAILURE(status)) {
        أرجع;
    }
    // Fix bogus initialCapacity values; avoid malloc(0) and integer overflow
    إذا ((initialCapacity < 1) || (initialCapacity > static_cast<int32_t>(INT32_MAX / حجم(UElement)))) {
        initialCapacity = DEFAULT_CAPACITY;
    }
    elements = static_cast<UElement*>(uprv_malloc(حجم(UElement) * initialCapacity));
    إذا (elements == nullptr) {
        status = U_MEMORY_ALLOCATION_ERROR;
    } وإلا {
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
فراغ UVector::assign(ثابت UVector& other, UElementAssigner *assign, UErrorCode &ec) {
    إذا (ensureCapacity(other.count, ec)) {
        setSize(other.count, ec);
        إذا (U_SUCCESS(ec)) {
            لكل (int32_t i=0; i<other.count; ++i) {
                إذا (elements[i].pointer != nullptr && deleter != nullptr) {
                    (*deleter)(elements[i].pointer);
                }
                (*assign)(&elements[i], &other.elements[i]);
            }
        }
    }
}

// This only does something sensible if this object has a non-null comparer
منطقي UVector::operator==(ثابت UVector& other) ثابت {
    U_ASSERT(comparer != nullptr);
    إذا (count != other.count) أرجع خطأ;
    إذا (comparer != nullptr) {
        // Compare using this object's comparer
        لكل (int32_t i=0; i<count; ++i) {
            إذا (!(*comparer)(elements[i], other.elements[i])) {
                أرجع خطأ;
            }
        }
    }
    أرجع true;
}

فراغ UVector::addElement(فراغ* obj, UErrorCode &status) {
    U_ASSERT(deleter == nullptr);
    إذا (ensureCapacity(count + 1, status)) {
        elements[count++].pointer = obj;
    }
}

فراغ UVector::adoptElement(فراغ* obj, UErrorCode &status) {
    U_ASSERT(deleter != nullptr);
    إذا (ensureCapacity(count + 1, status)) {
        elements[count++].pointer = obj;
    } وإلا {
        (*deleter)(obj);
    }
}
فراغ UVector::addElement(int32_t elem, UErrorCode &status) {
    U_ASSERT(deleter == nullptr);  // Usage error. Mixing up ints and pointers.
    إذا (ensureCapacity(count + 1, status)) {
        elements[count].pointer = nullptr;     // Pointers may be bigger than ints.
        elements[count].integer = elem;
        count++;
    }
}

فراغ UVector::setElementAt(فراغ* obj, int32_t index) {
    إذا (0 <= index && index < count) {
        إذا (elements[index].pointer != nullptr && deleter != nullptr) {
            (*deleter)(elements[index].pointer);
        }
        elements[index].pointer = obj;
    } وإلا {
        /* index out of range */
        إذا (deleter != nullptr) {
            (*deleter)(obj);
        }
    }
}

فراغ UVector::setElementAt(int32_t elem, int32_t index) {
    U_ASSERT(deleter == nullptr);  // Usage error. Mixing up ints and pointers.
    إذا (0 <= index && index < count) {
        elements[index].pointer = nullptr;
        elements[index].integer = elem;
    }
    /* else index out of range */
}

فراغ UVector::أدرج_Element_At(فراغ* obj, int32_t index, UErrorCode &status) {
    إذا (ensureCapacity(count + 1, status)) {
        إذا (0 <= index && index <= count) {
            لكل (int32_t i=count; i>index; --i) {
                elements[i] = elements[i-1];
            }
            elements[index].pointer = obj;
            ++count;
        } وإلا {
            /* index out of range */
            status = U_ILLEGAL_ARGUMENT_ERROR;
        }
    }
    إذا (U_FAILURE(status) && deleter != nullptr) {
        (*deleter)(obj);
    }
}

فراغ UVector::أدرج_Element_At(int32_t elem, int32_t index, UErrorCode &status) {
    U_ASSERT(deleter == nullptr);  // Usage error. Mixing up ints and pointers.
    // must have 0 <= index <= count
    إذا (ensureCapacity(count + 1, status)) {
        إذا (0 <= index && index <= count) {
            لكل (int32_t i=count; i>index; --i) {
                elements[i] = elements[i-1];
            }
            elements[index].pointer = nullptr;
            elements[index].integer = elem;
            ++count;
        } وإلا {
            /* index out of range */
            status = U_ILLEGAL_ARGUMENT_ERROR;
        }
    }
}

فراغ* UVector::elementAt(int32_t index) ثابت {
    أرجع (0 <= index && index < count) ? elements[index].pointer : nullptr;
}

int32_t UVector::elementAti(int32_t index) ثابت {
    أرجع (0 <= index && index < count) ? elements[index].integer : 0;
}

UBool UVector::containsAll(ثابت UVector& other) ثابت {
    لكل (int32_t i=0; i<other.size(); ++i) {
        إذا (indexOf(other.elements[i]) < 0) {
            أرجع خطأ;
        }
    }
    أرجع true;
}

UBool UVector::containsNone(ثابت UVector& other) ثابت {
    لكل (int32_t i=0; i<other.size(); ++i) {
        إذا (indexOf(other.elements[i]) >= 0) {
            أرجع خطأ;
        }
    }
    أرجع true;
}

UBool UVector::removeAll(ثابت UVector& other) {
    UBool changed = خطأ;
    لكل (int32_t i=0; i<other.size(); ++i) {
        int32_t j = indexOf(other.elements[i]);
        إذا (j >= 0) {
            removeElementAt(j);
            changed = true;
        }
    }
    أرجع changed;
}

UBool UVector::retainAll(ثابت UVector& other) {
    UBool changed = خطأ;
    لكل (int32_t j=size()-1; j>=0; --j) {
        int32_t i = other.indexOf(elements[j]);
        إذا (i < 0) {
            removeElementAt(j);
            changed = true;
        }
    }
    أرجع changed;
}

فراغ UVector::removeElementAt(int32_t index) {
    فراغ* e = orphanElementAt(index);
    إذا (e != nullptr && deleter != nullptr) {
        (*deleter)(e);
    }
}

UBool UVector::removeElement(فراغ* obj) {
    int32_t i = indexOf(obj);
    إذا (i >= 0) {
        removeElementAt(i);
        أرجع true;
    }
    أرجع خطأ;
}

فراغ UVector::removeAllElements() {
    إذا (deleter != nullptr) {
        لكل (int32_t i=0; i<count; ++i) {
            إذا (elements[i].pointer != nullptr) {
                (*deleter)(elements[i].pointer);
            }
        }
    }
    count = 0;
}

UBool   UVector::equals(ثابت UVector &other) ثابت {
    صحيح      i;

    إذا (هذا->count != other.count) {
        أرجع خطأ;
    }
    إذا (comparer == nullptr) {
        لكل (i=0; i<count; i++) {
            إذا (elements[i].pointer != other.elements[i].pointer) {
                أرجع خطأ;
            }
        }
    } وإلا {
        UElement مفتاح;
        لكل (i=0; i<count; i++) {
            مفتاح.pointer = &other.elements[i];
            إذا (!(*comparer)(مفتاح, elements[i])) {
                أرجع خطأ;
            }
        }
    }
    أرجع true;
}



int32_t UVector::indexOf(فراغ* obj, int32_t startIndex) ثابت {
    UElement مفتاح;
    مفتاح.pointer = obj;
    أرجع indexOf(مفتاح, startIndex, HINT_مفتاح_POINTER);
}

int32_t UVector::indexOf(int32_t obj, int32_t startIndex) ثابت {
    UElement مفتاح;
    مفتاح.integer = obj;
    أرجع indexOf(مفتاح, startIndex, HINT_مفتاح_INTEGER);
}

int32_t UVector::indexOf(UElement مفتاح, int32_t startIndex, int8_t hint) ثابت {
    إذا (comparer != nullptr) {
        لكل (int32_t i=startIndex; i<count; ++i) {
            إذا ((*comparer)(مفتاح, elements[i])) {
                أرجع i;
            }
        }
    } وإلا {
        لكل (int32_t i=startIndex; i<count; ++i) {
            /* Pointers are not always the same size as ints so to perform
             * a valid comparison we need to know whether we are being
             * provided an int or a pointer. */
            إذا (hint & HINT_مفتاح_POINTER) {
                إذا (مفتاح.pointer == elements[i].pointer) {
                    أرجع i;
                }
            } وإلا {
                إذا (مفتاح.integer == elements[i].integer) {
                    أرجع i;
                }
            }
        }
    }
    أرجع -1;
}

UBool UVector::ensureCapacity(int32_t minimumCapacity, UErrorCode &status) {
    إذا (U_FAILURE(status)) {
        أرجع خطأ;
    }
    إذا (minimumCapacity < 0) {
        status = U_ILLEGAL_ARGUMENT_ERROR;
        أرجع خطأ;
    }
    إذا (capacity < minimumCapacity) {
        إذا (capacity > (INT32_MAX - 1) / 2) {        	// integer overflow check
            status = U_ILLEGAL_ARGUMENT_ERROR;
            أرجع خطأ;
        }
        int32_t newCap = capacity * 2;
        إذا (newCap < minimumCapacity) {
            newCap = minimumCapacity;
        }
        إذا (newCap > static_cast<int32_t>(INT32_MAX / حجم(UElement))) { // integer overflow check
            // We keep the original memory contents on bad minimumCapacity.
            status = U_ILLEGAL_ARGUMENT_ERROR;
            أرجع خطأ;
        }
        UElement* newElems = static_cast<UElement*>(uprv_realloc(elements, حجم(UElement) * newCap));
        إذا (newElems == nullptr) {
            // We keep the original contents on the memory failure on realloc or bad minimumCapacity.
            status = U_MEMORY_ALLOCATION_ERROR;
            أرجع خطأ;
        }
        elements = newElems;
        capacity = newCap;
    }
    أرجع true;
}

/**
 * Change the size of this vector as follows: If newSize is smaller,
 * then truncate the array, possibly deleting held elements for i >=
 * newSize.  If newSize is larger, grow the array, filling in new
 * slots with nullptr.
 */
فراغ UVector::setSize(int32_t newSize, UErrorCode &status) {
    إذا (!ensureCapacity(newSize, status)) {
        أرجع;
    }
    إذا (newSize > count) {
        UElement empty;
        empty.pointer = nullptr;
        empty.integer = 0;
        لكل (int32_t i=count; i<newSize; ++i) {
            elements[i] = empty;
        }
    } وإلا {
        /* Most efficient to count down */
        لكل (int32_t i=count-1; i>=newSize; --i) {
            removeElementAt(i);
        }
    }
    count = newSize;
}

/**
 * Fill in the given array with all elements of this vector.
 */
فراغ** UVector::toArray(فراغ** result) ثابت {
    فراغ** a = result;
    لكل (صحيح i=0; i<count; ++i) {
        *a++ = elements[i].pointer;
    }
    أرجع result;
}

UObjectDeleter *UVector::setDeleter(UObjectDeleter *d) {
    UObjectDeleter *old = deleter;
    deleter = d;
    أرجع old;
}

UElementsAreEqual *UVector::setComparer(UElementsAreEqual *d) {
    UElementsAreEqual *old = comparer;
    comparer = d;
    أرجع old;
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
فراغ* UVector::orphanElementAt(int32_t index) {
    فراغ* e = nullptr;
    إذا (0 <= index && index < count) {
        e = elements[index].pointer;
        لكل (int32_t i=index; i<count-1; ++i) {
            elements[i] = elements[i+1];
        }
        --count;
    }
    /* else index out of range */
    أرجع e;
}

/**
 * Insert the given object into this vector at its sorted position
 * as defined by 'compare'.  The current elements are assumed to
 * be sorted already.
 */
فراغ UVector::sorted_أدرج(فراغ* obj, UElementComparator *compare, UErrorCode& ec) {
    UElement e;
    e.pointer = obj;
    sorted_أدرج(e, compare, ec);
}

/**
 * Insert the given integer into this vector at its sorted position
 * as defined by 'compare'.  The current elements are assumed to
 * be sorted already.
 */
فراغ UVector::sorted_أدرج(int32_t obj, UElementComparator *compare, UErrorCode& ec) {
    U_ASSERT(deleter == nullptr);
    UElement e {};
    e.integer = obj;
    sorted_أدرج(e, compare, ec);
}

// ASSUME elements[] IS CURRENTLY SORTED
فراغ UVector::sorted_أدرج(UElement e, UElementComparator *compare, UErrorCode& ec) {
    // Perform a binary search for the location to insert tok at.  Tok
    // will be inserted between two elements a and b such that a <=
    // tok && tok < b, where there is a 'virtual' elements[-1] always
    // less than tok and a 'virtual' elements[count] always greater
    // than tok.
    إذا (!ensureCapacity(count + 1, ec)) {
        إذا (deleter != nullptr) {
            (*deleter)(e.pointer);
        }
        أرجع;
    }
    int32_t min = 0, max = count;
    طالما (min != max) {
        int32_t probe = (min + max) / 2;
        int32_t c = (*compare)(elements[probe], e);
        إذا (c > 0) {
            max = probe;
        } وإلا {
            // assert(c <= 0);
            min = probe + 1;
        }
    }
    لكل (int32_t i=count; i>min; --i) {
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
ساكن int32_t U_CALLCONV
sortComparator(ثابت فراغ *context, ثابت فراغ *left, ثابت فراغ *يمين) {
    UElementComparator *compare = *static_cast<UElementComparator * ثابت *>(context);
    UElement e1 = *static_cast<ثابت UElement *>(left);
    UElement e2 = *static_cast<ثابت UElement *>(يمين);
    int32_t result = (*compare)(e1, e2);
    أرجع result;
}


/**
  *  Array sort comparison function for use from UVector::sorti()
  *  Compares int32_t vector elements.
  */
ساكن int32_t U_CALLCONV
sortiComparator(ثابت فراغ * /*context */, ثابت فراغ *left, ثابت فراغ *يمين) {
    ثابت UElement *e1 = static_cast<ثابت UElement *>(left);
    ثابت UElement *e2 = static_cast<ثابت UElement *>(يمين);
    int32_t result = e1->integer < e2->integer? -1 :
                     e1->integer == e2->integer? 0 : 1;
    أرجع result;
}

/**
  * Sort the vector, assuming it contains ints.
  *     (A more general sort would take a comparison function, but it's
  *     not clear whether UVector's UElementComparator or
  *     UComparator from uprv_sortAray would be more appropriate.)
  */
فراغ UVector::sorti(UErrorCode &ec) {
    إذا (U_SUCCESS(ec)) {
        uprv_sortArray(elements, count, حجم(UElement),
                       sortiComparator, nullptr,  خطأ, &ec);
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
فراغ UVector::sort(UElementComparator *compare, UErrorCode &ec) {
    إذا (U_SUCCESS(ec)) {
        uprv_sortArray(elements, count, حجم(UElement),
                       sortComparator, &compare, خطأ, &ec);
    }
}


/**
 *  Stable sort with a user supplied comparator of type UComparator.
 */
فراغ UVector::sortWithUComparator(UComparator *compare, ثابت فراغ *context, UErrorCode &ec) {
    إذا (U_SUCCESS(ec)) {
        uprv_sortArray(elements, count, حجم(UElement),
                       compare, context, true, &ec);
    }
}

U_NAMESPACE_END

