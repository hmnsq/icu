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

#અન્તર્ભાવ "unicode/utypes.h"
#અન્તર્ભાવ "unicode/uobject.h"
#અન્તર્ભાવ "cmemory.h"
#અન્તર્ભાવ "uarrsort.h"
#અન્તર્ભાવ "uelement.h"

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
class U_COMMON_API UVector : જાહેર UObject {
    // NOTE: UVector uses the UElement (union of void* and int32_t) as
    // its basic storage type.  It uses UElementsAreEqual as its
    // comparison function.  It uses UObjectDeleter as its deleter
    // function.  This allows sharing of support functions with UHashtable.

ખાનગી:
    int32_t ગણતરી = 0;

    int32_t capacity = 0;

    UElement* elements = nullptr;

    UObjectDeleter *deleter = nullptr;

    UElementsAreEqual *comparer = nullptr;

જાહેર:
    UVector(UErrorCode &status);

    UVector(int32_t initialCapacity, UErrorCode &status);

    UVector(UObjectDeleter *d, UElementsAreEqual *c, UErrorCode &status);

    UVector(UObjectDeleter *d, UElementsAreEqual *c, int32_t initialCapacity, UErrorCode &status);

    આભાસી ~UVector();

    /**
     * Assign this object to another (make this a copy of 'other').
     * Use the 'assign' function to assign each element.
     */
    શૂન્ય assign(અચલ UVector& other, UElementAssigner *assign, UErrorCode &ec);

    /**
     * Compare this vector with another.  They will be considered
     * equal if they are of the same size and all elements are equal,
     * as compared using this object's comparer.
     */
    bool operator==(અચલ UVector& other) અચલ;

    /**
     * Equivalent to !operator==()
     */
    અંતઃરેખ bool operator!=(અચલ UVector& other) અચલ {પાછા !operator==(other);}

    //------------------------------------------------------------
    // java.util.Vector API
    //------------------------------------------------------------

    /**
     * Add an element at the end of the vector.
     * For use only with vectors that do not adopt their elements, which is to say,
     * have not set an element deleter function. See `adoptElement()`.
     */
    શૂન્ય addElement(શૂન્ય *obj, UErrorCode &status);

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
    શૂન્ય adoptElement(શૂન્ય *obj, UErrorCode &status);

    શૂન્ય addElement(int32_t elem, UErrorCode &status);

    શૂન્ય setElementAt(શૂન્ય* obj, int32_t index);

    શૂન્ય setElementAt(int32_t elem, int32_t index);

    શૂન્ય ઉમેરો_Element_At(શૂન્ય* obj, int32_t index, UErrorCode &status);

    શૂન્ય ઉમેરો_Element_At(int32_t elem, int32_t index, UErrorCode &status);
    
    શૂન્ય* elementAt(int32_t index) અચલ;

    int32_t elementAti(int32_t index) અચલ;

    UBool equals(અચલ UVector &other) અચલ;

    અંતઃરેખ શૂન્ય* firstElement() અચલ {પાછા elementAt(0);}

    અંતઃરેખ શૂન્ય* lastElement() અચલ {પાછા elementAt(ગણતરી-1);}

    અંતઃરેખ int32_t lastElementi() અચલ {પાછા elementAti(ગણતરી-1);}

    int32_t indexOf(શૂન્ય* obj, int32_t startIndex = 0) અચલ;

    int32_t indexOf(int32_t obj, int32_t startIndex = 0) અચલ;

    અંતઃરેખ UBool contains(શૂન્ય* obj) અચલ {પાછા indexOf(obj) >= 0;}

    અંતઃરેખ UBool contains(int32_t obj) અચલ {પાછા indexOf(obj) >= 0;}

    UBool containsAll(અચલ UVector& other) અચલ;

    UBool removeAll(અચલ UVector& other);

    UBool retainAll(અચલ UVector& other);

    શૂન્ય removeElementAt(int32_t index);

    UBool removeElement(શૂન્ય* obj);

    શૂન્ય removeAllElements();

    અંતઃરેખ int32_t size() અચલ {પાછા ગણતરી;}

    અંતઃરેખ UBool isEmpty() અચલ {પાછા ગણતરી == 0;}

    UBool ensureCapacity(int32_t minimumCapacity, UErrorCode &status);

    /**
     * Change the size of this vector as follows: If newSize is
     * smaller, then truncate the array, possibly deleting held
     * elements for i >= newSize.  If newSize is larger, grow the
     * array, filling in new slots with nullptr.
     */
    શૂન્ય setSize(int32_t newSize, UErrorCode &status);

    /**
     * Fill in the given array with all elements of this vector.
     */
    શૂન્ય** toArray(શૂન્ય** result) અચલ;

    //------------------------------------------------------------
    // New API
    //------------------------------------------------------------

    UObjectDeleter *setDeleter(UObjectDeleter *d);
    bool hasDeleter() {પાછા deleter != nullptr;}

    UElementsAreEqual *setComparer(UElementsAreEqual *c);

    અંતઃરેખ શૂન્ય* operator[](int32_t index) અચલ {પાછા elementAt(index);}

    /**
     * Removes the element at the given index from this vector and
     * transfer ownership of it to the caller.  After this call, the
     * caller owns the result and must delete it and the vector entry
     * at 'index' is removed, shifting all subsequent entries back by
     * one index and shortening the size of the vector by one.  If the
     * index is out of range or if there is no item at the given index
     * then 0 is returned and the vector is unchanged.
     */
    શૂન્ય* orphanElementAt(int32_t index);

    /**
     * Returns true if this vector contains none of the elements
     * of the given vector.
     * @param other vector to be checked for containment
     * @return true if the test condition is met
     */
    UBool containsNone(અચલ UVector& other) અચલ;

    /**
     * Insert the given object into this vector at its sorted position
     * as defined by 'compare'.  The current elements are assumed to
     * be sorted already.
     */
    શૂન્ય sorted_ઉમેરો(શૂન્ય* obj, UElementComparator *compare, UErrorCode& ec);

    /**
     * Insert the given integer into this vector at its sorted position
     * as defined by 'compare'.  The current elements are assumed to
     * be sorted already.
     */
    શૂન્ય sorted_ઉમેરો(int32_t obj, UElementComparator *compare, UErrorCode& ec);

    /**
     * Sort the contents of the vector, assuming that the contents of the
     * vector are of type int32_t.
     */
    શૂન્ય sorti(UErrorCode &ec);

    /**
      * Sort the contents of this vector, using a caller-supplied function
      * to do the comparisons.  (It's confusing that
      *  UVector's UElementComparator function is different from the
      *  UComparator function type defined in uarrsort.h)
      */
    શૂન્ય sort(UElementComparator *compare, UErrorCode &ec);

    /**
     * Stable sort the contents of this vector using a caller-supplied function
     * of type UComparator to do the comparison.  Provides more flexibility
     * than UVector::sort() because an additional user parameter can be passed to
     * the comparison function.
     */
    શૂન્ય sortWithUComparator(UComparator *compare, અચલ શૂન્ય *context, UErrorCode &ec);

    /**
     * ICU "poor man's RTTI", returns a UClassID for this class.
     */
    સ્થિર UClassID U_EXPORT2 getStaticClassID();

    /**
     * ICU "poor man's RTTI", returns a UClassID for the actual class.
     */
    આભાસી UClassID getDynamicClassID() અચલ override;

ખાનગી:
    int32_t indexOf(UElement ચાવી, int32_t startIndex = 0, int8_t hint = 0) અચલ;

    શૂન્ય sorted_ઉમેરો(UElement e, UElementComparator *compare, UErrorCode& ec);

જાહેર:
    // Disallow
    UVector(અચલ UVector&) = delete;

    // Disallow
    UVector& operator=(અચલ UVector&) = delete;

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
class U_COMMON_API UStack : જાહેર UVector {
જાહેર:
    UStack(UErrorCode &status);

    UStack(int32_t initialCapacity, UErrorCode &status);

    UStack(UObjectDeleter *d, UElementsAreEqual *c, UErrorCode &status);

    UStack(UObjectDeleter *d, UElementsAreEqual *c, int32_t initialCapacity, UErrorCode &status);

    આભાસી ~UStack();

    // It's okay not to have a virtual destructor (in UVector)
    // because UStack has no special cleanup to do.

    અંતઃરેખ UBool empty() અચલ {પાછા isEmpty();}

    અંતઃરેખ શૂન્ય* peek() અચલ {પાછા lastElement();}

    અંતઃરેખ int32_t peeki() અચલ {પાછા lastElementi();}
    
    /**
     * Pop and return an element from the stack.
     * For stacks with a deleter function, the caller takes ownership
     * of the popped element.
     */
    શૂન્ય* કાઢો();
    
    int32_t popi();
    
    અંતઃરેખ શૂન્ય* ધકેલો(શૂન્ય* obj, UErrorCode &status) {
        જો (hasDeleter()) {
            adoptElement(obj, status);
            પાછા (U_SUCCESS(status)) ? obj : nullptr;
        } નહીંતર {
            addElement(obj, status);
            પાછા obj;
        }
    }

    અંતઃરેખ int32_t ધકેલો(int32_t i, UErrorCode &status) {
        addElement(i, status);
        પાછા i;
    }

    /*
    If the object o occurs as an item in this stack,
    this method returns the 1-based distance from the top of the stack.
    */
    int32_t શોધ(શૂન્ય* obj) અચલ;

    /**
     * ICU "poor man's RTTI", returns a UClassID for this class.
     */
    સ્થિર UClassID U_EXPORT2 getStaticClassID();

    /**
     * ICU "poor man's RTTI", returns a UClassID for the actual class.
     */
    આભાસી UClassID getDynamicClassID() અચલ override;

    // Disallow
    UStack(અચલ UStack&) = delete;

    // Disallow
    UStack& operator=(અચલ UStack&) = delete;
};

U_NAMESPACE_END

#endif
