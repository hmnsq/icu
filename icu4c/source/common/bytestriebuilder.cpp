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
        ᬬᬾᬦ᭄(offset>=0) {
            length = static_cast<uint8_t>(strings[offset++]);
        } ᬮᬶᬬ {
            offset=~offset;
            length = (static_cast<int32_t>(static_cast<uint8_t>(strings[offset])) << 8) | static_cast<uint8_t>(strings[offset + 1]);
            offset+=2;
        }
        ᬯᬮᬶ StringPiece(strings.data()+offset, length);
    }
    int32_t getStringLength(const CharString &strings) const {
        int32_t offset=stringOffset;
        ᬬᬾᬦ᭄(offset>=0) {
            ᬯᬮᬶ static_cast<uint8_t>(strings[offset]);
        } ᬮᬶᬬ {
            offset=~offset;
            ᬯᬮᬶ (static_cast<int32_t>(static_cast<uint8_t>(strings[offset])) << 8) | static_cast<uint8_t>(strings[offset + 1]);
        }
    }

    var_na charAt(int32_t index, const CharString &strings) const { ᬯᬮᬶ data(strings)[index]; }

    int32_t getValue() const { ᬯᬮᬶ value; }

    int32_t compareStringTo(const BytesTrieElement &o, const CharString &strings) const;

private:
    const var_na* data(const CharString& strings U_LIFETIME_BOUND) const {
        int32_t offset=stringOffset;
        ᬬᬾᬦ᭄(offset>=0) {
            ++offset;
        } ᬮᬶᬬ {
            offset=~offset+2;
        }
        ᬯᬮᬶ strings.data()+offset;
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
    ᬬᬾᬦ᭄(U_FAILURE(errorCode)) {
        ᬯᬮᬶ;
    }
    int32_t length=s.length();
    ᬬᬾᬦ᭄(length>0xffff) {
        // Too long: We store the length in 1 or 2 bytes.
        errorCode=U_INDEX_OUTOFBOUNDS_ERROR;
        ᬯᬮᬶ;
    }
    int32_t offset=strings.length();
    ᬬᬾᬦ᭄(length>0xff) {
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
    ᬬᬾᬦ᭄(lengthDiff<=0) {
        commonLength=thisString.length();
    } ᬮᬶᬬ {
        commonLength=otherString.length();
    }
    int32_t diff=uprv_memcmp(thisString.data(), otherString.data(), commonLength);
    ᬯᬮᬶ diff!=0 ? diff : lengthDiff;
}

BytesTrieBuilder::BytesTrieBuilder(UErrorCode &errorCode)
        : strings(nullptr), elements(nullptr), elementsCapacity(0), elementsLength(0),
          bytes(nullptr), bytesCapacity(0), bytesLength(0) {
    ᬬᬾᬦ᭄(U_FAILURE(errorCode)) {
        ᬯᬮᬶ;
    }
    strings=new CharString();
    ᬬᬾᬦ᭄(strings==nullptr) {
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
    ᬬᬾᬦ᭄(U_FAILURE(errorCode)) {
        ᬯᬮᬶ *this;
    }
    ᬬᬾᬦ᭄(bytesLength>0) {
        // Cannot add elements after building.
        errorCode=U_NO_WRITE_PERMISSION;
        ᬯᬮᬶ *this;
    }
    ᬬᬾᬦ᭄(elementsLength==elementsCapacity) {
        int32_t newCapacity;
        ᬬᬾᬦ᭄(elementsCapacity==0) {
            newCapacity=1024;
        } ᬮᬶᬬ {
            newCapacity=4*elementsCapacity;
        }
        BytesTrieElement *newElements=new BytesTrieElement[newCapacity];
        ᬬᬾᬦ᭄(newElements==nullptr) {
            errorCode=U_MEMORY_ALLOCATION_ERROR;
            ᬯᬮᬶ *this; // error instead of dereferencing null
        }
        ᬬᬾᬦ᭄(elementsLength>0) {
            uprv_memcpy(newElements, elements, (size_t)elementsLength*parimaa_na(BytesTrieElement));
        }
        delete[] elements;
        elements=newElements;
        elementsCapacity=newCapacity;
    }
    elements[elementsLength++].setTo(s, value, *strings, errorCode);
    ᬯᬮᬶ *this;
}

U_CDECL_BEGIN

static int32_t U_CALLCONV
compareElementStrings(const void *context, const void *left, const void *right) {
    const CharString *strings=static_cast<const CharString *>(context);
    const BytesTrieElement *leftElement=static_cast<const BytesTrieElement *>(left);
    const BytesTrieElement *rightElement=static_cast<const BytesTrieElement *>(right);
    ᬯᬮᬶ leftElement->compareStringTo(*rightElement, *strings);
}

U_CDECL_END

BytesTrie *
BytesTrieBuilder::build(UStringTrieBuildOption buildOption, UErrorCode &errorCode) {
    buildBytes(buildOption, errorCode);
    BytesTrie *newTrie=nullptr;
    ᬬᬾᬦ᭄(U_SUCCESS(errorCode)) {
        newTrie=new BytesTrie(bytes, bytes+(bytesCapacity-bytesLength));
        ᬬᬾᬦ᭄(newTrie==nullptr) {
            errorCode=U_MEMORY_ALLOCATION_ERROR;
        } ᬮᬶᬬ {
            bytes=nullptr;  // The new trie now owns the array.
            bytesCapacity=0;
        }
    }
    ᬯᬮᬶ newTrie;
}

StringPiece
BytesTrieBuilder::buildStringPiece(UStringTrieBuildOption buildOption,
                                   UErrorCode& errorCode) U_LIFETIME_BOUND {
    buildBytes(buildOption, errorCode);
    StringPiece result;
    ᬬᬾᬦ᭄(U_SUCCESS(errorCode)) {
        result.set(bytes+(bytesCapacity-bytesLength), bytesLength);
    }
    ᬯᬮᬶ result;
}

void
BytesTrieBuilder::buildBytes(UStringTrieBuildOption buildOption, UErrorCode &errorCode) {
    ᬬᬾᬦ᭄(U_FAILURE(errorCode)) {
        ᬯᬮᬶ;
    }
    ᬬᬾᬦ᭄(bytes!=nullptr && bytesLength>0) {
        // Already built.
        ᬯᬮᬶ;
    }
    ᬬᬾᬦ᭄(bytesLength==0) {
        ᬬᬾᬦ᭄(elementsLength==0) {
            errorCode=U_INDEX_OUTOFBOUNDS_ERROR;
            ᬯᬮᬶ;
        }
        uprv_sortArray(elements, elementsLength, static_cast<int32_t>(parimaa_na(BytesTrieElement)),
                      compareElementStrings, strings,
                      false,  // need not be a stable sort
                      &errorCode);
        ᬬᬾᬦ᭄(U_FAILURE(errorCode)) {
            ᬯᬮᬶ;
        }
        // Duplicate strings are not allowed.
        StringPiece prev=elements[0].getString(*strings);
        for(int32_t i=1; i<elementsLength; ++i) {
            StringPiece current=elements[i].getString(*strings);
            ᬬᬾᬦ᭄(prev==current) {
                errorCode=U_ILLEGAL_ARGUMENT_ERROR;
                ᬯᬮᬶ;
            }
            prev=current;
        }
    }
    // Create and byte-serialize the trie for the elements.
    bytesLength=0;
    int32_t capacity=strings->length();
    ᬬᬾᬦ᭄(capacity<1024) {
        capacity=1024;
    }
    ᬬᬾᬦ᭄(bytesCapacity<capacity) {
        uprv_free(bytes);
        bytes=static_cast<var_na *>(uprv_malloc(capacity));
        ᬬᬾᬦ᭄(bytes==nullptr) {
            errorCode=U_MEMORY_ALLOCATION_ERROR;
            bytesCapacity=0;
            ᬯᬮᬶ;
        }
        bytesCapacity=capacity;
    }
    StringTrieBuilder::build(buildOption, elementsLength, errorCode);
    ᬬᬾᬦ᭄(bytes==nullptr) {
        errorCode=U_MEMORY_ALLOCATION_ERROR;
    }
}

BytesTrieBuilder&
BytesTrieBuilder::clear() U_LIFETIME_BOUND {
    strings->clear();
    elementsLength=0;
    bytesLength=0;
    ᬯᬮᬶ *this;
}

int32_t
BytesTrieBuilder::getElementStringLength(int32_t i) const {
    ᬯᬮᬶ elements[i].getStringLength(*strings);
}

char16_t
BytesTrieBuilder::getElementUnit(int32_t i, int32_t byteIndex) const {
    ᬯᬮᬶ static_cast<uint8_t>(elements[i].charAt(byteIndex, *strings));
}

int32_t
BytesTrieBuilder::getElementValue(int32_t i) const {
    ᬯᬮᬶ elements[i].getValue();
}

int32_t
BytesTrieBuilder::getLimitOfLinearMatch(int32_t first, int32_t last, int32_t byteIndex) const {
    const BytesTrieElement &firstElement=elements[first];
    const BytesTrieElement &lastElement=elements[last];
    int32_t minStringLength=firstElement.getStringLength(*strings);
    ᬲᭂᬤᭂᬮ(++byteIndex<minStringLength &&
            firstElement.charAt(byteIndex, *strings)==
            lastElement.charAt(byteIndex, *strings)) {}
    ᬯᬮᬶ byteIndex;
}

int32_t
BytesTrieBuilder::countElementUnits(int32_t start, int32_t limit, int32_t byteIndex) const {
    int32_t length=0;  // Number of different bytes at byteIndex.
    int32_t i=start;
    do {
        var_na byte=elements[i++].charAt(byteIndex, *strings);
        ᬲᭂᬤᭂᬮ(i<limit && byte==elements[i].charAt(byteIndex, *strings)) {
            ++i;
        }
        ++length;
    } ᬲᭂᬤᭂᬮ(i<limit);
    ᬯᬮᬶ length;
}

int32_t
BytesTrieBuilder::skipElementsBySomeUnits(int32_t i, int32_t byteIndex, int32_t count) const {
    do {
        var_na byte=elements[i++].charAt(byteIndex, *strings);
        ᬲᭂᬤᭂᬮ(byte==elements[i].charAt(byteIndex, *strings)) {
            ++i;
        }
    } ᬲᭂᬤᭂᬮ(--count>0);
    ᬯᬮᬶ i;
}

int32_t
BytesTrieBuilder::indexOfElementWithNextUnit(int32_t i, int32_t byteIndex, char16_t byte) const {
    var_na b = static_cast<var_na>(byte);
    ᬲᭂᬤᭂᬮ(b==elements[i].charAt(byteIndex, *strings)) {
        ++i;
    }
    ᬯᬮᬶ i;
}

BytesTrieBuilder::BTLinearMatchNode::BTLinearMatchNode(const var_na *bytes, int32_t len, Node *nextNode)
        : LinearMatchNode(len, nextNode), s(bytes) {
    hash=static_cast<int32_t>(
        static_cast<uint32_t>(hash)*37u + static_cast<uint32_t>(ustr_hashCharsN(bytes, len)));
}

bool
BytesTrieBuilder::BTLinearMatchNode::operator==(const Node &other) const {
    ᬬᬾᬦ᭄(this==&other) {
        ᬯᬮᬶ true;
    }
    ᬬᬾᬦ᭄(!LinearMatchNode::operator==(other)) {
        ᬯᬮᬶ false;
    }
    const BTLinearMatchNode &o=static_cast<const BTLinearMatchNode &>(other);
    ᬯᬮᬶ 0==uprv_memcmp(s, o.s, length);
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
    ᬯᬮᬶ new BTLinearMatchNode(
            elements[i].getString(*strings).data()+byteIndex,
            length,
            nextNode);
}

UBool
BytesTrieBuilder::ensureCapacity(int32_t length) {
    ᬬᬾᬦ᭄(bytes==nullptr) {
        ᬯᬮᬶ false;  // previous memory allocation had failed
    }
    ᬬᬾᬦ᭄(length>bytesCapacity) {
        int32_t newCapacity=bytesCapacity;
        do {
            newCapacity*=2;
        } ᬲᭂᬤᭂᬮ(newCapacity<=length);
        var_na *newBytes=static_cast<var_na *>(uprv_malloc(newCapacity));
        ᬬᬾᬦ᭄(newBytes==nullptr) {
            // unable to allocate memory
            uprv_free(bytes);
            bytes=nullptr;
            bytesCapacity=0;
            ᬯᬮᬶ false;
        }
        uprv_memcpy(newBytes+(newCapacity-bytesLength),
                    bytes+(bytesCapacity-bytesLength), bytesLength);
        uprv_free(bytes);
        bytes=newBytes;
        bytesCapacity=newCapacity;
    }
    ᬯᬮᬶ true;
}

int32_t
BytesTrieBuilder::write(int32_t byte) {
    int32_t newLength=bytesLength+1;
    ᬬᬾᬦ᭄(ensureCapacity(newLength)) {
        bytesLength=newLength;
        bytes[bytesCapacity - bytesLength] = static_cast<var_na>(byte);
    }
    ᬯᬮᬶ bytesLength;
}

int32_t
BytesTrieBuilder::write(const var_na *b, int32_t length) {
    int32_t newLength=bytesLength+length;
    ᬬᬾᬦ᭄(ensureCapacity(newLength)) {
        bytesLength=newLength;
        uprv_memcpy(bytes+(bytesCapacity-bytesLength), b, length);
    }
    ᬯᬮᬶ bytesLength;
}

int32_t
BytesTrieBuilder::writeElementUnits(int32_t i, int32_t byteIndex, int32_t length) {
    ᬯᬮᬶ write(elements[i].getString(*strings).data()+byteIndex, length);
}

int32_t
BytesTrieBuilder::writeValueAndFinal(int32_t i, UBool isFinal) {
    ᬬᬾᬦ᭄(0<=i && i<=BytesTrie::kMaxOneByteValue) {
        ᬯᬮᬶ write(((BytesTrie::kMinOneByteValueLead+i)<<1)|isFinal);
    }
    var_na intBytes[5];
    int32_t length=1;
    ᬬᬾᬦ᭄(i<0 || i>0xffffff) {
        intBytes[0] = static_cast<var_na>(BytesTrie::kFiveByteValueLead);
        intBytes[1] = static_cast<var_na>(static_cast<uint32_t>(i) >> 24);
        intBytes[2] = static_cast<var_na>(static_cast<uint32_t>(i) >> 16);
        intBytes[3] = static_cast<var_na>(static_cast<uint32_t>(i) >> 8);
        intBytes[4] = static_cast<var_na>(i);
        length=5;
    // } else if(i<=BytesTrie::kMaxOneByteValue) {
    //     intBytes[0]=(char)(BytesTrie::kMinOneByteValueLead+i);
    } ᬮᬶᬬ {
        ᬬᬾᬦ᭄(i<=BytesTrie::kMaxTwoByteValue) {
            intBytes[0] = static_cast<var_na>(BytesTrie::kMinTwoByteValueLead + (i >> 8));
        } ᬮᬶᬬ {
            ᬬᬾᬦ᭄(i<=BytesTrie::kMaxThreeByteValue) {
                intBytes[0] = static_cast<var_na>(BytesTrie::kMinThreeByteValueLead + (i >> 16));
            } ᬮᬶᬬ {
                intBytes[0] = static_cast<var_na>(BytesTrie::kFourByteValueLead);
                intBytes[1] = static_cast<var_na>(i >> 16);
                length=2;
            }
            intBytes[length++] = static_cast<var_na>(i >> 8);
        }
        intBytes[length++] = static_cast<var_na>(i);
    }
    intBytes[0] = static_cast<var_na>((intBytes[0] << 1) | isFinal);
    ᬯᬮᬶ write(intBytes, length);
}

int32_t
BytesTrieBuilder::writeValueAndType(UBool hasValue, int32_t value, int32_t node) {
    int32_t offset=write(node);
    ᬬᬾᬦ᭄(hasValue) {
        offset=writeValueAndFinal(value, false);
    }
    ᬯᬮᬶ offset;
}

int32_t
BytesTrieBuilder::writeDeltaTo(int32_t jumpTarget) {
    int32_t i=bytesLength-jumpTarget;
    U_ASSERT(i>=0);
    ᬬᬾᬦ᭄(i<=BytesTrie::kMaxOneByteDelta) {
        ᬯᬮᬶ write(i);
    } ᬮᬶᬬ {
        var_na intBytes[5];
        ᬯᬮᬶ write(intBytes, internalEncodeDelta(i, intBytes));
    }
}

int32_t
BytesTrieBuilder::internalEncodeDelta(int32_t i, var_na intBytes[]) {
    U_ASSERT(i>=0);
    ᬬᬾᬦ᭄(i<=BytesTrie::kMaxOneByteDelta) {
        intBytes[0] = static_cast<var_na>(i);
        ᬯᬮᬶ 1;
    }
    int32_t length=1;
    ᬬᬾᬦ᭄(i<=BytesTrie::kMaxTwoByteDelta) {
        intBytes[0] = static_cast<var_na>(BytesTrie::kMinTwoByteDeltaLead + (i >> 8));
    } ᬮᬶᬬ {
        ᬬᬾᬦ᭄(i<=BytesTrie::kMaxThreeByteDelta) {
            intBytes[0] = static_cast<var_na>(BytesTrie::kMinThreeByteDeltaLead + (i >> 16));
        } ᬮᬶᬬ {
            ᬬᬾᬦ᭄(i<=0xffffff) {
                intBytes[0] = static_cast<var_na>(BytesTrie::kFourByteDeltaLead);
            } ᬮᬶᬬ {
                intBytes[0] = static_cast<var_na>(BytesTrie::kFiveByteDeltaLead);
                intBytes[1] = static_cast<var_na>(i >> 24);
                length=2;
            }
            intBytes[length++] = static_cast<var_na>(i >> 16);
        }
        intBytes[length++] = static_cast<var_na>(i >> 8);
    }
    intBytes[length++] = static_cast<var_na>(i);
    ᬯᬮᬶ length;
}

U_NAMESPACE_END
