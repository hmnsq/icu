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

#समावेश "unicode/utypes.h"
#समावेश "unicode/bytestrie.h"
#समावेश "unicode/bytestriebuilder.h"
#समावेश "unicode/stringpiece.h"
#समावेश "charstr.h"
#समावेश "cmemory.h"
#समावेश "uhash.h"
#समावेश "uarrsort.h"
#समावेश "uassert.h"
#समावेश "ustr_imp.h"

U_NAMESPACE_BEGIN

/*
 * Note: This builder implementation stores (bytes, value) pairs with full copies
 * of the byte sequences, until the BytesTrie is built.
 * It might(!) take less memory if we collected the data in a temporary, dynamic trie.
 */

श्रेणी BytesTrieElement : खुला UMemory {
खुला:
    // Use compiler's default constructor, initializes nothing.

    व्योम setTo(StringPiece s, int32_t val, CharString &strings, UErrorCode &errorCode);

    StringPiece getString(स्थिर CharString& strings U_LIFETIME_BOUND) स्थिर {
        int32_t offset=stringOffset;
        int32_t length;
        अगर(offset>=0) {
            length = static_cast<uint8_t>(strings[offset++]);
        } अन्यथा {
            offset=~offset;
            length = (static_cast<int32_t>(static_cast<uint8_t>(strings[offset])) << 8) | static_cast<uint8_t>(strings[offset + 1]);
            offset+=2;
        }
        वापस StringPiece(strings.तथ्य()+offset, length);
    }
    int32_t get_String_लंबाई(स्थिर CharString &strings) स्थिर {
        int32_t offset=stringOffset;
        अगर(offset>=0) {
            वापस static_cast<uint8_t>(strings[offset]);
        } अन्यथा {
            offset=~offset;
            वापस (static_cast<int32_t>(static_cast<uint8_t>(strings[offset])) << 8) | static_cast<uint8_t>(strings[offset + 1]);
        }
    }

    अक्षर charAt(int32_t सूचकांक, स्थिर CharString &strings) स्थिर { वापस तथ्य(strings)[सूचकांक]; }

    int32_t get_मूल्य() स्थिर { वापस value; }

    int32_t compareStringTo(स्थिर BytesTrieElement &o, स्थिर CharString &strings) स्थिर;

निजी:
    स्थिर अक्षर* तथ्य(स्थिर CharString& strings U_LIFETIME_BOUND) स्थिर {
        int32_t offset=stringOffset;
        अगर(offset>=0) {
            ++offset;
        } अन्यथा {
            offset=~offset+2;
        }
        वापस strings.तथ्य()+offset;
    }

    // If the stringOffset is non-negative, then the first strings byte contains
    // the string length.
    // If the stringOffset is negative, then the first two strings bytes contain
    // the string length (big-endian), and the offset needs to be bit-inverted.
    // (Compared with a stringLength field here, this saves 3 bytes per string for most strings.)
    int32_t stringOffset;
    int32_t value;
};

व्योम
BytesTrieElement::setTo(StringPiece s, int32_t val,
                        CharString &strings, UErrorCode &errorCode) {
    अगर(U_FAILURE(errorCode)) {
        वापस;
    }
    int32_t length=s.length();
    अगर(length>0xffff) {
        // Too long: We store the length in 1 or 2 bytes.
        errorCode=U_सूचकांक_OUTOFBOUNDS_ERROR;
        वापस;
    }
    int32_t offset=strings.length();
    अगर(length>0xff) {
        offset=~offset;
        strings.append(static_cast<अक्षर>(length >> 8), errorCode);
    }
    strings.append(static_cast<अक्षर>(length), errorCode);
    stringOffset=offset;
    value=val;
    strings.append(s, errorCode);
}

int32_t
BytesTrieElement::compareStringTo(स्थिर BytesTrieElement &other, स्थिर CharString &strings) स्थिर {
    // TODO: add StringPiece::compare(), see ticket #8187
    StringPiece thisString=getString(strings);
    StringPiece otherString=other.getString(strings);
    int32_t लंबाई_Diff=thisString.length()-otherString.length();
    int32_t common_लंबाई;
    अगर(लंबाई_Diff<=0) {
        common_लंबाई=thisString.length();
    } अन्यथा {
        common_लंबाई=otherString.length();
    }
    int32_t diff=uprv_memcmp(thisString.तथ्य(), otherString.तथ्य(), common_लंबाई);
    वापस diff!=0 ? diff : लंबाई_Diff;
}

BytesTrieBuilder::BytesTrieBuilder(UErrorCode &errorCode)
        : strings(nullptr), elements(nullptr), elementsCapacity(0), elements_लंबाई(0),
          bytes(nullptr), bytesCapacity(0), bytes_लंबाई(0) {
    अगर(U_FAILURE(errorCode)) {
        वापस;
    }
    strings=नया CharString();
    अगर(strings==nullptr) {
        errorCode=U_MEMORY_ALLOCATION_ERROR;
    }
}

BytesTrieBuilder::~BytesTrieBuilder() {
    मिटाओ strings;
    मिटाओ[] elements;
    uprv_free(bytes);
}

BytesTrieBuilder&
BytesTrieBuilder::add(StringPiece s, int32_t value, UErrorCode& errorCode) U_LIFETIME_BOUND {
    अगर(U_FAILURE(errorCode)) {
        वापस *यह;
    }
    अगर(bytes_लंबाई>0) {
        // Cannot add elements after building.
        errorCode=U_NO_WRITE_PERMISSION;
        वापस *यह;
    }
    अगर(elements_लंबाई==elementsCapacity) {
        int32_t newCapacity;
        अगर(elementsCapacity==0) {
            newCapacity=1024;
        } अन्यथा {
            newCapacity=4*elementsCapacity;
        }
        BytesTrieElement *newElements=नया BytesTrieElement[newCapacity];
        अगर(newElements==nullptr) {
            errorCode=U_MEMORY_ALLOCATION_ERROR;
            वापस *यह; // error instead of dereferencing null
        }
        अगर(elements_लंबाई>0) {
            uprv_memcpy(newElements, elements, (माप_प्रकार)elements_लंबाई*माप(BytesTrieElement));
        }
        मिटाओ[] elements;
        elements=newElements;
        elementsCapacity=newCapacity;
    }
    elements[elements_लंबाई++].setTo(s, value, *strings, errorCode);
    वापस *यह;
}

U_CDECL_BEGIN

अटल int32_t U_CALLCONV
compareElementStrings(स्थिर व्योम *context, स्थिर व्योम *बाँए, स्थिर व्योम *दाँए) {
    स्थिर CharString *strings=static_cast<स्थिर CharString *>(context);
    स्थिर BytesTrieElement *बायाँ_Element=static_cast<स्थिर BytesTrieElement *>(बाँए);
    स्थिर BytesTrieElement *दायाँ_Element=static_cast<स्थिर BytesTrieElement *>(दाँए);
    वापस बायाँ_Element->compareStringTo(*दायाँ_Element, *strings);
}

U_CDECL_END

BytesTrie *
BytesTrieBuilder::build(UStringTrieBuildOption buildOption, UErrorCode &errorCode) {
    buildBytes(buildOption, errorCode);
    BytesTrie *newTrie=nullptr;
    अगर(U_SUCCESS(errorCode)) {
        newTrie=नया BytesTrie(bytes, bytes+(bytesCapacity-bytes_लंबाई));
        अगर(newTrie==nullptr) {
            errorCode=U_MEMORY_ALLOCATION_ERROR;
        } अन्यथा {
            bytes=nullptr;  // The new trie now owns the array.
            bytesCapacity=0;
        }
    }
    वापस newTrie;
}

StringPiece
BytesTrieBuilder::buildStringPiece(UStringTrieBuildOption buildOption,
                                   UErrorCode& errorCode) U_LIFETIME_BOUND {
    buildBytes(buildOption, errorCode);
    StringPiece परिणाम;
    अगर(U_SUCCESS(errorCode)) {
        परिणाम.set(bytes+(bytesCapacity-bytes_लंबाई), bytes_लंबाई);
    }
    वापस परिणाम;
}

व्योम
BytesTrieBuilder::buildBytes(UStringTrieBuildOption buildOption, UErrorCode &errorCode) {
    अगर(U_FAILURE(errorCode)) {
        वापस;
    }
    अगर(bytes!=nullptr && bytes_लंबाई>0) {
        // Already built.
        वापस;
    }
    अगर(bytes_लंबाई==0) {
        अगर(elements_लंबाई==0) {
            errorCode=U_सूचकांक_OUTOFBOUNDS_ERROR;
            वापस;
        }
        uprv_sortArray(elements, elements_लंबाई, static_cast<int32_t>(माप(BytesTrieElement)),
                      compareElementStrings, strings,
                      false,  // need not be a stable sort
                      &errorCode);
        अगर(U_FAILURE(errorCode)) {
            वापस;
        }
        // Duplicate strings are not allowed.
        StringPiece पूर्व=elements[0].getString(*strings);
        क्रम(int32_t i=1; i<elements_लंबाई; ++i) {
            StringPiece current=elements[i].getString(*strings);
            अगर(पूर्व==current) {
                errorCode=U_ILLEGAL_ARGUMENT_ERROR;
                वापस;
            }
            पूर्व=current;
        }
    }
    // Create and byte-serialize the trie for the elements.
    bytes_लंबाई=0;
    int32_t capacity=strings->length();
    अगर(capacity<1024) {
        capacity=1024;
    }
    अगर(bytesCapacity<capacity) {
        uprv_free(bytes);
        bytes=static_cast<अक्षर *>(uprv_malloc(capacity));
        अगर(bytes==nullptr) {
            errorCode=U_MEMORY_ALLOCATION_ERROR;
            bytesCapacity=0;
            वापस;
        }
        bytesCapacity=capacity;
    }
    StringTrieBuilder::build(buildOption, elements_लंबाई, errorCode);
    अगर(bytes==nullptr) {
        errorCode=U_MEMORY_ALLOCATION_ERROR;
    }
}

BytesTrieBuilder&
BytesTrieBuilder::सफा() U_LIFETIME_BOUND {
    strings->सफा();
    elements_लंबाई=0;
    bytes_लंबाई=0;
    वापस *यह;
}

int32_t
BytesTrieBuilder::get_Element_String_लंबाई(int32_t i) स्थिर {
    वापस elements[i].get_String_लंबाई(*strings);
}

char16_t
BytesTrieBuilder::getElementUnit(int32_t i, int32_t byte_सूचकांक) स्थिर {
    वापस static_cast<uint8_t>(elements[i].charAt(byte_सूचकांक, *strings));
}

int32_t
BytesTrieBuilder::get_Element_मूल्य(int32_t i) स्थिर {
    वापस elements[i].get_मूल्य();
}

int32_t
BytesTrieBuilder::getLimitOfLinearMatch(int32_t first, int32_t last, int32_t byte_सूचकांक) स्थिर {
    स्थिर BytesTrieElement &firstElement=elements[first];
    स्थिर BytesTrieElement &lastElement=elements[last];
    int32_t min_String_लंबाई=firstElement.get_String_लंबाई(*strings);
    जबतक(++byte_सूचकांक<min_String_लंबाई &&
            firstElement.charAt(byte_सूचकांक, *strings)==
            lastElement.charAt(byte_सूचकांक, *strings)) {}
    वापस byte_सूचकांक;
}

int32_t
BytesTrieBuilder::गिनती_Element_Units(int32_t start, int32_t limit, int32_t byte_सूचकांक) स्थिर {
    int32_t length=0;  // Number of different bytes at byteIndex.
    int32_t i=start;
    करो {
        अक्षर byte=elements[i++].charAt(byte_सूचकांक, *strings);
        जबतक(i<limit && byte==elements[i].charAt(byte_सूचकांक, *strings)) {
            ++i;
        }
        ++length;
    } जबतक(i<limit);
    वापस length;
}

int32_t
BytesTrieBuilder::skipElementsBySomeUnits(int32_t i, int32_t byte_सूचकांक, int32_t count) स्थिर {
    करो {
        अक्षर byte=elements[i++].charAt(byte_सूचकांक, *strings);
        जबतक(byte==elements[i].charAt(byte_सूचकांक, *strings)) {
            ++i;
        }
    } जबतक(--count>0);
    वापस i;
}

int32_t
BytesTrieBuilder::सूचकांक_Of_Element_With_अग्र_Unit(int32_t i, int32_t byte_सूचकांक, char16_t byte) स्थिर {
    अक्षर b = static_cast<अक्षर>(byte);
    जबतक(b==elements[i].charAt(byte_सूचकांक, *strings)) {
        ++i;
    }
    वापस i;
}

BytesTrieBuilder::BTLinear_Match_गाँठ::BTLinear_Match_गाँठ(स्थिर अक्षर *bytes, int32_t len, Node *अग्र_गाँठ)
        : Linear_Match_गाँठ(len, अग्र_गाँठ), s(bytes) {
    hash=static_cast<int32_t>(
        static_cast<uint32_t>(hash)*37u + static_cast<uint32_t>(ustr_hashCharsN(bytes, len)));
}

bool
BytesTrieBuilder::BTLinear_Match_गाँठ::चालक==(स्थिर Node &other) स्थिर {
    अगर(यह==&other) {
        वापस true;
    }
    अगर(!Linear_Match_गाँठ::चालक==(other)) {
        वापस false;
    }
    स्थिर BTLinear_Match_गाँठ &o=static_cast<स्थिर BTLinear_Match_गाँठ &>(other);
    वापस 0==uprv_memcmp(s, o.s, length);
}

व्योम
BytesTrieBuilder::BTLinear_Match_गाँठ::ग_लिखो(StringTrieBuilder &builder) {
    BytesTrieBuilder &b=static_cast<BytesTrieBuilder &>(builder);
    next->ग_लिखो(builder);
    b.ग_लिखो(s, length);
    offset=b.ग_लिखो(b.getMinLinearMatch()+length-1);
}

StringTrieBuilder::Node *
BytesTrieBuilder::create_Linear_Match_गाँठ(int32_t i, int32_t byte_सूचकांक, int32_t length,
                                        Node *अग्र_गाँठ) स्थिर {
    वापस नया BTLinear_Match_गाँठ(
            elements[i].getString(*strings).तथ्य()+byte_सूचकांक,
            length,
            अग्र_गाँठ);
}

UBool
BytesTrieBuilder::ensureCapacity(int32_t length) {
    अगर(bytes==nullptr) {
        वापस false;  // previous memory allocation had failed
    }
    अगर(length>bytesCapacity) {
        int32_t newCapacity=bytesCapacity;
        करो {
            newCapacity*=2;
        } जबतक(newCapacity<=length);
        अक्षर *newBytes=static_cast<अक्षर *>(uprv_malloc(newCapacity));
        अगर(newBytes==nullptr) {
            // unable to allocate memory
            uprv_free(bytes);
            bytes=nullptr;
            bytesCapacity=0;
            वापस false;
        }
        uprv_memcpy(newBytes+(newCapacity-bytes_लंबाई),
                    bytes+(bytesCapacity-bytes_लंबाई), bytes_लंबाई);
        uprv_free(bytes);
        bytes=newBytes;
        bytesCapacity=newCapacity;
    }
    वापस true;
}

int32_t
BytesTrieBuilder::ग_लिखो(int32_t byte) {
    int32_t new_लंबाई=bytes_लंबाई+1;
    अगर(ensureCapacity(new_लंबाई)) {
        bytes_लंबाई=new_लंबाई;
        bytes[bytesCapacity - bytes_लंबाई] = static_cast<अक्षर>(byte);
    }
    वापस bytes_लंबाई;
}

int32_t
BytesTrieBuilder::ग_लिखो(स्थिर अक्षर *b, int32_t length) {
    int32_t new_लंबाई=bytes_लंबाई+length;
    अगर(ensureCapacity(new_लंबाई)) {
        bytes_लंबाई=new_लंबाई;
        uprv_memcpy(bytes+(bytesCapacity-bytes_लंबाई), b, length);
    }
    वापस bytes_लंबाई;
}

int32_t
BytesTrieBuilder::writeElementUnits(int32_t i, int32_t byte_सूचकांक, int32_t length) {
    वापस ग_लिखो(elements[i].getString(*strings).तथ्य()+byte_सूचकांक, length);
}

int32_t
BytesTrieBuilder::write_मूल्य_And_Final(int32_t i, UBool isFinal) {
    अगर(0<=i && i<=BytesTrie::k_Max_One_Byte_मूल्य) {
        वापस ग_लिखो(((BytesTrie::k_Min_One_Byte_मूल्य_Lead+i)<<1)|isFinal);
    }
    अक्षर intBytes[5];
    int32_t length=1;
    अगर(i<0 || i>0xffffff) {
        intBytes[0] = static_cast<अक्षर>(BytesTrie::k_Five_Byte_मूल्य_Lead);
        intBytes[1] = static_cast<अक्षर>(static_cast<uint32_t>(i) >> 24);
        intBytes[2] = static_cast<अक्षर>(static_cast<uint32_t>(i) >> 16);
        intBytes[3] = static_cast<अक्षर>(static_cast<uint32_t>(i) >> 8);
        intBytes[4] = static_cast<अक्षर>(i);
        length=5;
    // } else if(i<=BytesTrie::kMaxOneByteValue) {
    //     intBytes[0]=(char)(BytesTrie::kMinOneByteValueLead+i);
    } अन्यथा {
        अगर(i<=BytesTrie::k_Max_Two_Byte_मूल्य) {
            intBytes[0] = static_cast<अक्षर>(BytesTrie::k_Min_Two_Byte_मूल्य_Lead + (i >> 8));
        } अन्यथा {
            अगर(i<=BytesTrie::k_Max_Three_Byte_मूल्य) {
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
    वापस ग_लिखो(intBytes, length);
}

int32_t
BytesTrieBuilder::write_मूल्य_And_Type(UBool has_मूल्य, int32_t value, int32_t गाँठ) {
    int32_t offset=ग_लिखो(गाँठ);
    अगर(has_मूल्य) {
        offset=write_मूल्य_And_Final(value, false);
    }
    वापस offset;
}

int32_t
BytesTrieBuilder::writeDeltaTo(int32_t jumpTarget) {
    int32_t i=bytes_लंबाई-jumpTarget;
    U_ASSERT(i>=0);
    अगर(i<=BytesTrie::kMaxOneByteDelta) {
        वापस ग_लिखो(i);
    } अन्यथा {
        अक्षर intBytes[5];
        वापस ग_लिखो(intBytes, internalEncodeDelta(i, intBytes));
    }
}

int32_t
BytesTrieBuilder::internalEncodeDelta(int32_t i, अक्षर intBytes[]) {
    U_ASSERT(i>=0);
    अगर(i<=BytesTrie::kMaxOneByteDelta) {
        intBytes[0] = static_cast<अक्षर>(i);
        वापस 1;
    }
    int32_t length=1;
    अगर(i<=BytesTrie::kMaxTwoByteDelta) {
        intBytes[0] = static_cast<अक्षर>(BytesTrie::kMinTwoByteDeltaLead + (i >> 8));
    } अन्यथा {
        अगर(i<=BytesTrie::kMaxThreeByteDelta) {
            intBytes[0] = static_cast<अक्षर>(BytesTrie::kMinThreeByteDeltaLead + (i >> 16));
        } अन्यथा {
            अगर(i<=0xffffff) {
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
    वापस length;
}

U_NAMESPACE_END
