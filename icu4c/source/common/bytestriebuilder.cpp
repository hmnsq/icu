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

#અન્તર્ભાવ "unicode/utypes.h"
#અન્તર્ભાવ "unicode/bytestrie.h"
#અન્તર્ભાવ "unicode/bytestriebuilder.h"
#અન્તર્ભાવ "unicode/stringpiece.h"
#અન્તર્ભાવ "charstr.h"
#અન્તર્ભાવ "cmemory.h"
#અન્તર્ભાવ "uhash.h"
#અન્તર્ભાવ "uarrsort.h"
#અન્તર્ભાવ "uassert.h"
#અન્તર્ભાવ "ustr_imp.h"

U_NAMESPACE_BEGIN

/*
 * Note: This builder implementation stores (bytes, value) pairs with full copies
 * of the byte sequences, until the BytesTrie is built.
 * It might(!) take less memory if we collected the data in a temporary, dynamic trie.
 */

વર્ગ BytesTrieElement : જાહેર UMemory {
જાહેર:
    // Use compiler's default constructor, initializes nothing.

    શૂન્ય setTo(StringPiece s, int32_t val, CharString &strings, UErrorCode &errorCode);

    StringPiece getString(અચલ CharString& strings U_LIFETIME_BOUND) અચલ {
        int32_t offset=stringOffset;
        int32_t length;
        જો(offset>=0) {
            length = static_cast<uint8_t>(strings[offset++]);
        } નહીંતર {
            offset=~offset;
            length = (static_cast<int32_t>(static_cast<uint8_t>(strings[offset])) << 8) | static_cast<uint8_t>(strings[offset + 1]);
            offset+=2;
        }
        પાછા StringPiece(strings.data()+offset, length);
    }
    int32_t getStringLength(અચલ CharString &strings) અચલ {
        int32_t offset=stringOffset;
        જો(offset>=0) {
            પાછા static_cast<uint8_t>(strings[offset]);
        } નહીંતર {
            offset=~offset;
            પાછા (static_cast<int32_t>(static_cast<uint8_t>(strings[offset])) << 8) | static_cast<uint8_t>(strings[offset + 1]);
        }
    }

    અક્ષર charAt(int32_t index, અચલ CharString &strings) અચલ { પાછા data(strings)[index]; }

    int32_t get_કિંમત() અચલ { પાછા value; }

    int32_t compareStringTo(અચલ BytesTrieElement &o, અચલ CharString &strings) અચલ;

ખાનગી:
    અચલ અક્ષર* data(અચલ CharString& strings U_LIFETIME_BOUND) અચલ {
        int32_t offset=stringOffset;
        જો(offset>=0) {
            ++offset;
        } નહીંતર {
            offset=~offset+2;
        }
        પાછા strings.data()+offset;
    }

    // If the stringOffset is non-negative, then the first strings byte contains
    // the string length.
    // If the stringOffset is negative, then the first two strings bytes contain
    // the string length (big-endian), and the offset needs to be bit-inverted.
    // (Compared with a stringLength field here, this saves 3 bytes per string for most strings.)
    int32_t stringOffset;
    int32_t value;
};

શૂન્ય
BytesTrieElement::setTo(StringPiece s, int32_t val,
                        CharString &strings, UErrorCode &errorCode) {
    જો(U_FAILURE(errorCode)) {
        પાછા;
    }
    int32_t length=s.length();
    જો(length>0xffff) {
        // Too long: We store the length in 1 or 2 bytes.
        errorCode=U_INDEX_OUTOFBOUNDS_ERROR;
        પાછા;
    }
    int32_t offset=strings.length();
    જો(length>0xff) {
        offset=~offset;
        strings.append(static_cast<અક્ષર>(length >> 8), errorCode);
    }
    strings.append(static_cast<અક્ષર>(length), errorCode);
    stringOffset=offset;
    value=val;
    strings.append(s, errorCode);
}

int32_t
BytesTrieElement::compareStringTo(અચલ BytesTrieElement &other, અચલ CharString &strings) અચલ {
    // TODO: add StringPiece::compare(), see ticket #8187
    StringPiece thisString=getString(strings);
    StringPiece otherString=other.getString(strings);
    int32_t lengthDiff=thisString.length()-otherString.length();
    int32_t commonLength;
    જો(lengthDiff<=0) {
        commonLength=thisString.length();
    } નહીંતર {
        commonLength=otherString.length();
    }
    int32_t diff=uprv_memcmp(thisString.data(), otherString.data(), commonLength);
    પાછા diff!=0 ? diff : lengthDiff;
}

BytesTrieBuilder::BytesTrieBuilder(UErrorCode &errorCode)
        : strings(nullptr), elements(nullptr), elementsCapacity(0), elementsLength(0),
          bytes(nullptr), bytesCapacity(0), bytesLength(0) {
    જો(U_FAILURE(errorCode)) {
        પાછા;
    }
    strings=નવું CharString();
    જો(strings==nullptr) {
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
    જો(U_FAILURE(errorCode)) {
        પાછા *આ;
    }
    જો(bytesLength>0) {
        // Cannot add elements after building.
        errorCode=U_NO_WRITE_PERMISSION;
        પાછા *આ;
    }
    જો(elementsLength==elementsCapacity) {
        int32_t newCapacity;
        જો(elementsCapacity==0) {
            newCapacity=1024;
        } નહીંતર {
            newCapacity=4*elementsCapacity;
        }
        BytesTrieElement *newElements=નવું BytesTrieElement[newCapacity];
        જો(newElements==nullptr) {
            errorCode=U_MEMORY_ALLOCATION_ERROR;
            પાછા *આ; // error instead of dereferencing null
        }
        જો(elementsLength>0) {
            uprv_memcpy(newElements, elements, (size_t)elementsLength*કદ(BytesTrieElement));
        }
        delete[] elements;
        elements=newElements;
        elementsCapacity=newCapacity;
    }
    elements[elementsLength++].setTo(s, value, *strings, errorCode);
    પાછા *આ;
}

U_CDECL_BEGIN

સ્થિર int32_t U_CALLCONV
compareElementStrings(અચલ શૂન્ય *context, અચલ શૂન્ય *left, અચલ શૂન્ય *જમણું) {
    અચલ CharString *strings=static_cast<અચલ CharString *>(context);
    અચલ BytesTrieElement *ડાબું_Element=static_cast<અચલ BytesTrieElement *>(left);
    અચલ BytesTrieElement *જમણું_Element=static_cast<અચલ BytesTrieElement *>(જમણું);
    પાછા ડાબું_Element->compareStringTo(*જમણું_Element, *strings);
}

U_CDECL_END

BytesTrie *
BytesTrieBuilder::build(UStringTrieBuildOption buildOption, UErrorCode &errorCode) {
    buildBytes(buildOption, errorCode);
    BytesTrie *newTrie=nullptr;
    જો(U_SUCCESS(errorCode)) {
        newTrie=નવું BytesTrie(bytes, bytes+(bytesCapacity-bytesLength));
        જો(newTrie==nullptr) {
            errorCode=U_MEMORY_ALLOCATION_ERROR;
        } નહીંતર {
            bytes=nullptr;  // The new trie now owns the array.
            bytesCapacity=0;
        }
    }
    પાછા newTrie;
}

StringPiece
BytesTrieBuilder::buildStringPiece(UStringTrieBuildOption buildOption,
                                   UErrorCode& errorCode) U_LIFETIME_BOUND {
    buildBytes(buildOption, errorCode);
    StringPiece result;
    જો(U_SUCCESS(errorCode)) {
        result.set(bytes+(bytesCapacity-bytesLength), bytesLength);
    }
    પાછા result;
}

શૂન્ય
BytesTrieBuilder::buildBytes(UStringTrieBuildOption buildOption, UErrorCode &errorCode) {
    જો(U_FAILURE(errorCode)) {
        પાછા;
    }
    જો(bytes!=nullptr && bytesLength>0) {
        // Already built.
        પાછા;
    }
    જો(bytesLength==0) {
        જો(elementsLength==0) {
            errorCode=U_INDEX_OUTOFBOUNDS_ERROR;
            પાછા;
        }
        uprv_sortArray(elements, elementsLength, static_cast<int32_t>(કદ(BytesTrieElement)),
                      compareElementStrings, strings,
                      ખોટું,  // need not be a stable sort
                      &errorCode);
        જો(U_FAILURE(errorCode)) {
            પાછા;
        }
        // Duplicate strings are not allowed.
        StringPiece prev=elements[0].getString(*strings);
        માટે(int32_t i=1; i<elementsLength; ++i) {
            StringPiece current=elements[i].getString(*strings);
            જો(prev==current) {
                errorCode=U_ILLEGAL_ARGUMENT_ERROR;
                પાછા;
            }
            prev=current;
        }
    }
    // Create and byte-serialize the trie for the elements.
    bytesLength=0;
    int32_t capacity=strings->length();
    જો(capacity<1024) {
        capacity=1024;
    }
    જો(bytesCapacity<capacity) {
        uprv_free(bytes);
        bytes=static_cast<અક્ષર *>(uprv_malloc(capacity));
        જો(bytes==nullptr) {
            errorCode=U_MEMORY_ALLOCATION_ERROR;
            bytesCapacity=0;
            પાછા;
        }
        bytesCapacity=capacity;
    }
    StringTrieBuilder::build(buildOption, elementsLength, errorCode);
    જો(bytes==nullptr) {
        errorCode=U_MEMORY_ALLOCATION_ERROR;
    }
}

BytesTrieBuilder&
BytesTrieBuilder::clear() U_LIFETIME_BOUND {
    strings->clear();
    elementsLength=0;
    bytesLength=0;
    પાછા *આ;
}

int32_t
BytesTrieBuilder::getElementStringLength(int32_t i) અચલ {
    પાછા elements[i].getStringLength(*strings);
}

char16_t
BytesTrieBuilder::getElementUnit(int32_t i, int32_t byteIndex) અચલ {
    પાછા static_cast<uint8_t>(elements[i].charAt(byteIndex, *strings));
}

int32_t
BytesTrieBuilder::get_Element_કિંમત(int32_t i) અચલ {
    પાછા elements[i].get_કિંમત();
}

int32_t
BytesTrieBuilder::getLimitOfLinearMatch(int32_t first, int32_t last, int32_t byteIndex) અચલ {
    અચલ BytesTrieElement &firstElement=elements[first];
    અચલ BytesTrieElement &lastElement=elements[last];
    int32_t minStringLength=firstElement.getStringLength(*strings);
    જ્યારે(++byteIndex<minStringLength &&
            firstElement.charAt(byteIndex, *strings)==
            lastElement.charAt(byteIndex, *strings)) {}
    પાછા byteIndex;
}

int32_t
BytesTrieBuilder::ગણતરી_Element_Units(int32_t start, int32_t limit, int32_t byteIndex) અચલ {
    int32_t length=0;  // Number of different bytes at byteIndex.
    int32_t i=start;
    કરો {
        અક્ષર byte=elements[i++].charAt(byteIndex, *strings);
        જ્યારે(i<limit && byte==elements[i].charAt(byteIndex, *strings)) {
            ++i;
        }
        ++length;
    } જ્યારે(i<limit);
    પાછા length;
}

int32_t
BytesTrieBuilder::skipElementsBySomeUnits(int32_t i, int32_t byteIndex, int32_t count) અચલ {
    કરો {
        અક્ષર byte=elements[i++].charAt(byteIndex, *strings);
        જ્યારે(byte==elements[i].charAt(byteIndex, *strings)) {
            ++i;
        }
    } જ્યારે(--count>0);
    પાછા i;
}

int32_t
BytesTrieBuilder::indexOfElementWithNextUnit(int32_t i, int32_t byteIndex, char16_t byte) અચલ {
    અક્ષર b = static_cast<અક્ષર>(byte);
    જ્યારે(b==elements[i].charAt(byteIndex, *strings)) {
        ++i;
    }
    પાછા i;
}

BytesTrieBuilder::BTLinear_Match_ગાંઠ::BTLinear_Match_ગાંઠ(અચલ અક્ષર *bytes, int32_t len, Node *next_ગાંઠ)
        : Linear_Match_ગાંઠ(len, next_ગાંઠ), s(bytes) {
    hash=static_cast<int32_t>(
        static_cast<uint32_t>(hash)*37u + static_cast<uint32_t>(ustr_hashCharsN(bytes, len)));
}

તાર્કિક
BytesTrieBuilder::BTLinear_Match_ગાંઠ::operator==(અચલ Node &other) અચલ {
    જો(આ==&other) {
        પાછા સાચું;
    }
    જો(!Linear_Match_ગાંઠ::operator==(other)) {
        પાછા ખોટું;
    }
    અચલ BTLinear_Match_ગાંઠ &o=static_cast<અચલ BTLinear_Match_ગાંઠ &>(other);
    પાછા 0==uprv_memcmp(s, o.s, length);
}

શૂન્ય
BytesTrieBuilder::BTLinear_Match_ગાંઠ::લિખ(StringTrieBuilder &builder) {
    BytesTrieBuilder &b=static_cast<BytesTrieBuilder &>(builder);
    next->લિખ(builder);
    b.લિખ(s, length);
    offset=b.લિખ(b.getMinLinearMatch()+length-1);
}

StringTrieBuilder::Node *
BytesTrieBuilder::create_Linear_Match_ગાંઠ(int32_t i, int32_t byteIndex, int32_t length,
                                        Node *next_ગાંઠ) અચલ {
    પાછા નવું BTLinear_Match_ગાંઠ(
            elements[i].getString(*strings).data()+byteIndex,
            length,
            next_ગાંઠ);
}

UBool
BytesTrieBuilder::ensureCapacity(int32_t length) {
    જો(bytes==nullptr) {
        પાછા ખોટું;  // previous memory allocation had failed
    }
    જો(length>bytesCapacity) {
        int32_t newCapacity=bytesCapacity;
        કરો {
            newCapacity*=2;
        } જ્યારે(newCapacity<=length);
        અક્ષર *newBytes=static_cast<અક્ષર *>(uprv_malloc(newCapacity));
        જો(newBytes==nullptr) {
            // unable to allocate memory
            uprv_free(bytes);
            bytes=nullptr;
            bytesCapacity=0;
            પાછા ખોટું;
        }
        uprv_memcpy(newBytes+(newCapacity-bytesLength),
                    bytes+(bytesCapacity-bytesLength), bytesLength);
        uprv_free(bytes);
        bytes=newBytes;
        bytesCapacity=newCapacity;
    }
    પાછા સાચું;
}

int32_t
BytesTrieBuilder::લિખ(int32_t byte) {
    int32_t newLength=bytesLength+1;
    જો(ensureCapacity(newLength)) {
        bytesLength=newLength;
        bytes[bytesCapacity - bytesLength] = static_cast<અક્ષર>(byte);
    }
    પાછા bytesLength;
}

int32_t
BytesTrieBuilder::લિખ(અચલ અક્ષર *b, int32_t length) {
    int32_t newLength=bytesLength+length;
    જો(ensureCapacity(newLength)) {
        bytesLength=newLength;
        uprv_memcpy(bytes+(bytesCapacity-bytesLength), b, length);
    }
    પાછા bytesLength;
}

int32_t
BytesTrieBuilder::writeElementUnits(int32_t i, int32_t byteIndex, int32_t length) {
    પાછા લિખ(elements[i].getString(*strings).data()+byteIndex, length);
}

int32_t
BytesTrieBuilder::write_કિંમત_And_Final(int32_t i, UBool isFinal) {
    જો(0<=i && i<=BytesTrie::k_Max_One_Byte_કિંમત) {
        પાછા લિખ(((BytesTrie::k_Min_One_Byte_કિંમત_Lead+i)<<1)|isFinal);
    }
    અક્ષર intBytes[5];
    int32_t length=1;
    જો(i<0 || i>0xffffff) {
        intBytes[0] = static_cast<અક્ષર>(BytesTrie::k_Five_Byte_કિંમત_Lead);
        intBytes[1] = static_cast<અક્ષર>(static_cast<uint32_t>(i) >> 24);
        intBytes[2] = static_cast<અક્ષર>(static_cast<uint32_t>(i) >> 16);
        intBytes[3] = static_cast<અક્ષર>(static_cast<uint32_t>(i) >> 8);
        intBytes[4] = static_cast<અક્ષર>(i);
        length=5;
    // } else if(i<=BytesTrie::kMaxOneByteValue) {
    //     intBytes[0]=(char)(BytesTrie::kMinOneByteValueLead+i);
    } નહીંતર {
        જો(i<=BytesTrie::k_Max_Two_Byte_કિંમત) {
            intBytes[0] = static_cast<અક્ષર>(BytesTrie::k_Min_Two_Byte_કિંમત_Lead + (i >> 8));
        } નહીંતર {
            જો(i<=BytesTrie::k_Max_Three_Byte_કિંમત) {
                intBytes[0] = static_cast<અક્ષર>(BytesTrie::k_Min_Three_Byte_કિંમત_Lead + (i >> 16));
            } નહીંતર {
                intBytes[0] = static_cast<અક્ષર>(BytesTrie::k_Four_Byte_કિંમત_Lead);
                intBytes[1] = static_cast<અક્ષર>(i >> 16);
                length=2;
            }
            intBytes[length++] = static_cast<અક્ષર>(i >> 8);
        }
        intBytes[length++] = static_cast<અક્ષર>(i);
    }
    intBytes[0] = static_cast<અક્ષર>((intBytes[0] << 1) | isFinal);
    પાછા લિખ(intBytes, length);
}

int32_t
BytesTrieBuilder::write_કિંમત_And_Type(UBool has_કિંમત, int32_t value, int32_t ગાંઠ) {
    int32_t offset=લિખ(ગાંઠ);
    જો(has_કિંમત) {
        offset=write_કિંમત_And_Final(value, ખોટું);
    }
    પાછા offset;
}

int32_t
BytesTrieBuilder::writeDeltaTo(int32_t jumpTarget) {
    int32_t i=bytesLength-jumpTarget;
    U_ASSERT(i>=0);
    જો(i<=BytesTrie::kMaxOneByteDelta) {
        પાછા લિખ(i);
    } નહીંતર {
        અક્ષર intBytes[5];
        પાછા લિખ(intBytes, internalEncodeDelta(i, intBytes));
    }
}

int32_t
BytesTrieBuilder::internalEncodeDelta(int32_t i, અક્ષર intBytes[]) {
    U_ASSERT(i>=0);
    જો(i<=BytesTrie::kMaxOneByteDelta) {
        intBytes[0] = static_cast<અક્ષર>(i);
        પાછા 1;
    }
    int32_t length=1;
    જો(i<=BytesTrie::kMaxTwoByteDelta) {
        intBytes[0] = static_cast<અક્ષર>(BytesTrie::kMinTwoByteDeltaLead + (i >> 8));
    } નહીંતર {
        જો(i<=BytesTrie::kMaxThreeByteDelta) {
            intBytes[0] = static_cast<અક્ષર>(BytesTrie::kMinThreeByteDeltaLead + (i >> 16));
        } નહીંતર {
            જો(i<=0xffffff) {
                intBytes[0] = static_cast<અક્ષર>(BytesTrie::kFourByteDeltaLead);
            } નહીંતર {
                intBytes[0] = static_cast<અક્ષર>(BytesTrie::kFiveByteDeltaLead);
                intBytes[1] = static_cast<અક્ષર>(i >> 24);
                length=2;
            }
            intBytes[length++] = static_cast<અક્ષર>(i >> 16);
        }
        intBytes[length++] = static_cast<અક્ષર>(i >> 8);
    }
    intBytes[length++] = static_cast<અક્ષર>(i);
    પાછા length;
}

U_NAMESPACE_END
