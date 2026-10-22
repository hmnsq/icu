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

#अन्तर्भाव "unicode/utypes.h"
#अन्तर्भाव "unicode/ucharstrie.h"
#अन्तर्भाव "unicode/ucharstriebuilder.h"
#अन्तर्भाव "unicode/unistr.h"
#अन्तर्भाव "unicode/ustring.h"
#अन्तर्भाव "cmemory.h"
#अन्तर्भाव "uarrsort.h"
#अन्तर्भाव "uassert.h"
#अन्तर्भाव "uhash.h"
#अन्तर्भाव "ustr_imp.h"

U_NAMESPACE_BEGIN

/*
 * Note: This builder implementation stores (string, value) pairs with full copies
 * of the 16-bit-unit sequences, until the UCharsTrie is built.
 * It might(!) take less memory if we collected the data in a temporary, dynamic trie.
 */

वर्ग UCharsTrieElement : सार्वजनिक UMemory {
सार्वजनिक:
    // Use compiler's default constructor, initializes nothing.

    शून्य setTo(ध्रुव UnicodeString &s, int32_t val, UnicodeString &strings, UErrorCode &errorCode);

    UnicodeString getString(ध्रुव UnicodeString& strings U_LIFETIME_BOUND) ध्रुव {
        int32_t length=strings[stringOffset];
        प्रत्यागम strings.tempSubString(stringOffset+1, length);
    }
    int32_t getStringLength(ध्रुव UnicodeString &strings) ध्रुव {
        प्रत्यागम strings[stringOffset];
    }

    char16_t charAt(int32_t index, ध्रुव UnicodeString &strings) ध्रुव {
        प्रत्यागम strings[stringOffset+1+index];
    }

    int32_t get_मूल्य() ध्रुव { प्रत्यागम value; }

    int32_t compareStringTo(ध्रुव UCharsTrieElement &o, ध्रुव UnicodeString &strings) ध्रुव;

गुप्त:
    // The first strings unit contains the string length.
    // (Compared with a stringLength field here, this saves 2 bytes per string.)
    int32_t stringOffset;
    int32_t value;
};

शून्य
UCharsTrieElement::setTo(ध्रुव UnicodeString &s, int32_t val,
                         UnicodeString &strings, UErrorCode &errorCode) {
    यदि(U_FAILURE(errorCode)) {
        प्रत्यागम;
    }
    int32_t length=s.length();
    यदि(length>0xffff) {
        // Too long: We store the length in 1 unit.
        errorCode=U_INDEX_OUTOFBOUNDS_ERROR;
        प्रत्यागम;
    }
    stringOffset=strings.length();
    strings.append(static_cast<char16_t>(length));
    value=val;
    strings.append(s);
}

int32_t
UCharsTrieElement::compareStringTo(ध्रुव UCharsTrieElement &other, ध्रुव UnicodeString &strings) ध्रुव {
    प्रत्यागम getString(strings).compare(other.getString(strings));
}

UCharsTrieBuilder::UCharsTrieBuilder(UErrorCode & /*errorCode*/)
        : elements(nullptr), elementsCapacity(0), elementsLength(0),
          uchars(nullptr), ucharsCapacity(0), ucharsLength(0) {}

UCharsTrieBuilder::~UCharsTrieBuilder() {
    delete[] elements;
    uprv_free(uchars);
}

UCharsTrieBuilder&
UCharsTrieBuilder::add(ध्रुव UnicodeString& s,
                       int32_t value,
                       UErrorCode& errorCode) U_LIFETIME_BOUND {
    यदि(U_FAILURE(errorCode)) {
        प्रत्यागम *एतत्;
    }
    यदि(ucharsLength>0) {
        // Cannot add elements after building.
        errorCode=U_NO_WRITE_PERMISSION;
        प्रत्यागम *एतत्;
    }
    यदि(elementsLength==elementsCapacity) {
        int32_t newCapacity;
        यदि(elementsCapacity==0) {
            newCapacity=1024;
        } अन्यथा {
            newCapacity=4*elementsCapacity;
        }
        UCharsTrieElement *newElements=नूतन UCharsTrieElement[newCapacity];
        यदि(newElements==nullptr) {
            errorCode=U_MEMORY_ALLOCATION_ERROR;
            प्रत्यागम *एतत्;
        }
        यदि(elementsLength>0) {
            uprv_memcpy(newElements, elements, (माप_प्रकार)elementsLength*परिमाण(UCharsTrieElement));
        }
        delete[] elements;
        elements=newElements;
        elementsCapacity=newCapacity;
    }
    elements[elementsLength++].setTo(s, value, strings, errorCode);
    यदि(U_SUCCESS(errorCode) && strings.isBogus()) {
        errorCode=U_MEMORY_ALLOCATION_ERROR;
    }
    प्रत्यागम *एतत्;
}

U_CDECL_BEGIN

स्थिर int32_t U_CALLCONV
compareElementStrings(ध्रुव शून्य *context, ध्रुव शून्य *left, ध्रुव शून्य *दायाँ) {
    ध्रुव UnicodeString *strings=static_cast<ध्रुव UnicodeString *>(context);
    ध्रुव UCharsTrieElement *बायाँ_Element=static_cast<ध्रुव UCharsTrieElement *>(left);
    ध्रुव UCharsTrieElement *दायाँ_Element=static_cast<ध्रुव UCharsTrieElement *>(दायाँ);
    प्रत्यागम बायाँ_Element->compareStringTo(*दायाँ_Element, *strings);
}

U_CDECL_END

UCharsTrie *
UCharsTrieBuilder::build(UStringTrieBuildOption buildOption, UErrorCode &errorCode) {
    buildUChars(buildOption, errorCode);
    UCharsTrie *newTrie=nullptr;
    यदि(U_SUCCESS(errorCode)) {
        newTrie=नूतन UCharsTrie(uchars, uchars+(ucharsCapacity-ucharsLength));
        यदि(newTrie==nullptr) {
            errorCode=U_MEMORY_ALLOCATION_ERROR;
        } अन्यथा {
            uchars=nullptr;  // The new trie now owns the array.
            ucharsCapacity=0;
        }
    }
    प्रत्यागम newTrie;
}

UnicodeString&
UCharsTrieBuilder::buildUnicodeString(UStringTrieBuildOption buildOption,
                                      UnicodeString& result U_LIFETIME_BOUND,
                                      UErrorCode& errorCode) U_LIFETIME_BOUND {
    buildUChars(buildOption, errorCode);
    यदि(U_SUCCESS(errorCode)) {
        result.setTo(असत्य, uchars+(ucharsCapacity-ucharsLength), ucharsLength);
    }
    प्रत्यागम result;
}

शून्य
UCharsTrieBuilder::buildUChars(UStringTrieBuildOption buildOption, UErrorCode &errorCode) {
    यदि(U_FAILURE(errorCode)) {
        प्रत्यागम;
    }
    यदि(uchars!=nullptr && ucharsLength>0) {
        // Already built.
        प्रत्यागम;
    }
    यदि(ucharsLength==0) {
        यदि(elementsLength==0) {
            errorCode=U_INDEX_OUTOFBOUNDS_ERROR;
            प्रत्यागम;
        }
        यदि(strings.isBogus()) {
            errorCode=U_MEMORY_ALLOCATION_ERROR;
            प्रत्यागम;
        }
        uprv_sortArray(elements, elementsLength, static_cast<int32_t>(परिमाण(UCharsTrieElement)),
                      compareElementStrings, &strings,
                      असत्य,  // need not be a stable sort
                      &errorCode);
        यदि(U_FAILURE(errorCode)) {
            प्रत्यागम;
        }
        // Duplicate strings are not allowed.
        UnicodeString prev=elements[0].getString(strings);
        प्रति(int32_t i=1; i<elementsLength; ++i) {
            UnicodeString current=elements[i].getString(strings);
            यदि(prev==current) {
                errorCode=U_ILLEGAL_ARGUMENT_ERROR;
                प्रत्यागम;
            }
            prev.fastCopyFrom(current);
        }
    }
    // Create and char16_t-serialize the trie for the elements.
    ucharsLength=0;
    int32_t capacity=strings.length();
    यदि(capacity<1024) {
        capacity=1024;
    }
    यदि(ucharsCapacity<capacity) {
        uprv_free(uchars);
        uchars=static_cast<char16_t *>(uprv_malloc(capacity*2));
        यदि(uchars==nullptr) {
            errorCode=U_MEMORY_ALLOCATION_ERROR;
            ucharsCapacity=0;
            प्रत्यागम;
        }
        ucharsCapacity=capacity;
    }
    StringTrieBuilder::build(buildOption, elementsLength, errorCode);
    यदि(uchars==nullptr) {
        errorCode=U_MEMORY_ALLOCATION_ERROR;
    }
}

int32_t
UCharsTrieBuilder::getElementStringLength(int32_t i) ध्रुव {
    प्रत्यागम elements[i].getStringLength(strings);
}

char16_t
UCharsTrieBuilder::getElementUnit(int32_t i, int32_t unitIndex) ध्रुव {
    प्रत्यागम elements[i].charAt(unitIndex, strings);
}

int32_t
UCharsTrieBuilder::get_Element_मूल्य(int32_t i) ध्रुव {
    प्रत्यागम elements[i].get_मूल्य();
}

int32_t
UCharsTrieBuilder::getLimitOfLinearMatch(int32_t first, int32_t last, int32_t unitIndex) ध्रुव {
    ध्रुव UCharsTrieElement &firstElement=elements[first];
    ध्रुव UCharsTrieElement &lastElement=elements[last];
    int32_t minStringLength=firstElement.getStringLength(strings);
    यावत्(++unitIndex<minStringLength &&
            firstElement.charAt(unitIndex, strings)==
            lastElement.charAt(unitIndex, strings)) {}
    प्रत्यागम unitIndex;
}

int32_t
UCharsTrieBuilder::गिनती_Element_Units(int32_t start, int32_t limit, int32_t unitIndex) ध्रुव {
    int32_t length=0;  // Number of different units at unitIndex.
    int32_t i=start;
    कुरु {
        char16_t unit=elements[i++].charAt(unitIndex, strings);
        यावत्(i<limit && unit==elements[i].charAt(unitIndex, strings)) {
            ++i;
        }
        ++length;
    } यावत्(i<limit);
    प्रत्यागम length;
}

int32_t
UCharsTrieBuilder::skipElementsBySomeUnits(int32_t i, int32_t unitIndex, int32_t count) ध्रुव {
    कुरु {
        char16_t unit=elements[i++].charAt(unitIndex, strings);
        यावत्(unit==elements[i].charAt(unitIndex, strings)) {
            ++i;
        }
    } यावत्(--count>0);
    प्रत्यागम i;
}

int32_t
UCharsTrieBuilder::indexOfElementWithNextUnit(int32_t i, int32_t unitIndex, char16_t unit) ध्रुव {
    यावत्(unit==elements[i].charAt(unitIndex, strings)) {
        ++i;
    }
    प्रत्यागम i;
}

UCharsTrieBuilder::UCTLinear_Match_गाँठ::UCTLinear_Match_गाँठ(ध्रुव char16_t *units, int32_t len, Node *next_गाँठ)
        : Linear_Match_गाँठ(len, next_गाँठ), s(units) {
    hash=hash*37u+ustr_hashUCharsN(units, len);
}

तार्किक
UCharsTrieBuilder::UCTLinear_Match_गाँठ::चालक==(ध्रुव Node &other) ध्रुव {
    यदि(एतत्==&other) {
        प्रत्यागम सत्य;
    }
    यदि(!Linear_Match_गाँठ::चालक==(other)) {
        प्रत्यागम असत्य;
    }
    ध्रुव UCTLinear_Match_गाँठ &o=static_cast<ध्रुव UCTLinear_Match_गाँठ &>(other);
    प्रत्यागम 0==u_memcmp(s, o.s, length);
}

शून्य
UCharsTrieBuilder::UCTLinear_Match_गाँठ::लिख(StringTrieBuilder &builder) {
    UCharsTrieBuilder &b=(UCharsTrieBuilder &)builder;
    next->लिख(builder);
    b.लिख(s, length);
    offset=b.write_मूल्य_And_Type(has_मूल्य, value, b.getMinLinearMatch()+length-1);
}

StringTrieBuilder::Node *
UCharsTrieBuilder::create_Linear_Match_गाँठ(int32_t i, int32_t unitIndex, int32_t length,
                                         Node *next_गाँठ) ध्रुव {
    प्रत्यागम नूतन UCTLinear_Match_गाँठ(
            elements[i].getString(strings).getBuffer()+unitIndex,
            length,
            next_गाँठ);
}

UBool
UCharsTrieBuilder::ensureCapacity(int32_t length) {
    यदि(uchars==nullptr) {
        प्रत्यागम असत्य;  // previous memory allocation had failed
    }
    यदि(length>ucharsCapacity) {
        int32_t newCapacity=ucharsCapacity;
        कुरु {
            newCapacity*=2;
        } यावत्(newCapacity<=length);
        char16_t *newUChars=static_cast<char16_t *>(uprv_malloc(newCapacity*2));
        यदि(newUChars==nullptr) {
            // unable to allocate memory
            uprv_free(uchars);
            uchars=nullptr;
            ucharsCapacity=0;
            प्रत्यागम असत्य;
        }
        u_memcpy(newUChars+(newCapacity-ucharsLength),
                 uchars+(ucharsCapacity-ucharsLength), ucharsLength);
        uprv_free(uchars);
        uchars=newUChars;
        ucharsCapacity=newCapacity;
    }
    प्रत्यागम सत्य;
}

int32_t
UCharsTrieBuilder::लिख(int32_t unit) {
    int32_t newLength=ucharsLength+1;
    यदि(ensureCapacity(newLength)) {
        ucharsLength=newLength;
        uchars[ucharsCapacity - ucharsLength] = static_cast<char16_t>(unit);
    }
    प्रत्यागम ucharsLength;
}

int32_t
UCharsTrieBuilder::लिख(ध्रुव char16_t *s, int32_t length) {
    int32_t newLength=ucharsLength+length;
    यदि(ensureCapacity(newLength)) {
        ucharsLength=newLength;
        u_memcpy(uchars+(ucharsCapacity-ucharsLength), s, length);
    }
    प्रत्यागम ucharsLength;
}

int32_t
UCharsTrieBuilder::writeElementUnits(int32_t i, int32_t unitIndex, int32_t length) {
    प्रत्यागम लिख(elements[i].getString(strings).getBuffer()+unitIndex, length);
}

int32_t
UCharsTrieBuilder::write_मूल्य_And_Final(int32_t i, UBool isFinal) {
    यदि(0<=i && i<=UCharsTrie::k_Max_One_Unit_मूल्य) {
        प्रत्यागम लिख(i|(isFinal<<15));
    }
    char16_t intUnits[3];
    int32_t length;
    यदि(i<0 || i>UCharsTrie::k_Max_Two_Unit_मूल्य) {
        intUnits[0] = static_cast<char16_t>(UCharsTrie::k_Three_Unit_मूल्य_Lead);
        intUnits[1] = static_cast<char16_t>(static_cast<uint32_t>(i) >> 16);
        intUnits[2] = static_cast<char16_t>(i);
        length=3;
    // } else if(i<=UCharsTrie::kMaxOneUnitValue) {
    //     intUnits[0]=(char16_t)(i);
    //     length=1;
    } अन्यथा {
        intUnits[0] = static_cast<char16_t>(UCharsTrie::k_Min_Two_Unit_मूल्य_Lead + (i >> 16));
        intUnits[1] = static_cast<char16_t>(i);
        length=2;
    }
    intUnits[0] = static_cast<char16_t>(intUnits[0] | (isFinal << 15));
    प्रत्यागम लिख(intUnits, length);
}

int32_t
UCharsTrieBuilder::write_मूल्य_And_Type(UBool has_मूल्य, int32_t value, int32_t गाँठ) {
    यदि(!has_मूल्य) {
        प्रत्यागम लिख(गाँठ);
    }
    char16_t intUnits[3];
    int32_t length;
    यदि(value<0 || value>UCharsTrie::k_Max_Two_Unit_गाँठ_मूल्य) {
        intUnits[0] = static_cast<char16_t>(UCharsTrie::k_Three_Unit_गाँठ_मूल्य_Lead);
        intUnits[1] = static_cast<char16_t>(static_cast<uint32_t>(value) >> 16);
        intUnits[2] = static_cast<char16_t>(value);
        length=3;
    } अन्यथा यदि(value<=UCharsTrie::k_Max_One_Unit_गाँठ_मूल्य) {
        intUnits[0] = static_cast<char16_t>((value + 1) << 6);
        length=1;
    } अन्यथा {
        intUnits[0] = static_cast<char16_t>(UCharsTrie::k_Min_Two_Unit_गाँठ_मूल्य_Lead + ((value >> 10) & 0x7fc0));
        intUnits[1] = static_cast<char16_t>(value);
        length=2;
    }
    intUnits[0] |= static_cast<char16_t>(गाँठ);
    प्रत्यागम लिख(intUnits, length);
}

int32_t
UCharsTrieBuilder::writeDeltaTo(int32_t jumpTarget) {
    int32_t i=ucharsLength-jumpTarget;
    U_ASSERT(i>=0);
    यदि(i<=UCharsTrie::kMaxOneUnitDelta) {
        प्रत्यागम लिख(i);
    }
    char16_t intUnits[3];
    int32_t length;
    यदि(i<=UCharsTrie::kMaxTwoUnitDelta) {
        intUnits[0] = static_cast<char16_t>(UCharsTrie::kMinTwoUnitDeltaLead + (i >> 16));
        length=1;
    } अन्यथा {
        intUnits[0] = static_cast<char16_t>(UCharsTrie::kThreeUnitDeltaLead);
        intUnits[1] = static_cast<char16_t>(i >> 16);
        length=2;
    }
    intUnits[length++] = static_cast<char16_t>(i);
    प्रत्यागम लिख(intUnits, length);
}

U_NAMESPACE_END
