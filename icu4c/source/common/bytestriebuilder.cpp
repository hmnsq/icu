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

    StringPiece getString(स्थिरांक CharString& strings U_LIFETIME_BOUND) स्थिरांक {
        int32_t offset=stringOffset;
        int32_t length;
        जर(offset>=0) {
            length = static_cast<uint8_t>(strings[offset++]);
        } अन्यथा {
            offset=~offset;
            length = (static_cast<int32_t>(static_cast<uint8_t>(strings[offset])) << 8) | static_cast<uint8_t>(strings[offset + 1]);
            offset+=2;
        }
        परत StringPiece(strings.data()+offset, length);
    }
    int32_t getStringLength(स्थिरांक CharString &strings) स्थिरांक {
        int32_t offset=stringOffset;
        जर(offset>=0) {
            परत static_cast<uint8_t>(strings[offset]);
        } अन्यथा {
            offset=~offset;
            परत (static_cast<int32_t>(static_cast<uint8_t>(strings[offset])) << 8) | static_cast<uint8_t>(strings[offset + 1]);
        }
    }

    अक्षर charAt(int32_t index, स्थिरांक CharString &strings) स्थिरांक { परत data(strings)[index]; }

    int32_t get_मूल्य() स्थिरांक { परत value; }

    int32_t compareStringTo(स्थिरांक BytesTrieElement &o, स्थिरांक CharString &strings) स्थिरांक;

खाजगी:
    स्थिरांक अक्षर* data(स्थिरांक CharString& strings U_LIFETIME_BOUND) स्थिरांक {
        int32_t offset=stringOffset;
        जर(offset>=0) {
            ++offset;
        } अन्यथा {
            offset=~offset+2;
        }
        परत strings.data()+offset;
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
    जर(U_FAILURE(errorCode)) {
        परत;
    }
    int32_t length=s.length();
    जर(length>0xffff) {
        // Too long: We store the length in 1 or 2 bytes.
        errorCode=U_INDEX_OUTOFBOUNDS_ERROR;
        परत;
    }
    int32_t offset=strings.length();
    जर(length>0xff) {
        offset=~offset;
        strings.append(static_cast<अक्षर>(length >> 8), errorCode);
    }
    strings.append(static_cast<अक्षर>(length), errorCode);
    stringOffset=offset;
    value=val;
    strings.append(s, errorCode);
}

int32_t
BytesTrieElement::compareStringTo(स्थिरांक BytesTrieElement &other, स्थिरांक CharString &strings) स्थिरांक {
    // TODO: add StringPiece::compare(), see ticket #8187
    StringPiece thisString=getString(strings);
    StringPiece otherString=other.getString(strings);
    int32_t lengthDiff=thisString.length()-otherString.length();
    int32_t commonLength;
    जर(lengthDiff<=0) {
        commonLength=thisString.length();
    } अन्यथा {
        commonLength=otherString.length();
    }
    int32_t diff=uprv_memcmp(thisString.data(), otherString.data(), commonLength);
    परत diff!=0 ? diff : lengthDiff;
}

BytesTrieBuilder::BytesTrieBuilder(UErrorCode &errorCode)
        : strings(nullptr), elements(nullptr), elementsCapacity(0), elementsLength(0),
          bytes(nullptr), bytesCapacity(0), bytesLength(0) {
    जर(U_FAILURE(errorCode)) {
        परत;
    }
    strings=नवीन CharString();
    जर(strings==nullptr) {
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
    जर(U_FAILURE(errorCode)) {
        परत *हे;
    }
    जर(bytesLength>0) {
        // Cannot add elements after building.
        errorCode=U_NO_WRITE_PERMISSION;
        परत *हे;
    }
    जर(elementsLength==elementsCapacity) {
        int32_t newCapacity;
        जर(elementsCapacity==0) {
            newCapacity=1024;
        } अन्यथा {
            newCapacity=4*elementsCapacity;
        }
        BytesTrieElement *newElements=नवीन BytesTrieElement[newCapacity];
        जर(newElements==nullptr) {
            errorCode=U_MEMORY_ALLOCATION_ERROR;
            परत *हे; // error instead of dereferencing null
        }
        जर(elementsLength>0) {
            uprv_memcpy(newElements, elements, (माप_प्रकार)elementsLength*आकार(BytesTrieElement));
        }
        delete[] elements;
        elements=newElements;
        elementsCapacity=newCapacity;
    }
    elements[elementsLength++].setTo(s, value, *strings, errorCode);
    परत *हे;
}

U_CDECL_BEGIN

स्थिर int32_t U_CALLCONV
compareElementStrings(स्थिरांक शून्य *context, स्थिरांक शून्य *left, स्थिरांक शून्य *उजवा) {
    स्थिरांक CharString *strings=static_cast<स्थिरांक CharString *>(context);
    स्थिरांक BytesTrieElement *डावा_Element=static_cast<स्थिरांक BytesTrieElement *>(left);
    स्थिरांक BytesTrieElement *उजवा_Element=static_cast<स्थिरांक BytesTrieElement *>(उजवा);
    परत डावा_Element->compareStringTo(*उजवा_Element, *strings);
}

U_CDECL_END

BytesTrie *
BytesTrieBuilder::build(UStringTrieBuildOption buildOption, UErrorCode &errorCode) {
    buildBytes(buildOption, errorCode);
    BytesTrie *newTrie=nullptr;
    जर(U_SUCCESS(errorCode)) {
        newTrie=नवीन BytesTrie(bytes, bytes+(bytesCapacity-bytesLength));
        जर(newTrie==nullptr) {
            errorCode=U_MEMORY_ALLOCATION_ERROR;
        } अन्यथा {
            bytes=nullptr;  // The new trie now owns the array.
            bytesCapacity=0;
        }
    }
    परत newTrie;
}

StringPiece
BytesTrieBuilder::buildStringPiece(UStringTrieBuildOption buildOption,
                                   UErrorCode& errorCode) U_LIFETIME_BOUND {
    buildBytes(buildOption, errorCode);
    StringPiece result;
    जर(U_SUCCESS(errorCode)) {
        result.set(bytes+(bytesCapacity-bytesLength), bytesLength);
    }
    परत result;
}

शून्य
BytesTrieBuilder::buildBytes(UStringTrieBuildOption buildOption, UErrorCode &errorCode) {
    जर(U_FAILURE(errorCode)) {
        परत;
    }
    जर(bytes!=nullptr && bytesLength>0) {
        // Already built.
        परत;
    }
    जर(bytesLength==0) {
        जर(elementsLength==0) {
            errorCode=U_INDEX_OUTOFBOUNDS_ERROR;
            परत;
        }
        uprv_sortArray(elements, elementsLength, static_cast<int32_t>(आकार(BytesTrieElement)),
                      compareElementStrings, strings,
                      असत्य,  // need not be a stable sort
                      &errorCode);
        जर(U_FAILURE(errorCode)) {
            परत;
        }
        // Duplicate strings are not allowed.
        StringPiece prev=elements[0].getString(*strings);
        साठी(int32_t i=1; i<elementsLength; ++i) {
            StringPiece current=elements[i].getString(*strings);
            जर(prev==current) {
                errorCode=U_ILLEGAL_ARGUMENT_ERROR;
                परत;
            }
            prev=current;
        }
    }
    // Create and byte-serialize the trie for the elements.
    bytesLength=0;
    int32_t capacity=strings->length();
    जर(capacity<1024) {
        capacity=1024;
    }
    जर(bytesCapacity<capacity) {
        uprv_free(bytes);
        bytes=static_cast<अक्षर *>(uprv_malloc(capacity));
        जर(bytes==nullptr) {
            errorCode=U_MEMORY_ALLOCATION_ERROR;
            bytesCapacity=0;
            परत;
        }
        bytesCapacity=capacity;
    }
    StringTrieBuilder::build(buildOption, elementsLength, errorCode);
    जर(bytes==nullptr) {
        errorCode=U_MEMORY_ALLOCATION_ERROR;
    }
}

BytesTrieBuilder&
BytesTrieBuilder::clear() U_LIFETIME_BOUND {
    strings->clear();
    elementsLength=0;
    bytesLength=0;
    परत *हे;
}

int32_t
BytesTrieBuilder::getElementStringLength(int32_t i) स्थिरांक {
    परत elements[i].getStringLength(*strings);
}

char16_t
BytesTrieBuilder::getElementUnit(int32_t i, int32_t byteIndex) स्थिरांक {
    परत static_cast<uint8_t>(elements[i].charAt(byteIndex, *strings));
}

int32_t
BytesTrieBuilder::get_Element_मूल्य(int32_t i) स्थिरांक {
    परत elements[i].get_मूल्य();
}

int32_t
BytesTrieBuilder::getLimitOfLinearMatch(int32_t first, int32_t last, int32_t byteIndex) स्थिरांक {
    स्थिरांक BytesTrieElement &firstElement=elements[first];
    स्थिरांक BytesTrieElement &lastElement=elements[last];
    int32_t minStringLength=firstElement.getStringLength(*strings);
    पर्यंत(++byteIndex<minStringLength &&
            firstElement.charAt(byteIndex, *strings)==
            lastElement.charAt(byteIndex, *strings)) {}
    परत byteIndex;
}

int32_t
BytesTrieBuilder::मोजणी_Element_Units(int32_t start, int32_t limit, int32_t byteIndex) स्थिरांक {
    int32_t length=0;  // Number of different bytes at byteIndex.
    int32_t i=start;
    करा {
        अक्षर byte=elements[i++].charAt(byteIndex, *strings);
        पर्यंत(i<limit && byte==elements[i].charAt(byteIndex, *strings)) {
            ++i;
        }
        ++length;
    } पर्यंत(i<limit);
    परत length;
}

int32_t
BytesTrieBuilder::skipElementsBySomeUnits(int32_t i, int32_t byteIndex, int32_t count) स्थिरांक {
    करा {
        अक्षर byte=elements[i++].charAt(byteIndex, *strings);
        पर्यंत(byte==elements[i].charAt(byteIndex, *strings)) {
            ++i;
        }
    } पर्यंत(--count>0);
    परत i;
}

int32_t
BytesTrieBuilder::indexOfElementWithNextUnit(int32_t i, int32_t byteIndex, char16_t byte) स्थिरांक {
    अक्षर b = static_cast<अक्षर>(byte);
    पर्यंत(b==elements[i].charAt(byteIndex, *strings)) {
        ++i;
    }
    परत i;
}

BytesTrieBuilder::BTLinear_Match_गाठ::BTLinear_Match_गाठ(स्थिरांक अक्षर *bytes, int32_t len, Node *next_गाठ)
        : Linear_Match_गाठ(len, next_गाठ), s(bytes) {
    hash=static_cast<int32_t>(
        static_cast<uint32_t>(hash)*37u + static_cast<uint32_t>(ustr_hashCharsN(bytes, len)));
}

तार्किक
BytesTrieBuilder::BTLinear_Match_गाठ::चालक==(स्थिरांक Node &other) स्थिरांक {
    जर(हे==&other) {
        परत सत्य;
    }
    जर(!Linear_Match_गाठ::चालक==(other)) {
        परत असत्य;
    }
    स्थिरांक BTLinear_Match_गाठ &o=static_cast<स्थिरांक BTLinear_Match_गाठ &>(other);
    परत 0==uprv_memcmp(s, o.s, length);
}

शून्य
BytesTrieBuilder::BTLinear_Match_गाठ::लिख(StringTrieBuilder &builder) {
    BytesTrieBuilder &b=static_cast<BytesTrieBuilder &>(builder);
    next->लिख(builder);
    b.लिख(s, length);
    offset=b.लिख(b.getMinLinearMatch()+length-1);
}

StringTrieBuilder::Node *
BytesTrieBuilder::create_Linear_Match_गाठ(int32_t i, int32_t byteIndex, int32_t length,
                                        Node *next_गाठ) स्थिरांक {
    परत नवीन BTLinear_Match_गाठ(
            elements[i].getString(*strings).data()+byteIndex,
            length,
            next_गाठ);
}

UBool
BytesTrieBuilder::ensureCapacity(int32_t length) {
    जर(bytes==nullptr) {
        परत असत्य;  // previous memory allocation had failed
    }
    जर(length>bytesCapacity) {
        int32_t newCapacity=bytesCapacity;
        करा {
            newCapacity*=2;
        } पर्यंत(newCapacity<=length);
        अक्षर *newBytes=static_cast<अक्षर *>(uprv_malloc(newCapacity));
        जर(newBytes==nullptr) {
            // unable to allocate memory
            uprv_free(bytes);
            bytes=nullptr;
            bytesCapacity=0;
            परत असत्य;
        }
        uprv_memcpy(newBytes+(newCapacity-bytesLength),
                    bytes+(bytesCapacity-bytesLength), bytesLength);
        uprv_free(bytes);
        bytes=newBytes;
        bytesCapacity=newCapacity;
    }
    परत सत्य;
}

int32_t
BytesTrieBuilder::लिख(int32_t byte) {
    int32_t newLength=bytesLength+1;
    जर(ensureCapacity(newLength)) {
        bytesLength=newLength;
        bytes[bytesCapacity - bytesLength] = static_cast<अक्षर>(byte);
    }
    परत bytesLength;
}

int32_t
BytesTrieBuilder::लिख(स्थिरांक अक्षर *b, int32_t length) {
    int32_t newLength=bytesLength+length;
    जर(ensureCapacity(newLength)) {
        bytesLength=newLength;
        uprv_memcpy(bytes+(bytesCapacity-bytesLength), b, length);
    }
    परत bytesLength;
}

int32_t
BytesTrieBuilder::writeElementUnits(int32_t i, int32_t byteIndex, int32_t length) {
    परत लिख(elements[i].getString(*strings).data()+byteIndex, length);
}

int32_t
BytesTrieBuilder::write_मूल्य_And_Final(int32_t i, UBool isFinal) {
    जर(0<=i && i<=BytesTrie::k_Max_One_Byte_मूल्य) {
        परत लिख(((BytesTrie::k_Min_One_Byte_मूल्य_Lead+i)<<1)|isFinal);
    }
    अक्षर intBytes[5];
    int32_t length=1;
    जर(i<0 || i>0xffffff) {
        intBytes[0] = static_cast<अक्षर>(BytesTrie::k_Five_Byte_मूल्य_Lead);
        intBytes[1] = static_cast<अक्षर>(static_cast<uint32_t>(i) >> 24);
        intBytes[2] = static_cast<अक्षर>(static_cast<uint32_t>(i) >> 16);
        intBytes[3] = static_cast<अक्षर>(static_cast<uint32_t>(i) >> 8);
        intBytes[4] = static_cast<अक्षर>(i);
        length=5;
    // } else if(i<=BytesTrie::kMaxOneByteValue) {
    //     intBytes[0]=(char)(BytesTrie::kMinOneByteValueLead+i);
    } अन्यथा {
        जर(i<=BytesTrie::k_Max_Two_Byte_मूल्य) {
            intBytes[0] = static_cast<अक्षर>(BytesTrie::k_Min_Two_Byte_मूल्य_Lead + (i >> 8));
        } अन्यथा {
            जर(i<=BytesTrie::k_Max_Three_Byte_मूल्य) {
                intBytes[0] = static_cast<अक्षर>(BytesTrie::k_Min_Three_Byte_मूल्य_Lead + (i >> 16));
            } अन्यथा {
                intBytes[0] = static_cast<अक्षर>(BytesTrie::k_Four_Byte_मूल्य_Lead);
                intBytes[1] = static_cast<अक्षर>(i >> 16);
                length=2;
            }
            intBytes[length++] = static_cast<अक्षर>(i >> 8);
        }
        intBytes[length++] = static_cast<अक्षर>(i);
    }
    intBytes[0] = static_cast<अक्षर>((intBytes[0] << 1) | isFinal);
    परत लिख(intBytes, length);
}

int32_t
BytesTrieBuilder::write_मूल्य_And_Type(UBool has_मूल्य, int32_t value, int32_t गाठ) {
    int32_t offset=लिख(गाठ);
    जर(has_मूल्य) {
        offset=write_मूल्य_And_Final(value, असत्य);
    }
    परत offset;
}

int32_t
BytesTrieBuilder::writeDeltaTo(int32_t jumpTarget) {
    int32_t i=bytesLength-jumpTarget;
    U_ASSERT(i>=0);
    जर(i<=BytesTrie::kMaxOneByteDelta) {
        परत लिख(i);
    } अन्यथा {
        अक्षर intBytes[5];
        परत लिख(intBytes, internalEncodeDelta(i, intBytes));
    }
}

int32_t
BytesTrieBuilder::internalEncodeDelta(int32_t i, अक्षर intBytes[]) {
    U_ASSERT(i>=0);
    जर(i<=BytesTrie::kMaxOneByteDelta) {
        intBytes[0] = static_cast<अक्षर>(i);
        परत 1;
    }
    int32_t length=1;
    जर(i<=BytesTrie::kMaxTwoByteDelta) {
        intBytes[0] = static_cast<अक्षर>(BytesTrie::kMinTwoByteDeltaLead + (i >> 8));
    } अन्यथा {
        जर(i<=BytesTrie::kMaxThreeByteDelta) {
            intBytes[0] = static_cast<अक्षर>(BytesTrie::kMinThreeByteDeltaLead + (i >> 16));
        } अन्यथा {
            जर(i<=0xffffff) {
                intBytes[0] = static_cast<अक्षर>(BytesTrie::kFourByteDeltaLead);
            } अन्यथा {
                intBytes[0] = static_cast<अक्षर>(BytesTrie::kFiveByteDeltaLead);
                intBytes[1] = static_cast<अक्षर>(i >> 24);
                length=2;
            }
            intBytes[length++] = static_cast<अक्षर>(i >> 16);
        }
        intBytes[length++] = static_cast<अक्षर>(i >> 8);
    }
    intBytes[length++] = static_cast<अक्षर>(i);
    परत length;
}

U_NAMESPACE_END
