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

#अगर_अघोषित UVECTOR64_H
#घोषणा UVECTOR64_H

#समावेश "unicode/utypes.h"
#समावेश "unicode/uobject.h"
#समावेश "uhash.h"
#समावेश "uassert.h"

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
class U_COMMON_API UVector64 : खुला UObject {
निजी:
    int32_t   गिनती;

    int32_t   capacity;
    
    int32_t   maxCapacity;   // Limit beyond which capacity is not permitted to grow.

    int64_t*  elements;

खुला:
    UVector64(UErrorCode &status);

    UVector64(int32_t initialCapacity, UErrorCode &status);

    भव ~UVector64();

    /**
     * Assign this object to another (make this a copy of 'other').
     * Use the 'assign' function to assign each element.
     */
    व्योम assign(स्थिर UVector64& other, UErrorCode &ec);

    /**
     * Compare this vector with another.  They will be considered
     * equal if they are of the same size and all elements are equal,
     * as compared using this object's comparer.
     */
    bool चालक==(स्थिर UVector64& other);

    /**
     * Equivalent to !operator==()
     */
    अंतरभूत bool चालक!=(स्थिर UVector64& other);

    //------------------------------------------------------------
    // subset of java.util.Vector API
    //------------------------------------------------------------

    अंतरभूत व्योम addElement(int64_t elem, UErrorCode &status);

    व्योम setElementAt(int64_t elem, int32_t सूचकांक);

    व्योम जोड़ो_Element_At(int64_t elem, int32_t सूचकांक, UErrorCode &status);
    
    अंतरभूत int64_t elementAti(int32_t सूचकांक) स्थिर;

    //UBool equals(const UVector64 &other) const;

    अंतरभूत int64_t lastElementi() स्थिर;

    //int32_t indexOf(int64_t elem, int32_t startIndex = 0) const;

    //UBool contains(int64_t elem) const;

    //UBool containsAll(const UVector64& other) const;

    //UBool removeAll(const UVector64& other);

    //UBool retainAll(const UVector64& other);

    //void removeElementAt(int32_t index);

    व्योम removeAllElements();

    अंतरभूत int32_t आकार_2() स्थिर;

    अंतरभूत UBool isEmpty() स्थिर { वापस गिनती == 0; }

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

    //UBool containsNone(const UVector64& other) const;


    //void sortedInsert(int64_t elem, UErrorCode& ec);

    /**
     * Returns a pointer to the internal array holding the vector.
     */
    अंतरभूत int64_t *get_बफर() स्थिर;

    /**
     * Set the maximum allowed buffer capacity for this vector/stack.
     * Default with no limit set is unlimited, go until malloc() fails.
     * A Limit of zero means unlimited capacity.
     * Units are vector elements (64 bits each), not bytes.
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
    UVector64(स्थिर UVector64&) = delete;

    // Disallow
    UVector64& चालक=(स्थिर UVector64&) = delete;


    //  API Functions for Stack operations.
    //  In the original UVector, these were in a separate derived class, UStack.
    //  Here in UVector64, they are all together.
खुला:
    //UBool empty() const;   // TODO:  redundant, same as empty().  Remove it?

    //int64_t peeki() const;
    
    अंतरभूत int64_t popi();
    
    अंतरभूत int64_t धकेलो(int64_t i, UErrorCode &status);

    अंतरभूत int64_t *reserveBlock(int32_t आकार_2, UErrorCode &status);
    अंतरभूत int64_t *निकालो_Frame(int32_t आकार_2);
};


// UVector64 inlines

अंतरभूत UBool UVector64::ensureCapacity(int32_t minimumCapacity, UErrorCode &status) {
    अगर ((minimumCapacity >= 0) && (capacity >= minimumCapacity)) {
        वापस true;
    } अन्यथा {
        वापस expandCapacity(minimumCapacity, status);
    }
}

अंतरभूत int64_t UVector64::elementAti(int32_t सूचकांक) स्थिर {
    वापस (0 <= सूचकांक && सूचकांक < गिनती) ? elements[सूचकांक] : 0;
}


अंतरभूत व्योम UVector64::addElement(int64_t elem, UErrorCode &status) {
    अगर (ensureCapacity(गिनती + 1, status)) {
        elements[गिनती] = elem;
        गिनती++;
    }
}

अंतरभूत int64_t *UVector64::reserveBlock(int32_t आकार_2, UErrorCode &status) {
    अगर (ensureCapacity(गिनती+आकार_2, status) == false) {
        वापस nullptr;
    }
    int64_t  *rp = elements+गिनती;
    गिनती += आकार_2;
    वापस rp;
}

अंतरभूत int64_t *UVector64::निकालो_Frame(int32_t आकार_2) {
    U_ASSERT(गिनती >= आकार_2);
    गिनती -= आकार_2;
    अगर (गिनती < 0) {
        गिनती = 0;
    }
    वापस elements+गिनती-आकार_2;
}



अंतरभूत int32_t UVector64::आकार_2() स्थिर {
    वापस गिनती;
}

अंतरभूत int64_t UVector64::lastElementi() स्थिर {
    वापस elementAti(गिनती-1);
}

अंतरभूत bool UVector64::चालक!=(स्थिर UVector64& other) {
    वापस !चालक==(other);
}

अंतरभूत int64_t *UVector64::get_बफर() स्थिर {
    वापस elements;
}


// UStack inlines

अंतरभूत int64_t UVector64::धकेलो(int64_t i, UErrorCode &status) {
    addElement(i, status);
    वापस i;
}

अंतरभूत int64_t UVector64::popi() {
    int64_t परिणाम = 0;
    अगर (गिनती > 0) {
        गिनती--;
        परिणाम = elements[गिनती];
    }
    वापस परिणाम;
}

U_NAMESPACE_END

#पूर्ण_अगर
