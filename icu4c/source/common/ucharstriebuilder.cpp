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

#അന്തര്ഭാവ "unicode/utypes.h"
#അന്തര്ഭാവ "unicode/ucharstrie.h"
#അന്തര്ഭാവ "unicode/ucharstriebuilder.h"
#അന്തര്ഭാവ "unicode/unistr.h"
#അന്തര്ഭാവ "unicode/ustring.h"
#അന്തര്ഭാവ "cmemory.h"
#അന്തര്ഭാവ "uarrsort.h"
#അന്തര്ഭാവ "uassert.h"
#അന്തര്ഭാവ "uhash.h"
#അന്തര്ഭാവ "ustr_imp.h"

U_NAMESPACE_BEGIN

/*
 * Note: This builder implementation stores (string, value) pairs with full copies
 * of the 16-bit-unit sequences, until the UCharsTrie is built.
 * It might(!) take less memory if we collected the data in a temporary, dynamic trie.
 */

വർഗ്ഗം UCharsTrieElement : പൊതു UMemory {
പൊതു:
    // Use compiler's default constructor, initializes nothing.

    ശൂന്യം setTo(സ്ഥിരം UnicodeString &s, int32_t val, UnicodeString &strings, UErrorCode &errorCode);

    UnicodeString getString(സ്ഥിരം UnicodeString& strings U_LIFETIME_BOUND) സ്ഥിരം {
        int32_t length=strings[stringOffset];
        തിരികെ strings.tempSubString(stringOffset+1, length);
    }
    int32_t getStringLength(സ്ഥിരം UnicodeString &strings) സ്ഥിരം {
        തിരികെ strings[stringOffset];
    }

    char16_t charAt(int32_t index, സ്ഥിരം UnicodeString &strings) സ്ഥിരം {
        തിരികെ strings[stringOffset+1+index];
    }

    int32_t get_മൂല്യം() സ്ഥിരം { തിരികെ value; }

    int32_t compareStringTo(സ്ഥിരം UCharsTrieElement &o, സ്ഥിരം UnicodeString &strings) സ്ഥിരം;

സ്വകാര്യ:
    // The first strings unit contains the string length.
    // (Compared with a stringLength field here, this saves 2 bytes per string.)
    int32_t stringOffset;
    int32_t value;
};

ശൂന്യം
UCharsTrieElement::setTo(സ്ഥിരം UnicodeString &s, int32_t val,
                         UnicodeString &strings, UErrorCode &errorCode) {
    എങ്കിൽ(U_FAILURE(errorCode)) {
        തിരികെ;
    }
    int32_t length=s.length();
    എങ്കിൽ(length>0xffff) {
        // Too long: We store the length in 1 unit.
        errorCode=U_INDEX_OUTOFBOUNDS_ERROR;
        തിരികെ;
    }
    stringOffset=strings.length();
    strings.append(static_cast<char16_t>(length));
    value=val;
    strings.append(s);
}

int32_t
UCharsTrieElement::compareStringTo(സ്ഥിരം UCharsTrieElement &other, സ്ഥിരം UnicodeString &strings) സ്ഥിരം {
    തിരികെ getString(strings).compare(other.getString(strings));
}

UCharsTrieBuilder::UCharsTrieBuilder(UErrorCode & /*errorCode*/)
        : elements(nullptr), elementsCapacity(0), elementsLength(0),
          uchars(nullptr), ucharsCapacity(0), ucharsLength(0) {}

UCharsTrieBuilder::~UCharsTrieBuilder() {
    delete[] elements;
    uprv_free(uchars);
}

UCharsTrieBuilder&
UCharsTrieBuilder::add(സ്ഥിരം UnicodeString& s,
                       int32_t value,
                       UErrorCode& errorCode) U_LIFETIME_BOUND {
    എങ്കിൽ(U_FAILURE(errorCode)) {
        തിരികെ *ഇത്;
    }
    എങ്കിൽ(ucharsLength>0) {
        // Cannot add elements after building.
        errorCode=U_NO_WRITE_PERMISSION;
        തിരികെ *ഇത്;
    }
    എങ്കിൽ(elementsLength==elementsCapacity) {
        int32_t newCapacity;
        എങ്കിൽ(elementsCapacity==0) {
            newCapacity=1024;
        } അല്ലെങ്കിൽ {
            newCapacity=4*elementsCapacity;
        }
        UCharsTrieElement *newElements=പുതിയ UCharsTrieElement[newCapacity];
        എങ്കിൽ(newElements==nullptr) {
            errorCode=U_MEMORY_ALLOCATION_ERROR;
            തിരികെ *ഇത്;
        }
        എങ്കിൽ(elementsLength>0) {
            uprv_memcpy(newElements, elements, (size_t)elementsLength*വലുപ്പം(UCharsTrieElement));
        }
        delete[] elements;
        elements=newElements;
        elementsCapacity=newCapacity;
    }
    elements[elementsLength++].setTo(s, value, strings, errorCode);
    എങ്കിൽ(U_SUCCESS(errorCode) && strings.isBogus()) {
        errorCode=U_MEMORY_ALLOCATION_ERROR;
    }
    തിരികെ *ഇത്;
}

U_CDECL_BEGIN

സ്ഥിതം int32_t U_CALLCONV
compareElementStrings(സ്ഥിരം ശൂന്യം *context, സ്ഥിരം ശൂന്യം *left, സ്ഥിരം ശൂന്യം *വലത്) {
    സ്ഥിരം UnicodeString *strings=static_cast<സ്ഥിരം UnicodeString *>(context);
    സ്ഥിരം UCharsTrieElement *ഇടത്_Element=static_cast<സ്ഥിരം UCharsTrieElement *>(left);
    സ്ഥിരം UCharsTrieElement *വലത്_Element=static_cast<സ്ഥിരം UCharsTrieElement *>(വലത്);
    തിരികെ ഇടത്_Element->compareStringTo(*വലത്_Element, *strings);
}

U_CDECL_END

UCharsTrie *
UCharsTrieBuilder::build(UStringTrieBuildOption buildOption, UErrorCode &errorCode) {
    buildUChars(buildOption, errorCode);
    UCharsTrie *newTrie=nullptr;
    എങ്കിൽ(U_SUCCESS(errorCode)) {
        newTrie=പുതിയ UCharsTrie(uchars, uchars+(ucharsCapacity-ucharsLength));
        എങ്കിൽ(newTrie==nullptr) {
            errorCode=U_MEMORY_ALLOCATION_ERROR;
        } അല്ലെങ്കിൽ {
            uchars=nullptr;  // The new trie now owns the array.
            ucharsCapacity=0;
        }
    }
    തിരികെ newTrie;
}

UnicodeString&
UCharsTrieBuilder::buildUnicodeString(UStringTrieBuildOption buildOption,
                                      UnicodeString& result U_LIFETIME_BOUND,
                                      UErrorCode& errorCode) U_LIFETIME_BOUND {
    buildUChars(buildOption, errorCode);
    എങ്കിൽ(U_SUCCESS(errorCode)) {
        result.setTo(അസത്യം, uchars+(ucharsCapacity-ucharsLength), ucharsLength);
    }
    തിരികെ result;
}

ശൂന്യം
UCharsTrieBuilder::buildUChars(UStringTrieBuildOption buildOption, UErrorCode &errorCode) {
    എങ്കിൽ(U_FAILURE(errorCode)) {
        തിരികെ;
    }
    എങ്കിൽ(uchars!=nullptr && ucharsLength>0) {
        // Already built.
        തിരികെ;
    }
    എങ്കിൽ(ucharsLength==0) {
        എങ്കിൽ(elementsLength==0) {
            errorCode=U_INDEX_OUTOFBOUNDS_ERROR;
            തിരികെ;
        }
        എങ്കിൽ(strings.isBogus()) {
            errorCode=U_MEMORY_ALLOCATION_ERROR;
            തിരികെ;
        }
        uprv_sortArray(elements, elementsLength, static_cast<int32_t>(വലുപ്പം(UCharsTrieElement)),
                      compareElementStrings, &strings,
                      അസത്യം,  // need not be a stable sort
                      &errorCode);
        എങ്കിൽ(U_FAILURE(errorCode)) {
            തിരികെ;
        }
        // Duplicate strings are not allowed.
        UnicodeString prev=elements[0].getString(strings);
        വേണ്ടി(int32_t i=1; i<elementsLength; ++i) {
            UnicodeString current=elements[i].getString(strings);
            എങ്കിൽ(prev==current) {
                errorCode=U_ILLEGAL_ARGUMENT_ERROR;
                തിരികെ;
            }
            prev.fastCopyFrom(current);
        }
    }
    // Create and char16_t-serialize the trie for the elements.
    ucharsLength=0;
    int32_t capacity=strings.length();
    എങ്കിൽ(capacity<1024) {
        capacity=1024;
    }
    എങ്കിൽ(ucharsCapacity<capacity) {
        uprv_free(uchars);
        uchars=static_cast<char16_t *>(uprv_malloc(capacity*2));
        എങ്കിൽ(uchars==nullptr) {
            errorCode=U_MEMORY_ALLOCATION_ERROR;
            ucharsCapacity=0;
            തിരികെ;
        }
        ucharsCapacity=capacity;
    }
    StringTrieBuilder::build(buildOption, elementsLength, errorCode);
    എങ്കിൽ(uchars==nullptr) {
        errorCode=U_MEMORY_ALLOCATION_ERROR;
    }
}

int32_t
UCharsTrieBuilder::getElementStringLength(int32_t i) സ്ഥിരം {
    തിരികെ elements[i].getStringLength(strings);
}

char16_t
UCharsTrieBuilder::getElementUnit(int32_t i, int32_t unitIndex) സ്ഥിരം {
    തിരികെ elements[i].charAt(unitIndex, strings);
}

int32_t
UCharsTrieBuilder::get_Element_മൂല്യം(int32_t i) സ്ഥിരം {
    തിരികെ elements[i].get_മൂല്യം();
}

int32_t
UCharsTrieBuilder::getLimitOfLinearMatch(int32_t first, int32_t last, int32_t unitIndex) സ്ഥിരം {
    സ്ഥിരം UCharsTrieElement &firstElement=elements[first];
    സ്ഥിരം UCharsTrieElement &lastElement=elements[last];
    int32_t minStringLength=firstElement.getStringLength(strings);
    ആകുമ്പോൾ(++unitIndex<minStringLength &&
            firstElement.charAt(unitIndex, strings)==
            lastElement.charAt(unitIndex, strings)) {}
    തിരികെ unitIndex;
}

int32_t
UCharsTrieBuilder::എണ്ണം_Element_Units(int32_t start, int32_t limit, int32_t unitIndex) സ്ഥിരം {
    int32_t length=0;  // Number of different units at unitIndex.
    int32_t i=start;
    ചെയ്യുക {
        char16_t unit=elements[i++].charAt(unitIndex, strings);
        ആകുമ്പോൾ(i<limit && unit==elements[i].charAt(unitIndex, strings)) {
            ++i;
        }
        ++length;
    } ആകുമ്പോൾ(i<limit);
    തിരികെ length;
}

int32_t
UCharsTrieBuilder::skipElementsBySomeUnits(int32_t i, int32_t unitIndex, int32_t count) സ്ഥിരം {
    ചെയ്യുക {
        char16_t unit=elements[i++].charAt(unitIndex, strings);
        ആകുമ്പോൾ(unit==elements[i].charAt(unitIndex, strings)) {
            ++i;
        }
    } ആകുമ്പോൾ(--count>0);
    തിരികെ i;
}

int32_t
UCharsTrieBuilder::indexOfElementWithNextUnit(int32_t i, int32_t unitIndex, char16_t unit) സ്ഥിരം {
    ആകുമ്പോൾ(unit==elements[i].charAt(unitIndex, strings)) {
        ++i;
    }
    തിരികെ i;
}

UCharsTrieBuilder::UCTLinear_Match_കണ്ണി::UCTLinear_Match_കണ്ണി(സ്ഥിരം char16_t *units, int32_t len, Node *next_കണ്ണി)
        : Linear_Match_കണ്ണി(len, next_കണ്ണി), s(units) {
    hash=hash*37u+ustr_hashUCharsN(units, len);
}

തർക്കം
UCharsTrieBuilder::UCTLinear_Match_കണ്ണി::operator==(സ്ഥിരം Node &other) സ്ഥിരം {
    എങ്കിൽ(ഇത്==&other) {
        തിരികെ സത്യം;
    }
    എങ്കിൽ(!Linear_Match_കണ്ണി::operator==(other)) {
        തിരികെ അസത്യം;
    }
    സ്ഥിരം UCTLinear_Match_കണ്ണി &o=static_cast<സ്ഥിരം UCTLinear_Match_കണ്ണി &>(other);
    തിരികെ 0==u_memcmp(s, o.s, length);
}

ശൂന്യം
UCharsTrieBuilder::UCTLinear_Match_കണ്ണി::ലിഖ(StringTrieBuilder &builder) {
    UCharsTrieBuilder &b=(UCharsTrieBuilder &)builder;
    next->ലിഖ(builder);
    b.ലിഖ(s, length);
    offset=b.write_മൂല്യം_And_Type(has_മൂല്യം, value, b.getMinLinearMatch()+length-1);
}

StringTrieBuilder::Node *
UCharsTrieBuilder::create_Linear_Match_കണ്ണി(int32_t i, int32_t unitIndex, int32_t length,
                                         Node *next_കണ്ണി) സ്ഥിരം {
    തിരികെ പുതിയ UCTLinear_Match_കണ്ണി(
            elements[i].getString(strings).getBuffer()+unitIndex,
            length,
            next_കണ്ണി);
}

UBool
UCharsTrieBuilder::ensureCapacity(int32_t length) {
    എങ്കിൽ(uchars==nullptr) {
        തിരികെ അസത്യം;  // previous memory allocation had failed
    }
    എങ്കിൽ(length>ucharsCapacity) {
        int32_t newCapacity=ucharsCapacity;
        ചെയ്യുക {
            newCapacity*=2;
        } ആകുമ്പോൾ(newCapacity<=length);
        char16_t *newUChars=static_cast<char16_t *>(uprv_malloc(newCapacity*2));
        എങ്കിൽ(newUChars==nullptr) {
            // unable to allocate memory
            uprv_free(uchars);
            uchars=nullptr;
            ucharsCapacity=0;
            തിരികെ അസത്യം;
        }
        u_memcpy(newUChars+(newCapacity-ucharsLength),
                 uchars+(ucharsCapacity-ucharsLength), ucharsLength);
        uprv_free(uchars);
        uchars=newUChars;
        ucharsCapacity=newCapacity;
    }
    തിരികെ സത്യം;
}

int32_t
UCharsTrieBuilder::ലിഖ(int32_t unit) {
    int32_t newLength=ucharsLength+1;
    എങ്കിൽ(ensureCapacity(newLength)) {
        ucharsLength=newLength;
        uchars[ucharsCapacity - ucharsLength] = static_cast<char16_t>(unit);
    }
    തിരികെ ucharsLength;
}

int32_t
UCharsTrieBuilder::ലിഖ(സ്ഥിരം char16_t *s, int32_t length) {
    int32_t newLength=ucharsLength+length;
    എങ്കിൽ(ensureCapacity(newLength)) {
        ucharsLength=newLength;
        u_memcpy(uchars+(ucharsCapacity-ucharsLength), s, length);
    }
    തിരികെ ucharsLength;
}

int32_t
UCharsTrieBuilder::writeElementUnits(int32_t i, int32_t unitIndex, int32_t length) {
    തിരികെ ലിഖ(elements[i].getString(strings).getBuffer()+unitIndex, length);
}

int32_t
UCharsTrieBuilder::write_മൂല്യം_And_Final(int32_t i, UBool isFinal) {
    എങ്കിൽ(0<=i && i<=UCharsTrie::k_Max_One_Unit_മൂല്യം) {
        തിരികെ ലിഖ(i|(isFinal<<15));
    }
    char16_t intUnits[3];
    int32_t length;
    എങ്കിൽ(i<0 || i>UCharsTrie::k_Max_Two_Unit_മൂല്യം) {
        intUnits[0] = static_cast<char16_t>(UCharsTrie::k_Three_Unit_മൂല്യം_Lead);
        intUnits[1] = static_cast<char16_t>(static_cast<uint32_t>(i) >> 16);
        intUnits[2] = static_cast<char16_t>(i);
        length=3;
    // } else if(i<=UCharsTrie::kMaxOneUnitValue) {
    //     intUnits[0]=(char16_t)(i);
    //     length=1;
    } അല്ലെങ്കിൽ {
        intUnits[0] = static_cast<char16_t>(UCharsTrie::k_Min_Two_Unit_മൂല്യം_Lead + (i >> 16));
        intUnits[1] = static_cast<char16_t>(i);
        length=2;
    }
    intUnits[0] = static_cast<char16_t>(intUnits[0] | (isFinal << 15));
    തിരികെ ലിഖ(intUnits, length);
}

int32_t
UCharsTrieBuilder::write_മൂല്യം_And_Type(UBool has_മൂല്യം, int32_t value, int32_t കണ്ണി) {
    എങ്കിൽ(!has_മൂല്യം) {
        തിരികെ ലിഖ(കണ്ണി);
    }
    char16_t intUnits[3];
    int32_t length;
    എങ്കിൽ(value<0 || value>UCharsTrie::k_Max_Two_Unit_കണ്ണി_മൂല്യം) {
        intUnits[0] = static_cast<char16_t>(UCharsTrie::k_Three_Unit_കണ്ണി_മൂല്യം_Lead);
        intUnits[1] = static_cast<char16_t>(static_cast<uint32_t>(value) >> 16);
        intUnits[2] = static_cast<char16_t>(value);
        length=3;
    } അല്ലെങ്കിൽ എങ്കിൽ(value<=UCharsTrie::k_Max_One_Unit_കണ്ണി_മൂല്യം) {
        intUnits[0] = static_cast<char16_t>((value + 1) << 6);
        length=1;
    } അല്ലെങ്കിൽ {
        intUnits[0] = static_cast<char16_t>(UCharsTrie::k_Min_Two_Unit_കണ്ണി_മൂല്യം_Lead + ((value >> 10) & 0x7fc0));
        intUnits[1] = static_cast<char16_t>(value);
        length=2;
    }
    intUnits[0] |= static_cast<char16_t>(കണ്ണി);
    തിരികെ ലിഖ(intUnits, length);
}

int32_t
UCharsTrieBuilder::writeDeltaTo(int32_t jumpTarget) {
    int32_t i=ucharsLength-jumpTarget;
    U_ASSERT(i>=0);
    എങ്കിൽ(i<=UCharsTrie::kMaxOneUnitDelta) {
        തിരികെ ലിഖ(i);
    }
    char16_t intUnits[3];
    int32_t length;
    എങ്കിൽ(i<=UCharsTrie::kMaxTwoUnitDelta) {
        intUnits[0] = static_cast<char16_t>(UCharsTrie::kMinTwoUnitDeltaLead + (i >> 16));
        length=1;
    } അല്ലെങ്കിൽ {
        intUnits[0] = static_cast<char16_t>(UCharsTrie::kThreeUnitDeltaLead);
        intUnits[1] = static_cast<char16_t>(i >> 16);
        length=2;
    }
    intUnits[length++] = static_cast<char16_t>(i);
    തിരികെ ലിഖ(intUnits, length);
}

U_NAMESPACE_END
