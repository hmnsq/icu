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

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ UVECTOR_H
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ UVECTOR_H

#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/utypes.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/uobject.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "cmemory.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "uarrsort.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "uelement.h"

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
class U_COMMON_API UVector : ਜਨਤਕ UObject {
    // NOTE: UVector uses the UElement (union of void* and int32_t) as
    // its basic storage type.  It uses UElementsAreEqual as its
    // comparison function.  It uses UObjectDeleter as its deleter
    // function.  This allows sharing of support functions with UHashtable.

ਨਿਜੀ:
    int32_t ਗਿਣਤੀ_2 = 0;

    int32_t ਸਮਰੱਥਾ = 0;

    UElement* elements = nullptr;

    UObjectDeleter *deleter = nullptr;

    UElementsAreEqual *comparer = nullptr;

ਜਨਤਕ:
    UVector(UError_ਕੋਡ &ਸਥਿਤੀ);

    UVector(int32_t initial_ਸਮਰੱਥਾ, UError_ਕੋਡ &ਸਥਿਤੀ);

    UVector(UObjectDeleter *d, UElementsAreEqual *c, UError_ਕੋਡ &ਸਥਿਤੀ);

    UVector(UObjectDeleter *d, UElementsAreEqual *c, int32_t initial_ਸਮਰੱਥਾ, UError_ਕੋਡ &ਸਥਿਤੀ);

    ਆਭਾਸੀ ~UVector();

    /**
     * Assign this object to another (make this a copy of 'other').
     * Use the 'assign' function to assign each element.
     */
    ਖਾਲੀ ਸੌਂਪੋ(ਅਚਲ UVector& other, UElementAssigner *ਸੌਂਪੋ, UError_ਕੋਡ &ec);

    /**
     * Compare this vector with another.  They will be considered
     * equal if they are of the same size and all elements are equal,
     * as compared using this object's comparer.
     */
    bool ਸੰਚਾਲਕ==(ਅਚਲ UVector& other) ਅਚਲ;

    /**
     * Equivalent to !operator==()
     */
    ਸਤਰਅੰਦਰ bool ਸੰਚਾਲਕ!=(ਅਚਲ UVector& other) ਅਚਲ {ਵਾਪਸ !ਸੰਚਾਲਕ==(other);}

    //------------------------------------------------------------
    // java.util.Vector API
    //------------------------------------------------------------

    /**
     * Add an element at the end of the vector.
     * For use only with vectors that do not adopt their elements, which is to say,
     * have not set an element deleter function. See `adoptElement()`.
     */
    ਖਾਲੀ ਜੋੜੋ_Element(ਖਾਲੀ *obj, UError_ਕੋਡ &ਸਥਿਤੀ);

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
    ਖਾਲੀ adoptElement(ਖਾਲੀ *obj, UError_ਕੋਡ &ਸਥਿਤੀ);

    ਖਾਲੀ ਜੋੜੋ_Element(int32_t elem, UError_ਕੋਡ &ਸਥਿਤੀ);

    ਖਾਲੀ ਸੈੱਟ_Element_At(ਖਾਲੀ* obj, int32_t ਸੂਚਕਾਂਕ);

    ਖਾਲੀ ਸੈੱਟ_Element_At(int32_t elem, int32_t ਸੂਚਕਾਂਕ);

    ਖਾਲੀ ਪਾਓ_Element_At(ਖਾਲੀ* obj, int32_t ਸੂਚਕਾਂਕ, UError_ਕੋਡ &ਸਥਿਤੀ);

    ਖਾਲੀ ਪਾਓ_Element_At(int32_t elem, int32_t ਸੂਚਕਾਂਕ, UError_ਕੋਡ &ਸਥਿਤੀ);
    
    ਖਾਲੀ* elementAt(int32_t ਸੂਚਕਾਂਕ) ਅਚਲ;

    int32_t elementAti(int32_t ਸੂਚਕਾਂਕ) ਅਚਲ;

    UBool equals(ਅਚਲ UVector &other) ਅਚਲ;

    ਸਤਰਅੰਦਰ ਖਾਲੀ* ਪਹਿਲਾ_Element() ਅਚਲ {ਵਾਪਸ elementAt(0);}

    ਸਤਰਅੰਦਰ ਖਾਲੀ* ਆਖਰੀ_Element() ਅਚਲ {ਵਾਪਸ elementAt(ਗਿਣਤੀ_2-1);}

    ਸਤਰਅੰਦਰ int32_t ਆਖਰੀ_Elementi() ਅਚਲ {ਵਾਪਸ elementAti(ਗਿਣਤੀ_2-1);}

    int32_t ਸੂਚਕਾਂਕ_ਦਾ(ਖਾਲੀ* obj, int32_t ਸ਼ੁਰੂ_ਸੂਚਕਾਂਕ = 0) ਅਚਲ;

    int32_t ਸੂਚਕਾਂਕ_ਦਾ(int32_t obj, int32_t ਸ਼ੁਰੂ_ਸੂਚਕਾਂਕ = 0) ਅਚਲ;

    ਸਤਰਅੰਦਰ UBool ਸ਼ਾਮਲ_ਹੈ(ਖਾਲੀ* obj) ਅਚਲ {ਵਾਪਸ ਸੂਚਕਾਂਕ_ਦਾ(obj) >= 0;}

    ਸਤਰਅੰਦਰ UBool ਸ਼ਾਮਲ_ਹੈ(int32_t obj) ਅਚਲ {ਵਾਪਸ ਸੂਚਕਾਂਕ_ਦਾ(obj) >= 0;}

    UBool ਸ਼ਾਮਲ_ਹੈ_All(ਅਚਲ UVector& other) ਅਚਲ;

    UBool removeAll(ਅਚਲ UVector& other);

    UBool retainAll(ਅਚਲ UVector& other);

    ਖਾਲੀ removeElementAt(int32_t ਸੂਚਕਾਂਕ);

    UBool removeElement(ਖਾਲੀ* obj);

    ਖਾਲੀ removeAllElements();

    ਸਤਰਅੰਦਰ int32_t ਮਾਪ_2() ਅਚਲ {ਵਾਪਸ ਗਿਣਤੀ_2;}

    ਸਤਰਅੰਦਰ UBool is_ਖਾਲੀ_ਥਾਵਾਂ() ਅਚਲ {ਵਾਪਸ ਗਿਣਤੀ_2 == 0;}

    UBool ensure_ਸਮਰੱਥਾ(int32_t minimum_ਸਮਰੱਥਾ, UError_ਕੋਡ &ਸਥਿਤੀ);

    /**
     * Change the size of this vector as follows: If newSize is
     * smaller, then truncate the array, possibly deleting held
     * elements for i >= newSize.  If newSize is larger, grow the
     * array, filling in new slots with nullptr.
     */
    ਖਾਲੀ ਸੈੱਟ_ਮਾਪ(int32_t new_ਮਾਪ, UError_ਕੋਡ &ਸਥਿਤੀ);

    /**
     * Fill in the given array with all elements of this vector.
     */
    ਖਾਲੀ** ਮੰਜ਼ਿਲ_Array(ਖਾਲੀ** ਨਤੀਜਾ) ਅਚਲ;

    //------------------------------------------------------------
    // New API
    //------------------------------------------------------------

    UObjectDeleter *ਸੈੱਟ_Deleter(UObjectDeleter *d);
    bool hasDeleter() {ਵਾਪਸ deleter != nullptr;}

    UElementsAreEqual *ਸੈੱਟ_Comparer(UElementsAreEqual *c);

    ਸਤਰਅੰਦਰ ਖਾਲੀ* ਸੰਚਾਲਕ[](int32_t ਸੂਚਕਾਂਕ) ਅਚਲ {ਵਾਪਸ elementAt(ਸੂਚਕਾਂਕ);}

    /**
     * Removes the element at the given index from this vector and
     * transfer ownership of it to the caller.  After this call, the
     * caller owns the result and must delete it and the vector entry
     * at 'index' is removed, shifting all subsequent entries back by
     * one index and shortening the size of the vector by one.  If the
     * index is out of range or if there is no item at the given index
     * then 0 is returned and the vector is unchanged.
     */
    ਖਾਲੀ* orphanElementAt(int32_t ਸੂਚਕਾਂਕ);

    /**
     * Returns true if this vector contains none of the elements
     * of the given vector.
     * @param other vector to be checked for containment
     * @return true if the test condition is met
     */
    UBool ਸ਼ਾਮਲ_ਹੈ_None(ਅਚਲ UVector& other) ਅਚਲ;

    /**
     * Insert the given object into this vector at its sorted position
     * as defined by 'compare'.  The current elements are assumed to
     * be sorted already.
     */
    ਖਾਲੀ sorted_ਪਾਓ(ਖਾਲੀ* obj, UElementComparator *compare, UError_ਕੋਡ& ec);

    /**
     * Insert the given integer into this vector at its sorted position
     * as defined by 'compare'.  The current elements are assumed to
     * be sorted already.
     */
    ਖਾਲੀ sorted_ਪਾਓ(int32_t obj, UElementComparator *compare, UError_ਕੋਡ& ec);

    /**
     * Sort the contents of the vector, assuming that the contents of the
     * vector are of type int32_t.
     */
    ਖਾਲੀ sorti(UError_ਕੋਡ &ec);

    /**
      * Sort the contents of this vector, using a caller-supplied function
      * to do the comparisons.  (It's confusing that
      *  UVector's UElementComparator function is different from the
      *  UComparator function type defined in uarrsort.h)
      */
    ਖਾਲੀ ਛਾਂਟੋ(UElementComparator *compare, UError_ਕੋਡ &ec);

    /**
     * Stable sort the contents of this vector using a caller-supplied function
     * of type UComparator to do the comparison.  Provides more flexibility
     * than UVector::sort() because an additional user parameter can be passed to
     * the comparison function.
     */
    ਖਾਲੀ ਛਾਂਟੋ_With_UComparator(UComparator *compare, ਅਚਲ ਖਾਲੀ *context, UError_ਕੋਡ &ec);

    /**
     * ICU "poor man's RTTI", returns a UClassID for this class.
     */
    ਸਥਿਰ UClassID U_EXPORT2 getStaticClassID();

    /**
     * ICU "poor man's RTTI", returns a UClassID for the actual class.
     */
    ਆਭਾਸੀ UClassID getDynamicClassID() ਅਚਲ override;

ਨਿਜੀ:
    int32_t ਸੂਚਕਾਂਕ_ਦਾ(UElement ਕੁੰਜੀ, int32_t ਸ਼ੁਰੂ_ਸੂਚਕਾਂਕ = 0, int8_t hint = 0) ਅਚਲ;

    ਖਾਲੀ sorted_ਪਾਓ(UElement e, UElementComparator *compare, UError_ਕੋਡ& ec);

ਜਨਤਕ:
    // Disallow
    UVector(ਅਚਲ UVector&) = delete;

    // Disallow
    UVector& ਸੰਚਾਲਕ=(ਅਚਲ UVector&) = delete;

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
class U_COMMON_API UStack : ਜਨਤਕ UVector {
ਜਨਤਕ:
    UStack(UError_ਕੋਡ &ਸਥਿਤੀ);

    UStack(int32_t initial_ਸਮਰੱਥਾ, UError_ਕੋਡ &ਸਥਿਤੀ);

    UStack(UObjectDeleter *d, UElementsAreEqual *c, UError_ਕੋਡ &ਸਥਿਤੀ);

    UStack(UObjectDeleter *d, UElementsAreEqual *c, int32_t initial_ਸਮਰੱਥਾ, UError_ਕੋਡ &ਸਥਿਤੀ);

    ਆਭਾਸੀ ~UStack();

    // It's okay not to have a virtual destructor (in UVector)
    // because UStack has no special cleanup to do.

    ਸਤਰਅੰਦਰ UBool ਖਾਲੀ_ਥਾਵਾਂ() ਅਚਲ {ਵਾਪਸ is_ਖਾਲੀ_ਥਾਵਾਂ();}

    ਸਤਰਅੰਦਰ ਖਾਲੀ* peek() ਅਚਲ {ਵਾਪਸ ਆਖਰੀ_Element();}

    ਸਤਰਅੰਦਰ int32_t peeki() ਅਚਲ {ਵਾਪਸ ਆਖਰੀ_Elementi();}
    
    /**
     * Pop and return an element from the stack.
     * For stacks with a deleter function, the caller takes ownership
     * of the popped element.
     */
    ਖਾਲੀ* ਕੱਢੋ_2();
    
    int32_t popi();
    
    ਸਤਰਅੰਦਰ ਖਾਲੀ* ਧੱਕੋ(ਖਾਲੀ* obj, UError_ਕੋਡ &ਸਥਿਤੀ) {
        ਜੇ (hasDeleter()) {
            adoptElement(obj, ਸਥਿਤੀ);
            ਵਾਪਸ (U_SUCCESS(ਸਥਿਤੀ)) ? obj : nullptr;
        } ਨਹੀਂਤਾਂ {
            ਜੋੜੋ_Element(obj, ਸਥਿਤੀ);
            ਵਾਪਸ obj;
        }
    }

    ਸਤਰਅੰਦਰ int32_t ਧੱਕੋ(int32_t i, UError_ਕੋਡ &ਸਥਿਤੀ) {
        ਜੋੜੋ_Element(i, ਸਥਿਤੀ);
        ਵਾਪਸ i;
    }

    /*
    If the object o occurs as an item in this stack,
    this method returns the 1-based distance from the top of the stack.
    */
    int32_t ਖੋਜ(ਖਾਲੀ* obj) ਅਚਲ;

    /**
     * ICU "poor man's RTTI", returns a UClassID for this class.
     */
    ਸਥਿਰ UClassID U_EXPORT2 getStaticClassID();

    /**
     * ICU "poor man's RTTI", returns a UClassID for the actual class.
     */
    ਆਭਾਸੀ UClassID getDynamicClassID() ਅਚਲ override;

    // Disallow
    UStack(ਅਚਲ UStack&) = delete;

    // Disallow
    UStack& ਸੰਚਾਲਕ=(ਅਚਲ UStack&) = delete;
};

U_NAMESPACE_ਸਿਰਾ

#ਜੇ_ਅੰਤ
