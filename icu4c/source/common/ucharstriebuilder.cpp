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

    शून्य setTo(स्थिरांक UnicodeString &s, int32_t val, UnicodeString &strings, UErrorCode &errorCode);

    UnicodeString getString(स्थिरांक UnicodeString& strings U_LIFETIME_BOUND) स्थिरांक {
        int32_t length=strings[stringOffset];
        परत strings.tempSubString(stringOffset+1, length);
    }
    int32_t getStringLength(स्थिरांक UnicodeString &strings) स्थिरांक {
        परत strings[stringOffset];
    }

    char16_t charAt(int32_t index, स्थिरांक UnicodeString &strings) स्थिरांक {
        परत strings[stringOffset+1+index];
    }

    int32_t get_मूल्य() स्थिरांक { परत value; }

    int32_t compareStringTo(स्थिरांक UCharsTrieElement &o, स्थिरांक UnicodeString &strings) स्थिरांक;

खाजगी:
    // The first strings unit contains the string length.
    // (Compared with a stringLength field here, this saves 2 bytes per string.)
    int32_t stringOffset;
    int32_t value;
};

शून्य
UCharsTrieElement::setTo(स्थिरांक UnicodeString &s, int32_t val,
                         UnicodeString &strings, UErrorCode &errorCode) {
    जर(U_FAILURE(errorCode)) {
        परत;
    }
    int32_t length=s.length();
    जर(length>0xffff) {
        // Too long: We store the length in 1 unit.
        errorCode=U_INDEX_OUTOFBOUNDS_ERROR;
        परत;
    }
    stringOffset=strings.length();
    strings.append(static_cast<char16_t>(length));
    value=val;
    strings.append(s);
}

int32_t
UCharsTrieElement::compareStringTo(स्थिरांक UCharsTrieElement &other, स्थिरांक UnicodeString &strings) स्थिरांक {
    परत getString(strings).compare(other.getString(strings));
}

UCharsTrieBuilder::UCharsTrieBuilder(UErrorCode & /*errorCode*/)
        : elements(nullptr), elementsCapacity(0), elementsLength(0),
          uchars(nullptr), ucharsCapacity(0), ucharsLength(0) {}

UCharsTrieBuilder::~UCharsTrieBuilder() {
    delete[] elements;
    uprv_free(uchars);
}

UCharsTrieBuilder&
UCharsTrieBuilder::add(स्थिरांक UnicodeString& s,
                       int32_t value,
                       UErrorCode& errorCode) U_LIFETIME_BOUND {
    जर(U_FAILURE(errorCode)) {
        परत *हे;
    }
    जर(ucharsLength>0) {
        // Cannot add elements after building.
        errorCode=U_NO_WRITE_PERMISSION;
        परत *हे;
    }
    जर(elementsLength==elementsCapacity) {
        int32_t newCapacity;
        जर(elementsCapacity==0) {
            newCapacity=1024;
        } अन्यथा {
            newCapacity=4*elementsCapacity;
        }
        UCharsTrieElement *newElements=नवीन UCharsTrieElement[newCapacity];
        जर(newElements==nullptr) {
            errorCode=U_MEMORY_ALLOCATION_ERROR;
            परत *हे;
        }
        जर(elementsLength>0) {
            uprv_memcpy(newElements, elements, (माप_प्रकार)elementsLength*आकार(UCharsTrieElement));
        }
        delete[] elements;
        elements=newElements;
        elementsCapacity=newCapacity;
    }
    elements[elementsLength++].setTo(s, value, strings, errorCode);
    जर(U_SUCCESS(errorCode) && strings.isBogus()) {
        errorCode=U_MEMORY_ALLOCATION_ERROR;
    }
    परत *हे;
}

U_CDECL_BEGIN

स्थिर int32_t U_CALLCONV
compareElementStrings(स्थिरांक शून्य *context, स्थिरांक शून्य *left, स्थिरांक शून्य *उजवा) {
    स्थिरांक UnicodeString *strings=static_cast<स्थिरांक UnicodeString *>(context);
    स्थिरांक UCharsTrieElement *डावा_Element=static_cast<स्थिरांक UCharsTrieElement *>(left);
    स्थिरांक UCharsTrieElement *उजवा_Element=static_cast<स्थिरांक UCharsTrieElement *>(उजवा);
    परत डावा_Element->compareStringTo(*उजवा_Element, *strings);
}

U_CDECL_END

UCharsTrie *
UCharsTrieBuilder::build(UStringTrieBuildOption buildOption, UErrorCode &errorCode) {
    buildUChars(buildOption, errorCode);
    UCharsTrie *newTrie=nullptr;
    जर(U_SUCCESS(errorCode)) {
        newTrie=नवीन UCharsTrie(uchars, uchars+(ucharsCapacity-ucharsLength));
        जर(newTrie==nullptr) {
            errorCode=U_MEMORY_ALLOCATION_ERROR;
        } अन्यथा {
            uchars=nullptr;  // The new trie now owns the array.
            ucharsCapacity=0;
        }
    }
    परत newTrie;
}

UnicodeString&
UCharsTrieBuilder::buildUnicodeString(UStringTrieBuildOption buildOption,
                                      UnicodeString& result U_LIFETIME_BOUND,
                                      UErrorCode& errorCode) U_LIFETIME_BOUND {
    buildUChars(buildOption, errorCode);
    जर(U_SUCCESS(errorCode)) {
        result.setTo(असत्य, uchars+(ucharsCapacity-ucharsLength), ucharsLength);
    }
    परत result;
}

शून्य
UCharsTrieBuilder::buildUChars(UStringTrieBuildOption buildOption, UErrorCode &errorCode) {
    जर(U_FAILURE(errorCode)) {
        परत;
    }
    जर(uchars!=nullptr && ucharsLength>0) {
        // Already built.
        परत;
    }
    जर(ucharsLength==0) {
        जर(elementsLength==0) {
            errorCode=U_INDEX_OUTOFBOUNDS_ERROR;
            परत;
        }
        जर(strings.isBogus()) {
            errorCode=U_MEMORY_ALLOCATION_ERROR;
            परत;
        }
        uprv_sortArray(elements, elementsLength, static_cast<int32_t>(आकार(UCharsTrieElement)),
                      compareElementStrings, &strings,
                      असत्य,  // need not be a stable sort
                      &errorCode);
        जर(U_FAILURE(errorCode)) {
            परत;
        }
        // Duplicate strings are not allowed.
        UnicodeString prev=elements[0].getString(strings);
        साठी(int32_t i=1; i<elementsLength; ++i) {
            UnicodeString current=elements[i].getString(strings);
            जर(prev==current) {
                errorCode=U_ILLEGAL_ARGUMENT_ERROR;
                परत;
            }
            prev.fastCopyFrom(current);
        }
    }
    // Create and char16_t-serialize the trie for the elements.
    ucharsLength=0;
    int32_t capacity=strings.length();
    जर(capacity<1024) {
        capacity=1024;
    }
    जर(ucharsCapacity<capacity) {
        uprv_free(uchars);
        uchars=static_cast<char16_t *>(uprv_malloc(capacity*2));
        जर(uchars==nullptr) {
            errorCode=U_MEMORY_ALLOCATION_ERROR;
            ucharsCapacity=0;
            परत;
        }
        ucharsCapacity=capacity;
    }
    StringTrieBuilder::build(buildOption, elementsLength, errorCode);
    जर(uchars==nullptr) {
        errorCode=U_MEMORY_ALLOCATION_ERROR;
    }
}

int32_t
UCharsTrieBuilder::getElementStringLength(int32_t i) स्थिरांक {
    परत elements[i].getStringLength(strings);
}

char16_t
UCharsTrieBuilder::getElementUnit(int32_t i, int32_t unitIndex) स्थिरांक {
    परत elements[i].charAt(unitIndex, strings);
}

int32_t
UCharsTrieBuilder::get_Element_मूल्य(int32_t i) स्थिरांक {
    परत elements[i].get_मूल्य();
}

int32_t
UCharsTrieBuilder::getLimitOfLinearMatch(int32_t first, int32_t last, int32_t unitIndex) स्थिरांक {
    स्थिरांक UCharsTrieElement &firstElement=elements[first];
    स्थिरांक UCharsTrieElement &lastElement=elements[last];
    int32_t minStringLength=firstElement.getStringLength(strings);
    पर्यंत(++unitIndex<minStringLength &&
            firstElement.charAt(unitIndex, strings)==
            lastElement.charAt(unitIndex, strings)) {}
    परत unitIndex;
}

int32_t
UCharsTrieBuilder::मोजणी_Element_Units(int32_t start, int32_t limit, int32_t unitIndex) स्थिरांक {
    int32_t length=0;  // Number of different units at unitIndex.
    int32_t i=start;
    करा {
        char16_t unit=elements[i++].charAt(unitIndex, strings);
        पर्यंत(i<limit && unit==elements[i].charAt(unitIndex, strings)) {
            ++i;
        }
        ++length;
    } पर्यंत(i<limit);
    परत length;
}

int32_t
UCharsTrieBuilder::skipElementsBySomeUnits(int32_t i, int32_t unitIndex, int32_t count) स्थिरांक {
    करा {
        char16_t unit=elements[i++].charAt(unitIndex, strings);
        पर्यंत(unit==elements[i].charAt(unitIndex, strings)) {
            ++i;
        }
    } पर्यंत(--count>0);
    परत i;
}

int32_t
UCharsTrieBuilder::indexOfElementWithNextUnit(int32_t i, int32_t unitIndex, char16_t unit) स्थिरांक {
    पर्यंत(unit==elements[i].charAt(unitIndex, strings)) {
        ++i;
    }
    परत i;
}

UCharsTrieBuilder::UCTLinear_Match_गाठ::UCTLinear_Match_गाठ(स्थिरांक char16_t *units, int32_t len, Node *next_गाठ)
        : Linear_Match_गाठ(len, next_गाठ), s(units) {
    hash=hash*37u+ustr_hashUCharsN(units, len);
}

तार्किक
UCharsTrieBuilder::UCTLinear_Match_गाठ::चालक==(स्थिरांक Node &other) स्थिरांक {
    जर(हे==&other) {
        परत सत्य;
    }
    जर(!Linear_Match_गाठ::चालक==(other)) {
        परत असत्य;
    }
    स्थिरांक UCTLinear_Match_गाठ &o=static_cast<स्थिरांक UCTLinear_Match_गाठ &>(other);
    परत 0==u_memcmp(s, o.s, length);
}

शून्य
UCharsTrieBuilder::UCTLinear_Match_गाठ::लिख(StringTrieBuilder &builder) {
    UCharsTrieBuilder &b=(UCharsTrieBuilder &)builder;
    next->लिख(builder);
    b.लिख(s, length);
    offset=b.write_मूल्य_And_Type(has_मूल्य, value, b.getMinLinearMatch()+length-1);
}

StringTrieBuilder::Node *
UCharsTrieBuilder::create_Linear_Match_गाठ(int32_t i, int32_t unitIndex, int32_t length,
                                         Node *next_गाठ) स्थिरांक {
    परत नवीन UCTLinear_Match_गाठ(
            elements[i].getString(strings).getBuffer()+unitIndex,
            length,
            next_गाठ);
}

UBool
UCharsTrieBuilder::ensureCapacity(int32_t length) {
    जर(uchars==nullptr) {
        परत असत्य;  // previous memory allocation had failed
    }
    जर(length>ucharsCapacity) {
        int32_t newCapacity=ucharsCapacity;
        करा {
            newCapacity*=2;
        } पर्यंत(newCapacity<=length);
        char16_t *newUChars=static_cast<char16_t *>(uprv_malloc(newCapacity*2));
        जर(newUChars==nullptr) {
            // unable to allocate memory
            uprv_free(uchars);
            uchars=nullptr;
            ucharsCapacity=0;
            परत असत्य;
        }
        u_memcpy(newUChars+(newCapacity-ucharsLength),
                 uchars+(ucharsCapacity-ucharsLength), ucharsLength);
        uprv_free(uchars);
        uchars=newUChars;
        ucharsCapacity=newCapacity;
    }
    परत सत्य;
}

int32_t
UCharsTrieBuilder::लिख(int32_t unit) {
    int32_t newLength=ucharsLength+1;
    जर(ensureCapacity(newLength)) {
        ucharsLength=newLength;
        uchars[ucharsCapacity - ucharsLength] = static_cast<char16_t>(unit);
    }
    परत ucharsLength;
}

int32_t
UCharsTrieBuilder::लिख(स्थिरांक char16_t *s, int32_t length) {
    int32_t newLength=ucharsLength+length;
    जर(ensureCapacity(newLength)) {
        ucharsLength=newLength;
        u_memcpy(uchars+(ucharsCapacity-ucharsLength), s, length);
    }
    परत ucharsLength;
}

int32_t
UCharsTrieBuilder::writeElementUnits(int32_t i, int32_t unitIndex, int32_t length) {
    परत लिख(elements[i].getString(strings).getBuffer()+unitIndex, length);
}

int32_t
UCharsTrieBuilder::write_मूल्य_And_Final(int32_t i, UBool isFinal) {
    जर(0<=i && i<=UCharsTrie::k_Max_One_Unit_मूल्य) {
        परत लिख(i|(isFinal<<15));
    }
    char16_t intUnits[3];
    int32_t length;
    जर(i<0 || i>UCharsTrie::k_Max_Two_Unit_मूल्य) {
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
    परत लिख(intUnits, length);
}

int32_t
UCharsTrieBuilder::write_मूल्य_And_Type(UBool has_मूल्य, int32_t value, int32_t गाठ) {
    जर(!has_मूल्य) {
        परत लिख(गाठ);
    }
    char16_t intUnits[3];
    int32_t length;
    जर(value<0 || value>UCharsTrie::k_Max_Two_Unit_गाठ_मूल्य) {
        intUnits[0] = static_cast<char16_t>(UCharsTrie::k_Three_Unit_गाठ_मूल्य_Lead);
        intUnits[1] = static_cast<char16_t>(static_cast<uint32_t>(value) >> 16);
        intUnits[2] = static_cast<char16_t>(value);
        length=3;
    } अन्यथा जर(value<=UCharsTrie::k_Max_One_Unit_गाठ_मूल्य) {
        intUnits[0] = static_cast<char16_t>((value + 1) << 6);
        length=1;
    } अन्यथा {
        intUnits[0] = static_cast<char16_t>(UCharsTrie::k_Min_Two_Unit_गाठ_मूल्य_Lead + ((value >> 10) & 0x7fc0));
        intUnits[1] = static_cast<char16_t>(value);
        length=2;
    }
    intUnits[0] |= static_cast<char16_t>(गाठ);
    परत लिख(intUnits, length);
}

int32_t
UCharsTrieBuilder::writeDeltaTo(int32_t jumpTarget) {
    int32_t i=ucharsLength-jumpTarget;
    U_ASSERT(i>=0);
    जर(i<=UCharsTrie::kMaxOneUnitDelta) {
        परत लिख(i);
    }
    char16_t intUnits[3];
    int32_t length;
    जर(i<=UCharsTrie::kMaxTwoUnitDelta) {
        intUnits[0] = static_cast<char16_t>(UCharsTrie::kMinTwoUnitDeltaLead + (i >> 16));
        length=1;
    } अन्यथा {
        intUnits[0] = static_cast<char16_t>(UCharsTrie::kThreeUnitDeltaLead);
        intUnits[1] = static_cast<char16_t>(i >> 16);
        length=2;
    }
    intUnits[length++] = static_cast<char16_t>(i);
    परत लिख(intUnits, length);
}

U_NAMESPACE_END
