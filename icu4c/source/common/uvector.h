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

#جيڪڏهن_وصف_نه_ٿيل UVECTOR_H
#وصف_ڪريو UVECTOR_H

#ضمن "unicode/utypes.h"
#ضمن "unicode/uobject.h"
#ضمن "cmemory.h"
#ضمن "uarrsort.h"
#ضمن "uelement.h"

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
class U_COMMON_API UVector : عام UObject {
    // NOTE: UVector uses the UElement (union of void* and int32_t) as
    // its basic storage type.  It uses UElementsAreEqual as its
    // comparison function.  It uses UObjectDeleter as its deleter
    // function.  This allows sharing of support functions with UHashtable.

خاص:
    int32_t ڳڻپ = 0;

    int32_t صلاحيت = 0;

    UElement* elements = nullptr;

    UObjectDeleter *deleter = nullptr;

    UElementsAreEqual *comparer = nullptr;

عام:
    UVector(UError_ڪوڊ &حالت);

    UVector(int32_t initial_صلاحيت, UError_ڪوڊ &حالت);

    UVector(UObjectDeleter *d, UElementsAreEqual *c, UError_ڪوڊ &حالت);

    UVector(UObjectDeleter *d, UElementsAreEqual *c, int32_t initial_صلاحيت, UError_ڪوڊ &حالت);

    مجازي ~UVector();

    /**
     * Assign this object to another (make this a copy of 'other').
     * Use the 'assign' function to assign each element.
     */
    فراغ سونپيو(ثابت UVector& other, UElementAssigner *سونپيو, UError_ڪوڊ &ec);

    /**
     * Compare this vector with another.  They will be considered
     * equal if they are of the same size and all elements are equal,
     * as compared using this object's comparer.
     */
    bool عامل==(ثابت UVector& other) ثابت;

    /**
     * Equivalent to !operator==()
     */
    مضمن bool عامل!=(ثابت UVector& other) ثابت {أرجع !عامل==(other);}

    //------------------------------------------------------------
    // java.util.Vector API
    //------------------------------------------------------------

    /**
     * Add an element at the end of the vector.
     * For use only with vectors that do not adopt their elements, which is to say,
     * have not set an element deleter function. See `adoptElement()`.
     */
    فراغ جوڙيو_Element(فراغ *obj, UError_ڪوڊ &حالت);

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
    فراغ adoptElement(فراغ *obj, UError_ڪوڊ &حالت);

    فراغ جوڙيو_Element(int32_t elem, UError_ڪوڊ &حالت);

    فراغ سيٽ_Element_At(فراغ* obj, int32_t اشاريو);

    فراغ سيٽ_Element_At(int32_t elem, int32_t اشاريو);

    فراغ وجهو_Element_At(فراغ* obj, int32_t اشاريو, UError_ڪوڊ &حالت);

    فراغ وجهو_Element_At(int32_t elem, int32_t اشاريو, UError_ڪوڊ &حالت);
    
    فراغ* elementAt(int32_t اشاريو) ثابت;

    int32_t elementAti(int32_t اشاريو) ثابت;

    UBool equals(ثابت UVector &other) ثابت;

    مضمن فراغ* پهريون_Element() ثابت {أرجع elementAt(0);}

    مضمن فراغ* آخري_Element() ثابت {أرجع elementAt(ڳڻپ-1);}

    مضمن int32_t آخري_Elementi() ثابت {أرجع elementAti(ڳڻپ-1);}

    int32_t اشاريو_جو(فراغ* obj, int32_t شروعات_اشاريو = 0) ثابت;

    int32_t اشاريو_جو(int32_t obj, int32_t شروعات_اشاريو = 0) ثابت;

    مضمن UBool شامل_آهي(فراغ* obj) ثابت {أرجع اشاريو_جو(obj) >= 0;}

    مضمن UBool شامل_آهي(int32_t obj) ثابت {أرجع اشاريو_جو(obj) >= 0;}

    UBool شامل_آهي_All(ثابت UVector& other) ثابت;

    UBool removeAll(ثابت UVector& other);

    UBool retainAll(ثابت UVector& other);

    فراغ removeElementAt(int32_t اشاريو);

    UBool removeElement(فراغ* obj);

    فراغ removeAllElements();

    مضمن int32_t سائيز_2() ثابت {أرجع ڳڻپ;}

    مضمن UBool is_خالي_جايون() ثابت {أرجع ڳڻپ == 0;}

    UBool ensure_صلاحيت(int32_t minimum_صلاحيت, UError_ڪوڊ &حالت);

    /**
     * Change the size of this vector as follows: If newSize is
     * smaller, then truncate the array, possibly deleting held
     * elements for i >= newSize.  If newSize is larger, grow the
     * array, filling in new slots with nullptr.
     */
    فراغ سيٽ_سائيز(int32_t new_سائيز, UError_ڪوڊ &حالت);

    /**
     * Fill in the given array with all elements of this vector.
     */
    فراغ** منزل_Array(فراغ** نتيجو) ثابت;

    //------------------------------------------------------------
    // New API
    //------------------------------------------------------------

    UObjectDeleter *سيٽ_Deleter(UObjectDeleter *d);
    bool hasDeleter() {أرجع deleter != nullptr;}

    UElementsAreEqual *سيٽ_Comparer(UElementsAreEqual *c);

    مضمن فراغ* عامل[](int32_t اشاريو) ثابت {أرجع elementAt(اشاريو);}

    /**
     * Removes the element at the given index from this vector and
     * transfer ownership of it to the caller.  After this call, the
     * caller owns the result and must delete it and the vector entry
     * at 'index' is removed, shifting all subsequent entries back by
     * one index and shortening the size of the vector by one.  If the
     * index is out of range or if there is no item at the given index
     * then 0 is returned and the vector is unchanged.
     */
    فراغ* orphanElementAt(int32_t اشاريو);

    /**
     * Returns true if this vector contains none of the elements
     * of the given vector.
     * @param other vector to be checked for containment
     * @return true if the test condition is met
     */
    UBool شامل_آهي_None(ثابت UVector& other) ثابت;

    /**
     * Insert the given object into this vector at its sorted position
     * as defined by 'compare'.  The current elements are assumed to
     * be sorted already.
     */
    فراغ sorted_وجهو(فراغ* obj, UElementComparator *compare, UError_ڪوڊ& ec);

    /**
     * Insert the given integer into this vector at its sorted position
     * as defined by 'compare'.  The current elements are assumed to
     * be sorted already.
     */
    فراغ sorted_وجهو(int32_t obj, UElementComparator *compare, UError_ڪوڊ& ec);

    /**
     * Sort the contents of the vector, assuming that the contents of the
     * vector are of type int32_t.
     */
    فراغ sorti(UError_ڪوڊ &ec);

    /**
      * Sort the contents of this vector, using a caller-supplied function
      * to do the comparisons.  (It's confusing that
      *  UVector's UElementComparator function is different from the
      *  UComparator function type defined in uarrsort.h)
      */
    فراغ ترتيب_ڏيو(UElementComparator *compare, UError_ڪوڊ &ec);

    /**
     * Stable sort the contents of this vector using a caller-supplied function
     * of type UComparator to do the comparison.  Provides more flexibility
     * than UVector::sort() because an additional user parameter can be passed to
     * the comparison function.
     */
    فراغ ترتيب_ڏيو_With_UComparator(UComparator *compare, ثابت فراغ *context, UError_ڪوڊ &ec);

    /**
     * ICU "poor man's RTTI", returns a UClassID for this class.
     */
    ساكن UClassID U_EXPORT2 getStaticClassID();

    /**
     * ICU "poor man's RTTI", returns a UClassID for the actual class.
     */
    مجازي UClassID getDynamicClassID() ثابت override;

خاص:
    int32_t اشاريو_جو(UElement ڪنجي, int32_t شروعات_اشاريو = 0, int8_t hint = 0) ثابت;

    فراغ sorted_وجهو(UElement e, UElementComparator *compare, UError_ڪوڊ& ec);

عام:
    // Disallow
    UVector(ثابت UVector&) = delete;

    // Disallow
    UVector& عامل=(ثابت UVector&) = delete;

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
class U_COMMON_API UStack : عام UVector {
عام:
    UStack(UError_ڪوڊ &حالت);

    UStack(int32_t initial_صلاحيت, UError_ڪوڊ &حالت);

    UStack(UObjectDeleter *d, UElementsAreEqual *c, UError_ڪوڊ &حالت);

    UStack(UObjectDeleter *d, UElementsAreEqual *c, int32_t initial_صلاحيت, UError_ڪوڊ &حالت);

    مجازي ~UStack();

    // It's okay not to have a virtual destructor (in UVector)
    // because UStack has no special cleanup to do.

    مضمن UBool خالي_جايون() ثابت {أرجع is_خالي_جايون();}

    مضمن فراغ* peek() ثابت {أرجع آخري_Element();}

    مضمن int32_t peeki() ثابت {أرجع آخري_Elementi();}
    
    /**
     * Pop and return an element from the stack.
     * For stacks with a deleter function, the caller takes ownership
     * of the popped element.
     */
    فراغ* ڪڍو_2();
    
    int32_t popi();
    
    مضمن فراغ* ڌڪو(فراغ* obj, UError_ڪوڊ &حالت) {
        إذا (hasDeleter()) {
            adoptElement(obj, حالت);
            أرجع (U_SUCCESS(حالت)) ? obj : nullptr;
        } وإلا {
            جوڙيو_Element(obj, حالت);
            أرجع obj;
        }
    }

    مضمن int32_t ڌڪو(int32_t i, UError_ڪوڊ &حالت) {
        جوڙيو_Element(i, حالت);
        أرجع i;
    }

    /*
    If the object o occurs as an item in this stack,
    this method returns the 1-based distance from the top of the stack.
    */
    int32_t ڳولا(فراغ* obj) ثابت;

    /**
     * ICU "poor man's RTTI", returns a UClassID for this class.
     */
    ساكن UClassID U_EXPORT2 getStaticClassID();

    /**
     * ICU "poor man's RTTI", returns a UClassID for the actual class.
     */
    مجازي UClassID getDynamicClassID() ثابت override;

    // Disallow
    UStack(ثابت UStack&) = delete;

    // Disallow
    UStack& عامل=(ثابت UStack&) = delete;
};

U_NAMESPACE_پڇاڙي

#جيڪڏهن_پورو
