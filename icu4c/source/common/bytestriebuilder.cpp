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

#அந்தர்பாவ "unicode/utypes.h"
#அந்தர்பாவ "unicode/bytestrie.h"
#அந்தர்பாவ "unicode/bytestriebuilder.h"
#அந்தர்பாவ "unicode/stringpiece.h"
#அந்தர்பாவ "charstr.h"
#அந்தர்பாவ "cmemory.h"
#அந்தர்பாவ "uhash.h"
#அந்தர்பாவ "uarrsort.h"
#அந்தர்பாவ "uassert.h"
#அந்தர்பாவ "ustr_imp.h"

U_NAMESPACE_BEGIN

/*
 * Note: This builder implementation stores (bytes, value) pairs with full copies
 * of the byte sequences, until the BytesTrie is built.
 * It might(!) take less memory if we collected the data in a temporary, dynamic trie.
 */

வகுப்பு BytesTrieElement : பொது UMemory {
பொது:
    // Use compiler's default constructor, initializes nothing.

    வெறுமை setTo(StringPiece s, int32_t val, CharString &strings, UErrorCode &errorCode);

    StringPiece getString(மாறிலி CharString& strings U_LIFETIME_BOUND) மாறிலி {
        int32_t offset=stringOffset;
        int32_t length;
        எனில்(offset>=0) {
            length = static_cast<uint8_t>(strings[offset++]);
        } இல்லையெனில் {
            offset=~offset;
            length = (static_cast<int32_t>(static_cast<uint8_t>(strings[offset])) << 8) | static_cast<uint8_t>(strings[offset + 1]);
            offset+=2;
        }
        திரும்பு StringPiece(strings.data()+offset, length);
    }
    int32_t getStringLength(மாறிலி CharString &strings) மாறிலி {
        int32_t offset=stringOffset;
        எனில்(offset>=0) {
            திரும்பு static_cast<uint8_t>(strings[offset]);
        } இல்லையெனில் {
            offset=~offset;
            திரும்பு (static_cast<int32_t>(static_cast<uint8_t>(strings[offset])) << 8) | static_cast<uint8_t>(strings[offset + 1]);
        }
    }

    எழுத்து charAt(int32_t index, மாறிலி CharString &strings) மாறிலி { திரும்பு data(strings)[index]; }

    int32_t get_மதிப்பு() மாறிலி { திரும்பு value; }

    int32_t compareStringTo(மாறிலி BytesTrieElement &o, மாறிலி CharString &strings) மாறிலி;

தனிப்பட்ட:
    மாறிலி எழுத்து* data(மாறிலி CharString& strings U_LIFETIME_BOUND) மாறிலி {
        int32_t offset=stringOffset;
        எனில்(offset>=0) {
            ++offset;
        } இல்லையெனில் {
            offset=~offset+2;
        }
        திரும்பு strings.data()+offset;
    }

    // If the stringOffset is non-negative, then the first strings byte contains
    // the string length.
    // If the stringOffset is negative, then the first two strings bytes contain
    // the string length (big-endian), and the offset needs to be bit-inverted.
    // (Compared with a stringLength field here, this saves 3 bytes per string for most strings.)
    int32_t stringOffset;
    int32_t value;
};

வெறுமை
BytesTrieElement::setTo(StringPiece s, int32_t val,
                        CharString &strings, UErrorCode &errorCode) {
    எனில்(U_FAILURE(errorCode)) {
        திரும்பு;
    }
    int32_t length=s.length();
    எனில்(length>0xffff) {
        // Too long: We store the length in 1 or 2 bytes.
        errorCode=U_INDEX_OUTOFBOUNDS_ERROR;
        திரும்பு;
    }
    int32_t offset=strings.length();
    எனில்(length>0xff) {
        offset=~offset;
        strings.append(static_cast<எழுத்து>(length >> 8), errorCode);
    }
    strings.append(static_cast<எழுத்து>(length), errorCode);
    stringOffset=offset;
    value=val;
    strings.append(s, errorCode);
}

int32_t
BytesTrieElement::compareStringTo(மாறிலி BytesTrieElement &other, மாறிலி CharString &strings) மாறிலி {
    // TODO: add StringPiece::compare(), see ticket #8187
    StringPiece thisString=getString(strings);
    StringPiece otherString=other.getString(strings);
    int32_t lengthDiff=thisString.length()-otherString.length();
    int32_t commonLength;
    எனில்(lengthDiff<=0) {
        commonLength=thisString.length();
    } இல்லையெனில் {
        commonLength=otherString.length();
    }
    int32_t diff=uprv_memcmp(thisString.data(), otherString.data(), commonLength);
    திரும்பு diff!=0 ? diff : lengthDiff;
}

BytesTrieBuilder::BytesTrieBuilder(UErrorCode &errorCode)
        : strings(nullptr), elements(nullptr), elementsCapacity(0), elementsLength(0),
          bytes(nullptr), bytesCapacity(0), bytesLength(0) {
    எனில்(U_FAILURE(errorCode)) {
        திரும்பு;
    }
    strings=புதிய CharString();
    எனில்(strings==nullptr) {
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
    எனில்(U_FAILURE(errorCode)) {
        திரும்பு *இது;
    }
    எனில்(bytesLength>0) {
        // Cannot add elements after building.
        errorCode=U_NO_WRITE_PERMISSION;
        திரும்பு *இது;
    }
    எனில்(elementsLength==elementsCapacity) {
        int32_t newCapacity;
        எனில்(elementsCapacity==0) {
            newCapacity=1024;
        } இல்லையெனில் {
            newCapacity=4*elementsCapacity;
        }
        BytesTrieElement *newElements=புதிய BytesTrieElement[newCapacity];
        எனில்(newElements==nullptr) {
            errorCode=U_MEMORY_ALLOCATION_ERROR;
            திரும்பு *இது; // error instead of dereferencing null
        }
        எனில்(elementsLength>0) {
            uprv_memcpy(newElements, elements, (size_t)elementsLength*அளவு(BytesTrieElement));
        }
        delete[] elements;
        elements=newElements;
        elementsCapacity=newCapacity;
    }
    elements[elementsLength++].setTo(s, value, *strings, errorCode);
    திரும்பு *இது;
}

U_CDECL_BEGIN

நிலையான int32_t U_CALLCONV
compareElementStrings(மாறிலி வெறுமை *context, மாறிலி வெறுமை *left, மாறிலி வெறுமை *வலது) {
    மாறிலி CharString *strings=static_cast<மாறிலி CharString *>(context);
    மாறிலி BytesTrieElement *இடது_Element=static_cast<மாறிலி BytesTrieElement *>(left);
    மாறிலி BytesTrieElement *வலது_Element=static_cast<மாறிலி BytesTrieElement *>(வலது);
    திரும்பு இடது_Element->compareStringTo(*வலது_Element, *strings);
}

U_CDECL_END

BytesTrie *
BytesTrieBuilder::build(UStringTrieBuildOption buildOption, UErrorCode &errorCode) {
    buildBytes(buildOption, errorCode);
    BytesTrie *newTrie=nullptr;
    எனில்(U_SUCCESS(errorCode)) {
        newTrie=புதிய BytesTrie(bytes, bytes+(bytesCapacity-bytesLength));
        எனில்(newTrie==nullptr) {
            errorCode=U_MEMORY_ALLOCATION_ERROR;
        } இல்லையெனில் {
            bytes=nullptr;  // The new trie now owns the array.
            bytesCapacity=0;
        }
    }
    திரும்பு newTrie;
}

StringPiece
BytesTrieBuilder::buildStringPiece(UStringTrieBuildOption buildOption,
                                   UErrorCode& errorCode) U_LIFETIME_BOUND {
    buildBytes(buildOption, errorCode);
    StringPiece result;
    எனில்(U_SUCCESS(errorCode)) {
        result.set(bytes+(bytesCapacity-bytesLength), bytesLength);
    }
    திரும்பு result;
}

வெறுமை
BytesTrieBuilder::buildBytes(UStringTrieBuildOption buildOption, UErrorCode &errorCode) {
    எனில்(U_FAILURE(errorCode)) {
        திரும்பு;
    }
    எனில்(bytes!=nullptr && bytesLength>0) {
        // Already built.
        திரும்பு;
    }
    எனில்(bytesLength==0) {
        எனில்(elementsLength==0) {
            errorCode=U_INDEX_OUTOFBOUNDS_ERROR;
            திரும்பு;
        }
        uprv_sortArray(elements, elementsLength, static_cast<int32_t>(அளவு(BytesTrieElement)),
                      compareElementStrings, strings,
                      பொய்,  // need not be a stable sort
                      &errorCode);
        எனில்(U_FAILURE(errorCode)) {
            திரும்பு;
        }
        // Duplicate strings are not allowed.
        StringPiece prev=elements[0].getString(*strings);
        க்கு(int32_t i=1; i<elementsLength; ++i) {
            StringPiece current=elements[i].getString(*strings);
            எனில்(prev==current) {
                errorCode=U_ILLEGAL_ARGUMENT_ERROR;
                திரும்பு;
            }
            prev=current;
        }
    }
    // Create and byte-serialize the trie for the elements.
    bytesLength=0;
    int32_t capacity=strings->length();
    எனில்(capacity<1024) {
        capacity=1024;
    }
    எனில்(bytesCapacity<capacity) {
        uprv_free(bytes);
        bytes=static_cast<எழுத்து *>(uprv_malloc(capacity));
        எனில்(bytes==nullptr) {
            errorCode=U_MEMORY_ALLOCATION_ERROR;
            bytesCapacity=0;
            திரும்பு;
        }
        bytesCapacity=capacity;
    }
    StringTrieBuilder::build(buildOption, elementsLength, errorCode);
    எனில்(bytes==nullptr) {
        errorCode=U_MEMORY_ALLOCATION_ERROR;
    }
}

BytesTrieBuilder&
BytesTrieBuilder::clear() U_LIFETIME_BOUND {
    strings->clear();
    elementsLength=0;
    bytesLength=0;
    திரும்பு *இது;
}

int32_t
BytesTrieBuilder::getElementStringLength(int32_t i) மாறிலி {
    திரும்பு elements[i].getStringLength(*strings);
}

char16_t
BytesTrieBuilder::getElementUnit(int32_t i, int32_t byteIndex) மாறிலி {
    திரும்பு static_cast<uint8_t>(elements[i].charAt(byteIndex, *strings));
}

int32_t
BytesTrieBuilder::get_Element_மதிப்பு(int32_t i) மாறிலி {
    திரும்பு elements[i].get_மதிப்பு();
}

int32_t
BytesTrieBuilder::getLimitOfLinearMatch(int32_t first, int32_t last, int32_t byteIndex) மாறிலி {
    மாறிலி BytesTrieElement &firstElement=elements[first];
    மாறிலி BytesTrieElement &lastElement=elements[last];
    int32_t minStringLength=firstElement.getStringLength(*strings);
    போது(++byteIndex<minStringLength &&
            firstElement.charAt(byteIndex, *strings)==
            lastElement.charAt(byteIndex, *strings)) {}
    திரும்பு byteIndex;
}

int32_t
BytesTrieBuilder::மொத்தம்_Element_Units(int32_t start, int32_t limit, int32_t byteIndex) மாறிலி {
    int32_t length=0;  // Number of different bytes at byteIndex.
    int32_t i=start;
    செய் {
        எழுத்து byte=elements[i++].charAt(byteIndex, *strings);
        போது(i<limit && byte==elements[i].charAt(byteIndex, *strings)) {
            ++i;
        }
        ++length;
    } போது(i<limit);
    திரும்பு length;
}

int32_t
BytesTrieBuilder::skipElementsBySomeUnits(int32_t i, int32_t byteIndex, int32_t count) மாறிலி {
    செய் {
        எழுத்து byte=elements[i++].charAt(byteIndex, *strings);
        போது(byte==elements[i].charAt(byteIndex, *strings)) {
            ++i;
        }
    } போது(--count>0);
    திரும்பு i;
}

int32_t
BytesTrieBuilder::indexOfElementWithNextUnit(int32_t i, int32_t byteIndex, char16_t byte) மாறிலி {
    எழுத்து b = static_cast<எழுத்து>(byte);
    போது(b==elements[i].charAt(byteIndex, *strings)) {
        ++i;
    }
    திரும்பு i;
}

BytesTrieBuilder::BTLinear_Match_முனை::BTLinear_Match_முனை(மாறிலி எழுத்து *bytes, int32_t len, Node *next_முனை)
        : Linear_Match_முனை(len, next_முனை), s(bytes) {
    hash=static_cast<int32_t>(
        static_cast<uint32_t>(hash)*37u + static_cast<uint32_t>(ustr_hashCharsN(bytes, len)));
}

தருக்க
BytesTrieBuilder::BTLinear_Match_முனை::operator==(மாறிலி Node &other) மாறிலி {
    எனில்(இது==&other) {
        திரும்பு உண்மை;
    }
    எனில்(!Linear_Match_முனை::operator==(other)) {
        திரும்பு பொய்;
    }
    மாறிலி BTLinear_Match_முனை &o=static_cast<மாறிலி BTLinear_Match_முனை &>(other);
    திரும்பு 0==uprv_memcmp(s, o.s, length);
}

வெறுமை
BytesTrieBuilder::BTLinear_Match_முனை::லிக(StringTrieBuilder &builder) {
    BytesTrieBuilder &b=static_cast<BytesTrieBuilder &>(builder);
    next->லிக(builder);
    b.லிக(s, length);
    offset=b.லிக(b.getMinLinearMatch()+length-1);
}

StringTrieBuilder::Node *
BytesTrieBuilder::create_Linear_Match_முனை(int32_t i, int32_t byteIndex, int32_t length,
                                        Node *next_முனை) மாறிலி {
    திரும்பு புதிய BTLinear_Match_முனை(
            elements[i].getString(*strings).data()+byteIndex,
            length,
            next_முனை);
}

UBool
BytesTrieBuilder::ensureCapacity(int32_t length) {
    எனில்(bytes==nullptr) {
        திரும்பு பொய்;  // previous memory allocation had failed
    }
    எனில்(length>bytesCapacity) {
        int32_t newCapacity=bytesCapacity;
        செய் {
            newCapacity*=2;
        } போது(newCapacity<=length);
        எழுத்து *newBytes=static_cast<எழுத்து *>(uprv_malloc(newCapacity));
        எனில்(newBytes==nullptr) {
            // unable to allocate memory
            uprv_free(bytes);
            bytes=nullptr;
            bytesCapacity=0;
            திரும்பு பொய்;
        }
        uprv_memcpy(newBytes+(newCapacity-bytesLength),
                    bytes+(bytesCapacity-bytesLength), bytesLength);
        uprv_free(bytes);
        bytes=newBytes;
        bytesCapacity=newCapacity;
    }
    திரும்பு உண்மை;
}

int32_t
BytesTrieBuilder::லிக(int32_t byte) {
    int32_t newLength=bytesLength+1;
    எனில்(ensureCapacity(newLength)) {
        bytesLength=newLength;
        bytes[bytesCapacity - bytesLength] = static_cast<எழுத்து>(byte);
    }
    திரும்பு bytesLength;
}

int32_t
BytesTrieBuilder::லிக(மாறிலி எழுத்து *b, int32_t length) {
    int32_t newLength=bytesLength+length;
    எனில்(ensureCapacity(newLength)) {
        bytesLength=newLength;
        uprv_memcpy(bytes+(bytesCapacity-bytesLength), b, length);
    }
    திரும்பு bytesLength;
}

int32_t
BytesTrieBuilder::writeElementUnits(int32_t i, int32_t byteIndex, int32_t length) {
    திரும்பு லிக(elements[i].getString(*strings).data()+byteIndex, length);
}

int32_t
BytesTrieBuilder::write_மதிப்பு_And_Final(int32_t i, UBool isFinal) {
    எனில்(0<=i && i<=BytesTrie::k_Max_One_Byte_மதிப்பு) {
        திரும்பு லிக(((BytesTrie::k_Min_One_Byte_மதிப்பு_Lead+i)<<1)|isFinal);
    }
    எழுத்து intBytes[5];
    int32_t length=1;
    எனில்(i<0 || i>0xffffff) {
        intBytes[0] = static_cast<எழுத்து>(BytesTrie::k_Five_Byte_மதிப்பு_Lead);
        intBytes[1] = static_cast<எழுத்து>(static_cast<uint32_t>(i) >> 24);
        intBytes[2] = static_cast<எழுத்து>(static_cast<uint32_t>(i) >> 16);
        intBytes[3] = static_cast<எழுத்து>(static_cast<uint32_t>(i) >> 8);
        intBytes[4] = static_cast<எழுத்து>(i);
        length=5;
    // } else if(i<=BytesTrie::kMaxOneByteValue) {
    //     intBytes[0]=(char)(BytesTrie::kMinOneByteValueLead+i);
    } இல்லையெனில் {
        எனில்(i<=BytesTrie::k_Max_Two_Byte_மதிப்பு) {
            intBytes[0] = static_cast<எழுத்து>(BytesTrie::k_Min_Two_Byte_மதிப்பு_Lead + (i >> 8));
        } இல்லையெனில் {
            எனில்(i<=BytesTrie::k_Max_Three_Byte_மதிப்பு) {
                intBytes[0] = static_cast<எழுத்து>(BytesTrie::k_Min_Three_Byte_மதிப்பு_Lead + (i >> 16));
            } இல்லையெனில் {
                intBytes[0] = static_cast<எழுத்து>(BytesTrie::k_Four_Byte_மதிப்பு_Lead);
                intBytes[1] = static_cast<எழுத்து>(i >> 16);
                length=2;
            }
            intBytes[length++] = static_cast<எழுத்து>(i >> 8);
        }
        intBytes[length++] = static_cast<எழுத்து>(i);
    }
    intBytes[0] = static_cast<எழுத்து>((intBytes[0] << 1) | isFinal);
    திரும்பு லிக(intBytes, length);
}

int32_t
BytesTrieBuilder::write_மதிப்பு_And_Type(UBool has_மதிப்பு, int32_t value, int32_t முனை) {
    int32_t offset=லிக(முனை);
    எனில்(has_மதிப்பு) {
        offset=write_மதிப்பு_And_Final(value, பொய்);
    }
    திரும்பு offset;
}

int32_t
BytesTrieBuilder::writeDeltaTo(int32_t jumpTarget) {
    int32_t i=bytesLength-jumpTarget;
    U_ASSERT(i>=0);
    எனில்(i<=BytesTrie::kMaxOneByteDelta) {
        திரும்பு லிக(i);
    } இல்லையெனில் {
        எழுத்து intBytes[5];
        திரும்பு லிக(intBytes, internalEncodeDelta(i, intBytes));
    }
}

int32_t
BytesTrieBuilder::internalEncodeDelta(int32_t i, எழுத்து intBytes[]) {
    U_ASSERT(i>=0);
    எனில்(i<=BytesTrie::kMaxOneByteDelta) {
        intBytes[0] = static_cast<எழுத்து>(i);
        திரும்பு 1;
    }
    int32_t length=1;
    எனில்(i<=BytesTrie::kMaxTwoByteDelta) {
        intBytes[0] = static_cast<எழுத்து>(BytesTrie::kMinTwoByteDeltaLead + (i >> 8));
    } இல்லையெனில் {
        எனில்(i<=BytesTrie::kMaxThreeByteDelta) {
            intBytes[0] = static_cast<எழுத்து>(BytesTrie::kMinThreeByteDeltaLead + (i >> 16));
        } இல்லையெனில் {
            எனில்(i<=0xffffff) {
                intBytes[0] = static_cast<எழுத்து>(BytesTrie::kFourByteDeltaLead);
            } இல்லையெனில் {
                intBytes[0] = static_cast<எழுத்து>(BytesTrie::kFiveByteDeltaLead);
                intBytes[1] = static_cast<எழுத்து>(i >> 24);
                length=2;
            }
            intBytes[length++] = static_cast<எழுத்து>(i >> 16);
        }
        intBytes[length++] = static_cast<எழுத்து>(i >> 8);
    }
    intBytes[length++] = static_cast<எழுத்து>(i);
    திரும்பு length;
}

U_NAMESPACE_END
