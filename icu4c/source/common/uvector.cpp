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

#અન્તર્ભાવ "uvector.h"
#અન્તર્ભાવ "cmemory.h"
#અન્તર્ભાવ "uarrsort.h"
#અન્તર્ભાવ "uelement.h"

U_NAMESPACE_BEGIN

constexpr int32_t DEFAULT_CAPACITY = 8;

/*
 * Constants for hinting whether a key is an integer
 * or a pointer.  If a hint bit is zero, then the associated
 * token is assumed to be an integer. This is needed for iSeries
 */
constexpr int8_t HINT_ચાવી_POINTER = 1;
constexpr int8_t HINT_ચાવી_INTEGER = 0;
 
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
    જો (U_FAILURE(status)) {
        પાછા;
    }
    // Fix bogus initialCapacity values; avoid malloc(0) and integer overflow
    જો ((initialCapacity < 1) || (initialCapacity > static_cast<int32_t>(INT32_MAX / કદ(UElement)))) {
        initialCapacity = DEFAULT_CAPACITY;
    }
    elements = static_cast<UElement*>(uprv_malloc(કદ(UElement) * initialCapacity));
    જો (elements == nullptr) {
        status = U_MEMORY_ALLOCATION_ERROR;
    } નહીંતર {
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
શૂન્ય UVector::assign(અચલ UVector& other, UElementAssigner *assign, UErrorCode &ec) {
    જો (ensureCapacity(other.count, ec)) {
        setSize(other.count, ec);
        જો (U_SUCCESS(ec)) {
            માટે (int32_t i=0; i<other.count; ++i) {
                જો (elements[i].pointer != nullptr && deleter != nullptr) {
                    (*deleter)(elements[i].pointer);
                }
                (*assign)(&elements[i], &other.elements[i]);
            }
        }
    }
}

// This only does something sensible if this object has a non-null comparer
તાર્કિક UVector::operator==(અચલ UVector& other) અચલ {
    U_ASSERT(comparer != nullptr);
    જો (count != other.count) પાછા ખોટું;
    જો (comparer != nullptr) {
        // Compare using this object's comparer
        માટે (int32_t i=0; i<count; ++i) {
            જો (!(*comparer)(elements[i], other.elements[i])) {
                પાછા ખોટું;
            }
        }
    }
    પાછા સાચું;
}

શૂન્ય UVector::addElement(શૂન્ય* obj, UErrorCode &status) {
    U_ASSERT(deleter == nullptr);
    જો (ensureCapacity(count + 1, status)) {
        elements[count++].pointer = obj;
    }
}

શૂન્ય UVector::adoptElement(શૂન્ય* obj, UErrorCode &status) {
    U_ASSERT(deleter != nullptr);
    જો (ensureCapacity(count + 1, status)) {
        elements[count++].pointer = obj;
    } નહીંતર {
        (*deleter)(obj);
    }
}
શૂન્ય UVector::addElement(int32_t elem, UErrorCode &status) {
    U_ASSERT(deleter == nullptr);  // Usage error. Mixing up ints and pointers.
    જો (ensureCapacity(count + 1, status)) {
        elements[count].pointer = nullptr;     // Pointers may be bigger than ints.
        elements[count].integer = elem;
        count++;
    }
}

શૂન્ય UVector::setElementAt(શૂન્ય* obj, int32_t index) {
    જો (0 <= index && index < count) {
        જો (elements[index].pointer != nullptr && deleter != nullptr) {
            (*deleter)(elements[index].pointer);
        }
        elements[index].pointer = obj;
    } નહીંતર {
        /* index out of range */
        જો (deleter != nullptr) {
            (*deleter)(obj);
        }
    }
}

શૂન્ય UVector::setElementAt(int32_t elem, int32_t index) {
    U_ASSERT(deleter == nullptr);  // Usage error. Mixing up ints and pointers.
    જો (0 <= index && index < count) {
        elements[index].pointer = nullptr;
        elements[index].integer = elem;
    }
    /* else index out of range */
}

શૂન્ય UVector::ઉમેરો_Element_At(શૂન્ય* obj, int32_t index, UErrorCode &status) {
    જો (ensureCapacity(count + 1, status)) {
        જો (0 <= index && index <= count) {
            માટે (int32_t i=count; i>index; --i) {
                elements[i] = elements[i-1];
            }
            elements[index].pointer = obj;
            ++count;
        } નહીંતર {
            /* index out of range */
            status = U_ILLEGAL_ARGUMENT_ERROR;
        }
    }
    જો (U_FAILURE(status) && deleter != nullptr) {
        (*deleter)(obj);
    }
}

શૂન્ય UVector::ઉમેરો_Element_At(int32_t elem, int32_t index, UErrorCode &status) {
    U_ASSERT(deleter == nullptr);  // Usage error. Mixing up ints and pointers.
    // must have 0 <= index <= count
    જો (ensureCapacity(count + 1, status)) {
        જો (0 <= index && index <= count) {
            માટે (int32_t i=count; i>index; --i) {
                elements[i] = elements[i-1];
            }
            elements[index].pointer = nullptr;
            elements[index].integer = elem;
            ++count;
        } નહીંતર {
            /* index out of range */
            status = U_ILLEGAL_ARGUMENT_ERROR;
        }
    }
}

શૂન્ય* UVector::elementAt(int32_t index) અચલ {
    પાછા (0 <= index && index < count) ? elements[index].pointer : nullptr;
}

int32_t UVector::elementAti(int32_t index) અચલ {
    પાછા (0 <= index && index < count) ? elements[index].integer : 0;
}

UBool UVector::containsAll(અચલ UVector& other) અચલ {
    માટે (int32_t i=0; i<other.size(); ++i) {
        જો (indexOf(other.elements[i]) < 0) {
            પાછા ખોટું;
        }
    }
    પાછા સાચું;
}

UBool UVector::containsNone(અચલ UVector& other) અચલ {
    માટે (int32_t i=0; i<other.size(); ++i) {
        જો (indexOf(other.elements[i]) >= 0) {
            પાછા ખોટું;
        }
    }
    પાછા સાચું;
}

UBool UVector::removeAll(અચલ UVector& other) {
    UBool changed = ખોટું;
    માટે (int32_t i=0; i<other.size(); ++i) {
        int32_t j = indexOf(other.elements[i]);
        જો (j >= 0) {
            removeElementAt(j);
            changed = સાચું;
        }
    }
    પાછા changed;
}

UBool UVector::retainAll(અચલ UVector& other) {
    UBool changed = ખોટું;
    માટે (int32_t j=size()-1; j>=0; --j) {
        int32_t i = other.indexOf(elements[j]);
        જો (i < 0) {
            removeElementAt(j);
            changed = સાચું;
        }
    }
    પાછા changed;
}

શૂન્ય UVector::removeElementAt(int32_t index) {
    શૂન્ય* e = orphanElementAt(index);
    જો (e != nullptr && deleter != nullptr) {
        (*deleter)(e);
    }
}

UBool UVector::removeElement(શૂન્ય* obj) {
    int32_t i = indexOf(obj);
    જો (i >= 0) {
        removeElementAt(i);
        પાછા સાચું;
    }
    પાછા ખોટું;
}

શૂન્ય UVector::removeAllElements() {
    જો (deleter != nullptr) {
        માટે (int32_t i=0; i<count; ++i) {
            જો (elements[i].pointer != nullptr) {
                (*deleter)(elements[i].pointer);
            }
        }
    }
    count = 0;
}

UBool   UVector::equals(અચલ UVector &other) અચલ {
    પૂર્ણાંક      i;

    જો (આ->count != other.count) {
        પાછા ખોટું;
    }
    જો (comparer == nullptr) {
        માટે (i=0; i<count; i++) {
            જો (elements[i].pointer != other.elements[i].pointer) {
                પાછા ખોટું;
            }
        }
    } નહીંતર {
        UElement ચાવી;
        માટે (i=0; i<count; i++) {
            ચાવી.pointer = &other.elements[i];
            જો (!(*comparer)(ચાવી, elements[i])) {
                પાછા ખોટું;
            }
        }
    }
    પાછા સાચું;
}



int32_t UVector::indexOf(શૂન્ય* obj, int32_t startIndex) અચલ {
    UElement ચાવી;
    ચાવી.pointer = obj;
    પાછા indexOf(ચાવી, startIndex, HINT_ચાવી_POINTER);
}

int32_t UVector::indexOf(int32_t obj, int32_t startIndex) અચલ {
    UElement ચાવી;
    ચાવી.integer = obj;
    પાછા indexOf(ચાવી, startIndex, HINT_ચાવી_INTEGER);
}

int32_t UVector::indexOf(UElement ચાવી, int32_t startIndex, int8_t hint) અચલ {
    જો (comparer != nullptr) {
        માટે (int32_t i=startIndex; i<count; ++i) {
            જો ((*comparer)(ચાવી, elements[i])) {
                પાછા i;
            }
        }
    } નહીંતર {
        માટે (int32_t i=startIndex; i<count; ++i) {
            /* Pointers are not always the same size as ints so to perform
             * a valid comparison we need to know whether we are being
             * provided an int or a pointer. */
            જો (hint & HINT_ચાવી_POINTER) {
                જો (ચાવી.pointer == elements[i].pointer) {
                    પાછા i;
                }
            } નહીંતર {
                જો (ચાવી.integer == elements[i].integer) {
                    પાછા i;
                }
            }
        }
    }
    પાછા -1;
}

UBool UVector::ensureCapacity(int32_t minimumCapacity, UErrorCode &status) {
    જો (U_FAILURE(status)) {
        પાછા ખોટું;
    }
    જો (minimumCapacity < 0) {
        status = U_ILLEGAL_ARGUMENT_ERROR;
        પાછા ખોટું;
    }
    જો (capacity < minimumCapacity) {
        જો (capacity > (INT32_MAX - 1) / 2) {        	// integer overflow check
            status = U_ILLEGAL_ARGUMENT_ERROR;
            પાછા ખોટું;
        }
        int32_t newCap = capacity * 2;
        જો (newCap < minimumCapacity) {
            newCap = minimumCapacity;
        }
        જો (newCap > static_cast<int32_t>(INT32_MAX / કદ(UElement))) { // integer overflow check
            // We keep the original memory contents on bad minimumCapacity.
            status = U_ILLEGAL_ARGUMENT_ERROR;
            પાછા ખોટું;
        }
        UElement* newElems = static_cast<UElement*>(uprv_realloc(elements, કદ(UElement) * newCap));
        જો (newElems == nullptr) {
            // We keep the original contents on the memory failure on realloc or bad minimumCapacity.
            status = U_MEMORY_ALLOCATION_ERROR;
            પાછા ખોટું;
        }
        elements = newElems;
        capacity = newCap;
    }
    પાછા સાચું;
}

/**
 * Change the size of this vector as follows: If newSize is smaller,
 * then truncate the array, possibly deleting held elements for i >=
 * newSize.  If newSize is larger, grow the array, filling in new
 * slots with nullptr.
 */
શૂન્ય UVector::setSize(int32_t newSize, UErrorCode &status) {
    જો (!ensureCapacity(newSize, status)) {
        પાછા;
    }
    જો (newSize > count) {
        UElement empty;
        empty.pointer = nullptr;
        empty.integer = 0;
        માટે (int32_t i=count; i<newSize; ++i) {
            elements[i] = empty;
        }
    } નહીંતર {
        /* Most efficient to count down */
        માટે (int32_t i=count-1; i>=newSize; --i) {
            removeElementAt(i);
        }
    }
    count = newSize;
}

/**
 * Fill in the given array with all elements of this vector.
 */
શૂન્ય** UVector::toArray(શૂન્ય** result) અચલ {
    શૂન્ય** a = result;
    માટે (પૂર્ણાંક i=0; i<count; ++i) {
        *a++ = elements[i].pointer;
    }
    પાછા result;
}

UObjectDeleter *UVector::setDeleter(UObjectDeleter *d) {
    UObjectDeleter *old = deleter;
    deleter = d;
    પાછા old;
}

UElementsAreEqual *UVector::setComparer(UElementsAreEqual *d) {
    UElementsAreEqual *old = comparer;
    comparer = d;
    પાછા old;
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
શૂન્ય* UVector::orphanElementAt(int32_t index) {
    શૂન્ય* e = nullptr;
    જો (0 <= index && index < count) {
        e = elements[index].pointer;
        માટે (int32_t i=index; i<count-1; ++i) {
            elements[i] = elements[i+1];
        }
        --count;
    }
    /* else index out of range */
    પાછા e;
}

/**
 * Insert the given object into this vector at its sorted position
 * as defined by 'compare'.  The current elements are assumed to
 * be sorted already.
 */
શૂન્ય UVector::sorted_ઉમેરો(શૂન્ય* obj, UElementComparator *compare, UErrorCode& ec) {
    UElement e;
    e.pointer = obj;
    sorted_ઉમેરો(e, compare, ec);
}

/**
 * Insert the given integer into this vector at its sorted position
 * as defined by 'compare'.  The current elements are assumed to
 * be sorted already.
 */
શૂન્ય UVector::sorted_ઉમેરો(int32_t obj, UElementComparator *compare, UErrorCode& ec) {
    U_ASSERT(deleter == nullptr);
    UElement e {};
    e.integer = obj;
    sorted_ઉમેરો(e, compare, ec);
}

// ASSUME elements[] IS CURRENTLY SORTED
શૂન્ય UVector::sorted_ઉમેરો(UElement e, UElementComparator *compare, UErrorCode& ec) {
    // Perform a binary search for the location to insert tok at.  Tok
    // will be inserted between two elements a and b such that a <=
    // tok && tok < b, where there is a 'virtual' elements[-1] always
    // less than tok and a 'virtual' elements[count] always greater
    // than tok.
    જો (!ensureCapacity(count + 1, ec)) {
        જો (deleter != nullptr) {
            (*deleter)(e.pointer);
        }
        પાછા;
    }
    int32_t min = 0, max = count;
    જ્યારે (min != max) {
        int32_t probe = (min + max) / 2;
        int32_t c = (*compare)(elements[probe], e);
        જો (c > 0) {
            max = probe;
        } નહીંતર {
            // assert(c <= 0);
            min = probe + 1;
        }
    }
    માટે (int32_t i=count; i>min; --i) {
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
સ્થિર int32_t U_CALLCONV
sortComparator(અચલ શૂન્ય *context, અચલ શૂન્ય *left, અચલ શૂન્ય *જમણું) {
    UElementComparator *compare = *static_cast<UElementComparator * અચલ *>(context);
    UElement e1 = *static_cast<અચલ UElement *>(left);
    UElement e2 = *static_cast<અચલ UElement *>(જમણું);
    int32_t result = (*compare)(e1, e2);
    પાછા result;
}


/**
  *  Array sort comparison function for use from UVector::sorti()
  *  Compares int32_t vector elements.
  */
સ્થિર int32_t U_CALLCONV
sortiComparator(અચલ શૂન્ય * /*context */, અચલ શૂન્ય *left, અચલ શૂન્ય *જમણું) {
    અચલ UElement *e1 = static_cast<અચલ UElement *>(left);
    અચલ UElement *e2 = static_cast<અચલ UElement *>(જમણું);
    int32_t result = e1->integer < e2->integer? -1 :
                     e1->integer == e2->integer? 0 : 1;
    પાછા result;
}

/**
  * Sort the vector, assuming it contains ints.
  *     (A more general sort would take a comparison function, but it's
  *     not clear whether UVector's UElementComparator or
  *     UComparator from uprv_sortAray would be more appropriate.)
  */
શૂન્ય UVector::sorti(UErrorCode &ec) {
    જો (U_SUCCESS(ec)) {
        uprv_sortArray(elements, count, કદ(UElement),
                       sortiComparator, nullptr,  ખોટું, &ec);
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
શૂન્ય UVector::sort(UElementComparator *compare, UErrorCode &ec) {
    જો (U_SUCCESS(ec)) {
        uprv_sortArray(elements, count, કદ(UElement),
                       sortComparator, &compare, ખોટું, &ec);
    }
}


/**
 *  Stable sort with a user supplied comparator of type UComparator.
 */
શૂન્ય UVector::sortWithUComparator(UComparator *compare, અચલ શૂન્ય *context, UErrorCode &ec) {
    જો (U_SUCCESS(ec)) {
        uprv_sortArray(elements, count, કદ(UElement),
                       compare, context, સાચું, &ec);
    }
}

U_NAMESPACE_END

