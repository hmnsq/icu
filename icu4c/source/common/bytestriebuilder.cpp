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

वग्ग BytesTrieElement : सार्वजनिक UMemory {
सार्वजनिक:
    // Use compiler's default constructor, initializes nothing.

    शून्य setTo(StringPiece s, int32_t val, CharString &strings, UErrorCode &errorCode);

    StringPiece getString(ध्रुव CharString& strings U_LIFETIME_BOUND) ध्रुव {
        int32_t offset=stringOffset;
        int32_t length;
        सचे(offset>=0) {
            length = static_cast<uint8_t>(strings[offset++]);
        } अञ्ञथा {
            offset=~offset;
            length = (static_cast<int32_t>(static_cast<uint8_t>(strings[offset])) << 8) | static_cast<uint8_t>(strings[offset + 1]);
            offset+=2;
        }
        पटिनिवत्त StringPiece(strings.data()+offset, length);
    }
    int32_t getStringLength(ध्रुव CharString &strings) ध्रुव {
        int32_t offset=stringOffset;
        सचे(offset>=0) {
            पटिनिवत्त static_cast<uint8_t>(strings[offset]);
        } अञ्ञथा {
            offset=~offset;
            पटिनिवत्त (static_cast<int32_t>(static_cast<uint8_t>(strings[offset])) << 8) | static_cast<uint8_t>(strings[offset + 1]);
        }
    }

    वर्ण charAt(int32_t index, ध्रुव CharString &strings) ध्रुव { पटिनिवत्त data(strings)[index]; }

    int32_t get_मानम्() ध्रुव { पटिनिवत्त value; }

    int32_t compareStringTo(ध्रुव BytesTrieElement &o, ध्रुव CharString &strings) ध्रुव;

गुप्त:
    ध्रुव वर्ण* data(ध्रुव CharString& strings U_LIFETIME_BOUND) ध्रुव {
        int32_t offset=stringOffset;
        सचे(offset>=0) {
            ++offset;
        } अञ्ञथा {
            offset=~offset+2;
        }
        पटिनिवत्त strings.data()+offset;
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
    सचे(U_FAILURE(errorCode)) {
        पटिनिवत्त;
    }
    int32_t length=s.length();
    सचे(length>0xffff) {
        // Too long: We store the length in 1 or 2 bytes.
        errorCode=U_INDEX_OUTOFBOUNDS_ERROR;
        पटिनिवत्त;
    }
    int32_t offset=strings.length();
    सचे(length>0xff) {
        offset=~offset;
        strings.append(static_cast<वर्ण>(length >> 8), errorCode);
    }
    strings.append(static_cast<वर्ण>(length), errorCode);
    stringOffset=offset;
    value=val;
    strings.append(s, errorCode);
}

int32_t
BytesTrieElement::compareStringTo(ध्रुव BytesTrieElement &other, ध्रुव CharString &strings) ध्रुव {
    // TODO: add StringPiece::compare(), see ticket #8187
    StringPiece thisString=getString(strings);
    StringPiece otherString=other.getString(strings);
    int32_t lengthDiff=thisString.length()-otherString.length();
    int32_t commonLength;
    सचे(lengthDiff<=0) {
        commonLength=thisString.length();
    } अञ्ञथा {
        commonLength=otherString.length();
    }
    int32_t diff=uprv_memcmp(thisString.data(), otherString.data(), commonLength);
    पटिनिवत्त diff!=0 ? diff : lengthDiff;
}

BytesTrieBuilder::BytesTrieBuilder(UErrorCode &errorCode)
        : strings(nullptr), elements(nullptr), elementsCapacity(0), elementsLength(0),
          bytes(nullptr), bytesCapacity(0), bytesLength(0) {
    सचे(U_FAILURE(errorCode)) {
        पटिनिवत्त;
    }
    strings=नूतन CharString();
    सचे(strings==nullptr) {
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
    सचे(U_FAILURE(errorCode)) {
        पटिनिवत्त *एतत्;
    }
    सचे(bytesLength>0) {
        // Cannot add elements after building.
        errorCode=U_NO_WRITE_PERMISSION;
        पटिनिवत्त *एतत्;
    }
    सचे(elementsLength==elementsCapacity) {
        int32_t newCapacity;
        सचे(elementsCapacity==0) {
            newCapacity=1024;
        } अञ्ञथा {
            newCapacity=4*elementsCapacity;
        }
        BytesTrieElement *newElements=नूतन BytesTrieElement[newCapacity];
        सचे(newElements==nullptr) {
            errorCode=U_MEMORY_ALLOCATION_ERROR;
            पटिनिवत्त *एतत्; // error instead of dereferencing null
        }
        सचे(elementsLength>0) {
            uprv_memcpy(newElements, elements, (size_t)elementsLength*परिमाण(BytesTrieElement));
        }
        delete[] elements;
        elements=newElements;
        elementsCapacity=newCapacity;
    }
    elements[elementsLength++].setTo(s, value, *strings, errorCode);
    पटिनिवत्त *एतत्;
}

U_CDECL_BEGIN

स्थिर int32_t U_CALLCONV
compareElementStrings(ध्रुव शून्य *context, ध्रुव शून्य *left, ध्रुव शून्य *दक्षिणः) {
    ध्रुव CharString *strings=static_cast<ध्रुव CharString *>(context);
    ध्रुव BytesTrieElement *वामः_Element=static_cast<ध्रुव BytesTrieElement *>(left);
    ध्रुव BytesTrieElement *दक्षिणः_Element=static_cast<ध्रुव BytesTrieElement *>(दक्षिणः);
    पटिनिवत्त वामः_Element->compareStringTo(*दक्षिणः_Element, *strings);
}

U_CDECL_END

BytesTrie *
BytesTrieBuilder::build(UStringTrieBuildOption buildOption, UErrorCode &errorCode) {
    buildBytes(buildOption, errorCode);
    BytesTrie *newTrie=nullptr;
    सचे(U_SUCCESS(errorCode)) {
        newTrie=नूतन BytesTrie(bytes, bytes+(bytesCapacity-bytesLength));
        सचे(newTrie==nullptr) {
            errorCode=U_MEMORY_ALLOCATION_ERROR;
        } अञ्ञथा {
            bytes=nullptr;  // The new trie now owns the array.
            bytesCapacity=0;
        }
    }
    पटिनिवत्त newTrie;
}

StringPiece
BytesTrieBuilder::buildStringPiece(UStringTrieBuildOption buildOption,
                                   UErrorCode& errorCode) U_LIFETIME_BOUND {
    buildBytes(buildOption, errorCode);
    StringPiece result;
    सचे(U_SUCCESS(errorCode)) {
        result.set(bytes+(bytesCapacity-bytesLength), bytesLength);
    }
    पटिनिवत्त result;
}

शून्य
BytesTrieBuilder::buildBytes(UStringTrieBuildOption buildOption, UErrorCode &errorCode) {
    सचे(U_FAILURE(errorCode)) {
        पटिनिवत्त;
    }
    सचे(bytes!=nullptr && bytesLength>0) {
        // Already built.
        पटिनिवत्त;
    }
    सचे(bytesLength==0) {
        सचे(elementsLength==0) {
            errorCode=U_INDEX_OUTOFBOUNDS_ERROR;
            पटिनिवत्त;
        }
        uprv_sortArray(elements, elementsLength, static_cast<int32_t>(परिमाण(BytesTrieElement)),
                      compareElementStrings, strings,
                      मिच्छा,  // need not be a stable sort
                      &errorCode);
        सचे(U_FAILURE(errorCode)) {
            पटिनिवत्त;
        }
        // Duplicate strings are not allowed.
        StringPiece prev=elements[0].getString(*strings);
        अत्थाय(int32_t i=1; i<elementsLength; ++i) {
            StringPiece current=elements[i].getString(*strings);
            सचे(prev==current) {
                errorCode=U_ILLEGAL_ARGUMENT_ERROR;
                पटिनिवत्त;
            }
            prev=current;
        }
    }
    // Create and byte-serialize the trie for the elements.
    bytesLength=0;
    int32_t capacity=strings->length();
    सचे(capacity<1024) {
        capacity=1024;
    }
    सचे(bytesCapacity<capacity) {
        uprv_free(bytes);
        bytes=static_cast<वर्ण *>(uprv_malloc(capacity));
        सचे(bytes==nullptr) {
            errorCode=U_MEMORY_ALLOCATION_ERROR;
            bytesCapacity=0;
            पटिनिवत्त;
        }
        bytesCapacity=capacity;
    }
    StringTrieBuilder::build(buildOption, elementsLength, errorCode);
    सचे(bytes==nullptr) {
        errorCode=U_MEMORY_ALLOCATION_ERROR;
    }
}

BytesTrieBuilder&
BytesTrieBuilder::clear() U_LIFETIME_BOUND {
    strings->clear();
    elementsLength=0;
    bytesLength=0;
    पटिनिवत्त *एतत्;
}

int32_t
BytesTrieBuilder::getElementStringLength(int32_t i) ध्रुव {
    पटिनिवत्त elements[i].getStringLength(*strings);
}

char16_t
BytesTrieBuilder::getElementUnit(int32_t i, int32_t byteIndex) ध्रुव {
    पटिनिवत्त static_cast<uint8_t>(elements[i].charAt(byteIndex, *strings));
}

int32_t
BytesTrieBuilder::get_Element_मानम्(int32_t i) ध्रुव {
    पटिनिवत्त elements[i].get_मानम्();
}

int32_t
BytesTrieBuilder::getLimitOfLinearMatch(int32_t first, int32_t last, int32_t byteIndex) ध्रुव {
    ध्रुव BytesTrieElement &firstElement=elements[first];
    ध्रुव BytesTrieElement &lastElement=elements[last];
    int32_t minStringLength=firstElement.getStringLength(*strings);
    याव(++byteIndex<minStringLength &&
            firstElement.charAt(byteIndex, *strings)==
            lastElement.charAt(byteIndex, *strings)) {}
    पटिनिवत्त byteIndex;
}

int32_t
BytesTrieBuilder::गणनम्_Element_Units(int32_t start, int32_t limit, int32_t byteIndex) ध्रुव {
    int32_t length=0;  // Number of different bytes at byteIndex.
    int32_t i=start;
    करोहि {
        वर्ण byte=elements[i++].charAt(byteIndex, *strings);
        याव(i<limit && byte==elements[i].charAt(byteIndex, *strings)) {
            ++i;
        }
        ++length;
    } याव(i<limit);
    पटिनिवत्त length;
}

int32_t
BytesTrieBuilder::skipElementsBySomeUnits(int32_t i, int32_t byteIndex, int32_t count) ध्रुव {
    करोहि {
        वर्ण byte=elements[i++].charAt(byteIndex, *strings);
        याव(byte==elements[i].charAt(byteIndex, *strings)) {
            ++i;
        }
    } याव(--count>0);
    पटिनिवत्त i;
}

int32_t
BytesTrieBuilder::indexOfElementWithNextUnit(int32_t i, int32_t byteIndex, char16_t byte) ध्रुव {
    वर्ण b = static_cast<वर्ण>(byte);
    याव(b==elements[i].charAt(byteIndex, *strings)) {
        ++i;
    }
    पटिनिवत्त i;
}

BytesTrieBuilder::BTLinear_Match_ग्रन्थिः::BTLinear_Match_ग्रन्थिः(ध्रुव वर्ण *bytes, int32_t len, Node *next_ग्रन्थिः)
        : Linear_Match_ग्रन्थिः(len, next_ग्रन्थिः), s(bytes) {
    hash=static_cast<int32_t>(
        static_cast<uint32_t>(hash)*37u + static_cast<uint32_t>(ustr_hashCharsN(bytes, len)));
}

तार्किक
BytesTrieBuilder::BTLinear_Match_ग्रन्थिः::operator==(ध्रुव Node &other) ध्रुव {
    सचे(एतत्==&other) {
        पटिनिवत्त सच्च;
    }
    सचे(!Linear_Match_ग्रन्थिः::operator==(other)) {
        पटिनिवत्त मिच्छा;
    }
    ध्रुव BTLinear_Match_ग्रन्थिः &o=static_cast<ध्रुव BTLinear_Match_ग्रन्थिः &>(other);
    पटिनिवत्त 0==uprv_memcmp(s, o.s, length);
}

शून्य
BytesTrieBuilder::BTLinear_Match_ग्रन्थिः::लिख(StringTrieBuilder &builder) {
    BytesTrieBuilder &b=static_cast<BytesTrieBuilder &>(builder);
    next->लिख(builder);
    b.लिख(s, length);
    offset=b.लिख(b.getMinLinearMatch()+length-1);
}

StringTrieBuilder::Node *
BytesTrieBuilder::create_Linear_Match_ग्रन्थिः(int32_t i, int32_t byteIndex, int32_t length,
                                        Node *next_ग्रन्थिः) ध्रुव {
    पटिनिवत्त नूतन BTLinear_Match_ग्रन्थिः(
            elements[i].getString(*strings).data()+byteIndex,
            length,
            next_ग्रन्थिः);
}

UBool
BytesTrieBuilder::ensureCapacity(int32_t length) {
    सचे(bytes==nullptr) {
        पटिनिवत्त मिच्छा;  // previous memory allocation had failed
    }
    सचे(length>bytesCapacity) {
        int32_t newCapacity=bytesCapacity;
        करोहि {
            newCapacity*=2;
        } याव(newCapacity<=length);
        वर्ण *newBytes=static_cast<वर्ण *>(uprv_malloc(newCapacity));
        सचे(newBytes==nullptr) {
            // unable to allocate memory
            uprv_free(bytes);
            bytes=nullptr;
            bytesCapacity=0;
            पटिनिवत्त मिच्छा;
        }
        uprv_memcpy(newBytes+(newCapacity-bytesLength),
                    bytes+(bytesCapacity-bytesLength), bytesLength);
        uprv_free(bytes);
        bytes=newBytes;
        bytesCapacity=newCapacity;
    }
    पटिनिवत्त सच्च;
}

int32_t
BytesTrieBuilder::लिख(int32_t byte) {
    int32_t newLength=bytesLength+1;
    सचे(ensureCapacity(newLength)) {
        bytesLength=newLength;
        bytes[bytesCapacity - bytesLength] = static_cast<वर्ण>(byte);
    }
    पटिनिवत्त bytesLength;
}

int32_t
BytesTrieBuilder::लिख(ध्रुव वर्ण *b, int32_t length) {
    int32_t newLength=bytesLength+length;
    सचे(ensureCapacity(newLength)) {
        bytesLength=newLength;
        uprv_memcpy(bytes+(bytesCapacity-bytesLength), b, length);
    }
    पटिनिवत्त bytesLength;
}

int32_t
BytesTrieBuilder::writeElementUnits(int32_t i, int32_t byteIndex, int32_t length) {
    पटिनिवत्त लिख(elements[i].getString(*strings).data()+byteIndex, length);
}

int32_t
BytesTrieBuilder::write_मानम्_And_Final(int32_t i, UBool isFinal) {
    सचे(0<=i && i<=BytesTrie::k_Max_One_Byte_मानम्) {
        पटिनिवत्त लिख(((BytesTrie::k_Min_One_Byte_मानम्_Lead+i)<<1)|isFinal);
    }
    वर्ण intBytes[5];
    int32_t length=1;
    सचे(i<0 || i>0xffffff) {
        intBytes[0] = static_cast<वर्ण>(BytesTrie::k_Five_Byte_मानम्_Lead);
        intBytes[1] = static_cast<वर्ण>(static_cast<uint32_t>(i) >> 24);
        intBytes[2] = static_cast<वर्ण>(static_cast<uint32_t>(i) >> 16);
        intBytes[3] = static_cast<वर्ण>(static_cast<uint32_t>(i) >> 8);
        intBytes[4] = static_cast<वर्ण>(i);
        length=5;
    // } else if(i<=BytesTrie::kMaxOneByteValue) {
    //     intBytes[0]=(char)(BytesTrie::kMinOneByteValueLead+i);
    } अञ्ञथा {
        सचे(i<=BytesTrie::k_Max_Two_Byte_मानम्) {
            intBytes[0] = static_cast<वर्ण>(BytesTrie::k_Min_Two_Byte_मानम्_Lead + (i >> 8));
        } अञ्ञथा {
            सचे(i<=BytesTrie::k_Max_Three_Byte_मानम्) {
                intBytes[0] = static_cast<वर्ण>(BytesTrie::k_Min_Three_Byte_मानम्_Lead + (i >> 16));
            } अञ्ञथा {
                intBytes[0] = static_cast<वर्ण>(BytesTrie::k_Four_Byte_मानम्_Lead);
                intBytes[1] = static_cast<वर्ण>(i >> 16);
                length=2;
            }
            intBytes[length++] = static_cast<वर्ण>(i >> 8);
        }
        intBytes[length++] = static_cast<वर्ण>(i);
    }
    intBytes[0] = static_cast<वर्ण>((intBytes[0] << 1) | isFinal);
    पटिनिवत्त लिख(intBytes, length);
}

int32_t
BytesTrieBuilder::write_मानम्_And_Type(UBool has_मानम्, int32_t value, int32_t ग्रन्थिः) {
    int32_t offset=लिख(ग्रन्थिः);
    सचे(has_मानम्) {
        offset=write_मानम्_And_Final(value, मिच्छा);
    }
    पटिनिवत्त offset;
}

int32_t
BytesTrieBuilder::writeDeltaTo(int32_t jumpTarget) {
    int32_t i=bytesLength-jumpTarget;
    U_ASSERT(i>=0);
    सचे(i<=BytesTrie::kMaxOneByteDelta) {
        पटिनिवत्त लिख(i);
    } अञ्ञथा {
        वर्ण intBytes[5];
        पटिनिवत्त लिख(intBytes, internalEncodeDelta(i, intBytes));
    }
}

int32_t
BytesTrieBuilder::internalEncodeDelta(int32_t i, वर्ण intBytes[]) {
    U_ASSERT(i>=0);
    सचे(i<=BytesTrie::kMaxOneByteDelta) {
        intBytes[0] = static_cast<वर्ण>(i);
        पटिनिवत्त 1;
    }
    int32_t length=1;
    सचे(i<=BytesTrie::kMaxTwoByteDelta) {
        intBytes[0] = static_cast<वर्ण>(BytesTrie::kMinTwoByteDeltaLead + (i >> 8));
    } अञ्ञथा {
        सचे(i<=BytesTrie::kMaxThreeByteDelta) {
            intBytes[0] = static_cast<वर्ण>(BytesTrie::kMinThreeByteDeltaLead + (i >> 16));
        } अञ्ञथा {
            सचे(i<=0xffffff) {
                intBytes[0] = static_cast<वर्ण>(BytesTrie::kFourByteDeltaLead);
            } अञ्ञथा {
                intBytes[0] = static_cast<वर्ण>(BytesTrie::kFiveByteDeltaLead);
                intBytes[1] = static_cast<वर्ण>(i >> 24);
                length=2;
            }
            intBytes[length++] = static_cast<वर्ण>(i >> 16);
        }
        intBytes[length++] = static_cast<वर्ण>(i >> 8);
    }
    intBytes[length++] = static_cast<वर्ण>(i);
    पटिनिवत्त length;
}

U_NAMESPACE_END
