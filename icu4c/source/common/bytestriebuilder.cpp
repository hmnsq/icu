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

#ଅନ୍ତର୍ଭାଵ "unicode/utypes.h"
#ଅନ୍ତର୍ଭାଵ "unicode/bytestrie.h"
#ଅନ୍ତର୍ଭାଵ "unicode/bytestriebuilder.h"
#ଅନ୍ତର୍ଭାଵ "unicode/stringpiece.h"
#ଅନ୍ତର୍ଭାଵ "charstr.h"
#ଅନ୍ତର୍ଭାଵ "cmemory.h"
#ଅନ୍ତର୍ଭାଵ "uhash.h"
#ଅନ୍ତର୍ଭାଵ "uarrsort.h"
#ଅନ୍ତର୍ଭାଵ "uassert.h"
#ଅନ୍ତର୍ଭାଵ "ustr_imp.h"

U_NAMESPACE_BEGIN

/*
 * Note: This builder implementation stores (bytes, value) pairs with full copies
 * of the byte sequences, until the BytesTrie is built.
 * It might(!) take less memory if we collected the data in a temporary, dynamic trie.
 */

ଶ୍ରେଣୀ BytesTrieElement : ସର୍ବସାଧାରଣ UMemory {
ସର୍ବସାଧାରଣ:
    // Use compiler's default constructor, initializes nothing.

    ଶୂନ୍ୟ setTo(StringPiece s, int32_t val, CharString &strings, UErrorCode &errorCode);

    StringPiece getString(ଅଚଳ CharString& strings U_LIFETIME_BOUND) ଅଚଳ {
        int32_t offset=stringOffset;
        int32_t length;
        ଯଦି(offset>=0) {
            length = static_cast<uint8_t>(strings[offset++]);
        } ଅନ୍ୟଥା {
            offset=~offset;
            length = (static_cast<int32_t>(static_cast<uint8_t>(strings[offset])) << 8) | static_cast<uint8_t>(strings[offset + 1]);
            offset+=2;
        }
        ଫେରନ୍ତୁ StringPiece(strings.data()+offset, length);
    }
    int32_t getStringLength(ଅଚଳ CharString &strings) ଅଚଳ {
        int32_t offset=stringOffset;
        ଯଦି(offset>=0) {
            ଫେରନ୍ତୁ static_cast<uint8_t>(strings[offset]);
        } ଅନ୍ୟଥା {
            offset=~offset;
            ଫେରନ୍ତୁ (static_cast<int32_t>(static_cast<uint8_t>(strings[offset])) << 8) | static_cast<uint8_t>(strings[offset + 1]);
        }
    }

    ଅକ୍ଷର charAt(int32_t index, ଅଚଳ CharString &strings) ଅଚଳ { ଫେରନ୍ତୁ data(strings)[index]; }

    int32_t get_ମୂଲ୍ୟ() ଅଚଳ { ଫେରନ୍ତୁ value; }

    int32_t compareStringTo(ଅଚଳ BytesTrieElement &o, ଅଚଳ CharString &strings) ଅଚଳ;

ବ୍ୟକ୍ତିଗତ:
    ଅଚଳ ଅକ୍ଷର* data(ଅଚଳ CharString& strings U_LIFETIME_BOUND) ଅଚଳ {
        int32_t offset=stringOffset;
        ଯଦି(offset>=0) {
            ++offset;
        } ଅନ୍ୟଥା {
            offset=~offset+2;
        }
        ଫେରନ୍ତୁ strings.data()+offset;
    }

    // If the stringOffset is non-negative, then the first strings byte contains
    // the string length.
    // If the stringOffset is negative, then the first two strings bytes contain
    // the string length (big-endian), and the offset needs to be bit-inverted.
    // (Compared with a stringLength field here, this saves 3 bytes per string for most strings.)
    int32_t stringOffset;
    int32_t value;
};

ଶୂନ୍ୟ
BytesTrieElement::setTo(StringPiece s, int32_t val,
                        CharString &strings, UErrorCode &errorCode) {
    ଯଦି(U_FAILURE(errorCode)) {
        ଫେରନ୍ତୁ;
    }
    int32_t length=s.length();
    ଯଦି(length>0xffff) {
        // Too long: We store the length in 1 or 2 bytes.
        errorCode=U_INDEX_OUTOFBOUNDS_ERROR;
        ଫେରନ୍ତୁ;
    }
    int32_t offset=strings.length();
    ଯଦି(length>0xff) {
        offset=~offset;
        strings.append(static_cast<ଅକ୍ଷର>(length >> 8), errorCode);
    }
    strings.append(static_cast<ଅକ୍ଷର>(length), errorCode);
    stringOffset=offset;
    value=val;
    strings.append(s, errorCode);
}

int32_t
BytesTrieElement::compareStringTo(ଅଚଳ BytesTrieElement &other, ଅଚଳ CharString &strings) ଅଚଳ {
    // TODO: add StringPiece::compare(), see ticket #8187
    StringPiece thisString=getString(strings);
    StringPiece otherString=other.getString(strings);
    int32_t lengthDiff=thisString.length()-otherString.length();
    int32_t commonLength;
    ଯଦି(lengthDiff<=0) {
        commonLength=thisString.length();
    } ଅନ୍ୟଥା {
        commonLength=otherString.length();
    }
    int32_t diff=uprv_memcmp(thisString.data(), otherString.data(), commonLength);
    ଫେରନ୍ତୁ diff!=0 ? diff : lengthDiff;
}

BytesTrieBuilder::BytesTrieBuilder(UErrorCode &errorCode)
        : strings(nullptr), elements(nullptr), elementsCapacity(0), elementsLength(0),
          bytes(nullptr), bytesCapacity(0), bytesLength(0) {
    ଯଦି(U_FAILURE(errorCode)) {
        ଫେରନ୍ତୁ;
    }
    strings=ନୂତନ CharString();
    ଯଦି(strings==nullptr) {
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
    ଯଦି(U_FAILURE(errorCode)) {
        ଫେରନ୍ତୁ *ଏହି;
    }
    ଯଦି(bytesLength>0) {
        // Cannot add elements after building.
        errorCode=U_NO_WRITE_PERMISSION;
        ଫେରନ୍ତୁ *ଏହି;
    }
    ଯଦି(elementsLength==elementsCapacity) {
        int32_t newCapacity;
        ଯଦି(elementsCapacity==0) {
            newCapacity=1024;
        } ଅନ୍ୟଥା {
            newCapacity=4*elementsCapacity;
        }
        BytesTrieElement *newElements=ନୂତନ BytesTrieElement[newCapacity];
        ଯଦି(newElements==nullptr) {
            errorCode=U_MEMORY_ALLOCATION_ERROR;
            ଫେରନ୍ତୁ *ଏହି; // error instead of dereferencing null
        }
        ଯଦି(elementsLength>0) {
            uprv_memcpy(newElements, elements, (size_t)elementsLength*ଆକାର(BytesTrieElement));
        }
        delete[] elements;
        elements=newElements;
        elementsCapacity=newCapacity;
    }
    elements[elementsLength++].setTo(s, value, *strings, errorCode);
    ଫେରନ୍ତୁ *ଏହି;
}

U_CDECL_BEGIN

ସ୍ଥିର int32_t U_CALLCONV
compareElementStrings(ଅଚଳ ଶୂନ୍ୟ *context, ଅଚଳ ଶୂନ୍ୟ *left, ଅଚଳ ଶୂନ୍ୟ *ଡାହାଣ) {
    ଅଚଳ CharString *strings=static_cast<ଅଚଳ CharString *>(context);
    ଅଚଳ BytesTrieElement *ବାମ_Element=static_cast<ଅଚଳ BytesTrieElement *>(left);
    ଅଚଳ BytesTrieElement *ଡାହାଣ_Element=static_cast<ଅଚଳ BytesTrieElement *>(ଡାହାଣ);
    ଫେରନ୍ତୁ ବାମ_Element->compareStringTo(*ଡାହାଣ_Element, *strings);
}

U_CDECL_END

BytesTrie *
BytesTrieBuilder::build(UStringTrieBuildOption buildOption, UErrorCode &errorCode) {
    buildBytes(buildOption, errorCode);
    BytesTrie *newTrie=nullptr;
    ଯଦି(U_SUCCESS(errorCode)) {
        newTrie=ନୂତନ BytesTrie(bytes, bytes+(bytesCapacity-bytesLength));
        ଯଦି(newTrie==nullptr) {
            errorCode=U_MEMORY_ALLOCATION_ERROR;
        } ଅନ୍ୟଥା {
            bytes=nullptr;  // The new trie now owns the array.
            bytesCapacity=0;
        }
    }
    ଫେରନ୍ତୁ newTrie;
}

StringPiece
BytesTrieBuilder::buildStringPiece(UStringTrieBuildOption buildOption,
                                   UErrorCode& errorCode) U_LIFETIME_BOUND {
    buildBytes(buildOption, errorCode);
    StringPiece result;
    ଯଦି(U_SUCCESS(errorCode)) {
        result.set(bytes+(bytesCapacity-bytesLength), bytesLength);
    }
    ଫେରନ୍ତୁ result;
}

ଶୂନ୍ୟ
BytesTrieBuilder::buildBytes(UStringTrieBuildOption buildOption, UErrorCode &errorCode) {
    ଯଦି(U_FAILURE(errorCode)) {
        ଫେରନ୍ତୁ;
    }
    ଯଦି(bytes!=nullptr && bytesLength>0) {
        // Already built.
        ଫେରନ୍ତୁ;
    }
    ଯଦି(bytesLength==0) {
        ଯଦି(elementsLength==0) {
            errorCode=U_INDEX_OUTOFBOUNDS_ERROR;
            ଫେରନ୍ତୁ;
        }
        uprv_sortArray(elements, elementsLength, static_cast<int32_t>(ଆକାର(BytesTrieElement)),
                      compareElementStrings, strings,
                      ମିଥ୍ୟା,  // need not be a stable sort
                      &errorCode);
        ଯଦି(U_FAILURE(errorCode)) {
            ଫେରନ୍ତୁ;
        }
        // Duplicate strings are not allowed.
        StringPiece prev=elements[0].getString(*strings);
        ପାଇଁ(int32_t i=1; i<elementsLength; ++i) {
            StringPiece current=elements[i].getString(*strings);
            ଯଦି(prev==current) {
                errorCode=U_ILLEGAL_ARGUMENT_ERROR;
                ଫେରନ୍ତୁ;
            }
            prev=current;
        }
    }
    // Create and byte-serialize the trie for the elements.
    bytesLength=0;
    int32_t capacity=strings->length();
    ଯଦି(capacity<1024) {
        capacity=1024;
    }
    ଯଦି(bytesCapacity<capacity) {
        uprv_free(bytes);
        bytes=static_cast<ଅକ୍ଷର *>(uprv_malloc(capacity));
        ଯଦି(bytes==nullptr) {
            errorCode=U_MEMORY_ALLOCATION_ERROR;
            bytesCapacity=0;
            ଫେରନ୍ତୁ;
        }
        bytesCapacity=capacity;
    }
    StringTrieBuilder::build(buildOption, elementsLength, errorCode);
    ଯଦି(bytes==nullptr) {
        errorCode=U_MEMORY_ALLOCATION_ERROR;
    }
}

BytesTrieBuilder&
BytesTrieBuilder::clear() U_LIFETIME_BOUND {
    strings->clear();
    elementsLength=0;
    bytesLength=0;
    ଫେରନ୍ତୁ *ଏହି;
}

int32_t
BytesTrieBuilder::getElementStringLength(int32_t i) ଅଚଳ {
    ଫେରନ୍ତୁ elements[i].getStringLength(*strings);
}

char16_t
BytesTrieBuilder::getElementUnit(int32_t i, int32_t byteIndex) ଅଚଳ {
    ଫେରନ୍ତୁ static_cast<uint8_t>(elements[i].charAt(byteIndex, *strings));
}

int32_t
BytesTrieBuilder::get_Element_ମୂଲ୍ୟ(int32_t i) ଅଚଳ {
    ଫେରନ୍ତୁ elements[i].get_ମୂଲ୍ୟ();
}

int32_t
BytesTrieBuilder::getLimitOfLinearMatch(int32_t first, int32_t last, int32_t byteIndex) ଅଚଳ {
    ଅଚଳ BytesTrieElement &firstElement=elements[first];
    ଅଚଳ BytesTrieElement &lastElement=elements[last];
    int32_t minStringLength=firstElement.getStringLength(*strings);
    ଯେତେବେଳେ(++byteIndex<minStringLength &&
            firstElement.charAt(byteIndex, *strings)==
            lastElement.charAt(byteIndex, *strings)) {}
    ଫେରନ୍ତୁ byteIndex;
}

int32_t
BytesTrieBuilder::ଗଣନା_Element_Units(int32_t start, int32_t limit, int32_t byteIndex) ଅଚଳ {
    int32_t length=0;  // Number of different bytes at byteIndex.
    int32_t i=start;
    କର {
        ଅକ୍ଷର byte=elements[i++].charAt(byteIndex, *strings);
        ଯେତେବେଳେ(i<limit && byte==elements[i].charAt(byteIndex, *strings)) {
            ++i;
        }
        ++length;
    } ଯେତେବେଳେ(i<limit);
    ଫେରନ୍ତୁ length;
}

int32_t
BytesTrieBuilder::skipElementsBySomeUnits(int32_t i, int32_t byteIndex, int32_t count) ଅଚଳ {
    କର {
        ଅକ୍ଷର byte=elements[i++].charAt(byteIndex, *strings);
        ଯେତେବେଳେ(byte==elements[i].charAt(byteIndex, *strings)) {
            ++i;
        }
    } ଯେତେବେଳେ(--count>0);
    ଫେରନ୍ତୁ i;
}

int32_t
BytesTrieBuilder::indexOfElementWithNextUnit(int32_t i, int32_t byteIndex, char16_t byte) ଅଚଳ {
    ଅକ୍ଷର b = static_cast<ଅକ୍ଷର>(byte);
    ଯେତେବେଳେ(b==elements[i].charAt(byteIndex, *strings)) {
        ++i;
    }
    ଫେରନ୍ତୁ i;
}

BytesTrieBuilder::BTLinear_Match_ଗଣ୍ଠି::BTLinear_Match_ଗଣ୍ଠି(ଅଚଳ ଅକ୍ଷର *bytes, int32_t len, Node *next_ଗଣ୍ଠି)
        : Linear_Match_ଗଣ୍ଠି(len, next_ଗଣ୍ଠି), s(bytes) {
    hash=static_cast<int32_t>(
        static_cast<uint32_t>(hash)*37u + static_cast<uint32_t>(ustr_hashCharsN(bytes, len)));
}

ତାର୍କିକ
BytesTrieBuilder::BTLinear_Match_ଗଣ୍ଠି::operator==(ଅଚଳ Node &other) ଅଚଳ {
    ଯଦି(ଏହି==&other) {
        ଫେରନ୍ତୁ ସତ୍ୟ;
    }
    ଯଦି(!Linear_Match_ଗଣ୍ଠି::operator==(other)) {
        ଫେରନ୍ତୁ ମିଥ୍ୟା;
    }
    ଅଚଳ BTLinear_Match_ଗଣ୍ଠି &o=static_cast<ଅଚଳ BTLinear_Match_ଗଣ୍ଠି &>(other);
    ଫେରନ୍ତୁ 0==uprv_memcmp(s, o.s, length);
}

ଶୂନ୍ୟ
BytesTrieBuilder::BTLinear_Match_ଗଣ୍ଠି::ଲିଖ(StringTrieBuilder &builder) {
    BytesTrieBuilder &b=static_cast<BytesTrieBuilder &>(builder);
    next->ଲିଖ(builder);
    b.ଲିଖ(s, length);
    offset=b.ଲିଖ(b.getMinLinearMatch()+length-1);
}

StringTrieBuilder::Node *
BytesTrieBuilder::create_Linear_Match_ଗଣ୍ଠି(int32_t i, int32_t byteIndex, int32_t length,
                                        Node *next_ଗଣ୍ଠି) ଅଚଳ {
    ଫେରନ୍ତୁ ନୂତନ BTLinear_Match_ଗଣ୍ଠି(
            elements[i].getString(*strings).data()+byteIndex,
            length,
            next_ଗଣ୍ଠି);
}

UBool
BytesTrieBuilder::ensureCapacity(int32_t length) {
    ଯଦି(bytes==nullptr) {
        ଫେରନ୍ତୁ ମିଥ୍ୟା;  // previous memory allocation had failed
    }
    ଯଦି(length>bytesCapacity) {
        int32_t newCapacity=bytesCapacity;
        କର {
            newCapacity*=2;
        } ଯେତେବେଳେ(newCapacity<=length);
        ଅକ୍ଷର *newBytes=static_cast<ଅକ୍ଷର *>(uprv_malloc(newCapacity));
        ଯଦି(newBytes==nullptr) {
            // unable to allocate memory
            uprv_free(bytes);
            bytes=nullptr;
            bytesCapacity=0;
            ଫେରନ୍ତୁ ମିଥ୍ୟା;
        }
        uprv_memcpy(newBytes+(newCapacity-bytesLength),
                    bytes+(bytesCapacity-bytesLength), bytesLength);
        uprv_free(bytes);
        bytes=newBytes;
        bytesCapacity=newCapacity;
    }
    ଫେରନ୍ତୁ ସତ୍ୟ;
}

int32_t
BytesTrieBuilder::ଲିଖ(int32_t byte) {
    int32_t newLength=bytesLength+1;
    ଯଦି(ensureCapacity(newLength)) {
        bytesLength=newLength;
        bytes[bytesCapacity - bytesLength] = static_cast<ଅକ୍ଷର>(byte);
    }
    ଫେରନ୍ତୁ bytesLength;
}

int32_t
BytesTrieBuilder::ଲିଖ(ଅଚଳ ଅକ୍ଷର *b, int32_t length) {
    int32_t newLength=bytesLength+length;
    ଯଦି(ensureCapacity(newLength)) {
        bytesLength=newLength;
        uprv_memcpy(bytes+(bytesCapacity-bytesLength), b, length);
    }
    ଫେରନ୍ତୁ bytesLength;
}

int32_t
BytesTrieBuilder::writeElementUnits(int32_t i, int32_t byteIndex, int32_t length) {
    ଫେରନ୍ତୁ ଲିଖ(elements[i].getString(*strings).data()+byteIndex, length);
}

int32_t
BytesTrieBuilder::write_ମୂଲ୍ୟ_And_Final(int32_t i, UBool isFinal) {
    ଯଦି(0<=i && i<=BytesTrie::k_Max_One_Byte_ମୂଲ୍ୟ) {
        ଫେରନ୍ତୁ ଲିଖ(((BytesTrie::k_Min_One_Byte_ମୂଲ୍ୟ_Lead+i)<<1)|isFinal);
    }
    ଅକ୍ଷର intBytes[5];
    int32_t length=1;
    ଯଦି(i<0 || i>0xffffff) {
        intBytes[0] = static_cast<ଅକ୍ଷର>(BytesTrie::k_Five_Byte_ମୂଲ୍ୟ_Lead);
        intBytes[1] = static_cast<ଅକ୍ଷର>(static_cast<uint32_t>(i) >> 24);
        intBytes[2] = static_cast<ଅକ୍ଷର>(static_cast<uint32_t>(i) >> 16);
        intBytes[3] = static_cast<ଅକ୍ଷର>(static_cast<uint32_t>(i) >> 8);
        intBytes[4] = static_cast<ଅକ୍ଷର>(i);
        length=5;
    // } else if(i<=BytesTrie::kMaxOneByteValue) {
    //     intBytes[0]=(char)(BytesTrie::kMinOneByteValueLead+i);
    } ଅନ୍ୟଥା {
        ଯଦି(i<=BytesTrie::k_Max_Two_Byte_ମୂଲ୍ୟ) {
            intBytes[0] = static_cast<ଅକ୍ଷର>(BytesTrie::k_Min_Two_Byte_ମୂଲ୍ୟ_Lead + (i >> 8));
        } ଅନ୍ୟଥା {
            ଯଦି(i<=BytesTrie::k_Max_Three_Byte_ମୂଲ୍ୟ) {
                intBytes[0] = static_cast<ଅକ୍ଷର>(BytesTrie::k_Min_Three_Byte_ମୂଲ୍ୟ_Lead + (i >> 16));
            } ଅନ୍ୟଥା {
                intBytes[0] = static_cast<ଅକ୍ଷର>(BytesTrie::k_Four_Byte_ମୂଲ୍ୟ_Lead);
                intBytes[1] = static_cast<ଅକ୍ଷର>(i >> 16);
                length=2;
            }
            intBytes[length++] = static_cast<ଅକ୍ଷର>(i >> 8);
        }
        intBytes[length++] = static_cast<ଅକ୍ଷର>(i);
    }
    intBytes[0] = static_cast<ଅକ୍ଷର>((intBytes[0] << 1) | isFinal);
    ଫେରନ୍ତୁ ଲିଖ(intBytes, length);
}

int32_t
BytesTrieBuilder::write_ମୂଲ୍ୟ_And_Type(UBool has_ମୂଲ୍ୟ, int32_t value, int32_t ଗଣ୍ଠି) {
    int32_t offset=ଲିଖ(ଗଣ୍ଠି);
    ଯଦି(has_ମୂଲ୍ୟ) {
        offset=write_ମୂଲ୍ୟ_And_Final(value, ମିଥ୍ୟା);
    }
    ଫେରନ୍ତୁ offset;
}

int32_t
BytesTrieBuilder::writeDeltaTo(int32_t jumpTarget) {
    int32_t i=bytesLength-jumpTarget;
    U_ASSERT(i>=0);
    ଯଦି(i<=BytesTrie::kMaxOneByteDelta) {
        ଫେରନ୍ତୁ ଲିଖ(i);
    } ଅନ୍ୟଥା {
        ଅକ୍ଷର intBytes[5];
        ଫେରନ୍ତୁ ଲିଖ(intBytes, internalEncodeDelta(i, intBytes));
    }
}

int32_t
BytesTrieBuilder::internalEncodeDelta(int32_t i, ଅକ୍ଷର intBytes[]) {
    U_ASSERT(i>=0);
    ଯଦି(i<=BytesTrie::kMaxOneByteDelta) {
        intBytes[0] = static_cast<ଅକ୍ଷର>(i);
        ଫେରନ୍ତୁ 1;
    }
    int32_t length=1;
    ଯଦି(i<=BytesTrie::kMaxTwoByteDelta) {
        intBytes[0] = static_cast<ଅକ୍ଷର>(BytesTrie::kMinTwoByteDeltaLead + (i >> 8));
    } ଅନ୍ୟଥା {
        ଯଦି(i<=BytesTrie::kMaxThreeByteDelta) {
            intBytes[0] = static_cast<ଅକ୍ଷର>(BytesTrie::kMinThreeByteDeltaLead + (i >> 16));
        } ଅନ୍ୟଥା {
            ଯଦି(i<=0xffffff) {
                intBytes[0] = static_cast<ଅକ୍ଷର>(BytesTrie::kFourByteDeltaLead);
            } ଅନ୍ୟଥା {
                intBytes[0] = static_cast<ଅକ୍ଷର>(BytesTrie::kFiveByteDeltaLead);
                intBytes[1] = static_cast<ଅକ୍ଷର>(i >> 24);
                length=2;
            }
            intBytes[length++] = static_cast<ଅକ୍ଷର>(i >> 16);
        }
        intBytes[length++] = static_cast<ଅକ୍ଷର>(i >> 8);
    }
    intBytes[length++] = static_cast<ଅକ୍ଷର>(i);
    ଫେରନ୍ତୁ length;
}

U_NAMESPACE_END
