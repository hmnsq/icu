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

#నిర్వచితం_కాకపోతే UVECTOR_H
#నిర్వచించు UVECTOR_H

#అన్తర్భావ "unicode/utypes.h"
#అన్తర్భావ "unicode/uobject.h"
#అన్తర్భావ "cmemory.h"
#అన్తర్భావ "uarrsort.h"
#అన్తర్భావ "uelement.h"

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
class U_COMMON_API UVector : బహిరంగ UObject {
    // NOTE: UVector uses the UElement (union of void* and int32_t) as
    // its basic storage type.  It uses UElementsAreEqual as its
    // comparison function.  It uses UObjectDeleter as its deleter
    // function.  This allows sharing of support functions with UHashtable.

అంతరంగ:
    int32_t లెక్క = 0;

    int32_t సామర్థ్యం = 0;

    UElement* elements = nullptr;

    UObjectDeleter *deleter = nullptr;

    UElementsAreEqual *comparer = nullptr;

బహిరంగ:
    UVector(UError_సంకేతం &స్థితి_2);

    UVector(int32_t initial_సామర్థ్యం, UError_సంకేతం &స్థితి_2);

    UVector(UObjectDeleter *d, UElementsAreEqual *c, UError_సంకేతం &స్థితి_2);

    UVector(UObjectDeleter *d, UElementsAreEqual *c, int32_t initial_సామర్థ్యం, UError_సంకేతం &స్థితి_2);

    మాయా ~UVector();

    /**
     * Assign this object to another (make this a copy of 'other').
     * Use the 'assign' function to assign each element.
     */
    శూన్య కేటాయించు(స్థిరరాశి UVector& other, UElementAssigner *కేటాయించు, UError_సంకేతం &ec);

    /**
     * Compare this vector with another.  They will be considered
     * equal if they are of the same size and all elements are equal,
     * as compared using this object's comparer.
     */
    bool క్రియాకారకం==(స్థిరరాశి UVector& other) స్థిరరాశి;

    /**
     * Equivalent to !operator==()
     */
    అంతఃవరుస bool క్రియాకారకం!=(స్థిరరాశి UVector& other) స్థిరరాశి {తిరిగి !క్రియాకారకం==(other);}

    //------------------------------------------------------------
    // java.util.Vector API
    //------------------------------------------------------------

    /**
     * Add an element at the end of the vector.
     * For use only with vectors that do not adopt their elements, which is to say,
     * have not set an element deleter function. See `adoptElement()`.
     */
    శూన్య కూడు_Element(శూన్య *obj, UError_సంకేతం &స్థితి_2);

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
    శూన్య adoptElement(శూన్య *obj, UError_సంకేతం &స్థితి_2);

    శూన్య కూడు_Element(int32_t elem, UError_సంకేతం &స్థితి_2);

    శూన్య సమితి_Element_At(శూన్య* obj, int32_t సూచీ);

    శూన్య సమితి_Element_At(int32_t elem, int32_t సూచీ);

    శూన్య చొప్పించు_Element_At(శూన్య* obj, int32_t సూచీ, UError_సంకేతం &స్థితి_2);

    శూన్య చొప్పించు_Element_At(int32_t elem, int32_t సూచీ, UError_సంకేతం &స్థితి_2);
    
    శూన్య* elementAt(int32_t సూచీ) స్థిరరాశి;

    int32_t elementAti(int32_t సూచీ) స్థిరరాశి;

    UBool equals(స్థిరరాశి UVector &other) స్థిరరాశి;

    అంతఃవరుస శూన్య* మొదటి_Element() స్థిరరాశి {తిరిగి elementAt(0);}

    అంతఃవరుస శూన్య* చివరి_Element() స్థిరరాశి {తిరిగి elementAt(లెక్క-1);}

    అంతఃవరుస int32_t చివరి_Elementi() స్థిరరాశి {తిరిగి elementAti(లెక్క-1);}

    int32_t సూచీ_యొక్క(శూన్య* obj, int32_t ప్రారంభం_సూచీ = 0) స్థిరరాశి;

    int32_t సూచీ_యొక్క(int32_t obj, int32_t ప్రారంభం_సూచీ = 0) స్థిరరాశి;

    అంతఃవరుస UBool కలిగియుంది(శూన్య* obj) స్థిరరాశి {తిరిగి సూచీ_యొక్క(obj) >= 0;}

    అంతఃవరుస UBool కలిగియుంది(int32_t obj) స్థిరరాశి {తిరిగి సూచీ_యొక్క(obj) >= 0;}

    UBool కలిగియుంది_All(స్థిరరాశి UVector& other) స్థిరరాశి;

    UBool removeAll(స్థిరరాశి UVector& other);

    UBool retainAll(స్థిరరాశి UVector& other);

    శూన్య removeElementAt(int32_t సూచీ);

    UBool removeElement(శూన్య* obj);

    శూన్య removeAllElements();

    అంతఃవరుస int32_t కొలత_2() స్థిరరాశి {తిరిగి లెక్క;}

    అంతఃవరుస UBool is_ఖాళీలు() స్థిరరాశి {తిరిగి లెక్క == 0;}

    UBool ensure_సామర్థ్యం(int32_t minimum_సామర్థ్యం, UError_సంకేతం &స్థితి_2);

    /**
     * Change the size of this vector as follows: If newSize is
     * smaller, then truncate the array, possibly deleting held
     * elements for i >= newSize.  If newSize is larger, grow the
     * array, filling in new slots with nullptr.
     */
    శూన్య సమితి_కొలత(int32_t new_కొలత, UError_సంకేతం &స్థితి_2);

    /**
     * Fill in the given array with all elements of this vector.
     */
    శూన్య** గమ్యం_Array(శూన్య** ఫలితం) స్థిరరాశి;

    //------------------------------------------------------------
    // New API
    //------------------------------------------------------------

    UObjectDeleter *సమితి_Deleter(UObjectDeleter *d);
    bool hasDeleter() {తిరిగి deleter != nullptr;}

    UElementsAreEqual *సమితి_Comparer(UElementsAreEqual *c);

    అంతఃవరుస శూన్య* క్రియాకారకం[](int32_t సూచీ) స్థిరరాశి {తిరిగి elementAt(సూచీ);}

    /**
     * Removes the element at the given index from this vector and
     * transfer ownership of it to the caller.  After this call, the
     * caller owns the result and must delete it and the vector entry
     * at 'index' is removed, shifting all subsequent entries back by
     * one index and shortening the size of the vector by one.  If the
     * index is out of range or if there is no item at the given index
     * then 0 is returned and the vector is unchanged.
     */
    శూన్య* orphanElementAt(int32_t సూచీ);

    /**
     * Returns true if this vector contains none of the elements
     * of the given vector.
     * @param other vector to be checked for containment
     * @return true if the test condition is met
     */
    UBool కలిగియుంది_None(స్థిరరాశి UVector& other) స్థిరరాశి;

    /**
     * Insert the given object into this vector at its sorted position
     * as defined by 'compare'.  The current elements are assumed to
     * be sorted already.
     */
    శూన్య sorted_చొప్పించు(శూన్య* obj, UElementComparator *compare, UError_సంకేతం& ec);

    /**
     * Insert the given integer into this vector at its sorted position
     * as defined by 'compare'.  The current elements are assumed to
     * be sorted already.
     */
    శూన్య sorted_చొప్పించు(int32_t obj, UElementComparator *compare, UError_సంకేతం& ec);

    /**
     * Sort the contents of the vector, assuming that the contents of the
     * vector are of type int32_t.
     */
    శూన్య sorti(UError_సంకేతం &ec);

    /**
      * Sort the contents of this vector, using a caller-supplied function
      * to do the comparisons.  (It's confusing that
      *  UVector's UElementComparator function is different from the
      *  UComparator function type defined in uarrsort.h)
      */
    శూన్య క్రమీకరించు(UElementComparator *compare, UError_సంకేతం &ec);

    /**
     * Stable sort the contents of this vector using a caller-supplied function
     * of type UComparator to do the comparison.  Provides more flexibility
     * than UVector::sort() because an additional user parameter can be passed to
     * the comparison function.
     */
    శూన్య క్రమీకరించు_With_UComparator(UComparator *compare, స్థిరరాశి శూన్య *context, UError_సంకేతం &ec);

    /**
     * ICU "poor man's RTTI", returns a UClassID for this class.
     */
    స్థిర UClassID U_EXPORT2 getStaticClassID();

    /**
     * ICU "poor man's RTTI", returns a UClassID for the actual class.
     */
    మాయా UClassID getDynamicClassID() స్థిరరాశి override;

అంతరంగ:
    int32_t సూచీ_యొక్క(UElement కీలకం, int32_t ప్రారంభం_సూచీ = 0, int8_t hint = 0) స్థిరరాశి;

    శూన్య sorted_చొప్పించు(UElement e, UElementComparator *compare, UError_సంకేతం& ec);

బహిరంగ:
    // Disallow
    UVector(స్థిరరాశి UVector&) = delete;

    // Disallow
    UVector& క్రియాకారకం=(స్థిరరాశి UVector&) = delete;

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
class U_COMMON_API UStack : బహిరంగ UVector {
బహిరంగ:
    UStack(UError_సంకేతం &స్థితి_2);

    UStack(int32_t initial_సామర్థ్యం, UError_సంకేతం &స్థితి_2);

    UStack(UObjectDeleter *d, UElementsAreEqual *c, UError_సంకేతం &స్థితి_2);

    UStack(UObjectDeleter *d, UElementsAreEqual *c, int32_t initial_సామర్థ్యం, UError_సంకేతం &స్థితి_2);

    మాయా ~UStack();

    // It's okay not to have a virtual destructor (in UVector)
    // because UStack has no special cleanup to do.

    అంతఃవరుస UBool ఖాళీలు() స్థిరరాశి {తిరిగి is_ఖాళీలు();}

    అంతఃవరుస శూన్య* peek() స్థిరరాశి {తిరిగి చివరి_Element();}

    అంతఃవరుస int32_t peeki() స్థిరరాశి {తిరిగి చివరి_Elementi();}
    
    /**
     * Pop and return an element from the stack.
     * For stacks with a deleter function, the caller takes ownership
     * of the popped element.
     */
    శూన్య* తీయి();
    
    int32_t popi();
    
    అంతఃవరుస శూన్య* నెట్టు(శూన్య* obj, UError_సంకేతం &స్థితి_2) {
        ఒకవేళ (hasDeleter()) {
            adoptElement(obj, స్థితి_2);
            తిరిగి (U_SUCCESS(స్థితి_2)) ? obj : nullptr;
        } లేకపోతే {
            కూడు_Element(obj, స్థితి_2);
            తిరిగి obj;
        }
    }

    అంతఃవరుస int32_t నెట్టు(int32_t i, UError_సంకేతం &స్థితి_2) {
        కూడు_Element(i, స్థితి_2);
        తిరిగి i;
    }

    /*
    If the object o occurs as an item in this stack,
    this method returns the 1-based distance from the top of the stack.
    */
    int32_t శోధన(శూన్య* obj) స్థిరరాశి;

    /**
     * ICU "poor man's RTTI", returns a UClassID for this class.
     */
    స్థిర UClassID U_EXPORT2 getStaticClassID();

    /**
     * ICU "poor man's RTTI", returns a UClassID for the actual class.
     */
    మాయా UClassID getDynamicClassID() స్థిరరాశి override;

    // Disallow
    UStack(స్థిరరాశి UStack&) = delete;

    // Disallow
    UStack& క్రియాకారకం=(స్థిరరాశి UStack&) = delete;
};

U_NAMESPACE_చివర

#అయితే_ముగింపు
