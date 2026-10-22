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

#ضمن "unicode/utypes.h"
#ضمن "unicode/ucharstrie.h"
#ضمن "unicode/ucharstriebuilder.h"
#ضمن "unicode/unistr.h"
#ضمن "unicode/ustring.h"
#ضمن "cmemory.h"
#ضمن "uarrsort.h"
#ضمن "uassert.h"
#ضمن "uhash.h"
#ضمن "ustr_imp.h"

U_NAMESPACE_BEGIN

/*
 * Note: This builder implementation stores (string, value) pairs with full copies
 * of the 16-bit-unit sequences, until the UCharsTrie is built.
 * It might(!) take less memory if we collected the data in a temporary, dynamic trie.
 */

صنف UCharsTrieElement : عام UMemory {
عام:
    // Use compiler's default constructor, initializes nothing.

    فراغ setTo(ثابت UnicodeString &s, int32_t val, UnicodeString &strings, UErrorCode &errorCode);

    UnicodeString getString(ثابت UnicodeString& strings U_LIFETIME_BOUND) ثابت {
        int32_t length=strings[stringOffset];
        إرجاع strings.tempSubString(stringOffset+1, length);
    }
    int32_t getStringLength(ثابت UnicodeString &strings) ثابت {
        إرجاع strings[stringOffset];
    }

    char16_t charAt(int32_t index, ثابت UnicodeString &strings) ثابت {
        إرجاع strings[stringOffset+1+index];
    }

    int32_t get_قيمة() ثابت { إرجاع value; }

    int32_t compareStringTo(ثابت UCharsTrieElement &o, ثابت UnicodeString &strings) ثابت;

خاص:
    // The first strings unit contains the string length.
    // (Compared with a stringLength field here, this saves 2 bytes per string.)
    int32_t stringOffset;
    int32_t value;
};

فراغ
UCharsTrieElement::setTo(ثابت UnicodeString &s, int32_t val,
                         UnicodeString &strings, UErrorCode &errorCode) {
    إذا(U_FAILURE(errorCode)) {
        إرجاع;
    }
    int32_t length=s.length();
    إذا(length>0xffff) {
        // Too long: We store the length in 1 unit.
        errorCode=U_INDEX_OUTOFBOUNDS_ERROR;
        إرجاع;
    }
    stringOffset=strings.length();
    strings.append(static_cast<char16_t>(length));
    value=val;
    strings.append(s);
}

int32_t
UCharsTrieElement::compareStringTo(ثابت UCharsTrieElement &other, ثابت UnicodeString &strings) ثابت {
    إرجاع getString(strings).compare(other.getString(strings));
}

UCharsTrieBuilder::UCharsTrieBuilder(UErrorCode & /*errorCode*/)
        : elements(nullptr), elementsCapacity(0), elementsLength(0),
          uchars(nullptr), ucharsCapacity(0), ucharsLength(0) {}

UCharsTrieBuilder::~UCharsTrieBuilder() {
    delete[] elements;
    uprv_free(uchars);
}

UCharsTrieBuilder&
UCharsTrieBuilder::add(ثابت UnicodeString& s,
                       int32_t value,
                       UErrorCode& errorCode) U_LIFETIME_BOUND {
    إذا(U_FAILURE(errorCode)) {
        إرجاع *هذا;
    }
    إذا(ucharsLength>0) {
        // Cannot add elements after building.
        errorCode=U_NO_WRITE_PERMISSION;
        إرجاع *هذا;
    }
    إذا(elementsLength==elementsCapacity) {
        int32_t newCapacity;
        إذا(elementsCapacity==0) {
            newCapacity=1024;
        } وإلا {
            newCapacity=4*elementsCapacity;
        }
        UCharsTrieElement *newElements=جديد UCharsTrieElement[newCapacity];
        إذا(newElements==nullptr) {
            errorCode=U_MEMORY_ALLOCATION_ERROR;
            إرجاع *هذا;
        }
        إذا(elementsLength>0) {
            uprv_memcpy(newElements, elements, (size_t)elementsLength*حجم(UCharsTrieElement));
        }
        delete[] elements;
        elements=newElements;
        elementsCapacity=newCapacity;
    }
    elements[elementsLength++].setTo(s, value, strings, errorCode);
    إذا(U_SUCCESS(errorCode) && strings.isBogus()) {
        errorCode=U_MEMORY_ALLOCATION_ERROR;
    }
    إرجاع *هذا;
}

U_CDECL_BEGIN

ساكن int32_t U_CALLCONV
compareElementStrings(ثابت فراغ *context, ثابت فراغ *left, ثابت فراغ *يمين) {
    ثابت UnicodeString *strings=static_cast<ثابت UnicodeString *>(context);
    ثابت UCharsTrieElement *يسار_Element=static_cast<ثابت UCharsTrieElement *>(left);
    ثابت UCharsTrieElement *يمين_Element=static_cast<ثابت UCharsTrieElement *>(يمين);
    إرجاع يسار_Element->compareStringTo(*يمين_Element, *strings);
}

U_CDECL_END

UCharsTrie *
UCharsTrieBuilder::build(UStringTrieBuildOption buildOption, UErrorCode &errorCode) {
    buildUChars(buildOption, errorCode);
    UCharsTrie *newTrie=nullptr;
    إذا(U_SUCCESS(errorCode)) {
        newTrie=جديد UCharsTrie(uchars, uchars+(ucharsCapacity-ucharsLength));
        إذا(newTrie==nullptr) {
            errorCode=U_MEMORY_ALLOCATION_ERROR;
        } وإلا {
            uchars=nullptr;  // The new trie now owns the array.
            ucharsCapacity=0;
        }
    }
    إرجاع newTrie;
}

UnicodeString&
UCharsTrieBuilder::buildUnicodeString(UStringTrieBuildOption buildOption,
                                      UnicodeString& result U_LIFETIME_BOUND,
                                      UErrorCode& errorCode) U_LIFETIME_BOUND {
    buildUChars(buildOption, errorCode);
    إذا(U_SUCCESS(errorCode)) {
        result.setTo(خطأ, uchars+(ucharsCapacity-ucharsLength), ucharsLength);
    }
    إرجاع result;
}

فراغ
UCharsTrieBuilder::buildUChars(UStringTrieBuildOption buildOption, UErrorCode &errorCode) {
    إذا(U_FAILURE(errorCode)) {
        إرجاع;
    }
    إذا(uchars!=nullptr && ucharsLength>0) {
        // Already built.
        إرجاع;
    }
    إذا(ucharsLength==0) {
        إذا(elementsLength==0) {
            errorCode=U_INDEX_OUTOFBOUNDS_ERROR;
            إرجاع;
        }
        إذا(strings.isBogus()) {
            errorCode=U_MEMORY_ALLOCATION_ERROR;
            إرجاع;
        }
        uprv_sortArray(elements, elementsLength, static_cast<int32_t>(حجم(UCharsTrieElement)),
                      compareElementStrings, &strings,
                      خطأ,  // need not be a stable sort
                      &errorCode);
        إذا(U_FAILURE(errorCode)) {
            إرجاع;
        }
        // Duplicate strings are not allowed.
        UnicodeString prev=elements[0].getString(strings);
        لكل(int32_t i=1; i<elementsLength; ++i) {
            UnicodeString current=elements[i].getString(strings);
            إذا(prev==current) {
                errorCode=U_ILLEGAL_ARGUMENT_ERROR;
                إرجاع;
            }
            prev.fastCopyFrom(current);
        }
    }
    // Create and char16_t-serialize the trie for the elements.
    ucharsLength=0;
    int32_t capacity=strings.length();
    إذا(capacity<1024) {
        capacity=1024;
    }
    إذا(ucharsCapacity<capacity) {
        uprv_free(uchars);
        uchars=static_cast<char16_t *>(uprv_malloc(capacity*2));
        إذا(uchars==nullptr) {
            errorCode=U_MEMORY_ALLOCATION_ERROR;
            ucharsCapacity=0;
            إرجاع;
        }
        ucharsCapacity=capacity;
    }
    StringTrieBuilder::build(buildOption, elementsLength, errorCode);
    إذا(uchars==nullptr) {
        errorCode=U_MEMORY_ALLOCATION_ERROR;
    }
}

int32_t
UCharsTrieBuilder::getElementStringLength(int32_t i) ثابت {
    إرجاع elements[i].getStringLength(strings);
}

char16_t
UCharsTrieBuilder::getElementUnit(int32_t i, int32_t unitIndex) ثابت {
    إرجاع elements[i].charAt(unitIndex, strings);
}

int32_t
UCharsTrieBuilder::get_Element_قيمة(int32_t i) ثابت {
    إرجاع elements[i].get_قيمة();
}

int32_t
UCharsTrieBuilder::getLimitOfLinearMatch(int32_t first, int32_t last, int32_t unitIndex) ثابت {
    ثابت UCharsTrieElement &firstElement=elements[first];
    ثابت UCharsTrieElement &lastElement=elements[last];
    int32_t minStringLength=firstElement.getStringLength(strings);
    بينما(++unitIndex<minStringLength &&
            firstElement.charAt(unitIndex, strings)==
            lastElement.charAt(unitIndex, strings)) {}
    إرجاع unitIndex;
}

int32_t
UCharsTrieBuilder::تعداد_Element_Units(int32_t start, int32_t limit, int32_t unitIndex) ثابت {
    int32_t length=0;  // Number of different units at unitIndex.
    int32_t i=start;
    افعل {
        char16_t unit=elements[i++].charAt(unitIndex, strings);
        بينما(i<limit && unit==elements[i].charAt(unitIndex, strings)) {
            ++i;
        }
        ++length;
    } بينما(i<limit);
    إرجاع length;
}

int32_t
UCharsTrieBuilder::skipElementsBySomeUnits(int32_t i, int32_t unitIndex, int32_t count) ثابت {
    افعل {
        char16_t unit=elements[i++].charAt(unitIndex, strings);
        بينما(unit==elements[i].charAt(unitIndex, strings)) {
            ++i;
        }
    } بينما(--count>0);
    إرجاع i;
}

int32_t
UCharsTrieBuilder::indexOfElementWithNextUnit(int32_t i, int32_t unitIndex, char16_t unit) ثابت {
    بينما(unit==elements[i].charAt(unitIndex, strings)) {
        ++i;
    }
    إرجاع i;
}

UCharsTrieBuilder::UCTLinear_Match_عقدة::UCTLinear_Match_عقدة(ثابت char16_t *units, int32_t len, Node *next_عقدة)
        : Linear_Match_عقدة(len, next_عقدة), s(units) {
    hash=hash*37u+ustr_hashUCharsN(units, len);
}

منطقي
UCharsTrieBuilder::UCTLinear_Match_عقدة::operator==(ثابت Node &other) ثابت {
    إذا(هذا==&other) {
        إرجاع true;
    }
    إذا(!Linear_Match_عقدة::operator==(other)) {
        إرجاع خطأ;
    }
    ثابت UCTLinear_Match_عقدة &o=static_cast<ثابت UCTLinear_Match_عقدة &>(other);
    إرجاع 0==u_memcmp(s, o.s, length);
}

فراغ
UCharsTrieBuilder::UCTLinear_Match_عقدة::اكتب(StringTrieBuilder &builder) {
    UCharsTrieBuilder &b=(UCharsTrieBuilder &)builder;
    next->اكتب(builder);
    b.اكتب(s, length);
    offset=b.write_قيمة_And_Type(has_قيمة, value, b.getMinLinearMatch()+length-1);
}

StringTrieBuilder::Node *
UCharsTrieBuilder::create_Linear_Match_عقدة(int32_t i, int32_t unitIndex, int32_t length,
                                         Node *next_عقدة) ثابت {
    إرجاع جديد UCTLinear_Match_عقدة(
            elements[i].getString(strings).getBuffer()+unitIndex,
            length,
            next_عقدة);
}

UBool
UCharsTrieBuilder::ensureCapacity(int32_t length) {
    إذا(uchars==nullptr) {
        إرجاع خطأ;  // previous memory allocation had failed
    }
    إذا(length>ucharsCapacity) {
        int32_t newCapacity=ucharsCapacity;
        افعل {
            newCapacity*=2;
        } بينما(newCapacity<=length);
        char16_t *newUChars=static_cast<char16_t *>(uprv_malloc(newCapacity*2));
        إذا(newUChars==nullptr) {
            // unable to allocate memory
            uprv_free(uchars);
            uchars=nullptr;
            ucharsCapacity=0;
            إرجاع خطأ;
        }
        u_memcpy(newUChars+(newCapacity-ucharsLength),
                 uchars+(ucharsCapacity-ucharsLength), ucharsLength);
        uprv_free(uchars);
        uchars=newUChars;
        ucharsCapacity=newCapacity;
    }
    إرجاع true;
}

int32_t
UCharsTrieBuilder::اكتب(int32_t unit) {
    int32_t newLength=ucharsLength+1;
    إذا(ensureCapacity(newLength)) {
        ucharsLength=newLength;
        uchars[ucharsCapacity - ucharsLength] = static_cast<char16_t>(unit);
    }
    إرجاع ucharsLength;
}

int32_t
UCharsTrieBuilder::اكتب(ثابت char16_t *s, int32_t length) {
    int32_t newLength=ucharsLength+length;
    إذا(ensureCapacity(newLength)) {
        ucharsLength=newLength;
        u_memcpy(uchars+(ucharsCapacity-ucharsLength), s, length);
    }
    إرجاع ucharsLength;
}

int32_t
UCharsTrieBuilder::writeElementUnits(int32_t i, int32_t unitIndex, int32_t length) {
    إرجاع اكتب(elements[i].getString(strings).getBuffer()+unitIndex, length);
}

int32_t
UCharsTrieBuilder::write_قيمة_And_Final(int32_t i, UBool isFinal) {
    إذا(0<=i && i<=UCharsTrie::k_Max_One_Unit_قيمة) {
        إرجاع اكتب(i|(isFinal<<15));
    }
    char16_t intUnits[3];
    int32_t length;
    إذا(i<0 || i>UCharsTrie::k_Max_Two_Unit_قيمة) {
        intUnits[0] = static_cast<char16_t>(UCharsTrie::k_Three_Unit_قيمة_Lead);
        intUnits[1] = static_cast<char16_t>(static_cast<uint32_t>(i) >> 16);
        intUnits[2] = static_cast<char16_t>(i);
        length=3;
    // } else if(i<=UCharsTrie::kMaxOneUnitValue) {
    //     intUnits[0]=(char16_t)(i);
    //     length=1;
    } وإلا {
        intUnits[0] = static_cast<char16_t>(UCharsTrie::k_Min_Two_Unit_قيمة_Lead + (i >> 16));
        intUnits[1] = static_cast<char16_t>(i);
        length=2;
    }
    intUnits[0] = static_cast<char16_t>(intUnits[0] | (isFinal << 15));
    إرجاع اكتب(intUnits, length);
}

int32_t
UCharsTrieBuilder::write_قيمة_And_Type(UBool has_قيمة, int32_t value, int32_t عقدة) {
    إذا(!has_قيمة) {
        إرجاع اكتب(عقدة);
    }
    char16_t intUnits[3];
    int32_t length;
    إذا(value<0 || value>UCharsTrie::k_Max_Two_Unit_عقدة_قيمة) {
        intUnits[0] = static_cast<char16_t>(UCharsTrie::k_Three_Unit_عقدة_قيمة_Lead);
        intUnits[1] = static_cast<char16_t>(static_cast<uint32_t>(value) >> 16);
        intUnits[2] = static_cast<char16_t>(value);
        length=3;
    } وإلا إذا(value<=UCharsTrie::k_Max_One_Unit_عقدة_قيمة) {
        intUnits[0] = static_cast<char16_t>((value + 1) << 6);
        length=1;
    } وإلا {
        intUnits[0] = static_cast<char16_t>(UCharsTrie::k_Min_Two_Unit_عقدة_قيمة_Lead + ((value >> 10) & 0x7fc0));
        intUnits[1] = static_cast<char16_t>(value);
        length=2;
    }
    intUnits[0] |= static_cast<char16_t>(عقدة);
    إرجاع اكتب(intUnits, length);
}

int32_t
UCharsTrieBuilder::writeDeltaTo(int32_t jumpTarget) {
    int32_t i=ucharsLength-jumpTarget;
    U_ASSERT(i>=0);
    إذا(i<=UCharsTrie::kMaxOneUnitDelta) {
        إرجاع اكتب(i);
    }
    char16_t intUnits[3];
    int32_t length;
    إذا(i<=UCharsTrie::kMaxTwoUnitDelta) {
        intUnits[0] = static_cast<char16_t>(UCharsTrie::kMinTwoUnitDeltaLead + (i >> 16));
        length=1;
    } وإلا {
        intUnits[0] = static_cast<char16_t>(UCharsTrie::kThreeUnitDeltaLead);
        intUnits[1] = static_cast<char16_t>(i >> 16);
        length=2;
    }
    intUnits[length++] = static_cast<char16_t>(i);
    إرجاع اكتب(intUnits, length);
}

U_NAMESPACE_END
