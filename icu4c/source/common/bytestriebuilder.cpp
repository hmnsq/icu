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

#अन्तर्भाव "unicode/utypes.h"
#अन्तर्भाव "unicode/bytestrie.h"
#अन्तर्भाव "unicode/bytestriebuilder.h"
#अन्तर्भाव "unicode/stringpiece.h"
#अन्तर्भाव "charstr.h"
#अन्तर्भाव "cmemory.h"
#अन्तर्भाव "uhash.h"
#अन्तर्भाव "uarrsort.h"
#अन्तर्भाव "uassert.h"
#अन्तर्भाव "ustr_imp.h"

U_NAMESPACE_BEGIN

/*
 * Note: This builder implementation stores (bytes, value) pairs with full copies
 * of the byte sequences, until the BytesTrie is built.
 * It might(!) take less memory if we collected the data in a temporary, dynamic trie.
 */

वर्ग BytesTrieElement : सार्वजनिक UMemory {
सार्वजनिक:
    // Use compiler's default constructor, initializes nothing.

    शून्य setTo(StringPiece s, int32_t val, CharString &strings, UErrorCode &errorCode);

    StringPiece getString(अचर CharString& strings U_LIFETIME_BOUND) अचर {
        int32_t offset=stringOffset;
        int32_t length;
        यदि(offset>=0) {
            length = static_cast<uint8_t>(strings[offset++]);
        } अन्यथा {
            offset=~offset;
            length = (static_cast<int32_t>(static_cast<uint8_t>(strings[offset])) << 8) | static_cast<uint8_t>(strings[offset + 1]);
            offset+=2;
        }
        फर्काऊ StringPiece(strings.data()+offset, length);
    }
    int32_t getStringLength(अचर CharString &strings) अचर {
        int32_t offset=stringOffset;
        यदि(offset>=0) {
            फर्काऊ static_cast<uint8_t>(strings[offset]);
        } अन्यथा {
            offset=~offset;
            फर्काऊ (static_cast<int32_t>(static_cast<uint8_t>(strings[offset])) << 8) | static_cast<uint8_t>(strings[offset + 1]);
        }
    }

    वर्ण charAt(int32_t index, अचर CharString &strings) अचर { फर्काऊ data(strings)[index]; }

    int32_t get_मान() अचर { फर्काऊ value; }

    int32_t compareStringTo(अचर BytesTrieElement &o, अचर CharString &strings) अचर;

गुप्त:
    अचर वर्ण* data(अचर CharString& strings U_LIFETIME_BOUND) अचर {
        int32_t offset=stringOffset;
        यदि(offset>=0) {
            ++offset;
        } अन्यथा {
            offset=~offset+2;
        }
        फर्काऊ strings.data()+offset;
    }

    // If the stringOffset is non-negative, then the first strings byte contains
    // the string length.
    // If the stringOffset is negative, then the first two strings bytes contain
    // the string length (big-endian), and the offset needs to be bit-inverted.
    // (Compared with a stringLength field here, this saves 3 bytes per string for most strings.)
    int32_t stringOffset;
    int32_t value;
};

शून्य
BytesTrieElement::setTo(StringPiece s, int32_t val,
                        CharString &strings, UErrorCode &errorCode) {
    यदि(U_FAILURE(errorCode)) {
        फर्काऊ;
    }
    int32_t length=s.length();
    यदि(length>0xffff) {
        // Too long: We store the length in 1 or 2 bytes.
        errorCode=U_INDEX_OUTOFBOUNDS_ERROR;
        फर्काऊ;
    }
    int32_t offset=strings.length();
    यदि(length>0xff) {
        offset=~offset;
        strings.append(static_cast<वर्ण>(length >> 8), errorCode);
    }
    strings.append(static_cast<वर्ण>(length), errorCode);
    stringOffset=offset;
    value=val;
    strings.append(s, errorCode);
}

int32_t
BytesTrieElement::compareStringTo(अचर BytesTrieElement &other, अचर CharString &strings) अचर {
    // TODO: add StringPiece::compare(), see ticket #8187
    StringPiece thisString=getString(strings);
    StringPiece otherString=other.getString(strings);
    int32_t lengthDiff=thisString.length()-otherString.length();
    int32_t commonLength;
    यदि(lengthDiff<=0) {
        commonLength=thisString.length();
    } अन्यथा {
        commonLength=otherString.length();
    }
    int32_t diff=uprv_memcmp(thisString.data(), otherString.data(), commonLength);
    फर्काऊ diff!=0 ? diff : lengthDiff;
}

BytesTrieBuilder::BytesTrieBuilder(UErrorCode &errorCode)
        : strings(nullptr), elements(nullptr), elementsCapacity(0), elementsLength(0),
          bytes(nullptr), bytesCapacity(0), bytesLength(0) {
    यदि(U_FAILURE(errorCode)) {
        फर्काऊ;
    }
    strings=नूतन CharString();
    यदि(strings==nullptr) {
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
    यदि(U_FAILURE(errorCode)) {
        फर्काऊ *एतत्;
    }
    यदि(bytesLength>0) {
        // Cannot add elements after building.
        errorCode=U_NO_WRITE_PERMISSION;
        फर्काऊ *एतत्;
    }
    यदि(elementsLength==elementsCapacity) {
        int32_t newCapacity;
        यदि(elementsCapacity==0) {
            newCapacity=1024;
        } अन्यथा {
            newCapacity=4*elementsCapacity;
        }
        BytesTrieElement *newElements=नूतन BytesTrieElement[newCapacity];
        यदि(newElements==nullptr) {
            errorCode=U_MEMORY_ALLOCATION_ERROR;
            फर्काऊ *एतत्; // error instead of dereferencing null
        }
        यदि(elementsLength>0) {
            uprv_memcpy(newElements, elements, (माप_प्रकार)elementsLength*परिमाण(BytesTrieElement));
        }
        delete[] elements;
        elements=newElements;
        elementsCapacity=newCapacity;
    }
    elements[elementsLength++].setTo(s, value, *strings, errorCode);
    फर्काऊ *एतत्;
}

U_CDECL_BEGIN

स्थिर int32_t U_CALLCONV
compareElementStrings(अचर शून्य *context, अचर शून्य *left, अचर शून्य *दायाँ) {
    अचर CharString *strings=static_cast<अचर CharString *>(context);
    अचर BytesTrieElement *बायाँ_Element=static_cast<अचर BytesTrieElement *>(left);
    अचर BytesTrieElement *दायाँ_Element=static_cast<अचर BytesTrieElement *>(दायाँ);
    फर्काऊ बायाँ_Element->compareStringTo(*दायाँ_Element, *strings);
}

U_CDECL_END

BytesTrie *
BytesTrieBuilder::build(UStringTrieBuildOption buildOption, UErrorCode &errorCode) {
    buildBytes(buildOption, errorCode);
    BytesTrie *newTrie=nullptr;
    यदि(U_SUCCESS(errorCode)) {
        newTrie=नूतन BytesTrie(bytes, bytes+(bytesCapacity-bytesLength));
        यदि(newTrie==nullptr) {
            errorCode=U_MEMORY_ALLOCATION_ERROR;
        } अन्यथा {
            bytes=nullptr;  // The new trie now owns the array.
            bytesCapacity=0;
        }
    }
    फर्काऊ newTrie;
}

StringPiece
BytesTrieBuilder::buildStringPiece(UStringTrieBuildOption buildOption,
                                   UErrorCode& errorCode) U_LIFETIME_BOUND {
    buildBytes(buildOption, errorCode);
    StringPiece result;
    यदि(U_SUCCESS(errorCode)) {
        result.set(bytes+(bytesCapacity-bytesLength), bytesLength);
    }
    फर्काऊ result;
}

शून्य
BytesTrieBuilder::buildBytes(UStringTrieBuildOption buildOption, UErrorCode &errorCode) {
    यदि(U_FAILURE(errorCode)) {
        फर्काऊ;
    }
    यदि(bytes!=nullptr && bytesLength>0) {
        // Already built.
        फर्काऊ;
    }
    यदि(bytesLength==0) {
        यदि(elementsLength==0) {
            errorCode=U_INDEX_OUTOFBOUNDS_ERROR;
            फर्काऊ;
        }
        uprv_sortArray(elements, elementsLength, static_cast<int32_t>(परिमाण(BytesTrieElement)),
                      compareElementStrings, strings,
                      असत्य,  // need not be a stable sort
                      &errorCode);
        यदि(U_FAILURE(errorCode)) {
            फर्काऊ;
        }
        // Duplicate strings are not allowed.
        StringPiece prev=elements[0].getString(*strings);
        लागि(int32_t i=1; i<elementsLength; ++i) {
            StringPiece current=elements[i].getString(*strings);
            यदि(prev==current) {
                errorCode=U_ILLEGAL_ARGUMENT_ERROR;
                फर्काऊ;
            }
            prev=current;
        }
    }
    // Create and byte-serialize the trie for the elements.
    bytesLength=0;
    int32_t capacity=strings->length();
    यदि(capacity<1024) {
        capacity=1024;
    }
    यदि(bytesCapacity<capacity) {
        uprv_free(bytes);
        bytes=static_cast<वर्ण *>(uprv_malloc(capacity));
        यदि(bytes==nullptr) {
            errorCode=U_MEMORY_ALLOCATION_ERROR;
            bytesCapacity=0;
            फर्काऊ;
        }
        bytesCapacity=capacity;
    }
    StringTrieBuilder::build(buildOption, elementsLength, errorCode);
    यदि(bytes==nullptr) {
        errorCode=U_MEMORY_ALLOCATION_ERROR;
    }
}

BytesTrieBuilder&
BytesTrieBuilder::clear() U_LIFETIME_BOUND {
    strings->clear();
    elementsLength=0;
    bytesLength=0;
    फर्काऊ *एतत्;
}

int32_t
BytesTrieBuilder::getElementStringLength(int32_t i) अचर {
    फर्काऊ elements[i].getStringLength(*strings);
}

char16_t
BytesTrieBuilder::getElementUnit(int32_t i, int32_t byteIndex) अचर {
    फर्काऊ static_cast<uint8_t>(elements[i].charAt(byteIndex, *strings));
}

int32_t
BytesTrieBuilder::get_Element_मान(int32_t i) अचर {
    फर्काऊ elements[i].get_मान();
}

int32_t
BytesTrieBuilder::getLimitOfLinearMatch(int32_t first, int32_t last, int32_t byteIndex) अचर {
    अचर BytesTrieElement &firstElement=elements[first];
    अचर BytesTrieElement &lastElement=elements[last];
    int32_t minStringLength=firstElement.getStringLength(*strings);
    जबसम्म(++byteIndex<minStringLength &&
            firstElement.charAt(byteIndex, *strings)==
            lastElement.charAt(byteIndex, *strings)) {}
    फर्काऊ byteIndex;
}

int32_t
BytesTrieBuilder::गन्ती_Element_Units(int32_t start, int32_t limit, int32_t byteIndex) अचर {
    int32_t length=0;  // Number of different bytes at byteIndex.
    int32_t i=start;
    गर {
        वर्ण byte=elements[i++].charAt(byteIndex, *strings);
        जबसम्म(i<limit && byte==elements[i].charAt(byteIndex, *strings)) {
            ++i;
        }
        ++length;
    } जबसम्म(i<limit);
    फर्काऊ length;
}

int32_t
BytesTrieBuilder::skipElementsBySomeUnits(int32_t i, int32_t byteIndex, int32_t count) अचर {
    गर {
        वर्ण byte=elements[i++].charAt(byteIndex, *strings);
        जबसम्म(byte==elements[i].charAt(byteIndex, *strings)) {
            ++i;
        }
    } जबसम्म(--count>0);
    फर्काऊ i;
}

int32_t
BytesTrieBuilder::indexOfElementWithNextUnit(int32_t i, int32_t byteIndex, char16_t byte) अचर {
    वर्ण b = static_cast<वर्ण>(byte);
    जबसम्म(b==elements[i].charAt(byteIndex, *strings)) {
        ++i;
    }
    फर्काऊ i;
}

BytesTrieBuilder::BTLinear_Match_गाँठो::BTLinear_Match_गाँठो(अचर वर्ण *bytes, int32_t len, Node *next_गाँठो)
        : Linear_Match_गाँठो(len, next_गाँठो), s(bytes) {
    hash=static_cast<int32_t>(
        static_cast<uint32_t>(hash)*37u + static_cast<uint32_t>(ustr_hashCharsN(bytes, len)));
}

तार्किक
BytesTrieBuilder::BTLinear_Match_गाँठो::चालक==(अचर Node &other) अचर {
    यदि(एतत्==&other) {
        फर्काऊ सत्य;
    }
    यदि(!Linear_Match_गाँठो::चालक==(other)) {
        फर्काऊ असत्य;
    }
    अचर BTLinear_Match_गाँठो &o=static_cast<अचर BTLinear_Match_गाँठो &>(other);
    फर्काऊ 0==uprv_memcmp(s, o.s, length);
}

शून्य
BytesTrieBuilder::BTLinear_Match_गाँठो::लिख(StringTrieBuilder &builder) {
    BytesTrieBuilder &b=static_cast<BytesTrieBuilder &>(builder);
    next->लिख(builder);
    b.लिख(s, length);
    offset=b.लिख(b.getMinLinearMatch()+length-1);
}

StringTrieBuilder::Node *
BytesTrieBuilder::create_Linear_Match_गाँठो(int32_t i, int32_t byteIndex, int32_t length,
                                        Node *next_गाँठो) अचर {
    फर्काऊ नूतन BTLinear_Match_गाँठो(
            elements[i].getString(*strings).data()+byteIndex,
            length,
            next_गाँठो);
}

UBool
BytesTrieBuilder::ensureCapacity(int32_t length) {
    यदि(bytes==nullptr) {
        फर्काऊ असत्य;  // previous memory allocation had failed
    }
    यदि(length>bytesCapacity) {
        int32_t newCapacity=bytesCapacity;
        गर {
            newCapacity*=2;
        } जबसम्म(newCapacity<=length);
        वर्ण *newBytes=static_cast<वर्ण *>(uprv_malloc(newCapacity));
        यदि(newBytes==nullptr) {
            // unable to allocate memory
            uprv_free(bytes);
            bytes=nullptr;
            bytesCapacity=0;
            फर्काऊ असत्य;
        }
        uprv_memcpy(newBytes+(newCapacity-bytesLength),
                    bytes+(bytesCapacity-bytesLength), bytesLength);
        uprv_free(bytes);
        bytes=newBytes;
        bytesCapacity=newCapacity;
    }
    फर्काऊ सत्य;
}

int32_t
BytesTrieBuilder::लिख(int32_t byte) {
    int32_t newLength=bytesLength+1;
    यदि(ensureCapacity(newLength)) {
        bytesLength=newLength;
        bytes[bytesCapacity - bytesLength] = static_cast<वर्ण>(byte);
    }
    फर्काऊ bytesLength;
}

int32_t
BytesTrieBuilder::लिख(अचर वर्ण *b, int32_t length) {
    int32_t newLength=bytesLength+length;
    यदि(ensureCapacity(newLength)) {
        bytesLength=newLength;
        uprv_memcpy(bytes+(bytesCapacity-bytesLength), b, length);
    }
    फर्काऊ bytesLength;
}

int32_t
BytesTrieBuilder::writeElementUnits(int32_t i, int32_t byteIndex, int32_t length) {
    फर्काऊ लिख(elements[i].getString(*strings).data()+byteIndex, length);
}

int32_t
BytesTrieBuilder::write_मान_And_Final(int32_t i, UBool isFinal) {
    यदि(0<=i && i<=BytesTrie::k_Max_One_Byte_मान) {
        फर्काऊ लिख(((BytesTrie::k_Min_One_Byte_मान_Lead+i)<<1)|isFinal);
    }
    वर्ण intBytes[5];
    int32_t length=1;
    यदि(i<0 || i>0xffffff) {
        intBytes[0] = static_cast<वर्ण>(BytesTrie::k_Five_Byte_मान_Lead);
        intBytes[1] = static_cast<वर्ण>(static_cast<uint32_t>(i) >> 24);
        intBytes[2] = static_cast<वर्ण>(static_cast<uint32_t>(i) >> 16);
        intBytes[3] = static_cast<वर्ण>(static_cast<uint32_t>(i) >> 8);
        intBytes[4] = static_cast<वर्ण>(i);
        length=5;
    // } else if(i<=BytesTrie::kMaxOneByteValue) {
    //     intBytes[0]=(char)(BytesTrie::kMinOneByteValueLead+i);
    } अन्यथा {
        यदि(i<=BytesTrie::k_Max_Two_Byte_मान) {
            intBytes[0] = static_cast<वर्ण>(BytesTrie::k_Min_Two_Byte_मान_Lead + (i >> 8));
        } अन्यथा {
            यदि(i<=BytesTrie::k_Max_Three_Byte_मान) {
                intBytes[0] = static_cast<वर्ण>(BytesTrie::k_Min_Three_Byte_मान_Lead + (i >> 16));
            } अन्यथा {
                intBytes[0] = static_cast<वर्ण>(BytesTrie::k_Four_Byte_मान_Lead);
                intBytes[1] = static_cast<वर्ण>(i >> 16);
                length=2;
            }
            intBytes[length++] = static_cast<वर्ण>(i >> 8);
        }
        intBytes[length++] = static_cast<वर्ण>(i);
    }
    intBytes[0] = static_cast<वर्ण>((intBytes[0] << 1) | isFinal);
    फर्काऊ लिख(intBytes, length);
}

int32_t
BytesTrieBuilder::write_मान_And_Type(UBool has_मान, int32_t value, int32_t गाँठो) {
    int32_t offset=लिख(गाँठो);
    यदि(has_मान) {
        offset=write_मान_And_Final(value, असत्य);
    }
    फर्काऊ offset;
}

int32_t
BytesTrieBuilder::writeDeltaTo(int32_t jumpTarget) {
    int32_t i=bytesLength-jumpTarget;
    U_ASSERT(i>=0);
    यदि(i<=BytesTrie::kMaxOneByteDelta) {
        फर्काऊ लिख(i);
    } अन्यथा {
        वर्ण intBytes[5];
        फर्काऊ लिख(intBytes, internalEncodeDelta(i, intBytes));
    }
}

int32_t
BytesTrieBuilder::internalEncodeDelta(int32_t i, वर्ण intBytes[]) {
    U_ASSERT(i>=0);
    यदि(i<=BytesTrie::kMaxOneByteDelta) {
        intBytes[0] = static_cast<वर्ण>(i);
        फर्काऊ 1;
    }
    int32_t length=1;
    यदि(i<=BytesTrie::kMaxTwoByteDelta) {
        intBytes[0] = static_cast<वर्ण>(BytesTrie::kMinTwoByteDeltaLead + (i >> 8));
    } अन्यथा {
        यदि(i<=BytesTrie::kMaxThreeByteDelta) {
            intBytes[0] = static_cast<वर्ण>(BytesTrie::kMinThreeByteDeltaLead + (i >> 16));
        } अन्यथा {
            यदि(i<=0xffffff) {
                intBytes[0] = static_cast<वर्ण>(BytesTrie::kFourByteDeltaLead);
            } अन्यथा {
                intBytes[0] = static_cast<वर्ण>(BytesTrie::kFiveByteDeltaLead);
                intBytes[1] = static_cast<वर्ण>(i >> 24);
                length=2;
            }
            intBytes[length++] = static_cast<वर्ण>(i >> 16);
        }
        intBytes[length++] = static_cast<वर्ण>(i >> 8);
    }
    intBytes[length++] = static_cast<वर्ण>(i);
    फर्काऊ length;
}

U_NAMESPACE_END
