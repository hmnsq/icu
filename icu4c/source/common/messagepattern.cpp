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

#شامل "unicode/utypes.h"

#اگر !UCONFIG_NO_FORMATTING

#شامل "unicode/messagepattern.h"
#شامل "unicode/unistr.h"
#شامل "unicode/utf16.h"
#شامل "cmemory.h"
#شامل "cstring.h"
#شامل "messageimpl.h"
#شامل "patternprops.h"
#شامل "putilimp.h"
#شامل "uassert.h"

U_NAMESPACE_BEGIN

// Unicode character/code point constants ---------------------------------- ***

ساکن ثابت char16_t u_pound=0x23;
ساکن ثابت char16_t u_apos=0x27;
ساکن ثابت char16_t u_plus=0x2B;
ساکن ثابت char16_t u_comma=0x2C;
ساکن ثابت char16_t u_minus=0x2D;
ساکن ثابت char16_t u_dot=0x2E;
ساکن ثابت char16_t u_colon=0x3A;
ساکن ثابت char16_t u_lessThan=0x3C;
ساکن ثابت char16_t u_equal=0x3D;
ساکن ثابت char16_t u_A=0x41;
ساکن ثابت char16_t u_C=0x43;
ساکن ثابت char16_t u_D=0x44;
ساکن ثابت char16_t u_E=0x45;
ساکن ثابت char16_t u_H=0x48;
ساکن ثابت char16_t u_I=0x49;
ساکن ثابت char16_t u_L=0x4C;
ساکن ثابت char16_t u_N=0x4E;
ساکن ثابت char16_t u_O=0x4F;
ساکن ثابت char16_t u_P=0x50;
ساکن ثابت char16_t u_R=0x52;
ساکن ثابت char16_t u_S=0x53;
ساکن ثابت char16_t u_T=0x54;
ساکن ثابت char16_t u_U=0x55;
ساکن ثابت char16_t u_Z=0x5A;
ساکن ثابت char16_t u_a=0x61;
ساکن ثابت char16_t u_c=0x63;
ساکن ثابت char16_t u_d=0x64;
ساکن ثابت char16_t u_e=0x65;
ساکن ثابت char16_t u_f=0x66;
ساکن ثابت char16_t u_h=0x68;
ساکن ثابت char16_t u_i=0x69;
ساکن ثابت char16_t u_l=0x6C;
ساکن ثابت char16_t u_n=0x6E;
ساکن ثابت char16_t u_o=0x6F;
ساکن ثابت char16_t u_p=0x70;
ساکن ثابت char16_t u_r=0x72;
ساکن ثابت char16_t u_s=0x73;
ساکن ثابت char16_t u_t=0x74;
ساکن ثابت char16_t u_u=0x75;
ساکن ثابت char16_t u_z=0x7A;
ساکن ثابت char16_t u_بایاں_Curly_Brace=0x7B;
ساکن ثابت char16_t u_pipe=0x7C;
ساکن ثابت char16_t u_دایاں_Curly_Brace=0x7D;
ساکن ثابت char16_t u_lessOrEqual=0x2264;  // U+2264 is <=

ساکن ثابت char16_t k_انحراف_Colon[]={  // "offset:"
    u_o, u_f, u_f, u_s, u_e, u_t, u_colon
};

ساکن ثابت char16_t kOther[]={  // "other"
    u_o, u_t, u_h, u_e, u_r
};

// MessagePatternList ------------------------------------------------------ ***

سانچہ<قسم_نام T, int32_t ڈھیر_استعداد>
جماعت پیغام_نمونہ_List : عوامی UMemory {
عوامی:
    پیغام_نمونہ_List() {}
    خالی copyFrom(ثابت پیغام_نمونہ_List<T, ڈھیر_استعداد> &other,
                  int32_t length,
                  UError_رمز &error_رمز);
    UBool ensure_استعداد_For_One_مزید(int32_t old_لمبائی, UError_رمز &error_رمز);
    UBool equals(ثابت پیغام_نمونہ_List<T, ڈھیر_استعداد> &other, int32_t length) ثابت {
        برائے(int32_t i=0; i<length; ++i) {
            اگر(a[i]!=other.a[i]) { واپس جھوٹ; }
        }
        واپس سچ;
    }

    Maybe_ڈھیر_Array<T, ڈھیر_استعداد> a;
};

سانچہ<قسم_نام T, int32_t ڈھیر_استعداد>
خالی
پیغام_نمونہ_List<T, ڈھیر_استعداد>::copyFrom(
        ثابت پیغام_نمونہ_List<T, ڈھیر_استعداد> &other,
        int32_t length,
        UError_رمز &error_رمز) {
    اگر(U_SUCCESS(error_رمز) && length>0) {
        اگر(length>a.get_استعداد() && خالی_اشاریہ==a.resize(length)) {
            error_رمز=U_حافظہ_تخصیص_ERROR;
            واپس;
        }
        uprv_memcpy(a.getAlias(), other.a.getAlias(), (size_t)length*حجم(T));
    }
}

سانچہ<قسم_نام T, int32_t ڈھیر_استعداد>
UBool
پیغام_نمونہ_List<T, ڈھیر_استعداد>::ensure_استعداد_For_One_مزید(int32_t old_لمبائی, UError_رمز &error_رمز) {
    اگر(U_FAILURE(error_رمز)) {
        واپس جھوٹ;
    }
    اگر(a.get_استعداد()>old_لمبائی || a.resize(2*old_لمبائی, old_لمبائی)!=خالی_اشاریہ) {
        واپس سچ;
    }
    error_رمز=U_حافظہ_تخصیص_ERROR;
    واپس جھوٹ;
}

// MessagePatternList specializations -------------------------------------- ***

جماعت پیغام_نمونہ_Double_List : عوامی پیغام_نمونہ_List<دوہرا, 8> {
};

جماعت پیغام_نمونہ_Parts_List : عوامی پیغام_نمونہ_List<پیغام_نمونہ::Part, 32> {
};

// MessagePattern constructors etc. ---------------------------------------- ***

پیغام_نمونہ::پیغام_نمونہ(UError_رمز &error_رمز)
        : apos_منوال(UCONFIG_MSGPAT_DEFAULT_APOSTROPHE_منوال),
          partsList(خالی_اشاریہ), parts(خالی_اشاریہ), parts_لمبائی(0),
          numeric_اقدار_List(خالی_اشاریہ), numeric_اقدار(خالی_اشاریہ), numeric_اقدار_لمبائی(0),
          has_Arg_نام(جھوٹ), hasArgNumbers(جھوٹ), needsAutoQuoting(جھوٹ) {
    init(error_رمز);
}

پیغام_نمونہ::پیغام_نمونہ(UMessage_نمونہ_Apostrophe_منوال منوال, UError_رمز &error_رمز)
        : apos_منوال(منوال),
          partsList(خالی_اشاریہ), parts(خالی_اشاریہ), parts_لمبائی(0),
          numeric_اقدار_List(خالی_اشاریہ), numeric_اقدار(خالی_اشاریہ), numeric_اقدار_لمبائی(0),
          has_Arg_نام(جھوٹ), hasArgNumbers(جھوٹ), needsAutoQuoting(جھوٹ) {
    init(error_رمز);
}

پیغام_نمونہ::پیغام_نمونہ(ثابت UnicodeString &نمونہ, UParseError *parseError, UError_رمز &error_رمز)
        : apos_منوال(UCONFIG_MSGPAT_DEFAULT_APOSTROPHE_منوال),
          partsList(خالی_اشاریہ), parts(خالی_اشاریہ), parts_لمبائی(0),
          numeric_اقدار_List(خالی_اشاریہ), numeric_اقدار(خالی_اشاریہ), numeric_اقدار_لمبائی(0),
          has_Arg_نام(جھوٹ), hasArgNumbers(جھوٹ), needsAutoQuoting(جھوٹ) {
    اگر(init(error_رمز)) {
        parse(نمونہ, parseError, error_رمز);
    }
}

UBool
پیغام_نمونہ::init(UError_رمز &error_رمز) {
    اگر(U_FAILURE(error_رمز)) {
        واپس جھوٹ;
    }
    partsList=نیا پیغام_نمونہ_Parts_List();
    اگر(partsList==خالی_اشاریہ) {
        error_رمز=U_حافظہ_تخصیص_ERROR;
        واپس جھوٹ;
    }
    parts=partsList->a.getAlias();
    واپس سچ;
}

پیغام_نمونہ::پیغام_نمونہ(ثابت پیغام_نمونہ &other)
        : UObject(other), apos_منوال(other.apos_منوال), پیغام(other.پیغام),
          partsList(خالی_اشاریہ), parts(خالی_اشاریہ), parts_لمبائی(0),
          numeric_اقدار_List(خالی_اشاریہ), numeric_اقدار(خالی_اشاریہ), numeric_اقدار_لمبائی(0),
          has_Arg_نام(other.has_Arg_نام), hasArgNumbers(other.hasArgNumbers),
          needsAutoQuoting(other.needsAutoQuoting) {
    UError_رمز error_رمز=U_ZERO_ERROR;
    اگر(!copyStorage(other, error_رمز)) {
        clear();
    }
}

پیغام_نمونہ&
پیغام_نمونہ::عامل=(ثابت پیغام_نمونہ& other) U_LIFETIME_BOUND {
    اگر(یہ==&other) {
        واپس *یہ;
    }
    apos_منوال=other.apos_منوال;
    پیغام=other.پیغام;
    has_Arg_نام=other.has_Arg_نام;
    hasArgNumbers=other.hasArgNumbers;
    needsAutoQuoting=other.needsAutoQuoting;
    UError_رمز error_رمز=U_ZERO_ERROR;
    اگر(!copyStorage(other, error_رمز)) {
        clear();
    }
    واپس *یہ;
}

UBool
پیغام_نمونہ::copyStorage(ثابت پیغام_نمونہ &other, UError_رمز &error_رمز) {
    اگر(U_FAILURE(error_رمز)) {
        واپس جھوٹ;
    }
    parts=خالی_اشاریہ;
    parts_لمبائی=0;
    numeric_اقدار=خالی_اشاریہ;
    numeric_اقدار_لمبائی=0;
    اگر(partsList==خالی_اشاریہ) {
        partsList=نیا پیغام_نمونہ_Parts_List();
        اگر(partsList==خالی_اشاریہ) {
            error_رمز=U_حافظہ_تخصیص_ERROR;
            واپس جھوٹ;
        }
        parts=partsList->a.getAlias();
    }
    اگر(other.parts_لمبائی>0) {
        partsList->copyFrom(*other.partsList, other.parts_لمبائی, error_رمز);
        اگر(U_FAILURE(error_رمز)) {
            واپس جھوٹ;
        }
        parts=partsList->a.getAlias();
        parts_لمبائی=other.parts_لمبائی;
    }
    اگر(other.numeric_اقدار_لمبائی>0) {
        اگر(numeric_اقدار_List==خالی_اشاریہ) {
            numeric_اقدار_List=نیا پیغام_نمونہ_Double_List();
            اگر(numeric_اقدار_List==خالی_اشاریہ) {
                error_رمز=U_حافظہ_تخصیص_ERROR;
                واپس جھوٹ;
            }
            numeric_اقدار=numeric_اقدار_List->a.getAlias();
        }
        numeric_اقدار_List->copyFrom(
            *other.numeric_اقدار_List, other.numeric_اقدار_لمبائی, error_رمز);
        اگر(U_FAILURE(error_رمز)) {
            واپس جھوٹ;
        }
        numeric_اقدار=numeric_اقدار_List->a.getAlias();
        numeric_اقدار_لمبائی=other.numeric_اقدار_لمبائی;
    }
    واپس سچ;
}

پیغام_نمونہ::~پیغام_نمونہ() {
    حذف partsList;
    حذف numeric_اقدار_List;
}

// MessagePattern API ------------------------------------------------------ ***

پیغام_نمونہ&
پیغام_نمونہ::parse(ثابت UnicodeString& نمونہ,
                      UParseError* parseError,
                      UError_رمز& error_رمز) U_LIFETIME_BOUND {
    preParse(نمونہ, parseError, error_رمز);
    parse_پیغام(0, 0, 0, UMSGPAT_ARG_TYPE_NONE, parseError, error_رمز);
    postParse();
    واپس *یہ;
}

پیغام_نمونہ&
پیغام_نمونہ::parseChoiceStyle(ثابت UnicodeString& نمونہ,
                                 UParseError* parseError,
                                 UError_رمز& error_رمز) U_LIFETIME_BOUND {
    preParse(نمونہ, parseError, error_رمز);
    parseChoiceStyle(0, 0, parseError, error_رمز);
    postParse();
    واپس *یہ;
}

پیغام_نمونہ&
پیغام_نمونہ::parsePluralStyle(ثابت UnicodeString& نمونہ,
                                 UParseError* parseError,
                                 UError_رمز& error_رمز) U_LIFETIME_BOUND {
    preParse(نمونہ, parseError, error_رمز);
    parsePluralOrSelectStyle(UMSGPAT_ARG_TYPE_PLURAL, 0, 0, parseError, error_رمز);
    postParse();
    واپس *یہ;
}

پیغام_نمونہ&
پیغام_نمونہ::parseSelectStyle(ثابت UnicodeString& نمونہ,
                                 UParseError* parseError,
                                 UError_رمز& error_رمز) U_LIFETIME_BOUND {
    preParse(نمونہ, parseError, error_رمز);
    parsePluralOrSelectStyle(UMSGPAT_ARG_TYPE_SELECT, 0, 0, parseError, error_رمز);
    postParse();
    واپس *یہ;
}

خالی
پیغام_نمونہ::clear() {
    // Mostly the same as preParse().
    پیغام.remove();
    has_Arg_نام=hasArgNumbers=جھوٹ;
    needsAutoQuoting=جھوٹ;
    parts_لمبائی=0;
    numeric_اقدار_لمبائی=0;
}

منطقی
پیغام_نمونہ::عامل==(ثابت پیغام_نمونہ &other) ثابت {
    اگر(یہ==&other) {
        واپس سچ;
    }
    واپس
        apos_منوال==other.apos_منوال &&
        پیغام==other.پیغام &&
        // parts.equals(o.parts)
        parts_لمبائی==other.parts_لمبائی &&
        (parts_لمبائی==0 || partsList->equals(*other.partsList, parts_لمبائی));
    // No need to compare numericValues if msg and parts are the same.
}

int32_t
پیغام_نمونہ::hash_رمز() ثابت {
    int32_t hash=(apos_منوال*37+پیغام.hash_رمز())*37+parts_لمبائی;
    برائے(int32_t i=0; i<parts_لمبائی; ++i) {
        hash=hash*37+parts[i].hash_رمز();
    }
    واپس hash;
}

int32_t
پیغام_نمونہ::validate_دلیل_Name(ثابت UnicodeString &name) {
    اگر(!نمونہ_Props::isIdentifier(name.get_بفر(), name.length())) {
        واپس UMSGPAT_ARG_NAME_NOT_VALID;
    }
    واپس parse_Arg_نمبر(name, 0, name.length());
}

UnicodeString
پیغام_نمونہ::autoQuoteApostropheDeep() ثابت {
    اگر(!needsAutoQuoting) {
        واپس پیغام;
    }
    UnicodeString modified(پیغام);
    // Iterate backward so that the insertion indexes do not change.
    int32_t count=گنتی_Parts();
    برائے(int32_t i=count; i>0;) {
        ثابت Part &part=getPart(--i);
        اگر(part.getType()==UMSGPAT_PART_TYPE_داخل_کرو_CHAR) {
           modified.insert(part.اشاریہ, static_cast<char16_t>(part.value));
        }
    }
    واپس modified;
}

دوہرا
پیغام_نمونہ::get_Numeric_قدر(ثابت Part &part) ثابت {
    UMessage_نمونہ_Part_Type type=part.type;
    اگر(type==UMSGPAT_PART_TYPE_ARG_INT) {
        واپس part.value;
    } ورنہ اگر(type==UMSGPAT_PART_TYPE_ARG_DOUBLE) {
        واپس numeric_اقدار[part.value];
    } ورنہ {
        واپس UMSGPAT_NO_NUMERIC_قدر;
    }
}

/**
  * Returns the "offset:" value of a PluralFormat argument, or 0 if none is specified.
  * @param pluralStart the index of the first PluralFormat argument style part. (0..countParts()-1)
  * @return the "offset:" value.
  * @draft ICU 4.8
  */
دوہرا
پیغام_نمونہ::get_Plural_انحراف(int32_t plural_آغاز) ثابت {
    ثابت Part &part=getPart(plural_آغاز);
    اگر(Part::has_Numeric_قدر(part.type)) {
        واپس get_Numeric_قدر(part);
    } ورنہ {
        واپس 0;
    }
}

// MessagePattern::Part ---------------------------------------------------- ***

منطقی
پیغام_نمونہ::Part::عامل==(ثابت Part &other) ثابت {
    اگر(یہ==&other) {
        واپس سچ;
    }
    واپس
        type==other.type &&
        اشاریہ==other.اشاریہ &&
        length==other.length &&
        value==other.value &&
        حد_بندی_Part_اشاریہ==other.حد_بندی_Part_اشاریہ;
}

// MessagePattern parser --------------------------------------------------- ***

خالی
پیغام_نمونہ::preParse(ثابت UnicodeString &نمونہ, UParseError *parseError, UError_رمز &error_رمز) {
    اگر(U_FAILURE(error_رمز)) {
        واپس;
    }
    اگر(parseError!=خالی_اشاریہ) {
        parseError->سطر=0;
        parseError->انحراف=0;
        parseError->preContext[0]=0;
        parseError->postContext[0]=0;
    }
    پیغام=نمونہ;
    has_Arg_نام=hasArgNumbers=جھوٹ;
    needsAutoQuoting=جھوٹ;
    parts_لمبائی=0;
    numeric_اقدار_لمبائی=0;
}

خالی
پیغام_نمونہ::postParse() {
    اگر(partsList!=خالی_اشاریہ) {
        parts=partsList->a.getAlias();
    }
    اگر(numeric_اقدار_List!=خالی_اشاریہ) {
        numeric_اقدار=numeric_اقدار_List->a.getAlias();
    }
}

int32_t
پیغام_نمونہ::parse_پیغام(int32_t اشاریہ, int32_t پیغام_آغاز_لمبائی,
                             int32_t nestingLevel, UMessage_نمونہ_Arg_Type والد_Type,
                             UParseError *parseError, UError_رمز &error_رمز) {
    اگر(U_FAILURE(error_رمز)) {
        واپس 0;
    }
    اگر(nestingLevel>Part::MAX_NESTED_سطحیں) {
        error_رمز=U_اشاریہ_OUTOFBOUNDS_ERROR;
        واپس 0;
    }
    int32_t پیغام_آغاز=parts_لمبائی;
    جوڑو_Part(UMSGPAT_PART_TYPE_پیغام_آغاز, اشاریہ, پیغام_آغاز_لمبائی, nestingLevel, error_رمز);
    اشاریہ+=پیغام_آغاز_لمبائی;
    برائے(;;) {  // while(index<msg.length()) with U_FAILURE(errorCode) check
        اگر(U_FAILURE(error_رمز)) {
            واپس 0;
        }
        اگر(اشاریہ>=پیغام.length()) {
            توڑو;
        }
        char16_t c=پیغام.charAt(اشاریہ++);
        اگر(c==u_apos) {
            اگر(اشاریہ==پیغام.length()) {
                // The apostrophe is the last character in the pattern. 
                // Add a Part for auto-quoting.
                جوڑو_Part(UMSGPAT_PART_TYPE_داخل_کرو_CHAR, اشاریہ, 0,
                        u_apos, error_رمز);  // value=char to be inserted
                needsAutoQuoting=سچ;
            } ورنہ {
                c=پیغام.charAt(اشاریہ);
                اگر(c==u_apos) {
                    // double apostrophe, skip the second one
                    جوڑو_Part(UMSGPAT_PART_TYPE_SKIP_SYNTAX, اشاریہ++, 1, 0, error_رمز);
                } ورنہ اگر(
                    apos_منوال==UMSGPAT_APOS_DOUBLE_REQUIRED ||
                    c==u_بایاں_Curly_Brace || c==u_دایاں_Curly_Brace ||
                    (والد_Type==UMSGPAT_ARG_TYPE_CHOICE && c==u_pipe) ||
                    (UMSGPAT_ARG_TYPE_HAS_PLURAL_STYLE(والد_Type) && c==u_pound)
                ) {
                    // skip the quote-starting apostrophe
                    جوڑو_Part(UMSGPAT_PART_TYPE_SKIP_SYNTAX, اشاریہ-1, 1, 0, error_رمز);
                    // find the end of the quoted literal text
                    برائے(;;) {
                        اشاریہ=پیغام.اشاریہ_کا(u_apos, اشاریہ+1);
                        اگر(اشاریہ>=0) {
                            اگر(/*(index+1)<msg.length() &&*/ پیغام.charAt(اشاریہ+1)==u_apos) {
                                // double apostrophe inside quoted literal text
                                // still encodes a single apostrophe, skip the second one
                                جوڑو_Part(UMSGPAT_PART_TYPE_SKIP_SYNTAX, ++اشاریہ, 1, 0, error_رمز);
                            } ورنہ {
                                // skip the quote-ending apostrophe
                                جوڑو_Part(UMSGPAT_PART_TYPE_SKIP_SYNTAX, اشاریہ++, 1, 0, error_رمز);
                                توڑو;
                            }
                        } ورنہ {
                            // The quoted text reaches to the end of the of the message.
                            اشاریہ=پیغام.length();
                            // Add a Part for auto-quoting.
                            جوڑو_Part(UMSGPAT_PART_TYPE_داخل_کرو_CHAR, اشاریہ, 0,
                                    u_apos, error_رمز);  // value=char to be inserted
                            needsAutoQuoting=سچ;
                            توڑو;
                        }
                    }
                } ورنہ {
                    // Interpret the apostrophe as literal text.
                    // Add a Part for auto-quoting.
                    جوڑو_Part(UMSGPAT_PART_TYPE_داخل_کرو_CHAR, اشاریہ, 0,
                            u_apos, error_رمز);  // value=char to be inserted
                    needsAutoQuoting=سچ;
                }
            }
        } ورنہ اگر(UMSGPAT_ARG_TYPE_HAS_PLURAL_STYLE(والد_Type) && c==u_pound) {
            // The unquoted # in a plural message fragment will be replaced
            // with the (number-offset).
            جوڑو_Part(UMSGPAT_PART_TYPE_REPLACE_نمبر, اشاریہ-1, 1, 0, error_رمز);
        } ورنہ اگر(c==u_بایاں_Curly_Brace) {
            اشاریہ=parseArg(اشاریہ-1, 1, nestingLevel, parseError, error_رمز);
        } ورنہ اگر((nestingLevel>0 && c==u_دایاں_Curly_Brace) ||
                  (والد_Type==UMSGPAT_ARG_TYPE_CHOICE && c==u_pipe)) {
            // Finish the message before the terminator.
            // In a choice style, report the "}" substring only for the following ARG_LIMIT,
            // not for this MSG_LIMIT.
            int32_t حد_بندی_لمبائی=(والد_Type==UMSGPAT_ARG_TYPE_CHOICE && c==u_دایاں_Curly_Brace) ? 0 : 1;
            جوڑو_حد_بندی_Part(پیغام_آغاز, UMSGPAT_PART_TYPE_پیغام_حد_بندی, اشاریہ-1, حد_بندی_لمبائی,
                         nestingLevel, error_رمز);
            اگر(والد_Type==UMSGPAT_ARG_TYPE_CHOICE) {
                // Let the choice style parser see the '}' or '|'.
                واپس اشاریہ-1;
            } ورنہ {
                // continue parsing after the '}'
                واپس اشاریہ;
            }
        }  // else: c is part of literal text
    }
    اگر(nestingLevel>0 && !اندر_چوٹی_Level_Choice_پیغام(nestingLevel, والد_Type)) {
        سیٹ_Parse_Error(parseError, 0);  // Unmatched '{' braces in message.
        error_رمز=U_UNMATCHED_BRACES;
        واپس 0;
    }
    جوڑو_حد_بندی_Part(پیغام_آغاز, UMSGPAT_PART_TYPE_پیغام_حد_بندی, اشاریہ, 0, nestingLevel, error_رمز);
    واپس اشاریہ;
}

int32_t
پیغام_نمونہ::parseArg(int32_t اشاریہ, int32_t arg_آغاز_لمبائی, int32_t nestingLevel,
                         UParseError *parseError, UError_رمز &error_رمز) {
    int32_t arg_آغاز=parts_لمبائی;
    UMessage_نمونہ_Arg_Type argType=UMSGPAT_ARG_TYPE_NONE;
    جوڑو_Part(UMSGPAT_PART_TYPE_ARG_آغاز, اشاریہ, arg_آغاز_لمبائی, argType, error_رمز);
    اگر(U_FAILURE(error_رمز)) {
        واپس 0;
    }
    int32_t name_اشاریہ=اشاریہ=skipWhiteSpace(اشاریہ+arg_آغاز_لمبائی);
    اگر(اشاریہ==پیغام.length()) {
        سیٹ_Parse_Error(parseError, 0);  // Unmatched '{' braces in message.
        error_رمز=U_UNMATCHED_BRACES;
        واپس 0;
    }
    // parse argument name or number
    اشاریہ=skipIdentifier(اشاریہ);
    int32_t number=parse_Arg_نمبر(name_اشاریہ, اشاریہ);
    اگر(number>=0) {
        int32_t length=اشاریہ-name_اشاریہ;
        اگر(length>Part::MAX_لمبائی || number>Part::MAX_قدر) {
            سیٹ_Parse_Error(parseError, name_اشاریہ);  // Argument number too large.
            error_رمز=U_اشاریہ_OUTOFBOUNDS_ERROR;
            واپس 0;
        }
        hasArgNumbers=سچ;
        جوڑو_Part(UMSGPAT_PART_TYPE_ARG_نمبر, name_اشاریہ, length, number, error_رمز);
    } ورنہ اگر(number==UMSGPAT_ARG_NAME_NOT_نمبر) {
        int32_t length=اشاریہ-name_اشاریہ;
        اگر(length>Part::MAX_لمبائی) {
            سیٹ_Parse_Error(parseError, name_اشاریہ);  // Argument name too long.
            error_رمز=U_اشاریہ_OUTOFBOUNDS_ERROR;
            واپس 0;
        }
        has_Arg_نام=سچ;
        جوڑو_Part(UMSGPAT_PART_TYPE_ARG_NAME, name_اشاریہ, length, 0, error_رمز);
    } ورنہ {  // number<-1 (ARG_NAME_NOT_VALID)
        سیٹ_Parse_Error(parseError, name_اشاریہ);  // Bad argument syntax.
        error_رمز=U_نمونہ_SYNTAX_ERROR;
        واپس 0;
    }
    اشاریہ=skipWhiteSpace(اشاریہ);
    اگر(اشاریہ==پیغام.length()) {
        سیٹ_Parse_Error(parseError, 0);  // Unmatched '{' braces in message.
        error_رمز=U_UNMATCHED_BRACES;
        واپس 0;
    }
    char16_t c=پیغام.charAt(اشاریہ);
    اگر(c==u_دایاں_Curly_Brace) {
        // all done
    } ورنہ اگر(c!=u_comma) {
        سیٹ_Parse_Error(parseError, name_اشاریہ);  // Bad argument syntax.
        error_رمز=U_نمونہ_SYNTAX_ERROR;
        واپس 0;
    } ورنہ /* ',' */ {
        // parse argument type: case-sensitive a-zA-Z
        int32_t type_اشاریہ=اشاریہ=skipWhiteSpace(اشاریہ+1);
        جب(اشاریہ<پیغام.length() && isArgTypeChar(پیغام.charAt(اشاریہ))) {
            ++اشاریہ;
        }
        int32_t length=اشاریہ-type_اشاریہ;
        اشاریہ=skipWhiteSpace(اشاریہ);
        اگر(اشاریہ==پیغام.length()) {
            سیٹ_Parse_Error(parseError, 0);  // Unmatched '{' braces in message.
            error_رمز=U_UNMATCHED_BRACES;
            واپس 0;
        }
        اگر(length==0 || ((c=پیغام.charAt(اشاریہ))!=u_comma && c!=u_دایاں_Curly_Brace)) {
            سیٹ_Parse_Error(parseError, name_اشاریہ);  // Bad argument syntax.
            error_رمز=U_نمونہ_SYNTAX_ERROR;
            واپس 0;
        }
        اگر(length>Part::MAX_لمبائی) {
            سیٹ_Parse_Error(parseError, name_اشاریہ);  // Argument type name too long.
            error_رمز=U_اشاریہ_OUTOFBOUNDS_ERROR;
            واپس 0;
        }
        argType=UMSGPAT_ARG_TYPE_SIMPLE;
        اگر(length==6) {
            // case-insensitive comparisons for complex-type names
            اگر(isChoice(type_اشاریہ)) {
                argType=UMSGPAT_ARG_TYPE_CHOICE;
            } ورنہ اگر(isPlural(type_اشاریہ)) {
                argType=UMSGPAT_ARG_TYPE_PLURAL;
            } ورنہ اگر(isSelect(type_اشاریہ)) {
                argType=UMSGPAT_ARG_TYPE_SELECT;
            }
        } ورنہ اگر(length==13) {
            اگر(isSelect(type_اشاریہ) && isOrdinal(type_اشاریہ+6)) {
                argType=UMSGPAT_ARG_TYPE_SELECTORDINAL;
            }
        }
        // change the ARG_START type from NONE to argType
        partsList->a[arg_آغاز].value = static_cast<int16_t>(argType);
        اگر(argType==UMSGPAT_ARG_TYPE_SIMPLE) {
            جوڑو_Part(UMSGPAT_PART_TYPE_ARG_TYPE, type_اشاریہ, length, 0, error_رمز);
        }
        // look for an argument style (pattern)
        اگر(c==u_دایاں_Curly_Brace) {
            اگر(argType!=UMSGPAT_ARG_TYPE_SIMPLE) {
                سیٹ_Parse_Error(parseError, name_اشاریہ);  // No style field for complex argument.
                error_رمز=U_نمونہ_SYNTAX_ERROR;
                واپس 0;
            }
        } ورنہ /* ',' */ {
            ++اشاریہ;
            اگر(argType==UMSGPAT_ARG_TYPE_SIMPLE) {
                اشاریہ=parseSimpleStyle(اشاریہ, parseError, error_رمز);
            } ورنہ اگر(argType==UMSGPAT_ARG_TYPE_CHOICE) {
                اشاریہ=parseChoiceStyle(اشاریہ, nestingLevel, parseError, error_رمز);
            } ورنہ {
                اشاریہ=parsePluralOrSelectStyle(argType, اشاریہ, nestingLevel, parseError, error_رمز);
            }
        }
    }
    // Argument parsing stopped on the '}'.
    جوڑو_حد_بندی_Part(arg_آغاز, UMSGPAT_PART_TYPE_ARG_حد_بندی, اشاریہ, 1, argType, error_رمز);
    واپس اشاریہ+1;
}

int32_t
پیغام_نمونہ::parseSimpleStyle(int32_t اشاریہ, UParseError *parseError, UError_رمز &error_رمز) {
    اگر(U_FAILURE(error_رمز)) {
        واپس 0;
    }
    int32_t آغاز=اشاریہ;
    int32_t nestedBraces=0;
    جب(اشاریہ<پیغام.length()) {
        char16_t c=پیغام.charAt(اشاریہ++);
        اگر(c==u_apos) {
            // Treat apostrophe as quoting but include it in the style part.
            // Find the end of the quoted literal text.
            اشاریہ=پیغام.اشاریہ_کا(u_apos, اشاریہ);
            اگر(اشاریہ<0) {
                // Quoted literal argument style text reaches to the end of the message.
                سیٹ_Parse_Error(parseError, آغاز);
                error_رمز=U_نمونہ_SYNTAX_ERROR;
                واپس 0;
            }
            // skip the quote-ending apostrophe
            ++اشاریہ;
        } ورنہ اگر(c==u_بایاں_Curly_Brace) {
            ++nestedBraces;
        } ورنہ اگر(c==u_دایاں_Curly_Brace) {
            اگر(nestedBraces>0) {
                --nestedBraces;
            } ورنہ {
                int32_t length=--اشاریہ-آغاز;
                اگر(length>Part::MAX_لمبائی) {
                    سیٹ_Parse_Error(parseError, آغاز);  // Argument style text too long.
                    error_رمز=U_اشاریہ_OUTOFBOUNDS_ERROR;
                    واپس 0;
                }
                جوڑو_Part(UMSGPAT_PART_TYPE_ARG_STYLE, آغاز, length, 0, error_رمز);
                واپس اشاریہ;
            }
        }  // c is part of literal text
    }
    سیٹ_Parse_Error(parseError, 0);  // Unmatched '{' braces in message.
    error_رمز=U_UNMATCHED_BRACES;
    واپس 0;
}

int32_t
پیغام_نمونہ::parseChoiceStyle(int32_t اشاریہ, int32_t nestingLevel,
                                 UParseError *parseError, UError_رمز &error_رمز) {
    اگر(U_FAILURE(error_رمز)) {
        واپس 0;
    }
    int32_t آغاز=اشاریہ;
    اشاریہ=skipWhiteSpace(اشاریہ);
    اگر(اشاریہ==پیغام.length() || پیغام.charAt(اشاریہ)==u_دایاں_Curly_Brace) {
        سیٹ_Parse_Error(parseError, 0);  // Missing choice argument pattern.
        error_رمز=U_نمونہ_SYNTAX_ERROR;
        واپس 0;
    }
    برائے(;;) {
        // The choice argument style contains |-separated (number, separator, message) triples.
        // Parse the number.
        int32_t نمبر_اشاریہ=اشاریہ;
        اشاریہ=skipDouble(اشاریہ);
        int32_t length=اشاریہ-نمبر_اشاریہ;
        اگر(length==0) {
            سیٹ_Parse_Error(parseError, آغاز);  // Bad choice pattern syntax.
            error_رمز=U_نمونہ_SYNTAX_ERROR;
            واپس 0;
        }
        اگر(length>Part::MAX_لمبائی) {
            سیٹ_Parse_Error(parseError, نمبر_اشاریہ);  // Choice number too long.
            error_رمز=U_اشاریہ_OUTOFBOUNDS_ERROR;
            واپس 0;
        }
        parseDouble(نمبر_اشاریہ, اشاریہ, سچ, parseError, error_رمز);  // adds ARG_INT or ARG_DOUBLE
        اگر(U_FAILURE(error_رمز)) {
            واپس 0;
        }
        // Parse the separator.
        اشاریہ=skipWhiteSpace(اشاریہ);
        اگر(اشاریہ==پیغام.length()) {
            سیٹ_Parse_Error(parseError, آغاز);  // Bad choice pattern syntax.
            error_رمز=U_نمونہ_SYNTAX_ERROR;
            واپس 0;
        }
        char16_t c=پیغام.charAt(اشاریہ);
        اگر(!(c==u_pound || c==u_lessThan || c==u_lessOrEqual)) {  // U+2264 is <=
            سیٹ_Parse_Error(parseError, آغاز);  // Expected choice separator (#<\u2264) instead of c.
            error_رمز=U_نمونہ_SYNTAX_ERROR;
            واپس 0;
        }
        جوڑو_Part(UMSGPAT_PART_TYPE_ARG_SELECTOR, اشاریہ, 1, 0, error_رمز);
        // Parse the message fragment.
        اشاریہ=parse_پیغام(++اشاریہ, 0, nestingLevel+1, UMSGPAT_ARG_TYPE_CHOICE, parseError, error_رمز);
        اگر(U_FAILURE(error_رمز)) {
            واپس 0;
        }
        // parseMessage(..., CHOICE) returns the index of the terminator, or msg.length().
        اگر(اشاریہ==پیغام.length()) {
            واپس اشاریہ;
        }
        اگر(پیغام.charAt(اشاریہ)==u_دایاں_Curly_Brace) {
            اگر(!اندر_پیغام_Format_نمونہ(nestingLevel)) {
                سیٹ_Parse_Error(parseError, آغاز);  // Bad choice pattern syntax.
                error_رمز=U_نمونہ_SYNTAX_ERROR;
                واپس 0;
            }
            واپس اشاریہ;
        }  // else the terminator is '|'
        اشاریہ=skipWhiteSpace(اشاریہ+1);
    }
}

int32_t
پیغام_نمونہ::parsePluralOrSelectStyle(UMessage_نمونہ_Arg_Type argType,
                                         int32_t اشاریہ, int32_t nestingLevel,
                                         UParseError *parseError, UError_رمز &error_رمز) {
    اگر(U_FAILURE(error_رمز)) {
        واپس 0;
    }
    int32_t آغاز=اشاریہ;
    UBool is_خالی_خانے=سچ;
    UBool hasOther=جھوٹ;
    برائے(;;) {
        // First, collect the selector looking for a small set of terminators.
        // It would be a little faster to consider the syntax of each possible
        // token right here, but that makes the code too complicated.
        اشاریہ=skipWhiteSpace(اشاریہ);
        UBool eos=اشاریہ==پیغام.length();
        اگر(eos || پیغام.charAt(اشاریہ)==u_دایاں_Curly_Brace) {
            اگر(eos==اندر_پیغام_Format_نمونہ(nestingLevel)) {
                سیٹ_Parse_Error(parseError, آغاز);  // Bad plural/select pattern syntax.
                error_رمز=U_نمونہ_SYNTAX_ERROR;
                واپس 0;
            }
            اگر(!hasOther) {
                سیٹ_Parse_Error(parseError, 0);  // Missing 'other' keyword in plural/select pattern.
                error_رمز=U_DEFAULT_KEYWORD_MISSING;
                واپس 0;
            }
            واپس اشاریہ;
        }
        int32_t selector_اشاریہ=اشاریہ;
        اگر(UMSGPAT_ARG_TYPE_HAS_PLURAL_STYLE(argType) && پیغام.charAt(selector_اشاریہ)==u_equal) {
            // explicit-value plural selector: =double
            اشاریہ=skipDouble(اشاریہ+1);
            int32_t length=اشاریہ-selector_اشاریہ;
            اگر(length==1) {
                سیٹ_Parse_Error(parseError, آغاز);  // Bad plural/select pattern syntax.
                error_رمز=U_نمونہ_SYNTAX_ERROR;
                واپس 0;
            }
            اگر(length>Part::MAX_لمبائی) {
                سیٹ_Parse_Error(parseError, selector_اشاریہ);  // Argument selector too long.
                error_رمز=U_اشاریہ_OUTOFBOUNDS_ERROR;
                واپس 0;
            }
            جوڑو_Part(UMSGPAT_PART_TYPE_ARG_SELECTOR, selector_اشاریہ, length, 0, error_رمز);
            parseDouble(selector_اشاریہ+1, اشاریہ, جھوٹ,
                        parseError, error_رمز);  // adds ARG_INT or ARG_DOUBLE
        } ورنہ {
            اشاریہ=skipIdentifier(اشاریہ);
            int32_t length=اشاریہ-selector_اشاریہ;
            اگر(length==0) {
                سیٹ_Parse_Error(parseError, آغاز);  // Bad plural/select pattern syntax.
                error_رمز=U_نمونہ_SYNTAX_ERROR;
                واپس 0;
            }
            // Note: The ':' in "offset:" is just beyond the skipIdentifier() range.
            اگر( UMSGPAT_ARG_TYPE_HAS_PLURAL_STYLE(argType) && length==6 && اشاریہ<پیغام.length() &&
                0==پیغام.compare(selector_اشاریہ, 7, k_انحراف_Colon, 0, 7)
            ) {
                // plural offset, not a selector
                اگر(!is_خالی_خانے) {
                    // Plural argument 'offset:' (if present) must precede key-message pairs.
                    سیٹ_Parse_Error(parseError, آغاز);
                    error_رمز=U_نمونہ_SYNTAX_ERROR;
                    واپس 0;
                }
                // allow whitespace between offset: and its value
                int32_t قدر_اشاریہ=skipWhiteSpace(اشاریہ+1);  // The ':' is at index.
                اشاریہ=skipDouble(قدر_اشاریہ);
                اگر(اشاریہ==قدر_اشاریہ) {
                    سیٹ_Parse_Error(parseError, آغاز);  // Missing value for plural 'offset:'.
                    error_رمز=U_نمونہ_SYNTAX_ERROR;
                    واپس 0;
                }
                اگر((اشاریہ-قدر_اشاریہ)>Part::MAX_لمبائی) {
                    سیٹ_Parse_Error(parseError, قدر_اشاریہ);  // Plural offset value too long.
                    error_رمز=U_اشاریہ_OUTOFBOUNDS_ERROR;
                    واپس 0;
                }
                parseDouble(قدر_اشاریہ, اشاریہ, جھوٹ,
                            parseError, error_رمز);  // adds ARG_INT or ARG_DOUBLE
                اگر(U_FAILURE(error_رمز)) {
                    واپس 0;
                }
                is_خالی_خانے=جھوٹ;
                جاری;  // no message fragment after the offset
            } ورنہ {
                // normal selector word
                اگر(length>Part::MAX_لمبائی) {
                    سیٹ_Parse_Error(parseError, selector_اشاریہ);  // Argument selector too long.
                    error_رمز=U_اشاریہ_OUTOFBOUNDS_ERROR;
                    واپس 0;
                }
                جوڑو_Part(UMSGPAT_PART_TYPE_ARG_SELECTOR, selector_اشاریہ, length, 0, error_رمز);
                اگر(0==پیغام.compare(selector_اشاریہ, length, kOther, 0, 5)) {
                    hasOther=سچ;
                }
            }
        }
        اگر(U_FAILURE(error_رمز)) {
            واپس 0;
        }

        // parse the message fragment following the selector
        اشاریہ=skipWhiteSpace(اشاریہ);
        اگر(اشاریہ==پیغام.length() || پیغام.charAt(اشاریہ)!=u_بایاں_Curly_Brace) {
            سیٹ_Parse_Error(parseError, selector_اشاریہ);  // No message fragment after plural/select selector.
            error_رمز=U_نمونہ_SYNTAX_ERROR;
            واپس 0;
        }
        اشاریہ=parse_پیغام(اشاریہ, 1, nestingLevel+1, argType, parseError, error_رمز);
        اگر(U_FAILURE(error_رمز)) {
            واپس 0;
        }
        is_خالی_خانے=جھوٹ;
    }
}

int32_t
پیغام_نمونہ::parse_Arg_نمبر(ثابت UnicodeString &s, int32_t آغاز, int32_t حد_بندی) {
    // If the identifier contains only ASCII digits, then it is an argument _number_
    // and must not have leading zeros (except "0" itself).
    // Otherwise it is an argument _name_.
    اگر(آغاز>=حد_بندی) {
        واپس UMSGPAT_ARG_NAME_NOT_VALID;
    }
    int32_t number;
    // Defer numeric errors until we know there are only digits.
    UBool bad_نمبر;
    char16_t c=s.charAt(آغاز++);
    اگر(c==0x30) {
        اگر(آغاز==حد_بندی) {
            واپس 0;
        } ورنہ {
            number=0;
            bad_نمبر=سچ;  // leading zero
        }
    } ورنہ اگر(0x31<=c && c<=0x39) {
        number=c-0x30;
        bad_نمبر=جھوٹ;
    } ورنہ {
        واپس UMSGPAT_ARG_NAME_NOT_نمبر;
    }
    جب(آغاز<حد_بندی) {
        c=s.charAt(آغاز++);
        اگر(0x30<=c && c<=0x39) {
            اگر(number>=INT32_MAX/10) {
                bad_نمبر=سچ;  //حد_سے_تجاوز
            } ورنہ {
                number=number*10+(c-0x30);
            }
        } ورنہ {
            واپس UMSGPAT_ARG_NAME_NOT_نمبر;
        }
    }
    // There are only ASCII digits.
    اگر(bad_نمبر) {
        واپس UMSGPAT_ARG_NAME_NOT_VALID;
    } ورنہ {
        واپس number;
    }
}

خالی
پیغام_نمونہ::parseDouble(int32_t آغاز, int32_t حد_بندی, UBool allowInfinity,
                            UParseError *parseError, UError_رمز &error_رمز) {
    اگر(U_FAILURE(error_رمز)) {
        واپس;
    }
    U_ASSERT(آغاز<حد_بندی);
    // fake loop for easy exit and single throw statement
    برائے(;;) { /*loop doesn't iterate*/
        // fast path for small integers and infinity
        int32_t value=0;
        int32_t isNegative=0;  // not boolean so that we can easily add it to value
        int32_t اشاریہ=آغاز;
        char16_t c=پیغام.charAt(اشاریہ++);
        اگر(c==u_minus) {
            isNegative=1;
            اگر(اشاریہ==حد_بندی) {
                توڑو;  // no number
            }
            c=پیغام.charAt(اشاریہ++);
        } ورنہ اگر(c==u_plus) {
            اگر(اشاریہ==حد_بندی) {
                توڑو;  // no number
            }
            c=پیغام.charAt(اشاریہ++);
        }
        اگر(c==0x221e) {  // infinity
            اگر(allowInfinity && اشاریہ==حد_بندی) {
                دوہرا infinity=uprv_getInfinity();
                جوڑو_Arg_Double_Part(
                    isNegative!=0 ? -infinity : infinity,
                    آغاز, حد_بندی-آغاز, error_رمز);
                واپس;
            } ورنہ {
                توڑو;
            }
        }
        // try to parse the number as a small integer but fall back to a double
        جب('0'<=c && c<='9') {
            value=value*10+(c-'0');
            اگر(value>(Part::MAX_قدر+isNegative)) {
                توڑو;  // not a small-enough integer
            }
            اگر(اشاریہ==حد_بندی) {
                جوڑو_Part(UMSGPAT_PART_TYPE_ARG_INT, آغاز, حد_بندی-آغاز,
                        isNegative!=0 ? -value : value, error_رمز);
                واپس;
            }
            c=پیغام.charAt(اشاریہ++);
        }
        // Let Double.parseDouble() throw a NumberFormatException.
        حرف نمبر_Chars[128];
        int32_t capacity = static_cast<int32_t>(حجم(نمبر_Chars));
        int32_t length=حد_بندی-آغاز;
        اگر(length>=capacity) {
            توڑو;  // number too long
        }
        پیغام.extract(آغاز, length, نمبر_Chars, capacity, US_INV);
        اگر (static_cast<int32_t>(uprv_strlen(نمبر_Chars)) < length) {
            توڑو;  // contains non-invariant character that was turned into NUL
        }
        حرف *انتہا;
        دوہرا numeric_قدر=uprv_strtod(نمبر_Chars, &انتہا);
        اگر(انتہا!=(نمبر_Chars+length)) {
            توڑو;  // parsing error
        }
        جوڑو_Arg_Double_Part(numeric_قدر, آغاز, length, error_رمز);
        واپس;
    }
    سیٹ_Parse_Error(parseError, آغاز /*, limit*/);  // Bad syntax for numeric value.
    error_رمز=U_نمونہ_SYNTAX_ERROR;
}

int32_t
پیغام_نمونہ::skipWhiteSpace(int32_t اشاریہ) {
    ثابت char16_t *s=پیغام.get_بفر();
    int32_t پیغام_لمبائی=پیغام.length();
    ثابت char16_t *t=نمونہ_Props::skipWhiteSpace(s+اشاریہ, پیغام_لمبائی-اشاریہ);
    واپس static_cast<int32_t>(t - s);
}

int32_t
پیغام_نمونہ::skipIdentifier(int32_t اشاریہ) {
    ثابت char16_t *s=پیغام.get_بفر();
    int32_t پیغام_لمبائی=پیغام.length();
    ثابت char16_t *t=نمونہ_Props::skipIdentifier(s+اشاریہ, پیغام_لمبائی-اشاریہ);
    واپس static_cast<int32_t>(t - s);
}

int32_t
پیغام_نمونہ::skipDouble(int32_t اشاریہ) {
    int32_t پیغام_لمبائی=پیغام.length();
    جب(اشاریہ<پیغام_لمبائی) {
        char16_t c=پیغام.charAt(اشاریہ);
        // U+221E: Allow the infinity symbol, for ChoiceFormat patterns.
        اگر((c<0x30 && c!=u_plus && c!=u_minus && c!=u_dot) || (c>0x39 && c!=u_e && c!=u_E && c!=0x221e)) {
            توڑو;
        }
        ++اشاریہ;
    }
    واپس اشاریہ;
}

UBool
پیغام_نمونہ::isArgTypeChar(UChar32 c) {
    واپس (u_a<=c && c<=u_z) || (u_A<=c && c<=u_Z);
}

UBool
پیغام_نمونہ::isChoice(int32_t اشاریہ) {
    char16_t c;
    واپس
        ((c=پیغام.charAt(اشاریہ++))==u_c || c==u_C) &&
        ((c=پیغام.charAt(اشاریہ++))==u_h || c==u_H) &&
        ((c=پیغام.charAt(اشاریہ++))==u_o || c==u_O) &&
        ((c=پیغام.charAt(اشاریہ++))==u_i || c==u_I) &&
        ((c=پیغام.charAt(اشاریہ++))==u_c || c==u_C) &&
        ((c=پیغام.charAt(اشاریہ))==u_e || c==u_E);
}

UBool
پیغام_نمونہ::isPlural(int32_t اشاریہ) {
    char16_t c;
    واپس
        ((c=پیغام.charAt(اشاریہ++))==u_p || c==u_P) &&
        ((c=پیغام.charAt(اشاریہ++))==u_l || c==u_L) &&
        ((c=پیغام.charAt(اشاریہ++))==u_u || c==u_U) &&
        ((c=پیغام.charAt(اشاریہ++))==u_r || c==u_R) &&
        ((c=پیغام.charAt(اشاریہ++))==u_a || c==u_A) &&
        ((c=پیغام.charAt(اشاریہ))==u_l || c==u_L);
}

UBool
پیغام_نمونہ::isSelect(int32_t اشاریہ) {
    char16_t c;
    واپس
        ((c=پیغام.charAt(اشاریہ++))==u_s || c==u_S) &&
        ((c=پیغام.charAt(اشاریہ++))==u_e || c==u_E) &&
        ((c=پیغام.charAt(اشاریہ++))==u_l || c==u_L) &&
        ((c=پیغام.charAt(اشاریہ++))==u_e || c==u_E) &&
        ((c=پیغام.charAt(اشاریہ++))==u_c || c==u_C) &&
        ((c=پیغام.charAt(اشاریہ))==u_t || c==u_T);
}

UBool
پیغام_نمونہ::isOrdinal(int32_t اشاریہ) {
    char16_t c;
    واپس
        ((c=پیغام.charAt(اشاریہ++))==u_o || c==u_O) &&
        ((c=پیغام.charAt(اشاریہ++))==u_r || c==u_R) &&
        ((c=پیغام.charAt(اشاریہ++))==u_d || c==u_D) &&
        ((c=پیغام.charAt(اشاریہ++))==u_i || c==u_I) &&
        ((c=پیغام.charAt(اشاریہ++))==u_n || c==u_N) &&
        ((c=پیغام.charAt(اشاریہ++))==u_a || c==u_A) &&
        ((c=پیغام.charAt(اشاریہ))==u_l || c==u_L);
}

UBool
پیغام_نمونہ::اندر_پیغام_Format_نمونہ(int32_t nestingLevel) {
    واپس nestingLevel>0 || partsList->a[0].type==UMSGPAT_PART_TYPE_پیغام_آغاز;
}

UBool
پیغام_نمونہ::اندر_چوٹی_Level_Choice_پیغام(int32_t nestingLevel, UMessage_نمونہ_Arg_Type والد_Type) {
    واپس
        nestingLevel==1 &&
        والد_Type==UMSGPAT_ARG_TYPE_CHOICE &&
        partsList->a[0].type!=UMSGPAT_PART_TYPE_پیغام_آغاز;
}

خالی
پیغام_نمونہ::جوڑو_Part(UMessage_نمونہ_Part_Type type, int32_t اشاریہ, int32_t length,
                        int32_t value, UError_رمز &error_رمز) {
    اگر(partsList->ensure_استعداد_For_One_مزید(parts_لمبائی, error_رمز)) {
        Part &part=partsList->a[parts_لمبائی++];
        part.type=type;
        part.اشاریہ=اشاریہ;
        part.length = static_cast<uint16_t>(length);
        part.value = static_cast<int16_t>(value);
        part.حد_بندی_Part_اشاریہ=0;
    }
}

خالی
پیغام_نمونہ::جوڑو_حد_بندی_Part(int32_t آغاز,
                             UMessage_نمونہ_Part_Type type, int32_t اشاریہ, int32_t length,
                             int32_t value, UError_رمز &error_رمز) {
    partsList->a[آغاز].حد_بندی_Part_اشاریہ=parts_لمبائی;
    جوڑو_Part(type, اشاریہ, length, value, error_رمز);
}

خالی
پیغام_نمونہ::جوڑو_Arg_Double_Part(دوہرا numeric_قدر, int32_t آغاز, int32_t length,
                                 UError_رمز &error_رمز) {
    اگر(U_FAILURE(error_رمز)) {
        واپس;
    }
    int32_t numeric_اشاریہ=numeric_اقدار_لمبائی;
    اگر(numeric_اقدار_List==خالی_اشاریہ) {
        numeric_اقدار_List=نیا پیغام_نمونہ_Double_List();
        اگر(numeric_اقدار_List==خالی_اشاریہ) {
            error_رمز=U_حافظہ_تخصیص_ERROR;
            واپس;
        }
    } ورنہ اگر(!numeric_اقدار_List->ensure_استعداد_For_One_مزید(numeric_اقدار_لمبائی, error_رمز)) {
        واپس;
    } ورنہ {
        اگر(numeric_اشاریہ>Part::MAX_قدر) {
            error_رمز=U_اشاریہ_OUTOFBOUNDS_ERROR;
            واپس;
        }
    }
    numeric_اقدار_List->a[numeric_اقدار_لمبائی++]=numeric_قدر;
    جوڑو_Part(UMSGPAT_PART_TYPE_ARG_DOUBLE, آغاز, length, numeric_اشاریہ, error_رمز);
}

خالی
پیغام_نمونہ::سیٹ_Parse_Error(UParseError *parseError, int32_t اشاریہ) {
    اگر(parseError==خالی_اشاریہ) {
        واپس;
    }
    parseError->انحراف=اشاریہ;

    // Set preContext to some of msg before index.
    // Avoid splitting a surrogate pair.
    int32_t length=اشاریہ;
    اگر(length>=U_PARSE_CONTEXT_LEN) {
        length=U_PARSE_CONTEXT_LEN-1;
        اگر(length>0 && U16_IS_TRAIL(پیغام[اشاریہ-length])) {
            --length;
        }
    }
    پیغام.extract(اشاریہ-length, length, parseError->preContext);
    parseError->preContext[length]=0;

    // Set postContext to some of msg starting at index.
    length=پیغام.length()-اشاریہ;
    اگر(length>=U_PARSE_CONTEXT_LEN) {
        length=U_PARSE_CONTEXT_LEN-1;
        اگر(length>0 && U16_IS_LEAD(پیغام[اشاریہ+length-1])) {
            --length;
        }
    }
    پیغام.extract(اشاریہ, length, parseError->postContext);
    parseError->postContext[length]=0;
}

// MessageImpl ------------------------------------------------------------- ***

خالی
پیغام_Impl::appendReducedApostrophes(ثابت UnicodeString &s, int32_t آغاز, int32_t حد_بندی,
                                      UnicodeString &sb) {
    int32_t doubleApos=-1;
    برائے(;;) {
        int32_t i=s.اشاریہ_کا(u_apos, آغاز);
        اگر(i<0 || i>=حد_بندی) {
            sb.append(s, آغاز, حد_بندی-آغاز);
            توڑو;
        }
        اگر(i==doubleApos) {
            // Double apostrophe at start-1 and start==i, append one.
            sb.append(u_apos);
            ++آغاز;
            doubleApos=-1;
        } ورنہ {
            // Append text between apostrophes and skip this one.
            sb.append(s, آغاز, i-آغاز);
            doubleApos=آغاز=i+1;
        }
    }
}

// Ported from second half of ICU4J SelectFormat.format(String).
UnicodeString&
پیغام_Impl::append_Sub_پیغام_Without_Skip_Syntax(ثابت پیغام_نمونہ& پیغام_نمونہ_2,
                                               int32_t پیغام_آغاز,
                                               UnicodeString& نتیجہ U_LIFETIME_BOUND) {
    ثابت UnicodeString &پیغام_String=پیغام_نمونہ_2.get_نمونہ_String();
    int32_t پچھلا_اشاریہ=پیغام_نمونہ_2.getPart(پیغام_آغاز).get_حد_بندی();
    برائے(int32_t i=پیغام_آغاز;;) {
        ثابت پیغام_نمونہ::Part &part=پیغام_نمونہ_2.getPart(++i);
        UMessage_نمونہ_Part_Type type=part.getType();
        int32_t اشاریہ=part.get_اشاریہ();
        اگر(type==UMSGPAT_PART_TYPE_پیغام_حد_بندی) {
            واپس نتیجہ.append(پیغام_String, پچھلا_اشاریہ, اشاریہ-پچھلا_اشاریہ);
        } ورنہ اگر(type==UMSGPAT_PART_TYPE_SKIP_SYNTAX) {
            نتیجہ.append(پیغام_String, پچھلا_اشاریہ, اشاریہ-پچھلا_اشاریہ);
            پچھلا_اشاریہ=part.get_حد_بندی();
        } ورنہ اگر(type==UMSGPAT_PART_TYPE_ARG_آغاز) {
            نتیجہ.append(پیغام_String, پچھلا_اشاریہ, اشاریہ-پچھلا_اشاریہ);
            پچھلا_اشاریہ=اشاریہ;
            i=پیغام_نمونہ_2.get_حد_بندی_Part_اشاریہ(i);
            اشاریہ=پیغام_نمونہ_2.getPart(i).get_حد_بندی();
            appendReducedApostrophes(پیغام_String, پچھلا_اشاریہ, اشاریہ, نتیجہ);
            پچھلا_اشاریہ=اشاریہ;
        }
    }
}

U_NAMESPACE_انتہا

#اگر_ختم  // !UCONFIG_NO_FORMATTING
