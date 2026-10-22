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
        如果(offset>=0) {
            length = static_cast<uint8_t>(strings[offset++]);
        } 否则 {
            offset=~offset;
            length = (static_cast<int32_t>(static_cast<uint8_t>(strings[offset])) << 8) | static_cast<uint8_t>(strings[offset + 1]);
            offset+=2;
        }
        返回 StringPiece(strings.data()+offset, length);
    }
    int32_t getStringLength(const CharString &strings) const {
        int32_t offset=stringOffset;
        如果(offset>=0) {
            返回 static_cast<uint8_t>(strings[offset]);
        } 否则 {
            offset=~offset;
            返回 (static_cast<int32_t>(static_cast<uint8_t>(strings[offset])) << 8) | static_cast<uint8_t>(strings[offset + 1]);
        }
    }

    char charAt(int32_t index, const CharString &strings) const { 返回 data(strings)[index]; }

    int32_t getValue() const { 返回 value; }

    int32_t compareStringTo(const BytesTrieElement &o, const CharString &strings) const;

private:
    const char* data(const CharString& strings U_LIFETIME_BOUND) const {
        int32_t offset=stringOffset;
        如果(offset>=0) {
            ++offset;
        } 否则 {
            offset=~offset+2;
        }
        返回 strings.data()+offset;
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
    如果(U_FAILURE(errorCode)) {
        返回;
    }
    int32_t length=s.length();
    如果(length>0xffff) {
        // Too long: We store the length in 1 or 2 bytes.
        errorCode=U_INDEX_OUTOFBOUNDS_ERROR;
        返回;
    }
    int32_t offset=strings.length();
    如果(length>0xff) {
        offset=~offset;
        strings.append(static_cast<char>(length >> 8), errorCode);
    }
    strings.append(static_cast<char>(length), errorCode);
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
    如果(lengthDiff<=0) {
        commonLength=thisString.length();
    } 否则 {
        commonLength=otherString.length();
    }
    int32_t diff=uprv_memcmp(thisString.data(), otherString.data(), commonLength);
    返回 diff!=0 ? diff : lengthDiff;
}

BytesTrieBuilder::BytesTrieBuilder(UErrorCode &errorCode)
        : strings(nullptr), elements(nullptr), elementsCapacity(0), elementsLength(0),
          bytes(nullptr), bytesCapacity(0), bytesLength(0) {
    如果(U_FAILURE(errorCode)) {
        返回;
    }
    strings=new CharString();
    如果(strings==nullptr) {
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
    如果(U_FAILURE(errorCode)) {
        返回 *this;
    }
    如果(bytesLength>0) {
        // Cannot add elements after building.
        errorCode=U_NO_WRITE_PERMISSION;
        返回 *this;
    }
    如果(elementsLength==elementsCapacity) {
        int32_t newCapacity;
        如果(elementsCapacity==0) {
            newCapacity=1024;
        } 否则 {
            newCapacity=4*elementsCapacity;
        }
        BytesTrieElement *newElements=new BytesTrieElement[newCapacity];
        如果(newElements==nullptr) {
            errorCode=U_MEMORY_ALLOCATION_ERROR;
            返回 *this; // error instead of dereferencing null
        }
        如果(elementsLength>0) {
            uprv_memcpy(newElements, elements, (size_t)elementsLength*sizeof(BytesTrieElement));
        }
        delete[] elements;
        elements=newElements;
        elementsCapacity=newCapacity;
    }
    elements[elementsLength++].setTo(s, value, *strings, errorCode);
    返回 *this;
}

U_CDECL_BEGIN

static int32_t U_CALLCONV
compareElementStrings(const void *context, const void *left, const void *right) {
    const CharString *strings=static_cast<const CharString *>(context);
    const BytesTrieElement *leftElement=static_cast<const BytesTrieElement *>(left);
    const BytesTrieElement *rightElement=static_cast<const BytesTrieElement *>(right);
    返回 leftElement->compareStringTo(*rightElement, *strings);
}

U_CDECL_END

BytesTrie *
BytesTrieBuilder::build(UStringTrieBuildOption buildOption, UErrorCode &errorCode) {
    buildBytes(buildOption, errorCode);
    BytesTrie *newTrie=nullptr;
    如果(U_SUCCESS(errorCode)) {
        newTrie=new BytesTrie(bytes, bytes+(bytesCapacity-bytesLength));
        如果(newTrie==nullptr) {
            errorCode=U_MEMORY_ALLOCATION_ERROR;
        } 否则 {
            bytes=nullptr;  // The new trie now owns the array.
            bytesCapacity=0;
        }
    }
    返回 newTrie;
}

StringPiece
BytesTrieBuilder::buildStringPiece(UStringTrieBuildOption buildOption,
                                   UErrorCode& errorCode) U_LIFETIME_BOUND {
    buildBytes(buildOption, errorCode);
    StringPiece result;
    如果(U_SUCCESS(errorCode)) {
        result.set(bytes+(bytesCapacity-bytesLength), bytesLength);
    }
    返回 result;
}

void
BytesTrieBuilder::buildBytes(UStringTrieBuildOption buildOption, UErrorCode &errorCode) {
    如果(U_FAILURE(errorCode)) {
        返回;
    }
    如果(bytes!=nullptr && bytesLength>0) {
        // Already built.
        返回;
    }
    如果(bytesLength==0) {
        如果(elementsLength==0) {
            errorCode=U_INDEX_OUTOFBOUNDS_ERROR;
            返回;
        }
        uprv_sortArray(elements, elementsLength, static_cast<int32_t>(sizeof(BytesTrieElement)),
                      compareElementStrings, strings,
                      false,  // need not be a stable sort
                      &errorCode);
        如果(U_FAILURE(errorCode)) {
            返回;
        }
        // Duplicate strings are not allowed.
        StringPiece prev=elements[0].getString(*strings);
        为(int32_t i=1; i<elementsLength; ++i) {
            StringPiece current=elements[i].getString(*strings);
            如果(prev==current) {
                errorCode=U_ILLEGAL_ARGUMENT_ERROR;
                返回;
            }
            prev=current;
        }
    }
    // Create and byte-serialize the trie for the elements.
    bytesLength=0;
    int32_t capacity=strings->length();
    如果(capacity<1024) {
        capacity=1024;
    }
    如果(bytesCapacity<capacity) {
        uprv_free(bytes);
        bytes=static_cast<char *>(uprv_malloc(capacity));
        如果(bytes==nullptr) {
            errorCode=U_MEMORY_ALLOCATION_ERROR;
            bytesCapacity=0;
            返回;
        }
        bytesCapacity=capacity;
    }
    StringTrieBuilder::build(buildOption, elementsLength, errorCode);
    如果(bytes==nullptr) {
        errorCode=U_MEMORY_ALLOCATION_ERROR;
    }
}

BytesTrieBuilder&
BytesTrieBuilder::clear() U_LIFETIME_BOUND {
    strings->clear();
    elementsLength=0;
    bytesLength=0;
    返回 *this;
}

int32_t
BytesTrieBuilder::getElementStringLength(int32_t i) const {
    返回 elements[i].getStringLength(*strings);
}

char16_t
BytesTrieBuilder::getElementUnit(int32_t i, int32_t byteIndex) const {
    返回 static_cast<uint8_t>(elements[i].charAt(byteIndex, *strings));
}

int32_t
BytesTrieBuilder::getElementValue(int32_t i) const {
    返回 elements[i].getValue();
}

int32_t
BytesTrieBuilder::getLimitOfLinearMatch(int32_t first, int32_t last, int32_t byteIndex) const {
    const BytesTrieElement &firstElement=elements[first];
    const BytesTrieElement &lastElement=elements[last];
    int32_t minStringLength=firstElement.getStringLength(*strings);
    当(++byteIndex<minStringLength &&
            firstElement.charAt(byteIndex, *strings)==
            lastElement.charAt(byteIndex, *strings)) {}
    返回 byteIndex;
}

int32_t
BytesTrieBuilder::countElementUnits(int32_t start, int32_t limit, int32_t byteIndex) const {
    int32_t length=0;  // Number of different bytes at byteIndex.
    int32_t i=start;
    do {
        char byte=elements[i++].charAt(byteIndex, *strings);
        当(i<limit && byte==elements[i].charAt(byteIndex, *strings)) {
            ++i;
        }
        ++length;
    } 当(i<limit);
    返回 length;
}

int32_t
BytesTrieBuilder::skipElementsBySomeUnits(int32_t i, int32_t byteIndex, int32_t count) const {
    do {
        char byte=elements[i++].charAt(byteIndex, *strings);
        当(byte==elements[i].charAt(byteIndex, *strings)) {
            ++i;
        }
    } 当(--count>0);
    返回 i;
}

int32_t
BytesTrieBuilder::indexOfElementWithNextUnit(int32_t i, int32_t byteIndex, char16_t byte) const {
    char b = static_cast<char>(byte);
    当(b==elements[i].charAt(byteIndex, *strings)) {
        ++i;
    }
    返回 i;
}

BytesTrieBuilder::BTLinearMatchNode::BTLinearMatchNode(const char *bytes, int32_t len, Node *nextNode)
        : LinearMatchNode(len, nextNode), s(bytes) {
    hash=static_cast<int32_t>(
        static_cast<uint32_t>(hash)*37u + static_cast<uint32_t>(ustr_hashCharsN(bytes, len)));
}

bool
BytesTrieBuilder::BTLinearMatchNode::operator==(const Node &other) const {
    如果(this==&other) {
        返回 true;
    }
    如果(!LinearMatchNode::operator==(other)) {
        返回 false;
    }
    const BTLinearMatchNode &o=static_cast<const BTLinearMatchNode &>(other);
    返回 0==uprv_memcmp(s, o.s, length);
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
    返回 new BTLinearMatchNode(
            elements[i].getString(*strings).data()+byteIndex,
            length,
            nextNode);
}

UBool
BytesTrieBuilder::ensureCapacity(int32_t length) {
    如果(bytes==nullptr) {
        返回 false;  // previous memory allocation had failed
    }
    如果(length>bytesCapacity) {
        int32_t newCapacity=bytesCapacity;
        do {
            newCapacity*=2;
        } 当(newCapacity<=length);
        char *newBytes=static_cast<char *>(uprv_malloc(newCapacity));
        如果(newBytes==nullptr) {
            // unable to allocate memory
            uprv_free(bytes);
            bytes=nullptr;
            bytesCapacity=0;
            返回 false;
        }
        uprv_memcpy(newBytes+(newCapacity-bytesLength),
                    bytes+(bytesCapacity-bytesLength), bytesLength);
        uprv_free(bytes);
        bytes=newBytes;
        bytesCapacity=newCapacity;
    }
    返回 true;
}

int32_t
BytesTrieBuilder::write(int32_t byte) {
    int32_t newLength=bytesLength+1;
    如果(ensureCapacity(newLength)) {
        bytesLength=newLength;
        bytes[bytesCapacity - bytesLength] = static_cast<char>(byte);
    }
    返回 bytesLength;
}

int32_t
BytesTrieBuilder::write(const char *b, int32_t length) {
    int32_t newLength=bytesLength+length;
    如果(ensureCapacity(newLength)) {
        bytesLength=newLength;
        uprv_memcpy(bytes+(bytesCapacity-bytesLength), b, length);
    }
    返回 bytesLength;
}

int32_t
BytesTrieBuilder::writeElementUnits(int32_t i, int32_t byteIndex, int32_t length) {
    返回 write(elements[i].getString(*strings).data()+byteIndex, length);
}

int32_t
BytesTrieBuilder::writeValueAndFinal(int32_t i, UBool isFinal) {
    如果(0<=i && i<=BytesTrie::kMaxOneByteValue) {
        返回 write(((BytesTrie::kMinOneByteValueLead+i)<<1)|isFinal);
    }
    char intBytes[5];
    int32_t length=1;
    如果(i<0 || i>0xffffff) {
        intBytes[0] = static_cast<char>(BytesTrie::kFiveByteValueLead);
        intBytes[1] = static_cast<char>(static_cast<uint32_t>(i) >> 24);
        intBytes[2] = static_cast<char>(static_cast<uint32_t>(i) >> 16);
        intBytes[3] = static_cast<char>(static_cast<uint32_t>(i) >> 8);
        intBytes[4] = static_cast<char>(i);
        length=5;
    // } else if(i<=BytesTrie::kMaxOneByteValue) {
    //     intBytes[0]=(char)(BytesTrie::kMinOneByteValueLead+i);
    } 否则 {
        如果(i<=BytesTrie::kMaxTwoByteValue) {
            intBytes[0] = static_cast<char>(BytesTrie::kMinTwoByteValueLead + (i >> 8));
        } 否则 {
            如果(i<=BytesTrie::kMaxThreeByteValue) {
                intBytes[0] = static_cast<char>(BytesTrie::kMinThreeByteValueLead + (i >> 16));
            } 否则 {
                intBytes[0] = static_cast<char>(BytesTrie::kFourByteValueLead);
                intBytes[1] = static_cast<char>(i >> 16);
                length=2;
            }
            intBytes[length++] = static_cast<char>(i >> 8);
        }
        intBytes[length++] = static_cast<char>(i);
    }
    intBytes[0] = static_cast<char>((intBytes[0] << 1) | isFinal);
    返回 write(intBytes, length);
}

int32_t
BytesTrieBuilder::writeValueAndType(UBool hasValue, int32_t value, int32_t node) {
    int32_t offset=write(node);
    如果(hasValue) {
        offset=writeValueAndFinal(value, false);
    }
    返回 offset;
}

int32_t
BytesTrieBuilder::writeDeltaTo(int32_t jumpTarget) {
    int32_t i=bytesLength-jumpTarget;
    U_ASSERT(i>=0);
    如果(i<=BytesTrie::kMaxOneByteDelta) {
        返回 write(i);
    } 否则 {
        char intBytes[5];
        返回 write(intBytes, internalEncodeDelta(i, intBytes));
    }
}

int32_t
BytesTrieBuilder::internalEncodeDelta(int32_t i, char intBytes[]) {
    U_ASSERT(i>=0);
    如果(i<=BytesTrie::kMaxOneByteDelta) {
        intBytes[0] = static_cast<char>(i);
        返回 1;
    }
    int32_t length=1;
    如果(i<=BytesTrie::kMaxTwoByteDelta) {
        intBytes[0] = static_cast<char>(BytesTrie::kMinTwoByteDeltaLead + (i >> 8));
    } 否则 {
        如果(i<=BytesTrie::kMaxThreeByteDelta) {
            intBytes[0] = static_cast<char>(BytesTrie::kMinThreeByteDeltaLead + (i >> 16));
        } 否则 {
            如果(i<=0xffffff) {
                intBytes[0] = static_cast<char>(BytesTrie::kFourByteDeltaLead);
            } 否则 {
                intBytes[0] = static_cast<char>(BytesTrie::kFiveByteDeltaLead);
                intBytes[1] = static_cast<char>(i >> 24);
                length=2;
            }
            intBytes[length++] = static_cast<char>(i >> 16);
        }
        intBytes[length++] = static_cast<char>(i >> 8);
    }
    intBytes[length++] = static_cast<char>(i);
    返回 length;
}

U_NAMESPACE_END
