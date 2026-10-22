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

#ಅನ್ತರ್ಭಾವ "unicode/utypes.h"
#ಅನ್ತರ್ಭಾವ "unicode/ucharstrie.h"
#ಅನ್ತರ್ಭಾವ "unicode/ucharstriebuilder.h"
#ಅನ್ತರ್ಭಾವ "unicode/unistr.h"
#ಅನ್ತರ್ಭಾವ "unicode/ustring.h"
#ಅನ್ತರ್ಭಾವ "cmemory.h"
#ಅನ್ತರ್ಭಾವ "uarrsort.h"
#ಅನ್ತರ್ಭಾವ "uassert.h"
#ಅನ್ತರ್ಭಾವ "uhash.h"
#ಅನ್ತರ್ಭಾವ "ustr_imp.h"

U_NAMESPACE_BEGIN

/*
 * Note: This builder implementation stores (string, value) pairs with full copies
 * of the 16-bit-unit sequences, until the UCharsTrie is built.
 * It might(!) take less memory if we collected the data in a temporary, dynamic trie.
 */

ವರ್ಗ UCharsTrieElement : ಸಾರ್ವಜನಿಕ UMemory {
ಸಾರ್ವಜನಿಕ:
    // Use compiler's default constructor, initializes nothing.

    ಶೂನ್ಯ setTo(ಧ್ರುವ UnicodeString &s, int32_t val, UnicodeString &strings, UErrorCode &errorCode);

    UnicodeString getString(ಧ್ರುವ UnicodeString& strings U_LIFETIME_BOUND) ಧ್ರುವ {
        int32_t length=strings[stringOffset];
        ಪ್ರತ್ಯಾಗಮ strings.tempSubString(stringOffset+1, length);
    }
    int32_t getStringLength(ಧ್ರುವ UnicodeString &strings) ಧ್ರುವ {
        ಪ್ರತ್ಯಾಗಮ strings[stringOffset];
    }

    char16_t charAt(int32_t index, ಧ್ರುವ UnicodeString &strings) ಧ್ರುವ {
        ಪ್ರತ್ಯಾಗಮ strings[stringOffset+1+index];
    }

    int32_t get_ಮೌಲ್ಯ() ಧ್ರುವ { ಪ್ರತ್ಯಾಗಮ value; }

    int32_t compareStringTo(ಧ್ರುವ UCharsTrieElement &o, ಧ್ರುವ UnicodeString &strings) ಧ್ರುವ;

ಗುಪ್ತ:
    // The first strings unit contains the string length.
    // (Compared with a stringLength field here, this saves 2 bytes per string.)
    int32_t stringOffset;
    int32_t value;
};

ಶೂನ್ಯ
UCharsTrieElement::setTo(ಧ್ರುವ UnicodeString &s, int32_t val,
                         UnicodeString &strings, UErrorCode &errorCode) {
    ಯದಿ(U_FAILURE(errorCode)) {
        ಪ್ರತ್ಯಾಗಮ;
    }
    int32_t length=s.length();
    ಯದಿ(length>0xffff) {
        // Too long: We store the length in 1 unit.
        errorCode=U_INDEX_OUTOFBOUNDS_ERROR;
        ಪ್ರತ್ಯಾಗಮ;
    }
    stringOffset=strings.length();
    strings.append(static_cast<char16_t>(length));
    value=val;
    strings.append(s);
}

int32_t
UCharsTrieElement::compareStringTo(ಧ್ರುವ UCharsTrieElement &other, ಧ್ರುವ UnicodeString &strings) ಧ್ರುವ {
    ಪ್ರತ್ಯಾಗಮ getString(strings).compare(other.getString(strings));
}

UCharsTrieBuilder::UCharsTrieBuilder(UErrorCode & /*errorCode*/)
        : elements(nullptr), elementsCapacity(0), elementsLength(0),
          uchars(nullptr), ucharsCapacity(0), ucharsLength(0) {}

UCharsTrieBuilder::~UCharsTrieBuilder() {
    delete[] elements;
    uprv_free(uchars);
}

UCharsTrieBuilder&
UCharsTrieBuilder::add(ಧ್ರುವ UnicodeString& s,
                       int32_t value,
                       UErrorCode& errorCode) U_LIFETIME_BOUND {
    ಯದಿ(U_FAILURE(errorCode)) {
        ಪ್ರತ್ಯಾಗಮ *ಏತತ್;
    }
    ಯದಿ(ucharsLength>0) {
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
        UCharsTrieElement *newElements=ನೂತನ UCharsTrieElement[newCapacity];
        ಯದಿ(newElements==nullptr) {
            errorCode=U_MEMORY_ALLOCATION_ERROR;
            ಪ್ರತ್ಯಾಗಮ *ಏತತ್;
        }
        ಯದಿ(elementsLength>0) {
            uprv_memcpy(newElements, elements, (size_t)elementsLength*ಪರಿಮಾಣ(UCharsTrieElement));
        }
        delete[] elements;
        elements=newElements;
        elementsCapacity=newCapacity;
    }
    elements[elementsLength++].setTo(s, value, strings, errorCode);
    ಯದಿ(U_SUCCESS(errorCode) && strings.isBogus()) {
        errorCode=U_MEMORY_ALLOCATION_ERROR;
    }
    ಪ್ರತ್ಯಾಗಮ *ಏತತ್;
}

U_CDECL_BEGIN

ಸ್ಥಿರ int32_t U_CALLCONV
compareElementStrings(ಧ್ರುವ ಶೂನ್ಯ *context, ಧ್ರುವ ಶೂನ್ಯ *left, ಧ್ರುವ ಶೂನ್ಯ *ಬಲ) {
    ಧ್ರುವ UnicodeString *strings=static_cast<ಧ್ರುವ UnicodeString *>(context);
    ಧ್ರುವ UCharsTrieElement *ಎಡ_Element=static_cast<ಧ್ರುವ UCharsTrieElement *>(left);
    ಧ್ರುವ UCharsTrieElement *ಬಲ_Element=static_cast<ಧ್ರುವ UCharsTrieElement *>(ಬಲ);
    ಪ್ರತ್ಯಾಗಮ ಎಡ_Element->compareStringTo(*ಬಲ_Element, *strings);
}

U_CDECL_END

UCharsTrie *
UCharsTrieBuilder::build(UStringTrieBuildOption buildOption, UErrorCode &errorCode) {
    buildUChars(buildOption, errorCode);
    UCharsTrie *newTrie=nullptr;
    ಯದಿ(U_SUCCESS(errorCode)) {
        newTrie=ನೂತನ UCharsTrie(uchars, uchars+(ucharsCapacity-ucharsLength));
        ಯದಿ(newTrie==nullptr) {
            errorCode=U_MEMORY_ALLOCATION_ERROR;
        } ಅನ್ಯಥಾ {
            uchars=nullptr;  // The new trie now owns the array.
            ucharsCapacity=0;
        }
    }
    ಪ್ರತ್ಯಾಗಮ newTrie;
}

UnicodeString&
UCharsTrieBuilder::buildUnicodeString(UStringTrieBuildOption buildOption,
                                      UnicodeString& result U_LIFETIME_BOUND,
                                      UErrorCode& errorCode) U_LIFETIME_BOUND {
    buildUChars(buildOption, errorCode);
    ಯದಿ(U_SUCCESS(errorCode)) {
        result.setTo(ಅಸತ್ಯ, uchars+(ucharsCapacity-ucharsLength), ucharsLength);
    }
    ಪ್ರತ್ಯಾಗಮ result;
}

ಶೂನ್ಯ
UCharsTrieBuilder::buildUChars(UStringTrieBuildOption buildOption, UErrorCode &errorCode) {
    ಯದಿ(U_FAILURE(errorCode)) {
        ಪ್ರತ್ಯಾಗಮ;
    }
    ಯದಿ(uchars!=nullptr && ucharsLength>0) {
        // Already built.
        ಪ್ರತ್ಯಾಗಮ;
    }
    ಯದಿ(ucharsLength==0) {
        ಯದಿ(elementsLength==0) {
            errorCode=U_INDEX_OUTOFBOUNDS_ERROR;
            ಪ್ರತ್ಯಾಗಮ;
        }
        ಯದಿ(strings.isBogus()) {
            errorCode=U_MEMORY_ALLOCATION_ERROR;
            ಪ್ರತ್ಯಾಗಮ;
        }
        uprv_sortArray(elements, elementsLength, static_cast<int32_t>(ಪರಿಮಾಣ(UCharsTrieElement)),
                      compareElementStrings, &strings,
                      ಅಸತ್ಯ,  // need not be a stable sort
                      &errorCode);
        ಯದಿ(U_FAILURE(errorCode)) {
            ಪ್ರತ್ಯಾಗಮ;
        }
        // Duplicate strings are not allowed.
        UnicodeString prev=elements[0].getString(strings);
        ಪ್ರತಿ(int32_t i=1; i<elementsLength; ++i) {
            UnicodeString current=elements[i].getString(strings);
            ಯದಿ(prev==current) {
                errorCode=U_ILLEGAL_ARGUMENT_ERROR;
                ಪ್ರತ್ಯಾಗಮ;
            }
            prev.fastCopyFrom(current);
        }
    }
    // Create and char16_t-serialize the trie for the elements.
    ucharsLength=0;
    int32_t capacity=strings.length();
    ಯದಿ(capacity<1024) {
        capacity=1024;
    }
    ಯದಿ(ucharsCapacity<capacity) {
        uprv_free(uchars);
        uchars=static_cast<char16_t *>(uprv_malloc(capacity*2));
        ಯದಿ(uchars==nullptr) {
            errorCode=U_MEMORY_ALLOCATION_ERROR;
            ucharsCapacity=0;
            ಪ್ರತ್ಯಾಗಮ;
        }
        ucharsCapacity=capacity;
    }
    StringTrieBuilder::build(buildOption, elementsLength, errorCode);
    ಯದಿ(uchars==nullptr) {
        errorCode=U_MEMORY_ALLOCATION_ERROR;
    }
}

int32_t
UCharsTrieBuilder::getElementStringLength(int32_t i) ಧ್ರುವ {
    ಪ್ರತ್ಯಾಗಮ elements[i].getStringLength(strings);
}

char16_t
UCharsTrieBuilder::getElementUnit(int32_t i, int32_t unitIndex) ಧ್ರುವ {
    ಪ್ರತ್ಯಾಗಮ elements[i].charAt(unitIndex, strings);
}

int32_t
UCharsTrieBuilder::get_Element_ಮೌಲ್ಯ(int32_t i) ಧ್ರುವ {
    ಪ್ರತ್ಯಾಗಮ elements[i].get_ಮೌಲ್ಯ();
}

int32_t
UCharsTrieBuilder::getLimitOfLinearMatch(int32_t first, int32_t last, int32_t unitIndex) ಧ್ರುವ {
    ಧ್ರುವ UCharsTrieElement &firstElement=elements[first];
    ಧ್ರುವ UCharsTrieElement &lastElement=elements[last];
    int32_t minStringLength=firstElement.getStringLength(strings);
    ಯಾವತ್(++unitIndex<minStringLength &&
            firstElement.charAt(unitIndex, strings)==
            lastElement.charAt(unitIndex, strings)) {}
    ಪ್ರತ್ಯಾಗಮ unitIndex;
}

int32_t
UCharsTrieBuilder::ಎಣಿಕೆ_Element_Units(int32_t start, int32_t limit, int32_t unitIndex) ಧ್ರುವ {
    int32_t length=0;  // Number of different units at unitIndex.
    int32_t i=start;
    ಕುರು {
        char16_t unit=elements[i++].charAt(unitIndex, strings);
        ಯಾವತ್(i<limit && unit==elements[i].charAt(unitIndex, strings)) {
            ++i;
        }
        ++length;
    } ಯಾವತ್(i<limit);
    ಪ್ರತ್ಯಾಗಮ length;
}

int32_t
UCharsTrieBuilder::skipElementsBySomeUnits(int32_t i, int32_t unitIndex, int32_t count) ಧ್ರುವ {
    ಕುರು {
        char16_t unit=elements[i++].charAt(unitIndex, strings);
        ಯಾವತ್(unit==elements[i].charAt(unitIndex, strings)) {
            ++i;
        }
    } ಯಾವತ್(--count>0);
    ಪ್ರತ್ಯಾಗಮ i;
}

int32_t
UCharsTrieBuilder::indexOfElementWithNextUnit(int32_t i, int32_t unitIndex, char16_t unit) ಧ್ರುವ {
    ಯಾವತ್(unit==elements[i].charAt(unitIndex, strings)) {
        ++i;
    }
    ಪ್ರತ್ಯಾಗಮ i;
}

UCharsTrieBuilder::UCTLinear_Match_ಗಂಟು::UCTLinear_Match_ಗಂಟು(ಧ್ರುವ char16_t *units, int32_t len, Node *next_ಗಂಟು)
        : Linear_Match_ಗಂಟು(len, next_ಗಂಟು), s(units) {
    hash=hash*37u+ustr_hashUCharsN(units, len);
}

ತಾರ್ಕಿಕ
UCharsTrieBuilder::UCTLinear_Match_ಗಂಟು::operator==(ಧ್ರುವ Node &other) ಧ್ರುವ {
    ಯದಿ(ಏತತ್==&other) {
        ಪ್ರತ್ಯಾಗಮ ಸತ್ಯ;
    }
    ಯದಿ(!Linear_Match_ಗಂಟು::operator==(other)) {
        ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;
    }
    ಧ್ರುವ UCTLinear_Match_ಗಂಟು &o=static_cast<ಧ್ರುವ UCTLinear_Match_ಗಂಟು &>(other);
    ಪ್ರತ್ಯಾಗಮ 0==u_memcmp(s, o.s, length);
}

ಶೂನ್ಯ
UCharsTrieBuilder::UCTLinear_Match_ಗಂಟು::ಲಿಖ(StringTrieBuilder &builder) {
    UCharsTrieBuilder &b=(UCharsTrieBuilder &)builder;
    next->ಲಿಖ(builder);
    b.ಲಿಖ(s, length);
    offset=b.write_ಮೌಲ್ಯ_And_Type(has_ಮೌಲ್ಯ, value, b.getMinLinearMatch()+length-1);
}

StringTrieBuilder::Node *
UCharsTrieBuilder::create_Linear_Match_ಗಂಟು(int32_t i, int32_t unitIndex, int32_t length,
                                         Node *next_ಗಂಟು) ಧ್ರುವ {
    ಪ್ರತ್ಯಾಗಮ ನೂತನ UCTLinear_Match_ಗಂಟು(
            elements[i].getString(strings).getBuffer()+unitIndex,
            length,
            next_ಗಂಟು);
}

UBool
UCharsTrieBuilder::ensureCapacity(int32_t length) {
    ಯದಿ(uchars==nullptr) {
        ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;  // previous memory allocation had failed
    }
    ಯದಿ(length>ucharsCapacity) {
        int32_t newCapacity=ucharsCapacity;
        ಕುರು {
            newCapacity*=2;
        } ಯಾವತ್(newCapacity<=length);
        char16_t *newUChars=static_cast<char16_t *>(uprv_malloc(newCapacity*2));
        ಯದಿ(newUChars==nullptr) {
            // unable to allocate memory
            uprv_free(uchars);
            uchars=nullptr;
            ucharsCapacity=0;
            ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;
        }
        u_memcpy(newUChars+(newCapacity-ucharsLength),
                 uchars+(ucharsCapacity-ucharsLength), ucharsLength);
        uprv_free(uchars);
        uchars=newUChars;
        ucharsCapacity=newCapacity;
    }
    ಪ್ರತ್ಯಾಗಮ ಸತ್ಯ;
}

int32_t
UCharsTrieBuilder::ಲಿಖ(int32_t unit) {
    int32_t newLength=ucharsLength+1;
    ಯದಿ(ensureCapacity(newLength)) {
        ucharsLength=newLength;
        uchars[ucharsCapacity - ucharsLength] = static_cast<char16_t>(unit);
    }
    ಪ್ರತ್ಯಾಗಮ ucharsLength;
}

int32_t
UCharsTrieBuilder::ಲಿಖ(ಧ್ರುವ char16_t *s, int32_t length) {
    int32_t newLength=ucharsLength+length;
    ಯದಿ(ensureCapacity(newLength)) {
        ucharsLength=newLength;
        u_memcpy(uchars+(ucharsCapacity-ucharsLength), s, length);
    }
    ಪ್ರತ್ಯಾಗಮ ucharsLength;
}

int32_t
UCharsTrieBuilder::writeElementUnits(int32_t i, int32_t unitIndex, int32_t length) {
    ಪ್ರತ್ಯಾಗಮ ಲಿಖ(elements[i].getString(strings).getBuffer()+unitIndex, length);
}

int32_t
UCharsTrieBuilder::write_ಮೌಲ್ಯ_And_Final(int32_t i, UBool isFinal) {
    ಯದಿ(0<=i && i<=UCharsTrie::k_Max_One_Unit_ಮೌಲ್ಯ) {
        ಪ್ರತ್ಯಾಗಮ ಲಿಖ(i|(isFinal<<15));
    }
    char16_t intUnits[3];
    int32_t length;
    ಯದಿ(i<0 || i>UCharsTrie::k_Max_Two_Unit_ಮೌಲ್ಯ) {
        intUnits[0] = static_cast<char16_t>(UCharsTrie::k_Three_Unit_ಮೌಲ್ಯ_Lead);
        intUnits[1] = static_cast<char16_t>(static_cast<uint32_t>(i) >> 16);
        intUnits[2] = static_cast<char16_t>(i);
        length=3;
    // } else if(i<=UCharsTrie::kMaxOneUnitValue) {
    //     intUnits[0]=(char16_t)(i);
    //     length=1;
    } ಅನ್ಯಥಾ {
        intUnits[0] = static_cast<char16_t>(UCharsTrie::k_Min_Two_Unit_ಮೌಲ್ಯ_Lead + (i >> 16));
        intUnits[1] = static_cast<char16_t>(i);
        length=2;
    }
    intUnits[0] = static_cast<char16_t>(intUnits[0] | (isFinal << 15));
    ಪ್ರತ್ಯಾಗಮ ಲಿಖ(intUnits, length);
}

int32_t
UCharsTrieBuilder::write_ಮೌಲ್ಯ_And_Type(UBool has_ಮೌಲ್ಯ, int32_t value, int32_t ಗಂಟು) {
    ಯದಿ(!has_ಮೌಲ್ಯ) {
        ಪ್ರತ್ಯಾಗಮ ಲಿಖ(ಗಂಟು);
    }
    char16_t intUnits[3];
    int32_t length;
    ಯದಿ(value<0 || value>UCharsTrie::k_Max_Two_Unit_ಗಂಟು_ಮೌಲ್ಯ) {
        intUnits[0] = static_cast<char16_t>(UCharsTrie::k_Three_Unit_ಗಂಟು_ಮೌಲ್ಯ_Lead);
        intUnits[1] = static_cast<char16_t>(static_cast<uint32_t>(value) >> 16);
        intUnits[2] = static_cast<char16_t>(value);
        length=3;
    } ಅನ್ಯಥಾ ಯದಿ(value<=UCharsTrie::k_Max_One_Unit_ಗಂಟು_ಮೌಲ್ಯ) {
        intUnits[0] = static_cast<char16_t>((value + 1) << 6);
        length=1;
    } ಅನ್ಯಥಾ {
        intUnits[0] = static_cast<char16_t>(UCharsTrie::k_Min_Two_Unit_ಗಂಟು_ಮೌಲ್ಯ_Lead + ((value >> 10) & 0x7fc0));
        intUnits[1] = static_cast<char16_t>(value);
        length=2;
    }
    intUnits[0] |= static_cast<char16_t>(ಗಂಟು);
    ಪ್ರತ್ಯಾಗಮ ಲಿಖ(intUnits, length);
}

int32_t
UCharsTrieBuilder::writeDeltaTo(int32_t jumpTarget) {
    int32_t i=ucharsLength-jumpTarget;
    U_ASSERT(i>=0);
    ಯದಿ(i<=UCharsTrie::kMaxOneUnitDelta) {
        ಪ್ರತ್ಯಾಗಮ ಲಿಖ(i);
    }
    char16_t intUnits[3];
    int32_t length;
    ಯದಿ(i<=UCharsTrie::kMaxTwoUnitDelta) {
        intUnits[0] = static_cast<char16_t>(UCharsTrie::kMinTwoUnitDeltaLead + (i >> 16));
        length=1;
    } ಅನ್ಯಥಾ {
        intUnits[0] = static_cast<char16_t>(UCharsTrie::kThreeUnitDeltaLead);
        intUnits[1] = static_cast<char16_t>(i >> 16);
        length=2;
    }
    intUnits[length++] = static_cast<char16_t>(i);
    ಪ್ರತ್ಯಾಗಮ ಲಿಖ(intUnits, length);
}

U_NAMESPACE_END
