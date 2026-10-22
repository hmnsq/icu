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

#अगर_अघोषित UVECTOR_H
#घोषणा UVECTOR_H

#समावेश "unicode/utypes.h"
#समावेश "unicode/uobject.h"
#समावेश "cmemory.h"
#समावेश "uarrsort.h"
#समावेश "uelement.h"

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
class U_COMMON_API UVector : खुला UObject {
    // NOTE: UVector uses the UElement (union of void* and int32_t) as
    // its basic storage type.  It uses UElementsAreEqual as its
    // comparison function.  It uses UObjectDeleter as its deleter
    // function.  This allows sharing of support functions with UHashtable.

निजी:
    int32_t गिनती = 0;

    int32_t capacity = 0;

    UElement* elements = nullptr;

    UObjectDeleter *deleter = nullptr;

    UElementsAreEqual *comparer = nullptr;

खुला:
    UVector(UErrorCode &status);

    UVector(int32_t initialCapacity, UErrorCode &status);

    UVector(UObjectDeleter *d, UElementsAreEqual *c, UErrorCode &status);

    UVector(UObjectDeleter *d, UElementsAreEqual *c, int32_t initialCapacity, UErrorCode &status);

    भव ~UVector();

    /**
     * Assign this object to another (make this a copy of 'other').
     * Use the 'assign' function to assign each element.
     */
    व्योम assign(स्थिर UVector& other, UElementAssigner *assign, UErrorCode &ec);

    /**
     * Compare this vector with another.  They will be considered
     * equal if they are of the same size and all elements are equal,
     * as compared using this object's comparer.
     */
    bool चालक==(स्थिर UVector& other) स्थिर;

    /**
     * Equivalent to !operator==()
     */
    अंतरभूत bool चालक!=(स्थिर UVector& other) स्थिर {वापस !चालक==(other);}

    //------------------------------------------------------------
    // java.util.Vector API
    //------------------------------------------------------------

    /**
     * Add an element at the end of the vector.
     * For use only with vectors that do not adopt their elements, which is to say,
     * have not set an element deleter function. See `adoptElement()`.
     */
    व्योम addElement(व्योम *obj, UErrorCode &status);

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
    व्योम adoptElement(व्योम *obj, UErrorCode &status);

    व्योम addElement(int32_t elem, UErrorCode &status);

    व्योम setElementAt(व्योम* obj, int32_t सूचकांक);

    व्योम setElementAt(int32_t elem, int32_t सूचकांक);

    व्योम जोड़ो_Element_At(व्योम* obj, int32_t सूचकांक, UErrorCode &status);

    व्योम जोड़ो_Element_At(int32_t elem, int32_t सूचकांक, UErrorCode &status);
    
    व्योम* elementAt(int32_t सूचकांक) स्थिर;

    int32_t elementAti(int32_t सूचकांक) स्थिर;

    UBool equals(स्थिर UVector &other) स्थिर;

    अंतरभूत व्योम* firstElement() स्थिर {वापस elementAt(0);}

    अंतरभूत व्योम* lastElement() स्थिर {वापस elementAt(गिनती-1);}

    अंतरभूत int32_t lastElementi() स्थिर {वापस elementAti(गिनती-1);}

    int32_t सूचकांक_Of(व्योम* obj, int32_t start_सूचकांक = 0) स्थिर;

    int32_t सूचकांक_Of(int32_t obj, int32_t start_सूचकांक = 0) स्थिर;

    अंतरभूत UBool contains(व्योम* obj) स्थिर {वापस सूचकांक_Of(obj) >= 0;}

    अंतरभूत UBool contains(int32_t obj) स्थिर {वापस सूचकांक_Of(obj) >= 0;}

    UBool containsAll(स्थिर UVector& other) स्थिर;

    UBool removeAll(स्थिर UVector& other);

    UBool retainAll(स्थिर UVector& other);

    व्योम removeElementAt(int32_t सूचकांक);

    UBool removeElement(व्योम* obj);

    व्योम removeAllElements();

    अंतरभूत int32_t आकार_2() स्थिर {वापस गिनती;}

    अंतरभूत UBool isEmpty() स्थिर {वापस गिनती == 0;}

    UBool ensureCapacity(int32_t minimumCapacity, UErrorCode &status);

    /**
     * Change the size of this vector as follows: If newSize is
     * smaller, then truncate the array, possibly deleting held
     * elements for i >= newSize.  If newSize is larger, grow the
     * array, filling in new slots with nullptr.
     */
    व्योम set_आकार(int32_t new_आकार, UErrorCode &status);

    /**
     * Fill in the given array with all elements of this vector.
     */
    व्योम** toArray(व्योम** परिणाम) स्थिर;

    //------------------------------------------------------------
    // New API
    //------------------------------------------------------------

    UObjectDeleter *setDeleter(UObjectDeleter *d);
    bool hasDeleter() {वापस deleter != nullptr;}

    UElementsAreEqual *setComparer(UElementsAreEqual *c);

    अंतरभूत व्योम* चालक[](int32_t सूचकांक) स्थिर {वापस elementAt(सूचकांक);}

    /**
     * Removes the element at the given index from this vector and
     * transfer ownership of it to the caller.  After this call, the
     * caller owns the result and must delete it and the vector entry
     * at 'index' is removed, shifting all subsequent entries back by
     * one index and shortening the size of the vector by one.  If the
     * index is out of range or if there is no item at the given index
     * then 0 is returned and the vector is unchanged.
     */
    व्योम* orphanElementAt(int32_t सूचकांक);

    /**
     * Returns true if this vector contains none of the elements
     * of the given vector.
     * @param other vector to be checked for containment
     * @return true if the test condition is met
     */
    UBool containsNone(स्थिर UVector& other) स्थिर;

    /**
     * Insert the given object into this vector at its sorted position
     * as defined by 'compare'.  The current elements are assumed to
     * be sorted already.
     */
    व्योम sorted_जोड़ो(व्योम* obj, UElementComparator *compare, UErrorCode& ec);

    /**
     * Insert the given integer into this vector at its sorted position
     * as defined by 'compare'.  The current elements are assumed to
     * be sorted already.
     */
    व्योम sorted_जोड़ो(int32_t obj, UElementComparator *compare, UErrorCode& ec);

    /**
     * Sort the contents of the vector, assuming that the contents of the
     * vector are of type int32_t.
     */
    व्योम sorti(UErrorCode &ec);

    /**
      * Sort the contents of this vector, using a caller-supplied function
      * to do the comparisons.  (It's confusing that
      *  UVector's UElementComparator function is different from the
      *  UComparator function type defined in uarrsort.h)
      */
    व्योम sort(UElementComparator *compare, UErrorCode &ec);

    /**
     * Stable sort the contents of this vector using a caller-supplied function
     * of type UComparator to do the comparison.  Provides more flexibility
     * than UVector::sort() because an additional user parameter can be passed to
     * the comparison function.
     */
    व्योम sortWithUComparator(UComparator *compare, स्थिर व्योम *context, UErrorCode &ec);

    /**
     * ICU "poor man's RTTI", returns a UClassID for this class.
     */
    अटल UClassID U_EXPORT2 getStaticClassID();

    /**
     * ICU "poor man's RTTI", returns a UClassID for the actual class.
     */
    भव UClassID getDynamicClassID() स्थिर override;

निजी:
    int32_t सूचकांक_Of(UElement कुंजी, int32_t start_सूचकांक = 0, int8_t hint = 0) स्थिर;

    व्योम sorted_जोड़ो(UElement e, UElementComparator *compare, UErrorCode& ec);

खुला:
    // Disallow
    UVector(स्थिर UVector&) = delete;

    // Disallow
    UVector& चालक=(स्थिर UVector&) = delete;

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
class U_COMMON_API UStack : खुला UVector {
खुला:
    UStack(UErrorCode &status);

    UStack(int32_t initialCapacity, UErrorCode &status);

    UStack(UObjectDeleter *d, UElementsAreEqual *c, UErrorCode &status);

    UStack(UObjectDeleter *d, UElementsAreEqual *c, int32_t initialCapacity, UErrorCode &status);

    भव ~UStack();

    // It's okay not to have a virtual destructor (in UVector)
    // because UStack has no special cleanup to do.

    अंतरभूत UBool empty() स्थिर {वापस isEmpty();}

    अंतरभूत व्योम* peek() स्थिर {वापस lastElement();}

    अंतरभूत int32_t peeki() स्थिर {वापस lastElementi();}
    
    /**
     * Pop and return an element from the stack.
     * For stacks with a deleter function, the caller takes ownership
     * of the popped element.
     */
    व्योम* निकालो();
    
    int32_t popi();
    
    अंतरभूत व्योम* धकेलो(व्योम* obj, UErrorCode &status) {
        अगर (hasDeleter()) {
            adoptElement(obj, status);
            वापस (U_SUCCESS(status)) ? obj : nullptr;
        } अन्यथा {
            addElement(obj, status);
            वापस obj;
        }
    }

    अंतरभूत int32_t धकेलो(int32_t i, UErrorCode &status) {
        addElement(i, status);
        वापस i;
    }

    /*
    If the object o occurs as an item in this stack,
    this method returns the 1-based distance from the top of the stack.
    */
    int32_t खोज(व्योम* obj) स्थिर;

    /**
     * ICU "poor man's RTTI", returns a UClassID for this class.
     */
    अटल UClassID U_EXPORT2 getStaticClassID();

    /**
     * ICU "poor man's RTTI", returns a UClassID for the actual class.
     */
    भव UClassID getDynamicClassID() स्थिर override;

    // Disallow
    UStack(स्थिर UStack&) = delete;

    // Disallow
    UStack& चालक=(स्थिर UStack&) = delete;
};

U_NAMESPACE_END

#पूर्ण_अगर
