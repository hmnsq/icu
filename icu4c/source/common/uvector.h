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

#അന്തര്ഭാവ "unicode/utypes.h"
#അന്തര്ഭാവ "unicode/uobject.h"
#അന്തര്ഭാവ "cmemory.h"
#അന്തര്ഭാവ "uarrsort.h"
#അന്തര്ഭാവ "uelement.h"

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
class U_COMMON_API UVector : പൊതു UObject {
    // NOTE: UVector uses the UElement (union of void* and int32_t) as
    // its basic storage type.  It uses UElementsAreEqual as its
    // comparison function.  It uses UObjectDeleter as its deleter
    // function.  This allows sharing of support functions with UHashtable.

സ്വകാര്യ:
    int32_t എണ്ണം = 0;

    int32_t capacity = 0;

    UElement* elements = nullptr;

    UObjectDeleter *deleter = nullptr;

    UElementsAreEqual *comparer = nullptr;

പൊതു:
    UVector(UErrorCode &status);

    UVector(int32_t initialCapacity, UErrorCode &status);

    UVector(UObjectDeleter *d, UElementsAreEqual *c, UErrorCode &status);

    UVector(UObjectDeleter *d, UElementsAreEqual *c, int32_t initialCapacity, UErrorCode &status);

    മായാ ~UVector();

    /**
     * Assign this object to another (make this a copy of 'other').
     * Use the 'assign' function to assign each element.
     */
    ശൂന്യം assign(സ്ഥിരം UVector& other, UElementAssigner *assign, UErrorCode &ec);

    /**
     * Compare this vector with another.  They will be considered
     * equal if they are of the same size and all elements are equal,
     * as compared using this object's comparer.
     */
    bool operator==(സ്ഥിരം UVector& other) സ്ഥിരം;

    /**
     * Equivalent to !operator==()
     */
    വരിയിൽ bool operator!=(സ്ഥിരം UVector& other) സ്ഥിരം {തിരികെ !operator==(other);}

    //------------------------------------------------------------
    // java.util.Vector API
    //------------------------------------------------------------

    /**
     * Add an element at the end of the vector.
     * For use only with vectors that do not adopt their elements, which is to say,
     * have not set an element deleter function. See `adoptElement()`.
     */
    ശൂന്യം addElement(ശൂന്യം *obj, UErrorCode &status);

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
    ശൂന്യം adoptElement(ശൂന്യം *obj, UErrorCode &status);

    ശൂന്യം addElement(int32_t elem, UErrorCode &status);

    ശൂന്യം setElementAt(ശൂന്യം* obj, int32_t index);

    ശൂന്യം setElementAt(int32_t elem, int32_t index);

    ശൂന്യം തിരുകുക_Element_At(ശൂന്യം* obj, int32_t index, UErrorCode &status);

    ശൂന്യം തിരുകുക_Element_At(int32_t elem, int32_t index, UErrorCode &status);
    
    ശൂന്യം* elementAt(int32_t index) സ്ഥിരം;

    int32_t elementAti(int32_t index) സ്ഥിരം;

    UBool equals(സ്ഥിരം UVector &other) സ്ഥിരം;

    വരിയിൽ ശൂന്യം* firstElement() സ്ഥിരം {തിരികെ elementAt(0);}

    വരിയിൽ ശൂന്യം* lastElement() സ്ഥിരം {തിരികെ elementAt(എണ്ണം-1);}

    വരിയിൽ int32_t lastElementi() സ്ഥിരം {തിരികെ elementAti(എണ്ണം-1);}

    int32_t indexOf(ശൂന്യം* obj, int32_t startIndex = 0) സ്ഥിരം;

    int32_t indexOf(int32_t obj, int32_t startIndex = 0) സ്ഥിരം;

    വരിയിൽ UBool contains(ശൂന്യം* obj) സ്ഥിരം {തിരികെ indexOf(obj) >= 0;}

    വരിയിൽ UBool contains(int32_t obj) സ്ഥിരം {തിരികെ indexOf(obj) >= 0;}

    UBool containsAll(സ്ഥിരം UVector& other) സ്ഥിരം;

    UBool removeAll(സ്ഥിരം UVector& other);

    UBool retainAll(സ്ഥിരം UVector& other);

    ശൂന്യം removeElementAt(int32_t index);

    UBool removeElement(ശൂന്യം* obj);

    ശൂന്യം removeAllElements();

    വരിയിൽ int32_t size() സ്ഥിരം {തിരികെ എണ്ണം;}

    വരിയിൽ UBool isEmpty() സ്ഥിരം {തിരികെ എണ്ണം == 0;}

    UBool ensureCapacity(int32_t minimumCapacity, UErrorCode &status);

    /**
     * Change the size of this vector as follows: If newSize is
     * smaller, then truncate the array, possibly deleting held
     * elements for i >= newSize.  If newSize is larger, grow the
     * array, filling in new slots with nullptr.
     */
    ശൂന്യം setSize(int32_t newSize, UErrorCode &status);

    /**
     * Fill in the given array with all elements of this vector.
     */
    ശൂന്യം** toArray(ശൂന്യം** result) സ്ഥിരം;

    //------------------------------------------------------------
    // New API
    //------------------------------------------------------------

    UObjectDeleter *setDeleter(UObjectDeleter *d);
    bool hasDeleter() {തിരികെ deleter != nullptr;}

    UElementsAreEqual *setComparer(UElementsAreEqual *c);

    വരിയിൽ ശൂന്യം* operator[](int32_t index) സ്ഥിരം {തിരികെ elementAt(index);}

    /**
     * Removes the element at the given index from this vector and
     * transfer ownership of it to the caller.  After this call, the
     * caller owns the result and must delete it and the vector entry
     * at 'index' is removed, shifting all subsequent entries back by
     * one index and shortening the size of the vector by one.  If the
     * index is out of range or if there is no item at the given index
     * then 0 is returned and the vector is unchanged.
     */
    ശൂന്യം* orphanElementAt(int32_t index);

    /**
     * Returns true if this vector contains none of the elements
     * of the given vector.
     * @param other vector to be checked for containment
     * @return true if the test condition is met
     */
    UBool containsNone(സ്ഥിരം UVector& other) സ്ഥിരം;

    /**
     * Insert the given object into this vector at its sorted position
     * as defined by 'compare'.  The current elements are assumed to
     * be sorted already.
     */
    ശൂന്യം sorted_തിരുകുക(ശൂന്യം* obj, UElementComparator *compare, UErrorCode& ec);

    /**
     * Insert the given integer into this vector at its sorted position
     * as defined by 'compare'.  The current elements are assumed to
     * be sorted already.
     */
    ശൂന്യം sorted_തിരുകുക(int32_t obj, UElementComparator *compare, UErrorCode& ec);

    /**
     * Sort the contents of the vector, assuming that the contents of the
     * vector are of type int32_t.
     */
    ശൂന്യം sorti(UErrorCode &ec);

    /**
      * Sort the contents of this vector, using a caller-supplied function
      * to do the comparisons.  (It's confusing that
      *  UVector's UElementComparator function is different from the
      *  UComparator function type defined in uarrsort.h)
      */
    ശൂന്യം sort(UElementComparator *compare, UErrorCode &ec);

    /**
     * Stable sort the contents of this vector using a caller-supplied function
     * of type UComparator to do the comparison.  Provides more flexibility
     * than UVector::sort() because an additional user parameter can be passed to
     * the comparison function.
     */
    ശൂന്യം sortWithUComparator(UComparator *compare, സ്ഥിരം ശൂന്യം *context, UErrorCode &ec);

    /**
     * ICU "poor man's RTTI", returns a UClassID for this class.
     */
    സ്ഥിതം UClassID U_EXPORT2 getStaticClassID();

    /**
     * ICU "poor man's RTTI", returns a UClassID for the actual class.
     */
    മായാ UClassID getDynamicClassID() സ്ഥിരം override;

സ്വകാര്യ:
    int32_t indexOf(UElement താക്കോൽ, int32_t startIndex = 0, int8_t hint = 0) സ്ഥിരം;

    ശൂന്യം sorted_തിരുകുക(UElement e, UElementComparator *compare, UErrorCode& ec);

പൊതു:
    // Disallow
    UVector(സ്ഥിരം UVector&) = delete;

    // Disallow
    UVector& operator=(സ്ഥിരം UVector&) = delete;

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
class U_COMMON_API UStack : പൊതു UVector {
പൊതു:
    UStack(UErrorCode &status);

    UStack(int32_t initialCapacity, UErrorCode &status);

    UStack(UObjectDeleter *d, UElementsAreEqual *c, UErrorCode &status);

    UStack(UObjectDeleter *d, UElementsAreEqual *c, int32_t initialCapacity, UErrorCode &status);

    മായാ ~UStack();

    // It's okay not to have a virtual destructor (in UVector)
    // because UStack has no special cleanup to do.

    വരിയിൽ UBool empty() സ്ഥിരം {തിരികെ isEmpty();}

    വരിയിൽ ശൂന്യം* peek() സ്ഥിരം {തിരികെ lastElement();}

    വരിയിൽ int32_t peeki() സ്ഥിരം {തിരികെ lastElementi();}
    
    /**
     * Pop and return an element from the stack.
     * For stacks with a deleter function, the caller takes ownership
     * of the popped element.
     */
    ശൂന്യം* എടുക്കുക();
    
    int32_t popi();
    
    വരിയിൽ ശൂന്യം* തള്ളുക(ശൂന്യം* obj, UErrorCode &status) {
        എങ്കിൽ (hasDeleter()) {
            adoptElement(obj, status);
            തിരികെ (U_SUCCESS(status)) ? obj : nullptr;
        } അല്ലെങ്കിൽ {
            addElement(obj, status);
            തിരികെ obj;
        }
    }

    വരിയിൽ int32_t തള്ളുക(int32_t i, UErrorCode &status) {
        addElement(i, status);
        തിരികെ i;
    }

    /*
    If the object o occurs as an item in this stack,
    this method returns the 1-based distance from the top of the stack.
    */
    int32_t തിരയൽ(ശൂന്യം* obj) സ്ഥിരം;

    /**
     * ICU "poor man's RTTI", returns a UClassID for this class.
     */
    സ്ഥിതം UClassID U_EXPORT2 getStaticClassID();

    /**
     * ICU "poor man's RTTI", returns a UClassID for the actual class.
     */
    മായാ UClassID getDynamicClassID() സ്ഥിരം override;

    // Disallow
    UStack(സ്ഥിരം UStack&) = delete;

    // Disallow
    UStack& operator=(സ്ഥിരം UStack&) = delete;
};

U_NAMESPACE_END

#endif
