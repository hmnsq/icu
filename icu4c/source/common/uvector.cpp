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

constexpr int32_t DEFAULT_سمرتھا = 8;

/*
 * Constants for hinting whether a key is an integer
 * or a pointer.  If a hint bit is zero, then the associated
 * token is assumed to be an integer. This is needed for iSeries
 */
constexpr int8_t HINT_کلید_اشارہ_گر = 1;
constexpr int8_t HINT_کلید_INTEGER = 0;
 
UOBJECT_DEFINE_RTTI_IMPLEMENTATION(UVector)

UVector::UVector(UError_کوڈ &حالت) :
        UVector(خالی_سوچک, خالی_سوچک, DEFAULT_سمرتھا, حالت) {
}

UVector::UVector(int32_t initial_سمرتھا, UError_کوڈ &حالت) :
        UVector(خالی_سوچک, خالی_سوچک, initial_سمرتھا, حالت) {
}

UVector::UVector(UObjectDeleter *d, UElementsAreEqual *c, UError_کوڈ &حالت) :
        UVector(d, c, DEFAULT_سمرتھا, حالت) {
}

UVector::UVector(UObjectDeleter *d, UElementsAreEqual *c, int32_t initial_سمرتھا, UError_کوڈ &حالت) :
    deleter(d),
    comparer(c)
{
    إذا (U_FAILURE(حالت)) {
        أرجع;
    }
    // Fix bogus initialCapacity values; avoid malloc(0) and integer overflow
    إذا ((initial_سمرتھا < 1) || (initial_سمرتھا > static_cast<int32_t>(INT32_MAX / حجم(UElement)))) {
        initial_سمرتھا = DEFAULT_سمرتھا;
    }
    elements = static_cast<UElement*>(uprv_malloc(حجم(UElement) * initial_سمرتھا));
    إذا (elements == خالی_سوچک) {
        حالت = U_حافظہ_ونڈ_ERROR;
    } وإلا {
        capacity = initial_سمرتھا;
    }
}

UVector::~UVector() {
    removeAllElements();
    uprv_free(elements);
    elements = خالی_سوچک;
}

/**
 * Assign this object to another (make this a copy of 'other').
 * Use the 'assign' function to assign each element.
 */
فراغ UVector::assign(ثابت UVector& other, UElementAssigner *assign, UError_کوڈ &ec) {
    إذا (ensure_سمرتھا(other.count, ec)) {
        سیٹ_ناپ(other.count, ec);
        إذا (U_SUCCESS(ec)) {
            لكل (int32_t i=0; i<other.count; ++i) {
                إذا (elements[i].اشارہ_گر != خالی_سوچک && deleter != خالی_سوچک) {
                    (*deleter)(elements[i].اشارہ_گر);
                }
                (*assign)(&elements[i], &other.elements[i]);
            }
        }
    }
}

// This only does something sensible if this object has a non-null comparer
منطقي UVector::عامل==(ثابت UVector& other) ثابت {
    U_ASSERT(comparer != خالی_سوچک);
    إذا (count != other.count) أرجع خطأ;
    إذا (comparer != خالی_سوچک) {
        // Compare using this object's comparer
        لكل (int32_t i=0; i<count; ++i) {
            إذا (!(*comparer)(elements[i], other.elements[i])) {
                أرجع خطأ;
            }
        }
    }
    أرجع سچ;
}

فراغ UVector::جوڑو_Element(فراغ* obj, UError_کوڈ &حالت) {
    U_ASSERT(deleter == خالی_سوچک);
    إذا (ensure_سمرتھا(count + 1, حالت)) {
        elements[count++].اشارہ_گر = obj;
    }
}

فراغ UVector::adoptElement(فراغ* obj, UError_کوڈ &حالت) {
    U_ASSERT(deleter != خالی_سوچک);
    إذا (ensure_سمرتھا(count + 1, حالت)) {
        elements[count++].اشارہ_گر = obj;
    } وإلا {
        (*deleter)(obj);
    }
}
فراغ UVector::جوڑو_Element(int32_t elem, UError_کوڈ &حالت) {
    U_ASSERT(deleter == خالی_سوچک);  // Usage error. Mixing up ints and pointers.
    إذا (ensure_سمرتھا(count + 1, حالت)) {
        elements[count].اشارہ_گر = خالی_سوچک;     // Pointers may be bigger than ints.
        elements[count].integer = elem;
        count++;
    }
}

فراغ UVector::سیٹ_Element_At(فراغ* obj, int32_t اشاریہ) {
    إذا (0 <= اشاریہ && اشاریہ < count) {
        إذا (elements[اشاریہ].اشارہ_گر != خالی_سوچک && deleter != خالی_سوچک) {
            (*deleter)(elements[اشاریہ].اشارہ_گر);
        }
        elements[اشاریہ].اشارہ_گر = obj;
    } وإلا {
        /* index out of range */
        إذا (deleter != خالی_سوچک) {
            (*deleter)(obj);
        }
    }
}

فراغ UVector::سیٹ_Element_At(int32_t elem, int32_t اشاریہ) {
    U_ASSERT(deleter == خالی_سوچک);  // Usage error. Mixing up ints and pointers.
    إذا (0 <= اشاریہ && اشاریہ < count) {
        elements[اشاریہ].اشارہ_گر = خالی_سوچک;
        elements[اشاریہ].integer = elem;
    }
    /* else index out of range */
}

فراغ UVector::داخل_کرو_Element_At(فراغ* obj, int32_t اشاریہ, UError_کوڈ &حالت) {
    إذا (ensure_سمرتھا(count + 1, حالت)) {
        إذا (0 <= اشاریہ && اشاریہ <= count) {
            لكل (int32_t i=count; i>اشاریہ; --i) {
                elements[i] = elements[i-1];
            }
            elements[اشاریہ].اشارہ_گر = obj;
            ++count;
        } وإلا {
            /* index out of range */
            حالت = U_ILLEGAL_دلیل_ERROR;
        }
    }
    إذا (U_FAILURE(حالت) && deleter != خالی_سوچک) {
        (*deleter)(obj);
    }
}

فراغ UVector::داخل_کرو_Element_At(int32_t elem, int32_t اشاریہ, UError_کوڈ &حالت) {
    U_ASSERT(deleter == خالی_سوچک);  // Usage error. Mixing up ints and pointers.
    // must have 0 <= index <= count
    إذا (ensure_سمرتھا(count + 1, حالت)) {
        إذا (0 <= اشاریہ && اشاریہ <= count) {
            لكل (int32_t i=count; i>اشاریہ; --i) {
                elements[i] = elements[i-1];
            }
            elements[اشاریہ].اشارہ_گر = خالی_سوچک;
            elements[اشاریہ].integer = elem;
            ++count;
        } وإلا {
            /* index out of range */
            حالت = U_ILLEGAL_دلیل_ERROR;
        }
    }
}

فراغ* UVector::elementAt(int32_t اشاریہ) ثابت {
    أرجع (0 <= اشاریہ && اشاریہ < count) ? elements[اشاریہ].اشارہ_گر : خالی_سوچک;
}

int32_t UVector::elementAti(int32_t اشاریہ) ثابت {
    أرجع (0 <= اشاریہ && اشاریہ < count) ? elements[اشاریہ].integer : 0;
}

UBool UVector::شامل_اے_All(ثابت UVector& other) ثابت {
    لكل (int32_t i=0; i<other.size(); ++i) {
        إذا (اشاریہ_دا(other.elements[i]) < 0) {
            أرجع خطأ;
        }
    }
    أرجع سچ;
}

UBool UVector::شامل_اے_None(ثابت UVector& other) ثابت {
    لكل (int32_t i=0; i<other.size(); ++i) {
        إذا (اشاریہ_دا(other.elements[i]) >= 0) {
            أرجع خطأ;
        }
    }
    أرجع سچ;
}

UBool UVector::removeAll(ثابت UVector& other) {
    UBool بدلیا = خطأ;
    لكل (int32_t i=0; i<other.size(); ++i) {
        int32_t j = اشاریہ_دا(other.elements[i]);
        إذا (j >= 0) {
            removeElementAt(j);
            بدلیا = سچ;
        }
    }
    أرجع بدلیا;
}

UBool UVector::retainAll(ثابت UVector& other) {
    UBool بدلیا = خطأ;
    لكل (int32_t j=size()-1; j>=0; --j) {
        int32_t i = other.اشاریہ_دا(elements[j]);
        إذا (i < 0) {
            removeElementAt(j);
            بدلیا = سچ;
        }
    }
    أرجع بدلیا;
}

فراغ UVector::removeElementAt(int32_t اشاریہ) {
    فراغ* e = orphanElementAt(اشاریہ);
    إذا (e != خالی_سوچک && deleter != خالی_سوچک) {
        (*deleter)(e);
    }
}

UBool UVector::removeElement(فراغ* obj) {
    int32_t i = اشاریہ_دا(obj);
    إذا (i >= 0) {
        removeElementAt(i);
        أرجع سچ;
    }
    أرجع خطأ;
}

فراغ UVector::removeAllElements() {
    إذا (deleter != خالی_سوچک) {
        لكل (int32_t i=0; i<count; ++i) {
            إذا (elements[i].اشارہ_گر != خالی_سوچک) {
                (*deleter)(elements[i].اشارہ_گر);
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
    إذا (comparer == خالی_سوچک) {
        لكل (i=0; i<count; i++) {
            إذا (elements[i].اشارہ_گر != other.elements[i].اشارہ_گر) {
                أرجع خطأ;
            }
        }
    } وإلا {
        UElement کلید;
        لكل (i=0; i<count; i++) {
            کلید.اشارہ_گر = &other.elements[i];
            إذا (!(*comparer)(کلید, elements[i])) {
                أرجع خطأ;
            }
        }
    }
    أرجع سچ;
}



int32_t UVector::اشاریہ_دا(فراغ* obj, int32_t شروع_اشاریہ) ثابت {
    UElement کلید;
    کلید.اشارہ_گر = obj;
    أرجع اشاریہ_دا(کلید, شروع_اشاریہ, HINT_کلید_اشارہ_گر);
}

int32_t UVector::اشاریہ_دا(int32_t obj, int32_t شروع_اشاریہ) ثابت {
    UElement کلید;
    کلید.integer = obj;
    أرجع اشاریہ_دا(کلید, شروع_اشاریہ, HINT_کلید_INTEGER);
}

int32_t UVector::اشاریہ_دا(UElement کلید, int32_t شروع_اشاریہ, int8_t hint) ثابت {
    إذا (comparer != خالی_سوچک) {
        لكل (int32_t i=شروع_اشاریہ; i<count; ++i) {
            إذا ((*comparer)(کلید, elements[i])) {
                أرجع i;
            }
        }
    } وإلا {
        لكل (int32_t i=شروع_اشاریہ; i<count; ++i) {
            /* Pointers are not always the same size as ints so to perform
             * a valid comparison we need to know whether we are being
             * provided an int or a pointer. */
            إذا (hint & HINT_کلید_اشارہ_گر) {
                إذا (کلید.اشارہ_گر == elements[i].اشارہ_گر) {
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

UBool UVector::ensure_سمرتھا(int32_t minimum_سمرتھا, UError_کوڈ &حالت) {
    إذا (U_FAILURE(حالت)) {
        أرجع خطأ;
    }
    إذا (minimum_سمرتھا < 0) {
        حالت = U_ILLEGAL_دلیل_ERROR;
        أرجع خطأ;
    }
    إذا (capacity < minimum_سمرتھا) {
        إذا (capacity > (INT32_MAX - 1) / 2) {        	// integer overflow check
            حالت = U_ILLEGAL_دلیل_ERROR;
            أرجع خطأ;
        }
        int32_t newCap = capacity * 2;
        إذا (newCap < minimum_سمرتھا) {
            newCap = minimum_سمرتھا;
        }
        إذا (newCap > static_cast<int32_t>(INT32_MAX / حجم(UElement))) { // integer overflow check
            // We keep the original memory contents on bad minimumCapacity.
            حالت = U_ILLEGAL_دلیل_ERROR;
            أرجع خطأ;
        }
        UElement* newElems = static_cast<UElement*>(uprv_realloc(elements, حجم(UElement) * newCap));
        إذا (newElems == خالی_سوچک) {
            // We keep the original contents on the memory failure on realloc or bad minimumCapacity.
            حالت = U_حافظہ_ونڈ_ERROR;
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
فراغ UVector::سیٹ_ناپ(int32_t new_ناپ, UError_کوڈ &حالت) {
    إذا (!ensure_سمرتھا(new_ناپ, حالت)) {
        أرجع;
    }
    إذا (new_ناپ > count) {
        UElement empty;
        empty.اشارہ_گر = خالی_سوچک;
        empty.integer = 0;
        لكل (int32_t i=count; i<new_ناپ; ++i) {
            elements[i] = empty;
        }
    } وإلا {
        /* Most efficient to count down */
        لكل (int32_t i=count-1; i>=new_ناپ; --i) {
            removeElementAt(i);
        }
    }
    count = new_ناپ;
}

/**
 * Fill in the given array with all elements of this vector.
 */
فراغ** UVector::منزل_Array(فراغ** نتیجہ) ثابت {
    فراغ** a = نتیجہ;
    لكل (صحيح i=0; i<count; ++i) {
        *a++ = elements[i].اشارہ_گر;
    }
    أرجع نتیجہ;
}

UObjectDeleter *UVector::سیٹ_Deleter(UObjectDeleter *d) {
    UObjectDeleter *old = deleter;
    deleter = d;
    أرجع old;
}

UElementsAreEqual *UVector::سیٹ_Comparer(UElementsAreEqual *d) {
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
فراغ* UVector::orphanElementAt(int32_t اشاریہ) {
    فراغ* e = خالی_سوچک;
    إذا (0 <= اشاریہ && اشاریہ < count) {
        e = elements[اشاریہ].اشارہ_گر;
        لكل (int32_t i=اشاریہ; i<count-1; ++i) {
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
فراغ UVector::sorted_داخل_کرو(فراغ* obj, UElementComparator *compare, UError_کوڈ& ec) {
    UElement e;
    e.اشارہ_گر = obj;
    sorted_داخل_کرو(e, compare, ec);
}

/**
 * Insert the given integer into this vector at its sorted position
 * as defined by 'compare'.  The current elements are assumed to
 * be sorted already.
 */
فراغ UVector::sorted_داخل_کرو(int32_t obj, UElementComparator *compare, UError_کوڈ& ec) {
    U_ASSERT(deleter == خالی_سوچک);
    UElement e {};
    e.integer = obj;
    sorted_داخل_کرو(e, compare, ec);
}

// ASSUME elements[] IS CURRENTLY SORTED
فراغ UVector::sorted_داخل_کرو(UElement e, UElementComparator *compare, UError_کوڈ& ec) {
    // Perform a binary search for the location to insert tok at.  Tok
    // will be inserted between two elements a and b such that a <=
    // tok && tok < b, where there is a 'virtual' elements[-1] always
    // less than tok and a 'virtual' elements[count] always greater
    // than tok.
    إذا (!ensure_سمرتھا(count + 1, ec)) {
        إذا (deleter != خالی_سوچک) {
            (*deleter)(e.اشارہ_گر);
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
ترتیب_دیو_Comparator(ثابت فراغ *context, ثابت فراغ *کھبے, ثابت فراغ *سجے) {
    UElementComparator *compare = *static_cast<UElementComparator * ثابت *>(context);
    UElement e1 = *static_cast<ثابت UElement *>(کھبے);
    UElement e2 = *static_cast<ثابت UElement *>(سجے);
    int32_t نتیجہ = (*compare)(e1, e2);
    أرجع نتیجہ;
}


/**
  *  Array sort comparison function for use from UVector::sorti()
  *  Compares int32_t vector elements.
  */
ساكن int32_t U_CALLCONV
sortiComparator(ثابت فراغ * /*context */, ثابت فراغ *کھبے, ثابت فراغ *سجے) {
    ثابت UElement *e1 = static_cast<ثابت UElement *>(کھبے);
    ثابت UElement *e2 = static_cast<ثابت UElement *>(سجے);
    int32_t نتیجہ = e1->integer < e2->integer? -1 :
                     e1->integer == e2->integer? 0 : 1;
    أرجع نتیجہ;
}

/**
  * Sort the vector, assuming it contains ints.
  *     (A more general sort would take a comparison function, but it's
  *     not clear whether UVector's UElementComparator or
  *     UComparator from uprv_sortAray would be more appropriate.)
  */
فراغ UVector::sorti(UError_کوڈ &ec) {
    إذا (U_SUCCESS(ec)) {
        uprv_ترتیب_دیو_Array(elements, count, حجم(UElement),
                       sortiComparator, خالی_سوچک,  خطأ, &ec);
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
فراغ UVector::sort(UElementComparator *compare, UError_کوڈ &ec) {
    إذا (U_SUCCESS(ec)) {
        uprv_ترتیب_دیو_Array(elements, count, حجم(UElement),
                       ترتیب_دیو_Comparator, &compare, خطأ, &ec);
    }
}


/**
 *  Stable sort with a user supplied comparator of type UComparator.
 */
فراغ UVector::ترتیب_دیو_With_UComparator(UComparator *compare, ثابت فراغ *context, UError_کوڈ &ec) {
    إذا (U_SUCCESS(ec)) {
        uprv_ترتیب_دیو_Array(elements, count, حجم(UElement),
                       compare, context, سچ, &ec);
    }
}

U_NAMESPACE_سرا

