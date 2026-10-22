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

#اگر_تعریف_نشده UVECTOR_H
#تعریف_کن UVECTOR_H

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
    int32_t شمار = 0;

    int32_t ظرفیت = 0;

    UElement* elements = nullptr;

    UObjectDeleter *deleter = nullptr;

    UElementsAreEqual *comparer = nullptr;

عام:
    UVector(UError_کد &وضعیت);

    UVector(int32_t initial_ظرفیت, UError_کد &وضعیت);

    UVector(UObjectDeleter *d, UElementsAreEqual *c, UError_کد &وضعیت);

    UVector(UObjectDeleter *d, UElementsAreEqual *c, int32_t initial_ظرفیت, UError_کد &وضعیت);

    مجازی ~UVector();

    /**
     * Assign this object to another (make this a copy of 'other').
     * Use the 'assign' function to assign each element.
     */
    فراغ تخصیص_بده(ثابت UVector& other, UElementAssigner *تخصیص_بده, UError_کد &ec);

    /**
     * Compare this vector with another.  They will be considered
     * equal if they are of the same size and all elements are equal,
     * as compared using this object's comparer.
     */
    bool عملگر==(ثابت UVector& other) ثابت;

    /**
     * Equivalent to !operator==()
     */
    مضمن bool عملگر!=(ثابت UVector& other) ثابت {أرجع !عملگر==(other);}

    //------------------------------------------------------------
    // java.util.Vector API
    //------------------------------------------------------------

    /**
     * Add an element at the end of the vector.
     * For use only with vectors that do not adopt their elements, which is to say,
     * have not set an element deleter function. See `adoptElement()`.
     */
    فراغ جمع_کن_Element(فراغ *obj, UError_کد &وضعیت);

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
    فراغ adoptElement(فراغ *obj, UError_کد &وضعیت);

    فراغ جمع_کن_Element(int32_t elem, UError_کد &وضعیت);

    فراغ مجموعه_Element_At(فراغ* obj, int32_t شاخص);

    فراغ مجموعه_Element_At(int32_t elem, int32_t شاخص);

    فراغ درج_Element_At(فراغ* obj, int32_t شاخص, UError_کد &وضعیت);

    فراغ درج_Element_At(int32_t elem, int32_t شاخص, UError_کد &وضعیت);
    
    فراغ* elementAt(int32_t شاخص) ثابت;

    int32_t elementAti(int32_t شاخص) ثابت;

    UBool equals(ثابت UVector &other) ثابت;

    مضمن فراغ* اولی_Element() ثابت {أرجع elementAt(0);}

    مضمن فراغ* آخرین_Element() ثابت {أرجع elementAt(شمار-1);}

    مضمن int32_t آخرین_Elementi() ثابت {أرجع elementAti(شمار-1);}

    int32_t شاخص_از(فراغ* obj, int32_t شروع_شاخص = 0) ثابت;

    int32_t شاخص_از(int32_t obj, int32_t شروع_شاخص = 0) ثابت;

    مضمن UBool شامل_است(فراغ* obj) ثابت {أرجع شاخص_از(obj) >= 0;}

    مضمن UBool شامل_است(int32_t obj) ثابت {أرجع شاخص_از(obj) >= 0;}

    UBool شامل_است_All(ثابت UVector& other) ثابت;

    UBool removeAll(ثابت UVector& other);

    UBool retainAll(ثابت UVector& other);

    فراغ removeElementAt(int32_t شاخص);

    UBool removeElement(فراغ* obj);

    فراغ removeAllElements();

    مضمن int32_t اندازه_2() ثابت {أرجع شمار;}

    مضمن UBool is_خالی() ثابت {أرجع شمار == 0;}

    UBool ensure_ظرفیت(int32_t minimum_ظرفیت, UError_کد &وضعیت);

    /**
     * Change the size of this vector as follows: If newSize is
     * smaller, then truncate the array, possibly deleting held
     * elements for i >= newSize.  If newSize is larger, grow the
     * array, filling in new slots with nullptr.
     */
    فراغ مجموعه_اندازه(int32_t new_اندازه, UError_کد &وضعیت);

    /**
     * Fill in the given array with all elements of this vector.
     */
    فراغ** مقصد_Array(فراغ** نتیجه) ثابت;

    //------------------------------------------------------------
    // New API
    //------------------------------------------------------------

    UObjectDeleter *مجموعه_Deleter(UObjectDeleter *d);
    bool hasDeleter() {أرجع deleter != nullptr;}

    UElementsAreEqual *مجموعه_Comparer(UElementsAreEqual *c);

    مضمن فراغ* عملگر[](int32_t شاخص) ثابت {أرجع elementAt(شاخص);}

    /**
     * Removes the element at the given index from this vector and
     * transfer ownership of it to the caller.  After this call, the
     * caller owns the result and must delete it and the vector entry
     * at 'index' is removed, shifting all subsequent entries back by
     * one index and shortening the size of the vector by one.  If the
     * index is out of range or if there is no item at the given index
     * then 0 is returned and the vector is unchanged.
     */
    فراغ* orphanElementAt(int32_t شاخص);

    /**
     * Returns true if this vector contains none of the elements
     * of the given vector.
     * @param other vector to be checked for containment
     * @return true if the test condition is met
     */
    UBool شامل_است_None(ثابت UVector& other) ثابت;

    /**
     * Insert the given object into this vector at its sorted position
     * as defined by 'compare'.  The current elements are assumed to
     * be sorted already.
     */
    فراغ sorted_درج(فراغ* obj, UElementComparator *compare, UError_کد& ec);

    /**
     * Insert the given integer into this vector at its sorted position
     * as defined by 'compare'.  The current elements are assumed to
     * be sorted already.
     */
    فراغ sorted_درج(int32_t obj, UElementComparator *compare, UError_کد& ec);

    /**
     * Sort the contents of the vector, assuming that the contents of the
     * vector are of type int32_t.
     */
    فراغ sorti(UError_کد &ec);

    /**
      * Sort the contents of this vector, using a caller-supplied function
      * to do the comparisons.  (It's confusing that
      *  UVector's UElementComparator function is different from the
      *  UComparator function type defined in uarrsort.h)
      */
    فراغ مرتب_سازی(UElementComparator *compare, UError_کد &ec);

    /**
     * Stable sort the contents of this vector using a caller-supplied function
     * of type UComparator to do the comparison.  Provides more flexibility
     * than UVector::sort() because an additional user parameter can be passed to
     * the comparison function.
     */
    فراغ مرتب_سازی_With_UComparator(UComparator *compare, ثابت فراغ *context, UError_کد &ec);

    /**
     * ICU "poor man's RTTI", returns a UClassID for this class.
     */
    ساكن UClassID U_EXPORT2 getStaticClassID();

    /**
     * ICU "poor man's RTTI", returns a UClassID for the actual class.
     */
    مجازی UClassID getDynamicClassID() ثابت override;

خاص:
    int32_t شاخص_از(UElement کلید, int32_t شروع_شاخص = 0, int8_t hint = 0) ثابت;

    فراغ sorted_درج(UElement e, UElementComparator *compare, UError_کد& ec);

عام:
    // Disallow
    UVector(ثابت UVector&) = delete;

    // Disallow
    UVector& عملگر=(ثابت UVector&) = delete;

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
    UStack(UError_کد &وضعیت);

    UStack(int32_t initial_ظرفیت, UError_کد &وضعیت);

    UStack(UObjectDeleter *d, UElementsAreEqual *c, UError_کد &وضعیت);

    UStack(UObjectDeleter *d, UElementsAreEqual *c, int32_t initial_ظرفیت, UError_کد &وضعیت);

    مجازی ~UStack();

    // It's okay not to have a virtual destructor (in UVector)
    // because UStack has no special cleanup to do.

    مضمن UBool خالی() ثابت {أرجع is_خالی();}

    مضمن فراغ* peek() ثابت {أرجع آخرین_Element();}

    مضمن int32_t peeki() ثابت {أرجع آخرین_Elementi();}
    
    /**
     * Pop and return an element from the stack.
     * For stacks with a deleter function, the caller takes ownership
     * of the popped element.
     */
    فراغ* برداشت();
    
    int32_t popi();
    
    مضمن فراغ* افزودن(فراغ* obj, UError_کد &وضعیت) {
        إذا (hasDeleter()) {
            adoptElement(obj, وضعیت);
            أرجع (U_SUCCESS(وضعیت)) ? obj : nullptr;
        } وإلا {
            جمع_کن_Element(obj, وضعیت);
            أرجع obj;
        }
    }

    مضمن int32_t افزودن(int32_t i, UError_کد &وضعیت) {
        جمع_کن_Element(i, وضعیت);
        أرجع i;
    }

    /*
    If the object o occurs as an item in this stack,
    this method returns the 1-based distance from the top of the stack.
    */
    int32_t جستجو(فراغ* obj) ثابت;

    /**
     * ICU "poor man's RTTI", returns a UClassID for this class.
     */
    ساكن UClassID U_EXPORT2 getStaticClassID();

    /**
     * ICU "poor man's RTTI", returns a UClassID for the actual class.
     */
    مجازی UClassID getDynamicClassID() ثابت override;

    // Disallow
    UStack(ثابت UStack&) = delete;

    // Disallow
    UStack& عملگر=(ثابت UStack&) = delete;
};

U_NAMESPACE_انتها

#پایان_اگر
