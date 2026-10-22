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

#అన్తర్భావ "uvector.h"
#అన్తర్భావ "cmemory.h"
#అన్తర్భావ "uarrsort.h"
#అన్తర్భావ "uelement.h"

U_NAMESPACE_BEGIN

constexpr int32_t DEFAULT_సామర్థ్యం = 8;

/*
 * Constants for hinting whether a key is an integer
 * or a pointer.  If a hint bit is zero, then the associated
 * token is assumed to be an integer. This is needed for iSeries
 */
constexpr int8_t HINT_కీలకం_సూచిక = 1;
constexpr int8_t HINT_కీలకం_INTEGER = 0;
 
UOBJECT_DEFINE_RTTI_IMPLEMENTATION(UVector)

UVector::UVector(UError_సంకేతం &స్థితి_2) :
        UVector(శూన్యసూచిక, శూన్యసూచిక, DEFAULT_సామర్థ్యం, స్థితి_2) {
}

UVector::UVector(int32_t initial_సామర్థ్యం, UError_సంకేతం &స్థితి_2) :
        UVector(శూన్యసూచిక, శూన్యసూచిక, initial_సామర్థ్యం, స్థితి_2) {
}

UVector::UVector(UObjectDeleter *d, UElementsAreEqual *c, UError_సంకేతం &స్థితి_2) :
        UVector(d, c, DEFAULT_సామర్థ్యం, స్థితి_2) {
}

UVector::UVector(UObjectDeleter *d, UElementsAreEqual *c, int32_t initial_సామర్థ్యం, UError_సంకేతం &స్థితి_2) :
    deleter(d),
    comparer(c)
{
    ఒకవేళ (U_FAILURE(స్థితి_2)) {
        తిరిగి;
    }
    // Fix bogus initialCapacity values; avoid malloc(0) and integer overflow
    ఒకవేళ ((initial_సామర్థ్యం < 1) || (initial_సామర్థ్యం > static_cast<int32_t>(INT32_MAX / పరిమాణం(UElement)))) {
        initial_సామర్థ్యం = DEFAULT_సామర్థ్యం;
    }
    elements = static_cast<UElement*>(uprv_malloc(పరిమాణం(UElement) * initial_సామర్థ్యం));
    ఒకవేళ (elements == శూన్యసూచిక) {
        స్థితి_2 = U_జ్ఞాపకం_కేటాయింపు_ERROR;
    } లేకపోతే {
        capacity = initial_సామర్థ్యం;
    }
}

UVector::~UVector() {
    removeAllElements();
    uprv_free(elements);
    elements = శూన్యసూచిక;
}

/**
 * Assign this object to another (make this a copy of 'other').
 * Use the 'assign' function to assign each element.
 */
శూన్య UVector::assign(స్థిరరాశి UVector& other, UElementAssigner *assign, UError_సంకేతం &ec) {
    ఒకవేళ (ensure_సామర్థ్యం(other.count, ec)) {
        సమితి_కొలత(other.count, ec);
        ఒకవేళ (U_SUCCESS(ec)) {
            కొరకు (int32_t i=0; i<other.count; ++i) {
                ఒకవేళ (elements[i].సూచిక != శూన్యసూచిక && deleter != శూన్యసూచిక) {
                    (*deleter)(elements[i].సూచిక);
                }
                (*assign)(&elements[i], &other.elements[i]);
            }
        }
    }
}

// This only does something sensible if this object has a non-null comparer
తార్కిక UVector::క్రియాకారకం==(స్థిరరాశి UVector& other) స్థిరరాశి {
    U_ASSERT(comparer != శూన్యసూచిక);
    ఒకవేళ (count != other.count) తిరిగి అబద్ధం;
    ఒకవేళ (comparer != శూన్యసూచిక) {
        // Compare using this object's comparer
        కొరకు (int32_t i=0; i<count; ++i) {
            ఒకవేళ (!(*comparer)(elements[i], other.elements[i])) {
                తిరిగి అబద్ధం;
            }
        }
    }
    తిరిగి నిజం;
}

శూన్య UVector::కూడు_Element(శూన్య* obj, UError_సంకేతం &స్థితి_2) {
    U_ASSERT(deleter == శూన్యసూచిక);
    ఒకవేళ (ensure_సామర్థ్యం(count + 1, స్థితి_2)) {
        elements[count++].సూచిక = obj;
    }
}

శూన్య UVector::adoptElement(శూన్య* obj, UError_సంకేతం &స్థితి_2) {
    U_ASSERT(deleter != శూన్యసూచిక);
    ఒకవేళ (ensure_సామర్థ్యం(count + 1, స్థితి_2)) {
        elements[count++].సూచిక = obj;
    } లేకపోతే {
        (*deleter)(obj);
    }
}
శూన్య UVector::కూడు_Element(int32_t elem, UError_సంకేతం &స్థితి_2) {
    U_ASSERT(deleter == శూన్యసూచిక);  // Usage error. Mixing up ints and pointers.
    ఒకవేళ (ensure_సామర్థ్యం(count + 1, స్థితి_2)) {
        elements[count].సూచిక = శూన్యసూచిక;     // Pointers may be bigger than ints.
        elements[count].integer = elem;
        count++;
    }
}

శూన్య UVector::సమితి_Element_At(శూన్య* obj, int32_t సూచీ) {
    ఒకవేళ (0 <= సూచీ && సూచీ < count) {
        ఒకవేళ (elements[సూచీ].సూచిక != శూన్యసూచిక && deleter != శూన్యసూచిక) {
            (*deleter)(elements[సూచీ].సూచిక);
        }
        elements[సూచీ].సూచిక = obj;
    } లేకపోతే {
        /* index out of range */
        ఒకవేళ (deleter != శూన్యసూచిక) {
            (*deleter)(obj);
        }
    }
}

శూన్య UVector::సమితి_Element_At(int32_t elem, int32_t సూచీ) {
    U_ASSERT(deleter == శూన్యసూచిక);  // Usage error. Mixing up ints and pointers.
    ఒకవేళ (0 <= సూచీ && సూచీ < count) {
        elements[సూచీ].సూచిక = శూన్యసూచిక;
        elements[సూచీ].integer = elem;
    }
    /* else index out of range */
}

శూన్య UVector::చొప్పించు_Element_At(శూన్య* obj, int32_t సూచీ, UError_సంకేతం &స్థితి_2) {
    ఒకవేళ (ensure_సామర్థ్యం(count + 1, స్థితి_2)) {
        ఒకవేళ (0 <= సూచీ && సూచీ <= count) {
            కొరకు (int32_t i=count; i>సూచీ; --i) {
                elements[i] = elements[i-1];
            }
            elements[సూచీ].సూచిక = obj;
            ++count;
        } లేకపోతే {
            /* index out of range */
            స్థితి_2 = U_ILLEGAL_వాదం_ERROR;
        }
    }
    ఒకవేళ (U_FAILURE(స్థితి_2) && deleter != శూన్యసూచిక) {
        (*deleter)(obj);
    }
}

శూన్య UVector::చొప్పించు_Element_At(int32_t elem, int32_t సూచీ, UError_సంకేతం &స్థితి_2) {
    U_ASSERT(deleter == శూన్యసూచిక);  // Usage error. Mixing up ints and pointers.
    // must have 0 <= index <= count
    ఒకవేళ (ensure_సామర్థ్యం(count + 1, స్థితి_2)) {
        ఒకవేళ (0 <= సూచీ && సూచీ <= count) {
            కొరకు (int32_t i=count; i>సూచీ; --i) {
                elements[i] = elements[i-1];
            }
            elements[సూచీ].సూచిక = శూన్యసూచిక;
            elements[సూచీ].integer = elem;
            ++count;
        } లేకపోతే {
            /* index out of range */
            స్థితి_2 = U_ILLEGAL_వాదం_ERROR;
        }
    }
}

శూన్య* UVector::elementAt(int32_t సూచీ) స్థిరరాశి {
    తిరిగి (0 <= సూచీ && సూచీ < count) ? elements[సూచీ].సూచిక : శూన్యసూచిక;
}

int32_t UVector::elementAti(int32_t సూచీ) స్థిరరాశి {
    తిరిగి (0 <= సూచీ && సూచీ < count) ? elements[సూచీ].integer : 0;
}

UBool UVector::కలిగియుంది_All(స్థిరరాశి UVector& other) స్థిరరాశి {
    కొరకు (int32_t i=0; i<other.size(); ++i) {
        ఒకవేళ (సూచీ_యొక్క(other.elements[i]) < 0) {
            తిరిగి అబద్ధం;
        }
    }
    తిరిగి నిజం;
}

UBool UVector::కలిగియుంది_None(స్థిరరాశి UVector& other) స్థిరరాశి {
    కొరకు (int32_t i=0; i<other.size(); ++i) {
        ఒకవేళ (సూచీ_యొక్క(other.elements[i]) >= 0) {
            తిరిగి అబద్ధం;
        }
    }
    తిరిగి నిజం;
}

UBool UVector::removeAll(స్థిరరాశి UVector& other) {
    UBool మారింది = అబద్ధం;
    కొరకు (int32_t i=0; i<other.size(); ++i) {
        int32_t j = సూచీ_యొక్క(other.elements[i]);
        ఒకవేళ (j >= 0) {
            removeElementAt(j);
            మారింది = నిజం;
        }
    }
    తిరిగి మారింది;
}

UBool UVector::retainAll(స్థిరరాశి UVector& other) {
    UBool మారింది = అబద్ధం;
    కొరకు (int32_t j=size()-1; j>=0; --j) {
        int32_t i = other.సూచీ_యొక్క(elements[j]);
        ఒకవేళ (i < 0) {
            removeElementAt(j);
            మారింది = నిజం;
        }
    }
    తిరిగి మారింది;
}

శూన్య UVector::removeElementAt(int32_t సూచీ) {
    శూన్య* e = orphanElementAt(సూచీ);
    ఒకవేళ (e != శూన్యసూచిక && deleter != శూన్యసూచిక) {
        (*deleter)(e);
    }
}

UBool UVector::removeElement(శూన్య* obj) {
    int32_t i = సూచీ_యొక్క(obj);
    ఒకవేళ (i >= 0) {
        removeElementAt(i);
        తిరిగి నిజం;
    }
    తిరిగి అబద్ధం;
}

శూన్య UVector::removeAllElements() {
    ఒకవేళ (deleter != శూన్యసూచిక) {
        కొరకు (int32_t i=0; i<count; ++i) {
            ఒకవేళ (elements[i].సూచిక != శూన్యసూచిక) {
                (*deleter)(elements[i].సూచిక);
            }
        }
    }
    count = 0;
}

UBool   UVector::equals(స్థిరరాశి UVector &other) స్థిరరాశి {
    పూర్ణాంకం      i;

    ఒకవేళ (ఇది->count != other.count) {
        తిరిగి అబద్ధం;
    }
    ఒకవేళ (comparer == శూన్యసూచిక) {
        కొరకు (i=0; i<count; i++) {
            ఒకవేళ (elements[i].సూచిక != other.elements[i].సూచిక) {
                తిరిగి అబద్ధం;
            }
        }
    } లేకపోతే {
        UElement కీలకం;
        కొరకు (i=0; i<count; i++) {
            కీలకం.సూచిక = &other.elements[i];
            ఒకవేళ (!(*comparer)(కీలకం, elements[i])) {
                తిరిగి అబద్ధం;
            }
        }
    }
    తిరిగి నిజం;
}



int32_t UVector::సూచీ_యొక్క(శూన్య* obj, int32_t ప్రారంభం_సూచీ) స్థిరరాశి {
    UElement కీలకం;
    కీలకం.సూచిక = obj;
    తిరిగి సూచీ_యొక్క(కీలకం, ప్రారంభం_సూచీ, HINT_కీలకం_సూచిక);
}

int32_t UVector::సూచీ_యొక్క(int32_t obj, int32_t ప్రారంభం_సూచీ) స్థిరరాశి {
    UElement కీలకం;
    కీలకం.integer = obj;
    తిరిగి సూచీ_యొక్క(కీలకం, ప్రారంభం_సూచీ, HINT_కీలకం_INTEGER);
}

int32_t UVector::సూచీ_యొక్క(UElement కీలకం, int32_t ప్రారంభం_సూచీ, int8_t hint) స్థిరరాశి {
    ఒకవేళ (comparer != శూన్యసూచిక) {
        కొరకు (int32_t i=ప్రారంభం_సూచీ; i<count; ++i) {
            ఒకవేళ ((*comparer)(కీలకం, elements[i])) {
                తిరిగి i;
            }
        }
    } లేకపోతే {
        కొరకు (int32_t i=ప్రారంభం_సూచీ; i<count; ++i) {
            /* Pointers are not always the same size as ints so to perform
             * a valid comparison we need to know whether we are being
             * provided an int or a pointer. */
            ఒకవేళ (hint & HINT_కీలకం_సూచిక) {
                ఒకవేళ (కీలకం.సూచిక == elements[i].సూచిక) {
                    తిరిగి i;
                }
            } లేకపోతే {
                ఒకవేళ (కీలకం.integer == elements[i].integer) {
                    తిరిగి i;
                }
            }
        }
    }
    తిరిగి -1;
}

UBool UVector::ensure_సామర్థ్యం(int32_t minimum_సామర్థ్యం, UError_సంకేతం &స్థితి_2) {
    ఒకవేళ (U_FAILURE(స్థితి_2)) {
        తిరిగి అబద్ధం;
    }
    ఒకవేళ (minimum_సామర్థ్యం < 0) {
        స్థితి_2 = U_ILLEGAL_వాదం_ERROR;
        తిరిగి అబద్ధం;
    }
    ఒకవేళ (capacity < minimum_సామర్థ్యం) {
        ఒకవేళ (capacity > (INT32_MAX - 1) / 2) {        	// integer overflow check
            స్థితి_2 = U_ILLEGAL_వాదం_ERROR;
            తిరిగి అబద్ధం;
        }
        int32_t newCap = capacity * 2;
        ఒకవేళ (newCap < minimum_సామర్థ్యం) {
            newCap = minimum_సామర్థ్యం;
        }
        ఒకవేళ (newCap > static_cast<int32_t>(INT32_MAX / పరిమాణం(UElement))) { // integer overflow check
            // We keep the original memory contents on bad minimumCapacity.
            స్థితి_2 = U_ILLEGAL_వాదం_ERROR;
            తిరిగి అబద్ధం;
        }
        UElement* newElems = static_cast<UElement*>(uprv_realloc(elements, పరిమాణం(UElement) * newCap));
        ఒకవేళ (newElems == శూన్యసూచిక) {
            // We keep the original contents on the memory failure on realloc or bad minimumCapacity.
            స్థితి_2 = U_జ్ఞాపకం_కేటాయింపు_ERROR;
            తిరిగి అబద్ధం;
        }
        elements = newElems;
        capacity = newCap;
    }
    తిరిగి నిజం;
}

/**
 * Change the size of this vector as follows: If newSize is smaller,
 * then truncate the array, possibly deleting held elements for i >=
 * newSize.  If newSize is larger, grow the array, filling in new
 * slots with nullptr.
 */
శూన్య UVector::సమితి_కొలత(int32_t new_కొలత, UError_సంకేతం &స్థితి_2) {
    ఒకవేళ (!ensure_సామర్థ్యం(new_కొలత, స్థితి_2)) {
        తిరిగి;
    }
    ఒకవేళ (new_కొలత > count) {
        UElement empty;
        empty.సూచిక = శూన్యసూచిక;
        empty.integer = 0;
        కొరకు (int32_t i=count; i<new_కొలత; ++i) {
            elements[i] = empty;
        }
    } లేకపోతే {
        /* Most efficient to count down */
        కొరకు (int32_t i=count-1; i>=new_కొలత; --i) {
            removeElementAt(i);
        }
    }
    count = new_కొలత;
}

/**
 * Fill in the given array with all elements of this vector.
 */
శూన్య** UVector::గమ్యం_Array(శూన్య** ఫలితం) స్థిరరాశి {
    శూన్య** a = ఫలితం;
    కొరకు (పూర్ణాంకం i=0; i<count; ++i) {
        *a++ = elements[i].సూచిక;
    }
    తిరిగి ఫలితం;
}

UObjectDeleter *UVector::సమితి_Deleter(UObjectDeleter *d) {
    UObjectDeleter *old = deleter;
    deleter = d;
    తిరిగి old;
}

UElementsAreEqual *UVector::సమితి_Comparer(UElementsAreEqual *d) {
    UElementsAreEqual *old = comparer;
    comparer = d;
    తిరిగి old;
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
శూన్య* UVector::orphanElementAt(int32_t సూచీ) {
    శూన్య* e = శూన్యసూచిక;
    ఒకవేళ (0 <= సూచీ && సూచీ < count) {
        e = elements[సూచీ].సూచిక;
        కొరకు (int32_t i=సూచీ; i<count-1; ++i) {
            elements[i] = elements[i+1];
        }
        --count;
    }
    /* else index out of range */
    తిరిగి e;
}

/**
 * Insert the given object into this vector at its sorted position
 * as defined by 'compare'.  The current elements are assumed to
 * be sorted already.
 */
శూన్య UVector::sorted_చొప్పించు(శూన్య* obj, UElementComparator *compare, UError_సంకేతం& ec) {
    UElement e;
    e.సూచిక = obj;
    sorted_చొప్పించు(e, compare, ec);
}

/**
 * Insert the given integer into this vector at its sorted position
 * as defined by 'compare'.  The current elements are assumed to
 * be sorted already.
 */
శూన్య UVector::sorted_చొప్పించు(int32_t obj, UElementComparator *compare, UError_సంకేతం& ec) {
    U_ASSERT(deleter == శూన్యసూచిక);
    UElement e {};
    e.integer = obj;
    sorted_చొప్పించు(e, compare, ec);
}

// ASSUME elements[] IS CURRENTLY SORTED
శూన్య UVector::sorted_చొప్పించు(UElement e, UElementComparator *compare, UError_సంకేతం& ec) {
    // Perform a binary search for the location to insert tok at.  Tok
    // will be inserted between two elements a and b such that a <=
    // tok && tok < b, where there is a 'virtual' elements[-1] always
    // less than tok and a 'virtual' elements[count] always greater
    // than tok.
    ఒకవేళ (!ensure_సామర్థ్యం(count + 1, ec)) {
        ఒకవేళ (deleter != శూన్యసూచిక) {
            (*deleter)(e.సూచిక);
        }
        తిరిగి;
    }
    int32_t min = 0, max = count;
    అయితే (min != max) {
        int32_t probe = (min + max) / 2;
        int32_t c = (*compare)(elements[probe], e);
        ఒకవేళ (c > 0) {
            max = probe;
        } లేకపోతే {
            // assert(c <= 0);
            min = probe + 1;
        }
    }
    కొరకు (int32_t i=count; i>min; --i) {
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
స్థిర int32_t U_CALLCONV
క్రమీకరించు_Comparator(స్థిరరాశి శూన్య *context, స్థిరరాశి శూన్య *ఎడమ, స్థిరరాశి శూన్య *కుడి) {
    UElementComparator *compare = *static_cast<UElementComparator * స్థిరరాశి *>(context);
    UElement e1 = *static_cast<స్థిరరాశి UElement *>(ఎడమ);
    UElement e2 = *static_cast<స్థిరరాశి UElement *>(కుడి);
    int32_t ఫలితం = (*compare)(e1, e2);
    తిరిగి ఫలితం;
}


/**
  *  Array sort comparison function for use from UVector::sorti()
  *  Compares int32_t vector elements.
  */
స్థిర int32_t U_CALLCONV
sortiComparator(స్థిరరాశి శూన్య * /*context */, స్థిరరాశి శూన్య *ఎడమ, స్థిరరాశి శూన్య *కుడి) {
    స్థిరరాశి UElement *e1 = static_cast<స్థిరరాశి UElement *>(ఎడమ);
    స్థిరరాశి UElement *e2 = static_cast<స్థిరరాశి UElement *>(కుడి);
    int32_t ఫలితం = e1->integer < e2->integer? -1 :
                     e1->integer == e2->integer? 0 : 1;
    తిరిగి ఫలితం;
}

/**
  * Sort the vector, assuming it contains ints.
  *     (A more general sort would take a comparison function, but it's
  *     not clear whether UVector's UElementComparator or
  *     UComparator from uprv_sortAray would be more appropriate.)
  */
శూన్య UVector::sorti(UError_సంకేతం &ec) {
    ఒకవేళ (U_SUCCESS(ec)) {
        uprv_క్రమీకరించు_Array(elements, count, పరిమాణం(UElement),
                       sortiComparator, శూన్యసూచిక,  అబద్ధం, &ec);
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
శూన్య UVector::sort(UElementComparator *compare, UError_సంకేతం &ec) {
    ఒకవేళ (U_SUCCESS(ec)) {
        uprv_క్రమీకరించు_Array(elements, count, పరిమాణం(UElement),
                       క్రమీకరించు_Comparator, &compare, అబద్ధం, &ec);
    }
}


/**
 *  Stable sort with a user supplied comparator of type UComparator.
 */
శూన్య UVector::క్రమీకరించు_With_UComparator(UComparator *compare, స్థిరరాశి శూన్య *context, UError_సంకేతం &ec) {
    ఒకవేళ (U_SUCCESS(ec)) {
        uprv_క్రమీకరించు_Array(elements, count, పరిమాణం(UElement),
                       compare, context, నిజం, &ec);
    }
}

U_NAMESPACE_చివర

