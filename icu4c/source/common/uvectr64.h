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

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ UVECTOR64_H
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ UVECTOR64_H

#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/utypes.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/uobject.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "uhash.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "uassert.h"

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
class U_COMMON_API UVector64 : ਜਨਤਕ UObject {
ਨਿਜੀ:
    int32_t   ਗਿਣਤੀ_2;

    int32_t   ਸਮਰੱਥਾ;
    
    int32_t   max_ਸਮਰੱਥਾ;   // Limit beyond which capacity is not permitted to grow.

    int64_t*  elements;

ਜਨਤਕ:
    UVector64(UError_ਕੋਡ &ਸਥਿਤੀ);

    UVector64(int32_t initial_ਸਮਰੱਥਾ, UError_ਕੋਡ &ਸਥਿਤੀ);

    ਆਭਾਸੀ ~UVector64();

    /**
     * Assign this object to another (make this a copy of 'other').
     * Use the 'assign' function to assign each element.
     */
    ਖਾਲੀ ਸੌਂਪੋ(ਅਚਲ UVector64& other, UError_ਕੋਡ &ec);

    /**
     * Compare this vector with another.  They will be considered
     * equal if they are of the same size and all elements are equal,
     * as compared using this object's comparer.
     */
    bool ਸੰਚਾਲਕ==(ਅਚਲ UVector64& other);

    /**
     * Equivalent to !operator==()
     */
    ਸਤਰਅੰਦਰ bool ਸੰਚਾਲਕ!=(ਅਚਲ UVector64& other);

    //------------------------------------------------------------
    // subset of java.util.Vector API
    //------------------------------------------------------------

    ਸਤਰਅੰਦਰ ਖਾਲੀ ਜੋੜੋ_Element(int64_t elem, UError_ਕੋਡ &ਸਥਿਤੀ);

    ਖਾਲੀ ਸੈੱਟ_Element_At(int64_t elem, int32_t ਸੂਚਕਾਂਕ);

    ਖਾਲੀ ਪਾਓ_Element_At(int64_t elem, int32_t ਸੂਚਕਾਂਕ, UError_ਕੋਡ &ਸਥਿਤੀ);
    
    ਸਤਰਅੰਦਰ int64_t elementAti(int32_t ਸੂਚਕਾਂਕ) ਅਚਲ;

    //UBool equals(const UVector64 &other) const;

    ਸਤਰਅੰਦਰ int64_t ਆਖਰੀ_Elementi() ਅਚਲ;

    //int32_t indexOf(int64_t elem, int32_t startIndex = 0) const;

    //UBool contains(int64_t elem) const;

    //UBool containsAll(const UVector64& other) const;

    //UBool removeAll(const UVector64& other);

    //UBool retainAll(const UVector64& other);

    //void removeElementAt(int32_t index);

    ਖਾਲੀ removeAllElements();

    ਸਤਰਅੰਦਰ int32_t ਮਾਪ_2() ਅਚਲ;

    ਸਤਰਅੰਦਰ UBool is_ਖਾਲੀ_ਥਾਵਾਂ() ਅਚਲ { ਵਾਪਸ ਗਿਣਤੀ_2 == 0; }

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

    //UBool containsNone(const UVector64& other) const;


    //void sortedInsert(int64_t elem, UErrorCode& ec);

    /**
     * Returns a pointer to the internal array holding the vector.
     */
    ਸਤਰਅੰਦਰ int64_t *get_ਬਫਰ() ਅਚਲ;

    /**
     * Set the maximum allowed buffer capacity for this vector/stack.
     * Default with no limit set is unlimited, go until malloc() fails.
     * A Limit of zero means unlimited capacity.
     * Units are vector elements (64 bits each), not bytes.
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
    UVector64(ਅਚਲ UVector64&) = delete;

    // Disallow
    UVector64& ਸੰਚਾਲਕ=(ਅਚਲ UVector64&) = delete;


    //  API Functions for Stack operations.
    //  In the original UVector, these were in a separate derived class, UStack.
    //  Here in UVector64, they are all together.
ਜਨਤਕ:
    //UBool empty() const;   // TODO:  redundant, same as empty().  Remove it?

    //int64_t peeki() const;
    
    ਸਤਰਅੰਦਰ int64_t popi();
    
    ਸਤਰਅੰਦਰ int64_t ਧੱਕੋ(int64_t i, UError_ਕੋਡ &ਸਥਿਤੀ);

    ਸਤਰਅੰਦਰ int64_t *reserve_ਖੰਡ(int32_t ਮਾਪ_2, UError_ਕੋਡ &ਸਥਿਤੀ);
    ਸਤਰਅੰਦਰ int64_t *ਕੱਢੋ_ਚੌਖਟਾ(int32_t ਮਾਪ_2);
};


// UVector64 inlines

ਸਤਰਅੰਦਰ UBool UVector64::ensure_ਸਮਰੱਥਾ(int32_t minimum_ਸਮਰੱਥਾ, UError_ਕੋਡ &ਸਥਿਤੀ) {
    ਜੇ ((minimum_ਸਮਰੱਥਾ >= 0) && (ਸਮਰੱਥਾ >= minimum_ਸਮਰੱਥਾ)) {
        ਵਾਪਸ true;
    } ਨਹੀਂਤਾਂ {
        ਵਾਪਸ expand_ਸਮਰੱਥਾ(minimum_ਸਮਰੱਥਾ, ਸਥਿਤੀ);
    }
}

ਸਤਰਅੰਦਰ int64_t UVector64::elementAti(int32_t ਸੂਚਕਾਂਕ) ਅਚਲ {
    ਵਾਪਸ (0 <= ਸੂਚਕਾਂਕ && ਸੂਚਕਾਂਕ < ਗਿਣਤੀ_2) ? elements[ਸੂਚਕਾਂਕ] : 0;
}


ਸਤਰਅੰਦਰ ਖਾਲੀ UVector64::ਜੋੜੋ_Element(int64_t elem, UError_ਕੋਡ &ਸਥਿਤੀ) {
    ਜੇ (ensure_ਸਮਰੱਥਾ(ਗਿਣਤੀ_2 + 1, ਸਥਿਤੀ)) {
        elements[ਗਿਣਤੀ_2] = elem;
        ਗਿਣਤੀ_2++;
    }
}

ਸਤਰਅੰਦਰ int64_t *UVector64::reserve_ਖੰਡ(int32_t ਮਾਪ_2, UError_ਕੋਡ &ਸਥਿਤੀ) {
    ਜੇ (ensure_ਸਮਰੱਥਾ(ਗਿਣਤੀ_2+ਮਾਪ_2, ਸਥਿਤੀ) == false) {
        ਵਾਪਸ nullptr;
    }
    int64_t  *rp = elements+ਗਿਣਤੀ_2;
    ਗਿਣਤੀ_2 += ਮਾਪ_2;
    ਵਾਪਸ rp;
}

ਸਤਰਅੰਦਰ int64_t *UVector64::ਕੱਢੋ_ਚੌਖਟਾ(int32_t ਮਾਪ_2) {
    U_ASSERT(ਗਿਣਤੀ_2 >= ਮਾਪ_2);
    ਗਿਣਤੀ_2 -= ਮਾਪ_2;
    ਜੇ (ਗਿਣਤੀ_2 < 0) {
        ਗਿਣਤੀ_2 = 0;
    }
    ਵਾਪਸ elements+ਗਿਣਤੀ_2-ਮਾਪ_2;
}



ਸਤਰਅੰਦਰ int32_t UVector64::ਮਾਪ_2() ਅਚਲ {
    ਵਾਪਸ ਗਿਣਤੀ_2;
}

ਸਤਰਅੰਦਰ int64_t UVector64::ਆਖਰੀ_Elementi() ਅਚਲ {
    ਵਾਪਸ elementAti(ਗਿਣਤੀ_2-1);
}

ਸਤਰਅੰਦਰ bool UVector64::ਸੰਚਾਲਕ!=(ਅਚਲ UVector64& other) {
    ਵਾਪਸ !ਸੰਚਾਲਕ==(other);
}

ਸਤਰਅੰਦਰ int64_t *UVector64::get_ਬਫਰ() ਅਚਲ {
    ਵਾਪਸ elements;
}


// UStack inlines

ਸਤਰਅੰਦਰ int64_t UVector64::ਧੱਕੋ(int64_t i, UError_ਕੋਡ &ਸਥਿਤੀ) {
    ਜੋੜੋ_Element(i, ਸਥਿਤੀ);
    ਵਾਪਸ i;
}

ਸਤਰਅੰਦਰ int64_t UVector64::popi() {
    int64_t ਨਤੀਜਾ = 0;
    ਜੇ (ਗਿਣਤੀ_2 > 0) {
        ਗਿਣਤੀ_2--;
        ਨਤੀਜਾ = elements[ਗਿਣਤੀ_2];
    }
    ਵਾਪਸ ਨਤੀਜਾ;
}

U_NAMESPACE_ਸਿਰਾ

#ਜੇ_ਅੰਤ
