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

#यदि_अपरिभाषितम् UVECTOR_H
#परिभाषय UVECTOR_H

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

गुप्त:
    int32_t गणनम् = 0;

    int32_t क्षमता = 0;

    UElement* elements = nullptr;

    UObjectDeleter *deleter = nullptr;

    UElementsAreEqual *comparer = nullptr;

सार्वजनिक:
    UVector(UError_संहिता &स्थितिः);

    UVector(int32_t initial_क्षमता, UError_संहिता &स्थितिः);

    UVector(UObjectDeleter *d, UElementsAreEqual *c, UError_संहिता &स्थितिः);

    UVector(UObjectDeleter *d, UElementsAreEqual *c, int32_t initial_क्षमता, UError_संहिता &स्थितिः);

    आभास ~UVector();

    /**
     * Assign this object to another (make this a copy of 'other').
     * Use the 'assign' function to assign each element.
     */
    शून्य नियोजय(ध्रुव UVector& other, UElementAssigner *नियोजय, UError_संहिता &ec);

    /**
     * Compare this vector with another.  They will be considered
     * equal if they are of the same size and all elements are equal,
     * as compared using this object's comparer.
     */
    bool संक्रियकः==(ध्रुव UVector& other) ध्रुव;

    /**
     * Equivalent to !operator==()
     */
    अन्तःस्थ bool संक्रियकः!=(ध्रुव UVector& other) ध्रुव {प्रत्यागम !संक्रियकः==(other);}

    //------------------------------------------------------------
    // java.util.Vector API
    //------------------------------------------------------------

    /**
     * Add an element at the end of the vector.
     * For use only with vectors that do not adopt their elements, which is to say,
     * have not set an element deleter function. See `adoptElement()`.
     */
    शून्य सङ्कलय_Element(शून्य *obj, UError_संहिता &स्थितिः);

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
    शून्य adoptElement(शून्य *obj, UError_संहिता &स्थितिः);

    शून्य सङ्कलय_Element(int32_t elem, UError_संहिता &स्थितिः);

    शून्य समुच्चयः_Element_At(शून्य* obj, int32_t सूचकांक);

    शून्य समुच्चयः_Element_At(int32_t elem, int32_t सूचकांक);

    शून्य निवेशय_Element_At(शून्य* obj, int32_t सूचकांक, UError_संहिता &स्थितिः);

    शून्य निवेशय_Element_At(int32_t elem, int32_t सूचकांक, UError_संहिता &स्थितिः);
    
    शून्य* elementAt(int32_t सूचकांक) ध्रुव;

    int32_t elementAti(int32_t सूचकांक) ध्रुव;

    UBool equals(ध्रुव UVector &other) ध्रुव;

    अन्तःस्थ शून्य* firstElement() ध्रुव {प्रत्यागम elementAt(0);}

    अन्तःस्थ शून्य* चरमः_Element() ध्रुव {प्रत्यागम elementAt(गणनम्-1);}

    अन्तःस्थ int32_t चरमः_Elementi() ध्रुव {प्रत्यागम elementAti(गणनम्-1);}

    int32_t सूचकांक_सम्बन्धि(शून्य* obj, int32_t start_सूचकांक = 0) ध्रुव;

    int32_t सूचकांक_सम्बन्धि(int32_t obj, int32_t start_सूचकांक = 0) ध्रुव;

    अन्तःस्थ UBool अन्तर्भवति(शून्य* obj) ध्रुव {प्रत्यागम सूचकांक_सम्बन्धि(obj) >= 0;}

    अन्तःस्थ UBool अन्तर्भवति(int32_t obj) ध्रुव {प्रत्यागम सूचकांक_सम्बन्धि(obj) >= 0;}

    UBool अन्तर्भवति_All(ध्रुव UVector& other) ध्रुव;

    UBool removeAll(ध्रुव UVector& other);

    UBool retainAll(ध्रुव UVector& other);

    शून्य removeElementAt(int32_t सूचकांक);

    UBool removeElement(शून्य* obj);

    शून्य removeAllElements();

    अन्तःस्थ int32_t आकार_2() ध्रुव {प्रत्यागम गणनम्;}

    अन्तःस्थ UBool is_रिक्तानि() ध्रुव {प्रत्यागम गणनम् == 0;}

    UBool ensure_क्षमता(int32_t minimum_क्षमता, UError_संहिता &स्थितिः);

    /**
     * Change the size of this vector as follows: If newSize is
     * smaller, then truncate the array, possibly deleting held
     * elements for i >= newSize.  If newSize is larger, grow the
     * array, filling in new slots with nullptr.
     */
    शून्य समुच्चयः_आकार(int32_t new_आकार, UError_संहिता &स्थितिः);

    /**
     * Fill in the given array with all elements of this vector.
     */
    शून्य** गन्तव्यम्_Array(शून्य** परिणाम) ध्रुव;

    //------------------------------------------------------------
    // New API
    //------------------------------------------------------------

    UObjectDeleter *समुच्चयः_Deleter(UObjectDeleter *d);
    bool hasDeleter() {प्रत्यागम deleter != nullptr;}

    UElementsAreEqual *समुच्चयः_Comparer(UElementsAreEqual *c);

    अन्तःस्थ शून्य* संक्रियकः[](int32_t सूचकांक) ध्रुव {प्रत्यागम elementAt(सूचकांक);}

    /**
     * Removes the element at the given index from this vector and
     * transfer ownership of it to the caller.  After this call, the
     * caller owns the result and must delete it and the vector entry
     * at 'index' is removed, shifting all subsequent entries back by
     * one index and shortening the size of the vector by one.  If the
     * index is out of range or if there is no item at the given index
     * then 0 is returned and the vector is unchanged.
     */
    शून्य* orphanElementAt(int32_t सूचकांक);

    /**
     * Returns true if this vector contains none of the elements
     * of the given vector.
     * @param other vector to be checked for containment
     * @return true if the test condition is met
     */
    UBool अन्तर्भवति_None(ध्रुव UVector& other) ध्रुव;

    /**
     * Insert the given object into this vector at its sorted position
     * as defined by 'compare'.  The current elements are assumed to
     * be sorted already.
     */
    शून्य sorted_निवेशय(शून्य* obj, UElementComparator *compare, UError_संहिता& ec);

    /**
     * Insert the given integer into this vector at its sorted position
     * as defined by 'compare'.  The current elements are assumed to
     * be sorted already.
     */
    शून्य sorted_निवेशय(int32_t obj, UElementComparator *compare, UError_संहिता& ec);

    /**
     * Sort the contents of the vector, assuming that the contents of the
     * vector are of type int32_t.
     */
    शून्य sorti(UError_संहिता &ec);

    /**
      * Sort the contents of this vector, using a caller-supplied function
      * to do the comparisons.  (It's confusing that
      *  UVector's UElementComparator function is different from the
      *  UComparator function type defined in uarrsort.h)
      */
    शून्य क्रमय(UElementComparator *compare, UError_संहिता &ec);

    /**
     * Stable sort the contents of this vector using a caller-supplied function
     * of type UComparator to do the comparison.  Provides more flexibility
     * than UVector::sort() because an additional user parameter can be passed to
     * the comparison function.
     */
    शून्य क्रमय_With_UComparator(UComparator *compare, ध्रुव शून्य *context, UError_संहिता &ec);

    /**
     * ICU "poor man's RTTI", returns a UClassID for this class.
     */
    स्थिर UClassID U_EXPORT2 getStaticClassID();

    /**
     * ICU "poor man's RTTI", returns a UClassID for the actual class.
     */
    आभास UClassID getDynamicClassID() ध्रुव override;

गुप्त:
    int32_t सूचकांक_सम्बन्धि(UElement कुञ्चिका, int32_t start_सूचकांक = 0, int8_t hint = 0) ध्रुव;

    शून्य sorted_निवेशय(UElement e, UElementComparator *compare, UError_संहिता& ec);

सार्वजनिक:
    // Disallow
    UVector(ध्रुव UVector&) = delete;

    // Disallow
    UVector& संक्रियकः=(ध्रुव UVector&) = delete;

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
    UStack(UError_संहिता &स्थितिः);

    UStack(int32_t initial_क्षमता, UError_संहिता &स्थितिः);

    UStack(UObjectDeleter *d, UElementsAreEqual *c, UError_संहिता &स्थितिः);

    UStack(UObjectDeleter *d, UElementsAreEqual *c, int32_t initial_क्षमता, UError_संहिता &स्थितिः);

    आभास ~UStack();

    // It's okay not to have a virtual destructor (in UVector)
    // because UStack has no special cleanup to do.

    अन्तःस्थ UBool रिक्तानि() ध्रुव {प्रत्यागम is_रिक्तानि();}

    अन्तःस्थ शून्य* peek() ध्रुव {प्रत्यागम चरमः_Element();}

    अन्तःस्थ int32_t peeki() ध्रुव {प्रत्यागम चरमः_Elementi();}
    
    /**
     * Pop and return an element from the stack.
     * For stacks with a deleter function, the caller takes ownership
     * of the popped element.
     */
    शून्य* निष्कासय();
    
    int32_t popi();
    
    अन्तःस्थ शून्य* प्रक्षिप(शून्य* obj, UError_संहिता &स्थितिः) {
        यदि (hasDeleter()) {
            adoptElement(obj, स्थितिः);
            प्रत्यागम (U_SUCCESS(स्थितिः)) ? obj : nullptr;
        } अन्यथा {
            सङ्कलय_Element(obj, स्थितिः);
            प्रत्यागम obj;
        }
    }

    अन्तःस्थ int32_t प्रक्षिप(int32_t i, UError_संहिता &स्थितिः) {
        सङ्कलय_Element(i, स्थितिः);
        प्रत्यागम i;
    }

    /*
    If the object o occurs as an item in this stack,
    this method returns the 1-based distance from the top of the stack.
    */
    int32_t अन्वेषणम्(शून्य* obj) ध्रुव;

    /**
     * ICU "poor man's RTTI", returns a UClassID for this class.
     */
    स्थिर UClassID U_EXPORT2 getStaticClassID();

    /**
     * ICU "poor man's RTTI", returns a UClassID for the actual class.
     */
    आभास UClassID getDynamicClassID() ध्रुव override;

    // Disallow
    UStack(ध्रुव UStack&) = delete;

    // Disallow
    UStack& संक्रियकः=(ध्रुव UStack&) = delete;
};

U_NAMESPACE_अन्तः

#यदि_अन्तः
