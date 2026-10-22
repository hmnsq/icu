// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
**********************************************************************
*   Copyright (C) 1999-2016, International Business Machines
*   Corporation and others.  All Rights Reserved.
**********************************************************************
*   Date        Name        Description
*   10/22/99    alan        Creation.  This is an internal header.
*                           It should not be exported.
**********************************************************************
*/

#ifndef UVECTOR_H
#define UVECTOR_H

#অন্তর্ভাৱ "unicode/utypes.h"
#অন্তর্ভাৱ "unicode/uobject.h"
#অন্তর্ভাৱ "cmemory.h"
#অন্তর্ভাৱ "uarrsort.h"
#অন্তর্ভাৱ "uelement.h"

U_NAMESPACE_BEGIN

/**
 * Ultralightweight C++ implementation of a `void*` vector
 * that is (mostly) compatible with java.util.Vector.
 *
 * This is a very simple implementation, written to satisfy an
 * immediate porting need.  As such, it is not completely fleshed out,
 * and it aims for simplicity and conformity.  Nonetheless, it serves
 * its purpose (porting code from java that uses java.util.Vector)
 * well, and it could be easily made into a more robust vector class.
 *
 * *Design notes*
 *
 * There is index bounds checking, but little is done about it.  If
 * indices are out of bounds, either nothing happens, or zero is
 * returned.  We *do* avoid indexing off into the weeds.
 *
 * Since we don't have garbage collection, UVector was given the
 * option to *own* its contents.  To employ this, set a deleter
 * function.  The deleter is called on a `void *` pointer when that
 * pointer is released by the vector, either when the vector itself is
 * destructed, or when a call to `setElementAt()` overwrites an element,
 * or when a call to remove()` or one of its variants explicitly
 * removes an element.  If no deleter is set, or the deleter is set to
 * zero, then it is assumed that the caller will delete elements as
 * needed.
 *
 * *Error Handling* Functions that can fail, from out of memory conditions
 * for example, include a UErrorCode parameter. Any function called
 * with an error code already indicating a failure will not modify the
 * vector in any way.
 *
 * For vectors that have a deleter function, any failure in inserting
 * an element into the vector will instead delete the element that
 * could not be adopted. This simplifies object ownership
 * management around calls to `addElement()` and `insertElementAt()`;
 * error or no, the function always takes ownership of an incoming object
 * from the caller.
 *
 * In order to implement methods such as `contains()` and `indexOf()`,
 * UVector needs a way to compare objects for equality.  To do so, it
 * uses a comparison function, or "comparer."  If the comparer is not
 * set, or is set to zero, then all such methods will act as if the
 * vector contains no element.  That is, indexOf() will always return
 * -1, contains() will always return false, etc.
 *
 * <p><b>To do</b>
 *
 * <p>Improve the handling of index out of bounds errors.
 *
 * @author Alan Liu
 */
class U_COMMON_API UVector : সার্ৱজনিক UObject {
    // NOTE: UVector uses the UElement (union of void* and int32_t) as
    // its basic storage type.  It uses UElementsAreEqual as its
    // comparison function.  It uses UObjectDeleter as its deleter
    // function.  This allows sharing of support functions with UHashtable.

গুপ্ত:
    int32_t গুনতি = 0;

    int32_t capacity = 0;

    UElement* elements = nullptr;

    UObjectDeleter *deleter = nullptr;

    UElementsAreEqual *comparer = nullptr;

সার্ৱজনিক:
    UVector(UErrorCode &status);

    UVector(int32_t initialCapacity, UErrorCode &status);

    UVector(UObjectDeleter *d, UElementsAreEqual *c, UErrorCode &status);

    UVector(UObjectDeleter *d, UElementsAreEqual *c, int32_t initialCapacity, UErrorCode &status);

    আভাস ~UVector();

    /**
     * Assign this object to another (make this a copy of 'other').
     * Use the 'assign' function to assign each element.
     */
    শূন্য assign(ধ্রুৱ UVector& other, UElementAssigner *assign, UErrorCode &ec);

    /**
     * Compare this vector with another.  They will be considered
     * equal if they are of the same size and all elements are equal,
     * as compared using this object's comparer.
     */
    bool operator==(ধ্রুৱ UVector& other) ধ্রুৱ;

    /**
     * Equivalent to !operator==()
     */
    অন্তঃস্থ bool operator!=(ধ্রুৱ UVector& other) ধ্রুৱ {ঘূৰাই !operator==(other);}

    //------------------------------------------------------------
    // java.util.Vector API
    //------------------------------------------------------------

    /**
     * Add an element at the end of the vector.
     * For use only with vectors that do not adopt their elements, which is to say,
     * have not set an element deleter function. See `adoptElement()`.
     */
    শূন্য addElement(শূন্য *obj, UErrorCode &status);

    /**
     * Add an element at the end of the vector.
     * For use only with vectors that adopt their elements, which is to say,
     * have set an element deleter function. See `addElement()`.
     *
     * If the element cannot be successfully added, it will be deleted. This is
     * normal ICU _adopt_ behavior - one way or another ownership of the incoming
     * object is transferred from the caller.
     *
     * `addElement()` and `adoptElement()` are separate functions to make it easier
     * to see what the function is doing at call sites. Having a single combined function,
     * as in earlier versions of UVector, had proved to be error-prone.
     */
    শূন্য adoptElement(শূন্য *obj, UErrorCode &status);

    শূন্য addElement(int32_t elem, UErrorCode &status);

    শূন্য setElementAt(শূন্য* obj, int32_t index);

    শূন্য setElementAt(int32_t elem, int32_t index);

    শূন্য ঢোকাও_Element_At(শূন্য* obj, int32_t index, UErrorCode &status);

    শূন্য ঢোকাও_Element_At(int32_t elem, int32_t index, UErrorCode &status);
    
    শূন্য* elementAt(int32_t index) ধ্রুৱ;

    int32_t elementAti(int32_t index) ধ্রুৱ;

    UBool equals(ধ্রুৱ UVector &other) ধ্রুৱ;

    অন্তঃস্থ শূন্য* firstElement() ধ্রুৱ {ঘূৰাই elementAt(0);}

    অন্তঃস্থ শূন্য* lastElement() ধ্রুৱ {ঘূৰাই elementAt(গুনতি-1);}

    অন্তঃস্থ int32_t lastElementi() ধ্রুৱ {ঘূৰাই elementAti(গুনতি-1);}

    int32_t indexOf(শূন্য* obj, int32_t startIndex = 0) ধ্রুৱ;

    int32_t indexOf(int32_t obj, int32_t startIndex = 0) ধ্রুৱ;

    অন্তঃস্থ UBool contains(শূন্য* obj) ধ্রুৱ {ঘূৰাই indexOf(obj) >= 0;}

    অন্তঃস্থ UBool contains(int32_t obj) ধ্রুৱ {ঘূৰাই indexOf(obj) >= 0;}

    UBool containsAll(ধ্রুৱ UVector& other) ধ্রুৱ;

    UBool removeAll(ধ্রুৱ UVector& other);

    UBool retainAll(ধ্রুৱ UVector& other);

    শূন্য removeElementAt(int32_t index);

    UBool removeElement(শূন্য* obj);

    শূন্য removeAllElements();

    অন্তঃস্থ int32_t size() ধ্রুৱ {ঘূৰাই গুনতি;}

    অন্তঃস্থ UBool isEmpty() ধ্রুৱ {ঘূৰাই গুনতি == 0;}

    UBool ensureCapacity(int32_t minimumCapacity, UErrorCode &status);

    /**
     * Change the size of this vector as follows: If newSize is
     * smaller, then truncate the array, possibly deleting held
     * elements for i >= newSize.  If newSize is larger, grow the
     * array, filling in new slots with nullptr.
     */
    শূন্য setSize(int32_t newSize, UErrorCode &status);

    /**
     * Fill in the given array with all elements of this vector.
     */
    শূন্য** toArray(শূন্য** result) ধ্রুৱ;

    //------------------------------------------------------------
    // New API
    //------------------------------------------------------------

    UObjectDeleter *setDeleter(UObjectDeleter *d);
    bool hasDeleter() {ঘূৰাই deleter != nullptr;}

    UElementsAreEqual *setComparer(UElementsAreEqual *c);

    অন্তঃস্থ শূন্য* operator[](int32_t index) ধ্রুৱ {ঘূৰাই elementAt(index);}

    /**
     * Removes the element at the given index from this vector and
     * transfer ownership of it to the caller.  After this call, the
     * caller owns the result and must delete it and the vector entry
     * at 'index' is removed, shifting all subsequent entries back by
     * one index and shortening the size of the vector by one.  If the
     * index is out of range or if there is no item at the given index
     * then 0 is returned and the vector is unchanged.
     */
    শূন্য* orphanElementAt(int32_t index);

    /**
     * Returns true if this vector contains none of the elements
     * of the given vector.
     * @param other vector to be checked for containment
     * @return true if the test condition is met
     */
    UBool containsNone(ধ্রুৱ UVector& other) ধ্রুৱ;

    /**
     * Insert the given object into this vector at its sorted position
     * as defined by 'compare'.  The current elements are assumed to
     * be sorted already.
     */
    শূন্য sorted_ঢোকাও(শূন্য* obj, UElementComparator *compare, UErrorCode& ec);

    /**
     * Insert the given integer into this vector at its sorted position
     * as defined by 'compare'.  The current elements are assumed to
     * be sorted already.
     */
    শূন্য sorted_ঢোকাও(int32_t obj, UElementComparator *compare, UErrorCode& ec);

    /**
     * Sort the contents of the vector, assuming that the contents of the
     * vector are of type int32_t.
     */
    শূন্য sorti(UErrorCode &ec);

    /**
      * Sort the contents of this vector, using a caller-supplied function
      * to do the comparisons.  (It's confusing that
      *  UVector's UElementComparator function is different from the
      *  UComparator function type defined in uarrsort.h)
      */
    শূন্য sort(UElementComparator *compare, UErrorCode &ec);

    /**
     * Stable sort the contents of this vector using a caller-supplied function
     * of type UComparator to do the comparison.  Provides more flexibility
     * than UVector::sort() because an additional user parameter can be passed to
     * the comparison function.
     */
    শূন্য sortWithUComparator(UComparator *compare, ধ্রুৱ শূন্য *context, UErrorCode &ec);

    /**
     * ICU "poor man's RTTI", returns a UClassID for this class.
     */
    স্থির UClassID U_EXPORT2 getStaticClassID();

    /**
     * ICU "poor man's RTTI", returns a UClassID for the actual class.
     */
    আভাস UClassID getDynamicClassID() ধ্রুৱ override;

গুপ্ত:
    int32_t indexOf(UElement চাবি, int32_t startIndex = 0, int8_t hint = 0) ধ্রুৱ;

    শূন্য sorted_ঢোকাও(UElement e, UElementComparator *compare, UErrorCode& ec);

সার্ৱজনিক:
    // Disallow
    UVector(ধ্রুৱ UVector&) = delete;

    // Disallow
    UVector& operator=(ধ্রুৱ UVector&) = delete;

};


/**
 * Ultralightweight C++ implementation of a `void*` stack
 * that is (mostly) compatible with java.util.Stack.  As in java, this
 * is merely a paper thin layer around UVector.  See the UVector
 * documentation for further information.
 *
 * *Design notes*
 *
 * The element at index `n-1` is (of course) the top of the
 * stack.
 *
 * The poorly named `empty()` method doesn't empty the
 * stack; it determines if the stack is empty.
 *
 * @author Alan Liu
 */
class U_COMMON_API UStack : সার্ৱজনিক UVector {
সার্ৱজনিক:
    UStack(UErrorCode &status);

    UStack(int32_t initialCapacity, UErrorCode &status);

    UStack(UObjectDeleter *d, UElementsAreEqual *c, UErrorCode &status);

    UStack(UObjectDeleter *d, UElementsAreEqual *c, int32_t initialCapacity, UErrorCode &status);

    আভাস ~UStack();

    // It's okay not to have a virtual destructor (in UVector)
    // because UStack has no special cleanup to do.

    অন্তঃস্থ UBool empty() ধ্রুৱ {ঘূৰাই isEmpty();}

    অন্তঃস্থ শূন্য* peek() ধ্রুৱ {ঘূৰাই lastElement();}

    অন্তঃস্থ int32_t peeki() ধ্রুৱ {ঘূৰাই lastElementi();}
    
    /**
     * Pop and return an element from the stack.
     * For stacks with a deleter function, the caller takes ownership
     * of the popped element.
     */
    শূন্য* বেৰ_কৰো();
    
    int32_t popi();
    
    অন্তঃস্থ শূন্য* ঠেলো(শূন্য* obj, UErrorCode &status) {
        যদি (hasDeleter()) {
            adoptElement(obj, status);
            ঘূৰাই (U_SUCCESS(status)) ? obj : nullptr;
        } অন্যথা {
            addElement(obj, status);
            ঘূৰাই obj;
        }
    }

    অন্তঃস্থ int32_t ঠেলো(int32_t i, UErrorCode &status) {
        addElement(i, status);
        ঘূৰাই i;
    }

    /*
    If the object o occurs as an item in this stack,
    this method returns the 1-based distance from the top of the stack.
    */
    int32_t অনুসন্ধান(শূন্য* obj) ধ্রুৱ;

    /**
     * ICU "poor man's RTTI", returns a UClassID for this class.
     */
    স্থির UClassID U_EXPORT2 getStaticClassID();

    /**
     * ICU "poor man's RTTI", returns a UClassID for the actual class.
     */
    আভাস UClassID getDynamicClassID() ধ্রুৱ override;

    // Disallow
    UStack(ধ্রুৱ UStack&) = delete;

    // Disallow
    UStack& operator=(ধ্রুৱ UStack&) = delete;
};

U_NAMESPACE_END

#endif
