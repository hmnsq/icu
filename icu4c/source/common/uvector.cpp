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

#شامل "uvector.h"
#شامل "cmemory.h"
#شامل "uarrsort.h"
#شامل "uelement.h"

U_NAMESPACE_BEGIN

constexpr int32_t DEFAULT_CAPACITY = 8;

/*
 * Constants for hinting whether a key is an integer
 * or a pointer.  If a hint bit is zero, then the associated
 * token is assumed to be an integer. This is needed for iSeries
 */
constexpr int8_t HINT_کلید_POINTER = 1;
constexpr int8_t HINT_کلید_INTEGER = 0;
 
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
    اگر (U_FAILURE(status)) {
        بازگشت;
    }
    // Fix bogus initialCapacity values; avoid malloc(0) and integer overflow
    اگر ((initialCapacity < 1) || (initialCapacity > static_cast<int32_t>(INT32_MAX / اندازه(UElement)))) {
        initialCapacity = DEFAULT_CAPACITY;
    }
    elements = static_cast<UElement*>(uprv_malloc(اندازه(UElement) * initialCapacity));
    اگر (elements == nullptr) {
        status = U_MEMORY_ALLOCATION_ERROR;
    } وگرنه {
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
تهی UVector::assign(ثابت UVector& other, UElementAssigner *assign, UErrorCode &ec) {
    اگر (ensureCapacity(other.count, ec)) {
        setSize(other.count, ec);
        اگر (U_SUCCESS(ec)) {
            برای (int32_t i=0; i<other.count; ++i) {
                اگر (elements[i].pointer != nullptr && deleter != nullptr) {
                    (*deleter)(elements[i].pointer);
                }
                (*assign)(&elements[i], &other.elements[i]);
            }
        }
    }
}

// This only does something sensible if this object has a non-null comparer
منطقی UVector::operator==(ثابت UVector& other) ثابت {
    U_ASSERT(comparer != nullptr);
    اگر (count != other.count) بازگشت نادرست;
    اگر (comparer != nullptr) {
        // Compare using this object's comparer
        برای (int32_t i=0; i<count; ++i) {
            اگر (!(*comparer)(elements[i], other.elements[i])) {
                بازگشت نادرست;
            }
        }
    }
    بازگشت درست;
}

تهی UVector::addElement(تهی* obj, UErrorCode &status) {
    U_ASSERT(deleter == nullptr);
    اگر (ensureCapacity(count + 1, status)) {
        elements[count++].pointer = obj;
    }
}

تهی UVector::adoptElement(تهی* obj, UErrorCode &status) {
    U_ASSERT(deleter != nullptr);
    اگر (ensureCapacity(count + 1, status)) {
        elements[count++].pointer = obj;
    } وگرنه {
        (*deleter)(obj);
    }
}
تهی UVector::addElement(int32_t elem, UErrorCode &status) {
    U_ASSERT(deleter == nullptr);  // Usage error. Mixing up ints and pointers.
    اگر (ensureCapacity(count + 1, status)) {
        elements[count].pointer = nullptr;     // Pointers may be bigger than ints.
        elements[count].integer = elem;
        count++;
    }
}

تهی UVector::setElementAt(تهی* obj, int32_t index) {
    اگر (0 <= index && index < count) {
        اگر (elements[index].pointer != nullptr && deleter != nullptr) {
            (*deleter)(elements[index].pointer);
        }
        elements[index].pointer = obj;
    } وگرنه {
        /* index out of range */
        اگر (deleter != nullptr) {
            (*deleter)(obj);
        }
    }
}

تهی UVector::setElementAt(int32_t elem, int32_t index) {
    U_ASSERT(deleter == nullptr);  // Usage error. Mixing up ints and pointers.
    اگر (0 <= index && index < count) {
        elements[index].pointer = nullptr;
        elements[index].integer = elem;
    }
    /* else index out of range */
}

تهی UVector::درج_Element_At(تهی* obj, int32_t index, UErrorCode &status) {
    اگر (ensureCapacity(count + 1, status)) {
        اگر (0 <= index && index <= count) {
            برای (int32_t i=count; i>index; --i) {
                elements[i] = elements[i-1];
            }
            elements[index].pointer = obj;
            ++count;
        } وگرنه {
            /* index out of range */
            status = U_ILLEGAL_ARGUMENT_ERROR;
        }
    }
    اگر (U_FAILURE(status) && deleter != nullptr) {
        (*deleter)(obj);
    }
}

تهی UVector::درج_Element_At(int32_t elem, int32_t index, UErrorCode &status) {
    U_ASSERT(deleter == nullptr);  // Usage error. Mixing up ints and pointers.
    // must have 0 <= index <= count
    اگر (ensureCapacity(count + 1, status)) {
        اگر (0 <= index && index <= count) {
            برای (int32_t i=count; i>index; --i) {
                elements[i] = elements[i-1];
            }
            elements[index].pointer = nullptr;
            elements[index].integer = elem;
            ++count;
        } وگرنه {
            /* index out of range */
            status = U_ILLEGAL_ARGUMENT_ERROR;
        }
    }
}

تهی* UVector::elementAt(int32_t index) ثابت {
    بازگشت (0 <= index && index < count) ? elements[index].pointer : nullptr;
}

int32_t UVector::elementAti(int32_t index) ثابت {
    بازگشت (0 <= index && index < count) ? elements[index].integer : 0;
}

UBool UVector::containsAll(ثابت UVector& other) ثابت {
    برای (int32_t i=0; i<other.size(); ++i) {
        اگر (indexOf(other.elements[i]) < 0) {
            بازگشت نادرست;
        }
    }
    بازگشت درست;
}

UBool UVector::containsNone(ثابت UVector& other) ثابت {
    برای (int32_t i=0; i<other.size(); ++i) {
        اگر (indexOf(other.elements[i]) >= 0) {
            بازگشت نادرست;
        }
    }
    بازگشت درست;
}

UBool UVector::removeAll(ثابت UVector& other) {
    UBool changed = نادرست;
    برای (int32_t i=0; i<other.size(); ++i) {
        int32_t j = indexOf(other.elements[i]);
        اگر (j >= 0) {
            removeElementAt(j);
            changed = درست;
        }
    }
    بازگشت changed;
}

UBool UVector::retainAll(ثابت UVector& other) {
    UBool changed = نادرست;
    برای (int32_t j=size()-1; j>=0; --j) {
        int32_t i = other.indexOf(elements[j]);
        اگر (i < 0) {
            removeElementAt(j);
            changed = درست;
        }
    }
    بازگشت changed;
}

تهی UVector::removeElementAt(int32_t index) {
    تهی* e = orphanElementAt(index);
    اگر (e != nullptr && deleter != nullptr) {
        (*deleter)(e);
    }
}

UBool UVector::removeElement(تهی* obj) {
    int32_t i = indexOf(obj);
    اگر (i >= 0) {
        removeElementAt(i);
        بازگشت درست;
    }
    بازگشت نادرست;
}

تهی UVector::removeAllElements() {
    اگر (deleter != nullptr) {
        برای (int32_t i=0; i<count; ++i) {
            اگر (elements[i].pointer != nullptr) {
                (*deleter)(elements[i].pointer);
            }
        }
    }
    count = 0;
}

UBool   UVector::equals(ثابت UVector &other) ثابت {
    صحیح      i;

    اگر (این->count != other.count) {
        بازگشت نادرست;
    }
    اگر (comparer == nullptr) {
        برای (i=0; i<count; i++) {
            اگر (elements[i].pointer != other.elements[i].pointer) {
                بازگشت نادرست;
            }
        }
    } وگرنه {
        UElement کلید;
        برای (i=0; i<count; i++) {
            کلید.pointer = &other.elements[i];
            اگر (!(*comparer)(کلید, elements[i])) {
                بازگشت نادرست;
            }
        }
    }
    بازگشت درست;
}



int32_t UVector::indexOf(تهی* obj, int32_t startIndex) ثابت {
    UElement کلید;
    کلید.pointer = obj;
    بازگشت indexOf(کلید, startIndex, HINT_کلید_POINTER);
}

int32_t UVector::indexOf(int32_t obj, int32_t startIndex) ثابت {
    UElement کلید;
    کلید.integer = obj;
    بازگشت indexOf(کلید, startIndex, HINT_کلید_INTEGER);
}

int32_t UVector::indexOf(UElement کلید, int32_t startIndex, int8_t hint) ثابت {
    اگر (comparer != nullptr) {
        برای (int32_t i=startIndex; i<count; ++i) {
            اگر ((*comparer)(کلید, elements[i])) {
                بازگشت i;
            }
        }
    } وگرنه {
        برای (int32_t i=startIndex; i<count; ++i) {
            /* Pointers are not always the same size as ints so to perform
             * a valid comparison we need to know whether we are being
             * provided an int or a pointer. */
            اگر (hint & HINT_کلید_POINTER) {
                اگر (کلید.pointer == elements[i].pointer) {
                    بازگشت i;
                }
            } وگرنه {
                اگر (کلید.integer == elements[i].integer) {
                    بازگشت i;
                }
            }
        }
    }
    بازگشت -1;
}

UBool UVector::ensureCapacity(int32_t minimumCapacity, UErrorCode &status) {
    اگر (U_FAILURE(status)) {
        بازگشت نادرست;
    }
    اگر (minimumCapacity < 0) {
        status = U_ILLEGAL_ARGUMENT_ERROR;
        بازگشت نادرست;
    }
    اگر (capacity < minimumCapacity) {
        اگر (capacity > (INT32_MAX - 1) / 2) {        	// integer overflow check
            status = U_ILLEGAL_ARGUMENT_ERROR;
            بازگشت نادرست;
        }
        int32_t newCap = capacity * 2;
        اگر (newCap < minimumCapacity) {
            newCap = minimumCapacity;
        }
        اگر (newCap > static_cast<int32_t>(INT32_MAX / اندازه(UElement))) { // integer overflow check
            // We keep the original memory contents on bad minimumCapacity.
            status = U_ILLEGAL_ARGUMENT_ERROR;
            بازگشت نادرست;
        }
        UElement* newElems = static_cast<UElement*>(uprv_realloc(elements, اندازه(UElement) * newCap));
        اگر (newElems == nullptr) {
            // We keep the original contents on the memory failure on realloc or bad minimumCapacity.
            status = U_MEMORY_ALLOCATION_ERROR;
            بازگشت نادرست;
        }
        elements = newElems;
        capacity = newCap;
    }
    بازگشت درست;
}

/**
 * Change the size of this vector as follows: If newSize is smaller,
 * then truncate the array, possibly deleting held elements for i >=
 * newSize.  If newSize is larger, grow the array, filling in new
 * slots with nullptr.
 */
تهی UVector::setSize(int32_t newSize, UErrorCode &status) {
    اگر (!ensureCapacity(newSize, status)) {
        بازگشت;
    }
    اگر (newSize > count) {
        UElement empty;
        empty.pointer = nullptr;
        empty.integer = 0;
        برای (int32_t i=count; i<newSize; ++i) {
            elements[i] = empty;
        }
    } وگرنه {
        /* Most efficient to count down */
        برای (int32_t i=count-1; i>=newSize; --i) {
            removeElementAt(i);
        }
    }
    count = newSize;
}

/**
 * Fill in the given array with all elements of this vector.
 */
تهی** UVector::toArray(تهی** result) ثابت {
    تهی** a = result;
    برای (صحیح i=0; i<count; ++i) {
        *a++ = elements[i].pointer;
    }
    بازگشت result;
}

UObjectDeleter *UVector::setDeleter(UObjectDeleter *d) {
    UObjectDeleter *old = deleter;
    deleter = d;
    بازگشت old;
}

UElementsAreEqual *UVector::setComparer(UElementsAreEqual *d) {
    UElementsAreEqual *old = comparer;
    comparer = d;
    بازگشت old;
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
تهی* UVector::orphanElementAt(int32_t index) {
    تهی* e = nullptr;
    اگر (0 <= index && index < count) {
        e = elements[index].pointer;
        برای (int32_t i=index; i<count-1; ++i) {
            elements[i] = elements[i+1];
        }
        --count;
    }
    /* else index out of range */
    بازگشت e;
}

/**
 * Insert the given object into this vector at its sorted position
 * as defined by 'compare'.  The current elements are assumed to
 * be sorted already.
 */
تهی UVector::sorted_درج(تهی* obj, UElementComparator *compare, UErrorCode& ec) {
    UElement e;
    e.pointer = obj;
    sorted_درج(e, compare, ec);
}

/**
 * Insert the given integer into this vector at its sorted position
 * as defined by 'compare'.  The current elements are assumed to
 * be sorted already.
 */
تهی UVector::sorted_درج(int32_t obj, UElementComparator *compare, UErrorCode& ec) {
    U_ASSERT(deleter == nullptr);
    UElement e {};
    e.integer = obj;
    sorted_درج(e, compare, ec);
}

// ASSUME elements[] IS CURRENTLY SORTED
تهی UVector::sorted_درج(UElement e, UElementComparator *compare, UErrorCode& ec) {
    // Perform a binary search for the location to insert tok at.  Tok
    // will be inserted between two elements a and b such that a <=
    // tok && tok < b, where there is a 'virtual' elements[-1] always
    // less than tok and a 'virtual' elements[count] always greater
    // than tok.
    اگر (!ensureCapacity(count + 1, ec)) {
        اگر (deleter != nullptr) {
            (*deleter)(e.pointer);
        }
        بازگشت;
    }
    int32_t min = 0, max = count;
    وقتی (min != max) {
        int32_t probe = (min + max) / 2;
        int32_t c = (*compare)(elements[probe], e);
        اگر (c > 0) {
            max = probe;
        } وگرنه {
            // assert(c <= 0);
            min = probe + 1;
        }
    }
    برای (int32_t i=count; i>min; --i) {
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
ایستا int32_t U_CALLCONV
sortComparator(ثابت تهی *context, ثابت تهی *left, ثابت تهی *راست) {
    UElementComparator *compare = *static_cast<UElementComparator * ثابت *>(context);
    UElement e1 = *static_cast<ثابت UElement *>(left);
    UElement e2 = *static_cast<ثابت UElement *>(راست);
    int32_t result = (*compare)(e1, e2);
    بازگشت result;
}


/**
  *  Array sort comparison function for use from UVector::sorti()
  *  Compares int32_t vector elements.
  */
ایستا int32_t U_CALLCONV
sortiComparator(ثابت تهی * /*context */, ثابت تهی *left, ثابت تهی *راست) {
    ثابت UElement *e1 = static_cast<ثابت UElement *>(left);
    ثابت UElement *e2 = static_cast<ثابت UElement *>(راست);
    int32_t result = e1->integer < e2->integer? -1 :
                     e1->integer == e2->integer? 0 : 1;
    بازگشت result;
}

/**
  * Sort the vector, assuming it contains ints.
  *     (A more general sort would take a comparison function, but it's
  *     not clear whether UVector's UElementComparator or
  *     UComparator from uprv_sortAray would be more appropriate.)
  */
تهی UVector::sorti(UErrorCode &ec) {
    اگر (U_SUCCESS(ec)) {
        uprv_sortArray(elements, count, اندازه(UElement),
                       sortiComparator, nullptr,  نادرست, &ec);
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
تهی UVector::sort(UElementComparator *compare, UErrorCode &ec) {
    اگر (U_SUCCESS(ec)) {
        uprv_sortArray(elements, count, اندازه(UElement),
                       sortComparator, &compare, نادرست, &ec);
    }
}


/**
 *  Stable sort with a user supplied comparator of type UComparator.
 */
تهی UVector::sortWithUComparator(UComparator *compare, ثابت تهی *context, UErrorCode &ec) {
    اگر (U_SUCCESS(ec)) {
        uprv_sortArray(elements, count, اندازه(UElement),
                       compare, context, درست, &ec);
    }
}

U_NAMESPACE_END

