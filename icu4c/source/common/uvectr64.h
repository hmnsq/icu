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

#اگر_تعریف_نشده UVECTOR64_H
#تعریف_کن UVECTOR64_H

#ضمن "unicode/utypes.h"
#ضمن "unicode/uobject.h"
#ضمن "uhash.h"
#ضمن "uassert.h"

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
class U_COMMON_API UVector64 : عام UObject {
خاص:
    int32_t   شمار;

    int32_t   ظرفیت;
    
    int32_t   max_ظرفیت;   // Limit beyond which capacity is not permitted to grow.

    int64_t*  elements;

عام:
    UVector64(UError_کد &وضعیت);

    UVector64(int32_t initial_ظرفیت, UError_کد &وضعیت);

    مجازی ~UVector64();

    /**
     * Assign this object to another (make this a copy of 'other').
     * Use the 'assign' function to assign each element.
     */
    فراغ تخصیص_بده(ثابت UVector64& other, UError_کد &ec);

    /**
     * Compare this vector with another.  They will be considered
     * equal if they are of the same size and all elements are equal,
     * as compared using this object's comparer.
     */
    bool عملگر==(ثابت UVector64& other);

    /**
     * Equivalent to !operator==()
     */
    مضمن bool عملگر!=(ثابت UVector64& other);

    //------------------------------------------------------------
    // subset of java.util.Vector API
    //------------------------------------------------------------

    مضمن فراغ جمع_کن_Element(int64_t elem, UError_کد &وضعیت);

    فراغ مجموعه_Element_At(int64_t elem, int32_t شاخص);

    فراغ درج_Element_At(int64_t elem, int32_t شاخص, UError_کد &وضعیت);
    
    مضمن int64_t elementAti(int32_t شاخص) ثابت;

    //UBool equals(const UVector64 &other) const;

    مضمن int64_t آخرین_Elementi() ثابت;

    //int32_t indexOf(int64_t elem, int32_t startIndex = 0) const;

    //UBool contains(int64_t elem) const;

    //UBool containsAll(const UVector64& other) const;

    //UBool removeAll(const UVector64& other);

    //UBool retainAll(const UVector64& other);

    //void removeElementAt(int32_t index);

    فراغ removeAllElements();

    مضمن int32_t اندازه_2() ثابت;

    مضمن UBool is_خالی() ثابت { أرجع شمار == 0; }

    // Inline.  Use this one for speedy size check.
    مضمن UBool ensure_ظرفیت(int32_t minimum_ظرفیت, UError_کد &وضعیت);

    // Out-of-line, handles actual growth.  Called by ensureCapacity() when necessary.
    UBool expand_ظرفیت(int32_t minimum_ظرفیت, UError_کد &وضعیت);

    /**
     * Change the size of this vector as follows: If newSize is
     * smaller, then truncate the array, possibly deleting held
     * elements for i >= newSize.  If newSize is larger, grow the
     * array, filling in new slows with zero.
     */
    فراغ مجموعه_اندازه(int32_t new_اندازه);

    //------------------------------------------------------------
    // New API
    //------------------------------------------------------------

    //UBool containsNone(const UVector64& other) const;


    //void sortedInsert(int64_t elem, UErrorCode& ec);

    /**
     * Returns a pointer to the internal array holding the vector.
     */
    مضمن int64_t *get_میانگیر() ثابت;

    /**
     * Set the maximum allowed buffer capacity for this vector/stack.
     * Default with no limit set is unlimited, go until malloc() fails.
     * A Limit of zero means unlimited capacity.
     * Units are vector elements (64 bits each), not bytes.
     */
    فراغ مجموعه_Max_ظرفیت(int32_t حد);

    /**
     * ICU "poor man's RTTI", returns a UClassID for this class.
     */
    ساكن UClassID U_EXPORT2 getStaticClassID();

    /**
     * ICU "poor man's RTTI", returns a UClassID for the actual class.
     */
    مجازی UClassID getDynamicClassID() ثابت override;

خاص:
    فراغ _init(int32_t initial_ظرفیت, UError_کد &وضعیت);

    // Disallow
    UVector64(ثابت UVector64&) = delete;

    // Disallow
    UVector64& عملگر=(ثابت UVector64&) = delete;


    //  API Functions for Stack operations.
    //  In the original UVector, these were in a separate derived class, UStack.
    //  Here in UVector64, they are all together.
عام:
    //UBool empty() const;   // TODO:  redundant, same as empty().  Remove it?

    //int64_t peeki() const;
    
    مضمن int64_t popi();
    
    مضمن int64_t افزودن(int64_t i, UError_کد &وضعیت);

    مضمن int64_t *reserve_بلوک(int32_t اندازه_2, UError_کد &وضعیت);
    مضمن int64_t *برداشت_قاب(int32_t اندازه_2);
};


// UVector64 inlines

مضمن UBool UVector64::ensure_ظرفیت(int32_t minimum_ظرفیت, UError_کد &وضعیت) {
    إذا ((minimum_ظرفیت >= 0) && (ظرفیت >= minimum_ظرفیت)) {
        أرجع true;
    } وإلا {
        أرجع expand_ظرفیت(minimum_ظرفیت, وضعیت);
    }
}

مضمن int64_t UVector64::elementAti(int32_t شاخص) ثابت {
    أرجع (0 <= شاخص && شاخص < شمار) ? elements[شاخص] : 0;
}


مضمن فراغ UVector64::جمع_کن_Element(int64_t elem, UError_کد &وضعیت) {
    إذا (ensure_ظرفیت(شمار + 1, وضعیت)) {
        elements[شمار] = elem;
        شمار++;
    }
}

مضمن int64_t *UVector64::reserve_بلوک(int32_t اندازه_2, UError_کد &وضعیت) {
    إذا (ensure_ظرفیت(شمار+اندازه_2, وضعیت) == false) {
        أرجع nullptr;
    }
    int64_t  *rp = elements+شمار;
    شمار += اندازه_2;
    أرجع rp;
}

مضمن int64_t *UVector64::برداشت_قاب(int32_t اندازه_2) {
    U_ASSERT(شمار >= اندازه_2);
    شمار -= اندازه_2;
    إذا (شمار < 0) {
        شمار = 0;
    }
    أرجع elements+شمار-اندازه_2;
}



مضمن int32_t UVector64::اندازه_2() ثابت {
    أرجع شمار;
}

مضمن int64_t UVector64::آخرین_Elementi() ثابت {
    أرجع elementAti(شمار-1);
}

مضمن bool UVector64::عملگر!=(ثابت UVector64& other) {
    أرجع !عملگر==(other);
}

مضمن int64_t *UVector64::get_میانگیر() ثابت {
    أرجع elements;
}


// UStack inlines

مضمن int64_t UVector64::افزودن(int64_t i, UError_کد &وضعیت) {
    جمع_کن_Element(i, وضعیت);
    أرجع i;
}

مضمن int64_t UVector64::popi() {
    int64_t نتیجه = 0;
    إذا (شمار > 0) {
        شمار--;
        نتیجه = elements[شمار];
    }
    أرجع نتیجه;
}

U_NAMESPACE_انتها

#پایان_اگر
