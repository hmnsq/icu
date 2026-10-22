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

#અન્તર્ભાવ "unicode/utypes.h"
#અન્તર્ભાવ "unicode/uobject.h"
#અન્તર્ભાવ "uhash.h"
#અન્તર્ભાવ "uassert.h"

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
class U_COMMON_API UVector64 : જાહેર UObject {
ખાનગી:
    int32_t   ગણતરી;

    int32_t   capacity;
    
    int32_t   maxCapacity;   // Limit beyond which capacity is not permitted to grow.

    int64_t*  elements;

જાહેર:
    UVector64(UErrorCode &status);

    UVector64(int32_t initialCapacity, UErrorCode &status);

    આભાસી ~UVector64();

    /**
     * Assign this object to another (make this a copy of 'other').
     * Use the 'assign' function to assign each element.
     */
    શૂન્ય assign(અચલ UVector64& other, UErrorCode &ec);

    /**
     * Compare this vector with another.  They will be considered
     * equal if they are of the same size and all elements are equal,
     * as compared using this object's comparer.
     */
    bool operator==(અચલ UVector64& other);

    /**
     * Equivalent to !operator==()
     */
    અંતઃરેખ bool operator!=(અચલ UVector64& other);

    //------------------------------------------------------------
    // subset of java.util.Vector API
    //------------------------------------------------------------

    અંતઃરેખ શૂન્ય addElement(int64_t elem, UErrorCode &status);

    શૂન્ય setElementAt(int64_t elem, int32_t index);

    શૂન્ય ઉમેરો_Element_At(int64_t elem, int32_t index, UErrorCode &status);
    
    અંતઃરેખ int64_t elementAti(int32_t index) અચલ;

    //UBool equals(const UVector64 &other) const;

    અંતઃરેખ int64_t lastElementi() અચલ;

    //int32_t indexOf(int64_t elem, int32_t startIndex = 0) const;

    //UBool contains(int64_t elem) const;

    //UBool containsAll(const UVector64& other) const;

    //UBool removeAll(const UVector64& other);

    //UBool retainAll(const UVector64& other);

    //void removeElementAt(int32_t index);

    શૂન્ય removeAllElements();

    અંતઃરેખ int32_t size() અચલ;

    અંતઃરેખ UBool isEmpty() અચલ { પાછા ગણતરી == 0; }

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

    //UBool containsNone(const UVector64& other) const;


    //void sortedInsert(int64_t elem, UErrorCode& ec);

    /**
     * Returns a pointer to the internal array holding the vector.
     */
    અંતઃરેખ int64_t *getBuffer() અચલ;

    /**
     * Set the maximum allowed buffer capacity for this vector/stack.
     * Default with no limit set is unlimited, go until malloc() fails.
     * A Limit of zero means unlimited capacity.
     * Units are vector elements (64 bits each), not bytes.
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
    UVector64(અચલ UVector64&) = delete;

    // Disallow
    UVector64& operator=(અચલ UVector64&) = delete;


    //  API Functions for Stack operations.
    //  In the original UVector, these were in a separate derived class, UStack.
    //  Here in UVector64, they are all together.
જાહેર:
    //UBool empty() const;   // TODO:  redundant, same as empty().  Remove it?

    //int64_t peeki() const;
    
    અંતઃરેખ int64_t popi();
    
    અંતઃરેખ int64_t ધકેલો(int64_t i, UErrorCode &status);

    અંતઃરેખ int64_t *reserveBlock(int32_t size, UErrorCode &status);
    અંતઃરેખ int64_t *કાઢો_Frame(int32_t size);
};


// UVector64 inlines

અંતઃરેખ UBool UVector64::ensureCapacity(int32_t minimumCapacity, UErrorCode &status) {
    જો ((minimumCapacity >= 0) && (capacity >= minimumCapacity)) {
        પાછા true;
    } નહીંતર {
        પાછા expandCapacity(minimumCapacity, status);
    }
}

અંતઃરેખ int64_t UVector64::elementAti(int32_t index) અચલ {
    પાછા (0 <= index && index < ગણતરી) ? elements[index] : 0;
}


અંતઃરેખ શૂન્ય UVector64::addElement(int64_t elem, UErrorCode &status) {
    જો (ensureCapacity(ગણતરી + 1, status)) {
        elements[ગણતરી] = elem;
        ગણતરી++;
    }
}

અંતઃરેખ int64_t *UVector64::reserveBlock(int32_t size, UErrorCode &status) {
    જો (ensureCapacity(ગણતરી+size, status) == false) {
        પાછા nullptr;
    }
    int64_t  *rp = elements+ગણતરી;
    ગણતરી += size;
    પાછા rp;
}

અંતઃરેખ int64_t *UVector64::કાઢો_Frame(int32_t size) {
    U_ASSERT(ગણતરી >= size);
    ગણતરી -= size;
    જો (ગણતરી < 0) {
        ગણતરી = 0;
    }
    પાછા elements+ગણતરી-size;
}



અંતઃરેખ int32_t UVector64::size() અચલ {
    પાછા ગણતરી;
}

અંતઃરેખ int64_t UVector64::lastElementi() અચલ {
    પાછા elementAti(ગણતરી-1);
}

અંતઃરેખ bool UVector64::operator!=(અચલ UVector64& other) {
    પાછા !operator==(other);
}

અંતઃરેખ int64_t *UVector64::getBuffer() અચલ {
    પાછા elements;
}


// UStack inlines

અંતઃરેખ int64_t UVector64::ધકેલો(int64_t i, UErrorCode &status) {
    addElement(i, status);
    પાછા i;
}

અંતઃરેખ int64_t UVector64::popi() {
    int64_t result = 0;
    જો (ગણતરી > 0) {
        ગણતરી--;
        result = elements[ગણતરી];
    }
    પાછા result;
}

U_NAMESPACE_END

#endif
