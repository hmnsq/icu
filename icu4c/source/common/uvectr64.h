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

#ifndef UVECTOR64_H
#define UVECTOR64_H

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
class U_COMMON_API UVector64 : عمومی UObject {
خصوصی:
    int32_t   شمار;

    int32_t   capacity;
    
    int32_t   maxCapacity;   // Limit beyond which capacity is not permitted to grow.

    int64_t*  elements;

عمومی:
    UVector64(UErrorCode &status);

    UVector64(int32_t initialCapacity, UErrorCode &status);

    مجازی ~UVector64();

    /**
     * Assign this object to another (make this a copy of 'other').
     * Use the 'assign' function to assign each element.
     */
    تهی assign(ثابت UVector64& other, UErrorCode &ec);

    /**
     * Compare this vector with another.  They will be considered
     * equal if they are of the same size and all elements are equal,
     * as compared using this object's comparer.
     */
    bool operator==(ثابت UVector64& other);

    /**
     * Equivalent to !operator==()
     */
    درونخطی bool operator!=(ثابت UVector64& other);

    //------------------------------------------------------------
    // subset of java.util.Vector API
    //------------------------------------------------------------

    درونخطی تهی addElement(int64_t elem, UErrorCode &status);

    تهی setElementAt(int64_t elem, int32_t index);

    تهی درج_Element_At(int64_t elem, int32_t index, UErrorCode &status);
    
    درونخطی int64_t elementAti(int32_t index) ثابت;

    //UBool equals(const UVector64 &other) const;

    درونخطی int64_t lastElementi() ثابت;

    //int32_t indexOf(int64_t elem, int32_t startIndex = 0) const;

    //UBool contains(int64_t elem) const;

    //UBool containsAll(const UVector64& other) const;

    //UBool removeAll(const UVector64& other);

    //UBool retainAll(const UVector64& other);

    //void removeElementAt(int32_t index);

    تهی removeAllElements();

    درونخطی int32_t size() ثابت;

    درونخطی UBool isEmpty() ثابت { بازگشت شمار == 0; }

    // Inline.  Use this one for speedy size check.
    درونخطی UBool ensureCapacity(int32_t minimumCapacity, UErrorCode &status);

    // Out-of-line, handles actual growth.  Called by ensureCapacity() when necessary.
    UBool expandCapacity(int32_t minimumCapacity, UErrorCode &status);

    /**
     * Change the size of this vector as follows: If newSize is
     * smaller, then truncate the array, possibly deleting held
     * elements for i >= newSize.  If newSize is larger, grow the
     * array, filling in new slows with zero.
     */
    تهی setSize(int32_t newSize);

    //------------------------------------------------------------
    // New API
    //------------------------------------------------------------

    //UBool containsNone(const UVector64& other) const;


    //void sortedInsert(int64_t elem, UErrorCode& ec);

    /**
     * Returns a pointer to the internal array holding the vector.
     */
    درونخطی int64_t *getBuffer() ثابت;

    /**
     * Set the maximum allowed buffer capacity for this vector/stack.
     * Default with no limit set is unlimited, go until malloc() fails.
     * A Limit of zero means unlimited capacity.
     * Units are vector elements (64 bits each), not bytes.
     */
    تهی setMaxCapacity(int32_t limit);

    /**
     * ICU "poor man's RTTI", returns a UClassID for this class.
     */
    ایستا UClassID U_EXPORT2 getStaticClassID();

    /**
     * ICU "poor man's RTTI", returns a UClassID for the actual class.
     */
    مجازی UClassID getDynamicClassID() ثابت override;

خصوصی:
    تهی _init(int32_t initialCapacity, UErrorCode &status);

    // Disallow
    UVector64(ثابت UVector64&) = delete;

    // Disallow
    UVector64& operator=(ثابت UVector64&) = delete;


    //  API Functions for Stack operations.
    //  In the original UVector, these were in a separate derived class, UStack.
    //  Here in UVector64, they are all together.
عمومی:
    //UBool empty() const;   // TODO:  redundant, same as empty().  Remove it?

    //int64_t peeki() const;
    
    درونخطی int64_t popi();
    
    درونخطی int64_t افزودن(int64_t i, UErrorCode &status);

    درونخطی int64_t *reserveBlock(int32_t size, UErrorCode &status);
    درونخطی int64_t *برداشت_Frame(int32_t size);
};


// UVector64 inlines

درونخطی UBool UVector64::ensureCapacity(int32_t minimumCapacity, UErrorCode &status) {
    اگر ((minimumCapacity >= 0) && (capacity >= minimumCapacity)) {
        بازگشت true;
    } وگرنه {
        بازگشت expandCapacity(minimumCapacity, status);
    }
}

درونخطی int64_t UVector64::elementAti(int32_t index) ثابت {
    بازگشت (0 <= index && index < شمار) ? elements[index] : 0;
}


درونخطی تهی UVector64::addElement(int64_t elem, UErrorCode &status) {
    اگر (ensureCapacity(شمار + 1, status)) {
        elements[شمار] = elem;
        شمار++;
    }
}

درونخطی int64_t *UVector64::reserveBlock(int32_t size, UErrorCode &status) {
    اگر (ensureCapacity(شمار+size, status) == false) {
        بازگشت nullptr;
    }
    int64_t  *rp = elements+شمار;
    شمار += size;
    بازگشت rp;
}

درونخطی int64_t *UVector64::برداشت_Frame(int32_t size) {
    U_ASSERT(شمار >= size);
    شمار -= size;
    اگر (شمار < 0) {
        شمار = 0;
    }
    بازگشت elements+شمار-size;
}



درونخطی int32_t UVector64::size() ثابت {
    بازگشت شمار;
}

درونخطی int64_t UVector64::lastElementi() ثابت {
    بازگشت elementAti(شمار-1);
}

درونخطی bool UVector64::operator!=(ثابت UVector64& other) {
    بازگشت !operator==(other);
}

درونخطی int64_t *UVector64::getBuffer() ثابت {
    بازگشت elements;
}


// UStack inlines

درونخطی int64_t UVector64::افزودن(int64_t i, UErrorCode &status) {
    addElement(i, status);
    بازگشت i;
}

درونخطی int64_t UVector64::popi() {
    int64_t result = 0;
    اگر (شمار > 0) {
        شمار--;
        result = elements[شمار];
    }
    بازگشت result;
}

U_NAMESPACE_END

#endif
