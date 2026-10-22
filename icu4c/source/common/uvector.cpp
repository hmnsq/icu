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

constexpr int32_t DEFAULT_صلاحيت = 8;

/*
 * Constants for hinting whether a key is an integer
 * or a pointer.  If a hint bit is zero, then the associated
 * token is assumed to be an integer. This is needed for iSeries
 */
constexpr int8_t HINT_ڪنجي_اشارو = 1;
constexpr int8_t HINT_ڪنجي_INTEGER = 0;
 
UOBJECT_DEFINE_RTTI_IMPLEMENTATION(UVector)

UVector::UVector(UError_ڪوڊ &حالت) :
        UVector(خالي_اشارو, خالي_اشارو, DEFAULT_صلاحيت, حالت) {
}

UVector::UVector(int32_t initial_صلاحيت, UError_ڪوڊ &حالت) :
        UVector(خالي_اشارو, خالي_اشارو, initial_صلاحيت, حالت) {
}

UVector::UVector(UObjectDeleter *d, UElementsAreEqual *c, UError_ڪوڊ &حالت) :
        UVector(d, c, DEFAULT_صلاحيت, حالت) {
}

UVector::UVector(UObjectDeleter *d, UElementsAreEqual *c, int32_t initial_صلاحيت, UError_ڪوڊ &حالت) :
    deleter(d),
    comparer(c)
{
    إذا (U_FAILURE(حالت)) {
        أرجع;
    }
    // Fix bogus initialCapacity values; avoid malloc(0) and integer overflow
    إذا ((initial_صلاحيت < 1) || (initial_صلاحيت > static_cast<int32_t>(INT32_MAX / حجم(UElement)))) {
        initial_صلاحيت = DEFAULT_صلاحيت;
    }
    elements = static_cast<UElement*>(uprv_malloc(حجم(UElement) * initial_صلاحيت));
    إذا (elements == خالي_اشارو) {
        حالت = U_يادگيري_ورهاست_ERROR;
    } وإلا {
        capacity = initial_صلاحيت;
    }
}

UVector::~UVector() {
    removeAllElements();
    uprv_free(elements);
    elements = خالي_اشارو;
}

/**
 * Assign this object to another (make this a copy of 'other').
 * Use the 'assign' function to assign each element.
 */
فراغ UVector::assign(ثابت UVector& other, UElementAssigner *assign, UError_ڪوڊ &ec) {
    إذا (ensure_صلاحيت(other.count, ec)) {
        سيٽ_سائيز(other.count, ec);
        إذا (U_SUCCESS(ec)) {
            لكل (int32_t i=0; i<other.count; ++i) {
                إذا (elements[i].اشارو != خالي_اشارو && deleter != خالي_اشارو) {
                    (*deleter)(elements[i].اشارو);
                }
                (*assign)(&elements[i], &other.elements[i]);
            }
        }
    }
}

// This only does something sensible if this object has a non-null comparer
منطقي UVector::عامل==(ثابت UVector& other) ثابت {
    U_ASSERT(comparer != خالي_اشارو);
    إذا (count != other.count) أرجع خطأ;
    إذا (comparer != خالي_اشارو) {
        // Compare using this object's comparer
        لكل (int32_t i=0; i<count; ++i) {
            إذا (!(*comparer)(elements[i], other.elements[i])) {
                أرجع خطأ;
            }
        }
    }
    أرجع سچ;
}

فراغ UVector::جوڙيو_Element(فراغ* obj, UError_ڪوڊ &حالت) {
    U_ASSERT(deleter == خالي_اشارو);
    إذا (ensure_صلاحيت(count + 1, حالت)) {
        elements[count++].اشارو = obj;
    }
}

فراغ UVector::adoptElement(فراغ* obj, UError_ڪوڊ &حالت) {
    U_ASSERT(deleter != خالي_اشارو);
    إذا (ensure_صلاحيت(count + 1, حالت)) {
        elements[count++].اشارو = obj;
    } وإلا {
        (*deleter)(obj);
    }
}
فراغ UVector::جوڙيو_Element(int32_t elem, UError_ڪوڊ &حالت) {
    U_ASSERT(deleter == خالي_اشارو);  // Usage error. Mixing up ints and pointers.
    إذا (ensure_صلاحيت(count + 1, حالت)) {
        elements[count].اشارو = خالي_اشارو;     // Pointers may be bigger than ints.
        elements[count].integer = elem;
        count++;
    }
}

فراغ UVector::سيٽ_Element_At(فراغ* obj, int32_t اشاريو) {
    إذا (0 <= اشاريو && اشاريو < count) {
        إذا (elements[اشاريو].اشارو != خالي_اشارو && deleter != خالي_اشارو) {
            (*deleter)(elements[اشاريو].اشارو);
        }
        elements[اشاريو].اشارو = obj;
    } وإلا {
        /* index out of range */
        إذا (deleter != خالي_اشارو) {
            (*deleter)(obj);
        }
    }
}

فراغ UVector::سيٽ_Element_At(int32_t elem, int32_t اشاريو) {
    U_ASSERT(deleter == خالي_اشارو);  // Usage error. Mixing up ints and pointers.
    إذا (0 <= اشاريو && اشاريو < count) {
        elements[اشاريو].اشارو = خالي_اشارو;
        elements[اشاريو].integer = elem;
    }
    /* else index out of range */
}

فراغ UVector::وجهو_Element_At(فراغ* obj, int32_t اشاريو, UError_ڪوڊ &حالت) {
    إذا (ensure_صلاحيت(count + 1, حالت)) {
        إذا (0 <= اشاريو && اشاريو <= count) {
            لكل (int32_t i=count; i>اشاريو; --i) {
                elements[i] = elements[i-1];
            }
            elements[اشاريو].اشارو = obj;
            ++count;
        } وإلا {
            /* index out of range */
            حالت = U_ILLEGAL_دليل_ERROR;
        }
    }
    إذا (U_FAILURE(حالت) && deleter != خالي_اشارو) {
        (*deleter)(obj);
    }
}

فراغ UVector::وجهو_Element_At(int32_t elem, int32_t اشاريو, UError_ڪوڊ &حالت) {
    U_ASSERT(deleter == خالي_اشارو);  // Usage error. Mixing up ints and pointers.
    // must have 0 <= index <= count
    إذا (ensure_صلاحيت(count + 1, حالت)) {
        إذا (0 <= اشاريو && اشاريو <= count) {
            لكل (int32_t i=count; i>اشاريو; --i) {
                elements[i] = elements[i-1];
            }
            elements[اشاريو].اشارو = خالي_اشارو;
            elements[اشاريو].integer = elem;
            ++count;
        } وإلا {
            /* index out of range */
            حالت = U_ILLEGAL_دليل_ERROR;
        }
    }
}

فراغ* UVector::elementAt(int32_t اشاريو) ثابت {
    أرجع (0 <= اشاريو && اشاريو < count) ? elements[اشاريو].اشارو : خالي_اشارو;
}

int32_t UVector::elementAti(int32_t اشاريو) ثابت {
    أرجع (0 <= اشاريو && اشاريو < count) ? elements[اشاريو].integer : 0;
}

UBool UVector::شامل_آهي_All(ثابت UVector& other) ثابت {
    لكل (int32_t i=0; i<other.size(); ++i) {
        إذا (اشاريو_جو(other.elements[i]) < 0) {
            أرجع خطأ;
        }
    }
    أرجع سچ;
}

UBool UVector::شامل_آهي_None(ثابت UVector& other) ثابت {
    لكل (int32_t i=0; i<other.size(); ++i) {
        إذا (اشاريو_جو(other.elements[i]) >= 0) {
            أرجع خطأ;
        }
    }
    أرجع سچ;
}

UBool UVector::removeAll(ثابت UVector& other) {
    UBool بدليو = خطأ;
    لكل (int32_t i=0; i<other.size(); ++i) {
        int32_t j = اشاريو_جو(other.elements[i]);
        إذا (j >= 0) {
            removeElementAt(j);
            بدليو = سچ;
        }
    }
    أرجع بدليو;
}

UBool UVector::retainAll(ثابت UVector& other) {
    UBool بدليو = خطأ;
    لكل (int32_t j=size()-1; j>=0; --j) {
        int32_t i = other.اشاريو_جو(elements[j]);
        إذا (i < 0) {
            removeElementAt(j);
            بدليو = سچ;
        }
    }
    أرجع بدليو;
}

فراغ UVector::removeElementAt(int32_t اشاريو) {
    فراغ* e = orphanElementAt(اشاريو);
    إذا (e != خالي_اشارو && deleter != خالي_اشارو) {
        (*deleter)(e);
    }
}

UBool UVector::removeElement(فراغ* obj) {
    int32_t i = اشاريو_جو(obj);
    إذا (i >= 0) {
        removeElementAt(i);
        أرجع سچ;
    }
    أرجع خطأ;
}

فراغ UVector::removeAllElements() {
    إذا (deleter != خالي_اشارو) {
        لكل (int32_t i=0; i<count; ++i) {
            إذا (elements[i].اشارو != خالي_اشارو) {
                (*deleter)(elements[i].اشارو);
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
    إذا (comparer == خالي_اشارو) {
        لكل (i=0; i<count; i++) {
            إذا (elements[i].اشارو != other.elements[i].اشارو) {
                أرجع خطأ;
            }
        }
    } وإلا {
        UElement ڪنجي;
        لكل (i=0; i<count; i++) {
            ڪنجي.اشارو = &other.elements[i];
            إذا (!(*comparer)(ڪنجي, elements[i])) {
                أرجع خطأ;
            }
        }
    }
    أرجع سچ;
}



int32_t UVector::اشاريو_جو(فراغ* obj, int32_t شروعات_اشاريو) ثابت {
    UElement ڪنجي;
    ڪنجي.اشارو = obj;
    أرجع اشاريو_جو(ڪنجي, شروعات_اشاريو, HINT_ڪنجي_اشارو);
}

int32_t UVector::اشاريو_جو(int32_t obj, int32_t شروعات_اشاريو) ثابت {
    UElement ڪنجي;
    ڪنجي.integer = obj;
    أرجع اشاريو_جو(ڪنجي, شروعات_اشاريو, HINT_ڪنجي_INTEGER);
}

int32_t UVector::اشاريو_جو(UElement ڪنجي, int32_t شروعات_اشاريو, int8_t hint) ثابت {
    إذا (comparer != خالي_اشارو) {
        لكل (int32_t i=شروعات_اشاريو; i<count; ++i) {
            إذا ((*comparer)(ڪنجي, elements[i])) {
                أرجع i;
            }
        }
    } وإلا {
        لكل (int32_t i=شروعات_اشاريو; i<count; ++i) {
            /* Pointers are not always the same size as ints so to perform
             * a valid comparison we need to know whether we are being
             * provided an int or a pointer. */
            إذا (hint & HINT_ڪنجي_اشارو) {
                إذا (ڪنجي.اشارو == elements[i].اشارو) {
                    أرجع i;
                }
            } وإلا {
                إذا (ڪنجي.integer == elements[i].integer) {
                    أرجع i;
                }
            }
        }
    }
    أرجع -1;
}

UBool UVector::ensure_صلاحيت(int32_t minimum_صلاحيت, UError_ڪوڊ &حالت) {
    إذا (U_FAILURE(حالت)) {
        أرجع خطأ;
    }
    إذا (minimum_صلاحيت < 0) {
        حالت = U_ILLEGAL_دليل_ERROR;
        أرجع خطأ;
    }
    إذا (capacity < minimum_صلاحيت) {
        إذا (capacity > (INT32_MAX - 1) / 2) {        	// integer overflow check
            حالت = U_ILLEGAL_دليل_ERROR;
            أرجع خطأ;
        }
        int32_t newCap = capacity * 2;
        إذا (newCap < minimum_صلاحيت) {
            newCap = minimum_صلاحيت;
        }
        إذا (newCap > static_cast<int32_t>(INT32_MAX / حجم(UElement))) { // integer overflow check
            // We keep the original memory contents on bad minimumCapacity.
            حالت = U_ILLEGAL_دليل_ERROR;
            أرجع خطأ;
        }
        UElement* newElems = static_cast<UElement*>(uprv_realloc(elements, حجم(UElement) * newCap));
        إذا (newElems == خالي_اشارو) {
            // We keep the original contents on the memory failure on realloc or bad minimumCapacity.
            حالت = U_يادگيري_ورهاست_ERROR;
            أرجع خطأ;
        }
        elements = newElems;
        capacity = newCap;
    }
    أرجع سچ;
}

/**
 * Change the size of this vector as follows: If newSize is smaller,
 * then truncate the array, possibly deleting held elements for i >=
 * newSize.  If newSize is larger, grow the array, filling in new
 * slots with nullptr.
 */
فراغ UVector::سيٽ_سائيز(int32_t new_سائيز, UError_ڪوڊ &حالت) {
    إذا (!ensure_صلاحيت(new_سائيز, حالت)) {
        أرجع;
    }
    إذا (new_سائيز > count) {
        UElement empty;
        empty.اشارو = خالي_اشارو;
        empty.integer = 0;
        لكل (int32_t i=count; i<new_سائيز; ++i) {
            elements[i] = empty;
        }
    } وإلا {
        /* Most efficient to count down */
        لكل (int32_t i=count-1; i>=new_سائيز; --i) {
            removeElementAt(i);
        }
    }
    count = new_سائيز;
}

/**
 * Fill in the given array with all elements of this vector.
 */
فراغ** UVector::منزل_Array(فراغ** نتيجو) ثابت {
    فراغ** a = نتيجو;
    لكل (صحيح i=0; i<count; ++i) {
        *a++ = elements[i].اشارو;
    }
    أرجع نتيجو;
}

UObjectDeleter *UVector::سيٽ_Deleter(UObjectDeleter *d) {
    UObjectDeleter *old = deleter;
    deleter = d;
    أرجع old;
}

UElementsAreEqual *UVector::سيٽ_Comparer(UElementsAreEqual *d) {
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
فراغ* UVector::orphanElementAt(int32_t اشاريو) {
    فراغ* e = خالي_اشارو;
    إذا (0 <= اشاريو && اشاريو < count) {
        e = elements[اشاريو].اشارو;
        لكل (int32_t i=اشاريو; i<count-1; ++i) {
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
فراغ UVector::sorted_وجهو(فراغ* obj, UElementComparator *compare, UError_ڪوڊ& ec) {
    UElement e;
    e.اشارو = obj;
    sorted_وجهو(e, compare, ec);
}

/**
 * Insert the given integer into this vector at its sorted position
 * as defined by 'compare'.  The current elements are assumed to
 * be sorted already.
 */
فراغ UVector::sorted_وجهو(int32_t obj, UElementComparator *compare, UError_ڪوڊ& ec) {
    U_ASSERT(deleter == خالي_اشارو);
    UElement e {};
    e.integer = obj;
    sorted_وجهو(e, compare, ec);
}

// ASSUME elements[] IS CURRENTLY SORTED
فراغ UVector::sorted_وجهو(UElement e, UElementComparator *compare, UError_ڪوڊ& ec) {
    // Perform a binary search for the location to insert tok at.  Tok
    // will be inserted between two elements a and b such that a <=
    // tok && tok < b, where there is a 'virtual' elements[-1] always
    // less than tok and a 'virtual' elements[count] always greater
    // than tok.
    إذا (!ensure_صلاحيت(count + 1, ec)) {
        إذا (deleter != خالي_اشارو) {
            (*deleter)(e.اشارو);
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
ترتيب_ڏيو_Comparator(ثابت فراغ *context, ثابت فراغ *کاٻي, ثابت فراغ *ساڄي) {
    UElementComparator *compare = *static_cast<UElementComparator * ثابت *>(context);
    UElement e1 = *static_cast<ثابت UElement *>(کاٻي);
    UElement e2 = *static_cast<ثابت UElement *>(ساڄي);
    int32_t نتيجو = (*compare)(e1, e2);
    أرجع نتيجو;
}


/**
  *  Array sort comparison function for use from UVector::sorti()
  *  Compares int32_t vector elements.
  */
ساكن int32_t U_CALLCONV
sortiComparator(ثابت فراغ * /*context */, ثابت فراغ *کاٻي, ثابت فراغ *ساڄي) {
    ثابت UElement *e1 = static_cast<ثابت UElement *>(کاٻي);
    ثابت UElement *e2 = static_cast<ثابت UElement *>(ساڄي);
    int32_t نتيجو = e1->integer < e2->integer? -1 :
                     e1->integer == e2->integer? 0 : 1;
    أرجع نتيجو;
}

/**
  * Sort the vector, assuming it contains ints.
  *     (A more general sort would take a comparison function, but it's
  *     not clear whether UVector's UElementComparator or
  *     UComparator from uprv_sortAray would be more appropriate.)
  */
فراغ UVector::sorti(UError_ڪوڊ &ec) {
    إذا (U_SUCCESS(ec)) {
        uprv_ترتيب_ڏيو_Array(elements, count, حجم(UElement),
                       sortiComparator, خالي_اشارو,  خطأ, &ec);
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
فراغ UVector::sort(UElementComparator *compare, UError_ڪوڊ &ec) {
    إذا (U_SUCCESS(ec)) {
        uprv_ترتيب_ڏيو_Array(elements, count, حجم(UElement),
                       ترتيب_ڏيو_Comparator, &compare, خطأ, &ec);
    }
}


/**
 *  Stable sort with a user supplied comparator of type UComparator.
 */
فراغ UVector::ترتيب_ڏيو_With_UComparator(UComparator *compare, ثابت فراغ *context, UError_ڪوڊ &ec) {
    إذا (U_SUCCESS(ec)) {
        uprv_ترتيب_ڏيو_Array(elements, count, حجم(UElement),
                       compare, context, سچ, &ec);
    }
}

U_NAMESPACE_پڇاڙي

