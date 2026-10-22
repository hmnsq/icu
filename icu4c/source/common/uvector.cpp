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

#ಅನ್ತರ್ಭಾವ "uvector.h"
#ಅನ್ತರ್ಭಾವ "cmemory.h"
#ಅನ್ತರ್ಭಾವ "uarrsort.h"
#ಅನ್ತರ್ಭಾವ "uelement.h"

U_NAMESPACE_BEGIN

constexpr int32_t DEFAULT_ಸಾಮರ್ಥ್ಯ = 8;

/*
 * Constants for hinting whether a key is an integer
 * or a pointer.  If a hint bit is zero, then the associated
 * token is assumed to be an integer. This is needed for iSeries
 */
constexpr int8_t HINT_ಕೀಲಿ_ಸೂಚಕ = 1;
constexpr int8_t HINT_ಕೀಲಿ_INTEGER = 0;
 
UOBJECT_DEFINE_RTTI_IMPLEMENTATION(UVector)

UVector::UVector(UError_ಸಂಕೇತ &ಸ್ಥಿತಿ_2) :
        UVector(ಶೂನ್ಯಸೂಚಕ, ಶೂನ್ಯಸೂಚಕ, DEFAULT_ಸಾಮರ್ಥ್ಯ, ಸ್ಥಿತಿ_2) {
}

UVector::UVector(int32_t initial_ಸಾಮರ್ಥ್ಯ, UError_ಸಂಕೇತ &ಸ್ಥಿತಿ_2) :
        UVector(ಶೂನ್ಯಸೂಚಕ, ಶೂನ್ಯಸೂಚಕ, initial_ಸಾಮರ್ಥ್ಯ, ಸ್ಥಿತಿ_2) {
}

UVector::UVector(UObjectDeleter *d, UElementsAreEqual *c, UError_ಸಂಕೇತ &ಸ್ಥಿತಿ_2) :
        UVector(d, c, DEFAULT_ಸಾಮರ್ಥ್ಯ, ಸ್ಥಿತಿ_2) {
}

UVector::UVector(UObjectDeleter *d, UElementsAreEqual *c, int32_t initial_ಸಾಮರ್ಥ್ಯ, UError_ಸಂಕೇತ &ಸ್ಥಿತಿ_2) :
    deleter(d),
    comparer(c)
{
    ಆದರೆ (U_FAILURE(ಸ್ಥಿತಿ_2)) {
        ಹಿಂತಿರುಗಿ;
    }
    // Fix bogus initialCapacity values; avoid malloc(0) and integer overflow
    ಆದರೆ ((initial_ಸಾಮರ್ಥ್ಯ < 1) || (initial_ಸಾಮರ್ಥ್ಯ > static_cast<int32_t>(INT32_MAX / ಗಾತ್ರ(UElement)))) {
        initial_ಸಾಮರ್ಥ್ಯ = DEFAULT_ಸಾಮರ್ಥ್ಯ;
    }
    elements = static_cast<UElement*>(uprv_malloc(ಗಾತ್ರ(UElement) * initial_ಸಾಮರ್ಥ್ಯ));
    ಆದರೆ (elements == ಶೂನ್ಯಸೂಚಕ) {
        ಸ್ಥಿತಿ_2 = U_ಸ್ಮರಣೆ_ಹಂಚಿಕೆ_ERROR;
    } ಇಲ್ಲದಿದ್ದರೆ {
        capacity = initial_ಸಾಮರ್ಥ್ಯ;
    }
}

UVector::~UVector() {
    removeAllElements();
    uprv_free(elements);
    elements = ಶೂನ್ಯಸೂಚಕ;
}

/**
 * Assign this object to another (make this a copy of 'other').
 * Use the 'assign' function to assign each element.
 */
ಶೂನ್ಯ UVector::assign(ಸ್ಥಿರಾಂಕ UVector& other, UElementAssigner *assign, UError_ಸಂಕೇತ &ec) {
    ಆದರೆ (ensure_ಸಾಮರ್ಥ್ಯ(other.count, ec)) {
        ಗಣ_ಗಾತ್ರ(other.count, ec);
        ಆದರೆ (U_SUCCESS(ec)) {
            ಗಾಗಿ (int32_t i=0; i<other.count; ++i) {
                ಆದರೆ (elements[i].ಸೂಚಕ != ಶೂನ್ಯಸೂಚಕ && deleter != ಶೂನ್ಯಸೂಚಕ) {
                    (*deleter)(elements[i].ಸೂಚಕ);
                }
                (*assign)(&elements[i], &other.elements[i]);
            }
        }
    }
}

// This only does something sensible if this object has a non-null comparer
ತಾರ್ಕಿಕ UVector::ಕ್ರಿಯಾಕಾರಕ==(ಸ್ಥಿರಾಂಕ UVector& other) ಸ್ಥಿರಾಂಕ {
    U_ASSERT(comparer != ಶೂನ್ಯಸೂಚಕ);
    ಆದರೆ (count != other.count) ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
    ಆದರೆ (comparer != ಶೂನ್ಯಸೂಚಕ) {
        // Compare using this object's comparer
        ಗಾಗಿ (int32_t i=0; i<count; ++i) {
            ಆದರೆ (!(*comparer)(elements[i], other.elements[i])) {
                ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
            }
        }
    }
    ಹಿಂತಿರುಗಿ ನಿಜ;
}

ಶೂನ್ಯ UVector::ಕೂಡಿಸು_Element(ಶೂನ್ಯ* obj, UError_ಸಂಕೇತ &ಸ್ಥಿತಿ_2) {
    U_ASSERT(deleter == ಶೂನ್ಯಸೂಚಕ);
    ಆದರೆ (ensure_ಸಾಮರ್ಥ್ಯ(count + 1, ಸ್ಥಿತಿ_2)) {
        elements[count++].ಸೂಚಕ = obj;
    }
}

ಶೂನ್ಯ UVector::adoptElement(ಶೂನ್ಯ* obj, UError_ಸಂಕೇತ &ಸ್ಥಿತಿ_2) {
    U_ASSERT(deleter != ಶೂನ್ಯಸೂಚಕ);
    ಆದರೆ (ensure_ಸಾಮರ್ಥ್ಯ(count + 1, ಸ್ಥಿತಿ_2)) {
        elements[count++].ಸೂಚಕ = obj;
    } ಇಲ್ಲದಿದ್ದರೆ {
        (*deleter)(obj);
    }
}
ಶೂನ್ಯ UVector::ಕೂಡಿಸು_Element(int32_t elem, UError_ಸಂಕೇತ &ಸ್ಥಿತಿ_2) {
    U_ASSERT(deleter == ಶೂನ್ಯಸೂಚಕ);  // Usage error. Mixing up ints and pointers.
    ಆದರೆ (ensure_ಸಾಮರ್ಥ್ಯ(count + 1, ಸ್ಥಿತಿ_2)) {
        elements[count].ಸೂಚಕ = ಶೂನ್ಯಸೂಚಕ;     // Pointers may be bigger than ints.
        elements[count].integer = elem;
        count++;
    }
}

ಶೂನ್ಯ UVector::ಗಣ_Element_At(ಶೂನ್ಯ* obj, int32_t ಸೂಚ್ಯಂಕ) {
    ಆದರೆ (0 <= ಸೂಚ್ಯಂಕ && ಸೂಚ್ಯಂಕ < count) {
        ಆದರೆ (elements[ಸೂಚ್ಯಂಕ].ಸೂಚಕ != ಶೂನ್ಯಸೂಚಕ && deleter != ಶೂನ್ಯಸೂಚಕ) {
            (*deleter)(elements[ಸೂಚ್ಯಂಕ].ಸೂಚಕ);
        }
        elements[ಸೂಚ್ಯಂಕ].ಸೂಚಕ = obj;
    } ಇಲ್ಲದಿದ್ದರೆ {
        /* index out of range */
        ಆದರೆ (deleter != ಶೂನ್ಯಸೂಚಕ) {
            (*deleter)(obj);
        }
    }
}

ಶೂನ್ಯ UVector::ಗಣ_Element_At(int32_t elem, int32_t ಸೂಚ್ಯಂಕ) {
    U_ASSERT(deleter == ಶೂನ್ಯಸೂಚಕ);  // Usage error. Mixing up ints and pointers.
    ಆದರೆ (0 <= ಸೂಚ್ಯಂಕ && ಸೂಚ್ಯಂಕ < count) {
        elements[ಸೂಚ್ಯಂಕ].ಸೂಚಕ = ಶೂನ್ಯಸೂಚಕ;
        elements[ಸೂಚ್ಯಂಕ].integer = elem;
    }
    /* else index out of range */
}

ಶೂನ್ಯ UVector::ಸೇರಿಸು_Element_At(ಶೂನ್ಯ* obj, int32_t ಸೂಚ್ಯಂಕ, UError_ಸಂಕೇತ &ಸ್ಥಿತಿ_2) {
    ಆದರೆ (ensure_ಸಾಮರ್ಥ್ಯ(count + 1, ಸ್ಥಿತಿ_2)) {
        ಆದರೆ (0 <= ಸೂಚ್ಯಂಕ && ಸೂಚ್ಯಂಕ <= count) {
            ಗಾಗಿ (int32_t i=count; i>ಸೂಚ್ಯಂಕ; --i) {
                elements[i] = elements[i-1];
            }
            elements[ಸೂಚ್ಯಂಕ].ಸೂಚಕ = obj;
            ++count;
        } ಇಲ್ಲದಿದ್ದರೆ {
            /* index out of range */
            ಸ್ಥಿತಿ_2 = U_ILLEGAL_ವಾದ_ERROR;
        }
    }
    ಆದರೆ (U_FAILURE(ಸ್ಥಿತಿ_2) && deleter != ಶೂನ್ಯಸೂಚಕ) {
        (*deleter)(obj);
    }
}

ಶೂನ್ಯ UVector::ಸೇರಿಸು_Element_At(int32_t elem, int32_t ಸೂಚ್ಯಂಕ, UError_ಸಂಕೇತ &ಸ್ಥಿತಿ_2) {
    U_ASSERT(deleter == ಶೂನ್ಯಸೂಚಕ);  // Usage error. Mixing up ints and pointers.
    // must have 0 <= index <= count
    ಆದರೆ (ensure_ಸಾಮರ್ಥ್ಯ(count + 1, ಸ್ಥಿತಿ_2)) {
        ಆದರೆ (0 <= ಸೂಚ್ಯಂಕ && ಸೂಚ್ಯಂಕ <= count) {
            ಗಾಗಿ (int32_t i=count; i>ಸೂಚ್ಯಂಕ; --i) {
                elements[i] = elements[i-1];
            }
            elements[ಸೂಚ್ಯಂಕ].ಸೂಚಕ = ಶೂನ್ಯಸೂಚಕ;
            elements[ಸೂಚ್ಯಂಕ].integer = elem;
            ++count;
        } ಇಲ್ಲದಿದ್ದರೆ {
            /* index out of range */
            ಸ್ಥಿತಿ_2 = U_ILLEGAL_ವಾದ_ERROR;
        }
    }
}

ಶೂನ್ಯ* UVector::elementAt(int32_t ಸೂಚ್ಯಂಕ) ಸ್ಥಿರಾಂಕ {
    ಹಿಂತಿರುಗಿ (0 <= ಸೂಚ್ಯಂಕ && ಸೂಚ್ಯಂಕ < count) ? elements[ಸೂಚ್ಯಂಕ].ಸೂಚಕ : ಶೂನ್ಯಸೂಚಕ;
}

int32_t UVector::elementAti(int32_t ಸೂಚ್ಯಂಕ) ಸ್ಥಿರಾಂಕ {
    ಹಿಂತಿರುಗಿ (0 <= ಸೂಚ್ಯಂಕ && ಸೂಚ್ಯಂಕ < count) ? elements[ಸೂಚ್ಯಂಕ].integer : 0;
}

UBool UVector::ಒಳಗೊಂಡಿದೆ_All(ಸ್ಥಿರಾಂಕ UVector& other) ಸ್ಥಿರಾಂಕ {
    ಗಾಗಿ (int32_t i=0; i<other.size(); ++i) {
        ಆದರೆ (ಸೂಚ್ಯಂಕ_ದ(other.elements[i]) < 0) {
            ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
        }
    }
    ಹಿಂತಿರುಗಿ ನಿಜ;
}

UBool UVector::ಒಳಗೊಂಡಿದೆ_None(ಸ್ಥಿರಾಂಕ UVector& other) ಸ್ಥಿರಾಂಕ {
    ಗಾಗಿ (int32_t i=0; i<other.size(); ++i) {
        ಆದರೆ (ಸೂಚ್ಯಂಕ_ದ(other.elements[i]) >= 0) {
            ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
        }
    }
    ಹಿಂತಿರುಗಿ ನಿಜ;
}

UBool UVector::removeAll(ಸ್ಥಿರಾಂಕ UVector& other) {
    UBool ಬದಲಾಯಿತು = ಸುಳ್ಳು;
    ಗಾಗಿ (int32_t i=0; i<other.size(); ++i) {
        int32_t j = ಸೂಚ್ಯಂಕ_ದ(other.elements[i]);
        ಆದರೆ (j >= 0) {
            removeElementAt(j);
            ಬದಲಾಯಿತು = ನಿಜ;
        }
    }
    ಹಿಂತಿರುಗಿ ಬದಲಾಯಿತು;
}

UBool UVector::retainAll(ಸ್ಥಿರಾಂಕ UVector& other) {
    UBool ಬದಲಾಯಿತು = ಸುಳ್ಳು;
    ಗಾಗಿ (int32_t j=size()-1; j>=0; --j) {
        int32_t i = other.ಸೂಚ್ಯಂಕ_ದ(elements[j]);
        ಆದರೆ (i < 0) {
            removeElementAt(j);
            ಬದಲಾಯಿತು = ನಿಜ;
        }
    }
    ಹಿಂತಿರುಗಿ ಬದಲಾಯಿತು;
}

ಶೂನ್ಯ UVector::removeElementAt(int32_t ಸೂಚ್ಯಂಕ) {
    ಶೂನ್ಯ* e = orphanElementAt(ಸೂಚ್ಯಂಕ);
    ಆದರೆ (e != ಶೂನ್ಯಸೂಚಕ && deleter != ಶೂನ್ಯಸೂಚಕ) {
        (*deleter)(e);
    }
}

UBool UVector::removeElement(ಶೂನ್ಯ* obj) {
    int32_t i = ಸೂಚ್ಯಂಕ_ದ(obj);
    ಆದರೆ (i >= 0) {
        removeElementAt(i);
        ಹಿಂತಿರುಗಿ ನಿಜ;
    }
    ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
}

ಶೂನ್ಯ UVector::removeAllElements() {
    ಆದರೆ (deleter != ಶೂನ್ಯಸೂಚಕ) {
        ಗಾಗಿ (int32_t i=0; i<count; ++i) {
            ಆದರೆ (elements[i].ಸೂಚಕ != ಶೂನ್ಯಸೂಚಕ) {
                (*deleter)(elements[i].ಸೂಚಕ);
            }
        }
    }
    count = 0;
}

UBool   UVector::equals(ಸ್ಥಿರಾಂಕ UVector &other) ಸ್ಥಿರಾಂಕ {
    ಪೂರ್ಣಾಂಕ      i;

    ಆದರೆ (ಇದು->count != other.count) {
        ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
    }
    ಆದರೆ (comparer == ಶೂನ್ಯಸೂಚಕ) {
        ಗಾಗಿ (i=0; i<count; i++) {
            ಆದರೆ (elements[i].ಸೂಚಕ != other.elements[i].ಸೂಚಕ) {
                ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
            }
        }
    } ಇಲ್ಲದಿದ್ದರೆ {
        UElement ಕೀಲಿ;
        ಗಾಗಿ (i=0; i<count; i++) {
            ಕೀಲಿ.ಸೂಚಕ = &other.elements[i];
            ಆದರೆ (!(*comparer)(ಕೀಲಿ, elements[i])) {
                ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
            }
        }
    }
    ಹಿಂತಿರುಗಿ ನಿಜ;
}



int32_t UVector::ಸೂಚ್ಯಂಕ_ದ(ಶೂನ್ಯ* obj, int32_t ಆರಂಭ_ಸೂಚ್ಯಂಕ) ಸ್ಥಿರಾಂಕ {
    UElement ಕೀಲಿ;
    ಕೀಲಿ.ಸೂಚಕ = obj;
    ಹಿಂತಿರುಗಿ ಸೂಚ್ಯಂಕ_ದ(ಕೀಲಿ, ಆರಂಭ_ಸೂಚ್ಯಂಕ, HINT_ಕೀಲಿ_ಸೂಚಕ);
}

int32_t UVector::ಸೂಚ್ಯಂಕ_ದ(int32_t obj, int32_t ಆರಂಭ_ಸೂಚ್ಯಂಕ) ಸ್ಥಿರಾಂಕ {
    UElement ಕೀಲಿ;
    ಕೀಲಿ.integer = obj;
    ಹಿಂತಿರುಗಿ ಸೂಚ್ಯಂಕ_ದ(ಕೀಲಿ, ಆರಂಭ_ಸೂಚ್ಯಂಕ, HINT_ಕೀಲಿ_INTEGER);
}

int32_t UVector::ಸೂಚ್ಯಂಕ_ದ(UElement ಕೀಲಿ, int32_t ಆರಂಭ_ಸೂಚ್ಯಂಕ, int8_t hint) ಸ್ಥಿರಾಂಕ {
    ಆದರೆ (comparer != ಶೂನ್ಯಸೂಚಕ) {
        ಗಾಗಿ (int32_t i=ಆರಂಭ_ಸೂಚ್ಯಂಕ; i<count; ++i) {
            ಆದರೆ ((*comparer)(ಕೀಲಿ, elements[i])) {
                ಹಿಂತಿರುಗಿ i;
            }
        }
    } ಇಲ್ಲದಿದ್ದರೆ {
        ಗಾಗಿ (int32_t i=ಆರಂಭ_ಸೂಚ್ಯಂಕ; i<count; ++i) {
            /* Pointers are not always the same size as ints so to perform
             * a valid comparison we need to know whether we are being
             * provided an int or a pointer. */
            ಆದರೆ (hint & HINT_ಕೀಲಿ_ಸೂಚಕ) {
                ಆದರೆ (ಕೀಲಿ.ಸೂಚಕ == elements[i].ಸೂಚಕ) {
                    ಹಿಂತಿರುಗಿ i;
                }
            } ಇಲ್ಲದಿದ್ದರೆ {
                ಆದರೆ (ಕೀಲಿ.integer == elements[i].integer) {
                    ಹಿಂತಿರುಗಿ i;
                }
            }
        }
    }
    ಹಿಂತಿರುಗಿ -1;
}

UBool UVector::ensure_ಸಾಮರ್ಥ್ಯ(int32_t minimum_ಸಾಮರ್ಥ್ಯ, UError_ಸಂಕೇತ &ಸ್ಥಿತಿ_2) {
    ಆದರೆ (U_FAILURE(ಸ್ಥಿತಿ_2)) {
        ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
    }
    ಆದರೆ (minimum_ಸಾಮರ್ಥ್ಯ < 0) {
        ಸ್ಥಿತಿ_2 = U_ILLEGAL_ವಾದ_ERROR;
        ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
    }
    ಆದರೆ (capacity < minimum_ಸಾಮರ್ಥ್ಯ) {
        ಆದರೆ (capacity > (INT32_MAX - 1) / 2) {        	// integer overflow check
            ಸ್ಥಿತಿ_2 = U_ILLEGAL_ವಾದ_ERROR;
            ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
        }
        int32_t newCap = capacity * 2;
        ಆದರೆ (newCap < minimum_ಸಾಮರ್ಥ್ಯ) {
            newCap = minimum_ಸಾಮರ್ಥ್ಯ;
        }
        ಆದರೆ (newCap > static_cast<int32_t>(INT32_MAX / ಗಾತ್ರ(UElement))) { // integer overflow check
            // We keep the original memory contents on bad minimumCapacity.
            ಸ್ಥಿತಿ_2 = U_ILLEGAL_ವಾದ_ERROR;
            ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
        }
        UElement* newElems = static_cast<UElement*>(uprv_realloc(elements, ಗಾತ್ರ(UElement) * newCap));
        ಆದರೆ (newElems == ಶೂನ್ಯಸೂಚಕ) {
            // We keep the original contents on the memory failure on realloc or bad minimumCapacity.
            ಸ್ಥಿತಿ_2 = U_ಸ್ಮರಣೆ_ಹಂಚಿಕೆ_ERROR;
            ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
        }
        elements = newElems;
        capacity = newCap;
    }
    ಹಿಂತಿರುಗಿ ನಿಜ;
}

/**
 * Change the size of this vector as follows: If newSize is smaller,
 * then truncate the array, possibly deleting held elements for i >=
 * newSize.  If newSize is larger, grow the array, filling in new
 * slots with nullptr.
 */
ಶೂನ್ಯ UVector::ಗಣ_ಗಾತ್ರ(int32_t new_ಗಾತ್ರ, UError_ಸಂಕೇತ &ಸ್ಥಿತಿ_2) {
    ಆದರೆ (!ensure_ಸಾಮರ್ಥ್ಯ(new_ಗಾತ್ರ, ಸ್ಥಿತಿ_2)) {
        ಹಿಂತಿರುಗಿ;
    }
    ಆದರೆ (new_ಗಾತ್ರ > count) {
        UElement empty;
        empty.ಸೂಚಕ = ಶೂನ್ಯಸೂಚಕ;
        empty.integer = 0;
        ಗಾಗಿ (int32_t i=count; i<new_ಗಾತ್ರ; ++i) {
            elements[i] = empty;
        }
    } ಇಲ್ಲದಿದ್ದರೆ {
        /* Most efficient to count down */
        ಗಾಗಿ (int32_t i=count-1; i>=new_ಗಾತ್ರ; --i) {
            removeElementAt(i);
        }
    }
    count = new_ಗಾತ್ರ;
}

/**
 * Fill in the given array with all elements of this vector.
 */
ಶೂನ್ಯ** UVector::ಗಮ್ಯ_Array(ಶೂನ್ಯ** ಫಲಿತಾಂಶ) ಸ್ಥಿರಾಂಕ {
    ಶೂನ್ಯ** a = ಫಲಿತಾಂಶ;
    ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i=0; i<count; ++i) {
        *a++ = elements[i].ಸೂಚಕ;
    }
    ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

UObjectDeleter *UVector::ಗಣ_Deleter(UObjectDeleter *d) {
    UObjectDeleter *old = deleter;
    deleter = d;
    ಹಿಂತಿರುಗಿ old;
}

UElementsAreEqual *UVector::ಗಣ_Comparer(UElementsAreEqual *d) {
    UElementsAreEqual *old = comparer;
    comparer = d;
    ಹಿಂತಿರುಗಿ old;
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
ಶೂನ್ಯ* UVector::orphanElementAt(int32_t ಸೂಚ್ಯಂಕ) {
    ಶೂನ್ಯ* e = ಶೂನ್ಯಸೂಚಕ;
    ಆದರೆ (0 <= ಸೂಚ್ಯಂಕ && ಸೂಚ್ಯಂಕ < count) {
        e = elements[ಸೂಚ್ಯಂಕ].ಸೂಚಕ;
        ಗಾಗಿ (int32_t i=ಸೂಚ್ಯಂಕ; i<count-1; ++i) {
            elements[i] = elements[i+1];
        }
        --count;
    }
    /* else index out of range */
    ಹಿಂತಿರುಗಿ e;
}

/**
 * Insert the given object into this vector at its sorted position
 * as defined by 'compare'.  The current elements are assumed to
 * be sorted already.
 */
ಶೂನ್ಯ UVector::sorted_ಸೇರಿಸು(ಶೂನ್ಯ* obj, UElementComparator *compare, UError_ಸಂಕೇತ& ec) {
    UElement e;
    e.ಸೂಚಕ = obj;
    sorted_ಸೇರಿಸು(e, compare, ec);
}

/**
 * Insert the given integer into this vector at its sorted position
 * as defined by 'compare'.  The current elements are assumed to
 * be sorted already.
 */
ಶೂನ್ಯ UVector::sorted_ಸೇರಿಸು(int32_t obj, UElementComparator *compare, UError_ಸಂಕೇತ& ec) {
    U_ASSERT(deleter == ಶೂನ್ಯಸೂಚಕ);
    UElement e {};
    e.integer = obj;
    sorted_ಸೇರಿಸು(e, compare, ec);
}

// ASSUME elements[] IS CURRENTLY SORTED
ಶೂನ್ಯ UVector::sorted_ಸೇರಿಸು(UElement e, UElementComparator *compare, UError_ಸಂಕೇತ& ec) {
    // Perform a binary search for the location to insert tok at.  Tok
    // will be inserted between two elements a and b such that a <=
    // tok && tok < b, where there is a 'virtual' elements[-1] always
    // less than tok and a 'virtual' elements[count] always greater
    // than tok.
    ಆದರೆ (!ensure_ಸಾಮರ್ಥ್ಯ(count + 1, ec)) {
        ಆದರೆ (deleter != ಶೂನ್ಯಸೂಚಕ) {
            (*deleter)(e.ಸೂಚಕ);
        }
        ಹಿಂತಿರುಗಿ;
    }
    int32_t min = 0, max = count;
    ಆಗ (min != max) {
        int32_t probe = (min + max) / 2;
        int32_t c = (*compare)(elements[probe], e);
        ಆದರೆ (c > 0) {
            max = probe;
        } ಇಲ್ಲದಿದ್ದರೆ {
            // assert(c <= 0);
            min = probe + 1;
        }
    }
    ಗಾಗಿ (int32_t i=count; i>min; --i) {
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
ಸ್ಥಿರ int32_t U_CALLCONV
ವಿಂಗಡಿಸು_Comparator(ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *context, ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *ಎಡ, ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *ಬಲ) {
    UElementComparator *compare = *static_cast<UElementComparator * ಸ್ಥಿರಾಂಕ *>(context);
    UElement e1 = *static_cast<ಸ್ಥಿರಾಂಕ UElement *>(ಎಡ);
    UElement e2 = *static_cast<ಸ್ಥಿರಾಂಕ UElement *>(ಬಲ);
    int32_t ಫಲಿತಾಂಶ = (*compare)(e1, e2);
    ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}


/**
  *  Array sort comparison function for use from UVector::sorti()
  *  Compares int32_t vector elements.
  */
ಸ್ಥಿರ int32_t U_CALLCONV
sortiComparator(ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ * /*context */, ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *ಎಡ, ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *ಬಲ) {
    ಸ್ಥಿರಾಂಕ UElement *e1 = static_cast<ಸ್ಥಿರಾಂಕ UElement *>(ಎಡ);
    ಸ್ಥಿರಾಂಕ UElement *e2 = static_cast<ಸ್ಥಿರಾಂಕ UElement *>(ಬಲ);
    int32_t ಫಲಿತಾಂಶ = e1->integer < e2->integer? -1 :
                     e1->integer == e2->integer? 0 : 1;
    ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

/**
  * Sort the vector, assuming it contains ints.
  *     (A more general sort would take a comparison function, but it's
  *     not clear whether UVector's UElementComparator or
  *     UComparator from uprv_sortAray would be more appropriate.)
  */
ಶೂನ್ಯ UVector::sorti(UError_ಸಂಕೇತ &ec) {
    ಆದರೆ (U_SUCCESS(ec)) {
        uprv_ವಿಂಗಡಿಸು_Array(elements, count, ಗಾತ್ರ(UElement),
                       sortiComparator, ಶೂನ್ಯಸೂಚಕ,  ಸುಳ್ಳು, &ec);
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
ಶೂನ್ಯ UVector::sort(UElementComparator *compare, UError_ಸಂಕೇತ &ec) {
    ಆದರೆ (U_SUCCESS(ec)) {
        uprv_ವಿಂಗಡಿಸು_Array(elements, count, ಗಾತ್ರ(UElement),
                       ವಿಂಗಡಿಸು_Comparator, &compare, ಸುಳ್ಳು, &ec);
    }
}


/**
 *  Stable sort with a user supplied comparator of type UComparator.
 */
ಶೂನ್ಯ UVector::ವಿಂಗಡಿಸು_With_UComparator(UComparator *compare, ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *context, UError_ಸಂಕೇತ &ec) {
    ಆದರೆ (U_SUCCESS(ec)) {
        uprv_ವಿಂಗಡಿಸು_Array(elements, count, ಗಾತ್ರ(UElement),
                       compare, context, ನಿಜ, &ec);
    }
}

U_NAMESPACE_ಕೊನೆ

