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

#اگر_غیر_تعریف_شدہ UVECTOR64_H
#تعریف_کرو UVECTOR64_H

#شامل "unicode/utypes.h"
#شامل "unicode/uobject.h"
#شامل "uhash.h"
#شامل "uassert.h"

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
class U_COMMON_API UVector64 : عوامی UObject {
نجی:
    int32_t   گنتی;

    int32_t   استعداد;
    
    int32_t   max_استعداد;   // Limit beyond which capacity is not permitted to grow.

    int64_t*  elements;

عوامی:
    UVector64(UError_رمز &حالت_2);

    UVector64(int32_t initial_استعداد, UError_رمز &حالت_2);

    مجازی ~UVector64();

    /**
     * Assign this object to another (make this a copy of 'other').
     * Use the 'assign' function to assign each element.
     */
    خالی تفویض(ثابت UVector64& other, UError_رمز &ec);

    /**
     * Compare this vector with another.  They will be considered
     * equal if they are of the same size and all elements are equal,
     * as compared using this object's comparer.
     */
    bool عامل==(ثابت UVector64& other);

    /**
     * Equivalent to !operator==()
     */
    اندرونی bool عامل!=(ثابت UVector64& other);

    //------------------------------------------------------------
    // subset of java.util.Vector API
    //------------------------------------------------------------

    اندرونی خالی جوڑو_Element(int64_t elem, UError_رمز &حالت_2);

    خالی سیٹ_Element_At(int64_t elem, int32_t اشاریہ);

    خالی داخل_کرو_Element_At(int64_t elem, int32_t اشاریہ, UError_رمز &حالت_2);
    
    اندرونی int64_t elementAti(int32_t اشاریہ) ثابت;

    //UBool equals(const UVector64 &other) const;

    اندرونی int64_t آخری_Elementi() ثابت;

    //int32_t indexOf(int64_t elem, int32_t startIndex = 0) const;

    //UBool contains(int64_t elem) const;

    //UBool containsAll(const UVector64& other) const;

    //UBool removeAll(const UVector64& other);

    //UBool retainAll(const UVector64& other);

    //void removeElementAt(int32_t index);

    خالی removeAllElements();

    اندرونی int32_t سائز_2() ثابت;

    اندرونی UBool is_خالی_خانے() ثابت { واپس گنتی == 0; }

    // Inline.  Use this one for speedy size check.
    اندرونی UBool ensure_استعداد(int32_t minimum_استعداد, UError_رمز &حالت_2);

    // Out-of-line, handles actual growth.  Called by ensureCapacity() when necessary.
    UBool expand_استعداد(int32_t minimum_استعداد, UError_رمز &حالت_2);

    /**
     * Change the size of this vector as follows: If newSize is
     * smaller, then truncate the array, possibly deleting held
     * elements for i >= newSize.  If newSize is larger, grow the
     * array, filling in new slows with zero.
     */
    خالی سیٹ_سائز(int32_t new_سائز);

    //------------------------------------------------------------
    // New API
    //------------------------------------------------------------

    //UBool containsNone(const UVector64& other) const;


    //void sortedInsert(int64_t elem, UErrorCode& ec);

    /**
     * Returns a pointer to the internal array holding the vector.
     */
    اندرونی int64_t *get_بفر() ثابت;

    /**
     * Set the maximum allowed buffer capacity for this vector/stack.
     * Default with no limit set is unlimited, go until malloc() fails.
     * A Limit of zero means unlimited capacity.
     * Units are vector elements (64 bits each), not bytes.
     */
    خالی سیٹ_Max_استعداد(int32_t حد_بندی);

    /**
     * ICU "poor man's RTTI", returns a UClassID for this class.
     */
    ساکن UClassID U_EXPORT2 getStaticClassID();

    /**
     * ICU "poor man's RTTI", returns a UClassID for the actual class.
     */
    مجازی UClassID getDynamicClassID() ثابت override;

نجی:
    خالی _init(int32_t initial_استعداد, UError_رمز &حالت_2);

    // Disallow
    UVector64(ثابت UVector64&) = delete;

    // Disallow
    UVector64& عامل=(ثابت UVector64&) = delete;


    //  API Functions for Stack operations.
    //  In the original UVector, these were in a separate derived class, UStack.
    //  Here in UVector64, they are all together.
عوامی:
    //UBool empty() const;   // TODO:  redundant, same as empty().  Remove it?

    //int64_t peeki() const;
    
    اندرونی int64_t popi();
    
    اندرونی int64_t ڈالو(int64_t i, UError_رمز &حالت_2);

    اندرونی int64_t *reserve_بلاک(int32_t سائز_2, UError_رمز &حالت_2);
    اندرونی int64_t *نکالو_چوکھٹا(int32_t سائز_2);
};


// UVector64 inlines

اندرونی UBool UVector64::ensure_استعداد(int32_t minimum_استعداد, UError_رمز &حالت_2) {
    اگر ((minimum_استعداد >= 0) && (استعداد >= minimum_استعداد)) {
        واپس true;
    } ورنہ {
        واپس expand_استعداد(minimum_استعداد, حالت_2);
    }
}

اندرونی int64_t UVector64::elementAti(int32_t اشاریہ) ثابت {
    واپس (0 <= اشاریہ && اشاریہ < گنتی) ? elements[اشاریہ] : 0;
}


اندرونی خالی UVector64::جوڑو_Element(int64_t elem, UError_رمز &حالت_2) {
    اگر (ensure_استعداد(گنتی + 1, حالت_2)) {
        elements[گنتی] = elem;
        گنتی++;
    }
}

اندرونی int64_t *UVector64::reserve_بلاک(int32_t سائز_2, UError_رمز &حالت_2) {
    اگر (ensure_استعداد(گنتی+سائز_2, حالت_2) == false) {
        واپس nullptr;
    }
    int64_t  *rp = elements+گنتی;
    گنتی += سائز_2;
    واپس rp;
}

اندرونی int64_t *UVector64::نکالو_چوکھٹا(int32_t سائز_2) {
    U_ASSERT(گنتی >= سائز_2);
    گنتی -= سائز_2;
    اگر (گنتی < 0) {
        گنتی = 0;
    }
    واپس elements+گنتی-سائز_2;
}



اندرونی int32_t UVector64::سائز_2() ثابت {
    واپس گنتی;
}

اندرونی int64_t UVector64::آخری_Elementi() ثابت {
    واپس elementAti(گنتی-1);
}

اندرونی bool UVector64::عامل!=(ثابت UVector64& other) {
    واپس !عامل==(other);
}

اندرونی int64_t *UVector64::get_بفر() ثابت {
    واپس elements;
}


// UStack inlines

اندرونی int64_t UVector64::ڈالو(int64_t i, UError_رمز &حالت_2) {
    جوڑو_Element(i, حالت_2);
    واپس i;
}

اندرونی int64_t UVector64::popi() {
    int64_t نتیجہ = 0;
    اگر (گنتی > 0) {
        گنتی--;
        نتیجہ = elements[گنتی];
    }
    واپس نتیجہ;
}

U_NAMESPACE_انتہا

#اگر_ختم
