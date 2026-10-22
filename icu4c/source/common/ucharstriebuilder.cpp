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

#அந்தர்பாவ "unicode/utypes.h"
#அந்தர்பாவ "unicode/ucharstrie.h"
#அந்தர்பாவ "unicode/ucharstriebuilder.h"
#அந்தர்பாவ "unicode/unistr.h"
#அந்தர்பாவ "unicode/ustring.h"
#அந்தர்பாவ "cmemory.h"
#அந்தர்பாவ "uarrsort.h"
#அந்தர்பாவ "uassert.h"
#அந்தர்பாவ "uhash.h"
#அந்தர்பாவ "ustr_imp.h"

U_NAMESPACE_BEGIN

/*
 * Note: This builder implementation stores (string, value) pairs with full copies
 * of the 16-bit-unit sequences, until the UCharsTrie is built.
 * It might(!) take less memory if we collected the data in a temporary, dynamic trie.
 */

வகுப்பு UCharsTrieElement : பொது UMemory {
பொது:
    // Use compiler's default constructor, initializes nothing.

    வெறுமை setTo(மாறிலி UnicodeString &s, int32_t val, UnicodeString &strings, UErrorCode &errorCode);

    UnicodeString getString(மாறிலி UnicodeString& strings U_LIFETIME_BOUND) மாறிலி {
        int32_t length=strings[stringOffset];
        திரும்பு strings.tempSubString(stringOffset+1, length);
    }
    int32_t getStringLength(மாறிலி UnicodeString &strings) மாறிலி {
        திரும்பு strings[stringOffset];
    }

    char16_t charAt(int32_t index, மாறிலி UnicodeString &strings) மாறிலி {
        திரும்பு strings[stringOffset+1+index];
    }

    int32_t get_மதிப்பு() மாறிலி { திரும்பு value; }

    int32_t compareStringTo(மாறிலி UCharsTrieElement &o, மாறிலி UnicodeString &strings) மாறிலி;

தனிப்பட்ட:
    // The first strings unit contains the string length.
    // (Compared with a stringLength field here, this saves 2 bytes per string.)
    int32_t stringOffset;
    int32_t value;
};

வெறுமை
UCharsTrieElement::setTo(மாறிலி UnicodeString &s, int32_t val,
                         UnicodeString &strings, UErrorCode &errorCode) {
    எனில்(U_FAILURE(errorCode)) {
        திரும்பு;
    }
    int32_t length=s.length();
    எனில்(length>0xffff) {
        // Too long: We store the length in 1 unit.
        errorCode=U_INDEX_OUTOFBOUNDS_ERROR;
        திரும்பு;
    }
    stringOffset=strings.length();
    strings.append(static_cast<char16_t>(length));
    value=val;
    strings.append(s);
}

int32_t
UCharsTrieElement::compareStringTo(மாறிலி UCharsTrieElement &other, மாறிலி UnicodeString &strings) மாறிலி {
    திரும்பு getString(strings).compare(other.getString(strings));
}

UCharsTrieBuilder::UCharsTrieBuilder(UErrorCode & /*errorCode*/)
        : elements(nullptr), elementsCapacity(0), elementsLength(0),
          uchars(nullptr), ucharsCapacity(0), ucharsLength(0) {}

UCharsTrieBuilder::~UCharsTrieBuilder() {
    delete[] elements;
    uprv_free(uchars);
}

UCharsTrieBuilder&
UCharsTrieBuilder::add(மாறிலி UnicodeString& s,
                       int32_t value,
                       UErrorCode& errorCode) U_LIFETIME_BOUND {
    எனில்(U_FAILURE(errorCode)) {
        திரும்பு *இது;
    }
    எனில்(ucharsLength>0) {
        // Cannot add elements after building.
        errorCode=U_NO_WRITE_PERMISSION;
        திரும்பு *இது;
    }
    எனில்(elementsLength==elementsCapacity) {
        int32_t newCapacity;
        எனில்(elementsCapacity==0) {
            newCapacity=1024;
        } இல்லையெனில் {
            newCapacity=4*elementsCapacity;
        }
        UCharsTrieElement *newElements=புதிய UCharsTrieElement[newCapacity];
        எனில்(newElements==nullptr) {
            errorCode=U_MEMORY_ALLOCATION_ERROR;
            திரும்பு *இது;
        }
        எனில்(elementsLength>0) {
            uprv_memcpy(newElements, elements, (size_t)elementsLength*அளவு(UCharsTrieElement));
        }
        delete[] elements;
        elements=newElements;
        elementsCapacity=newCapacity;
    }
    elements[elementsLength++].setTo(s, value, strings, errorCode);
    எனில்(U_SUCCESS(errorCode) && strings.isBogus()) {
        errorCode=U_MEMORY_ALLOCATION_ERROR;
    }
    திரும்பு *இது;
}

U_CDECL_BEGIN

நிலையான int32_t U_CALLCONV
compareElementStrings(மாறிலி வெறுமை *context, மாறிலி வெறுமை *left, மாறிலி வெறுமை *வலது) {
    மாறிலி UnicodeString *strings=static_cast<மாறிலி UnicodeString *>(context);
    மாறிலி UCharsTrieElement *இடது_Element=static_cast<மாறிலி UCharsTrieElement *>(left);
    மாறிலி UCharsTrieElement *வலது_Element=static_cast<மாறிலி UCharsTrieElement *>(வலது);
    திரும்பு இடது_Element->compareStringTo(*வலது_Element, *strings);
}

U_CDECL_END

UCharsTrie *
UCharsTrieBuilder::build(UStringTrieBuildOption buildOption, UErrorCode &errorCode) {
    buildUChars(buildOption, errorCode);
    UCharsTrie *newTrie=nullptr;
    எனில்(U_SUCCESS(errorCode)) {
        newTrie=புதிய UCharsTrie(uchars, uchars+(ucharsCapacity-ucharsLength));
        எனில்(newTrie==nullptr) {
            errorCode=U_MEMORY_ALLOCATION_ERROR;
        } இல்லையெனில் {
            uchars=nullptr;  // The new trie now owns the array.
            ucharsCapacity=0;
        }
    }
    திரும்பு newTrie;
}

UnicodeString&
UCharsTrieBuilder::buildUnicodeString(UStringTrieBuildOption buildOption,
                                      UnicodeString& result U_LIFETIME_BOUND,
                                      UErrorCode& errorCode) U_LIFETIME_BOUND {
    buildUChars(buildOption, errorCode);
    எனில்(U_SUCCESS(errorCode)) {
        result.setTo(பொய், uchars+(ucharsCapacity-ucharsLength), ucharsLength);
    }
    திரும்பு result;
}

வெறுமை
UCharsTrieBuilder::buildUChars(UStringTrieBuildOption buildOption, UErrorCode &errorCode) {
    எனில்(U_FAILURE(errorCode)) {
        திரும்பு;
    }
    எனில்(uchars!=nullptr && ucharsLength>0) {
        // Already built.
        திரும்பு;
    }
    எனில்(ucharsLength==0) {
        எனில்(elementsLength==0) {
            errorCode=U_INDEX_OUTOFBOUNDS_ERROR;
            திரும்பு;
        }
        எனில்(strings.isBogus()) {
            errorCode=U_MEMORY_ALLOCATION_ERROR;
            திரும்பு;
        }
        uprv_sortArray(elements, elementsLength, static_cast<int32_t>(அளவு(UCharsTrieElement)),
                      compareElementStrings, &strings,
                      பொய்,  // need not be a stable sort
                      &errorCode);
        எனில்(U_FAILURE(errorCode)) {
            திரும்பு;
        }
        // Duplicate strings are not allowed.
        UnicodeString prev=elements[0].getString(strings);
        க்கு(int32_t i=1; i<elementsLength; ++i) {
            UnicodeString current=elements[i].getString(strings);
            எனில்(prev==current) {
                errorCode=U_ILLEGAL_ARGUMENT_ERROR;
                திரும்பு;
            }
            prev.fastCopyFrom(current);
        }
    }
    // Create and char16_t-serialize the trie for the elements.
    ucharsLength=0;
    int32_t capacity=strings.length();
    எனில்(capacity<1024) {
        capacity=1024;
    }
    எனில்(ucharsCapacity<capacity) {
        uprv_free(uchars);
        uchars=static_cast<char16_t *>(uprv_malloc(capacity*2));
        எனில்(uchars==nullptr) {
            errorCode=U_MEMORY_ALLOCATION_ERROR;
            ucharsCapacity=0;
            திரும்பு;
        }
        ucharsCapacity=capacity;
    }
    StringTrieBuilder::build(buildOption, elementsLength, errorCode);
    எனில்(uchars==nullptr) {
        errorCode=U_MEMORY_ALLOCATION_ERROR;
    }
}

int32_t
UCharsTrieBuilder::getElementStringLength(int32_t i) மாறிலி {
    திரும்பு elements[i].getStringLength(strings);
}

char16_t
UCharsTrieBuilder::getElementUnit(int32_t i, int32_t unitIndex) மாறிலி {
    திரும்பு elements[i].charAt(unitIndex, strings);
}

int32_t
UCharsTrieBuilder::get_Element_மதிப்பு(int32_t i) மாறிலி {
    திரும்பு elements[i].get_மதிப்பு();
}

int32_t
UCharsTrieBuilder::getLimitOfLinearMatch(int32_t first, int32_t last, int32_t unitIndex) மாறிலி {
    மாறிலி UCharsTrieElement &firstElement=elements[first];
    மாறிலி UCharsTrieElement &lastElement=elements[last];
    int32_t minStringLength=firstElement.getStringLength(strings);
    போது(++unitIndex<minStringLength &&
            firstElement.charAt(unitIndex, strings)==
            lastElement.charAt(unitIndex, strings)) {}
    திரும்பு unitIndex;
}

int32_t
UCharsTrieBuilder::மொத்தம்_Element_Units(int32_t start, int32_t limit, int32_t unitIndex) மாறிலி {
    int32_t length=0;  // Number of different units at unitIndex.
    int32_t i=start;
    செய் {
        char16_t unit=elements[i++].charAt(unitIndex, strings);
        போது(i<limit && unit==elements[i].charAt(unitIndex, strings)) {
            ++i;
        }
        ++length;
    } போது(i<limit);
    திரும்பு length;
}

int32_t
UCharsTrieBuilder::skipElementsBySomeUnits(int32_t i, int32_t unitIndex, int32_t count) மாறிலி {
    செய் {
        char16_t unit=elements[i++].charAt(unitIndex, strings);
        போது(unit==elements[i].charAt(unitIndex, strings)) {
            ++i;
        }
    } போது(--count>0);
    திரும்பு i;
}

int32_t
UCharsTrieBuilder::indexOfElementWithNextUnit(int32_t i, int32_t unitIndex, char16_t unit) மாறிலி {
    போது(unit==elements[i].charAt(unitIndex, strings)) {
        ++i;
    }
    திரும்பு i;
}

UCharsTrieBuilder::UCTLinear_Match_முனை::UCTLinear_Match_முனை(மாறிலி char16_t *units, int32_t len, Node *next_முனை)
        : Linear_Match_முனை(len, next_முனை), s(units) {
    hash=hash*37u+ustr_hashUCharsN(units, len);
}

தருக்க
UCharsTrieBuilder::UCTLinear_Match_முனை::operator==(மாறிலி Node &other) மாறிலி {
    எனில்(இது==&other) {
        திரும்பு உண்மை;
    }
    எனில்(!Linear_Match_முனை::operator==(other)) {
        திரும்பு பொய்;
    }
    மாறிலி UCTLinear_Match_முனை &o=static_cast<மாறிலி UCTLinear_Match_முனை &>(other);
    திரும்பு 0==u_memcmp(s, o.s, length);
}

வெறுமை
UCharsTrieBuilder::UCTLinear_Match_முனை::லிக(StringTrieBuilder &builder) {
    UCharsTrieBuilder &b=(UCharsTrieBuilder &)builder;
    next->லிக(builder);
    b.லிக(s, length);
    offset=b.write_மதிப்பு_And_Type(has_மதிப்பு, value, b.getMinLinearMatch()+length-1);
}

StringTrieBuilder::Node *
UCharsTrieBuilder::create_Linear_Match_முனை(int32_t i, int32_t unitIndex, int32_t length,
                                         Node *next_முனை) மாறிலி {
    திரும்பு புதிய UCTLinear_Match_முனை(
            elements[i].getString(strings).getBuffer()+unitIndex,
            length,
            next_முனை);
}

UBool
UCharsTrieBuilder::ensureCapacity(int32_t length) {
    எனில்(uchars==nullptr) {
        திரும்பு பொய்;  // previous memory allocation had failed
    }
    எனில்(length>ucharsCapacity) {
        int32_t newCapacity=ucharsCapacity;
        செய் {
            newCapacity*=2;
        } போது(newCapacity<=length);
        char16_t *newUChars=static_cast<char16_t *>(uprv_malloc(newCapacity*2));
        எனில்(newUChars==nullptr) {
            // unable to allocate memory
            uprv_free(uchars);
            uchars=nullptr;
            ucharsCapacity=0;
            திரும்பு பொய்;
        }
        u_memcpy(newUChars+(newCapacity-ucharsLength),
                 uchars+(ucharsCapacity-ucharsLength), ucharsLength);
        uprv_free(uchars);
        uchars=newUChars;
        ucharsCapacity=newCapacity;
    }
    திரும்பு உண்மை;
}

int32_t
UCharsTrieBuilder::லிக(int32_t unit) {
    int32_t newLength=ucharsLength+1;
    எனில்(ensureCapacity(newLength)) {
        ucharsLength=newLength;
        uchars[ucharsCapacity - ucharsLength] = static_cast<char16_t>(unit);
    }
    திரும்பு ucharsLength;
}

int32_t
UCharsTrieBuilder::லிக(மாறிலி char16_t *s, int32_t length) {
    int32_t newLength=ucharsLength+length;
    எனில்(ensureCapacity(newLength)) {
        ucharsLength=newLength;
        u_memcpy(uchars+(ucharsCapacity-ucharsLength), s, length);
    }
    திரும்பு ucharsLength;
}

int32_t
UCharsTrieBuilder::writeElementUnits(int32_t i, int32_t unitIndex, int32_t length) {
    திரும்பு லிக(elements[i].getString(strings).getBuffer()+unitIndex, length);
}

int32_t
UCharsTrieBuilder::write_மதிப்பு_And_Final(int32_t i, UBool isFinal) {
    எனில்(0<=i && i<=UCharsTrie::k_Max_One_Unit_மதிப்பு) {
        திரும்பு லிக(i|(isFinal<<15));
    }
    char16_t intUnits[3];
    int32_t length;
    எனில்(i<0 || i>UCharsTrie::k_Max_Two_Unit_மதிப்பு) {
        intUnits[0] = static_cast<char16_t>(UCharsTrie::k_Three_Unit_மதிப்பு_Lead);
        intUnits[1] = static_cast<char16_t>(static_cast<uint32_t>(i) >> 16);
        intUnits[2] = static_cast<char16_t>(i);
        length=3;
    // } else if(i<=UCharsTrie::kMaxOneUnitValue) {
    //     intUnits[0]=(char16_t)(i);
    //     length=1;
    } இல்லையெனில் {
        intUnits[0] = static_cast<char16_t>(UCharsTrie::k_Min_Two_Unit_மதிப்பு_Lead + (i >> 16));
        intUnits[1] = static_cast<char16_t>(i);
        length=2;
    }
    intUnits[0] = static_cast<char16_t>(intUnits[0] | (isFinal << 15));
    திரும்பு லிக(intUnits, length);
}

int32_t
UCharsTrieBuilder::write_மதிப்பு_And_Type(UBool has_மதிப்பு, int32_t value, int32_t முனை) {
    எனில்(!has_மதிப்பு) {
        திரும்பு லிக(முனை);
    }
    char16_t intUnits[3];
    int32_t length;
    எனில்(value<0 || value>UCharsTrie::k_Max_Two_Unit_முனை_மதிப்பு) {
        intUnits[0] = static_cast<char16_t>(UCharsTrie::k_Three_Unit_முனை_மதிப்பு_Lead);
        intUnits[1] = static_cast<char16_t>(static_cast<uint32_t>(value) >> 16);
        intUnits[2] = static_cast<char16_t>(value);
        length=3;
    } இல்லையெனில் எனில்(value<=UCharsTrie::k_Max_One_Unit_முனை_மதிப்பு) {
        intUnits[0] = static_cast<char16_t>((value + 1) << 6);
        length=1;
    } இல்லையெனில் {
        intUnits[0] = static_cast<char16_t>(UCharsTrie::k_Min_Two_Unit_முனை_மதிப்பு_Lead + ((value >> 10) & 0x7fc0));
        intUnits[1] = static_cast<char16_t>(value);
        length=2;
    }
    intUnits[0] |= static_cast<char16_t>(முனை);
    திரும்பு லிக(intUnits, length);
}

int32_t
UCharsTrieBuilder::writeDeltaTo(int32_t jumpTarget) {
    int32_t i=ucharsLength-jumpTarget;
    U_ASSERT(i>=0);
    எனில்(i<=UCharsTrie::kMaxOneUnitDelta) {
        திரும்பு லிக(i);
    }
    char16_t intUnits[3];
    int32_t length;
    எனில்(i<=UCharsTrie::kMaxTwoUnitDelta) {
        intUnits[0] = static_cast<char16_t>(UCharsTrie::kMinTwoUnitDeltaLead + (i >> 16));
        length=1;
    } இல்லையெனில் {
        intUnits[0] = static_cast<char16_t>(UCharsTrie::kThreeUnitDeltaLead);
        intUnits[1] = static_cast<char16_t>(i >> 16);
        length=2;
    }
    intUnits[length++] = static_cast<char16_t>(i);
    திரும்பு லிக(intUnits, length);
}

U_NAMESPACE_END
