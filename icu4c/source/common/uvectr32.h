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

#ifndef UVECTOR32_H
#define UVECTOR32_H

#अन्तर्भाव "unicode/utypes.h"
#अन्तर्भाव "unicode/uobject.h"
#अन्तर्भाव "uhash.h"
#अन्तर्भाव "uassert.h"

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
class U_COMMON_API UVector32 : सार्वजनिक UObject {
गुप्त:
    int32_t   गन्ती;

    int32_t   capacity;
    
    int32_t   maxCapacity;   // Limit beyond which capacity is not permitted to grow.

    int32_t*  elements;

सार्वजनिक:
    UVector32(UErrorCode &status);

    UVector32(int32_t initialCapacity, UErrorCode &status);

    आभास ~UVector32();

    /**
     * Assign this object to another (make this a copy of 'other').
     * Use the 'assign' function to assign each element.
     */
    शून्य assign(अचर UVector32& other, UErrorCode &ec);

    /**
     * Compare this vector with another.  They will be considered
     * equal if they are of the same size and all elements are equal,
     * as compared using this object's comparer.
     */
    bool चालक==(अचर UVector32& other) अचर;

    /**
     * Equivalent to !operator==()
     */
    अन्तःस्थ bool चालक!=(अचर UVector32& other) अचर;

    //------------------------------------------------------------
    // java.util.Vector API
    //------------------------------------------------------------

    अन्तःस्थ शून्य addElement(int32_t elem, UErrorCode &status);

    शून्य setElementAt(int32_t elem, int32_t index);

    शून्य घुसाउ_Element_At(int32_t elem, int32_t index, UErrorCode &status);
    
    अन्तःस्थ int32_t elementAti(int32_t index) अचर;

    UBool equals(अचर UVector32 &other) अचर;

    अन्तःस्थ int32_t lastElementi() अचर;

    int32_t indexOf(int32_t elem, int32_t startIndex = 0) अचर;

    अन्तःस्थ UBool contains(int32_t elem) अचर;

    UBool containsAll(अचर UVector32& other) अचर;

    UBool removeAll(अचर UVector32& other);

    UBool retainAll(अचर UVector32& other);

    शून्य removeElementAt(int32_t index);

    शून्य removeAllElements();

    अन्तःस्थ int32_t size() अचर;

    अन्तःस्थ UBool isEmpty() अचर;

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

    /**
     * Returns true if this vector contains none of the elements
     * of the given vector.
     * @param other vector to be checked for containment
     * @return true if the test condition is met
     */
    UBool containsNone(अचर UVector32& other) अचर;


    /**
     * Insert the given integer into this vector at its sorted position.
     * The current elements are assumed to be sorted already.
     */
    शून्य sorted_घुसाउ(int32_t elem, UErrorCode& ec);

    /**
     * Returns a pointer to the internal array holding the vector.
     */
    अन्तःस्थ int32_t *getBuffer() अचर;

    /**
     * Set the maximum allowed buffer capacity for this vector/stack.
     * Default with no limit set is unlimited, go until malloc() fails.
     * A Limit of zero means unlimited capacity.
     * Units are vector elements (32 bits each), not bytes.
     */
    शून्य setMaxCapacity(int32_t limit);

    /**
     * ICU "poor man's RTTI", returns a UClassID for this class.
     */
    स्थिर UClassID U_EXPORT2 getStaticClassID();

    /**
     * ICU "poor man's RTTI", returns a UClassID for the actual class.
     */
    आभास UClassID getDynamicClassID() अचर override;

गुप्त:
    शून्य _init(int32_t initialCapacity, UErrorCode &status);

    // Disallow
    UVector32(अचर UVector32&) = delete;

    // Disallow
    UVector32& चालक=(अचर UVector32&) = delete;


    //  API Functions for Stack operations.
    //  In the original UVector, these were in a separate derived class, UStack.
    //  Here in UVector32, they are all together.
सार्वजनिक:
    अन्तःस्थ UBool empty() अचर;   // TODO:  redundant, same as empty().  Remove it?

    अन्तःस्थ int32_t peeki() अचर;
    
    अन्तःस्थ int32_t popi();
    
    अन्तःस्थ int32_t थप(int32_t i, UErrorCode &status);

    अन्तःस्थ int32_t *reserveBlock(int32_t size, UErrorCode &status);
    अन्तःस्थ int32_t *झिक_Frame(int32_t size);
};


// UVector32 inlines

अन्तःस्थ UBool UVector32::ensureCapacity(int32_t minimumCapacity, UErrorCode &status) {
    यदि ((minimumCapacity >= 0) && (capacity >= minimumCapacity)) {
        फर्काऊ true;
    } अन्यथा {
        फर्काऊ expandCapacity(minimumCapacity, status);
    }
}

अन्तःस्थ int32_t UVector32::elementAti(int32_t index) अचर {
    फर्काऊ (index >= 0 && गन्ती > 0 && गन्ती - index > 0) ? elements[index] : 0;
}


अन्तःस्थ शून्य UVector32::addElement(int32_t elem, UErrorCode &status) {
    यदि (ensureCapacity(गन्ती + 1, status)) {
        elements[गन्ती] = elem;
        गन्ती++;
    }
}

अन्तःस्थ int32_t *UVector32::reserveBlock(int32_t size, UErrorCode &status) {
    यदि (ensureCapacity(गन्ती+size, status) == false) {
        फर्काऊ nullptr;
    }
    int32_t  *rp = elements+गन्ती;
    गन्ती += size;
    फर्काऊ rp;
}

अन्तःस्थ int32_t *UVector32::झिक_Frame(int32_t size) {
    U_ASSERT(गन्ती >= size);
    गन्ती -= size;
    यदि (गन्ती < 0) {
        गन्ती = 0;
    }
    फर्काऊ elements+गन्ती-size;
}



अन्तःस्थ int32_t UVector32::size() अचर {
    फर्काऊ गन्ती;
}

अन्तःस्थ UBool UVector32::isEmpty() अचर {
    फर्काऊ गन्ती == 0;
}

अन्तःस्थ UBool UVector32::contains(int32_t obj) अचर {
    फर्काऊ indexOf(obj) >= 0;
}

अन्तःस्थ int32_t UVector32::lastElementi() अचर {
    फर्काऊ elementAti(गन्ती-1);
}

अन्तःस्थ bool UVector32::चालक!=(अचर UVector32& other) अचर {
    फर्काऊ !चालक==(other);
}

अन्तःस्थ int32_t *UVector32::getBuffer() अचर {
    फर्काऊ elements;
}


// UStack inlines

अन्तःस्थ UBool UVector32::empty() अचर {
    फर्काऊ isEmpty();
}

अन्तःस्थ int32_t UVector32::peeki() अचर {
    फर्काऊ lastElementi();
}

अन्तःस्थ int32_t UVector32::थप(int32_t i, UErrorCode &status) {
    addElement(i, status);
    फर्काऊ i;
}

अन्तःस्थ int32_t UVector32::popi() {
    int32_t result = 0;
    यदि (गन्ती > 0) {
        गन्ती--;
        result = elements[गन्ती];
    }
    फर्काऊ result;
}

U_NAMESPACE_END

#endif
