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

constexpr int32_t DEFAULT_استعداد = 8;

/*
 * Constants for hinting whether a key is an integer
 * or a pointer.  If a hint bit is zero, then the associated
 * token is assumed to be an integer. This is needed for iSeries
 */
constexpr int8_t HINT_کلید_اشارہ_گر = 1;
constexpr int8_t HINT_کلید_INTEGER = 0;
 
UOBJECT_DEFINE_RTTI_IMPLEMENTATION(UVector)

UVector::UVector(UError_رمز &حالت_2) :
        UVector(خالی_اشاریہ, خالی_اشاریہ, DEFAULT_استعداد, حالت_2) {
}

UVector::UVector(int32_t initial_استعداد, UError_رمز &حالت_2) :
        UVector(خالی_اشاریہ, خالی_اشاریہ, initial_استعداد, حالت_2) {
}

UVector::UVector(UObjectDeleter *d, UElementsAreEqual *c, UError_رمز &حالت_2) :
        UVector(d, c, DEFAULT_استعداد, حالت_2) {
}

UVector::UVector(UObjectDeleter *d, UElementsAreEqual *c, int32_t initial_استعداد, UError_رمز &حالت_2) :
    deleter(d),
    comparer(c)
{
    اگر (U_FAILURE(حالت_2)) {
        واپس;
    }
    // Fix bogus initialCapacity values; avoid malloc(0) and integer overflow
    اگر ((initial_استعداد < 1) || (initial_استعداد > static_cast<int32_t>(INT32_MAX / حجم(UElement)))) {
        initial_استعداد = DEFAULT_استعداد;
    }
    elements = static_cast<UElement*>(uprv_malloc(حجم(UElement) * initial_استعداد));
    اگر (elements == خالی_اشاریہ) {
        حالت_2 = U_حافظہ_تخصیص_ERROR;
    } ورنہ {
        capacity = initial_استعداد;
    }
}

UVector::~UVector() {
    removeAllElements();
    uprv_free(elements);
    elements = خالی_اشاریہ;
}

/**
 * Assign this object to another (make this a copy of 'other').
 * Use the 'assign' function to assign each element.
 */
خالی UVector::assign(ثابت UVector& other, UElementAssigner *assign, UError_رمز &ec) {
    اگر (ensure_استعداد(other.count, ec)) {
        سیٹ_سائز(other.count, ec);
        اگر (U_SUCCESS(ec)) {
            برائے (int32_t i=0; i<other.count; ++i) {
                اگر (elements[i].اشارہ_گر != خالی_اشاریہ && deleter != خالی_اشاریہ) {
                    (*deleter)(elements[i].اشارہ_گر);
                }
                (*assign)(&elements[i], &other.elements[i]);
            }
        }
    }
}

// This only does something sensible if this object has a non-null comparer
منطقی UVector::عامل==(ثابت UVector& other) ثابت {
    U_ASSERT(comparer != خالی_اشاریہ);
    اگر (count != other.count) واپس جھوٹ;
    اگر (comparer != خالی_اشاریہ) {
        // Compare using this object's comparer
        برائے (int32_t i=0; i<count; ++i) {
            اگر (!(*comparer)(elements[i], other.elements[i])) {
                واپس جھوٹ;
            }
        }
    }
    واپس سچ;
}

خالی UVector::جوڑو_Element(خالی* obj, UError_رمز &حالت_2) {
    U_ASSERT(deleter == خالی_اشاریہ);
    اگر (ensure_استعداد(count + 1, حالت_2)) {
        elements[count++].اشارہ_گر = obj;
    }
}

خالی UVector::adoptElement(خالی* obj, UError_رمز &حالت_2) {
    U_ASSERT(deleter != خالی_اشاریہ);
    اگر (ensure_استعداد(count + 1, حالت_2)) {
        elements[count++].اشارہ_گر = obj;
    } ورنہ {
        (*deleter)(obj);
    }
}
خالی UVector::جوڑو_Element(int32_t elem, UError_رمز &حالت_2) {
    U_ASSERT(deleter == خالی_اشاریہ);  // Usage error. Mixing up ints and pointers.
    اگر (ensure_استعداد(count + 1, حالت_2)) {
        elements[count].اشارہ_گر = خالی_اشاریہ;     // Pointers may be bigger than ints.
        elements[count].integer = elem;
        count++;
    }
}

خالی UVector::سیٹ_Element_At(خالی* obj, int32_t اشاریہ) {
    اگر (0 <= اشاریہ && اشاریہ < count) {
        اگر (elements[اشاریہ].اشارہ_گر != خالی_اشاریہ && deleter != خالی_اشاریہ) {
            (*deleter)(elements[اشاریہ].اشارہ_گر);
        }
        elements[اشاریہ].اشارہ_گر = obj;
    } ورنہ {
        /* index out of range */
        اگر (deleter != خالی_اشاریہ) {
            (*deleter)(obj);
        }
    }
}

خالی UVector::سیٹ_Element_At(int32_t elem, int32_t اشاریہ) {
    U_ASSERT(deleter == خالی_اشاریہ);  // Usage error. Mixing up ints and pointers.
    اگر (0 <= اشاریہ && اشاریہ < count) {
        elements[اشاریہ].اشارہ_گر = خالی_اشاریہ;
        elements[اشاریہ].integer = elem;
    }
    /* else index out of range */
}

خالی UVector::داخل_کرو_Element_At(خالی* obj, int32_t اشاریہ, UError_رمز &حالت_2) {
    اگر (ensure_استعداد(count + 1, حالت_2)) {
        اگر (0 <= اشاریہ && اشاریہ <= count) {
            برائے (int32_t i=count; i>اشاریہ; --i) {
                elements[i] = elements[i-1];
            }
            elements[اشاریہ].اشارہ_گر = obj;
            ++count;
        } ورنہ {
            /* index out of range */
            حالت_2 = U_ILLEGAL_دلیل_ERROR;
        }
    }
    اگر (U_FAILURE(حالت_2) && deleter != خالی_اشاریہ) {
        (*deleter)(obj);
    }
}

خالی UVector::داخل_کرو_Element_At(int32_t elem, int32_t اشاریہ, UError_رمز &حالت_2) {
    U_ASSERT(deleter == خالی_اشاریہ);  // Usage error. Mixing up ints and pointers.
    // must have 0 <= index <= count
    اگر (ensure_استعداد(count + 1, حالت_2)) {
        اگر (0 <= اشاریہ && اشاریہ <= count) {
            برائے (int32_t i=count; i>اشاریہ; --i) {
                elements[i] = elements[i-1];
            }
            elements[اشاریہ].اشارہ_گر = خالی_اشاریہ;
            elements[اشاریہ].integer = elem;
            ++count;
        } ورنہ {
            /* index out of range */
            حالت_2 = U_ILLEGAL_دلیل_ERROR;
        }
    }
}

خالی* UVector::elementAt(int32_t اشاریہ) ثابت {
    واپس (0 <= اشاریہ && اشاریہ < count) ? elements[اشاریہ].اشارہ_گر : خالی_اشاریہ;
}

int32_t UVector::elementAti(int32_t اشاریہ) ثابت {
    واپس (0 <= اشاریہ && اشاریہ < count) ? elements[اشاریہ].integer : 0;
}

UBool UVector::شامل_ہے_All(ثابت UVector& other) ثابت {
    برائے (int32_t i=0; i<other.size(); ++i) {
        اگر (اشاریہ_کا(other.elements[i]) < 0) {
            واپس جھوٹ;
        }
    }
    واپس سچ;
}

UBool UVector::شامل_ہے_None(ثابت UVector& other) ثابت {
    برائے (int32_t i=0; i<other.size(); ++i) {
        اگر (اشاریہ_کا(other.elements[i]) >= 0) {
            واپس جھوٹ;
        }
    }
    واپس سچ;
}

UBool UVector::removeAll(ثابت UVector& other) {
    UBool بدلا = جھوٹ;
    برائے (int32_t i=0; i<other.size(); ++i) {
        int32_t j = اشاریہ_کا(other.elements[i]);
        اگر (j >= 0) {
            removeElementAt(j);
            بدلا = سچ;
        }
    }
    واپس بدلا;
}

UBool UVector::retainAll(ثابت UVector& other) {
    UBool بدلا = جھوٹ;
    برائے (int32_t j=size()-1; j>=0; --j) {
        int32_t i = other.اشاریہ_کا(elements[j]);
        اگر (i < 0) {
            removeElementAt(j);
            بدلا = سچ;
        }
    }
    واپس بدلا;
}

خالی UVector::removeElementAt(int32_t اشاریہ) {
    خالی* e = orphanElementAt(اشاریہ);
    اگر (e != خالی_اشاریہ && deleter != خالی_اشاریہ) {
        (*deleter)(e);
    }
}

UBool UVector::removeElement(خالی* obj) {
    int32_t i = اشاریہ_کا(obj);
    اگر (i >= 0) {
        removeElementAt(i);
        واپس سچ;
    }
    واپس جھوٹ;
}

خالی UVector::removeAllElements() {
    اگر (deleter != خالی_اشاریہ) {
        برائے (int32_t i=0; i<count; ++i) {
            اگر (elements[i].اشارہ_گر != خالی_اشاریہ) {
                (*deleter)(elements[i].اشارہ_گر);
            }
        }
    }
    count = 0;
}

UBool   UVector::equals(ثابت UVector &other) ثابت {
    صحیح      i;

    اگر (یہ->count != other.count) {
        واپس جھوٹ;
    }
    اگر (comparer == خالی_اشاریہ) {
        برائے (i=0; i<count; i++) {
            اگر (elements[i].اشارہ_گر != other.elements[i].اشارہ_گر) {
                واپس جھوٹ;
            }
        }
    } ورنہ {
        UElement کلید;
        برائے (i=0; i<count; i++) {
            کلید.اشارہ_گر = &other.elements[i];
            اگر (!(*comparer)(کلید, elements[i])) {
                واپس جھوٹ;
            }
        }
    }
    واپس سچ;
}



int32_t UVector::اشاریہ_کا(خالی* obj, int32_t آغاز_اشاریہ) ثابت {
    UElement کلید;
    کلید.اشارہ_گر = obj;
    واپس اشاریہ_کا(کلید, آغاز_اشاریہ, HINT_کلید_اشارہ_گر);
}

int32_t UVector::اشاریہ_کا(int32_t obj, int32_t آغاز_اشاریہ) ثابت {
    UElement کلید;
    کلید.integer = obj;
    واپس اشاریہ_کا(کلید, آغاز_اشاریہ, HINT_کلید_INTEGER);
}

int32_t UVector::اشاریہ_کا(UElement کلید, int32_t آغاز_اشاریہ, int8_t hint) ثابت {
    اگر (comparer != خالی_اشاریہ) {
        برائے (int32_t i=آغاز_اشاریہ; i<count; ++i) {
            اگر ((*comparer)(کلید, elements[i])) {
                واپس i;
            }
        }
    } ورنہ {
        برائے (int32_t i=آغاز_اشاریہ; i<count; ++i) {
            /* Pointers are not always the same size as ints so to perform
             * a valid comparison we need to know whether we are being
             * provided an int or a pointer. */
            اگر (hint & HINT_کلید_اشارہ_گر) {
                اگر (کلید.اشارہ_گر == elements[i].اشارہ_گر) {
                    واپس i;
                }
            } ورنہ {
                اگر (کلید.integer == elements[i].integer) {
                    واپس i;
                }
            }
        }
    }
    واپس -1;
}

UBool UVector::ensure_استعداد(int32_t minimum_استعداد, UError_رمز &حالت_2) {
    اگر (U_FAILURE(حالت_2)) {
        واپس جھوٹ;
    }
    اگر (minimum_استعداد < 0) {
        حالت_2 = U_ILLEGAL_دلیل_ERROR;
        واپس جھوٹ;
    }
    اگر (capacity < minimum_استعداد) {
        اگر (capacity > (INT32_MAX - 1) / 2) {        	// integer overflow check
            حالت_2 = U_ILLEGAL_دلیل_ERROR;
            واپس جھوٹ;
        }
        int32_t newCap = capacity * 2;
        اگر (newCap < minimum_استعداد) {
            newCap = minimum_استعداد;
        }
        اگر (newCap > static_cast<int32_t>(INT32_MAX / حجم(UElement))) { // integer overflow check
            // We keep the original memory contents on bad minimumCapacity.
            حالت_2 = U_ILLEGAL_دلیل_ERROR;
            واپس جھوٹ;
        }
        UElement* newElems = static_cast<UElement*>(uprv_realloc(elements, حجم(UElement) * newCap));
        اگر (newElems == خالی_اشاریہ) {
            // We keep the original contents on the memory failure on realloc or bad minimumCapacity.
            حالت_2 = U_حافظہ_تخصیص_ERROR;
            واپس جھوٹ;
        }
        elements = newElems;
        capacity = newCap;
    }
    واپس سچ;
}

/**
 * Change the size of this vector as follows: If newSize is smaller,
 * then truncate the array, possibly deleting held elements for i >=
 * newSize.  If newSize is larger, grow the array, filling in new
 * slots with nullptr.
 */
خالی UVector::سیٹ_سائز(int32_t new_سائز, UError_رمز &حالت_2) {
    اگر (!ensure_استعداد(new_سائز, حالت_2)) {
        واپس;
    }
    اگر (new_سائز > count) {
        UElement empty;
        empty.اشارہ_گر = خالی_اشاریہ;
        empty.integer = 0;
        برائے (int32_t i=count; i<new_سائز; ++i) {
            elements[i] = empty;
        }
    } ورنہ {
        /* Most efficient to count down */
        برائے (int32_t i=count-1; i>=new_سائز; --i) {
            removeElementAt(i);
        }
    }
    count = new_سائز;
}

/**
 * Fill in the given array with all elements of this vector.
 */
خالی** UVector::مقصد_Array(خالی** نتیجہ) ثابت {
    خالی** a = نتیجہ;
    برائے (صحیح i=0; i<count; ++i) {
        *a++ = elements[i].اشارہ_گر;
    }
    واپس نتیجہ;
}

UObjectDeleter *UVector::سیٹ_Deleter(UObjectDeleter *d) {
    UObjectDeleter *old = deleter;
    deleter = d;
    واپس old;
}

UElementsAreEqual *UVector::سیٹ_Comparer(UElementsAreEqual *d) {
    UElementsAreEqual *old = comparer;
    comparer = d;
    واپس old;
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
خالی* UVector::orphanElementAt(int32_t اشاریہ) {
    خالی* e = خالی_اشاریہ;
    اگر (0 <= اشاریہ && اشاریہ < count) {
        e = elements[اشاریہ].اشارہ_گر;
        برائے (int32_t i=اشاریہ; i<count-1; ++i) {
            elements[i] = elements[i+1];
        }
        --count;
    }
    /* else index out of range */
    واپس e;
}

/**
 * Insert the given object into this vector at its sorted position
 * as defined by 'compare'.  The current elements are assumed to
 * be sorted already.
 */
خالی UVector::sorted_داخل_کرو(خالی* obj, UElementComparator *compare, UError_رمز& ec) {
    UElement e;
    e.اشارہ_گر = obj;
    sorted_داخل_کرو(e, compare, ec);
}

/**
 * Insert the given integer into this vector at its sorted position
 * as defined by 'compare'.  The current elements are assumed to
 * be sorted already.
 */
خالی UVector::sorted_داخل_کرو(int32_t obj, UElementComparator *compare, UError_رمز& ec) {
    U_ASSERT(deleter == خالی_اشاریہ);
    UElement e {};
    e.integer = obj;
    sorted_داخل_کرو(e, compare, ec);
}

// ASSUME elements[] IS CURRENTLY SORTED
خالی UVector::sorted_داخل_کرو(UElement e, UElementComparator *compare, UError_رمز& ec) {
    // Perform a binary search for the location to insert tok at.  Tok
    // will be inserted between two elements a and b such that a <=
    // tok && tok < b, where there is a 'virtual' elements[-1] always
    // less than tok and a 'virtual' elements[count] always greater
    // than tok.
    اگر (!ensure_استعداد(count + 1, ec)) {
        اگر (deleter != خالی_اشاریہ) {
            (*deleter)(e.اشارہ_گر);
        }
        واپس;
    }
    int32_t min = 0, max = count;
    جب (min != max) {
        int32_t probe = (min + max) / 2;
        int32_t c = (*compare)(elements[probe], e);
        اگر (c > 0) {
            max = probe;
        } ورنہ {
            // assert(c <= 0);
            min = probe + 1;
        }
    }
    برائے (int32_t i=count; i>min; --i) {
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
ساکن int32_t U_CALLCONV
ترتیب_دو_Comparator(ثابت خالی *context, ثابت خالی *بائیں, ثابت خالی *دائیں) {
    UElementComparator *compare = *static_cast<UElementComparator * ثابت *>(context);
    UElement e1 = *static_cast<ثابت UElement *>(بائیں);
    UElement e2 = *static_cast<ثابت UElement *>(دائیں);
    int32_t نتیجہ = (*compare)(e1, e2);
    واپس نتیجہ;
}


/**
  *  Array sort comparison function for use from UVector::sorti()
  *  Compares int32_t vector elements.
  */
ساکن int32_t U_CALLCONV
sortiComparator(ثابت خالی * /*context */, ثابت خالی *بائیں, ثابت خالی *دائیں) {
    ثابت UElement *e1 = static_cast<ثابت UElement *>(بائیں);
    ثابت UElement *e2 = static_cast<ثابت UElement *>(دائیں);
    int32_t نتیجہ = e1->integer < e2->integer? -1 :
                     e1->integer == e2->integer? 0 : 1;
    واپس نتیجہ;
}

/**
  * Sort the vector, assuming it contains ints.
  *     (A more general sort would take a comparison function, but it's
  *     not clear whether UVector's UElementComparator or
  *     UComparator from uprv_sortAray would be more appropriate.)
  */
خالی UVector::sorti(UError_رمز &ec) {
    اگر (U_SUCCESS(ec)) {
        uprv_ترتیب_دو_Array(elements, count, حجم(UElement),
                       sortiComparator, خالی_اشاریہ,  جھوٹ, &ec);
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
خالی UVector::sort(UElementComparator *compare, UError_رمز &ec) {
    اگر (U_SUCCESS(ec)) {
        uprv_ترتیب_دو_Array(elements, count, حجم(UElement),
                       ترتیب_دو_Comparator, &compare, جھوٹ, &ec);
    }
}


/**
 *  Stable sort with a user supplied comparator of type UComparator.
 */
خالی UVector::ترتیب_دو_With_UComparator(UComparator *compare, ثابت خالی *context, UError_رمز &ec) {
    اگر (U_SUCCESS(ec)) {
        uprv_ترتیب_دو_Array(elements, count, حجم(UElement),
                       compare, context, سچ, &ec);
    }
}

U_NAMESPACE_انتہا

