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

#شامل "unicode/utypes.h"
#شامل "unicode/uobject.h"
#شامل "cmemory.h"
#شامل "uarrsort.h"
#شامل "uelement.h"

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
class U_COMMON_API UVector : عمومی UObject {
    // NOTE: UVector uses the UElement (union of void* and int32_t) as
    // its basic storage type.  It uses UElementsAreEqual as its
    // comparison function.  It uses UObjectDeleter as its deleter
    // function.  This allows sharing of support functions with UHashtable.

خصوصی:
    int32_t شمار = 0;

    int32_t capacity = 0;

    UElement* elements = nullptr;

    UObjectDeleter *deleter = nullptr;

    UElementsAreEqual *comparer = nullptr;

عمومی:
    UVector(UErrorCode &status);

    UVector(int32_t initialCapacity, UErrorCode &status);

    UVector(UObjectDeleter *d, UElementsAreEqual *c, UErrorCode &status);

    UVector(UObjectDeleter *d, UElementsAreEqual *c, int32_t initialCapacity, UErrorCode &status);

    مجازی ~UVector();

    /**
     * Assign this object to another (make this a copy of 'other').
     * Use the 'assign' function to assign each element.
     */
    تهی assign(ثابت UVector& other, UElementAssigner *assign, UErrorCode &ec);

    /**
     * Compare this vector with another.  They will be considered
     * equal if they are of the same size and all elements are equal,
     * as compared using this object's comparer.
     */
    bool operator==(ثابت UVector& other) ثابت;

    /**
     * Equivalent to !operator==()
     */
    درونخطی bool operator!=(ثابت UVector& other) ثابت {بازگشت !operator==(other);}

    //------------------------------------------------------------
    // java.util.Vector API
    //------------------------------------------------------------

    /**
     * Add an element at the end of the vector.
     * For use only with vectors that do not adopt their elements, which is to say,
     * have not set an element deleter function. See `adoptElement()`.
     */
    تهی addElement(تهی *obj, UErrorCode &status);

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
    تهی adoptElement(تهی *obj, UErrorCode &status);

    تهی addElement(int32_t elem, UErrorCode &status);

    تهی setElementAt(تهی* obj, int32_t index);

    تهی setElementAt(int32_t elem, int32_t index);

    تهی درج_Element_At(تهی* obj, int32_t index, UErrorCode &status);

    تهی درج_Element_At(int32_t elem, int32_t index, UErrorCode &status);
    
    تهی* elementAt(int32_t index) ثابت;

    int32_t elementAti(int32_t index) ثابت;

    UBool equals(ثابت UVector &other) ثابت;

    درونخطی تهی* firstElement() ثابت {بازگشت elementAt(0);}

    درونخطی تهی* lastElement() ثابت {بازگشت elementAt(شمار-1);}

    درونخطی int32_t lastElementi() ثابت {بازگشت elementAti(شمار-1);}

    int32_t indexOf(تهی* obj, int32_t startIndex = 0) ثابت;

    int32_t indexOf(int32_t obj, int32_t startIndex = 0) ثابت;

    درونخطی UBool contains(تهی* obj) ثابت {بازگشت indexOf(obj) >= 0;}

    درونخطی UBool contains(int32_t obj) ثابت {بازگشت indexOf(obj) >= 0;}

    UBool containsAll(ثابت UVector& other) ثابت;

    UBool removeAll(ثابت UVector& other);

    UBool retainAll(ثابت UVector& other);

    تهی removeElementAt(int32_t index);

    UBool removeElement(تهی* obj);

    تهی removeAllElements();

    درونخطی int32_t size() ثابت {بازگشت شمار;}

    درونخطی UBool isEmpty() ثابت {بازگشت شمار == 0;}

    UBool ensureCapacity(int32_t minimumCapacity, UErrorCode &status);

    /**
     * Change the size of this vector as follows: If newSize is
     * smaller, then truncate the array, possibly deleting held
     * elements for i >= newSize.  If newSize is larger, grow the
     * array, filling in new slots with nullptr.
     */
    تهی setSize(int32_t newSize, UErrorCode &status);

    /**
     * Fill in the given array with all elements of this vector.
     */
    تهی** toArray(تهی** result) ثابت;

    //------------------------------------------------------------
    // New API
    //------------------------------------------------------------

    UObjectDeleter *setDeleter(UObjectDeleter *d);
    bool hasDeleter() {بازگشت deleter != nullptr;}

    UElementsAreEqual *setComparer(UElementsAreEqual *c);

    درونخطی تهی* operator[](int32_t index) ثابت {بازگشت elementAt(index);}

    /**
     * Removes the element at the given index from this vector and
     * transfer ownership of it to the caller.  After this call, the
     * caller owns the result and must delete it and the vector entry
     * at 'index' is removed, shifting all subsequent entries back by
     * one index and shortening the size of the vector by one.  If the
     * index is out of range or if there is no item at the given index
     * then 0 is returned and the vector is unchanged.
     */
    تهی* orphanElementAt(int32_t index);

    /**
     * Returns true if this vector contains none of the elements
     * of the given vector.
     * @param other vector to be checked for containment
     * @return true if the test condition is met
     */
    UBool containsNone(ثابت UVector& other) ثابت;

    /**
     * Insert the given object into this vector at its sorted position
     * as defined by 'compare'.  The current elements are assumed to
     * be sorted already.
     */
    تهی sorted_درج(تهی* obj, UElementComparator *compare, UErrorCode& ec);

    /**
     * Insert the given integer into this vector at its sorted position
     * as defined by 'compare'.  The current elements are assumed to
     * be sorted already.
     */
    تهی sorted_درج(int32_t obj, UElementComparator *compare, UErrorCode& ec);

    /**
     * Sort the contents of the vector, assuming that the contents of the
     * vector are of type int32_t.
     */
    تهی sorti(UErrorCode &ec);

    /**
      * Sort the contents of this vector, using a caller-supplied function
      * to do the comparisons.  (It's confusing that
      *  UVector's UElementComparator function is different from the
      *  UComparator function type defined in uarrsort.h)
      */
    تهی sort(UElementComparator *compare, UErrorCode &ec);

    /**
     * Stable sort the contents of this vector using a caller-supplied function
     * of type UComparator to do the comparison.  Provides more flexibility
     * than UVector::sort() because an additional user parameter can be passed to
     * the comparison function.
     */
    تهی sortWithUComparator(UComparator *compare, ثابت تهی *context, UErrorCode &ec);

    /**
     * ICU "poor man's RTTI", returns a UClassID for this class.
     */
    ایستا UClassID U_EXPORT2 getStaticClassID();

    /**
     * ICU "poor man's RTTI", returns a UClassID for the actual class.
     */
    مجازی UClassID getDynamicClassID() ثابت override;

خصوصی:
    int32_t indexOf(UElement کلید, int32_t startIndex = 0, int8_t hint = 0) ثابت;

    تهی sorted_درج(UElement e, UElementComparator *compare, UErrorCode& ec);

عمومی:
    // Disallow
    UVector(ثابت UVector&) = delete;

    // Disallow
    UVector& operator=(ثابت UVector&) = delete;

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
class U_COMMON_API UStack : عمومی UVector {
عمومی:
    UStack(UErrorCode &status);

    UStack(int32_t initialCapacity, UErrorCode &status);

    UStack(UObjectDeleter *d, UElementsAreEqual *c, UErrorCode &status);

    UStack(UObjectDeleter *d, UElementsAreEqual *c, int32_t initialCapacity, UErrorCode &status);

    مجازی ~UStack();

    // It's okay not to have a virtual destructor (in UVector)
    // because UStack has no special cleanup to do.

    درونخطی UBool empty() ثابت {بازگشت isEmpty();}

    درونخطی تهی* peek() ثابت {بازگشت lastElement();}

    درونخطی int32_t peeki() ثابت {بازگشت lastElementi();}
    
    /**
     * Pop and return an element from the stack.
     * For stacks with a deleter function, the caller takes ownership
     * of the popped element.
     */
    تهی* برداشت();
    
    int32_t popi();
    
    درونخطی تهی* افزودن(تهی* obj, UErrorCode &status) {
        اگر (hasDeleter()) {
            adoptElement(obj, status);
            بازگشت (U_SUCCESS(status)) ? obj : nullptr;
        } وگرنه {
            addElement(obj, status);
            بازگشت obj;
        }
    }

    درونخطی int32_t افزودن(int32_t i, UErrorCode &status) {
        addElement(i, status);
        بازگشت i;
    }

    /*
    If the object o occurs as an item in this stack,
    this method returns the 1-based distance from the top of the stack.
    */
    int32_t جستجو(تهی* obj) ثابت;

    /**
     * ICU "poor man's RTTI", returns a UClassID for this class.
     */
    ایستا UClassID U_EXPORT2 getStaticClassID();

    /**
     * ICU "poor man's RTTI", returns a UClassID for the actual class.
     */
    مجازی UClassID getDynamicClassID() ثابت override;

    // Disallow
    UStack(ثابت UStack&) = delete;

    // Disallow
    UStack& operator=(ثابت UStack&) = delete;
};

U_NAMESPACE_END

#endif
