// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
*******************************************************************************
*   Copyright (C) 2011-2012, International Business Machines
*   Corporation and others.  All Rights Reserved.
*******************************************************************************
*   file name:  messagepattern.cpp
*   encoding:   UTF-8
*   tab size:   8 (not used)
*   indentation:4
*
*   created on: 2011mar14
*   created by: Markus W. Scherer
*/

#ضمن "unicode/utypes.h"

#إذا !UCONFIG_NO_FORMATTING

#ضمن "unicode/messagepattern.h"
#ضمن "unicode/unistr.h"
#ضمن "unicode/utf16.h"
#ضمن "cmemory.h"
#ضمن "cstring.h"
#ضمن "messageimpl.h"
#ضمن "patternprops.h"
#ضمن "putilimp.h"
#ضمن "uassert.h"

U_NAMESPACE_BEGIN

// Unicode character/code point constants ---------------------------------- ***

ساكن ثابت char16_t u_pound=0x23;
ساكن ثابت char16_t u_apos=0x27;
ساكن ثابت char16_t u_plus=0x2B;
ساكن ثابت char16_t u_comma=0x2C;
ساكن ثابت char16_t u_minus=0x2D;
ساكن ثابت char16_t u_dot=0x2E;
ساكن ثابت char16_t u_colon=0x3A;
ساكن ثابت char16_t u_lessThan=0x3C;
ساكن ثابت char16_t u_equal=0x3D;
ساكن ثابت char16_t u_A=0x41;
ساكن ثابت char16_t u_C=0x43;
ساكن ثابت char16_t u_D=0x44;
ساكن ثابت char16_t u_E=0x45;
ساكن ثابت char16_t u_H=0x48;
ساكن ثابت char16_t u_I=0x49;
ساكن ثابت char16_t u_L=0x4C;
ساكن ثابت char16_t u_N=0x4E;
ساكن ثابت char16_t u_O=0x4F;
ساكن ثابت char16_t u_P=0x50;
ساكن ثابت char16_t u_R=0x52;
ساكن ثابت char16_t u_S=0x53;
ساكن ثابت char16_t u_T=0x54;
ساكن ثابت char16_t u_U=0x55;
ساكن ثابت char16_t u_Z=0x5A;
ساكن ثابت char16_t u_a=0x61;
ساكن ثابت char16_t u_c=0x63;
ساكن ثابت char16_t u_d=0x64;
ساكن ثابت char16_t u_e=0x65;
ساكن ثابت char16_t u_f=0x66;
ساكن ثابت char16_t u_h=0x68;
ساكن ثابت char16_t u_i=0x69;
ساكن ثابت char16_t u_l=0x6C;
ساكن ثابت char16_t u_n=0x6E;
ساكن ثابت char16_t u_o=0x6F;
ساكن ثابت char16_t u_p=0x70;
ساكن ثابت char16_t u_r=0x72;
ساكن ثابت char16_t u_s=0x73;
ساكن ثابت char16_t u_t=0x74;
ساكن ثابت char16_t u_u=0x75;
ساكن ثابت char16_t u_z=0x7A;
ساكن ثابت char16_t u_بایاں_Curly_Brace=0x7B;
ساكن ثابت char16_t u_pipe=0x7C;
ساكن ثابت char16_t u_دایاں_Curly_Brace=0x7D;
ساكن ثابت char16_t u_lessOrEqual=0x2264;  // U+2264 is <=

ساكن ثابت char16_t k_وتھ_Colon[]={  // "offset:"
    u_o, u_f, u_f, u_s, u_e, u_t, u_colon
};

ساكن ثابت char16_t kOther[]={  // "other"
    u_o, u_t, u_h, u_e, u_r
};

// MessagePatternList ------------------------------------------------------ ***

سانچا<قسم_ناں T, int32_t ڈھیر_سمرتھا>
صنف سنیہا_نمونہ_List : عام UMemory {
عام:
    سنیہا_نمونہ_List() {}
    فراغ copyFrom(ثابت سنیہا_نمونہ_List<T, ڈھیر_سمرتھا> &other,
                  int32_t length,
                  UError_کوڈ &error_کوڈ);
    UBool ensure_سمرتھا_For_One_ہور(int32_t old_لمائی, UError_کوڈ &error_کوڈ);
    UBool equals(ثابت سنیہا_نمونہ_List<T, ڈھیر_سمرتھا> &other, int32_t length) ثابت {
        لكل(int32_t i=0; i<length; ++i) {
            إذا(a[i]!=other.a[i]) { أرجع خطأ; }
        }
        أرجع سچ;
    }

    Maybe_ڈھیر_Array<T, ڈھیر_سمرتھا> a;
};

سانچا<قسم_ناں T, int32_t ڈھیر_سمرتھا>
فراغ
سنیہا_نمونہ_List<T, ڈھیر_سمرتھا>::copyFrom(
        ثابت سنیہا_نمونہ_List<T, ڈھیر_سمرتھا> &other,
        int32_t length,
        UError_کوڈ &error_کوڈ) {
    إذا(U_SUCCESS(error_کوڈ) && length>0) {
        إذا(length>a.get_سمرتھا() && خالی_سوچک==a.resize(length)) {
            error_کوڈ=U_حافظہ_ونڈ_ERROR;
            أرجع;
        }
        uprv_memcpy(a.getAlias(), other.a.getAlias(), (size_t)length*حجم(T));
    }
}

سانچا<قسم_ناں T, int32_t ڈھیر_سمرتھا>
UBool
سنیہا_نمونہ_List<T, ڈھیر_سمرتھا>::ensure_سمرتھا_For_One_ہور(int32_t old_لمائی, UError_کوڈ &error_کوڈ) {
    إذا(U_FAILURE(error_کوڈ)) {
        أرجع خطأ;
    }
    إذا(a.get_سمرتھا()>old_لمائی || a.resize(2*old_لمائی, old_لمائی)!=خالی_سوچک) {
        أرجع سچ;
    }
    error_کوڈ=U_حافظہ_ونڈ_ERROR;
    أرجع خطأ;
}

// MessagePatternList specializations -------------------------------------- ***

صنف سنیہا_نمونہ_Double_List : عام سنیہا_نمونہ_List<مزدوج, 8> {
};

صنف سنیہا_نمونہ_Parts_List : عام سنیہا_نمونہ_List<سنیہا_نمونہ::Part, 32> {
};

// MessagePattern constructors etc. ---------------------------------------- ***

سنیہا_نمونہ::سنیہا_نمونہ(UError_کوڈ &error_کوڈ)
        : apos_منوال(UCONFIG_MSGPAT_DEFAULT_APOSTROPHE_منوال),
          partsList(خالی_سوچک), parts(خالی_سوچک), parts_لمائی(0),
          numeric_اقدار_List(خالی_سوچک), numeric_اقدار(خالی_سوچک), numeric_اقدار_لمائی(0),
          has_Arg_ناں(خطأ), hasArgNumbers(خطأ), needsAutoQuoting(خطأ) {
    init(error_کوڈ);
}

سنیہا_نمونہ::سنیہا_نمونہ(UMessage_نمونہ_Apostrophe_منوال منوال, UError_کوڈ &error_کوڈ)
        : apos_منوال(منوال),
          partsList(خالی_سوچک), parts(خالی_سوچک), parts_لمائی(0),
          numeric_اقدار_List(خالی_سوچک), numeric_اقدار(خالی_سوچک), numeric_اقدار_لمائی(0),
          has_Arg_ناں(خطأ), hasArgNumbers(خطأ), needsAutoQuoting(خطأ) {
    init(error_کوڈ);
}

سنیہا_نمونہ::سنیہا_نمونہ(ثابت UnicodeString &نمونہ, UParseError *parseError, UError_کوڈ &error_کوڈ)
        : apos_منوال(UCONFIG_MSGPAT_DEFAULT_APOSTROPHE_منوال),
          partsList(خالی_سوچک), parts(خالی_سوچک), parts_لمائی(0),
          numeric_اقدار_List(خالی_سوچک), numeric_اقدار(خالی_سوچک), numeric_اقدار_لمائی(0),
          has_Arg_ناں(خطأ), hasArgNumbers(خطأ), needsAutoQuoting(خطأ) {
    إذا(init(error_کوڈ)) {
        parse(نمونہ, parseError, error_کوڈ);
    }
}

UBool
سنیہا_نمونہ::init(UError_کوڈ &error_کوڈ) {
    إذا(U_FAILURE(error_کوڈ)) {
        أرجع خطأ;
    }
    partsList=جديد سنیہا_نمونہ_Parts_List();
    إذا(partsList==خالی_سوچک) {
        error_کوڈ=U_حافظہ_ونڈ_ERROR;
        أرجع خطأ;
    }
    parts=partsList->a.getAlias();
    أرجع سچ;
}

سنیہا_نمونہ::سنیہا_نمونہ(ثابت سنیہا_نمونہ &other)
        : UObject(other), apos_منوال(other.apos_منوال), پیغام(other.پیغام),
          partsList(خالی_سوچک), parts(خالی_سوچک), parts_لمائی(0),
          numeric_اقدار_List(خالی_سوچک), numeric_اقدار(خالی_سوچک), numeric_اقدار_لمائی(0),
          has_Arg_ناں(other.has_Arg_ناں), hasArgNumbers(other.hasArgNumbers),
          needsAutoQuoting(other.needsAutoQuoting) {
    UError_کوڈ error_کوڈ=U_ZERO_ERROR;
    إذا(!copyStorage(other, error_کوڈ)) {
        clear();
    }
}

سنیہا_نمونہ&
سنیہا_نمونہ::عامل=(ثابت سنیہا_نمونہ& other) U_LIFETIME_BOUND {
    إذا(هذا==&other) {
        أرجع *هذا;
    }
    apos_منوال=other.apos_منوال;
    پیغام=other.پیغام;
    has_Arg_ناں=other.has_Arg_ناں;
    hasArgNumbers=other.hasArgNumbers;
    needsAutoQuoting=other.needsAutoQuoting;
    UError_کوڈ error_کوڈ=U_ZERO_ERROR;
    إذا(!copyStorage(other, error_کوڈ)) {
        clear();
    }
    أرجع *هذا;
}

UBool
سنیہا_نمونہ::copyStorage(ثابت سنیہا_نمونہ &other, UError_کوڈ &error_کوڈ) {
    إذا(U_FAILURE(error_کوڈ)) {
        أرجع خطأ;
    }
    parts=خالی_سوچک;
    parts_لمائی=0;
    numeric_اقدار=خالی_سوچک;
    numeric_اقدار_لمائی=0;
    إذا(partsList==خالی_سوچک) {
        partsList=جديد سنیہا_نمونہ_Parts_List();
        إذا(partsList==خالی_سوچک) {
            error_کوڈ=U_حافظہ_ونڈ_ERROR;
            أرجع خطأ;
        }
        parts=partsList->a.getAlias();
    }
    إذا(other.parts_لمائی>0) {
        partsList->copyFrom(*other.partsList, other.parts_لمائی, error_کوڈ);
        إذا(U_FAILURE(error_کوڈ)) {
            أرجع خطأ;
        }
        parts=partsList->a.getAlias();
        parts_لمائی=other.parts_لمائی;
    }
    إذا(other.numeric_اقدار_لمائی>0) {
        إذا(numeric_اقدار_List==خالی_سوچک) {
            numeric_اقدار_List=جديد سنیہا_نمونہ_Double_List();
            إذا(numeric_اقدار_List==خالی_سوچک) {
                error_کوڈ=U_حافظہ_ونڈ_ERROR;
                أرجع خطأ;
            }
            numeric_اقدار=numeric_اقدار_List->a.getAlias();
        }
        numeric_اقدار_List->copyFrom(
            *other.numeric_اقدار_List, other.numeric_اقدار_لمائی, error_کوڈ);
        إذا(U_FAILURE(error_کوڈ)) {
            أرجع خطأ;
        }
        numeric_اقدار=numeric_اقدار_List->a.getAlias();
        numeric_اقدار_لمائی=other.numeric_اقدار_لمائی;
    }
    أرجع سچ;
}

سنیہا_نمونہ::~سنیہا_نمونہ() {
    مٹاؤ partsList;
    مٹاؤ numeric_اقدار_List;
}

// MessagePattern API ------------------------------------------------------ ***

سنیہا_نمونہ&
سنیہا_نمونہ::parse(ثابت UnicodeString& نمونہ,
                      UParseError* parseError,
                      UError_کوڈ& error_کوڈ) U_LIFETIME_BOUND {
    preParse(نمونہ, parseError, error_کوڈ);
    parse_سنیہا(0, 0, 0, UMSGPAT_ARG_TYPE_NONE, parseError, error_کوڈ);
    postParse();
    أرجع *هذا;
}

سنیہا_نمونہ&
سنیہا_نمونہ::parseChoiceStyle(ثابت UnicodeString& نمونہ,
                                 UParseError* parseError,
                                 UError_کوڈ& error_کوڈ) U_LIFETIME_BOUND {
    preParse(نمونہ, parseError, error_کوڈ);
    parseChoiceStyle(0, 0, parseError, error_کوڈ);
    postParse();
    أرجع *هذا;
}

سنیہا_نمونہ&
سنیہا_نمونہ::parsePluralStyle(ثابت UnicodeString& نمونہ,
                                 UParseError* parseError,
                                 UError_کوڈ& error_کوڈ) U_LIFETIME_BOUND {
    preParse(نمونہ, parseError, error_کوڈ);
    parsePluralOrSelectStyle(UMSGPAT_ARG_TYPE_PLURAL, 0, 0, parseError, error_کوڈ);
    postParse();
    أرجع *هذا;
}

سنیہا_نمونہ&
سنیہا_نمونہ::parseSelectStyle(ثابت UnicodeString& نمونہ,
                                 UParseError* parseError,
                                 UError_کوڈ& error_کوڈ) U_LIFETIME_BOUND {
    preParse(نمونہ, parseError, error_کوڈ);
    parsePluralOrSelectStyle(UMSGPAT_ARG_TYPE_SELECT, 0, 0, parseError, error_کوڈ);
    postParse();
    أرجع *هذا;
}

فراغ
سنیہا_نمونہ::clear() {
    // Mostly the same as preParse().
    پیغام.remove();
    has_Arg_ناں=hasArgNumbers=خطأ;
    needsAutoQuoting=خطأ;
    parts_لمائی=0;
    numeric_اقدار_لمائی=0;
}

منطقي
سنیہا_نمونہ::عامل==(ثابت سنیہا_نمونہ &other) ثابت {
    إذا(هذا==&other) {
        أرجع سچ;
    }
    أرجع
        apos_منوال==other.apos_منوال &&
        پیغام==other.پیغام &&
        // parts.equals(o.parts)
        parts_لمائی==other.parts_لمائی &&
        (parts_لمائی==0 || partsList->equals(*other.partsList, parts_لمائی));
    // No need to compare numericValues if msg and parts are the same.
}

int32_t
سنیہا_نمونہ::hash_کوڈ() ثابت {
    int32_t hash=(apos_منوال*37+پیغام.hash_کوڈ())*37+parts_لمائی;
    لكل(int32_t i=0; i<parts_لمائی; ++i) {
        hash=hash*37+parts[i].hash_کوڈ();
    }
    أرجع hash;
}

int32_t
سنیہا_نمونہ::validate_دلیل_Name(ثابت UnicodeString &name) {
    إذا(!نمونہ_Props::isIdentifier(name.get_بفر(), name.length())) {
        أرجع UMSGPAT_ARG_NAME_NOT_VALID;
    }
    أرجع parse_Arg_نمبر(name, 0, name.length());
}

UnicodeString
سنیہا_نمونہ::autoQuoteApostropheDeep() ثابت {
    إذا(!needsAutoQuoting) {
        أرجع پیغام;
    }
    UnicodeString modified(پیغام);
    // Iterate backward so that the insertion indexes do not change.
    int32_t count=گنتی_Parts();
    لكل(int32_t i=count; i>0;) {
        ثابت Part &part=getPart(--i);
        إذا(part.getType()==UMSGPAT_PART_TYPE_داخل_کرو_CHAR) {
           modified.insert(part.اشاریہ, static_cast<char16_t>(part.value));
        }
    }
    أرجع modified;
}

مزدوج
سنیہا_نمونہ::get_Numeric_قدر(ثابت Part &part) ثابت {
    UMessage_نمونہ_Part_Type type=part.type;
    إذا(type==UMSGPAT_PART_TYPE_ARG_INT) {
        أرجع part.value;
    } وإلا إذا(type==UMSGPAT_PART_TYPE_ARG_DOUBLE) {
        أرجع numeric_اقدار[part.value];
    } وإلا {
        أرجع UMSGPAT_NO_NUMERIC_قدر;
    }
}

/**
  * Returns the "offset:" value of a PluralFormat argument, or 0 if none is specified.
  * @param pluralStart the index of the first PluralFormat argument style part. (0..countParts()-1)
  * @return the "offset:" value.
  * @draft ICU 4.8
  */
مزدوج
سنیہا_نمونہ::get_Plural_وتھ(int32_t plural_شروع) ثابت {
    ثابت Part &part=getPart(plural_شروع);
    إذا(Part::has_Numeric_قدر(part.type)) {
        أرجع get_Numeric_قدر(part);
    } وإلا {
        أرجع 0;
    }
}

// MessagePattern::Part ---------------------------------------------------- ***

منطقي
سنیہا_نمونہ::Part::عامل==(ثابت Part &other) ثابت {
    إذا(هذا==&other) {
        أرجع سچ;
    }
    أرجع
        type==other.type &&
        اشاریہ==other.اشاریہ &&
        length==other.length &&
        value==other.value &&
        حد_بندی_Part_اشاریہ==other.حد_بندی_Part_اشاریہ;
}

// MessagePattern parser --------------------------------------------------- ***

فراغ
سنیہا_نمونہ::preParse(ثابت UnicodeString &نمونہ, UParseError *parseError, UError_کوڈ &error_کوڈ) {
    إذا(U_FAILURE(error_کوڈ)) {
        أرجع;
    }
    إذا(parseError!=خالی_سوچک) {
        parseError->سطر=0;
        parseError->وتھ=0;
        parseError->preContext[0]=0;
        parseError->postContext[0]=0;
    }
    پیغام=نمونہ;
    has_Arg_ناں=hasArgNumbers=خطأ;
    needsAutoQuoting=خطأ;
    parts_لمائی=0;
    numeric_اقدار_لمائی=0;
}

فراغ
سنیہا_نمونہ::postParse() {
    إذا(partsList!=خالی_سوچک) {
        parts=partsList->a.getAlias();
    }
    إذا(numeric_اقدار_List!=خالی_سوچک) {
        numeric_اقدار=numeric_اقدار_List->a.getAlias();
    }
}

int32_t
سنیہا_نمونہ::parse_سنیہا(int32_t اشاریہ, int32_t پیغام_شروع_لمائی,
                             int32_t nestingLevel, UMessage_نمونہ_Arg_Type ماپا_Type,
                             UParseError *parseError, UError_کوڈ &error_کوڈ) {
    إذا(U_FAILURE(error_کوڈ)) {
        أرجع 0;
    }
    إذا(nestingLevel>Part::MAX_NESTED_پدھراں) {
        error_کوڈ=U_اشاریہ_OUTOFBOUNDS_ERROR;
        أرجع 0;
    }
    int32_t پیغام_شروع=parts_لمائی;
    جوڑو_Part(UMSGPAT_PART_TYPE_پیغام_شروع, اشاریہ, پیغام_شروع_لمائی, nestingLevel, error_کوڈ);
    اشاریہ+=پیغام_شروع_لمائی;
    لكل(;;) {  // while(index<msg.length()) with U_FAILURE(errorCode) check
        إذا(U_FAILURE(error_کوڈ)) {
            أرجع 0;
        }
        إذا(اشاریہ>=پیغام.length()) {
            اكسر;
        }
        char16_t c=پیغام.charAt(اشاریہ++);
        إذا(c==u_apos) {
            إذا(اشاریہ==پیغام.length()) {
                // The apostrophe is the last character in the pattern. 
                // Add a Part for auto-quoting.
                جوڑو_Part(UMSGPAT_PART_TYPE_داخل_کرو_CHAR, اشاریہ, 0,
                        u_apos, error_کوڈ);  // value=char to be inserted
                needsAutoQuoting=سچ;
            } وإلا {
                c=پیغام.charAt(اشاریہ);
                إذا(c==u_apos) {
                    // double apostrophe, skip the second one
                    جوڑو_Part(UMSGPAT_PART_TYPE_SKIP_SYNTAX, اشاریہ++, 1, 0, error_کوڈ);
                } وإلا إذا(
                    apos_منوال==UMSGPAT_APOS_DOUBLE_REQUIRED ||
                    c==u_بایاں_Curly_Brace || c==u_دایاں_Curly_Brace ||
                    (ماپا_Type==UMSGPAT_ARG_TYPE_CHOICE && c==u_pipe) ||
                    (UMSGPAT_ARG_TYPE_HAS_PLURAL_STYLE(ماپا_Type) && c==u_pound)
                ) {
                    // skip the quote-starting apostrophe
                    جوڑو_Part(UMSGPAT_PART_TYPE_SKIP_SYNTAX, اشاریہ-1, 1, 0, error_کوڈ);
                    // find the end of the quoted literal text
                    لكل(;;) {
                        اشاریہ=پیغام.اشاریہ_دا(u_apos, اشاریہ+1);
                        إذا(اشاریہ>=0) {
                            إذا(/*(index+1)<msg.length() &&*/ پیغام.charAt(اشاریہ+1)==u_apos) {
                                // double apostrophe inside quoted literal text
                                // still encodes a single apostrophe, skip the second one
                                جوڑو_Part(UMSGPAT_PART_TYPE_SKIP_SYNTAX, ++اشاریہ, 1, 0, error_کوڈ);
                            } وإلا {
                                // skip the quote-ending apostrophe
                                جوڑو_Part(UMSGPAT_PART_TYPE_SKIP_SYNTAX, اشاریہ++, 1, 0, error_کوڈ);
                                اكسر;
                            }
                        } وإلا {
                            // The quoted text reaches to the end of the of the message.
                            اشاریہ=پیغام.length();
                            // Add a Part for auto-quoting.
                            جوڑو_Part(UMSGPAT_PART_TYPE_داخل_کرو_CHAR, اشاریہ, 0,
                                    u_apos, error_کوڈ);  // value=char to be inserted
                            needsAutoQuoting=سچ;
                            اكسر;
                        }
                    }
                } وإلا {
                    // Interpret the apostrophe as literal text.
                    // Add a Part for auto-quoting.
                    جوڑو_Part(UMSGPAT_PART_TYPE_داخل_کرو_CHAR, اشاریہ, 0,
                            u_apos, error_کوڈ);  // value=char to be inserted
                    needsAutoQuoting=سچ;
                }
            }
        } وإلا إذا(UMSGPAT_ARG_TYPE_HAS_PLURAL_STYLE(ماپا_Type) && c==u_pound) {
            // The unquoted # in a plural message fragment will be replaced
            // with the (number-offset).
            جوڑو_Part(UMSGPAT_PART_TYPE_REPLACE_نمبر, اشاریہ-1, 1, 0, error_کوڈ);
        } وإلا إذا(c==u_بایاں_Curly_Brace) {
            اشاریہ=parseArg(اشاریہ-1, 1, nestingLevel, parseError, error_کوڈ);
        } وإلا إذا((nestingLevel>0 && c==u_دایاں_Curly_Brace) ||
                  (ماپا_Type==UMSGPAT_ARG_TYPE_CHOICE && c==u_pipe)) {
            // Finish the message before the terminator.
            // In a choice style, report the "}" substring only for the following ARG_LIMIT,
            // not for this MSG_LIMIT.
            int32_t حد_بندی_لمائی=(ماپا_Type==UMSGPAT_ARG_TYPE_CHOICE && c==u_دایاں_Curly_Brace) ? 0 : 1;
            جوڑو_حد_بندی_Part(پیغام_شروع, UMSGPAT_PART_TYPE_پیغام_حد_بندی, اشاریہ-1, حد_بندی_لمائی,
                         nestingLevel, error_کوڈ);
            إذا(ماپا_Type==UMSGPAT_ARG_TYPE_CHOICE) {
                // Let the choice style parser see the '}' or '|'.
                أرجع اشاریہ-1;
            } وإلا {
                // continue parsing after the '}'
                أرجع اشاریہ;
            }
        }  // else: c is part of literal text
    }
    إذا(nestingLevel>0 && !وچ_چوٹی_Level_Choice_سنیہا(nestingLevel, ماپا_Type)) {
        سیٹ_Parse_Error(parseError, 0);  // Unmatched '{' braces in message.
        error_کوڈ=U_UNMATCHED_BRACES;
        أرجع 0;
    }
    جوڑو_حد_بندی_Part(پیغام_شروع, UMSGPAT_PART_TYPE_پیغام_حد_بندی, اشاریہ, 0, nestingLevel, error_کوڈ);
    أرجع اشاریہ;
}

int32_t
سنیہا_نمونہ::parseArg(int32_t اشاریہ, int32_t arg_شروع_لمائی, int32_t nestingLevel,
                         UParseError *parseError, UError_کوڈ &error_کوڈ) {
    int32_t arg_شروع=parts_لمائی;
    UMessage_نمونہ_Arg_Type argType=UMSGPAT_ARG_TYPE_NONE;
    جوڑو_Part(UMSGPAT_PART_TYPE_ARG_شروع, اشاریہ, arg_شروع_لمائی, argType, error_کوڈ);
    إذا(U_FAILURE(error_کوڈ)) {
        أرجع 0;
    }
    int32_t name_اشاریہ=اشاریہ=skipWhiteSpace(اشاریہ+arg_شروع_لمائی);
    إذا(اشاریہ==پیغام.length()) {
        سیٹ_Parse_Error(parseError, 0);  // Unmatched '{' braces in message.
        error_کوڈ=U_UNMATCHED_BRACES;
        أرجع 0;
    }
    // parse argument name or number
    اشاریہ=skipIdentifier(اشاریہ);
    int32_t number=parse_Arg_نمبر(name_اشاریہ, اشاریہ);
    إذا(number>=0) {
        int32_t length=اشاریہ-name_اشاریہ;
        إذا(length>Part::MAX_لمائی || number>Part::MAX_قدر) {
            سیٹ_Parse_Error(parseError, name_اشاریہ);  // Argument number too large.
            error_کوڈ=U_اشاریہ_OUTOFBOUNDS_ERROR;
            أرجع 0;
        }
        hasArgNumbers=سچ;
        جوڑو_Part(UMSGPAT_PART_TYPE_ARG_نمبر, name_اشاریہ, length, number, error_کوڈ);
    } وإلا إذا(number==UMSGPAT_ARG_NAME_NOT_نمبر) {
        int32_t length=اشاریہ-name_اشاریہ;
        إذا(length>Part::MAX_لمائی) {
            سیٹ_Parse_Error(parseError, name_اشاریہ);  // Argument name too long.
            error_کوڈ=U_اشاریہ_OUTOFBOUNDS_ERROR;
            أرجع 0;
        }
        has_Arg_ناں=سچ;
        جوڑو_Part(UMSGPAT_PART_TYPE_ARG_NAME, name_اشاریہ, length, 0, error_کوڈ);
    } وإلا {  // number<-1 (ARG_NAME_NOT_VALID)
        سیٹ_Parse_Error(parseError, name_اشاریہ);  // Bad argument syntax.
        error_کوڈ=U_نمونہ_SYNTAX_ERROR;
        أرجع 0;
    }
    اشاریہ=skipWhiteSpace(اشاریہ);
    إذا(اشاریہ==پیغام.length()) {
        سیٹ_Parse_Error(parseError, 0);  // Unmatched '{' braces in message.
        error_کوڈ=U_UNMATCHED_BRACES;
        أرجع 0;
    }
    char16_t c=پیغام.charAt(اشاریہ);
    إذا(c==u_دایاں_Curly_Brace) {
        // all done
    } وإلا إذا(c!=u_comma) {
        سیٹ_Parse_Error(parseError, name_اشاریہ);  // Bad argument syntax.
        error_کوڈ=U_نمونہ_SYNTAX_ERROR;
        أرجع 0;
    } وإلا /* ',' */ {
        // parse argument type: case-sensitive a-zA-Z
        int32_t type_اشاریہ=اشاریہ=skipWhiteSpace(اشاریہ+1);
        طالما(اشاریہ<پیغام.length() && isArgTypeChar(پیغام.charAt(اشاریہ))) {
            ++اشاریہ;
        }
        int32_t length=اشاریہ-type_اشاریہ;
        اشاریہ=skipWhiteSpace(اشاریہ);
        إذا(اشاریہ==پیغام.length()) {
            سیٹ_Parse_Error(parseError, 0);  // Unmatched '{' braces in message.
            error_کوڈ=U_UNMATCHED_BRACES;
            أرجع 0;
        }
        إذا(length==0 || ((c=پیغام.charAt(اشاریہ))!=u_comma && c!=u_دایاں_Curly_Brace)) {
            سیٹ_Parse_Error(parseError, name_اشاریہ);  // Bad argument syntax.
            error_کوڈ=U_نمونہ_SYNTAX_ERROR;
            أرجع 0;
        }
        إذا(length>Part::MAX_لمائی) {
            سیٹ_Parse_Error(parseError, name_اشاریہ);  // Argument type name too long.
            error_کوڈ=U_اشاریہ_OUTOFBOUNDS_ERROR;
            أرجع 0;
        }
        argType=UMSGPAT_ARG_TYPE_SIMPLE;
        إذا(length==6) {
            // case-insensitive comparisons for complex-type names
            إذا(isChoice(type_اشاریہ)) {
                argType=UMSGPAT_ARG_TYPE_CHOICE;
            } وإلا إذا(isPlural(type_اشاریہ)) {
                argType=UMSGPAT_ARG_TYPE_PLURAL;
            } وإلا إذا(isSelect(type_اشاریہ)) {
                argType=UMSGPAT_ARG_TYPE_SELECT;
            }
        } وإلا إذا(length==13) {
            إذا(isSelect(type_اشاریہ) && isOrdinal(type_اشاریہ+6)) {
                argType=UMSGPAT_ARG_TYPE_SELECTORDINAL;
            }
        }
        // change the ARG_START type from NONE to argType
        partsList->a[arg_شروع].value = static_cast<int16_t>(argType);
        إذا(argType==UMSGPAT_ARG_TYPE_SIMPLE) {
            جوڑو_Part(UMSGPAT_PART_TYPE_ARG_TYPE, type_اشاریہ, length, 0, error_کوڈ);
        }
        // look for an argument style (pattern)
        إذا(c==u_دایاں_Curly_Brace) {
            إذا(argType!=UMSGPAT_ARG_TYPE_SIMPLE) {
                سیٹ_Parse_Error(parseError, name_اشاریہ);  // No style field for complex argument.
                error_کوڈ=U_نمونہ_SYNTAX_ERROR;
                أرجع 0;
            }
        } وإلا /* ',' */ {
            ++اشاریہ;
            إذا(argType==UMSGPAT_ARG_TYPE_SIMPLE) {
                اشاریہ=parseSimpleStyle(اشاریہ, parseError, error_کوڈ);
            } وإلا إذا(argType==UMSGPAT_ARG_TYPE_CHOICE) {
                اشاریہ=parseChoiceStyle(اشاریہ, nestingLevel, parseError, error_کوڈ);
            } وإلا {
                اشاریہ=parsePluralOrSelectStyle(argType, اشاریہ, nestingLevel, parseError, error_کوڈ);
            }
        }
    }
    // Argument parsing stopped on the '}'.
    جوڑو_حد_بندی_Part(arg_شروع, UMSGPAT_PART_TYPE_ARG_حد_بندی, اشاریہ, 1, argType, error_کوڈ);
    أرجع اشاریہ+1;
}

int32_t
سنیہا_نمونہ::parseSimpleStyle(int32_t اشاریہ, UParseError *parseError, UError_کوڈ &error_کوڈ) {
    إذا(U_FAILURE(error_کوڈ)) {
        أرجع 0;
    }
    int32_t شروع=اشاریہ;
    int32_t nestedBraces=0;
    طالما(اشاریہ<پیغام.length()) {
        char16_t c=پیغام.charAt(اشاریہ++);
        إذا(c==u_apos) {
            // Treat apostrophe as quoting but include it in the style part.
            // Find the end of the quoted literal text.
            اشاریہ=پیغام.اشاریہ_دا(u_apos, اشاریہ);
            إذا(اشاریہ<0) {
                // Quoted literal argument style text reaches to the end of the message.
                سیٹ_Parse_Error(parseError, شروع);
                error_کوڈ=U_نمونہ_SYNTAX_ERROR;
                أرجع 0;
            }
            // skip the quote-ending apostrophe
            ++اشاریہ;
        } وإلا إذا(c==u_بایاں_Curly_Brace) {
            ++nestedBraces;
        } وإلا إذا(c==u_دایاں_Curly_Brace) {
            إذا(nestedBraces>0) {
                --nestedBraces;
            } وإلا {
                int32_t length=--اشاریہ-شروع;
                إذا(length>Part::MAX_لمائی) {
                    سیٹ_Parse_Error(parseError, شروع);  // Argument style text too long.
                    error_کوڈ=U_اشاریہ_OUTOFBOUNDS_ERROR;
                    أرجع 0;
                }
                جوڑو_Part(UMSGPAT_PART_TYPE_ARG_STYLE, شروع, length, 0, error_کوڈ);
                أرجع اشاریہ;
            }
        }  // c is part of literal text
    }
    سیٹ_Parse_Error(parseError, 0);  // Unmatched '{' braces in message.
    error_کوڈ=U_UNMATCHED_BRACES;
    أرجع 0;
}

int32_t
سنیہا_نمونہ::parseChoiceStyle(int32_t اشاریہ, int32_t nestingLevel,
                                 UParseError *parseError, UError_کوڈ &error_کوڈ) {
    إذا(U_FAILURE(error_کوڈ)) {
        أرجع 0;
    }
    int32_t شروع=اشاریہ;
    اشاریہ=skipWhiteSpace(اشاریہ);
    إذا(اشاریہ==پیغام.length() || پیغام.charAt(اشاریہ)==u_دایاں_Curly_Brace) {
        سیٹ_Parse_Error(parseError, 0);  // Missing choice argument pattern.
        error_کوڈ=U_نمونہ_SYNTAX_ERROR;
        أرجع 0;
    }
    لكل(;;) {
        // The choice argument style contains |-separated (number, separator, message) triples.
        // Parse the number.
        int32_t نمبر_اشاریہ=اشاریہ;
        اشاریہ=skipDouble(اشاریہ);
        int32_t length=اشاریہ-نمبر_اشاریہ;
        إذا(length==0) {
            سیٹ_Parse_Error(parseError, شروع);  // Bad choice pattern syntax.
            error_کوڈ=U_نمونہ_SYNTAX_ERROR;
            أرجع 0;
        }
        إذا(length>Part::MAX_لمائی) {
            سیٹ_Parse_Error(parseError, نمبر_اشاریہ);  // Choice number too long.
            error_کوڈ=U_اشاریہ_OUTOFBOUNDS_ERROR;
            أرجع 0;
        }
        parseDouble(نمبر_اشاریہ, اشاریہ, سچ, parseError, error_کوڈ);  // adds ARG_INT or ARG_DOUBLE
        إذا(U_FAILURE(error_کوڈ)) {
            أرجع 0;
        }
        // Parse the separator.
        اشاریہ=skipWhiteSpace(اشاریہ);
        إذا(اشاریہ==پیغام.length()) {
            سیٹ_Parse_Error(parseError, شروع);  // Bad choice pattern syntax.
            error_کوڈ=U_نمونہ_SYNTAX_ERROR;
            أرجع 0;
        }
        char16_t c=پیغام.charAt(اشاریہ);
        إذا(!(c==u_pound || c==u_lessThan || c==u_lessOrEqual)) {  // U+2264 is <=
            سیٹ_Parse_Error(parseError, شروع);  // Expected choice separator (#<\u2264) instead of c.
            error_کوڈ=U_نمونہ_SYNTAX_ERROR;
            أرجع 0;
        }
        جوڑو_Part(UMSGPAT_PART_TYPE_ARG_SELECTOR, اشاریہ, 1, 0, error_کوڈ);
        // Parse the message fragment.
        اشاریہ=parse_سنیہا(++اشاریہ, 0, nestingLevel+1, UMSGPAT_ARG_TYPE_CHOICE, parseError, error_کوڈ);
        إذا(U_FAILURE(error_کوڈ)) {
            أرجع 0;
        }
        // parseMessage(..., CHOICE) returns the index of the terminator, or msg.length().
        إذا(اشاریہ==پیغام.length()) {
            أرجع اشاریہ;
        }
        إذا(پیغام.charAt(اشاریہ)==u_دایاں_Curly_Brace) {
            إذا(!وچ_سنیہا_Format_نمونہ(nestingLevel)) {
                سیٹ_Parse_Error(parseError, شروع);  // Bad choice pattern syntax.
                error_کوڈ=U_نمونہ_SYNTAX_ERROR;
                أرجع 0;
            }
            أرجع اشاریہ;
        }  // else the terminator is '|'
        اشاریہ=skipWhiteSpace(اشاریہ+1);
    }
}

int32_t
سنیہا_نمونہ::parsePluralOrSelectStyle(UMessage_نمونہ_Arg_Type argType,
                                         int32_t اشاریہ, int32_t nestingLevel,
                                         UParseError *parseError, UError_کوڈ &error_کوڈ) {
    إذا(U_FAILURE(error_کوڈ)) {
        أرجع 0;
    }
    int32_t شروع=اشاریہ;
    UBool is_خالی_تھاواں=سچ;
    UBool hasOther=خطأ;
    لكل(;;) {
        // First, collect the selector looking for a small set of terminators.
        // It would be a little faster to consider the syntax of each possible
        // token right here, but that makes the code too complicated.
        اشاریہ=skipWhiteSpace(اشاریہ);
        UBool eos=اشاریہ==پیغام.length();
        إذا(eos || پیغام.charAt(اشاریہ)==u_دایاں_Curly_Brace) {
            إذا(eos==وچ_سنیہا_Format_نمونہ(nestingLevel)) {
                سیٹ_Parse_Error(parseError, شروع);  // Bad plural/select pattern syntax.
                error_کوڈ=U_نمونہ_SYNTAX_ERROR;
                أرجع 0;
            }
            إذا(!hasOther) {
                سیٹ_Parse_Error(parseError, 0);  // Missing 'other' keyword in plural/select pattern.
                error_کوڈ=U_DEFAULT_KEYWORD_MISSING;
                أرجع 0;
            }
            أرجع اشاریہ;
        }
        int32_t selector_اشاریہ=اشاریہ;
        إذا(UMSGPAT_ARG_TYPE_HAS_PLURAL_STYLE(argType) && پیغام.charAt(selector_اشاریہ)==u_equal) {
            // explicit-value plural selector: =double
            اشاریہ=skipDouble(اشاریہ+1);
            int32_t length=اشاریہ-selector_اشاریہ;
            إذا(length==1) {
                سیٹ_Parse_Error(parseError, شروع);  // Bad plural/select pattern syntax.
                error_کوڈ=U_نمونہ_SYNTAX_ERROR;
                أرجع 0;
            }
            إذا(length>Part::MAX_لمائی) {
                سیٹ_Parse_Error(parseError, selector_اشاریہ);  // Argument selector too long.
                error_کوڈ=U_اشاریہ_OUTOFBOUNDS_ERROR;
                أرجع 0;
            }
            جوڑو_Part(UMSGPAT_PART_TYPE_ARG_SELECTOR, selector_اشاریہ, length, 0, error_کوڈ);
            parseDouble(selector_اشاریہ+1, اشاریہ, خطأ,
                        parseError, error_کوڈ);  // adds ARG_INT or ARG_DOUBLE
        } وإلا {
            اشاریہ=skipIdentifier(اشاریہ);
            int32_t length=اشاریہ-selector_اشاریہ;
            إذا(length==0) {
                سیٹ_Parse_Error(parseError, شروع);  // Bad plural/select pattern syntax.
                error_کوڈ=U_نمونہ_SYNTAX_ERROR;
                أرجع 0;
            }
            // Note: The ':' in "offset:" is just beyond the skipIdentifier() range.
            إذا( UMSGPAT_ARG_TYPE_HAS_PLURAL_STYLE(argType) && length==6 && اشاریہ<پیغام.length() &&
                0==پیغام.compare(selector_اشاریہ, 7, k_وتھ_Colon, 0, 7)
            ) {
                // plural offset, not a selector
                إذا(!is_خالی_تھاواں) {
                    // Plural argument 'offset:' (if present) must precede key-message pairs.
                    سیٹ_Parse_Error(parseError, شروع);
                    error_کوڈ=U_نمونہ_SYNTAX_ERROR;
                    أرجع 0;
                }
                // allow whitespace between offset: and its value
                int32_t قدر_اشاریہ=skipWhiteSpace(اشاریہ+1);  // The ':' is at index.
                اشاریہ=skipDouble(قدر_اشاریہ);
                إذا(اشاریہ==قدر_اشاریہ) {
                    سیٹ_Parse_Error(parseError, شروع);  // Missing value for plural 'offset:'.
                    error_کوڈ=U_نمونہ_SYNTAX_ERROR;
                    أرجع 0;
                }
                إذا((اشاریہ-قدر_اشاریہ)>Part::MAX_لمائی) {
                    سیٹ_Parse_Error(parseError, قدر_اشاریہ);  // Plural offset value too long.
                    error_کوڈ=U_اشاریہ_OUTOFBOUNDS_ERROR;
                    أرجع 0;
                }
                parseDouble(قدر_اشاریہ, اشاریہ, خطأ,
                            parseError, error_کوڈ);  // adds ARG_INT or ARG_DOUBLE
                إذا(U_FAILURE(error_کوڈ)) {
                    أرجع 0;
                }
                is_خالی_تھاواں=خطأ;
                استمر;  // no message fragment after the offset
            } وإلا {
                // normal selector word
                إذا(length>Part::MAX_لمائی) {
                    سیٹ_Parse_Error(parseError, selector_اشاریہ);  // Argument selector too long.
                    error_کوڈ=U_اشاریہ_OUTOFBOUNDS_ERROR;
                    أرجع 0;
                }
                جوڑو_Part(UMSGPAT_PART_TYPE_ARG_SELECTOR, selector_اشاریہ, length, 0, error_کوڈ);
                إذا(0==پیغام.compare(selector_اشاریہ, length, kOther, 0, 5)) {
                    hasOther=سچ;
                }
            }
        }
        إذا(U_FAILURE(error_کوڈ)) {
            أرجع 0;
        }

        // parse the message fragment following the selector
        اشاریہ=skipWhiteSpace(اشاریہ);
        إذا(اشاریہ==پیغام.length() || پیغام.charAt(اشاریہ)!=u_بایاں_Curly_Brace) {
            سیٹ_Parse_Error(parseError, selector_اشاریہ);  // No message fragment after plural/select selector.
            error_کوڈ=U_نمونہ_SYNTAX_ERROR;
            أرجع 0;
        }
        اشاریہ=parse_سنیہا(اشاریہ, 1, nestingLevel+1, argType, parseError, error_کوڈ);
        إذا(U_FAILURE(error_کوڈ)) {
            أرجع 0;
        }
        is_خالی_تھاواں=خطأ;
    }
}

int32_t
سنیہا_نمونہ::parse_Arg_نمبر(ثابت UnicodeString &s, int32_t شروع, int32_t حد_بندی) {
    // If the identifier contains only ASCII digits, then it is an argument _number_
    // and must not have leading zeros (except "0" itself).
    // Otherwise it is an argument _name_.
    إذا(شروع>=حد_بندی) {
        أرجع UMSGPAT_ARG_NAME_NOT_VALID;
    }
    int32_t number;
    // Defer numeric errors until we know there are only digits.
    UBool bad_نمبر;
    char16_t c=s.charAt(شروع++);
    إذا(c==0x30) {
        إذا(شروع==حد_بندی) {
            أرجع 0;
        } وإلا {
            number=0;
            bad_نمبر=سچ;  // leading zero
        }
    } وإلا إذا(0x31<=c && c<=0x39) {
        number=c-0x30;
        bad_نمبر=خطأ;
    } وإلا {
        أرجع UMSGPAT_ARG_NAME_NOT_نمبر;
    }
    طالما(شروع<حد_بندی) {
        c=s.charAt(شروع++);
        إذا(0x30<=c && c<=0x39) {
            إذا(number>=INT32_MAX/10) {
                bad_نمبر=سچ;  //حد_توں_ودھ
            } وإلا {
                number=number*10+(c-0x30);
            }
        } وإلا {
            أرجع UMSGPAT_ARG_NAME_NOT_نمبر;
        }
    }
    // There are only ASCII digits.
    إذا(bad_نمبر) {
        أرجع UMSGPAT_ARG_NAME_NOT_VALID;
    } وإلا {
        أرجع number;
    }
}

فراغ
سنیہا_نمونہ::parseDouble(int32_t شروع, int32_t حد_بندی, UBool allowInfinity,
                            UParseError *parseError, UError_کوڈ &error_کوڈ) {
    إذا(U_FAILURE(error_کوڈ)) {
        أرجع;
    }
    U_ASSERT(شروع<حد_بندی);
    // fake loop for easy exit and single throw statement
    لكل(;;) { /*loop doesn't iterate*/
        // fast path for small integers and infinity
        int32_t value=0;
        int32_t isNegative=0;  // not boolean so that we can easily add it to value
        int32_t اشاریہ=شروع;
        char16_t c=پیغام.charAt(اشاریہ++);
        إذا(c==u_minus) {
            isNegative=1;
            إذا(اشاریہ==حد_بندی) {
                اكسر;  // no number
            }
            c=پیغام.charAt(اشاریہ++);
        } وإلا إذا(c==u_plus) {
            إذا(اشاریہ==حد_بندی) {
                اكسر;  // no number
            }
            c=پیغام.charAt(اشاریہ++);
        }
        إذا(c==0x221e) {  // infinity
            إذا(allowInfinity && اشاریہ==حد_بندی) {
                مزدوج infinity=uprv_getInfinity();
                جوڑو_Arg_Double_Part(
                    isNegative!=0 ? -infinity : infinity,
                    شروع, حد_بندی-شروع, error_کوڈ);
                أرجع;
            } وإلا {
                اكسر;
            }
        }
        // try to parse the number as a small integer but fall back to a double
        طالما('0'<=c && c<='9') {
            value=value*10+(c-'0');
            إذا(value>(Part::MAX_قدر+isNegative)) {
                اكسر;  // not a small-enough integer
            }
            إذا(اشاریہ==حد_بندی) {
                جوڑو_Part(UMSGPAT_PART_TYPE_ARG_INT, شروع, حد_بندی-شروع,
                        isNegative!=0 ? -value : value, error_کوڈ);
                أرجع;
            }
            c=پیغام.charAt(اشاریہ++);
        }
        // Let Double.parseDouble() throw a NumberFormatException.
        حرف نمبر_Chars[128];
        int32_t capacity = static_cast<int32_t>(حجم(نمبر_Chars));
        int32_t length=حد_بندی-شروع;
        إذا(length>=capacity) {
            اكسر;  // number too long
        }
        پیغام.extract(شروع, length, نمبر_Chars, capacity, US_INV);
        إذا (static_cast<int32_t>(uprv_strlen(نمبر_Chars)) < length) {
            اكسر;  // contains non-invariant character that was turned into NUL
        }
        حرف *سرا;
        مزدوج numeric_قدر=uprv_strtod(نمبر_Chars, &سرا);
        إذا(سرا!=(نمبر_Chars+length)) {
            اكسر;  // parsing error
        }
        جوڑو_Arg_Double_Part(numeric_قدر, شروع, length, error_کوڈ);
        أرجع;
    }
    سیٹ_Parse_Error(parseError, شروع /*, limit*/);  // Bad syntax for numeric value.
    error_کوڈ=U_نمونہ_SYNTAX_ERROR;
}

int32_t
سنیہا_نمونہ::skipWhiteSpace(int32_t اشاریہ) {
    ثابت char16_t *s=پیغام.get_بفر();
    int32_t پیغام_لمائی=پیغام.length();
    ثابت char16_t *t=نمونہ_Props::skipWhiteSpace(s+اشاریہ, پیغام_لمائی-اشاریہ);
    أرجع static_cast<int32_t>(t - s);
}

int32_t
سنیہا_نمونہ::skipIdentifier(int32_t اشاریہ) {
    ثابت char16_t *s=پیغام.get_بفر();
    int32_t پیغام_لمائی=پیغام.length();
    ثابت char16_t *t=نمونہ_Props::skipIdentifier(s+اشاریہ, پیغام_لمائی-اشاریہ);
    أرجع static_cast<int32_t>(t - s);
}

int32_t
سنیہا_نمونہ::skipDouble(int32_t اشاریہ) {
    int32_t پیغام_لمائی=پیغام.length();
    طالما(اشاریہ<پیغام_لمائی) {
        char16_t c=پیغام.charAt(اشاریہ);
        // U+221E: Allow the infinity symbol, for ChoiceFormat patterns.
        إذا((c<0x30 && c!=u_plus && c!=u_minus && c!=u_dot) || (c>0x39 && c!=u_e && c!=u_E && c!=0x221e)) {
            اكسر;
        }
        ++اشاریہ;
    }
    أرجع اشاریہ;
}

UBool
سنیہا_نمونہ::isArgTypeChar(UChar32 c) {
    أرجع (u_a<=c && c<=u_z) || (u_A<=c && c<=u_Z);
}

UBool
سنیہا_نمونہ::isChoice(int32_t اشاریہ) {
    char16_t c;
    أرجع
        ((c=پیغام.charAt(اشاریہ++))==u_c || c==u_C) &&
        ((c=پیغام.charAt(اشاریہ++))==u_h || c==u_H) &&
        ((c=پیغام.charAt(اشاریہ++))==u_o || c==u_O) &&
        ((c=پیغام.charAt(اشاریہ++))==u_i || c==u_I) &&
        ((c=پیغام.charAt(اشاریہ++))==u_c || c==u_C) &&
        ((c=پیغام.charAt(اشاریہ))==u_e || c==u_E);
}

UBool
سنیہا_نمونہ::isPlural(int32_t اشاریہ) {
    char16_t c;
    أرجع
        ((c=پیغام.charAt(اشاریہ++))==u_p || c==u_P) &&
        ((c=پیغام.charAt(اشاریہ++))==u_l || c==u_L) &&
        ((c=پیغام.charAt(اشاریہ++))==u_u || c==u_U) &&
        ((c=پیغام.charAt(اشاریہ++))==u_r || c==u_R) &&
        ((c=پیغام.charAt(اشاریہ++))==u_a || c==u_A) &&
        ((c=پیغام.charAt(اشاریہ))==u_l || c==u_L);
}

UBool
سنیہا_نمونہ::isSelect(int32_t اشاریہ) {
    char16_t c;
    أرجع
        ((c=پیغام.charAt(اشاریہ++))==u_s || c==u_S) &&
        ((c=پیغام.charAt(اشاریہ++))==u_e || c==u_E) &&
        ((c=پیغام.charAt(اشاریہ++))==u_l || c==u_L) &&
        ((c=پیغام.charAt(اشاریہ++))==u_e || c==u_E) &&
        ((c=پیغام.charAt(اشاریہ++))==u_c || c==u_C) &&
        ((c=پیغام.charAt(اشاریہ))==u_t || c==u_T);
}

UBool
سنیہا_نمونہ::isOrdinal(int32_t اشاریہ) {
    char16_t c;
    أرجع
        ((c=پیغام.charAt(اشاریہ++))==u_o || c==u_O) &&
        ((c=پیغام.charAt(اشاریہ++))==u_r || c==u_R) &&
        ((c=پیغام.charAt(اشاریہ++))==u_d || c==u_D) &&
        ((c=پیغام.charAt(اشاریہ++))==u_i || c==u_I) &&
        ((c=پیغام.charAt(اشاریہ++))==u_n || c==u_N) &&
        ((c=پیغام.charAt(اشاریہ++))==u_a || c==u_A) &&
        ((c=پیغام.charAt(اشاریہ))==u_l || c==u_L);
}

UBool
سنیہا_نمونہ::وچ_سنیہا_Format_نمونہ(int32_t nestingLevel) {
    أرجع nestingLevel>0 || partsList->a[0].type==UMSGPAT_PART_TYPE_پیغام_شروع;
}

UBool
سنیہا_نمونہ::وچ_چوٹی_Level_Choice_سنیہا(int32_t nestingLevel, UMessage_نمونہ_Arg_Type ماپا_Type) {
    أرجع
        nestingLevel==1 &&
        ماپا_Type==UMSGPAT_ARG_TYPE_CHOICE &&
        partsList->a[0].type!=UMSGPAT_PART_TYPE_پیغام_شروع;
}

فراغ
سنیہا_نمونہ::جوڑو_Part(UMessage_نمونہ_Part_Type type, int32_t اشاریہ, int32_t length,
                        int32_t value, UError_کوڈ &error_کوڈ) {
    إذا(partsList->ensure_سمرتھا_For_One_ہور(parts_لمائی, error_کوڈ)) {
        Part &part=partsList->a[parts_لمائی++];
        part.type=type;
        part.اشاریہ=اشاریہ;
        part.length = static_cast<uint16_t>(length);
        part.value = static_cast<int16_t>(value);
        part.حد_بندی_Part_اشاریہ=0;
    }
}

فراغ
سنیہا_نمونہ::جوڑو_حد_بندی_Part(int32_t شروع,
                             UMessage_نمونہ_Part_Type type, int32_t اشاریہ, int32_t length,
                             int32_t value, UError_کوڈ &error_کوڈ) {
    partsList->a[شروع].حد_بندی_Part_اشاریہ=parts_لمائی;
    جوڑو_Part(type, اشاریہ, length, value, error_کوڈ);
}

فراغ
سنیہا_نمونہ::جوڑو_Arg_Double_Part(مزدوج numeric_قدر, int32_t شروع, int32_t length,
                                 UError_کوڈ &error_کوڈ) {
    إذا(U_FAILURE(error_کوڈ)) {
        أرجع;
    }
    int32_t numeric_اشاریہ=numeric_اقدار_لمائی;
    إذا(numeric_اقدار_List==خالی_سوچک) {
        numeric_اقدار_List=جديد سنیہا_نمونہ_Double_List();
        إذا(numeric_اقدار_List==خالی_سوچک) {
            error_کوڈ=U_حافظہ_ونڈ_ERROR;
            أرجع;
        }
    } وإلا إذا(!numeric_اقدار_List->ensure_سمرتھا_For_One_ہور(numeric_اقدار_لمائی, error_کوڈ)) {
        أرجع;
    } وإلا {
        إذا(numeric_اشاریہ>Part::MAX_قدر) {
            error_کوڈ=U_اشاریہ_OUTOFBOUNDS_ERROR;
            أرجع;
        }
    }
    numeric_اقدار_List->a[numeric_اقدار_لمائی++]=numeric_قدر;
    جوڑو_Part(UMSGPAT_PART_TYPE_ARG_DOUBLE, شروع, length, numeric_اشاریہ, error_کوڈ);
}

فراغ
سنیہا_نمونہ::سیٹ_Parse_Error(UParseError *parseError, int32_t اشاریہ) {
    إذا(parseError==خالی_سوچک) {
        أرجع;
    }
    parseError->وتھ=اشاریہ;

    // Set preContext to some of msg before index.
    // Avoid splitting a surrogate pair.
    int32_t length=اشاریہ;
    إذا(length>=U_PARSE_CONTEXT_LEN) {
        length=U_PARSE_CONTEXT_LEN-1;
        إذا(length>0 && U16_IS_TRAIL(پیغام[اشاریہ-length])) {
            --length;
        }
    }
    پیغام.extract(اشاریہ-length, length, parseError->preContext);
    parseError->preContext[length]=0;

    // Set postContext to some of msg starting at index.
    length=پیغام.length()-اشاریہ;
    إذا(length>=U_PARSE_CONTEXT_LEN) {
        length=U_PARSE_CONTEXT_LEN-1;
        إذا(length>0 && U16_IS_LEAD(پیغام[اشاریہ+length-1])) {
            --length;
        }
    }
    پیغام.extract(اشاریہ, length, parseError->postContext);
    parseError->postContext[length]=0;
}

// MessageImpl ------------------------------------------------------------- ***

فراغ
سنیہا_Impl::appendReducedApostrophes(ثابت UnicodeString &s, int32_t شروع, int32_t حد_بندی,
                                      UnicodeString &sb) {
    int32_t doubleApos=-1;
    لكل(;;) {
        int32_t i=s.اشاریہ_دا(u_apos, شروع);
        إذا(i<0 || i>=حد_بندی) {
            sb.append(s, شروع, حد_بندی-شروع);
            اكسر;
        }
        إذا(i==doubleApos) {
            // Double apostrophe at start-1 and start==i, append one.
            sb.append(u_apos);
            ++شروع;
            doubleApos=-1;
        } وإلا {
            // Append text between apostrophes and skip this one.
            sb.append(s, شروع, i-شروع);
            doubleApos=شروع=i+1;
        }
    }
}

// Ported from second half of ICU4J SelectFormat.format(String).
UnicodeString&
سنیہا_Impl::append_Sub_سنیہا_Without_Skip_Syntax(ثابت سنیہا_نمونہ& پیغام_نمونہ,
                                               int32_t پیغام_شروع,
                                               UnicodeString& نتیجہ U_LIFETIME_BOUND) {
    ثابت UnicodeString &پیغام_String=پیغام_نمونہ.get_نمونہ_String();
    int32_t prev_اشاریہ=پیغام_نمونہ.getPart(پیغام_شروع).get_حد_بندی();
    لكل(int32_t i=پیغام_شروع;;) {
        ثابت سنیہا_نمونہ::Part &part=پیغام_نمونہ.getPart(++i);
        UMessage_نمونہ_Part_Type type=part.getType();
        int32_t اشاریہ=part.get_اشاریہ();
        إذا(type==UMSGPAT_PART_TYPE_پیغام_حد_بندی) {
            أرجع نتیجہ.append(پیغام_String, prev_اشاریہ, اشاریہ-prev_اشاریہ);
        } وإلا إذا(type==UMSGPAT_PART_TYPE_SKIP_SYNTAX) {
            نتیجہ.append(پیغام_String, prev_اشاریہ, اشاریہ-prev_اشاریہ);
            prev_اشاریہ=part.get_حد_بندی();
        } وإلا إذا(type==UMSGPAT_PART_TYPE_ARG_شروع) {
            نتیجہ.append(پیغام_String, prev_اشاریہ, اشاریہ-prev_اشاریہ);
            prev_اشاریہ=اشاریہ;
            i=پیغام_نمونہ.get_حد_بندی_Part_اشاریہ(i);
            اشاریہ=پیغام_نمونہ.getPart(i).get_حد_بندی();
            appendReducedApostrophes(پیغام_String, prev_اشاریہ, اشاریہ, نتیجہ);
            prev_اشاریہ=اشاریہ;
        }
    }
}

U_NAMESPACE_سرا

#جے_انت  // !UCONFIG_NO_FORMATTING
