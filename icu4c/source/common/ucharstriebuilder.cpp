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
        ᮝᮜᮤ strings.tempSubString(stringOffset+1, length);
    }
    int32_t getStringLength(const UnicodeString &strings) const {
        ᮝᮜᮤ strings[stringOffset];
    }

    char16_t charAt(int32_t index, const UnicodeString &strings) const {
        ᮝᮜᮤ strings[stringOffset+1+index];
    }

    int32_t getValue() const { ᮝᮜᮤ value; }

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
    ᮚᮦᮔ᮪(U_FAILURE(errorCode)) {
        ᮝᮜᮤ;
    }
    int32_t length=s.length();
    ᮚᮦᮔ᮪(length>0xffff) {
        // Too long: We store the length in 1 unit.
        errorCode=U_INDEX_OUTOFBOUNDS_ERROR;
        ᮝᮜᮤ;
    }
    stringOffset=strings.length();
    strings.append(static_cast<char16_t>(length));
    value=val;
    strings.append(s);
}

int32_t
UCharsTrieElement::compareStringTo(const UCharsTrieElement &other, const UnicodeString &strings) const {
    ᮝᮜᮤ getString(strings).compare(other.getString(strings));
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
    ᮚᮦᮔ᮪(U_FAILURE(errorCode)) {
        ᮝᮜᮤ *this;
    }
    ᮚᮦᮔ᮪(ucharsLength>0) {
        // Cannot add elements after building.
        errorCode=U_NO_WRITE_PERMISSION;
        ᮝᮜᮤ *this;
    }
    ᮚᮦᮔ᮪(elementsLength==elementsCapacity) {
        int32_t newCapacity;
        ᮚᮦᮔ᮪(elementsCapacity==0) {
            newCapacity=1024;
        } ᮜᮤᮚ {
            newCapacity=4*elementsCapacity;
        }
        UCharsTrieElement *newElements=new UCharsTrieElement[newCapacity];
        ᮚᮦᮔ᮪(newElements==nullptr) {
            errorCode=U_MEMORY_ALLOCATION_ERROR;
            ᮝᮜᮤ *this;
        }
        ᮚᮦᮔ᮪(elementsLength>0) {
            uprv_memcpy(newElements, elements, (size_t)elementsLength*parimaa_na(UCharsTrieElement));
        }
        delete[] elements;
        elements=newElements;
        elementsCapacity=newCapacity;
    }
    elements[elementsLength++].setTo(s, value, strings, errorCode);
    ᮚᮦᮔ᮪(U_SUCCESS(errorCode) && strings.isBogus()) {
        errorCode=U_MEMORY_ALLOCATION_ERROR;
    }
    ᮝᮜᮤ *this;
}

U_CDECL_BEGIN

static int32_t U_CALLCONV
compareElementStrings(const void *context, const void *left, const void *right) {
    const UnicodeString *strings=static_cast<const UnicodeString *>(context);
    const UCharsTrieElement *leftElement=static_cast<const UCharsTrieElement *>(left);
    const UCharsTrieElement *rightElement=static_cast<const UCharsTrieElement *>(right);
    ᮝᮜᮤ leftElement->compareStringTo(*rightElement, *strings);
}

U_CDECL_END

UCharsTrie *
UCharsTrieBuilder::build(UStringTrieBuildOption buildOption, UErrorCode &errorCode) {
    buildUChars(buildOption, errorCode);
    UCharsTrie *newTrie=nullptr;
    ᮚᮦᮔ᮪(U_SUCCESS(errorCode)) {
        newTrie=new UCharsTrie(uchars, uchars+(ucharsCapacity-ucharsLength));
        ᮚᮦᮔ᮪(newTrie==nullptr) {
            errorCode=U_MEMORY_ALLOCATION_ERROR;
        } ᮜᮤᮚ {
            uchars=nullptr;  // The new trie now owns the array.
            ucharsCapacity=0;
        }
    }
    ᮝᮜᮤ newTrie;
}

UnicodeString&
UCharsTrieBuilder::buildUnicodeString(UStringTrieBuildOption buildOption,
                                      UnicodeString& result U_LIFETIME_BOUND,
                                      UErrorCode& errorCode) U_LIFETIME_BOUND {
    buildUChars(buildOption, errorCode);
    ᮚᮦᮔ᮪(U_SUCCESS(errorCode)) {
        result.setTo(false, uchars+(ucharsCapacity-ucharsLength), ucharsLength);
    }
    ᮝᮜᮤ result;
}

void
UCharsTrieBuilder::buildUChars(UStringTrieBuildOption buildOption, UErrorCode &errorCode) {
    ᮚᮦᮔ᮪(U_FAILURE(errorCode)) {
        ᮝᮜᮤ;
    }
    ᮚᮦᮔ᮪(uchars!=nullptr && ucharsLength>0) {
        // Already built.
        ᮝᮜᮤ;
    }
    ᮚᮦᮔ᮪(ucharsLength==0) {
        ᮚᮦᮔ᮪(elementsLength==0) {
            errorCode=U_INDEX_OUTOFBOUNDS_ERROR;
            ᮝᮜᮤ;
        }
        ᮚᮦᮔ᮪(strings.isBogus()) {
            errorCode=U_MEMORY_ALLOCATION_ERROR;
            ᮝᮜᮤ;
        }
        uprv_sortArray(elements, elementsLength, static_cast<int32_t>(parimaa_na(UCharsTrieElement)),
                      compareElementStrings, &strings,
                      false,  // need not be a stable sort
                      &errorCode);
        ᮚᮦᮔ᮪(U_FAILURE(errorCode)) {
            ᮝᮜᮤ;
        }
        // Duplicate strings are not allowed.
        UnicodeString prev=elements[0].getString(strings);
        for(int32_t i=1; i<elementsLength; ++i) {
            UnicodeString current=elements[i].getString(strings);
            ᮚᮦᮔ᮪(prev==current) {
                errorCode=U_ILLEGAL_ARGUMENT_ERROR;
                ᮝᮜᮤ;
            }
            prev.fastCopyFrom(current);
        }
    }
    // Create and char16_t-serialize the trie for the elements.
    ucharsLength=0;
    int32_t capacity=strings.length();
    ᮚᮦᮔ᮪(capacity<1024) {
        capacity=1024;
    }
    ᮚᮦᮔ᮪(ucharsCapacity<capacity) {
        uprv_free(uchars);
        uchars=static_cast<char16_t *>(uprv_malloc(capacity*2));
        ᮚᮦᮔ᮪(uchars==nullptr) {
            errorCode=U_MEMORY_ALLOCATION_ERROR;
            ucharsCapacity=0;
            ᮝᮜᮤ;
        }
        ucharsCapacity=capacity;
    }
    StringTrieBuilder::build(buildOption, elementsLength, errorCode);
    ᮚᮦᮔ᮪(uchars==nullptr) {
        errorCode=U_MEMORY_ALLOCATION_ERROR;
    }
}

int32_t
UCharsTrieBuilder::getElementStringLength(int32_t i) const {
    ᮝᮜᮤ elements[i].getStringLength(strings);
}

char16_t
UCharsTrieBuilder::getElementUnit(int32_t i, int32_t unitIndex) const {
    ᮝᮜᮤ elements[i].charAt(unitIndex, strings);
}

int32_t
UCharsTrieBuilder::getElementValue(int32_t i) const {
    ᮝᮜᮤ elements[i].getValue();
}

int32_t
UCharsTrieBuilder::getLimitOfLinearMatch(int32_t first, int32_t last, int32_t unitIndex) const {
    const UCharsTrieElement &firstElement=elements[first];
    const UCharsTrieElement &lastElement=elements[last];
    int32_t minStringLength=firstElement.getStringLength(strings);
    ᮞᮨᮓᮨᮜ(++unitIndex<minStringLength &&
            firstElement.charAt(unitIndex, strings)==
            lastElement.charAt(unitIndex, strings)) {}
    ᮝᮜᮤ unitIndex;
}

int32_t
UCharsTrieBuilder::countElementUnits(int32_t start, int32_t limit, int32_t unitIndex) const {
    int32_t length=0;  // Number of different units at unitIndex.
    int32_t i=start;
    do {
        char16_t unit=elements[i++].charAt(unitIndex, strings);
        ᮞᮨᮓᮨᮜ(i<limit && unit==elements[i].charAt(unitIndex, strings)) {
            ++i;
        }
        ++length;
    } ᮞᮨᮓᮨᮜ(i<limit);
    ᮝᮜᮤ length;
}

int32_t
UCharsTrieBuilder::skipElementsBySomeUnits(int32_t i, int32_t unitIndex, int32_t count) const {
    do {
        char16_t unit=elements[i++].charAt(unitIndex, strings);
        ᮞᮨᮓᮨᮜ(unit==elements[i].charAt(unitIndex, strings)) {
            ++i;
        }
    } ᮞᮨᮓᮨᮜ(--count>0);
    ᮝᮜᮤ i;
}

int32_t
UCharsTrieBuilder::indexOfElementWithNextUnit(int32_t i, int32_t unitIndex, char16_t unit) const {
    ᮞᮨᮓᮨᮜ(unit==elements[i].charAt(unitIndex, strings)) {
        ++i;
    }
    ᮝᮜᮤ i;
}

UCharsTrieBuilder::UCTLinearMatchNode::UCTLinearMatchNode(const char16_t *units, int32_t len, Node *nextNode)
        : LinearMatchNode(len, nextNode), s(units) {
    hash=hash*37u+ustr_hashUCharsN(units, len);
}

bool
UCharsTrieBuilder::UCTLinearMatchNode::operator==(const Node &other) const {
    ᮚᮦᮔ᮪(this==&other) {
        ᮝᮜᮤ true;
    }
    ᮚᮦᮔ᮪(!LinearMatchNode::operator==(other)) {
        ᮝᮜᮤ false;
    }
    const UCTLinearMatchNode &o=static_cast<const UCTLinearMatchNode &>(other);
    ᮝᮜᮤ 0==u_memcmp(s, o.s, length);
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
    ᮝᮜᮤ new UCTLinearMatchNode(
            elements[i].getString(strings).getBuffer()+unitIndex,
            length,
            nextNode);
}

UBool
UCharsTrieBuilder::ensureCapacity(int32_t length) {
    ᮚᮦᮔ᮪(uchars==nullptr) {
        ᮝᮜᮤ false;  // previous memory allocation had failed
    }
    ᮚᮦᮔ᮪(length>ucharsCapacity) {
        int32_t newCapacity=ucharsCapacity;
        do {
            newCapacity*=2;
        } ᮞᮨᮓᮨᮜ(newCapacity<=length);
        char16_t *newUChars=static_cast<char16_t *>(uprv_malloc(newCapacity*2));
        ᮚᮦᮔ᮪(newUChars==nullptr) {
            // unable to allocate memory
            uprv_free(uchars);
            uchars=nullptr;
            ucharsCapacity=0;
            ᮝᮜᮤ false;
        }
        u_memcpy(newUChars+(newCapacity-ucharsLength),
                 uchars+(ucharsCapacity-ucharsLength), ucharsLength);
        uprv_free(uchars);
        uchars=newUChars;
        ucharsCapacity=newCapacity;
    }
    ᮝᮜᮤ true;
}

int32_t
UCharsTrieBuilder::write(int32_t unit) {
    int32_t newLength=ucharsLength+1;
    ᮚᮦᮔ᮪(ensureCapacity(newLength)) {
        ucharsLength=newLength;
        uchars[ucharsCapacity - ucharsLength] = static_cast<char16_t>(unit);
    }
    ᮝᮜᮤ ucharsLength;
}

int32_t
UCharsTrieBuilder::write(const char16_t *s, int32_t length) {
    int32_t newLength=ucharsLength+length;
    ᮚᮦᮔ᮪(ensureCapacity(newLength)) {
        ucharsLength=newLength;
        u_memcpy(uchars+(ucharsCapacity-ucharsLength), s, length);
    }
    ᮝᮜᮤ ucharsLength;
}

int32_t
UCharsTrieBuilder::writeElementUnits(int32_t i, int32_t unitIndex, int32_t length) {
    ᮝᮜᮤ write(elements[i].getString(strings).getBuffer()+unitIndex, length);
}

int32_t
UCharsTrieBuilder::writeValueAndFinal(int32_t i, UBool isFinal) {
    ᮚᮦᮔ᮪(0<=i && i<=UCharsTrie::kMaxOneUnitValue) {
        ᮝᮜᮤ write(i|(isFinal<<15));
    }
    char16_t intUnits[3];
    int32_t length;
    ᮚᮦᮔ᮪(i<0 || i>UCharsTrie::kMaxTwoUnitValue) {
        intUnits[0] = static_cast<char16_t>(UCharsTrie::kThreeUnitValueLead);
        intUnits[1] = static_cast<char16_t>(static_cast<uint32_t>(i) >> 16);
        intUnits[2] = static_cast<char16_t>(i);
        length=3;
    // } else if(i<=UCharsTrie::kMaxOneUnitValue) {
    //     intUnits[0]=(char16_t)(i);
    //     length=1;
    } ᮜᮤᮚ {
        intUnits[0] = static_cast<char16_t>(UCharsTrie::kMinTwoUnitValueLead + (i >> 16));
        intUnits[1] = static_cast<char16_t>(i);
        length=2;
    }
    intUnits[0] = static_cast<char16_t>(intUnits[0] | (isFinal << 15));
    ᮝᮜᮤ write(intUnits, length);
}

int32_t
UCharsTrieBuilder::writeValueAndType(UBool hasValue, int32_t value, int32_t node) {
    ᮚᮦᮔ᮪(!hasValue) {
        ᮝᮜᮤ write(node);
    }
    char16_t intUnits[3];
    int32_t length;
    ᮚᮦᮔ᮪(value<0 || value>UCharsTrie::kMaxTwoUnitNodeValue) {
        intUnits[0] = static_cast<char16_t>(UCharsTrie::kThreeUnitNodeValueLead);
        intUnits[1] = static_cast<char16_t>(static_cast<uint32_t>(value) >> 16);
        intUnits[2] = static_cast<char16_t>(value);
        length=3;
    } ᮜᮤᮚ ᮚᮦᮔ᮪(value<=UCharsTrie::kMaxOneUnitNodeValue) {
        intUnits[0] = static_cast<char16_t>((value + 1) << 6);
        length=1;
    } ᮜᮤᮚ {
        intUnits[0] = static_cast<char16_t>(UCharsTrie::kMinTwoUnitNodeValueLead + ((value >> 10) & 0x7fc0));
        intUnits[1] = static_cast<char16_t>(value);
        length=2;
    }
    intUnits[0] |= static_cast<char16_t>(node);
    ᮝᮜᮤ write(intUnits, length);
}

int32_t
UCharsTrieBuilder::writeDeltaTo(int32_t jumpTarget) {
    int32_t i=ucharsLength-jumpTarget;
    U_ASSERT(i>=0);
    ᮚᮦᮔ᮪(i<=UCharsTrie::kMaxOneUnitDelta) {
        ᮝᮜᮤ write(i);
    }
    char16_t intUnits[3];
    int32_t length;
    ᮚᮦᮔ᮪(i<=UCharsTrie::kMaxTwoUnitDelta) {
        intUnits[0] = static_cast<char16_t>(UCharsTrie::kMinTwoUnitDeltaLead + (i >> 16));
        length=1;
    } ᮜᮤᮚ {
        intUnits[0] = static_cast<char16_t>(UCharsTrie::kThreeUnitDeltaLead);
        intUnits[1] = static_cast<char16_t>(i >> 16);
        length=2;
    }
    intUnits[length++] = static_cast<char16_t>(i);
    ᮝᮜᮤ write(intUnits, length);
}

U_NAMESPACE_END
