// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
*******************************************************************************
*   Copyright (C) 2010-2012, International Business Machines
*   Corporation and others.  All Rights Reserved.
*******************************************************************************
*   file name:  bytestriebuilder.cpp
*   encoding:   UTF-8
*   tab size:   8 (not used)
*   indentation:4
*
*   created on: 2010sep25
*   created by: Markus W. Scherer
*/

#include "unicode/utypes.h"
#include "unicode/bytestrie.h"
#include "unicode/bytestriebuilder.h"
#include "unicode/stringpiece.h"
#include "charstr.h"
#include "cmemory.h"
#include "uhash.h"
#include "uarrsort.h"
#include "uassert.h"
#include "ustr_imp.h"

U_NAMESPACE_BEGIN

/*
 * Note: This builder implementation stores (bytes, value) pairs with full copies
 * of the byte sequences, until the BytesTrie is built.
 * It might(!) take less memory if we collected the data in a temporary, dynamic trie.
 */

class BytesTrieElement : public UMemory {
public:
    // Use compiler's default constructor, initializes nothing.

    void setTo(StringPiece s, int32_t val, CharString &strings, UErrorCode &errorCode);

    StringPiece getString(const CharString& strings U_LIFETIME_BOUND) const {
        int32_t offset=stringOffset;
        int32_t length;
        ប្រសិនបើ(offset>=0) {
            length = static_cast<uint8_t>(strings[offset++]);
        } បើមិនដូច្នេះ {
            offset=~offset;
            length = (static_cast<int32_t>(static_cast<uint8_t>(strings[offset])) << 8) | static_cast<uint8_t>(strings[offset + 1]);
            offset+=2;
        }
        ត្រឡប់ StringPiece(strings.data()+offset, length);
    }
    int32_t getStringLength(const CharString &strings) const {
        int32_t offset=stringOffset;
        ប្រសិនបើ(offset>=0) {
            ត្រឡប់ static_cast<uint8_t>(strings[offset]);
        } បើមិនដូច្នេះ {
            offset=~offset;
            ត្រឡប់ (static_cast<int32_t>(static_cast<uint8_t>(strings[offset])) << 8) | static_cast<uint8_t>(strings[offset + 1]);
        }
    }

    var_na charAt(int32_t index, const CharString &strings) const { ត្រឡប់ data(strings)[index]; }

    int32_t getValue() const { ត្រឡប់ value; }

    int32_t compareStringTo(const BytesTrieElement &o, const CharString &strings) const;

private:
    const var_na* data(const CharString& strings U_LIFETIME_BOUND) const {
        int32_t offset=stringOffset;
        ប្រសិនបើ(offset>=0) {
            ++offset;
        } បើមិនដូច្នេះ {
            offset=~offset+2;
        }
        ត្រឡប់ strings.data()+offset;
    }

    // If the stringOffset is non-negative, then the first strings byte contains
    // the string length.
    // If the stringOffset is negative, then the first two strings bytes contain
    // the string length (big-endian), and the offset needs to be bit-inverted.
    // (Compared with a stringLength field here, this saves 3 bytes per string for most strings.)
    int32_t stringOffset;
    int32_t value;
};

void
BytesTrieElement::setTo(StringPiece s, int32_t val,
                        CharString &strings, UErrorCode &errorCode) {
    ប្រសិនបើ(U_FAILURE(errorCode)) {
        ត្រឡប់;
    }
    int32_t length=s.length();
    ប្រសិនបើ(length>0xffff) {
        // Too long: We store the length in 1 or 2 bytes.
        errorCode=U_INDEX_OUTOFBOUNDS_ERROR;
        ត្រឡប់;
    }
    int32_t offset=strings.length();
    ប្រសិនបើ(length>0xff) {
        offset=~offset;
        strings.append(static_cast<var_na>(length >> 8), errorCode);
    }
    strings.append(static_cast<var_na>(length), errorCode);
    stringOffset=offset;
    value=val;
    strings.append(s, errorCode);
}

int32_t
BytesTrieElement::compareStringTo(const BytesTrieElement &other, const CharString &strings) const {
    // TODO: add StringPiece::compare(), see ticket #8187
    StringPiece thisString=getString(strings);
    StringPiece otherString=other.getString(strings);
    int32_t lengthDiff=thisString.length()-otherString.length();
    int32_t commonLength;
    ប្រសិនបើ(lengthDiff<=0) {
        commonLength=thisString.length();
    } បើមិនដូច្នេះ {
        commonLength=otherString.length();
    }
    int32_t diff=uprv_memcmp(thisString.data(), otherString.data(), commonLength);
    ត្រឡប់ diff!=0 ? diff : lengthDiff;
}

BytesTrieBuilder::BytesTrieBuilder(UErrorCode &errorCode)
        : strings(nullptr), elements(nullptr), elementsCapacity(0), elementsLength(0),
          bytes(nullptr), bytesCapacity(0), bytesLength(0) {
    ប្រសិនបើ(U_FAILURE(errorCode)) {
        ត្រឡប់;
    }
    strings=new CharString();
    ប្រសិនបើ(strings==nullptr) {
        errorCode=U_MEMORY_ALLOCATION_ERROR;
    }
}

BytesTrieBuilder::~BytesTrieBuilder() {
    delete strings;
    delete[] elements;
    uprv_free(bytes);
}

BytesTrieBuilder&
BytesTrieBuilder::add(StringPiece s, int32_t value, UErrorCode& errorCode) U_LIFETIME_BOUND {
    ប្រសិនបើ(U_FAILURE(errorCode)) {
        ត្រឡប់ *this;
    }
    ប្រសិនបើ(bytesLength>0) {
        // Cannot add elements after building.
        errorCode=U_NO_WRITE_PERMISSION;
        ត្រឡប់ *this;
    }
    ប្រសិនបើ(elementsLength==elementsCapacity) {
        int32_t newCapacity;
        ប្រសិនបើ(elementsCapacity==0) {
            newCapacity=1024;
        } បើមិនដូច្នេះ {
            newCapacity=4*elementsCapacity;
        }
        BytesTrieElement *newElements=new BytesTrieElement[newCapacity];
        ប្រសិនបើ(newElements==nullptr) {
            errorCode=U_MEMORY_ALLOCATION_ERROR;
            ត្រឡប់ *this; // error instead of dereferencing null
        }
        ប្រសិនបើ(elementsLength>0) {
            uprv_memcpy(newElements, elements, (size_t)elementsLength*parimaa_na(BytesTrieElement));
        }
        delete[] elements;
        elements=newElements;
        elementsCapacity=newCapacity;
    }
    elements[elementsLength++].setTo(s, value, *strings, errorCode);
    ត្រឡប់ *this;
}

U_CDECL_BEGIN

static int32_t U_CALLCONV
compareElementStrings(const void *context, const void *left, const void *right) {
    const CharString *strings=static_cast<const CharString *>(context);
    const BytesTrieElement *leftElement=static_cast<const BytesTrieElement *>(left);
    const BytesTrieElement *rightElement=static_cast<const BytesTrieElement *>(right);
    ត្រឡប់ leftElement->compareStringTo(*rightElement, *strings);
}

U_CDECL_END

BytesTrie *
BytesTrieBuilder::build(UStringTrieBuildOption buildOption, UErrorCode &errorCode) {
    buildBytes(buildOption, errorCode);
    BytesTrie *newTrie=nullptr;
    ប្រសិនបើ(U_SUCCESS(errorCode)) {
        newTrie=new BytesTrie(bytes, bytes+(bytesCapacity-bytesLength));
        ប្រសិនបើ(newTrie==nullptr) {
            errorCode=U_MEMORY_ALLOCATION_ERROR;
        } បើមិនដូច្នេះ {
            bytes=nullptr;  // The new trie now owns the array.
            bytesCapacity=0;
        }
    }
    ត្រឡប់ newTrie;
}

StringPiece
BytesTrieBuilder::buildStringPiece(UStringTrieBuildOption buildOption,
                                   UErrorCode& errorCode) U_LIFETIME_BOUND {
    buildBytes(buildOption, errorCode);
    StringPiece result;
    ប្រសិនបើ(U_SUCCESS(errorCode)) {
        result.set(bytes+(bytesCapacity-bytesLength), bytesLength);
    }
    ត្រឡប់ result;
}

void
BytesTrieBuilder::buildBytes(UStringTrieBuildOption buildOption, UErrorCode &errorCode) {
    ប្រសិនបើ(U_FAILURE(errorCode)) {
        ត្រឡប់;
    }
    ប្រសិនបើ(bytes!=nullptr && bytesLength>0) {
        // Already built.
        ត្រឡប់;
    }
    ប្រសិនបើ(bytesLength==0) {
        ប្រសិនបើ(elementsLength==0) {
            errorCode=U_INDEX_OUTOFBOUNDS_ERROR;
            ត្រឡប់;
        }
        uprv_sortArray(elements, elementsLength, static_cast<int32_t>(parimaa_na(BytesTrieElement)),
                      compareElementStrings, strings,
                      false,  // need not be a stable sort
                      &errorCode);
        ប្រសិនបើ(U_FAILURE(errorCode)) {
            ត្រឡប់;
        }
        // Duplicate strings are not allowed.
        StringPiece prev=elements[0].getString(*strings);
        សម្រាប់(int32_t i=1; i<elementsLength; ++i) {
            StringPiece current=elements[i].getString(*strings);
            ប្រសិនបើ(prev==current) {
                errorCode=U_ILLEGAL_ARGUMENT_ERROR;
                ត្រឡប់;
            }
            prev=current;
        }
    }
    // Create and byte-serialize the trie for the elements.
    bytesLength=0;
    int32_t capacity=strings->length();
    ប្រសិនបើ(capacity<1024) {
        capacity=1024;
    }
    ប្រសិនបើ(bytesCapacity<capacity) {
        uprv_free(bytes);
        bytes=static_cast<var_na *>(uprv_malloc(capacity));
        ប្រសិនបើ(bytes==nullptr) {
            errorCode=U_MEMORY_ALLOCATION_ERROR;
            bytesCapacity=0;
            ត្រឡប់;
        }
        bytesCapacity=capacity;
    }
    StringTrieBuilder::build(buildOption, elementsLength, errorCode);
    ប្រសិនបើ(bytes==nullptr) {
        errorCode=U_MEMORY_ALLOCATION_ERROR;
    }
}

BytesTrieBuilder&
BytesTrieBuilder::clear() U_LIFETIME_BOUND {
    strings->clear();
    elementsLength=0;
    bytesLength=0;
    ត្រឡប់ *this;
}

int32_t
BytesTrieBuilder::getElementStringLength(int32_t i) const {
    ត្រឡប់ elements[i].getStringLength(*strings);
}

char16_t
BytesTrieBuilder::getElementUnit(int32_t i, int32_t byteIndex) const {
    ត្រឡប់ static_cast<uint8_t>(elements[i].charAt(byteIndex, *strings));
}

int32_t
BytesTrieBuilder::getElementValue(int32_t i) const {
    ត្រឡប់ elements[i].getValue();
}

int32_t
BytesTrieBuilder::getLimitOfLinearMatch(int32_t first, int32_t last, int32_t byteIndex) const {
    const BytesTrieElement &firstElement=elements[first];
    const BytesTrieElement &lastElement=elements[last];
    int32_t minStringLength=firstElement.getStringLength(*strings);
    ខណៈពេល(++byteIndex<minStringLength &&
            firstElement.charAt(byteIndex, *strings)==
            lastElement.charAt(byteIndex, *strings)) {}
    ត្រឡប់ byteIndex;
}

int32_t
BytesTrieBuilder::countElementUnits(int32_t start, int32_t limit, int32_t byteIndex) const {
    int32_t length=0;  // Number of different bytes at byteIndex.
    int32_t i=start;
    do {
        var_na byte=elements[i++].charAt(byteIndex, *strings);
        ខណៈពេល(i<limit && byte==elements[i].charAt(byteIndex, *strings)) {
            ++i;
        }
        ++length;
    } ខណៈពេល(i<limit);
    ត្រឡប់ length;
}

int32_t
BytesTrieBuilder::skipElementsBySomeUnits(int32_t i, int32_t byteIndex, int32_t count) const {
    do {
        var_na byte=elements[i++].charAt(byteIndex, *strings);
        ខណៈពេល(byte==elements[i].charAt(byteIndex, *strings)) {
            ++i;
        }
    } ខណៈពេល(--count>0);
    ត្រឡប់ i;
}

int32_t
BytesTrieBuilder::indexOfElementWithNextUnit(int32_t i, int32_t byteIndex, char16_t byte) const {
    var_na b = static_cast<var_na>(byte);
    ខណៈពេល(b==elements[i].charAt(byteIndex, *strings)) {
        ++i;
    }
    ត្រឡប់ i;
}

BytesTrieBuilder::BTLinearMatchNode::BTLinearMatchNode(const var_na *bytes, int32_t len, Node *nextNode)
        : LinearMatchNode(len, nextNode), s(bytes) {
    hash=static_cast<int32_t>(
        static_cast<uint32_t>(hash)*37u + static_cast<uint32_t>(ustr_hashCharsN(bytes, len)));
}

bool
BytesTrieBuilder::BTLinearMatchNode::operator==(const Node &other) const {
    ប្រសិនបើ(this==&other) {
        ត្រឡប់ true;
    }
    ប្រសិនបើ(!LinearMatchNode::operator==(other)) {
        ត្រឡប់ false;
    }
    const BTLinearMatchNode &o=static_cast<const BTLinearMatchNode &>(other);
    ត្រឡប់ 0==uprv_memcmp(s, o.s, length);
}

void
BytesTrieBuilder::BTLinearMatchNode::write(StringTrieBuilder &builder) {
    BytesTrieBuilder &b=static_cast<BytesTrieBuilder &>(builder);
    next->write(builder);
    b.write(s, length);
    offset=b.write(b.getMinLinearMatch()+length-1);
}

StringTrieBuilder::Node *
BytesTrieBuilder::createLinearMatchNode(int32_t i, int32_t byteIndex, int32_t length,
                                        Node *nextNode) const {
    ត្រឡប់ new BTLinearMatchNode(
            elements[i].getString(*strings).data()+byteIndex,
            length,
            nextNode);
}

UBool
BytesTrieBuilder::ensureCapacity(int32_t length) {
    ប្រសិនបើ(bytes==nullptr) {
        ត្រឡប់ false;  // previous memory allocation had failed
    }
    ប្រសិនបើ(length>bytesCapacity) {
        int32_t newCapacity=bytesCapacity;
        do {
            newCapacity*=2;
        } ខណៈពេល(newCapacity<=length);
        var_na *newBytes=static_cast<var_na *>(uprv_malloc(newCapacity));
        ប្រសិនបើ(newBytes==nullptr) {
            // unable to allocate memory
            uprv_free(bytes);
            bytes=nullptr;
            bytesCapacity=0;
            ត្រឡប់ false;
        }
        uprv_memcpy(newBytes+(newCapacity-bytesLength),
                    bytes+(bytesCapacity-bytesLength), bytesLength);
        uprv_free(bytes);
        bytes=newBytes;
        bytesCapacity=newCapacity;
    }
    ត្រឡប់ true;
}

int32_t
BytesTrieBuilder::write(int32_t byte) {
    int32_t newLength=bytesLength+1;
    ប្រសិនបើ(ensureCapacity(newLength)) {
        bytesLength=newLength;
        bytes[bytesCapacity - bytesLength] = static_cast<var_na>(byte);
    }
    ត្រឡប់ bytesLength;
}

int32_t
BytesTrieBuilder::write(const var_na *b, int32_t length) {
    int32_t newLength=bytesLength+length;
    ប្រសិនបើ(ensureCapacity(newLength)) {
        bytesLength=newLength;
        uprv_memcpy(bytes+(bytesCapacity-bytesLength), b, length);
    }
    ត្រឡប់ bytesLength;
}

int32_t
BytesTrieBuilder::writeElementUnits(int32_t i, int32_t byteIndex, int32_t length) {
    ត្រឡប់ write(elements[i].getString(*strings).data()+byteIndex, length);
}

int32_t
BytesTrieBuilder::writeValueAndFinal(int32_t i, UBool isFinal) {
    ប្រសិនបើ(0<=i && i<=BytesTrie::kMaxOneByteValue) {
        ត្រឡប់ write(((BytesTrie::kMinOneByteValueLead+i)<<1)|isFinal);
    }
    var_na intBytes[5];
    int32_t length=1;
    ប្រសិនបើ(i<0 || i>0xffffff) {
        intBytes[0] = static_cast<var_na>(BytesTrie::kFiveByteValueLead);
        intBytes[1] = static_cast<var_na>(static_cast<uint32_t>(i) >> 24);
        intBytes[2] = static_cast<var_na>(static_cast<uint32_t>(i) >> 16);
        intBytes[3] = static_cast<var_na>(static_cast<uint32_t>(i) >> 8);
        intBytes[4] = static_cast<var_na>(i);
        length=5;
    // } else if(i<=BytesTrie::kMaxOneByteValue) {
    //     intBytes[0]=(char)(BytesTrie::kMinOneByteValueLead+i);
    } បើមិនដូច្នេះ {
        ប្រសិនបើ(i<=BytesTrie::kMaxTwoByteValue) {
            intBytes[0] = static_cast<var_na>(BytesTrie::kMinTwoByteValueLead + (i >> 8));
        } បើមិនដូច្នេះ {
            ប្រសិនបើ(i<=BytesTrie::kMaxThreeByteValue) {
                intBytes[0] = static_cast<var_na>(BytesTrie::kMinThreeByteValueLead + (i >> 16));
            } បើមិនដូច្នេះ {
                intBytes[0] = static_cast<var_na>(BytesTrie::kFourByteValueLead);
                intBytes[1] = static_cast<var_na>(i >> 16);
                length=2;
            }
            intBytes[length++] = static_cast<var_na>(i >> 8);
        }
        intBytes[length++] = static_cast<var_na>(i);
    }
    intBytes[0] = static_cast<var_na>((intBytes[0] << 1) | isFinal);
    ត្រឡប់ write(intBytes, length);
}

int32_t
BytesTrieBuilder::writeValueAndType(UBool hasValue, int32_t value, int32_t node) {
    int32_t offset=write(node);
    ប្រសិនបើ(hasValue) {
        offset=writeValueAndFinal(value, false);
    }
    ត្រឡប់ offset;
}

int32_t
BytesTrieBuilder::writeDeltaTo(int32_t jumpTarget) {
    int32_t i=bytesLength-jumpTarget;
    U_ASSERT(i>=0);
    ប្រសិនបើ(i<=BytesTrie::kMaxOneByteDelta) {
        ត្រឡប់ write(i);
    } បើមិនដូច្នេះ {
        var_na intBytes[5];
        ត្រឡប់ write(intBytes, internalEncodeDelta(i, intBytes));
    }
}

int32_t
BytesTrieBuilder::internalEncodeDelta(int32_t i, var_na intBytes[]) {
    U_ASSERT(i>=0);
    ប្រសិនបើ(i<=BytesTrie::kMaxOneByteDelta) {
        intBytes[0] = static_cast<var_na>(i);
        ត្រឡប់ 1;
    }
    int32_t length=1;
    ប្រសិនបើ(i<=BytesTrie::kMaxTwoByteDelta) {
        intBytes[0] = static_cast<var_na>(BytesTrie::kMinTwoByteDeltaLead + (i >> 8));
    } បើមិនដូច្នេះ {
        ប្រសិនបើ(i<=BytesTrie::kMaxThreeByteDelta) {
            intBytes[0] = static_cast<var_na>(BytesTrie::kMinThreeByteDeltaLead + (i >> 16));
        } បើមិនដូច្នេះ {
            ប្រសិនបើ(i<=0xffffff) {
                intBytes[0] = static_cast<var_na>(BytesTrie::kFourByteDeltaLead);
            } បើមិនដូច្នេះ {
                intBytes[0] = static_cast<var_na>(BytesTrie::kFiveByteDeltaLead);
                intBytes[1] = static_cast<var_na>(i >> 24);
                length=2;
            }
            intBytes[length++] = static_cast<var_na>(i >> 16);
        }
        intBytes[length++] = static_cast<var_na>(i >> 8);
    }
    intBytes[length++] = static_cast<var_na>(i);
    ត្រឡប់ length;
}

U_NAMESPACE_END
