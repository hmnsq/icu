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

#ifndef UVECTOR64_H
#define UVECTOR64_H

#അന്തര്ഭാവ "unicode/utypes.h"
#അന്തര്ഭാവ "unicode/uobject.h"
#അന്തര്ഭാവ "uhash.h"
#അന്തര്ഭാവ "uassert.h"

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
class U_COMMON_API UVector64 : പൊതു UObject {
സ്വകാര്യ:
    int32_t   എണ്ണം;

    int32_t   capacity;
    
    int32_t   maxCapacity;   // Limit beyond which capacity is not permitted to grow.

    int64_t*  elements;

പൊതു:
    UVector64(UErrorCode &status);

    UVector64(int32_t initialCapacity, UErrorCode &status);

    മായാ ~UVector64();

    /**
     * Assign this object to another (make this a copy of 'other').
     * Use the 'assign' function to assign each element.
     */
    ശൂന്യം assign(സ്ഥിരം UVector64& other, UErrorCode &ec);

    /**
     * Compare this vector with another.  They will be considered
     * equal if they are of the same size and all elements are equal,
     * as compared using this object's comparer.
     */
    bool operator==(സ്ഥിരം UVector64& other);

    /**
     * Equivalent to !operator==()
     */
    വരിയിൽ bool operator!=(സ്ഥിരം UVector64& other);

    //------------------------------------------------------------
    // subset of java.util.Vector API
    //------------------------------------------------------------

    വരിയിൽ ശൂന്യം addElement(int64_t elem, UErrorCode &status);

    ശൂന്യം setElementAt(int64_t elem, int32_t index);

    ശൂന്യം തിരുകുക_Element_At(int64_t elem, int32_t index, UErrorCode &status);
    
    വരിയിൽ int64_t elementAti(int32_t index) സ്ഥിരം;

    //UBool equals(const UVector64 &other) const;

    വരിയിൽ int64_t lastElementi() സ്ഥിരം;

    //int32_t indexOf(int64_t elem, int32_t startIndex = 0) const;

    //UBool contains(int64_t elem) const;

    //UBool containsAll(const UVector64& other) const;

    //UBool removeAll(const UVector64& other);

    //UBool retainAll(const UVector64& other);

    //void removeElementAt(int32_t index);

    ശൂന്യം removeAllElements();

    വരിയിൽ int32_t size() സ്ഥിരം;

    വരിയിൽ UBool isEmpty() സ്ഥിരം { തിരികെ എണ്ണം == 0; }

    // Inline.  Use this one for speedy size check.
    വരിയിൽ UBool ensureCapacity(int32_t minimumCapacity, UErrorCode &status);

    // Out-of-line, handles actual growth.  Called by ensureCapacity() when necessary.
    UBool expandCapacity(int32_t minimumCapacity, UErrorCode &status);

    /**
     * Change the size of this vector as follows: If newSize is
     * smaller, then truncate the array, possibly deleting held
     * elements for i >= newSize.  If newSize is larger, grow the
     * array, filling in new slows with zero.
     */
    ശൂന്യം setSize(int32_t newSize);

    //------------------------------------------------------------
    // New API
    //------------------------------------------------------------

    //UBool containsNone(const UVector64& other) const;


    //void sortedInsert(int64_t elem, UErrorCode& ec);

    /**
     * Returns a pointer to the internal array holding the vector.
     */
    വരിയിൽ int64_t *getBuffer() സ്ഥിരം;

    /**
     * Set the maximum allowed buffer capacity for this vector/stack.
     * Default with no limit set is unlimited, go until malloc() fails.
     * A Limit of zero means unlimited capacity.
     * Units are vector elements (64 bits each), not bytes.
     */
    ശൂന്യം setMaxCapacity(int32_t limit);

    /**
     * ICU "poor man's RTTI", returns a UClassID for this class.
     */
    സ്ഥിതം UClassID U_EXPORT2 getStaticClassID();

    /**
     * ICU "poor man's RTTI", returns a UClassID for the actual class.
     */
    മായാ UClassID getDynamicClassID() സ്ഥിരം override;

സ്വകാര്യ:
    ശൂന്യം _init(int32_t initialCapacity, UErrorCode &status);

    // Disallow
    UVector64(സ്ഥിരം UVector64&) = delete;

    // Disallow
    UVector64& operator=(സ്ഥിരം UVector64&) = delete;


    //  API Functions for Stack operations.
    //  In the original UVector, these were in a separate derived class, UStack.
    //  Here in UVector64, they are all together.
പൊതു:
    //UBool empty() const;   // TODO:  redundant, same as empty().  Remove it?

    //int64_t peeki() const;
    
    വരിയിൽ int64_t popi();
    
    വരിയിൽ int64_t തള്ളുക(int64_t i, UErrorCode &status);

    വരിയിൽ int64_t *reserveBlock(int32_t size, UErrorCode &status);
    വരിയിൽ int64_t *എടുക്കുക_Frame(int32_t size);
};


// UVector64 inlines

വരിയിൽ UBool UVector64::ensureCapacity(int32_t minimumCapacity, UErrorCode &status) {
    എങ്കിൽ ((minimumCapacity >= 0) && (capacity >= minimumCapacity)) {
        തിരികെ true;
    } അല്ലെങ്കിൽ {
        തിരികെ expandCapacity(minimumCapacity, status);
    }
}

വരിയിൽ int64_t UVector64::elementAti(int32_t index) സ്ഥിരം {
    തിരികെ (0 <= index && index < എണ്ണം) ? elements[index] : 0;
}


വരിയിൽ ശൂന്യം UVector64::addElement(int64_t elem, UErrorCode &status) {
    എങ്കിൽ (ensureCapacity(എണ്ണം + 1, status)) {
        elements[എണ്ണം] = elem;
        എണ്ണം++;
    }
}

വരിയിൽ int64_t *UVector64::reserveBlock(int32_t size, UErrorCode &status) {
    എങ്കിൽ (ensureCapacity(എണ്ണം+size, status) == false) {
        തിരികെ nullptr;
    }
    int64_t  *rp = elements+എണ്ണം;
    എണ്ണം += size;
    തിരികെ rp;
}

വരിയിൽ int64_t *UVector64::എടുക്കുക_Frame(int32_t size) {
    U_ASSERT(എണ്ണം >= size);
    എണ്ണം -= size;
    എങ്കിൽ (എണ്ണം < 0) {
        എണ്ണം = 0;
    }
    തിരികെ elements+എണ്ണം-size;
}



വരിയിൽ int32_t UVector64::size() സ്ഥിരം {
    തിരികെ എണ്ണം;
}

വരിയിൽ int64_t UVector64::lastElementi() സ്ഥിരം {
    തിരികെ elementAti(എണ്ണം-1);
}

വരിയിൽ bool UVector64::operator!=(സ്ഥിരം UVector64& other) {
    തിരികെ !operator==(other);
}

വരിയിൽ int64_t *UVector64::getBuffer() സ്ഥിരം {
    തിരികെ elements;
}


// UStack inlines

വരിയിൽ int64_t UVector64::തള്ളുക(int64_t i, UErrorCode &status) {
    addElement(i, status);
    തിരികെ i;
}

വരിയിൽ int64_t UVector64::popi() {
    int64_t result = 0;
    എങ്കിൽ (എണ്ണം > 0) {
        എണ്ണം--;
        result = elements[എണ്ണം];
    }
    തിരികെ result;
}

U_NAMESPACE_END

#endif
