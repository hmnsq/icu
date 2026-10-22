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
ساكن ثابت char16_t u_کاٻو_Curly_Brace=0x7B;
ساكن ثابت char16_t u_pipe=0x7C;
ساكن ثابت char16_t u_ساڄو_Curly_Brace=0x7D;
ساكن ثابت char16_t u_lessOrEqual=0x2264;  // U+2264 is <=

ساكن ثابت char16_t k_وٿي_Colon[]={  // "offset:"
    u_o, u_f, u_f, u_s, u_e, u_t, u_colon
};

ساكن ثابت char16_t kOther[]={  // "other"
    u_o, u_t, u_h, u_e, u_r
};

// MessagePatternList ------------------------------------------------------ ***

سانچو<قسم_نالو T, int32_t ڍير_صلاحيت>
صنف پيغام_نمونو_List : عام UMemory {
عام:
    پيغام_نمونو_List() {}
    فراغ copyFrom(ثابت پيغام_نمونو_List<T, ڍير_صلاحيت> &other,
                  int32_t length,
                  UError_ڪوڊ &error_ڪوڊ);
    UBool ensure_صلاحيت_For_One_وڌيڪ(int32_t old_ڊيگهائي, UError_ڪوڊ &error_ڪوڊ);
    UBool equals(ثابت پيغام_نمونو_List<T, ڍير_صلاحيت> &other, int32_t length) ثابت {
        لكل(int32_t i=0; i<length; ++i) {
            إذا(a[i]!=other.a[i]) { أرجع خطأ; }
        }
        أرجع سچ;
    }

    Maybe_ڍير_Array<T, ڍير_صلاحيت> a;
};

سانچو<قسم_نالو T, int32_t ڍير_صلاحيت>
فراغ
پيغام_نمونو_List<T, ڍير_صلاحيت>::copyFrom(
        ثابت پيغام_نمونو_List<T, ڍير_صلاحيت> &other,
        int32_t length,
        UError_ڪوڊ &error_ڪوڊ) {
    إذا(U_SUCCESS(error_ڪوڊ) && length>0) {
        إذا(length>a.get_صلاحيت() && خالي_اشارو==a.resize(length)) {
            error_ڪوڊ=U_يادگيري_ورهاست_ERROR;
            أرجع;
        }
        uprv_memcpy(a.getAlias(), other.a.getAlias(), (size_t)length*حجم(T));
    }
}

سانچو<قسم_نالو T, int32_t ڍير_صلاحيت>
UBool
پيغام_نمونو_List<T, ڍير_صلاحيت>::ensure_صلاحيت_For_One_وڌيڪ(int32_t old_ڊيگهائي, UError_ڪوڊ &error_ڪوڊ) {
    إذا(U_FAILURE(error_ڪوڊ)) {
        أرجع خطأ;
    }
    إذا(a.get_صلاحيت()>old_ڊيگهائي || a.resize(2*old_ڊيگهائي, old_ڊيگهائي)!=خالي_اشارو) {
        أرجع سچ;
    }
    error_ڪوڊ=U_يادگيري_ورهاست_ERROR;
    أرجع خطأ;
}

// MessagePatternList specializations -------------------------------------- ***

صنف پيغام_نمونو_Double_List : عام پيغام_نمونو_List<مزدوج, 8> {
};

صنف پيغام_نمونو_Parts_List : عام پيغام_نمونو_List<پيغام_نمونو::Part, 32> {
};

// MessagePattern constructors etc. ---------------------------------------- ***

پيغام_نمونو::پيغام_نمونو(UError_ڪوڊ &error_ڪوڊ)
        : apos_گهڻائي(UCONFIG_MSGPAT_DEFAULT_APOSTROPHE_گهڻائي),
          partsList(خالي_اشارو), parts(خالي_اشارو), parts_ڊيگهائي(0),
          numeric_قدرون_List(خالي_اشارو), numeric_قدرون(خالي_اشارو), numeric_قدرون_ڊيگهائي(0),
          has_Arg_نالا(خطأ), hasArgNumbers(خطأ), needsAutoQuoting(خطأ) {
    init(error_ڪوڊ);
}

پيغام_نمونو::پيغام_نمونو(UMessage_نمونو_Apostrophe_گهڻائي گهڻائي, UError_ڪوڊ &error_ڪوڊ)
        : apos_گهڻائي(گهڻائي),
          partsList(خالي_اشارو), parts(خالي_اشارو), parts_ڊيگهائي(0),
          numeric_قدرون_List(خالي_اشارو), numeric_قدرون(خالي_اشارو), numeric_قدرون_ڊيگهائي(0),
          has_Arg_نالا(خطأ), hasArgNumbers(خطأ), needsAutoQuoting(خطأ) {
    init(error_ڪوڊ);
}

پيغام_نمونو::پيغام_نمونو(ثابت UnicodeString &نمونو, UParseError *parseError, UError_ڪوڊ &error_ڪوڊ)
        : apos_گهڻائي(UCONFIG_MSGPAT_DEFAULT_APOSTROPHE_گهڻائي),
          partsList(خالي_اشارو), parts(خالي_اشارو), parts_ڊيگهائي(0),
          numeric_قدرون_List(خالي_اشارو), numeric_قدرون(خالي_اشارو), numeric_قدرون_ڊيگهائي(0),
          has_Arg_نالا(خطأ), hasArgNumbers(خطأ), needsAutoQuoting(خطأ) {
    إذا(init(error_ڪوڊ)) {
        parse(نمونو, parseError, error_ڪوڊ);
    }
}

UBool
پيغام_نمونو::init(UError_ڪوڊ &error_ڪوڊ) {
    إذا(U_FAILURE(error_ڪوڊ)) {
        أرجع خطأ;
    }
    partsList=جديد پيغام_نمونو_Parts_List();
    إذا(partsList==خالي_اشارو) {
        error_ڪوڊ=U_يادگيري_ورهاست_ERROR;
        أرجع خطأ;
    }
    parts=partsList->a.getAlias();
    أرجع سچ;
}

پيغام_نمونو::پيغام_نمونو(ثابت پيغام_نمونو &other)
        : UObject(other), apos_گهڻائي(other.apos_گهڻائي), نياپو(other.نياپو),
          partsList(خالي_اشارو), parts(خالي_اشارو), parts_ڊيگهائي(0),
          numeric_قدرون_List(خالي_اشارو), numeric_قدرون(خالي_اشارو), numeric_قدرون_ڊيگهائي(0),
          has_Arg_نالا(other.has_Arg_نالا), hasArgNumbers(other.hasArgNumbers),
          needsAutoQuoting(other.needsAutoQuoting) {
    UError_ڪوڊ error_ڪوڊ=U_ZERO_ERROR;
    إذا(!copyStorage(other, error_ڪوڊ)) {
        clear();
    }
}

پيغام_نمونو&
پيغام_نمونو::عامل=(ثابت پيغام_نمونو& other) U_LIFETIME_BOUND {
    إذا(هذا==&other) {
        أرجع *هذا;
    }
    apos_گهڻائي=other.apos_گهڻائي;
    نياپو=other.نياپو;
    has_Arg_نالا=other.has_Arg_نالا;
    hasArgNumbers=other.hasArgNumbers;
    needsAutoQuoting=other.needsAutoQuoting;
    UError_ڪوڊ error_ڪوڊ=U_ZERO_ERROR;
    إذا(!copyStorage(other, error_ڪوڊ)) {
        clear();
    }
    أرجع *هذا;
}

UBool
پيغام_نمونو::copyStorage(ثابت پيغام_نمونو &other, UError_ڪوڊ &error_ڪوڊ) {
    إذا(U_FAILURE(error_ڪوڊ)) {
        أرجع خطأ;
    }
    parts=خالي_اشارو;
    parts_ڊيگهائي=0;
    numeric_قدرون=خالي_اشارو;
    numeric_قدرون_ڊيگهائي=0;
    إذا(partsList==خالي_اشارو) {
        partsList=جديد پيغام_نمونو_Parts_List();
        إذا(partsList==خالي_اشارو) {
            error_ڪوڊ=U_يادگيري_ورهاست_ERROR;
            أرجع خطأ;
        }
        parts=partsList->a.getAlias();
    }
    إذا(other.parts_ڊيگهائي>0) {
        partsList->copyFrom(*other.partsList, other.parts_ڊيگهائي, error_ڪوڊ);
        إذا(U_FAILURE(error_ڪوڊ)) {
            أرجع خطأ;
        }
        parts=partsList->a.getAlias();
        parts_ڊيگهائي=other.parts_ڊيگهائي;
    }
    إذا(other.numeric_قدرون_ڊيگهائي>0) {
        إذا(numeric_قدرون_List==خالي_اشارو) {
            numeric_قدرون_List=جديد پيغام_نمونو_Double_List();
            إذا(numeric_قدرون_List==خالي_اشارو) {
                error_ڪوڊ=U_يادگيري_ورهاست_ERROR;
                أرجع خطأ;
            }
            numeric_قدرون=numeric_قدرون_List->a.getAlias();
        }
        numeric_قدرون_List->copyFrom(
            *other.numeric_قدرون_List, other.numeric_قدرون_ڊيگهائي, error_ڪوڊ);
        إذا(U_FAILURE(error_ڪوڊ)) {
            أرجع خطأ;
        }
        numeric_قدرون=numeric_قدرون_List->a.getAlias();
        numeric_قدرون_ڊيگهائي=other.numeric_قدرون_ڊيگهائي;
    }
    أرجع سچ;
}

پيغام_نمونو::~پيغام_نمونو() {
    ميٽيو partsList;
    ميٽيو numeric_قدرون_List;
}

// MessagePattern API ------------------------------------------------------ ***

پيغام_نمونو&
پيغام_نمونو::parse(ثابت UnicodeString& نمونو,
                      UParseError* parseError,
                      UError_ڪوڊ& error_ڪوڊ) U_LIFETIME_BOUND {
    preParse(نمونو, parseError, error_ڪوڊ);
    parse_پيغام(0, 0, 0, UMSGPAT_ARG_TYPE_NONE, parseError, error_ڪوڊ);
    postParse();
    أرجع *هذا;
}

پيغام_نمونو&
پيغام_نمونو::parseChoiceStyle(ثابت UnicodeString& نمونو,
                                 UParseError* parseError,
                                 UError_ڪوڊ& error_ڪوڊ) U_LIFETIME_BOUND {
    preParse(نمونو, parseError, error_ڪوڊ);
    parseChoiceStyle(0, 0, parseError, error_ڪوڊ);
    postParse();
    أرجع *هذا;
}

پيغام_نمونو&
پيغام_نمونو::parsePluralStyle(ثابت UnicodeString& نمونو,
                                 UParseError* parseError,
                                 UError_ڪوڊ& error_ڪوڊ) U_LIFETIME_BOUND {
    preParse(نمونو, parseError, error_ڪوڊ);
    parsePluralOrSelectStyle(UMSGPAT_ARG_TYPE_PLURAL, 0, 0, parseError, error_ڪوڊ);
    postParse();
    أرجع *هذا;
}

پيغام_نمونو&
پيغام_نمونو::parseSelectStyle(ثابت UnicodeString& نمونو,
                                 UParseError* parseError,
                                 UError_ڪوڊ& error_ڪوڊ) U_LIFETIME_BOUND {
    preParse(نمونو, parseError, error_ڪوڊ);
    parsePluralOrSelectStyle(UMSGPAT_ARG_TYPE_SELECT, 0, 0, parseError, error_ڪوڊ);
    postParse();
    أرجع *هذا;
}

فراغ
پيغام_نمونو::clear() {
    // Mostly the same as preParse().
    نياپو.remove();
    has_Arg_نالا=hasArgNumbers=خطأ;
    needsAutoQuoting=خطأ;
    parts_ڊيگهائي=0;
    numeric_قدرون_ڊيگهائي=0;
}

منطقي
پيغام_نمونو::عامل==(ثابت پيغام_نمونو &other) ثابت {
    إذا(هذا==&other) {
        أرجع سچ;
    }
    أرجع
        apos_گهڻائي==other.apos_گهڻائي &&
        نياپو==other.نياپو &&
        // parts.equals(o.parts)
        parts_ڊيگهائي==other.parts_ڊيگهائي &&
        (parts_ڊيگهائي==0 || partsList->equals(*other.partsList, parts_ڊيگهائي));
    // No need to compare numericValues if msg and parts are the same.
}

int32_t
پيغام_نمونو::hash_ڪوڊ() ثابت {
    int32_t hash=(apos_گهڻائي*37+نياپو.hash_ڪوڊ())*37+parts_ڊيگهائي;
    لكل(int32_t i=0; i<parts_ڊيگهائي; ++i) {
        hash=hash*37+parts[i].hash_ڪوڊ();
    }
    أرجع hash;
}

int32_t
پيغام_نمونو::validate_دليل_Name(ثابت UnicodeString &name) {
    إذا(!نمونو_Props::isIdentifier(name.get_بفر(), name.length())) {
        أرجع UMSGPAT_ARG_NAME_NOT_VALID;
    }
    أرجع parse_Arg_عدد(name, 0, name.length());
}

UnicodeString
پيغام_نمونو::autoQuoteApostropheDeep() ثابت {
    إذا(!needsAutoQuoting) {
        أرجع نياپو;
    }
    UnicodeString modified(نياپو);
    // Iterate backward so that the insertion indexes do not change.
    int32_t count=ڳڻپ_Parts();
    لكل(int32_t i=count; i>0;) {
        ثابت Part &part=getPart(--i);
        إذا(part.getType()==UMSGPAT_PART_TYPE_وجهو_CHAR) {
           modified.insert(part.اشاريو, static_cast<char16_t>(part.value));
        }
    }
    أرجع modified;
}

مزدوج
پيغام_نمونو::get_Numeric_قدر(ثابت Part &part) ثابت {
    UMessage_نمونو_Part_Type type=part.type;
    إذا(type==UMSGPAT_PART_TYPE_ARG_INT) {
        أرجع part.value;
    } وإلا إذا(type==UMSGPAT_PART_TYPE_ARG_DOUBLE) {
        أرجع numeric_قدرون[part.value];
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
پيغام_نمونو::get_Plural_وٿي(int32_t plural_شروعات) ثابت {
    ثابت Part &part=getPart(plural_شروعات);
    إذا(Part::has_Numeric_قدر(part.type)) {
        أرجع get_Numeric_قدر(part);
    } وإلا {
        أرجع 0;
    }
}

// MessagePattern::Part ---------------------------------------------------- ***

منطقي
پيغام_نمونو::Part::عامل==(ثابت Part &other) ثابت {
    إذا(هذا==&other) {
        أرجع سچ;
    }
    أرجع
        type==other.type &&
        اشاريو==other.اشاريو &&
        length==other.length &&
        value==other.value &&
        حد_بندي_Part_اشاريو==other.حد_بندي_Part_اشاريو;
}

// MessagePattern parser --------------------------------------------------- ***

فراغ
پيغام_نمونو::preParse(ثابت UnicodeString &نمونو, UParseError *parseError, UError_ڪوڊ &error_ڪوڊ) {
    إذا(U_FAILURE(error_ڪوڊ)) {
        أرجع;
    }
    إذا(parseError!=خالي_اشارو) {
        parseError->سٽ=0;
        parseError->وٿي=0;
        parseError->preContext[0]=0;
        parseError->postContext[0]=0;
    }
    نياپو=نمونو;
    has_Arg_نالا=hasArgNumbers=خطأ;
    needsAutoQuoting=خطأ;
    parts_ڊيگهائي=0;
    numeric_قدرون_ڊيگهائي=0;
}

فراغ
پيغام_نمونو::postParse() {
    إذا(partsList!=خالي_اشارو) {
        parts=partsList->a.getAlias();
    }
    إذا(numeric_قدرون_List!=خالي_اشارو) {
        numeric_قدرون=numeric_قدرون_List->a.getAlias();
    }
}

int32_t
پيغام_نمونو::parse_پيغام(int32_t اشاريو, int32_t نياپو_شروعات_ڊيگهائي,
                             int32_t nestingLevel, UMessage_نمونو_Arg_Type والد_Type,
                             UParseError *parseError, UError_ڪوڊ &error_ڪوڊ) {
    إذا(U_FAILURE(error_ڪوڊ)) {
        أرجع 0;
    }
    إذا(nestingLevel>Part::MAX_NESTED_سطحون) {
        error_ڪوڊ=U_اشاريو_OUTOFBOUNDS_ERROR;
        أرجع 0;
    }
    int32_t نياپو_شروعات=parts_ڊيگهائي;
    جوڙيو_Part(UMSGPAT_PART_TYPE_نياپو_شروعات, اشاريو, نياپو_شروعات_ڊيگهائي, nestingLevel, error_ڪوڊ);
    اشاريو+=نياپو_شروعات_ڊيگهائي;
    لكل(;;) {  // while(index<msg.length()) with U_FAILURE(errorCode) check
        إذا(U_FAILURE(error_ڪوڊ)) {
            أرجع 0;
        }
        إذا(اشاريو>=نياپو.length()) {
            اكسر;
        }
        char16_t c=نياپو.charAt(اشاريو++);
        إذا(c==u_apos) {
            إذا(اشاريو==نياپو.length()) {
                // The apostrophe is the last character in the pattern. 
                // Add a Part for auto-quoting.
                جوڙيو_Part(UMSGPAT_PART_TYPE_وجهو_CHAR, اشاريو, 0,
                        u_apos, error_ڪوڊ);  // value=char to be inserted
                needsAutoQuoting=سچ;
            } وإلا {
                c=نياپو.charAt(اشاريو);
                إذا(c==u_apos) {
                    // double apostrophe, skip the second one
                    جوڙيو_Part(UMSGPAT_PART_TYPE_SKIP_SYNTAX, اشاريو++, 1, 0, error_ڪوڊ);
                } وإلا إذا(
                    apos_گهڻائي==UMSGPAT_APOS_DOUBLE_REQUIRED ||
                    c==u_کاٻو_Curly_Brace || c==u_ساڄو_Curly_Brace ||
                    (والد_Type==UMSGPAT_ARG_TYPE_CHOICE && c==u_pipe) ||
                    (UMSGPAT_ARG_TYPE_HAS_PLURAL_STYLE(والد_Type) && c==u_pound)
                ) {
                    // skip the quote-starting apostrophe
                    جوڙيو_Part(UMSGPAT_PART_TYPE_SKIP_SYNTAX, اشاريو-1, 1, 0, error_ڪوڊ);
                    // find the end of the quoted literal text
                    لكل(;;) {
                        اشاريو=نياپو.اشاريو_جو(u_apos, اشاريو+1);
                        إذا(اشاريو>=0) {
                            إذا(/*(index+1)<msg.length() &&*/ نياپو.charAt(اشاريو+1)==u_apos) {
                                // double apostrophe inside quoted literal text
                                // still encodes a single apostrophe, skip the second one
                                جوڙيو_Part(UMSGPAT_PART_TYPE_SKIP_SYNTAX, ++اشاريو, 1, 0, error_ڪوڊ);
                            } وإلا {
                                // skip the quote-ending apostrophe
                                جوڙيو_Part(UMSGPAT_PART_TYPE_SKIP_SYNTAX, اشاريو++, 1, 0, error_ڪوڊ);
                                اكسر;
                            }
                        } وإلا {
                            // The quoted text reaches to the end of the of the message.
                            اشاريو=نياپو.length();
                            // Add a Part for auto-quoting.
                            جوڙيو_Part(UMSGPAT_PART_TYPE_وجهو_CHAR, اشاريو, 0,
                                    u_apos, error_ڪوڊ);  // value=char to be inserted
                            needsAutoQuoting=سچ;
                            اكسر;
                        }
                    }
                } وإلا {
                    // Interpret the apostrophe as literal text.
                    // Add a Part for auto-quoting.
                    جوڙيو_Part(UMSGPAT_PART_TYPE_وجهو_CHAR, اشاريو, 0,
                            u_apos, error_ڪوڊ);  // value=char to be inserted
                    needsAutoQuoting=سچ;
                }
            }
        } وإلا إذا(UMSGPAT_ARG_TYPE_HAS_PLURAL_STYLE(والد_Type) && c==u_pound) {
            // The unquoted # in a plural message fragment will be replaced
            // with the (number-offset).
            جوڙيو_Part(UMSGPAT_PART_TYPE_REPLACE_عدد, اشاريو-1, 1, 0, error_ڪوڊ);
        } وإلا إذا(c==u_کاٻو_Curly_Brace) {
            اشاريو=parseArg(اشاريو-1, 1, nestingLevel, parseError, error_ڪوڊ);
        } وإلا إذا((nestingLevel>0 && c==u_ساڄو_Curly_Brace) ||
                  (والد_Type==UMSGPAT_ARG_TYPE_CHOICE && c==u_pipe)) {
            // Finish the message before the terminator.
            // In a choice style, report the "}" substring only for the following ARG_LIMIT,
            // not for this MSG_LIMIT.
            int32_t حد_بندي_ڊيگهائي=(والد_Type==UMSGPAT_ARG_TYPE_CHOICE && c==u_ساڄو_Curly_Brace) ? 0 : 1;
            جوڙيو_حد_بندي_Part(نياپو_شروعات, UMSGPAT_PART_TYPE_نياپو_حد_بندي, اشاريو-1, حد_بندي_ڊيگهائي,
                         nestingLevel, error_ڪوڊ);
            إذا(والد_Type==UMSGPAT_ARG_TYPE_CHOICE) {
                // Let the choice style parser see the '}' or '|'.
                أرجع اشاريو-1;
            } وإلا {
                // continue parsing after the '}'
                أرجع اشاريو;
            }
        }  // else: c is part of literal text
    }
    إذا(nestingLevel>0 && !اندريون_چوٽي_Level_Choice_پيغام(nestingLevel, والد_Type)) {
        سيٽ_Parse_Error(parseError, 0);  // Unmatched '{' braces in message.
        error_ڪوڊ=U_UNMATCHED_BRACES;
        أرجع 0;
    }
    جوڙيو_حد_بندي_Part(نياپو_شروعات, UMSGPAT_PART_TYPE_نياپو_حد_بندي, اشاريو, 0, nestingLevel, error_ڪوڊ);
    أرجع اشاريو;
}

int32_t
پيغام_نمونو::parseArg(int32_t اشاريو, int32_t arg_شروعات_ڊيگهائي, int32_t nestingLevel,
                         UParseError *parseError, UError_ڪوڊ &error_ڪوڊ) {
    int32_t arg_شروعات=parts_ڊيگهائي;
    UMessage_نمونو_Arg_Type argType=UMSGPAT_ARG_TYPE_NONE;
    جوڙيو_Part(UMSGPAT_PART_TYPE_ARG_شروعات, اشاريو, arg_شروعات_ڊيگهائي, argType, error_ڪوڊ);
    إذا(U_FAILURE(error_ڪوڊ)) {
        أرجع 0;
    }
    int32_t name_اشاريو=اشاريو=skipWhiteSpace(اشاريو+arg_شروعات_ڊيگهائي);
    إذا(اشاريو==نياپو.length()) {
        سيٽ_Parse_Error(parseError, 0);  // Unmatched '{' braces in message.
        error_ڪوڊ=U_UNMATCHED_BRACES;
        أرجع 0;
    }
    // parse argument name or number
    اشاريو=skipIdentifier(اشاريو);
    int32_t number=parse_Arg_عدد(name_اشاريو, اشاريو);
    إذا(number>=0) {
        int32_t length=اشاريو-name_اشاريو;
        إذا(length>Part::MAX_ڊيگهائي || number>Part::MAX_قدر) {
            سيٽ_Parse_Error(parseError, name_اشاريو);  // Argument number too large.
            error_ڪوڊ=U_اشاريو_OUTOFBOUNDS_ERROR;
            أرجع 0;
        }
        hasArgNumbers=سچ;
        جوڙيو_Part(UMSGPAT_PART_TYPE_ARG_عدد, name_اشاريو, length, number, error_ڪوڊ);
    } وإلا إذا(number==UMSGPAT_ARG_NAME_NOT_عدد) {
        int32_t length=اشاريو-name_اشاريو;
        إذا(length>Part::MAX_ڊيگهائي) {
            سيٽ_Parse_Error(parseError, name_اشاريو);  // Argument name too long.
            error_ڪوڊ=U_اشاريو_OUTOFBOUNDS_ERROR;
            أرجع 0;
        }
        has_Arg_نالا=سچ;
        جوڙيو_Part(UMSGPAT_PART_TYPE_ARG_NAME, name_اشاريو, length, 0, error_ڪوڊ);
    } وإلا {  // number<-1 (ARG_NAME_NOT_VALID)
        سيٽ_Parse_Error(parseError, name_اشاريو);  // Bad argument syntax.
        error_ڪوڊ=U_نمونو_SYNTAX_ERROR;
        أرجع 0;
    }
    اشاريو=skipWhiteSpace(اشاريو);
    إذا(اشاريو==نياپو.length()) {
        سيٽ_Parse_Error(parseError, 0);  // Unmatched '{' braces in message.
        error_ڪوڊ=U_UNMATCHED_BRACES;
        أرجع 0;
    }
    char16_t c=نياپو.charAt(اشاريو);
    إذا(c==u_ساڄو_Curly_Brace) {
        // all done
    } وإلا إذا(c!=u_comma) {
        سيٽ_Parse_Error(parseError, name_اشاريو);  // Bad argument syntax.
        error_ڪوڊ=U_نمونو_SYNTAX_ERROR;
        أرجع 0;
    } وإلا /* ',' */ {
        // parse argument type: case-sensitive a-zA-Z
        int32_t type_اشاريو=اشاريو=skipWhiteSpace(اشاريو+1);
        طالما(اشاريو<نياپو.length() && isArgTypeChar(نياپو.charAt(اشاريو))) {
            ++اشاريو;
        }
        int32_t length=اشاريو-type_اشاريو;
        اشاريو=skipWhiteSpace(اشاريو);
        إذا(اشاريو==نياپو.length()) {
            سيٽ_Parse_Error(parseError, 0);  // Unmatched '{' braces in message.
            error_ڪوڊ=U_UNMATCHED_BRACES;
            أرجع 0;
        }
        إذا(length==0 || ((c=نياپو.charAt(اشاريو))!=u_comma && c!=u_ساڄو_Curly_Brace)) {
            سيٽ_Parse_Error(parseError, name_اشاريو);  // Bad argument syntax.
            error_ڪوڊ=U_نمونو_SYNTAX_ERROR;
            أرجع 0;
        }
        إذا(length>Part::MAX_ڊيگهائي) {
            سيٽ_Parse_Error(parseError, name_اشاريو);  // Argument type name too long.
            error_ڪوڊ=U_اشاريو_OUTOFBOUNDS_ERROR;
            أرجع 0;
        }
        argType=UMSGPAT_ARG_TYPE_SIMPLE;
        إذا(length==6) {
            // case-insensitive comparisons for complex-type names
            إذا(isChoice(type_اشاريو)) {
                argType=UMSGPAT_ARG_TYPE_CHOICE;
            } وإلا إذا(isPlural(type_اشاريو)) {
                argType=UMSGPAT_ARG_TYPE_PLURAL;
            } وإلا إذا(isSelect(type_اشاريو)) {
                argType=UMSGPAT_ARG_TYPE_SELECT;
            }
        } وإلا إذا(length==13) {
            إذا(isSelect(type_اشاريو) && isOrdinal(type_اشاريو+6)) {
                argType=UMSGPAT_ARG_TYPE_SELECTORDINAL;
            }
        }
        // change the ARG_START type from NONE to argType
        partsList->a[arg_شروعات].value = static_cast<int16_t>(argType);
        إذا(argType==UMSGPAT_ARG_TYPE_SIMPLE) {
            جوڙيو_Part(UMSGPAT_PART_TYPE_ARG_TYPE, type_اشاريو, length, 0, error_ڪوڊ);
        }
        // look for an argument style (pattern)
        إذا(c==u_ساڄو_Curly_Brace) {
            إذا(argType!=UMSGPAT_ARG_TYPE_SIMPLE) {
                سيٽ_Parse_Error(parseError, name_اشاريو);  // No style field for complex argument.
                error_ڪوڊ=U_نمونو_SYNTAX_ERROR;
                أرجع 0;
            }
        } وإلا /* ',' */ {
            ++اشاريو;
            إذا(argType==UMSGPAT_ARG_TYPE_SIMPLE) {
                اشاريو=parseSimpleStyle(اشاريو, parseError, error_ڪوڊ);
            } وإلا إذا(argType==UMSGPAT_ARG_TYPE_CHOICE) {
                اشاريو=parseChoiceStyle(اشاريو, nestingLevel, parseError, error_ڪوڊ);
            } وإلا {
                اشاريو=parsePluralOrSelectStyle(argType, اشاريو, nestingLevel, parseError, error_ڪوڊ);
            }
        }
    }
    // Argument parsing stopped on the '}'.
    جوڙيو_حد_بندي_Part(arg_شروعات, UMSGPAT_PART_TYPE_ARG_حد_بندي, اشاريو, 1, argType, error_ڪوڊ);
    أرجع اشاريو+1;
}

int32_t
پيغام_نمونو::parseSimpleStyle(int32_t اشاريو, UParseError *parseError, UError_ڪوڊ &error_ڪوڊ) {
    إذا(U_FAILURE(error_ڪوڊ)) {
        أرجع 0;
    }
    int32_t شروعات=اشاريو;
    int32_t nestedBraces=0;
    طالما(اشاريو<نياپو.length()) {
        char16_t c=نياپو.charAt(اشاريو++);
        إذا(c==u_apos) {
            // Treat apostrophe as quoting but include it in the style part.
            // Find the end of the quoted literal text.
            اشاريو=نياپو.اشاريو_جو(u_apos, اشاريو);
            إذا(اشاريو<0) {
                // Quoted literal argument style text reaches to the end of the message.
                سيٽ_Parse_Error(parseError, شروعات);
                error_ڪوڊ=U_نمونو_SYNTAX_ERROR;
                أرجع 0;
            }
            // skip the quote-ending apostrophe
            ++اشاريو;
        } وإلا إذا(c==u_کاٻو_Curly_Brace) {
            ++nestedBraces;
        } وإلا إذا(c==u_ساڄو_Curly_Brace) {
            إذا(nestedBraces>0) {
                --nestedBraces;
            } وإلا {
                int32_t length=--اشاريو-شروعات;
                إذا(length>Part::MAX_ڊيگهائي) {
                    سيٽ_Parse_Error(parseError, شروعات);  // Argument style text too long.
                    error_ڪوڊ=U_اشاريو_OUTOFBOUNDS_ERROR;
                    أرجع 0;
                }
                جوڙيو_Part(UMSGPAT_PART_TYPE_ARG_STYLE, شروعات, length, 0, error_ڪوڊ);
                أرجع اشاريو;
            }
        }  // c is part of literal text
    }
    سيٽ_Parse_Error(parseError, 0);  // Unmatched '{' braces in message.
    error_ڪوڊ=U_UNMATCHED_BRACES;
    أرجع 0;
}

int32_t
پيغام_نمونو::parseChoiceStyle(int32_t اشاريو, int32_t nestingLevel,
                                 UParseError *parseError, UError_ڪوڊ &error_ڪوڊ) {
    إذا(U_FAILURE(error_ڪوڊ)) {
        أرجع 0;
    }
    int32_t شروعات=اشاريو;
    اشاريو=skipWhiteSpace(اشاريو);
    إذا(اشاريو==نياپو.length() || نياپو.charAt(اشاريو)==u_ساڄو_Curly_Brace) {
        سيٽ_Parse_Error(parseError, 0);  // Missing choice argument pattern.
        error_ڪوڊ=U_نمونو_SYNTAX_ERROR;
        أرجع 0;
    }
    لكل(;;) {
        // The choice argument style contains |-separated (number, separator, message) triples.
        // Parse the number.
        int32_t عدد_اشاريو=اشاريو;
        اشاريو=skipDouble(اشاريو);
        int32_t length=اشاريو-عدد_اشاريو;
        إذا(length==0) {
            سيٽ_Parse_Error(parseError, شروعات);  // Bad choice pattern syntax.
            error_ڪوڊ=U_نمونو_SYNTAX_ERROR;
            أرجع 0;
        }
        إذا(length>Part::MAX_ڊيگهائي) {
            سيٽ_Parse_Error(parseError, عدد_اشاريو);  // Choice number too long.
            error_ڪوڊ=U_اشاريو_OUTOFBOUNDS_ERROR;
            أرجع 0;
        }
        parseDouble(عدد_اشاريو, اشاريو, سچ, parseError, error_ڪوڊ);  // adds ARG_INT or ARG_DOUBLE
        إذا(U_FAILURE(error_ڪوڊ)) {
            أرجع 0;
        }
        // Parse the separator.
        اشاريو=skipWhiteSpace(اشاريو);
        إذا(اشاريو==نياپو.length()) {
            سيٽ_Parse_Error(parseError, شروعات);  // Bad choice pattern syntax.
            error_ڪوڊ=U_نمونو_SYNTAX_ERROR;
            أرجع 0;
        }
        char16_t c=نياپو.charAt(اشاريو);
        إذا(!(c==u_pound || c==u_lessThan || c==u_lessOrEqual)) {  // U+2264 is <=
            سيٽ_Parse_Error(parseError, شروعات);  // Expected choice separator (#<\u2264) instead of c.
            error_ڪوڊ=U_نمونو_SYNTAX_ERROR;
            أرجع 0;
        }
        جوڙيو_Part(UMSGPAT_PART_TYPE_ARG_SELECTOR, اشاريو, 1, 0, error_ڪوڊ);
        // Parse the message fragment.
        اشاريو=parse_پيغام(++اشاريو, 0, nestingLevel+1, UMSGPAT_ARG_TYPE_CHOICE, parseError, error_ڪوڊ);
        إذا(U_FAILURE(error_ڪوڊ)) {
            أرجع 0;
        }
        // parseMessage(..., CHOICE) returns the index of the terminator, or msg.length().
        إذا(اشاريو==نياپو.length()) {
            أرجع اشاريو;
        }
        إذا(نياپو.charAt(اشاريو)==u_ساڄو_Curly_Brace) {
            إذا(!اندريون_پيغام_Format_نمونو(nestingLevel)) {
                سيٽ_Parse_Error(parseError, شروعات);  // Bad choice pattern syntax.
                error_ڪوڊ=U_نمونو_SYNTAX_ERROR;
                أرجع 0;
            }
            أرجع اشاريو;
        }  // else the terminator is '|'
        اشاريو=skipWhiteSpace(اشاريو+1);
    }
}

int32_t
پيغام_نمونو::parsePluralOrSelectStyle(UMessage_نمونو_Arg_Type argType,
                                         int32_t اشاريو, int32_t nestingLevel,
                                         UParseError *parseError, UError_ڪوڊ &error_ڪوڊ) {
    إذا(U_FAILURE(error_ڪوڊ)) {
        أرجع 0;
    }
    int32_t شروعات=اشاريو;
    UBool is_خالي_جايون=سچ;
    UBool hasOther=خطأ;
    لكل(;;) {
        // First, collect the selector looking for a small set of terminators.
        // It would be a little faster to consider the syntax of each possible
        // token right here, but that makes the code too complicated.
        اشاريو=skipWhiteSpace(اشاريو);
        UBool eos=اشاريو==نياپو.length();
        إذا(eos || نياپو.charAt(اشاريو)==u_ساڄو_Curly_Brace) {
            إذا(eos==اندريون_پيغام_Format_نمونو(nestingLevel)) {
                سيٽ_Parse_Error(parseError, شروعات);  // Bad plural/select pattern syntax.
                error_ڪوڊ=U_نمونو_SYNTAX_ERROR;
                أرجع 0;
            }
            إذا(!hasOther) {
                سيٽ_Parse_Error(parseError, 0);  // Missing 'other' keyword in plural/select pattern.
                error_ڪوڊ=U_DEFAULT_KEYWORD_MISSING;
                أرجع 0;
            }
            أرجع اشاريو;
        }
        int32_t selector_اشاريو=اشاريو;
        إذا(UMSGPAT_ARG_TYPE_HAS_PLURAL_STYLE(argType) && نياپو.charAt(selector_اشاريو)==u_equal) {
            // explicit-value plural selector: =double
            اشاريو=skipDouble(اشاريو+1);
            int32_t length=اشاريو-selector_اشاريو;
            إذا(length==1) {
                سيٽ_Parse_Error(parseError, شروعات);  // Bad plural/select pattern syntax.
                error_ڪوڊ=U_نمونو_SYNTAX_ERROR;
                أرجع 0;
            }
            إذا(length>Part::MAX_ڊيگهائي) {
                سيٽ_Parse_Error(parseError, selector_اشاريو);  // Argument selector too long.
                error_ڪوڊ=U_اشاريو_OUTOFBOUNDS_ERROR;
                أرجع 0;
            }
            جوڙيو_Part(UMSGPAT_PART_TYPE_ARG_SELECTOR, selector_اشاريو, length, 0, error_ڪوڊ);
            parseDouble(selector_اشاريو+1, اشاريو, خطأ,
                        parseError, error_ڪوڊ);  // adds ARG_INT or ARG_DOUBLE
        } وإلا {
            اشاريو=skipIdentifier(اشاريو);
            int32_t length=اشاريو-selector_اشاريو;
            إذا(length==0) {
                سيٽ_Parse_Error(parseError, شروعات);  // Bad plural/select pattern syntax.
                error_ڪوڊ=U_نمونو_SYNTAX_ERROR;
                أرجع 0;
            }
            // Note: The ':' in "offset:" is just beyond the skipIdentifier() range.
            إذا( UMSGPAT_ARG_TYPE_HAS_PLURAL_STYLE(argType) && length==6 && اشاريو<نياپو.length() &&
                0==نياپو.compare(selector_اشاريو, 7, k_وٿي_Colon, 0, 7)
            ) {
                // plural offset, not a selector
                إذا(!is_خالي_جايون) {
                    // Plural argument 'offset:' (if present) must precede key-message pairs.
                    سيٽ_Parse_Error(parseError, شروعات);
                    error_ڪوڊ=U_نمونو_SYNTAX_ERROR;
                    أرجع 0;
                }
                // allow whitespace between offset: and its value
                int32_t قدر_اشاريو=skipWhiteSpace(اشاريو+1);  // The ':' is at index.
                اشاريو=skipDouble(قدر_اشاريو);
                إذا(اشاريو==قدر_اشاريو) {
                    سيٽ_Parse_Error(parseError, شروعات);  // Missing value for plural 'offset:'.
                    error_ڪوڊ=U_نمونو_SYNTAX_ERROR;
                    أرجع 0;
                }
                إذا((اشاريو-قدر_اشاريو)>Part::MAX_ڊيگهائي) {
                    سيٽ_Parse_Error(parseError, قدر_اشاريو);  // Plural offset value too long.
                    error_ڪوڊ=U_اشاريو_OUTOFBOUNDS_ERROR;
                    أرجع 0;
                }
                parseDouble(قدر_اشاريو, اشاريو, خطأ,
                            parseError, error_ڪوڊ);  // adds ARG_INT or ARG_DOUBLE
                إذا(U_FAILURE(error_ڪوڊ)) {
                    أرجع 0;
                }
                is_خالي_جايون=خطأ;
                استمر;  // no message fragment after the offset
            } وإلا {
                // normal selector word
                إذا(length>Part::MAX_ڊيگهائي) {
                    سيٽ_Parse_Error(parseError, selector_اشاريو);  // Argument selector too long.
                    error_ڪوڊ=U_اشاريو_OUTOFBOUNDS_ERROR;
                    أرجع 0;
                }
                جوڙيو_Part(UMSGPAT_PART_TYPE_ARG_SELECTOR, selector_اشاريو, length, 0, error_ڪوڊ);
                إذا(0==نياپو.compare(selector_اشاريو, length, kOther, 0, 5)) {
                    hasOther=سچ;
                }
            }
        }
        إذا(U_FAILURE(error_ڪوڊ)) {
            أرجع 0;
        }

        // parse the message fragment following the selector
        اشاريو=skipWhiteSpace(اشاريو);
        إذا(اشاريو==نياپو.length() || نياپو.charAt(اشاريو)!=u_کاٻو_Curly_Brace) {
            سيٽ_Parse_Error(parseError, selector_اشاريو);  // No message fragment after plural/select selector.
            error_ڪوڊ=U_نمونو_SYNTAX_ERROR;
            أرجع 0;
        }
        اشاريو=parse_پيغام(اشاريو, 1, nestingLevel+1, argType, parseError, error_ڪوڊ);
        إذا(U_FAILURE(error_ڪوڊ)) {
            أرجع 0;
        }
        is_خالي_جايون=خطأ;
    }
}

int32_t
پيغام_نمونو::parse_Arg_عدد(ثابت UnicodeString &s, int32_t شروعات, int32_t حد_بندي) {
    // If the identifier contains only ASCII digits, then it is an argument _number_
    // and must not have leading zeros (except "0" itself).
    // Otherwise it is an argument _name_.
    إذا(شروعات>=حد_بندي) {
        أرجع UMSGPAT_ARG_NAME_NOT_VALID;
    }
    int32_t number;
    // Defer numeric errors until we know there are only digits.
    UBool bad_عدد;
    char16_t c=s.charAt(شروعات++);
    إذا(c==0x30) {
        إذا(شروعات==حد_بندي) {
            أرجع 0;
        } وإلا {
            number=0;
            bad_عدد=سچ;  // leading zero
        }
    } وإلا إذا(0x31<=c && c<=0x39) {
        number=c-0x30;
        bad_عدد=خطأ;
    } وإلا {
        أرجع UMSGPAT_ARG_NAME_NOT_عدد;
    }
    طالما(شروعات<حد_بندي) {
        c=s.charAt(شروعات++);
        إذا(0x30<=c && c<=0x39) {
            إذا(number>=INT32_MAX/10) {
                bad_عدد=سچ;  //حد_کان_مٿي
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
پيغام_نمونو::parseDouble(int32_t شروعات, int32_t حد_بندي, UBool allowInfinity,
                            UParseError *parseError, UError_ڪوڊ &error_ڪوڊ) {
    إذا(U_FAILURE(error_ڪوڊ)) {
        أرجع;
    }
    U_ASSERT(شروعات<حد_بندي);
    // fake loop for easy exit and single throw statement
    لكل(;;) { /*loop doesn't iterate*/
        // fast path for small integers and infinity
        int32_t value=0;
        int32_t isNegative=0;  // not boolean so that we can easily add it to value
        int32_t اشاريو=شروعات;
        char16_t c=نياپو.charAt(اشاريو++);
        إذا(c==u_minus) {
            isNegative=1;
            إذا(اشاريو==حد_بندي) {
                اكسر;  // no number
            }
            c=نياپو.charAt(اشاريو++);
        } وإلا إذا(c==u_plus) {
            إذا(اشاريو==حد_بندي) {
                اكسر;  // no number
            }
            c=نياپو.charAt(اشاريو++);
        }
        إذا(c==0x221e) {  // infinity
            إذا(allowInfinity && اشاريو==حد_بندي) {
                مزدوج infinity=uprv_getInfinity();
                جوڙيو_Arg_Double_Part(
                    isNegative!=0 ? -infinity : infinity,
                    شروعات, حد_بندي-شروعات, error_ڪوڊ);
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
            إذا(اشاريو==حد_بندي) {
                جوڙيو_Part(UMSGPAT_PART_TYPE_ARG_INT, شروعات, حد_بندي-شروعات,
                        isNegative!=0 ? -value : value, error_ڪوڊ);
                أرجع;
            }
            c=نياپو.charAt(اشاريو++);
        }
        // Let Double.parseDouble() throw a NumberFormatException.
        حرف عدد_Chars[128];
        int32_t capacity = static_cast<int32_t>(حجم(عدد_Chars));
        int32_t length=حد_بندي-شروعات;
        إذا(length>=capacity) {
            اكسر;  // number too long
        }
        نياپو.extract(شروعات, length, عدد_Chars, capacity, US_INV);
        إذا (static_cast<int32_t>(uprv_strlen(عدد_Chars)) < length) {
            اكسر;  // contains non-invariant character that was turned into NUL
        }
        حرف *پڇاڙي;
        مزدوج numeric_قدر=uprv_strtod(عدد_Chars, &پڇاڙي);
        إذا(پڇاڙي!=(عدد_Chars+length)) {
            اكسر;  // parsing error
        }
        جوڙيو_Arg_Double_Part(numeric_قدر, شروعات, length, error_ڪوڊ);
        أرجع;
    }
    سيٽ_Parse_Error(parseError, شروعات /*, limit*/);  // Bad syntax for numeric value.
    error_ڪوڊ=U_نمونو_SYNTAX_ERROR;
}

int32_t
پيغام_نمونو::skipWhiteSpace(int32_t اشاريو) {
    ثابت char16_t *s=نياپو.get_بفر();
    int32_t نياپو_ڊيگهائي=نياپو.length();
    ثابت char16_t *t=نمونو_Props::skipWhiteSpace(s+اشاريو, نياپو_ڊيگهائي-اشاريو);
    أرجع static_cast<int32_t>(t - s);
}

int32_t
پيغام_نمونو::skipIdentifier(int32_t اشاريو) {
    ثابت char16_t *s=نياپو.get_بفر();
    int32_t نياپو_ڊيگهائي=نياپو.length();
    ثابت char16_t *t=نمونو_Props::skipIdentifier(s+اشاريو, نياپو_ڊيگهائي-اشاريو);
    أرجع static_cast<int32_t>(t - s);
}

int32_t
پيغام_نمونو::skipDouble(int32_t اشاريو) {
    int32_t نياپو_ڊيگهائي=نياپو.length();
    طالما(اشاريو<نياپو_ڊيگهائي) {
        char16_t c=نياپو.charAt(اشاريو);
        // U+221E: Allow the infinity symbol, for ChoiceFormat patterns.
        إذا((c<0x30 && c!=u_plus && c!=u_minus && c!=u_dot) || (c>0x39 && c!=u_e && c!=u_E && c!=0x221e)) {
            اكسر;
        }
        ++اشاريو;
    }
    أرجع اشاريو;
}

UBool
پيغام_نمونو::isArgTypeChar(UChar32 c) {
    أرجع (u_a<=c && c<=u_z) || (u_A<=c && c<=u_Z);
}

UBool
پيغام_نمونو::isChoice(int32_t اشاريو) {
    char16_t c;
    أرجع
        ((c=نياپو.charAt(اشاريو++))==u_c || c==u_C) &&
        ((c=نياپو.charAt(اشاريو++))==u_h || c==u_H) &&
        ((c=نياپو.charAt(اشاريو++))==u_o || c==u_O) &&
        ((c=نياپو.charAt(اشاريو++))==u_i || c==u_I) &&
        ((c=نياپو.charAt(اشاريو++))==u_c || c==u_C) &&
        ((c=نياپو.charAt(اشاريو))==u_e || c==u_E);
}

UBool
پيغام_نمونو::isPlural(int32_t اشاريو) {
    char16_t c;
    أرجع
        ((c=نياپو.charAt(اشاريو++))==u_p || c==u_P) &&
        ((c=نياپو.charAt(اشاريو++))==u_l || c==u_L) &&
        ((c=نياپو.charAt(اشاريو++))==u_u || c==u_U) &&
        ((c=نياپو.charAt(اشاريو++))==u_r || c==u_R) &&
        ((c=نياپو.charAt(اشاريو++))==u_a || c==u_A) &&
        ((c=نياپو.charAt(اشاريو))==u_l || c==u_L);
}

UBool
پيغام_نمونو::isSelect(int32_t اشاريو) {
    char16_t c;
    أرجع
        ((c=نياپو.charAt(اشاريو++))==u_s || c==u_S) &&
        ((c=نياپو.charAt(اشاريو++))==u_e || c==u_E) &&
        ((c=نياپو.charAt(اشاريو++))==u_l || c==u_L) &&
        ((c=نياپو.charAt(اشاريو++))==u_e || c==u_E) &&
        ((c=نياپو.charAt(اشاريو++))==u_c || c==u_C) &&
        ((c=نياپو.charAt(اشاريو))==u_t || c==u_T);
}

UBool
پيغام_نمونو::isOrdinal(int32_t اشاريو) {
    char16_t c;
    أرجع
        ((c=نياپو.charAt(اشاريو++))==u_o || c==u_O) &&
        ((c=نياپو.charAt(اشاريو++))==u_r || c==u_R) &&
        ((c=نياپو.charAt(اشاريو++))==u_d || c==u_D) &&
        ((c=نياپو.charAt(اشاريو++))==u_i || c==u_I) &&
        ((c=نياپو.charAt(اشاريو++))==u_n || c==u_N) &&
        ((c=نياپو.charAt(اشاريو++))==u_a || c==u_A) &&
        ((c=نياپو.charAt(اشاريو))==u_l || c==u_L);
}

UBool
پيغام_نمونو::اندريون_پيغام_Format_نمونو(int32_t nestingLevel) {
    أرجع nestingLevel>0 || partsList->a[0].type==UMSGPAT_PART_TYPE_نياپو_شروعات;
}

UBool
پيغام_نمونو::اندريون_چوٽي_Level_Choice_پيغام(int32_t nestingLevel, UMessage_نمونو_Arg_Type والد_Type) {
    أرجع
        nestingLevel==1 &&
        والد_Type==UMSGPAT_ARG_TYPE_CHOICE &&
        partsList->a[0].type!=UMSGPAT_PART_TYPE_نياپو_شروعات;
}

فراغ
پيغام_نمونو::جوڙيو_Part(UMessage_نمونو_Part_Type type, int32_t اشاريو, int32_t length,
                        int32_t value, UError_ڪوڊ &error_ڪوڊ) {
    إذا(partsList->ensure_صلاحيت_For_One_وڌيڪ(parts_ڊيگهائي, error_ڪوڊ)) {
        Part &part=partsList->a[parts_ڊيگهائي++];
        part.type=type;
        part.اشاريو=اشاريو;
        part.length = static_cast<uint16_t>(length);
        part.value = static_cast<int16_t>(value);
        part.حد_بندي_Part_اشاريو=0;
    }
}

فراغ
پيغام_نمونو::جوڙيو_حد_بندي_Part(int32_t شروعات,
                             UMessage_نمونو_Part_Type type, int32_t اشاريو, int32_t length,
                             int32_t value, UError_ڪوڊ &error_ڪوڊ) {
    partsList->a[شروعات].حد_بندي_Part_اشاريو=parts_ڊيگهائي;
    جوڙيو_Part(type, اشاريو, length, value, error_ڪوڊ);
}

فراغ
پيغام_نمونو::جوڙيو_Arg_Double_Part(مزدوج numeric_قدر, int32_t شروعات, int32_t length,
                                 UError_ڪوڊ &error_ڪوڊ) {
    إذا(U_FAILURE(error_ڪوڊ)) {
        أرجع;
    }
    int32_t numeric_اشاريو=numeric_قدرون_ڊيگهائي;
    إذا(numeric_قدرون_List==خالي_اشارو) {
        numeric_قدرون_List=جديد پيغام_نمونو_Double_List();
        إذا(numeric_قدرون_List==خالي_اشارو) {
            error_ڪوڊ=U_يادگيري_ورهاست_ERROR;
            أرجع;
        }
    } وإلا إذا(!numeric_قدرون_List->ensure_صلاحيت_For_One_وڌيڪ(numeric_قدرون_ڊيگهائي, error_ڪوڊ)) {
        أرجع;
    } وإلا {
        إذا(numeric_اشاريو>Part::MAX_قدر) {
            error_ڪوڊ=U_اشاريو_OUTOFBOUNDS_ERROR;
            أرجع;
        }
    }
    numeric_قدرون_List->a[numeric_قدرون_ڊيگهائي++]=numeric_قدر;
    جوڙيو_Part(UMSGPAT_PART_TYPE_ARG_DOUBLE, شروعات, length, numeric_اشاريو, error_ڪوڊ);
}

فراغ
پيغام_نمونو::سيٽ_Parse_Error(UParseError *parseError, int32_t اشاريو) {
    إذا(parseError==خالي_اشارو) {
        أرجع;
    }
    parseError->وٿي=اشاريو;

    // Set preContext to some of msg before index.
    // Avoid splitting a surrogate pair.
    int32_t length=اشاريو;
    إذا(length>=U_PARSE_CONTEXT_LEN) {
        length=U_PARSE_CONTEXT_LEN-1;
        إذا(length>0 && U16_IS_TRAIL(نياپو[اشاريو-length])) {
            --length;
        }
    }
    نياپو.extract(اشاريو-length, length, parseError->preContext);
    parseError->preContext[length]=0;

    // Set postContext to some of msg starting at index.
    length=نياپو.length()-اشاريو;
    إذا(length>=U_PARSE_CONTEXT_LEN) {
        length=U_PARSE_CONTEXT_LEN-1;
        إذا(length>0 && U16_IS_LEAD(نياپو[اشاريو+length-1])) {
            --length;
        }
    }
    نياپو.extract(اشاريو, length, parseError->postContext);
    parseError->postContext[length]=0;
}

// MessageImpl ------------------------------------------------------------- ***

فراغ
پيغام_Impl::appendReducedApostrophes(ثابت UnicodeString &s, int32_t شروعات, int32_t حد_بندي,
                                      UnicodeString &sb) {
    int32_t doubleApos=-1;
    لكل(;;) {
        int32_t i=s.اشاريو_جو(u_apos, شروعات);
        إذا(i<0 || i>=حد_بندي) {
            sb.append(s, شروعات, حد_بندي-شروعات);
            اكسر;
        }
        إذا(i==doubleApos) {
            // Double apostrophe at start-1 and start==i, append one.
            sb.append(u_apos);
            ++شروعات;
            doubleApos=-1;
        } وإلا {
            // Append text between apostrophes and skip this one.
            sb.append(s, شروعات, i-شروعات);
            doubleApos=شروعات=i+1;
        }
    }
}

// Ported from second half of ICU4J SelectFormat.format(String).
UnicodeString&
پيغام_Impl::append_Sub_پيغام_Without_Skip_Syntax(ثابت پيغام_نمونو& نياپو_نمونو,
                                               int32_t نياپو_شروعات,
                                               UnicodeString& نتيجو U_LIFETIME_BOUND) {
    ثابت UnicodeString &نياپو_String=نياپو_نمونو.get_نمونو_String();
    int32_t prev_اشاريو=نياپو_نمونو.getPart(نياپو_شروعات).get_حد_بندي();
    لكل(int32_t i=نياپو_شروعات;;) {
        ثابت پيغام_نمونو::Part &part=نياپو_نمونو.getPart(++i);
        UMessage_نمونو_Part_Type type=part.getType();
        int32_t اشاريو=part.get_اشاريو();
        إذا(type==UMSGPAT_PART_TYPE_نياپو_حد_بندي) {
            أرجع نتيجو.append(نياپو_String, prev_اشاريو, اشاريو-prev_اشاريو);
        } وإلا إذا(type==UMSGPAT_PART_TYPE_SKIP_SYNTAX) {
            نتيجو.append(نياپو_String, prev_اشاريو, اشاريو-prev_اشاريو);
            prev_اشاريو=part.get_حد_بندي();
        } وإلا إذا(type==UMSGPAT_PART_TYPE_ARG_شروعات) {
            نتيجو.append(نياپو_String, prev_اشاريو, اشاريو-prev_اشاريو);
            prev_اشاريو=اشاريو;
            i=نياپو_نمونو.get_حد_بندي_Part_اشاريو(i);
            اشاريو=نياپو_نمونو.getPart(i).get_حد_بندي();
            appendReducedApostrophes(نياپو_String, prev_اشاريو, اشاريو, نتيجو);
            prev_اشاريو=اشاريو;
        }
    }
}

U_NAMESPACE_پڇاڙي

#جيڪڏهن_پورو  // !UCONFIG_NO_FORMATTING
