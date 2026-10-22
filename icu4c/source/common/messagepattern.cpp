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

#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/utypes.h"

#ਜੇ !UCONFIG_NO_FORMATTING

#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/messagepattern.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/unistr.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/utf16.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "cmemory.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "cstring.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "messageimpl.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "patternprops.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "putilimp.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "uassert.h"

U_NAMESPACE_BEGIN

// Unicode character/code point constants ---------------------------------- ***

ਸਥਿਰ ਅਚਲ char16_t u_pound=0x23;
ਸਥਿਰ ਅਚਲ char16_t u_apos=0x27;
ਸਥਿਰ ਅਚਲ char16_t u_plus=0x2B;
ਸਥਿਰ ਅਚਲ char16_t u_comma=0x2C;
ਸਥਿਰ ਅਚਲ char16_t u_minus=0x2D;
ਸਥਿਰ ਅਚਲ char16_t u_dot=0x2E;
ਸਥਿਰ ਅਚਲ char16_t u_colon=0x3A;
ਸਥਿਰ ਅਚਲ char16_t u_lessThan=0x3C;
ਸਥਿਰ ਅਚਲ char16_t u_equal=0x3D;
ਸਥਿਰ ਅਚਲ char16_t u_A=0x41;
ਸਥਿਰ ਅਚਲ char16_t u_C=0x43;
ਸਥਿਰ ਅਚਲ char16_t u_D=0x44;
ਸਥਿਰ ਅਚਲ char16_t u_E=0x45;
ਸਥਿਰ ਅਚਲ char16_t u_H=0x48;
ਸਥਿਰ ਅਚਲ char16_t u_I=0x49;
ਸਥਿਰ ਅਚਲ char16_t u_L=0x4C;
ਸਥਿਰ ਅਚਲ char16_t u_N=0x4E;
ਸਥਿਰ ਅਚਲ char16_t u_O=0x4F;
ਸਥਿਰ ਅਚਲ char16_t u_P=0x50;
ਸਥਿਰ ਅਚਲ char16_t u_R=0x52;
ਸਥਿਰ ਅਚਲ char16_t u_S=0x53;
ਸਥਿਰ ਅਚਲ char16_t u_T=0x54;
ਸਥਿਰ ਅਚਲ char16_t u_U=0x55;
ਸਥਿਰ ਅਚਲ char16_t u_Z=0x5A;
ਸਥਿਰ ਅਚਲ char16_t u_a=0x61;
ਸਥਿਰ ਅਚਲ char16_t u_c=0x63;
ਸਥਿਰ ਅਚਲ char16_t u_d=0x64;
ਸਥਿਰ ਅਚਲ char16_t u_e=0x65;
ਸਥਿਰ ਅਚਲ char16_t u_f=0x66;
ਸਥਿਰ ਅਚਲ char16_t u_h=0x68;
ਸਥਿਰ ਅਚਲ char16_t u_i=0x69;
ਸਥਿਰ ਅਚਲ char16_t u_l=0x6C;
ਸਥਿਰ ਅਚਲ char16_t u_n=0x6E;
ਸਥਿਰ ਅਚਲ char16_t u_o=0x6F;
ਸਥਿਰ ਅਚਲ char16_t u_p=0x70;
ਸਥਿਰ ਅਚਲ char16_t u_r=0x72;
ਸਥਿਰ ਅਚਲ char16_t u_s=0x73;
ਸਥਿਰ ਅਚਲ char16_t u_t=0x74;
ਸਥਿਰ ਅਚਲ char16_t u_u=0x75;
ਸਥਿਰ ਅਚਲ char16_t u_z=0x7A;
ਸਥਿਰ ਅਚਲ char16_t u_ਖੱਬਾ_Curly_Brace=0x7B;
ਸਥਿਰ ਅਚਲ char16_t u_pipe=0x7C;
ਸਥਿਰ ਅਚਲ char16_t u_ਸੱਜਾ_Curly_Brace=0x7D;
ਸਥਿਰ ਅਚਲ char16_t u_lessOrEqual=0x2264;  // U+2264 is <=

ਸਥਿਰ ਅਚਲ char16_t k_ਵਿੱਥ_Colon[]={  // "offset:"
    u_o, u_f, u_f, u_s, u_e, u_t, u_colon
};

ਸਥਿਰ ਅਚਲ char16_t kOther[]={  // "other"
    u_o, u_t, u_h, u_e, u_r
};

// MessagePatternList ------------------------------------------------------ ***

ਸਾਂਚਾ<ਕਿਸਮ_ਨਾਂ T, int32_t ਢੇਰ_ਸਮਰੱਥਾ>
ਵਰਗ ਸੁਨੇਹਾ_ਨਮੂਨਾ_List : ਜਨਤਕ UMemory {
ਜਨਤਕ:
    ਸੁਨੇਹਾ_ਨਮੂਨਾ_List() {}
    ਖਾਲੀ copyFrom(ਅਚਲ ਸੁਨੇਹਾ_ਨਮੂਨਾ_List<T, ਢੇਰ_ਸਮਰੱਥਾ> &other,
                  int32_t length,
                  UError_ਕੋਡ &error_ਕੋਡ);
    UBool ensure_ਸਮਰੱਥਾ_For_One_ਹੋਰ(int32_t old_ਲੰਮਾਈ, UError_ਕੋਡ &error_ਕੋਡ);
    UBool equals(ਅਚਲ ਸੁਨੇਹਾ_ਨਮੂਨਾ_List<T, ਢੇਰ_ਸਮਰੱਥਾ> &other, int32_t length) ਅਚਲ {
        ਲਈ(int32_t i=0; i<length; ++i) {
            ਜੇ(a[i]!=other.a[i]) { ਵਾਪਸ ਝੂਠ; }
        }
        ਵਾਪਸ ਸੱਚ;
    }

    Maybe_ਢੇਰ_Array<T, ਢੇਰ_ਸਮਰੱਥਾ> a;
};

ਸਾਂਚਾ<ਕਿਸਮ_ਨਾਂ T, int32_t ਢੇਰ_ਸਮਰੱਥਾ>
ਖਾਲੀ
ਸੁਨੇਹਾ_ਨਮੂਨਾ_List<T, ਢੇਰ_ਸਮਰੱਥਾ>::copyFrom(
        ਅਚਲ ਸੁਨੇਹਾ_ਨਮੂਨਾ_List<T, ਢੇਰ_ਸਮਰੱਥਾ> &other,
        int32_t length,
        UError_ਕੋਡ &error_ਕੋਡ) {
    ਜੇ(U_SUCCESS(error_ਕੋਡ) && length>0) {
        ਜੇ(length>a.get_ਸਮਰੱਥਾ() && ਖਾਲੀ_ਸੂਚਕ==a.resize(length)) {
            error_ਕੋਡ=U_ਸਮ੍ਰਿਤੀ_ਵੰਡ_ERROR;
            ਵਾਪਸ;
        }
        uprv_memcpy(a.getAlias(), other.a.getAlias(), (size_t)length*ਆਕਾਰ(T));
    }
}

ਸਾਂਚਾ<ਕਿਸਮ_ਨਾਂ T, int32_t ਢੇਰ_ਸਮਰੱਥਾ>
UBool
ਸੁਨੇਹਾ_ਨਮੂਨਾ_List<T, ਢੇਰ_ਸਮਰੱਥਾ>::ensure_ਸਮਰੱਥਾ_For_One_ਹੋਰ(int32_t old_ਲੰਮਾਈ, UError_ਕੋਡ &error_ਕੋਡ) {
    ਜੇ(U_FAILURE(error_ਕੋਡ)) {
        ਵਾਪਸ ਝੂਠ;
    }
    ਜੇ(a.get_ਸਮਰੱਥਾ()>old_ਲੰਮਾਈ || a.resize(2*old_ਲੰਮਾਈ, old_ਲੰਮਾਈ)!=ਖਾਲੀ_ਸੂਚਕ) {
        ਵਾਪਸ ਸੱਚ;
    }
    error_ਕੋਡ=U_ਸਮ੍ਰਿਤੀ_ਵੰਡ_ERROR;
    ਵਾਪਸ ਝੂਠ;
}

// MessagePatternList specializations -------------------------------------- ***

ਵਰਗ ਸੁਨੇਹਾ_ਨਮੂਨਾ_Double_List : ਜਨਤਕ ਸੁਨੇਹਾ_ਨਮੂਨਾ_List<ਦੁੱਗਣਾ, 8> {
};

ਵਰਗ ਸੁਨੇਹਾ_ਨਮੂਨਾ_Parts_List : ਜਨਤਕ ਸੁਨੇਹਾ_ਨਮੂਨਾ_List<ਸੁਨੇਹਾ_ਨਮੂਨਾ::Part, 32> {
};

// MessagePattern constructors etc. ---------------------------------------- ***

ਸੁਨੇਹਾ_ਨਮੂਨਾ::ਸੁਨੇਹਾ_ਨਮੂਨਾ(UError_ਕੋਡ &error_ਕੋਡ)
        : apos_ਬਹੁਲਕ(UCONFIG_MSGPAT_DEFAULT_APOSTROPHE_ਬਹੁਲਕ),
          partsList(ਖਾਲੀ_ਸੂਚਕ), parts(ਖਾਲੀ_ਸੂਚਕ), parts_ਲੰਮਾਈ(0),
          numeric_ਮੁੱਲਾਂ_List(ਖਾਲੀ_ਸੂਚਕ), numeric_ਮੁੱਲਾਂ(ਖਾਲੀ_ਸੂਚਕ), numeric_ਮੁੱਲਾਂ_ਲੰਮਾਈ(0),
          has_Arg_ਨਾਂ(ਝੂਠ), hasArgNumbers(ਝੂਠ), needsAutoQuoting(ਝੂਠ) {
    init(error_ਕੋਡ);
}

ਸੁਨੇਹਾ_ਨਮੂਨਾ::ਸੁਨੇਹਾ_ਨਮੂਨਾ(UMessage_ਨਮੂਨਾ_Apostrophe_ਬਹੁਲਕ ਬਹੁਲਕ, UError_ਕੋਡ &error_ਕੋਡ)
        : apos_ਬਹੁਲਕ(ਬਹੁਲਕ),
          partsList(ਖਾਲੀ_ਸੂਚਕ), parts(ਖਾਲੀ_ਸੂਚਕ), parts_ਲੰਮਾਈ(0),
          numeric_ਮੁੱਲਾਂ_List(ਖਾਲੀ_ਸੂਚਕ), numeric_ਮੁੱਲਾਂ(ਖਾਲੀ_ਸੂਚਕ), numeric_ਮੁੱਲਾਂ_ਲੰਮਾਈ(0),
          has_Arg_ਨਾਂ(ਝੂਠ), hasArgNumbers(ਝੂਠ), needsAutoQuoting(ਝੂਠ) {
    init(error_ਕੋਡ);
}

ਸੁਨੇਹਾ_ਨਮੂਨਾ::ਸੁਨੇਹਾ_ਨਮੂਨਾ(ਅਚਲ UnicodeString &ਨਮੂਨਾ, UParseError *parseError, UError_ਕੋਡ &error_ਕੋਡ)
        : apos_ਬਹੁਲਕ(UCONFIG_MSGPAT_DEFAULT_APOSTROPHE_ਬਹੁਲਕ),
          partsList(ਖਾਲੀ_ਸੂਚਕ), parts(ਖਾਲੀ_ਸੂਚਕ), parts_ਲੰਮਾਈ(0),
          numeric_ਮੁੱਲਾਂ_List(ਖਾਲੀ_ਸੂਚਕ), numeric_ਮੁੱਲਾਂ(ਖਾਲੀ_ਸੂਚਕ), numeric_ਮੁੱਲਾਂ_ਲੰਮਾਈ(0),
          has_Arg_ਨਾਂ(ਝੂਠ), hasArgNumbers(ਝੂਠ), needsAutoQuoting(ਝੂਠ) {
    ਜੇ(init(error_ਕੋਡ)) {
        parse(ਨਮੂਨਾ, parseError, error_ਕੋਡ);
    }
}

UBool
ਸੁਨੇਹਾ_ਨਮੂਨਾ::init(UError_ਕੋਡ &error_ਕੋਡ) {
    ਜੇ(U_FAILURE(error_ਕੋਡ)) {
        ਵਾਪਸ ਝੂਠ;
    }
    partsList=ਨਵਾਂ ਸੁਨੇਹਾ_ਨਮੂਨਾ_Parts_List();
    ਜੇ(partsList==ਖਾਲੀ_ਸੂਚਕ) {
        error_ਕੋਡ=U_ਸਮ੍ਰਿਤੀ_ਵੰਡ_ERROR;
        ਵਾਪਸ ਝੂਠ;
    }
    parts=partsList->a.getAlias();
    ਵਾਪਸ ਸੱਚ;
}

ਸੁਨੇਹਾ_ਨਮੂਨਾ::ਸੁਨੇਹਾ_ਨਮੂਨਾ(ਅਚਲ ਸੁਨੇਹਾ_ਨਮੂਨਾ &other)
        : UObject(other), apos_ਬਹੁਲਕ(other.apos_ਬਹੁਲਕ), ਸੁਨੇਹਾ(other.ਸੁਨੇਹਾ),
          partsList(ਖਾਲੀ_ਸੂਚਕ), parts(ਖਾਲੀ_ਸੂਚਕ), parts_ਲੰਮਾਈ(0),
          numeric_ਮੁੱਲਾਂ_List(ਖਾਲੀ_ਸੂਚਕ), numeric_ਮੁੱਲਾਂ(ਖਾਲੀ_ਸੂਚਕ), numeric_ਮੁੱਲਾਂ_ਲੰਮਾਈ(0),
          has_Arg_ਨਾਂ(other.has_Arg_ਨਾਂ), hasArgNumbers(other.hasArgNumbers),
          needsAutoQuoting(other.needsAutoQuoting) {
    UError_ਕੋਡ error_ਕੋਡ=U_ZERO_ERROR;
    ਜੇ(!copyStorage(other, error_ਕੋਡ)) {
        clear();
    }
}

ਸੁਨੇਹਾ_ਨਮੂਨਾ&
ਸੁਨੇਹਾ_ਨਮੂਨਾ::ਸੰਚਾਲਕ=(ਅਚਲ ਸੁਨੇਹਾ_ਨਮੂਨਾ& other) U_LIFETIME_BOUND {
    ਜੇ(ਇਹ==&other) {
        ਵਾਪਸ *ਇਹ;
    }
    apos_ਬਹੁਲਕ=other.apos_ਬਹੁਲਕ;
    ਸੁਨੇਹਾ=other.ਸੁਨੇਹਾ;
    has_Arg_ਨਾਂ=other.has_Arg_ਨਾਂ;
    hasArgNumbers=other.hasArgNumbers;
    needsAutoQuoting=other.needsAutoQuoting;
    UError_ਕੋਡ error_ਕੋਡ=U_ZERO_ERROR;
    ਜੇ(!copyStorage(other, error_ਕੋਡ)) {
        clear();
    }
    ਵਾਪਸ *ਇਹ;
}

UBool
ਸੁਨੇਹਾ_ਨਮੂਨਾ::copyStorage(ਅਚਲ ਸੁਨੇਹਾ_ਨਮੂਨਾ &other, UError_ਕੋਡ &error_ਕੋਡ) {
    ਜੇ(U_FAILURE(error_ਕੋਡ)) {
        ਵਾਪਸ ਝੂਠ;
    }
    parts=ਖਾਲੀ_ਸੂਚਕ;
    parts_ਲੰਮਾਈ=0;
    numeric_ਮੁੱਲਾਂ=ਖਾਲੀ_ਸੂਚਕ;
    numeric_ਮੁੱਲਾਂ_ਲੰਮਾਈ=0;
    ਜੇ(partsList==ਖਾਲੀ_ਸੂਚਕ) {
        partsList=ਨਵਾਂ ਸੁਨੇਹਾ_ਨਮੂਨਾ_Parts_List();
        ਜੇ(partsList==ਖਾਲੀ_ਸੂਚਕ) {
            error_ਕੋਡ=U_ਸਮ੍ਰਿਤੀ_ਵੰਡ_ERROR;
            ਵਾਪਸ ਝੂਠ;
        }
        parts=partsList->a.getAlias();
    }
    ਜੇ(other.parts_ਲੰਮਾਈ>0) {
        partsList->copyFrom(*other.partsList, other.parts_ਲੰਮਾਈ, error_ਕੋਡ);
        ਜੇ(U_FAILURE(error_ਕੋਡ)) {
            ਵਾਪਸ ਝੂਠ;
        }
        parts=partsList->a.getAlias();
        parts_ਲੰਮਾਈ=other.parts_ਲੰਮਾਈ;
    }
    ਜੇ(other.numeric_ਮੁੱਲਾਂ_ਲੰਮਾਈ>0) {
        ਜੇ(numeric_ਮੁੱਲਾਂ_List==ਖਾਲੀ_ਸੂਚਕ) {
            numeric_ਮੁੱਲਾਂ_List=ਨਵਾਂ ਸੁਨੇਹਾ_ਨਮੂਨਾ_Double_List();
            ਜੇ(numeric_ਮੁੱਲਾਂ_List==ਖਾਲੀ_ਸੂਚਕ) {
                error_ਕੋਡ=U_ਸਮ੍ਰਿਤੀ_ਵੰਡ_ERROR;
                ਵਾਪਸ ਝੂਠ;
            }
            numeric_ਮੁੱਲਾਂ=numeric_ਮੁੱਲਾਂ_List->a.getAlias();
        }
        numeric_ਮੁੱਲਾਂ_List->copyFrom(
            *other.numeric_ਮੁੱਲਾਂ_List, other.numeric_ਮੁੱਲਾਂ_ਲੰਮਾਈ, error_ਕੋਡ);
        ਜੇ(U_FAILURE(error_ਕੋਡ)) {
            ਵਾਪਸ ਝੂਠ;
        }
        numeric_ਮੁੱਲਾਂ=numeric_ਮੁੱਲਾਂ_List->a.getAlias();
        numeric_ਮੁੱਲਾਂ_ਲੰਮਾਈ=other.numeric_ਮੁੱਲਾਂ_ਲੰਮਾਈ;
    }
    ਵਾਪਸ ਸੱਚ;
}

ਸੁਨੇਹਾ_ਨਮੂਨਾ::~ਸੁਨੇਹਾ_ਨਮੂਨਾ() {
    ਮਿਟਾਓ partsList;
    ਮਿਟਾਓ numeric_ਮੁੱਲਾਂ_List;
}

// MessagePattern API ------------------------------------------------------ ***

ਸੁਨੇਹਾ_ਨਮੂਨਾ&
ਸੁਨੇਹਾ_ਨਮੂਨਾ::parse(ਅਚਲ UnicodeString& ਨਮੂਨਾ,
                      UParseError* parseError,
                      UError_ਕੋਡ& error_ਕੋਡ) U_LIFETIME_BOUND {
    preParse(ਨਮੂਨਾ, parseError, error_ਕੋਡ);
    parse_ਸੁਨੇਹਾ(0, 0, 0, UMSGPAT_ARG_TYPE_NONE, parseError, error_ਕੋਡ);
    postParse();
    ਵਾਪਸ *ਇਹ;
}

ਸੁਨੇਹਾ_ਨਮੂਨਾ&
ਸੁਨੇਹਾ_ਨਮੂਨਾ::parseChoiceStyle(ਅਚਲ UnicodeString& ਨਮੂਨਾ,
                                 UParseError* parseError,
                                 UError_ਕੋਡ& error_ਕੋਡ) U_LIFETIME_BOUND {
    preParse(ਨਮੂਨਾ, parseError, error_ਕੋਡ);
    parseChoiceStyle(0, 0, parseError, error_ਕੋਡ);
    postParse();
    ਵਾਪਸ *ਇਹ;
}

ਸੁਨੇਹਾ_ਨਮੂਨਾ&
ਸੁਨੇਹਾ_ਨਮੂਨਾ::parsePluralStyle(ਅਚਲ UnicodeString& ਨਮੂਨਾ,
                                 UParseError* parseError,
                                 UError_ਕੋਡ& error_ਕੋਡ) U_LIFETIME_BOUND {
    preParse(ਨਮੂਨਾ, parseError, error_ਕੋਡ);
    parsePluralOrSelectStyle(UMSGPAT_ARG_TYPE_PLURAL, 0, 0, parseError, error_ਕੋਡ);
    postParse();
    ਵਾਪਸ *ਇਹ;
}

ਸੁਨੇਹਾ_ਨਮੂਨਾ&
ਸੁਨੇਹਾ_ਨਮੂਨਾ::parseSelectStyle(ਅਚਲ UnicodeString& ਨਮੂਨਾ,
                                 UParseError* parseError,
                                 UError_ਕੋਡ& error_ਕੋਡ) U_LIFETIME_BOUND {
    preParse(ਨਮੂਨਾ, parseError, error_ਕੋਡ);
    parsePluralOrSelectStyle(UMSGPAT_ARG_TYPE_SELECT, 0, 0, parseError, error_ਕੋਡ);
    postParse();
    ਵਾਪਸ *ਇਹ;
}

ਖਾਲੀ
ਸੁਨੇਹਾ_ਨਮੂਨਾ::clear() {
    // Mostly the same as preParse().
    ਸੁਨੇਹਾ.remove();
    has_Arg_ਨਾਂ=hasArgNumbers=ਝੂਠ;
    needsAutoQuoting=ਝੂਠ;
    parts_ਲੰਮਾਈ=0;
    numeric_ਮੁੱਲਾਂ_ਲੰਮਾਈ=0;
}

ਤਾਰਕਿਕ
ਸੁਨੇਹਾ_ਨਮੂਨਾ::ਸੰਚਾਲਕ==(ਅਚਲ ਸੁਨੇਹਾ_ਨਮੂਨਾ &other) ਅਚਲ {
    ਜੇ(ਇਹ==&other) {
        ਵਾਪਸ ਸੱਚ;
    }
    ਵਾਪਸ
        apos_ਬਹੁਲਕ==other.apos_ਬਹੁਲਕ &&
        ਸੁਨੇਹਾ==other.ਸੁਨੇਹਾ &&
        // parts.equals(o.parts)
        parts_ਲੰਮਾਈ==other.parts_ਲੰਮਾਈ &&
        (parts_ਲੰਮਾਈ==0 || partsList->equals(*other.partsList, parts_ਲੰਮਾਈ));
    // No need to compare numericValues if msg and parts are the same.
}

int32_t
ਸੁਨੇਹਾ_ਨਮੂਨਾ::hash_ਕੋਡ() ਅਚਲ {
    int32_t hash=(apos_ਬਹੁਲਕ*37+ਸੁਨੇਹਾ.hash_ਕੋਡ())*37+parts_ਲੰਮਾਈ;
    ਲਈ(int32_t i=0; i<parts_ਲੰਮਾਈ; ++i) {
        hash=hash*37+parts[i].hash_ਕੋਡ();
    }
    ਵਾਪਸ hash;
}

int32_t
ਸੁਨੇਹਾ_ਨਮੂਨਾ::validate_ਦਲੀਲ_Name(ਅਚਲ UnicodeString &name) {
    ਜੇ(!ਨਮੂਨਾ_Props::isIdentifier(name.get_ਬਫਰ(), name.length())) {
        ਵਾਪਸ UMSGPAT_ARG_NAME_NOT_VALID;
    }
    ਵਾਪਸ parse_Arg_ਸੰਖਿਆ(name, 0, name.length());
}

UnicodeString
ਸੁਨੇਹਾ_ਨਮੂਨਾ::autoQuoteApostropheDeep() ਅਚਲ {
    ਜੇ(!needsAutoQuoting) {
        ਵਾਪਸ ਸੁਨੇਹਾ;
    }
    UnicodeString modified(ਸੁਨੇਹਾ);
    // Iterate backward so that the insertion indexes do not change.
    int32_t count=ਗਿਣਤੀ_Parts();
    ਲਈ(int32_t i=count; i>0;) {
        ਅਚਲ Part &part=getPart(--i);
        ਜੇ(part.getType()==UMSGPAT_PART_TYPE_ਪਾਓ_CHAR) {
           modified.insert(part.ਸੂਚਕਾਂਕ, static_cast<char16_t>(part.value));
        }
    }
    ਵਾਪਸ modified;
}

ਦੁੱਗਣਾ
ਸੁਨੇਹਾ_ਨਮੂਨਾ::get_Numeric_ਮੁੱਲ(ਅਚਲ Part &part) ਅਚਲ {
    UMessage_ਨਮੂਨਾ_Part_Type type=part.type;
    ਜੇ(type==UMSGPAT_PART_TYPE_ARG_INT) {
        ਵਾਪਸ part.value;
    } ਨਹੀਂਤਾਂ ਜੇ(type==UMSGPAT_PART_TYPE_ARG_DOUBLE) {
        ਵਾਪਸ numeric_ਮੁੱਲਾਂ[part.value];
    } ਨਹੀਂਤਾਂ {
        ਵਾਪਸ UMSGPAT_NO_NUMERIC_ਮੁੱਲ;
    }
}

/**
  * Returns the "offset:" value of a PluralFormat argument, or 0 if none is specified.
  * @param pluralStart the index of the first PluralFormat argument style part. (0..countParts()-1)
  * @return the "offset:" value.
  * @draft ICU 4.8
  */
ਦੁੱਗਣਾ
ਸੁਨੇਹਾ_ਨਮੂਨਾ::get_Plural_ਵਿੱਥ(int32_t plural_ਸ਼ੁਰੂ) ਅਚਲ {
    ਅਚਲ Part &part=getPart(plural_ਸ਼ੁਰੂ);
    ਜੇ(Part::has_Numeric_ਮੁੱਲ(part.type)) {
        ਵਾਪਸ get_Numeric_ਮੁੱਲ(part);
    } ਨਹੀਂਤਾਂ {
        ਵਾਪਸ 0;
    }
}

// MessagePattern::Part ---------------------------------------------------- ***

ਤਾਰਕਿਕ
ਸੁਨੇਹਾ_ਨਮੂਨਾ::Part::ਸੰਚਾਲਕ==(ਅਚਲ Part &other) ਅਚਲ {
    ਜੇ(ਇਹ==&other) {
        ਵਾਪਸ ਸੱਚ;
    }
    ਵਾਪਸ
        type==other.type &&
        ਸੂਚਕਾਂਕ==other.ਸੂਚਕਾਂਕ &&
        length==other.length &&
        value==other.value &&
        ਹੱਦਬੰਦੀ_Part_ਸੂਚਕਾਂਕ==other.ਹੱਦਬੰਦੀ_Part_ਸੂਚਕਾਂਕ;
}

// MessagePattern parser --------------------------------------------------- ***

ਖਾਲੀ
ਸੁਨੇਹਾ_ਨਮੂਨਾ::preParse(ਅਚਲ UnicodeString &ਨਮੂਨਾ, UParseError *parseError, UError_ਕੋਡ &error_ਕੋਡ) {
    ਜੇ(U_FAILURE(error_ਕੋਡ)) {
        ਵਾਪਸ;
    }
    ਜੇ(parseError!=ਖਾਲੀ_ਸੂਚਕ) {
        parseError->ਸਤਰ=0;
        parseError->ਵਿੱਥ=0;
        parseError->preContext[0]=0;
        parseError->postContext[0]=0;
    }
    ਸੁਨੇਹਾ=ਨਮੂਨਾ;
    has_Arg_ਨਾਂ=hasArgNumbers=ਝੂਠ;
    needsAutoQuoting=ਝੂਠ;
    parts_ਲੰਮਾਈ=0;
    numeric_ਮੁੱਲਾਂ_ਲੰਮਾਈ=0;
}

ਖਾਲੀ
ਸੁਨੇਹਾ_ਨਮੂਨਾ::postParse() {
    ਜੇ(partsList!=ਖਾਲੀ_ਸੂਚਕ) {
        parts=partsList->a.getAlias();
    }
    ਜੇ(numeric_ਮੁੱਲਾਂ_List!=ਖਾਲੀ_ਸੂਚਕ) {
        numeric_ਮੁੱਲਾਂ=numeric_ਮੁੱਲਾਂ_List->a.getAlias();
    }
}

int32_t
ਸੁਨੇਹਾ_ਨਮੂਨਾ::parse_ਸੁਨੇਹਾ(int32_t ਸੂਚਕਾਂਕ, int32_t ਸੁਨੇਹਾ_ਸ਼ੁਰੂ_ਲੰਮਾਈ,
                             int32_t nestingLevel, UMessage_ਨਮੂਨਾ_Arg_Type ਮਾਪਾ_Type,
                             UParseError *parseError, UError_ਕੋਡ &error_ਕੋਡ) {
    ਜੇ(U_FAILURE(error_ਕੋਡ)) {
        ਵਾਪਸ 0;
    }
    ਜੇ(nestingLevel>Part::MAX_NESTED_ਪੱਧਰ) {
        error_ਕੋਡ=U_ਸੂਚਕਾਂਕ_OUTOFBOUNDS_ERROR;
        ਵਾਪਸ 0;
    }
    int32_t ਸੁਨੇਹਾ_ਸ਼ੁਰੂ=parts_ਲੰਮਾਈ;
    ਜੋੜੋ_Part(UMSGPAT_PART_TYPE_ਸੁਨੇਹਾ_ਸ਼ੁਰੂ, ਸੂਚਕਾਂਕ, ਸੁਨੇਹਾ_ਸ਼ੁਰੂ_ਲੰਮਾਈ, nestingLevel, error_ਕੋਡ);
    ਸੂਚਕਾਂਕ+=ਸੁਨੇਹਾ_ਸ਼ੁਰੂ_ਲੰਮਾਈ;
    ਲਈ(;;) {  // while(index<msg.length()) with U_FAILURE(errorCode) check
        ਜੇ(U_FAILURE(error_ਕੋਡ)) {
            ਵਾਪਸ 0;
        }
        ਜੇ(ਸੂਚਕਾਂਕ>=ਸੁਨੇਹਾ.length()) {
            ਰੋਕੋ;
        }
        char16_t c=ਸੁਨੇਹਾ.charAt(ਸੂਚਕਾਂਕ++);
        ਜੇ(c==u_apos) {
            ਜੇ(ਸੂਚਕਾਂਕ==ਸੁਨੇਹਾ.length()) {
                // The apostrophe is the last character in the pattern. 
                // Add a Part for auto-quoting.
                ਜੋੜੋ_Part(UMSGPAT_PART_TYPE_ਪਾਓ_CHAR, ਸੂਚਕਾਂਕ, 0,
                        u_apos, error_ਕੋਡ);  // value=char to be inserted
                needsAutoQuoting=ਸੱਚ;
            } ਨਹੀਂਤਾਂ {
                c=ਸੁਨੇਹਾ.charAt(ਸੂਚਕਾਂਕ);
                ਜੇ(c==u_apos) {
                    // double apostrophe, skip the second one
                    ਜੋੜੋ_Part(UMSGPAT_PART_TYPE_SKIP_SYNTAX, ਸੂਚਕਾਂਕ++, 1, 0, error_ਕੋਡ);
                } ਨਹੀਂਤਾਂ ਜੇ(
                    apos_ਬਹੁਲਕ==UMSGPAT_APOS_DOUBLE_REQUIRED ||
                    c==u_ਖੱਬਾ_Curly_Brace || c==u_ਸੱਜਾ_Curly_Brace ||
                    (ਮਾਪਾ_Type==UMSGPAT_ARG_TYPE_CHOICE && c==u_pipe) ||
                    (UMSGPAT_ARG_TYPE_HAS_PLURAL_STYLE(ਮਾਪਾ_Type) && c==u_pound)
                ) {
                    // skip the quote-starting apostrophe
                    ਜੋੜੋ_Part(UMSGPAT_PART_TYPE_SKIP_SYNTAX, ਸੂਚਕਾਂਕ-1, 1, 0, error_ਕੋਡ);
                    // find the end of the quoted literal text
                    ਲਈ(;;) {
                        ਸੂਚਕਾਂਕ=ਸੁਨੇਹਾ.ਸੂਚਕਾਂਕ_ਦਾ(u_apos, ਸੂਚਕਾਂਕ+1);
                        ਜੇ(ਸੂਚਕਾਂਕ>=0) {
                            ਜੇ(/*(index+1)<msg.length() &&*/ ਸੁਨੇਹਾ.charAt(ਸੂਚਕਾਂਕ+1)==u_apos) {
                                // double apostrophe inside quoted literal text
                                // still encodes a single apostrophe, skip the second one
                                ਜੋੜੋ_Part(UMSGPAT_PART_TYPE_SKIP_SYNTAX, ++ਸੂਚਕਾਂਕ, 1, 0, error_ਕੋਡ);
                            } ਨਹੀਂਤਾਂ {
                                // skip the quote-ending apostrophe
                                ਜੋੜੋ_Part(UMSGPAT_PART_TYPE_SKIP_SYNTAX, ਸੂਚਕਾਂਕ++, 1, 0, error_ਕੋਡ);
                                ਰੋਕੋ;
                            }
                        } ਨਹੀਂਤਾਂ {
                            // The quoted text reaches to the end of the of the message.
                            ਸੂਚਕਾਂਕ=ਸੁਨੇਹਾ.length();
                            // Add a Part for auto-quoting.
                            ਜੋੜੋ_Part(UMSGPAT_PART_TYPE_ਪਾਓ_CHAR, ਸੂਚਕਾਂਕ, 0,
                                    u_apos, error_ਕੋਡ);  // value=char to be inserted
                            needsAutoQuoting=ਸੱਚ;
                            ਰੋਕੋ;
                        }
                    }
                } ਨਹੀਂਤਾਂ {
                    // Interpret the apostrophe as literal text.
                    // Add a Part for auto-quoting.
                    ਜੋੜੋ_Part(UMSGPAT_PART_TYPE_ਪਾਓ_CHAR, ਸੂਚਕਾਂਕ, 0,
                            u_apos, error_ਕੋਡ);  // value=char to be inserted
                    needsAutoQuoting=ਸੱਚ;
                }
            }
        } ਨਹੀਂਤਾਂ ਜੇ(UMSGPAT_ARG_TYPE_HAS_PLURAL_STYLE(ਮਾਪਾ_Type) && c==u_pound) {
            // The unquoted # in a plural message fragment will be replaced
            // with the (number-offset).
            ਜੋੜੋ_Part(UMSGPAT_PART_TYPE_REPLACE_ਸੰਖਿਆ, ਸੂਚਕਾਂਕ-1, 1, 0, error_ਕੋਡ);
        } ਨਹੀਂਤਾਂ ਜੇ(c==u_ਖੱਬਾ_Curly_Brace) {
            ਸੂਚਕਾਂਕ=parseArg(ਸੂਚਕਾਂਕ-1, 1, nestingLevel, parseError, error_ਕੋਡ);
        } ਨਹੀਂਤਾਂ ਜੇ((nestingLevel>0 && c==u_ਸੱਜਾ_Curly_Brace) ||
                  (ਮਾਪਾ_Type==UMSGPAT_ARG_TYPE_CHOICE && c==u_pipe)) {
            // Finish the message before the terminator.
            // In a choice style, report the "}" substring only for the following ARG_LIMIT,
            // not for this MSG_LIMIT.
            int32_t ਹੱਦਬੰਦੀ_ਲੰਮਾਈ=(ਮਾਪਾ_Type==UMSGPAT_ARG_TYPE_CHOICE && c==u_ਸੱਜਾ_Curly_Brace) ? 0 : 1;
            ਜੋੜੋ_ਹੱਦਬੰਦੀ_Part(ਸੁਨੇਹਾ_ਸ਼ੁਰੂ, UMSGPAT_PART_TYPE_ਸੁਨੇਹਾ_ਹੱਦਬੰਦੀ, ਸੂਚਕਾਂਕ-1, ਹੱਦਬੰਦੀ_ਲੰਮਾਈ,
                         nestingLevel, error_ਕੋਡ);
            ਜੇ(ਮਾਪਾ_Type==UMSGPAT_ARG_TYPE_CHOICE) {
                // Let the choice style parser see the '}' or '|'.
                ਵਾਪਸ ਸੂਚਕਾਂਕ-1;
            } ਨਹੀਂਤਾਂ {
                // continue parsing after the '}'
                ਵਾਪਸ ਸੂਚਕਾਂਕ;
            }
        }  // else: c is part of literal text
    }
    ਜੇ(nestingLevel>0 && !ਵਿੱਚ_ਸਿਖਰ_Level_Choice_ਸੁਨੇਹਾ(nestingLevel, ਮਾਪਾ_Type)) {
        ਸੈੱਟ_Parse_Error(parseError, 0);  // Unmatched '{' braces in message.
        error_ਕੋਡ=U_UNMATCHED_BRACES;
        ਵਾਪਸ 0;
    }
    ਜੋੜੋ_ਹੱਦਬੰਦੀ_Part(ਸੁਨੇਹਾ_ਸ਼ੁਰੂ, UMSGPAT_PART_TYPE_ਸੁਨੇਹਾ_ਹੱਦਬੰਦੀ, ਸੂਚਕਾਂਕ, 0, nestingLevel, error_ਕੋਡ);
    ਵਾਪਸ ਸੂਚਕਾਂਕ;
}

int32_t
ਸੁਨੇਹਾ_ਨਮੂਨਾ::parseArg(int32_t ਸੂਚਕਾਂਕ, int32_t arg_ਸ਼ੁਰੂ_ਲੰਮਾਈ, int32_t nestingLevel,
                         UParseError *parseError, UError_ਕੋਡ &error_ਕੋਡ) {
    int32_t arg_ਸ਼ੁਰੂ=parts_ਲੰਮਾਈ;
    UMessage_ਨਮੂਨਾ_Arg_Type argType=UMSGPAT_ARG_TYPE_NONE;
    ਜੋੜੋ_Part(UMSGPAT_PART_TYPE_ARG_ਸ਼ੁਰੂ, ਸੂਚਕਾਂਕ, arg_ਸ਼ੁਰੂ_ਲੰਮਾਈ, argType, error_ਕੋਡ);
    ਜੇ(U_FAILURE(error_ਕੋਡ)) {
        ਵਾਪਸ 0;
    }
    int32_t name_ਸੂਚਕਾਂਕ=ਸੂਚਕਾਂਕ=skipWhiteSpace(ਸੂਚਕਾਂਕ+arg_ਸ਼ੁਰੂ_ਲੰਮਾਈ);
    ਜੇ(ਸੂਚਕਾਂਕ==ਸੁਨੇਹਾ.length()) {
        ਸੈੱਟ_Parse_Error(parseError, 0);  // Unmatched '{' braces in message.
        error_ਕੋਡ=U_UNMATCHED_BRACES;
        ਵਾਪਸ 0;
    }
    // parse argument name or number
    ਸੂਚਕਾਂਕ=skipIdentifier(ਸੂਚਕਾਂਕ);
    int32_t number=parse_Arg_ਸੰਖਿਆ(name_ਸੂਚਕਾਂਕ, ਸੂਚਕਾਂਕ);
    ਜੇ(number>=0) {
        int32_t length=ਸੂਚਕਾਂਕ-name_ਸੂਚਕਾਂਕ;
        ਜੇ(length>Part::MAX_ਲੰਮਾਈ || number>Part::MAX_ਮੁੱਲ) {
            ਸੈੱਟ_Parse_Error(parseError, name_ਸੂਚਕਾਂਕ);  // Argument number too large.
            error_ਕੋਡ=U_ਸੂਚਕਾਂਕ_OUTOFBOUNDS_ERROR;
            ਵਾਪਸ 0;
        }
        hasArgNumbers=ਸੱਚ;
        ਜੋੜੋ_Part(UMSGPAT_PART_TYPE_ARG_ਸੰਖਿਆ, name_ਸੂਚਕਾਂਕ, length, number, error_ਕੋਡ);
    } ਨਹੀਂਤਾਂ ਜੇ(number==UMSGPAT_ARG_NAME_NOT_ਸੰਖਿਆ) {
        int32_t length=ਸੂਚਕਾਂਕ-name_ਸੂਚਕਾਂਕ;
        ਜੇ(length>Part::MAX_ਲੰਮਾਈ) {
            ਸੈੱਟ_Parse_Error(parseError, name_ਸੂਚਕਾਂਕ);  // Argument name too long.
            error_ਕੋਡ=U_ਸੂਚਕਾਂਕ_OUTOFBOUNDS_ERROR;
            ਵਾਪਸ 0;
        }
        has_Arg_ਨਾਂ=ਸੱਚ;
        ਜੋੜੋ_Part(UMSGPAT_PART_TYPE_ARG_NAME, name_ਸੂਚਕਾਂਕ, length, 0, error_ਕੋਡ);
    } ਨਹੀਂਤਾਂ {  // number<-1 (ARG_NAME_NOT_VALID)
        ਸੈੱਟ_Parse_Error(parseError, name_ਸੂਚਕਾਂਕ);  // Bad argument syntax.
        error_ਕੋਡ=U_ਨਮੂਨਾ_SYNTAX_ERROR;
        ਵਾਪਸ 0;
    }
    ਸੂਚਕਾਂਕ=skipWhiteSpace(ਸੂਚਕਾਂਕ);
    ਜੇ(ਸੂਚਕਾਂਕ==ਸੁਨੇਹਾ.length()) {
        ਸੈੱਟ_Parse_Error(parseError, 0);  // Unmatched '{' braces in message.
        error_ਕੋਡ=U_UNMATCHED_BRACES;
        ਵਾਪਸ 0;
    }
    char16_t c=ਸੁਨੇਹਾ.charAt(ਸੂਚਕਾਂਕ);
    ਜੇ(c==u_ਸੱਜਾ_Curly_Brace) {
        // all done
    } ਨਹੀਂਤਾਂ ਜੇ(c!=u_comma) {
        ਸੈੱਟ_Parse_Error(parseError, name_ਸੂਚਕਾਂਕ);  // Bad argument syntax.
        error_ਕੋਡ=U_ਨਮੂਨਾ_SYNTAX_ERROR;
        ਵਾਪਸ 0;
    } ਨਹੀਂਤਾਂ /* ',' */ {
        // parse argument type: case-sensitive a-zA-Z
        int32_t type_ਸੂਚਕਾਂਕ=ਸੂਚਕਾਂਕ=skipWhiteSpace(ਸੂਚਕਾਂਕ+1);
        ਜਦੋਂ(ਸੂਚਕਾਂਕ<ਸੁਨੇਹਾ.length() && isArgTypeChar(ਸੁਨੇਹਾ.charAt(ਸੂਚਕਾਂਕ))) {
            ++ਸੂਚਕਾਂਕ;
        }
        int32_t length=ਸੂਚਕਾਂਕ-type_ਸੂਚਕਾਂਕ;
        ਸੂਚਕਾਂਕ=skipWhiteSpace(ਸੂਚਕਾਂਕ);
        ਜੇ(ਸੂਚਕਾਂਕ==ਸੁਨੇਹਾ.length()) {
            ਸੈੱਟ_Parse_Error(parseError, 0);  // Unmatched '{' braces in message.
            error_ਕੋਡ=U_UNMATCHED_BRACES;
            ਵਾਪਸ 0;
        }
        ਜੇ(length==0 || ((c=ਸੁਨੇਹਾ.charAt(ਸੂਚਕਾਂਕ))!=u_comma && c!=u_ਸੱਜਾ_Curly_Brace)) {
            ਸੈੱਟ_Parse_Error(parseError, name_ਸੂਚਕਾਂਕ);  // Bad argument syntax.
            error_ਕੋਡ=U_ਨਮੂਨਾ_SYNTAX_ERROR;
            ਵਾਪਸ 0;
        }
        ਜੇ(length>Part::MAX_ਲੰਮਾਈ) {
            ਸੈੱਟ_Parse_Error(parseError, name_ਸੂਚਕਾਂਕ);  // Argument type name too long.
            error_ਕੋਡ=U_ਸੂਚਕਾਂਕ_OUTOFBOUNDS_ERROR;
            ਵਾਪਸ 0;
        }
        argType=UMSGPAT_ARG_TYPE_SIMPLE;
        ਜੇ(length==6) {
            // case-insensitive comparisons for complex-type names
            ਜੇ(isChoice(type_ਸੂਚਕਾਂਕ)) {
                argType=UMSGPAT_ARG_TYPE_CHOICE;
            } ਨਹੀਂਤਾਂ ਜੇ(isPlural(type_ਸੂਚਕਾਂਕ)) {
                argType=UMSGPAT_ARG_TYPE_PLURAL;
            } ਨਹੀਂਤਾਂ ਜੇ(isSelect(type_ਸੂਚਕਾਂਕ)) {
                argType=UMSGPAT_ARG_TYPE_SELECT;
            }
        } ਨਹੀਂਤਾਂ ਜੇ(length==13) {
            ਜੇ(isSelect(type_ਸੂਚਕਾਂਕ) && isOrdinal(type_ਸੂਚਕਾਂਕ+6)) {
                argType=UMSGPAT_ARG_TYPE_SELECTORDINAL;
            }
        }
        // change the ARG_START type from NONE to argType
        partsList->a[arg_ਸ਼ੁਰੂ].value = static_cast<int16_t>(argType);
        ਜੇ(argType==UMSGPAT_ARG_TYPE_SIMPLE) {
            ਜੋੜੋ_Part(UMSGPAT_PART_TYPE_ARG_TYPE, type_ਸੂਚਕਾਂਕ, length, 0, error_ਕੋਡ);
        }
        // look for an argument style (pattern)
        ਜੇ(c==u_ਸੱਜਾ_Curly_Brace) {
            ਜੇ(argType!=UMSGPAT_ARG_TYPE_SIMPLE) {
                ਸੈੱਟ_Parse_Error(parseError, name_ਸੂਚਕਾਂਕ);  // No style field for complex argument.
                error_ਕੋਡ=U_ਨਮੂਨਾ_SYNTAX_ERROR;
                ਵਾਪਸ 0;
            }
        } ਨਹੀਂਤਾਂ /* ',' */ {
            ++ਸੂਚਕਾਂਕ;
            ਜੇ(argType==UMSGPAT_ARG_TYPE_SIMPLE) {
                ਸੂਚਕਾਂਕ=parseSimpleStyle(ਸੂਚਕਾਂਕ, parseError, error_ਕੋਡ);
            } ਨਹੀਂਤਾਂ ਜੇ(argType==UMSGPAT_ARG_TYPE_CHOICE) {
                ਸੂਚਕਾਂਕ=parseChoiceStyle(ਸੂਚਕਾਂਕ, nestingLevel, parseError, error_ਕੋਡ);
            } ਨਹੀਂਤਾਂ {
                ਸੂਚਕਾਂਕ=parsePluralOrSelectStyle(argType, ਸੂਚਕਾਂਕ, nestingLevel, parseError, error_ਕੋਡ);
            }
        }
    }
    // Argument parsing stopped on the '}'.
    ਜੋੜੋ_ਹੱਦਬੰਦੀ_Part(arg_ਸ਼ੁਰੂ, UMSGPAT_PART_TYPE_ARG_ਹੱਦਬੰਦੀ, ਸੂਚਕਾਂਕ, 1, argType, error_ਕੋਡ);
    ਵਾਪਸ ਸੂਚਕਾਂਕ+1;
}

int32_t
ਸੁਨੇਹਾ_ਨਮੂਨਾ::parseSimpleStyle(int32_t ਸੂਚਕਾਂਕ, UParseError *parseError, UError_ਕੋਡ &error_ਕੋਡ) {
    ਜੇ(U_FAILURE(error_ਕੋਡ)) {
        ਵਾਪਸ 0;
    }
    int32_t ਸ਼ੁਰੂ=ਸੂਚਕਾਂਕ;
    int32_t nestedBraces=0;
    ਜਦੋਂ(ਸੂਚਕਾਂਕ<ਸੁਨੇਹਾ.length()) {
        char16_t c=ਸੁਨੇਹਾ.charAt(ਸੂਚਕਾਂਕ++);
        ਜੇ(c==u_apos) {
            // Treat apostrophe as quoting but include it in the style part.
            // Find the end of the quoted literal text.
            ਸੂਚਕਾਂਕ=ਸੁਨੇਹਾ.ਸੂਚਕਾਂਕ_ਦਾ(u_apos, ਸੂਚਕਾਂਕ);
            ਜੇ(ਸੂਚਕਾਂਕ<0) {
                // Quoted literal argument style text reaches to the end of the message.
                ਸੈੱਟ_Parse_Error(parseError, ਸ਼ੁਰੂ);
                error_ਕੋਡ=U_ਨਮੂਨਾ_SYNTAX_ERROR;
                ਵਾਪਸ 0;
            }
            // skip the quote-ending apostrophe
            ++ਸੂਚਕਾਂਕ;
        } ਨਹੀਂਤਾਂ ਜੇ(c==u_ਖੱਬਾ_Curly_Brace) {
            ++nestedBraces;
        } ਨਹੀਂਤਾਂ ਜੇ(c==u_ਸੱਜਾ_Curly_Brace) {
            ਜੇ(nestedBraces>0) {
                --nestedBraces;
            } ਨਹੀਂਤਾਂ {
                int32_t length=--ਸੂਚਕਾਂਕ-ਸ਼ੁਰੂ;
                ਜੇ(length>Part::MAX_ਲੰਮਾਈ) {
                    ਸੈੱਟ_Parse_Error(parseError, ਸ਼ੁਰੂ);  // Argument style text too long.
                    error_ਕੋਡ=U_ਸੂਚਕਾਂਕ_OUTOFBOUNDS_ERROR;
                    ਵਾਪਸ 0;
                }
                ਜੋੜੋ_Part(UMSGPAT_PART_TYPE_ARG_STYLE, ਸ਼ੁਰੂ, length, 0, error_ਕੋਡ);
                ਵਾਪਸ ਸੂਚਕਾਂਕ;
            }
        }  // c is part of literal text
    }
    ਸੈੱਟ_Parse_Error(parseError, 0);  // Unmatched '{' braces in message.
    error_ਕੋਡ=U_UNMATCHED_BRACES;
    ਵਾਪਸ 0;
}

int32_t
ਸੁਨੇਹਾ_ਨਮੂਨਾ::parseChoiceStyle(int32_t ਸੂਚਕਾਂਕ, int32_t nestingLevel,
                                 UParseError *parseError, UError_ਕੋਡ &error_ਕੋਡ) {
    ਜੇ(U_FAILURE(error_ਕੋਡ)) {
        ਵਾਪਸ 0;
    }
    int32_t ਸ਼ੁਰੂ=ਸੂਚਕਾਂਕ;
    ਸੂਚਕਾਂਕ=skipWhiteSpace(ਸੂਚਕਾਂਕ);
    ਜੇ(ਸੂਚਕਾਂਕ==ਸੁਨੇਹਾ.length() || ਸੁਨੇਹਾ.charAt(ਸੂਚਕਾਂਕ)==u_ਸੱਜਾ_Curly_Brace) {
        ਸੈੱਟ_Parse_Error(parseError, 0);  // Missing choice argument pattern.
        error_ਕੋਡ=U_ਨਮੂਨਾ_SYNTAX_ERROR;
        ਵਾਪਸ 0;
    }
    ਲਈ(;;) {
        // The choice argument style contains |-separated (number, separator, message) triples.
        // Parse the number.
        int32_t ਸੰਖਿਆ_ਸੂਚਕਾਂਕ=ਸੂਚਕਾਂਕ;
        ਸੂਚਕਾਂਕ=skipDouble(ਸੂਚਕਾਂਕ);
        int32_t length=ਸੂਚਕਾਂਕ-ਸੰਖਿਆ_ਸੂਚਕਾਂਕ;
        ਜੇ(length==0) {
            ਸੈੱਟ_Parse_Error(parseError, ਸ਼ੁਰੂ);  // Bad choice pattern syntax.
            error_ਕੋਡ=U_ਨਮੂਨਾ_SYNTAX_ERROR;
            ਵਾਪਸ 0;
        }
        ਜੇ(length>Part::MAX_ਲੰਮਾਈ) {
            ਸੈੱਟ_Parse_Error(parseError, ਸੰਖਿਆ_ਸੂਚਕਾਂਕ);  // Choice number too long.
            error_ਕੋਡ=U_ਸੂਚਕਾਂਕ_OUTOFBOUNDS_ERROR;
            ਵਾਪਸ 0;
        }
        parseDouble(ਸੰਖਿਆ_ਸੂਚਕਾਂਕ, ਸੂਚਕਾਂਕ, ਸੱਚ, parseError, error_ਕੋਡ);  // adds ARG_INT or ARG_DOUBLE
        ਜੇ(U_FAILURE(error_ਕੋਡ)) {
            ਵਾਪਸ 0;
        }
        // Parse the separator.
        ਸੂਚਕਾਂਕ=skipWhiteSpace(ਸੂਚਕਾਂਕ);
        ਜੇ(ਸੂਚਕਾਂਕ==ਸੁਨੇਹਾ.length()) {
            ਸੈੱਟ_Parse_Error(parseError, ਸ਼ੁਰੂ);  // Bad choice pattern syntax.
            error_ਕੋਡ=U_ਨਮੂਨਾ_SYNTAX_ERROR;
            ਵਾਪਸ 0;
        }
        char16_t c=ਸੁਨੇਹਾ.charAt(ਸੂਚਕਾਂਕ);
        ਜੇ(!(c==u_pound || c==u_lessThan || c==u_lessOrEqual)) {  // U+2264 is <=
            ਸੈੱਟ_Parse_Error(parseError, ਸ਼ੁਰੂ);  // Expected choice separator (#<\u2264) instead of c.
            error_ਕੋਡ=U_ਨਮੂਨਾ_SYNTAX_ERROR;
            ਵਾਪਸ 0;
        }
        ਜੋੜੋ_Part(UMSGPAT_PART_TYPE_ARG_SELECTOR, ਸੂਚਕਾਂਕ, 1, 0, error_ਕੋਡ);
        // Parse the message fragment.
        ਸੂਚਕਾਂਕ=parse_ਸੁਨੇਹਾ(++ਸੂਚਕਾਂਕ, 0, nestingLevel+1, UMSGPAT_ARG_TYPE_CHOICE, parseError, error_ਕੋਡ);
        ਜੇ(U_FAILURE(error_ਕੋਡ)) {
            ਵਾਪਸ 0;
        }
        // parseMessage(..., CHOICE) returns the index of the terminator, or msg.length().
        ਜੇ(ਸੂਚਕਾਂਕ==ਸੁਨੇਹਾ.length()) {
            ਵਾਪਸ ਸੂਚਕਾਂਕ;
        }
        ਜੇ(ਸੁਨੇਹਾ.charAt(ਸੂਚਕਾਂਕ)==u_ਸੱਜਾ_Curly_Brace) {
            ਜੇ(!ਵਿੱਚ_ਸੁਨੇਹਾ_Format_ਨਮੂਨਾ(nestingLevel)) {
                ਸੈੱਟ_Parse_Error(parseError, ਸ਼ੁਰੂ);  // Bad choice pattern syntax.
                error_ਕੋਡ=U_ਨਮੂਨਾ_SYNTAX_ERROR;
                ਵਾਪਸ 0;
            }
            ਵਾਪਸ ਸੂਚਕਾਂਕ;
        }  // else the terminator is '|'
        ਸੂਚਕਾਂਕ=skipWhiteSpace(ਸੂਚਕਾਂਕ+1);
    }
}

int32_t
ਸੁਨੇਹਾ_ਨਮੂਨਾ::parsePluralOrSelectStyle(UMessage_ਨਮੂਨਾ_Arg_Type argType,
                                         int32_t ਸੂਚਕਾਂਕ, int32_t nestingLevel,
                                         UParseError *parseError, UError_ਕੋਡ &error_ਕੋਡ) {
    ਜੇ(U_FAILURE(error_ਕੋਡ)) {
        ਵਾਪਸ 0;
    }
    int32_t ਸ਼ੁਰੂ=ਸੂਚਕਾਂਕ;
    UBool is_ਖਾਲੀ_ਥਾਵਾਂ=ਸੱਚ;
    UBool hasOther=ਝੂਠ;
    ਲਈ(;;) {
        // First, collect the selector looking for a small set of terminators.
        // It would be a little faster to consider the syntax of each possible
        // token right here, but that makes the code too complicated.
        ਸੂਚਕਾਂਕ=skipWhiteSpace(ਸੂਚਕਾਂਕ);
        UBool eos=ਸੂਚਕਾਂਕ==ਸੁਨੇਹਾ.length();
        ਜੇ(eos || ਸੁਨੇਹਾ.charAt(ਸੂਚਕਾਂਕ)==u_ਸੱਜਾ_Curly_Brace) {
            ਜੇ(eos==ਵਿੱਚ_ਸੁਨੇਹਾ_Format_ਨਮੂਨਾ(nestingLevel)) {
                ਸੈੱਟ_Parse_Error(parseError, ਸ਼ੁਰੂ);  // Bad plural/select pattern syntax.
                error_ਕੋਡ=U_ਨਮੂਨਾ_SYNTAX_ERROR;
                ਵਾਪਸ 0;
            }
            ਜੇ(!hasOther) {
                ਸੈੱਟ_Parse_Error(parseError, 0);  // Missing 'other' keyword in plural/select pattern.
                error_ਕੋਡ=U_DEFAULT_KEYWORD_MISSING;
                ਵਾਪਸ 0;
            }
            ਵਾਪਸ ਸੂਚਕਾਂਕ;
        }
        int32_t selector_ਸੂਚਕਾਂਕ=ਸੂਚਕਾਂਕ;
        ਜੇ(UMSGPAT_ARG_TYPE_HAS_PLURAL_STYLE(argType) && ਸੁਨੇਹਾ.charAt(selector_ਸੂਚਕਾਂਕ)==u_equal) {
            // explicit-value plural selector: =double
            ਸੂਚਕਾਂਕ=skipDouble(ਸੂਚਕਾਂਕ+1);
            int32_t length=ਸੂਚਕਾਂਕ-selector_ਸੂਚਕਾਂਕ;
            ਜੇ(length==1) {
                ਸੈੱਟ_Parse_Error(parseError, ਸ਼ੁਰੂ);  // Bad plural/select pattern syntax.
                error_ਕੋਡ=U_ਨਮੂਨਾ_SYNTAX_ERROR;
                ਵਾਪਸ 0;
            }
            ਜੇ(length>Part::MAX_ਲੰਮਾਈ) {
                ਸੈੱਟ_Parse_Error(parseError, selector_ਸੂਚਕਾਂਕ);  // Argument selector too long.
                error_ਕੋਡ=U_ਸੂਚਕਾਂਕ_OUTOFBOUNDS_ERROR;
                ਵਾਪਸ 0;
            }
            ਜੋੜੋ_Part(UMSGPAT_PART_TYPE_ARG_SELECTOR, selector_ਸੂਚਕਾਂਕ, length, 0, error_ਕੋਡ);
            parseDouble(selector_ਸੂਚਕਾਂਕ+1, ਸੂਚਕਾਂਕ, ਝੂਠ,
                        parseError, error_ਕੋਡ);  // adds ARG_INT or ARG_DOUBLE
        } ਨਹੀਂਤਾਂ {
            ਸੂਚਕਾਂਕ=skipIdentifier(ਸੂਚਕਾਂਕ);
            int32_t length=ਸੂਚਕਾਂਕ-selector_ਸੂਚਕਾਂਕ;
            ਜੇ(length==0) {
                ਸੈੱਟ_Parse_Error(parseError, ਸ਼ੁਰੂ);  // Bad plural/select pattern syntax.
                error_ਕੋਡ=U_ਨਮੂਨਾ_SYNTAX_ERROR;
                ਵਾਪਸ 0;
            }
            // Note: The ':' in "offset:" is just beyond the skipIdentifier() range.
            ਜੇ( UMSGPAT_ARG_TYPE_HAS_PLURAL_STYLE(argType) && length==6 && ਸੂਚਕਾਂਕ<ਸੁਨੇਹਾ.length() &&
                0==ਸੁਨੇਹਾ.compare(selector_ਸੂਚਕਾਂਕ, 7, k_ਵਿੱਥ_Colon, 0, 7)
            ) {
                // plural offset, not a selector
                ਜੇ(!is_ਖਾਲੀ_ਥਾਵਾਂ) {
                    // Plural argument 'offset:' (if present) must precede key-message pairs.
                    ਸੈੱਟ_Parse_Error(parseError, ਸ਼ੁਰੂ);
                    error_ਕੋਡ=U_ਨਮੂਨਾ_SYNTAX_ERROR;
                    ਵਾਪਸ 0;
                }
                // allow whitespace between offset: and its value
                int32_t ਮੁੱਲ_ਸੂਚਕਾਂਕ=skipWhiteSpace(ਸੂਚਕਾਂਕ+1);  // The ':' is at index.
                ਸੂਚਕਾਂਕ=skipDouble(ਮੁੱਲ_ਸੂਚਕਾਂਕ);
                ਜੇ(ਸੂਚਕਾਂਕ==ਮੁੱਲ_ਸੂਚਕਾਂਕ) {
                    ਸੈੱਟ_Parse_Error(parseError, ਸ਼ੁਰੂ);  // Missing value for plural 'offset:'.
                    error_ਕੋਡ=U_ਨਮੂਨਾ_SYNTAX_ERROR;
                    ਵਾਪਸ 0;
                }
                ਜੇ((ਸੂਚਕਾਂਕ-ਮੁੱਲ_ਸੂਚਕਾਂਕ)>Part::MAX_ਲੰਮਾਈ) {
                    ਸੈੱਟ_Parse_Error(parseError, ਮੁੱਲ_ਸੂਚਕਾਂਕ);  // Plural offset value too long.
                    error_ਕੋਡ=U_ਸੂਚਕਾਂਕ_OUTOFBOUNDS_ERROR;
                    ਵਾਪਸ 0;
                }
                parseDouble(ਮੁੱਲ_ਸੂਚਕਾਂਕ, ਸੂਚਕਾਂਕ, ਝੂਠ,
                            parseError, error_ਕੋਡ);  // adds ARG_INT or ARG_DOUBLE
                ਜੇ(U_FAILURE(error_ਕੋਡ)) {
                    ਵਾਪਸ 0;
                }
                is_ਖਾਲੀ_ਥਾਵਾਂ=ਝੂਠ;
                ਜਾਰੀ;  // no message fragment after the offset
            } ਨਹੀਂਤਾਂ {
                // normal selector word
                ਜੇ(length>Part::MAX_ਲੰਮਾਈ) {
                    ਸੈੱਟ_Parse_Error(parseError, selector_ਸੂਚਕਾਂਕ);  // Argument selector too long.
                    error_ਕੋਡ=U_ਸੂਚਕਾਂਕ_OUTOFBOUNDS_ERROR;
                    ਵਾਪਸ 0;
                }
                ਜੋੜੋ_Part(UMSGPAT_PART_TYPE_ARG_SELECTOR, selector_ਸੂਚਕਾਂਕ, length, 0, error_ਕੋਡ);
                ਜੇ(0==ਸੁਨੇਹਾ.compare(selector_ਸੂਚਕਾਂਕ, length, kOther, 0, 5)) {
                    hasOther=ਸੱਚ;
                }
            }
        }
        ਜੇ(U_FAILURE(error_ਕੋਡ)) {
            ਵਾਪਸ 0;
        }

        // parse the message fragment following the selector
        ਸੂਚਕਾਂਕ=skipWhiteSpace(ਸੂਚਕਾਂਕ);
        ਜੇ(ਸੂਚਕਾਂਕ==ਸੁਨੇਹਾ.length() || ਸੁਨੇਹਾ.charAt(ਸੂਚਕਾਂਕ)!=u_ਖੱਬਾ_Curly_Brace) {
            ਸੈੱਟ_Parse_Error(parseError, selector_ਸੂਚਕਾਂਕ);  // No message fragment after plural/select selector.
            error_ਕੋਡ=U_ਨਮੂਨਾ_SYNTAX_ERROR;
            ਵਾਪਸ 0;
        }
        ਸੂਚਕਾਂਕ=parse_ਸੁਨੇਹਾ(ਸੂਚਕਾਂਕ, 1, nestingLevel+1, argType, parseError, error_ਕੋਡ);
        ਜੇ(U_FAILURE(error_ਕੋਡ)) {
            ਵਾਪਸ 0;
        }
        is_ਖਾਲੀ_ਥਾਵਾਂ=ਝੂਠ;
    }
}

int32_t
ਸੁਨੇਹਾ_ਨਮੂਨਾ::parse_Arg_ਸੰਖਿਆ(ਅਚਲ UnicodeString &s, int32_t ਸ਼ੁਰੂ, int32_t ਹੱਦਬੰਦੀ) {
    // If the identifier contains only ASCII digits, then it is an argument _number_
    // and must not have leading zeros (except "0" itself).
    // Otherwise it is an argument _name_.
    ਜੇ(ਸ਼ੁਰੂ>=ਹੱਦਬੰਦੀ) {
        ਵਾਪਸ UMSGPAT_ARG_NAME_NOT_VALID;
    }
    int32_t number;
    // Defer numeric errors until we know there are only digits.
    UBool bad_ਸੰਖਿਆ;
    char16_t c=s.charAt(ਸ਼ੁਰੂ++);
    ਜੇ(c==0x30) {
        ਜੇ(ਸ਼ੁਰੂ==ਹੱਦਬੰਦੀ) {
            ਵਾਪਸ 0;
        } ਨਹੀਂਤਾਂ {
            number=0;
            bad_ਸੰਖਿਆ=ਸੱਚ;  // leading zero
        }
    } ਨਹੀਂਤਾਂ ਜੇ(0x31<=c && c<=0x39) {
        number=c-0x30;
        bad_ਸੰਖਿਆ=ਝੂਠ;
    } ਨਹੀਂਤਾਂ {
        ਵਾਪਸ UMSGPAT_ARG_NAME_NOT_ਸੰਖਿਆ;
    }
    ਜਦੋਂ(ਸ਼ੁਰੂ<ਹੱਦਬੰਦੀ) {
        c=s.charAt(ਸ਼ੁਰੂ++);
        ਜੇ(0x30<=c && c<=0x39) {
            ਜੇ(number>=INT32_MAX/10) {
                bad_ਸੰਖਿਆ=ਸੱਚ;  //ਉਛਾਲ
            } ਨਹੀਂਤਾਂ {
                number=number*10+(c-0x30);
            }
        } ਨਹੀਂਤਾਂ {
            ਵਾਪਸ UMSGPAT_ARG_NAME_NOT_ਸੰਖਿਆ;
        }
    }
    // There are only ASCII digits.
    ਜੇ(bad_ਸੰਖਿਆ) {
        ਵਾਪਸ UMSGPAT_ARG_NAME_NOT_VALID;
    } ਨਹੀਂਤਾਂ {
        ਵਾਪਸ number;
    }
}

ਖਾਲੀ
ਸੁਨੇਹਾ_ਨਮੂਨਾ::parseDouble(int32_t ਸ਼ੁਰੂ, int32_t ਹੱਦਬੰਦੀ, UBool allowInfinity,
                            UParseError *parseError, UError_ਕੋਡ &error_ਕੋਡ) {
    ਜੇ(U_FAILURE(error_ਕੋਡ)) {
        ਵਾਪਸ;
    }
    U_ASSERT(ਸ਼ੁਰੂ<ਹੱਦਬੰਦੀ);
    // fake loop for easy exit and single throw statement
    ਲਈ(;;) { /*loop doesn't iterate*/
        // fast path for small integers and infinity
        int32_t value=0;
        int32_t isNegative=0;  // not boolean so that we can easily add it to value
        int32_t ਸੂਚਕਾਂਕ=ਸ਼ੁਰੂ;
        char16_t c=ਸੁਨੇਹਾ.charAt(ਸੂਚਕਾਂਕ++);
        ਜੇ(c==u_minus) {
            isNegative=1;
            ਜੇ(ਸੂਚਕਾਂਕ==ਹੱਦਬੰਦੀ) {
                ਰੋਕੋ;  // no number
            }
            c=ਸੁਨੇਹਾ.charAt(ਸੂਚਕਾਂਕ++);
        } ਨਹੀਂਤਾਂ ਜੇ(c==u_plus) {
            ਜੇ(ਸੂਚਕਾਂਕ==ਹੱਦਬੰਦੀ) {
                ਰੋਕੋ;  // no number
            }
            c=ਸੁਨੇਹਾ.charAt(ਸੂਚਕਾਂਕ++);
        }
        ਜੇ(c==0x221e) {  // infinity
            ਜੇ(allowInfinity && ਸੂਚਕਾਂਕ==ਹੱਦਬੰਦੀ) {
                ਦੁੱਗਣਾ infinity=uprv_getInfinity();
                ਜੋੜੋ_Arg_Double_Part(
                    isNegative!=0 ? -infinity : infinity,
                    ਸ਼ੁਰੂ, ਹੱਦਬੰਦੀ-ਸ਼ੁਰੂ, error_ਕੋਡ);
                ਵਾਪਸ;
            } ਨਹੀਂਤਾਂ {
                ਰੋਕੋ;
            }
        }
        // try to parse the number as a small integer but fall back to a double
        ਜਦੋਂ('0'<=c && c<='9') {
            value=value*10+(c-'0');
            ਜੇ(value>(Part::MAX_ਮੁੱਲ+isNegative)) {
                ਰੋਕੋ;  // not a small-enough integer
            }
            ਜੇ(ਸੂਚਕਾਂਕ==ਹੱਦਬੰਦੀ) {
                ਜੋੜੋ_Part(UMSGPAT_PART_TYPE_ARG_INT, ਸ਼ੁਰੂ, ਹੱਦਬੰਦੀ-ਸ਼ੁਰੂ,
                        isNegative!=0 ? -value : value, error_ਕੋਡ);
                ਵਾਪਸ;
            }
            c=ਸੁਨੇਹਾ.charAt(ਸੂਚਕਾਂਕ++);
        }
        // Let Double.parseDouble() throw a NumberFormatException.
        ਅੱਖਰ ਸੰਖਿਆ_Chars[128];
        int32_t capacity = static_cast<int32_t>(ਆਕਾਰ(ਸੰਖਿਆ_Chars));
        int32_t length=ਹੱਦਬੰਦੀ-ਸ਼ੁਰੂ;
        ਜੇ(length>=capacity) {
            ਰੋਕੋ;  // number too long
        }
        ਸੁਨੇਹਾ.extract(ਸ਼ੁਰੂ, length, ਸੰਖਿਆ_Chars, capacity, US_INV);
        ਜੇ (static_cast<int32_t>(uprv_strlen(ਸੰਖਿਆ_Chars)) < length) {
            ਰੋਕੋ;  // contains non-invariant character that was turned into NUL
        }
        ਅੱਖਰ *ਸਿਰਾ;
        ਦੁੱਗਣਾ numeric_ਮੁੱਲ=uprv_strtod(ਸੰਖਿਆ_Chars, &ਸਿਰਾ);
        ਜੇ(ਸਿਰਾ!=(ਸੰਖਿਆ_Chars+length)) {
            ਰੋਕੋ;  // parsing error
        }
        ਜੋੜੋ_Arg_Double_Part(numeric_ਮੁੱਲ, ਸ਼ੁਰੂ, length, error_ਕੋਡ);
        ਵਾਪਸ;
    }
    ਸੈੱਟ_Parse_Error(parseError, ਸ਼ੁਰੂ /*, limit*/);  // Bad syntax for numeric value.
    error_ਕੋਡ=U_ਨਮੂਨਾ_SYNTAX_ERROR;
}

int32_t
ਸੁਨੇਹਾ_ਨਮੂਨਾ::skipWhiteSpace(int32_t ਸੂਚਕਾਂਕ) {
    ਅਚਲ char16_t *s=ਸੁਨੇਹਾ.get_ਬਫਰ();
    int32_t ਸੁਨੇਹਾ_ਲੰਮਾਈ=ਸੁਨੇਹਾ.length();
    ਅਚਲ char16_t *t=ਨਮੂਨਾ_Props::skipWhiteSpace(s+ਸੂਚਕਾਂਕ, ਸੁਨੇਹਾ_ਲੰਮਾਈ-ਸੂਚਕਾਂਕ);
    ਵਾਪਸ static_cast<int32_t>(t - s);
}

int32_t
ਸੁਨੇਹਾ_ਨਮੂਨਾ::skipIdentifier(int32_t ਸੂਚਕਾਂਕ) {
    ਅਚਲ char16_t *s=ਸੁਨੇਹਾ.get_ਬਫਰ();
    int32_t ਸੁਨੇਹਾ_ਲੰਮਾਈ=ਸੁਨੇਹਾ.length();
    ਅਚਲ char16_t *t=ਨਮੂਨਾ_Props::skipIdentifier(s+ਸੂਚਕਾਂਕ, ਸੁਨੇਹਾ_ਲੰਮਾਈ-ਸੂਚਕਾਂਕ);
    ਵਾਪਸ static_cast<int32_t>(t - s);
}

int32_t
ਸੁਨੇਹਾ_ਨਮੂਨਾ::skipDouble(int32_t ਸੂਚਕਾਂਕ) {
    int32_t ਸੁਨੇਹਾ_ਲੰਮਾਈ=ਸੁਨੇਹਾ.length();
    ਜਦੋਂ(ਸੂਚਕਾਂਕ<ਸੁਨੇਹਾ_ਲੰਮਾਈ) {
        char16_t c=ਸੁਨੇਹਾ.charAt(ਸੂਚਕਾਂਕ);
        // U+221E: Allow the infinity symbol, for ChoiceFormat patterns.
        ਜੇ((c<0x30 && c!=u_plus && c!=u_minus && c!=u_dot) || (c>0x39 && c!=u_e && c!=u_E && c!=0x221e)) {
            ਰੋਕੋ;
        }
        ++ਸੂਚਕਾਂਕ;
    }
    ਵਾਪਸ ਸੂਚਕਾਂਕ;
}

UBool
ਸੁਨੇਹਾ_ਨਮੂਨਾ::isArgTypeChar(UChar32 c) {
    ਵਾਪਸ (u_a<=c && c<=u_z) || (u_A<=c && c<=u_Z);
}

UBool
ਸੁਨੇਹਾ_ਨਮੂਨਾ::isChoice(int32_t ਸੂਚਕਾਂਕ) {
    char16_t c;
    ਵਾਪਸ
        ((c=ਸੁਨੇਹਾ.charAt(ਸੂਚਕਾਂਕ++))==u_c || c==u_C) &&
        ((c=ਸੁਨੇਹਾ.charAt(ਸੂਚਕਾਂਕ++))==u_h || c==u_H) &&
        ((c=ਸੁਨੇਹਾ.charAt(ਸੂਚਕਾਂਕ++))==u_o || c==u_O) &&
        ((c=ਸੁਨੇਹਾ.charAt(ਸੂਚਕਾਂਕ++))==u_i || c==u_I) &&
        ((c=ਸੁਨੇਹਾ.charAt(ਸੂਚਕਾਂਕ++))==u_c || c==u_C) &&
        ((c=ਸੁਨੇਹਾ.charAt(ਸੂਚਕਾਂਕ))==u_e || c==u_E);
}

UBool
ਸੁਨੇਹਾ_ਨਮੂਨਾ::isPlural(int32_t ਸੂਚਕਾਂਕ) {
    char16_t c;
    ਵਾਪਸ
        ((c=ਸੁਨੇਹਾ.charAt(ਸੂਚਕਾਂਕ++))==u_p || c==u_P) &&
        ((c=ਸੁਨੇਹਾ.charAt(ਸੂਚਕਾਂਕ++))==u_l || c==u_L) &&
        ((c=ਸੁਨੇਹਾ.charAt(ਸੂਚਕਾਂਕ++))==u_u || c==u_U) &&
        ((c=ਸੁਨੇਹਾ.charAt(ਸੂਚਕਾਂਕ++))==u_r || c==u_R) &&
        ((c=ਸੁਨੇਹਾ.charAt(ਸੂਚਕਾਂਕ++))==u_a || c==u_A) &&
        ((c=ਸੁਨੇਹਾ.charAt(ਸੂਚਕਾਂਕ))==u_l || c==u_L);
}

UBool
ਸੁਨੇਹਾ_ਨਮੂਨਾ::isSelect(int32_t ਸੂਚਕਾਂਕ) {
    char16_t c;
    ਵਾਪਸ
        ((c=ਸੁਨੇਹਾ.charAt(ਸੂਚਕਾਂਕ++))==u_s || c==u_S) &&
        ((c=ਸੁਨੇਹਾ.charAt(ਸੂਚਕਾਂਕ++))==u_e || c==u_E) &&
        ((c=ਸੁਨੇਹਾ.charAt(ਸੂਚਕਾਂਕ++))==u_l || c==u_L) &&
        ((c=ਸੁਨੇਹਾ.charAt(ਸੂਚਕਾਂਕ++))==u_e || c==u_E) &&
        ((c=ਸੁਨੇਹਾ.charAt(ਸੂਚਕਾਂਕ++))==u_c || c==u_C) &&
        ((c=ਸੁਨੇਹਾ.charAt(ਸੂਚਕਾਂਕ))==u_t || c==u_T);
}

UBool
ਸੁਨੇਹਾ_ਨਮੂਨਾ::isOrdinal(int32_t ਸੂਚਕਾਂਕ) {
    char16_t c;
    ਵਾਪਸ
        ((c=ਸੁਨੇਹਾ.charAt(ਸੂਚਕਾਂਕ++))==u_o || c==u_O) &&
        ((c=ਸੁਨੇਹਾ.charAt(ਸੂਚਕਾਂਕ++))==u_r || c==u_R) &&
        ((c=ਸੁਨੇਹਾ.charAt(ਸੂਚਕਾਂਕ++))==u_d || c==u_D) &&
        ((c=ਸੁਨੇਹਾ.charAt(ਸੂਚਕਾਂਕ++))==u_i || c==u_I) &&
        ((c=ਸੁਨੇਹਾ.charAt(ਸੂਚਕਾਂਕ++))==u_n || c==u_N) &&
        ((c=ਸੁਨੇਹਾ.charAt(ਸੂਚਕਾਂਕ++))==u_a || c==u_A) &&
        ((c=ਸੁਨੇਹਾ.charAt(ਸੂਚਕਾਂਕ))==u_l || c==u_L);
}

UBool
ਸੁਨੇਹਾ_ਨਮੂਨਾ::ਵਿੱਚ_ਸੁਨੇਹਾ_Format_ਨਮੂਨਾ(int32_t nestingLevel) {
    ਵਾਪਸ nestingLevel>0 || partsList->a[0].type==UMSGPAT_PART_TYPE_ਸੁਨੇਹਾ_ਸ਼ੁਰੂ;
}

UBool
ਸੁਨੇਹਾ_ਨਮੂਨਾ::ਵਿੱਚ_ਸਿਖਰ_Level_Choice_ਸੁਨੇਹਾ(int32_t nestingLevel, UMessage_ਨਮੂਨਾ_Arg_Type ਮਾਪਾ_Type) {
    ਵਾਪਸ
        nestingLevel==1 &&
        ਮਾਪਾ_Type==UMSGPAT_ARG_TYPE_CHOICE &&
        partsList->a[0].type!=UMSGPAT_PART_TYPE_ਸੁਨੇਹਾ_ਸ਼ੁਰੂ;
}

ਖਾਲੀ
ਸੁਨੇਹਾ_ਨਮੂਨਾ::ਜੋੜੋ_Part(UMessage_ਨਮੂਨਾ_Part_Type type, int32_t ਸੂਚਕਾਂਕ, int32_t length,
                        int32_t value, UError_ਕੋਡ &error_ਕੋਡ) {
    ਜੇ(partsList->ensure_ਸਮਰੱਥਾ_For_One_ਹੋਰ(parts_ਲੰਮਾਈ, error_ਕੋਡ)) {
        Part &part=partsList->a[parts_ਲੰਮਾਈ++];
        part.type=type;
        part.ਸੂਚਕਾਂਕ=ਸੂਚਕਾਂਕ;
        part.length = static_cast<uint16_t>(length);
        part.value = static_cast<int16_t>(value);
        part.ਹੱਦਬੰਦੀ_Part_ਸੂਚਕਾਂਕ=0;
    }
}

ਖਾਲੀ
ਸੁਨੇਹਾ_ਨਮੂਨਾ::ਜੋੜੋ_ਹੱਦਬੰਦੀ_Part(int32_t ਸ਼ੁਰੂ,
                             UMessage_ਨਮੂਨਾ_Part_Type type, int32_t ਸੂਚਕਾਂਕ, int32_t length,
                             int32_t value, UError_ਕੋਡ &error_ਕੋਡ) {
    partsList->a[ਸ਼ੁਰੂ].ਹੱਦਬੰਦੀ_Part_ਸੂਚਕਾਂਕ=parts_ਲੰਮਾਈ;
    ਜੋੜੋ_Part(type, ਸੂਚਕਾਂਕ, length, value, error_ਕੋਡ);
}

ਖਾਲੀ
ਸੁਨੇਹਾ_ਨਮੂਨਾ::ਜੋੜੋ_Arg_Double_Part(ਦੁੱਗਣਾ numeric_ਮੁੱਲ, int32_t ਸ਼ੁਰੂ, int32_t length,
                                 UError_ਕੋਡ &error_ਕੋਡ) {
    ਜੇ(U_FAILURE(error_ਕੋਡ)) {
        ਵਾਪਸ;
    }
    int32_t numeric_ਸੂਚਕਾਂਕ=numeric_ਮੁੱਲਾਂ_ਲੰਮਾਈ;
    ਜੇ(numeric_ਮੁੱਲਾਂ_List==ਖਾਲੀ_ਸੂਚਕ) {
        numeric_ਮੁੱਲਾਂ_List=ਨਵਾਂ ਸੁਨੇਹਾ_ਨਮੂਨਾ_Double_List();
        ਜੇ(numeric_ਮੁੱਲਾਂ_List==ਖਾਲੀ_ਸੂਚਕ) {
            error_ਕੋਡ=U_ਸਮ੍ਰਿਤੀ_ਵੰਡ_ERROR;
            ਵਾਪਸ;
        }
    } ਨਹੀਂਤਾਂ ਜੇ(!numeric_ਮੁੱਲਾਂ_List->ensure_ਸਮਰੱਥਾ_For_One_ਹੋਰ(numeric_ਮੁੱਲਾਂ_ਲੰਮਾਈ, error_ਕੋਡ)) {
        ਵਾਪਸ;
    } ਨਹੀਂਤਾਂ {
        ਜੇ(numeric_ਸੂਚਕਾਂਕ>Part::MAX_ਮੁੱਲ) {
            error_ਕੋਡ=U_ਸੂਚਕਾਂਕ_OUTOFBOUNDS_ERROR;
            ਵਾਪਸ;
        }
    }
    numeric_ਮੁੱਲਾਂ_List->a[numeric_ਮੁੱਲਾਂ_ਲੰਮਾਈ++]=numeric_ਮੁੱਲ;
    ਜੋੜੋ_Part(UMSGPAT_PART_TYPE_ARG_DOUBLE, ਸ਼ੁਰੂ, length, numeric_ਸੂਚਕਾਂਕ, error_ਕੋਡ);
}

ਖਾਲੀ
ਸੁਨੇਹਾ_ਨਮੂਨਾ::ਸੈੱਟ_Parse_Error(UParseError *parseError, int32_t ਸੂਚਕਾਂਕ) {
    ਜੇ(parseError==ਖਾਲੀ_ਸੂਚਕ) {
        ਵਾਪਸ;
    }
    parseError->ਵਿੱਥ=ਸੂਚਕਾਂਕ;

    // Set preContext to some of msg before index.
    // Avoid splitting a surrogate pair.
    int32_t length=ਸੂਚਕਾਂਕ;
    ਜੇ(length>=U_PARSE_CONTEXT_LEN) {
        length=U_PARSE_CONTEXT_LEN-1;
        ਜੇ(length>0 && U16_IS_TRAIL(ਸੁਨੇਹਾ[ਸੂਚਕਾਂਕ-length])) {
            --length;
        }
    }
    ਸੁਨੇਹਾ.extract(ਸੂਚਕਾਂਕ-length, length, parseError->preContext);
    parseError->preContext[length]=0;

    // Set postContext to some of msg starting at index.
    length=ਸੁਨੇਹਾ.length()-ਸੂਚਕਾਂਕ;
    ਜੇ(length>=U_PARSE_CONTEXT_LEN) {
        length=U_PARSE_CONTEXT_LEN-1;
        ਜੇ(length>0 && U16_IS_LEAD(ਸੁਨੇਹਾ[ਸੂਚਕਾਂਕ+length-1])) {
            --length;
        }
    }
    ਸੁਨੇਹਾ.extract(ਸੂਚਕਾਂਕ, length, parseError->postContext);
    parseError->postContext[length]=0;
}

// MessageImpl ------------------------------------------------------------- ***

ਖਾਲੀ
ਸੁਨੇਹਾ_Impl::appendReducedApostrophes(ਅਚਲ UnicodeString &s, int32_t ਸ਼ੁਰੂ, int32_t ਹੱਦਬੰਦੀ,
                                      UnicodeString &sb) {
    int32_t doubleApos=-1;
    ਲਈ(;;) {
        int32_t i=s.ਸੂਚਕਾਂਕ_ਦਾ(u_apos, ਸ਼ੁਰੂ);
        ਜੇ(i<0 || i>=ਹੱਦਬੰਦੀ) {
            sb.append(s, ਸ਼ੁਰੂ, ਹੱਦਬੰਦੀ-ਸ਼ੁਰੂ);
            ਰੋਕੋ;
        }
        ਜੇ(i==doubleApos) {
            // Double apostrophe at start-1 and start==i, append one.
            sb.append(u_apos);
            ++ਸ਼ੁਰੂ;
            doubleApos=-1;
        } ਨਹੀਂਤਾਂ {
            // Append text between apostrophes and skip this one.
            sb.append(s, ਸ਼ੁਰੂ, i-ਸ਼ੁਰੂ);
            doubleApos=ਸ਼ੁਰੂ=i+1;
        }
    }
}

// Ported from second half of ICU4J SelectFormat.format(String).
UnicodeString&
ਸੁਨੇਹਾ_Impl::append_Sub_ਸੁਨੇਹਾ_Without_Skip_Syntax(ਅਚਲ ਸੁਨੇਹਾ_ਨਮੂਨਾ& ਸੁਨੇਹਾ_ਨਮੂਨਾ_2,
                                               int32_t ਸੁਨੇਹਾ_ਸ਼ੁਰੂ,
                                               UnicodeString& ਨਤੀਜਾ U_LIFETIME_BOUND) {
    ਅਚਲ UnicodeString &ਸੁਨੇਹਾ_String=ਸੁਨੇਹਾ_ਨਮੂਨਾ_2.get_ਨਮੂਨਾ_String();
    int32_t prev_ਸੂਚਕਾਂਕ=ਸੁਨੇਹਾ_ਨਮੂਨਾ_2.getPart(ਸੁਨੇਹਾ_ਸ਼ੁਰੂ).get_ਹੱਦਬੰਦੀ();
    ਲਈ(int32_t i=ਸੁਨੇਹਾ_ਸ਼ੁਰੂ;;) {
        ਅਚਲ ਸੁਨੇਹਾ_ਨਮੂਨਾ::Part &part=ਸੁਨੇਹਾ_ਨਮੂਨਾ_2.getPart(++i);
        UMessage_ਨਮੂਨਾ_Part_Type type=part.getType();
        int32_t ਸੂਚਕਾਂਕ=part.get_ਸੂਚਕਾਂਕ();
        ਜੇ(type==UMSGPAT_PART_TYPE_ਸੁਨੇਹਾ_ਹੱਦਬੰਦੀ) {
            ਵਾਪਸ ਨਤੀਜਾ.append(ਸੁਨੇਹਾ_String, prev_ਸੂਚਕਾਂਕ, ਸੂਚਕਾਂਕ-prev_ਸੂਚਕਾਂਕ);
        } ਨਹੀਂਤਾਂ ਜੇ(type==UMSGPAT_PART_TYPE_SKIP_SYNTAX) {
            ਨਤੀਜਾ.append(ਸੁਨੇਹਾ_String, prev_ਸੂਚਕਾਂਕ, ਸੂਚਕਾਂਕ-prev_ਸੂਚਕਾਂਕ);
            prev_ਸੂਚਕਾਂਕ=part.get_ਹੱਦਬੰਦੀ();
        } ਨਹੀਂਤਾਂ ਜੇ(type==UMSGPAT_PART_TYPE_ARG_ਸ਼ੁਰੂ) {
            ਨਤੀਜਾ.append(ਸੁਨੇਹਾ_String, prev_ਸੂਚਕਾਂਕ, ਸੂਚਕਾਂਕ-prev_ਸੂਚਕਾਂਕ);
            prev_ਸੂਚਕਾਂਕ=ਸੂਚਕਾਂਕ;
            i=ਸੁਨੇਹਾ_ਨਮੂਨਾ_2.get_ਹੱਦਬੰਦੀ_Part_ਸੂਚਕਾਂਕ(i);
            ਸੂਚਕਾਂਕ=ਸੁਨੇਹਾ_ਨਮੂਨਾ_2.getPart(i).get_ਹੱਦਬੰਦੀ();
            appendReducedApostrophes(ਸੁਨੇਹਾ_String, prev_ਸੂਚਕਾਂਕ, ਸੂਚਕਾਂਕ, ਨਤੀਜਾ);
            prev_ਸੂਚਕਾਂਕ=ਸੂਚਕਾਂਕ;
        }
    }
}

U_NAMESPACE_ਸਿਰਾ

#ਜੇ_ਅੰਤ  // !UCONFIG_NO_FORMATTING
