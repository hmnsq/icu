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

#அந்தர்பாவ "uvector.h"
#அந்தர்பாவ "cmemory.h"
#அந்தர்பாவ "uarrsort.h"
#அந்தர்பாவ "uelement.h"

U_NAMESPACE_BEGIN

constexpr int32_t DEFAULT_CAPACITY = 8;

/*
 * Constants for hinting whether a key is an integer
 * or a pointer.  If a hint bit is zero, then the associated
 * token is assumed to be an integer. This is needed for iSeries
 */
constexpr int8_t HINT_சாவி_POINTER = 1;
constexpr int8_t HINT_சாவி_INTEGER = 0;
 
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
    எனில் (U_FAILURE(status)) {
        திரும்பு;
    }
    // Fix bogus initialCapacity values; avoid malloc(0) and integer overflow
    எனில் ((initialCapacity < 1) || (initialCapacity > static_cast<int32_t>(INT32_MAX / அளவு(UElement)))) {
        initialCapacity = DEFAULT_CAPACITY;
    }
    elements = static_cast<UElement*>(uprv_malloc(அளவு(UElement) * initialCapacity));
    எனில் (elements == nullptr) {
        status = U_MEMORY_ALLOCATION_ERROR;
    } இல்லையெனில் {
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
வெறுமை UVector::assign(மாறிலி UVector& other, UElementAssigner *assign, UErrorCode &ec) {
    எனில் (ensureCapacity(other.count, ec)) {
        setSize(other.count, ec);
        எனில் (U_SUCCESS(ec)) {
            க்கு (int32_t i=0; i<other.count; ++i) {
                எனில் (elements[i].pointer != nullptr && deleter != nullptr) {
                    (*deleter)(elements[i].pointer);
                }
                (*assign)(&elements[i], &other.elements[i]);
            }
        }
    }
}

// This only does something sensible if this object has a non-null comparer
தருக்க UVector::operator==(மாறிலி UVector& other) மாறிலி {
    U_ASSERT(comparer != nullptr);
    எனில் (count != other.count) திரும்பு பொய்;
    எனில் (comparer != nullptr) {
        // Compare using this object's comparer
        க்கு (int32_t i=0; i<count; ++i) {
            எனில் (!(*comparer)(elements[i], other.elements[i])) {
                திரும்பு பொய்;
            }
        }
    }
    திரும்பு உண்மை;
}

வெறுமை UVector::addElement(வெறுமை* obj, UErrorCode &status) {
    U_ASSERT(deleter == nullptr);
    எனில் (ensureCapacity(count + 1, status)) {
        elements[count++].pointer = obj;
    }
}

வெறுமை UVector::adoptElement(வெறுமை* obj, UErrorCode &status) {
    U_ASSERT(deleter != nullptr);
    எனில் (ensureCapacity(count + 1, status)) {
        elements[count++].pointer = obj;
    } இல்லையெனில் {
        (*deleter)(obj);
    }
}
வெறுமை UVector::addElement(int32_t elem, UErrorCode &status) {
    U_ASSERT(deleter == nullptr);  // Usage error. Mixing up ints and pointers.
    எனில் (ensureCapacity(count + 1, status)) {
        elements[count].pointer = nullptr;     // Pointers may be bigger than ints.
        elements[count].integer = elem;
        count++;
    }
}

வெறுமை UVector::setElementAt(வெறுமை* obj, int32_t index) {
    எனில் (0 <= index && index < count) {
        எனில் (elements[index].pointer != nullptr && deleter != nullptr) {
            (*deleter)(elements[index].pointer);
        }
        elements[index].pointer = obj;
    } இல்லையெனில் {
        /* index out of range */
        எனில் (deleter != nullptr) {
            (*deleter)(obj);
        }
    }
}

வெறுமை UVector::setElementAt(int32_t elem, int32_t index) {
    U_ASSERT(deleter == nullptr);  // Usage error. Mixing up ints and pointers.
    எனில் (0 <= index && index < count) {
        elements[index].pointer = nullptr;
        elements[index].integer = elem;
    }
    /* else index out of range */
}

வெறுமை UVector::செருகு_Element_At(வெறுமை* obj, int32_t index, UErrorCode &status) {
    எனில் (ensureCapacity(count + 1, status)) {
        எனில் (0 <= index && index <= count) {
            க்கு (int32_t i=count; i>index; --i) {
                elements[i] = elements[i-1];
            }
            elements[index].pointer = obj;
            ++count;
        } இல்லையெனில் {
            /* index out of range */
            status = U_ILLEGAL_ARGUMENT_ERROR;
        }
    }
    எனில் (U_FAILURE(status) && deleter != nullptr) {
        (*deleter)(obj);
    }
}

வெறுமை UVector::செருகு_Element_At(int32_t elem, int32_t index, UErrorCode &status) {
    U_ASSERT(deleter == nullptr);  // Usage error. Mixing up ints and pointers.
    // must have 0 <= index <= count
    எனில் (ensureCapacity(count + 1, status)) {
        எனில் (0 <= index && index <= count) {
            க்கு (int32_t i=count; i>index; --i) {
                elements[i] = elements[i-1];
            }
            elements[index].pointer = nullptr;
            elements[index].integer = elem;
            ++count;
        } இல்லையெனில் {
            /* index out of range */
            status = U_ILLEGAL_ARGUMENT_ERROR;
        }
    }
}

வெறுமை* UVector::elementAt(int32_t index) மாறிலி {
    திரும்பு (0 <= index && index < count) ? elements[index].pointer : nullptr;
}

int32_t UVector::elementAti(int32_t index) மாறிலி {
    திரும்பு (0 <= index && index < count) ? elements[index].integer : 0;
}

UBool UVector::containsAll(மாறிலி UVector& other) மாறிலி {
    க்கு (int32_t i=0; i<other.size(); ++i) {
        எனில் (indexOf(other.elements[i]) < 0) {
            திரும்பு பொய்;
        }
    }
    திரும்பு உண்மை;
}

UBool UVector::containsNone(மாறிலி UVector& other) மாறிலி {
    க்கு (int32_t i=0; i<other.size(); ++i) {
        எனில் (indexOf(other.elements[i]) >= 0) {
            திரும்பு பொய்;
        }
    }
    திரும்பு உண்மை;
}

UBool UVector::removeAll(மாறிலி UVector& other) {
    UBool changed = பொய்;
    க்கு (int32_t i=0; i<other.size(); ++i) {
        int32_t j = indexOf(other.elements[i]);
        எனில் (j >= 0) {
            removeElementAt(j);
            changed = உண்மை;
        }
    }
    திரும்பு changed;
}

UBool UVector::retainAll(மாறிலி UVector& other) {
    UBool changed = பொய்;
    க்கு (int32_t j=size()-1; j>=0; --j) {
        int32_t i = other.indexOf(elements[j]);
        எனில் (i < 0) {
            removeElementAt(j);
            changed = உண்மை;
        }
    }
    திரும்பு changed;
}

வெறுமை UVector::removeElementAt(int32_t index) {
    வெறுமை* e = orphanElementAt(index);
    எனில் (e != nullptr && deleter != nullptr) {
        (*deleter)(e);
    }
}

UBool UVector::removeElement(வெறுமை* obj) {
    int32_t i = indexOf(obj);
    எனில் (i >= 0) {
        removeElementAt(i);
        திரும்பு உண்மை;
    }
    திரும்பு பொய்;
}

வெறுமை UVector::removeAllElements() {
    எனில் (deleter != nullptr) {
        க்கு (int32_t i=0; i<count; ++i) {
            எனில் (elements[i].pointer != nullptr) {
                (*deleter)(elements[i].pointer);
            }
        }
    }
    count = 0;
}

UBool   UVector::equals(மாறிலி UVector &other) மாறிலி {
    முழு      i;

    எனில் (இது->count != other.count) {
        திரும்பு பொய்;
    }
    எனில் (comparer == nullptr) {
        க்கு (i=0; i<count; i++) {
            எனில் (elements[i].pointer != other.elements[i].pointer) {
                திரும்பு பொய்;
            }
        }
    } இல்லையெனில் {
        UElement சாவி;
        க்கு (i=0; i<count; i++) {
            சாவி.pointer = &other.elements[i];
            எனில் (!(*comparer)(சாவி, elements[i])) {
                திரும்பு பொய்;
            }
        }
    }
    திரும்பு உண்மை;
}



int32_t UVector::indexOf(வெறுமை* obj, int32_t startIndex) மாறிலி {
    UElement சாவி;
    சாவி.pointer = obj;
    திரும்பு indexOf(சாவி, startIndex, HINT_சாவி_POINTER);
}

int32_t UVector::indexOf(int32_t obj, int32_t startIndex) மாறிலி {
    UElement சாவி;
    சாவி.integer = obj;
    திரும்பு indexOf(சாவி, startIndex, HINT_சாவி_INTEGER);
}

int32_t UVector::indexOf(UElement சாவி, int32_t startIndex, int8_t hint) மாறிலி {
    எனில் (comparer != nullptr) {
        க்கு (int32_t i=startIndex; i<count; ++i) {
            எனில் ((*comparer)(சாவி, elements[i])) {
                திரும்பு i;
            }
        }
    } இல்லையெனில் {
        க்கு (int32_t i=startIndex; i<count; ++i) {
            /* Pointers are not always the same size as ints so to perform
             * a valid comparison we need to know whether we are being
             * provided an int or a pointer. */
            எனில் (hint & HINT_சாவி_POINTER) {
                எனில் (சாவி.pointer == elements[i].pointer) {
                    திரும்பு i;
                }
            } இல்லையெனில் {
                எனில் (சாவி.integer == elements[i].integer) {
                    திரும்பு i;
                }
            }
        }
    }
    திரும்பு -1;
}

UBool UVector::ensureCapacity(int32_t minimumCapacity, UErrorCode &status) {
    எனில் (U_FAILURE(status)) {
        திரும்பு பொய்;
    }
    எனில் (minimumCapacity < 0) {
        status = U_ILLEGAL_ARGUMENT_ERROR;
        திரும்பு பொய்;
    }
    எனில் (capacity < minimumCapacity) {
        எனில் (capacity > (INT32_MAX - 1) / 2) {        	// integer overflow check
            status = U_ILLEGAL_ARGUMENT_ERROR;
            திரும்பு பொய்;
        }
        int32_t newCap = capacity * 2;
        எனில் (newCap < minimumCapacity) {
            newCap = minimumCapacity;
        }
        எனில் (newCap > static_cast<int32_t>(INT32_MAX / அளவு(UElement))) { // integer overflow check
            // We keep the original memory contents on bad minimumCapacity.
            status = U_ILLEGAL_ARGUMENT_ERROR;
            திரும்பு பொய்;
        }
        UElement* newElems = static_cast<UElement*>(uprv_realloc(elements, அளவு(UElement) * newCap));
        எனில் (newElems == nullptr) {
            // We keep the original contents on the memory failure on realloc or bad minimumCapacity.
            status = U_MEMORY_ALLOCATION_ERROR;
            திரும்பு பொய்;
        }
        elements = newElems;
        capacity = newCap;
    }
    திரும்பு உண்மை;
}

/**
 * Change the size of this vector as follows: If newSize is smaller,
 * then truncate the array, possibly deleting held elements for i >=
 * newSize.  If newSize is larger, grow the array, filling in new
 * slots with nullptr.
 */
வெறுமை UVector::setSize(int32_t newSize, UErrorCode &status) {
    எனில் (!ensureCapacity(newSize, status)) {
        திரும்பு;
    }
    எனில் (newSize > count) {
        UElement empty;
        empty.pointer = nullptr;
        empty.integer = 0;
        க்கு (int32_t i=count; i<newSize; ++i) {
            elements[i] = empty;
        }
    } இல்லையெனில் {
        /* Most efficient to count down */
        க்கு (int32_t i=count-1; i>=newSize; --i) {
            removeElementAt(i);
        }
    }
    count = newSize;
}

/**
 * Fill in the given array with all elements of this vector.
 */
வெறுமை** UVector::toArray(வெறுமை** result) மாறிலி {
    வெறுமை** a = result;
    க்கு (முழு i=0; i<count; ++i) {
        *a++ = elements[i].pointer;
    }
    திரும்பு result;
}

UObjectDeleter *UVector::setDeleter(UObjectDeleter *d) {
    UObjectDeleter *old = deleter;
    deleter = d;
    திரும்பு old;
}

UElementsAreEqual *UVector::setComparer(UElementsAreEqual *d) {
    UElementsAreEqual *old = comparer;
    comparer = d;
    திரும்பு old;
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
வெறுமை* UVector::orphanElementAt(int32_t index) {
    வெறுமை* e = nullptr;
    எனில் (0 <= index && index < count) {
        e = elements[index].pointer;
        க்கு (int32_t i=index; i<count-1; ++i) {
            elements[i] = elements[i+1];
        }
        --count;
    }
    /* else index out of range */
    திரும்பு e;
}

/**
 * Insert the given object into this vector at its sorted position
 * as defined by 'compare'.  The current elements are assumed to
 * be sorted already.
 */
வெறுமை UVector::sorted_செருகு(வெறுமை* obj, UElementComparator *compare, UErrorCode& ec) {
    UElement e;
    e.pointer = obj;
    sorted_செருகு(e, compare, ec);
}

/**
 * Insert the given integer into this vector at its sorted position
 * as defined by 'compare'.  The current elements are assumed to
 * be sorted already.
 */
வெறுமை UVector::sorted_செருகு(int32_t obj, UElementComparator *compare, UErrorCode& ec) {
    U_ASSERT(deleter == nullptr);
    UElement e {};
    e.integer = obj;
    sorted_செருகு(e, compare, ec);
}

// ASSUME elements[] IS CURRENTLY SORTED
வெறுமை UVector::sorted_செருகு(UElement e, UElementComparator *compare, UErrorCode& ec) {
    // Perform a binary search for the location to insert tok at.  Tok
    // will be inserted between two elements a and b such that a <=
    // tok && tok < b, where there is a 'virtual' elements[-1] always
    // less than tok and a 'virtual' elements[count] always greater
    // than tok.
    எனில் (!ensureCapacity(count + 1, ec)) {
        எனில் (deleter != nullptr) {
            (*deleter)(e.pointer);
        }
        திரும்பு;
    }
    int32_t min = 0, max = count;
    போது (min != max) {
        int32_t probe = (min + max) / 2;
        int32_t c = (*compare)(elements[probe], e);
        எனில் (c > 0) {
            max = probe;
        } இல்லையெனில் {
            // assert(c <= 0);
            min = probe + 1;
        }
    }
    க்கு (int32_t i=count; i>min; --i) {
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
நிலையான int32_t U_CALLCONV
sortComparator(மாறிலி வெறுமை *context, மாறிலி வெறுமை *left, மாறிலி வெறுமை *வலது) {
    UElementComparator *compare = *static_cast<UElementComparator * மாறிலி *>(context);
    UElement e1 = *static_cast<மாறிலி UElement *>(left);
    UElement e2 = *static_cast<மாறிலி UElement *>(வலது);
    int32_t result = (*compare)(e1, e2);
    திரும்பு result;
}


/**
  *  Array sort comparison function for use from UVector::sorti()
  *  Compares int32_t vector elements.
  */
நிலையான int32_t U_CALLCONV
sortiComparator(மாறிலி வெறுமை * /*context */, மாறிலி வெறுமை *left, மாறிலி வெறுமை *வலது) {
    மாறிலி UElement *e1 = static_cast<மாறிலி UElement *>(left);
    மாறிலி UElement *e2 = static_cast<மாறிலி UElement *>(வலது);
    int32_t result = e1->integer < e2->integer? -1 :
                     e1->integer == e2->integer? 0 : 1;
    திரும்பு result;
}

/**
  * Sort the vector, assuming it contains ints.
  *     (A more general sort would take a comparison function, but it's
  *     not clear whether UVector's UElementComparator or
  *     UComparator from uprv_sortAray would be more appropriate.)
  */
வெறுமை UVector::sorti(UErrorCode &ec) {
    எனில் (U_SUCCESS(ec)) {
        uprv_sortArray(elements, count, அளவு(UElement),
                       sortiComparator, nullptr,  பொய், &ec);
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
வெறுமை UVector::sort(UElementComparator *compare, UErrorCode &ec) {
    எனில் (U_SUCCESS(ec)) {
        uprv_sortArray(elements, count, அளவு(UElement),
                       sortComparator, &compare, பொய், &ec);
    }
}


/**
 *  Stable sort with a user supplied comparator of type UComparator.
 */
வெறுமை UVector::sortWithUComparator(UComparator *compare, மாறிலி வெறுமை *context, UErrorCode &ec) {
    எனில் (U_SUCCESS(ec)) {
        uprv_sortArray(elements, count, அளவு(UElement),
                       compare, context, உண்மை, &ec);
    }
}

U_NAMESPACE_END

