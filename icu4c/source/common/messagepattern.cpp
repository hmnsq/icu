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
ساكن ثابت char16_t u_چپ_Curly_Brace=0x7B;
ساكن ثابت char16_t u_pipe=0x7C;
ساكن ثابت char16_t u_راست_Curly_Brace=0x7D;
ساكن ثابت char16_t u_lessOrEqual=0x2264;  // U+2264 is <=

ساكن ثابت char16_t k_جابه_جایی_Colon[]={  // "offset:"
    u_o, u_f, u_f, u_s, u_e, u_t, u_colon
};

ساكن ثابت char16_t kOther[]={  // "other"
    u_o, u_t, u_h, u_e, u_r
};

// MessagePatternList ------------------------------------------------------ ***

قالب<نام_نوع T, int32_t پشته_ظرفیت>
صنف پیام_الگو_List : عام UMemory {
عام:
    پیام_الگو_List() {}
    فراغ copyFrom(ثابت پیام_الگو_List<T, پشته_ظرفیت> &other,
                  int32_t length,
                  UError_کد &error_کد);
    UBool ensure_ظرفیت_For_One_بیشتر(int32_t old_درازی, UError_کد &error_کد);
    UBool equals(ثابت پیام_الگو_List<T, پشته_ظرفیت> &other, int32_t length) ثابت {
        لكل(int32_t i=0; i<length; ++i) {
            إذا(a[i]!=other.a[i]) { أرجع خطأ; }
        }
        أرجع درست;
    }

    Maybe_پشته_Array<T, پشته_ظرفیت> a;
};

قالب<نام_نوع T, int32_t پشته_ظرفیت>
فراغ
پیام_الگو_List<T, پشته_ظرفیت>::copyFrom(
        ثابت پیام_الگو_List<T, پشته_ظرفیت> &other,
        int32_t length,
        UError_کد &error_کد) {
    إذا(U_SUCCESS(error_کد) && length>0) {
        إذا(length>a.get_ظرفیت() && اشاره_تهی==a.resize(length)) {
            error_کد=U_حافظه_تخصیص_ERROR;
            أرجع;
        }
        uprv_memcpy(a.getAlias(), other.a.getAlias(), (size_t)length*حجم(T));
    }
}

قالب<نام_نوع T, int32_t پشته_ظرفیت>
UBool
پیام_الگو_List<T, پشته_ظرفیت>::ensure_ظرفیت_For_One_بیشتر(int32_t old_درازی, UError_کد &error_کد) {
    إذا(U_FAILURE(error_کد)) {
        أرجع خطأ;
    }
    إذا(a.get_ظرفیت()>old_درازی || a.resize(2*old_درازی, old_درازی)!=اشاره_تهی) {
        أرجع درست;
    }
    error_کد=U_حافظه_تخصیص_ERROR;
    أرجع خطأ;
}

// MessagePatternList specializations -------------------------------------- ***

صنف پیام_الگو_Double_List : عام پیام_الگو_List<مزدوج, 8> {
};

صنف پیام_الگو_Parts_List : عام پیام_الگو_List<پیام_الگو::Part, 32> {
};

// MessagePattern constructors etc. ---------------------------------------- ***

پیام_الگو::پیام_الگو(UError_کد &error_کد)
        : apos_نما(UCONFIG_MSGPAT_DEFAULT_APOSTROPHE_نما),
          partsList(اشاره_تهی), parts(اشاره_تهی), parts_درازی(0),
          numeric_مقادیر_List(اشاره_تهی), numeric_مقادیر(اشاره_تهی), numeric_مقادیر_درازی(0),
          has_Arg_نامها(خطأ), hasArgNumbers(خطأ), needsAutoQuoting(خطأ) {
    init(error_کد);
}

پیام_الگو::پیام_الگو(UMessage_الگو_Apostrophe_نما نما, UError_کد &error_کد)
        : apos_نما(نما),
          partsList(اشاره_تهی), parts(اشاره_تهی), parts_درازی(0),
          numeric_مقادیر_List(اشاره_تهی), numeric_مقادیر(اشاره_تهی), numeric_مقادیر_درازی(0),
          has_Arg_نامها(خطأ), hasArgNumbers(خطأ), needsAutoQuoting(خطأ) {
    init(error_کد);
}

پیام_الگو::پیام_الگو(ثابت UnicodeString &الگو, UParseError *parseError, UError_کد &error_کد)
        : apos_نما(UCONFIG_MSGPAT_DEFAULT_APOSTROPHE_نما),
          partsList(اشاره_تهی), parts(اشاره_تهی), parts_درازی(0),
          numeric_مقادیر_List(اشاره_تهی), numeric_مقادیر(اشاره_تهی), numeric_مقادیر_درازی(0),
          has_Arg_نامها(خطأ), hasArgNumbers(خطأ), needsAutoQuoting(خطأ) {
    إذا(init(error_کد)) {
        parse(الگو, parseError, error_کد);
    }
}

UBool
پیام_الگو::init(UError_کد &error_کد) {
    إذا(U_FAILURE(error_کد)) {
        أرجع خطأ;
    }
    partsList=جديد پیام_الگو_Parts_List();
    إذا(partsList==اشاره_تهی) {
        error_کد=U_حافظه_تخصیص_ERROR;
        أرجع خطأ;
    }
    parts=partsList->a.getAlias();
    أرجع درست;
}

پیام_الگو::پیام_الگو(ثابت پیام_الگو &other)
        : UObject(other), apos_نما(other.apos_نما), پیام(other.پیام),
          partsList(اشاره_تهی), parts(اشاره_تهی), parts_درازی(0),
          numeric_مقادیر_List(اشاره_تهی), numeric_مقادیر(اشاره_تهی), numeric_مقادیر_درازی(0),
          has_Arg_نامها(other.has_Arg_نامها), hasArgNumbers(other.hasArgNumbers),
          needsAutoQuoting(other.needsAutoQuoting) {
    UError_کد error_کد=U_ZERO_ERROR;
    إذا(!copyStorage(other, error_کد)) {
        clear();
    }
}

پیام_الگو&
پیام_الگو::عملگر=(ثابت پیام_الگو& other) U_LIFETIME_BOUND {
    إذا(هذا==&other) {
        أرجع *هذا;
    }
    apos_نما=other.apos_نما;
    پیام=other.پیام;
    has_Arg_نامها=other.has_Arg_نامها;
    hasArgNumbers=other.hasArgNumbers;
    needsAutoQuoting=other.needsAutoQuoting;
    UError_کد error_کد=U_ZERO_ERROR;
    إذا(!copyStorage(other, error_کد)) {
        clear();
    }
    أرجع *هذا;
}

UBool
پیام_الگو::copyStorage(ثابت پیام_الگو &other, UError_کد &error_کد) {
    إذا(U_FAILURE(error_کد)) {
        أرجع خطأ;
    }
    parts=اشاره_تهی;
    parts_درازی=0;
    numeric_مقادیر=اشاره_تهی;
    numeric_مقادیر_درازی=0;
    إذا(partsList==اشاره_تهی) {
        partsList=جديد پیام_الگو_Parts_List();
        إذا(partsList==اشاره_تهی) {
            error_کد=U_حافظه_تخصیص_ERROR;
            أرجع خطأ;
        }
        parts=partsList->a.getAlias();
    }
    إذا(other.parts_درازی>0) {
        partsList->copyFrom(*other.partsList, other.parts_درازی, error_کد);
        إذا(U_FAILURE(error_کد)) {
            أرجع خطأ;
        }
        parts=partsList->a.getAlias();
        parts_درازی=other.parts_درازی;
    }
    إذا(other.numeric_مقادیر_درازی>0) {
        إذا(numeric_مقادیر_List==اشاره_تهی) {
            numeric_مقادیر_List=جديد پیام_الگو_Double_List();
            إذا(numeric_مقادیر_List==اشاره_تهی) {
                error_کد=U_حافظه_تخصیص_ERROR;
                أرجع خطأ;
            }
            numeric_مقادیر=numeric_مقادیر_List->a.getAlias();
        }
        numeric_مقادیر_List->copyFrom(
            *other.numeric_مقادیر_List, other.numeric_مقادیر_درازی, error_کد);
        إذا(U_FAILURE(error_کد)) {
            أرجع خطأ;
        }
        numeric_مقادیر=numeric_مقادیر_List->a.getAlias();
        numeric_مقادیر_درازی=other.numeric_مقادیر_درازی;
    }
    أرجع درست;
}

پیام_الگو::~پیام_الگو() {
    حذف partsList;
    حذف numeric_مقادیر_List;
}

// MessagePattern API ------------------------------------------------------ ***

پیام_الگو&
پیام_الگو::parse(ثابت UnicodeString& الگو,
                      UParseError* parseError,
                      UError_کد& error_کد) U_LIFETIME_BOUND {
    preParse(الگو, parseError, error_کد);
    parse_پیام(0, 0, 0, UMSGPAT_ARG_TYPE_NONE, parseError, error_کد);
    postParse();
    أرجع *هذا;
}

پیام_الگو&
پیام_الگو::parseChoiceStyle(ثابت UnicodeString& الگو,
                                 UParseError* parseError,
                                 UError_کد& error_کد) U_LIFETIME_BOUND {
    preParse(الگو, parseError, error_کد);
    parseChoiceStyle(0, 0, parseError, error_کد);
    postParse();
    أرجع *هذا;
}

پیام_الگو&
پیام_الگو::parsePluralStyle(ثابت UnicodeString& الگو,
                                 UParseError* parseError,
                                 UError_کد& error_کد) U_LIFETIME_BOUND {
    preParse(الگو, parseError, error_کد);
    parsePluralOrSelectStyle(UMSGPAT_ARG_TYPE_PLURAL, 0, 0, parseError, error_کد);
    postParse();
    أرجع *هذا;
}

پیام_الگو&
پیام_الگو::parseSelectStyle(ثابت UnicodeString& الگو,
                                 UParseError* parseError,
                                 UError_کد& error_کد) U_LIFETIME_BOUND {
    preParse(الگو, parseError, error_کد);
    parsePluralOrSelectStyle(UMSGPAT_ARG_TYPE_SELECT, 0, 0, parseError, error_کد);
    postParse();
    أرجع *هذا;
}

فراغ
پیام_الگو::clear() {
    // Mostly the same as preParse().
    پیام.remove();
    has_Arg_نامها=hasArgNumbers=خطأ;
    needsAutoQuoting=خطأ;
    parts_درازی=0;
    numeric_مقادیر_درازی=0;
}

منطقي
پیام_الگو::عملگر==(ثابت پیام_الگو &other) ثابت {
    إذا(هذا==&other) {
        أرجع درست;
    }
    أرجع
        apos_نما==other.apos_نما &&
        پیام==other.پیام &&
        // parts.equals(o.parts)
        parts_درازی==other.parts_درازی &&
        (parts_درازی==0 || partsList->equals(*other.partsList, parts_درازی));
    // No need to compare numericValues if msg and parts are the same.
}

int32_t
پیام_الگو::hash_کد() ثابت {
    int32_t hash=(apos_نما*37+پیام.hash_کد())*37+parts_درازی;
    لكل(int32_t i=0; i<parts_درازی; ++i) {
        hash=hash*37+parts[i].hash_کد();
    }
    أرجع hash;
}

int32_t
پیام_الگو::validate_آرگومان_Name(ثابت UnicodeString &name) {
    إذا(!الگو_Props::isIdentifier(name.get_میانگیر(), name.length())) {
        أرجع UMSGPAT_ARG_NAME_NOT_VALID;
    }
    أرجع parse_Arg_عدد(name, 0, name.length());
}

UnicodeString
پیام_الگو::autoQuoteApostropheDeep() ثابت {
    إذا(!needsAutoQuoting) {
        أرجع پیام;
    }
    UnicodeString modified(پیام);
    // Iterate backward so that the insertion indexes do not change.
    int32_t count=شمار_Parts();
    لكل(int32_t i=count; i>0;) {
        ثابت Part &part=getPart(--i);
        إذا(part.getType()==UMSGPAT_PART_TYPE_درج_CHAR) {
           modified.insert(part.شاخص, static_cast<char16_t>(part.value));
        }
    }
    أرجع modified;
}

مزدوج
پیام_الگو::get_Numeric_مقدار(ثابت Part &part) ثابت {
    UMessage_الگو_Part_Type type=part.type;
    إذا(type==UMSGPAT_PART_TYPE_ARG_INT) {
        أرجع part.value;
    } وإلا إذا(type==UMSGPAT_PART_TYPE_ARG_DOUBLE) {
        أرجع numeric_مقادیر[part.value];
    } وإلا {
        أرجع UMSGPAT_NO_NUMERIC_مقدار;
    }
}

/**
  * Returns the "offset:" value of a PluralFormat argument, or 0 if none is specified.
  * @param pluralStart the index of the first PluralFormat argument style part. (0..countParts()-1)
  * @return the "offset:" value.
  * @draft ICU 4.8
  */
مزدوج
پیام_الگو::get_Plural_جابه_جایی(int32_t plural_شروع) ثابت {
    ثابت Part &part=getPart(plural_شروع);
    إذا(Part::has_Numeric_مقدار(part.type)) {
        أرجع get_Numeric_مقدار(part);
    } وإلا {
        أرجع 0;
    }
}

// MessagePattern::Part ---------------------------------------------------- ***

منطقي
پیام_الگو::Part::عملگر==(ثابت Part &other) ثابت {
    إذا(هذا==&other) {
        أرجع درست;
    }
    أرجع
        type==other.type &&
        شاخص==other.شاخص &&
        length==other.length &&
        value==other.value &&
        حد_Part_شاخص==other.حد_Part_شاخص;
}

// MessagePattern parser --------------------------------------------------- ***

فراغ
پیام_الگو::preParse(ثابت UnicodeString &الگو, UParseError *parseError, UError_کد &error_کد) {
    إذا(U_FAILURE(error_کد)) {
        أرجع;
    }
    إذا(parseError!=اشاره_تهی) {
        parseError->سطر=0;
        parseError->جابه_جایی=0;
        parseError->preContext[0]=0;
        parseError->postContext[0]=0;
    }
    پیام=الگو;
    has_Arg_نامها=hasArgNumbers=خطأ;
    needsAutoQuoting=خطأ;
    parts_درازی=0;
    numeric_مقادیر_درازی=0;
}

فراغ
پیام_الگو::postParse() {
    إذا(partsList!=اشاره_تهی) {
        parts=partsList->a.getAlias();
    }
    إذا(numeric_مقادیر_List!=اشاره_تهی) {
        numeric_مقادیر=numeric_مقادیر_List->a.getAlias();
    }
}

int32_t
پیام_الگو::parse_پیام(int32_t شاخص, int32_t پیام_شروع_درازی,
                             int32_t nestingLevel, UMessage_الگو_Arg_Type والد_Type,
                             UParseError *parseError, UError_کد &error_کد) {
    إذا(U_FAILURE(error_کد)) {
        أرجع 0;
    }
    إذا(nestingLevel>Part::MAX_NESTED_سطوح) {
        error_کد=U_شاخص_OUTOFBOUNDS_ERROR;
        أرجع 0;
    }
    int32_t پیام_شروع=parts_درازی;
    جمع_کن_Part(UMSGPAT_PART_TYPE_پیام_شروع, شاخص, پیام_شروع_درازی, nestingLevel, error_کد);
    شاخص+=پیام_شروع_درازی;
    لكل(;;) {  // while(index<msg.length()) with U_FAILURE(errorCode) check
        إذا(U_FAILURE(error_کد)) {
            أرجع 0;
        }
        إذا(شاخص>=پیام.length()) {
            اكسر;
        }
        char16_t c=پیام.charAt(شاخص++);
        إذا(c==u_apos) {
            إذا(شاخص==پیام.length()) {
                // The apostrophe is the last character in the pattern. 
                // Add a Part for auto-quoting.
                جمع_کن_Part(UMSGPAT_PART_TYPE_درج_CHAR, شاخص, 0,
                        u_apos, error_کد);  // value=char to be inserted
                needsAutoQuoting=درست;
            } وإلا {
                c=پیام.charAt(شاخص);
                إذا(c==u_apos) {
                    // double apostrophe, skip the second one
                    جمع_کن_Part(UMSGPAT_PART_TYPE_SKIP_SYNTAX, شاخص++, 1, 0, error_کد);
                } وإلا إذا(
                    apos_نما==UMSGPAT_APOS_DOUBLE_REQUIRED ||
                    c==u_چپ_Curly_Brace || c==u_راست_Curly_Brace ||
                    (والد_Type==UMSGPAT_ARG_TYPE_CHOICE && c==u_pipe) ||
                    (UMSGPAT_ARG_TYPE_HAS_PLURAL_STYLE(والد_Type) && c==u_pound)
                ) {
                    // skip the quote-starting apostrophe
                    جمع_کن_Part(UMSGPAT_PART_TYPE_SKIP_SYNTAX, شاخص-1, 1, 0, error_کد);
                    // find the end of the quoted literal text
                    لكل(;;) {
                        شاخص=پیام.شاخص_از(u_apos, شاخص+1);
                        إذا(شاخص>=0) {
                            إذا(/*(index+1)<msg.length() &&*/ پیام.charAt(شاخص+1)==u_apos) {
                                // double apostrophe inside quoted literal text
                                // still encodes a single apostrophe, skip the second one
                                جمع_کن_Part(UMSGPAT_PART_TYPE_SKIP_SYNTAX, ++شاخص, 1, 0, error_کد);
                            } وإلا {
                                // skip the quote-ending apostrophe
                                جمع_کن_Part(UMSGPAT_PART_TYPE_SKIP_SYNTAX, شاخص++, 1, 0, error_کد);
                                اكسر;
                            }
                        } وإلا {
                            // The quoted text reaches to the end of the of the message.
                            شاخص=پیام.length();
                            // Add a Part for auto-quoting.
                            جمع_کن_Part(UMSGPAT_PART_TYPE_درج_CHAR, شاخص, 0,
                                    u_apos, error_کد);  // value=char to be inserted
                            needsAutoQuoting=درست;
                            اكسر;
                        }
                    }
                } وإلا {
                    // Interpret the apostrophe as literal text.
                    // Add a Part for auto-quoting.
                    جمع_کن_Part(UMSGPAT_PART_TYPE_درج_CHAR, شاخص, 0,
                            u_apos, error_کد);  // value=char to be inserted
                    needsAutoQuoting=درست;
                }
            }
        } وإلا إذا(UMSGPAT_ARG_TYPE_HAS_PLURAL_STYLE(والد_Type) && c==u_pound) {
            // The unquoted # in a plural message fragment will be replaced
            // with the (number-offset).
            جمع_کن_Part(UMSGPAT_PART_TYPE_REPLACE_عدد, شاخص-1, 1, 0, error_کد);
        } وإلا إذا(c==u_چپ_Curly_Brace) {
            شاخص=parseArg(شاخص-1, 1, nestingLevel, parseError, error_کد);
        } وإلا إذا((nestingLevel>0 && c==u_راست_Curly_Brace) ||
                  (والد_Type==UMSGPAT_ARG_TYPE_CHOICE && c==u_pipe)) {
            // Finish the message before the terminator.
            // In a choice style, report the "}" substring only for the following ARG_LIMIT,
            // not for this MSG_LIMIT.
            int32_t حد_درازی=(والد_Type==UMSGPAT_ARG_TYPE_CHOICE && c==u_راست_Curly_Brace) ? 0 : 1;
            جمع_کن_حد_Part(پیام_شروع, UMSGPAT_PART_TYPE_پیام_حد, شاخص-1, حد_درازی,
                         nestingLevel, error_کد);
            إذا(والد_Type==UMSGPAT_ARG_TYPE_CHOICE) {
                // Let the choice style parser see the '}' or '|'.
                أرجع شاخص-1;
            } وإلا {
                // continue parsing after the '}'
                أرجع شاخص;
            }
        }  // else: c is part of literal text
    }
    إذا(nestingLevel>0 && !درون_بالاترین_Level_Choice_پیام(nestingLevel, والد_Type)) {
        مجموعه_Parse_Error(parseError, 0);  // Unmatched '{' braces in message.
        error_کد=U_UNMATCHED_BRACES;
        أرجع 0;
    }
    جمع_کن_حد_Part(پیام_شروع, UMSGPAT_PART_TYPE_پیام_حد, شاخص, 0, nestingLevel, error_کد);
    أرجع شاخص;
}

int32_t
پیام_الگو::parseArg(int32_t شاخص, int32_t arg_شروع_درازی, int32_t nestingLevel,
                         UParseError *parseError, UError_کد &error_کد) {
    int32_t arg_شروع=parts_درازی;
    UMessage_الگو_Arg_Type argType=UMSGPAT_ARG_TYPE_NONE;
    جمع_کن_Part(UMSGPAT_PART_TYPE_ARG_شروع, شاخص, arg_شروع_درازی, argType, error_کد);
    إذا(U_FAILURE(error_کد)) {
        أرجع 0;
    }
    int32_t name_شاخص=شاخص=skipWhiteSpace(شاخص+arg_شروع_درازی);
    إذا(شاخص==پیام.length()) {
        مجموعه_Parse_Error(parseError, 0);  // Unmatched '{' braces in message.
        error_کد=U_UNMATCHED_BRACES;
        أرجع 0;
    }
    // parse argument name or number
    شاخص=skipIdentifier(شاخص);
    int32_t number=parse_Arg_عدد(name_شاخص, شاخص);
    إذا(number>=0) {
        int32_t length=شاخص-name_شاخص;
        إذا(length>Part::MAX_درازی || number>Part::MAX_مقدار) {
            مجموعه_Parse_Error(parseError, name_شاخص);  // Argument number too large.
            error_کد=U_شاخص_OUTOFBOUNDS_ERROR;
            أرجع 0;
        }
        hasArgNumbers=درست;
        جمع_کن_Part(UMSGPAT_PART_TYPE_ARG_عدد, name_شاخص, length, number, error_کد);
    } وإلا إذا(number==UMSGPAT_ARG_NAME_NOT_عدد) {
        int32_t length=شاخص-name_شاخص;
        إذا(length>Part::MAX_درازی) {
            مجموعه_Parse_Error(parseError, name_شاخص);  // Argument name too long.
            error_کد=U_شاخص_OUTOFBOUNDS_ERROR;
            أرجع 0;
        }
        has_Arg_نامها=درست;
        جمع_کن_Part(UMSGPAT_PART_TYPE_ARG_NAME, name_شاخص, length, 0, error_کد);
    } وإلا {  // number<-1 (ARG_NAME_NOT_VALID)
        مجموعه_Parse_Error(parseError, name_شاخص);  // Bad argument syntax.
        error_کد=U_الگو_SYNTAX_ERROR;
        أرجع 0;
    }
    شاخص=skipWhiteSpace(شاخص);
    إذا(شاخص==پیام.length()) {
        مجموعه_Parse_Error(parseError, 0);  // Unmatched '{' braces in message.
        error_کد=U_UNMATCHED_BRACES;
        أرجع 0;
    }
    char16_t c=پیام.charAt(شاخص);
    إذا(c==u_راست_Curly_Brace) {
        // all done
    } وإلا إذا(c!=u_comma) {
        مجموعه_Parse_Error(parseError, name_شاخص);  // Bad argument syntax.
        error_کد=U_الگو_SYNTAX_ERROR;
        أرجع 0;
    } وإلا /* ',' */ {
        // parse argument type: case-sensitive a-zA-Z
        int32_t type_شاخص=شاخص=skipWhiteSpace(شاخص+1);
        طالما(شاخص<پیام.length() && isArgTypeChar(پیام.charAt(شاخص))) {
            ++شاخص;
        }
        int32_t length=شاخص-type_شاخص;
        شاخص=skipWhiteSpace(شاخص);
        إذا(شاخص==پیام.length()) {
            مجموعه_Parse_Error(parseError, 0);  // Unmatched '{' braces in message.
            error_کد=U_UNMATCHED_BRACES;
            أرجع 0;
        }
        إذا(length==0 || ((c=پیام.charAt(شاخص))!=u_comma && c!=u_راست_Curly_Brace)) {
            مجموعه_Parse_Error(parseError, name_شاخص);  // Bad argument syntax.
            error_کد=U_الگو_SYNTAX_ERROR;
            أرجع 0;
        }
        إذا(length>Part::MAX_درازی) {
            مجموعه_Parse_Error(parseError, name_شاخص);  // Argument type name too long.
            error_کد=U_شاخص_OUTOFBOUNDS_ERROR;
            أرجع 0;
        }
        argType=UMSGPAT_ARG_TYPE_SIMPLE;
        إذا(length==6) {
            // case-insensitive comparisons for complex-type names
            إذا(isChoice(type_شاخص)) {
                argType=UMSGPAT_ARG_TYPE_CHOICE;
            } وإلا إذا(isPlural(type_شاخص)) {
                argType=UMSGPAT_ARG_TYPE_PLURAL;
            } وإلا إذا(isSelect(type_شاخص)) {
                argType=UMSGPAT_ARG_TYPE_SELECT;
            }
        } وإلا إذا(length==13) {
            إذا(isSelect(type_شاخص) && isOrdinal(type_شاخص+6)) {
                argType=UMSGPAT_ARG_TYPE_SELECTORDINAL;
            }
        }
        // change the ARG_START type from NONE to argType
        partsList->a[arg_شروع].value = static_cast<int16_t>(argType);
        إذا(argType==UMSGPAT_ARG_TYPE_SIMPLE) {
            جمع_کن_Part(UMSGPAT_PART_TYPE_ARG_TYPE, type_شاخص, length, 0, error_کد);
        }
        // look for an argument style (pattern)
        إذا(c==u_راست_Curly_Brace) {
            إذا(argType!=UMSGPAT_ARG_TYPE_SIMPLE) {
                مجموعه_Parse_Error(parseError, name_شاخص);  // No style field for complex argument.
                error_کد=U_الگو_SYNTAX_ERROR;
                أرجع 0;
            }
        } وإلا /* ',' */ {
            ++شاخص;
            إذا(argType==UMSGPAT_ARG_TYPE_SIMPLE) {
                شاخص=parseSimpleStyle(شاخص, parseError, error_کد);
            } وإلا إذا(argType==UMSGPAT_ARG_TYPE_CHOICE) {
                شاخص=parseChoiceStyle(شاخص, nestingLevel, parseError, error_کد);
            } وإلا {
                شاخص=parsePluralOrSelectStyle(argType, شاخص, nestingLevel, parseError, error_کد);
            }
        }
    }
    // Argument parsing stopped on the '}'.
    جمع_کن_حد_Part(arg_شروع, UMSGPAT_PART_TYPE_ARG_حد, شاخص, 1, argType, error_کد);
    أرجع شاخص+1;
}

int32_t
پیام_الگو::parseSimpleStyle(int32_t شاخص, UParseError *parseError, UError_کد &error_کد) {
    إذا(U_FAILURE(error_کد)) {
        أرجع 0;
    }
    int32_t شروع=شاخص;
    int32_t nestedBraces=0;
    طالما(شاخص<پیام.length()) {
        char16_t c=پیام.charAt(شاخص++);
        إذا(c==u_apos) {
            // Treat apostrophe as quoting but include it in the style part.
            // Find the end of the quoted literal text.
            شاخص=پیام.شاخص_از(u_apos, شاخص);
            إذا(شاخص<0) {
                // Quoted literal argument style text reaches to the end of the message.
                مجموعه_Parse_Error(parseError, شروع);
                error_کد=U_الگو_SYNTAX_ERROR;
                أرجع 0;
            }
            // skip the quote-ending apostrophe
            ++شاخص;
        } وإلا إذا(c==u_چپ_Curly_Brace) {
            ++nestedBraces;
        } وإلا إذا(c==u_راست_Curly_Brace) {
            إذا(nestedBraces>0) {
                --nestedBraces;
            } وإلا {
                int32_t length=--شاخص-شروع;
                إذا(length>Part::MAX_درازی) {
                    مجموعه_Parse_Error(parseError, شروع);  // Argument style text too long.
                    error_کد=U_شاخص_OUTOFBOUNDS_ERROR;
                    أرجع 0;
                }
                جمع_کن_Part(UMSGPAT_PART_TYPE_ARG_STYLE, شروع, length, 0, error_کد);
                أرجع شاخص;
            }
        }  // c is part of literal text
    }
    مجموعه_Parse_Error(parseError, 0);  // Unmatched '{' braces in message.
    error_کد=U_UNMATCHED_BRACES;
    أرجع 0;
}

int32_t
پیام_الگو::parseChoiceStyle(int32_t شاخص, int32_t nestingLevel,
                                 UParseError *parseError, UError_کد &error_کد) {
    إذا(U_FAILURE(error_کد)) {
        أرجع 0;
    }
    int32_t شروع=شاخص;
    شاخص=skipWhiteSpace(شاخص);
    إذا(شاخص==پیام.length() || پیام.charAt(شاخص)==u_راست_Curly_Brace) {
        مجموعه_Parse_Error(parseError, 0);  // Missing choice argument pattern.
        error_کد=U_الگو_SYNTAX_ERROR;
        أرجع 0;
    }
    لكل(;;) {
        // The choice argument style contains |-separated (number, separator, message) triples.
        // Parse the number.
        int32_t عدد_شاخص=شاخص;
        شاخص=skipDouble(شاخص);
        int32_t length=شاخص-عدد_شاخص;
        إذا(length==0) {
            مجموعه_Parse_Error(parseError, شروع);  // Bad choice pattern syntax.
            error_کد=U_الگو_SYNTAX_ERROR;
            أرجع 0;
        }
        إذا(length>Part::MAX_درازی) {
            مجموعه_Parse_Error(parseError, عدد_شاخص);  // Choice number too long.
            error_کد=U_شاخص_OUTOFBOUNDS_ERROR;
            أرجع 0;
        }
        parseDouble(عدد_شاخص, شاخص, درست, parseError, error_کد);  // adds ARG_INT or ARG_DOUBLE
        إذا(U_FAILURE(error_کد)) {
            أرجع 0;
        }
        // Parse the separator.
        شاخص=skipWhiteSpace(شاخص);
        إذا(شاخص==پیام.length()) {
            مجموعه_Parse_Error(parseError, شروع);  // Bad choice pattern syntax.
            error_کد=U_الگو_SYNTAX_ERROR;
            أرجع 0;
        }
        char16_t c=پیام.charAt(شاخص);
        إذا(!(c==u_pound || c==u_lessThan || c==u_lessOrEqual)) {  // U+2264 is <=
            مجموعه_Parse_Error(parseError, شروع);  // Expected choice separator (#<\u2264) instead of c.
            error_کد=U_الگو_SYNTAX_ERROR;
            أرجع 0;
        }
        جمع_کن_Part(UMSGPAT_PART_TYPE_ARG_SELECTOR, شاخص, 1, 0, error_کد);
        // Parse the message fragment.
        شاخص=parse_پیام(++شاخص, 0, nestingLevel+1, UMSGPAT_ARG_TYPE_CHOICE, parseError, error_کد);
        إذا(U_FAILURE(error_کد)) {
            أرجع 0;
        }
        // parseMessage(..., CHOICE) returns the index of the terminator, or msg.length().
        إذا(شاخص==پیام.length()) {
            أرجع شاخص;
        }
        إذا(پیام.charAt(شاخص)==u_راست_Curly_Brace) {
            إذا(!درون_پیام_Format_الگو(nestingLevel)) {
                مجموعه_Parse_Error(parseError, شروع);  // Bad choice pattern syntax.
                error_کد=U_الگو_SYNTAX_ERROR;
                أرجع 0;
            }
            أرجع شاخص;
        }  // else the terminator is '|'
        شاخص=skipWhiteSpace(شاخص+1);
    }
}

int32_t
پیام_الگو::parsePluralOrSelectStyle(UMessage_الگو_Arg_Type argType,
                                         int32_t شاخص, int32_t nestingLevel,
                                         UParseError *parseError, UError_کد &error_کد) {
    إذا(U_FAILURE(error_کد)) {
        أرجع 0;
    }
    int32_t شروع=شاخص;
    UBool is_خالی=درست;
    UBool hasOther=خطأ;
    لكل(;;) {
        // First, collect the selector looking for a small set of terminators.
        // It would be a little faster to consider the syntax of each possible
        // token right here, but that makes the code too complicated.
        شاخص=skipWhiteSpace(شاخص);
        UBool eos=شاخص==پیام.length();
        إذا(eos || پیام.charAt(شاخص)==u_راست_Curly_Brace) {
            إذا(eos==درون_پیام_Format_الگو(nestingLevel)) {
                مجموعه_Parse_Error(parseError, شروع);  // Bad plural/select pattern syntax.
                error_کد=U_الگو_SYNTAX_ERROR;
                أرجع 0;
            }
            إذا(!hasOther) {
                مجموعه_Parse_Error(parseError, 0);  // Missing 'other' keyword in plural/select pattern.
                error_کد=U_DEFAULT_KEYWORD_MISSING;
                أرجع 0;
            }
            أرجع شاخص;
        }
        int32_t selector_شاخص=شاخص;
        إذا(UMSGPAT_ARG_TYPE_HAS_PLURAL_STYLE(argType) && پیام.charAt(selector_شاخص)==u_equal) {
            // explicit-value plural selector: =double
            شاخص=skipDouble(شاخص+1);
            int32_t length=شاخص-selector_شاخص;
            إذا(length==1) {
                مجموعه_Parse_Error(parseError, شروع);  // Bad plural/select pattern syntax.
                error_کد=U_الگو_SYNTAX_ERROR;
                أرجع 0;
            }
            إذا(length>Part::MAX_درازی) {
                مجموعه_Parse_Error(parseError, selector_شاخص);  // Argument selector too long.
                error_کد=U_شاخص_OUTOFBOUNDS_ERROR;
                أرجع 0;
            }
            جمع_کن_Part(UMSGPAT_PART_TYPE_ARG_SELECTOR, selector_شاخص, length, 0, error_کد);
            parseDouble(selector_شاخص+1, شاخص, خطأ,
                        parseError, error_کد);  // adds ARG_INT or ARG_DOUBLE
        } وإلا {
            شاخص=skipIdentifier(شاخص);
            int32_t length=شاخص-selector_شاخص;
            إذا(length==0) {
                مجموعه_Parse_Error(parseError, شروع);  // Bad plural/select pattern syntax.
                error_کد=U_الگو_SYNTAX_ERROR;
                أرجع 0;
            }
            // Note: The ':' in "offset:" is just beyond the skipIdentifier() range.
            إذا( UMSGPAT_ARG_TYPE_HAS_PLURAL_STYLE(argType) && length==6 && شاخص<پیام.length() &&
                0==پیام.compare(selector_شاخص, 7, k_جابه_جایی_Colon, 0, 7)
            ) {
                // plural offset, not a selector
                إذا(!is_خالی) {
                    // Plural argument 'offset:' (if present) must precede key-message pairs.
                    مجموعه_Parse_Error(parseError, شروع);
                    error_کد=U_الگو_SYNTAX_ERROR;
                    أرجع 0;
                }
                // allow whitespace between offset: and its value
                int32_t مقدار_شاخص=skipWhiteSpace(شاخص+1);  // The ':' is at index.
                شاخص=skipDouble(مقدار_شاخص);
                إذا(شاخص==مقدار_شاخص) {
                    مجموعه_Parse_Error(parseError, شروع);  // Missing value for plural 'offset:'.
                    error_کد=U_الگو_SYNTAX_ERROR;
                    أرجع 0;
                }
                إذا((شاخص-مقدار_شاخص)>Part::MAX_درازی) {
                    مجموعه_Parse_Error(parseError, مقدار_شاخص);  // Plural offset value too long.
                    error_کد=U_شاخص_OUTOFBOUNDS_ERROR;
                    أرجع 0;
                }
                parseDouble(مقدار_شاخص, شاخص, خطأ,
                            parseError, error_کد);  // adds ARG_INT or ARG_DOUBLE
                إذا(U_FAILURE(error_کد)) {
                    أرجع 0;
                }
                is_خالی=خطأ;
                استمر;  // no message fragment after the offset
            } وإلا {
                // normal selector word
                إذا(length>Part::MAX_درازی) {
                    مجموعه_Parse_Error(parseError, selector_شاخص);  // Argument selector too long.
                    error_کد=U_شاخص_OUTOFBOUNDS_ERROR;
                    أرجع 0;
                }
                جمع_کن_Part(UMSGPAT_PART_TYPE_ARG_SELECTOR, selector_شاخص, length, 0, error_کد);
                إذا(0==پیام.compare(selector_شاخص, length, kOther, 0, 5)) {
                    hasOther=درست;
                }
            }
        }
        إذا(U_FAILURE(error_کد)) {
            أرجع 0;
        }

        // parse the message fragment following the selector
        شاخص=skipWhiteSpace(شاخص);
        إذا(شاخص==پیام.length() || پیام.charAt(شاخص)!=u_چپ_Curly_Brace) {
            مجموعه_Parse_Error(parseError, selector_شاخص);  // No message fragment after plural/select selector.
            error_کد=U_الگو_SYNTAX_ERROR;
            أرجع 0;
        }
        شاخص=parse_پیام(شاخص, 1, nestingLevel+1, argType, parseError, error_کد);
        إذا(U_FAILURE(error_کد)) {
            أرجع 0;
        }
        is_خالی=خطأ;
    }
}

int32_t
پیام_الگو::parse_Arg_عدد(ثابت UnicodeString &s, int32_t شروع, int32_t حد) {
    // If the identifier contains only ASCII digits, then it is an argument _number_
    // and must not have leading zeros (except "0" itself).
    // Otherwise it is an argument _name_.
    إذا(شروع>=حد) {
        أرجع UMSGPAT_ARG_NAME_NOT_VALID;
    }
    int32_t number;
    // Defer numeric errors until we know there are only digits.
    UBool bad_عدد;
    char16_t c=s.charAt(شروع++);
    إذا(c==0x30) {
        إذا(شروع==حد) {
            أرجع 0;
        } وإلا {
            number=0;
            bad_عدد=درست;  // leading zero
        }
    } وإلا إذا(0x31<=c && c<=0x39) {
        number=c-0x30;
        bad_عدد=خطأ;
    } وإلا {
        أرجع UMSGPAT_ARG_NAME_NOT_عدد;
    }
    طالما(شروع<حد) {
        c=s.charAt(شروع++);
        إذا(0x30<=c && c<=0x39) {
            إذا(number>=INT32_MAX/10) {
                bad_عدد=درست;  //سرریز
            } وإلا {
                number=number*10+(c-0x30);
            }
        } وإلا {
            أرجع UMSGPAT_ARG_NAME_NOT_عدد;
        }
    }
    // There are only ASCII digits.
    إذا(bad_عدد) {
        أرجع UMSGPAT_ARG_NAME_NOT_VALID;
    } وإلا {
        أرجع number;
    }
}

فراغ
پیام_الگو::parseDouble(int32_t شروع, int32_t حد, UBool allowInfinity,
                            UParseError *parseError, UError_کد &error_کد) {
    إذا(U_FAILURE(error_کد)) {
        أرجع;
    }
    U_ASSERT(شروع<حد);
    // fake loop for easy exit and single throw statement
    لكل(;;) { /*loop doesn't iterate*/
        // fast path for small integers and infinity
        int32_t value=0;
        int32_t isNegative=0;  // not boolean so that we can easily add it to value
        int32_t شاخص=شروع;
        char16_t c=پیام.charAt(شاخص++);
        إذا(c==u_minus) {
            isNegative=1;
            إذا(شاخص==حد) {
                اكسر;  // no number
            }
            c=پیام.charAt(شاخص++);
        } وإلا إذا(c==u_plus) {
            إذا(شاخص==حد) {
                اكسر;  // no number
            }
            c=پیام.charAt(شاخص++);
        }
        إذا(c==0x221e) {  // infinity
            إذا(allowInfinity && شاخص==حد) {
                مزدوج infinity=uprv_getInfinity();
                جمع_کن_Arg_Double_Part(
                    isNegative!=0 ? -infinity : infinity,
                    شروع, حد-شروع, error_کد);
                أرجع;
            } وإلا {
                اكسر;
            }
        }
        // try to parse the number as a small integer but fall back to a double
        طالما('0'<=c && c<='9') {
            value=value*10+(c-'0');
            إذا(value>(Part::MAX_مقدار+isNegative)) {
                اكسر;  // not a small-enough integer
            }
            إذا(شاخص==حد) {
                جمع_کن_Part(UMSGPAT_PART_TYPE_ARG_INT, شروع, حد-شروع,
                        isNegative!=0 ? -value : value, error_کد);
                أرجع;
            }
            c=پیام.charAt(شاخص++);
        }
        // Let Double.parseDouble() throw a NumberFormatException.
        حرف عدد_Chars[128];
        int32_t capacity = static_cast<int32_t>(حجم(عدد_Chars));
        int32_t length=حد-شروع;
        إذا(length>=capacity) {
            اكسر;  // number too long
        }
        پیام.extract(شروع, length, عدد_Chars, capacity, US_INV);
        إذا (static_cast<int32_t>(uprv_strlen(عدد_Chars)) < length) {
            اكسر;  // contains non-invariant character that was turned into NUL
        }
        حرف *انتها;
        مزدوج numeric_مقدار=uprv_strtod(عدد_Chars, &انتها);
        إذا(انتها!=(عدد_Chars+length)) {
            اكسر;  // parsing error
        }
        جمع_کن_Arg_Double_Part(numeric_مقدار, شروع, length, error_کد);
        أرجع;
    }
    مجموعه_Parse_Error(parseError, شروع /*, limit*/);  // Bad syntax for numeric value.
    error_کد=U_الگو_SYNTAX_ERROR;
}

int32_t
پیام_الگو::skipWhiteSpace(int32_t شاخص) {
    ثابت char16_t *s=پیام.get_میانگیر();
    int32_t پیام_درازی=پیام.length();
    ثابت char16_t *t=الگو_Props::skipWhiteSpace(s+شاخص, پیام_درازی-شاخص);
    أرجع static_cast<int32_t>(t - s);
}

int32_t
پیام_الگو::skipIdentifier(int32_t شاخص) {
    ثابت char16_t *s=پیام.get_میانگیر();
    int32_t پیام_درازی=پیام.length();
    ثابت char16_t *t=الگو_Props::skipIdentifier(s+شاخص, پیام_درازی-شاخص);
    أرجع static_cast<int32_t>(t - s);
}

int32_t
پیام_الگو::skipDouble(int32_t شاخص) {
    int32_t پیام_درازی=پیام.length();
    طالما(شاخص<پیام_درازی) {
        char16_t c=پیام.charAt(شاخص);
        // U+221E: Allow the infinity symbol, for ChoiceFormat patterns.
        إذا((c<0x30 && c!=u_plus && c!=u_minus && c!=u_dot) || (c>0x39 && c!=u_e && c!=u_E && c!=0x221e)) {
            اكسر;
        }
        ++شاخص;
    }
    أرجع شاخص;
}

UBool
پیام_الگو::isArgTypeChar(UChar32 c) {
    أرجع (u_a<=c && c<=u_z) || (u_A<=c && c<=u_Z);
}

UBool
پیام_الگو::isChoice(int32_t شاخص) {
    char16_t c;
    أرجع
        ((c=پیام.charAt(شاخص++))==u_c || c==u_C) &&
        ((c=پیام.charAt(شاخص++))==u_h || c==u_H) &&
        ((c=پیام.charAt(شاخص++))==u_o || c==u_O) &&
        ((c=پیام.charAt(شاخص++))==u_i || c==u_I) &&
        ((c=پیام.charAt(شاخص++))==u_c || c==u_C) &&
        ((c=پیام.charAt(شاخص))==u_e || c==u_E);
}

UBool
پیام_الگو::isPlural(int32_t شاخص) {
    char16_t c;
    أرجع
        ((c=پیام.charAt(شاخص++))==u_p || c==u_P) &&
        ((c=پیام.charAt(شاخص++))==u_l || c==u_L) &&
        ((c=پیام.charAt(شاخص++))==u_u || c==u_U) &&
        ((c=پیام.charAt(شاخص++))==u_r || c==u_R) &&
        ((c=پیام.charAt(شاخص++))==u_a || c==u_A) &&
        ((c=پیام.charAt(شاخص))==u_l || c==u_L);
}

UBool
پیام_الگو::isSelect(int32_t شاخص) {
    char16_t c;
    أرجع
        ((c=پیام.charAt(شاخص++))==u_s || c==u_S) &&
        ((c=پیام.charAt(شاخص++))==u_e || c==u_E) &&
        ((c=پیام.charAt(شاخص++))==u_l || c==u_L) &&
        ((c=پیام.charAt(شاخص++))==u_e || c==u_E) &&
        ((c=پیام.charAt(شاخص++))==u_c || c==u_C) &&
        ((c=پیام.charAt(شاخص))==u_t || c==u_T);
}

UBool
پیام_الگو::isOrdinal(int32_t شاخص) {
    char16_t c;
    أرجع
        ((c=پیام.charAt(شاخص++))==u_o || c==u_O) &&
        ((c=پیام.charAt(شاخص++))==u_r || c==u_R) &&
        ((c=پیام.charAt(شاخص++))==u_d || c==u_D) &&
        ((c=پیام.charAt(شاخص++))==u_i || c==u_I) &&
        ((c=پیام.charAt(شاخص++))==u_n || c==u_N) &&
        ((c=پیام.charAt(شاخص++))==u_a || c==u_A) &&
        ((c=پیام.charAt(شاخص))==u_l || c==u_L);
}

UBool
پیام_الگو::درون_پیام_Format_الگو(int32_t nestingLevel) {
    أرجع nestingLevel>0 || partsList->a[0].type==UMSGPAT_PART_TYPE_پیام_شروع;
}

UBool
پیام_الگو::درون_بالاترین_Level_Choice_پیام(int32_t nestingLevel, UMessage_الگو_Arg_Type والد_Type) {
    أرجع
        nestingLevel==1 &&
        والد_Type==UMSGPAT_ARG_TYPE_CHOICE &&
        partsList->a[0].type!=UMSGPAT_PART_TYPE_پیام_شروع;
}

فراغ
پیام_الگو::جمع_کن_Part(UMessage_الگو_Part_Type type, int32_t شاخص, int32_t length,
                        int32_t value, UError_کد &error_کد) {
    إذا(partsList->ensure_ظرفیت_For_One_بیشتر(parts_درازی, error_کد)) {
        Part &part=partsList->a[parts_درازی++];
        part.type=type;
        part.شاخص=شاخص;
        part.length = static_cast<uint16_t>(length);
        part.value = static_cast<int16_t>(value);
        part.حد_Part_شاخص=0;
    }
}

فراغ
پیام_الگو::جمع_کن_حد_Part(int32_t شروع,
                             UMessage_الگو_Part_Type type, int32_t شاخص, int32_t length,
                             int32_t value, UError_کد &error_کد) {
    partsList->a[شروع].حد_Part_شاخص=parts_درازی;
    جمع_کن_Part(type, شاخص, length, value, error_کد);
}

فراغ
پیام_الگو::جمع_کن_Arg_Double_Part(مزدوج numeric_مقدار, int32_t شروع, int32_t length,
                                 UError_کد &error_کد) {
    إذا(U_FAILURE(error_کد)) {
        أرجع;
    }
    int32_t numeric_شاخص=numeric_مقادیر_درازی;
    إذا(numeric_مقادیر_List==اشاره_تهی) {
        numeric_مقادیر_List=جديد پیام_الگو_Double_List();
        إذا(numeric_مقادیر_List==اشاره_تهی) {
            error_کد=U_حافظه_تخصیص_ERROR;
            أرجع;
        }
    } وإلا إذا(!numeric_مقادیر_List->ensure_ظرفیت_For_One_بیشتر(numeric_مقادیر_درازی, error_کد)) {
        أرجع;
    } وإلا {
        إذا(numeric_شاخص>Part::MAX_مقدار) {
            error_کد=U_شاخص_OUTOFBOUNDS_ERROR;
            أرجع;
        }
    }
    numeric_مقادیر_List->a[numeric_مقادیر_درازی++]=numeric_مقدار;
    جمع_کن_Part(UMSGPAT_PART_TYPE_ARG_DOUBLE, شروع, length, numeric_شاخص, error_کد);
}

فراغ
پیام_الگو::مجموعه_Parse_Error(UParseError *parseError, int32_t شاخص) {
    إذا(parseError==اشاره_تهی) {
        أرجع;
    }
    parseError->جابه_جایی=شاخص;

    // Set preContext to some of msg before index.
    // Avoid splitting a surrogate pair.
    int32_t length=شاخص;
    إذا(length>=U_PARSE_CONTEXT_LEN) {
        length=U_PARSE_CONTEXT_LEN-1;
        إذا(length>0 && U16_IS_TRAIL(پیام[شاخص-length])) {
            --length;
        }
    }
    پیام.extract(شاخص-length, length, parseError->preContext);
    parseError->preContext[length]=0;

    // Set postContext to some of msg starting at index.
    length=پیام.length()-شاخص;
    إذا(length>=U_PARSE_CONTEXT_LEN) {
        length=U_PARSE_CONTEXT_LEN-1;
        إذا(length>0 && U16_IS_LEAD(پیام[شاخص+length-1])) {
            --length;
        }
    }
    پیام.extract(شاخص, length, parseError->postContext);
    parseError->postContext[length]=0;
}

// MessageImpl ------------------------------------------------------------- ***

فراغ
پیام_Impl::appendReducedApostrophes(ثابت UnicodeString &s, int32_t شروع, int32_t حد,
                                      UnicodeString &sb) {
    int32_t doubleApos=-1;
    لكل(;;) {
        int32_t i=s.شاخص_از(u_apos, شروع);
        إذا(i<0 || i>=حد) {
            sb.append(s, شروع, حد-شروع);
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
پیام_Impl::append_Sub_پیام_Without_Skip_Syntax(ثابت پیام_الگو& پیام_الگو_2,
                                               int32_t پیام_شروع,
                                               UnicodeString& نتیجه U_LIFETIME_BOUND) {
    ثابت UnicodeString &پیام_String=پیام_الگو_2.get_الگو_String();
    int32_t prev_شاخص=پیام_الگو_2.getPart(پیام_شروع).get_حد();
    لكل(int32_t i=پیام_شروع;;) {
        ثابت پیام_الگو::Part &part=پیام_الگو_2.getPart(++i);
        UMessage_الگو_Part_Type type=part.getType();
        int32_t شاخص=part.get_شاخص();
        إذا(type==UMSGPAT_PART_TYPE_پیام_حد) {
            أرجع نتیجه.append(پیام_String, prev_شاخص, شاخص-prev_شاخص);
        } وإلا إذا(type==UMSGPAT_PART_TYPE_SKIP_SYNTAX) {
            نتیجه.append(پیام_String, prev_شاخص, شاخص-prev_شاخص);
            prev_شاخص=part.get_حد();
        } وإلا إذا(type==UMSGPAT_PART_TYPE_ARG_شروع) {
            نتیجه.append(پیام_String, prev_شاخص, شاخص-prev_شاخص);
            prev_شاخص=شاخص;
            i=پیام_الگو_2.get_حد_Part_شاخص(i);
            شاخص=پیام_الگو_2.getPart(i).get_حد();
            appendReducedApostrophes(پیام_String, prev_شاخص, شاخص, نتیجه);
            prev_شاخص=شاخص;
        }
    }
}

U_NAMESPACE_انتها

#پایان_اگر  // !UCONFIG_NO_FORMATTING
