// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
**********************************************************************
*   Copyright (C) 1999-2011, International Business Machines
*   Corporation and others.  All Rights Reserved.
**********************************************************************
*/

//
//  UVector32 is a class implementing a vector of 32 bit integers.
//            It is similar to UVector, but holds int32_t values rather than pointers.
//            Most of the code is unchanged from UVector.
//

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ UVECTOR32_H
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ UVECTOR32_H

#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/utypes.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/uobject.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "uhash.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "uassert.h"

U_NAMESPACE_BEGIN



/**
 * <p>Ultralightweight C++ implementation of a <tt>void*</tt> vector
 * that is (mostly) compatible with java.util.Vector.
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
 * @author Alan Liu
 */
class U_COMMON_API UVector32 : ਜਨਤਕ UObject {
ਨਿਜੀ:
    int32_t   ਗਿਣਤੀ_2;

    int32_t   ਸਮਰੱਥਾ;
    
    int32_t   max_ਸਮਰੱਥਾ;   // Limit beyond which capacity is not permitted to grow.

    int32_t*  elements;

ਜਨਤਕ:
    UVector32(UError_ਕੋਡ &ਸਥਿਤੀ);

    UVector32(int32_t initial_ਸਮਰੱਥਾ, UError_ਕੋਡ &ਸਥਿਤੀ);

    ਆਭਾਸੀ ~UVector32();

    /**
     * Assign this object to another (make this a copy of 'other').
     * Use the 'assign' function to assign each element.
     */
    ਖਾਲੀ ਸੌਂਪੋ(ਅਚਲ UVector32& other, UError_ਕੋਡ &ec);

    /**
     * Compare this vector with another.  They will be considered
     * equal if they are of the same size and all elements are equal,
     * as compared using this object's comparer.
     */
    bool ਸੰਚਾਲਕ==(ਅਚਲ UVector32& other) ਅਚਲ;

    /**
     * Equivalent to !operator==()
     */
    ਸਤਰਅੰਦਰ bool ਸੰਚਾਲਕ!=(ਅਚਲ UVector32& other) ਅਚਲ;

    //------------------------------------------------------------
    // java.util.Vector API
    //------------------------------------------------------------

    ਸਤਰਅੰਦਰ ਖਾਲੀ ਜੋੜੋ_Element(int32_t elem, UError_ਕੋਡ &ਸਥਿਤੀ);

    ਖਾਲੀ ਸੈੱਟ_Element_At(int32_t elem, int32_t ਸੂਚਕਾਂਕ);

    ਖਾਲੀ ਪਾਓ_Element_At(int32_t elem, int32_t ਸੂਚਕਾਂਕ, UError_ਕੋਡ &ਸਥਿਤੀ);
    
    ਸਤਰਅੰਦਰ int32_t elementAti(int32_t ਸੂਚਕਾਂਕ) ਅਚਲ;

    UBool equals(ਅਚਲ UVector32 &other) ਅਚਲ;

    ਸਤਰਅੰਦਰ int32_t ਆਖਰੀ_Elementi() ਅਚਲ;

    int32_t ਸੂਚਕਾਂਕ_ਦਾ(int32_t elem, int32_t ਸ਼ੁਰੂ_ਸੂਚਕਾਂਕ = 0) ਅਚਲ;

    ਸਤਰਅੰਦਰ UBool ਸ਼ਾਮਲ_ਹੈ(int32_t elem) ਅਚਲ;

    UBool ਸ਼ਾਮਲ_ਹੈ_All(ਅਚਲ UVector32& other) ਅਚਲ;

    UBool removeAll(ਅਚਲ UVector32& other);

    UBool retainAll(ਅਚਲ UVector32& other);

    ਖਾਲੀ removeElementAt(int32_t ਸੂਚਕਾਂਕ);

    ਖਾਲੀ removeAllElements();

    ਸਤਰਅੰਦਰ int32_t ਮਾਪ_2() ਅਚਲ;

    ਸਤਰਅੰਦਰ UBool is_ਖਾਲੀ_ਥਾਵਾਂ() ਅਚਲ;

    // Inline.  Use this one for speedy size check.
    ਸਤਰਅੰਦਰ UBool ensure_ਸਮਰੱਥਾ(int32_t minimum_ਸਮਰੱਥਾ, UError_ਕੋਡ &ਸਥਿਤੀ);

    // Out-of-line, handles actual growth.  Called by ensureCapacity() when necessary.
    UBool expand_ਸਮਰੱਥਾ(int32_t minimum_ਸਮਰੱਥਾ, UError_ਕੋਡ &ਸਥਿਤੀ);

    /**
     * Change the size of this vector as follows: If newSize is
     * smaller, then truncate the array, possibly deleting held
     * elements for i >= newSize.  If newSize is larger, grow the
     * array, filling in new slows with zero.
     */
    ਖਾਲੀ ਸੈੱਟ_ਮਾਪ(int32_t new_ਮਾਪ);

    //------------------------------------------------------------
    // New API
    //------------------------------------------------------------

    /**
     * Returns true if this vector contains none of the elements
     * of the given vector.
     * @param other vector to be checked for containment
     * @return true if the test condition is met
     */
    UBool ਸ਼ਾਮਲ_ਹੈ_None(ਅਚਲ UVector32& other) ਅਚਲ;


    /**
     * Insert the given integer into this vector at its sorted position.
     * The current elements are assumed to be sorted already.
     */
    ਖਾਲੀ sorted_ਪਾਓ(int32_t elem, UError_ਕੋਡ& ec);

    /**
     * Returns a pointer to the internal array holding the vector.
     */
    ਸਤਰਅੰਦਰ int32_t *get_ਬਫਰ() ਅਚਲ;

    /**
     * Set the maximum allowed buffer capacity for this vector/stack.
     * Default with no limit set is unlimited, go until malloc() fails.
     * A Limit of zero means unlimited capacity.
     * Units are vector elements (32 bits each), not bytes.
     */
    ਖਾਲੀ ਸੈੱਟ_Max_ਸਮਰੱਥਾ(int32_t ਹੱਦਬੰਦੀ);

    /**
     * ICU "poor man's RTTI", returns a UClassID for this class.
     */
    ਸਥਿਰ UClassID U_EXPORT2 getStaticClassID();

    /**
     * ICU "poor man's RTTI", returns a UClassID for the actual class.
     */
    ਆਭਾਸੀ UClassID getDynamicClassID() ਅਚਲ override;

ਨਿਜੀ:
    ਖਾਲੀ _init(int32_t initial_ਸਮਰੱਥਾ, UError_ਕੋਡ &ਸਥਿਤੀ);

    // Disallow
    UVector32(ਅਚਲ UVector32&) = delete;

    // Disallow
    UVector32& ਸੰਚਾਲਕ=(ਅਚਲ UVector32&) = delete;


    //  API Functions for Stack operations.
    //  In the original UVector, these were in a separate derived class, UStack.
    //  Here in UVector32, they are all together.
ਜਨਤਕ:
    ਸਤਰਅੰਦਰ UBool ਖਾਲੀ_ਥਾਵਾਂ() ਅਚਲ;   // TODO:  redundant, same as empty().  Remove it?

    ਸਤਰਅੰਦਰ int32_t peeki() ਅਚਲ;
    
    ਸਤਰਅੰਦਰ int32_t popi();
    
    ਸਤਰਅੰਦਰ int32_t ਧੱਕੋ(int32_t i, UError_ਕੋਡ &ਸਥਿਤੀ);

    ਸਤਰਅੰਦਰ int32_t *reserve_ਖੰਡ(int32_t ਮਾਪ_2, UError_ਕੋਡ &ਸਥਿਤੀ);
    ਸਤਰਅੰਦਰ int32_t *ਕੱਢੋ_ਚੌਖਟਾ(int32_t ਮਾਪ_2);
};


// UVector32 inlines

ਸਤਰਅੰਦਰ UBool UVector32::ensure_ਸਮਰੱਥਾ(int32_t minimum_ਸਮਰੱਥਾ, UError_ਕੋਡ &ਸਥਿਤੀ) {
    ਜੇ ((minimum_ਸਮਰੱਥਾ >= 0) && (ਸਮਰੱਥਾ >= minimum_ਸਮਰੱਥਾ)) {
        ਵਾਪਸ true;
    } ਨਹੀਂਤਾਂ {
        ਵਾਪਸ expand_ਸਮਰੱਥਾ(minimum_ਸਮਰੱਥਾ, ਸਥਿਤੀ);
    }
}

ਸਤਰਅੰਦਰ int32_t UVector32::elementAti(int32_t ਸੂਚਕਾਂਕ) ਅਚਲ {
    ਵਾਪਸ (ਸੂਚਕਾਂਕ >= 0 && ਗਿਣਤੀ_2 > 0 && ਗਿਣਤੀ_2 - ਸੂਚਕਾਂਕ > 0) ? elements[ਸੂਚਕਾਂਕ] : 0;
}


ਸਤਰਅੰਦਰ ਖਾਲੀ UVector32::ਜੋੜੋ_Element(int32_t elem, UError_ਕੋਡ &ਸਥਿਤੀ) {
    ਜੇ (ensure_ਸਮਰੱਥਾ(ਗਿਣਤੀ_2 + 1, ਸਥਿਤੀ)) {
        elements[ਗਿਣਤੀ_2] = elem;
        ਗਿਣਤੀ_2++;
    }
}

ਸਤਰਅੰਦਰ int32_t *UVector32::reserve_ਖੰਡ(int32_t ਮਾਪ_2, UError_ਕੋਡ &ਸਥਿਤੀ) {
    ਜੇ (ensure_ਸਮਰੱਥਾ(ਗਿਣਤੀ_2+ਮਾਪ_2, ਸਥਿਤੀ) == false) {
        ਵਾਪਸ nullptr;
    }
    int32_t  *rp = elements+ਗਿਣਤੀ_2;
    ਗਿਣਤੀ_2 += ਮਾਪ_2;
    ਵਾਪਸ rp;
}

ਸਤਰਅੰਦਰ int32_t *UVector32::ਕੱਢੋ_ਚੌਖਟਾ(int32_t ਮਾਪ_2) {
    U_ASSERT(ਗਿਣਤੀ_2 >= ਮਾਪ_2);
    ਗਿਣਤੀ_2 -= ਮਾਪ_2;
    ਜੇ (ਗਿਣਤੀ_2 < 0) {
        ਗਿਣਤੀ_2 = 0;
    }
    ਵਾਪਸ elements+ਗਿਣਤੀ_2-ਮਾਪ_2;
}



ਸਤਰਅੰਦਰ int32_t UVector32::ਮਾਪ_2() ਅਚਲ {
    ਵਾਪਸ ਗਿਣਤੀ_2;
}

ਸਤਰਅੰਦਰ UBool UVector32::is_ਖਾਲੀ_ਥਾਵਾਂ() ਅਚਲ {
    ਵਾਪਸ ਗਿਣਤੀ_2 == 0;
}

ਸਤਰਅੰਦਰ UBool UVector32::ਸ਼ਾਮਲ_ਹੈ(int32_t obj) ਅਚਲ {
    ਵਾਪਸ ਸੂਚਕਾਂਕ_ਦਾ(obj) >= 0;
}

ਸਤਰਅੰਦਰ int32_t UVector32::ਆਖਰੀ_Elementi() ਅਚਲ {
    ਵਾਪਸ elementAti(ਗਿਣਤੀ_2-1);
}

ਸਤਰਅੰਦਰ bool UVector32::ਸੰਚਾਲਕ!=(ਅਚਲ UVector32& other) ਅਚਲ {
    ਵਾਪਸ !ਸੰਚਾਲਕ==(other);
}

ਸਤਰਅੰਦਰ int32_t *UVector32::get_ਬਫਰ() ਅਚਲ {
    ਵਾਪਸ elements;
}


// UStack inlines

ਸਤਰਅੰਦਰ UBool UVector32::ਖਾਲੀ_ਥਾਵਾਂ() ਅਚਲ {
    ਵਾਪਸ is_ਖਾਲੀ_ਥਾਵਾਂ();
}

ਸਤਰਅੰਦਰ int32_t UVector32::peeki() ਅਚਲ {
    ਵਾਪਸ ਆਖਰੀ_Elementi();
}

ਸਤਰਅੰਦਰ int32_t UVector32::ਧੱਕੋ(int32_t i, UError_ਕੋਡ &ਸਥਿਤੀ) {
    ਜੋੜੋ_Element(i, ਸਥਿਤੀ);
    ਵਾਪਸ i;
}

ਸਤਰਅੰਦਰ int32_t UVector32::popi() {
    int32_t ਨਤੀਜਾ = 0;
    ਜੇ (ਗਿਣਤੀ_2 > 0) {
        ਗਿਣਤੀ_2--;
        ਨਤੀਜਾ = elements[ਗਿਣਤੀ_2];
    }
    ਵਾਪਸ ਨਤੀਜਾ;
}

U_NAMESPACE_ਸਿਰਾ

#ਜੇ_ਅੰਤ
