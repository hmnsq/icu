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

    فراغ مجموعه_مقصد(ثابت UnicodeString &s, int32_t val, UnicodeString &رشتهها, UError_کد &error_کد);

    UnicodeString getString(ثابت UnicodeString& رشتهها U_LIFETIME_BOUND) ثابت {
        int32_t length=رشتهها[string_جابه_جایی];
        أرجع رشتهها.موقت_Sub_String(string_جابه_جایی+1, length);
    }
    int32_t get_String_درازی(ثابت UnicodeString &رشتهها) ثابت {
        أرجع رشتهها[string_جابه_جایی];
    }

    char16_t charAt(int32_t شاخص, ثابت UnicodeString &رشتهها) ثابت {
        أرجع رشتهها[string_جابه_جایی+1+شاخص];
    }

    int32_t get_مقدار() ثابت { أرجع value; }

    int32_t compare_String_مقصد(ثابت UCharsTrieElement &o, ثابت UnicodeString &رشتهها) ثابت;

خاص:
    // The first strings unit contains the string length.
    // (Compared with a stringLength field here, this saves 2 bytes per string.)
    int32_t string_جابه_جایی;
    int32_t value;
};

فراغ
UCharsTrieElement::مجموعه_مقصد(ثابت UnicodeString &s, int32_t val,
                         UnicodeString &رشتهها, UError_کد &error_کد) {
    إذا(U_FAILURE(error_کد)) {
        أرجع;
    }
    int32_t length=s.length();
    إذا(length>0xffff) {
        // Too long: We store the length in 1 unit.
        error_کد=U_شاخص_OUTOFBOUNDS_ERROR;
        أرجع;
    }
    string_جابه_جایی=رشتهها.length();
    رشتهها.append(static_cast<char16_t>(length));
    value=val;
    رشتهها.append(s);
}

int32_t
UCharsTrieElement::compare_String_مقصد(ثابت UCharsTrieElement &other, ثابت UnicodeString &رشتهها) ثابت {
    أرجع getString(رشتهها).compare(other.getString(رشتهها));
}

UCharsTrieBuilder::UCharsTrieBuilder(UError_کد & /*errorCode*/)
        : elements(اشاره_تهی), elements_ظرفیت(0), elements_درازی(0),
          uchars(اشاره_تهی), uchars_ظرفیت(0), uchars_درازی(0) {}

UCharsTrieBuilder::~UCharsTrieBuilder() {
    حذف[] elements;
    uprv_free(uchars);
}

UCharsTrieBuilder&
UCharsTrieBuilder::جمع_کن(ثابت UnicodeString& s,
                       int32_t value,
                       UError_کد& error_کد) U_LIFETIME_BOUND {
    إذا(U_FAILURE(error_کد)) {
        أرجع *هذا;
    }
    إذا(uchars_درازی>0) {
        // Cannot add elements after building.
        error_کد=U_NO_WRITE_PERMISSION;
        أرجع *هذا;
    }
    إذا(elements_درازی==elements_ظرفیت) {
        int32_t new_ظرفیت;
        إذا(elements_ظرفیت==0) {
            new_ظرفیت=1024;
        } وإلا {
            new_ظرفیت=4*elements_ظرفیت;
        }
        UCharsTrieElement *newElements=جديد UCharsTrieElement[new_ظرفیت];
        إذا(newElements==اشاره_تهی) {
            error_کد=U_حافظه_تخصیص_ERROR;
            أرجع *هذا;
        }
        إذا(elements_درازی>0) {
            uprv_memcpy(newElements, elements, (size_t)elements_درازی*حجم(UCharsTrieElement));
        }
        حذف[] elements;
        elements=newElements;
        elements_ظرفیت=new_ظرفیت;
    }
    elements[elements_درازی++].مجموعه_مقصد(s, value, رشتهها, error_کد);
    إذا(U_SUCCESS(error_کد) && رشتهها.isBogus()) {
        error_کد=U_حافظه_تخصیص_ERROR;
    }
    أرجع *هذا;
}

U_CDECL_BEGIN

ساكن int32_t U_CALLCONV
compare_Element_رشتهها(ثابت فراغ *context, ثابت فراغ *چپ, ثابت فراغ *راست) {
    ثابت UnicodeString *رشتهها=static_cast<ثابت UnicodeString *>(context);
    ثابت UCharsTrieElement *چپ_Element=static_cast<ثابت UCharsTrieElement *>(چپ);
    ثابت UCharsTrieElement *راست_Element=static_cast<ثابت UCharsTrieElement *>(راست);
    أرجع چپ_Element->compare_String_مقصد(*راست_Element, *رشتهها);
}

U_CDECL_انتها

UCharsTrie *
UCharsTrieBuilder::build(UStringTrieBuildOption buildOption, UError_کد &error_کد) {
    buildUChars(buildOption, error_کد);
    UCharsTrie *newTrie=اشاره_تهی;
    إذا(U_SUCCESS(error_کد)) {
        newTrie=جديد UCharsTrie(uchars, uchars+(uchars_ظرفیت-uchars_درازی));
        إذا(newTrie==اشاره_تهی) {
            error_کد=U_حافظه_تخصیص_ERROR;
        } وإلا {
            uchars=اشاره_تهی;  // The new trie now owns the array.
            uchars_ظرفیت=0;
        }
    }
    أرجع newTrie;
}

UnicodeString&
UCharsTrieBuilder::buildUnicodeString(UStringTrieBuildOption buildOption,
                                      UnicodeString& نتیجه U_LIFETIME_BOUND,
                                      UError_کد& error_کد) U_LIFETIME_BOUND {
    buildUChars(buildOption, error_کد);
    إذا(U_SUCCESS(error_کد)) {
        نتیجه.مجموعه_مقصد(خطأ, uchars+(uchars_ظرفیت-uchars_درازی), uchars_درازی);
    }
    أرجع نتیجه;
}

فراغ
UCharsTrieBuilder::buildUChars(UStringTrieBuildOption buildOption, UError_کد &error_کد) {
    إذا(U_FAILURE(error_کد)) {
        أرجع;
    }
    إذا(uchars!=اشاره_تهی && uchars_درازی>0) {
        // Already built.
        أرجع;
    }
    إذا(uchars_درازی==0) {
        إذا(elements_درازی==0) {
            error_کد=U_شاخص_OUTOFBOUNDS_ERROR;
            أرجع;
        }
        إذا(رشتهها.isBogus()) {
            error_کد=U_حافظه_تخصیص_ERROR;
            أرجع;
        }
        uprv_مرتب_سازی_Array(elements, elements_درازی, static_cast<int32_t>(حجم(UCharsTrieElement)),
                      compare_Element_رشتهها, &رشتهها,
                      خطأ,  // need not be a stable sort
                      &error_کد);
        إذا(U_FAILURE(error_کد)) {
            أرجع;
        }
        // Duplicate strings are not allowed.
        UnicodeString prev=elements[0].getString(رشتهها);
        لكل(int32_t i=1; i<elements_درازی; ++i) {
            UnicodeString current=elements[i].getString(رشتهها);
            إذا(prev==current) {
                error_کد=U_ILLEGAL_آرگومان_ERROR;
                أرجع;
            }
            prev.fastCopyFrom(current);
        }
    }
    // Create and char16_t-serialize the trie for the elements.
    uchars_درازی=0;
    int32_t capacity=رشتهها.length();
    إذا(capacity<1024) {
        capacity=1024;
    }
    إذا(uchars_ظرفیت<capacity) {
        uprv_free(uchars);
        uchars=static_cast<char16_t *>(uprv_malloc(capacity*2));
        إذا(uchars==اشاره_تهی) {
            error_کد=U_حافظه_تخصیص_ERROR;
            uchars_ظرفیت=0;
            أرجع;
        }
        uchars_ظرفیت=capacity;
    }
    StringTrieBuilder::build(buildOption, elements_درازی, error_کد);
    إذا(uchars==اشاره_تهی) {
        error_کد=U_حافظه_تخصیص_ERROR;
    }
}

int32_t
UCharsTrieBuilder::get_Element_String_درازی(int32_t i) ثابت {
    أرجع elements[i].get_String_درازی(رشتهها);
}

char16_t
UCharsTrieBuilder::getElementUnit(int32_t i, int32_t unit_شاخص) ثابت {
    أرجع elements[i].charAt(unit_شاخص, رشتهها);
}

int32_t
UCharsTrieBuilder::get_Element_مقدار(int32_t i) ثابت {
    أرجع elements[i].get_مقدار();
}

int32_t
UCharsTrieBuilder::get_حد_از_خطی_تطابق(int32_t first, int32_t آخرین, int32_t unit_شاخص) ثابت {
    ثابت UCharsTrieElement &اولی_Element=elements[first];
    ثابت UCharsTrieElement &آخرین_Element=elements[آخرین];
    int32_t min_String_درازی=اولی_Element.get_String_درازی(رشتهها);
    طالما(++unit_شاخص<min_String_درازی &&
            اولی_Element.charAt(unit_شاخص, رشتهها)==
            آخرین_Element.charAt(unit_شاخص, رشتهها)) {}
    أرجع unit_شاخص;
}

int32_t
UCharsTrieBuilder::شمار_Element_Units(int32_t شروع, int32_t حد, int32_t unit_شاخص) ثابت {
    int32_t length=0;  // Number of different units at unitIndex.
    int32_t i=شروع;
    افعل {
        char16_t unit=elements[i++].charAt(unit_شاخص, رشتهها);
        طالما(i<حد && unit==elements[i].charAt(unit_شاخص, رشتهها)) {
            ++i;
        }
        ++length;
    } طالما(i<حد);
    أرجع length;
}

int32_t
UCharsTrieBuilder::skipElementsBySomeUnits(int32_t i, int32_t unit_شاخص, int32_t count) ثابت {
    افعل {
        char16_t unit=elements[i++].charAt(unit_شاخص, رشتهها);
        طالما(unit==elements[i].charAt(unit_شاخص, رشتهها)) {
            ++i;
        }
    } طالما(--count>0);
    أرجع i;
}

int32_t
UCharsTrieBuilder::شاخص_از_Element_With_بعدی_Unit(int32_t i, int32_t unit_شاخص, char16_t unit) ثابت {
    طالما(unit==elements[i].charAt(unit_شاخص, رشتهها)) {
        ++i;
    }
    أرجع i;
}

UCharsTrieBuilder::UCTLinear_تطابق_گره::UCTLinear_تطابق_گره(ثابت char16_t *units, int32_t len, Node *بعدی_گره)
        : خطی_تطابق_گره(len, بعدی_گره), s(units) {
    hash=hash*37u+ustr_hashUCharsN(units, len);
}

منطقي
UCharsTrieBuilder::UCTLinear_تطابق_گره::عملگر==(ثابت Node &other) ثابت {
    إذا(هذا==&other) {
        أرجع درست;
    }
    إذا(!خطی_تطابق_گره::عملگر==(other)) {
        أرجع خطأ;
    }
    ثابت UCTLinear_تطابق_گره &o=static_cast<ثابت UCTLinear_تطابق_گره &>(other);
    أرجع 0==u_memcmp(s, o.s, length);
}

فراغ
UCharsTrieBuilder::UCTLinear_تطابق_گره::اكتب(StringTrieBuilder &builder) {
    UCharsTrieBuilder &b=(UCharsTrieBuilder &)builder;
    next->اكتب(builder);
    b.اكتب(s, length);
    جابه_جایی=b.write_مقدار_And_Type(has_مقدار, value, b.get_Min_خطی_تطابق()+length-1);
}

StringTrieBuilder::Node *
UCharsTrieBuilder::create_خطی_تطابق_گره(int32_t i, int32_t unit_شاخص, int32_t length,
                                         Node *بعدی_گره) ثابت {
    أرجع جديد UCTLinear_تطابق_گره(
            elements[i].getString(رشتهها).get_میانگیر()+unit_شاخص,
            length,
            بعدی_گره);
}

UBool
UCharsTrieBuilder::ensure_ظرفیت(int32_t length) {
    إذا(uchars==اشاره_تهی) {
        أرجع خطأ;  // previous memory allocation had failed
    }
    إذا(length>uchars_ظرفیت) {
        int32_t new_ظرفیت=uchars_ظرفیت;
        افعل {
            new_ظرفیت*=2;
        } طالما(new_ظرفیت<=length);
        char16_t *newUChars=static_cast<char16_t *>(uprv_malloc(new_ظرفیت*2));
        إذا(newUChars==اشاره_تهی) {
            // unable to allocate memory
            uprv_free(uchars);
            uchars=اشاره_تهی;
            uchars_ظرفیت=0;
            أرجع خطأ;
        }
        u_memcpy(newUChars+(new_ظرفیت-uchars_درازی),
                 uchars+(uchars_ظرفیت-uchars_درازی), uchars_درازی);
        uprv_free(uchars);
        uchars=newUChars;
        uchars_ظرفیت=new_ظرفیت;
    }
    أرجع درست;
}

int32_t
UCharsTrieBuilder::اكتب(int32_t unit) {
    int32_t new_درازی=uchars_درازی+1;
    إذا(ensure_ظرفیت(new_درازی)) {
        uchars_درازی=new_درازی;
        uchars[uchars_ظرفیت - uchars_درازی] = static_cast<char16_t>(unit);
    }
    أرجع uchars_درازی;
}

int32_t
UCharsTrieBuilder::اكتب(ثابت char16_t *s, int32_t length) {
    int32_t new_درازی=uchars_درازی+length;
    إذا(ensure_ظرفیت(new_درازی)) {
        uchars_درازی=new_درازی;
        u_memcpy(uchars+(uchars_ظرفیت-uchars_درازی), s, length);
    }
    أرجع uchars_درازی;
}

int32_t
UCharsTrieBuilder::writeElementUnits(int32_t i, int32_t unit_شاخص, int32_t length) {
    أرجع اكتب(elements[i].getString(رشتهها).get_میانگیر()+unit_شاخص, length);
}

int32_t
UCharsTrieBuilder::write_مقدار_And_Final(int32_t i, UBool isFinal) {
    إذا(0<=i && i<=UCharsTrie::k_Max_One_Unit_مقدار) {
        أرجع اكتب(i|(isFinal<<15));
    }
    char16_t intUnits[3];
    int32_t length;
    إذا(i<0 || i>UCharsTrie::k_Max_Two_Unit_مقدار) {
        intUnits[0] = static_cast<char16_t>(UCharsTrie::k_Three_Unit_مقدار_Lead);
        intUnits[1] = static_cast<char16_t>(static_cast<uint32_t>(i) >> 16);
        intUnits[2] = static_cast<char16_t>(i);
        length=3;
    // } else if(i<=UCharsTrie::kMaxOneUnitValue) {
    //     intUnits[0]=(char16_t)(i);
    //     length=1;
    } وإلا {
        intUnits[0] = static_cast<char16_t>(UCharsTrie::k_Min_Two_Unit_مقدار_Lead + (i >> 16));
        intUnits[1] = static_cast<char16_t>(i);
        length=2;
    }
    intUnits[0] = static_cast<char16_t>(intUnits[0] | (isFinal << 15));
    أرجع اكتب(intUnits, length);
}

int32_t
UCharsTrieBuilder::write_مقدار_And_Type(UBool has_مقدار, int32_t value, int32_t گره) {
    إذا(!has_مقدار) {
        أرجع اكتب(گره);
    }
    char16_t intUnits[3];
    int32_t length;
    إذا(value<0 || value>UCharsTrie::k_Max_Two_Unit_گره_مقدار) {
        intUnits[0] = static_cast<char16_t>(UCharsTrie::k_Three_Unit_گره_مقدار_Lead);
        intUnits[1] = static_cast<char16_t>(static_cast<uint32_t>(value) >> 16);
        intUnits[2] = static_cast<char16_t>(value);
        length=3;
    } وإلا إذا(value<=UCharsTrie::k_Max_One_Unit_گره_مقدار) {
        intUnits[0] = static_cast<char16_t>((value + 1) << 6);
        length=1;
    } وإلا {
        intUnits[0] = static_cast<char16_t>(UCharsTrie::k_Min_Two_Unit_گره_مقدار_Lead + ((value >> 10) & 0x7fc0));
        intUnits[1] = static_cast<char16_t>(value);
        length=2;
    }
    intUnits[0] |= static_cast<char16_t>(گره);
    أرجع اكتب(intUnits, length);
}

int32_t
UCharsTrieBuilder::write_Delta_مقصد(int32_t jump_هدف) {
    int32_t i=uchars_درازی-jump_هدف;
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

U_NAMESPACE_انتها
