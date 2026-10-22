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
        nếu(offset>=0) {
            length = static_cast<uint8_t>(strings[offset++]);
        } không_thì {
            offset=~offset;
            length = (static_cast<int32_t>(static_cast<uint8_t>(strings[offset])) << 8) | static_cast<uint8_t>(strings[offset + 1]);
            offset+=2;
        }
        trả_về StringPiece(strings.data()+offset, length);
    }
    int32_t getStringLength(const CharString &strings) const {
        int32_t offset=stringOffset;
        nếu(offset>=0) {
            trả_về static_cast<uint8_t>(strings[offset]);
        } không_thì {
            offset=~offset;
            trả_về (static_cast<int32_t>(static_cast<uint8_t>(strings[offset])) << 8) | static_cast<uint8_t>(strings[offset + 1]);
        }
    }

    char charAt(int32_t index, const CharString &strings) const { trả_về data(strings)[index]; }

    int32_t getValue() const { trả_về value; }

    int32_t compareStringTo(const BytesTrieElement &o, const CharString &strings) const;

private:
    const char* data(const CharString& strings U_LIFETIME_BOUND) const {
        int32_t offset=stringOffset;
        nếu(offset>=0) {
            ++offset;
        } không_thì {
            offset=~offset+2;
        }
        trả_về strings.data()+offset;
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
    nếu(U_FAILURE(errorCode)) {
        trả_về;
    }
    int32_t length=s.length();
    nếu(length>0xffff) {
        // Too long: We store the length in 1 or 2 bytes.
        errorCode=U_INDEX_OUTOFBOUNDS_ERROR;
        trả_về;
    }
    int32_t offset=strings.length();
    nếu(length>0xff) {
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
    nếu(lengthDiff<=0) {
        commonLength=thisString.length();
    } không_thì {
        commonLength=otherString.length();
    }
    int32_t diff=uprv_memcmp(thisString.data(), otherString.data(), commonLength);
    trả_về diff!=0 ? diff : lengthDiff;
}

BytesTrieBuilder::BytesTrieBuilder(UErrorCode &errorCode)
        : strings(nullptr), elements(nullptr), elementsCapacity(0), elementsLength(0),
          bytes(nullptr), bytesCapacity(0), bytesLength(0) {
    nếu(U_FAILURE(errorCode)) {
        trả_về;
    }
    strings=new CharString();
    nếu(strings==nullptr) {
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
    nếu(U_FAILURE(errorCode)) {
        trả_về *this;
    }
    nếu(bytesLength>0) {
        // Cannot add elements after building.
        errorCode=U_NO_WRITE_PERMISSION;
        trả_về *this;
    }
    nếu(elementsLength==elementsCapacity) {
        int32_t newCapacity;
        nếu(elementsCapacity==0) {
            newCapacity=1024;
        } không_thì {
            newCapacity=4*elementsCapacity;
        }
        BytesTrieElement *newElements=new BytesTrieElement[newCapacity];
        nếu(newElements==nullptr) {
            errorCode=U_MEMORY_ALLOCATION_ERROR;
            trả_về *this; // error instead of dereferencing null
        }
        nếu(elementsLength>0) {
            uprv_memcpy(newElements, elements, (size_t)elementsLength*sizeof(BytesTrieElement));
        }
        delete[] elements;
        elements=newElements;
        elementsCapacity=newCapacity;
    }
    elements[elementsLength++].setTo(s, value, *strings, errorCode);
    trả_về *this;
}

U_CDECL_BEGIN

static int32_t U_CALLCONV
compareElementStrings(const void *context, const void *left, const void *right) {
    const CharString *strings=static_cast<const CharString *>(context);
    const BytesTrieElement *leftElement=static_cast<const BytesTrieElement *>(left);
    const BytesTrieElement *rightElement=static_cast<const BytesTrieElement *>(right);
    trả_về leftElement->compareStringTo(*rightElement, *strings);
}

U_CDECL_END

BytesTrie *
BytesTrieBuilder::build(UStringTrieBuildOption buildOption, UErrorCode &errorCode) {
    buildBytes(buildOption, errorCode);
    BytesTrie *newTrie=nullptr;
    nếu(U_SUCCESS(errorCode)) {
        newTrie=new BytesTrie(bytes, bytes+(bytesCapacity-bytesLength));
        nếu(newTrie==nullptr) {
            errorCode=U_MEMORY_ALLOCATION_ERROR;
        } không_thì {
            bytes=nullptr;  // The new trie now owns the array.
            bytesCapacity=0;
        }
    }
    trả_về newTrie;
}

StringPiece
BytesTrieBuilder::buildStringPiece(UStringTrieBuildOption buildOption,
                                   UErrorCode& errorCode) U_LIFETIME_BOUND {
    buildBytes(buildOption, errorCode);
    StringPiece result;
    nếu(U_SUCCESS(errorCode)) {
        result.set(bytes+(bytesCapacity-bytesLength), bytesLength);
    }
    trả_về result;
}

void
BytesTrieBuilder::buildBytes(UStringTrieBuildOption buildOption, UErrorCode &errorCode) {
    nếu(U_FAILURE(errorCode)) {
        trả_về;
    }
    nếu(bytes!=nullptr && bytesLength>0) {
        // Already built.
        trả_về;
    }
    nếu(bytesLength==0) {
        nếu(elementsLength==0) {
            errorCode=U_INDEX_OUTOFBOUNDS_ERROR;
            trả_về;
        }
        uprv_sortArray(elements, elementsLength, static_cast<int32_t>(sizeof(BytesTrieElement)),
                      compareElementStrings, strings,
                      false,  // need not be a stable sort
                      &errorCode);
        nếu(U_FAILURE(errorCode)) {
            trả_về;
        }
        // Duplicate strings are not allowed.
        StringPiece prev=elements[0].getString(*strings);
        cho(int32_t i=1; i<elementsLength; ++i) {
            StringPiece current=elements[i].getString(*strings);
            nếu(prev==current) {
                errorCode=U_ILLEGAL_ARGUMENT_ERROR;
                trả_về;
            }
            prev=current;
        }
    }
    // Create and byte-serialize the trie for the elements.
    bytesLength=0;
    int32_t capacity=strings->length();
    nếu(capacity<1024) {
        capacity=1024;
    }
    nếu(bytesCapacity<capacity) {
        uprv_free(bytes);
        bytes=static_cast<char *>(uprv_malloc(capacity));
        nếu(bytes==nullptr) {
            errorCode=U_MEMORY_ALLOCATION_ERROR;
            bytesCapacity=0;
            trả_về;
        }
        bytesCapacity=capacity;
    }
    StringTrieBuilder::build(buildOption, elementsLength, errorCode);
    nếu(bytes==nullptr) {
        errorCode=U_MEMORY_ALLOCATION_ERROR;
    }
}

BytesTrieBuilder&
BytesTrieBuilder::clear() U_LIFETIME_BOUND {
    strings->clear();
    elementsLength=0;
    bytesLength=0;
    trả_về *this;
}

int32_t
BytesTrieBuilder::getElementStringLength(int32_t i) const {
    trả_về elements[i].getStringLength(*strings);
}

char16_t
BytesTrieBuilder::getElementUnit(int32_t i, int32_t byteIndex) const {
    trả_về static_cast<uint8_t>(elements[i].charAt(byteIndex, *strings));
}

int32_t
BytesTrieBuilder::getElementValue(int32_t i) const {
    trả_về elements[i].getValue();
}

int32_t
BytesTrieBuilder::getLimitOfLinearMatch(int32_t first, int32_t last, int32_t byteIndex) const {
    const BytesTrieElement &firstElement=elements[first];
    const BytesTrieElement &lastElement=elements[last];
    int32_t minStringLength=firstElement.getStringLength(*strings);
    trong_khi(++byteIndex<minStringLength &&
            firstElement.charAt(byteIndex, *strings)==
            lastElement.charAt(byteIndex, *strings)) {}
    trả_về byteIndex;
}

int32_t
BytesTrieBuilder::countElementUnits(int32_t start, int32_t limit, int32_t byteIndex) const {
    int32_t length=0;  // Number of different bytes at byteIndex.
    int32_t i=start;
    do {
        char byte=elements[i++].charAt(byteIndex, *strings);
        trong_khi(i<limit && byte==elements[i].charAt(byteIndex, *strings)) {
            ++i;
        }
        ++length;
    } trong_khi(i<limit);
    trả_về length;
}

int32_t
BytesTrieBuilder::skipElementsBySomeUnits(int32_t i, int32_t byteIndex, int32_t count) const {
    do {
        char byte=elements[i++].charAt(byteIndex, *strings);
        trong_khi(byte==elements[i].charAt(byteIndex, *strings)) {
            ++i;
        }
    } trong_khi(--count>0);
    trả_về i;
}

int32_t
BytesTrieBuilder::indexOfElementWithNextUnit(int32_t i, int32_t byteIndex, char16_t byte) const {
    char b = static_cast<char>(byte);
    trong_khi(b==elements[i].charAt(byteIndex, *strings)) {
        ++i;
    }
    trả_về i;
}

BytesTrieBuilder::BTLinearMatchNode::BTLinearMatchNode(const char *bytes, int32_t len, Node *nextNode)
        : LinearMatchNode(len, nextNode), s(bytes) {
    hash=static_cast<int32_t>(
        static_cast<uint32_t>(hash)*37u + static_cast<uint32_t>(ustr_hashCharsN(bytes, len)));
}

bool
BytesTrieBuilder::BTLinearMatchNode::operator==(const Node &other) const {
    nếu(this==&other) {
        trả_về true;
    }
    nếu(!LinearMatchNode::operator==(other)) {
        trả_về false;
    }
    const BTLinearMatchNode &o=static_cast<const BTLinearMatchNode &>(other);
    trả_về 0==uprv_memcmp(s, o.s, length);
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
    trả_về new BTLinearMatchNode(
            elements[i].getString(*strings).data()+byteIndex,
            length,
            nextNode);
}

UBool
BytesTrieBuilder::ensureCapacity(int32_t length) {
    nếu(bytes==nullptr) {
        trả_về false;  // previous memory allocation had failed
    }
    nếu(length>bytesCapacity) {
        int32_t newCapacity=bytesCapacity;
        do {
            newCapacity*=2;
        } trong_khi(newCapacity<=length);
        char *newBytes=static_cast<char *>(uprv_malloc(newCapacity));
        nếu(newBytes==nullptr) {
            // unable to allocate memory
            uprv_free(bytes);
            bytes=nullptr;
            bytesCapacity=0;
            trả_về false;
        }
        uprv_memcpy(newBytes+(newCapacity-bytesLength),
                    bytes+(bytesCapacity-bytesLength), bytesLength);
        uprv_free(bytes);
        bytes=newBytes;
        bytesCapacity=newCapacity;
    }
    trả_về true;
}

int32_t
BytesTrieBuilder::write(int32_t byte) {
    int32_t newLength=bytesLength+1;
    nếu(ensureCapacity(newLength)) {
        bytesLength=newLength;
        bytes[bytesCapacity - bytesLength] = static_cast<char>(byte);
    }
    trả_về bytesLength;
}

int32_t
BytesTrieBuilder::write(const char *b, int32_t length) {
    int32_t newLength=bytesLength+length;
    nếu(ensureCapacity(newLength)) {
        bytesLength=newLength;
        uprv_memcpy(bytes+(bytesCapacity-bytesLength), b, length);
    }
    trả_về bytesLength;
}

int32_t
BytesTrieBuilder::writeElementUnits(int32_t i, int32_t byteIndex, int32_t length) {
    trả_về write(elements[i].getString(*strings).data()+byteIndex, length);
}

int32_t
BytesTrieBuilder::writeValueAndFinal(int32_t i, UBool isFinal) {
    nếu(0<=i && i<=BytesTrie::kMaxOneByteValue) {
        trả_về write(((BytesTrie::kMinOneByteValueLead+i)<<1)|isFinal);
    }
    char intBytes[5];
    int32_t length=1;
    nếu(i<0 || i>0xffffff) {
        intBytes[0] = static_cast<char>(BytesTrie::kFiveByteValueLead);
        intBytes[1] = static_cast<char>(static_cast<uint32_t>(i) >> 24);
        intBytes[2] = static_cast<char>(static_cast<uint32_t>(i) >> 16);
        intBytes[3] = static_cast<char>(static_cast<uint32_t>(i) >> 8);
        intBytes[4] = static_cast<char>(i);
        length=5;
    // } else if(i<=BytesTrie::kMaxOneByteValue) {
    //     intBytes[0]=(char)(BytesTrie::kMinOneByteValueLead+i);
    } không_thì {
        nếu(i<=BytesTrie::kMaxTwoByteValue) {
            intBytes[0] = static_cast<char>(BytesTrie::kMinTwoByteValueLead + (i >> 8));
        } không_thì {
            nếu(i<=BytesTrie::kMaxThreeByteValue) {
                intBytes[0] = static_cast<char>(BytesTrie::kMinThreeByteValueLead + (i >> 16));
            } không_thì {
                intBytes[0] = static_cast<char>(BytesTrie::kFourByteValueLead);
                intBytes[1] = static_cast<char>(i >> 16);
                length=2;
            }
            intBytes[length++] = static_cast<char>(i >> 8);
        }
        intBytes[length++] = static_cast<char>(i);
    }
    intBytes[0] = static_cast<char>((intBytes[0] << 1) | isFinal);
    trả_về write(intBytes, length);
}

int32_t
BytesTrieBuilder::writeValueAndType(UBool hasValue, int32_t value, int32_t node) {
    int32_t offset=write(node);
    nếu(hasValue) {
        offset=writeValueAndFinal(value, false);
    }
    trả_về offset;
}

int32_t
BytesTrieBuilder::writeDeltaTo(int32_t jumpTarget) {
    int32_t i=bytesLength-jumpTarget;
    U_ASSERT(i>=0);
    nếu(i<=BytesTrie::kMaxOneByteDelta) {
        trả_về write(i);
    } không_thì {
        char intBytes[5];
        trả_về write(intBytes, internalEncodeDelta(i, intBytes));
    }
}

int32_t
BytesTrieBuilder::internalEncodeDelta(int32_t i, char intBytes[]) {
    U_ASSERT(i>=0);
    nếu(i<=BytesTrie::kMaxOneByteDelta) {
        intBytes[0] = static_cast<char>(i);
        trả_về 1;
    }
    int32_t length=1;
    nếu(i<=BytesTrie::kMaxTwoByteDelta) {
        intBytes[0] = static_cast<char>(BytesTrie::kMinTwoByteDeltaLead + (i >> 8));
    } không_thì {
        nếu(i<=BytesTrie::kMaxThreeByteDelta) {
            intBytes[0] = static_cast<char>(BytesTrie::kMinThreeByteDeltaLead + (i >> 16));
        } không_thì {
            nếu(i<=0xffffff) {
                intBytes[0] = static_cast<char>(BytesTrie::kFourByteDeltaLead);
            } không_thì {
                intBytes[0] = static_cast<char>(BytesTrie::kFiveByteDeltaLead);
                intBytes[1] = static_cast<char>(i >> 24);
                length=2;
            }
            intBytes[length++] = static_cast<char>(i >> 16);
        }
        intBytes[length++] = static_cast<char>(i >> 8);
    }
    intBytes[length++] = static_cast<char>(i);
    trả_về length;
}

U_NAMESPACE_END
