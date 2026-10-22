// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
*******************************************************************************
*   Copyright (C) 2010-2012, International Business Machines
*   Corporation and others.  All Rights Reserved.
*******************************************************************************
*   file name:  ucharstriebuilder.h
*   encoding:   UTF-8
*   tab size:   8 (not used)
*   indentation:4
*
*   created on: 2010nov14
*   created by: Markus W. Scherer
*/

#অন্তর্ভাব "unicode/utypes.h"
#অন্তর্ভাব "unicode/ucharstrie.h"
#অন্তর্ভাব "unicode/ucharstriebuilder.h"
#অন্তর্ভাব "unicode/unistr.h"
#অন্তর্ভাব "unicode/ustring.h"
#অন্তর্ভাব "cmemory.h"
#অন্তর্ভাব "uarrsort.h"
#অন্তর্ভাব "uassert.h"
#অন্তর্ভাব "uhash.h"
#অন্তর্ভাব "ustr_imp.h"

U_NAMESPACE_BEGIN

/*
 * Note: This builder implementation stores (string, value) pairs with full copies
 * of the 16-bit-unit sequences, until the UCharsTrie is built.
 * It might(!) take less memory if we collected the data in a temporary, dynamic trie.
 */

বর্গ UCharsTrieElement : সার্বজনিক UMemory {
সার্বজনিক:
    // Use compiler's default constructor, initializes nothing.

    শূন্য setTo(ধ্রুব UnicodeString &s, int32_t val, UnicodeString &strings, UErrorCode &errorCode);

    UnicodeString getString(ধ্রুব UnicodeString& strings U_LIFETIME_BOUND) ধ্রুব {
        int32_t length=strings[stringOffset];
        প্রত্যাগম strings.tempSubString(stringOffset+1, length);
    }
    int32_t getStringLength(ধ্রুব UnicodeString &strings) ধ্রুব {
        প্রত্যাগম strings[stringOffset];
    }

    char16_t charAt(int32_t index, ধ্রুব UnicodeString &strings) ধ্রুব {
        প্রত্যাগম strings[stringOffset+1+index];
    }

    int32_t get_মান() ধ্রুব { প্রত্যাগম value; }

    int32_t compareStringTo(ধ্রুব UCharsTrieElement &o, ধ্রুব UnicodeString &strings) ধ্রুব;

গুপ্ত:
    // The first strings unit contains the string length.
    // (Compared with a stringLength field here, this saves 2 bytes per string.)
    int32_t stringOffset;
    int32_t value;
};

শূন্য
UCharsTrieElement::setTo(ধ্রুব UnicodeString &s, int32_t val,
                         UnicodeString &strings, UErrorCode &errorCode) {
    যদি(U_FAILURE(errorCode)) {
        প্রত্যাগম;
    }
    int32_t length=s.length();
    যদি(length>0xffff) {
        // Too long: We store the length in 1 unit.
        errorCode=U_INDEX_OUTOFBOUNDS_ERROR;
        প্রত্যাগম;
    }
    stringOffset=strings.length();
    strings.append(static_cast<char16_t>(length));
    value=val;
    strings.append(s);
}

int32_t
UCharsTrieElement::compareStringTo(ধ্রুব UCharsTrieElement &other, ধ্রুব UnicodeString &strings) ধ্রুব {
    প্রত্যাগম getString(strings).compare(other.getString(strings));
}

UCharsTrieBuilder::UCharsTrieBuilder(UErrorCode & /*errorCode*/)
        : elements(nullptr), elementsCapacity(0), elementsLength(0),
          uchars(nullptr), ucharsCapacity(0), ucharsLength(0) {}

UCharsTrieBuilder::~UCharsTrieBuilder() {
    delete[] elements;
    uprv_free(uchars);
}

UCharsTrieBuilder&
UCharsTrieBuilder::add(ধ্রুব UnicodeString& s,
                       int32_t value,
                       UErrorCode& errorCode) U_LIFETIME_BOUND {
    যদি(U_FAILURE(errorCode)) {
        প্রত্যাগম *এতত্;
    }
    যদি(ucharsLength>0) {
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
        UCharsTrieElement *newElements=নূতন UCharsTrieElement[newCapacity];
        যদি(newElements==nullptr) {
            errorCode=U_MEMORY_ALLOCATION_ERROR;
            প্রত্যাগম *এতত্;
        }
        যদি(elementsLength>0) {
            uprv_memcpy(newElements, elements, (size_t)elementsLength*পরিমাণ(UCharsTrieElement));
        }
        delete[] elements;
        elements=newElements;
        elementsCapacity=newCapacity;
    }
    elements[elementsLength++].setTo(s, value, strings, errorCode);
    যদি(U_SUCCESS(errorCode) && strings.isBogus()) {
        errorCode=U_MEMORY_ALLOCATION_ERROR;
    }
    প্রত্যাগম *এতত্;
}

U_CDECL_BEGIN

স্থির int32_t U_CALLCONV
compareElementStrings(ধ্রুব শূন্য *context, ধ্রুব শূন্য *left, ধ্রুব শূন্য *ডান) {
    ধ্রুব UnicodeString *strings=static_cast<ধ্রুব UnicodeString *>(context);
    ধ্রুব UCharsTrieElement *বাম_Element=static_cast<ধ্রুব UCharsTrieElement *>(left);
    ধ্রুব UCharsTrieElement *ডান_Element=static_cast<ধ্রুব UCharsTrieElement *>(ডান);
    প্রত্যাগম বাম_Element->compareStringTo(*ডান_Element, *strings);
}

U_CDECL_END

UCharsTrie *
UCharsTrieBuilder::build(UStringTrieBuildOption buildOption, UErrorCode &errorCode) {
    buildUChars(buildOption, errorCode);
    UCharsTrie *newTrie=nullptr;
    যদি(U_SUCCESS(errorCode)) {
        newTrie=নূতন UCharsTrie(uchars, uchars+(ucharsCapacity-ucharsLength));
        যদি(newTrie==nullptr) {
            errorCode=U_MEMORY_ALLOCATION_ERROR;
        } অন্যথা {
            uchars=nullptr;  // The new trie now owns the array.
            ucharsCapacity=0;
        }
    }
    প্রত্যাগম newTrie;
}

UnicodeString&
UCharsTrieBuilder::buildUnicodeString(UStringTrieBuildOption buildOption,
                                      UnicodeString& result U_LIFETIME_BOUND,
                                      UErrorCode& errorCode) U_LIFETIME_BOUND {
    buildUChars(buildOption, errorCode);
    যদি(U_SUCCESS(errorCode)) {
        result.setTo(অসত্য, uchars+(ucharsCapacity-ucharsLength), ucharsLength);
    }
    প্রত্যাগম result;
}

শূন্য
UCharsTrieBuilder::buildUChars(UStringTrieBuildOption buildOption, UErrorCode &errorCode) {
    যদি(U_FAILURE(errorCode)) {
        প্রত্যাগম;
    }
    যদি(uchars!=nullptr && ucharsLength>0) {
        // Already built.
        প্রত্যাগম;
    }
    যদি(ucharsLength==0) {
        যদি(elementsLength==0) {
            errorCode=U_INDEX_OUTOFBOUNDS_ERROR;
            প্রত্যাগম;
        }
        যদি(strings.isBogus()) {
            errorCode=U_MEMORY_ALLOCATION_ERROR;
            প্রত্যাগম;
        }
        uprv_sortArray(elements, elementsLength, static_cast<int32_t>(পরিমাণ(UCharsTrieElement)),
                      compareElementStrings, &strings,
                      অসত্য,  // need not be a stable sort
                      &errorCode);
        যদি(U_FAILURE(errorCode)) {
            প্রত্যাগম;
        }
        // Duplicate strings are not allowed.
        UnicodeString prev=elements[0].getString(strings);
        প্রতি(int32_t i=1; i<elementsLength; ++i) {
            UnicodeString current=elements[i].getString(strings);
            যদি(prev==current) {
                errorCode=U_ILLEGAL_ARGUMENT_ERROR;
                প্রত্যাগম;
            }
            prev.fastCopyFrom(current);
        }
    }
    // Create and char16_t-serialize the trie for the elements.
    ucharsLength=0;
    int32_t capacity=strings.length();
    যদি(capacity<1024) {
        capacity=1024;
    }
    যদি(ucharsCapacity<capacity) {
        uprv_free(uchars);
        uchars=static_cast<char16_t *>(uprv_malloc(capacity*2));
        যদি(uchars==nullptr) {
            errorCode=U_MEMORY_ALLOCATION_ERROR;
            ucharsCapacity=0;
            প্রত্যাগম;
        }
        ucharsCapacity=capacity;
    }
    StringTrieBuilder::build(buildOption, elementsLength, errorCode);
    যদি(uchars==nullptr) {
        errorCode=U_MEMORY_ALLOCATION_ERROR;
    }
}

int32_t
UCharsTrieBuilder::getElementStringLength(int32_t i) ধ্রুব {
    প্রত্যাগম elements[i].getStringLength(strings);
}

char16_t
UCharsTrieBuilder::getElementUnit(int32_t i, int32_t unitIndex) ধ্রুব {
    প্রত্যাগম elements[i].charAt(unitIndex, strings);
}

int32_t
UCharsTrieBuilder::get_Element_মান(int32_t i) ধ্রুব {
    প্রত্যাগম elements[i].get_মান();
}

int32_t
UCharsTrieBuilder::getLimitOfLinearMatch(int32_t first, int32_t last, int32_t unitIndex) ধ্রুব {
    ধ্রুব UCharsTrieElement &firstElement=elements[first];
    ধ্রুব UCharsTrieElement &lastElement=elements[last];
    int32_t minStringLength=firstElement.getStringLength(strings);
    যাবত্(++unitIndex<minStringLength &&
            firstElement.charAt(unitIndex, strings)==
            lastElement.charAt(unitIndex, strings)) {}
    প্রত্যাগম unitIndex;
}

int32_t
UCharsTrieBuilder::গুনতি_Element_Units(int32_t start, int32_t limit, int32_t unitIndex) ধ্রুব {
    int32_t length=0;  // Number of different units at unitIndex.
    int32_t i=start;
    কুরু {
        char16_t unit=elements[i++].charAt(unitIndex, strings);
        যাবত্(i<limit && unit==elements[i].charAt(unitIndex, strings)) {
            ++i;
        }
        ++length;
    } যাবত্(i<limit);
    প্রত্যাগম length;
}

int32_t
UCharsTrieBuilder::skipElementsBySomeUnits(int32_t i, int32_t unitIndex, int32_t count) ধ্রুব {
    কুরু {
        char16_t unit=elements[i++].charAt(unitIndex, strings);
        যাবত্(unit==elements[i].charAt(unitIndex, strings)) {
            ++i;
        }
    } যাবত্(--count>0);
    প্রত্যাগম i;
}

int32_t
UCharsTrieBuilder::indexOfElementWithNextUnit(int32_t i, int32_t unitIndex, char16_t unit) ধ্রুব {
    যাবত্(unit==elements[i].charAt(unitIndex, strings)) {
        ++i;
    }
    প্রত্যাগম i;
}

UCharsTrieBuilder::UCTLinear_Match_গ্রন্থি::UCTLinear_Match_গ্রন্থি(ধ্রুব char16_t *units, int32_t len, Node *next_গ্রন্থি)
        : Linear_Match_গ্রন্থি(len, next_গ্রন্থি), s(units) {
    hash=hash*37u+ustr_hashUCharsN(units, len);
}

তার্কিক
UCharsTrieBuilder::UCTLinear_Match_গ্রন্থি::operator==(ধ্রুব Node &other) ধ্রুব {
    যদি(এতত্==&other) {
        প্রত্যাগম সত্য;
    }
    যদি(!Linear_Match_গ্রন্থি::operator==(other)) {
        প্রত্যাগম অসত্য;
    }
    ধ্রুব UCTLinear_Match_গ্রন্থি &o=static_cast<ধ্রুব UCTLinear_Match_গ্রন্থি &>(other);
    প্রত্যাগম 0==u_memcmp(s, o.s, length);
}

শূন্য
UCharsTrieBuilder::UCTLinear_Match_গ্রন্থি::লিখ(StringTrieBuilder &builder) {
    UCharsTrieBuilder &b=(UCharsTrieBuilder &)builder;
    next->লিখ(builder);
    b.লিখ(s, length);
    offset=b.write_মান_And_Type(has_মান, value, b.getMinLinearMatch()+length-1);
}

StringTrieBuilder::Node *
UCharsTrieBuilder::create_Linear_Match_গ্রন্থি(int32_t i, int32_t unitIndex, int32_t length,
                                         Node *next_গ্রন্থি) ধ্রুব {
    প্রত্যাগম নূতন UCTLinear_Match_গ্রন্থি(
            elements[i].getString(strings).getBuffer()+unitIndex,
            length,
            next_গ্রন্থি);
}

UBool
UCharsTrieBuilder::ensureCapacity(int32_t length) {
    যদি(uchars==nullptr) {
        প্রত্যাগম অসত্য;  // previous memory allocation had failed
    }
    যদি(length>ucharsCapacity) {
        int32_t newCapacity=ucharsCapacity;
        কুরু {
            newCapacity*=2;
        } যাবত্(newCapacity<=length);
        char16_t *newUChars=static_cast<char16_t *>(uprv_malloc(newCapacity*2));
        যদি(newUChars==nullptr) {
            // unable to allocate memory
            uprv_free(uchars);
            uchars=nullptr;
            ucharsCapacity=0;
            প্রত্যাগম অসত্য;
        }
        u_memcpy(newUChars+(newCapacity-ucharsLength),
                 uchars+(ucharsCapacity-ucharsLength), ucharsLength);
        uprv_free(uchars);
        uchars=newUChars;
        ucharsCapacity=newCapacity;
    }
    প্রত্যাগম সত্য;
}

int32_t
UCharsTrieBuilder::লিখ(int32_t unit) {
    int32_t newLength=ucharsLength+1;
    যদি(ensureCapacity(newLength)) {
        ucharsLength=newLength;
        uchars[ucharsCapacity - ucharsLength] = static_cast<char16_t>(unit);
    }
    প্রত্যাগম ucharsLength;
}

int32_t
UCharsTrieBuilder::লিখ(ধ্রুব char16_t *s, int32_t length) {
    int32_t newLength=ucharsLength+length;
    যদি(ensureCapacity(newLength)) {
        ucharsLength=newLength;
        u_memcpy(uchars+(ucharsCapacity-ucharsLength), s, length);
    }
    প্রত্যাগম ucharsLength;
}

int32_t
UCharsTrieBuilder::writeElementUnits(int32_t i, int32_t unitIndex, int32_t length) {
    প্রত্যাগম লিখ(elements[i].getString(strings).getBuffer()+unitIndex, length);
}

int32_t
UCharsTrieBuilder::write_মান_And_Final(int32_t i, UBool isFinal) {
    যদি(0<=i && i<=UCharsTrie::k_Max_One_Unit_মান) {
        প্রত্যাগম লিখ(i|(isFinal<<15));
    }
    char16_t intUnits[3];
    int32_t length;
    যদি(i<0 || i>UCharsTrie::k_Max_Two_Unit_মান) {
        intUnits[0] = static_cast<char16_t>(UCharsTrie::k_Three_Unit_মান_Lead);
        intUnits[1] = static_cast<char16_t>(static_cast<uint32_t>(i) >> 16);
        intUnits[2] = static_cast<char16_t>(i);
        length=3;
    // } else if(i<=UCharsTrie::kMaxOneUnitValue) {
    //     intUnits[0]=(char16_t)(i);
    //     length=1;
    } অন্যথা {
        intUnits[0] = static_cast<char16_t>(UCharsTrie::k_Min_Two_Unit_মান_Lead + (i >> 16));
        intUnits[1] = static_cast<char16_t>(i);
        length=2;
    }
    intUnits[0] = static_cast<char16_t>(intUnits[0] | (isFinal << 15));
    প্রত্যাগম লিখ(intUnits, length);
}

int32_t
UCharsTrieBuilder::write_মান_And_Type(UBool has_মান, int32_t value, int32_t গ্রন্থি) {
    যদি(!has_মান) {
        প্রত্যাগম লিখ(গ্রন্থি);
    }
    char16_t intUnits[3];
    int32_t length;
    যদি(value<0 || value>UCharsTrie::k_Max_Two_Unit_গ্রন্থি_মান) {
        intUnits[0] = static_cast<char16_t>(UCharsTrie::k_Three_Unit_গ্রন্থি_মান_Lead);
        intUnits[1] = static_cast<char16_t>(static_cast<uint32_t>(value) >> 16);
        intUnits[2] = static_cast<char16_t>(value);
        length=3;
    } অন্যথা যদি(value<=UCharsTrie::k_Max_One_Unit_গ্রন্থি_মান) {
        intUnits[0] = static_cast<char16_t>((value + 1) << 6);
        length=1;
    } অন্যথা {
        intUnits[0] = static_cast<char16_t>(UCharsTrie::k_Min_Two_Unit_গ্রন্থি_মান_Lead + ((value >> 10) & 0x7fc0));
        intUnits[1] = static_cast<char16_t>(value);
        length=2;
    }
    intUnits[0] |= static_cast<char16_t>(গ্রন্থি);
    প্রত্যাগম লিখ(intUnits, length);
}

int32_t
UCharsTrieBuilder::writeDeltaTo(int32_t jumpTarget) {
    int32_t i=ucharsLength-jumpTarget;
    U_ASSERT(i>=0);
    যদি(i<=UCharsTrie::kMaxOneUnitDelta) {
        প্রত্যাগম লিখ(i);
    }
    char16_t intUnits[3];
    int32_t length;
    যদি(i<=UCharsTrie::kMaxTwoUnitDelta) {
        intUnits[0] = static_cast<char16_t>(UCharsTrie::kMinTwoUnitDeltaLead + (i >> 16));
        length=1;
    } অন্যথা {
        intUnits[0] = static_cast<char16_t>(UCharsTrie::kThreeUnitDeltaLead);
        intUnits[1] = static_cast<char16_t>(i >> 16);
        length=2;
    }
    intUnits[length++] = static_cast<char16_t>(i);
    প্রত্যাগম লিখ(intUnits, length);
}

U_NAMESPACE_END
