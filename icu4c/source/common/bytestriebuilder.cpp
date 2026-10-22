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

#অন্তর্ভাৱ "unicode/utypes.h"
#অন্তর্ভাৱ "unicode/bytestrie.h"
#অন্তর্ভাৱ "unicode/bytestriebuilder.h"
#অন্তর্ভাৱ "unicode/stringpiece.h"
#অন্তর্ভাৱ "charstr.h"
#অন্তর্ভাৱ "cmemory.h"
#অন্তর্ভাৱ "uhash.h"
#অন্তর্ভাৱ "uarrsort.h"
#অন্তর্ভাৱ "uassert.h"
#অন্তর্ভাৱ "ustr_imp.h"

U_NAMESPACE_BEGIN

/*
 * Note: This builder implementation stores (bytes, value) pairs with full copies
 * of the byte sequences, until the BytesTrie is built.
 * It might(!) take less memory if we collected the data in a temporary, dynamic trie.
 */

শ্ৰেণী BytesTrieElement : সার্ৱজনিক UMemory {
সার্ৱজনিক:
    // Use compiler's default constructor, initializes nothing.

    শূন্য setTo(StringPiece s, int32_t val, CharString &strings, UErrorCode &errorCode);

    StringPiece getString(ধ্রুৱ CharString& strings U_LIFETIME_BOUND) ধ্রুৱ {
        int32_t offset=stringOffset;
        int32_t length;
        যদি(offset>=0) {
            length = static_cast<uint8_t>(strings[offset++]);
        } অন্যথা {
            offset=~offset;
            length = (static_cast<int32_t>(static_cast<uint8_t>(strings[offset])) << 8) | static_cast<uint8_t>(strings[offset + 1]);
            offset+=2;
        }
        ঘূৰাই StringPiece(strings.data()+offset, length);
    }
    int32_t getStringLength(ধ্রুৱ CharString &strings) ধ্রুৱ {
        int32_t offset=stringOffset;
        যদি(offset>=0) {
            ঘূৰাই static_cast<uint8_t>(strings[offset]);
        } অন্যথা {
            offset=~offset;
            ঘূৰাই (static_cast<int32_t>(static_cast<uint8_t>(strings[offset])) << 8) | static_cast<uint8_t>(strings[offset + 1]);
        }
    }

    ৱর্ণ charAt(int32_t index, ধ্রুৱ CharString &strings) ধ্রুৱ { ঘূৰাই data(strings)[index]; }

    int32_t get_মান() ধ্রুৱ { ঘূৰাই value; }

    int32_t compareStringTo(ধ্রুৱ BytesTrieElement &o, ধ্রুৱ CharString &strings) ধ্রুৱ;

গুপ্ত:
    ধ্রুৱ ৱর্ণ* data(ধ্রুৱ CharString& strings U_LIFETIME_BOUND) ধ্রুৱ {
        int32_t offset=stringOffset;
        যদি(offset>=0) {
            ++offset;
        } অন্যথা {
            offset=~offset+2;
        }
        ঘূৰাই strings.data()+offset;
    }

    // If the stringOffset is non-negative, then the first strings byte contains
    // the string length.
    // If the stringOffset is negative, then the first two strings bytes contain
    // the string length (big-endian), and the offset needs to be bit-inverted.
    // (Compared with a stringLength field here, this saves 3 bytes per string for most strings.)
    int32_t stringOffset;
    int32_t value;
};

শূন্য
BytesTrieElement::setTo(StringPiece s, int32_t val,
                        CharString &strings, UErrorCode &errorCode) {
    যদি(U_FAILURE(errorCode)) {
        ঘূৰাই;
    }
    int32_t length=s.length();
    যদি(length>0xffff) {
        // Too long: We store the length in 1 or 2 bytes.
        errorCode=U_INDEX_OUTOFBOUNDS_ERROR;
        ঘূৰাই;
    }
    int32_t offset=strings.length();
    যদি(length>0xff) {
        offset=~offset;
        strings.append(static_cast<ৱর্ণ>(length >> 8), errorCode);
    }
    strings.append(static_cast<ৱর্ণ>(length), errorCode);
    stringOffset=offset;
    value=val;
    strings.append(s, errorCode);
}

int32_t
BytesTrieElement::compareStringTo(ধ্রুৱ BytesTrieElement &other, ধ্রুৱ CharString &strings) ধ্রুৱ {
    // TODO: add StringPiece::compare(), see ticket #8187
    StringPiece thisString=getString(strings);
    StringPiece otherString=other.getString(strings);
    int32_t lengthDiff=thisString.length()-otherString.length();
    int32_t commonLength;
    যদি(lengthDiff<=0) {
        commonLength=thisString.length();
    } অন্যথা {
        commonLength=otherString.length();
    }
    int32_t diff=uprv_memcmp(thisString.data(), otherString.data(), commonLength);
    ঘূৰাই diff!=0 ? diff : lengthDiff;
}

BytesTrieBuilder::BytesTrieBuilder(UErrorCode &errorCode)
        : strings(nullptr), elements(nullptr), elementsCapacity(0), elementsLength(0),
          bytes(nullptr), bytesCapacity(0), bytesLength(0) {
    যদি(U_FAILURE(errorCode)) {
        ঘূৰাই;
    }
    strings=নূতন CharString();
    যদি(strings==nullptr) {
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
    যদি(U_FAILURE(errorCode)) {
        ঘূৰাই *এতত্;
    }
    যদি(bytesLength>0) {
        // Cannot add elements after building.
        errorCode=U_NO_WRITE_PERMISSION;
        ঘূৰাই *এতত্;
    }
    যদি(elementsLength==elementsCapacity) {
        int32_t newCapacity;
        যদি(elementsCapacity==0) {
            newCapacity=1024;
        } অন্যথা {
            newCapacity=4*elementsCapacity;
        }
        BytesTrieElement *newElements=নূতন BytesTrieElement[newCapacity];
        যদি(newElements==nullptr) {
            errorCode=U_MEMORY_ALLOCATION_ERROR;
            ঘূৰাই *এতত্; // error instead of dereferencing null
        }
        যদি(elementsLength>0) {
            uprv_memcpy(newElements, elements, (size_t)elementsLength*পরিমাণ(BytesTrieElement));
        }
        delete[] elements;
        elements=newElements;
        elementsCapacity=newCapacity;
    }
    elements[elementsLength++].setTo(s, value, *strings, errorCode);
    ঘূৰাই *এতত্;
}

U_CDECL_BEGIN

স্থির int32_t U_CALLCONV
compareElementStrings(ধ্রুৱ শূন্য *context, ধ্রুৱ শূন্য *left, ধ্রুৱ শূন্য *ডান) {
    ধ্রুৱ CharString *strings=static_cast<ধ্রুৱ CharString *>(context);
    ধ্রুৱ BytesTrieElement *বাম_Element=static_cast<ধ্রুৱ BytesTrieElement *>(left);
    ধ্রুৱ BytesTrieElement *ডান_Element=static_cast<ধ্রুৱ BytesTrieElement *>(ডান);
    ঘূৰাই বাম_Element->compareStringTo(*ডান_Element, *strings);
}

U_CDECL_END

BytesTrie *
BytesTrieBuilder::build(UStringTrieBuildOption buildOption, UErrorCode &errorCode) {
    buildBytes(buildOption, errorCode);
    BytesTrie *newTrie=nullptr;
    যদি(U_SUCCESS(errorCode)) {
        newTrie=নূতন BytesTrie(bytes, bytes+(bytesCapacity-bytesLength));
        যদি(newTrie==nullptr) {
            errorCode=U_MEMORY_ALLOCATION_ERROR;
        } অন্যথা {
            bytes=nullptr;  // The new trie now owns the array.
            bytesCapacity=0;
        }
    }
    ঘূৰাই newTrie;
}

StringPiece
BytesTrieBuilder::buildStringPiece(UStringTrieBuildOption buildOption,
                                   UErrorCode& errorCode) U_LIFETIME_BOUND {
    buildBytes(buildOption, errorCode);
    StringPiece result;
    যদি(U_SUCCESS(errorCode)) {
        result.set(bytes+(bytesCapacity-bytesLength), bytesLength);
    }
    ঘূৰাই result;
}

শূন্য
BytesTrieBuilder::buildBytes(UStringTrieBuildOption buildOption, UErrorCode &errorCode) {
    যদি(U_FAILURE(errorCode)) {
        ঘূৰাই;
    }
    যদি(bytes!=nullptr && bytesLength>0) {
        // Already built.
        ঘূৰাই;
    }
    যদি(bytesLength==0) {
        যদি(elementsLength==0) {
            errorCode=U_INDEX_OUTOFBOUNDS_ERROR;
            ঘূৰাই;
        }
        uprv_sortArray(elements, elementsLength, static_cast<int32_t>(পরিমাণ(BytesTrieElement)),
                      compareElementStrings, strings,
                      মিছা,  // need not be a stable sort
                      &errorCode);
        যদি(U_FAILURE(errorCode)) {
            ঘূৰাই;
        }
        // Duplicate strings are not allowed.
        StringPiece prev=elements[0].getString(*strings);
        বাবে(int32_t i=1; i<elementsLength; ++i) {
            StringPiece current=elements[i].getString(*strings);
            যদি(prev==current) {
                errorCode=U_ILLEGAL_ARGUMENT_ERROR;
                ঘূৰাই;
            }
            prev=current;
        }
    }
    // Create and byte-serialize the trie for the elements.
    bytesLength=0;
    int32_t capacity=strings->length();
    যদি(capacity<1024) {
        capacity=1024;
    }
    যদি(bytesCapacity<capacity) {
        uprv_free(bytes);
        bytes=static_cast<ৱর্ণ *>(uprv_malloc(capacity));
        যদি(bytes==nullptr) {
            errorCode=U_MEMORY_ALLOCATION_ERROR;
            bytesCapacity=0;
            ঘূৰাই;
        }
        bytesCapacity=capacity;
    }
    StringTrieBuilder::build(buildOption, elementsLength, errorCode);
    যদি(bytes==nullptr) {
        errorCode=U_MEMORY_ALLOCATION_ERROR;
    }
}

BytesTrieBuilder&
BytesTrieBuilder::clear() U_LIFETIME_BOUND {
    strings->clear();
    elementsLength=0;
    bytesLength=0;
    ঘূৰাই *এতত্;
}

int32_t
BytesTrieBuilder::getElementStringLength(int32_t i) ধ্রুৱ {
    ঘূৰাই elements[i].getStringLength(*strings);
}

char16_t
BytesTrieBuilder::getElementUnit(int32_t i, int32_t byteIndex) ধ্রুৱ {
    ঘূৰাই static_cast<uint8_t>(elements[i].charAt(byteIndex, *strings));
}

int32_t
BytesTrieBuilder::get_Element_মান(int32_t i) ধ্রুৱ {
    ঘূৰাই elements[i].get_মান();
}

int32_t
BytesTrieBuilder::getLimitOfLinearMatch(int32_t first, int32_t last, int32_t byteIndex) ধ্রুৱ {
    ধ্রুৱ BytesTrieElement &firstElement=elements[first];
    ধ্রুৱ BytesTrieElement &lastElement=elements[last];
    int32_t minStringLength=firstElement.getStringLength(*strings);
    যেতিয়ালৈ(++byteIndex<minStringLength &&
            firstElement.charAt(byteIndex, *strings)==
            lastElement.charAt(byteIndex, *strings)) {}
    ঘূৰাই byteIndex;
}

int32_t
BytesTrieBuilder::গুনতি_Element_Units(int32_t start, int32_t limit, int32_t byteIndex) ধ্রুৱ {
    int32_t length=0;  // Number of different bytes at byteIndex.
    int32_t i=start;
    কৰা {
        ৱর্ণ byte=elements[i++].charAt(byteIndex, *strings);
        যেতিয়ালৈ(i<limit && byte==elements[i].charAt(byteIndex, *strings)) {
            ++i;
        }
        ++length;
    } যেতিয়ালৈ(i<limit);
    ঘূৰাই length;
}

int32_t
BytesTrieBuilder::skipElementsBySomeUnits(int32_t i, int32_t byteIndex, int32_t count) ধ্রুৱ {
    কৰা {
        ৱর্ণ byte=elements[i++].charAt(byteIndex, *strings);
        যেতিয়ালৈ(byte==elements[i].charAt(byteIndex, *strings)) {
            ++i;
        }
    } যেতিয়ালৈ(--count>0);
    ঘূৰাই i;
}

int32_t
BytesTrieBuilder::indexOfElementWithNextUnit(int32_t i, int32_t byteIndex, char16_t byte) ধ্রুৱ {
    ৱর্ণ b = static_cast<ৱর্ণ>(byte);
    যেতিয়ালৈ(b==elements[i].charAt(byteIndex, *strings)) {
        ++i;
    }
    ঘূৰাই i;
}

BytesTrieBuilder::BTLinear_Match_গ্ৰন্থি::BTLinear_Match_গ্ৰন্থি(ধ্রুৱ ৱর্ণ *bytes, int32_t len, Node *next_গ্ৰন্থি)
        : Linear_Match_গ্ৰন্থি(len, next_গ্ৰন্থি), s(bytes) {
    hash=static_cast<int32_t>(
        static_cast<uint32_t>(hash)*37u + static_cast<uint32_t>(ustr_hashCharsN(bytes, len)));
}

তার্কিক
BytesTrieBuilder::BTLinear_Match_গ্ৰন্থি::operator==(ধ্রুৱ Node &other) ধ্রুৱ {
    যদি(এতত্==&other) {
        ঘূৰাই সঁচা;
    }
    যদি(!Linear_Match_গ্ৰন্থি::operator==(other)) {
        ঘূৰাই মিছা;
    }
    ধ্রুৱ BTLinear_Match_গ্ৰন্থি &o=static_cast<ধ্রুৱ BTLinear_Match_গ্ৰন্থি &>(other);
    ঘূৰাই 0==uprv_memcmp(s, o.s, length);
}

শূন্য
BytesTrieBuilder::BTLinear_Match_গ্ৰন্থি::লিখ(StringTrieBuilder &builder) {
    BytesTrieBuilder &b=static_cast<BytesTrieBuilder &>(builder);
    next->লিখ(builder);
    b.লিখ(s, length);
    offset=b.লিখ(b.getMinLinearMatch()+length-1);
}

StringTrieBuilder::Node *
BytesTrieBuilder::create_Linear_Match_গ্ৰন্থি(int32_t i, int32_t byteIndex, int32_t length,
                                        Node *next_গ্ৰন্থি) ধ্রুৱ {
    ঘূৰাই নূতন BTLinear_Match_গ্ৰন্থি(
            elements[i].getString(*strings).data()+byteIndex,
            length,
            next_গ্ৰন্থি);
}

UBool
BytesTrieBuilder::ensureCapacity(int32_t length) {
    যদি(bytes==nullptr) {
        ঘূৰাই মিছা;  // previous memory allocation had failed
    }
    যদি(length>bytesCapacity) {
        int32_t newCapacity=bytesCapacity;
        কৰা {
            newCapacity*=2;
        } যেতিয়ালৈ(newCapacity<=length);
        ৱর্ণ *newBytes=static_cast<ৱর্ণ *>(uprv_malloc(newCapacity));
        যদি(newBytes==nullptr) {
            // unable to allocate memory
            uprv_free(bytes);
            bytes=nullptr;
            bytesCapacity=0;
            ঘূৰাই মিছা;
        }
        uprv_memcpy(newBytes+(newCapacity-bytesLength),
                    bytes+(bytesCapacity-bytesLength), bytesLength);
        uprv_free(bytes);
        bytes=newBytes;
        bytesCapacity=newCapacity;
    }
    ঘূৰাই সঁচা;
}

int32_t
BytesTrieBuilder::লিখ(int32_t byte) {
    int32_t newLength=bytesLength+1;
    যদি(ensureCapacity(newLength)) {
        bytesLength=newLength;
        bytes[bytesCapacity - bytesLength] = static_cast<ৱর্ণ>(byte);
    }
    ঘূৰাই bytesLength;
}

int32_t
BytesTrieBuilder::লিখ(ধ্রুৱ ৱর্ণ *b, int32_t length) {
    int32_t newLength=bytesLength+length;
    যদি(ensureCapacity(newLength)) {
        bytesLength=newLength;
        uprv_memcpy(bytes+(bytesCapacity-bytesLength), b, length);
    }
    ঘূৰাই bytesLength;
}

int32_t
BytesTrieBuilder::writeElementUnits(int32_t i, int32_t byteIndex, int32_t length) {
    ঘূৰাই লিখ(elements[i].getString(*strings).data()+byteIndex, length);
}

int32_t
BytesTrieBuilder::write_মান_And_Final(int32_t i, UBool isFinal) {
    যদি(0<=i && i<=BytesTrie::k_Max_One_Byte_মান) {
        ঘূৰাই লিখ(((BytesTrie::k_Min_One_Byte_মান_Lead+i)<<1)|isFinal);
    }
    ৱর্ণ intBytes[5];
    int32_t length=1;
    যদি(i<0 || i>0xffffff) {
        intBytes[0] = static_cast<ৱর্ণ>(BytesTrie::k_Five_Byte_মান_Lead);
        intBytes[1] = static_cast<ৱর্ণ>(static_cast<uint32_t>(i) >> 24);
        intBytes[2] = static_cast<ৱর্ণ>(static_cast<uint32_t>(i) >> 16);
        intBytes[3] = static_cast<ৱর্ণ>(static_cast<uint32_t>(i) >> 8);
        intBytes[4] = static_cast<ৱর্ণ>(i);
        length=5;
    // } else if(i<=BytesTrie::kMaxOneByteValue) {
    //     intBytes[0]=(char)(BytesTrie::kMinOneByteValueLead+i);
    } অন্যথা {
        যদি(i<=BytesTrie::k_Max_Two_Byte_মান) {
            intBytes[0] = static_cast<ৱর্ণ>(BytesTrie::k_Min_Two_Byte_মান_Lead + (i >> 8));
        } অন্যথা {
            যদি(i<=BytesTrie::k_Max_Three_Byte_মান) {
                intBytes[0] = static_cast<ৱর্ণ>(BytesTrie::k_Min_Three_Byte_মান_Lead + (i >> 16));
            } অন্যথা {
                intBytes[0] = static_cast<ৱর্ণ>(BytesTrie::k_Four_Byte_মান_Lead);
                intBytes[1] = static_cast<ৱর্ণ>(i >> 16);
                length=2;
            }
            intBytes[length++] = static_cast<ৱর্ণ>(i >> 8);
        }
        intBytes[length++] = static_cast<ৱর্ণ>(i);
    }
    intBytes[0] = static_cast<ৱর্ণ>((intBytes[0] << 1) | isFinal);
    ঘূৰাই লিখ(intBytes, length);
}

int32_t
BytesTrieBuilder::write_মান_And_Type(UBool has_মান, int32_t value, int32_t গ্ৰন্থি) {
    int32_t offset=লিখ(গ্ৰন্থি);
    যদি(has_মান) {
        offset=write_মান_And_Final(value, মিছা);
    }
    ঘূৰাই offset;
}

int32_t
BytesTrieBuilder::writeDeltaTo(int32_t jumpTarget) {
    int32_t i=bytesLength-jumpTarget;
    U_ASSERT(i>=0);
    যদি(i<=BytesTrie::kMaxOneByteDelta) {
        ঘূৰাই লিখ(i);
    } অন্যথা {
        ৱর্ণ intBytes[5];
        ঘূৰাই লিখ(intBytes, internalEncodeDelta(i, intBytes));
    }
}

int32_t
BytesTrieBuilder::internalEncodeDelta(int32_t i, ৱর্ণ intBytes[]) {
    U_ASSERT(i>=0);
    যদি(i<=BytesTrie::kMaxOneByteDelta) {
        intBytes[0] = static_cast<ৱর্ণ>(i);
        ঘূৰাই 1;
    }
    int32_t length=1;
    যদি(i<=BytesTrie::kMaxTwoByteDelta) {
        intBytes[0] = static_cast<ৱর্ণ>(BytesTrie::kMinTwoByteDeltaLead + (i >> 8));
    } অন্যথা {
        যদি(i<=BytesTrie::kMaxThreeByteDelta) {
            intBytes[0] = static_cast<ৱর্ণ>(BytesTrie::kMinThreeByteDeltaLead + (i >> 16));
        } অন্যথা {
            যদি(i<=0xffffff) {
                intBytes[0] = static_cast<ৱর্ণ>(BytesTrie::kFourByteDeltaLead);
            } অন্যথা {
                intBytes[0] = static_cast<ৱর্ণ>(BytesTrie::kFiveByteDeltaLead);
                intBytes[1] = static_cast<ৱর্ণ>(i >> 24);
                length=2;
            }
            intBytes[length++] = static_cast<ৱর্ণ>(i >> 16);
        }
        intBytes[length++] = static_cast<ৱর্ণ>(i >> 8);
    }
    intBytes[length++] = static_cast<ৱর্ণ>(i);
    ঘূৰাই length;
}

U_NAMESPACE_END
