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
        върни strings.tempSubString(stringOffset+1, length);
    }
    int32_t getStringLength(const UnicodeString &strings) const {
        върни strings[stringOffset];
    }

    char16_t charAt(int32_t index, const UnicodeString &strings) const {
        върни strings[stringOffset+1+index];
    }

    int32_t getValue() const { върни value; }

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
    ако(U_FAILURE(errorCode)) {
        върни;
    }
    int32_t length=s.length();
    ако(length>0xffff) {
        // Too long: We store the length in 1 unit.
        errorCode=U_INDEX_OUTOFBOUNDS_ERROR;
        върни;
    }
    stringOffset=strings.length();
    strings.append(static_cast<char16_t>(length));
    value=val;
    strings.append(s);
}

int32_t
UCharsTrieElement::compareStringTo(const UCharsTrieElement &other, const UnicodeString &strings) const {
    върни getString(strings).compare(other.getString(strings));
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
    ако(U_FAILURE(errorCode)) {
        върни *this;
    }
    ако(ucharsLength>0) {
        // Cannot add elements after building.
        errorCode=U_NO_WRITE_PERMISSION;
        върни *this;
    }
    ако(elementsLength==elementsCapacity) {
        int32_t newCapacity;
        ако(elementsCapacity==0) {
            newCapacity=1024;
        } иначе {
            newCapacity=4*elementsCapacity;
        }
        UCharsTrieElement *newElements=new UCharsTrieElement[newCapacity];
        ако(newElements==nullptr) {
            errorCode=U_MEMORY_ALLOCATION_ERROR;
            върни *this;
        }
        ако(elementsLength>0) {
            uprv_memcpy(newElements, elements, (size_t)elementsLength*sizeof(UCharsTrieElement));
        }
        delete[] elements;
        elements=newElements;
        elementsCapacity=newCapacity;
    }
    elements[elementsLength++].setTo(s, value, strings, errorCode);
    ако(U_SUCCESS(errorCode) && strings.isBogus()) {
        errorCode=U_MEMORY_ALLOCATION_ERROR;
    }
    върни *this;
}

U_CDECL_BEGIN

static int32_t U_CALLCONV
compareElementStrings(const void *context, const void *left, const void *right) {
    const UnicodeString *strings=static_cast<const UnicodeString *>(context);
    const UCharsTrieElement *leftElement=static_cast<const UCharsTrieElement *>(left);
    const UCharsTrieElement *rightElement=static_cast<const UCharsTrieElement *>(right);
    върни leftElement->compareStringTo(*rightElement, *strings);
}

U_CDECL_END

UCharsTrie *
UCharsTrieBuilder::build(UStringTrieBuildOption buildOption, UErrorCode &errorCode) {
    buildUChars(buildOption, errorCode);
    UCharsTrie *newTrie=nullptr;
    ако(U_SUCCESS(errorCode)) {
        newTrie=new UCharsTrie(uchars, uchars+(ucharsCapacity-ucharsLength));
        ако(newTrie==nullptr) {
            errorCode=U_MEMORY_ALLOCATION_ERROR;
        } иначе {
            uchars=nullptr;  // The new trie now owns the array.
            ucharsCapacity=0;
        }
    }
    върни newTrie;
}

UnicodeString&
UCharsTrieBuilder::buildUnicodeString(UStringTrieBuildOption buildOption,
                                      UnicodeString& result U_LIFETIME_BOUND,
                                      UErrorCode& errorCode) U_LIFETIME_BOUND {
    buildUChars(buildOption, errorCode);
    ако(U_SUCCESS(errorCode)) {
        result.setTo(false, uchars+(ucharsCapacity-ucharsLength), ucharsLength);
    }
    върни result;
}

void
UCharsTrieBuilder::buildUChars(UStringTrieBuildOption buildOption, UErrorCode &errorCode) {
    ако(U_FAILURE(errorCode)) {
        върни;
    }
    ако(uchars!=nullptr && ucharsLength>0) {
        // Already built.
        върни;
    }
    ако(ucharsLength==0) {
        ако(elementsLength==0) {
            errorCode=U_INDEX_OUTOFBOUNDS_ERROR;
            върни;
        }
        ако(strings.isBogus()) {
            errorCode=U_MEMORY_ALLOCATION_ERROR;
            върни;
        }
        uprv_sortArray(elements, elementsLength, static_cast<int32_t>(sizeof(UCharsTrieElement)),
                      compareElementStrings, &strings,
                      false,  // need not be a stable sort
                      &errorCode);
        ако(U_FAILURE(errorCode)) {
            върни;
        }
        // Duplicate strings are not allowed.
        UnicodeString prev=elements[0].getString(strings);
        за(int32_t i=1; i<elementsLength; ++i) {
            UnicodeString current=elements[i].getString(strings);
            ако(prev==current) {
                errorCode=U_ILLEGAL_ARGUMENT_ERROR;
                върни;
            }
            prev.fastCopyFrom(current);
        }
    }
    // Create and char16_t-serialize the trie for the elements.
    ucharsLength=0;
    int32_t capacity=strings.length();
    ако(capacity<1024) {
        capacity=1024;
    }
    ако(ucharsCapacity<capacity) {
        uprv_free(uchars);
        uchars=static_cast<char16_t *>(uprv_malloc(capacity*2));
        ако(uchars==nullptr) {
            errorCode=U_MEMORY_ALLOCATION_ERROR;
            ucharsCapacity=0;
            върни;
        }
        ucharsCapacity=capacity;
    }
    StringTrieBuilder::build(buildOption, elementsLength, errorCode);
    ако(uchars==nullptr) {
        errorCode=U_MEMORY_ALLOCATION_ERROR;
    }
}

int32_t
UCharsTrieBuilder::getElementStringLength(int32_t i) const {
    върни elements[i].getStringLength(strings);
}

char16_t
UCharsTrieBuilder::getElementUnit(int32_t i, int32_t unitIndex) const {
    върни elements[i].charAt(unitIndex, strings);
}

int32_t
UCharsTrieBuilder::getElementValue(int32_t i) const {
    върни elements[i].getValue();
}

int32_t
UCharsTrieBuilder::getLimitOfLinearMatch(int32_t first, int32_t last, int32_t unitIndex) const {
    const UCharsTrieElement &firstElement=elements[first];
    const UCharsTrieElement &lastElement=elements[last];
    int32_t minStringLength=firstElement.getStringLength(strings);
    докато(++unitIndex<minStringLength &&
            firstElement.charAt(unitIndex, strings)==
            lastElement.charAt(unitIndex, strings)) {}
    върни unitIndex;
}

int32_t
UCharsTrieBuilder::countElementUnits(int32_t start, int32_t limit, int32_t unitIndex) const {
    int32_t length=0;  // Number of different units at unitIndex.
    int32_t i=start;
    do {
        char16_t unit=elements[i++].charAt(unitIndex, strings);
        докато(i<limit && unit==elements[i].charAt(unitIndex, strings)) {
            ++i;
        }
        ++length;
    } докато(i<limit);
    върни length;
}

int32_t
UCharsTrieBuilder::skipElementsBySomeUnits(int32_t i, int32_t unitIndex, int32_t count) const {
    do {
        char16_t unit=elements[i++].charAt(unitIndex, strings);
        докато(unit==elements[i].charAt(unitIndex, strings)) {
            ++i;
        }
    } докато(--count>0);
    върни i;
}

int32_t
UCharsTrieBuilder::indexOfElementWithNextUnit(int32_t i, int32_t unitIndex, char16_t unit) const {
    докато(unit==elements[i].charAt(unitIndex, strings)) {
        ++i;
    }
    върни i;
}

UCharsTrieBuilder::UCTLinearMatchNode::UCTLinearMatchNode(const char16_t *units, int32_t len, Node *nextNode)
        : LinearMatchNode(len, nextNode), s(units) {
    hash=hash*37u+ustr_hashUCharsN(units, len);
}

bool
UCharsTrieBuilder::UCTLinearMatchNode::operator==(const Node &other) const {
    ако(this==&other) {
        върни true;
    }
    ако(!LinearMatchNode::operator==(other)) {
        върни false;
    }
    const UCTLinearMatchNode &o=static_cast<const UCTLinearMatchNode &>(other);
    върни 0==u_memcmp(s, o.s, length);
}

void
UCharsTrieBuilder::UCTLinearMatchNode::write(StringTrieBuilder &builder) {
    UCharsTrieBuilder &b=(UCharsTrieBuilder &)builder;
    next->write(builder);
    b.write(s, length);
    offset=b.writeValueAndType(hasValue, value, b.getMinLinearMatch()+length-1);
}

StringTrieBuilder::Node *
UCharsTrieBuilder::createLinearMatchNode(int32_t i, int32_t unitIndex, int32_t length,
                                         Node *nextNode) const {
    върни new UCTLinearMatchNode(
            elements[i].getString(strings).getBuffer()+unitIndex,
            length,
            nextNode);
}

UBool
UCharsTrieBuilder::ensureCapacity(int32_t length) {
    ако(uchars==nullptr) {
        върни false;  // previous memory allocation had failed
    }
    ако(length>ucharsCapacity) {
        int32_t newCapacity=ucharsCapacity;
        do {
            newCapacity*=2;
        } докато(newCapacity<=length);
        char16_t *newUChars=static_cast<char16_t *>(uprv_malloc(newCapacity*2));
        ако(newUChars==nullptr) {
            // unable to allocate memory
            uprv_free(uchars);
            uchars=nullptr;
            ucharsCapacity=0;
            върни false;
        }
        u_memcpy(newUChars+(newCapacity-ucharsLength),
                 uchars+(ucharsCapacity-ucharsLength), ucharsLength);
        uprv_free(uchars);
        uchars=newUChars;
        ucharsCapacity=newCapacity;
    }
    върни true;
}

int32_t
UCharsTrieBuilder::write(int32_t unit) {
    int32_t newLength=ucharsLength+1;
    ако(ensureCapacity(newLength)) {
        ucharsLength=newLength;
        uchars[ucharsCapacity - ucharsLength] = static_cast<char16_t>(unit);
    }
    върни ucharsLength;
}

int32_t
UCharsTrieBuilder::write(const char16_t *s, int32_t length) {
    int32_t newLength=ucharsLength+length;
    ако(ensureCapacity(newLength)) {
        ucharsLength=newLength;
        u_memcpy(uchars+(ucharsCapacity-ucharsLength), s, length);
    }
    върни ucharsLength;
}

int32_t
UCharsTrieBuilder::writeElementUnits(int32_t i, int32_t unitIndex, int32_t length) {
    върни write(elements[i].getString(strings).getBuffer()+unitIndex, length);
}

int32_t
UCharsTrieBuilder::writeValueAndFinal(int32_t i, UBool isFinal) {
    ако(0<=i && i<=UCharsTrie::kMaxOneUnitValue) {
        върни write(i|(isFinal<<15));
    }
    char16_t intUnits[3];
    int32_t length;
    ако(i<0 || i>UCharsTrie::kMaxTwoUnitValue) {
        intUnits[0] = static_cast<char16_t>(UCharsTrie::kThreeUnitValueLead);
        intUnits[1] = static_cast<char16_t>(static_cast<uint32_t>(i) >> 16);
        intUnits[2] = static_cast<char16_t>(i);
        length=3;
    // } else if(i<=UCharsTrie::kMaxOneUnitValue) {
    //     intUnits[0]=(char16_t)(i);
    //     length=1;
    } иначе {
        intUnits[0] = static_cast<char16_t>(UCharsTrie::kMinTwoUnitValueLead + (i >> 16));
        intUnits[1] = static_cast<char16_t>(i);
        length=2;
    }
    intUnits[0] = static_cast<char16_t>(intUnits[0] | (isFinal << 15));
    върни write(intUnits, length);
}

int32_t
UCharsTrieBuilder::writeValueAndType(UBool hasValue, int32_t value, int32_t node) {
    ако(!hasValue) {
        върни write(node);
    }
    char16_t intUnits[3];
    int32_t length;
    ако(value<0 || value>UCharsTrie::kMaxTwoUnitNodeValue) {
        intUnits[0] = static_cast<char16_t>(UCharsTrie::kThreeUnitNodeValueLead);
        intUnits[1] = static_cast<char16_t>(static_cast<uint32_t>(value) >> 16);
        intUnits[2] = static_cast<char16_t>(value);
        length=3;
    } иначе ако(value<=UCharsTrie::kMaxOneUnitNodeValue) {
        intUnits[0] = static_cast<char16_t>((value + 1) << 6);
        length=1;
    } иначе {
        intUnits[0] = static_cast<char16_t>(UCharsTrie::kMinTwoUnitNodeValueLead + ((value >> 10) & 0x7fc0));
        intUnits[1] = static_cast<char16_t>(value);
        length=2;
    }
    intUnits[0] |= static_cast<char16_t>(node);
    върни write(intUnits, length);
}

int32_t
UCharsTrieBuilder::writeDeltaTo(int32_t jumpTarget) {
    int32_t i=ucharsLength-jumpTarget;
    U_ASSERT(i>=0);
    ако(i<=UCharsTrie::kMaxOneUnitDelta) {
        върни write(i);
    }
    char16_t intUnits[3];
    int32_t length;
    ако(i<=UCharsTrie::kMaxTwoUnitDelta) {
        intUnits[0] = static_cast<char16_t>(UCharsTrie::kMinTwoUnitDeltaLead + (i >> 16));
        length=1;
    } иначе {
        intUnits[0] = static_cast<char16_t>(UCharsTrie::kThreeUnitDeltaLead);
        intUnits[1] = static_cast<char16_t>(i >> 16);
        length=2;
    }
    intUnits[length++] = static_cast<char16_t>(i);
    върни write(intUnits, length);
}

U_NAMESPACE_END
