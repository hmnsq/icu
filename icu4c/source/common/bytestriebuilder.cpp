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

#ضمن "unicode/utypes.h"
#ضمن "unicode/bytestrie.h"
#ضمن "unicode/bytestriebuilder.h"
#ضمن "unicode/stringpiece.h"
#ضمن "charstr.h"
#ضمن "cmemory.h"
#ضمن "uhash.h"
#ضمن "uarrsort.h"
#ضمن "uassert.h"
#ضمن "ustr_imp.h"

U_NAMESPACE_BEGIN

/*
 * Note: This builder implementation stores (bytes, value) pairs with full copies
 * of the byte sequences, until the BytesTrie is built.
 * It might(!) take less memory if we collected the data in a temporary, dynamic trie.
 */

מחלקה BytesTrieElement : عام UMemory {
عام:
    // Use compiler's default constructor, initializes nothing.

    فراغ setTo(StringPiece s, int32_t val, CharString &strings, UErrorCode &errorCode);

    StringPiece getString(ثابت CharString& strings U_LIFETIME_BOUND) ثابت {
        int32_t offset=stringOffset;
        int32_t length;
        אם(offset>=0) {
            length = static_cast<uint8_t>(strings[offset++]);
        } אחרת {
            offset=~offset;
            length = (static_cast<int32_t>(static_cast<uint8_t>(strings[offset])) << 8) | static_cast<uint8_t>(strings[offset + 1]);
            offset+=2;
        }
        החזר StringPiece(strings.data()+offset, length);
    }
    int32_t getStringLength(ثابت CharString &strings) ثابت {
        int32_t offset=stringOffset;
        אם(offset>=0) {
            החזר static_cast<uint8_t>(strings[offset]);
        } אחרת {
            offset=~offset;
            החזר (static_cast<int32_t>(static_cast<uint8_t>(strings[offset])) << 8) | static_cast<uint8_t>(strings[offset + 1]);
        }
    }

    حرف charAt(int32_t index, ثابت CharString &strings) ثابت { החזר data(strings)[index]; }

    int32_t getValue() ثابت { החזר value; }

    int32_t compareStringTo(ثابت BytesTrieElement &o, ثابت CharString &strings) ثابت;

خاص:
    ثابت حرف* data(ثابت CharString& strings U_LIFETIME_BOUND) ثابت {
        int32_t offset=stringOffset;
        אם(offset>=0) {
            ++offset;
        } אחרת {
            offset=~offset+2;
        }
        החזר strings.data()+offset;
    }

    // If the stringOffset is non-negative, then the first strings byte contains
    // the string length.
    // If the stringOffset is negative, then the first two strings bytes contain
    // the string length (big-endian), and the offset needs to be bit-inverted.
    // (Compared with a stringLength field here, this saves 3 bytes per string for most strings.)
    int32_t stringOffset;
    int32_t value;
};

فراغ
BytesTrieElement::setTo(StringPiece s, int32_t val,
                        CharString &strings, UErrorCode &errorCode) {
    אם(U_FAILURE(errorCode)) {
        החזר;
    }
    int32_t length=s.length();
    אם(length>0xffff) {
        // Too long: We store the length in 1 or 2 bytes.
        errorCode=U_INDEX_OUTOFBOUNDS_ERROR;
        החזר;
    }
    int32_t offset=strings.length();
    אם(length>0xff) {
        offset=~offset;
        strings.append(static_cast<حرف>(length >> 8), errorCode);
    }
    strings.append(static_cast<حرف>(length), errorCode);
    stringOffset=offset;
    value=val;
    strings.append(s, errorCode);
}

int32_t
BytesTrieElement::compareStringTo(ثابت BytesTrieElement &other, ثابت CharString &strings) ثابت {
    // TODO: add StringPiece::compare(), see ticket #8187
    StringPiece thisString=getString(strings);
    StringPiece otherString=other.getString(strings);
    int32_t lengthDiff=thisString.length()-otherString.length();
    int32_t commonLength;
    אם(lengthDiff<=0) {
        commonLength=thisString.length();
    } אחרת {
        commonLength=otherString.length();
    }
    int32_t diff=uprv_memcmp(thisString.data(), otherString.data(), commonLength);
    החזר diff!=0 ? diff : lengthDiff;
}

BytesTrieBuilder::BytesTrieBuilder(UErrorCode &errorCode)
        : strings(nullptr), elements(nullptr), elementsCapacity(0), elementsLength(0),
          bytes(nullptr), bytesCapacity(0), bytesLength(0) {
    אם(U_FAILURE(errorCode)) {
        החזר;
    }
    strings=جديد CharString();
    אם(strings==nullptr) {
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
    אם(U_FAILURE(errorCode)) {
        החזר *هذا;
    }
    אם(bytesLength>0) {
        // Cannot add elements after building.
        errorCode=U_NO_WRITE_PERMISSION;
        החזר *هذا;
    }
    אם(elementsLength==elementsCapacity) {
        int32_t newCapacity;
        אם(elementsCapacity==0) {
            newCapacity=1024;
        } אחרת {
            newCapacity=4*elementsCapacity;
        }
        BytesTrieElement *newElements=جديد BytesTrieElement[newCapacity];
        אם(newElements==nullptr) {
            errorCode=U_MEMORY_ALLOCATION_ERROR;
            החזר *هذا; // error instead of dereferencing null
        }
        אם(elementsLength>0) {
            uprv_memcpy(newElements, elements, (size_t)elementsLength*حجم(BytesTrieElement));
        }
        delete[] elements;
        elements=newElements;
        elementsCapacity=newCapacity;
    }
    elements[elementsLength++].setTo(s, value, *strings, errorCode);
    החזר *هذا;
}

U_CDECL_BEGIN

ساكن int32_t U_CALLCONV
compareElementStrings(ثابت فراغ *context, ثابت فراغ *left, ثابت فراغ *right) {
    ثابت CharString *strings=static_cast<ثابت CharString *>(context);
    ثابت BytesTrieElement *leftElement=static_cast<ثابت BytesTrieElement *>(left);
    ثابت BytesTrieElement *rightElement=static_cast<ثابت BytesTrieElement *>(right);
    החזר leftElement->compareStringTo(*rightElement, *strings);
}

U_CDECL_END

BytesTrie *
BytesTrieBuilder::build(UStringTrieBuildOption buildOption, UErrorCode &errorCode) {
    buildBytes(buildOption, errorCode);
    BytesTrie *newTrie=nullptr;
    אם(U_SUCCESS(errorCode)) {
        newTrie=جديد BytesTrie(bytes, bytes+(bytesCapacity-bytesLength));
        אם(newTrie==nullptr) {
            errorCode=U_MEMORY_ALLOCATION_ERROR;
        } אחרת {
            bytes=nullptr;  // The new trie now owns the array.
            bytesCapacity=0;
        }
    }
    החזר newTrie;
}

StringPiece
BytesTrieBuilder::buildStringPiece(UStringTrieBuildOption buildOption,
                                   UErrorCode& errorCode) U_LIFETIME_BOUND {
    buildBytes(buildOption, errorCode);
    StringPiece result;
    אם(U_SUCCESS(errorCode)) {
        result.set(bytes+(bytesCapacity-bytesLength), bytesLength);
    }
    החזר result;
}

فراغ
BytesTrieBuilder::buildBytes(UStringTrieBuildOption buildOption, UErrorCode &errorCode) {
    אם(U_FAILURE(errorCode)) {
        החזר;
    }
    אם(bytes!=nullptr && bytesLength>0) {
        // Already built.
        החזר;
    }
    אם(bytesLength==0) {
        אם(elementsLength==0) {
            errorCode=U_INDEX_OUTOFBOUNDS_ERROR;
            החזר;
        }
        uprv_sortArray(elements, elementsLength, static_cast<int32_t>(حجم(BytesTrieElement)),
                      compareElementStrings, strings,
                      שקר,  // need not be a stable sort
                      &errorCode);
        אם(U_FAILURE(errorCode)) {
            החזר;
        }
        // Duplicate strings are not allowed.
        StringPiece prev=elements[0].getString(*strings);
        עבור(int32_t i=1; i<elementsLength; ++i) {
            StringPiece current=elements[i].getString(*strings);
            אם(prev==current) {
                errorCode=U_ILLEGAL_ARGUMENT_ERROR;
                החזר;
            }
            prev=current;
        }
    }
    // Create and byte-serialize the trie for the elements.
    bytesLength=0;
    int32_t capacity=strings->length();
    אם(capacity<1024) {
        capacity=1024;
    }
    אם(bytesCapacity<capacity) {
        uprv_free(bytes);
        bytes=static_cast<حرف *>(uprv_malloc(capacity));
        אם(bytes==nullptr) {
            errorCode=U_MEMORY_ALLOCATION_ERROR;
            bytesCapacity=0;
            החזר;
        }
        bytesCapacity=capacity;
    }
    StringTrieBuilder::build(buildOption, elementsLength, errorCode);
    אם(bytes==nullptr) {
        errorCode=U_MEMORY_ALLOCATION_ERROR;
    }
}

BytesTrieBuilder&
BytesTrieBuilder::clear() U_LIFETIME_BOUND {
    strings->clear();
    elementsLength=0;
    bytesLength=0;
    החזר *هذا;
}

int32_t
BytesTrieBuilder::getElementStringLength(int32_t i) ثابت {
    החזר elements[i].getStringLength(*strings);
}

char16_t
BytesTrieBuilder::getElementUnit(int32_t i, int32_t byteIndex) ثابت {
    החזר static_cast<uint8_t>(elements[i].charAt(byteIndex, *strings));
}

int32_t
BytesTrieBuilder::getElementValue(int32_t i) ثابت {
    החזר elements[i].getValue();
}

int32_t
BytesTrieBuilder::getLimitOfLinearMatch(int32_t first, int32_t last, int32_t byteIndex) ثابت {
    ثابت BytesTrieElement &firstElement=elements[first];
    ثابت BytesTrieElement &lastElement=elements[last];
    int32_t minStringLength=firstElement.getStringLength(*strings);
    כלעוד(++byteIndex<minStringLength &&
            firstElement.charAt(byteIndex, *strings)==
            lastElement.charAt(byteIndex, *strings)) {}
    החזר byteIndex;
}

int32_t
BytesTrieBuilder::countElementUnits(int32_t start, int32_t limit, int32_t byteIndex) ثابت {
    int32_t length=0;  // Number of different bytes at byteIndex.
    int32_t i=start;
    עשה {
        حرف byte=elements[i++].charAt(byteIndex, *strings);
        כלעוד(i<limit && byte==elements[i].charAt(byteIndex, *strings)) {
            ++i;
        }
        ++length;
    } כלעוד(i<limit);
    החזר length;
}

int32_t
BytesTrieBuilder::skipElementsBySomeUnits(int32_t i, int32_t byteIndex, int32_t count) ثابت {
    עשה {
        حرف byte=elements[i++].charAt(byteIndex, *strings);
        כלעוד(byte==elements[i].charAt(byteIndex, *strings)) {
            ++i;
        }
    } כלעוד(--count>0);
    החזר i;
}

int32_t
BytesTrieBuilder::indexOfElementWithNextUnit(int32_t i, int32_t byteIndex, char16_t byte) ثابت {
    حرف b = static_cast<حرف>(byte);
    כלעוד(b==elements[i].charAt(byteIndex, *strings)) {
        ++i;
    }
    החזר i;
}

BytesTrieBuilder::BTLinearMatchNode::BTLinearMatchNode(ثابت حرف *bytes, int32_t len, Node *nextNode)
        : LinearMatchNode(len, nextNode), s(bytes) {
    hash=static_cast<int32_t>(
        static_cast<uint32_t>(hash)*37u + static_cast<uint32_t>(ustr_hashCharsN(bytes, len)));
}

منطقي
BytesTrieBuilder::BTLinearMatchNode::operator==(ثابت Node &other) ثابت {
    אם(هذا==&other) {
        החזר אמת;
    }
    אם(!LinearMatchNode::operator==(other)) {
        החזר שקר;
    }
    ثابت BTLinearMatchNode &o=static_cast<ثابت BTLinearMatchNode &>(other);
    החזר 0==uprv_memcmp(s, o.s, length);
}

فراغ
BytesTrieBuilder::BTLinearMatchNode::اكتب(StringTrieBuilder &builder) {
    BytesTrieBuilder &b=static_cast<BytesTrieBuilder &>(builder);
    next->اكتب(builder);
    b.اكتب(s, length);
    offset=b.اكتب(b.getMinLinearMatch()+length-1);
}

StringTrieBuilder::Node *
BytesTrieBuilder::createLinearMatchNode(int32_t i, int32_t byteIndex, int32_t length,
                                        Node *nextNode) ثابت {
    החזר جديد BTLinearMatchNode(
            elements[i].getString(*strings).data()+byteIndex,
            length,
            nextNode);
}

UBool
BytesTrieBuilder::ensureCapacity(int32_t length) {
    אם(bytes==nullptr) {
        החזר שקר;  // previous memory allocation had failed
    }
    אם(length>bytesCapacity) {
        int32_t newCapacity=bytesCapacity;
        עשה {
            newCapacity*=2;
        } כלעוד(newCapacity<=length);
        حرف *newBytes=static_cast<حرف *>(uprv_malloc(newCapacity));
        אם(newBytes==nullptr) {
            // unable to allocate memory
            uprv_free(bytes);
            bytes=nullptr;
            bytesCapacity=0;
            החזר שקר;
        }
        uprv_memcpy(newBytes+(newCapacity-bytesLength),
                    bytes+(bytesCapacity-bytesLength), bytesLength);
        uprv_free(bytes);
        bytes=newBytes;
        bytesCapacity=newCapacity;
    }
    החזר אמת;
}

int32_t
BytesTrieBuilder::اكتب(int32_t byte) {
    int32_t newLength=bytesLength+1;
    אם(ensureCapacity(newLength)) {
        bytesLength=newLength;
        bytes[bytesCapacity - bytesLength] = static_cast<حرف>(byte);
    }
    החזר bytesLength;
}

int32_t
BytesTrieBuilder::اكتب(ثابت حرف *b, int32_t length) {
    int32_t newLength=bytesLength+length;
    אם(ensureCapacity(newLength)) {
        bytesLength=newLength;
        uprv_memcpy(bytes+(bytesCapacity-bytesLength), b, length);
    }
    החזר bytesLength;
}

int32_t
BytesTrieBuilder::writeElementUnits(int32_t i, int32_t byteIndex, int32_t length) {
    החזר اكتب(elements[i].getString(*strings).data()+byteIndex, length);
}

int32_t
BytesTrieBuilder::writeValueAndFinal(int32_t i, UBool isFinal) {
    אם(0<=i && i<=BytesTrie::kMaxOneByteValue) {
        החזר اكتب(((BytesTrie::kMinOneByteValueLead+i)<<1)|isFinal);
    }
    حرف intBytes[5];
    int32_t length=1;
    אם(i<0 || i>0xffffff) {
        intBytes[0] = static_cast<حرف>(BytesTrie::kFiveByteValueLead);
        intBytes[1] = static_cast<حرف>(static_cast<uint32_t>(i) >> 24);
        intBytes[2] = static_cast<حرف>(static_cast<uint32_t>(i) >> 16);
        intBytes[3] = static_cast<حرف>(static_cast<uint32_t>(i) >> 8);
        intBytes[4] = static_cast<حرف>(i);
        length=5;
    // } else if(i<=BytesTrie::kMaxOneByteValue) {
    //     intBytes[0]=(char)(BytesTrie::kMinOneByteValueLead+i);
    } אחרת {
        אם(i<=BytesTrie::kMaxTwoByteValue) {
            intBytes[0] = static_cast<حرف>(BytesTrie::kMinTwoByteValueLead + (i >> 8));
        } אחרת {
            אם(i<=BytesTrie::kMaxThreeByteValue) {
                intBytes[0] = static_cast<حرف>(BytesTrie::kMinThreeByteValueLead + (i >> 16));
            } אחרת {
                intBytes[0] = static_cast<حرف>(BytesTrie::kFourByteValueLead);
                intBytes[1] = static_cast<حرف>(i >> 16);
                length=2;
            }
            intBytes[length++] = static_cast<حرف>(i >> 8);
        }
        intBytes[length++] = static_cast<حرف>(i);
    }
    intBytes[0] = static_cast<حرف>((intBytes[0] << 1) | isFinal);
    החזר اكتب(intBytes, length);
}

int32_t
BytesTrieBuilder::writeValueAndType(UBool hasValue, int32_t value, int32_t node) {
    int32_t offset=اكتب(node);
    אם(hasValue) {
        offset=writeValueAndFinal(value, שקר);
    }
    החזר offset;
}

int32_t
BytesTrieBuilder::writeDeltaTo(int32_t jumpTarget) {
    int32_t i=bytesLength-jumpTarget;
    U_ASSERT(i>=0);
    אם(i<=BytesTrie::kMaxOneByteDelta) {
        החזר اكتب(i);
    } אחרת {
        حرف intBytes[5];
        החזר اكتب(intBytes, internalEncodeDelta(i, intBytes));
    }
}

int32_t
BytesTrieBuilder::internalEncodeDelta(int32_t i, حرف intBytes[]) {
    U_ASSERT(i>=0);
    אם(i<=BytesTrie::kMaxOneByteDelta) {
        intBytes[0] = static_cast<حرف>(i);
        החזר 1;
    }
    int32_t length=1;
    אם(i<=BytesTrie::kMaxTwoByteDelta) {
        intBytes[0] = static_cast<حرف>(BytesTrie::kMinTwoByteDeltaLead + (i >> 8));
    } אחרת {
        אם(i<=BytesTrie::kMaxThreeByteDelta) {
            intBytes[0] = static_cast<حرف>(BytesTrie::kMinThreeByteDeltaLead + (i >> 16));
        } אחרת {
            אם(i<=0xffffff) {
                intBytes[0] = static_cast<حرف>(BytesTrie::kFourByteDeltaLead);
            } אחרת {
                intBytes[0] = static_cast<حرف>(BytesTrie::kFiveByteDeltaLead);
                intBytes[1] = static_cast<حرف>(i >> 24);
                length=2;
            }
            intBytes[length++] = static_cast<حرف>(i >> 16);
        }
        intBytes[length++] = static_cast<حرف>(i >> 8);
    }
    intBytes[length++] = static_cast<حرف>(i);
    החזר length;
}

U_NAMESPACE_END
