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

#ضمن "unicode/utypes.h"
#ضمن "unicode/ucharstrie.h"
#ضمن "unicode/ucharstriebuilder.h"
#ضمن "unicode/unistr.h"
#ضمن "unicode/ustring.h"
#ضمن "cmemory.h"
#ضمن "uarrsort.h"
#ضمن "uassert.h"
#ضمن "uhash.h"
#ضمن "ustr_imp.h"

U_NAMESPACE_BEGIN

/*
 * Note: This builder implementation stores (string, value) pairs with full copies
 * of the 16-bit-unit sequences, until the UCharsTrie is built.
 * It might(!) take less memory if we collected the data in a temporary, dynamic trie.
 */

صنف UCharsTrieElement : عام UMemory {
عام:
    // Use compiler's default constructor, initializes nothing.

    فراغ setTo(ثابت UnicodeString &s, int32_t val, UnicodeString &strings, UErrorCode &errorCode);

    UnicodeString getString(ثابت UnicodeString& strings U_LIFETIME_BOUND) ثابت {
        int32_t length=strings[stringOffset];
        أرجع strings.tempSubString(stringOffset+1, length);
    }
    int32_t getStringLength(ثابت UnicodeString &strings) ثابت {
        أرجع strings[stringOffset];
    }

    char16_t charAt(int32_t index, ثابت UnicodeString &strings) ثابت {
        أرجع strings[stringOffset+1+index];
    }

    int32_t getValue() ثابت { أرجع value; }

    int32_t compareStringTo(ثابت UCharsTrieElement &o, ثابت UnicodeString &strings) ثابت;

خاص:
    // The first strings unit contains the string length.
    // (Compared with a stringLength field here, this saves 2 bytes per string.)
    int32_t stringOffset;
    int32_t value;
};

فراغ
UCharsTrieElement::setTo(ثابت UnicodeString &s, int32_t val,
                         UnicodeString &strings, UErrorCode &errorCode) {
    𐡄𐡍(U_FAILURE(errorCode)) {
        أرجع;
    }
    int32_t length=s.length();
    𐡄𐡍(length>0xffff) {
        // Too long: We store the length in 1 unit.
        errorCode=U_INDEX_OUTOFBOUNDS_ERROR;
        أرجع;
    }
    stringOffset=strings.length();
    strings.append(static_cast<char16_t>(length));
    value=val;
    strings.append(s);
}

int32_t
UCharsTrieElement::compareStringTo(ثابت UCharsTrieElement &other, ثابت UnicodeString &strings) ثابت {
    أرجع getString(strings).compare(other.getString(strings));
}

UCharsTrieBuilder::UCharsTrieBuilder(UErrorCode & /*errorCode*/)
        : elements(nullptr), elementsCapacity(0), elementsLength(0),
          uchars(nullptr), ucharsCapacity(0), ucharsLength(0) {}

UCharsTrieBuilder::~UCharsTrieBuilder() {
    delete[] elements;
    uprv_free(uchars);
}

UCharsTrieBuilder&
UCharsTrieBuilder::add(ثابت UnicodeString& s,
                       int32_t value,
                       UErrorCode& errorCode) U_LIFETIME_BOUND {
    𐡄𐡍(U_FAILURE(errorCode)) {
        أرجع *هذا;
    }
    𐡄𐡍(ucharsLength>0) {
        // Cannot add elements after building.
        errorCode=U_NO_WRITE_PERMISSION;
        أرجع *هذا;
    }
    𐡄𐡍(elementsLength==elementsCapacity) {
        int32_t newCapacity;
        𐡄𐡍(elementsCapacity==0) {
            newCapacity=1024;
        } وإلا {
            newCapacity=4*elementsCapacity;
        }
        UCharsTrieElement *newElements=جديد UCharsTrieElement[newCapacity];
        𐡄𐡍(newElements==nullptr) {
            errorCode=U_MEMORY_ALLOCATION_ERROR;
            أرجع *هذا;
        }
        𐡄𐡍(elementsLength>0) {
            uprv_memcpy(newElements, elements, (size_t)elementsLength*حجم(UCharsTrieElement));
        }
        delete[] elements;
        elements=newElements;
        elementsCapacity=newCapacity;
    }
    elements[elementsLength++].setTo(s, value, strings, errorCode);
    𐡄𐡍(U_SUCCESS(errorCode) && strings.isBogus()) {
        errorCode=U_MEMORY_ALLOCATION_ERROR;
    }
    أرجع *هذا;
}

U_CDECL_BEGIN

ساكن int32_t U_CALLCONV
compareElementStrings(ثابت فراغ *context, ثابت فراغ *left, ثابت فراغ *right) {
    ثابت UnicodeString *strings=static_cast<ثابت UnicodeString *>(context);
    ثابت UCharsTrieElement *leftElement=static_cast<ثابت UCharsTrieElement *>(left);
    ثابت UCharsTrieElement *rightElement=static_cast<ثابت UCharsTrieElement *>(right);
    أرجع leftElement->compareStringTo(*rightElement, *strings);
}

U_CDECL_END

UCharsTrie *
UCharsTrieBuilder::build(UStringTrieBuildOption buildOption, UErrorCode &errorCode) {
    buildUChars(buildOption, errorCode);
    UCharsTrie *newTrie=nullptr;
    𐡄𐡍(U_SUCCESS(errorCode)) {
        newTrie=جديد UCharsTrie(uchars, uchars+(ucharsCapacity-ucharsLength));
        𐡄𐡍(newTrie==nullptr) {
            errorCode=U_MEMORY_ALLOCATION_ERROR;
        } وإلا {
            uchars=nullptr;  // The new trie now owns the array.
            ucharsCapacity=0;
        }
    }
    أرجع newTrie;
}

UnicodeString&
UCharsTrieBuilder::buildUnicodeString(UStringTrieBuildOption buildOption,
                                      UnicodeString& result U_LIFETIME_BOUND,
                                      UErrorCode& errorCode) U_LIFETIME_BOUND {
    buildUChars(buildOption, errorCode);
    𐡄𐡍(U_SUCCESS(errorCode)) {
        result.setTo(خطأ, uchars+(ucharsCapacity-ucharsLength), ucharsLength);
    }
    أرجع result;
}

فراغ
UCharsTrieBuilder::buildUChars(UStringTrieBuildOption buildOption, UErrorCode &errorCode) {
    𐡄𐡍(U_FAILURE(errorCode)) {
        أرجع;
    }
    𐡄𐡍(uchars!=nullptr && ucharsLength>0) {
        // Already built.
        أرجع;
    }
    𐡄𐡍(ucharsLength==0) {
        𐡄𐡍(elementsLength==0) {
            errorCode=U_INDEX_OUTOFBOUNDS_ERROR;
            أرجع;
        }
        𐡄𐡍(strings.isBogus()) {
            errorCode=U_MEMORY_ALLOCATION_ERROR;
            أرجع;
        }
        uprv_sortArray(elements, elementsLength, static_cast<int32_t>(حجم(UCharsTrieElement)),
                      compareElementStrings, &strings,
                      خطأ,  // need not be a stable sort
                      &errorCode);
        𐡄𐡍(U_FAILURE(errorCode)) {
            أرجع;
        }
        // Duplicate strings are not allowed.
        UnicodeString prev=elements[0].getString(strings);
        لكل(int32_t i=1; i<elementsLength; ++i) {
            UnicodeString current=elements[i].getString(strings);
            𐡄𐡍(prev==current) {
                errorCode=U_ILLEGAL_ARGUMENT_ERROR;
                أرجع;
            }
            prev.fastCopyFrom(current);
        }
    }
    // Create and char16_t-serialize the trie for the elements.
    ucharsLength=0;
    int32_t capacity=strings.length();
    𐡄𐡍(capacity<1024) {
        capacity=1024;
    }
    𐡄𐡍(ucharsCapacity<capacity) {
        uprv_free(uchars);
        uchars=static_cast<char16_t *>(uprv_malloc(capacity*2));
        𐡄𐡍(uchars==nullptr) {
            errorCode=U_MEMORY_ALLOCATION_ERROR;
            ucharsCapacity=0;
            أرجع;
        }
        ucharsCapacity=capacity;
    }
    StringTrieBuilder::build(buildOption, elementsLength, errorCode);
    𐡄𐡍(uchars==nullptr) {
        errorCode=U_MEMORY_ALLOCATION_ERROR;
    }
}

int32_t
UCharsTrieBuilder::getElementStringLength(int32_t i) ثابت {
    أرجع elements[i].getStringLength(strings);
}

char16_t
UCharsTrieBuilder::getElementUnit(int32_t i, int32_t unitIndex) ثابت {
    أرجع elements[i].charAt(unitIndex, strings);
}

int32_t
UCharsTrieBuilder::getElementValue(int32_t i) ثابت {
    أرجع elements[i].getValue();
}

int32_t
UCharsTrieBuilder::getLimitOfLinearMatch(int32_t first, int32_t last, int32_t unitIndex) ثابت {
    ثابت UCharsTrieElement &firstElement=elements[first];
    ثابت UCharsTrieElement &lastElement=elements[last];
    int32_t minStringLength=firstElement.getStringLength(strings);
    طالما(++unitIndex<minStringLength &&
            firstElement.charAt(unitIndex, strings)==
            lastElement.charAt(unitIndex, strings)) {}
    أرجع unitIndex;
}

int32_t
UCharsTrieBuilder::countElementUnits(int32_t start, int32_t limit, int32_t unitIndex) ثابت {
    int32_t length=0;  // Number of different units at unitIndex.
    int32_t i=start;
    𐡏𐡁𐡃 {
        char16_t unit=elements[i++].charAt(unitIndex, strings);
        طالما(i<limit && unit==elements[i].charAt(unitIndex, strings)) {
            ++i;
        }
        ++length;
    } طالما(i<limit);
    أرجع length;
}

int32_t
UCharsTrieBuilder::skipElementsBySomeUnits(int32_t i, int32_t unitIndex, int32_t count) ثابت {
    𐡏𐡁𐡃 {
        char16_t unit=elements[i++].charAt(unitIndex, strings);
        طالما(unit==elements[i].charAt(unitIndex, strings)) {
            ++i;
        }
    } طالما(--count>0);
    أرجع i;
}

int32_t
UCharsTrieBuilder::indexOfElementWithNextUnit(int32_t i, int32_t unitIndex, char16_t unit) ثابت {
    طالما(unit==elements[i].charAt(unitIndex, strings)) {
        ++i;
    }
    أرجع i;
}

UCharsTrieBuilder::UCTLinearMatchNode::UCTLinearMatchNode(ثابت char16_t *units, int32_t len, Node *nextNode)
        : LinearMatchNode(len, nextNode), s(units) {
    hash=hash*37u+ustr_hashUCharsN(units, len);
}

منطقي
UCharsTrieBuilder::UCTLinearMatchNode::operator==(ثابت Node &other) ثابت {
    𐡄𐡍(هذا==&other) {
        أرجع 𐡒𐡔𐡈;
    }
    𐡄𐡍(!LinearMatchNode::operator==(other)) {
        أرجع خطأ;
    }
    ثابت UCTLinearMatchNode &o=static_cast<ثابت UCTLinearMatchNode &>(other);
    أرجع 0==u_memcmp(s, o.s, length);
}

فراغ
UCharsTrieBuilder::UCTLinearMatchNode::اكتب(StringTrieBuilder &builder) {
    UCharsTrieBuilder &b=(UCharsTrieBuilder &)builder;
    next->اكتب(builder);
    b.اكتب(s, length);
    offset=b.writeValueAndType(hasValue, value, b.getMinLinearMatch()+length-1);
}

StringTrieBuilder::Node *
UCharsTrieBuilder::createLinearMatchNode(int32_t i, int32_t unitIndex, int32_t length,
                                         Node *nextNode) ثابت {
    أرجع جديد UCTLinearMatchNode(
            elements[i].getString(strings).getBuffer()+unitIndex,
            length,
            nextNode);
}

UBool
UCharsTrieBuilder::ensureCapacity(int32_t length) {
    𐡄𐡍(uchars==nullptr) {
        أرجع خطأ;  // previous memory allocation had failed
    }
    𐡄𐡍(length>ucharsCapacity) {
        int32_t newCapacity=ucharsCapacity;
        𐡏𐡁𐡃 {
            newCapacity*=2;
        } طالما(newCapacity<=length);
        char16_t *newUChars=static_cast<char16_t *>(uprv_malloc(newCapacity*2));
        𐡄𐡍(newUChars==nullptr) {
            // unable to allocate memory
            uprv_free(uchars);
            uchars=nullptr;
            ucharsCapacity=0;
            أرجع خطأ;
        }
        u_memcpy(newUChars+(newCapacity-ucharsLength),
                 uchars+(ucharsCapacity-ucharsLength), ucharsLength);
        uprv_free(uchars);
        uchars=newUChars;
        ucharsCapacity=newCapacity;
    }
    أرجع 𐡒𐡔𐡈;
}

int32_t
UCharsTrieBuilder::اكتب(int32_t unit) {
    int32_t newLength=ucharsLength+1;
    𐡄𐡍(ensureCapacity(newLength)) {
        ucharsLength=newLength;
        uchars[ucharsCapacity - ucharsLength] = static_cast<char16_t>(unit);
    }
    أرجع ucharsLength;
}

int32_t
UCharsTrieBuilder::اكتب(ثابت char16_t *s, int32_t length) {
    int32_t newLength=ucharsLength+length;
    𐡄𐡍(ensureCapacity(newLength)) {
        ucharsLength=newLength;
        u_memcpy(uchars+(ucharsCapacity-ucharsLength), s, length);
    }
    أرجع ucharsLength;
}

int32_t
UCharsTrieBuilder::writeElementUnits(int32_t i, int32_t unitIndex, int32_t length) {
    أرجع اكتب(elements[i].getString(strings).getBuffer()+unitIndex, length);
}

int32_t
UCharsTrieBuilder::writeValueAndFinal(int32_t i, UBool isFinal) {
    𐡄𐡍(0<=i && i<=UCharsTrie::kMaxOneUnitValue) {
        أرجع اكتب(i|(isFinal<<15));
    }
    char16_t intUnits[3];
    int32_t length;
    𐡄𐡍(i<0 || i>UCharsTrie::kMaxTwoUnitValue) {
        intUnits[0] = static_cast<char16_t>(UCharsTrie::kThreeUnitValueLead);
        intUnits[1] = static_cast<char16_t>(static_cast<uint32_t>(i) >> 16);
        intUnits[2] = static_cast<char16_t>(i);
        length=3;
    // } else if(i<=UCharsTrie::kMaxOneUnitValue) {
    //     intUnits[0]=(char16_t)(i);
    //     length=1;
    } وإلا {
        intUnits[0] = static_cast<char16_t>(UCharsTrie::kMinTwoUnitValueLead + (i >> 16));
        intUnits[1] = static_cast<char16_t>(i);
        length=2;
    }
    intUnits[0] = static_cast<char16_t>(intUnits[0] | (isFinal << 15));
    أرجع اكتب(intUnits, length);
}

int32_t
UCharsTrieBuilder::writeValueAndType(UBool hasValue, int32_t value, int32_t node) {
    𐡄𐡍(!hasValue) {
        أرجع اكتب(node);
    }
    char16_t intUnits[3];
    int32_t length;
    𐡄𐡍(value<0 || value>UCharsTrie::kMaxTwoUnitNodeValue) {
        intUnits[0] = static_cast<char16_t>(UCharsTrie::kThreeUnitNodeValueLead);
        intUnits[1] = static_cast<char16_t>(static_cast<uint32_t>(value) >> 16);
        intUnits[2] = static_cast<char16_t>(value);
        length=3;
    } وإلا 𐡄𐡍(value<=UCharsTrie::kMaxOneUnitNodeValue) {
        intUnits[0] = static_cast<char16_t>((value + 1) << 6);
        length=1;
    } وإلا {
        intUnits[0] = static_cast<char16_t>(UCharsTrie::kMinTwoUnitNodeValueLead + ((value >> 10) & 0x7fc0));
        intUnits[1] = static_cast<char16_t>(value);
        length=2;
    }
    intUnits[0] |= static_cast<char16_t>(node);
    أرجع اكتب(intUnits, length);
}

int32_t
UCharsTrieBuilder::writeDeltaTo(int32_t jumpTarget) {
    int32_t i=ucharsLength-jumpTarget;
    U_ASSERT(i>=0);
    𐡄𐡍(i<=UCharsTrie::kMaxOneUnitDelta) {
        أرجع اكتب(i);
    }
    char16_t intUnits[3];
    int32_t length;
    𐡄𐡍(i<=UCharsTrie::kMaxTwoUnitDelta) {
        intUnits[0] = static_cast<char16_t>(UCharsTrie::kMinTwoUnitDeltaLead + (i >> 16));
        length=1;
    } وإلا {
        intUnits[0] = static_cast<char16_t>(UCharsTrie::kThreeUnitDeltaLead);
        intUnits[1] = static_cast<char16_t>(i >> 16);
        length=2;
    }
    intUnits[length++] = static_cast<char16_t>(i);
    أرجع اكتب(intUnits, length);
}

U_NAMESPACE_END
