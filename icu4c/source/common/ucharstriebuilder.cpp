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

#અન્તર્ભાવ "unicode/utypes.h"
#અન્તર્ભાવ "unicode/ucharstrie.h"
#અન્તર્ભાવ "unicode/ucharstriebuilder.h"
#અન્તર્ભાવ "unicode/unistr.h"
#અન્તર્ભાવ "unicode/ustring.h"
#અન્તર્ભાવ "cmemory.h"
#અન્તર્ભાવ "uarrsort.h"
#અન્તર્ભાવ "uassert.h"
#અન્તર્ભાવ "uhash.h"
#અન્તર્ભાવ "ustr_imp.h"

U_NAMESPACE_BEGIN

/*
 * Note: This builder implementation stores (string, value) pairs with full copies
 * of the 16-bit-unit sequences, until the UCharsTrie is built.
 * It might(!) take less memory if we collected the data in a temporary, dynamic trie.
 */

વર્ગ UCharsTrieElement : જાહેર UMemory {
જાહેર:
    // Use compiler's default constructor, initializes nothing.

    શૂન્ય setTo(અચલ UnicodeString &s, int32_t val, UnicodeString &strings, UErrorCode &errorCode);

    UnicodeString getString(અચલ UnicodeString& strings U_LIFETIME_BOUND) અચલ {
        int32_t length=strings[stringOffset];
        પાછા strings.tempSubString(stringOffset+1, length);
    }
    int32_t getStringLength(અચલ UnicodeString &strings) અચલ {
        પાછા strings[stringOffset];
    }

    char16_t charAt(int32_t index, અચલ UnicodeString &strings) અચલ {
        પાછા strings[stringOffset+1+index];
    }

    int32_t get_કિંમત() અચલ { પાછા value; }

    int32_t compareStringTo(અચલ UCharsTrieElement &o, અચલ UnicodeString &strings) અચલ;

ખાનગી:
    // The first strings unit contains the string length.
    // (Compared with a stringLength field here, this saves 2 bytes per string.)
    int32_t stringOffset;
    int32_t value;
};

શૂન્ય
UCharsTrieElement::setTo(અચલ UnicodeString &s, int32_t val,
                         UnicodeString &strings, UErrorCode &errorCode) {
    જો(U_FAILURE(errorCode)) {
        પાછા;
    }
    int32_t length=s.length();
    જો(length>0xffff) {
        // Too long: We store the length in 1 unit.
        errorCode=U_INDEX_OUTOFBOUNDS_ERROR;
        પાછા;
    }
    stringOffset=strings.length();
    strings.append(static_cast<char16_t>(length));
    value=val;
    strings.append(s);
}

int32_t
UCharsTrieElement::compareStringTo(અચલ UCharsTrieElement &other, અચલ UnicodeString &strings) અચલ {
    પાછા getString(strings).compare(other.getString(strings));
}

UCharsTrieBuilder::UCharsTrieBuilder(UErrorCode & /*errorCode*/)
        : elements(nullptr), elementsCapacity(0), elementsLength(0),
          uchars(nullptr), ucharsCapacity(0), ucharsLength(0) {}

UCharsTrieBuilder::~UCharsTrieBuilder() {
    delete[] elements;
    uprv_free(uchars);
}

UCharsTrieBuilder&
UCharsTrieBuilder::add(અચલ UnicodeString& s,
                       int32_t value,
                       UErrorCode& errorCode) U_LIFETIME_BOUND {
    જો(U_FAILURE(errorCode)) {
        પાછા *આ;
    }
    જો(ucharsLength>0) {
        // Cannot add elements after building.
        errorCode=U_NO_WRITE_PERMISSION;
        પાછા *આ;
    }
    જો(elementsLength==elementsCapacity) {
        int32_t newCapacity;
        જો(elementsCapacity==0) {
            newCapacity=1024;
        } નહીંતર {
            newCapacity=4*elementsCapacity;
        }
        UCharsTrieElement *newElements=નવું UCharsTrieElement[newCapacity];
        જો(newElements==nullptr) {
            errorCode=U_MEMORY_ALLOCATION_ERROR;
            પાછા *આ;
        }
        જો(elementsLength>0) {
            uprv_memcpy(newElements, elements, (size_t)elementsLength*કદ(UCharsTrieElement));
        }
        delete[] elements;
        elements=newElements;
        elementsCapacity=newCapacity;
    }
    elements[elementsLength++].setTo(s, value, strings, errorCode);
    જો(U_SUCCESS(errorCode) && strings.isBogus()) {
        errorCode=U_MEMORY_ALLOCATION_ERROR;
    }
    પાછા *આ;
}

U_CDECL_BEGIN

સ્થિર int32_t U_CALLCONV
compareElementStrings(અચલ શૂન્ય *context, અચલ શૂન્ય *left, અચલ શૂન્ય *જમણું) {
    અચલ UnicodeString *strings=static_cast<અચલ UnicodeString *>(context);
    અચલ UCharsTrieElement *ડાબું_Element=static_cast<અચલ UCharsTrieElement *>(left);
    અચલ UCharsTrieElement *જમણું_Element=static_cast<અચલ UCharsTrieElement *>(જમણું);
    પાછા ડાબું_Element->compareStringTo(*જમણું_Element, *strings);
}

U_CDECL_END

UCharsTrie *
UCharsTrieBuilder::build(UStringTrieBuildOption buildOption, UErrorCode &errorCode) {
    buildUChars(buildOption, errorCode);
    UCharsTrie *newTrie=nullptr;
    જો(U_SUCCESS(errorCode)) {
        newTrie=નવું UCharsTrie(uchars, uchars+(ucharsCapacity-ucharsLength));
        જો(newTrie==nullptr) {
            errorCode=U_MEMORY_ALLOCATION_ERROR;
        } નહીંતર {
            uchars=nullptr;  // The new trie now owns the array.
            ucharsCapacity=0;
        }
    }
    પાછા newTrie;
}

UnicodeString&
UCharsTrieBuilder::buildUnicodeString(UStringTrieBuildOption buildOption,
                                      UnicodeString& result U_LIFETIME_BOUND,
                                      UErrorCode& errorCode) U_LIFETIME_BOUND {
    buildUChars(buildOption, errorCode);
    જો(U_SUCCESS(errorCode)) {
        result.setTo(ખોટું, uchars+(ucharsCapacity-ucharsLength), ucharsLength);
    }
    પાછા result;
}

શૂન્ય
UCharsTrieBuilder::buildUChars(UStringTrieBuildOption buildOption, UErrorCode &errorCode) {
    જો(U_FAILURE(errorCode)) {
        પાછા;
    }
    જો(uchars!=nullptr && ucharsLength>0) {
        // Already built.
        પાછા;
    }
    જો(ucharsLength==0) {
        જો(elementsLength==0) {
            errorCode=U_INDEX_OUTOFBOUNDS_ERROR;
            પાછા;
        }
        જો(strings.isBogus()) {
            errorCode=U_MEMORY_ALLOCATION_ERROR;
            પાછા;
        }
        uprv_sortArray(elements, elementsLength, static_cast<int32_t>(કદ(UCharsTrieElement)),
                      compareElementStrings, &strings,
                      ખોટું,  // need not be a stable sort
                      &errorCode);
        જો(U_FAILURE(errorCode)) {
            પાછા;
        }
        // Duplicate strings are not allowed.
        UnicodeString prev=elements[0].getString(strings);
        માટે(int32_t i=1; i<elementsLength; ++i) {
            UnicodeString current=elements[i].getString(strings);
            જો(prev==current) {
                errorCode=U_ILLEGAL_ARGUMENT_ERROR;
                પાછા;
            }
            prev.fastCopyFrom(current);
        }
    }
    // Create and char16_t-serialize the trie for the elements.
    ucharsLength=0;
    int32_t capacity=strings.length();
    જો(capacity<1024) {
        capacity=1024;
    }
    જો(ucharsCapacity<capacity) {
        uprv_free(uchars);
        uchars=static_cast<char16_t *>(uprv_malloc(capacity*2));
        જો(uchars==nullptr) {
            errorCode=U_MEMORY_ALLOCATION_ERROR;
            ucharsCapacity=0;
            પાછા;
        }
        ucharsCapacity=capacity;
    }
    StringTrieBuilder::build(buildOption, elementsLength, errorCode);
    જો(uchars==nullptr) {
        errorCode=U_MEMORY_ALLOCATION_ERROR;
    }
}

int32_t
UCharsTrieBuilder::getElementStringLength(int32_t i) અચલ {
    પાછા elements[i].getStringLength(strings);
}

char16_t
UCharsTrieBuilder::getElementUnit(int32_t i, int32_t unitIndex) અચલ {
    પાછા elements[i].charAt(unitIndex, strings);
}

int32_t
UCharsTrieBuilder::get_Element_કિંમત(int32_t i) અચલ {
    પાછા elements[i].get_કિંમત();
}

int32_t
UCharsTrieBuilder::getLimitOfLinearMatch(int32_t first, int32_t last, int32_t unitIndex) અચલ {
    અચલ UCharsTrieElement &firstElement=elements[first];
    અચલ UCharsTrieElement &lastElement=elements[last];
    int32_t minStringLength=firstElement.getStringLength(strings);
    જ્યારે(++unitIndex<minStringLength &&
            firstElement.charAt(unitIndex, strings)==
            lastElement.charAt(unitIndex, strings)) {}
    પાછા unitIndex;
}

int32_t
UCharsTrieBuilder::ગણતરી_Element_Units(int32_t start, int32_t limit, int32_t unitIndex) અચલ {
    int32_t length=0;  // Number of different units at unitIndex.
    int32_t i=start;
    કરો {
        char16_t unit=elements[i++].charAt(unitIndex, strings);
        જ્યારે(i<limit && unit==elements[i].charAt(unitIndex, strings)) {
            ++i;
        }
        ++length;
    } જ્યારે(i<limit);
    પાછા length;
}

int32_t
UCharsTrieBuilder::skipElementsBySomeUnits(int32_t i, int32_t unitIndex, int32_t count) અચલ {
    કરો {
        char16_t unit=elements[i++].charAt(unitIndex, strings);
        જ્યારે(unit==elements[i].charAt(unitIndex, strings)) {
            ++i;
        }
    } જ્યારે(--count>0);
    પાછા i;
}

int32_t
UCharsTrieBuilder::indexOfElementWithNextUnit(int32_t i, int32_t unitIndex, char16_t unit) અચલ {
    જ્યારે(unit==elements[i].charAt(unitIndex, strings)) {
        ++i;
    }
    પાછા i;
}

UCharsTrieBuilder::UCTLinear_Match_ગાંઠ::UCTLinear_Match_ગાંઠ(અચલ char16_t *units, int32_t len, Node *next_ગાંઠ)
        : Linear_Match_ગાંઠ(len, next_ગાંઠ), s(units) {
    hash=hash*37u+ustr_hashUCharsN(units, len);
}

તાર્કિક
UCharsTrieBuilder::UCTLinear_Match_ગાંઠ::operator==(અચલ Node &other) અચલ {
    જો(આ==&other) {
        પાછા સાચું;
    }
    જો(!Linear_Match_ગાંઠ::operator==(other)) {
        પાછા ખોટું;
    }
    અચલ UCTLinear_Match_ગાંઠ &o=static_cast<અચલ UCTLinear_Match_ગાંઠ &>(other);
    પાછા 0==u_memcmp(s, o.s, length);
}

શૂન્ય
UCharsTrieBuilder::UCTLinear_Match_ગાંઠ::લિખ(StringTrieBuilder &builder) {
    UCharsTrieBuilder &b=(UCharsTrieBuilder &)builder;
    next->લિખ(builder);
    b.લિખ(s, length);
    offset=b.write_કિંમત_And_Type(has_કિંમત, value, b.getMinLinearMatch()+length-1);
}

StringTrieBuilder::Node *
UCharsTrieBuilder::create_Linear_Match_ગાંઠ(int32_t i, int32_t unitIndex, int32_t length,
                                         Node *next_ગાંઠ) અચલ {
    પાછા નવું UCTLinear_Match_ગાંઠ(
            elements[i].getString(strings).getBuffer()+unitIndex,
            length,
            next_ગાંઠ);
}

UBool
UCharsTrieBuilder::ensureCapacity(int32_t length) {
    જો(uchars==nullptr) {
        પાછા ખોટું;  // previous memory allocation had failed
    }
    જો(length>ucharsCapacity) {
        int32_t newCapacity=ucharsCapacity;
        કરો {
            newCapacity*=2;
        } જ્યારે(newCapacity<=length);
        char16_t *newUChars=static_cast<char16_t *>(uprv_malloc(newCapacity*2));
        જો(newUChars==nullptr) {
            // unable to allocate memory
            uprv_free(uchars);
            uchars=nullptr;
            ucharsCapacity=0;
            પાછા ખોટું;
        }
        u_memcpy(newUChars+(newCapacity-ucharsLength),
                 uchars+(ucharsCapacity-ucharsLength), ucharsLength);
        uprv_free(uchars);
        uchars=newUChars;
        ucharsCapacity=newCapacity;
    }
    પાછા સાચું;
}

int32_t
UCharsTrieBuilder::લિખ(int32_t unit) {
    int32_t newLength=ucharsLength+1;
    જો(ensureCapacity(newLength)) {
        ucharsLength=newLength;
        uchars[ucharsCapacity - ucharsLength] = static_cast<char16_t>(unit);
    }
    પાછા ucharsLength;
}

int32_t
UCharsTrieBuilder::લિખ(અચલ char16_t *s, int32_t length) {
    int32_t newLength=ucharsLength+length;
    જો(ensureCapacity(newLength)) {
        ucharsLength=newLength;
        u_memcpy(uchars+(ucharsCapacity-ucharsLength), s, length);
    }
    પાછા ucharsLength;
}

int32_t
UCharsTrieBuilder::writeElementUnits(int32_t i, int32_t unitIndex, int32_t length) {
    પાછા લિખ(elements[i].getString(strings).getBuffer()+unitIndex, length);
}

int32_t
UCharsTrieBuilder::write_કિંમત_And_Final(int32_t i, UBool isFinal) {
    જો(0<=i && i<=UCharsTrie::k_Max_One_Unit_કિંમત) {
        પાછા લિખ(i|(isFinal<<15));
    }
    char16_t intUnits[3];
    int32_t length;
    જો(i<0 || i>UCharsTrie::k_Max_Two_Unit_કિંમત) {
        intUnits[0] = static_cast<char16_t>(UCharsTrie::k_Three_Unit_કિંમત_Lead);
        intUnits[1] = static_cast<char16_t>(static_cast<uint32_t>(i) >> 16);
        intUnits[2] = static_cast<char16_t>(i);
        length=3;
    // } else if(i<=UCharsTrie::kMaxOneUnitValue) {
    //     intUnits[0]=(char16_t)(i);
    //     length=1;
    } નહીંતર {
        intUnits[0] = static_cast<char16_t>(UCharsTrie::k_Min_Two_Unit_કિંમત_Lead + (i >> 16));
        intUnits[1] = static_cast<char16_t>(i);
        length=2;
    }
    intUnits[0] = static_cast<char16_t>(intUnits[0] | (isFinal << 15));
    પાછા લિખ(intUnits, length);
}

int32_t
UCharsTrieBuilder::write_કિંમત_And_Type(UBool has_કિંમત, int32_t value, int32_t ગાંઠ) {
    જો(!has_કિંમત) {
        પાછા લિખ(ગાંઠ);
    }
    char16_t intUnits[3];
    int32_t length;
    જો(value<0 || value>UCharsTrie::k_Max_Two_Unit_ગાંઠ_કિંમત) {
        intUnits[0] = static_cast<char16_t>(UCharsTrie::k_Three_Unit_ગાંઠ_કિંમત_Lead);
        intUnits[1] = static_cast<char16_t>(static_cast<uint32_t>(value) >> 16);
        intUnits[2] = static_cast<char16_t>(value);
        length=3;
    } નહીંતર જો(value<=UCharsTrie::k_Max_One_Unit_ગાંઠ_કિંમત) {
        intUnits[0] = static_cast<char16_t>((value + 1) << 6);
        length=1;
    } નહીંતર {
        intUnits[0] = static_cast<char16_t>(UCharsTrie::k_Min_Two_Unit_ગાંઠ_કિંમત_Lead + ((value >> 10) & 0x7fc0));
        intUnits[1] = static_cast<char16_t>(value);
        length=2;
    }
    intUnits[0] |= static_cast<char16_t>(ગાંઠ);
    પાછા લિખ(intUnits, length);
}

int32_t
UCharsTrieBuilder::writeDeltaTo(int32_t jumpTarget) {
    int32_t i=ucharsLength-jumpTarget;
    U_ASSERT(i>=0);
    જો(i<=UCharsTrie::kMaxOneUnitDelta) {
        પાછા લિખ(i);
    }
    char16_t intUnits[3];
    int32_t length;
    જો(i<=UCharsTrie::kMaxTwoUnitDelta) {
        intUnits[0] = static_cast<char16_t>(UCharsTrie::kMinTwoUnitDeltaLead + (i >> 16));
        length=1;
    } નહીંતર {
        intUnits[0] = static_cast<char16_t>(UCharsTrie::kThreeUnitDeltaLead);
        intUnits[1] = static_cast<char16_t>(i >> 16);
        length=2;
    }
    intUnits[length++] = static_cast<char16_t>(i);
    પાછા લિખ(intUnits, length);
}

U_NAMESPACE_END
