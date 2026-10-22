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

    فراغ سېټ_منزل(ثابت UnicodeString &s, int32_t val, UnicodeString &مزي_ګانې, UError_کوډ &error_کوډ);

    UnicodeString getString(ثابت UnicodeString& مزي_ګانې U_LIFETIME_BOUND) ثابت {
        int32_t length=مزي_ګانې[string_دننه_واټن];
        أرجع مزي_ګانې.لنډمهاله_Sub_String(string_دننه_واټن+1, length);
    }
    int32_t get_String_اوږدتیا(ثابت UnicodeString &مزي_ګانې) ثابت {
        أرجع مزي_ګانې[string_دننه_واټن];
    }

    char16_t charAt(int32_t شاخص, ثابت UnicodeString &مزي_ګانې) ثابت {
        أرجع مزي_ګانې[string_دننه_واټن+1+شاخص];
    }

    int32_t get_ارزښت() ثابت { أرجع value; }

    int32_t compare_String_منزل(ثابت UCharsTrieElement &o, ثابت UnicodeString &مزي_ګانې) ثابت;

خاص:
    // The first strings unit contains the string length.
    // (Compared with a stringLength field here, this saves 2 bytes per string.)
    int32_t string_دننه_واټن;
    int32_t value;
};

فراغ
UCharsTrieElement::سېټ_منزل(ثابت UnicodeString &s, int32_t val,
                         UnicodeString &مزي_ګانې, UError_کوډ &error_کوډ) {
    إذا(U_FAILURE(error_کوډ)) {
        أرجع;
    }
    int32_t length=s.length();
    إذا(length>0xffff) {
        // Too long: We store the length in 1 unit.
        error_کوډ=U_شاخص_OUTOFBOUNDS_ERROR;
        أرجع;
    }
    string_دننه_واټن=مزي_ګانې.length();
    مزي_ګانې.append(static_cast<char16_t>(length));
    value=val;
    مزي_ګانې.append(s);
}

int32_t
UCharsTrieElement::compare_String_منزل(ثابت UCharsTrieElement &other, ثابت UnicodeString &مزي_ګانې) ثابت {
    أرجع getString(مزي_ګانې).compare(other.getString(مزي_ګانې));
}

UCharsTrieBuilder::UCharsTrieBuilder(UError_کوډ & /*errorCode*/)
        : elements(تش_اشاره), elements_ظرفیت(0), elements_اوږدتیا(0),
          uchars(تش_اشاره), uchars_ظرفیت(0), uchars_اوږدتیا(0) {}

UCharsTrieBuilder::~UCharsTrieBuilder() {
    ړنګول[] elements;
    uprv_free(uchars);
}

UCharsTrieBuilder&
UCharsTrieBuilder::جمع_کړه(ثابت UnicodeString& s,
                       int32_t value,
                       UError_کوډ& error_کوډ) U_LIFETIME_BOUND {
    إذا(U_FAILURE(error_کوډ)) {
        أرجع *هذا;
    }
    إذا(uchars_اوږدتیا>0) {
        // Cannot add elements after building.
        error_کوډ=U_NO_WRITE_PERMISSION;
        أرجع *هذا;
    }
    إذا(elements_اوږدتیا==elements_ظرفیت) {
        int32_t new_ظرفیت;
        إذا(elements_ظرفیت==0) {
            new_ظرفیت=1024;
        } وإلا {
            new_ظرفیت=4*elements_ظرفیت;
        }
        UCharsTrieElement *newElements=جديد UCharsTrieElement[new_ظرفیت];
        إذا(newElements==تش_اشاره) {
            error_کوډ=U_حافظه_ویش_ERROR;
            أرجع *هذا;
        }
        إذا(elements_اوږدتیا>0) {
            uprv_memcpy(newElements, elements, (size_t)elements_اوږدتیا*حجم(UCharsTrieElement));
        }
        ړنګول[] elements;
        elements=newElements;
        elements_ظرفیت=new_ظرفیت;
    }
    elements[elements_اوږدتیا++].سېټ_منزل(s, value, مزي_ګانې, error_کوډ);
    إذا(U_SUCCESS(error_کوډ) && مزي_ګانې.isBogus()) {
        error_کوډ=U_حافظه_ویش_ERROR;
    }
    أرجع *هذا;
}

U_CDECL_BEGIN

ساكن int32_t U_CALLCONV
compare_Element_مزي_ګانې(ثابت فراغ *context, ثابت فراغ *کیڼ, ثابت فراغ *ښی) {
    ثابت UnicodeString *مزي_ګانې=static_cast<ثابت UnicodeString *>(context);
    ثابت UCharsTrieElement *کیڼ_Element=static_cast<ثابت UCharsTrieElement *>(کیڼ);
    ثابت UCharsTrieElement *ښی_Element=static_cast<ثابت UCharsTrieElement *>(ښی);
    أرجع کیڼ_Element->compare_String_منزل(*ښی_Element, *مزي_ګانې);
}

U_CDECL_پای_ټکی

UCharsTrie *
UCharsTrieBuilder::build(UStringTrieBuildOption buildOption, UError_کوډ &error_کوډ) {
    buildUChars(buildOption, error_کوډ);
    UCharsTrie *newTrie=تش_اشاره;
    إذا(U_SUCCESS(error_کوډ)) {
        newTrie=جديد UCharsTrie(uchars, uchars+(uchars_ظرفیت-uchars_اوږدتیا));
        إذا(newTrie==تش_اشاره) {
            error_کوډ=U_حافظه_ویش_ERROR;
        } وإلا {
            uchars=تش_اشاره;  // The new trie now owns the array.
            uchars_ظرفیت=0;
        }
    }
    أرجع newTrie;
}

UnicodeString&
UCharsTrieBuilder::buildUnicodeString(UStringTrieBuildOption buildOption,
                                      UnicodeString& پایله U_LIFETIME_BOUND,
                                      UError_کوډ& error_کوډ) U_LIFETIME_BOUND {
    buildUChars(buildOption, error_کوډ);
    إذا(U_SUCCESS(error_کوډ)) {
        پایله.سېټ_منزل(خطأ, uchars+(uchars_ظرفیت-uchars_اوږدتیا), uchars_اوږدتیا);
    }
    أرجع پایله;
}

فراغ
UCharsTrieBuilder::buildUChars(UStringTrieBuildOption buildOption, UError_کوډ &error_کوډ) {
    إذا(U_FAILURE(error_کوډ)) {
        أرجع;
    }
    إذا(uchars!=تش_اشاره && uchars_اوږدتیا>0) {
        // Already built.
        أرجع;
    }
    إذا(uchars_اوږدتیا==0) {
        إذا(elements_اوږدتیا==0) {
            error_کوډ=U_شاخص_OUTOFBOUNDS_ERROR;
            أرجع;
        }
        إذا(مزي_ګانې.isBogus()) {
            error_کوډ=U_حافظه_ویش_ERROR;
            أرجع;
        }
        uprv_ترتیبول_Array(elements, elements_اوږدتیا, static_cast<int32_t>(حجم(UCharsTrieElement)),
                      compare_Element_مزي_ګانې, &مزي_ګانې,
                      خطأ,  // need not be a stable sort
                      &error_کوډ);
        إذا(U_FAILURE(error_کوډ)) {
            أرجع;
        }
        // Duplicate strings are not allowed.
        UnicodeString prev=elements[0].getString(مزي_ګانې);
        لكل(int32_t i=1; i<elements_اوږدتیا; ++i) {
            UnicodeString current=elements[i].getString(مزي_ګانې);
            إذا(prev==current) {
                error_کوډ=U_ILLEGAL_دلیل_ERROR;
                أرجع;
            }
            prev.fastCopyFrom(current);
        }
    }
    // Create and char16_t-serialize the trie for the elements.
    uchars_اوږدتیا=0;
    int32_t capacity=مزي_ګانې.length();
    إذا(capacity<1024) {
        capacity=1024;
    }
    إذا(uchars_ظرفیت<capacity) {
        uprv_free(uchars);
        uchars=static_cast<char16_t *>(uprv_malloc(capacity*2));
        إذا(uchars==تش_اشاره) {
            error_کوډ=U_حافظه_ویش_ERROR;
            uchars_ظرفیت=0;
            أرجع;
        }
        uchars_ظرفیت=capacity;
    }
    StringTrieBuilder::build(buildOption, elements_اوږدتیا, error_کوډ);
    إذا(uchars==تش_اشاره) {
        error_کوډ=U_حافظه_ویش_ERROR;
    }
}

int32_t
UCharsTrieBuilder::get_Element_String_اوږدتیا(int32_t i) ثابت {
    أرجع elements[i].get_String_اوږدتیا(مزي_ګانې);
}

char16_t
UCharsTrieBuilder::getElementUnit(int32_t i, int32_t unit_شاخص) ثابت {
    أرجع elements[i].charAt(unit_شاخص, مزي_ګانې);
}

int32_t
UCharsTrieBuilder::get_Element_ارزښت(int32_t i) ثابت {
    أرجع elements[i].get_ارزښت();
}

int32_t
UCharsTrieBuilder::get_حد_د_خطي_سمون(int32_t first, int32_t وروستی, int32_t unit_شاخص) ثابت {
    ثابت UCharsTrieElement &لومړی_Element=elements[first];
    ثابت UCharsTrieElement &وروستی_Element=elements[وروستی];
    int32_t min_String_اوږدتیا=لومړی_Element.get_String_اوږدتیا(مزي_ګانې);
    طالما(++unit_شاخص<min_String_اوږدتیا &&
            لومړی_Element.charAt(unit_شاخص, مزي_ګانې)==
            وروستی_Element.charAt(unit_شاخص, مزي_ګانې)) {}
    أرجع unit_شاخص;
}

int32_t
UCharsTrieBuilder::شمېر_Element_Units(int32_t پیل, int32_t حد, int32_t unit_شاخص) ثابت {
    int32_t length=0;  // Number of different units at unitIndex.
    int32_t i=پیل;
    افعل {
        char16_t unit=elements[i++].charAt(unit_شاخص, مزي_ګانې);
        طالما(i<حد && unit==elements[i].charAt(unit_شاخص, مزي_ګانې)) {
            ++i;
        }
        ++length;
    } طالما(i<حد);
    أرجع length;
}

int32_t
UCharsTrieBuilder::skipElementsBySomeUnits(int32_t i, int32_t unit_شاخص, int32_t count) ثابت {
    افعل {
        char16_t unit=elements[i++].charAt(unit_شاخص, مزي_ګانې);
        طالما(unit==elements[i].charAt(unit_شاخص, مزي_ګانې)) {
            ++i;
        }
    } طالما(--count>0);
    أرجع i;
}

int32_t
UCharsTrieBuilder::شاخص_د_Element_With_بل_Unit(int32_t i, int32_t unit_شاخص, char16_t unit) ثابت {
    طالما(unit==elements[i].charAt(unit_شاخص, مزي_ګانې)) {
        ++i;
    }
    أرجع i;
}

UCharsTrieBuilder::UCTLinear_سمون_غوټه::UCTLinear_سمون_غوټه(ثابت char16_t *units, int32_t len, Node *بل_غوټه)
        : خطي_سمون_غوټه(len, بل_غوټه), s(units) {
    hash=hash*37u+ustr_hashUCharsN(units, len);
}

منطقي
UCharsTrieBuilder::UCTLinear_سمون_غوټه::چلوونکی==(ثابت Node &other) ثابت {
    إذا(هذا==&other) {
        أرجع ریښتیا;
    }
    إذا(!خطي_سمون_غوټه::چلوونکی==(other)) {
        أرجع خطأ;
    }
    ثابت UCTLinear_سمون_غوټه &o=static_cast<ثابت UCTLinear_سمون_غوټه &>(other);
    أرجع 0==u_memcmp(s, o.s, length);
}

فراغ
UCharsTrieBuilder::UCTLinear_سمون_غوټه::اكتب(StringTrieBuilder &builder) {
    UCharsTrieBuilder &b=(UCharsTrieBuilder &)builder;
    next->اكتب(builder);
    b.اكتب(s, length);
    دننه_واټن=b.write_ارزښت_And_Type(has_ارزښت, value, b.get_Min_خطي_سمون()+length-1);
}

StringTrieBuilder::Node *
UCharsTrieBuilder::create_خطي_سمون_غوټه(int32_t i, int32_t unit_شاخص, int32_t length,
                                         Node *بل_غوټه) ثابت {
    أرجع جديد UCTLinear_سمون_غوټه(
            elements[i].getString(مزي_ګانې).get_بفر()+unit_شاخص,
            length,
            بل_غوټه);
}

UBool
UCharsTrieBuilder::ensure_ظرفیت(int32_t length) {
    إذا(uchars==تش_اشاره) {
        أرجع خطأ;  // previous memory allocation had failed
    }
    إذا(length>uchars_ظرفیت) {
        int32_t new_ظرفیت=uchars_ظرفیت;
        افعل {
            new_ظرفیت*=2;
        } طالما(new_ظرفیت<=length);
        char16_t *newUChars=static_cast<char16_t *>(uprv_malloc(new_ظرفیت*2));
        إذا(newUChars==تش_اشاره) {
            // unable to allocate memory
            uprv_free(uchars);
            uchars=تش_اشاره;
            uchars_ظرفیت=0;
            أرجع خطأ;
        }
        u_memcpy(newUChars+(new_ظرفیت-uchars_اوږدتیا),
                 uchars+(uchars_ظرفیت-uchars_اوږدتیا), uchars_اوږدتیا);
        uprv_free(uchars);
        uchars=newUChars;
        uchars_ظرفیت=new_ظرفیت;
    }
    أرجع ریښتیا;
}

int32_t
UCharsTrieBuilder::اكتب(int32_t unit) {
    int32_t new_اوږدتیا=uchars_اوږدتیا+1;
    إذا(ensure_ظرفیت(new_اوږدتیا)) {
        uchars_اوږدتیا=new_اوږدتیا;
        uchars[uchars_ظرفیت - uchars_اوږدتیا] = static_cast<char16_t>(unit);
    }
    أرجع uchars_اوږدتیا;
}

int32_t
UCharsTrieBuilder::اكتب(ثابت char16_t *s, int32_t length) {
    int32_t new_اوږدتیا=uchars_اوږدتیا+length;
    إذا(ensure_ظرفیت(new_اوږدتیا)) {
        uchars_اوږدتیا=new_اوږدتیا;
        u_memcpy(uchars+(uchars_ظرفیت-uchars_اوږدتیا), s, length);
    }
    أرجع uchars_اوږدتیا;
}

int32_t
UCharsTrieBuilder::writeElementUnits(int32_t i, int32_t unit_شاخص, int32_t length) {
    أرجع اكتب(elements[i].getString(مزي_ګانې).get_بفر()+unit_شاخص, length);
}

int32_t
UCharsTrieBuilder::write_ارزښت_And_Final(int32_t i, UBool isFinal) {
    إذا(0<=i && i<=UCharsTrie::k_Max_One_Unit_ارزښت) {
        أرجع اكتب(i|(isFinal<<15));
    }
    char16_t intUnits[3];
    int32_t length;
    إذا(i<0 || i>UCharsTrie::k_Max_Two_Unit_ارزښت) {
        intUnits[0] = static_cast<char16_t>(UCharsTrie::k_Three_Unit_ارزښت_Lead);
        intUnits[1] = static_cast<char16_t>(static_cast<uint32_t>(i) >> 16);
        intUnits[2] = static_cast<char16_t>(i);
        length=3;
    // } else if(i<=UCharsTrie::kMaxOneUnitValue) {
    //     intUnits[0]=(char16_t)(i);
    //     length=1;
    } وإلا {
        intUnits[0] = static_cast<char16_t>(UCharsTrie::k_Min_Two_Unit_ارزښت_Lead + (i >> 16));
        intUnits[1] = static_cast<char16_t>(i);
        length=2;
    }
    intUnits[0] = static_cast<char16_t>(intUnits[0] | (isFinal << 15));
    أرجع اكتب(intUnits, length);
}

int32_t
UCharsTrieBuilder::write_ارزښت_And_Type(UBool has_ارزښت, int32_t value, int32_t غوټه) {
    إذا(!has_ارزښت) {
        أرجع اكتب(غوټه);
    }
    char16_t intUnits[3];
    int32_t length;
    إذا(value<0 || value>UCharsTrie::k_Max_Two_Unit_غوټه_ارزښت) {
        intUnits[0] = static_cast<char16_t>(UCharsTrie::k_Three_Unit_غوټه_ارزښت_Lead);
        intUnits[1] = static_cast<char16_t>(static_cast<uint32_t>(value) >> 16);
        intUnits[2] = static_cast<char16_t>(value);
        length=3;
    } وإلا إذا(value<=UCharsTrie::k_Max_One_Unit_غوټه_ارزښت) {
        intUnits[0] = static_cast<char16_t>((value + 1) << 6);
        length=1;
    } وإلا {
        intUnits[0] = static_cast<char16_t>(UCharsTrie::k_Min_Two_Unit_غوټه_ارزښت_Lead + ((value >> 10) & 0x7fc0));
        intUnits[1] = static_cast<char16_t>(value);
        length=2;
    }
    intUnits[0] |= static_cast<char16_t>(غوټه);
    أرجع اكتب(intUnits, length);
}

int32_t
UCharsTrieBuilder::write_Delta_منزل(int32_t jump_موخه) {
    int32_t i=uchars_اوږدتیا-jump_موخه;
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

U_NAMESPACE_پای_ټکی
