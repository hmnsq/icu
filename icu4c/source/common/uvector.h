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

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ UVECTOR_H
#ವ್ಯಾಖ್ಯಾನಿಸು UVECTOR_H

#ಅನ್ತರ್ಭಾವ "unicode/utypes.h"
#ಅನ್ತರ್ಭಾವ "unicode/uobject.h"
#ಅನ್ತರ್ಭಾವ "cmemory.h"
#ಅನ್ತರ್ಭಾವ "uarrsort.h"
#ಅನ್ತರ್ಭಾವ "uelement.h"

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
class U_COMMON_API UVector : ಸಾರ್ವಜನಿಕ UObject {
    // NOTE: UVector uses the UElement (union of void* and int32_t) as
    // its basic storage type.  It uses UElementsAreEqual as its
    // comparison function.  It uses UObjectDeleter as its deleter
    // function.  This allows sharing of support functions with UHashtable.

ಖಾಸಗಿ:
    int32_t ಎಣಿಕೆ_2 = 0;

    int32_t ಸಾಮರ್ಥ್ಯ = 0;

    UElement* elements = nullptr;

    UObjectDeleter *deleter = nullptr;

    UElementsAreEqual *comparer = nullptr;

ಸಾರ್ವಜನಿಕ:
    UVector(UError_ಸಂಕೇತ &ಸ್ಥಿತಿ_2);

    UVector(int32_t initial_ಸಾಮರ್ಥ್ಯ, UError_ಸಂಕೇತ &ಸ್ಥಿತಿ_2);

    UVector(UObjectDeleter *d, UElementsAreEqual *c, UError_ಸಂಕೇತ &ಸ್ಥಿತಿ_2);

    UVector(UObjectDeleter *d, UElementsAreEqual *c, int32_t initial_ಸಾಮರ್ಥ್ಯ, UError_ಸಂಕೇತ &ಸ್ಥಿತಿ_2);

    ವಾಸ್ತವಿಕ ~UVector();

    /**
     * Assign this object to another (make this a copy of 'other').
     * Use the 'assign' function to assign each element.
     */
    ಶೂನ್ಯ ನಿಯೋಜಿಸು(ಸ್ಥಿರಾಂಕ UVector& other, UElementAssigner *ನಿಯೋಜಿಸು, UError_ಸಂಕೇತ &ec);

    /**
     * Compare this vector with another.  They will be considered
     * equal if they are of the same size and all elements are equal,
     * as compared using this object's comparer.
     */
    bool ಕ್ರಿಯಾಕಾರಕ==(ಸ್ಥಿರಾಂಕ UVector& other) ಸ್ಥಿರಾಂಕ;

    /**
     * Equivalent to !operator==()
     */
    ಸಾಲೊಳಗೆ bool ಕ್ರಿಯಾಕಾರಕ!=(ಸ್ಥಿರಾಂಕ UVector& other) ಸ್ಥಿರಾಂಕ {ಹಿಂತಿರುಗಿ !ಕ್ರಿಯಾಕಾರಕ==(other);}

    //------------------------------------------------------------
    // java.util.Vector API
    //------------------------------------------------------------

    /**
     * Add an element at the end of the vector.
     * For use only with vectors that do not adopt their elements, which is to say,
     * have not set an element deleter function. See `adoptElement()`.
     */
    ಶೂನ್ಯ ಕೂಡಿಸು_Element(ಶೂನ್ಯ *obj, UError_ಸಂಕೇತ &ಸ್ಥಿತಿ_2);

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
    ಶೂನ್ಯ adoptElement(ಶೂನ್ಯ *obj, UError_ಸಂಕೇತ &ಸ್ಥಿತಿ_2);

    ಶೂನ್ಯ ಕೂಡಿಸು_Element(int32_t elem, UError_ಸಂಕೇತ &ಸ್ಥಿತಿ_2);

    ಶೂನ್ಯ ಗಣ_Element_At(ಶೂನ್ಯ* obj, int32_t ಸೂಚ್ಯಂಕ);

    ಶೂನ್ಯ ಗಣ_Element_At(int32_t elem, int32_t ಸೂಚ್ಯಂಕ);

    ಶೂನ್ಯ ಸೇರಿಸು_Element_At(ಶೂನ್ಯ* obj, int32_t ಸೂಚ್ಯಂಕ, UError_ಸಂಕೇತ &ಸ್ಥಿತಿ_2);

    ಶೂನ್ಯ ಸೇರಿಸು_Element_At(int32_t elem, int32_t ಸೂಚ್ಯಂಕ, UError_ಸಂಕೇತ &ಸ್ಥಿತಿ_2);
    
    ಶೂನ್ಯ* elementAt(int32_t ಸೂಚ್ಯಂಕ) ಸ್ಥಿರಾಂಕ;

    int32_t elementAti(int32_t ಸೂಚ್ಯಂಕ) ಸ್ಥಿರಾಂಕ;

    UBool equals(ಸ್ಥಿರಾಂಕ UVector &other) ಸ್ಥಿರಾಂಕ;

    ಸಾಲೊಳಗೆ ಶೂನ್ಯ* ಮೊದಲ_Element() ಸ್ಥಿರಾಂಕ {ಹಿಂತಿರುಗಿ elementAt(0);}

    ಸಾಲೊಳಗೆ ಶೂನ್ಯ* ಕೊನೆಯ_Element() ಸ್ಥಿರಾಂಕ {ಹಿಂತಿರುಗಿ elementAt(ಎಣಿಕೆ_2-1);}

    ಸಾಲೊಳಗೆ int32_t ಕೊನೆಯ_Elementi() ಸ್ಥಿರಾಂಕ {ಹಿಂತಿರುಗಿ elementAti(ಎಣಿಕೆ_2-1);}

    int32_t ಸೂಚ್ಯಂಕ_ದ(ಶೂನ್ಯ* obj, int32_t ಆರಂಭ_ಸೂಚ್ಯಂಕ = 0) ಸ್ಥಿರಾಂಕ;

    int32_t ಸೂಚ್ಯಂಕ_ದ(int32_t obj, int32_t ಆರಂಭ_ಸೂಚ್ಯಂಕ = 0) ಸ್ಥಿರಾಂಕ;

    ಸಾಲೊಳಗೆ UBool ಒಳಗೊಂಡಿದೆ(ಶೂನ್ಯ* obj) ಸ್ಥಿರಾಂಕ {ಹಿಂತಿರುಗಿ ಸೂಚ್ಯಂಕ_ದ(obj) >= 0;}

    ಸಾಲೊಳಗೆ UBool ಒಳಗೊಂಡಿದೆ(int32_t obj) ಸ್ಥಿರಾಂಕ {ಹಿಂತಿರುಗಿ ಸೂಚ್ಯಂಕ_ದ(obj) >= 0;}

    UBool ಒಳಗೊಂಡಿದೆ_All(ಸ್ಥಿರಾಂಕ UVector& other) ಸ್ಥಿರಾಂಕ;

    UBool removeAll(ಸ್ಥಿರಾಂಕ UVector& other);

    UBool retainAll(ಸ್ಥಿರಾಂಕ UVector& other);

    ಶೂನ್ಯ removeElementAt(int32_t ಸೂಚ್ಯಂಕ);

    UBool removeElement(ಶೂನ್ಯ* obj);

    ಶೂನ್ಯ removeAllElements();

    ಸಾಲೊಳಗೆ int32_t ಗಾತ್ರ_3() ಸ್ಥಿರಾಂಕ {ಹಿಂತಿರುಗಿ ಎಣಿಕೆ_2;}

    ಸಾಲೊಳಗೆ UBool is_ಖಾಲಿಗಳು() ಸ್ಥಿರಾಂಕ {ಹಿಂತಿರುಗಿ ಎಣಿಕೆ_2 == 0;}

    UBool ensure_ಸಾಮರ್ಥ್ಯ(int32_t minimum_ಸಾಮರ್ಥ್ಯ, UError_ಸಂಕೇತ &ಸ್ಥಿತಿ_2);

    /**
     * Change the size of this vector as follows: If newSize is
     * smaller, then truncate the array, possibly deleting held
     * elements for i >= newSize.  If newSize is larger, grow the
     * array, filling in new slots with nullptr.
     */
    ಶೂನ್ಯ ಗಣ_ಗಾತ್ರ(int32_t new_ಗಾತ್ರ, UError_ಸಂಕೇತ &ಸ್ಥಿತಿ_2);

    /**
     * Fill in the given array with all elements of this vector.
     */
    ಶೂನ್ಯ** ಗಮ್ಯ_Array(ಶೂನ್ಯ** ಫಲಿತಾಂಶ) ಸ್ಥಿರಾಂಕ;

    //------------------------------------------------------------
    // New API
    //------------------------------------------------------------

    UObjectDeleter *ಗಣ_Deleter(UObjectDeleter *d);
    bool hasDeleter() {ಹಿಂತಿರುಗಿ deleter != nullptr;}

    UElementsAreEqual *ಗಣ_Comparer(UElementsAreEqual *c);

    ಸಾಲೊಳಗೆ ಶೂನ್ಯ* ಕ್ರಿಯಾಕಾರಕ[](int32_t ಸೂಚ್ಯಂಕ) ಸ್ಥಿರಾಂಕ {ಹಿಂತಿರುಗಿ elementAt(ಸೂಚ್ಯಂಕ);}

    /**
     * Removes the element at the given index from this vector and
     * transfer ownership of it to the caller.  After this call, the
     * caller owns the result and must delete it and the vector entry
     * at 'index' is removed, shifting all subsequent entries back by
     * one index and shortening the size of the vector by one.  If the
     * index is out of range or if there is no item at the given index
     * then 0 is returned and the vector is unchanged.
     */
    ಶೂನ್ಯ* orphanElementAt(int32_t ಸೂಚ್ಯಂಕ);

    /**
     * Returns true if this vector contains none of the elements
     * of the given vector.
     * @param other vector to be checked for containment
     * @return true if the test condition is met
     */
    UBool ಒಳಗೊಂಡಿದೆ_None(ಸ್ಥಿರಾಂಕ UVector& other) ಸ್ಥಿರಾಂಕ;

    /**
     * Insert the given object into this vector at its sorted position
     * as defined by 'compare'.  The current elements are assumed to
     * be sorted already.
     */
    ಶೂನ್ಯ sorted_ಸೇರಿಸು(ಶೂನ್ಯ* obj, UElementComparator *compare, UError_ಸಂಕೇತ& ec);

    /**
     * Insert the given integer into this vector at its sorted position
     * as defined by 'compare'.  The current elements are assumed to
     * be sorted already.
     */
    ಶೂನ್ಯ sorted_ಸೇರಿಸು(int32_t obj, UElementComparator *compare, UError_ಸಂಕೇತ& ec);

    /**
     * Sort the contents of the vector, assuming that the contents of the
     * vector are of type int32_t.
     */
    ಶೂನ್ಯ sorti(UError_ಸಂಕೇತ &ec);

    /**
      * Sort the contents of this vector, using a caller-supplied function
      * to do the comparisons.  (It's confusing that
      *  UVector's UElementComparator function is different from the
      *  UComparator function type defined in uarrsort.h)
      */
    ಶೂನ್ಯ ವಿಂಗಡಿಸು(UElementComparator *compare, UError_ಸಂಕೇತ &ec);

    /**
     * Stable sort the contents of this vector using a caller-supplied function
     * of type UComparator to do the comparison.  Provides more flexibility
     * than UVector::sort() because an additional user parameter can be passed to
     * the comparison function.
     */
    ಶೂನ್ಯ ವಿಂಗಡಿಸು_With_UComparator(UComparator *compare, ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *context, UError_ಸಂಕೇತ &ec);

    /**
     * ICU "poor man's RTTI", returns a UClassID for this class.
     */
    ಸ್ಥಿರ UClassID U_EXPORT2 getStaticClassID();

    /**
     * ICU "poor man's RTTI", returns a UClassID for the actual class.
     */
    ವಾಸ್ತವಿಕ UClassID getDynamicClassID() ಸ್ಥಿರಾಂಕ override;

ಖಾಸಗಿ:
    int32_t ಸೂಚ್ಯಂಕ_ದ(UElement ಕೀಲಿ, int32_t ಆರಂಭ_ಸೂಚ್ಯಂಕ = 0, int8_t hint = 0) ಸ್ಥಿರಾಂಕ;

    ಶೂನ್ಯ sorted_ಸೇರಿಸು(UElement e, UElementComparator *compare, UError_ಸಂಕೇತ& ec);

ಸಾರ್ವಜನಿಕ:
    // Disallow
    UVector(ಸ್ಥಿರಾಂಕ UVector&) = delete;

    // Disallow
    UVector& ಕ್ರಿಯಾಕಾರಕ=(ಸ್ಥಿರಾಂಕ UVector&) = delete;

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
class U_COMMON_API UStack : ಸಾರ್ವಜನಿಕ UVector {
ಸಾರ್ವಜನಿಕ:
    UStack(UError_ಸಂಕೇತ &ಸ್ಥಿತಿ_2);

    UStack(int32_t initial_ಸಾಮರ್ಥ್ಯ, UError_ಸಂಕೇತ &ಸ್ಥಿತಿ_2);

    UStack(UObjectDeleter *d, UElementsAreEqual *c, UError_ಸಂಕೇತ &ಸ್ಥಿತಿ_2);

    UStack(UObjectDeleter *d, UElementsAreEqual *c, int32_t initial_ಸಾಮರ್ಥ್ಯ, UError_ಸಂಕೇತ &ಸ್ಥಿತಿ_2);

    ವಾಸ್ತವಿಕ ~UStack();

    // It's okay not to have a virtual destructor (in UVector)
    // because UStack has no special cleanup to do.

    ಸಾಲೊಳಗೆ UBool ಖಾಲಿಗಳು() ಸ್ಥಿರಾಂಕ {ಹಿಂತಿರುಗಿ is_ಖಾಲಿಗಳು();}

    ಸಾಲೊಳಗೆ ಶೂನ್ಯ* peek() ಸ್ಥಿರಾಂಕ {ಹಿಂತಿರುಗಿ ಕೊನೆಯ_Element();}

    ಸಾಲೊಳಗೆ int32_t peeki() ಸ್ಥಿರಾಂಕ {ಹಿಂತಿರುಗಿ ಕೊನೆಯ_Elementi();}
    
    /**
     * Pop and return an element from the stack.
     * For stacks with a deleter function, the caller takes ownership
     * of the popped element.
     */
    ಶೂನ್ಯ* ತೆಗೆ();
    
    int32_t popi();
    
    ಸಾಲೊಳಗೆ ಶೂನ್ಯ* ತಳ್ಳು(ಶೂನ್ಯ* obj, UError_ಸಂಕೇತ &ಸ್ಥಿತಿ_2) {
        ಆದರೆ (hasDeleter()) {
            adoptElement(obj, ಸ್ಥಿತಿ_2);
            ಹಿಂತಿರುಗಿ (U_SUCCESS(ಸ್ಥಿತಿ_2)) ? obj : nullptr;
        } ಇಲ್ಲದಿದ್ದರೆ {
            ಕೂಡಿಸು_Element(obj, ಸ್ಥಿತಿ_2);
            ಹಿಂತಿರುಗಿ obj;
        }
    }

    ಸಾಲೊಳಗೆ int32_t ತಳ್ಳು(int32_t i, UError_ಸಂಕೇತ &ಸ್ಥಿತಿ_2) {
        ಕೂಡಿಸು_Element(i, ಸ್ಥಿತಿ_2);
        ಹಿಂತಿರುಗಿ i;
    }

    /*
    If the object o occurs as an item in this stack,
    this method returns the 1-based distance from the top of the stack.
    */
    int32_t ಹುಡುಕು(ಶೂನ್ಯ* obj) ಸ್ಥಿರಾಂಕ;

    /**
     * ICU "poor man's RTTI", returns a UClassID for this class.
     */
    ಸ್ಥಿರ UClassID U_EXPORT2 getStaticClassID();

    /**
     * ICU "poor man's RTTI", returns a UClassID for the actual class.
     */
    ವಾಸ್ತವಿಕ UClassID getDynamicClassID() ಸ್ಥಿರಾಂಕ override;

    // Disallow
    UStack(ಸ್ಥಿರಾಂಕ UStack&) = delete;

    // Disallow
    UStack& ಕ್ರಿಯಾಕಾರಕ=(ಸ್ಥಿರಾಂಕ UStack&) = delete;
};

U_NAMESPACE_ಕೊನೆ

#ಆದರೆ_ಅಂತ್ಯ
