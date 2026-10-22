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

#شامل "unicode/utypes.h"
#شامل "unicode/ucharstrie.h"
#شامل "unicode/ucharstriebuilder.h"
#شامل "unicode/unistr.h"
#شامل "unicode/ustring.h"
#شامل "cmemory.h"
#شامل "uarrsort.h"
#شامل "uassert.h"
#شامل "uhash.h"
#شامل "ustr_imp.h"

U_NAMESPACE_BEGIN

/*
 * Note: This builder implementation stores (string, value) pairs with full copies
 * of the 16-bit-unit sequences, until the UCharsTrie is built.
 * It might(!) take less memory if we collected the data in a temporary, dynamic trie.
 */

رده UCharsTrieElement : عمومی UMemory {
عمومی:
    // Use compiler's default constructor, initializes nothing.

    تهی setTo(ثابت UnicodeString &s, int32_t val, UnicodeString &strings, UErrorCode &errorCode);

    UnicodeString getString(ثابت UnicodeString& strings U_LIFETIME_BOUND) ثابت {
        int32_t length=strings[stringOffset];
        بازگشت strings.tempSubString(stringOffset+1, length);
    }
    int32_t getStringLength(ثابت UnicodeString &strings) ثابت {
        بازگشت strings[stringOffset];
    }

    char16_t charAt(int32_t index, ثابت UnicodeString &strings) ثابت {
        بازگشت strings[stringOffset+1+index];
    }

    int32_t get_مقدار() ثابت { بازگشت value; }

    int32_t compareStringTo(ثابت UCharsTrieElement &o, ثابت UnicodeString &strings) ثابت;

خصوصی:
    // The first strings unit contains the string length.
    // (Compared with a stringLength field here, this saves 2 bytes per string.)
    int32_t stringOffset;
    int32_t value;
};

تهی
UCharsTrieElement::setTo(ثابت UnicodeString &s, int32_t val,
                         UnicodeString &strings, UErrorCode &errorCode) {
    اگر(U_FAILURE(errorCode)) {
        بازگشت;
    }
    int32_t length=s.length();
    اگر(length>0xffff) {
        // Too long: We store the length in 1 unit.
        errorCode=U_INDEX_OUTOFBOUNDS_ERROR;
        بازگشت;
    }
    stringOffset=strings.length();
    strings.append(static_cast<char16_t>(length));
    value=val;
    strings.append(s);
}

int32_t
UCharsTrieElement::compareStringTo(ثابت UCharsTrieElement &other, ثابت UnicodeString &strings) ثابت {
    بازگشت getString(strings).compare(other.getString(strings));
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
    اگر(U_FAILURE(errorCode)) {
        بازگشت *این;
    }
    اگر(ucharsLength>0) {
        // Cannot add elements after building.
        errorCode=U_NO_WRITE_PERMISSION;
        بازگشت *این;
    }
    اگر(elementsLength==elementsCapacity) {
        int32_t newCapacity;
        اگر(elementsCapacity==0) {
            newCapacity=1024;
        } وگرنه {
            newCapacity=4*elementsCapacity;
        }
        UCharsTrieElement *newElements=جدید UCharsTrieElement[newCapacity];
        اگر(newElements==nullptr) {
            errorCode=U_MEMORY_ALLOCATION_ERROR;
            بازگشت *این;
        }
        اگر(elementsLength>0) {
            uprv_memcpy(newElements, elements, (size_t)elementsLength*اندازه(UCharsTrieElement));
        }
        delete[] elements;
        elements=newElements;
        elementsCapacity=newCapacity;
    }
    elements[elementsLength++].setTo(s, value, strings, errorCode);
    اگر(U_SUCCESS(errorCode) && strings.isBogus()) {
        errorCode=U_MEMORY_ALLOCATION_ERROR;
    }
    بازگشت *این;
}

U_CDECL_BEGIN

ایستا int32_t U_CALLCONV
compareElementStrings(ثابت تهی *context, ثابت تهی *left, ثابت تهی *راست) {
    ثابت UnicodeString *strings=static_cast<ثابت UnicodeString *>(context);
    ثابت UCharsTrieElement *چپ_Element=static_cast<ثابت UCharsTrieElement *>(left);
    ثابت UCharsTrieElement *راست_Element=static_cast<ثابت UCharsTrieElement *>(راست);
    بازگشت چپ_Element->compareStringTo(*راست_Element, *strings);
}

U_CDECL_END

UCharsTrie *
UCharsTrieBuilder::build(UStringTrieBuildOption buildOption, UErrorCode &errorCode) {
    buildUChars(buildOption, errorCode);
    UCharsTrie *newTrie=nullptr;
    اگر(U_SUCCESS(errorCode)) {
        newTrie=جدید UCharsTrie(uchars, uchars+(ucharsCapacity-ucharsLength));
        اگر(newTrie==nullptr) {
            errorCode=U_MEMORY_ALLOCATION_ERROR;
        } وگرنه {
            uchars=nullptr;  // The new trie now owns the array.
            ucharsCapacity=0;
        }
    }
    بازگشت newTrie;
}

UnicodeString&
UCharsTrieBuilder::buildUnicodeString(UStringTrieBuildOption buildOption,
                                      UnicodeString& result U_LIFETIME_BOUND,
                                      UErrorCode& errorCode) U_LIFETIME_BOUND {
    buildUChars(buildOption, errorCode);
    اگر(U_SUCCESS(errorCode)) {
        result.setTo(نادرست, uchars+(ucharsCapacity-ucharsLength), ucharsLength);
    }
    بازگشت result;
}

تهی
UCharsTrieBuilder::buildUChars(UStringTrieBuildOption buildOption, UErrorCode &errorCode) {
    اگر(U_FAILURE(errorCode)) {
        بازگشت;
    }
    اگر(uchars!=nullptr && ucharsLength>0) {
        // Already built.
        بازگشت;
    }
    اگر(ucharsLength==0) {
        اگر(elementsLength==0) {
            errorCode=U_INDEX_OUTOFBOUNDS_ERROR;
            بازگشت;
        }
        اگر(strings.isBogus()) {
            errorCode=U_MEMORY_ALLOCATION_ERROR;
            بازگشت;
        }
        uprv_sortArray(elements, elementsLength, static_cast<int32_t>(اندازه(UCharsTrieElement)),
                      compareElementStrings, &strings,
                      نادرست,  // need not be a stable sort
                      &errorCode);
        اگر(U_FAILURE(errorCode)) {
            بازگشت;
        }
        // Duplicate strings are not allowed.
        UnicodeString prev=elements[0].getString(strings);
        برای(int32_t i=1; i<elementsLength; ++i) {
            UnicodeString current=elements[i].getString(strings);
            اگر(prev==current) {
                errorCode=U_ILLEGAL_ARGUMENT_ERROR;
                بازگشت;
            }
            prev.fastCopyFrom(current);
        }
    }
    // Create and char16_t-serialize the trie for the elements.
    ucharsLength=0;
    int32_t capacity=strings.length();
    اگر(capacity<1024) {
        capacity=1024;
    }
    اگر(ucharsCapacity<capacity) {
        uprv_free(uchars);
        uchars=static_cast<char16_t *>(uprv_malloc(capacity*2));
        اگر(uchars==nullptr) {
            errorCode=U_MEMORY_ALLOCATION_ERROR;
            ucharsCapacity=0;
            بازگشت;
        }
        ucharsCapacity=capacity;
    }
    StringTrieBuilder::build(buildOption, elementsLength, errorCode);
    اگر(uchars==nullptr) {
        errorCode=U_MEMORY_ALLOCATION_ERROR;
    }
}

int32_t
UCharsTrieBuilder::getElementStringLength(int32_t i) ثابت {
    بازگشت elements[i].getStringLength(strings);
}

char16_t
UCharsTrieBuilder::getElementUnit(int32_t i, int32_t unitIndex) ثابت {
    بازگشت elements[i].charAt(unitIndex, strings);
}

int32_t
UCharsTrieBuilder::get_Element_مقدار(int32_t i) ثابت {
    بازگشت elements[i].get_مقدار();
}

int32_t
UCharsTrieBuilder::getLimitOfLinearMatch(int32_t first, int32_t last, int32_t unitIndex) ثابت {
    ثابت UCharsTrieElement &firstElement=elements[first];
    ثابت UCharsTrieElement &lastElement=elements[last];
    int32_t minStringLength=firstElement.getStringLength(strings);
    وقتی(++unitIndex<minStringLength &&
            firstElement.charAt(unitIndex, strings)==
            lastElement.charAt(unitIndex, strings)) {}
    بازگشت unitIndex;
}

int32_t
UCharsTrieBuilder::شمار_Element_Units(int32_t start, int32_t limit, int32_t unitIndex) ثابت {
    int32_t length=0;  // Number of different units at unitIndex.
    int32_t i=start;
    انجامبده {
        char16_t unit=elements[i++].charAt(unitIndex, strings);
        وقتی(i<limit && unit==elements[i].charAt(unitIndex, strings)) {
            ++i;
        }
        ++length;
    } وقتی(i<limit);
    بازگشت length;
}

int32_t
UCharsTrieBuilder::skipElementsBySomeUnits(int32_t i, int32_t unitIndex, int32_t count) ثابت {
    انجامبده {
        char16_t unit=elements[i++].charAt(unitIndex, strings);
        وقتی(unit==elements[i].charAt(unitIndex, strings)) {
            ++i;
        }
    } وقتی(--count>0);
    بازگشت i;
}

int32_t
UCharsTrieBuilder::indexOfElementWithNextUnit(int32_t i, int32_t unitIndex, char16_t unit) ثابت {
    وقتی(unit==elements[i].charAt(unitIndex, strings)) {
        ++i;
    }
    بازگشت i;
}

UCharsTrieBuilder::UCTLinear_Match_گره::UCTLinear_Match_گره(ثابت char16_t *units, int32_t len, Node *next_گره)
        : Linear_Match_گره(len, next_گره), s(units) {
    hash=hash*37u+ustr_hashUCharsN(units, len);
}

منطقی
UCharsTrieBuilder::UCTLinear_Match_گره::operator==(ثابت Node &other) ثابت {
    اگر(این==&other) {
        بازگشت درست;
    }
    اگر(!Linear_Match_گره::operator==(other)) {
        بازگشت نادرست;
    }
    ثابت UCTLinear_Match_گره &o=static_cast<ثابت UCTLinear_Match_گره &>(other);
    بازگشت 0==u_memcmp(s, o.s, length);
}

تهی
UCharsTrieBuilder::UCTLinear_Match_گره::اكتب(StringTrieBuilder &builder) {
    UCharsTrieBuilder &b=(UCharsTrieBuilder &)builder;
    next->اكتب(builder);
    b.اكتب(s, length);
    offset=b.write_مقدار_And_Type(has_مقدار, value, b.getMinLinearMatch()+length-1);
}

StringTrieBuilder::Node *
UCharsTrieBuilder::create_Linear_Match_گره(int32_t i, int32_t unitIndex, int32_t length,
                                         Node *next_گره) ثابت {
    بازگشت جدید UCTLinear_Match_گره(
            elements[i].getString(strings).getBuffer()+unitIndex,
            length,
            next_گره);
}

UBool
UCharsTrieBuilder::ensureCapacity(int32_t length) {
    اگر(uchars==nullptr) {
        بازگشت نادرست;  // previous memory allocation had failed
    }
    اگر(length>ucharsCapacity) {
        int32_t newCapacity=ucharsCapacity;
        انجامبده {
            newCapacity*=2;
        } وقتی(newCapacity<=length);
        char16_t *newUChars=static_cast<char16_t *>(uprv_malloc(newCapacity*2));
        اگر(newUChars==nullptr) {
            // unable to allocate memory
            uprv_free(uchars);
            uchars=nullptr;
            ucharsCapacity=0;
            بازگشت نادرست;
        }
        u_memcpy(newUChars+(newCapacity-ucharsLength),
                 uchars+(ucharsCapacity-ucharsLength), ucharsLength);
        uprv_free(uchars);
        uchars=newUChars;
        ucharsCapacity=newCapacity;
    }
    بازگشت درست;
}

int32_t
UCharsTrieBuilder::اكتب(int32_t unit) {
    int32_t newLength=ucharsLength+1;
    اگر(ensureCapacity(newLength)) {
        ucharsLength=newLength;
        uchars[ucharsCapacity - ucharsLength] = static_cast<char16_t>(unit);
    }
    بازگشت ucharsLength;
}

int32_t
UCharsTrieBuilder::اكتب(ثابت char16_t *s, int32_t length) {
    int32_t newLength=ucharsLength+length;
    اگر(ensureCapacity(newLength)) {
        ucharsLength=newLength;
        u_memcpy(uchars+(ucharsCapacity-ucharsLength), s, length);
    }
    بازگشت ucharsLength;
}

int32_t
UCharsTrieBuilder::writeElementUnits(int32_t i, int32_t unitIndex, int32_t length) {
    بازگشت اكتب(elements[i].getString(strings).getBuffer()+unitIndex, length);
}

int32_t
UCharsTrieBuilder::write_مقدار_And_Final(int32_t i, UBool isFinal) {
    اگر(0<=i && i<=UCharsTrie::k_Max_One_Unit_مقدار) {
        بازگشت اكتب(i|(isFinal<<15));
    }
    char16_t intUnits[3];
    int32_t length;
    اگر(i<0 || i>UCharsTrie::k_Max_Two_Unit_مقدار) {
        intUnits[0] = static_cast<char16_t>(UCharsTrie::k_Three_Unit_مقدار_Lead);
        intUnits[1] = static_cast<char16_t>(static_cast<uint32_t>(i) >> 16);
        intUnits[2] = static_cast<char16_t>(i);
        length=3;
    // } else if(i<=UCharsTrie::kMaxOneUnitValue) {
    //     intUnits[0]=(char16_t)(i);
    //     length=1;
    } وگرنه {
        intUnits[0] = static_cast<char16_t>(UCharsTrie::k_Min_Two_Unit_مقدار_Lead + (i >> 16));
        intUnits[1] = static_cast<char16_t>(i);
        length=2;
    }
    intUnits[0] = static_cast<char16_t>(intUnits[0] | (isFinal << 15));
    بازگشت اكتب(intUnits, length);
}

int32_t
UCharsTrieBuilder::write_مقدار_And_Type(UBool has_مقدار, int32_t value, int32_t گره) {
    اگر(!has_مقدار) {
        بازگشت اكتب(گره);
    }
    char16_t intUnits[3];
    int32_t length;
    اگر(value<0 || value>UCharsTrie::k_Max_Two_Unit_گره_مقدار) {
        intUnits[0] = static_cast<char16_t>(UCharsTrie::k_Three_Unit_گره_مقدار_Lead);
        intUnits[1] = static_cast<char16_t>(static_cast<uint32_t>(value) >> 16);
        intUnits[2] = static_cast<char16_t>(value);
        length=3;
    } وگرنه اگر(value<=UCharsTrie::k_Max_One_Unit_گره_مقدار) {
        intUnits[0] = static_cast<char16_t>((value + 1) << 6);
        length=1;
    } وگرنه {
        intUnits[0] = static_cast<char16_t>(UCharsTrie::k_Min_Two_Unit_گره_مقدار_Lead + ((value >> 10) & 0x7fc0));
        intUnits[1] = static_cast<char16_t>(value);
        length=2;
    }
    intUnits[0] |= static_cast<char16_t>(گره);
    بازگشت اكتب(intUnits, length);
}

int32_t
UCharsTrieBuilder::writeDeltaTo(int32_t jumpTarget) {
    int32_t i=ucharsLength-jumpTarget;
    U_ASSERT(i>=0);
    اگر(i<=UCharsTrie::kMaxOneUnitDelta) {
        بازگشت اكتب(i);
    }
    char16_t intUnits[3];
    int32_t length;
    اگر(i<=UCharsTrie::kMaxTwoUnitDelta) {
        intUnits[0] = static_cast<char16_t>(UCharsTrie::kMinTwoUnitDeltaLead + (i >> 16));
        length=1;
    } وگرنه {
        intUnits[0] = static_cast<char16_t>(UCharsTrie::kThreeUnitDeltaLead);
        intUnits[1] = static_cast<char16_t>(i >> 16);
        length=2;
    }
    intUnits[length++] = static_cast<char16_t>(i);
    بازگشت اكتب(intUnits, length);
}

U_NAMESPACE_END
