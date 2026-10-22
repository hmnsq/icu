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

جماعت UCharsTrieElement : عوامی UMemory {
عوامی:
    // Use compiler's default constructor, initializes nothing.

    خالی سیٹ_مقصد(ثابت UnicodeString &s, int32_t val, UnicodeString &لڑیاں, UError_رمز &error_رمز);

    UnicodeString getString(ثابت UnicodeString& لڑیاں U_LIFETIME_BOUND) ثابت {
        int32_t length=لڑیاں[string_انحراف];
        واپس لڑیاں.عارضی_Sub_String(string_انحراف+1, length);
    }
    int32_t get_String_لمبائی(ثابت UnicodeString &لڑیاں) ثابت {
        واپس لڑیاں[string_انحراف];
    }

    char16_t charAt(int32_t اشاریہ, ثابت UnicodeString &لڑیاں) ثابت {
        واپس لڑیاں[string_انحراف+1+اشاریہ];
    }

    int32_t get_قدر() ثابت { واپس value; }

    int32_t compare_String_مقصد(ثابت UCharsTrieElement &o, ثابت UnicodeString &لڑیاں) ثابت;

نجی:
    // The first strings unit contains the string length.
    // (Compared with a stringLength field here, this saves 2 bytes per string.)
    int32_t string_انحراف;
    int32_t value;
};

خالی
UCharsTrieElement::سیٹ_مقصد(ثابت UnicodeString &s, int32_t val,
                         UnicodeString &لڑیاں, UError_رمز &error_رمز) {
    اگر(U_FAILURE(error_رمز)) {
        واپس;
    }
    int32_t length=s.length();
    اگر(length>0xffff) {
        // Too long: We store the length in 1 unit.
        error_رمز=U_اشاریہ_OUTOFBOUNDS_ERROR;
        واپس;
    }
    string_انحراف=لڑیاں.length();
    لڑیاں.append(static_cast<char16_t>(length));
    value=val;
    لڑیاں.append(s);
}

int32_t
UCharsTrieElement::compare_String_مقصد(ثابت UCharsTrieElement &other, ثابت UnicodeString &لڑیاں) ثابت {
    واپس getString(لڑیاں).compare(other.getString(لڑیاں));
}

UCharsTrieBuilder::UCharsTrieBuilder(UError_رمز & /*errorCode*/)
        : elements(خالی_اشاریہ), elements_استعداد(0), elements_لمبائی(0),
          uchars(خالی_اشاریہ), uchars_استعداد(0), uchars_لمبائی(0) {}

UCharsTrieBuilder::~UCharsTrieBuilder() {
    حذف[] elements;
    uprv_free(uchars);
}

UCharsTrieBuilder&
UCharsTrieBuilder::جوڑو(ثابت UnicodeString& s,
                       int32_t value,
                       UError_رمز& error_رمز) U_LIFETIME_BOUND {
    اگر(U_FAILURE(error_رمز)) {
        واپس *یہ;
    }
    اگر(uchars_لمبائی>0) {
        // Cannot add elements after building.
        error_رمز=U_NO_WRITE_PERMISSION;
        واپس *یہ;
    }
    اگر(elements_لمبائی==elements_استعداد) {
        int32_t new_استعداد;
        اگر(elements_استعداد==0) {
            new_استعداد=1024;
        } ورنہ {
            new_استعداد=4*elements_استعداد;
        }
        UCharsTrieElement *newElements=نیا UCharsTrieElement[new_استعداد];
        اگر(newElements==خالی_اشاریہ) {
            error_رمز=U_حافظہ_تخصیص_ERROR;
            واپس *یہ;
        }
        اگر(elements_لمبائی>0) {
            uprv_memcpy(newElements, elements, (size_t)elements_لمبائی*حجم(UCharsTrieElement));
        }
        حذف[] elements;
        elements=newElements;
        elements_استعداد=new_استعداد;
    }
    elements[elements_لمبائی++].سیٹ_مقصد(s, value, لڑیاں, error_رمز);
    اگر(U_SUCCESS(error_رمز) && لڑیاں.isBogus()) {
        error_رمز=U_حافظہ_تخصیص_ERROR;
    }
    واپس *یہ;
}

U_CDECL_BEGIN

ساکن int32_t U_CALLCONV
compare_Element_لڑیاں(ثابت خالی *context, ثابت خالی *بائیں, ثابت خالی *دائیں) {
    ثابت UnicodeString *لڑیاں=static_cast<ثابت UnicodeString *>(context);
    ثابت UCharsTrieElement *بایاں_Element=static_cast<ثابت UCharsTrieElement *>(بائیں);
    ثابت UCharsTrieElement *دایاں_Element=static_cast<ثابت UCharsTrieElement *>(دائیں);
    واپس بایاں_Element->compare_String_مقصد(*دایاں_Element, *لڑیاں);
}

U_CDECL_انتہا

UCharsTrie *
UCharsTrieBuilder::build(UStringTrieBuildOption buildOption, UError_رمز &error_رمز) {
    buildUChars(buildOption, error_رمز);
    UCharsTrie *newTrie=خالی_اشاریہ;
    اگر(U_SUCCESS(error_رمز)) {
        newTrie=نیا UCharsTrie(uchars, uchars+(uchars_استعداد-uchars_لمبائی));
        اگر(newTrie==خالی_اشاریہ) {
            error_رمز=U_حافظہ_تخصیص_ERROR;
        } ورنہ {
            uchars=خالی_اشاریہ;  // The new trie now owns the array.
            uchars_استعداد=0;
        }
    }
    واپس newTrie;
}

UnicodeString&
UCharsTrieBuilder::buildUnicodeString(UStringTrieBuildOption buildOption,
                                      UnicodeString& نتیجہ U_LIFETIME_BOUND,
                                      UError_رمز& error_رمز) U_LIFETIME_BOUND {
    buildUChars(buildOption, error_رمز);
    اگر(U_SUCCESS(error_رمز)) {
        نتیجہ.سیٹ_مقصد(جھوٹ, uchars+(uchars_استعداد-uchars_لمبائی), uchars_لمبائی);
    }
    واپس نتیجہ;
}

خالی
UCharsTrieBuilder::buildUChars(UStringTrieBuildOption buildOption, UError_رمز &error_رمز) {
    اگر(U_FAILURE(error_رمز)) {
        واپس;
    }
    اگر(uchars!=خالی_اشاریہ && uchars_لمبائی>0) {
        // Already built.
        واپس;
    }
    اگر(uchars_لمبائی==0) {
        اگر(elements_لمبائی==0) {
            error_رمز=U_اشاریہ_OUTOFBOUNDS_ERROR;
            واپس;
        }
        اگر(لڑیاں.isBogus()) {
            error_رمز=U_حافظہ_تخصیص_ERROR;
            واپس;
        }
        uprv_ترتیب_دو_Array(elements, elements_لمبائی, static_cast<int32_t>(حجم(UCharsTrieElement)),
                      compare_Element_لڑیاں, &لڑیاں,
                      جھوٹ,  // need not be a stable sort
                      &error_رمز);
        اگر(U_FAILURE(error_رمز)) {
            واپس;
        }
        // Duplicate strings are not allowed.
        UnicodeString پچھلا=elements[0].getString(لڑیاں);
        برائے(int32_t i=1; i<elements_لمبائی; ++i) {
            UnicodeString current=elements[i].getString(لڑیاں);
            اگر(پچھلا==current) {
                error_رمز=U_ILLEGAL_دلیل_ERROR;
                واپس;
            }
            پچھلا.fastCopyFrom(current);
        }
    }
    // Create and char16_t-serialize the trie for the elements.
    uchars_لمبائی=0;
    int32_t capacity=لڑیاں.length();
    اگر(capacity<1024) {
        capacity=1024;
    }
    اگر(uchars_استعداد<capacity) {
        uprv_free(uchars);
        uchars=static_cast<char16_t *>(uprv_malloc(capacity*2));
        اگر(uchars==خالی_اشاریہ) {
            error_رمز=U_حافظہ_تخصیص_ERROR;
            uchars_استعداد=0;
            واپس;
        }
        uchars_استعداد=capacity;
    }
    StringTrieBuilder::build(buildOption, elements_لمبائی, error_رمز);
    اگر(uchars==خالی_اشاریہ) {
        error_رمز=U_حافظہ_تخصیص_ERROR;
    }
}

int32_t
UCharsTrieBuilder::get_Element_String_لمبائی(int32_t i) ثابت {
    واپس elements[i].get_String_لمبائی(لڑیاں);
}

char16_t
UCharsTrieBuilder::getElementUnit(int32_t i, int32_t unit_اشاریہ) ثابت {
    واپس elements[i].charAt(unit_اشاریہ, لڑیاں);
}

int32_t
UCharsTrieBuilder::get_Element_قدر(int32_t i) ثابت {
    واپس elements[i].get_قدر();
}

int32_t
UCharsTrieBuilder::get_حد_بندی_کا_خطی_مطابقت(int32_t first, int32_t آخری, int32_t unit_اشاریہ) ثابت {
    ثابت UCharsTrieElement &پہلا_Element=elements[first];
    ثابت UCharsTrieElement &آخری_Element=elements[آخری];
    int32_t min_String_لمبائی=پہلا_Element.get_String_لمبائی(لڑیاں);
    جب(++unit_اشاریہ<min_String_لمبائی &&
            پہلا_Element.charAt(unit_اشاریہ, لڑیاں)==
            آخری_Element.charAt(unit_اشاریہ, لڑیاں)) {}
    واپس unit_اشاریہ;
}

int32_t
UCharsTrieBuilder::گنتی_Element_Units(int32_t آغاز, int32_t حد_بندی, int32_t unit_اشاریہ) ثابت {
    int32_t length=0;  // Number of different units at unitIndex.
    int32_t i=آغاز;
    کرو {
        char16_t unit=elements[i++].charAt(unit_اشاریہ, لڑیاں);
        جب(i<حد_بندی && unit==elements[i].charAt(unit_اشاریہ, لڑیاں)) {
            ++i;
        }
        ++length;
    } جب(i<حد_بندی);
    واپس length;
}

int32_t
UCharsTrieBuilder::skipElementsBySomeUnits(int32_t i, int32_t unit_اشاریہ, int32_t count) ثابت {
    کرو {
        char16_t unit=elements[i++].charAt(unit_اشاریہ, لڑیاں);
        جب(unit==elements[i].charAt(unit_اشاریہ, لڑیاں)) {
            ++i;
        }
    } جب(--count>0);
    واپس i;
}

int32_t
UCharsTrieBuilder::اشاریہ_کا_Element_With_اگلا_Unit(int32_t i, int32_t unit_اشاریہ, char16_t unit) ثابت {
    جب(unit==elements[i].charAt(unit_اشاریہ, لڑیاں)) {
        ++i;
    }
    واپس i;
}

UCharsTrieBuilder::UCTLinear_مطابقت_گرہ::UCTLinear_مطابقت_گرہ(ثابت char16_t *units, int32_t len, Node *اگلا_گرہ)
        : خطی_مطابقت_گرہ(len, اگلا_گرہ), s(units) {
    hash=hash*37u+ustr_hashUCharsN(units, len);
}

منطقی
UCharsTrieBuilder::UCTLinear_مطابقت_گرہ::عامل==(ثابت Node &other) ثابت {
    اگر(یہ==&other) {
        واپس سچ;
    }
    اگر(!خطی_مطابقت_گرہ::عامل==(other)) {
        واپس جھوٹ;
    }
    ثابت UCTLinear_مطابقت_گرہ &o=static_cast<ثابت UCTLinear_مطابقت_گرہ &>(other);
    واپس 0==u_memcmp(s, o.s, length);
}

خالی
UCharsTrieBuilder::UCTLinear_مطابقت_گرہ::اكتب(StringTrieBuilder &builder) {
    UCharsTrieBuilder &b=(UCharsTrieBuilder &)builder;
    next->اكتب(builder);
    b.اكتب(s, length);
    انحراف=b.write_قدر_And_Type(has_قدر, value, b.get_Min_خطی_مطابقت()+length-1);
}

StringTrieBuilder::Node *
UCharsTrieBuilder::create_خطی_مطابقت_گرہ(int32_t i, int32_t unit_اشاریہ, int32_t length,
                                         Node *اگلا_گرہ) ثابت {
    واپس نیا UCTLinear_مطابقت_گرہ(
            elements[i].getString(لڑیاں).get_بفر()+unit_اشاریہ,
            length,
            اگلا_گرہ);
}

UBool
UCharsTrieBuilder::ensure_استعداد(int32_t length) {
    اگر(uchars==خالی_اشاریہ) {
        واپس جھوٹ;  // previous memory allocation had failed
    }
    اگر(length>uchars_استعداد) {
        int32_t new_استعداد=uchars_استعداد;
        کرو {
            new_استعداد*=2;
        } جب(new_استعداد<=length);
        char16_t *newUChars=static_cast<char16_t *>(uprv_malloc(new_استعداد*2));
        اگر(newUChars==خالی_اشاریہ) {
            // unable to allocate memory
            uprv_free(uchars);
            uchars=خالی_اشاریہ;
            uchars_استعداد=0;
            واپس جھوٹ;
        }
        u_memcpy(newUChars+(new_استعداد-uchars_لمبائی),
                 uchars+(uchars_استعداد-uchars_لمبائی), uchars_لمبائی);
        uprv_free(uchars);
        uchars=newUChars;
        uchars_استعداد=new_استعداد;
    }
    واپس سچ;
}

int32_t
UCharsTrieBuilder::اكتب(int32_t unit) {
    int32_t new_لمبائی=uchars_لمبائی+1;
    اگر(ensure_استعداد(new_لمبائی)) {
        uchars_لمبائی=new_لمبائی;
        uchars[uchars_استعداد - uchars_لمبائی] = static_cast<char16_t>(unit);
    }
    واپس uchars_لمبائی;
}

int32_t
UCharsTrieBuilder::اكتب(ثابت char16_t *s, int32_t length) {
    int32_t new_لمبائی=uchars_لمبائی+length;
    اگر(ensure_استعداد(new_لمبائی)) {
        uchars_لمبائی=new_لمبائی;
        u_memcpy(uchars+(uchars_استعداد-uchars_لمبائی), s, length);
    }
    واپس uchars_لمبائی;
}

int32_t
UCharsTrieBuilder::writeElementUnits(int32_t i, int32_t unit_اشاریہ, int32_t length) {
    واپس اكتب(elements[i].getString(لڑیاں).get_بفر()+unit_اشاریہ, length);
}

int32_t
UCharsTrieBuilder::write_قدر_And_Final(int32_t i, UBool isFinal) {
    اگر(0<=i && i<=UCharsTrie::k_Max_One_Unit_قدر) {
        واپس اكتب(i|(isFinal<<15));
    }
    char16_t intUnits[3];
    int32_t length;
    اگر(i<0 || i>UCharsTrie::k_Max_Two_Unit_قدر) {
        intUnits[0] = static_cast<char16_t>(UCharsTrie::k_Three_Unit_قدر_Lead);
        intUnits[1] = static_cast<char16_t>(static_cast<uint32_t>(i) >> 16);
        intUnits[2] = static_cast<char16_t>(i);
        length=3;
    // } else if(i<=UCharsTrie::kMaxOneUnitValue) {
    //     intUnits[0]=(char16_t)(i);
    //     length=1;
    } ورنہ {
        intUnits[0] = static_cast<char16_t>(UCharsTrie::k_Min_Two_Unit_قدر_Lead + (i >> 16));
        intUnits[1] = static_cast<char16_t>(i);
        length=2;
    }
    intUnits[0] = static_cast<char16_t>(intUnits[0] | (isFinal << 15));
    واپس اكتب(intUnits, length);
}

int32_t
UCharsTrieBuilder::write_قدر_And_Type(UBool has_قدر, int32_t value, int32_t گرہ) {
    اگر(!has_قدر) {
        واپس اكتب(گرہ);
    }
    char16_t intUnits[3];
    int32_t length;
    اگر(value<0 || value>UCharsTrie::k_Max_Two_Unit_گرہ_قدر) {
        intUnits[0] = static_cast<char16_t>(UCharsTrie::k_Three_Unit_گرہ_قدر_Lead);
        intUnits[1] = static_cast<char16_t>(static_cast<uint32_t>(value) >> 16);
        intUnits[2] = static_cast<char16_t>(value);
        length=3;
    } ورنہ اگر(value<=UCharsTrie::k_Max_One_Unit_گرہ_قدر) {
        intUnits[0] = static_cast<char16_t>((value + 1) << 6);
        length=1;
    } ورنہ {
        intUnits[0] = static_cast<char16_t>(UCharsTrie::k_Min_Two_Unit_گرہ_قدر_Lead + ((value >> 10) & 0x7fc0));
        intUnits[1] = static_cast<char16_t>(value);
        length=2;
    }
    intUnits[0] |= static_cast<char16_t>(گرہ);
    واپس اكتب(intUnits, length);
}

int32_t
UCharsTrieBuilder::write_Delta_مقصد(int32_t jump_ہدف) {
    int32_t i=uchars_لمبائی-jump_ہدف;
    U_ASSERT(i>=0);
    اگر(i<=UCharsTrie::kMaxOneUnitDelta) {
        واپس اكتب(i);
    }
    char16_t intUnits[3];
    int32_t length;
    اگر(i<=UCharsTrie::kMaxTwoUnitDelta) {
        intUnits[0] = static_cast<char16_t>(UCharsTrie::kMinTwoUnitDeltaLead + (i >> 16));
        length=1;
    } ورنہ {
        intUnits[0] = static_cast<char16_t>(UCharsTrie::kThreeUnitDeltaLead);
        intUnits[1] = static_cast<char16_t>(i >> 16);
        length=2;
    }
    intUnits[length++] = static_cast<char16_t>(i);
    واپس اكتب(intUnits, length);
}

U_NAMESPACE_انتہا
