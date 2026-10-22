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

#ಅನ್ತರ್ಭಾವ "unicode/utypes.h"
#ಅನ್ತರ್ಭಾವ "unicode/bytestrie.h"
#ಅನ್ತರ್ಭಾವ "unicode/bytestriebuilder.h"
#ಅನ್ತರ್ಭಾವ "unicode/stringpiece.h"
#ಅನ್ತರ್ಭಾವ "charstr.h"
#ಅನ್ತರ್ಭಾವ "cmemory.h"
#ಅನ್ತರ್ಭಾವ "uhash.h"
#ಅನ್ತರ್ಭಾವ "uarrsort.h"
#ಅನ್ತರ್ಭಾವ "uassert.h"
#ಅನ್ತರ್ಭಾವ "ustr_imp.h"

U_NAMESPACE_BEGIN

/*
 * Note: This builder implementation stores (bytes, value) pairs with full copies
 * of the byte sequences, until the BytesTrie is built.
 * It might(!) take less memory if we collected the data in a temporary, dynamic trie.
 */

ವರ್ಗ BytesTrieElement : ಸಾರ್ವಜನಿಕ UMemory {
ಸಾರ್ವಜನಿಕ:
    // Use compiler's default constructor, initializes nothing.

    ಶೂನ್ಯ setTo(StringPiece s, int32_t val, CharString &strings, UErrorCode &errorCode);

    StringPiece getString(ಧ್ರುವ CharString& strings U_LIFETIME_BOUND) ಧ್ರುವ {
        int32_t offset=stringOffset;
        int32_t length;
        ಯದಿ(offset>=0) {
            length = static_cast<uint8_t>(strings[offset++]);
        } ಅನ್ಯಥಾ {
            offset=~offset;
            length = (static_cast<int32_t>(static_cast<uint8_t>(strings[offset])) << 8) | static_cast<uint8_t>(strings[offset + 1]);
            offset+=2;
        }
        ಪ್ರತ್ಯಾಗಮ StringPiece(strings.data()+offset, length);
    }
    int32_t getStringLength(ಧ್ರುವ CharString &strings) ಧ್ರುವ {
        int32_t offset=stringOffset;
        ಯದಿ(offset>=0) {
            ಪ್ರತ್ಯಾಗಮ static_cast<uint8_t>(strings[offset]);
        } ಅನ್ಯಥಾ {
            offset=~offset;
            ಪ್ರತ್ಯಾಗಮ (static_cast<int32_t>(static_cast<uint8_t>(strings[offset])) << 8) | static_cast<uint8_t>(strings[offset + 1]);
        }
    }

    ವರ್ಣ charAt(int32_t index, ಧ್ರುವ CharString &strings) ಧ್ರುವ { ಪ್ರತ್ಯಾಗಮ data(strings)[index]; }

    int32_t get_ಮೌಲ್ಯ() ಧ್ರುವ { ಪ್ರತ್ಯಾಗಮ value; }

    int32_t compareStringTo(ಧ್ರುವ BytesTrieElement &o, ಧ್ರುವ CharString &strings) ಧ್ರುವ;

ಗುಪ್ತ:
    ಧ್ರುವ ವರ್ಣ* data(ಧ್ರುವ CharString& strings U_LIFETIME_BOUND) ಧ್ರುವ {
        int32_t offset=stringOffset;
        ಯದಿ(offset>=0) {
            ++offset;
        } ಅನ್ಯಥಾ {
            offset=~offset+2;
        }
        ಪ್ರತ್ಯಾಗಮ strings.data()+offset;
    }

    // If the stringOffset is non-negative, then the first strings byte contains
    // the string length.
    // If the stringOffset is negative, then the first two strings bytes contain
    // the string length (big-endian), and the offset needs to be bit-inverted.
    // (Compared with a stringLength field here, this saves 3 bytes per string for most strings.)
    int32_t stringOffset;
    int32_t value;
};

ಶೂನ್ಯ
BytesTrieElement::setTo(StringPiece s, int32_t val,
                        CharString &strings, UErrorCode &errorCode) {
    ಯದಿ(U_FAILURE(errorCode)) {
        ಪ್ರತ್ಯಾಗಮ;
    }
    int32_t length=s.length();
    ಯದಿ(length>0xffff) {
        // Too long: We store the length in 1 or 2 bytes.
        errorCode=U_INDEX_OUTOFBOUNDS_ERROR;
        ಪ್ರತ್ಯಾಗಮ;
    }
    int32_t offset=strings.length();
    ಯದಿ(length>0xff) {
        offset=~offset;
        strings.append(static_cast<ವರ್ಣ>(length >> 8), errorCode);
    }
    strings.append(static_cast<ವರ್ಣ>(length), errorCode);
    stringOffset=offset;
    value=val;
    strings.append(s, errorCode);
}

int32_t
BytesTrieElement::compareStringTo(ಧ್ರುವ BytesTrieElement &other, ಧ್ರುವ CharString &strings) ಧ್ರುವ {
    // TODO: add StringPiece::compare(), see ticket #8187
    StringPiece thisString=getString(strings);
    StringPiece otherString=other.getString(strings);
    int32_t lengthDiff=thisString.length()-otherString.length();
    int32_t commonLength;
    ಯದಿ(lengthDiff<=0) {
        commonLength=thisString.length();
    } ಅನ್ಯಥಾ {
        commonLength=otherString.length();
    }
    int32_t diff=uprv_memcmp(thisString.data(), otherString.data(), commonLength);
    ಪ್ರತ್ಯಾಗಮ diff!=0 ? diff : lengthDiff;
}

BytesTrieBuilder::BytesTrieBuilder(UErrorCode &errorCode)
        : strings(nullptr), elements(nullptr), elementsCapacity(0), elementsLength(0),
          bytes(nullptr), bytesCapacity(0), bytesLength(0) {
    ಯದಿ(U_FAILURE(errorCode)) {
        ಪ್ರತ್ಯಾಗಮ;
    }
    strings=ನೂತನ CharString();
    ಯದಿ(strings==nullptr) {
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
    ಯದಿ(U_FAILURE(errorCode)) {
        ಪ್ರತ್ಯಾಗಮ *ಏತತ್;
    }
    ಯದಿ(bytesLength>0) {
        // Cannot add elements after building.
        errorCode=U_NO_WRITE_PERMISSION;
        ಪ್ರತ್ಯಾಗಮ *ಏತತ್;
    }
    ಯದಿ(elementsLength==elementsCapacity) {
        int32_t newCapacity;
        ಯದಿ(elementsCapacity==0) {
            newCapacity=1024;
        } ಅನ್ಯಥಾ {
            newCapacity=4*elementsCapacity;
        }
        BytesTrieElement *newElements=ನೂತನ BytesTrieElement[newCapacity];
        ಯದಿ(newElements==nullptr) {
            errorCode=U_MEMORY_ALLOCATION_ERROR;
            ಪ್ರತ್ಯಾಗಮ *ಏತತ್; // error instead of dereferencing null
        }
        ಯದಿ(elementsLength>0) {
            uprv_memcpy(newElements, elements, (size_t)elementsLength*ಪರಿಮಾಣ(BytesTrieElement));
        }
        delete[] elements;
        elements=newElements;
        elementsCapacity=newCapacity;
    }
    elements[elementsLength++].setTo(s, value, *strings, errorCode);
    ಪ್ರತ್ಯಾಗಮ *ಏತತ್;
}

U_CDECL_BEGIN

ಸ್ಥಿರ int32_t U_CALLCONV
compareElementStrings(ಧ್ರುವ ಶೂನ್ಯ *context, ಧ್ರುವ ಶೂನ್ಯ *left, ಧ್ರುವ ಶೂನ್ಯ *ಬಲ) {
    ಧ್ರುವ CharString *strings=static_cast<ಧ್ರುವ CharString *>(context);
    ಧ್ರುವ BytesTrieElement *ಎಡ_Element=static_cast<ಧ್ರುವ BytesTrieElement *>(left);
    ಧ್ರುವ BytesTrieElement *ಬಲ_Element=static_cast<ಧ್ರುವ BytesTrieElement *>(ಬಲ);
    ಪ್ರತ್ಯಾಗಮ ಎಡ_Element->compareStringTo(*ಬಲ_Element, *strings);
}

U_CDECL_END

BytesTrie *
BytesTrieBuilder::build(UStringTrieBuildOption buildOption, UErrorCode &errorCode) {
    buildBytes(buildOption, errorCode);
    BytesTrie *newTrie=nullptr;
    ಯದಿ(U_SUCCESS(errorCode)) {
        newTrie=ನೂತನ BytesTrie(bytes, bytes+(bytesCapacity-bytesLength));
        ಯದಿ(newTrie==nullptr) {
            errorCode=U_MEMORY_ALLOCATION_ERROR;
        } ಅನ್ಯಥಾ {
            bytes=nullptr;  // The new trie now owns the array.
            bytesCapacity=0;
        }
    }
    ಪ್ರತ್ಯಾಗಮ newTrie;
}

StringPiece
BytesTrieBuilder::buildStringPiece(UStringTrieBuildOption buildOption,
                                   UErrorCode& errorCode) U_LIFETIME_BOUND {
    buildBytes(buildOption, errorCode);
    StringPiece result;
    ಯದಿ(U_SUCCESS(errorCode)) {
        result.set(bytes+(bytesCapacity-bytesLength), bytesLength);
    }
    ಪ್ರತ್ಯಾಗಮ result;
}

ಶೂನ್ಯ
BytesTrieBuilder::buildBytes(UStringTrieBuildOption buildOption, UErrorCode &errorCode) {
    ಯದಿ(U_FAILURE(errorCode)) {
        ಪ್ರತ್ಯಾಗಮ;
    }
    ಯದಿ(bytes!=nullptr && bytesLength>0) {
        // Already built.
        ಪ್ರತ್ಯಾಗಮ;
    }
    ಯದಿ(bytesLength==0) {
        ಯದಿ(elementsLength==0) {
            errorCode=U_INDEX_OUTOFBOUNDS_ERROR;
            ಪ್ರತ್ಯಾಗಮ;
        }
        uprv_sortArray(elements, elementsLength, static_cast<int32_t>(ಪರಿಮಾಣ(BytesTrieElement)),
                      compareElementStrings, strings,
                      ಅಸತ್ಯ,  // need not be a stable sort
                      &errorCode);
        ಯದಿ(U_FAILURE(errorCode)) {
            ಪ್ರತ್ಯಾಗಮ;
        }
        // Duplicate strings are not allowed.
        StringPiece prev=elements[0].getString(*strings);
        ಪ್ರತಿ(int32_t i=1; i<elementsLength; ++i) {
            StringPiece current=elements[i].getString(*strings);
            ಯದಿ(prev==current) {
                errorCode=U_ILLEGAL_ARGUMENT_ERROR;
                ಪ್ರತ್ಯಾಗಮ;
            }
            prev=current;
        }
    }
    // Create and byte-serialize the trie for the elements.
    bytesLength=0;
    int32_t capacity=strings->length();
    ಯದಿ(capacity<1024) {
        capacity=1024;
    }
    ಯದಿ(bytesCapacity<capacity) {
        uprv_free(bytes);
        bytes=static_cast<ವರ್ಣ *>(uprv_malloc(capacity));
        ಯದಿ(bytes==nullptr) {
            errorCode=U_MEMORY_ALLOCATION_ERROR;
            bytesCapacity=0;
            ಪ್ರತ್ಯಾಗಮ;
        }
        bytesCapacity=capacity;
    }
    StringTrieBuilder::build(buildOption, elementsLength, errorCode);
    ಯದಿ(bytes==nullptr) {
        errorCode=U_MEMORY_ALLOCATION_ERROR;
    }
}

BytesTrieBuilder&
BytesTrieBuilder::clear() U_LIFETIME_BOUND {
    strings->clear();
    elementsLength=0;
    bytesLength=0;
    ಪ್ರತ್ಯಾಗಮ *ಏತತ್;
}

int32_t
BytesTrieBuilder::getElementStringLength(int32_t i) ಧ್ರುವ {
    ಪ್ರತ್ಯಾಗಮ elements[i].getStringLength(*strings);
}

char16_t
BytesTrieBuilder::getElementUnit(int32_t i, int32_t byteIndex) ಧ್ರುವ {
    ಪ್ರತ್ಯಾಗಮ static_cast<uint8_t>(elements[i].charAt(byteIndex, *strings));
}

int32_t
BytesTrieBuilder::get_Element_ಮೌಲ್ಯ(int32_t i) ಧ್ರುವ {
    ಪ್ರತ್ಯಾಗಮ elements[i].get_ಮೌಲ್ಯ();
}

int32_t
BytesTrieBuilder::getLimitOfLinearMatch(int32_t first, int32_t last, int32_t byteIndex) ಧ್ರುವ {
    ಧ್ರುವ BytesTrieElement &firstElement=elements[first];
    ಧ್ರುವ BytesTrieElement &lastElement=elements[last];
    int32_t minStringLength=firstElement.getStringLength(*strings);
    ಯಾವತ್(++byteIndex<minStringLength &&
            firstElement.charAt(byteIndex, *strings)==
            lastElement.charAt(byteIndex, *strings)) {}
    ಪ್ರತ್ಯಾಗಮ byteIndex;
}

int32_t
BytesTrieBuilder::ಎಣಿಕೆ_Element_Units(int32_t start, int32_t limit, int32_t byteIndex) ಧ್ರುವ {
    int32_t length=0;  // Number of different bytes at byteIndex.
    int32_t i=start;
    ಕುರು {
        ವರ್ಣ byte=elements[i++].charAt(byteIndex, *strings);
        ಯಾವತ್(i<limit && byte==elements[i].charAt(byteIndex, *strings)) {
            ++i;
        }
        ++length;
    } ಯಾವತ್(i<limit);
    ಪ್ರತ್ಯಾಗಮ length;
}

int32_t
BytesTrieBuilder::skipElementsBySomeUnits(int32_t i, int32_t byteIndex, int32_t count) ಧ್ರುವ {
    ಕುರು {
        ವರ್ಣ byte=elements[i++].charAt(byteIndex, *strings);
        ಯಾವತ್(byte==elements[i].charAt(byteIndex, *strings)) {
            ++i;
        }
    } ಯಾವತ್(--count>0);
    ಪ್ರತ್ಯಾಗಮ i;
}

int32_t
BytesTrieBuilder::indexOfElementWithNextUnit(int32_t i, int32_t byteIndex, char16_t byte) ಧ್ರುವ {
    ವರ್ಣ b = static_cast<ವರ್ಣ>(byte);
    ಯಾವತ್(b==elements[i].charAt(byteIndex, *strings)) {
        ++i;
    }
    ಪ್ರತ್ಯಾಗಮ i;
}

BytesTrieBuilder::BTLinear_Match_ಗಂಟು::BTLinear_Match_ಗಂಟು(ಧ್ರುವ ವರ್ಣ *bytes, int32_t len, Node *next_ಗಂಟು)
        : Linear_Match_ಗಂಟು(len, next_ಗಂಟು), s(bytes) {
    hash=static_cast<int32_t>(
        static_cast<uint32_t>(hash)*37u + static_cast<uint32_t>(ustr_hashCharsN(bytes, len)));
}

ತಾರ್ಕಿಕ
BytesTrieBuilder::BTLinear_Match_ಗಂಟು::operator==(ಧ್ರುವ Node &other) ಧ್ರುವ {
    ಯದಿ(ಏತತ್==&other) {
        ಪ್ರತ್ಯಾಗಮ ಸತ್ಯ;
    }
    ಯದಿ(!Linear_Match_ಗಂಟು::operator==(other)) {
        ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;
    }
    ಧ್ರುವ BTLinear_Match_ಗಂಟು &o=static_cast<ಧ್ರುವ BTLinear_Match_ಗಂಟು &>(other);
    ಪ್ರತ್ಯಾಗಮ 0==uprv_memcmp(s, o.s, length);
}

ಶೂನ್ಯ
BytesTrieBuilder::BTLinear_Match_ಗಂಟು::ಲಿಖ(StringTrieBuilder &builder) {
    BytesTrieBuilder &b=static_cast<BytesTrieBuilder &>(builder);
    next->ಲಿಖ(builder);
    b.ಲಿಖ(s, length);
    offset=b.ಲಿಖ(b.getMinLinearMatch()+length-1);
}

StringTrieBuilder::Node *
BytesTrieBuilder::create_Linear_Match_ಗಂಟು(int32_t i, int32_t byteIndex, int32_t length,
                                        Node *next_ಗಂಟು) ಧ್ರುವ {
    ಪ್ರತ್ಯಾಗಮ ನೂತನ BTLinear_Match_ಗಂಟು(
            elements[i].getString(*strings).data()+byteIndex,
            length,
            next_ಗಂಟು);
}

UBool
BytesTrieBuilder::ensureCapacity(int32_t length) {
    ಯದಿ(bytes==nullptr) {
        ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;  // previous memory allocation had failed
    }
    ಯದಿ(length>bytesCapacity) {
        int32_t newCapacity=bytesCapacity;
        ಕುರು {
            newCapacity*=2;
        } ಯಾವತ್(newCapacity<=length);
        ವರ್ಣ *newBytes=static_cast<ವರ್ಣ *>(uprv_malloc(newCapacity));
        ಯದಿ(newBytes==nullptr) {
            // unable to allocate memory
            uprv_free(bytes);
            bytes=nullptr;
            bytesCapacity=0;
            ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;
        }
        uprv_memcpy(newBytes+(newCapacity-bytesLength),
                    bytes+(bytesCapacity-bytesLength), bytesLength);
        uprv_free(bytes);
        bytes=newBytes;
        bytesCapacity=newCapacity;
    }
    ಪ್ರತ್ಯಾಗಮ ಸತ್ಯ;
}

int32_t
BytesTrieBuilder::ಲಿಖ(int32_t byte) {
    int32_t newLength=bytesLength+1;
    ಯದಿ(ensureCapacity(newLength)) {
        bytesLength=newLength;
        bytes[bytesCapacity - bytesLength] = static_cast<ವರ್ಣ>(byte);
    }
    ಪ್ರತ್ಯಾಗಮ bytesLength;
}

int32_t
BytesTrieBuilder::ಲಿಖ(ಧ್ರುವ ವರ್ಣ *b, int32_t length) {
    int32_t newLength=bytesLength+length;
    ಯದಿ(ensureCapacity(newLength)) {
        bytesLength=newLength;
        uprv_memcpy(bytes+(bytesCapacity-bytesLength), b, length);
    }
    ಪ್ರತ್ಯಾಗಮ bytesLength;
}

int32_t
BytesTrieBuilder::writeElementUnits(int32_t i, int32_t byteIndex, int32_t length) {
    ಪ್ರತ್ಯಾಗಮ ಲಿಖ(elements[i].getString(*strings).data()+byteIndex, length);
}

int32_t
BytesTrieBuilder::write_ಮೌಲ್ಯ_And_Final(int32_t i, UBool isFinal) {
    ಯದಿ(0<=i && i<=BytesTrie::k_Max_One_Byte_ಮೌಲ್ಯ) {
        ಪ್ರತ್ಯಾಗಮ ಲಿಖ(((BytesTrie::k_Min_One_Byte_ಮೌಲ್ಯ_Lead+i)<<1)|isFinal);
    }
    ವರ್ಣ intBytes[5];
    int32_t length=1;
    ಯದಿ(i<0 || i>0xffffff) {
        intBytes[0] = static_cast<ವರ್ಣ>(BytesTrie::k_Five_Byte_ಮೌಲ್ಯ_Lead);
        intBytes[1] = static_cast<ವರ್ಣ>(static_cast<uint32_t>(i) >> 24);
        intBytes[2] = static_cast<ವರ್ಣ>(static_cast<uint32_t>(i) >> 16);
        intBytes[3] = static_cast<ವರ್ಣ>(static_cast<uint32_t>(i) >> 8);
        intBytes[4] = static_cast<ವರ್ಣ>(i);
        length=5;
    // } else if(i<=BytesTrie::kMaxOneByteValue) {
    //     intBytes[0]=(char)(BytesTrie::kMinOneByteValueLead+i);
    } ಅನ್ಯಥಾ {
        ಯದಿ(i<=BytesTrie::k_Max_Two_Byte_ಮೌಲ್ಯ) {
            intBytes[0] = static_cast<ವರ್ಣ>(BytesTrie::k_Min_Two_Byte_ಮೌಲ್ಯ_Lead + (i >> 8));
        } ಅನ್ಯಥಾ {
            ಯದಿ(i<=BytesTrie::k_Max_Three_Byte_ಮೌಲ್ಯ) {
                intBytes[0] = static_cast<ವರ್ಣ>(BytesTrie::k_Min_Three_Byte_ಮೌಲ್ಯ_Lead + (i >> 16));
            } ಅನ್ಯಥಾ {
                intBytes[0] = static_cast<ವರ್ಣ>(BytesTrie::k_Four_Byte_ಮೌಲ್ಯ_Lead);
                intBytes[1] = static_cast<ವರ್ಣ>(i >> 16);
                length=2;
            }
            intBytes[length++] = static_cast<ವರ್ಣ>(i >> 8);
        }
        intBytes[length++] = static_cast<ವರ್ಣ>(i);
    }
    intBytes[0] = static_cast<ವರ್ಣ>((intBytes[0] << 1) | isFinal);
    ಪ್ರತ್ಯಾಗಮ ಲಿಖ(intBytes, length);
}

int32_t
BytesTrieBuilder::write_ಮೌಲ್ಯ_And_Type(UBool has_ಮೌಲ್ಯ, int32_t value, int32_t ಗಂಟು) {
    int32_t offset=ಲಿಖ(ಗಂಟು);
    ಯದಿ(has_ಮೌಲ್ಯ) {
        offset=write_ಮೌಲ್ಯ_And_Final(value, ಅಸತ್ಯ);
    }
    ಪ್ರತ್ಯಾಗಮ offset;
}

int32_t
BytesTrieBuilder::writeDeltaTo(int32_t jumpTarget) {
    int32_t i=bytesLength-jumpTarget;
    U_ASSERT(i>=0);
    ಯದಿ(i<=BytesTrie::kMaxOneByteDelta) {
        ಪ್ರತ್ಯಾಗಮ ಲಿಖ(i);
    } ಅನ್ಯಥಾ {
        ವರ್ಣ intBytes[5];
        ಪ್ರತ್ಯಾಗಮ ಲಿಖ(intBytes, internalEncodeDelta(i, intBytes));
    }
}

int32_t
BytesTrieBuilder::internalEncodeDelta(int32_t i, ವರ್ಣ intBytes[]) {
    U_ASSERT(i>=0);
    ಯದಿ(i<=BytesTrie::kMaxOneByteDelta) {
        intBytes[0] = static_cast<ವರ್ಣ>(i);
        ಪ್ರತ್ಯಾಗಮ 1;
    }
    int32_t length=1;
    ಯದಿ(i<=BytesTrie::kMaxTwoByteDelta) {
        intBytes[0] = static_cast<ವರ್ಣ>(BytesTrie::kMinTwoByteDeltaLead + (i >> 8));
    } ಅನ್ಯಥಾ {
        ಯದಿ(i<=BytesTrie::kMaxThreeByteDelta) {
            intBytes[0] = static_cast<ವರ್ಣ>(BytesTrie::kMinThreeByteDeltaLead + (i >> 16));
        } ಅನ್ಯಥಾ {
            ಯದಿ(i<=0xffffff) {
                intBytes[0] = static_cast<ವರ್ಣ>(BytesTrie::kFourByteDeltaLead);
            } ಅನ್ಯಥಾ {
                intBytes[0] = static_cast<ವರ್ಣ>(BytesTrie::kFiveByteDeltaLead);
                intBytes[1] = static_cast<ವರ್ಣ>(i >> 24);
                length=2;
            }
            intBytes[length++] = static_cast<ವರ್ಣ>(i >> 16);
        }
        intBytes[length++] = static_cast<ವರ್ಣ>(i >> 8);
    }
    intBytes[length++] = static_cast<ವರ್ಣ>(i);
    ಪ್ರತ್ಯಾಗಮ length;
}

U_NAMESPACE_END
