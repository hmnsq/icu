// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
**********************************************************************
*   Copyright (C) 1999-2014, International Business Machines
*   Corporation and others.  All Rights Reserved.
**********************************************************************
*/

//
//  UVector64 is a class implementing a vector of 64 bit integers.
//            It is similar to UVector32, but holds int64_t values rather than int32_t.
//            Most of the code is unchanged from UVector.
//

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ UVECTOR64_H
#ವ್ಯಾಖ್ಯಾನಿಸು UVECTOR64_H

#ಅನ್ತರ್ಭಾವ "unicode/utypes.h"
#ಅನ್ತರ್ಭಾವ "unicode/uobject.h"
#ಅನ್ತರ್ಭಾವ "uhash.h"
#ಅನ್ತರ್ಭಾವ "uassert.h"

U_NAMESPACE_BEGIN



/**
 * <p>Ultralightweight C++ implementation of an <tt>int64_t</tt> vector
 * that has a subset of methods from UVector32
 *
 * <p>This is a very simple implementation, written to satisfy an
 * immediate porting need.  As such, it is not completely fleshed out,
 * and it aims for simplicity and conformity.  Nonetheless, it serves
 * its purpose (porting code from java that uses java.util.Vector)
 * well, and it could be easily made into a more robust vector class.
 *
 * <p><b>Design notes</b>
 *
 * <p>There is index bounds checking, but little is done about it.  If
 * indices are out of bounds, either nothing happens, or zero is
 * returned.  We <em>do</em> avoid indexing off into the weeds.
 *
 * <p>There is detection of out of memory, but the handling is very
 * coarse-grained -- similar to UnicodeString's protocol, but even
 * coarser.  The class contains <em>one static flag</em> that is set
 * when any call to <tt>new</tt> returns zero.  This allows the caller
 * to use several vectors and make just one check at the end to see if
 * a memory failure occurred.  This is more efficient than making a
 * check after each call on each vector when doing many operations on
 * multiple vectors.  The single static flag works best when memory
 * failures are infrequent, and when recovery options are limited or
 * nonexistent.
 *
 * <p><b>To do</b>
 *
 * <p>Improve the handling of index out of bounds errors.
 *
 */
class U_COMMON_API UVector64 : ಸಾರ್ವಜನಿಕ UObject {
ಖಾಸಗಿ:
    int32_t   ಎಣಿಕೆ_2;

    int32_t   ಸಾಮರ್ಥ್ಯ;
    
    int32_t   max_ಸಾಮರ್ಥ್ಯ;   // Limit beyond which capacity is not permitted to grow.

    int64_t*  elements;

ಸಾರ್ವಜನಿಕ:
    UVector64(UError_ಸಂಕೇತ &ಸ್ಥಿತಿ_2);

    UVector64(int32_t initial_ಸಾಮರ್ಥ್ಯ, UError_ಸಂಕೇತ &ಸ್ಥಿತಿ_2);

    ವಾಸ್ತವಿಕ ~UVector64();

    /**
     * Assign this object to another (make this a copy of 'other').
     * Use the 'assign' function to assign each element.
     */
    ಶೂನ್ಯ ನಿಯೋಜಿಸು(ಸ್ಥಿರಾಂಕ UVector64& other, UError_ಸಂಕೇತ &ec);

    /**
     * Compare this vector with another.  They will be considered
     * equal if they are of the same size and all elements are equal,
     * as compared using this object's comparer.
     */
    bool ಕ್ರಿಯಾಕಾರಕ==(ಸ್ಥಿರಾಂಕ UVector64& other);

    /**
     * Equivalent to !operator==()
     */
    ಸಾಲೊಳಗೆ bool ಕ್ರಿಯಾಕಾರಕ!=(ಸ್ಥಿರಾಂಕ UVector64& other);

    //------------------------------------------------------------
    // subset of java.util.Vector API
    //------------------------------------------------------------

    ಸಾಲೊಳಗೆ ಶೂನ್ಯ ಕೂಡಿಸು_Element(int64_t elem, UError_ಸಂಕೇತ &ಸ್ಥಿತಿ_2);

    ಶೂನ್ಯ ಗಣ_Element_At(int64_t elem, int32_t ಸೂಚ್ಯಂಕ);

    ಶೂನ್ಯ ಸೇರಿಸು_Element_At(int64_t elem, int32_t ಸೂಚ್ಯಂಕ, UError_ಸಂಕೇತ &ಸ್ಥಿತಿ_2);
    
    ಸಾಲೊಳಗೆ int64_t elementAti(int32_t ಸೂಚ್ಯಂಕ) ಸ್ಥಿರಾಂಕ;

    //UBool equals(const UVector64 &other) const;

    ಸಾಲೊಳಗೆ int64_t ಕೊನೆಯ_Elementi() ಸ್ಥಿರಾಂಕ;

    //int32_t indexOf(int64_t elem, int32_t startIndex = 0) const;

    //UBool contains(int64_t elem) const;

    //UBool containsAll(const UVector64& other) const;

    //UBool removeAll(const UVector64& other);

    //UBool retainAll(const UVector64& other);

    //void removeElementAt(int32_t index);

    ಶೂನ್ಯ removeAllElements();

    ಸಾಲೊಳಗೆ int32_t ಗಾತ್ರ_3() ಸ್ಥಿರಾಂಕ;

    ಸಾಲೊಳಗೆ UBool is_ಖಾಲಿಗಳು() ಸ್ಥಿರಾಂಕ { ಹಿಂತಿರುಗಿ ಎಣಿಕೆ_2 == 0; }

    // Inline.  Use this one for speedy size check.
    ಸಾಲೊಳಗೆ UBool ensure_ಸಾಮರ್ಥ್ಯ(int32_t minimum_ಸಾಮರ್ಥ್ಯ, UError_ಸಂಕೇತ &ಸ್ಥಿತಿ_2);

    // Out-of-line, handles actual growth.  Called by ensureCapacity() when necessary.
    UBool expand_ಸಾಮರ್ಥ್ಯ(int32_t minimum_ಸಾಮರ್ಥ್ಯ, UError_ಸಂಕೇತ &ಸ್ಥಿತಿ_2);

    /**
     * Change the size of this vector as follows: If newSize is
     * smaller, then truncate the array, possibly deleting held
     * elements for i >= newSize.  If newSize is larger, grow the
     * array, filling in new slows with zero.
     */
    ಶೂನ್ಯ ಗಣ_ಗಾತ್ರ(int32_t new_ಗಾತ್ರ);

    //------------------------------------------------------------
    // New API
    //------------------------------------------------------------

    //UBool containsNone(const UVector64& other) const;


    //void sortedInsert(int64_t elem, UErrorCode& ec);

    /**
     * Returns a pointer to the internal array holding the vector.
     */
    ಸಾಲೊಳಗೆ int64_t *get_ಬಫರ್() ಸ್ಥಿರಾಂಕ;

    /**
     * Set the maximum allowed buffer capacity for this vector/stack.
     * Default with no limit set is unlimited, go until malloc() fails.
     * A Limit of zero means unlimited capacity.
     * Units are vector elements (64 bits each), not bytes.
     */
    ಶೂನ್ಯ ಗಣ_Max_ಸಾಮರ್ಥ್ಯ(int32_t ಎಲ್ಲೆ);

    /**
     * ICU "poor man's RTTI", returns a UClassID for this class.
     */
    ಸ್ಥಿರ UClassID U_EXPORT2 getStaticClassID();

    /**
     * ICU "poor man's RTTI", returns a UClassID for the actual class.
     */
    ವಾಸ್ತವಿಕ UClassID getDynamicClassID() ಸ್ಥಿರಾಂಕ override;

ಖಾಸಗಿ:
    ಶೂನ್ಯ _init(int32_t initial_ಸಾಮರ್ಥ್ಯ, UError_ಸಂಕೇತ &ಸ್ಥಿತಿ_2);

    // Disallow
    UVector64(ಸ್ಥಿರಾಂಕ UVector64&) = delete;

    // Disallow
    UVector64& ಕ್ರಿಯಾಕಾರಕ=(ಸ್ಥಿರಾಂಕ UVector64&) = delete;


    //  API Functions for Stack operations.
    //  In the original UVector, these were in a separate derived class, UStack.
    //  Here in UVector64, they are all together.
ಸಾರ್ವಜನಿಕ:
    //UBool empty() const;   // TODO:  redundant, same as empty().  Remove it?

    //int64_t peeki() const;
    
    ಸಾಲೊಳಗೆ int64_t popi();
    
    ಸಾಲೊಳಗೆ int64_t ತಳ್ಳು(int64_t i, UError_ಸಂಕೇತ &ಸ್ಥಿತಿ_2);

    ಸಾಲೊಳಗೆ int64_t *reserve_ಖಂಡ(int32_t ಗಾತ್ರ_3, UError_ಸಂಕೇತ &ಸ್ಥಿತಿ_2);
    ಸಾಲೊಳಗೆ int64_t *ತೆಗೆ_ಚೌಕಟ್ಟು(int32_t ಗಾತ್ರ_3);
};


// UVector64 inlines

ಸಾಲೊಳಗೆ UBool UVector64::ensure_ಸಾಮರ್ಥ್ಯ(int32_t minimum_ಸಾಮರ್ಥ್ಯ, UError_ಸಂಕೇತ &ಸ್ಥಿತಿ_2) {
    ಆದರೆ ((minimum_ಸಾಮರ್ಥ್ಯ >= 0) && (ಸಾಮರ್ಥ್ಯ >= minimum_ಸಾಮರ್ಥ್ಯ)) {
        ಹಿಂತಿರುಗಿ true;
    } ಇಲ್ಲದಿದ್ದರೆ {
        ಹಿಂತಿರುಗಿ expand_ಸಾಮರ್ಥ್ಯ(minimum_ಸಾಮರ್ಥ್ಯ, ಸ್ಥಿತಿ_2);
    }
}

ಸಾಲೊಳಗೆ int64_t UVector64::elementAti(int32_t ಸೂಚ್ಯಂಕ) ಸ್ಥಿರಾಂಕ {
    ಹಿಂತಿರುಗಿ (0 <= ಸೂಚ್ಯಂಕ && ಸೂಚ್ಯಂಕ < ಎಣಿಕೆ_2) ? elements[ಸೂಚ್ಯಂಕ] : 0;
}


ಸಾಲೊಳಗೆ ಶೂನ್ಯ UVector64::ಕೂಡಿಸು_Element(int64_t elem, UError_ಸಂಕೇತ &ಸ್ಥಿತಿ_2) {
    ಆದರೆ (ensure_ಸಾಮರ್ಥ್ಯ(ಎಣಿಕೆ_2 + 1, ಸ್ಥಿತಿ_2)) {
        elements[ಎಣಿಕೆ_2] = elem;
        ಎಣಿಕೆ_2++;
    }
}

ಸಾಲೊಳಗೆ int64_t *UVector64::reserve_ಖಂಡ(int32_t ಗಾತ್ರ_3, UError_ಸಂಕೇತ &ಸ್ಥಿತಿ_2) {
    ಆದರೆ (ensure_ಸಾಮರ್ಥ್ಯ(ಎಣಿಕೆ_2+ಗಾತ್ರ_3, ಸ್ಥಿತಿ_2) == false) {
        ಹಿಂತಿರುಗಿ nullptr;
    }
    int64_t  *rp = elements+ಎಣಿಕೆ_2;
    ಎಣಿಕೆ_2 += ಗಾತ್ರ_3;
    ಹಿಂತಿರುಗಿ rp;
}

ಸಾಲೊಳಗೆ int64_t *UVector64::ತೆಗೆ_ಚೌಕಟ್ಟು(int32_t ಗಾತ್ರ_3) {
    U_ASSERT(ಎಣಿಕೆ_2 >= ಗಾತ್ರ_3);
    ಎಣಿಕೆ_2 -= ಗಾತ್ರ_3;
    ಆದರೆ (ಎಣಿಕೆ_2 < 0) {
        ಎಣಿಕೆ_2 = 0;
    }
    ಹಿಂತಿರುಗಿ elements+ಎಣಿಕೆ_2-ಗಾತ್ರ_3;
}



ಸಾಲೊಳಗೆ int32_t UVector64::ಗಾತ್ರ_3() ಸ್ಥಿರಾಂಕ {
    ಹಿಂತಿರುಗಿ ಎಣಿಕೆ_2;
}

ಸಾಲೊಳಗೆ int64_t UVector64::ಕೊನೆಯ_Elementi() ಸ್ಥಿರಾಂಕ {
    ಹಿಂತಿರುಗಿ elementAti(ಎಣಿಕೆ_2-1);
}

ಸಾಲೊಳಗೆ bool UVector64::ಕ್ರಿಯಾಕಾರಕ!=(ಸ್ಥಿರಾಂಕ UVector64& other) {
    ಹಿಂತಿರುಗಿ !ಕ್ರಿಯಾಕಾರಕ==(other);
}

ಸಾಲೊಳಗೆ int64_t *UVector64::get_ಬಫರ್() ಸ್ಥಿರಾಂಕ {
    ಹಿಂತಿರುಗಿ elements;
}


// UStack inlines

ಸಾಲೊಳಗೆ int64_t UVector64::ತಳ್ಳು(int64_t i, UError_ಸಂಕೇತ &ಸ್ಥಿತಿ_2) {
    ಕೂಡಿಸು_Element(i, ಸ್ಥಿತಿ_2);
    ಹಿಂತಿರುಗಿ i;
}

ಸಾಲೊಳಗೆ int64_t UVector64::popi() {
    int64_t ಫಲಿತಾಂಶ = 0;
    ಆದರೆ (ಎಣಿಕೆ_2 > 0) {
        ಎಣಿಕೆ_2--;
        ಫಲಿತಾಂಶ = elements[ಎಣಿಕೆ_2];
    }
    ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

U_NAMESPACE_ಕೊನೆ

#ಆದರೆ_ಅಂತ್ಯ
