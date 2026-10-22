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

#অন্তর্ভাব "unicode/utypes.h"
#অন্তর্ভাব "unicode/uobject.h"
#অন্তর্ভাব "uhash.h"
#অন্তর্ভাব "uassert.h"

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
class U_COMMON_API UVector32 : সর্বজনীন UObject {
ব্যক্তিগত:
    int32_t   গুনতি;

    int32_t   capacity;
    
    int32_t   maxCapacity;   // Limit beyond which capacity is not permitted to grow.

    int32_t*  elements;

সর্বজনীন:
    UVector32(UErrorCode &status);

    UVector32(int32_t initialCapacity, UErrorCode &status);

    আভাসী ~UVector32();

    /**
     * Assign this object to another (make this a copy of 'other').
     * Use the 'assign' function to assign each element.
     */
    শূন্য assign(ধ্রুবক UVector32& other, UErrorCode &ec);

    /**
     * Compare this vector with another.  They will be considered
     * equal if they are of the same size and all elements are equal,
     * as compared using this object's comparer.
     */
    bool operator==(ধ্রুবক UVector32& other) ধ্রুবক;

    /**
     * Equivalent to !operator==()
     */
    অন্তঃরেখ bool operator!=(ধ্রুবক UVector32& other) ধ্রুবক;

    //------------------------------------------------------------
    // java.util.Vector API
    //------------------------------------------------------------

    অন্তঃরেখ শূন্য addElement(int32_t elem, UErrorCode &status);

    শূন্য setElementAt(int32_t elem, int32_t সূচক);

    শূন্য ঢোকাও_Element_At(int32_t elem, int32_t সূচক, UErrorCode &status);
    
    অন্তঃরেখ int32_t elementAti(int32_t সূচক) ধ্রুবক;

    UBool equals(ধ্রুবক UVector32 &other) ধ্রুবক;

    অন্তঃরেখ int32_t lastElementi() ধ্রুবক;

    int32_t সূচক_Of(int32_t elem, int32_t start_সূচক = 0) ধ্রুবক;

    অন্তঃরেখ UBool contains(int32_t elem) ধ্রুবক;

    UBool containsAll(ধ্রুবক UVector32& other) ধ্রুবক;

    UBool removeAll(ধ্রুবক UVector32& other);

    UBool retainAll(ধ্রুবক UVector32& other);

    শূন্য removeElementAt(int32_t সূচক);

    শূন্য removeAllElements();

    অন্তঃরেখ int32_t আকার_3() ধ্রুবক;

    অন্তঃরেখ UBool isEmpty() ধ্রুবক;

    // Inline.  Use this one for speedy size check.
    অন্তঃরেখ UBool ensureCapacity(int32_t minimumCapacity, UErrorCode &status);

    // Out-of-line, handles actual growth.  Called by ensureCapacity() when necessary.
    UBool expandCapacity(int32_t minimumCapacity, UErrorCode &status);

    /**
     * Change the size of this vector as follows: If newSize is
     * smaller, then truncate the array, possibly deleting held
     * elements for i >= newSize.  If newSize is larger, grow the
     * array, filling in new slows with zero.
     */
    শূন্য set_আকার(int32_t new_আকার);

    //------------------------------------------------------------
    // New API
    //------------------------------------------------------------

    /**
     * Returns true if this vector contains none of the elements
     * of the given vector.
     * @param other vector to be checked for containment
     * @return true if the test condition is met
     */
    UBool containsNone(ধ্রুবক UVector32& other) ধ্রুবক;


    /**
     * Insert the given integer into this vector at its sorted position.
     * The current elements are assumed to be sorted already.
     */
    শূন্য sorted_ঢোকাও(int32_t elem, UErrorCode& ec);

    /**
     * Returns a pointer to the internal array holding the vector.
     */
    অন্তঃরেখ int32_t *get_বাফার() ধ্রুবক;

    /**
     * Set the maximum allowed buffer capacity for this vector/stack.
     * Default with no limit set is unlimited, go until malloc() fails.
     * A Limit of zero means unlimited capacity.
     * Units are vector elements (32 bits each), not bytes.
     */
    শূন্য setMaxCapacity(int32_t limit);

    /**
     * ICU "poor man's RTTI", returns a UClassID for this class.
     */
    স্থির UClassID U_EXPORT2 getStaticClassID();

    /**
     * ICU "poor man's RTTI", returns a UClassID for the actual class.
     */
    আভাসী UClassID getDynamicClassID() ধ্রুবক override;

ব্যক্তিগত:
    শূন্য _init(int32_t initialCapacity, UErrorCode &status);

    // Disallow
    UVector32(ধ্রুবক UVector32&) = delete;

    // Disallow
    UVector32& operator=(ধ্রুবক UVector32&) = delete;


    //  API Functions for Stack operations.
    //  In the original UVector, these were in a separate derived class, UStack.
    //  Here in UVector32, they are all together.
সর্বজনীন:
    অন্তঃরেখ UBool empty() ধ্রুবক;   // TODO:  redundant, same as empty().  Remove it?

    অন্তঃরেখ int32_t peeki() ধ্রুবক;
    
    অন্তঃরেখ int32_t popi();
    
    অন্তঃরেখ int32_t ঠেলো(int32_t i, UErrorCode &status);

    অন্তঃরেখ int32_t *reserveBlock(int32_t আকার_3, UErrorCode &status);
    অন্তঃরেখ int32_t *বের_করো_Frame(int32_t আকার_3);
};


// UVector32 inlines

অন্তঃরেখ UBool UVector32::ensureCapacity(int32_t minimumCapacity, UErrorCode &status) {
    যদি ((minimumCapacity >= 0) && (capacity >= minimumCapacity)) {
        ফেরত true;
    } অন্যথা {
        ফেরত expandCapacity(minimumCapacity, status);
    }
}

অন্তঃরেখ int32_t UVector32::elementAti(int32_t সূচক) ধ্রুবক {
    ফেরত (সূচক >= 0 && গুনতি > 0 && গুনতি - সূচক > 0) ? elements[সূচক] : 0;
}


অন্তঃরেখ শূন্য UVector32::addElement(int32_t elem, UErrorCode &status) {
    যদি (ensureCapacity(গুনতি + 1, status)) {
        elements[গুনতি] = elem;
        গুনতি++;
    }
}

অন্তঃরেখ int32_t *UVector32::reserveBlock(int32_t আকার_3, UErrorCode &status) {
    যদি (ensureCapacity(গুনতি+আকার_3, status) == false) {
        ফেরত nullptr;
    }
    int32_t  *rp = elements+গুনতি;
    গুনতি += আকার_3;
    ফেরত rp;
}

অন্তঃরেখ int32_t *UVector32::বের_করো_Frame(int32_t আকার_3) {
    U_ASSERT(গুনতি >= আকার_3);
    গুনতি -= আকার_3;
    যদি (গুনতি < 0) {
        গুনতি = 0;
    }
    ফেরত elements+গুনতি-আকার_3;
}



অন্তঃরেখ int32_t UVector32::আকার_3() ধ্রুবক {
    ফেরত গুনতি;
}

অন্তঃরেখ UBool UVector32::isEmpty() ধ্রুবক {
    ফেরত গুনতি == 0;
}

অন্তঃরেখ UBool UVector32::contains(int32_t obj) ধ্রুবক {
    ফেরত সূচক_Of(obj) >= 0;
}

অন্তঃরেখ int32_t UVector32::lastElementi() ধ্রুবক {
    ফেরত elementAti(গুনতি-1);
}

অন্তঃরেখ bool UVector32::operator!=(ধ্রুবক UVector32& other) ধ্রুবক {
    ফেরত !operator==(other);
}

অন্তঃরেখ int32_t *UVector32::get_বাফার() ধ্রুবক {
    ফেরত elements;
}


// UStack inlines

অন্তঃরেখ UBool UVector32::empty() ধ্রুবক {
    ফেরত isEmpty();
}

অন্তঃরেখ int32_t UVector32::peeki() ধ্রুবক {
    ফেরত lastElementi();
}

অন্তঃরেখ int32_t UVector32::ঠেলো(int32_t i, UErrorCode &status) {
    addElement(i, status);
    ফেরত i;
}

অন্তঃরেখ int32_t UVector32::popi() {
    int32_t ফলাফল = 0;
    যদি (গুনতি > 0) {
        গুনতি--;
        ফলাফল = elements[গুনতি];
    }
    ফেরত ফলাফল;
}

U_NAMESPACE_END

#endif
