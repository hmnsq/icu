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

वग्ग UCharsTrieElement : सार्वजनिक UMemory {
सार्वजनिक:
    // Use compiler's default constructor, initializes nothing.

    शून्य setTo(ध्रुव UnicodeString &s, int32_t val, UnicodeString &strings, UErrorCode &errorCode);

    UnicodeString getString(ध्रुव UnicodeString& strings U_LIFETIME_BOUND) ध्रुव {
        int32_t length=strings[stringOffset];
        पटिनिवत्त strings.tempSubString(stringOffset+1, length);
    }
    int32_t getStringLength(ध्रुव UnicodeString &strings) ध्रुव {
        पटिनिवत्त strings[stringOffset];
    }

    char16_t charAt(int32_t index, ध्रुव UnicodeString &strings) ध्रुव {
        पटिनिवत्त strings[stringOffset+1+index];
    }

    int32_t get_मानम्() ध्रुव { पटिनिवत्त value; }

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
    सचे(U_FAILURE(errorCode)) {
        पटिनिवत्त;
    }
    int32_t length=s.length();
    सचे(length>0xffff) {
        // Too long: We store the length in 1 unit.
        errorCode=U_INDEX_OUTOFBOUNDS_ERROR;
        पटिनिवत्त;
    }
    stringOffset=strings.length();
    strings.append(static_cast<char16_t>(length));
    value=val;
    strings.append(s);
}

int32_t
UCharsTrieElement::compareStringTo(ध्रुव UCharsTrieElement &other, ध्रुव UnicodeString &strings) ध्रुव {
    पटिनिवत्त getString(strings).compare(other.getString(strings));
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
    सचे(U_FAILURE(errorCode)) {
        पटिनिवत्त *एतत्;
    }
    सचे(ucharsLength>0) {
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
        UCharsTrieElement *newElements=नूतन UCharsTrieElement[newCapacity];
        सचे(newElements==nullptr) {
            errorCode=U_MEMORY_ALLOCATION_ERROR;
            पटिनिवत्त *एतत्;
        }
        सचे(elementsLength>0) {
            uprv_memcpy(newElements, elements, (size_t)elementsLength*परिमाण(UCharsTrieElement));
        }
        delete[] elements;
        elements=newElements;
        elementsCapacity=newCapacity;
    }
    elements[elementsLength++].setTo(s, value, strings, errorCode);
    सचे(U_SUCCESS(errorCode) && strings.isBogus()) {
        errorCode=U_MEMORY_ALLOCATION_ERROR;
    }
    पटिनिवत्त *एतत्;
}

U_CDECL_BEGIN

स्थिर int32_t U_CALLCONV
compareElementStrings(ध्रुव शून्य *context, ध्रुव शून्य *left, ध्रुव शून्य *दक्षिणः) {
    ध्रुव UnicodeString *strings=static_cast<ध्रुव UnicodeString *>(context);
    ध्रुव UCharsTrieElement *वामः_Element=static_cast<ध्रुव UCharsTrieElement *>(left);
    ध्रुव UCharsTrieElement *दक्षिणः_Element=static_cast<ध्रुव UCharsTrieElement *>(दक्षिणः);
    पटिनिवत्त वामः_Element->compareStringTo(*दक्षिणः_Element, *strings);
}

U_CDECL_END

UCharsTrie *
UCharsTrieBuilder::build(UStringTrieBuildOption buildOption, UErrorCode &errorCode) {
    buildUChars(buildOption, errorCode);
    UCharsTrie *newTrie=nullptr;
    सचे(U_SUCCESS(errorCode)) {
        newTrie=नूतन UCharsTrie(uchars, uchars+(ucharsCapacity-ucharsLength));
        सचे(newTrie==nullptr) {
            errorCode=U_MEMORY_ALLOCATION_ERROR;
        } अञ्ञथा {
            uchars=nullptr;  // The new trie now owns the array.
            ucharsCapacity=0;
        }
    }
    पटिनिवत्त newTrie;
}

UnicodeString&
UCharsTrieBuilder::buildUnicodeString(UStringTrieBuildOption buildOption,
                                      UnicodeString& result U_LIFETIME_BOUND,
                                      UErrorCode& errorCode) U_LIFETIME_BOUND {
    buildUChars(buildOption, errorCode);
    सचे(U_SUCCESS(errorCode)) {
        result.setTo(मिच्छा, uchars+(ucharsCapacity-ucharsLength), ucharsLength);
    }
    पटिनिवत्त result;
}

शून्य
UCharsTrieBuilder::buildUChars(UStringTrieBuildOption buildOption, UErrorCode &errorCode) {
    सचे(U_FAILURE(errorCode)) {
        पटिनिवत्त;
    }
    सचे(uchars!=nullptr && ucharsLength>0) {
        // Already built.
        पटिनिवत्त;
    }
    सचे(ucharsLength==0) {
        सचे(elementsLength==0) {
            errorCode=U_INDEX_OUTOFBOUNDS_ERROR;
            पटिनिवत्त;
        }
        सचे(strings.isBogus()) {
            errorCode=U_MEMORY_ALLOCATION_ERROR;
            पटिनिवत्त;
        }
        uprv_sortArray(elements, elementsLength, static_cast<int32_t>(परिमाण(UCharsTrieElement)),
                      compareElementStrings, &strings,
                      मिच्छा,  // need not be a stable sort
                      &errorCode);
        सचे(U_FAILURE(errorCode)) {
            पटिनिवत्त;
        }
        // Duplicate strings are not allowed.
        UnicodeString prev=elements[0].getString(strings);
        अत्थाय(int32_t i=1; i<elementsLength; ++i) {
            UnicodeString current=elements[i].getString(strings);
            सचे(prev==current) {
                errorCode=U_ILLEGAL_ARGUMENT_ERROR;
                पटिनिवत्त;
            }
            prev.fastCopyFrom(current);
        }
    }
    // Create and char16_t-serialize the trie for the elements.
    ucharsLength=0;
    int32_t capacity=strings.length();
    सचे(capacity<1024) {
        capacity=1024;
    }
    सचे(ucharsCapacity<capacity) {
        uprv_free(uchars);
        uchars=static_cast<char16_t *>(uprv_malloc(capacity*2));
        सचे(uchars==nullptr) {
            errorCode=U_MEMORY_ALLOCATION_ERROR;
            ucharsCapacity=0;
            पटिनिवत्त;
        }
        ucharsCapacity=capacity;
    }
    StringTrieBuilder::build(buildOption, elementsLength, errorCode);
    सचे(uchars==nullptr) {
        errorCode=U_MEMORY_ALLOCATION_ERROR;
    }
}

int32_t
UCharsTrieBuilder::getElementStringLength(int32_t i) ध्रुव {
    पटिनिवत्त elements[i].getStringLength(strings);
}

char16_t
UCharsTrieBuilder::getElementUnit(int32_t i, int32_t unitIndex) ध्रुव {
    पटिनिवत्त elements[i].charAt(unitIndex, strings);
}

int32_t
UCharsTrieBuilder::get_Element_मानम्(int32_t i) ध्रुव {
    पटिनिवत्त elements[i].get_मानम्();
}

int32_t
UCharsTrieBuilder::getLimitOfLinearMatch(int32_t first, int32_t last, int32_t unitIndex) ध्रुव {
    ध्रुव UCharsTrieElement &firstElement=elements[first];
    ध्रुव UCharsTrieElement &lastElement=elements[last];
    int32_t minStringLength=firstElement.getStringLength(strings);
    याव(++unitIndex<minStringLength &&
            firstElement.charAt(unitIndex, strings)==
            lastElement.charAt(unitIndex, strings)) {}
    पटिनिवत्त unitIndex;
}

int32_t
UCharsTrieBuilder::गणनम्_Element_Units(int32_t start, int32_t limit, int32_t unitIndex) ध्रुव {
    int32_t length=0;  // Number of different units at unitIndex.
    int32_t i=start;
    करोहि {
        char16_t unit=elements[i++].charAt(unitIndex, strings);
        याव(i<limit && unit==elements[i].charAt(unitIndex, strings)) {
            ++i;
        }
        ++length;
    } याव(i<limit);
    पटिनिवत्त length;
}

int32_t
UCharsTrieBuilder::skipElementsBySomeUnits(int32_t i, int32_t unitIndex, int32_t count) ध्रुव {
    करोहि {
        char16_t unit=elements[i++].charAt(unitIndex, strings);
        याव(unit==elements[i].charAt(unitIndex, strings)) {
            ++i;
        }
    } याव(--count>0);
    पटिनिवत्त i;
}

int32_t
UCharsTrieBuilder::indexOfElementWithNextUnit(int32_t i, int32_t unitIndex, char16_t unit) ध्रुव {
    याव(unit==elements[i].charAt(unitIndex, strings)) {
        ++i;
    }
    पटिनिवत्त i;
}

UCharsTrieBuilder::UCTLinear_Match_ग्रन्थिः::UCTLinear_Match_ग्रन्थिः(ध्रुव char16_t *units, int32_t len, Node *next_ग्रन्थिः)
        : Linear_Match_ग्रन्थिः(len, next_ग्रन्थिः), s(units) {
    hash=hash*37u+ustr_hashUCharsN(units, len);
}

तार्किक
UCharsTrieBuilder::UCTLinear_Match_ग्रन्थिः::operator==(ध्रुव Node &other) ध्रुव {
    सचे(एतत्==&other) {
        पटिनिवत्त सच्च;
    }
    सचे(!Linear_Match_ग्रन्थिः::operator==(other)) {
        पटिनिवत्त मिच्छा;
    }
    ध्रुव UCTLinear_Match_ग्रन्थिः &o=static_cast<ध्रुव UCTLinear_Match_ग्रन्थिः &>(other);
    पटिनिवत्त 0==u_memcmp(s, o.s, length);
}

शून्य
UCharsTrieBuilder::UCTLinear_Match_ग्रन्थिः::लिख(StringTrieBuilder &builder) {
    UCharsTrieBuilder &b=(UCharsTrieBuilder &)builder;
    next->लिख(builder);
    b.लिख(s, length);
    offset=b.write_मानम्_And_Type(has_मानम्, value, b.getMinLinearMatch()+length-1);
}

StringTrieBuilder::Node *
UCharsTrieBuilder::create_Linear_Match_ग्रन्थिः(int32_t i, int32_t unitIndex, int32_t length,
                                         Node *next_ग्रन्थिः) ध्रुव {
    पटिनिवत्त नूतन UCTLinear_Match_ग्रन्थिः(
            elements[i].getString(strings).getBuffer()+unitIndex,
            length,
            next_ग्रन्थिः);
}

UBool
UCharsTrieBuilder::ensureCapacity(int32_t length) {
    सचे(uchars==nullptr) {
        पटिनिवत्त मिच्छा;  // previous memory allocation had failed
    }
    सचे(length>ucharsCapacity) {
        int32_t newCapacity=ucharsCapacity;
        करोहि {
            newCapacity*=2;
        } याव(newCapacity<=length);
        char16_t *newUChars=static_cast<char16_t *>(uprv_malloc(newCapacity*2));
        सचे(newUChars==nullptr) {
            // unable to allocate memory
            uprv_free(uchars);
            uchars=nullptr;
            ucharsCapacity=0;
            पटिनिवत्त मिच्छा;
        }
        u_memcpy(newUChars+(newCapacity-ucharsLength),
                 uchars+(ucharsCapacity-ucharsLength), ucharsLength);
        uprv_free(uchars);
        uchars=newUChars;
        ucharsCapacity=newCapacity;
    }
    पटिनिवत्त सच्च;
}

int32_t
UCharsTrieBuilder::लिख(int32_t unit) {
    int32_t newLength=ucharsLength+1;
    सचे(ensureCapacity(newLength)) {
        ucharsLength=newLength;
        uchars[ucharsCapacity - ucharsLength] = static_cast<char16_t>(unit);
    }
    पटिनिवत्त ucharsLength;
}

int32_t
UCharsTrieBuilder::लिख(ध्रुव char16_t *s, int32_t length) {
    int32_t newLength=ucharsLength+length;
    सचे(ensureCapacity(newLength)) {
        ucharsLength=newLength;
        u_memcpy(uchars+(ucharsCapacity-ucharsLength), s, length);
    }
    पटिनिवत्त ucharsLength;
}

int32_t
UCharsTrieBuilder::writeElementUnits(int32_t i, int32_t unitIndex, int32_t length) {
    पटिनिवत्त लिख(elements[i].getString(strings).getBuffer()+unitIndex, length);
}

int32_t
UCharsTrieBuilder::write_मानम्_And_Final(int32_t i, UBool isFinal) {
    सचे(0<=i && i<=UCharsTrie::k_Max_One_Unit_मानम्) {
        पटिनिवत्त लिख(i|(isFinal<<15));
    }
    char16_t intUnits[3];
    int32_t length;
    सचे(i<0 || i>UCharsTrie::k_Max_Two_Unit_मानम्) {
        intUnits[0] = static_cast<char16_t>(UCharsTrie::k_Three_Unit_मानम्_Lead);
        intUnits[1] = static_cast<char16_t>(static_cast<uint32_t>(i) >> 16);
        intUnits[2] = static_cast<char16_t>(i);
        length=3;
    // } else if(i<=UCharsTrie::kMaxOneUnitValue) {
    //     intUnits[0]=(char16_t)(i);
    //     length=1;
    } अञ्ञथा {
        intUnits[0] = static_cast<char16_t>(UCharsTrie::k_Min_Two_Unit_मानम्_Lead + (i >> 16));
        intUnits[1] = static_cast<char16_t>(i);
        length=2;
    }
    intUnits[0] = static_cast<char16_t>(intUnits[0] | (isFinal << 15));
    पटिनिवत्त लिख(intUnits, length);
}

int32_t
UCharsTrieBuilder::write_मानम्_And_Type(UBool has_मानम्, int32_t value, int32_t ग्रन्थिः) {
    सचे(!has_मानम्) {
        पटिनिवत्त लिख(ग्रन्थिः);
    }
    char16_t intUnits[3];
    int32_t length;
    सचे(value<0 || value>UCharsTrie::k_Max_Two_Unit_ग्रन्थिः_मानम्) {
        intUnits[0] = static_cast<char16_t>(UCharsTrie::k_Three_Unit_ग्रन्थिः_मानम्_Lead);
        intUnits[1] = static_cast<char16_t>(static_cast<uint32_t>(value) >> 16);
        intUnits[2] = static_cast<char16_t>(value);
        length=3;
    } अञ्ञथा सचे(value<=UCharsTrie::k_Max_One_Unit_ग्रन्थिः_मानम्) {
        intUnits[0] = static_cast<char16_t>((value + 1) << 6);
        length=1;
    } अञ्ञथा {
        intUnits[0] = static_cast<char16_t>(UCharsTrie::k_Min_Two_Unit_ग्रन्थिः_मानम्_Lead + ((value >> 10) & 0x7fc0));
        intUnits[1] = static_cast<char16_t>(value);
        length=2;
    }
    intUnits[0] |= static_cast<char16_t>(ग्रन्थिः);
    पटिनिवत्त लिख(intUnits, length);
}

int32_t
UCharsTrieBuilder::writeDeltaTo(int32_t jumpTarget) {
    int32_t i=ucharsLength-jumpTarget;
    U_ASSERT(i>=0);
    सचे(i<=UCharsTrie::kMaxOneUnitDelta) {
        पटिनिवत्त लिख(i);
    }
    char16_t intUnits[3];
    int32_t length;
    सचे(i<=UCharsTrie::kMaxTwoUnitDelta) {
        intUnits[0] = static_cast<char16_t>(UCharsTrie::kMinTwoUnitDeltaLead + (i >> 16));
        length=1;
    } अञ्ञथा {
        intUnits[0] = static_cast<char16_t>(UCharsTrie::kThreeUnitDeltaLead);
        intUnits[1] = static_cast<char16_t>(i >> 16);
        length=2;
    }
    intUnits[length++] = static_cast<char16_t>(i);
    पटिनिवत्त लिख(intUnits, length);
}

U_NAMESPACE_END
