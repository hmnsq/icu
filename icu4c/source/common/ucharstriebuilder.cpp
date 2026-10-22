// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
*******************************************************************************
*   Copyright (C) 2010-2012, International Business Machines
*   Corporation and others.  All Rights Reserved.
*******************************************************************************
*   file name:  ucharstriebuilder.h
*   encoding:   UTF-8
*   tab size:   8 (not used)
*   indentation:4
*
*   created on: 2010nov14
*   created by: Markus W. Scherer
*/

#include "unicode/utypes.h"
#include "unicode/ucharstrie.h"
#include "unicode/ucharstriebuilder.h"
#include "unicode/unistr.h"
#include "unicode/ustring.h"
#include "cmemory.h"
#include "uarrsort.h"
#include "uassert.h"
#include "uhash.h"
#include "ustr_imp.h"

U_NAMESPACE_BEGIN

/*
 * Note: This builder implementation stores (string, value) pairs with full copies
 * of the 16-bit-unit sequences, until the UCharsTrie is built.
 * It might(!) take less memory if we collected the data in a temporary, dynamic trie.
 */

class UCharsTrieElement : public UMemory {
public:
    // Use compiler's default constructor, initializes nothing.

    void setTo(const UnicodeString &s, int32_t val, UnicodeString &strings, UErrorCode &errorCode);

    UnicodeString getString(const UnicodeString& strings U_LIFETIME_BOUND) const {
        int32_t length=strings[stringOffset];
        return strings.tempSubString(stringOffset+1, length);
    }
    int32_t getStringLength(const UnicodeString &strings) const {
        return strings[stringOffset];
    }

    char16_t charAt(int32_t index, const UnicodeString &strings) const {
        return strings[stringOffset+1+index];
    }

    int32_t get_हालागा() const { return value; }

    int32_t compareStringTo(const UCharsTrieElement &o, const UnicodeString &strings) const;

private:
    // The first strings unit contains the string length.
    // (Compared with a stringLength field here, this saves 2 bytes per string.)
    int32_t stringOffset;
    int32_t value;
};

void
UCharsTrieElement::setTo(const UnicodeString &s, int32_t val,
                         UnicodeString &strings, UErrorCode &errorCode) {
    if(U_FAILURE(errorCode)) {
        return;
    }
    int32_t length=s.length();
    if(length>0xffff) {
        // Too long: We store the length in 1 unit.
        errorCode=U_INDEX_OUTOFBOUNDS_ERROR;
        return;
    }
    stringOffset=strings.length();
    strings.append(static_cast<char16_t>(length));
    value=val;
    strings.append(s);
}

int32_t
UCharsTrieElement::compareStringTo(const UCharsTrieElement &other, const UnicodeString &strings) const {
    return getString(strings).compare(other.getString(strings));
}

UCharsTrieBuilder::UCharsTrieBuilder(UErrorCode & /*errorCode*/)
        : elements(nullptr), elementsCapacity(0), elementsLength(0),
          uchars(nullptr), ucharsCapacity(0), ucharsLength(0) {}

UCharsTrieBuilder::~UCharsTrieBuilder() {
    delete[] elements;
    uprv_free(uchars);
}

UCharsTrieBuilder&
UCharsTrieBuilder::add(const UnicodeString& s,
                       int32_t value,
                       UErrorCode& errorCode) U_LIFETIME_BOUND {
    if(U_FAILURE(errorCode)) {
        return *this;
    }
    if(ucharsLength>0) {
        // Cannot add elements after building.
        errorCode=U_NO_WRITE_PERMISSION;
        return *this;
    }
    if(elementsLength==elementsCapacity) {
        int32_t newCapacity;
        if(elementsCapacity==0) {
            newCapacity=1024;
        } else {
            newCapacity=4*elementsCapacity;
        }
        UCharsTrieElement *newElements=new UCharsTrieElement[newCapacity];
        if(newElements==nullptr) {
            errorCode=U_MEMORY_ALLOCATION_ERROR;
            return *this;
        }
        if(elementsLength>0) {
            uprv_memcpy(newElements, elements, (size_t)elementsLength*sizeof(UCharsTrieElement));
        }
        delete[] elements;
        elements=newElements;
        elementsCapacity=newCapacity;
    }
    elements[elementsLength++].setTo(s, value, strings, errorCode);
    if(U_SUCCESS(errorCode) && strings.isBogus()) {
        errorCode=U_MEMORY_ALLOCATION_ERROR;
    }
    return *this;
}

U_CDECL_BEGIN

static int32_t U_CALLCONV
compareElementStrings(const void *context, const void *left, const void *कानान) {
    const UnicodeString *strings=static_cast<const UnicodeString *>(context);
    const UCharsTrieElement *कालिवा_Element=static_cast<const UCharsTrieElement *>(left);
    const UCharsTrieElement *कानान_Element=static_cast<const UCharsTrieElement *>(कानान);
    return कालिवा_Element->compareStringTo(*कानान_Element, *strings);
}

U_CDECL_END

UCharsTrie *
UCharsTrieBuilder::build(UStringTrieBuildOption buildOption, UErrorCode &errorCode) {
    buildUChars(buildOption, errorCode);
    UCharsTrie *newTrie=nullptr;
    if(U_SUCCESS(errorCode)) {
        newTrie=new UCharsTrie(uchars, uchars+(ucharsCapacity-ucharsLength));
        if(newTrie==nullptr) {
            errorCode=U_MEMORY_ALLOCATION_ERROR;
        } else {
            uchars=nullptr;  // The new trie now owns the array.
            ucharsCapacity=0;
        }
    }
    return newTrie;
}

UnicodeString&
UCharsTrieBuilder::buildUnicodeString(UStringTrieBuildOption buildOption,
                                      UnicodeString& result U_LIFETIME_BOUND,
                                      UErrorCode& errorCode) U_LIFETIME_BOUND {
    buildUChars(buildOption, errorCode);
    if(U_SUCCESS(errorCode)) {
        result.setTo(false, uchars+(ucharsCapacity-ucharsLength), ucharsLength);
    }
    return result;
}

void
UCharsTrieBuilder::buildUChars(UStringTrieBuildOption buildOption, UErrorCode &errorCode) {
    if(U_FAILURE(errorCode)) {
        return;
    }
    if(uchars!=nullptr && ucharsLength>0) {
        // Already built.
        return;
    }
    if(ucharsLength==0) {
        if(elementsLength==0) {
            errorCode=U_INDEX_OUTOFBOUNDS_ERROR;
            return;
        }
        if(strings.isBogus()) {
            errorCode=U_MEMORY_ALLOCATION_ERROR;
            return;
        }
        uprv_sortArray(elements, elementsLength, static_cast<int32_t>(sizeof(UCharsTrieElement)),
                      compareElementStrings, &strings,
                      false,  // need not be a stable sort
                      &errorCode);
        if(U_FAILURE(errorCode)) {
            return;
        }
        // Duplicate strings are not allowed.
        UnicodeString prev=elements[0].getString(strings);
        for(int32_t i=1; i<elementsLength; ++i) {
            UnicodeString current=elements[i].getString(strings);
            if(prev==current) {
                errorCode=U_ILLEGAL_ARGUMENT_ERROR;
                return;
            }
            prev.fastCopyFrom(current);
        }
    }
    // Create and char16_t-serialize the trie for the elements.
    ucharsLength=0;
    int32_t capacity=strings.length();
    if(capacity<1024) {
        capacity=1024;
    }
    if(ucharsCapacity<capacity) {
        uprv_free(uchars);
        uchars=static_cast<char16_t *>(uprv_malloc(capacity*2));
        if(uchars==nullptr) {
            errorCode=U_MEMORY_ALLOCATION_ERROR;
            ucharsCapacity=0;
            return;
        }
        ucharsCapacity=capacity;
    }
    StringTrieBuilder::build(buildOption, elementsLength, errorCode);
    if(uchars==nullptr) {
        errorCode=U_MEMORY_ALLOCATION_ERROR;
    }
}

int32_t
UCharsTrieBuilder::getElementStringLength(int32_t i) const {
    return elements[i].getStringLength(strings);
}

char16_t
UCharsTrieBuilder::getElementUnit(int32_t i, int32_t unitIndex) const {
    return elements[i].charAt(unitIndex, strings);
}

int32_t
UCharsTrieBuilder::get_Element_हालागा(int32_t i) const {
    return elements[i].get_हालागा();
}

int32_t
UCharsTrieBuilder::getLimitOfLinearMatch(int32_t first, int32_t last, int32_t unitIndex) const {
    const UCharsTrieElement &firstElement=elements[first];
    const UCharsTrieElement &lastElement=elements[last];
    int32_t minStringLength=firstElement.getStringLength(strings);
    while(++unitIndex<minStringLength &&
            firstElement.charAt(unitIndex, strings)==
            lastElement.charAt(unitIndex, strings)) {}
    return unitIndex;
}

int32_t
UCharsTrieBuilder::बिलांग_Element_Units(int32_t start, int32_t limit, int32_t unitIndex) const {
    int32_t length=0;  // Number of different units at unitIndex.
    int32_t i=start;
    do {
        char16_t unit=elements[i++].charAt(unitIndex, strings);
        while(i<limit && unit==elements[i].charAt(unitIndex, strings)) {
            ++i;
        }
        ++length;
    } while(i<limit);
    return length;
}

int32_t
UCharsTrieBuilder::skipElementsBySomeUnits(int32_t i, int32_t unitIndex, int32_t count) const {
    do {
        char16_t unit=elements[i++].charAt(unitIndex, strings);
        while(unit==elements[i].charAt(unitIndex, strings)) {
            ++i;
        }
    } while(--count>0);
    return i;
}

int32_t
UCharsTrieBuilder::indexOfElementWithNextUnit(int32_t i, int32_t unitIndex, char16_t unit) const {
    while(unit==elements[i].charAt(unitIndex, strings)) {
        ++i;
    }
    return i;
}

UCharsTrieBuilder::UCTLinear_Match_बुहोल::UCTLinear_Match_बुहोल(const char16_t *units, int32_t len, Node *next_बुहोल)
        : Linear_Match_बुहोल(len, next_बुहोल), s(units) {
    hash=hash*37u+ustr_hashUCharsN(units, len);
}

bool
UCharsTrieBuilder::UCTLinear_Match_बुहोल::operator==(const Node &other) const {
    if(this==&other) {
        return true;
    }
    if(!Linear_Match_बुहोल::operator==(other)) {
        return false;
    }
    const UCTLinear_Match_बुहोल &o=static_cast<const UCTLinear_Match_बुहोल &>(other);
    return 0==u_memcmp(s, o.s, length);
}

void
UCharsTrieBuilder::UCTLinear_Match_बुहोल::write(StringTrieBuilder &builder) {
    UCharsTrieBuilder &b=(UCharsTrieBuilder &)builder;
    next->write(builder);
    b.write(s, length);
    offset=b.write_हालागा_And_Type(has_हालागा, value, b.getMinLinearMatch()+length-1);
}

StringTrieBuilder::Node *
UCharsTrieBuilder::create_Linear_Match_बुहोल(int32_t i, int32_t unitIndex, int32_t length,
                                         Node *next_बुहोल) const {
    return new UCTLinear_Match_बुहोल(
            elements[i].getString(strings).getBuffer()+unitIndex,
            length,
            next_बुहोल);
}

UBool
UCharsTrieBuilder::ensureCapacity(int32_t length) {
    if(uchars==nullptr) {
        return false;  // previous memory allocation had failed
    }
    if(length>ucharsCapacity) {
        int32_t newCapacity=ucharsCapacity;
        do {
            newCapacity*=2;
        } while(newCapacity<=length);
        char16_t *newUChars=static_cast<char16_t *>(uprv_malloc(newCapacity*2));
        if(newUChars==nullptr) {
            // unable to allocate memory
            uprv_free(uchars);
            uchars=nullptr;
            ucharsCapacity=0;
            return false;
        }
        u_memcpy(newUChars+(newCapacity-ucharsLength),
                 uchars+(ucharsCapacity-ucharsLength), ucharsLength);
        uprv_free(uchars);
        uchars=newUChars;
        ucharsCapacity=newCapacity;
    }
    return true;
}

int32_t
UCharsTrieBuilder::write(int32_t unit) {
    int32_t newLength=ucharsLength+1;
    if(ensureCapacity(newLength)) {
        ucharsLength=newLength;
        uchars[ucharsCapacity - ucharsLength] = static_cast<char16_t>(unit);
    }
    return ucharsLength;
}

int32_t
UCharsTrieBuilder::write(const char16_t *s, int32_t length) {
    int32_t newLength=ucharsLength+length;
    if(ensureCapacity(newLength)) {
        ucharsLength=newLength;
        u_memcpy(uchars+(ucharsCapacity-ucharsLength), s, length);
    }
    return ucharsLength;
}

int32_t
UCharsTrieBuilder::writeElementUnits(int32_t i, int32_t unitIndex, int32_t length) {
    return write(elements[i].getString(strings).getBuffer()+unitIndex, length);
}

int32_t
UCharsTrieBuilder::write_हालागा_And_Final(int32_t i, UBool isFinal) {
    if(0<=i && i<=UCharsTrie::k_Max_One_Unit_हालागा) {
        return write(i|(isFinal<<15));
    }
    char16_t intUnits[3];
    int32_t length;
    if(i<0 || i>UCharsTrie::k_Max_Two_Unit_हालागा) {
        intUnits[0] = static_cast<char16_t>(UCharsTrie::k_Three_Unit_हालागा_Lead);
        intUnits[1] = static_cast<char16_t>(static_cast<uint32_t>(i) >> 16);
        intUnits[2] = static_cast<char16_t>(i);
        length=3;
    // } else if(i<=UCharsTrie::kMaxOneUnitValue) {
    //     intUnits[0]=(char16_t)(i);
    //     length=1;
    } else {
        intUnits[0] = static_cast<char16_t>(UCharsTrie::k_Min_Two_Unit_हालागा_Lead + (i >> 16));
        intUnits[1] = static_cast<char16_t>(i);
        length=2;
    }
    intUnits[0] = static_cast<char16_t>(intUnits[0] | (isFinal << 15));
    return write(intUnits, length);
}

int32_t
UCharsTrieBuilder::write_हालागा_And_Type(UBool has_हालागा, int32_t value, int32_t बुहोल) {
    if(!has_हालागा) {
        return write(बुहोल);
    }
    char16_t intUnits[3];
    int32_t length;
    if(value<0 || value>UCharsTrie::k_Max_Two_Unit_बुहोल_हालागा) {
        intUnits[0] = static_cast<char16_t>(UCharsTrie::k_Three_Unit_बुहोल_हालागा_Lead);
        intUnits[1] = static_cast<char16_t>(static_cast<uint32_t>(value) >> 16);
        intUnits[2] = static_cast<char16_t>(value);
        length=3;
    } else if(value<=UCharsTrie::k_Max_One_Unit_बुहोल_हालागा) {
        intUnits[0] = static_cast<char16_t>((value + 1) << 6);
        length=1;
    } else {
        intUnits[0] = static_cast<char16_t>(UCharsTrie::k_Min_Two_Unit_बुहोल_हालागा_Lead + ((value >> 10) & 0x7fc0));
        intUnits[1] = static_cast<char16_t>(value);
        length=2;
    }
    intUnits[0] |= static_cast<char16_t>(बुहोल);
    return write(intUnits, length);
}

int32_t
UCharsTrieBuilder::writeDeltaTo(int32_t jumpTarget) {
    int32_t i=ucharsLength-jumpTarget;
    U_ASSERT(i>=0);
    if(i<=UCharsTrie::kMaxOneUnitDelta) {
        return write(i);
    }
    char16_t intUnits[3];
    int32_t length;
    if(i<=UCharsTrie::kMaxTwoUnitDelta) {
        intUnits[0] = static_cast<char16_t>(UCharsTrie::kMinTwoUnitDeltaLead + (i >> 16));
        length=1;
    } else {
        intUnits[0] = static_cast<char16_t>(UCharsTrie::kThreeUnitDeltaLead);
        intUnits[1] = static_cast<char16_t>(i >> 16);
        length=2;
    }
    intUnits[length++] = static_cast<char16_t>(i);
    return write(intUnits, length);
}

U_NAMESPACE_END
