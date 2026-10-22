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

#അന്തര്ഭാവ "unicode/utypes.h"
#അന്തര്ഭാവ "unicode/bytestrie.h"
#അന്തര്ഭാവ "unicode/bytestriebuilder.h"
#അന്തര്ഭാവ "unicode/stringpiece.h"
#അന്തര്ഭാവ "charstr.h"
#അന്തര്ഭാവ "cmemory.h"
#അന്തര്ഭാവ "uhash.h"
#അന്തര്ഭാവ "uarrsort.h"
#അന്തര്ഭാവ "uassert.h"
#അന്തര്ഭാവ "ustr_imp.h"

U_NAMESPACE_BEGIN

/*
 * Note: This builder implementation stores (bytes, value) pairs with full copies
 * of the byte sequences, until the BytesTrie is built.
 * It might(!) take less memory if we collected the data in a temporary, dynamic trie.
 */

വർഗ്ഗം BytesTrieElement : പൊതു UMemory {
പൊതു:
    // Use compiler's default constructor, initializes nothing.

    ശൂന്യം setTo(StringPiece s, int32_t val, CharString &strings, UErrorCode &errorCode);

    StringPiece getString(സ്ഥിരം CharString& strings U_LIFETIME_BOUND) സ്ഥിരം {
        int32_t offset=stringOffset;
        int32_t length;
        എങ്കിൽ(offset>=0) {
            length = static_cast<uint8_t>(strings[offset++]);
        } അല്ലെങ്കിൽ {
            offset=~offset;
            length = (static_cast<int32_t>(static_cast<uint8_t>(strings[offset])) << 8) | static_cast<uint8_t>(strings[offset + 1]);
            offset+=2;
        }
        തിരികെ StringPiece(strings.data()+offset, length);
    }
    int32_t getStringLength(സ്ഥിരം CharString &strings) സ്ഥിരം {
        int32_t offset=stringOffset;
        എങ്കിൽ(offset>=0) {
            തിരികെ static_cast<uint8_t>(strings[offset]);
        } അല്ലെങ്കിൽ {
            offset=~offset;
            തിരികെ (static_cast<int32_t>(static_cast<uint8_t>(strings[offset])) << 8) | static_cast<uint8_t>(strings[offset + 1]);
        }
    }

    അക്ഷരം charAt(int32_t index, സ്ഥിരം CharString &strings) സ്ഥിരം { തിരികെ data(strings)[index]; }

    int32_t get_മൂല്യം() സ്ഥിരം { തിരികെ value; }

    int32_t compareStringTo(സ്ഥിരം BytesTrieElement &o, സ്ഥിരം CharString &strings) സ്ഥിരം;

സ്വകാര്യ:
    സ്ഥിരം അക്ഷരം* data(സ്ഥിരം CharString& strings U_LIFETIME_BOUND) സ്ഥിരം {
        int32_t offset=stringOffset;
        എങ്കിൽ(offset>=0) {
            ++offset;
        } അല്ലെങ്കിൽ {
            offset=~offset+2;
        }
        തിരികെ strings.data()+offset;
    }

    // If the stringOffset is non-negative, then the first strings byte contains
    // the string length.
    // If the stringOffset is negative, then the first two strings bytes contain
    // the string length (big-endian), and the offset needs to be bit-inverted.
    // (Compared with a stringLength field here, this saves 3 bytes per string for most strings.)
    int32_t stringOffset;
    int32_t value;
};

ശൂന്യം
BytesTrieElement::setTo(StringPiece s, int32_t val,
                        CharString &strings, UErrorCode &errorCode) {
    എങ്കിൽ(U_FAILURE(errorCode)) {
        തിരികെ;
    }
    int32_t length=s.length();
    എങ്കിൽ(length>0xffff) {
        // Too long: We store the length in 1 or 2 bytes.
        errorCode=U_INDEX_OUTOFBOUNDS_ERROR;
        തിരികെ;
    }
    int32_t offset=strings.length();
    എങ്കിൽ(length>0xff) {
        offset=~offset;
        strings.append(static_cast<അക്ഷരം>(length >> 8), errorCode);
    }
    strings.append(static_cast<അക്ഷരം>(length), errorCode);
    stringOffset=offset;
    value=val;
    strings.append(s, errorCode);
}

int32_t
BytesTrieElement::compareStringTo(സ്ഥിരം BytesTrieElement &other, സ്ഥിരം CharString &strings) സ്ഥിരം {
    // TODO: add StringPiece::compare(), see ticket #8187
    StringPiece thisString=getString(strings);
    StringPiece otherString=other.getString(strings);
    int32_t lengthDiff=thisString.length()-otherString.length();
    int32_t commonLength;
    എങ്കിൽ(lengthDiff<=0) {
        commonLength=thisString.length();
    } അല്ലെങ്കിൽ {
        commonLength=otherString.length();
    }
    int32_t diff=uprv_memcmp(thisString.data(), otherString.data(), commonLength);
    തിരികെ diff!=0 ? diff : lengthDiff;
}

BytesTrieBuilder::BytesTrieBuilder(UErrorCode &errorCode)
        : strings(nullptr), elements(nullptr), elementsCapacity(0), elementsLength(0),
          bytes(nullptr), bytesCapacity(0), bytesLength(0) {
    എങ്കിൽ(U_FAILURE(errorCode)) {
        തിരികെ;
    }
    strings=പുതിയ CharString();
    എങ്കിൽ(strings==nullptr) {
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
    എങ്കിൽ(U_FAILURE(errorCode)) {
        തിരികെ *ഇത്;
    }
    എങ്കിൽ(bytesLength>0) {
        // Cannot add elements after building.
        errorCode=U_NO_WRITE_PERMISSION;
        തിരികെ *ഇത്;
    }
    എങ്കിൽ(elementsLength==elementsCapacity) {
        int32_t newCapacity;
        എങ്കിൽ(elementsCapacity==0) {
            newCapacity=1024;
        } അല്ലെങ്കിൽ {
            newCapacity=4*elementsCapacity;
        }
        BytesTrieElement *newElements=പുതിയ BytesTrieElement[newCapacity];
        എങ്കിൽ(newElements==nullptr) {
            errorCode=U_MEMORY_ALLOCATION_ERROR;
            തിരികെ *ഇത്; // error instead of dereferencing null
        }
        എങ്കിൽ(elementsLength>0) {
            uprv_memcpy(newElements, elements, (size_t)elementsLength*വലുപ്പം(BytesTrieElement));
        }
        delete[] elements;
        elements=newElements;
        elementsCapacity=newCapacity;
    }
    elements[elementsLength++].setTo(s, value, *strings, errorCode);
    തിരികെ *ഇത്;
}

U_CDECL_BEGIN

സ്ഥിതം int32_t U_CALLCONV
compareElementStrings(സ്ഥിരം ശൂന്യം *context, സ്ഥിരം ശൂന്യം *left, സ്ഥിരം ശൂന്യം *വലത്) {
    സ്ഥിരം CharString *strings=static_cast<സ്ഥിരം CharString *>(context);
    സ്ഥിരം BytesTrieElement *ഇടത്_Element=static_cast<സ്ഥിരം BytesTrieElement *>(left);
    സ്ഥിരം BytesTrieElement *വലത്_Element=static_cast<സ്ഥിരം BytesTrieElement *>(വലത്);
    തിരികെ ഇടത്_Element->compareStringTo(*വലത്_Element, *strings);
}

U_CDECL_END

BytesTrie *
BytesTrieBuilder::build(UStringTrieBuildOption buildOption, UErrorCode &errorCode) {
    buildBytes(buildOption, errorCode);
    BytesTrie *newTrie=nullptr;
    എങ്കിൽ(U_SUCCESS(errorCode)) {
        newTrie=പുതിയ BytesTrie(bytes, bytes+(bytesCapacity-bytesLength));
        എങ്കിൽ(newTrie==nullptr) {
            errorCode=U_MEMORY_ALLOCATION_ERROR;
        } അല്ലെങ്കിൽ {
            bytes=nullptr;  // The new trie now owns the array.
            bytesCapacity=0;
        }
    }
    തിരികെ newTrie;
}

StringPiece
BytesTrieBuilder::buildStringPiece(UStringTrieBuildOption buildOption,
                                   UErrorCode& errorCode) U_LIFETIME_BOUND {
    buildBytes(buildOption, errorCode);
    StringPiece result;
    എങ്കിൽ(U_SUCCESS(errorCode)) {
        result.set(bytes+(bytesCapacity-bytesLength), bytesLength);
    }
    തിരികെ result;
}

ശൂന്യം
BytesTrieBuilder::buildBytes(UStringTrieBuildOption buildOption, UErrorCode &errorCode) {
    എങ്കിൽ(U_FAILURE(errorCode)) {
        തിരികെ;
    }
    എങ്കിൽ(bytes!=nullptr && bytesLength>0) {
        // Already built.
        തിരികെ;
    }
    എങ്കിൽ(bytesLength==0) {
        എങ്കിൽ(elementsLength==0) {
            errorCode=U_INDEX_OUTOFBOUNDS_ERROR;
            തിരികെ;
        }
        uprv_sortArray(elements, elementsLength, static_cast<int32_t>(വലുപ്പം(BytesTrieElement)),
                      compareElementStrings, strings,
                      അസത്യം,  // need not be a stable sort
                      &errorCode);
        എങ്കിൽ(U_FAILURE(errorCode)) {
            തിരികെ;
        }
        // Duplicate strings are not allowed.
        StringPiece prev=elements[0].getString(*strings);
        വേണ്ടി(int32_t i=1; i<elementsLength; ++i) {
            StringPiece current=elements[i].getString(*strings);
            എങ്കിൽ(prev==current) {
                errorCode=U_ILLEGAL_ARGUMENT_ERROR;
                തിരികെ;
            }
            prev=current;
        }
    }
    // Create and byte-serialize the trie for the elements.
    bytesLength=0;
    int32_t capacity=strings->length();
    എങ്കിൽ(capacity<1024) {
        capacity=1024;
    }
    എങ്കിൽ(bytesCapacity<capacity) {
        uprv_free(bytes);
        bytes=static_cast<അക്ഷരം *>(uprv_malloc(capacity));
        എങ്കിൽ(bytes==nullptr) {
            errorCode=U_MEMORY_ALLOCATION_ERROR;
            bytesCapacity=0;
            തിരികെ;
        }
        bytesCapacity=capacity;
    }
    StringTrieBuilder::build(buildOption, elementsLength, errorCode);
    എങ്കിൽ(bytes==nullptr) {
        errorCode=U_MEMORY_ALLOCATION_ERROR;
    }
}

BytesTrieBuilder&
BytesTrieBuilder::clear() U_LIFETIME_BOUND {
    strings->clear();
    elementsLength=0;
    bytesLength=0;
    തിരികെ *ഇത്;
}

int32_t
BytesTrieBuilder::getElementStringLength(int32_t i) സ്ഥിരം {
    തിരികെ elements[i].getStringLength(*strings);
}

char16_t
BytesTrieBuilder::getElementUnit(int32_t i, int32_t byteIndex) സ്ഥിരം {
    തിരികെ static_cast<uint8_t>(elements[i].charAt(byteIndex, *strings));
}

int32_t
BytesTrieBuilder::get_Element_മൂല്യം(int32_t i) സ്ഥിരം {
    തിരികെ elements[i].get_മൂല്യം();
}

int32_t
BytesTrieBuilder::getLimitOfLinearMatch(int32_t first, int32_t last, int32_t byteIndex) സ്ഥിരം {
    സ്ഥിരം BytesTrieElement &firstElement=elements[first];
    സ്ഥിരം BytesTrieElement &lastElement=elements[last];
    int32_t minStringLength=firstElement.getStringLength(*strings);
    ആകുമ്പോൾ(++byteIndex<minStringLength &&
            firstElement.charAt(byteIndex, *strings)==
            lastElement.charAt(byteIndex, *strings)) {}
    തിരികെ byteIndex;
}

int32_t
BytesTrieBuilder::എണ്ണം_Element_Units(int32_t start, int32_t limit, int32_t byteIndex) സ്ഥിരം {
    int32_t length=0;  // Number of different bytes at byteIndex.
    int32_t i=start;
    ചെയ്യുക {
        അക്ഷരം byte=elements[i++].charAt(byteIndex, *strings);
        ആകുമ്പോൾ(i<limit && byte==elements[i].charAt(byteIndex, *strings)) {
            ++i;
        }
        ++length;
    } ആകുമ്പോൾ(i<limit);
    തിരികെ length;
}

int32_t
BytesTrieBuilder::skipElementsBySomeUnits(int32_t i, int32_t byteIndex, int32_t count) സ്ഥിരം {
    ചെയ്യുക {
        അക്ഷരം byte=elements[i++].charAt(byteIndex, *strings);
        ആകുമ്പോൾ(byte==elements[i].charAt(byteIndex, *strings)) {
            ++i;
        }
    } ആകുമ്പോൾ(--count>0);
    തിരികെ i;
}

int32_t
BytesTrieBuilder::indexOfElementWithNextUnit(int32_t i, int32_t byteIndex, char16_t byte) സ്ഥിരം {
    അക്ഷരം b = static_cast<അക്ഷരം>(byte);
    ആകുമ്പോൾ(b==elements[i].charAt(byteIndex, *strings)) {
        ++i;
    }
    തിരികെ i;
}

BytesTrieBuilder::BTLinear_Match_കണ്ണി::BTLinear_Match_കണ്ണി(സ്ഥിരം അക്ഷരം *bytes, int32_t len, Node *next_കണ്ണി)
        : Linear_Match_കണ്ണി(len, next_കണ്ണി), s(bytes) {
    hash=static_cast<int32_t>(
        static_cast<uint32_t>(hash)*37u + static_cast<uint32_t>(ustr_hashCharsN(bytes, len)));
}

തർക്കം
BytesTrieBuilder::BTLinear_Match_കണ്ണി::operator==(സ്ഥിരം Node &other) സ്ഥിരം {
    എങ്കിൽ(ഇത്==&other) {
        തിരികെ സത്യം;
    }
    എങ്കിൽ(!Linear_Match_കണ്ണി::operator==(other)) {
        തിരികെ അസത്യം;
    }
    സ്ഥിരം BTLinear_Match_കണ്ണി &o=static_cast<സ്ഥിരം BTLinear_Match_കണ്ണി &>(other);
    തിരികെ 0==uprv_memcmp(s, o.s, length);
}

ശൂന്യം
BytesTrieBuilder::BTLinear_Match_കണ്ണി::ലിഖ(StringTrieBuilder &builder) {
    BytesTrieBuilder &b=static_cast<BytesTrieBuilder &>(builder);
    next->ലിഖ(builder);
    b.ലിഖ(s, length);
    offset=b.ലിഖ(b.getMinLinearMatch()+length-1);
}

StringTrieBuilder::Node *
BytesTrieBuilder::create_Linear_Match_കണ്ണി(int32_t i, int32_t byteIndex, int32_t length,
                                        Node *next_കണ്ണി) സ്ഥിരം {
    തിരികെ പുതിയ BTLinear_Match_കണ്ണി(
            elements[i].getString(*strings).data()+byteIndex,
            length,
            next_കണ്ണി);
}

UBool
BytesTrieBuilder::ensureCapacity(int32_t length) {
    എങ്കിൽ(bytes==nullptr) {
        തിരികെ അസത്യം;  // previous memory allocation had failed
    }
    എങ്കിൽ(length>bytesCapacity) {
        int32_t newCapacity=bytesCapacity;
        ചെയ്യുക {
            newCapacity*=2;
        } ആകുമ്പോൾ(newCapacity<=length);
        അക്ഷരം *newBytes=static_cast<അക്ഷരം *>(uprv_malloc(newCapacity));
        എങ്കിൽ(newBytes==nullptr) {
            // unable to allocate memory
            uprv_free(bytes);
            bytes=nullptr;
            bytesCapacity=0;
            തിരികെ അസത്യം;
        }
        uprv_memcpy(newBytes+(newCapacity-bytesLength),
                    bytes+(bytesCapacity-bytesLength), bytesLength);
        uprv_free(bytes);
        bytes=newBytes;
        bytesCapacity=newCapacity;
    }
    തിരികെ സത്യം;
}

int32_t
BytesTrieBuilder::ലിഖ(int32_t byte) {
    int32_t newLength=bytesLength+1;
    എങ്കിൽ(ensureCapacity(newLength)) {
        bytesLength=newLength;
        bytes[bytesCapacity - bytesLength] = static_cast<അക്ഷരം>(byte);
    }
    തിരികെ bytesLength;
}

int32_t
BytesTrieBuilder::ലിഖ(സ്ഥിരം അക്ഷരം *b, int32_t length) {
    int32_t newLength=bytesLength+length;
    എങ്കിൽ(ensureCapacity(newLength)) {
        bytesLength=newLength;
        uprv_memcpy(bytes+(bytesCapacity-bytesLength), b, length);
    }
    തിരികെ bytesLength;
}

int32_t
BytesTrieBuilder::writeElementUnits(int32_t i, int32_t byteIndex, int32_t length) {
    തിരികെ ലിഖ(elements[i].getString(*strings).data()+byteIndex, length);
}

int32_t
BytesTrieBuilder::write_മൂല്യം_And_Final(int32_t i, UBool isFinal) {
    എങ്കിൽ(0<=i && i<=BytesTrie::k_Max_One_Byte_മൂല്യം) {
        തിരികെ ലിഖ(((BytesTrie::k_Min_One_Byte_മൂല്യം_Lead+i)<<1)|isFinal);
    }
    അക്ഷരം intBytes[5];
    int32_t length=1;
    എങ്കിൽ(i<0 || i>0xffffff) {
        intBytes[0] = static_cast<അക്ഷരം>(BytesTrie::k_Five_Byte_മൂല്യം_Lead);
        intBytes[1] = static_cast<അക്ഷരം>(static_cast<uint32_t>(i) >> 24);
        intBytes[2] = static_cast<അക്ഷരം>(static_cast<uint32_t>(i) >> 16);
        intBytes[3] = static_cast<അക്ഷരം>(static_cast<uint32_t>(i) >> 8);
        intBytes[4] = static_cast<അക്ഷരം>(i);
        length=5;
    // } else if(i<=BytesTrie::kMaxOneByteValue) {
    //     intBytes[0]=(char)(BytesTrie::kMinOneByteValueLead+i);
    } അല്ലെങ്കിൽ {
        എങ്കിൽ(i<=BytesTrie::k_Max_Two_Byte_മൂല്യം) {
            intBytes[0] = static_cast<അക്ഷരം>(BytesTrie::k_Min_Two_Byte_മൂല്യം_Lead + (i >> 8));
        } അല്ലെങ്കിൽ {
            എങ്കിൽ(i<=BytesTrie::k_Max_Three_Byte_മൂല്യം) {
                intBytes[0] = static_cast<അക്ഷരം>(BytesTrie::k_Min_Three_Byte_മൂല്യം_Lead + (i >> 16));
            } അല്ലെങ്കിൽ {
                intBytes[0] = static_cast<അക്ഷരം>(BytesTrie::k_Four_Byte_മൂല്യം_Lead);
                intBytes[1] = static_cast<അക്ഷരം>(i >> 16);
                length=2;
            }
            intBytes[length++] = static_cast<അക്ഷരം>(i >> 8);
        }
        intBytes[length++] = static_cast<അക്ഷരം>(i);
    }
    intBytes[0] = static_cast<അക്ഷരം>((intBytes[0] << 1) | isFinal);
    തിരികെ ലിഖ(intBytes, length);
}

int32_t
BytesTrieBuilder::write_മൂല്യം_And_Type(UBool has_മൂല്യം, int32_t value, int32_t കണ്ണി) {
    int32_t offset=ലിഖ(കണ്ണി);
    എങ്കിൽ(has_മൂല്യം) {
        offset=write_മൂല്യം_And_Final(value, അസത്യം);
    }
    തിരികെ offset;
}

int32_t
BytesTrieBuilder::writeDeltaTo(int32_t jumpTarget) {
    int32_t i=bytesLength-jumpTarget;
    U_ASSERT(i>=0);
    എങ്കിൽ(i<=BytesTrie::kMaxOneByteDelta) {
        തിരികെ ലിഖ(i);
    } അല്ലെങ്കിൽ {
        അക്ഷരം intBytes[5];
        തിരികെ ലിഖ(intBytes, internalEncodeDelta(i, intBytes));
    }
}

int32_t
BytesTrieBuilder::internalEncodeDelta(int32_t i, അക്ഷരം intBytes[]) {
    U_ASSERT(i>=0);
    എങ്കിൽ(i<=BytesTrie::kMaxOneByteDelta) {
        intBytes[0] = static_cast<അക്ഷരം>(i);
        തിരികെ 1;
    }
    int32_t length=1;
    എങ്കിൽ(i<=BytesTrie::kMaxTwoByteDelta) {
        intBytes[0] = static_cast<അക്ഷരം>(BytesTrie::kMinTwoByteDeltaLead + (i >> 8));
    } അല്ലെങ്കിൽ {
        എങ്കിൽ(i<=BytesTrie::kMaxThreeByteDelta) {
            intBytes[0] = static_cast<അക്ഷരം>(BytesTrie::kMinThreeByteDeltaLead + (i >> 16));
        } അല്ലെങ്കിൽ {
            എങ്കിൽ(i<=0xffffff) {
                intBytes[0] = static_cast<അക്ഷരം>(BytesTrie::kFourByteDeltaLead);
            } അല്ലെങ്കിൽ {
                intBytes[0] = static_cast<അക്ഷരം>(BytesTrie::kFiveByteDeltaLead);
                intBytes[1] = static_cast<അക്ഷരം>(i >> 24);
                length=2;
            }
            intBytes[length++] = static_cast<അക്ഷരം>(i >> 16);
        }
        intBytes[length++] = static_cast<അക്ഷരം>(i >> 8);
    }
    intBytes[length++] = static_cast<അക്ഷരം>(i);
    തിരികെ length;
}

U_NAMESPACE_END
