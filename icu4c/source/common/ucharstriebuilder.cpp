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

#অন্তর্ভাব "unicode/utypes.h"
#অন্তর্ভাব "unicode/ucharstrie.h"
#অন্তর্ভাব "unicode/ucharstriebuilder.h"
#অন্তর্ভাব "unicode/unistr.h"
#অন্তর্ভাব "unicode/ustring.h"
#অন্তর্ভাব "cmemory.h"
#অন্তর্ভাব "uarrsort.h"
#অন্তর্ভাব "uassert.h"
#অন্তর্ভাব "uhash.h"
#অন্তর্ভাব "ustr_imp.h"

U_NAMESPACE_BEGIN

/*
 * Note: This builder implementation stores (string, value) pairs with full copies
 * of the 16-bit-unit sequences, until the UCharsTrie is built.
 * It might(!) take less memory if we collected the data in a temporary, dynamic trie.
 */

শ্রেণী UCharsTrieElement : সর্বজনীন UMemory {
সর্বজনীন:
    // Use compiler's default constructor, initializes nothing.

    শূন্য setTo(ধ্রুবক UnicodeString &s, int32_t val, UnicodeString &strings, UErrorCode &errorCode);

    UnicodeString getString(ধ্রুবক UnicodeString& strings U_LIFETIME_BOUND) ধ্রুবক {
        int32_t length=strings[stringOffset];
        ফেরত strings.অস্থায়ী_Sub_String(stringOffset+1, length);
    }
    int32_t get_String_দৈর্ঘ্য(ধ্রুবক UnicodeString &strings) ধ্রুবক {
        ফেরত strings[stringOffset];
    }

    char16_t charAt(int32_t সূচক, ধ্রুবক UnicodeString &strings) ধ্রুবক {
        ফেরত strings[stringOffset+1+সূচক];
    }

    int32_t get_মান() ধ্রুবক { ফেরত value; }

    int32_t compareStringTo(ধ্রুবক UCharsTrieElement &o, ধ্রুবক UnicodeString &strings) ধ্রুবক;

ব্যক্তিগত:
    // The first strings unit contains the string length.
    // (Compared with a stringLength field here, this saves 2 bytes per string.)
    int32_t stringOffset;
    int32_t value;
};

শূন্য
UCharsTrieElement::setTo(ধ্রুবক UnicodeString &s, int32_t val,
                         UnicodeString &strings, UErrorCode &errorCode) {
    যদি(U_FAILURE(errorCode)) {
        ফেরত;
    }
    int32_t length=s.length();
    যদি(length>0xffff) {
        // Too long: We store the length in 1 unit.
        errorCode=U_সূচক_OUTOFBOUNDS_ERROR;
        ফেরত;
    }
    stringOffset=strings.length();
    strings.append(static_cast<char16_t>(length));
    value=val;
    strings.append(s);
}

int32_t
UCharsTrieElement::compareStringTo(ধ্রুবক UCharsTrieElement &other, ধ্রুবক UnicodeString &strings) ধ্রুবক {
    ফেরত getString(strings).compare(other.getString(strings));
}

UCharsTrieBuilder::UCharsTrieBuilder(UErrorCode & /*errorCode*/)
        : elements(nullptr), elementsCapacity(0), elements_দৈর্ঘ্য(0),
          uchars(nullptr), ucharsCapacity(0), uchars_দৈর্ঘ্য(0) {}

UCharsTrieBuilder::~UCharsTrieBuilder() {
    delete[] elements;
    uprv_free(uchars);
}

UCharsTrieBuilder&
UCharsTrieBuilder::add(ধ্রুবক UnicodeString& s,
                       int32_t value,
                       UErrorCode& errorCode) U_LIFETIME_BOUND {
    যদি(U_FAILURE(errorCode)) {
        ফেরত *এই;
    }
    যদি(uchars_দৈর্ঘ্য>0) {
        // Cannot add elements after building.
        errorCode=U_NO_WRITE_PERMISSION;
        ফেরত *এই;
    }
    যদি(elements_দৈর্ঘ্য==elementsCapacity) {
        int32_t newCapacity;
        যদি(elementsCapacity==0) {
            newCapacity=1024;
        } অন্যথা {
            newCapacity=4*elementsCapacity;
        }
        UCharsTrieElement *newElements=নতুন UCharsTrieElement[newCapacity];
        যদি(newElements==nullptr) {
            errorCode=U_MEMORY_ALLOCATION_ERROR;
            ফেরত *এই;
        }
        যদি(elements_দৈর্ঘ্য>0) {
            uprv_memcpy(newElements, elements, (size_t)elements_দৈর্ঘ্য*আকার(UCharsTrieElement));
        }
        delete[] elements;
        elements=newElements;
        elementsCapacity=newCapacity;
    }
    elements[elements_দৈর্ঘ্য++].setTo(s, value, strings, errorCode);
    যদি(U_SUCCESS(errorCode) && strings.isBogus()) {
        errorCode=U_MEMORY_ALLOCATION_ERROR;
    }
    ফেরত *এই;
}

U_CDECL_BEGIN

স্থির int32_t U_CALLCONV
compareElementStrings(ধ্রুবক শূন্য *context, ধ্রুবক শূন্য *left, ধ্রুবক শূন্য *ডান) {
    ধ্রুবক UnicodeString *strings=static_cast<ধ্রুবক UnicodeString *>(context);
    ধ্রুবক UCharsTrieElement *বাম_Element=static_cast<ধ্রুবক UCharsTrieElement *>(left);
    ধ্রুবক UCharsTrieElement *ডান_Element=static_cast<ধ্রুবক UCharsTrieElement *>(ডান);
    ফেরত বাম_Element->compareStringTo(*ডান_Element, *strings);
}

U_CDECL_END

UCharsTrie *
UCharsTrieBuilder::build(UStringTrieBuildOption buildOption, UErrorCode &errorCode) {
    buildUChars(buildOption, errorCode);
    UCharsTrie *newTrie=nullptr;
    যদি(U_SUCCESS(errorCode)) {
        newTrie=নতুন UCharsTrie(uchars, uchars+(ucharsCapacity-uchars_দৈর্ঘ্য));
        যদি(newTrie==nullptr) {
            errorCode=U_MEMORY_ALLOCATION_ERROR;
        } অন্যথা {
            uchars=nullptr;  // The new trie now owns the array.
            ucharsCapacity=0;
        }
    }
    ফেরত newTrie;
}

UnicodeString&
UCharsTrieBuilder::buildUnicodeString(UStringTrieBuildOption buildOption,
                                      UnicodeString& ফলাফল U_LIFETIME_BOUND,
                                      UErrorCode& errorCode) U_LIFETIME_BOUND {
    buildUChars(buildOption, errorCode);
    যদি(U_SUCCESS(errorCode)) {
        ফলাফল.setTo(মিথ্যা, uchars+(ucharsCapacity-uchars_দৈর্ঘ্য), uchars_দৈর্ঘ্য);
    }
    ফেরত ফলাফল;
}

শূন্য
UCharsTrieBuilder::buildUChars(UStringTrieBuildOption buildOption, UErrorCode &errorCode) {
    যদি(U_FAILURE(errorCode)) {
        ফেরত;
    }
    যদি(uchars!=nullptr && uchars_দৈর্ঘ্য>0) {
        // Already built.
        ফেরত;
    }
    যদি(uchars_দৈর্ঘ্য==0) {
        যদি(elements_দৈর্ঘ্য==0) {
            errorCode=U_সূচক_OUTOFBOUNDS_ERROR;
            ফেরত;
        }
        যদি(strings.isBogus()) {
            errorCode=U_MEMORY_ALLOCATION_ERROR;
            ফেরত;
        }
        uprv_sortArray(elements, elements_দৈর্ঘ্য, static_cast<int32_t>(আকার(UCharsTrieElement)),
                      compareElementStrings, &strings,
                      মিথ্যা,  // need not be a stable sort
                      &errorCode);
        যদি(U_FAILURE(errorCode)) {
            ফেরত;
        }
        // Duplicate strings are not allowed.
        UnicodeString পূর্ব=elements[0].getString(strings);
        জন্য(int32_t i=1; i<elements_দৈর্ঘ্য; ++i) {
            UnicodeString current=elements[i].getString(strings);
            যদি(পূর্ব==current) {
                errorCode=U_ILLEGAL_ARGUMENT_ERROR;
                ফেরত;
            }
            পূর্ব.fastCopyFrom(current);
        }
    }
    // Create and char16_t-serialize the trie for the elements.
    uchars_দৈর্ঘ্য=0;
    int32_t capacity=strings.length();
    যদি(capacity<1024) {
        capacity=1024;
    }
    যদি(ucharsCapacity<capacity) {
        uprv_free(uchars);
        uchars=static_cast<char16_t *>(uprv_malloc(capacity*2));
        যদি(uchars==nullptr) {
            errorCode=U_MEMORY_ALLOCATION_ERROR;
            ucharsCapacity=0;
            ফেরত;
        }
        ucharsCapacity=capacity;
    }
    StringTrieBuilder::build(buildOption, elements_দৈর্ঘ্য, errorCode);
    যদি(uchars==nullptr) {
        errorCode=U_MEMORY_ALLOCATION_ERROR;
    }
}

int32_t
UCharsTrieBuilder::get_Element_String_দৈর্ঘ্য(int32_t i) ধ্রুবক {
    ফেরত elements[i].get_String_দৈর্ঘ্য(strings);
}

char16_t
UCharsTrieBuilder::getElementUnit(int32_t i, int32_t unit_সূচক) ধ্রুবক {
    ফেরত elements[i].charAt(unit_সূচক, strings);
}

int32_t
UCharsTrieBuilder::get_Element_মান(int32_t i) ধ্রুবক {
    ফেরত elements[i].get_মান();
}

int32_t
UCharsTrieBuilder::getLimitOfLinearMatch(int32_t first, int32_t last, int32_t unit_সূচক) ধ্রুবক {
    ধ্রুবক UCharsTrieElement &firstElement=elements[first];
    ধ্রুবক UCharsTrieElement &lastElement=elements[last];
    int32_t min_String_দৈর্ঘ্য=firstElement.get_String_দৈর্ঘ্য(strings);
    যখন(++unit_সূচক<min_String_দৈর্ঘ্য &&
            firstElement.charAt(unit_সূচক, strings)==
            lastElement.charAt(unit_সূচক, strings)) {}
    ফেরত unit_সূচক;
}

int32_t
UCharsTrieBuilder::গুনতি_Element_Units(int32_t start, int32_t limit, int32_t unit_সূচক) ধ্রুবক {
    int32_t length=0;  // Number of different units at unitIndex.
    int32_t i=start;
    করো {
        char16_t unit=elements[i++].charAt(unit_সূচক, strings);
        যখন(i<limit && unit==elements[i].charAt(unit_সূচক, strings)) {
            ++i;
        }
        ++length;
    } যখন(i<limit);
    ফেরত length;
}

int32_t
UCharsTrieBuilder::skipElementsBySomeUnits(int32_t i, int32_t unit_সূচক, int32_t count) ধ্রুবক {
    করো {
        char16_t unit=elements[i++].charAt(unit_সূচক, strings);
        যখন(unit==elements[i].charAt(unit_সূচক, strings)) {
            ++i;
        }
    } যখন(--count>0);
    ফেরত i;
}

int32_t
UCharsTrieBuilder::সূচক_Of_Element_With_পরবর্তী_Unit(int32_t i, int32_t unit_সূচক, char16_t unit) ধ্রুবক {
    যখন(unit==elements[i].charAt(unit_সূচক, strings)) {
        ++i;
    }
    ফেরত i;
}

UCharsTrieBuilder::UCTLinear_Match_গ্রন্থি::UCTLinear_Match_গ্রন্থি(ধ্রুবক char16_t *units, int32_t len, Node *পরবর্তী_গ্রন্থি)
        : Linear_Match_গ্রন্থি(len, পরবর্তী_গ্রন্থি), s(units) {
    hash=hash*37u+ustr_hashUCharsN(units, len);
}

যৌক্তিক
UCharsTrieBuilder::UCTLinear_Match_গ্রন্থি::operator==(ধ্রুবক Node &other) ধ্রুবক {
    যদি(এই==&other) {
        ফেরত সত্য;
    }
    যদি(!Linear_Match_গ্রন্থি::operator==(other)) {
        ফেরত মিথ্যা;
    }
    ধ্রুবক UCTLinear_Match_গ্রন্থি &o=static_cast<ধ্রুবক UCTLinear_Match_গ্রন্থি &>(other);
    ফেরত 0==u_memcmp(s, o.s, length);
}

শূন্য
UCharsTrieBuilder::UCTLinear_Match_গ্রন্থি::লিখ(StringTrieBuilder &builder) {
    UCharsTrieBuilder &b=(UCharsTrieBuilder &)builder;
    next->লিখ(builder);
    b.লিখ(s, length);
    offset=b.write_মান_And_Type(has_মান, value, b.getMinLinearMatch()+length-1);
}

StringTrieBuilder::Node *
UCharsTrieBuilder::create_Linear_Match_গ্রন্থি(int32_t i, int32_t unit_সূচক, int32_t length,
                                         Node *পরবর্তী_গ্রন্থি) ধ্রুবক {
    ফেরত নতুন UCTLinear_Match_গ্রন্থি(
            elements[i].getString(strings).get_বাফার()+unit_সূচক,
            length,
            পরবর্তী_গ্রন্থি);
}

UBool
UCharsTrieBuilder::ensureCapacity(int32_t length) {
    যদি(uchars==nullptr) {
        ফেরত মিথ্যা;  // previous memory allocation had failed
    }
    যদি(length>ucharsCapacity) {
        int32_t newCapacity=ucharsCapacity;
        করো {
            newCapacity*=2;
        } যখন(newCapacity<=length);
        char16_t *newUChars=static_cast<char16_t *>(uprv_malloc(newCapacity*2));
        যদি(newUChars==nullptr) {
            // unable to allocate memory
            uprv_free(uchars);
            uchars=nullptr;
            ucharsCapacity=0;
            ফেরত মিথ্যা;
        }
        u_memcpy(newUChars+(newCapacity-uchars_দৈর্ঘ্য),
                 uchars+(ucharsCapacity-uchars_দৈর্ঘ্য), uchars_দৈর্ঘ্য);
        uprv_free(uchars);
        uchars=newUChars;
        ucharsCapacity=newCapacity;
    }
    ফেরত সত্য;
}

int32_t
UCharsTrieBuilder::লিখ(int32_t unit) {
    int32_t new_দৈর্ঘ্য=uchars_দৈর্ঘ্য+1;
    যদি(ensureCapacity(new_দৈর্ঘ্য)) {
        uchars_দৈর্ঘ্য=new_দৈর্ঘ্য;
        uchars[ucharsCapacity - uchars_দৈর্ঘ্য] = static_cast<char16_t>(unit);
    }
    ফেরত uchars_দৈর্ঘ্য;
}

int32_t
UCharsTrieBuilder::লিখ(ধ্রুবক char16_t *s, int32_t length) {
    int32_t new_দৈর্ঘ্য=uchars_দৈর্ঘ্য+length;
    যদি(ensureCapacity(new_দৈর্ঘ্য)) {
        uchars_দৈর্ঘ্য=new_দৈর্ঘ্য;
        u_memcpy(uchars+(ucharsCapacity-uchars_দৈর্ঘ্য), s, length);
    }
    ফেরত uchars_দৈর্ঘ্য;
}

int32_t
UCharsTrieBuilder::writeElementUnits(int32_t i, int32_t unit_সূচক, int32_t length) {
    ফেরত লিখ(elements[i].getString(strings).get_বাফার()+unit_সূচক, length);
}

int32_t
UCharsTrieBuilder::write_মান_And_Final(int32_t i, UBool isFinal) {
    যদি(0<=i && i<=UCharsTrie::k_Max_One_Unit_মান) {
        ফেরত লিখ(i|(isFinal<<15));
    }
    char16_t intUnits[3];
    int32_t length;
    যদি(i<0 || i>UCharsTrie::k_Max_Two_Unit_মান) {
        intUnits[0] = static_cast<char16_t>(UCharsTrie::k_Three_Unit_মান_Lead);
        intUnits[1] = static_cast<char16_t>(static_cast<uint32_t>(i) >> 16);
        intUnits[2] = static_cast<char16_t>(i);
        length=3;
    // } else if(i<=UCharsTrie::kMaxOneUnitValue) {
    //     intUnits[0]=(char16_t)(i);
    //     length=1;
    } অন্যথা {
        intUnits[0] = static_cast<char16_t>(UCharsTrie::k_Min_Two_Unit_মান_Lead + (i >> 16));
        intUnits[1] = static_cast<char16_t>(i);
        length=2;
    }
    intUnits[0] = static_cast<char16_t>(intUnits[0] | (isFinal << 15));
    ফেরত লিখ(intUnits, length);
}

int32_t
UCharsTrieBuilder::write_মান_And_Type(UBool has_মান, int32_t value, int32_t গ্রন্থি) {
    যদি(!has_মান) {
        ফেরত লিখ(গ্রন্থি);
    }
    char16_t intUnits[3];
    int32_t length;
    যদি(value<0 || value>UCharsTrie::k_Max_Two_Unit_গ্রন্থি_মান) {
        intUnits[0] = static_cast<char16_t>(UCharsTrie::k_Three_Unit_গ্রন্থি_মান_Lead);
        intUnits[1] = static_cast<char16_t>(static_cast<uint32_t>(value) >> 16);
        intUnits[2] = static_cast<char16_t>(value);
        length=3;
    } অন্যথা যদি(value<=UCharsTrie::k_Max_One_Unit_গ্রন্থি_মান) {
        intUnits[0] = static_cast<char16_t>((value + 1) << 6);
        length=1;
    } অন্যথা {
        intUnits[0] = static_cast<char16_t>(UCharsTrie::k_Min_Two_Unit_গ্রন্থি_মান_Lead + ((value >> 10) & 0x7fc0));
        intUnits[1] = static_cast<char16_t>(value);
        length=2;
    }
    intUnits[0] |= static_cast<char16_t>(গ্রন্থি);
    ফেরত লিখ(intUnits, length);
}

int32_t
UCharsTrieBuilder::writeDeltaTo(int32_t jumpTarget) {
    int32_t i=uchars_দৈর্ঘ্য-jumpTarget;
    U_ASSERT(i>=0);
    যদি(i<=UCharsTrie::kMaxOneUnitDelta) {
        ফেরত লিখ(i);
    }
    char16_t intUnits[3];
    int32_t length;
    যদি(i<=UCharsTrie::kMaxTwoUnitDelta) {
        intUnits[0] = static_cast<char16_t>(UCharsTrie::kMinTwoUnitDeltaLead + (i >> 16));
        length=1;
    } অন্যথা {
        intUnits[0] = static_cast<char16_t>(UCharsTrie::kThreeUnitDeltaLead);
        intUnits[1] = static_cast<char16_t>(i >> 16);
        length=2;
    }
    intUnits[length++] = static_cast<char16_t>(i);
    ফেরত লিখ(intUnits, length);
}

U_NAMESPACE_END
