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

বর্গ BytesTrieElement : সার্বজনিক UMemory {
সার্বজনিক:
    // Use compiler's default constructor, initializes nothing.

    শূন্য setTo(StringPiece s, int32_t val, CharString &strings, UErrorCode &errorCode);

    StringPiece getString(ধ্রুব CharString& strings U_LIFETIME_BOUND) ধ্রুব {
        int32_t offset=stringOffset;
        int32_t length;
        যদি(offset>=0) {
            length = static_cast<uint8_t>(strings[offset++]);
        } অন্যথা {
            offset=~offset;
            length = (static_cast<int32_t>(static_cast<uint8_t>(strings[offset])) << 8) | static_cast<uint8_t>(strings[offset + 1]);
            offset+=2;
        }
        প্রত্যাগম StringPiece(strings.data()+offset, length);
    }
    int32_t getStringLength(ধ্রুব CharString &strings) ধ্রুব {
        int32_t offset=stringOffset;
        যদি(offset>=0) {
            প্রত্যাগম static_cast<uint8_t>(strings[offset]);
        } অন্যথা {
            offset=~offset;
            প্রত্যাগম (static_cast<int32_t>(static_cast<uint8_t>(strings[offset])) << 8) | static_cast<uint8_t>(strings[offset + 1]);
        }
    }

    বর্ণ charAt(int32_t index, ধ্রুব CharString &strings) ধ্রুব { প্রত্যাগম data(strings)[index]; }

    int32_t get_মান() ধ্রুব { প্রত্যাগম value; }

    int32_t compareStringTo(ধ্রুব BytesTrieElement &o, ধ্রুব CharString &strings) ধ্রুব;

গুপ্ত:
    ধ্রুব বর্ণ* data(ধ্রুব CharString& strings U_LIFETIME_BOUND) ধ্রুব {
        int32_t offset=stringOffset;
        যদি(offset>=0) {
            ++offset;
        } অন্যথা {
            offset=~offset+2;
        }
        প্রত্যাগম strings.data()+offset;
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
        প্রত্যাগম;
    }
    int32_t length=s.length();
    যদি(length>0xffff) {
        // Too long: We store the length in 1 or 2 bytes.
        errorCode=U_INDEX_OUTOFBOUNDS_ERROR;
        প্রত্যাগম;
    }
    int32_t offset=strings.length();
    যদি(length>0xff) {
        offset=~offset;
        strings.append(static_cast<বর্ণ>(length >> 8), errorCode);
    }
    strings.append(static_cast<বর্ণ>(length), errorCode);
    stringOffset=offset;
    value=val;
    strings.append(s, errorCode);
}

int32_t
BytesTrieElement::compareStringTo(ধ্রুব BytesTrieElement &other, ধ্রুব CharString &strings) ধ্রুব {
    // TODO: add StringPiece::compare(), see ticket #8187
    StringPiece thisString=getString(strings);
    StringPiece otherString=other.getString(strings);
    int32_t lengthDiff=thisString.length()-otherString.length();
    int32_t commonLength;
    যদি(lengthDiff<=0) {
        commonLength=thisString.length();
    } অন্যথা {
        commonLength=otherString.length();
    }
    int32_t diff=uprv_memcmp(thisString.data(), otherString.data(), commonLength);
    প্রত্যাগম diff!=0 ? diff : lengthDiff;
}

BytesTrieBuilder::BytesTrieBuilder(UErrorCode &errorCode)
        : strings(nullptr), elements(nullptr), elementsCapacity(0), elementsLength(0),
          bytes(nullptr), bytesCapacity(0), bytesLength(0) {
    যদি(U_FAILURE(errorCode)) {
        প্রত্যাগম;
    }
    strings=নূতন CharString();
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
        প্রত্যাগম *এতত্;
    }
    যদি(bytesLength>0) {
        // Cannot add elements after building.
        errorCode=U_NO_WRITE_PERMISSION;
        প্রত্যাগম *এতত্;
    }
    যদি(elementsLength==elementsCapacity) {
        int32_t newCapacity;
        যদি(elementsCapacity==0) {
            newCapacity=1024;
        } অন্যথা {
            newCapacity=4*elementsCapacity;
        }
        BytesTrieElement *newElements=নূতন BytesTrieElement[newCapacity];
        যদি(newElements==nullptr) {
            errorCode=U_MEMORY_ALLOCATION_ERROR;
            প্রত্যাগম *এতত্; // error instead of dereferencing null
        }
        যদি(elementsLength>0) {
            uprv_memcpy(newElements, elements, (size_t)elementsLength*পরিমাণ(BytesTrieElement));
        }
        delete[] elements;
        elements=newElements;
        elementsCapacity=newCapacity;
    }
    elements[elementsLength++].setTo(s, value, *strings, errorCode);
    প্রত্যাগম *এতত্;
}

U_CDECL_BEGIN

স্থির int32_t U_CALLCONV
compareElementStrings(ধ্রুব শূন্য *context, ধ্রুব শূন্য *left, ধ্রুব শূন্য *ডান) {
    ধ্রুব CharString *strings=static_cast<ধ্রুব CharString *>(context);
    ধ্রুব BytesTrieElement *বাম_Element=static_cast<ধ্রুব BytesTrieElement *>(left);
    ধ্রুব BytesTrieElement *ডান_Element=static_cast<ধ্রুব BytesTrieElement *>(ডান);
    প্রত্যাগম বাম_Element->compareStringTo(*ডান_Element, *strings);
}

U_CDECL_END

BytesTrie *
BytesTrieBuilder::build(UStringTrieBuildOption buildOption, UErrorCode &errorCode) {
    buildBytes(buildOption, errorCode);
    BytesTrie *newTrie=nullptr;
    যদি(U_SUCCESS(errorCode)) {
        newTrie=নূতন BytesTrie(bytes, bytes+(bytesCapacity-bytesLength));
        যদি(newTrie==nullptr) {
            errorCode=U_MEMORY_ALLOCATION_ERROR;
        } অন্যথা {
            bytes=nullptr;  // The new trie now owns the array.
            bytesCapacity=0;
        }
    }
    প্রত্যাগম newTrie;
}

StringPiece
BytesTrieBuilder::buildStringPiece(UStringTrieBuildOption buildOption,
                                   UErrorCode& errorCode) U_LIFETIME_BOUND {
    buildBytes(buildOption, errorCode);
    StringPiece result;
    যদি(U_SUCCESS(errorCode)) {
        result.set(bytes+(bytesCapacity-bytesLength), bytesLength);
    }
    প্রত্যাগম result;
}

শূন্য
BytesTrieBuilder::buildBytes(UStringTrieBuildOption buildOption, UErrorCode &errorCode) {
    যদি(U_FAILURE(errorCode)) {
        প্রত্যাগম;
    }
    যদি(bytes!=nullptr && bytesLength>0) {
        // Already built.
        প্রত্যাগম;
    }
    যদি(bytesLength==0) {
        যদি(elementsLength==0) {
            errorCode=U_INDEX_OUTOFBOUNDS_ERROR;
            প্রত্যাগম;
        }
        uprv_sortArray(elements, elementsLength, static_cast<int32_t>(পরিমাণ(BytesTrieElement)),
                      compareElementStrings, strings,
                      অসত্য,  // need not be a stable sort
                      &errorCode);
        যদি(U_FAILURE(errorCode)) {
            প্রত্যাগম;
        }
        // Duplicate strings are not allowed.
        StringPiece prev=elements[0].getString(*strings);
        প্রতি(int32_t i=1; i<elementsLength; ++i) {
            StringPiece current=elements[i].getString(*strings);
            যদি(prev==current) {
                errorCode=U_ILLEGAL_ARGUMENT_ERROR;
                প্রত্যাগম;
            }
            prev=current;
        }
    }
    // Create and byte-serialize the trie for the elements.
    bytesLength=0;
    int32_t capacity=strings->length();
    যদি(capacity<1024) {
        capacity=1024;
    }
    যদি(bytesCapacity<capacity) {
        uprv_free(bytes);
        bytes=static_cast<বর্ণ *>(uprv_malloc(capacity));
        যদি(bytes==nullptr) {
            errorCode=U_MEMORY_ALLOCATION_ERROR;
            bytesCapacity=0;
            প্রত্যাগম;
        }
        bytesCapacity=capacity;
    }
    StringTrieBuilder::build(buildOption, elementsLength, errorCode);
    যদি(bytes==nullptr) {
        errorCode=U_MEMORY_ALLOCATION_ERROR;
    }
}

BytesTrieBuilder&
BytesTrieBuilder::clear() U_LIFETIME_BOUND {
    strings->clear();
    elementsLength=0;
    bytesLength=0;
    প্রত্যাগম *এতত্;
}

int32_t
BytesTrieBuilder::getElementStringLength(int32_t i) ধ্রুব {
    প্রত্যাগম elements[i].getStringLength(*strings);
}

char16_t
BytesTrieBuilder::getElementUnit(int32_t i, int32_t byteIndex) ধ্রুব {
    প্রত্যাগম static_cast<uint8_t>(elements[i].charAt(byteIndex, *strings));
}

int32_t
BytesTrieBuilder::get_Element_মান(int32_t i) ধ্রুব {
    প্রত্যাগম elements[i].get_মান();
}

int32_t
BytesTrieBuilder::getLimitOfLinearMatch(int32_t first, int32_t last, int32_t byteIndex) ধ্রুব {
    ধ্রুব BytesTrieElement &firstElement=elements[first];
    ধ্রুব BytesTrieElement &lastElement=elements[last];
    int32_t minStringLength=firstElement.getStringLength(*strings);
    যাবত্(++byteIndex<minStringLength &&
            firstElement.charAt(byteIndex, *strings)==
            lastElement.charAt(byteIndex, *strings)) {}
    প্রত্যাগম byteIndex;
}

int32_t
BytesTrieBuilder::গুনতি_Element_Units(int32_t start, int32_t limit, int32_t byteIndex) ধ্রুব {
    int32_t length=0;  // Number of different bytes at byteIndex.
    int32_t i=start;
    কুরু {
        বর্ণ byte=elements[i++].charAt(byteIndex, *strings);
        যাবত্(i<limit && byte==elements[i].charAt(byteIndex, *strings)) {
            ++i;
        }
        ++length;
    } যাবত্(i<limit);
    প্রত্যাগম length;
}

int32_t
BytesTrieBuilder::skipElementsBySomeUnits(int32_t i, int32_t byteIndex, int32_t count) ধ্রুব {
    কুরু {
        বর্ণ byte=elements[i++].charAt(byteIndex, *strings);
        যাবত্(byte==elements[i].charAt(byteIndex, *strings)) {
            ++i;
        }
    } যাবত্(--count>0);
    প্রত্যাগম i;
}

int32_t
BytesTrieBuilder::indexOfElementWithNextUnit(int32_t i, int32_t byteIndex, char16_t byte) ধ্রুব {
    বর্ণ b = static_cast<বর্ণ>(byte);
    যাবত্(b==elements[i].charAt(byteIndex, *strings)) {
        ++i;
    }
    প্রত্যাগম i;
}

BytesTrieBuilder::BTLinear_Match_গ্রন্থি::BTLinear_Match_গ্রন্থি(ধ্রুব বর্ণ *bytes, int32_t len, Node *next_গ্রন্থি)
        : Linear_Match_গ্রন্থি(len, next_গ্রন্থি), s(bytes) {
    hash=static_cast<int32_t>(
        static_cast<uint32_t>(hash)*37u + static_cast<uint32_t>(ustr_hashCharsN(bytes, len)));
}

তার্কিক
BytesTrieBuilder::BTLinear_Match_গ্রন্থি::operator==(ধ্রুব Node &other) ধ্রুব {
    যদি(এতত্==&other) {
        প্রত্যাগম সত্য;
    }
    যদি(!Linear_Match_গ্রন্থি::operator==(other)) {
        প্রত্যাগম অসত্য;
    }
    ধ্রুব BTLinear_Match_গ্রন্থি &o=static_cast<ধ্রুব BTLinear_Match_গ্রন্থি &>(other);
    প্রত্যাগম 0==uprv_memcmp(s, o.s, length);
}

শূন্য
BytesTrieBuilder::BTLinear_Match_গ্রন্থি::লিখ(StringTrieBuilder &builder) {
    BytesTrieBuilder &b=static_cast<BytesTrieBuilder &>(builder);
    next->লিখ(builder);
    b.লিখ(s, length);
    offset=b.লিখ(b.getMinLinearMatch()+length-1);
}

StringTrieBuilder::Node *
BytesTrieBuilder::create_Linear_Match_গ্রন্থি(int32_t i, int32_t byteIndex, int32_t length,
                                        Node *next_গ্রন্থি) ধ্রুব {
    প্রত্যাগম নূতন BTLinear_Match_গ্রন্থি(
            elements[i].getString(*strings).data()+byteIndex,
            length,
            next_গ্রন্থি);
}

UBool
BytesTrieBuilder::ensureCapacity(int32_t length) {
    যদি(bytes==nullptr) {
        প্রত্যাগম অসত্য;  // previous memory allocation had failed
    }
    যদি(length>bytesCapacity) {
        int32_t newCapacity=bytesCapacity;
        কুরু {
            newCapacity*=2;
        } যাবত্(newCapacity<=length);
        বর্ণ *newBytes=static_cast<বর্ণ *>(uprv_malloc(newCapacity));
        যদি(newBytes==nullptr) {
            // unable to allocate memory
            uprv_free(bytes);
            bytes=nullptr;
            bytesCapacity=0;
            প্রত্যাগম অসত্য;
        }
        uprv_memcpy(newBytes+(newCapacity-bytesLength),
                    bytes+(bytesCapacity-bytesLength), bytesLength);
        uprv_free(bytes);
        bytes=newBytes;
        bytesCapacity=newCapacity;
    }
    প্রত্যাগম সত্য;
}

int32_t
BytesTrieBuilder::লিখ(int32_t byte) {
    int32_t newLength=bytesLength+1;
    যদি(ensureCapacity(newLength)) {
        bytesLength=newLength;
        bytes[bytesCapacity - bytesLength] = static_cast<বর্ণ>(byte);
    }
    প্রত্যাগম bytesLength;
}

int32_t
BytesTrieBuilder::লিখ(ধ্রুব বর্ণ *b, int32_t length) {
    int32_t newLength=bytesLength+length;
    যদি(ensureCapacity(newLength)) {
        bytesLength=newLength;
        uprv_memcpy(bytes+(bytesCapacity-bytesLength), b, length);
    }
    প্রত্যাগম bytesLength;
}

int32_t
BytesTrieBuilder::writeElementUnits(int32_t i, int32_t byteIndex, int32_t length) {
    প্রত্যাগম লিখ(elements[i].getString(*strings).data()+byteIndex, length);
}

int32_t
BytesTrieBuilder::write_মান_And_Final(int32_t i, UBool isFinal) {
    যদি(0<=i && i<=BytesTrie::k_Max_One_Byte_মান) {
        প্রত্যাগম লিখ(((BytesTrie::k_Min_One_Byte_মান_Lead+i)<<1)|isFinal);
    }
    বর্ণ intBytes[5];
    int32_t length=1;
    যদি(i<0 || i>0xffffff) {
        intBytes[0] = static_cast<বর্ণ>(BytesTrie::k_Five_Byte_মান_Lead);
        intBytes[1] = static_cast<বর্ণ>(static_cast<uint32_t>(i) >> 24);
        intBytes[2] = static_cast<বর্ণ>(static_cast<uint32_t>(i) >> 16);
        intBytes[3] = static_cast<বর্ণ>(static_cast<uint32_t>(i) >> 8);
        intBytes[4] = static_cast<বর্ণ>(i);
        length=5;
    // } else if(i<=BytesTrie::kMaxOneByteValue) {
    //     intBytes[0]=(char)(BytesTrie::kMinOneByteValueLead+i);
    } অন্যথা {
        যদি(i<=BytesTrie::k_Max_Two_Byte_মান) {
            intBytes[0] = static_cast<বর্ণ>(BytesTrie::k_Min_Two_Byte_মান_Lead + (i >> 8));
        } অন্যথা {
            যদি(i<=BytesTrie::k_Max_Three_Byte_মান) {
                intBytes[0] = static_cast<বর্ণ>(BytesTrie::k_Min_Three_Byte_মান_Lead + (i >> 16));
            } অন্যথা {
                intBytes[0] = static_cast<বর্ণ>(BytesTrie::k_Four_Byte_মান_Lead);
                intBytes[1] = static_cast<বর্ণ>(i >> 16);
                length=2;
            }
            intBytes[length++] = static_cast<বর্ণ>(i >> 8);
        }
        intBytes[length++] = static_cast<বর্ণ>(i);
    }
    intBytes[0] = static_cast<বর্ণ>((intBytes[0] << 1) | isFinal);
    প্রত্যাগম লিখ(intBytes, length);
}

int32_t
BytesTrieBuilder::write_মান_And_Type(UBool has_মান, int32_t value, int32_t গ্রন্থি) {
    int32_t offset=লিখ(গ্রন্থি);
    যদি(has_মান) {
        offset=write_মান_And_Final(value, অসত্য);
    }
    প্রত্যাগম offset;
}

int32_t
BytesTrieBuilder::writeDeltaTo(int32_t jumpTarget) {
    int32_t i=bytesLength-jumpTarget;
    U_ASSERT(i>=0);
    যদি(i<=BytesTrie::kMaxOneByteDelta) {
        প্রত্যাগম লিখ(i);
    } অন্যথা {
        বর্ণ intBytes[5];
        প্রত্যাগম লিখ(intBytes, internalEncodeDelta(i, intBytes));
    }
}

int32_t
BytesTrieBuilder::internalEncodeDelta(int32_t i, বর্ণ intBytes[]) {
    U_ASSERT(i>=0);
    যদি(i<=BytesTrie::kMaxOneByteDelta) {
        intBytes[0] = static_cast<বর্ণ>(i);
        প্রত্যাগম 1;
    }
    int32_t length=1;
    যদি(i<=BytesTrie::kMaxTwoByteDelta) {
        intBytes[0] = static_cast<বর্ণ>(BytesTrie::kMinTwoByteDeltaLead + (i >> 8));
    } অন্যথা {
        যদি(i<=BytesTrie::kMaxThreeByteDelta) {
            intBytes[0] = static_cast<বর্ণ>(BytesTrie::kMinThreeByteDeltaLead + (i >> 16));
        } অন্যথা {
            যদি(i<=0xffffff) {
                intBytes[0] = static_cast<বর্ণ>(BytesTrie::kFourByteDeltaLead);
            } অন্যথা {
                intBytes[0] = static_cast<বর্ণ>(BytesTrie::kFiveByteDeltaLead);
                intBytes[1] = static_cast<বর্ণ>(i >> 24);
                length=2;
            }
            intBytes[length++] = static_cast<বর্ণ>(i >> 16);
        }
        intBytes[length++] = static_cast<বর্ণ>(i >> 8);
    }
    intBytes[length++] = static_cast<বর্ণ>(i);
    প্রত্যাগম length;
}

U_NAMESPACE_END
