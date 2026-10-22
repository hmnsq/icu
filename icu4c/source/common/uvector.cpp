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

#অন্তর্ভাৱ "uvector.h"
#অন্তর্ভাৱ "cmemory.h"
#অন্তর্ভাৱ "uarrsort.h"
#অন্তর্ভাৱ "uelement.h"

U_NAMESPACE_BEGIN

constexpr int32_t DEFAULT_CAPACITY = 8;

/*
 * Constants for hinting whether a key is an integer
 * or a pointer.  If a hint bit is zero, then the associated
 * token is assumed to be an integer. This is needed for iSeries
 */
constexpr int8_t HINT_চাবি_POINTER = 1;
constexpr int8_t HINT_চাবি_INTEGER = 0;
 
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
    যদি (U_FAILURE(status)) {
        ঘূৰাই;
    }
    // Fix bogus initialCapacity values; avoid malloc(0) and integer overflow
    যদি ((initialCapacity < 1) || (initialCapacity > static_cast<int32_t>(INT32_MAX / পরিমাণ(UElement)))) {
        initialCapacity = DEFAULT_CAPACITY;
    }
    elements = static_cast<UElement*>(uprv_malloc(পরিমাণ(UElement) * initialCapacity));
    যদি (elements == nullptr) {
        status = U_MEMORY_ALLOCATION_ERROR;
    } অন্যথা {
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
শূন্য UVector::assign(ধ্রুৱ UVector& other, UElementAssigner *assign, UErrorCode &ec) {
    যদি (ensureCapacity(other.count, ec)) {
        setSize(other.count, ec);
        যদি (U_SUCCESS(ec)) {
            বাবে (int32_t i=0; i<other.count; ++i) {
                যদি (elements[i].pointer != nullptr && deleter != nullptr) {
                    (*deleter)(elements[i].pointer);
                }
                (*assign)(&elements[i], &other.elements[i]);
            }
        }
    }
}

// This only does something sensible if this object has a non-null comparer
তার্কিক UVector::operator==(ধ্রুৱ UVector& other) ধ্রুৱ {
    U_ASSERT(comparer != nullptr);
    যদি (count != other.count) ঘূৰাই মিছা;
    যদি (comparer != nullptr) {
        // Compare using this object's comparer
        বাবে (int32_t i=0; i<count; ++i) {
            যদি (!(*comparer)(elements[i], other.elements[i])) {
                ঘূৰাই মিছা;
            }
        }
    }
    ঘূৰাই সঁচা;
}

শূন্য UVector::addElement(শূন্য* obj, UErrorCode &status) {
    U_ASSERT(deleter == nullptr);
    যদি (ensureCapacity(count + 1, status)) {
        elements[count++].pointer = obj;
    }
}

শূন্য UVector::adoptElement(শূন্য* obj, UErrorCode &status) {
    U_ASSERT(deleter != nullptr);
    যদি (ensureCapacity(count + 1, status)) {
        elements[count++].pointer = obj;
    } অন্যথা {
        (*deleter)(obj);
    }
}
শূন্য UVector::addElement(int32_t elem, UErrorCode &status) {
    U_ASSERT(deleter == nullptr);  // Usage error. Mixing up ints and pointers.
    যদি (ensureCapacity(count + 1, status)) {
        elements[count].pointer = nullptr;     // Pointers may be bigger than ints.
        elements[count].integer = elem;
        count++;
    }
}

শূন্য UVector::setElementAt(শূন্য* obj, int32_t index) {
    যদি (0 <= index && index < count) {
        যদি (elements[index].pointer != nullptr && deleter != nullptr) {
            (*deleter)(elements[index].pointer);
        }
        elements[index].pointer = obj;
    } অন্যথা {
        /* index out of range */
        যদি (deleter != nullptr) {
            (*deleter)(obj);
        }
    }
}

শূন্য UVector::setElementAt(int32_t elem, int32_t index) {
    U_ASSERT(deleter == nullptr);  // Usage error. Mixing up ints and pointers.
    যদি (0 <= index && index < count) {
        elements[index].pointer = nullptr;
        elements[index].integer = elem;
    }
    /* else index out of range */
}

শূন্য UVector::ঢোকাও_Element_At(শূন্য* obj, int32_t index, UErrorCode &status) {
    যদি (ensureCapacity(count + 1, status)) {
        যদি (0 <= index && index <= count) {
            বাবে (int32_t i=count; i>index; --i) {
                elements[i] = elements[i-1];
            }
            elements[index].pointer = obj;
            ++count;
        } অন্যথা {
            /* index out of range */
            status = U_ILLEGAL_ARGUMENT_ERROR;
        }
    }
    যদি (U_FAILURE(status) && deleter != nullptr) {
        (*deleter)(obj);
    }
}

শূন্য UVector::ঢোকাও_Element_At(int32_t elem, int32_t index, UErrorCode &status) {
    U_ASSERT(deleter == nullptr);  // Usage error. Mixing up ints and pointers.
    // must have 0 <= index <= count
    যদি (ensureCapacity(count + 1, status)) {
        যদি (0 <= index && index <= count) {
            বাবে (int32_t i=count; i>index; --i) {
                elements[i] = elements[i-1];
            }
            elements[index].pointer = nullptr;
            elements[index].integer = elem;
            ++count;
        } অন্যথা {
            /* index out of range */
            status = U_ILLEGAL_ARGUMENT_ERROR;
        }
    }
}

শূন্য* UVector::elementAt(int32_t index) ধ্রুৱ {
    ঘূৰাই (0 <= index && index < count) ? elements[index].pointer : nullptr;
}

int32_t UVector::elementAti(int32_t index) ধ্রুৱ {
    ঘূৰাই (0 <= index && index < count) ? elements[index].integer : 0;
}

UBool UVector::containsAll(ধ্রুৱ UVector& other) ধ্রুৱ {
    বাবে (int32_t i=0; i<other.size(); ++i) {
        যদি (indexOf(other.elements[i]) < 0) {
            ঘূৰাই মিছা;
        }
    }
    ঘূৰাই সঁচা;
}

UBool UVector::containsNone(ধ্রুৱ UVector& other) ধ্রুৱ {
    বাবে (int32_t i=0; i<other.size(); ++i) {
        যদি (indexOf(other.elements[i]) >= 0) {
            ঘূৰাই মিছা;
        }
    }
    ঘূৰাই সঁচা;
}

UBool UVector::removeAll(ধ্রুৱ UVector& other) {
    UBool changed = মিছা;
    বাবে (int32_t i=0; i<other.size(); ++i) {
        int32_t j = indexOf(other.elements[i]);
        যদি (j >= 0) {
            removeElementAt(j);
            changed = সঁচা;
        }
    }
    ঘূৰাই changed;
}

UBool UVector::retainAll(ধ্রুৱ UVector& other) {
    UBool changed = মিছা;
    বাবে (int32_t j=size()-1; j>=0; --j) {
        int32_t i = other.indexOf(elements[j]);
        যদি (i < 0) {
            removeElementAt(j);
            changed = সঁচা;
        }
    }
    ঘূৰাই changed;
}

শূন্য UVector::removeElementAt(int32_t index) {
    শূন্য* e = orphanElementAt(index);
    যদি (e != nullptr && deleter != nullptr) {
        (*deleter)(e);
    }
}

UBool UVector::removeElement(শূন্য* obj) {
    int32_t i = indexOf(obj);
    যদি (i >= 0) {
        removeElementAt(i);
        ঘূৰাই সঁচা;
    }
    ঘূৰাই মিছা;
}

শূন্য UVector::removeAllElements() {
    যদি (deleter != nullptr) {
        বাবে (int32_t i=0; i<count; ++i) {
            যদি (elements[i].pointer != nullptr) {
                (*deleter)(elements[i].pointer);
            }
        }
    }
    count = 0;
}

UBool   UVector::equals(ধ্রুৱ UVector &other) ধ্রুৱ {
    পূর্ণাঙ্ক      i;

    যদি (এতত্->count != other.count) {
        ঘূৰাই মিছা;
    }
    যদি (comparer == nullptr) {
        বাবে (i=0; i<count; i++) {
            যদি (elements[i].pointer != other.elements[i].pointer) {
                ঘূৰাই মিছা;
            }
        }
    } অন্যথা {
        UElement চাবি;
        বাবে (i=0; i<count; i++) {
            চাবি.pointer = &other.elements[i];
            যদি (!(*comparer)(চাবি, elements[i])) {
                ঘূৰাই মিছা;
            }
        }
    }
    ঘূৰাই সঁচা;
}



int32_t UVector::indexOf(শূন্য* obj, int32_t startIndex) ধ্রুৱ {
    UElement চাবি;
    চাবি.pointer = obj;
    ঘূৰাই indexOf(চাবি, startIndex, HINT_চাবি_POINTER);
}

int32_t UVector::indexOf(int32_t obj, int32_t startIndex) ধ্রুৱ {
    UElement চাবি;
    চাবি.integer = obj;
    ঘূৰাই indexOf(চাবি, startIndex, HINT_চাবি_INTEGER);
}

int32_t UVector::indexOf(UElement চাবি, int32_t startIndex, int8_t hint) ধ্রুৱ {
    যদি (comparer != nullptr) {
        বাবে (int32_t i=startIndex; i<count; ++i) {
            যদি ((*comparer)(চাবি, elements[i])) {
                ঘূৰাই i;
            }
        }
    } অন্যথা {
        বাবে (int32_t i=startIndex; i<count; ++i) {
            /* Pointers are not always the same size as ints so to perform
             * a valid comparison we need to know whether we are being
             * provided an int or a pointer. */
            যদি (hint & HINT_চাবি_POINTER) {
                যদি (চাবি.pointer == elements[i].pointer) {
                    ঘূৰাই i;
                }
            } অন্যথা {
                যদি (চাবি.integer == elements[i].integer) {
                    ঘূৰাই i;
                }
            }
        }
    }
    ঘূৰাই -1;
}

UBool UVector::ensureCapacity(int32_t minimumCapacity, UErrorCode &status) {
    যদি (U_FAILURE(status)) {
        ঘূৰাই মিছা;
    }
    যদি (minimumCapacity < 0) {
        status = U_ILLEGAL_ARGUMENT_ERROR;
        ঘূৰাই মিছা;
    }
    যদি (capacity < minimumCapacity) {
        যদি (capacity > (INT32_MAX - 1) / 2) {        	// integer overflow check
            status = U_ILLEGAL_ARGUMENT_ERROR;
            ঘূৰাই মিছা;
        }
        int32_t newCap = capacity * 2;
        যদি (newCap < minimumCapacity) {
            newCap = minimumCapacity;
        }
        যদি (newCap > static_cast<int32_t>(INT32_MAX / পরিমাণ(UElement))) { // integer overflow check
            // We keep the original memory contents on bad minimumCapacity.
            status = U_ILLEGAL_ARGUMENT_ERROR;
            ঘূৰাই মিছা;
        }
        UElement* newElems = static_cast<UElement*>(uprv_realloc(elements, পরিমাণ(UElement) * newCap));
        যদি (newElems == nullptr) {
            // We keep the original contents on the memory failure on realloc or bad minimumCapacity.
            status = U_MEMORY_ALLOCATION_ERROR;
            ঘূৰাই মিছা;
        }
        elements = newElems;
        capacity = newCap;
    }
    ঘূৰাই সঁচা;
}

/**
 * Change the size of this vector as follows: If newSize is smaller,
 * then truncate the array, possibly deleting held elements for i >=
 * newSize.  If newSize is larger, grow the array, filling in new
 * slots with nullptr.
 */
শূন্য UVector::setSize(int32_t newSize, UErrorCode &status) {
    যদি (!ensureCapacity(newSize, status)) {
        ঘূৰাই;
    }
    যদি (newSize > count) {
        UElement empty;
        empty.pointer = nullptr;
        empty.integer = 0;
        বাবে (int32_t i=count; i<newSize; ++i) {
            elements[i] = empty;
        }
    } অন্যথা {
        /* Most efficient to count down */
        বাবে (int32_t i=count-1; i>=newSize; --i) {
            removeElementAt(i);
        }
    }
    count = newSize;
}

/**
 * Fill in the given array with all elements of this vector.
 */
শূন্য** UVector::toArray(শূন্য** result) ধ্রুৱ {
    শূন্য** a = result;
    বাবে (পূর্ণাঙ্ক i=0; i<count; ++i) {
        *a++ = elements[i].pointer;
    }
    ঘূৰাই result;
}

UObjectDeleter *UVector::setDeleter(UObjectDeleter *d) {
    UObjectDeleter *old = deleter;
    deleter = d;
    ঘূৰাই old;
}

UElementsAreEqual *UVector::setComparer(UElementsAreEqual *d) {
    UElementsAreEqual *old = comparer;
    comparer = d;
    ঘূৰাই old;
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
শূন্য* UVector::orphanElementAt(int32_t index) {
    শূন্য* e = nullptr;
    যদি (0 <= index && index < count) {
        e = elements[index].pointer;
        বাবে (int32_t i=index; i<count-1; ++i) {
            elements[i] = elements[i+1];
        }
        --count;
    }
    /* else index out of range */
    ঘূৰাই e;
}

/**
 * Insert the given object into this vector at its sorted position
 * as defined by 'compare'.  The current elements are assumed to
 * be sorted already.
 */
শূন্য UVector::sorted_ঢোকাও(শূন্য* obj, UElementComparator *compare, UErrorCode& ec) {
    UElement e;
    e.pointer = obj;
    sorted_ঢোকাও(e, compare, ec);
}

/**
 * Insert the given integer into this vector at its sorted position
 * as defined by 'compare'.  The current elements are assumed to
 * be sorted already.
 */
শূন্য UVector::sorted_ঢোকাও(int32_t obj, UElementComparator *compare, UErrorCode& ec) {
    U_ASSERT(deleter == nullptr);
    UElement e {};
    e.integer = obj;
    sorted_ঢোকাও(e, compare, ec);
}

// ASSUME elements[] IS CURRENTLY SORTED
শূন্য UVector::sorted_ঢোকাও(UElement e, UElementComparator *compare, UErrorCode& ec) {
    // Perform a binary search for the location to insert tok at.  Tok
    // will be inserted between two elements a and b such that a <=
    // tok && tok < b, where there is a 'virtual' elements[-1] always
    // less than tok and a 'virtual' elements[count] always greater
    // than tok.
    যদি (!ensureCapacity(count + 1, ec)) {
        যদি (deleter != nullptr) {
            (*deleter)(e.pointer);
        }
        ঘূৰাই;
    }
    int32_t min = 0, max = count;
    যেতিয়ালৈ (min != max) {
        int32_t probe = (min + max) / 2;
        int32_t c = (*compare)(elements[probe], e);
        যদি (c > 0) {
            max = probe;
        } অন্যথা {
            // assert(c <= 0);
            min = probe + 1;
        }
    }
    বাবে (int32_t i=count; i>min; --i) {
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
স্থির int32_t U_CALLCONV
sortComparator(ধ্রুৱ শূন্য *context, ধ্রুৱ শূন্য *left, ধ্রুৱ শূন্য *ডান) {
    UElementComparator *compare = *static_cast<UElementComparator * ধ্রুৱ *>(context);
    UElement e1 = *static_cast<ধ্রুৱ UElement *>(left);
    UElement e2 = *static_cast<ধ্রুৱ UElement *>(ডান);
    int32_t result = (*compare)(e1, e2);
    ঘূৰাই result;
}


/**
  *  Array sort comparison function for use from UVector::sorti()
  *  Compares int32_t vector elements.
  */
স্থির int32_t U_CALLCONV
sortiComparator(ধ্রুৱ শূন্য * /*context */, ধ্রুৱ শূন্য *left, ধ্রুৱ শূন্য *ডান) {
    ধ্রুৱ UElement *e1 = static_cast<ধ্রুৱ UElement *>(left);
    ধ্রুৱ UElement *e2 = static_cast<ধ্রুৱ UElement *>(ডান);
    int32_t result = e1->integer < e2->integer? -1 :
                     e1->integer == e2->integer? 0 : 1;
    ঘূৰাই result;
}

/**
  * Sort the vector, assuming it contains ints.
  *     (A more general sort would take a comparison function, but it's
  *     not clear whether UVector's UElementComparator or
  *     UComparator from uprv_sortAray would be more appropriate.)
  */
শূন্য UVector::sorti(UErrorCode &ec) {
    যদি (U_SUCCESS(ec)) {
        uprv_sortArray(elements, count, পরিমাণ(UElement),
                       sortiComparator, nullptr,  মিছা, &ec);
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
শূন্য UVector::sort(UElementComparator *compare, UErrorCode &ec) {
    যদি (U_SUCCESS(ec)) {
        uprv_sortArray(elements, count, পরিমাণ(UElement),
                       sortComparator, &compare, মিছা, &ec);
    }
}


/**
 *  Stable sort with a user supplied comparator of type UComparator.
 */
শূন্য UVector::sortWithUComparator(UComparator *compare, ধ্রুৱ শূন্য *context, UErrorCode &ec) {
    যদি (U_SUCCESS(ec)) {
        uprv_sortArray(elements, count, পরিমাণ(UElement),
                       compare, context, সঁচা, &ec);
    }
}

U_NAMESPACE_END

