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

#અન્તર્ભાવ "unicode/utypes.h"
#અન્તર્ભાવ "unicode/uobject.h"
#અન્તર્ભાવ "uhash.h"
#અન્તર્ભાવ "uassert.h"

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
class U_COMMON_API UVector32 : જાહેર UObject {
ખાનગી:
    int32_t   ગણતરી;

    int32_t   capacity;
    
    int32_t   maxCapacity;   // Limit beyond which capacity is not permitted to grow.

    int32_t*  elements;

જાહેર:
    UVector32(UErrorCode &status);

    UVector32(int32_t initialCapacity, UErrorCode &status);

    આભાસી ~UVector32();

    /**
     * Assign this object to another (make this a copy of 'other').
     * Use the 'assign' function to assign each element.
     */
    શૂન્ય assign(અચલ UVector32& other, UErrorCode &ec);

    /**
     * Compare this vector with another.  They will be considered
     * equal if they are of the same size and all elements are equal,
     * as compared using this object's comparer.
     */
    bool operator==(અચલ UVector32& other) અચલ;

    /**
     * Equivalent to !operator==()
     */
    અંતઃરેખ bool operator!=(અચલ UVector32& other) અચલ;

    //------------------------------------------------------------
    // java.util.Vector API
    //------------------------------------------------------------

    અંતઃરેખ શૂન્ય addElement(int32_t elem, UErrorCode &status);

    શૂન્ય setElementAt(int32_t elem, int32_t index);

    શૂન્ય ઉમેરો_Element_At(int32_t elem, int32_t index, UErrorCode &status);
    
    અંતઃરેખ int32_t elementAti(int32_t index) અચલ;

    UBool equals(અચલ UVector32 &other) અચલ;

    અંતઃરેખ int32_t lastElementi() અચલ;

    int32_t indexOf(int32_t elem, int32_t startIndex = 0) અચલ;

    અંતઃરેખ UBool contains(int32_t elem) અચલ;

    UBool containsAll(અચલ UVector32& other) અચલ;

    UBool removeAll(અચલ UVector32& other);

    UBool retainAll(અચલ UVector32& other);

    શૂન્ય removeElementAt(int32_t index);

    શૂન્ય removeAllElements();

    અંતઃરેખ int32_t size() અચલ;

    અંતઃરેખ UBool isEmpty() અચલ;

    // Inline.  Use this one for speedy size check.
    અંતઃરેખ UBool ensureCapacity(int32_t minimumCapacity, UErrorCode &status);

    // Out-of-line, handles actual growth.  Called by ensureCapacity() when necessary.
    UBool expandCapacity(int32_t minimumCapacity, UErrorCode &status);

    /**
     * Change the size of this vector as follows: If newSize is
     * smaller, then truncate the array, possibly deleting held
     * elements for i >= newSize.  If newSize is larger, grow the
     * array, filling in new slows with zero.
     */
    શૂન્ય setSize(int32_t newSize);

    //------------------------------------------------------------
    // New API
    //------------------------------------------------------------

    /**
     * Returns true if this vector contains none of the elements
     * of the given vector.
     * @param other vector to be checked for containment
     * @return true if the test condition is met
     */
    UBool containsNone(અચલ UVector32& other) અચલ;


    /**
     * Insert the given integer into this vector at its sorted position.
     * The current elements are assumed to be sorted already.
     */
    શૂન્ય sorted_ઉમેરો(int32_t elem, UErrorCode& ec);

    /**
     * Returns a pointer to the internal array holding the vector.
     */
    અંતઃરેખ int32_t *getBuffer() અચલ;

    /**
     * Set the maximum allowed buffer capacity for this vector/stack.
     * Default with no limit set is unlimited, go until malloc() fails.
     * A Limit of zero means unlimited capacity.
     * Units are vector elements (32 bits each), not bytes.
     */
    શૂન્ય setMaxCapacity(int32_t limit);

    /**
     * ICU "poor man's RTTI", returns a UClassID for this class.
     */
    સ્થિર UClassID U_EXPORT2 getStaticClassID();

    /**
     * ICU "poor man's RTTI", returns a UClassID for the actual class.
     */
    આભાસી UClassID getDynamicClassID() અચલ override;

ખાનગી:
    શૂન્ય _init(int32_t initialCapacity, UErrorCode &status);

    // Disallow
    UVector32(અચલ UVector32&) = delete;

    // Disallow
    UVector32& operator=(અચલ UVector32&) = delete;


    //  API Functions for Stack operations.
    //  In the original UVector, these were in a separate derived class, UStack.
    //  Here in UVector32, they are all together.
જાહેર:
    અંતઃરેખ UBool empty() અચલ;   // TODO:  redundant, same as empty().  Remove it?

    અંતઃરેખ int32_t peeki() અચલ;
    
    અંતઃરેખ int32_t popi();
    
    અંતઃરેખ int32_t ધકેલો(int32_t i, UErrorCode &status);

    અંતઃરેખ int32_t *reserveBlock(int32_t size, UErrorCode &status);
    અંતઃરેખ int32_t *કાઢો_Frame(int32_t size);
};


// UVector32 inlines

અંતઃરેખ UBool UVector32::ensureCapacity(int32_t minimumCapacity, UErrorCode &status) {
    જો ((minimumCapacity >= 0) && (capacity >= minimumCapacity)) {
        પાછા true;
    } નહીંતર {
        પાછા expandCapacity(minimumCapacity, status);
    }
}

અંતઃરેખ int32_t UVector32::elementAti(int32_t index) અચલ {
    પાછા (index >= 0 && ગણતરી > 0 && ગણતરી - index > 0) ? elements[index] : 0;
}


અંતઃરેખ શૂન્ય UVector32::addElement(int32_t elem, UErrorCode &status) {
    જો (ensureCapacity(ગણતરી + 1, status)) {
        elements[ગણતરી] = elem;
        ગણતરી++;
    }
}

અંતઃરેખ int32_t *UVector32::reserveBlock(int32_t size, UErrorCode &status) {
    જો (ensureCapacity(ગણતરી+size, status) == false) {
        પાછા nullptr;
    }
    int32_t  *rp = elements+ગણતરી;
    ગણતરી += size;
    પાછા rp;
}

અંતઃરેખ int32_t *UVector32::કાઢો_Frame(int32_t size) {
    U_ASSERT(ગણતરી >= size);
    ગણતરી -= size;
    જો (ગણતરી < 0) {
        ગણતરી = 0;
    }
    પાછા elements+ગણતરી-size;
}



અંતઃરેખ int32_t UVector32::size() અચલ {
    પાછા ગણતરી;
}

અંતઃરેખ UBool UVector32::isEmpty() અચલ {
    પાછા ગણતરી == 0;
}

અંતઃરેખ UBool UVector32::contains(int32_t obj) અચલ {
    પાછા indexOf(obj) >= 0;
}

અંતઃરેખ int32_t UVector32::lastElementi() અચલ {
    પાછા elementAti(ગણતરી-1);
}

અંતઃરેખ bool UVector32::operator!=(અચલ UVector32& other) અચલ {
    પાછા !operator==(other);
}

અંતઃરેખ int32_t *UVector32::getBuffer() અચલ {
    પાછા elements;
}


// UStack inlines

અંતઃરેખ UBool UVector32::empty() અચલ {
    પાછા isEmpty();
}

અંતઃરેખ int32_t UVector32::peeki() અચલ {
    પાછા lastElementi();
}

અંતઃરેખ int32_t UVector32::ધકેલો(int32_t i, UErrorCode &status) {
    addElement(i, status);
    પાછા i;
}

અંતઃરેખ int32_t UVector32::popi() {
    int32_t result = 0;
    જો (ગણતરી > 0) {
        ગણતરી--;
        result = elements[ગણતરી];
    }
    પાછા result;
}

U_NAMESPACE_END

#endif
