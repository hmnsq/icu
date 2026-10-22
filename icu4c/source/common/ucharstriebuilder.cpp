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

#समावेश "unicode/utypes.h"
#समावेश "unicode/ucharstrie.h"
#समावेश "unicode/ucharstriebuilder.h"
#समावेश "unicode/unistr.h"
#समावेश "unicode/ustring.h"
#समावेश "cmemory.h"
#समावेश "uarrsort.h"
#समावेश "uassert.h"
#समावेश "uhash.h"
#समावेश "ustr_imp.h"

U_NAMESPACE_BEGIN

/*
 * Note: This builder implementation stores (string, value) pairs with full copies
 * of the 16-bit-unit sequences, until the UCharsTrie is built.
 * It might(!) take less memory if we collected the data in a temporary, dynamic trie.
 */

श्रेणी UCharsTrieElement : खुला UMemory {
खुला:
    // Use compiler's default constructor, initializes nothing.

    व्योम setTo(स्थिर UnicodeString &s, int32_t val, UnicodeString &strings, UErrorCode &errorCode);

    UnicodeString getString(स्थिर UnicodeString& strings U_LIFETIME_BOUND) स्थिर {
        int32_t length=strings[stringOffset];
        वापस strings.अस्थायी_Sub_String(stringOffset+1, length);
    }
    int32_t get_String_लंबाई(स्थिर UnicodeString &strings) स्थिर {
        वापस strings[stringOffset];
    }

    char16_t charAt(int32_t सूचकांक, स्थिर UnicodeString &strings) स्थिर {
        वापस strings[stringOffset+1+सूचकांक];
    }

    int32_t get_मूल्य() स्थिर { वापस value; }

    int32_t compareStringTo(स्थिर UCharsTrieElement &o, स्थिर UnicodeString &strings) स्थिर;

निजी:
    // The first strings unit contains the string length.
    // (Compared with a stringLength field here, this saves 2 bytes per string.)
    int32_t stringOffset;
    int32_t value;
};

व्योम
UCharsTrieElement::setTo(स्थिर UnicodeString &s, int32_t val,
                         UnicodeString &strings, UErrorCode &errorCode) {
    अगर(U_FAILURE(errorCode)) {
        वापस;
    }
    int32_t length=s.length();
    अगर(length>0xffff) {
        // Too long: We store the length in 1 unit.
        errorCode=U_सूचकांक_OUTOFBOUNDS_ERROR;
        वापस;
    }
    stringOffset=strings.length();
    strings.append(static_cast<char16_t>(length));
    value=val;
    strings.append(s);
}

int32_t
UCharsTrieElement::compareStringTo(स्थिर UCharsTrieElement &other, स्थिर UnicodeString &strings) स्थिर {
    वापस getString(strings).compare(other.getString(strings));
}

UCharsTrieBuilder::UCharsTrieBuilder(UErrorCode & /*errorCode*/)
        : elements(nullptr), elementsCapacity(0), elements_लंबाई(0),
          uchars(nullptr), ucharsCapacity(0), uchars_लंबाई(0) {}

UCharsTrieBuilder::~UCharsTrieBuilder() {
    मिटाओ[] elements;
    uprv_free(uchars);
}

UCharsTrieBuilder&
UCharsTrieBuilder::add(स्थिर UnicodeString& s,
                       int32_t value,
                       UErrorCode& errorCode) U_LIFETIME_BOUND {
    अगर(U_FAILURE(errorCode)) {
        वापस *यह;
    }
    अगर(uchars_लंबाई>0) {
        // Cannot add elements after building.
        errorCode=U_NO_WRITE_PERMISSION;
        वापस *यह;
    }
    अगर(elements_लंबाई==elementsCapacity) {
        int32_t newCapacity;
        अगर(elementsCapacity==0) {
            newCapacity=1024;
        } अन्यथा {
            newCapacity=4*elementsCapacity;
        }
        UCharsTrieElement *newElements=नया UCharsTrieElement[newCapacity];
        अगर(newElements==nullptr) {
            errorCode=U_MEMORY_ALLOCATION_ERROR;
            वापस *यह;
        }
        अगर(elements_लंबाई>0) {
            uprv_memcpy(newElements, elements, (माप_प्रकार)elements_लंबाई*माप(UCharsTrieElement));
        }
        मिटाओ[] elements;
        elements=newElements;
        elementsCapacity=newCapacity;
    }
    elements[elements_लंबाई++].setTo(s, value, strings, errorCode);
    अगर(U_SUCCESS(errorCode) && strings.isBogus()) {
        errorCode=U_MEMORY_ALLOCATION_ERROR;
    }
    वापस *यह;
}

U_CDECL_BEGIN

अटल int32_t U_CALLCONV
compareElementStrings(स्थिर व्योम *context, स्थिर व्योम *बाँए, स्थिर व्योम *दाँए) {
    स्थिर UnicodeString *strings=static_cast<स्थिर UnicodeString *>(context);
    स्थिर UCharsTrieElement *बायाँ_Element=static_cast<स्थिर UCharsTrieElement *>(बाँए);
    स्थिर UCharsTrieElement *दायाँ_Element=static_cast<स्थिर UCharsTrieElement *>(दाँए);
    वापस बायाँ_Element->compareStringTo(*दायाँ_Element, *strings);
}

U_CDECL_END

UCharsTrie *
UCharsTrieBuilder::build(UStringTrieBuildOption buildOption, UErrorCode &errorCode) {
    buildUChars(buildOption, errorCode);
    UCharsTrie *newTrie=nullptr;
    अगर(U_SUCCESS(errorCode)) {
        newTrie=नया UCharsTrie(uchars, uchars+(ucharsCapacity-uchars_लंबाई));
        अगर(newTrie==nullptr) {
            errorCode=U_MEMORY_ALLOCATION_ERROR;
        } अन्यथा {
            uchars=nullptr;  // The new trie now owns the array.
            ucharsCapacity=0;
        }
    }
    वापस newTrie;
}

UnicodeString&
UCharsTrieBuilder::buildUnicodeString(UStringTrieBuildOption buildOption,
                                      UnicodeString& परिणाम U_LIFETIME_BOUND,
                                      UErrorCode& errorCode) U_LIFETIME_BOUND {
    buildUChars(buildOption, errorCode);
    अगर(U_SUCCESS(errorCode)) {
        परिणाम.setTo(false, uchars+(ucharsCapacity-uchars_लंबाई), uchars_लंबाई);
    }
    वापस परिणाम;
}

व्योम
UCharsTrieBuilder::buildUChars(UStringTrieBuildOption buildOption, UErrorCode &errorCode) {
    अगर(U_FAILURE(errorCode)) {
        वापस;
    }
    अगर(uchars!=nullptr && uchars_लंबाई>0) {
        // Already built.
        वापस;
    }
    अगर(uchars_लंबाई==0) {
        अगर(elements_लंबाई==0) {
            errorCode=U_सूचकांक_OUTOFBOUNDS_ERROR;
            वापस;
        }
        अगर(strings.isBogus()) {
            errorCode=U_MEMORY_ALLOCATION_ERROR;
            वापस;
        }
        uprv_sortArray(elements, elements_लंबाई, static_cast<int32_t>(माप(UCharsTrieElement)),
                      compareElementStrings, &strings,
                      false,  // need not be a stable sort
                      &errorCode);
        अगर(U_FAILURE(errorCode)) {
            वापस;
        }
        // Duplicate strings are not allowed.
        UnicodeString पूर्व=elements[0].getString(strings);
        क्रम(int32_t i=1; i<elements_लंबाई; ++i) {
            UnicodeString current=elements[i].getString(strings);
            अगर(पूर्व==current) {
                errorCode=U_ILLEGAL_ARGUMENT_ERROR;
                वापस;
            }
            पूर्व.fastCopyFrom(current);
        }
    }
    // Create and char16_t-serialize the trie for the elements.
    uchars_लंबाई=0;
    int32_t capacity=strings.length();
    अगर(capacity<1024) {
        capacity=1024;
    }
    अगर(ucharsCapacity<capacity) {
        uprv_free(uchars);
        uchars=static_cast<char16_t *>(uprv_malloc(capacity*2));
        अगर(uchars==nullptr) {
            errorCode=U_MEMORY_ALLOCATION_ERROR;
            ucharsCapacity=0;
            वापस;
        }
        ucharsCapacity=capacity;
    }
    StringTrieBuilder::build(buildOption, elements_लंबाई, errorCode);
    अगर(uchars==nullptr) {
        errorCode=U_MEMORY_ALLOCATION_ERROR;
    }
}

int32_t
UCharsTrieBuilder::get_Element_String_लंबाई(int32_t i) स्थिर {
    वापस elements[i].get_String_लंबाई(strings);
}

char16_t
UCharsTrieBuilder::getElementUnit(int32_t i, int32_t unit_सूचकांक) स्थिर {
    वापस elements[i].charAt(unit_सूचकांक, strings);
}

int32_t
UCharsTrieBuilder::get_Element_मूल्य(int32_t i) स्थिर {
    वापस elements[i].get_मूल्य();
}

int32_t
UCharsTrieBuilder::getLimitOfLinearMatch(int32_t first, int32_t last, int32_t unit_सूचकांक) स्थिर {
    स्थिर UCharsTrieElement &firstElement=elements[first];
    स्थिर UCharsTrieElement &lastElement=elements[last];
    int32_t min_String_लंबाई=firstElement.get_String_लंबाई(strings);
    जबतक(++unit_सूचकांक<min_String_लंबाई &&
            firstElement.charAt(unit_सूचकांक, strings)==
            lastElement.charAt(unit_सूचकांक, strings)) {}
    वापस unit_सूचकांक;
}

int32_t
UCharsTrieBuilder::गिनती_Element_Units(int32_t start, int32_t limit, int32_t unit_सूचकांक) स्थिर {
    int32_t length=0;  // Number of different units at unitIndex.
    int32_t i=start;
    करो {
        char16_t unit=elements[i++].charAt(unit_सूचकांक, strings);
        जबतक(i<limit && unit==elements[i].charAt(unit_सूचकांक, strings)) {
            ++i;
        }
        ++length;
    } जबतक(i<limit);
    वापस length;
}

int32_t
UCharsTrieBuilder::skipElementsBySomeUnits(int32_t i, int32_t unit_सूचकांक, int32_t count) स्थिर {
    करो {
        char16_t unit=elements[i++].charAt(unit_सूचकांक, strings);
        जबतक(unit==elements[i].charAt(unit_सूचकांक, strings)) {
            ++i;
        }
    } जबतक(--count>0);
    वापस i;
}

int32_t
UCharsTrieBuilder::सूचकांक_Of_Element_With_अग्र_Unit(int32_t i, int32_t unit_सूचकांक, char16_t unit) स्थिर {
    जबतक(unit==elements[i].charAt(unit_सूचकांक, strings)) {
        ++i;
    }
    वापस i;
}

UCharsTrieBuilder::UCTLinear_Match_गाँठ::UCTLinear_Match_गाँठ(स्थिर char16_t *units, int32_t len, Node *अग्र_गाँठ)
        : Linear_Match_गाँठ(len, अग्र_गाँठ), s(units) {
    hash=hash*37u+ustr_hashUCharsN(units, len);
}

bool
UCharsTrieBuilder::UCTLinear_Match_गाँठ::चालक==(स्थिर Node &other) स्थिर {
    अगर(यह==&other) {
        वापस true;
    }
    अगर(!Linear_Match_गाँठ::चालक==(other)) {
        वापस false;
    }
    स्थिर UCTLinear_Match_गाँठ &o=static_cast<स्थिर UCTLinear_Match_गाँठ &>(other);
    वापस 0==u_memcmp(s, o.s, length);
}

व्योम
UCharsTrieBuilder::UCTLinear_Match_गाँठ::ग_लिखो(StringTrieBuilder &builder) {
    UCharsTrieBuilder &b=(UCharsTrieBuilder &)builder;
    next->ग_लिखो(builder);
    b.ग_लिखो(s, length);
    offset=b.write_मूल्य_And_Type(has_मूल्य, value, b.getMinLinearMatch()+length-1);
}

StringTrieBuilder::Node *
UCharsTrieBuilder::create_Linear_Match_गाँठ(int32_t i, int32_t unit_सूचकांक, int32_t length,
                                         Node *अग्र_गाँठ) स्थिर {
    वापस नया UCTLinear_Match_गाँठ(
            elements[i].getString(strings).get_बफर()+unit_सूचकांक,
            length,
            अग्र_गाँठ);
}

UBool
UCharsTrieBuilder::ensureCapacity(int32_t length) {
    अगर(uchars==nullptr) {
        वापस false;  // previous memory allocation had failed
    }
    अगर(length>ucharsCapacity) {
        int32_t newCapacity=ucharsCapacity;
        करो {
            newCapacity*=2;
        } जबतक(newCapacity<=length);
        char16_t *newUChars=static_cast<char16_t *>(uprv_malloc(newCapacity*2));
        अगर(newUChars==nullptr) {
            // unable to allocate memory
            uprv_free(uchars);
            uchars=nullptr;
            ucharsCapacity=0;
            वापस false;
        }
        u_memcpy(newUChars+(newCapacity-uchars_लंबाई),
                 uchars+(ucharsCapacity-uchars_लंबाई), uchars_लंबाई);
        uprv_free(uchars);
        uchars=newUChars;
        ucharsCapacity=newCapacity;
    }
    वापस true;
}

int32_t
UCharsTrieBuilder::ग_लिखो(int32_t unit) {
    int32_t new_लंबाई=uchars_लंबाई+1;
    अगर(ensureCapacity(new_लंबाई)) {
        uchars_लंबाई=new_लंबाई;
        uchars[ucharsCapacity - uchars_लंबाई] = static_cast<char16_t>(unit);
    }
    वापस uchars_लंबाई;
}

int32_t
UCharsTrieBuilder::ग_लिखो(स्थिर char16_t *s, int32_t length) {
    int32_t new_लंबाई=uchars_लंबाई+length;
    अगर(ensureCapacity(new_लंबाई)) {
        uchars_लंबाई=new_लंबाई;
        u_memcpy(uchars+(ucharsCapacity-uchars_लंबाई), s, length);
    }
    वापस uchars_लंबाई;
}

int32_t
UCharsTrieBuilder::writeElementUnits(int32_t i, int32_t unit_सूचकांक, int32_t length) {
    वापस ग_लिखो(elements[i].getString(strings).get_बफर()+unit_सूचकांक, length);
}

int32_t
UCharsTrieBuilder::write_मूल्य_And_Final(int32_t i, UBool isFinal) {
    अगर(0<=i && i<=UCharsTrie::k_Max_One_Unit_मूल्य) {
        वापस ग_लिखो(i|(isFinal<<15));
    }
    char16_t intUnits[3];
    int32_t length;
    अगर(i<0 || i>UCharsTrie::k_Max_Two_Unit_मूल्य) {
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
    वापस ग_लिखो(intUnits, length);
}

int32_t
UCharsTrieBuilder::write_मूल्य_And_Type(UBool has_मूल्य, int32_t value, int32_t गाँठ) {
    अगर(!has_मूल्य) {
        वापस ग_लिखो(गाँठ);
    }
    char16_t intUnits[3];
    int32_t length;
    अगर(value<0 || value>UCharsTrie::k_Max_Two_Unit_गाँठ_मूल्य) {
        intUnits[0] = static_cast<char16_t>(UCharsTrie::k_Three_Unit_गाँठ_मूल्य_Lead);
        intUnits[1] = static_cast<char16_t>(static_cast<uint32_t>(value) >> 16);
        intUnits[2] = static_cast<char16_t>(value);
        length=3;
    } अन्यथा अगर(value<=UCharsTrie::k_Max_One_Unit_गाँठ_मूल्य) {
        intUnits[0] = static_cast<char16_t>((value + 1) << 6);
        length=1;
    } अन्यथा {
        intUnits[0] = static_cast<char16_t>(UCharsTrie::k_Min_Two_Unit_गाँठ_मूल्य_Lead + ((value >> 10) & 0x7fc0));
        intUnits[1] = static_cast<char16_t>(value);
        length=2;
    }
    intUnits[0] |= static_cast<char16_t>(गाँठ);
    वापस ग_लिखो(intUnits, length);
}

int32_t
UCharsTrieBuilder::writeDeltaTo(int32_t jumpTarget) {
    int32_t i=uchars_लंबाई-jumpTarget;
    U_ASSERT(i>=0);
    अगर(i<=UCharsTrie::kMaxOneUnitDelta) {
        वापस ग_लिखो(i);
    }
    char16_t intUnits[3];
    int32_t length;
    अगर(i<=UCharsTrie::kMaxTwoUnitDelta) {
        intUnits[0] = static_cast<char16_t>(UCharsTrie::kMinTwoUnitDeltaLead + (i >> 16));
        length=1;
    } अन्यथा {
        intUnits[0] = static_cast<char16_t>(UCharsTrie::kThreeUnitDeltaLead);
        intUnits[1] = static_cast<char16_t>(i >> 16);
        length=2;
    }
    intUnits[length++] = static_cast<char16_t>(i);
    वापस ग_लिखो(intUnits, length);
}

U_NAMESPACE_END
