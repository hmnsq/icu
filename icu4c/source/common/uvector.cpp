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

#অন্তর্ভাব "uvector.h"
#অন্তর্ভাব "cmemory.h"
#অন্তর্ভাব "uarrsort.h"
#অন্তর্ভাব "uelement.h"

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
        ফেরত;
    }
    // Fix bogus initialCapacity values; avoid malloc(0) and integer overflow
    যদি ((initialCapacity < 1) || (initialCapacity > static_cast<int32_t>(INT32_MAX / আকার(UElement)))) {
        initialCapacity = DEFAULT_CAPACITY;
    }
    elements = static_cast<UElement*>(uprv_malloc(আকার(UElement) * initialCapacity));
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
শূন্য UVector::assign(ধ্রুবক UVector& other, UElementAssigner *assign, UErrorCode &ec) {
    যদি (ensureCapacity(other.count, ec)) {
        set_আকার(other.count, ec);
        যদি (U_SUCCESS(ec)) {
            জন্য (int32_t i=0; i<other.count; ++i) {
                যদি (elements[i].pointer != nullptr && deleter != nullptr) {
                    (*deleter)(elements[i].pointer);
                }
                (*assign)(&elements[i], &other.elements[i]);
            }
        }
    }
}

// This only does something sensible if this object has a non-null comparer
যৌক্তিক UVector::operator==(ধ্রুবক UVector& other) ধ্রুবক {
    U_ASSERT(comparer != nullptr);
    যদি (count != other.count) ফেরত মিথ্যা;
    যদি (comparer != nullptr) {
        // Compare using this object's comparer
        জন্য (int32_t i=0; i<count; ++i) {
            যদি (!(*comparer)(elements[i], other.elements[i])) {
                ফেরত মিথ্যা;
            }
        }
    }
    ফেরত সত্য;
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

শূন্য UVector::setElementAt(শূন্য* obj, int32_t সূচক) {
    যদি (0 <= সূচক && সূচক < count) {
        যদি (elements[সূচক].pointer != nullptr && deleter != nullptr) {
            (*deleter)(elements[সূচক].pointer);
        }
        elements[সূচক].pointer = obj;
    } অন্যথা {
        /* index out of range */
        যদি (deleter != nullptr) {
            (*deleter)(obj);
        }
    }
}

শূন্য UVector::setElementAt(int32_t elem, int32_t সূচক) {
    U_ASSERT(deleter == nullptr);  // Usage error. Mixing up ints and pointers.
    যদি (0 <= সূচক && সূচক < count) {
        elements[সূচক].pointer = nullptr;
        elements[সূচক].integer = elem;
    }
    /* else index out of range */
}

শূন্য UVector::ঢোকাও_Element_At(শূন্য* obj, int32_t সূচক, UErrorCode &status) {
    যদি (ensureCapacity(count + 1, status)) {
        যদি (0 <= সূচক && সূচক <= count) {
            জন্য (int32_t i=count; i>সূচক; --i) {
                elements[i] = elements[i-1];
            }
            elements[সূচক].pointer = obj;
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

শূন্য UVector::ঢোকাও_Element_At(int32_t elem, int32_t সূচক, UErrorCode &status) {
    U_ASSERT(deleter == nullptr);  // Usage error. Mixing up ints and pointers.
    // must have 0 <= index <= count
    যদি (ensureCapacity(count + 1, status)) {
        যদি (0 <= সূচক && সূচক <= count) {
            জন্য (int32_t i=count; i>সূচক; --i) {
                elements[i] = elements[i-1];
            }
            elements[সূচক].pointer = nullptr;
            elements[সূচক].integer = elem;
            ++count;
        } অন্যথা {
            /* index out of range */
            status = U_ILLEGAL_ARGUMENT_ERROR;
        }
    }
}

শূন্য* UVector::elementAt(int32_t সূচক) ধ্রুবক {
    ফেরত (0 <= সূচক && সূচক < count) ? elements[সূচক].pointer : nullptr;
}

int32_t UVector::elementAti(int32_t সূচক) ধ্রুবক {
    ফেরত (0 <= সূচক && সূচক < count) ? elements[সূচক].integer : 0;
}

UBool UVector::containsAll(ধ্রুবক UVector& other) ধ্রুবক {
    জন্য (int32_t i=0; i<other.size(); ++i) {
        যদি (সূচক_Of(other.elements[i]) < 0) {
            ফেরত মিথ্যা;
        }
    }
    ফেরত সত্য;
}

UBool UVector::containsNone(ধ্রুবক UVector& other) ধ্রুবক {
    জন্য (int32_t i=0; i<other.size(); ++i) {
        যদি (সূচক_Of(other.elements[i]) >= 0) {
            ফেরত মিথ্যা;
        }
    }
    ফেরত সত্য;
}

UBool UVector::removeAll(ধ্রুবক UVector& other) {
    UBool changed = মিথ্যা;
    জন্য (int32_t i=0; i<other.size(); ++i) {
        int32_t j = সূচক_Of(other.elements[i]);
        যদি (j >= 0) {
            removeElementAt(j);
            changed = সত্য;
        }
    }
    ফেরত changed;
}

UBool UVector::retainAll(ধ্রুবক UVector& other) {
    UBool changed = মিথ্যা;
    জন্য (int32_t j=size()-1; j>=0; --j) {
        int32_t i = other.সূচক_Of(elements[j]);
        যদি (i < 0) {
            removeElementAt(j);
            changed = সত্য;
        }
    }
    ফেরত changed;
}

শূন্য UVector::removeElementAt(int32_t সূচক) {
    শূন্য* e = orphanElementAt(সূচক);
    যদি (e != nullptr && deleter != nullptr) {
        (*deleter)(e);
    }
}

UBool UVector::removeElement(শূন্য* obj) {
    int32_t i = সূচক_Of(obj);
    যদি (i >= 0) {
        removeElementAt(i);
        ফেরত সত্য;
    }
    ফেরত মিথ্যা;
}

শূন্য UVector::removeAllElements() {
    যদি (deleter != nullptr) {
        জন্য (int32_t i=0; i<count; ++i) {
            যদি (elements[i].pointer != nullptr) {
                (*deleter)(elements[i].pointer);
            }
        }
    }
    count = 0;
}

UBool   UVector::equals(ধ্রুবক UVector &other) ধ্রুবক {
    পূর্ণসংখ্যা      i;

    যদি (এই->count != other.count) {
        ফেরত মিথ্যা;
    }
    যদি (comparer == nullptr) {
        জন্য (i=0; i<count; i++) {
            যদি (elements[i].pointer != other.elements[i].pointer) {
                ফেরত মিথ্যা;
            }
        }
    } অন্যথা {
        UElement চাবি;
        জন্য (i=0; i<count; i++) {
            চাবি.pointer = &other.elements[i];
            যদি (!(*comparer)(চাবি, elements[i])) {
                ফেরত মিথ্যা;
            }
        }
    }
    ফেরত সত্য;
}



int32_t UVector::সূচক_Of(শূন্য* obj, int32_t start_সূচক) ধ্রুবক {
    UElement চাবি;
    চাবি.pointer = obj;
    ফেরত সূচক_Of(চাবি, start_সূচক, HINT_চাবি_POINTER);
}

int32_t UVector::সূচক_Of(int32_t obj, int32_t start_সূচক) ধ্রুবক {
    UElement চাবি;
    চাবি.integer = obj;
    ফেরত সূচক_Of(চাবি, start_সূচক, HINT_চাবি_INTEGER);
}

int32_t UVector::সূচক_Of(UElement চাবি, int32_t start_সূচক, int8_t hint) ধ্রুবক {
    যদি (comparer != nullptr) {
        জন্য (int32_t i=start_সূচক; i<count; ++i) {
            যদি ((*comparer)(চাবি, elements[i])) {
                ফেরত i;
            }
        }
    } অন্যথা {
        জন্য (int32_t i=start_সূচক; i<count; ++i) {
            /* Pointers are not always the same size as ints so to perform
             * a valid comparison we need to know whether we are being
             * provided an int or a pointer. */
            যদি (hint & HINT_চাবি_POINTER) {
                যদি (চাবি.pointer == elements[i].pointer) {
                    ফেরত i;
                }
            } অন্যথা {
                যদি (চাবি.integer == elements[i].integer) {
                    ফেরত i;
                }
            }
        }
    }
    ফেরত -1;
}

UBool UVector::ensureCapacity(int32_t minimumCapacity, UErrorCode &status) {
    যদি (U_FAILURE(status)) {
        ফেরত মিথ্যা;
    }
    যদি (minimumCapacity < 0) {
        status = U_ILLEGAL_ARGUMENT_ERROR;
        ফেরত মিথ্যা;
    }
    যদি (capacity < minimumCapacity) {
        যদি (capacity > (INT32_MAX - 1) / 2) {        	// integer overflow check
            status = U_ILLEGAL_ARGUMENT_ERROR;
            ফেরত মিথ্যা;
        }
        int32_t newCap = capacity * 2;
        যদি (newCap < minimumCapacity) {
            newCap = minimumCapacity;
        }
        যদি (newCap > static_cast<int32_t>(INT32_MAX / আকার(UElement))) { // integer overflow check
            // We keep the original memory contents on bad minimumCapacity.
            status = U_ILLEGAL_ARGUMENT_ERROR;
            ফেরত মিথ্যা;
        }
        UElement* newElems = static_cast<UElement*>(uprv_realloc(elements, আকার(UElement) * newCap));
        যদি (newElems == nullptr) {
            // We keep the original contents on the memory failure on realloc or bad minimumCapacity.
            status = U_MEMORY_ALLOCATION_ERROR;
            ফেরত মিথ্যা;
        }
        elements = newElems;
        capacity = newCap;
    }
    ফেরত সত্য;
}

/**
 * Change the size of this vector as follows: If newSize is smaller,
 * then truncate the array, possibly deleting held elements for i >=
 * newSize.  If newSize is larger, grow the array, filling in new
 * slots with nullptr.
 */
শূন্য UVector::set_আকার(int32_t new_আকার, UErrorCode &status) {
    যদি (!ensureCapacity(new_আকার, status)) {
        ফেরত;
    }
    যদি (new_আকার > count) {
        UElement empty;
        empty.pointer = nullptr;
        empty.integer = 0;
        জন্য (int32_t i=count; i<new_আকার; ++i) {
            elements[i] = empty;
        }
    } অন্যথা {
        /* Most efficient to count down */
        জন্য (int32_t i=count-1; i>=new_আকার; --i) {
            removeElementAt(i);
        }
    }
    count = new_আকার;
}

/**
 * Fill in the given array with all elements of this vector.
 */
শূন্য** UVector::toArray(শূন্য** ফলাফল) ধ্রুবক {
    শূন্য** a = ফলাফল;
    জন্য (পূর্ণসংখ্যা i=0; i<count; ++i) {
        *a++ = elements[i].pointer;
    }
    ফেরত ফলাফল;
}

UObjectDeleter *UVector::setDeleter(UObjectDeleter *d) {
    UObjectDeleter *old = deleter;
    deleter = d;
    ফেরত old;
}

UElementsAreEqual *UVector::setComparer(UElementsAreEqual *d) {
    UElementsAreEqual *old = comparer;
    comparer = d;
    ফেরত old;
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
শূন্য* UVector::orphanElementAt(int32_t সূচক) {
    শূন্য* e = nullptr;
    যদি (0 <= সূচক && সূচক < count) {
        e = elements[সূচক].pointer;
        জন্য (int32_t i=সূচক; i<count-1; ++i) {
            elements[i] = elements[i+1];
        }
        --count;
    }
    /* else index out of range */
    ফেরত e;
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
        ফেরত;
    }
    int32_t min = 0, max = count;
    যখন (min != max) {
        int32_t probe = (min + max) / 2;
        int32_t c = (*compare)(elements[probe], e);
        যদি (c > 0) {
            max = probe;
        } অন্যথা {
            // assert(c <= 0);
            min = probe + 1;
        }
    }
    জন্য (int32_t i=count; i>min; --i) {
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
sortComparator(ধ্রুবক শূন্য *context, ধ্রুবক শূন্য *left, ধ্রুবক শূন্য *ডান) {
    UElementComparator *compare = *static_cast<UElementComparator * ধ্রুবক *>(context);
    UElement e1 = *static_cast<ধ্রুবক UElement *>(left);
    UElement e2 = *static_cast<ধ্রুবক UElement *>(ডান);
    int32_t ফলাফল = (*compare)(e1, e2);
    ফেরত ফলাফল;
}


/**
  *  Array sort comparison function for use from UVector::sorti()
  *  Compares int32_t vector elements.
  */
স্থির int32_t U_CALLCONV
sortiComparator(ধ্রুবক শূন্য * /*context */, ধ্রুবক শূন্য *left, ধ্রুবক শূন্য *ডান) {
    ধ্রুবক UElement *e1 = static_cast<ধ্রুবক UElement *>(left);
    ধ্রুবক UElement *e2 = static_cast<ধ্রুবক UElement *>(ডান);
    int32_t ফলাফল = e1->integer < e2->integer? -1 :
                     e1->integer == e2->integer? 0 : 1;
    ফেরত ফলাফল;
}

/**
  * Sort the vector, assuming it contains ints.
  *     (A more general sort would take a comparison function, but it's
  *     not clear whether UVector's UElementComparator or
  *     UComparator from uprv_sortAray would be more appropriate.)
  */
শূন্য UVector::sorti(UErrorCode &ec) {
    যদি (U_SUCCESS(ec)) {
        uprv_sortArray(elements, count, আকার(UElement),
                       sortiComparator, nullptr,  মিথ্যা, &ec);
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
        uprv_sortArray(elements, count, আকার(UElement),
                       sortComparator, &compare, মিথ্যা, &ec);
    }
}


/**
 *  Stable sort with a user supplied comparator of type UComparator.
 */
শূন্য UVector::sortWithUComparator(UComparator *compare, ধ্রুবক শূন্য *context, UErrorCode &ec) {
    যদি (U_SUCCESS(ec)) {
        uprv_sortArray(elements, count, আকার(UElement),
                       compare, context, সত্য, &ec);
    }
}

U_NAMESPACE_END

