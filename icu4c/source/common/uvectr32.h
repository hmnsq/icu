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

#ଅନ୍ତର୍ଭାଵ "unicode/utypes.h"
#ଅନ୍ତର୍ଭାଵ "unicode/uobject.h"
#ଅନ୍ତର୍ଭାଵ "uhash.h"
#ଅନ୍ତର୍ଭାଵ "uassert.h"

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
class U_COMMON_API UVector32 : ସର୍ବସାଧାରଣ UObject {
ବ୍ୟକ୍ତିଗତ:
    int32_t   ଗଣନା_2;

    int32_t   capacity;
    
    int32_t   maxCapacity;   // Limit beyond which capacity is not permitted to grow.

    int32_t*  elements;

ସର୍ବସାଧାରଣ:
    UVector32(UErrorCode &status);

    UVector32(int32_t initialCapacity, UErrorCode &status);

    ଆଭାସୀ ~UVector32();

    /**
     * Assign this object to another (make this a copy of 'other').
     * Use the 'assign' function to assign each element.
     */
    ଶୂନ୍ୟ assign(ଅଚଳ UVector32& other, UErrorCode &ec);

    /**
     * Compare this vector with another.  They will be considered
     * equal if they are of the same size and all elements are equal,
     * as compared using this object's comparer.
     */
    bool operator==(ଅଚଳ UVector32& other) ଅଚଳ;

    /**
     * Equivalent to !operator==()
     */
    ଧାଡ଼ିଭିତରେ bool operator!=(ଅଚଳ UVector32& other) ଅଚଳ;

    //------------------------------------------------------------
    // java.util.Vector API
    //------------------------------------------------------------

    ଧାଡ଼ିଭିତରେ ଶୂନ୍ୟ addElement(int32_t elem, UErrorCode &status);

    ଶୂନ୍ୟ setElementAt(int32_t elem, int32_t index);

    ଶୂନ୍ୟ ଭର୍ତ୍ତି_Element_At(int32_t elem, int32_t index, UErrorCode &status);
    
    ଧାଡ଼ିଭିତରେ int32_t elementAti(int32_t index) ଅଚଳ;

    UBool equals(ଅଚଳ UVector32 &other) ଅଚଳ;

    ଧାଡ଼ିଭିତରେ int32_t lastElementi() ଅଚଳ;

    int32_t indexOf(int32_t elem, int32_t startIndex = 0) ଅଚଳ;

    ଧାଡ଼ିଭିତରେ UBool contains(int32_t elem) ଅଚଳ;

    UBool containsAll(ଅଚଳ UVector32& other) ଅଚଳ;

    UBool removeAll(ଅଚଳ UVector32& other);

    UBool retainAll(ଅଚଳ UVector32& other);

    ଶୂନ୍ୟ removeElementAt(int32_t index);

    ଶୂନ୍ୟ removeAllElements();

    ଧାଡ଼ିଭିତରେ int32_t size() ଅଚଳ;

    ଧାଡ଼ିଭିତରେ UBool isEmpty() ଅଚଳ;

    // Inline.  Use this one for speedy size check.
    ଧାଡ଼ିଭିତରେ UBool ensureCapacity(int32_t minimumCapacity, UErrorCode &status);

    // Out-of-line, handles actual growth.  Called by ensureCapacity() when necessary.
    UBool expandCapacity(int32_t minimumCapacity, UErrorCode &status);

    /**
     * Change the size of this vector as follows: If newSize is
     * smaller, then truncate the array, possibly deleting held
     * elements for i >= newSize.  If newSize is larger, grow the
     * array, filling in new slows with zero.
     */
    ଶୂନ୍ୟ setSize(int32_t newSize);

    //------------------------------------------------------------
    // New API
    //------------------------------------------------------------

    /**
     * Returns true if this vector contains none of the elements
     * of the given vector.
     * @param other vector to be checked for containment
     * @return true if the test condition is met
     */
    UBool containsNone(ଅଚଳ UVector32& other) ଅଚଳ;


    /**
     * Insert the given integer into this vector at its sorted position.
     * The current elements are assumed to be sorted already.
     */
    ଶୂନ୍ୟ sorted_ଭର୍ତ୍ତି(int32_t elem, UErrorCode& ec);

    /**
     * Returns a pointer to the internal array holding the vector.
     */
    ଧାଡ଼ିଭିତରେ int32_t *getBuffer() ଅଚଳ;

    /**
     * Set the maximum allowed buffer capacity for this vector/stack.
     * Default with no limit set is unlimited, go until malloc() fails.
     * A Limit of zero means unlimited capacity.
     * Units are vector elements (32 bits each), not bytes.
     */
    ଶୂନ୍ୟ setMaxCapacity(int32_t limit);

    /**
     * ICU "poor man's RTTI", returns a UClassID for this class.
     */
    ସ୍ଥିର UClassID U_EXPORT2 getStaticClassID();

    /**
     * ICU "poor man's RTTI", returns a UClassID for the actual class.
     */
    ଆଭାସୀ UClassID getDynamicClassID() ଅଚଳ override;

ବ୍ୟକ୍ତିଗତ:
    ଶୂନ୍ୟ _init(int32_t initialCapacity, UErrorCode &status);

    // Disallow
    UVector32(ଅଚଳ UVector32&) = delete;

    // Disallow
    UVector32& operator=(ଅଚଳ UVector32&) = delete;


    //  API Functions for Stack operations.
    //  In the original UVector, these were in a separate derived class, UStack.
    //  Here in UVector32, they are all together.
ସର୍ବସାଧାରଣ:
    ଧାଡ଼ିଭିତରେ UBool empty() ଅଚଳ;   // TODO:  redundant, same as empty().  Remove it?

    ଧାଡ଼ିଭିତରେ int32_t peeki() ଅଚଳ;
    
    ଧାଡ଼ିଭିତରେ int32_t popi();
    
    ଧାଡ଼ିଭିତରେ int32_t ଠେଲ(int32_t i, UErrorCode &status);

    ଧାଡ଼ିଭିତରେ int32_t *reserveBlock(int32_t size, UErrorCode &status);
    ଧାଡ଼ିଭିତରେ int32_t *କାଢ଼_Frame(int32_t size);
};


// UVector32 inlines

ଧାଡ଼ିଭିତରେ UBool UVector32::ensureCapacity(int32_t minimumCapacity, UErrorCode &status) {
    ଯଦି ((minimumCapacity >= 0) && (capacity >= minimumCapacity)) {
        ଫେରନ୍ତୁ true;
    } ଅନ୍ୟଥା {
        ଫେରନ୍ତୁ expandCapacity(minimumCapacity, status);
    }
}

ଧାଡ଼ିଭିତରେ int32_t UVector32::elementAti(int32_t index) ଅଚଳ {
    ଫେରନ୍ତୁ (index >= 0 && ଗଣନା_2 > 0 && ଗଣନା_2 - index > 0) ? elements[index] : 0;
}


ଧାଡ଼ିଭିତରେ ଶୂନ୍ୟ UVector32::addElement(int32_t elem, UErrorCode &status) {
    ଯଦି (ensureCapacity(ଗଣନା_2 + 1, status)) {
        elements[ଗଣନା_2] = elem;
        ଗଣନା_2++;
    }
}

ଧାଡ଼ିଭିତରେ int32_t *UVector32::reserveBlock(int32_t size, UErrorCode &status) {
    ଯଦି (ensureCapacity(ଗଣନା_2+size, status) == false) {
        ଫେରନ୍ତୁ nullptr;
    }
    int32_t  *rp = elements+ଗଣନା_2;
    ଗଣନା_2 += size;
    ଫେରନ୍ତୁ rp;
}

ଧାଡ଼ିଭିତରେ int32_t *UVector32::କାଢ଼_Frame(int32_t size) {
    U_ASSERT(ଗଣନା_2 >= size);
    ଗଣନା_2 -= size;
    ଯଦି (ଗଣନା_2 < 0) {
        ଗଣନା_2 = 0;
    }
    ଫେରନ୍ତୁ elements+ଗଣନା_2-size;
}



ଧାଡ଼ିଭିତରେ int32_t UVector32::size() ଅଚଳ {
    ଫେରନ୍ତୁ ଗଣନା_2;
}

ଧାଡ଼ିଭିତରେ UBool UVector32::isEmpty() ଅଚଳ {
    ଫେରନ୍ତୁ ଗଣନା_2 == 0;
}

ଧାଡ଼ିଭିତରେ UBool UVector32::contains(int32_t obj) ଅଚଳ {
    ଫେରନ୍ତୁ indexOf(obj) >= 0;
}

ଧାଡ଼ିଭିତରେ int32_t UVector32::lastElementi() ଅଚଳ {
    ଫେରନ୍ତୁ elementAti(ଗଣନା_2-1);
}

ଧାଡ଼ିଭିତରେ bool UVector32::operator!=(ଅଚଳ UVector32& other) ଅଚଳ {
    ଫେରନ୍ତୁ !operator==(other);
}

ଧାଡ଼ିଭିତରେ int32_t *UVector32::getBuffer() ଅଚଳ {
    ଫେରନ୍ତୁ elements;
}


// UStack inlines

ଧାଡ଼ିଭିତରେ UBool UVector32::empty() ଅଚଳ {
    ଫେରନ୍ତୁ isEmpty();
}

ଧାଡ଼ିଭିତରେ int32_t UVector32::peeki() ଅଚଳ {
    ଫେରନ୍ତୁ lastElementi();
}

ଧାଡ଼ିଭିତରେ int32_t UVector32::ଠେଲ(int32_t i, UErrorCode &status) {
    addElement(i, status);
    ଫେରନ୍ତୁ i;
}

ଧାଡ଼ିଭିତରେ int32_t UVector32::popi() {
    int32_t result = 0;
    ଯଦି (ଗଣନା_2 > 0) {
        ଗଣନା_2--;
        result = elements[ଗଣନା_2];
    }
    ଫେରନ୍ତୁ result;
}

U_NAMESPACE_END

#endif
