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

    शून्य setTo(अचर UnicodeString &s, int32_t val, UnicodeString &strings, UErrorCode &errorCode);

    UnicodeString getString(अचर UnicodeString& strings U_LIFETIME_BOUND) अचर {
        int32_t length=strings[stringOffset];
        फर्काऊ strings.tempSubString(stringOffset+1, length);
    }
    int32_t getStringLength(अचर UnicodeString &strings) अचर {
        फर्काऊ strings[stringOffset];
    }

    char16_t charAt(int32_t index, अचर UnicodeString &strings) अचर {
        फर्काऊ strings[stringOffset+1+index];
    }

    int32_t get_मान() अचर { फर्काऊ value; }

    int32_t compareStringTo(अचर UCharsTrieElement &o, अचर UnicodeString &strings) अचर;

गुप्त:
    // The first strings unit contains the string length.
    // (Compared with a stringLength field here, this saves 2 bytes per string.)
    int32_t stringOffset;
    int32_t value;
};

शून्य
UCharsTrieElement::setTo(अचर UnicodeString &s, int32_t val,
                         UnicodeString &strings, UErrorCode &errorCode) {
    यदि(U_FAILURE(errorCode)) {
        फर्काऊ;
    }
    int32_t length=s.length();
    यदि(length>0xffff) {
        // Too long: We store the length in 1 unit.
        errorCode=U_INDEX_OUTOFBOUNDS_ERROR;
        फर्काऊ;
    }
    stringOffset=strings.length();
    strings.append(static_cast<char16_t>(length));
    value=val;
    strings.append(s);
}

int32_t
UCharsTrieElement::compareStringTo(अचर UCharsTrieElement &other, अचर UnicodeString &strings) अचर {
    फर्काऊ getString(strings).compare(other.getString(strings));
}

UCharsTrieBuilder::UCharsTrieBuilder(UErrorCode & /*errorCode*/)
        : elements(nullptr), elementsCapacity(0), elementsLength(0),
          uchars(nullptr), ucharsCapacity(0), ucharsLength(0) {}

UCharsTrieBuilder::~UCharsTrieBuilder() {
    delete[] elements;
    uprv_free(uchars);
}

UCharsTrieBuilder&
UCharsTrieBuilder::add(अचर UnicodeString& s,
                       int32_t value,
                       UErrorCode& errorCode) U_LIFETIME_BOUND {
    यदि(U_FAILURE(errorCode)) {
        फर्काऊ *एतत्;
    }
    यदि(ucharsLength>0) {
        // Cannot add elements after building.
        errorCode=U_NO_WRITE_PERMISSION;
        फर्काऊ *एतत्;
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
            फर्काऊ *एतत्;
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
    फर्काऊ *एतत्;
}

U_CDECL_BEGIN

स्थिर int32_t U_CALLCONV
compareElementStrings(अचर शून्य *context, अचर शून्य *left, अचर शून्य *दायाँ) {
    अचर UnicodeString *strings=static_cast<अचर UnicodeString *>(context);
    अचर UCharsTrieElement *बायाँ_Element=static_cast<अचर UCharsTrieElement *>(left);
    अचर UCharsTrieElement *दायाँ_Element=static_cast<अचर UCharsTrieElement *>(दायाँ);
    फर्काऊ बायाँ_Element->compareStringTo(*दायाँ_Element, *strings);
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
    फर्काऊ newTrie;
}

UnicodeString&
UCharsTrieBuilder::buildUnicodeString(UStringTrieBuildOption buildOption,
                                      UnicodeString& result U_LIFETIME_BOUND,
                                      UErrorCode& errorCode) U_LIFETIME_BOUND {
    buildUChars(buildOption, errorCode);
    यदि(U_SUCCESS(errorCode)) {
        result.setTo(असत्य, uchars+(ucharsCapacity-ucharsLength), ucharsLength);
    }
    फर्काऊ result;
}

शून्य
UCharsTrieBuilder::buildUChars(UStringTrieBuildOption buildOption, UErrorCode &errorCode) {
    यदि(U_FAILURE(errorCode)) {
        फर्काऊ;
    }
    यदि(uchars!=nullptr && ucharsLength>0) {
        // Already built.
        फर्काऊ;
    }
    यदि(ucharsLength==0) {
        यदि(elementsLength==0) {
            errorCode=U_INDEX_OUTOFBOUNDS_ERROR;
            फर्काऊ;
        }
        यदि(strings.isBogus()) {
            errorCode=U_MEMORY_ALLOCATION_ERROR;
            फर्काऊ;
        }
        uprv_sortArray(elements, elementsLength, static_cast<int32_t>(परिमाण(UCharsTrieElement)),
                      compareElementStrings, &strings,
                      असत्य,  // need not be a stable sort
                      &errorCode);
        यदि(U_FAILURE(errorCode)) {
            फर्काऊ;
        }
        // Duplicate strings are not allowed.
        UnicodeString prev=elements[0].getString(strings);
        लागि(int32_t i=1; i<elementsLength; ++i) {
            UnicodeString current=elements[i].getString(strings);
            यदि(prev==current) {
                errorCode=U_ILLEGAL_ARGUMENT_ERROR;
                फर्काऊ;
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
            फर्काऊ;
        }
        ucharsCapacity=capacity;
    }
    StringTrieBuilder::build(buildOption, elementsLength, errorCode);
    यदि(uchars==nullptr) {
        errorCode=U_MEMORY_ALLOCATION_ERROR;
    }
}

int32_t
UCharsTrieBuilder::getElementStringLength(int32_t i) अचर {
    फर्काऊ elements[i].getStringLength(strings);
}

char16_t
UCharsTrieBuilder::getElementUnit(int32_t i, int32_t unitIndex) अचर {
    फर्काऊ elements[i].charAt(unitIndex, strings);
}

int32_t
UCharsTrieBuilder::get_Element_मान(int32_t i) अचर {
    फर्काऊ elements[i].get_मान();
}

int32_t
UCharsTrieBuilder::getLimitOfLinearMatch(int32_t first, int32_t last, int32_t unitIndex) अचर {
    अचर UCharsTrieElement &firstElement=elements[first];
    अचर UCharsTrieElement &lastElement=elements[last];
    int32_t minStringLength=firstElement.getStringLength(strings);
    जबसम्म(++unitIndex<minStringLength &&
            firstElement.charAt(unitIndex, strings)==
            lastElement.charAt(unitIndex, strings)) {}
    फर्काऊ unitIndex;
}

int32_t
UCharsTrieBuilder::गन्ती_Element_Units(int32_t start, int32_t limit, int32_t unitIndex) अचर {
    int32_t length=0;  // Number of different units at unitIndex.
    int32_t i=start;
    गर {
        char16_t unit=elements[i++].charAt(unitIndex, strings);
        जबसम्म(i<limit && unit==elements[i].charAt(unitIndex, strings)) {
            ++i;
        }
        ++length;
    } जबसम्म(i<limit);
    फर्काऊ length;
}

int32_t
UCharsTrieBuilder::skipElementsBySomeUnits(int32_t i, int32_t unitIndex, int32_t count) अचर {
    गर {
        char16_t unit=elements[i++].charAt(unitIndex, strings);
        जबसम्म(unit==elements[i].charAt(unitIndex, strings)) {
            ++i;
        }
    } जबसम्म(--count>0);
    फर्काऊ i;
}

int32_t
UCharsTrieBuilder::indexOfElementWithNextUnit(int32_t i, int32_t unitIndex, char16_t unit) अचर {
    जबसम्म(unit==elements[i].charAt(unitIndex, strings)) {
        ++i;
    }
    फर्काऊ i;
}

UCharsTrieBuilder::UCTLinear_Match_गाँठो::UCTLinear_Match_गाँठो(अचर char16_t *units, int32_t len, Node *next_गाँठो)
        : Linear_Match_गाँठो(len, next_गाँठो), s(units) {
    hash=hash*37u+ustr_hashUCharsN(units, len);
}

तार्किक
UCharsTrieBuilder::UCTLinear_Match_गाँठो::चालक==(अचर Node &other) अचर {
    यदि(एतत्==&other) {
        फर्काऊ सत्य;
    }
    यदि(!Linear_Match_गाँठो::चालक==(other)) {
        फर्काऊ असत्य;
    }
    अचर UCTLinear_Match_गाँठो &o=static_cast<अचर UCTLinear_Match_गाँठो &>(other);
    फर्काऊ 0==u_memcmp(s, o.s, length);
}

शून्य
UCharsTrieBuilder::UCTLinear_Match_गाँठो::लिख(StringTrieBuilder &builder) {
    UCharsTrieBuilder &b=(UCharsTrieBuilder &)builder;
    next->लिख(builder);
    b.लिख(s, length);
    offset=b.write_मान_And_Type(has_मान, value, b.getMinLinearMatch()+length-1);
}

StringTrieBuilder::Node *
UCharsTrieBuilder::create_Linear_Match_गाँठो(int32_t i, int32_t unitIndex, int32_t length,
                                         Node *next_गाँठो) अचर {
    फर्काऊ नूतन UCTLinear_Match_गाँठो(
            elements[i].getString(strings).getBuffer()+unitIndex,
            length,
            next_गाँठो);
}

UBool
UCharsTrieBuilder::ensureCapacity(int32_t length) {
    यदि(uchars==nullptr) {
        फर्काऊ असत्य;  // previous memory allocation had failed
    }
    यदि(length>ucharsCapacity) {
        int32_t newCapacity=ucharsCapacity;
        गर {
            newCapacity*=2;
        } जबसम्म(newCapacity<=length);
        char16_t *newUChars=static_cast<char16_t *>(uprv_malloc(newCapacity*2));
        यदि(newUChars==nullptr) {
            // unable to allocate memory
            uprv_free(uchars);
            uchars=nullptr;
            ucharsCapacity=0;
            फर्काऊ असत्य;
        }
        u_memcpy(newUChars+(newCapacity-ucharsLength),
                 uchars+(ucharsCapacity-ucharsLength), ucharsLength);
        uprv_free(uchars);
        uchars=newUChars;
        ucharsCapacity=newCapacity;
    }
    फर्काऊ सत्य;
}

int32_t
UCharsTrieBuilder::लिख(int32_t unit) {
    int32_t newLength=ucharsLength+1;
    यदि(ensureCapacity(newLength)) {
        ucharsLength=newLength;
        uchars[ucharsCapacity - ucharsLength] = static_cast<char16_t>(unit);
    }
    फर्काऊ ucharsLength;
}

int32_t
UCharsTrieBuilder::लिख(अचर char16_t *s, int32_t length) {
    int32_t newLength=ucharsLength+length;
    यदि(ensureCapacity(newLength)) {
        ucharsLength=newLength;
        u_memcpy(uchars+(ucharsCapacity-ucharsLength), s, length);
    }
    फर्काऊ ucharsLength;
}

int32_t
UCharsTrieBuilder::writeElementUnits(int32_t i, int32_t unitIndex, int32_t length) {
    फर्काऊ लिख(elements[i].getString(strings).getBuffer()+unitIndex, length);
}

int32_t
UCharsTrieBuilder::write_मान_And_Final(int32_t i, UBool isFinal) {
    यदि(0<=i && i<=UCharsTrie::k_Max_One_Unit_मान) {
        फर्काऊ लिख(i|(isFinal<<15));
    }
    char16_t intUnits[3];
    int32_t length;
    यदि(i<0 || i>UCharsTrie::k_Max_Two_Unit_मान) {
        intUnits[0] = static_cast<char16_t>(UCharsTrie::k_Three_Unit_मान_Lead);
        intUnits[1] = static_cast<char16_t>(static_cast<uint32_t>(i) >> 16);
        intUnits[2] = static_cast<char16_t>(i);
        length=3;
    // } else if(i<=UCharsTrie::kMaxOneUnitValue) {
    //     intUnits[0]=(char16_t)(i);
    //     length=1;
    } अन्यथा {
        intUnits[0] = static_cast<char16_t>(UCharsTrie::k_Min_Two_Unit_मान_Lead + (i >> 16));
        intUnits[1] = static_cast<char16_t>(i);
        length=2;
    }
    intUnits[0] = static_cast<char16_t>(intUnits[0] | (isFinal << 15));
    फर्काऊ लिख(intUnits, length);
}

int32_t
UCharsTrieBuilder::write_मान_And_Type(UBool has_मान, int32_t value, int32_t गाँठो) {
    यदि(!has_मान) {
        फर्काऊ लिख(गाँठो);
    }
    char16_t intUnits[3];
    int32_t length;
    यदि(value<0 || value>UCharsTrie::k_Max_Two_Unit_गाँठो_मान) {
        intUnits[0] = static_cast<char16_t>(UCharsTrie::k_Three_Unit_गाँठो_मान_Lead);
        intUnits[1] = static_cast<char16_t>(static_cast<uint32_t>(value) >> 16);
        intUnits[2] = static_cast<char16_t>(value);
        length=3;
    } अन्यथा यदि(value<=UCharsTrie::k_Max_One_Unit_गाँठो_मान) {
        intUnits[0] = static_cast<char16_t>((value + 1) << 6);
        length=1;
    } अन्यथा {
        intUnits[0] = static_cast<char16_t>(UCharsTrie::k_Min_Two_Unit_गाँठो_मान_Lead + ((value >> 10) & 0x7fc0));
        intUnits[1] = static_cast<char16_t>(value);
        length=2;
    }
    intUnits[0] |= static_cast<char16_t>(गाँठो);
    फर्काऊ लिख(intUnits, length);
}

int32_t
UCharsTrieBuilder::writeDeltaTo(int32_t jumpTarget) {
    int32_t i=ucharsLength-jumpTarget;
    U_ASSERT(i>=0);
    यदि(i<=UCharsTrie::kMaxOneUnitDelta) {
        फर्काऊ लिख(i);
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
    फर्काऊ लिख(intUnits, length);
}

U_NAMESPACE_END
