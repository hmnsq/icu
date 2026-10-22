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

#அந்தர்பாவ "unicode/utypes.h"
#அந்தர்பாவ "unicode/uobject.h"
#அந்தர்பாவ "cmemory.h"
#அந்தர்பாவ "uarrsort.h"
#அந்தர்பாவ "uelement.h"

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
class U_COMMON_API UVector : பொது UObject {
    // NOTE: UVector uses the UElement (union of void* and int32_t) as
    // its basic storage type.  It uses UElementsAreEqual as its
    // comparison function.  It uses UObjectDeleter as its deleter
    // function.  This allows sharing of support functions with UHashtable.

தனிப்பட்ட:
    int32_t மொத்தம் = 0;

    int32_t capacity = 0;

    UElement* elements = nullptr;

    UObjectDeleter *deleter = nullptr;

    UElementsAreEqual *comparer = nullptr;

பொது:
    UVector(UErrorCode &status);

    UVector(int32_t initialCapacity, UErrorCode &status);

    UVector(UObjectDeleter *d, UElementsAreEqual *c, UErrorCode &status);

    UVector(UObjectDeleter *d, UElementsAreEqual *c, int32_t initialCapacity, UErrorCode &status);

    மெய்நிகர் ~UVector();

    /**
     * Assign this object to another (make this a copy of 'other').
     * Use the 'assign' function to assign each element.
     */
    வெறுமை assign(மாறிலி UVector& other, UElementAssigner *assign, UErrorCode &ec);

    /**
     * Compare this vector with another.  They will be considered
     * equal if they are of the same size and all elements are equal,
     * as compared using this object's comparer.
     */
    bool operator==(மாறிலி UVector& other) மாறிலி;

    /**
     * Equivalent to !operator==()
     */
    உள்வரி bool operator!=(மாறிலி UVector& other) மாறிலி {திரும்பு !operator==(other);}

    //------------------------------------------------------------
    // java.util.Vector API
    //------------------------------------------------------------

    /**
     * Add an element at the end of the vector.
     * For use only with vectors that do not adopt their elements, which is to say,
     * have not set an element deleter function. See `adoptElement()`.
     */
    வெறுமை addElement(வெறுமை *obj, UErrorCode &status);

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
    வெறுமை adoptElement(வெறுமை *obj, UErrorCode &status);

    வெறுமை addElement(int32_t elem, UErrorCode &status);

    வெறுமை setElementAt(வெறுமை* obj, int32_t index);

    வெறுமை setElementAt(int32_t elem, int32_t index);

    வெறுமை செருகு_Element_At(வெறுமை* obj, int32_t index, UErrorCode &status);

    வெறுமை செருகு_Element_At(int32_t elem, int32_t index, UErrorCode &status);
    
    வெறுமை* elementAt(int32_t index) மாறிலி;

    int32_t elementAti(int32_t index) மாறிலி;

    UBool equals(மாறிலி UVector &other) மாறிலி;

    உள்வரி வெறுமை* firstElement() மாறிலி {திரும்பு elementAt(0);}

    உள்வரி வெறுமை* lastElement() மாறிலி {திரும்பு elementAt(மொத்தம்-1);}

    உள்வரி int32_t lastElementi() மாறிலி {திரும்பு elementAti(மொத்தம்-1);}

    int32_t indexOf(வெறுமை* obj, int32_t startIndex = 0) மாறிலி;

    int32_t indexOf(int32_t obj, int32_t startIndex = 0) மாறிலி;

    உள்வரி UBool contains(வெறுமை* obj) மாறிலி {திரும்பு indexOf(obj) >= 0;}

    உள்வரி UBool contains(int32_t obj) மாறிலி {திரும்பு indexOf(obj) >= 0;}

    UBool containsAll(மாறிலி UVector& other) மாறிலி;

    UBool removeAll(மாறிலி UVector& other);

    UBool retainAll(மாறிலி UVector& other);

    வெறுமை removeElementAt(int32_t index);

    UBool removeElement(வெறுமை* obj);

    வெறுமை removeAllElements();

    உள்வரி int32_t size() மாறிலி {திரும்பு மொத்தம்;}

    உள்வரி UBool isEmpty() மாறிலி {திரும்பு மொத்தம் == 0;}

    UBool ensureCapacity(int32_t minimumCapacity, UErrorCode &status);

    /**
     * Change the size of this vector as follows: If newSize is
     * smaller, then truncate the array, possibly deleting held
     * elements for i >= newSize.  If newSize is larger, grow the
     * array, filling in new slots with nullptr.
     */
    வெறுமை setSize(int32_t newSize, UErrorCode &status);

    /**
     * Fill in the given array with all elements of this vector.
     */
    வெறுமை** toArray(வெறுமை** result) மாறிலி;

    //------------------------------------------------------------
    // New API
    //------------------------------------------------------------

    UObjectDeleter *setDeleter(UObjectDeleter *d);
    bool hasDeleter() {திரும்பு deleter != nullptr;}

    UElementsAreEqual *setComparer(UElementsAreEqual *c);

    உள்வரி வெறுமை* operator[](int32_t index) மாறிலி {திரும்பு elementAt(index);}

    /**
     * Removes the element at the given index from this vector and
     * transfer ownership of it to the caller.  After this call, the
     * caller owns the result and must delete it and the vector entry
     * at 'index' is removed, shifting all subsequent entries back by
     * one index and shortening the size of the vector by one.  If the
     * index is out of range or if there is no item at the given index
     * then 0 is returned and the vector is unchanged.
     */
    வெறுமை* orphanElementAt(int32_t index);

    /**
     * Returns true if this vector contains none of the elements
     * of the given vector.
     * @param other vector to be checked for containment
     * @return true if the test condition is met
     */
    UBool containsNone(மாறிலி UVector& other) மாறிலி;

    /**
     * Insert the given object into this vector at its sorted position
     * as defined by 'compare'.  The current elements are assumed to
     * be sorted already.
     */
    வெறுமை sorted_செருகு(வெறுமை* obj, UElementComparator *compare, UErrorCode& ec);

    /**
     * Insert the given integer into this vector at its sorted position
     * as defined by 'compare'.  The current elements are assumed to
     * be sorted already.
     */
    வெறுமை sorted_செருகு(int32_t obj, UElementComparator *compare, UErrorCode& ec);

    /**
     * Sort the contents of the vector, assuming that the contents of the
     * vector are of type int32_t.
     */
    வெறுமை sorti(UErrorCode &ec);

    /**
      * Sort the contents of this vector, using a caller-supplied function
      * to do the comparisons.  (It's confusing that
      *  UVector's UElementComparator function is different from the
      *  UComparator function type defined in uarrsort.h)
      */
    வெறுமை sort(UElementComparator *compare, UErrorCode &ec);

    /**
     * Stable sort the contents of this vector using a caller-supplied function
     * of type UComparator to do the comparison.  Provides more flexibility
     * than UVector::sort() because an additional user parameter can be passed to
     * the comparison function.
     */
    வெறுமை sortWithUComparator(UComparator *compare, மாறிலி வெறுமை *context, UErrorCode &ec);

    /**
     * ICU "poor man's RTTI", returns a UClassID for this class.
     */
    நிலையான UClassID U_EXPORT2 getStaticClassID();

    /**
     * ICU "poor man's RTTI", returns a UClassID for the actual class.
     */
    மெய்நிகர் UClassID getDynamicClassID() மாறிலி override;

தனிப்பட்ட:
    int32_t indexOf(UElement சாவி, int32_t startIndex = 0, int8_t hint = 0) மாறிலி;

    வெறுமை sorted_செருகு(UElement e, UElementComparator *compare, UErrorCode& ec);

பொது:
    // Disallow
    UVector(மாறிலி UVector&) = delete;

    // Disallow
    UVector& operator=(மாறிலி UVector&) = delete;

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
class U_COMMON_API UStack : பொது UVector {
பொது:
    UStack(UErrorCode &status);

    UStack(int32_t initialCapacity, UErrorCode &status);

    UStack(UObjectDeleter *d, UElementsAreEqual *c, UErrorCode &status);

    UStack(UObjectDeleter *d, UElementsAreEqual *c, int32_t initialCapacity, UErrorCode &status);

    மெய்நிகர் ~UStack();

    // It's okay not to have a virtual destructor (in UVector)
    // because UStack has no special cleanup to do.

    உள்வரி UBool empty() மாறிலி {திரும்பு isEmpty();}

    உள்வரி வெறுமை* peek() மாறிலி {திரும்பு lastElement();}

    உள்வரி int32_t peeki() மாறிலி {திரும்பு lastElementi();}
    
    /**
     * Pop and return an element from the stack.
     * For stacks with a deleter function, the caller takes ownership
     * of the popped element.
     */
    வெறுமை* எடு();
    
    int32_t popi();
    
    உள்வரி வெறுமை* தள்ளு(வெறுமை* obj, UErrorCode &status) {
        எனில் (hasDeleter()) {
            adoptElement(obj, status);
            திரும்பு (U_SUCCESS(status)) ? obj : nullptr;
        } இல்லையெனில் {
            addElement(obj, status);
            திரும்பு obj;
        }
    }

    உள்வரி int32_t தள்ளு(int32_t i, UErrorCode &status) {
        addElement(i, status);
        திரும்பு i;
    }

    /*
    If the object o occurs as an item in this stack,
    this method returns the 1-based distance from the top of the stack.
    */
    int32_t தேடல்(வெறுமை* obj) மாறிலி;

    /**
     * ICU "poor man's RTTI", returns a UClassID for this class.
     */
    நிலையான UClassID U_EXPORT2 getStaticClassID();

    /**
     * ICU "poor man's RTTI", returns a UClassID for the actual class.
     */
    மெய்நிகர் UClassID getDynamicClassID() மாறிலி override;

    // Disallow
    UStack(மாறிலி UStack&) = delete;

    // Disallow
    UStack& operator=(மாறிலி UStack&) = delete;
};

U_NAMESPACE_END

#endif
