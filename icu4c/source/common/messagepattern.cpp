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
ساكن ثابت char16_t u_کیڼ_Curly_Brace=0x7B;
ساكن ثابت char16_t u_pipe=0x7C;
ساكن ثابت char16_t u_ښی_Curly_Brace=0x7D;
ساكن ثابت char16_t u_lessOrEqual=0x2264;  // U+2264 is <=

ساكن ثابت char16_t k_دننه_واټن_Colon[]={  // "offset:"
    u_o, u_f, u_f, u_s, u_e, u_t, u_colon
};

ساكن ثابت char16_t kOther[]={  // "other"
    u_o, u_t, u_h, u_e, u_r
};

// MessagePatternList ------------------------------------------------------ ***

کينډۍ<ډول_نوم T, int32_t ډېری_ظرفیت>
صنف پیغام_نمونه_List : عام UMemory {
عام:
    پیغام_نمونه_List() {}
    فراغ copyFrom(ثابت پیغام_نمونه_List<T, ډېری_ظرفیت> &other,
                  int32_t length,
                  UError_کوډ &error_کوډ);
    UBool ensure_ظرفیت_For_One_نور(int32_t old_اوږدتیا, UError_کوډ &error_کوډ);
    UBool equals(ثابت پیغام_نمونه_List<T, ډېری_ظرفیت> &other, int32_t length) ثابت {
        لكل(int32_t i=0; i<length; ++i) {
            إذا(a[i]!=other.a[i]) { أرجع خطأ; }
        }
        أرجع ریښتیا;
    }

    Maybe_ډېری_Array<T, ډېری_ظرفیت> a;
};

کينډۍ<ډول_نوم T, int32_t ډېری_ظرفیت>
فراغ
پیغام_نمونه_List<T, ډېری_ظرفیت>::copyFrom(
        ثابت پیغام_نمونه_List<T, ډېری_ظرفیت> &other,
        int32_t length,
        UError_کوډ &error_کوډ) {
    إذا(U_SUCCESS(error_کوډ) && length>0) {
        إذا(length>a.get_ظرفیت() && تش_اشاره==a.resize(length)) {
            error_کوډ=U_حافظه_ویش_ERROR;
            أرجع;
        }
        uprv_memcpy(a.getAlias(), other.a.getAlias(), (size_t)length*حجم(T));
    }
}

کينډۍ<ډول_نوم T, int32_t ډېری_ظرفیت>
UBool
پیغام_نمونه_List<T, ډېری_ظرفیت>::ensure_ظرفیت_For_One_نور(int32_t old_اوږدتیا, UError_کوډ &error_کوډ) {
    إذا(U_FAILURE(error_کوډ)) {
        أرجع خطأ;
    }
    إذا(a.get_ظرفیت()>old_اوږدتیا || a.resize(2*old_اوږدتیا, old_اوږدتیا)!=تش_اشاره) {
        أرجع ریښتیا;
    }
    error_کوډ=U_حافظه_ویش_ERROR;
    أرجع خطأ;
}

// MessagePatternList specializations -------------------------------------- ***

صنف پیغام_نمونه_Double_List : عام پیغام_نمونه_List<مزدوج, 8> {
};

صنف پیغام_نمونه_Parts_List : عام پیغام_نمونه_List<پیغام_نمونه::Part, 32> {
};

// MessagePattern constructors etc. ---------------------------------------- ***

پیغام_نمونه::پیغام_نمونه(UError_کوډ &error_کوډ)
        : apos_ډېرپېښ(UCONFIG_MSGPAT_DEFAULT_APOSTROPHE_ډېرپېښ),
          partsList(تش_اشاره), parts(تش_اشاره), parts_اوږدتیا(0),
          numeric_ارزښتونه_List(تش_اشاره), numeric_ارزښتونه(تش_اشاره), numeric_ارزښتونه_اوږدتیا(0),
          has_Arg_نومونه(خطأ), hasArgNumbers(خطأ), needsAutoQuoting(خطأ) {
    init(error_کوډ);
}

پیغام_نمونه::پیغام_نمونه(UMessage_نمونه_Apostrophe_ډېرپېښ ډېرپېښ, UError_کوډ &error_کوډ)
        : apos_ډېرپېښ(ډېرپېښ),
          partsList(تش_اشاره), parts(تش_اشاره), parts_اوږدتیا(0),
          numeric_ارزښتونه_List(تش_اشاره), numeric_ارزښتونه(تش_اشاره), numeric_ارزښتونه_اوږدتیا(0),
          has_Arg_نومونه(خطأ), hasArgNumbers(خطأ), needsAutoQuoting(خطأ) {
    init(error_کوډ);
}

پیغام_نمونه::پیغام_نمونه(ثابت UnicodeString &نمونه, UParseError *parseError, UError_کوډ &error_کوډ)
        : apos_ډېرپېښ(UCONFIG_MSGPAT_DEFAULT_APOSTROPHE_ډېرپېښ),
          partsList(تش_اشاره), parts(تش_اشاره), parts_اوږدتیا(0),
          numeric_ارزښتونه_List(تش_اشاره), numeric_ارزښتونه(تش_اشاره), numeric_ارزښتونه_اوږدتیا(0),
          has_Arg_نومونه(خطأ), hasArgNumbers(خطأ), needsAutoQuoting(خطأ) {
    إذا(init(error_کوډ)) {
        parse(نمونه, parseError, error_کوډ);
    }
}

UBool
پیغام_نمونه::init(UError_کوډ &error_کوډ) {
    إذا(U_FAILURE(error_کوډ)) {
        أرجع خطأ;
    }
    partsList=جديد پیغام_نمونه_Parts_List();
    إذا(partsList==تش_اشاره) {
        error_کوډ=U_حافظه_ویش_ERROR;
        أرجع خطأ;
    }
    parts=partsList->a.getAlias();
    أرجع ریښتیا;
}

پیغام_نمونه::پیغام_نمونه(ثابت پیغام_نمونه &other)
        : UObject(other), apos_ډېرپېښ(other.apos_ډېرپېښ), پیغام(other.پیغام),
          partsList(تش_اشاره), parts(تش_اشاره), parts_اوږدتیا(0),
          numeric_ارزښتونه_List(تش_اشاره), numeric_ارزښتونه(تش_اشاره), numeric_ارزښتونه_اوږدتیا(0),
          has_Arg_نومونه(other.has_Arg_نومونه), hasArgNumbers(other.hasArgNumbers),
          needsAutoQuoting(other.needsAutoQuoting) {
    UError_کوډ error_کوډ=U_ZERO_ERROR;
    إذا(!copyStorage(other, error_کوډ)) {
        clear();
    }
}

پیغام_نمونه&
پیغام_نمونه::چلوونکی=(ثابت پیغام_نمونه& other) U_LIFETIME_BOUND {
    إذا(هذا==&other) {
        أرجع *هذا;
    }
    apos_ډېرپېښ=other.apos_ډېرپېښ;
    پیغام=other.پیغام;
    has_Arg_نومونه=other.has_Arg_نومونه;
    hasArgNumbers=other.hasArgNumbers;
    needsAutoQuoting=other.needsAutoQuoting;
    UError_کوډ error_کوډ=U_ZERO_ERROR;
    إذا(!copyStorage(other, error_کوډ)) {
        clear();
    }
    أرجع *هذا;
}

UBool
پیغام_نمونه::copyStorage(ثابت پیغام_نمونه &other, UError_کوډ &error_کوډ) {
    إذا(U_FAILURE(error_کوډ)) {
        أرجع خطأ;
    }
    parts=تش_اشاره;
    parts_اوږدتیا=0;
    numeric_ارزښتونه=تش_اشاره;
    numeric_ارزښتونه_اوږدتیا=0;
    إذا(partsList==تش_اشاره) {
        partsList=جديد پیغام_نمونه_Parts_List();
        إذا(partsList==تش_اشاره) {
            error_کوډ=U_حافظه_ویش_ERROR;
            أرجع خطأ;
        }
        parts=partsList->a.getAlias();
    }
    إذا(other.parts_اوږدتیا>0) {
        partsList->copyFrom(*other.partsList, other.parts_اوږدتیا, error_کوډ);
        إذا(U_FAILURE(error_کوډ)) {
            أرجع خطأ;
        }
        parts=partsList->a.getAlias();
        parts_اوږدتیا=other.parts_اوږدتیا;
    }
    إذا(other.numeric_ارزښتونه_اوږدتیا>0) {
        إذا(numeric_ارزښتونه_List==تش_اشاره) {
            numeric_ارزښتونه_List=جديد پیغام_نمونه_Double_List();
            إذا(numeric_ارزښتونه_List==تش_اشاره) {
                error_کوډ=U_حافظه_ویش_ERROR;
                أرجع خطأ;
            }
            numeric_ارزښتونه=numeric_ارزښتونه_List->a.getAlias();
        }
        numeric_ارزښتونه_List->copyFrom(
            *other.numeric_ارزښتونه_List, other.numeric_ارزښتونه_اوږدتیا, error_کوډ);
        إذا(U_FAILURE(error_کوډ)) {
            أرجع خطأ;
        }
        numeric_ارزښتونه=numeric_ارزښتونه_List->a.getAlias();
        numeric_ارزښتونه_اوږدتیا=other.numeric_ارزښتونه_اوږدتیا;
    }
    أرجع ریښتیا;
}

پیغام_نمونه::~پیغام_نمونه() {
    ړنګول partsList;
    ړنګول numeric_ارزښتونه_List;
}

// MessagePattern API ------------------------------------------------------ ***

پیغام_نمونه&
پیغام_نمونه::parse(ثابت UnicodeString& نمونه,
                      UParseError* parseError,
                      UError_کوډ& error_کوډ) U_LIFETIME_BOUND {
    preParse(نمونه, parseError, error_کوډ);
    parse_پیغام(0, 0, 0, UMSGPAT_ARG_TYPE_NONE, parseError, error_کوډ);
    postParse();
    أرجع *هذا;
}

پیغام_نمونه&
پیغام_نمونه::parseChoiceStyle(ثابت UnicodeString& نمونه,
                                 UParseError* parseError,
                                 UError_کوډ& error_کوډ) U_LIFETIME_BOUND {
    preParse(نمونه, parseError, error_کوډ);
    parseChoiceStyle(0, 0, parseError, error_کوډ);
    postParse();
    أرجع *هذا;
}

پیغام_نمونه&
پیغام_نمونه::parsePluralStyle(ثابت UnicodeString& نمونه,
                                 UParseError* parseError,
                                 UError_کوډ& error_کوډ) U_LIFETIME_BOUND {
    preParse(نمونه, parseError, error_کوډ);
    parsePluralOrSelectStyle(UMSGPAT_ARG_TYPE_PLURAL, 0, 0, parseError, error_کوډ);
    postParse();
    أرجع *هذا;
}

پیغام_نمونه&
پیغام_نمونه::parseSelectStyle(ثابت UnicodeString& نمونه,
                                 UParseError* parseError,
                                 UError_کوډ& error_کوډ) U_LIFETIME_BOUND {
    preParse(نمونه, parseError, error_کوډ);
    parsePluralOrSelectStyle(UMSGPAT_ARG_TYPE_SELECT, 0, 0, parseError, error_کوډ);
    postParse();
    أرجع *هذا;
}

فراغ
پیغام_نمونه::clear() {
    // Mostly the same as preParse().
    پیغام.remove();
    has_Arg_نومونه=hasArgNumbers=خطأ;
    needsAutoQuoting=خطأ;
    parts_اوږدتیا=0;
    numeric_ارزښتونه_اوږدتیا=0;
}

منطقي
پیغام_نمونه::چلوونکی==(ثابت پیغام_نمونه &other) ثابت {
    إذا(هذا==&other) {
        أرجع ریښتیا;
    }
    أرجع
        apos_ډېرپېښ==other.apos_ډېرپېښ &&
        پیغام==other.پیغام &&
        // parts.equals(o.parts)
        parts_اوږدتیا==other.parts_اوږدتیا &&
        (parts_اوږدتیا==0 || partsList->equals(*other.partsList, parts_اوږدتیا));
    // No need to compare numericValues if msg and parts are the same.
}

int32_t
پیغام_نمونه::hash_کوډ() ثابت {
    int32_t hash=(apos_ډېرپېښ*37+پیغام.hash_کوډ())*37+parts_اوږدتیا;
    لكل(int32_t i=0; i<parts_اوږدتیا; ++i) {
        hash=hash*37+parts[i].hash_کوډ();
    }
    أرجع hash;
}

int32_t
پیغام_نمونه::validate_دلیل_Name(ثابت UnicodeString &name) {
    إذا(!نمونه_Props::isIdentifier(name.get_بفر(), name.length())) {
        أرجع UMSGPAT_ARG_NAME_NOT_VALID;
    }
    أرجع parse_Arg_شمېره(name, 0, name.length());
}

UnicodeString
پیغام_نمونه::autoQuoteApostropheDeep() ثابت {
    إذا(!needsAutoQuoting) {
        أرجع پیغام;
    }
    UnicodeString modified(پیغام);
    // Iterate backward so that the insertion indexes do not change.
    int32_t count=شمېر_Parts();
    لكل(int32_t i=count; i>0;) {
        ثابت Part &part=getPart(--i);
        إذا(part.getType()==UMSGPAT_PART_TYPE_ورننباسه_CHAR) {
           modified.insert(part.شاخص, static_cast<char16_t>(part.value));
        }
    }
    أرجع modified;
}

مزدوج
پیغام_نمونه::get_Numeric_ارزښت(ثابت Part &part) ثابت {
    UMessage_نمونه_Part_Type type=part.type;
    إذا(type==UMSGPAT_PART_TYPE_ARG_INT) {
        أرجع part.value;
    } وإلا إذا(type==UMSGPAT_PART_TYPE_ARG_DOUBLE) {
        أرجع numeric_ارزښتونه[part.value];
    } وإلا {
        أرجع UMSGPAT_NO_NUMERIC_ارزښت;
    }
}

/**
  * Returns the "offset:" value of a PluralFormat argument, or 0 if none is specified.
  * @param pluralStart the index of the first PluralFormat argument style part. (0..countParts()-1)
  * @return the "offset:" value.
  * @draft ICU 4.8
  */
مزدوج
پیغام_نمونه::get_Plural_دننه_واټن(int32_t plural_پیل) ثابت {
    ثابت Part &part=getPart(plural_پیل);
    إذا(Part::has_Numeric_ارزښت(part.type)) {
        أرجع get_Numeric_ارزښت(part);
    } وإلا {
        أرجع 0;
    }
}

// MessagePattern::Part ---------------------------------------------------- ***

منطقي
پیغام_نمونه::Part::چلوونکی==(ثابت Part &other) ثابت {
    إذا(هذا==&other) {
        أرجع ریښتیا;
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
پیغام_نمونه::preParse(ثابت UnicodeString &نمونه, UParseError *parseError, UError_کوډ &error_کوډ) {
    إذا(U_FAILURE(error_کوډ)) {
        أرجع;
    }
    إذا(parseError!=تش_اشاره) {
        parseError->کرښه=0;
        parseError->دننه_واټن=0;
        parseError->preContext[0]=0;
        parseError->postContext[0]=0;
    }
    پیغام=نمونه;
    has_Arg_نومونه=hasArgNumbers=خطأ;
    needsAutoQuoting=خطأ;
    parts_اوږدتیا=0;
    numeric_ارزښتونه_اوږدتیا=0;
}

فراغ
پیغام_نمونه::postParse() {
    إذا(partsList!=تش_اشاره) {
        parts=partsList->a.getAlias();
    }
    إذا(numeric_ارزښتونه_List!=تش_اشاره) {
        numeric_ارزښتونه=numeric_ارزښتونه_List->a.getAlias();
    }
}

int32_t
پیغام_نمونه::parse_پیغام(int32_t شاخص, int32_t پیغام_پیل_اوږدتیا,
                             int32_t nestingLevel, UMessage_نمونه_Arg_Type پلار_Type,
                             UParseError *parseError, UError_کوډ &error_کوډ) {
    إذا(U_FAILURE(error_کوډ)) {
        أرجع 0;
    }
    إذا(nestingLevel>Part::MAX_NESTED_پوړونه) {
        error_کوډ=U_شاخص_OUTOFBOUNDS_ERROR;
        أرجع 0;
    }
    int32_t پیغام_پیل=parts_اوږدتیا;
    جمع_کړه_Part(UMSGPAT_PART_TYPE_پیغام_پیل, شاخص, پیغام_پیل_اوږدتیا, nestingLevel, error_کوډ);
    شاخص+=پیغام_پیل_اوږدتیا;
    لكل(;;) {  // while(index<msg.length()) with U_FAILURE(errorCode) check
        إذا(U_FAILURE(error_کوډ)) {
            أرجع 0;
        }
        إذا(شاخص>=پیغام.length()) {
            اكسر;
        }
        char16_t c=پیغام.charAt(شاخص++);
        إذا(c==u_apos) {
            إذا(شاخص==پیغام.length()) {
                // The apostrophe is the last character in the pattern. 
                // Add a Part for auto-quoting.
                جمع_کړه_Part(UMSGPAT_PART_TYPE_ورننباسه_CHAR, شاخص, 0,
                        u_apos, error_کوډ);  // value=char to be inserted
                needsAutoQuoting=ریښتیا;
            } وإلا {
                c=پیغام.charAt(شاخص);
                إذا(c==u_apos) {
                    // double apostrophe, skip the second one
                    جمع_کړه_Part(UMSGPAT_PART_TYPE_SKIP_SYNTAX, شاخص++, 1, 0, error_کوډ);
                } وإلا إذا(
                    apos_ډېرپېښ==UMSGPAT_APOS_DOUBLE_REQUIRED ||
                    c==u_کیڼ_Curly_Brace || c==u_ښی_Curly_Brace ||
                    (پلار_Type==UMSGPAT_ARG_TYPE_CHOICE && c==u_pipe) ||
                    (UMSGPAT_ARG_TYPE_HAS_PLURAL_STYLE(پلار_Type) && c==u_pound)
                ) {
                    // skip the quote-starting apostrophe
                    جمع_کړه_Part(UMSGPAT_PART_TYPE_SKIP_SYNTAX, شاخص-1, 1, 0, error_کوډ);
                    // find the end of the quoted literal text
                    لكل(;;) {
                        شاخص=پیغام.شاخص_د(u_apos, شاخص+1);
                        إذا(شاخص>=0) {
                            إذا(/*(index+1)<msg.length() &&*/ پیغام.charAt(شاخص+1)==u_apos) {
                                // double apostrophe inside quoted literal text
                                // still encodes a single apostrophe, skip the second one
                                جمع_کړه_Part(UMSGPAT_PART_TYPE_SKIP_SYNTAX, ++شاخص, 1, 0, error_کوډ);
                            } وإلا {
                                // skip the quote-ending apostrophe
                                جمع_کړه_Part(UMSGPAT_PART_TYPE_SKIP_SYNTAX, شاخص++, 1, 0, error_کوډ);
                                اكسر;
                            }
                        } وإلا {
                            // The quoted text reaches to the end of the of the message.
                            شاخص=پیغام.length();
                            // Add a Part for auto-quoting.
                            جمع_کړه_Part(UMSGPAT_PART_TYPE_ورننباسه_CHAR, شاخص, 0,
                                    u_apos, error_کوډ);  // value=char to be inserted
                            needsAutoQuoting=ریښتیا;
                            اكسر;
                        }
                    }
                } وإلا {
                    // Interpret the apostrophe as literal text.
                    // Add a Part for auto-quoting.
                    جمع_کړه_Part(UMSGPAT_PART_TYPE_ورننباسه_CHAR, شاخص, 0,
                            u_apos, error_کوډ);  // value=char to be inserted
                    needsAutoQuoting=ریښتیا;
                }
            }
        } وإلا إذا(UMSGPAT_ARG_TYPE_HAS_PLURAL_STYLE(پلار_Type) && c==u_pound) {
            // The unquoted # in a plural message fragment will be replaced
            // with the (number-offset).
            جمع_کړه_Part(UMSGPAT_PART_TYPE_REPLACE_شمېره, شاخص-1, 1, 0, error_کوډ);
        } وإلا إذا(c==u_کیڼ_Curly_Brace) {
            شاخص=parseArg(شاخص-1, 1, nestingLevel, parseError, error_کوډ);
        } وإلا إذا((nestingLevel>0 && c==u_ښی_Curly_Brace) ||
                  (پلار_Type==UMSGPAT_ARG_TYPE_CHOICE && c==u_pipe)) {
            // Finish the message before the terminator.
            // In a choice style, report the "}" substring only for the following ARG_LIMIT,
            // not for this MSG_LIMIT.
            int32_t حد_اوږدتیا=(پلار_Type==UMSGPAT_ARG_TYPE_CHOICE && c==u_ښی_Curly_Brace) ? 0 : 1;
            جمع_کړه_حد_Part(پیغام_پیل, UMSGPAT_PART_TYPE_پیغام_حد, شاخص-1, حد_اوږدتیا,
                         nestingLevel, error_کوډ);
            إذا(پلار_Type==UMSGPAT_ARG_TYPE_CHOICE) {
                // Let the choice style parser see the '}' or '|'.
                أرجع شاخص-1;
            } وإلا {
                // continue parsing after the '}'
                أرجع شاخص;
            }
        }  // else: c is part of literal text
    }
    إذا(nestingLevel>0 && !دننه_سر_Level_Choice_پیغام(nestingLevel, پلار_Type)) {
        سېټ_Parse_Error(parseError, 0);  // Unmatched '{' braces in message.
        error_کوډ=U_UNMATCHED_BRACES;
        أرجع 0;
    }
    جمع_کړه_حد_Part(پیغام_پیل, UMSGPAT_PART_TYPE_پیغام_حد, شاخص, 0, nestingLevel, error_کوډ);
    أرجع شاخص;
}

int32_t
پیغام_نمونه::parseArg(int32_t شاخص, int32_t arg_پیل_اوږدتیا, int32_t nestingLevel,
                         UParseError *parseError, UError_کوډ &error_کوډ) {
    int32_t arg_پیل=parts_اوږدتیا;
    UMessage_نمونه_Arg_Type argType=UMSGPAT_ARG_TYPE_NONE;
    جمع_کړه_Part(UMSGPAT_PART_TYPE_ARG_پیل, شاخص, arg_پیل_اوږدتیا, argType, error_کوډ);
    إذا(U_FAILURE(error_کوډ)) {
        أرجع 0;
    }
    int32_t name_شاخص=شاخص=skipWhiteSpace(شاخص+arg_پیل_اوږدتیا);
    إذا(شاخص==پیغام.length()) {
        سېټ_Parse_Error(parseError, 0);  // Unmatched '{' braces in message.
        error_کوډ=U_UNMATCHED_BRACES;
        أرجع 0;
    }
    // parse argument name or number
    شاخص=skipIdentifier(شاخص);
    int32_t number=parse_Arg_شمېره(name_شاخص, شاخص);
    إذا(number>=0) {
        int32_t length=شاخص-name_شاخص;
        إذا(length>Part::MAX_اوږدتیا || number>Part::MAX_ارزښت) {
            سېټ_Parse_Error(parseError, name_شاخص);  // Argument number too large.
            error_کوډ=U_شاخص_OUTOFBOUNDS_ERROR;
            أرجع 0;
        }
        hasArgNumbers=ریښتیا;
        جمع_کړه_Part(UMSGPAT_PART_TYPE_ARG_شمېره, name_شاخص, length, number, error_کوډ);
    } وإلا إذا(number==UMSGPAT_ARG_NAME_NOT_شمېره) {
        int32_t length=شاخص-name_شاخص;
        إذا(length>Part::MAX_اوږدتیا) {
            سېټ_Parse_Error(parseError, name_شاخص);  // Argument name too long.
            error_کوډ=U_شاخص_OUTOFBOUNDS_ERROR;
            أرجع 0;
        }
        has_Arg_نومونه=ریښتیا;
        جمع_کړه_Part(UMSGPAT_PART_TYPE_ARG_NAME, name_شاخص, length, 0, error_کوډ);
    } وإلا {  // number<-1 (ARG_NAME_NOT_VALID)
        سېټ_Parse_Error(parseError, name_شاخص);  // Bad argument syntax.
        error_کوډ=U_نمونه_SYNTAX_ERROR;
        أرجع 0;
    }
    شاخص=skipWhiteSpace(شاخص);
    إذا(شاخص==پیغام.length()) {
        سېټ_Parse_Error(parseError, 0);  // Unmatched '{' braces in message.
        error_کوډ=U_UNMATCHED_BRACES;
        أرجع 0;
    }
    char16_t c=پیغام.charAt(شاخص);
    إذا(c==u_ښی_Curly_Brace) {
        // all done
    } وإلا إذا(c!=u_comma) {
        سېټ_Parse_Error(parseError, name_شاخص);  // Bad argument syntax.
        error_کوډ=U_نمونه_SYNTAX_ERROR;
        أرجع 0;
    } وإلا /* ',' */ {
        // parse argument type: case-sensitive a-zA-Z
        int32_t type_شاخص=شاخص=skipWhiteSpace(شاخص+1);
        طالما(شاخص<پیغام.length() && isArgTypeChar(پیغام.charAt(شاخص))) {
            ++شاخص;
        }
        int32_t length=شاخص-type_شاخص;
        شاخص=skipWhiteSpace(شاخص);
        إذا(شاخص==پیغام.length()) {
            سېټ_Parse_Error(parseError, 0);  // Unmatched '{' braces in message.
            error_کوډ=U_UNMATCHED_BRACES;
            أرجع 0;
        }
        إذا(length==0 || ((c=پیغام.charAt(شاخص))!=u_comma && c!=u_ښی_Curly_Brace)) {
            سېټ_Parse_Error(parseError, name_شاخص);  // Bad argument syntax.
            error_کوډ=U_نمونه_SYNTAX_ERROR;
            أرجع 0;
        }
        إذا(length>Part::MAX_اوږدتیا) {
            سېټ_Parse_Error(parseError, name_شاخص);  // Argument type name too long.
            error_کوډ=U_شاخص_OUTOFBOUNDS_ERROR;
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
        partsList->a[arg_پیل].value = static_cast<int16_t>(argType);
        إذا(argType==UMSGPAT_ARG_TYPE_SIMPLE) {
            جمع_کړه_Part(UMSGPAT_PART_TYPE_ARG_TYPE, type_شاخص, length, 0, error_کوډ);
        }
        // look for an argument style (pattern)
        إذا(c==u_ښی_Curly_Brace) {
            إذا(argType!=UMSGPAT_ARG_TYPE_SIMPLE) {
                سېټ_Parse_Error(parseError, name_شاخص);  // No style field for complex argument.
                error_کوډ=U_نمونه_SYNTAX_ERROR;
                أرجع 0;
            }
        } وإلا /* ',' */ {
            ++شاخص;
            إذا(argType==UMSGPAT_ARG_TYPE_SIMPLE) {
                شاخص=parseSimpleStyle(شاخص, parseError, error_کوډ);
            } وإلا إذا(argType==UMSGPAT_ARG_TYPE_CHOICE) {
                شاخص=parseChoiceStyle(شاخص, nestingLevel, parseError, error_کوډ);
            } وإلا {
                شاخص=parsePluralOrSelectStyle(argType, شاخص, nestingLevel, parseError, error_کوډ);
            }
        }
    }
    // Argument parsing stopped on the '}'.
    جمع_کړه_حد_Part(arg_پیل, UMSGPAT_PART_TYPE_ARG_حد, شاخص, 1, argType, error_کوډ);
    أرجع شاخص+1;
}

int32_t
پیغام_نمونه::parseSimpleStyle(int32_t شاخص, UParseError *parseError, UError_کوډ &error_کوډ) {
    إذا(U_FAILURE(error_کوډ)) {
        أرجع 0;
    }
    int32_t پیل=شاخص;
    int32_t nestedBraces=0;
    طالما(شاخص<پیغام.length()) {
        char16_t c=پیغام.charAt(شاخص++);
        إذا(c==u_apos) {
            // Treat apostrophe as quoting but include it in the style part.
            // Find the end of the quoted literal text.
            شاخص=پیغام.شاخص_د(u_apos, شاخص);
            إذا(شاخص<0) {
                // Quoted literal argument style text reaches to the end of the message.
                سېټ_Parse_Error(parseError, پیل);
                error_کوډ=U_نمونه_SYNTAX_ERROR;
                أرجع 0;
            }
            // skip the quote-ending apostrophe
            ++شاخص;
        } وإلا إذا(c==u_کیڼ_Curly_Brace) {
            ++nestedBraces;
        } وإلا إذا(c==u_ښی_Curly_Brace) {
            إذا(nestedBraces>0) {
                --nestedBraces;
            } وإلا {
                int32_t length=--شاخص-پیل;
                إذا(length>Part::MAX_اوږدتیا) {
                    سېټ_Parse_Error(parseError, پیل);  // Argument style text too long.
                    error_کوډ=U_شاخص_OUTOFBOUNDS_ERROR;
                    أرجع 0;
                }
                جمع_کړه_Part(UMSGPAT_PART_TYPE_ARG_STYLE, پیل, length, 0, error_کوډ);
                أرجع شاخص;
            }
        }  // c is part of literal text
    }
    سېټ_Parse_Error(parseError, 0);  // Unmatched '{' braces in message.
    error_کوډ=U_UNMATCHED_BRACES;
    أرجع 0;
}

int32_t
پیغام_نمونه::parseChoiceStyle(int32_t شاخص, int32_t nestingLevel,
                                 UParseError *parseError, UError_کوډ &error_کوډ) {
    إذا(U_FAILURE(error_کوډ)) {
        أرجع 0;
    }
    int32_t پیل=شاخص;
    شاخص=skipWhiteSpace(شاخص);
    إذا(شاخص==پیغام.length() || پیغام.charAt(شاخص)==u_ښی_Curly_Brace) {
        سېټ_Parse_Error(parseError, 0);  // Missing choice argument pattern.
        error_کوډ=U_نمونه_SYNTAX_ERROR;
        أرجع 0;
    }
    لكل(;;) {
        // The choice argument style contains |-separated (number, separator, message) triples.
        // Parse the number.
        int32_t شمېره_شاخص=شاخص;
        شاخص=skipDouble(شاخص);
        int32_t length=شاخص-شمېره_شاخص;
        إذا(length==0) {
            سېټ_Parse_Error(parseError, پیل);  // Bad choice pattern syntax.
            error_کوډ=U_نمونه_SYNTAX_ERROR;
            أرجع 0;
        }
        إذا(length>Part::MAX_اوږدتیا) {
            سېټ_Parse_Error(parseError, شمېره_شاخص);  // Choice number too long.
            error_کوډ=U_شاخص_OUTOFBOUNDS_ERROR;
            أرجع 0;
        }
        parseDouble(شمېره_شاخص, شاخص, ریښتیا, parseError, error_کوډ);  // adds ARG_INT or ARG_DOUBLE
        إذا(U_FAILURE(error_کوډ)) {
            أرجع 0;
        }
        // Parse the separator.
        شاخص=skipWhiteSpace(شاخص);
        إذا(شاخص==پیغام.length()) {
            سېټ_Parse_Error(parseError, پیل);  // Bad choice pattern syntax.
            error_کوډ=U_نمونه_SYNTAX_ERROR;
            أرجع 0;
        }
        char16_t c=پیغام.charAt(شاخص);
        إذا(!(c==u_pound || c==u_lessThan || c==u_lessOrEqual)) {  // U+2264 is <=
            سېټ_Parse_Error(parseError, پیل);  // Expected choice separator (#<\u2264) instead of c.
            error_کوډ=U_نمونه_SYNTAX_ERROR;
            أرجع 0;
        }
        جمع_کړه_Part(UMSGPAT_PART_TYPE_ARG_SELECTOR, شاخص, 1, 0, error_کوډ);
        // Parse the message fragment.
        شاخص=parse_پیغام(++شاخص, 0, nestingLevel+1, UMSGPAT_ARG_TYPE_CHOICE, parseError, error_کوډ);
        إذا(U_FAILURE(error_کوډ)) {
            أرجع 0;
        }
        // parseMessage(..., CHOICE) returns the index of the terminator, or msg.length().
        إذا(شاخص==پیغام.length()) {
            أرجع شاخص;
        }
        إذا(پیغام.charAt(شاخص)==u_ښی_Curly_Brace) {
            إذا(!دننه_پیغام_Format_نمونه(nestingLevel)) {
                سېټ_Parse_Error(parseError, پیل);  // Bad choice pattern syntax.
                error_کوډ=U_نمونه_SYNTAX_ERROR;
                أرجع 0;
            }
            أرجع شاخص;
        }  // else the terminator is '|'
        شاخص=skipWhiteSpace(شاخص+1);
    }
}

int32_t
پیغام_نمونه::parsePluralOrSelectStyle(UMessage_نمونه_Arg_Type argType,
                                         int32_t شاخص, int32_t nestingLevel,
                                         UParseError *parseError, UError_کوډ &error_کوډ) {
    إذا(U_FAILURE(error_کوډ)) {
        أرجع 0;
    }
    int32_t پیل=شاخص;
    UBool is_تشه=ریښتیا;
    UBool hasOther=خطأ;
    لكل(;;) {
        // First, collect the selector looking for a small set of terminators.
        // It would be a little faster to consider the syntax of each possible
        // token right here, but that makes the code too complicated.
        شاخص=skipWhiteSpace(شاخص);
        UBool eos=شاخص==پیغام.length();
        إذا(eos || پیغام.charAt(شاخص)==u_ښی_Curly_Brace) {
            إذا(eos==دننه_پیغام_Format_نمونه(nestingLevel)) {
                سېټ_Parse_Error(parseError, پیل);  // Bad plural/select pattern syntax.
                error_کوډ=U_نمونه_SYNTAX_ERROR;
                أرجع 0;
            }
            إذا(!hasOther) {
                سېټ_Parse_Error(parseError, 0);  // Missing 'other' keyword in plural/select pattern.
                error_کوډ=U_DEFAULT_KEYWORD_MISSING;
                أرجع 0;
            }
            أرجع شاخص;
        }
        int32_t selector_شاخص=شاخص;
        إذا(UMSGPAT_ARG_TYPE_HAS_PLURAL_STYLE(argType) && پیغام.charAt(selector_شاخص)==u_equal) {
            // explicit-value plural selector: =double
            شاخص=skipDouble(شاخص+1);
            int32_t length=شاخص-selector_شاخص;
            إذا(length==1) {
                سېټ_Parse_Error(parseError, پیل);  // Bad plural/select pattern syntax.
                error_کوډ=U_نمونه_SYNTAX_ERROR;
                أرجع 0;
            }
            إذا(length>Part::MAX_اوږدتیا) {
                سېټ_Parse_Error(parseError, selector_شاخص);  // Argument selector too long.
                error_کوډ=U_شاخص_OUTOFBOUNDS_ERROR;
                أرجع 0;
            }
            جمع_کړه_Part(UMSGPAT_PART_TYPE_ARG_SELECTOR, selector_شاخص, length, 0, error_کوډ);
            parseDouble(selector_شاخص+1, شاخص, خطأ,
                        parseError, error_کوډ);  // adds ARG_INT or ARG_DOUBLE
        } وإلا {
            شاخص=skipIdentifier(شاخص);
            int32_t length=شاخص-selector_شاخص;
            إذا(length==0) {
                سېټ_Parse_Error(parseError, پیل);  // Bad plural/select pattern syntax.
                error_کوډ=U_نمونه_SYNTAX_ERROR;
                أرجع 0;
            }
            // Note: The ':' in "offset:" is just beyond the skipIdentifier() range.
            إذا( UMSGPAT_ARG_TYPE_HAS_PLURAL_STYLE(argType) && length==6 && شاخص<پیغام.length() &&
                0==پیغام.compare(selector_شاخص, 7, k_دننه_واټن_Colon, 0, 7)
            ) {
                // plural offset, not a selector
                إذا(!is_تشه) {
                    // Plural argument 'offset:' (if present) must precede key-message pairs.
                    سېټ_Parse_Error(parseError, پیل);
                    error_کوډ=U_نمونه_SYNTAX_ERROR;
                    أرجع 0;
                }
                // allow whitespace between offset: and its value
                int32_t ارزښت_شاخص=skipWhiteSpace(شاخص+1);  // The ':' is at index.
                شاخص=skipDouble(ارزښت_شاخص);
                إذا(شاخص==ارزښت_شاخص) {
                    سېټ_Parse_Error(parseError, پیل);  // Missing value for plural 'offset:'.
                    error_کوډ=U_نمونه_SYNTAX_ERROR;
                    أرجع 0;
                }
                إذا((شاخص-ارزښت_شاخص)>Part::MAX_اوږدتیا) {
                    سېټ_Parse_Error(parseError, ارزښت_شاخص);  // Plural offset value too long.
                    error_کوډ=U_شاخص_OUTOFBOUNDS_ERROR;
                    أرجع 0;
                }
                parseDouble(ارزښت_شاخص, شاخص, خطأ,
                            parseError, error_کوډ);  // adds ARG_INT or ARG_DOUBLE
                إذا(U_FAILURE(error_کوډ)) {
                    أرجع 0;
                }
                is_تشه=خطأ;
                استمر;  // no message fragment after the offset
            } وإلا {
                // normal selector word
                إذا(length>Part::MAX_اوږدتیا) {
                    سېټ_Parse_Error(parseError, selector_شاخص);  // Argument selector too long.
                    error_کوډ=U_شاخص_OUTOFBOUNDS_ERROR;
                    أرجع 0;
                }
                جمع_کړه_Part(UMSGPAT_PART_TYPE_ARG_SELECTOR, selector_شاخص, length, 0, error_کوډ);
                إذا(0==پیغام.compare(selector_شاخص, length, kOther, 0, 5)) {
                    hasOther=ریښتیا;
                }
            }
        }
        إذا(U_FAILURE(error_کوډ)) {
            أرجع 0;
        }

        // parse the message fragment following the selector
        شاخص=skipWhiteSpace(شاخص);
        إذا(شاخص==پیغام.length() || پیغام.charAt(شاخص)!=u_کیڼ_Curly_Brace) {
            سېټ_Parse_Error(parseError, selector_شاخص);  // No message fragment after plural/select selector.
            error_کوډ=U_نمونه_SYNTAX_ERROR;
            أرجع 0;
        }
        شاخص=parse_پیغام(شاخص, 1, nestingLevel+1, argType, parseError, error_کوډ);
        إذا(U_FAILURE(error_کوډ)) {
            أرجع 0;
        }
        is_تشه=خطأ;
    }
}

int32_t
پیغام_نمونه::parse_Arg_شمېره(ثابت UnicodeString &s, int32_t پیل, int32_t حد) {
    // If the identifier contains only ASCII digits, then it is an argument _number_
    // and must not have leading zeros (except "0" itself).
    // Otherwise it is an argument _name_.
    إذا(پیل>=حد) {
        أرجع UMSGPAT_ARG_NAME_NOT_VALID;
    }
    int32_t number;
    // Defer numeric errors until we know there are only digits.
    UBool bad_شمېره;
    char16_t c=s.charAt(پیل++);
    إذا(c==0x30) {
        إذا(پیل==حد) {
            أرجع 0;
        } وإلا {
            number=0;
            bad_شمېره=ریښتیا;  // leading zero
        }
    } وإلا إذا(0x31<=c && c<=0x39) {
        number=c-0x30;
        bad_شمېره=خطأ;
    } وإلا {
        أرجع UMSGPAT_ARG_NAME_NOT_شمېره;
    }
    طالما(پیل<حد) {
        c=s.charAt(پیل++);
        إذا(0x30<=c && c<=0x39) {
            إذا(number>=INT32_MAX/10) {
                bad_شمېره=ریښتیا;  //ډکېدنه
            } وإلا {
                number=number*10+(c-0x30);
            }
        } وإلا {
            أرجع UMSGPAT_ARG_NAME_NOT_شمېره;
        }
    }
    // There are only ASCII digits.
    إذا(bad_شمېره) {
        أرجع UMSGPAT_ARG_NAME_NOT_VALID;
    } وإلا {
        أرجع number;
    }
}

فراغ
پیغام_نمونه::parseDouble(int32_t پیل, int32_t حد, UBool allowInfinity,
                            UParseError *parseError, UError_کوډ &error_کوډ) {
    إذا(U_FAILURE(error_کوډ)) {
        أرجع;
    }
    U_ASSERT(پیل<حد);
    // fake loop for easy exit and single throw statement
    لكل(;;) { /*loop doesn't iterate*/
        // fast path for small integers and infinity
        int32_t value=0;
        int32_t isNegative=0;  // not boolean so that we can easily add it to value
        int32_t شاخص=پیل;
        char16_t c=پیغام.charAt(شاخص++);
        إذا(c==u_minus) {
            isNegative=1;
            إذا(شاخص==حد) {
                اكسر;  // no number
            }
            c=پیغام.charAt(شاخص++);
        } وإلا إذا(c==u_plus) {
            إذا(شاخص==حد) {
                اكسر;  // no number
            }
            c=پیغام.charAt(شاخص++);
        }
        إذا(c==0x221e) {  // infinity
            إذا(allowInfinity && شاخص==حد) {
                مزدوج infinity=uprv_getInfinity();
                جمع_کړه_Arg_Double_Part(
                    isNegative!=0 ? -infinity : infinity,
                    پیل, حد-پیل, error_کوډ);
                أرجع;
            } وإلا {
                اكسر;
            }
        }
        // try to parse the number as a small integer but fall back to a double
        طالما('0'<=c && c<='9') {
            value=value*10+(c-'0');
            إذا(value>(Part::MAX_ارزښت+isNegative)) {
                اكسر;  // not a small-enough integer
            }
            إذا(شاخص==حد) {
                جمع_کړه_Part(UMSGPAT_PART_TYPE_ARG_INT, پیل, حد-پیل,
                        isNegative!=0 ? -value : value, error_کوډ);
                أرجع;
            }
            c=پیغام.charAt(شاخص++);
        }
        // Let Double.parseDouble() throw a NumberFormatException.
        حرف شمېره_Chars[128];
        int32_t capacity = static_cast<int32_t>(حجم(شمېره_Chars));
        int32_t length=حد-پیل;
        إذا(length>=capacity) {
            اكسر;  // number too long
        }
        پیغام.extract(پیل, length, شمېره_Chars, capacity, US_INV);
        إذا (static_cast<int32_t>(uprv_strlen(شمېره_Chars)) < length) {
            اكسر;  // contains non-invariant character that was turned into NUL
        }
        حرف *پای_ټکی;
        مزدوج numeric_ارزښت=uprv_strtod(شمېره_Chars, &پای_ټکی);
        إذا(پای_ټکی!=(شمېره_Chars+length)) {
            اكسر;  // parsing error
        }
        جمع_کړه_Arg_Double_Part(numeric_ارزښت, پیل, length, error_کوډ);
        أرجع;
    }
    سېټ_Parse_Error(parseError, پیل /*, limit*/);  // Bad syntax for numeric value.
    error_کوډ=U_نمونه_SYNTAX_ERROR;
}

int32_t
پیغام_نمونه::skipWhiteSpace(int32_t شاخص) {
    ثابت char16_t *s=پیغام.get_بفر();
    int32_t پیغام_اوږدتیا=پیغام.length();
    ثابت char16_t *t=نمونه_Props::skipWhiteSpace(s+شاخص, پیغام_اوږدتیا-شاخص);
    أرجع static_cast<int32_t>(t - s);
}

int32_t
پیغام_نمونه::skipIdentifier(int32_t شاخص) {
    ثابت char16_t *s=پیغام.get_بفر();
    int32_t پیغام_اوږدتیا=پیغام.length();
    ثابت char16_t *t=نمونه_Props::skipIdentifier(s+شاخص, پیغام_اوږدتیا-شاخص);
    أرجع static_cast<int32_t>(t - s);
}

int32_t
پیغام_نمونه::skipDouble(int32_t شاخص) {
    int32_t پیغام_اوږدتیا=پیغام.length();
    طالما(شاخص<پیغام_اوږدتیا) {
        char16_t c=پیغام.charAt(شاخص);
        // U+221E: Allow the infinity symbol, for ChoiceFormat patterns.
        إذا((c<0x30 && c!=u_plus && c!=u_minus && c!=u_dot) || (c>0x39 && c!=u_e && c!=u_E && c!=0x221e)) {
            اكسر;
        }
        ++شاخص;
    }
    أرجع شاخص;
}

UBool
پیغام_نمونه::isArgTypeChar(UChar32 c) {
    أرجع (u_a<=c && c<=u_z) || (u_A<=c && c<=u_Z);
}

UBool
پیغام_نمونه::isChoice(int32_t شاخص) {
    char16_t c;
    أرجع
        ((c=پیغام.charAt(شاخص++))==u_c || c==u_C) &&
        ((c=پیغام.charAt(شاخص++))==u_h || c==u_H) &&
        ((c=پیغام.charAt(شاخص++))==u_o || c==u_O) &&
        ((c=پیغام.charAt(شاخص++))==u_i || c==u_I) &&
        ((c=پیغام.charAt(شاخص++))==u_c || c==u_C) &&
        ((c=پیغام.charAt(شاخص))==u_e || c==u_E);
}

UBool
پیغام_نمونه::isPlural(int32_t شاخص) {
    char16_t c;
    أرجع
        ((c=پیغام.charAt(شاخص++))==u_p || c==u_P) &&
        ((c=پیغام.charAt(شاخص++))==u_l || c==u_L) &&
        ((c=پیغام.charAt(شاخص++))==u_u || c==u_U) &&
        ((c=پیغام.charAt(شاخص++))==u_r || c==u_R) &&
        ((c=پیغام.charAt(شاخص++))==u_a || c==u_A) &&
        ((c=پیغام.charAt(شاخص))==u_l || c==u_L);
}

UBool
پیغام_نمونه::isSelect(int32_t شاخص) {
    char16_t c;
    أرجع
        ((c=پیغام.charAt(شاخص++))==u_s || c==u_S) &&
        ((c=پیغام.charAt(شاخص++))==u_e || c==u_E) &&
        ((c=پیغام.charAt(شاخص++))==u_l || c==u_L) &&
        ((c=پیغام.charAt(شاخص++))==u_e || c==u_E) &&
        ((c=پیغام.charAt(شاخص++))==u_c || c==u_C) &&
        ((c=پیغام.charAt(شاخص))==u_t || c==u_T);
}

UBool
پیغام_نمونه::isOrdinal(int32_t شاخص) {
    char16_t c;
    أرجع
        ((c=پیغام.charAt(شاخص++))==u_o || c==u_O) &&
        ((c=پیغام.charAt(شاخص++))==u_r || c==u_R) &&
        ((c=پیغام.charAt(شاخص++))==u_d || c==u_D) &&
        ((c=پیغام.charAt(شاخص++))==u_i || c==u_I) &&
        ((c=پیغام.charAt(شاخص++))==u_n || c==u_N) &&
        ((c=پیغام.charAt(شاخص++))==u_a || c==u_A) &&
        ((c=پیغام.charAt(شاخص))==u_l || c==u_L);
}

UBool
پیغام_نمونه::دننه_پیغام_Format_نمونه(int32_t nestingLevel) {
    أرجع nestingLevel>0 || partsList->a[0].type==UMSGPAT_PART_TYPE_پیغام_پیل;
}

UBool
پیغام_نمونه::دننه_سر_Level_Choice_پیغام(int32_t nestingLevel, UMessage_نمونه_Arg_Type پلار_Type) {
    أرجع
        nestingLevel==1 &&
        پلار_Type==UMSGPAT_ARG_TYPE_CHOICE &&
        partsList->a[0].type!=UMSGPAT_PART_TYPE_پیغام_پیل;
}

فراغ
پیغام_نمونه::جمع_کړه_Part(UMessage_نمونه_Part_Type type, int32_t شاخص, int32_t length,
                        int32_t value, UError_کوډ &error_کوډ) {
    إذا(partsList->ensure_ظرفیت_For_One_نور(parts_اوږدتیا, error_کوډ)) {
        Part &part=partsList->a[parts_اوږدتیا++];
        part.type=type;
        part.شاخص=شاخص;
        part.length = static_cast<uint16_t>(length);
        part.value = static_cast<int16_t>(value);
        part.حد_Part_شاخص=0;
    }
}

فراغ
پیغام_نمونه::جمع_کړه_حد_Part(int32_t پیل,
                             UMessage_نمونه_Part_Type type, int32_t شاخص, int32_t length,
                             int32_t value, UError_کوډ &error_کوډ) {
    partsList->a[پیل].حد_Part_شاخص=parts_اوږدتیا;
    جمع_کړه_Part(type, شاخص, length, value, error_کوډ);
}

فراغ
پیغام_نمونه::جمع_کړه_Arg_Double_Part(مزدوج numeric_ارزښت, int32_t پیل, int32_t length,
                                 UError_کوډ &error_کوډ) {
    إذا(U_FAILURE(error_کوډ)) {
        أرجع;
    }
    int32_t numeric_شاخص=numeric_ارزښتونه_اوږدتیا;
    إذا(numeric_ارزښتونه_List==تش_اشاره) {
        numeric_ارزښتونه_List=جديد پیغام_نمونه_Double_List();
        إذا(numeric_ارزښتونه_List==تش_اشاره) {
            error_کوډ=U_حافظه_ویش_ERROR;
            أرجع;
        }
    } وإلا إذا(!numeric_ارزښتونه_List->ensure_ظرفیت_For_One_نور(numeric_ارزښتونه_اوږدتیا, error_کوډ)) {
        أرجع;
    } وإلا {
        إذا(numeric_شاخص>Part::MAX_ارزښت) {
            error_کوډ=U_شاخص_OUTOFBOUNDS_ERROR;
            أرجع;
        }
    }
    numeric_ارزښتونه_List->a[numeric_ارزښتونه_اوږدتیا++]=numeric_ارزښت;
    جمع_کړه_Part(UMSGPAT_PART_TYPE_ARG_DOUBLE, پیل, length, numeric_شاخص, error_کوډ);
}

فراغ
پیغام_نمونه::سېټ_Parse_Error(UParseError *parseError, int32_t شاخص) {
    إذا(parseError==تش_اشاره) {
        أرجع;
    }
    parseError->دننه_واټن=شاخص;

    // Set preContext to some of msg before index.
    // Avoid splitting a surrogate pair.
    int32_t length=شاخص;
    إذا(length>=U_PARSE_CONTEXT_LEN) {
        length=U_PARSE_CONTEXT_LEN-1;
        إذا(length>0 && U16_IS_TRAIL(پیغام[شاخص-length])) {
            --length;
        }
    }
    پیغام.extract(شاخص-length, length, parseError->preContext);
    parseError->preContext[length]=0;

    // Set postContext to some of msg starting at index.
    length=پیغام.length()-شاخص;
    إذا(length>=U_PARSE_CONTEXT_LEN) {
        length=U_PARSE_CONTEXT_LEN-1;
        إذا(length>0 && U16_IS_LEAD(پیغام[شاخص+length-1])) {
            --length;
        }
    }
    پیغام.extract(شاخص, length, parseError->postContext);
    parseError->postContext[length]=0;
}

// MessageImpl ------------------------------------------------------------- ***

فراغ
پیغام_Impl::appendReducedApostrophes(ثابت UnicodeString &s, int32_t پیل, int32_t حد,
                                      UnicodeString &sb) {
    int32_t doubleApos=-1;
    لكل(;;) {
        int32_t i=s.شاخص_د(u_apos, پیل);
        إذا(i<0 || i>=حد) {
            sb.append(s, پیل, حد-پیل);
            اكسر;
        }
        إذا(i==doubleApos) {
            // Double apostrophe at start-1 and start==i, append one.
            sb.append(u_apos);
            ++پیل;
            doubleApos=-1;
        } وإلا {
            // Append text between apostrophes and skip this one.
            sb.append(s, پیل, i-پیل);
            doubleApos=پیل=i+1;
        }
    }
}

// Ported from second half of ICU4J SelectFormat.format(String).
UnicodeString&
پیغام_Impl::append_Sub_پیغام_Without_Skip_Syntax(ثابت پیغام_نمونه& پیغام_نمونه_2,
                                               int32_t پیغام_پیل,
                                               UnicodeString& پایله U_LIFETIME_BOUND) {
    ثابت UnicodeString &پیغام_String=پیغام_نمونه_2.get_نمونه_String();
    int32_t prev_شاخص=پیغام_نمونه_2.getPart(پیغام_پیل).get_حد();
    لكل(int32_t i=پیغام_پیل;;) {
        ثابت پیغام_نمونه::Part &part=پیغام_نمونه_2.getPart(++i);
        UMessage_نمونه_Part_Type type=part.getType();
        int32_t شاخص=part.get_شاخص();
        إذا(type==UMSGPAT_PART_TYPE_پیغام_حد) {
            أرجع پایله.append(پیغام_String, prev_شاخص, شاخص-prev_شاخص);
        } وإلا إذا(type==UMSGPAT_PART_TYPE_SKIP_SYNTAX) {
            پایله.append(پیغام_String, prev_شاخص, شاخص-prev_شاخص);
            prev_شاخص=part.get_حد();
        } وإلا إذا(type==UMSGPAT_PART_TYPE_ARG_پیل) {
            پایله.append(پیغام_String, prev_شاخص, شاخص-prev_شاخص);
            prev_شاخص=شاخص;
            i=پیغام_نمونه_2.get_حد_Part_شاخص(i);
            شاخص=پیغام_نمونه_2.getPart(i).get_حد();
            appendReducedApostrophes(پیغام_String, prev_شاخص, شاخص, پایله);
            prev_شاخص=شاخص;
        }
    }
}

U_NAMESPACE_پای_ټکی

#د_که_پای  // !UCONFIG_NO_FORMATTING
