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

#অন্তর্ভাৱ "unicode/utypes.h"
#অন্তর্ভাৱ "unicode/ucharstrie.h"
#অন্তর্ভাৱ "unicode/ucharstriebuilder.h"
#অন্তর্ভাৱ "unicode/unistr.h"
#অন্তর্ভাৱ "unicode/ustring.h"
#অন্তর্ভাৱ "cmemory.h"
#অন্তর্ভাৱ "uarrsort.h"
#অন্তর্ভাৱ "uassert.h"
#অন্তর্ভাৱ "uhash.h"
#অন্তর্ভাৱ "ustr_imp.h"

U_NAMESPACE_BEGIN

/*
 * Note: This builder implementation stores (string, value) pairs with full copies
 * of the 16-bit-unit sequences, until the UCharsTrie is built.
 * It might(!) take less memory if we collected the data in a temporary, dynamic trie.
 */

শ্ৰেণী UCharsTrieElement : সার্ৱজনিক UMemory {
সার্ৱজনিক:
    // Use compiler's default constructor, initializes nothing.

    শূন্য setTo(ধ্রুৱ UnicodeString &s, int32_t val, UnicodeString &strings, UErrorCode &errorCode);

    UnicodeString getString(ধ্রুৱ UnicodeString& strings U_LIFETIME_BOUND) ধ্রুৱ {
        int32_t length=strings[stringOffset];
        ঘূৰাই strings.tempSubString(stringOffset+1, length);
    }
    int32_t getStringLength(ধ্রুৱ UnicodeString &strings) ধ্রুৱ {
        ঘূৰাই strings[stringOffset];
    }

    char16_t charAt(int32_t index, ধ্রুৱ UnicodeString &strings) ধ্রুৱ {
        ঘূৰাই strings[stringOffset+1+index];
    }

    int32_t get_মান() ধ্রুৱ { ঘূৰাই value; }

    int32_t compareStringTo(ধ্রুৱ UCharsTrieElement &o, ধ্রুৱ UnicodeString &strings) ধ্রুৱ;

গুপ্ত:
    // The first strings unit contains the string length.
    // (Compared with a stringLength field here, this saves 2 bytes per string.)
    int32_t stringOffset;
    int32_t value;
};

শূন্য
UCharsTrieElement::setTo(ধ্রুৱ UnicodeString &s, int32_t val,
                         UnicodeString &strings, UErrorCode &errorCode) {
    যদি(U_FAILURE(errorCode)) {
        ঘূৰাই;
    }
    int32_t length=s.length();
    যদি(length>0xffff) {
        // Too long: We store the length in 1 unit.
        errorCode=U_INDEX_OUTOFBOUNDS_ERROR;
        ঘূৰাই;
    }
    stringOffset=strings.length();
    strings.append(static_cast<char16_t>(length));
    value=val;
    strings.append(s);
}

int32_t
UCharsTrieElement::compareStringTo(ধ্রুৱ UCharsTrieElement &other, ধ্রুৱ UnicodeString &strings) ধ্রুৱ {
    ঘূৰাই getString(strings).compare(other.getString(strings));
}

UCharsTrieBuilder::UCharsTrieBuilder(UErrorCode & /*errorCode*/)
        : elements(nullptr), elementsCapacity(0), elementsLength(0),
          uchars(nullptr), ucharsCapacity(0), ucharsLength(0) {}

UCharsTrieBuilder::~UCharsTrieBuilder() {
    delete[] elements;
    uprv_free(uchars);
}

UCharsTrieBuilder&
UCharsTrieBuilder::add(ধ্রুৱ UnicodeString& s,
                       int32_t value,
                       UErrorCode& errorCode) U_LIFETIME_BOUND {
    যদি(U_FAILURE(errorCode)) {
        ঘূৰাই *এতত্;
    }
    যদি(ucharsLength>0) {
        // Cannot add elements after building.
        errorCode=U_NO_WRITE_PERMISSION;
        ঘূৰাই *এতত্;
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
            ঘূৰাই *এতত্;
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
    ঘূৰাই *এতত্;
}

U_CDECL_BEGIN

স্থির int32_t U_CALLCONV
compareElementStrings(ধ্রুৱ শূন্য *context, ধ্রুৱ শূন্য *left, ধ্রুৱ শূন্য *ডান) {
    ধ্রুৱ UnicodeString *strings=static_cast<ধ্রুৱ UnicodeString *>(context);
    ধ্রুৱ UCharsTrieElement *বাম_Element=static_cast<ধ্রুৱ UCharsTrieElement *>(left);
    ধ্রুৱ UCharsTrieElement *ডান_Element=static_cast<ধ্রুৱ UCharsTrieElement *>(ডান);
    ঘূৰাই বাম_Element->compareStringTo(*ডান_Element, *strings);
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
    ঘূৰাই newTrie;
}

UnicodeString&
UCharsTrieBuilder::buildUnicodeString(UStringTrieBuildOption buildOption,
                                      UnicodeString& result U_LIFETIME_BOUND,
                                      UErrorCode& errorCode) U_LIFETIME_BOUND {
    buildUChars(buildOption, errorCode);
    যদি(U_SUCCESS(errorCode)) {
        result.setTo(মিছা, uchars+(ucharsCapacity-ucharsLength), ucharsLength);
    }
    ঘূৰাই result;
}

শূন্য
UCharsTrieBuilder::buildUChars(UStringTrieBuildOption buildOption, UErrorCode &errorCode) {
    যদি(U_FAILURE(errorCode)) {
        ঘূৰাই;
    }
    যদি(uchars!=nullptr && ucharsLength>0) {
        // Already built.
        ঘূৰাই;
    }
    যদি(ucharsLength==0) {
        যদি(elementsLength==0) {
            errorCode=U_INDEX_OUTOFBOUNDS_ERROR;
            ঘূৰাই;
        }
        যদি(strings.isBogus()) {
            errorCode=U_MEMORY_ALLOCATION_ERROR;
            ঘূৰাই;
        }
        uprv_sortArray(elements, elementsLength, static_cast<int32_t>(পরিমাণ(UCharsTrieElement)),
                      compareElementStrings, &strings,
                      মিছা,  // need not be a stable sort
                      &errorCode);
        যদি(U_FAILURE(errorCode)) {
            ঘূৰাই;
        }
        // Duplicate strings are not allowed.
        UnicodeString prev=elements[0].getString(strings);
        বাবে(int32_t i=1; i<elementsLength; ++i) {
            UnicodeString current=elements[i].getString(strings);
            যদি(prev==current) {
                errorCode=U_ILLEGAL_ARGUMENT_ERROR;
                ঘূৰাই;
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
            ঘূৰাই;
        }
        ucharsCapacity=capacity;
    }
    StringTrieBuilder::build(buildOption, elementsLength, errorCode);
    যদি(uchars==nullptr) {
        errorCode=U_MEMORY_ALLOCATION_ERROR;
    }
}

int32_t
UCharsTrieBuilder::getElementStringLength(int32_t i) ধ্রুৱ {
    ঘূৰাই elements[i].getStringLength(strings);
}

char16_t
UCharsTrieBuilder::getElementUnit(int32_t i, int32_t unitIndex) ধ্রুৱ {
    ঘূৰাই elements[i].charAt(unitIndex, strings);
}

int32_t
UCharsTrieBuilder::get_Element_মান(int32_t i) ধ্রুৱ {
    ঘূৰাই elements[i].get_মান();
}

int32_t
UCharsTrieBuilder::getLimitOfLinearMatch(int32_t first, int32_t last, int32_t unitIndex) ধ্রুৱ {
    ধ্রুৱ UCharsTrieElement &firstElement=elements[first];
    ধ্রুৱ UCharsTrieElement &lastElement=elements[last];
    int32_t minStringLength=firstElement.getStringLength(strings);
    যেতিয়ালৈ(++unitIndex<minStringLength &&
            firstElement.charAt(unitIndex, strings)==
            lastElement.charAt(unitIndex, strings)) {}
    ঘূৰাই unitIndex;
}

int32_t
UCharsTrieBuilder::গুনতি_Element_Units(int32_t start, int32_t limit, int32_t unitIndex) ধ্রুৱ {
    int32_t length=0;  // Number of different units at unitIndex.
    int32_t i=start;
    কৰা {
        char16_t unit=elements[i++].charAt(unitIndex, strings);
        যেতিয়ালৈ(i<limit && unit==elements[i].charAt(unitIndex, strings)) {
            ++i;
        }
        ++length;
    } যেতিয়ালৈ(i<limit);
    ঘূৰাই length;
}

int32_t
UCharsTrieBuilder::skipElementsBySomeUnits(int32_t i, int32_t unitIndex, int32_t count) ধ্রুৱ {
    কৰা {
        char16_t unit=elements[i++].charAt(unitIndex, strings);
        যেতিয়ালৈ(unit==elements[i].charAt(unitIndex, strings)) {
            ++i;
        }
    } যেতিয়ালৈ(--count>0);
    ঘূৰাই i;
}

int32_t
UCharsTrieBuilder::indexOfElementWithNextUnit(int32_t i, int32_t unitIndex, char16_t unit) ধ্রুৱ {
    যেতিয়ালৈ(unit==elements[i].charAt(unitIndex, strings)) {
        ++i;
    }
    ঘূৰাই i;
}

UCharsTrieBuilder::UCTLinear_Match_গ্ৰন্থি::UCTLinear_Match_গ্ৰন্থি(ধ্রুৱ char16_t *units, int32_t len, Node *next_গ্ৰন্থি)
        : Linear_Match_গ্ৰন্থি(len, next_গ্ৰন্থি), s(units) {
    hash=hash*37u+ustr_hashUCharsN(units, len);
}

তার্কিক
UCharsTrieBuilder::UCTLinear_Match_গ্ৰন্থি::operator==(ধ্রুৱ Node &other) ধ্রুৱ {
    যদি(এতত্==&other) {
        ঘূৰাই সঁচা;
    }
    যদি(!Linear_Match_গ্ৰন্থি::operator==(other)) {
        ঘূৰাই মিছা;
    }
    ধ্রুৱ UCTLinear_Match_গ্ৰন্থি &o=static_cast<ধ্রুৱ UCTLinear_Match_গ্ৰন্থি &>(other);
    ঘূৰাই 0==u_memcmp(s, o.s, length);
}

শূন্য
UCharsTrieBuilder::UCTLinear_Match_গ্ৰন্থি::লিখ(StringTrieBuilder &builder) {
    UCharsTrieBuilder &b=(UCharsTrieBuilder &)builder;
    next->লিখ(builder);
    b.লিখ(s, length);
    offset=b.write_মান_And_Type(has_মান, value, b.getMinLinearMatch()+length-1);
}

StringTrieBuilder::Node *
UCharsTrieBuilder::create_Linear_Match_গ্ৰন্থি(int32_t i, int32_t unitIndex, int32_t length,
                                         Node *next_গ্ৰন্থি) ধ্রুৱ {
    ঘূৰাই নূতন UCTLinear_Match_গ্ৰন্থি(
            elements[i].getString(strings).getBuffer()+unitIndex,
            length,
            next_গ্ৰন্থি);
}

UBool
UCharsTrieBuilder::ensureCapacity(int32_t length) {
    যদি(uchars==nullptr) {
        ঘূৰাই মিছা;  // previous memory allocation had failed
    }
    যদি(length>ucharsCapacity) {
        int32_t newCapacity=ucharsCapacity;
        কৰা {
            newCapacity*=2;
        } যেতিয়ালৈ(newCapacity<=length);
        char16_t *newUChars=static_cast<char16_t *>(uprv_malloc(newCapacity*2));
        যদি(newUChars==nullptr) {
            // unable to allocate memory
            uprv_free(uchars);
            uchars=nullptr;
            ucharsCapacity=0;
            ঘূৰাই মিছা;
        }
        u_memcpy(newUChars+(newCapacity-ucharsLength),
                 uchars+(ucharsCapacity-ucharsLength), ucharsLength);
        uprv_free(uchars);
        uchars=newUChars;
        ucharsCapacity=newCapacity;
    }
    ঘূৰাই সঁচা;
}

int32_t
UCharsTrieBuilder::লিখ(int32_t unit) {
    int32_t newLength=ucharsLength+1;
    যদি(ensureCapacity(newLength)) {
        ucharsLength=newLength;
        uchars[ucharsCapacity - ucharsLength] = static_cast<char16_t>(unit);
    }
    ঘূৰাই ucharsLength;
}

int32_t
UCharsTrieBuilder::লিখ(ধ্রুৱ char16_t *s, int32_t length) {
    int32_t newLength=ucharsLength+length;
    যদি(ensureCapacity(newLength)) {
        ucharsLength=newLength;
        u_memcpy(uchars+(ucharsCapacity-ucharsLength), s, length);
    }
    ঘূৰাই ucharsLength;
}

int32_t
UCharsTrieBuilder::writeElementUnits(int32_t i, int32_t unitIndex, int32_t length) {
    ঘূৰাই লিখ(elements[i].getString(strings).getBuffer()+unitIndex, length);
}

int32_t
UCharsTrieBuilder::write_মান_And_Final(int32_t i, UBool isFinal) {
    যদি(0<=i && i<=UCharsTrie::k_Max_One_Unit_মান) {
        ঘূৰাই লিখ(i|(isFinal<<15));
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
    ঘূৰাই লিখ(intUnits, length);
}

int32_t
UCharsTrieBuilder::write_মান_And_Type(UBool has_মান, int32_t value, int32_t গ্ৰন্থি) {
    যদি(!has_মান) {
        ঘূৰাই লিখ(গ্ৰন্থি);
    }
    char16_t intUnits[3];
    int32_t length;
    যদি(value<0 || value>UCharsTrie::k_Max_Two_Unit_গ্ৰন্থি_মান) {
        intUnits[0] = static_cast<char16_t>(UCharsTrie::k_Three_Unit_গ্ৰন্থি_মান_Lead);
        intUnits[1] = static_cast<char16_t>(static_cast<uint32_t>(value) >> 16);
        intUnits[2] = static_cast<char16_t>(value);
        length=3;
    } অন্যথা যদি(value<=UCharsTrie::k_Max_One_Unit_গ্ৰন্থি_মান) {
        intUnits[0] = static_cast<char16_t>((value + 1) << 6);
        length=1;
    } অন্যথা {
        intUnits[0] = static_cast<char16_t>(UCharsTrie::k_Min_Two_Unit_গ্ৰন্থি_মান_Lead + ((value >> 10) & 0x7fc0));
        intUnits[1] = static_cast<char16_t>(value);
        length=2;
    }
    intUnits[0] |= static_cast<char16_t>(গ্ৰন্থি);
    ঘূৰাই লিখ(intUnits, length);
}

int32_t
UCharsTrieBuilder::writeDeltaTo(int32_t jumpTarget) {
    int32_t i=ucharsLength-jumpTarget;
    U_ASSERT(i>=0);
    যদি(i<=UCharsTrie::kMaxOneUnitDelta) {
        ঘূৰাই লিখ(i);
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
    ঘূৰাই লিখ(intUnits, length);
}

U_NAMESPACE_END
