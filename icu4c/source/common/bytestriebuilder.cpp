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

#অন্তর্ভাব "unicode/utypes.h"
#অন্তর্ভাব "unicode/bytestrie.h"
#অন্তর্ভাব "unicode/bytestriebuilder.h"
#অন্তর্ভাব "unicode/stringpiece.h"
#অন্তর্ভাব "charstr.h"
#অন্তর্ভাব "cmemory.h"
#অন্তর্ভাব "uhash.h"
#অন্তর্ভাব "uarrsort.h"
#অন্তর্ভাব "uassert.h"
#অন্তর্ভাব "ustr_imp.h"

U_NAMESPACE_BEGIN

/*
 * Note: This builder implementation stores (bytes, value) pairs with full copies
 * of the byte sequences, until the BytesTrie is built.
 * It might(!) take less memory if we collected the data in a temporary, dynamic trie.
 */

শ্রেণী BytesTrieElement : সর্বজনীন UMemory {
সর্বজনীন:
    // Use compiler's default constructor, initializes nothing.

    শূন্য setTo(StringPiece s, int32_t val, CharString &strings, UErrorCode &errorCode);

    StringPiece getString(ধ্রুবক CharString& strings U_LIFETIME_BOUND) ধ্রুবক {
        int32_t offset=stringOffset;
        int32_t length;
        যদি(offset>=0) {
            length = static_cast<uint8_t>(strings[offset++]);
        } অন্যথা {
            offset=~offset;
            length = (static_cast<int32_t>(static_cast<uint8_t>(strings[offset])) << 8) | static_cast<uint8_t>(strings[offset + 1]);
            offset+=2;
        }
        ফেরত StringPiece(strings.তথ্য()+offset, length);
    }
    int32_t get_String_দৈর্ঘ্য(ধ্রুবক CharString &strings) ধ্রুবক {
        int32_t offset=stringOffset;
        যদি(offset>=0) {
            ফেরত static_cast<uint8_t>(strings[offset]);
        } অন্যথা {
            offset=~offset;
            ফেরত (static_cast<int32_t>(static_cast<uint8_t>(strings[offset])) << 8) | static_cast<uint8_t>(strings[offset + 1]);
        }
    }

    অক্ষর charAt(int32_t সূচক, ধ্রুবক CharString &strings) ধ্রুবক { ফেরত তথ্য(strings)[সূচক]; }

    int32_t get_মান() ধ্রুবক { ফেরত value; }

    int32_t compareStringTo(ধ্রুবক BytesTrieElement &o, ধ্রুবক CharString &strings) ধ্রুবক;

ব্যক্তিগত:
    ধ্রুবক অক্ষর* তথ্য(ধ্রুবক CharString& strings U_LIFETIME_BOUND) ধ্রুবক {
        int32_t offset=stringOffset;
        যদি(offset>=0) {
            ++offset;
        } অন্যথা {
            offset=~offset+2;
        }
        ফেরত strings.তথ্য()+offset;
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
        ফেরত;
    }
    int32_t length=s.length();
    যদি(length>0xffff) {
        // Too long: We store the length in 1 or 2 bytes.
        errorCode=U_সূচক_OUTOFBOUNDS_ERROR;
        ফেরত;
    }
    int32_t offset=strings.length();
    যদি(length>0xff) {
        offset=~offset;
        strings.append(static_cast<অক্ষর>(length >> 8), errorCode);
    }
    strings.append(static_cast<অক্ষর>(length), errorCode);
    stringOffset=offset;
    value=val;
    strings.append(s, errorCode);
}

int32_t
BytesTrieElement::compareStringTo(ধ্রুবক BytesTrieElement &other, ধ্রুবক CharString &strings) ধ্রুবক {
    // TODO: add StringPiece::compare(), see ticket #8187
    StringPiece thisString=getString(strings);
    StringPiece otherString=other.getString(strings);
    int32_t দৈর্ঘ্য_Diff=thisString.length()-otherString.length();
    int32_t common_দৈর্ঘ্য;
    যদি(দৈর্ঘ্য_Diff<=0) {
        common_দৈর্ঘ্য=thisString.length();
    } অন্যথা {
        common_দৈর্ঘ্য=otherString.length();
    }
    int32_t diff=uprv_memcmp(thisString.তথ্য(), otherString.তথ্য(), common_দৈর্ঘ্য);
    ফেরত diff!=0 ? diff : দৈর্ঘ্য_Diff;
}

BytesTrieBuilder::BytesTrieBuilder(UErrorCode &errorCode)
        : strings(nullptr), elements(nullptr), elementsCapacity(0), elements_দৈর্ঘ্য(0),
          bytes(nullptr), bytesCapacity(0), bytes_দৈর্ঘ্য(0) {
    যদি(U_FAILURE(errorCode)) {
        ফেরত;
    }
    strings=নতুন CharString();
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
        ফেরত *এই;
    }
    যদি(bytes_দৈর্ঘ্য>0) {
        // Cannot add elements after building.
        errorCode=U_NO_WRITE_PERMISSION;
        ফেরত *এই;
    }
    যদি(elements_দৈর্ঘ্য==elementsCapacity) {
        int32_t newCapacity;
        যদি(elementsCapacity==0) {
            newCapacity=1024;
        } অন্যথা {
            newCapacity=4*elementsCapacity;
        }
        BytesTrieElement *newElements=নতুন BytesTrieElement[newCapacity];
        যদি(newElements==nullptr) {
            errorCode=U_MEMORY_ALLOCATION_ERROR;
            ফেরত *এই; // error instead of dereferencing null
        }
        যদি(elements_দৈর্ঘ্য>0) {
            uprv_memcpy(newElements, elements, (size_t)elements_দৈর্ঘ্য*আকার(BytesTrieElement));
        }
        delete[] elements;
        elements=newElements;
        elementsCapacity=newCapacity;
    }
    elements[elements_দৈর্ঘ্য++].setTo(s, value, *strings, errorCode);
    ফেরত *এই;
}

U_CDECL_BEGIN

স্থির int32_t U_CALLCONV
compareElementStrings(ধ্রুবক শূন্য *context, ধ্রুবক শূন্য *left, ধ্রুবক শূন্য *ডান) {
    ধ্রুবক CharString *strings=static_cast<ধ্রুবক CharString *>(context);
    ধ্রুবক BytesTrieElement *বাম_Element=static_cast<ধ্রুবক BytesTrieElement *>(left);
    ধ্রুবক BytesTrieElement *ডান_Element=static_cast<ধ্রুবক BytesTrieElement *>(ডান);
    ফেরত বাম_Element->compareStringTo(*ডান_Element, *strings);
}

U_CDECL_END

BytesTrie *
BytesTrieBuilder::build(UStringTrieBuildOption buildOption, UErrorCode &errorCode) {
    buildBytes(buildOption, errorCode);
    BytesTrie *newTrie=nullptr;
    যদি(U_SUCCESS(errorCode)) {
        newTrie=নতুন BytesTrie(bytes, bytes+(bytesCapacity-bytes_দৈর্ঘ্য));
        যদি(newTrie==nullptr) {
            errorCode=U_MEMORY_ALLOCATION_ERROR;
        } অন্যথা {
            bytes=nullptr;  // The new trie now owns the array.
            bytesCapacity=0;
        }
    }
    ফেরত newTrie;
}

StringPiece
BytesTrieBuilder::buildStringPiece(UStringTrieBuildOption buildOption,
                                   UErrorCode& errorCode) U_LIFETIME_BOUND {
    buildBytes(buildOption, errorCode);
    StringPiece ফলাফল;
    যদি(U_SUCCESS(errorCode)) {
        ফলাফল.set(bytes+(bytesCapacity-bytes_দৈর্ঘ্য), bytes_দৈর্ঘ্য);
    }
    ফেরত ফলাফল;
}

শূন্য
BytesTrieBuilder::buildBytes(UStringTrieBuildOption buildOption, UErrorCode &errorCode) {
    যদি(U_FAILURE(errorCode)) {
        ফেরত;
    }
    যদি(bytes!=nullptr && bytes_দৈর্ঘ্য>0) {
        // Already built.
        ফেরত;
    }
    যদি(bytes_দৈর্ঘ্য==0) {
        যদি(elements_দৈর্ঘ্য==0) {
            errorCode=U_সূচক_OUTOFBOUNDS_ERROR;
            ফেরত;
        }
        uprv_sortArray(elements, elements_দৈর্ঘ্য, static_cast<int32_t>(আকার(BytesTrieElement)),
                      compareElementStrings, strings,
                      মিথ্যা,  // need not be a stable sort
                      &errorCode);
        যদি(U_FAILURE(errorCode)) {
            ফেরত;
        }
        // Duplicate strings are not allowed.
        StringPiece পূর্ব=elements[0].getString(*strings);
        জন্য(int32_t i=1; i<elements_দৈর্ঘ্য; ++i) {
            StringPiece current=elements[i].getString(*strings);
            যদি(পূর্ব==current) {
                errorCode=U_ILLEGAL_ARGUMENT_ERROR;
                ফেরত;
            }
            পূর্ব=current;
        }
    }
    // Create and byte-serialize the trie for the elements.
    bytes_দৈর্ঘ্য=0;
    int32_t capacity=strings->length();
    যদি(capacity<1024) {
        capacity=1024;
    }
    যদি(bytesCapacity<capacity) {
        uprv_free(bytes);
        bytes=static_cast<অক্ষর *>(uprv_malloc(capacity));
        যদি(bytes==nullptr) {
            errorCode=U_MEMORY_ALLOCATION_ERROR;
            bytesCapacity=0;
            ফেরত;
        }
        bytesCapacity=capacity;
    }
    StringTrieBuilder::build(buildOption, elements_দৈর্ঘ্য, errorCode);
    যদি(bytes==nullptr) {
        errorCode=U_MEMORY_ALLOCATION_ERROR;
    }
}

BytesTrieBuilder&
BytesTrieBuilder::clear() U_LIFETIME_BOUND {
    strings->clear();
    elements_দৈর্ঘ্য=0;
    bytes_দৈর্ঘ্য=0;
    ফেরত *এই;
}

int32_t
BytesTrieBuilder::get_Element_String_দৈর্ঘ্য(int32_t i) ধ্রুবক {
    ফেরত elements[i].get_String_দৈর্ঘ্য(*strings);
}

char16_t
BytesTrieBuilder::getElementUnit(int32_t i, int32_t byte_সূচক) ধ্রুবক {
    ফেরত static_cast<uint8_t>(elements[i].charAt(byte_সূচক, *strings));
}

int32_t
BytesTrieBuilder::get_Element_মান(int32_t i) ধ্রুবক {
    ফেরত elements[i].get_মান();
}

int32_t
BytesTrieBuilder::getLimitOfLinearMatch(int32_t first, int32_t last, int32_t byte_সূচক) ধ্রুবক {
    ধ্রুবক BytesTrieElement &firstElement=elements[first];
    ধ্রুবক BytesTrieElement &lastElement=elements[last];
    int32_t min_String_দৈর্ঘ্য=firstElement.get_String_দৈর্ঘ্য(*strings);
    যখন(++byte_সূচক<min_String_দৈর্ঘ্য &&
            firstElement.charAt(byte_সূচক, *strings)==
            lastElement.charAt(byte_সূচক, *strings)) {}
    ফেরত byte_সূচক;
}

int32_t
BytesTrieBuilder::গুনতি_Element_Units(int32_t start, int32_t limit, int32_t byte_সূচক) ধ্রুবক {
    int32_t length=0;  // Number of different bytes at byteIndex.
    int32_t i=start;
    করো {
        অক্ষর byte=elements[i++].charAt(byte_সূচক, *strings);
        যখন(i<limit && byte==elements[i].charAt(byte_সূচক, *strings)) {
            ++i;
        }
        ++length;
    } যখন(i<limit);
    ফেরত length;
}

int32_t
BytesTrieBuilder::skipElementsBySomeUnits(int32_t i, int32_t byte_সূচক, int32_t count) ধ্রুবক {
    করো {
        অক্ষর byte=elements[i++].charAt(byte_সূচক, *strings);
        যখন(byte==elements[i].charAt(byte_সূচক, *strings)) {
            ++i;
        }
    } যখন(--count>0);
    ফেরত i;
}

int32_t
BytesTrieBuilder::সূচক_Of_Element_With_পরবর্তী_Unit(int32_t i, int32_t byte_সূচক, char16_t byte) ধ্রুবক {
    অক্ষর b = static_cast<অক্ষর>(byte);
    যখন(b==elements[i].charAt(byte_সূচক, *strings)) {
        ++i;
    }
    ফেরত i;
}

BytesTrieBuilder::BTLinear_Match_গ্রন্থি::BTLinear_Match_গ্রন্থি(ধ্রুবক অক্ষর *bytes, int32_t len, Node *পরবর্তী_গ্রন্থি)
        : Linear_Match_গ্রন্থি(len, পরবর্তী_গ্রন্থি), s(bytes) {
    hash=static_cast<int32_t>(
        static_cast<uint32_t>(hash)*37u + static_cast<uint32_t>(ustr_hashCharsN(bytes, len)));
}

যৌক্তিক
BytesTrieBuilder::BTLinear_Match_গ্রন্থি::operator==(ধ্রুবক Node &other) ধ্রুবক {
    যদি(এই==&other) {
        ফেরত সত্য;
    }
    যদি(!Linear_Match_গ্রন্থি::operator==(other)) {
        ফেরত মিথ্যা;
    }
    ধ্রুবক BTLinear_Match_গ্রন্থি &o=static_cast<ধ্রুবক BTLinear_Match_গ্রন্থি &>(other);
    ফেরত 0==uprv_memcmp(s, o.s, length);
}

শূন্য
BytesTrieBuilder::BTLinear_Match_গ্রন্থি::লিখ(StringTrieBuilder &builder) {
    BytesTrieBuilder &b=static_cast<BytesTrieBuilder &>(builder);
    next->লিখ(builder);
    b.লিখ(s, length);
    offset=b.লিখ(b.getMinLinearMatch()+length-1);
}

StringTrieBuilder::Node *
BytesTrieBuilder::create_Linear_Match_গ্রন্থি(int32_t i, int32_t byte_সূচক, int32_t length,
                                        Node *পরবর্তী_গ্রন্থি) ধ্রুবক {
    ফেরত নতুন BTLinear_Match_গ্রন্থি(
            elements[i].getString(*strings).তথ্য()+byte_সূচক,
            length,
            পরবর্তী_গ্রন্থি);
}

UBool
BytesTrieBuilder::ensureCapacity(int32_t length) {
    যদি(bytes==nullptr) {
        ফেরত মিথ্যা;  // previous memory allocation had failed
    }
    যদি(length>bytesCapacity) {
        int32_t newCapacity=bytesCapacity;
        করো {
            newCapacity*=2;
        } যখন(newCapacity<=length);
        অক্ষর *newBytes=static_cast<অক্ষর *>(uprv_malloc(newCapacity));
        যদি(newBytes==nullptr) {
            // unable to allocate memory
            uprv_free(bytes);
            bytes=nullptr;
            bytesCapacity=0;
            ফেরত মিথ্যা;
        }
        uprv_memcpy(newBytes+(newCapacity-bytes_দৈর্ঘ্য),
                    bytes+(bytesCapacity-bytes_দৈর্ঘ্য), bytes_দৈর্ঘ্য);
        uprv_free(bytes);
        bytes=newBytes;
        bytesCapacity=newCapacity;
    }
    ফেরত সত্য;
}

int32_t
BytesTrieBuilder::লিখ(int32_t byte) {
    int32_t new_দৈর্ঘ্য=bytes_দৈর্ঘ্য+1;
    যদি(ensureCapacity(new_দৈর্ঘ্য)) {
        bytes_দৈর্ঘ্য=new_দৈর্ঘ্য;
        bytes[bytesCapacity - bytes_দৈর্ঘ্য] = static_cast<অক্ষর>(byte);
    }
    ফেরত bytes_দৈর্ঘ্য;
}

int32_t
BytesTrieBuilder::লিখ(ধ্রুবক অক্ষর *b, int32_t length) {
    int32_t new_দৈর্ঘ্য=bytes_দৈর্ঘ্য+length;
    যদি(ensureCapacity(new_দৈর্ঘ্য)) {
        bytes_দৈর্ঘ্য=new_দৈর্ঘ্য;
        uprv_memcpy(bytes+(bytesCapacity-bytes_দৈর্ঘ্য), b, length);
    }
    ফেরত bytes_দৈর্ঘ্য;
}

int32_t
BytesTrieBuilder::writeElementUnits(int32_t i, int32_t byte_সূচক, int32_t length) {
    ফেরত লিখ(elements[i].getString(*strings).তথ্য()+byte_সূচক, length);
}

int32_t
BytesTrieBuilder::write_মান_And_Final(int32_t i, UBool isFinal) {
    যদি(0<=i && i<=BytesTrie::k_Max_One_Byte_মান) {
        ফেরত লিখ(((BytesTrie::k_Min_One_Byte_মান_Lead+i)<<1)|isFinal);
    }
    অক্ষর intBytes[5];
    int32_t length=1;
    যদি(i<0 || i>0xffffff) {
        intBytes[0] = static_cast<অক্ষর>(BytesTrie::k_Five_Byte_মান_Lead);
        intBytes[1] = static_cast<অক্ষর>(static_cast<uint32_t>(i) >> 24);
        intBytes[2] = static_cast<অক্ষর>(static_cast<uint32_t>(i) >> 16);
        intBytes[3] = static_cast<অক্ষর>(static_cast<uint32_t>(i) >> 8);
        intBytes[4] = static_cast<অক্ষর>(i);
        length=5;
    // } else if(i<=BytesTrie::kMaxOneByteValue) {
    //     intBytes[0]=(char)(BytesTrie::kMinOneByteValueLead+i);
    } অন্যথা {
        যদি(i<=BytesTrie::k_Max_Two_Byte_মান) {
            intBytes[0] = static_cast<অক্ষর>(BytesTrie::k_Min_Two_Byte_মান_Lead + (i >> 8));
        } অন্যথা {
            যদি(i<=BytesTrie::k_Max_Three_Byte_মান) {
                intBytes[0] = static_cast<অক্ষর>(BytesTrie::k_Min_Three_Byte_মান_Lead + (i >> 16));
            } অন্যথা {
                intBytes[0] = static_cast<অক্ষর>(BytesTrie::k_Four_Byte_মান_Lead);
                intBytes[1] = static_cast<অক্ষর>(i >> 16);
                length=2;
            }
            intBytes[length++] = static_cast<অক্ষর>(i >> 8);
        }
        intBytes[length++] = static_cast<অক্ষর>(i);
    }
    intBytes[0] = static_cast<অক্ষর>((intBytes[0] << 1) | isFinal);
    ফেরত লিখ(intBytes, length);
}

int32_t
BytesTrieBuilder::write_মান_And_Type(UBool has_মান, int32_t value, int32_t গ্রন্থি) {
    int32_t offset=লিখ(গ্রন্থি);
    যদি(has_মান) {
        offset=write_মান_And_Final(value, মিথ্যা);
    }
    ফেরত offset;
}

int32_t
BytesTrieBuilder::writeDeltaTo(int32_t jumpTarget) {
    int32_t i=bytes_দৈর্ঘ্য-jumpTarget;
    U_ASSERT(i>=0);
    যদি(i<=BytesTrie::kMaxOneByteDelta) {
        ফেরত লিখ(i);
    } অন্যথা {
        অক্ষর intBytes[5];
        ফেরত লিখ(intBytes, internalEncodeDelta(i, intBytes));
    }
}

int32_t
BytesTrieBuilder::internalEncodeDelta(int32_t i, অক্ষর intBytes[]) {
    U_ASSERT(i>=0);
    যদি(i<=BytesTrie::kMaxOneByteDelta) {
        intBytes[0] = static_cast<অক্ষর>(i);
        ফেরত 1;
    }
    int32_t length=1;
    যদি(i<=BytesTrie::kMaxTwoByteDelta) {
        intBytes[0] = static_cast<অক্ষর>(BytesTrie::kMinTwoByteDeltaLead + (i >> 8));
    } অন্যথা {
        যদি(i<=BytesTrie::kMaxThreeByteDelta) {
            intBytes[0] = static_cast<অক্ষর>(BytesTrie::kMinThreeByteDeltaLead + (i >> 16));
        } অন্যথা {
            যদি(i<=0xffffff) {
                intBytes[0] = static_cast<অক্ষর>(BytesTrie::kFourByteDeltaLead);
            } অন্যথা {
                intBytes[0] = static_cast<অক্ষর>(BytesTrie::kFiveByteDeltaLead);
                intBytes[1] = static_cast<অক্ষর>(i >> 24);
                length=2;
            }
            intBytes[length++] = static_cast<অক্ষর>(i >> 16);
        }
        intBytes[length++] = static_cast<অক্ষর>(i >> 8);
    }
    intBytes[length++] = static_cast<অক্ষর>(i);
    ফেরত length;
}

U_NAMESPACE_END
