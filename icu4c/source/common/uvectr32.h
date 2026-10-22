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

#अगर_अघोषित UVECTOR32_H
#घोषणा UVECTOR32_H

#समावेश "unicode/utypes.h"
#समावेश "unicode/uobject.h"
#समावेश "uhash.h"
#समावेश "uassert.h"

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
class U_COMMON_API UVector32 : खुला UObject {
निजी:
    int32_t   गिनती;

    int32_t   capacity;
    
    int32_t   maxCapacity;   // Limit beyond which capacity is not permitted to grow.

    int32_t*  elements;

खुला:
    UVector32(UErrorCode &status);

    UVector32(int32_t initialCapacity, UErrorCode &status);

    भव ~UVector32();

    /**
     * Assign this object to another (make this a copy of 'other').
     * Use the 'assign' function to assign each element.
     */
    व्योम assign(स्थिर UVector32& other, UErrorCode &ec);

    /**
     * Compare this vector with another.  They will be considered
     * equal if they are of the same size and all elements are equal,
     * as compared using this object's comparer.
     */
    bool चालक==(स्थिर UVector32& other) स्थिर;

    /**
     * Equivalent to !operator==()
     */
    अंतरभूत bool चालक!=(स्थिर UVector32& other) स्थिर;

    //------------------------------------------------------------
    // java.util.Vector API
    //------------------------------------------------------------

    अंतरभूत व्योम addElement(int32_t elem, UErrorCode &status);

    व्योम setElementAt(int32_t elem, int32_t सूचकांक);

    व्योम जोड़ो_Element_At(int32_t elem, int32_t सूचकांक, UErrorCode &status);
    
    अंतरभूत int32_t elementAti(int32_t सूचकांक) स्थिर;

    UBool equals(स्थिर UVector32 &other) स्थिर;

    अंतरभूत int32_t lastElementi() स्थिर;

    int32_t सूचकांक_Of(int32_t elem, int32_t start_सूचकांक = 0) स्थिर;

    अंतरभूत UBool contains(int32_t elem) स्थिर;

    UBool containsAll(स्थिर UVector32& other) स्थिर;

    UBool removeAll(स्थिर UVector32& other);

    UBool retainAll(स्थिर UVector32& other);

    व्योम removeElementAt(int32_t सूचकांक);

    व्योम removeAllElements();

    अंतरभूत int32_t आकार_2() स्थिर;

    अंतरभूत UBool isEmpty() स्थिर;

    // Inline.  Use this one for speedy size check.
    अंतरभूत UBool ensureCapacity(int32_t minimumCapacity, UErrorCode &status);

    // Out-of-line, handles actual growth.  Called by ensureCapacity() when necessary.
    UBool expandCapacity(int32_t minimumCapacity, UErrorCode &status);

    /**
     * Change the size of this vector as follows: If newSize is
     * smaller, then truncate the array, possibly deleting held
     * elements for i >= newSize.  If newSize is larger, grow the
     * array, filling in new slows with zero.
     */
    व्योम set_आकार(int32_t new_आकार);

    //------------------------------------------------------------
    // New API
    //------------------------------------------------------------

    /**
     * Returns true if this vector contains none of the elements
     * of the given vector.
     * @param other vector to be checked for containment
     * @return true if the test condition is met
     */
    UBool containsNone(स्थिर UVector32& other) स्थिर;


    /**
     * Insert the given integer into this vector at its sorted position.
     * The current elements are assumed to be sorted already.
     */
    व्योम sorted_जोड़ो(int32_t elem, UErrorCode& ec);

    /**
     * Returns a pointer to the internal array holding the vector.
     */
    अंतरभूत int32_t *get_बफर() स्थिर;

    /**
     * Set the maximum allowed buffer capacity for this vector/stack.
     * Default with no limit set is unlimited, go until malloc() fails.
     * A Limit of zero means unlimited capacity.
     * Units are vector elements (32 bits each), not bytes.
     */
    व्योम setMaxCapacity(int32_t limit);

    /**
     * ICU "poor man's RTTI", returns a UClassID for this class.
     */
    अटल UClassID U_EXPORT2 getStaticClassID();

    /**
     * ICU "poor man's RTTI", returns a UClassID for the actual class.
     */
    भव UClassID getDynamicClassID() स्थिर override;

निजी:
    व्योम _init(int32_t initialCapacity, UErrorCode &status);

    // Disallow
    UVector32(स्थिर UVector32&) = delete;

    // Disallow
    UVector32& चालक=(स्थिर UVector32&) = delete;


    //  API Functions for Stack operations.
    //  In the original UVector, these were in a separate derived class, UStack.
    //  Here in UVector32, they are all together.
खुला:
    अंतरभूत UBool empty() स्थिर;   // TODO:  redundant, same as empty().  Remove it?

    अंतरभूत int32_t peeki() स्थिर;
    
    अंतरभूत int32_t popi();
    
    अंतरभूत int32_t धकेलो(int32_t i, UErrorCode &status);

    अंतरभूत int32_t *reserveBlock(int32_t आकार_2, UErrorCode &status);
    अंतरभूत int32_t *निकालो_Frame(int32_t आकार_2);
};


// UVector32 inlines

अंतरभूत UBool UVector32::ensureCapacity(int32_t minimumCapacity, UErrorCode &status) {
    अगर ((minimumCapacity >= 0) && (capacity >= minimumCapacity)) {
        वापस true;
    } अन्यथा {
        वापस expandCapacity(minimumCapacity, status);
    }
}

अंतरभूत int32_t UVector32::elementAti(int32_t सूचकांक) स्थिर {
    वापस (सूचकांक >= 0 && गिनती > 0 && गिनती - सूचकांक > 0) ? elements[सूचकांक] : 0;
}


अंतरभूत व्योम UVector32::addElement(int32_t elem, UErrorCode &status) {
    अगर (ensureCapacity(गिनती + 1, status)) {
        elements[गिनती] = elem;
        गिनती++;
    }
}

अंतरभूत int32_t *UVector32::reserveBlock(int32_t आकार_2, UErrorCode &status) {
    अगर (ensureCapacity(गिनती+आकार_2, status) == false) {
        वापस nullptr;
    }
    int32_t  *rp = elements+गिनती;
    गिनती += आकार_2;
    वापस rp;
}

अंतरभूत int32_t *UVector32::निकालो_Frame(int32_t आकार_2) {
    U_ASSERT(गिनती >= आकार_2);
    गिनती -= आकार_2;
    अगर (गिनती < 0) {
        गिनती = 0;
    }
    वापस elements+गिनती-आकार_2;
}



अंतरभूत int32_t UVector32::आकार_2() स्थिर {
    वापस गिनती;
}

अंतरभूत UBool UVector32::isEmpty() स्थिर {
    वापस गिनती == 0;
}

अंतरभूत UBool UVector32::contains(int32_t obj) स्थिर {
    वापस सूचकांक_Of(obj) >= 0;
}

अंतरभूत int32_t UVector32::lastElementi() स्थिर {
    वापस elementAti(गिनती-1);
}

अंतरभूत bool UVector32::चालक!=(स्थिर UVector32& other) स्थिर {
    वापस !चालक==(other);
}

अंतरभूत int32_t *UVector32::get_बफर() स्थिर {
    वापस elements;
}


// UStack inlines

अंतरभूत UBool UVector32::empty() स्थिर {
    वापस isEmpty();
}

अंतरभूत int32_t UVector32::peeki() स्थिर {
    वापस lastElementi();
}

अंतरभूत int32_t UVector32::धकेलो(int32_t i, UErrorCode &status) {
    addElement(i, status);
    वापस i;
}

अंतरभूत int32_t UVector32::popi() {
    int32_t परिणाम = 0;
    अगर (गिनती > 0) {
        गिनती--;
        परिणाम = elements[गिनती];
    }
    वापस परिणाम;
}

U_NAMESPACE_END

#पूर्ण_अगर
