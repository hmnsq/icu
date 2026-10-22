// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
**********************************************************************
*   Copyright (C) 1999-2011, International Business Machines
*   Corporation and others.  All Rights Reserved.
**********************************************************************
*/

//
//  UVector32 is a class implementing a vector of 32 bit integers.
//            It is similar to UVector, but holds int32_t values rather than pointers.
//            Most of the code is unchanged from UVector.
//

#यदि_अपरिभाषितम् UVECTOR32_H
#परिभाषय UVECTOR32_H

#अन्तर्भाव "unicode/utypes.h"
#अन्तर्भाव "unicode/uobject.h"
#अन्तर्भाव "uhash.h"
#अन्तर्भाव "uassert.h"

U_NAMESPACE_BEGIN



/**
 * <p>Ultralightweight C++ implementation of a <tt>void*</tt> vector
 * that is (mostly) compatible with java.util.Vector.
 *
 * <p>This is a very simple implementation, written to satisfy an
 * immediate porting need.  As such, it is not completely fleshed out,
 * and it aims for simplicity and conformity.  Nonetheless, it serves
 * its purpose (porting code from java that uses java.util.Vector)
 * well, and it could be easily made into a more robust vector class.
 *
 * <p><b>Design notes</b>
 *
 * <p>There is index bounds checking, but little is done about it.  If
 * indices are out of bounds, either nothing happens, or zero is
 * returned.  We <em>do</em> avoid indexing off into the weeds.
 *
 * <p>There is detection of out of memory, but the handling is very
 * coarse-grained -- similar to UnicodeString's protocol, but even
 * coarser.  The class contains <em>one static flag</em> that is set
 * when any call to <tt>new</tt> returns zero.  This allows the caller
 * to use several vectors and make just one check at the end to see if
 * a memory failure occurred.  This is more efficient than making a
 * check after each call on each vector when doing many operations on
 * multiple vectors.  The single static flag works best when memory
 * failures are infrequent, and when recovery options are limited or
 * nonexistent.
 *
 * <p><b>To do</b>
 *
 * <p>Improve the handling of index out of bounds errors.
 *
 * @author Alan Liu
 */
class U_COMMON_API UVector32 : सार्वजनिक UObject {
गुप्त:
    int32_t   गणनम्;

    int32_t   क्षमता;
    
    int32_t   max_क्षमता;   // Limit beyond which capacity is not permitted to grow.

    int32_t*  elements;

सार्वजनिक:
    UVector32(UError_संहिता &स्थितिः);

    UVector32(int32_t initial_क्षमता, UError_संहिता &स्थितिः);

    आभास ~UVector32();

    /**
     * Assign this object to another (make this a copy of 'other').
     * Use the 'assign' function to assign each element.
     */
    शून्य नियोजय(ध्रुव UVector32& other, UError_संहिता &ec);

    /**
     * Compare this vector with another.  They will be considered
     * equal if they are of the same size and all elements are equal,
     * as compared using this object's comparer.
     */
    bool संक्रियकः==(ध्रुव UVector32& other) ध्रुव;

    /**
     * Equivalent to !operator==()
     */
    अन्तःस्थ bool संक्रियकः!=(ध्रुव UVector32& other) ध्रुव;

    //------------------------------------------------------------
    // java.util.Vector API
    //------------------------------------------------------------

    अन्तःस्थ शून्य सङ्कलय_Element(int32_t elem, UError_संहिता &स्थितिः);

    शून्य समुच्चयः_Element_At(int32_t elem, int32_t सूचकांक);

    शून्य निवेशय_Element_At(int32_t elem, int32_t सूचकांक, UError_संहिता &स्थितिः);
    
    अन्तःस्थ int32_t elementAti(int32_t सूचकांक) ध्रुव;

    UBool equals(ध्रुव UVector32 &other) ध्रुव;

    अन्तःस्थ int32_t चरमः_Elementi() ध्रुव;

    int32_t सूचकांक_सम्बन्धि(int32_t elem, int32_t start_सूचकांक = 0) ध्रुव;

    अन्तःस्थ UBool अन्तर्भवति(int32_t elem) ध्रुव;

    UBool अन्तर्भवति_All(ध्रुव UVector32& other) ध्रुव;

    UBool removeAll(ध्रुव UVector32& other);

    UBool retainAll(ध्रुव UVector32& other);

    शून्य removeElementAt(int32_t सूचकांक);

    शून्य removeAllElements();

    अन्तःस्थ int32_t आकार_2() ध्रुव;

    अन्तःस्थ UBool is_रिक्तानि() ध्रुव;

    // Inline.  Use this one for speedy size check.
    अन्तःस्थ UBool ensure_क्षमता(int32_t minimum_क्षमता, UError_संहिता &स्थितिः);

    // Out-of-line, handles actual growth.  Called by ensureCapacity() when necessary.
    UBool expand_क्षमता(int32_t minimum_क्षमता, UError_संहिता &स्थितिः);

    /**
     * Change the size of this vector as follows: If newSize is
     * smaller, then truncate the array, possibly deleting held
     * elements for i >= newSize.  If newSize is larger, grow the
     * array, filling in new slows with zero.
     */
    शून्य समुच्चयः_आकार(int32_t new_आकार);

    //------------------------------------------------------------
    // New API
    //------------------------------------------------------------

    /**
     * Returns true if this vector contains none of the elements
     * of the given vector.
     * @param other vector to be checked for containment
     * @return true if the test condition is met
     */
    UBool अन्तर्भवति_None(ध्रुव UVector32& other) ध्रुव;


    /**
     * Insert the given integer into this vector at its sorted position.
     * The current elements are assumed to be sorted already.
     */
    शून्य sorted_निवेशय(int32_t elem, UError_संहिता& ec);

    /**
     * Returns a pointer to the internal array holding the vector.
     */
    अन्तःस्थ int32_t *get_बफर() ध्रुव;

    /**
     * Set the maximum allowed buffer capacity for this vector/stack.
     * Default with no limit set is unlimited, go until malloc() fails.
     * A Limit of zero means unlimited capacity.
     * Units are vector elements (32 bits each), not bytes.
     */
    शून्य समुच्चयः_Max_क्षमता(int32_t सीमा);

    /**
     * ICU "poor man's RTTI", returns a UClassID for this class.
     */
    स्थिर UClassID U_EXPORT2 getStaticClassID();

    /**
     * ICU "poor man's RTTI", returns a UClassID for the actual class.
     */
    आभास UClassID getDynamicClassID() ध्रुव override;

गुप्त:
    शून्य _init(int32_t initial_क्षमता, UError_संहिता &स्थितिः);

    // Disallow
    UVector32(ध्रुव UVector32&) = delete;

    // Disallow
    UVector32& संक्रियकः=(ध्रुव UVector32&) = delete;


    //  API Functions for Stack operations.
    //  In the original UVector, these were in a separate derived class, UStack.
    //  Here in UVector32, they are all together.
सार्वजनिक:
    अन्तःस्थ UBool रिक्तानि() ध्रुव;   // TODO:  redundant, same as empty().  Remove it?

    अन्तःस्थ int32_t peeki() ध्रुव;
    
    अन्तःस्थ int32_t popi();
    
    अन्तःस्थ int32_t प्रक्षिप(int32_t i, UError_संहिता &स्थितिः);

    अन्तःस्थ int32_t *reserve_खण्डम्(int32_t आकार_2, UError_संहिता &स्थितिः);
    अन्तःस्थ int32_t *निष्कासय_Frame(int32_t आकार_2);
};


// UVector32 inlines

अन्तःस्थ UBool UVector32::ensure_क्षमता(int32_t minimum_क्षमता, UError_संहिता &स्थितिः) {
    यदि ((minimum_क्षमता >= 0) && (क्षमता >= minimum_क्षमता)) {
        प्रत्यागम true;
    } अन्यथा {
        प्रत्यागम expand_क्षमता(minimum_क्षमता, स्थितिः);
    }
}

अन्तःस्थ int32_t UVector32::elementAti(int32_t सूचकांक) ध्रुव {
    प्रत्यागम (सूचकांक >= 0 && गणनम् > 0 && गणनम् - सूचकांक > 0) ? elements[सूचकांक] : 0;
}


अन्तःस्थ शून्य UVector32::सङ्कलय_Element(int32_t elem, UError_संहिता &स्थितिः) {
    यदि (ensure_क्षमता(गणनम् + 1, स्थितिः)) {
        elements[गणनम्] = elem;
        गणनम्++;
    }
}

अन्तःस्थ int32_t *UVector32::reserve_खण्डम्(int32_t आकार_2, UError_संहिता &स्थितिः) {
    यदि (ensure_क्षमता(गणनम्+आकार_2, स्थितिः) == false) {
        प्रत्यागम nullptr;
    }
    int32_t  *rp = elements+गणनम्;
    गणनम् += आकार_2;
    प्रत्यागम rp;
}

अन्तःस्थ int32_t *UVector32::निष्कासय_Frame(int32_t आकार_2) {
    U_ASSERT(गणनम् >= आकार_2);
    गणनम् -= आकार_2;
    यदि (गणनम् < 0) {
        गणनम् = 0;
    }
    प्रत्यागम elements+गणनम्-आकार_2;
}



अन्तःस्थ int32_t UVector32::आकार_2() ध्रुव {
    प्रत्यागम गणनम्;
}

अन्तःस्थ UBool UVector32::is_रिक्तानि() ध्रुव {
    प्रत्यागम गणनम् == 0;
}

अन्तःस्थ UBool UVector32::अन्तर्भवति(int32_t obj) ध्रुव {
    प्रत्यागम सूचकांक_सम्बन्धि(obj) >= 0;
}

अन्तःस्थ int32_t UVector32::चरमः_Elementi() ध्रुव {
    प्रत्यागम elementAti(गणनम्-1);
}

अन्तःस्थ bool UVector32::संक्रियकः!=(ध्रुव UVector32& other) ध्रुव {
    प्रत्यागम !संक्रियकः==(other);
}

अन्तःस्थ int32_t *UVector32::get_बफर() ध्रुव {
    प्रत्यागम elements;
}


// UStack inlines

अन्तःस्थ UBool UVector32::रिक्तानि() ध्रुव {
    प्रत्यागम is_रिक्तानि();
}

अन्तःस्थ int32_t UVector32::peeki() ध्रुव {
    प्रत्यागम चरमः_Elementi();
}

अन्तःस्थ int32_t UVector32::प्रक्षिप(int32_t i, UError_संहिता &स्थितिः) {
    सङ्कलय_Element(i, स्थितिः);
    प्रत्यागम i;
}

अन्तःस्थ int32_t UVector32::popi() {
    int32_t परिणाम = 0;
    यदि (गणनम् > 0) {
        गणनम्--;
        परिणाम = elements[गणनम्];
    }
    प्रत्यागम परिणाम;
}

U_NAMESPACE_अन्तः

#यदि_अन्तः
