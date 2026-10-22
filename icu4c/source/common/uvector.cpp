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

#ਅਨ੍ਤਰ੍ਭਾਵ "uvector.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "cmemory.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "uarrsort.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "uelement.h"

U_NAMESPACE_BEGIN

constexpr int32_t DEFAULT_ਸਮਰੱਥਾ = 8;

/*
 * Constants for hinting whether a key is an integer
 * or a pointer.  If a hint bit is zero, then the associated
 * token is assumed to be an integer. This is needed for iSeries
 */
constexpr int8_t HINT_ਕੁੰਜੀ_ਸੂਚਕ = 1;
constexpr int8_t HINT_ਕੁੰਜੀ_INTEGER = 0;
 
UOBJECT_DEFINE_RTTI_IMPLEMENTATION(UVector)

UVector::UVector(UError_ਕੋਡ &ਸਥਿਤੀ) :
        UVector(ਖਾਲੀ_ਸੂਚਕ, ਖਾਲੀ_ਸੂਚਕ, DEFAULT_ਸਮਰੱਥਾ, ਸਥਿਤੀ) {
}

UVector::UVector(int32_t initial_ਸਮਰੱਥਾ, UError_ਕੋਡ &ਸਥਿਤੀ) :
        UVector(ਖਾਲੀ_ਸੂਚਕ, ਖਾਲੀ_ਸੂਚਕ, initial_ਸਮਰੱਥਾ, ਸਥਿਤੀ) {
}

UVector::UVector(UObjectDeleter *d, UElementsAreEqual *c, UError_ਕੋਡ &ਸਥਿਤੀ) :
        UVector(d, c, DEFAULT_ਸਮਰੱਥਾ, ਸਥਿਤੀ) {
}

UVector::UVector(UObjectDeleter *d, UElementsAreEqual *c, int32_t initial_ਸਮਰੱਥਾ, UError_ਕੋਡ &ਸਥਿਤੀ) :
    deleter(d),
    comparer(c)
{
    ਜੇ (U_FAILURE(ਸਥਿਤੀ)) {
        ਵਾਪਸ;
    }
    // Fix bogus initialCapacity values; avoid malloc(0) and integer overflow
    ਜੇ ((initial_ਸਮਰੱਥਾ < 1) || (initial_ਸਮਰੱਥਾ > static_cast<int32_t>(INT32_MAX / ਆਕਾਰ(UElement)))) {
        initial_ਸਮਰੱਥਾ = DEFAULT_ਸਮਰੱਥਾ;
    }
    elements = static_cast<UElement*>(uprv_malloc(ਆਕਾਰ(UElement) * initial_ਸਮਰੱਥਾ));
    ਜੇ (elements == ਖਾਲੀ_ਸੂਚਕ) {
        ਸਥਿਤੀ = U_ਸਮ੍ਰਿਤੀ_ਵੰਡ_ERROR;
    } ਨਹੀਂਤਾਂ {
        capacity = initial_ਸਮਰੱਥਾ;
    }
}

UVector::~UVector() {
    removeAllElements();
    uprv_free(elements);
    elements = ਖਾਲੀ_ਸੂਚਕ;
}

/**
 * Assign this object to another (make this a copy of 'other').
 * Use the 'assign' function to assign each element.
 */
ਖਾਲੀ UVector::assign(ਅਚਲ UVector& other, UElementAssigner *assign, UError_ਕੋਡ &ec) {
    ਜੇ (ensure_ਸਮਰੱਥਾ(other.count, ec)) {
        ਸੈੱਟ_ਮਾਪ(other.count, ec);
        ਜੇ (U_SUCCESS(ec)) {
            ਲਈ (int32_t i=0; i<other.count; ++i) {
                ਜੇ (elements[i].ਸੂਚਕ != ਖਾਲੀ_ਸੂਚਕ && deleter != ਖਾਲੀ_ਸੂਚਕ) {
                    (*deleter)(elements[i].ਸੂਚਕ);
                }
                (*assign)(&elements[i], &other.elements[i]);
            }
        }
    }
}

// This only does something sensible if this object has a non-null comparer
ਤਾਰਕਿਕ UVector::ਸੰਚਾਲਕ==(ਅਚਲ UVector& other) ਅਚਲ {
    U_ASSERT(comparer != ਖਾਲੀ_ਸੂਚਕ);
    ਜੇ (count != other.count) ਵਾਪਸ ਝੂਠ;
    ਜੇ (comparer != ਖਾਲੀ_ਸੂਚਕ) {
        // Compare using this object's comparer
        ਲਈ (int32_t i=0; i<count; ++i) {
            ਜੇ (!(*comparer)(elements[i], other.elements[i])) {
                ਵਾਪਸ ਝੂਠ;
            }
        }
    }
    ਵਾਪਸ ਸੱਚ;
}

ਖਾਲੀ UVector::ਜੋੜੋ_Element(ਖਾਲੀ* obj, UError_ਕੋਡ &ਸਥਿਤੀ) {
    U_ASSERT(deleter == ਖਾਲੀ_ਸੂਚਕ);
    ਜੇ (ensure_ਸਮਰੱਥਾ(count + 1, ਸਥਿਤੀ)) {
        elements[count++].ਸੂਚਕ = obj;
    }
}

ਖਾਲੀ UVector::adoptElement(ਖਾਲੀ* obj, UError_ਕੋਡ &ਸਥਿਤੀ) {
    U_ASSERT(deleter != ਖਾਲੀ_ਸੂਚਕ);
    ਜੇ (ensure_ਸਮਰੱਥਾ(count + 1, ਸਥਿਤੀ)) {
        elements[count++].ਸੂਚਕ = obj;
    } ਨਹੀਂਤਾਂ {
        (*deleter)(obj);
    }
}
ਖਾਲੀ UVector::ਜੋੜੋ_Element(int32_t elem, UError_ਕੋਡ &ਸਥਿਤੀ) {
    U_ASSERT(deleter == ਖਾਲੀ_ਸੂਚਕ);  // Usage error. Mixing up ints and pointers.
    ਜੇ (ensure_ਸਮਰੱਥਾ(count + 1, ਸਥਿਤੀ)) {
        elements[count].ਸੂਚਕ = ਖਾਲੀ_ਸੂਚਕ;     // Pointers may be bigger than ints.
        elements[count].integer = elem;
        count++;
    }
}

ਖਾਲੀ UVector::ਸੈੱਟ_Element_At(ਖਾਲੀ* obj, int32_t ਸੂਚਕਾਂਕ) {
    ਜੇ (0 <= ਸੂਚਕਾਂਕ && ਸੂਚਕਾਂਕ < count) {
        ਜੇ (elements[ਸੂਚਕਾਂਕ].ਸੂਚਕ != ਖਾਲੀ_ਸੂਚਕ && deleter != ਖਾਲੀ_ਸੂਚਕ) {
            (*deleter)(elements[ਸੂਚਕਾਂਕ].ਸੂਚਕ);
        }
        elements[ਸੂਚਕਾਂਕ].ਸੂਚਕ = obj;
    } ਨਹੀਂਤਾਂ {
        /* index out of range */
        ਜੇ (deleter != ਖਾਲੀ_ਸੂਚਕ) {
            (*deleter)(obj);
        }
    }
}

ਖਾਲੀ UVector::ਸੈੱਟ_Element_At(int32_t elem, int32_t ਸੂਚਕਾਂਕ) {
    U_ASSERT(deleter == ਖਾਲੀ_ਸੂਚਕ);  // Usage error. Mixing up ints and pointers.
    ਜੇ (0 <= ਸੂਚਕਾਂਕ && ਸੂਚਕਾਂਕ < count) {
        elements[ਸੂਚਕਾਂਕ].ਸੂਚਕ = ਖਾਲੀ_ਸੂਚਕ;
        elements[ਸੂਚਕਾਂਕ].integer = elem;
    }
    /* else index out of range */
}

ਖਾਲੀ UVector::ਪਾਓ_Element_At(ਖਾਲੀ* obj, int32_t ਸੂਚਕਾਂਕ, UError_ਕੋਡ &ਸਥਿਤੀ) {
    ਜੇ (ensure_ਸਮਰੱਥਾ(count + 1, ਸਥਿਤੀ)) {
        ਜੇ (0 <= ਸੂਚਕਾਂਕ && ਸੂਚਕਾਂਕ <= count) {
            ਲਈ (int32_t i=count; i>ਸੂਚਕਾਂਕ; --i) {
                elements[i] = elements[i-1];
            }
            elements[ਸੂਚਕਾਂਕ].ਸੂਚਕ = obj;
            ++count;
        } ਨਹੀਂਤਾਂ {
            /* index out of range */
            ਸਥਿਤੀ = U_ILLEGAL_ਦਲੀਲ_ERROR;
        }
    }
    ਜੇ (U_FAILURE(ਸਥਿਤੀ) && deleter != ਖਾਲੀ_ਸੂਚਕ) {
        (*deleter)(obj);
    }
}

ਖਾਲੀ UVector::ਪਾਓ_Element_At(int32_t elem, int32_t ਸੂਚਕਾਂਕ, UError_ਕੋਡ &ਸਥਿਤੀ) {
    U_ASSERT(deleter == ਖਾਲੀ_ਸੂਚਕ);  // Usage error. Mixing up ints and pointers.
    // must have 0 <= index <= count
    ਜੇ (ensure_ਸਮਰੱਥਾ(count + 1, ਸਥਿਤੀ)) {
        ਜੇ (0 <= ਸੂਚਕਾਂਕ && ਸੂਚਕਾਂਕ <= count) {
            ਲਈ (int32_t i=count; i>ਸੂਚਕਾਂਕ; --i) {
                elements[i] = elements[i-1];
            }
            elements[ਸੂਚਕਾਂਕ].ਸੂਚਕ = ਖਾਲੀ_ਸੂਚਕ;
            elements[ਸੂਚਕਾਂਕ].integer = elem;
            ++count;
        } ਨਹੀਂਤਾਂ {
            /* index out of range */
            ਸਥਿਤੀ = U_ILLEGAL_ਦਲੀਲ_ERROR;
        }
    }
}

ਖਾਲੀ* UVector::elementAt(int32_t ਸੂਚਕਾਂਕ) ਅਚਲ {
    ਵਾਪਸ (0 <= ਸੂਚਕਾਂਕ && ਸੂਚਕਾਂਕ < count) ? elements[ਸੂਚਕਾਂਕ].ਸੂਚਕ : ਖਾਲੀ_ਸੂਚਕ;
}

int32_t UVector::elementAti(int32_t ਸੂਚਕਾਂਕ) ਅਚਲ {
    ਵਾਪਸ (0 <= ਸੂਚਕਾਂਕ && ਸੂਚਕਾਂਕ < count) ? elements[ਸੂਚਕਾਂਕ].integer : 0;
}

UBool UVector::ਸ਼ਾਮਲ_ਹੈ_All(ਅਚਲ UVector& other) ਅਚਲ {
    ਲਈ (int32_t i=0; i<other.size(); ++i) {
        ਜੇ (ਸੂਚਕਾਂਕ_ਦਾ(other.elements[i]) < 0) {
            ਵਾਪਸ ਝੂਠ;
        }
    }
    ਵਾਪਸ ਸੱਚ;
}

UBool UVector::ਸ਼ਾਮਲ_ਹੈ_None(ਅਚਲ UVector& other) ਅਚਲ {
    ਲਈ (int32_t i=0; i<other.size(); ++i) {
        ਜੇ (ਸੂਚਕਾਂਕ_ਦਾ(other.elements[i]) >= 0) {
            ਵਾਪਸ ਝੂਠ;
        }
    }
    ਵਾਪਸ ਸੱਚ;
}

UBool UVector::removeAll(ਅਚਲ UVector& other) {
    UBool ਬਦਲਿਆ = ਝੂਠ;
    ਲਈ (int32_t i=0; i<other.size(); ++i) {
        int32_t j = ਸੂਚਕਾਂਕ_ਦਾ(other.elements[i]);
        ਜੇ (j >= 0) {
            removeElementAt(j);
            ਬਦਲਿਆ = ਸੱਚ;
        }
    }
    ਵਾਪਸ ਬਦਲਿਆ;
}

UBool UVector::retainAll(ਅਚਲ UVector& other) {
    UBool ਬਦਲਿਆ = ਝੂਠ;
    ਲਈ (int32_t j=size()-1; j>=0; --j) {
        int32_t i = other.ਸੂਚਕਾਂਕ_ਦਾ(elements[j]);
        ਜੇ (i < 0) {
            removeElementAt(j);
            ਬਦਲਿਆ = ਸੱਚ;
        }
    }
    ਵਾਪਸ ਬਦਲਿਆ;
}

ਖਾਲੀ UVector::removeElementAt(int32_t ਸੂਚਕਾਂਕ) {
    ਖਾਲੀ* e = orphanElementAt(ਸੂਚਕਾਂਕ);
    ਜੇ (e != ਖਾਲੀ_ਸੂਚਕ && deleter != ਖਾਲੀ_ਸੂਚਕ) {
        (*deleter)(e);
    }
}

UBool UVector::removeElement(ਖਾਲੀ* obj) {
    int32_t i = ਸੂਚਕਾਂਕ_ਦਾ(obj);
    ਜੇ (i >= 0) {
        removeElementAt(i);
        ਵਾਪਸ ਸੱਚ;
    }
    ਵਾਪਸ ਝੂਠ;
}

ਖਾਲੀ UVector::removeAllElements() {
    ਜੇ (deleter != ਖਾਲੀ_ਸੂਚਕ) {
        ਲਈ (int32_t i=0; i<count; ++i) {
            ਜੇ (elements[i].ਸੂਚਕ != ਖਾਲੀ_ਸੂਚਕ) {
                (*deleter)(elements[i].ਸੂਚਕ);
            }
        }
    }
    count = 0;
}

UBool   UVector::equals(ਅਚਲ UVector &other) ਅਚਲ {
    ਪੂਰਨ      i;

    ਜੇ (ਇਹ->count != other.count) {
        ਵਾਪਸ ਝੂਠ;
    }
    ਜੇ (comparer == ਖਾਲੀ_ਸੂਚਕ) {
        ਲਈ (i=0; i<count; i++) {
            ਜੇ (elements[i].ਸੂਚਕ != other.elements[i].ਸੂਚਕ) {
                ਵਾਪਸ ਝੂਠ;
            }
        }
    } ਨਹੀਂਤਾਂ {
        UElement ਕੁੰਜੀ;
        ਲਈ (i=0; i<count; i++) {
            ਕੁੰਜੀ.ਸੂਚਕ = &other.elements[i];
            ਜੇ (!(*comparer)(ਕੁੰਜੀ, elements[i])) {
                ਵਾਪਸ ਝੂਠ;
            }
        }
    }
    ਵਾਪਸ ਸੱਚ;
}



int32_t UVector::ਸੂਚਕਾਂਕ_ਦਾ(ਖਾਲੀ* obj, int32_t ਸ਼ੁਰੂ_ਸੂਚਕਾਂਕ) ਅਚਲ {
    UElement ਕੁੰਜੀ;
    ਕੁੰਜੀ.ਸੂਚਕ = obj;
    ਵਾਪਸ ਸੂਚਕਾਂਕ_ਦਾ(ਕੁੰਜੀ, ਸ਼ੁਰੂ_ਸੂਚਕਾਂਕ, HINT_ਕੁੰਜੀ_ਸੂਚਕ);
}

int32_t UVector::ਸੂਚਕਾਂਕ_ਦਾ(int32_t obj, int32_t ਸ਼ੁਰੂ_ਸੂਚਕਾਂਕ) ਅਚਲ {
    UElement ਕੁੰਜੀ;
    ਕੁੰਜੀ.integer = obj;
    ਵਾਪਸ ਸੂਚਕਾਂਕ_ਦਾ(ਕੁੰਜੀ, ਸ਼ੁਰੂ_ਸੂਚਕਾਂਕ, HINT_ਕੁੰਜੀ_INTEGER);
}

int32_t UVector::ਸੂਚਕਾਂਕ_ਦਾ(UElement ਕੁੰਜੀ, int32_t ਸ਼ੁਰੂ_ਸੂਚਕਾਂਕ, int8_t hint) ਅਚਲ {
    ਜੇ (comparer != ਖਾਲੀ_ਸੂਚਕ) {
        ਲਈ (int32_t i=ਸ਼ੁਰੂ_ਸੂਚਕਾਂਕ; i<count; ++i) {
            ਜੇ ((*comparer)(ਕੁੰਜੀ, elements[i])) {
                ਵਾਪਸ i;
            }
        }
    } ਨਹੀਂਤਾਂ {
        ਲਈ (int32_t i=ਸ਼ੁਰੂ_ਸੂਚਕਾਂਕ; i<count; ++i) {
            /* Pointers are not always the same size as ints so to perform
             * a valid comparison we need to know whether we are being
             * provided an int or a pointer. */
            ਜੇ (hint & HINT_ਕੁੰਜੀ_ਸੂਚਕ) {
                ਜੇ (ਕੁੰਜੀ.ਸੂਚਕ == elements[i].ਸੂਚਕ) {
                    ਵਾਪਸ i;
                }
            } ਨਹੀਂਤਾਂ {
                ਜੇ (ਕੁੰਜੀ.integer == elements[i].integer) {
                    ਵਾਪਸ i;
                }
            }
        }
    }
    ਵਾਪਸ -1;
}

UBool UVector::ensure_ਸਮਰੱਥਾ(int32_t minimum_ਸਮਰੱਥਾ, UError_ਕੋਡ &ਸਥਿਤੀ) {
    ਜੇ (U_FAILURE(ਸਥਿਤੀ)) {
        ਵਾਪਸ ਝੂਠ;
    }
    ਜੇ (minimum_ਸਮਰੱਥਾ < 0) {
        ਸਥਿਤੀ = U_ILLEGAL_ਦਲੀਲ_ERROR;
        ਵਾਪਸ ਝੂਠ;
    }
    ਜੇ (capacity < minimum_ਸਮਰੱਥਾ) {
        ਜੇ (capacity > (INT32_MAX - 1) / 2) {        	// integer overflow check
            ਸਥਿਤੀ = U_ILLEGAL_ਦਲੀਲ_ERROR;
            ਵਾਪਸ ਝੂਠ;
        }
        int32_t newCap = capacity * 2;
        ਜੇ (newCap < minimum_ਸਮਰੱਥਾ) {
            newCap = minimum_ਸਮਰੱਥਾ;
        }
        ਜੇ (newCap > static_cast<int32_t>(INT32_MAX / ਆਕਾਰ(UElement))) { // integer overflow check
            // We keep the original memory contents on bad minimumCapacity.
            ਸਥਿਤੀ = U_ILLEGAL_ਦਲੀਲ_ERROR;
            ਵਾਪਸ ਝੂਠ;
        }
        UElement* newElems = static_cast<UElement*>(uprv_realloc(elements, ਆਕਾਰ(UElement) * newCap));
        ਜੇ (newElems == ਖਾਲੀ_ਸੂਚਕ) {
            // We keep the original contents on the memory failure on realloc or bad minimumCapacity.
            ਸਥਿਤੀ = U_ਸਮ੍ਰਿਤੀ_ਵੰਡ_ERROR;
            ਵਾਪਸ ਝੂਠ;
        }
        elements = newElems;
        capacity = newCap;
    }
    ਵਾਪਸ ਸੱਚ;
}

/**
 * Change the size of this vector as follows: If newSize is smaller,
 * then truncate the array, possibly deleting held elements for i >=
 * newSize.  If newSize is larger, grow the array, filling in new
 * slots with nullptr.
 */
ਖਾਲੀ UVector::ਸੈੱਟ_ਮਾਪ(int32_t new_ਮਾਪ, UError_ਕੋਡ &ਸਥਿਤੀ) {
    ਜੇ (!ensure_ਸਮਰੱਥਾ(new_ਮਾਪ, ਸਥਿਤੀ)) {
        ਵਾਪਸ;
    }
    ਜੇ (new_ਮਾਪ > count) {
        UElement empty;
        empty.ਸੂਚਕ = ਖਾਲੀ_ਸੂਚਕ;
        empty.integer = 0;
        ਲਈ (int32_t i=count; i<new_ਮਾਪ; ++i) {
            elements[i] = empty;
        }
    } ਨਹੀਂਤਾਂ {
        /* Most efficient to count down */
        ਲਈ (int32_t i=count-1; i>=new_ਮਾਪ; --i) {
            removeElementAt(i);
        }
    }
    count = new_ਮਾਪ;
}

/**
 * Fill in the given array with all elements of this vector.
 */
ਖਾਲੀ** UVector::ਮੰਜ਼ਿਲ_Array(ਖਾਲੀ** ਨਤੀਜਾ) ਅਚਲ {
    ਖਾਲੀ** a = ਨਤੀਜਾ;
    ਲਈ (ਪੂਰਨ i=0; i<count; ++i) {
        *a++ = elements[i].ਸੂਚਕ;
    }
    ਵਾਪਸ ਨਤੀਜਾ;
}

UObjectDeleter *UVector::ਸੈੱਟ_Deleter(UObjectDeleter *d) {
    UObjectDeleter *old = deleter;
    deleter = d;
    ਵਾਪਸ old;
}

UElementsAreEqual *UVector::ਸੈੱਟ_Comparer(UElementsAreEqual *d) {
    UElementsAreEqual *old = comparer;
    comparer = d;
    ਵਾਪਸ old;
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
ਖਾਲੀ* UVector::orphanElementAt(int32_t ਸੂਚਕਾਂਕ) {
    ਖਾਲੀ* e = ਖਾਲੀ_ਸੂਚਕ;
    ਜੇ (0 <= ਸੂਚਕਾਂਕ && ਸੂਚਕਾਂਕ < count) {
        e = elements[ਸੂਚਕਾਂਕ].ਸੂਚਕ;
        ਲਈ (int32_t i=ਸੂਚਕਾਂਕ; i<count-1; ++i) {
            elements[i] = elements[i+1];
        }
        --count;
    }
    /* else index out of range */
    ਵਾਪਸ e;
}

/**
 * Insert the given object into this vector at its sorted position
 * as defined by 'compare'.  The current elements are assumed to
 * be sorted already.
 */
ਖਾਲੀ UVector::sorted_ਪਾਓ(ਖਾਲੀ* obj, UElementComparator *compare, UError_ਕੋਡ& ec) {
    UElement e;
    e.ਸੂਚਕ = obj;
    sorted_ਪਾਓ(e, compare, ec);
}

/**
 * Insert the given integer into this vector at its sorted position
 * as defined by 'compare'.  The current elements are assumed to
 * be sorted already.
 */
ਖਾਲੀ UVector::sorted_ਪਾਓ(int32_t obj, UElementComparator *compare, UError_ਕੋਡ& ec) {
    U_ASSERT(deleter == ਖਾਲੀ_ਸੂਚਕ);
    UElement e {};
    e.integer = obj;
    sorted_ਪਾਓ(e, compare, ec);
}

// ASSUME elements[] IS CURRENTLY SORTED
ਖਾਲੀ UVector::sorted_ਪਾਓ(UElement e, UElementComparator *compare, UError_ਕੋਡ& ec) {
    // Perform a binary search for the location to insert tok at.  Tok
    // will be inserted between two elements a and b such that a <=
    // tok && tok < b, where there is a 'virtual' elements[-1] always
    // less than tok and a 'virtual' elements[count] always greater
    // than tok.
    ਜੇ (!ensure_ਸਮਰੱਥਾ(count + 1, ec)) {
        ਜੇ (deleter != ਖਾਲੀ_ਸੂਚਕ) {
            (*deleter)(e.ਸੂਚਕ);
        }
        ਵਾਪਸ;
    }
    int32_t min = 0, max = count;
    ਜਦੋਂ (min != max) {
        int32_t probe = (min + max) / 2;
        int32_t c = (*compare)(elements[probe], e);
        ਜੇ (c > 0) {
            max = probe;
        } ਨਹੀਂਤਾਂ {
            // assert(c <= 0);
            min = probe + 1;
        }
    }
    ਲਈ (int32_t i=count; i>min; --i) {
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
ਸਥਿਰ int32_t U_CALLCONV
ਛਾਂਟੋ_Comparator(ਅਚਲ ਖਾਲੀ *context, ਅਚਲ ਖਾਲੀ *ਖੱਬੇ, ਅਚਲ ਖਾਲੀ *ਸੱਜੇ) {
    UElementComparator *compare = *static_cast<UElementComparator * ਅਚਲ *>(context);
    UElement e1 = *static_cast<ਅਚਲ UElement *>(ਖੱਬੇ);
    UElement e2 = *static_cast<ਅਚਲ UElement *>(ਸੱਜੇ);
    int32_t ਨਤੀਜਾ = (*compare)(e1, e2);
    ਵਾਪਸ ਨਤੀਜਾ;
}


/**
  *  Array sort comparison function for use from UVector::sorti()
  *  Compares int32_t vector elements.
  */
ਸਥਿਰ int32_t U_CALLCONV
sortiComparator(ਅਚਲ ਖਾਲੀ * /*context */, ਅਚਲ ਖਾਲੀ *ਖੱਬੇ, ਅਚਲ ਖਾਲੀ *ਸੱਜੇ) {
    ਅਚਲ UElement *e1 = static_cast<ਅਚਲ UElement *>(ਖੱਬੇ);
    ਅਚਲ UElement *e2 = static_cast<ਅਚਲ UElement *>(ਸੱਜੇ);
    int32_t ਨਤੀਜਾ = e1->integer < e2->integer? -1 :
                     e1->integer == e2->integer? 0 : 1;
    ਵਾਪਸ ਨਤੀਜਾ;
}

/**
  * Sort the vector, assuming it contains ints.
  *     (A more general sort would take a comparison function, but it's
  *     not clear whether UVector's UElementComparator or
  *     UComparator from uprv_sortAray would be more appropriate.)
  */
ਖਾਲੀ UVector::sorti(UError_ਕੋਡ &ec) {
    ਜੇ (U_SUCCESS(ec)) {
        uprv_ਛਾਂਟੋ_Array(elements, count, ਆਕਾਰ(UElement),
                       sortiComparator, ਖਾਲੀ_ਸੂਚਕ,  ਝੂਠ, &ec);
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
ਖਾਲੀ UVector::sort(UElementComparator *compare, UError_ਕੋਡ &ec) {
    ਜੇ (U_SUCCESS(ec)) {
        uprv_ਛਾਂਟੋ_Array(elements, count, ਆਕਾਰ(UElement),
                       ਛਾਂਟੋ_Comparator, &compare, ਝੂਠ, &ec);
    }
}


/**
 *  Stable sort with a user supplied comparator of type UComparator.
 */
ਖਾਲੀ UVector::ਛਾਂਟੋ_With_UComparator(UComparator *compare, ਅਚਲ ਖਾਲੀ *context, UError_ਕੋਡ &ec) {
    ਜੇ (U_SUCCESS(ec)) {
        uprv_ਛਾਂਟੋ_Array(elements, count, ਆਕਾਰ(UElement),
                       compare, context, ਸੱਚ, &ec);
    }
}

U_NAMESPACE_ਸਿਰਾ

