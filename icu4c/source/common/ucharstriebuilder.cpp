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

#ଅନ୍ତର୍ଭାଵ "unicode/utypes.h"
#ଅନ୍ତର୍ଭାଵ "unicode/ucharstrie.h"
#ଅନ୍ତର୍ଭାଵ "unicode/ucharstriebuilder.h"
#ଅନ୍ତର୍ଭାଵ "unicode/unistr.h"
#ଅନ୍ତର୍ଭାଵ "unicode/ustring.h"
#ଅନ୍ତର୍ଭାଵ "cmemory.h"
#ଅନ୍ତର୍ଭାଵ "uarrsort.h"
#ଅନ୍ତର୍ଭାଵ "uassert.h"
#ଅନ୍ତର୍ଭାଵ "uhash.h"
#ଅନ୍ତର୍ଭାଵ "ustr_imp.h"

U_NAMESPACE_BEGIN

/*
 * Note: This builder implementation stores (string, value) pairs with full copies
 * of the 16-bit-unit sequences, until the UCharsTrie is built.
 * It might(!) take less memory if we collected the data in a temporary, dynamic trie.
 */

ଶ୍ରେଣୀ UCharsTrieElement : ସର୍ବସାଧାରଣ UMemory {
ସର୍ବସାଧାରଣ:
    // Use compiler's default constructor, initializes nothing.

    ଶୂନ୍ୟ setTo(ଅଚଳ UnicodeString &s, int32_t val, UnicodeString &strings, UErrorCode &errorCode);

    UnicodeString getString(ଅଚଳ UnicodeString& strings U_LIFETIME_BOUND) ଅଚଳ {
        int32_t length=strings[stringOffset];
        ଫେରନ୍ତୁ strings.tempSubString(stringOffset+1, length);
    }
    int32_t getStringLength(ଅଚଳ UnicodeString &strings) ଅଚଳ {
        ଫେରନ୍ତୁ strings[stringOffset];
    }

    char16_t charAt(int32_t index, ଅଚଳ UnicodeString &strings) ଅଚଳ {
        ଫେରନ୍ତୁ strings[stringOffset+1+index];
    }

    int32_t get_ମୂଲ୍ୟ() ଅଚଳ { ଫେରନ୍ତୁ value; }

    int32_t compareStringTo(ଅଚଳ UCharsTrieElement &o, ଅଚଳ UnicodeString &strings) ଅଚଳ;

ବ୍ୟକ୍ତିଗତ:
    // The first strings unit contains the string length.
    // (Compared with a stringLength field here, this saves 2 bytes per string.)
    int32_t stringOffset;
    int32_t value;
};

ଶୂନ୍ୟ
UCharsTrieElement::setTo(ଅଚଳ UnicodeString &s, int32_t val,
                         UnicodeString &strings, UErrorCode &errorCode) {
    ଯଦି(U_FAILURE(errorCode)) {
        ଫେରନ୍ତୁ;
    }
    int32_t length=s.length();
    ଯଦି(length>0xffff) {
        // Too long: We store the length in 1 unit.
        errorCode=U_INDEX_OUTOFBOUNDS_ERROR;
        ଫେରନ୍ତୁ;
    }
    stringOffset=strings.length();
    strings.append(static_cast<char16_t>(length));
    value=val;
    strings.append(s);
}

int32_t
UCharsTrieElement::compareStringTo(ଅଚଳ UCharsTrieElement &other, ଅଚଳ UnicodeString &strings) ଅଚଳ {
    ଫେରନ୍ତୁ getString(strings).compare(other.getString(strings));
}

UCharsTrieBuilder::UCharsTrieBuilder(UErrorCode & /*errorCode*/)
        : elements(nullptr), elementsCapacity(0), elementsLength(0),
          uchars(nullptr), ucharsCapacity(0), ucharsLength(0) {}

UCharsTrieBuilder::~UCharsTrieBuilder() {
    delete[] elements;
    uprv_free(uchars);
}

UCharsTrieBuilder&
UCharsTrieBuilder::add(ଅଚଳ UnicodeString& s,
                       int32_t value,
                       UErrorCode& errorCode) U_LIFETIME_BOUND {
    ଯଦି(U_FAILURE(errorCode)) {
        ଫେରନ୍ତୁ *ଏହି;
    }
    ଯଦି(ucharsLength>0) {
        // Cannot add elements after building.
        errorCode=U_NO_WRITE_PERMISSION;
        ଫେରନ୍ତୁ *ଏହି;
    }
    ଯଦି(elementsLength==elementsCapacity) {
        int32_t newCapacity;
        ଯଦି(elementsCapacity==0) {
            newCapacity=1024;
        } ଅନ୍ୟଥା {
            newCapacity=4*elementsCapacity;
        }
        UCharsTrieElement *newElements=ନୂତନ UCharsTrieElement[newCapacity];
        ଯଦି(newElements==nullptr) {
            errorCode=U_MEMORY_ALLOCATION_ERROR;
            ଫେରନ୍ତୁ *ଏହି;
        }
        ଯଦି(elementsLength>0) {
            uprv_memcpy(newElements, elements, (size_t)elementsLength*ଆକାର(UCharsTrieElement));
        }
        delete[] elements;
        elements=newElements;
        elementsCapacity=newCapacity;
    }
    elements[elementsLength++].setTo(s, value, strings, errorCode);
    ଯଦି(U_SUCCESS(errorCode) && strings.isBogus()) {
        errorCode=U_MEMORY_ALLOCATION_ERROR;
    }
    ଫେରନ୍ତୁ *ଏହି;
}

U_CDECL_BEGIN

ସ୍ଥିର int32_t U_CALLCONV
compareElementStrings(ଅଚଳ ଶୂନ୍ୟ *context, ଅଚଳ ଶୂନ୍ୟ *left, ଅଚଳ ଶୂନ୍ୟ *ଡାହାଣ) {
    ଅଚଳ UnicodeString *strings=static_cast<ଅଚଳ UnicodeString *>(context);
    ଅଚଳ UCharsTrieElement *ବାମ_Element=static_cast<ଅଚଳ UCharsTrieElement *>(left);
    ଅଚଳ UCharsTrieElement *ଡାହାଣ_Element=static_cast<ଅଚଳ UCharsTrieElement *>(ଡାହାଣ);
    ଫେରନ୍ତୁ ବାମ_Element->compareStringTo(*ଡାହାଣ_Element, *strings);
}

U_CDECL_END

UCharsTrie *
UCharsTrieBuilder::build(UStringTrieBuildOption buildOption, UErrorCode &errorCode) {
    buildUChars(buildOption, errorCode);
    UCharsTrie *newTrie=nullptr;
    ଯଦି(U_SUCCESS(errorCode)) {
        newTrie=ନୂତନ UCharsTrie(uchars, uchars+(ucharsCapacity-ucharsLength));
        ଯଦି(newTrie==nullptr) {
            errorCode=U_MEMORY_ALLOCATION_ERROR;
        } ଅନ୍ୟଥା {
            uchars=nullptr;  // The new trie now owns the array.
            ucharsCapacity=0;
        }
    }
    ଫେରନ୍ତୁ newTrie;
}

UnicodeString&
UCharsTrieBuilder::buildUnicodeString(UStringTrieBuildOption buildOption,
                                      UnicodeString& result U_LIFETIME_BOUND,
                                      UErrorCode& errorCode) U_LIFETIME_BOUND {
    buildUChars(buildOption, errorCode);
    ଯଦି(U_SUCCESS(errorCode)) {
        result.setTo(ମିଥ୍ୟା, uchars+(ucharsCapacity-ucharsLength), ucharsLength);
    }
    ଫେରନ୍ତୁ result;
}

ଶୂନ୍ୟ
UCharsTrieBuilder::buildUChars(UStringTrieBuildOption buildOption, UErrorCode &errorCode) {
    ଯଦି(U_FAILURE(errorCode)) {
        ଫେରନ୍ତୁ;
    }
    ଯଦି(uchars!=nullptr && ucharsLength>0) {
        // Already built.
        ଫେରନ୍ତୁ;
    }
    ଯଦି(ucharsLength==0) {
        ଯଦି(elementsLength==0) {
            errorCode=U_INDEX_OUTOFBOUNDS_ERROR;
            ଫେରନ୍ତୁ;
        }
        ଯଦି(strings.isBogus()) {
            errorCode=U_MEMORY_ALLOCATION_ERROR;
            ଫେରନ୍ତୁ;
        }
        uprv_sortArray(elements, elementsLength, static_cast<int32_t>(ଆକାର(UCharsTrieElement)),
                      compareElementStrings, &strings,
                      ମିଥ୍ୟା,  // need not be a stable sort
                      &errorCode);
        ଯଦି(U_FAILURE(errorCode)) {
            ଫେରନ୍ତୁ;
        }
        // Duplicate strings are not allowed.
        UnicodeString prev=elements[0].getString(strings);
        ପାଇଁ(int32_t i=1; i<elementsLength; ++i) {
            UnicodeString current=elements[i].getString(strings);
            ଯଦି(prev==current) {
                errorCode=U_ILLEGAL_ARGUMENT_ERROR;
                ଫେରନ୍ତୁ;
            }
            prev.fastCopyFrom(current);
        }
    }
    // Create and char16_t-serialize the trie for the elements.
    ucharsLength=0;
    int32_t capacity=strings.length();
    ଯଦି(capacity<1024) {
        capacity=1024;
    }
    ଯଦି(ucharsCapacity<capacity) {
        uprv_free(uchars);
        uchars=static_cast<char16_t *>(uprv_malloc(capacity*2));
        ଯଦି(uchars==nullptr) {
            errorCode=U_MEMORY_ALLOCATION_ERROR;
            ucharsCapacity=0;
            ଫେରନ୍ତୁ;
        }
        ucharsCapacity=capacity;
    }
    StringTrieBuilder::build(buildOption, elementsLength, errorCode);
    ଯଦି(uchars==nullptr) {
        errorCode=U_MEMORY_ALLOCATION_ERROR;
    }
}

int32_t
UCharsTrieBuilder::getElementStringLength(int32_t i) ଅଚଳ {
    ଫେରନ୍ତୁ elements[i].getStringLength(strings);
}

char16_t
UCharsTrieBuilder::getElementUnit(int32_t i, int32_t unitIndex) ଅଚଳ {
    ଫେରନ୍ତୁ elements[i].charAt(unitIndex, strings);
}

int32_t
UCharsTrieBuilder::get_Element_ମୂଲ୍ୟ(int32_t i) ଅଚଳ {
    ଫେରନ୍ତୁ elements[i].get_ମୂଲ୍ୟ();
}

int32_t
UCharsTrieBuilder::getLimitOfLinearMatch(int32_t first, int32_t last, int32_t unitIndex) ଅଚଳ {
    ଅଚଳ UCharsTrieElement &firstElement=elements[first];
    ଅଚଳ UCharsTrieElement &lastElement=elements[last];
    int32_t minStringLength=firstElement.getStringLength(strings);
    ଯେତେବେଳେ(++unitIndex<minStringLength &&
            firstElement.charAt(unitIndex, strings)==
            lastElement.charAt(unitIndex, strings)) {}
    ଫେରନ୍ତୁ unitIndex;
}

int32_t
UCharsTrieBuilder::ଗଣନା_Element_Units(int32_t start, int32_t limit, int32_t unitIndex) ଅଚଳ {
    int32_t length=0;  // Number of different units at unitIndex.
    int32_t i=start;
    କର {
        char16_t unit=elements[i++].charAt(unitIndex, strings);
        ଯେତେବେଳେ(i<limit && unit==elements[i].charAt(unitIndex, strings)) {
            ++i;
        }
        ++length;
    } ଯେତେବେଳେ(i<limit);
    ଫେରନ୍ତୁ length;
}

int32_t
UCharsTrieBuilder::skipElementsBySomeUnits(int32_t i, int32_t unitIndex, int32_t count) ଅଚଳ {
    କର {
        char16_t unit=elements[i++].charAt(unitIndex, strings);
        ଯେତେବେଳେ(unit==elements[i].charAt(unitIndex, strings)) {
            ++i;
        }
    } ଯେତେବେଳେ(--count>0);
    ଫେରନ୍ତୁ i;
}

int32_t
UCharsTrieBuilder::indexOfElementWithNextUnit(int32_t i, int32_t unitIndex, char16_t unit) ଅଚଳ {
    ଯେତେବେଳେ(unit==elements[i].charAt(unitIndex, strings)) {
        ++i;
    }
    ଫେରନ୍ତୁ i;
}

UCharsTrieBuilder::UCTLinear_Match_ଗଣ୍ଠି::UCTLinear_Match_ଗଣ୍ଠି(ଅଚଳ char16_t *units, int32_t len, Node *next_ଗଣ୍ଠି)
        : Linear_Match_ଗଣ୍ଠି(len, next_ଗଣ୍ଠି), s(units) {
    hash=hash*37u+ustr_hashUCharsN(units, len);
}

ତାର୍କିକ
UCharsTrieBuilder::UCTLinear_Match_ଗଣ୍ଠି::operator==(ଅଚଳ Node &other) ଅଚଳ {
    ଯଦି(ଏହି==&other) {
        ଫେରନ୍ତୁ ସତ୍ୟ;
    }
    ଯଦି(!Linear_Match_ଗଣ୍ଠି::operator==(other)) {
        ଫେରନ୍ତୁ ମିଥ୍ୟା;
    }
    ଅଚଳ UCTLinear_Match_ଗଣ୍ଠି &o=static_cast<ଅଚଳ UCTLinear_Match_ଗଣ୍ଠି &>(other);
    ଫେରନ୍ତୁ 0==u_memcmp(s, o.s, length);
}

ଶୂନ୍ୟ
UCharsTrieBuilder::UCTLinear_Match_ଗଣ୍ଠି::ଲିଖ(StringTrieBuilder &builder) {
    UCharsTrieBuilder &b=(UCharsTrieBuilder &)builder;
    next->ଲିଖ(builder);
    b.ଲିଖ(s, length);
    offset=b.write_ମୂଲ୍ୟ_And_Type(has_ମୂଲ୍ୟ, value, b.getMinLinearMatch()+length-1);
}

StringTrieBuilder::Node *
UCharsTrieBuilder::create_Linear_Match_ଗଣ୍ଠି(int32_t i, int32_t unitIndex, int32_t length,
                                         Node *next_ଗଣ୍ଠି) ଅଚଳ {
    ଫେରନ୍ତୁ ନୂତନ UCTLinear_Match_ଗଣ୍ଠି(
            elements[i].getString(strings).getBuffer()+unitIndex,
            length,
            next_ଗଣ୍ଠି);
}

UBool
UCharsTrieBuilder::ensureCapacity(int32_t length) {
    ଯଦି(uchars==nullptr) {
        ଫେରନ୍ତୁ ମିଥ୍ୟା;  // previous memory allocation had failed
    }
    ଯଦି(length>ucharsCapacity) {
        int32_t newCapacity=ucharsCapacity;
        କର {
            newCapacity*=2;
        } ଯେତେବେଳେ(newCapacity<=length);
        char16_t *newUChars=static_cast<char16_t *>(uprv_malloc(newCapacity*2));
        ଯଦି(newUChars==nullptr) {
            // unable to allocate memory
            uprv_free(uchars);
            uchars=nullptr;
            ucharsCapacity=0;
            ଫେରନ୍ତୁ ମିଥ୍ୟା;
        }
        u_memcpy(newUChars+(newCapacity-ucharsLength),
                 uchars+(ucharsCapacity-ucharsLength), ucharsLength);
        uprv_free(uchars);
        uchars=newUChars;
        ucharsCapacity=newCapacity;
    }
    ଫେରନ୍ତୁ ସତ୍ୟ;
}

int32_t
UCharsTrieBuilder::ଲିଖ(int32_t unit) {
    int32_t newLength=ucharsLength+1;
    ଯଦି(ensureCapacity(newLength)) {
        ucharsLength=newLength;
        uchars[ucharsCapacity - ucharsLength] = static_cast<char16_t>(unit);
    }
    ଫେରନ୍ତୁ ucharsLength;
}

int32_t
UCharsTrieBuilder::ଲିଖ(ଅଚଳ char16_t *s, int32_t length) {
    int32_t newLength=ucharsLength+length;
    ଯଦି(ensureCapacity(newLength)) {
        ucharsLength=newLength;
        u_memcpy(uchars+(ucharsCapacity-ucharsLength), s, length);
    }
    ଫେରନ୍ତୁ ucharsLength;
}

int32_t
UCharsTrieBuilder::writeElementUnits(int32_t i, int32_t unitIndex, int32_t length) {
    ଫେରନ୍ତୁ ଲିଖ(elements[i].getString(strings).getBuffer()+unitIndex, length);
}

int32_t
UCharsTrieBuilder::write_ମୂଲ୍ୟ_And_Final(int32_t i, UBool isFinal) {
    ଯଦି(0<=i && i<=UCharsTrie::k_Max_One_Unit_ମୂଲ୍ୟ) {
        ଫେରନ୍ତୁ ଲିଖ(i|(isFinal<<15));
    }
    char16_t intUnits[3];
    int32_t length;
    ଯଦି(i<0 || i>UCharsTrie::k_Max_Two_Unit_ମୂଲ୍ୟ) {
        intUnits[0] = static_cast<char16_t>(UCharsTrie::k_Three_Unit_ମୂଲ୍ୟ_Lead);
        intUnits[1] = static_cast<char16_t>(static_cast<uint32_t>(i) >> 16);
        intUnits[2] = static_cast<char16_t>(i);
        length=3;
    // } else if(i<=UCharsTrie::kMaxOneUnitValue) {
    //     intUnits[0]=(char16_t)(i);
    //     length=1;
    } ଅନ୍ୟଥା {
        intUnits[0] = static_cast<char16_t>(UCharsTrie::k_Min_Two_Unit_ମୂଲ୍ୟ_Lead + (i >> 16));
        intUnits[1] = static_cast<char16_t>(i);
        length=2;
    }
    intUnits[0] = static_cast<char16_t>(intUnits[0] | (isFinal << 15));
    ଫେରନ୍ତୁ ଲିଖ(intUnits, length);
}

int32_t
UCharsTrieBuilder::write_ମୂଲ୍ୟ_And_Type(UBool has_ମୂଲ୍ୟ, int32_t value, int32_t ଗଣ୍ଠି) {
    ଯଦି(!has_ମୂଲ୍ୟ) {
        ଫେରନ୍ତୁ ଲିଖ(ଗଣ୍ଠି);
    }
    char16_t intUnits[3];
    int32_t length;
    ଯଦି(value<0 || value>UCharsTrie::k_Max_Two_Unit_ଗଣ୍ଠି_ମୂଲ୍ୟ) {
        intUnits[0] = static_cast<char16_t>(UCharsTrie::k_Three_Unit_ଗଣ୍ଠି_ମୂଲ୍ୟ_Lead);
        intUnits[1] = static_cast<char16_t>(static_cast<uint32_t>(value) >> 16);
        intUnits[2] = static_cast<char16_t>(value);
        length=3;
    } ଅନ୍ୟଥା ଯଦି(value<=UCharsTrie::k_Max_One_Unit_ଗଣ୍ଠି_ମୂଲ୍ୟ) {
        intUnits[0] = static_cast<char16_t>((value + 1) << 6);
        length=1;
    } ଅନ୍ୟଥା {
        intUnits[0] = static_cast<char16_t>(UCharsTrie::k_Min_Two_Unit_ଗଣ୍ଠି_ମୂଲ୍ୟ_Lead + ((value >> 10) & 0x7fc0));
        intUnits[1] = static_cast<char16_t>(value);
        length=2;
    }
    intUnits[0] |= static_cast<char16_t>(ଗଣ୍ଠି);
    ଫେରନ୍ତୁ ଲିଖ(intUnits, length);
}

int32_t
UCharsTrieBuilder::writeDeltaTo(int32_t jumpTarget) {
    int32_t i=ucharsLength-jumpTarget;
    U_ASSERT(i>=0);
    ଯଦି(i<=UCharsTrie::kMaxOneUnitDelta) {
        ଫେରନ୍ତୁ ଲିଖ(i);
    }
    char16_t intUnits[3];
    int32_t length;
    ଯଦି(i<=UCharsTrie::kMaxTwoUnitDelta) {
        intUnits[0] = static_cast<char16_t>(UCharsTrie::kMinTwoUnitDeltaLead + (i >> 16));
        length=1;
    } ଅନ୍ୟଥା {
        intUnits[0] = static_cast<char16_t>(UCharsTrie::kThreeUnitDeltaLead);
        intUnits[1] = static_cast<char16_t>(i >> 16);
        length=2;
    }
    intUnits[length++] = static_cast<char16_t>(i);
    ଫେରନ୍ତୁ ଲିଖ(intUnits, length);
}

U_NAMESPACE_END
