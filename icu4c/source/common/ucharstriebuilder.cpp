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

    فراغ سیٹ_منزل(ثابت UnicodeString &s, int32_t val, UnicodeString &لڑیاں, UError_کوڈ &error_کوڈ);

    UnicodeString getString(ثابت UnicodeString& لڑیاں U_LIFETIME_BOUND) ثابت {
        int32_t length=لڑیاں[string_وتھ];
        أرجع لڑیاں.عارضی_Sub_String(string_وتھ+1, length);
    }
    int32_t get_String_لمائی(ثابت UnicodeString &لڑیاں) ثابت {
        أرجع لڑیاں[string_وتھ];
    }

    char16_t charAt(int32_t اشاریہ, ثابت UnicodeString &لڑیاں) ثابت {
        أرجع لڑیاں[string_وتھ+1+اشاریہ];
    }

    int32_t get_قدر() ثابت { أرجع value; }

    int32_t compare_String_منزل(ثابت UCharsTrieElement &o, ثابت UnicodeString &لڑیاں) ثابت;

خاص:
    // The first strings unit contains the string length.
    // (Compared with a stringLength field here, this saves 2 bytes per string.)
    int32_t string_وتھ;
    int32_t value;
};

فراغ
UCharsTrieElement::سیٹ_منزل(ثابت UnicodeString &s, int32_t val,
                         UnicodeString &لڑیاں, UError_کوڈ &error_کوڈ) {
    إذا(U_FAILURE(error_کوڈ)) {
        أرجع;
    }
    int32_t length=s.length();
    إذا(length>0xffff) {
        // Too long: We store the length in 1 unit.
        error_کوڈ=U_اشاریہ_OUTOFBOUNDS_ERROR;
        أرجع;
    }
    string_وتھ=لڑیاں.length();
    لڑیاں.append(static_cast<char16_t>(length));
    value=val;
    لڑیاں.append(s);
}

int32_t
UCharsTrieElement::compare_String_منزل(ثابت UCharsTrieElement &other, ثابت UnicodeString &لڑیاں) ثابت {
    أرجع getString(لڑیاں).compare(other.getString(لڑیاں));
}

UCharsTrieBuilder::UCharsTrieBuilder(UError_کوڈ & /*errorCode*/)
        : elements(خالی_سوچک), elements_سمرتھا(0), elements_لمائی(0),
          uchars(خالی_سوچک), uchars_سمرتھا(0), uchars_لمائی(0) {}

UCharsTrieBuilder::~UCharsTrieBuilder() {
    مٹاؤ[] elements;
    uprv_free(uchars);
}

UCharsTrieBuilder&
UCharsTrieBuilder::جوڑو(ثابت UnicodeString& s,
                       int32_t value,
                       UError_کوڈ& error_کوڈ) U_LIFETIME_BOUND {
    إذا(U_FAILURE(error_کوڈ)) {
        أرجع *هذا;
    }
    إذا(uchars_لمائی>0) {
        // Cannot add elements after building.
        error_کوڈ=U_NO_WRITE_PERMISSION;
        أرجع *هذا;
    }
    إذا(elements_لمائی==elements_سمرتھا) {
        int32_t new_سمرتھا;
        إذا(elements_سمرتھا==0) {
            new_سمرتھا=1024;
        } وإلا {
            new_سمرتھا=4*elements_سمرتھا;
        }
        UCharsTrieElement *newElements=جديد UCharsTrieElement[new_سمرتھا];
        إذا(newElements==خالی_سوچک) {
            error_کوڈ=U_حافظہ_ونڈ_ERROR;
            أرجع *هذا;
        }
        إذا(elements_لمائی>0) {
            uprv_memcpy(newElements, elements, (size_t)elements_لمائی*حجم(UCharsTrieElement));
        }
        مٹاؤ[] elements;
        elements=newElements;
        elements_سمرتھا=new_سمرتھا;
    }
    elements[elements_لمائی++].سیٹ_منزل(s, value, لڑیاں, error_کوڈ);
    إذا(U_SUCCESS(error_کوڈ) && لڑیاں.isBogus()) {
        error_کوڈ=U_حافظہ_ونڈ_ERROR;
    }
    أرجع *هذا;
}

U_CDECL_BEGIN

ساكن int32_t U_CALLCONV
compare_Element_لڑیاں(ثابت فراغ *context, ثابت فراغ *کھبے, ثابت فراغ *سجے) {
    ثابت UnicodeString *لڑیاں=static_cast<ثابت UnicodeString *>(context);
    ثابت UCharsTrieElement *بایاں_Element=static_cast<ثابت UCharsTrieElement *>(کھبے);
    ثابت UCharsTrieElement *دایاں_Element=static_cast<ثابت UCharsTrieElement *>(سجے);
    أرجع بایاں_Element->compare_String_منزل(*دایاں_Element, *لڑیاں);
}

U_CDECL_سرا

UCharsTrie *
UCharsTrieBuilder::build(UStringTrieBuildOption buildOption, UError_کوڈ &error_کوڈ) {
    buildUChars(buildOption, error_کوڈ);
    UCharsTrie *newTrie=خالی_سوچک;
    إذا(U_SUCCESS(error_کوڈ)) {
        newTrie=جديد UCharsTrie(uchars, uchars+(uchars_سمرتھا-uchars_لمائی));
        إذا(newTrie==خالی_سوچک) {
            error_کوڈ=U_حافظہ_ونڈ_ERROR;
        } وإلا {
            uchars=خالی_سوچک;  // The new trie now owns the array.
            uchars_سمرتھا=0;
        }
    }
    أرجع newTrie;
}

UnicodeString&
UCharsTrieBuilder::buildUnicodeString(UStringTrieBuildOption buildOption,
                                      UnicodeString& نتیجہ U_LIFETIME_BOUND,
                                      UError_کوڈ& error_کوڈ) U_LIFETIME_BOUND {
    buildUChars(buildOption, error_کوڈ);
    إذا(U_SUCCESS(error_کوڈ)) {
        نتیجہ.سیٹ_منزل(خطأ, uchars+(uchars_سمرتھا-uchars_لمائی), uchars_لمائی);
    }
    أرجع نتیجہ;
}

فراغ
UCharsTrieBuilder::buildUChars(UStringTrieBuildOption buildOption, UError_کوڈ &error_کوڈ) {
    إذا(U_FAILURE(error_کوڈ)) {
        أرجع;
    }
    إذا(uchars!=خالی_سوچک && uchars_لمائی>0) {
        // Already built.
        أرجع;
    }
    إذا(uchars_لمائی==0) {
        إذا(elements_لمائی==0) {
            error_کوڈ=U_اشاریہ_OUTOFBOUNDS_ERROR;
            أرجع;
        }
        إذا(لڑیاں.isBogus()) {
            error_کوڈ=U_حافظہ_ونڈ_ERROR;
            أرجع;
        }
        uprv_ترتیب_دیو_Array(elements, elements_لمائی, static_cast<int32_t>(حجم(UCharsTrieElement)),
                      compare_Element_لڑیاں, &لڑیاں,
                      خطأ,  // need not be a stable sort
                      &error_کوڈ);
        إذا(U_FAILURE(error_کوڈ)) {
            أرجع;
        }
        // Duplicate strings are not allowed.
        UnicodeString prev=elements[0].getString(لڑیاں);
        لكل(int32_t i=1; i<elements_لمائی; ++i) {
            UnicodeString current=elements[i].getString(لڑیاں);
            إذا(prev==current) {
                error_کوڈ=U_ILLEGAL_دلیل_ERROR;
                أرجع;
            }
            prev.fastCopyFrom(current);
        }
    }
    // Create and char16_t-serialize the trie for the elements.
    uchars_لمائی=0;
    int32_t capacity=لڑیاں.length();
    إذا(capacity<1024) {
        capacity=1024;
    }
    إذا(uchars_سمرتھا<capacity) {
        uprv_free(uchars);
        uchars=static_cast<char16_t *>(uprv_malloc(capacity*2));
        إذا(uchars==خالی_سوچک) {
            error_کوڈ=U_حافظہ_ونڈ_ERROR;
            uchars_سمرتھا=0;
            أرجع;
        }
        uchars_سمرتھا=capacity;
    }
    StringTrieBuilder::build(buildOption, elements_لمائی, error_کوڈ);
    إذا(uchars==خالی_سوچک) {
        error_کوڈ=U_حافظہ_ونڈ_ERROR;
    }
}

int32_t
UCharsTrieBuilder::get_Element_String_لمائی(int32_t i) ثابت {
    أرجع elements[i].get_String_لمائی(لڑیاں);
}

char16_t
UCharsTrieBuilder::getElementUnit(int32_t i, int32_t unit_اشاریہ) ثابت {
    أرجع elements[i].charAt(unit_اشاریہ, لڑیاں);
}

int32_t
UCharsTrieBuilder::get_Element_قدر(int32_t i) ثابت {
    أرجع elements[i].get_قدر();
}

int32_t
UCharsTrieBuilder::get_حد_بندی_دا_خطی_میل(int32_t first, int32_t آخری, int32_t unit_اشاریہ) ثابت {
    ثابت UCharsTrieElement &پہلا_Element=elements[first];
    ثابت UCharsTrieElement &آخری_Element=elements[آخری];
    int32_t min_String_لمائی=پہلا_Element.get_String_لمائی(لڑیاں);
    طالما(++unit_اشاریہ<min_String_لمائی &&
            پہلا_Element.charAt(unit_اشاریہ, لڑیاں)==
            آخری_Element.charAt(unit_اشاریہ, لڑیاں)) {}
    أرجع unit_اشاریہ;
}

int32_t
UCharsTrieBuilder::گنتی_Element_Units(int32_t شروع, int32_t حد_بندی, int32_t unit_اشاریہ) ثابت {
    int32_t length=0;  // Number of different units at unitIndex.
    int32_t i=شروع;
    افعل {
        char16_t unit=elements[i++].charAt(unit_اشاریہ, لڑیاں);
        طالما(i<حد_بندی && unit==elements[i].charAt(unit_اشاریہ, لڑیاں)) {
            ++i;
        }
        ++length;
    } طالما(i<حد_بندی);
    أرجع length;
}

int32_t
UCharsTrieBuilder::skipElementsBySomeUnits(int32_t i, int32_t unit_اشاریہ, int32_t count) ثابت {
    افعل {
        char16_t unit=elements[i++].charAt(unit_اشاریہ, لڑیاں);
        طالما(unit==elements[i].charAt(unit_اشاریہ, لڑیاں)) {
            ++i;
        }
    } طالما(--count>0);
    أرجع i;
}

int32_t
UCharsTrieBuilder::اشاریہ_دا_Element_With_اگلا_Unit(int32_t i, int32_t unit_اشاریہ, char16_t unit) ثابت {
    طالما(unit==elements[i].charAt(unit_اشاریہ, لڑیاں)) {
        ++i;
    }
    أرجع i;
}

UCharsTrieBuilder::UCTLinear_میل_گرہ::UCTLinear_میل_گرہ(ثابت char16_t *units, int32_t len, Node *اگلا_گرہ)
        : خطی_میل_گرہ(len, اگلا_گرہ), s(units) {
    hash=hash*37u+ustr_hashUCharsN(units, len);
}

منطقي
UCharsTrieBuilder::UCTLinear_میل_گرہ::عامل==(ثابت Node &other) ثابت {
    إذا(هذا==&other) {
        أرجع سچ;
    }
    إذا(!خطی_میل_گرہ::عامل==(other)) {
        أرجع خطأ;
    }
    ثابت UCTLinear_میل_گرہ &o=static_cast<ثابت UCTLinear_میل_گرہ &>(other);
    أرجع 0==u_memcmp(s, o.s, length);
}

فراغ
UCharsTrieBuilder::UCTLinear_میل_گرہ::اكتب(StringTrieBuilder &builder) {
    UCharsTrieBuilder &b=(UCharsTrieBuilder &)builder;
    next->اكتب(builder);
    b.اكتب(s, length);
    وتھ=b.write_قدر_And_Type(has_قدر, value, b.get_Min_خطی_میل()+length-1);
}

StringTrieBuilder::Node *
UCharsTrieBuilder::create_خطی_میل_گرہ(int32_t i, int32_t unit_اشاریہ, int32_t length,
                                         Node *اگلا_گرہ) ثابت {
    أرجع جديد UCTLinear_میل_گرہ(
            elements[i].getString(لڑیاں).get_بفر()+unit_اشاریہ,
            length,
            اگلا_گرہ);
}

UBool
UCharsTrieBuilder::ensure_سمرتھا(int32_t length) {
    إذا(uchars==خالی_سوچک) {
        أرجع خطأ;  // previous memory allocation had failed
    }
    إذا(length>uchars_سمرتھا) {
        int32_t new_سمرتھا=uchars_سمرتھا;
        افعل {
            new_سمرتھا*=2;
        } طالما(new_سمرتھا<=length);
        char16_t *newUChars=static_cast<char16_t *>(uprv_malloc(new_سمرتھا*2));
        إذا(newUChars==خالی_سوچک) {
            // unable to allocate memory
            uprv_free(uchars);
            uchars=خالی_سوچک;
            uchars_سمرتھا=0;
            أرجع خطأ;
        }
        u_memcpy(newUChars+(new_سمرتھا-uchars_لمائی),
                 uchars+(uchars_سمرتھا-uchars_لمائی), uchars_لمائی);
        uprv_free(uchars);
        uchars=newUChars;
        uchars_سمرتھا=new_سمرتھا;
    }
    أرجع سچ;
}

int32_t
UCharsTrieBuilder::اكتب(int32_t unit) {
    int32_t new_لمائی=uchars_لمائی+1;
    إذا(ensure_سمرتھا(new_لمائی)) {
        uchars_لمائی=new_لمائی;
        uchars[uchars_سمرتھا - uchars_لمائی] = static_cast<char16_t>(unit);
    }
    أرجع uchars_لمائی;
}

int32_t
UCharsTrieBuilder::اكتب(ثابت char16_t *s, int32_t length) {
    int32_t new_لمائی=uchars_لمائی+length;
    إذا(ensure_سمرتھا(new_لمائی)) {
        uchars_لمائی=new_لمائی;
        u_memcpy(uchars+(uchars_سمرتھا-uchars_لمائی), s, length);
    }
    أرجع uchars_لمائی;
}

int32_t
UCharsTrieBuilder::writeElementUnits(int32_t i, int32_t unit_اشاریہ, int32_t length) {
    أرجع اكتب(elements[i].getString(لڑیاں).get_بفر()+unit_اشاریہ, length);
}

int32_t
UCharsTrieBuilder::write_قدر_And_Final(int32_t i, UBool isFinal) {
    إذا(0<=i && i<=UCharsTrie::k_Max_One_Unit_قدر) {
        أرجع اكتب(i|(isFinal<<15));
    }
    char16_t intUnits[3];
    int32_t length;
    إذا(i<0 || i>UCharsTrie::k_Max_Two_Unit_قدر) {
        intUnits[0] = static_cast<char16_t>(UCharsTrie::k_Three_Unit_قدر_Lead);
        intUnits[1] = static_cast<char16_t>(static_cast<uint32_t>(i) >> 16);
        intUnits[2] = static_cast<char16_t>(i);
        length=3;
    // } else if(i<=UCharsTrie::kMaxOneUnitValue) {
    //     intUnits[0]=(char16_t)(i);
    //     length=1;
    } وإلا {
        intUnits[0] = static_cast<char16_t>(UCharsTrie::k_Min_Two_Unit_قدر_Lead + (i >> 16));
        intUnits[1] = static_cast<char16_t>(i);
        length=2;
    }
    intUnits[0] = static_cast<char16_t>(intUnits[0] | (isFinal << 15));
    أرجع اكتب(intUnits, length);
}

int32_t
UCharsTrieBuilder::write_قدر_And_Type(UBool has_قدر, int32_t value, int32_t گرہ) {
    إذا(!has_قدر) {
        أرجع اكتب(گرہ);
    }
    char16_t intUnits[3];
    int32_t length;
    إذا(value<0 || value>UCharsTrie::k_Max_Two_Unit_گرہ_قدر) {
        intUnits[0] = static_cast<char16_t>(UCharsTrie::k_Three_Unit_گرہ_قدر_Lead);
        intUnits[1] = static_cast<char16_t>(static_cast<uint32_t>(value) >> 16);
        intUnits[2] = static_cast<char16_t>(value);
        length=3;
    } وإلا إذا(value<=UCharsTrie::k_Max_One_Unit_گرہ_قدر) {
        intUnits[0] = static_cast<char16_t>((value + 1) << 6);
        length=1;
    } وإلا {
        intUnits[0] = static_cast<char16_t>(UCharsTrie::k_Min_Two_Unit_گرہ_قدر_Lead + ((value >> 10) & 0x7fc0));
        intUnits[1] = static_cast<char16_t>(value);
        length=2;
    }
    intUnits[0] |= static_cast<char16_t>(گرہ);
    أرجع اكتب(intUnits, length);
}

int32_t
UCharsTrieBuilder::write_Delta_منزل(int32_t jump_نشانہ) {
    int32_t i=uchars_لمائی-jump_نشانہ;
    U_ASSERT(i>=0);
    إذا(i<=UCharsTrie::kMaxOneUnitDelta) {
        أرجع اكتب(i);
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
    أرجع اكتب(intUnits, length);
}

U_NAMESPACE_سرا
