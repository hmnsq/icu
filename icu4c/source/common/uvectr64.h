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

#அந்தர்பாவ "unicode/utypes.h"
#அந்தர்பாவ "unicode/uobject.h"
#அந்தர்பாவ "uhash.h"
#அந்தர்பாவ "uassert.h"

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
class U_COMMON_API UVector64 : பொது UObject {
தனிப்பட்ட:
    int32_t   மொத்தம்;

    int32_t   capacity;
    
    int32_t   maxCapacity;   // Limit beyond which capacity is not permitted to grow.

    int64_t*  elements;

பொது:
    UVector64(UErrorCode &status);

    UVector64(int32_t initialCapacity, UErrorCode &status);

    மெய்நிகர் ~UVector64();

    /**
     * Assign this object to another (make this a copy of 'other').
     * Use the 'assign' function to assign each element.
     */
    வெறுமை assign(மாறிலி UVector64& other, UErrorCode &ec);

    /**
     * Compare this vector with another.  They will be considered
     * equal if they are of the same size and all elements are equal,
     * as compared using this object's comparer.
     */
    bool operator==(மாறிலி UVector64& other);

    /**
     * Equivalent to !operator==()
     */
    உள்வரி bool operator!=(மாறிலி UVector64& other);

    //------------------------------------------------------------
    // subset of java.util.Vector API
    //------------------------------------------------------------

    உள்வரி வெறுமை addElement(int64_t elem, UErrorCode &status);

    வெறுமை setElementAt(int64_t elem, int32_t index);

    வெறுமை செருகு_Element_At(int64_t elem, int32_t index, UErrorCode &status);
    
    உள்வரி int64_t elementAti(int32_t index) மாறிலி;

    //UBool equals(const UVector64 &other) const;

    உள்வரி int64_t lastElementi() மாறிலி;

    //int32_t indexOf(int64_t elem, int32_t startIndex = 0) const;

    //UBool contains(int64_t elem) const;

    //UBool containsAll(const UVector64& other) const;

    //UBool removeAll(const UVector64& other);

    //UBool retainAll(const UVector64& other);

    //void removeElementAt(int32_t index);

    வெறுமை removeAllElements();

    உள்வரி int32_t size() மாறிலி;

    உள்வரி UBool isEmpty() மாறிலி { திரும்பு மொத்தம் == 0; }

    // Inline.  Use this one for speedy size check.
    உள்வரி UBool ensureCapacity(int32_t minimumCapacity, UErrorCode &status);

    // Out-of-line, handles actual growth.  Called by ensureCapacity() when necessary.
    UBool expandCapacity(int32_t minimumCapacity, UErrorCode &status);

    /**
     * Change the size of this vector as follows: If newSize is
     * smaller, then truncate the array, possibly deleting held
     * elements for i >= newSize.  If newSize is larger, grow the
     * array, filling in new slows with zero.
     */
    வெறுமை setSize(int32_t newSize);

    //------------------------------------------------------------
    // New API
    //------------------------------------------------------------

    //UBool containsNone(const UVector64& other) const;


    //void sortedInsert(int64_t elem, UErrorCode& ec);

    /**
     * Returns a pointer to the internal array holding the vector.
     */
    உள்வரி int64_t *getBuffer() மாறிலி;

    /**
     * Set the maximum allowed buffer capacity for this vector/stack.
     * Default with no limit set is unlimited, go until malloc() fails.
     * A Limit of zero means unlimited capacity.
     * Units are vector elements (64 bits each), not bytes.
     */
    வெறுமை setMaxCapacity(int32_t limit);

    /**
     * ICU "poor man's RTTI", returns a UClassID for this class.
     */
    நிலையான UClassID U_EXPORT2 getStaticClassID();

    /**
     * ICU "poor man's RTTI", returns a UClassID for the actual class.
     */
    மெய்நிகர் UClassID getDynamicClassID() மாறிலி override;

தனிப்பட்ட:
    வெறுமை _init(int32_t initialCapacity, UErrorCode &status);

    // Disallow
    UVector64(மாறிலி UVector64&) = delete;

    // Disallow
    UVector64& operator=(மாறிலி UVector64&) = delete;


    //  API Functions for Stack operations.
    //  In the original UVector, these were in a separate derived class, UStack.
    //  Here in UVector64, they are all together.
பொது:
    //UBool empty() const;   // TODO:  redundant, same as empty().  Remove it?

    //int64_t peeki() const;
    
    உள்வரி int64_t popi();
    
    உள்வரி int64_t தள்ளு(int64_t i, UErrorCode &status);

    உள்வரி int64_t *reserveBlock(int32_t size, UErrorCode &status);
    உள்வரி int64_t *எடு_Frame(int32_t size);
};


// UVector64 inlines

உள்வரி UBool UVector64::ensureCapacity(int32_t minimumCapacity, UErrorCode &status) {
    எனில் ((minimumCapacity >= 0) && (capacity >= minimumCapacity)) {
        திரும்பு true;
    } இல்லையெனில் {
        திரும்பு expandCapacity(minimumCapacity, status);
    }
}

உள்வரி int64_t UVector64::elementAti(int32_t index) மாறிலி {
    திரும்பு (0 <= index && index < மொத்தம்) ? elements[index] : 0;
}


உள்வரி வெறுமை UVector64::addElement(int64_t elem, UErrorCode &status) {
    எனில் (ensureCapacity(மொத்தம் + 1, status)) {
        elements[மொத்தம்] = elem;
        மொத்தம்++;
    }
}

உள்வரி int64_t *UVector64::reserveBlock(int32_t size, UErrorCode &status) {
    எனில் (ensureCapacity(மொத்தம்+size, status) == false) {
        திரும்பு nullptr;
    }
    int64_t  *rp = elements+மொத்தம்;
    மொத்தம் += size;
    திரும்பு rp;
}

உள்வரி int64_t *UVector64::எடு_Frame(int32_t size) {
    U_ASSERT(மொத்தம் >= size);
    மொத்தம் -= size;
    எனில் (மொத்தம் < 0) {
        மொத்தம் = 0;
    }
    திரும்பு elements+மொத்தம்-size;
}



உள்வரி int32_t UVector64::size() மாறிலி {
    திரும்பு மொத்தம்;
}

உள்வரி int64_t UVector64::lastElementi() மாறிலி {
    திரும்பு elementAti(மொத்தம்-1);
}

உள்வரி bool UVector64::operator!=(மாறிலி UVector64& other) {
    திரும்பு !operator==(other);
}

உள்வரி int64_t *UVector64::getBuffer() மாறிலி {
    திரும்பு elements;
}


// UStack inlines

உள்வரி int64_t UVector64::தள்ளு(int64_t i, UErrorCode &status) {
    addElement(i, status);
    திரும்பு i;
}

உள்வரி int64_t UVector64::popi() {
    int64_t result = 0;
    எனில் (மொத்தம் > 0) {
        மொத்தம்--;
        result = elements[மொத்தம்];
    }
    திரும்பு result;
}

U_NAMESPACE_END

#endif
