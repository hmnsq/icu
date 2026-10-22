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

#अन्तर्भाव "unicode/utypes.h"
#अन्तर्भाव "unicode/uobject.h"
#अन्तर्भाव "cmemory.h"
#अन्तर्भाव "uarrsort.h"
#अन्तर्भाव "uelement.h"

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
class U_COMMON_API UVector : सार्वजनिक UObject {
    // NOTE: UVector uses the UElement (union of void* and int32_t) as
    // its basic storage type.  It uses UElementsAreEqual as its
    // comparison function.  It uses UObjectDeleter as its deleter
    // function.  This allows sharing of support functions with UHashtable.

खाजगी:
    int32_t मोजणी = 0;

    int32_t capacity = 0;

    UElement* elements = nullptr;

    UObjectDeleter *deleter = nullptr;

    UElementsAreEqual *comparer = nullptr;

सार्वजनिक:
    UVector(UErrorCode &status);

    UVector(int32_t initialCapacity, UErrorCode &status);

    UVector(UObjectDeleter *d, UElementsAreEqual *c, UErrorCode &status);

    UVector(UObjectDeleter *d, UElementsAreEqual *c, int32_t initialCapacity, UErrorCode &status);

    आभासी ~UVector();

    /**
     * Assign this object to another (make this a copy of 'other').
     * Use the 'assign' function to assign each element.
     */
    शून्य assign(स्थिरांक UVector& other, UElementAssigner *assign, UErrorCode &ec);

    /**
     * Compare this vector with another.  They will be considered
     * equal if they are of the same size and all elements are equal,
     * as compared using this object's comparer.
     */
    bool चालक==(स्थिरांक UVector& other) स्थिरांक;

    /**
     * Equivalent to !operator==()
     */
    ओळीत bool चालक!=(स्थिरांक UVector& other) स्थिरांक {परत !चालक==(other);}

    //------------------------------------------------------------
    // java.util.Vector API
    //------------------------------------------------------------

    /**
     * Add an element at the end of the vector.
     * For use only with vectors that do not adopt their elements, which is to say,
     * have not set an element deleter function. See `adoptElement()`.
     */
    शून्य addElement(शून्य *obj, UErrorCode &status);

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
    शून्य adoptElement(शून्य *obj, UErrorCode &status);

    शून्य addElement(int32_t elem, UErrorCode &status);

    शून्य setElementAt(शून्य* obj, int32_t index);

    शून्य setElementAt(int32_t elem, int32_t index);

    शून्य घाला_Element_At(शून्य* obj, int32_t index, UErrorCode &status);

    शून्य घाला_Element_At(int32_t elem, int32_t index, UErrorCode &status);
    
    शून्य* elementAt(int32_t index) स्थिरांक;

    int32_t elementAti(int32_t index) स्थिरांक;

    UBool equals(स्थिरांक UVector &other) स्थिरांक;

    ओळीत शून्य* firstElement() स्थिरांक {परत elementAt(0);}

    ओळीत शून्य* lastElement() स्थिरांक {परत elementAt(मोजणी-1);}

    ओळीत int32_t lastElementi() स्थिरांक {परत elementAti(मोजणी-1);}

    int32_t indexOf(शून्य* obj, int32_t startIndex = 0) स्थिरांक;

    int32_t indexOf(int32_t obj, int32_t startIndex = 0) स्थिरांक;

    ओळीत UBool contains(शून्य* obj) स्थिरांक {परत indexOf(obj) >= 0;}

    ओळीत UBool contains(int32_t obj) स्थिरांक {परत indexOf(obj) >= 0;}

    UBool containsAll(स्थिरांक UVector& other) स्थिरांक;

    UBool removeAll(स्थिरांक UVector& other);

    UBool retainAll(स्थिरांक UVector& other);

    शून्य removeElementAt(int32_t index);

    UBool removeElement(शून्य* obj);

    शून्य removeAllElements();

    ओळीत int32_t size() स्थिरांक {परत मोजणी;}

    ओळीत UBool isEmpty() स्थिरांक {परत मोजणी == 0;}

    UBool ensureCapacity(int32_t minimumCapacity, UErrorCode &status);

    /**
     * Change the size of this vector as follows: If newSize is
     * smaller, then truncate the array, possibly deleting held
     * elements for i >= newSize.  If newSize is larger, grow the
     * array, filling in new slots with nullptr.
     */
    शून्य setSize(int32_t newSize, UErrorCode &status);

    /**
     * Fill in the given array with all elements of this vector.
     */
    शून्य** toArray(शून्य** result) स्थिरांक;

    //------------------------------------------------------------
    // New API
    //------------------------------------------------------------

    UObjectDeleter *setDeleter(UObjectDeleter *d);
    bool hasDeleter() {परत deleter != nullptr;}

    UElementsAreEqual *setComparer(UElementsAreEqual *c);

    ओळीत शून्य* चालक[](int32_t index) स्थिरांक {परत elementAt(index);}

    /**
     * Removes the element at the given index from this vector and
     * transfer ownership of it to the caller.  After this call, the
     * caller owns the result and must delete it and the vector entry
     * at 'index' is removed, shifting all subsequent entries back by
     * one index and shortening the size of the vector by one.  If the
     * index is out of range or if there is no item at the given index
     * then 0 is returned and the vector is unchanged.
     */
    शून्य* orphanElementAt(int32_t index);

    /**
     * Returns true if this vector contains none of the elements
     * of the given vector.
     * @param other vector to be checked for containment
     * @return true if the test condition is met
     */
    UBool containsNone(स्थिरांक UVector& other) स्थिरांक;

    /**
     * Insert the given object into this vector at its sorted position
     * as defined by 'compare'.  The current elements are assumed to
     * be sorted already.
     */
    शून्य sorted_घाला(शून्य* obj, UElementComparator *compare, UErrorCode& ec);

    /**
     * Insert the given integer into this vector at its sorted position
     * as defined by 'compare'.  The current elements are assumed to
     * be sorted already.
     */
    शून्य sorted_घाला(int32_t obj, UElementComparator *compare, UErrorCode& ec);

    /**
     * Sort the contents of the vector, assuming that the contents of the
     * vector are of type int32_t.
     */
    शून्य sorti(UErrorCode &ec);

    /**
      * Sort the contents of this vector, using a caller-supplied function
      * to do the comparisons.  (It's confusing that
      *  UVector's UElementComparator function is different from the
      *  UComparator function type defined in uarrsort.h)
      */
    शून्य sort(UElementComparator *compare, UErrorCode &ec);

    /**
     * Stable sort the contents of this vector using a caller-supplied function
     * of type UComparator to do the comparison.  Provides more flexibility
     * than UVector::sort() because an additional user parameter can be passed to
     * the comparison function.
     */
    शून्य sortWithUComparator(UComparator *compare, स्थिरांक शून्य *context, UErrorCode &ec);

    /**
     * ICU "poor man's RTTI", returns a UClassID for this class.
     */
    स्थिर UClassID U_EXPORT2 getStaticClassID();

    /**
     * ICU "poor man's RTTI", returns a UClassID for the actual class.
     */
    आभासी UClassID getDynamicClassID() स्थिरांक override;

खाजगी:
    int32_t indexOf(UElement किल्ली, int32_t startIndex = 0, int8_t hint = 0) स्थिरांक;

    शून्य sorted_घाला(UElement e, UElementComparator *compare, UErrorCode& ec);

सार्वजनिक:
    // Disallow
    UVector(स्थिरांक UVector&) = delete;

    // Disallow
    UVector& चालक=(स्थिरांक UVector&) = delete;

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
class U_COMMON_API UStack : सार्वजनिक UVector {
सार्वजनिक:
    UStack(UErrorCode &status);

    UStack(int32_t initialCapacity, UErrorCode &status);

    UStack(UObjectDeleter *d, UElementsAreEqual *c, UErrorCode &status);

    UStack(UObjectDeleter *d, UElementsAreEqual *c, int32_t initialCapacity, UErrorCode &status);

    आभासी ~UStack();

    // It's okay not to have a virtual destructor (in UVector)
    // because UStack has no special cleanup to do.

    ओळीत UBool empty() स्थिरांक {परत isEmpty();}

    ओळीत शून्य* peek() स्थिरांक {परत lastElement();}

    ओळीत int32_t peeki() स्थिरांक {परत lastElementi();}
    
    /**
     * Pop and return an element from the stack.
     * For stacks with a deleter function, the caller takes ownership
     * of the popped element.
     */
    शून्य* काढा();
    
    int32_t popi();
    
    ओळीत शून्य* ढकला(शून्य* obj, UErrorCode &status) {
        जर (hasDeleter()) {
            adoptElement(obj, status);
            परत (U_SUCCESS(status)) ? obj : nullptr;
        } अन्यथा {
            addElement(obj, status);
            परत obj;
        }
    }

    ओळीत int32_t ढकला(int32_t i, UErrorCode &status) {
        addElement(i, status);
        परत i;
    }

    /*
    If the object o occurs as an item in this stack,
    this method returns the 1-based distance from the top of the stack.
    */
    int32_t शोध(शून्य* obj) स्थिरांक;

    /**
     * ICU "poor man's RTTI", returns a UClassID for this class.
     */
    स्थिर UClassID U_EXPORT2 getStaticClassID();

    /**
     * ICU "poor man's RTTI", returns a UClassID for the actual class.
     */
    आभासी UClassID getDynamicClassID() स्थिरांक override;

    // Disallow
    UStack(स्थिरांक UStack&) = delete;

    // Disallow
    UStack& चालक=(स्थिरांक UStack&) = delete;
};

U_NAMESPACE_END

#endif
