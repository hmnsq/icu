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

constexpr int32_t DEFAULT_ظرفیت = 8;

/*
 * Constants for hinting whether a key is an integer
 * or a pointer.  If a hint bit is zero, then the associated
 * token is assumed to be an integer. This is needed for iSeries
 */
constexpr int8_t HINT_کلید_اشاره_گر = 1;
constexpr int8_t HINT_کلید_INTEGER = 0;
 
UOBJECT_DEFINE_RTTI_IMPLEMENTATION(UVector)

UVector::UVector(UError_کد &وضعیت) :
        UVector(اشاره_تهی, اشاره_تهی, DEFAULT_ظرفیت, وضعیت) {
}

UVector::UVector(int32_t initial_ظرفیت, UError_کد &وضعیت) :
        UVector(اشاره_تهی, اشاره_تهی, initial_ظرفیت, وضعیت) {
}

UVector::UVector(UObjectDeleter *d, UElementsAreEqual *c, UError_کد &وضعیت) :
        UVector(d, c, DEFAULT_ظرفیت, وضعیت) {
}

UVector::UVector(UObjectDeleter *d, UElementsAreEqual *c, int32_t initial_ظرفیت, UError_کد &وضعیت) :
    deleter(d),
    comparer(c)
{
    إذا (U_FAILURE(وضعیت)) {
        أرجع;
    }
    // Fix bogus initialCapacity values; avoid malloc(0) and integer overflow
    إذا ((initial_ظرفیت < 1) || (initial_ظرفیت > static_cast<int32_t>(INT32_MAX / حجم(UElement)))) {
        initial_ظرفیت = DEFAULT_ظرفیت;
    }
    elements = static_cast<UElement*>(uprv_malloc(حجم(UElement) * initial_ظرفیت));
    إذا (elements == اشاره_تهی) {
        وضعیت = U_حافظه_تخصیص_ERROR;
    } وإلا {
        capacity = initial_ظرفیت;
    }
}

UVector::~UVector() {
    removeAllElements();
    uprv_free(elements);
    elements = اشاره_تهی;
}

/**
 * Assign this object to another (make this a copy of 'other').
 * Use the 'assign' function to assign each element.
 */
فراغ UVector::assign(ثابت UVector& other, UElementAssigner *assign, UError_کد &ec) {
    إذا (ensure_ظرفیت(other.count, ec)) {
        مجموعه_اندازه(other.count, ec);
        إذا (U_SUCCESS(ec)) {
            لكل (int32_t i=0; i<other.count; ++i) {
                إذا (elements[i].اشاره_گر != اشاره_تهی && deleter != اشاره_تهی) {
                    (*deleter)(elements[i].اشاره_گر);
                }
                (*assign)(&elements[i], &other.elements[i]);
            }
        }
    }
}

// This only does something sensible if this object has a non-null comparer
منطقي UVector::عملگر==(ثابت UVector& other) ثابت {
    U_ASSERT(comparer != اشاره_تهی);
    إذا (count != other.count) أرجع خطأ;
    إذا (comparer != اشاره_تهی) {
        // Compare using this object's comparer
        لكل (int32_t i=0; i<count; ++i) {
            إذا (!(*comparer)(elements[i], other.elements[i])) {
                أرجع خطأ;
            }
        }
    }
    أرجع درست;
}

فراغ UVector::جمع_کن_Element(فراغ* obj, UError_کد &وضعیت) {
    U_ASSERT(deleter == اشاره_تهی);
    إذا (ensure_ظرفیت(count + 1, وضعیت)) {
        elements[count++].اشاره_گر = obj;
    }
}

فراغ UVector::adoptElement(فراغ* obj, UError_کد &وضعیت) {
    U_ASSERT(deleter != اشاره_تهی);
    إذا (ensure_ظرفیت(count + 1, وضعیت)) {
        elements[count++].اشاره_گر = obj;
    } وإلا {
        (*deleter)(obj);
    }
}
فراغ UVector::جمع_کن_Element(int32_t elem, UError_کد &وضعیت) {
    U_ASSERT(deleter == اشاره_تهی);  // Usage error. Mixing up ints and pointers.
    إذا (ensure_ظرفیت(count + 1, وضعیت)) {
        elements[count].اشاره_گر = اشاره_تهی;     // Pointers may be bigger than ints.
        elements[count].integer = elem;
        count++;
    }
}

فراغ UVector::مجموعه_Element_At(فراغ* obj, int32_t شاخص) {
    إذا (0 <= شاخص && شاخص < count) {
        إذا (elements[شاخص].اشاره_گر != اشاره_تهی && deleter != اشاره_تهی) {
            (*deleter)(elements[شاخص].اشاره_گر);
        }
        elements[شاخص].اشاره_گر = obj;
    } وإلا {
        /* index out of range */
        إذا (deleter != اشاره_تهی) {
            (*deleter)(obj);
        }
    }
}

فراغ UVector::مجموعه_Element_At(int32_t elem, int32_t شاخص) {
    U_ASSERT(deleter == اشاره_تهی);  // Usage error. Mixing up ints and pointers.
    إذا (0 <= شاخص && شاخص < count) {
        elements[شاخص].اشاره_گر = اشاره_تهی;
        elements[شاخص].integer = elem;
    }
    /* else index out of range */
}

فراغ UVector::درج_Element_At(فراغ* obj, int32_t شاخص, UError_کد &وضعیت) {
    إذا (ensure_ظرفیت(count + 1, وضعیت)) {
        إذا (0 <= شاخص && شاخص <= count) {
            لكل (int32_t i=count; i>شاخص; --i) {
                elements[i] = elements[i-1];
            }
            elements[شاخص].اشاره_گر = obj;
            ++count;
        } وإلا {
            /* index out of range */
            وضعیت = U_ILLEGAL_آرگومان_ERROR;
        }
    }
    إذا (U_FAILURE(وضعیت) && deleter != اشاره_تهی) {
        (*deleter)(obj);
    }
}

فراغ UVector::درج_Element_At(int32_t elem, int32_t شاخص, UError_کد &وضعیت) {
    U_ASSERT(deleter == اشاره_تهی);  // Usage error. Mixing up ints and pointers.
    // must have 0 <= index <= count
    إذا (ensure_ظرفیت(count + 1, وضعیت)) {
        إذا (0 <= شاخص && شاخص <= count) {
            لكل (int32_t i=count; i>شاخص; --i) {
                elements[i] = elements[i-1];
            }
            elements[شاخص].اشاره_گر = اشاره_تهی;
            elements[شاخص].integer = elem;
            ++count;
        } وإلا {
            /* index out of range */
            وضعیت = U_ILLEGAL_آرگومان_ERROR;
        }
    }
}

فراغ* UVector::elementAt(int32_t شاخص) ثابت {
    أرجع (0 <= شاخص && شاخص < count) ? elements[شاخص].اشاره_گر : اشاره_تهی;
}

int32_t UVector::elementAti(int32_t شاخص) ثابت {
    أرجع (0 <= شاخص && شاخص < count) ? elements[شاخص].integer : 0;
}

UBool UVector::شامل_است_All(ثابت UVector& other) ثابت {
    لكل (int32_t i=0; i<other.size(); ++i) {
        إذا (شاخص_از(other.elements[i]) < 0) {
            أرجع خطأ;
        }
    }
    أرجع درست;
}

UBool UVector::شامل_است_None(ثابت UVector& other) ثابت {
    لكل (int32_t i=0; i<other.size(); ++i) {
        إذا (شاخص_از(other.elements[i]) >= 0) {
            أرجع خطأ;
        }
    }
    أرجع درست;
}

UBool UVector::removeAll(ثابت UVector& other) {
    UBool تغییریافته = خطأ;
    لكل (int32_t i=0; i<other.size(); ++i) {
        int32_t j = شاخص_از(other.elements[i]);
        إذا (j >= 0) {
            removeElementAt(j);
            تغییریافته = درست;
        }
    }
    أرجع تغییریافته;
}

UBool UVector::retainAll(ثابت UVector& other) {
    UBool تغییریافته = خطأ;
    لكل (int32_t j=size()-1; j>=0; --j) {
        int32_t i = other.شاخص_از(elements[j]);
        إذا (i < 0) {
            removeElementAt(j);
            تغییریافته = درست;
        }
    }
    أرجع تغییریافته;
}

فراغ UVector::removeElementAt(int32_t شاخص) {
    فراغ* e = orphanElementAt(شاخص);
    إذا (e != اشاره_تهی && deleter != اشاره_تهی) {
        (*deleter)(e);
    }
}

UBool UVector::removeElement(فراغ* obj) {
    int32_t i = شاخص_از(obj);
    إذا (i >= 0) {
        removeElementAt(i);
        أرجع درست;
    }
    أرجع خطأ;
}

فراغ UVector::removeAllElements() {
    إذا (deleter != اشاره_تهی) {
        لكل (int32_t i=0; i<count; ++i) {
            إذا (elements[i].اشاره_گر != اشاره_تهی) {
                (*deleter)(elements[i].اشاره_گر);
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
    إذا (comparer == اشاره_تهی) {
        لكل (i=0; i<count; i++) {
            إذا (elements[i].اشاره_گر != other.elements[i].اشاره_گر) {
                أرجع خطأ;
            }
        }
    } وإلا {
        UElement کلید;
        لكل (i=0; i<count; i++) {
            کلید.اشاره_گر = &other.elements[i];
            إذا (!(*comparer)(کلید, elements[i])) {
                أرجع خطأ;
            }
        }
    }
    أرجع درست;
}



int32_t UVector::شاخص_از(فراغ* obj, int32_t شروع_شاخص) ثابت {
    UElement کلید;
    کلید.اشاره_گر = obj;
    أرجع شاخص_از(کلید, شروع_شاخص, HINT_کلید_اشاره_گر);
}

int32_t UVector::شاخص_از(int32_t obj, int32_t شروع_شاخص) ثابت {
    UElement کلید;
    کلید.integer = obj;
    أرجع شاخص_از(کلید, شروع_شاخص, HINT_کلید_INTEGER);
}

int32_t UVector::شاخص_از(UElement کلید, int32_t شروع_شاخص, int8_t hint) ثابت {
    إذا (comparer != اشاره_تهی) {
        لكل (int32_t i=شروع_شاخص; i<count; ++i) {
            إذا ((*comparer)(کلید, elements[i])) {
                أرجع i;
            }
        }
    } وإلا {
        لكل (int32_t i=شروع_شاخص; i<count; ++i) {
            /* Pointers are not always the same size as ints so to perform
             * a valid comparison we need to know whether we are being
             * provided an int or a pointer. */
            إذا (hint & HINT_کلید_اشاره_گر) {
                إذا (کلید.اشاره_گر == elements[i].اشاره_گر) {
                    أرجع i;
                }
            } وإلا {
                إذا (کلید.integer == elements[i].integer) {
                    أرجع i;
                }
            }
        }
    }
    أرجع -1;
}

UBool UVector::ensure_ظرفیت(int32_t minimum_ظرفیت, UError_کد &وضعیت) {
    إذا (U_FAILURE(وضعیت)) {
        أرجع خطأ;
    }
    إذا (minimum_ظرفیت < 0) {
        وضعیت = U_ILLEGAL_آرگومان_ERROR;
        أرجع خطأ;
    }
    إذا (capacity < minimum_ظرفیت) {
        إذا (capacity > (INT32_MAX - 1) / 2) {        	// integer overflow check
            وضعیت = U_ILLEGAL_آرگومان_ERROR;
            أرجع خطأ;
        }
        int32_t newCap = capacity * 2;
        إذا (newCap < minimum_ظرفیت) {
            newCap = minimum_ظرفیت;
        }
        إذا (newCap > static_cast<int32_t>(INT32_MAX / حجم(UElement))) { // integer overflow check
            // We keep the original memory contents on bad minimumCapacity.
            وضعیت = U_ILLEGAL_آرگومان_ERROR;
            أرجع خطأ;
        }
        UElement* newElems = static_cast<UElement*>(uprv_realloc(elements, حجم(UElement) * newCap));
        إذا (newElems == اشاره_تهی) {
            // We keep the original contents on the memory failure on realloc or bad minimumCapacity.
            وضعیت = U_حافظه_تخصیص_ERROR;
            أرجع خطأ;
        }
        elements = newElems;
        capacity = newCap;
    }
    أرجع درست;
}

/**
 * Change the size of this vector as follows: If newSize is smaller,
 * then truncate the array, possibly deleting held elements for i >=
 * newSize.  If newSize is larger, grow the array, filling in new
 * slots with nullptr.
 */
فراغ UVector::مجموعه_اندازه(int32_t new_اندازه, UError_کد &وضعیت) {
    إذا (!ensure_ظرفیت(new_اندازه, وضعیت)) {
        أرجع;
    }
    إذا (new_اندازه > count) {
        UElement empty;
        empty.اشاره_گر = اشاره_تهی;
        empty.integer = 0;
        لكل (int32_t i=count; i<new_اندازه; ++i) {
            elements[i] = empty;
        }
    } وإلا {
        /* Most efficient to count down */
        لكل (int32_t i=count-1; i>=new_اندازه; --i) {
            removeElementAt(i);
        }
    }
    count = new_اندازه;
}

/**
 * Fill in the given array with all elements of this vector.
 */
فراغ** UVector::مقصد_Array(فراغ** نتیجه) ثابت {
    فراغ** a = نتیجه;
    لكل (صحيح i=0; i<count; ++i) {
        *a++ = elements[i].اشاره_گر;
    }
    أرجع نتیجه;
}

UObjectDeleter *UVector::مجموعه_Deleter(UObjectDeleter *d) {
    UObjectDeleter *old = deleter;
    deleter = d;
    أرجع old;
}

UElementsAreEqual *UVector::مجموعه_Comparer(UElementsAreEqual *d) {
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
فراغ* UVector::orphanElementAt(int32_t شاخص) {
    فراغ* e = اشاره_تهی;
    إذا (0 <= شاخص && شاخص < count) {
        e = elements[شاخص].اشاره_گر;
        لكل (int32_t i=شاخص; i<count-1; ++i) {
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
فراغ UVector::sorted_درج(فراغ* obj, UElementComparator *compare, UError_کد& ec) {
    UElement e;
    e.اشاره_گر = obj;
    sorted_درج(e, compare, ec);
}

/**
 * Insert the given integer into this vector at its sorted position
 * as defined by 'compare'.  The current elements are assumed to
 * be sorted already.
 */
فراغ UVector::sorted_درج(int32_t obj, UElementComparator *compare, UError_کد& ec) {
    U_ASSERT(deleter == اشاره_تهی);
    UElement e {};
    e.integer = obj;
    sorted_درج(e, compare, ec);
}

// ASSUME elements[] IS CURRENTLY SORTED
فراغ UVector::sorted_درج(UElement e, UElementComparator *compare, UError_کد& ec) {
    // Perform a binary search for the location to insert tok at.  Tok
    // will be inserted between two elements a and b such that a <=
    // tok && tok < b, where there is a 'virtual' elements[-1] always
    // less than tok and a 'virtual' elements[count] always greater
    // than tok.
    إذا (!ensure_ظرفیت(count + 1, ec)) {
        إذا (deleter != اشاره_تهی) {
            (*deleter)(e.اشاره_گر);
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
مرتب_سازی_Comparator(ثابت فراغ *context, ثابت فراغ *چپ, ثابت فراغ *راست) {
    UElementComparator *compare = *static_cast<UElementComparator * ثابت *>(context);
    UElement e1 = *static_cast<ثابت UElement *>(چپ);
    UElement e2 = *static_cast<ثابت UElement *>(راست);
    int32_t نتیجه = (*compare)(e1, e2);
    أرجع نتیجه;
}


/**
  *  Array sort comparison function for use from UVector::sorti()
  *  Compares int32_t vector elements.
  */
ساكن int32_t U_CALLCONV
sortiComparator(ثابت فراغ * /*context */, ثابت فراغ *چپ, ثابت فراغ *راست) {
    ثابت UElement *e1 = static_cast<ثابت UElement *>(چپ);
    ثابت UElement *e2 = static_cast<ثابت UElement *>(راست);
    int32_t نتیجه = e1->integer < e2->integer? -1 :
                     e1->integer == e2->integer? 0 : 1;
    أرجع نتیجه;
}

/**
  * Sort the vector, assuming it contains ints.
  *     (A more general sort would take a comparison function, but it's
  *     not clear whether UVector's UElementComparator or
  *     UComparator from uprv_sortAray would be more appropriate.)
  */
فراغ UVector::sorti(UError_کد &ec) {
    إذا (U_SUCCESS(ec)) {
        uprv_مرتب_سازی_Array(elements, count, حجم(UElement),
                       sortiComparator, اشاره_تهی,  خطأ, &ec);
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
فراغ UVector::sort(UElementComparator *compare, UError_کد &ec) {
    إذا (U_SUCCESS(ec)) {
        uprv_مرتب_سازی_Array(elements, count, حجم(UElement),
                       مرتب_سازی_Comparator, &compare, خطأ, &ec);
    }
}


/**
 *  Stable sort with a user supplied comparator of type UComparator.
 */
فراغ UVector::مرتب_سازی_With_UComparator(UComparator *compare, ثابت فراغ *context, UError_کد &ec) {
    إذا (U_SUCCESS(ec)) {
        uprv_مرتب_سازی_Array(elements, count, حجم(UElement),
                       compare, context, درست, &ec);
    }
}

U_NAMESPACE_انتها

