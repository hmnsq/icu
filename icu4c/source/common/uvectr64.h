// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
**********************************************************************
*   Copyright (C) 1999-2014, International Business Machines
*   Corporation and others.  All Rights Reserved.
**********************************************************************
*/

//
//  UVector64 is a class implementing a vector of 64 bit integers.
//            It is similar to UVector32, but holds int64_t values rather than int32_t.
//            Most of the code is unchanged from UVector.
//

#यदि_अपरिभाषितम् UVECTOR64_H
#परिभाषय UVECTOR64_H

#अन्तर्भाव "unicode/utypes.h"
#अन्तर्भाव "unicode/uobject.h"
#अन्तर्भाव "uhash.h"
#अन्तर्भाव "uassert.h"

U_NAMESPACE_BEGIN



/**
 * <p>Ultralightweight C++ implementation of an <tt>int64_t</tt> vector
 * that has a subset of methods from UVector32
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
 */
class U_COMMON_API UVector64 : सार्वजनिक UObject {
गुप्त:
    int32_t   गणनम्;

    int32_t   क्षमता;
    
    int32_t   max_क्षमता;   // Limit beyond which capacity is not permitted to grow.

    int64_t*  elements;

सार्वजनिक:
    UVector64(UError_संहिता &स्थितिः);

    UVector64(int32_t initial_क्षमता, UError_संहिता &स्थितिः);

    आभास ~UVector64();

    /**
     * Assign this object to another (make this a copy of 'other').
     * Use the 'assign' function to assign each element.
     */
    शून्य नियोजय(ध्रुव UVector64& other, UError_संहिता &ec);

    /**
     * Compare this vector with another.  They will be considered
     * equal if they are of the same size and all elements are equal,
     * as compared using this object's comparer.
     */
    bool संक्रियकः==(ध्रुव UVector64& other);

    /**
     * Equivalent to !operator==()
     */
    अन्तःस्थ bool संक्रियकः!=(ध्रुव UVector64& other);

    //------------------------------------------------------------
    // subset of java.util.Vector API
    //------------------------------------------------------------

    अन्तःस्थ शून्य सङ्कलय_Element(int64_t elem, UError_संहिता &स्थितिः);

    शून्य समुच्चयः_Element_At(int64_t elem, int32_t सूचकांक);

    शून्य निवेशय_Element_At(int64_t elem, int32_t सूचकांक, UError_संहिता &स्थितिः);
    
    अन्तःस्थ int64_t elementAti(int32_t सूचकांक) ध्रुव;

    //UBool equals(const UVector64 &other) const;

    अन्तःस्थ int64_t चरमः_Elementi() ध्रुव;

    //int32_t indexOf(int64_t elem, int32_t startIndex = 0) const;

    //UBool contains(int64_t elem) const;

    //UBool containsAll(const UVector64& other) const;

    //UBool removeAll(const UVector64& other);

    //UBool retainAll(const UVector64& other);

    //void removeElementAt(int32_t index);

    शून्य removeAllElements();

    अन्तःस्थ int32_t आकार_2() ध्रुव;

    अन्तःस्थ UBool is_रिक्तानि() ध्रुव { प्रत्यागम गणनम् == 0; }

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

    //UBool containsNone(const UVector64& other) const;


    //void sortedInsert(int64_t elem, UErrorCode& ec);

    /**
     * Returns a pointer to the internal array holding the vector.
     */
    अन्तःस्थ int64_t *get_बफर() ध्रुव;

    /**
     * Set the maximum allowed buffer capacity for this vector/stack.
     * Default with no limit set is unlimited, go until malloc() fails.
     * A Limit of zero means unlimited capacity.
     * Units are vector elements (64 bits each), not bytes.
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
    UVector64(ध्रुव UVector64&) = delete;

    // Disallow
    UVector64& संक्रियकः=(ध्रुव UVector64&) = delete;


    //  API Functions for Stack operations.
    //  In the original UVector, these were in a separate derived class, UStack.
    //  Here in UVector64, they are all together.
सार्वजनिक:
    //UBool empty() const;   // TODO:  redundant, same as empty().  Remove it?

    //int64_t peeki() const;
    
    अन्तःस्थ int64_t popi();
    
    अन्तःस्थ int64_t प्रक्षिप(int64_t i, UError_संहिता &स्थितिः);

    अन्तःस्थ int64_t *reserve_खण्डम्(int32_t आकार_2, UError_संहिता &स्थितिः);
    अन्तःस्थ int64_t *निष्कासय_Frame(int32_t आकार_2);
};


// UVector64 inlines

अन्तःस्थ UBool UVector64::ensure_क्षमता(int32_t minimum_क्षमता, UError_संहिता &स्थितिः) {
    यदि ((minimum_क्षमता >= 0) && (क्षमता >= minimum_क्षमता)) {
        प्रत्यागम true;
    } अन्यथा {
        प्रत्यागम expand_क्षमता(minimum_क्षमता, स्थितिः);
    }
}

अन्तःस्थ int64_t UVector64::elementAti(int32_t सूचकांक) ध्रुव {
    प्रत्यागम (0 <= सूचकांक && सूचकांक < गणनम्) ? elements[सूचकांक] : 0;
}


अन्तःस्थ शून्य UVector64::सङ्कलय_Element(int64_t elem, UError_संहिता &स्थितिः) {
    यदि (ensure_क्षमता(गणनम् + 1, स्थितिः)) {
        elements[गणनम्] = elem;
        गणनम्++;
    }
}

अन्तःस्थ int64_t *UVector64::reserve_खण्डम्(int32_t आकार_2, UError_संहिता &स्थितिः) {
    यदि (ensure_क्षमता(गणनम्+आकार_2, स्थितिः) == false) {
        प्रत्यागम nullptr;
    }
    int64_t  *rp = elements+गणनम्;
    गणनम् += आकार_2;
    प्रत्यागम rp;
}

अन्तःस्थ int64_t *UVector64::निष्कासय_Frame(int32_t आकार_2) {
    U_ASSERT(गणनम् >= आकार_2);
    गणनम् -= आकार_2;
    यदि (गणनम् < 0) {
        गणनम् = 0;
    }
    प्रत्यागम elements+गणनम्-आकार_2;
}



अन्तःस्थ int32_t UVector64::आकार_2() ध्रुव {
    प्रत्यागम गणनम्;
}

अन्तःस्थ int64_t UVector64::चरमः_Elementi() ध्रुव {
    प्रत्यागम elementAti(गणनम्-1);
}

अन्तःस्थ bool UVector64::संक्रियकः!=(ध्रुव UVector64& other) {
    प्रत्यागम !संक्रियकः==(other);
}

अन्तःस्थ int64_t *UVector64::get_बफर() ध्रुव {
    प्रत्यागम elements;
}


// UStack inlines

अन्तःस्थ int64_t UVector64::प्रक्षिप(int64_t i, UError_संहिता &स्थितिः) {
    सङ्कलय_Element(i, स्थितिः);
    प्रत्यागम i;
}

अन्तःस्थ int64_t UVector64::popi() {
    int64_t परिणाम = 0;
    यदि (गणनम् > 0) {
        गणनम्--;
        परिणाम = elements[गणनम्];
    }
    प्रत्यागम परिणाम;
}

U_NAMESPACE_अन्तः

#यदि_अन्तः
