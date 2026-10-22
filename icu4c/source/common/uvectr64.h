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

#अन्तर्भाव "unicode/utypes.h"
#अन्तर्भाव "unicode/uobject.h"
#अन्तर्भाव "uhash.h"
#अन्तर्भाव "uassert.h"

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
class U_COMMON_API UVector64 : सार्वजनिक UObject {
गुप्त:
    int32_t   गणनम्;

    int32_t   capacity;
    
    int32_t   maxCapacity;   // Limit beyond which capacity is not permitted to grow.

    int64_t*  elements;

सार्वजनिक:
    UVector64(UErrorCode &status);

    UVector64(int32_t initialCapacity, UErrorCode &status);

    आभास ~UVector64();

    /**
     * Assign this object to another (make this a copy of 'other').
     * Use the 'assign' function to assign each element.
     */
    शून्य assign(ध्रुव UVector64& other, UErrorCode &ec);

    /**
     * Compare this vector with another.  They will be considered
     * equal if they are of the same size and all elements are equal,
     * as compared using this object's comparer.
     */
    bool operator==(ध्रुव UVector64& other);

    /**
     * Equivalent to !operator==()
     */
    अन्तःस्थ bool operator!=(ध्रुव UVector64& other);

    //------------------------------------------------------------
    // subset of java.util.Vector API
    //------------------------------------------------------------

    अन्तःस्थ शून्य addElement(int64_t elem, UErrorCode &status);

    शून्य setElementAt(int64_t elem, int32_t index);

    शून्य निवेशय_Element_At(int64_t elem, int32_t index, UErrorCode &status);
    
    अन्तःस्थ int64_t elementAti(int32_t index) ध्रुव;

    //UBool equals(const UVector64 &other) const;

    अन्तःस्थ int64_t lastElementi() ध्रुव;

    //int32_t indexOf(int64_t elem, int32_t startIndex = 0) const;

    //UBool contains(int64_t elem) const;

    //UBool containsAll(const UVector64& other) const;

    //UBool removeAll(const UVector64& other);

    //UBool retainAll(const UVector64& other);

    //void removeElementAt(int32_t index);

    शून्य removeAllElements();

    अन्तःस्थ int32_t size() ध्रुव;

    अन्तःस्थ UBool isEmpty() ध्रुव { पटिनिवत्त गणनम् == 0; }

    // Inline.  Use this one for speedy size check.
    अन्तःस्थ UBool ensureCapacity(int32_t minimumCapacity, UErrorCode &status);

    // Out-of-line, handles actual growth.  Called by ensureCapacity() when necessary.
    UBool expandCapacity(int32_t minimumCapacity, UErrorCode &status);

    /**
     * Change the size of this vector as follows: If newSize is
     * smaller, then truncate the array, possibly deleting held
     * elements for i >= newSize.  If newSize is larger, grow the
     * array, filling in new slows with zero.
     */
    शून्य setSize(int32_t newSize);

    //------------------------------------------------------------
    // New API
    //------------------------------------------------------------

    //UBool containsNone(const UVector64& other) const;


    //void sortedInsert(int64_t elem, UErrorCode& ec);

    /**
     * Returns a pointer to the internal array holding the vector.
     */
    अन्तःस्थ int64_t *getBuffer() ध्रुव;

    /**
     * Set the maximum allowed buffer capacity for this vector/stack.
     * Default with no limit set is unlimited, go until malloc() fails.
     * A Limit of zero means unlimited capacity.
     * Units are vector elements (64 bits each), not bytes.
     */
    शून्य setMaxCapacity(int32_t limit);

    /**
     * ICU "poor man's RTTI", returns a UClassID for this class.
     */
    स्थिर UClassID U_EXPORT2 getStaticClassID();

    /**
     * ICU "poor man's RTTI", returns a UClassID for the actual class.
     */
    आभास UClassID getDynamicClassID() ध्रुव override;

गुप्त:
    शून्य _init(int32_t initialCapacity, UErrorCode &status);

    // Disallow
    UVector64(ध्रुव UVector64&) = delete;

    // Disallow
    UVector64& operator=(ध्रुव UVector64&) = delete;


    //  API Functions for Stack operations.
    //  In the original UVector, these were in a separate derived class, UStack.
    //  Here in UVector64, they are all together.
सार्वजनिक:
    //UBool empty() const;   // TODO:  redundant, same as empty().  Remove it?

    //int64_t peeki() const;
    
    अन्तःस्थ int64_t popi();
    
    अन्तःस्थ int64_t प्रक्षिप(int64_t i, UErrorCode &status);

    अन्तःस्थ int64_t *reserveBlock(int32_t size, UErrorCode &status);
    अन्तःस्थ int64_t *निष्कासय_Frame(int32_t size);
};


// UVector64 inlines

अन्तःस्थ UBool UVector64::ensureCapacity(int32_t minimumCapacity, UErrorCode &status) {
    सचे ((minimumCapacity >= 0) && (capacity >= minimumCapacity)) {
        पटिनिवत्त true;
    } अञ्ञथा {
        पटिनिवत्त expandCapacity(minimumCapacity, status);
    }
}

अन्तःस्थ int64_t UVector64::elementAti(int32_t index) ध्रुव {
    पटिनिवत्त (0 <= index && index < गणनम्) ? elements[index] : 0;
}


अन्तःस्थ शून्य UVector64::addElement(int64_t elem, UErrorCode &status) {
    सचे (ensureCapacity(गणनम् + 1, status)) {
        elements[गणनम्] = elem;
        गणनम्++;
    }
}

अन्तःस्थ int64_t *UVector64::reserveBlock(int32_t size, UErrorCode &status) {
    सचे (ensureCapacity(गणनम्+size, status) == false) {
        पटिनिवत्त nullptr;
    }
    int64_t  *rp = elements+गणनम्;
    गणनम् += size;
    पटिनिवत्त rp;
}

अन्तःस्थ int64_t *UVector64::निष्कासय_Frame(int32_t size) {
    U_ASSERT(गणनम् >= size);
    गणनम् -= size;
    सचे (गणनम् < 0) {
        गणनम् = 0;
    }
    पटिनिवत्त elements+गणनम्-size;
}



अन्तःस्थ int32_t UVector64::size() ध्रुव {
    पटिनिवत्त गणनम्;
}

अन्तःस्थ int64_t UVector64::lastElementi() ध्रुव {
    पटिनिवत्त elementAti(गणनम्-1);
}

अन्तःस्थ bool UVector64::operator!=(ध्रुव UVector64& other) {
    पटिनिवत्त !operator==(other);
}

अन्तःस्थ int64_t *UVector64::getBuffer() ध्रुव {
    पटिनिवत्त elements;
}


// UStack inlines

अन्तःस्थ int64_t UVector64::प्रक्षिप(int64_t i, UErrorCode &status) {
    addElement(i, status);
    पटिनिवत्त i;
}

अन्तःस्थ int64_t UVector64::popi() {
    int64_t result = 0;
    सचे (गणनम् > 0) {
        गणनम्--;
        result = elements[गणनम्];
    }
    पटिनिवत्त result;
}

U_NAMESPACE_END

#endif
