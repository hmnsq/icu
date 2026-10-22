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

    فراغ سيٽ_منزل(ثابت UnicodeString &s, int32_t val, UnicodeString &لڙيون, UError_ڪوڊ &error_ڪوڊ);

    UnicodeString getString(ثابت UnicodeString& لڙيون U_LIFETIME_BOUND) ثابت {
        int32_t length=لڙيون[string_وٿي];
        أرجع لڙيون.عارضي_Sub_String(string_وٿي+1, length);
    }
    int32_t get_String_ڊيگهائي(ثابت UnicodeString &لڙيون) ثابت {
        أرجع لڙيون[string_وٿي];
    }

    char16_t charAt(int32_t اشاريو, ثابت UnicodeString &لڙيون) ثابت {
        أرجع لڙيون[string_وٿي+1+اشاريو];
    }

    int32_t get_قدر() ثابت { أرجع value; }

    int32_t compare_String_منزل(ثابت UCharsTrieElement &o, ثابت UnicodeString &لڙيون) ثابت;

خاص:
    // The first strings unit contains the string length.
    // (Compared with a stringLength field here, this saves 2 bytes per string.)
    int32_t string_وٿي;
    int32_t value;
};

فراغ
UCharsTrieElement::سيٽ_منزل(ثابت UnicodeString &s, int32_t val,
                         UnicodeString &لڙيون, UError_ڪوڊ &error_ڪوڊ) {
    إذا(U_FAILURE(error_ڪوڊ)) {
        أرجع;
    }
    int32_t length=s.length();
    إذا(length>0xffff) {
        // Too long: We store the length in 1 unit.
        error_ڪوڊ=U_اشاريو_OUTOFBOUNDS_ERROR;
        أرجع;
    }
    string_وٿي=لڙيون.length();
    لڙيون.append(static_cast<char16_t>(length));
    value=val;
    لڙيون.append(s);
}

int32_t
UCharsTrieElement::compare_String_منزل(ثابت UCharsTrieElement &other, ثابت UnicodeString &لڙيون) ثابت {
    أرجع getString(لڙيون).compare(other.getString(لڙيون));
}

UCharsTrieBuilder::UCharsTrieBuilder(UError_ڪوڊ & /*errorCode*/)
        : elements(خالي_اشارو), elements_صلاحيت(0), elements_ڊيگهائي(0),
          uchars(خالي_اشارو), uchars_صلاحيت(0), uchars_ڊيگهائي(0) {}

UCharsTrieBuilder::~UCharsTrieBuilder() {
    ميٽيو[] elements;
    uprv_free(uchars);
}

UCharsTrieBuilder&
UCharsTrieBuilder::جوڙيو(ثابت UnicodeString& s,
                       int32_t value,
                       UError_ڪوڊ& error_ڪوڊ) U_LIFETIME_BOUND {
    إذا(U_FAILURE(error_ڪوڊ)) {
        أرجع *هذا;
    }
    إذا(uchars_ڊيگهائي>0) {
        // Cannot add elements after building.
        error_ڪوڊ=U_NO_WRITE_PERMISSION;
        أرجع *هذا;
    }
    إذا(elements_ڊيگهائي==elements_صلاحيت) {
        int32_t new_صلاحيت;
        إذا(elements_صلاحيت==0) {
            new_صلاحيت=1024;
        } وإلا {
            new_صلاحيت=4*elements_صلاحيت;
        }
        UCharsTrieElement *newElements=جديد UCharsTrieElement[new_صلاحيت];
        إذا(newElements==خالي_اشارو) {
            error_ڪوڊ=U_يادگيري_ورهاست_ERROR;
            أرجع *هذا;
        }
        إذا(elements_ڊيگهائي>0) {
            uprv_memcpy(newElements, elements, (size_t)elements_ڊيگهائي*حجم(UCharsTrieElement));
        }
        ميٽيو[] elements;
        elements=newElements;
        elements_صلاحيت=new_صلاحيت;
    }
    elements[elements_ڊيگهائي++].سيٽ_منزل(s, value, لڙيون, error_ڪوڊ);
    إذا(U_SUCCESS(error_ڪوڊ) && لڙيون.isBogus()) {
        error_ڪوڊ=U_يادگيري_ورهاست_ERROR;
    }
    أرجع *هذا;
}

U_CDECL_BEGIN

ساكن int32_t U_CALLCONV
compare_Element_لڙيون(ثابت فراغ *context, ثابت فراغ *کاٻي, ثابت فراغ *ساڄي) {
    ثابت UnicodeString *لڙيون=static_cast<ثابت UnicodeString *>(context);
    ثابت UCharsTrieElement *کاٻو_Element=static_cast<ثابت UCharsTrieElement *>(کاٻي);
    ثابت UCharsTrieElement *ساڄو_Element=static_cast<ثابت UCharsTrieElement *>(ساڄي);
    أرجع کاٻو_Element->compare_String_منزل(*ساڄو_Element, *لڙيون);
}

U_CDECL_پڇاڙي

UCharsTrie *
UCharsTrieBuilder::build(UStringTrieBuildOption buildOption, UError_ڪوڊ &error_ڪوڊ) {
    buildUChars(buildOption, error_ڪوڊ);
    UCharsTrie *newTrie=خالي_اشارو;
    إذا(U_SUCCESS(error_ڪوڊ)) {
        newTrie=جديد UCharsTrie(uchars, uchars+(uchars_صلاحيت-uchars_ڊيگهائي));
        إذا(newTrie==خالي_اشارو) {
            error_ڪوڊ=U_يادگيري_ورهاست_ERROR;
        } وإلا {
            uchars=خالي_اشارو;  // The new trie now owns the array.
            uchars_صلاحيت=0;
        }
    }
    أرجع newTrie;
}

UnicodeString&
UCharsTrieBuilder::buildUnicodeString(UStringTrieBuildOption buildOption,
                                      UnicodeString& نتيجو U_LIFETIME_BOUND,
                                      UError_ڪوڊ& error_ڪوڊ) U_LIFETIME_BOUND {
    buildUChars(buildOption, error_ڪوڊ);
    إذا(U_SUCCESS(error_ڪوڊ)) {
        نتيجو.سيٽ_منزل(خطأ, uchars+(uchars_صلاحيت-uchars_ڊيگهائي), uchars_ڊيگهائي);
    }
    أرجع نتيجو;
}

فراغ
UCharsTrieBuilder::buildUChars(UStringTrieBuildOption buildOption, UError_ڪوڊ &error_ڪوڊ) {
    إذا(U_FAILURE(error_ڪوڊ)) {
        أرجع;
    }
    إذا(uchars!=خالي_اشارو && uchars_ڊيگهائي>0) {
        // Already built.
        أرجع;
    }
    إذا(uchars_ڊيگهائي==0) {
        إذا(elements_ڊيگهائي==0) {
            error_ڪوڊ=U_اشاريو_OUTOFBOUNDS_ERROR;
            أرجع;
        }
        إذا(لڙيون.isBogus()) {
            error_ڪوڊ=U_يادگيري_ورهاست_ERROR;
            أرجع;
        }
        uprv_ترتيب_ڏيو_Array(elements, elements_ڊيگهائي, static_cast<int32_t>(حجم(UCharsTrieElement)),
                      compare_Element_لڙيون, &لڙيون,
                      خطأ,  // need not be a stable sort
                      &error_ڪوڊ);
        إذا(U_FAILURE(error_ڪوڊ)) {
            أرجع;
        }
        // Duplicate strings are not allowed.
        UnicodeString prev=elements[0].getString(لڙيون);
        لكل(int32_t i=1; i<elements_ڊيگهائي; ++i) {
            UnicodeString current=elements[i].getString(لڙيون);
            إذا(prev==current) {
                error_ڪوڊ=U_ILLEGAL_دليل_ERROR;
                أرجع;
            }
            prev.fastCopyFrom(current);
        }
    }
    // Create and char16_t-serialize the trie for the elements.
    uchars_ڊيگهائي=0;
    int32_t capacity=لڙيون.length();
    إذا(capacity<1024) {
        capacity=1024;
    }
    إذا(uchars_صلاحيت<capacity) {
        uprv_free(uchars);
        uchars=static_cast<char16_t *>(uprv_malloc(capacity*2));
        إذا(uchars==خالي_اشارو) {
            error_ڪوڊ=U_يادگيري_ورهاست_ERROR;
            uchars_صلاحيت=0;
            أرجع;
        }
        uchars_صلاحيت=capacity;
    }
    StringTrieBuilder::build(buildOption, elements_ڊيگهائي, error_ڪوڊ);
    إذا(uchars==خالي_اشارو) {
        error_ڪوڊ=U_يادگيري_ورهاست_ERROR;
    }
}

int32_t
UCharsTrieBuilder::get_Element_String_ڊيگهائي(int32_t i) ثابت {
    أرجع elements[i].get_String_ڊيگهائي(لڙيون);
}

char16_t
UCharsTrieBuilder::getElementUnit(int32_t i, int32_t unit_اشاريو) ثابت {
    أرجع elements[i].charAt(unit_اشاريو, لڙيون);
}

int32_t
UCharsTrieBuilder::get_Element_قدر(int32_t i) ثابت {
    أرجع elements[i].get_قدر();
}

int32_t
UCharsTrieBuilder::get_حد_بندي_جو_لڪيري_ميلاپ(int32_t first, int32_t آخري, int32_t unit_اشاريو) ثابت {
    ثابت UCharsTrieElement &پهريون_Element=elements[first];
    ثابت UCharsTrieElement &آخري_Element=elements[آخري];
    int32_t min_String_ڊيگهائي=پهريون_Element.get_String_ڊيگهائي(لڙيون);
    طالما(++unit_اشاريو<min_String_ڊيگهائي &&
            پهريون_Element.charAt(unit_اشاريو, لڙيون)==
            آخري_Element.charAt(unit_اشاريو, لڙيون)) {}
    أرجع unit_اشاريو;
}

int32_t
UCharsTrieBuilder::ڳڻپ_Element_Units(int32_t شروعات, int32_t حد_بندي, int32_t unit_اشاريو) ثابت {
    int32_t length=0;  // Number of different units at unitIndex.
    int32_t i=شروعات;
    افعل {
        char16_t unit=elements[i++].charAt(unit_اشاريو, لڙيون);
        طالما(i<حد_بندي && unit==elements[i].charAt(unit_اشاريو, لڙيون)) {
            ++i;
        }
        ++length;
    } طالما(i<حد_بندي);
    أرجع length;
}

int32_t
UCharsTrieBuilder::skipElementsBySomeUnits(int32_t i, int32_t unit_اشاريو, int32_t count) ثابت {
    افعل {
        char16_t unit=elements[i++].charAt(unit_اشاريو, لڙيون);
        طالما(unit==elements[i].charAt(unit_اشاريو, لڙيون)) {
            ++i;
        }
    } طالما(--count>0);
    أرجع i;
}

int32_t
UCharsTrieBuilder::اشاريو_جو_Element_With_ايندڙ_Unit(int32_t i, int32_t unit_اشاريو, char16_t unit) ثابت {
    طالما(unit==elements[i].charAt(unit_اشاريو, لڙيون)) {
        ++i;
    }
    أرجع i;
}

UCharsTrieBuilder::UCTLinear_ميلاپ_ڳنڍ::UCTLinear_ميلاپ_ڳنڍ(ثابت char16_t *units, int32_t len, Node *ايندڙ_ڳنڍ)
        : لڪيري_ميلاپ_ڳنڍ(len, ايندڙ_ڳنڍ), s(units) {
    hash=hash*37u+ustr_hashUCharsN(units, len);
}

منطقي
UCharsTrieBuilder::UCTLinear_ميلاپ_ڳنڍ::عامل==(ثابت Node &other) ثابت {
    إذا(هذا==&other) {
        أرجع سچ;
    }
    إذا(!لڪيري_ميلاپ_ڳنڍ::عامل==(other)) {
        أرجع خطأ;
    }
    ثابت UCTLinear_ميلاپ_ڳنڍ &o=static_cast<ثابت UCTLinear_ميلاپ_ڳنڍ &>(other);
    أرجع 0==u_memcmp(s, o.s, length);
}

فراغ
UCharsTrieBuilder::UCTLinear_ميلاپ_ڳنڍ::اكتب(StringTrieBuilder &builder) {
    UCharsTrieBuilder &b=(UCharsTrieBuilder &)builder;
    next->اكتب(builder);
    b.اكتب(s, length);
    وٿي=b.write_قدر_And_Type(has_قدر, value, b.get_Min_لڪيري_ميلاپ()+length-1);
}

StringTrieBuilder::Node *
UCharsTrieBuilder::create_لڪيري_ميلاپ_ڳنڍ(int32_t i, int32_t unit_اشاريو, int32_t length,
                                         Node *ايندڙ_ڳنڍ) ثابت {
    أرجع جديد UCTLinear_ميلاپ_ڳنڍ(
            elements[i].getString(لڙيون).get_بفر()+unit_اشاريو,
            length,
            ايندڙ_ڳنڍ);
}

UBool
UCharsTrieBuilder::ensure_صلاحيت(int32_t length) {
    إذا(uchars==خالي_اشارو) {
        أرجع خطأ;  // previous memory allocation had failed
    }
    إذا(length>uchars_صلاحيت) {
        int32_t new_صلاحيت=uchars_صلاحيت;
        افعل {
            new_صلاحيت*=2;
        } طالما(new_صلاحيت<=length);
        char16_t *newUChars=static_cast<char16_t *>(uprv_malloc(new_صلاحيت*2));
        إذا(newUChars==خالي_اشارو) {
            // unable to allocate memory
            uprv_free(uchars);
            uchars=خالي_اشارو;
            uchars_صلاحيت=0;
            أرجع خطأ;
        }
        u_memcpy(newUChars+(new_صلاحيت-uchars_ڊيگهائي),
                 uchars+(uchars_صلاحيت-uchars_ڊيگهائي), uchars_ڊيگهائي);
        uprv_free(uchars);
        uchars=newUChars;
        uchars_صلاحيت=new_صلاحيت;
    }
    أرجع سچ;
}

int32_t
UCharsTrieBuilder::اكتب(int32_t unit) {
    int32_t new_ڊيگهائي=uchars_ڊيگهائي+1;
    إذا(ensure_صلاحيت(new_ڊيگهائي)) {
        uchars_ڊيگهائي=new_ڊيگهائي;
        uchars[uchars_صلاحيت - uchars_ڊيگهائي] = static_cast<char16_t>(unit);
    }
    أرجع uchars_ڊيگهائي;
}

int32_t
UCharsTrieBuilder::اكتب(ثابت char16_t *s, int32_t length) {
    int32_t new_ڊيگهائي=uchars_ڊيگهائي+length;
    إذا(ensure_صلاحيت(new_ڊيگهائي)) {
        uchars_ڊيگهائي=new_ڊيگهائي;
        u_memcpy(uchars+(uchars_صلاحيت-uchars_ڊيگهائي), s, length);
    }
    أرجع uchars_ڊيگهائي;
}

int32_t
UCharsTrieBuilder::writeElementUnits(int32_t i, int32_t unit_اشاريو, int32_t length) {
    أرجع اكتب(elements[i].getString(لڙيون).get_بفر()+unit_اشاريو, length);
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
UCharsTrieBuilder::write_قدر_And_Type(UBool has_قدر, int32_t value, int32_t ڳنڍ) {
    إذا(!has_قدر) {
        أرجع اكتب(ڳنڍ);
    }
    char16_t intUnits[3];
    int32_t length;
    إذا(value<0 || value>UCharsTrie::k_Max_Two_Unit_ڳنڍ_قدر) {
        intUnits[0] = static_cast<char16_t>(UCharsTrie::k_Three_Unit_ڳنڍ_قدر_Lead);
        intUnits[1] = static_cast<char16_t>(static_cast<uint32_t>(value) >> 16);
        intUnits[2] = static_cast<char16_t>(value);
        length=3;
    } وإلا إذا(value<=UCharsTrie::k_Max_One_Unit_ڳنڍ_قدر) {
        intUnits[0] = static_cast<char16_t>((value + 1) << 6);
        length=1;
    } وإلا {
        intUnits[0] = static_cast<char16_t>(UCharsTrie::k_Min_Two_Unit_ڳنڍ_قدر_Lead + ((value >> 10) & 0x7fc0));
        intUnits[1] = static_cast<char16_t>(value);
        length=2;
    }
    intUnits[0] |= static_cast<char16_t>(ڳنڍ);
    أرجع اكتب(intUnits, length);
}

int32_t
UCharsTrieBuilder::write_Delta_منزل(int32_t jump_هدف) {
    int32_t i=uchars_ڊيگهائي-jump_هدف;
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

U_NAMESPACE_پڇاڙي
