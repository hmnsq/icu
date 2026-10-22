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
खाजगी:
    int32_t   मोजणी;

    int32_t   capacity;
    
    int32_t   maxCapacity;   // Limit beyond which capacity is not permitted to grow.

    int32_t*  elements;

सार्वजनिक:
    UVector32(UErrorCode &status);

    UVector32(int32_t initialCapacity, UErrorCode &status);

    आभासी ~UVector32();

    /**
     * Assign this object to another (make this a copy of 'other').
     * Use the 'assign' function to assign each element.
     */
    शून्य assign(स्थिरांक UVector32& other, UErrorCode &ec);

    /**
     * Compare this vector with another.  They will be considered
     * equal if they are of the same size and all elements are equal,
     * as compared using this object's comparer.
     */
    bool चालक==(स्थिरांक UVector32& other) स्थिरांक;

    /**
     * Equivalent to !operator==()
     */
    ओळीत bool चालक!=(स्थिरांक UVector32& other) स्थिरांक;

    //------------------------------------------------------------
    // java.util.Vector API
    //------------------------------------------------------------

    ओळीत शून्य addElement(int32_t elem, UErrorCode &status);

    शून्य setElementAt(int32_t elem, int32_t index);

    शून्य घाला_Element_At(int32_t elem, int32_t index, UErrorCode &status);
    
    ओळीत int32_t elementAti(int32_t index) स्थिरांक;

    UBool equals(स्थिरांक UVector32 &other) स्थिरांक;

    ओळीत int32_t lastElementi() स्थिरांक;

    int32_t indexOf(int32_t elem, int32_t startIndex = 0) स्थिरांक;

    ओळीत UBool contains(int32_t elem) स्थिरांक;

    UBool containsAll(स्थिरांक UVector32& other) स्थिरांक;

    UBool removeAll(स्थिरांक UVector32& other);

    UBool retainAll(स्थिरांक UVector32& other);

    शून्य removeElementAt(int32_t index);

    शून्य removeAllElements();

    ओळीत int32_t size() स्थिरांक;

    ओळीत UBool isEmpty() स्थिरांक;

    // Inline.  Use this one for speedy size check.
    ओळीत UBool ensureCapacity(int32_t minimumCapacity, UErrorCode &status);

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
    UBool containsNone(स्थिरांक UVector32& other) स्थिरांक;


    /**
     * Insert the given integer into this vector at its sorted position.
     * The current elements are assumed to be sorted already.
     */
    शून्य sorted_घाला(int32_t elem, UErrorCode& ec);

    /**
     * Returns a pointer to the internal array holding the vector.
     */
    ओळीत int32_t *getBuffer() स्थिरांक;

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
    आभासी UClassID getDynamicClassID() स्थिरांक override;

खाजगी:
    शून्य _init(int32_t initialCapacity, UErrorCode &status);

    // Disallow
    UVector32(स्थिरांक UVector32&) = delete;

    // Disallow
    UVector32& चालक=(स्थिरांक UVector32&) = delete;


    //  API Functions for Stack operations.
    //  In the original UVector, these were in a separate derived class, UStack.
    //  Here in UVector32, they are all together.
सार्वजनिक:
    ओळीत UBool empty() स्थिरांक;   // TODO:  redundant, same as empty().  Remove it?

    ओळीत int32_t peeki() स्थिरांक;
    
    ओळीत int32_t popi();
    
    ओळीत int32_t ढकला(int32_t i, UErrorCode &status);

    ओळीत int32_t *reserveBlock(int32_t size, UErrorCode &status);
    ओळीत int32_t *काढा_Frame(int32_t size);
};


// UVector32 inlines

ओळीत UBool UVector32::ensureCapacity(int32_t minimumCapacity, UErrorCode &status) {
    जर ((minimumCapacity >= 0) && (capacity >= minimumCapacity)) {
        परत true;
    } अन्यथा {
        परत expandCapacity(minimumCapacity, status);
    }
}

ओळीत int32_t UVector32::elementAti(int32_t index) स्थिरांक {
    परत (index >= 0 && मोजणी > 0 && मोजणी - index > 0) ? elements[index] : 0;
}


ओळीत शून्य UVector32::addElement(int32_t elem, UErrorCode &status) {
    जर (ensureCapacity(मोजणी + 1, status)) {
        elements[मोजणी] = elem;
        मोजणी++;
    }
}

ओळीत int32_t *UVector32::reserveBlock(int32_t size, UErrorCode &status) {
    जर (ensureCapacity(मोजणी+size, status) == false) {
        परत nullptr;
    }
    int32_t  *rp = elements+मोजणी;
    मोजणी += size;
    परत rp;
}

ओळीत int32_t *UVector32::काढा_Frame(int32_t size) {
    U_ASSERT(मोजणी >= size);
    मोजणी -= size;
    जर (मोजणी < 0) {
        मोजणी = 0;
    }
    परत elements+मोजणी-size;
}



ओळीत int32_t UVector32::size() स्थिरांक {
    परत मोजणी;
}

ओळीत UBool UVector32::isEmpty() स्थिरांक {
    परत मोजणी == 0;
}

ओळीत UBool UVector32::contains(int32_t obj) स्थिरांक {
    परत indexOf(obj) >= 0;
}

ओळीत int32_t UVector32::lastElementi() स्थिरांक {
    परत elementAti(मोजणी-1);
}

ओळीत bool UVector32::चालक!=(स्थिरांक UVector32& other) स्थिरांक {
    परत !चालक==(other);
}

ओळीत int32_t *UVector32::getBuffer() स्थिरांक {
    परत elements;
}


// UStack inlines

ओळीत UBool UVector32::empty() स्थिरांक {
    परत isEmpty();
}

ओळीत int32_t UVector32::peeki() स्थिरांक {
    परत lastElementi();
}

ओळीत int32_t UVector32::ढकला(int32_t i, UErrorCode &status) {
    addElement(i, status);
    परत i;
}

ओळीत int32_t UVector32::popi() {
    int32_t result = 0;
    जर (मोजणी > 0) {
        मोजणी--;
        result = elements[मोजणी];
    }
    परत result;
}

U_NAMESPACE_END

#endif
